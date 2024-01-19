#ifndef PHP_HASH_BLAKE3_H
#define PHP_HASH_BLAKE3_H

#include "ext/standard/basic_functions.h"
#include "php_hash.h"
#include "blake3/upstream_blake3/c/blake3.h"


// typedef struct blake3_hasher PHP_BLAKE3_CTX;
#define PHP_BLAKE3_CTX blake3_hasher
// help: is V correct?
//#define PHP_BLAKE3_SPEC "b8b8qb64bbbbb1760"
#define PHP_BLAKE3_SPEC "L8L8Qa64CCCCL8Ca1760"

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