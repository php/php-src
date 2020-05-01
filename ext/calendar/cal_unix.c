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
   | Authors: Shane Caraveo             <shane@caraveo.com>               |
   |          Colin Viebrock            <colin@easydns.com>               |
   |          Hartmut Holzgraefe        <hholzgra@php.net>                |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_calendar.h"
#include "sdncal.h"
#include <time.h>

/* {{{ proto int unixtojd([int timestamp])
   Convert UNIX timestamp to Julian Day */
PHP_FUNCTION(unixtojd)
{
	time_t ts;
	zend_bool ts_is_null = 1;
	struct tm *ta, tmbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &ts, &ts_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (ts_is_null) {
		ts = time(NULL);
	} else if (ts < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
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
		RETURN_THROWS();
	}
	uday -= 2440588 /* J.D. of 1.1.1970 */;

	if (uday < 0 || uday > 24755) {
		zend_value_error("jday must be within the Unix epoch");
		RETURN_THROWS();
	}

	RETURN_LONG(uday * 24 * 3600);
}
/* }}} */
