/* raise.c: bcmath library file. */
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
#include "private.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

static inline size_t bc_multiply_vector_ex(
	BC_VECTOR **n1_vector, size_t n1_arr_size, BC_VECTOR *n2_vector, size_t n2_arr_size, BC_VECTOR **result_vector)
{
	size_t result_arr_size = n1_arr_size + n2_arr_size;
	bc_multiply_vector(*n1_vector, n1_arr_size, n2_vector, n2_arr_size, *result_vector, result_arr_size);

	/* Eliminate extra zeros because they increase the number of calculations. */
	while ((*result_vector)[result_arr_size - 1] == 0) {
		result_arr_size--;
	}

	/* Swap n1_vector and result_vector. */
	BC_VECTOR *tmp = *n1_vector;
	*n1_vector = *result_vector;
	*result_vector = tmp;

	return result_arr_size;
}

static inline size_t bc_square_vector_ex(BC_VECTOR **base_vector, size_t base_arr_size, BC_VECTOR **result_vector)
{
	return bc_multiply_vector_ex(base_vector, base_arr_size, *base_vector, base_arr_size, result_vector);
}

/* Use "exponentiation by squaring". This is the fast path when the results are small. */
static inline bc_num bc_fast_raise(
	const char *base_end, long exponent, size_t base_len, size_t power_len, size_t power_scale, size_t power_full_len)
{
	BC_VECTOR base_vector = 0;

	/* Convert to BC_VECTOR[] */
	bc_convert_to_vector(&base_vector, base_end, base_len);

	while ((exponent & 1) == 0) {
		base_vector *= base_vector;
		exponent >>= 1;
	}

	/* copy base to power */
	BC_VECTOR power_vector = base_vector;
	exponent >>= 1;

	while (exponent > 0) {
		base_vector *= base_vector;
		if ((exponent & 1) == 1) {
			power_vector *= base_vector;
		}
		exponent >>= 1;
	}

	bc_num power = bc_new_num_nonzeroed(power_len, power_scale);
	char *pptr = power->n_value;
	char *pend = pptr + power_full_len - 1;

	while (pend >= pptr) {
		*pend-- = power_vector % BASE;
		power_vector /= BASE;
	}
	return power;
}

/* Use "exponentiation by squaring". This is the standard path. */
static bc_num bc_standard_raise(
	const char *base_ptr, const char *base_end, long exponent, size_t base_len, size_t power_scale)
{
	/* Remove the leading zeros as they will be filled in later. */
	while (*base_ptr == 0) {
		base_ptr++;
		base_len--;
	}

	size_t base_arr_size = BC_ARR_SIZE_FROM_LEN(base_len);
	/* Since it is guaranteed that base_len * exponent does not overflow, there is no possibility of overflow here. */
	size_t max_power_arr_size =	base_arr_size * exponent;

	/* The allocated memory area is reused on a rotational basis, so the same size is required. */
	BC_VECTOR *buf = safe_emalloc(max_power_arr_size, sizeof(BC_VECTOR) * 3, 0);
	BC_VECTOR *base_vector = buf;
	BC_VECTOR *power_vector = base_vector + max_power_arr_size;
	BC_VECTOR *tmp_result_vector = power_vector + max_power_arr_size;

	/* Convert to BC_VECTOR[] */
	bc_convert_to_vector(base_vector, base_end, base_len);

	while ((exponent & 1) == 0) {
		base_arr_size = bc_square_vector_ex(&base_vector, base_arr_size, &tmp_result_vector);
		exponent >>= 1;
	}

	/* copy base to power */
	size_t power_arr_size = base_arr_size;
	for (size_t i = 0; i < base_arr_size; i++) {
		power_vector[i] = base_vector[i];
	}
	exponent >>= 1;

	while (exponent > 0) {
		base_arr_size = bc_square_vector_ex(&base_vector, base_arr_size, &tmp_result_vector);
		if ((exponent & 1) == 1) {
			power_arr_size = bc_multiply_vector_ex(&power_vector, power_arr_size, base_vector, base_arr_size, &tmp_result_vector);
		}
		exponent >>= 1;
	}

	/* Convert to bc_num */
	size_t power_leading_zeros = 0;
	size_t power_len;
	size_t power_full_len = power_arr_size * BC_VECTOR_SIZE;
	if (power_full_len > power_scale) {
		power_len = power_full_len - power_scale;
	} else {
		power_len = 1;
		power_leading_zeros = power_scale - power_full_len + 1;
		power_full_len = power_scale + 1;
	}
	bc_num power = bc_new_num_nonzeroed(power_len, power_scale);

	char *pptr = power->n_value;
	char *pend = pptr + power_full_len - 1;

	/* Pad with leading zeros if necessary. */
	memset(pptr, 0, power_leading_zeros);
	pptr += power_leading_zeros;

	bc_convert_vector_to_char(power_vector, pptr, pend, power_arr_size);

	efree(buf);

	return power;
}

/* Raise "base" to the "exponent" power.  The result is placed in RESULT.
   Maximum exponent is LONG_MAX.  If a "exponent" is not an integer,
   only the integer part is used.  */
bc_raise_status bc_raise(bc_num base, long exponent, bc_num *result, size_t scale) {
	size_t rscale;
	bool is_neg;

	/* Special case if exponent is a zero. */
	if (exponent == 0) {
		bc_free_num (result);
		*result = bc_copy_num(BCG(_one_));
		return BC_RAISE_STATUS_OK;
	}

	/* Other initializations. */
	if (exponent < 0) {
		is_neg = true;
		exponent = -exponent;
		rscale = scale;
	} else {
		is_neg = false;
		rscale = MIN (base->n_scale * exponent, MAX(scale, base->n_scale));
	}

	if (bc_is_zero(base)) {
		/* If the exponent is negative, it divides by 0 */
		return is_neg ? BC_RAISE_STATUS_DIVIDE_BY_ZERO : BC_RAISE_STATUS_OK;
	}

	/* check overflow */
	if (UNEXPECTED(base->n_len > SIZE_MAX / exponent)) {
		return BC_RAISE_STATUS_LEN_IS_OVERFLOW;
	}
	if (UNEXPECTED(base->n_scale > SIZE_MAX / exponent)) {
		return BC_RAISE_STATUS_SCALE_IS_OVERFLOW;
	}

	size_t base_len = base->n_len + base->n_scale;
	size_t power_len = base->n_len * exponent;
	size_t power_scale = base->n_scale * exponent;

	/* check overflow */
	if (UNEXPECTED(power_len > SIZE_MAX - power_scale)) {
		return BC_RAISE_STATUS_FULLLEN_IS_OVERFLOW;
	}
	size_t power_full_len = power_len + power_scale;

	sign power_sign;
	if (base->n_sign == MINUS && (exponent & 1) == 1) {
		power_sign = MINUS;
	} else {
		power_sign = PLUS;
	}

	const char *base_end = base->n_value + base_len - 1;

	bc_num power;
	if (base_len <= BC_VECTOR_SIZE && power_full_len <= BC_VECTOR_SIZE * 2) {
		power = bc_fast_raise(base_end, exponent, base_len, power_len, power_scale, power_full_len);
	} else {
		power = bc_standard_raise(base->n_value, base_end, exponent, base_len, power_scale);
	}

	_bc_rm_leading_zeros(power);
	if (bc_is_zero(power)) {
		power->n_sign = PLUS;
		power->n_scale = 0;
	} else {
		power->n_sign = power_sign;
	}

	/* Assign the value. */
	if (is_neg) {
		if (bc_divide(BCG(_one_), power, result, rscale) == false) {
			bc_free_num (&power);
			return BC_RAISE_STATUS_DIVIDE_BY_ZERO;
		}
		bc_free_num (&power);
	} else {
		bc_free_num (result);
		*result = power;
		(*result)->n_scale = MIN(scale, (*result)->n_scale);
	}
	return BC_RAISE_STATUS_OK;
}

/* This is used internally by BCMath */
void bc_raise_bc_exponent(bc_num base, bc_num expo, bc_num *result, size_t scale) {
	/* Exponent must not have fractional part */
	assert(expo->n_scale == 0);

	long exponent = bc_num2long(expo);
	/* Exponent must be properly convertable to long */
	if (exponent == 0 && (expo->n_len > 1 || expo->n_value[0] != 0)) {
		assert(false && "Exponent is not well formed in internal call");
		//assert(exponent != 0 || (expo->n_len == 0 && expo->n_value[0] == 0));
	}
	//assert(exponent != 0 || (expo->n_len == 0 && expo->n_value[0] == 0));
	bc_raise(base, exponent, result, scale);
}

