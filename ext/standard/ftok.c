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
   | Author: Andrew Sitnikov <sitnikov@infonet.ee>                        |
   +----------------------------------------------------------------------+
*/

#include "php.h"

#include <sys/types.h>

#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

#ifdef PHP_WIN32
#include "win32/ipc.h"
#endif

#if HAVE_FTOK
/* {{{ Convert a pathname and a project identifier to a System V IPC key */
PHP_FUNCTION(ftok)
{
	char *pathname, *proj;
	size_t pathname_len, proj_len;
	key_t k;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_PATH(pathname, pathname_len)
		Z_PARAM_STRING(proj, proj_len)
	ZEND_PARSE_PARAMETERS_END();

	if (pathname_len == 0){
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (proj_len != 1){
		zend_argument_value_error(2, "must be a single character");
		RETURN_THROWS();
	}

	if (php_check_open_basedir(pathname)) {
		RETURN_LONG(-1);
	}

	k = ftok(pathname, proj[0]);
	if (k == -1) {
		php_error_docref(NULL, E_WARNING, "ftok() failed - %s", strerror(errno));
	}

	RETURN_LONG(k);
}
/* }}} */
#endif
