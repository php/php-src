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
  | Author: Martin Jansen <mj@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_JOAAT_H
#define PHP_HASH_JOAAT_H

typedef struct {
	uint32_t state;
} PHP_JOAAT_CTX;
#define PHP_JOAAT_SPEC "l."

PHP_HASH_API void PHP_JOAATInit(PHP_JOAAT_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_JOAATUpdate(PHP_JOAAT_CTX *context, const unsigned char *input, size_t inputLen);
PHP_HASH_API void PHP_JOAATFinal(unsigned char digest[4], PHP_JOAAT_CTX * context);

static uint32_t joaat_buf(void *buf, size_t len, uint32_t hval);

#endif
