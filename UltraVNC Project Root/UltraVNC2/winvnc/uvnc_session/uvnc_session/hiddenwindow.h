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
#include "SystemTray.h"
#include "vncListDlg.h"
#include "communication.h"

#ifndef HIDDENWINDOW
#define HIDDENWINDOW

#define MAX_LOADSTRING 100
#define	WM_ICON_NOTIFY WM_APP+10

typedef DWORD (WINAPI* pWTSGetActiveConsoleSessionId)(VOID);
typedef BOOL (WINAPI * pProcessIdToSessionId)(DWORD,DWORD*);

class hiddenwindow
{
public:
    hiddenwindow();
	virtual ~hiddenwindow();
	ATOM				MyRegisterClass(HINSTANCE hInstance);
	BOOL				InitInstance(HINSTANCE);
	CSystemTray *TrayIcon;
	bool Start();
	void Stop();
	const bool wait() const;
	const HANDLE& GetHandle() const { return m_hThread; }
	void ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);
	void Wallpaperaero();
	bool Viewer_connected();
	void Set_server_shared_memory( _shareservermem *IN_serverSharedmem);
	bool Isrunning();

private:
	hiddenwindow(const hiddenwindow&);
    hiddenwindow& operator=(const hiddenwindow&);

    static unsigned __stdcall threadProc(void*);
    unsigned int run();

	HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;

	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM); 
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	bool Check_Inputdesktop();
	bool Set_Inputdesktop();
	void SetThreadDesktopName();	
	char threadname[256];
	char inputname[256];
	HWND hWnd_HW;

	HICON load;
	HICON free;
	int nr_clients_connected;
	bool bool_RemoveWallpaperEnabled;
	bool bool_RemoveAeroEnabled;
	char m_username[UNLEN+1];
	char m_old_username[UNLEN+1];
	DWORD GetCurrentSessionID();
	void GetIPAddrString(char *buffer, int buflen);

	vncListDlg		m_ListDlg;

	comm_serv AuthClientCount;
	comm_serv SockConnected;
	comm_serv KillAuthClients;
	pProcessIdToSessionId WTSProcessIdToSessionIdF;
	 _shareservermem *serverSharedmem;
};

#endif