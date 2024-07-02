/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "bcmath.h"
#include "convert.h"
#include "private.h"
#ifdef __SSE2__
# include <emmintrin.h>
#endif

char *bc_copy_and_toggle_bcd(char *restrict dest, const char *source, const char *source_end)
{
	const size_t bulk_shift = SWAR_REPEAT('0');

#ifdef __SSE2__
	/* SIMD SSE2 bulk shift + copy */
	__m128i shift_vector = _mm_set1_epi8('0');
	while (source + sizeof(__m128i) <= source_end) {
		__m128i bytes = _mm_loadu_si128((const __m128i *) source);
		bytes = _mm_xor_si128(bytes, shift_vector);
		_mm_storeu_si128((__m128i *) dest, bytes);

		source += sizeof(__m128i);
		dest += sizeof(__m128i);
	}
#endif

	/* Handle sizeof(size_t) (i.e. 4/8) bytes at once.
	 * We know that adding/subtracting an individual byte cannot overflow,
	 * so it is possible to add/subtract an entire word of bytes at once
	 * by using SWAR_REPEAT. */
	while (source + sizeof(size_t) <= source_end) {
		size_t bytes;
		memcpy(&bytes, source, sizeof(bytes));

		bytes ^= bulk_shift;
		memcpy(dest, &bytes, sizeof(bytes));

		source += sizeof(size_t);
		dest += sizeof(size_t);
	}

	while (source < source_end) {
		*dest = *source ^ '0';
		dest++;
		source++;
	}

	return dest;
}

/* This is based on the technique described in https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html.
 * This function transforms AABBCCDD into 1000 * AA + 100 * BB + 10 * CC + DD,
 * with the caveat that all components must be in the interval [0, 25] to prevent overflow
 * due to the multiplication by power of 10 (10 * 25 = 250 is the largest number that fits in a byte).
 * The advantage of this method instead of using shifts + 3 multiplications is that this is cheaper
 * due to its divide-and-conquer nature.
 */
#if SIZEOF_SIZE_T == 4
BC_VECTOR bc_parse_chunk_chars(const char *str)
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
BC_VECTOR bc_parse_chunk_chars(const char *str)
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
void bc_write_bcd_representation(uint32_t value, char *str)
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
