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
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "bcmath.h"
#include "convert.h"
#include "private.h"
#include <stdbool.h>

static inline size_t bc_count_len_in_vector(BC_VECTOR vector)
{
	size_t len = 0;
	BC_VECTOR tmp = vector;
#if BC_VECTOR_SIZE == 8
	tmp /= 10000;
	if (tmp > 0) {
		len += 4;
	} else {
		tmp = vector;
	}
#endif

	tmp /= 100;
	if (tmp > 0) {
		len += 2;
	} else {
		tmp = vector;
	}

	if (tmp >= 10) {
		tmp += 2;
	} else {
		tmp += 1;
	}

	return len;
}

static inline void bc_bulk_convert_vector_to_num(BC_VECTOR vector, char *end)
{
#if BC_VECTOR_SIZE == 4
		bc_write_bcd_representation(vector, end - 3);
#else
		bc_write_bcd_representation(vector / 10000, end - 7);
		bc_write_bcd_representation(vector % 10000, end - 3);
#endif
}

static inline void bc_convert_vector_to_num(BC_VECTOR vector, const char *ptr, char *end)
{
	while (end >= ptr) {
		*end-- = vector % BASE;
		vector /= BASE;
	}
}

void bc_long2num(bc_num *num, zend_long lval)
{
	if (UNEXPECTED(lval == 0)) {
		*num = bc_copy_num(BCG(_zero_));
		return;
	}

	bool negative = false;
	if (lval < 0) {
		lval = -lval;
		negative = true;
	}

	BC_VECTOR low = lval % BC_VECTOR_BOUNDARY_NUM;
	lval /= BC_VECTOR_BOUNDARY_NUM;

	BC_VECTOR mid = lval % BC_VECTOR_BOUNDARY_NUM;
	BC_VECTOR high = lval / BC_VECTOR_BOUNDARY_NUM;

	size_t len;
	size_t vector_size;
	if (high > 0) {
		len = bc_count_len_in_vector(high) + BC_VECTOR_SIZE * 2;
		vector_size = 3;
	} else if (mid > 0) {
		len = bc_count_len_in_vector(mid) + BC_VECTOR_SIZE;
		vector_size = 2;
	} else {
		len = bc_count_len_in_vector(low);
		vector_size = 1;
	}

	*num = bc_new_num_nonzeroed(len, 0);
	(*num)->n_sign = negative ? MINUS : PLUS;
	char *ptr = (*num)->n_value;
	char *end = ptr + len - 1;

	switch (vector_size) {
		case 3:
			bc_bulk_convert_vector_to_num(low, end);
			end -= BC_VECTOR_SIZE;
			bc_bulk_convert_vector_to_num(mid, end);
			end -= BC_VECTOR_SIZE;
			bc_convert_vector_to_num(low, ptr, end);
			break;
		case 2:
			bc_bulk_convert_vector_to_num(low, end);
			end -= BC_VECTOR_SIZE;
			bc_convert_vector_to_num(low, ptr, end);
			break;
		case 1:
			bc_convert_vector_to_num(low, ptr, end);
			break;
	}
}
