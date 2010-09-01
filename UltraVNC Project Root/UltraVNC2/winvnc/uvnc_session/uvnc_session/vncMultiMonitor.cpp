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
//#include <WinAble.h>
#include "../../omnithread/omnithread.h"
//#include "WinVNC.h"
#include "../../VNCHooks/VNCHooks.h"
#include "remoteServer.h"
//#include "vncKeymap.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "vncDesktop.h"
//#include "vncService.h"
// Modif rdv@2002 - v1.1.x - videodriver
#include "vncOSVersion.h"

#include "mmSystem.h" // sf@2002
//#include "TextChat.h" // sf@2002
#include "vncdesktopthread.h"
#include "../../../common/win32_helpers.h"

void 
vncDesktop::Checkmonitors()
{
  nr_monitors=GetNrMonitors();
  DEVMODE devMode;
  if (nr_monitors>0)
  {
	if(OSversion()==1 || OSversion()==2 || OSversion()==4 )GetPrimaryDevice();
	devMode.dmSize = sizeof(DEVMODE);
	if(OSversion()==1 || OSversion()==2 || OSversion()==4 ) EnumDisplaySettings(desktopsharedmem->mymonitor[0].device, ENUM_CURRENT_SETTINGS, &devMode);
	else EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	desktopsharedmem->mymonitor[0].offsetx=devMode.dmPosition.x;
	desktopsharedmem->mymonitor[0].offsety=devMode.dmPosition.y;
	desktopsharedmem->mymonitor[0].Width=devMode.dmPelsWidth;
	desktopsharedmem->mymonitor[0].Height=devMode.dmPelsHeight;
	desktopsharedmem->mymonitor[0].Depth=devMode.dmBitsPerPel;
  }
  if (nr_monitors>1)
  {
	GetSecondaryDevice();
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(desktopsharedmem->mymonitor[1].device, ENUM_CURRENT_SETTINGS, &devMode);
	desktopsharedmem->mymonitor[1].offsetx=devMode.dmPosition.x;
	desktopsharedmem->mymonitor[1].offsety=devMode.dmPosition.y;
	desktopsharedmem->mymonitor[1].Width=devMode.dmPelsWidth;
	desktopsharedmem->mymonitor[1].Height=devMode.dmPelsHeight;
	desktopsharedmem->mymonitor[1].Depth=devMode.dmBitsPerPel;
  }
	///
    desktopsharedmem->mymonitor[2].offsetx=GetSystemMetrics(SM_XVIRTUALSCREEN);
    desktopsharedmem->mymonitor[2].offsety=GetSystemMetrics(SM_YVIRTUALSCREEN);
    desktopsharedmem->mymonitor[2].Width=GetSystemMetrics(SM_CXVIRTUALSCREEN);
    desktopsharedmem->mymonitor[2].Height=GetSystemMetrics(SM_CYVIRTUALSCREEN);
	desktopsharedmem->mymonitor[2].Depth=desktopsharedmem->mymonitor[0].Depth;//depth primary monitor is used

}



int
vncDesktop::GetNrMonitors()
{
	if(OSversion()==3 || OSversion()==5) return 1;
	int i;
    int j=0;
    
    helper::DynamicFn<pEnumDisplayDevices> pd("USER32","EnumDisplayDevicesA");

    if (pd.isValid())
    {
        DISPLAY_DEVICE dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        for (i=0; (*pd)(NULL, i, &dd, 0); i++)
			{
				if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
					if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))j++;
			}
	}
	return j;
}

void
vncDesktop::GetPrimaryDevice()
{
	int i;
    int j=0;
    helper::DynamicFn<pEnumDisplayDevices> pd("USER32","EnumDisplayDevicesA");

    if (pd.isValid())
    {
        DISPLAY_DEVICE dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        for (i=0; (*pd)(NULL, i, &dd, 0); i++)
			{
				if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
					if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
						if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
						{
							strcpy(desktopsharedmem->mymonitor[0].device,(char *)dd.DeviceName);
						}

			}
	}
}

void
vncDesktop::GetSecondaryDevice()
{
	int i;
    int j=0;
    helper::DynamicFn<pEnumDisplayDevices> pd("USER32","EnumDisplayDevicesA");

    if (pd.isValid())
    {
        DISPLAY_DEVICE dd;
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        for (i=0; (*pd)(NULL, i, &dd, 0); i++)
			{
				if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
					if (!(dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
						if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
						{
							strcpy(desktopsharedmem->mymonitor[1].device,(char *)dd.DeviceName);
						}

			}
	}
}
