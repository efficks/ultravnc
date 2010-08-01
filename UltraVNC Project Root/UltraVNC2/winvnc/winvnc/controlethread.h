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