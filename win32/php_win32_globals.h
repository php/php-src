/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@php.net>                                    |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_WIN32_GLOBALS_H
#define PHP_WIN32_GLOBALS_H

/* misc globals for thread-safety under win32 */

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
