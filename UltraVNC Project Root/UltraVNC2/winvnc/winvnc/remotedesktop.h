/////////////////////////////////////////////////////////////////////////////
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

