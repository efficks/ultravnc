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

#include "stdhdrs.h"
#include "resource.h"
#include "vncclient.h"
#include "vncserver.h"
#include "TextChat.h"
#include "commctrl.h"
#include "richedit.h"
#include "common/win32_helpers.h"

#include "localization.h" // Act : add localization on messages

#define TEXTMAXSIZE 16384
#define MAXNAMESIZE	128 // MAX_COMPUTERNAME_LENGTH+1 (32)
#define CHAT_OPEN  -1  // Todo; put these codes in rfbproto.h
#define CHAT_CLOSE -2
#define CHAT_FINISHED -3

//	[v1.0.2-jp1 fix]
LRESULT CALLBACK SBProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LONG pDefSBProc;
extern comm_serv TextChatIPCFn;
extern BOOL	fRunningFromExternalService;
///////////////////////////////////////////////////////
TextChat::TextChat(vncClient *pCC)
{
	m_pCC = pCC;


	m_fTextChatRunning = false;
	m_szRemoteName = new char[MAXNAMESIZE]; // Name of remote machine
	memset(m_szRemoteName,0,MAXNAMESIZE);
	 m_Thread = INVALID_HANDLE_VALUE;
	 Isinit=false;
}

void TextChat::TextChatIPC(char *name, bool pluginenabled)
{
	_textchatstart tcs;
	strcpy_s(tcs.name,128,name);
	tcs.pluginenabled=pluginenabled;
	TextChatIPCFn.Call_Fnction((char*)&tcs,NULL);
}

void TextChat::ProcessTextChatMsg1IPC(int len)
{
	ProcessTextChatMsg1IPCFn.Call_Fnction((char*)&len,NULL);
}
void TextChat::ProcessTextChatMsg2IPC()
{
	ProcessTextChatMsg2IPCFn.Call_Fnction(NULL,NULL);
}
void TextChat::KillDialogIPC()
{
	if (Isinit) KillDialogIPCFn_OUT.Call_Fnction(NULL,NULL);
}




//
//
//
TextChat::~TextChat()
{
	if (m_szRemoteName != NULL) delete [] m_szRemoteName;
	m_fTextChatRunning = false;
}

//
//
//
void TextChat::ProcessTextChatMsg(int nTO)
{
	rfbTextChatMsg tcm;
	m_pCC->m_socket->ReadExact(((char *) &tcm) + nTO, sz_rfbTextChatMsg - nTO);
	int len = Swap32IfLE(tcm.length);
	
	if (len == CHAT_OPEN)
	{
		if (!m_fTextChatRunning) DisplayTextChat();
		else return;
	}
	else if (len == CHAT_CLOSE)
	{
		if (!m_fTextChatRunning) return;
	}
	else if (len == CHAT_FINISHED)
	{
		if (!m_fTextChatRunning) return;
	}

	if ((len == CHAT_OPEN) || (len == CHAT_CLOSE) || (len == CHAT_FINISHED) || len == 0 )
	{
		ProcessTextChatMsg1IPC(len);
		return;
	}
	if (len > TEXTMAXSIZE) return;
	
	memset(m_szRemoteText, 0, TEXTMAXSIZE);
	m_pCC->m_socket->ReadExact(m_szRemoteText, len);
	ProcessTextChatMsg2IPC();
}


//
// Tell the client to initiate a TextChat
//
void TextChat::OrderTextChat()
{
	SendTextChatRequest(CHAT_OPEN);
	return;
}


//
//
//
void TextChat::SendTextChatRequest(int nMsg)
{
	if (m_pCC->m_fFileTransferRunning) return; // Don't break a running file transfer please...
    rfbTextChatMsg tcm;
    tcm.type = rfbTextChat;
	tcm.length = Swap32IfLE(nMsg);
    m_pCC->m_socket->SendExact((char *)&tcm, sz_rfbTextChatMsg, rfbTextChat);
	return;
}


//
// Send local text content
//
void TextChat::SendLocalText(void)
{
	if (m_pCC->m_fFileTransferRunning) return; // Don't break a running file transfer please...

    rfbTextChatMsg tcm;
    tcm.type = rfbTextChat;
	tcm.length = Swap32IfLE(strlen(m_szLocalText));
    m_pCC->m_socket->SendExact((char *)&tcm, sz_rfbTextChatMsg, rfbTextChat);
	m_pCC->m_socket->SendExact((char *)m_szLocalText, strlen(m_szLocalText));

	return;
}



LRESULT CALLBACK TextChat::DoDialogThread(LPVOID lpParameter)
{
	TextChat* _this = (TextChat*)lpParameter;
	_this->m_fTextChatRunning = true;

	HANDLE event0[3];
	event0[0]=_this->KillDialogIPCFn_IN.GetEvent();
	event0[1]=_this->SendTextChatGuiRequestIPCFn.GetEvent();
	event0[2]=_this->SendLocalTextIPCFn.GetEvent();
	while (_this->m_fTextChatRunning)
	{
		DWORD dwEvent=WaitForMultipleObjects(3,event0,false,1000);
		switch(dwEvent)
				{
					case WAIT_OBJECT_0 + 0: 
						{
						_this->KillDialogIPCFn_IN.ReadData(NULL);
						_this->m_fTextChatRunning = false;
						_this->KillDialogIPCFn_IN.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 1: 
						{
							int len;
						_this->SendTextChatGuiRequestIPCFn.ReadData((char*)&len);
						_this->SendTextChatRequest(len);
						_this->SendTextChatGuiRequestIPCFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 2: 
						{
						_this->SendLocalTextIPCFn.ReadData(NULL);
						_this->SendLocalText();
						_this->SendLocalTextIPCFn.SetData(NULL);
						}
						break;
		}
	}
	return 0;
}

HWND TextChat::DisplayTextChat()
{
	Isinit=true;
	char name[128];
	if (_snprintf(m_szRemoteName,MAXNAMESIZE-1,"%s", m_pCC->GetClientName()) < 0 )
			{
				m_szRemoteName[MAXNAMESIZE-4]='.';
				m_szRemoteName[MAXNAMESIZE-3]='.';
				m_szRemoteName[MAXNAMESIZE-2]='.';
				m_szRemoteName[MAXNAMESIZE-1]=0x00;
			}
	//shared memory
	//there can be multiple dialogs, so we need to use the remote name to differentiate
	// hell for ipc
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"sharedmem");	
	sharedmem.Init(name,sizeof(_textchatdata),0,!fRunningFromExternalService,true);
	//outgoing
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"ProcessTextChatMsg1IPC");	
	ProcessTextChatMsg1IPCFn.Init(name,sizeof(int),0,!fRunningFromExternalService,true);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"ProcessTextChatMsg2IPC");	
	ProcessTextChatMsg2IPCFn.Init(name,0,0,!fRunningFromExternalService,true);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"KillDialogIPC_OUT");	
	KillDialogIPCFn_OUT.Init(name,0,0,!fRunningFromExternalService,true);
	
	//incoming
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"KillDialogIPC_IN");
	KillDialogIPCFn_IN.Init(name,0,0,!fRunningFromExternalService,true);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"SendTextChatGuiRequestIPC");
	SendTextChatGuiRequestIPCFn.Init(name,sizeof(int),0,!fRunningFromExternalService,true);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"SendLocalTextIPC");
	SendLocalTextIPCFn.Init(name,0,0,!fRunningFromExternalService,true);

	_textchatdata *smem=(_textchatdata*)sharedmem.Getsharedmem();

	m_szLocalText = smem->m_szLastLocalText; // Text (message) entered by local user
	memset(m_szLocalText, 0, TEXTMAXSIZE);
	m_szLastLocalText = smem->m_szLastLocalText; // Last local text (no more necessary)
	memset(m_szLastLocalText, 0, TEXTMAXSIZE);
	m_szRemoteText = smem->m_szRemoteText; // Incoming remote text (remote message)
	memset(m_szRemoteText, 0, TEXTMAXSIZE);	
	m_szTextBoxBuffer = smem->m_szTextBoxBuffer; // History Text (containing all chat messages from everybody)
	memset(m_szTextBoxBuffer,0,TEXTMAXSIZE);

   
	bool pluginenabled=m_pCC->m_server->GetDSMPluginPointer()->IsEnabled();
	TextChatIPC(m_szRemoteName,pluginenabled);

	DWORD threadID;
	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(TextChat::DoDialogThread),(LPVOID)this, 0, &threadID);
	if (m_Thread) ResumeThread(m_Thread);
	return (HWND)0;

}


void TextChat::KillDialog()
{
	m_fTextChatRunning = false;
    if (m_Thread != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(m_Thread, INFINITE); 
    }
	KillDialogIPC();
}

void TextChat::RefuseTextChat()
{
	SendTextChatRequest(CHAT_CLOSE);
	SendTextChatRequest(CHAT_FINISHED);
}

