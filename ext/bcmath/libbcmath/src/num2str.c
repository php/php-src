/* num2str.c: bcmath library file. */
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
#include <stddef.h>
#include "zend_string.h"

/* Convert a numbers to a string.  Base 10 only.*/
zend_string *bc_num2str_ex(bc_num num, size_t scale)
{
	zend_string *str;
	char *sptr;
	size_t index;
	bool signch;

	/* Number of sign chars. */
	signch = num->n_sign != PLUS && !bc_is_zero_for_scale(num, MIN(num->n_scale, scale));
	/* Allocate the string memory. */
	if (scale > 0) {
		str = zend_string_alloc(num->n_len + scale + signch + 1, 0);
	} else {
		str = zend_string_alloc(num->n_len + signch, 0);
	}

	/* The negative sign if needed. */
	sptr = ZSTR_VAL(str);
	if (signch) *sptr++ = '-';

	/* Load the whole number. */
	const char *nptr = num->n_value;
	for (index = num->n_len; index > 0; index--) {
		*sptr++ = BCD_CHAR(*nptr++);
	}

	/* Now the fraction. */
	if (scale > 0) {
		*sptr++ = '.';
		for (index = 0; index < scale && index < num->n_scale; index++) {
			*sptr++ = BCD_CHAR(*nptr++);
		}
		for (index = num->n_scale; index < scale; index++) {
			*sptr++ = BCD_CHAR(0);
		}
	}

	/* Terminate the string and return it! */
	*sptr = '\0';
	ZSTR_LEN(str) = sptr - (char *)ZSTR_VAL(str);
	return str;
}
