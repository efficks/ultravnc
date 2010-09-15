/////////////////////////////////////////////////////////////////////////////
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
#include "uvnc_session/uvnc_session/communication.h"
#include "vncServer.h"
#include "ReadProperties.h"

extern bool PostAddAutoConnectClient_bool;
extern bool PostAddNewClient_bool;
extern bool PostAddAutoConnectClient_bool_null;
extern bool PostAddConnectClient_bool;
extern bool PostAddConnectClient_bool_null;
extern bool PostAddNewRepeaterClient_bool;
extern char pszId_char[20];
extern VCard32 address_vcard;
extern int port_int;
extern char* g_szRepeaterHost;

void Init_shared_mem_and_events();
DWORD WINAPI tcp_controle_thread(LPVOID lpParam);
DWORD WINAPI settings_change_monitor(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread8(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread7(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread6(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread5(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread4(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread3(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread2(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread(LPVOID lpParam);