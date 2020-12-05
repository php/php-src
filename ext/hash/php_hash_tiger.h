/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_TIGER_H
#define PHP_HASH_TIGER_H

/* TIGER context */
typedef struct {
	uint64_t state[3];
	uint64_t passed;
	unsigned char buffer[64];
	uint32_t length;
	unsigned int passes:1;
} PHP_TIGER_CTX;
#define PHP_TIGER_SPEC "q3qb64l"

PHP_HASH_API void PHP_3TIGERInit(PHP_TIGER_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_4TIGERInit(PHP_TIGER_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_TIGERUpdate(PHP_TIGER_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_TIGER128Final(unsigned char digest[16], PHP_TIGER_CTX *context);
PHP_HASH_API void PHP_TIGER160Final(unsigned char digest[20], PHP_TIGER_CTX *context);
PHP_HASH_API void PHP_TIGER192Final(unsigned char digest[24], PHP_TIGER_CTX *context);

#endif
