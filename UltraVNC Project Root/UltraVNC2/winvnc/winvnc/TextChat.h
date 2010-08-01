/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002 Ultr@VNC Team Members. All Rights Reserved.
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
// http://ultravnc.sourceforge.net/

#include "uvnc_session/uvnc_session/communication.h"
#ifndef TEXTCHAT_H__
#define TEXTCHAT_H__
#pragma once

#define BLACK	0x00000000
#define RED		0x000000FF
#define GREEN	0x0000FF00
#define BLUE	0x00FF0000
#define GREY	0x00888888

class vncClient;

class TextChat  
{
public:
	// Props
	vncClient			*m_pCC;
	bool				m_fTextChatRunning;
	char*				m_szLocalText;
	char*				m_szLastLocalText;
	char*				m_szRemoteText;	
	char*				m_szRemoteName;
	char*				m_szTextBoxBuffer;
    HANDLE              m_Thread;

	// Methods
	TextChat(vncClient *pCC);
	HWND DisplayTextChat();
	void KillDialog();
	void OrderTextChat();
	void RefuseTextChat();
	void ProcessTextChatMsg(int nTO);
   	virtual ~TextChat();
	static LRESULT CALLBACK DoDialogThread(LPVOID lpParameter);
	void SendTextChatRequest(int Msg);
	void SendLocalText(void);

	comm_serv sharedmem;

	//outgoing
	comm_serv ProcessTextChatMsg1IPCFn;
	comm_serv ProcessTextChatMsg2IPCFn;
	comm_serv KillDialogIPCFn_OUT;

	//incoming
	comm_serv KillDialogIPCFn_IN;
	comm_serv SendTextChatGuiRequestIPCFn;
	comm_serv SendLocalTextIPCFn;

	void ProcessTextChatMsg1IPC(int len);
	void ProcessTextChatMsg2IPC();
	void KillDialogIPC();
	void TextChatIPC(char *name, bool pluginenabled);
	bool Isinit;
};

#endif 



