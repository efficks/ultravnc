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
#include "keymouse.h"
#include "communication.h"
#include "vncKeymap.h"

keymouse::keymouse()
{
	ThreadRunning=false;
	m_hThread=NULL;
	if (!mouse_eventFn.Init("mouse_event",sizeof(mouseventdata),0,app,false)) goto error;
	if (!sas_eventFn.Init("sas_event",0,0,app,false)) goto error;
	if (!SendInputFn.Init("SendInput",sizeof(INPUT),0,app,false)) goto error;
	if (!ClearShiftKeysFn.Init("ClearShiftKeys",0,0,app,false)) goto error;
	if (!keyEventFn.Init("keyEvent",sizeof(keyEventdata),0,app,false)) goto error;
	if (!ClearKeyStateFn.Init("ClearKeyState",sizeof(BYTE),0,app,false)) goto error;
	return;
	error: //shared memory failed, service part isn't running
	MessageBox(NULL,"service part isn't running, no shared mem exist","",0);
}
keymouse::~keymouse() 
{
	if (GetHandle()) CloseHandle(GetHandle());
}

void keymouse::Stop()
{
	ThreadRunning=false;
}


bool keymouse::Start()
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

const bool keymouse::wait() const
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

unsigned __stdcall keymouse::threadProc(void* a_param)
{
    keymouse* pthread = static_cast<keymouse*>(a_param);
    return pthread->run();
}

unsigned keymouse::run()
{
	HANDLE Events[5];
	Events[0]=mouse_eventFn.GetEvent();
	Events[1]=SendInputFn.GetEvent();
	Events[2]=ClearShiftKeysFn.GetEvent();
	Events[3]=keyEventFn.GetEvent();
	Events[4]=ClearKeyStateFn.GetEvent();

	 InputDesktopSelected();
	 //check done input desktop auto selected
	while (ThreadRunning)
		{ 
				DWORD dwEvent = WaitForMultipleObjects(5,Events,FALSE,1000);
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ eventInput %i\n",dwEvent-WAIT_OBJECT_0);
					SetLastError(0);
					if ( dwEvent-WAIT_OBJECT_0 != 258) OutputDebugString(szText);		
			#endif
				switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						mouseventdata med;
						mouse_eventFn.ReadData((char*)&med);
						mouse_event(med.dwFlags,med.dx,med.dy,med.dwData,0);
						mouse_eventFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 1:
						{
						INPUT inp;
						SendInputFn.ReadData((char*)&inp);
						SendInput(1, &inp, sizeof(inp));
						SendInputFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 2:
						{
						ClearShiftKeysFn.ReadData(NULL);
						vncKeymap::ClearShiftKeys();
						ClearShiftKeysFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 3:
						{
						keyEventdata ked;
						keyEventFn.ReadData((char*)&ked);
						vncKeymap::keyEvent(ked.keysym, ked.down,ked.jap,ked.version,ked.from_service,&sas_eventFn);
						keyEventFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 4:
						BYTE b;
						ClearKeyStateFn.ReadData((char*)&b);
						ClearKeyState(b);
						ClearKeyStateFn.SetData(NULL);
						break;
					case WAIT_TIMEOUT:
						InputDesktopSelected();
						break;
				}
		}			
	ThreadRunning=false;
    return 0;
}

bool keymouse::SelectDesktop()
{
		HDESK desktop;
		HDESK old_desktop;
		DWORD dummy;
		char new_name[256];
		desktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		if (desktop == NULL) return FALSE;
		old_desktop = GetThreadDesktop(GetCurrentThreadId());
		if (!GetUserObjectInformation(desktop, UOI_NAME, &new_name, 256, &dummy)) {
			CloseDesktop(desktop);
			return FALSE;
		}
		if(!SetThreadDesktop(desktop)) {
			CloseDesktop(desktop);
			return FALSE;
		}
		CloseDesktop(old_desktop);			
		return TRUE;

}

bool keymouse::InputDesktopSelected()
{

	DWORD dummy;
	char threadname[256];
	char inputname[256];
	HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
	HDESK inputdesktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);

	if (inputdesktop == NULL) return FALSE;
	if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) {
			CloseDesktop(inputdesktop);
			return FALSE;
		}
	if (!GetUserObjectInformation(inputdesktop, UOI_NAME, &inputname, 256, &dummy)) {
			CloseDesktop(inputdesktop);
			return FALSE;
		}
	CloseDesktop(inputdesktop);
	if (strcmp(threadname, inputname) != 0)
		{
			//if (strcmp(inputname, "Screen-saver") == 0)
			{
				return SelectDesktop();
			}
			return FALSE;
		}
	return TRUE;
}
