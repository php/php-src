/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "zend.h"
#include "php.h"
#include "spprintf.h"
#include "phpdbg.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"
#include "ext/standard/html.h"

#ifdef _WIN32
#	include "win32/time.h"
#elif defined(HAVE_SYS_IOCTL_H)
#	include "sys/ioctl.h"
#	ifndef GWINSZ_IN_SYS_IOCTL
#		include <termios.h>
#	endif
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

/* {{{ color structures */
const static phpdbg_color_t colors[] = {
	PHPDBG_COLOR_D("none",             "0;0"),

	PHPDBG_COLOR_D("white",            "0;64"),
	PHPDBG_COLOR_D("white-bold",       "1;64"),
	PHPDBG_COLOR_D("white-underline",  "4;64"),
	PHPDBG_COLOR_D("red",              "0;31"),
	PHPDBG_COLOR_D("red-bold",         "1;31"),
	PHPDBG_COLOR_D("red-underline",    "4;31"),
	PHPDBG_COLOR_D("green",            "0;32"),
	PHPDBG_COLOR_D("green-bold",       "1;32"),
	PHPDBG_COLOR_D("green-underline",  "4;32"),
	PHPDBG_COLOR_D("yellow",           "0;33"),
	PHPDBG_COLOR_D("yellow-bold",      "1;33"),
	PHPDBG_COLOR_D("yellow-underline", "4;33"),
	PHPDBG_COLOR_D("blue",             "0;34"),
	PHPDBG_COLOR_D("blue-bold",        "1;34"),
	PHPDBG_COLOR_D("blue-underline",   "4;34"),
	PHPDBG_COLOR_D("purple",           "0;35"),
	PHPDBG_COLOR_D("purple-bold",      "1;35"),
	PHPDBG_COLOR_D("purple-underline", "4;35"),
	PHPDBG_COLOR_D("cyan",             "0;36"),
	PHPDBG_COLOR_D("cyan-bold",        "1;36"),
	PHPDBG_COLOR_D("cyan-underline",   "4;36"),
	PHPDBG_COLOR_D("black",            "0;30"),
	PHPDBG_COLOR_D("black-bold",       "1;30"),
	PHPDBG_COLOR_D("black-underline",  "4;30"),
	PHPDBG_COLOR_END
}; /* }}} */

/* {{{ */
const static phpdbg_element_t elements[] = {
	PHPDBG_ELEMENT_D("prompt", PHPDBG_COLOR_PROMPT),
	PHPDBG_ELEMENT_D("error", PHPDBG_COLOR_ERROR),
	PHPDBG_ELEMENT_D("notice", PHPDBG_COLOR_NOTICE),
	PHPDBG_ELEMENT_END
}; /* }}} */

PHPDBG_API int phpdbg_is_numeric(const char *str) /* {{{ */
{
	if (!str)
		return 0;

	for (; *str; str++) {
		if (isspace(*str) || *str == '-') {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

PHPDBG_API int phpdbg_is_empty(const char *str) /* {{{ */
{
	if (!str)
		return 1;

	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

PHPDBG_API int phpdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

PHPDBG_API int phpdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	char *sep = NULL;

	if (strstr(str, "#") != NULL)
		return 0;

	if (strstr(str, " ") != NULL)
		return 0;

	sep = strstr(str, "::");

	if (!sep || sep == str || sep+2 == str+len-1) {
		return 0;
	}

	if (class != NULL) {

		if (str[0] == '\\') {
			str++;
			len--;
		}

		*class = estrndup(str, sep - str);
		(*class)[sep - str] = 0;
	}

	if (method != NULL) {
		*method = estrndup(sep+2, str + len - (sep + 2));
	}

	return 1;
} /* }}} */

PHPDBG_API char *phpdbg_resolve_path(const char *path TSRMLS_DC) /* {{{ */
{
	char resolved_name[MAXPATHLEN];

	if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
		return NULL;
	}

	return estrdup(resolved_name);
} /* }}} */

PHPDBG_API const char *phpdbg_current_file(TSRMLS_D) /* {{{ */
{
	const char *file = zend_get_executed_filename(TSRMLS_C);

	if (memcmp(file, "[no active file]", sizeof("[no active file]")) == 0) {
		return PHPDBG_G(exec);
	}

	return file;
} /* }}} */

PHPDBG_API const zend_function *phpdbg_get_function(const char *fname, const char *cname TSRMLS_DC) /* {{{ */
{
	zend_function *func = NULL;
	size_t fname_len = strlen(fname);
	char *lcname = zend_str_tolower_dup(fname, fname_len);

	if (cname) {
		zend_class_entry **ce;
		size_t cname_len = strlen(cname);
		char *lc_cname = zend_str_tolower_dup(cname, cname_len);
		int ret = zend_lookup_class(lc_cname, cname_len, &ce TSRMLS_CC);

		efree(lc_cname);

		if (ret == SUCCESS) {
			zend_hash_find(&(*ce)->function_table, lcname, fname_len+1,
				(void**)&func);
		}
	} else {
		zend_hash_find(EG(function_table), lcname, fname_len+1,
			(void**)&func);
	}

	efree(lcname);
	return func;
} /* }}} */

PHPDBG_API char *phpdbg_trim(const char *str, size_t len, size_t *new_len) /* {{{ */
{
	const char *p = str;
	char *new = NULL;

	while (p && isspace(*p)) {
		++p;
		--len;
	}

	while (*p && isspace(*(p + len -1))) {
		--len;
	}

	if (len == 0) {
		new = estrndup("", sizeof(""));
		*new_len = 0;
	} else {
		new = estrndup(p, len);
		*(new + len) = '\0';

		if (new_len) {
			*new_len = len;
		}
	}

	return new;

} /* }}} */

/* copied from php-src/main/snprintf.c and slightly modified */
/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 * Emax = 1023
 * NDIG = 320
 * NUM_BUF_SIZE >= strlen("-") + Emax + strlrn(".") + NDIG + strlen("E+1023") + 1;
 */
#define NUM_BUF_SIZE        2048

/*
 * Descriptor for buffer area
 */
struct buf_area {
	char *buf_end;
	char *nextb; /* pointer to next byte to read/write   */
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
    if ( adjust )					\
	while ( s_len < precision )			\
	{						\
	    *--s = '0' ;				\
	    s_len++ ;					\
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
	while ( width > len )

/*
 * Prefix the character ch to the string str
 * Increase length
 * Set the has_prefix flag
 */
#define PREFIX( str, length, ch )	 *--str = ch ; length++ ; has_prefix = YES


#ifdef HAVE_LOCALE_H
#include <locale.h>
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#else
#define LCONV_DECIMAL_POINT '.'
#endif
#define NUL '\0'
#define S_NULL "(null)"
#define S_NULL_LEN 6
#define FLOAT_DIGITS 6

/*
 * Do format conversion placing the output in buffer
 */
static int format_converter(register buffy *odp, const char *fmt, zend_bool escape_xml, va_list ap TSRMLS_DC) {
	char *sp;
	char *bep;
	int cc = 0;
	int i;

	char *s = NULL, *free_s = NULL;
	int s_len, free_zcopy;
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
	u_wide_int ui_num;

	char num_buf[NUM_BUF_SIZE];
	char char_buf[2];			/* for printing %% and %<unknown> */

#ifdef HAVE_LOCALE_H
	struct lconv *lconv = NULL;
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

	sp = odp->nextb;
	bep = odp->buf_end;

	while (*fmt) {
		if (*fmt != '%') {
			INS_CHAR(*fmt, sp, bep, cc);
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

					if (precision > FORMAT_CONV_MAX_PRECISION && *fmt != 's' && *fmt != 'v' && *fmt != 'b') {
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
				case 'Z':
					zvp = (zval*) va_arg(ap, zval*);
					zend_make_printable_zval(zvp, &zcopy, &free_zcopy);
					if (free_zcopy) {
						zvp = &zcopy;
					}
					s_len = Z_STRLEN_P(zvp);
					s = Z_STRVAL_P(zvp);
					if (adjust_precision && precision < s_len) {
						s_len = precision;
					}
					break;
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
				case 'v':
					s = va_arg(ap, char *);
					if (s != NULL) {
						if (adjust_precision) {
							s_len = precision;
						} else {
							s_len = strlen(s);
						}

						if (escape_xml) {
							/* added: support for xml escaping */

							int old_slen = s_len, i = 0;
							char *old_s = s, *s_ptr;
							free_s = s_ptr = s = emalloc(old_slen * 6 + 1);
							do {
								if (old_s[i] == '&' || old_s[i] == '"') {
									*s_ptr++ = '&';
									if (old_s[i] == '"') {
										s_len += 5;
										*s_ptr++ = 'q';
										*s_ptr++ = 'u';
										*s_ptr++ = 'o';
										*s_ptr++ = 't';
									} else {
										s_len += 4;
										*s_ptr++ = 'a';
										*s_ptr++ = 'm';
										*s_ptr++ = 'p';
									}
									*s_ptr++ = ';';
								} else {
									*s_ptr++ = old_s[i];
								}
							} while (i++ < old_slen);
						}
					} else {
						s = S_NULL;
						s_len = S_NULL_LEN;
					}
					pad_char = ' ';
					break;


				case 'b':
					if (escape_xml) {
						s = PHPDBG_G(err_buf).xml;
					} else {
						s = PHPDBG_G(err_buf).msg;
					}

					if (s != NULL) {
						if (escape_xml) {
							s_len = PHPDBG_G(err_buf).xmllen;
						} else {
							s_len = PHPDBG_G(err_buf).msglen;
						}

						if (adjust_precision && precision != s_len) {
							s_len = precision;
						}
					} else {
						s = S_NULL;
						s_len = S_NULL_LEN;
					}
					pad_char = ' ';
					break;


				case 'r':
					s_len = spprintf(&s, 0, "req=\"%lu\"", PHPDBG_G(req_id));
					free_s = s;
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
#ifdef HAVE_LOCALE_H
						if (!lconv) {
							lconv = localeconv();
						}
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

					if (adjust_precision == NO) {
						precision = FLOAT_DIGITS;
					} else if (precision == 0) {
						precision = 1;
					}
					/*
					 * * We use &num_buf[ 1 ], so that we have room for the sign
					 */
#ifdef HAVE_LOCALE_H
					if (!lconv) {
						lconv = localeconv();
					}
#endif
					s = php_gcvt(fp_num, precision, (*fmt=='H' || *fmt == 'k') ? '.' : LCONV_DECIMAL_POINT, (*fmt == 'G' || *fmt == 'H')?'E':'e', &num_buf[1]);
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

			if (adjust_width && adjust == LEFT && min_width > s_len)
				PAD(min_width, s_len, pad_char);
			if (free_zcopy) {
				zval_dtor(&zcopy);
			}
		}
skip_output:
		if (free_s) {
			efree(free_s);
			free_s = NULL;
		}

		fmt++;
	}
	odp->nextb = sp;
	return (cc);
}

static void strx_printv(int *ccp, char *buf, size_t len, const char *format, zend_bool escape_xml, va_list ap TSRMLS_DC) {
	buffy od;
	int cc;

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
	cc = format_converter(&od, format, escape_xml, ap TSRMLS_CC);
	if (len != 0 && od.nextb <= od.buf_end) {
		*(od.nextb) = '\0';
	}
	if (ccp) {
		*ccp = cc;
	}
}

static int phpdbg_xml_vsnprintf(char *buf, size_t len, const char *format, zend_bool escape_xml, va_list ap TSRMLS_DC) {
	int cc;

	strx_printv(&cc, buf, len, format, escape_xml, ap TSRMLS_CC);
	return (cc);
}

PHPAPI int phpdbg_xml_vasprintf(char **buf, const char *format, zend_bool escape_xml, va_list ap TSRMLS_DC) {
	va_list ap2;
	int cc;

	va_copy(ap2, ap);
	cc = phpdbg_xml_vsnprintf(NULL, 0, format, escape_xml, ap2 TSRMLS_CC);
	va_end(ap2);

	*buf = NULL;

	if (cc >= 0) {
		if ((*buf = emalloc(++cc)) != NULL) {
			if ((cc = phpdbg_xml_vsnprintf(*buf, cc, format, escape_xml, ap TSRMLS_CC)) < 0) {
				efree(*buf);
				*buf = NULL;
			}
		}
	}

	return cc;
}
/* copy end */

static int phpdbg_encode_xml(char **buf, char *msg, int msglen, char from, char *to) {
	int i;
	char *tmp = *buf = emalloc(msglen * strlen(to));
	int tolen = strlen(to);
	for (i = 0; i++ < msglen; msg++) {
		if (*msg == '&') {
			memcpy(tmp, ZEND_STRL("&amp;"));
			tmp += sizeof("&amp;") - 1;
		} else if (*msg == from) {
			memcpy(tmp, to, tolen);
			tmp += tolen;
		} else {
			*tmp++ = *msg;
		}
	}

	{
		int len = tmp - *buf;
		*buf = erealloc(*buf, len);
		return len;
	}
}

static int phpdbg_process_print(int fd, int type, const char *tag, const char *msg, int msglen, const char *xml, int xmllen TSRMLS_DC) {
	char *msgout = NULL, *buf;
	int msgoutlen, xmloutlen, buflen;
	const char *severity;

	if ((PHPDBG_G(flags) & PHPDBG_WRITE_XML) && PHPDBG_G(in_script_xml) && PHPDBG_G(in_script_xml) != type) {
		write(fd, ZEND_STRL("</stream>"));
		PHPDBG_G(in_script_xml) = 0;
	}

	/* TODO(anyone) colours */

	switch (type) {
		case P_ERROR:
			severity = "error";
			if (!PHPDBG_G(last_was_newline)) {
				if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
					write(fd, ZEND_STRL("<phpdbg>\n</phpdbg>"));
				} else {
					write(fd, ZEND_STRL("\n"));
				}
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = spprintf(&msgout, 0, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_ERROR]->code, msglen, msg);
			} else {
				msgoutlen = spprintf(&msgout, 0, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_NOTICE:
			severity = "notice";
			if (!PHPDBG_G(last_was_newline)) {
				if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
					write(fd, ZEND_STRL("<phpdbg>\n</phpdbg>"));
				} else {
					write(fd, ZEND_STRL("\n"));
				}
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = spprintf(&msgout, 0, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_NOTICE]->code, msglen, msg);
			} else {
				msgoutlen = spprintf(&msgout, 0, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_WRITELN:
			severity = "normal";
			if (msg) {
				msgoutlen = spprintf(&msgout, 0, "%.*s\n", msglen, msg);
			} else {
				msgoutlen = 1;
				msgout = estrdup("\n");
			}
			PHPDBG_G(last_was_newline) = 1;
			break;

		case P_WRITE:
			severity = "normal";
			if (msg) {
				msgout = estrndup(msg, msglen);
				msgoutlen = msglen;
				PHPDBG_G(last_was_newline) = msg[msglen - 1] == '\n';
			} else {
				msgoutlen = 0;
				msgout = estrdup("");
			}
			break;

		case P_STDOUT:
		case P_STDERR:
			if (msg) {
				PHPDBG_G(last_was_newline) = msg[msglen - 1] == '\n';
				if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
					if (PHPDBG_G(in_script_xml) != type) {
						char *stream_buf;
						int stream_buflen = spprintf(&stream_buf, 0, "<stream type=\"%s\">", type == P_STDERR ? "stderr" : "stdout");
						write(fd, stream_buf, stream_buflen);
						efree(stream_buf);
						PHPDBG_G(in_script_xml) = type;
					}
					buf = php_escape_html_entities((unsigned char *) msg, msglen, (size_t *) &buflen, 0, ENT_NOQUOTES, PG(internal_encoding) && PG(internal_encoding)[0] ? PG(internal_encoding) : (SG(default_charset) ? SG(default_charset) : "UTF-8") TSRMLS_CC);
					write(fd, buf, buflen);
					efree(buf);
				} else {
					write(fd, msg, msglen);
				}
				return msglen;
			}
		break;

		/* no formatting on logging output */
		case P_LOG:
			severity = "log";
			if (msg) {
				struct timeval tp;
				if (gettimeofday(&tp, NULL) == SUCCESS) {
					msgoutlen = spprintf(&msgout, 0, "[%ld %.8F]: %.*s\n", tp.tv_sec, tp.tv_usec / 1000000., msglen, msg);
				} else {
					msgoutlen = FAILURE;
				}
			}
			break;
	}

	if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
		char *xmlout;

		if (PHPDBG_G(req_id)) {
			char *xmlbuf = NULL;
			xmllen = spprintf(&xmlbuf, 0, "req=\"%lu\" %.*s", PHPDBG_G(req_id), xmllen, xml);
			xml = xmlbuf;
		}
		if (msgout) {
//			unsigned char *tmp = msgout;
//			buf = php_escape_html_entities(tmp, msgoutlen, (size_t *) &buflen, 0, ENT_COMPAT, PG(internal_encoding) && PG(internal_encoding)[0] ? PG(internal_encoding) : (SG(default_charset) ? SG(default_charset) : "UTF-8") TSRMLS_CC);
			buflen = phpdbg_encode_xml(&buf, msgout, msgoutlen, '"', "&quot;");
			xmloutlen = spprintf(&xmlout, 0, "<%s severity=\"%s\" %.*s msgout=\"%.*s\" />", tag, severity, xmllen, xml, buflen, buf);

			efree(buf);
		} else {
			xmloutlen = spprintf(&xmlout, 0, "<%s severity=\"%s\" %.*s msgout=\"\" />", tag, severity, xmllen, xml);
		}

		write(fd, xmlout, xmloutlen);
		efree(xmlout);
	} else if (msgout) {
		write(fd, msgout, msgoutlen);
	}

	if (PHPDBG_G(req_id) && (PHPDBG_G(flags) & PHPDBG_WRITE_XML)) {
		efree((char *) xml);
	}

	if (msgout) {
		efree(msgout);
	}

	return msgout ? msgoutlen : xmloutlen;
} /* }}} */

PHPDBG_API int phpdbg_vprint(int type TSRMLS_DC, int fd, const char *tag, const char *xmlfmt, const char *strfmt, va_list args) {
	char *msg = NULL, *xml = NULL;
	int msglen = 0, xmllen = 0;
	int len;
	va_list argcpy;

	if (strfmt != NULL && strlen(strfmt) > 0L) {
		va_copy(argcpy, args);
		msglen = phpdbg_xml_vasprintf(&msg, strfmt, 0, argcpy TSRMLS_CC);
		va_end(argcpy);
	}

	if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
		if (xmlfmt != NULL && strlen(xmlfmt) > 0L) {
			va_copy(argcpy, args);
			xmllen = phpdbg_xml_vasprintf(&xml, xmlfmt, 1, argcpy TSRMLS_CC);
			va_end(argcpy);
		}
	}

	if (PHPDBG_G(err_buf).active && type != P_STDOUT && type != P_STDERR) {
		PHPDBG_G(err_buf).type = type;
		PHPDBG_G(err_buf).fd = fd;
		PHPDBG_G(err_buf).tag = estrdup(tag);
		PHPDBG_G(err_buf).msg = msg;
		PHPDBG_G(err_buf).msglen = msglen;
		PHPDBG_G(err_buf).xml = xml;
		PHPDBG_G(err_buf).xmllen = xmllen;

		return msglen;
	}

	len = phpdbg_process_print(fd, type, tag, msg, msglen, xml, xmllen TSRMLS_CC);

	if (msg) {
		efree(msg);
	}

	if (xml) {
		efree(xml);
	}

	return len;
}

PHPDBG_API void phpdbg_free_err_buf(TSRMLS_D) {
	if (PHPDBG_G(err_buf).type == 0) {
		return;
	}

	PHPDBG_G(err_buf).type = 0;

	efree(PHPDBG_G(err_buf).tag);
	efree(PHPDBG_G(err_buf).msg);
	efree(PHPDBG_G(err_buf).xml);
}

PHPDBG_API void phpdbg_activate_err_buf(zend_bool active TSRMLS_DC) {
	PHPDBG_G(err_buf).active = active;
}

PHPDBG_API int phpdbg_output_err_buf(const char *tag, const char *xmlfmt, const char *strfmt TSRMLS_DC, ...) {
	int len;
	va_list args;
	int errbuf_active = PHPDBG_G(err_buf).active;

	PHPDBG_G(err_buf).active = 0;

#ifdef ZTS
	va_start(args, tsrm_ls);
#else
	va_start(args, strfmt);
#endif
	len = phpdbg_vprint(PHPDBG_G(err_buf).type TSRMLS_CC, PHPDBG_G(err_buf).fd, tag ? tag : PHPDBG_G(err_buf).tag, xmlfmt, strfmt, args);
	va_end(args);

	PHPDBG_G(err_buf).active = errbuf_active;
	phpdbg_free_err_buf(TSRMLS_C);

	return len;
}

PHPDBG_API int phpdbg_print(int type TSRMLS_DC, int fd, const char *tag, const char *xmlfmt, const char *strfmt, ...) {
	va_list args;
	int len;

	va_start(args, strfmt);
	len = phpdbg_vprint(type TSRMLS_CC, fd, tag, xmlfmt, strfmt, args);
	va_end(args);

	return len;
}

PHPDBG_API int phpdbg_xml_internal(int fd TSRMLS_DC, const char *fmt, ...) {
	int len = 0;

	if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
		va_list args;
		char *buffer;
		int buflen;

		va_start(args, fmt);
		buflen = phpdbg_xml_vasprintf(&buffer, fmt, 0, args TSRMLS_CC);
		va_end(args);

		if (PHPDBG_G(in_script_xml)) {
			write(fd, ZEND_STRL("</stream>"));
			PHPDBG_G(in_script_xml) = 0;
		}

		len = write(fd, buffer, buflen);
		efree(buffer);
	}

	return len;
}

PHPDBG_API int phpdbg_log_internal(int fd TSRMLS_DC, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	va_start(args, fmt);
	buflen = vspprintf(&buffer, 0, fmt, args);
	va_end(args);

	len = write(fd, buffer, buflen);
	efree(buffer);

	return len;
}

PHPDBG_API int phpdbg_out_internal(int fd TSRMLS_DC, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	va_start(args, fmt);
	buflen = vspprintf(&buffer, 0, fmt, args);
	va_end(args);

	if (PHPDBG_G(flags) & PHPDBG_WRITE_XML) {
		if (PHPDBG_G(in_script_xml)) {
			write(fd, ZEND_STRL("</stream>"));
			PHPDBG_G(in_script_xml) = 0;
		}

		write(fd, ZEND_STRL("<phpdbg>"));
		len = write(fd, buffer, buflen);
		write(fd, ZEND_STRL("</phpdbg>"));
	} else {
		len = write(fd, buffer, buflen);
	}

	return len;
}


PHPDBG_API int phpdbg_rlog(int fd, const char *fmt, ...) { /* {{{ */
	int rc = 0;

	va_list args;
	struct timeval tp;

	va_start(args, fmt);
	if (gettimeofday(&tp, NULL) == SUCCESS) {
		char friendly[100];
		char *format = NULL, *buffer = NULL, *outbuf = NULL;
		const time_t tt = tp.tv_sec;

		strftime(friendly, 100, "%a %b %d %T.%%04d %Y", localtime(&tt));
		spprintf(&buffer, 0,  friendly, tp.tv_usec/1000);
		spprintf(&format, 0, "[%s]: %s\n", buffer, fmt);
		rc = vspprintf(&outbuf, 0, format, args);

		if (outbuf) {
			rc = write(fd, outbuf, rc);
			efree(outbuf);
		}

		efree(format);
		efree(buffer);
	}
	va_end(args);

	return rc;
} /* }}} */

PHPDBG_API const phpdbg_color_t *phpdbg_get_color(const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const phpdbg_color_t *color = colors;

	while (color && color->name) {
		if (name_length == color->name_length &&
			memcmp(name, color->name, name_length) == SUCCESS) {
			phpdbg_debug("phpdbg_get_color(%s, %lu): %s", name, name_length, color->code);
			return color;
		}
		++color;
	}

	phpdbg_debug("phpdbg_get_color(%s, %lu): failed", name, name_length);

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_set_color(int element, const phpdbg_color_t *color TSRMLS_DC) /* {{{ */
{
	PHPDBG_G(colors)[element] = color;
} /* }}} */

PHPDBG_API void phpdbg_set_color_ex(int element, const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const phpdbg_color_t *color = phpdbg_get_color(name, name_length TSRMLS_CC);

	if (color) {
		phpdbg_set_color(element, color TSRMLS_CC);
	} else PHPDBG_G(colors)[element] = colors;
} /* }}} */

PHPDBG_API const phpdbg_color_t* phpdbg_get_colors(TSRMLS_D) /* {{{ */
{
	return colors;
} /* }}} */

PHPDBG_API int phpdbg_get_element(const char *name, size_t len TSRMLS_DC) {
	const phpdbg_element_t *element = elements;

	while (element && element->name) {
		if (len == element->name_length) {
			if (strncasecmp(name, element->name, len) == SUCCESS) {
				return element->id;
			}
		}
		element++;
	}

	return PHPDBG_COLOR_INVALID;
}

PHPDBG_API void phpdbg_set_prompt(const char *prompt TSRMLS_DC) /* {{{ */
{
	/* free formatted prompt */
	if (PHPDBG_G(prompt)[1]) {
		free(PHPDBG_G(prompt)[1]);
		PHPDBG_G(prompt)[1] = NULL;
	}
	/* free old prompt */
	if (PHPDBG_G(prompt)[0]) {
		free(PHPDBG_G(prompt)[0]);
		PHPDBG_G(prompt)[0] = NULL;
	}

	/* copy new prompt */
	PHPDBG_G(prompt)[0] = strdup(prompt);
} /* }}} */

PHPDBG_API const char *phpdbg_get_prompt(TSRMLS_D) /* {{{ */
{
	/* find cached prompt */
	if (PHPDBG_G(prompt)[1]) {
		return PHPDBG_G(prompt)[1];
	}

	/* create cached prompt */
#ifndef HAVE_LIBEDIT
	/* TODO: libedit doesn't seems to support coloured prompt */
	if ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED)) {
		asprintf(
			&PHPDBG_G(prompt)[1], "\033[%sm%s\033[0m ",
			PHPDBG_G(colors)[PHPDBG_COLOR_PROMPT]->code,
			PHPDBG_G(prompt)[0]);
	} else
#endif
	{
		asprintf(
			&PHPDBG_G(prompt)[1], "%s ",
			PHPDBG_G(prompt)[0]);
	}

	return PHPDBG_G(prompt)[1];
} /* }}} */

int phpdbg_rebuild_symtable(TSRMLS_D) {
	if (!EG(active_op_array)) {
		phpdbg_error("inactive", "type=\"op_array\"", "No active op array!");
		return FAILURE;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);

		if (!EG(active_symbol_table)) {
			phpdbg_error("inactive", "type=\"symbol_table\"", "No active symbol table!");
			return FAILURE;
		}
	}

	return SUCCESS;
}

PHPDBG_API int phpdbg_get_terminal_width(TSRMLS_D) /* {{{ */
{
	int columns;
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(HAVE_SYS_IOCTL_H) && defined(TIOCGWINSZ)
	struct winsize w;

	columns = ioctl(fileno(stdout), TIOCGWINSZ, &w) == 0 ? w.ws_col : 80;
#else
	columns = 80;
#endif
	return columns;
} /* }}} */

PHPDBG_API void phpdbg_set_async_io(int fd) {
#ifndef _WIN32
	int flags;
	fcntl(STDIN_FILENO, F_SETOWN, getpid());
	flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags | FASYNC);
#endif
}

int phpdbg_safe_class_lookup(const char *name, int name_length, zend_class_entry ***ce TSRMLS_DC) {
	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		char *lc_name, *lc_free;
		int lc_length, ret = FAILURE;

		if (name == NULL || !name_length) {
			return FAILURE;
		}

		lc_free = lc_name = emalloc(name_length + 1);
		zend_str_tolower_copy(lc_name, name, name_length);
		lc_length = name_length + 1;

		if (lc_name[0] == '\\') {
			lc_name += 1;
			lc_length -= 1;
		}

		phpdbg_try_access {
			ret = zend_hash_find(EG(class_table), lc_name, lc_length, (void **) &ce);
		} phpdbg_catch_access {
			phpdbg_error("signalsegv", "class=\"%.*s\"", "Could not fetch class %.*s, invalid data source", name_length, name);
		} phpdbg_end_try_access();

		efree(lc_free);
		return ret;
	} else {
		return zend_lookup_class(name, name_length, ce TSRMLS_CC);
	}
}

char *phpdbg_get_property_key(char *key) {
	if (*key != 0) {
		return key;
	}
	return strchr(key + 1, 0) + 1;
}

static int phpdbg_parse_variable_arg_wrapper(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval **zv, phpdbg_parse_var_func callback TSRMLS_DC) {
	return callback(name, len, keyname, keylen, parent, zv TSRMLS_CC);
}

PHPDBG_API int phpdbg_parse_variable(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_func callback, zend_bool silent TSRMLS_DC) {
	return phpdbg_parse_variable_with_arg(input, len, parent, i, (phpdbg_parse_var_with_arg_func) phpdbg_parse_variable_arg_wrapper, silent, callback TSRMLS_CC);
}

PHPDBG_API int phpdbg_parse_variable_with_arg(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_with_arg_func callback, zend_bool silent, void *arg TSRMLS_DC) {
	int ret = FAILURE;
	zend_bool new_index = 1;
	char *last_index;
	size_t index_len = 0;
	zval **zv;

	if (len < 2 || *input != '$') {
		goto error;
	}

	while (i++ < len) {
		if (i == len) {
			new_index = 1;
		} else {
			switch (input[i]) {
				case '[':
					new_index = 1;
					break;
				case ']':
					break;
				case '>':
					if (last_index[index_len - 1] == '-') {
						new_index = 1;
						index_len--;
					}
					break;

				default:
					if (new_index) {
						last_index = input + i;
						new_index = 0;
					}
					if (input[i - 1] == ']') {
						goto error;
					}
					index_len++;
			}
		}

		if (new_index && index_len == 0) {
			HashPosition position;
			for (zend_hash_internal_pointer_reset_ex(parent, &position);
			     zend_hash_get_current_data_ex(parent, (void **)&zv, &position) == SUCCESS;
			     zend_hash_move_forward_ex(parent, &position)) {
				if (i == len || (i == len - 1 && input[len - 1] == ']')) {
					zval *key = emalloc(sizeof(zval));
					size_t namelen;
					char *name;
					char *keyname = estrndup(last_index, index_len);
					zend_hash_get_current_key_zval_ex(parent, key, &position);
					convert_to_string(key);
					name = emalloc(i + Z_STRLEN_P(key) + 2);
					namelen = sprintf(name, "%.*s%s%s", (int)i, input, phpdbg_get_property_key(Z_STRVAL_P(key)), input[len - 1] == ']'?"]":"");
					efree(key);

					ret = callback(name, namelen, keyname, index_len, parent, zv, arg TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
					phpdbg_parse_variable_with_arg(input, len, Z_OBJPROP_PP(zv), i, callback, silent, arg TSRMLS_CC);
				} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
					phpdbg_parse_variable_with_arg(input, len, Z_ARRVAL_PP(zv), i, callback, silent, arg TSRMLS_CC);
				} else {
					/* Ignore silently */
				}
			}
			return ret;
		} else if (new_index) {
			char last_chr = last_index[index_len];
			last_index[index_len] = 0;
			if (zend_symtable_find(parent, last_index, index_len + 1, (void **)&zv) == FAILURE) {
				if (!silent) {
					phpdbg_error("variable", "type=\"undefined\" variable=\"%.*s\"", "%.*s is undefined", (int) i, input);
				}
				return FAILURE;
			}
			last_index[index_len] = last_chr;
			if (i == len) {
				char *name = estrndup(input, len);
				char *keyname = estrndup(last_index, index_len);

				ret = callback(name, len, keyname, index_len, parent, zv, arg TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
			} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
				parent = Z_OBJPROP_PP(zv);
			} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
				parent = Z_ARRVAL_PP(zv);
			} else {
				phpdbg_error("variable", "type=\"notiterable\" variable=\"%.*s\"", "%.*s is nor an array nor an object", (int) i, input);
				return FAILURE;
			}
			index_len = 0;
		}
	}

	return ret;
	error:
		phpdbg_error("variable", "type=\"invalidinput\"", "Malformed input");
		return FAILURE;
}
