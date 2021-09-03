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
#include "php_hash_crc32.h"
#include "php_hash_crc32_tables.h"
#include "ext/standard/crc32_x86.h"

PHP_HASH_API void PHP_CRC32Init(PHP_CRC32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	context->state = ~0;
}

PHP_HASH_API void PHP_CRC32Update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i = 0;

#if ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE || ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER
	i += crc32_x86_simd_update(X86_CRC32, &context->state, input, len);
#endif

	for (; i < len; ++i) {
		context->state = (context->state << 8) ^ crc32_table[(context->state >> 24) ^ (input[i] & 0xff)];
	}
}

PHP_HASH_API void PHP_CRC32BUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i = 0;

#if ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE || ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER
	i += crc32_x86_simd_update(X86_CRC32B, &context->state, input, len);
#endif

	for (; i < len; ++i) {
		context->state = (context->state >> 8) ^ crc32b_table[(context->state ^ input[i]) & 0xff];
	}
}

PHP_HASH_API void PHP_CRC32CUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i = 0;

#if ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE || ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER
	i += crc32_x86_simd_update(X86_CRC32C, &context->state, input, len);
#endif

	for (; i < len; ++i) {
		context->state = (context->state >> 8) ^ crc32c_table[(context->state ^ input[i]) & 0xff];
	}
}

PHP_HASH_API void PHP_CRC32LEFinal(unsigned char digest[4], PHP_CRC32_CTX *context)
{
	context->state=~context->state;
	digest[3] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[0] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

PHP_HASH_API void PHP_CRC32BEFinal(unsigned char digest[4], PHP_CRC32_CTX *context)
{
	context->state=~context->state;
	digest[0] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[3] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

PHP_HASH_API int PHP_CRC32Copy(const php_hash_ops *ops, PHP_CRC32_CTX *orig_context, PHP_CRC32_CTX *copy_context)
{
	copy_context->state = orig_context->state;
	return SUCCESS;
}

const php_hash_ops php_hash_crc32_ops = {
	"crc32",
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32Update,
	(php_hash_final_func_t) PHP_CRC32LEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32b_ops = {
	"crc32b",
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32BUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};

const php_hash_ops php_hash_crc32c_ops = {
	"crc32c",
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32CUpdate,
	(php_hash_final_func_t) PHP_CRC32BEFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_CRC32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX),
	0
};
