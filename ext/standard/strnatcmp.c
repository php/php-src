/* -*- mode: c; c-file-style: "k&r" -*-

  Modified for PHP by Andrei Zmievski <andrei@ispi.net>

  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000 by Martin Pool <mbp@humbug.org.au>

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
#endif

static char const *version UNUSED =
    "$Id$";

PHPAPI int strnatcmp_ex(char const *a, size_t a_len, char const *b, size_t b_len, int fold_case)
{
	int ai, bi;
	char ca, cb;
	const char *aend = a + a_len,
			   *bend = b + b_len;

	if (a_len == 0 || b_len == 0)
		return a_len - b_len;

	assert(a && b);
	ai = bi = 0;
	while (1) {
		ca = a[ai]; cb = b[bi];

		/* skip over leading spaces or zeros */
		while (isspace(ca)  ||  ca == '0')
			ca = a[++ai];

		while (isspace(cb)  ||  cb == '0')
			cb = b[++bi];

		/* process run of digits */
		if (isdigit(ca)  &&  isdigit(cb)) {
			int bias = 0;
			/* The longest run of digits (stripping off leading
			   zeros) wins.  That aside, the greatest value wins,
			   but we can't know that it will until we've scanned
			   both numbers to know that they have the same
			   magnitude, so we remember it in BIAS. */
			while (1) {
				if (!isdigit(ca)  &&  !isdigit(cb))
					goto done_number;
				else if (!isdigit(ca))
					return -1;
				else if (!isdigit(cb))
					return +1;
				else if (ca < cb) {
					if (!bias)
						bias = -1;
				} else if (ca > cb) {
					if (!bias)
						bias = +1;
				}

				++ai; ++bi;
				if (a + ai == aend && b + bi == bend)
					/* Return the current bias if at the end of both strings. */
					return bias;
				else if (a + ai == aend)
					return -1;
				else if (b + bi == bend)
					return 1;

				ca = a[ai]; cb = b[bi];
			}
done_number:
			if (bias)
				return bias;
		}

		if (fold_case) {
			ca = toupper(ca);
			cb = toupper(cb);
		}

		if (ca < cb)
			return -1;
		else if (ca > cb)
			return +1;

		++ai; ++bi;
		if (a + ai == aend && b + bi == bend)
			/* The strings compare the same.  Perhaps the caller
			   will want to call strcmp to break the tie. */
			return 0;
		else if (a + ai == aend)
			return -1;
		else if (b + bi == bend)
			return 1;
	}
}
