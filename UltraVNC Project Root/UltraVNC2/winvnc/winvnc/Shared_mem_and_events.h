#include "uvnc_session/uvnc_session/communication.h"
#include "vncServer.h"
#include "ReadProperties.h"

extern bool PostAddAutoConnectClient_bool;
extern bool PostAddNewClient_bool;
extern bool PostAddAutoConnectClient_bool_null;
extern bool PostAddConnectClient_bool;
extern bool PostAddConnectClient_bool_null;
extern bool PostAddNewRepeaterClient_bool;
extern char pszId_char[20];
extern VCard32 address_vcard;
extern int port_int;
extern char* g_szRepeaterHost;

void Init_shared_mem_and_events();
DWORD WINAPI tcp_controle_thread(LPVOID lpParam);
DWORD WINAPI settings_change_monitor(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread7(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread6(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread5(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread4(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread3(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread2(LPVOID lpParam);
DWORD WINAPI imp_desktop_thread(LPVOID lpParam);