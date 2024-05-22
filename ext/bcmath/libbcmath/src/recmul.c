/* recmul.c: bcmath library file. */
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
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include "private.h" /* For _bc_rm_leading_zeros() */
#include "zend_alloc.h"


#if SIZEOF_SIZE_T >= 8
#  define BC_MUL_UINT_DIGITS 8
#  define BC_MUL_UINT_OVERFLOW (BC_UINT_T) 100000000
#else
#  define BC_MUL_UINT_DIGITS 4
#  define BC_MUL_UINT_OVERFLOW (BC_UINT_T) 10000
#endif

#define BC_MUL_MAX_ADD_COUNT (~((BC_UINT_T) 0) / (BC_MUL_UINT_OVERFLOW * BC_MUL_UINT_OVERFLOW))


/* Multiply utility routines */

static inline void bc_digits_adjustment(BC_UINT_T *prod_uint, size_t prod_arr_size)
{
	for (size_t i = 0; i < prod_arr_size - 1; i++) {
		prod_uint[i + 1] += prod_uint[i] / BC_MUL_UINT_OVERFLOW;
		prod_uint[i] %= BC_MUL_UINT_OVERFLOW;
	}
}

/*
 * Converts BCD to uint, going backwards from pointer n by the number of
 * characters specified by len.
 */
static inline BC_UINT_T bc_partial_convert_to_uint(const char *n, size_t len)
{
	BC_UINT_T num = 0;
	BC_UINT_T base = 1;

	for (size_t i = 0; i < len; i++) {
		num += *n * base;
		base *= BASE;
		n--;
	}

	return num;
}

static inline void bc_convert_to_uint(BC_UINT_T *n_uint, const char *nend, size_t nlen)
{
	size_t i = 0;
	while (nlen > 0) {
		size_t len = MIN(BC_MUL_UINT_DIGITS, nlen);
		n_uint[i] = bc_partial_convert_to_uint(nend, len);
		nend -= len;
		nlen -= len;
		i++;
	}
}

/*
 * If the n_values of n1 and n2 are both 4 (32-bit) or 8 (64-bit) digits or less,
 * the calculation will be performed at high speed without using an array.
 */
static inline void bc_fast_mul(bc_num n1, size_t n1len, bc_num n2, int n2len, bc_num *prod)
{
	const char *n1end = n1->n_value + n1len - 1;
	const char *n2end = n2->n_value + n2len - 1;

	BC_UINT_T n1_uint = bc_partial_convert_to_uint(n1end, n1len);
	BC_UINT_T n2_uint = bc_partial_convert_to_uint(n2end, n2len);
	BC_UINT_T prod_uint = n1_uint * n2_uint;

	size_t prodlen = n1len + n2len;
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;

	while (pend >= pptr) {
		*pend-- = prod_uint % BASE;
		prod_uint /= BASE;
	}
}

/*
 * Converts the BCD of bc_num by 4 (32 bits) or 8 (64 bits) digits to an array of BC_UINT_Ts.
 * The array is generated starting with the smaller digits.
 * e.g. 12345678901234567890 => {34567890, 56789012, 1234}
 *
 * Multiply and add these groups of numbers to perform multiplication fast.
 * How much to shift the digits when adding values can be calculated from the index of the array.
 */
static void bc_standard_mul(bc_num n1, size_t n1len, bc_num n2, size_t n2len, bc_num *prod)
{
	size_t i;
	const char *n1end = n1->n_value + n1len - 1;
	const char *n2end = n2->n_value + n2len - 1;
	size_t prodlen = n1len + n2len;

	size_t n1_arr_size = (n1len + BC_MUL_UINT_DIGITS - 1) / BC_MUL_UINT_DIGITS;
	size_t n2_arr_size = (n2len + BC_MUL_UINT_DIGITS - 1) / BC_MUL_UINT_DIGITS;
	size_t prod_arr_size = n1_arr_size + n2_arr_size - 1;

	/*
	 * let's say that N is the max of n1len and n2len (and a multiple of BC_MUL_UINT_DIGITS for simplicity),
	 * then this sum is <= N/BC_MUL_UINT_DIGITS + N/BC_MUL_UINT_DIGITS + N/BC_MUL_UINT_DIGITS + N/BC_MUL_UINT_DIGITS - 1
	 * which is equal to N - 1 if BC_MUL_UINT_DIGITS is 4, and N/2 - 1 if BC_MUL_UINT_DIGITS is 8.
	 */
	BC_UINT_T *buf = safe_emalloc(n1_arr_size + n2_arr_size + prod_arr_size, sizeof(BC_UINT_T), 0);

	BC_UINT_T *n1_uint = buf;
	BC_UINT_T *n2_uint = buf + n1_arr_size;
	BC_UINT_T *prod_uint = n2_uint + n2_arr_size;

	for (i = 0; i < prod_arr_size; i++) {
		prod_uint[i] = 0;
	}

	/* Convert to uint[] */
	bc_convert_to_uint(n1_uint, n1end, n1len);
	bc_convert_to_uint(n2_uint, n2end, n2len);

	/* Multiplication and addition */
	size_t count = 0;
	for (i = 0; i < n1_arr_size; i++) {
		/*
		 * This calculation adds the result multiple times to the array entries.
		 * When multiplying large numbers of digits, there is a possibility of
		 * overflow, so digit adjustment is performed beforehand.
		 */
		if (UNEXPECTED(count >= BC_MUL_MAX_ADD_COUNT)) {
			bc_digits_adjustment(prod_uint, prod_arr_size);
			count = 0;
		}
		count++;
		for (size_t j = 0; j < n2_arr_size; j++) {
			prod_uint[i + j] += n1_uint[i] * n2_uint[j];
		}
	}

	/*
	 * Move a value exceeding 4/8 digits by carrying to the next digit.
	 * However, the last digit does nothing.
	 */
	bc_digits_adjustment(prod_uint, prod_arr_size);

	/* Convert to bc_num */
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;
	i = 0;
	while (i < prod_arr_size - 1) {
		for (size_t j = 0; j < BC_MUL_UINT_DIGITS; j++) {
			*pend-- = prod_uint[i] % BASE;
			prod_uint[i] /= BASE;
		}
		i++;
	}

	/*
	 * The last digit may carry over.
	 * Also need to fill it to the end with zeros, so loop until the end of the string.
	 */
	while (pend >= pptr) {
		*pend-- = prod_uint[i] % BASE;
		prod_uint[i] /= BASE;
	}

	efree(buf);
}

/* The multiply routine.  N2 times N1 is put int PROD with the scale of
   the result being MIN(N2 scale+N1 scale, MAX (SCALE, N2 scale, N1 scale)).
   */

bc_num bc_multiply(bc_num n1, bc_num n2, size_t scale)
{
	bc_num prod;

	/* Initialize things. */
	size_t len1 = n1->n_len + n1->n_scale;
	size_t len2 = n2->n_len + n2->n_scale;
	size_t full_scale = n1->n_scale + n2->n_scale;
	size_t prod_scale = MIN(full_scale, MAX(scale, MAX(n1->n_scale, n2->n_scale)));

	/* Do the multiply */
	if (len1 <= BC_MUL_UINT_DIGITS && len2 <= BC_MUL_UINT_DIGITS) {
		bc_fast_mul(n1, len1, n2, len2, &prod);
	} else {
		bc_standard_mul(n1, len1, n2, len2, &prod);
	}

	/* Assign to prod and clean up the number. */
	prod->n_sign = (n1->n_sign == n2->n_sign ? PLUS : MINUS);
	prod->n_len -= full_scale;
	prod->n_scale = prod_scale;
	_bc_rm_leading_zeros(prod);
	if (bc_is_zero(prod)) {
		prod->n_sign = PLUS;
	}
	return prod;
}
