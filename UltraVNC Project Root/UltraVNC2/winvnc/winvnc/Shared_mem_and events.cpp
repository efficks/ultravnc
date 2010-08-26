#include "uvnc_session/uvnc_session/communication.h"
#include "vncServer.h"
#include "ReadProperties.h"
#include <sas.h>
#include "controlethread.h"


bool PostAddAutoConnectClient_bool=false;
bool PostAddNewClient_bool=false;
bool PostAddAutoConnectClient_bool_null=false;
bool PostAddConnectClient_bool=false;
bool PostAddConnectClient_bool_null=false;
bool PostAddNewRepeaterClient_bool=false;
char pszId_char[20];
VCard32 address_vcard;
int port_int;
char* g_szRepeaterHost = NULL;

int MENU_ADD_CLIENT_MSG_fn( WPARAM wParam, LPARAM lParam,vncServer *m_server);
int MENU_REPEATER_ID_MSG_fn(vncServer *m_server,char *IN_szId);
int MENU_AUTO_RECONNECT_MSG_fn(vncServer *m_server,char *IN_szId);
void get_update(UpdateTracker &to);
extern BOOL	fRunningFromExternalService;

comm_serv NotifyBalloon;
comm_serv AuthClientCount;
comm_serv SockConnected;
comm_serv Closebyservice;
comm_serv Closebyapp;

comm_serv KillAuthClients;
comm_serv KillClient; 
comm_serv TextChatClient; 
comm_serv ListAuthClients; 
comm_serv ListUnauthClients;
comm_serv Get_g_szRepeaterHost;
comm_serv Reverse;

comm_serv mouse_eventFn;
comm_serv SendInputFn;
comm_serv ClearShiftKeysFn;
comm_serv keyEventFn;
comm_serv ClearKeyStateFn;
comm_serv AcceptDialogFn;
comm_serv TextChatIPCFn;
comm_serv sas_eventFn;

//server
comm_serv Shared_memory_server;
comm_serv RemoteEventReceivedFn;
comm_serv All_clients_initialalizedFn;
comm_serv IsThereFileTransBusyFn;
	
comm_serv IsThereAUltraEncodingClientFn;
comm_serv UpdateWantedFn;	
comm_serv UpdateCursorShapeFn;
comm_serv Clear_Update_TrackerFn;
comm_serv KillAuthClientsFn;
comm_serv SetScreenOffsetFn;
comm_serv SetSWOffsetFn;
comm_serv UpdatePaletteFn;
comm_serv UpdateLocalFormatFn;
comm_serv SetNewSWSizeFn;
comm_serv UpdateMouseFn;
comm_serv NotifyClients_StateChangeFn;
comm_serv UpdateClipTextFn;
comm_serv CopyRegionsPoint_to_serverFn;
_shareservermem *serverSharedmem;

comm_serv SendCursorShapeFn;
comm_serv block_inputFn;
comm_serv InitFn;
comm_serv SethookMechanismFn;
comm_serv SetClipTextFn;

comm_serv TriggerUpdateFn;
comm_serv SetBlockInputStateFn;
comm_serv SetSWFn;

comm_serv QueueRectFn;
comm_serv newdesktopFN;
comm_serv deletedesktopFn;
comm_serv UltraEncoder_usedFn;
comm_serv CheckBufferFn;
comm_serv Shared_memory_desktop;
comm_serv LockFn;
comm_serv UnLock;
_sharedesktopmem *desktopsharedmem;


void Init_shared_mem_and_events()
{
	NotifyBalloon.Init("NotifyBalloon",255,0,!fRunningFromExternalService,true);
	Closebyservice.Init("Closebyservice",0,0,!fRunningFromExternalService,true);
	Closebyapp.Init("Closebyapp",0,0,!fRunningFromExternalService,true);
	AuthClientCount.Init("AuthClientCount",0,sizeof(int),!fRunningFromExternalService,true);
	SockConnected.Init("SockConnected",0,1,!fRunningFromExternalService,true);
	KillAuthClients.Init("KillAuthClients",0,0,!fRunningFromExternalService,true);
	KillClient.Init("KillClient",128,0,!fRunningFromExternalService,true);
	TextChatClient.Init("TextChatClient",128,0,!fRunningFromExternalService,true);
	ListAuthClients.Init("ListAuthClients",0,sizeof(_clientlist),!fRunningFromExternalService,true);
	ListUnauthClients.Init("ListUnauthClients",0,sizeof(_clientlist),!fRunningFromExternalService,true);
	Get_g_szRepeaterHost.Init("Get_g_szRepeaterHost",0,256,!fRunningFromExternalService,true);
	Reverse.Init("Reverse",sizeof(_reverse),1,!fRunningFromExternalService,true);
	mouse_eventFn.Init("mouse_event",sizeof(mouseventdata),0,!fRunningFromExternalService,true);
	sas_eventFn.Init("sas_event",0,0,!fRunningFromExternalService,true);
	SendInputFn.Init("SendInput",sizeof(INPUT),0,!fRunningFromExternalService,true);
	ClearShiftKeysFn.Init("ClearShiftKeys",0,0,!fRunningFromExternalService,true);
	keyEventFn.Init("keyEvent",sizeof(keyEventdata),0,!fRunningFromExternalService,true);
	ClearKeyStateFn.Init("ClearKeyState",sizeof(BYTE),0,!fRunningFromExternalService,true);
	AcceptDialogFn.Init("AcceptDialog",sizeof(_acceptdialog),1,!fRunningFromExternalService,true);
	TextChatIPCFn.Init("TextChat",sizeof(_textchatstart),0,!fRunningFromExternalService,true);
	//server functions
	RemoteEventReceivedFn.Init("RemoteEventReceived",0,1,!fRunningFromExternalService,true);
	All_clients_initialalizedFn.Init("All_clients_initialalized",0,1,!fRunningFromExternalService,true);
	IsThereFileTransBusyFn.Init("IsThereFileTransBusy",0,1,!fRunningFromExternalService,true);	
	IsThereAUltraEncodingClientFn.Init("IsThereAUltraEncodingClient",0,1,!fRunningFromExternalService,true);
	UpdateWantedFn.Init("UpdateWanted",0,1,!fRunningFromExternalService,true);	
	UpdateCursorShapeFn.Init("UpdateCursorShape",0,0,!fRunningFromExternalService,true);
	Clear_Update_TrackerFn.Init("Clear_Update_Tracker",0,0,!fRunningFromExternalService,true);
	KillAuthClientsFn.Init("KillAuthClients",0,0,!fRunningFromExternalService,true);
	UpdateMouseFn.Init("UpdateMouse",sizeof(POINT),0,!fRunningFromExternalService,true);	
	SetScreenOffsetFn.Init("SetScreenOffset",sizeof(_SetScreenOffset),0,!fRunningFromExternalService,true);	
	SetSWOffsetFn.Init("SetSWOffset",sizeof(_SetSWOffset),0,!fRunningFromExternalService,true);	
	UpdatePaletteFn.Init("UpdatePalette",1,0,!fRunningFromExternalService,true);	
	UpdateLocalFormatFn.Init("UpdateLocalFormat",1,0,!fRunningFromExternalService,true);	
	SetNewSWSizeFn.Init("SetNewSWSize",sizeof(_SetNewSWSize),0,!fRunningFromExternalService,true);				
	NotifyClients_StateChangeFn.Init("NotifyClients_StateChange",sizeof(_NotifyClients_StateChange),0,!fRunningFromExternalService,true);	
	UpdateClipTextFn.Init("UpdateClipText",0,0,!fRunningFromExternalService,true);
	Shared_memory_server.Init("Shared_memory_server",sizeof(_shareservermem),0,!fRunningFromExternalService,true);
	serverSharedmem=(_shareservermem *)Shared_memory_server.Getsharedmem();
	CopyRegionsPoint_to_serverFn.Init("CopyRegionsPoint_to_server",0,0,!fRunningFromExternalService,true);
	SendCursorShapeFn.Init("SendCursorShape",sizeof(_doublebool),1,!fRunningFromExternalService,true);
	block_inputFn.Init("block_input",0,0,!fRunningFromExternalService,true);
	InitFn.Init("Init",0,1,!fRunningFromExternalService,true);	
	SethookMechanismFn.Init("SethookMechanism",sizeof(_doublebool),0,!fRunningFromExternalService,true);
	SetClipTextFn.Init("SetClipText",0,0,!fRunningFromExternalService,true);
	TriggerUpdateFn.Init("TriggerUpdate",0,0,!fRunningFromExternalService,true);
	SetBlockInputStateFn.Init("SetBlockInputState",sizeof(char),0,!fRunningFromExternalService,true);
	SetSWFn.Init("SetSW",sizeof(_SetSW),0,!fRunningFromExternalService,true);
	QueueRectFn.Init("QueueRect",0,0,!fRunningFromExternalService,true);
	newdesktopFN.Init("newdesktop",0,0,!fRunningFromExternalService,true);
	deletedesktopFn.Init("deletedesktop",0,0,!fRunningFromExternalService,true);
	UltraEncoder_usedFn.Init("UltraEncoder_used",0,1,!fRunningFromExternalService,true);
	CheckBufferFn.Init("checkbuffer",0,0,!fRunningFromExternalService,true);
	Shared_memory_desktop.Init("Shared_memory_desktop",sizeof(_sharedesktopmem),0,!fRunningFromExternalService,true);
	desktopsharedmem=(_sharedesktopmem *)Shared_memory_desktop.Getsharedmem();
	LockFn.Init("LockFn",0,0,!fRunningFromExternalService,true);
	UnLock.Init("UnLock",0,0,!fRunningFromExternalService,true);
}

// Seperate thread for invers connection to avoid lock during timeout;
DWORD WINAPI imp_desktop_thread2(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=Reverse.GetEvent();
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
							_reverse rv;
							char status;
							Reverse.ReadData((char*)&rv);

							char *portp;
							int port;
							bool id;
							char hostname[_MAX_PATH];
							char idcode[_MAX_PATH];
							strcpy_s(hostname,260,rv.host);
							strcpy_s(idcode,260,rv.id);
							

							if (strcmp(idcode,"")==NULL) id=false;
							else id=true;

							// Calculate the Display and Port offset.
							port = INCOMING_PORT_OFFSET;
							portp = strchr(hostname, ':');
							if (portp)
							{
								*portp++ = '\0';
								if (*portp == ':') // Tight127 method
								{
									port = atoi(++portp);		// Port number after "::"
								}
								else // RealVNC method
								{
									if (atoi(portp) < 100)		// If < 100 after ":" -> display number
										port += atoi(portp);
									else
										port = atoi(portp);	    // If > 100 after ":" -> Port number
								}
							}
			
							// Attempt to create a new socket
							VSocket *tmpsock;
							tmpsock = new VSocket;
							if (!tmpsock) {
								return TRUE;
							}
							
							// Connect out to the specified host on the VNCviewer listen port
							// To be really good, we should allow a display number here but
							// for now we'll just assume we're connecting to display zero
							tmpsock->Create();
							if (tmpsock->Connect(hostname, port))
							{
								if (id) 
									{							
										char finalidcode[_MAX_PATH];

										//adzm 2009-06-20
										{
											size_t i = 0;

											for (i = 0; i < strlen(idcode); i++)
											{
												finalidcode[i] = toupper(idcode[i]);
											} 
											finalidcode[i] = 0;

											if (0 != strncmp("ID:", idcode, 3)) {
												strcpy(finalidcode, "ID:");
												
												for (i = 0; i < strlen(idcode); i++)
												{
													finalidcode[i+3] = toupper(idcode[i]);
												} 
												finalidcode[i+3] = 0;
											}
										}

										tmpsock->Send(finalidcode,250);
										tmpsock->SetTimeout(0);
										server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, finalidcode, hostname, port,true);
									} else {

										server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, NULL, hostname, port,true);				
									}
								status=true;
							}
							else
							{
								// Print up an error message
								status=false;
								delete tmpsock;
							}

							Reverse.SetData(&status);
						}
						break;
		}
	}
	return 0;
}

DWORD WINAPI imp_desktop_thread3(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=UpdateWantedFn.GetEvent();	
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 : 
						{
						char result;
						UpdateWantedFn.ReadData(NULL);
						result=server->UpdateWanted();						
						UpdateWantedFn.SetData(&result);
						}
						break;
		}
	}
	return 0;
}

DWORD WINAPI imp_desktop_thread4(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=CopyRegionsPoint_to_serverFn.GetEvent();	
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0: 
						{
						CopyRegionsPoint_to_serverFn.ReadData(NULL);
						get_update(server->GetUpdateTracker());
						CopyRegionsPoint_to_serverFn.SetData(NULL);
						}
						break;
		}
	}
	return 0;
}

DWORD WINAPI imp_desktop_thread5(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=UpdateCursorShapeFn.GetEvent();
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 : 
						{
						UpdateCursorShapeFn.ReadData(NULL);
						server->UpdateCursorShape();
						UpdateCursorShapeFn.SetData(NULL);
						}
						break;
		}
	}
	return 0;
}

DWORD WINAPI imp_desktop_thread6(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=UpdateMouseFn.GetEvent();
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						POINT point;
	#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ UpdateMouseFn 1\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
						UpdateMouseFn.ReadData((char*)&point);
	#ifdef _DEBUG
				//	char			szText[256];
					sprintf(szText," ++++++ UpdateMouseFn 2\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
						server->UpdateMouse(&point);
	#ifdef _DEBUG
		//			char			szText[256];
					sprintf(szText," ++++++ UpdateMouseFn 3\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
						UpdateMouseFn.SetData(NULL);
	#ifdef _DEBUG
			//		char			szText[256];
					sprintf(szText," ++++++ UpdateMouseFn 4\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
						}
						break;
		}
	}
	return 0;
}

DWORD WINAPI imp_desktop_thread7(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	HANDLE event0[1];
	event0[0]=AuthClientCount.GetEvent();
	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(1,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0: 
						{
						int result;
						AuthClientCount.ReadData(NULL);
						result=server->AuthClientCount();
						AuthClientCount.SetData((char *)&result);
						}
						break;
		}
	}
	return 0;
}


DWORD WINAPI imp_desktop_thread(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	ReadProperties m_readproperties;
	if (!m_readproperties.Init(server))
	{
//		desk_dep_desk.Stop_DESK();
		PostQuitMessage(0);
		return 0;
	}

	if (PostAddAutoConnectClient_bool)
		MENU_AUTO_RECONNECT_MSG_fn(server,pszId_char);

	if (PostAddAutoConnectClient_bool_null)
		MENU_AUTO_RECONNECT_MSG_fn(server,NULL);

	if (PostAddConnectClient_bool)
		 MENU_REPEATER_ID_MSG_fn(server,pszId_char);
		
	if (PostAddConnectClient_bool_null)
		MENU_REPEATER_ID_MSG_fn(server,NULL);

	if (PostAddNewClient_bool)
	{
		PostAddNewClient_bool=false;
		vnclog.Print(LL_INTERR, VNCLOG("PostAddNewClient IIIII\n"));
		MENU_ADD_CLIENT_MSG_fn((WPARAM)port_int, (LPARAM)address_vcard,server);
	}
	//adzm 2009-06-20
	if (PostAddNewRepeaterClient_bool)
	{
		PostAddNewRepeaterClient_bool=false;
		vnclog.Print(LL_INTERR, VNCLOG("PostAddNewRepeaterClient II\n"));
		MENU_ADD_CLIENT_MSG_fn( 0xFFFFFFFF, 0xFFFFFFFF,server);
	}


	HANDLE event0[22];

	event0[0]=SockConnected.GetEvent();
	event0[1]=KillAuthClients.GetEvent();
	event0[2]=KillClient.GetEvent();
	event0[3]=TextChatClient.GetEvent();
	event0[4]=ListAuthClients.GetEvent();
	event0[5]=ListUnauthClients.GetEvent();
	event0[6]=Get_g_szRepeaterHost.GetEvent();
	event0[7]=RemoteEventReceivedFn.GetEvent();
	event0[8]=All_clients_initialalizedFn.GetEvent();
	event0[9]=IsThereFileTransBusyFn.GetEvent();		
	event0[10]=IsThereAUltraEncodingClientFn.GetEvent();
	event0[11]=Clear_Update_TrackerFn.GetEvent();
	event0[12]=KillAuthClientsFn.GetEvent();
	event0[13]=SetScreenOffsetFn.GetEvent();
	event0[14]=SetSWOffsetFn.GetEvent();
	event0[15]=UpdatePaletteFn.GetEvent();
	event0[16]=UpdateLocalFormatFn.GetEvent();
	event0[17]=SetNewSWSizeFn.GetEvent();
	event0[18]=NotifyClients_StateChangeFn.GetEvent();
	event0[19]=UpdateClipTextFn.GetEvent();
	event0[20]=Closebyapp.GetEvent();
	event0[21]=sas_eventFn.GetEvent();


	while (!fShutdownOrdered)
	{
		DWORD dwEvent=WaitForMultipleObjects(22,event0,false,1000);
		#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ event %i\n",dwEvent-WAIT_OBJECT_0);
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						char result;
						SockConnected.ReadData(NULL);
						result=server->SockConnected();
						SockConnected.SetData(&result);
						}
						break;
					
					case WAIT_OBJECT_0 + 1: 
						{
							KillAuthClients.ReadData(NULL);
							server->KillAuthClients();
							KillAuthClients.SetData(NULL);

						}
						break;
					case WAIT_OBJECT_0 + 2: 
						{
							char name[128];
							KillClient.ReadData(name);
							server->KillClient(name);
							KillClient.SetData(NULL);

						}
						break;
					case WAIT_OBJECT_0 + 3: 
						{
							char name[128];
							TextChatClient.ReadData(name);
							server->TextChatClient(name);
							TextChatClient.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 4: 
						{
							_clientlist cl;
							ListAuthClients.ReadData(NULL);
							server->ListAuthClients(&cl);
							ListAuthClients.SetData((char*)&cl);
						}
						break;
					case WAIT_OBJECT_0 + 5: 
						{
							_clientlist cl;;
							ListUnauthClients.ReadData(NULL);
							server->ListUnauthClients(&cl);
							ListUnauthClients.SetData((char*)&cl);
						}
						break;
					case WAIT_OBJECT_0 + 6: 
						{
							char name[256];
							Get_g_szRepeaterHost.ReadData(NULL);
							if (g_szRepeaterHost) strcpy_s(name,256,g_szRepeaterHost);
							else strcpy_s(name,256,"");
							Get_g_szRepeaterHost.SetData(name);
						}
						break;
					case WAIT_OBJECT_0 + 7: 
						{
						char result;
						RemoteEventReceivedFn.ReadData(NULL);
						result=server->RemoteEventReceived();
						RemoteEventReceivedFn.SetData(&result);
						}
						break;
					case WAIT_OBJECT_0 + 8: 
						{
						char result;
						All_clients_initialalizedFn.ReadData(NULL);
						result=server->All_clients_initialalized();
						All_clients_initialalizedFn.SetData(&result);
						}
						break;
					case WAIT_OBJECT_0 + 9: 
						{
						char result;
						IsThereFileTransBusyFn.ReadData(NULL);
						result=server->IsThereFileTransBusy();
						IsThereFileTransBusyFn.SetData(&result);
						}
						break;
					case WAIT_OBJECT_0 + 10: 
						{
						char result;
						IsThereAUltraEncodingClientFn.ReadData(NULL);
						result=server->IsThereAUltraEncodingClient();
						IsThereAUltraEncodingClientFn.SetData(&result);
						}
						break;
					
					case WAIT_OBJECT_0 + 11: 
						{
						Clear_Update_TrackerFn.ReadData(NULL);
						server->Clear_Update_Tracker();
						Clear_Update_TrackerFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 12: 
						{
						KillAuthClientsFn.ReadData(NULL);
						server->KillAuthClients();
						KillAuthClientsFn.SetData(NULL);
						}
						break;
					
					case WAIT_OBJECT_0 + 13: 
						{
						 _SetScreenOffset so;
						SetScreenOffsetFn.ReadData((char*)&so);
						server->SetScreenOffset(so.x,so.y,so.mon);
						SetScreenOffsetFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 14: 
						{
						_SetSWOffset so;
						SetSWOffsetFn.ReadData((char*)&so);
						server->SetSWOffset(so.x,so.y);
						SetSWOffsetFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 15: 
						{
						char value;
						UpdatePaletteFn.ReadData((char*) &value);
						UpdatePaletteFn.ReadData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 16: 
						{
						char value;
						UpdateLocalFormatFn.ReadData((char*) &value);
						UpdateLocalFormatFn.ReadData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 17: 
						{
						_SetNewSWSize so;
						SetNewSWSizeFn.ReadData((char*)&so);
						server->SetNewSWSize(so.x,so.y,so.value);
						SetNewSWSizeFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 18: 
						{
						_NotifyClients_StateChange so;
						NotifyClients_StateChangeFn.ReadData((char*)&so);
						server->NotifyClients_StateChange(so.state,so.value);
						NotifyClients_StateChangeFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 19: 
						{
						UpdateClipTextFn.ReadData(NULL);
						server->UpdateClipText(serverSharedmem->cliptext);
						UpdateClipTextFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 20: 
						{
						fShutdownOrdered=true;
						}
						break;
					case WAIT_OBJECT_0 + 21: 
						{
						typedef VOID (WINAPI *SendSas)(BOOL asUser);
						HINSTANCE Inst = LoadLibrary("sas.dll");
						SendSas sendSas = (SendSas) GetProcAddress(Inst, "SendSAS");
						sendSas(FALSE);
						FreeLibrary(Inst);
						}
						break;
					case WAIT_TIMEOUT:
						break;
				}
	}
	return 0;

}

void reverse_connection(vncServer *server,int preset,int recon)
{
	char *portp;
	int port;
	bool id;
	char hostname[_MAX_PATH];
	char idcode[_MAX_PATH];
	char preset1[128];
	char preset1ID[128];
	IniFile myIniFile;
	if (preset==1)
	{
	myIniFile.ReadString("presets", "preset1", preset1,128);
	myIniFile.ReadString("presets", "preset1ID", preset1ID,128);
	}
	else if (preset==2)
	{
	myIniFile.ReadString("presets", "preset2", preset1,128);
	myIniFile.ReadString("presets", "preset2ID", preset1ID,128);
	}
	else if (preset==3)
	{
	myIniFile.ReadString("presets", "preset3", preset1,128);
	myIniFile.ReadString("presets", "preset3ID", preset1ID,128);
	}

	if (recon)
	{
		server->AutoReconnect(true);			
	}


	strcpy_s(hostname,260,preset1);
	strcpy_s(idcode,260,preset1ID);
	if (strcmp(idcode,"")==NULL) id=false;
	else id=true;

	// Calculate the Display and Port offset.
	port = INCOMING_PORT_OFFSET;
	portp = strchr(hostname, ':');
	if (portp)
		{
			*portp++ = '\0';
			if (*portp == ':') // Tight127 method
				{
					port = atoi(++portp);		// Port number after "::"
				}
			else // RealVNC method
				{
					if (atoi(portp) < 100)		// If < 100 after ":" -> display number
						port += atoi(portp);
					else
						port = atoi(portp);	    // If > 100 after ":" -> Port number
				}
		}
			
	// Attempt to create a new socket
	VSocket *tmpsock;
	tmpsock = new VSocket;
	if (!tmpsock) 
		{
			return;
		}
					
	// Connect out to the specified host on the VNCviewer listen port
	// To be really good, we should allow a display number here but
	// for now we'll just assume we're connecting to display zero
	tmpsock->Create();
	if ( recon) server->AutoReconnectPort(port);
	if ( recon) server->AutoReconnectAdr(hostname);
	if (tmpsock->Connect(hostname, port))
		{
			if (id) 
				{							
					char finalidcode[_MAX_PATH];
					//adzm 2009-06-20
					{
						size_t i = 0;
						for (i = 0; i < strlen(idcode); i++)
								{
									finalidcode[i] = toupper(idcode[i]);
								} 
						finalidcode[i] = 0;
						if (0 != strncmp("ID:", idcode, 3)) 
						{
							strcpy(finalidcode, "ID:");				
							for (i = 0; i < strlen(idcode); i++)
								{
									finalidcode[i+3] = toupper(idcode[i]);
								} 
							finalidcode[i+3] = 0;
						}
					}
					if ( recon) server->AutoReconnectId(finalidcode);
					tmpsock->Send(finalidcode,250);
					tmpsock->SetTimeout(0);
					server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, finalidcode, hostname, port,true);
				} 
			else {
					server->AddClient(tmpsock, TRUE, TRUE, 0, NULL, NULL, hostname, port,true);				
				}
			}
		else
			{
				delete tmpsock;
			}
}

DWORD WINAPI tcp_controle_thread(LPVOID lpParam)
{
	vncServer *server = (vncServer *)lpParam;
	controlethread MTC;
	if (!MTC.WSAInit()) return 0;
	if (!MTC.SetupListener()) return 0;
	while(!fShutdownOrdered)
	{
		if(MTC.Accept())
		{
			while(!fShutdownOrdered)
			{
				compack bufpack;
				if (!MTC.read(&bufpack)) break;
				switch(bufpack.command)
				{
					case 0:
						{
						int port=server->GetPort();
						bufpack.int_value=port;
						MTC.write(&bufpack);
						}
						break;
					case 1:
						{
						int port=server->GetPort();
						bufpack.int_value=port;
						MTC.write(&bufpack);
						}
						break;
					case 2:
						{
						int port=server->GetHttpPort();
						bufpack.int_value=port;
						MTC.write(&bufpack);
						}
						break;
					case 3:
						reverse_connection(server,1,0);
						break;
					case 4:
						reverse_connection(server,2,0);
						break;
					case 5:
						reverse_connection(server,3,0);
						break;
					case 6:
						reverse_connection(server,1,1);
						break;
					case 7:
						reverse_connection(server,2,1);
						break;
					case 8:
						reverse_connection(server,3,1);
						break;
				}
			}
		}
	}
	return 0;
}

DWORD WINAPI settings_change_monitor(LPVOID lpParam)
{
	char WORKDIR[MAX_PATH];
	if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
		{
		char* p = strrchr(WORKDIR, '\\');
		if (p == NULL) return 0;
		*p = '\0';
		}
	strcat(WORKDIR,"\\settings\\");

	vncServer *server = (vncServer *)lpParam;
	ReadProperties m_readproperties;
	DWORD dwWaitStatus=0;
	HANDLE event0[1];
	HANDLE file= FindFirstChangeNotification(WORKDIR,false,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if (file == INVALID_HANDLE_VALUE) return 0;
	if (file == NULL) return 0;
	event0[0]=file;
	while (!fShutdownOrdered)
	{
		dwWaitStatus=WaitForMultipleObjects(1, event0, FALSE, 1000); 
		switch (dwWaitStatus) 
			{ 
				case WAIT_OBJECT_0:
					 FindNextChangeNotification(event0[0]);
					 m_readproperties.Init(server);
					 Sleep(1000);
					break;
				case WAIT_TIMEOUT:
					//added to be able to break onfShutdownOrdered
					break;
			}
	}
	FindCloseChangeNotification(event0[0]);
	return 0;
}

void get_update(UpdateTracker &to) {
	Region2D changed;
	Region2D cached;
	Region2D copied;
	Point copy_delta;
	changed.clear();
	cached.clear();
	copied.clear();
	rfb::Rect rect;

	for (int i=0;i<serverSharedmem->changed_pos;i++)
	{
		rect.tl.x=serverSharedmem->changed[i].left;
		rect.tl.y=serverSharedmem->changed[i].top;
		rect.br.x=serverSharedmem->changed[i].right;
		rect.br.y=serverSharedmem->changed[i].bottom;
		changed.assign_union(rect);
	}
	for (int i=0;i<serverSharedmem->copied_pos;i++)
	{
		rect.tl.x=serverSharedmem->copied[i].left;
		rect.tl.y=serverSharedmem->copied[i].top;
		rect.br.x=serverSharedmem->copied[i].right;
		rect.br.y=serverSharedmem->copied[i].bottom;
		copied.assign_union(rect);
	}
	for (int i=0;i<serverSharedmem->cached_pos;i++)
	{
		rect.tl.x=serverSharedmem->cached[i].left;
		rect.tl.y=serverSharedmem->cached[i].top;
		rect.br.x=serverSharedmem->cached[i].right;
		rect.br.y=serverSharedmem->cached[i].bottom;
		cached.assign_union(rect);
	}

	copy_delta.x=serverSharedmem->point.x;
	copy_delta.y=serverSharedmem->point.y;


	if (!copied.is_empty()) {
		to.add_copied(copied, copy_delta);
	}
	if (!changed.is_empty()) {
		to.add_changed(changed);
	}
	if (!cached.is_empty()) {
		to.add_cached(cached);
	}
}