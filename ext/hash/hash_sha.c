/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Steffan Esser <sesser@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_hash.h"
#include "php_hash_sha.h"

static const unsigned char PADDING[128] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* {{{ SHAEncode32
   Encodes input (php_hash_uint32) into output (unsigned char). Assumes len is
   a multiple of 4.
 */
static void SHAEncode32(unsigned char *output, php_hash_uint32 *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char) ((input[i] >> 24) & 0xff);
		output[j + 1] = (unsigned char) ((input[i] >> 16) & 0xff);
		output[j + 2] = (unsigned char) ((input[i] >> 8) & 0xff);
		output[j + 3] = (unsigned char) (input[i] & 0xff);
	}
}
/* }}} */


/* {{{ SHADecode32
   Decodes input (unsigned char) into output (php_hash_uint32). Assumes len is
   a multiple of 4.
 */
static void SHADecode32(php_hash_uint32 *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((php_hash_uint32) input[j + 3]) | (((php_hash_uint32) input[j + 2]) << 8) |
			(((php_hash_uint32) input[j + 1]) << 16) | (((php_hash_uint32) input[j]) << 24);
}
/* }}} */

const php_hash_ops php_hash_sha1_ops = {
	(php_hash_init_func_t) PHP_SHA1Init,
	(php_hash_update_func_t) PHP_SHA1Update,
	(php_hash_final_func_t) PHP_SHA1Final,
	(php_hash_copy_func_t) php_hash_copy,
	20,
	64,
	sizeof(PHP_SHA1_CTX)
};

#ifdef PHP_HASH_SHA1_NOT_IN_CORE

PHP_HASH_API void make_sha1_digest(char *sha1str, unsigned char *digest)
{
	php_hash_bin2hex(sha1str, digest, 20);
	sha1str[40] = '\0';
}

/* {{{ proto string sha1(string str [, bool raw_output])
   Calculate the sha1 hash of a string */
PHP_FUNCTION(sha1)
{
	char *arg;
	int arg_len;
	zend_bool raw_output = 0;
	char sha1str[41];
	PHP_SHA1_CTX context;
	unsigned char digest[20];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &arg, &arg_len, &raw_output) == FAILURE) {
		return;
	}

	sha1str[0] = '\0';
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, arg, arg_len);
	PHP_SHA1Final(digest, &context);
	if (raw_output) {
		RETURN_STRINGL(digest, 20, 1);
	} else {
		make_sha1_digest(sha1str, digest);
		RETVAL_STRING(sha1str, 1);
	}

}

/* }}} */

/* {{{ proto string sha1_file(string filename [, bool raw_output])
   Calculate the sha1 hash of given filename */
PHP_FUNCTION(sha1_file)
{
	char          *arg;
	int           arg_len;
	zend_bool raw_output = 0;
	char          sha1str[41];
	unsigned char buf[1024];
	unsigned char digest[20];
	PHP_SHA1_CTX   context;
	int           n;
	php_stream    *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|b", &arg, &arg_len, &raw_output) == FAILURE) {
		return;
	}
	
	stream = php_stream_open_wrapper(arg, "rb", REPORT_ERRORS, NULL);
	if (!stream) {
		RETURN_FALSE;
	}

	PHP_SHA1Init(&context);

	while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		PHP_SHA1Update(&context, buf, n);
	}

	PHP_SHA1Final(digest, &context);

	php_stream_close(stream);

	if (n<0) {
		RETURN_FALSE;
	}

	if (raw_output) {
		RETURN_STRINGL(digest, 20, 1);
	} else {
		make_sha1_digest(sha1str, digest);
		RETVAL_STRING(sha1str, 1);
	}
}
/* }}} */

/* F, G, H and I are basic SHA1 functions.
 */
#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) ((x) ^ (y) ^ (z))
#define H(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define I(x, y, z) ((x) ^ (y) ^ (z))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* W[i]
 */
#define W(i) ( tmp=x[(i-3)&15]^x[(i-8)&15]^x[(i-14)&15]^x[i&15], \
	(x[i&15]=ROTATE_LEFT(tmp, 1)) )  

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 */
#define FF(a, b, c, d, e, w) { \
 (e) += F ((b), (c), (d)) + (w) + (php_hash_uint32)(0x5A827999); \
 (e) += ROTATE_LEFT ((a), 5); \
 (b) = ROTATE_LEFT((b), 30); \
  }
#define GG(a, b, c, d, e, w) { \
 (e) += G ((b), (c), (d)) + (w) + (php_hash_uint32)(0x6ED9EBA1); \
 (e) += ROTATE_LEFT ((a), 5); \
 (b) = ROTATE_LEFT((b), 30); \
  }
#define HH(a, b, c, d, e, w) { \
 (e) += H ((b), (c), (d)) + (w) + (php_hash_uint32)(0x8F1BBCDC); \
 (e) += ROTATE_LEFT ((a), 5); \
 (b) = ROTATE_LEFT((b), 30); \
  }
#define II(a, b, c, d, e, w) { \
 (e) += I ((b), (c), (d)) + (w) + (php_hash_uint32)(0xCA62C1D6); \
 (e) += ROTATE_LEFT ((a), 5); \
 (b) = ROTATE_LEFT((b), 30); \
  }
			                    

/* {{{ PHP_SHA1Init
 * SHA1 initialization. Begins an SHA1 operation, writing a new context.
 */
PHP_HASH_API void PHP_SHA1Init(PHP_SHA1_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
	context->state[4] = 0xc3d2e1f0;
}
/* }}} */

/* {{{ SHA1Transform
 * SHA1 basic transformation. Transforms state based on block.
 */
static void SHA1Transform(php_hash_uint32 state[5], const unsigned char block[64])
{
	php_hash_uint32 a = state[0], b = state[1], c = state[2];
	php_hash_uint32 d = state[3], e = state[4], x[16], tmp;

	SHADecode32(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, e, x[0]);   /* 1 */
	FF(e, a, b, c, d, x[1]);   /* 2 */
	FF(d, e, a, b, c, x[2]);   /* 3 */
	FF(c, d, e, a, b, x[3]);   /* 4 */
	FF(b, c, d, e, a, x[4]);   /* 5 */
	FF(a, b, c, d, e, x[5]);   /* 6 */
	FF(e, a, b, c, d, x[6]);   /* 7 */
	FF(d, e, a, b, c, x[7]);   /* 8 */
	FF(c, d, e, a, b, x[8]);   /* 9 */
	FF(b, c, d, e, a, x[9]);   /* 10 */
	FF(a, b, c, d, e, x[10]);  /* 11 */
	FF(e, a, b, c, d, x[11]);  /* 12 */
	FF(d, e, a, b, c, x[12]);  /* 13 */
	FF(c, d, e, a, b, x[13]);  /* 14 */
	FF(b, c, d, e, a, x[14]);  /* 15 */
	FF(a, b, c, d, e, x[15]);  /* 16 */
	FF(e, a, b, c, d, W(16));  /* 17 */
	FF(d, e, a, b, c, W(17));  /* 18 */
	FF(c, d, e, a, b, W(18));  /* 19 */
	FF(b, c, d, e, a, W(19));  /* 20 */

	/* Round 2 */
	GG(a, b, c, d, e, W(20));  /* 21 */
	GG(e, a, b, c, d, W(21));  /* 22 */
	GG(d, e, a, b, c, W(22));  /* 23 */
	GG(c, d, e, a, b, W(23));  /* 24 */
	GG(b, c, d, e, a, W(24));  /* 25 */
	GG(a, b, c, d, e, W(25));  /* 26 */
	GG(e, a, b, c, d, W(26));  /* 27 */
	GG(d, e, a, b, c, W(27));  /* 28 */
	GG(c, d, e, a, b, W(28));  /* 29 */
	GG(b, c, d, e, a, W(29));  /* 30 */
	GG(a, b, c, d, e, W(30));  /* 31 */
	GG(e, a, b, c, d, W(31));  /* 32 */
	GG(d, e, a, b, c, W(32));  /* 33 */
	GG(c, d, e, a, b, W(33));  /* 34 */
	GG(b, c, d, e, a, W(34));  /* 35 */
	GG(a, b, c, d, e, W(35));  /* 36 */
	GG(e, a, b, c, d, W(36));  /* 37 */
	GG(d, e, a, b, c, W(37));  /* 38 */
	GG(c, d, e, a, b, W(38));  /* 39 */
	GG(b, c, d, e, a, W(39));  /* 40 */

	/* Round 3 */
	HH(a, b, c, d, e, W(40));  /* 41 */
	HH(e, a, b, c, d, W(41));  /* 42 */
	HH(d, e, a, b, c, W(42));  /* 43 */
	HH(c, d, e, a, b, W(43));  /* 44 */
	HH(b, c, d, e, a, W(44));  /* 45 */
	HH(a, b, c, d, e, W(45));  /* 46 */
	HH(e, a, b, c, d, W(46));  /* 47 */
	HH(d, e, a, b, c, W(47));  /* 48 */
	HH(c, d, e, a, b, W(48));  /* 49 */
	HH(b, c, d, e, a, W(49));  /* 50 */
	HH(a, b, c, d, e, W(50));  /* 51 */
	HH(e, a, b, c, d, W(51));  /* 52 */
	HH(d, e, a, b, c, W(52));  /* 53 */
	HH(c, d, e, a, b, W(53));  /* 54 */
	HH(b, c, d, e, a, W(54));  /* 55 */
	HH(a, b, c, d, e, W(55));  /* 56 */
	HH(e, a, b, c, d, W(56));  /* 57 */
	HH(d, e, a, b, c, W(57));  /* 58 */
	HH(c, d, e, a, b, W(58));  /* 59 */
	HH(b, c, d, e, a, W(59));  /* 60 */

	/* Round 4 */
	II(a, b, c, d, e, W(60));  /* 61 */
	II(e, a, b, c, d, W(61));  /* 62 */
	II(d, e, a, b, c, W(62));  /* 63 */
	II(c, d, e, a, b, W(63));  /* 64 */
	II(b, c, d, e, a, W(64));  /* 65 */
	II(a, b, c, d, e, W(65));  /* 66 */
	II(e, a, b, c, d, W(66));  /* 67 */
	II(d, e, a, b, c, W(67));  /* 68 */
	II(c, d, e, a, b, W(68));  /* 69 */
	II(b, c, d, e, a, W(69));  /* 70 */
	II(a, b, c, d, e, W(70));  /* 71 */
	II(e, a, b, c, d, W(71));  /* 72 */
	II(d, e, a, b, c, W(72));  /* 73 */
	II(c, d, e, a, b, W(73));  /* 74 */
	II(b, c, d, e, a, W(74));  /* 75 */
	II(a, b, c, d, e, W(75));  /* 76 */
	II(e, a, b, c, d, W(76));  /* 77 */
	II(d, e, a, b, c, W(77));  /* 78 */
	II(c, d, e, a, b, W(78));  /* 79 */
	II(b, c, d, e, a, W(79));  /* 80 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	/* Zeroize sensitive information. */
	memset((unsigned char*) x, 0, sizeof(x));
}
/* }}} */

/* {{{ PHP_SHA1Update
   SHA1 block update operation. Continues an SHA1 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_SHA1Update(PHP_SHA1_CTX * context, const unsigned char *input,
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
		SHA1Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			SHA1Transform(context->state, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	memcpy
		((unsigned char*) & context->buffer[index], (unsigned char*) & input[i],
		 inputLen - i);
}
/* }}} */

/* {{{ PHP_SHA1Final
   SHA1 finalization. Ends an SHA1 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_SHA1Final(unsigned char digest[20], PHP_SHA1_CTX * context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[7] = context->count[0] & 0xFF;
	bits[6] = (context->count[0] >> 8) & 0xFF;
	bits[5] = (context->count[0] >> 16) & 0xFF;
	bits[4] = (context->count[0] >> 24) & 0xFF;
	bits[3] = context->count[1] & 0xFF;
	bits[2] = (context->count[1] >> 8) & 0xFF;
	bits[1] = (context->count[1] >> 16) & 0xFF;
	bits[0] = (context->count[1] >> 24) & 0xFF;
	
	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PHP_SHA1Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_SHA1Update(context, bits, 8);

	/* Store state in digest */
	SHAEncode32(digest, context->state, 20);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) context, 0, sizeof(*context));
}
/* }}} */

#endif /* PHP_HASH_SHA1_NOT_IN_CORE */

/* sha224/sha256 */

const php_hash_ops php_hash_sha256_ops = {
	(php_hash_init_func_t) PHP_SHA256Init,
	(php_hash_update_func_t) PHP_SHA256Update,
	(php_hash_final_func_t) PHP_SHA256Final,
	(php_hash_copy_func_t) php_hash_copy,
	32,
	64,
	sizeof(PHP_SHA256_CTX)
};

const php_hash_ops php_hash_sha224_ops = {
	(php_hash_init_func_t) PHP_SHA224Init,
	(php_hash_update_func_t) PHP_SHA224Update,
	(php_hash_final_func_t) PHP_SHA224Final,
	(php_hash_copy_func_t) php_hash_copy,
	28,
	64,
	sizeof(PHP_SHA224_CTX)
};

#define ROTR32(b,x)		((x >> b) | (x << (32 - b)))
#define ROTR64(b,x)		((x >> b) | (x << (64 - b)))
#define SHR(b, x)		(x >> b)

/* Ch */
#define SHA256_F0(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
/* Maj */
#define SHA256_F1(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
/* SUM0 */
#define SHA256_F2(x)		(ROTR32( 2,(x)) ^ ROTR32(13,(x)) ^ ROTR32(22,(x)))
/* SUM1 */
#define SHA256_F3(x)		(ROTR32( 6,(x)) ^ ROTR32(11,(x)) ^ ROTR32(25,(x)))
/* OM0 */
#define SHA256_F4(x)		(ROTR32( 7,(x)) ^ ROTR32(18,(x)) ^ SHR( 3,(x)))
/* OM1 */
#define SHA256_F5(x)		(ROTR32(17,(x)) ^ ROTR32(19,(x)) ^ SHR(10,(x)))

static const php_hash_uint32 SHA256_K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

/* {{{ PHP_SHA256Init
 * SHA256 initialization. Begins an SHA256 operation, writing a new context.
 */
PHP_HASH_API void PHP_SHA256Init(PHP_SHA256_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = 0x6a09e667;
	context->state[1] = 0xbb67ae85;
	context->state[2] = 0x3c6ef372;
	context->state[3] = 0xa54ff53a;
	context->state[4] = 0x510e527f;
	context->state[5] = 0x9b05688c;
	context->state[6] = 0x1f83d9ab;
	context->state[7] = 0x5be0cd19;
}
/* }}} */

/* {{{ SHA256Transform
 * SHA256 basic transformation. Transforms state based on block.
 */
static void SHA256Transform(php_hash_uint32 state[8], const unsigned char block[64])
{
	php_hash_uint32 a = state[0], b = state[1], c = state[2], d = state[3];
	php_hash_uint32 e = state[4], f = state[5], g = state[6], h = state[7];
	php_hash_uint32 x[16], T1, T2, W[64];
	int i;

	SHADecode32(x, block, 64);

	/* Schedule */
	for(i = 0; i < 16; i++) {
		W[i] = x[i];
	}
	for(i = 16; i < 64; i++) {
		W[i] = SHA256_F5(W[i-2]) + W[i-7] + SHA256_F4(W[i-15]) + W[i-16];
	}

	for (i = 0; i < 64; i++) {
		T1 = h + SHA256_F3(e) + SHA256_F0(e,f,g) + SHA256_K[i] + W[i];
		T2 = SHA256_F2(a) + SHA256_F1(a,b,c);
		h = g; g = f; f = e; e = d + T1;
		d = c; c = b; b = a; a = T1 + T2;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;

	/* Zeroize sensitive information. */
	memset((unsigned char*) x, 0, sizeof(x));
}
/* }}} */

/* {{{ PHP_SHA224Init
 * SHA224 initialization. Begins an SHA224 operation, writing a new context.
 */
PHP_HASH_API void PHP_SHA224Init(PHP_SHA224_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = 0xc1059ed8;
	context->state[1] = 0x367cd507;
	context->state[2] = 0x3070dd17;
	context->state[3] = 0xf70e5939;
	context->state[4] = 0xffc00b31;
	context->state[5] = 0x68581511;
	context->state[6] = 0x64f98fa7;
	context->state[7] = 0xbefa4fa4;
}
/* }}} */

/* {{{ PHP_SHA224Update
   SHA224 block update operation. Continues an SHA224 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_SHA224Update(PHP_SHA224_CTX * context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint32) inputLen << 3)) < ((php_hash_uint32) inputLen << 3)) {
		context->count[1]++;
	}
	context->count[1] += ((php_hash_uint32) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		SHA256Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64) {
			SHA256Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) & context->buffer[index], (unsigned char*) & input[i], inputLen - i);
}
/* }}} */

/* {{{ PHP_SHA224Final
   SHA224 finalization. Ends an SHA224 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_SHA224Final(unsigned char digest[28], PHP_SHA224_CTX * context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[7] = (unsigned char) (context->count[0] & 0xFF);
	bits[6] = (unsigned char) ((context->count[0] >> 8) & 0xFF);
	bits[5] = (unsigned char) ((context->count[0] >> 16) & 0xFF);
	bits[4] = (unsigned char) ((context->count[0] >> 24) & 0xFF);
	bits[3] = (unsigned char) (context->count[1] & 0xFF);
	bits[2] = (unsigned char) ((context->count[1] >> 8) & 0xFF);
	bits[1] = (unsigned char) ((context->count[1] >> 16) & 0xFF);
	bits[0] = (unsigned char) ((context->count[1] >> 24) & 0xFF);

	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PHP_SHA224Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_SHA224Update(context, bits, 8);

	/* Store state in digest */
	SHAEncode32(digest, context->state, 28);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) context, 0, sizeof(*context));
}
/* }}} */

/* {{{ PHP_SHA256Update
   SHA256 block update operation. Continues an SHA256 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_SHA256Update(PHP_SHA256_CTX * context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint32) inputLen << 3)) < ((php_hash_uint32) inputLen << 3)) {
		context->count[1]++;
	}
	context->count[1] += ((php_hash_uint32) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		SHA256Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64) {
			SHA256Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) & context->buffer[index], (unsigned char*) & input[i], inputLen - i);
}
/* }}} */

/* {{{ PHP_SHA256Final
   SHA256 finalization. Ends an SHA256 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_SHA256Final(unsigned char digest[32], PHP_SHA256_CTX * context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[7] = (unsigned char) (context->count[0] & 0xFF);
	bits[6] = (unsigned char) ((context->count[0] >> 8) & 0xFF);
	bits[5] = (unsigned char) ((context->count[0] >> 16) & 0xFF);
	bits[4] = (unsigned char) ((context->count[0] >> 24) & 0xFF);
	bits[3] = (unsigned char) (context->count[1] & 0xFF);
	bits[2] = (unsigned char) ((context->count[1] >> 8) & 0xFF);
	bits[1] = (unsigned char) ((context->count[1] >> 16) & 0xFF);
	bits[0] = (unsigned char) ((context->count[1] >> 24) & 0xFF);
	
	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	PHP_SHA256Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_SHA256Update(context, bits, 8);

	/* Store state in digest */
	SHAEncode32(digest, context->state, 32);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) context, 0, sizeof(*context));
}
/* }}} */

/* sha384/sha512 */

/* Ch */
#define SHA512_F0(x,y,z)		(((x) & (y)) ^ ((~(x)) & (z)))
/* Maj */
#define SHA512_F1(x,y,z)		(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
/* SUM0 */
#define SHA512_F2(x)			(ROTR64(28, x) ^ ROTR64(34, x) ^ ROTR64(39, x))
/* SUM1 */
#define SHA512_F3(x)			(ROTR64(14, x) ^ ROTR64(18, x) ^ ROTR64(41, x))
/* OM0 */
#define SHA512_F4(x)			(ROTR64( 1, x) ^ ROTR64( 8, x) ^ SHR(7, x))
/* OM1 */
#define SHA512_F5(x)			(ROTR64(19, x) ^ ROTR64(61, x) ^ SHR(6, x))

static const php_hash_uint64 SHA512_K[128] = {
	L64(0x428a2f98d728ae22), L64(0x7137449123ef65cd), L64(0xb5c0fbcfec4d3b2f), L64(0xe9b5dba58189dbbc),
	L64(0x3956c25bf348b538), L64(0x59f111f1b605d019), L64(0x923f82a4af194f9b), L64(0xab1c5ed5da6d8118),
	L64(0xd807aa98a3030242), L64(0x12835b0145706fbe), L64(0x243185be4ee4b28c), L64(0x550c7dc3d5ffb4e2),
	L64(0x72be5d74f27b896f), L64(0x80deb1fe3b1696b1), L64(0x9bdc06a725c71235), L64(0xc19bf174cf692694),
	L64(0xe49b69c19ef14ad2), L64(0xefbe4786384f25e3), L64(0x0fc19dc68b8cd5b5), L64(0x240ca1cc77ac9c65),
	L64(0x2de92c6f592b0275), L64(0x4a7484aa6ea6e483), L64(0x5cb0a9dcbd41fbd4), L64(0x76f988da831153b5),
	L64(0x983e5152ee66dfab), L64(0xa831c66d2db43210), L64(0xb00327c898fb213f), L64(0xbf597fc7beef0ee4),
	L64(0xc6e00bf33da88fc2), L64(0xd5a79147930aa725), L64(0x06ca6351e003826f), L64(0x142929670a0e6e70),
	L64(0x27b70a8546d22ffc), L64(0x2e1b21385c26c926), L64(0x4d2c6dfc5ac42aed), L64(0x53380d139d95b3df),
	L64(0x650a73548baf63de), L64(0x766a0abb3c77b2a8), L64(0x81c2c92e47edaee6), L64(0x92722c851482353b),
	L64(0xa2bfe8a14cf10364), L64(0xa81a664bbc423001), L64(0xc24b8b70d0f89791), L64(0xc76c51a30654be30),
	L64(0xd192e819d6ef5218), L64(0xd69906245565a910), L64(0xf40e35855771202a), L64(0x106aa07032bbd1b8),
	L64(0x19a4c116b8d2d0c8), L64(0x1e376c085141ab53), L64(0x2748774cdf8eeb99), L64(0x34b0bcb5e19b48a8),
	L64(0x391c0cb3c5c95a63), L64(0x4ed8aa4ae3418acb), L64(0x5b9cca4f7763e373), L64(0x682e6ff3d6b2b8a3),
	L64(0x748f82ee5defb2fc), L64(0x78a5636f43172f60), L64(0x84c87814a1f0ab72), L64(0x8cc702081a6439ec),
	L64(0x90befffa23631e28), L64(0xa4506cebde82bde9), L64(0xbef9a3f7b2c67915), L64(0xc67178f2e372532b),
	L64(0xca273eceea26619c), L64(0xd186b8c721c0c207), L64(0xeada7dd6cde0eb1e), L64(0xf57d4f7fee6ed178),
	L64(0x06f067aa72176fba), L64(0x0a637dc5a2c898a6), L64(0x113f9804bef90dae), L64(0x1b710b35131c471b),
	L64(0x28db77f523047d84), L64(0x32caab7b40c72493), L64(0x3c9ebe0a15c9bebc), L64(0x431d67c49c100d4c),
	L64(0x4cc5d4becb3e42b6), L64(0x597f299cfc657e2a), L64(0x5fcb6fab3ad6faec), L64(0x6c44198c4a475817) };

/* {{{ SHAEncode64
   Encodes input (php_hash_uint64) into output (unsigned char). Assumes len is
   a multiple of 8.
 */
static void SHAEncode64(unsigned char *output, php_hash_uint64 *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 8) {
		output[j] = (unsigned char) ((input[i] >> 56) & 0xff);
		output[j + 1] = (unsigned char) ((input[i] >> 48) & 0xff);
		output[j + 2] = (unsigned char) ((input[i] >> 40) & 0xff);
		output[j + 3] = (unsigned char) ((input[i] >> 32) & 0xff);
		output[j + 4] = (unsigned char) ((input[i] >> 24) & 0xff);
		output[j + 5] = (unsigned char) ((input[i] >> 16) & 0xff);
		output[j + 6] = (unsigned char) ((input[i] >> 8) & 0xff);
		output[j + 7] = (unsigned char) (input[i] & 0xff);
	}
}
/* }}} */


/* {{{ SHADecode64
   Decodes input (unsigned char) into output (php_hash_uint64). Assumes len is
   a multiple of 8.
 */
static void SHADecode64(php_hash_uint64 *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 8)
		output[i] = 
			((php_hash_uint64) input[j + 7]) | (((php_hash_uint64) input[j + 6]) << 8) |
			(((php_hash_uint64) input[j + 5]) << 16) | (((php_hash_uint64) input[j + 4]) << 24) |
			(((php_hash_uint64) input[j + 3]) << 32) | (((php_hash_uint64) input[j + 2]) << 40) |
			(((php_hash_uint64) input[j + 1]) << 48) | (((php_hash_uint64) input[j]) << 56);
}
/* }}} */

/* {{{ PHP_SHA384Init
 * SHA384 initialization. Begins an SHA384 operation, writing a new context.
 */
PHP_HASH_API void PHP_SHA384Init(PHP_SHA384_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = L64(0xcbbb9d5dc1059ed8);
	context->state[1] = L64(0x629a292a367cd507);
	context->state[2] = L64(0x9159015a3070dd17);
	context->state[3] = L64(0x152fecd8f70e5939);
	context->state[4] = L64(0x67332667ffc00b31);
	context->state[5] = L64(0x8eb44a8768581511);
	context->state[6] = L64(0xdb0c2e0d64f98fa7);
	context->state[7] = L64(0x47b5481dbefa4fa4);
}
/* }}} */

/* {{{ SHA512Transform
 * SHA512 basic transformation. Transforms state based on block.
 * SHA384 uses the exact same algorithm
 */
static void SHA512Transform(php_hash_uint64 state[8], const unsigned char block[128])
{
	php_hash_uint64 a = state[0], b = state[1], c = state[2], d = state[3];
	php_hash_uint64 e = state[4], f = state[5], g = state[6], h = state[7];
	php_hash_uint64 x[16], T1, T2, W[80];
	int i;

	SHADecode64(x, block, 128);

	/* Schedule */
	for(i = 0; i < 16; i++) {
		W[i] = x[i];
	}
	for(i = 16; i < 80; i++) {
		W[i] = SHA512_F5(W[i-2]) + W[i-7] + SHA512_F4(W[i-15]) + W[i-16];
	}

	for (i = 0; i < 80; i++) {
		T1 = h + SHA512_F3(e) + SHA512_F0(e,f,g) + SHA512_K[i] + W[i];
		T2 = SHA512_F2(a) + SHA512_F1(a,b,c);
		h = g; g = f; f = e; e = d + T1;
		d = c; c = b; b = a; a = T1 + T2;
	}

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;

	/* Zeroize sensitive information. */
	memset((unsigned char*) x, 0, sizeof(x));
}
/* }}} */

/* {{{ PHP_SHA384Update
   SHA384 block update operation. Continues an SHA384 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_SHA384Update(PHP_SHA384_CTX * context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 128 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint64) inputLen << 3)) < ((php_hash_uint64) inputLen << 3)) {
		context->count[1]++;
	}
	context->count[1] += ((php_hash_uint64) inputLen >> 61);

	partLen = 128 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		SHA512Transform(context->state, context->buffer);

		for (i = partLen; i + 127 < inputLen; i += 128) {
			SHA512Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) & context->buffer[index], (unsigned char*) & input[i], inputLen - i);
}
/* }}} */

/* {{{ PHP_SHA384Final
   SHA384 finalization. Ends an SHA384 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_SHA384Final(unsigned char digest[48], PHP_SHA384_CTX * context)
{
	unsigned char bits[16];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[15] = (unsigned char) (context->count[0] & 0xFF);
	bits[14] = (unsigned char) ((context->count[0] >> 8) & 0xFF);
	bits[13] = (unsigned char) ((context->count[0] >> 16) & 0xFF);
	bits[12] = (unsigned char) ((context->count[0] >> 24) & 0xFF);
	bits[11] = (unsigned char) ((context->count[0] >> 32) & 0xFF);
	bits[10] = (unsigned char) ((context->count[0] >> 40) & 0xFF);
	bits[9]  = (unsigned char) ((context->count[0] >> 48) & 0xFF);
	bits[8]  = (unsigned char) ((context->count[0] >> 56) & 0xFF);
	bits[7]  = (unsigned char) (context->count[1] & 0xFF);
	bits[6]  = (unsigned char) ((context->count[1] >> 8) & 0xFF);
	bits[5]  = (unsigned char) ((context->count[1] >> 16) & 0xFF);
	bits[4]  = (unsigned char) ((context->count[1] >> 24) & 0xFF);
	bits[3]  = (unsigned char) ((context->count[1] >> 32) & 0xFF);
	bits[2]  = (unsigned char) ((context->count[1] >> 40) & 0xFF);
	bits[1]  = (unsigned char) ((context->count[1] >> 48) & 0xFF);
	bits[0]  = (unsigned char) ((context->count[1] >> 56) & 0xFF);
	
	/* Pad out to 112 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 112) ? (112 - index) : (240 - index);
	PHP_SHA384Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_SHA384Update(context, bits, 16);

	/* Store state in digest */
	SHAEncode64(digest, context->state, 48);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) context, 0, sizeof(*context));
}
/* }}} */

const php_hash_ops php_hash_sha384_ops = {
	(php_hash_init_func_t) PHP_SHA384Init,
	(php_hash_update_func_t) PHP_SHA384Update,
	(php_hash_final_func_t) PHP_SHA384Final,
	(php_hash_copy_func_t) php_hash_copy,
	48,
	128,
	sizeof(PHP_SHA384_CTX)
};

/* {{{ PHP_SHA512Init
 * SHA512 initialization. Begins an SHA512 operation, writing a new context.
 */
PHP_HASH_API void PHP_SHA512Init(PHP_SHA512_CTX * context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	 */
	context->state[0] = L64(0x6a09e667f3bcc908);
	context->state[1] = L64(0xbb67ae8584caa73b);
	context->state[2] = L64(0x3c6ef372fe94f82b);
	context->state[3] = L64(0xa54ff53a5f1d36f1);
	context->state[4] = L64(0x510e527fade682d1);
	context->state[5] = L64(0x9b05688c2b3e6c1f);
	context->state[6] = L64(0x1f83d9abfb41bd6b);
	context->state[7] = L64(0x5be0cd19137e2179);
}
/* }}} */

/* {{{ PHP_SHA512Update
   SHA512 block update operation. Continues an SHA512 message-digest
   operation, processing another message block, and updating the
   context.
 */
PHP_HASH_API void PHP_SHA512Update(PHP_SHA512_CTX * context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 128 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7F);

	/* Update number of bits */
	if ((context->count[0] += ((php_hash_uint64) inputLen << 3)) < ((php_hash_uint64) inputLen << 3)) {
		context->count[1]++;
	}
	context->count[1] += ((php_hash_uint64) inputLen >> 61);

	partLen = 128 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		SHA512Transform(context->state, context->buffer);

		for (i = partLen; i + 127 < inputLen; i += 128) {
			SHA512Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) & context->buffer[index], (unsigned char*) & input[i], inputLen - i);
}
/* }}} */

/* {{{ PHP_SHA512Final
   SHA512 finalization. Ends an SHA512 message-digest operation, writing the
   the message digest and zeroizing the context.
 */
PHP_HASH_API void PHP_SHA512Final(unsigned char digest[64], PHP_SHA512_CTX * context)
{
	unsigned char bits[16];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[15] = (unsigned char) (context->count[0] & 0xFF);
	bits[14] = (unsigned char) ((context->count[0] >> 8) & 0xFF);
	bits[13] = (unsigned char) ((context->count[0] >> 16) & 0xFF);
	bits[12] = (unsigned char) ((context->count[0] >> 24) & 0xFF);
	bits[11] = (unsigned char) ((context->count[0] >> 32) & 0xFF);
	bits[10] = (unsigned char) ((context->count[0] >> 40) & 0xFF);
	bits[9]  = (unsigned char) ((context->count[0] >> 48) & 0xFF);
	bits[8]  = (unsigned char) ((context->count[0] >> 56) & 0xFF);
	bits[7]  = (unsigned char) (context->count[1] & 0xFF);
	bits[6]  = (unsigned char) ((context->count[1] >> 8) & 0xFF);
	bits[5]  = (unsigned char) ((context->count[1] >> 16) & 0xFF);
	bits[4]  = (unsigned char) ((context->count[1] >> 24) & 0xFF);
	bits[3]  = (unsigned char) ((context->count[1] >> 32) & 0xFF);
	bits[2]  = (unsigned char) ((context->count[1] >> 40) & 0xFF);
	bits[1]  = (unsigned char) ((context->count[1] >> 48) & 0xFF);
	bits[0]  = (unsigned char) ((context->count[1] >> 56) & 0xFF);

	/* Pad out to 112 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 112) ? (112 - index) : (240 - index);
	PHP_SHA512Update(context, PADDING, padLen);

	/* Append length (before padding) */
	PHP_SHA512Update(context, bits, 16);

	/* Store state in digest */
	SHAEncode64(digest, context->state, 64);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) context, 0, sizeof(*context));
}
/* }}} */

const php_hash_ops php_hash_sha512_ops = {
	(php_hash_init_func_t) PHP_SHA512Init,
	(php_hash_update_func_t) PHP_SHA512Update,
	(php_hash_final_func_t) PHP_SHA512Final,
	(php_hash_copy_func_t) php_hash_copy,
	64,
	128,
	sizeof(PHP_SHA512_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
