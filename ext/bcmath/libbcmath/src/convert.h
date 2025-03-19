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

#include "private.h"

#ifndef BCMATH_CONVERT_H
#define BCMATH_CONVERT_H

char *bc_copy_and_toggle_bcd(char *restrict dest, const char *source, const char *source_end);
void bc_write_bcd_representation(uint32_t value, char *str);
BC_VECTOR bc_parse_chunk_chars(const char *str);

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
	BC_VECTOR digit_base_value = 1;

	for (size_t i = 0; i < len; i++) {
		num += *n * digit_base_value;
		digit_base_value *= BASE;
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

static inline void bc_convert_to_vector_with_zero_pad(BC_VECTOR *n_vector, const char *nend, size_t nlen, size_t zeros)
{
	while (zeros >= BC_VECTOR_SIZE) {
		*n_vector = 0;
		n_vector++;
		zeros -= BC_VECTOR_SIZE;
	}

	if (zeros > 0) {
		*n_vector = 0;
		BC_VECTOR digit_base_value = BC_POW_10_LUT[zeros];
		size_t len_to_write = MIN(BC_VECTOR_SIZE - zeros, nlen);
		for (size_t i = 0; i < len_to_write; i++) {
			*n_vector += *nend * digit_base_value;
			digit_base_value *= BASE;
			nend--;
		}
		n_vector++;
		nlen -= len_to_write;
	}

	if (nlen == 0) {
		return;
	}

	bc_convert_to_vector(n_vector, nend, nlen);
}

static inline void bc_convert_vector_to_char(const BC_VECTOR *vector, char *nptr, char *nend, size_t arr_size)
{
	size_t i = 0;
	while (i < arr_size - 1) {
#if BC_VECTOR_SIZE == 4
		bc_write_bcd_representation(vector[i], nend - 3);
		nend -= 4;
#else
		bc_write_bcd_representation(vector[i] / 10000, nend - 7);
		bc_write_bcd_representation(vector[i] % 10000, nend - 3);
		nend -= 8;
#endif
		i++;
	}

	/*
	 * The last digit may carry over.
	 * Also need to fill it to the end with zeros, so loop until the end of the string.
	 */
	BC_VECTOR last = vector[i];
	while (nend >= nptr) {
		*nend-- = last % BASE;
		last /= BASE;
	}
}

#endif
