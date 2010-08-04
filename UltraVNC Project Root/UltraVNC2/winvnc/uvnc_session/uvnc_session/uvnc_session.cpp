// uvnc_session.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "uvnc_session.h"
#include "hiddenwindow.h"
#include "communication.h"
#include "keymouse.h"
#include "AcceptDialog_Thread.h"
#include "textchatgui.h"
#include "remoteserver.h"

// Gloval var used to cleanup chatgui thread
// check if not better way is possible
TextChatGui *temppointer=NULL;
HANDLE tempevent=NULL;




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
	typedef BOOL (*SetProcessDPIAwareFunc)();
	SetProcessDPIAwareFunc setDPIAware = (SetProcessDPIAwareFunc)GetProcAddress(hUser32, "SetProcessDPIAware");
	if (setDPIAware) setDPIAware();
	FreeLibrary(hUser32);

	vnclog.SetMode(1);
	vnclog.SetLevel(10);
	app=true;
	hInst=hInstance;		
	if (_stricmp(lpCmdLine, "-service") == 0)
		app=false;
	WORD wVersionRequested;
    WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	// event used to cleanup chatgui thread
	tempevent=CreateEvent(NULL, FALSE, FALSE, "tempevent");

	//worker threads
	hiddenwindow hw;
	keymouse km;
	acceptdialog_thread adt;
	remoteServer rs(&hw);

	// IPC functions
	comm_serv NotifyBalloon;
	comm_serv Closebyservice;
	comm_serv TextChatIPCFn;
	comm_serv Closebyapp;
	if (!Closebyservice.Init("Closebyservice",0,0,app,false)) goto error;
	if (!NotifyBalloon.Init("NotifyBalloon",255,0,app,false)) goto error;
	if (!TextChatIPCFn.Init("TextChat",sizeof(_textchatstart),0,app,false)) goto error;
	if (!Closebyapp.Init("Closebyapp",0,0,app,false))goto error;

	
	//Start wrokers thread
	hw.Start();
	Sleep(500);
	km.Start();
	adt.Start();
	rs.Start();
	

	// loop that listen to IPC request
	HANDLE event0[4];
	event0[0]=NotifyBalloon.GetEvent();
	event0[1]=Closebyservice.GetEvent();
	event0[2]=TextChatIPCFn.GetEvent();
	event0[3]=tempevent;
	while(GLOBAL_RUNNING)
	{
		DWORD dwEvent=WaitForMultipleObjects(4,event0,false,1000);
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ eventNotify %i\n",dwEvent-WAIT_OBJECT_0);
					SetLastError(0);
					if ( dwEvent-WAIT_OBJECT_0 != 258)OutputDebugString(szText);		
#endif
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						char data[1024];
						NotifyBalloon.ReadData(data);
						hw.ShowBalloon(data);
						NotifyBalloon.SetData(NULL);
						break;
					case WAIT_OBJECT_0 + 1: 
						GLOBAL_RUNNING=false;
						break;
					case WAIT_OBJECT_0 + 2:
						{
						 //no need to select desktop
						_textchatstart tcs;
						TextChatIPCFn.ReadData((char*)&tcs);
						TextChatGui *TextChatGuiinstance=new TextChatGui(tcs.name,tcs.pluginenabled,app);
						TextChatGuiinstance->Setpointer(TextChatGuiinstance);
						TextChatIPCFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 3:
						{
							Sleep(200);
							if (temppointer) delete temppointer;
							temppointer=NULL;
						}
						break;
					case WAIT_TIMEOUT:
						if (!hw.Isrunning()) hw.Start();
						break;
				}

	}
	if (app) Closebyapp.Call_Fnction_no_feedback();
	WSACleanup();
	km.Stop();
	hw.Stop();
	adt.Stop();
	rs.Stop();
	hw.wait();
	km.wait();
	adt.wait();
	rs.wait();
	if (tempevent) CloseHandle(tempevent);
	return 0;
error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);
	WSACleanup();
	if (tempevent) CloseHandle(tempevent);
	return 0;
}
