/*  pwd.c - Try to approximate UN*X's getuser...() functions under MS-DOS.
   Copyright (C) 1990 by Thorsten Ohl, td12@ddagsi3.bitnet

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Header$
 */

/* This 'implementation' is conjectured from the use of this functions in
   the RCS and BASH distributions.  Of course these functions don't do too
   much useful things under MS-DOS, but using them avoids many "#ifdef
   MSDOS" in ported UN*X code ...  */

#include "php.h"				/*php specific */
/* Need to take care of all the commented stuff later for NetWare */
/*
#define WIN32_LEAN_AND_MEAN
#include <windows.h>  
#include <lmaccess.h>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <lmapibuf.h>*/
#include "pwd.h"
#include "grp.h"

#ifndef THREAD_SAFE
static struct passwd pw;		/* should we return a malloc()'d structure   */
#endif
static char *home_dir = ".";	/* we feel (no|every)where at home */
static char *login_shell = "not command.com!";

struct passwd *getpwnam(char *name)
{
	return (struct passwd *) 0;
}


char *getlogin()
{
/*
	static char name[256];
	DWORD dw = 256;
	GetUserName(name, &dw);
	return name;
*/
    return NULL;    /* For now */
}

struct passwd *
 getpwuid(int uid)
{
	pw.pw_name = getlogin();
	pw.pw_dir = home_dir;
	pw.pw_shell = login_shell;
	pw.pw_uid = 0;

	return &pw;
}

/* Implementation for now */
int getpid()
{
    return -1;
}

/*
 * Local Variables:
 * mode:C
 * ChangeLog:ChangeLog
 * compile-command:make
 * End:
 */
