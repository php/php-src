/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

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

#ifdef HAVE_GETTIMEOFDAY
ZEND_TLS struct timeval prev_tv = { 0, 0 };

/* {{{ proto string uniqid([string prefix [, bool more_entropy]])
   Generates a unique ID */
PHP_FUNCTION(uniqid)
{
	char *prefix = "";
	zend_bool more_entropy = 0;
	zend_string *uniqid;
	int sec, usec;
	size_t prefix_len = 0;
	struct timeval tv;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(prefix, prefix_len)
		Z_PARAM_BOOL(more_entropy)
	ZEND_PARSE_PARAMETERS_END();

	/* This implementation needs current microsecond to change,
	 * hence we poll time until it does. This is much faster than
	 * calling usleep(1) which may cause the kernel to schedule
	 * another process, causing a pause of around 10ms.
	 */
	do {
		(void)gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	} while (tv.tv_sec == prev_tv.tv_sec && tv.tv_usec == prev_tv.tv_usec);

	prev_tv.tv_sec = tv.tv_sec;
	prev_tv.tv_usec = tv.tv_usec;

	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 0x100000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs.
	 */
	if (more_entropy) {
		uniqid = strpprintf(0, "%s%08x%05x%.8F", prefix, sec, usec, php_combined_lcg() * 10);
	} else {
		uniqid = strpprintf(0, "%s%08x%05x", prefix, sec, usec);
	}

	RETURN_STR(uniqid);
}
#endif
/* }}} */
