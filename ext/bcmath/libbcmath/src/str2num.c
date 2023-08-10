/* str2num.c: bcmath library file. */
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
#include <stdbool.h>
#include <stddef.h>

/* Convert strings to bc numbers.  Base 10 only.*/

bool bc_str2num(bc_num *num, char *str, size_t scale)
{
	size_t digits = 0;
	size_t strscale = 0;
	char *ptr, *nptr;
	size_t trailing_zeros = 0;
	bool zero_int = false;

	/* Prepare num. */
	bc_free_num (num);

	/* Check for valid number and count digits. */
	ptr = str;

	if ((*ptr == '+') || (*ptr == '-')) {
		/* Skip Sign */
		ptr++;
	}
	/* Skip leading zeros. */
	while (*ptr == '0') {
		ptr++;
	}
	/* digits before the decimal point */
	while (*ptr >= '0' && *ptr <= '9') {
		ptr++;
		digits++;
	}
	/* decimal point */
	if (*ptr == '.') {
		ptr++;
	}
	/* digits after the decimal point */
	while (*ptr >= '0' && *ptr <= '9') {
		if (*ptr == '0') {
			trailing_zeros++;
		} else {
			trailing_zeros = 0;
		}
		ptr++;
		strscale++;
	}

	if (trailing_zeros > 0) {
		/* Trailing zeros should not take part in the computation of the overall scale, as it is pointless. */
		strscale = strscale - trailing_zeros;
	}
	if ((*ptr != '\0') || (digits + strscale == 0)) {
		*num = bc_copy_num(BCG(_zero_));
		return *ptr == '\0';
	}

	/* Adjust numbers and allocate storage and initialize fields. */
	strscale = MIN(strscale, scale);
	if (digits == 0) {
		zero_int = true;
		digits = 1;
	}
	*num = bc_new_num (digits, strscale);

	/* Build the whole number. */
	ptr = str;
	if (*ptr == '-') {
		(*num)->n_sign = MINUS;
		ptr++;
	} else {
		(*num)->n_sign = PLUS;
		if (*ptr == '+') ptr++;
	}
	/* Skip leading zeros. */
	while (*ptr == '0') {
		ptr++;
	}
	nptr = (*num)->n_value;
	if (zero_int) {
		*nptr++ = 0;
		digits = 0;
	}
	for (; digits > 0; digits--) {
		*nptr++ = CH_VAL(*ptr++);
	}

	/* Build the fractional part. */
	if (strscale > 0) {
		/* skip the decimal point! */
		ptr++;
		for (; strscale > 0; strscale--) {
			*nptr++ = CH_VAL(*ptr++);
		}
	}

	if (bc_is_zero(*num)) {
		(*num)->n_sign = PLUS;
	}

	return true;
}
