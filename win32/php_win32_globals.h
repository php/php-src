/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@php.net>                                    |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_WIN32_GLOBALS_H
#define PHP_WIN32_GLOBALS_H

/* misc globals for thread safety under win32 */

#include "win32/sendmail.h"

typedef struct _php_win32_core_globals php_win32_core_globals;

#ifdef ZTS
# define PW32G(v)		ZEND_TSRMG(php_win32_core_globals_id, php_win32_core_globals*, v)
extern PHPAPI int php_win32_core_globals_id;
#else
# define PW32G(v)		(the_php_win32_core_globals.v)
extern PHPAPI struct _php_win32_core_globals the_php_win32_core_globals;
#endif

struct _php_win32_core_globals {
	/* syslog */
	char *log_header;
	HANDLE log_source;

	HKEY       registry_key;
	HANDLE     registry_event;
	HashTable *registry_directories;

	char   mail_buffer[MAIL_BUFFER_SIZE];
	SOCKET mail_socket;
	char   mail_host[HOST_NAME_LEN];
	char   mail_local_host[HOST_NAME_LEN];
};

void php_win32_core_globals_ctor(void *vg);
void php_win32_core_globals_dtor(void *vg);
PHP_RSHUTDOWN_FUNCTION(win32_core_globals);

#endif
