//  Copyright (C) 2010 Ultr@VNC Team Members. All Rights Reserved.
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/
//
////////////////////////////////////////////////////////////////////////////


// ScrBuffer implementation

#include "stdafx.h"

// Header

#include "vncDesktop.h"
#include "rfbMisc.h"

#include "vncBuffer.h"

vncBuffer::vncBuffer()
{
	CheckBufferFn.Init("checkbuffer",0,0,app,false);
	bufferIPC.Init("bufferIPC",sizeof(_sharedbuffermem),0,app,false);
	sharedbuffermem=(_sharedbuffermem *)bufferIPC.Getsharedmem();

	m_freemainbuff = FALSE;
	m_mainbuff = NULL;
	m_backbuff = NULL;
	m_cachebuff =NULL;
	m_ScaledBuff = NULL;

	m_backbuff_IPC = NULL;
	m_cachebuff_IPC = NULL;
	m_ScaledBuff_IPC = NULL;

	sharedbuffermem->m_use_cache = FALSE;
	sharedbuffermem->m_cursor_shape_cleared = FALSE;
	m_backbuffsize = 0;
	m_desktop=NULL;

	// Modif sf@2002 - Scaling
	
	sharedbuffermem->m_nScale = 1;
	m_ScaledSize = 0;

	m_nAccuracyDiv = 4;

	nRowIndex = 0;
	sharedbuffermem->m_cursorpending = false;
	sharedbuffermem->m_single_monitor=1;
	sharedbuffermem->m_multi_monitor=0;

	// sf@2005 - Grey Palette
	sharedbuffermem->m_fGreyPalette = false;
}

vncBuffer::~vncBuffer()
{
	if (m_freemainbuff) {
		if (m_mainbuff != NULL)
		{
			delete [] m_mainbuff;
			m_mainbuff = NULL;
		}
	}

	if (m_backbuff_IPC != NULL)
	{
		delete [] m_backbuff_IPC;
		m_backbuff_IPC = NULL;
		m_backbuff = NULL;
	}

	if (m_cachebuff_IPC != NULL)
	{
		delete [] m_cachebuff_IPC;
		m_cachebuff_IPC = NULL;
		m_cachebuff = NULL;
	}

	// Modif sf@2002 - Scaling
	if (m_ScaledBuff_IPC != NULL)
	{
		delete [] m_ScaledBuff_IPC;
		m_ScaledBuff_IPC = NULL;
		m_ScaledBuff = NULL;
	}
	m_ScaledSize = 0;


	m_backbuffsize = 0;
}

BOOL 
vncBuffer::SetDesktop(vncDesktop *desktop)
{
	// Called from vncdesktop startup
	// only possible on startup or reinitialization
	// access is block by m_desktop->m_update_lock
	m_desktop=desktop;
	return CheckBuffer();
}

BOOL
vncBuffer::CheckBuffer()
{
	// Get the screen format, in case it has changed
	m_desktop->FillDisplayInfo(&sharedbuffermem->m_scrinfo);
	sharedbuffermem->m_bytesPerRow = sharedbuffermem->m_scrinfo.framebufferWidth * sharedbuffermem->m_scrinfo.format.bitsPerPixel/8;
	// Check that the local format buffers are sufficient
	//if ((m_backbuffsize != m_desktop->ScreenBuffSize()))
	{
		vnclog.Print(LL_INTINFO, VNCLOG("request local buffer[%d]\n"), m_desktop->ScreenBuffSize());
		
		if (m_freemainbuff) {
			// Slow blits were enabled - free the slow blit buffer
			// Modif rdv@2002 - v1.1.x - Videodriver
			if (m_mainbuff != NULL)
			{
				delete [] m_mainbuff;
				m_mainbuff = NULL;
			}
		}

		m_mainbuff = (BYTE *)m_desktop->OptimisedBlitBuffer();
		if (m_mainbuff) {
			// Prevent us from freeing the DIBsection buffer
			m_freemainbuff = FALSE;
			vnclog.Print(LL_INTINFO, VNCLOG("fast blits detected - using DIBsection buffer\n"));
		} else {
			// Create our own buffer to copy blits through
			m_freemainbuff = TRUE;
			if ((m_mainbuff = new BYTE [m_desktop->ScreenBuffSize()]) == NULL)
			{
				vnclog.Print(LL_INTERR, VNCLOG("unable to allocate main buffer[%d]\n"), m_desktop->ScreenBuffSize());
				return FALSE;
			}
			memset(m_mainbuff, 0, m_desktop->ScreenBuffSize());
		}

		if (m_cachebuff_IPC != NULL)
		{
			delete [] m_cachebuff_IPC;
			m_cachebuff_IPC = NULL;
			m_cachebuff = NULL;
		}

		if (m_backbuff_IPC != NULL)
		    {
			    delete [] m_backbuff_IPC;
			    m_backbuff_IPC = NULL;
				 m_backbuff = NULL;
        		m_backbuffsize = 0;
		    }



        if (m_ScaledBuff_IPC != NULL)
	        {
		        delete [] m_ScaledBuff_IPC;
		        m_ScaledBuff_IPC = NULL;
				m_ScaledBuff = NULL;
           		m_ScaledSize = 0;
	        }




		CheckBufferFn.Call_Fnction(NULL,NULL);



		m_backbuff_IPC = new comm_serv;
		m_backbuff_IPC->Init("m_backbuff_IPC",m_desktop->ScreenBuffSize(),0,app,false);
		m_backbuff=(unsigned char*)m_backbuff_IPC->Getsharedmem();

		if (m_backbuff == NULL)
		    {
			    vnclog.Print(LL_INTERR, VNCLOG("unable to allocate back buffer[%d]\n"), m_desktop->ScreenBuffSize());
			    return FALSE;
		    }
    	m_backbuffsize = m_desktop->ScreenBuffSize();

		m_cachebuff_IPC = new comm_serv;
		m_cachebuff_IPC->Init("m_cachebuff_IPC",m_desktop->ScreenBuffSize(),0,app,false);
		m_cachebuff=(unsigned char*)m_cachebuff_IPC->Getsharedmem();

		if (m_cachebuff  == NULL)
			{
				vnclog.Print(LL_INTERR, VNCLOG("unable to allocate cache buffer[%d]\n"), m_desktop->ScreenBuffSize());
				return FALSE;
			}
			ClearCache();
		memset(m_backbuff, 0, m_desktop->ScreenBuffSize());
		
 
		m_ScaledBuff_IPC = new comm_serv;
		m_ScaledBuff_IPC->Init("m_ScaledBuff_IPC",m_desktop->ScreenBuffSize(),0,app,false);
		m_ScaledBuff=(unsigned char*)m_ScaledBuff_IPC->Getsharedmem();

                // Modif sf@2002 - Scaling
	    if (m_ScaledBuff == NULL)
			    {
				    vnclog.Print(LL_INTERR, VNCLOG("unable to allocate scaled buffer[%d]\n"), m_desktop->ScreenBuffSize());
				    return FALSE;
			    }
		m_ScaledSize = m_desktop->ScreenBuffSize();
		memcpy(m_ScaledBuff, m_mainbuff, m_desktop->ScreenBuffSize());

	}

	vnclog.Print(LL_INTINFO, VNCLOG("local buffer=%d\n"), m_backbuffsize);

	return TRUE;
}


// Modif sf@2002 - v1.1.0
//
// Set the accuracy divider factor 
// that is utilized in the GetChangedRegion function
// for changes detection in Rectangles.
// The higher the value the less accuracy.
// WARNING :this value must be a 32 divider.
//
BOOL vncBuffer::SetAccuracy(int nAccuracy)
{
	m_nAccuracyDiv = nAccuracy;
	nRowIndex = 0;

	return TRUE;
}



// Check a specified rectangle for changes and fill the region with
// the changed subrects
//#pragma function(memcpy,Save_memcmp)

const int BLOCK_SIZE = 32;
void vncBuffer::CheckRect(rfb::Region2D &dest, rfb::Region2D &cacheRgn, const rfb::Rect &srcrect)
{
	//only called from desktopthread
	if (!FastCheckMainbuffer())
		return;

	const UINT bytesPerPixel = sharedbuffermem->m_scrinfo.format.bitsPerPixel >> 3; // divide by 8

	rfb::Rect new_rect;
	rfb::Rect srect = srcrect;

	// Modif sf@2002 - Scaling
	rfb::Rect ScaledRect;
	ScaledRect.tl.y = srect.tl.y / sharedbuffermem->m_nScale;
	ScaledRect.br.y = srect.br.y / sharedbuffermem->m_nScale;
	ScaledRect.tl.x = srect.tl.x / sharedbuffermem->m_nScale;
	ScaledRect.br.x = srect.br.x / sharedbuffermem->m_nScale;

	int x, y;
	UINT ay, by;

	// DWORD align the incoming rectangle.  (bPP will be 8, 16 or 32)
	if (bytesPerPixel < 4) {
		if (bytesPerPixel == 1)				// 1 byte per pixel
			ScaledRect.tl.x -= (ScaledRect.tl.x & 3);	// round down to nearest multiple of 4
		else								// 2 bytes per pixel
			ScaledRect.tl.x -= (ScaledRect.tl.x & 1);	// round down to nearest multiple of 2
	}

	// Modif sf@2002 - Scaling
	// Problem, driver buffer is not writable
	// so we always need a m_scalednuff
	unsigned char *TheBuffer;
	TheBuffer = m_ScaledBuff;

	// sf@2004 - Optimization (attempt...)
	int nOptimizedBlockSize; 
	bool fSmallRect = false;
	if (ScaledRect.br.x - ScaledRect.tl.x < 16)
	{
		nOptimizedBlockSize = BLOCK_SIZE / 4;
		fSmallRect = true;
	}
	else if (ScaledRect.br.x - ScaledRect.tl.x < 32)
		nOptimizedBlockSize = BLOCK_SIZE / 2;
	else
		nOptimizedBlockSize = BLOCK_SIZE * 2;

	ptrdiff_t ptrBlockOffset = (ScaledRect.tl.y * sharedbuffermem->m_bytesPerRow) + (ScaledRect.tl.x * bytesPerPixel);

	if (sharedbuffermem->m_use_cache /*&& !m_desktop->m_UltraEncoder_used*/ && m_cachebuff)
	{
	// Scan down the rectangle
		unsigned char *o_topleft_ptr = m_backbuff  + ptrBlockOffset;
		unsigned char *c_topleft_ptr = m_cachebuff + ptrBlockOffset;
		unsigned char *n_topleft_ptr = TheBuffer   + ptrBlockOffset;
		int last_x, last_y;
		for (y = ScaledRect.tl.y, last_y = ScaledRect.br.y; y < last_y; y += nOptimizedBlockSize)
	{
		// Work out way down the bitmap
		unsigned char * o_row_ptr = o_topleft_ptr;
		unsigned char * n_row_ptr = n_topleft_ptr;
		unsigned char * c_row_ptr = c_topleft_ptr;

		const UINT blockbottom = min(y + nOptimizedBlockSize, ScaledRect.br.y);
			for (x = ScaledRect.tl.x, last_x = ScaledRect.br.x; x < last_x; x += nOptimizedBlockSize)
		{
			// Work our way across the row
			unsigned char *n_block_ptr = n_row_ptr;
			unsigned char *o_block_ptr = o_row_ptr;
			unsigned char *c_block_ptr = c_row_ptr;

			const UINT blockright = min(x + nOptimizedBlockSize, ScaledRect.br.x);
			const UINT bytesPerBlockRow = (blockright-x) * bytesPerPixel;

				bool fCache = true; // RDV@2002
			// Modif sf@2002 - v1.1.0
			// int nRowIndex = 0; // We don't reinit nRowIndex for each rect -> we don't always miss the same changed pixels
			int nOffset = (int)(bytesPerBlockRow / m_nAccuracyDiv);

			unsigned char *y_o_ptr = o_block_ptr;
			unsigned char *y_n_ptr = n_block_ptr;
			unsigned char *y_c_ptr = c_block_ptr;

			// Scan this block
			for (ay = y; ay < blockbottom; ay++)
			{
					int nBlockOffset =  nRowIndex * nOffset;
					if (memcmp(n_block_ptr + nBlockOffset, o_block_ptr + nBlockOffset, nOffset) != 0)
				{
					// A pixel has changed, so this block needs updating
					if (fSmallRect) // Ignore unchanged first rows (samll rect : less chance to miss changes when using nAccuracy div)
					{
						new_rect.tl.y = ay * sharedbuffermem->m_nScale;
						y_n_ptr += ((ay-y) *sharedbuffermem-> m_bytesPerRow);
						y_o_ptr += ((ay-y) *sharedbuffermem-> m_bytesPerRow);
					}
					else
					{
						new_rect.tl.y = y * sharedbuffermem->m_nScale;
					}
					new_rect.tl.x = x * sharedbuffermem->m_nScale;
					new_rect.br.x = blockright * sharedbuffermem->m_nScale;
					new_rect.br.y = blockbottom * sharedbuffermem->m_nScale;

					for (by = ((fSmallRect) ? ay : y); by < blockbottom; by++)
					{
						// Until then, new rect and cached rect are equal.
						// So test the next row
						// TODO: send the common part as cached rect and the rest as changed rect
						if (fCache)
							{
							if (memcmp(y_n_ptr, y_c_ptr, bytesPerBlockRow) != 0)
							{
									fCache = false; // New rect and cached rect differ
								}
							}
						memcpy(y_c_ptr, y_o_ptr, bytesPerBlockRow); // Save back buffer rect in cache
						memcpy(y_o_ptr, y_n_ptr, bytesPerBlockRow); // Save new rect in back buffer
						y_n_ptr += sharedbuffermem->m_bytesPerRow;
						y_o_ptr += sharedbuffermem->m_bytesPerRow;
						y_c_ptr += sharedbuffermem->m_bytesPerRow;
					}
					if (fCache) // The Rect was in the cache
					{
							cacheRgn.assign_union(rfb::Region2D(new_rect));
					}
					else // The Rect wasn't in the cache
					{
							dest.assign_union(new_rect);
					}

					break;
				}

				n_block_ptr += sharedbuffermem->m_bytesPerRow;
				o_block_ptr += sharedbuffermem->m_bytesPerRow;
				c_block_ptr += sharedbuffermem->m_bytesPerRow;

				nRowIndex = (nRowIndex + 1) % m_nAccuracyDiv; // sf@2002 - v1.1.0
			}

			o_row_ptr += bytesPerBlockRow;
			n_row_ptr += bytesPerBlockRow;
			c_row_ptr += bytesPerBlockRow;
		}

		o_topleft_ptr += sharedbuffermem->m_bytesPerRow * nOptimizedBlockSize;
		n_topleft_ptr += sharedbuffermem->m_bytesPerRow * nOptimizedBlockSize;
		c_topleft_ptr += sharedbuffermem->m_bytesPerRow * nOptimizedBlockSize;
	}
	}
	else
	{
	// Scan down the rectangle
		unsigned char *o_topleft_ptr = m_backbuff + ptrBlockOffset;
		unsigned char *n_topleft_ptr = TheBuffer  + ptrBlockOffset;
	for (y = ScaledRect.tl.y; y < ScaledRect.br.y; y += nOptimizedBlockSize)
	{
		// Work out way down the bitmap
		unsigned char * o_row_ptr = o_topleft_ptr;
		unsigned char * n_row_ptr = n_topleft_ptr;

		const UINT blockbottom = min(y + nOptimizedBlockSize, ScaledRect.br.y);
		for (x = ScaledRect.tl.x; x < ScaledRect.br.x; x += nOptimizedBlockSize)
		{
			// Work our way across the row
			unsigned char *n_block_ptr = n_row_ptr;
			unsigned char *o_block_ptr = o_row_ptr;

			const UINT blockright = min(x+nOptimizedBlockSize, ScaledRect.br.x);
			const UINT bytesPerBlockRow = (blockright-x) * bytesPerPixel;

			// Modif sf@2002 
			unsigned char *y_o_ptr = o_block_ptr;
			unsigned char *y_n_ptr = n_block_ptr;

			// int nRowIndex = 0;
			int nOffset = (int)(bytesPerBlockRow / m_nAccuracyDiv);

			// Scan this block
			for (ay = y; ay < blockbottom; ay++)
			{
					int nBlockOffset =  nRowIndex * nOffset;
					if (memcmp(n_block_ptr + nBlockOffset, o_block_ptr + nBlockOffset, nOffset) != 0)
				{
					// A pixel has changed, so this block needs updating
					if (fSmallRect) // Ignore unchanged first rows (samll rect : less chance to miss changes when using nAccuracy div)
					{
						new_rect.tl.y = ay * sharedbuffermem->m_nScale;
						y_n_ptr += ((ay-y) * sharedbuffermem->m_bytesPerRow);
						y_o_ptr += ((ay-y) * sharedbuffermem->m_bytesPerRow);
					}
					else
					{
						new_rect.tl.y = y * sharedbuffermem->m_nScale;
					}
					new_rect.tl.x = x * sharedbuffermem->m_nScale;
					new_rect.br.x = (blockright * sharedbuffermem->m_nScale);
					new_rect.br.y = (blockbottom * sharedbuffermem->m_nScale);
						dest.assign_union(new_rect);

					// Copy the changes to the back buffer
					for (by = ((fSmallRect) ? ay : y); by < blockbottom; by++)
					{
						memcpy(y_o_ptr, y_n_ptr, bytesPerBlockRow);
						y_n_ptr += sharedbuffermem->m_bytesPerRow;
						y_o_ptr += sharedbuffermem->m_bytesPerRow;
					}

					break;
				}
				// Last Run Ptr gets over de bounding
				// Boundcheker keeps complaining
				if (ay!=blockbottom-1)
				{
					n_block_ptr += sharedbuffermem->m_bytesPerRow;
					o_block_ptr += sharedbuffermem->m_bytesPerRow;
					nRowIndex = (nRowIndex + 1) % m_nAccuracyDiv;
				}
			}
			if (x != ScaledRect.br.x-1)
			{
				o_row_ptr += bytesPerBlockRow;
				n_row_ptr += bytesPerBlockRow;
			}
		}
		if (y != ScaledRect.br.y-1)
		{
			o_topleft_ptr += sharedbuffermem->m_bytesPerRow * nOptimizedBlockSize;
			n_topleft_ptr += sharedbuffermem->m_bytesPerRow * nOptimizedBlockSize;
		}
	}
	}
}


//rdv modif scaled and videodriver
void
vncBuffer::GrabRegion(rfb::Region2D &src,BOOL driver,BOOL capture)
{
	rfb::RectVector rects;
	rfb::RectVector::iterator i;
	rfb::Rect grabRect;

	//
	// - Are there any rectangles to check?
	//
	src.get_rects(rects, 1, 1);
	if (rects.empty()) return;

	//
	// - Grab the rectangles that may have changed
	//
	//
	//************ Added extra cliprect check
	int x,y,w,h;
	for (i = rects.begin(); i != rects.end(); ++i)
	{
		rfb::Rect rect = *i;
		x=rect.tl.x;
		y=rect.tl.y;
		w=rect.br.x-rect.tl.x;
		h=rect.br.y-rect.tl.y;
		src=src.subtract(rect);
		if (ClipRect(&x, &y, &w, &h, m_desktop->m_Cliprect.tl.x, m_desktop->m_Cliprect.tl.y,
				m_desktop->m_Cliprect.br.x-m_desktop->m_Cliprect.tl.x, m_desktop->m_Cliprect.br.y-m_desktop->m_Cliprect.tl.y))
			{
				rect.tl.x=x;
				rect.tl.y=y;
				rect.br.x=x+w;
				rect.br.y=y+h;
				src=src.union_(rect);
			}
	}
	src.get_rects(rects, 1, 1);
	if (rects.empty()) 
		{
			return;
		}

	// The rectangles should have arrived in order of height
	for (i = rects.begin(); i != rects.end(); i++)
	{
		rfb::Rect current = *i;


		// Check that this rectangle is part of this capture region
		if (current.tl.y > grabRect.br.y)
		{
			// If the existing rect is non-null the capture it
			if (!grabRect.is_empty()) GrabRect(grabRect,driver,capture);

			grabRect = current;
		} else {
			grabRect = current.union_boundary(grabRect);
		}
	}

	// If there are still some rects to be done then do them
	if (!grabRect.is_empty()) GrabRect(grabRect,driver,capture);
}

void
vncBuffer::CheckRegion(rfb::Region2D &dest,rfb::Region2D &cacheRgn ,const rfb::Region2D &src)
{
	rfb::RectVector rects;
	rfb::RectVector::iterator i;

	// If there is nothing to do then do nothing...
	src.get_rects(rects, 1, 1);
	if (rects.empty()) return;

	//
	// - Scan the specified rectangles for changes
	//
	// Block desactivation mainbuff while running
	// No Lock Needed, only called from vncdesktopthread
	for (i = rects.begin(); i != rects.end(); ++i)
	{
		CheckRect(dest,cacheRgn, *i);
	}
}



// Reduce possible colors to 8 shades of gray
int To8GreyColors(int r, int g, int b)
{
    int Value;
    Value = (r*11 + g*16 +b*5 ) / 32;
	
	if (Value <= 0x20)
		return 0x101010;
	else if (Value <= 0x40)
		return 0x303030;
	else if (Value <= 0x60)
		return 0x505050;
	else if (Value <= 0x80)
		return 0x707070;
	else if (Value <= 0xa0)
		return 0x909090;
	else if (Value <= 0xc0)
		return 0xb0b0b0;
	else if (Value <= 0xe0)
		return 0xd0d0d0;
	else
		return 0xf0f0f0;  
}
 

// Modif sf@2002 - Scaling
void vncBuffer::ScaleRect(rfb::Rect &rect)
{
	//rdv 2010  overhead
	// This function is always called, also when scaling=1....
	if (sharedbuffermem->m_nScale==1)
	{
		BYTE *pMain   =	m_mainbuff + (rect.tl.y * sharedbuffermem->m_bytesPerRow) + 
						(rect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
		BYTE *pScaled = m_ScaledBuff + (rect.tl.y * sharedbuffermem->m_bytesPerRow) +
						(rect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
		const UINT bytesPerLine = (rect.br.x-rect.tl.x)*(sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

		for (int y = rect.tl.y; y < rect.br.y; y++)
			{
				memcpy(pScaled, pMain, bytesPerLine);
				pMain   += sharedbuffermem->m_bytesPerRow; // Skip m_nScale raws of the mainbuffer's Rect
				pScaled += sharedbuffermem->m_bytesPerRow; 
			}
	}
	else
	{

		bool fCanReduceColors = true;

		//JK 26th Jan, 2005: Color conversion to 8 shades of gray added,
		//at the moment only works if server has 24/32-bit color

		// this construct is faster than the old method -- it has no jumps, so there's no chance of
		// branch mispredictions, and it generates better asm code .
		fCanReduceColors = (((sharedbuffermem->m_scrinfo.format.redMax ^ sharedbuffermem->m_scrinfo.format.blueMax ^ 
							sharedbuffermem->m_scrinfo.format.greenMax ^ 0xff) == 0)  && sharedbuffermem->m_fGreyPalette);

	#if 0
		if ((m_scrinfo.format.redMax == 255) 
			&& 
			(m_scrinfo.format.blueMax == 255)
			&&
			(m_scrinfo.format.greenMax == 255)
			&&
			m_fGreyPalette
			)
			fCanReduceColors = true;
		else
			fCanReduceColors = false;
	#endif
		//End JK

		// called from grabrect and grabmouse
		// grabmouse is only called from desktopthread, no lock needed
		// grebrect called from grabregion called from desktopthread, no lock needed
		// no lock needed
		if (!FastCheckMainbuffer())
			return;
		///////////
		rect.tl.y = (rect.tl.y - (rect.tl.y % sharedbuffermem->m_nScale));
		rect.br.y = (rect.br.y - (rect.br.y % sharedbuffermem->m_nScale)) + sharedbuffermem->m_nScale - 1;
		rect.tl.x = (rect.tl.x - (rect.tl.x % sharedbuffermem->m_nScale));
		rect.br.x = (rect.br.x - (rect.br.x % sharedbuffermem->m_nScale)) + sharedbuffermem->m_nScale - 1;

		rfb::Rect ScaledRect;
		ScaledRect.tl.y = rect.tl.y / sharedbuffermem->m_nScale;
		ScaledRect.br.y = rect.br.y / sharedbuffermem->m_nScale;
		ScaledRect.tl.x = rect.tl.x / sharedbuffermem->m_nScale;
		ScaledRect.br.x = rect.br.x / sharedbuffermem->m_nScale;

		// Copy and scale data from screen Main buffer to Scaled buffer
		BYTE *pMain   =	m_mainbuff + (rect.tl.y * sharedbuffermem->m_bytesPerRow) + 
						(rect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
		BYTE *pScaled = m_ScaledBuff + (ScaledRect.tl.y * sharedbuffermem->m_bytesPerRow) +
						(ScaledRect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);

		// We keep one pixel for m_Scale*m_nScale pixels in the Main buffer
		// and copy it in the Scaled Buffer

		// TrueColor Case.
		// Pixels Blending (takes the "medium" pixel of each m_Scale*m_nScale square)
		// This TrueColor Pixel blending routine comes from the Harakan's WinVNC with Server Side Scaling
		// Extension. It replaces my own buggy Blending function that given *ugly* results.
		if (sharedbuffermem->m_scrinfo.format.trueColour)
		{
			unsigned long lRed;
			unsigned long lGreen;
			unsigned long lBlue;
			unsigned long lScaledPixel;
			UINT nBytesPerPixel = (sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);

			// For each line of the Destination ScaledRect
			for (int y = ScaledRect.tl.y; y < ScaledRect.br.y; y++)
			{
				// For each Pixel of the line 
				for (int x = 0; x < (ScaledRect.br.x - ScaledRect.tl.x); x++)
				{
					lRed   = 0; 
					lGreen = 0;
					lBlue  = 0;
					// Take a m_Scale*m_nScale square of pixels in the Main Buffer
					// and get the global Red, Green, Blue values for this square
					for (UINT r = 0; r < sharedbuffermem->m_nScale; r++)
					{
						for (UINT c = 0; c < sharedbuffermem->m_nScale; c++)
						{
							lScaledPixel = 0;
							for (UINT b = 0; b < nBytesPerPixel; b++)
							{
								lScaledPixel += (pMain[(((x * sharedbuffermem->m_nScale) + c) * nBytesPerPixel) + (r * sharedbuffermem->m_bytesPerRow) + b]) << (8 * b);
							}
							lRed   += (lScaledPixel >> sharedbuffermem->m_scrinfo.format.redShift) & sharedbuffermem->m_scrinfo.format.redMax;
							lGreen += (lScaledPixel >> sharedbuffermem->m_scrinfo.format.greenShift) & sharedbuffermem->m_scrinfo.format.greenMax;
							lBlue  += (lScaledPixel >> sharedbuffermem->m_scrinfo.format.blueShift) & sharedbuffermem->m_scrinfo.format.blueMax;
						}
					}
					// Get the medium R,G,B values for the sqare
					lRed   /= sharedbuffermem->m_nScale * sharedbuffermem->m_nScale;
					lGreen /= sharedbuffermem->m_nScale * sharedbuffermem->m_nScale;
					lBlue  /= sharedbuffermem->m_nScale *sharedbuffermem-> m_nScale;
					// Calculate the resulting "medium" pixel
					// lScaledPixel = (lRed << m_scrinfo.format.redShift) + (lGreen << m_scrinfo.format.greenShift) + (lBlue << m_scrinfo.format.blueShift);
					// JK 26th Jan, 2005: Reduce possible colors to 8 shades of gray
					if (fCanReduceColors)
					{
						lScaledPixel = To8GreyColors(lRed, lGreen, lBlue);
					}
					else
					{
						// Calculate the resulting "medium" pixel
						lScaledPixel = (lRed << sharedbuffermem->m_scrinfo.format.redShift) + (lGreen << sharedbuffermem->m_scrinfo.format.greenShift) + (lBlue << sharedbuffermem->m_scrinfo.format.blueShift);
					} 
				
					// Copy the resulting pixel in the Scaled Buffer
					for (UINT b = 0; b < nBytesPerPixel; b++)
					{
						pScaled[(x * nBytesPerPixel) + b] = (lScaledPixel >> (8 * b)) & 0xFF;
					}
				}
				// Move the buffers' pointers to their next "line"
				pMain   += (sharedbuffermem->m_bytesPerRow * sharedbuffermem->m_nScale); // Skip m_nScale raws of the mainbuffer's Rect
				pScaled += sharedbuffermem->m_bytesPerRow; 
			}
		}
		// Keep only the topleft pixel of each MainBuffer's m_Scale*m_nScale block
		// Very incurate method...but bearable result in 256 and 16bit colors modes.
		else 
		{
			UINT nBytesPerPixel = (sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
			for (int y = ScaledRect.tl.y; y < ScaledRect.br.y; y++)
			{
				for (int x = 0; x < (ScaledRect.br.x - ScaledRect.tl.x); x++)
				{
					memcpy(&pScaled[x * nBytesPerPixel], &pMain[x * sharedbuffermem->m_nScale * nBytesPerPixel], nBytesPerPixel);
				}
				// Move the buffers' pointers to their next "line"
				pMain   += (sharedbuffermem->m_bytesPerRow * sharedbuffermem->m_nScale); // Skip m_nScale lines of the mainbuffer's Rect
				pScaled += sharedbuffermem->m_bytesPerRow;
			}
		}
	}
}


// Modif sf@2005 - Grey Scale transformation
// Ok, it's a little wild to do it here... should be done in Translate.cpp,..
void vncBuffer::GreyScaleRect(rfb::Rect &rect)
{
	bool fCanReduceColors = true;
    fCanReduceColors = (((sharedbuffermem->m_scrinfo.format.redMax ^ sharedbuffermem->m_scrinfo.format.blueMax ^ 
                        sharedbuffermem->m_scrinfo.format.greenMax ^ 0xff) == 0)  && sharedbuffermem->m_fGreyPalette);
#if 0
    //JK 26th Jan, 2005: Color conversion to 8 shades of gray added,
	//at the moment only works if server has 24/32-bit color
	if ((m_scrinfo.format.redMax == 255) 
		&& 
		(m_scrinfo.format.blueMax == 255)
		&&
		(m_scrinfo.format.greenMax == 255)
		&&
		m_fGreyPalette
		)
		fCanReduceColors = true;
	else
		fCanReduceColors = false;
	//End JK
#endif
	if (!fCanReduceColors) return;
	if (!FastCheckMainbuffer())
		return;
	///////////
	rect.tl.y = (rect.tl.y - (rect.tl.y % sharedbuffermem->m_nScale));
	rect.br.y = (rect.br.y - (rect.br.y % sharedbuffermem->m_nScale)) + sharedbuffermem->m_nScale - 1;
	rect.tl.x = (rect.tl.x - (rect.tl.x % sharedbuffermem->m_nScale));
	rect.br.x = (rect.br.x - (rect.br.x % sharedbuffermem->m_nScale)) + sharedbuffermem->m_nScale - 1;

	rfb::Rect ScaledRect;
	ScaledRect.tl.y = rect.tl.y / sharedbuffermem->m_nScale;
	ScaledRect.br.y = rect.br.y / sharedbuffermem->m_nScale;
	ScaledRect.tl.x = rect.tl.x / sharedbuffermem->m_nScale;
	ScaledRect.br.x = rect.br.x / sharedbuffermem->m_nScale;

	// Copy and scale data from screen Main buffer to Scaled buffer
	BYTE *pMain   =	m_mainbuff + (rect.tl.y * sharedbuffermem->m_bytesPerRow) + 
					(rect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
	BYTE *pScaled = m_ScaledBuff + (ScaledRect.tl.y * sharedbuffermem->m_bytesPerRow) +
					(ScaledRect.tl.x * sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);
	
	UINT nBytesPerPixel = (sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8);

	unsigned long lRed;
	unsigned long lGreen;
	unsigned long lBlue;
	unsigned long lPixel;

		for (int y = ScaledRect.tl.y; y < ScaledRect.br.y; y++)
		{
			for (int x = 0; x < (ScaledRect.br.x - ScaledRect.tl.x); x++)
			{
				lPixel = 0;
			for (UINT b = 0; b < nBytesPerPixel; b++)
				lPixel += ((pMain[(x * nBytesPerPixel) + b]) << (8 * b));

			lRed   = (lPixel >> sharedbuffermem->m_scrinfo.format.redShift) & sharedbuffermem->m_scrinfo.format.redMax;
			lGreen = (lPixel >> sharedbuffermem->m_scrinfo.format.greenShift) & sharedbuffermem->m_scrinfo.format.greenMax;
			lBlue  = (lPixel >> sharedbuffermem->m_scrinfo.format.blueShift) & sharedbuffermem->m_scrinfo.format.blueMax;
			lPixel = To8GreyColors(lRed, lGreen, lBlue);
			
			for (UINT bb = 0; bb < nBytesPerPixel; bb++)
				pScaled[(x * nBytesPerPixel) + bb] = (lPixel >> (8 * bb)) & 0xFF;
			}
			// Move the buffers' pointers to their next "line"
			pMain   += (sharedbuffermem->m_bytesPerRow * sharedbuffermem->m_nScale); // Skip m_nScale lines of the mainbuffer's Rect
			pScaled += sharedbuffermem->m_bytesPerRow;
		}
}


void
vncBuffer::GrabRect(const rfb::Rect &rect,BOOL driver,BOOL capture)
{
	//no lock needed
	if (!FastCheckMainbuffer()) return;

	if (!driver) m_desktop->CaptureScreen(rect, m_mainbuff, m_backbuffsize,capture);

	// Modif sf@2002 - Scaling
	// Only use scaledbuffer if necessary !
	rfb::Rect TheRect = rect;
	// if (m_nScale > 1) ScaleRect(rfb::Rect(rect.tl.x, rect.tl.y, rect.br.x, rect.br.y)); // sf@2002 - Waste of time !!!
	// Problem, driver buffer is not writable
	// so we always need a m_scalednuff
	if (sharedbuffermem->m_fGreyPalette)GreyScaleRect(TheRect);
	else ScaleRect(TheRect);

}


void
vncBuffer::CopyRect(const rfb::Rect &dest, const rfb::Point &delta)
{
	rfb::Rect ScaledDest;
	rfb::Rect ScaledSource;

	rfb::Rect src = dest.translate(delta.negate());

	// Modif sf@2002 - Scaling
	ScaledDest.tl.y = dest.tl.y / sharedbuffermem->m_nScale;
	ScaledDest.br.y = dest.br.y / sharedbuffermem->m_nScale;
	ScaledDest.tl.x = dest.tl.x / sharedbuffermem->m_nScale;
	ScaledDest.br.x = dest.br.x / sharedbuffermem->m_nScale;
	ScaledSource.tl.y = src.tl.y / sharedbuffermem->m_nScale;
	ScaledSource.br.y = src.br.y / sharedbuffermem->m_nScale;
	ScaledSource.tl.x = src.tl.x / sharedbuffermem->m_nScale;
	ScaledSource.br.x = src.br.x / sharedbuffermem->m_nScale;

	ClearCacheRect(ScaledSource);
	ClearCacheRect(ScaledDest);


	// Copy the data from one part of the back-buffer to another!
	const UINT bytesPerPixel = sharedbuffermem->m_scrinfo.format.bitsPerPixel / 8;
	const UINT bytesPerLine = (ScaledDest.br.x - ScaledDest.tl.x) * bytesPerPixel;
	BYTE *srcptr = m_backbuff + (ScaledSource.tl.y * sharedbuffermem->m_bytesPerRow) +
					(ScaledSource.tl.x * bytesPerPixel);

	BYTE *destptr = m_backbuff + (ScaledDest.tl.y * sharedbuffermem->m_bytesPerRow) +
					(ScaledDest.tl.x * bytesPerPixel);
	// Copy the data around in the right order
	if (ScaledDest.tl.y < ScaledSource.tl.y)
	{
		for (int y = ScaledDest.tl.y; y < ScaledDest.br.y; y++)
		{
			memmove(destptr, srcptr, bytesPerLine);
			srcptr += sharedbuffermem->m_bytesPerRow;
			destptr += sharedbuffermem->m_bytesPerRow;
		}
	}
	else
	{
		srcptr += (sharedbuffermem->m_bytesPerRow * ((ScaledDest.br.y - ScaledDest.tl.y)-1));
		destptr += (sharedbuffermem->m_bytesPerRow * ((ScaledDest.br.y - ScaledDest.tl.y)-1));
		for (int y = ScaledDest.br.y; y > ScaledDest.tl.y; y--)
		{
			memmove(destptr, srcptr, bytesPerLine);
			srcptr -= sharedbuffermem->m_bytesPerRow;
			destptr -= sharedbuffermem->m_bytesPerRow;
		}
	}
}


void
vncBuffer::ClearCache()
{
	sharedbuffermem->m_cursor_shape_cleared=TRUE;
	if (sharedbuffermem->m_use_cache && m_cachebuff)
	{
	RECT dest;
	dest.left=0;
	dest.top=0;
	dest.right=sharedbuffermem->m_scrinfo.framebufferWidth;
	dest.bottom=sharedbuffermem->m_scrinfo.framebufferHeight;
	// Modif sf@2002 - v1.1.0 - Scramble the cache
	int nValue = 0;
	BYTE *cacheptr = m_cachebuff + (dest.top * sharedbuffermem->m_bytesPerRow) +
		(dest.left * sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

	const UINT bytesPerLine = (dest.right-dest.left)*(sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

		for (int y=dest.top; y < dest.bottom; y++)
		{
			memset(cacheptr, nValue++, bytesPerLine);
			cacheptr+=sharedbuffermem->m_bytesPerRow;
			if (nValue == 255) nValue = 0; 
		}
	}
}

void
vncBuffer::ClearCacheRect(const rfb::Rect &dest)
{
	if (sharedbuffermem->m_use_cache && m_cachebuff)
	{
	int nValue = 0;
	BYTE *cacheptr = m_cachebuff + (dest.tl.y * sharedbuffermem->m_bytesPerRow) +
		(dest.tl.x *sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

	const UINT bytesPerLine = (dest.br.x-dest.tl.x)*(sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

		for (int y=dest.tl.y; y < dest.br.y; y++)
		{
			memset(cacheptr, nValue++, bytesPerLine);
			cacheptr+=sharedbuffermem->m_bytesPerRow;
			if (nValue == 255) nValue = 0; 
		}
	}
}

//***************************
// This is used on SW switch
// if backbuffer is not cleared
// the checker marks the rects
// as already send
//***************************
void
vncBuffer::BlackBack()
{
	RECT dest;
	dest.left=0;
	dest.top=0;
	dest.right=sharedbuffermem->m_scrinfo.framebufferWidth;
	dest.bottom=sharedbuffermem->m_scrinfo.framebufferHeight;
	int nValue = 1;
	BYTE *backptr = m_backbuff + (dest.top * sharedbuffermem->m_bytesPerRow) +
		(dest.left * sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

	const UINT bytesPerLine = (dest.right-dest.left)*(sharedbuffermem->m_scrinfo.format.bitsPerPixel/8);

		for (int y=dest.top; y < dest.bottom; y++)
		{
			memset(backptr, nValue, bytesPerLine);
			backptr+=sharedbuffermem->m_bytesPerRow;
			if (nValue == 255) nValue = 0; 
		}
}

void
vncBuffer::GrabMouse()
{
	//no lock needed
	if (!FastCheckMainbuffer()) return;
	m_desktop->CaptureMouse(m_mainbuff, m_backbuffsize);

	// Modif sf@2002 - Scaling
	rfb::Rect rect = m_desktop->MouseRect();
	
	// Problem, driver buffer is not writable
		// so we always need a m_scalednuff
		ScaleRect(rect);
}

// Verify that the fast blit buffer hasn't changed
inline BOOL
vncBuffer::FastCheckMainbuffer() {
	// Modif rdv@2002 - v1.1.x - videodriver
	VOID *tmp = m_desktop->OptimisedBlitBuffer();
	if (tmp && (m_mainbuff != tmp))
		{
			BOOL result=CheckBuffer();
			return result;
		}
	return TRUE;
}


BOOL
vncBuffer::IsShapeCleared()
{
	BOOL value=sharedbuffermem->m_cursor_shape_cleared;
	sharedbuffermem->m_cursor_shape_cleared=FALSE;
	return value;
}

void
vncBuffer::MultiMonitors(int number)
{
	if (number==1) {sharedbuffermem->m_single_monitor=true;sharedbuffermem->m_multi_monitor=false;}
	if (number==2) {sharedbuffermem->m_single_monitor=false;sharedbuffermem->m_multi_monitor=true;}
}

bool
vncBuffer::IsMultiMonitor()
{
	if (sharedbuffermem->m_single_monitor) return false;//we need at least 1 display
	else return true;
}


				   
bool
vncBuffer::ClipRect(int *x, int *y, int *w, int *h,
	    int cx, int cy, int cw, int ch) {
  if (*x < cx) {
    *w -= (cx-*x);
    *x = cx;
  }
  if (*y < cy) {
    *h -= (cy-*y);
    *y = cy;
  }
  if (*x+*w > cx+cw) {
    *w = (cx+cw)-*x;
  }
  if (*y+*h > cy+ch) {
    *h = (cy+ch)-*y;
  }
  return (*w>0) && (*h>0);
}
