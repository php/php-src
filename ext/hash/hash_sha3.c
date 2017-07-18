/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sara Golemon <pollita@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_sha3.h"

#define SUCCESS SHA3_SUCCESS /* Avoid conflict between KeccacHash.h and zend_types.h */
#include "KeccakHash.h"


// ==========================================================================

static int hash_sha3_copy(const void *ops, void *orig_context, void *dest_context)
{
	PHP_SHA3_CTX* orig = (PHP_SHA3_CTX*)orig_context;
	PHP_SHA3_CTX* dest = (PHP_SHA3_CTX*)dest_context;
	memcpy(dest->hashinstance, orig->hashinstance, sizeof(Keccak_HashInstance));
	return SUCCESS;
}

#define DECLARE_SHA3_OPS(bits) \
void PHP_SHA3##bits##Init(PHP_SHA3_##bits##_CTX* ctx) { \
	ctx->hashinstance = emalloc(sizeof(Keccak_HashInstance)); \
	Keccak_HashInitialize_SHA3_##bits((Keccak_HashInstance *)ctx->hashinstance); \
} \
void PHP_SHA3##bits##Update(PHP_SHA3_##bits##_CTX* ctx, \
                            const unsigned char* input, \
                            unsigned int inputLen) { \
	Keccak_HashUpdate((Keccak_HashInstance *)ctx->hashinstance, input, inputLen * 8); \
} \
void PHP_SHA3##bits##Final(unsigned char* digest, \
                           PHP_SHA3_##bits##_CTX* ctx) { \
	Keccak_HashFinal((Keccak_HashInstance *)ctx->hashinstance, digest); \
	efree(ctx->hashinstance); \
	ctx->hashinstance = NULL; \
} \
const php_hash_ops php_hash_sha3_##bits##_ops = { \
	(php_hash_init_func_t) PHP_SHA3##bits##Init, \
	(php_hash_update_func_t) PHP_SHA3##bits##Update, \
	(php_hash_final_func_t) PHP_SHA3##bits##Final, \
	hash_sha3_copy, \
	bits >> 3, \
	(1600 - (2 * bits)) >> 3, \
	sizeof(PHP_SHA3_##bits##_CTX), \
	1 \
}

DECLARE_SHA3_OPS(224);
DECLARE_SHA3_OPS(256);
DECLARE_SHA3_OPS(384);
DECLARE_SHA3_OPS(512);

#undef DECLARE_SHA3_OPS

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
