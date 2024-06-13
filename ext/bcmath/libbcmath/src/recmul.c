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
/* It is the next smallest power of 2 after BC_VECTOR_NO_OVERFLOW_ADD_COUNT / 2 */
#  define BC_KARATSUBA_MUL_NEED_CARRY_CALC_SIZE 512
#  define BC_MUL_USE_KARATSUBA_SIZE (BC_VECTOR_SIZE * 32)
#else
#  define BC_VECTOR_SIZE 4
/* The boundary number is computed from BASE ** BC_VECTOR_SIZE */
#  define BC_VECTOR_BOUNDARY_NUM (BC_VECTOR) 10000
/* It is the next smallest power of 2 after BC_VECTOR_NO_OVERFLOW_ADD_COUNT / 2 */
#  define BC_KARATSUBA_MUL_NEED_CARRY_CALC_SIZE 16
#  define BC_MUL_USE_KARATSUBA_SIZE (BC_VECTOR_SIZE * 16)
#endif

/*
 * Adding more than this many times may cause uint32_t/uint64_t to overflow.
 * Typically this is 1844 for 64bit and 42 for 32bit.
 */
#define BC_VECTOR_NO_OVERFLOW_ADD_COUNT (~((BC_VECTOR) 0) / (BC_VECTOR_BOUNDARY_NUM * BC_VECTOR_BOUNDARY_NUM))
#define BC_VECTOR_MAX_HALF (~((BC_VECTOR) 0) / 2)

/*
 * If the number is small, the overhead will be large, so when dividing and conquering using
 * Karatsuba algorithm, if the size becomes smaller than this value, normal multiplication is
 * performed without dividing it any further.
 */
#define BC_MUL_KARATSUBA_DO_CALC_SIZE 16


/* Multiply utility routines */

static inline void bc_mul_carry_calc(BC_VECTOR *prod_vector, size_t prod_arr_size)
{
	for (size_t i = 0; i < prod_arr_size - 1; i++) {
		prod_vector[i + 1] += prod_vector[i] / BC_VECTOR_BOUNDARY_NUM;
		prod_vector[i] %= BC_VECTOR_BOUNDARY_NUM;
	}
}

/*
 * When calculating carry with the karatsuba algorithm, negative values ​​must be taken into account.
 * To accurately calculate "negative values" ​​in unsigned integers, use this function before the value exceeds
 * half of the maximum value of the type by any means other than underflow.
 * All values ​​greater than half of the maximum value are interpreted as negative values ​​due to underflow,
 * converted to absolute values, and calculated appropriately.
 * However, "negative values" ​​are not corrected to positive values.
 */
static inline void bc_karatsuba_mul_carry_calc_single(BC_VECTOR *ret)
{
	if (ret[0] <= BC_VECTOR_MAX_HALF) {
		/* "positive number" */
		ret[1] += ret[0] / BC_VECTOR_BOUNDARY_NUM;
		ret[0] %= BC_VECTOR_BOUNDARY_NUM;
	} else {
		/* "negative number" */
		BC_VECTOR abs = (BC_VECTOR) 0 - ret[0];
		BC_VECTOR borrow = abs / BC_VECTOR_BOUNDARY_NUM;
		ret[1] -= borrow;
		ret[0] +=  BC_VECTOR_BOUNDARY_NUM * borrow;
	}
}

static inline void bc_karatsuba_mul_carry_calc(BC_VECTOR *ret, size_t ret_arr_size)
{
	for (size_t i = 0; i < ret_arr_size - 1; i++) {
		bc_karatsuba_mul_carry_calc_single(&ret[i]);
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
static void bc_write_bcd_representation(uint32_t value, char *str)
{
	uint32_t upper = value / 100; /* e.g. 12 */
	uint32_t lower = value % 100; /* e.g. 34 */

#if BC_LITTLE_ENDIAN
	/* Note: little endian, so `lower` comes before `upper`! */
	uint32_t digits = bc_expand_lut(LUT[lower]) << 16 | bc_expand_lut(LUT[upper]);
#else
	/* Note: big endian, so `upper` comes before `lower`! */
	uint32_t digits = bc_expand_lut(LUT[upper]) << 16 | bc_expand_lut(LUT[lower]);
#endif
	memcpy(str, &digits, sizeof(digits));
}

static inline void bc_mul_vector_to_bc_num(BC_VECTOR *prod_vector, size_t prodlen, size_t prod_arr_size, bc_num *prod)
{
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;
	size_t i = 0;
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
	size_t prod_arr_size = (prodlen + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;

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
			bc_mul_carry_calc(prod_vector, prod_arr_size);
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
	bc_mul_carry_calc(prod_vector, prod_arr_size);

	/* Convert to bc_num */
	bc_mul_vector_to_bc_num(prod_vector, prodlen, prod_arr_size, prod);

	efree(buf);
}

/*
 * BC_VECTOR may overflow during the calculation of Karatsuba algorithm. Since the calc_arr_size
 * that can overflow is known, we need to do the carry-over calculation before that.
 * This function determines whether a carry calculation needs to be performed.
 */
static inline bool bc_karatsuba_mul_need_carry_calc(size_t calc_arr_size)
{
retry:
	if (EXPECTED(calc_arr_size < BC_KARATSUBA_MUL_NEED_CARRY_CALC_SIZE)) {
		return false;
	}

	calc_arr_size /= BC_KARATSUBA_MUL_NEED_CARRY_CALC_SIZE;
	if (UNEXPECTED(calc_arr_size == 1)) {
		return true;
	}
	goto retry;
}

static inline void bc_karatsuba_mul_buf_calc_for_mid(BC_VECTOR *buf, BC_VECTOR high, BC_VECTOR low, BC_VECTOR *carry)
{
	*buf = high - low + *carry;

	/*
	 * Carry may occur during subtraction. If we perform multiplication while a carry is occurring,
	 * BC_VECTOR may overflow sooner than expected, so we calculate the carry.
	 */
	if (*buf >= BC_VECTOR_BOUNDARY_NUM && *buf <= BC_VECTOR_MAX_HALF) {
		*carry = 1;
		*buf -= BC_VECTOR_BOUNDARY_NUM;
	} else if ((BC_VECTOR) 0 - *buf >= BC_VECTOR_BOUNDARY_NUM && *buf > BC_VECTOR_MAX_HALF) {
		*carry = -1;
		*buf += BC_VECTOR_BOUNDARY_NUM;
	} else {
		*carry = 0;
	}
}

/*
 * When a subtraction results in a carry that cannot fit in the buffer, add the result to mid.
 * Carry is always 1 or -1, so can just add/subtract the buffer as is.
 */
static inline void bc_karatsuba_mul_add_buf_to_mid(BC_VECTOR *mid, const BC_VECTOR *buf, BC_VECTOR carry, size_t half_size)
{
	if (EXPECTED(carry == 0)) {
		return;
	}

	if (carry == 1) {
		for (size_t i = 0; i < half_size; i++) {
			mid[i + half_size] += buf[i];
		}
	} else {
		for (size_t i = 0; i < half_size; i++) {
			mid[i + half_size] -= buf[i];
		}
	}
}

static inline void bc_karatsuba_mul_do_calc(BC_VECTOR *n1, size_t n1_size, BC_VECTOR *n2, size_t n2_size, BC_VECTOR *ret)
{
	for (size_t i = 0; i < n1_size + n2_size; i++) {
		ret[i] = 0;
	}

	for (size_t i = 0; i < n1_size; i++) {
		for (size_t j = 0; j < n2_size; j++) {
			ret[i + j] += n1[i] * n2[j];
		}
	}
#if SIZEOF_SIZE_T < 8
	/* In the case of 32-bit, there is a possibility of overflow, so carry-over calculations are performed. */
	bc_karatsuba_mul_carry_calc(ret, n1_size + n2_size);
#endif
}

/*
 * The Karatsuba algorithm uses recursive divide-and-conquer computation. This function is
 * a process that is called recursively.
 * When calc_arr_size, n1_size and n2_size are equal, can skip the size check.
 * This function is a faster version that can be used in such cases.
 */
static void bc_karatsuba_mul_fast_recursive(
	BC_VECTOR *n1, BC_VECTOR *n1_buf, size_t n1_size,
	BC_VECTOR *n2, BC_VECTOR *n2_buf, size_t n2_size,
	size_t calc_arr_size, BC_VECTOR *ret)
{
	if (calc_arr_size <= BC_MUL_KARATSUBA_DO_CALC_SIZE) {
		bc_karatsuba_mul_do_calc(n1, n1_size, n2, n2_size, ret);
		return;
	}

	ZEND_ASSERT(n1_size == n2_size && n1_size == calc_arr_size);

	size_t i;
	size_t half_size = calc_arr_size / 2;

	/* low */
	BC_VECTOR *low = ret;
	bc_karatsuba_mul_fast_recursive(n1, n1_buf, half_size, n2, n2_buf, half_size, half_size, low);
	/* high */
	BC_VECTOR *high = low + calc_arr_size;
	bc_karatsuba_mul_fast_recursive(n1 + half_size, n1_buf, half_size, n2 + half_size, n2_buf, half_size, half_size, high);

	/* prepare mid */
	BC_VECTOR n1_buf_carry = 0;
	BC_VECTOR n2_buf_carry = 0;
	for (i = 0; i < half_size; i++) {
		/*
		 * Calculation of carry during subtraction is also performed at the same time.
		 * Carries that cannot fit into the buffer are recorded in n1_buf_carry and n2_buf_carry.
		 */
		bc_karatsuba_mul_buf_calc_for_mid(&n1_buf[i], n1[i + half_size], n1[i], &n1_buf_carry);
		bc_karatsuba_mul_buf_calc_for_mid(&n2_buf[i], n2[i + half_size], n2[i], &n2_buf_carry);
	}

	/* mid */
	BC_VECTOR *mid = high + calc_arr_size;
	bc_karatsuba_mul_fast_recursive(n1_buf, n1_buf + half_size, half_size, n2_buf, n2_buf + half_size, half_size, half_size, mid);

	/* If n1_buf has a carry, add n2_buf. The opposite is true when n2_buf has a carry. */
	bc_karatsuba_mul_add_buf_to_mid(mid, n1_buf, n2_buf_carry, half_size);
	bc_karatsuba_mul_add_buf_to_mid(mid, n2_buf, n1_buf_carry, half_size);

	/* Add to ret */
	for (i = 0; i < half_size; i++) {
		mid[i] -= high[i] + low[i];
	}
	for (; i < calc_arr_size; i++) {
		ret[i + half_size] += low[i] + high[i] - mid[i];
	}
	for (i = 0; i < half_size; i++) {
		ret[i + half_size] -= mid[i];
	}
	/* n1_buf and n2_buf, if there is a carry that doesn't fit in the buffer, add/subtract it extra. */
	if (UNEXPECTED(n1_buf_carry != 0 && n2_buf_carry != 0)) {
		if ((n1_buf_carry == 1 && n2_buf_carry == 1) || (n1_buf_carry == (BC_VECTOR) -1 && n2_buf_carry == (BC_VECTOR) -1)) {
			ret[calc_arr_size + half_size] -= 1;
		} else {
			ret[calc_arr_size + half_size] += 1;
		}
	}

	/* Calc carry if need */
	if (UNEXPECTED(bc_karatsuba_mul_need_carry_calc(calc_arr_size))) {
		bc_karatsuba_mul_carry_calc(ret, calc_arr_size * 2);
	}
}

/*
 * The Karatsuba algorithm uses recursive divide-and-conquer computation. This function is
 * a process that is called recursively. n1_size is always larger than n2_size.
 */
static void bc_karatsuba_mul_recursive(
	BC_VECTOR *n1, BC_VECTOR *n1_buf, size_t n1_size,
	BC_VECTOR *n2, BC_VECTOR *n2_buf, size_t n2_size,
	size_t calc_arr_size, BC_VECTOR *ret)
{
	if (calc_arr_size <= BC_MUL_KARATSUBA_DO_CALC_SIZE) {
		bc_karatsuba_mul_do_calc(n1, n1_size, n2, n2_size, ret);
		return;
	}

	size_t ret_size = n1_size + n2_size;

	size_t i;
	size_t half_size = calc_arr_size / 2;

	/* low */
	BC_VECTOR *low = ret;
	size_t low_ret_size;
	if (n2_size >= half_size) {
		/* n1_size is always greater than n2_size, so if n2_size is greater than half_size, low can use the fast version. */
		bc_karatsuba_mul_fast_recursive(n1, n1_buf, half_size, n2, n2_buf, half_size, half_size, low);
		low_ret_size = calc_arr_size;
	} else {
		bc_karatsuba_mul_recursive(n1, n1_buf, MIN(n1_size, half_size), n2, n2_buf, n2_size, half_size, low);
		low_ret_size = MIN(n1_size, half_size) + n2_size;
	}

	/* high */
	BC_VECTOR *high;
	size_t high_ret_size;
	/*
	 * If this condition is met, all values ​​used for high of n2 are 0, so it is obvious that the calculation result will be 0.
	 * Therefore, will skip the calculation.
	 */
	if (n2_size > half_size) {
		high = low + calc_arr_size;
		bc_karatsuba_mul_recursive(n1 + half_size, n1_buf, n1_size - half_size, n2 + half_size, n2_buf, n2_size - half_size, half_size, high);
		high_ret_size = n1_size - half_size + n2_size - half_size;
	}

	/* mid */
	BC_VECTOR *mid_ex = ret + ret_size; // The ex suffix is ​​added because the meaning is slightly different from the fast version mid.
	BC_VECTOR n1_buf_carry = 0;
	BC_VECTOR n2_buf_carry = 0;
	size_t mid_ret_size;

	/*
	 * When n1_size and n2_size are less than half_size, the calculation result of mid
	 * is exactly the same as low. So in that case can skip it.
	 * Taking ab * cd as an example, if both n1_size and n2_size are less than half_size, a and c are 0.
	 * If decompose this using the Karatsuba algorithm, we get the following.
	 * low = b * d
	 * high = 0 * 0
	 * mid = (0 - b) * (0 - d)
	 * ret = high * 100 + (high + low - mid) * 10 + low
	 *     = 0 + (0 + b * d - (-b * -d)) * 10 + b * d
	 *     = (b * d - b * d) * 10 + b * d
	 *     = b * d
	 * So in this case, only low is added to the result.
	 */
	if (n2_size > half_size) {
		/* If n1_size and n2_size are both greater than half_size, calculate mid as usual. */

		/* prepare mid */
		mid_ret_size = calc_arr_size;

		for (i = 0; i < n1_size - half_size; i++) {
			/*
			 * Calculation of carry during subtraction is also performed at the same time.
			 * Carries that cannot fit into the buffer are recorded in n1_buf_carry and n2_buf_carry.
			 */
			bc_karatsuba_mul_buf_calc_for_mid(&n1_buf[i], n1[i + half_size], n1[i], &n1_buf_carry);
		}
		for (; i < half_size; i++) {
			bc_karatsuba_mul_buf_calc_for_mid(&n1_buf[i], 0, n1[i], &n1_buf_carry);
		}

		for (i = 0; i < n2_size - half_size; i++) {
			bc_karatsuba_mul_buf_calc_for_mid(&n2_buf[i], n2[i + half_size], n2[i], &n2_buf_carry);
		}
		for (; i < half_size; i++) {
			bc_karatsuba_mul_buf_calc_for_mid(&n2_buf[i], 0, n2[i], &n2_buf_carry);
		}

		/* mid */
		bc_karatsuba_mul_fast_recursive(n1_buf, n1_buf + half_size, half_size, n2_buf, n2_buf + half_size, half_size, half_size, mid_ex);

		/* If n1_buf has a carry, add n2_buf. The opposite is true when n2_buf has a carry. */
		bc_karatsuba_mul_add_buf_to_mid(mid_ex, n1_buf, n2_buf_carry, half_size);
		bc_karatsuba_mul_add_buf_to_mid(mid_ex, n2_buf, n1_buf_carry, half_size);

		if (ret_size - half_size >= mid_ret_size) {
			for (i = 0; i < high_ret_size; i++) {
				mid_ex[i] -= high[i] + low[i];
			}
			for (; i < low_ret_size; i++) {
				mid_ex[i] -= low[i];
			}
		} else {
			/*
			 * In this case, the large values ​​of mid and low cancel each other out, and mid_ex becomes a small value.
			 * However, due to differences in the calculation process, results may be incorrect if carry-over calculations
			 * are not performed.
			 * Values ​​larger than ret_size - half_size will be to 0, so it is sufficient to carry up to ret_size - half_size.
			 */
			for (i = 0; i < high_ret_size; i++) {
				mid_ex[i] -= high[i] + low[i];
				bc_karatsuba_mul_carry_calc_single(&mid_ex[i]);
			}
			for (; i < ret_size - half_size; i++) {
				mid_ex[i] -= low[i];
				bc_karatsuba_mul_carry_calc_single(&mid_ex[i]);
			}
			for (; i < low_ret_size; i++) {
				mid_ex[i] -= low[i];
			}
		}
	} else if (n1_size > half_size) {
		/*
		 * If only n1_size is greater than half_size, do the calculation a little differently.
		 * For example, in the multiplication ab * cd, when only n1_size is greater than half_size, c is 0.
		 * If we decompose this using the Karatsuba algorithm, we get the following.
		 * low = b * d
		 * high = a * 0
		 * mid = (a - b) * (0 - d)
		 * ret = high * 100 + (high + low - mid) * 10 + low
		 *     = 0 + (0 + b * d - (a - b) * -d) * 10 + b * d
		 *     = ((b + a - b) * d) * 10 + b * d
		 *     = (a * d) * 10 + b * d
		 * So use a * d instead of high + low - mid.
		 */
		size_t n1_high_size = n1_size - half_size;
		mid_ret_size = n1_high_size + n2_size;

		/* mid ex */
		if (n1_high_size == half_size && n2_size == half_size) {
			bc_karatsuba_mul_fast_recursive(n1 + half_size, n1_buf, half_size, n2, n2_buf, half_size, half_size, mid_ex);
		} else {
			if (n1_high_size >= n2_size) {
				bc_karatsuba_mul_recursive(n1 + half_size, n1_buf, n1_high_size, n2, n2_buf, n2_size, half_size, mid_ex);
			} else {
				bc_karatsuba_mul_recursive(n2, n2_buf, n2_size, n1 + half_size, n1_buf, n1_high_size, half_size, mid_ex);
			}
		}
	}

	/* Add low and high to ret */

	/*
	 * When skipping mid, there is no need to add mid. This is because mid is equal to
	 * low at this time, so the calculation result will always be 0.
	 */
	if (n2_size > half_size) {
		/* Case of not skipping high and mid */
		/* low_ret_size and mid ret size are always equal. */
		if (ret_size - half_size > mid_ret_size) {
			for (i = 0; i < mid_ret_size; i++) {
				ret[i + half_size] -= mid_ex[i];
			}
			/* n1_buf and n2_buf, if there is a carry that doesn't fit in the buffer, add/subtract it extra. */
			if (UNEXPECTED(n1_buf_carry != 0 && n2_buf_carry != 0)) {
				if ((n1_buf_carry == 1 && n2_buf_carry == 1) || (n1_buf_carry == (BC_VECTOR) -1 && n2_buf_carry == (BC_VECTOR) -1)) {
					ret[i + half_size] -= 1;
				} else {
					ret[i + half_size] += 1;
				}
			}
		} else {
			for (i = 0; i < ret_size - half_size; i++) {
				ret[i + half_size] -= mid_ex[i];
			}
			/* n1_buf and n2_buf, if there is a carry that doesn't fit in the buffer, add/subtract it extra. */
			if (UNEXPECTED(n1_buf_carry != 0 && n2_buf_carry != 0)) {
				if ((n1_buf_carry == 1 && n2_buf_carry == 1) || (n1_buf_carry == (BC_VECTOR) -1 && n2_buf_carry == (BC_VECTOR) -1)) {
					ret[i + half_size - 1] -= BC_VECTOR_BOUNDARY_NUM;
				} else {
					ret[i + half_size - 1] += BC_VECTOR_BOUNDARY_NUM;
				}
			}
		}
	} else if (n1_size > half_size) {
		/* Case to skip high and not use buffer for sub for mid calculation */
		for (i = low_ret_size; i < ret_size; i++) {
			ret[i] = 0;
		}
		for (i = 0; i < mid_ret_size; i++) {
			ret[i + half_size] += mid_ex[i];
		}
	}

	/* Calc carry if need */
	if (UNEXPECTED(bc_karatsuba_mul_need_carry_calc(calc_arr_size))) {
		bc_karatsuba_mul_carry_calc(ret, ret_size);
	}
}

/*
 * Returns the next largest power of 2 of val.
 * https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
 */
static inline size_t bc_mul_next_pow_2(size_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
#if SIZEOF_SIZE_T >= 8
	v |= v >> 32;
#endif
	v++;

	return v;
}

/*
 * For multiplication of very large values, use the Karatsuba algorithm.
 * https://en.wikipedia.org/w/index.php?title=Karatsuba_algorithm&oldid=1190009898
 */
static void bc_karatsuba_mul(bc_num n1, size_t n1len, bc_num n2, size_t n2len, bc_num *prod)
{
	const char *n1end = n1->n_value + n1len - 1;
	const char *n2end = n2->n_value + n2len - 1;
	size_t prodlen = n1len + n2len;

	size_t n1_arr_size = (n1len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t n2_arr_size = (n2len + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;
	size_t prod_arr_size = (prodlen + BC_VECTOR_SIZE - 1) / BC_VECTOR_SIZE;

	size_t calc_arr_size;
	size_t small_arr_pow_size;
	if (n1_arr_size >= n2_arr_size) {
		calc_arr_size = bc_mul_next_pow_2(n1_arr_size);
		small_arr_pow_size = bc_mul_next_pow_2(n2_arr_size);
	} else {
		calc_arr_size = bc_mul_next_pow_2(n2_arr_size);
		small_arr_pow_size = bc_mul_next_pow_2(n1_arr_size);
	}

	/*
	 * How to calculate memory size
	 *
	 * sub_n1_buf and sub_n2_buf:
	 * These are the necessary buffers for the divide and conquer "mid".
	 * Assuming the computation length is M, the first split uses a buffer of M/2, and the second
	 * split uses a buffer of M/4.... The required buffer size increases by half until the size
	 * reaches 2. In other words, it is the sum of a geometric progression of 1/2 with a geometric
	 * ratio of 1/2 from M(1) to 2(N).
	 * This sum can be calculated using the following formula, where the first term is a and the
	 * geometric ratio is r: a(r^(N-1) - 1)/(r - 1)
	 * M((1/2)^N - 1)/(1/2 - 1) = -2M((1/2)^N - 1) = -2M((1/2)^N) + 2M = -M((1/2)^(N-1)) + 2M
	 * Here, M((1/2)^(N-1)) is the last term, 2, so this formula becomes: 2M - 2
	 * In other words, the calculation length - 2 is the required buffer size.
	 * However, there are cases where the calculation of mid can be skipped, and the calculation length
	 * is actually the next largest power of 2 of the smaller of n1_arr_size and n2_arr_size.
	 * Let the smaller calculation length be small_arr_pow_size.
	 *
	 * prod_vector:
	 * Share the results and the buffers used in intermediate calculations. The result is prod_arr_size.
	 * The buffer increases by half like n_buf, but when calc_arr_size is 2, the required buffer size is 4.
	 * In other words, they are almost the same geometric progression, but the last term is 4.
	 * a(r^(N-1) - 1)/(r - 1): -M((1/2)^(N-1)) + 2M (Up to this point, it is the same as sub_n1_buf and sub_n2_buf)
	 * The last term is 4, so -M((1/2)^(N-1)) is 4. So: 2M - 4
	 * Therefore, the required size is prod_arr_size + calc_arr_size * 2 - 4.
	 *
	 * -----------------------------------------------
	 *
	 * Consider the case where prodlen is the maximum value of size_t (x).
	 *
	 * prod_arr_size is x/4 for 32-bit and x/8 for 64-bit.
	 *
	 * calc_arr_size and small_arr_pow_size depend on the larger
	 * and smaller of n1len and n2len, respectively. We are considering the case where prodlen is the maximum value
	 * of size_t, so when n1len increases, n2len decreases. In this case, calc_arr_size and small_arr_pow_size are each
	 * the largest when n1len is only slightly larger than n2len (or vice versa). This is because it takes a value that
	 * is a power of 2 larger than n1_arr_size/n2_arr_size.
	 * For the maximum values, calc_arr_size is approximately x/4 and small_arr_pow_size is approximately x/8 for 32-bit,
	 * and x/8 and x/16, respectively, for 64-bit.
	 *
	 * Need two each of prod_arr_size, calc_arr_size, and small_arr_pow_size, so the total is approximately 5x/4 for 32-bit
	 * and 5x/8 for 64-bit.
	 * Therefore, 64-bit allocates memory all at once, while 32-bit allocates memory in parts.
	 */
#if SIZEOF_SIZE_T >= 8
	BC_VECTOR *buf = safe_emalloc(prod_arr_size * 2 + small_arr_pow_size * 2 - 4 + calc_arr_size * 2 - 4, sizeof(BC_VECTOR), 0 );
	BC_VECTOR *n1_vector = buf;
	BC_VECTOR *n2_vector = buf + n1_arr_size;

	BC_VECTOR *sub_n1_buf = n2_vector + n2_arr_size;
	BC_VECTOR *sub_n2_buf = sub_n1_buf + small_arr_pow_size - 2;

	BC_VECTOR *prod_vector = sub_n2_buf + small_arr_pow_size - 2;
#else
	BC_VECTOR *buf = safe_emalloc(prod_arr_size + small_arr_pow_size * 2 - 4, sizeof(BC_VECTOR), 0 );
	BC_VECTOR *n1_vector = buf;
	BC_VECTOR *n2_vector = buf + n1_arr_size;

	BC_VECTOR *sub_n1_buf = n2_vector + n2_arr_size;
	BC_VECTOR *sub_n2_buf = sub_n1_buf + small_arr_pow_size - 2;

	BC_VECTOR *prod_vector = safe_emalloc(prod_arr_size + calc_arr_size * 2 - 4, sizeof(BC_VECTOR), 0);
#endif

	/* Convert to BC_VECTOR[] */
	bc_convert_to_vector(n1_vector, n1end, n1len);
	bc_convert_to_vector(n2_vector, n2end, n2len);

	/* Do multiplication */
	if (n1_arr_size >= n2_arr_size) {
		if (n2_arr_size == calc_arr_size) {
			/* If this condition is met, n1_arr_size, n2_arr_size, and calc_size are all equal, so use the fast version. */
			bc_karatsuba_mul_fast_recursive(n1_vector, sub_n1_buf, n1_arr_size, n2_vector, sub_n2_buf, n2_arr_size, calc_arr_size, prod_vector);
		} else {
			bc_karatsuba_mul_recursive(n1_vector, sub_n1_buf, n1_arr_size, n2_vector, sub_n2_buf, n2_arr_size, calc_arr_size, prod_vector);
		}
	} else {
		/* Pass the arguments in reverse so that n1_size on the function side is always larger than n2_size. */
		bc_karatsuba_mul_recursive(n2_vector, sub_n2_buf, n2_arr_size, n1_vector, sub_n1_buf, n1_arr_size, calc_arr_size, prod_vector);
	}

	/*
	 * Calc carry
	 * It is almost the same as bc_karatsuba_mul_carry_calc, but all "negative values" ​​are corrected to positive values.
	 */
	for (size_t i = 0; i < prod_arr_size - 1; i++) {
		if (prod_vector[i] <= BC_VECTOR_MAX_HALF) {
			/* "positive number" */
			prod_vector[i + 1] += prod_vector[i] / BC_VECTOR_BOUNDARY_NUM;
			prod_vector[i] %= BC_VECTOR_BOUNDARY_NUM;
		} else {
			/* "negative number" */
			BC_VECTOR abs = (BC_VECTOR) 0 - prod_vector[i];
			BC_VECTOR borrow = abs / BC_VECTOR_BOUNDARY_NUM;
			if (EXPECTED(abs % BC_VECTOR_BOUNDARY_NUM != 0)) {
				borrow++;
			}
			prod_vector[i + 1] -= borrow;
			prod_vector[i] +=  BC_VECTOR_BOUNDARY_NUM * borrow;
		}
	}

	/* Convert to bc_num */
	bc_mul_vector_to_bc_num(prod_vector, prodlen, prod_arr_size, prod);

	efree(buf);
#if SIZEOF_SIZE_T < 8
	efree(prod_vector);
#endif
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
	} else if (UNEXPECTED(len1 >= BC_MUL_USE_KARATSUBA_SIZE && len2 >= BC_MUL_USE_KARATSUBA_SIZE)) {
		bc_karatsuba_mul(n1, len1, n2, len2, &prod);
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
