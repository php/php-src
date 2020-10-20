#include "php_hash.h"
#include "php_hash_blake3.h"
#include "blake3/blake3.h"
#include <string.h> // memcpy

PHP_HASH_API void PHP_BLAKE3Init(PHP_BLAKE3_CTX *context)
{
	blake3_hasher_init(context);
}

PHP_HASH_API void PHP_BLAKE3Update(PHP_BLAKE3_CTX *context, const unsigned char *input, size_t len)
{
	blake3_hasher_update(context, input, len);
}

PHP_HASH_API void PHP_BLAKE3Final(unsigned char digest[BLAKE3_OUT_LEN/*32*/], PHP_BLAKE3_CTX *context)
{
	blake3_hasher_finalize(context, digest, BLAKE3_OUT_LEN);
}

PHP_HASH_API int PHP_BLAKE3Copy(const php_hash_ops *ops, PHP_BLAKE3_CTX *orig_context, PHP_BLAKE3_CTX *copy_context)
{
	memcpy(copy_context, orig_context, sizeof(*orig_context));
	return SUCCESS;
}
