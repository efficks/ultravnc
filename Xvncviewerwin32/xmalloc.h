//  Copyright (C) 2002 Ultr@Vnc Team Members. All Rights Reserved.
//  Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
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


#ifndef XMALLOC_H
#define XMALLOC_H

/* Like malloc, but calls fatal() if out of memory. */
void *xmalloc(size_t size);

/* Like realloc, but calls fatal() if out of memory. */
void *xrealloc(void *ptr, size_t new_size);

/* Frees memory allocated using xmalloc or xrealloc. */
void xfree(void *ptr);

/* Allocates memory using xmalloc, and copies the string into that memory. */
char *xstrdup(const char *str);

#endif /* XMALLOC_H */
