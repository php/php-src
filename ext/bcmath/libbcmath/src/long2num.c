/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "bcmath.h"
#include <stdbool.h>
#include "convert.h"

#define BC_LONG_MAX_DIGITS (sizeof(LONG_MIN_DIGITS) - 1)

bc_num bc_long2num(zend_long lval)
{
	bc_num num;

	if (UNEXPECTED(lval == 0)) {
		num = bc_copy_num(BCG(_zero_));
		return num;
	}

	bool negative = lval < 0;
	if (UNEXPECTED(lval == ZEND_LONG_MIN)) {
		num = bc_new_num_nonzeroed(BC_LONG_MAX_DIGITS, 0);
		const char *ptr = LONG_MIN_DIGITS;
		bc_copy_and_toggle_bcd(num->n_value, ptr, ptr + BC_LONG_MAX_DIGITS);
		num->n_sign = MINUS;
		return num;
	} else if (negative) {
		lval = -lval;
	}

	zend_long tmp = lval;
	size_t len = 0;
	while (tmp > 0) {
		tmp /= BASE;
		len++;
	}

	num = bc_new_num_nonzeroed(len, 0);
	char *ptr = num->n_value + len - 1;
	for (; len > 0; len--) {
		*ptr-- = lval % BASE;
		lval /= BASE;
	}
	num->n_sign = negative ? MINUS : PLUS;

	return num;
}
