/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Kristian Koehntopp (kris@koehntopp.de)                      |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_POSIX_H
#define PHP_POSIX_H

#if HAVE_POSIX
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry posix_module_entry;
#define posix_module_ptr &posix_module_entry

PHP_FUNCTION(posix_kill);

PHP_FUNCTION(posix_getpid);
PHP_FUNCTION(posix_getppid);

PHP_FUNCTION(posix_getuid);
PHP_FUNCTION(posix_getgid);
PHP_FUNCTION(posix_geteuid);
PHP_FUNCTION(posix_getegid);
PHP_FUNCTION(posix_setuid);
PHP_FUNCTION(posix_setgid);
PHP_FUNCTION(posix_getgroups);
PHP_FUNCTION(posix_getlogin);

PHP_FUNCTION(posix_getpgrp);
PHP_FUNCTION(posix_setsid);
PHP_FUNCTION(posix_setpgid);
PHP_FUNCTION(posix_getpgid);
PHP_FUNCTION(posix_getsid);

PHP_FUNCTION(posix_uname);
PHP_FUNCTION(posix_times);

PHP_FUNCTION(posix_ctermid);
PHP_FUNCTION(posix_ttyname);
PHP_FUNCTION(posix_isatty);

PHP_FUNCTION(posix_getcwd);

PHP_FUNCTION(posix_mkfifo);
PHP_FUNCTION(posix_getgrnam);
PHP_FUNCTION(posix_getgrgid);
PHP_FUNCTION(posix_getpwnam);
PHP_FUNCTION(posix_getpwuid);

PHP_FUNCTION(posix_getrlimit);

typedef struct {
	int dummy;
} posix_module;

#else

#define posix_module_ptr NULL

#endif

#define phpext_posix_ptr posix_module_ptr

#endif /* PHP_POSIX_H */
