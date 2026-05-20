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
   | Author: Don MacAskill <don@smugmug.com>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_CRC64_H
#define PHP_HASH_CRC64_H

#include "ext/standard/basic_functions.h"

#ifdef HAVE_CRC_FAST
#include "php_hash_crc_fast.h"
#endif

typedef struct {
#ifdef HAVE_CRC_FAST
	bool using_crc_fast;  /* Move bool to beginning for better alignment */
	php_crc_fast_context crc_fast_ctx;
#endif
	uint64_t state;  /* Keep state at end for cache line optimization */
} PHP_CRC64_CTX;
#define PHP_CRC64_SPEC "q."

PHP_HASH_API void PHP_CRC64NVMEInit(PHP_CRC64_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_CRC64ECMA182Init(PHP_CRC64_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_CRC64NVMEUpdate(PHP_CRC64_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC64ECMA182Update(PHP_CRC64_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC64NVMEFinal(unsigned char digest[8], PHP_CRC64_CTX *context);
PHP_HASH_API void PHP_CRC64ECMA182Final(unsigned char digest[8], PHP_CRC64_CTX *context);
PHP_HASH_API zend_result PHP_CRC64Copy(const php_hash_ops *ops, const PHP_CRC64_CTX *orig_context, PHP_CRC64_CTX *copy_context);

#endif
