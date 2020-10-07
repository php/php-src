/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo             <shane@caraveo.com>               |
   |          Colin Viebrock            <colin@easydns.com>               |
   |          Hartmut Holzgraefe        <hholzgra@php.net>                |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_calendar.h"
#include "sdncal.h"
#include <time.h>

#define SECS_PER_DAY (24 * 3600)

/* {{{ proto int unixtojd([int timestamp])
   Convert UNIX timestamp to Julian Day */
PHP_FUNCTION(unixtojd)
{
	time_t ts = 0;
	zend_long tl = 0;
	struct tm *ta, tmbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &tl) == FAILURE) {
		return;
	}

	if (!tl) {
		ts = time(NULL);
	} else if (tl >= 0) {
		ts = (time_t) tl;
	} else {
		RETURN_FALSE;
	}

	if (!(ta = php_localtime_r(&ts, &tmbuf))) {
		RETURN_FALSE;
	}

	RETURN_LONG(GregorianToSdn(ta->tm_year+1900, ta->tm_mon+1, ta->tm_mday));
}
/* }}} */

/* {{{ proto int jdtounix(int jday)
   Convert Julian Day to UNIX timestamp */
PHP_FUNCTION(jdtounix)
{
	zend_long uday;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &uday) == FAILURE) {
		return;
	}
	uday -= 2440588 /* J.D. of 1.1.1970 */;

	if (uday < 0 || uday > ZEND_LONG_MAX / SECS_PER_DAY) { /* before beginning of unix epoch or greater than representable */
		RETURN_FALSE;
	}

	RETURN_LONG(uday * SECS_PER_DAY);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
