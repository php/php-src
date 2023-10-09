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
#include "php_hash_xxhash.h"

static int php_hash_xxh32_unserialize(
		php_hashcontext_object *hash, zend_long magic, const zval *zv);
static int php_hash_xxh64_unserialize(
		php_hashcontext_object *hash, zend_long magic, const zval *zv);

const php_hash_ops php_hash_xxh32_ops = {
	"xxh32",
	(php_hash_init_func_t) PHP_XXH32Init,
	(php_hash_update_func_t) PHP_XXH32Update,
	(php_hash_final_func_t) PHP_XXH32Final,
	(php_hash_copy_func_t) PHP_XXH32Copy,
	php_hash_serialize,
	php_hash_xxh32_unserialize,
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
	XXH32_canonicalFromHash((XXH32_canonical_t*)digest, XXH32_digest(&ctx->s));
}

PHP_HASH_API int PHP_XXH32Copy(const php_hash_ops *ops, PHP_XXH32_CTX *orig_context, PHP_XXH32_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

static int php_hash_xxh32_unserialize(
		php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	PHP_XXH32_CTX *ctx = (PHP_XXH32_CTX *) hash->context;
	int r = FAILURE;
	if (magic == PHP_HASH_SERIALIZE_MAGIC_SPEC
		&& (r = php_hash_unserialize_spec(hash, zv, PHP_XXH32_SPEC)) == SUCCESS
		&& ctx->s.memsize < 16) {
		return SUCCESS;
	} else {
		return r != SUCCESS ? r : -2000;
	}
}

const php_hash_ops php_hash_xxh64_ops = {
	"xxh64",
	(php_hash_init_func_t) PHP_XXH64Init,
	(php_hash_update_func_t) PHP_XXH64Update,
	(php_hash_final_func_t) PHP_XXH64Final,
	(php_hash_copy_func_t) PHP_XXH64Copy,
	php_hash_serialize,
	php_hash_xxh64_unserialize,
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
	XXH64_canonicalFromHash((XXH64_canonical_t*)digest, XXH64_digest(&ctx->s));
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

typedef XXH_errorcode (*xxh3_reset_with_secret_func_t)(XXH3_state_t*, const void*, size_t);
typedef XXH_errorcode (*xxh3_reset_with_seed_func_t)(XXH3_state_t*, XXH64_hash_t);

zend_always_inline static void _PHP_XXH3_Init(PHP_XXH3_64_CTX *ctx, HashTable *args,
		xxh3_reset_with_seed_func_t func_init_seed, xxh3_reset_with_secret_func_t func_init_secret, const char* algo_name)
{
	memset(&ctx->s, 0, sizeof ctx->s);

	if (args) {
		zval *_seed = zend_hash_str_find_deref(args, "seed", sizeof("seed") - 1);
		zval *_secret = zend_hash_str_find_deref(args, "secret", sizeof("secret") - 1);

		if (_seed && _secret) {
			zend_throw_error(NULL, "%s: Only one of seed or secret is to be passed for initialization", algo_name);
			return;
		}

		if (_seed && IS_LONG == Z_TYPE_P(_seed)) {
			/* This might be a bit too restrictive, but thinking that a seed might be set
				once and for all, it should be done a clean way. */
			func_init_seed(&ctx->s, (XXH64_hash_t)Z_LVAL_P(_seed));
			return;
		} else if (_secret) {
			if (!try_convert_to_string(_secret)) {
				return;
			}
			size_t len = Z_STRLEN_P(_secret);
			if (len < PHP_XXH3_SECRET_SIZE_MIN) {
				zend_throw_error(NULL, "%s: Secret length must be >= %u bytes, %zu bytes passed", algo_name, XXH3_SECRET_SIZE_MIN, len);
				return;
			}
			if (len > sizeof(ctx->secret)) {
				len = sizeof(ctx->secret);
				php_error_docref(NULL, E_WARNING, "%s: Secret content exceeding %zu bytes discarded", algo_name, sizeof(ctx->secret));
			}
			memcpy((unsigned char *)ctx->secret, Z_STRVAL_P(_secret), len);
			func_init_secret(&ctx->s, ctx->secret, len);
			return;
		}
	}

	func_init_seed(&ctx->s, 0);
}

PHP_HASH_API void PHP_XXH3_64_Init(PHP_XXH3_64_CTX *ctx, HashTable *args)
{
	_PHP_XXH3_Init(ctx, args, XXH3_64bits_reset_withSeed, XXH3_64bits_reset_withSecret, "xxh3");
}

PHP_HASH_API void PHP_XXH3_64_Update(PHP_XXH3_64_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH3_64bits_update(&ctx->s, in, len);
}

PHP_HASH_API void PHP_XXH3_64_Final(unsigned char digest[8], PHP_XXH3_64_CTX *ctx)
{
	XXH64_canonicalFromHash((XXH64_canonical_t*)digest, XXH3_64bits_digest(&ctx->s));
}

PHP_HASH_API int PHP_XXH3_64_Copy(const php_hash_ops *ops, PHP_XXH3_64_CTX *orig_context, PHP_XXH3_64_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

static int php_hash_xxh64_unserialize(
		php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	PHP_XXH64_CTX *ctx = (PHP_XXH64_CTX *) hash->context;
	int r = FAILURE;
	if (magic == PHP_HASH_SERIALIZE_MAGIC_SPEC
		&& (r = php_hash_unserialize_spec(hash, zv, PHP_XXH64_SPEC)) == SUCCESS
		&& ctx->s.memsize < 32) {
		return SUCCESS;
	} else {
		return r != SUCCESS ? r : -2000;
	}
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
	_PHP_XXH3_Init(ctx, args, XXH3_128bits_reset_withSeed, XXH3_128bits_reset_withSecret, "xxh128");
}

PHP_HASH_API void PHP_XXH3_128_Update(PHP_XXH3_128_CTX *ctx, const unsigned char *in, size_t len)
{
	XXH3_128bits_update(&ctx->s, in, len);
}

PHP_HASH_API void PHP_XXH3_128_Final(unsigned char digest[16], PHP_XXH3_128_CTX *ctx)
{
	XXH128_canonicalFromHash((XXH128_canonical_t*)digest, XXH3_128bits_digest(&ctx->s));
}

PHP_HASH_API int PHP_XXH3_128_Copy(const php_hash_ops *ops, PHP_XXH3_128_CTX *orig_context, PHP_XXH3_128_CTX *copy_context)
{
	copy_context->s = orig_context->s;
	return SUCCESS;
}

