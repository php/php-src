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

#include "zend_time.h"

ZEND_API struct timespec zend_time_real_spec(void) {
   struct timespec ts;

#if defined(HAVE_CLOCK_GETTIME)

	(void) clock_gettime(CLOCK_REALTIME, &ts);

#elif defined(HAVE_TIMESPEC_GET)

	(void) timespec_get(&ts, TIME_UTC);

#elif defined(HAVE_GETTIMEOFDAY)

	struct timeval tv;
	(void) gettimeofday(&tv, NULL);
	ts = zend_time_val2spec(tv);

#else

	ts.tv_sec  = zend_time_real_get();
	ts.tv_nsec = 0;

#endif

   return ts;
}

ZEND_API uint64_t zend_time_mono_fallback_nsec(void) {
#if ZEND_HRTIME_AVAILABLE
	return (uint64_t)zend_hrtime();
#else
	struct timespec ts = zend_time_real_spec();
	return ((uint64_t) ts.tv_sec * ZEND_NANO_IN_SEC) + ts.tv_nsec;
#endif
}
