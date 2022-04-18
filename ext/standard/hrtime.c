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
#include "hrtime.h"

/* {{{ */
/* This file reuses code parts from the cross-platform timer library
	Public Domain - 2011 Mattias Jansson / Rampant Pixels */

#if PHP_HRTIME_PLATFORM_POSIX

# include <unistd.h>
# include <time.h>
# include <string.h>

#elif PHP_HRTIME_PLATFORM_WINDOWS

# define WIN32_LEAN_AND_MEAN

static double _timer_scale = .0;

#elif PHP_HRTIME_PLATFORM_APPLE

# include <mach/mach_time.h>
# include <string.h>
static mach_timebase_info_data_t _timerlib_info;

#elif PHP_HRTIME_PLATFORM_HPUX

# include <sys/time.h>

#elif PHP_HRTIME_PLATFORM_AIX

# include <sys/time.h>
# include <sys/systemcfg.h>

#endif

#define NANO_IN_SEC 1000000000
/* }}} */

static int _timer_init(void)
{/*{{{*/
#if PHP_HRTIME_PLATFORM_WINDOWS

	LARGE_INTEGER tf = {0};
	if (!QueryPerformanceFrequency(&tf) || 0 == tf.QuadPart) {
		return -1;
	}
	_timer_scale = (double)NANO_IN_SEC / (php_hrtime_t)tf.QuadPart;

#elif PHP_HRTIME_PLATFORM_APPLE

	if (mach_timebase_info(&_timerlib_info)) {
		return -1;
	}

#elif PHP_HRTIME_PLATFORM_POSIX

#if !_POSIX_MONOTONIC_CLOCK
#ifdef _SC_MONOTONIC_CLOCK
	if (0 >= sysconf(_SC_MONOTONIC_CLOCK)) {
		return -1;
	}
#endif
#endif

#elif PHP_HRTIME_PLATFORM_HPUX

	/* pass */

#elif PHP_HRTIME_PLATFORM_AIX

	/* pass */

#else
	/* Timer unavailable. */
	return -1;
#endif

	return 0;
}/*}}}*/

/* {{{ */
PHP_MINIT_FUNCTION(hrtime)
{
	if (0 > _timer_init()) {
		php_error_docref(NULL, E_WARNING, "Failed to initialize high-resolution timer");
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static zend_always_inline php_hrtime_t _timer_current(void)
{/*{{{*/
#if PHP_HRTIME_PLATFORM_WINDOWS
	LARGE_INTEGER lt = {0};
	QueryPerformanceCounter(&lt);
	return (php_hrtime_t)((php_hrtime_t)lt.QuadPart * _timer_scale);
#elif PHP_HRTIME_PLATFORM_APPLE
	return (php_hrtime_t)mach_absolute_time() * _timerlib_info.numer / _timerlib_info.denom;
#elif PHP_HRTIME_PLATFORM_POSIX
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	if (0 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		return ((php_hrtime_t) ts.tv_sec * (php_hrtime_t)NANO_IN_SEC) + ts.tv_nsec;
	}
	return 0;
#elif PHP_HRTIME_PLATFORM_HPUX
	return (php_hrtime_t) gethrtime();
#elif  PHP_HRTIME_PLATFORM_AIX
	timebasestruct_t t;
	read_wall_time(&t, TIMEBASE_SZ);
	time_base_to_time(&t, TIMEBASE_SZ);
	return (php_hrtime_t) t.tb_high * (php_hrtime_t)NANO_IN_SEC + t.tb_low;
#else
	return 0;
#endif
}/*}}}*/

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
#if HRTIME_AVAILABLE
	bool get_as_num = 0;
	php_hrtime_t t = _timer_current();

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(get_as_num)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(get_as_num)) {
		PHP_RETURN_HRTIME(t);
	} else {
		array_init_size(return_value, 2);
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		add_next_index_long(return_value, (zend_long)(t / (php_hrtime_t)NANO_IN_SEC));
		add_next_index_long(return_value, (zend_long)(t % (php_hrtime_t)NANO_IN_SEC));
	}
#else
	RETURN_FALSE;
#endif
}
/* }}} */

PHPAPI php_hrtime_t php_hrtime_current(void)
{/*{{{*/
	return _timer_current();
}/*}}}*/
