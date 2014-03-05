/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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
#include "php_hash_gost.h"
#include "php_hash_gost_tables.h"

/* {{{ Gost()
 * derived from gost_compress() by Markku-Juhani Saarinen <mjos@ssh.fi>
 */

#define round(k1, k2) \
	t = (k1) + r; \
	l ^= tables[0][t & 0xff] ^ tables[1][(t >> 8) & 0xff] ^ \
		tables[2][(t >> 16) & 0xff] ^ tables[3][t >> 24]; \
	t = (k2) + l; \
	r ^= tables[0][t & 0xff] ^ tables[1][(t >> 8) & 0xff] ^ \
		tables[2][(t >> 16) & 0xff] ^ tables[3][t >> 24];

#define R(key, h, i, t, l, r) \
	r = h[i]; \
	l = h[i + 1]; \
	round(key[0], key[1]) \
	round(key[2], key[3]) \
	round(key[4], key[5]) \
	round(key[6], key[7]) \
	round(key[0], key[1]) \
	round(key[2], key[3]) \
	round(key[4], key[5]) \
	round(key[6], key[7]) \
	round(key[0], key[1]) \
	round(key[2], key[3]) \
	round(key[4], key[5]) \
	round(key[6], key[7]) \
	round(key[7], key[6]) \
	round(key[5], key[4]) \
	round(key[3], key[2]) \
	round(key[1], key[0]) \
	t = r; \
	r = l; \
	l = t; \

#define X(w, u, v) \
	w[0] = u[0] ^ v[0]; \
	w[1] = u[1] ^ v[1]; \
	w[2] = u[2] ^ v[2]; \
	w[3] = u[3] ^ v[3]; \
	w[4] = u[4] ^ v[4]; \
	w[5] = u[5] ^ v[5]; \
	w[6] = u[6] ^ v[6]; \
	w[7] = u[7] ^ v[7];

#define P(key, w) \
	key[0] = (w[0]  & 0x000000ff) | ((w[2] & 0x000000ff) << 8) | \
		((w[4] & 0x000000ff) << 16) | ((w[6] & 0x000000ff) << 24); \
	key[1] = ((w[0] & 0x0000ff00) >> 8)  | (w[2]  & 0x0000ff00) | \
		((w[4] & 0x0000ff00) << 8) | ((w[6] & 0x0000ff00) << 16); \
	key[2] = ((w[0] & 0x00ff0000) >> 16) | ((w[2] & 0x00ff0000) >> 8) | \
		(w[4] & 0x00ff0000) | ((w[6] & 0x00ff0000) << 8); \
	key[3] = ((w[0] & 0xff000000) >> 24) | ((w[2] & 0xff000000) >> 16) | \
		((w[4] & 0xff000000) >> 8) | (w[6] & 0xff000000); \
	key[4] = (w[1] & 0x000000ff) | ((w[3] & 0x000000ff) << 8) | \
		((w[5] & 0x000000ff) << 16) | ((w[7] & 0x000000ff) << 24); \
	key[5] = ((w[1] & 0x0000ff00) >> 8) | (w[3]  & 0x0000ff00) | \
		((w[5] & 0x0000ff00) << 8) | ((w[7] & 0x0000ff00) << 16); \
	key[6] = ((w[1] & 0x00ff0000) >> 16) | ((w[3] & 0x00ff0000) >> 8) | \
		(w[5] & 0x00ff0000) | ((w[7] & 0x00ff0000) << 8); \
	key[7] = ((w[1] & 0xff000000) >> 24) | ((w[3] & 0xff000000) >> 16) | \
		((w[5] & 0xff000000) >> 8) | (w[7] & 0xff000000);  

#define A(x, l, r) \
	l = x[0] ^ x[2]; \
	r = x[1] ^ x[3]; \
	x[0] = x[2]; \
	x[1] = x[3]; \
	x[2] = x[4]; \
	x[3] = x[5]; \
	x[4] = x[6]; \
	x[5] = x[7]; \
	x[6] = l; \
	x[7] = r;

#define AA(x, l, r) \
	l = x[0]; \
	r = x[2]; \
	x[0] = x[4]; \
	x[2] = x[6]; \
	x[4] = l ^ r; \
	x[6] = x[0] ^ r; \
	l = x[1]; \
	r = x[3]; \
	x[1] = x[5]; \
	x[3] = x[7]; \
	x[5] = l ^ r; \
	x[7] = x[1] ^ r;

#define C(x) \
	x[0] ^= 0xff00ff00; \
	x[1] ^= 0xff00ff00; \
	x[2] ^= 0x00ff00ff; \
	x[3] ^= 0x00ff00ff; \
	x[4] ^= 0x00ffff00; \
	x[5] ^= 0xff0000ff; \
	x[6] ^= 0x000000ff; \
	x[7] ^= 0xff00ffff;

#define S(s, l, r) \
		s[i] = r; \
		s[i + 1] = l;

#define SHIFT12(u, m, s) \
	u[0] = m[0] ^ s[6]; \
	u[1] = m[1] ^ s[7]; \
	u[2] = m[2] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff) ^ \
		(s[1] & 0xffff) ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[6] ^ (s[6] << 16) ^ \
		(s[7] & 0xffff0000) ^ (s[7] >> 16); \
	u[3] = m[3] ^ (s[0] & 0xffff) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^ \
		(s[1] << 16) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^ \
		(s[3] << 16) ^ s[6] ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^ \
		(s[7] << 16) ^ (s[7] >> 16); \
	u[4] = m[4] ^ \
		(s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[0] >> 16) ^ \
		(s[1] & 0xffff0000) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^ \
		(s[3] << 16) ^ (s[3] >> 16) ^ (s[4] << 16) ^ (s[6] << 16) ^ \
		(s[6] >> 16) ^(s[7] & 0xffff) ^ (s[7] << 16) ^ (s[7] >> 16); \
	u[5] = m[5] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff0000) ^ \
		(s[1] & 0xffff) ^ s[2] ^ (s[2] >> 16) ^ (s[3] << 16) ^ (s[3] >> 16) ^ \
		(s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^  (s[6] << 16) ^ \
		(s[6] >> 16) ^ (s[7] & 0xffff0000) ^ (s[7] << 16) ^ (s[7] >> 16); \
	u[6] = m[6] ^ s[0] ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[3] ^ (s[3] >> 16) ^ \
		(s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[5] >> 16) ^ s[6] ^ \
		(s[6] << 16) ^ (s[6] >> 16) ^ (s[7] << 16); \
	u[7] = m[7] ^ (s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^ \
		(s[1] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ s[4] ^ (s[4] >> 16) ^ \
		(s[5] << 16) ^ (s[5] >> 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^ \
		(s[7] << 16) ^ (s[7] >> 16);

#define SHIFT16(h, v, u) \
	v[0] = h[0] ^ (u[1] << 16) ^ (u[0] >> 16); \
	v[1] = h[1] ^ (u[2] << 16) ^ (u[1] >> 16); \
	v[2] = h[2] ^ (u[3] << 16) ^ (u[2] >> 16); \
	v[3] = h[3] ^ (u[4] << 16) ^ (u[3] >> 16); \
	v[4] = h[4] ^ (u[5] << 16) ^ (u[4] >> 16); \
	v[5] = h[5] ^ (u[6] << 16) ^ (u[5] >> 16); \
	v[6] = h[6] ^ (u[7] << 16) ^ (u[6] >> 16); \
	v[7] = h[7] ^ (u[0] & 0xffff0000) ^ (u[0] << 16) ^ (u[7] >> 16) ^ \
		(u[1] & 0xffff0000) ^ (u[1] << 16) ^ (u[6] << 16) ^ (u[7] & 0xffff0000);

#define SHIFT61(h, v) \
	h[0] = (v[0] & 0xffff0000) ^ (v[0] << 16) ^ (v[0] >> 16) ^ (v[1] >> 16) ^ \
		(v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ (v[4] << 16) ^ \
		(v[5] >> 16) ^ v[5] ^ (v[6] >> 16) ^ (v[7] << 16) ^ (v[7] >> 16) ^ \
		(v[7] & 0xffff); \
	h[1] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^ (v[1] & 0xffff) ^ \
	v[2] ^ (v[2] >> 16) ^ (v[3] << 16) ^ (v[4] >> 16) ^ (v[5] << 16) ^ \
		(v[6] << 16) ^ v[6] ^ (v[7] & 0xffff0000) ^ (v[7] >> 16); \
	h[2] = (v[0] & 0xffff) ^ (v[0] << 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^ \
		(v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ \
		(v[5] >> 16) ^ v[6] ^ (v[6] >> 16) ^ (v[7] & 0xffff) ^ (v[7] << 16) ^ \
		(v[7] >> 16); \
	h[3] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^ \
		(v[1] & 0xffff0000) ^ (v[1] >> 16) ^ (v[2] << 16) ^ (v[2] >> 16) ^ v[2] ^ \
		(v[3] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^ \
		(v[7] & 0xffff) ^ (v[7] >> 16); \
	h[4] = (v[0] >> 16) ^ (v[1] << 16) ^ v[1] ^ (v[2] >> 16) ^ v[2] ^ \
		(v[3] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ (v[5] >> 16) ^ \
	v[5] ^ (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16); \
	h[5] = (v[0] << 16) ^ (v[0] & 0xffff0000) ^ (v[1] << 16) ^ (v[1] >> 16) ^ \
		(v[1] & 0xffff0000) ^ (v[2] << 16) ^ v[2] ^ (v[3] >> 16) ^ v[3] ^ \
		(v[4] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^ \
		(v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ (v[7] >> 16) ^ (v[7] & 0xffff0000); \
	h[6] = v[0] ^ v[2] ^ (v[2] >> 16) ^ v[3] ^ (v[3] << 16) ^ v[4] ^ \
		(v[4] >> 16) ^ (v[5] << 16) ^ (v[5] >> 16) ^ v[5] ^ (v[6] << 16) ^ \
		(v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ v[7]; \
	h[7] = v[0] ^ (v[0] >> 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^ (v[2] << 16) ^ \
		(v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ v[4] ^ (v[5] >> 16) ^ v[5] ^ \
		(v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16) ^ v[7];

#define PASS \
	X(w, u, v); \
	P(key, w); \
	R(key, h, i, t, l, r); \
	S(s, l, r); \
	if (i != 6) { \
		A(u, l, r); \
		if (i == 2)	{ \
			C(u); \
		} \
		AA(v, l, r); \
	}

static inline void Gost(php_hash_uint32 state[8], php_hash_uint32 data[8])
{
	int i;
	php_hash_uint32 l, r, t, key[8], u[8], v[8], w[8], s[8], *h = state, *m = data;
	
	memcpy(u, state, sizeof(u));
	memcpy(v, data, sizeof(v));
	
	for (i = 0; i < 8; i += 2) {
		PASS;
	}
	SHIFT12(u, m, s);
	SHIFT16(h, v, u);
	SHIFT61(h, v);
}
/* }}} */

static inline void GostTransform(PHP_GOST_CTX *context, const unsigned char input[32])
{
	int i, j;
	php_hash_uint32 data[8], temp = 0, save = 0;
	
	for (i = 0, j = 0; i < 8; ++i, j += 4) {
		data[i] =	((php_hash_uint32) input[j]) | (((php_hash_uint32) input[j + 1]) << 8) | 
					(((php_hash_uint32) input[j + 2]) << 16) | (((php_hash_uint32) input[j + 3]) << 24);
		save = context->state[i + 8];
		context->state[i + 8] += data[i] + temp;
		temp = ((context->state[i + 8] < data[i]) || (context->state[i + 8] < save)) ? 1 : 0;     
	}
	
	Gost(context->state, data);
}

PHP_HASH_API void PHP_GOSTInit(PHP_GOST_CTX *context)
{
	memset(context, 0, sizeof(*context));
}

static const php_hash_uint32 MAX32 = 0xffffffffLU;

PHP_HASH_API void PHP_GOSTUpdate(PHP_GOST_CTX *context, const unsigned char *input, size_t len)
{
	if ((MAX32 - context->count[0]) < (len * 8)) {
		context->count[1]++;
		context->count[0] = MAX32 - context->count[0];
		context->count[0] = (len * 8) - context->count[0];
	} else {
		context->count[0] += len * 8;
	}
	
	if (context->length + len < 32) {
		memcpy(&context->buffer[context->length], input, len);
		context->length += len;
	} else {
		size_t i = 0, r = (context->length + len) % 32;
		
		if (context->length) {
			i = 32 - context->length;
			memcpy(&context->buffer[context->length], input, i);
			GostTransform(context, context->buffer);
		}
		
		for (; i + 32 <= len; i += 32) {
			GostTransform(context, input + i);
		}
		
		memcpy(context->buffer, input + i, r);
		memset(&context->buffer[r], 0, 32 - r);
		context->length = r;
	}
}

PHP_HASH_API void PHP_GOSTFinal(unsigned char digest[32], PHP_GOST_CTX *context)
{
	php_hash_uint32 i, j, l[8] = {0};
	
	if (context->length) {
		GostTransform(context, context->buffer);
	}
	
	memcpy(l, context->count, sizeof(context->count));
	Gost(context->state, l);
	memcpy(l, &context->state[8], sizeof(l));
	Gost(context->state, l);
	
	for (i = 0, j = 0; j < 32; i++, j += 4) {
		digest[j] = (unsigned char) (context->state[i] & 0xff);
		digest[j + 1] = (unsigned char) ((context->state[i] >> 8) & 0xff);
		digest[j + 2] = (unsigned char) ((context->state[i] >> 16) & 0xff);
		digest[j + 3] = (unsigned char) ((context->state[i] >> 24) & 0xff);
	}
	
	memset(context, 0, sizeof(*context));
}

const php_hash_ops php_hash_gost_ops = {
	(php_hash_init_func_t) PHP_GOSTInit,
	(php_hash_update_func_t) PHP_GOSTUpdate,
	(php_hash_final_func_t) PHP_GOSTFinal,
	(php_hash_copy_func_t) php_hash_copy,
	32,
	32,
	sizeof(PHP_GOST_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
