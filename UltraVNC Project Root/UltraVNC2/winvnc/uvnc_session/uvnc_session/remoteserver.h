#include "stdafx.h"
#include "rfbUpdateTracker.h"
#include "communication.h"
#include "videodriver.h"
#include "hiddenwindow.h"

extern _sharedesktopmem *desktopsharedmem;

// Call to server class are forwarded to the server
// Calls from the server to thye desktop class are handled in the thread  run()
extern bool app;

class remoteServer
{
private:
   		remoteServer(const remoteServer&);
    	remoteServer& operator=(const remoteServer&);
    	static unsigned __stdcall threadProc(void* a_param);
    	unsigned run();
    	HANDLE m_hThread;
    	unsigned m_tid;
		bool ThreadRunning;
		bool InputDesktopSelected();
		bool SelectDesktop();
public:
	remoteServer(hiddenwindow *hw);
	virtual ~remoteServer();
	bool Start();
	void Stop();
    const bool wait() const;
    const HANDLE& GetHandle() const { return m_hThread; }
	hiddenwindow *hw;
	//server
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
	comm_serv Shared_memory_server;
	 _shareservermem *serverSharedmem;

	// desktop	
	comm_serv SendCursorShapeFn;
	comm_serv block_inputFn;
	comm_serv InitFn;	
	comm_serv SethookMechanismFn;
	comm_serv SetClipTextFn;

	comm_serv TriggerUpdateFn;
	comm_serv SetBlockInputStateFn;
	comm_serv SetSWFn;

	comm_serv QueueRectFn;
	comm_serv SetDisableInputFn;
	comm_serv newdesktopFN;
	comm_serv deletedesktopFn;
	comm_serv UltraEncoder_usedFn;
	comm_serv Shared_memory_desktop;

	bool RemoteEventReceived(){ char value;RemoteEventReceivedFn.Call_Fnction(NULL,&value);return value;};	
	bool All_clients_initialalized(){ char value;All_clients_initialalizedFn.Call_Fnction(NULL,&value);return value;};	
	bool IsThereFileTransBusy(){ char value;IsThereFileTransBusyFn.Call_Fnction(NULL,&value);return value;};	
	bool IsXRichCursorEnabled(){ return serverSharedmem->m_fXRichCursor;};	
	bool IsThereAUltraEncodingClient(){ char value;IsThereAUltraEncodingClientFn.Call_Fnction(NULL,&value);return value;};	
	bool UpdateWanted(){ char value;UpdateWantedFn.Call_Fnction(NULL,&value);return value;};	
	void UpdateCursorShape(){UpdateCursorShapeFn.Call_Fnction(NULL,NULL);};	
	void Clear_Update_Tracker(){Clear_Update_TrackerFn.Call_Fnction(NULL,NULL);};	
	void KillAuthClients(){KillAuthClientsFn.Call_Fnction(NULL,NULL);};	
	void UpdateMouse(){POINT point;GetCursorPos(&point);UpdateMouseFn.Call_Fnction((char*)&point,NULL);};	

	void SetScreenOffset(int x,int y,int monitor){ _SetScreenOffset so;so.x=x;so.y=y;so.mon=monitor;SetScreenOffsetFn.Call_Fnction((char*)&so,NULL);};	
	void SetSWOffset(int x,int y){ _SetSWOffset so;so.x=x;so.y=y;SetSWOffsetFn.Call_Fnction((char*)&so,NULL);};
	void UpdatePalette(bool value){ char value1;value1=value;UpdatePaletteFn.Call_Fnction(&value1,NULL);};
	void UpdateLocalFormat(bool value){ char value1;value1=value;UpdateLocalFormatFn.Call_Fnction(&value1,NULL);};
	void SetNewSWSize(int w, int h, bool value){ _SetNewSWSize so;so.x=w;so.y=h;so.value=value;SetNewSWSizeFn.Call_Fnction((char*)&so,NULL);};	
	/// complex functio, need to sync regions
	void CopyRegionsPoint_to_server(rfb::Region2D *changedrgn,rfb::Region2D *cachedrgn,rfb::Region2D *copiedrgn,rfb::Point *copy_delta)
	{
		rfb::RectVector cached;
		rfb::RectVector copied;
		rfb::RectVector changed;
		cached.clear();
		copied.clear();
		changed.clear();
		cachedrgn->get_rects(cached, 1, 1);
		copiedrgn->get_rects(copied, 1, 1);
		changedrgn->get_rects(changed, 1, 1);
		rfb::RectVector::const_iterator i;
		serverSharedmem->changed_pos=0;
		for ( i=changed.begin(); i != changed.end(); i++)
		{
			serverSharedmem->changed[serverSharedmem->changed_pos].left=(*i).tl.x;
			serverSharedmem->changed[serverSharedmem->changed_pos].top=(*i).tl.y;
			serverSharedmem->changed[serverSharedmem->changed_pos].right=(*i).br.x;
			serverSharedmem->changed[serverSharedmem->changed_pos].bottom=(*i).br.y;
			serverSharedmem->changed_pos++;
			if (serverSharedmem->changed_pos>1999) break;
		}

		serverSharedmem->copied_pos=0;
		for ( i=copied.begin(); i != copied.end(); i++)
		{
			serverSharedmem->copied[serverSharedmem->copied_pos].left=(*i).tl.x;
			serverSharedmem->copied[serverSharedmem->copied_pos].top=(*i).tl.y;
			serverSharedmem->copied[serverSharedmem->copied_pos].right=(*i).br.x;
			serverSharedmem->copied[serverSharedmem->copied_pos].bottom=(*i).br.y;
			serverSharedmem->copied_pos++;
			if (serverSharedmem->copied_pos>1999) break;
		}

		serverSharedmem->cached_pos=0;
		for ( i=cached.begin(); i != cached.end(); i++)
		{
			serverSharedmem->cached[serverSharedmem->cached_pos].left=(*i).tl.x;
			serverSharedmem->cached[serverSharedmem->cached_pos].top=(*i).tl.y;
			serverSharedmem->cached[serverSharedmem->cached_pos].right=(*i).br.x;
			serverSharedmem->cached[serverSharedmem->cached_pos].bottom=(*i).br.y;
			serverSharedmem->cached_pos++;
			if (serverSharedmem->cached_pos>1999) break;
		}

		serverSharedmem->point.x=copy_delta->x;
		serverSharedmem->point.y=copy_delta->y;
		CopyRegionsPoint_to_serverFn.Call_Fnction(NULL,NULL);
	};

	void NotifyClients_StateChange(int value, unsigned long state){ _NotifyClients_StateChange so;so.value=value;so.state=state;NotifyClients_StateChangeFn.Call_Fnction((char*)&so,NULL);};	
	void UpdateClipText(LPSTR text){ if (strlen(text)<64000){strcpy_s(serverSharedmem->cliptext,64000,text);UpdateClipTextFn.Call_Fnction(NULL,NULL);}};

	bool Driver(){return serverSharedmem->m_driver;};
	bool CaptureAlphaBlending(){return serverSharedmem->m_fCaptureAlphaBlending;};
	bool BlackAlphaBlending(){return serverSharedmem->m_fBlackAlphaBlending;};
	bool BlankMonitorEnabled(){return serverSharedmem->m_fBlankMonitorEnabled;};
	bool PollFullScreen(){return serverSharedmem->m_poll_fullscreen;};
	bool BlankInputsOnly(){return serverSharedmem->m_fBlankInputsOnly;};
	bool LocalInputsDisabled(){return serverSharedmem->m_disable_local_inputs;};
	bool PollOnEventOnly(){return serverSharedmem->m_poll_oneventonly;};
	bool PollConsoleOnly(){return serverSharedmem->m_poll_consoleonly;};
	bool TurboMode(){return serverSharedmem->m_TurboMode;};
	bool PollForeground(){return serverSharedmem->m_poll_foreground;};
	bool PollUnderCursor(){return serverSharedmem->m_poll_undercursor;};
	int MaxCpu(){return serverSharedmem->m_MaxCpu;};
	bool Hook(){return serverSharedmem->m_hook;};

	void Driver(bool value){serverSharedmem->m_driver=value;};
	void Hook(bool value){serverSharedmem->m_hook=value;};
	void PollFullScreen(bool value){serverSharedmem->m_poll_fullscreen=value;};
	void BlackAlphaBlending(bool value){serverSharedmem->m_fBlackAlphaBlending=value;};
	void DisableLocalInputs(bool value){serverSharedmem->m_disable_local_inputs=value;};
	char *GetWindowName(){return serverSharedmem->m_szWindowName;};



};