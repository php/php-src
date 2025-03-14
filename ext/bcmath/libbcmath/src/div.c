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

/*
 * This function should be used when the divisor is not split into multiple chunks, i.e. when the size of the array is one.
 * This is because the algorithm can be simplified.
 * This function is therefore an optimized version of bc_standard_div().
 */
static inline void bc_fast_div(
	BC_VECTOR *numerator_vectors, size_t numerator_arr_size, BC_VECTOR divisor_vector, BC_VECTOR *quot_vectors, size_t quot_arr_size)
{
	size_t numerator_top_index = numerator_arr_size - 1;
	size_t quot_top_index = quot_arr_size - 1;
	for (size_t i = 0; i < quot_top_index; i++) {
		if (numerator_vectors[numerator_top_index - i] < divisor_vector) {
			quot_vectors[quot_top_index - i] = 0;
			/* numerator_vectors[numerator_top_index - i] < divisor_vector, so there will be no overflow. */
			numerator_vectors[numerator_top_index - i - 1] += numerator_vectors[numerator_top_index - i] * BC_VECTOR_BOUNDARY_NUM;
			continue;
		}
		quot_vectors[quot_top_index - i] = numerator_vectors[numerator_top_index - i] / divisor_vector;
		numerator_vectors[numerator_top_index - i] -= divisor_vector * quot_vectors[quot_top_index - i];
		numerator_vectors[numerator_top_index - i - 1] += numerator_vectors[numerator_top_index - i] * BC_VECTOR_BOUNDARY_NUM;
		numerator_vectors[numerator_top_index - i] = 0;
	}
	/* last */
	quot_vectors[0] = numerator_vectors[0] / divisor_vector;
	numerator_vectors[0] -= divisor_vector * quot_vectors[0];
}

/*
 * Used when the divisor is split into 2 or more chunks.
 * This use the restoring division algorithm.
 * https://en.wikipedia.org/wiki/Division_algorithm#Restoring_division
 */
static inline void bc_standard_div(
	BC_VECTOR *numerator_vectors, size_t numerator_arr_size,
	const BC_VECTOR *divisor_vectors, size_t divisor_arr_size, size_t divisor_len,
	BC_VECTOR *quot_vectors, size_t quot_arr_size
) {
	size_t numerator_top_index = numerator_arr_size - 1;
	size_t divisor_top_index = divisor_arr_size - 1;
	size_t quot_top_index = quot_arr_size - 1;

	BC_VECTOR div_carry = 0;

	/*
	 * Errors might occur between the true quotient and the temporary quotient calculated using only the high order digits.
	 * For example, the true quotient of 240 / 121 is 1.
	 * However, if it is calculated using only the high-order digits (24 / 12), we would get 2.
	 * We refer to this value of 2 as the temporary quotient.
	 * We also define E to be error between the true quotient and the temporary quotient,
	 * which in this case, is 1.
	 *
	 * Another example: Calculating 999_0000_0000 / 1000_9999 with base 10000.
	 * The true quotient is 9980, but if it is calculated using only the high-order digits (999 / 1000), we would get 0
	 * If the temporary quotient is 0, we need to carry the digits to the next division, which is 999_0000 / 1000.
	 * The new temporary quotient we get is 9990, with error E = 10.
	 *
	 * Because we use the restoring division we need to perform E restorations, which can be significant if E is large.
	 *
	 * Therefore, for the error E to be at most 1 we adjust the number of high-order digits used to calculate the temporary quotient as follows:
	 * - Include BC_VECTOR_SIZE + 1 digits of the divisor used in the calculation of the temporary quotient.
	      The missing digits are filled in from the next array element.
	 * - Adjust the number of digits in the numerator similarly to what was done for the divisor.
	 *
	 * e.g.
	 * numerator = 123456780000
	 * divisor = 123456789
	 * base = 10000
	 * numerator_arr = [0, 5678, 1234]
	 * divisor_arr = [6789, 2345, 1]
	 * numerator_top = 1234
	 * divisor_top = 1
	 * divisor_top_tmp = 12345 (+4 digits)
	 * numerator_top_tmp = 12345678 (+4 digits)
	 * tmp_quot = numerator_top_tmp / divisor_top_tmp = 1000
	 * tmp_rem = -9000
	 * tmp_quot is too large, so tmp_quot--, tmp_rem += divisor (restoring)
	 * quot = 999
	 * rem = 123447789
	 *
	 * Details:
	 * Suppose that when calculating the temporary quotient, only the high-order elements of the BC_VECTOR array are used.
	 * At this time, numerator and divisor can be considered to be decomposed as follows. (Here, B = 10^b, any b ∈ ℕ, any k ∈ ℕ)
	 * numerator = numerator_high * B^k + numerator_low
	 * divisor = divisor_high * B^k + divisor_low
	 *
	 * The error E can be expressed by the following formula.
	 *
	 * E = (numerator_high * B^k) / (divisor_high * B^k) - (numerator_high * B^k + numerator_low) / (divisor_high * B^k + divisor_low)
	 * <=> E = numerator_high / divisor_high - (numerator_high * B^k + numerator_low) / (divisor_high * B^k + divisor_low)
	 * <=> E = (numerator_high * (divisor_high * B^k + divisor_low) - (numerator_high * B^k + numerator_low) * divisor_high) / (divisor_high * (divisor_high * B^k + divisor_low))
	 * <=> E = (numerator_high * divisor_low - divisor_high * numerator_low) / (divisor_high * (divisor_high * B^k + divisor_low))
	 *
	 * Find the error MAX_E when the error E is maximum (0 <= E <= MAX_E).
	 * First, numerator_high, which only exists in the numerator, uses its maximum value.
	 * Considering carry-back, numerator_high can be expressed as follows.
	 * numerator_high = divisor_high * B
	 * Also, numerator_low is only present in the numerator, but since this is a subtraction, use the smallest possible value here, 0.
	 * numerator_low = 0
	 *
	 * MAX_E = (numerator_high * divisor_low - divisor_high * numerator_low) / (divisor_high * (divisor_high * B^k + divisor_low))
	 * <=> MAX_E = (divisor_high * B * divisor_low) / (divisor_high * (divisor_high * B^k + divisor_low))
	 *
	 * divisor_low uses the maximum value.
	 * divisor_low = B^k - 1
	 * MAX_E = (divisor_high * B * divisor_low) / (divisor_high * (divisor_high * B^k + divisor_low))
	 * <=> MAX_E = (divisor_high * B * (B^k - 1)) / (divisor_high * (divisor_high * B^k + B^k - 1))
	 *
	 * divisor_high uses the minimum value, but want to see how the number of digits affects the error, so represent
	 * the minimum value as:
	 * divisor_high = 10^x (any x ∈ ℕ)
	 * Since B = 10^b, the formula becomes:
	 * MAX_E = (divisor_high * B * (B^k - 1)) / (divisor_high * (divisor_high * B^k + B^k - 1))
	 * <=> MAX_E = (10^x * 10^b * ((10^b)^k - 1)) / (10^x * (10^x * (10^b)^k + (10^b)^k - 1))
	 *
	 * Now let's make an approximation. Remove -1 from the numerator. Approximate the numerator to be
	 * large and the denominator to be small, such that MAX_E is less than this expression.
	 * Also, the denominator "(10^b)^k - 1" is never a negative value, so delete it.
	 *
	 * MAX_E = (10^x * 10^b * ((10^b)^k - 1)) / (10^x * (10^x * (10^b)^k + (10^b)^k - 1))
	 * MAX_E < (10^x * 10^b * (10^b)^k) / (10^x * 10^x * (10^b)^k)
	 * <=> MAX_E < 10^b / 10^x
	 * <=> MAX_E < 10^(b - x)
	 *
	 * Therefore, found that if the number of digits in base B and divisor_high are equal, the error will be less
	 * than 1 regardless of the number of digits in the value of k.
	 *
	 * Here, B is BC_VECTOR_BOUNDARY_NUM, so adjust the number of digits in high part of divisor to be BC_VECTOR_SIZE + 1.
	 */
	/* the number of usable digits, thus divisor_len % BC_VECTOR_SIZE == 0 implies we have BC_VECTOR_SIZE usable digits */
	size_t divisor_top_digits = divisor_len % BC_VECTOR_SIZE;
	if (divisor_top_digits == 0) {
		divisor_top_digits = BC_VECTOR_SIZE;
	}

	size_t high_part_shift = BC_POW_10_LUT[BC_VECTOR_SIZE - divisor_top_digits + 1];
	size_t low_part_shift = BC_POW_10_LUT[divisor_top_digits - 1];
	BC_VECTOR divisor_high_part = divisor_vectors[divisor_top_index] * high_part_shift + divisor_vectors[divisor_top_index - 1] / low_part_shift;
	for (size_t i = 0; i < quot_arr_size; i++) {
		BC_VECTOR numerator_high_part = numerator_vectors[numerator_top_index - i] * high_part_shift + numerator_vectors[numerator_top_index - i - 1] / low_part_shift;

		/*
		 * Determine the temporary quotient.
		 * The maximum value of numerator_high is divisor_high * B in the previous example, so here numerator_high_part is
		 * divisor_high_part * BC_VECTOR_BOUNDARY_NUM.
		 * The number of digits in divisor_high_part is BC_VECTOR_SIZE + 1, so even if divisor_high_part is at its maximum value,
		 * it will never overflow here.
		 */
		numerator_high_part += div_carry * BC_VECTOR_BOUNDARY_NUM * high_part_shift;

		/* numerator_high_part < divisor_high_part => quotient == 0 */
		if (numerator_high_part < divisor_high_part) {
			quot_vectors[quot_top_index - i] = 0;
			div_carry = numerator_vectors[numerator_top_index - i];
			numerator_vectors[numerator_top_index - i] = 0;
			continue;
		}

		BC_VECTOR quot_guess = numerator_high_part / divisor_high_part;

		/* For sub, add the remainder to the high-order digit */
		numerator_vectors[numerator_top_index - i] += div_carry * BC_VECTOR_BOUNDARY_NUM;

		/*
		 * It is impossible for the temporary quotient to underestimate the true quotient.
		 * Therefore a temporary quotient of 0 implies the true quotient is also 0.
		 */
		if (quot_guess == 0) {
			quot_vectors[quot_top_index - i] = 0;
			div_carry = numerator_vectors[numerator_top_index - i];
			numerator_vectors[numerator_top_index - i] = 0;
			continue;
		}

		/* sub */
		BC_VECTOR sub;
		BC_VECTOR borrow = 0;
		BC_VECTOR *numerator_calc_bottom = numerator_vectors + numerator_arr_size - divisor_arr_size - i;
		size_t j;
		for (j = 0; j < divisor_arr_size - 1; j++) {
			sub = divisor_vectors[j] * quot_guess + borrow;
			BC_VECTOR sub_low = sub % BC_VECTOR_BOUNDARY_NUM;
			borrow = sub / BC_VECTOR_BOUNDARY_NUM;

			if (numerator_calc_bottom[j] >= sub_low) {
				numerator_calc_bottom[j] -= sub_low;
			} else {
				numerator_calc_bottom[j] += BC_VECTOR_BOUNDARY_NUM - sub_low;
				borrow++;
			}
		}
		/* last digit sub */
		sub = divisor_vectors[j] * quot_guess + borrow;
		bool neg_remainder = numerator_calc_bottom[j] < sub;
		numerator_calc_bottom[j] -= sub;

		/* If the temporary quotient is too large, add back divisor */
		BC_VECTOR carry = 0;
		if (neg_remainder) {
			quot_guess--;
			for (j = 0; j < divisor_arr_size - 1; j++) {
				numerator_calc_bottom[j] += divisor_vectors[j] + carry;
				carry = numerator_calc_bottom[j] / BC_VECTOR_BOUNDARY_NUM;
				numerator_calc_bottom[j] %= BC_VECTOR_BOUNDARY_NUM;
			}
			/* last add */
			numerator_calc_bottom[j] += divisor_vectors[j] + carry;
		}

		quot_vectors[quot_top_index - i] = quot_guess;
		div_carry = numerator_vectors[numerator_top_index - i];
		numerator_vectors[numerator_top_index - i] = 0;
	}
	numerator_vectors[numerator_top_index - quot_arr_size + 1] = div_carry;
}

static void bc_do_div(
	const char *numerator, size_t numerator_size, size_t numerator_readable_size,
	const char *divisor, size_t divisor_size,
	bc_num *quot, size_t quot_size,
	bc_num *rem, size_t rem_over_size, size_t rem_write_size,
	bool use_quot, bool use_rem
) {
	size_t numerator_arr_size = BC_ARR_SIZE_FROM_LEN(numerator_size);
	size_t divisor_arr_size = BC_ARR_SIZE_FROM_LEN(divisor_size);
	size_t quot_arr_size = numerator_arr_size - divisor_arr_size + 1;
	size_t quot_real_arr_size = MIN(quot_arr_size, BC_ARR_SIZE_FROM_LEN(quot_size));

	BC_VECTOR stack_vectors[BC_STACK_VECTOR_SIZE];
	size_t allocation_arr_size = numerator_arr_size + divisor_arr_size + quot_arr_size;

	BC_VECTOR *numerator_vectors;
	if (allocation_arr_size <= BC_STACK_VECTOR_SIZE) {
		numerator_vectors = stack_vectors;
	} else {
		numerator_vectors = safe_emalloc(allocation_arr_size, sizeof(BC_VECTOR), 0);
	}
	BC_VECTOR *divisor_vectors = numerator_vectors + numerator_arr_size;
	BC_VECTOR *quot_vectors = divisor_vectors + divisor_arr_size;

	size_t numerator_extension = numerator_size > numerator_readable_size ? numerator_size - numerator_readable_size : 0;

	/* Bulk convert numerator and divisor to vectors */
	size_t numerator_use_size = numerator_size - numerator_extension;
	const char *numerator_end = numerator + numerator_use_size - 1;
	bc_convert_to_vector_with_zero_pad(numerator_vectors, numerator_end, numerator_use_size, numerator_extension);

	const char *divisor_end = divisor + divisor_size - 1;
	bc_convert_to_vector(divisor_vectors, divisor_end, divisor_size);

	/* Do the division */
	if (divisor_arr_size == 1) {
		bc_fast_div(numerator_vectors, numerator_arr_size, divisor_vectors[0], quot_vectors, quot_arr_size);
	} else {
		bc_standard_div(numerator_vectors, numerator_arr_size, divisor_vectors, divisor_arr_size, divisor_size, quot_vectors, quot_arr_size);
	}

	/* Convert to bc_num */
	if (use_quot) {
		char *qptr = (*quot)->n_value;
		char *qend = qptr + (*quot)->n_len + (*quot)->n_scale - 1;
		bc_convert_vector_to_char(quot_vectors, qptr, qend, quot_real_arr_size);
	}
	if (use_rem) {
		char *rptr = (*rem)->n_value;
		char *rend = rptr + rem_write_size - 1;

		size_t rem_arr_size = (rem_write_size + rem_over_size + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
		if (UNEXPECTED(rem_arr_size > numerator_arr_size)) {
			/* If numerator_arr_size is exceeded because the integer part is zero */
			rem_arr_size = numerator_arr_size;
			*rptr = 0;
		}
		BC_VECTOR *rem_vectors = numerator_vectors;

		if (rem_over_size > 0) {
			bc_convert_vector_to_char_with_skip(rem_vectors, rptr, rend, rem_arr_size, rem_over_size);
		} else {
			bc_convert_vector_to_char(rem_vectors, rptr, rend, rem_arr_size);
		}
	}

	if (allocation_arr_size > BC_STACK_VECTOR_SIZE) {
		efree(numerator_vectors);
	}
}

static inline void bc_divide_copy_numerator(bc_num numerator, bc_num *num, size_t scale)
{
	scale = MIN(numerator->n_scale, scale);
	*num = bc_new_num_nonzeroed(numerator->n_len, scale);
	memcpy((*num)->n_value, numerator->n_value, numerator->n_len + scale);
}

static inline void bc_divide_by_one(
	bc_num numerator, bc_num divisor, bc_num *quot, bc_num *rem,
	size_t quot_scale, size_t rem_scale, bool use_quot, bool use_rem)
{
	if (use_quot) {
		bc_divide_copy_numerator(numerator, quot, quot_scale);
		(*quot)->n_sign = numerator->n_sign == divisor->n_sign ? PLUS : MINUS;
	}
	if (use_rem) {
		/* When dividing by 1, the integer part of rem is always 0. */
		rem_scale = MIN(numerator->n_scale, rem_scale);
		if (rem_scale == 0) {
			*rem = bc_copy_num(BCG(_zero_));
		} else {
			*rem = bc_new_num_nonzeroed(1, rem_scale); /* 1 is for 0 */
			(*rem)->n_value[0] = 0;
			/* copy fractional part */
			memcpy((*rem)->n_value + 1, numerator->n_value + numerator->n_len, rem_scale);
			if (bc_is_zero(*rem)) {
				(*rem)->n_sign = PLUS;
				(*rem)->n_scale = 0;
			} else {
				(*rem)->n_sign = numerator->n_sign;
			}
		}
	}
}

static inline void bc_divide_by_pow_10(
	const char *numeratorptr, size_t numerator_readable_size, size_t numerator_leading_zeros,
	bc_num *quot, size_t quot_size, size_t quot_scale, bool use_quot,
	bc_num *rem, size_t rem_size, bool use_rem,
	size_t numerator_rem_len_diff)
{
	if (use_quot) {
		char *qptr = (*quot)->n_value;
		for (size_t i = quot_size; i <= quot_scale; i++) {
			*qptr++ = 0;
		}

		size_t numerator_use_size = quot_size > numerator_readable_size ? numerator_readable_size : quot_size;
		memcpy(qptr, numeratorptr, numerator_use_size);
		qptr += numerator_use_size;

		if (numerator_use_size < (*quot)->n_len) {
			/* e.g. 12.3 / 0.01 <=> 1230  */
			for (size_t i = numerator_use_size; i < (*quot)->n_len; i++) {
				*qptr++ = 0;
			}
			(*quot)->n_scale = 0;
		} else {
			char *qend = (*quot)->n_value + (*quot)->n_len + (*quot)->n_scale;
			(*quot)->n_scale -= qend - qptr;
		}
	}
	if (use_rem) {
		size_t rem_leading_zeros = numerator_leading_zeros + quot_size - numerator_rem_len_diff;
		if (rem_size <= rem_leading_zeros) {
			bc_free_num(rem);
			*rem = bc_copy_num(BCG(_zero_));
			return;
		}
		/* The values after this have already been copied, so just need to set them to 0. */
		for (size_t i = 0; i < rem_leading_zeros; i++) {
			(*rem)->n_value[i] = 0;
		}
		_bc_rm_leading_zeros(*rem);
		if (bc_is_zero(*rem)) {
			(*rem)->n_sign = PLUS;
			(*rem)->n_scale = 0;
		}
	}
}

bool bc_divide_ex(bc_num numerator, bc_num divisor, bc_num *quot, bc_num *rem, size_t scale, bool use_quot, bool use_rem)
{
	/* divide by zero */
	if (bc_is_zero(divisor)) {
		return false;
	}

	size_t quot_scale = 0;
	size_t rem_scale = 0;
	if (use_quot) {
		bc_free_num(quot);
	}
	if (use_rem) {
		bc_free_num(rem);
		rem_scale = scale;
	} else {
		quot_scale = scale;
	}

	/* If numerator is zero, the quotient is always zero. */
	if (bc_is_zero(numerator)) {
		goto quot_zero;
	}

	/* If divisor is 1 / -1, the quotient's n_value is equal to numerator's n_value. */
	if (_bc_do_compare(divisor, BCG(_one_), divisor->n_scale, false) == BCMATH_EQUAL) {
		bc_divide_by_one(numerator, divisor, quot, rem, quot_scale, rem_scale, use_quot, use_rem);
		return true;
	}

	const char *numeratorptr = numerator->n_value;
	size_t numerator_size = numerator->n_len + quot_scale + divisor->n_scale;

	const char *divisorptr = divisor->n_value;
	size_t divisor_size = divisor->n_len + divisor->n_scale;

	/* check and remove numerator leading zeros */
	size_t numerator_leading_zeros = 0;
	while (*numeratorptr == 0) {
		numeratorptr++;
		numerator_leading_zeros++;
		if (numerator_leading_zeros == numerator_size) {
			goto quot_zero;
		}
	}
	numerator_size -= numerator_leading_zeros;

	/* check and remove divisor leading zeros */
	size_t divisor_leading_zeros = 0;
	while (*divisorptr == 0) {
		divisorptr++;
		divisor_leading_zeros++;
	}
	divisor_size -= divisor_leading_zeros;

	if (divisor_size > numerator_size) {
		goto quot_zero;
	}

	/* check and remove divisor trailing zeros. The divisor is not 0, so leave only one digit */
	size_t divisor_trailing_zeros = 0;
	for (size_t i = divisor_size - 1; i > 0; i--) {
		if (divisorptr[i] != 0) {
			break;
		}
		divisor_trailing_zeros++;
	}
	divisor_size -= divisor_trailing_zeros;
	numerator_size -= divisor_trailing_zeros;

	size_t quot_size = numerator_size - divisor_size + 1; /* numerator_size >= divisor_size */
	if (use_quot) {
		if (quot_size > quot_scale) {
			*quot = bc_new_num_nonzeroed(quot_size - quot_scale, quot_scale);
		} else {
			*quot = bc_new_num_nonzeroed(1, quot_scale); /* 1 is for 0 */
		}
		(*quot)->n_sign = numerator->n_sign == divisor->n_sign ? PLUS : MINUS;
	}

	/**
	 * If the calculation uses more digits than the scale of rem, writing the vector directly to rem
	 * will exceed the size, so calculate the excess size in advance.
	 */
	size_t rem_over_size = 0;

	/**
	 * Conversely, there are cases where the vector does not fill the rem size.
	 * In this case, the size to be written is calculated in advance to determine the start position for writing to rem.
	 */
	size_t rem_write_size = 0;

	size_t rem_size = 0;
	size_t numerator_rem_len_diff = 0;
	if (use_rem) {
		size_t divisor_int_size = divisor->n_len > divisor_leading_zeros ? divisor->n_len - divisor_leading_zeros : 0;
		size_t divisor_frac_size = divisor->n_scale > divisor_trailing_zeros ? divisor->n_scale - divisor_trailing_zeros : 0;
		rem_scale = MIN(MAX(numerator->n_scale, divisor_frac_size), rem_scale);

		*rem = bc_new_num_nonzeroed(divisor_int_size > 0 ? divisor_int_size : 1, rem_scale); // 1 is for 0
		(*rem)->n_sign = numerator->n_sign;

		if (divisor_frac_size > rem_scale) {
			rem_over_size = divisor_frac_size - rem_scale;
			rem_write_size = (*rem)->n_len + rem_scale;
		} else {
			if (divisor_frac_size > 0) {
				rem_write_size = (*rem)->n_len + divisor_frac_size;
			} else {
				/* e.g. 123 % 30 */
				rem_write_size = (*rem)->n_len - (divisor_trailing_zeros - divisor->n_scale);
			}
		}

		rem_size = (*rem)->n_len + (*rem)->n_scale;
		if (rem_size > rem_write_size) {
			size_t copy_size = rem_size - rem_write_size;
			numerator_rem_len_diff = numerator->n_len - (*rem)->n_len;
			memcpy((*rem)->n_value + rem_write_size, numerator->n_value + rem_write_size + numerator_rem_len_diff, copy_size);
		}
	}

	/* Size that can be read from numeratorptr */
	size_t numerator_readable_size = numerator->n_len + numerator->n_scale - numerator_leading_zeros;

	/* If divisor is 1 here, return the result of adjusting the decimal point position of numerator. */
	if (divisor_size == 1 && *divisorptr == 1) {
		bc_divide_by_pow_10(
			numeratorptr, numerator_readable_size, numerator_leading_zeros,
			quot, quot_size, quot_scale, use_quot,
			rem, rem_size, use_rem, numerator_rem_len_diff
		);
		return true;
	}

	/* do divide */
	bc_do_div(
		numeratorptr, numerator_size, numerator_readable_size,
		divisorptr, divisor_size,
		quot, quot_size,
		rem, rem_over_size, rem_write_size,
		use_quot, use_rem
	);

	if (use_quot) {
		_bc_rm_leading_zeros(*quot);
		if (bc_is_zero(*quot)) {
			(*quot)->n_sign = PLUS;
			(*quot)->n_scale = 0;
		}
	}
	if (use_rem) {
		_bc_rm_leading_zeros(*rem);
		if (bc_is_zero(*rem)) {
			(*rem)->n_sign = PLUS;
			(*rem)->n_scale = 0;
		}
	}
	return true;

quot_zero:
	if (use_quot) {
		*quot = bc_copy_num(BCG(_zero_));
	}
	if (use_rem) {
		bc_divide_copy_numerator(numerator, rem, rem_scale);
		(*rem)->n_sign = numerator->n_sign;
	}
	return true;
}
