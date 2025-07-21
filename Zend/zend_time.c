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

#include "zend_time.h"

/* Current real/wall-time in seconds */
ZEND_API time_t zend_time_real_get(void) {
	return time(NULL);
}

ZEND_API void zend_time_real_spec(struct timespec *ts) {
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

ZEND_API uint64_t zend_time_mono_fallback(void) {
#if ZEND_HRTIME_AVAILABLE
	return (uint64_t)zend_hrtime();
#else
	struct timespec ts;
	zend_time_real_spec(&ts);
	return ((uint64_t) ts.tv_sec * ZEND_NANO_IN_SEC) + ts.tv_nsec;
#endif
}
