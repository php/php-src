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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

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
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include "php.h"

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

#include "snprintf.h"

#define NUL             '\0'
#define INT_NULL        ((int *)0)

#define S_NULL          "(null)"
#define S_NULL_LEN      6

#define FLOAT_DIGITS    6
#define EXPONENT_LENGTH 10

#include "zend_smart_str.h"
#include "zend_smart_string.h"

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
 * which can be at most max length of double
 */
#define NUM_BUF_SIZE ZEND_DOUBLE_MAX_LENGTH

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
		while (s_len < (size_t)precision) {				\
			*--s = '0';                             	\
			s_len++;                                	\
		}												\
} while (0)

/* }}} */

/*
 * Do format conversion placing the output in buffer
 */
static void xbuf_format_converter(void *xbuf, bool is_char, const char *fmt, va_list ap) /* {{{ */
{
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
	uint64_t ui_num = (uint64_t) 0;

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

	while (*fmt) {
		if (*fmt != '%') {
			INS_CHAR(xbuf, *fmt, is_char);
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
				case 'S': {
					zend_string *str = va_arg(ap, zend_string*);
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
					s = va_arg(ap, char *);
					if (s != NULL) {
						if (!adjust_precision) {
							s_len = strlen(s);
						} else {
							s_len = zend_strnlen(s, precision);
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

					if (adjust_precision == false)
						precision = FLOAT_DIGITS;
					else if (precision == 0)
						precision = 1;
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

			if (adjust_width && adjust == LEFT && (size_t)min_width > s_len) {
				PAD_CHAR(xbuf, pad_char, min_width - s_len, is_char);
			}

			zend_tmp_string_release(tmp_str);
		}
skip_output:
		fmt++;
	}
	return;
}
/* }}} */

PHPAPI void php_printf_to_smart_string(smart_string *buf, const char *format, va_list ap) /* {{{ */
{
	xbuf_format_converter(buf, 1, format, ap);
}
/* }}} */

PHPAPI void php_printf_to_smart_str(smart_str *buf, const char *format, va_list ap) /* {{{ */
{
	xbuf_format_converter(buf, 0, format, ap);
}
/* }}} */
