/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sara Golemon <pollita@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_SHA3_H
#define PHP_HASH_SHA3_H

#include "php.h"

PHP_HASH_API void PHP_SHA3224Init(void *);
PHP_HASH_API void PHP_SHA3224Update(void *, const unsigned char*, unsigned int);
PHP_HASH_API void PHP_SAH3224Final(unsigned char[32], void *);

PHP_HASH_API void PHP_SHA3256Init(void *);
PHP_HASH_API void PHP_SHA3256Update(void *, const unsigned char*, unsigned int);
PHP_HASH_API void PHP_SAH3256Final(unsigned char[32], void *);

PHP_HASH_API void PHP_SHA3384Init(void *);
PHP_HASH_API void PHP_SHA3384Update(void *, const unsigned char*, unsigned int);
PHP_HASH_API void PHP_SAH3384Final(unsigned char[32], void *);

PHP_HASH_API void PHP_SHA3512Init(void *);
PHP_HASH_API void PHP_SHA3512Update(void *, const unsigned char*, unsigned int);
PHP_HASH_API void PHP_SAH3512Final(unsigned char[32], void *);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
