#include "stdafx.h"
#include "resource.h"
#include "communication.h"

class keymouse
{
public:
    keymouse();
	virtual ~keymouse();
	bool Start();
	void Stop();
    const bool wait() const;
    const HANDLE& GetHandle() const { return m_hThread; }

private:
    keymouse(const keymouse&);
    keymouse& operator=(const keymouse&);

    static unsigned __stdcall threadProc(void*);
    unsigned run();

    HANDLE m_hThread;
    unsigned m_tid;
	bool ThreadRunning;

	bool InputDesktopSelected();
	bool SelectDesktop();
	comm_serv mouse_eventFn;
	comm_serv sas_eventFn;
	comm_serv SendInputFn;
	comm_serv ClearShiftKeysFn;
	comm_serv keyEventFn;
	comm_serv ClearKeyStateFn;
};