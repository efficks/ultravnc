#include "stdafx.h"
#include "stdio.h"

char app_name[]="UltraVnc";
char service_name[]="uvnc_service";
static char service_path[MAX_PATH];

int padd()
{
	char exe_file_name[MAX_PATH], dir[MAX_PATH], *ptr;
    GetModuleFileName(0, exe_file_name, MAX_PATH);

    /* set current directory */
    strcpy(dir, exe_file_name);
    ptr=strrchr(dir, '\\'); /* last backslash */
    if(ptr)
        ptr[1]='\0'; /* truncate program name */
    if(!SetCurrentDirectory(dir)) {
        return 1;
    }

    strcpy(service_path, "\"");
    strcat(service_path, dir);
	strcat(service_path, "winvnc.exe");
    strcat(service_path, "\" -service");
	return 0;
}

void set_service_description()
{
    // Add service description 
	DWORD	dw;
	HKEY hKey;
	char tempName[256];
    char desc[] = "Provides secure remote desktop sharing";
	_snprintf(tempName,  sizeof tempName, "SYSTEM\\CurrentControlSet\\Services\\%s", service_name);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
						tempName,
						0,
						REG_NONE,
						REG_OPTION_NON_VOLATILE,
						KEY_READ|KEY_WRITE,
						NULL,
						&hKey,
						&dw);
	RegSetValueEx(hKey,
					"Description",
					0,
					REG_SZ,
					(const BYTE *)desc,
					strlen(desc)+1);


	RegCloseKey(hKey);
}

int install_service(void) {
    SC_HANDLE scm, service;
	padd();

    scm=OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if(!scm) {
        MessageBox(NULL, "Failed to open service control manager",
            app_name, MB_ICONERROR);
        return 1;
    }
    //"Provides secure remote desktop sharing"
    service=CreateService(scm,service_name, service_name, SERVICE_ALL_ACCESS,
                          SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                          SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, service_path,
        NULL, NULL, NULL, NULL, NULL);
    if(!service) {
		DWORD myerror=GetLastError();
		if (myerror==ERROR_ACCESS_DENIED)
		{
			MessageBox(NULL, "Failed: Permission denied",
            app_name, MB_ICONERROR);
			CloseServiceHandle(scm);
			return 1;
		}
		if (myerror==ERROR_SERVICE_EXISTS)
		{
			MessageBox(NULL, "Failed: Already exist",
            "UltraVnc", MB_ICONERROR);
			CloseServiceHandle(scm);
			return 1;
		}

        MessageBox(NULL, "Failed to create a new service",
            app_name, MB_ICONERROR);
        CloseServiceHandle(scm);
        return 1;
    }
    else 
		{
			set_service_description();
			MessageBox(NULL, "uvnc_service installed",
            app_name, MB_ICONERROR);
		}
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return 0;
}

int uninstall_service(void) {
    SC_HANDLE scm, service;
    SERVICE_STATUS serviceStatus;

    scm=OpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if(!scm) {
        MessageBox(NULL, "Failed to open service control manager",
            app_name, MB_ICONERROR);
        return 1;
    }

	service=OpenService(scm, service_name,
        SERVICE_QUERY_STATUS | DELETE);
    if(!service) {
		DWORD myerror=GetLastError();
		if (myerror==ERROR_ACCESS_DENIED)
		{
			MessageBox(NULL, "Failed: Permission denied",
            app_name, MB_ICONERROR);
			CloseServiceHandle(scm);
			return 1;
		}
		if (myerror==ERROR_SERVICE_DOES_NOT_EXIST)
		{

			MessageBox(NULL, "Failed: Service is not installed",
            app_name, MB_ICONERROR);

			CloseServiceHandle(scm);
			return 1;
		}

        MessageBox(NULL, "Failed to open the service",
            app_name, MB_ICONERROR);
        CloseServiceHandle(scm);
        return 1;
    }
    if(!QueryServiceStatus(service, &serviceStatus)) {
        MessageBox(NULL, "Failed to query service status",
            app_name, MB_ICONERROR);
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 1;
    }
    if(serviceStatus.dwCurrentState!=SERVICE_STOPPED) {
        //MessageBox(NULL, "The service is still running, disable it first",
        //    "UltraVnc", MB_ICONERROR);
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
		Sleep(2500);uninstall_service();
        return 1;
    }
    if(!DeleteService(service)) {
        MessageBox(NULL, "Failed to delete the service",
            app_name, MB_ICONERROR);
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 1;
    }
	 MessageBox(NULL, "service deleted",
            app_name, MB_ICONERROR);
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return 0;
}