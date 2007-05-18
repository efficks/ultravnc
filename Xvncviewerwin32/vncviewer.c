//  Copyright (C) 2002 Ultr@Vnc Team Members. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
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

#include "vncviewer.h"
#include "arguments.h"

int
main(int argc, char **argv)
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);

	fd_set fds;
    struct timeval tv, *tvp;
    int msWait;
    
    processArgs(argc, argv);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) 
		{
			return 0;
		}

    {
	if (!ConnectToRFBServer(hostname, port)) exit(1);
    }

    if (!InitialiseRFBConnection(rfbsock)) exit(1);

    if (!CreateXWindow()) exit(1);

    if (!SetFormatAndEncodings()) {
	ShutdownX();
	WSACleanup ();
	exit(1);
    }

    if (!SendFramebufferUpdateRequest(updateRequestX, updateRequestY,
				      updateRequestW, updateRequestH, False)) {
	ShutdownX();
	WSACleanup ();
	exit(1);
    }

    while (True) {
	/*
	 * Always handle all X events before doing select.  This is the
	 * simplest way of ensuring that we don't block in select while
	 * Xlib has some events on its queue.
	 */

	if (!HandleXEvents()) {
	    ShutdownX();
		WSACleanup ();
	    exit(1);
	}

	tvp = NULL;

	if (sendUpdateRequest) {
	    gettimeofday(&tv);

	    msWait = (updateRequestPeriodms +
		      ((updateRequestTime.tv_sec - tv.tv_sec) * 1000) +
		      ((updateRequestTime.tv_usec - tv.tv_usec) / 1000));

	    if (msWait > 0) {
		tv.tv_sec = msWait / 1000;
		tv.tv_usec = (msWait % 1000) * 1000;

		tvp = &tv;
	    } else {
		if (!SendIncrementalFramebufferUpdateRequest()) {
		    ShutdownX();
			WSACleanup ();
		    exit(1);
		}
	    }
	}

	FD_ZERO(&fds);
	FD_SET(ConnectionNumber(dpy),&fds);
	FD_SET(rfbsock,&fds);

	if (select(FD_SETSIZE, &fds, NULL, NULL, tvp) < 0) {
	    perror("select");
	    ShutdownX();
		WSACleanup ();
	    exit(1);
	}

	if (FD_ISSET(rfbsock, &fds)) {
	    if (!HandleRFBServerMessage()) {
		ShutdownX();
		WSACleanup ();
		exit(1);
	    }
	}
    }
	WSACleanup ();
    return 0;
}
