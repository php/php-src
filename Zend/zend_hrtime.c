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

#include "zend.h"
#include "zend_hrtime.h"

/* This file reuses code parts from the cross-platform timer library
	Public Domain - 2011 Mattias Jansson / Rampant Pixels */

#if ZEND_HRTIME_PLATFORM_POSIX

# include <unistd.h>
# include <time.h>
# include <string.h>

#elif ZEND_HRTIME_PLATFORM_WINDOWS

# define WIN32_LEAN_AND_MEAN

double zend_hrtime_timer_scale = .0;

#elif ZEND_HRTIME_PLATFORM_APPLE

# include <mach/mach_time.h>
# include <string.h>
mach_timebase_info_data_t zend_hrtime_timerlib_info = {
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

#elif ZEND_HRTIME_PLATFORM_APPLE

	mach_timebase_info(&zend_hrtime_timerlib_info);

#endif
}
