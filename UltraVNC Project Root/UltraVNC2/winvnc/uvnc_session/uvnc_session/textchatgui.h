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