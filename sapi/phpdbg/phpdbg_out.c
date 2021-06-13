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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "php.h"
#include "spprintf.h"
#include "phpdbg.h"
#include "phpdbg_io.h"
#include "ext/standard/html.h"

#ifdef _WIN32
#	include "win32/time.h"
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

/* copied from php-src/main/snprintf.c and slightly modified */
/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 * Emax = 1023
 * NDIG = 320
 * NUM_BUF_SIZE >= strlen("-") + Emax + strlen(".") + NDIG + strlen("E+1023") + 1;
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

#include <locale.h>
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#define NUL '\0'
#define S_NULL "(null)"
#define S_NULL_LEN 6
#define FLOAT_DIGITS 6

/*
 * Do format conversion placing the output in buffer
 */
static int format_converter(register buffy *odp, const char *fmt, va_list ap) {
	char *sp;
	char *bep;
	int cc = 0;
	int i;

	char *s = NULL, *free_s = NULL;
	size_t s_len;

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

	struct lconv *lconv = NULL;

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
			zend_string *tmp_str = NULL;
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
					zval *zvp = va_arg(ap, zval *);
					zend_string *str = zval_get_tmp_string(zvp, &tmp_str);
					s_len = ZSTR_LEN(str);
					s = ZSTR_VAL(str);
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
					} else {
						s = S_NULL;
						s_len = S_NULL_LEN;
					}
					pad_char = ' ';
					break;


				case 'b':
				    s = PHPDBG_G(err_buf).msg;
					if (s != NULL) {
						s_len = PHPDBG_G(err_buf).msglen;

						if (adjust_precision && precision != s_len) {
							s_len = precision;
						}
					} else {
						s = "";
						s_len = 0;
					}
					pad_char = ' ';
					break;


				case 'r':
					if (PHPDBG_G(req_id)) {
						s_len = spprintf(&s, 0, "req=\"" ZEND_ULONG_FMT "\"", PHPDBG_G(req_id));
						free_s = s;
					} else {
						s = "";
						s_len = 0;
					}
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
						if (!lconv) {
							lconv = localeconv();
						}

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
					if (!lconv) {
						lconv = localeconv();
					}

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
			zend_tmp_string_release(tmp_str);
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

static void strx_printv(int *ccp, char *buf, size_t len, const char *format, va_list ap) {
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
	cc = format_converter(&od, format, ap);
	if (len != 0 && od.nextb <= od.buf_end) {
		*(od.nextb) = '\0';
	}
	if (ccp) {
		*ccp = cc;
	}
}

static int phpdbg_vsnprintf(char *buf, size_t len, const char *format, va_list ap) {
	int cc;

	strx_printv(&cc, buf, len, format, ap);
	return (cc);
}

PHPDBG_API int phpdbg_vasprintf(char **buf, const char *format, va_list ap) {
	va_list ap2;
	int cc;

	va_copy(ap2, ap);
	cc = phpdbg_vsnprintf(NULL, 0, format, ap2);
	va_end(ap2);

	*buf = NULL;

	if (cc >= 0) {
		*buf = emalloc(++cc);
		if ((cc = phpdbg_vsnprintf(*buf, cc, format, ap)) < 0) {
			efree(*buf);
			*buf = NULL;
		}
	}

	return cc;
}
/* copy end */

PHPDBG_API int _phpdbg_asprintf(char **buf, const char *format, ...) {
	int ret;
	va_list va;

	va_start(va, format);
	ret = phpdbg_vasprintf(buf, format, va);
	va_end(va);

	return ret;
}

static int phpdbg_process_print(int fd, int type, const char *msg, int msglen) {
	char *msgout = NULL;
	int msgoutlen;

	switch (type) {
		case P_ERROR:
			if (!PHPDBG_G(last_was_newline)) {
				phpdbg_mixed_write(fd, ZEND_STRL("\n"));
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = phpdbg_asprintf(&msgout, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_ERROR]->code, msglen, msg);
			} else {
				msgoutlen = phpdbg_asprintf(&msgout, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_NOTICE:
			if (!PHPDBG_G(last_was_newline)) {
				phpdbg_mixed_write(fd, ZEND_STRL("\n"));
				PHPDBG_G(last_was_newline) = 1;
			}
			if (PHPDBG_G(flags) & PHPDBG_IS_COLOURED) {
				msgoutlen = phpdbg_asprintf(&msgout, "\033[%sm[%.*s]\033[0m\n", PHPDBG_G(colors)[PHPDBG_COLOR_NOTICE]->code, msglen, msg);
			} else {
				msgoutlen = phpdbg_asprintf(&msgout, "[%.*s]\n", msglen, msg);
			}
			break;

		case P_WRITELN:
			if (msg) {
				msgoutlen = phpdbg_asprintf(&msgout, "%.*s\n", msglen, msg);
			} else {
				msgoutlen = 1;
				msgout = estrdup("\n");
			}
			PHPDBG_G(last_was_newline) = 1;
			break;

		case P_WRITE:
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
				phpdbg_mixed_write(fd, msg, msglen);
			}
			return msglen;

		/* no formatting on logging output */
		case P_LOG:
			if (msg) {
				struct timeval tp;
				if (gettimeofday(&tp, NULL) == SUCCESS) {
					msgoutlen = phpdbg_asprintf(&msgout, "[%ld %.8F]: %.*s\n", tp.tv_sec, tp.tv_usec / 1000000., msglen, msg);
				} else {
					msgoutlen = FAILURE;
				}
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	if (msgout) {
		phpdbg_mixed_write(fd, msgout, msgoutlen);
	}

	if (msgout) {
		efree(msgout);
	}

	return msgout ? msgoutlen : 0;
} /* }}} */

PHPDBG_API int phpdbg_vprint(int type, int fd, const char *strfmt, va_list args) {
	char *msg = NULL;
	int msglen = 0;
	int len;
	va_list argcpy;

	if (strfmt != NULL && strlen(strfmt) > 0L) {
		va_copy(argcpy, args);
		msglen = phpdbg_vasprintf(&msg, strfmt, argcpy);
		va_end(argcpy);
	}

	if (PHPDBG_G(err_buf).active && type != P_STDOUT && type != P_STDERR) {
		phpdbg_free_err_buf();

		PHPDBG_G(err_buf).type = type;
		PHPDBG_G(err_buf).fd = fd;
		PHPDBG_G(err_buf).msg = msg;
		PHPDBG_G(err_buf).msglen = msglen;

		return msglen;
	}

	len = phpdbg_process_print(fd, type, msg, msglen);

	if (msg) {
		efree(msg);
	}

	return len;
}

PHPDBG_API void phpdbg_free_err_buf(void) {
	if (PHPDBG_G(err_buf).type == 0) {
		return;
	}

	efree(PHPDBG_G(err_buf).msg);

	PHPDBG_G(err_buf).type = 0;
}

PHPDBG_API void phpdbg_activate_err_buf(bool active) {
	PHPDBG_G(err_buf).active = active;
}

PHPDBG_API int phpdbg_output_err_buf(const char *strfmt, ...) {
	int len;
	va_list args;
	int errbuf_active = PHPDBG_G(err_buf).active;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	PHPDBG_G(err_buf).active = 0;

	va_start(args, strfmt);
	len = phpdbg_vprint(PHPDBG_G(err_buf).type, PHPDBG_G(err_buf).fd, strfmt, args);
	va_end(args);

	PHPDBG_G(err_buf).active = errbuf_active;
	phpdbg_free_err_buf();

	return len;
}

PHPDBG_API int phpdbg_print(int type, int fd, const char *strfmt, ...) {
	va_list args;
	int len;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	va_start(args, strfmt);
	len = phpdbg_vprint(type, fd, strfmt, args);
	va_end(args);

	return len;
}

PHPDBG_API int phpdbg_log_internal(int fd, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	va_start(args, fmt);
	buflen = phpdbg_vasprintf(&buffer, fmt, args);
	va_end(args);

	len = phpdbg_mixed_write(fd, buffer, buflen);
	efree(buffer);

	return len;
}

PHPDBG_API int phpdbg_out_internal(int fd, const char *fmt, ...) {
	va_list args;
	char *buffer;
	int buflen;
	int len = 0;

	if (PHPDBG_G(flags) & PHPDBG_DISCARD_OUTPUT) {
		return 0;
	}

	va_start(args, fmt);
	buflen = phpdbg_vasprintf(&buffer, fmt, args);
	va_end(args);

	len = phpdbg_mixed_write(fd, buffer, buflen);

	efree(buffer);
	return len;
}
