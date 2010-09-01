/////////////////////////////////////////////////////////////////////////////
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
#include <winsock2.h>
#include <windows.h>

#define GETPORT 1
#define GETHTTPPORT 2

typedef struct _compack
{
	int version;
	int command;
	int int_value;
	bool b_value;
	char key[130];
	char text[512];
	char dummy[30];
}compack;

class controlethread
{

// Fields
public:
	struct sockaddr_in ServerAddress;
	SOCKET ListenSocket, RemoteSocket;
	WSADATA wsaData;
	bool WSAInit();
	bool SetupListener();
	bool SetupConnect();
	bool Accept();
	bool read(compack *bufpack);
	bool write(compack *bufpack);

public:	
	controlethread();
	~controlethread();
};