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
   | Author: Jim Winstead <jimw@php.net>                                  |
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
 */

#include <string.h>

#include "php.h"
#include "base64.h"

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

static zend_always_inline uint8x16_t encode_toascii(const uint8x16_t input, const uint8x16x2_t shift_LUT)
{
	/* reduce  0..51 -> 0
	          52..61 -> 1 .. 10
	              62 -> 11
	              63 -> 12 */
	uint8x16_t result = vqsubq_u8(input, vdupq_n_u8(51));
	/* distinguish between ranges 0..25 and 26..51:
	   0 .. 25 -> remains 0
	   26 .. 51 -> becomes 13 */
	const uint8x16_t less = vcgtq_u8(vdupq_n_u8(26), input);
	result = vorrq_u8(result, vandq_u8(less, vdupq_n_u8(13)));
	/* read shift */
	result = vqtbl2q_u8(shift_LUT, result);
	return vaddq_u8(result, input);
}

static zend_always_inline unsigned char *neon_base64_encode(const unsigned char *in, size_t inl, unsigned char *out, size_t *left)
{
	const uint8_t shift_LUT_[32] = {'a' - 26, '0' - 52, '0' - 52, '0' - 52,
					'0' - 52, '0' - 52, '0' - 52, '0' - 52,
					'0' - 52, '0' - 52, '0' - 52, '+' - 62,
					'/' - 63, 'A',      0,        0,
					'a' - 26, '0' - 52, '0' - 52, '0' - 52,
					'0' - 52, '0' - 52, '0' - 52, '0' - 52,
					'0' - 52, '0' - 52, '0' - 52, '+' - 62,
					'/' - 63, 'A',      0,        0};
	const uint8x16x2_t shift_LUT = *((const uint8x16x2_t *)shift_LUT_);
	do {
		/* [ccdddddd | bbbbcccc | aaaaaabb]
		    x.val[2] | x.val[1] | x.val[0] */
		const uint8x16x3_t x = vld3q_u8((const uint8_t *)(in));

		/* [00aa_aaaa] */
		const uint8x16_t field_a = vshrq_n_u8(x.val[0], 2);

		const uint8x16_t field_b =             /* [00bb_bbbb] */
		    vbslq_u8(vdupq_n_u8(0x30),         /* [0011_0000] */
		             vshlq_n_u8(x.val[0], 4),  /* [aabb_0000] */
		             vshrq_n_u8(x.val[1], 4)); /* [0000_bbbb] */

		const uint8x16_t field_c =             /* [00cc_cccc] */
		    vbslq_u8(vdupq_n_u8(0x3c),         /* [0011_1100] */
		             vshlq_n_u8(x.val[1], 2),  /* [bbcc_cc00] */
		             vshrq_n_u8(x.val[2], 6)); /* [0000_00cc] */

		/* [00dd_dddd] */
		const uint8x16_t field_d = vandq_u8(x.val[2], vdupq_n_u8(0x3f));

		uint8x16x4_t result;
		result.val[0] = encode_toascii(field_a, shift_LUT);
		result.val[1] = encode_toascii(field_b, shift_LUT);
		result.val[2] = encode_toascii(field_c, shift_LUT);
		result.val[3] = encode_toascii(field_d, shift_LUT);

		vst4q_u8((uint8_t *)out, result);
		out += 64;
		in += 16 * 3;
		inl -= 16 * 3;
	} while (inl >= 16 * 3);

	*left = inl;
	return out;
}
#endif /* defined(__aarch64__) || defined(_M_ARM64) */

static zend_always_inline unsigned char *php_base64_encode_impl(const unsigned char *in, size_t inl, unsigned char *out) /* {{{ */
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
			*out++ = base64_pad;
		} else {
			*out++ = base64_table[(in[0] & 0x03) << 4];
			*out++ = base64_pad;
			*out++ = base64_pad;
		}
	}

	*out = '\0';

	return out;
}
/* }}} */

#if defined(__aarch64__) || defined(_M_ARM64)
static zend_always_inline uint8x16_t decode_fromascii(const uint8x16_t input, uint8x16_t *error, const uint8x16x2_t shiftLUT, const uint8x16x2_t maskLUT, const uint8x16x2_t bitposLUT) {
	const uint8x16_t higher_nibble = vshrq_n_u8(input, 4);
	const uint8x16_t lower_nibble = vandq_u8(input, vdupq_n_u8(0x0f));
	const uint8x16_t sh = vqtbl2q_u8(shiftLUT, higher_nibble);
	const uint8x16_t eq_2f = vceqq_u8(input, vdupq_n_u8(0x2f));
	const uint8x16_t shift = vbslq_u8(eq_2f, vdupq_n_u8(16), sh);
	const uint8x16_t M = vqtbl2q_u8(maskLUT, lower_nibble);
	const uint8x16_t bit = vqtbl2q_u8(bitposLUT, higher_nibble);
	*error = vceqq_u8(vandq_u8(M, bit), vdupq_n_u8(0));
	return vaddq_u8(input, shift);
}

static zend_always_inline size_t neon_base64_decode(const unsigned char *in, size_t inl, unsigned char *out, size_t *left) {
	unsigned char *out_orig = out;
	const uint8_t shiftLUT_[32] = {
		0,   0,  19,   4, (uint8_t)-65, (uint8_t)-65, (uint8_t)-71, (uint8_t)-71,
		0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,  19,   4, (uint8_t)-65, (uint8_t)-65, (uint8_t)-71, (uint8_t)-71,
		0,   0,   0,   0,   0,   0,   0,   0};
	const uint8_t maskLUT_[32] = {
		/* 0        : 0b1010_1000*/ 0xa8,
		/* 1 .. 9   : 0b1111_1000*/ 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
		/* 10       : 0b1111_0000*/ 0xf0,
		/* 11       : 0b0101_0100*/ 0x54,
		/* 12 .. 14 : 0b0101_0000*/ 0x50, 0x50, 0x50,
		/* 15       : 0b0101_0100*/ 0x54,

		/* 0        : 0b1010_1000*/ 0xa8,
		/* 1 .. 9   : 0b1111_1000*/ 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
		/* 10       : 0b1111_0000*/ 0xf0,
		/* 11       : 0b0101_0100*/ 0x54,
		/* 12 .. 14 : 0b0101_0000*/ 0x50, 0x50, 0x50,
		/* 15       : 0b0101_0100*/ 0x54
	};
	const uint8_t bitposLUT_[32] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	const uint8x16x2_t shiftLUT = *((const uint8x16x2_t *)shiftLUT_);
	const uint8x16x2_t maskLUT = *((const uint8x16x2_t *)maskLUT_);
	const uint8x16x2_t bitposLUT = *((const uint8x16x2_t *)bitposLUT_);;

	do {
		const uint8x16x4_t x = vld4q_u8((const unsigned char *)in);
		uint8x16_t error_a;
		uint8x16_t error_b;
		uint8x16_t error_c;
		uint8x16_t error_d;
		uint8x16_t field_a = decode_fromascii(x.val[0], &error_a, shiftLUT, maskLUT, bitposLUT);
		uint8x16_t field_b = decode_fromascii(x.val[1], &error_b, shiftLUT, maskLUT, bitposLUT);
		uint8x16_t field_c = decode_fromascii(x.val[2], &error_c, shiftLUT, maskLUT, bitposLUT);
		uint8x16_t field_d = decode_fromascii(x.val[3], &error_d, shiftLUT, maskLUT, bitposLUT);

		const uint8x16_t err = vorrq_u8(vorrq_u8(error_a, error_b), vorrq_u8(error_c, error_d));
		union {uint8_t mem[16]; uint64_t dw[2]; } error;
		vst1q_u8(error.mem, err);

		/* Check that the input only contains bytes belonging to the alphabet of
		   Base64. If there are errors, decode the rest of the string with the
		   scalar decoder. */
		if (error.dw[0] | error.dw[1])
			break;

		uint8x16x3_t result;
		result.val[0] = vorrq_u8(vshrq_n_u8(field_b, 4), vshlq_n_u8(field_a, 2));
		result.val[1] = vorrq_u8(vshrq_n_u8(field_c, 2), vshlq_n_u8(field_b, 4));
		result.val[2] = vorrq_u8(field_d, vshlq_n_u8(field_c, 6));

		vst3q_u8((unsigned char *)out, result);
		out += 16 * 3;
		in += 16 * 4;
		inl -= 16 * 4;
	} while (inl >= 16 * 4);
	*left = inl;
	return out - out_orig;
}
#endif /* defined(__aarch64__) || defined(_M_ARM64) */

static zend_always_inline int php_base64_decode_impl(const unsigned char *in, size_t inl, unsigned char *out, size_t *outl, bool strict) /* {{{ */
{
	int ch;
	size_t i = 0, padding = 0, j = *outl;

#if defined(__aarch64__) || defined(_M_ARM64)
	if (inl >= 16 * 4) {
		size_t left = 0;
		j += neon_base64_decode(in, inl, out, &left);
		i = inl - left;
		in += i;
		inl = left;
	}
#endif

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
				goto fail;
			}
		}

		switch (i % 4) {
			case 0:
				out[j] = ch << 2;
				break;
			case 1:
				out[j++] |= ch >> 4;
				out[j] = (ch & 0x0f) << 4;
				break;
			case 2:
				out[j++] |= ch >>2;
				out[j] = (ch & 0x03) << 6;
				break;
			case 3:
				out[j++] |= ch;
				break;
		}
		i++;
	}

	/* fail if the input is truncated (only one char in last group) */
	if (strict && i % 4 == 1) {
		goto fail;
	}

	/* fail if the padding length is wrong (not VV==, VVV=), but accept zero padding
	 * RFC 4648: "In some circumstances, the use of padding [--] is not required" */
	if (strict && padding && (padding > 2 || (i + padding) % 4 != 0)) {
		goto fail;
	}

	*outl = j;
	out[j] = '\0';

	return 1;

fail:
	return 0;
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
ZEND_INTRIN_AVX512_FUNC_DECL(zend_string *php_base64_encode_avx512(const unsigned char *str, size_t length));
ZEND_INTRIN_AVX512_FUNC_DECL(zend_string *php_base64_decode_ex_avx512(const unsigned char *str, size_t length, bool strict));
# endif
# if defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PTR)
ZEND_INTRIN_AVX512_VBMI_FUNC_DECL(zend_string *php_base64_encode_avx512_vbmi(const unsigned char *str, size_t length));
ZEND_INTRIN_AVX512_VBMI_FUNC_DECL(zend_string *php_base64_decode_ex_avx512_vbmi(const unsigned char *str, size_t length, bool strict));
# endif

# ifdef ZEND_INTRIN_AVX2_RESOLVER
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_encode_avx2(const unsigned char *str, size_t length));
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_decode_ex_avx2(const unsigned char *str, size_t length, bool strict));
# endif

# ifdef ZEND_INTRIN_SSSE3_RESOLVER
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length));
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, bool strict));
# endif

zend_string *php_base64_encode_default(const unsigned char *str, size_t length);
zend_string *php_base64_decode_ex_default(const unsigned char *str, size_t length, bool strict);

# if (defined(ZEND_INTRIN_AVX2_FUNC_PROTO) || defined(ZEND_INTRIN_SSSE3_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO))
PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length) __attribute__((ifunc("resolve_base64_encode")));
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, bool strict) __attribute__((ifunc("resolve_base64_decode")));

typedef zend_string *(*base64_encode_func_t)(const unsigned char *, size_t);
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
# ifdef BASE64_INTRIN_AVX512_FUNC_PROTO
	if (zend_cpu_supports_avx512()) {
		return php_base64_decode_ex_avx512;
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

PHPAPI zend_string *(*php_base64_encode_ptr)(const unsigned char *str, size_t length) = NULL;
PHPAPI zend_string *(*php_base64_decode_ex_ptr)(const unsigned char *str, size_t length, bool strict) = NULL;

PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length) {
	return php_base64_encode_ptr(str, length);
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
		php_base64_decode_ex_ptr = php_base64_decode_ex_avx512;
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

#if defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_VBMI_FUNC_PTR)
zend_string *php_base64_encode_avx512_vbmi(const unsigned char *str, size_t length)
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

	while (length > 63) {
		/* Step 1: load input data */
		__m512i str = _mm512_loadu_si512((const __m512i *)c);

		/* Step 2: splitting 24-bit words into 32-bit lanes */
		str = _mm512_permutexvar_epi8(shuffle_splitting, str);

		/* Step 3: moving 6-bit word to sperate bytes */
		str = _mm512_multishift_epi64_epi8(multi_shifts, str);

		/* Step 4: conversion to ASCII */
		str = _mm512_permutexvar_epi8(str, ascii_lookup);

		/* Step 5: store the final result */
		_mm512_storeu_si512((__m512i *)o, str);
		c += 48;
		o += 64;
		length -= 48;
	}

	o = php_base64_encode_impl(c, length, o);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}

zend_string *php_base64_decode_ex_avx512_vbmi(const unsigned char *str, size_t length, bool strict)
{
	const unsigned char *c = str;
	unsigned char *o;
	size_t outl = 0;
	zend_string *result;

	result = zend_string_alloc(length, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	const __m512i lookup_0 = _mm512_setr_epi32(
		0x80808080, 0x80808080, 0x80808080, 0x80808080, 0x80808080, 0x80808080,
		0x80808080, 0x80808080, 0x80808080, 0x80808080, 0x3e808080, 0x3f808080,
		0x37363534, 0x3b3a3938, 0x80803d3c, 0x80808080);
	const __m512i lookup_1 = _mm512_setr_epi32(
		0x02010080, 0x06050403, 0x0a090807, 0x0e0d0c0b, 0x1211100f, 0x16151413,
		0x80191817, 0x80808080, 0x1c1b1a80, 0x201f1e1d, 0x24232221, 0x28272625,
		0x2c2b2a29, 0x302f2e2d, 0x80333231, 0x80808080);

	const __m512i merge_mask1 = _mm512_set1_epi32(0x01400140);
	const __m512i merge_mask2 = _mm512_set1_epi32(0x00011000);

	const __m512i continuous_mask = _mm512_setr_epi32(
		0x06000102, 0x090a0405, 0x0c0d0e08, 0x16101112, 0x191a1415, 0x1c1d1e18,
		0x26202122, 0x292a2425, 0x2c2d2e28, 0x36303132, 0x393a3435, 0x3c3d3e38,
		0x00000000, 0x00000000, 0x00000000, 0x00000000);

	while (length > 64) {
		/* Step 1: load input data */
		const __m512i input = _mm512_loadu_si512((__m512i *)c);

		/* Step 2: translation into 6-bit values(saved on bytes) from ASCII and error detection */
		__m512i str = _mm512_permutex2var_epi8(lookup_0, input, lookup_1);
		const uint64_t mask = _mm512_movepi8_mask(_mm512_or_epi64(str, input)); /* convert MSBs to the mask */
		if (mask) {
			break;
		}

		/* Step 3: pack four fields within 32-bit words into 24-bit words. */
		const __m512i merge_ab_and_bc = _mm512_maddubs_epi16(str, merge_mask1);
		str = _mm512_madd_epi16(merge_ab_and_bc, merge_mask2);

		/* Step 4: move 3-byte words into the continuous array. */
		str = _mm512_permutexvar_epi8(continuous_mask, str);

		/* Step 5: store the final result */
		_mm512_storeu_si512((__m512i *)o, str);

		c += 64;
		o += 48;
		outl += 48;
		length -= 64;
	}

	if (!php_base64_decode_impl(c, length, (unsigned char*)ZSTR_VAL(result), &outl, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

	return result;
}
#endif

#if defined(BASE64_INTRIN_AVX512_FUNC_PROTO) || defined(BASE64_INTRIN_AVX512_FUNC_PTR)
zend_string *php_base64_encode_avx512(const unsigned char *str, size_t length)
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

		/* Step 3: moving 6-bit word to sperate bytes */
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

	o = php_base64_encode_impl(c, length, o);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}

#define build_dword(b0, b1, b2, b3)					\
	((uint32_t)(uint8_t)b0 << 0) | ((uint32_t)(uint8_t)b1 << 8) |	\
	((uint32_t)(uint8_t)b2 << 16) | ((uint32_t)(uint8_t)b3 << 24)

#define _mm512_set4lanes_epi8(b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15)	\
	_mm512_setr4_epi32(build_dword(b0, b1, b2, b3), build_dword(b4, b5, b6, b7),			\
			   build_dword(b8, b9, b10, b11), build_dword(b12, b13, b14, b15))

zend_string *php_base64_decode_ex_avx512(const unsigned char *str, size_t length, bool strict)
{
	const unsigned char *c = str;
	unsigned char *o;
	size_t outl = 0;
	zend_string *result;

	result = zend_string_alloc(length, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	while (length > 64) {
		/* Step 1: load input data */
		__m512i str = _mm512_loadu_si512((__m512i *)c);

		/* Step 2: translation into 6-bit values(saved on bytes) from ASCII and error detection */
		const __m512i higher_nibble = _mm512_and_si512(_mm512_srli_epi32(str, 4), _mm512_set1_epi8(0x0f));
		const __m512i lower_nibble = _mm512_and_si512(str, _mm512_set1_epi8(0x0f));
		const __m512i shiftLUT = _mm512_set4lanes_epi8(
				0, 0, 19, 4, -65, -65, -71, -71, 0, 0, 0, 0, 0, 0, 0, 0);
		const __m512i maskLUT = _mm512_set4lanes_epi8(
				/* 0        : 0b1010_1000*/ 0xa8,
				/* 1 .. 9   : 0b1111_1000*/ 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
				/* 10       : 0b1111_0000*/ 0xf0,
				/* 11       : 0b0101_0100*/ 0x54,
				/* 12 .. 14 : 0b0101_0000*/ 0x50, 0x50, 0x50,
				/* 15       : 0b0101_0100*/ 0x54);
		const __m512i bitposLUT = _mm512_set4lanes_epi8(
				0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
		const __m512i M = _mm512_shuffle_epi8(maskLUT, lower_nibble);
		const __m512i bit = _mm512_shuffle_epi8(bitposLUT, higher_nibble);
		const uint64_t match = _mm512_test_epi8_mask(M, bit);
		if (match != (uint64_t)-1) {
			break;
		}
		const __m512i sh = _mm512_shuffle_epi8(shiftLUT, higher_nibble);
		const __mmask64 eq_2f = _mm512_cmpeq_epi8_mask(str, _mm512_set1_epi8(0x2f));
		const __m512i shift = _mm512_mask_mov_epi8(sh, eq_2f, _mm512_set1_epi8(16));
		str = _mm512_add_epi8(str, shift);

		/* Step 3: pack four fields within 32-bit words into 24-bit words. */
		const __m512i merge_ab_and_bc = _mm512_maddubs_epi16(str, _mm512_set1_epi32(0x01400140));
		str = _mm512_madd_epi16(merge_ab_and_bc, _mm512_set1_epi32(0x00011000));

		/* Step 4: move 3-byte words into the continuous array. */
		const __m512i t1 = _mm512_shuffle_epi8(str,
			_mm512_set4lanes_epi8(2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));
		const __m512i s6 = _mm512_setr_epi32(0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, 0, 0, 0, 0);
		const __m512i t2 = _mm512_permutexvar_epi32(s6, t1);

		/* Step 5: store the final result */
		_mm512_storeu_si512((__m512i *)o, t2);

		c += 64;
		o += 48;
		outl += 48;
		length -= 64;
	}

	if (!php_base64_decode_impl(c, length, (unsigned char*)ZSTR_VAL(result), &outl, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

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
PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length)
# elif defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_encode_avx2(const unsigned char *str, size_t length)
# else /* ZEND_INTRIN_SSSE3_RESOLVER */
zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length)
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

	o = php_base64_encode_impl(c, length, o);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}

# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length)
{
	const unsigned char *c = str;
	unsigned char *o;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	PHP_BASE64_ENCODE_SSSE3_LOOP;

	o = php_base64_encode_impl(c, length, o);

	ZSTR_LEN(result) = (o - (unsigned char *)ZSTR_VAL(result));

	return result;
}
# endif
#endif /* ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER */

/* }}} */

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
# if defined(ZEND_INTRIN_AVX2_RESOLVER) && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
static __m256i php_base64_decode_avx2_reshuffle(__m256i in) __attribute__((target("avx2")));
# endif

static __m256i php_base64_decode_avx2_reshuffle(__m256i in)
{
	__m256i merge_ab_and_bc, out;

	merge_ab_and_bc = _mm256_maddubs_epi16(in, _mm256_set1_epi32(0x01400140));

	out = _mm256_madd_epi16(merge_ab_and_bc, _mm256_set1_epi32(0x00011000));

	out = _mm256_shuffle_epi8(out, _mm256_setr_epi8(
				2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1,
				2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, -1, -1, -1, -1));

	return _mm256_permutevar8x32_epi32(out, _mm256_setr_epi32(0, 1, 2, 4, 5, 6, -1, -1));
}
#endif

#if defined(ZEND_INTRIN_SSSE3_NATIVE) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
static __m128i php_base64_decode_ssse3_reshuffle(__m128i in) __attribute__((target("ssse3")));
# endif

static __m128i php_base64_decode_ssse3_reshuffle(__m128i in)
{
	__m128i merge_ab_and_bc, out;

	merge_ab_and_bc = _mm_maddubs_epi16(in, _mm_set1_epi32(0x01400140));
	/* 0000kkkk LLllllll 0000JJJJ JJjjKKKK
	 * 0000hhhh IIiiiiii 0000GGGG GGggHHHH
	 * 0000eeee FFffffff 0000DDDD DDddEEEE
	 * 0000bbbb CCcccccc 0000AAAA AAaaBBBB */

	out = _mm_madd_epi16(merge_ab_and_bc, _mm_set1_epi32(0x00011000));
	/* 00000000 JJJJJJjj KKKKkkkk LLllllll
	 * 00000000 GGGGGGgg HHHHhhhh IIiiiiii
	 * 00000000 DDDDDDdd EEEEeeee FFffffff
	 * 00000000 AAAAAAaa BBBBbbbb CCcccccc */

	return  _mm_shuffle_epi8(out, _mm_setr_epi8(
		 2,  1,  0,
		 6,  5,  4,
		10,  9,  8,
		14, 13, 12,
		-1, -1, -1, -1));
	/* 00000000 00000000 00000000 00000000
	 * LLllllll KKKKkkkk JJJJJJjj IIiiiiii
	 * HHHHhhhh GGGGGGgg FFffffff EEEEeeee
	 * DDDDDDdd CCcccccc BBBBbbbb AAAAAAaa */
}

#define PHP_BASE64_DECODE_SSSE3_LOOP								\
	while (length > 15 + 6 + 2) {									\
		__m128i lut_lo, lut_hi, lut_roll;							\
		__m128i hi_nibbles, lo_nibbles, hi, lo;						\
		__m128i s = _mm_loadu_si128((__m128i *)c);					\
																	\
		lut_lo = _mm_setr_epi8(										\
				0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,		\
				0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);	\
		lut_hi = _mm_setr_epi8(										\
				0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,		\
				0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);	\
		lut_roll = _mm_setr_epi8(									\
				0,  16,  19,   4, -65, -65, -71, -71,				\
				0,   0,   0,   0,   0,   0,   0,   0);				\
																	\
		hi_nibbles  = _mm_and_si128(								\
						_mm_srli_epi32(s, 4), _mm_set1_epi8(0x2f));	\
		lo_nibbles  = _mm_and_si128(s, _mm_set1_epi8(0x2f));		\
		hi          = _mm_shuffle_epi8(lut_hi, hi_nibbles);			\
		lo          = _mm_shuffle_epi8(lut_lo, lo_nibbles);			\
																	\
																	\
		if (UNEXPECTED(												\
			_mm_movemask_epi8(										\
				_mm_cmpgt_epi8(										\
					_mm_and_si128(lo, hi), _mm_set1_epi8(0))))) {	\
			break;													\
		} else {													\
			__m128i eq_2f, roll;									\
																	\
			eq_2f = _mm_cmpeq_epi8(s, _mm_set1_epi8(0x2f));			\
			roll = _mm_shuffle_epi8(								\
					lut_roll, _mm_add_epi8(eq_2f, hi_nibbles));		\
																	\
			s = _mm_add_epi8(s, roll);								\
			s = php_base64_decode_ssse3_reshuffle(s);				\
																	\
			_mm_storeu_si128((__m128i *)o, s);						\
																	\
			c += 16;												\
			o += 12;												\
			outl += 12;												\
			length -= 16;											\
		}															\
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
	const unsigned char *c = str;
	unsigned char *o;
	size_t outl = 0;
	zend_string *result;

	result = zend_string_alloc(length, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	/* See: "Faster Base64 Encoding and Decoding using AVX2 Instructions"
	* https://arxiv.org/pdf/1704.00605.pdf */
# if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)
	while (length > 31 + 11 + 2) {
		__m256i lut_lo, lut_hi, lut_roll;
		__m256i hi_nibbles, lo_nibbles, hi, lo;
		__m256i str = _mm256_loadu_si256((__m256i *)c);

		lut_lo = _mm256_setr_epi8(
				0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
				0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A,
				0x15, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
				0x11, 0x11, 0x13, 0x1A, 0x1B, 0x1B, 0x1B, 0x1A);

		lut_hi = _mm256_setr_epi8(
				0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
				0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
				0x10, 0x10, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08,
				0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10);

		lut_roll = _mm256_setr_epi8(
				0,  16,  19,   4, -65, -65, -71, -71,
				0,   0,   0,   0,   0,   0,   0,   0,
				0,  16,  19,   4, -65, -65, -71, -71,
				0,   0,   0,   0,   0,   0,   0,   0);

		hi_nibbles  = _mm256_and_si256(_mm256_srli_epi32(str, 4), _mm256_set1_epi8(0x2f));
		lo_nibbles  = _mm256_and_si256(str, _mm256_set1_epi8(0x2f));
		hi          = _mm256_shuffle_epi8(lut_hi, hi_nibbles);
		lo          = _mm256_shuffle_epi8(lut_lo, lo_nibbles);

		if (!_mm256_testz_si256(lo, hi)) {
			break;
		} else {
			__m256i eq_2f, roll;
			eq_2f = _mm256_cmpeq_epi8(str, _mm256_set1_epi8(0x2f));
			roll  = _mm256_shuffle_epi8(lut_roll, _mm256_add_epi8(eq_2f, hi_nibbles));


			str = _mm256_add_epi8(str, roll);

			str = php_base64_decode_avx2_reshuffle(str);

			_mm256_storeu_si256((__m256i *)o, str);

			c += 32;
			o += 24;
			outl += 24;
			length -= 32;
		}
	}
# else
	PHP_BASE64_DECODE_SSSE3_LOOP;
# endif

	if (!php_base64_decode_impl(c, length, (unsigned char*)ZSTR_VAL(result), &outl, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

	return result;
}

# if defined(ZEND_INTRIN_SSSE3_RESOLVER) && defined(ZEND_INTRIN_AVX2_RESOLVER)
zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, bool strict)
{
	const unsigned char *c = str;
	unsigned char *o;
	size_t outl = 0;
	zend_string *result;

	result = zend_string_alloc(length, 0);
	o = (unsigned char *)ZSTR_VAL(result);

	PHP_BASE64_DECODE_SSSE3_LOOP;

	if (!php_base64_decode_impl(c, length, (unsigned char*)ZSTR_VAL(result), &outl, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

	return result;
}
# endif
#endif /* ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER */

#if !defined(ZEND_INTRIN_AVX2_NATIVE) && !defined(ZEND_INTRIN_SSSE3_NATIVE)
#if defined(ZEND_INTRIN_AVX2_RESOLVER) || defined(ZEND_INTRIN_SSSE3_RESOLVER)
zend_string *php_base64_encode_default(const unsigned char *str, size_t length)
#else
PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length)
#endif
{
	unsigned char *p;
	zend_string *result;

	result = zend_string_safe_alloc(((length + 2) / 3), 4 * sizeof(char), 0, 0);
	p = (unsigned char *)ZSTR_VAL(result);

	p = php_base64_encode_impl(str, length, p);

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
	zend_string *result;
	size_t outl = 0;

	result = zend_string_alloc(length, 0);

	if (!php_base64_decode_impl(str, length, (unsigned char*)ZSTR_VAL(result), &outl, strict)) {
		zend_string_efree(result);
		return NULL;
	}

	ZSTR_LEN(result) = outl;

	return result;
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
