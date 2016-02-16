/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* This is the spprintf implementation.
 * It has emerged from apache snprintf. See original header:
 */

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
#define _GNU_SOURCE
#include "php.h"

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#ifdef ZTS
#include "ext/standard/php_string.h"
#define LCONV_DECIMAL_POINT (*lconv.decimal_point)
#else
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#endif
#else
#define LCONV_DECIMAL_POINT '.'
#endif

#include "snprintf.h"

#define FALSE           0
#define TRUE            1
#define NUL             '\0'
#define INT_NULL        ((int *)0)

#define S_NULL          "(null)"
#define S_NULL_LEN      6

#define FLOAT_DIGITS    6
#define EXPONENT_LENGTH 10

#include "zend_smart_str.h"
#include "ext/standard/php_smart_string.h"

/* {{{ macros */

#define INS_CHAR(xbuf, ch, is_char) do { \
	if ((is_char)) { \
		smart_string_appendc((smart_string *)(xbuf), (ch)); \
	} else { \
		smart_str_appendc((smart_str *)(xbuf), (ch)); \
	} \
} while (0);

#define INS_STRING(xbuf, str, len, is_char) do { \
	if ((is_char)) { \
		smart_string_appendl((smart_string *)(xbuf), (str), (len)); \
	} else { \
		smart_str_appendl((smart_str *)(xbuf), (str), (len)); \
	} \
} while (0);

#define PAD_CHAR(xbuf, ch, count, is_char) do { \
	size_t newlen; \
	if ((is_char)) { \
		smart_string_alloc(((smart_string *)(xbuf)), (count), 0); \
		memset(((smart_string *)(xbuf))->c + ((smart_string *)(xbuf))->len, (ch), (count)); \
		((smart_string *)(xbuf))->len += (count); \
	} else { \
		smart_str_alloc(((smart_str *)(xbuf)), (count), 0); \
		memset(ZSTR_VAL(((smart_str *)(xbuf))->s) + ZSTR_LEN(((smart_str *)(xbuf))->s), (ch), (count)); \
		ZSTR_LEN(((smart_str *)(xbuf))->s) += (count); \
	} \
} while (0);

/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 * Emax = 1023
 * NDIG = 320
 * NUM_BUF_SIZE >= strlen("-") + Emax + strlrn(".") + NDIG + strlen("E+1023") + 1;
 */
#define NUM_BUF_SIZE		2048

#define NUM(c) (c - '0')

#define STR_TO_DEC(str, num) do {			\
	num = NUM(*str++);                  	\
	while (isdigit((int)*str)) {        	\
		num *= 10;                      	\
		num += NUM(*str++);             	\
		if (num >= INT_MAX / 10) {			\
			while (isdigit((int)*str++));	\
			break;							\
		}									\
    }										\
} while (0)

/*
 * This macro does zero padding so that the precision
 * requirement is satisfied. The padding is done by
 * adding '0's to the left of the string that is going
 * to be printed.
 */
#define FIX_PRECISION(adjust, precision, s, s_len) do {	\
    if (adjust)					                    	\
		while (s_len < precision) {                 	\
			*--s = '0';                             	\
			s_len++;                                	\
		}												\
} while (0)

/* }}} */

#if !HAVE_STRNLEN
static size_t strnlen(const char *s, size_t maxlen) {
	char *r = memchr(s, '\0', maxlen);
	return r ? r-s : maxlen;
}
#endif

/*
 * Do format conversion placing the output in buffer
 */
static void xbuf_format_converter(void *xbuf, zend_bool is_char, const char *fmt, va_list ap) /* {{{ */
{
	char *s = NULL;
	size_t s_len;
	int free_zcopy;
	zval *zvp, zcopy;

	int min_width = 0;
	int precision = 0;
	enum {
		LEFT, RIGHT
	} adjust;
	char pad_char;
	char prefix_char;

	double fp_num;
	wide_int i_num = (wide_int) 0;
	u_wide_int ui_num = (u_wide_int) 0;

	char num_buf[NUM_BUF_SIZE];
	char char_buf[2];			/* for printing %% and %<unknown> */

#ifdef HAVE_LOCALE_H
#ifdef ZTS
	struct lconv lconv;
#else
	struct lconv *lconv = NULL;
#endif
#endif

	/*
	 * Flag variables
	 */
	length_modifier_e modifier;
	boolean_e alternate_form;
	boolean_e print_sign;
	boolean_e print_blank;
	boolean_e adjust_precision;
	boolean_e adjust_width;
	bool_int is_negative;

	while (*fmt) {
		if (*fmt != '%') {
			INS_CHAR(xbuf, *fmt, is_char);
		} else {
			/*
			 * Default variable settings
			 */
			adjust = RIGHT;
			alternate_form = print_sign = print_blank = NO;
			pad_char = ' ';
			prefix_char = NUL;
			free_zcopy = 0;

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
						print_sign = YES;
					else if (*fmt == '#')
						alternate_form = YES;
					else if (*fmt == ' ')
						print_blank = YES;
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
					adjust_width = YES;
				} else if (*fmt == '*') {
					min_width = va_arg(ap, int);
					fmt++;
					adjust_width = YES;
					if (min_width < 0) {
						adjust = LEFT;
						min_width = -min_width;
					}
				} else
					adjust_width = NO;

				/*
				 * Check if a precision was specified
				 */
				if (*fmt == '.') {
					adjust_precision = YES;
					fmt++;
					if (isdigit((int)*fmt)) {
						STR_TO_DEC(fmt, precision);
					} else if (*fmt == '*') {
						precision = va_arg(ap, int);
						fmt++;
						if (precision < 0)
							precision = 0;
					} else
						precision = 0;

					if (precision > FORMAT_CONV_MAX_PRECISION) {
						precision = FORMAT_CONV_MAX_PRECISION;
					}
				} else
					adjust_precision = NO;
			} else
				adjust_precision = adjust_width = NO;

			/*
			 * Modifier check
			 */
			switch (*fmt) {
				case 'L':
					fmt++;
					modifier = LM_LONG_DOUBLE;
					break;
				case 'I':
					fmt++;
#if SIZEOF_LONG_LONG
					if (*fmt == '6' && *(fmt+1) == '4') {
						fmt += 2;
						modifier = LM_LONG_LONG;
					} else
#endif
						if (*fmt == '3' && *(fmt+1) == '2') {
							fmt += 2;
							modifier = LM_LONG;
						} else {
#ifdef _WIN64
							modifier = LM_LONG_LONG;
#else
							modifier = LM_LONG;
#endif
						}
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
				case 'p': {
						char __next = *(fmt+1);
						if ('d' == __next || 'u' == __next || 'x' == __next || 'o' == __next) {
							fmt++;
							modifier = LM_PHP_INT_T;
						} else {
							modifier = LM_STD;
						}
					}
					break;
				case 'h':
					fmt++;
					if (*fmt == 'h') {
						fmt++;
					}
					/* these are promoted to int, so no break */
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
									zvp = (zval*) va_arg(ap, zval*);
					free_zcopy = zend_make_printable_zval(zvp, &zcopy);
					if (free_zcopy) {
						zvp = &zcopy;
					}
					s_len = Z_STRLEN_P(zvp);
					s = Z_STRVAL_P(zvp);
					if (adjust_precision && precision < s_len) {
						s_len = precision;
					}
					break;
				}
				case 'u':
					switch(modifier) {
						default:
							i_num = (wide_int) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							i_num = (wide_int) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							i_num = (wide_int) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							i_num = (wide_int) va_arg(ap, u_wide_int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							i_num = (wide_int) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							i_num = (wide_int) va_arg(ap, ptrdiff_t);
							break;
#endif
						case LM_PHP_INT_T:
							i_num = (wide_int) va_arg(ap, zend_ulong);
							break;
					}
					/*
					 * The rest also applies to other integer formats, so fall
					 * into that case.
					 */
				case 'd':
				case 'i':
					/*
					 * Get the arg if we haven't already.
					 */
					if ((*fmt) != 'u') {
						switch(modifier) {
							default:
								i_num = (wide_int) va_arg(ap, int);
								break;
							case LM_LONG_DOUBLE:
								goto fmt_error;
							case LM_LONG:
								i_num = (wide_int) va_arg(ap, long int);
								break;
							case LM_SIZE_T:
#if SIZEOF_SSIZE_T
								i_num = (wide_int) va_arg(ap, ssize_t);
#else
								i_num = (wide_int) va_arg(ap, size_t);
#endif
								break;
#if SIZEOF_LONG_LONG
							case LM_LONG_LONG:
								i_num = (wide_int) va_arg(ap, wide_int);
								break;
#endif
#if SIZEOF_INTMAX_T
							case LM_INTMAX_T:
								i_num = (wide_int) va_arg(ap, intmax_t);
								break;
#endif
#if SIZEOF_PTRDIFF_T
							case LM_PTRDIFF_T:
								i_num = (wide_int) va_arg(ap, ptrdiff_t);
								break;
#endif
							case LM_PHP_INT_T:
								i_num = (wide_int) va_arg(ap, zend_long);
								break;
						}
					}
					s = ap_php_conv_10(i_num, (*fmt) == 'u', &is_negative,
								&num_buf[NUM_BUF_SIZE], &s_len);
					FIX_PRECISION(adjust_precision, precision, s, s_len);

					if (*fmt != 'u') {
						if (is_negative)
							prefix_char = '-';
						else if (print_sign)
							prefix_char = '+';
						else if (print_blank)
							prefix_char = ' ';
					}
					break;


				case 'o':
					switch(modifier) {
						default:
							ui_num = (u_wide_int) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							ui_num = (u_wide_int) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							ui_num = (u_wide_int) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							ui_num = (u_wide_int) va_arg(ap, u_wide_int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							ui_num = (u_wide_int) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							ui_num = (u_wide_int) va_arg(ap, ptrdiff_t);
							break;
#endif
						case LM_PHP_INT_T:
							ui_num = (u_wide_int) va_arg(ap, zend_ulong);
							break;
					}
					s = ap_php_conv_p2(ui_num, 3, *fmt,
								&num_buf[NUM_BUF_SIZE], &s_len);
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
							ui_num = (u_wide_int) va_arg(ap, unsigned int);
							break;
						case LM_LONG_DOUBLE:
							goto fmt_error;
						case LM_LONG:
							ui_num = (u_wide_int) va_arg(ap, unsigned long int);
							break;
						case LM_SIZE_T:
							ui_num = (u_wide_int) va_arg(ap, size_t);
							break;
#if SIZEOF_LONG_LONG
						case LM_LONG_LONG:
							ui_num = (u_wide_int) va_arg(ap, u_wide_int);
							break;
#endif
#if SIZEOF_INTMAX_T
						case LM_INTMAX_T:
							ui_num = (u_wide_int) va_arg(ap, uintmax_t);
							break;
#endif
#if SIZEOF_PTRDIFF_T
						case LM_PTRDIFF_T:
							ui_num = (u_wide_int) va_arg(ap, ptrdiff_t);
							break;
#endif
						case LM_PHP_INT_T:
							ui_num = (u_wide_int) va_arg(ap, zend_ulong);
							break;
					}
					s = ap_php_conv_p2(ui_num, 4, *fmt,
								&num_buf[NUM_BUF_SIZE], &s_len);
					FIX_PRECISION(adjust_precision, precision, s, s_len);
					if (alternate_form && ui_num != 0) {
						*--s = *fmt;	/* 'x' or 'X' */
						*--s = '0';
						s_len += 2;
					}
					break;


				case 's':
				case 'v':
					s = va_arg(ap, char *);
					if (s != NULL) {
						if (!adjust_precision) {
							s_len = strlen(s);
						} else {
							s_len = strnlen(s, precision);
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
						s = "nan";
						s_len = 3;
					} else if (zend_isinf(fp_num)) {
						s = "inf";
						s_len = 3;
					} else {
#ifdef HAVE_LOCALE_H
#ifdef ZTS
						localeconv_r(&lconv);
#else
						if (!lconv) {
							lconv = localeconv();
						}
#endif
#endif
						s = php_conv_fp((*fmt == 'f')?'F':*fmt, fp_num, alternate_form,
						 (adjust_precision == NO) ? FLOAT_DIGITS : precision,
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

					if (adjust_precision == NO)
						precision = FLOAT_DIGITS;
					else if (precision == 0)
						precision = 1;
					/*
					 * * We use &num_buf[ 1 ], so that we have room for the sign
					 */
#ifdef HAVE_LOCALE_H
#ifdef ZTS
					localeconv_r(&lconv);
#else
					if (!lconv) {
						lconv = localeconv();
					}
#endif
#endif
					s = php_gcvt(fp_num, precision, (*fmt=='H' || *fmt == 'k') ? '.' : LCONV_DECIMAL_POINT, (*fmt == 'G' || *fmt == 'H')?'E':'e', &num_buf[1]);
					if (*s == '-')
						prefix_char = *s++;
					else if (print_sign)
						prefix_char = '+';
					else if (print_blank)
						prefix_char = ' ';

					s_len = strlen(s);

					if (alternate_form && (strchr(s, '.')) == NULL)
						s[s_len++] = '.';
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
					*(va_arg(ap, int *)) = is_char? (int)((smart_string *)xbuf)->len : (int)ZSTR_LEN(((smart_str *)xbuf)->s);
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
					if (sizeof(char *) <= sizeof(u_wide_int)) {
						ui_num = (u_wide_int)((size_t) va_arg(ap, char *));
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
			if (adjust_width && adjust == RIGHT && min_width > s_len) {
				if (pad_char == '0' && prefix_char != NUL) {
					INS_CHAR(xbuf, *s, is_char);
					s++;
					s_len--;
					min_width--;
				}
				PAD_CHAR(xbuf, pad_char, min_width - s_len, is_char);
			}
			/*
			 * Print the string s.
			 */
			INS_STRING(xbuf, s, s_len, is_char);

			if (adjust_width && adjust == LEFT && min_width > s_len) {
				PAD_CHAR(xbuf, pad_char, min_width - s_len, is_char);
			}

			if (free_zcopy) {
				zval_dtor(&zcopy);
			}
		}
skip_output:
		fmt++;
	}
	return;
}
/* }}} */

/*
 * This is the general purpose conversion function.
 */
PHPAPI size_t vspprintf(char **pbuf, size_t max_len, const char *format, va_list ap) /* {{{ */
{
	smart_string buf = {0};

	/* since there are places where (v)spprintf called without checking for null,
	   a bit of defensive coding here */
	if(!pbuf) {
		return 0;
	}
	xbuf_format_converter(&buf, 1, format, ap);

	if (max_len && buf.len > max_len) {
		buf.len = max_len;
	}

	smart_string_0(&buf);

	if (buf.c) {
		*pbuf = buf.c;
		return buf.len;
	} else {
		*pbuf = estrndup("", 0);
		return 0;
	}
}
/* }}} */

PHPAPI size_t spprintf(char **pbuf, size_t max_len, const char *format, ...) /* {{{ */
{
	size_t cc;
	va_list ap;

	va_start(ap, format);
	cc = vspprintf(pbuf, max_len, format, ap);
	va_end(ap);
	return (cc);
}
/* }}} */

PHPAPI zend_string *vstrpprintf(size_t max_len, const char *format, va_list ap) /* {{{ */
{
	smart_str buf = {0};

	xbuf_format_converter(&buf, 0, format, ap);

	if (!buf.s) {
		return ZSTR_EMPTY_ALLOC();
	}

	if (max_len && ZSTR_LEN(buf.s) > max_len) {
		ZSTR_LEN(buf.s) = max_len;
	}

	smart_str_0(&buf);
	return buf.s;
}
/* }}} */

PHPAPI zend_string *strpprintf(size_t max_len, const char *format, ...) /* {{{ */
{
	va_list ap;
	zend_string *str;

	va_start(ap, format);
	str = vstrpprintf(max_len, format, ap);
	va_end(ap);
	return str;
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
