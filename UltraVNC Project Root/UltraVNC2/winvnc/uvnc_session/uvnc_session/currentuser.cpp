#include "stdafx.h"


char aa[16384];
#include <tlhelp32.h>

typedef DWORD (WINAPI* pWTSGetActiveConsoleSessionId)(VOID);
typedef BOOL (WINAPI * pProcessIdToSessionId)(DWORD,DWORD*);
pProcessIdToSessionId WTSProcessIdToSessionIdF1=NULL;

DWORD GetExplorerLogonPid()
{
	DWORD dwSessionId;
	DWORD dwExplorerLogonPid=0;
	PROCESSENTRY32 procEntry;
//	HANDLE hProcess,hPToken;

	pWTSGetActiveConsoleSessionId WTSGetActiveConsoleSessionIdF=NULL;
	WTSProcessIdToSessionIdF1=NULL;

	HMODULE  hlibkernel = LoadLibrary("kernel32.dll"); 
	if (hlibkernel)
	{
	WTSGetActiveConsoleSessionIdF=(pWTSGetActiveConsoleSessionId)GetProcAddress(hlibkernel, "WTSGetActiveConsoleSessionId");
	WTSProcessIdToSessionIdF1=(pProcessIdToSessionId)GetProcAddress(hlibkernel, "ProcessIdToSessionId");
	}
	if (WTSGetActiveConsoleSessionIdF!=NULL)
	   dwSessionId =WTSGetActiveConsoleSessionIdF();
	else dwSessionId=0;

	if( GetSystemMetrics( SM_REMOTESESSION))
		if (WTSProcessIdToSessionIdF1!=NULL)
		{
			DWORD dw		 = GetCurrentProcessId();
			DWORD pSessionId = 0xFFFFFFFF;
			WTSProcessIdToSessionIdF1( dw, &pSessionId );
			dwSessionId=pSessionId;
		}

	

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
    {
		if (hlibkernel) FreeLibrary(hlibkernel);
        return 0 ;
    }

    procEntry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnap, &procEntry))
    {
		CloseHandle(hSnap);
		if (hlibkernel) FreeLibrary(hlibkernel);
        return 0 ;
    }

    do
    {
        if (_stricmp(procEntry.szExeFile, "explorer.exe") == 0)
        {
          DWORD dwExplorerSessId = 0;
		  if (WTSProcessIdToSessionIdF1!=NULL)
		  {
			  if (WTSProcessIdToSessionIdF1(procEntry.th32ProcessID, &dwExplorerSessId) 
						&& dwExplorerSessId == dwSessionId)
				{
					dwExplorerLogonPid = procEntry.th32ProcessID;
					break;
				}
		  }
		  else dwExplorerLogonPid = procEntry.th32ProcessID;
        }

    } while (Process32Next(hSnap, &procEntry));
	CloseHandle(hSnap);
	if (hlibkernel) FreeLibrary(hlibkernel);
	return dwExplorerLogonPid;
}

bool
GetConsoleUser(char *buffer, UINT size)
{

	HANDLE hProcess,hPToken;
	DWORD dwExplorerLogonPid=GetExplorerLogonPid();
	if (dwExplorerLogonPid==0) 
	{
		strcpy_s(buffer,size,"");
		return 0;
	}
	hProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,dwExplorerLogonPid);

   if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
                                    |TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
                                    |TOKEN_READ|TOKEN_WRITE,&hPToken))
		{     
			   strcpy_s(buffer,size,"");
			   CloseHandle(hProcess);
			   return 0 ;
		}


   // token user
    TOKEN_USER *ptu;
	DWORD needed;
	ptu = (TOKEN_USER *) aa;//malloc( 16384 );
	if (GetTokenInformation( hPToken, TokenUser, ptu, 16384, &needed ) )
	{
		char  DomainName[64];
		memset(DomainName, 0, sizeof(DomainName));
		DWORD DomainSize;
		DomainSize =sizeof(DomainName)-1;
		SID_NAME_USE SidType;
		DWORD dwsize=size;
		LookupAccountSid(NULL, ptu->User.Sid, buffer, &dwsize, DomainName, &DomainSize, &SidType);
		//free(ptu);
		CloseHandle(hPToken);
		CloseHandle(hProcess);
		return 1;
	}
	//free(ptu);
	strcpy_s(buffer,size,"");
	CloseHandle(hPToken);
	CloseHandle(hProcess);
	return 0;
}

bool GetCurrentUser(char *buffer, UINT size,bool app)
{
	if (!app)
	{
		HWINSTA station = GetProcessWindowStation();
		if (station == NULL) return FALSE;
		DWORD usersize;
		GetUserObjectInformation(station, UOI_USER_SID, NULL, 0, &usersize);
		if (usersize == 0) 
		{
			strcpy_s(buffer,size, "");
			return true;
		}
	}


	DWORD length = size;
	if ( GetConsoleUser(buffer, size) == 0)
			{
				if (GetUserName(buffer, &length) == 0)
				{
					UINT error = GetLastError();
					if (error == ERROR_NOT_LOGGED_ON)
					{
						strcpy_s(buffer,size, "");
						return TRUE;
					}
					else
					{
						return false;
					}
				}
			}
	return true;

}