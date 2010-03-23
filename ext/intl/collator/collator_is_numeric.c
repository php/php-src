/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#include "collator_is_numeric.h"

#if ZEND_MODULE_API_NO < 20071006
/* not 5.3 */
#ifndef ALLOCA_FLAG
#define ALLOCA_FLAG(use_heap)
#endif
#define _do_alloca(x, y) do_alloca((x))
#define _free_alloca(x, y) free_alloca((x))
#else
#define _do_alloca do_alloca
#define _free_alloca free_alloca
#endif
/* {{{ collator_u_strtod
 * Taken from PHP6:zend_u_strtod()
 */
static double collator_u_strtod(const UChar *nptr, UChar **endptr) /* {{{ */
{
	const UChar *u = nptr, *nstart;
	UChar c = *u;
	int any = 0;
	ALLOCA_FLAG(use_heap);

	while (u_isspace(c)) {
		c = *++u;
	}
	nstart = u;

	if (c == 0x2D /*'-'*/ || c == 0x2B /*'+'*/) {
		c = *++u;
	}

	while (c >= 0x30 /*'0'*/ && c <= 0x39 /*'9'*/) {
		any = 1;
		c = *++u;
	}

	if (c == 0x2E /*'.'*/) {
		c = *++u;
		while (c >= 0x30 /*'0'*/ && c <= 0x39 /*'9'*/) {
			any = 1;
			c = *++u;
		}
	}

	if ((c == 0x65 /*'e'*/ || c == 0x45 /*'E'*/) && any) {
		const UChar *e = u;
		int any_exp = 0;

		c = *++u;
		if (c == 0x2D /*'-'*/ || c == 0x2B /*'+'*/) {
			c = *++u;
		}

		while (c >= 0x30 /*'0'*/ && c <= 0x39 /*'9'*/) {
			any_exp = 1;
			c = *++u;
		}

		if (!any_exp) {
			u = e;
		}
	}

	if (any) {
		char buf[64], *numbuf, *bufpos;
		int length = u - nstart;
		double value;

		if (length < sizeof(buf)) {
			numbuf = buf;
		} else {
			numbuf = (char *) _do_alloca(length + 1, use_heap);
		}

		bufpos = numbuf;

		while (nstart < u) {
			*bufpos++ = (char) *nstart++;
		}

		*bufpos = '\0';
		value = zend_strtod(numbuf, NULL);

		if (numbuf != buf) {
			_free_alloca(numbuf, use_heap);
		}

		if (endptr != NULL) {
			*endptr = (UChar *)u;
		}

		return value;
	}

	if (endptr != NULL) {
		*endptr = (UChar *)nptr;
	}

	return 0;
}
/* }}} */

/* {{{ collator_u_strtol
 * Taken from PHP6:zend_u_strtol()
 *
 * Convert a Unicode string to a long integer.
 *
 * Ignores `locale' stuff.
 */
static long collator_u_strtol(nptr, endptr, base)
	const UChar *nptr;
	UChar **endptr;
	register int base;
{
	register const UChar *s = nptr;
	register unsigned long acc;
	register UChar c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	if (s == NULL) {
		errno = ERANGE;
		if (endptr != NULL) {
			*endptr = NULL;
		}
		return 0;
	}

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (u_isspace(c));
	if (c == 0x2D /*'-'*/) {
		neg = 1;
		c = *s++;
	} else if (c == 0x2B /*'+'*/)
		c = *s++;
	if ((base == 0 || base == 16) &&
	    (c == 0x30 /*'0'*/)
		 && (*s == 0x78 /*'x'*/ || *s == 0x58 /*'X'*/)) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = (c == 0x30 /*'0'*/) ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (c >= 0x30 /*'0'*/ && c <= 0x39 /*'9'*/)
			c -= 0x30 /*'0'*/;
		else if (c >= 0x41 /*'A'*/ && c <= 0x5A /*'Z'*/)
			c -= 0x41 /*'A'*/ - 10;
		else if (c >= 0x61 /*'a'*/ && c <= 0x7A /*'z'*/)
			c -= 0x61 /*'a'*/ - 10;
		else
			break;
		if (c >= base)
			break;

		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != NULL)
		*endptr = (UChar *)(any ? s - 1 : nptr);
	return (acc);
}
/* }}} */


/* {{{ collator_is_numeric]
 * Taken from PHP6:is_numeric_unicode()
 */
zend_uchar collator_is_numeric( UChar *str, int length, long *lval, double *dval, int allow_errors )
{
	long local_lval;
	double local_dval;
	UChar *end_ptr_long, *end_ptr_double;
	int conv_base=10;

	if (!length) {
		return 0;
	}

	/* handle hex numbers */
	if (length>=2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')) {
		conv_base=16;
	}

	errno=0;
	local_lval = collator_u_strtol(str, &end_ptr_long, conv_base);
	if (errno != ERANGE) {
		if (end_ptr_long == str+length) { /* integer string */
			if (lval) {
				*lval = local_lval;
			}
			return IS_LONG;
		} else if (end_ptr_long == str && *end_ptr_long != '\0' && *str != '.' && *str != '-') { /* ignore partial string matches */
			return 0;
		}
	} else {
		end_ptr_long = NULL;
	}

	if (conv_base == 16) { /* hex string, under UNIX strtod() messes it up */
		/* UTODO: keep compatibility with is_numeric_string() here? */
		return 0;
	}

	local_dval = collator_u_strtod(str, &end_ptr_double);
	if (local_dval == 0 && end_ptr_double == str) {
		end_ptr_double = NULL;
	} else {
		if (end_ptr_double == str+length) { /* floating point string */
			if (!zend_finite(local_dval)) {
				/* "inf","nan" and maybe other weird ones */
				return 0;
			}

			if (dval) {
				*dval = local_dval;
			}
			return IS_DOUBLE;
		}
	}

	if (!allow_errors) {
		return 0;
	}
	if (allow_errors == -1) {
		zend_error(E_NOTICE, "A non well formed numeric value encountered");
	}

	if (allow_errors) {
		if (end_ptr_double > end_ptr_long && dval) {
			*dval = local_dval;
			return IS_DOUBLE;
		} else if (end_ptr_long && lval) {
			*lval = local_lval;
			return IS_LONG;
		}
	}
	return 0;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
