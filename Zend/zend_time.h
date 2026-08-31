/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Marc Bennewitz <marc@mabe.berlin>                           |
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

#define ZEND_TIME_MILLI_IN_SEC 1000U
#define ZEND_TIME_MICRO_IN_SEC 1000000U

BEGIN_EXTERN_C()

/* Assign seconds to timeval */
static zend_always_inline struct timeval zend_time_sec2val(time_t s) {
	struct timeval tv;

	tv.tv_sec = (tv_sec_t) s;
	tv.tv_usec = 0;

	return tv;
}

/* Assign microseconds to timeval */
static zend_always_inline struct timeval zend_time_usec2val(int64_t usec) {
	struct timeval tv;

	tv.tv_sec = (tv_sec_t) (usec / ZEND_TIME_MICRO_IN_SEC);
	tv.tv_usec = (tv_usec_t) (usec % ZEND_TIME_MICRO_IN_SEC);

	if (UNEXPECTED(tv.tv_usec < 0)) {
		tv.tv_usec += ZEND_TIME_MICRO_IN_SEC;
		tv.tv_sec  -= 1;
	}

	return tv;
}

/* Assign double (seconds) to timeval */
static zend_always_inline struct timeval zend_time_dbl2val(double s) {
	struct timeval tv;

	tv.tv_sec  = (tv_sec_t) s;
	tv.tv_usec = (tv_usec_t) ((s - tv.tv_sec) * ZEND_TIME_MICRO_IN_SEC);

	if (UNEXPECTED(tv.tv_usec < 0)) {
		tv.tv_usec += ZEND_TIME_MICRO_IN_SEC;
		tv.tv_sec  -= 1;
	} else if (UNEXPECTED(tv.tv_usec >= ZEND_TIME_MICRO_IN_SEC)) {
		// rare, but protects against rounding up to exactly 1 second
		tv.tv_usec -= ZEND_TIME_MICRO_IN_SEC;
		tv.tv_sec  += 1;
	}

	return tv;
}

/* Assign timeval to timespec */
static zend_always_inline struct timespec zend_time_val2spec(struct timeval tv) {
	struct timespec ts;

	ts.tv_sec  = (time_t) tv.tv_sec;
	ts.tv_nsec = (long) (tv.tv_usec * 1000);

	return ts;
}

/* Current real/wall-time in seconds */
static zend_always_inline time_t zend_time_real_sec(void) {
	return time(NULL);
}

/* Current real/wall-time in up-to nano seconds */
ZEND_API struct timespec zend_time_real_spec(void);

/* Monotonic time in nanoseconds with a fallback to real/wall-time 
   if no monotonic timer is available */
ZEND_API uint64_t zend_time_mono_fallback_nsec(void);

END_EXTERN_C()

#endif // ZEND_TIME_H
