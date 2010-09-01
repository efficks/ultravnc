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
#include "stdafx.h"
#include "vncTimedMsgBox.h"

// The message-box delay
const UINT TIMED_MSGBOX_DELAY = 1000;

class vncTimedMsgBoxThread
{
private:
	 static unsigned __stdcall threadProc(void* a_param)
		{
    		vncTimedMsgBoxThread* pthread = static_cast<vncTimedMsgBoxThread*>(a_param);
    		return pthread->run();
		};

public:
    vncTimedMsgBoxThread(const char *caption, const char *title, UINT type)
	{
		m_type = type;
		m_caption = _strdup(caption);
		m_title = _strdup(title);
		ThreadRunning=false;
		m_hThread=NULL;
	};
	virtual ~vncTimedMsgBoxThread()
	{
		if (GetHandle()) CloseHandle(GetHandle());
		if (m_caption != NULL)
			free(m_caption);
		if (m_title != NULL)
			free(m_title);
	};

	bool Start()
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
	};

	void Stop()
		{
			ThreadRunning=false;
		};

    const bool wait() const
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

    const HANDLE& GetHandle() const { return m_hThread; }

private:
    vncTimedMsgBoxThread(const vncTimedMsgBoxThread&);
    vncTimedMsgBoxThread& operator=(const vncTimedMsgBoxThread&);

    unsigned run()
	{
		if (m_caption == NULL)
			return 0;
		MessageBox(NULL, m_caption, m_title, m_type | MB_OK|MB_SETFOREGROUND|MB_SERVICE_NOTIFICATION);
		return 0;
	}

    HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;

	char *m_caption;
	char *m_title;
	UINT m_type;
};

// The main vncTimedMsgBox class

void
vncTimedMsgBox::Do(const char *caption, const char *title, UINT type)
{
	// Create the thread object
	vncTimedMsgBoxThread *thread = new vncTimedMsgBoxThread(caption, title, type);
	if (thread == NULL)
		return;

	// Start the thread object
	thread->Start();

	// And wait a few seconds
	Sleep(TIMED_MSGBOX_DELAY);
}