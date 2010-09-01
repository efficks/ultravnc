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

#include "remotedesktop.h"

extern comm_serv CheckBufferFn;
extern comm_serv LockFn;
extern comm_serv UnLock;
omni_mutex		m_update_lock;

remoteDesktop::remoteDesktop()
{
	InitializeCriticalSection(&CriticalSection1); 
	m_bufferpointer=NULL;
	m_bufferpointer=new vncBuffer;
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ new remoteDesktop %i\n",m_bufferpointer);
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
	newdesktopFN.Call_Fnction(NULL,NULL);
	m_bufferpointer->SetDesktop(this);
	ThreadRunning=false;
	m_hThread=NULL;
	Start();
}

remoteDesktop::~remoteDesktop()
{
	deletedesktopFn.Call_Fnction(NULL,NULL);
	if (GetHandle()) CloseHandle(GetHandle());
	Stop();
	wait();
	if (m_bufferpointer) delete m_bufferpointer;
	m_bufferpointer=NULL;
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ ~remoteDesktop\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
	DeleteCriticalSection(&CriticalSection1);
}

void remoteDesktop::Stop()
{
	ThreadRunning=false;
}


bool remoteDesktop::Start()
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

const bool remoteDesktop::wait() const
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



unsigned __stdcall remoteDesktop::threadProc(void* a_param)
{
    remoteDesktop* pthread = static_cast<remoteDesktop*>(a_param);
    return pthread->run();
}

bool ominilocked;

DWORD WINAPI threadLock(LPVOID lpParam)
{
	#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ LOCK remote \n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
	omni_mutex_lock l(m_update_lock);
	while (ominilocked)
	{
		Sleep(10);
	}
	#ifdef _DEBUG
					//char			szText[256];
					sprintf(szText," ++++++ UnLOCK remote \n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
	return 0;
}

unsigned remoteDesktop::run()
{
	#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ STartrun\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
	HANDLE Events[3];
	Events[0]=CheckBufferFn.GetEvent();
	Events[1]=LockFn.GetEvent();
	Events[2]=UnLock.GetEvent();
	// no input desktop required
	while (ThreadRunning)
		{ 
				DWORD dwEvent = WaitForMultipleObjects(3,Events,FALSE,100);
				switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						CheckBufferFn.ReadData(NULL);
						m_bufferpointer->CheckBuffer();
						CheckBufferFn.SetData(NULL);
						}
					break;
					case WAIT_OBJECT_0 + 1: 
						{
						ominilocked=true;
						HANDLE threadHandle;
						DWORD dwTId;
						threadHandle = CreateThread(NULL, 0, threadLock, NULL,NULL, &dwTId);
						CloseHandle(threadHandle);
						}
					break;
					case WAIT_OBJECT_0 + 2: 
						{
						ominilocked=false;
						}
					break;
				}
		}			
	ThreadRunning=false;
	#ifdef _DEBUG
//					char			szText[256];
					sprintf(szText," ++++++ stoprun\n");
					SetLastError(0);
					OutputDebugString(szText);		
			#endif
    return 0;
}



rfb::Rect
remoteDesktop::GetSize()
{
	if (!desktopsharedmem->m_videodriver)
		{
		desktopsharedmem->m_SWOffsetx=0;
		desktopsharedmem->m_SWOffsety=0;
		return rfb::Rect(0, 0, desktopsharedmem->m_scrinfo.framebufferWidth, desktopsharedmem->m_scrinfo.framebufferHeight);
		}
	 else
		{
			if (desktopsharedmem->multi_monitor)
				return rfb::Rect(0,0,desktopsharedmem->mymonitor[2].Width,desktopsharedmem->mymonitor[2].Height);	
			else
				return rfb::Rect(0,0,desktopsharedmem->mymonitor[0].Width,desktopsharedmem->mymonitor[0].Height);
		}
}

int
remoteDesktop::ScreenBuffSize()
{
	return desktopsharedmem->m_scrinfo.format.bitsPerPixel/8 *
		desktopsharedmem->m_scrinfo.framebufferWidth *
		desktopsharedmem->m_scrinfo.framebufferHeight;
}

rfb::Rect
remoteDesktop::MouseRect()
{
	m_cursorpos.br.x=desktopsharedmem->m_cursorpos_rect.right;
	m_cursorpos.br.y=desktopsharedmem->m_cursorpos_rect.bottom;
	m_cursorpos.tl.x=desktopsharedmem->m_cursorpos_rect.left;
	m_cursorpos.tl.y=desktopsharedmem->m_cursorpos_rect.top;
	return m_cursorpos;
}

bool remoteDesktop::GetBlockInputState()
{ 
	return desktopsharedmem->m_bIsInputDisabledByClient; 
}

void
remoteDesktop::FillDisplayInfo(rfbServerInitMsg *scrinfo)
{
	memcpy(scrinfo, &desktopsharedmem->m_scrinfo, sz_rfbServerInitMsg);
}