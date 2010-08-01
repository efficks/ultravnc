#include "stdafx.h"
#include "resource.h"
#include "communication.h"

class acceptdialog_thread
{
public:
    acceptdialog_thread();
	virtual ~acceptdialog_thread();
	bool Start();
	void Stop();
    const bool wait() const;
    const HANDLE& GetHandle() const { return m_hThread; }

private:
    acceptdialog_thread(const acceptdialog_thread&);
    acceptdialog_thread& operator=(const acceptdialog_thread&);

    static unsigned __stdcall threadProc(void*);
    unsigned run();

    HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;

	comm_serv AcceptDialogFn;
};