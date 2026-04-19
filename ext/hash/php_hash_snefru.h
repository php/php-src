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
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_SNEFRU_H
#define PHP_HASH_SNEFRU_H

/* SNEFRU-2.5a with 8 passes and 256 bit hash output
 * AKA "Xerox Secure Hash Function"
 */

/* SNEFRU context */
typedef struct {
	uint32_t state[16];
	uint32_t count[2];
	unsigned char length;
	unsigned char buffer[32];
} PHP_SNEFRU_CTX;
#define PHP_SNEFRU_SPEC "l16l2bb32"

PHP_HASH_API void PHP_SNEFRUInit(PHP_SNEFRU_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_SNEFRUUpdate(PHP_SNEFRU_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SNEFRUFinal(unsigned char[32], PHP_SNEFRU_CTX *);

#endif
