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
  | Author: Don MacAskill <don@smugmug.com>                              |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_crc64.h"
#include "php_hash_crc64_tables.h"
#include "php_hash_crc_common.h"

#ifdef HAVE_CRC_FAST
#include "php_hash_crc_fast.h"
#endif

/* CRC-64/NVME initialization */
PHP_HASH_API void PHP_CRC64NVMEInit(PHP_CRC64_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
#ifdef HAVE_CRC_FAST
	php_crc_init_context64(&context->state, ~0ULL, &context->using_crc_fast, &context->crc_fast_ctx, CRC_FAST_CRC64_NVME);
#else
	php_crc_init_context64(&context->state, ~0ULL);
#endif
}

/* CRC-64/ECMA-182 initialization */
PHP_HASH_API void PHP_CRC64ECMA182Init(PHP_CRC64_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
#ifdef HAVE_CRC_FAST
	php_crc_init_context64(&context->state, 0ULL, &context->using_crc_fast, &context->crc_fast_ctx, CRC_FAST_CRC64_ECMA182);
#else
	php_crc_init_context64(&context->state, 0ULL);
#endif
}

/* Software-only CRC64 NVME update function */
static void php_crc64_nvme_software_update(PHP_CRC64_CTX *context, const unsigned char *input, size_t len)
{
	if (UNEXPECTED(len == 0)) return;
	
	const unsigned char *end = input + len;
	const unsigned char *p = input;
	
	while (p < end) {
		context->state = (context->state >> 8) ^ crc64_nvme_table[(context->state ^ *p) & 0xff];
		p++;
	}
}

/* CRC-64/NVME update function */
PHP_HASH_API void PHP_CRC64NVMEUpdate(PHP_CRC64_CTX *context, const unsigned char *input, size_t len)
{
#ifdef HAVE_CRC_FAST
	if (php_crc_update_context(context->using_crc_fast, &context->crc_fast_ctx, input, len)) {
		return;
	}
#endif
	php_crc64_nvme_software_update(context, input, len);
}

/* Software-only CRC64 ECMA-182 update function */
static void php_crc64_ecma182_software_update(PHP_CRC64_CTX *context, const unsigned char *input, size_t len)
{
	if (UNEXPECTED(len == 0)) return;
	
	const unsigned char *end = input + len;
	const unsigned char *p = input;
	
	while (p < end) {
		context->state = (context->state << 8) ^ crc64_ecma182_table[((context->state >> 56) ^ *p) & 0xff];
		p++;
	}
}

/* CRC-64/ECMA-182 update function */
PHP_HASH_API void PHP_CRC64ECMA182Update(PHP_CRC64_CTX *context, const unsigned char *input, size_t len)
{
#ifdef HAVE_CRC_FAST
	if (php_crc_update_context(context->using_crc_fast, &context->crc_fast_ctx, input, len)) {
		return;
	}
#endif
	php_crc64_ecma182_software_update(context, input, len);
}

/* Software-only CRC64 NVME finalization */
static void php_crc64_nvme_software_final(unsigned char digest[8], PHP_CRC64_CTX *context)
{
	uint64_t final_state = ~context->state;
	
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &final_state, 8);
#else
	final_state = ZEND_BYTES_SWAP64(final_state);
	memcpy(digest, &final_state, 8);
#endif
	context->state = 0;
}

/* CRC-64/NVME finalization */
PHP_HASH_API void PHP_CRC64NVMEFinal(unsigned char digest[8], PHP_CRC64_CTX *context)
{
#ifdef HAVE_CRC_FAST
	if (php_crc_finalize_context(context->using_crc_fast, &context->crc_fast_ctx, digest, 8, &context->state)) {
		return;
	}
#endif
	php_crc64_nvme_software_final(digest, context);
}

/* Software-only CRC64 ECMA-182 finalization */
static void php_crc64_ecma182_software_final(unsigned char digest[8], PHP_CRC64_CTX *context)
{
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &context->state, 8);
#else
	uint64_t final_state = ZEND_BYTES_SWAP64(context->state);
	memcpy(digest, &final_state, 8);
#endif
	context->state = 0;
}

/* CRC-64/ECMA-182 finalization */
PHP_HASH_API void PHP_CRC64ECMA182Final(unsigned char digest[8], PHP_CRC64_CTX *context)
{
#ifdef HAVE_CRC_FAST
	if (php_crc_finalize_context(context->using_crc_fast, &context->crc_fast_ctx, digest, 8, &context->state)) {
		return;
	}
#endif
	php_crc64_ecma182_software_final(digest, context);
}

/* CRC64 context copy function */
PHP_HASH_API zend_result PHP_CRC64Copy(const php_hash_ops *ops, const PHP_CRC64_CTX *orig_context, PHP_CRC64_CTX *copy_context)
{
#ifdef HAVE_CRC_FAST
	php_crc_copy_context64(orig_context->using_crc_fast, &orig_context->crc_fast_ctx, orig_context->state,
	                       &copy_context->using_crc_fast, &copy_context->crc_fast_ctx, &copy_context->state);
#else
	php_crc_copy_context64(orig_context->state, &copy_context->state);
#endif
	return SUCCESS;
}

/* CRC64 serialization function that handles crc-fast contexts */
static hash_spec_result php_crc64_serialize(const php_hashcontext_object *hash, zend_long *magic, zval *zv)
{
	const PHP_CRC64_CTX *ctx = (const PHP_CRC64_CTX *) hash->context;
	zval tmp;
	
	*magic = PHP_HASH_SERIALIZE_MAGIC_SPEC;
	
#ifdef HAVE_CRC_FAST
	uint64_t state_to_serialize = php_crc_get_serialization_state(
		ctx->using_crc_fast, &ctx->crc_fast_ctx, ctx->state);
#else
	uint64_t state_to_serialize = php_crc_get_serialization_state(ctx->state);
#endif
	
	array_init(zv);
	/* Serialize 64-bit value as two 32-bit values for compatibility.
	 * Cast to int32_t for consistent serialization across 32-bit and 64-bit platforms. */
	ZVAL_LONG(&tmp, (int32_t) (state_to_serialize & 0xFFFFFFFF));
	zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
	ZVAL_LONG(&tmp, (int32_t) (state_to_serialize >> 32));
	zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
	
	return HASH_SPEC_SUCCESS;
}

/* CRC64 unserialization function */
static hash_spec_result php_crc64_unserialize(php_hashcontext_object *hash, zend_long magic, const zval *zv)
{
	PHP_CRC64_CTX *ctx = (PHP_CRC64_CTX *) hash->context;
	zval *tmp1, *tmp2;
	
	if (magic != PHP_HASH_SERIALIZE_MAGIC_SPEC || Z_TYPE_P(zv) != IS_ARRAY) {
		return HASH_SPEC_FAILURE;
	}
	
	tmp1 = zend_hash_index_find(Z_ARRVAL_P(zv), 0);
	tmp2 = zend_hash_index_find(Z_ARRVAL_P(zv), 1);
	if (!tmp1 || Z_TYPE_P(tmp1) != IS_LONG || !tmp2 || Z_TYPE_P(tmp2) != IS_LONG) {
		return HASH_SPEC_FAILURE;
	}
	
	/* Reconstruct 64-bit value from two 32-bit values */
	ctx->state = ((uint64_t) Z_LVAL_P(tmp2) << 32) | ((uint64_t) Z_LVAL_P(tmp1) & 0xFFFFFFFF);
#ifdef HAVE_CRC_FAST
	php_crc_reset_context_for_unserialize(&ctx->using_crc_fast, &ctx->crc_fast_ctx);
#else
	php_crc_reset_context_for_unserialize();
#endif
	
	return HASH_SPEC_SUCCESS;
}

/* CRC-64/NVME hash operations structure */
const php_hash_ops php_hash_crc64_nvme_ops = {
	"crc64-nvme",
	(php_hash_init_func_t) PHP_CRC64NVMEInit,
	(php_hash_update_func_t) PHP_CRC64NVMEUpdate,
	(php_hash_final_func_t) PHP_CRC64NVMEFinal,
	(php_hash_copy_func_t) PHP_CRC64Copy,
	php_crc64_serialize,
	php_crc64_unserialize,
	PHP_CRC64_SPEC,
	8,
	8,
	sizeof(PHP_CRC64_CTX),
	0
};

/* CRC-64/ECMA-182 hash operations structure */
const php_hash_ops php_hash_crc64_ecma182_ops = {
	"crc64-ecma-182",
	(php_hash_init_func_t) PHP_CRC64ECMA182Init,
	(php_hash_update_func_t) PHP_CRC64ECMA182Update,
	(php_hash_final_func_t) PHP_CRC64ECMA182Final,
	(php_hash_copy_func_t) PHP_CRC64Copy,
	php_crc64_serialize,
	php_crc64_unserialize,
	PHP_CRC64_SPEC,
	8,
	8,
	sizeof(PHP_CRC64_CTX),
	0
};
