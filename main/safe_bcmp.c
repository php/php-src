/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
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
