/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
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
#if MSVC5
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include "../lcg/php_lcg.h"
#include "uniqid.h"

/* {{{ proto string uniqid(string prefix)
   Generate a unique id */
PHP_FUNCTION(uniqid)
{
#ifdef HAVE_GETTIMEOFDAY
	pval *prefix;
	char uniqid[138];
	int sec, usec;
	struct timeval tv;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&prefix)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(prefix);

	/* Do some bounds checking since we are using a char array. */
	if (strlen(prefix->value.str.val) > 114) {
		php_error(E_WARNING, "The prefix to uniqid should not be more than 114 characters.");
		return;
	}
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 1000000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs:
	 */
	sprintf(uniqid, "%s%08x%05x%.8f", prefix->value.str.val, sec, usec, php_combined_lcg() * 10);

	RETURN_STRING(uniqid,1);
#endif
}
/* }}} */

function_entry uniqid_functions[] = {
	{"uniqid",		php3_uniqid,		NULL},
	{NULL, NULL, NULL}
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
