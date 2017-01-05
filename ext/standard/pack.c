/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Chris Schneider <cschneid@relog.ch>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PHP_WIN32
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#elif defined(NETWARE)
#ifdef USE_WINSOCK
#include <novsock2.h>
#else
#include <sys/socket.h>
#endif
#include <sys/param.h>
#else
#include <sys/param.h>
#endif
#include "ext/standard/head.h"
#include "php_string.h"
#include "pack.h"
#if HAVE_PWD_H
#ifdef PHP_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#include "fsock.h"
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#define INC_OUTPUTPOS(a,b) \
	if ((a) < 0 || ((INT_MAX - outputpos)/((int)b)) < (a)) { \
		efree(formatcodes);	\
		efree(formatargs);	\
		php_error_docref(NULL, E_WARNING, "Type %c: integer overflow in format string", code); \
		RETURN_FALSE; \
	} \
	outputpos += (a)*(b);

/* Whether machine is little endian */
char machine_little_endian;

/* Mapping of byte from char (8bit) to long for machine endian */
static int byte_map[1];

/* Mappings of bytes from int (machine dependent) to int for machine endian */
static int int_map[sizeof(int)];

/* Mappings of bytes from shorts (16bit) for all endian environments */
static int machine_endian_short_map[2];
static int big_endian_short_map[2];
static int little_endian_short_map[2];

/* Mappings of bytes from longs (32bit) for all endian environments */
static int machine_endian_long_map[4];
static int big_endian_long_map[4];
static int little_endian_long_map[4];

#if SIZEOF_ZEND_LONG > 4
/* Mappings of bytes from quads (64bit) for all endian environments */
static int machine_endian_longlong_map[8];
static int big_endian_longlong_map[8];
static int little_endian_longlong_map[8];
#endif

/* {{{ php_pack
 */
static void php_pack(zval *val, size_t size, int *map, char *output)
{
	int i;
	char *v;

	convert_to_long_ex(val);
	v = (char *) &Z_LVAL_P(val);

	for (i = 0; i < size; i++) {
		*output++ = v[map[i]];
	}
}
/* }}} */

/* {{{ php_pack_reverse_int32
 */
static inline uint32_t php_pack_reverse_int32(uint32_t arg)
{
    uint32_t result;
    result = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);

	return result;
}
/* }}} */

/* {{{ php_pack
 */
static inline uint64_t php_pack_reverse_int64(uint64_t arg)
{
	union Swap64 {
		uint64_t i;
		uint32_t ul[2];
	} tmp, result;
	tmp.i = arg;
	result.ul[0] = php_pack_reverse_int32(tmp.ul[1]);
	result.ul[1] = php_pack_reverse_int32(tmp.ul[0]);

	return result.i;
}
/* }}} */

/* {{{ php_pack_copy_float
 */
static void php_pack_copy_float(int is_little_endian, void * dst, float f)
{
	union Copy32 {
		float f;
		uint32_t i;
	} m;
	m.f = f;

#ifdef WORDS_BIGENDIAN
	if (is_little_endian) {
		m.i = php_pack_reverse_int32(m.i);
	}
#else /* WORDS_BIGENDIAN */
	if (!is_little_endian) {
		m.i = php_pack_reverse_int32(m.i);
	}
#endif /* WORDS_BIGENDIAN */

	memcpy(dst, &m.f, sizeof(float));
}
/* }}} */

/* {{{ php_pack_copy_double
 */
static void php_pack_copy_double(int is_little_endian, void * dst, double d)
{
	union Copy64 {
		double d;
		uint64_t i;
	} m;
	m.d = d;

#ifdef WORDS_BIGENDIAN
	if (is_little_endian) {
		m.i = php_pack_reverse_int64(m.i);
	}
#else /* WORDS_BIGENDIAN */
	if (!is_little_endian) {
		m.i = php_pack_reverse_int64(m.i);
	}
#endif /* WORDS_BIGENDIAN */

	memcpy(dst, &m.d, sizeof(double));
}
/* }}} */

/* {{{ php_pack_parse_float
 */
static float php_pack_parse_float(int is_little_endian, void * src)
{
	union Copy32 {
		float f;
		uint32_t i;
	} m;
	memcpy(&m.i, src, sizeof(float));

#ifdef WORDS_BIGENDIAN
	if (is_little_endian) {
		m.i = php_pack_reverse_int32(m.i);
	}
#else /* WORDS_BIGENDIAN */
	if (!is_little_endian) {
		m.i = php_pack_reverse_int32(m.i);
	}
#endif /* WORDS_BIGENDIAN */

	return m.f;
}
/* }}} */

/* {{{ php_pack_parse_double
 */
static double php_pack_parse_double(int is_little_endian, void * src)
{
	union Copy64 {
		double d;
		uint64_t i;
	} m;
	memcpy(&m.i, src, sizeof(double));

#ifdef WORDS_BIGENDIAN
	if (is_little_endian) {
		m.i = php_pack_reverse_int64(m.i);
	}
#else /* WORDS_BIGENDIAN */
	if (!is_little_endian) {
		m.i = php_pack_reverse_int64(m.i);
	}
#endif /* WORDS_BIGENDIAN */

	return m.d;
}
/* }}} */

/* pack() idea stolen from Perl (implemented formats behave the same as there except J and P)
 * Implemented formats are Z, A, a, h, H, c, C, s, S, i, I, l, L, n, N, q, Q, J, P, f, d, x, X, @.
 * Added g, G for little endian float and big endian float, added e, E for little endian double and big endian double.
 */
/* {{{ proto string pack(string format, mixed arg1 [, mixed arg2 [, mixed ...]])
   Takes one or more arguments and packs them into a binary string according to the format argument */
PHP_FUNCTION(pack)
{
	zval *argv = NULL;
	int num_args = 0, i;
	int currentarg;
	char *format;
	size_t formatlen;
	char *formatcodes;
	int *formatargs;
	int formatcount = 0;
	int outputpos = 0, outputsize = 0;
	zend_string *output;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s*", &format, &formatlen, &argv, &num_args) == FAILURE) {
		return;
	}

	/* We have a maximum of <formatlen> format codes to deal with */
	formatcodes = safe_emalloc(formatlen, sizeof(*formatcodes), 0);
	formatargs = safe_emalloc(formatlen, sizeof(*formatargs), 0);
	currentarg = 0;

	/* Preprocess format into formatcodes and formatargs */
	for (i = 0; i < formatlen; formatcount++) {
		char code = format[i++];
		int arg = 1;

		/* Handle format arguments if any */
		if (i < formatlen) {
			char c = format[i];

			if (c == '*') {
				arg = -1;
				i++;
			}
			else if (c >= '0' && c <= '9') {
				arg = atoi(&format[i]);

				while (format[i] >= '0' && format[i] <= '9' && i < formatlen) {
					i++;
				}
			}
		}

		/* Handle special arg '*' for all codes and check argv overflows */
		switch ((int) code) {
			/* Never uses any args */
			case 'x':
			case 'X':
			case '@':
				if (arg < 0) {
					php_error_docref(NULL, E_WARNING, "Type %c: '*' ignored", code);
					arg = 1;
				}
				break;

			/* Always uses one arg */
			case 'a':
			case 'A':
			case 'Z':
			case 'h':
			case 'H':
				if (currentarg >= num_args) {
					efree(formatcodes);
					efree(formatargs);
					php_error_docref(NULL, E_WARNING, "Type %c: not enough arguments", code);
					RETURN_FALSE;
				}

				if (arg < 0) {
					convert_to_string(&argv[currentarg]);
					arg = Z_STRLEN(argv[currentarg]);
					if (code == 'Z') {
						/* add one because Z is always NUL-terminated:
						 * pack("Z*", "aa") === "aa\0"
						 * pack("Z2", "aa") === "a\0" */
						arg++;
					}
				}

				currentarg++;
				break;

			/* Use as many args as specified */
			case 'q':
			case 'Q':
			case 'J':
			case 'P':
#if SIZEOF_ZEND_LONG < 8
					efree(formatcodes);
					efree(formatargs);
					php_error_docref(NULL, E_WARNING, "64-bit format codes are not available for 32-bit versions of PHP");
					RETURN_FALSE;
#endif
			case 'c':
			case 'C':
			case 's':
			case 'S':
			case 'i':
			case 'I':
			case 'l':
			case 'L':
			case 'n':
			case 'N':
			case 'v':
			case 'V':
			case 'f': /* float */
			case 'g': /* little endian float */
			case 'G': /* big endian float */
			case 'd': /* double */
			case 'e': /* little endian double */
			case 'E': /* big endian double */
				if (arg < 0) {
					arg = num_args - currentarg;
				}

				currentarg += arg;

				if (currentarg > num_args) {
					efree(formatcodes);
					efree(formatargs);
					php_error_docref(NULL, E_WARNING, "Type %c: too few arguments", code);
					RETURN_FALSE;
				}
				break;

			default:
				efree(formatcodes);
				efree(formatargs);
				php_error_docref(NULL, E_WARNING, "Type %c: unknown format code", code);
				RETURN_FALSE;
		}

		formatcodes[formatcount] = code;
		formatargs[formatcount] = arg;
	}

	if (currentarg < num_args) {
		php_error_docref(NULL, E_WARNING, "%d arguments unused", (num_args - currentarg));
	}

	/* Calculate output length and upper bound while processing*/
	for (i = 0; i < formatcount; i++) {
	    int code = (int) formatcodes[i];
		int arg = formatargs[i];

		switch ((int) code) {
			case 'h':
			case 'H':
				INC_OUTPUTPOS((arg + (arg % 2)) / 2,1)	/* 4 bit per arg */
				break;

			case 'a':
			case 'A':
			case 'Z':
			case 'c':
			case 'C':
			case 'x':
				INC_OUTPUTPOS(arg,1)		/* 8 bit per arg */
				break;

			case 's':
			case 'S':
			case 'n':
			case 'v':
				INC_OUTPUTPOS(arg,2)		/* 16 bit per arg */
				break;

			case 'i':
			case 'I':
				INC_OUTPUTPOS(arg,sizeof(int))
				break;

			case 'l':
			case 'L':
			case 'N':
			case 'V':
				INC_OUTPUTPOS(arg,4)		/* 32 bit per arg */
				break;

#if SIZEOF_ZEND_LONG > 4
			case 'q':
			case 'Q':
			case 'J':
			case 'P':
				INC_OUTPUTPOS(arg,8)		/* 32 bit per arg */
				break;
#endif

			case 'f': /* float */
			case 'g': /* little endian float */
			case 'G': /* big endian float */
				INC_OUTPUTPOS(arg,sizeof(float))
				break;

			case 'd': /* double */
			case 'e': /* little endian double */
			case 'E': /* big endian double */
				INC_OUTPUTPOS(arg,sizeof(double))
				break;

			case 'X':
				outputpos -= arg;

				if (outputpos < 0) {
					php_error_docref(NULL, E_WARNING, "Type %c: outside of string", code);
					outputpos = 0;
				}
				break;

			case '@':
				outputpos = arg;
				break;
		}

		if (outputsize < outputpos) {
			outputsize = outputpos;
		}
	}

	output = zend_string_alloc(outputsize, 0);
	outputpos = 0;
	currentarg = 0;

	/* Do actual packing */
	for (i = 0; i < formatcount; i++) {
	    int code = (int) formatcodes[i];
		int arg = formatargs[i];

		switch ((int) code) {
			case 'a':
			case 'A':
			case 'Z': {
				int arg_cp = (code != 'Z') ? arg : MAX(0, arg - 1);

				zend_string *str = zval_get_string(&argv[currentarg++]);

				memset(&ZSTR_VAL(output)[outputpos], (code == 'a' || code == 'Z') ? '\0' : ' ', arg);
				memcpy(&ZSTR_VAL(output)[outputpos], ZSTR_VAL(str),
					   (ZSTR_LEN(str) < arg_cp) ? ZSTR_LEN(str) : arg_cp);

				outputpos += arg;
				zend_string_release(str);
				break;
			}

			case 'h':
			case 'H': {
				int nibbleshift = (code == 'h') ? 0 : 4;
				int first = 1;

				zend_string *str = zval_get_string(&argv[currentarg++]);
				char *v = ZSTR_VAL(str);

				outputpos--;
				if(arg > ZSTR_LEN(str)) {
					php_error_docref(NULL, E_WARNING, "Type %c: not enough characters in string", code);
					arg = ZSTR_LEN(str);
				}

				while (arg-- > 0) {
					char n = *v++;

					if (n >= '0' && n <= '9') {
						n -= '0';
					} else if (n >= 'A' && n <= 'F') {
						n -= ('A' - 10);
					} else if (n >= 'a' && n <= 'f') {
						n -= ('a' - 10);
					} else {
						php_error_docref(NULL, E_WARNING, "Type %c: illegal hex digit %c", code, n);
						n = 0;
					}

					if (first--) {
						ZSTR_VAL(output)[++outputpos] = 0;
					} else {
					  first = 1;
					}

					ZSTR_VAL(output)[outputpos] |= (n << nibbleshift);
					nibbleshift = (nibbleshift + 4) & 7;
				}

				outputpos++;
				zend_string_release(str);
				break;
			}

			case 'c':
			case 'C':
				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 1, byte_map, &ZSTR_VAL(output)[outputpos]);
					outputpos++;
				}
				break;

			case 's':
			case 'S':
			case 'n':
			case 'v': {
				int *map = machine_endian_short_map;

				if (code == 'n') {
					map = big_endian_short_map;
				} else if (code == 'v') {
					map = little_endian_short_map;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 2, map, &ZSTR_VAL(output)[outputpos]);
					outputpos += 2;
				}
				break;
			}

			case 'i':
			case 'I':
				while (arg-- > 0) {
					php_pack(&argv[currentarg++], sizeof(int), int_map, &ZSTR_VAL(output)[outputpos]);
					outputpos += sizeof(int);
				}
				break;

			case 'l':
			case 'L':
			case 'N':
			case 'V': {
				int *map = machine_endian_long_map;

				if (code == 'N') {
					map = big_endian_long_map;
				} else if (code == 'V') {
					map = little_endian_long_map;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 4, map, &ZSTR_VAL(output)[outputpos]);
					outputpos += 4;
				}
				break;
			}

#if SIZEOF_ZEND_LONG > 4
			case 'q':
			case 'Q':
			case 'J':
			case 'P': {
				int *map = machine_endian_longlong_map;

				if (code == 'J') {
					map = big_endian_longlong_map;
				} else if (code == 'P') {
					map = little_endian_longlong_map;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 8, map, &ZSTR_VAL(output)[outputpos]);
					outputpos += 8;
				}
				break;
			}
#endif

			case 'f': {
				while (arg-- > 0) {
					float v = (float) zval_get_double(&argv[currentarg++]);
					memcpy(&ZSTR_VAL(output)[outputpos], &v, sizeof(v));
					outputpos += sizeof(v);
				}
				break;
			}
			
			case 'g': {
				/* pack little endian float */
				while (arg-- > 0) {
					float v = (float) zval_get_double(&argv[currentarg++]);
					php_pack_copy_float(1, &ZSTR_VAL(output)[outputpos], v);
					outputpos += sizeof(v);
				}
				
				break;
			}
			case 'G': {
				/* pack big endian float */
				while (arg-- > 0) {
					float v = (float) zval_get_double(&argv[currentarg++]);
					php_pack_copy_float(0, &ZSTR_VAL(output)[outputpos], v);
					outputpos += sizeof(v);
				}
				break;
			}

			case 'd': {
				while (arg-- > 0) {
					double v = (double) zval_get_double(&argv[currentarg++]);
					memcpy(&ZSTR_VAL(output)[outputpos], &v, sizeof(v));
					outputpos += sizeof(v);
				}
				break;
			}
			
			case 'e': {
				/* pack little endian double */
				while (arg-- > 0) {
					double v = (double) zval_get_double(&argv[currentarg++]);
					php_pack_copy_double(1, &ZSTR_VAL(output)[outputpos], v);
					outputpos += sizeof(v);
				}
				break;
			}
			
			case 'E': {
				/* pack big endian double */
				while (arg-- > 0) {
					double v = (double) zval_get_double(&argv[currentarg++]);
					php_pack_copy_double(0, &ZSTR_VAL(output)[outputpos], v);
					outputpos += sizeof(v);
				}
				break;
			}

			case 'x':
				memset(&ZSTR_VAL(output)[outputpos], '\0', arg);
				outputpos += arg;
				break;

			case 'X':
				outputpos -= arg;

				if (outputpos < 0) {
					outputpos = 0;
				}
				break;

			case '@':
				if (arg > outputpos) {
					memset(&ZSTR_VAL(output)[outputpos], '\0', arg - outputpos);
				}
				outputpos = arg;
				break;
		}
	}

	efree(formatcodes);
	efree(formatargs);
	ZSTR_VAL(output)[outputpos] = '\0';
	ZSTR_LEN(output) = outputpos;
	RETURN_NEW_STR(output);
}
/* }}} */

/* {{{ php_unpack
 */
static zend_long php_unpack(char *data, size_t size, int issigned, int *map)
{
	zend_long result;
	char *cresult = (char *) &result;
	int i;

	result = issigned ? -1 : 0;

	for (i = 0; i < size; i++) {
		cresult[map[i]] = *data++;
	}

	return result;
}
/* }}} */

/* unpack() is based on Perl's unpack(), but is modified a bit from there.
 * Rather than depending on error-prone ordered lists or syntactically
 * unpleasant pass-by-reference, we return an object with named parameters
 * (like *_fetch_object()). Syntax is "f[repeat]name/...", where "f" is the
 * formatter char (like pack()), "[repeat]" is the optional repeater argument,
 * and "name" is the name of the variable to use.
 * Example: "c2chars/nints" will return an object with fields
 * chars1, chars2, and ints.
 * Numeric pack types will return numbers, a and A will return strings,
 * f and d will return doubles.
 * Implemented formats are Z, A, a, h, H, c, C, s, S, i, I, l, L, n, N, q, Q, J, P, f, d, x, X, @.
 * Added g, G for little endian float and big endian float, added e, E for little endian double and big endian double.
 */
/* {{{ proto array unpack(string format, string input)
   Unpack binary string into named array elements according to format argument */
PHP_FUNCTION(unpack)
{
	char *format, *input;
	zend_string *formatarg, *inputarg;
	zend_long formatlen, inputpos, inputlen;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &formatarg,
		&inputarg) == FAILURE) {
		return;
	}

	format = ZSTR_VAL(formatarg);
	formatlen = ZSTR_LEN(formatarg);
	input = ZSTR_VAL(inputarg);
	inputlen = ZSTR_LEN(inputarg);
	inputpos = 0;

	array_init(return_value);

	while (formatlen-- > 0) {
		char type = *(format++);
		char c;
		int arg = 1, argb;
		char *name;
		int namelen;
		int size=0;

		/* Handle format arguments if any */
		if (formatlen > 0) {
			c = *format;

			if (c >= '0' && c <= '9') {
				arg = atoi(format);

				while (formatlen > 0 && *format >= '0' && *format <= '9') {
					format++;
					formatlen--;
				}
			} else if (c == '*') {
				arg = -1;
				format++;
				formatlen--;
			}
		}

		/* Get of new value in array */
		name = format;
		argb = arg;

		while (formatlen > 0 && *format != '/') {
			formatlen--;
			format++;
		}

		namelen = format - name;

		if (namelen > 200)
			namelen = 200;

		switch ((int) type) {
			/* Never use any input */
			case 'X':
				size = -1;
				break;

			case '@':
				size = 0;
				break;

			case 'a':
			case 'A':
			case 'Z':
				size = arg;
				arg = 1;
				break;

			case 'h':
			case 'H':
				size = (arg > 0) ? (arg + (arg % 2)) / 2 : arg;
				arg = 1;
				break;

			/* Use 1 byte of input */
			case 'c':
			case 'C':
			case 'x':
				size = 1;
				break;

			/* Use 2 bytes of input */
			case 's':
			case 'S':
			case 'n':
			case 'v':
				size = 2;
				break;

			/* Use sizeof(int) bytes of input */
			case 'i':
			case 'I':
				size = sizeof(int);
				break;

			/* Use 4 bytes of input */
			case 'l':
			case 'L':
			case 'N':
			case 'V':
				size = 4;
				break;

			/* Use 8 bytes of input */
			case 'q':
			case 'Q':
			case 'J':
			case 'P':
#if SIZEOF_ZEND_LONG > 4
				size = 8;
				break;
#else
				php_error_docref(NULL, E_WARNING, "64-bit format codes are not available for 32-bit versions of PHP");
				zval_dtor(return_value);
				RETURN_FALSE;
#endif

			/* Use sizeof(float) bytes of input */
			case 'f':
			case 'g':
			case 'G':
				size = sizeof(float);
				break;

			/* Use sizeof(double) bytes of input */
			case 'd':
			case 'e':
			case 'E':
				size = sizeof(double);
				break;

			default:
				php_error_docref(NULL, E_WARNING, "Invalid format type %c", type);
				zval_dtor(return_value);
				RETURN_FALSE;
				break;
		}

		if (size != 0 && size != -1 && size < 0) {
			php_error_docref(NULL, E_WARNING, "Type %c: integer overflow", type);
			zval_dtor(return_value);
			RETURN_FALSE;
		}

		/* Do actual unpacking */
		for (i = 0; i != arg; i++ ) {
			/* Space for name + number, safe as namelen is ensured <= 200 */
			char n[256];

			if (arg != 1 || namelen == 0) {
				/* Need to add element number to name */
				snprintf(n, sizeof(n), "%.*s%d", namelen, name, i + 1);
			} else {
				/* Truncate name to next format code or end of string */
				snprintf(n, sizeof(n), "%.*s", namelen, name);
			}

			if (size != 0 && size != -1 && INT_MAX - size + 1 < inputpos) {
				php_error_docref(NULL, E_WARNING, "Type %c: integer overflow", type);
				zval_dtor(return_value);
				RETURN_FALSE;
			}

			if ((inputpos + size) <= inputlen) {
				switch ((int) type) {
					case 'a': {
						/* a will not strip any trailing whitespace or null padding */
						size_t len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						add_assoc_stringl(return_value, n, &input[inputpos], len);
						break;
					}
					case 'A': {
						/* A will strip any trailing whitespace */
						char padn = '\0'; char pads = ' '; char padt = '\t'; char padc = '\r'; char padl = '\n';
						zend_long len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						/* Remove trailing white space and nulls chars from unpacked data */
						while (--len >= 0) {
							if (input[inputpos + len] != padn
								&& input[inputpos + len] != pads
								&& input[inputpos + len] != padt
								&& input[inputpos + len] != padc
								&& input[inputpos + len] != padl
							)
								break;
						}

						add_assoc_stringl(return_value, n, &input[inputpos], len + 1);
						break;
					}
					/* New option added for Z to remain in-line with the Perl implementation */
					case 'Z': {
						/* Z will strip everything after the first null character */
						char pad = '\0';
						size_t	 s,
							 len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						/* Remove everything after the first null */
						for (s=0 ; s < len ; s++) {
							if (input[inputpos + s] == pad)
								break;
						}
						len = s;

						add_assoc_stringl(return_value, n, &input[inputpos], len);
						break;
					}


					case 'h':
					case 'H': {
						size_t len = (inputlen - inputpos) * 2;	/* Remaining */
						int nibbleshift = (type == 'h') ? 0 : 4;
						int first = 1;
						char *buf;
						size_t ipos, opos;

						/* If size was given take minimum of len and size */
						if (size >= 0 && len > (size * 2)) {
							len = size * 2;
						}

						if (len > 0 && argb > 0) {
							len -= argb % 2;
						}

						buf = emalloc(len + 1);

						for (ipos = opos = 0; opos < len; opos++) {
							char cc = (input[inputpos + ipos] >> nibbleshift) & 0xf;

							if (cc < 10) {
								cc += '0';
							} else {
								cc += 'a' - 10;
							}

							buf[opos] = cc;
							nibbleshift = (nibbleshift + 4) & 7;

							if (first-- == 0) {
								ipos++;
								first = 1;
							}
						}

						buf[len] = '\0';
						add_assoc_stringl(return_value, n, buf, len);
						efree(buf);
						break;
					}

					case 'c':
					case 'C': {
						int issigned = (type == 'c') ? (input[inputpos] & 0x80) : 0;
						zend_long v = php_unpack(&input[inputpos], 1, issigned, byte_map);
						add_assoc_long(return_value, n, v);
						break;
					}

					case 's':
					case 'S':
					case 'n':
					case 'v': {
						zend_long v;
						int issigned = 0;
						int *map = machine_endian_short_map;

						if (type == 's') {
							issigned = input[inputpos + (machine_little_endian ? 1 : 0)] & 0x80;
						} else if (type == 'n') {
							map = big_endian_short_map;
						} else if (type == 'v') {
							map = little_endian_short_map;
						}

						v = php_unpack(&input[inputpos], 2, issigned, map);
						add_assoc_long(return_value, n, v);
						break;
					}

					case 'i':
					case 'I': {
						zend_long v;
						int issigned = 0;

						if (type == 'i') {
							issigned = input[inputpos + (machine_little_endian ? (sizeof(int) - 1) : 0)] & 0x80;
						}

						v = php_unpack(&input[inputpos], sizeof(int), issigned, int_map);
						add_assoc_long(return_value, n, v);
						break;
					}

					case 'l':
					case 'L':
					case 'N':
					case 'V': {
						int issigned = 0;
						int *map = machine_endian_long_map;
						zend_long v = 0;

						if (type == 'l' || type == 'L') {
							issigned = input[inputpos + (machine_little_endian ? 3 : 0)] & 0x80;
						} else if (type == 'N') {
							issigned = input[inputpos] & 0x80;
							map = big_endian_long_map;
						} else if (type == 'V') {
							issigned = input[inputpos + 3] & 0x80;
							map = little_endian_long_map;
						}

						if (SIZEOF_ZEND_LONG > 4 && issigned) {
							v = ~INT_MAX;
						}

						v |= php_unpack(&input[inputpos], 4, issigned, map);
						if (SIZEOF_ZEND_LONG > 4) {
 							if (type == 'l') {
								v = (signed int) v;
							} else {
								v = (unsigned int) v;
							}
						}
						add_assoc_long(return_value, n, v);
						break;
					}

#if SIZEOF_ZEND_LONG > 4
					case 'q':
					case 'Q':
					case 'J':
					case 'P': {
						int issigned = 0;
						int *map = machine_endian_longlong_map;
						zend_long v = 0;

						if (type == 'q' || type == 'Q') {
							issigned = input[inputpos + (machine_little_endian ? 7 : 0)] & 0x80;
						} else if (type == 'J') {
							issigned = input[inputpos] & 0x80;
							map = big_endian_longlong_map;
						} else if (type == 'P') {
							issigned = input[inputpos + 7] & 0x80;
							map = little_endian_longlong_map;
						}

						v = php_unpack(&input[inputpos], 8, issigned, map);

						if (type == 'q') {
							v = (zend_long) v;
						} else {
							v = (zend_ulong) v;
						}

						add_assoc_long(return_value, n, v);
						break;
					}
#endif

					case 'f': /* float */ 
					case 'g': /* little endian float*/
					case 'G': /* big endian float*/
					{
						float v;

						if (type == 'g') {
							v = php_pack_parse_float(1, &input[inputpos]);
						} else if (type == 'G') {
							v = php_pack_parse_float(0, &input[inputpos]);
						} else {
							memcpy(&v, &input[inputpos], sizeof(float));
						}
						
						add_assoc_double(return_value, n, (double)v);
						break;
					}
					

					case 'd': /* double */
					case 'e': /* little endian float */
					case 'E': /* big endian float */ 
					{
						double v;
						if (type == 'e') {
							v = php_pack_parse_double(1, &input[inputpos]);
						} else if (type == 'E') {
							v = php_pack_parse_double(0, &input[inputpos]);
						} else {
							memcpy(&v, &input[inputpos], sizeof(double));
						}
						add_assoc_double(return_value, n, v);
						break;
					}

					case 'x':
						/* Do nothing with input, just skip it */
						break;

					case 'X':
						if (inputpos < size) {
							inputpos = -size;
							i = arg - 1;		/* Break out of for loop */

							if (arg >= 0) {
								php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
							}
						}
						break;

					case '@':
						if (arg <= inputlen) {
							inputpos = arg;
						} else {
							php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
						}

						i = arg - 1;	/* Done, break out of for loop */
						break;
				}

				inputpos += size;
				if (inputpos < 0) {
					if (size != -1) { /* only print warning if not working with * */
						php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
					}
					inputpos = 0;
				}
			} else if (arg < 0) {
				/* Reached end of input for '*' repeater */
				break;
			} else {
				php_error_docref(NULL, E_WARNING, "Type %c: not enough input, need %d, have " ZEND_LONG_FMT, type, size, inputlen - inputpos);
				zval_dtor(return_value);
				RETURN_FALSE;
			}
		}

		if (formatlen > 0) {
			formatlen--;	/* Skip '/' separator, does no harm if inputlen == 0 */
			format++;
		}
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pack)
{
	int machine_endian_check = 1;
	int i;

	machine_little_endian = ((char *)&machine_endian_check)[0];

	if (machine_little_endian) {
		/* Where to get lo to hi bytes from */
		byte_map[0] = 0;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = i;
		}

		machine_endian_short_map[0] = 0;
		machine_endian_short_map[1] = 1;
		big_endian_short_map[0] = 1;
		big_endian_short_map[1] = 0;
		little_endian_short_map[0] = 0;
		little_endian_short_map[1] = 1;

		machine_endian_long_map[0] = 0;
		machine_endian_long_map[1] = 1;
		machine_endian_long_map[2] = 2;
		machine_endian_long_map[3] = 3;
		big_endian_long_map[0] = 3;
		big_endian_long_map[1] = 2;
		big_endian_long_map[2] = 1;
		big_endian_long_map[3] = 0;
		little_endian_long_map[0] = 0;
		little_endian_long_map[1] = 1;
		little_endian_long_map[2] = 2;
		little_endian_long_map[3] = 3;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = 0;
		machine_endian_longlong_map[1] = 1;
		machine_endian_longlong_map[2] = 2;
		machine_endian_longlong_map[3] = 3;
		machine_endian_longlong_map[4] = 4;
		machine_endian_longlong_map[5] = 5;
		machine_endian_longlong_map[6] = 6;
		machine_endian_longlong_map[7] = 7;
		big_endian_longlong_map[0] = 7;
		big_endian_longlong_map[1] = 6;
		big_endian_longlong_map[2] = 5;
		big_endian_longlong_map[3] = 4;
		big_endian_longlong_map[4] = 3;
		big_endian_longlong_map[5] = 2;
		big_endian_longlong_map[6] = 1;
		big_endian_longlong_map[7] = 0;
		little_endian_longlong_map[0] = 0;
		little_endian_longlong_map[1] = 1;
		little_endian_longlong_map[2] = 2;
		little_endian_longlong_map[3] = 3;
		little_endian_longlong_map[4] = 4;
		little_endian_longlong_map[5] = 5;
		little_endian_longlong_map[6] = 6;
		little_endian_longlong_map[7] = 7;
#endif
	}
	else {
		zval val;
		int size = sizeof(Z_LVAL(val));
		Z_LVAL(val)=0; /*silence a warning*/

		/* Where to get hi to lo bytes from */
		byte_map[0] = size - 1;

		for (i = 0; i < (int)sizeof(int); i++) {
			int_map[i] = size - (sizeof(int) - i);
		}

		machine_endian_short_map[0] = size - 2;
		machine_endian_short_map[1] = size - 1;
		big_endian_short_map[0] = size - 2;
		big_endian_short_map[1] = size - 1;
		little_endian_short_map[0] = size - 1;
		little_endian_short_map[1] = size - 2;

		machine_endian_long_map[0] = size - 4;
		machine_endian_long_map[1] = size - 3;
		machine_endian_long_map[2] = size - 2;
		machine_endian_long_map[3] = size - 1;
		big_endian_long_map[0] = size - 4;
		big_endian_long_map[1] = size - 3;
		big_endian_long_map[2] = size - 2;
		big_endian_long_map[3] = size - 1;
		little_endian_long_map[0] = size - 1;
		little_endian_long_map[1] = size - 2;
		little_endian_long_map[2] = size - 3;
		little_endian_long_map[3] = size - 4;

#if SIZEOF_ZEND_LONG > 4
		machine_endian_longlong_map[0] = size - 8;
		machine_endian_longlong_map[1] = size - 7;
		machine_endian_longlong_map[2] = size - 6;
		machine_endian_longlong_map[3] = size - 5;
		machine_endian_longlong_map[4] = size - 4;
		machine_endian_longlong_map[5] = size - 3;
		machine_endian_longlong_map[6] = size - 2;
		machine_endian_longlong_map[7] = size - 1;
		big_endian_longlong_map[0] = size - 8;
		big_endian_longlong_map[1] = size - 7;
		big_endian_longlong_map[2] = size - 6;
		big_endian_longlong_map[3] = size - 5;
		big_endian_longlong_map[4] = size - 4;
		big_endian_longlong_map[5] = size - 3;
		big_endian_longlong_map[6] = size - 2;
		big_endian_longlong_map[7] = size - 1;
		little_endian_longlong_map[0] = size - 1;
		little_endian_longlong_map[1] = size - 2;
		little_endian_longlong_map[2] = size - 3;
		little_endian_longlong_map[3] = size - 4;
		little_endian_longlong_map[4] = size - 5;
		little_endian_longlong_map[5] = size - 6;
		little_endian_longlong_map[6] = size - 7;
		little_endian_longlong_map[7] = size - 8;
#endif
	}

	return SUCCESS;
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
