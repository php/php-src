/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Authors: Michael Wallner <mike@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_adler32.h"

PHP_HASH_API void PHP_ADLER32Init(PHP_ADLER32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args)
{
	context->state = 1;
}

PHP_HASH_API void PHP_ADLER32Update(PHP_ADLER32_CTX *context, const unsigned char *input, size_t len)
{
	uint32_t s[2];

	s[0] = context->state & 0xffff;
	s[1] = (context->state >> 16) & 0xffff;
	for (size_t i = 0; i < len; ++i) {
		s[0] += input[i];
		s[1] += s[0];
		if (s[1]>=0x7fffffff)
		{
			s[0] = s[0] % 65521;
			s[1] = s[1] % 65521;
		}
	}
	s[0] = s[0] % 65521;
	s[1] = s[1] % 65521;
	context->state = s[0] + (s[1] << 16);
}

PHP_HASH_API void PHP_ADLER32Final(unsigned char digest[4], PHP_ADLER32_CTX *context)
{
	digest[0] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[3] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

PHP_HASH_API zend_result PHP_ADLER32Copy(const php_hash_ops *ops, const PHP_ADLER32_CTX *orig_context, PHP_ADLER32_CTX *copy_context)
{
	copy_context->state = orig_context->state;
	return SUCCESS;
}

const php_hash_ops php_hash_adler32_ops = {
	"adler32",
	(php_hash_init_func_t) PHP_ADLER32Init,
	(php_hash_update_func_t) PHP_ADLER32Update,
	(php_hash_final_func_t) PHP_ADLER32Final,
	(php_hash_copy_func_t) PHP_ADLER32Copy,
	php_hash_serialize,
	php_hash_unserialize,
	PHP_ADLER32_SPEC,
	4, /* what to say here? */
	4,
	sizeof(PHP_ADLER32_CTX),
	0
};
