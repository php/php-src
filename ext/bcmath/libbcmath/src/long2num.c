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
#include <stdbool.h>

#if SIZEOF_ZEND_LONG == 8
#  define BC_LONG_MAX_DIGITS 19
#else
#  define BC_LONG_MAX_DIGITS 10
#endif

void bc_long2num(bc_num *num, zend_long lval)
{
	if (UNEXPECTED(lval == 0)) {
		*num = bc_copy_num(BCG(_zero_));
		return;
	}

	bool negative = lval < 0;
	if (UNEXPECTED(lval == LONG_MIN)) {
		*num = bc_new_num_nonzeroed(BC_LONG_MAX_DIGITS, 0);
		char *ptr = (*num)->n_value;
		memcpy(ptr, LONG_MIN_DIGITS, BC_LONG_MAX_DIGITS);
		for (size_t i = 0; i < BC_LONG_MAX_DIGITS; i++) {
			*ptr = *ptr - '0';
			ptr++;
		}
		(*num)->n_sign = MINUS;
		return;
	} else if (negative) {
		lval = -lval;
	}

	zend_long tmp = lval;
	size_t len = 0;
	while (tmp > 0) {
		tmp /= BASE;
		len++;
	}

	*num = bc_new_num_nonzeroed(len, 0);
	char *ptr = (*num)->n_value + len - 1;
	for (; len > 0; len--) {
		*ptr-- = lval % BASE;
		lval /= BASE;
	}
	(*num)->n_sign = negative ? MINUS : PLUS;
}
