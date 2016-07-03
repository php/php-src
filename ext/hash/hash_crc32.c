/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Michael Wallner <mike@php.net>                              |
  |          Sara Golemon <pollita@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_hash.h"
#include "php_hash_crc32.h"
#include "php_hash_crc32_tables.h"

PHP_HASH_API void PHP_CRC32Init(PHP_CRC32_CTX *context)
{
	context->state = ~0;
}

PHP_HASH_API void PHP_CRC32Update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; ++i) {
		context->state = (context->state << 8) ^ crc32_table[(context->state >> 24) ^ (input[i] & 0xff)];
	}
}

PHP_HASH_API void PHP_CRC32BUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; ++i) {
		context->state = (context->state >> 8) ^ crc32b_table[(context->state ^ input[i]) & 0xff];
	}
}

PHP_HASH_API void PHP_CRC32Final(unsigned char digest[4], PHP_CRC32_CTX *context)
{
	context->state=~context->state;
	digest[3] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[0] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

PHP_HASH_API void PHP_CRC32BFinal(unsigned char digest[4], PHP_CRC32_CTX *context)
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
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32Update,
	(php_hash_final_func_t) PHP_CRC32Final,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX)
};

const php_hash_ops php_hash_crc32b_ops = {
	(php_hash_init_func_t) PHP_CRC32Init,
	(php_hash_update_func_t) PHP_CRC32BUpdate,
	(php_hash_final_func_t) PHP_CRC32BFinal,
	(php_hash_copy_func_t) PHP_CRC32Copy,
	4, /* what to say here? */
	4,
	sizeof(PHP_CRC32_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
