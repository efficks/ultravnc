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

class vncListDlg;

#if (!defined(_WINVNC_VNCLISTDLG))
#define _WINVNC_VNCLISTDLG

#include "stdafx.h"
#include "communication.h"


class vncListDlg
{
public:
	// Constructor / destructor
	vncListDlg();
	~vncListDlg();

	BOOL Init();
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Display();
	BOOL m_dlgvisible;
private:
	comm_serv KillClient; 
	comm_serv TextChatClient; 
	comm_serv ListAuthClients; 
	comm_serv ListUnauthClients;
};

#endif
