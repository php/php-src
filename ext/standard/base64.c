/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
 */

#include <string.h>

#include "php.h"
#include "base64.h"
#include "zend_bitset.h"

/* {{{ base64 tables */
static const char base64_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
	-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
	-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};
/* }}} */

#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>

static zend_always_inline unsigned char *neon_base64_encode(const unsigned char *in, size_t inl, unsigned char *out, size_t *left)
{
	const uint8x16x4_t table = {{
		vld1q_u8((const uint8_t *)base64_table), vld1q_u8((const uint8_t *)base64_table + 16),
		vld1q_u8((const uint8_t *)base64_table + 32), vld1q_u8((const uint8_t *)base64_table + 48)
	}};

	do {
		/* [ccdddddd | bbbbcccc | aaaaaabb]
		    x.val[2] | x.val[1] | x.val[0] */
		const uint8x16x3_t x = vld3q_u8(in);
		uint8x16x4_t result;

		result.val[0] = vqtbl4q_u8(table, vshrq_n_u8(x.val[0], 2));
		result.val[1] = vqtbl4q_u8(table, vandq_u8(vsliq_n_u8(vshrq_n_u8(x.val[1], 4), x.val[0], 4), vdupq_n_u8(0x3f)));
		result.val[2] = vqtbl4q_u8(table, vandq_u8(vsliq_n_u8(vshrq_n_u8(x.val[2], 6), x.val[1], 2), vdupq_n_u8(0x3f)));
		result.val[3] = vqtbl4q_u8(table, vandq_u8(x.val[2], vdupq_n_u8(0x3f)));
		vst4q_u8(out, result);
		out += 64;
		in += 16 * 3;
		inl -= 16 * 3;
	} while (inl >= 16 * 3);

	*left = inl;
	return out;
}
#endif /* defined(__aarch64__) || defined(_M_ARM64) */

static zend_always_inline unsigned char *php_base64_encode_impl(const unsigned char *in, size_t inl, unsigned char *out, zend_long flags) /* {{{ */
{
#if defined(__aarch64__) || defined(_M_ARM64)
	if (inl >= 16 * 3) {
		size_t left = 0;
		out = neon_base64_encode(in, inl, out, &left);
		in += inl - left;
		inl = left;
	}
#endif

	while (inl > 2) { /* keep going until we have less than 24 bits */
		*out++ = base64_table[in[0] >> 2];
		*out++ = base64_table[((in[0] & 0x03) << 4) + (in[1] >> 4)];
		*out++ = base64_table[((in[1] & 0x0f) << 2) + (in[2] >> 6)];
		*out++ = base64_table[in[2] & 0x3f];

		in += 3;
		inl -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if (inl != 0) {
		*out++ = base64_table[in[0] >> 2];
		if (inl > 1) {
			*out++ = base64_table[((in[0] & 0x03) << 4) + (in[1] >> 4)];
			*out++ = base64_table[(in[1] & 0x0f) << 2];
			if ((flags & PHP_BASE64_NO_PADDING) == 0) {
				*out++ = base64_pad;
			}
		} else {
			*out++ = base64_table[(in[0] & 0x03) << 4];
			if ((flags & PHP_BASE64_NO_PADDING) == 0) {
				*out++ = base64_pad;
				*out++ = base64_pad;
			}
		}
	}

	*out = '\0';

	return out;
}
/* }}} */

/* Append the 6-bit value ch, which is the i-th alphabet character of the input,
 * to the output. out[*j] holds the bits of a partially decoded byte. */
static zend_always_inline void php_base64_decode_put(unsigned char *out, size_t *j, size_t i, int ch)
{
	switch (i % 4) {
		case 0:
			out[*j] = ch << 2;
			break;
		case 1:
			out[(*j)++] |= ch >> 4;
			out[*j] = (ch & 0x0f) << 4;
			break;
		case 2:
			out[(*j)++] |= ch >>2;
			out[*j] = (ch & 0x03) << 6;
			break;
		case 3:
			out[(*j)++] |= ch;
			break;
	}
}

/* Decode in[0..inl) one character at a time. *i counts the alphabet characters
 * seen so far and *padding the '=' characters, so that a vectorized decoder can
 * hand over its state in the middle of the input. */
static zend_always_inline int php_base64_decode_loop(const unsigned char *in, size_t inl, unsigned char *out, size_t *outl, size_t *ip, size_t *paddingp, bool strict) /* {{{ */
{
	int ch;
	size_t i = *ip, padding = *paddingp, j = *outl;

	/* run through the whole string, converting as we go */
	while (inl-- > 0) {
		ch = *in++;
		if (ch == base64_pad) {
			padding++;
			continue;
		}

		ch = base64_reverse_table[ch];
		if (!strict) {
			/* skip unknown characters and whitespace */
			if (ch < 0) {
				continue;
			}
		} else {
			/* skip whitespace */
			if (ch == -1) {
				continue;
			}
			/* fail on bad characters or if any data follows padding */
			if (ch == -2 || padding) {
				return 0;
			}
		}

		php_base64_decode_put(out, &j, i, ch);
		i++;
	}

	*ip = i;
	*paddingp = padding;
	*outl = j;
	return 1;
}
/* }}} */

/* Check the end of the input and terminate the output. */
static zend_always_inline int php_base64_decode_finish(unsigned char *out, size_t outl, size_t i, size_t padding, bool strict) /* {{{ */
{
	/* fail if the input is truncated (only one char in last group) */
	if (strict && i % 4 == 1) {
		return 0;
	}

	/* fail if the padding length is wrong (not VV==, VVV=), but accept zero padding
	 * RFC 4648: "In some circumstances, the use of padding [--] is not required" */
	if (strict && padding && (padding > 2 || (i + padding) % 4 != 0)) {
		return 0;
	}

	out[outl] = '\0';

	return 1;
}
/* }}} */

/* {{{ php_base64_encode */

#ifdef ZEND_INTRIN_AVX2_NATIVE
# undef ZEND_INTRIN_SSSE3_NATIVE
# undef ZEND_INTRIN_SSSE3_RESOLVER
# undef ZEND_INTRIN_SSSE3_FUNC_PROTO
# undef ZEND_INTRIN_SSSE3_FUNC_PTR
#elif defined(ZEND_INTRIN_AVX2_FUNC_PROTO) && defined(ZEND_INTRIN_SSSE3_NATIVE)
# undef ZEND_INTRIN_SSSE3_NATIVE
# undef ZEND_INTRIN_SSSE3_RESOLVER
# define ZEND_INTRIN_SSSE3_RESOLVER 1
# define ZEND_INTRIN_SSSE3_FUNC_PROTO 1
# undef ZEND_INTRIN_SSSE3_FUNC_DECL
# ifdef HAVE_FUNC_ATTRIBUTE_TARGET
#  define ZEND_INTRIN_SSSE3_FUNC_DECL(func) ZEND_API func __attribute__((target("ssse3")))
# else
#  define ZEND_INTRIN_SSSE3_FUNC_DECL(func) ZEND_API func
# endif
#elif defined(ZEND_INTRIN_AVX2_FUNC_PTR) && defined(ZEND_INTRIN_SSSE3_NATIVE)
# undef ZEND_INTRIN_SSSE3_NATIVE
# undef ZEND_INTRIN_SSSE3_RESOLVER
# define ZEND_INTRIN_SSSE3_RESOLVER 1
# define ZEND_INTRIN_SSSE3_FUNC_PTR 1
# undef ZEND_INTRIN_SSSE3_FUNC_DECL
# ifdef HAVE_FUNC_ATTRIBUTE_TARGET
#  define ZEND_INTRIN_SSSE3_FUNC_DECL(func) ZEND_API func __attribute__((target("ssse3")))
# else
#  define ZEND_INTRIN_SSSE3_FUNC_DECL(func) ZEND_API func
# endif
#endif

/* Only enable avx512 resolver if avx2 use resolver also */
#if defined(ZEND_INTRIN_AVX2_FUNC_PROTO) && defined(ZEND_INTRIN_AVX512_FUNC_PROTO)
#define BASE64_INTRIN_AVX512_FUNC_PROTO 1
#endif
#if defined(ZEND_INTRIN_AVX2_FUNC_PTR) && defined(ZEND_INTRIN_AVX512_FUNC_PTR)
#define BASE64_INTRIN_AVX512_FUNC_PTR 1
#endif
#if defined(ZEND_INTRIN_AVX2_FUNC_PROTO) && defined(ZEND_INTRIN_AVX512_VBMI_FUNC_PROTO)
#define BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO 1
#endif
#if defined(ZEND_INTRIN_AVX2_FUNC_PTR) && defined(ZEND_INTRIN_AVX512_VBMI_FUNC_PTR)
#define BASE64_INTRIN_AVX512_VBMI_FUNC_PTR 1
#endif

#ifdef ZEND_INTRIN_AVX2_NATIVE
# include <immintrin.h>
#elif defined(ZEND_INTRIN_SSSE3_NATIVE)
# include <tmmintrin.h>
#elif defined(ZEND_INTRIN_SSSE3_RESOLVER) || defined(ZEND_INTRIN_AVX2_RESOLVER)
# ifdef ZEND_INTRIN_AVX2_RESOLVER
#  include <immintrin.h>
# else
#  include <tmmintrin.h>
# endif /* (ZEND_INTRIN_SSSE3_RESOLVER || ZEND_INTRIN_AVX2_RESOLVER) */
# include "Zend/zend_cpuinfo.h"

# if defined(BASE64_INTRIN_AVX512_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_FUNC_PTR)
ZEND_INTRIN_AVX512_FUNC_DECL(zend_string *php_base64_encode_avx512(const unsigned char *str, size_t length, zend_long flags));
# endif
# if defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PTR)
ZEND_INTRIN_AVX512_VBMI_FUNC_DECL(zend_string *php_base64_encode_avx512_vbmi(const unsigned char *str, size_t length, zend_long flags));
ZEND_INTRIN_AVX512_VBMI_FUNC_DECL(zend_string *php_base64_decode_ex_avx512_vbmi(const unsigned char *str, size_t length, bool strict));
# endif

# ifdef ZEND_INTRIN_AVX2_RESOLVER
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_encode_avx2(const unsigned char *str, size_t length, zend_long flags));
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_decode_ex_avx2(const unsigned char *str, size_t length, bool strict));
# endif

# ifdef ZEND_INTRIN_SSSE3_RESOLVER
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length, zend_long flags));
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, bool strict));
# endif

zend_string *php_base64_encode_default(const unsigned char *str, size_t length, zend_long flags);
zend_string *php_base64_decode_ex_default(const unsigned char *str, size_t length, bool strict);

# if (defined(ZEND_INTRIN_AVX2_FUNC_PROTO) || defined(ZEND_INTRIN_SSSE3_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO))
PHPAPI zend_string *php_base64_encode_ex(const unsigned char *str, size_t length, zend_long flags) __attribute__((ifunc("resolve_base64_encode")));
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, bool strict) __attribute__((ifunc("resolve_base64_decode")));

typedef zend_string *(*base64_encode_func_t)(const unsigned char *, size_t, zend_long flags);
typedef zend_string *(*base64_decode_func_t)(const unsigned char *, size_t, bool);

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED /* clang mistakenly warns about this */
static base64_encode_func_t resolve_base64_encode(void) {
# ifdef BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO
	if (zend_cpu_supports_avx512_vbmi()) {
		return php_base64_encode_avx512_vbmi;
	} else
# endif
# ifdef BASE64_INTRIN_AVX512_FUNC_PROTO
	if (zend_cpu_supports_avx512()) {
		return php_base64_encode_avx512;
	} else
# endif
# ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
	if (zend_cpu_supports_avx2()) {
		return php_base64_encode_avx2;
	} else
# endif
#ifdef ZEND_INTRIN_SSSE3_FUNC_PROTO
	if (zend_cpu_supports_ssse3()) {
		return php_base64_encode_ssse3;
	}
#endif
	return php_base64_encode_default;
}

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED /* clang mistakenly warns about this */
static base64_decode_func_t resolve_base64_decode(void) {
# ifdef BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO
	if (zend_cpu_supports_avx512_vbmi()) {
		return php_base64_decode_ex_avx512_vbmi;
	} else
# endif
# ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
	if (zend_cpu_supports_avx2()) {
		return php_base64_decode_ex_avx2;
	} else
# endif
#ifdef ZEND_INTRIN_SSSE3_FUNC_PROTO
	if (zend_cpu_supports_ssse3()) {
		return php_base64_decode_ex_ssse3;
	}
#endif
	return php_base64_decode_ex_default;
}
# else /* (ZEND_INTRIN_AVX2_FUNC_PROTO || ZEND_INTRIN_SSSE3_FUNC_PROTO) */

PHPAPI zend_string *(*php_base64_encode_ptr)(const unsigned char *str, size_t length, zend_long flags) = NULL;
PHPAPI zend_string *(*php_base64_decode_ex_ptr)(const unsigned char *str, size_t length, bool strict) = NULL;

PHPAPI zend_string *php_base64_encode_ex(const unsigned char *str, size_t length, zend_long flags) {
	return php_base64_encode_ptr(str, length, flags);
}
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, bool strict) {
	return php_base64_decode_ex_ptr(str, length, strict);
}

PHP_MINIT_FUNCTION(base64_intrin)
{
# ifdef BASE64_INTRIN_AVX512_VBMI_FUNC_PTR
	if (zend_cpu_supports_avx512_vbmi()) {
		php_base64_encode_ptr = php_base64_encode_avx512_vbmi;
		php_base64_decode_ex_ptr = php_base64_decode_ex_avx512_vbmi;
	} else
# endif
# ifdef BASE64_INTRIN_AVX512_FUNC_PTR
	if (zend_cpu_supports_avx512()) {
		php_base64_encode_ptr = php_base64_encode_avx512;
		php_base64_decode_ex_ptr = php_base64_decode_ex_avx2;
	} else
# endif
# ifdef ZEND_INTRIN_AVX2_FUNC_PTR
	if (zend_cpu_supports_avx2()) {
		php_base64_encode_ptr = php_base64_encode_avx2;
		php_base64_decode_ex_ptr = php_base64_decode_ex_avx2;
	} else
# endif
#ifdef ZEND_INTRIN_SSSE3_FUNC_PTR
	if (zend_cpu_supports_ssse3()) {
		php_base64_encode_ptr = php_base64_encode_ssse3;
		php_base64_decode_ex_ptr = php_base64_decode_ex_ssse3;
	} else
#endif
	{
		php_base64_encode_ptr = php_base64_encode_default;
		php_base64_decode_ex_ptr = php_base64_decode_ex_default;
	}
	return SUCCESS;
}
# endif /* (ZEND_INTRIN_AVX2_FUNC_PROTO || ZEND_INTRIN_SSSE3_FUNC_PROTO) */
#endif /* ZEND_INTRIN_AVX2_NATIVE */

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER) || defined(__aarch64__) || defined(_M_ARM64)
/*
 * The vectorized decoders follow simdutf (https://github.com/simdutf/simdutf).
 * See:
 *
 * Wojciech Muła, Daniel Lemire, Faster Base64 Encoding and Decoding using AVX2
 * Instructions, ACM Transactions on the Web 12 (3), 2018.
 * https://doi.org/10.1145/3132709
 *
 * Wojciech Muła, Daniel Lemire, Base64 encoding and decoding at almost the
 * speed of a memory copy, Software: Practice and Experience 50 (2), 2020.
 * https://doi.org/10.1002/spe.2777
 *
 * Daniel Lemire, Fast Forgiving Base64 Decoding with SIMD Instructions,
 * ACM Transactions on Architecture and Code Optimization, 2026.
 * https://doi.org/10.1145/3859286
 *
 * The input is read in blocks of 64 bytes, translated to 6-bit values with a
 * mask of the bytes outside the alphabet. A block made only of alphabet
 * characters is decoded directly. Otherwise its alphabet characters are
 * compressed into a buffer of pending values, decoded 64 at a time. In
 * non-strict mode every other byte is ignored, so the whole input is decoded
 * this way. In strict mode only whitespace is: a block holding '=' or an
 * invalid byte goes to the scalar loop, except for the last block, whose
 * padding is checked here.
 *
 * Writing 64 bytes for 48 decoded bytes is safe: when it happens, at least 64
 * alphabet characters have been read, so the output written so far plus 64
 * bytes is at most the input length, which is the size of the output buffer.
 */

/* The whitespace skipped in strict mode, and the padding, as tables indexed by
 * the low nibble of a byte: a byte is in the set if the entry is the byte. */
static const uint8_t php_base64_ws_set[16] = {
	' ', 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, '\t', '\n', 0xff, 0xff, '\r', 0xff, 0xff
};
static const uint8_t php_base64_pad_set[16] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, '=', 0xff, 0xff
};

/* Strict mode, after '=' or an invalid byte before the last block: decode the
 * n pending values, then the rest of the input one character at a time. */
static zend_never_inline zend_string *php_base64_decode_resume(zend_string *result, unsigned char *o, const uint8_t *values, size_t n, const unsigned char *in, size_t inl)
{
	unsigned char *out = (unsigned char *)ZSTR_VAL(result);
	size_t outl = o - out, i, padding = 0;

	for (i = 0; i < n; i++) {
		php_base64_decode_put(out, &outl, i, values[i]);
	}
	if (!php_base64_decode_loop(in, inl, out, &outl, &i, &padding, true)
			|| !php_base64_decode_finish(out, outl, i, padding, true)) {
		zend_string_efree(result);
		return NULL;
	}
	ZSTR_LEN(result) = outl;

	return result;
}

/* Strict mode, last block: the bytes outside the alphabet must be whitespace or
 * padding, and no alphabet character may follow the padding. Returns the
 * padding length (3 for 3 or more), or -1. */
static zend_always_inline int php_base64_decode_padding(uint64_t good, uint64_t bad, uint64_t ws, uint64_t pad)
{
	int padding = 0;

	if ((bad & ~ws & ~pad) || (good & (0 - (pad & (0 - pad))))) {
		return -1;
	}
	for (; pad && padding < 3; pad &= pad - 1) {
		padding++;
	}

	return padding;
}

/* For each 8-bit mask, the positions of its zero bits, in order, one per byte:
 * a byte shuffle that removes the bytes whose bit is set. */
static const uint64_t php_base64_thintable[256] = {
	UINT64_C(0x0706050403020100), UINT64_C(0x0007060504030201), UINT64_C(0x0007060504030200), UINT64_C(0x0000070605040302),
	UINT64_C(0x0007060504030100), UINT64_C(0x0000070605040301), UINT64_C(0x0000070605040300), UINT64_C(0x0000000706050403),
	UINT64_C(0x0007060504020100), UINT64_C(0x0000070605040201), UINT64_C(0x0000070605040200), UINT64_C(0x0000000706050402),
	UINT64_C(0x0000070605040100), UINT64_C(0x0000000706050401), UINT64_C(0x0000000706050400), UINT64_C(0x0000000007060504),
	UINT64_C(0x0007060503020100), UINT64_C(0x0000070605030201), UINT64_C(0x0000070605030200), UINT64_C(0x0000000706050302),
	UINT64_C(0x0000070605030100), UINT64_C(0x0000000706050301), UINT64_C(0x0000000706050300), UINT64_C(0x0000000007060503),
	UINT64_C(0x0000070605020100), UINT64_C(0x0000000706050201), UINT64_C(0x0000000706050200), UINT64_C(0x0000000007060502),
	UINT64_C(0x0000000706050100), UINT64_C(0x0000000007060501), UINT64_C(0x0000000007060500), UINT64_C(0x0000000000070605),
	UINT64_C(0x0007060403020100), UINT64_C(0x0000070604030201), UINT64_C(0x0000070604030200), UINT64_C(0x0000000706040302),
	UINT64_C(0x0000070604030100), UINT64_C(0x0000000706040301), UINT64_C(0x0000000706040300), UINT64_C(0x0000000007060403),
	UINT64_C(0x0000070604020100), UINT64_C(0x0000000706040201), UINT64_C(0x0000000706040200), UINT64_C(0x0000000007060402),
	UINT64_C(0x0000000706040100), UINT64_C(0x0000000007060401), UINT64_C(0x0000000007060400), UINT64_C(0x0000000000070604),
	UINT64_C(0x0000070603020100), UINT64_C(0x0000000706030201), UINT64_C(0x0000000706030200), UINT64_C(0x0000000007060302),
	UINT64_C(0x0000000706030100), UINT64_C(0x0000000007060301), UINT64_C(0x0000000007060300), UINT64_C(0x0000000000070603),
	UINT64_C(0x0000000706020100), UINT64_C(0x0000000007060201), UINT64_C(0x0000000007060200), UINT64_C(0x0000000000070602),
	UINT64_C(0x0000000007060100), UINT64_C(0x0000000000070601), UINT64_C(0x0000000000070600), UINT64_C(0x0000000000000706),
	UINT64_C(0x0007050403020100), UINT64_C(0x0000070504030201), UINT64_C(0x0000070504030200), UINT64_C(0x0000000705040302),
	UINT64_C(0x0000070504030100), UINT64_C(0x0000000705040301), UINT64_C(0x0000000705040300), UINT64_C(0x0000000007050403),
	UINT64_C(0x0000070504020100), UINT64_C(0x0000000705040201), UINT64_C(0x0000000705040200), UINT64_C(0x0000000007050402),
	UINT64_C(0x0000000705040100), UINT64_C(0x0000000007050401), UINT64_C(0x0000000007050400), UINT64_C(0x0000000000070504),
	UINT64_C(0x0000070503020100), UINT64_C(0x0000000705030201), UINT64_C(0x0000000705030200), UINT64_C(0x0000000007050302),
	UINT64_C(0x0000000705030100), UINT64_C(0x0000000007050301), UINT64_C(0x0000000007050300), UINT64_C(0x0000000000070503),
	UINT64_C(0x0000000705020100), UINT64_C(0x0000000007050201), UINT64_C(0x0000000007050200), UINT64_C(0x0000000000070502),
	UINT64_C(0x0000000007050100), UINT64_C(0x0000000000070501), UINT64_C(0x0000000000070500), UINT64_C(0x0000000000000705),
	UINT64_C(0x0000070403020100), UINT64_C(0x0000000704030201), UINT64_C(0x0000000704030200), UINT64_C(0x0000000007040302),
	UINT64_C(0x0000000704030100), UINT64_C(0x0000000007040301), UINT64_C(0x0000000007040300), UINT64_C(0x0000000000070403),
	UINT64_C(0x0000000704020100), UINT64_C(0x0000000007040201), UINT64_C(0x0000000007040200), UINT64_C(0x0000000000070402),
	UINT64_C(0x0000000007040100), UINT64_C(0x0000000000070401), UINT64_C(0x0000000000070400), UINT64_C(0x0000000000000704),
	UINT64_C(0x0000000703020100), UINT64_C(0x0000000007030201), UINT64_C(0x0000000007030200), UINT64_C(0x0000000000070302),
	UINT64_C(0x0000000007030100), UINT64_C(0x0000000000070301), UINT64_C(0x0000000000070300), UINT64_C(0x0000000000000703),
	UINT64_C(0x0000000007020100), UINT64_C(0x0000000000070201), UINT64_C(0x0000000000070200), UINT64_C(0x0000000000000702),
	UINT64_C(0x0000000000070100), UINT64_C(0x0000000000000701), UINT64_C(0x0000000000000700), UINT64_C(0x0000000000000007),
	UINT64_C(0x0006050403020100), UINT64_C(0x0000060504030201), UINT64_C(0x0000060504030200), UINT64_C(0x0000000605040302),
	UINT64_C(0x0000060504030100), UINT64_C(0x0000000605040301), UINT64_C(0x0000000605040300), UINT64_C(0x0000000006050403),
	UINT64_C(0x0000060504020100), UINT64_C(0x0000000605040201), UINT64_C(0x0000000605040200), UINT64_C(0x0000000006050402),
	UINT64_C(0x0000000605040100), UINT64_C(0x0000000006050401), UINT64_C(0x0000000006050400), UINT64_C(0x0000000000060504),
	UINT64_C(0x0000060503020100), UINT64_C(0x0000000605030201), UINT64_C(0x0000000605030200), UINT64_C(0x0000000006050302),
	UINT64_C(0x0000000605030100), UINT64_C(0x0000000006050301), UINT64_C(0x0000000006050300), UINT64_C(0x0000000000060503),
	UINT64_C(0x0000000605020100), UINT64_C(0x0000000006050201), UINT64_C(0x0000000006050200), UINT64_C(0x0000000000060502),
	UINT64_C(0x0000000006050100), UINT64_C(0x0000000000060501), UINT64_C(0x0000000000060500), UINT64_C(0x0000000000000605),
	UINT64_C(0x0000060403020100), UINT64_C(0x0000000604030201), UINT64_C(0x0000000604030200), UINT64_C(0x0000000006040302),
	UINT64_C(0x0000000604030100), UINT64_C(0x0000000006040301), UINT64_C(0x0000000006040300), UINT64_C(0x0000000000060403),
	UINT64_C(0x0000000604020100), UINT64_C(0x0000000006040201), UINT64_C(0x0000000006040200), UINT64_C(0x0000000000060402),
	UINT64_C(0x0000000006040100), UINT64_C(0x0000000000060401), UINT64_C(0x0000000000060400), UINT64_C(0x0000000000000604),
	UINT64_C(0x0000000603020100), UINT64_C(0x0000000006030201), UINT64_C(0x0000000006030200), UINT64_C(0x0000000000060302),
	UINT64_C(0x0000000006030100), UINT64_C(0x0000000000060301), UINT64_C(0x0000000000060300), UINT64_C(0x0000000000000603),
	UINT64_C(0x0000000006020100), UINT64_C(0x0000000000060201), UINT64_C(0x0000000000060200), UINT64_C(0x0000000000000602),
	UINT64_C(0x0000000000060100), UINT64_C(0x0000000000000601), UINT64_C(0x0000000000000600), UINT64_C(0x0000000000000006),
	UINT64_C(0x0000050403020100), UINT64_C(0x0000000504030201), UINT64_C(0x0000000504030200), UINT64_C(0x0000000005040302),
	UINT64_C(0x0000000504030100), UINT64_C(0x0000000005040301), UINT64_C(0x0000000005040300), UINT64_C(0x0000000000050403),
	UINT64_C(0x0000000504020100), UINT64_C(0x0000000005040201), UINT64_C(0x0000000005040200), UINT64_C(0x0000000000050402),
	UINT64_C(0x0000000005040100), UINT64_C(0x0000000000050401), UINT64_C(0x0000000000050400), UINT64_C(0x0000000000000504),
	UINT64_C(0x0000000503020100), UINT64_C(0x0000000005030201), UINT64_C(0x0000000005030200), UINT64_C(0x0000000000050302),
	UINT64_C(0x0000000005030100), UINT64_C(0x0000000000050301), UINT64_C(0x0000000000050300), UINT64_C(0x0000000000000503),
	UINT64_C(0x0000000005020100), UINT64_C(0x0000000000050201), UINT64_C(0x0000000000050200), UINT64_C(0x0000000000000502),
	UINT64_C(0x0000000000050100), UINT64_C(0x0000000000000501), UINT64_C(0x0000000000000500), UINT64_C(0x0000000000000005),
	UINT64_C(0x0000000403020100), UINT64_C(0x0000000004030201), UINT64_C(0x0000000004030200), UINT64_C(0x0000000000040302),
	UINT64_C(0x0000000004030100), UINT64_C(0x0000000000040301), UINT64_C(0x0000000000040300), UINT64_C(0x0000000000000403),
	UINT64_C(0x0000000004020100), UINT64_C(0x0000000000040201), UINT64_C(0x0000000000040200), UINT64_C(0x0000000000000402),
	UINT64_C(0x0000000000040100), UINT64_C(0x0000000000000401), UINT64_C(0x0000000000000400), UINT64_C(0x0000000000000004),
	UINT64_C(0x0000000003020100), UINT64_C(0x0000000000030201), UINT64_C(0x0000000000030200), UINT64_C(0x0000000000000302),
	UINT64_C(0x0000000000030100), UINT64_C(0x0000000000000301), UINT64_C(0x0000000000000300), UINT64_C(0x0000000000000003),
	UINT64_C(0x0000000000020100), UINT64_C(0x0000000000000201), UINT64_C(0x0000000000000200), UINT64_C(0x0000000000000002),
	UINT64_C(0x0000000000000100), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
};

/* Row k: a byte shuffle moving the high half of a vector right after the first
 * k bytes of its low half. */
static const uint8_t php_base64_combine[9][16] = {
	{0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x01, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff},
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff},
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff},
	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
};

/* For each byte, the number of its bits that are not set. */
#define PHP_BASE64_B2(n) n, n - 1, n - 1, n - 2
#define PHP_BASE64_B4(n) PHP_BASE64_B2(n), PHP_BASE64_B2(n - 1), PHP_BASE64_B2(n - 1), PHP_BASE64_B2(n - 2)
#define PHP_BASE64_B6(n) PHP_BASE64_B4(n), PHP_BASE64_B4(n - 1), PHP_BASE64_B4(n - 1), PHP_BASE64_B4(n - 2)
static const uint8_t php_base64_zeros[256] = {
	PHP_BASE64_B6(8), PHP_BASE64_B6(7), PHP_BASE64_B6(7), PHP_BASE64_B6(6)
};

/* Copy n < 64 bytes with a few overlapping copies of fixed size. */
static zend_always_inline void php_base64_copy_short(unsigned char *dst, const unsigned char *src, size_t n)
{
	if (n >= 32) {
		memcpy(dst, src, 32);
		memcpy(dst + n - 32, src + n - 32, 32);
	} else if (n >= 16) {
		memcpy(dst, src, 16);
		memcpy(dst + n - 16, src + n - 16, 16);
	} else if (n >= 8) {
		memcpy(dst, src, 8);
		memcpy(dst + n - 8, src + n - 8, 8);
	} else if (n >= 4) {
		memcpy(dst, src, 4);
		memcpy(dst + n - 4, src + n - 4, 4);
	} else if (n > 0) {
		dst[0] = src[0];
		dst[n / 2] = src[n / 2];
		dst[n - 1] = src[n - 1];
	}
}

/* The position of the lowest bit set in x, which is not zero. */
static zend_always_inline size_t php_base64_ctz64(uint64_t x)
{
#if SIZEOF_ZEND_LONG == 8
	return zend_ulong_ntz(x);
#else
	return (uint32_t)x ? zend_ulong_ntz((uint32_t)x) : 32 + zend_ulong_ntz((uint32_t)(x >> 32));
#endif
}

/* Pending values are decoded in batches, so that the stores into the buffer
 * are complete when they are read back. */
#define PHP_BASE64_DECODE_BATCH 512

/* The body of the decoder for an instruction set, built on the functions
 * php_base64_decode_<isa>_translate(), _match(), _compress() and _pack(), which
 * work on a block of 64 bytes held in a php_base64_<isa>_block. If pairs is
 * true, clean input is first decoded two blocks at a time: this helps with
 * 16-byte and 32-byte vectors, but not with 64-byte vectors on Ice Lake. */
#define PHP_BASE64_DECODE_SIMD(isa, pairs) \
	const unsigned char *c = str, *const end = str + length; \
	zend_string *result = zend_string_alloc(length, 0); \
	unsigned char *const out = (unsigned char *)ZSTR_VAL(result); \
	unsigned char *o = out; \
	/* Up to 63 values more than a batch, then room for a compressed block. */ \
	php_base64_##isa##_block pending[PHP_BASE64_DECODE_BATCH / 64 + 2]; \
	uint8_t *const buffer = (uint8_t *)pending; \
	unsigned char last[64]; \
	size_t buffered = 0, k; \
	int padding = 0; \
	php_base64_##isa##_block b, b2; \
	uint64_t bad, good, in_mask; \
	\
	/* Two blocks at a time while there are no pending values and no byte to skip. */ \
	while ((pairs) && end - c >= 128 && !(php_base64_decode_##isa##_translate(&b, c) | php_base64_decode_##isa##_translate(&b2, c + 64))) { \
		php_base64_decode_##isa##_pack(&b, o); \
		php_base64_decode_##isa##_pack(&b2, o + 48); \
		o += 96; \
		c += 128; \
	} \
	/* One block at a time while there are no pending values and no byte to skip: \
	 * testing for pending values in the loop below costs a cycle per block. */ \
	while (end - c >= 64 && !php_base64_decode_##isa##_translate(&b, c)) { \
		php_base64_decode_##isa##_pack(&b, o); \
		o += 48; \
		c += 64; \
	} \
	while (end - c >= 64) { \
		bad = php_base64_decode_##isa##_translate(&b, c); \
		if (EXPECTED(bad == 0 && buffered == 0)) { \
			php_base64_decode_##isa##_pack(&b, o); \
			o += 48; \
		} else { \
			if (strict && bad && (bad & ~php_base64_decode_##isa##_match(c, php_base64_ws_set))) { \
				return php_base64_decode_resume(result, o, buffer, buffered, c, end - c); \
			} \
			buffered += php_base64_decode_##isa##_compress(&b, bad, buffer + buffered); \
			if (buffered >= PHP_BASE64_DECODE_BATCH) { \
				for (k = 0; k + 64 <= buffered; k += 64) { \
					php_base64_decode_##isa##_pack(&pending[k / 64], o); \
					o += 48; \
				} \
				buffered -= k; \
				pending[0] = pending[k / 64]; \
			} \
		} \
		c += 64; \
	} \
	\
	/* Without pending values, the last 0 to 63 bytes are decoded 16 at a time while \
	 * they are in the alphabet, and the rest one character at a time. Writing 16 \
	 * bytes for 12 decoded bytes is safe, as 16 more bytes have been read. */ \
	if (buffered == 0) { \
		size_t i = 0, pad = 0, outl; \
		while (end - c >= 16 && php_base64_decode_##isa##_step16(c, o)) { \
			c += 16; \
			o += 12; \
		} \
		outl = o - out; \
		if (!php_base64_decode_loop(c, end - c, out, &outl, &i, &pad, strict) \
				|| !php_base64_decode_finish(out, outl, i, pad, strict)) { \
			goto fail; \
		} \
		ZSTR_LEN(result) = outl; \
		return result; \
	} \
	\
	/* The last 0 to 63 bytes, completed with zero bytes, which are not in the alphabet. */ \
	memset(last, 0, sizeof(last)); \
	php_base64_copy_short(last, c, end - c); \
	in_mask = ((uint64_t)1 << (end - c)) - 1; \
	bad = php_base64_decode_##isa##_translate(&b, last) & in_mask; \
	good = ~bad & in_mask; \
	if (strict && bad) { \
		padding = php_base64_decode_padding(good, bad, \
			php_base64_decode_##isa##_match(last, php_base64_ws_set), \
			php_base64_decode_##isa##_match(last, php_base64_pad_set)); \
		if (padding < 0) { \
			goto fail; \
		} \
	} \
	buffered += php_base64_decode_##isa##_compress(&b, ~good, buffer + buffered); \
	for (k = 0; k + 64 <= buffered; k += 64) { \
		php_base64_decode_##isa##_pack(&pending[k / 64], o); \
		o += 48; \
	} \
	buffered -= k; \
	\
	/* The last values: a trailing group of 2 or 3 gives 1 or 2 bytes. */ \
	memset(buffer + k + buffered, 0, 64); \
	php_base64_decode_##isa##_pack(&pending[k / 64], last); \
	k = (buffered / 4) * 3 + (buffered % 4 ? buffered % 4 - 1 : 0); \
	php_base64_copy_short(o, last, k); \
	o += k; \
	if (!php_base64_decode_finish(out, o - out, buffered, padding, strict)) { \
		goto fail; \
	} \
	ZSTR_LEN(result) = o - out; \
	return result; \
	\
fail: \
	zend_string_efree(result); \
	return NULL;
#endif

#if defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PTR)
zend_string *php_base64_encode_avx512_vbmi(const unsigned char *str, size_t length, zend_long flags)
{
	const unsigned char *c = str;
	unsigned char *o;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	const __m512i shuffle_splitting = _mm512_setr_epi32(
		0x01020001, 0x04050304, 0x07080607, 0x0a0b090a, 0x0d0e0c0d, 0x10110f10,
		0x13141213, 0x16171516, 0x191a1819, 0x1c1d1b1c, 0x1f201e1f, 0x22232122,
		0x25262425, 0x28292728, 0x2b2c2a2b, 0x2e2f2d2e);
	const __m512i multi_shifts = _mm512_set1_epi64(0x3036242a1016040a);
	const char *ascii_lookup_tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const __m512i ascii_lookup = _mm512_loadu_si512((__m512i *)ascii_lookup_tbl);

	/* Each step consumes 48 bytes and writes 64 bytes, which the output
	 * buffer always has room for. Only the first 48 loaded bytes are used:
	 * the main loop loads 64 bytes while that stays within the input, and a
	 * final step uses a masked load to avoid reading past the input. */
#define PHP_BASE64_ENCODE_AVX512_VBMI_STEP(load) do { \
		__m512i str = (load); \
		/* splitting 24-bit words into 32-bit lanes */ \
		str = _mm512_permutexvar_epi8(shuffle_splitting, str); \
		/* moving 6-bit word to separate bytes */ \
		str = _mm512_multishift_epi64_epi8(multi_shifts, str); \
		/* conversion to ASCII */ \
		str = _mm512_permutexvar_epi8(str, ascii_lookup); \
		_mm512_storeu_si512((__m512i *)o, str); \
		c += 48; \
		o += 64; \
		length -= 48; \
	} while (0)

	while (length >= 64) {
		PHP_BASE64_ENCODE_AVX512_VBMI_STEP(_mm512_loadu_si512((const void *)c));
	}
	if (length >= 48) {
		PHP_BASE64_ENCODE_AVX512_VBMI_STEP(_mm512_maskz_loadu_epi8((__mmask64)0xffffffffffff, (const void *)c));
	}
#undef PHP_BASE64_ENCODE_AVX512_VBMI_STEP

	/* The last 0 to 47 bytes: masked load, padding with '=', masked store. */
	if (length > 0) {
		const size_t full_len = ((length + 2) / 3) * 4;
		const size_t data_len = full_len - (3 - length % 3) % 3;
		const size_t out_len = (flags & PHP_BASE64_NO_PADDING) ? data_len : full_len;

		__m512i str = _mm512_maskz_loadu_epi8(((__mmask64)1 << length) - 1, (const void *)c);
		str = _mm512_permutexvar_epi8(shuffle_splitting, str);
		str = _mm512_multishift_epi64_epi8(multi_shifts, str);
		/* data_len and out_len are 2 to 64 */
		str = _mm512_mask_permutexvar_epi8(_mm512_set1_epi8(base64_pad),
			UINT64_MAX >> (64 - data_len), str, ascii_lookup);
		_mm512_mask_storeu_epi8((void *)o, UINT64_MAX >> (64 - out_len), str);
		o += out_len;
	}
	*o = '\0';

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}

#endif

#if defined(BASE64_INTRIN_AVX512_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_FUNC_PTR)
zend_string *php_base64_encode_avx512(const unsigned char *str, size_t length, zend_long flags)
{
	const unsigned char *c = str;
	unsigned char *o;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	while (length > 63) {
		/* Step 1: load input data */
		/* [????|????|????|????|PPPO|OONN|NMMM|LLLK|KKJJ|JIII|HHHG|GGFF|FEEE|DDDC|CCBB|BAAA] */
		__m512i str = _mm512_loadu_si512((const __m512i *)c);

		/* Step 2: splitting 24-bit words into 32-bit lanes */
		/* [0000|PPPO|OONN|NMMM|0000|LLLK|KKJJ|JIII|0000|HHHG|GGFF|FEEE|0000|DDDC|CCBB|BAAA] */
		str = _mm512_permutexvar_epi32(
			_mm512_set_epi32(-1, 11, 10, 9, -1, 8, 7, 6, -1, 5, 4, 3, -1, 2, 1, 0), str);
		/* [D1 D2 D0 D1|C1 C2 C0 C1|B1 B2 B0 B1|A1 A2 A0 A1] x 4 */
		str = _mm512_shuffle_epi8(str, _mm512_set4_epi32(0x0a0b090a, 0x07080607, 0x04050304, 0x01020001));

		/* Step 3: moving 6-bit word to separate bytes */
		/* in:  [bbbbcccc|ccdddddd|aaaaaabb|bbbbcccc] */
		/* t0:  [0000cccc|cc000000|aaaaaa00|00000000] */
		const __m512i t0 = _mm512_and_si512(str, _mm512_set1_epi32(0x0fc0fc00));
		/* t1:  [00000000|00cccccc|00000000|00aaaaaa] */
		const __m512i t1 = _mm512_srlv_epi16(t0, _mm512_set1_epi32(0x0006000a));
		/* t2:  [ccdddddd|00000000|aabbbbbb|cccc0000] */
		const __m512i t2 = _mm512_sllv_epi16(str, _mm512_set1_epi32(0x00080004));
		/* str: [00dddddd|00cccccc|00bbbbbb|00aaaaaa] */
		str = _mm512_ternarylogic_epi32(_mm512_set1_epi32(0x3f003f00), t2, t1, 0xca);

		/* Step 4: conversion to ASCII */
		__m512i result = _mm512_subs_epu8(str, _mm512_set1_epi8(51));
		const __mmask64 less = _mm512_cmpgt_epi8_mask(_mm512_set1_epi8(26), str);
		result = _mm512_mask_mov_epi8(result, less, _mm512_set1_epi8(13));
		const __m512i lut = _mm512_set4_epi32(0x000041f0, 0xedfcfcfc, 0xfcfcfcfc, 0xfcfcfc47);
		result = _mm512_shuffle_epi8(lut, result);
		result = _mm512_add_epi8(result, str);

		/* Step 5: store the final result */
		_mm512_storeu_si512((__m512i *)o, result);
		c += 48;
		o += 64;
		length -= 48;
	}

	o = php_base64_encode_impl(c, length, o, flags);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}
#endif

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
# if defined(ZEND_INTRIN_AVX2_RESOLVER) && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
static __m256i php_base64_encode_avx2_reshuffle(__m256i in) __attribute__((target("avx2")));
static __m256i php_base64_encode_avx2_translate(__m256i in) __attribute__((target("avx2")));
# endif
static __m256i php_base64_encode_avx2_reshuffle(__m256i in)
{
	/* This one works with shifted (4 bytes) input in order to
	 * be able to work efficiently in the 2 128-bit lanes */
	__m256i t0, t1, t2, t3;

	/* input, bytes MSB to LSB:
	 * 0 0 0 0 x w v u t s r q p o n m
	 * l k j i h g f e d c b a 0 0 0 0 */
	in = _mm256_shuffle_epi8(in, _mm256_set_epi8(
		10, 11,  9, 10,
		 7,  8,  6,  7,
		 4,  5,  3,  4,
		 1,  2,  0,  1,

		14, 15, 13, 14,
		11, 12, 10, 11,
		 8,  9,  7,  8,
		 5,  6,  4,  5));

	t0 = _mm256_and_si256(in, _mm256_set1_epi32(0x0fc0fc00));

	t1 = _mm256_mulhi_epu16(t0, _mm256_set1_epi32(0x04000040));

	t2 = _mm256_and_si256(in, _mm256_set1_epi32(0x003f03f0));

	t3 = _mm256_mullo_epi16(t2, _mm256_set1_epi32(0x01000010));

	return _mm256_or_si256(t1, t3);
	/* 00xxxxxx 00wwwwXX 00vvWWWW 00VVVVVV
	 * 00uuuuuu 00ttttUU 00ssTTTT 00SSSSSS
	 * 00rrrrrr 00qqqqRR 00ppQQQQ 00PPPPPP
	 * 00oooooo 00nnnnOO 00mmNNNN 00MMMMMM
	 * 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	 * 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	 * 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	 * 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA */
}

static __m256i php_base64_encode_avx2_translate(__m256i in)
{
	__m256i lut, indices, mask;

	lut = _mm256_setr_epi8(
			65, 71, -4, -4, -4, -4, -4, -4,
			-4, -4, -4, -4, -19, -16, 0, 0,
			65, 71, -4, -4, -4, -4, -4, -4,
			-4, -4, -4, -4, -19, -16, 0, 0);

	indices = _mm256_subs_epu8(in, _mm256_set1_epi8(51));

	mask = _mm256_cmpgt_epi8(in, _mm256_set1_epi8(25));

	indices = _mm256_sub_epi8(indices, mask);

	return _mm256_add_epi8(in, _mm256_shuffle_epi8(lut, indices));

}
#endif /* ZEND_INTRIN_AVX2_NATIVE || (ZEND_INTRIN_AVX2_RESOLVER && !ZEND_INTRIN_SSSE3_NATIVE) */

#if defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)

# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
static __m128i php_base64_encode_ssse3_reshuffle(__m128i in) __attribute__((target("ssse3")));
static __m128i php_base64_encode_ssse3_translate(__m128i in) __attribute__((target("ssse3")));
# endif

static __m128i php_base64_encode_ssse3_reshuffle(__m128i in)
{
	__m128i t0, t1, t2, t3;

	/* input, bytes MSB to LSB:
	 * 0 0 0 0 l k j i h g f e d c b a */
	in = _mm_shuffle_epi8(in, _mm_set_epi8(
				10, 11,  9, 10,
				7,  8,  6,  7,
				4,  5,  3,  4,
				1,  2,  0,  1));

	t0 = _mm_and_si128(in, _mm_set1_epi32(0x0fc0fc00));

	t1 = _mm_mulhi_epu16(t0, _mm_set1_epi32(0x04000040));

	t2 = _mm_and_si128(in, _mm_set1_epi32(0x003f03f0));

	t3 = _mm_mullo_epi16(t2, _mm_set1_epi32(0x01000010));

	/* output (upper case are MSB, lower case are LSB):
	 * 00llllll 00kkkkLL 00jjKKKK 00JJJJJJ
	 * 00iiiiii 00hhhhII 00ggHHHH 00GGGGGG
	 * 00ffffff 00eeeeFF 00ddEEEE 00DDDDDD
	 * 00cccccc 00bbbbCC 00aaBBBB 00AAAAAA */
	return _mm_or_si128(t1, t3);
}

static __m128i php_base64_encode_ssse3_translate(__m128i in)
{
	__m128i mask, indices;
	__m128i lut = _mm_setr_epi8(
			65,  71, -4, -4,
			-4,  -4, -4, -4,
			-4,  -4, -4, -4,
			-19, -16,  0,  0
			);

	/* Translate values 0..63 to the Base64 alphabet. There are five sets:
	 * #  From      To         Abs    Index  Characters
	 * 0  [0..25]   [65..90]   +65        0  ABCDEFGHIJKLMNOPQRSTUVWXYZ
	 * 1  [26..51]  [97..122]  +71        1  abcdefghijklmnopqrstuvwxyz
	 * 2  [52..61]  [48..57]    -4  [2..11]  0123456789
	 * 3  [62]      [43]       -19       12  +
	 * 4  [63]      [47]       -16       13  / */

	/* Create LUT indices from input:
	 * the index for range #0 is right, others are 1 less than expected: */
	indices = _mm_subs_epu8(in, _mm_set1_epi8(51));

	/* mask is 0xFF (-1) for range #[1..4] and 0x00 for range #0: */
	mask = _mm_cmpgt_epi8(in, _mm_set1_epi8(25));

	/* subtract -1, so add 1 to indices for range #[1..4], All indices are now correct: */
	indices = _mm_sub_epi8(indices, mask);

	/* Add offsets to input values: */
	return _mm_add_epi8(in, _mm_shuffle_epi8(lut, indices));
}

#define PHP_BASE64_ENCODE_SSSE3_LOOP				\
	while (length > 15) {							\
		__m128i s = _mm_loadu_si128((__m128i *)c);	\
													\
		s = php_base64_encode_ssse3_reshuffle(s);	\
													\
		s = php_base64_encode_ssse3_translate(s);	\
													\
		_mm_storeu_si128((__m128i *)o, s);			\
		c += 12;									\
		o += 16;									\
		length -= 12;								\
	}

#endif /* ZEND_INTRIN_SSSE3_NATIVE || (ZEND_INTRIN_SSSE3_RESOLVER && !ZEND_INTRIN_AVX2_NATIVE) */

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
# if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_SSSE3_NATIVE)
PHPAPI zend_string *php_base64_encode_ex(const unsigned char *str, size_t length, zend_long flags)
# elif defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_encode_avx2(const unsigned char *str, size_t length, zend_long flags)
# else /* ZEND_INTRIN_SSSE3_RESOLVER */
zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length, zend_long flags)
# endif
{
	const unsigned char *c = str;
	unsigned char *o;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	o = (unsigned char *)ZSTR_VAL(result);
# if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
	if (length > 31) {
		__m256i s = _mm256_loadu_si256((__m256i *)c);

		s = _mm256_permutevar8x32_epi32(s, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6));

		for (;;) {
			s = php_base64_encode_avx2_reshuffle(s);

			s = php_base64_encode_avx2_translate(s);

			_mm256_storeu_si256((__m256i *)o, s);
			c += 24;
			o += 32;
			length -= 24;
			if (length < 28) {
				break;
			}
			s = _mm256_loadu_si256((__m256i *)(c - 4));
		}
	}
# else
	PHP_BASE64_ENCODE_SSSE3_LOOP;
# endif

	o = php_base64_encode_impl(c, length, o, flags);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}

# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length, zend_long flags)
{
	const unsigned char *c = str;
	unsigned char *o;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	PHP_BASE64_ENCODE_SSSE3_LOOP;

	o = php_base64_encode_impl(c, length, o, flags);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}
# endif
#endif /* ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER */

/* }}} */

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
# if defined(HAVE_FUNC_ATTRIBUTE_TARGET) && !defined(__SSSE3__)
#  define PHP_BASE64_SSSE3_TARGET __attribute__((target("ssse3")))
# else
#  define PHP_BASE64_SSSE3_TARGET
# endif
# if defined(HAVE_FUNC_ATTRIBUTE_TARGET) && !defined(__AVX2__)
#  define PHP_BASE64_AVX2_TARGET __attribute__((target("avx2")))
# else
#  define PHP_BASE64_AVX2_TARGET
# endif

/* Translation from ASCII to 6-bit values (credit: aqrit, via simdutf): two
 * hashes of each byte index a table of offsets to add, and a table of checks
 * that give a negative sum for the bytes outside the alphabet. */
static const uint8_t php_base64_delta_asso[16] = {
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x0f
};
static const uint8_t php_base64_delta_values[16] = {
	0x00, 0x00, 0x00, 0x13, 0x04, 0xbf, 0xbf, 0xb9, 0xb9, 0x00, 0x10, 0xc3, 0xbf, 0xbf, 0xb9, 0xb9
};
static const uint8_t php_base64_check_asso[16] = {
	0x0d, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x07, 0x0b, 0x0b, 0x0b, 0x0f
};
static const uint8_t php_base64_check_values[16] = {
	0x80, 0x80, 0x80, 0x80, 0xcf, 0xbf, 0xd5, 0xa6, 0xb5, 0x86, 0xd1, 0x80, 0xb1, 0x80, 0x91, 0x80
};

#define PHP_BASE64_LUT16(t) _mm_loadu_si128((const __m128i *)(t))

/* Translate 16 bytes to 6-bit values; returns the mask of the bytes outside the alphabet. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET uint64_t php_base64_decode_translate16(const unsigned char *in, __m128i *values)
{
	const __m128i s = _mm_loadu_si128((const __m128i *)in);
	const __m128i shifted = _mm_srli_epi32(s, 3);
	const __m128i delta_hash = _mm_avg_epu8(_mm_shuffle_epi8(PHP_BASE64_LUT16(php_base64_delta_asso), s), shifted);
	const __m128i check_hash = _mm_avg_epu8(_mm_shuffle_epi8(PHP_BASE64_LUT16(php_base64_check_asso), s), shifted);

	*values = _mm_adds_epi8(_mm_shuffle_epi8(PHP_BASE64_LUT16(php_base64_delta_values), delta_hash), s);

	return (uint64_t)_mm_movemask_epi8(_mm_adds_epi8(_mm_shuffle_epi8(PHP_BASE64_LUT16(php_base64_check_values), check_hash), s));
}

/* The mask of the 16 bytes in a set (see php_base64_ws_set). */
static zend_always_inline PHP_BASE64_SSSE3_TARGET uint64_t php_base64_decode_match16(const unsigned char *in, const uint8_t *set)
{
	const __m128i s = _mm_loadu_si128((const __m128i *)in);

	return (uint64_t)_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_shuffle_epi8(_mm_loadu_si128((const __m128i *)set), s), s));
}

/* Remove the bytes of v whose bit is set in the low 16 bits of bad, given the
 * number of bytes kept from the low 8. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET __m128i php_base64_decode_compress16(__m128i v, uint64_t bad, size_t kept)
{
	if ((bad & 0xffff) == 0) {
		return v;
	}
	/* Pack each half, then move the high half after what is kept of the low half. */
	v = _mm_shuffle_epi8(v, _mm_set_epi64x(
		(long long)(php_base64_thintable[(bad >> 8) & 0xff] + UINT64_C(0x0808080808080808)),
		(long long)php_base64_thintable[bad & 0xff]));

	return _mm_shuffle_epi8(v, _mm_loadu_si128((const __m128i *)php_base64_combine[kept]));
}

/* Store the bytes of v whose bit is not set in the low 16 bits of bad; returns how many. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET size_t php_base64_decode_store16(__m128i v, uint64_t bad, uint8_t *out)
{
	const size_t kept = php_base64_zeros[bad & 0xff];

	_mm_storeu_si128((__m128i *)out, php_base64_decode_compress16(v, bad, kept));

	return kept + php_base64_zeros[(bad >> 8) & 0xff];
}

/* Store the bytes of v0 to v3 but the one at pos; returns how many. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET size_t php_base64_decode_remove1(__m128i v0, __m128i v1, __m128i v2, __m128i v3, size_t pos, uint8_t *out)
{
	const __m128i iota = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	/* Move the bytes after pos down by one, in the vector holding pos. */
	const __m128i shift = _mm_sub_epi8(iota, _mm_cmpgt_epi8(iota, _mm_set1_epi8((char)(pos & 15) - 1)));
	const size_t i = pos >> 4;

	_mm_storeu_si128((__m128i *)out, i == 0 ? _mm_shuffle_epi8(v0, shift) : v0);
	_mm_storeu_si128((__m128i *)(out + 16 - (i < 1)), i == 1 ? _mm_shuffle_epi8(v1, shift) : v1);
	_mm_storeu_si128((__m128i *)(out + 32 - (i < 2)), i == 2 ? _mm_shuffle_epi8(v2, shift) : v2);
	_mm_storeu_si128((__m128i *)(out + 48 - (i < 3)), i == 3 ? _mm_shuffle_epi8(v3, shift) : v3);

	return 63;
}

/* Store the bytes of v0 to v3 whose bit is not set in bad; returns how many. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET size_t php_base64_decode_compress64(__m128i v0, __m128i v1, __m128i v2, __m128i v3, uint64_t bad, uint8_t *out)
{
	size_t n;

	if (bad && !(bad & (bad - 1))) {
		return php_base64_decode_remove1(v0, v1, v2, v3, php_base64_ctz64(bad), out);
	}
	n = php_base64_decode_store16(v0, bad, out);
	n += php_base64_decode_store16(v1, bad >> 16, out + n);
	n += php_base64_decode_store16(v2, bad >> 32, out + n);

	return n + php_base64_decode_store16(v3, bad >> 48, out + n);
}

/* Pack the 6-bit values in 32-bit lanes into 12 bytes, followed by 4 zero bytes. */
static zend_always_inline PHP_BASE64_SSSE3_TARGET __m128i php_base64_decode_pack16(__m128i in)
{
	const __m128i merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	/* 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	 * 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	 * 0000eeee FFffffff 0000DDDD DDddEEEE
	 * 0000bbbb CCcccccc 0000AAAA AAaaBBBB */
	const __m128i out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	/* 00000000 JJJJJJjj KKKKkkkk LLllllll
	 * 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	 * 00000000 DDDDDDdd EEEEeeee FFffffff
	 * 00000000 AAAAAAaa BBBBbbbb CCcccccc */
	return _mm_shuffle_epi8(out, _mm_setr_epi8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));
	/* 00000000 00000000 00000000 00000000
	 * LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	 * HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	 * DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa */
}

/* If the 16 bytes at in are in the alphabet, decode them (writing 16 bytes). */
static zend_always_inline PHP_BASE64_SSSE3_TARGET bool php_base64_decode_step16(const unsigned char *in, unsigned char *out)
{
	__m128i values;

	if (php_base64_decode_translate16(in, &values)) {
		return false;
	}
	_mm_storeu_si128((__m128i *)out, php_base64_decode_pack16(values));

	return true;
}
#define php_base64_decode_ssse3_step16 php_base64_decode_step16
#define php_base64_decode_avx2_step16 php_base64_decode_step16
#endif

#if defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
typedef struct {
	__m128i v[4];
} php_base64_ssse3_block;

static zend_always_inline PHP_BASE64_SSSE3_TARGET uint64_t php_base64_decode_ssse3_translate(php_base64_ssse3_block *b, const unsigned char *in)
{
	return php_base64_decode_translate16(in, &b->v[0]) | php_base64_decode_translate16(in + 16, &b->v[1]) << 16
		| php_base64_decode_translate16(in + 32, &b->v[2]) << 32 | php_base64_decode_translate16(in + 48, &b->v[3]) << 48;
}

static zend_always_inline PHP_BASE64_SSSE3_TARGET uint64_t php_base64_decode_ssse3_match(const unsigned char *in, const uint8_t *set)
{
	return php_base64_decode_match16(in, set) | php_base64_decode_match16(in + 16, set) << 16
		| php_base64_decode_match16(in + 32, set) << 32 | php_base64_decode_match16(in + 48, set) << 48;
}

static zend_always_inline PHP_BASE64_SSSE3_TARGET size_t php_base64_decode_ssse3_compress(const php_base64_ssse3_block *b, uint64_t bad, uint8_t *out)
{
	return php_base64_decode_compress64(b->v[0], b->v[1], b->v[2], b->v[3], bad, out);
}

static zend_always_inline PHP_BASE64_SSSE3_TARGET void php_base64_decode_ssse3_pack(const php_base64_ssse3_block *b, unsigned char *out)
{
	_mm_storeu_si128((__m128i *)out, php_base64_decode_pack16(b->v[0]));
	_mm_storeu_si128((__m128i *)(out + 12), php_base64_decode_pack16(b->v[1]));
	_mm_storeu_si128((__m128i *)(out + 24), php_base64_decode_pack16(b->v[2]));
	_mm_storeu_si128((__m128i *)(out + 36), php_base64_decode_pack16(b->v[3]));
}
#endif

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
typedef struct {
	__m256i v[2];
} php_base64_avx2_block;

#define PHP_BASE64_LUT32(t) _mm256_broadcastsi128_si256(_mm_loadu_si128((const __m128i *)(t)))

static zend_always_inline PHP_BASE64_AVX2_TARGET uint64_t php_base64_decode_translate32(const unsigned char *in, __m256i *values)
{
	const __m256i s = _mm256_loadu_si256((const __m256i *)in);
	const __m256i shifted = _mm256_srli_epi32(s, 3);
	const __m256i delta_hash = _mm256_avg_epu8(_mm256_shuffle_epi8(PHP_BASE64_LUT32(php_base64_delta_asso), s), shifted);
	const __m256i check_hash = _mm256_avg_epu8(_mm256_shuffle_epi8(PHP_BASE64_LUT32(php_base64_check_asso), s), shifted);

	*values = _mm256_adds_epi8(_mm256_shuffle_epi8(PHP_BASE64_LUT32(php_base64_delta_values), delta_hash), s);

	return (uint32_t)_mm256_movemask_epi8(_mm256_adds_epi8(_mm256_shuffle_epi8(PHP_BASE64_LUT32(php_base64_check_values), check_hash), s));
}

static zend_always_inline PHP_BASE64_AVX2_TARGET uint64_t php_base64_decode_match32(const unsigned char *in, const uint8_t *set)
{
	const __m256i s = _mm256_loadu_si256((const __m256i *)in);

	return (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(
		_mm256_shuffle_epi8(_mm256_broadcastsi128_si256(_mm_loadu_si128((const __m128i *)set)), s), s));
}

/* Pack the 6-bit values in 32-bit lanes into 24 bytes, followed by 8 zero bytes. */
static zend_always_inline PHP_BASE64_AVX2_TARGET __m256i php_base64_decode_pack32(__m256i in)
{
	in = _mm256_madd_epi16(_mm256_maddubs_epi16(in, _mm256_set1_epi32(0x01400140)), _mm256_set1_epi32(0x00011000));
	in = _mm256_shuffle_epi8(in, _mm256_setr_epi8(
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1,
		2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));

	return _mm256_permutevar8x32_epi32(in, _mm256_setr_epi32(0, 1, 2, 4, 5, 6, -1, -1));
}

static zend_always_inline PHP_BASE64_AVX2_TARGET uint64_t php_base64_decode_avx2_translate(php_base64_avx2_block *b, const unsigned char *in)
{
	return php_base64_decode_translate32(in, &b->v[0]) | php_base64_decode_translate32(in + 32, &b->v[1]) << 32;
}

static zend_always_inline PHP_BASE64_AVX2_TARGET uint64_t php_base64_decode_avx2_match(const unsigned char *in, const uint8_t *set)
{
	return php_base64_decode_match32(in, set) | php_base64_decode_match32(in + 32, set) << 32;
}

/* AVX2 implies POPCNT. */
static zend_always_inline PHP_BASE64_AVX2_TARGET size_t php_base64_popcount(uint32_t x)
{
#if defined(_MSC_VER) && !defined(__clang__)
	return __popcnt(x);
#else
	return __builtin_popcount(x);
#endif
}

/* Store the bytes of v whose bit is not set in bad; returns how many. */
static zend_always_inline PHP_BASE64_AVX2_TARGET size_t php_base64_decode_compress32(__m256i v, uint32_t bad, uint8_t *out)
{
	if (bad == 0) {
		_mm256_storeu_si256((__m256i *)out, v);
		return 32;
	}
	_mm_storeu_si128((__m128i *)out,
		php_base64_decode_compress16(_mm256_castsi256_si128(v), bad, 8 - php_base64_popcount(bad & 0xff)));
	_mm_storeu_si128((__m128i *)(out + 16 - php_base64_popcount(bad & 0xffff)),
		php_base64_decode_compress16(_mm256_extracti128_si256(v, 1), bad >> 16, 8 - php_base64_popcount((bad >> 16) & 0xff)));

	return 32 - php_base64_popcount(bad);
}

static zend_always_inline PHP_BASE64_AVX2_TARGET size_t php_base64_decode_avx2_compress(const php_base64_avx2_block *b, uint64_t bad, uint8_t *out)
{
	size_t n;

	if (bad && !(bad & (bad - 1))) {
		return php_base64_decode_remove1(_mm256_castsi256_si128(b->v[0]), _mm256_extracti128_si256(b->v[0], 1),
			_mm256_castsi256_si128(b->v[1]), _mm256_extracti128_si256(b->v[1], 1), php_base64_ctz64(bad), out);
	}
	n = php_base64_decode_compress32(b->v[0], (uint32_t)bad, out);

	return n + php_base64_decode_compress32(b->v[1], (uint32_t)(bad >> 32), out + n);
}

static zend_always_inline PHP_BASE64_AVX2_TARGET void php_base64_decode_avx2_pack(const php_base64_avx2_block *b, unsigned char *out)
{
	_mm256_storeu_si256((__m256i *)out, php_base64_decode_pack32(b->v[0]));
	_mm256_storeu_si256((__m256i *)(out + 24), php_base64_decode_pack32(b->v[1]));
}
#endif

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
# if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_SSSE3_NATIVE)
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, bool strict)
# elif defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_decode_ex_avx2(const unsigned char *str, size_t length, bool strict)
# else
zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, bool strict)
# endif
{
# if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
	PHP_BASE64_DECODE_SIMD(avx2, true)
# else
	PHP_BASE64_DECODE_SIMD(ssse3, true)
# endif
}

# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, bool strict)
{
	PHP_BASE64_DECODE_SIMD(ssse3, true)
}
# endif
#endif /* ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER */

#if defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PTR)
#define PHP_BASE64_AVX512_VBMI_TARGET __attribute__((target("avx512f,avx512cd,avx512vl,avx512dq,avx512bw,avx512vbmi,avx512vbmi2,popcnt")))

/* ASCII to 6-bit values, for the 128 ASCII bytes; the others map to 0x80. */
static const uint8_t php_base64_avx512_lookup[128] = {
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x3e, 0x80, 0x80, 0x80, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x80, 0x80, 0x80, 0x80, 0x80
};

typedef struct {
	__m512i v;
} php_base64_avx512_vbmi_block;

static zend_always_inline PHP_BASE64_AVX512_VBMI_TARGET uint64_t php_base64_decode_avx512_vbmi_translate(php_base64_avx512_vbmi_block *b, const unsigned char *in)
{
	const __m512i s = _mm512_loadu_si512((const void *)in);

	b->v = _mm512_permutex2var_epi8(_mm512_loadu_si512((const void *)php_base64_avx512_lookup), s,
		_mm512_loadu_si512((const void *)(php_base64_avx512_lookup + 64)));

	return _mm512_movepi8_mask(_mm512_or_si512(b->v, s));
}

static zend_always_inline PHP_BASE64_AVX512_VBMI_TARGET uint64_t php_base64_decode_avx512_vbmi_match(const unsigned char *in, const uint8_t *set)
{
	const __m512i s = _mm512_loadu_si512((const void *)in);

	return _mm512_cmpeq_epi8_mask(_mm512_shuffle_epi8(_mm512_broadcast_i32x4(_mm_loadu_si128((const __m128i *)set)), s), s);
}

static zend_always_inline PHP_BASE64_AVX512_VBMI_TARGET size_t php_base64_decode_avx512_vbmi_compress(const php_base64_avx512_vbmi_block *b, uint64_t bad, uint8_t *out)
{
	_mm512_storeu_si512((void *)out, _mm512_maskz_compress_epi8(~bad, b->v));

	return 64 - __builtin_popcountll(bad);
}

static zend_always_inline PHP_BASE64_AVX512_VBMI_TARGET void php_base64_decode_avx512_vbmi_pack(const php_base64_avx512_vbmi_block *b, unsigned char *out)
{
	const __m512i s = _mm512_madd_epi16(_mm512_maddubs_epi16(b->v, _mm512_set1_epi32(0x01400140)), _mm512_set1_epi32(0x00011000));

	_mm512_storeu_si512((void *)out, _mm512_permutexvar_epi8(_mm512_setr_epi32(
		0x06000102, 0x090a0405, 0x0c0d0e08, 0x16101112, 0x191a1415, 0x1c1d1e18,
		0x26202122, 0x292a2425, 0x2c2d2e28, 0x36303132, 0x393a3435, 0x3c3d3e38,
		0x00000000, 0x00000000, 0x00000000, 0x00000000), s));
}

#define php_base64_decode_avx512_vbmi_step16 php_base64_decode_step16

zend_string *php_base64_decode_ex_avx512_vbmi(const unsigned char *str, size_t length, bool strict)
{
	PHP_BASE64_DECODE_SIMD(avx512_vbmi, false)
}
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
/* Translation from ASCII to 6-bit values (from simdutf): the entries for the
 * low nibble and for the top five bits of a byte share no bit (or only the bit 0
 * for whitespace) if the byte is outside the alphabet. Otherwise, the position of
 * their highest common bit selects the offset to add. */
static const uint8_t php_base64_neon_lut_lo[16] = {
	0xa9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf9, 0xf1, 0xa2, 0xa1, 0xa1, 0xa0, 0xa4
};
static const uint8_t php_base64_neon_lut_hi[16] = {
	0x00, 0x01, 0x00, 0x00, 0x01, 0x06, 0x08, 0x08, 0x10, 0x20, 0x20, 0x10, 0x40, 0x80, 0x80, 0x40
};
static const uint8_t php_base64_neon_roll[16] = {
	0xb9, 0xb9, 0xbf, 0xbf, 0x04, 0x10, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* For php_base64_neon_movemask() */
static const uint8_t php_base64_neon_bits[16] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

typedef struct {
	uint8x16_t v[4];
} php_base64_neon_block;

/* One bit per byte of m[0..3], each byte being 0x00 or 0xff. */
static zend_always_inline uint64_t php_base64_neon_movemask(const uint8x16_t *m)
{
	const uint8x16_t bits = vld1q_u8(php_base64_neon_bits);
	uint8x16_t s0 = vpaddq_u8(vandq_u8(m[0], bits), vandq_u8(m[1], bits));
	const uint8x16_t s1 = vpaddq_u8(vandq_u8(m[2], bits), vandq_u8(m[3], bits));

	s0 = vpaddq_u8(s0, s1);
	s0 = vpaddq_u8(s0, s0);

	return vgetq_lane_u64(vreinterpretq_u64_u8(s0), 0);
}

/* Translate 16 bytes to 6-bit values; returns 0 or 1 for the bytes outside the alphabet. */
static zend_always_inline uint8x16_t php_base64_decode_translate16(const unsigned char *in, uint8x16_t *values)
{
	const uint8x16_t s = vld1q_u8(in);
	const uint8x16_t res = vandq_u8(vqtbl1q_u8(vld1q_u8(php_base64_neon_lut_lo), vandq_u8(s, vdupq_n_u8(0x0f))),
		vqtbl1q_u8(vld1q_u8(php_base64_neon_lut_hi), vshrq_n_u8(s, 3)));

	*values = vaddq_u8(s, vqtbl1q_u8(vld1q_u8(php_base64_neon_roll), vclzq_u8(res)));

	return res;
}

static zend_always_inline uint64_t php_base64_decode_neon_translate(php_base64_neon_block *b, const unsigned char *in)
{
	uint8x16_t res[4];

	res[0] = php_base64_decode_translate16(in, &b->v[0]);
	res[1] = php_base64_decode_translate16(in + 16, &b->v[1]);
	res[2] = php_base64_decode_translate16(in + 32, &b->v[2]);
	res[3] = php_base64_decode_translate16(in + 48, &b->v[3]);
	if (EXPECTED(vminvq_u8(vminq_u8(vminq_u8(res[0], res[1]), vminq_u8(res[2], res[3]))) > 1)) {
		return 0;
	}
	res[0] = vcleq_u8(res[0], vdupq_n_u8(1));
	res[1] = vcleq_u8(res[1], vdupq_n_u8(1));
	res[2] = vcleq_u8(res[2], vdupq_n_u8(1));
	res[3] = vcleq_u8(res[3], vdupq_n_u8(1));

	return php_base64_neon_movemask(res);
}

/* Bytes from 0x80 give indexes out of the table, hence 0. */
#define PHP_BASE64_NEON_MATCH16(s, lut) vceqq_u8(vqtbl1q_u8((lut), vandq_u8((s), vdupq_n_u8(0x8f))), (s))

static zend_always_inline uint64_t php_base64_decode_neon_match(const unsigned char *in, const uint8_t *set)
{
	const uint8x16_t lut = vld1q_u8(set);
	const uint8x16_t s0 = vld1q_u8(in), s1 = vld1q_u8(in + 16), s2 = vld1q_u8(in + 32), s3 = vld1q_u8(in + 48);
	const uint8x16_t match[4] = {
		PHP_BASE64_NEON_MATCH16(s0, lut), PHP_BASE64_NEON_MATCH16(s1, lut),
		PHP_BASE64_NEON_MATCH16(s2, lut), PHP_BASE64_NEON_MATCH16(s3, lut)
	};

	return php_base64_neon_movemask(match);
}

static zend_always_inline size_t php_base64_decode_neon_compress(const php_base64_neon_block *b, uint64_t bad, uint8_t *out)
{
	size_t n = 0;
	int i;

	if (bad && !(bad & (bad - 1))) {
		/* A single byte to remove: move the bytes after it down by one. */
		static const uint8_t iota[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
		const size_t pos = php_base64_ctz64(bad), j = pos >> 4;
		const uint8x16_t shift = vsubq_u8(vld1q_u8(iota), vcgeq_u8(vld1q_u8(iota), vdupq_n_u8(pos & 15)));

		for (i = 0; i < 4; i++) {
			vst1q_u8(out + 16 * i - (j < (size_t)i), (size_t)i == j ? vqtbl1q_u8(b->v[i], shift) : b->v[i]);
		}

		return 63;
	}

	for (i = 0; i < 4; i++, bad >>= 16) {
		const size_t kept = php_base64_zeros[bad & 0xff];

		if (bad & 0xffff) {
			/* Pack each half, then move the high half after what is kept of the low half. */
			const uint8x16_t halves = vqtbl1q_u8(b->v[i], vcombine_u8(vcreate_u8(php_base64_thintable[bad & 0xff]),
				vcreate_u8(php_base64_thintable[(bad >> 8) & 0xff] + UINT64_C(0x0808080808080808))));

			vst1q_u8(out + n, vqtbl1q_u8(halves, vld1q_u8(php_base64_combine[kept])));
		} else {
			vst1q_u8(out + n, b->v[i]);
		}
		n += kept + php_base64_zeros[(bad >> 8) & 0xff];
	}

	return n;
}

static zend_always_inline void php_base64_decode_neon_pack(const php_base64_neon_block *b, unsigned char *out)
{
	/* Gather the first, second, third and fourth values of each group of four. */
	const uint8x16_t even_lo = vuzp1q_u8(b->v[0], b->v[1]), odd_lo = vuzp2q_u8(b->v[0], b->v[1]);
	const uint8x16_t even_hi = vuzp1q_u8(b->v[2], b->v[3]), odd_hi = vuzp2q_u8(b->v[2], b->v[3]);
	const uint8x16_t v0 = vuzp1q_u8(even_lo, even_hi), v2 = vuzp2q_u8(even_lo, even_hi);
	const uint8x16_t v1 = vuzp1q_u8(odd_lo, odd_hi), v3 = vuzp2q_u8(odd_lo, odd_hi);
	uint8x16x3_t result;

	result.val[0] = vorrq_u8(vshrq_n_u8(v1, 4), vshlq_n_u8(v0, 2));
	result.val[1] = vorrq_u8(vshrq_n_u8(v2, 2), vshlq_n_u8(v1, 4));
	result.val[2] = vorrq_u8(v3, vshlq_n_u8(v2, 6));
	vst3q_u8(out, result);
}

/* If the 16 bytes at in are in the alphabet, decode them (writing 16 bytes). */
static zend_always_inline bool php_base64_decode_neon_step16(const unsigned char *in, unsigned char *out)
{
	static const uint8_t order[16] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 0xff, 0xff, 0xff, 0xff};
	uint8x16_t values;
	uint16x8_t w;
	uint32x4_t u;

	if (vminvq_u8(php_base64_decode_translate16(in, &values)) <= 1) {
		return false;
	}
	/* aaaaaa bbbbbb cccccc dddddd: aaaaaabbbbbb and ccccccdddddd, then the 24 bits */
	w = vreinterpretq_u16_u8(values);
	w = vorrq_u16(vandq_u16(vshlq_n_u16(w, 6), vdupq_n_u16(0x0fc0)), vshrq_n_u16(w, 8));
	u = vreinterpretq_u32_u16(w);
	u = vorrq_u32(vshlq_n_u32(vandq_u32(u, vdupq_n_u32(0xffff)), 12), vshrq_n_u32(u, 16));
	vst1q_u8(out, vqtbl1q_u8(vreinterpretq_u8_u32(u), vld1q_u8(order)));

	return true;
}
#endif

#if !defined(ZEND_INTRIN_AVX2_NATIVE) && !defined(ZEND_INTRIN_SSSE3_NATIVE)
#if defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
zend_string *php_base64_encode_default(const unsigned char *str, size_t length, zend_long flags)
#else
PHPAPI zend_string *php_base64_encode_ex(const unsigned char *str, size_t length, zend_long flags)
#endif
{
	unsigned char *p;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	p = (unsigned char *)ZSTR_VAL(result);

	p = php_base64_encode_impl(str, length, p, flags);

	ZSTR_LEN(result) = (p - (unsigned char *)ZSTR_VAL(result));

	return result;
}
#endif

#if !defined(ZEND_INTRIN_AVX2_NATIVE) && !defined(ZEND_INTRIN_SSSE3_NATIVE)
#if defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
zend_string *php_base64_decode_ex_default(const unsigned char *str, size_t length, bool strict)
#else
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, bool strict)
#endif
{
#if defined(__aarch64__) || defined(_M_ARM64)
	PHP_BASE64_DECODE_SIMD(neon, true)
#else
	zend_string *result;
	size_t outl = 0, i = 0, padding = 0;

	result = zend_string_alloc(length, 0);

	if (!php_base64_decode_loop(str, length, (unsigned char*)ZSTR_VAL(result), &outl, &i, &padding, strict)
			|| !php_base64_decode_finish((unsigned char*)ZSTR_VAL(result), outl, i, padding, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

	return result;
#endif
}
#endif
/* }}} */

/* {{{ Encodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_encode)
{
	char *str;
	size_t str_len;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	result = php_base64_encode((unsigned char*)str, str_len);
	RETURN_STR(result);
}
/* }}} */

/* {{{ Decodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_decode)
{
	char *str;
	bool strict = 0;
	size_t str_len;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();

	result = php_base64_decode_ex((unsigned char*)str, str_len, strict);
	if (result != NULL) {
		RETURN_STR(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
