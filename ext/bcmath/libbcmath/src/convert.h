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

#endif
