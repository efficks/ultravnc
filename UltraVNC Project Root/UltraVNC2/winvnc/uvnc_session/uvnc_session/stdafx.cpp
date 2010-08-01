// stdafx.cpp : source file that includes just the standard includes
// uvnc_session.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
VNCLog vnclog;
BOOL SPECIAL_SC_PROMPT=false;
HINSTANCE hInst;
bool GLOBAL_RUNNING=true;
bool app;
char old_buffer[1024];
char old_buflen=0;
int counter=0;
bool bool_GetAllowEditClients;
bool g_DesktopThread_running;
bool g_update_triggered;
bool g_Desktop_running;


