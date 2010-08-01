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
		}
		return 0;	

	case WM_SHOWWINDOW:
		SetDlgItemText(hwnd, IDC_S_PRESET1D, preset1D);
			SetDlgItemText(hwnd, IDC_S_PRESET2D, preset2D);
			SetDlgItemText(hwnd, IDC_S_PRESET3D, preset2D);
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