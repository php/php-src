/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
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
PHP_FUNCTION(posix_seteuid);
PHP_FUNCTION(posix_setegid);

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
