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
	if (0 && num_calc_full_len < MAX_LENGTH_OF_LONG) {
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

		/* convert num to n_vector */
		size_t n_vector_size = BC_ARR_SIZE_FROM_LEN(num_calc_full_len);
		BC_VECTOR *buf = safe_emalloc(n_vector_size, sizeof(BC_VECTOR) * 2, 0);
		BC_VECTOR *n_vector = buf;
		BC_VECTOR *n_vector_copy = n_vector + n_vector_size;

		size_t n_full_len = (*num)->n_len + (*num)->n_scale;
		const char *nend = (*num)->n_value + n_full_len - 1;
		size_t n_extend_zeros = num_calc_full_len - n_full_len;

		bc_convert_to_vector_with_zero_pad(n_vector, nend, n_full_len, n_extend_zeros);

		/* init vectors */
		size_t guess_len = ((*num)->n_len + 1) / 2;
		size_t guess_scale = rscale + 1;
		size_t guess_full_len = guess_len + guess_scale;
		size_t guess_vector_size = BC_ARR_SIZE_FROM_LEN(guess_full_len);

		BC_VECTOR *buf2 = safe_emalloc(guess_vector_size, sizeof(BC_VECTOR) * 2, 0);
		BC_VECTOR *guess_vector = buf2;
		BC_VECTOR *guess1_vector = guess_vector + guess_vector_size;

		size_t guess_quot_size = guess_vector_size + 1;
		BC_VECTOR *tmp_div_ret_vector = safe_emalloc(guess_quot_size, sizeof(BC_VECTOR), 0);

		for (size_t i = 0; i < guess_vector_size - 1; i++) {
			guess_vector[i] = BC_VECTOR_BOUNDARY_NUM - 1;
		}
		if (guess_full_len % BC_VECTOR_SIZE == 0) {
			guess_vector[guess_vector_size - 1] = BC_VECTOR_BOUNDARY_NUM / 10;
		} else {
			guess_vector[guess_vector_size - 1] = 0;
			for (size_t i = 0; i < guess_full_len % BC_VECTOR_SIZE; i++) {
				guess_vector[guess_vector_size - 1] *= BASE;
				guess_vector[guess_vector_size - 1] += 9;
			}
		}

		BC_VECTOR two[1] = { 2 };

		/* Newton's algorithm. */
		//bool done = false;
		for (size_t i = 0; i < 100000; i++) {
		//do {
			for (size_t i = 0; i < n_vector_size; i++) {
				n_vector_copy[i] = n_vector[i];
			}
			bool div_ret = bc_divide_vector(n_vector_copy, n_vector_size, guess_vector, guess_vector_size, tmp_div_ret_vector, guess_quot_size);
			ZEND_ASSERT(div_ret);

			BC_VECTOR *tmp_vptr = guess1_vector;
			guess1_vector = guess_vector;
			guess_vector = tmp_vptr;
			int carry = 0;
			for (size_t i = 0; i < guess_vector_size; i++) {
				guess_vector[i] = guess1_vector[i] + tmp_div_ret_vector[i] + carry;
				if (guess_vector[i] >= BC_VECTOR_BOUNDARY_NUM) {
					guess_vector[i] -= BC_VECTOR_BOUNDARY_NUM;
					carry = 1;
				} else {
					carry = 0;
				}
			}
			ZEND_ASSERT(carry == 0);
			div_ret = bc_divide_vector(guess_vector, guess_vector_size, two, 1, tmp_div_ret_vector, guess_vector_size);
			ZEND_ASSERT(div_ret);

			for (size_t i = 0; i < guess_vector_size; i++) {
				guess_vector[i] = tmp_div_ret_vector[i];
			}

			//size_t diff = guess_vector[0] > guess1_vector[0] ? guess_vector[0] - guess1_vector[0] : guess1_vector[0] - guess_vector[0];
			// if (diff <= 1) {
			// 	bool is_same = true;
			// 	for (size_t i = 1; i < guess_vector_size; i++) {
			// 		if (guess_vector[i] != guess1_vector[i]) {
			// 			is_same = false;
			// 			break;
			// 		}
			// 	}
			// 	done = is_same;
			// }
		//} while (!done);
		}

		bc_num ret = bc_new_num_nonzeroed(guess_len, guess_scale);
		char *rptr = ret->n_value;
		char *rend = rptr + guess_full_len - 1;

		bc_convert_vector_to_char(guess_vector, rptr, rend, guess_vector_size);

		ret->n_scale = rscale;
		_bc_rm_leading_zeros(ret);

		bc_free_num(num);
		*num = ret;

		efree(buf);
		efree(buf2);
		efree(tmp_div_ret_vector);
	}

	return true;
}
