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
   | Authors: Tim Düsterhus <timwolla@php.net>                            |
   |                                                                      |
   | Based on code from: Frédéric Goualard                                |
   | Corrected to handled appropriately random integers larger than 2^53  |
   | converted to double precision floats, and to avoid overflows for     |
   | large gammas.                                                        |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"
#include <math.h>

/* This file implements the γ-section algorithm as published in:
 *
 * Drawing Random Floating-Point Numbers from an Interval. Frédéric
 * Goualard, ACM Trans. Model. Comput. Simul., 32:3, 2022.
 * https://doi.org/10.1145/3503512
 */

static double gamma_low(double x)
{
	return x - nextafter(x, -DBL_MAX);
}

static double gamma_high(double x)
{
	return nextafter(x, DBL_MAX) - x;
}

static double gamma_max(double x, double y)
{
	return (fabs(x) > fabs(y)) ? gamma_high(x) : gamma_low(y);
}

static void splitint64(uint64_t v, double *vhi, double *vlo)
{
	*vhi = v >> 2;
	*vlo = v & UINT64_C(0x3);
}

static uint64_t ceilint(double a, double b, double g)
{
	double s = b / g - a / g;
	double e;

	if (fabs(a) <= fabs(b)) {
		e = -a / g - (s - b / g);
	} else {
		e = b / g - (s + a / g);
	}

	double si = ceil(s);

	return (s != si) ? (uint64_t)si : (uint64_t)si + (e > 0);
}

PHPAPI double php_random_gammasection_closed_open(const php_random_algo *algo, void *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 1)) {
		return NAN;
	}

	uint64_t k = 1 + php_random_range64(algo, status, hi - 1); /* [1, hi] */

	if (fabs(min) <= fabs(max)) {
		if (k == hi) {
			return min;
		} else {
			double k_hi, k_lo;
			splitint64(k, &k_hi, &k_lo);

			return 0x1p+2 * (max * 0x1p-2 - k_hi * g) - k_lo * g;
		}
	} else {
		double k_hi, k_lo;
		splitint64(k - 1, &k_hi, &k_lo);

		return 0x1p+2 * (min * 0x1p-2 + k_hi * g) + k_lo * g;
	}
}

PHPAPI double php_random_gammasection_closed_closed(const php_random_algo *algo, void *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max < min)) {
		return NAN;
	}

	uint64_t k = php_random_range64(algo, status, hi); /* [0, hi] */

	if (fabs(min) <= fabs(max)) {
		if (k == hi) {
			return min;
		} else {
			double k_hi, k_lo;
			splitint64(k, &k_hi, &k_lo);

			return 0x1p+2 * (max * 0x1p-2 - k_hi * g) - k_lo * g;
		}
	} else {
		if (k == hi) {
			return max;
		} else {
			double k_hi, k_lo;
			splitint64(k, &k_hi, &k_lo);

			return 0x1p+2 * (min * 0x1p-2 + k_hi * g) + k_lo * g;
		}
	}
}

PHPAPI double php_random_gammasection_open_closed(const php_random_algo *algo, void *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 1)) {
		return NAN;
	}

	uint64_t k = php_random_range64(algo, status, hi - 1); /* [0, hi - 1] */

	if (fabs(min) <= fabs(max)) {
		double k_hi, k_lo;
		splitint64(k, &k_hi, &k_lo);

		return 0x1p+2 * (max * 0x1p-2 - k_hi * g) - k_lo * g;
	} else {
		if (k == (hi - 1)) {
			return max;
		} else {
			double k_hi, k_lo;
			splitint64(k + 1, &k_hi, &k_lo);

			return 0x1p+2 * (min * 0x1p-2 + k_hi * g) + k_lo * g;
		}
	}
}

PHPAPI double php_random_gammasection_open_open(const php_random_algo *algo, void *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 2)) {
		return NAN;
	}

	uint64_t k = 1 + php_random_range64(algo, status, hi - 2); /* [1, hi - 1] */

	if (fabs(min) <= fabs(max)) {
		double k_hi, k_lo;
		splitint64(k, &k_hi, &k_lo);

		return 0x1p+2 * (max * 0x1p-2 - k_hi * g) - k_lo * g;
	} else {
		double k_hi, k_lo;
		splitint64(k, &k_hi, &k_lo);

		return 0x1p+2 * (min * 0x1p-2 + k_hi * g) + k_lo * g;
	}
}
