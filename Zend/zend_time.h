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

/* Helper macro to assign seconds to timeval */
#define zend_time_sec2val(s, tv) \
	(tv).tv_sec  = (tv_sec_t) (s); \
	(tv).tv_usec = 0;

/* Helper macro to assign microseconds to timeval */
#define zend_time_usec2val(usec, tv) \
	(tv).tv_sec  = (tv_sec_t) ((usec) / ZEND_MICRO_IN_SEC); \
	(tv).tv_usec = (tv_usec_t) ((usec) % ZEND_MICRO_IN_SEC);

/* Helper macro to assign double (seconds) to timeval */
#define zend_time_dbl2val(d, tv) \
	(tv).tv_sec  = (tv_sec_t) (d); \
	(tv).tv_usec = (tv_usec_t) (((d) - (tv).tv_sec) * ZEND_MICRO_IN_SEC);

/* Helper macro to copy timeval to timespec */
#define zend_time_val2spec(tv, ts) \
	(ts).tv_sec  = (time_t) (tv).tv_sec; \
	(ts).tv_nsec = (long) ((tv).tv_usec * 1000);

BEGIN_EXTERN_C()

/* Like time() but with up to nanosecond */
ZEND_API time_t zend_realtime_get(time_t *sec, long *nsec);

/* wrapper around clock_gettime/timestamp_get/gettimeofday/time */
ZEND_API void zend_realtime_spec(struct timespec *ts);

/* Monotonic time in nanoseconds with a fallback to real/wall-time 
   if no monotonic timer is available */
#if ZEND_HRTIME_AVAILABLE
# define zend_monotime_fallback() (uint64_t)zend_hrtime()
#else
ZEND_API zend_always_inline uint64_t zend_monotime_fallback(void) {
   struct timespec ts;
   zend_realtime_spec(&ts);
   return ((uint64_t) ts.tv_sec * ZEND_NANO_IN_SEC) + ts.tv_nsec;
}
#endif

END_EXTERN_C()

#endif // ZEND_TIME_H
