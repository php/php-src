/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Taken from: ext/standard/md5.c                                       |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_hash.h"
#include "php_hash_md.h"

const php_hash_ops php_hash_md5_ops = {
	(php_hash_init_func_t) PHP_MD5Init,
	(php_hash_update_func_t) PHP_MD5Update,
	(php_hash_final_func_t) PHP_MD5Final,
	(php_hash_copy_func_t) php_hash_copy,
	16,
	64,
	sizeof(PHP_MD5_CTX)
};

const php_hash_ops php_hash_md4_ops = {
	(php_hash_init_func_t) PHP_MD4Init,
	(php_hash_update_func_t) PHP_MD4Update,
	(php_hash_final_func_t) PHP_MD4Final,
	(php_hash_copy_func_t) php_hash_copy,
	16,
	64,
	sizeof(PHP_MD4_CTX)
};

const php_hash_ops php_hash_md2_ops = {
	(php_hash_init_func_t) PHP_MD2Init,
	(php_hash_update_func_t) PHP_MD2Update,
	(php_hash_final_func_t) PHP_MD2Final,
	(php_hash_copy_func_t) php_hash_copy,
	16,
	16,
	sizeof(PHP_MD2_CTX)
};

/* MD common stuff */

static const unsigned char PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* {{{ Encode
   Encodes input (php_hash_uint32) into output (unsigned char). Assumes len is
   a multiple of 4.
 */
static void Encode(unsigned char *output, php_hash_uint32 *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char) (input[i] & 0xff);
		output[j + 1] = (unsigned char) ((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char) ((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char) ((input[i] >> 24) & 0xff);
	}
}
/* }}} */

/* {{{ Decode
   Decodes input (unsigned char) into output (php_hash_uint32). Assumes len is
   a multiple of 4.
 */
static void Decode(php_hash_uint32 *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((php_hash_uint32) input[j]) | (((php_hash_uint32) input[j + 1]) << 8) |
			(((php_hash_uint32) input[j + 2]) << 16) | (((php_hash_uint32) input[j + 3]) << 24);
}
/* }}} */

#ifdef PHP_HASH_MD5_NOT_IN_CORE

/* MD5 */

PHP_HASH_API void make_digest(char *md5str, unsigned char *digest)
{
	php_hash_bin2hex(md5str, digest, 16);
	md5str[32] = '\0';
}

/* {{{ proto string md5(string str, [ bool raw_output])
   Calculate the md5 hash of a string */
PHP_NAMED_FUNCTION(php_if_md5)
{
	char *arg;
	size_t arg_len;
	zend_bool raw_output = 0;
	char md5str[33];
	PHP_MD5_CTX context;
	unsigned char digest[16];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &arg, &arg_len, &raw_output) == FAILURE) {
		return;
	}

	md5str[0] = '\0';
	PHP_MD5Init(&context);
	PHP_MD5Update(&context, arg, arg_len);
	PHP_MD5Final(digest, &context);
	if (raw_output) {
		RETURN_STRINGL(digest, 16);
	} else {
		make_digest(md5str, digest);
		RETVAL_STRING(md5str);
	}

}
/* }}} */

/* {{{ proto string md5_file(string filename [, bool raw_output])
   Calculate the md5 hash of given filename */
PHP_NAMED_FUNCTION(php_if_md5_file)
{
	char          *arg;
	size_t        arg_len;
	zend_bool raw_output = 0;
	char          md5str[33];
	unsigned char buf[1024];
	unsigned char digest[16];
	PHP_MD5_CTX   context;
	int           n;
	php_stream    *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|b", &arg, &arg_len, &raw_output) == FAILURE) {
		return;
	}

	stream = php_stream_open_wrapper(arg, "rb", REPORT_ERRORS, NULL);
	if (!stream) {
		RETURN_FALSE;
	}

	PHP_MD5Init(&context);

	while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		PHP_MD5Update(&context, buf, n);
	}

	PHP_MD5Final(digest, &context);

	php_stream_close(stream);

	if (n<0) {
		RETURN_FALSE;
	}

	if (raw_output) {
		RETURN_STRINGL(digest, 16);
	} else {
		make_digest(md5str, digest);
		RETVAL_STRING(md5str);
	}
}
/* }}} */

/*
 * The remaining code is the reference MD5 code (md5c.c) from rfc1321
 */
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

/* Constants for MD5Transform routine.
 */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform(php_hash_uint32[4], const unsigned char[64]);

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (php_hash_uint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (php_hash_uint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (php_hash_uint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (php_hash_uint32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/* {{{ PHP_MD5Init
 * MD5 initialization. Begins an MD5 operation, writing a new context.
 */
PHP_HASH_API void PHP_MD5Init(PHP_MD5_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}
/* }}} */

/* {{{ PHP_MD5Update
   MD5 block update operation. Continues an MD5 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_MD5Update(PHP_MD5_CTX * context, const unsigned char *input,
			   unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint32) inputLen << 3))
		< ((php_hash_uint32) inputLen << 3))
		context->count[1]++;
	context->count[1] += ((php_hash_uint32) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy
			((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		MD5Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	memcpy
		((unsigned char*) & context->buffer[index], (unsigned char*) & input[i],
		 inputLen - i);
}
/* }}} */

/* {{{ PHP_MD5Final
   MD5 finalization. Ends an MD5 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_MD5Final(unsigned char digest[16], PHP_MD5_CTX * context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	Encode(bits, context->count, 8);

	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PHP_MD5Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_MD5Update(context, bits, 8);

	/* Store state in digest */
	Encode(digest, context->state, 16);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* {{{ MD5Transform
 * MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform(state, block)
php_hash_uint32 state[4];
const unsigned char block[64];
{
	php_hash_uint32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478);	/* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);	/* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db);	/* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);	/* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);	/* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a);	/* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613);	/* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501);	/* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8);	/* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);	/* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);		/* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be);		/* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122);		/* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193);		/* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e);		/* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821);		/* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562);	/* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340);	/* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51);		/* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);	/* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d);	/* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453);	/* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);		/* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);	/* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);	/* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6);		/* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);	/* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed);	/* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);		/* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);	/* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9);	/* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);		/* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942);	/* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681);	/* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);		/* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c);		/* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44);	/* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);	/* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);	/* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);		/* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);		/* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);	/* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);	/* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05);	/* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);	/* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);		/* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);		/* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);	/* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244);	/* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97);	/* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7);		/* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039);	/* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3);		/* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);	/* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d);		/* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1);	/* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);	/* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);		/* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314);	/* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1);		/* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82);	/* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235);		/* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);	/* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391);	/* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	ZEND_SECURE_ZERO((unsigned char*) x, sizeof(x));
}
/* }}} */

#endif /* PHP_HASH_MD5_NOT_IN_CORE */

/* MD4 */

#define MD4_F(x,y,z)			((z) ^ ((x) & ((y) ^ (z))))
#define MD4_G(x,y,z)			(((x) & ((y) | (z))) | ((y) & (z)))
#define MD4_H(x,y,z)			((x) ^ (y) ^ (z))

#define ROTL32(s,v)				(((v) << (s)) | ((v) >> (32 - (s))))

#define MD4_R1(a,b,c,d,k,s)		a = ROTL32(s, a + MD4_F(b,c,d) + x[k])
#define MD4_R2(a,b,c,d,k,s)		a = ROTL32(s, a + MD4_G(b,c,d) + x[k] + 0x5A827999)
#define MD4_R3(a,b,c,d,k,s)		a = ROTL32(s, a + MD4_H(b,c,d) + x[k] + 0x6ED9EBA1)

static void MD4Transform(php_hash_uint32 state[4], const unsigned char block[64])
{
	php_hash_uint32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
	MD4_R1(a,b,c,d, 0, 3);
	MD4_R1(d,a,b,c, 1, 7);
	MD4_R1(c,d,a,b, 2,11);
	MD4_R1(b,c,d,a, 3,19);
	MD4_R1(a,b,c,d, 4, 3);
	MD4_R1(d,a,b,c, 5, 7);
	MD4_R1(c,d,a,b, 6,11);
	MD4_R1(b,c,d,a, 7,19);
	MD4_R1(a,b,c,d, 8, 3);
	MD4_R1(d,a,b,c, 9, 7);
	MD4_R1(c,d,a,b,10,11);
	MD4_R1(b,c,d,a,11,19);
	MD4_R1(a,b,c,d,12, 3);
	MD4_R1(d,a,b,c,13, 7);
	MD4_R1(c,d,a,b,14,11);
	MD4_R1(b,c,d,a,15,19);

	/* Round 2 */
	MD4_R2(a,b,c,d, 0, 3);
	MD4_R2(d,a,b,c, 4, 5);
	MD4_R2(c,d,a,b, 8, 9);
	MD4_R2(b,c,d,a,12,13);
	MD4_R2(a,b,c,d, 1, 3);
	MD4_R2(d,a,b,c, 5, 5);
	MD4_R2(c,d,a,b, 9, 9);
	MD4_R2(b,c,d,a,13,13);
	MD4_R2(a,b,c,d, 2, 3);
	MD4_R2(d,a,b,c, 6, 5);
	MD4_R2(c,d,a,b,10, 9);
	MD4_R2(b,c,d,a,14,13);
	MD4_R2(a,b,c,d, 3, 3);
	MD4_R2(d,a,b,c, 7, 5);
	MD4_R2(c,d,a,b,11, 9);
	MD4_R2(b,c,d,a,15,13);

	/* Round 3 */
	MD4_R3(a,b,c,d, 0, 3);
	MD4_R3(d,a,b,c, 8, 9);
	MD4_R3(c,d,a,b, 4,11);
	MD4_R3(b,c,d,a,12,15);
	MD4_R3(a,b,c,d, 2, 3);
	MD4_R3(d,a,b,c,10, 9);
	MD4_R3(c,d,a,b, 6,11);
	MD4_R3(b,c,d,a,14,15);
	MD4_R3(a,b,c,d, 1, 3);
	MD4_R3(d,a,b,c, 9, 9);
	MD4_R3(c,d,a,b, 5,11);
	MD4_R3(b,c,d,a,13,15);
	MD4_R3(a,b,c,d, 3, 3);
	MD4_R3(d,a,b,c,11, 9);
	MD4_R3(c,d,a,b, 7,11);
	MD4_R3(b,c,d,a,15,15);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

/* {{{ PHP_MD4Init
 * MD4 initialization. Begins an MD4 operation, writing a new context.
 */
PHP_HASH_API void PHP_MD4Init(PHP_MD4_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}
/* }}} */

/* {{{ PHP_MD4Update
   MD4 block update operation. Continues an MD4 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_MD4Update(PHP_MD4_CTX * context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint32) inputLen << 3))
		< ((php_hash_uint32) inputLen << 3))
		context->count[1]++;
	context->count[1] += ((php_hash_uint32) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		MD4Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64) {
			MD4Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) & context->buffer[index], (unsigned char*) & input[i], inputLen - i);
}
/* }}} */

/* {{{ PHP_MD4Final
   MD4 finalization. Ends an MD4 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_MD4Final(unsigned char digest[16], PHP_MD4_CTX * context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	Encode(bits, context->count, 8);

	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PHP_MD4Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_MD4Update(context, bits, 8);

	/* Store state in digest */
	Encode(digest, context->state, 16);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* MD2 */

static const unsigned char MD2_S[256] = {
	 41,  46,  67, 201, 162, 216, 124,   1,  61,  54,  84, 161, 236, 240,   6,  19,
	 98, 167,   5, 243, 192, 199, 115, 140, 152, 147,  43, 217, 188,  76, 130, 202,
	 30, 155,  87,  60, 253, 212, 224,  22, 103,  66, 111,  24, 138,  23, 229,  18,
	190,  78, 196, 214, 218, 158, 222,  73, 160, 251, 245, 142, 187,  47, 238, 122,
	169, 104, 121, 145,  21, 178,   7,  63, 148, 194,  16, 137,  11,  34,  95,  33,
	128, 127,  93, 154,  90, 144,  50,  39,  53,  62, 204, 231, 191, 247, 151,   3,
	255,  25,  48, 179,  72, 165, 181, 209, 215,  94, 146,  42, 172,  86, 170, 198,
	 79, 184,  56, 210, 150, 164, 125, 182, 118, 252, 107, 226, 156, 116,   4, 241,
	 69, 157, 112,  89, 100, 113, 135,  32, 134,  91, 207, 101, 230,  45, 168,   2,
	 27,  96,  37, 173, 174, 176, 185, 246,  28,  70,  97, 105,  52,  64, 126,  15,
	 85,  71, 163,  35, 221,  81, 175,  58, 195,  92, 249, 206, 186, 197, 234,  38,
	 44,  83,  13, 110, 133,  40, 132,   9, 211, 223, 205, 244,  65, 129,  77,  82,
	106, 220,  55, 200, 108, 193, 171, 250,  36, 225, 123,   8,  12, 189, 177,  74,
	120, 136, 149, 139, 227,  99, 232, 109, 233, 203, 213, 254,  59,   0,  29,  57,
	242, 239, 183,  14, 102,  88, 208, 228, 166, 119, 114, 248, 235, 117,  75,  10,
	 49,  68,  80, 180, 143, 237,  31,  26, 219, 153, 141,  51, 159,  17, 131,  20 };

PHP_HASH_API void PHP_MD2Init(PHP_MD2_CTX *context)
{
	memset(context, 0, sizeof(PHP_MD2_CTX));
}

static void MD2_Transform(PHP_MD2_CTX *context, const unsigned char *block)
{
	unsigned char i,j,t = 0;

	for(i = 0; i < 16; i++) {
		context->state[16+i] = block[i];
		context->state[32+i] = (context->state[16+i] ^ context->state[i]);
	}

	for(i = 0; i < 18; i++) {
		for(j = 0; j < 48; j++) {
			t = context->state[j] = context->state[j] ^ MD2_S[t];
		}
		t += i;
	}

	/* Update checksum -- must be after transform to avoid fouling up last message block */
	t = context->checksum[15];
	for(i = 0; i < 16; i++) {
		t = context->checksum[i] ^= MD2_S[block[i] ^ t];
	}
}

PHP_HASH_API void PHP_MD2Update(PHP_MD2_CTX *context, const unsigned char *buf, unsigned int len)
{
	const unsigned char *p = buf, *e = buf + len;

	if (context->in_buffer) {
		if (context->in_buffer + len < 16) {
			/* Not enough for block, just pass into buffer */
			memcpy(context->buffer + context->in_buffer, p, len);
			context->in_buffer += len;
			return;
		}
		/* Put buffered data together with inbound for a single block */
		memcpy(context->buffer + context->in_buffer, p, 16 - context->in_buffer);
		MD2_Transform(context, context->buffer);
		p += 16 - context->in_buffer;
		context->in_buffer = 0;
	}

	/* Process as many whole blocks as remain */
	while ((p + 16) <= e) {
		MD2_Transform(context, p);
		p += 16;
	}

	/* Copy remaining data to buffer */
	if (p < e) {
		memcpy(context->buffer, p, e - p);
		context->in_buffer = e - p;
	}
}

PHP_HASH_API void PHP_MD2Final(unsigned char output[16], PHP_MD2_CTX *context)
{
	memset(context->buffer + context->in_buffer, 16 - context->in_buffer, 16 - context->in_buffer);
	MD2_Transform(context, context->buffer);
	MD2_Transform(context, context->checksum);

	memcpy(output, context->state, 16);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
