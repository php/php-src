/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Michael Wallner <mike@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_hash.h"
#include "php_hash_tiger.h"
#include "php_hash_tiger_tables.h"

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

/* {{{ */
#define save_abc \
	aa = a; \
	bb = b; \
	cc = c;

#define round(a,b,c,x,mul) \
	c ^= x; \
	a -= t1[(unsigned char)(c)] ^ \
		t2[(unsigned char)(((php_hash_uint32)(c))>>(2*8))] ^ \
		t3[(unsigned char)((c)>>(4*8))] ^ \
		t4[(unsigned char)(((php_hash_uint32)((c)>>(4*8)))>>(2*8))] ; \
	b += t4[(unsigned char)(((php_hash_uint32)(c))>>(1*8))] ^ \
		t3[(unsigned char)(((php_hash_uint32)(c))>>(3*8))] ^ \
		t2[(unsigned char)(((php_hash_uint32)((c)>>(4*8)))>>(1*8))] ^ \
		t1[(unsigned char)(((php_hash_uint32)((c)>>(4*8)))>>(3*8))]; \
	b *= mul;

#define pass(a,b,c,mul) \
	round(a,b,c,x0,mul) \
	round(b,c,a,x1,mul) \
	round(c,a,b,x2,mul) \
	round(a,b,c,x3,mul) \
	round(b,c,a,x4,mul) \
	round(c,a,b,x5,mul) \
	round(a,b,c,x6,mul) \
	round(b,c,a,x7,mul)

#define key_schedule \
	x0 -= x7 ^ L64(0xA5A5A5A5A5A5A5A5); \
	x1 ^= x0; \
	x2 += x1; \
	x3 -= x2 ^ ((~x1)<<19); \
	x4 ^= x3; \
	x5 += x4; \
	x6 -= x5 ^ ((~x4)>>23); \
	x7 ^= x6; \
	x0 += x7; \
	x1 -= x0 ^ ((~x7)<<19); \
	x2 ^= x1; \
	x3 += x2; \
	x4 -= x3 ^ ((~x2)>>23); \
	x5 ^= x4; \
	x6 += x5; \
	x7 -= x6 ^ L64(0x0123456789ABCDEF);

#define feedforward \
	a ^= aa; \
	b -= bb; \
	c += cc;

#define compress(passes) \
	save_abc \
	pass(a,b,c,5) \
	key_schedule \
	pass(c,a,b,7) \
	key_schedule \
	pass(b,c,a,9) \
	for(pass_no=0; pass_no<passes; pass_no++) { \
		key_schedule \
		pass(a,b,c,9) \
		tmpa=a; a=c; c=b; b=tmpa; \
	} \
	feedforward

#define split_ex(str) \
	x0=str[0]; x1=str[1]; x2=str[2]; x3=str[3]; \
	x4=str[4]; x5=str[5]; x6=str[6]; x7=str[7];
#ifdef WORDS_BIGENDIAN
#	define split(str) \
	{ \
		int i; \
		php_hash_uint64 tmp[8]; \
		 \
		for (i = 0; i < 64; ++i) { \
			((unsigned char *) tmp)[i^7] = ((unsigned char *) str)[i]; \
		} \
		split_ex(tmp); \
	}
#else
#	define split split_ex
#endif

#define tiger_compress(passes, str, state) \
{ \
	register php_hash_uint64 a, b, c, tmpa, x0, x1, x2, x3, x4, x5, x6, x7; \
	php_hash_uint64 aa, bb, cc; \
	unsigned int pass_no; \
	\
	a = state[0]; \
	b = state[1]; \
	c = state[2]; \
	\
	split(str); \
	\
	compress(passes); \
	\
	state[0] = a; \
	state[1] = b; \
	state[2] = c; \
}
/* }}} */

static inline void TigerFinalize(PHP_TIGER_CTX *context)
{
	context->passed += (php_hash_uint64) context->length << 3;

	context->buffer[context->length++] = 0x1;
	if (context->length % 8) {
		memset(&context->buffer[context->length], 0, 8-context->length%8);
		context->length += 8-context->length%8;
	}

	if (context->length > 56) {
		memset(&context->buffer[context->length], 0, 64 - context->length);
		tiger_compress(context->passes, ((php_hash_uint64 *) context->buffer), context->state);
		memset(context->buffer, 0, 56);
	} else {
		memset(&context->buffer[context->length], 0, 56 - context->length);
	}

#ifndef WORDS_BIGENDIAN
	memcpy(&context->buffer[56], &context->passed, sizeof(php_hash_uint64));
#else
	context->buffer[56] = (unsigned char) (context->passed & 0xff);
	context->buffer[57] = (unsigned char) ((context->passed >> 8) & 0xff);
	context->buffer[58] = (unsigned char) ((context->passed >> 16) & 0xff);
	context->buffer[59] = (unsigned char) ((context->passed >> 24) & 0xff);
	context->buffer[60] = (unsigned char) ((context->passed >> 32) & 0xff);
	context->buffer[61] = (unsigned char) ((context->passed >> 40) & 0xff);
	context->buffer[62] = (unsigned char) ((context->passed >> 48) & 0xff);
	context->buffer[63] = (unsigned char) ((context->passed >> 56) & 0xff);
#endif
	tiger_compress(context->passes, ((php_hash_uint64 *) context->buffer), context->state);
}

static inline void TigerDigest(unsigned char *digest_str, unsigned int digest_len, PHP_TIGER_CTX *context)
{
	unsigned int i;

	for (i = 0; i < digest_len; ++i) {
		digest_str[i] = (unsigned char) ((context->state[i/8] >> (8 * (i%8))) & 0xff);
	}
}

PHP_HASH_API void PHP_3TIGERInit(PHP_TIGER_CTX *context)
{
	memset(context, 0, sizeof(*context));
	context->state[0] = L64(0x0123456789ABCDEF);
	context->state[1] = L64(0xFEDCBA9876543210);
	context->state[2] = L64(0xF096A5B4C3B2E187);
}

PHP_HASH_API void PHP_4TIGERInit(PHP_TIGER_CTX *context)
{
	memset(context, 0, sizeof(*context));
	context->passes = 1;
	context->state[0] = L64(0x0123456789ABCDEF);
	context->state[1] = L64(0xFEDCBA9876543210);
	context->state[2] = L64(0xF096A5B4C3B2E187);
}

PHP_HASH_API void PHP_TIGERUpdate(PHP_TIGER_CTX *context, const unsigned char *input, size_t len)
{
	if (context->length + len < 64) {
		memcpy(&context->buffer[context->length], input, len);
		context->length += len;
	} else {
		size_t i = 0, r = (context->length + len) % 64;

		if (context->length) {
			i = 64 - context->length;
			memcpy(&context->buffer[context->length], input, i);
			tiger_compress(context->passes, ((const php_hash_uint64 *) context->buffer), context->state);
			ZEND_SECURE_ZERO(context->buffer, 64);
			context->passed += 512;
		}

		for (; i + 64 <= len; i += 64) {
			memcpy(context->buffer, &input[i], 64);
			tiger_compress(context->passes, ((const php_hash_uint64 *) context->buffer), context->state);
			context->passed += 512;
		}
		ZEND_SECURE_ZERO(&context->buffer[r], 64-r);
		memcpy(context->buffer, &input[i], r);
		context->length = r;
	}
}

PHP_HASH_API void PHP_TIGER128Final(unsigned char digest[16], PHP_TIGER_CTX *context)
{
	TigerFinalize(context);
	TigerDigest(digest, 16, context);
	ZEND_SECURE_ZERO(context, sizeof(*context));
}

PHP_HASH_API void PHP_TIGER160Final(unsigned char digest[20], PHP_TIGER_CTX *context)
{
	TigerFinalize(context);
	TigerDigest(digest, 20, context);
	ZEND_SECURE_ZERO(context, sizeof(*context));
}

PHP_HASH_API void PHP_TIGER192Final(unsigned char digest[24], PHP_TIGER_CTX *context)
{
	TigerFinalize(context);
	TigerDigest(digest, 24, context);
	ZEND_SECURE_ZERO(context, sizeof(*context));
}

#define PHP_HASH_TIGER_OPS(p, b) \
	const php_hash_ops php_hash_##p##tiger##b##_ops = { \
		(php_hash_init_func_t) PHP_##p##TIGERInit, \
		(php_hash_update_func_t) PHP_TIGERUpdate, \
		(php_hash_final_func_t) PHP_TIGER##b##Final, \
		(php_hash_copy_func_t) php_hash_copy, \
		b/8, \
		64, \
		sizeof(PHP_TIGER_CTX) \
	}

PHP_HASH_TIGER_OPS(3, 128);
PHP_HASH_TIGER_OPS(3, 160);
PHP_HASH_TIGER_OPS(3, 192);
PHP_HASH_TIGER_OPS(4, 128);
PHP_HASH_TIGER_OPS(4, 160);
PHP_HASH_TIGER_OPS(4, 192);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
