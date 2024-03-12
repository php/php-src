/* round.c: bcmath library file. */
/*
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.
    Copyright (C) 2000 Philip A. Nelson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.  (LICENSE)

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.

    You may contact the author by:
       e-mail:  philnelson@acm.org
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062

*************************************************************************/

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
	if (precision < 0 && num->n_len <= (size_t) (-(precision + Z_L(1))) + 1) {
		*result = bc_copy_num(BCG(_zero_));
		return;
	}
	if (precision >= 0 && num->n_scale <= precision) {
		*result = bc_copy_num(num);
		return;
	}

	/* Initialize result */
	*result = bc_new_num(num->n_len, precision > 0 ? precision : 0);
	(*result)->n_sign = num->n_sign;
	/*
	 * If the calculation result is a negative value, there is an early return,
	 * so no underflow will occur.
	 */
	size_t rounded_len = num->n_len + precision;
	memcpy((*result)->n_value, num->n_value, rounded_len);

	const char *nptr = num->n_value + rounded_len;

	/* Check cases that can be determined without looping. */
	switch (mode) {
		case PHP_ROUND_HALF_UP:
			if (*nptr >= 5) {
				goto up;
			} else if (*nptr < 5) {
				return;
			}
			break;

		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
			if (*nptr > 5) {
				goto up;
			} else if (*nptr < 5) {
				return;
			}
			/* if *nptr == 5, we need to look-up further digits before making a decision. */
			break;

		case PHP_ROUND_CEILING:
			if (num->n_sign != PLUS) {
				return;
			} else if (*nptr > 0) {
				goto up;
			}
			/* if *nptr == 0, a loop is required for judgment. */
			break;

		case PHP_ROUND_FLOOR:
			if (num->n_sign != MINUS) {
				return;
			} else if (*nptr > 0) {
				goto up;
			}
			/* if *nptr == 0, a loop is required for judgment. */
			break;

		case PHP_ROUND_TOWARD_ZERO:
			return;

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
	while ((count > 0) && (*nptr++ == 0)) {
		count--;
	}

	if (count > 0) {
		goto up;
	}

	switch (mode) {
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_AWAY_FROM_ZERO:
			return;

		case PHP_ROUND_HALF_EVEN:
			if (num->n_value[rounded_len - 1] % 2 == 0) {
				return;
			}
			break;

		case PHP_ROUND_HALF_ODD:
			if (num->n_value[rounded_len - 1] % 2 == 1) {
				return;
			}
			break;

		EMPTY_SWITCH_DEFAULT_CASE()
	}

up:
	{
		bc_num scaled_one = bc_new_num((*result)->n_len, (*result)->n_scale);
		scaled_one->n_value[rounded_len - 1] = 1;

		bc_num tmp = _bc_do_add(*result, scaled_one, (*result)->n_scale);
		tmp->n_sign = (*result)->n_sign;

		bc_free_num(&scaled_one);
		bc_free_num(result);
		*result = tmp;
	}
}
