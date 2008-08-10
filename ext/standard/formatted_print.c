/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <math.h>				/* modf() */
#include "php.h"
#include "ext/standard/head.h"
#include "php_string.h"
#include "zend_execute.h"
#include <stdio.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#else
#define LCONV_DECIMAL_POINT '.'
#endif

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ADJ_WIDTH 1
#define ADJ_PRECISION 2
#define NUM_BUF_SIZE 500
#define	NDIG 80
#define FLOAT_DIGITS 6
#define FLOAT_PRECISION 6
#define MAX_FLOAT_DIGITS 38
#define MAX_FLOAT_PRECISION 40

#define PHP_OUTPUT 0
#define PHP_RUNTIME 1

#if 0
/* trick to control varargs functions through cpp */
# define PRINTF_DEBUG(arg) php_printf arg
#else
# define PRINTF_DEBUG(arg)
#endif

static char hexchars[] = "0123456789abcdef";
static char HEXCHARS[] = "0123456789ABCDEF";

static UChar u_hexchars[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66};
static UChar u_HEXCHARS[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};

/* php_sprintf_appendchar() {{{ */
inline static void
php_sprintf_appendchar(char **buffer, int *pos, int *size, char add TSRMLS_DC)
{
	if ((*pos + 1) >= *size) {
		*size <<= 1;
		PRINTF_DEBUG(("%s(): ereallocing buffer to %d bytes\n", get_active_function_name(TSRMLS_C), *size));
		*buffer = erealloc(*buffer, *size);
	}
	PRINTF_DEBUG(("sprintf: appending '%c', pos=\n", add, *pos));
	(*buffer)[(*pos)++] = add;
}
/* }}} */

/* php_u_sprintf_appendchar() {{{ */
inline static void
php_u_sprintf_appendchar(UChar **buffer, int *pos, int *size, UChar add TSRMLS_DC)
{
	if ((*pos + 1) >= *size) {
		*size <<= 1;
		*buffer = eurealloc(*buffer, *size);
	}
	(*buffer)[(*pos)++] = add;
}
/* }}} */

/* php_sprintf_appendstring() {{{ */
inline static void
php_sprintf_appendstring(char **buffer, int *pos, int *size, char *add,
						   int min_width, int max_width, char padding,
						   int alignment, int len, int neg, int expprec, int always_sign)
{
	register int npad;
	int req_size;
	int copy_len;
	int m_width;

	copy_len = (expprec ? MIN(max_width, len) : len);
	npad = min_width - copy_len;

	if (npad < 0) {
		npad = 0;
	}
	
	PRINTF_DEBUG(("sprintf: appendstring(%x, %d, %d, \"%s\", %d, '%c', %d)\n",
				  *buffer, *pos, *size, add, min_width, padding, alignment));
	m_width = MAX(min_width, copy_len);

	if(m_width > INT_MAX - *pos - 1) {
		zend_error_noreturn(E_ERROR, "Field width %d is too long", m_width);
	}

	req_size = *pos + m_width + 1;

	if (req_size > *size) {
		while (req_size > *size) {
			if(*size > INT_MAX/2) {
				zend_error_noreturn(E_ERROR, "Field width %d is too long", req_size); 
			}
			*size <<= 1;
		}
		PRINTF_DEBUG(("sprintf ereallocing buffer to %d bytes\n", *size));
		*buffer = erealloc(*buffer, *size);
	}
	if (alignment == ALIGN_RIGHT) {
		if ((neg || always_sign) && padding=='0') {
			(*buffer)[(*pos)++] = (neg) ? '-' : '+';
			add++;
			len--;
			copy_len--;
		}
		while (npad-- > 0) {
			(*buffer)[(*pos)++] = padding;
		}
	}
	PRINTF_DEBUG(("sprintf: appending \"%s\"\n", add));
	memcpy(&(*buffer)[*pos], add, copy_len + 1);
	*pos += copy_len;
	if (alignment == ALIGN_LEFT) {
		while (npad--) {
			(*buffer)[(*pos)++] = padding;
		}
	}
}
/* }}} */

/* php_u_sprintf_appendstring() {{{ */
inline static void
php_u_sprintf_appendstring(UChar **buffer, int *pos, int *size, UChar *add,
						   int min_width, int max_width, UChar padding,
						   int alignment, int len, int neg, int expprec, int always_sign)
{
	register int npad;
	int req_size;
	int copy_len;

	copy_len = (expprec ? MIN(max_width, len) : len);
	npad = min_width - copy_len;

	if (npad < 0) {
		npad = 0;
	}
	
	req_size = *pos + MAX(min_width, copy_len) + 1;

	if (req_size > *size) {
		while (req_size > *size) {
			*size <<= 1;
		}
		*buffer = eurealloc(*buffer, *size);
	}
	if (alignment == ALIGN_RIGHT) {
		if ((neg || always_sign) && padding == 0x30 /* '0' */) {
			(*buffer)[(*pos)++] = (neg) ? 0x2D /* '-' */ : 0x2B /* '+' */;
			add++;
			len--;
			copy_len--;
		}
		while (npad-- > 0) {
			(*buffer)[(*pos)++] = padding;
		}
	}
	u_memcpy(&(*buffer)[*pos], add, copy_len + 1);
	*pos += copy_len;
	if (alignment == ALIGN_LEFT) {
		while (npad--) {
			(*buffer)[(*pos)++] = padding;
		}
	}
}
/* }}} */

/* php_sprintf_appendint() {{{ */ 
inline static void
php_sprintf_appendint(char **buffer, int *pos, int *size, long number, 
						int width, char padding, int alignment, 
						int always_sign)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1, neg = 0;

	PRINTF_DEBUG(("sprintf: appendint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, size, number, width, padding, alignment));
	if (number < 0) {
		neg = 1;
		magn = ((unsigned long) -(number + 1)) + 1;
	} else {
		magn = (unsigned long) number;
	}

	/* Can't right-pad 0's on integers */
	if(alignment==0 && padding=='0') padding=' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	}
	while (magn > 0 && i > 0);
	if (neg) {
		numbuf[--i] = '-';
	} else if (always_sign) {
		numbuf[--i] = '+';
	}
	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n",
				  number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 neg, 0, always_sign);
}
/* }}} */

/* php_u_sprintf_appendint() {{{ */ 
inline static void
php_u_sprintf_appendint(UChar **buffer, int *pos, int *size, long number, 
						int width, UChar padding, int alignment, 
						int always_sign)
{
	UChar numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1, neg = 0;

	if (number < 0) {
		neg = 1;
		magn = ((unsigned long) -(number + 1)) + 1;
	} else {
		magn = (unsigned long) number;
	}

	/* Can't right-pad 0's on integers */
	if (alignment==0 && padding== 0x30 /* '0' */) padding = 0x20 /* ' ' */;

	numbuf[i] = 0x0A /* '\0' */;

	do {
		nmagn = magn / 10;

		numbuf[--i] = (UChar)(magn - (nmagn * 10)) + 0x30 /* '0' */;
		magn = nmagn;
	}
	while (magn > 0 && i > 0);
	if (neg) {
		numbuf[--i] = 0x2D /* '-' */;
	} else if (always_sign) {
		numbuf[--i] = 0x2B /* '+' */;
	}
	php_u_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 neg, 0, always_sign);
}
/* }}} */

/* php_sprintf_appenduint() {{{ */
inline static void
php_sprintf_appenduint(char **buffer, int *pos, int *size,
					   unsigned long number,
					   int width, char padding, int alignment)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1;

	PRINTF_DEBUG(("sprintf: appenduint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, size, number, width, padding, alignment));
	magn = (unsigned long) number;

	/* Can't right-pad 0's on integers */
	if (alignment == 0 && padding == '0') padding = ' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	} while (magn > 0 && i > 0);

	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n", number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i, 0, 0, 0);
}
/* }}} */

/* php_u_sprintf_appenduint() {{{ */
inline static void
php_u_sprintf_appenduint(UChar **buffer, int *pos, int *size,
					   unsigned long number,
					   int width, UChar padding, int alignment)
{
	UChar numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1;

	magn = (unsigned long) number;

	/* Can't right-pad 0's on integers */
	if (alignment == 0 && padding == 0x30 /* '0' */) padding = 0x20 /* ' ' */;

	numbuf[i] = 0x0A /* '\0' */;

	do {
		nmagn = magn / 10;

		numbuf[--i] = (UChar)(magn - (nmagn * 10)) + 0x30 /* '0' */;
		magn = nmagn;
	} while (magn > 0 && i > 0);

	php_u_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i, 0, 0, 0);
}
/* }}} */

/* php_sprintf_appenddouble() {{{ */
inline static void
php_sprintf_appenddouble(char **buffer, int *pos,
						 int *size, double number,
						 int width, char padding,
						 int alignment, int precision,
						 int adjust, char fmt,
						 int always_sign
						 TSRMLS_DC)
{
	char num_buf[NUM_BUF_SIZE];
	char *s = NULL;
	int s_len = 0, is_negative = 0;
#ifdef HAVE_LOCALE_H
	struct lconv *lconv;
#endif

	PRINTF_DEBUG(("sprintf: appenddouble(%x, %x, %x, %f, %d, '%c', %d, %c)\n",
				  *buffer, pos, size, number, width, padding, alignment, fmt));
	if ((adjust & ADJ_PRECISION) == 0) {
		precision = FLOAT_PRECISION;
	} else if (precision > MAX_FLOAT_PRECISION) {
		precision = MAX_FLOAT_PRECISION;
	}
	
	if (zend_isnan(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, size, "NaN", 3, 0, padding,
								 alignment, precision, is_negative, 0, always_sign);
		return;
	}

	if (zend_isinf(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, size, "INF", 3, 0, padding,
								 alignment, precision, is_negative, 0, always_sign);
		return;
	}

	switch (fmt) {		
		case 'e':
		case 'E':
		case 'f':
		case 'F':
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_conv_fp((fmt == 'f')?'F':fmt, number, NO, precision,
							(fmt == 'f')?LCONV_DECIMAL_POINT:'.',
							&is_negative, &num_buf[1], &s_len);
			if (is_negative) {
				num_buf[0] = '-';
				s = num_buf;
				s_len++;
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
				s_len++;
			}
			break;

		case 'g':
		case 'G':
			if (precision == 0)
				precision = 1;
			/*
			 * * We use &num_buf[ 1 ], so that we have room for the sign
			 */
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_gcvt(number, precision, LCONV_DECIMAL_POINT, (fmt == 'G')?'E':'e', &num_buf[1]);
			is_negative = 0;
			if (*s == '-') {
				is_negative = 1;
				s = &num_buf[1];
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
			}

			s_len = strlen(s);
			break;
	}

	php_sprintf_appendstring(buffer, pos, size, s, width, 0, padding,
							 alignment, s_len, is_negative, 0, always_sign);
}
/* }}} */

/* php_u_sprintf_appenddouble() {{{ */
inline static void
php_u_sprintf_appenddouble(UChar **buffer, int *pos,
						 int *size, double number,
						 int width, UChar padding,
						 int alignment, int precision,
						 int adjust, UChar fmt,
						 int always_sign
						 TSRMLS_DC)
{
	char num_buf[NUM_BUF_SIZE];
	char *s = NULL, s_fmt;
	UChar *uni_s;
	int s_len = 0, is_negative = 0;
#ifdef HAVE_LOCALE_H
	struct lconv *lconv;
#endif

	if ((adjust & ADJ_PRECISION) == 0) {
		precision = FLOAT_PRECISION;
	} else if (precision > MAX_FLOAT_PRECISION) {
		precision = MAX_FLOAT_PRECISION;
	}
	
	if (zend_isnan(number)) {
		UChar *nan = USTR_MAKE("NaN");
		is_negative = (number<0);
		php_u_sprintf_appendstring(buffer, pos, size, nan, 3, 0, padding,
								 alignment, precision, is_negative, 0, always_sign);
		efree(nan);
		return;
	}

	if (zend_isinf(number)) {
		UChar *inf = USTR_MAKE("INF");
		is_negative = (number<0);
		php_u_sprintf_appendstring(buffer, pos, size, inf, 3, 0, padding,
								 alignment, precision, is_negative, 0, always_sign);
		efree(inf);
		return;
	}

	switch (fmt) {
		case 0x66 /* 'f' */:
			s_fmt = 'f';
			break;
		case 0x46 /* 'F' */:
			s_fmt = 'F';
			break;
		case 0x65 /* 'e' */:
			s_fmt = 'e';
			break;
		case 0x45 /* 'E' */:
			s_fmt = 'E';
			break;
		case 0x67 /* 'g' */:
			s_fmt = 'g';
			break;
		case 0x47 /* 'G' */:
			s_fmt = 'G';
			break;
	}

	switch (fmt) {		
		case 0x65 /* 'e' */:
		case 0x45 /* 'E' */:
		case 0x46 /* 'F' */:
		case 0x66 /* 'f' */:
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_conv_fp((s_fmt == 'f')?'F':s_fmt, number, NO, precision,
							(s_fmt == 'f')?LCONV_DECIMAL_POINT:'.',
							&is_negative, &num_buf[1], &s_len);
			if (is_negative) {
				num_buf[0] = '-';
				s = num_buf;
				s_len++;
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
				s_len++;
			}
			break;

		case 0x67 /* 'g' */:
		case 0x47 /* 'G' */:
			if (precision == 0)
				precision = 1;
			/*
			 * * We use &num_buf[ 1 ], so that we have room for the sign
			 */
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_gcvt(number, precision, LCONV_DECIMAL_POINT, (s_fmt == 'G')?'E':'e', &num_buf[1]);
			is_negative = 0;
			if (*s == '-') {
				is_negative = 1;
				s = &num_buf[1];
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
			}

			s_len = strlen(s);
			break;
	}

	uni_s = zend_ascii_to_unicode(s, s_len + 1 ZEND_FILE_LINE_CC);
	php_u_sprintf_appendstring(buffer, pos, size, uni_s, width, 0, padding,
							 alignment, s_len, is_negative, 0, always_sign);
	efree(uni_s);
}
/* }}} */

/* php_sprintf_append2n() {{{ */
inline static void
php_sprintf_append2n(char **buffer, int *pos, int *size, long number,
					 int width, char padding, int alignment, int n,
					 char *chartable, int expprec)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long num;
	register unsigned int  i = NUM_BUF_SIZE - 1;
	register int andbits = (1 << n) - 1;

	PRINTF_DEBUG(("sprintf: append2n(%x, %x, %x, %d, %d, '%c', %d, %d, %x)\n",
				  *buffer, pos, size, number, width, padding, alignment, n,
				  chartable));
	PRINTF_DEBUG(("sprintf: append2n 2^%d andbits=%x\n", n, andbits));

	num = (unsigned long) number;
	numbuf[i] = '\0';

	do {
		numbuf[--i] = chartable[(num & andbits)];
		num >>= n;
	}
	while (num > 0);

	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 0, expprec, 0);
}
/* }}} */

/* php_u_sprintf_append2n() {{{ */
inline static void
php_u_sprintf_append2n(UChar **buffer, int *pos, int *size, long number,
					 int width, UChar padding, int alignment, int n,
					 UChar *chartable, int expprec)
{
	UChar numbuf[NUM_BUF_SIZE];
	register unsigned long num;
	register unsigned int  i = NUM_BUF_SIZE - 1;
	register int andbits = (1 << n) - 1;

	num = (unsigned long) number;
	numbuf[i] = '\0';

	do {
		numbuf[--i] = chartable[(num & andbits)];
		num >>= n;
	}
	while (num > 0);

	php_u_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 0, expprec, 0);
}
/* }}} */

/* php_sprintf_getnumber() {{{ */
inline static int
php_sprintf_getnumber(char *buffer, int *pos)
{
	char *endptr;
	register long num = strtol(&buffer[*pos], &endptr, 10);
	register int i = 0;

	if (endptr != NULL) {
		i = (endptr - &buffer[*pos]);
	}
	PRINTF_DEBUG(("sprintf_getnumber: number was %d bytes long\n", i));
	*pos += i;

	if (num >= INT_MAX || num < 0) {
		return -1;
	} else {
		return (int) num;
	}
}
/* }}} */

/* php_u_sprintf_getnumber() {{{ */
inline static long
php_u_sprintf_getnumber(UChar *buffer, int *pos) 
{
	UChar *endptr;
	register long num = zend_u_strtol(&buffer[*pos], &endptr, 10);
	register int i = 0;

	if (endptr != NULL) {
		i = (endptr - &buffer[*pos]);
	}
	*pos += i;
	return num;
}
/* }}} */

/* {{{ php_formatted_print()
 * New sprintf implementation for PHP.
 *
 * Modifiers:
 *
 *  " "   pad integers with spaces
 *  "-"   left adjusted field
 *   n    field size
 *  "."n  precision (floats only)
 *  "+"   Always place a sign (+ or -) in front of a number
 *
 * Type specifiers:
 *
 *  "%"   literal "%", modifiers are ignored.
 *  "b"   integer argument is printed as binary
 *  "c"   integer argument is printed as a single character
 *  "d"   argument is an integer
 *  "f"   the argument is a float
 *  "o"   integer argument is printed as octal
 *  "s"   argument is a string
 *  "x"   integer argument is printed as lowercase hexadecimal
 *  "X"   integer argument is printed as uppercase hexadecimal
 *
 */
static char * php_formatted_print(int ht, int *len, int use_array, int format_offset, int type TSRMLS_DC)
{
	zval ***args, **z_format;
	int argc, size = 240, inpos = 0, outpos = 0, temppos;
	int alignment, currarg, adjusting, argnum, width, precision;
	char *format, *result, padding;
	int always_sign;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return NULL;
	}

	/* verify the number of args */
	if ((use_array && argc != (2 + format_offset)) 
			|| (!use_array && argc < (1 + format_offset))) {
		efree(args);
		WRONG_PARAM_COUNT_WITH_RETVAL(NULL);
	}
	
	if (use_array) {
		int i = 1;
		zval ***newargs;
		zval **array;

		z_format = args[format_offset];
		array = args[1 + format_offset];
		
		SEPARATE_ZVAL(array);
		convert_to_array_ex(array);
		
		argc = 1 + zend_hash_num_elements(Z_ARRVAL_PP(array));
		newargs = (zval ***)safe_emalloc(argc, sizeof(zval *), 0);
		newargs[0] = z_format;
		
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(array));
			 zend_hash_get_current_data(Z_ARRVAL_PP(array), (void **)&newargs[i++]) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_PP(array)));

		efree(args);
		args = newargs;
		format_offset = 0;
	}
	
	convert_to_string_ex(args[format_offset]);
	format = Z_STRVAL_PP(args[format_offset]);
	result = emalloc(size);

	currarg = 1;

	while (inpos<Z_STRLEN_PP(args[format_offset])) {
		int expprec = 0, multiuse = 0;
		zval *tmp;

		PRINTF_DEBUG(("sprintf: format[%d]='%c'\n", inpos, format[inpos]));
		PRINTF_DEBUG(("sprintf: outpos=%d\n", outpos));
		if (format[inpos] != '%') {
			php_sprintf_appendchar(&result, &outpos, &size, format[inpos++] TSRMLS_CC);
		} else if (format[inpos + 1] == '%') {
			php_sprintf_appendchar(&result, &outpos, &size, '%' TSRMLS_CC);
			inpos += 2;
		} else {
			/* starting a new format specifier, reset variables */
			alignment = ALIGN_RIGHT;
			adjusting = 0;
			padding = ' ';
			always_sign = 0;
			inpos++;			/* skip the '%' */

			PRINTF_DEBUG(("sprintf: first looking at '%c', inpos=%d\n",
						  format[inpos], inpos));
			if (isascii((int)format[inpos]) && !isalpha((int)format[inpos])) {
				/* first look for argnum */
				temppos = inpos;
				while (isdigit((int)format[temppos])) temppos++;
				if (format[temppos] == '$') {
					argnum = php_sprintf_getnumber(format, &inpos);

					if (argnum <= 0) {
						efree(result);
						efree(args);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument number must be greater than zero");
						return NULL;
					}

					multiuse = 1;
					inpos++;  /* skip the '$' */
				} else {
					argnum = currarg++;
				}

				argnum += format_offset;

				/* after argnum comes modifiers */
				PRINTF_DEBUG(("sprintf: looking for modifiers\n"
							  "sprintf: now looking at '%c', inpos=%d\n",
							  format[inpos], inpos));
				for (;; inpos++) {
					if (format[inpos] == ' ' || format[inpos] == '0') {
						padding = format[inpos];
					} else if (format[inpos] == '-') {
						alignment = ALIGN_LEFT;
						/* space padding, the default */
					} else if (format[inpos] == '+') {
						always_sign = 1;
					} else if (format[inpos] == '\'') {
						padding = format[++inpos];
					} else {
						PRINTF_DEBUG(("sprintf: end of modifiers\n"));
						break;
					}
				}
				PRINTF_DEBUG(("sprintf: padding='%c'\n", padding));
				PRINTF_DEBUG(("sprintf: alignment=%s\n",
							  (alignment == ALIGN_LEFT) ? "left" : "right"));


				/* after modifiers comes width */
				if (isdigit((int)format[inpos])) {
					PRINTF_DEBUG(("sprintf: getting width\n"));
					if ((width = php_sprintf_getnumber(format, &inpos)) < 0) {
						efree(result);
						efree(args);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Width must be greater than zero and less than %d", INT_MAX);
						return NULL;
					}
					adjusting |= ADJ_WIDTH;
				} else {
					width = 0;
				}
				PRINTF_DEBUG(("sprintf: width=%d\n", width));

				/* after width and argnum comes precision */
				if (format[inpos] == '.') {
					inpos++;
					PRINTF_DEBUG(("sprintf: getting precision\n"));
					if (isdigit((int)format[inpos])) {
						if ((precision = php_sprintf_getnumber(format, &inpos)) < 0) {
							efree(result);
							efree(args);
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Precision must be greater than zero and less than %d", INT_MAX);
							return NULL;
						}
						adjusting |= ADJ_PRECISION;
						expprec = 1;
					} else {
						precision = 0;
					}
				} else {
					precision = 0;
				}
				PRINTF_DEBUG(("sprintf: precision=%d\n", precision));
			} else {
				width = precision = 0;
				argnum = currarg++ + format_offset;
			}

			if (argnum >= argc) {
				efree(result);
				efree(args);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too few arguments");
				return NULL;
			}

			if (format[inpos] == 'l') {
				inpos++;
			}
			PRINTF_DEBUG(("sprintf: format character='%c'\n", format[inpos]));
			/* now we expect to find a type specifier */
			if (multiuse) {
				MAKE_STD_ZVAL(tmp);
				*tmp = **(args[argnum]);
				INIT_PZVAL(tmp);
				zval_copy_ctor(tmp);
			} else {
				SEPARATE_ZVAL(args[argnum]);
				tmp = *(args[argnum]);
			}

			switch (format[inpos]) {
				case 's': {
					zval *var, var_copy;
					int use_copy = 0;

					if (Z_TYPE_P(tmp) != IS_UNICODE) {
						zend_make_string_zval(tmp, &var_copy, &use_copy);
					} else {
						var_copy = *tmp;
						zval_copy_ctor(&var_copy);
						INIT_PZVAL(&var_copy);
						use_copy = 1;

						switch (type) {
							case PHP_OUTPUT:
								convert_to_string_with_converter(&var_copy, ZEND_U_CONVERTER(UG(output_encoding_conv)));
								break;
							case PHP_RUNTIME:
							default:
								convert_to_string_with_converter(&var_copy, ZEND_U_CONVERTER(UG(runtime_encoding_conv)));
								break;
						}
					}
					if (use_copy) {
						var = &var_copy;
					} else {
						var = tmp;
					}
					php_sprintf_appendstring(&result, &outpos, &size,
											 Z_STRVAL_P(var),
											 width, precision, padding,
											 alignment,
											 Z_STRLEN_P(var),
											 0, expprec, 0);
					if (use_copy) {
						zval_dtor(&var_copy);
					}
					break;
				}

				case 'd':
					convert_to_long(tmp);
					php_sprintf_appendint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment,
										  always_sign);
					break;

				case 'u':
					convert_to_long(tmp);
					php_sprintf_appenduint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment);
					break;

				case 'g':
				case 'G':
				case 'e':
				case 'E':
				case 'f':
				case 'F':
					convert_to_double(tmp);
					php_sprintf_appenddouble(&result, &outpos, &size,
											 Z_DVAL_P(tmp),
											 width, padding, alignment,
											 precision, adjusting,
											 format[inpos], always_sign
											 TSRMLS_CC);
					break;
					
				case 'c':
					convert_to_long(tmp);
					php_sprintf_appendchar(&result, &outpos, &size,
										(char) Z_LVAL_P(tmp) TSRMLS_CC);
					break;

				case 'o':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 3,
										 hexchars, expprec);
					break;

				case 'x':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 hexchars, expprec);
					break;

				case 'X':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 HEXCHARS, expprec);
					break;

				case 'b':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 1,
										 hexchars, expprec);
					break;

				case '%':
					php_sprintf_appendchar(&result, &outpos, &size, '%' TSRMLS_CC);

					break;
				default:
					break;
			}
			if (multiuse) {
				zval_ptr_dtor(&tmp);
			}
			inpos++;
		}
	}
	
	efree(args);
	
	/* possibly, we have to make sure we have room for the terminating null? */
	result[outpos]=0;
	*len = outpos;	
	return result;
}
/* }}} */

/* php_u_formatted_print() {{{ */
static zstr php_u_formatted_print(int ht, int *len, int use_array, int format_offset, int type TSRMLS_DC)
{
	zval ***args, **z_format;
	int argc, size = 240, inpos = 0, outpos = 0, temppos;
	int alignment, width, precision, currarg, adjusting, argnum;
	UChar *format, *result, padding;
	int always_sign;
	zstr result_str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return NULL_ZSTR;
	}

	/* verify the number of args */
	if ((use_array && argc != (2 + format_offset)) 
			|| (!use_array && argc < (1 + format_offset))) {
		efree(args);
		WRONG_PARAM_COUNT_WITH_RETVAL(NULL_ZSTR);
	}
	
	if (use_array) {
		int i = 1;
		zval ***newargs;
		zval **array;

		z_format = args[format_offset];
		array = args[1 + format_offset];
		
		SEPARATE_ZVAL(array);
		convert_to_array_ex(array);
		
		argc = 1 + zend_hash_num_elements(Z_ARRVAL_PP(array));
		newargs = (zval ***)safe_emalloc(argc, sizeof(zval *), 0);
		newargs[0] = z_format;
		
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(array));
			 zend_hash_get_current_data(Z_ARRVAL_PP(array), (void **)&newargs[i++]) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_PP(array)));

		efree(args);
		args = newargs;
		format_offset = 0;
	}
	
	convert_to_unicode_ex(args[format_offset]);
	format = Z_USTRVAL_PP(args[format_offset]);
	result = eumalloc(size);

	currarg = 1;

	while (inpos<Z_USTRLEN_PP(args[format_offset])) {
		int expprec = 0, multiuse = 0;
		zval *tmp;

		if (format[inpos] != 0x25 /* '%' */) {
			php_u_sprintf_appendchar(&result, &outpos, &size, format[inpos++] TSRMLS_CC);
		} else if (format[inpos + 1] == 0x25 /* '%' */) {
			php_u_sprintf_appendchar(&result, &outpos, &size, 0x25 /* '%' */ TSRMLS_CC);
			inpos += 2;
		} else {
			/* starting a new format specifier, reset variables */
			alignment = ALIGN_RIGHT;
			adjusting = 0;
			padding = 0x20 /* ' ' */;
			always_sign = 0;
			inpos++;			/* skip the '%' */

			if ((format[inpos] < 0x7f) && !u_isalpha(format[inpos])) {
				/* first look for argnum */
				temppos = inpos;
				while (format[temppos] >= 0x30 /* '0' */ && format[temppos] <= 0x39 /* '9' */) temppos++;
				if (format[temppos] == 0x24 /* '$' */) {
					argnum = php_u_sprintf_getnumber(format, &inpos);

					if (argnum == 0) {
						efree(result);
						efree(args);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Zero is not a valid argument number");
						return NULL_ZSTR;
					}

					multiuse = 1;
					inpos++;  /* skip the '$' */
				} else {
					argnum = currarg++;
				}

				argnum += format_offset;

				/* after argnum comes modifiers */
				for (;; inpos++) {
					if (u_isspace(format[inpos]) || format[inpos] == 0x30 /* '0' */) {
						padding = format[inpos];
					} else if (format[inpos] == 0x2D /* '-' */) {
						alignment = ALIGN_LEFT;
						/* space padding, the default */
					} else if (format[inpos] == 0x2B /* '+' */) {
						always_sign = 1;
					} else if (format[inpos] == 0x27 /* '\'' */) {
						padding = format[++inpos];
					} else {
						break;
					}
				}

				/* after modifiers comes width */
				if (isdigit((int)format[inpos])) {
					width = php_u_sprintf_getnumber(format, &inpos);
					adjusting |= ADJ_WIDTH;
				} else {
					width = 0;
				}

				/* after width and argnum comes precision */
				if (format[inpos] == 0x2E /* '.' */) {
					inpos++;
					if (format[inpos] >= 0x30 /* '0' */ && format[inpos] <= 0x39 /* '9' */) {
						precision = php_u_sprintf_getnumber(format, &inpos);
						adjusting |= ADJ_PRECISION;
						expprec = 1;
					} else {
						precision = 0;
					}
				} else {
					precision = 0;
				}
			} else {
				width = precision = 0;
				argnum = currarg++ + format_offset;
			}

			if (argnum >= argc) {
				efree(result);
				efree(args);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too few arguments");
				return NULL_ZSTR;
			}

			if (format[inpos] == 0x6C /* 'l' */) {
				inpos++;
			}
			/* now we expect to find a type specifier */
			if (multiuse) {
				MAKE_STD_ZVAL(tmp);
				*tmp = **(args[argnum]);
				INIT_PZVAL(tmp);
				zval_copy_ctor(tmp);
			} else {
				SEPARATE_ZVAL(args[argnum]);
				tmp = *(args[argnum]);
			}

			switch (format[inpos]) {
				case 0x73 /* 's' */: {
					zval *var, var_copy;
					int use_copy;

					zend_make_unicode_zval(tmp, &var_copy, &use_copy);
					if (use_copy) {
						var = &var_copy;
					} else {
						var = tmp;
					}
					php_u_sprintf_appendstring(&result, &outpos, &size,
											 Z_USTRVAL_P(var),
											 width, precision, padding,
											 alignment,
											 Z_USTRLEN_P(var),
											 0, expprec, 0);
					if (use_copy) {
						zval_dtor(&var_copy);
					}
					break;
				}

				case 0x64 /* 'd' */:
					convert_to_long(tmp);
					php_u_sprintf_appendint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment,
										  always_sign);
					break;

				case 0x75 /* 'u' */:
					convert_to_long(tmp);
					php_u_sprintf_appenduint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment);
					break;

				case 0x67 /* 'g' */:
				case 0x47 /* 'G' */:
				case 0x65 /* 'e' */:
				case 0x45 /* 'E' */:
				case 0x66 /* 'f' */:
				case 0x46 /* 'F' */:
					convert_to_double(tmp);
					php_u_sprintf_appenddouble(&result, &outpos, &size,
											 Z_DVAL_P(tmp),
											 width, padding, alignment,
											 precision, adjusting,
											 format[inpos], always_sign
											 TSRMLS_CC);
					break;
					
				case 0x63 /* 'c' */:
					convert_to_long(tmp);
					php_u_sprintf_appendchar(&result, &outpos, &size,
										(char) Z_LVAL_P(tmp) TSRMLS_CC);
					break;

				case 0x6F /* 'o' */:
					convert_to_long(tmp);
					php_u_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 3,
										 u_hexchars, expprec);
					break;

				case 0x78 /* 'x' */:
					convert_to_long(tmp);
					php_u_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 u_hexchars, expprec);
					break;

				case 0x58 /* 'X' */:
					convert_to_long(tmp);
					php_u_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 u_HEXCHARS, expprec);
					break;

				case 0x62 /* 'b' */:
					convert_to_long(tmp);
					php_u_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 1,
										 u_hexchars, expprec);
					break;

				case 0x25 /* '%' */:
					php_u_sprintf_appendchar(&result, &outpos, &size, 0x25 /* '%' */ TSRMLS_CC);

					break;
				default:
					break;
			}
			if (multiuse) {
				zval_ptr_dtor(&tmp);
			}
			inpos++;
		}
	}
	
	efree(args);
	
	/* possibly, we have to make sure we have room for the terminating null? */
	result[outpos] = 0;
	*len = outpos;	
	result_str.u = result;

	switch (type) {
		case PHP_OUTPUT: 
		{
			UErrorCode status = U_ZERO_ERROR;
			char *s;
			int s_len;

			zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(output_encoding_conv)), &s, &s_len, result, outpos, &status);
			if(U_FAILURE(status)) {
				efree(s);
				efree(result);
				return NULL_ZSTR;
			}

			efree(result_str.v);
			result_str.s = s;
			*len = s_len;
			break;
		}
		case PHP_RUNTIME:
		default:
			/* nothing to be done */
			break;
	}

	return result_str;
}
/* }}} */

/* {{{ proto string sprintf(string format [, mixed arg1 [, mixed ...]]) U
   Return a formatted string */
PHP_FUNCTION(user_sprintf)
{
	int len;
	zstr result;

	if (!UG(unicode)) {
		if ((result.s = php_formatted_print(ht, &len, 0, 0, PHP_RUNTIME TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL(result.s, len, 0);
	} else {
		result = php_u_formatted_print(ht, &len, 0, 0, PHP_RUNTIME TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
		RETVAL_UNICODEL(result.u, len, 0);
	}
}
/* }}} */

/* {{{ proto string vsprintf(string format, array args) U
   Return a formatted string */
PHP_FUNCTION(vsprintf)
{
	int len;
	zstr result;

	if (!UG(unicode)) {
		if ((result.s = php_formatted_print(ht, &len, 1, 0, PHP_RUNTIME TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL(result.s, len, 0);
	} else {
		result = php_u_formatted_print(ht, &len, 1, 0, PHP_RUNTIME TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
		RETVAL_UNICODEL(result.u, len, 0);
	}
}
/* }}} */

/* {{{ proto int printf(string format [, mixed arg1 [, mixed ...]]) U
   Output a formatted string */
PHP_FUNCTION(user_printf)
{
	int len;
	zstr result;

	if (!UG(unicode)) {
		if ((result.s = php_formatted_print(ht, &len, 0, 0, PHP_OUTPUT TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
	} else {
		result = php_u_formatted_print(ht, &len, 0, 0, PHP_OUTPUT TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
	}
	
	PHPWRITE(result.s, len);
	efree(result.v);
	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto int vprintf(string format, array args) U
   Output a formatted string */
PHP_FUNCTION(vprintf)
{
	int len;
	zstr result;
	
	if (!UG(unicode)) {
		if ((result.s = php_formatted_print(ht, &len, 1, 0, PHP_OUTPUT TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
	} else {
		result = php_u_formatted_print(ht, &len, 1, 0, PHP_OUTPUT TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
	}

	PHPWRITE(result.s, len);
	efree(result.v);
	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto int fprintf(resource stream, string format [, mixed arg1 [, mixed ...]]) U
   Output a formatted string into a stream */
PHP_FUNCTION(fprintf)
{
	php_stream *stream;
	zval *arg1, **arg2;
	zstr result;
	int len, ret;
	
	if (ZEND_NUM_ARGS() < 2) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(2 TSRMLS_CC, "rZ", &arg1, &arg2) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &arg1);

	if (Z_TYPE_PP(arg2) != IS_STRING && Z_TYPE_PP(arg2) != IS_UNICODE) {
		convert_to_text_ex(arg2);
	}

	if (Z_TYPE_PP(arg2) == IS_STRING) {
		if ((result.s = php_formatted_print(ht, &len, 0, 1, PHP_RUNTIME TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
		ret = php_stream_write(stream, result.s, len);
	} else {
		result = php_u_formatted_print(ht, &len, 0, 1, PHP_RUNTIME TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
		ret = php_stream_write_unicode(stream, result.u, len);
	}
	
	efree(result.v);

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int vfprintf(resource stream, string format, array args) U
   Output a formatted string into a stream */
PHP_FUNCTION(vfprintf)
{
	php_stream *stream;
	zval *arg1, **arg2;
	zstr result;
	int len, ret;
	
	if (ZEND_NUM_ARGS() != 3) {
		WRONG_PARAM_COUNT;
	}
	
	if (zend_parse_parameters(2 TSRMLS_CC, "rZ", &arg1, &arg2) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &arg1);

	if (Z_TYPE_PP(arg2) != IS_STRING && Z_TYPE_PP(arg2) != IS_UNICODE) {
		convert_to_text_ex(arg2);
	}

	if (Z_TYPE_PP(arg2) == IS_STRING) {
		if ((result.s = php_formatted_print(ht, &len, 1, 1, PHP_RUNTIME TSRMLS_CC))==NULL) {
			RETURN_FALSE;
		}
		ret = php_stream_write(stream, result.s, len);
	} else {
		result = php_u_formatted_print(ht, &len, 1, 1, PHP_RUNTIME TSRMLS_CC);
		if (result.v == NULL) {
			RETURN_FALSE;
		}
		ret = php_stream_write_unicode(stream, result.u, len);
	}

	efree(result.v);

	RETURN_LONG(ret);
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
