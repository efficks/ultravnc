#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Marscha@2004 - authSSP: from stdhdrs.h, required for logging
#include "VNCLog.h"
extern VNCLog vnclog;
#include "inifile.h"

// No logging at all
#define LL_NONE		0
#define LL_STATE	0
#define LL_CLIENTS	1
#define LL_CONNERR	0
#define LL_SOCKERR	4
#define LL_INTERR	0

// Log internal warnings
#define LL_INTWARN	8
#define LL_INTINFO	9
#define LL_SOCKINFO	10
#define LL_ALL		10
#define VNCLOG(s)	(__FILE__ " : " s)
#define MAXSTRING 254

#include "localization.h" // Act : add localization on messages

typedef int (*CheckUserPasswordSDUniFn)(const char * userin, const char *password, const char *machine);
typedef BOOL (*CheckUserGroupPasswordFn)( char * userin,char *password,char *machine,char *group,int locdom);

CheckUserGroupPasswordFn CheckUserGroupPassword = 0;
int CheckUserGroupPasswordUni(char * userin,char *password,const char *machine);
int CheckUserGroupPasswordUni2(char * userin,char *password,const char *machine);
BOOL IsNewMSLogon();
CheckUserPasswordSDUniFn CheckUserPasswordSDUni = 0;



LONG
LoadInt(LPCSTR valname, LONG defval)
{
	IniFile myIniFile;
	return myIniFile.ReadInt("admin_auth", (char *)valname, defval);
}

TCHAR *
LoadString(LPCSTR keyname)
{
	IniFile myIniFile;	
	TCHAR *authhosts=new char[150];
	myIniFile.ReadString("admin_auth", (char *)keyname,authhosts,150);
	return (TCHAR *)authhosts;
}

void
SaveInt(LPCSTR valname, LONG val)
{
	IniFile myIniFile;
	myIniFile.WriteInt("admin_auth", (char *)valname, val);
}

void
SaveString(LPCSTR valname, TCHAR *buffer)
{
	IniFile myIniFile;
	myIniFile.WriteString("admin_auth", (char *)valname,buffer);
}

void
savegroup1(TCHAR *value)
{
	SaveString("group1", value);
}
TCHAR*
Readgroup1()
{
	TCHAR *value=NULL;
	value=LoadString ("group1");
	return value;
}

void
savegroup2(TCHAR *value)
{
	SaveString("group2", value);
}
TCHAR*
Readgroup2()
{
	TCHAR *value=NULL;
	value=LoadString ("group2");
	return value;
}

void
savegroup3(TCHAR *value)
{
	SaveString("group3", value);
}
TCHAR*
Readgroup3()
{
	TCHAR *value=NULL;
	value=LoadString ("group3");
	return value;
}

LONG
Readlocdom1(LONG returnvalue)
{
	returnvalue=LoadInt("locdom1",returnvalue);
	return returnvalue;
}

void
savelocdom1(LONG value)
{
	SaveInt("locdom1", value);
}

LONG
Readlocdom2(LONG returnvalue)
{
	returnvalue=LoadInt( "locdom2",returnvalue);
	return returnvalue;
}

void
savelocdom2(LONG value)
{
	SaveInt("locdom2", value);
}

LONG
Readlocdom3(LONG returnvalue)
{
	returnvalue=LoadInt("locdom3",returnvalue);
	return returnvalue;
}

void
savelocdom3(LONG value)
{
	SaveInt( "locdom3", value);
}

///////////////////////////////////////////////////////////
bool CheckAD()
{
	HMODULE hModule = LoadLibrary("Activeds.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

bool CheckNetapi95()
{
	HMODULE hModule = LoadLibrary("netapi32.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

bool CheckDsGetDcNameW()
{
	HMODULE hModule = LoadLibrary("netapi32.dll");
	if (hModule)
	{
		FARPROC test=NULL;
		test=GetProcAddress( hModule, "DsGetDcNameW" );
		FreeLibrary(hModule);
		if (test) return true;
	}
	return false;
}

bool CheckNetApiNT()
{
	HMODULE hModule = LoadLibrary("radmin32.dll");
	if (hModule)
	{
		FreeLibrary(hModule);
		return true;
	}
	return false;
}

int CheckUserGroupPasswordUni(char * userin,char *password,const char *machine)
{
	int result = 0;
	// Marscha@2004 - authSSP: if "New MS-Logon" is checked, call CUPSD in authSSP.dll,
	// else call "old" mslogon method.
	if (IsNewMSLogon()){
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH)) {
			char* p = strrchr(szCurrentDir, '\\');
			*p = '\0';
			strcat (szCurrentDir,"\\authSSP.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule) {
			CheckUserPasswordSDUni = (CheckUserPasswordSDUniFn) GetProcAddress(hModule, "CUPSD");
			vnclog.Print(LL_INTINFO, VNCLOG("GetProcAddress"));
			HRESULT hr = CoInitialize(NULL);
			result = CheckUserPasswordSDUni(userin, password, machine);
			vnclog.Print(LL_INTINFO, "CheckUserPasswordSDUni result=%i", result);
			CoUninitialize();
			FreeLibrary(hModule);
			//result = CheckUserPasswordSDUni(userin, password, machine);
		} else {
			LPCTSTR sz_ID_AUTHSSP_NOT_FO = // to be moved to localization.h
				"You selected ms-logon, but authSSP.dll\nwas not found.Check you installation";
			MessageBox(NULL, sz_ID_AUTHSSP_NOT_FO, sz_ID_WARNING, MB_OK);
		}
	} else 
		result = CheckUserGroupPasswordUni2(userin, password, machine);
	return result;
}

int CheckUserGroupPasswordUni2(char * userin,char *password,const char *machine)
{
	int result=0;
	BOOL NT4OS=false;
	BOOL W2KOS=false;
	char clientname[256];
	strcpy_s(clientname,256,machine);
	if (!CheckNetapi95() && !CheckNetApiNT())
	{
		return false;
	}
	OSVERSIONINFO VerInfo;
	VerInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (!GetVersionEx (&VerInfo))   // If this fails, something has gone wrong
		{
		  return FALSE;
		}
	if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VerInfo.dwMajorVersion == 4)
		{
			NT4OS=true;
		}
	if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && VerInfo.dwMajorVersion >= 5)
		{
			W2KOS=true;
		}
	//////////////////////////////////////////////////
	// Load reg settings
	//////////////////////////////////////////////////
	char pszgroup1[256];
	char pszgroup2[256];
	char pszgroup3[256];
	char *group1=NULL;
	char *group2=NULL;
	char *group3=NULL;
	long locdom1=1;
	long locdom2=0;
	long locdom3=0;
	group1=Readgroup1();
	group2=Readgroup2();
	group3=Readgroup3();
	locdom1=Readlocdom1(locdom1);
	locdom2=Readlocdom2(locdom2);
	locdom3=Readlocdom3(locdom3);
	strcpy(pszgroup1,"VNCACCESS");
	strcpy(pszgroup2,"Administrators");
	strcpy(pszgroup3,"VNCVIEWONLY");
	if (group1){strcpy(pszgroup1,group1);}
	if (group2){strcpy(pszgroup2,group2);}
	if (group3){strcpy(pszgroup3,group3);}

	savegroup1(pszgroup1);
	savegroup2(pszgroup2);
	savegroup3(pszgroup3);
	savelocdom1(locdom1);
	savelocdom2(locdom2);
	savelocdom3(locdom3);

	if (group1){strcpy(pszgroup1,group1);delete group1;}
	if (group2){strcpy(pszgroup2,group2);delete group2;}
	if (group3){strcpy(pszgroup3,group3);delete group3;}

	//////////////////////////////////////////////////
	// logon user only works on NT>
	// NT4/w2k only as service (system account)
	// XP> works also as application
	// Group is not used...admin access rights is needed
	// MS keep changes there security model for each version....
	//////////////////////////////////////////////////
if (strcmp(pszgroup1,"")!=NULL)
{
	
	///////////////////////////////////////////////////
	// NT4 domain and workgroups
	//
	///////////////////////////////////////////////////
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\workgrpdomnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup1,locdom1);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	////////////////////////////////////////////////////
	if ( NT4OS || W2KOS){
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\authadmin.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup1,locdom1);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	/////////////////////////////////////////////////////////////////
	if (CheckAD() && W2KOS && (locdom1==2||locdom1==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup1,locdom1);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
	//////////////////////////////////////////////////////////////////////
	if (CheckAD() && NT4OS && CheckDsGetDcNameW() && (locdom1==2||locdom1==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauthnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup1,locdom1);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
	//////////////////////////////////////////////////////////////////////
	if (CheckAD() && !NT4OS && !W2KOS && (locdom1==2||locdom1==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth9x.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup1,locdom1);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
}
/////////////////////////////////////////////////
if (strcmp(pszgroup2,"")!=NULL)
{
	///////////////////////////////////////////////////
	// NT4 domain and workgroups
	//
	///////////////////////////////////////////////////
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\workgrpdomnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup2,locdom2);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	//////////////////////////////////////////////////////
	if ( NT4OS || W2KOS){
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\authadmin.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup2,locdom2);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	//////////////////////////////////////////////////////////////////
	if (CheckAD() && W2KOS && (locdom2==2||locdom2==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup2,locdom2);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
	///////////////////////////////////////////////////////////////////////
	if (CheckAD() && NT4OS && CheckDsGetDcNameW() && (locdom2==2||locdom2==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauthnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup2,locdom2);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
	///////////////////////////////////////////////////////////////////////
	if (CheckAD() && !NT4OS && !W2KOS && (locdom2==2||locdom2==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth9x.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup2,locdom2);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
}
////////////////////////////
if (strcmp(pszgroup3,"")!=NULL)
{
	///////////////////////////////////////////////////
	// NT4 domain and workgroups
	//
	///////////////////////////////////////////////////
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\workgrpdomnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup3,locdom3);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	////////////////////////////////////////////////////////
	if ( NT4OS || W2KOS){
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\authadmin.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup3,locdom3);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		else MessageBox(NULL, sz_ID_AUTH_NOT_FO, sz_ID_WARNING, MB_OK);

	}
	if (result==1) goto accessOK;
	////////////////////////////////////////////////////////////////
	if (CheckAD() && W2KOS && (locdom3==2||locdom3==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup3,locdom3);
				CoUninitialize();
				FreeLibrary(hModule);
			}
	}
	if (result==1) goto accessOK;
	///////////////////////////////////////////////////////////////////
	if (CheckAD() && NT4OS && CheckDsGetDcNameW() && (locdom3==2||locdom3==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauthnt4.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup3,locdom3);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		}
		if (result==1) goto accessOK2;
		///////////////////////////////////////////////////////////////////
	if (CheckAD() && !NT4OS && !W2KOS && (locdom3==2||locdom3==3))
	{
		char szCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return false;
			*p = '\0';
			strcat (szCurrentDir,"\\ldapauth9x.dll");
		}
		HMODULE hModule = LoadLibrary(szCurrentDir);
		if (hModule)
			{
				CheckUserGroupPassword = (CheckUserGroupPasswordFn) GetProcAddress( hModule, "CUGP" );
				HRESULT hr = CoInitialize(NULL);
				result=CheckUserGroupPassword(userin,password,clientname,pszgroup3,locdom3);
				CoUninitialize();
				FreeLibrary(hModule);
			}
		}
		if (result==1) goto accessOK2;
	}

	/////////////////////////////////////////////////
	// If we reach this place auth failed
	/////////////////////////////////////////////////
	{
				typedef BOOL (*LogeventFn)(char *machine,char *user);
				LogeventFn Logevent = 0;
				char szCurrentDir[MAX_PATH];
				if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
					{
						char* p = strrchr(szCurrentDir, '\\');
						*p = '\0';
						strcat (szCurrentDir,"\\logging.dll");
					}
				HMODULE hModule = LoadLibrary(szCurrentDir);
				if (hModule)
					{
						Logevent = (LogeventFn) GetProcAddress( hModule, "LOGFAILEDUSER" );
						Logevent((char *)clientname,userin);
						FreeLibrary(hModule);
					}
				return result;
	}

	accessOK://full access
	{
				typedef BOOL (*LogeventFn)(char *machine,char *user);
				LogeventFn Logevent = 0;
				char szCurrentDir[MAX_PATH];
				if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
					{
						char* p = strrchr(szCurrentDir, '\\');
						*p = '\0';
						strcat (szCurrentDir,"\\logging.dll");
					}
				HMODULE hModule = LoadLibrary(szCurrentDir);
				if (hModule)
					{
						Logevent = (LogeventFn) GetProcAddress( hModule, "LOGLOGONUSER" );
						Logevent((char *)clientname,userin);
						FreeLibrary(hModule);
					}
				return result;
	}

	accessOK2://readonly
	{
				typedef BOOL (*LogeventFn)(char *machine,char *user);
				LogeventFn Logevent = 0;
				char szCurrentDir[MAX_PATH];
				if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
					{
						char* p = strrchr(szCurrentDir, '\\');
						*p = '\0';
						strcat (szCurrentDir,"\\logging.dll");
					}
				HMODULE hModule = LoadLibrary(szCurrentDir);
				if (hModule)
					{
						Logevent = (LogeventFn) GetProcAddress( hModule, "LOGLOGONUSER" );
						Logevent((char *)clientname,userin);
						FreeLibrary(hModule);
					}
				result=2;
	}

	return result;
}

// Marscha@2004 - authSSP: Is New MS-Logon activated?
BOOL IsNewMSLogon(){
	IniFile myIniFile;	
	BOOL newmslogon=false;
	newmslogon=myIniFile.ReadInt("admin", "NewMSLogon", newmslogon);
	return newmslogon;
}
