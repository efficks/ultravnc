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
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// vncAcceptDialog.cpp: implementation of the vncAcceptDialog class, used
// to query whether or not to accept incoming connections.

#include "stdafx.h"
#include "vncAcceptDialog.h"
//#include "WinVNC.h"
//#include "vncService.h"

#include "resource.h"
#include "win32_helpers.h"

//#include "localization.h" // Act : add localization on messages


BOOL IsWSLocked()
{
	bool bLocked = false;
	HDESK hDesk;
	BOOL bRes;
	DWORD dwLen;
	char sName[200];
	
	hDesk = OpenInputDesktop(0, FALSE, 0);

	if (hDesk == NULL)
	{
		 bLocked = true;
	}
	else 
	{
		bRes = GetUserObjectInformation(hDesk, UOI_NAME, sName, sizeof(sName), &dwLen);

		if (bRes)
			sName[dwLen]='\0';
		else
			sName[0]='\0';


		if (_stricmp(sName,"Default") != 0)
			 bLocked = true; // WS is locked or screen saver active
		else
			 bLocked = false ;
	}
	if (hDesk != NULL)
		CloseDesktop(hDesk);

	return bLocked;
}

unsigned char  vncAcceptDialogFn(unsigned char verified, bool QueryIfNoLogon,unsigned int QueryAccept,unsigned int QueryTimeout,char *PeerName )
{
	// If necessary, query the connection with a timed dialog
	char username[UNLEN+1];
	if (!GetCurrentUser(username, sizeof(username),app)) return 99;
	if ((strcmp(username, "") != 0) || QueryIfNoLogon) // marscha@2006 - Is AcceptDialog required even if no user is logged on
	{
		if (verified == aqrQuery) {
            // 10 Dec 2008 jdp reject/accept all incoming connections if the workstation is locked
            if (IsWSLocked() && !QueryIfNoLogon) {
                verified = QueryAccept == 1 ? aqrAccept : aqrReject;
            } else {

			vncAcceptDialog *acceptDlg = new vncAcceptDialog(QueryTimeout,QueryAccept, PeerName);
	
			if (acceptDlg == NULL) 
				{
					if (QueryAccept==1) 
					{
						verified = aqrAccept;
					}
					else 
					{
						verified = aqrReject;
					}
				}
			else 
				{
						HDESK desktop;
						desktop = OpenInputDesktop(0, FALSE,
													DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
													DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
													DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
													DESKTOP_SWITCHDESKTOP | GENERIC_WRITE
													);

						HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
						
					
						SetThreadDesktop(desktop);

						if ( !(acceptDlg->DoDialog()) ) verified = aqrReject;

						SetThreadDesktop(old_desktop);
						CloseDesktop(desktop);
				}
            }
		}
    }
	return verified;
}

// Constructor

vncAcceptDialog::vncAcceptDialog(UINT timeoutSecs,BOOL acceptOnTimeout, const char *ipAddress)
{
	m_timeoutSecs = timeoutSecs;
	m_ipAddress = _strdup(ipAddress);
	m_foreground_hack=FALSE;
	m_acceptOnTimeout = acceptOnTimeout;
}

// Destructor

vncAcceptDialog::~vncAcceptDialog()
{
	if (m_ipAddress)
		free(m_ipAddress);
}

// Routine called to activate the dialog and, once it's done, delete it

BOOL vncAcceptDialog::DoDialog()
{
	//	[v1.0.2-jp1 fix]
	//int retVal = DialogBoxParam(hAppInstance, MAKEINTRESOURCE(IDD_ACCEPT_CONN), 
	int retVal = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ACCEPT_CONN), 
		NULL, (DLGPROC) vncAcceptDlgProc, (LONG) this);
	delete this;
	switch (retVal) 
	{
		case IDREJECT:
			return 0;
		case IDACCEPT:
			return 1;
	}
	return (m_acceptOnTimeout) ? 1 : 0;

}

// Callback function - handles messages sent to the dialog box

BOOL CALLBACK vncAcceptDialog::vncAcceptDlgProc(HWND hwnd,
											UINT uMsg,
											WPARAM wParam,
											LPARAM lParam) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
     vncAcceptDialog *_thiss = helper::SafeGetWindowUserData<vncAcceptDialog>(hwnd);
	switch (uMsg) {

		// Dialog has just been created
	case WM_INITDIALOG:
		{
			// Save the lParam into our user data so that subsequent calls have
			// access to the parent C++ object
            helper::SafeSetWindowUserData(hwnd, lParam);
            vncAcceptDialog *_this = (vncAcceptDialog *) lParam;

			// Set the IP-address string
			SetDlgItemText(hwnd, IDC_ACCEPT_IP, _this->m_ipAddress);
			if (SetTimer(hwnd, 1, 1000, NULL) == 0)
			{
				if (_this->m_acceptOnTimeout)
					EndDialog(hwnd, IDACCEPT);
				else
				EndDialog(hwnd, IDREJECT);
			}
			_this->m_timeoutCount = _this->m_timeoutSecs;

			char temp[256];
			if (_this->m_acceptOnTimeout)
				sprintf_s(temp, "AutoAccept:%u", (_this->m_timeoutCount));
			else
				sprintf_s(temp, "AutoReject:%u", (_this->m_timeoutCount));
			SetDlgItemText(hwnd, IDC_ACCEPT_TIMEOUT, temp);

			if (!_this->m_foreground_hack) {
				SetForegroundWindow(hwnd);
			}

			// Beep
			MessageBeep(MB_ICONEXCLAMATION);
            
            // Return success!
			return TRUE;
		}

		// Timer event
	case WM_TIMER:
		if ((_thiss->m_timeoutCount) == 0)
			{
				if ( _thiss->m_acceptOnTimeout ) 
					{
						EndDialog(hwnd, IDACCEPT);
					}
				else 
					{
						EndDialog(hwnd, IDREJECT);
					}
			}
		_thiss->m_timeoutCount--;

		// Flash if necessary
		if (_thiss->m_foreground_hack) {
			if (GetWindowThreadProcessId(GetForegroundWindow(), NULL) != GetCurrentProcessId())
			{
				_thiss->m_flash_state = !_thiss->m_flash_state;
				FlashWindow(hwnd, _thiss->m_flash_state);
			} else {
				_thiss->m_foreground_hack = FALSE;
			}
		}

		// Update the displayed count
		char temp[256];
		if ( _thiss->m_acceptOnTimeout )
			sprintf_s(temp, 256,"AutoAccept: %u", (_thiss->m_timeoutCount));
		else
			sprintf_s(temp, 256,"AutoReject: %u", (_thiss->m_timeoutCount));
		SetDlgItemText(hwnd, IDC_ACCEPT_TIMEOUT, temp);
		break;

		// Dialog has just received a command
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

			// User clicked Accept or pressed return
		case IDACCEPT:
		case IDOK:
			EndDialog(hwnd, IDACCEPT);
			return TRUE;

		case IDREJECT:
		case IDCANCEL:
			EndDialog(hwnd, IDREJECT);
			return TRUE;
		};

		break;

		// Window is being destroyed!  (Should never happen)
	case WM_DESTROY:
		EndDialog(hwnd, IDREJECT);
		return TRUE;
	}
	return 0;
}

