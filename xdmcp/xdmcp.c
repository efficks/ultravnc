//  Copyright (C) 2002 Ultr@Vnc Team Members. All Rights Reserved.
//  Original xwinx cygwin based code, modified for VC++ and Win32-X11
//  Copyright (C) 2001 Kim Saunders, Sanjay Rao
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
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
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <process.h>
#include <windows.h>

#include "X11/Xlib.h"
#include "X11/Xmd.h"
#include "X11/Xdmcp.h"

void *xdmcpmalloc (size_t size)
{
	void *mem;
	struct params *dummyparams = 0;

	mem = malloc(size);
	
	return(mem);
}


void *xdmcpcalloc (size_t nmemb, size_t size)
{
	unsigned int bytes;
	void *mem;
	if (nmemb==1) bytes=size;
	else bytes=nmemb*size;


	mem=xdmcpmalloc(bytes);
	if (mem) memset(mem,0,bytes);

	return(mem);

}

void *xdmcprealloc (void *ptr, size_t size)
{
	void *mem;
	struct params *dummyparams = 0;
	
	mem = realloc(ptr, size);
	return(mem);
}

void *xdmcpstrdup (const char *string)
{
	void *mem;
	struct params *dummyparams = 0;

	mem = strdup(string);

	return(mem);
}


char *xdmcphostname_gethostname()
{
	LPTSTR host_str;
	DWORD host_siz; 
	char *hostname;
	
	host_siz = MAX_COMPUTERNAME_LENGTH + 1; 
	host_str = xdmcpmalloc(host_siz);

	GetComputerName(host_str, &host_siz);
	hostname = xdmcpstrdup(host_str);
	free(host_str);
	
	return(hostname);
}


/* structure to hold info for a session. only the cookie and ID are stored
 * at the moment, but in future we may wish to store the session start time,
 * display number, etc. not sure, just used a struct for flexibility basically.
 */
struct xdmcpdm_session
{
	unsigned char cookie[16];	/* MIT-MAGIC-COOKIE-1 session cookie */
	unsigned int id;		/* session ID */
};

/* file descriptor for socket */
static int fd = -1;

/* current session ID counter */
unsigned int sess_id;

/* sess_db is an array pointers to xdmcpdm_session structs */
struct xdmcpdm_session **sess_db;

typedef struct _XdmAuth
{
	ARRAY8 authentication;
	ARRAY8 authorization;
} XdmAuthRec, *XdmAuthPtr;

static XdmAuthRec slist = 
{
	{ (CARD16) 0, (CARD8 *) 0 },
	{ (CARD16) 0, (CARD8 *) 0 }
};

/* XdmcpBuffer for reading incoming xdmcp packets into */
XdmcpBuffer buffa;

/* XdmcpHeader to store the header of incoming packets in */
XdmcpHeader hedda;

/* sockaddr_in structure for the client connection */
struct sockaddr_in	client_sa;

/* string containing the IP address of the client, as a dotted quad */
char			*client_ip;

/* write the packet written to buffa to fd using XdmcpFlush */
void xdmcpdm_flush ()
{
	if (
		!XdmcpFlush(
			fd,
			&buffa,
			(XdmcpNetaddr)&client_sa,
			(int)sizeof(struct sockaddr_in)
		)
	)
		fprintf(stderr, "\tERROR: Can't flush XDMCP packet.\n");
}

/* sets the data member of an ARRAY8 and calculate and set the length member */
void set_ARRAY8(ARRAY8 *aeight, char *string)
{
	aeight->data = string;
	aeight->length = strlen(aeight->data);
}

/* create and send a WILLING packet */
void send_willing (ARRAY8Ptr hostname, ARRAY8Ptr status)
{
	/* header for outgoing WILLING packet */
	XdmcpHeader	willing;

	/* set up the header for the packet */
	willing.opcode = (CARD16) WILLING;
	willing.length = 6;
	willing.length += slist.authentication.length;
	willing.length += hostname->length;
	willing.length += status->length;
	willing.version = XDM_PROTOCOL_VERSION;

	/* write the packet into buffa */
	XdmcpWriteHeader(&buffa, &willing);
	XdmcpWriteARRAY8(&buffa, &slist.authentication);
	XdmcpWriteARRAY8(&buffa, hostname);
	XdmcpWriteARRAY8(&buffa, status);

	/* send the packet */
	xdmcpdm_flush();
}

void send_accept (
	CARD32Ptr session,
	ARRAY8Ptr authen_name,
	ARRAY8Ptr authen_data,
	ARRAY8Ptr author_name,
	ARRAY8Ptr author_data
)
{
	XdmcpHeader	accept;
	
	/* set up header for outgoing ACCEPT packet */
	accept.opcode = (CARD16) ACCEPT;
	accept.length = 12;
	accept.length += authen_name->length;
	accept.length += authen_data->length;
	accept.length += author_name->length;
	accept.length += author_data->length;
	accept.version = XDM_PROTOCOL_VERSION;

	/* write packing into buffa */

	/* packet header */
	XdmcpWriteHeader(&buffa, &accept);

	/* session ID number */
	XdmcpWriteCARD32(&buffa, *session);

	/* authentication name and data */
	XdmcpWriteARRAY8(&buffa, authen_name);
	XdmcpWriteARRAY8(&buffa, authen_data);

	/* authorisation name and data */
	XdmcpWriteARRAY8(&buffa, author_name);
	XdmcpWriteARRAY8(&buffa, author_data);

	/* send packet to client */
	xdmcpdm_flush();
}

/* respond to a QUERY packet that has been written info buffa */
void resp_query ()
{
	ARRAY8 host, stat;
	ARRAYofARRAY8 list;

	/* we don't actually use the auth list for anything, indeed it's
	 * usually empty, but we just read it to make sure the packet is
	 * properly formed.
	 */
	if (!XdmcpReadARRAYofARRAY8(&buffa, &list))
	{
		XdmcpDisposeARRAYofARRAY8(&list);
		fprintf(stderr, "\tERROR: Can't read auth list.\n");
		return;
	}

	/* setup ARRAY8s for the hostname and status strings to send back */
	set_ARRAY8(&host, xdmcphostname_gethostname());
	set_ARRAY8(&stat, strdup("Ultravnc"));

	/* send a WILLING packet back to the client. We're not fussy at the
	 * moment, sending a WILLING packet to anyone who sends a QUERY to us.
	 * At some point, I guess I'll add some sort of ACL to make sure the
	 * client IP address is legal, but at this stage XDMCP is bare-bones
	 */
	send_willing(&host, &stat);
	
	/* this frees memory pointed to these structures */
	XdmcpDisposeARRAY8(&host);
	XdmcpDisposeARRAY8(&stat);
	XdmcpDisposeARRAYofARRAY8(&list);
}

/* respond to a REQUEST packet in buffa */
void resp_request ()
{
	/* client display number, manufacturer and authorisation names */
	CARD16			cli_dpynumb;
	ARRAY8			cli_dpymanu;
	ARRAYofARRAY8		cli_authnames;
	
	/* server authentication, authorisation names and data */
	ARRAY8			srv_authenname;
	ARRAY8			srv_authendata;
	ARRAY8			srv_authorname;
	ARRAY8			srv_authordata;

	/* pointer so a xdmcpdm session struct to store session details in */
	struct xdmcpdm_session	*session;

	/* array of bytes to store 128 boot MIT-MAGIC-COOKIE-1 cookie in */
	unsigned char		*cookie_dat;

	/* int for loops */
	int			i;

	/* temp variables to read things into then discard */
	ARRAY8			tmp_ARRAY8;
	ARRAY16			tmp_ARRAY16;
	ARRAYofARRAY8		tmp_ARRAYofARRAY8;

	/* read all of the received packet fields. Again, not many of these
	 * actually matter to xdmcpdm, but we read them anyway to make sure the
	 * packet is properly formed with all the fields, and so we can print
	 * them for debugging if necessary
	 */

	/* this is the display number. keep this, relatively interesting */
	if (!XdmcpReadCARD16 (&buffa, &cli_dpynumb))
	{
		fprintf(stderr, "\tERROR: Can't read display number.\n");
		return;
	}

	/* these are connection types and connection addresses. NFI what it's
	 * about, so just read it into a temp variable to make sure it's
	 * there, then discard
	 */
	if (!XdmcpReadARRAY16 (&buffa, &tmp_ARRAY16))
	{
		fprintf(stderr, "\tERROR: Can't read connection type.\n");
		XdmcpDisposeARRAY16(&tmp_ARRAY16);
		return;
	}
	XdmcpDisposeARRAY16(&tmp_ARRAY16);

	if (!XdmcpReadARRAYofARRAY8 (&buffa, &tmp_ARRAYofARRAY8))
	{
		fprintf(stderr, "\tERROR: Can't read client addresses.\n");
		XdmcpDisposeARRAYofARRAY8(&tmp_ARRAYofARRAY8);
		return;
	}
	XdmcpDisposeARRAYofARRAY8(&tmp_ARRAYofARRAY8);

	/* this is for the X display to authenticate itself with us. neither
	 * xdmcpdm nor any other display managers seem to support this, so same
	 * deal, read it to make sure we can, then discard.
	 */
	if (!XdmcpReadARRAY8 (&buffa, &tmp_ARRAY8))
	{
		fprintf(stderr, "\tERROR: Can't read authentication name.\n");
		XdmcpDisposeARRAY8(&tmp_ARRAY8);
		return;
	}
	XdmcpDisposeARRAY8(&tmp_ARRAY8);

	if (!XdmcpReadARRAY8 (&buffa, &tmp_ARRAY8))
	{
		fprintf(stderr, "\tERROR: Can't read authentication data.\n");
		XdmcpDisposeARRAY8(&tmp_ARRAY8);
		return;
	}
	XdmcpDisposeARRAY8(&tmp_ARRAY8);
	
	/* this is an array of authorisation names that the X server accepts.
	 * This is important, we need to look at it, and use MIT-MAGIC-COOKIE-1
	 * if possible
	 */
	if (!XdmcpReadARRAYofARRAY8 (&buffa, &cli_authnames))
	{
		fprintf(stderr, "\tERROR: Can't read auth names.\n");
		XdmcpDisposeARRAYofARRAY8(&cli_authnames);
		return;
	}

	/* the manufacturer string for the client. not important, but human
	 * readable, so we read it and print it out.
	 */
	if (!XdmcpReadARRAY8 (&buffa, &cli_dpymanu))
	{
		fprintf(stderr, "\tERROR: Can't read display manufacturer.\n");
		XdmcpDisposeARRAYofARRAY8(&cli_authnames);
		XdmcpDisposeARRAY8(&cli_dpymanu);
		return;
	}

	/* print the display manufacturer and display number out, it's vaguely
	 * intersting...
	 */
	printf("\tDisplay number: %d\n", cli_dpynumb);
	printf("\tDisplay manufacturer: %s\n", cli_dpymanu.data);

	/* printed the manufacturer sting out so it can be discarded now */
	XdmcpDisposeARRAY8(&cli_dpymanu);

	/* go throught the authorisation names supported by the display */
	for (i = 0; i < cli_authnames.length; i++)
	{
		int j;

		/* XDMCP fields aren't neccesarily null terminated, so print
		 * out the name char by char instead of as a string
		 */
		printf("\tAuthorisation: ");
		for (j = 0; j < cli_authnames.data[i].length; j++)
			printf("%c", cli_authnames.data[i].data[j]);
		printf("\n");
	}

	/* set the authentication type and data to null. Authentication
	 * isn't supported by xdmcpdm, and doesn't seem to be used by anything
	 * else I've seen (gdm certainly doesn't use it)
	 */
	set_ARRAY8(&srv_authenname, strdup(""));
	set_ARRAY8(&srv_authendata, strdup(""));

	/* set up MIT-MAGIC-COOKIE-1 authorisation */
	set_ARRAY8(&srv_authorname, strdup("MIT-MAGIC-COOKIE-1"));
	
	/* ultra-secure random seed... */
	srand(getpid());

	/* allocate 16 bytes (128 bits) for the cookie */
	cookie_dat = (unsigned char *)xdmcpmalloc(16);

	/* set each byte of cookie_dat to a random number between 0 and 255 */
	for (i = 0; i < 16; i++)
		cookie_dat[i] = rand() % 256;
	
	/* don't use set_ARRAY8 since this is just an array of bytes, not a
	 * null terminated string
	 */
	srv_authordata.data = (CARD8Ptr)cookie_dat;
	srv_authordata.length = 16;

	/* send an ACCEPT packet to the client. xdmcpdm is still fairly
	 * primitive, we should be sending decline in some circumstances
	 * (dodgey packet, request that doesn't follow a query, etc)
	 */
	send_accept(
		(CARD32Ptr)&sess_id,
		&srv_authenname,
		&srv_authendata,
		&srv_authorname,
		&srv_authordata
	);

	/* reallocate the sess_db array to hold another pointer */
	sess_db = xdmcprealloc(
		sess_db,
		sizeof(struct xdmcpdm_session *) * (sess_id + 1)
	);

	/* allocate memory for an xdmcpdm_session struct */
	session = (struct xdmcpdm_session *)xdmcpmalloc(
		sizeof(struct xdmcpdm_session)
	);
	
	/* set the members to the session structure */
	session->id = sess_id;
	memcpy(&(session->cookie), cookie_dat, 16);

	/* parcket has been send, discard all the ARRAY8s */
	XdmcpDisposeARRAY8(&srv_authenname);
	XdmcpDisposeARRAY8(&srv_authendata);
	XdmcpDisposeARRAY8(&srv_authorname);
	XdmcpDisposeARRAY8(&srv_authordata);

	sess_db[sess_id] = session;

	sess_id++;
}

void resp_manage ()
{
	/* session and display number from display */
	CARD32			man_session;
	CARD16			man_display;
	char buffer[20];

	/* command to run */
	char			*run_str;
	int			run_len;

	/* a session to dig out from sess_db */
	struct xdmcpdm_session	*session;

	/* counter for loops */
	int			i;

	/* return value from snprintf */
	int			siz;

	/* string to hold 32 char hex string for cookie */
	char			chex[33];

	/* pid for forking */
	//pid_t			pid;
	
	if (!XdmcpReadCARD32(&buffa, &man_session))
	{
		fprintf(stderr, "\tERROR: Can't read session ID.\n");
		return;
	}

	if (!XdmcpReadCARD16(&buffa, &man_display))
	{
		fprintf(stderr, "\tERROR: Can't read display number.\n");
		return;
	}

	/* session_id is larger than the sessions xdmcpdm knows about. Ideally,
	 * xdmcpdm should send a REFUSE packet
	 */
	if ( !( (int)man_session < sess_id ) )
		return;

	/* fetch the relevant session record from the array */
	session = sess_db[ (int)man_session ];
	
	/* construct a string containing the hex for the 128 bit cookie */
	for (i = 0; i < 16; i++)
		sprintf((char *)&chex[i * 2], "%02x", session->cookie[i]);

	/* null terminator for string */
	chex[32] = 0;

	/* allocate memory for the run string */
	run_len = 100;
	run_str = (char *)xdmcpmalloc(run_len);
	siz = 0;
	strcpy(run_str,"\\connect.exe -display ");
	strcat(run_str,client_ip);
	strcat(run_str,":");
	strcat(run_str,itoa(man_display,buffer,10));
	strcat(run_str," -c ");
	strncat(run_str,chex,33);
	strcat(run_str," localhost:5900 ");	
	strcat(run_str,"\0");
	printf("\tStarting UltraVnc (session %d):\n", (int)man_session);
	printf("\t%s\n", run_str);
	{
	STARTUPINFO ssi;
	PROCESS_INFORMATION ppi;
	char szCurrentDir[MAX_PATH];
	if (GetModuleFileName(NULL, szCurrentDir, MAX_PATH))
		{
			char* p = strrchr(szCurrentDir, '\\');
			if (p == NULL) return;
			*p = '\0';
			strcat (szCurrentDir,run_str);
		}
			
		ZeroMemory( &ssi, sizeof(ssi) );
		ssi.cb = sizeof(ssi);
		// Start the child process. 
		CreateProcess( NULL, // No module name (use command line). 
			szCurrentDir, // Command line. 
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			0,                // No creation flags. 
			NULL,             // Use parent's environment block. 
			NULL,             // Use parent's starting directory. 
			&ssi,              // Pointer to STARTUPINFO structure.
			&ppi ) ;            // Pointer to PROCESS_INFORMATION structure.
	}		
	return;
}

/* return a name of an opcode as a string */
char *xdmcpdm_opcode_string (int opcode)
{
	switch (opcode)
	{
		case BROADCAST_QUERY:
			return strdup("BROADCAST_QUERY");
		case QUERY:
			return strdup("QUERY");
		case WILLING:
			return strdup("WILLING");
		case REQUEST:
			return strdup("REQUEST");
		case MANAGE:
			return strdup("MANAGE");
		case ACCEPT:
			return strdup("ACCEPT");
		default:
			return strdup("Unknown packet");
	}
}

/* print a message announcing a packet, and the ip it came from */
void xdmcpdm_announce (int opcode)
{
	/* a string to store the name of the opcode */
	char *ocstring;

	/* get the string for the opcode */
	ocstring = xdmcpdm_opcode_string(opcode);

	/* print the announcement */
	printf("%s packet received from %s.\n", ocstring, client_ip);

	/* free the memory allocated for the string name */
	free(ocstring);
}

int main ()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	/* a socket to listen on */
	struct sockaddr_in server_sa = {0};

	/* start session ids at 1 */
	sess_id = 1;

	/* allocate enough memory to hold one pointer to a sess_db record */
	sess_db = xdmcpmalloc(sizeof(void *));
	if (WSAStartup(wVersionRequested, &wsaData) != 0) 
		{
			return 0;
		}

	/* try and open a socket */
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	if (fd < 0)
	{
		fprintf(stderr, "ERROR: Can't create socket!\n");
		exit(EXIT_FAILURE);
	}

	/* set up the socket to listen on port 177 (XDMCP) */
	server_sa.sin_family = AF_INET;
	server_sa.sin_port = htons((short)177);
	server_sa.sin_addr.s_addr = htonl(INADDR_ANY);	

	/* try and bind the socket to the port */
	if (bind(fd, (struct sockaddr*)&server_sa, sizeof(server_sa)) == -1)
	{
		fprintf(stderr, "ERROR: Can't bind to XDMCP socket!\n");
		WSACleanup ();
		exit(EXIT_FAILURE);
	}

	printf("Xdmcp: Waiting for XDMCP packets...\n");
	while (1)
	{
		/* length of buffa */
		int buffa_len;

		buffa_len = sizeof(buffa);
		
		/* read a packet from fd into buffa */
		if (	
			!XdmcpFill(
				fd,
				&buffa,
				(XdmcpNetaddr)&client_sa,
				&buffa_len
			)
		)
		{
			fprintf(stderr, "ERROR: XdmcpFill failed.\n");
			continue;
		}

		/* get client IP address string as dotted quad */
		client_ip = inet_ntoa(client_sa.sin_addr);
		
		/* read the XDMCP packet header from buffa */
		if (!XdmcpReadHeader(&buffa, &hedda))	
		{
			fprintf(stderr, "ERROR: Can't read XDMCP header.\n");
			continue;
		}

		/* check the incoming packet is using the same XDMCP version */
		if (hedda.version != XDM_PROTOCOL_VERSION)
		{
			fprintf(stderr, "ERROR: Incompatible XDMCP version.\n");
			continue;
		}

		/* print a message announcing the receipt of the packet */
		xdmcpdm_announce(hedda.opcode);

		/* respond the the packet, depending on the opcode */
		switch (hedda.opcode)
		{
			case BROADCAST_QUERY:
			case QUERY:	
				resp_query();
				break;
			case REQUEST:
				resp_request();
				break;
			case MANAGE:
				resp_manage();
				break;
		}
		printf("\n");
	}
	WSACleanup ();
	return(EXIT_SUCCESS);
}
