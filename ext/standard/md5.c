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
   | Author: Alexander Peslyak (Solar Designer) <solar at openwall.com>   |
   |         Lachlan Roche                                                |
   |         Alessandro Astarita <aleast@capri.it>                        |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "md5.h"

PHPAPI void make_digest(char *md5str, const unsigned char *digest) /* {{{ */
{
	make_digest_ex(md5str, digest, 16);
}
/* }}} */

PHPAPI void make_digest_ex(char *md5str, const unsigned char *digest, int len) /* {{{ */
{
	static const char hexits[17] = "0123456789abcdef";
	int i;

	for (i = 0; i < len; i++) {
		md5str[i * 2]       = hexits[digest[i] >> 4];
		md5str[(i * 2) + 1] = hexits[digest[i] &  0x0F];
	}
	md5str[len * 2] = '\0';
}
/* }}} */

/* {{{ Calculate the md5 hash of a string */
PHP_FUNCTION(md5)
{
	zend_string *arg;
	bool raw_output = 0;
	PHP_MD5_CTX context;
	unsigned char digest[16];

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(arg)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	PHP_MD5Init(&context);
	PHP_MD5Update(&context, ZSTR_VAL(arg), ZSTR_LEN(arg));
	PHP_MD5Final(digest, &context);
	if (raw_output) {
		RETURN_STRINGL((char *) digest, 16);
	} else {
		RETVAL_NEW_STR(zend_string_alloc(32, 0));
		make_digest_ex(Z_STRVAL_P(return_value), digest, 16);
	}

}
/* }}} */

/* {{{ Calculate the md5 hash of given filename */
PHP_FUNCTION(md5_file)
{
	char          *arg;
	size_t           arg_len;
	bool raw_output = 0;
	unsigned char buf[1024];
	unsigned char digest[16];
	PHP_MD5_CTX   context;
	ssize_t       n;
	php_stream    *stream;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(arg, arg_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	stream = php_stream_open_wrapper(arg, "rb", REPORT_ERRORS, NULL);
	if (!stream) {
		RETURN_FALSE;
	}

	PHP_MD5Init(&context);

	while ((n = php_stream_read(stream, (char*)buf, sizeof(buf))) > 0) {
		PHP_MD5Update(&context, buf, n);
	}

	/* XXX this probably can be improved with some number of retries */
	if (!php_stream_eof(stream)) {
		php_stream_close(stream);
		PHP_MD5Final(digest, &context);

		RETURN_FALSE;
	}

	php_stream_close(stream);

	PHP_MD5Final(digest, &context);

	if (raw_output) {
		RETURN_STRINGL((char *) digest, 16);
	} else {
		RETVAL_NEW_STR(zend_string_alloc(32, 0));
		make_digest_ex(Z_STRVAL_P(return_value), digest, 16);
	}
}
/* }}} */

/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security,
 * Inc. MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Written by Solar Designer <solar at openwall.com> in 2001, and placed
 * in the public domain.  There's absolutely no warranty.
 *
 * This differs from Colin Plumb's older public domain implementation in
 * that no 32-bit integer data type is required, there's no compile-time
 * endianness configuration, and the function prototypes match OpenSSL's.
 * The primary goals are portability and ease of use.
 *
 * This implementation is meant to be fast, but not as fast as possible.
 * Some known optimizations are not included to reduce source code size
 * and avoid compile-time configuration.
 */

#include <string.h>

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, just like in Colin Plumb's
 * implementation.
 */
#define F(x, y, z)			((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)			((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)			((x) ^ (y) ^ (z))
#define I(x, y, z)			((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
	(a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
typedef ZEND_SET_ALIGNED(1, uint32_t unaligned_uint32_t);
# define SET(n) \
	(*(unaligned_uint32_t *)&ptr[(n) * 4])
# define GET(n) \
	SET(n)
#else
# define SET(n) \
	(ctx->block[(n)] = \
	(uint32_t)ptr[(n) * 4] | \
	((uint32_t)ptr[(n) * 4 + 1] << 8) | \
	((uint32_t)ptr[(n) * 4 + 2] << 16) | \
	((uint32_t)ptr[(n) * 4 + 3] << 24))
# define GET(n) \
	(ctx->block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There are no alignment requirements.
 */
static const void *body(PHP_MD5_CTX *ctx, const void *data, size_t size)
{
	const unsigned char *ptr;
	uint32_t a, b, c, d;
	uint32_t saved_a, saved_b, saved_c, saved_d;

	ptr = data;

	a = ctx->a;
	b = ctx->b;
	c = ctx->c;
	d = ctx->d;

	do {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;

/* Round 1 */
		STEP(F, a, b, c, d, SET(0), 0xd76aa478, 7)
		STEP(F, d, a, b, c, SET(1), 0xe8c7b756, 12)
		STEP(F, c, d, a, b, SET(2), 0x242070db, 17)
		STEP(F, b, c, d, a, SET(3), 0xc1bdceee, 22)
		STEP(F, a, b, c, d, SET(4), 0xf57c0faf, 7)
		STEP(F, d, a, b, c, SET(5), 0x4787c62a, 12)
		STEP(F, c, d, a, b, SET(6), 0xa8304613, 17)
		STEP(F, b, c, d, a, SET(7), 0xfd469501, 22)
		STEP(F, a, b, c, d, SET(8), 0x698098d8, 7)
		STEP(F, d, a, b, c, SET(9), 0x8b44f7af, 12)
		STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17)
		STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
		STEP(F, a, b, c, d, SET(12), 0x6b901122, 7)
		STEP(F, d, a, b, c, SET(13), 0xfd987193, 12)
		STEP(F, c, d, a, b, SET(14), 0xa679438e, 17)
		STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)

/* Round 2 */
		STEP(G, a, b, c, d, GET(1), 0xf61e2562, 5)
		STEP(G, d, a, b, c, GET(6), 0xc040b340, 9)
		STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14)
		STEP(G, b, c, d, a, GET(0), 0xe9b6c7aa, 20)
		STEP(G, a, b, c, d, GET(5), 0xd62f105d, 5)
		STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
		STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14)
		STEP(G, b, c, d, a, GET(4), 0xe7d3fbc8, 20)
		STEP(G, a, b, c, d, GET(9), 0x21e1cde6, 5)
		STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9)
		STEP(G, c, d, a, b, GET(3), 0xf4d50d87, 14)
		STEP(G, b, c, d, a, GET(8), 0x455a14ed, 20)
		STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5)
		STEP(G, d, a, b, c, GET(2), 0xfcefa3f8, 9)
		STEP(G, c, d, a, b, GET(7), 0x676f02d9, 14)
		STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)

/* Round 3 */
		STEP(H, a, b, c, d, GET(5), 0xfffa3942, 4)
		STEP(H, d, a, b, c, GET(8), 0x8771f681, 11)
		STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16)
		STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23)
		STEP(H, a, b, c, d, GET(1), 0xa4beea44, 4)
		STEP(H, d, a, b, c, GET(4), 0x4bdecfa9, 11)
		STEP(H, c, d, a, b, GET(7), 0xf6bb4b60, 16)
		STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23)
		STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4)
		STEP(H, d, a, b, c, GET(0), 0xeaa127fa, 11)
		STEP(H, c, d, a, b, GET(3), 0xd4ef3085, 16)
		STEP(H, b, c, d, a, GET(6), 0x04881d05, 23)
		STEP(H, a, b, c, d, GET(9), 0xd9d4d039, 4)
		STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11)
		STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16)
		STEP(H, b, c, d, a, GET(2), 0xc4ac5665, 23)

/* Round 4 */
		STEP(I, a, b, c, d, GET(0), 0xf4292244, 6)
		STEP(I, d, a, b, c, GET(7), 0x432aff97, 10)
		STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15)
		STEP(I, b, c, d, a, GET(5), 0xfc93a039, 21)
		STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6)
		STEP(I, d, a, b, c, GET(3), 0x8f0ccc92, 10)
		STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15)
		STEP(I, b, c, d, a, GET(1), 0x85845dd1, 21)
		STEP(I, a, b, c, d, GET(8), 0x6fa87e4f, 6)
		STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10)
		STEP(I, c, d, a, b, GET(6), 0xa3014314, 15)
		STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
		STEP(I, a, b, c, d, GET(4), 0xf7537e82, 6)
		STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10)
		STEP(I, c, d, a, b, GET(2), 0x2ad7d2bb, 15)
		STEP(I, b, c, d, a, GET(9), 0xeb86d391, 21)

		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	ctx->a = a;
	ctx->b = b;
	ctx->c = c;
	ctx->d = d;

	return ptr;
}

PHPAPI void PHP_MD5InitArgs(PHP_MD5_CTX *ctx, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	ctx->a = 0x67452301;
	ctx->b = 0xefcdab89;
	ctx->c = 0x98badcfe;
	ctx->d = 0x10325476;

	ctx->lo = 0;
	ctx->hi = 0;
}

PHPAPI void PHP_MD5Update(PHP_MD5_CTX *ctx, const void *data, size_t size)
{
	uint32_t saved_lo;
	uint32_t used, free;

	saved_lo = ctx->lo;
	if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo) {
		ctx->hi++;
	}
	ctx->hi += size >> 29;

	used = saved_lo & 0x3f;

	if (used) {
		free = 64 - used;

		if (size < free) {
			memcpy(&ctx->buffer[used], data, size);
			return;
		}

		memcpy(&ctx->buffer[used], data, free);
		data = (unsigned char *)data + free;
		size -= free;
		body(ctx, ctx->buffer, 64);
	}

	if (size >= 64) {
		data = body(ctx, data, size & ~(size_t)0x3f);
		size &= 0x3f;
	}

	memcpy(ctx->buffer, data, size);
}

PHPAPI void PHP_MD5Final(unsigned char *result, PHP_MD5_CTX *ctx)
{
	uint32_t used, free;

	used = ctx->lo & 0x3f;

	ctx->buffer[used++] = 0x80;

	free = 64 - used;

	if (free < 8) {
		memset(&ctx->buffer[used], 0, free);
		body(ctx, ctx->buffer, 64);
		used = 0;
		free = 64;
	}

	memset(&ctx->buffer[used], 0, free - 8);

	ctx->lo <<= 3;
	ctx->buffer[56] = ctx->lo;
	ctx->buffer[57] = ctx->lo >> 8;
	ctx->buffer[58] = ctx->lo >> 16;
	ctx->buffer[59] = ctx->lo >> 24;
	ctx->buffer[60] = ctx->hi;
	ctx->buffer[61] = ctx->hi >> 8;
	ctx->buffer[62] = ctx->hi >> 16;
	ctx->buffer[63] = ctx->hi >> 24;

	body(ctx, ctx->buffer, 64);

	result[0] = ctx->a;
	result[1] = ctx->a >> 8;
	result[2] = ctx->a >> 16;
	result[3] = ctx->a >> 24;
	result[4] = ctx->b;
	result[5] = ctx->b >> 8;
	result[6] = ctx->b >> 16;
	result[7] = ctx->b >> 24;
	result[8] = ctx->c;
	result[9] = ctx->c >> 8;
	result[10] = ctx->c >> 16;
	result[11] = ctx->c >> 24;
	result[12] = ctx->d;
	result[13] = ctx->d >> 8;
	result[14] = ctx->d >> 16;
	result[15] = ctx->d >> 24;

	ZEND_SECURE_ZERO(ctx, sizeof(*ctx));
}
