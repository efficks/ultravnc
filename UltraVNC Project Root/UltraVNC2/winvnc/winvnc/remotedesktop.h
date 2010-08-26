#include "stdhdrs.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "rfb.h"
#include <omnithread.h>
#include "uvnc_session/uvnc_session/communication.h"
#include "vncbuffer.h"

#if !defined(_WINVNC_REMOTEDESKTOP)
#define _WINVNC_REMOTEDESKTOP


extern comm_serv SendCursorShapeFn;
extern comm_serv block_inputFn;
extern comm_serv InitFn;
extern comm_serv SethookMechanismFn;
extern comm_serv SetClipTextFn;

extern comm_serv TriggerUpdateFn;
extern comm_serv SetBlockInputStateFn;
extern comm_serv SetSWFn;

extern comm_serv QueueRectFn;
extern comm_serv newdesktopFN;
extern comm_serv deletedesktopFn;
extern comm_serv UltraEncoder_usedFn;
extern comm_serv Shared_memory_desktop;
extern _sharedesktopmem *desktopsharedmem;
extern _shareservermem *serverSharedmem;
extern omni_mutex		m_update_lock;

class remoteDesktop
{
public:
	remoteDesktop();
	~remoteDesktop();

	bool Start();
	void Stop();
    const bool wait() const;
    const HANDLE& GetHandle() const { return m_hThread; }


	vncBuffer *m_bufferpointer;
	rfb::Rect m_cursorpos;

	int ScreenBuffSize();
	rfb::Rect GetSize();
	rfb::Rect MouseRect();
	bool GetBlockInputState();
	void FillDisplayInfo(rfbServerInitMsg *m_scrinfo);
	CRITICAL_SECTION *GetUpdateLock() 
	{
		return &CriticalSection1;
	};

	int SendCursorShape(bool value1,bool value2)
		{
			_doublebool sc;
			sc.value1=value1;
			sc.value2=value2;
			char value;
			#ifdef _DEBUG
										char			szText[256];
										sprintf(szText," SendCursorShapeFn %d %i \n",value1,value2);
										OutputDebugString(szText);		
									#endif
			SendCursorShapeFn.Call_Fnction((char*)&sc,(char*)&value);
			#ifdef _DEBUG
//										char			szText[256];
										sprintf(szText," SendCursorShapeFn %d \n",value);
										OutputDebugString(szText);		
									#endif
			return value;
		};
	void block_input()
		//{return true;};
		//{block_inputFn.Call_Fnction(NULL,NULL);return;};
		{block_inputFn.Call_Fnction_no_feedback();return;};
	bool Init()
		{char value;InitFn.Call_Fnction_Long(NULL,(char*)&value);return value;};
	void SethookMechanism(bool value1,bool value2)
		{_doublebool sh;sh.value1=value1;sh.value2=value2;SethookMechanismFn.Call_Fnction((char*)&sh,NULL);};
	void SetClipText(char *text)
	{if (strlen(text)<64000){strcpy_s(serverSharedmem->cliptext,64000,text);SetClipTextFn.Call_Fnction(NULL,NULL);}};

	void TriggerUpdate()
		{TriggerUpdateFn.Call_Fnction_no_feedback();};
	void SetBlockInputState(bool value)
		{char val;
			val=value;
			SetBlockInputStateFn.Call_Fnction((char*)&val,NULL);
		};
	void SetSW(int x, int y)
		{_SetSW sw;sw.x=x;sw.y=y;SetSWFn.Call_Fnction((char*)&sw,NULL);};
	void QueueRect()
		{QueueRectFn.Call_Fnction_no_feedback();};

	//fake, need to be removed
	char * OptimisedBlitBuffer(){return NULL;};
private:
    remoteDesktop(const remoteDesktop&);
    remoteDesktop& operator=(const remoteDesktop&);

    static unsigned __stdcall threadProc(void*);
    unsigned run();

    HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;
	CRITICAL_SECTION CriticalSection1;

};

#endif

