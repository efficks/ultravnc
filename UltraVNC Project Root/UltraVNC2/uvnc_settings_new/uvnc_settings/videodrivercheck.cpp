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
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include "vncOSVersion.h"


///////////////////////////////////////////////////////////////////
BOOL GetDllProductVersion(char* dllName, char *vBuffer, int size)
{
   char *versionInfo;
   void *lpBuffer;
   unsigned int cBytes;
   DWORD rBuffer;

   if( !dllName || !vBuffer )
      return(FALSE);

//   DWORD sName = GetModuleFileName(NULL, fileName, sizeof(fileName));
// FYI only
   DWORD sVersion = GetFileVersionInfoSize(dllName, &rBuffer);
   if (sVersion==0) return (FALSE);
   versionInfo = new char[sVersion];

   BOOL resultVersion = GetFileVersionInfo(dllName,
                                           NULL,
                                           sVersion,
                                           versionInfo);

   BOOL resultValue = VerQueryValue(versionInfo,  

TEXT("\\StringFileInfo\\040904b0\\ProductVersion"), 
                                    &lpBuffer, 
                                    &cBytes);

   if( !resultValue )
   {
	   resultValue = VerQueryValue(versionInfo,TEXT("\\StringFileInfo\\000004b0\\ProductVersion"), 
                                    &lpBuffer, 
                                    &cBytes);

   }

   if( resultValue )
   {
      strncpy(vBuffer, (char *) lpBuffer, size);
      delete [] versionInfo;
      return(TRUE);
   }
   else
   {
      *vBuffer = '\0';
      delete [] versionInfo;
      return(FALSE);
   }
}

///////////////////////////////////////////////////////////////////

bool
CheckVideoDriver(bool Box)
{
		typedef BOOL (WINAPI* pEnumDisplayDevices)(PVOID,DWORD,PVOID,DWORD);
		HDC m_hrootdc=NULL;
		LPSTR driverName = "mv video hook driver2";
		BOOL DriverFound;
		DEVMODE devmode;
		FillMemory(&devmode, sizeof(DEVMODE), 0);
		devmode.dmSize = sizeof(DEVMODE);
		devmode.dmDriverExtra = 0;
		BOOL change = EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devmode);
		devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		char deviceName[64];
		DISPLAY_DEVICE dd;
		ZeroMemory(&dd, sizeof(dd));
		dd.cb = sizeof(dd);
		devmode.dmDeviceName[0] = '\0';
		INT devNum = 0;
		BOOL result;
		DriverFound=false;
		while (result = EnumDisplayDevices(NULL,devNum, &dd,0))
				{
					if (strcmp((const char *)&dd.DeviceString[0], driverName) == 0)
					{
					DriverFound=true;
					break;
					}
					ZeroMemory(&dd, sizeof(dd));
					dd.cb = sizeof(dd);
					devNum++;
				}
		
		if (DriverFound)
				{
					if(Box)
					{
						char buf[512];
						strcpy(buf,"");
						GetDllProductVersion("mv2.dll",buf,512);
						if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
						{
							strcat_s(buf,512," driver Active");
							HDC testdc=NULL;
							strcpy_s(deviceName,64,(LPSTR)&dd.DeviceName[0]);
							testdc = CreateDC("DISPLAY",deviceName,NULL,NULL);	
							if (testdc)
							{
								DeleteDC(testdc);
								strcat_s(buf,512," access ok");
								MessageBox(NULL,buf,"Driver found: version ok",0);
							}
							else
							{
								strcat_s(buf,512," access denied, permission problem");
								MessageBox(NULL,buf,"Driver found: version ok",0);
							}
						}
						else
							if (strcmp(buf,"1.00.22")==NULL)
							{
								strcat_s(buf,512," driver Current Not Active (viewer not connected ?)");
								MessageBox(NULL,buf,"Driver found: version ok",0);
							}
							else
							{
								strcat_s(buf,512," driver Not Active");
								MessageBox(NULL,buf,"Driver found: required version 1.00.22",0);
							}
					}
					return true;
				}
				else if(Box) MessageBox(NULL,"Driver not found: Perhaps you need to reboot after install","driver info: required version 1.00.22",0);
	return false;
}
