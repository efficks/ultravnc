#include "stdafx.h"
#include "resource.h"
#include "DSMPlugin.h"
#include <comutil.h>
#include "vncsetauth.h"
extern HINSTANCE hInst;
extern LONG UseDSMPlugin;
char* GetDSMPluginName();
void SetDSMPluginName(char* szDSMPlugin);
CDSMPlugin* m_pDSMPlugin=NULL;
CDSMPlugin* GetDSMPluginPointer() { return m_pDSMPlugin;};
vncSetAuth m_vncauth;
extern char DSMPlugin[128];

BOOL CALLBACK DlgProcEncryption(HWND hwnd, UINT uMsg,
											   WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg) {
		
	case WM_INITDIALOG: 
		{	
			m_pDSMPlugin = new CDSMPlugin();
			SetDSMPluginName(DSMPlugin);
			SendMessage(GetDlgItem(hwnd, IDC_PLUGIN_CHECK), BM_SETCHECK, UseDSMPlugin, 0);
			HWND hPlugins = GetDlgItem(hwnd, IDC_PLUGINS_COMBO);
			int nPlugins = GetDSMPluginPointer()->ListPlugins(hPlugins);
			if (!nPlugins) 
			{
				SendMessage(hPlugins, CB_ADDSTRING, 0, (LPARAM) "No Plugin Detected");
				SendMessage(hPlugins, CB_SETCURSEL, 0, 0);
			}
			else
				SendMessage(hPlugins, CB_SELECTSTRING, 0, (LPARAM)GetDSMPluginName());
			return TRUE;
		}
	
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{	
		case IDC_PLUGIN_BUTTON:
			{
				HWND hPlugin = GetDlgItem(hwnd, IDC_PLUGIN_CHECK);
				if (SendMessage(hPlugin, BM_GETCHECK, 0, 0) == BST_CHECKED)
				{
					TCHAR szPlugin[MAX_PATH];
					GetDlgItemText(hwnd, IDC_PLUGINS_COMBO, szPlugin, MAX_PATH);
					if (!GetDSMPluginPointer()->IsLoaded())
						GetDSMPluginPointer()->LoadPlugin(szPlugin, false);
					else
					{
						// sf@2003 - We check if the loaded plugin is the same than
						// the currently selected one or not
						GetDSMPluginPointer()->DescribePlugin();
						if (_stricmp(GetDSMPluginPointer()->GetPluginFileName(), szPlugin))
						{
							GetDSMPluginPointer()->UnloadPlugin();
							GetDSMPluginPointer()->LoadPlugin(szPlugin, false);
						}
					}
				
					if (GetDSMPluginPointer()->IsLoaded())
					{
						// We don't send the password yet... no matter the plugin requires
						// it or not, we will provide it later (at plugin "real" init)
						// Knowing the environnement ("server-svc" or "server-app") right 
						// now can be usefull or even mandatory for the plugin 
						// (specific params saving and so on...)
						char szParams[32];
						strcpy(szParams, "NoPassword,");
						strcat(szParams, "server-app");
						GetDSMPluginPointer()->SetPluginParams(hwnd, szParams);
					}
					/*else
					{
						MessageBox(NULL, 
							sz_ID_PLUGIN_NOT_LOAD, 
							sz_ID_PLUGIN_LOADIN, MB_OK | MB_ICONEXCLAMATION );
					}*/
				}				
				return TRUE;
			}
			case IDC_MSLOGON_CHECKD:
			{
				BOOL bMSLogonChecked =
				(SendDlgItemMessage(hwnd, IDC_MSLOGON_CHECKD,
										BM_GETCHECK, 0, 0) == BST_CHECKED);

				EnableWindow(GetDlgItem(hwnd, IDC_NEW_MSLOGON), bMSLogonChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_MSLOGON), bMSLogonChecked);

			}
			return TRUE;
			case IDC_MSLOGON:
			{
				// Marscha@2004 - authSSP: if "New MS-Logon" is checked,
				// call vncEditSecurity from SecurityEditor.dll,
				// else call "old" dialog.
				BOOL bNewMSLogonChecked =
				(SendDlgItemMessage(hwnd, IDC_NEW_MSLOGON,
										BM_GETCHECK, 0, 0) == BST_CHECKED);
				if (bNewMSLogonChecked) 
					{
						typedef void (*vncEditSecurityFn) (HWND hwnd, HINSTANCE hInstance);
						vncEditSecurityFn vncEditSecurity = 0;
						char szCurrentDir[MAX_PATH];
							if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH)) {
								char* p = strrchr(szCurrentDir, '\\');
								*p = '\0';
								strcat (szCurrentDir,"\\authSSP.dll");
							}
						HMODULE hModule = LoadLibrary(szCurrentDir);
						if (hModule) {
							vncEditSecurity = (vncEditSecurityFn) GetProcAddress(hModule, "vncEditSecurity");
							HRESULT hr = CoInitialize(NULL);
							vncEditSecurity(NULL, hInst);
							CoUninitialize();
							FreeLibrary(hModule);
						}
					}
				 else { 
					// Marscha@2004 - authSSP: end of change
					m_vncauth.Show(TRUE);
				}
			}
			return TRUE;

		case IDOK:	
			{
				UseDSMPlugin=SendDlgItemMessage(hwnd, IDC_PLUGIN_CHECK, BM_GETCHECK, 0, 0);
				GetDlgItemText(hwnd, IDC_PLUGINS_COMBO, DSMPlugin, MAX_PATH);
				
			}
			break;
		case IDCANCEL:
			if (m_pDSMPlugin != NULL)
				{
					delete(m_pDSMPlugin);
					m_pDSMPlugin=NULL;
				}
			EndDialog(hwnd, IDCANCEL);
			return TRUE;
		}
		return 0;	
	}

	return 0;
}