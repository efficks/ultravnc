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
#include "communication.h"

#define BLACK	0x00000000
#define RED		0x000000FF
#define GREEN	0x0000FF00
#define BLUE	0x00FF0000
#define GREY	0x00888888

class TextChatGui  
{
public:
	HWND				m_hWnd;
	HWND				m_hDlg;
	bool				m_fTextChatGuiRunning;
	char*				m_szLocalText;
	char*				m_szLastLocalText;
	char*				m_szRemoteText;	
	char*				m_szRemoteName;
	char*				m_szLocalName;
	char*				m_szTextBoxBuffer;
    HANDLE              m_Thread;
	HANDLE 				m_ipc_Thread;
	bool				m_fTextChat_ipc_Running;
	char				peername[128];
	bool				DSMPluginPointerEnabled;

	TextChatGui			*todelete;

	// Methods
	TextChatGui(char *IN_peername,bool DSMPluginPointerEnabled,bool app);
	virtual ~TextChatGui();

	HWND DisplayTextChatGui();
	HWND TextChatGui_IPC_Thread();
	void KillDialog();
	void ProcessTextChatGuiMsg1(int len);
	void ProcessTextChatGuiMsg2();
	void Setpointer(TextChatGui *pointer);
   	
	static LRESULT CALLBACK DoDialogThread(LPVOID lpParameter);
	static LRESULT CALLBACK TextChatGui::IPC_Thread(LPVOID lpParameter);
	static BOOL CALLBACK TextChatGuiDlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void PrintMessage(const char* szMessage, const char* szSender, DWORD color = BLACK);
	void SetTextFormat(bool bBold = false, bool bItalic = false, long nSize = 0x75, const char* szFaceName = "MS Sans Serif", DWORD dwColor = BLACK);
	void ShowChatWindow(bool fVisible);

	void KillDialogIPC();
	void SendTextChatGuiRequestIPC(int Msg);
	void SendLocalTextIPC(void);

	comm_serv sharedmem;

	//outgoing
	comm_serv ProcessTextChatMsg1IPCFn;
	comm_serv ProcessTextChatMsg2IPCFn;
	comm_serv KillDialogIPCFn_OUT;

	//incoming
	comm_serv KillDialogIPCFn_IN;
	comm_serv SendTextChatGuiRequestIPCFn;
	comm_serv SendLocalTextIPCFn;

	HINSTANCE hRichEdit;
};