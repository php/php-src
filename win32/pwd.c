/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"				/*php specific */
#include <lmaccess.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lmapibuf.h>
#include "pwd.h"
#include "grp.h"

#ifndef THREAD_SAFE
static struct passwd pwd;
#endif

static char *home_dir = ".";
static char *login_shell = "not command.com!";

struct passwd *
getpwnam(char *name)
{
	return (struct passwd *) 0;
}


char *
getlogin()
{
	static char name[256];
	DWORD max_len = 256;

	GetUserName(name, &max_len);
	return name;
}

struct passwd *
getpwuid(int user_id)
{
	pwd.pw_name = getlogin();
	pwd.pw_dir = home_dir;
	pwd.pw_shell = login_shell;
	pwd.pw_uid = 0;

	return &pwd;
}

