/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

/* $Id$ */

#include "php.h"

#include <sys/types.h>

#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

#if HAVE_FTOK
/* {{{ proto int ftok(string pathname, string proj)
   Convert a pathname and a project identifier to a System V IPC key */
PHP_FUNCTION(ftok)
{
	char *pathname, *proj;
	size_t pathname_len, proj_len;
	key_t k;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ps", &pathname, &pathname_len, &proj, &proj_len) == FAILURE) {
		return;
	}

	if (pathname_len == 0){
		php_error_docref(NULL, E_WARNING, "Pathname is invalid");
		RETURN_LONG(-1);
	}

	if (proj_len != 1){
		php_error_docref(NULL, E_WARNING, "Project identifier is invalid");
		RETURN_LONG(-1);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
