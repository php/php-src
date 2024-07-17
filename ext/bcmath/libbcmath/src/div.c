/* div.c: bcmath library file. */
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
#include "convert.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zend_alloc.h"

static const BC_VECTOR POW_10_LUT[9] = {
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000
};

/*
 * Use this function when the number of array elements in n2 is 1, that is, when n2 is not divided into multiple array elements.
 * The algorithm can be simplified if n2 is not divided, and this function is an optimization of bc_standard_div.
 */
static inline void bc_fast_div(BC_VECTOR *n1_vector, size_t n1_arr_size, BC_VECTOR n2_vector, BC_VECTOR *quot_vector, size_t quot_arr_size)
{
	size_t n1_top_index = n1_arr_size - 1;
	size_t quot_top_index = quot_arr_size - 1;
	for (size_t i = 0; i < quot_arr_size - 1; i++) {
		if (n1_vector[n1_top_index - i] < n2_vector) {
			quot_vector[quot_top_index - i] = 0;
			/* n1_vector[n1_top_index - i] is always a smaller number than n2_vector. Therefore, there will be no overflow. */
			n1_vector[n1_top_index - i - 1] += n1_vector[n1_top_index - i] * BC_VECTOR_BOUNDARY_NUM;
			continue;
		}
		quot_vector[quot_top_index - i] = n1_vector[n1_top_index - i] / n2_vector;
		n1_vector[n1_top_index - i] -= n2_vector * quot_vector[quot_top_index - i];
		n1_vector[n1_top_index - i - 1] += n1_vector[n1_top_index - i] * BC_VECTOR_BOUNDARY_NUM;
		n1_vector[n1_top_index - i] = 0;
	}
	/* last */
	quot_vector[0] = n1_vector[0] / n2_vector;
}

/*
 * Used when the number of elements in the n2 vector is 2 or more.
 */
static inline void bc_standard_div(
	BC_VECTOR *n1_vector, size_t n1_arr_size,
	BC_VECTOR *n2_vector, size_t n2_arr_size, size_t n2_len,
	BC_VECTOR *quot_vector, size_t quot_arr_size
) {
	size_t i, j;
	size_t n1_top_index = n1_arr_size - 1;
	size_t n2_top_index = n2_arr_size - 1;
	size_t quot_top_index = quot_arr_size - 1;

	BC_VECTOR div_carry = 0;

	/*
	 * Consider the error E between the actual quotient and the temporary quotient.
	 * For example, the quotient of 240 / 121 is 1, but if calculate the quotient only using the high-order digits (24 / 12),
	 * it will be 2. In this case, 2 is the temporary quotient. Here, the error E is 1.
	 * Also note that for example 2400000 / 120, there will be 5 divisions.
	 *
	 * Another example: Calculating 99900000000 / 10009999 with base 10000.
	 * The actual quotient is 9980, but if calculate it using only the high-order digits:
	 * 999 / 1000 = 0, Numbers that are not divisible are carried forward to the next division.
	 * 9990000 / 1000 = 9990 (Here, the temporary quotient is 9990 and the error E is 10.)
	 *
	 * If calculate the temporary quotient using only one array of n1 and n2, the error E can be larger than 1.
	 * In other words, in the restoring division, the count of additions for restore increases significantly.
	 *
	 * Therefore, in order to keep the error within 1 and to limit the number of additions required for restoration to
	 * at most one, adjust the number of high-order digits used to calculate the temporary quotient as follows.
	 * - Adjust the number of digits of n2 used in the calculation to BC_VECTOR_SIZE + 1 digit. The missing digits are
	 *   filled in from the next array element.
	 * - Add digits to n1 in the same way as the number of digits adjusted by n2.
	 *
	 * e.g.
	 * n1 = 123456780000
	 * n2 = 123456789
	 * base = 10000
	 * n1_arr = [0, 5678, 1234]
	 * n2_arr = [6789, 2345, 1]
	 * n1_top = 1234
	 * n2_top = 1
	 * n2_top_tmp = 12345 (+4 digits)
	 * n1_top_tmp = 12345678 (+4 digits)
	 * tmp_quot = n1_top_tmp / n2_top_tmp = 1000
	 * tmp_rem = -9000
	 * tmp_quot is too large, so tmp_quot--, tmp_rem += n2 (restoring)
	 * quot = 999
	 * rem = 123447789
	 *
	 * Details:
	 * Suppose that when calculating the temporary quotient, only the high-order elements of the BC_VECTOR array are used.
	 * At this time, n1 and n2 can be considered to be decomposed as follows. (Here, B = 10^b, any b ∈ ℕ, any k ∈ ℕ)
	 * n1 = n1_high * B^k + n1_low
	 * n2 = n2_high * B^k + n2_low
	 *
	 * The error E can be expressed by the following formula.
	 *
	 * E = (n1_high * B^k) / (n2_high * B^k) - (n1_high * B^k + n1_low) / (n2_high * B^k + n2_low)
	 * E = n1_high / n2_high - (n1_high * B^k + n1_low) / (n2_high * B^k + n2_low)
	 * E = (n1_high * (n2_high * B^k + n2_low) - (n1_high * B^k + n1_low) * n2_high) / (n2_high * (n2_high * B^k + n2_low))
	 * E = (n1_high * n2_low - n2_high * n1_low) / (n2_high * (n2_high * B^k + n2_low))
	 *
	 * Find the error MAX_E when the error E is maximum.
	 * First, n1_high, which only exists in the numerator, uses its maximum value. Considering carry-back,
	 * n1_high can be expressed as follows.
	 * n1_high = n2_high * B
	 * Also, n1_low is only present in the numerator, but since this is a subtraction, use the smallest possible value here, 0.
	 * n1_low = 0
	 *
	 * MAX_E = (n1_high * n2_low - n2_high * n1_low) / (n2_high * (n2_high * B^k + n2_low))
	 * MAX_E = (n2_high * B * n2_low) / (n2_high * (n2_high * B^k + n2_low))
	 *
	 * n2_low uses the maximum value.
	 * n2_low = B^k - 1
	 * MAX_E = (n2_high * B * n2_low) / (n2_high * (n2_high * B^k + n2_low))
	 * MAX_E = (n2_high * B * (B^k - 1)) / (n2_high * (n2_high * B^k + B^k - 1))
	 *
	 * n2_high uses the minimum value, but want to see how the number of digits affects the error, so represent
	 * the minimum value as:
	 * n2_high = 10^x (any x ∈ ℕ)
	 * Since B = 10^b, the formula becomes:
	 * MAX_E = (n2_high * B * (B^k - 1)) / (n2_high * (n2_high * B^k + B^k - 1))
	 * MAX_E = (10^x * 10^b * ((10^b)^k - 1)) / (10^x * (10^x * (10^b)^k + (10^b)^k - 1))
	 *
	 * Now let's make an approximation. Remove -1 from the numerator. Approximate the numerator to be
	 * large and the denominator to be small, such that MAX_E is less than this expression.
	 * Also, the denominator "(10^b)^k - 1" is never a negative value, so delete it.
	 *
	 * MAX_E = (10^x * 10^b * ((10^b)^k - 1)) / (10^x * (10^x * (10^b)^k + (10^b)^k - 1))
	 * MAX_E < (10^x * 10^b * (10^b)^k) / (10^x * 10^x * (10^b)^k)
	 * MAX_E < 10^b / 10^x
	 * MAX_E < 10^(b - x)
	 *
	 * Therefore, found that if the number of digits in base B and n2_high are equal, the error will be less
	 * than 1 regardless of the number of digits in the value of k.
	 *
	 * Here, B is BC_VECTOR_BOUNDARY_NUM, so adjust the number of digits in divider to be BC_VECTOR_SIZE + 1.
	 */
	size_t n2_top_digits = n2_len % BC_VECTOR_SIZE;
	if (n2_top_digits == 0) {
		n2_top_digits = BC_VECTOR_SIZE;
	}
	size_t high_part_shift = POW_10_LUT[BC_VECTOR_SIZE - n2_top_digits + 1];
	size_t low_part_shift = POW_10_LUT[n2_top_digits - 1];
	BC_VECTOR divider = n2_vector[n2_top_index] * high_part_shift + n2_vector[n2_top_index - 1] / low_part_shift;
	for (i = 0; i < quot_arr_size; i++) {
		BC_VECTOR divided = n1_vector[n1_top_index - i] * high_part_shift + n1_vector[n1_top_index - i - 1] / low_part_shift;

		/* If it is clear that n2 is greater in this loop, then the quotient is 0. */
		if (div_carry == 0 && divided < divider) {
			quot_vector[quot_top_index - i] = 0;
			div_carry = n1_vector[n1_top_index - i];
			n1_vector[n1_top_index - i] = 0;
			continue;
		}

		/*
		 * Determine the temporary quotient.
		 * "divided" is n1_high in the previous example. The maximum value of n1_high is n2_high * B,
		 * so here it is divider * BC_VECTOR_BOUNDARY_NUM.
		 * The number of digits in divider is BC_VECTOR_SIZE + 1, so even if divider is at its maximum value,
		 * it will never overflow here.
		 */
		divided += div_carry * BC_VECTOR_BOUNDARY_NUM * high_part_shift;
		BC_VECTOR quot_guess = divided / divider;

		/* For sub, add the remainder to the high-order digit */
		n1_vector[n1_top_index - i] += div_carry * BC_VECTOR_BOUNDARY_NUM;

		/*
		 * The temporary quotient can only have errors in the "too large" direction.
		 * So if the temporary quotient is 0 here, the quotient is 0.
		 */
		if (quot_guess == 0) {
			quot_vector[quot_top_index - i] = 0;
			div_carry = n1_vector[n1_top_index - i];
			n1_vector[n1_top_index - i] = 0;
			continue;
		}

		/* sub */
		BC_VECTOR sub;
		BC_VECTOR borrow = 0;
		BC_VECTOR *n1_calc_bottom = n1_vector + n1_arr_size - n2_arr_size - i;
		for (j = 0; j < n2_arr_size - 1; j++) {
			sub = n2_vector[j] * quot_guess + borrow;
			BC_VECTOR sub_low = sub % BC_VECTOR_BOUNDARY_NUM;
			borrow = sub / BC_VECTOR_BOUNDARY_NUM;

			if (n1_calc_bottom[j] >= sub_low) {
				n1_calc_bottom[j] -= sub_low;
			} else {
				n1_calc_bottom[j] += BC_VECTOR_BOUNDARY_NUM - sub_low;
				borrow++;
			}
		}
		/* last digit sub */
		sub = n2_vector[j] * quot_guess + borrow;
		bool neg_remainder = n1_calc_bottom[j] < sub;
		n1_calc_bottom[j] -= sub;

		/* If the temporary quotient is too large, add back n2 */
		BC_VECTOR carry = 0;
		if (neg_remainder) {
			quot_guess--;
			for (j = 0; j < n2_arr_size - 1; j++) {
				n1_calc_bottom[j] += n2_vector[j] + carry;
				carry = n1_calc_bottom[j] / BC_VECTOR_BOUNDARY_NUM;
				n1_calc_bottom[j] %= BC_VECTOR_BOUNDARY_NUM;
			}
			/* last add */
			n1_calc_bottom[j] += n2_vector[j] + carry;
		}

		quot_vector[quot_top_index - i] = quot_guess;
		div_carry = n1_vector[n1_top_index - i];
		n1_vector[n1_top_index - i] = 0;
	}
}

static void bc_do_div(char *n1, size_t n1_readable_len, size_t n1_bottom_extension, char *n2, size_t n2_len, bc_num *quot, size_t quot_len)
{
	size_t i;

	size_t n2_arr_size = (n2_len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t n1_arr_size = (n1_readable_len + n1_bottom_extension + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t quot_arr_size = n1_arr_size - n2_arr_size + 1;
	size_t quot_real_arr_size = MIN(quot_arr_size, (quot_len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE);

	BC_VECTOR *n1_vector = safe_emalloc(n1_arr_size + n2_arr_size + quot_arr_size, sizeof(BC_VECTOR), 0);
	BC_VECTOR *n2_vector = n1_vector + n1_arr_size;
	BC_VECTOR *quot_vector = n2_vector + n2_arr_size;

	/* Fill with zeros and convert as many vector elements as needed */
	size_t n1_vector_count = 0;
	while (n1_bottom_extension >= BC_VECTOR_SIZE) {
		n1_vector[n1_vector_count] = 0;
		n1_bottom_extension -= BC_VECTOR_SIZE;
		n1_vector_count++;
	}

	size_t n1_bottom_read_len = BC_VECTOR_SIZE - n1_bottom_extension;

	size_t base;
	size_t n1_read = 0;
	if (n1_bottom_read_len < BC_VECTOR_SIZE) {
		n1_read = MIN(n1_bottom_read_len, n1_readable_len);
		base = POW_10_LUT[n1_bottom_extension];
		n1_vector[n1_vector_count] = 0;
		for (i = 0; i < n1_read; i++) {
			n1_vector[n1_vector_count] += *n1 * base;
			base *= BASE;
			n1--;
		}
		n1_vector_count++;
	}

	/* Bulk convert n1 and n2 to vectors */
	if (n1_readable_len > n1_read) {
		bc_convert_to_vector(n1_vector + n1_vector_count, n1, n1_readable_len - n1_read);
	}
	bc_convert_to_vector(n2_vector, n2, n2_len);

	/* Do the divide */
	if (n2_arr_size == 1) {
		bc_fast_div(n1_vector, n1_arr_size, n2_vector[0], quot_vector, quot_arr_size);
	} else {
		bc_standard_div(n1_vector, n1_arr_size, n2_vector, n2_arr_size, n2_len, quot_vector, quot_arr_size);
	}

	/* Convert to bc_num */
	char *qptr = (*quot)->n_value;
	char *qend = qptr + quot_len - 1;

	for (i = 0; i < quot_real_arr_size - 1; i++) {
#if BC_VECTOR_SIZE == 4
		bc_write_bcd_representation(quot_vector[i], qend - 3);
		qend -= 4;
#else
		bc_write_bcd_representation(quot_vector[i] / 10000, qend - 7);
		bc_write_bcd_representation(quot_vector[i] % 10000, qend - 3);
		qend -= 8;
#endif
	}

	while (qend >= qptr) {
		*qend-- = quot_vector[i] % BASE;
		quot_vector[i] /= BASE;
	}

	efree(n1_vector);
}

bool bc_divide(bc_num n1, bc_num n2, bc_num *quot, size_t scale)
{
	/* divide by zero */
	if (bc_is_zero(n2)) {
		return false;
	}

	bc_free_num(quot);

	/* If n1 is zero, the quotient is always zero. */
	if (bc_is_zero(n1)) {
		*quot = bc_copy_num(BCG(_zero_));
		return true;
	}

	char *n1ptr = n1->n_value;
	char *n1end = n1ptr + n1->n_len + n1->n_scale - 1;
	size_t n1_len = n1->n_len;
	size_t n1_scale = n1->n_scale;

	char *n2ptr = n2->n_value;
	char *n2end = n2ptr + n2->n_len + n2->n_scale - 1;
	size_t n2_len = n2->n_len;
	size_t n2_scale = n2->n_scale;
	size_t n2_int_right_zeros = 0;

	/* remove n2 trailing zeros */
	while (*n2end == 0 && n2_scale > 0) {
		n2end--;
		n2_scale--;
	}
	while (*n2end == 0) {
		n2end--;
		n2_int_right_zeros++;
	}

	if (*n1ptr == 0 && n1_len == 1) {
		n1ptr++;
		n1_len = 0;
	}

	size_t n1_top_extension = 0;
	size_t n1_bottom_extension = 0;
	if (n2_scale > 0) {
		/*
		 * e.g. n2_scale = 4
		 * n2 = .0002, to be 2 or n2 = 200.001, to be 200001
		 * n1 = .03, to be 300 or n1 = .000003, to be .03
		 * n1 may become longer than the original data length due to the addition of
		 * trailing zeros in the integer part.
		 */
		n1_len += n2_scale;
		n1_bottom_extension = n1_scale < n2_scale ? n2_scale - n1_scale : 0;
		n1_scale = n1_scale > n2_scale ? n1_scale - n2_scale : 0;
		n2_len += n2_scale;
		n2_scale = 0;
	} else if (n2_int_right_zeros > 0) {
		/*
		 * e.g. n2_int_right_zeros = 4
		 * n2 = 2000, to be 2
		 * n1 = 30, to be .03 or n1 = 30000, to be 30
		 * Also, n1 may become longer than the original data length due to the addition of
		 * leading zeros in the fractional part.
		 */
		n1_top_extension = n1_len < n2_int_right_zeros ? n2_int_right_zeros - n1_len : 0;
		n1_len = n1_len > n2_int_right_zeros ? n1_len - n2_int_right_zeros : 0;
		n1_scale += n2_int_right_zeros;
		n2_len -= n2_int_right_zeros;
		n2_scale = 0;
	}

	/* remove n1 leading zeros */
	while (*n1ptr == 0 && n1_len > 0) {
		n1ptr++;
		n1_len--;
	}
	/* remove n2 leading zeros */
	while (*n2ptr == 0) {
		n2ptr++;
		n2_len--;
	}

	/* Considering the scale specification, the quotient is always 0 if this condition is met */
	if (n2_len > n1_len + scale) {
		*quot = bc_copy_num(BCG(_zero_));
		return true;
	}

	/* set scale to n1 */
	if (n1_scale > scale) {
		size_t scale_diff = n1_scale - scale;
		if (n1_bottom_extension > scale_diff) {
			n1_bottom_extension -= scale_diff;
		} else {
			n1_bottom_extension = 0;
			n1end -= scale_diff - n1_bottom_extension;
		}
	} else {
		n1_bottom_extension += scale - n1_scale;
	}
	n1_scale = scale;

	/* Length of n1 data that can be read */
	size_t n1_readable_len = n1end - n1ptr + 1;

	/* If n2 is 1 here, return the result of adjusting the decimal point position of n1. */
	if (n2_len == 1 && *n2ptr == 1) {
		if (n1_len == 0) {
			n1_len = 1;
			n1_top_extension++;
		}
		size_t quot_scale = n1_scale > n1_bottom_extension ? n1_scale - n1_bottom_extension : 0;
		n1_bottom_extension = n1_scale < n1_bottom_extension ? n1_bottom_extension - n1_scale : 0;

		*quot = bc_new_num_nonzeroed(n1_len, quot_scale);
		char *qptr = (*quot)->n_value;
		for (size_t i = 0; i < n1_top_extension; i++) {
			*qptr++ = 0;
		}
		memcpy(qptr, n1ptr, n1_readable_len);
		qptr += n1_readable_len;
		for (size_t i = 0; i < n1_bottom_extension; i++) {
			*qptr++ = 0;
		}
		(*quot)->n_sign = n1->n_sign == n2->n_sign ? PLUS : MINUS;
		return true;
	}

	size_t quot_full_len;
	if (n2_len > n1_len) {
		*quot = bc_new_num_nonzeroed(1, scale);
		quot_full_len = 1 + scale;
	} else {
		*quot = bc_new_num_nonzeroed(n1_len - n2_len + 1, scale);
		quot_full_len = n1_len - n2_len + 1 + scale;
	}

	/* do divide */
	bc_do_div(n1end, n1_readable_len, n1_bottom_extension, n2end, n2_len, quot, quot_full_len);
	(*quot)->n_sign = n1->n_sign == n2->n_sign ? PLUS : MINUS;
	_bc_rm_leading_zeros(*quot);

	return true;
}
