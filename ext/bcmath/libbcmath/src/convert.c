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
#ifdef __SSE2__
# include <emmintrin.h>
#endif

static char *bc_copy_and_shift_numbers(char *restrict dest, const char *source, const char *source_end, unsigned char shift, bool add)
{
	size_t bulk_shift = SWAR_REPEAT(shift);
	if (!add) {
		bulk_shift = -bulk_shift;
		shift = -shift;
	}

#ifdef __SSE2__
	/* SIMD SSE2 bulk shift + copy */
	__m128i shift_vector = _mm_set1_epi8(shift);
	while (source + sizeof(__m128i) <= source_end) {
		__m128i bytes = _mm_loadu_si128((const __m128i *) source);
		bytes = _mm_add_epi8(bytes, shift_vector);
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

		bytes += bulk_shift;
		memcpy(dest, &bytes, sizeof(bytes));

		source += sizeof(size_t);
		dest += sizeof(size_t);
	}

	while (source < source_end) {
		*dest = *source + shift;
		dest++;
		source++;
	}

	return dest;
}

char *bc_copy_ch_val(char *restrict dest, const char *source, const char *source_end)
{
	return bc_copy_and_shift_numbers(dest, source, source_end, '0', false);
}

char *bc_copy_bcd_val(char *restrict dest, const char *source, const char *source_end)
{
	return bc_copy_and_shift_numbers(dest, source, source_end, '0', true);
}
