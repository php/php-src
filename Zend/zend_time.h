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
   | Author: Marc Bennewitz <marc@mabe.berlin>                            |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_TIME_H
#define ZEND_TIME_H

#include "zend_portability.h"

#ifdef PHP_WIN32
# include "win32/time.h"
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <time.h>

#include "zend_hrtime.h"

#ifndef PHP_WIN32
# define tv_sec_t time_t
# define tv_usec_t suseconds_t
#else
# define tv_sec_t long
# define tv_usec_t long
#endif

#define ZEND_MILLI_IN_SEC 1000U
#define ZEND_MICRO_IN_SEC 1000000U

BEGIN_EXTERN_C()

/* Assign seconds to timeval */
static zend_always_inline void zend_time_sec2val(time_t s, struct timeval *tv) {
	tv->tv_sec  = (tv_sec_t) s;
	tv->tv_usec = 0;
}

/* Assign microseconds to timeval */
static zend_always_inline void zend_time_usec2val(int64_t usec, struct timeval *tv) {
	tv->tv_sec  = (tv_sec_t) (usec / ZEND_MICRO_IN_SEC);
	tv->tv_usec = (tv_usec_t) (usec % ZEND_MICRO_IN_SEC);

	if (UNEXPECTED(tv->tv_usec < 0)) {
		tv->tv_usec += ZEND_MICRO_IN_SEC;
		tv->tv_sec  -= 1;
	}
}

/* Assign double (seconds) to timeval */
static zend_always_inline void zend_time_dbl2val(double s, struct timeval *tv) {
	tv->tv_sec  = (tv_sec_t) s;
	tv->tv_usec = (tv_usec_t) ((s - tv->tv_sec) * ZEND_MICRO_IN_SEC);

	if (UNEXPECTED(tv->tv_usec < 0)) {
		tv->tv_usec += ZEND_MICRO_IN_SEC;
		tv->tv_sec  -= 1;
	} else if (UNEXPECTED(tv->tv_usec >= ZEND_MICRO_IN_SEC)) {
		// rare, but protects against rounding up to exactly 1 second
		tv->tv_usec -= ZEND_MICRO_IN_SEC;
		tv->tv_sec  += 1;
	}
}

/* Assign timeval to timespec */
static zend_always_inline void zend_time_val2spec(struct timeval tv, struct timespec *ts) {
	ts->tv_sec  = (time_t) tv.tv_sec;
	ts->tv_nsec = (long) (tv.tv_usec * 1000);
}

/* Get current timestamp in seconds */
static zend_always_inline time_t zend_time_real_get(void) {
	return time(NULL);
}

/* wrapper around clock_gettime/timestamp_get/gettimeofday/time */
static zend_always_inline void zend_time_real_spec(struct timespec *ts) {
	#if defined(HAVE_CLOCK_GETTIME)

		(void) clock_gettime(CLOCK_REALTIME, ts);

	#elif defined(HAVE_TIMESPEC_GET)

		(void) timespec_get(ts, TIME_UTC);

	#elif defined(HAVE_GETTIMEOFDAY)

		struct timeval tv;
		(void) gettimeofday(&tv, NULL);
		zend_time_val2spec(tv, ts);

	#else

		ts->tv_sec  = zend_time_real_get();
		ts->tv_nsec = 0;

	#endif
}

/* Monotonic time in nanoseconds with a fallback to real/wall-time 
   if no monotonic timer is available */
static zend_always_inline uint64_t zend_time_mono_fallback(void) {
	#if ZEND_HRTIME_AVAILABLE
		return (uint64_t)zend_hrtime();
	#else
		struct timespec ts;
		zend_time_real_spec(&ts);
		return ((uint64_t) ts.tv_sec * ZEND_NANO_IN_SEC) + ts.tv_nsec;
	#endif
}

END_EXTERN_C()

#endif // ZEND_TIME_H
