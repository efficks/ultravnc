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
#include "remoteserver.h"
#include "rfb.h"
#include "vncdesktop.h"

bool CheckVideoDriver(bool Box);

remoteServer::remoteServer(hiddenwindow *IN_hw)
{
	hw=IN_hw;
	ThreadRunning=false;
	m_hThread=NULL;

	RemoteEventReceivedFn.Init("RemoteEventReceived",0,1,app,false);
	All_clients_initialalizedFn.Init("All_clients_initialalized",0,1,app,false);
	IsThereFileTransBusyFn.Init("IsThereFileTransBusy",0,1,app,false);
	
	IsThereAUltraEncodingClientFn.Init("IsThereAUltraEncodingClient",0,1,app,false);
	UpdateWantedFn.Init("UpdateWanted",0,1,app,false);	
	UpdateCursorShapeFn.Init("UpdateCursorShape",0,0,app,false);
	Clear_Update_TrackerFn.Init("Clear_Update_Tracker",0,0,app,false);
	KillAuthClientsFn.Init("KillAuthClients",0,0,app,false);
	UpdateMouseFn.Init("UpdateMouse",sizeof(POINT),0,app,false);	
	SetScreenOffsetFn.Init("SetScreenOffset",sizeof(_SetScreenOffset),0,app,false);	
	SetSWOffsetFn.Init("SetSWOffset",sizeof(_SetSWOffset),0,app,false);	
	UpdatePaletteFn.Init("UpdatePalette",1,0,app,false);	
	UpdateLocalFormatFn.Init("UpdateLocalFormat",1,0,app,false);	
	SetNewSWSizeFn.Init("SetNewSWSize",sizeof(_SetNewSWSize),0,app,false);			
	NotifyClients_StateChangeFn.Init("NotifyClients_StateChange",sizeof(_NotifyClients_StateChange),0,app,false);	
	UpdateClipTextFn.Init("UpdateClipText",0,0,app,false);
	Shared_memory_server.Init("Shared_memory_server",sizeof(_shareservermem),0,app,false);
	serverSharedmem=(_shareservermem *)Shared_memory_server.Getsharedmem();
	CopyRegionsPoint_to_serverFn.Init("CopyRegionsPoint_to_server",0,0,app,false);


	//desktop	
	SendCursorShapeFn.Init("SendCursorShape",sizeof(_doublebool),1,app,false);
	block_inputFn.Init("block_input",0,0,app,false);	
	InitFn.Init("Init",0,1,app,true);	
	SethookMechanismFn.Init("SethookMechanism",sizeof(_doublebool),0,app,false);
	SetClipTextFn.Init("SetClipText",0,0,app,false);

	TriggerUpdateFn.Init("TriggerUpdate",0,0,app,false);
	SetBlockInputStateFn.Init("SetBlockInputState",sizeof(char),0,app,false);
	SetSWFn.Init("SetSW",sizeof(_SetSW),0,app,false);

	QueueRectFn.Init("QueueRect",0,0,app,false);
	newdesktopFN.Init("newdesktop",0,0,app,false);
	deletedesktopFn.Init("deletedesktop",0,0,app,false);
	UltraEncoder_usedFn.Init("UltraEncoder_used",0,1,app,false);
	Shared_memory_desktop.Init("Shared_memory_desktop",sizeof(_sharedesktopmem),0,app,false);
	desktopsharedmem=(_sharedesktopmem *)Shared_memory_desktop.Getsharedmem();
	hw->Set_server_shared_memory(serverSharedmem);
}

remoteServer::~remoteServer()
{
	if (GetHandle()) CloseHandle(GetHandle());
}

bool remoteServer::Start()
{
	if (ThreadRunning==true) return false;
		ThreadRunning=true;
    	m_hThread = reinterpret_cast<HANDLE>(
        ::_beginthreadex(
                0, // security
                0, // stack size
                threadProc, // thread routine
                static_cast<void*>(this), // thread arg
                0, // initial state flag
                &m_tid // thread ID
            )
        );
    	if (m_hThread == 0) 
    	{
	        return false;
	    }
		return true;
}

void remoteServer::Stop()
{
	ThreadRunning=false;
}

const bool remoteServer::wait() const
		{
    	bool bWaitSuccess = false;
		if (!GetHandle()) return true;
    	// a thread waiting on itself will cause a deadlock
    	if (::GetCurrentThreadId() != m_tid)
    	{
	        DWORD nResult = ::WaitForSingleObject(GetHandle(), INFINITE);
        	// nResult will be WAIT_OBJECT_0 if the thread has terminated;
        	// other possible results: WAIT_FAILED, WAIT_TIMEOUT,
        	// or WAIT_ABANDONED
        	bWaitSuccess = (nResult == WAIT_OBJECT_0);
    	}
    	return bWaitSuccess;
	};

unsigned __stdcall remoteServer::threadProc(void* a_param)
{
    remoteServer* pthread = static_cast<remoteServer*>(a_param);
    return pthread->run();
}

bool remoteServer::SelectDesktop()
{
		HDESK desktop;
		HDESK old_desktop;
		DWORD dummy;
		char new_name[256];
		desktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		if (desktop == NULL) return FALSE;
		old_desktop = GetThreadDesktop(GetCurrentThreadId());
		if (!GetUserObjectInformation(desktop, UOI_NAME, &new_name, 256, &dummy)) {
			CloseDesktop(desktop);
			return FALSE;
		}
		if(!SetThreadDesktop(desktop)) {
			CloseDesktop(desktop);
			return FALSE;
		}
		CloseDesktop(old_desktop);			
		return TRUE;

}

bool remoteServer::InputDesktopSelected()
{

	DWORD dummy;
	char threadname[256];
	char inputname[256];
	HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
	HDESK inputdesktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);

	if (inputdesktop == NULL) return FALSE;
	if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) {
			CloseDesktop(inputdesktop);
			return FALSE;
		}
	if (!GetUserObjectInformation(inputdesktop, UOI_NAME, &inputname, 256, &dummy)) {
			CloseDesktop(inputdesktop);
			return FALSE;
		}
	CloseDesktop(inputdesktop);
	if (strcmp(threadname, inputname) != 0)
		{
			//if (strcmp(inputname, "Screen-saver") == 0)
			{
				return SelectDesktop();
			}
			return FALSE;
		}
	return TRUE;
}

unsigned remoteServer::run()
{
			vncDesktop *m_desktop=NULL;
			HANDLE Events[12];		
			Events[0]=newdesktopFN.GetEvent();
			Events[1]=deletedesktopFn.GetEvent();
			Events[2]=InitFn.GetEvent();	
			Events[3]=SendCursorShapeFn.GetEvent();
			Events[4]=block_inputFn.GetEvent();
			Events[5]=SethookMechanismFn.GetEvent();
			Events[6]=SetClipTextFn.GetEvent();
			Events[7]=TriggerUpdateFn.GetEvent();
			Events[8]=SetBlockInputStateFn.GetEvent();
			Events[9]=SetSWFn.GetEvent();
			Events[10]=QueueRectFn.GetEvent();
			Events[11]=UltraEncoder_usedFn.GetEvent();
			 //check done input desktop auto selected
			InputDesktopSelected();
			if (hw->Viewer_connected())
								{
									if (m_desktop == NULL)
									{
										// We need to check if aero/wallpaper need to be set
										hw->Wallpaperaero();
										m_desktop = new vncDesktop();
										if (m_desktop) m_desktop->Init(this);
									}
								}
			while (ThreadRunning)
				{ 
					DWORD dwEvent = WaitForMultipleObjects(12,Events,FALSE,1000);
					#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ eventnewdesk %i\n",dwEvent-WAIT_OBJECT_0);
					DWORD aa=GetLastError();
					if ( dwEvent-WAIT_OBJECT_0 != 258) OutputDebugString(szText);		
			#endif
					switch(dwEvent)
						{
							case WAIT_OBJECT_0 + 0:
								newdesktopFN.ReadData(NULL);
								if (m_desktop == NULL)
									{
										// We need to check if aero/wallpaper need to be set
										hw->Wallpaperaero();
										m_desktop = new vncDesktop();
									}								
								newdesktopFN.SetData(NULL);
							break;
							case WAIT_OBJECT_0 + 1:
								deletedesktopFn.ReadData(NULL);
								if (m_desktop) delete m_desktop;	
								m_desktop=NULL;
								deletedesktopFn.SetData(NULL);
							break;
							case WAIT_OBJECT_0 + 2:
								char status;
								InitFn.ReadData(NULL);
								if (m_desktop) status=m_desktop->Init(this);							
 								InitFn.SetData(&status);
							break;
							
							case WAIT_OBJECT_0 + 3:
								{
									char value;
									_doublebool cs;
									SendCursorShapeFn.ReadData((char*)&cs);
									if (m_desktop) value=m_desktop->SendCursorShape(cs.value1,cs.value2);
									SendCursorShapeFn.SetData((char*)&value);
								}
								break;
							case WAIT_OBJECT_0 + 4:
								{
									//char value;
									//block_inputFn.ReadData(&value);
									if (m_desktop) m_desktop->block_input();
									//block_inputFn.SetData((char*)&value);
								}
								break;
							
							

							case WAIT_OBJECT_0 + 5:
								{
									_doublebool sh;
									SethookMechanismFn.ReadData((char*)&sh);
									if (m_desktop) m_desktop->SethookMechanism(sh.value1,sh.value2);
									SethookMechanismFn.SetData(NULL);
								}
								break;
							case WAIT_OBJECT_0 + 6:
									{
										SetClipTextFn.ReadData(NULL);
										if (m_desktop) m_desktop->SetClipText(serverSharedmem->cliptext);
										SetClipTextFn.SetData(NULL);
									}
									break;

							case WAIT_OBJECT_0 + 7:
									{
										//TriggerUpdateFn.ReadData(NULL);
										if (m_desktop) m_desktop->TriggerUpdate();
										//TriggerUpdateFn.SetData(NULL);
									}
									break;

							case WAIT_OBJECT_0 + 8:
								{
									char value;
									SetBlockInputStateFn.ReadData((char*)&value);
									if (m_desktop) m_desktop->SetBlockInputState(value);
									SetBlockInputStateFn.SetData(NULL);
								}
								break;

							case WAIT_OBJECT_0 + 9:
								{
									_SetSW sw;
									SetSWFn.ReadData((char*)&sw);
									if (m_desktop) m_desktop->SetSW(sw.x,sw.y);
									SetSWFn.SetData(NULL);
								}
								break;
			
							case WAIT_OBJECT_0 +10:
								{
									//rect isn't used
									rfb::Rect rfbrect;
									//QueueRectFn.ReadData(NULL);
									if (m_desktop) m_desktop->QueueRect(rfbrect);
									//QueueRectFn.SetData(NULL);
								}
								break;
			
							case WAIT_OBJECT_0 + 11:
								{
									int value;
									UltraEncoder_usedFn.ReadData(NULL);
									if (m_desktop) value=m_desktop->m_UltraEncoder_used;
									UltraEncoder_usedFn.SetData((char*)&value);
								}
								break;
							case WAIT_TIMEOUT:
								InputDesktopSelected();
								break;
								
						}
				}
			ThreadRunning=false;
			if (m_desktop) delete m_desktop;
			m_desktop=NULL;
    		return 0;
}