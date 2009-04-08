/* -*- mode: c; c-file-style: "k&r" -*-

  Modified for PHP by Andrei Zmievski <andrei@php.net>

  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "php.h"
#include "php_string.h"

#if defined(__GNUC__)
#  define UNUSED __attribute__((__unused__))
#else
#  define UNUSED
#endif

#if 0
static char const *version UNUSED =
    "$Id$";
#endif
/* {{{ compare_right
 */
static int
compare_right(char const *a, char const *b)
{
	int bias = 0;

	/* The longest run of digits wins.  That aside, the greatest
	   value wins, but we can't know that it will until we've scanned
	   both numbers to know that they have the same magnitude, so we
	   remember it in BIAS. */
	for(;; a++, b++) {
		if (!isdigit((int)(unsigned char)*a) &&
			!isdigit((int)(unsigned char)*b))
			return bias;
		else if (!isdigit((int)(unsigned char)*a))
			return -1;
		else if (!isdigit((int)(unsigned char)*b))
			return +1;
		else if (*a < *b) {
			if (!bias)
				bias = -1;
		} else if (*a > *b) {
			if (!bias)
				bias = +1;
		} else if (!*a && !*b) {
			return bias;
		}
     }

     return 0;
}
/* }}} */

/* {{{ compare_left
 */
static int
compare_left(char const *a, char const *b)
{
     /* Compare two left-aligned numbers: the first to have a
        different value wins. */
	for(;; a++, b++) {
		if (!isdigit((int)(unsigned char)*a) &&
			!isdigit((int)(unsigned char)*b))
			return 0;
		else if (!isdigit((int)(unsigned char)*a))
			return -1;
		else if (!isdigit((int)(unsigned char)*b))
			return +1;
		 else if (*a < *b)
			 return -1;
		 else if (*a > *b)
			 return +1;
     }
	  
     return 0;
}
/* }}} */

/* {{{ strnatcmp_ex
 */
PHPAPI int strnatcmp_ex(char const *a, size_t a_len, char const *b, size_t b_len, int fold_case)
{
	char ca, cb;
	int ai, bi;
	int fractional, result;

	ai = bi = 0;
	while (1) {
		ca = a[ai]; cb = b[bi];

		/* skip over leading spaces or zeros */
		while (isspace((int)(unsigned char)ca) || (ca == '0' && ap+1 < aend))
			ca = a[++ai];

		while (isspace((int)(unsigned char)cb) || (cb == '0' && bp+1 < bend))
			cb = b[++bi];

		/* process run of digits */
		if (isdigit((int)(unsigned char)ca)  &&  isdigit((int)(unsigned char)cb)) {
			fractional = (ca == '0' || cb == '0');

			if (fractional) {
				if ((result = compare_left(a+ai, b+bi)) != 0) {
					return result;
				}
			} else {
				if ((result = compare_right(a+ai, b+bi)) != 0)
					return result;
			}
		}

		if (!ca && !cb) {
			/* The strings compare the same.  Perhaps the caller
			   will want to call strcmp to break the tie. */
			return 0;
		}

		if (fold_case) {
			ca = toupper((int)(unsigned char)ca);
			cb = toupper((int)(unsigned char)cb);
		}

		if (ca < cb)
			return -1;
		else if (ca > cb)
			return +1;

		++ai; ++bi;
	}
}
/* }}} */

/* {{{ u_compare_right
 */
static int
u_compare_right(UChar const *a, int a_len, int *a_curr, UChar const *b, int b_len, int *b_curr)
{
	UChar32 ca, cb;
	int a_off, b_off;
	int bias = 0;

	/* The longest run of digits wins.  That aside, the greatest
	   value wins, but we can't know that it will until we've scanned
	   both numbers to know that they have the same magnitude, so we
	   remember it in BIAS. */

	for (;;) {
		a_off = *a_curr;
		b_off = *b_curr;
		U16_NEXT(a, a_off, a_len, ca);
		U16_NEXT(b, b_off, b_len, cb);

		if (!u_isdigit(ca) && !u_isdigit(cb)) {
			return bias;
		} else if (!u_isdigit(ca)) {
			return -1;
		} else if (!u_isdigit(cb)) {
			return +1;
		} else if (ca < cb) {
			if (!bias)
				bias = -1;
		} else if (ca > cb) {
			if (!bias)
				bias = +1;
		} else if (ca == 0 && cb == 0) {
			return bias;
		}
		*a_curr = a_off;
		*b_curr = b_off;
	}

	return 0;
}
/* }}} */

/* {{{ u_compare_left
 */
static int
u_compare_left(UChar const *a, int a_len, int *a_curr, UChar const *b, int b_len, int *b_curr)
{
	int a_off, b_off;
	UChar32 ca, cb;

     /* Compare two left-aligned numbers: the first to have a
        different value wins. */
	for (;;) {
		a_off = *a_curr;
		b_off = *b_curr;
		U16_NEXT(a, a_off, a_len, ca);
		U16_NEXT(b, b_off, b_len, cb);

		if (!u_isdigit(ca) && !u_isdigit(cb)) {
			return 0;
		} else if (!u_isdigit(ca)) {
			return -1;
		} else if (!u_isdigit(cb)) {
			return +1;
		} else if (ca < cb) {
			return -1;
		} else if (ca > cb) {
			return +1;
		}
		*a_curr = a_off;
		*b_curr = b_off;
	}

	return 0;
}
/* }}} */

/* {{{ u_strnatcmp_ex
 */
PHPAPI int u_strnatcmp_ex(UChar const *a, size_t a_len, UChar const *b, size_t b_len, int fold_case)
{
	UChar ca, cb;
	UChar const *ap, *bp;
	int fractional, result;
	int a_off, b_off;
	int a_curr, b_curr;

	if (a_len == 0 || b_len == 0)
		return a_len - b_len;

	ap = a;
	bp = b;
	a_curr = b_curr = 0;

	while (1) {
		a_off = a_curr;
		b_off = b_curr;
		U16_NEXT(a, a_curr, a_len, ca);
		U16_NEXT(b, b_curr, b_len, cb);

		/* skip over leading spaces */
		for ( ; u_isspace(ca) && a_curr < a_len; ) {
			a_off = a_curr;
			U16_NEXT(a, a_curr, a_len, ca);
		}

		for ( ; u_isspace(cb) && b_curr < b_len; ) {
			b_off = b_curr;
			U16_NEXT(b, b_curr, b_len, cb);
		}

		/* process run of digits */
		if (u_isdigit(ca) && u_isdigit(cb)) {
			fractional = (ca == 0x30 /*'0'*/ || cb == 0x30 /*'0'*/);

			if (fractional) {
				if ((result = u_compare_left(a, a_len, &a_off, b, b_len, &b_off)) != 0) {
					return result;
				}
			} else {
				if ((result = u_compare_right(a, a_len, &a_off, b, b_len, &b_off)) != 0) {
					return result;
				}
			}

			a_curr = a_off;
			b_curr = b_off;
		}

		if (ca == 0 && cb == 0) {
			/* The strings compare the same.  Perhaps the caller
			   will want to call strcmp to break the tie. */
			return 0;
		}

		if (fold_case) {
			ca = u_toupper(ca);
			cb = u_toupper(cb);
		}

		if (ca < cb)
			return -1;
		else if (ca > cb)
			return +1;
	}
}
/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
