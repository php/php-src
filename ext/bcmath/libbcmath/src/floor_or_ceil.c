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
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "bcmath.h"
#include "private.h"
#include <stddef.h>

bc_num bc_floor_or_ceil(bc_num num, bool is_floor)
{
	/*  Initialize result */
	bc_num result = bc_new_num(num->n_len, 0);
	result->n_sign = num->n_sign;

	/* copy integer part */
	memcpy(result->n_value, num->n_value, num->n_len);

	/* If the number is positive and we are flooring, then nothing else needs to be done.
	 * Similarly, if the number is negative and we are ceiling, then nothing else needs to be done. */
	if (num->n_scale == 0 || result->n_sign == (is_floor ? PLUS : MINUS)) {
		goto check_zero;
	}

	/* check fractional part. */
	size_t count = num->n_scale;
	const char *nptr = num->n_value + num->n_len;
	while ((count > 0) && (*nptr == 0)) {
		count--;
		nptr++;
	}

	/* If all digits past the decimal point are 0 */
	if (count == 0) {
		goto check_zero;
	}

	/* Increment the absolute value of the result by 1 and add sign information */
	bc_num tmp = _bc_do_add(result, BCG(_one_));
	tmp->n_sign = result->n_sign;
	bc_free_num(&result);
	result = tmp;

check_zero:
	if (bc_is_zero(result)) {
		result->n_sign = PLUS;
	}

	return result;
}
