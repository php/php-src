#if _MSC_VERS <= 1300
#include "php_strtoi64.h"
/*
	From APR, apr_strings.c
	See http://www.apache.org/licenses/LICENSE-2.0
*/

PHPAPI int64_t _strtoi64(const char *nptr, char **endptr, int base)
{
	const char *s;
	int64_t acc;
	int64_t val;
	int neg, any;
	char c;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	s = nptr;
	do {
		c = *s++;
	} while (isspace((unsigned char)c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+') {
			c = *s++;
		}
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}
	acc = any = 0;
	if (base < 2 || base > 36) {
		errno = EINVAL;
		if (endptr != NULL) {
			*endptr = (char *)(any ? s - 1 : nptr);
		}
		return acc;
	}

	/* The classic bsd implementation requires div/mod operators
	 * to compute a cutoff.  Benchmarking proves that iss very, very
	 * evil to some 32 bit processors.  Instead, look for underflow
	 * in both the mult and add/sub operation.  Unlike the bsd impl,
	 * we also work strictly in a signed int64 word as we haven't
	 * implemented the unsigned type in win32.
	 *
	 * Set 'any' if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
    val = 0;
	for ( ; ; c = *s++) {
		if (c >= '0' && c <= '9')
			c -= '0';

#if (('Z' - 'A') == 25)
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
#elif APR_CHARSET_EBCDIC
		else if (c >= 'A' && c <= 'I')
			c -= 'A' - 10;
		else if (c >= 'J' && c <= 'R')
			c -= 'J' - 19;
		else if (c >= 'S' && c <= 'Z')
			c -= 'S' - 28;
		else if (c >= 'a' && c <= 'i')
			c -= 'a' - 10;
		else if (c >= 'j' && c <= 'r')
			c -= 'j' - 19;
		else if (c >= 's' && c <= 'z')
			c -= 'z' - 28;
#else
# error "CANNOT COMPILE apr_strtoi64(), only ASCII and EBCDIC supported"
#endif
		else {
			break;
		}

		if (c >= base) {
			break;
		}

		val *= base;
		if ( (any < 0)	/* already noted an over/under flow - short circuit */
				|| (neg && (val > acc || (val -= c) > acc)) /* underflow */
				|| (val < acc || (val += c) < acc)) {       /* overflow */
			any = -1;	/* once noted, over/underflows never go away */
#ifdef APR_STRTOI64_OVERFLOW_IS_BAD_CHAR
			break;
#endif
		} else {
			acc = val;
			any = 1;
		}
	}

	if (any < 0) {
		acc = neg ? INT64_MIN : INT64_MAX;
		errno = ERANGE;
	} else if (!any) {
		errno = EINVAL;
	}

	if (endptr != NULL) {
		*endptr = (char *)(any ? s - 1 : nptr);
	}
	return (acc);
}
#endif
