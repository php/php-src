/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#include "php.h"
#include "snprintf.h"

#define FALSE           0
#define TRUE            1
#define NUL             '\0'
#define INT_NULL        ((int *)0)

#define S_NULL          "(null)"
#define S_NULL_LEN      6

#define FLOAT_DIGITS    6
#define EXPONENT_LENGTH 10

/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 */
#define NUM_BUF_SIZE    512


/*
 * Size for realloc operations
 */
#define SPPRINTF_BLOCK_SIZE 128

/*
 * Descriptor for buffer area
 */
struct xbuf_area {
	char    *buf;      /* pointer to buffer */
	size_t  size;
	size_t  max_len;
	char    *buf_end;  /* pointer to buffer end or ~0 */
	char    *nextb;    /* pointer to next byte to read/write   */
};

typedef struct xbuf_area xbuffy;

/* Resize xbuf so that add bytes can be added. Reallocation is done
 * in defined block size to minimize calls to realloc.
 */
static void xbuf_resize(xbuffy *xbuf, size_t add) 
{
	char *buf;
	size_t size, offset;

	if (xbuf->buf) {
		offset = xbuf->nextb - xbuf->buf;
		if (offset+add < xbuf->size) {
			return; /* do not change size if not necessary */
		}
	} else {
		offset = 0;
	}
	if (add<SPPRINTF_BLOCK_SIZE) {
		size = xbuf->size + SPPRINTF_BLOCK_SIZE;
	} else {
		size = xbuf->size + add;
	}
	if (xbuf->max_len && size > xbuf->max_len) {
		size = xbuf->max_len;
	}

	buf = erealloc(xbuf->buf, size+1); /* alloc space for NUL */
	
	if (buf) {
		xbuf->buf = buf;
		xbuf->buf_end = xbuf->max_len ? &buf[size] : (char *) ~0;
		xbuf->nextb = buf+offset;
		xbuf->size = size;
	}
}

/* Initialise xbuffy with size spprintf_BLOCK_SIZE
 */
static char * xbuf_init(xbuffy *xbuf, size_t max_len) 
{
	xbuf->buf = NULL;
	xbuf->size = 0;
	xbuf->max_len = max_len;
	xbuf_resize(xbuf, 0); /* NOT max_len */
	return xbuf->buf;
}

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
#define INS_CHAR_NR(xbuf, ch, cc)       \
	if (xbuf->nextb < xbuf->buf_end) {  \
		*(xbuf->nextb++) = ch;          \
		cc++;                           \
	}

#define INS_CHAR(xbuf, ch, cc)          \
	xbuf_resize(xbuf, 1);               \
	INS_CHAR_NR(xbuf, ch, cc)

/*
 * Macro that does padding. The padding is done by printing
 * the character ch.
 */
#define PAD(xbuf, width, len, ch)       \
	if (width > len) {                  \
		xbuf_resize(xbuf, width-len);   \
		do {                            \
			INS_CHAR_NR(xbuf, ch, cc);  \
			width--;                    \
		}                               \
		while (width > len);\
	}

#define NUM(c) (c - '0')

#define STR_TO_DEC(str, num)            \
	num = NUM(*str++);                  \
	while (isdigit((int)*str)) {        \
		num *= 10;                      \
		num += NUM(*str++);             \
    }

/*
 * This macro does zero padding so that the precision
 * requirement is satisfied. The padding is done by
 * adding '0's to the left of the string that is going
 * to be printed.
 */
#define FIX_PRECISION(adjust, precision, s, s_len)  \
    if (adjust)					                    \
		while (s_len < precision) {                 \
			*--s = '0';                             \
			s_len++;                                \
		}

/*
 * Prefix the character ch to the string str
 * Increase length
 * Set the has_prefix flag
 */
#define PREFIX(str, length, ch) \
	*--str = ch;                \
	length++;                   \
	has_prefix = YES



/*
 * Do format conversion placing the output in buffer
 */
static int xbuf_format_converter(register xbuffy * xbuf, const char *fmt, va_list ap)
{
	register int cc = 0;
	register int i;

	register char *s = NULL;
	char *q;
	int s_len;

	register int min_width = 0;
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

	/*
	 * Flag variables
	 */
	boolean_e is_long;
	boolean_e alternate_form;
	boolean_e print_sign;
	boolean_e print_blank;
	boolean_e adjust_precision;
	boolean_e adjust_width;
	bool_int is_negative;

	while (*fmt) {
		if (*fmt != '%') {
			INS_CHAR(xbuf, *fmt, cc);
		} else {
			/*
			 * Default variable settings
			 */
			adjust = RIGHT;
			alternate_form = print_sign = print_blank = NO;
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
				 *
				 * XXX: an unreasonable amount of precision may be specified
				 * resulting in overflow of num_buf. Currently we
				 * ignore this possibility.
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
				} else
					adjust_precision = NO;
			} else
				adjust_precision = adjust_width = NO;

			/*
			 * Modifier check
			 */
			if (*fmt == 'l') {
				is_long = YES;
				fmt++;
			} else
				is_long = NO;

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
				case 'u':
					if (is_long)
						i_num = va_arg(ap, u_wide_int);
					else
						i_num = (wide_int) va_arg(ap, unsigned int);
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
						if (is_long)
							i_num = va_arg(ap, wide_int);
						else
							i_num = (wide_int) va_arg(ap, int);
					};
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
					if (is_long)
						ui_num = va_arg(ap, u_wide_int);
					else
						ui_num = (u_wide_int) va_arg(ap, unsigned int);
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
					if (is_long)
						ui_num = (u_wide_int) va_arg(ap, u_wide_int);
					else
						ui_num = (u_wide_int) va_arg(ap, unsigned int);
					s = ap_php_conv_p2(ui_num, 4, *fmt,
								&num_buf[NUM_BUF_SIZE], &s_len);
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
						if (adjust_precision && precision < s_len)
							s_len = precision;
					} else {
						s = S_NULL;
						s_len = S_NULL_LEN;
					}
					pad_char = ' ';
					break;


				case 'f':
				case 'e':
				case 'E':
					fp_num = va_arg(ap, double);

					if (zend_isnan(fp_num)) {
						s = "nan";
						s_len = 3;
					} else if (zend_isinf(fp_num)) {
						s = "inf";
						s_len = 3;
					} else {
						s = ap_php_conv_fp(*fmt, fp_num, alternate_form,
									(adjust_precision == NO) ? FLOAT_DIGITS : precision,
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
				case 'G':
					if (adjust_precision == NO)
						precision = FLOAT_DIGITS;
					else if (precision == 0)
						precision = 1;
					/*
					 * * We use &num_buf[ 1 ], so that we have room for the sign
					 */
					s = ap_php_gcvt(va_arg(ap, double), precision, &num_buf[1],
							alternate_form);
					if (*s == '-')
						prefix_char = *s++;
					else if (print_sign)
						prefix_char = '+';
					else if (print_blank)
						prefix_char = ' ';

					s_len = strlen(s);

					if (alternate_form && (q = strchr(s, '.')) == NULL)
						s[s_len++] = '.';
					if (*fmt == 'G' && (q = strchr(s, 'e')) != NULL)
						*q = 'E';
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
					break;

					/*
					 * Always extract the argument as a "char *" pointer. We 
					 * should be using "void *" but there are still machines 
					 * that don't understand it.
					 * If the pointer size is equal to the size of an unsigned
					 * integer we convert the pointer to a hex number, otherwise 
					 * we print "%p" to indicate that we don't handle "%p".
					 */
				case 'p':
					ui_num = (u_wide_int) va_arg(ap, char *);

					if (sizeof(char *) <= sizeof(u_wide_int))
						s = ap_php_conv_p2(ui_num, 4, 'x', &num_buf[NUM_BUF_SIZE], &s_len);
					else {
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
					INS_CHAR(xbuf, *s, cc)
						s++;
					s_len--;
					min_width--;
				}
				PAD(xbuf, min_width, s_len, pad_char);
			}
			/*
			 * Print the string s. 
			 */
			xbuf_resize(xbuf, s_len);
			for (i = s_len; i != 0; i--) {
				INS_CHAR_NR(xbuf, *s, cc);
				s++;
			}

			if (adjust_width && adjust == LEFT && min_width > s_len)
				PAD(xbuf, min_width, s_len, pad_char);
		}
		fmt++;
	}
	return (cc);
}


/*
 * This is the general purpose conversion function.
 */
PHPAPI int vspprintf(char **pbuf, size_t max_len, const char *format, va_list ap)
{
	xbuffy xbuf;
	int cc;

	assert(pbuf != NULL);
	/*
	 * First initialize the descriptor
	 * Notice that if no length is given, we initialize buf_end to the
	 * highest possible address.
	 */
	if (!xbuf_init(&xbuf, max_len)) {
		if (pbuf)
			*pbuf = NULL;
		return 0;
	} else {
		/*
		 * Do the conversion
		 */
		cc = xbuf_format_converter(&xbuf, format, ap);
		if (xbuf.nextb <= xbuf.buf_end)
			*(xbuf.nextb) = '\0';
		else if (xbuf.size)
			xbuf.buf[xbuf.size-1] = '\0';
		if (pbuf)
			*pbuf = xbuf.buf;
		else
			efree(pbuf);
		return cc;
	}
}


PHPAPI int spprintf(char **pbuf, size_t max_len, const char *format, ...)
{
	int cc;
	va_list ap;

	va_start(ap, format);
	cc = vspprintf(pbuf, max_len, format, ap);
	va_end(ap);
	return (cc);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
