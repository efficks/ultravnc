#include "stdafx.h"
#include "hiddenwindow.h"
#include "communication.h"
#include "wallaero.h"
#include <tlhelp32.h>
#include "vncconndialog.h"

hiddenwindow::hiddenwindow()
{
	// Initialize global strings
	WTSProcessIdToSessionIdF=NULL;
	ThreadRunning=false;
	m_hThread=NULL;
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_UVNC_SESSION, szWindowClass, MAX_LOADSTRING);
	//TrayIcon= new CSystemTray;
	load=NULL;
	free=NULL;
	nr_clients_connected=0;
	if (!AuthClientCount.Init("AuthClientCount",0,sizeof(int),app,false)) goto error;
	if (!SockConnected.Init("SockConnected",0,1,app,false)) goto error;
	if (!KillAuthClients.Init("KillAuthClients",0,0,app,false)) goto error;
	return;
	error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);
	serverSharedmem=NULL;
}

hiddenwindow::~hiddenwindow()
{
	//if (TrayIcon) delete TrayIcon;
	if (GetHandle()) CloseHandle(GetHandle());
}


void hiddenwindow::ShowBalloon(LPCTSTR szText, LPCTSTR szTitle,
                     DWORD dwIcon, UINT uTimeout)
{
	TrayIcon->ShowBalloon(szText, szTitle,dwIcon ,uTimeout);
}

void hiddenwindow::SetThreadDesktopName()
{
	DWORD dummy;
	HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
	if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) {
			return ;
		}
}

bool hiddenwindow::Check_Inputdesktop()
{
	DWORD dummy;
	HDESK inputdesktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	if (inputdesktop == NULL) return FALSE;
	if (!GetUserObjectInformation(inputdesktop, UOI_NAME, &inputname, 256, &dummy)) {
			CloseDesktop(inputdesktop);
			return FALSE;
		}
	CloseDesktop(inputdesktop);
		if (strcmp(threadname, inputname) != 0)
		{
			return false;
		}
	return true;
}

bool hiddenwindow::Set_Inputdesktop()
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
			DWORD aa=GetLastError();
			CloseDesktop(desktop);
			return FALSE;
		}
	strcpy_s(threadname,256,new_name);
	CloseDesktop(old_desktop);			
	return TRUE;
}

bool hiddenwindow::Isrunning()
{
	return ThreadRunning;
}

unsigned int hiddenwindow::run()
{
	SetThreadDesktopName();
	while (GLOBAL_RUNNING)
	{
		//only load the settings once
		//else if user change to off, wallpaper is never restored
		//now settings are activated on next run, after pervious settings were used to restore
		bool_RemoveWallpaperEnabled=true;
		bool_RemoveAeroEnabled=true;
		bool_RemoveWallpaperEnabled=serverSharedmem->m_remove_wallpaper;
		bool_RemoveAeroEnabled=serverSharedmem->m_remove_Aero;
		bool_GetAllowEditClients=serverSharedmem->m_AllowEditClients;
		
		// TODO: Place code here.
		TrayIcon= new CSystemTray;
		while (GLOBAL_RUNNING)
		{
			if (!Check_Inputdesktop())
			{
				if (!Set_Inputdesktop())
				{
					Sleep(1000);
					ThreadRunning=false;
					if (TrayIcon) delete TrayIcon;
					TrayIcon=NULL;
					return 0;
				}
				else break;
			}
			else break;
		}
		serverSharedmem->SWAPBUTTON=GetSystemMetrics(SM_SWAPBUTTON);
		serverSharedmem->XVIRTUALSCREEN=GetSystemMetrics(SM_XVIRTUALSCREEN);
		serverSharedmem->YVIRTUALSCREEN=GetSystemMetrics(SM_YVIRTUALSCREEN);
		serverSharedmem->CXVIRTUALSCREEN=GetSystemMetrics(SM_CXVIRTUALSCREEN);
		serverSharedmem->CYVIRTUALSCREEN=GetSystemMetrics(SM_CYVIRTUALSCREEN);

		MSG msg;
		HACCEL hAccelTable;
		MyRegisterClass(hInst);
		if (!InitInstance (hInst)) goto error;

		hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_UVNC_SESSION));
		free=LoadIcon(hInst, (LPCTSTR)IDI_WINVNC);
		load=LoadIcon(hInst, (LPCTSTR)IDI_FLASH);

		if (!TrayIcon->Create(hInst,hWnd_HW, WM_ICON_NOTIFY,"Initializing", free,IDC_UVNC_SESSION)) 
	        goto error;

		if (hWnd_HW==NULL) goto error;
		#ifndef _X64
		SetWindowLong(hWnd_HW, GWL_USERDATA, (long)this);
		#else
		SetWindowLongPtr(hWnd_HW, GWLP_USERDATA, (long)this);
		#endif

		SetTimer(hWnd_HW, 100, 1000, NULL);
		SetTimer(hWnd_HW, 200, 4000, NULL);
		strcpy_s(m_old_username,UNLEN+1,"init");
		while (GetMessage(&msg, NULL, 0, 0) && GLOBAL_RUNNING)
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (bool_RemoveWallpaperEnabled)
						RestoreWallpaper();
		if (bool_RemoveAeroEnabled)
						ResetAero();

		TrayIcon->RemoveIcon();
		if (TrayIcon) delete TrayIcon;
		TrayIcon=NULL;
		//GLOBAL_RUNNING=false;
		hWnd_HW=NULL;
		continue;
error:
		Sleep(5000);
	}
	return 0;
}

void hiddenwindow::Stop()
{
	ThreadRunning=false;
}

const bool hiddenwindow::wait() const
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
}

unsigned __stdcall hiddenwindow::threadProc(void* a_param)
{
    hiddenwindow* pthread = static_cast<hiddenwindow*>(a_param);
    return pthread->run();
}

bool hiddenwindow::Start()
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

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM hiddenwindow::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UVNC_SESSION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_UVNC_SESSION);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL hiddenwindow::InitInstance(HINSTANCE hInstance)
{
    hWnd_HW = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd_HW)
   {
      return FALSE;
   }
   // Create the tray icon
 //  ShowWindow(hWnd_HW, SW_SHOW);
   UpdateWindow(hWnd_HW);
   return TRUE;
}

static bool running=false;
DWORD WINAPI vncConnDialogThread(LPVOID lpParam)
  {
	  running=true;
	  HDESK desktop;
		desktop = OpenInputDesktop(0, FALSE,
									DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
									DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
									DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
									DESKTOP_SWITCHDESKTOP | GENERIC_WRITE
								);

	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
	SetThreadDesktop(desktop);
	vncConnDialog *newconn = new vncConnDialog();
		if (newconn)
			{
				newconn->DoDialog();
			}
	SetThreadDesktop(old_desktop);
	CloseDesktop(desktop);
	running=false;
	return 0;
  }

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK hiddenwindow::WndProc(HWND hWndProc, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

#ifndef _X64
	hiddenwindow *_this = (hiddenwindow *) GetWindowLong(hWndProc, GWL_USERDATA);
#else
	hiddenwindow *_this = (hiddenwindow *) GetWindowLongPtr(hWndProc, GWLP_USERDATA);
#endif

	switch (message)
	{
		case WM_QUERYENDSESSION:
		{
			//shutdown or reboot
			if((lParam & ENDSESSION_LOGOFF) != ENDSESSION_LOGOFF)
			{
				GLOBAL_RUNNING=false;
				DestroyWindow(hWndProc);
				break;
			}
			DWORD SessionID;
			SessionID=_this->GetCurrentSessionID();
			if (SessionID!=0)
			{
				GLOBAL_RUNNING=false;
				DestroyWindow(hWndProc);
				break;
			}
		}	
		break;
		case WM_TIMER:
			if (wParam==100)
			{
			if (!_this->Check_Inputdesktop()) 
				{
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++  Check_Inputdesktop \n");
					SetLastError(0);
					OutputDebugString(szText);		
#endif
					DestroyWindow(hWndProc);
			}
			if (!_this->ThreadRunning) DestroyWindow(hWndProc);
			if (!GLOBAL_RUNNING) DestroyWindow(hWndProc);
			}
			if (wParam==200)
			{
			bool_GetAllowEditClients=_this->serverSharedmem->m_AllowEditClients;
			if (_this->serverSharedmem->m_disableTrayIcon) _this->TrayIcon->HideIcon();
			else _this->TrayIcon->ShowIcon();

			if (_this->bool_RemoveWallpaperEnabled!=_this->serverSharedmem->m_remove_wallpaper || _this->bool_RemoveAeroEnabled!=_this->serverSharedmem->m_remove_Aero)
			{
				DestroyWindow(hWndProc);
			}
			// allow to kill cleints
			//connect/idle change icon
			int int_status;
			_this->AuthClientCount.Call_Fnction(NULL,(char*)&int_status);
			if (int_status>0) _this->TrayIcon->SetIcon(_this->load);
			else  _this->TrayIcon->SetIcon(_this->free);
			bool result=GetCurrentUser(_this->m_username, sizeof(_this->m_username),app);
			if (result && (_stricmp(_this->inputname, "System") != 0) && (_stricmp(_this->inputname, "") != 0))
			if ((_this->nr_clients_connected!=int_status)  && (_stricmp(_this->inputname, "Default") == 0) )
			{
				if (int_status>0 && _this->nr_clients_connected==0)
				{
					if (_this->bool_RemoveWallpaperEnabled) //PGM @ Advantig
						KillWallpaper();
					if (_this->bool_RemoveAeroEnabled) //PGM @ Advantig
						DisableAero(); 
				}
				else if (int_status==0 && _this->nr_clients_connected!=0)
				{
					if (_this->bool_RemoveWallpaperEnabled)
						RestoreWallpaper();
					if (_this->bool_RemoveAeroEnabled)
						ResetAero();
				}
				_this->nr_clients_connected=int_status;
			}
			if(result)
			{
				if ((_stricmp(_this->m_old_username, _this->m_username) != 0) && (_stricmp(_this->m_old_username, "init") != 0))
				{
					//username was set, and user name changed
					//restart hidden window
					DestroyWindow(hWndProc);
				}
				strcpy_s(_this->m_old_username,UNLEN+1,_this->m_username);
			}
			//show ip as tooltip
			char status=0;
			_this->SockConnected.Call_Fnction(NULL,&status);
			bool bool_status=status;
			if (bool_status)
				{
					char text[1024];
					_this->GetIPAddrString(text, 1024);
					_this->TrayIcon->SetTooltipText(text);
				}
			else
				{
					_this->TrayIcon->SetTooltipText("Not listening");
				}



			}

			break;
		case WM_ICON_NOTIFY:
            return _this->TrayIcon->OnTrayNotification(wParam, lParam);
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndProc, hiddenwindow::About);
				break;
			case IDM_EXIT:
				GLOBAL_RUNNING=false;
				DestroyWindow(hWndProc);
				break;

			case ID_POPUP_KILLALLCONNECTIONS:
				_this->KillAuthClients.Call_Fnction(NULL,NULL);
				break;
			case ID_POPUP_LISTCONNECTIONS:
				_this->m_ListDlg.Display();
				break;
			case ID_POPUP_CONNECTTO:
				{
					if (running) break;
					DWORD dw;
	  				HANDLE myhandle=NULL;
					myhandle=CreateThread(NULL,0,vncConnDialogThread,NULL,0,&dw);
	  				if (myhandle) CloseHandle(myhandle);
				}
				break;

			default:
				return DefWindowProc(hWndProc, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWndProc, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWndProc, &ps);
			break;
		//case WM_DISPLAYCHANGE:
		case WM_DESTROY:
			//GLOBAL_RUNNING=false;
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++  WM_DESTROY \n");
					SetLastError(0);
					OutputDebugString(szText);		
#endif
			PostQuitMessage(0);
			break;
		default:
		return DefWindowProc(hWndProc, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK hiddenwindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void hiddenwindow::GetIPAddrString(char *buffer, int buflen) {
	if (old_buflen!=0 && counter<1000)
	{
		counter++;
		strcpy_s(buffer,buflen,old_buffer);
		return;
	}
	counter=0;
    char namebuf[256];

    if (gethostname(namebuf, 256) != 0) {
		strncpy_s(buffer,buflen,"Host name unavailable", buflen);
		return;
    };

    HOSTENT *ph = gethostbyname(namebuf);
    if (!ph) {
		strncpy_s(buffer,buflen, "IP address unavailable", buflen);
		return;
    };

    *buffer = '\0';
    char digtxt[5];
    for (int i = 0; ph->h_addr_list[i]; i++) {
    	for (int j = 0; j < ph->h_length; j++) {
			sprintf_s(digtxt, 5,"%d.", (unsigned char) ph->h_addr_list[i][j]);
			strncat_s(buffer,buflen, digtxt, (buflen-1)-strlen(buffer));
		}	
		buffer[strlen(buffer)-1] = '\0';
		if (ph->h_addr_list[i+1] != 0)
			strncat_s(buffer,buflen, ", ", (buflen-1)-strlen(buffer));
    }
	if (strlen(buffer)<512) // just in case it would be bigger then our buffer
	{
	old_buflen=strlen(buffer);
	strncpy_s(old_buffer,1024,buffer,strlen(buffer));
	}
}

DWORD hiddenwindow::GetCurrentSessionID()
{
	DWORD dwSessionId;
	pWTSGetActiveConsoleSessionId WTSGetActiveConsoleSessionIdF=NULL;
	WTSProcessIdToSessionIdF=NULL;

	HMODULE  hlibkernel = LoadLibrary("kernel32.dll");
	if (hlibkernel)
	{
	WTSGetActiveConsoleSessionIdF=(pWTSGetActiveConsoleSessionId)GetProcAddress(hlibkernel, "WTSGetActiveConsoleSessionId");
	WTSProcessIdToSessionIdF=(pProcessIdToSessionId)GetProcAddress(hlibkernel, "ProcessIdToSessionId");
	}
	if (WTSGetActiveConsoleSessionIdF!=NULL)
	   dwSessionId =WTSGetActiveConsoleSessionIdF();
	else dwSessionId=0;

	if( GetSystemMetrics( SM_REMOTESESSION))
		if (WTSProcessIdToSessionIdF!=NULL)
		{
			DWORD dw		 = GetCurrentProcessId();
			DWORD pSessionId = 0xFFFFFFFF;
			WTSProcessIdToSessionIdF( dw, &pSessionId );
			dwSessionId=pSessionId;
		}
	if (hlibkernel) FreeLibrary(hlibkernel);
	return dwSessionId;
}

void hiddenwindow::Wallpaperaero()
{
			if (serverSharedmem->m_disableTrayIcon) TrayIcon->HideIcon();
			else TrayIcon->ShowIcon();
			// allow to kill cleints
			//connect/idle change icon
			int int_status;
			AuthClientCount.Call_Fnction(NULL,(char*)&int_status);
			if (int_status>0) TrayIcon->SetIcon(load);
			else  TrayIcon->SetIcon(free);
			bool result=GetCurrentUser(m_username, sizeof(m_username),app);
			if (result && (_stricmp(inputname, "System") != 0) && (_stricmp(inputname, "") != 0))
			if ((nr_clients_connected!=int_status)  && (_stricmp(inputname, "Default") == 0) )
			{
				if (int_status>0 && nr_clients_connected==0)
				{
					if (bool_RemoveWallpaperEnabled) //PGM @ Advantig
						KillWallpaper();
					if (bool_RemoveAeroEnabled) //PGM @ Advantig
						DisableAero(); 
				}
				else if (int_status==0 && nr_clients_connected!=0)
				{
					if (bool_RemoveWallpaperEnabled)
						RestoreWallpaper();
					if (bool_RemoveAeroEnabled)
						ResetAero();
				}
				nr_clients_connected=int_status;
			}
			
}


bool hiddenwindow::Viewer_connected()
{

			int int_status;
			AuthClientCount.Call_Fnction(NULL,(char*)&int_status);
			if (int_status>0) return true;
			else return false;
			
}

void hiddenwindow::Set_server_shared_memory( _shareservermem *IN_serverSharedmem)
{
	serverSharedmem=IN_serverSharedmem;
}