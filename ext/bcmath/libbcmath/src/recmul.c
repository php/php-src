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


#if SIZEOF_SIZE_T >= 8
#  define BC_MUL_UINT_DIGITS 8
#  define BC_MUL_UINT_OVERFLOW (BC_UINT_T) 100000000
#else
#  define BC_MUL_UINT_DIGITS 4
#  define BC_MUL_UINT_OVERFLOW (BC_UINT_T) 10000
#endif

/*
 * If n1 and n2 are both greater than this order of magnitude, use the
 * divide-and-conquer method (as a result of measurement, a clear speed
 * difference appears from this order of magnitude).
 * If the number of digits is small, the overhead impact is large and slow.
 */
#define BC_MUL_MAX_ADD_COUNT (~((BC_UINT_T) 0) / (BC_MUL_UINT_OVERFLOW * BC_MUL_UINT_OVERFLOW))

/*
 * In divide-and-conquer calculations, additions are concentrated on array
 * entries around half of the ret size length.
 * e.g. ret size is 8, [7][6][5][4][3][2][1][0]
 * In this case, addition is most concentrated on [3].
 *
 * Therefore, when the addition is repeated, that entry will be the first to
 * overflow. Defines the maximum calculation length for this entry without overflowing.
 *
 * As can see by actually doing it, the value added to [3] in this example is the
 * accumulation of all (high + low - mid) when calculating 2 digits.
 * In this example, the ret size is 8, so the calculation length is 4, and from 4^1.585,
 * (If normal multiplication of N digits and N digits involves multiplying one digit N^2
 * times, the Karatsuba-algorithm requires N^log2(3) times of calculation. N^log2(3) is
 * approximately N^1.585.)
 * there is a minimum unit calculation set of 9, so add (high + low - mid) 9 times.
 *
 * At this time, if consider one portion (high + low - mid) using ab*cd as an example,
 * this becomes: ac + bd - (a-b)(c-d) = ac + bd - ac + ad + bc - bd = ad + bc
 * Can see that the maximum value of this is obtained by 99*99.
 *
 * This law holds regardless of the calculation length, so when considering the
 * maximum value, all mids are canceled out and can be ignored. Therefore, mid and all
 * calculations that further divide mid can be ignored from the calculation results that
 * are being accumulated.
 * In other words, if the calculation length is N and the minimum calculation unit length
 * is 2, there are N/2 high and low pairs. Therefore, the number of times the value is
 * added is N times.
 *
 * If all the values ​​are the maximum value, adding more times than BC_MUL_MAX_ADD_COUNT
 * will cause an overflow, so specify the maximum power of 2 within the range below
 * BC_MUL_MAX_ADD_COUNT (because the calculation length is always adjusted to the power of 2).
 */
#if SIZEOF_SIZE_T >= 8
   /* BC_MUL_MAX_ADD_COUNT is 1844 in 64-bit, so specify the next smallest power of 2. */
#  define BC_REC_MUL_DO_ADJUST_EXPO 1024
#  define BC_USE_REC_MUL_DIGITS 160 * 8
#else
   /* As with 64-bit, specify the next smallest power of 2 after 42. */
#  define BC_REC_MUL_DO_ADJUST_EXPO 32
#  define BC_USE_REC_MUL_DIGITS 160 * 8
#endif


/* Multiply utility routines */

static inline void bc_digits_adjustment_single(BC_UINT_T *ret)
{
	ret[1] += ret[0] / BC_MUL_UINT_OVERFLOW;
	ret[0] %= BC_MUL_UINT_OVERFLOW;
}

static inline void bc_digits_adjustment(BC_UINT_T *prod_uint, size_t prod_arr_size)
{
	for (size_t i = 0; i < prod_arr_size - 1; i++) {
		bc_digits_adjustment_single(prod_uint + i);
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
static uint32_t bc_parse_chunk_chars(const char *str)
{
	uint32_t tmp;
	memcpy(&tmp, str, sizeof(tmp));
#if !BC_LITTLE_ENDIAN
	tmp = BC_BSWAP(tmp);
#endif

	uint32_t lower_digits = (tmp & 0x0f000f00) >> 8;
	uint32_t upper_digits = (tmp & 0x000f000f) * 10;

	tmp = lower_digits + upper_digits;

	lower_digits = (tmp & 0x00ff0000) >> 16;
	upper_digits = (tmp & 0x000000ff) * 100;

	return lower_digits + upper_digits;
}
#elif SIZEOF_SIZE_T == 8
static uint64_t bc_parse_chunk_chars(const char *str)
{
	uint64_t tmp;
	memcpy(&tmp, str, sizeof(tmp));
#if !BC_LITTLE_ENDIAN
	tmp = BC_BSWAP(tmp);
#endif

	uint64_t lower_digits = (tmp & 0x0f000f000f000f00) >> 8;
	uint64_t upper_digits = (tmp & 0x000f000f000f000f) * 10;

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
 * Converts BCD to uint, going backwards from pointer n by the number of
 * characters specified by len.
 */
static inline BC_UINT_T bc_partial_convert_to_uint(const char *n, size_t len)
{
	if (len == BC_MUL_UINT_DIGITS) {
		return bc_parse_chunk_chars(n - BC_MUL_UINT_DIGITS + 1);
	}

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
static inline void bc_fast_mul(bc_num n1, size_t n1len, bc_num n2, size_t n2len, bc_num *prod)
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

static inline void bc_mul_convert_uint_to_bcd(BC_UINT_T *prod_uint, size_t prodlen,  size_t prod_arr_size, bc_num *prod)
{
	*prod = bc_new_num_nonzeroed(prodlen, 0);
	char *pptr = (*prod)->n_value;
	char *pend = pptr + prodlen - 1;
	size_t i = 0;
	while (i < prod_arr_size - 1) {
#if BC_MUL_UINT_DIGITS == 4
		bc_write_bcd_representation(prod_uint[i], pend - 3);
		pend -= 4;
#else
		bc_write_bcd_representation(prod_uint[i] / 10000, pend - 7);
		bc_write_bcd_representation(prod_uint[i] % 10000, pend - 3);
		pend -= 8;
#endif
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
	bc_mul_convert_uint_to_bcd(prod_uint, prodlen, prod_arr_size, prod);

	efree(buf);
}

/*
 * In divide-and-conquer calculations, determine whether the calculation length is
 * such that digits should be adjusted to prevent overflow during calculation.
 * Digit adjustment is performed when the calculation length is a power of
 * BC_REC_MUL_DO_ADJUST_EXPO.
 */
static inline bool bc_rec_mul_near_overflow(size_t calc_size)
{
	if (EXPECTED(calc_size < BC_REC_MUL_DO_ADJUST_EXPO)) {
		return false;
	}

	while (calc_size > 0) {
		calc_size /= BC_REC_MUL_DO_ADJUST_EXPO;
		if (UNEXPECTED(calc_size == 1)) {
			return true;
		}
	}
	return false;
}

/*
 * This is a function that is called recursively in divide-and-conquer calculations.
 * if the size of n1 and n2 are both equal to calc_size, the calculation is performed
 * without checking the number of digits.
 * See bc_rec_mul's comment for the meaning of high, mid, and low.
 *
 * Also, functions called recursively within this function will always use the fast version,
 * since all sizes are equal to half_size.
 */
static void bc_rec_mul_recursive_fast(
	const BC_UINT_T *n1, BC_UINT_T *n1_buf,
	const BC_UINT_T *n2, BC_UINT_T *n2_buf,
	size_t calc_size, BC_UINT_T *ret)
{
	/*
	 * Size 2 is the smallest unit.
	 * The calculation itself is skipped for the combination of size 2 and 0, and the array
	 * size is adjusted to a multiple of 2, so the combination of 2 and 1 is impossible.
	 * Therefore, the minimum unit calculation is always performed using this func.
	 */
	if (calc_size == 2) {
		BC_UINT_T low = n1[0] * n2[0];
		BC_UINT_T high = n1[1] * n2[1];
		ret[0] = low;
		ret[1] = low + high - (n1[1] - n1[0]) * (n2[1] - n2[0]);
		ret[2] = high;
		ret[3] = 0;
		return;
	}

	size_t i;
	size_t half_size = calc_size / 2;

	/* low */
	BC_UINT_T *low = ret;
	bc_rec_mul_recursive_fast(n1, n1_buf, n2, n2_buf, half_size, low);
	/* high */
	BC_UINT_T *high = ret + calc_size;
	bc_rec_mul_recursive_fast(n1 + half_size, n1_buf, n2 + half_size, n2_buf, half_size, high);

	/* prepare mid */
	for (i = 0; i < half_size; i++) {
		size_t high_i = i + half_size;
		n1_buf[i] = n1[high_i] - n1[i];
		n2_buf[i] = n2[high_i] - n2[i];
	}

	/* mid */
	BC_UINT_T *mid = high + calc_size;
	bc_rec_mul_recursive_fast(n1_buf, n1_buf + half_size, n2_buf, n2_buf + half_size, half_size, mid);

	/*
	 * Add to ret.
	 * From 0 to half_size, adding it to ret will affect the low value, so add it later.
	 */
	for (i = 0; i < half_size; i++) {
		mid[i] -= low[i] + high[i];
	}
	for (; i < calc_size; i++) {
		ret[i + half_size] += low[i] + high[i] - mid[i];
	}
	for (i = 0; i < half_size; i++) {
		ret[i + half_size] -= mid[i];
	}

	/*
	 * Perform digit adjustment to avoid overflow.
	 */
	if (UNEXPECTED(bc_rec_mul_near_overflow(calc_size))) {
		for (i = 0; i < calc_size * 2 - 1; i++) {
			bc_digits_adjustment_single(ret + i);
		}
	}
}

/*
 * This is a function that is called recursively in divide-and-conquer calculations.
 * Compute the three multiplications of Karatsuba-algorithm as high, mid, and low, respectively.
 * e.g. 1234 * 5678
 * low = 34 * 78
 * high = 12 * 56
 * mid = (12 - 34) * (56 - 78)
 * ret = high * 100^2 + (high + low - mid) * 100 + low
 * This function differs from "fast" in that it does things like check the number of digits.
 *
 * Since the sizes of n1 and n2 are always even numbers, the possible combinations when size=2,
 * which is the smallest calculation unit, are 2size * 0size or 2size * 2size.
 * The former always uses the fast version, and the latter skips the calculation, so there is
 * no need for this function to calculate the smallest unit.
 */
static void bc_rec_mul_recursive(
	const BC_UINT_T *n1, BC_UINT_T *n1_buf, size_t n1_size,
	const BC_UINT_T *n2, BC_UINT_T *n2_buf, size_t n2_size,
	size_t calc_size, BC_UINT_T *ret)
{
	size_t i;
	size_t half_size = calc_size / 2;

	/*
	 * Find the size of the values ​​used to calculate high and low. It also checks whether
	 * to skip calculating high.
	 */
	bool fast;
	bool skip_high;
	size_t n1_low_size;
	size_t n1_high_size;
	size_t n2_low_size;
	size_t n2_high_size;
	if (n1_size > half_size) {
		n1_low_size = half_size;
		n1_high_size = n1_size - half_size;
	} else {
		n1_low_size = n1_size;
		n1_high_size = 0;
	}
	if (n2_size > half_size) {
		n2_low_size = half_size;
		n2_high_size = n2_size - half_size;
		skip_high = false;
		fast = true;
	} else {
		n2_low_size = n2_size;
		n2_high_size = 0;
		skip_high = true;
		fast = n2_size == half_size;
	}

	/*
	 * low
	 * low_ret_size may be smaller than calc size as a programmatic convenience to save buffer size.
	 * If the size of n1 and n2 is always equal to calc_size and padded on the left with 0, then
	 * low_ret_size is always calc_size.
	 * e.g.: Value when saving buffer (value when filled with 0)
	 * n1low = 1234(1234), n2low = 12(0012), calc_size = 8
	 * n1_low_size = 4(4), n2_low_size = 2(4), low_ret_size = 6(8)
	 */
	BC_UINT_T *low = ret;
	size_t low_ret_size = n1_low_size + n2_low_size;
	if (fast) {
		/*
		 * When n1_low_size and n2_low_size are both equal to half_size, use a version
		 * that skips the size check.
		 */
		bc_rec_mul_recursive_fast(n1, n1_buf, n2, n2_buf, half_size, low);
	} else {
		bc_rec_mul_recursive(n1, n1_buf, n1_low_size, n2, n2_buf, n2_low_size, half_size, low);
	}

	/* high */
	BC_UINT_T *high = low + low_ret_size;
	size_t high_ret_size = n1_high_size + n2_high_size;
	if (!skip_high) {
		bc_rec_mul_recursive(n1 + half_size, n1_buf, n1_high_size, n2 + half_size, n2_buf, n2_high_size, half_size, high);
	} else {
		/*
		 * Actually, should think of filling calc_size - low_ret_size with 0 and filling
		 * high_ret_size - (calc_size - low_ret_size) with 0, but the result is the same,
		 * so write it like this.
		 */
		for (i = 0; i < high_ret_size; i++) {
			high[i] = 0;
		}
	}

	/* prepare mid */
	for (i = 0; i < n1_high_size; i++) {
		n1_buf[i] = n1[i + half_size] - n1[i];
	}
	for (; i < n1_low_size; i++) {
		n1_buf[i] = -n1[i];
	}

	for (i = 0; i < n2_high_size; i++) {
		n2_buf[i] = n2[i + half_size] - n2[i];
	}
	for (; i < n2_low_size; i++) {
		n2_buf[i] = -n2[i];
	}

	/* mid */
	BC_UINT_T *mid = high + high_ret_size;
	size_t n1_mid_size = n1_low_size;
	size_t n2_mid_size = n2_low_size;
	size_t mid_ret_size = low_ret_size;
	if (fast) {
		/*
		 * When n1_mid_size and n2_mid_size are both equal to half_size, use a version
		 * that skips the size check.
		 */
		bc_rec_mul_recursive_fast(n1_buf, n1_buf + half_size, n2_buf, n2_buf + half_size, half_size, mid);
	} else {
		bc_rec_mul_recursive(
			n1_buf, n1_buf + n1_mid_size, n1_mid_size, n2_buf, n2_buf + n2_mid_size, n2_mid_size, half_size, mid);
	}

	/* Add to ret */
	i = 0;
	size_t limit_size;
	if (!skip_high) {
		limit_size = MIN(high_ret_size, half_size) + half_size;
		for (i = 0; i < high_ret_size; i++) {
			mid[i] -= low[i] + high[i];
		}
	} else {
		limit_size = MIN(low_ret_size + high_ret_size, mid_ret_size + half_size);
	}
	for (; i < mid_ret_size; i++) {
		mid[i] -= low[i];
	}
	for (i = 0; i < limit_size; i++) {
		ret[i + half_size] -= mid[i];
	}

	/*
	 * Perform digit adjustment to avoid overflow.
	 */
	if (UNEXPECTED(bc_rec_mul_near_overflow(calc_size))) {
		size_t ret_size = low_ret_size + high_ret_size;
		for (i = 0; i < ret_size - 1; i++) {
			bc_digits_adjustment_single(ret + i);
		}
	}
}

/*
 * Returns the next largest power of 2 of val.
 */
static inline size_t bc_mul_next_pow_2(size_t val)
{
	if ((val & (val - 1)) == 0) {
		return val;
	}

	size_t ret = 1;
	while (val > 0) {
		val >>= 1;
		ret <<= 1;
	}

	return ret;
}

/*
 * Compute multiplication quickly using divide and conquer.
 * The actual calculation calls bc_rec_mul_recursive recursively.
 *
 * For example, calculating 12 * 34 using normal methods requires four multiplications
 * (single-digit).
 * When performing calculations using an algorithm called the Karatsuba algorithm, only
 * three multiplications are required.
 * This difference becomes larger as the number of digits increases, and when multiplying
 * N digits, the usual method requires N^2 multiplications, but the number can be reduced
 * to about N^1.585.
 */
static void bc_rec_mul(bc_num n1, size_t n1len, bc_num n2, size_t n2len, bc_num *prod)
{
	size_t i;
	const char *n1end = n1->n_value + n1len - 1;
	const char *n2end = n2->n_value + n2len - 1;
	size_t prodlen = n1len + n2len;

	size_t n1_arr_size = (n1len + BC_MUL_UINT_DIGITS - 1) / BC_MUL_UINT_DIGITS;
	size_t n2_arr_size = (n2len + BC_MUL_UINT_DIGITS - 1) / BC_MUL_UINT_DIGITS;
	/*
	 * For computational efficiency, the size of prod_uint is slightly larger.
	 * Therefore, calculate the actual required size to avoid overflow when converting to BCD.
	 */
	size_t prod_arr_real_size = (prodlen + BC_MUL_UINT_DIGITS - 1) / BC_MUL_UINT_DIGITS;

	/*
	 * Adjust size to a multiple of 2 for computational efficiency.
	 */
	n1_arr_size = (n1_arr_size + 1) & ~1;
	n2_arr_size = (n2_arr_size + 1) & ~1;
	size_t prod_arr_size = n1_arr_size + n2_arr_size;

	/*
	 * Find the next largest power of 2 for n1_arr_size and n2_arr_size.
	 * Use the larger one as calc_size. This is the computational length for the
	 * divide and conquer calculation. e.g. For 7 * 3, the powers of 2 are 8 and 4,
	 * and the larger one, 8, is the calc_size.
	 *
	 * Each value found here is used to calculate the buffer size to hold the
	 * subtraction result of the divide-and-conquer mid calculation (so the variables
	 * are reused).
	 *
	 * How to calculate the buffer size is explained in the memory allocation comments.
	 */
	size_t calc_size;
	size_t n1_buf_size = bc_mul_next_pow_2(n1_arr_size);
	size_t n2_buf_size = bc_mul_next_pow_2(n2_arr_size);
	if (n1_buf_size > n2_buf_size) {
		/*
		 * If arr_size is small, size adjustment will be performed. See the memory
		 * allocation comments for more details.
		 */
		n2_buf_size += n2_arr_size * (n1_buf_size / n2_buf_size - 1);
		calc_size = n1_buf_size;
	} else if (n1_buf_size < n2_buf_size) {
		/*
		 * If arr_size is small, size adjustment will be performed. See the memory
		 * allocation comments for more details.
		 */
		n1_buf_size += n1_arr_size * (n2_buf_size / n1_buf_size - 1);
		calc_size = n2_buf_size;
	} else {
		calc_size = n1_buf_size;
	}

	/*
	 * How to calculate memory size
	 *
	 * n1_uint, n2_uint:
	 * Use arr_size. The sum of these is prod_arr_size.
	 *
	 * n1_buf, n2_buf:
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
	 *
	 * However, if n_arr_size is very small compared to the calculation length, may not need a buffer
	 * half of the calculation length when dividing. If n_arr_size is smaller than half of the calculation
	 * length, the buffer size required during the division is equal to n_arr_size.
	 * Therefore, the buffer size required in this case is the number of divisions until n_arr_size is
	 * larger than half of the calculation length * n_arr_size + the sum of the calculations of the geometric
	 * progression after that calculation length.
	 * e.g. 62size(n1) * 7size(n2) => 64size * 8size (calc_size = 64)
	 * divide and conquer 1: 32size * 8size (n2 requires 8 buf size)
	 * divide and conquer 2: 16size * 8size (n2 requires 8 buf size)
	 * divide and conquer 3: 8size * 8size (n2 requires 8 buf size)
	 * divide and conquer 4: 4size * 4size (n2 requires 4 buf size)
	 * divide and conquer 5: 2size * 2size (n2 requires 2 buf size)
	 * So, the required buffer size is (8 * 3) + (2^3 - 2) = 30.
	 *
	 * prod_uint:
	 * Share the results and the buffers used in intermediate calculations. The result is prod_arr_size.
	 * The buffer increases by half like n_buf, but when calc_size is 2, the required buffer size is 4.
	 * In other words, they are almost the same geometric progression, but the last term is 4.
	 * a(r^(N-1) - 1)/(r - 1): -M((1/2)^(N-1)) + 2M (Up to this point, it is the same as n1_buf and n2_buf)
	 * The last term is 4, so -M((1/2)^(N-1)) is 4. So: 2M - 4
	 * Therefore, the required size is prod_arr_size + calc_size * 2 - 4.
	 */
	n1_buf_size -= 2;
	n2_buf_size -= 2;

	BC_UINT_T *buf = safe_emalloc(prod_arr_size * 2 + calc_size * 2 - 4 + n1_buf_size + n2_buf_size, sizeof(BC_UINT_T), 0);
	BC_UINT_T *n1_uint = buf;
	BC_UINT_T *n2_uint = buf + n1_arr_size;
	BC_UINT_T *n1_buf = n2_uint + n2_arr_size;
	BC_UINT_T *n2_buf = n1_buf + n1_buf_size;
	BC_UINT_T *prod_uint = n2_buf + n2_buf_size;

	/*
	 * The last entry may not be initialized because the size is adjusted to an even number for
	 * computational efficiency. So, initialize it to 0.
	 */
	n1_uint[n1_arr_size - 1] = 0;
	n2_uint[n2_arr_size - 1] = 0;

	/* Convert to uint[] */
	bc_convert_to_uint(n1_uint, n1end, n1len);
	bc_convert_to_uint(n2_uint, n2end, n2len);

	/*
	 * divide and conquer mul.
	 * For computational efficiency, n1 should always be the larger value.
	 */
	if (n1_arr_size >= n2_arr_size) {
		if (n2_arr_size == calc_size) {
			/*
			 * n1_arr_size is never greater than calc_size, so if this condition is met,
			 * n1_arr_size = n2_arr_size = calc_size.
			 * In this case, use the fast version that omits the size check.
			 */
			bc_rec_mul_recursive_fast(n1_uint, n1_buf, n2_uint, n2_buf, calc_size, prod_uint);
		} else {
			bc_rec_mul_recursive(n1_uint, n1_buf, n1_arr_size, n2_uint, n2_buf, n2_arr_size, calc_size, prod_uint);
		}
	} else {
		bc_rec_mul_recursive(n2_uint, n2_buf, n2_arr_size, n1_uint, n1_buf, n1_arr_size, calc_size, prod_uint);
	}

	/*
	 * Move a value exceeding 4/8 digits by carrying to the next digit.
	 * However, the last digit does nothing.
	 */
	for (i = 0; i < prod_arr_real_size - 1; i++) {
		bc_digits_adjustment_single(prod_uint + i);
	}

	/* Convert to bc_num */
	bc_mul_convert_uint_to_bcd(prod_uint, prodlen, prod_arr_real_size, prod);

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
	} else if (UNEXPECTED(len1 >= BC_USE_REC_MUL_DIGITS && len2 >= BC_USE_REC_MUL_DIGITS)) {
		bc_rec_mul(n1, len1, n2, len2, &prod);
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
