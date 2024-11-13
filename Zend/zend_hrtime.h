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

#ifndef ZEND_HRTIME_H
#define ZEND_HRTIME_H

#include "zend_portability.h"
#include "zend_types.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifndef PHP_WIN32
# include <time.h>
#endif

/* This file reuses code parts from the cross-platform timer library
	Public Domain - 2011 Mattias Jansson / Rampant Pixels */

#define ZEND_HRTIME_PLATFORM_POSIX   0
#define ZEND_HRTIME_PLATFORM_WINDOWS 0
#define ZEND_HRTIME_PLATFORM_APPLE   0
#define ZEND_HRTIME_PLATFORM_HPUX    0
#define ZEND_HRTIME_PLATFORM_AIX     0

#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS > 0) || defined(__OpenBSD__)) && defined(_POSIX_MONOTONIC_CLOCK) && defined(CLOCK_MONOTONIC)
# undef  ZEND_HRTIME_PLATFORM_POSIX
# define ZEND_HRTIME_PLATFORM_POSIX 1
#elif defined(_WIN32) || defined(_WIN64)
# undef  ZEND_HRTIME_PLATFORM_WINDOWS
# define ZEND_HRTIME_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
# undef  ZEND_HRTIME_PLATFORM_APPLE
# define ZEND_HRTIME_PLATFORM_APPLE 1
#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
# undef  ZEND_HRTIME_PLATFORM_HPUX
# define ZEND_HRTIME_PLATFORM_HPUX 1
#elif defined(_AIX)
# undef  ZEND_HRTIME_PLATFORM_AIX
# define ZEND_HRTIME_PLATFORM_AIX 1
#endif

#define ZEND_HRTIME_AVAILABLE (ZEND_HRTIME_PLATFORM_POSIX || ZEND_HRTIME_PLATFORM_WINDOWS || ZEND_HRTIME_PLATFORM_APPLE || ZEND_HRTIME_PLATFORM_HPUX || ZEND_HRTIME_PLATFORM_AIX)

BEGIN_EXTERN_C()

#if ZEND_HRTIME_PLATFORM_WINDOWS

ZEND_API extern double zend_hrtime_timer_scale;

#elif ZEND_HRTIME_PLATFORM_APPLE

# include <mach/mach_time.h>
# include <string.h>
ZEND_API extern mach_timebase_info_data_t zend_hrtime_timerlib_info;

#endif

#define ZEND_NANO_IN_SEC UINT64_C(1000000000)

typedef uint64_t zend_hrtime_t;

void zend_startup_hrtime(void);

static zend_always_inline zend_hrtime_t zend_hrtime(void)
{
#if ZEND_HRTIME_PLATFORM_WINDOWS
	LARGE_INTEGER lt = {0};
	QueryPerformanceCounter(&lt);
	return (zend_hrtime_t)((zend_hrtime_t)lt.QuadPart * zend_hrtime_timer_scale);
#elif ZEND_HRTIME_PLATFORM_APPLE
	return (zend_hrtime_t)mach_absolute_time() * zend_hrtime_timerlib_info.numer / zend_hrtime_timerlib_info.denom;
#elif ZEND_HRTIME_PLATFORM_POSIX
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	if (EXPECTED(0 == clock_gettime(CLOCK_MONOTONIC, &ts))) {
		return ((zend_hrtime_t) ts.tv_sec * (zend_hrtime_t)ZEND_NANO_IN_SEC) + ts.tv_nsec;
	}
	return 0;
#elif ZEND_HRTIME_PLATFORM_HPUX
	return (zend_hrtime_t) gethrtime();
#elif  ZEND_HRTIME_PLATFORM_AIX
	timebasestruct_t t;
	read_wall_time(&t, TIMEBASE_SZ);
	time_base_to_time(&t, TIMEBASE_SZ);
	return (zend_hrtime_t) t.tb_high * (zend_hrtime_t)NANO_IN_SEC + t.tb_low;
#else
	return 0;
#endif
}

END_EXTERN_C()

#endif /* ZEND_HRTIME_H */
