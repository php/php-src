/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HASH_WHIRLPOOL_H
#define PHP_HASH_WHIRLPOOL_H

/* WHIRLPOOL context */
typedef struct {
	php_hash_uint64 state[8];
	unsigned char bitlength[32];
	struct {
		int pos;
		int bits;
		unsigned char data[64];
	} buffer;
} PHP_WHIRLPOOL_CTX;

PHP_HASH_API void PHP_WHIRLPOOLInit(PHP_WHIRLPOOL_CTX *);
PHP_HASH_API void PHP_WHIRLPOOLUpdate(PHP_WHIRLPOOL_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_WHIRLPOOLFinal(unsigned char[64], PHP_WHIRLPOOL_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
