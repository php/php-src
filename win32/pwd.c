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

