/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

static zend_always_inline unsigned char *php_base64_encode_impl(const unsigned char *in, size_t inl, unsigned char *out) /* {{{ */
{

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

static zend_always_inline int php_base64_decode_impl(const unsigned char *in, size_t inl, unsigned char *out, size_t *outl, zend_bool strict) /* {{{ */
{
	int ch;
	size_t i = 0, padding = 0, j = *outl;

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

#if ZEND_INTRIN_AVX2_NATIVE
# undef ZEND_INTRIN_SSSE3_NATIVE
# undef ZEND_INTRIN_SSSE3_RESOLVER
# undef ZEND_INTRIN_SSSE3_FUNC_PROTO
# undef ZEND_INTRIN_SSSE3_FUNC_PTR
#elif ZEND_INTRIN_AVX2_FUNC_PROTO && ZEND_INTRIN_SSSE3_NATIVE
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
#elif ZEND_INTRIN_AVX2_FUNC_PTR && ZEND_INTRIN_SSSE3_NATIVE
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

#if ZEND_INTRIN_AVX2_NATIVE
# include <immintrin.h>
#elif ZEND_INTRIN_SSSE3_NATIVE
# include <tmmintrin.h>
#elif (ZEND_INTRIN_SSSE3_RESOLVER || ZEND_INTRIN_AVX2_RESOLVER)
# if ZEND_INTRIN_AVX2_RESOLVER
#  include <immintrin.h>
# else
#  include <tmmintrin.h>
# endif /* (ZEND_INTRIN_SSSE3_RESOLVER || ZEND_INTRIN_AVX2_RESOLVER) */
# include "Zend/zend_cpuinfo.h"

# if ZEND_INTRIN_AVX2_RESOLVER
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_encode_avx2(const unsigned char *str, size_t length));
ZEND_INTRIN_AVX2_FUNC_DECL(zend_string *php_base64_decode_ex_avx2(const unsigned char *str, size_t length, zend_bool strict));
# endif

# if ZEND_INTRIN_SSSE3_RESOLVER
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_encode_ssse3(const unsigned char *str, size_t length));
ZEND_INTRIN_SSSE3_FUNC_DECL(zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, zend_bool strict));
# endif

zend_string *php_base64_encode_default(const unsigned char *str, size_t length);
zend_string *php_base64_decode_ex_default(const unsigned char *str, size_t length, zend_bool strict);

# if (ZEND_INTRIN_AVX2_FUNC_PROTO || ZEND_INTRIN_SSSE3_FUNC_PROTO)
PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length) __attribute__((ifunc("resolve_base64_encode")));
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, zend_bool strict) __attribute__((ifunc("resolve_base64_decode")));

ZEND_NO_SANITIZE_ADDRESS
static void *resolve_base64_encode() {
# if ZEND_INTRIN_AVX2_FUNC_PROTO
	if (zend_cpu_supports_avx2()) {
		return php_base64_encode_avx2;
	} else
# endif
#if ZEND_INTRIN_SSSE3_FUNC_PROTO
	if (zend_cpu_supports_ssse3()) {
		return php_base64_encode_ssse3;
	}
#endif
	return php_base64_encode_default;
}

ZEND_NO_SANITIZE_ADDRESS
static void *resolve_base64_decode() {
# if ZEND_INTRIN_AVX2_FUNC_PROTO
	if (zend_cpu_supports_avx2()) {
		return php_base64_decode_ex_avx2;
	} else
# endif
#if ZEND_INTRIN_SSSE3_FUNC_PROTO
	if (zend_cpu_supports_ssse3()) {
		return php_base64_decode_ex_ssse3;
	}
#endif
	return php_base64_decode_ex_default;
}
# else /* (ZEND_INTRIN_AVX2_FUNC_PROTO || ZEND_INTRIN_SSSE3_FUNC_PROTO) */

PHPAPI zend_string *(*php_base64_encode)(const unsigned char *str, size_t length) = NULL;
PHPAPI zend_string *(*php_base64_decode_ex)(const unsigned char *str, size_t length, zend_bool strict) = NULL;

PHP_MINIT_FUNCTION(base64_intrin)
{
# if ZEND_INTRIN_AVX2_FUNC_PTR
	if (zend_cpu_supports_avx2()) {
		php_base64_encode = php_base64_encode_avx2;
		php_base64_decode_ex = php_base64_decode_ex_avx2;
	} else
# endif
#if ZEND_INTRIN_SSSE3_FUNC_PTR
	if (zend_cpu_supports_ssse3()) {
		php_base64_encode = php_base64_encode_ssse3;
		php_base64_decode_ex = php_base64_decode_ex_ssse3;
	} else
#endif
	{
		php_base64_encode = php_base64_encode_default;
		php_base64_decode_ex = php_base64_decode_ex_default;
	}
	return SUCCESS;
}
# endif /* (ZEND_INTRIN_AVX2_FUNC_PROTO || ZEND_INTRIN_SSSE3_FUNC_PROTO) */
#endif /* ZEND_INTRIN_AVX2_NATIVE */

#if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER
# if ZEND_INTRIN_AVX2_RESOLVER && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
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

#if ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER

# if ZEND_INTRIN_SSSE3_RESOLVER && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
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

#if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER
# if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_SSSE3_NATIVE
PHPAPI zend_string *php_base64_encode(const unsigned char *str, size_t length)
# elif ZEND_INTRIN_AVX2_RESOLVER
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
# if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER
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

# if ZEND_INTRIN_SSSE3_RESOLVER && ZEND_INTRIN_AVX2_RESOLVER
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

/* {{{ php_base64_decode_ex */
/* generate reverse table (do not set index 0 to 64)
static unsigned short base64_reverse_table[256];
#define rt base64_reverse_table
void php_base64_init(void)
{
	char *s = emalloc(10240), *sp;
	char *chp;
	short idx;

	for(ch = 0; ch < 256; ch++) {
		chp = strchr(base64_table, ch);
		if(ch && chp) {
			idx = chp - base64_table;
			if (idx >= 64) idx = -1;
			rt[ch] = idx;
		} else {
			rt[ch] = -1;
		}
	}
	sp = s;
	sprintf(sp, "static const short base64_reverse_table[256] = {\n");
	for(ch =0; ch < 256;) {
		sp = s+strlen(s);
		sprintf(sp, "\t% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,\n", rt[ch+0], rt[ch+1], rt[ch+2], rt[ch+3], rt[ch+4], rt[ch+5], rt[ch+6], rt[ch+7], rt[ch+8], rt[ch+9], rt[ch+10], rt[ch+11], rt[ch+12], rt[ch+13], rt[ch+14], rt[ch+15]);
		ch += 16;
	}
	sprintf(sp, "};");
	php_error_docref(NULL, E_NOTICE, "Reverse_table:\n%s", s);
	efree(s);
}
*/

#if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER
# if ZEND_INTRIN_AVX2_RESOLVER && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
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

#if ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER
# if ZEND_INTRIN_SSSE3_RESOLVER && defined(HAVE_FUNC_ATTRIBUTE_TARGET)
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

#if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_NATIVE || ZEND_INTRIN_SSSE3_RESOLVER
# if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_SSSE3_NATIVE
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, zend_bool strict)
# elif ZEND_INTRIN_AVX2_RESOLVER
zend_string *php_base64_decode_ex_avx2(const unsigned char *str, size_t length, zend_bool strict)
# else
zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, zend_bool strict)
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
# if ZEND_INTRIN_AVX2_NATIVE || ZEND_INTRIN_AVX2_RESOLVER
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

# if ZEND_INTRIN_SSSE3_RESOLVER && ZEND_INTRIN_AVX2_RESOLVER
zend_string *php_base64_decode_ex_ssse3(const unsigned char *str, size_t length, zend_bool strict)
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

#if !ZEND_INTRIN_AVX2_NATIVE && !ZEND_INTRIN_SSSE3_NATIVE
#if ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_RESOLVER
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

#if !ZEND_INTRIN_AVX2_NATIVE && !ZEND_INTRIN_SSSE3_NATIVE
#if ZEND_INTRIN_AVX2_RESOLVER || ZEND_INTRIN_SSSE3_RESOLVER
zend_string *php_base64_decode_ex_default(const unsigned char *str, size_t length, zend_bool strict)
#else
PHPAPI zend_string *php_base64_decode_ex(const unsigned char *str, size_t length, zend_bool strict)
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

/* {{{ proto string base64_encode(string str)
   Encodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_encode)
{
	char *str;
	size_t str_len;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	result = php_base64_encode((unsigned char*)str, str_len);
	if (result != NULL) {
		RETURN_STR(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string base64_decode(string str[, bool strict])
   Decodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_decode)
{
	char *str;
	zend_bool strict = 0;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
