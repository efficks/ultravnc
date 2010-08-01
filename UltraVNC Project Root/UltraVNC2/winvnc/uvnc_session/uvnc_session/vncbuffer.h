//  Copyright (C) 2002 Ultr@VNC Team Members. All Rights Reserved.
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

// vncBuffer object

// The vncBuffer object provides a client-local copy of the screen
// It can tell the client which bits have changed in a given region
// It uses the specified vncDesktop to read screen data from
class vncDesktop;

class vncBuffer;

#if !defined(_WINVNC_VNCBUFFER)
#define _WINVNC_VNCBUFFER
#pragma once


// Includes

#include "stdafx.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "rfb.h"
#include "communication.h"


// Class definition
struct _sharedbuffermem
{
	rfbServerInitMsg	 m_scrinfo;
	BOOL				m_cursorpending;
	UINT				m_nScale;	
	BOOL				m_use_cache;
	UINT				m_bytesPerRow;
	BOOL				m_single_monitor;
	BOOL				m_multi_monitor;
	BOOL 				m_fGreyPalette;
	BOOL				m_cursor_shape_cleared;
};

class vncBuffer
{
// Methods
public:
	// Create/Destroy methods
	vncBuffer();
	~vncBuffer();	
	bool  IsMultiMonitor();
	BOOL SetAccuracy(int accuracy);
	void GrabRegion(rfb::Region2D &src,BOOL driver,BOOL capture);
	void CheckRegion(rfb::Region2D &dest,rfb::Region2D &cache, const rfb::Region2D &src);
	BOOL IsShapeCleared();	
	void ClearCacheRect(const rfb::Rect &dest);
	void CopyRect(const rfb::Rect &dest, const rfb::Point &delta);	
	void GrabMouse();
	void SetCursorPending(BOOL enable){sharedbuffermem->m_cursorpending=enable;};
	BOOL SetDesktop(vncDesktop *desktop);
	
	void ClearCache();
	void BlackBack();
	BOOL IsCursorUpdatePending(){return sharedbuffermem->m_cursorpending;};
	BOOL CheckBuffer();
	void MultiMonitors(int number);
	_sharedbuffermem *sharedbuffermem;
	comm_serv bufferIPC;
	comm_serv CheckBufferFn;
	BOOL		m_freemainbuff;


private:

	BYTE			*m_backbuff;
	comm_serv		*m_backbuff_IPC;

	BYTE			*m_cachebuff;
	comm_serv		*m_cachebuff_IPC;

	BYTE			*m_ScaledBuff;
	comm_serv		*m_ScaledBuff_IPC;

	BYTE			*m_mainbuff;

	UINT				m_backbuffsize;
	UINT				m_ScaledSize;

	vncDesktop			*m_desktop;		
	int					m_nAccuracyDiv; // Accuracy divider for changes detection in Rects
	int					nRowIndex;
	
	
	

	
	

	bool 		ClipRect(int *x, int *y, int *w, int *h, int cx, int cy, int cw, int ch);
	void 		CheckRect(rfb::Region2D &dest,rfb::Region2D &cache, const rfb::Rect &src);
	void 		GrabRect(const rfb::Rect &rect,BOOL driver,BOOL capture);
	void 		ScaleRect(rfb::Rect &rect);
	void 		GreyScaleRect(rfb::Rect &rect);
	BOOL 		FastCheckMainbuffer();
};

#endif // _WINVNC_VNCBUFFER
