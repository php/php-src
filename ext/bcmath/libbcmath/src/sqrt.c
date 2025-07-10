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
	/* Use sqrt() from <math.h> to determine the initial value. */
	BC_VECTOR guess_vector = (BC_VECTOR) round(sqrt((double) n_vector));

	/* Newton's algorithm. Iterative expression is `x_{n+1} = (x_n + a / x_n) / 2` */
	BC_VECTOR guess1_vector;
	size_t diff;
	do {
		guess1_vector = guess_vector;
		guess_vector = (guess1_vector + n_vector / guess1_vector) / 2; /* Iterative expression */
		diff = guess1_vector > guess_vector ? guess1_vector - guess_vector : guess_vector - guess1_vector;
	} while (diff > 1);
	return guess_vector;
}

static inline void bc_fast_sqrt(bc_num *num, size_t scale, size_t num_calc_full_len, size_t num_use_full_len, size_t leading_zeros)
{
	BC_VECTOR n_vector = 0;
	const char *nptr = (*num)->n_value + leading_zeros;
	for (size_t i = 0; i < num_use_full_len; i++) {
		n_vector = n_vector * BASE + *nptr++;
	}
	/* When calculating the square root of a number using only integer operations,
	 * need to adjust the digit scale accordingly.
	 * Considering that the original number is the square of the result,
	 * if the desired scale of the result is 5, the input number should be scaled
	 * by twice that, i.e., scale 10. */
	n_vector *= bc_sqrt_get_pow_10(num_calc_full_len - num_use_full_len);

	/* Get sqrt */
	BC_VECTOR guess_vector = bc_fast_sqrt_vector(n_vector);

	size_t ret_len;
	if (leading_zeros > 0) {
		ret_len = 1;
	} else {
		ret_len = ((*num)->n_len + 1) / 2;
	}

	bc_num ret = bc_new_num_nonzeroed(ret_len, scale);
	char *rptr = ret->n_value;
	char *rend = rptr + ret_len + scale - 1;

	guess_vector /= BASE; /* Since the scale of guess_vector is scale + 1, reduce the scale by 1. */
	while (rend >= rptr) {
		*rend-- = guess_vector % BASE;
		guess_vector /= BASE;
	}
	bc_free_num(num);
	*num = ret;
}

static inline void bc_standard_sqrt(bc_num *num, size_t scale, bcmath_compare_result num_cmp_one)
{
	size_t cscale;
	bc_num guess, guess1, point5, diff;
	size_t rscale = MAX(scale, (*num)->n_scale);

	bc_init_num(&guess1);
	bc_init_num(&diff);
	point5 = bc_new_num (1, 1);
	point5->n_value[1] = 5;


	/* Calculate the initial guess. */
	if (num_cmp_one == BCMATH_RIGHT_GREATER) {
		/* The number is between 0 and 1.  Guess should start at 1. */
		guess = bc_copy_num(BCG(_one_));
		cscale = (*num)->n_scale;
	} else {
		/* The number is greater than 1.  Guess should start at 10^(exp/2). */
		bc_init_num(&guess);
		bc_int2num(&guess, 10);

		bc_int2num(&guess1, (*num)->n_len);
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
}

bool bc_sqrt(bc_num *num, size_t scale)
{
	/* Initial checks. */
	if (bc_is_neg(*num)) {
		/* Cannot take the square root of a negative number */
		return false;
	}

	size_t num_calc_scale = (scale + 1) * 2;
	size_t num_use_scale = MIN((*num)->n_scale, num_calc_scale);

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
	size_t leading_zeros = 0;
	size_t num_calc_full_len = (*num)->n_len + num_calc_scale;
	size_t num_use_full_len = (*num)->n_len + num_use_scale;
	if (num_cmp_one == BCMATH_RIGHT_GREATER) {
		const char *nptr = (*num)->n_value;
		while (*nptr == 0) {
			leading_zeros++;
			nptr++;
		}
		num_calc_full_len -= leading_zeros;
		num_use_full_len -= leading_zeros;
	}

	/* Find the square root using Newton's algorithm. */
	if (num_calc_full_len < MAX_LENGTH_OF_LONG) {
		bc_fast_sqrt(num, scale, num_calc_full_len, num_use_full_len, leading_zeros);
	} else {
		bc_standard_sqrt(num, scale, num_cmp_one);
	}

	return true;
}
