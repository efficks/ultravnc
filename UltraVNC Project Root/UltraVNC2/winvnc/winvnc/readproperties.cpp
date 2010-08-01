//  Copyright (C) 2002 Ultr@VNC Team Members. All Rights Reserved.
//  Copyright (C) 2000-2002 Const Kaplinsky. All Rights Reserved.
//  Copyright (C) 2002 TightVNC. All Rights Reserved.
//  Copyright (C) 2002 RealVNC Ltd. All Rights Reserved.
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


// ReadProperties.cpp

// Implementation of the Properties dialog!

#include "stdhdrs.h"
#include "lmcons.h"
#include "vncService.h"

#include "WinVNC.h"
#include "readproperties.h"
#include "vncServer.h"
#include "vncPasswd.h"
#include "vncOSVersion.h"
#include "common/win32_helpers.h"
#include "localization.h" // ACT : Add localization on messages


const char WINVNC_REGISTRY_KEY [] = "Software\\ORL\\WinVNC3";
unsigned int G_SENDBUFFER=8192;

BOOL SPECIAL_SC_EXIT=false;
BOOL SPECIAL_SC_PROMPT=false;

// Constructor & Destructor
ReadProperties::ReadProperties()
{
	m_pref_RequireMSLogon=false;
	m_pref_NewMSLogon = false;
	m_pref_UseDSMPlugin=false;
	m_pref_AutoPortSelect=TRUE;
    m_pref_HTTPConnect = TRUE;
	m_pref_XDMCPConnect = TRUE;
	m_pref_PortNumber = RFB_PORT_OFFSET; 
	m_pref_SockConnect=TRUE;
	{
	    vncPasswd::FromClear crypt;
	    memcpy(m_pref_passwd, crypt, MAXPWLEN);
	}
	m_pref_QuerySetting=2;
	m_pref_QueryTimeout=10;
	m_pref_QueryAccept=0;
	m_pref_IdleTimeout=0;
	m_pref_EnableRemoteInputs=TRUE;
	m_pref_DisableLocalInputs=FALSE;
	m_pref_EnableJapInput=FALSE;
	m_pref_clearconsole=FALSE;
	m_pref_LockSettings=-1;
	m_pref_RemoveWallpaper=TRUE;
	m_pref_RemoveAero=TRUE;
    m_alloweditclients = TRUE;
	m_allowshutdown = TRUE;
	m_allowproperties = TRUE;
	m_pref_SingleWindow = FALSE;
	m_pref_UseDSMPlugin = FALSE;
	*m_pref_szDSMPlugin = '\0';
	m_pref_EnableFileTransfer = TRUE;
	m_pref_FTUserImpersonation = TRUE;
	m_pref_EnableBlankMonitor = TRUE;
	m_pref_DefaultScale = 1;
	m_pref_CaptureAlphaBlending = FALSE; 
	m_pref_BlackAlphaBlending = FALSE; 
	m_pref_TurboMode = ((vncService::VersionMajor() >= 6) ? TRUE : FALSE);
	m_pref_PollUnderCursor=FALSE;
	m_pref_PollForeground= TRUE;
	m_pref_PollFullScreen= ((vncService::VersionMajor() >= 6) ? FALSE : TRUE);
	m_pref_PollConsoleOnly=FALSE;
	m_pref_PollOnEventOnly=TRUE;
	m_pref_MaxCpu=40;
	m_pref_Hook=TRUE;
	m_pref_Virtual=FALSE;
	m_pref_SingleWindow = 0;
	*m_pref_szSingleWindowName = '\0';
    m_alloweditclients = TRUE;
	m_allowproperties = TRUE;
	m_allowshutdown = TRUE;
	m_dlgvisible = FALSE;
	m_usersettings = TRUE;
	Lock_service_helper=TRUE;
    m_ftTimeout = FT_RECV_TIMEOUT;
    m_keepAliveInterval = KEEPALIVE_INTERVAL;
	m_pref_Primary=true;
	m_pref_Secondary=false;
}

ReadProperties::~ReadProperties()
{
}

// Initialisation
BOOL
ReadProperties::Init(vncServer *server)
{
	// Save the server pointer
	m_server = server;
	LoadFromIniFile();

	// If the password is empty then always show a dialog
	char passwd[MAXPWLEN];
	m_server->GetPassword(passwd);
	{
	    vncPasswd::ToText plain(passwd);
	    if (strlen(plain) == 0)
				if(m_server->AuthRequired()) {
					PostQuitMessage(0);
				} 
	}
	Lock_service_helper=false;
	return TRUE;
}
void
ReadProperties::ApplyUserPrefs()
{
	//logging
	vnclog.SetMode(myIniFile.ReadInt("admin", "DebugMode", 0));
	char temp[512];
	myIniFile.ReadString("admin", "path", temp,512);
	vnclog.SetPath(temp);
	vnclog.SetLevel(myIniFile.ReadInt("admin", "DebugLevel", 0));
	vnclog.SetVideo(myIniFile.ReadInt("admin", "Avilog", 0) ? true : false);

	m_server->SetDisableTrayIcon(myIniFile.ReadInt("admin", "DisableTrayIcon", false));
	m_server->SetLoopbackOnly(myIniFile.ReadInt("admin", "LoopbackOnly", false));

	m_server->RequireMSLogon(m_pref_RequireMSLogon);
	m_server->SetNewMSLogon(m_pref_NewMSLogon);

	if (m_server->LoopbackOnly()) m_server->SetLoopbackOk(true);
	else m_server->SetLoopbackOk(myIniFile.ReadInt("admin", "AllowLoopback", false));
	m_server->SetAuthRequired(myIniFile.ReadInt("admin", "AuthRequired", true));
	m_server->SetConnectPriority(myIniFile.ReadInt("admin", "ConnectPriority", 0));
	if (!m_server->LoopbackOnly())
	{
		char *authhosts=new char[150];
		myIniFile.ReadString("admin", "AuthHosts",authhosts,150);
		if (authhosts != 0) {
			m_server->SetAuthHosts(authhosts);
			delete [] authhosts;
		} else {
			m_server->SetAuthHosts(0);
		}
	} else {
		m_server->SetAuthHosts(0);
	}

	m_server->SetQuerySetting(m_pref_QuerySetting);
	m_server->SetQueryTimeout(m_pref_QueryTimeout);
	m_server->SetQueryAccept(m_pref_QueryAccept);
	m_server->SetQueryIfNoLogon(m_pref_QueryIfNoLogon);

	m_server->SetFTTimeout(m_ftTimeout);
    m_server->SetKeepAliveInterval(m_keepAliveInterval);


	m_server->EnableFileTransfer(m_pref_EnableFileTransfer);
	m_server->FTUserImpersonation(m_pref_FTUserImpersonation); // sf@2005
	m_server->CaptureAlphaBlending(m_pref_CaptureAlphaBlending); // sf@2005
	m_server->BlackAlphaBlending(m_pref_BlackAlphaBlending); // sf@2005
	m_server->Primary(m_pref_Primary);
	m_server->Secondary(m_pref_Secondary);

	m_server->BlankMonitorEnabled(m_pref_EnableBlankMonitor);
	m_server->BlankInputsOnly(m_pref_BlankInputsOnly); //PGM
	m_server->SetDefaultScale(m_pref_DefaultScale);

	// Update the connection querying settings
	m_server->SetQuerySetting(m_pref_QuerySetting);
	m_server->SetQueryTimeout(m_pref_QueryTimeout);
	m_server->SetQueryAccept(m_pref_QueryAccept);
	m_server->SetAutoIdleDisconnectTimeout(m_pref_IdleTimeout);
	m_server->EnableRemoveWallpaper(m_pref_RemoveWallpaper);
	m_server->EnableRemoveAero(m_pref_RemoveAero);

	// Is the listening socket closing?

	if (!m_pref_SockConnect)
		m_server->SockConnect(m_pref_SockConnect);

	m_server->EnableHTTPConnect(m_pref_HTTPConnect);
	m_server->EnableXDMCPConnect(m_pref_XDMCPConnect);

	// Are inputs being disabled?
	if (!m_pref_EnableRemoteInputs)
		m_server->EnableRemoteInputs(m_pref_EnableRemoteInputs);
	if (m_pref_DisableLocalInputs)
		m_server->DisableLocalInputs(m_pref_DisableLocalInputs);
	if (m_pref_EnableJapInput)
		m_server->EnableJapInput(m_pref_EnableJapInput);
	m_server->Clearconsole(m_pref_clearconsole);

	// Update the password
	m_server->SetPassword(m_pref_passwd);
	m_server->SetPassword2(m_pref_passwd2); //PGM

	// Now change the listening port settings
	m_server->SetAutoPortSelect(m_pref_AutoPortSelect);
	if (!m_pref_AutoPortSelect)
		// m_server->SetPort(m_pref_PortNumber);
		m_server->SetPorts(m_pref_PortNumber, m_pref_HttpPortNumber); // Tight 1.2.7

	m_server->SockConnect(m_pref_SockConnect);


	// Remote access prefs
	m_server->EnableRemoteInputs(m_pref_EnableRemoteInputs);
	m_server->SetLockSettings(m_pref_LockSettings);
	m_server->DisableLocalInputs(m_pref_DisableLocalInputs);
	m_server->EnableJapInput(m_pref_EnableJapInput);
	m_server->Clearconsole(m_pref_clearconsole);

	// DSM Plugin prefs
	m_server->EnableDSMPlugin(m_pref_UseDSMPlugin);
	m_server->SetDSMPluginName(m_pref_szDSMPlugin);
	if (m_server->IsDSMPluginEnabled()) 
	{
		vnclog.Print(LL_INTINFO, VNCLOG("$$$$$$$$$$ ApplyUserPrefs - Plugin Enabled - Call SetDSMPlugin() \n"));
		m_server->SetDSMPlugin(false);
	}
	else
	{
		vnclog.Print(LL_INTINFO, VNCLOG("$$$$$$$$$$ ApplyUserPrefs - Plugin NOT enabled \n"));
	}

	 m_server->TurboMode(m_pref_TurboMode);
	// Polling prefs
	m_server->PollUnderCursor(m_pref_PollUnderCursor);
	m_server->PollForeground(m_pref_PollForeground);
	m_server->PollFullScreen(m_pref_PollFullScreen);
	m_server->PollConsoleOnly(m_pref_PollConsoleOnly);
	m_server->PollOnEventOnly(m_pref_PollOnEventOnly);
	m_server->MaxCpu(m_pref_MaxCpu);
		
	if (m_pref_Driver) m_server->Driver(m_pref_Driver);
	else m_server->Driver(false);
	m_server->Hook(m_pref_Hook);
	m_server->Virtual(m_pref_Virtual);
	// [v1.0.2-jp2 fix]
	m_server->SingleWindow(m_pref_SingleWindow);
	m_server->SetSingleWindowName(m_pref_szSingleWindowName);
	vnclog.SetMode(1);
	vnclog.SetLevel(10);
}

void
ReadProperties::LoadDSMPluginName(HKEY key, char *buffer)
{
	DWORD type = REG_BINARY;
	int slen=MAXPATH;
	char inouttext[MAXPATH];

	if (RegQueryValueEx(key,
		"DSMPlugin",
		NULL,
		&type,
		(LPBYTE) &inouttext,
		(LPDWORD) &slen) != ERROR_SUCCESS)
		return;

	if (slen > MAXPATH)
		return;

	memcpy(buffer, inouttext, MAXPATH);
}



// ********************************************************************
// Ini file part - Wwill replace registry access completely, some day
// WARNING: until then, when adding/modifying a config parameter
//          don't forget to modify both ini file & registry parts !
// ********************************************************************

void ReadProperties::LoadFromIniFile()
{
	m_pref_RequireMSLogon = myIniFile.ReadInt("admin", "MSLogonRequired", m_pref_RequireMSLogon);
	m_pref_NewMSLogon = myIniFile.ReadInt("admin", "NewMSLogon", m_pref_NewMSLogon);
	m_pref_UseDSMPlugin = myIniFile.ReadInt("admin", "UseDSMPlugin", m_pref_UseDSMPlugin);
	myIniFile.ReadString("admin", "DSMPlugin",m_pref_szDSMPlugin,128);
	m_pref_EnableFileTransfer = myIniFile.ReadInt("admin", "FileTransferEnabled", m_pref_EnableFileTransfer);
	m_pref_FTUserImpersonation = myIniFile.ReadInt("admin", "FTUserImpersonation", m_pref_FTUserImpersonation); // sf@2005
	m_pref_EnableBlankMonitor = myIniFile.ReadInt("admin", "BlankMonitorEnabled", m_pref_EnableBlankMonitor);
	m_pref_BlankInputsOnly = myIniFile.ReadInt("admin", "BlankInputsOnly", m_pref_BlankInputsOnly); //PGM
	m_pref_DefaultScale = myIniFile.ReadInt("admin", "DefaultScale", m_pref_DefaultScale);
	m_pref_CaptureAlphaBlending = myIniFile.ReadInt("admin", "CaptureAlphaBlending", m_pref_CaptureAlphaBlending); // sf@2005
	m_pref_BlackAlphaBlending = myIniFile.ReadInt("admin", "BlackAlphaBlending", m_pref_BlackAlphaBlending); // sf@2005
	m_pref_UseDSMPlugin = myIniFile.ReadInt("admin", "UseDSMPlugin", m_pref_UseDSMPlugin);
	myIniFile.ReadString("admin", "DSMPlugin",m_pref_szDSMPlugin,128);
	m_pref_Primary = myIniFile.ReadInt("admin", "primary", m_pref_Primary);
	m_pref_Secondary = myIniFile.ReadInt("admin", "secondary", m_pref_Secondary);
	// Connection prefs
	m_pref_SockConnect=myIniFile.ReadInt("admin", "SocketConnect", m_pref_SockConnect);
	m_pref_HTTPConnect=myIniFile.ReadInt("admin", "HTTPConnect", m_pref_HTTPConnect);
	m_pref_XDMCPConnect=myIniFile.ReadInt("admin", "XDMCPConnect", m_pref_XDMCPConnect);
	m_pref_AutoPortSelect=myIniFile.ReadInt("admin", "AutoPortSelect", m_pref_AutoPortSelect);
	m_pref_PortNumber=myIniFile.ReadInt("admin", "PortNumber", m_pref_PortNumber);
	m_pref_HttpPortNumber=myIniFile.ReadInt("admin", "HTTPPortNumber",DISPLAY_TO_HPORT(PORT_TO_DISPLAY(m_pref_PortNumber)));
	m_pref_IdleTimeout=myIniFile.ReadInt("admin", "IdleTimeout", m_pref_IdleTimeout);	
	m_pref_RemoveWallpaper=myIniFile.ReadInt("admin", "RemoveWallpaper", m_pref_RemoveWallpaper);
	m_pref_RemoveAero=myIniFile.ReadInt("admin", "RemoveAero", m_pref_RemoveAero);
	// Connection querying settings
	m_pref_QuerySetting=myIniFile.ReadInt("admin", "QuerySetting", m_pref_QuerySetting);
	m_pref_QueryTimeout=myIniFile.ReadInt("admin", "QueryTimeout", m_pref_QueryTimeout);	
	m_pref_QueryAccept=myIniFile.ReadInt("admin", "QueryAccept", m_pref_QueryAccept);
	m_pref_QueryIfNoLogon=myIniFile.ReadInt("admin", "QueryIfNoLogon", m_pref_QueryIfNoLogon);	
	// Load the password
	myIniFile.ReadPassword(m_pref_passwd,MAXPWLEN);
	myIniFile.ReadPassword2(m_pref_passwd2,MAXPWLEN); //PGM
	// Remote access prefs
	m_pref_EnableRemoteInputs=myIniFile.ReadInt("admin", "InputsEnabled", m_pref_EnableRemoteInputs);
	m_pref_LockSettings=myIniFile.ReadInt("admin", "LockSetting", m_pref_LockSettings);
	m_pref_DisableLocalInputs=myIniFile.ReadInt("admin", "LocalInputsDisabled", m_pref_DisableLocalInputs);
	m_pref_EnableJapInput=myIniFile.ReadInt("admin", "EnableJapInput", m_pref_EnableJapInput);
	m_pref_clearconsole=myIniFile.ReadInt("admin", "clearconsole", m_pref_clearconsole);
	G_SENDBUFFER=myIniFile.ReadInt("admin", "sendbuffer", G_SENDBUFFER);
	// Polling prefs
	m_pref_TurboMode = myIniFile.ReadInt("poll", "TurboMode", m_pref_TurboMode);
	m_pref_PollUnderCursor=myIniFile.ReadInt("poll", "PollUnderCursor", m_pref_PollUnderCursor);
	m_pref_PollForeground=myIniFile.ReadInt("poll", "PollForeground", m_pref_PollForeground);
	m_pref_PollFullScreen=myIniFile.ReadInt("poll", "PollFullScreen", m_pref_PollFullScreen);
	m_pref_PollConsoleOnly=myIniFile.ReadInt("poll", "OnlyPollConsole", m_pref_PollConsoleOnly);
	m_pref_PollOnEventOnly=myIniFile.ReadInt("poll", "OnlyPollOnEvent", m_pref_PollOnEventOnly);
	m_pref_MaxCpu=myIniFile.ReadInt("poll", "MaxCpu", m_pref_MaxCpu);
	if (m_pref_MaxCpu==0) m_pref_MaxCpu=100;
	m_pref_Driver=myIniFile.ReadInt("poll", "EnableDriver", m_pref_Driver);
	m_pref_Hook=myIniFile.ReadInt("poll", "EnableHook", m_pref_Hook);
	m_pref_Virtual=myIniFile.ReadInt("poll", "EnableVirtual", m_pref_Virtual);	
	m_pref_SingleWindow=myIniFile.ReadInt("poll","SingleWindow",m_pref_SingleWindow);
	myIniFile.ReadString("poll", "SingleWindowName", m_pref_szSingleWindowName,32);
	m_allowshutdown = myIniFile.ReadInt("admin", "AllowShutdown", m_allowshutdown);
	m_allowproperties = myIniFile.ReadInt("admin", "AllowProperties", m_allowproperties);
	m_alloweditclients = myIniFile.ReadInt("admin", "AllowEditClients", m_alloweditclients);
    m_ftTimeout = myIniFile.ReadInt("admin", "FileTransferTimeout", m_ftTimeout);
    if (m_ftTimeout > 60)
        m_ftTimeout = 60;
    m_keepAliveInterval = myIniFile.ReadInt("admin", "KeepAliveInterval", m_keepAliveInterval);
    if (m_keepAliveInterval >= (m_ftTimeout - KEEPALIVE_HEADROOM))
        m_keepAliveInterval = m_ftTimeout - KEEPALIVE_HEADROOM;

	SPECIAL_SC_EXIT = myIniFile.ReadInt("admin", "SC_EXIT", SPECIAL_SC_EXIT );
	SPECIAL_SC_PROMPT = myIniFile.ReadInt("admin", "SC_PROMPT", SPECIAL_SC_PROMPT);

	ApplyUserPrefs();
}


