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

bool bc_str2num(bc_num *num, char *str, size_t scale, bool auto_scale)
{
	size_t digits = 0;
	size_t str_scale = 0;
	char *ptr = str;
	char *nptr;
	char *integer_ptr;
	char *integer_end;
	char *fractional_ptr = NULL;
	char *fractional_end = NULL;
	char *decimal_point;
	bool zero_int = false;

	/* Prepare num. */
	bc_free_num (num);

	/* Check for valid number and count digits. */
	if ((*ptr == '+') || (*ptr == '-')) {
		/* Skip Sign */
		ptr++;
	}
	/* Skip leading zeros. */
	while (*ptr == '0') {
		ptr++;
	}
	integer_ptr = ptr;
	/* digits before the decimal point */
	while (*ptr >= '0' && *ptr <= '9') {
		ptr++;
		digits++;
	}
	/* decimal point */
	decimal_point = (*ptr == '.') ? ptr : NULL;

	/* If a non-digit and non-decimal-point indicator is in the string, i.e. an invalid character */
	if (!decimal_point && *ptr != '\0') {
		goto fail;
	}

	/* search and validate fractional end if exists */
	if (decimal_point) {
		/* search */
		fractional_ptr = fractional_end = decimal_point + 1;
		if (*fractional_ptr == '\0') {
			goto after_fractional;
		}

		/* validate */
		while (*fractional_ptr >= '0' && *fractional_ptr <= '9') {
			fractional_end = *fractional_ptr != '0' ? fractional_ptr + 1 : fractional_end;
			fractional_ptr++;
		}
		if (*fractional_ptr != '\0') {
			/* invalid num */
			goto fail;
		}
		fractional_ptr = decimal_point + 1;

		str_scale = fractional_end - fractional_ptr;
		if (str_scale > scale && !auto_scale) {
			fractional_end -= str_scale - scale;
			str_scale = scale;
		}
	}

after_fractional:

	if (digits + str_scale == 0) {
		goto zero;
	}

	/* Adjust numbers and allocate storage and initialize fields. */
	if (digits == 0) {
		zero_int = true;
		digits = 1;
	}
	*num = bc_new_num (digits, str_scale);
	(*num)->n_sign = *str == '-' ? MINUS : PLUS;
	nptr = (*num)->n_value;

	if (zero_int) {
		nptr++;
		while (fractional_ptr < fractional_end) {
			*nptr = CH_VAL(*fractional_ptr);
			nptr++;
			fractional_ptr++;
		}
	} else {
		integer_end = integer_ptr + digits;
		while (integer_ptr < integer_end) {
			*nptr = CH_VAL(*integer_ptr);
			nptr++;
			integer_ptr++;
		}
		if (str_scale > 0) {
			while (fractional_ptr < fractional_end) {
				*nptr = CH_VAL(*fractional_ptr);
				nptr++;
				fractional_ptr++;
			}
		}
	}

	return true;

zero:
	*num = bc_copy_num(BCG(_zero_));
	return true;

fail:
	*num = bc_copy_num(BCG(_zero_));
	return false;
}
