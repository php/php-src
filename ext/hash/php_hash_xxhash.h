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

#ifndef PHP_HASH_XXHASH_H
#define PHP_HASH_XXHASH_H

/* Undefined, if xxhash namespace was already set elsewhere. */
#undef XXH_NAMESPACE
#define XXH_NAMESPACE PHP_NAMESPACE
#define XXH_INLINE_ALL 1
#include "xxhash/xxhash.h"

typedef struct {
	XXH32_state_t s;
} PHP_XXH32_CTX;
#define PHP_XXH32_SPEC "llllllllllll"

PHP_HASH_API void PHP_XXH32Init(PHP_XXH32_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_XXH32Update(PHP_XXH32_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_XXH32Final(unsigned char digest[4], PHP_XXH32_CTX *ctx);
PHP_HASH_API int PHP_XXH32Copy(const php_hash_ops *ops, PHP_XXH32_CTX *orig_context, PHP_XXH32_CTX *copy_context);

typedef struct {
	XXH64_state_t s;
} PHP_XXH64_CTX;
#define PHP_XXH64_SPEC "qqqqqqqqqllq"

PHP_HASH_API void PHP_XXH64Init(PHP_XXH64_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_XXH64Update(PHP_XXH64_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_XXH64Final(unsigned char digest[8], PHP_XXH64_CTX *ctx);
PHP_HASH_API int PHP_XXH64Copy(const php_hash_ops *ops, PHP_XXH64_CTX *orig_context, PHP_XXH64_CTX *copy_context);

#define PHP_XXH3_SECRET_SIZE_MIN XXH3_SECRET_SIZE_MIN
#define PHP_XXH3_SECRET_SIZE_MAX 256

typedef struct {
	XXH3_state_t s;
	/* The value must survive the whole streaming cycle from init to final.

	   A more flexible mechanism would be to carry zend_string* passed through
	   the options. However, that will require to introduce a destructor
	   handler for ctx, so then it wolud be automatically called from the
	   object destructor. Until that is given, the viable way is to use a
	   plausible max secret length. */
	const unsigned char secret[PHP_XXH3_SECRET_SIZE_MAX];
} PHP_XXH3_CTX;

typedef PHP_XXH3_CTX PHP_XXH3_64_CTX;

PHP_HASH_API void PHP_XXH3_64_Init(PHP_XXH3_64_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_XXH3_64_Update(PHP_XXH3_64_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_XXH3_64_Final(unsigned char digest[8], PHP_XXH3_64_CTX *ctx);
PHP_HASH_API int PHP_XXH3_64_Copy(const php_hash_ops *ops, PHP_XXH3_64_CTX *orig_context, PHP_XXH3_64_CTX *copy_context);

typedef PHP_XXH3_CTX PHP_XXH3_128_CTX;

PHP_HASH_API void PHP_XXH3_128_Init(PHP_XXH3_128_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_XXH3_128_Update(PHP_XXH3_128_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_XXH3_128_Final(unsigned char digest[16], PHP_XXH3_128_CTX *ctx);
PHP_HASH_API int PHP_XXH3_128_Copy(const php_hash_ops *ops, PHP_XXH3_128_CTX *orig_context, PHP_XXH3_128_CTX *copy_context);

#endif /* PHP_HASH_XXHASH_H */

