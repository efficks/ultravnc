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


// ReadProperties

// Object implementing the Properties dialog for WinVNC.
// The Properties dialog is displayed whenever the user selects the
// Properties option from the system tray menu.
// The Properties dialog also takes care of loading the program
// settings and saving them on exit.

class ReadProperties;

#if (!defined(_WINVNC_ReadProperties))
#define _WINVNC_ReadProperties

// Includes
// Marscha@2004 - authSSP: objbase.h needed for CoInitialize etc.
#include <objbase.h>

#include "stdhdrs.h"
#include "vncServer.h"
#include "inifile.h"
#include <Userenv.h>
// The ReadProperties class itself
class ReadProperties
{
public:
	// Constructor/destructor
	ReadProperties();
	~ReadProperties();

	// Initialisation
	BOOL Init(vncServer *server);
	// TRAY ICON MENU SETTINGS
	BOOL AllowProperties() {return m_allowproperties;};
	BOOL AllowShutdown() {return m_allowshutdown;};
	BOOL AllowEditClients() {return m_alloweditclients;};
	bool Lock_service_helper;

	// Ini file
	IniFile myIniFile;
	void LoadFromIniFile();

	// Implementation
protected:
	// The server object to which this properties object is attached.
	vncServer *			m_server;

	// Flag to indicate whether the currently loaded settings are for
	// the current user, or are default system settings
	BOOL				m_usersettings;

	// Tray icon menu settings
	BOOL				m_allowproperties;
	BOOL				m_allowshutdown;
	BOOL				m_alloweditclients;
    int                 m_ftTimeout;
    int                 m_keepAliveInterval;


	// Password handling
	void LoadPassword(HKEY k, char *buffer);
	void LoadPassword2(HKEY k, char *buffer); //PGM

	// String handling
	char * LoadString(HKEY k, LPCSTR valname);

	// Manipulate the registry settings
	LONG LoadInt(HKEY key, LPCSTR valname, LONG defval);

	// Loading/saving all the user prefs
	void LoadUserPrefs(HKEY appkey);

	// Making the loaded user prefs active
	void ApplyUserPrefs();
	
	BOOL m_returncode_valid;
	BOOL m_dlgvisible;

	// STORAGE FOR THE PROPERTIES PRIOR TO APPLICATION
	BOOL m_pref_SockConnect;
	BOOL m_pref_HTTPConnect;
	BOOL m_pref_XDMCPConnect;
	BOOL m_pref_AutoPortSelect;
	LONG m_pref_PortNumber;
	LONG m_pref_HttpPortNumber;  // TightVNC 1.1.7
	char m_pref_passwd[MAXPWLEN];
	char m_pref_passwd2[MAXPWLEN]; //PGM
	UINT m_pref_QuerySetting;
	// Marscha@2006 - Is AcceptDialog required even if no user is logged on
	UINT m_pref_QueryIfNoLogon;
	UINT m_pref_QueryAccept;
	UINT m_pref_QueryTimeout;
	UINT m_pref_IdleTimeout;
	BOOL m_pref_RemoveWallpaper;
	BOOL m_pref_RemoveAero;
	BOOL m_pref_EnableRemoteInputs;
	int m_pref_LockSettings;
	BOOL m_pref_DisableLocalInputs;
	BOOL m_pref_EnableJapInput;
	BOOL m_pref_clearconsole;

	// Modif sf@2002
	// [v1.0.2-jp2 fix]
	BOOL m_pref_SingleWindow;
	BOOL m_pref_EnableFileTransfer;
	BOOL m_pref_FTUserImpersonation;
	BOOL m_pref_EnableBlankMonitor;
	BOOL m_pref_BlankInputsOnly; //PGM
	int  m_pref_DefaultScale;
	BOOL m_pref_RequireMSLogon;
	BOOL m_pref_CaptureAlphaBlending;
	BOOL m_pref_BlackAlphaBlending;
//	BOOL m_pref_GammaGray;	// [v1.0.2-jp1 fix1]

	
	// Marscha@2004 - authSSP: added state of "New MS-Logon"
	BOOL m_pref_NewMSLogon;

	BOOL m_pref_UseDSMPlugin;
	char m_pref_szDSMPlugin[128];
    void LoadDSMPluginName(HKEY key, char *buffer);

	char m_pref_path111[500];
	char m_Tempfile[MAX_PATH];
	BOOL m_pref_Primary;
	BOOL m_pref_Secondary;

	BOOL m_pref_TurboMode;
	
	BOOL m_pref_PollUnderCursor;
	BOOL m_pref_PollForeground;
	BOOL m_pref_PollFullScreen;
	BOOL m_pref_PollConsoleOnly;
	BOOL m_pref_PollOnEventOnly;
	LONG m_pref_MaxCpu;

	BOOL m_pref_Driver;
	BOOL m_pref_Hook;
	BOOL m_pref_Virtual;

	// [v1.0.2-jp2 fix]
	char m_pref_szSingleWindowName[32];

private:
};

#endif // _WINVNC_ReadProperties
