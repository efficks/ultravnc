#if (!defined(_WINVNC_COMM))
#define _WINVNC_COMM

#include "stdafx.h"
#include "rfb.h"
#define TEXTMAXSIZE 16384
struct _clientlist
{
	int size;
	char szDescription[20][256];
};

struct _reverse
{
	char host[260];
	char id[260];
};

struct mouseventdata
{
	DWORD dwFlags;
	DWORD dx;
	DWORD dy;
	DWORD dwData;
};

struct keyEventdata
{
	unsigned long keysym;
	bool down;
	bool jap;
	int version;
	bool from_service;
};

struct _acceptdialog
{
	unsigned char verified;
	bool QueryIfNoLogon;
	unsigned int QueryAccept;
	unsigned int QueryTimeout;
	char PeerName[32];
};

struct _textchatdata
{
	char	m_szLocalText[TEXTMAXSIZE];
	char	m_szLastLocalText[TEXTMAXSIZE];
	char	m_szRemoteText[TEXTMAXSIZE];	
	char	m_szTextBoxBuffer[TEXTMAXSIZE];
};

struct _textchatstart
{
	char name[128];
	unsigned char pluginenabled;
};

struct _shareservermem
{
	bool m_driver;
	bool m_fCaptureAlphaBlending;
	bool m_fBlackAlphaBlending;
	bool m_fBlankMonitorEnabled;
	bool m_poll_fullscreen;
	bool m_poll_oneventonly;
	bool m_poll_consoleonly;
	bool m_poll_foreground;
	bool m_poll_undercursor;

	bool	m_remove_wallpaper;
	bool	m_remove_Aero;
	bool	m_disableTrayIcon;
	bool	m_AllowEditClients;

	bool m_fBlankInputsOnly;
	bool m_TurboMode;
	bool m_fXRichCursor;
	int m_MaxCpu;
	bool m_hook;
	char m_szWindowName[32];
	bool m_disable_local_inputs;
	RECT changed[2000];
	int changed_pos;
	RECT cached[2000];
	int cached_pos;
	RECT copied[2000];
	int copied_pos;
	POINT point;
	bool SWAPBUTTON;
	int XVIRTUALSCREEN;
	int YVIRTUALSCREEN;
	int CXVIRTUALSCREEN;
	int CYVIRTUALSCREEN;
	char cliptext[64000];


};

struct _SetScreenOffset
{
	int x;
	int y;
	int mon;
};

struct _SetSWOffset
{
	int x;
	int y;
};

struct _SetNewSWSize
{
	int x;
	int y;
	bool value;
};

struct _NotifyClients_StateChange
{
	int value;
	unsigned long state;
};

struct _doublebool
{
	bool value1;
	bool value2;
};

struct _SetSW
{
	int x;
	int y;
};

struct monitor
{
	int Width;
	int Height;
	int Depth;
	char device[32];
	int offsetx;
	int offsety;
};

struct _sharedesktopmem
{
	void *m_videodriver;
	int m_SWOffsetx;
	int m_SWOffsety;
	rfbServerInitMsg	m_scrinfo;
	monitor mymonitor[3];
	bool multi_monitor;

	//cursorshape
	BYTE mbits[64*64*4];
	BYTE cbits[64*64*4];
	int width;
	int bmHeight;
	int bmWidthBytes;
	int xHotspot;
	int yHotspot;
	int height;

	RECT m_cursorpos_rect;
	bool m_bIsInputDisabledByClient; 
};

class comm_serv
{
public:
    comm_serv();
	virtual ~comm_serv();
	bool Init(char *name,int datasize_IN,int datasize_OUT,bool app,bool master);
	HANDLE GetEvent();
	char *Getsharedmem();
	void ReadData(char *databuffer);
	void SetData(char *databuffer);
	void Call_Fnction(char *databuffer_IN,char *databuffer_OUT);
	void Call_Fnction_no_feedback();
	void Call_Fnction_Long(char *databuffer_IN,char *databuffer_OUT);
	HANDLE InitFileHandle(char *name,int IN_datasize_IN,int IN_datasize_OUT,bool app,bool master);
	void Force_unblock();
	void Release();

private:
	HANDLE event_E_IN;
	HANDLE event_E_IN_DONE;
	HANDLE event_E_OUT;
	HANDLE event_E_OUT_DONE;
	HANDLE hMapFile_IN;
	HANDLE hMapFile_OUT;
	char *data_IN;
	char *data_OUT;
	CRITICAL_SECTION CriticalSection_IN;
	CRITICAL_SECTION CriticalSection_OUT;
	SECURITY_ATTRIBUTES secAttr;
	void create_sec_attribute();
	char filemapping_IN[64];
	char filemapping_OUT[64];
	char event_IN[64];
	char event_IN_DONE[64];
	char event_OUT[64];
	char event_OUT_DONE[64];
	int datasize_IN;
	int datasize_OUT;
	int timeout_counter;
	
};
#endif