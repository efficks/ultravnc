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


// WinVNC header file

#include "stdhdrs.h"
#include "resource.h"

// Application specific messages

// Message used for system tray notifications
#define WM_TRAYNOTIFY				WM_APP+1

// Messages used for the server object to notify windows of things
#define WM_SRV_CLIENT_CONNECT		WM_APP+2
#define WM_SRV_CLIENT_AUTHENTICATED	WM_APP+3
#define WM_SRV_CLIENT_DISCONNECT	WM_APP+4

#define WM_MOUSESHAPE				WM_APP+6
#define WM_HOOKCHANGE				WM_APP+7
#define WM_SHUTDOWN					WM_APP+8


// Export the application details
extern const char	*szAppName;
extern DWORD		mainthreadId;

// Main VNC server routine

extern int WinVNCAppMain();

// Standard command-line flag definitions
const char winvncStartService[]		= "-service";
const char winvncRunAsUserApp[]		= "-run";
const char winvncConnect[]		= "-connect";
const char winvncAutoReconnect[]	= "-autoreconnect";
const char winvncAutoReconnectId[]	= "id:";
const char winvncReconnectId[]	= "-id:";

//adzm 2009-06-20
// for use with -sc
const char winvncRepeater[]	= "-repeater"; // set default repeater host
extern char* g_szRepeaterHost;

const char winvncKill[]						= "-kill";

// Usage string
const char winvncUsageText[]		= "winvnc [-id:????] [-autoreconnect[ ID:????]] [-connect host[:display]] [-connect host[::port]] [-repeater host[:port]] [-run]\n";
