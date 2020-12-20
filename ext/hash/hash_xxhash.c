/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_xxhash.h"

const php_hash_ops php_hash_xxh32_ops = {
	"xxh32",
	(php_hash_init_func_t) PHP_XXH32Init,
	(php_hash_update_func_t) PHP_XXH32Update,
	(php_hash_final_func_t) PHP_XXH32Final,
	(php_hash_copy_func_t) PHP_XXH32Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_XXH32_SPEC,
	4,
	4,
	sizeof(PHP_XXH32_CTX),
	0
};

PHP_HASH_API void PHP_XXH32Init(PHP_XXH32_CTX *ctx, HashTable *args)
{
	/* XXH32_createState() is not used intentionally. */
	memset(&ctx->s, 0, sizeof ctx->s);

	if (args) {
		zval *seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (seed && IS_LONG == Z_TYPE_P(seed)) {
			XXH32_reset(&ctx->s, (XXH32_hash_t)Z_LVAL_P(seed));
		} else {
			XXH32_reset(&ctx->s, 0);
		}
	} else {
		XXH32_reset(&ctx->s, 0);
	}
}

PHP_HASH_API void PHP_XXH32Update(PHP_XXH32_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH32_update(&ctx->s, in, len);
}
	
PHP_HASH_API void PHP_XXH32Final(unsigned char digest[4], PHP_XXH32_CTX *ctx)
{
	XXH32_hash_t const h = XXH32_digest(&ctx->s);

	digest[0] = (unsigned char)((h >> 24) & 0xff);
	digest[1] = (unsigned char)((h >> 16) & 0xff);
	digest[2] = (unsigned char)((h >> 8) & 0xff);
	digest[3] = (unsigned char)(h & 0xff);
}

PHP_HASH_API int PHP_XXH32Copy(const php_hash_ops *ops, PHP_XXH32_CTX *orig_context, PHP_XXH32_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

const php_hash_ops php_hash_xxh64_ops = {
	"xxh64",
	(php_hash_init_func_t) PHP_XXH64Init,
	(php_hash_update_func_t) PHP_XXH64Update,
	(php_hash_final_func_t) PHP_XXH64Final,
	(php_hash_copy_func_t) PHP_XXH64Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_XXH64_SPEC,
	8,
	8,
	sizeof(PHP_XXH64_CTX),
	0
};

PHP_HASH_API void PHP_XXH64Init(PHP_XXH64_CTX *ctx, HashTable *args)
{
	/* XXH64_createState() is not used intentionally. */
	memset(&ctx->s, 0, sizeof ctx->s);

	if (args) {
		zval *seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (seed && IS_LONG == Z_TYPE_P(seed)) {
			XXH64_reset(&ctx->s, (XXH64_hash_t)Z_LVAL_P(seed));
		} else {
			XXH64_reset(&ctx->s, 0);
		}
	} else {
		XXH64_reset(&ctx->s, 0);
	}
}

PHP_HASH_API void PHP_XXH64Update(PHP_XXH64_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH64_update(&ctx->s, in, len);
}

PHP_HASH_API void PHP_XXH64Final(unsigned char digest[8], PHP_XXH64_CTX *ctx)
{
	XXH64_hash_t const h = XXH64_digest(&ctx->s);

	digest[0] = (unsigned char)((h >> 56) & 0xff);
	digest[1] = (unsigned char)((h >> 48) & 0xff);
	digest[2] = (unsigned char)((h >> 40) & 0xff);
	digest[3] = (unsigned char)((h >> 32) & 0xff);
	digest[4] = (unsigned char)((h >> 24) & 0xff);
	digest[5] = (unsigned char)((h >> 16) & 0xff);
	digest[6] = (unsigned char)((h >> 8) & 0xff);
	digest[7] = (unsigned char)(h & 0xff);
}

PHP_HASH_API int PHP_XXH64Copy(const php_hash_ops *ops, PHP_XXH64_CTX *orig_context, PHP_XXH64_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

const php_hash_ops php_hash_xxh3_64_ops = {
	"xxh3",
	(php_hash_init_func_t) PHP_XXH3_64_Init,
	(php_hash_update_func_t) PHP_XXH3_64_Update,
	(php_hash_final_func_t) PHP_XXH3_64_Final,
	(php_hash_copy_func_t) PHP_XXH3_64_Copy,
	php_hash_serialize,
	php_hash_unserialize,
	NULL,
	8,
	8,
	sizeof(PHP_XXH3_64_CTX),
	0
};

PHP_HASH_API void PHP_XXH3_64_Init(PHP_XXH3_64_CTX *ctx, HashTable *args)
{
	/* TODO integrate also XXH3_64bits_reset_withSecret(). */
	XXH64_hash_t seed = 0;

	if (args) {
		zval *_seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (_seed && IS_LONG == Z_TYPE_P(_seed)) {
			seed = (XXH64_hash_t)Z_LVAL_P(_seed);
		}
	}

	XXH3_64bits_reset_withSeed(&ctx->s, seed);
}

PHP_HASH_API void PHP_XXH3_64_Update(PHP_XXH3_64_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH3_64bits_update(&ctx->s, in, len);
}

PHP_HASH_API void PHP_XXH3_64_Final(unsigned char digest[8], PHP_XXH3_64_CTX *ctx)
{
	XXH64_hash_t const h = XXH3_64bits_digest(&ctx->s);

	digest[0] = (unsigned char)((h >> 56) & 0xff);
	digest[1] = (unsigned char)((h >> 48) & 0xff);
	digest[2] = (unsigned char)((h >> 40) & 0xff);
	digest[3] = (unsigned char)((h >> 32) & 0xff);
	digest[4] = (unsigned char)((h >> 24) & 0xff);
	digest[5] = (unsigned char)((h >> 16) & 0xff);
	digest[6] = (unsigned char)((h >> 8) & 0xff);
	digest[7] = (unsigned char)(h & 0xff);
}

PHP_HASH_API int PHP_XXH3_64_Copy(const php_hash_ops *ops, PHP_XXH3_64_CTX *orig_context, PHP_XXH3_64_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

const php_hash_ops php_hash_xxh3_128_ops = {
	"xxh128",
	(php_hash_init_func_t) PHP_XXH3_128_Init,
	(php_hash_update_func_t) PHP_XXH3_128_Update,
	(php_hash_final_func_t) PHP_XXH3_128_Final,
	(php_hash_copy_func_t) PHP_XXH3_128_Copy,
	php_hash_serialize,
	php_hash_unserialize,
	NULL,
	16,
	8,
	sizeof(PHP_XXH3_128_CTX),
	0
};

PHP_HASH_API void PHP_XXH3_128_Init(PHP_XXH3_128_CTX *ctx, HashTable *args)
{
	/* TODO integrate also XXH3_128__64bits_reset_withSecret(). */
	XXH64_hash_t seed = 0;

	if (args) {
		zval *_seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		/* This might be a bit too restrictive, but thinking that a seed might be set
			once and for all, it should be done a clean way. */
		if (_seed && IS_LONG == Z_TYPE_P(_seed)) {
			seed = (XXH64_hash_t)Z_LVAL_P(_seed);
		}
	}

	XXH3_128bits_reset_withSeed(&ctx->s, seed);
}

PHP_HASH_API void PHP_XXH3_128_Update(PHP_XXH3_128_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH3_128bits_update(&ctx->s, in, len);
}

PHP_HASH_API void PHP_XXH3_128_Final(unsigned char digest[16], PHP_XXH3_128_CTX *ctx)
{
	XXH128_hash_t const h = XXH3_128bits_digest(&ctx->s);

	digest[0]  = (unsigned char)((h.high64 >> 56) & 0xff);
	digest[1]  = (unsigned char)((h.high64 >> 48) & 0xff);
	digest[2]  = (unsigned char)((h.high64 >> 40) & 0xff);
	digest[3]  = (unsigned char)((h.high64 >> 32) & 0xff);
	digest[4]  = (unsigned char)((h.high64 >> 24) & 0xff);
	digest[5]  = (unsigned char)((h.high64 >> 16) & 0xff);
	digest[6]  = (unsigned char)((h.high64 >> 8) & 0xff);
	digest[7]  = (unsigned char)(h.high64 & 0xff);
	digest[8]  = (unsigned char)((h.low64 >> 56) & 0xff);
	digest[9]  = (unsigned char)((h.low64 >> 48) & 0xff);
	digest[10] = (unsigned char)((h.low64 >> 40) & 0xff);
	digest[11] = (unsigned char)((h.low64 >> 32) & 0xff);
	digest[12] = (unsigned char)((h.low64 >> 24) & 0xff);
	digest[13] = (unsigned char)((h.low64 >> 16) & 0xff);
	digest[14] = (unsigned char)((h.low64 >> 8) & 0xff);
	digest[15] = (unsigned char)(h.low64 & 0xff);
}

PHP_HASH_API int PHP_XXH3_128_Copy(const php_hash_ops *ops, PHP_XXH3_128_CTX *orig_context, PHP_XXH3_128_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

