/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@guardian.no>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#ifdef PHP_WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include "php_lcg.h"
#include "uniqid.h"

#define MORE_ENTROPY (argc == 2 && Z_LVAL_PP(flags))

/* {{{ proto string uniqid(string prefix, [bool more_entropy])
   Generate a unique id */
PHP_FUNCTION(uniqid)
{
#ifdef HAVE_GETTIMEOFDAY
	pval **prefix, **flags;
	char uniqid[138];
	int sec, usec, argc;
	struct timeval tv;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &prefix, &flags)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(prefix);
	if (argc == 2) {
		convert_to_boolean_ex(flags);
	}

	/* Do some bounds checking since we are using a char array. */
	if (Z_STRLEN_PP(prefix) > 114) {
		php_error(E_WARNING, "The prefix to uniqid should not be more than 114 characters.");
		return;
	}
#if HAVE_USLEEP && !defined(PHP_WIN32)
	if (!MORE_ENTROPY) {
		usleep(1);
	}
#endif
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 1000000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs.
	 */
	if (MORE_ENTROPY) {
		sprintf(uniqid, "%s%08x%05x%.8f", Z_STRVAL_PP(prefix), sec, usec, php_combined_lcg(TSRMLS_C) * 10);
	} else {
		sprintf(uniqid, "%s%08x%05x", Z_STRVAL_PP(prefix), sec, usec);
	}

	RETURN_STRING(uniqid, 1);
#endif
}
/* }}} */

function_entry uniqid_functions[] = {
	PHP_FE(uniqid, NULL)
	{NULL, NULL, NULL}
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
