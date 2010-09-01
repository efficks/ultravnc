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
#include <Richedit.h>
#include <Mmsystem.h>
#include "textchatgui.h"
#include "win32_helpers.h"

#define TEXTMAXSIZE 16384
#define MAXNAMESIZE	128

#define CHAT_OPEN  -1  // Todo; put these codes in rfbproto.h
#define CHAT_CLOSE -2
#define CHAT_FINISHED -3

LRESULT CALLBACK SBProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LONG pDefSBProc;
extern TextChatGui *temppointer;
extern HANDLE tempevent;


void AdjustLeft(LPRECT lprc)
{
	int cx = lprc->right - lprc->left - GetSystemMetrics(SM_CXSIZEFRAME) * 2;

	if(cx < 240){
		lprc->left = lprc->right - 240 - GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	}
}

void AdjustTop(LPRECT lprc)
{
	int cy = lprc->bottom - lprc->top - GetSystemMetrics(SM_CYSIZEFRAME) * 2;

	if(cy < 179){
		lprc->top = lprc->bottom - 179 - GetSystemMetrics(SM_CYSIZEFRAME) * 2;
	}
}

void AdjustRight(LPRECT lprc)
{
	int cx = lprc->right - lprc->left - GetSystemMetrics(SM_CXSIZEFRAME) * 2;

	if(cx < 240){
		lprc->right = lprc->left + 240 + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	}
}

void AdjustBottom(LPRECT lprc)
{
	int cy = lprc->bottom - lprc->top - GetSystemMetrics(SM_CYSIZEFRAME) * 2;

	if(cy < 179){
		lprc->bottom = lprc->top + 179 + GetSystemMetrics(SM_CYSIZEFRAME) * 2;
	}
}

BOOL PlayResource()
{
	char szWavFile[MAX_PATH]; //PGM 
	if (GetModuleFileName(NULL, szWavFile, MAX_PATH)) //PGM 
	{ // PGM 
		char* p = strrchr(szWavFile, '\\'); //PGM 
		*p = '\0'; //PGM 
		strcat_s(szWavFile,260,"\\"); //PGM 
	} //PGM 
	strcat_s(szWavFile,260,"ding_dong.wav"); //PGM 
	if(::PlaySound(szWavFile, NULL, SND_APPLICATION | SND_FILENAME | SND_ASYNC | SND_NOWAIT)!= ERROR_SUCCESS) //PGM
		return FALSE; //PGM 
	else //PGM 
		return TRUE; //PGM 

}


TextChatGui::TextChatGui(char *IN_peername,bool IN_DSMPluginPointerEnabled,bool app)
{
	strcpy_s(peername,128,IN_peername);
	DSMPluginPointerEnabled=IN_DSMPluginPointerEnabled;
	m_szRemoteName = new char[MAXNAMESIZE]; // Name of remote machine
	memset(m_szRemoteName,0,MAXNAMESIZE);
	strcpy_s(m_szRemoteName,128,peername);
	m_hDlg=NULL;
	m_fTextChatGuiRunning = false;

	char name[128];
	//shared memory
	//there can be multiple dialogs, so we need to use the remote name to differentiate
	// hell for ipc
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"sharedmem");	
	sharedmem.Init(name,sizeof(_textchatdata),0,app,false);
	//incoming
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"ProcessTextChatMsg1IPC");	
	ProcessTextChatMsg1IPCFn.Init(name,sizeof(int),0,app,false);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"ProcessTextChatMsg2IPC");	
	ProcessTextChatMsg2IPCFn.Init(name,0,0,app,false);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"KillDialogIPC_OUT");	
	KillDialogIPCFn_OUT.Init(name,0,0,app,false);
	
	//outgoing
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"KillDialogIPC_IN");
	KillDialogIPCFn_IN.Init(name,0,0,app,false);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"SendTextChatGuiRequestIPC");
	SendTextChatGuiRequestIPCFn.Init(name,sizeof(int),0,app,false);
	strcpy_s(name,128,"");
	strncat_s(name,128,m_szRemoteName,15);
	strcat_s(name,128,"SendLocalTextIPC");
	SendLocalTextIPCFn.Init(name,0,0,app,false);
	
	_textchatdata *smem=(_textchatdata*)sharedmem.Getsharedmem();

	m_szLocalText = smem->m_szLastLocalText; // Text (message) entered by local user
	memset(m_szLocalText, 0, TEXTMAXSIZE);
	m_szLastLocalText = smem->m_szLastLocalText; // Last local text (no more necessary)
	memset(m_szLastLocalText, 0, TEXTMAXSIZE);
	m_szRemoteText = smem->m_szRemoteText; // Incoming remote text (remote message)
	memset(m_szRemoteText, 0, TEXTMAXSIZE);	
	m_szTextBoxBuffer = smem->m_szTextBoxBuffer; // History Text (containing all chat messages from everybody)
	memset(m_szTextBoxBuffer,0,TEXTMAXSIZE);

	
	m_szLocalName =  new char[MAXNAMESIZE]; // Name of local machine
	memset(m_szLocalName,0,MAXNAMESIZE);

	unsigned long pcSize=MAXNAMESIZE;
	if (GetComputerName(m_szLocalName, &pcSize))
	{
		if( pcSize >= MAXNAMESIZE)
		{
			m_szLocalName[MAXNAMESIZE-4]='.';
			m_szLocalName[MAXNAMESIZE-3]='.';
			m_szLocalName[MAXNAMESIZE-2]='.';
			m_szLocalName[MAXNAMESIZE-1]=0x00;
		}
	}
    m_Thread = INVALID_HANDLE_VALUE;
	m_ipc_Thread = INVALID_HANDLE_VALUE;
	TextChatGui_IPC_Thread();
	hRichEdit = LoadLibrary("RichEd32.dll");

}

TextChatGui::~TextChatGui()
{
	if (m_szRemoteName != NULL) delete [] m_szRemoteName;
	if (m_szLocalName != NULL) delete [] m_szLocalName;
	m_fTextChatGuiRunning = false;
	if (m_hDlg!=NULL) SendMessage(m_hDlg, WM_COMMAND, IDCANCEL, 0L);
	FreeLibrary(hRichEdit);

}

void TextChatGui::Setpointer(TextChatGui *pointer)
{
	todelete=pointer;
}

void TextChatGui::SendTextChatGuiRequestIPC(int Msg)
{
	SendTextChatGuiRequestIPCFn.Call_Fnction((char*)&Msg,NULL);
}

void TextChatGui::SendLocalTextIPC(void)
{
	SendLocalTextIPCFn.Call_Fnction(NULL,NULL);
}

void TextChatGui::KillDialogIPC()
{
	KillDialogIPCFn_IN.Call_Fnction(NULL,NULL);
}

void TextChatGui::ShowChatWindow(bool fVisible)
{
	ShowWindow(m_hDlg, fVisible ? SW_RESTORE : SW_MINIMIZE);
	SetForegroundWindow(m_hDlg);
}

//
// Set text format to a selection in the Chat area
//
void TextChatGui::SetTextFormat(bool bBold , bool bItalic , long nSize , const char* szFaceName , DWORD dwColor )
{
	if ( GetDlgItem( m_hDlg, IDC_CHATAREA_EDIT ) )  //  Sanity Check
	{		
		CHARFORMAT cf;
		memset( &cf, 0, sizeof(CHARFORMAT) ); //  Initialize structure

		cf.cbSize = sizeof(CHARFORMAT);             
		cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
		if (bBold) {
			cf.dwMask |= CFM_BOLD;
			cf.dwEffects |= CFE_BOLD;
		}
		if (bItalic) {
			cf.dwMask |= CFM_ITALIC;  
			cf.dwEffects |= CFE_ITALIC;
		}
		cf.crTextColor = dwColor;					// set color in AABBGGRR mode (alpha-RGB)
		cf.yHeight = nSize;							// set size in points
		strcpy_s( cf.szFaceName, 32, szFaceName);
													
		SendDlgItemMessage( m_hDlg, IDC_CHATAREA_EDIT, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf );
	}
}

//
// Output messages in the chat area 
//
//
void TextChatGui::PrintMessage(const char* szMessage,const char* szSender,DWORD dwColor /*= BLACK*/)
{
	// char szTextBoxBuffer[TEXTMAXSIZE];			
	// memset(szTextBoxBuffer,0,TEXTMAXSIZE);			
	CHARRANGE cr;	
	
	// sf@2005 - Empty the RichEdit control when it's close to the TEXTMAXSIZE limit
	// In worst case, the RichEdit can contains 2*TEXTMAXSIZE + NAMEMAXSIZE + 4 - 32 bytes
	GETTEXTLENGTHEX lpG;
	lpG.flags = GTL_NUMBYTES;
	lpG.codepage = CP_ACP; // ANSI

	int nLen = SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT, EM_GETTEXTLENGTHEX, (WPARAM)&lpG, 0L);
	if (nLen + 32 > TEXTMAXSIZE )
	{
		memset(m_szTextBoxBuffer, 0, TEXTMAXSIZE);
		strcpy_s(m_szTextBoxBuffer,TEXTMAXSIZE, "------------------------------------------------------------------------------------------------------------------------\n");
		SetDlgItemText(m_hDlg, IDC_CHATAREA_EDIT, m_szTextBoxBuffer);
	}

	// Todo: test if chat text + sender + message > TEXTMAXSIZE -> Modulo display
	if (szSender != NULL && strlen(szSender) > 0) //print the sender's name
	{
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,WM_GETTEXT, TEXTMAXSIZE-1,(LONG)m_szTextBoxBuffer);
		cr.cpMax = nLen; //strlen(m_szTextBoxBuffer);	 // Select the last caracter to make the text insertion
		cr.cpMin  = cr.cpMax;
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,EM_EXSETSEL,0,(LONG) &cr);

		//	[v1.0.2-jp1 fix]
		//SetTextFormat(false,false,0x75,"MS Sans Serif",dwColor);
		if(!hInst){
			SetTextFormat(false,false,0x75,"MS Sans Serif",dwColor);
		}
		else{
			SetTextFormat(false, false, 0xb4, "‚l‚r ‚oƒSƒVƒbƒN", dwColor);
		}

		_snprintf_s(m_szTextBoxBuffer,TEXTMAXSIZE, MAXNAMESIZE-1 + 4, "<%s>: ", szSender);		
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,EM_REPLACESEL,FALSE,(LONG)m_szTextBoxBuffer); // Replace the selection with the message
	}

	nLen = SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT, EM_GETTEXTLENGTHEX, (WPARAM)&lpG, 0L);
	if (szMessage != NULL) //print the sender's message
	{	
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,WM_GETTEXT, TEXTMAXSIZE-1,(LONG)m_szTextBoxBuffer);
		cr.cpMax = nLen; //strlen(m_szTextBoxBuffer);	 // Select the last caracter to make the text insertion
		cr.cpMin  = cr.cpMax;
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,EM_EXSETSEL,0,(LONG) &cr);

		//	[v1.0.2-jp1 fix]
		//SetTextFormat(false, false, 0x75, "MS Sans Serif", dwColor != GREY ? BLACK : GREY);	
		if(!hInst){
			SetTextFormat(false, false, 0x75, "MS Sans Serif", dwColor != GREY ? BLACK : GREY);
		}
		else{
			SetTextFormat(false, false, 0xb4, "‚l‚r ‚oƒSƒVƒbƒN", dwColor != GREY ? BLACK : GREY);
		}

		_snprintf_s(m_szTextBoxBuffer,TEXTMAXSIZE, TEXTMAXSIZE-1, "%s", szMessage);		
		SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT,EM_REPLACESEL,FALSE,(LONG)m_szTextBoxBuffer); 
	}

	// Scroll down the chat window
	// The following seems necessary under W9x & Me if we want the Edit to scroll to bottom...
	SCROLLINFO si;
    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE|SIF_PAGE;
    GetScrollInfo(GetDlgItem(m_hDlg, IDC_CHATAREA_EDIT), SB_VERT, &si);
	si.nPos = si.nMax - max(si.nPage - 1, 0);
	SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, si.nPos), 0L);	// Scroll down the ch

	// This line does the bottom scrolling correctly under NT4,W2K, XP...
	// SendDlgItemMessage(m_hDlg, IDC_CHATAREA_EDIT, WM_VSCROLL, SB_BOTTOM, 0L);

}


LRESULT CALLBACK TextChatGui::DoDialogThread(LPVOID lpParameter)
{
	TextChatGui* _this = (TextChatGui*)lpParameter;

	_this->m_fTextChatGuiRunning = true;
	// TODO: Place code here.
	HDESK desktop;
	desktop = OpenInputDesktop(0, FALSE,
								DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
								DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
								DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
								DESKTOP_SWITCHDESKTOP | GENERIC_WRITE
								);

	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
	DWORD dummy;

	char new_name[256];
	if (desktop)
	{
		GetUserObjectInformation(desktop, UOI_NAME, &new_name, 256, &dummy);
		SetThreadDesktop(desktop);
	}

	 //	[v1.0.2-jp1 fix]
 	 DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TEXTCHAT_DLG), 
	 						NULL, (DLGPROC) TextChatGuiDlgProc, (LONG) _this);
	 SetThreadDesktop(old_desktop);
	 if (desktop) CloseDesktop(desktop);
	 _this->KillDialogIPC();
	 _this->m_fTextChat_ipc_Running=false;
	 return 0;
}

HWND TextChatGui::DisplayTextChatGui()
{
	DWORD threadID;
	m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(TextChatGui::DoDialogThread),(LPVOID)this, 0, &threadID);
	if (m_Thread) ResumeThread(m_Thread);
	return (HWND)0;

}


LRESULT CALLBACK TextChatGui::IPC_Thread(LPVOID lpParameter)
{
	TextChatGui* _this = (TextChatGui*)lpParameter;

	_this->m_fTextChat_ipc_Running = true;
	// TODO: Place code here.
	HDESK desktop;
	desktop = OpenInputDesktop(0, FALSE,
								DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
								DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
								DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
								DESKTOP_SWITCHDESKTOP | GENERIC_WRITE
								);

	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
	DWORD dummy;

	char new_name[256];
	if (desktop)
	{
		GetUserObjectInformation(desktop, UOI_NAME, &new_name, 256, &dummy);
		SetThreadDesktop(desktop);
	}

	HANDLE event_ipc[3];
	event_ipc[0]=_this->ProcessTextChatMsg1IPCFn.GetEvent();
	event_ipc[1]=_this->ProcessTextChatMsg2IPCFn.GetEvent();
	event_ipc[2]=_this->KillDialogIPCFn_OUT.GetEvent();
	while (_this->m_fTextChat_ipc_Running)
	{
		 //check done input desktop auto selected
		DWORD dwEvent=WaitForMultipleObjects(3,event_ipc,false,1000);
#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ eventchat %i\n",dwEvent-WAIT_OBJECT_0);
					SetLastError(0);
					if ( dwEvent-WAIT_OBJECT_0 != 258)OutputDebugString(szText);		
#endif
		switch(dwEvent)
		{
			case WAIT_OBJECT_0 + 0: 
						{
						int len;
						_this->ProcessTextChatMsg1IPCFn.ReadData((char*)&len);
						_this->ProcessTextChatGuiMsg1(len);
						_this->ProcessTextChatMsg1IPCFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 1: 
						{
						_this->ProcessTextChatMsg2IPCFn.ReadData(NULL);
						_this->ProcessTextChatGuiMsg2();
						_this->ProcessTextChatMsg2IPCFn.SetData(NULL);
						}
						break;
					case WAIT_OBJECT_0 + 2: 
						{
						_this->KillDialogIPCFn_OUT.ReadData(NULL);
						_this->KillDialog();
						_this->m_fTextChat_ipc_Running=false;
						_this->KillDialogIPCFn_OUT.SetData(NULL);
						}
		}

	}


	 SetThreadDesktop(old_desktop);
	 if (desktop) CloseDesktop(desktop);
	 //we need to inform uvnc_session that this "new textchatgui" need to be deleted
	 temppointer=_this->todelete;
	 SetEvent(tempevent);
	 return 0;
}

HWND TextChatGui::TextChatGui_IPC_Thread()
{
	DWORD threadID;
	m_ipc_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(TextChatGui::IPC_Thread),(LPVOID)this, 0, &threadID);
	if (m_ipc_Thread) ResumeThread(m_ipc_Thread);
	return (HWND)0;

}

void TextChatGui::ProcessTextChatGuiMsg1(int len)
{	
	if (len == CHAT_OPEN)
	{
		PlayResource();
		if (m_fTextChatGuiRunning) return;
		DisplayTextChatGui();
		return;
	}
	if (len == CHAT_CLOSE)
	{
		// Close TextChat Dialog
		if (!m_fTextChatGuiRunning) return;
		PostMessage(m_hDlg, WM_COMMAND, IDOK, 0);
		return;
	}
	else if (len == CHAT_FINISHED)
	{
		// Close TextChat Dialog
		if (!m_fTextChatGuiRunning) return;
		// m_fTextChatRunning = false;
		// PostMessage(m_hDlg, WM_COMMAND, IDCANCEL, 0);
		return;
	}
	else
	{
		// Read the incoming text
		if (len > TEXTMAXSIZE) return; // Todo: Improve this...
		if (len == 0)
		{
			SetDlgItemText(m_hDlg, IDC_CHATAREA_EDIT, "");
			memset(m_szRemoteText, 0, TEXTMAXSIZE);
		}
	}
}
void TextChatGui::ProcessTextChatGuiMsg2()
{
	ShowChatWindow(true); // Show the chat window on new message reception
	PrintMessage(m_szRemoteText, m_szRemoteName, RED);
}


void TextChatGui::KillDialog()
{
	m_fTextChatGuiRunning = false;
	m_fTextChat_ipc_Running = false;
    if (m_Thread != INVALID_HANDLE_VALUE)
    {
	    SendMessage(m_hDlg, WM_COMMAND, IDCANCEL, 0);
        ::WaitForSingleObject(m_Thread, INFINITE); // wait for thread to exit
        m_hDlg=NULL;
    }
	if (m_ipc_Thread != INVALID_HANDLE_VALUE)
    {
		WaitForSingleObject(m_ipc_Thread, INFINITE);
	}

}

BOOL CALLBACK TextChatGui::TextChatGuiDlgProc(  HWND hWnd,  UINT uMsg,  WPARAM wParam, LPARAM lParam ) {

    TextChatGui *_thiss = helper::SafeGetWindowUserData<TextChatGui>(hWnd);

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
            helper::SafeSetWindowUserData(hWnd, lParam);
            TextChatGui *_this = (TextChatGui *) lParam;

			if (_this->m_szLocalText == NULL || _this->m_szRemoteText == NULL)
				EndDialog(hWnd, FALSE);

			_this->m_hWnd = hWnd;
			_this->m_hDlg = hWnd;

			const long lTitleBufSize = 256;			
			char szTitle[lTitleBufSize];
			
			_snprintf_s(szTitle,256,lTitleBufSize-1, " Chat with <%s> - Ultr@VNC",_this->m_szRemoteName);
			SetWindowText(hWnd, szTitle);			

			memset(_this->m_szLocalText, 0, TEXTMAXSIZE);

			// Local message box	
			SetDlgItemText(hWnd, IDC_INPUTAREA_EDIT, _this->m_szLocalText); 
			
			//  Chat area			
			_this->SetTextFormat(); //  Set character formatting and background color
			SendDlgItemMessage( hWnd, IDC_CHATAREA_EDIT, EM_SETBKGNDCOLOR, FALSE, 0xFFFFFF ); 

			
			memset(_this->m_szLastLocalText, 0, TEXTMAXSIZE); // Future retype functionnality
			memset(_this->m_szTextBoxBuffer, 0, TEXTMAXSIZE); // Clear Chat area 
			//  Load and display diclaimer message
			// sf@2005 - Only if the DSMplugin is used
			if (!_this->DSMPluginPointerEnabled)
			{
				//	[v1.0.2-jp1 fix]
				//if (LoadString(hAppInstance,IDS_WARNING,_this->m_szRemoteText, TEXTMAXSIZE -1) )
				//	_this->PrintMessage(_this->m_szRemoteText, NULL ,GREY);
				if (LoadString(hInst,IDS_WARNING,_this->m_szRemoteText, TEXTMAXSIZE -1) )
					_this->PrintMessage(_this->m_szRemoteText, NULL ,GREY);
			}
			
			
			// Scroll down the chat window
			// The following seems necessary under W9x & Me if we want the Edit to scroll to bottom...
			SCROLLINFO si;
			ZeroMemory(&si, sizeof(SCROLLINFO));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE|SIF_PAGE;
			GetScrollInfo(GetDlgItem(hWnd, IDC_CHATAREA_EDIT), SB_VERT, &si);
			si.nPos = si.nMax - max(si.nPage - 1, 0);
			SendDlgItemMessage(hWnd, IDC_CHATAREA_EDIT, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, si.nPos), 0L);			
			SetForegroundWindow(hWnd);

			//	[v1.0.2-jp1 fix] SUBCLASS Split bar
            pDefSBProc = helper::SafeGetWindowProc(GetDlgItem(hWnd, IDC_STATIC_SPLIT));
            helper::SafeSetWindowProc(GetDlgItem(hWnd, IDC_STATIC_SPLIT), (LONG)SBProc);

            return TRUE;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// Client order to close TextChat 			

			//	[v1.0.2-jp1 fix] UNSUBCLASS Split bar
            helper::SafeSetWindowProc(GetDlgItem(hWnd, IDC_STATIC_SPLIT), pDefSBProc);
			EndDialog(hWnd, FALSE);
			_thiss->m_fTextChatGuiRunning = false;
			_thiss->SendTextChatGuiRequestIPC(CHAT_FINISHED);
			return TRUE;

		case IDCANCEL:			
			_thiss->SendTextChatGuiRequestIPC(CHAT_CLOSE); // Client must close TextChat

			//	[v1.0.2-jp1 fix] UNSUBCLASS Split bar
            helper::SafeSetWindowProc(GetDlgItem(hWnd, IDC_STATIC_SPLIT), pDefSBProc);
			EndDialog(hWnd, FALSE);
			_thiss->m_fTextChatGuiRunning = false;
			_thiss->SendTextChatGuiRequestIPC(CHAT_FINISHED);
			return TRUE;

		case IDC_SEND_B:
			{
			memset(_thiss->m_szLocalText,0,TEXTMAXSIZE);
			UINT nRes = GetDlgItemText( hWnd, IDC_INPUTAREA_EDIT, _thiss->m_szLocalText, TEXTMAXSIZE-1);
			strcat_s(_thiss->m_szLocalText,TEXTMAXSIZE, "\n");
			memcpy(_thiss->m_szLastLocalText,_thiss-> m_szLocalText, strlen(_thiss->m_szLocalText));
			_thiss->PrintMessage(_thiss->m_szLocalText, _thiss->m_szLocalName, BLUE);
			_thiss->SendLocalTextIPC();	
			SetDlgItemText(hWnd, IDC_INPUTAREA_EDIT, "");
			SetFocus(GetDlgItem(hWnd, IDC_INPUTAREA_EDIT));
			}
			return TRUE;

		case IDC_INPUTAREA_EDIT:
			if(HIWORD(wParam) == EN_UPDATE)			
			{
				UINT nRes = GetDlgItemText( hWnd, IDC_INPUTAREA_EDIT, _thiss->m_szLocalText, TEXTMAXSIZE);
				if (strstr(_thiss->m_szLocalText, "\n") > 0 ) // Enter triggers the message transmission
				{
					// nRes = GetDlgItemText( hWnd, IDC_USERID_EDIT, _this->m_szUserID, 16);
					memcpy(_thiss->m_szLastLocalText, _thiss->m_szLocalText, strlen(_thiss->m_szLocalText));
					_thiss->PrintMessage(_thiss->m_szLocalText,_thiss-> m_szLocalName, BLUE);
					_thiss->SendLocalTextIPC();
					SetDlgItemText(hWnd, IDC_INPUTAREA_EDIT, "");
				}								
			}
			return TRUE;

		case IDC_HIDE_B:
			_thiss->ShowChatWindow(false);
			return TRUE;

		default:
			return TRUE;
			break;
		}
		break;

	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case SC_RESTORE:
			_thiss->ShowChatWindow(true);
			//SetFocus(GetDlgItem(hWnd, IDC_INPUTAREA_EDIT));
			return TRUE;
		}
		break;

	//	[v1.0.2-jp1 fix-->]
	case WM_SIZING:
		LPRECT lprc;
		lprc = (LPRECT)lParam;
		switch(wParam){
		case WMSZ_TOPLEFT:
			AdjustTop(lprc);
			AdjustLeft(lprc);
		case WMSZ_TOP:
			AdjustTop(lprc);
		case WMSZ_TOPRIGHT:
			AdjustTop(lprc);
			AdjustRight(lprc);
		case WMSZ_LEFT:
			AdjustLeft(lprc);
		case WMSZ_RIGHT:
			AdjustRight(lprc);
		case WMSZ_BOTTOMLEFT:
			AdjustBottom(lprc);
			AdjustLeft(lprc);
		case WMSZ_BOTTOM:
			AdjustBottom(lprc);
		case WMSZ_BOTTOMRIGHT:
			AdjustBottom(lprc);
			AdjustRight(lprc);
		}
		return TRUE;

	case WM_SIZE:
		int cx;
		int cy;
		int icy;
		RECT rc;

		if(wParam == SIZE_MINIMIZED){
			break;
		}

		cx = LOWORD(lParam);
		cy = HIWORD(lParam);
		GetWindowRect(GetDlgItem(hWnd, IDC_INPUTAREA_EDIT), &rc);
		icy = rc.bottom - rc.top;
		if(cy - icy - 12 < 80){
			icy = cy - 92;
		}
		MoveWindow(GetDlgItem(hWnd, IDC_CHATAREA_EDIT),  4,             4, cx -  8, cy - icy - 16, TRUE); 
		MoveWindow(GetDlgItem(hWnd, IDC_STATIC_SPLIT),   4, cy - icy - 12, cx -  8,             8, TRUE); 
		MoveWindow(GetDlgItem(hWnd, IDC_INPUTAREA_EDIT), 4, cy - icy -  4, cx - 88,           icy, TRUE);

		MoveWindow(GetDlgItem(hWnd, IDC_SEND_B), cx - 76, cy - 64, 72, 20, TRUE); 
		MoveWindow(GetDlgItem(hWnd, IDC_HIDE_B), cx - 76, cy - 40, 72, 18, TRUE); 
		MoveWindow(GetDlgItem(hWnd, IDCANCEL),   cx - 76, cy - 22, 72, 18, TRUE);

		InvalidateRect(hWnd, NULL, FALSE);
		return TRUE;
	//	[<--v1.0.2-jp1 fix]

	case WM_DESTROY:
		// _this->m_fTextChatRunning = false;
		// _this->SendTextChatRequest(CHAT_FINISHED);
		EndDialog(hWnd, FALSE);
		return TRUE;
	}
	return 0;
}

//	[v1.0.2-jp1 fix-->] Split bar
void DrawResizeLine(HWND hWnd, int y)
{
	HWND hParent;
	RECT rc;
	HDC hDC;

	hParent = GetParent(hWnd);
	GetClientRect(hParent, &rc);

	if(y < 80){
		y = 80;
	}
	else if(y > rc.bottom - 80){
		y = rc.bottom - 80;
	}
	
	hDC = GetDC(hParent);
	SetROP2(hDC, R2_NOTXORPEN);
	MoveToEx(hDC, rc.left, y, NULL);
	LineTo(hDC, rc.right, y);
	MoveToEx(hDC, rc.left, y+1, NULL);
	LineTo(hDC, rc.right, y+1);
	MoveToEx(hDC, rc.left, y+2, NULL);
	LineTo(hDC, rc.right, y+2);
	MoveToEx(hDC, rc.left, y+3, NULL);
	LineTo(hDC, rc.right, y+3);
	ReleaseDC(hParent, hDC);
}

//	[v1.0.2-jp1 fix-->] Split bar proc
LRESULT CALLBACK SBProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bCapture;
	static UINT u;
	static int oldy;
	HWND hParent;
	RECT rc;
	POINT cp;
	int y;
	int cx;
	int cy;

	switch(uMsg){
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return TRUE;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		bCapture = TRUE;
		u = GetCaretBlinkTime();
		SetCaretBlinkTime(0x7fffffff);
		GetCursorPos(&cp);
		hParent = GetParent(hWnd);
		ScreenToClient(hParent, &cp);
		DrawResizeLine(hWnd, cp.y);
		oldy = cp.y;
		break;
	case WM_MOUSEMOVE:
		if(bCapture){
			GetCursorPos(&cp);
			hParent = GetParent(hWnd);
			ScreenToClient(hParent, &cp);
			DrawResizeLine(hWnd, oldy);
			DrawResizeLine(hWnd, cp.y);
			oldy = cp.y;
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		bCapture = FALSE;
		SetCaretBlinkTime(u);
		GetCursorPos(&cp);
		hParent = GetParent(hWnd);
		GetClientRect(hParent, &rc);
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
		ScreenToClient(hParent, &cp);
		DrawResizeLine(hWnd, cp.y);
		y = cp.y;
		if(y < 80){
			y = 80;
		}
		else if(y > cy - 80){
			y = cy - 80;
		}
		MoveWindow(GetDlgItem(hParent, IDC_CHATAREA_EDIT),  4,         4, cx -  8,       y - 4, TRUE); 
		MoveWindow(GetDlgItem(hParent, IDC_STATIC_SPLIT),   4,         y, cx -  8,           8, TRUE); 
		MoveWindow(GetDlgItem(hParent, IDC_INPUTAREA_EDIT), 4,     y + 8, cx - 88, cy - y - 12, TRUE);
		break;
	}

	return CallWindowProc((WNDPROC)pDefSBProc, hWnd, uMsg, wParam, lParam);
}