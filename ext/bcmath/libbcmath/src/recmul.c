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
#include "private.h" /* For _bc_rm_leading_zeros() */
#include "zend_alloc.h"


/* Multiply utility routines */

/*
 * Converts BCD to long, going backwards from pointer n by the number of
 * characters specified by len.
 */
static inline unsigned long bc_partial_convert_to_long(const char *n, size_t len)
{
	unsigned long num = 0;
	unsigned long base = 1;

	for (size_t i = 0; i < len; i++) {
		num += *n * base;
		base *= BASE;
		n--;
	}

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

	unsigned long n1_l = bc_partial_convert_to_long(n1end, n1len);
	unsigned long n2_l = bc_partial_convert_to_long(n2end, n2len);
	unsigned long prod_l = n1_l * n2_l;

	size_t prodlen = n1len + n2len;
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;

	while (pend >= pptr) {
		*pend-- = prod_l % BASE;
		prod_l /= BASE;
	}
}

/*
 * Converts the BCD of bc_num by 4 (32 bits) or 8 (64 bits) digits to an array of unsigned longs.
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

	size_t n1_arr_size = n1len / BC_LONGABLE_DIGITS + (n1len % BC_LONGABLE_DIGITS ? 1 : 0);
	size_t n2_arr_size = n2len / BC_LONGABLE_DIGITS + (n2len % BC_LONGABLE_DIGITS ? 1 : 0);
	size_t prod_arr_size = n1_arr_size + n2_arr_size - 1;

	unsigned long *n1_l = emalloc(n1_arr_size * sizeof(unsigned long));
	unsigned long *n2_l = emalloc(n2_arr_size * sizeof(unsigned long));
	unsigned long *prod_l = ecalloc(prod_arr_size, sizeof(unsigned long));

	/* Convert n1 to long[] */
	i = 0;
	while (n1len > 0) {
		size_t len = MIN(BC_LONGABLE_DIGITS, n1len);
		n1_l[i] = bc_partial_convert_to_long(n1end, len);
		n1end -= len;
		n1len -= len;
		i++;
	}

	/* Convert n2 to long[] */
	i = 0;
	while (n2len > 0) {
		size_t len = MIN(BC_LONGABLE_DIGITS, n2len);
		n2_l[i] = bc_partial_convert_to_long(n2end, len);
		n2end -= len;
		n2len -= len;
		i++;
	}

	/* Multiplication and addition */
	for (i = 0; i < n1_arr_size; i++) {
		for (size_t j = 0; j < n2_arr_size; j++) {
			prod_l[i + j] += n1_l[i] * n2_l[j];
		}
	}

	/*
	 * Move a value exceeding 8 digits by carrying to the next digit.
	 * However, the last digit does nothing.
	 */
	for (i = 0; i < prod_arr_size - 1; i++) {
		prod_l[i + 1] += prod_l[i] / BC_LONGABLE_OVERFLOW;
		prod_l[i] %= BC_LONGABLE_OVERFLOW;
	}

	/* Convert to bc_num */
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;
	i = 0;
	while (i < prod_arr_size - 1) {
		for (size_t j = 0; j < BC_LONGABLE_DIGITS; j++) {
			*pend-- = prod_l[i] % BASE;
			prod_l[i] /= BASE;
		}
		i++;
	}

	/*
	 * The last digit may carry over.
	 * Also need to fill it to the end with zeros, so loop until the end of the string.
	 */
	while (pend >= pptr) {
		*pend-- = prod_l[i] % BASE;
		prod_l[i] /= BASE;
	}

	efree(n1_l);
	efree(n2_l);
	efree(prod_l);
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
	if (len1 <= BC_LONGABLE_DIGITS && len2 <= BC_LONGABLE_DIGITS) {
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
