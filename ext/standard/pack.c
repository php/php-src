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
   | Author: Chris Schneider <cschneid@relog.ch>                          |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#define INC_OUTPUTPOS(a,b) \
	if ((a) < 0 || ((INT_MAX - outputpos)/((int)b)) < (a)) { \
		efree(formatcodes);	\
		efree(formatargs);	\
		zend_value_error("Type %c: integer overflow in format string", code); \
		RETURN_THROWS(); \
	} \
	outputpos += (a)*(b);

typedef enum {
	PHP_LITTLE_ENDIAN,
	PHP_BIG_ENDIAN,
} php_pack_endianness;

#ifdef WORDS_BIGENDIAN
# define MACHINE_LITTLE_ENDIAN 0
# define PHP_MACHINE_ENDIAN PHP_BIG_ENDIAN
#else
# define MACHINE_LITTLE_ENDIAN 1
# define PHP_MACHINE_ENDIAN PHP_LITTLE_ENDIAN
#endif

#ifdef ZEND_ENABLE_ZVAL_LONG64
# define PHP_LONG_BSWAP(u) ZEND_BYTES_SWAP64(u)
#else
# define PHP_LONG_BSWAP(u) ZEND_BYTES_SWAP32(u)
#endif

typedef ZEND_SET_ALIGNED(1, uint16_t unaligned_uint16_t);
typedef ZEND_SET_ALIGNED(1, uint32_t unaligned_uint32_t);
typedef ZEND_SET_ALIGNED(1, uint64_t unaligned_uint64_t);
typedef ZEND_SET_ALIGNED(1, unsigned int unaligned_uint);
typedef ZEND_SET_ALIGNED(1, int unaligned_int);

/* {{{ php_pack */
static void php_pack(const zval *val, size_t size, php_pack_endianness endianness, char *output)
{
	zend_ulong zl = zval_get_long(val);

	if ((endianness == PHP_LITTLE_ENDIAN) != MACHINE_LITTLE_ENDIAN) {
		zl = PHP_LONG_BSWAP(zl);
#if MACHINE_LITTLE_ENDIAN
		zl >>= (sizeof(zl) - size) * 8;
#endif
	} else {
#if !MACHINE_LITTLE_ENDIAN
		zl <<= (sizeof(zl) - size) * 8;
#endif
	}

	memcpy(output, (const char *) &zl, size);
}
/* }}} */

ZEND_ATTRIBUTE_CONST static inline uint16_t php_pack_reverse_int16(uint16_t arg)
{
	return ((arg & 0xFF) << 8) | ((arg >> 8) & 0xFF);
}

/* {{{ php_pack_reverse_int32 */
ZEND_ATTRIBUTE_CONST static inline uint32_t php_pack_reverse_int32(uint32_t arg)
{
	return ZEND_BYTES_SWAP32(arg);
}
/* }}} */

/* {{{ php_pack */
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

/* {{{ php_pack_copy_float */
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

/* {{{ php_pack_copy_double */
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

/* {{{ php_pack_parse_float */
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

/* {{{ php_pack_parse_double */
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
/* {{{ Takes one or more arguments and packs them into a binary string according to the format argument */
PHP_FUNCTION(pack)
{
	zval *argv = NULL;
	int num_args = 0;
	size_t i;
	int currentarg;
	char *format;
	size_t formatlen;
	char *formatcodes;
	int *formatargs;
	size_t formatcount = 0;
	int outputpos = 0, outputsize = 0;
	zend_string *output;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_STRING(format, formatlen)
		Z_PARAM_VARIADIC('*', argv, num_args)
	ZEND_PARSE_PARAMETERS_END();

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
		switch (code) {
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
					zend_value_error("Type %c: not enough arguments", code);
					RETURN_THROWS();
				}

				if (arg < 0) {
					if (!try_convert_to_string(&argv[currentarg])) {
						efree(formatcodes);
						efree(formatargs);
						RETURN_THROWS();
					}

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
					zend_value_error("64-bit format codes are not available for 32-bit versions of PHP");
					RETURN_THROWS();
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
				if (currentarg > INT_MAX - arg) {
					goto too_few_args;
				}
				currentarg += arg;

				if (currentarg > num_args) {
too_few_args:
					efree(formatcodes);
					efree(formatargs);
					zend_value_error("Type %c: too few arguments", code);
					RETURN_THROWS();
				}
				break;

			default:
				efree(formatcodes);
				efree(formatargs);
				zend_value_error("Type %c: unknown format code", code);
				RETURN_THROWS();
		}

		formatcodes[formatcount] = code;
		formatargs[formatcount] = arg;
	}

	if (currentarg < num_args) {
		php_error_docref(NULL, E_WARNING, "%d arguments unused", (num_args - currentarg));
	}

	/* Calculate output length and upper bound while processing*/
	for (i = 0; i < formatcount; i++) {
		char code = formatcodes[i];
		int arg = formatargs[i];

		switch (code) {
			case 'h':
			case 'H':
				INC_OUTPUTPOS((arg / 2) + (arg % 2),1)	/* 4 bit per arg */
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
		char code = formatcodes[i];
		int arg = formatargs[i];

		switch (code) {
			case 'a':
			case 'A':
			case 'Z': {
				size_t arg_cp = (code != 'Z') ? arg : MAX(0, arg - 1);
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(&argv[currentarg++], &tmp_str);

				memset(&ZSTR_VAL(output)[outputpos], (code == 'a' || code == 'Z') ? '\0' : ' ', arg);
				memcpy(&ZSTR_VAL(output)[outputpos], ZSTR_VAL(str),
					   (ZSTR_LEN(str) < arg_cp) ? ZSTR_LEN(str) : arg_cp);

				outputpos += arg;
				zend_tmp_string_release(tmp_str);
				break;
			}

			case 'h':
			case 'H': {
				int nibbleshift = (code == 'h') ? 0 : 4;
				int first = 1;
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(&argv[currentarg++], &tmp_str);
				char *v = ZSTR_VAL(str);

				outputpos--;
				if ((size_t)arg > ZSTR_LEN(str)) {
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
				zend_tmp_string_release(tmp_str);
				break;
			}

			case 'c':
			case 'C':
				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 1, PHP_MACHINE_ENDIAN, &ZSTR_VAL(output)[outputpos]);
					outputpos++;
				}
				break;

			case 's':
			case 'S':
			case 'n':
			case 'v': {
				php_pack_endianness endianness = PHP_MACHINE_ENDIAN;

				if (code == 'n') {
					endianness = PHP_BIG_ENDIAN;
				} else if (code == 'v') {
					endianness = PHP_LITTLE_ENDIAN;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 2, endianness, &ZSTR_VAL(output)[outputpos]);
					outputpos += 2;
				}
				break;
			}

			case 'i':
			case 'I':
				while (arg-- > 0) {
					php_pack(&argv[currentarg++], sizeof(int), PHP_MACHINE_ENDIAN, &ZSTR_VAL(output)[outputpos]);
					outputpos += sizeof(int);
				}
				break;

			case 'l':
			case 'L':
			case 'N':
			case 'V': {
				php_pack_endianness endianness = PHP_MACHINE_ENDIAN;

				if (code == 'N') {
					endianness = PHP_BIG_ENDIAN;
				} else if (code == 'V') {
					endianness = PHP_LITTLE_ENDIAN;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 4, endianness, &ZSTR_VAL(output)[outputpos]);
					outputpos += 4;
				}
				break;
			}

#if SIZEOF_ZEND_LONG > 4
			case 'q':
			case 'Q':
			case 'J':
			case 'P': {
				php_pack_endianness endianness = PHP_MACHINE_ENDIAN;

				if (code == 'J') {
					endianness = PHP_BIG_ENDIAN;
				} else if (code == 'P') {
					endianness = PHP_LITTLE_ENDIAN;
				}

				while (arg-- > 0) {
					php_pack(&argv[currentarg++], 8, endianness, &ZSTR_VAL(output)[outputpos]);
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
					double v = zval_get_double(&argv[currentarg++]);
					memcpy(&ZSTR_VAL(output)[outputpos], &v, sizeof(v));
					outputpos += sizeof(v);
				}
				break;
			}

			case 'e': {
				/* pack little endian double */
				while (arg-- > 0) {
					double v = zval_get_double(&argv[currentarg++]);
					php_pack_copy_double(1, &ZSTR_VAL(output)[outputpos], v);
					outputpos += sizeof(v);
				}
				break;
			}

			case 'E': {
				/* pack big endian double */
				while (arg-- > 0) {
					double v = zval_get_double(&argv[currentarg++]);
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
/* {{{ Unpack binary string into named array elements according to format argument */
PHP_FUNCTION(unpack)
{
	char *format, *input;
	zend_string *formatarg, *inputarg;
	zend_long formatlen, inputpos, inputlen;
	int i;
	zend_long offset = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(formatarg)
		Z_PARAM_STR(inputarg)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	format = ZSTR_VAL(formatarg);
	formatlen = ZSTR_LEN(formatarg);
	input = ZSTR_VAL(inputarg);
	inputlen = ZSTR_LEN(inputarg);
	inputpos = 0;


	if (offset < 0 || offset > inputlen) {
		zend_argument_value_error(3, "must be contained in argument #2 ($data)");
		RETURN_THROWS();
	}

	input += offset;
	inputlen -= offset;

	array_init(return_value);

	while (formatlen-- > 0) {
		char type = *(format++);
		int repetitions = 1, argb;
		char *name;
		int namelen;
		int size = 0;

		/* Handle format arguments if any */
		if (formatlen > 0) {
			char c = *format;

			if (c >= '0' && c <= '9') {
				errno = 0;
				long tmp = strtol(format, NULL, 10);
				/* There is not strtoi. We have to check the range ourselves.
				 * With 32-bit long the INT_{MIN,MAX} are useless because long == int, but with 64-bit they do limit us to 32-bit. */
				if (errno || tmp < INT_MIN || tmp > INT_MAX) {
					php_error_docref(NULL, E_WARNING, "Type %c: integer overflow", type);
					zend_array_destroy(Z_ARR_P(return_value));
					RETURN_FALSE;
				}
				repetitions = tmp;

				while (formatlen > 0 && *format >= '0' && *format <= '9') {
					format++;
					formatlen--;
				}
			} else if (c == '*') {
				repetitions = -1;
				format++;
				formatlen--;
			}
		}

		/* Get of new value in array */
		name = format;
		argb = repetitions;

		while (formatlen > 0 && *format != '/') {
			formatlen--;
			format++;
		}

		namelen = format - name;

		if (namelen > 200)
			namelen = 200;

		switch (type) {
			/* Never use any input */
			case 'X':
				size = -1;
				if (repetitions < 0) {
					php_error_docref(NULL, E_WARNING, "Type %c: '*' ignored", type);
					repetitions = 1;
				}
				break;

			case '@':
				size = 0;
				break;

			case 'a':
			case 'A':
			case 'Z':
				size = repetitions;
				repetitions = 1;
				break;

			case 'h':
			case 'H':
				size = (repetitions > 0) ? ((unsigned int) repetitions + 1) / 2 : repetitions;
				repetitions = 1;
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
				zend_value_error("64-bit format codes are not available for 32-bit versions of PHP");
				RETURN_THROWS();
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
				zend_value_error("Invalid format type %c", type);
				RETURN_THROWS();
		}


		/* Do actual unpacking */
		for (i = 0; i != repetitions; i++ ) {

			if (size != 0 && size != -1 && INT_MAX - size + 1 < inputpos) {
				php_error_docref(NULL, E_WARNING, "Type %c: integer overflow", type);
				zend_array_destroy(Z_ARR_P(return_value));
				RETURN_FALSE;
			}

			if ((inputpos + size) <= inputlen) {

				zend_string* real_name;
				zend_long long_key = 0;
				zval val;

				if (namelen == 0) {
					real_name = NULL;
					long_key = i + 1;
				} else if (repetitions == 1) {
					/* Use a part of the formatarg argument directly as the name. */
					real_name = zend_string_init_fast(name, namelen);
				} else {
					/* Need to add the 1-based element number to the name */
					char buf[MAX_LENGTH_OF_LONG + 1];
					char *res = zend_print_ulong_to_buf(buf + sizeof(buf) - 1, i+1);
					size_t digits = buf + sizeof(buf) - 1 - res;
					real_name = zend_string_concat2(name, namelen, res, digits);
				}

				switch (type) {
					case 'a': {
						/* a will not strip any trailing whitespace or null padding */
						zend_long len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						ZVAL_STRINGL(&val, &input[inputpos], len);
						break;
					}
					case 'A': {
						/* A will strip any trailing whitespace */
						zend_long len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						/* Remove trailing white space and nulls chars from unpacked data */
						while (--len >= 0) {
							if (input[inputpos + len] != '\0'
								&& input[inputpos + len] != ' '
								&& input[inputpos + len] != '\t'
								&& input[inputpos + len] != '\r'
								&& input[inputpos + len] != '\n'
							)
								break;
						}

						ZVAL_STRINGL(&val, &input[inputpos], len + 1);
						break;
					}
					/* New option added for Z to remain in-line with the Perl implementation */
					case 'Z': {
						/* Z will strip everything after the first null character */
						zend_long s,
							 len = inputlen - inputpos;	/* Remaining string */

						/* If size was given take minimum of len and size */
						if ((size >= 0) && (len > size)) {
							len = size;
						}

						size = len;

						/* Remove everything after the first null */
						for (s=0 ; s < len ; s++) {
							if (input[inputpos + s] == '\0')
								break;
						}
						len = s;

						ZVAL_STRINGL(&val, &input[inputpos], len);
						break;
					}


					case 'h':
					case 'H': {
						zend_long len = (inputlen - inputpos) * 2;	/* Remaining */
						int nibbleshift = (type == 'h') ? 0 : 4;
						int first = 1;
						zend_string *buf;
						zend_long ipos, opos;


						if (size > INT_MAX / 2) {
							if (real_name) {
								zend_string_release_ex(real_name, false);
							}
							zend_argument_value_error(1, "repeater must be less than or equal to %d", INT_MAX / 2);
							RETURN_THROWS();
						}

						/* If size was given take minimum of len and size */
						if (size >= 0 && len > (size * 2)) {
							len = size * 2;
						}

						if (len > 0 && argb > 0) {
							len -= argb % 2;
						}

						buf = zend_string_alloc(len, 0);

						for (ipos = opos = 0; opos < len; opos++) {
							char cc = (input[inputpos + ipos] >> nibbleshift) & 0xf;

							if (cc < 10) {
								cc += '0';
							} else {
								cc += 'a' - 10;
							}

							ZSTR_VAL(buf)[opos] = cc;
							nibbleshift = (nibbleshift + 4) & 7;

							if (first-- == 0) {
								ipos++;
								first = 1;
							}
						}

						ZSTR_VAL(buf)[len] = '\0';

						ZVAL_STR(&val, buf);
						break;
					}

					case 'c':   /* signed */
					case 'C': { /* unsigned */
						uint8_t x = input[inputpos];
						zend_long v = (type == 'c') ? (int8_t) x : x;

						ZVAL_LONG(&val, v);
						break;
					}

					case 's':   /* signed machine endian   */
					case 'S':   /* unsigned machine endian */
					case 'n':   /* unsigned big endian     */
					case 'v': { /* unsigned little endian  */
						zend_long v = 0;
						uint16_t x = *((unaligned_uint16_t*) &input[inputpos]);

						if (type == 's') {
							v = (int16_t) x;
						} else if ((type == 'n' && MACHINE_LITTLE_ENDIAN) || (type == 'v' && !MACHINE_LITTLE_ENDIAN)) {
							v = php_pack_reverse_int16(x);
						} else {
							v = x;
						}

						ZVAL_LONG(&val, v);
						break;
					}

					case 'i':   /* signed integer, machine size, machine endian */
					case 'I': { /* unsigned integer, machine size, machine endian */
						zend_long v;
						if (type == 'i') {
							int x = *((unaligned_int*) &input[inputpos]);
							v = x;
						} else {
							unsigned int x = *((unaligned_uint*) &input[inputpos]);
							v = x;
						}

						ZVAL_LONG(&val, v);
						break;
					}

					case 'l':   /* signed machine endian   */
					case 'L':   /* unsigned machine endian */
					case 'N':   /* unsigned big endian     */
					case 'V': { /* unsigned little endian  */
						zend_long v = 0;
						uint32_t x = *((unaligned_uint32_t*) &input[inputpos]);

						if (type == 'l') {
							v = (int32_t) x;
						} else if ((type == 'N' && MACHINE_LITTLE_ENDIAN) || (type == 'V' && !MACHINE_LITTLE_ENDIAN)) {
							v = php_pack_reverse_int32(x);
						} else {
							v = x;
						}

						ZVAL_LONG(&val, v);
						break;
					}

#if SIZEOF_ZEND_LONG > 4
					case 'q':   /* signed machine endian   */
					case 'Q':   /* unsigned machine endian */
					case 'J':   /* unsigned big endian     */
					case 'P': { /* unsigned little endian  */
						zend_long v = 0;
						uint64_t x = *((unaligned_uint64_t*) &input[inputpos]);

						if (type == 'q') {
							v = (int64_t) x;
						} else if ((type == 'J' && MACHINE_LITTLE_ENDIAN) || (type == 'P' && !MACHINE_LITTLE_ENDIAN)) {
							v = php_pack_reverse_int64(x);
						} else {
							v = x;
						}

						ZVAL_LONG(&val, v);
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

						ZVAL_DOUBLE(&val, v);
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

						ZVAL_DOUBLE(&val, v);
						break;
					}

					case 'x':
						/* Do nothing with input, just skip it */
						goto no_output;

					case 'X':
						if (inputpos < size) {
							inputpos = -size;
							i = repetitions - 1;		/* Break out of for loop */

							if (repetitions >= 0) {
								php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
							}
						}
						goto no_output;

					case '@':
						if (repetitions <= inputlen) {
							inputpos = repetitions;
						} else {
							php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
						}

						i = repetitions - 1;	/* Done, break out of for loop */
						goto no_output;
				}

				if (real_name) {
					zend_symtable_update(Z_ARRVAL_P(return_value), real_name, &val);
				} else {
					zend_hash_index_update(Z_ARRVAL_P(return_value), long_key, &val);
				}

no_output:
				if (real_name) {
					zend_string_release_ex(real_name, false);
				}

				inputpos += size;
				if (inputpos < 0) {
					if (size != -1) { /* only print warning if not working with * */
						php_error_docref(NULL, E_WARNING, "Type %c: outside of string", type);
					}
					inputpos = 0;
				}
			} else if (repetitions < 0) {
				/* Reached end of input for '*' repeater */
				break;
			} else {
				php_error_docref(NULL, E_WARNING, "Type %c: not enough input values, need %d values but only " ZEND_LONG_FMT " %s provided", type, size, inputlen - inputpos, inputlen - inputpos == 1 ? "was" : "were");
				zend_array_destroy(Z_ARR_P(return_value));
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
