/*
  +----------------------------------------------------------------------+
  | PHP Version 8                                                        |
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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#include <string.h>

PHPAPI int php_safe_bcmp(const void *a, const void *b, size_t siz)
{
	const volatile unsigned char *ua = (const volatile unsigned char *)a;
	const volatile unsigned char *ub = (const volatile unsigned char *)b;
	size_t i = 0;
	int r = 0;

	while (i < siz) {
		r |= ua[i] ^ ub[i];
		++i;
	}

	return r;
}
