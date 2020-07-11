/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include "php_win32_globals.h"
#include "syslog.h"

#ifdef ZTS
PHPAPI int php_win32_core_globals_id;
#else
php_win32_core_globals the_php_win32_core_globals;
#endif

void php_win32_core_globals_ctor(void *vg)
{/*{{{*/
	php_win32_core_globals *wg = (php_win32_core_globals*)vg;
	memset(wg, 0, sizeof(*wg));

	wg->mail_socket = INVALID_SOCKET;

	wg->log_source = INVALID_HANDLE_VALUE;
}/*}}}*/

void php_win32_core_globals_dtor(void *vg)
{/*{{{*/
	php_win32_core_globals *wg = (php_win32_core_globals*)vg;

	if (wg->registry_key) {
		RegCloseKey(wg->registry_key);
		wg->registry_key = NULL;
	}
	if (wg->registry_event) {
		CloseHandle(wg->registry_event);
		wg->registry_event = NULL;
	}
	if (wg->registry_directories) {
		zend_hash_destroy(wg->registry_directories);
		free(wg->registry_directories);
		wg->registry_directories = NULL;
	}

	if (INVALID_SOCKET != wg->mail_socket) {
		closesocket(wg->mail_socket);
		wg->mail_socket = INVALID_SOCKET;
	}
}/*}}}*/


PHP_RSHUTDOWN_FUNCTION(win32_core_globals)
{/*{{{*/
	closelog();

	return SUCCESS;
}/*}}}*/
