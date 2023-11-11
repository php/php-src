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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include "php.h"

#include <zend_strtod.h>

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#include <locale.h>
#ifdef ZTS
#include "ext/standard/php_string.h"
#define LCONV_DECIMAL_POINT (*lconv.decimal_point)
#else
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#endif

/*
 * Copyright (c) 2002, 2006 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

static char * __cvt(double value, int ndigit, int *decpt, bool *sign, int fmode, int pad) /* {{{ */
{
	char *s = NULL;
	char *p, *rve, c;
	size_t siz;

	if (ndigit < 0) {
		siz = -ndigit + 1;
	} else {
		siz = ndigit + 1;
	}

	/* __dtoa() doesn't allocate space for 0 so we do it by hand */
	if (value == 0.0) {
		*decpt = 1 - fmode; /* 1 for 'e', 0 for 'f' */
		*sign = 0;
		if ((rve = s = (char *)malloc(ndigit?siz:2)) == NULL) {
			return(NULL);
		}
		*rve++ = '0';
		*rve = '\0';
		if (!ndigit) {
			return(s);
		}
	} else {
		p = zend_dtoa(value, fmode + 2, ndigit, decpt, sign, &rve);
		if (*decpt == 9999) {
			/* Infinity or Nan, convert to inf or nan like printf */
			*decpt = 0;
			c = *p;
			zend_freedtoa(p);
			return strdup((c == 'I' ? "INF" : "NAN"));
		}
		/* Make a local copy and adjust rve to be in terms of s */
		if (pad && fmode) {
			siz += *decpt;
		}
		if ((s = (char *)malloc(siz+1)) == NULL) {
			zend_freedtoa(p);
			return(NULL);
		}
		(void) strlcpy(s, p, siz);
		rve = s + (rve - p);
		zend_freedtoa(p);
	}

	/* Add trailing zeros */
	if (pad) {
		siz -= rve - s;
		while (--siz) {
			*rve++ = '0';
		}
		*rve = '\0';
	}

	return(s);
}
/* }}} */

static inline char *php_ecvt(double value, int ndigit, int *decpt, bool *sign) /* {{{ */
{
	return(__cvt(value, ndigit, decpt, sign, 0, 1));
}
/* }}} */

static inline char *php_fcvt(double value, int ndigit, int *decpt, bool *sign) /* {{{ */
{
	return(__cvt(value, ndigit, decpt, sign, 1, 1));
}
/* }}} */

/* {{{ Apache license */
/* ====================================================================
 * Copyright (c) 1995-1998 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 * This code is based on, and used with the permission of, the
 * SIO stdio-replacement strx_* functions by Panos Tsirigotis
 * <panos@alumni.cs.colorado.edu> for xinetd.
 */
/* }}} */

#define NUL			'\0'
#define INT_NULL		((int *)0)

#define S_NULL			"(null)"
#define S_NULL_LEN		6

#define FLOAT_DIGITS		6
#define EXPONENT_LENGTH		10


/*
 * Convert num to its decimal format.
 * Return value:
 *   - a pointer to a string containing the number (no sign)
 *   - len contains the length of the string
 *   - is_negative is set to true or false depending on the sign
 *     of the number (always set to false if is_unsigned is true)
 *
 * The caller provides a buffer for the string: that is the buf_end argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], buf_end should be &buf[ 100 ])
 */
/* char * ap_php_conv_10() {{{ */
PHPAPI char * ap_php_conv_10(int64_t num, bool is_unsigned,
	   bool * is_negative, char *buf_end, size_t *len)
{
	char *p = buf_end;
	uint64_t magnitude;

	if (is_unsigned) {
		magnitude = (uint64_t) num;
		*is_negative = false;
	} else {
		*is_negative = (num < 0);

		/*
		 * On a 2's complement machine, negating the most negative integer
		 * results in a number that cannot be represented as a signed integer.
		 * Here is what we do to obtain the number's magnitude:
		 *      a. add 1 to the number
		 *      b. negate it (becomes positive)
		 *      c. convert it to unsigned
		 *      d. add 1
		 */
		if (*is_negative) {
			int64_t t = num + 1;
			magnitude = ((uint64_t) - t) + 1;
		} else {
			magnitude = (uint64_t) num;
		}
	}

	/*
	 * We use a do-while loop so that we write at least 1 digit
	 */
	do {
		uint64_t new_magnitude = magnitude / 10;

		*--p = (char)(magnitude - new_magnitude * 10 + '0');
		magnitude = new_magnitude;
	}
	while (magnitude);

	*len = buf_end - p;
	return (p);
}
/* }}} */

/* If you change this value then also change bug24640.phpt.
 * Also NDIG must be reasonable smaller than NUM_BUF_SIZE.
 */
#define	NDIG	320


/*
 * Convert a floating point number to a string formats 'f', 'e' or 'E'.
 * The result is placed in buf, and len denotes the length of the string
 * The sign is returned in the is_negative argument (and is not placed
 * in buf).
 */
/* PHPAPI char * php_conv_fp() {{{ */
PHPAPI char * php_conv_fp(char format, double num,
		 bool add_dp, int precision, char dec_point, bool * is_negative, char *buf, size_t *len)
{
	char *s = buf;
	char *p, *p_orig;
	int decimal_point;

	if (precision >= NDIG - 1) {
		precision = NDIG - 2;
	}

	if (format == 'F') {
		p_orig = p = php_fcvt(num, precision, &decimal_point, is_negative);
	} else {						/* either e or E format */
		p_orig = p = php_ecvt(num, precision + 1, &decimal_point, is_negative);
	}

	/*
	 * Check for Infinity and NaN
	 */
	if (isalpha((int)*p)) {
		*len = strlen(p);
		memcpy(buf, p, *len + 1);
		*is_negative = false;
		free(p_orig);
		return (buf);
	}
	if (format == 'F') {
		if (decimal_point <= 0) {
			if (num != 0 || precision > 0) {
				*s++ = '0';
				if (precision > 0) {
					*s++ = dec_point;
					while (decimal_point++ < 0) {
						*s++ = '0';
					}
				} else if (add_dp) {
					*s++ = dec_point;
				}
			}
		} else {
			int addz = decimal_point >= NDIG ? decimal_point - NDIG + 1 : 0;
			decimal_point -= addz;
			while (decimal_point-- > 0) {
				*s++ = *p++;
			}
			while (addz-- > 0) {
				*s++ = '0';
			}
			if (precision > 0 || add_dp) {
				*s++ = dec_point;
			}
		}
	} else {
		*s++ = *p++;
		if (precision > 0 || add_dp) {
			*s++ = '.';
		}
	}

	/*
	 * copy the rest of p, the NUL is NOT copied
	 */
	while (*p) {
		*s++ = *p++;
	}

	if (format != 'F') {
		char temp[EXPONENT_LENGTH];		/* for exponent conversion */
		size_t t_len;
		bool exponent_is_negative;

		*s++ = format;			/* either e or E */
		decimal_point--;
		if (decimal_point != 0) {
			p = ap_php_conv_10((int64_t) decimal_point, false, &exponent_is_negative, &temp[EXPONENT_LENGTH], &t_len);
			*s++ = exponent_is_negative ? '-' : '+';

			/*
			 * Make sure the exponent has at least 2 digits
			 */
			while (t_len--) {
				*s++ = *p++;
			}
		} else {
			*s++ = '+';
			*s++ = '0';
		}
	}
	*len = s - buf;
	free(p_orig);
	return (buf);
}
/* }}} */

/*
 * Convert num to a base X number where X is a power of 2. nbits determines X.
 * For example, if nbits is 3, we do base 8 conversion
 * Return value:
 *      a pointer to a string containing the number
 *
 * The caller provides a buffer for the string: that is the buf_end argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], buf_end should be &buf[ 100 ])
 */
PHPAPI char * ap_php_conv_p2(uint64_t num, int nbits, char format, char *buf_end, size_t *len) /* {{{ */
{
	int mask = (1 << nbits) - 1;
	char *p = buf_end;
	static const char low_digits[] = "0123456789abcdef";
	static const char upper_digits[] = "0123456789ABCDEF";
	const char *digits = (format == 'X') ? upper_digits : low_digits;

	do {
		*--p = digits[num & mask];
		num >>= nbits;
	}
	while (num);

	*len = buf_end - p;
	return (p);
}
/* }}} */

/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 * Emax = 1023
 * NDIG = 320
 * NUM_BUF_SIZE >= strlen("-") + Emax + strlen(".") + NDIG + strlen("E+1023") + 1;
 */
#define NUM_BUF_SIZE		2048


/*
 * Descriptor for buffer area
 */
struct buf_area {
	char *buf_end;
	char *nextb;				/* pointer to next byte to read/write   */
};

typedef struct buf_area buffy;

/*
 * The INS_CHAR macro inserts a character in the buffer and writes
 * the buffer back to disk if necessary
 * It uses the char pointers sp and bep:
 *      sp points to the next available character in the buffer
 *      bep points to the end-of-buffer+1
 * While using this macro, note that the nextb pointer is NOT updated.
 *
 * NOTE: Evaluation of the c argument should not have any side-effects
 */
#define INS_CHAR(c, sp, bep, cc) \
	{                            \
		if (sp < bep)            \
		{                        \
			*sp++ = c;           \
		}                        \
		cc++;                    \
	}

#define NUM( c )			( c - '0' )

#define STR_TO_DEC( str, num )		\
    num = NUM( *str++ ) ;		\
    while ( isdigit((int)*str ) )		\
    {					\
	num *= 10 ;			\
	num += NUM( *str++ ) ;		\
    }

/*
 * This macro does zero padding so that the precision
 * requirement is satisfied. The padding is done by
 * adding '0's to the left of the string that is going
 * to be printed.
 */
#define FIX_PRECISION( adjust, precision, s, s_len )	\
	if ( adjust )						\
	while ( s_len < (size_t)precision )	\
	{									\
	    *--s = '0' ;					\
	    s_len++ ;						\
	}

/*
 * Macro that does padding. The padding is done by printing
 * the character ch.
 */
#define PAD( width, len, ch )	do		\
	{					\
	    INS_CHAR( ch, sp, bep, cc ) ;	\
	    width-- ;				\
	}					\
	while ( (size_t)width > len )

/*
 * Do format conversion placing the output in buffer
 */
static size_t format_converter(buffy * odp, const char *fmt, va_list ap) /* {{{ */
{
	char *sp;
	char *bep;
	size_t cc = 0;
	size_t i;

	char *s = NULL;
	size_t s_len;

	int min_width = 0;
	int precision = 0;
	enum {
		LEFT, RIGHT
	} adjust;
	char pad_char;
	char prefix_char;

	double fp_num;
	int64_t i_num = (int64_t) 0;
	uint64_t ui_num;

	char num_buf[NUM_BUF_SIZE];
	char char_buf[2];			/* for printing %% and %<unknown> */

#ifdef ZTS
	struct lconv lconv;
#else
	struct lconv *lconv = NULL;
#endif

	/*
	 * Flag variables
	 */
	length_modifier_e modifier;
	bool alternate_form;
	bool print_sign;
	bool print_blank;
	bool adjust_precision;
	bool adjust_width;
	bool is_negative;

	sp = odp->nextb;
	bep = odp->buf_end;

	while (*fmt) {
		if (*fmt != '%') {
			INS_CHAR(*fmt, sp, bep, cc);
		} else {
			/*
			 * Default variable settings
			 */
			zend_string *tmp_str = NULL;
			adjust = RIGHT;
			alternate_form = print_sign = print_blank = false;
			pad_char = ' ';
			prefix_char = NUL;

			fmt++;

			/*
			 * Try to avoid checking for flags, width or precision
			 */
			if (isascii((int)*fmt) && !islower((int)*fmt)) {
				/*
				 * Recognize flags: -, #, BLANK, +
				 */
				for (;; fmt++) {
					if (*fmt == '-')
						adjust = LEFT;
					else if (*fmt == '+')
						print_sign = true;
					else if (*fmt == '#')
						alternate_form = true;
					else if (*fmt == ' ')
						print_blank = true;
					else if (*fmt == '0')
						pad_char = '0';
					else
						break;
				}

				/*
				 * Check if a width was specified
				 */
				if (isdigit((int)*fmt)) {
					STR_TO_DEC(fmt, min_width);
					adjust_width = true;
				} else if (*fmt == '*') {
					min_width = va_arg(ap, int);
					fmt++;
					adjust_width = true;
					if (min_width < 0) {
						adjust = LEFT;
						min_width = -min_width;
					}
				} else
					adjust_width = false;

				/*
				 * Check if a precision was specified
				 */
				if (*fmt == '.') {
					adjust_precision = true;
					fmt++;
					if (isdigit((int)*fmt)) {
						STR_TO_DEC(fmt, precision);
					} else if (*fmt == '*') {
						precision = va_arg(ap, int);
						fmt++;
						if (precision < -1)
							precision = -1;
					} else
						precision = 0;
				} else
					adjust_precision = false;
			} else
				adjust_precision = adjust_width = false;

			/*
			 * Modifier check
			 */
			switch (*fmt) {
				case 'L':
					fmt++;
					modifier = LM_LONG_DOUBLE;
					break;
				case 'l':
					fmt++;
#if SIZEOF_LONG_LONG
					if (*fmt == 'l') {
						fmt++;
						modifier = LM_LONG_LONG;
					} else
#endif
						modifier = LM_LONG;
					break;
				case 'z':
					fmt++;
					modifier = LM_SIZE_T;
					break;
				case 'j':
					fmt++;
#if SIZEOF_INTMAX_T
					modifier = LM_INTMAX_T;
#else
					modifier = LM_SIZE_T;
#endif
					break;
				case 't':
					fmt++;
#if SIZEOF_PTRDIFF_T
					modifier = LM_PTRDIFF_T;
#else
					modifier = LM_SIZE_T;
#endif
					break;
				case 'p':
				{
					char __next = *(fmt+1);
					if ('d' == __next || 'u' == __next || 'x' == __next || 'o' == __next) {
						zend_error_noreturn(E_CORE_ERROR,
							"printf \"p\" modifier is no longer supported, use ZEND_LONG_FMT");
					}
					modifier = LM_STD;
					break;
				}
				case 'h':
					fmt++;
					if (*fmt == 'h') {
						fmt++;
					}
					/* these are promoted to int, so no break */
					ZEND_FALLTHROUGH;
				default:
					modifier = LM_STD;
					break;
			}

			/*
			 * Argument extraction and printing.
			 * First we determine the argument type.
			 * Then, we convert the argument to a string.
			 * On exit from the switch, s points to the string that
			 * must be printed, s_len has the length of the string
			 * The precision requirements, if any, are reflected in s_len.
			 *
			 * NOTE: pad_char may be set to '0' because of the 0 flag.
			 *   It is reset to ' ' by non-numeric formats
			 */
			switch (*fmt) {
				case 'Z': {
					zval *zvp = va_arg(ap, zval*);
					zend_string *str = zval_get_tmp_string(zvp, &tmp_str);
					s_len = ZSTR_LEN(str);
					s = ZSTR_VAL(str);
					if (adjust_precision && (size_t)precision < s_len) {
						s_len = precision;
					}
					break;
				}
				case 'u':
					switch(modifier) {
						default:
							i_num = (int64_t) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							i_num = (int64_t) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							i_num = (int64_t) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							i_num = (int64_t) va_arg(ap, unsigned long long int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							i_num = (int64_t) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							i_num = (int64_t) va_arg(ap, ptrdiff_t);
							break;
#endif
					}
					/*
					 * The rest also applies to other integer formats, so fall
					 * into that case.
					 */
					ZEND_FALLTHROUGH;
				case 'd':
				case 'i':
					/*
					 * Get the arg if we haven't already.
					 */
					if ((*fmt) != 'u') {
						switch(modifier) {
							default:
								i_num = (int64_t) va_arg(ap, int);
								break;
							case LM_LONG_DOUBLE:
								goto fmt_error;
							case LM_LONG:
								i_num = (int64_t) va_arg(ap, long int);
								break;
							case LM_SIZE_T:
#if SIZEOF_SSIZE_T
								i_num = (int64_t) va_arg(ap, ssize_t);
#else
								i_num = (int64_t) va_arg(ap, size_t);
#endif
								break;
#if SIZEOF_LONG_LONG
							case LM_LONG_LONG:
								i_num = (int64_t) va_arg(ap, long long int);
								break;
#endif
#if SIZEOF_INTMAX_T
							case LM_INTMAX_T:
								i_num = (int64_t) va_arg(ap, intmax_t);
								break;
#endif
#if SIZEOF_PTRDIFF_T
							case LM_PTRDIFF_T:
								i_num = (int64_t) va_arg(ap, ptrdiff_t);
								break;
#endif
						}
					}
					s = ap_php_conv_10(i_num, (*fmt) == 'u', &is_negative,
								&num_buf[NUM_BUF_SIZE], &s_len);
					FIX_PRECISION(adjust_precision, precision, s, s_len);

					if (*fmt != 'u') {
						if (is_negative) {
							prefix_char = '-';
						} else if (print_sign) {
							prefix_char = '+';
						} else if (print_blank) {
							prefix_char = ' ';
						}
					}
					break;


				case 'o':
					switch(modifier) {
						default:
							ui_num = (uint64_t) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							ui_num = (uint64_t) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							ui_num = (uint64_t) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							ui_num = (uint64_t) va_arg(ap, unsigned long long int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							ui_num = (uint64_t) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							ui_num = (uint64_t) va_arg(ap, ptrdiff_t);
							break;
#endif
					}
					s = ap_php_conv_p2(ui_num, 3, *fmt, &num_buf[NUM_BUF_SIZE], &s_len);
					FIX_PRECISION(adjust_precision, precision, s, s_len);
					if (alternate_form && *s != '0') {
						*--s = '0';
						s_len++;
					}
					break;


				case 'x':
				case 'X':
					switch(modifier) {
						default:
							ui_num = (uint64_t) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							ui_num = (uint64_t) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							ui_num = (uint64_t) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							ui_num = (uint64_t) va_arg(ap, unsigned long long int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							ui_num = (uint64_t) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							ui_num = (uint64_t) va_arg(ap, ptrdiff_t);
							break;
#endif
					}
					s = ap_php_conv_p2(ui_num, 4, *fmt, &num_buf[NUM_BUF_SIZE], &s_len);
					FIX_PRECISION(adjust_precision, precision, s, s_len);
					if (alternate_form && i_num != 0) {
						*--s = *fmt;	/* 'x' or 'X' */
						*--s = '0';
						s_len += 2;
					}
					break;


				case 's':
					s = va_arg(ap, char *);
					if (s != NULL) {
						s_len = strlen(s);
						if (adjust_precision && (size_t)precision < s_len) {
							s_len = precision;
						}
					} else {
						s = S_NULL;
						s_len = S_NULL_LEN;
					}
					pad_char = ' ';
					break;


				case 'f':
				case 'F':
				case 'e':
				case 'E':
					switch(modifier) {
						case LM_LONG_DOUBLE:
							fp_num = (double) va_arg(ap, long double);
							break;
						case LM_STD:
							fp_num = va_arg(ap, double);
							break;
						default:
							goto fmt_error;
					}

					if (zend_isnan(fp_num)) {
						s = "NAN";
						s_len = 3;
					} else if (zend_isinf(fp_num)) {
						s = "INF";
						s_len = 3;
					} else {
#ifdef ZTS
						localeconv_r(&lconv);
#else
						if (!lconv) {
							lconv = localeconv();
						}
#endif
						s = php_conv_fp((*fmt == 'f')?'F':*fmt, fp_num, alternate_form,
						 (adjust_precision == false) ? FLOAT_DIGITS : precision,
						 (*fmt == 'f')?LCONV_DECIMAL_POINT:'.',
									&is_negative, &num_buf[1], &s_len);
						if (is_negative)
							prefix_char = '-';
						else if (print_sign)
							prefix_char = '+';
						else if (print_blank)
							prefix_char = ' ';
					}
					break;


				case 'g':
				case 'k':
				case 'G':
				case 'H':
					switch(modifier) {
						case LM_LONG_DOUBLE:
							fp_num = (double) va_arg(ap, long double);
							break;
						case LM_STD:
							fp_num = va_arg(ap, double);
							break;
						default:
							goto fmt_error;
					}

					if (zend_isnan(fp_num)) {
						s = "NAN";
						s_len = 3;
						break;
					} else if (zend_isinf(fp_num)) {
						if (fp_num > 0) {
							s = "INF";
							s_len = 3;
						} else {
							s = "-INF";
							s_len = 4;
						}
						break;
					}

					if (adjust_precision == false) {
						precision = FLOAT_DIGITS;
					} else if (precision == 0) {
						precision = 1;
					}
					/*
					 * * We use &num_buf[ 1 ], so that we have room for the sign
					 */
#ifdef ZTS
					localeconv_r(&lconv);
#else
					if (!lconv) {
						lconv = localeconv();
					}
#endif
					s = zend_gcvt(fp_num, precision, (*fmt=='H' || *fmt == 'k') ? '.' : LCONV_DECIMAL_POINT, (*fmt == 'G' || *fmt == 'H')?'E':'e', &num_buf[1]);
					if (*s == '-') {
						prefix_char = *s++;
					} else if (print_sign) {
						prefix_char = '+';
					} else if (print_blank) {
						prefix_char = ' ';
					}

					s_len = strlen(s);

					if (alternate_form && (strchr(s, '.')) == NULL) {
						s[s_len++] = '.';
					}
					break;


				case 'c':
					char_buf[0] = (char) (va_arg(ap, int));
					s = &char_buf[0];
					s_len = 1;
					pad_char = ' ';
					break;


				case '%':
					char_buf[0] = '%';
					s = &char_buf[0];
					s_len = 1;
					pad_char = ' ';
					break;


				case 'n':
					*(va_arg(ap, int *)) = cc;
					goto skip_output;

					/*
					 * Always extract the argument as a "char *" pointer. We
					 * should be using "void *" but there are still machines
					 * that don't understand it.
					 * If the pointer size is equal to the size of an unsigned
					 * integer we convert the pointer to a hex number, otherwise
					 * we print "%p" to indicate that we don't handle "%p".
					 */
				case 'p':
					if (sizeof(char *) <= sizeof(uint64_t)) {
						ui_num = (uint64_t)((size_t) va_arg(ap, char *));
						s = ap_php_conv_p2(ui_num, 4, 'x',
								&num_buf[NUM_BUF_SIZE], &s_len);
						if (ui_num != 0) {
							*--s = 'x';
							*--s = '0';
							s_len += 2;
						}
					} else {
						s = "%p";
						s_len = 2;
					}
					pad_char = ' ';
					break;


				case NUL:
					/*
					 * The last character of the format string was %.
					 * We ignore it.
					 */
					continue;


fmt_error:
				php_error(E_ERROR, "Illegal length modifier specified '%c' in s[np]printf call", *fmt);
					/*
					 * The default case is for unrecognized %'s.
					 * We print %<char> to help the user identify what
					 * option is not understood.
					 * This is also useful in case the user wants to pass
					 * the output of format_converter to another function
					 * that understands some other %<char> (like syslog).
					 * Note that we can't point s inside fmt because the
					 * unknown <char> could be preceded by width etc.
					 */
					ZEND_FALLTHROUGH;
				default:
					char_buf[0] = '%';
					char_buf[1] = *fmt;
					s = char_buf;
					s_len = 2;
					pad_char = ' ';
					break;
			}

			if (prefix_char != NUL) {
				*--s = prefix_char;
				s_len++;
			}
			if (adjust_width && adjust == RIGHT && (size_t)min_width > s_len) {
				if (pad_char == '0' && prefix_char != NUL) {
					INS_CHAR(*s, sp, bep, cc)
						s++;
					s_len--;
					min_width--;
				}
				PAD(min_width, s_len, pad_char);
			}
			/*
			 * Print the string s.
			 */
			for (i = s_len; i != 0; i--) {
				INS_CHAR(*s, sp, bep, cc);
				s++;
			}

			if (adjust_width && adjust == LEFT && (size_t)min_width > s_len)
				PAD(min_width, s_len, pad_char);
			zend_tmp_string_release(tmp_str);
		}
skip_output:
		fmt++;
	}
	odp->nextb = sp;
	return (cc);
}
/* }}} */

/*
 * This is the general purpose conversion function.
 */
static size_t strx_printv(char *buf, size_t len, const char *format, va_list ap) /* {{{ */
{
	buffy od;
	size_t cc;

	/*
	 * First initialize the descriptor
	 * Notice that if no length is given, we initialize buf_end to the
	 * highest possible address.
	 */
	if (len == 0) {
		od.buf_end = (char *) ~0;
		od.nextb   = (char *) ~0;
	} else {
		od.buf_end = &buf[len-1];
		od.nextb   = buf;
	}

	/*
	 * Do the conversion
	 */
	cc = format_converter(&od, format, ap);
	if (len != 0 && od.nextb <= od.buf_end) {
		*(od.nextb) = '\0';
	}
	return cc;
}
/* }}} */

PHPAPI int ap_php_slprintf(char *buf, size_t len, const char *format,...) /* {{{ */
{
	size_t cc;
	va_list ap;

	va_start(ap, format);
	cc = strx_printv(buf, len, format, ap);
	va_end(ap);
	if (cc >= len) {
		cc = len -1;
		buf[cc] = '\0';
	}
	return (int) cc;
}
/* }}} */

PHPAPI int ap_php_vslprintf(char *buf, size_t len, const char *format, va_list ap) /* {{{ */
{
	size_t cc = strx_printv(buf, len, format, ap);
	if (cc >= len) {
		cc = len -1;
		buf[cc] = '\0';
	}
	return (int) cc;
}
/* }}} */

PHPAPI int ap_php_snprintf(char *buf, size_t len, const char *format,...) /* {{{ */
{
	size_t cc;
	va_list ap;

	va_start(ap, format);
	cc = strx_printv(buf, len, format, ap);
	va_end(ap);
	return (int) cc;
}
/* }}} */

PHPAPI int ap_php_vsnprintf(char *buf, size_t len, const char *format, va_list ap) /* {{{ */
{
	size_t cc = strx_printv(buf, len, format, ap);
	return (int) cc;
}
/* }}} */

PHPAPI int ap_php_vasprintf(char **buf, const char *format, va_list ap) /* {{{ */
{
	va_list ap2;
	int cc;

	va_copy(ap2, ap);
	cc = ap_php_vsnprintf(NULL, 0, format, ap2);
	va_end(ap2);

	*buf = NULL;

	if (cc >= 0) {
		if ((*buf = malloc(++cc)) != NULL) {
			if ((cc = ap_php_vsnprintf(*buf, cc, format, ap)) < 0) {
				free(*buf);
				*buf = NULL;
			}
		}
	}

	return cc;
}
/* }}} */

PHPAPI int ap_php_asprintf(char **buf, const char *format, ...) /* {{{ */
{
	int cc;
	va_list ap;

	va_start(ap, format);
	cc = vasprintf(buf, format, ap);
	va_end(ap);
	return cc;
}
/* }}} */
