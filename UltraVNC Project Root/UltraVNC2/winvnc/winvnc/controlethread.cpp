#include "controlethread.h"

bool mastercon;
extern bool fShutdownOrdered;


bool
controlethread::Accept()
{
	struct sockaddr_in ClientAddress;
	int    nClientLength;
     nClientLength = sizeof(ClientAddress);
     
     //Accept remote connection attempt from the client
	 //debug("Wait on port 5995 for control (listening).");
     RemoteSocket = accept(ListenSocket, (struct sockaddr *) &ClientAddress, &nClientLength);
	 //debug("Accept 5995.");
	 if (fShutdownOrdered ) 
	 {
		 //exit asked
		closesocket(RemoteSocket);
		shutdown(RemoteSocket, 1);
		closesocket(ListenSocket);
		return 0;
	 }
     
     if (INVALID_SOCKET == RemoteSocket)
     {
          closesocket(ListenSocket);
          
          //debug("\nError occurred while accepting socket: %ld.", WSAGetLastError());
          return 0;
     }
     else
     {
          //debug("Connected port 5995.");
     }
	 mastercon=true;
	 return 1;
}
  
bool controlethread::read(compack *bufpack)
{
	struct fd_set read_fds;
	struct timeval tm;
	int count;
	tm.tv_sec = 0;
	tm.tv_usec = 1000;
	char *buf=(char*)bufpack;
	while (!fShutdownOrdered ) 
		{
			FD_ZERO(&read_fds);
			FD_SET( RemoteSocket, &read_fds );
			count = select(0, &read_fds, NULL, NULL, &tm);
			if ( count == -1 )
				{
					//debug( "select() failed, %d\n", WSAGetLastError());
					closesocket(RemoteSocket);
					shutdown(RemoteSocket, 1);
					mastercon=false;
					return 0;
				}
			else if (count==0)
			{
				Sleep(100);
			}
			else if (count==1)
			{
				int count=0;
				int count2=0;
				int templen=sizeof(bufpack);
				struct timeval tm;
				int bytes=0;
				int buffered=0;

				while (templen > 0 && !fShutdownOrdered) 
				{
				// Wait until some data can be read or sent
						do {
							FD_ZERO(&read_fds);
							FD_SET(RemoteSocket, &read_fds);
							tm.tv_sec = 1;
							tm.tv_usec = 0;
							count = select(0, &read_fds, NULL, NULL, &tm);
							if (count==0)
								{
									count2++;
									if (count2>5)
									{
										closesocket(RemoteSocket);
										shutdown(RemoteSocket, 1);
										mastercon=false;
										return 0;
									}
								}
						} while (count == 0 && !fShutdownOrdered);
						if (fShutdownOrdered) return 0;

				//data on socket
						if (FD_ISSET(RemoteSocket, &read_fds)) 
						{
							// Try to read some data in
							bytes = recv(RemoteSocket,buf + buffered, sizeof(compack) - buffered,0);
							if (bytes > 0) {
								// Adjust the buffer position and size
								buffered += bytes;
								templen-=bytes;
		//						debug("\nReceived %i bytes, %i to go",bytes,512-buffered);
							} 
							else if (bytes < 0)
							{
								//read error
								//debug("read error");
								closesocket(RemoteSocket);
								shutdown(RemoteSocket, 1);
								mastercon=false;
								return 0;
							}
							else if (bytes == 0)
							{
								//read error
								//debug("read error");
								closesocket(RemoteSocket);
								shutdown(RemoteSocket, 1);
								mastercon=false;
								return 0;
							}
						}
				}// all data is in buffer
				return 1;
			}//count==1
	}//while (!fShutdownOrdered ) 
	return 0;
}

bool
controlethread::write(compack *bufpack)
{
	int bufflen=sizeof(compack);
	char *buff=(char*)bufpack;
	int val =0;
	unsigned int totsend=0;
	while (totsend <bufflen)
	  {
		struct fd_set write_fds;
		struct timeval tm;
		int count;
		int aa=0;
		do {
			FD_ZERO(&write_fds);
			FD_SET(RemoteSocket, &write_fds);
			tm.tv_sec = 0;
			tm.tv_usec = 150;
			count = select(RemoteSocket+ 1, NULL, &write_fds, NULL, &tm);
		} while (count == 0&& !fShutdownOrdered);
		if (fShutdownOrdered) return 0;
		if (count < 0 || count > 1) return 0;
		if (FD_ISSET(RemoteSocket, &write_fds)) 
		{
			val=send(RemoteSocket, buff+totsend, bufflen-totsend, 0);
		}
		if (val==0) 
			return false;
		if (val==SOCKET_ERROR) 
			return false;
		totsend+=val;
	  }
	return 1;
}

bool
controlethread::SetupConnect()
{ 
     int    nPortNo;  
     RemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (INVALID_SOCKET == RemoteSocket) 
     {
          return 0;
     }
     else
     {
          //debug("\nsocket() successful.");
     }
     
     //Cleanup and Init with 0 the ServerAddress
     ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));  
     //Port number will be supplied as a commandline argument
     nPortNo = 5995;   
     //Fill up the address structure
     ServerAddress.sin_family = AF_INET;
     ServerAddress.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
     ServerAddress.sin_port = htons(nPortNo);    //comes from commandline

	 if (connect(RemoteSocket, (struct sockaddr *) &ServerAddress, sizeof (ServerAddress)) == SOCKET_ERROR)
	 {
		 Sleep(1000);
		 closesocket(RemoteSocket);
		 return 0;
	 }

	 return 1;
}

bool
controlethread::SetupListener()
{ 
     int    nPortNo;  
     struct sockaddr_in ServerAddress;
     ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (INVALID_SOCKET == ListenSocket) 
     {
          //debug("\nError occurred while opening socket: %ld.", WSAGetLastError());
          return 0;
     }
     else
     {
          //debug("\nsocket() successful.");
     }
     
     //Cleanup and Init with 0 the ServerAddress
     ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));  
     //Port number will be supplied as a commandline argument
     nPortNo = 5995;   
     //Fill up the address structure
     ServerAddress.sin_family = AF_INET;
     ServerAddress.sin_addr.s_addr = INADDR_ANY; //WinSock will supply address
     ServerAddress.sin_port = htons(nPortNo);    //comes from commandline
     
     //Assign local address and port number
     if (SOCKET_ERROR == bind(ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
     {
          closesocket(ListenSocket);          
          //debug("\nError occurred while binding.");
          return 0;
     }
     else
     {
          //debug("\nbind() successful.");
     }
     
     //Make the socket a listening socket
     if (SOCKET_ERROR == listen(ListenSocket,SOMAXCONN))
     {
          closesocket(ListenSocket);     
          //debug("\nError occurred while listening.");
          return 0;
     }
     else
     {
          //debug("\nlisten() successful.");
     }
	 return 1;
}

bool
controlethread::WSAInit()
{
    int nResult;   
    nResult = WSAStartup(MAKEWORD(2,2), &wsaData);     
    if (NO_ERROR != nResult)
     {
          //debug("\nError occurred while executing WSAStartup().");
          return 0; //error
     }
    else
     {
          //debug("\nWSAStartup() successful.");
     }
	return 1;
}

controlethread::~controlethread()
{
	WSACleanup();
	closesocket(RemoteSocket);
	closesocket(ListenSocket);
}

controlethread::controlethread()
{
//	strcpy(G_key,"mykey12345");
}


DWORD WINAPI workthread( LPVOID lpParam ) 
{
	controlethread MTC;
	if (!MTC.WSAInit()) return 0;
	if (!MTC.SetupListener()) return 0;
	while(!fShutdownOrdered)
	{
		if(MTC.Accept())
		{
			while(!fShutdownOrdered)
			{
				compack bufpack;
				if (!MTC.read(&bufpack)) break;
				switch(bufpack.command)
				{
				}
			}
		}
	}
	return 0;
}