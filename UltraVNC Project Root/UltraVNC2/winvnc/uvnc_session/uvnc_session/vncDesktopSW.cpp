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

// System headers
#include <assert.h>
#include "stdafx.h"

// Custom headers
#include "../../omnithread/omnithread.h"
#include "../../VNCHooks/VNCHooks.h"
#include "remoteServer.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "vncDesktop.h"

extern _sharedesktopmem *desktopsharedmem;

void vncDesktop::SWinit()
{
	m_Single_hWnd=NULL;
	m_Single_hWnd_backup=NULL;
	m_SWHeight=0;
	m_SWWidth=0;
	m_SWSizeChanged=FALSE;
	m_SWmoved=FALSE;
	desktopsharedmem->m_SWOffsetx=0;
	desktopsharedmem->m_SWOffsety=0;
	vnclog.Print(LL_INTINFO, VNCLOG("SWinit \n"));
}

rfb::Rect
vncDesktop::GetSize()
{
	if (!m_videodriver)
		{
		desktopsharedmem->m_SWOffsetx=0;
		desktopsharedmem->m_SWOffsety=0;
		return rfb::Rect(0, 0,desktopsharedmem->m_scrinfo.framebufferWidth, desktopsharedmem->m_scrinfo.framebufferHeight);
		}
	 else
		{
			if (desktopsharedmem->multi_monitor)
				return rfb::Rect(0,0,desktopsharedmem->mymonitor[2].Width,desktopsharedmem->mymonitor[2].Height);	
			else
				return rfb::Rect(0,0,desktopsharedmem->mymonitor[0].Width,desktopsharedmem->mymonitor[0].Height);
		}
}

rfb::Rect
vncDesktop::GetQuarterSize()
{
	vnclog.Print(LL_INTINFO, VNCLOG("GetQuarterSize \n"));
	{ 
	desktopsharedmem->m_SWOffsetx=0;
	desktopsharedmem->m_SWOffsety=0;
	m_Cliprect.tl.x=0;
	m_Cliprect.tl.y=0;
	m_Cliprect.br.x=m_bmrect.br.x;
	m_Cliprect.br.y=m_bmrect.br.y;
	vnclog.Print(LL_INTINFO, VNCLOG("GetQuarterSize \n"));
	return rfb::Rect(0, 0, m_bmrect.br.x, m_bmrect.br.y/4);
	}
}

