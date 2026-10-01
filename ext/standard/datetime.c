/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_globals.h"

#include <time.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef HAVE_STRPTIME
#ifndef HAVE_DECL_STRPTIME
char *strptime(const char *s, const char *format, struct tm *tm);
#endif

/* {{{ Parse a time/date generated with strftime() */
PHP_FUNCTION(strptime)
{
	char      *ts;
	size_t        ts_length;
	char      *format;
	size_t        format_length;
	struct tm  parsed_time;
	char      *unparsed_part;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(ts, ts_length)
		Z_PARAM_STRING(format, format_length)
	ZEND_PARSE_PARAMETERS_END();

	memset(&parsed_time, 0, sizeof(parsed_time));

	unparsed_part = strptime(ts, format, &parsed_time);
	if (unparsed_part == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_long(return_value, "tm_sec",   parsed_time.tm_sec);
	add_assoc_long(return_value, "tm_min",   parsed_time.tm_min);
	add_assoc_long(return_value, "tm_hour",  parsed_time.tm_hour);
	add_assoc_long(return_value, "tm_mday",  parsed_time.tm_mday);
	add_assoc_long(return_value, "tm_mon",   parsed_time.tm_mon);
	add_assoc_long(return_value, "tm_year",  parsed_time.tm_year);
	add_assoc_long(return_value, "tm_wday",  parsed_time.tm_wday);
	add_assoc_long(return_value, "tm_yday",  parsed_time.tm_yday);
	add_assoc_string(return_value, "unparsed", unparsed_part);
}
/* }}} */

#endif
