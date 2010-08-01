//  Copyright (C) 2002 Ultr@VNC Team Members. All Rights Reserved.
//  Copyright (C) 2000-2002 Const Kaplinsky. All Rights Reserved.
//  Copyright (C) 2002 RealVNC Ltd. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
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
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// ScrBuffer implementation

#include "stdhdrs.h"

// Header

#include "remoteDesktop.h"
#include "rfbMisc.h"

#include "vncBuffer.h"
extern BOOL	fRunningFromExternalService;

vncBuffer::vncBuffer()
{
	bufferIPC.Init("bufferIPC",sizeof(_sharedbuffermem),0,!fRunningFromExternalService,true);
	sharedbuffermem=(_sharedbuffermem *)bufferIPC.Getsharedmem();

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

	sharedbuffermem->m_cursorpending = false;
	sharedbuffermem->m_single_monitor=1;
	sharedbuffermem->m_multi_monitor=0;

	// sf@2005 - Grey Palette
	sharedbuffermem->m_fGreyPalette = false;
}

vncBuffer::~vncBuffer()
{

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

rfb::Rect
vncBuffer::GetSize()
{
try
	{
		return m_desktop->GetSize();
	}
	catch (...)
	{	
		//return rfb::Rect(0, 0, m_scrinfo.framebufferWidth, m_scrinfo.framebufferHeight);
		return rfb::Rect(0, 0, 0, 0);
	}

}

// Modif sf@2002 - Scaling
UINT vncBuffer::GetScale()
{
	return sharedbuffermem->m_nScale;
}


BOOL vncBuffer::SetScale(int nScale)
{
	//called by
	//vncClientThread::run(void *arg) Lock Added
	// case rfbSetScale:  Lock Added OK

	sharedbuffermem->m_nScale = nScale;

	// Problem, driver buffer is not writable
	// so we always need a m_scalednuff
	{
		// sf@2002 - Idealy, we must do a ScaleRect of the whole screen here.
		// ScaleRect(rfb::Rect(0, 0, m_scrinfo.framebufferWidth / m_nScale, m_scrinfo.framebufferHeight / m_nScale));
		if (!CheckBuffer())//added to create scaled buffer
            return FALSE;
		ZeroMemory(m_ScaledBuff, m_desktop->ScreenBuffSize());
		ZeroMemory(m_backbuff, m_desktop->ScreenBuffSize());
	}
	
	return TRUE;
}


rfb::Rect vncBuffer::GetViewerSize()
{
	rfb::Rect rect;
	rect=m_desktop->GetSize();
	return rfb::Rect(rect.tl.x,rect.tl.y, rect.br.x / sharedbuffermem->m_nScale, rect.br.y / sharedbuffermem->m_nScale);
}


rfbPixelFormat
vncBuffer::GetLocalFormat()
{
	return sharedbuffermem->m_scrinfo.format;
}

BOOL
vncBuffer::CheckBuffer()
{
	// Get the screen format, in case it has changed
	m_desktop->FillDisplayInfo(&sharedbuffermem->m_scrinfo);
	sharedbuffermem->m_bytesPerRow = sharedbuffermem->m_scrinfo.framebufferWidth * sharedbuffermem->m_scrinfo.format.bitsPerPixel/8;

	// Check that the local format buffers are sufficient
	if ((m_backbuffsize != m_desktop->ScreenBuffSize()))
	{
		vnclog.Print(LL_INTINFO, VNCLOG("request local buffer[%d]\n"), m_desktop->ScreenBuffSize());

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

		m_backbuff_IPC = new comm_serv;
		m_backbuff_IPC->Init("m_backbuff_IPC",m_desktop->ScreenBuffSize(),0,!fRunningFromExternalService,true);
		m_backbuff=(unsigned char*)m_backbuff_IPC->Getsharedmem();

		if (m_backbuff == NULL)
		    {
			    vnclog.Print(LL_INTERR, VNCLOG("unable to allocate back buffer[%d]\n"), m_desktop->ScreenBuffSize());
			    return FALSE;
		    }
    	m_backbuffsize = m_desktop->ScreenBuffSize();



		m_cachebuff_IPC = new comm_serv;
		m_cachebuff_IPC->Init("m_cachebuff_IPC",m_desktop->ScreenBuffSize(),0,!fRunningFromExternalService,true);
		m_cachebuff=(unsigned char*)m_cachebuff_IPC->Getsharedmem();

		if (m_cachebuff  == NULL)
			{
				vnclog.Print(LL_INTERR, VNCLOG("unable to allocate cache buffer[%d]\n"), m_desktop->ScreenBuffSize());
				return FALSE;
			}
		ClearCache();
		memset(m_backbuff, 0, m_desktop->ScreenBuffSize());

		// Problem, driver buffer is not writable
		// so we always need a m_scalednuff
		/// If scale==1 we don't need to allocate the memory
		

		m_ScaledBuff_IPC = new comm_serv;
		m_ScaledBuff_IPC->Init("m_ScaledBuff_IPC",m_desktop->ScreenBuffSize(),0,!fRunningFromExternalService,true);
		m_ScaledBuff=(unsigned char*)m_ScaledBuff_IPC->Getsharedmem();

                // Modif sf@2002 - Scaling
	    if (m_ScaledBuff == NULL)
			    {
				    vnclog.Print(LL_INTERR, VNCLOG("unable to allocate scaled buffer[%d]\n"), m_desktop->ScreenBuffSize());
				    return FALSE;
			    }
	    m_ScaledSize = m_desktop->ScreenBuffSize();

	}

	vnclog.Print(LL_INTINFO, VNCLOG("local buffer=%d\n"), m_backbuffsize);

	return TRUE;
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
vncBuffer::ClearBack()
{
	if (m_ScaledBuff) memcpy(m_backbuff, m_ScaledBuff, m_desktop->ScreenBuffSize());
}

void
vncBuffer::GetMousePos(rfb::Rect &rect)
{
	rect = m_desktop->MouseRect();
}

// sf@2005
void vncBuffer::EnableGreyPalette(BOOL enable)
{
	sharedbuffermem->m_fGreyPalette = enable;
}

// RDV
void
vncBuffer::EnableCache(BOOL enable)
{
	sharedbuffermem->m_use_cache = enable;
	/*if (sharedbuffermem->m_use_cache)
	{
		if (m_cachebuff != NULL)
		{
			delete [] m_cachebuff;
			m_cachebuff = NULL;
		}
		if ((m_cachebuff = new BYTE [m_desktop->ScreenBuffSize()]) == NULL)
		{
			vnclog.Print(LL_INTERR, VNCLOG("unable to allocate cache buffer[%d]\n"), m_desktop->ScreenBuffSize());
			return;
		}
		ClearCache();
		// BlackBack();
	}*/
}

void
vncBuffer::MultiMonitors(int number)
{
	if (number==1) {sharedbuffermem->m_single_monitor=true;sharedbuffermem->m_multi_monitor=false;}
	if (number==2) {sharedbuffermem->m_single_monitor=false;sharedbuffermem->m_multi_monitor=true;}
}

