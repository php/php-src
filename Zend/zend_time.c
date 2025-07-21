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

ZEND_API time_t zend_realtime_get(time_t *sec, long *nsec) {
	if (!nsec) {
		return time(sec);
	}

#if defined(HAVE_CLOCK_GETTIME)

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	if (sec) *sec  = ts.tv_sec;
	*nsec = ts.tv_nsec;
	return ts.tv_sec;

#elif defined(HAVE_TIMESPEC_GET)

	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	if (sec) *sec  = ts.tv_sec;
	*nsec = ts.tv_nsec;
	return ts.tv_sec;

#elif defined(HAVE_GETTIMEOFDAY)

	struct timeval tv;
	gettimeofday(&tv, NULL);

	if (sec) *sec  = tv.tv_sec;
	*nsec = tv.tv_usec * 1000;
	return tv.tv_sec;

#else

	*nsec = 0;
	return time(sec);

#endif
}

ZEND_API void zend_realtime_spec(struct timespec *ts) {
#if defined(HAVE_CLOCK_GETTIME)

	clock_gettime(CLOCK_REALTIME, ts);

#elif defined(HAVE_TIMESPEC_GET)

	timespec_get(ts, TIME_UTC);

#elif defined(HAVE_GETTIMEOFDAY)

	struct timeval tv;
	gettimeofday(&tv, NULL);
	zend_time_val2spec(tv, ts);

#else

	ts->tv_sec  = time(NULL);
	ts->tv_nsec = 0;

#endif
}
