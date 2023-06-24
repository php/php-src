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

#ifndef ZEND_TIME_H
#define ZEND_TIME_H

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_TIME_H
# include <time.h>
#endif

#define ZEND_TIME_PLATFORM_POSIX   0
#define ZEND_TIME_PLATFORM_WINDOWS 0
#define ZEND_TIME_PLATFORM_APPLE   0
#define ZEND_TIME_PLATFORM_HPUX    0
#define ZEND_TIME_PLATFORM_AIX     0

#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS > 0) || defined(__OpenBSD__)) && defined(_POSIX_MONOTONIC_CLOCK) && defined(CLOCK_MONOTONIC)
# undef  ZEND_TIME_PLATFORM_POSIX
# define ZEND_TIME_PLATFORM_POSIX 1
#elif defined(_WIN32) || defined(_WIN64)
# undef  ZEND_TIME_PLATFORM_WINDOWS
# define ZEND_TIME_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
# undef  ZEND_TIME_PLATFORM_APPLE
# define ZEND_TIME_PLATFORM_APPLE 1
#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
# undef  ZEND_TIME_PLATFORM_HPUX
# define ZEND_TIME_PLATFORM_HPUX 1
#elif defined(_AIX)
# undef  ZEND_TIME_PLATFORM_AIX
# define ZEND_TIME_PLATFORM_AIX 1
#endif

#define ZEND_MONOTONIC_TIME_AVAILABLE (ZEND_TIME_PLATFORM_POSIX || ZEND_TIME_PLATFORM_WINDOWS || ZEND_TIME_PLATFORM_APPLE || ZEND_TIME_PLATFORM_HPUX || ZEND_TIME_PLATFORM_AIX)

#include "zend_portability.h"
#include "zend_types.h"

BEGIN_EXTERN_C()

#if ZEND_TIME_PLATFORM_WINDOWS

extern double zend_timer_scale = .0;

#elif ZEND_TIME_PLATFORM_APPLE

# include <mach/mach_time.h>
# include <string.h>
extern mach_timebase_info_data_t zend_timerlib_info;

#endif

#define ZEND_NANO_IN_SEC 1000000000

typedef uint64_t zend_time_t;

zend_result zend_startup_time(void);

static zend_always_inline zend_time_t zend_monotonic_time(void)
{
#if ZEND_TIME_PLATFORM_WINDOWS
	LARGE_INTEGER lt = {0};
	QueryPerformanceCounter(&lt);
	return (zend_time_t)((zend_time_t)lt.QuadPart * zend_timer_scale);
#elif ZEND_TIME_PLATFORM_APPLE
	return (zend_time_t)mach_absolute_time() * zend_timerlib_info.numer / zend_timerlib_info.denom;
#elif ZEND_TIME_PLATFORM_POSIX
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	if (0 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		return ((zend_time_t) ts.tv_sec * (zend_time_t)ZEND_NANO_IN_SEC) + ts.tv_nsec;
	}
	return 0;
#elif ZEND_TIME_PLATFORM_HPUX
	return (zend_time_t) gethrtime();
#elif  ZEND_TIME_PLATFORM_AIX
	timebasestruct_t t;
	read_wall_time(&t, TIMEBASE_SZ);
	time_base_to_time(&t, TIMEBASE_SZ);
	return (zend_time_t) t.tb_high * (zend_time_t)NANO_IN_SEC + t.tb_low;
#else
	return 0;
#endif
}

END_EXTERN_C()

#endif /* ZEND_TIME_H */
