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
#include "../../winvnc/winvnc/controlethread.h"
extern char preset1[128];
extern char preset1D[128];
extern char preset1ID[128];
extern char preset2[128];
extern char preset2D[128];
extern char preset2ID[128];
extern char preset3[128];
extern char preset3D[128];
extern char preset3ID[128];
extern controlethread MTC;
extern bool server_online;
extern LONG autorecon;

BOOL CALLBACK DlgProcSC(HWND hwnd, UINT uMsg,
											   WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg) {
		
	case WM_INITDIALOG: 
		{	
			SetDlgItemText(hwnd, IDC_S_PRESET1D, preset1D);
			SetDlgItemText(hwnd, IDC_S_PRESET2D, preset2D);
			SetDlgItemText(hwnd, IDC_S_PRESET3D, preset3D);
			if (strlen(preset1D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET1), false);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET1), true);
			if (strlen(preset2D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET2), false);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET2), true);
			if (strlen(preset3D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET3), false);
			else 
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET3), true);


			return TRUE;
		}
	
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{	
		case IDOK:	
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			return TRUE;
		case IDC_B_PRESET1:
			{
			compack bufpack;
			if (autorecon) bufpack.command=6;
			else  bufpack.command=3;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			}
			break;
		case IDC_B_PRESET2:
			{
			compack bufpack;
			if (autorecon) bufpack.command=7;
			else  bufpack.command=4;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			}
			break;
		case IDC_B_PRESET3:
			{
			compack bufpack;
			if (autorecon) bufpack.command=8;
			else  bufpack.command=5;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			}
			break;

		case IDC_DISRECON:
			{
			compack bufpack;
			bufpack.command=9;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			}
			break;

		}
		return 0;	

	case WM_SHOWWINDOW:
			SetDlgItemText(hwnd, IDC_S_PRESET1D, preset1D);
			SetDlgItemText(hwnd, IDC_S_PRESET2D, preset2D);
			SetDlgItemText(hwnd, IDC_S_PRESET3D, preset3D);
			if (strlen(preset1D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET1), false);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET1), true);
			if (strlen(preset2D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET2), false);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET2), true);
			if (strlen(preset3D)==0)
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET3), false);
			else 
				EnableWindow(GetDlgItem(hwnd, IDC_B_PRESET3), true);
		break;
	}

	return 0;
}