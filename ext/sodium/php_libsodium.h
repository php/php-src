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
   | Authors: Frank Denis <jedisct1@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LIBSODIUM_H
#define PHP_LIBSODIUM_H

extern zend_module_entry sodium_module_entry;
#define phpext_sodium_ptr &sodium_module_entry

#define PHP_SODIUM_VERSION PHP_VERSION

#ifdef ZTS
# include "TSRM.h"
#endif

#define SODIUM_LIBRARY_VERSION() (char *) (void *) sodium_version_string()

#define SODIUM_CRYPTO_BOX_KEYPAIRBYTES() crypto_box_SECRETKEYBYTES + crypto_box_PUBLICKEYBYTES

#define SODIUM_CRYPTO_KX_KEYPAIRBYTES() crypto_kx_SECRETKEYBYTES + crypto_kx_PUBLICKEYBYTES

#define SODIUM_CRYPTO_SIGN_KEYPAIRBYTES() crypto_sign_SECRETKEYBYTES + crypto_sign_PUBLICKEYBYTES

PHP_MINIT_FUNCTION(sodium);
PHP_MINIT_FUNCTION(sodium_password_hash);
PHP_MSHUTDOWN_FUNCTION(sodium);
PHP_RINIT_FUNCTION(sodium);
PHP_RSHUTDOWN_FUNCTION(sodium);
PHP_MINFO_FUNCTION(sodium);

#endif	/* PHP_LIBSODIUM_H */
