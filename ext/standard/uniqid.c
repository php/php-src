/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "zend_time.h"

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>

#include "ext/random/php_random.h"
#include "ext/random/php_random_csprng.h"

ZEND_TLS time_t prev_tv_sec  = 0;
ZEND_TLS long   prev_tv_usec = 0;

/* {{{ Generates a unique ID */
PHP_FUNCTION(uniqid)
{
	char *prefix = "";
	bool more_entropy = 0;
	zend_string *uniqid;
	size_t prefix_len = 0;
	struct timespec ts;
	long tv_usec;
	int isec, iusec;

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
		zend_time_real_spec(&ts);
		tv_usec = ts.tv_nsec / 1000;
	} while (ts.tv_sec == prev_tv_sec && tv_usec == prev_tv_usec);

	prev_tv_sec  = ts.tv_sec;
	prev_tv_usec = tv_usec;

	isec  = (int) ts.tv_sec;
	iusec = (int) (tv_usec % 0x100000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs.
	 */
	if (more_entropy) {
		uint32_t bytes;
		double seed;
		if (php_random_bytes_silent(&bytes, sizeof(uint32_t)) == FAILURE) {
			bytes = php_random_generate_fallback_seed();
		}
		seed = ((double) bytes / UINT32_MAX) * 10.0;
		uniqid = strpprintf(0, "%s%08x%05x%.8F", prefix, isec, iusec, seed);
	} else {
		uniqid = strpprintf(0, "%s%08x%05x", prefix, isec, iusec);
	}

	RETURN_STR(uniqid);
}
/* }}} */
