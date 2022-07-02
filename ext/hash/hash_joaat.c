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
  | Author: Martin Jansen <mj@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* Implements Jenkins's one-at-a-time hashing algorithm as presented on
 * http://www.burtleburtle.net/bob/hash/doobs.html.
 */

#include "php_hash.h"
#include "php_hash_joaat.h"

const php_hash_ops php_hash_joaat_ops = {
	"joaat",
	(php_hash_init_func_t) PHP_JOAATInit,
	(php_hash_update_func_t) PHP_JOAATUpdate,
	(php_hash_final_func_t) PHP_JOAATFinal,
	php_hash_copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_JOAAT_SPEC,
	4,
	4,
	sizeof(PHP_JOAAT_CTX),
	0
};

PHP_HASH_API void PHP_JOAATInit(PHP_JOAAT_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	context->state = 0;
}

PHP_HASH_API void PHP_JOAATUpdate(PHP_JOAAT_CTX *context, const unsigned char *input, size_t inputLen)
{
	context->state = joaat_buf((void *)input, inputLen, context->state);
}

PHP_HASH_API void PHP_JOAATFinal(unsigned char digest[4], PHP_JOAAT_CTX * context)
{
	uint32_t hval = context->state;
	hval += (hval << 3);
	hval ^= (hval >> 11);
	hval += (hval << 15);

#ifdef WORDS_BIGENDIAN
	memcpy(digest, &hval, 4);
#else
	int i = 0;
	unsigned char *c = (unsigned char *) &hval;

	for (i = 0; i < 4; i++) {
		digest[i] = c[3 - i];
	}
#endif
	context->state = 0;
}

/*
 * joaat_buf - perform a Jenkins's one-at-a-time hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *
 * returns:
 *  32 bit hash as a static hash type
 */
static uint32_t
joaat_buf(void *buf, size_t len, uint32_t hval)
{
	size_t i;
	unsigned char *input = (unsigned char *)buf;

	for (i = 0; i < len; i++) {
		hval += input[i];
		hval += (hval << 10);
		hval ^= (hval >> 6);
	}

	return hval;
}
