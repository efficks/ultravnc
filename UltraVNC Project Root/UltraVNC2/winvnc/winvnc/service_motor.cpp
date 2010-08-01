#include "stdhdrs.h"
#include <windows.h>
#include <Wtsapi32.h>
#include "common/win32_helpers.h"

static SERVICE_STATUS serviceStatus;
static SERVICE_STATUS_HANDLE serviceStatusHandle=0;
char service_name[256]="uvnc_service";
static void WINAPI control_handler(DWORD controlCode);
static DWORD WINAPI control_handler_ex(DWORD controlCode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
void monitor_sessions();
void disconnect_remote_sessions();
HANDLE stopServiceEvent=0;
extern int clear_console;
extern bool			fShutdownOrdered;
////////////////////////////////////////////////////////////////////////////////
bool IsWin2000()
{
	OSVERSIONINFO OSversion;
	
	OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&OSversion);

    if (OSversion.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (OSversion.dwMajorVersion==5 && OSversion.dwMinorVersion==0)
            return true; 
						
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////////
static void WINAPI service_main(DWORD argc, LPTSTR* argv) {
    /* initialise service status */
    serviceStatus.dwServiceType=SERVICE_WIN32;
    serviceStatus.dwCurrentState=SERVICE_STOPPED;
    serviceStatus.dwControlsAccepted=0;
    serviceStatus.dwWin32ExitCode=NO_ERROR;
    serviceStatus.dwServiceSpecificExitCode=NO_ERROR;
    serviceStatus.dwCheckPoint=0;
    serviceStatus.dwWaitHint=0;

    typedef SERVICE_STATUS_HANDLE (WINAPI * pfnRegisterServiceCtrlHandlerEx)(LPCTSTR, LPHANDLER_FUNCTION_EX, LPVOID);
    helper::DynamicFn<pfnRegisterServiceCtrlHandlerEx> pRegisterServiceCtrlHandlerEx("advapi32.dll","RegisterServiceCtrlHandlerExA");

    if (pRegisterServiceCtrlHandlerEx.isValid())
      serviceStatusHandle = (*pRegisterServiceCtrlHandlerEx)(service_name, control_handler_ex, 0);
    else 
      serviceStatusHandle = RegisterServiceCtrlHandler(service_name, control_handler);

    if(serviceStatusHandle) {
        /* service is starting */
        serviceStatus.dwCurrentState=SERVICE_START_PENDING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

        /* do initialisation here */
        stopServiceEvent=CreateEvent(0, FALSE, FALSE, 0);

        /* running */
        serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        if (!IsWin2000())
            serviceStatus.dwControlsAccepted |= SERVICE_ACCEPT_SESSIONCHANGE;

        serviceStatus.dwCurrentState=SERVICE_RUNNING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

monitor_sessions();

        /* service was stopped */
        serviceStatus.dwCurrentState=SERVICE_STOP_PENDING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);

        /* do cleanup here */
        if (stopServiceEvent) CloseHandle(stopServiceEvent);
        stopServiceEvent=0;

        /* service is now stopped */
        serviceStatus.dwControlsAccepted&=
            ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        serviceStatus.dwCurrentState=SERVICE_STOPPED;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
    }
}
////////////////////////////////////////////////////////////////////////////////
static void WINAPI control_handler(DWORD controlCode)
{
  control_handler_ex(controlCode, 0, 0, 0);
}
////////////////////////////////////////////////////////////////////////////////
static DWORD WINAPI control_handler_ex(DWORD controlCode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
    switch (controlCode) {
    case SERVICE_CONTROL_INTERROGATE:
        break;

    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        serviceStatus.dwCurrentState=SERVICE_STOP_PENDING;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        SetEvent(stopServiceEvent);
		fShutdownOrdered=true;
        return NO_ERROR;

    case SERVICE_CONTROL_PAUSE:
        break;

    case SERVICE_CONTROL_CONTINUE:
        break;

    case SERVICE_CONTROL_SESSIONCHANGE:
        {
            if (dwEventType == WTS_REMOTE_DISCONNECT)
            {
                // disconnect rdp, and reconnect to the console
                if ( clear_console) disconnect_remote_sessions();
            }
        }
        break;

    default:
        if(controlCode >= 128 && controlCode <= 255)
            break; 
        else
            break;
    }
    SetServiceStatus(serviceStatusHandle, &serviceStatus);
    return NO_ERROR;
}
////////////////////////////////////////////////////////////////////////////////
int start_service() {
    SERVICE_TABLE_ENTRY serviceTable[]={
	 {service_name, service_main},
        {0, 0}
    };

    if(!StartServiceCtrlDispatcher(serviceTable)) {
        return 1;
    }
    return 0; /* NT service started */
}