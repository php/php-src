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
   | Author: Niklas Keller <kelunik@php.net>                              |
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
 */

#include "zend.h"
#include "zend_hrtime.h"

/* This file reuses code parts from the cross-platform timer library
	Public Domain - 2011 Mattias Jansson / Rampant Pixels */

#if ZEND_HRTIME_PLATFORM_POSIX

# include <unistd.h>
# include <time.h>
# include <string.h>

ZEND_API clockid_t zend_hrtime_posix_clock_id = CLOCK_MONOTONIC;

#elif ZEND_HRTIME_PLATFORM_WINDOWS

# define WIN32_LEAN_AND_MEAN

ZEND_API double zend_hrtime_timer_scale = .0;

#elif ZEND_HRTIME_PLATFORM_APPLE_MACH_ABSOLUTE

# include <mach/mach_time.h>
# include <string.h>
ZEND_API mach_timebase_info_data_t zend_hrtime_timerlib_info = {
	.numer = 0,
	.denom = 1,
};

#elif ZEND_HRTIME_PLATFORM_HPUX

# include <sys/time.h>

#elif ZEND_HRTIME_PLATFORM_AIX

# include <sys/time.h>
# include <sys/systemcfg.h>

#endif

void zend_startup_hrtime(void)
{
#if ZEND_HRTIME_PLATFORM_WINDOWS

	LARGE_INTEGER tf = {0};
	if (QueryPerformanceFrequency(&tf) || 0 != tf.QuadPart) {
		zend_hrtime_timer_scale = (double)ZEND_NANO_IN_SEC / (zend_hrtime_t)tf.QuadPart;
	}

#elif ZEND_HRTIME_PLATFORM_APPLE_MACH_ABSOLUTE

	mach_timebase_info(&zend_hrtime_timerlib_info);

#elif ZEND_HRTIME_PLATFORM_POSIX

	struct timespec ts;

#ifdef CLOCK_MONOTONIC_RAW
	if (EXPECTED(0 == clock_gettime(CLOCK_MONOTONIC_RAW, &ts))) {
		zend_hrtime_posix_clock_id = CLOCK_MONOTONIC_RAW;
		return;
	}
#endif

	if (EXPECTED(0 == clock_gettime(zend_hrtime_posix_clock_id, &ts))) {
		return;
	}

	// zend_error mechanism is not initialized at that point
	fprintf(stderr, "No working CLOCK_MONOTONIC* found, this should never happen\n");
	abort();

#endif
}
