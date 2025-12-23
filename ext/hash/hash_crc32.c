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
  |          Don MacAskill <don@smugmug.com>           		             |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_crc32.h"
#include "php_hash_crc32_tables.h"
#include "php_hash_crc_common.h"
#include "ext/standard/crc32_x86.h"

#ifdef HAVE_CRC_FAST
#include "php_hash_crc_fast.h"
#endif

PHP_HASH_API void PHP_CRC32Init(PHP_CRC32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	PHP_CRC_INIT_CONTEXT(context, ~0, CRC_FAST_CRC32_BZIP2);
}

PHP_HASH_API void PHP_CRC32BInit(PHP_CRC32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	PHP_CRC_INIT_CONTEXT(context, ~0, CRC_FAST_CRC32_ISO_HDLC);
}

PHP_HASH_API void PHP_CRC32CInit(PHP_CRC32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	PHP_CRC_INIT_CONTEXT(context, ~0, CRC_FAST_CRC32_ISCSI);
}

/* Software-only CRC32 update function */
static void php_crc32_software_update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	if (UNEXPECTED(len == 0)) return;
	
	size_t i = 0;

#if defined(ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE) || defined(ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER)
	i += crc32_x86_simd_update(X86_CRC32, &context->state, input, len);
#endif

	const unsigned char *end = input + len;
	const unsigned char *p = input + i;
	
	while (p < end) {
		context->state = (context->state << 8) ^ crc32_table[(context->state >> 24) ^ (*p & 0xff)];
		p++;
	}
}

PHP_HASH_API void PHP_CRC32Update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	PHP_CRC_UPDATE_CONTEXT(context, input, len, php_crc32_software_update);
}

/* Software-only CRC32B update function */
static void php_crc32b_software_update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	if (UNEXPECTED(len == 0)) return;
	
	size_t i = 0;

#if defined(ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE) || defined(ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER)
	i += crc32_x86_simd_update(X86_CRC32B, &context->state, input, len);
#endif

	const unsigned char *end = input + len;
	const unsigned char *p = input + i;
	
	while (p < end) {
		context->state = (context->state >> 8) ^ crc32b_table[(context->state ^ *p) & 0xff];
		p++;
	}
}

PHP_HASH_API void PHP_CRC32BUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	PHP_CRC_UPDATE_CONTEXT(context, input, len, php_crc32b_software_update);
}

/* Software-only CRC32C update function */
static void php_crc32c_software_update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	if (UNEXPECTED(len == 0)) return;
	
	size_t i = 0;

#if defined(ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE) || defined(ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER)
	i += crc32_x86_simd_update(X86_CRC32C, &context->state, input, len);
#endif

	const unsigned char *end = input + len;
	const unsigned char *p = input + i;
	
	while (p < end) {
		context->state = (context->state >> 8) ^ crc32c_table[(context->state ^ *p) & 0xff];
		p++;
	}
}

PHP_HASH_API void PHP_CRC32CUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	PHP_CRC_UPDATE_CONTEXT(context, input, len, php_crc32c_software_update);
}

/* Software-only CRC32 little-endian finalization */
static void php_crc32le_software_final(unsigned char digest[4], PHP_CRC32_CTX *context)
{
	uint32_t final_state = ~context->state;
	digest[0] = (unsigned char) (final_state & 0xff);
	digest[1] = (unsigned char) ((final_state >> 8) & 0xff);
	digest[2] = (unsigned char) ((final_state >> 16) & 0xff);
	digest[3] = (unsigned char) ((final_state >> 24) & 0xff);
	context->state = 0;
}

PHP_HASH_API void PHP_CRC32LEFinal(unsigned char digest[4], PHP_CRC32_CTX *context)
{
#ifdef HAVE_CRC_FAST
	if (EXPECTED(context->using_crc_fast)) {
		uint32_t crc32_result = (uint32_t)php_crc_fast_final(&context->crc_fast_ctx);
		
		/* Store in little-endian format for CRC32 */
		digest[0] = (unsigned char) (crc32_result & 0xff);
		digest[1] = (unsigned char) ((crc32_result >> 8) & 0xff);
		digest[2] = (unsigned char) ((crc32_result >> 16) & 0xff);
		digest[3] = (unsigned char) ((crc32_result >> 24) & 0xff);
		
		php_crc_fast_free(&context->crc_fast_ctx);
		context->state = 0;
		return;
	}
#endif
	php_crc32le_software_final(digest, context);
}

/* Software-only CRC32 big-endian finalization */
static void php_crc32be_software_final(unsigned char digest[4], PHP_CRC32_CTX *context)
{
	uint32_t final_state = ~context->state;
	digest[0] = (unsigned char) ((final_state >> 24) & 0xff);
	digest[1] = (unsigned char) ((final_state >> 16) & 0xff);
	digest[2] = (unsigned char) ((final_state >> 8) & 0xff);
	digest[3] = (unsigned char) (final_state & 0xff);
	context->state = 0;
}

PHP_HASH_API void PHP_CRC32BEFinal(unsigned char digest[4], PHP_CRC32_CTX *context)
{
#ifdef HAVE_CRC_FAST
	if (EXPECTED(context->using_crc_fast)) {
		uint32_t crc32_result = (uint32_t)php_crc_fast_final(&context->crc_fast_ctx);
		
		/* Store in big-endian format for CRC32B/CRC32C */
		digest[0] = (unsigned char) ((crc32_result >> 24) & 0xff);
		digest[1] = (unsigned char) ((crc32_result >> 16) & 0xff);
		digest[2] = (unsigned char) ((crc32_result >> 8) & 0xff);
		digest[3] = (unsigned char) (crc32_result & 0xff);
		
		php_crc_fast_free(&context->crc_fast_ctx);
		context->state = 0;
		return;
	}
#endif
	php_crc32be_software_final(digest, context);
}

PHP_HASH_API zend_result PHP_CRC32Copy(const php_hash_ops *ops, const PHP_CRC32_CTX *orig_context, PHP_CRC32_CTX *copy_context)
{
	PHP_CRC_COPY_CONTEXT(orig_context, copy_context, uint32_t);
	return SUCCESS;
}

/* CRC32 serialization function that handles crc-fast contexts */
static hash_spec_result php_crc32_serialize(const php_hashcontext_object *hash, zend_long *magic, zval *zv)
{
	const PHP_CRC32_CTX *ctx = (const PHP_CRC32_CTX *) hash->context;
	zval tmp;
	
	*magic = PHP_HASH_SERIALIZE_MAGIC_SPEC;
	
#ifdef HAVE_CRC_FAST
	uint32_t state_to_serialize = (uint32_t)php_crc_get_serialization_state(
		ctx, ctx->using_crc_fast, &ctx->crc_fast_ctx, ctx->state);
#else
	uint32_t state_to_serialize = ctx->state;
#endif
	
	array_init(zv);
	/* Cast to int32_t for consistent serialization across 32-bit and 64-bit platforms */
	ZVAL_LONG(&tmp, (int32_t) state_to_serialize);
	zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
	
	return HASH_SPEC_SUCCESS;
}

/* CRC32 unserialization function */
static hash_spec_result php_crc32_unserialize(php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	PHP_CRC32_CTX *ctx = (PHP_CRC32_CTX *) hash->context;
	zval *tmp;
	
	if (magic != PHP_HASH_SERIALIZE_MAGIC_SPEC || Z_TYPE_P(zv) != IS_ARRAY) {
		return HASH_SPEC_FAILURE;
	}
	
	tmp = zend_hash_index_find(Z_ARRVAL_P(zv), 0);
	if (!tmp || Z_TYPE_P(tmp) != IS_LONG) {
		return HASH_SPEC_FAILURE;
	}
	
	ctx->state = (uint32_t) Z_LVAL_P(tmp);
	PHP_CRC_RESET_CONTEXT_FOR_UNSERIALIZE(ctx);
	
	return HASH_SPEC_SUCCESS;
}

const php_hash_ops php_hash_crc32_ops = {
	"crc32",
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32Update,
	(php_hash_final_func_t) PHP_CRC32LEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32b_ops = {
	"crc32b",
	(php_hash_init_func_t) PHP_CRC32BInit,
	(php_hash_update_func_t) PHP_CRC32BUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32c_ops = {
	"crc32c",
	(php_hash_init_func_t) PHP_CRC32CInit,
	(php_hash_update_func_t) PHP_CRC32CUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32_iso_hdlc_ops = {
	"crc32-iso-hdlc",
	(php_hash_init_func_t) PHP_CRC32BInit,
	(php_hash_update_func_t) PHP_CRC32BUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4,
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32_iscsi_ops = {
	"crc32-iscsi",
	(php_hash_init_func_t) PHP_CRC32CInit,
	(php_hash_update_func_t) PHP_CRC32CUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4,
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32_php_ops = {
	"crc32-php",
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32Update,
	(php_hash_final_func_t) PHP_CRC32LEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_crc32_serialize,
	php_crc32_unserialize,
	PHP_CRC32_SPEC,
	4,
	4,
	sizeof(PHP_CRC32_CTX),
	0
};
