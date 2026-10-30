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
#include "convert.h"
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

static inline void bc_standard_sqrt(bc_num *num, size_t scale, size_t num_calc_full_len, size_t num_use_full_len, size_t leading_zeros)
{
	/* allocate memory */
	size_t n_arr_size = BC_ARR_SIZE_FROM_LEN(num_calc_full_len);

	size_t guess_full_len = (num_calc_full_len + 1) / 2;
	/* Since add the old guess and the new guess together during the calculation,
	 * there is a chance of overflow, so allocate an extra size. */
	size_t guess_arr_size = BC_ARR_SIZE_FROM_LEN(guess_full_len) + 1;

	/* n_arr_size * 2 + guess_arr_size * 3 */
	BC_VECTOR *buf = safe_emalloc(n_arr_size + guess_arr_size, sizeof(BC_VECTOR) * 2, guess_arr_size * sizeof(BC_VECTOR));

	BC_VECTOR *n_vector = buf;
	/* In division by successive approximation, the numerator is modified during the computation,
	 * so it must be copied each time. */
	BC_VECTOR *n_vector_copy = n_vector + n_arr_size;
	BC_VECTOR *guess_vector = n_vector_copy + n_arr_size;
	BC_VECTOR *guess1_vector = guess_vector + guess_arr_size;
	BC_VECTOR *tmp_div_ret_vector = guess1_vector + guess_arr_size;

	/* convert num to n_vector */
	const char *nend = (*num)->n_value + leading_zeros + num_use_full_len - 1;
	bc_convert_to_vector_with_zero_pad(n_vector, nend, num_use_full_len, num_calc_full_len - num_use_full_len);

	/* Prepare guess_vector. Use `bc_fast_sqrt_vector()` to quickly obtain a highly accurate initial value. */

	/* 18 on 64-bit, 10 on 32-bit */
	size_t n_top_len_for_initial_guess = (MAX_LENGTH_OF_LONG - 1) & ~1;

	/* Set the number of digits of num to be used as the initial value for Newton's method.
	 * Just as the square roots of 1000 and 100 differ significantly, the number of digits
	 * to "ignore" here must be even. */
	if (num_calc_full_len & 1) {
		n_top_len_for_initial_guess--;
	}

	BC_VECTOR n_top = n_vector[n_arr_size - 1];
	size_t n_top_index = n_arr_size - 2;
	size_t n_top_vector_len = num_calc_full_len % BC_VECTOR_SIZE == 0 ? BC_VECTOR_SIZE : num_calc_full_len % BC_VECTOR_SIZE;
	size_t count = n_top_len_for_initial_guess - n_top_vector_len;
	while (count >= BC_VECTOR_SIZE) {
		n_top *= BC_VECTOR_BOUNDARY_NUM;
		n_top += n_vector[n_top_index--];
		count -= BC_VECTOR_SIZE;
	}
	if (count > 0) {
		n_top *= BC_POW_10_LUT[count];
		n_top += n_vector[n_top_index] / BC_POW_10_LUT[BC_VECTOR_SIZE - count];
	}

	/* Calculate the initial guess. */
	BC_VECTOR initial_guess = bc_fast_sqrt_vector(n_top);

	/* Set the obtained initial guess to guess_vector. */
	size_t initial_guess_len = (n_top_len_for_initial_guess + 1) / 2;
	size_t guess_top_vector_len = guess_full_len % BC_VECTOR_SIZE == 0 ? BC_VECTOR_SIZE : guess_full_len % BC_VECTOR_SIZE;
	size_t guess_len_diff = initial_guess_len - guess_top_vector_len;
	guess_vector[guess_arr_size - 2] = initial_guess / BC_POW_10_LUT[guess_len_diff];
	initial_guess %= BC_POW_10_LUT[guess_len_diff];
	guess_vector[guess_arr_size - 3] = initial_guess * BC_POW_10_LUT[BC_VECTOR_SIZE - guess_len_diff];
	guess_vector[guess_arr_size - 3] += BC_POW_10_LUT[BC_VECTOR_SIZE - guess_len_diff] - 1;

	/* Initialize the uninitialized vector with `BC_VECTOR_BOUNDARY_NUM - 1`. */
	for (size_t i = 0; i < guess_arr_size - 3; i++) {
		guess_vector[i] = BC_VECTOR_BOUNDARY_NUM - 1;
		guess1_vector[i] = BC_VECTOR_BOUNDARY_NUM - 1;
	}
	guess_vector[guess_arr_size - 1] = 0;

	BC_VECTOR two[1] = { 2 };

	/* The precision (number of vectors) used for the calculation.
	 * Since the initial value uses two vectors, the initial precision is set to 2. */
	size_t guess_precision = 2;
	size_t guess_offset = guess_arr_size - 1 - guess_precision;
	size_t n_offset = guess_offset * 2;
	size_t n_precision = n_arr_size - n_offset;
	size_t quot_size = n_precision - (guess_precision) + 1;
	size_t guess_use_len = guess_top_vector_len + BC_VECTOR_SIZE;
	bool updated_precision = false;

	/**
	 * Newton's algorithm. Iterative expression is `x_{n+1} = (x_n + a / x_n) / 2`
	 * If break down the calculation into detailed steps, it looks like this:
	 * 1. quot = a / x_n
	 * 2. add = x_n + quot1
	 * 3. x_{n+1} = add / 2
	 * 4. repeat until the difference between the `x_n` and `x_{n+1}` is less than or equal to 1.
	 */
	bool done = false;
	do {
		if (updated_precision) {
			guess_offset = guess_arr_size - 1 - guess_precision;
			n_offset = guess_offset * 2;
			n_precision = n_arr_size - n_offset;
			quot_size = n_precision - (guess_precision) + 1;
			guess_use_len = guess_top_vector_len + (guess_precision - 1) * BC_VECTOR_SIZE;
			updated_precision = false;
		}

		/* Since the value changes during division by successive approximation, use a copied version of it. */
		memcpy(n_vector_copy + n_offset, n_vector + n_offset, n_precision * sizeof(BC_VECTOR));

		/* 1. quot = a / x_n */
		bc_divide_vector(
			n_vector_copy + n_offset, n_precision,
			guess_vector + guess_offset, guess_precision, guess_use_len,
			tmp_div_ret_vector + guess_offset, quot_size
		);

		BC_VECTOR *tmp_vptr = guess1_vector;
		guess1_vector = guess_vector;
		guess_vector = tmp_vptr;

		/* 2. add = x_n + quot1 */
		int carry = 0;
		for (size_t i = guess_offset; i < guess_arr_size - 1; i++) {
			guess_vector[i] = guess1_vector[i] + tmp_div_ret_vector[i] + carry;
			if (guess_vector[i] >= BC_VECTOR_BOUNDARY_NUM) {
				guess_vector[i] -= BC_VECTOR_BOUNDARY_NUM;
				carry = 1;
			} else {
				carry = 0;
			}
		}
		guess_vector[guess_arr_size - 1] = tmp_div_ret_vector[guess_arr_size - 1] + carry;

		/* 3. x_{n+1} = add / 2 */
		bc_divide_vector(
			guess_vector + guess_offset, guess_precision + 1,
			two, 1, 1,
			tmp_div_ret_vector + guess_offset, guess_precision + 1
		);

		memcpy(guess_vector + guess_offset, tmp_div_ret_vector + guess_offset, guess_precision * sizeof(BC_VECTOR));

		/* 4. repeat until the difference between the `x_n` and `x_{n+1}` is less than or equal to 1. */
		if (guess_precision < guess_arr_size - 1) {
			/* If the precision has not yet reached the maximum number of digits, it will be increased. */
			guess_precision = MIN(guess_precision * 2, guess_arr_size - 1);
			updated_precision = true;
		} else {
			size_t diff = guess_vector[0] > guess1_vector[0] ? guess_vector[0] - guess1_vector[0] : guess1_vector[0] - guess_vector[0];
			if (diff <= 1) {
				bool is_same = true;
				for (size_t i = 1; i < guess_arr_size - 1; i++) {
					if (guess_vector[i] != guess1_vector[i]) {
						is_same = false;
						break;
					}
				}
				done = is_same;
			}
		}
	} while (!done);

	size_t guess_len;
	size_t guess_leading_zeros = 0;
	if (leading_zeros > 0) {
		guess_len = 1; /* for int zero */
		guess_leading_zeros = (leading_zeros + 1) / 2;
	} else {
		guess_len = ((*num)->n_len + 1) / 2;
	}
	bc_num ret = bc_new_num_nonzeroed(guess_len, scale + 1);
	char *rptr = ret->n_value;
	char *rend = rptr + guess_len + scale + 1 - 1;

	for (size_t i = 0; i < guess_leading_zeros; i++) {
		*rptr++ = 0;
	}
	bc_convert_vector_to_char(guess_vector, rptr, rend, guess_arr_size - 1);
	ret->n_scale = scale;

	bc_free_num(num);
	*num = ret;

	efree(buf);
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
	if (bc_is_zero_for_scale(*num, num_use_scale)) {
		bc_free_num (num);
		*num = bc_copy_num(BCG(_zero_));
		return true;
	}

	bcmath_compare_result num_cmp_one = bc_compare(*num, BCG(_one_), num_use_scale);
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
		bc_standard_sqrt(num, scale, num_calc_full_len, num_use_full_len, leading_zeros);
	}

	return true;
}
