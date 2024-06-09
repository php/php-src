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
#include "private.h"
#include "zend_alloc.h"


#if SIZEOF_SIZE_T >= 8
#  define BC_VECTOR_SIZE 8
/* The boundary number is computed from BASE ** BC_VECTOR_SIZE */
#  define BC_VECTOR_BOUNDARY_NUM (BC_VECTOR) 100000000
/* Adding more than this many times may cause int64_t to overflow. Typically this is 922. */
#  define BC_VECTOR_NO_OVERFLOW_ADD_COUNT (((BC_VECTOR) 0x7fffffffffffffff) / (BC_VECTOR_BOUNDARY_NUM * BC_VECTOR_BOUNDARY_NUM))
#else
#  define BC_VECTOR_SIZE 4
/* The boundary number is computed from BASE ** BC_VECTOR_SIZE */
#  define BC_VECTOR_BOUNDARY_NUM (BC_VECTOR) 10000
/* Adding more than this many times may cause int32_t to overflow. Typically this is 21. */
#  define BC_VECTOR_NO_OVERFLOW_ADD_COUNT (((BC_VECTOR) 0x7fffffff) / (BC_VECTOR_BOUNDARY_NUM * BC_VECTOR_BOUNDARY_NUM))
#endif


/* Multiply utility routines */

static inline void bc_split_number(BC_VECTOR *prod_vector, size_t prod_arr_size)
{
	for (size_t i = 0; i < prod_arr_size - 1; i++) {
		prod_vector[i + 1] += prod_vector[i] / BC_VECTOR_BOUNDARY_NUM;
		prod_vector[i] %= BC_VECTOR_BOUNDARY_NUM;
	}
}

/* This is based on the technique described in https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html.
 * This function transforms AABBCCDD into 1000 * AA + 100 * BB + 10 * CC + DD,
 * with the caveat that all components must be in the interval [0, 25] to prevent overflow
 * due to the multiplication by power of 10 (10 * 25 = 250 is the largest number that fits in a byte).
 * The advantage of this method instead of using shifts + 3 multiplications is that this is cheaper
 * due to its divide-and-conquer nature.
 */
#if SIZEOF_SIZE_T == 4
static BC_VECTOR bc_parse_chunk_chars(const char *str)
{
	BC_VECTOR tmp;
	memcpy(&tmp, str, sizeof(tmp));
#if !BC_LITTLE_ENDIAN
	tmp = BC_BSWAP(tmp);
#endif

	BC_VECTOR lower_digits = (tmp & 0x0f000f00) >> 8;
	BC_VECTOR upper_digits = (tmp & 0x000f000f) * 10;

	tmp = lower_digits + upper_digits;

	lower_digits = (tmp & 0x00ff0000) >> 16;
	upper_digits = (tmp & 0x000000ff) * 100;

	return lower_digits + upper_digits;
}
#elif SIZEOF_SIZE_T == 8
static BC_VECTOR bc_parse_chunk_chars(const char *str)
{
	BC_VECTOR tmp;
	memcpy(&tmp, str, sizeof(tmp));
#if !BC_LITTLE_ENDIAN
	tmp = BC_BSWAP(tmp);
#endif

	BC_VECTOR lower_digits = (tmp & 0x0f000f000f000f00) >> 8;
	BC_VECTOR upper_digits = (tmp & 0x000f000f000f000f) * 10;

	tmp = lower_digits + upper_digits;

	lower_digits = (tmp & 0x00ff000000ff0000) >> 16;
	upper_digits = (tmp & 0x000000ff000000ff) * 100;

	tmp = lower_digits + upper_digits;

	lower_digits = (tmp & 0x0000ffff00000000) >> 32;
	upper_digits = (tmp & 0x000000000000ffff) * 10000;

	return lower_digits + upper_digits;
}
#endif

/*
 * Converts bc_num to BC_VECTOR, going backwards from pointer n by the number of
 * characters specified by len.
 */
static inline BC_VECTOR bc_partial_convert_to_vector(const char *n, size_t len)
{
	if (len == BC_VECTOR_SIZE) {
		return bc_parse_chunk_chars(n - BC_VECTOR_SIZE + 1);
	}

	BC_VECTOR num = 0;
	BC_VECTOR base = 1;

	for (size_t i = 0; i < len; i++) {
		num += *n * base;
		base *= BASE;
		n--;
	}

	return num;
}

static inline void bc_convert_to_vector(BC_VECTOR *n_vector, const char *nend, size_t nlen)
{
	size_t i = 0;
	while (nlen > 0) {
		size_t len = MIN(BC_VECTOR_SIZE, nlen);
		n_vector[i] = bc_partial_convert_to_vector(nend, len);
		nend -= len;
		nlen -= len;
		i++;
	}
}

/*
 * If the n_values of n1 and n2 are both 4 (32-bit) or 8 (64-bit) digits or less,
 * the calculation will be performed at high speed without using an array.
 */
static inline void bc_fast_mul(bc_num n1, size_t n1len, bc_num n2, size_t n2len, bc_num *prod)
{
	const char *n1end = n1->n_value + n1len - 1;
	const char *n2end = n2->n_value + n2len - 1;

	BC_VECTOR n1_vector = bc_partial_convert_to_vector(n1end, n1len);
	BC_VECTOR n2_vector = bc_partial_convert_to_vector(n2end, n2len);
	BC_VECTOR prod_vector = n1_vector * n2_vector;

	size_t prodlen = n1len + n2len;
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;

	while (pend >= pptr) {
		*pend-- = prod_vector % BASE;
		prod_vector /= BASE;
	}
}

#if BC_LITTLE_ENDIAN
# define BC_ENCODE_LUT(A, B) ((A) | (B) << 4)
#else
# define BC_ENCODE_LUT(A, B) ((B) | (A) << 4)
#endif

#define LUT_ITERATE(_, A) \
	_(A, 0), _(A, 1), _(A, 2), _(A, 3), _(A, 4), _(A, 5), _(A, 6), _(A, 7), _(A, 8), _(A, 9)

/* This LUT encodes the decimal representation of numbers 0-100
 * such that we can avoid taking modulos and divisions which would be slow. */
static const unsigned char LUT[100] = {
	LUT_ITERATE(BC_ENCODE_LUT, 0),
	LUT_ITERATE(BC_ENCODE_LUT, 1),
	LUT_ITERATE(BC_ENCODE_LUT, 2),
	LUT_ITERATE(BC_ENCODE_LUT, 3),
	LUT_ITERATE(BC_ENCODE_LUT, 4),
	LUT_ITERATE(BC_ENCODE_LUT, 5),
	LUT_ITERATE(BC_ENCODE_LUT, 6),
	LUT_ITERATE(BC_ENCODE_LUT, 7),
	LUT_ITERATE(BC_ENCODE_LUT, 8),
	LUT_ITERATE(BC_ENCODE_LUT, 9),
};

static inline unsigned short bc_expand_lut(unsigned char c)
{
	return (c & 0x0f) | (c & 0xf0) << 4;
}

/* Writes the character representation of the number encoded in value.
 * E.g. if value = 1234, then the string "1234" will be written to str. */
static void bc_write_bcd_representation(int32_t value, char *str)
{
	int32_t upper = value / 100; /* e.g. 12 */
	int32_t lower = value % 100; /* e.g. 34 */

#if BC_LITTLE_ENDIAN
	/* Note: little endian, so `lower` comes before `upper`! */
	int32_t digits = bc_expand_lut(LUT[lower]) << 16 | bc_expand_lut(LUT[upper]);
#else
	/* Note: big endian, so `upper` comes before `lower`! */
	int32_t digits = bc_expand_lut(LUT[upper]) << 16 | bc_expand_lut(LUT[lower]);
#endif
	memcpy(str, &digits, sizeof(digits));
}

/*
 * Converts the BCD of bc_num by 4 (32 bits) or 8 (64 bits) digits to an array of BC_VECTOR.
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

	size_t n1_arr_size = (n1len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t n2_arr_size = (n2len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t prod_arr_size = n1_arr_size + n2_arr_size - 1;

	/*
	 * let's say that N is the max of n1len and n2len (and a multiple of BC_VECTOR_SIZE for simplicity),
	 * then this sum is <= N/BC_VECTOR_SIZE + N/BC_VECTOR_SIZE + N/BC_VECTOR_SIZE + N/BC_VECTOR_SIZE - 1
	 * which is equal to N - 1 if BC_VECTOR_SIZE is 4, and N/2 - 1 if BC_VECTOR_SIZE is 8.
	 */
	BC_VECTOR *buf = safe_emalloc(n1_arr_size + n2_arr_size + prod_arr_size, sizeof(BC_VECTOR), 0);

	BC_VECTOR *n1_vector = buf;
	BC_VECTOR *n2_vector = buf + n1_arr_size;
	BC_VECTOR *prod_vector = n2_vector + n2_arr_size;

	for (i = 0; i < prod_arr_size; i++) {
		prod_vector[i] = 0;
	}

	/* Convert to BC_VECTOR[] */
	bc_convert_to_vector(n1_vector, n1end, n1len);
	bc_convert_to_vector(n2_vector, n2end, n2len);

	/* Multiplication and addition */
	size_t count = 0;
	for (i = 0; i < n1_arr_size; i++) {
		/*
		 * This calculation adds the result multiple times to the array entries.
		 * When multiplying large numbers of digits, there is a possibility of
		 * overflow, so digit adjustment is performed beforehand.
		 */
		if (UNEXPECTED(count >= BC_VECTOR_NO_OVERFLOW_ADD_COUNT)) {
			bc_split_number(prod_vector, prod_arr_size);
			count = 0;
		}
		count++;
		for (size_t j = 0; j < n2_arr_size; j++) {
			prod_vector[i + j] += n1_vector[i] * n2_vector[j];
		}
	}

	/*
	 * Move a value exceeding 4/8 digits by carrying to the next digit.
	 * However, the last digit does nothing.
	 */
	bc_split_number(prod_vector, prod_arr_size);

	/* Convert to bc_num */
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;
	i = 0;
	while (i < prod_arr_size - 1) {
#if BC_VECTOR_SIZE == 4
		bc_write_bcd_representation(prod_vector[i], pend - 3);
		pend -= 4;
#else
		bc_write_bcd_representation(prod_vector[i] / 10000, pend - 7);
		bc_write_bcd_representation(prod_vector[i] % 10000, pend - 3);
		pend -= 8;
#endif
		i++;
	}

	/*
	 * The last digit may carry over.
	 * Also need to fill it to the end with zeros, so loop until the end of the string.
	 */
	while (pend >= pptr) {
		*pend-- = prod_vector[i] % BASE;
		prod_vector[i] /= BASE;
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
	if (len1 <= BC_VECTOR_SIZE && len2 <= BC_VECTOR_SIZE) {
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
