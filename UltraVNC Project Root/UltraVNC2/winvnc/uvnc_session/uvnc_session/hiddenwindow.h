#include "stdafx.h"
#include "resource.h"
#include "SystemTray.h"
#include "vncListDlg.h"
#include "communication.h"

#ifndef HIDDENWINDOW
#define HIDDENWINDOW

#define MAX_LOADSTRING 100
#define	WM_ICON_NOTIFY WM_APP+10

typedef DWORD (WINAPI* pWTSGetActiveConsoleSessionId)(VOID);
typedef BOOL (WINAPI * pProcessIdToSessionId)(DWORD,DWORD*);

class hiddenwindow
{
public:
    hiddenwindow();
	virtual ~hiddenwindow();
	ATOM				MyRegisterClass(HINSTANCE hInstance);
	BOOL				InitInstance(HINSTANCE);
	CSystemTray *TrayIcon;
	bool Start();
	void Stop();
	const bool wait() const;
	const HANDLE& GetHandle() const { return m_hThread; }
	void ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);
	void Wallpaperaero();
	bool Viewer_connected();
	void Set_server_shared_memory( _shareservermem *IN_serverSharedmem);
	bool Isrunning();

private:
	hiddenwindow(const hiddenwindow&);
    hiddenwindow& operator=(const hiddenwindow&);

    static unsigned __stdcall threadProc(void*);
    unsigned int run();

	HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;

	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM); 
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	bool Check_Inputdesktop();
	bool Set_Inputdesktop();
	void SetThreadDesktopName();	
	char threadname[256];
	char inputname[256];
	HWND hWnd_HW;

	HICON load;
	HICON free;
	int nr_clients_connected;
	bool bool_RemoveWallpaperEnabled;
	bool bool_RemoveAeroEnabled;
	char m_username[UNLEN+1];
	char m_old_username[UNLEN+1];
	DWORD GetCurrentSessionID();
	void GetIPAddrString(char *buffer, int buflen);

	vncListDlg		m_ListDlg;

	comm_serv AuthClientCount;
	comm_serv SockConnected;
	comm_serv KillAuthClients;
	pProcessIdToSessionId WTSProcessIdToSessionIdF;
	 _shareservermem *serverSharedmem;
	bool bool_disableTrayIcon;
	bool bool_AllowEditClients;
};

#endif