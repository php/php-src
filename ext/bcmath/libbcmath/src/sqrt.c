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
#include "private.h"

/* Take the square root NUM and return it in NUM with SCALE digits
   after the decimal place. */

static inline BC_VECTOR bc_sqrt_get_pow_10(size_t exponent)
{
	BC_VECTOR value = 1;
	while (exponent >= 8) {
		value *= BC_POW_10_LUT[8];
		exponent -= 8;
	}
	value *= BC_POW_10_LUT[exponent];
	return value;
}

static BC_VECTOR bc_fast_sqrt_vector(BC_VECTOR n_vector)
{
	/* Use a bitwise method for approximating the square root
	 * as the initial guess for Newton's method. */
	union {
		uint64_t i;
		double d;
	} u;
	u.d = (double) n_vector;
	u.i = (1ULL << 61) + (u.i >> 1) - (1ULL << 50);
	BC_VECTOR guess_vector = (BC_VECTOR) u.d;

	/* Newton's algorithm. */
	BC_VECTOR guess1_vector;
	size_t diff;
	do {
		guess1_vector = guess_vector;
		guess_vector = n_vector / guess_vector;
		guess_vector += guess1_vector;
		guess_vector /= 2;
		diff = guess1_vector > guess_vector ? guess1_vector - guess_vector : guess_vector - guess1_vector;
	} while (diff > 1);
	return guess_vector;
}

bool bc_sqrt(bc_num *num, size_t scale)
{
	/* Initial checks. */
	if (bc_is_neg(*num)) {
		/* Cannot take the square root of a negative number */
		return false;
	}
	/* Square root of 0 is 0 */
	if (bc_is_zero(*num)) {
		bc_free_num (num);
		*num = bc_copy_num(BCG(_zero_));
		return true;
	}

	bcmath_compare_result num_cmp_one = bc_compare(*num, BCG(_one_), (*num)->n_scale);
	/* Square root of 1 is 1 */
	if (num_cmp_one == BCMATH_EQUAL) {
		bc_free_num (num);
		*num = bc_copy_num(BCG(_one_));
		return true;
	}

	/* Initialize the variables. */
	size_t rscale = MAX(scale, (*num)->n_scale);
	size_t num_calc_full_len = (*num)->n_len + (rscale + 1) * 2;

	/* Find the square root using Newton's algorithm. */
	if (num_calc_full_len < MAX_LENGTH_OF_LONG) {
		/* Fast path */

		BC_VECTOR n_vector = 0;
		size_t i = 0;
		for (; i < (*num)->n_len + (*num)->n_scale; i++) {
			n_vector = n_vector * BASE + (*num)->n_value[i];
		}
		/* When calculating the square root of a number using only integer operations,
		 * need to adjust the digit scale accordingly.
		 * Considering that the original number is the square of the result,
		 * if the desired scale of the result is 5, the input number should be scaled
		 * by twice that, i.e., scale 10. */
		n_vector *= bc_sqrt_get_pow_10((rscale + 1) * 2 - (*num)->n_scale);

		/* Get sqrt */
		BC_VECTOR guess_vector = bc_fast_sqrt_vector(n_vector);

		size_t full_len = 0;
		BC_VECTOR tmp_guess_vector = guess_vector;
		do {
			tmp_guess_vector /= BASE;
			full_len++;
		} while (tmp_guess_vector > 0);

		size_t ret_ren = full_len > rscale + 1 ? full_len - (rscale + 1) : 1; /* for int zero */
		bc_num ret = bc_new_num_nonzeroed(ret_ren, rscale);
		char *rptr = ret->n_value;
		char *rend = rptr + ret_ren + rscale - 1;

		guess_vector /= BASE; /* Since the scale of guess_vector is rscale + 1, reduce the scale by 1. */
		while (rend >= rptr) {
			*rend-- = guess_vector % BASE;
			guess_vector /= BASE;
		}
		bc_free_num(num);
		*num = ret;
	} else {
		/* Standard path */

		bc_num guess;
		size_t cscale;
		/* Calculate the initial guess. */
		if (num_cmp_one == BCMATH_RIGHT_GREATER) {
			/* The number is between 0 and 1.  Guess should start at 1. */
			guess = bc_copy_num(BCG(_one_));
			cscale = (*num)->n_scale;
		} else {
			/* The number is greater than 1.  Guess should start at 10^(exp/2). */
			/* If just divide size_t by 2 it will not overflow. */
			size_t exponent_for_initial_guess = (size_t) (*num)->n_len >> 1;

			/* 10^n is a 1 followed by n zeros. */
			guess = bc_new_num(exponent_for_initial_guess + 1, 0);
			guess->n_value[0] = 1;
			cscale = 3;
		}

		bc_num guess1 = NULL;
		bc_num point5 = bc_new_num (1, 1);
		point5->n_value[1] = 5;
		bc_num diff = NULL;

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
	}

	return true;
}
