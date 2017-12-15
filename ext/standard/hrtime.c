/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Niklas Keller <kelunik@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "zend_exceptions.h"
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
# include <windows.h>

static uint64_t _timer_freq = 0;
static double _timer_int = .0;

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

static int _timer_init()
{/*{{{*/
#if PHP_HRTIME_PLATFORM_WINDOWS

	uint64_t unused;
	if (!QueryPerformanceFrequency((LARGE_INTEGER*) &_timer_freq) ||
		!QueryPerformanceCounter((LARGE_INTEGER*) &unused)) {
		return -1;
	}
	_timer_int = 1.0 / _timer_freq;

#elif PHP_HRTIME_PLATFORM_APPLE

	if (mach_timebase_info(&_timerlib_info)) {
		return -1;
	}

#elif PHP_HRTIME_PLATFORM_POSIX

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };

	if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
		return -1;
	}

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
	uint64_t cur;
	QueryPerformanceCounter((LARGE_INTEGER*) &cur);
	return (php_hrtime_t)((double)cur * _timer_int * (double)NANO_IN_SEC);
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

#if ZEND_ENABLE_ZVAL_LONG64
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

/* {{{ proto mixed hrtime([bool get_as_numu = false])
	Returns an array of integers in form [seconds, nanoseconds] counted
	from an arbitrary point in time. If an optional boolean argument is
	passed, returns an integer on 64-bit platforms or float on 32-bit
	containing the current high-resolution time in nanoseconds */
PHP_FUNCTION(hrtime)
{
#if HRTIME_AVAILABLE
	zend_bool get_as_num = 0;
	php_hrtime_t t = _timer_current();

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(get_as_num)
	ZEND_PARSE_PARAMETERS_END();

	if (!get_as_num) {
		/* TODO On 32-bit care about millenium bug. */
		array_init(return_value);
		add_next_index_long(return_value, (zend_long)(t / (php_hrtime_t)NANO_IN_SEC));
		add_next_index_long(return_value, (zend_long)(t % (php_hrtime_t)NANO_IN_SEC));
	} else {
		PHP_RETURN_HRTIME(t);
	}
#else
	RETURN_LONG(0);
#endif
}
/* }}} */

PHPAPI php_hrtime_t php_hrtime_current(void)
{/*{{{*/
	return _timer_current();
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

