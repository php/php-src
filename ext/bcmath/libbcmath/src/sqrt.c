/* sqrt.c: bcmath library file. */
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

/* Take the square root NUM and return it in NUM with SCALE digits
   after the decimal place. */

bool bc_sqrt(bc_num *num, size_t scale)
{
	const bc_num local_num = *num;
	/* Initial checks. */
	if (bc_is_neg(local_num)) {
		/* Cannot take the square root of a negative number */
		return false;
	}
	/* Square root of 0 is 0 */
	if (bc_is_zero(local_num)) {
		bc_free_num (num);
		*num = bc_copy_num(BCG(_zero_));
		return true;
	}

	bcmath_compare_result num_cmp_one = bc_compare(local_num, BCG(_one_), local_num->n_scale);
	/* Square root of 1 is 1 */
	if (num_cmp_one == BCMATH_EQUAL) {
		bc_free_num (num);
		*num = bc_copy_num(BCG(_one_));
		return true;
	}

	/* Initialize the variables. */
	size_t cscale;
	bc_num guess, guess1, point5, diff;
	size_t rscale = MAX(scale, local_num->n_scale);

	bc_init_num(&guess1);
	bc_init_num(&diff);
	point5 = bc_new_num (1, 1);
	point5->n_value[1] = 5;


	/* Calculate the initial guess. */
	if (num_cmp_one == BCMATH_RIGHT_GREATER) {
		/* The number is between 0 and 1.  Guess should start at 1. */
		guess = bc_copy_num(BCG(_one_));
		cscale = local_num->n_scale;
	} else {
		/* The number is greater than 1.  Guess should start at 10^(exp/2). */
		bc_init_num(&guess);
		bc_int2num(&guess, 10);

		bc_int2num(&guess1, local_num->n_len);
		bc_multiply_ex(guess1, point5, &guess1, 0);
		guess1->n_scale = 0;
		bc_raise_bc_exponent(guess, guess1, &guess, 0);
		bc_free_num (&guess1);
		cscale = 3;
	}

	/* Find the square root using Newton's algorithm. */
	bool done = false;
	while (!done) {
		bc_free_num (&guess1);
		guess1 = bc_copy_num(guess);
		bc_divide(*num, guess, &guess, cscale);
		bc_add_ex(guess, guess1, &guess, 0);
		bc_multiply_ex(guess, point5, &guess, cscale);
		bc_sub_ex(guess, guess1, &diff, cscale + 1);
		if (bc_is_near_zero(diff, cscale)) {
			if (cscale < rscale + 1) {
				cscale = MIN (cscale * 3, rscale + 1);
			} else {
				done = true;
			}
		}
	}

	/* Assign the number and clean up. */
	bc_free_num (num);
	bc_divide(guess, BCG(_one_), num, rscale);
	bc_free_num (&guess);
	bc_free_num (&guess1);
	bc_free_num (&point5);
	bc_free_num (&diff);
	return true;
}
