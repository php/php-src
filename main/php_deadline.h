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
 */

#ifndef PHP_DEADLINE_H
#define PHP_DEADLINE_H

#include <php.h>

#include <limits.h>
#include <stddef.h>
#include "Zend/zend_hrtime.h"

typedef struct php_deadline {
	zend_hrtime_t hrtime;
} php_deadline;

static inline bool php_deadline_is_infinite(php_deadline *deadline)
{
	return deadline->hrtime == (zend_hrtime_t)-1;
}

/* Initialize an infinite deadline. */
static inline void php_deadline_init_infinite(php_deadline *deadline)
{
	deadline->hrtime = (zend_hrtime_t)-1;
}

/* Initialize a non-blocking deadline. */
static inline void php_deadline_init_nonblock(php_deadline *deadline)
{
	deadline->hrtime = 0;
}

/* Initialize deadline from a timeout relative to now.
 * tv_usec may be out of range. Negative or overflowing timeouts are infinite. */
static inline void php_deadline_init(php_deadline *deadline, struct timeval *timeout)
{
	if (timeout == NULL || timeout->tv_sec < 0
			|| (uintmax_t)timeout->tv_sec >= INT64_MAX / (int64_t)ZEND_NANO_IN_SEC) {
		php_deadline_init_infinite(deadline);
		return;
	}

	int64_t ns = (int64_t)timeout->tv_sec * (int64_t)ZEND_NANO_IN_SEC;
	int64_t usec = timeout->tv_usec;

	if (usec < -(ns / 1000) || usec > (INT64_MAX - ns) / 1000) {
		php_deadline_init_infinite(deadline);
		return;
	}

	ns += usec * 1000;

	deadline->hrtime = zend_hrtime();

	if (UNEXPECTED((zend_hrtime_t)ns >= ZEND_HRTIME_T_MAX - deadline->hrtime)) {
		php_deadline_init_infinite(deadline);
		return;
	}

	deadline->hrtime += ns;
}

/* Compute the remaining time until the deadline, in milliseconds, suitable
 * for passing directly to poll(2).
 * Returns -1 for an infinite deadline or if the remaining time overflows int.
 * Returns 0 if the deadline has already passed (non-blocking poll). */
static inline int php_deadline_to_timeout_ms(php_deadline *deadline)
{
	if (deadline == NULL || php_deadline_is_infinite(deadline)) {
		return -1;
	}

	zend_hrtime_t now = zend_hrtime();

	if (deadline->hrtime <= now) {
		/* Deadline expired */
		return 0;
	}

	zend_hrtime_t remaining = deadline->hrtime - now;
	const int nano_in_milli = 1000*1000;

	if (UNEXPECTED(remaining >= (zend_hrtime_t)INT_MAX * nano_in_milli)) {
		return -1;
	}

	return (remaining + nano_in_milli - 1) / nano_in_milli;
}

#endif /* PHP_DEADLINE_H */
