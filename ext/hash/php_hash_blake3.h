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
   | Author: Hans Henrik Bergan <hans@loltek.net>                         |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_BLAKE3_H
#define PHP_HASH_BLAKE3_H

#include "ext/standard/basic_functions.h"
#include "php_hash.h"
#include "blake3/upstream_blake3/c/blake3.h"


// typedef struct blake3_hasher PHP_BLAKE3_CTX;
#define PHP_BLAKE3_CTX blake3_hasher
// When testing, the compiler insert 5 alignment-padding bytes after buf_len, and 7 trailing alignment bytes at the end of the struct.
// Unsure if it is portable to rely on this exact padding behavior..
// If the CIs pass, I'll assume it is portable.
#define PHP_BLAKE3_SPEC /* uint32_t key[8]; */"l8" \
/* uint32_t cv[8]; */ "l8" \
/* uint64_t chunk_counter; */ "q" \
/* uint8_t buf[BLAKE3_BLOCK_LEN];  */ "b64" \
/* uint8_t buf_len; */ "b" \
/* skip 5 bytes of alignment padding in chunk */ "B5" \
/* uint8_t blocks_compressed */ "b" \
/* uint8_t flags; */ "b" \
/* uint8_t cv_stack_len; */ "b" \
/* uint8_t cv_stack[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN]; */ "b1760" \
/* skip 7 trailing alignment bytes */     "B7" \
"."






PHP_HASH_API void PHP_BLAKE3Init(PHP_BLAKE3_CTX *context, HashTable *args);
PHP_HASH_API void PHP_BLAKE3Update(PHP_BLAKE3_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_BLAKE3Final(unsigned char digest[BLAKE3_OUT_LEN/*32*/], PHP_BLAKE3_CTX *context);
PHP_HASH_API int PHP_BLAKE3Copy(const php_hash_ops *ops, PHP_BLAKE3_CTX *orig_context, PHP_BLAKE3_CTX *copy_context);

const php_hash_ops php_hash_blake3_ops = {
	"blake3",
	(php_hash_init_func_t) PHP_BLAKE3Init,
	(php_hash_update_func_t) PHP_BLAKE3Update,
	(php_hash_final_func_t) PHP_BLAKE3Final,
	(php_hash_copy_func_t) PHP_BLAKE3Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_BLAKE3_SPEC, // << don't know what this should be, hopefully a dev that knows can remove this comment
	BLAKE3_OUT_LEN /*32*/,
	BLAKE3_CHUNK_LEN /*1024*/,
	sizeof(PHP_BLAKE3_CTX),
	1
};

#endif