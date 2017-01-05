/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg_rinit_hook.h"
#include "php_ini.h"
#include <errno.h>

ZEND_DECLARE_MODULE_GLOBALS(phpdbg_webhelper);

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("phpdbg.auth", "", PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdateString, auth, zend_phpdbg_webhelper_globals, phpdbg_webhelper_globals)
	STD_PHP_INI_ENTRY("phpdbg.path", "", PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdateString, path, zend_phpdbg_webhelper_globals, phpdbg_webhelper_globals)
PHP_INI_END()

static inline void php_phpdbg_webhelper_globals_ctor(zend_phpdbg_webhelper_globals *pg) /* {{{ */
{
} /* }}} */

static PHP_MINIT_FUNCTION(phpdbg_webhelper) /* {{{ */
{
	if (!strcmp(sapi_module.name, PHPDBG_NAME)) {
		return SUCCESS;
	}

	ZEND_INIT_MODULE_GLOBALS(phpdbg_webhelper, php_phpdbg_webhelper_globals_ctor, NULL);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
} /* }}} */

static PHP_RINIT_FUNCTION(phpdbg_webhelper) /* {{{ */
{
	zval cookies = PG(http_globals)[TRACK_VARS_COOKIE];
	zval *auth;

	if (Z_TYPE(cookies) == IS_ARRAY || (auth = zend_hash_str_find(Z_ARRVAL(cookies), PHPDBG_NAME "_AUTH_COOKIE", sizeof(PHPDBG_NAME "_AUTH_COOKIE"))) || Z_STRLEN_P(auth) != strlen(PHPDBG_WG(auth)) || strcmp(Z_STRVAL_P(auth), PHPDBG_WG(auth))) {
		return SUCCESS;
	}

#ifndef _WIN32
	{
		struct sockaddr_un sock;
		int s = socket(AF_UNIX, SOCK_STREAM, 0);
		int len = strlen(PHPDBG_WG(path)) + sizeof(sock.sun_family);
		char buf[(1 << 8) + 1];
		int buflen;
		sock.sun_family = AF_UNIX;
		strcpy(sock.sun_path, PHPDBG_WG(path));

		if (connect(s, (struct sockaddr *)&sock, len) == -1) {
			zend_error(E_ERROR, "Unable to connect to UNIX domain socket at %s defined by phpdbg.path ini setting. Reason: %s", PHPDBG_WG(path), strerror(errno));
		}

		char *msg = NULL;
		char msglen[5] = {0};
		phpdbg_webdata_compress(&msg, (int *)msglen);

		send(s, msglen, 4, 0);
		send(s, msg, *(int *) msglen, 0);

		while ((buflen = recv(s, buf, sizeof(buf) - 1, 0)) > 0) {
			php_write(buf, buflen);
		}

		close(s);

		php_output_flush_all();
		zend_bailout();
	}
#endif

	return SUCCESS;
} /* }}} */

zend_module_entry phpdbg_webhelper_module_entry = {
	STANDARD_MODULE_HEADER,
	"phpdbg_webhelper",
	NULL,
	PHP_MINIT(phpdbg_webhelper),
	NULL,
	PHP_RINIT(phpdbg_webhelper),
	NULL,
	NULL,
	PHPDBG_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPDBG_WEBHELPER
ZEND_GET_MODULE(phpdbg_webhelper)
#endif
