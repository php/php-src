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
  | Author:                                                              |
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
#ifdef HAVE_EXPLICIT_MEMSET
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
