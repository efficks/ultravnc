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
#include "resource.h"
#include "upnp.h"
#include "firewall.h"
#include "log.h"
#include <iphlpapi.h>
#pragma comment ( lib, "iphlpapi" )
#include <shlwapi.h>
#pragma comment ( lib, "shlwapi" )
#ifndef _WINSOCK2API_
	#include <winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
#endif

extern LONG SocketConnect;
extern LONG HTTPConnect;
extern LONG XDMCPConnect;
extern LONG AutoPortSelect;
extern LONG PortNumber;
extern LONG HttpPortNumber;
extern LONG LoopbackOnly;
extern LONG AllowLoopback;

extern bool FireWall_status;
extern bool Service_status;
bool UPNP_status=false;
bool UPNP_status_checked=false;

HWND EditControl;
HWND networkproc;
char strLocalIP[256];
char strExternIP[256];

DWORD WINAPI upnpthread( LPVOID lpParam )
{
	if(FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
       CoUninitialize();
        return 0;
    }
	WSADATA wdata;
    WSAStartup(MAKEWORD(2, 2), &wdata);

	UPnP UPnPvar;

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	DWORD dwRetVal = 0;
	
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}
	
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		strcpy(strLocalIP,pAdapterInfo->IpAddressList.IpAddress.String);
		debug("Using port 5999 for Ext IP");
		UPnPvar.Set_UPnP(strLocalIP,"TCP","UVNC",5999);
		UPnPvar.OpenPorts(false);
		strcpy(strExternIP,UPnPvar.GetExternalIP());
		SendMessage(networkproc,WM_COMMAND,2003,0);
		
	}
	free(pAdapterInfo);
	UPnPvar.Set_UPnP(strLocalIP,"TCP","UVNC",5999);
	UPnPvar.ClosePorts(false);
	debug("Close 5999");
	WSACleanup();
	CoUninitialize();
	return 0;
}

DWORD WINAPI checkthread( LPVOID lpParam )
{
	if(FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
        CoUninitialize();
        return 0;
    }
	WSADATA wdata;
    WSAStartup(MAKEWORD(2, 2), &wdata);
	FirewallCheck(networkproc);
	
	WSACleanup();
	CoUninitialize();
	return 0;
}

DWORD WINAPI mapthread( LPVOID lpParam )
{
	if(FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
       CoUninitialize();
        return 0;
    }
	WSADATA wdata;
    WSAStartup(MAKEWORD(2, 2), &wdata);
	checksetport(PortNumber);

	UPnP UPnPvar;

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	DWORD dwRetVal = 0;
	
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}
	
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		strcpy(strLocalIP,pAdapterInfo->IpAddressList.IpAddress.String);
		UPnPvar.Set_UPnP(strLocalIP,"TCP","UVNC_TCP",PortNumber);
		UPnPvar.OpenPorts(true);
		strcpy(strExternIP,UPnPvar.GetExternalIP());
		SendMessage(networkproc,WM_COMMAND,2003,0);
		
	}
	free(pAdapterInfo);
	WSACleanup();
	CoUninitialize();
	return 0;
}

DWORD WINAPI unmapthread( LPVOID lpParam )
{
	if(FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
       CoUninitialize();
        return 0;
    }
	WSADATA wdata;
    WSAStartup(MAKEWORD(2, 2), &wdata);

	UPnP UPnPvar;

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	DWORD dwRetVal = 0;
	
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}
	
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		strcpy(strLocalIP,pAdapterInfo->IpAddressList.IpAddress.String);
		UPnPvar.Set_UPnP(strLocalIP,"TCP","UVNC_TCP",PortNumber);
		//UPnPvar.OpenPorts(true);
		strcpy(strExternIP,UPnPvar.GetExternalIP());
		SendMessage(networkproc,WM_COMMAND,2003,0);
		
	}
	free(pAdapterInfo);
	UPnPvar.ClosePorts(true);
	WSACleanup();
	CoUninitialize();
	return 0;
}

DWORD WINAPI Fixhread( LPVOID lpParam )
{
	if(FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
        CoUninitialize();
        return 0;
    }
	WSADATA wdata;
    WSAStartup(MAKEWORD(2, 2), &wdata);
	if (!Service_status) 
	{
		if (ControlSSDPService(true)) Service_status=true;
	}
	if (!FireWall_status) 
	{
		if(IsICSConnEnabled()) debug("WARNING!  Internet Connection Sharing is active.  Opening UPnP ports could expose the computer directly to the Internet.");
		if (ControlUPnPPorts(true)) FireWall_status=true;
	}
	
	WSACleanup();
	CoUninitialize();
	SendMessage(networkproc,WM_COMMAND,2001,0);
	return 0;
}
bool initdone7=false;
BOOL CALLBACK DlgProcOptions6(HWND hwnd, UINT uMsg,
											   WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg) {
		
	case WM_INITDIALOG: 
		{	
			initdone7=false;
			EditControl=GetDlgItem(hwnd,IDC_EDIT3);
			networkproc=hwnd;
			initdone7=true;
			SendMessage(hwnd,WM_COMMAND,2005,0);
			return TRUE;
		}
	
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{	
		case 2001:
			if (FireWall_status && Service_status) UPNP_status=true;
				else UPNP_status=false;
			UPNP_status_checked=true;
			SendMessage(hwnd,WM_COMMAND,2002,0);
			break;
		case 2003:
			SetWindowText(GetDlgItem(hwnd, IDC_LOCAL),strLocalIP);
			SetWindowText(GetDlgItem(hwnd, IDC_REMOTE),strExternIP);
			break;
		case IDC_CHECKUPNP:
			{
			DWORD dw;
			HANDLE thread=CreateThread( NULL, 0,checkthread, NULL, 0, &dw);
			}
			break;
		case IDC_TESTNET:
			{
			DWORD dw;
			HANDLE thread=CreateThread( NULL, 0,upnpthread, NULL, 0, &dw);
			}
			break;
		case IDC_FIXUPNP:
			{
			DWORD dw;
			HANDLE thread=CreateThread( NULL, 0,Fixhread, NULL, 0, &dw);
			}
			break;
		case IDC_UPNPON:
			{
			DWORD dw;
			HANDLE thread=CreateThread( NULL, 0,mapthread, NULL, 0, &dw);
			}
			break;
		case IDC_UPNPOFF:
			{
			DWORD dw;
			HANDLE thread=CreateThread( NULL, 0,unmapthread, NULL, 0, &dw);
			}
			break;
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			return TRUE;
		default:
			if (initdone7){
				EnableWindow(GetDlgItem(hwnd, IDC_UPNPON),  UPNP_status);
				EnableWindow(GetDlgItem(hwnd, IDC_UPNPOFF),  UPNP_status);
				EnableWindow(GetDlgItem(hwnd, IDC_TESTNET),UPNP_status);
				EnableWindow(GetDlgItem(hwnd, IDC_CHECKUPNP), true);
				EnableWindow(GetDlgItem(hwnd, IDC_FIXUPNP),!UPNP_status && UPNP_status_checked );
			}
			break;

		}
		return 0;	
	}

	return 0;
}