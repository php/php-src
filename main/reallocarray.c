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
  | Author: David Carlier <devnexen@gmail.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#ifndef HAVE_REALLOCARRAY

#include <string.h>

PHPAPI void* php_reallocarray(void *p, size_t nmb, size_t siz)
{
#ifndef _WIN32
	size_t r;
	if (__builtin_mul_overflow(nmb, siz, &r)) {
#else
        // nmb an siz must be lower than sqrt SIZE_MAX+1
        static size_t elim = 1UL << (sizeof(size_t) * CHAR_BIT / 2);
	if ((nmb >= elim || siz >= elim) && nmb > 0  && SIZE_MAX / nmb < siz) {
#endif
		// EOVERFLOW may have been, arguably, more appropriate
		// but this is what other implementations set
		errno = ENOMEM;
		return NULL;
	}

	return realloc(p, nmb * siz);
}
#endif
