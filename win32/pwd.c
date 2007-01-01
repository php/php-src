/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"				/*php specific */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <lmaccess.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lmapibuf.h>
#include "pwd.h"
#include "grp.h"
#include "php_win32_globals.h"

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
	char name[256];
	DWORD max_len = 256;
	TSRMLS_FETCH();

	STR_FREE(PW32G(login_name));	
	GetUserName(name, &max_len);
	name[max_len] = '\0';
	PW32G(login_name) = estrdup(name);
	return PW32G(login_name);
}

struct passwd *
getpwuid(int user_id)
{
	TSRMLS_FETCH();
	PW32G(pwd).pw_name = getlogin();
	PW32G(pwd).pw_dir = home_dir;
	PW32G(pwd).pw_shell = login_shell;
	PW32G(pwd).pw_uid = 0;

	return &PW32G(pwd);
}

