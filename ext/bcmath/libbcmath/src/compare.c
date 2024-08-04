/* compare.c: bcmath library file. */
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

#include <stdbool.h>
#include "bcmath.h"
#include "private.h"
#include <stddef.h>


/* Compare two bc numbers.  Return value is 0 if equal, -1 if N1 is less
   than N2 and +1 if N1 is greater than N2.  If USE_SIGN is false, just
   compare the magnitudes. */

bcmath_compare_result _bc_do_compare(bc_num n1, bc_num n2, size_t scale, bool use_sign)
{
	char *n1ptr, *n2ptr;

	/* First, compare signs. */
	if (use_sign && n1->n_sign != n2->n_sign) {
		if (n1->n_sign == PLUS) {
			/* Positive N1 > Negative N2 */
			return BCMATH_LEFT_GREATER;
		} else {
			/* Negative N1 < Positive N1 */
			return BCMATH_RIGHT_GREATER;
		}
	}

	/* Now compare the magnitude. */
	if (n1->n_len != n2->n_len) {
		if (n1->n_len > n2->n_len) {
			/* Magnitude of n1 > n2. */
			if (!use_sign || n1->n_sign == PLUS) {
				return BCMATH_LEFT_GREATER;
			} else {
				return BCMATH_RIGHT_GREATER;
			}
		} else {
			/* Magnitude of n1 < n2. */
			if (!use_sign || n1->n_sign == PLUS) {
				return BCMATH_RIGHT_GREATER;
			} else {
				return BCMATH_LEFT_GREATER;
			}
		}
	}

	size_t n1_scale = MIN(n1->n_scale, scale);
	size_t n2_scale = MIN(n2->n_scale, scale);

	/* If we get here, they have the same number of integer digits.
	   check the integer part and the equal length part of the fraction. */
	size_t count = n1->n_len + MIN (n1_scale, n2_scale);
	n1ptr = n1->n_value;
	n2ptr = n2->n_value;

	while ((count > 0) && (*n1ptr == *n2ptr)) {
		n1ptr++;
		n2ptr++;
		count--;
	}

	if (count != 0) {
		if (*n1ptr > *n2ptr) {
			/* Magnitude of n1 > n2. */
			if (!use_sign || n1->n_sign == PLUS) {
				return BCMATH_LEFT_GREATER;
			} else {
				return BCMATH_RIGHT_GREATER;
			}
		} else {
			/* Magnitude of n1 < n2. */
			if (!use_sign || n1->n_sign == PLUS) {
				return BCMATH_RIGHT_GREATER;
			} else {
				return BCMATH_LEFT_GREATER;
			}
		}
	}

	/* They are equal up to the last part of the equal part of the fraction. */
	if (n1_scale != n2_scale) {
		if (n1_scale > n2_scale) {
			for (count = n1_scale - n2_scale; count > 0; count--) {
				if (*n1ptr++ != 0) {
					/* Magnitude of n1 > n2. */
					if (!use_sign || n1->n_sign == PLUS) {
						return BCMATH_LEFT_GREATER;
					} else {
						return BCMATH_RIGHT_GREATER;
					}
				}
			}
		} else {
			for (count = n2_scale - n1_scale; count > 0; count--) {
				if (*n2ptr++ != 0) {
					/* Magnitude of n1 < n2. */
					if (!use_sign || n1->n_sign == PLUS) {
						return BCMATH_RIGHT_GREATER;
					} else {
						return BCMATH_LEFT_GREATER;
					}
				}
			}
		}
	}

	/* They must be equal! */
	return BCMATH_EQUAL;
}


/* This is the "user callable" routine to compare numbers N1 and N2. */
bcmath_compare_result bc_compare(bc_num n1, bc_num n2, size_t scale)
{
	return _bc_do_compare(n1, n2, scale, true);
}
