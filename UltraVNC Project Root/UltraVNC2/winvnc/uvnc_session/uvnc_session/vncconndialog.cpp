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
#include "vncConnDialog.h"
#include "resource.h"
#include "win32_helpers.h"
#include "communication.h"

vncConnDialog::vncConnDialog()
{
	m_hicon = NULL;
	m_hfont = NULL;
	if (!Get_g_szRepeaterHost.Init("Get_g_szRepeaterHost",0,256,app,false)) goto error;
	if (!Reverse.Init("Reverse",sizeof(_reverse),1,app,false)) goto error;
	return;
	error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);

}

// Destructor

vncConnDialog::~vncConnDialog()
{
}

// Routine called to activate the dialog and, once it's done, delete it

INT_PTR vncConnDialog::DoDialog()
{
	//	[v1.0.2-jp1 fix]
	//DialogBoxParam(hAppInstance, MAKEINTRESOURCE(IDD_OUTGOING_CONN), 	
	//adzm 2009-06-20 - Return the result
	INT_PTR nResult = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_OUTGOING_CONN), 
		NULL, (DLGPROC) vncConnDlgProc, (LONG) this);
	delete this;
	return nResult;
}

// Callback function - handles messages sent to the dialog box

BOOL CALLBACK vncConnDialog::vncConnDlgProc(HWND hwnd,
											UINT uMsg,
											WPARAM wParam,
											LPARAM lParam) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
     vncConnDialog *_thiss = helper::SafeGetWindowUserData<vncConnDialog>(hwnd);
	switch (uMsg) {
	case WM_PAINT:
		{
			//adzm 2009-06-20
			if (GetUpdateRect(hwnd, NULL, TRUE)) {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				{
					RECT rcIcon;
					rcIcon.top = 0;
					rcIcon.left = 0;
					rcIcon.bottom = 48;
					rcIcon.right = 48;
					
					RECT rcClient;
					if (GetClientRect(hwnd, &rcClient)) {
						int nDifference = (rcClient.bottom - rcIcon.bottom) / 2;
						if (nDifference > 0) {
							rcIcon.top += nDifference;
							rcIcon.bottom += nDifference;
						}
					}

					RECT rcLabel;
					HWND hwndLabel = GetDlgItem(hwnd, IDC_CONNECTION_NUMBER_STATIC);
					if (GetWindowRect(hwndLabel, &rcLabel)) {
						LPRECT lprcLabel = &rcLabel;
						ScreenToClient(hwnd, (LPPOINT)lprcLabel);
						ScreenToClient(hwnd, ((LPPOINT)lprcLabel)+1);

						int nAdjustment = (rcLabel.left - rcIcon.right) / 2;

						rcIcon.left += nAdjustment;
						rcIcon.right += nAdjustment;
					}

					RECT rcIntersect;

					if (IntersectRect(&rcIntersect, &rcIcon, &(ps.rcPaint))) {
						if (_thiss->m_hicon == NULL) {
							_thiss->m_hicon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_WINVNC), IMAGE_ICON,
							   48, 48, 0);
						}
						if (_thiss->m_hicon) {
							HBRUSH hbr = (HBRUSH)SendMessage(hwnd, WM_CTLCOLORDLG, (WPARAM)hdc, (WPARAM)hwnd);
							DrawIconEx(hdc, rcIcon.left, rcIcon.top, _thiss->m_hicon, 48, 48, 0, hbr, DI_NORMAL);
						}
					}
				}

				EndPaint(hwnd, &ps);
			}

			return 0;
		}

		// Dialog has just been created
	case WM_INITDIALOG:
		{
			// Save the lParam into our user data so that subsequent calls have
			// access to the parent C++ object
            helper::SafeSetWindowUserData(hwnd, lParam);

            vncConnDialog *_this = (vncConnDialog *) lParam;
			char rphost[256];
			_this->Get_g_szRepeaterHost.Call_Fnction(NULL,rphost);

			//adzm 2009-06-20
			if (strlen(rphost)>0) {
				SetDlgItemText(hwnd, IDC_HOSTNAME_EDIT, rphost);

				//adzm 2009-06-20
				if (SPECIAL_SC_PROMPT) {
					HWND hwndChild = GetDlgItem(hwnd, IDC_HOSTNAME_EDIT);
					if (hwndChild) {
						ShowWindow(hwndChild, SW_HIDE);
					}
					hwndChild = GetDlgItem(hwnd, IDC_HOSTNAME_STATIC);
					if (hwndChild) {
						ShowWindow(hwndChild, SW_HIDE);
					}

					HWND hwndEdit = GetDlgItem(hwnd, IDC_IDCODE);
					HWND hwndLabel = GetDlgItem(hwnd, IDC_CONNECTION_NUMBER_STATIC);

					RECT rcEdit;
					RECT rcLabel;
					GetWindowRect(hwndEdit, &rcEdit);
					GetWindowRect(hwndLabel, &rcLabel);

					LPRECT lprcEdit = &rcEdit;
					LPRECT lprcLabel = &rcLabel;

					ScreenToClient(hwnd, (LPPOINT)lprcEdit);
					ScreenToClient(hwnd, ((LPPOINT)lprcEdit)+1);
					
					ScreenToClient(hwnd, (LPPOINT)lprcLabel);
					ScreenToClient(hwnd, ((LPPOINT)lprcLabel)+1);

					RECT rcClient;
					GetClientRect(hwnd, &rcClient);

					long nTotalHeight = rcEdit.bottom - rcLabel.top;

					long nAdjustedTop = (rcClient.bottom - nTotalHeight) / 2;

					long nAdjustment = nAdjustedTop - rcLabel.top;

					MoveWindow(hwndLabel, rcLabel.left, rcLabel.top + nAdjustment, rcLabel.right - rcLabel.left, rcLabel.bottom - rcLabel.top, TRUE);
					MoveWindow(hwndEdit, rcEdit.left, rcEdit.top + nAdjustment, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, TRUE);

					HWND hwndCaption = GetDlgItem(hwnd, IDC_CAPTION_STATIC);
					HFONT hFont = (HFONT)SendMessage(hwndCaption, WM_GETFONT, 0, 0);
					if (hFont) {
						LOGFONT lf;
						if (GetObject(hFont, sizeof(LOGFONT), &lf)) {
							lf.lfWidth = 0;
							lf.lfHeight = (lf.lfHeight * 6) / 4;

							_this->m_hfont = CreateFontIndirect(&lf);

							if (_this->m_hfont) {
								SendMessage(hwndCaption, WM_SETFONT, (WPARAM)_this->m_hfont, (LPARAM)TRUE);
							}
						}
					}
					SetWindowText(hwndCaption, "Connect to Technical Support");
					ShowWindow(hwndCaption, SW_SHOWNA);
				}

				SetFocus(GetDlgItem(hwnd, IDC_IDCODE));
			} else {            
				// Make the text entry box active
				SetFocus(GetDlgItem(hwnd, IDC_HOSTNAME_EDIT));
			}
			
			SetForegroundWindow(hwnd);
			
            // Return success!
			return TRUE;
		}

		// Dialog has just received a command
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			// User clicked OK or pressed return
		case IDOK:
			{
			// sf@2002 - host:num & host::num analyse.
			// Compatible with both RealVNC and TightVNC methods
			char hostname[_MAX_PATH];
			char idcode[_MAX_PATH];

			// Get the hostname of the VNCviewer
			GetDlgItemText(hwnd, IDC_HOSTNAME_EDIT, hostname, _MAX_PATH);
			GetDlgItemText(hwnd, IDC_IDCODE, idcode, _MAX_PATH);
			char returnval=0;
			_reverse rv;
			strcpy_s(rv.host,260,hostname);
			strcpy_s(rv.id,260,idcode);
			_thiss->Reverse.Call_Fnction_Long((char*)&rv,&returnval);
			if (returnval)
			{
				EndDialog(hwnd, TRUE);
			}
			else
			{
				MessageBox(hwnd, 
					"Failed to connect to listening VNC viewer",
					"Outgoing Connection",
					MB_OK | MB_ICONEXCLAMATION );
			}
			return TRUE;
		}

		
			// Cancel the dialog
		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			return TRUE;
		};

		break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		if (_thiss->m_hicon != NULL) {
			DestroyIcon(_thiss->m_hicon);
			_thiss->m_hicon = NULL;
		}
		if (_thiss->m_hfont != NULL) {
			DeleteObject(_thiss->m_hfont);
			_thiss->m_hfont = NULL;
		}
		return TRUE;
	}
	return 0;
}

