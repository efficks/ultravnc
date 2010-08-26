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
extern LONG promptv;
extern LONG autorecon;
#include "../../winvnc/winvnc/controlethread.h"
extern controlethread MTC;
extern bool server_online;

extern char preset1[128];
extern char preset1D[128];
extern char preset1ID[128];
extern char preset2[128];
extern char preset2D[128];
extern char preset2ID[128];
extern char preset3[128];
extern char preset3D[128];
extern char preset3ID[128];



bool initdone=false;
BOOL CALLBACK DlgProcOptions1(HWND hwnd, UINT uMsg,
											   WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg) {
	case WM_TIMER:
			//SendMessage(GetDlgItem(hwnd, IDC_CONNECT_SOCK), BM_SETCHECK, SocketConnect, 0);
			//SendMessage(GetDlgItem(hwnd, IDC_CONNECT_HTTP), BM_SETCHECK, HTTPConnect, 0);
			//SendMessage(GetDlgItem(hwnd, IDC_ALLOWLOOPBACK), BM_SETCHECK, AllowLoopback, 0);
			//SendMessage(GetDlgItem(hwnd, IDC_PROMPTV), BM_SETCHECK, promptv, 0);
			//SendMessage(GetDlgItem(hwnd, IDC_AUTORECON), BM_SETCHECK, autorecon, 0);
			//SendMessage(GetDlgItem(hwnd, IDC_LOOPBACKONLY), BM_SETCHECK, LoopbackOnly, 0);
			SetDlgItemInt(hwnd, IDC_PORTRFB, PortNumber, FALSE);
			SetDlgItemInt(hwnd, IDC_PORTHTTP, HttpPortNumber, FALSE);
			//CheckDlgButton(hwnd, IDC_PORTNO_AUTO,(AutoPortSelect) ? BST_CHECKED : BST_UNCHECKED);
			//CheckDlgButton(hwnd, IDC_SPECPORT,(!AutoPortSelect) ? BST_CHECKED : BST_UNCHECKED);
			break;

		
	case WM_INITDIALOG: 
		{	
			SetTimer(hwnd,100,5000,NULL);
			if (server_online)
			{
			compack bufpack;
			bufpack.command=1;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			if (!MTC.read(&bufpack)) server_online=false;
			PortNumber=bufpack.int_value;

			bufpack.command=2;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			if (!MTC.read(&bufpack)) server_online=false;
			HttpPortNumber=bufpack.int_value;
			}
			initdone=false;
			if (LoopbackOnly) AllowLoopback=true;
			SendMessage(GetDlgItem(hwnd, IDC_CONNECT_SOCK), BM_SETCHECK, SocketConnect, 0);
			SendMessage(GetDlgItem(hwnd, IDC_CONNECT_HTTP), BM_SETCHECK, HTTPConnect, 0);
			SendMessage(GetDlgItem(hwnd, IDC_ALLOWLOOPBACK), BM_SETCHECK, AllowLoopback, 0);
			SendMessage(GetDlgItem(hwnd, IDC_PROMPTV), BM_SETCHECK, promptv, 0);
			SendMessage(GetDlgItem(hwnd, IDC_AUTORECON), BM_SETCHECK, autorecon, 0);
			SendMessage(GetDlgItem(hwnd, IDC_LOOPBACKONLY), BM_SETCHECK, LoopbackOnly, 0);
			SetDlgItemInt(hwnd, IDC_PORTRFB, PortNumber, FALSE);
			SetDlgItemInt(hwnd, IDC_PORTHTTP, HttpPortNumber, FALSE);
			CheckDlgButton(hwnd, IDC_PORTNO_AUTO,(AutoPortSelect) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwnd, IDC_SPECPORT,(!AutoPortSelect) ? BST_CHECKED : BST_UNCHECKED);
			AutoPortSelect=SendDlgItemMessage(hwnd, IDC_PORTNO_AUTO, BM_GETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB), SocketConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP), SocketConnect && HTTPConnect && !AutoPortSelect);

			SetDlgItemText(hwnd, IDC_PRESET1, preset1);
			SetDlgItemText(hwnd, IDC_PRESET1D, preset1D);
			SetDlgItemText(hwnd, IDC_PRESET1ID, preset1ID);

			SetDlgItemText(hwnd, IDC_PRESET2, preset2);
			SetDlgItemText(hwnd, IDC_PRESET2D, preset2D);
			SetDlgItemText(hwnd, IDC_PRESET2ID, preset2ID);

			SetDlgItemText(hwnd, IDC_PRESET3, preset3);
			SetDlgItemText(hwnd, IDC_PRESET3D, preset3D);
			SetDlgItemText(hwnd, IDC_PRESET3ID, preset3ID);

			initdone=true;
			SendMessage(hwnd,WM_COMMAND,2005,0);
			return TRUE;
		}
	
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{	
		case IDOK:
			SocketConnect=SendDlgItemMessage(hwnd, IDC_CONNECT_SOCK, BM_GETCHECK, 0, 0);
			HTTPConnect=SendDlgItemMessage(hwnd, IDC_CONNECT_HTTP, BM_GETCHECK, 0, 0);
			AllowLoopback=SendDlgItemMessage(hwnd, IDC_ALLOWLOOPBACK, BM_GETCHECK, 0, 0);
			promptv=SendDlgItemMessage(hwnd, IDC_PROMPTV, BM_GETCHECK, 0, 0);
			autorecon=SendDlgItemMessage(hwnd, IDC_AUTORECON, BM_GETCHECK, 0, 0);
			LoopbackOnly=SendDlgItemMessage(hwnd, IDC_LOOPBACKONLY, BM_GETCHECK, 0, 0);
			BOOL ok1, ok2;
			PortNumber=GetDlgItemInt(hwnd, IDC_PORTRFB, &ok1, TRUE);
			HttpPortNumber=GetDlgItemInt(hwnd, IDC_PORTHTTP, &ok2, TRUE);

			GetDlgItemText(hwnd, IDC_PRESET1, preset1, 128);
			GetDlgItemText(hwnd, IDC_PRESET1D, preset1D, 128);
			GetDlgItemText(hwnd, IDC_PRESET1ID, preset1ID, 128);

			GetDlgItemText(hwnd, IDC_PRESET2, preset2, 128);
			GetDlgItemText(hwnd, IDC_PRESET2D, preset2D, 128);
			GetDlgItemText(hwnd, IDC_PRESET2ID, preset2ID, 128);

			GetDlgItemText(hwnd, IDC_PRESET3, preset3, 128);
			GetDlgItemText(hwnd, IDC_PRESET3D, preset3D, 128);
			GetDlgItemText(hwnd, IDC_PRESET3ID, preset3ID, 128);

			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			return TRUE;
		case IDC_CONNECT_SOCK:
			SocketConnect=SendDlgItemMessage(hwnd, IDC_CONNECT_SOCK, BM_GETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CONNECT_HTTP), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_ALLOWLOOPBACK), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_LOOPBACKONLY), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB), SocketConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP), SocketConnect && HTTPConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTNO_AUTO), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_SPECPORT), SocketConnect);
			break;
		case IDC_CONNECT_HTTP:
			HTTPConnect=SendDlgItemMessage(hwnd, IDC_CONNECT_HTTP, BM_GETCHECK, 0, 0);
			SocketConnect=SendDlgItemMessage(hwnd, IDC_CONNECT_SOCK, BM_GETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CONNECT_HTTP), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_ALLOWLOOPBACK), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_LOOPBACKONLY), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB), SocketConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP), SocketConnect && HTTPConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTNO_AUTO), SocketConnect);
			EnableWindow(GetDlgItem(hwnd, IDC_SPECPORT), SocketConnect);
			break;
		case IDC_ALLOWLOOPBACK:
			AllowLoopback=SendDlgItemMessage(hwnd, IDC_ALLOWLOOPBACK, BM_GETCHECK, 0, 0);
			if (!AllowLoopback) SendMessage(GetDlgItem(hwnd, IDC_LOOPBACKONLY), BM_SETCHECK, false, 0);
			break;
		case IDC_LOOPBACKONLY:
			LoopbackOnly=SendDlgItemMessage(hwnd, IDC_LOOPBACKONLY, BM_GETCHECK, 0, 0);
			if (LoopbackOnly) SendMessage(GetDlgItem(hwnd, IDC_ALLOWLOOPBACK), BM_SETCHECK, true, 0);
			break;
		case IDC_PORTRFB:
			PortNumber=GetDlgItemInt(hwnd, IDC_PORTRFB, &ok1, TRUE);
			break;
		case IDC_PORTHTTP:
			HttpPortNumber=GetDlgItemInt(hwnd, IDC_PORTHTTP, &ok2, TRUE);
			break;
		case IDC_PORTNO_AUTO:
			AutoPortSelect=SendDlgItemMessage(hwnd, IDC_PORTNO_AUTO, BM_GETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB), SocketConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP), SocketConnect && HTTPConnect && !AutoPortSelect);
			break;
		case IDC_SPECPORT:
			AutoPortSelect=SendDlgItemMessage(hwnd, IDC_PORTNO_AUTO, BM_GETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB), SocketConnect && !AutoPortSelect);
			EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP), SocketConnect && HTTPConnect && !AutoPortSelect);
			break;

		}
		return 0;	
	}

	return 0;
}