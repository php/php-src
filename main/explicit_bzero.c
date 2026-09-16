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
*/

#include "php.h"

#ifndef HAVE_EXPLICIT_BZERO

/*	$OpenBSD: explicit_bzero.c,v 1.4 2015/08/31 02:53:57 guenther Exp $ */
/*
 * Public domain.
 * Written by Matthew Dempsky.
 */

#include <string.h>

PHPAPI void php_explicit_bzero(void *dst, size_t siz)
{
#ifdef HAVE_MEMSET_EXPLICIT /* C23 */
	memset_explicit(dst, 0, siz);
#elif defined(HAVE_EXPLICIT_MEMSET) /* NetBSD-specific */
	explicit_memset(dst, 0, siz);
#elif defined(PHP_WIN32)
	RtlSecureZeroMemory(dst, siz);
#elif defined(__GNUC__)
	memset(dst, 0, siz);
	asm __volatile__("" :: "r"(dst) : "memory");
#else
	size_t i = 0;
	volatile unsigned char *buf = (volatile unsigned char *)dst;

	for (; i < siz; i ++)
		buf[i] = 0;
#endif
}
#endif
