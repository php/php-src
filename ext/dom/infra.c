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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "infra.h"

/* https://infra.spec.whatwg.org/#ascii-whitespace */
const char *ascii_whitespace = "\x09\x0A\x0C\x0D\x20";

/* https://infra.spec.whatwg.org/#strip-and-collapse-ascii-whitespace */
zend_string *dom_strip_and_collapse_ascii_whitespace(zend_string *input)
{
	if (input == zend_empty_string) {
		return input;
	}

	ZEND_ASSERT(!ZSTR_IS_INTERNED(input));
	ZEND_ASSERT(GC_REFCOUNT(input) == 1);

	char *write_ptr = ZSTR_VAL(input);

	const char *start = ZSTR_VAL(input);
	const char *current = start;
	const char *end = current + ZSTR_LEN(input);

	current += strspn(current, ascii_whitespace);

	while (current < end) {
		/* Copy non-whitespace */
		size_t non_whitespace_len = strcspn(current, ascii_whitespace);
		/* If the pointers are equal, we still haven't encountered collapsible or strippable whitespace. */
		if (write_ptr != current) {
			memmove(write_ptr, current, non_whitespace_len);
		}
		current += non_whitespace_len;
		write_ptr += non_whitespace_len;

		/* Skip whitespace */
		current += strspn(current, ascii_whitespace);
		if (current < end) {
			/* Only make a space when we're not yet at the end of the input, because that means more non-whitespace
			 * input is to come. */
			*write_ptr++ = ' ';
		}
	}

	*write_ptr = '\0';

	size_t len = write_ptr - start;
	if (len != ZSTR_LEN(input)) {
		return zend_string_truncate(input, len, false);
	} else {
		/* Forget the hash value since we may have transformed non-space-whitespace into spaces. */
		zend_string_forget_hash_val(input);
		return input;
	}
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
