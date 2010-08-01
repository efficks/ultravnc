#include "stdafx.h"
#include "acceptdialog_thread.h"
#include "communication.h"

acceptdialog_thread::acceptdialog_thread()
{
	ThreadRunning=false;
	m_hThread=NULL;	
	if (!AcceptDialogFn.Init("AcceptDialog",sizeof(_acceptdialog),1,app,false)) goto error;
	return;
	error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);

}
acceptdialog_thread::~acceptdialog_thread() 
{
	if (GetHandle()) CloseHandle(GetHandle());
}

void acceptdialog_thread::Stop()
{
	ThreadRunning=false;
}


bool acceptdialog_thread::Start()
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

const bool acceptdialog_thread::wait() const
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

unsigned __stdcall acceptdialog_thread::threadProc(void* a_param)
{
    acceptdialog_thread* pthread = static_cast<acceptdialog_thread*>(a_param);
    return pthread->run();
}

unsigned acceptdialog_thread::run()
{
	HANDLE Events[1];
	Events[0]=AcceptDialogFn.GetEvent();

	while (ThreadRunning)
		{ 
				DWORD dwEvent = WaitForMultipleObjects(1,Events,FALSE,1000);
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ eventAccept %i\n",dwEvent-WAIT_OBJECT_0);
					SetLastError(0);
					if ( dwEvent-WAIT_OBJECT_0 != 258)OutputDebugString(szText);		
			#endif
				switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						_acceptdialog ad;
						unsigned char value;
						AcceptDialogFn.ReadData((char*)&ad);
						// Check done, run in inputdesktop
						value=vncAcceptDialogFn(ad.verified, ad.QueryIfNoLogon,ad.QueryAccept,ad.QueryTimeout,ad.PeerName);
						AcceptDialogFn.SetData((char*) &value);
						}
						break;
					case WAIT_TIMEOUT:
						break;
				}
		}			
	ThreadRunning=false;
    return 0;
}
