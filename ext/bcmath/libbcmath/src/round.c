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

void bc_round(bc_num num, zend_long precision, zend_long mode, bc_num *result)
{
	/* clear result */
	bc_free_num(result);

	/*
	* The following cases result in an early return:
	*
	* - When rounding to an integer part which is larger than the number
	*   e.g. Rounding 21.123 to 3 digits before the decimal point.
	* - When rounding to a greater decimal precision then the number has, the number is unchanged
	*   e.g. Rounding 21.123 to 4 digits after the decimal point.
	* - If the fractional part ends with zeros, the zeros are omitted and the number of digits in num is reduced.
	*   Meaning we might end up in the previous case.
	*/
	if (precision < 0 && num->n_len < (size_t) (-(precision + Z_L(1))) + 1) {
		*result = bc_copy_num(BCG(_zero_));
		return;
	}
	/* Just like bcadd('1', '1', 4) becomes '2.0000', it pads with zeros at the end if necessary. */
	if (precision >= 0 && num->n_scale <= precision) {
		if (num->n_scale == precision) {
			*result = bc_copy_num(num);
		} else if(num->n_scale < precision) {
			*result = bc_new_num(num->n_len, precision);
			(*result)->n_sign = num->n_sign;
			memcpy((*result)->n_value, num->n_value, num->n_len + num->n_scale);
		}
		return;
	}

	/*
	 * If the calculation result is a negative value, there is an early return,
	 * so no underflow will occur.
	 */
	size_t rounded_len = num->n_len + precision;

	/*
	 * Initialize result
	 * For example, if rounded_len is 0, it means trying to round 50 to 100 or 0.
	 * If the result of rounding is carried over, it will be added later, so first set it to 0 here.
	 */
	if (rounded_len == 0) {
		*result = bc_copy_num(BCG(_zero_));
	} else {
		*result = bc_new_num(num->n_len, precision > 0 ? precision : 0);
		memcpy((*result)->n_value, num->n_value, rounded_len);
	}
	(*result)->n_sign = num->n_sign;

	const char *nptr = num->n_value + rounded_len;

	/* Check cases that can be determined without looping. */
	switch (mode) {
		case PHP_ROUND_HALF_UP:
			if (*nptr >= 5) {
				goto up;
			} else if (*nptr < 5) {
				goto check_zero;
			}
			break;

		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
			if (*nptr > 5) {
				goto up;
			} else if (*nptr < 5) {
				goto check_zero;
			}
			/* if *nptr == 5, we need to look-up further digits before making a decision. */
			break;

		case PHP_ROUND_CEILING:
			if (num->n_sign != PLUS) {
				goto check_zero;
			} else if (*nptr > 0) {
				goto up;
			}
			/* if *nptr == 0, a loop is required for judgment. */
			break;

		case PHP_ROUND_FLOOR:
			if (num->n_sign != MINUS) {
				goto check_zero;
			} else if (*nptr > 0) {
				goto up;
			}
			/* if *nptr == 0, a loop is required for judgment. */
			break;

		case PHP_ROUND_TOWARD_ZERO:
			goto check_zero;

		case PHP_ROUND_AWAY_FROM_ZERO:
			if (*nptr > 0) {
				goto up;
			}
			/* if *nptr == 0, a loop is required for judgment. */
			break;

		EMPTY_SWITCH_DEFAULT_CASE()
	}

	/* Loop through the remaining digits. */
	size_t count = num->n_len + num->n_scale - rounded_len - 1;
	nptr++;
	while ((count > 0) && (*nptr == 0)) {
		count--;
		nptr++;
	}

	if (count > 0) {
		goto up;
	}

	switch (mode) {
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_AWAY_FROM_ZERO:
			goto check_zero;

		case PHP_ROUND_HALF_EVEN:
			if (rounded_len == 0 || num->n_value[rounded_len - 1] % 2 == 0) {
				goto check_zero;
			}
			break;

		case PHP_ROUND_HALF_ODD:
			if (rounded_len != 0 && num->n_value[rounded_len - 1] % 2 == 1) {
				goto check_zero;
			}
			break;

		EMPTY_SWITCH_DEFAULT_CASE()
	}

up:
	{
		bc_num tmp;

		if (rounded_len == 0) {
			tmp = bc_new_num(num->n_len + 1, 0);
			tmp->n_value[0] = 1;
			tmp->n_sign = num->n_sign;
		} else {
			bc_num scaled_one = bc_new_num((*result)->n_len, (*result)->n_scale);
			scaled_one->n_value[rounded_len - 1] = 1;

			tmp = _bc_do_add(*result, scaled_one);
			tmp->n_sign = (*result)->n_sign;
			bc_free_num(&scaled_one);
		}

		bc_free_num(result);
		*result = tmp;
	}

check_zero:
	if (bc_is_zero(*result)) {
		(*result)->n_sign = PLUS;
	}
}
