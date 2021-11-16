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
  | Authors: Michael Wallner <mike@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_snefru.h"
#include "php_hash_snefru_tables.h"

#define round(L, C, N, SB)	\
	SBE = SB[C & 0xff]; \
	L ^= SBE; \
	N ^= SBE

#ifndef DBG_SNEFRU
#define DBG_SNEFRU 0
#endif

#if DBG_SNEFRU
void ph(uint32_t h[16])
{
	int i;
	for (i = 0; i < 16; i++)
		printf ("%08lx", h[i]); printf("\n");
}
#endif

static inline void Snefru(uint32_t input[16])
{
	static int shifts[4] = {16, 8, 16, 24};
	int b, index, rshift, lshift;
	const uint32_t *t0,*t1;
	uint32_t SBE,B00,B01,B02,B03,B04,B05,B06,B07,B08,B09,B10,B11,B12,B13,B14,B15;

	B00 = input[0];
	B01 = input[1];
	B02 = input[2];
	B03 = input[3];
	B04 = input[4];
	B05 = input[5];
	B06 = input[6];
	B07 = input[7];
	B08 = input[8];
	B09 = input[9];
	B10 = input[10];
	B11 = input[11];
	B12 = input[12];
	B13 = input[13];
	B14 = input[14];
	B15 = input[15];

	for (index = 0; index < 8; index++) {
		t0 = tables[2*index+0];
		t1 = tables[2*index+1];
		for (b = 0; b < 4; b++) {
			round(B15, B00, B01, t0);
			round(B00, B01, B02, t0);
			round(B01, B02, B03, t1);
			round(B02, B03, B04, t1);
			round(B03, B04, B05, t0);
			round(B04, B05, B06, t0);
			round(B05, B06, B07, t1);
			round(B06, B07, B08, t1);
			round(B07, B08, B09, t0);
			round(B08, B09, B10, t0);
			round(B09, B10, B11, t1);
			round(B10, B11, B12, t1);
			round(B11, B12, B13, t0);
			round(B12, B13, B14, t0);
			round(B13, B14, B15, t1);
			round(B14, B15, B00, t1);

			rshift = shifts[b];
			lshift = 32-rshift;

			B00 = (B00 >> rshift) | (B00 << lshift);
			B01 = (B01 >> rshift) | (B01 << lshift);
			B02 = (B02 >> rshift) | (B02 << lshift);
			B03 = (B03 >> rshift) | (B03 << lshift);
			B04 = (B04 >> rshift) | (B04 << lshift);
			B05 = (B05 >> rshift) | (B05 << lshift);
			B06 = (B06 >> rshift) | (B06 << lshift);
			B07 = (B07 >> rshift) | (B07 << lshift);
			B08 = (B08 >> rshift) | (B08 << lshift);
			B09 = (B09 >> rshift) | (B09 << lshift);
			B10 = (B10 >> rshift) | (B10 << lshift);
			B11 = (B11 >> rshift) | (B11 << lshift);
			B12 = (B12 >> rshift) | (B12 << lshift);
			B13 = (B13 >> rshift) | (B13 << lshift);
			B14 = (B14 >> rshift) | (B14 << lshift);
			B15 = (B15 >> rshift) | (B15 << lshift);
		}
	}
	input[0] ^= B15;
	input[1] ^= B14;
	input[2] ^= B13;
	input[3] ^= B12;
	input[4] ^= B11;
	input[5] ^= B10;
	input[6] ^= B09;
	input[7] ^= B08;
#if DBG_SNEFRU
	ph(input);
#endif
}

static inline void SnefruTransform(PHP_SNEFRU_CTX *context, const unsigned char input[32])
{
	int i, j;

	for (i = 0, j = 0; i < 32; i += 4, ++j) {
		context->state[8+j] =	((unsigned)input[i] << 24) | ((unsigned)input[i+1] << 16) |
								((unsigned)input[i+2] << 8) | (unsigned)input[i+3];
	}
	Snefru(context->state);
	ZEND_SECURE_ZERO(&context->state[8], sizeof(uint32_t) * 8);
}

PHP_HASH_API void PHP_SNEFRUInit(PHP_SNEFRU_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	memset(context, 0, sizeof(*context));
}

static const uint32_t MAX32 = 0xffffffffLU;

PHP_HASH_API void PHP_SNEFRUUpdate(PHP_SNEFRU_CTX *context, const unsigned char *input, size_t len)
{
	if ((MAX32 - context->count[1]) < (len * 8)) {
		context->count[0]++;
		context->count[1] = MAX32 - context->count[1];
		context->count[1] = ((uint32_t) len * 8) - context->count[1];
	} else {
		context->count[1] += (uint32_t) len * 8;
	}

	if (context->length + len < 32) {
		memcpy(&context->buffer[context->length], input, len);
		context->length += (unsigned char)len;
	} else {
		size_t i = 0, r = (context->length + len) % 32;

		if (context->length) {
			i = 32 - context->length;
			memcpy(&context->buffer[context->length], input, i);
			SnefruTransform(context, context->buffer);
		}

		for (; i + 32 <= len; i += 32) {
			SnefruTransform(context, input + i);
		}

		memcpy(context->buffer, input + i, r);
		ZEND_SECURE_ZERO(&context->buffer[r], 32 - r);
		context->length = (unsigned char)r;
	}
}

PHP_HASH_API void PHP_SNEFRUFinal(unsigned char digest[32], PHP_SNEFRU_CTX *context)
{
	uint32_t i, j;

	if (context->length) {
		SnefruTransform(context, context->buffer);
	}

	context->state[14] = context->count[0];
	context->state[15] = context->count[1];
	Snefru(context->state);

	for (i = 0, j = 0; j < 32; i++, j += 4) {
		digest[j] = (unsigned char) ((context->state[i] >> 24) & 0xff);
		digest[j + 1] = (unsigned char) ((context->state[i] >> 16) & 0xff);
		digest[j + 2] = (unsigned char) ((context->state[i] >> 8) & 0xff);
		digest[j + 3] = (unsigned char) (context->state[i] & 0xff);
	}

	ZEND_SECURE_ZERO(context, sizeof(*context));
}

static int php_snefru_unserialize(php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	PHP_SNEFRU_CTX *ctx = (PHP_SNEFRU_CTX *) hash->context;
	int r = FAILURE;
	if (magic == PHP_HASH_SERIALIZE_MAGIC_SPEC
		&& (r = php_hash_unserialize_spec(hash, zv, PHP_SNEFRU_SPEC)) == SUCCESS
		&& ctx->length < sizeof(ctx->buffer)) {
		return SUCCESS;
	} else {
		return r != SUCCESS ? r : -2000;
	}
}

const php_hash_ops php_hash_snefru_ops = {
	"snefru",
	(php_hash_init_func_t) PHP_SNEFRUInit,
	(php_hash_update_func_t) PHP_SNEFRUUpdate,
	(php_hash_final_func_t) PHP_SNEFRUFinal,
	php_hash_copy,
	php_hash_serialize,
	php_snefru_unserialize,
	PHP_SNEFRU_SPEC,
	32,
	32,
	sizeof(PHP_SNEFRU_CTX),
	1
};
