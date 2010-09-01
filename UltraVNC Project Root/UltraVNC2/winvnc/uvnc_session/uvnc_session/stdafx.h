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
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <winsock2.h>
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
#include <iostream>
#include <sddl.h>
#pragma warning (disable:6387)
#include <shobjidl.h>
#include <atldbcli.h>
#pragma warning (default:6387)
#include <lmcons.h>


// TODO: reference additional headers your program requires here
#include "VNCLog.h"
extern VNCLog vnclog;

// TODO: reference additional headers your program requires here
// No logging at all
#define LL_NONE		0
// Log server startup/shutdown
#define LL_STATE	0
// Log connect/disconnect
#define LL_CLIENTS	1
// Log connection errors (wrong pixfmt, etc)
#define LL_CONNERR	0
// Log socket errors
#define LL_SOCKERR	4
// Log internal errors
#define LL_INTERR	0

// Log internal warnings
#define LL_INTWARN	8
// Log internal info
#define LL_INTINFO	9
// Log socket errors
#define LL_SOCKINFO	10
// Log everything, including internal table setup, etc.
#define LL_ALL		10

// Macros for sticking in the current file name
#define VNCLOG(s)	(__FILE__ " : " s)

#define WM_MOUSESHAPE				WM_APP+6
#define WM_HOOKCHANGE				WM_APP+7
#define WM_SHUTDOWN					WM_APP+8

typedef BOOL (WINAPI *CHANGEWINDOWMESSAGEFILTER)(UINT message, DWORD dwFlag);

extern bool app;
extern bool GLOBAL_RUNNING;
extern HINSTANCE hInst;
extern bool bool_GetAllowEditClients;
extern BOOL SPECIAL_SC_PROMPT;
extern char old_buffer[1024];
extern char old_buflen;
extern int counter;
extern bool g_DesktopThread_running;
extern bool g_update_triggered;
extern bool stop_hookwatch;
extern bool g_Desktop_running;



int OSversion();
DWORD WINAPI Driverwatch(LPVOID lpParam);
DWORD WINAPI InitWindowThread(LPVOID lpParam);
DWORD WINAPI hookwatch(LPVOID lpParam);
DWORD WINAPI BlackWindow(LPVOID lpParam);

bool GetCurrentUser(char *buffer, UINT size,bool app);
unsigned char  vncAcceptDialogFn(unsigned char verified, bool QueryIfNoLogon,unsigned int QueryAccept,unsigned int QueryTimeout,char *PeerName );
enum AcceptQueryReject {aqrAccept, aqrQuery, aqrReject};
bool GetCurrentUser(char *buffer, UINT size,bool app);
bool SelectHDESK(HDESK desk);
bool InputDesktopSelected2();
bool SelectDesktop(char *name,HDESK *desk);