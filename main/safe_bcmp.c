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
  | Author: David Carlier <devnexen@gmail.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#include <string.h>

/*
 * Returns 0 if both inputs match, non-zero if they don't.
 * Returns -1 early if inputs do not have the same lengths.
 *
 */
PHPAPI int php_safe_bcmp(const zend_string *a, const zend_string *b)
{
	const volatile unsigned char *ua = (const volatile unsigned char *)ZSTR_VAL(a);
	const volatile unsigned char *ub = (const volatile unsigned char *)ZSTR_VAL(b);
	size_t i = 0;
	int r = 0;

	if (ZSTR_LEN(a) != ZSTR_LEN(b)) {
		return -1;
	}

	/* This is security sensitive code. Do not optimize this for speed. */
	while (i < ZSTR_LEN(a)) {
		r |= ua[i] ^ ub[i];
		++i;
	}

	return r;
}
