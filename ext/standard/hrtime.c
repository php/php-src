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
   | Author: Niklas Keller <kelunik@php.net>                              |
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "zend_hrtime.h"

#ifdef ZEND_ENABLE_ZVAL_LONG64
#define PHP_RETURN_HRTIME(t) RETURN_LONG((zend_long)t)
#else
#ifdef _WIN32
# define HRTIME_U64A(i, s, len) _ui64toa_s(i, s, len, 10)
#else
# define HRTIME_U64A(i, s, len) \
	do { \
		int st = snprintf(s, len, "%llu", i); \
		s[st] = '\0'; \
	} while (0)
#endif
#define PHP_RETURN_HRTIME(t) do { \
	char _a[ZEND_LTOA_BUF_LEN]; \
	double _d; \
	HRTIME_U64A(t, _a, ZEND_LTOA_BUF_LEN); \
	_d = zend_strtod(_a, NULL); \
	RETURN_DOUBLE(_d); \
	} while (0)
#endif

/* {{{ Returns an array of integers in form [seconds, nanoseconds] counted
	from an arbitrary point in time. If an optional boolean argument is
	passed, returns an integer on 64-bit platforms or float on 32-bit
	containing the current high-resolution time in nanoseconds. The
	delivered timestamp is monotonic and cannot be adjusted. */
PHP_FUNCTION(hrtime)
{
#if ZEND_HRTIME_AVAILABLE
	bool get_as_num = 0;
	zend_hrtime_t t = zend_hrtime();

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(get_as_num)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(get_as_num)) {
		PHP_RETURN_HRTIME(t);
	} else {
		array_init_size(return_value, 2);
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		add_next_index_long(return_value, (zend_long)(t / (zend_hrtime_t)ZEND_NANO_IN_SEC));
		add_next_index_long(return_value, (zend_long)(t % (zend_hrtime_t)ZEND_NANO_IN_SEC));
	}
#else
	RETURN_FALSE;
#endif
}
/* }}} */
