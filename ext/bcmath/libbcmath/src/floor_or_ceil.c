/* floor_or_ceil.c: bcmath library file. */
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

void bc_floor_or_ceil(bc_num num, bool is_floor, bc_num *result)
{
	/* clear result */
	bc_free_num(result);

	/*  Initialize result */
	*result = bc_new_num(num->n_len, 0);
	(*result)->n_sign = num->n_sign;

	/* copy integer part */
	memcpy((*result)->n_value, num->n_value, num->n_len);

	/* If the number is positive and we are flooring, then nothing else needs to be done.
	 * Similarly, if the number is negative and we are ceiling, then nothing else needs to be done. */
	if (num->n_scale == 0 || (*result)->n_sign == (is_floor ? PLUS : MINUS)) {
		return;
	}

	/* check fractional part. */
	size_t count = num->n_scale;
	const char *nptr = num->n_value + num->n_len;
	while ((count > 0) && (*nptr++ == 0)) {
		count--;
	}

	/* If all digits past the decimal point are 0 */
	if (count == 0) {
		return;
	}

	/* Increment the absolute value of the result by 1 and add sign information */
	bc_num tmp = _bc_do_add(*result, BCG(_one_), 0);
	tmp->n_sign = (*result)->n_sign;
	bc_free_num(result);
	*result = tmp;
}
