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

#include "php.h"
#include "php_win32_globals.h"
#include "syslog.h"
#include "signal.h"

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

	php_win32_signal_ctrl_handler_request_shutdown();

	return SUCCESS;
}/*}}}*/
