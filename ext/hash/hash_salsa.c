/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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
#include "php_hash_salsa.h"

#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

/* {{{ Salsa10
 
 The 64-byte input x to Salsa10 is viewed in little-endian form as 16 integers 
 x0, x1, x2, ..., x15 in {0,1,...,2^32-1}. These 16 integers are fed through 
 320 invertible modifications, where each modification changes one integer. 
 The modifications involve, overall,

    * 10 additions of constants modulo 2^32;
    * 320 more additions modulo 2^32;
    * 80 ``or'' operations;
    * 240 ``xor'' operations; and
    * 320 constant-distance rotations. 

 The resulting 16 integers are added to the original x0, x1, x2, ..., x15 
 respectively modulo 2^32, producing, in little-endian form, the 64-byte output 
 Salsa10(x).
 
 D.J.Bernstein
*/
static void Salsa10(php_hash_uint32 x[16], php_hash_uint32 in[16])
{
	int i;
	
	for (i = 10; i > 0; --i) {
		x[ 4] ^= R(x[ 0]+x[12], 6);  x[ 8] ^= R(x[ 4]+x[ 0],17);
		x[12] += R(x[ 8]|x[ 4],16);  x[ 0] += R(x[12]^x[ 8], 5);
		x[ 9] += R(x[ 5]|x[ 1], 8);  x[13] += R(x[ 9]|x[ 5], 7);
		x[ 1] ^= R(x[13]+x[ 9],17);  x[ 5] += R(x[ 1]^x[13],12);
		x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] += R(x[14]^x[10],15);
		x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],15);
		x[ 3] += R(x[15]|x[11],20);  x[ 7] ^= R(x[ 3]+x[15],16);
		x[11] += R(x[ 7]^x[ 3], 7);  x[15] += R(x[11]^x[ 7], 8);
		x[ 1] += R(x[ 0]|x[ 3], 8)^i;x[ 2] ^= R(x[ 1]+x[ 0],14);
		x[ 3] ^= R(x[ 2]+x[ 1], 6);  x[ 0] += R(x[ 3]^x[ 2],18);
		x[ 6] += R(x[ 5]^x[ 4], 8);  x[ 7] += R(x[ 6]^x[ 5],12);
		x[ 4] += R(x[ 7]|x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],15);
		x[11] ^= R(x[10]+x[ 9],18);  x[ 8] += R(x[11]^x[10],11);
		x[ 9] ^= R(x[ 8]+x[11], 8);  x[10] += R(x[ 9]|x[ 8], 6);
		x[12] += R(x[15]^x[14],17);  x[13] ^= R(x[12]+x[15],15);
		x[14] += R(x[13]|x[12], 9);  x[15] += R(x[14]^x[13], 7);
	}
	for (i = 0; i < 16; ++i) {
		x[i] += in[i];
	}
}
/* }}} */

/* {{{ Salsa20
 
 The 64-byte input x to Salsa20 is viewed in little-endian form as 16 words 
 x0, x1, x2, ..., x15 in {0,1,...,2^32-1}. These 16 words are fed through 320 
 invertible modifications, where each modification changes one word. The 
 resulting 16 words are added to the original x0, x1, x2, ..., x15 respectively 
 modulo 2^32, producing, in little-endian form, the 64-byte output Salsa20(x).

 Each modification involves xor'ing into one word a rotated version of the sum 
 of two other words modulo 2^32. Thus the 320 modifications involve, overall, 
 320 additions, 320 xor's, and 320 rotations. The rotations are all by constant 
 distances.

 The entire series of modifications is a series of 10 identical double-rounds. 
 Each double-round is a series of 2 rounds. Each round is a set of 4 parallel 
 quarter-rounds. Each quarter-round modifies 4 words.
 
 D.J.Bernstein
*/
static void Salsa20(php_hash_uint32 x[16], php_hash_uint32 in[16])
{
	int i;
	
	for (i = 20; i > 0; i -= 2) {
		x[ 4] ^= R(x[ 0]+x[12], 7);  x[ 8] ^= R(x[ 4]+x[ 0], 9);
		x[12] ^= R(x[ 8]+x[ 4],13);  x[ 0] ^= R(x[12]+x[ 8],18);
		x[ 9] ^= R(x[ 5]+x[ 1], 7);  x[13] ^= R(x[ 9]+x[ 5], 9);
		x[ 1] ^= R(x[13]+x[ 9],13);  x[ 5] ^= R(x[ 1]+x[13],18);
		x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] ^= R(x[14]+x[10], 9);
		x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],18);
		x[ 3] ^= R(x[15]+x[11], 7);  x[ 7] ^= R(x[ 3]+x[15], 9);
		x[11] ^= R(x[ 7]+x[ 3],13);  x[15] ^= R(x[11]+x[ 7],18);
		x[ 1] ^= R(x[ 0]+x[ 3], 7);  x[ 2] ^= R(x[ 1]+x[ 0], 9);
		x[ 3] ^= R(x[ 2]+x[ 1],13);  x[ 0] ^= R(x[ 3]+x[ 2],18);
		x[ 6] ^= R(x[ 5]+x[ 4], 7);  x[ 7] ^= R(x[ 6]+x[ 5], 9);
		x[ 4] ^= R(x[ 7]+x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],18);
		x[11] ^= R(x[10]+x[ 9], 7);  x[ 8] ^= R(x[11]+x[10], 9);
		x[ 9] ^= R(x[ 8]+x[11],13);  x[10] ^= R(x[ 9]+x[ 8],18);
		x[12] ^= R(x[15]+x[14], 7);  x[13] ^= R(x[12]+x[15], 9);
		x[14] ^= R(x[13]+x[12],13);  x[15] ^= R(x[14]+x[13],18);
	}
	for (i = 0; i < 16; ++i) {
		x[i] += in[i];
	}
}
/* }}} */

static inline void SalsaTransform(PHP_SALSA_CTX *context, const unsigned char input[64])
{
	php_hash_uint32 i, j, a[16];

#if 0
	fprintf(stderr, "> INPUT: %.*s\n", 64, input);
#endif
	
	for (i = 0, j = 0; j < 64; i++, j += 4) {
		a[i] = ((php_hash_uint32) input[j + 3]) | (((php_hash_uint32) input[j + 2]) << 8) |
			(((php_hash_uint32) input[j + 1]) << 16) | (((php_hash_uint32) input[j]) << 24);
	}
	
	if (!context->init) {
		memcpy(context->state, a, sizeof(a));
		context->init = 1;
	}
	
	context->Transform(context->state, a);
	memset(a, 0, sizeof(a));
}

PHP_HASH_API void PHP_SALSA10Init(PHP_SALSA_CTX *context)
{
	memset(context, 0, sizeof(*context));
	context->Transform = Salsa10;
}

PHP_HASH_API void PHP_SALSA20Init(PHP_SALSA_CTX *context)
{
	memset(context, 0, sizeof(*context));
	context->Transform = Salsa20;
}

PHP_HASH_API void PHP_SALSAUpdate(PHP_SALSA_CTX *context, const unsigned char *input, size_t len)
{
	if (context->length + len < 64) {
		memcpy(&context->buffer[context->length], input, len);
		context->length += len;
	} else {
		size_t i = 0, r = (context->length + len) % 64;
		
		if (context->length) {
			i = 64 - context->length;
			memcpy(&context->buffer[context->length], input, i);
			SalsaTransform(context, context->buffer);
			memset(context->buffer, 0, 64);
		}
		
		for (; i + 64 <= len; i += 64) {
			SalsaTransform(context, input + i);
		}
		
		memcpy(context->buffer, input + i, r);
		context->length = r;
	}
}

PHP_HASH_API void PHP_SALSAFinal(unsigned char digest[64], PHP_SALSA_CTX *context)
{
	php_hash_uint32 i, j;
	
	if (context->length) {
		SalsaTransform(context, context->buffer);
	}
	
	for (i = 0, j = 0; j < 64; i++, j += 4) {
		digest[j] = (unsigned char) ((context->state[i] >> 24) & 0xff);
		digest[j + 1] = (unsigned char) ((context->state[i] >> 16) & 0xff);
		digest[j + 2] = (unsigned char) ((context->state[i] >> 8) & 0xff);
		digest[j + 3] = (unsigned char) (context->state[i] & 0xff);
	}
	
	memset(context, 0, sizeof(*context));
}

const php_hash_ops php_hash_salsa10_ops = {
	(php_hash_init_func_t) PHP_SALSA10Init,
	(php_hash_update_func_t) PHP_SALSAUpdate,
	(php_hash_final_func_t) PHP_SALSAFinal,
	64,
	64,
	sizeof(PHP_SALSA_CTX)
};

const php_hash_ops php_hash_salsa20_ops = {
	(php_hash_init_func_t) PHP_SALSA20Init,
	(php_hash_update_func_t) PHP_SALSAUpdate,
	(php_hash_final_func_t) PHP_SALSAFinal,
	64,
	64,
	sizeof(PHP_SALSA_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
