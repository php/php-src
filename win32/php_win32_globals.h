/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@php.net>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_WIN32_GLOBALS_H
#define PHP_WIN32_GLOBALS_H

/* misc globals for thread-safety under win32 */

#include "pwd.h"

typedef struct _php_win32_core_globals php_win32_core_globals;

#ifdef ZTS
# define PW32G(v)		TSRMG(php_win32_core_globals_id, php_win32_core_globals*, v)
extern PHPAPI int php_win32_core_globals_id;
#else
# define PW32G(v)		(php_win32_core_globals.v)
extern PHPAPI struct _php_win32_core_globals php_win32_core_globals;
#endif

struct _php_win32_core_globals {
	/* syslog */
	char *log_header;
	HANDLE log_source;

	/* getpwuid */
	struct passwd pwd;

	/* getlogin */
	char *login_name;

	/* time */
	struct timeval starttime;
	__int64			lasttime, freq;
};

void php_win32_core_globals_ctor(void *vg TSRMLS_DC);
PHP_RSHUTDOWN_FUNCTION(win32_core_globals);

#endif

