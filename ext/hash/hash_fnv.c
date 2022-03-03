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
  | Author: Michael Maclean <mgdm@php.net>                               |
  +----------------------------------------------------------------------+
*/

/*  Based on the public domain algorithm found at
	http://www.isthe.com/chongo/tech/comp/fnv/index.html */

#include "php_hash.h"
#include "php_hash_fnv.h"

const php_hash_ops php_hash_fnv132_ops = {
	"fnv132",
	(php_hash_init_func_t) PHP_FNV132Init,
	(php_hash_update_func_t) PHP_FNV132Update,
	(php_hash_final_func_t) PHP_FNV132Final,
	php_hash_copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_FNV132_SPEC,
	4,
	4,
	sizeof(PHP_FNV132_CTX),
	0
};

const php_hash_ops php_hash_fnv1a32_ops = {
	"fnv1a32",
	(php_hash_init_func_t) PHP_FNV132Init,
	(php_hash_update_func_t) PHP_FNV1a32Update,
	(php_hash_final_func_t) PHP_FNV132Final,
	php_hash_copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_FNV132_SPEC,
	4,
	4,
	sizeof(PHP_FNV132_CTX),
	0
};

const php_hash_ops php_hash_fnv164_ops = {
	"fnv164",
	(php_hash_init_func_t) PHP_FNV164Init,
	(php_hash_update_func_t) PHP_FNV164Update,
	(php_hash_final_func_t) PHP_FNV164Final,
	php_hash_copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_FNV164_SPEC,
	8,
	4,
	sizeof(PHP_FNV164_CTX),
	0
};

const php_hash_ops php_hash_fnv1a64_ops = {
	"fnv1a64",
	(php_hash_init_func_t) PHP_FNV164Init,
	(php_hash_update_func_t) PHP_FNV1a64Update,
	(php_hash_final_func_t) PHP_FNV164Final,
	php_hash_copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_FNV164_SPEC,
	8,
	4,
	sizeof(PHP_FNV164_CTX),
	0
};

/* {{{ PHP_FNV132Init
 * 32-bit FNV-1 hash initialisation
 */
PHP_HASH_API void PHP_FNV132Init(PHP_FNV132_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	context->state = PHP_FNV1_32_INIT;
}
/* }}} */

PHP_HASH_API void PHP_FNV132Update(PHP_FNV132_CTX *context, const unsigned char *input,
		size_t inputLen)
{
	context->state = fnv_32_buf((void *)input, inputLen, context->state, 0);
}

PHP_HASH_API void PHP_FNV1a32Update(PHP_FNV132_CTX *context, const unsigned char *input,
		size_t inputLen)
{
	context->state = fnv_32_buf((void *)input, inputLen, context->state, 1);
}

PHP_HASH_API void PHP_FNV132Final(unsigned char digest[4], PHP_FNV132_CTX * context)
{
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &context->state, 4);
#else
	int i = 0;
	unsigned char *c = (unsigned char *) &context->state;

	for (i = 0; i < 4; i++) {
		digest[i] = c[3 - i];
	}
#endif
}

/* {{{ PHP_FNV164Init
 * 64-bit FNV-1 hash initialisation
 */
PHP_HASH_API void PHP_FNV164Init(PHP_FNV164_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	context->state = PHP_FNV1_64_INIT;
}
/* }}} */

PHP_HASH_API void PHP_FNV164Update(PHP_FNV164_CTX *context, const unsigned char *input,
		size_t inputLen)
{
	context->state = fnv_64_buf((void *)input, inputLen, context->state, 0);
}

PHP_HASH_API void PHP_FNV1a64Update(PHP_FNV164_CTX *context, const unsigned char *input,
		size_t inputLen)
{
	context->state = fnv_64_buf((void *)input, inputLen, context->state, 1);
}

PHP_HASH_API void PHP_FNV164Final(unsigned char digest[8], PHP_FNV164_CTX * context)
{
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &context->state, 8);
#else
	int i = 0;
	unsigned char *c = (unsigned char *) &context->state;

	for (i = 0; i < 8; i++) {
		digest[i] = c[7 - i];
	}
#endif
}


/*
 * fnv_32_buf - perform a 32 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *  hval	- previous hash value or 0 if first call
 *  alternate - if > 0 use the alternate version
 *
 * returns:
 *  32 bit hash as a static hash type
 */
static uint32_t
fnv_32_buf(void *buf, size_t len, uint32_t hval, int alternate)
{
	unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
	unsigned char *be = bp + len;	   /* beyond end of buffer */

	/*
	 * FNV-1 hash each octet in the buffer
	 */
	if (alternate == 0) {
		while (bp < be) {
			/* multiply by the 32 bit FNV magic prime mod 2^32 */
			hval *= PHP_FNV_32_PRIME;

			/* xor the bottom with the current octet */
			hval ^= (uint32_t)*bp++;
		}
	} else {
		while (bp < be) {
			/* xor the bottom with the current octet */
			hval ^= (uint32_t)*bp++;

			/* multiply by the 32 bit FNV magic prime mod 2^32 */
			hval *= PHP_FNV_32_PRIME;
		}
	}

	/* return our new hash value */
	return hval;
}

/*
 * fnv_64_buf - perform a 64 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *  hval	- previous hash value or 0 if first call
 *  alternate - if > 0 use the alternate version
 *
 * returns:
 *  64 bit hash as a static hash type
 */
static uint64_t
fnv_64_buf(void *buf, size_t len, uint64_t hval, int alternate)
{
	unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
	unsigned char *be = bp + len;	   /* beyond end of buffer */

	/*
	 * FNV-1 hash each octet of the buffer
	 */

	if (alternate == 0) {
		while (bp < be) {
			/* multiply by the 64 bit FNV magic prime mod 2^64 */
			hval *= PHP_FNV_64_PRIME;

			/* xor the bottom with the current octet */
			hval ^= (uint64_t)*bp++;
		}
	 } else {
		while (bp < be) {
			/* xor the bottom with the current octet */
			hval ^= (uint64_t)*bp++;

			/* multiply by the 64 bit FNV magic prime mod 2^64 */
			hval *= PHP_FNV_64_PRIME;
		 }
	}

	/* return our new hash value */
	return hval;
}
