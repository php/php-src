/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include "pageinfo.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_PWD_H
#if MSVC5
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#if MSVC5
#include <process.h>
#endif

#ifndef THREAD_SAFE
static long page_uid   = -1;
static long page_inode = -1;
static long page_mtime = -1;
#endif

static void _php3_statpage(void)
{
#if !APACHE
	char *path;
	struct stat sb;
#else
	request_rec *r;
	SLS_FETCH();
	
	r = ((request_rec *) SG(server_context));
#endif
	
#if APACHE
	/* Apache has already gone through the trouble of doing
	   the stat(), so the only real overhead is copying three
	   values. We can afford it, and it means we don't have to
	   worry about resetting the static variables after every
	   hit. */
	page_uid   = r ->finfo.st_uid;
	page_inode = r->finfo.st_ino;
	page_mtime = r->finfo.st_mtime;
#else
	if (page_uid == -1) {
		path = SG(request_info).path_translated;
		if (path != NULL) {
			if (stat(path, &sb) == -1) {
				php3_error(E_WARNING, "Unable to find file:  '%s'", path);
				return;
			}
			page_uid   = sb.st_uid;
			page_inode = sb.st_ino;
			page_mtime = sb.st_mtime;
		}
	}
#endif
}

long _php3_getuid(void)
{
	_php3_statpage();
	return (page_uid);
}

/* {{{ proto int getmyuid(void)
   Get PHP script owner's UID */
PHP_FUNCTION(getmyuid)
{
	long uid;
	
	uid = _php3_getuid();
	if (uid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(uid);
	}
}
/* }}} */

/* {{{ proto int getmypid(void)
   Get current process ID */
PHP_FUNCTION(getmypid)
{
	int pid;
	
	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG((long) pid);
	}
}
/* }}} */

/* {{{ proto int getmyinode(void)
   Get the inode of the current script being parsed */
PHP_FUNCTION(getmyinode)
{
	_php3_statpage();
	if (page_inode < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(page_inode);
	}
}
/* }}} */

/* {{{ proto int getlastmod(void)
   Get time of last page modification */
PHP_FUNCTION(getlastmod)
{
	_php3_statpage();
	if (page_mtime < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(page_mtime);
	}
}
/* }}} */
