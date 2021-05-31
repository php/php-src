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
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_murmur.h"

#include "murmur/PMurHash.h"
#include "murmur/PMurHash128.h"


const php_hash_ops php_hash_murmur3a_ops = {
	"murmur3a",
	(php_hash_init_func_t) PHP_MURMUR3AInit,
	(php_hash_update_func_t) PHP_MURMUR3AUpdate,
	(php_hash_final_func_t) PHP_MURMUR3AFinal,
	(php_hash_copy_func_t) PHP_MURMUR3ACopy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_MURMUR3A_SPEC,
	4,
	4,
	sizeof(PHP_MURMUR3A_CTX),
	0
};

PHP_HASH_API void PHP_MURMUR3AInit(PHP_MURMUR3A_CTX *ctx, HashTable *args)
{
	if (args) {
		zval *seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (seed && IS_LONG == Z_TYPE_P(seed)) {
			ctx->h = (uint32_t)Z_LVAL_P(seed);
		} else {
			ctx->h = 0;
		}
	} else {
		ctx->h = 0;
	}
	ctx->carry = 0;
	ctx->len = 0;
}

PHP_HASH_API void PHP_MURMUR3AUpdate(PHP_MURMUR3A_CTX *ctx, const unsigned char *in, size_t len)
{
	ctx->len += len;
	PMurHash32_Process(&ctx->h, &ctx->carry, in, len);
}

PHP_HASH_API void PHP_MURMUR3AFinal(unsigned char digest[4], PHP_MURMUR3A_CTX *ctx)
{
	ctx->h = PMurHash32_Result(ctx->h, ctx->carry, ctx->len);

	digest[0] = (unsigned char)((ctx->h >> 24) & 0xff);
	digest[1] = (unsigned char)((ctx->h >> 16) & 0xff);
	digest[2] = (unsigned char)((ctx->h >> 8) & 0xff);
	digest[3] = (unsigned char)(ctx->h & 0xff);
}

PHP_HASH_API int PHP_MURMUR3ACopy(const php_hash_ops *ops, PHP_MURMUR3A_CTX *orig_context, PHP_MURMUR3A_CTX *copy_context)
{
	copy_context->h = orig_context->h;
	copy_context->carry = orig_context->carry;
	copy_context->len = orig_context->len;
	return SUCCESS;
}

const php_hash_ops php_hash_murmur3c_ops = {
	"murmur3c",
	(php_hash_init_func_t) PHP_MURMUR3CInit,
	(php_hash_update_func_t) PHP_MURMUR3CUpdate,
	(php_hash_final_func_t) PHP_MURMUR3CFinal,
	(php_hash_copy_func_t) PHP_MURMUR3CCopy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_MURMUR3C_SPEC,
	16,
	4,
	sizeof(PHP_MURMUR3C_CTX),
	0
};

PHP_HASH_API void PHP_MURMUR3CInit(PHP_MURMUR3C_CTX *ctx, HashTable *args)
{
	if (args) {
		zval *seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (seed && IS_LONG == Z_TYPE_P(seed)) {
			uint32_t _seed = (uint32_t)Z_LVAL_P(seed);
			ctx->h[0] = _seed;
			ctx->h[1] = _seed;
			ctx->h[2] = _seed;
			ctx->h[3] = _seed;
		} else {
			memset(&ctx->h, 0, sizeof ctx->h);
		}
	} else {
		memset(&ctx->h, 0, sizeof ctx->h);
	}
	memset(&ctx->carry, 0, sizeof ctx->carry);
	ctx->len = 0;
}

PHP_HASH_API void PHP_MURMUR3CUpdate(PHP_MURMUR3C_CTX *ctx, const unsigned char *in, size_t len)
{
	ctx->len += len;
	PMurHash128x86_Process(ctx->h, ctx->carry, in, len);
}

PHP_HASH_API void PHP_MURMUR3CFinal(unsigned char digest[16], PHP_MURMUR3C_CTX *ctx)
{
	uint32_t h[4] = {0, 0, 0, 0};
	PMurHash128x86_Result(ctx->h, ctx->carry, ctx->len, h);

	digest[0]  = (unsigned char)((h[0] >> 24) & 0xff);
	digest[1]  = (unsigned char)((h[0] >> 16) & 0xff);
	digest[2]  = (unsigned char)((h[0] >> 8) & 0xff);
	digest[3]  = (unsigned char)(h[0] & 0xff);
	digest[4]  = (unsigned char)((h[1] >> 24) & 0xff);
	digest[5]  = (unsigned char)((h[1] >> 16) & 0xff);
	digest[6]  = (unsigned char)((h[1] >> 8) & 0xff);
	digest[7]  = (unsigned char)(h[1] & 0xff);
	digest[8]  = (unsigned char)((h[2] >> 24) & 0xff);
	digest[9]  = (unsigned char)((h[2] >> 16) & 0xff);
	digest[10] = (unsigned char)((h[2] >> 8) & 0xff);
	digest[11] = (unsigned char)(h[2] & 0xff);
	digest[12] = (unsigned char)((h[3] >> 24) & 0xff);
	digest[13] = (unsigned char)((h[3] >> 16) & 0xff);
	digest[14] = (unsigned char)((h[3] >> 8) & 0xff);
	digest[15] = (unsigned char)(h[3] & 0xff);
}

PHP_HASH_API int PHP_MURMUR3CCopy(const php_hash_ops *ops, PHP_MURMUR3C_CTX *orig_context, PHP_MURMUR3C_CTX *copy_context)
{
	memcpy(&copy_context->h, &orig_context->h, sizeof orig_context->h);
	memcpy(&copy_context->carry, &orig_context->carry, sizeof orig_context->carry);
	copy_context->len = orig_context->len;
	return SUCCESS;
}

const php_hash_ops php_hash_murmur3f_ops = {
	"murmur3f",
	(php_hash_init_func_t) PHP_MURMUR3FInit,
	(php_hash_update_func_t) PHP_MURMUR3FUpdate,
	(php_hash_final_func_t) PHP_MURMUR3FFinal,
	(php_hash_copy_func_t) PHP_MURMUR3FCopy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_MURMUR3F_SPEC,
	16,
	8,
	sizeof(PHP_MURMUR3F_CTX),
	0
};

PHP_HASH_API void PHP_MURMUR3FInit(PHP_MURMUR3F_CTX *ctx, HashTable *args)
{
	if (args) {
		zval *seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (seed && IS_LONG == Z_TYPE_P(seed)) {
			uint64_t _seed = (uint64_t)Z_LVAL_P(seed);
			ctx->h[0] = _seed;
			ctx->h[1] = _seed;
		} else {
			memset(&ctx->h, 0, sizeof ctx->h);
		}
	} else {
		memset(&ctx->h, 0, sizeof ctx->h);
	}
	memset(&ctx->carry, 0, sizeof ctx->carry);
	ctx->len = 0;
}

PHP_HASH_API void PHP_MURMUR3FUpdate(PHP_MURMUR3F_CTX *ctx, const unsigned char *in, size_t len)
{
	ctx->len += len;
	PMurHash128x64_Process(ctx->h, ctx->carry, in, len);
}

PHP_HASH_API void PHP_MURMUR3FFinal(unsigned char digest[16], PHP_MURMUR3F_CTX *ctx)
{
	uint64_t h[2] = {0, 0};
	PMurHash128x64_Result(ctx->h, ctx->carry, ctx->len, h);

	digest[0]  = (unsigned char)((h[0] >> 56) & 0xff);
	digest[1]  = (unsigned char)((h[0] >> 48) & 0xff);
	digest[2]  = (unsigned char)((h[0] >> 40) & 0xff);
	digest[3]  = (unsigned char)((h[0] >> 32) & 0xff);
	digest[4]  = (unsigned char)((h[0] >> 24) & 0xff);
	digest[5]  = (unsigned char)((h[0] >> 16) & 0xff);
	digest[6]  = (unsigned char)((h[0] >> 8) & 0xff);
	digest[7]  = (unsigned char)(h[0] & 0xff);
	digest[8]  = (unsigned char)((h[1] >> 56) & 0xff);
	digest[9]  = (unsigned char)((h[1] >> 48) & 0xff);
	digest[10] = (unsigned char)((h[1] >> 40) & 0xff);
	digest[11] = (unsigned char)((h[1] >> 32) & 0xff);
	digest[12] = (unsigned char)((h[1] >> 24) & 0xff);
	digest[13] = (unsigned char)((h[1] >> 16) & 0xff);
	digest[14] = (unsigned char)((h[1] >> 8) & 0xff);
	digest[15] = (unsigned char)(h[1] & 0xff);
}

PHP_HASH_API int PHP_MURMUR3FCopy(const php_hash_ops *ops, PHP_MURMUR3F_CTX *orig_context, PHP_MURMUR3F_CTX *copy_context)
{
	memcpy(&copy_context->h, &orig_context->h, sizeof orig_context->h);
	memcpy(&copy_context->carry, &orig_context->carry, sizeof orig_context->carry);
	copy_context->len = orig_context->len;
	return SUCCESS;
}
