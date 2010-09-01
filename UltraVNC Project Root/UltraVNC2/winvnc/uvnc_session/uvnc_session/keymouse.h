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