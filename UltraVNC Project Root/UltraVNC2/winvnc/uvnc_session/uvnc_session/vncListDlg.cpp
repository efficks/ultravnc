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

// vncListDlg.cpp

// Implementation of the vncListDlg dialog!

#include "stdafx.h"
#include "vncListDlg.h"
#include "resource.h"
#include "win32_helpers.h"
#include "communication.h"

vncListDlg::vncListDlg()
{
	m_dlgvisible = FALSE;
	if (!KillClient.Init("KillClient",128,0,app,false)) goto error;
	if (!TextChatClient.Init("TextChatClient",128,0,app,false)) goto error;
	if (!ListAuthClients.Init("ListAuthClients",0,sizeof(_clientlist),app,false)) goto error;
	if (!ListUnauthClients.Init("ListUnauthClients",0,sizeof(_clientlist),app,false)) goto error;
	return;
	error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);
}

//
//
//
vncListDlg::~vncListDlg()
{
}

//
//
//
BOOL vncListDlg::Init()
{
	return TRUE;
}

//
//
//
void vncListDlg::Display()
{
	if (!m_dlgvisible)
	{
		// [v1.0.2-jp1 fix] Load resouce from dll
		//DialogBoxParam(	hAppInstance,
		DialogBoxParam(	hInst,
						MAKEINTRESOURCE(IDD_LIST_DLG), 
						NULL,
						(DLGPROC) DialogProc,
						(LONG) this
						);
	}
}

//
//
//
BOOL CALLBACK vncListDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    vncListDlg *_this = helper::SafeGetWindowUserData<vncListDlg>(hwnd);
	switch (uMsg)
	{

	case WM_INITDIALOG:
		{
            helper::SafeSetWindowUserData(hwnd, lParam);
			_this = (vncListDlg *) lParam;

			_clientlist cl;
			HWND hList = GetDlgItem(hwnd, IDC_VIEWERS_LISTBOX);
			_this->ListAuthClients.Call_Fnction(NULL,(char*)&cl);
			for (int j=0;j<cl.size;j++) SendMessage(hList, LB_ADDSTRING,0,(LPARAM) cl.szDescription[j] );
			SendMessage(hList, LB_SETCURSEL, -1, 0);

			// adzm 2009-07-05
			HWND hPendingList = GetDlgItem(hwnd, IDC_PENDING_LISTBOX);
			_clientlist cpl;
			_this->ListUnauthClients.Call_Fnction(NULL,(char*)&cpl);
			for (int j=0;j<cpl.size;j++) SendMessage(hPendingList, LB_ADDSTRING,0,(LPARAM) cpl.szDescription[j] );

			SetForegroundWindow(hwnd);
			_this->m_dlgvisible = TRUE;
			if (bool_GetAllowEditClients)
			{
				EnableWindow(GetDlgItem(hwnd, IDC_KILL_B), true);
			}
			else EnableWindow(GetDlgItem(hwnd, IDC_KILL_B), false);
			return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
			EndDialog(hwnd, TRUE);
			_this->m_dlgvisible = FALSE;
			return TRUE;

		case IDC_KILL_B:
			{
			HWND hList = GetDlgItem(hwnd, IDC_VIEWERS_LISTBOX);
			DWORD nSelected = SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (nSelected != LB_ERR)
			{
				char szClient[128];
				if (SendMessage(hList, LB_GETTEXT, nSelected, (LPARAM)szClient) > 0)
					_this->KillClient.Call_Fnction(szClient,NULL);
			}
			EndDialog(hwnd, TRUE);
			_this->m_dlgvisible = FALSE;
			return TRUE;
			}
			break;

		case IDC_TEXTCHAT_B:
			{
			HWND hList = GetDlgItem(hwnd, IDC_VIEWERS_LISTBOX);
			DWORD nSelected = SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (nSelected != LB_ERR)
			{
				char szClient[128];
				if (SendMessage(hList, LB_GETTEXT, nSelected, (LPARAM)szClient) > 0)
					_this->TextChatClient.Call_Fnction(szClient,NULL);
			}
			EndDialog(hwnd, TRUE);
			_this->m_dlgvisible = FALSE;
			return TRUE;
			}
			break;

		}
		break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		_this->m_dlgvisible = FALSE;
		return TRUE;
	}
	return 0;
}
