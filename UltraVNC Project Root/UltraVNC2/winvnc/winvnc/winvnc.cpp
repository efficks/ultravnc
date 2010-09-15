//  Copyright (C) 2010 Ultr@VNC Team Members. All Rights Reserved.
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
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/

// WinVNC.cpp

// 24/11/97		WEZ

// WinMain and main WndProc for the new version of WinVNC

////////////////////////////
// System headers
#include "stdhdrs.h"

#include "mmsystem.h"

////////////////////////////
// Custom headers
#include "VSocket.h"
#include "WinVNC.h"
#include "vncServer.h"
#include "vncInstHandler.h"
#include "vncService.h"
#include "vncOSVersion.h"
#include "shared_mem_and_events.h"
#ifdef IPP
void InitIpp();
#endif

#define LOCALIZATION_MESSAGES   // ACT: full declaration instead on extern ones
#include "localization.h" // Act : add localization on messages
// Application instance and name
bool GLOBAL_RUNNING=true;;
const char	*szAppName = "WinVNC";
DWORD		mainthreadId;
BOOL		fRunningFromExternalService=false;
HINSTANCE g_inst;
// sf@2007 - New shutdown order handling stuff (with uvnc_service)
bool			fShutdownOrdered = false;

void WRITETOLOG(char *szText, int size, DWORD *byteswritten, void *);
void Init_shared_mem_and_events();
extern comm_serv Closebyservice;
int start_service();
// winvnc.exe will also be used for helper exe
// This allow us to minimize the number of seperate exe
bool
Myinit(HINSTANCE hInstance)
{
	SetOSVersion();
	setbuf(stderr, 0);
    //Load all messages from ressource file
    Load_Localization(hInstance) ;
	//vnclog.SetFile();
	//vnclog.SetMode(2);
	//vnclog.SetLevel(10);

	// Save the application instance and main thread id
	mainthreadId = GetCurrentThreadId();


	// Initialise the VSocket system
	VSocketSystem socksys;
	if (!socksys.Initialised())
	{
		MessageBox(NULL, sz_ID_FAILED_INIT, szAppName, MB_OK);
		return 0;
	}
	return 1;
}
//#define CRASHRPT
#ifdef CRASHRPT
#include "C:/DATA/crash/crashrpt/include/crashrpt.h"
#pragma comment(lib, "C:/DATA/crash/crashrpt/lib/crashrpt")
#endif

// WinMain parses the command line and either calls the main App
// routine or, under NT, the main service routine.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	g_inst=hInstance;
#ifdef IPP
	InitIpp();
#endif
#ifdef CRASHRPT
	Install(NULL, _T("ultravnc@skynet.be"), _T(""));
#endif
	SetOSVersion();
	setbuf(stderr, 0);
    //Load all messages from ressource file
    Load_Localization( hInstance) ;

	char WORKDIR[MAX_PATH];
	if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
		{
		char* p = strrchr(WORKDIR, '\\');
		if (p == NULL) return 0;
		*p = '\0';
		}
    char progname[MAX_PATH];
    strncpy(progname, WORKDIR, sizeof progname);
    progname[MAX_PATH - 1] = 0;
	vnclog.SetFile();


	// Save the application instance and main thread id
	mainthreadId = GetCurrentThreadId();


	// Initialise the VSocket system
	VSocketSystem socksys;
	if (!socksys.Initialised())
	{
		MessageBox(NULL, sz_ID_FAILED_INIT, szAppName, MB_OK);
		return 0;
	}

	// Make the command-line lowercase and parse it
	size_t i;
	for (i = 0; i < strlen(szCmdLine); i++)
	{
		szCmdLine[i] = tolower(szCmdLine[i]);
	} 


	BOOL argfound = FALSE;
	for (i = 0; i < strlen(szCmdLine); i++)
	{
		if (szCmdLine[i] <= ' ')
			continue;
		argfound = TRUE;		

		if (strncmp(&szCmdLine[i], winvncStartService, strlen(winvncStartService)) == 0)
		{
		start_service();
		return 0;
		}

		if (strncmp(&szCmdLine[i], winvncRunAsUserApp, strlen(winvncRunAsUserApp)) == 0)
		{
			// WinVNC is being run as a user-level program
			if (!Myinit(hInstance)) return 0;
			return WinVNCAppMain();
		}

		if (strncmp(&szCmdLine[i], winvncAutoReconnect, strlen(winvncAutoReconnect)) == 0)
		{
			// Note that this "autoreconnect" param MUST be BEFORE the "connect" one
			// on the command line !
			// wa@2005 -- added support for the AutoReconnectId
			i+=strlen(winvncAutoReconnect);

			int start, end;
			char* pszId = NULL;
			start = i;
			// skip any spaces and grab the parameter
			while (szCmdLine[start] <= ' ' && szCmdLine[start] != 0) start++;
			
			if ( strncmp( &szCmdLine[start], winvncAutoReconnectId, strlen(winvncAutoReconnectId) ) == 0 )
			{
				end = start;
				while (szCmdLine[end] > ' ') end++;
				
				pszId = new char[ end - start + 1 ];
				if (pszId != 0) 
				{
					strncpy( pszId, &(szCmdLine[start]), end - start );
					pszId[ end - start ] = 0;
					pszId = _strupr( pszId );
				}
//multiple spaces between autoreconnect and id 
				i = end;
			}// end of condition we found the ID: parameter
			
			// NOTE:  id must be NULL or the ID:???? (pointer will get deleted when message is processed)
			// We can not contact a runnning service, permissions, so we must store the settings
			// and process until the vncmenu has been started

				PostAddAutoConnectClient_bool=true;
				if (pszId==NULL)
				{
					PostAddAutoConnectClient_bool_null=true;
					PostAddAutoConnectClient_bool=false;
				}
				else
				{
					strcpy(pszId_char,pszId);
					//memory leak fix
					delete [] pszId;
				}
			continue;
		}

			
		if ( strncmp( &szCmdLine[i], winvncReconnectId, strlen(winvncReconnectId) ) == 0 )
			{
				i+=strlen("-");
				int start, end;
				char* pszId = NULL;
				start = i;
				end = start;
				while (szCmdLine[end] > ' ') end++;

				pszId = new char[ end - start + 1 ];
				if (pszId != 0) 
				{
					strncpy( pszId, &(szCmdLine[start]), end - start );
					pszId[ end - start ] = 0;
					pszId = _strupr( pszId );
				}
				i = end;
				PostAddConnectClient_bool=true;
				if (pszId==NULL)
				{
					PostAddConnectClient_bool_null=true;
					PostAddConnectClient_bool=false;
				}
				else
				{
					strcpy(pszId_char,pszId);
					//memory leak fix
					delete [] pszId;
				}
			continue;
		}

		if (strncmp(&szCmdLine[i], winvncConnect, strlen(winvncConnect)) == 0)
		{
			// Add a new client to an existing copy of winvnc
			i+=strlen(winvncConnect);

			// First, we have to parse the command line to get the filename to use
			int start, end;
			start=i;
			while (szCmdLine[start] <= ' ' && szCmdLine[start] != 0) start++;
			end = start;
			while (szCmdLine[end] > ' ') end++;

			// Was there a hostname (and optionally a port number) given?
			if (end-start > 0)
			{
				char *name = new char[end-start+1];
				if (name != 0) {
					strncpy(name, &(szCmdLine[start]), end-start);
					name[end-start] = 0;

					int port = INCOMING_PORT_OFFSET;
					char *portp = strchr(name, ':');
					if (portp) {
						*portp++ = '\0';
						if (*portp == ':') {
							port = atoi(++portp);	// Port number after "::"
						} else {
							port = atoi(portp);	// Display number after ":"
						}
					}
					vnclog.Print(LL_STATE, VNCLOG("test... %s %d\n"),name,port);
					VCard32 address = VSocket::Resolve(name);
					delete [] name;
					if (address != 0) {
						// Post the IP address to the server
						// We can not contact a runnning service, permissions, so we must store the settings
						// and process until the vncmenu has been started
						vnclog.Print(LL_INTERR, VNCLOG("PostAddNewClient III \n"));
						PostAddNewClient_bool=true;
						port_int=port;
						address_vcard=address;
					}
					else
					{
						PostAddNewClient_bool=true;
						port_int=0;
						address_vcard=0;
					}
				}
				i=end;
				continue;
			}
			else 
			{
				// Tell the server to show the Add New Client dialog
				// We can not contact a runnning service, permissions, so we must store the settings
				// and process until the vncmenu has been started
				vnclog.Print(LL_INTERR, VNCLOG("PostAddNewClient IIII\n"));
				PostAddNewClient_bool=true;
				port_int=0;
				address_vcard=0;

			}
			continue;
		}

		//adzm 2009-06-20
		if (strncmp(&szCmdLine[i], winvncRepeater, strlen(winvncRepeater)) == 0)
		{
			// set the default repeater host
			i+=strlen(winvncRepeater);

			// First, we have to parse the command line to get the host to use
			int start, end;
			start=i;
			while (szCmdLine[start] <= ' ' && szCmdLine[start] != 0) start++;
			end = start;
			while (szCmdLine[end] > ' ') end++;

			// Was there a hostname (and optionally a port number) given?
			if (end-start > 0)
			{
				if (g_szRepeaterHost) {
					delete[] g_szRepeaterHost;
					g_szRepeaterHost = NULL;
				}
				g_szRepeaterHost = new char[end-start+1];
				if (g_szRepeaterHost != 0) {
					strncpy(g_szRepeaterHost, &(szCmdLine[start]), end-start);
					g_szRepeaterHost[end-start] = 0;
					
					// We can not contact a runnning service, permissions, so we must store the settings
					// and process until the vncmenu has been started
					vnclog.Print(LL_INTERR, VNCLOG("PostAddNewRepeaterClient I\n"));
					PostAddNewRepeaterClient_bool=true;
					port_int=0;
					address_vcard=0;
				}
				i=end;
				continue;
			}
			continue;
		}

		// Either the user gave the -help option or there is something odd on the cmd-line!

		// Show the usage dialog
		MessageBox(NULL, winvncUsageText, sz_ID_WINVNC_USAGE, MB_OK | MB_ICONINFORMATION);
		break;
	};

	// If no arguments were given then just run
	if (!argfound)
	{
		if (!Myinit(hInstance)) return 0;
		return WinVNCAppMain();
	}
	return 0;
}






// This is the main routine for WinVNC when running as an application
// (under Windows 95 or Windows NT)
// Under NT, WinVNC can also run as a service.  The WinVNCServerMain routine,
// defined in the vncService header, is used instead when running as a service.


int WinVNCAppMain()
{
	SetOSVersion();
	
	vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - WinVNCAPPMain\n"));
#ifdef CRASH_ENABLED
	LPVOID lpvState = Install(NULL,  "rudi.de.vos@skynet.be", "UltraVnc");
#endif

	// Set this process to be the last application to be shut down.
	// Check for previous instances of WinVNC!
	vncInstHandler *instancehan=new vncInstHandler;

	if (!instancehan->Init())
		{	
    		vnclog.Print(LL_INTINFO, VNCLOG("%s -- exiting\n"), sz_ID_ANOTHER_INST);
			// We don't allow multiple instances!
			if (!fRunningFromExternalService)
				MessageBox(NULL, sz_ID_ANOTHER_INST, szAppName, MB_OK);		

			if (instancehan!=NULL) delete instancehan;

			vnclog.Print(LL_STATE, VNCLOG("################## SHUTING DOWN SERVER ####################\n"));

			//adzm 2009-06-20
			if (g_szRepeaterHost) {
				delete[] g_szRepeaterHost;
				g_szRepeaterHost = NULL;
			}
		return 0;
		}

	Init_shared_mem_and_events();

	if (!vncService::RunningAsService())
	{
		char WORKDIR[MAX_PATH];
		if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
			{
			char* p = strrchr(WORKDIR, '\\');
			if (p == NULL) return 0;
			*p = '\0';
			}
		strcat(WORKDIR,"\\uvnc_session.exe");

		SHELLEXECUTEINFO shExecInfo;
		memset(&shExecInfo,0,sizeof(shExecInfo));
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = NULL;
		shExecInfo.hwnd = GetForegroundWindow();
		//shExecInfo.lpVerb = "runas";
		shExecInfo.lpFile = WORKDIR;
		shExecInfo.lpParameters ="";
		shExecInfo.lpDirectory = NULL;
		shExecInfo.nShow = SW_SHOWNORMAL;
		shExecInfo.hInstApp = NULL;
		ShellExecuteEx(&shExecInfo);
	}


	//vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Previous instance checked - Trying to create server\n"));
	// CREATE SERVER
	vncServer server;

	// Set the name and port number
	server.SetName(szAppName);
	vnclog.Print(LL_STATE, VNCLOG("server created ok\n"));
	///uninstall driver before cont
	
	// sf@2007 - Set Application0 special mode
	server.RunningFromExternalService(fRunningFromExternalService);

	while ( !fShutdownOrdered)
	{
		//vnclog.Print(LL_STATE, VNCLOG("################## Creating Imp Thread : %d \n"), nn);

		HANDLE threadHandle;
		DWORD dwTId;
		threadHandle = CreateThread(NULL, 0, imp_desktop_thread, &server, 0, &dwTId);

		HANDLE threadHandle2;
		DWORD dwTId2;
		threadHandle2 = CreateThread(NULL, 0, imp_desktop_thread2, &server, 0, &dwTId2);
		CloseHandle(threadHandle2);

		HANDLE threadHandle3;
		DWORD dwTId3;
		threadHandle3 = CreateThread(NULL, 0, imp_desktop_thread3, &server, 0, &dwTId3);
		CloseHandle(threadHandle3);

		HANDLE threadHandle4;
		DWORD dwTId4;
		threadHandle4 = CreateThread(NULL, 0, imp_desktop_thread4, &server, 0, &dwTId4);
		CloseHandle(threadHandle4);

		HANDLE threadHandle5;
		DWORD dwTId5;
		threadHandle5 = CreateThread(NULL, 0, imp_desktop_thread5, &server, 0, &dwTId5);
		CloseHandle(threadHandle5);

		HANDLE threadHandle6;
		DWORD dwTId6;
		threadHandle6 = CreateThread(NULL, 0, imp_desktop_thread6, &server, 0, &dwTId6);
		CloseHandle(threadHandle6);

		HANDLE threadHandle7;
		DWORD dwTId7;
		threadHandle7 = CreateThread(NULL, 0, imp_desktop_thread7, &server, 0, &dwTId7);
		CloseHandle(threadHandle7);

		HANDLE threadHandle8;
		DWORD dwTId8;
		threadHandle8 = CreateThread(NULL, 0, settings_change_monitor, &server, 0, &dwTId8);
		CloseHandle(threadHandle8);

		HANDLE threadHandle9;
		DWORD dwTId9;
		threadHandle9 = CreateThread(NULL, 0, tcp_controle_thread, &server, 0, &dwTId9);
		CloseHandle(threadHandle9);

		HANDLE threadHandle10;
		DWORD dwTId10;
		threadHandle10 = CreateThread(NULL, 0, imp_desktop_thread8, &server, 0, &dwTId10);
		CloseHandle(threadHandle10);

		if (threadHandle)  
		{
			WaitForSingleObject( threadHandle, INFINITE );
			CloseHandle(threadHandle);
		}
		vnclog.Print(LL_STATE, VNCLOG("################## Closing Imp Thread\n"));
	}

		Closebyservice.Call_Fnction_no_feedback();

		if (instancehan!=NULL)
			delete instancehan;

		vnclog.Print(LL_STATE, VNCLOG("################## SHUTING DOWN SERVER ####################\n"));

		//adzm 2009-06-20
		if (g_szRepeaterHost) {
			delete[] g_szRepeaterHost;
			g_szRepeaterHost = NULL;
		}
	return 1;
};

int
MENU_ADD_CLIENT_MSG_fn( WPARAM wParam, LPARAM lParam,vncServer *m_server)
{
			
			unsigned short nport = 0;
			char *nameDup = 0;
			char szAdrName[64];
			char szId[MAX_PATH] = {0};
			// sf@2003 - Values are already converted
			if ((m_server->AutoReconnect()|| m_server->IdReconnect() )&& strlen(m_server->AutoReconnectAdr()) > 0)
			{
				nport = m_server->AutoReconnectPort();
				strcpy(szAdrName, m_server->AutoReconnectAdr());
			}
			else
			{
				// Get the IP address stringified
				struct in_addr address;
				address.S_un.S_addr = lParam;
				char *name = inet_ntoa(address);
				if (name == 0)
					return 0;
				nameDup = _strdup(name);
				if (nameDup == 0)
					return 0;
				strcpy(szAdrName, nameDup);
				// Free the duplicate name
				if (nameDup != 0) free(nameDup);

				// Get the port number
				nport = (unsigned short)wParam;
				if (nport == 0)
					nport = INCOMING_PORT_OFFSET;
				
			}

			// wa@2005 -- added support for the AutoReconnectId
			// (but it's not required)
			bool bId = ( strlen(m_server->AutoReconnectId() ) > 0);
			if ( bId )
				strcpy( szId, m_server->AutoReconnectId() );
			
			// sf@2003
			// Stores the client adr/ports the first time we try to connect
			// This way we can call this message again later to reconnect with the same values
			if ((m_server->AutoReconnect() || m_server->IdReconnect())&& strlen(m_server->AutoReconnectAdr()) == 0)
			{
				m_server->AutoReconnectAdr(szAdrName);
				m_server->AutoReconnectPort(nport);
			}

			// Attempt to create a new socket
			VSocket *tmpsock;
			tmpsock = new VSocket;
			if (tmpsock) {

				// Connect out to the specified host on the VNCviewer listen port
				tmpsock->Create();
				if (tmpsock->Connect(szAdrName, nport)) {
					if ( bId )
					{
						// wa@2005 -- added support for the AutoReconnectId
						// Set the ID for this client -- code taken from vncconndialog.cpp (ln:142)
						tmpsock->Send(szId,250);
						tmpsock->SetTimeout(0);
						
						// adzm 2009-07-05 - repeater IDs
						// Add the new client to this server
						// adzm 2009-08-02
						m_server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, szId, szAdrName, nport,true);
					} else {
						// Add the new client to this server
						// adzm 2009-08-02
						m_server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, NULL, szAdrName, nport,true);
					}
				} else {
					delete tmpsock;
					m_server->AutoConnectRetry();
				}
			}
		
			return 0;
		}

int MENU_REPEATER_ID_MSG_fn(vncServer *m_server,char *IN_szId)
 		{
			m_server->IdReconnect(true);			
			if ( IN_szId)
				m_server->AutoReconnectId(IN_szId);
			
			return 0;
		}

int MENU_AUTO_RECONNECT_MSG_fn(vncServer *m_server,char *IN_szId)
		{		
			m_server->AutoReconnect(true);			
			if ( IN_szId)
				m_server->AutoReconnectId(IN_szId);
			
			return 0;
		}
DWORD WINAPI Start_service_non_desktop_part(LPVOID lpParam)
{

	//Run as service
	if (!Myinit(g_inst)) return 0;
	fRunningFromExternalService = true;
	vncService::RunningFromExternalService(true); 
	return WinVNCAppMain();
}

