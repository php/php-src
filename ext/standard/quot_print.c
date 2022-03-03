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
   | Author: Kirill Maximov <kir@actimind.com>                            |
   +----------------------------------------------------------------------+
 */

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include "php.h"
#include "quot_print.h"

#include <stdio.h>

/*
*  Converting HEX char to INT value
*/
static char php_hex2int(int c) /* {{{ */
{
	if (isdigit(c)) {
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	else {
		return -1;
	}
}
/* }}} */

PHPAPI zend_string *php_quot_print_decode(const unsigned char *str, size_t length, int replace_us_by_ws) /* {{{ */
{
	size_t i;
	unsigned const char *p1;
	unsigned char *p2;
	unsigned int h_nbl, l_nbl;

	size_t decoded_len, buf_size;
	zend_string *retval;

	static unsigned int hexval_tbl[256] = {
		64, 64, 64, 64, 64, 64, 64, 64, 64, 32, 16, 64, 64, 16, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 64, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};

	if (replace_us_by_ws) {
		replace_us_by_ws = '_';
	}

	i = length, p1 = str; buf_size = length;

	while (i > 1 && *p1 != '\0') {
		if (*p1 == '=') {
			buf_size -= 2;
			p1++;
			i--;
		}
		p1++;
		i--;
	}

	retval = zend_string_alloc(buf_size, 0);
	i = length; p1 = str; p2 = (unsigned char*)ZSTR_VAL(retval);
	decoded_len = 0;

	while (i > 0 && *p1 != '\0') {
		if (*p1 == '=') {
			i--, p1++;
			if (i == 0 || *p1 == '\0') {
				break;
			}
			h_nbl = hexval_tbl[*p1];
			if (h_nbl < 16) {
				/* next char should be a hexadecimal digit */
				if ((--i) == 0 || (l_nbl = hexval_tbl[*(++p1)]) >= 16) {
					efree(retval);
					return NULL;
				}
				*(p2++) = (h_nbl << 4) | l_nbl, decoded_len++;
				i--, p1++;
			} else if (h_nbl < 64) {
				/* soft line break */
				while (h_nbl == 32) {
					if (--i == 0 || (h_nbl = hexval_tbl[*(++p1)]) == 64) {
						efree(retval);
						return NULL;
					}
				}
				if (p1[0] == '\r' && i >= 2 && p1[1] == '\n') {
					i--, p1++;
				}
				i--, p1++;
			} else {
				efree(retval);
				return NULL;
			}
		} else {
			*(p2++) = (replace_us_by_ws == *p1 ? '\x20': *p1);
			i--, p1++, decoded_len++;
		}
	}

	*p2 = '\0';
	ZSTR_LEN(retval) = decoded_len;
	return retval;
}
/* }}} */

#define PHP_QPRINT_MAXL 75

PHPAPI zend_string *php_quot_print_encode(const unsigned char *str, size_t length) /* {{{ */
{
	zend_ulong lp = 0;
	unsigned char c, *d;
	char *hex = "0123456789ABCDEF";
	zend_string *ret;

	ret = zend_string_safe_alloc(3, (length + (((3 * length)/(PHP_QPRINT_MAXL-9)) + 1)), 0, 0);
	d = (unsigned char*)ZSTR_VAL(ret);

	while (length--) {
		if (((c = *str++) == '\015') && (*str == '\012') && length > 0) {
			*d++ = '\015';
			*d++ = *str++;
			length--;
			lp = 0;
		} else {
			if (iscntrl (c) || (c == 0x7f) || (c & 0x80) || (c == '=') || ((c == ' ') && (*str == '\015'))) {
				if ((((lp+= 3) > PHP_QPRINT_MAXL) && (c <= 0x7f))
				    || ((c > 0x7f) && (c <= 0xdf) && ((lp + 3) > PHP_QPRINT_MAXL))
				    || ((c > 0xdf) && (c <= 0xef) && ((lp + 6) > PHP_QPRINT_MAXL))
				    || ((c > 0xef) && (c <= 0xf4) && ((lp + 9) > PHP_QPRINT_MAXL))) {
					*d++ = '=';
					*d++ = '\015';
					*d++ = '\012';
					lp = 3;
				}
				*d++ = '=';
				*d++ = hex[c >> 4];
				*d++ = hex[c & 0xf];
			} else {
				if ((++lp) > PHP_QPRINT_MAXL) {
					*d++ = '=';
					*d++ = '\015';
					*d++ = '\012';
					lp = 1;
				}
				*d++ = c;
			}
		}
	}
	*d = '\0';
	ret = zend_string_truncate(ret, d - (unsigned char*)ZSTR_VAL(ret), 0);
	return ret;
}
/* }}} */

/*
*
* Decoding  Quoted-printable string.
*
*/
/* {{{ Convert a quoted-printable string to an 8 bit string */
PHP_FUNCTION(quoted_printable_decode)
{
	zend_string *arg1;
	char *str_in;
	zend_string *str_out;
	size_t i = 0, j = 0, k;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg1)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(arg1) == 0) {
		/* shortcut */
		RETURN_EMPTY_STRING();
	}

	str_in = ZSTR_VAL(arg1);
	str_out = zend_string_alloc(ZSTR_LEN(arg1), 0);
	while (str_in[i]) {
		switch (str_in[i]) {
		case '=':
			if (str_in[i + 1] && str_in[i + 2] &&
				isxdigit((int) str_in[i + 1]) &&
				isxdigit((int) str_in[i + 2]))
			{
				ZSTR_VAL(str_out)[j++] = (php_hex2int((int) str_in[i + 1]) << 4)
						+ php_hex2int((int) str_in[i + 2]);
				i += 3;
			} else  /* check for soft line break according to RFC 2045*/ {
				k = 1;
				while (str_in[i + k] && ((str_in[i + k] == 32) || (str_in[i + k] == 9))) {
					/* Possibly, skip spaces/tabs at the end of line */
					k++;
				}
				if (!str_in[i + k]) {
					/* End of line reached */
					i += k;
				}
				else if ((str_in[i + k] == 13) && (str_in[i + k + 1] == 10)) {
					/* CRLF */
					i += k + 2;
				}
				else if ((str_in[i + k] == 13) || (str_in[i + k] == 10)) {
					/* CR or LF */
					i += k + 1;
				}
				else {
					ZSTR_VAL(str_out)[j++] = str_in[i++];
				}
			}
			break;
		default:
			ZSTR_VAL(str_out)[j++] = str_in[i++];
		}
	}
	ZSTR_VAL(str_out)[j] = '\0';
	ZSTR_LEN(str_out) = j;

	RETVAL_NEW_STR(str_out);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(quoted_printable_encode)
{
	zend_string *str;
	zend_string *new_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	new_str = php_quot_print_encode((unsigned char *)ZSTR_VAL(str), ZSTR_LEN(str));
	RETURN_STR(new_str);
}
/* }}} */
