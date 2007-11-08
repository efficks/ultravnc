#include "stdhdrs.h"
#include "inifile.h"

//  We first use shellexecute with "runas"
//  This way we can use UAC and user/passwd
//	Runas is standard OS, so no security risk

const char winvncSettings[]				= "-settings";
const char winvncStopservice[]			= "-stopservice";
const char winvncStartservice[]			= "-startservice";
const char winvncInstallService[]		= "-install";
const char winvncUnInstallService[]		= "-uninstall";

extern char service_name[];

/////////////////////////////////////////////////////////////////////
void
Set_settings_as_admin(char *mycommand)
{
	char exe_file_name[MAX_PATH];
	char commanline[MAX_PATH];
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	strcpy(commanline, winvncSettings);
	strcat(commanline, ":");
	strcat(commanline, mycommand);

	SHELLEXECUTEINFO shExecInfo;
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = commanline;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);
}


void Copy_to_Secure_from_temp(char *lpCmdLine)
{
IniFile myIniFile_In;
IniFile myIniFile_Out;
myIniFile_Out.IniFileSetSecure();
myIniFile_In.IniFileSetTemp(lpCmdLine);

TCHAR *group1=new char[150];
TCHAR *group2=new char[150];
TCHAR *group3=new char[150];
BOOL BUseRegistry;
LONG MSLogonRequired;
LONG NewMSLogon;
LONG locdom1;
LONG locdom2;
LONG locdom3;
LONG DebugMode=2;
LONG DebugLevel=10;
LONG DisableTrayIcon;
LONG LoopbackOnly;
LONG UseDSMPlugin;
LONG AllowLoopback;
LONG AuthRequired;
LONG ConnectPriority;

char DSMPlugin[128];
char *authhosts=new char[150];

LONG AllowShutdown=1;
LONG AllowProperties=1;
LONG AllowEditClients=1;

LONG FileTransferEnabled;
LONG FTUserImpersonation;
LONG BlankMonitorEnabled=1;
LONG DefaultScale=1;
LONG CaptureAlphaBlending=1;
LONG BlackAlphaBlending=1;

LONG SocketConnect=1;
LONG HTTPConnect;
LONG XDMCPConnect;
LONG AutoPortSelect=1;
LONG PortNumber;
LONG HttpPortNumber;
LONG IdleTimeout;

LONG RemoveWallpaper=1;
LONG RemoveEaro=1;

LONG QuerySetting=1;
LONG QueryTimeout=10;
LONG QueryAccept;
LONG QueryIfNoLogon;

LONG EnableRemoteInputs=1;
LONG LockSettings;
LONG DisableLocalInputs;

#define MAXPWLEN 8
char passwd[MAXPWLEN];

LONG TurboMode=1;
LONG PollUnderCursor=0;
LONG PollForeground=1;
LONG PollFullScreen=1;
LONG PollConsoleOnly=0;
LONG PollOnEventOnly=0;
LONG Driver=0;
LONG Hook=1;
LONG Virtual;

BUseRegistry = myIniFile_In.ReadInt("admin", "UseRegistry", 0);
myIniFile_Out.WriteInt("admin", "UseRegistry", BUseRegistry);

MSLogonRequired=myIniFile_In.ReadInt("admin", "MSLogonRequired", false);
myIniFile_Out.WriteInt("admin", "MSLogonRequired", MSLogonRequired);
NewMSLogon=myIniFile_In.ReadInt("admin", "NewMSLogon", false);
myIniFile_Out.WriteInt("admin", "NewMSLogon", NewMSLogon);


myIniFile_In.ReadString("admin_auth","group1",group1,150);
myIniFile_In.ReadString("admin_auth","group2",group2,150);
myIniFile_In.ReadString("admin_auth","group3",group3,150);
myIniFile_Out.WriteString("admin_auth", "group1",group1);
myIniFile_Out.WriteString("admin_auth", "group2",group2);
myIniFile_Out.WriteString("admin_auth", "group3",group3);



locdom1=myIniFile_In.ReadInt("admin_auth", "locdom1",0);
locdom2=myIniFile_In.ReadInt("admin_auth", "locdom2",0);
locdom3=myIniFile_In.ReadInt("admin_auth", "locdom3",0);
myIniFile_Out.WriteInt("admin_auth", "locdom1", locdom1);
myIniFile_Out.WriteInt("admin_auth", "locdom2", locdom2);
myIniFile_Out.WriteInt("admin_auth", "locdom3", locdom3);


DebugMode=myIniFile_In.ReadInt("admin", "DebugMode", 0);
DebugLevel=myIniFile_In.ReadInt("admin", "DebugLevel", 0);
DisableTrayIcon=myIniFile_In.ReadInt("admin", "DisableTrayIcon", false);
LoopbackOnly=myIniFile_In.ReadInt("admin", "LoopbackOnly", false);

myIniFile_Out.WriteInt("admin", "DebugMode", DebugMode);
myIniFile_Out.WriteInt("admin", "DebugLevel", DebugLevel);
myIniFile_Out.WriteInt("admin", "DisableTrayIcon", DisableTrayIcon);
myIniFile_Out.WriteInt("admin", "LoopbackOnly", LoopbackOnly);

UseDSMPlugin=myIniFile_In.ReadInt("admin", "UseDSMPlugin", false);
AllowLoopback=myIniFile_In.ReadInt("admin", "AllowLoopback", false);
AuthRequired=myIniFile_In.ReadInt("admin", "AuthRequired", true);
ConnectPriority=myIniFile_In.ReadInt("admin", "ConnectPriority", 0);

myIniFile_Out.WriteInt("admin", "UseDSMPlugin", UseDSMPlugin);
myIniFile_Out.WriteInt("admin", "AllowLoopback", AllowLoopback);
myIniFile_Out.WriteInt("admin", "AuthRequired", AuthRequired);
myIniFile_Out.WriteInt("admin", "ConnectPriority", ConnectPriority);


myIniFile_In.ReadString("admin", "DSMPlugin",DSMPlugin,128);
myIniFile_In.ReadString("admin", "AuthHosts",authhosts,150);

myIniFile_Out.WriteString("admin", "DSMPlugin",DSMPlugin);
myIniFile_Out.WriteString("admin", "AuthHosts",authhosts);

AllowShutdown=myIniFile_In.ReadInt("admin", "AllowShutdown", true);
AllowProperties=myIniFile_In.ReadInt("admin", "AllowProperties", true);
AllowEditClients=myIniFile_In.ReadInt("admin", "AllowEditClients", true);
myIniFile_Out.WriteInt("admin", "AllowShutdown" ,AllowShutdown);
myIniFile_Out.WriteInt("admin", "AllowProperties" ,AllowProperties);
myIniFile_Out.WriteInt("admin", "AllowEditClients" ,AllowEditClients);


FileTransferEnabled=myIniFile_In.ReadInt("admin", "FileTransferEnabled", true);
FTUserImpersonation=myIniFile_In.ReadInt("admin", "FTUserImpersonation", true);
BlankMonitorEnabled = myIniFile_In.ReadInt("admin", "BlankMonitorEnabled", true);
DefaultScale = myIniFile_In.ReadInt("admin", "DefaultScale", 1);
CaptureAlphaBlending = myIniFile_In.ReadInt("admin", "CaptureAlphaBlending", false); // sf@2005
BlackAlphaBlending = myIniFile_In.ReadInt("admin", "BlackAlphaBlending", false); // sf@2005
myIniFile_Out.WriteInt("admin", "FileTransferEnabled", FileTransferEnabled);
myIniFile_Out.WriteInt("admin", "FTUserImpersonation", FTUserImpersonation);
myIniFile_Out.WriteInt("admin", "BlankMonitorEnabled", BlankMonitorEnabled);
myIniFile_Out.WriteInt("admin", "DefaultScale", DefaultScale);
myIniFile_Out.WriteInt("admin", "CaptureAlphaBlending", CaptureAlphaBlending);
myIniFile_Out.WriteInt("admin", "BlackAlphaBlending", BlackAlphaBlending);


	// Connection prefs
SocketConnect=myIniFile_In.ReadInt("admin", "SocketConnect", true);
HTTPConnect=myIniFile_In.ReadInt("admin", "HTTPConnect", true);
XDMCPConnect=myIniFile_In.ReadInt("admin", "XDMCPConnect", true);
AutoPortSelect=myIniFile_In.ReadInt("admin", "AutoPortSelect", true);
PortNumber=myIniFile_In.ReadInt("admin", "PortNumber", 0);
HttpPortNumber=myIniFile_In.ReadInt("admin", "HTTPPortNumber",0);
IdleTimeout=myIniFile_In.ReadInt("admin", "IdleTimeout", 0);
myIniFile_Out.WriteInt("admin", "SocketConnect", SocketConnect);
myIniFile_Out.WriteInt("admin", "HTTPConnect", HTTPConnect);
myIniFile_Out.WriteInt("admin", "XDMCPConnect", XDMCPConnect);
myIniFile_Out.WriteInt("admin", "AutoPortSelect", AutoPortSelect);
myIniFile_Out.WriteInt("admin", "PortNumber", PortNumber);
myIniFile_Out.WriteInt("admin", "HTTPPortNumber", HttpPortNumber);
myIniFile_Out.WriteInt("admin", "IdleTimeout", IdleTimeout);
	
RemoveWallpaper=myIniFile_In.ReadInt("admin", "RemoveWallpaper", 0);
RemoveEaro=myIniFile_In.ReadInt("admin", "RemoveEaro", 0);
myIniFile_Out.WriteInt("admin", "RemoveWallpaper", RemoveWallpaper);
myIniFile_Out.WriteInt("admin", "RemoveEaro", RemoveEaro);

	// Connection querying settings
QuerySetting=myIniFile_In.ReadInt("admin", "QuerySetting", 0);
QueryTimeout=myIniFile_In.ReadInt("admin", "QueryTimeout", 0);
QueryAccept=myIniFile_In.ReadInt("admin", "QueryAccept", 0);
QueryIfNoLogon=myIniFile_In.ReadInt("admin", "QueryIfNoLogon", 0);
myIniFile_Out.WriteInt("admin", "QuerySetting", QuerySetting);
myIniFile_Out.WriteInt("admin", "QueryTimeout", QueryTimeout);
myIniFile_Out.WriteInt("admin", "QueryAccept", QueryAccept);
myIniFile_Out.WriteInt("admin", "QueryIfNoLogon", QueryIfNoLogon);

myIniFile_In.ReadPassword(passwd,MAXPWLEN);
myIniFile_Out.WritePassword(passwd);

EnableRemoteInputs=myIniFile_In.ReadInt("admin", "InputsEnabled", 0);
LockSettings=myIniFile_In.ReadInt("admin", "LockSetting", 0);
DisableLocalInputs=myIniFile_In.ReadInt("admin", "LocalInputsDisabled", 0);
myIniFile_Out.WriteInt("admin", "InputsEnabled", EnableRemoteInputs);
myIniFile_Out.WriteInt("admin", "LockSetting", LockSettings);
myIniFile_Out.WriteInt("admin", "LocalInputsDisabled", DisableLocalInputs);	



TurboMode = myIniFile_In.ReadInt("poll", "TurboMode", 0);
PollUnderCursor=myIniFile_In.ReadInt("poll", "PollUnderCursor", 0);
PollForeground=myIniFile_In.ReadInt("poll", "PollForeground", 0);
PollFullScreen=myIniFile_In.ReadInt("poll", "PollFullScreen", 0);
PollConsoleOnly=myIniFile_In.ReadInt("poll", "OnlyPollConsole", 0);
PollOnEventOnly=myIniFile_In.ReadInt("poll", "OnlyPollOnEvent", 0);
Driver=myIniFile_In.ReadInt("poll", "EnableDriver", 0);
Hook=myIniFile_In.ReadInt("poll", "EnableHook", 0);
Virtual=myIniFile_In.ReadInt("poll", "EnableVirtual", 0);

myIniFile_Out.WriteInt("poll", "TurboMode", TurboMode);
myIniFile_Out.WriteInt("poll", "PollUnderCursor", PollUnderCursor);
myIniFile_Out.WriteInt("poll", "PollForeground", PollForeground);
myIniFile_Out.WriteInt("poll", "PollFullScreen", PollFullScreen);
myIniFile_Out.WriteInt("poll", "OnlyPollConsole",PollConsoleOnly);
myIniFile_Out.WriteInt("poll", "OnlyPollOnEvent", PollOnEventOnly);
myIniFile_Out.WriteInt("poll", "EnableDriver", Driver);
myIniFile_Out.WriteInt("poll", "EnableHook", Hook);
myIniFile_Out.WriteInt("poll", "EnableVirtual", Virtual);
DeleteFile(lpCmdLine);
}
void
Real_settings(char *mycommand)
{
Copy_to_Secure_from_temp(mycommand);
}

void
Set_stop_service_as_admin()
{
	char exe_file_name[MAX_PATH];
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = winvncStopservice;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

}

void
Real_stop_service()
{
	char command[100];
	strcpy(command,"net stop ");
	strcat(command,service_name);
	WinExec(command,SW_HIDE);
}

void
Set_start_service_as_admin()
{
	char exe_file_name[MAX_PATH];
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = winvncStartservice;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

}

void
Real_start_service()
{
	char command[100];
	strcpy(command,"net start ");
	strcat(command,service_name);
	WinExec(command,SW_HIDE);
}

void
Set_install_service_as_admin()
{
	char exe_file_name[MAX_PATH];
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = winvncInstallService;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

}

void
Set_uninstall_service_as_admin()
{
	char exe_file_name[MAX_PATH];
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = winvncUnInstallService;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOWNORMAL;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

}