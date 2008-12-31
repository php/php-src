/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
	zval **pathname, **proj;
	key_t k;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &pathname, &proj) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(pathname);
	convert_to_string_ex(proj);

	if (Z_STRLEN_PP(pathname)==0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Pathname is invalid");
		RETURN_LONG(-1);
	}

	if (Z_STRLEN_PP(proj)!=1){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Project identifier is invalid");
		RETURN_LONG(-1);
    }

	if ((PG(safe_mode) && (!php_checkuid(Z_STRVAL_PP(pathname), NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(Z_STRVAL_PP(pathname) TSRMLS_CC)) {
		RETURN_LONG(-1);
	}

	k = ftok(Z_STRVAL_PP(pathname),Z_STRVAL_PP(proj)[0]);
	if (k == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "ftok() failed - %s", strerror(errno));
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
