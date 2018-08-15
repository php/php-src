/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Martin Jansen <mj@php.net>                                   |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_JOAAT_H
#define PHP_HASH_JOAAT_H

typedef struct {
	uint32_t state;
} PHP_JOAAT_CTX;

PHP_HASH_API void PHP_JOAATInit(PHP_JOAAT_CTX *context);
PHP_HASH_API void PHP_JOAATUpdate(PHP_JOAAT_CTX *context, const unsigned char *input, unsigned int inputLen);
PHP_HASH_API void PHP_JOAATFinal(unsigned char digest[16], PHP_JOAAT_CTX * context);

static uint32_t joaat_buf(void *buf, size_t len, uint32_t hval);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
