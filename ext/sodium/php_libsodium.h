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

#if !defined(HAVE_ARGON2LIB) && (SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6))

/**
 * MEMLIMIT is normalized to KB even though sodium uses Bytes in order to
 * present a consistent user-facing API.
 *
 * Threads are fixed at 1 by libsodium.
 *
 * When updating these values, synchronize ext/standard/php_password.h values.
 */
#if defined(PHP_PASSWORD_ARGON2_MEMORY_COST)
#define PHP_SODIUM_PWHASH_MEMLIMIT PHP_PASSWORD_ARGON2_MEMORY_COST
#else
#define PHP_SODIUM_PWHASH_MEMLIMIT (64 << 10)
#endif
#if defined(PHP_PASSWORD_ARGON2_TIME_COST)
#define PHP_SODIUM_PWHASH_OPSLIMIT PHP_PASSWORD_ARGON2_TIME_COST
#else
#define PHP_SODIUM_PWHASH_OPSLIMIT 4
#endif
#if defined(PHP_SODIUM_PWHASH_THREADS)
#define PHP_SODIUM_PWHASH_THREADS PHP_SODIUM_PWHASH_THREADS
#else
#define PHP_SODIUM_PWHASH_THREADS 1
#endif

#endif

PHP_MINIT_FUNCTION(sodium);
#if !defined(HAVE_ARGON2LIB) && (SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6))
PHP_MINIT_FUNCTION(sodium_password_hash);
#endif
PHP_MSHUTDOWN_FUNCTION(sodium);
PHP_RINIT_FUNCTION(sodium);
PHP_RSHUTDOWN_FUNCTION(sodium);
PHP_MINFO_FUNCTION(sodium);

#endif	/* PHP_LIBSODIUM_H */
