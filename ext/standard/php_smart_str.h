/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SMART_STR_H
#define PHP_SMART_STR_H

#include "php_smart_str_public.h"

#define smart_str_0(x) ((x)->c[(x)->len] = '\0')

#define smart_str_alloc(d,n) {\
	/* if (!d->c) d->len = d->a = 0; */ \
	if (n >= d->a) {\
		d->c = erealloc(d->c, n + 129); \
		d->a = n + 128; \
	}\
}

#define smart_str_appends(dest, src) smart_str_appendl(dest, src, sizeof(src)-1)

static inline void smart_str_appendc(smart_str *dest, char c)
{
	++dest->len;
	smart_str_alloc(dest, dest->len);
	dest->c[dest->len - 1] = c;
}

static inline void smart_str_free(smart_str *s)
{
	if (s->c) {
		efree(s->c);
		s->c = NULL;
	}
	s->a = s->len = 0;
}

static inline void smart_str_appendl(smart_str *dest, const char *src, size_t len)
{
	size_t newlen;

	newlen = dest->len + len;
	smart_str_alloc(dest, newlen);
	memcpy(dest->c + dest->len, src, len);
	dest->len = newlen;
}

static inline void smart_str_append(smart_str *dest, smart_str *src)
{
	smart_str_appendl(dest, src->c, src->len);
}

static inline void smart_str_setl(smart_str *dest, const char *src, size_t len)
{
	dest->len = len;
	dest->a = len + 1;
	dest->c = (char *) src;
}

static inline void smart_str_sets(smart_str *dest, const char *src)
{
	smart_str_setl(dest, src, strlen(src));
}

#endif
