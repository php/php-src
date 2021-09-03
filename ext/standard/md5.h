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
   | Author: Alexander Peslyak (Solar Designer) <solar at openwall.com>   |
   |         Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef MD5_H
#define MD5_H

PHPAPI void make_digest(char *md5str, const unsigned char *digest);
PHPAPI void make_digest_ex(char *md5str, const unsigned char *digest, int len);

#include "ext/standard/basic_functions.h"

/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security,
 * Inc. MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Written by Solar Designer <solar at openwall.com> in 2001, and placed
 * in the public domain.  There's absolutely no warranty.
 *
 * See md5.c for more information.
 */

/* MD5 context. */
typedef struct {
	uint32_t lo, hi;
	uint32_t a, b, c, d;
	unsigned char buffer[64];
	uint32_t block[16];
} PHP_MD5_CTX;
#define PHP_MD5_SPEC "llllllb64l16."

#define PHP_MD5Init(ctx) PHP_MD5InitArgs(ctx, NULL)
PHPAPI void PHP_MD5InitArgs(PHP_MD5_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHPAPI void PHP_MD5Update(PHP_MD5_CTX *ctx, const void *data, size_t size);
PHPAPI void PHP_MD5Final(unsigned char *result, PHP_MD5_CTX *ctx);

#endif
