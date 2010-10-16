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
// uvnc_settings.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <shellapi.h>
#include <string>
#include "uvnc_settings.h"
#include <commctrl.h>
#include "inifile.h"
#include "resource.h"
#include "vncOSVersion.h"
#include "../../winvnc/winvnc/controlethread.h"

int install_service(void);
int uninstall_service(void);


#define MAX_LOADSTRING 100
#define MAXPWLEN 8

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcOptions1(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcOptions6(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK security(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcOFT(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcCON(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcCAP(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcMISC(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcEncryption(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcSC(HWND hwnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
HWND m_hParent;
HWND m_hTab;
HWND hTab0dialog,hTab1dialog,hTab2dialog,hTab3dialog,hTab4dialog,hTab5dialog,hTab6dialog,hTab7dialog,hTab8dialog;

TCHAR *group1=new char[150];
TCHAR *group2=new char[150];
TCHAR *group3=new char[150];
LONG BUseRegistry;
LONG MSLogonRequired;
LONG NewMSLogon;
LONG locdom1;
LONG locdom2;
LONG locdom3;
LONG DebugMode=2;
LONG Avilog=0;
LONG DebugLevel=10;
LONG DisableTrayIcon=0;
LONG LoopbackOnly=0;
LONG UseDSMPlugin=0;
LONG AllowLoopback=1;
LONG promptv=0;
LONG autorecon=0;
LONG AuthRequired=1;
LONG ConnectPriority;

char DSMPlugin[128];
char authhosts[150]= ("\0");

char DSMPluginConfig[512]= ("\0");

LONG AllowShutdown=1;
LONG AllowProperties=1;
LONG AllowEditClients=1;

LONG FileTransferEnabled=0;
LONG FTUserImpersonation;
LONG BlankMonitorEnabled=0;
LONG BlankInputsOnly=0;
LONG DefaultScale=1;
LONG CaptureAlphaBlending=1;
LONG BlackAlphaBlending=1;
LONG FTTimeout = 30;

LONG SocketConnect=0;
LONG HTTPConnect=1;
LONG XDMCPConnect;
LONG AutoPortSelect=1;
LONG PortNumber=5900;
LONG HttpPortNumber;
LONG IdleTimeout;

LONG RemoveWallpaper=1;
LONG RemoveAero=1;

LONG QuerySetting=1;
LONG QueryTimeout=10;
LONG QueryAccept;
LONG QueryIfNoLogon;

LONG EnableRemoteInputs=1;
LONG LockSettings=0;
LONG DisableLocalInputs=0;
LONG EnableJapInput=0;
char passwd[9];
char passwd2[9];

LONG TurboMode=1;
LONG PollUnderCursor=0;
LONG PollForeground=0;
LONG PollFullScreen=0;
LONG PollConsoleOnly=0;
LONG PollOnEventOnly=0;
LONG Driver=0;
LONG Hook=1;
LONG Virtual;
LONG SingleWindow=0;
char SingleWindowName[32];
char path[MAX_PATH];

LONG kickrdp=0;
char servicecmdline[256]=("\0");

LONG Primary=1;
LONG Secundary=0;

char preset1[128];
char preset1D[128];
char preset1ID[128];
char preset2[128];
char preset2D[128];
char preset2ID[128];
char preset3[128];
char preset3D[128];
char preset3ID[128];

int vncEncryptPasswd(char *passwd, char *encryptedPasswd);

void Save_settings()
{
IniFile myIniFile_Out;
myIniFile_Out.IniFileSetSecure();
if (! myIniFile_Out.WriteInt("admin", "UseRegistry", BUseRegistry))
{
	MessageBox(NULL,"Permission denied, can't save to ultravnc.ini","Error",0);
	return;
}
myIniFile_Out.WriteInt("admin", "MSLogonRequired", MSLogonRequired);
myIniFile_Out.WriteInt("admin", "NewMSLogon", NewMSLogon);
myIniFile_Out.WriteInt("admin", "DebugMode", DebugMode);
myIniFile_Out.WriteInt("admin", "Avilog", Avilog);
myIniFile_Out.WriteString("admin", "path", path);

myIniFile_Out.WriteInt("admin", "kickrdp", kickrdp);
myIniFile_Out.WriteString("admin", "service_commandline", servicecmdline);

myIniFile_Out.WriteInt("admin", "DebugLevel", DebugLevel);
myIniFile_Out.WriteInt("admin", "DisableTrayIcon", DisableTrayIcon);
myIniFile_Out.WriteInt("admin", "LoopbackOnly", LoopbackOnly);
myIniFile_Out.WriteInt("admin", "UseDSMPlugin", UseDSMPlugin);
myIniFile_Out.WriteInt("admin", "AllowLoopback", AllowLoopback);
myIniFile_Out.WriteInt("admin", "SC_PROMPT", promptv);
myIniFile_Out.WriteInt("admin", "SC_AUTORECON", autorecon);
myIniFile_Out.WriteInt("admin", "AuthRequired", AuthRequired);
myIniFile_Out.WriteInt("admin", "ConnectPriority", ConnectPriority);
myIniFile_Out.WriteString("admin", "DSMPlugin",DSMPlugin);
myIniFile_Out.WriteString("admin", "DSMPluginConfig",DSMPluginConfig);
myIniFile_Out.WriteString("admin", "AuthHosts",authhosts);
myIniFile_Out.WriteInt("admin", "AllowShutdown" ,AllowShutdown);
myIniFile_Out.WriteInt("admin", "AllowProperties" ,AllowProperties);
myIniFile_Out.WriteInt("admin", "AllowEditClients" ,AllowEditClients);
myIniFile_Out.WriteInt("admin", "FileTransferEnabled", FileTransferEnabled);
myIniFile_Out.WriteInt("admin", "FTUserImpersonation", FTUserImpersonation);
myIniFile_Out.WriteInt("admin", "BlankMonitorEnabled", BlankMonitorEnabled);
myIniFile_Out.WriteInt("admin", "BlankInputsOnly",BlankInputsOnly );
myIniFile_Out.WriteInt("admin", "DefaultScale", DefaultScale);
myIniFile_Out.WriteInt("admin", "CaptureAlphaBlending", CaptureAlphaBlending);
myIniFile_Out.WriteInt("admin", "BlackAlphaBlending", BlackAlphaBlending);
myIniFile_Out.WriteInt("admin", "SocketConnect", SocketConnect);
myIniFile_Out.WriteInt("admin", "HTTPConnect", HTTPConnect);
myIniFile_Out.WriteInt("admin", "XDMCPConnect", XDMCPConnect);
myIniFile_Out.WriteInt("admin", "AutoPortSelect", AutoPortSelect);
myIniFile_Out.WriteInt("admin", "PortNumber", PortNumber);
myIniFile_Out.WriteInt("admin", "HTTPPortNumber", HttpPortNumber);
myIniFile_Out.WriteInt("admin", "IdleTimeout", IdleTimeout);
myIniFile_Out.WriteInt("admin", "RemoveWallpaper", RemoveWallpaper);
myIniFile_Out.WriteInt("admin", "RemoveAero", RemoveAero);
myIniFile_Out.WriteInt("admin", "QuerySetting", QuerySetting);
myIniFile_Out.WriteInt("admin", "QueryTimeout", QueryTimeout);
myIniFile_Out.WriteInt("admin", "QueryAccept", QueryAccept);
myIniFile_Out.WriteInt("admin", "QueryIfNoLogon", QueryIfNoLogon);

myIniFile_Out.WriteInt("admin", "primary", Primary);
myIniFile_Out.WriteInt("admin", "secundary", Secundary);


myIniFile_Out.WritePassword(passwd);
myIniFile_Out.WritePassword2(passwd2);
myIniFile_Out.WriteInt("admin", "InputsEnabled", EnableRemoteInputs);
myIniFile_Out.WriteInt("admin", "LockSetting", LockSettings);
myIniFile_Out.WriteInt("admin", "LocalInputsDisabled", DisableLocalInputs);	
myIniFile_Out.WriteInt("admin", "EnableJapInput", EnableJapInput);	
myIniFile_Out.WriteInt("poll", "TurboMode", TurboMode);
myIniFile_Out.WriteInt("poll", "PollUnderCursor", PollUnderCursor);
myIniFile_Out.WriteInt("poll", "PollForeground", PollForeground);
myIniFile_Out.WriteInt("poll", "PollFullScreen", PollFullScreen);
myIniFile_Out.WriteInt("poll", "OnlyPollConsole",PollConsoleOnly);
myIniFile_Out.WriteInt("poll", "OnlyPollOnEvent", PollOnEventOnly);
myIniFile_Out.WriteInt("poll", "EnableDriver", Driver);
myIniFile_Out.WriteInt("poll", "EnableHook", Hook);
myIniFile_Out.WriteInt("poll", "EnableVirtual", Virtual);
myIniFile_Out.WriteInt("poll", "SingleWindow", SingleWindow);
myIniFile_Out.WriteString("poll", "SingleWindowName", SingleWindowName);
myIniFile_Out.WriteInt("admin", "FileTransferTimeout", FTTimeout);

myIniFile_Out.WriteString("presets", "preset1", preset1);
myIniFile_Out.WriteString("presets", "preset1D", preset1D);
myIniFile_Out.WriteString("presets", "preset1ID", preset1ID);

myIniFile_Out.WriteString("presets", "preset2", preset2);
myIniFile_Out.WriteString("presets", "preset2D", preset2D);
myIniFile_Out.WriteString("presets", "preset2ID", preset2ID);

myIniFile_Out.WriteString("presets", "preset3", preset3);
myIniFile_Out.WriteString("presets", "preset3D", preset3D);
myIniFile_Out.WriteString("presets", "preset3ID", preset3ID);

}

void Read_Settings_from_ini()
{
memset(passwd,0,MAXPWLEN);
memset(passwd2,0,MAXPWLEN);
IniFile myIniFile_In;
kickrdp=myIniFile_In.ReadInt("admin", "kickrdp", kickrdp);
myIniFile_In.ReadString("admin", "service_commandline",servicecmdline,256);
myIniFile_In.IniFileSetSecure();
BUseRegistry = myIniFile_In.ReadInt("admin", "UseRegistry", 0);
MSLogonRequired=myIniFile_In.ReadInt("admin", "MSLogonRequired", false);
NewMSLogon=myIniFile_In.ReadInt("admin", "NewMSLogon", false);
myIniFile_In.ReadString("admin_auth","group1",group1,150);
myIniFile_In.ReadString("admin_auth","group2",group2,150);
myIniFile_In.ReadString("admin_auth","group3",group3,150);
locdom1=myIniFile_In.ReadInt("admin_auth", "locdom1",0);
locdom2=myIniFile_In.ReadInt("admin_auth", "locdom2",0);
locdom3=myIniFile_In.ReadInt("admin_auth", "locdom3",0);
DebugMode=myIniFile_In.ReadInt("admin", "DebugMode", 0);
Avilog=myIniFile_In.ReadInt("admin", "Avilog", 0);
myIniFile_In.ReadString("admin", "path", path,512);
DebugLevel=myIniFile_In.ReadInt("admin", "DebugLevel", 0);
DisableTrayIcon=myIniFile_In.ReadInt("admin", "DisableTrayIcon", false);
LoopbackOnly=myIniFile_In.ReadInt("admin", "LoopbackOnly", false);
UseDSMPlugin=myIniFile_In.ReadInt("admin", "UseDSMPlugin", false);
AllowLoopback=myIniFile_In.ReadInt("admin", "AllowLoopback", false);
promptv=myIniFile_In.ReadInt("admin", "SC_PROMPT", false);
autorecon=myIniFile_In.ReadInt("admin", "SC_AUTORECON", false);
AuthRequired=myIniFile_In.ReadInt("admin", "AuthRequired", true);
ConnectPriority=myIniFile_In.ReadInt("admin", "ConnectPriority", 0);
myIniFile_In.ReadString("admin", "DSMPlugin",DSMPlugin,128);
myIniFile_In.ReadString("admin", "DSMPluginConfig",DSMPluginConfig,512);
myIniFile_In.ReadString("admin", "AuthHosts",authhosts,150);
AllowShutdown=myIniFile_In.ReadInt("admin", "AllowShutdown", true);
AllowProperties=myIniFile_In.ReadInt("admin", "AllowProperties", true);
AllowEditClients=myIniFile_In.ReadInt("admin", "AllowEditClients", true);
FileTransferEnabled=myIniFile_In.ReadInt("admin", "FileTransferEnabled", true);
FTUserImpersonation=myIniFile_In.ReadInt("admin", "FTUserImpersonation", true);
BlankMonitorEnabled = myIniFile_In.ReadInt("admin", "BlankMonitorEnabled", true);
BlankInputsOnly = myIniFile_In.ReadInt("admin", "BlankInputsOnly", false);
DefaultScale = myIniFile_In.ReadInt("admin", "DefaultScale", 1);
CaptureAlphaBlending = myIniFile_In.ReadInt("admin", "CaptureAlphaBlending", false); // sf@2005
BlackAlphaBlending = myIniFile_In.ReadInt("admin", "BlackAlphaBlending", false); // sf@2005
FTTimeout = myIniFile_In.ReadInt("admin", "FileTransferTimeout", 30);
SocketConnect=myIniFile_In.ReadInt("admin", "SocketConnect", true);
HTTPConnect=myIniFile_In.ReadInt("admin", "HTTPConnect", true);
XDMCPConnect=myIniFile_In.ReadInt("admin", "XDMCPConnect", true);
AutoPortSelect=myIniFile_In.ReadInt("admin", "AutoPortSelect", true);
PortNumber=myIniFile_In.ReadInt("admin", "PortNumber", 0);
HttpPortNumber=myIniFile_In.ReadInt("admin", "HTTPPortNumber",0);
IdleTimeout=myIniFile_In.ReadInt("admin", "IdleTimeout", 0);	
RemoveWallpaper=myIniFile_In.ReadInt("admin", "RemoveWallpaper", 0);
RemoveAero=myIniFile_In.ReadInt("admin", "RemoveAero", 0);
QuerySetting=myIniFile_In.ReadInt("admin", "QuerySetting", 0);
QueryTimeout=myIniFile_In.ReadInt("admin", "QueryTimeout", 0);
QueryAccept=myIniFile_In.ReadInt("admin", "QueryAccept", 0);
QueryIfNoLogon=myIniFile_In.ReadInt("admin", "QueryIfNoLogon", 0);
myIniFile_In.ReadPassword(passwd,MAXPWLEN);
myIniFile_In.ReadPassword2(passwd2,MAXPWLEN);
passwd[8] = 0;
passwd2[8] = 0;
if (AuthRequired==1 && strlen(passwd)==0 && strlen(passwd2)==0)
{
	MessageBox(NULL,"Authentication error: You enabled authentication but no password have been set for <full> and <view only>","Error",MB_ICONERROR);
}
else if (AuthRequired==1 && strlen(passwd)==0 )
{
	MessageBox(NULL,"Authentication error: You enabled authentication but no password have been set for <full>","Error",MB_ICONERROR);
}
else if (AuthRequired==1 && strlen(passwd2)==0)
{
	MessageBox(NULL,"Authentication error: You enabled authentication but no password have been set for <view only>","Error",MB_ICONERROR);
}

EnableRemoteInputs=myIniFile_In.ReadInt("admin", "InputsEnabled", 0);
LockSettings=myIniFile_In.ReadInt("admin", "LockSetting", 0);
DisableLocalInputs=myIniFile_In.ReadInt("admin", "LocalInputsDisabled", 0);
EnableJapInput=myIniFile_In.ReadInt("admin", "EnableJapInput", 0);

Primary=myIniFile_In.ReadInt("admin", "primary", true);
Secundary=myIniFile_In.ReadInt("admin", "secundary", false);

TurboMode = myIniFile_In.ReadInt("poll", "TurboMode", 0);
PollUnderCursor=myIniFile_In.ReadInt("poll", "PollUnderCursor", 0);
PollForeground=myIniFile_In.ReadInt("poll", "PollForeground", 0);
PollFullScreen=myIniFile_In.ReadInt("poll", "PollFullScreen", 0);
PollConsoleOnly=myIniFile_In.ReadInt("poll", "OnlyPollConsole", 0);
PollOnEventOnly=myIniFile_In.ReadInt("poll", "OnlyPollOnEvent", 0);
Driver=myIniFile_In.ReadInt("poll", "EnableDriver", 0);
Hook=myIniFile_In.ReadInt("poll", "EnableHook", 0);
Virtual=myIniFile_In.ReadInt("poll", "EnableVirtual", 0);
SingleWindow=myIniFile_In.ReadInt("poll","SingleWindow",SingleWindow);
myIniFile_In.ReadString("poll", "SingleWindowName", SingleWindowName,32);

strcpy(preset1,"");
strcpy(preset1D,"");
strcpy(preset1ID,"");
myIniFile_In.ReadString("presets", "preset1", preset1,128);
myIniFile_In.ReadString("presets", "preset1D", preset1D,128);
myIniFile_In.ReadString("presets", "preset1ID", preset1ID,128);

strcpy(preset2,"");
strcpy(preset2D,"");
strcpy(preset2ID,"");
myIniFile_In.ReadString("presets", "preset2", preset2,128);
myIniFile_In.ReadString("presets", "preset2D", preset2D,128);
myIniFile_In.ReadString("presets", "preset2ID", preset2ID,128);

strcpy(preset3,"");
strcpy(preset3D,"");
strcpy(preset3ID,"");
myIniFile_In.ReadString("presets", "preset3", preset3,128);
myIniFile_In.ReadString("presets", "preset3D", preset3D,128);
myIniFile_In.ReadString("presets", "preset3ID", preset3ID,128);
}

DWORD WINAPI upnpthread( LPVOID lpParam );
bool fShutdownOrdered=false;
controlethread MTC;
bool server_online=false;
bool running_as_admin=false;
bool write_permission=false;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	if (!MTC.WSAInit()) return 0;
	if (MTC.SetupConnect()) server_online=true;
	SC_HANDLE scm=OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if(!scm) 
			{
				running_as_admin=false;
			}
			if (scm)
			{
				running_as_admin=true;
				CloseServiceHandle(scm);
			}

	IniFile myIniFile_Out;
	myIniFile_Out.IniFileSetSecure();
	if (myIniFile_Out.WriteInt("admin", "dummy", 0)) write_permission=true;

	SetOSVersion();
	InitCommonControls(); 
	INITCOMMONCONTROLSEX icex;
    memset(&icex, 0x0, sizeof(INITCOMMONCONTROLSEX));
    icex.dwSize = sizeof (INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES;
    InitCommonControlsEx (&icex);
	


	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	hInst=hInstance;
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_UVNC_SETTINGS, szWindowClass, MAX_LOADSTRING);
	Read_Settings_from_ini();
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PARENT), 
							NULL, (DLGPROC) DlgProc, NULL);
	fShutdownOrdered=true;
	
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg,
										WPARAM wParam, LPARAM lParam)
{
	// We use the dialog-box's USERDATA to store a _this pointer
	// This is set only once WM_INITDIALOG has been recieved, though!
//	VNCOptions *_this = (VNCOptions *) GetWindowLong(hwndDlg, GWL_USERDATA);

	switch (uMsg) {
	case WM_TIMER:
		{
		char text[128];
		compack bufpack;
		bufpack.command=0;
		bufpack.version=100;
		if (server_online==false)if (MTC.SetupConnect()) server_online=true;
		if (server_online)
			{
			compack bufpack;
			bufpack.command=1;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			if (!MTC.read(&bufpack)) server_online=false;
			else PortNumber=bufpack.int_value;

			bufpack.command=2;
			bufpack.version=100;
			if (!MTC.write(&bufpack)) server_online=false;
			if (!MTC.read(&bufpack)) server_online=false;
			else HttpPortNumber=bufpack.int_value;
			
			if (!MTC.write(&bufpack)) server_online=false;
			if (!MTC.read(&bufpack)) server_online=false;
		}
			if (server_online) strcpy(text,"vnc running YES - ");
			if (!server_online) strcpy(text,"vnc running NO - ");
			if (running_as_admin) strcat(text,"service access YES - ");
			if (!running_as_admin) strcat(text,"service access NO - ");
			if (write_permission) strcat(text,"save settings YES - ");
			if (!write_permission) strcat(text,"save settings NO - ");
			if (!AuthRequired) strcat(text," Authentication OFF ");
			if (AuthRequired) strcat(text," Authentication ON ");
			SetWindowText(hwndDlg,text);
		}
		break;
	case WM_INITDIALOG: 
		{
			InitCommonControls();
			SetTimer(hwndDlg,100,5000,NULL);
			m_hParent = hwndDlg;
			m_hTab = GetDlgItem(hwndDlg, IDC_TAB);

			TCITEM item;
			item.mask = TCIF_TEXT; 
			if (write_permission)
			{
			item.pszText="Network";
			TabCtrl_InsertItem(m_hTab, 0, &item);
			item.pszText = "Authentication";
			TabCtrl_InsertItem(m_hTab, 1, &item);
			item.pszText = "Input/FileTransfer";
			TabCtrl_InsertItem(m_hTab, 2, &item);
			item.pszText = "Connections";
			TabCtrl_InsertItem(m_hTab, 3, &item);
			item.pszText = "Screen Capture";
			TabCtrl_InsertItem(m_hTab, 4, &item);
			item.pszText = "Misc/logging";
			TabCtrl_InsertItem(m_hTab, 5, &item);
			item.pszText = "UPnP";
			TabCtrl_InsertItem(m_hTab, 6, &item);
			item.pszText = "Encryption";
			TabCtrl_InsertItem(m_hTab, 7, &item);
			}
			item.pszText = "SC connect";
			TabCtrl_InsertItem(m_hTab, 8, &item);

			hTab0dialog = CreateDialogParam(hInst,
				MAKEINTRESOURCE(IDD_OPTION1),
				hwndDlg,
				(DLGPROC)DlgProcOptions1,
				NULL);

			hTab1dialog = CreateDialogParam(hInst, 
				MAKEINTRESOURCE(IDD_OPTION2),
				hwndDlg,
				(DLGPROC)security,
				NULL);

			hTab2dialog = CreateDialogParam(hInst, 
				MAKEINTRESOURCE(IDD_OPTION3),
				hwndDlg,
				(DLGPROC)DlgProcOFT,
				NULL);
			hTab3dialog = CreateDialogParam(hInst, 
				MAKEINTRESOURCE(IDD_OPTION4),
				hwndDlg,
				(DLGPROC)DlgProcCON,
				NULL);

			hTab4dialog = CreateDialogParam(hInst, 
				MAKEINTRESOURCE(IDD_OPTION5),
				hwndDlg,
				(DLGPROC)DlgProcCAP,
				NULL);

			hTab5dialog = CreateDialogParam(hInst, 
				MAKEINTRESOURCE(IDD_OPTION7),
				hwndDlg,
				(DLGPROC)DlgProcMISC,
				NULL);
			hTab6dialog = CreateDialogParam(hInst,
				MAKEINTRESOURCE(IDD_OPTION6),
				hwndDlg,
				(DLGPROC)DlgProcOptions6,
				NULL);

			hTab7dialog = CreateDialogParam(hInst,
				MAKEINTRESOURCE(IDD_OPTION8),
				hwndDlg,
				(DLGPROC)DlgProcEncryption,
				NULL);

			hTab8dialog = CreateDialogParam(hInst,
				MAKEINTRESOURCE(IDD_OPTION9),
				hwndDlg,
				(DLGPROC)DlgProcSC,
				NULL);

			// Position child dialogs, to fit the Tab control's display area
			RECT rc;
			GetWindowRect(m_hTab, &rc);
			MapWindowPoints(NULL, hwndDlg, (POINT *)&rc, 2);
			TabCtrl_AdjustRect(m_hTab, FALSE, &rc);
			SetWindowPos(hTab0dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_SHOWWINDOW);
			SetWindowPos(hTab1dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab2dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab3dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab4dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab5dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab6dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab7dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SetWindowPos(hTab8dialog, HWND_TOP, rc.left, rc.top,
						 rc.right - rc.left, rc.bottom - rc.top,
						 SWP_HIDEWINDOW);
			SC_HANDLE scm=OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
			if(!scm) 
			{
				if (write_permission) SetWindowText(GetDlgItem(hwndDlg, IDC_INFO),"Run as admin for service access");
				else SetWindowText(GetDlgItem(hwndDlg, IDC_INFO),"Run as admin for write/service access");
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON5), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON6), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON7), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON8), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_SERVICE), SW_HIDE);
			}
			if (scm)
			{
				SetWindowText(GetDlgItem(hwndDlg, IDC_INFO),"");
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON5), SW_SHOW);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON6), SW_SHOW);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON7), SW_SHOW);
				ShowWindow(GetDlgItem(hwndDlg, IDC_BUTTON8), SW_SHOW);
				ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_SERVICE), SW_SHOW);
			}
			CloseServiceHandle(scm);
			char text[128];
			if (server_online) strcpy(text,"vnc running YES - ");
			if (!server_online) strcpy(text,"vnc running NO - ");
			if (running_as_admin) strcat(text,"service access YES - ");
			if (!running_as_admin) strcat(text,"service access NO - ");
			if (write_permission) strcat(text,"save settings YES - ");
			if (!write_permission) strcat(text,"save settings NO - ");
			if (!AuthRequired) strcat(text," Authentication OFF ");
			if (AuthRequired) strcat(text," Authentication ON ");
			SetWindowText(hwndDlg,text);
			return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_BUTTON1:
			ShellExecute(NULL, "open", "http://www.uvnc.com", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDC_BUTTON2:
			ShellExecute(NULL, "open", "http://doc.uvnc.com", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDC_BUTTON3:
			ShellExecute(NULL, "open", "http://forum.uvnc.com", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDC_BUTTON5:
			install_service();
			break;
		case IDC_BUTTON6:
			uninstall_service();
			break;
		case IDC_BUTTON7:
			{
			char command[MAX_PATH + 32]; // 29 January 2008 jdp 
			_snprintf(command, sizeof command, "net start \"%s\"", "uvnc_service");
			WinExec(command,SW_HIDE);
			}
			break;
		case IDC_BUTTON8:
			{
			char command[MAX_PATH + 32]; // 29 January 2008 jdp 
			_snprintf(command, sizeof command, "net stop \"%s\"", "uvnc_service");
			WinExec(command,SW_HIDE);
			}
			break;
		case IDOK:
			///SetFocus(GetDlgItem(hwndDlg, IDOK));
			SendMessage(hTab0dialog, WM_COMMAND, IDOK, 0);
			SendMessage(hTab1dialog, WM_COMMAND, IDOK, 0);
			SendMessage(hTab2dialog, WM_COMMAND, IDOK, 0);
			SendMessage(hTab3dialog, WM_COMMAND, IDOK, 0);
			SendMessage(hTab4dialog, WM_COMMAND, IDOK, 0);
			SendMessage(hTab5dialog, WM_COMMAND, IDOK, 0);	
			SendMessage(hTab6dialog, WM_COMMAND, IDOK, 0);	
			SendMessage(hTab7dialog, WM_COMMAND, IDOK, 0);	
			SendMessage(hTab8dialog, WM_COMMAND, IDOK, 0);	
			//SendMessage(_this->m_hPageGeneral, WM_COMMAND, IDC_OK, 0);
			//DestroyWindow(hTab0dialog);
			//DestroyWindow(hTab1dialog);
			//DestroyWindow(hTab2dialog);
			//DestroyWindow(hTab3dialog);
			//DestroyWindow(hTab4dialog);
			//DestroyWindow(hTab5dialog);
			Save_settings();
			//EndDialog(hwndDlg, TRUE);
			return TRUE;
		case IDCANCEL:			
			SendMessage(hTab0dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab1dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab2dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab3dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab4dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab5dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab6dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab7dialog, WM_COMMAND, IDCANCEL, 0);
			SendMessage(hTab8dialog, WM_COMMAND, IDCANCEL, 0);
			
			DestroyWindow(hTab0dialog);
			DestroyWindow(hTab1dialog);
			DestroyWindow(hTab2dialog);
			DestroyWindow(hTab3dialog);
			DestroyWindow(hTab4dialog);
			DestroyWindow(hTab5dialog);
			DestroyWindow(hTab6dialog);
			DestroyWindow(hTab7dialog);
			DestroyWindow(hTab8dialog);
			
			EndDialog(hwndDlg, FALSE);
			return TRUE;
		}
		return FALSE;

	case WM_NOTIFY:
		{
			LPNMHDR pn = (LPNMHDR)lParam;
			switch (pn->code) {		
			case TCN_SELCHANGE:
				switch (pn->idFrom) {
				case IDC_TAB:
					int i = TabCtrl_GetCurFocus(m_hTab);
					switch (i) {
					case 0:
						ShowWindow(hTab0dialog, SW_SHOW);
						SetFocus(hTab0dialog);
						return 0;
					case 1:
						ShowWindow(hTab1dialog, SW_SHOW);
						SetFocus(hTab1dialog);
						return 0;
					case 2:
						ShowWindow(hTab2dialog, SW_SHOW);
						SetFocus(hTab2dialog);
						return 0;
					case 3:
						ShowWindow(hTab3dialog, SW_SHOW);
						SetFocus(hTab3dialog);
						return 0;
					case 4:
						ShowWindow(hTab4dialog, SW_SHOW);
						SetFocus(hTab4dialog);
						return 0;
					case 5:
						ShowWindow(hTab5dialog, SW_SHOW);
						SetFocus(hTab5dialog);
						return 0;
					case 6:
						ShowWindow(hTab6dialog, SW_SHOW);
						SetFocus(hTab6dialog);
						return 0;
					case 7:
						ShowWindow(hTab7dialog, SW_SHOW);
						SetFocus(hTab7dialog);
						return 0;
					case 8:
						ShowWindow(hTab8dialog, SW_SHOW);
						SetFocus(hTab8dialog);
						return 0;
					}
					return 0;
				}
				return 0;
			case TCN_SELCHANGING:
				switch (pn->idFrom) {
				case IDC_TAB:
					int i = TabCtrl_GetCurFocus(m_hTab);
					switch (i) {
					case 0:
						ShowWindow(hTab0dialog, SW_HIDE);
						break;
					case 1:
						ShowWindow(hTab1dialog, SW_HIDE);
						break;
					case 2:
						ShowWindow(hTab2dialog, SW_HIDE);
						break;
					case 3:
						ShowWindow(hTab3dialog, SW_HIDE);
						break;
					case 4:
						ShowWindow(hTab4dialog, SW_HIDE);
						break;
					case 5:
						ShowWindow(hTab5dialog, SW_HIDE);
						break;
					case 6:
						ShowWindow(hTab6dialog, SW_HIDE);
						break;
					case 7:
						ShowWindow(hTab7dialog, SW_HIDE);
						break;
					case 8:
						ShowWindow(hTab8dialog, SW_HIDE);
						break;
					}
					return 0;
				}
				return 0;
			}
			return 0;
		}
			
	}
    return 0;
}