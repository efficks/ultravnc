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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if 0
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
#endif

extern void fatal(char *fmt, ...);

void *xmalloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr == NULL)
    fatal("xmalloc: out of memory (allocating %d bytes)", (int)size);
  return ptr;
}

void *xrealloc(void *ptr, size_t new_size)
{
  void *new_ptr;

  if (ptr == NULL)
    fatal("xrealloc: NULL pointer given as argument");
  new_ptr = realloc(ptr, new_size);
  if (new_ptr == NULL)
   fatal("xrealloc: out of memory (new_size %d bytes)", (int)new_size);
  return new_ptr;
}

void xfree(void *ptr)
{
  if (ptr == NULL)
    fatal("xfree: NULL pointer given as argument");
  free(ptr);
}

char *xstrdup(const char *str)
{
  char *cp = xmalloc(strlen(str) + 1);
  strcpy(cp, str);
  return cp;
}
