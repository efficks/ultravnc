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
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/

// vncBuffer object

// The vncBuffer object provides a client-local copy of the screen
// It can tell the client which bits have changed in a given region
// It uses the specified remoteDesktop to read screen data from

#if !defined(_WINVNC_VNCBUFFER)
#define _WINVNC_VNCBUFFER
#pragma once


// Includes
class vncBuffer;
#include "stdhdrs.h"
#include "remotedesktop.h"
#include "vncEncoder.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "rfb.h"
#include "vncmemcpy.h"


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

	remoteDesktop	*m_desktop;
	void SetDesktop(remoteDesktop *desktop){m_desktop=desktop;};
	void EnableCache(BOOL enable);
	void EnableGreyPalette(BOOL enable);
	rfb::Rect GetSize();	
	void SetCursorPending(BOOL enable){sharedbuffermem->m_cursorpending=enable;};	
	rfb::Rect GetViewerSize();
	UINT GetScale();
	void GetMousePos(rfb::Rect &rect);	
	rfbPixelFormat GetLocalFormat();
	BOOL SetScale(int scale);


	void ClearCache();
	void ClearBack();
	BOOL IsCursorUpdatePending(){return sharedbuffermem->m_cursorpending;};
	BOOL CheckBuffer();
	void MultiMonitors(int number);	

	BYTE			*m_backbuff;
	comm_serv		*m_backbuff_IPC;

	BYTE			*m_cachebuff;
	comm_serv		*m_cachebuff_IPC;

	BYTE			*m_ScaledBuff;
	comm_serv		*m_ScaledBuff_IPC;

	UINT			m_backbuffsize;
	UINT			m_ScaledSize;

	_sharedbuffermem *sharedbuffermem;
	comm_serv bufferIPC;

private:
	
	
};

#endif // _WINVNC_VNCBUFFER
