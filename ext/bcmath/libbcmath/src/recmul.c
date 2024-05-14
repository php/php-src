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
#include <stdlib.h>
#include "private.h" /* For _bc_rm_leading_zeros() */
#include "zend_alloc.h"


#if SIZEOF_SIZE_T >= 8
#  define BC_MUL_UINT_DIGITS 8
#  define BC_MUL_UINT_OVERFLOW 100000000
#else
#  define BC_MUL_UINT_DIGITS 4
#  define BC_MUL_UINT_OVERFLOW 10000
#endif

#define BC_UINT_TO_BCD_ONE_DIGIT(num, bcd) do { \
	bcd = num % 10; \
	num /= 10; \
} while (0)


/* Multiply utility routines */

/*
 * Converts BCD to uint, going backwards from pointer n by the number of
 * characters specified by len.
 *
 * Since the upper limit of len is known, open the loop to save loop cost.
 */
static inline BC_UINT_T bc_partial_convert_to_uint(const char *n, size_t len)
{
	BC_UINT_T num = n[0];

	switch (len) {
		case 1:
			return num;
		case 2:
			num += n[-1] * 10;
			return num;
		case 3:
			num += n[-1] * 10;
			num += n[-2] * 100;
			return num;
		case 4:
			num += n[-1] * 10;
			num += n[-2] * 100;
			num += n[-3] * 1000;
			return num;
#if BC_MUL_UINT_DIGITS == 8
		case 5:
			num += n[-1] * 10;
			num += n[-2] * 100;
			num += n[-3] * 1000;
			num += n[-4] * 10000;
			return num;
		case 6:
			num += n[-1] * 10;
			num += n[-2] * 100;
			num += n[-3] * 1000;
			num += n[-4] * 10000;
			num += n[-5] * 100000;
			return num;
		case 7:
			num += n[-1] * 10;
			num += n[-2] * 100;
			num += n[-3] * 1000;
			num += n[-4] * 10000;
			num += n[-5] * 100000;
			num += n[-6] * 1000000;
			return num;
		case 8:
			num += n[-1] * 10;
			num += n[-2] * 100;
			num += n[-3] * 1000;
			num += n[-4] * 10000;
			num += n[-5] * 100000;
			num += n[-6] * 1000000;
			num += n[-7] * 10000000;
			return num;
#endif
	}

	return num;
}

/*
 * Since the number of digits is fixed, open the loop and saves cost.
 */
static inline BC_UINT_T bc_partial_convert_to_uint_fixed_digits(const char *n)
{
	BC_UINT_T num = n[0];
	num += n[-1] * 10;
	num += n[-2] * 100;
	num += n[-3] * 1000;
#if BC_MUL_UINT_DIGITS == 8
	num += n[-4] * 10000;
	num += n[-5] * 100000;
	num += n[-6] * 1000000;
	num += n[-7] * 10000000;
#endif

	return num;
}

/*
 * Convert HEX to BCD. The number of digits can be up to 16 decimal digits (64-bit).
 */
static inline void bc_uint_to_bcd(BC_UINT_T num, char *bcd, size_t len)
{
	switch (len) {
		case 0:
			return;
		case 1:
			*bcd = num % 10;
			return;
		case 2:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			bcd[-1] = num % 10;
			return;
		case 3:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			bcd[-2] = num % 10;
			return;
		case 4:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			bcd[-3] = num % 10;
			return;
		case 5:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			bcd[-4] = num % 10;
			return;
		case 6:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			bcd[-5] = num % 10;
			return;
		case 7:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			bcd[-6] = num % 10;
			return;
		case 8:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			bcd[-7] = num % 10;
			return;
#if BC_MUL_UINT_DIGITS == 8
		case 9:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			bcd[-8] = num % 10;
			return;
		case 10:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			bcd[-9] = num % 10;
			return;
		case 11:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			bcd[-10] = num % 10;
			return;
		case 12:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-10]);
			bcd[-11] = num % 10;
			return;
		case 13:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-10]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-11]);
			bcd[-12] = num % 10;
			return;
		case 14:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-10]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-11]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-12]);
			bcd[-13] = num % 10;
			return;
		case 15:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-10]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-11]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-12]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-13]);
			bcd[-14] = num % 10;
			return;
		case 16:
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-8]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-9]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-10]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-11]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-12]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-13]);
			BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-14]);
			bcd[-15] = num % 10;
			return;
#endif
	}
}

/*
 * Converts 8 decimal digits of HEX to 8 digits of BCD (64-bit).
 * Since the number of digits is fixed, open the loop and saves cost.
 */
static inline BC_UINT_T bc_uint_to_bcd_fixed_digits(BC_UINT_T num, char *bcd)
{
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[0]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-1]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-2]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-3]);
#if BC_MUL_UINT_DIGITS == 8
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-4]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-5]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-6]);
	BC_UINT_TO_BCD_ONE_DIGIT(num, bcd[-7]);
#endif

	return num;
}

/*
 * If the n_values ​​of n1 and n2 are both 4 (32-bit) or 8 (64-bit) digits or less,
 * the calculation will be performed at high speed without using an array.
 */
static inline void bc_fast_mul(bc_num n1, size_t n1len, bc_num n2, int n2len, bc_num *prod)
{
	char *n1end = n1->n_value + n1len - 1;
	char *n2end = n2->n_value + n2len - 1;

	BC_UINT_T n1_uint = bc_partial_convert_to_uint(n1end, n1len);
	BC_UINT_T n2_uint = bc_partial_convert_to_uint(n2end, n2len);
	BC_UINT_T prod_uint = n1_uint * n2_uint;

	size_t prodlen = n1len + n2len;
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pend = (*prod)->n_value + prodlen - 1;

	bc_uint_to_bcd(prod_uint, pend, prodlen);
}

/*
 * Converts the BCD of bc_num by 4 (32 bits) or 8 (64 bits) digits to an array of BC_UINT_Ts.
 * The array is generated starting with the smaller digits.
 * e.g. 12345678901234567890 => {34567890, 56789012, 1234}
 *
 * Multiply and add these groups of numbers to perform multiplication fast.
 * How much to shift the digits when adding values ​​can be calculated from the index of the array.
 */
static void bc_standard_mul(bc_num n1, size_t n1len, bc_num n2, int n2len, bc_num *prod)
{
	size_t i;
	char *n1end = n1->n_value + n1len - 1;
	char *n2end = n2->n_value + n2len - 1;
	size_t prodlen = n1len + n2len;

	size_t n1_arr_size = n1len / BC_MUL_UINT_DIGITS + (n1len % BC_MUL_UINT_DIGITS ? 1 : 0);
	size_t n2_arr_size = n2len / BC_MUL_UINT_DIGITS + (n2len % BC_MUL_UINT_DIGITS ? 1 : 0);
	size_t prod_arr_size = n1_arr_size + n2_arr_size - 1;

	BC_UINT_T *buf = emalloc((n1_arr_size + n2_arr_size + prod_arr_size) * sizeof(BC_UINT_T));

	BC_UINT_T *n1_uint = buf;
	BC_UINT_T *n2_uint = buf + n1_arr_size;
	BC_UINT_T *prod_uint = n2_uint + n2_arr_size;

	for (i = 0; i < prod_arr_size; i++) {
		prod_uint[i] = 0;
	}

	/* Convert n1 to uint[] */
	i = 0;
	while (n1len >= BC_MUL_UINT_DIGITS) {
		n1_uint[i] = bc_partial_convert_to_uint_fixed_digits(n1end);
		n1end -= BC_MUL_UINT_DIGITS;
		n1len -= BC_MUL_UINT_DIGITS;
		i++;
	}
	if (n1len > 0) {
		n1_uint[i] = bc_partial_convert_to_uint(n1end, n1len);
	}

	/* Convert n2 to uint[] */
	i = 0;
	while (n2len >= BC_MUL_UINT_DIGITS) {
		n2_uint[i] = bc_partial_convert_to_uint_fixed_digits(n2end);
		n2end -= BC_MUL_UINT_DIGITS;
		n2len -= BC_MUL_UINT_DIGITS;
		i++;
	}
	if (n2len > 0) {
		n2_uint[i] = bc_partial_convert_to_uint(n2end, n2len);
	}

	/* Multiplication and addition */
	for (i = 0; i < n1_arr_size; i++) {
		for (size_t j = 0; j < n2_arr_size; j++) {
			prod_uint[i + j] += n1_uint[i] * n2_uint[j];
		}
	}

	/*
	 * Move a value exceeding 4/8 digits by carrying to the next digit.
	 * However, the last digit does nothing.
	 */
	for (i = 0; i < prod_arr_size - 1; i++) {
		prod_uint[i + 1] += prod_uint[i] / BC_MUL_UINT_OVERFLOW;
		prod_uint[i] %= BC_MUL_UINT_OVERFLOW;
	}

	/* Convert to bc_num */
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pend = (*prod)->n_value + prodlen - 1;
	for (i = 0; i < prod_arr_size - 1; i++) {
		prod_uint[i] = bc_uint_to_bcd_fixed_digits(prod_uint[i], pend);
		pend -= BC_MUL_UINT_DIGITS;
		prodlen -= BC_MUL_UINT_DIGITS;
	}

	/*
	 * The last digit may carry over.
	 * Also need to fill it to the end with zeros, so loop until the end of the string.
	 */
	bc_uint_to_bcd(prod_uint[i], pend, prodlen);

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
