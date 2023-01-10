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

PHPAPI double php_random_gammasection_closed_open(const php_random_algo *algo, php_random_status *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 1)) {
		return NAN;
	}

	uint64_t k = 1 + php_random_range64(algo, status, hi - 1); /* [1, hi] */

	if (fabs(min) <= fabs(max)) {
		return k == hi ? min : max - k * g;
	} else {
		return min + (k - 1) * g;
	}
}

PHPAPI double php_random_gammasection_closed_closed(const php_random_algo *algo, php_random_status *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max < min)) {
		return NAN;
	}

	uint64_t k = php_random_range64(algo, status, hi); /* [0, hi] */

	if (fabs(min) <= fabs(max)) {
		return k == hi ? min : max - k * g;
	} else {
		return k == hi ? max : min + k * g;
	}
}

PHPAPI double php_random_gammasection_open_closed(const php_random_algo *algo, php_random_status *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 1)) {
		return NAN;
	}

	uint64_t k = php_random_range64(algo, status, hi - 1); /* [0, hi - 1] */

	if (fabs(min) <= fabs(max)) {
		return max - k * g;
	} else {
		return k == (hi - 1) ? max : min + (k + 1) * g;
	}
}

PHPAPI double php_random_gammasection_open_open(const php_random_algo *algo, php_random_status *status, double min, double max)
{
	double g = gamma_max(min, max);
	uint64_t hi = ceilint(min, max, g);

	if (UNEXPECTED(max <= min || hi < 2)) {
		return NAN;
	}

	uint64_t k = 1 + php_random_range64(algo, status, hi - 2); /* [1, hi - 1] */

	if (fabs(min) <= fabs(max)) {
		return max - k * g;
	} else {
		return min + k * g;
	}
}
