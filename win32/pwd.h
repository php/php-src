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

#ifndef PWD_H
#define PWD_H

struct passwd {
	char *pw_name;		
	char *pw_passwd;		
	int pw_uid;	
	int pw_gid;	
	char *pw_comment;	
	char *pw_gecos;	
	char *pw_dir;
	char *pw_shell;	
};

extern struct passwd *getpwuid(int);
extern struct passwd *getpwnam(char *name);
extern char *getlogin(void);
#endif
