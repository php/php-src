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
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   |          Charles R. Portwood II <charlesportwoodii@erianna.com>      |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_PASSWORD_H
#define PHP_PASSWORD_H

PHP_MINIT_FUNCTION(password);
PHP_MSHUTDOWN_FUNCTION(password);

#define PHP_PASSWORD_DEFAULT    PHP_PASSWORD_BCRYPT
#define PHP_PASSWORD_BCRYPT_COST 12

#ifdef HAVE_ARGON2LIB
/**
 * When updating these values, synchronize values in
 *     ext/sodium/php_libsodium.h
 *     ext/openssl/php_openssl.h
 * Note that libargon/openssl express memlimit in KB, while libsodium uses bytes.
 */
#define PHP_PASSWORD_ARGON2_MEMORY_COST (64 << 10)
#define PHP_PASSWORD_ARGON2_TIME_COST 4
#define PHP_PASSWORD_ARGON2_THREADS 1
#endif

typedef struct _php_password_algo {
	const char *name;
	zend_string *(*hash)(const zend_string *password, zend_array *options);
	bool (*verify)(const zend_string *password, const zend_string *hash);
	bool (*needs_rehash)(const zend_string *password, zend_array *options);
	int (*get_info)(zval *return_value, const zend_string *hash);
	bool (*valid)(const zend_string *hash);
} php_password_algo;

extern const php_password_algo php_password_algo_bcrypt;
#ifdef HAVE_ARGON2LIB
extern const php_password_algo php_password_algo_argon2i;
extern const php_password_algo php_password_algo_argon2id;
#endif

PHPAPI int php_password_algo_register(const char*, const php_password_algo*);
PHPAPI void php_password_algo_unregister(const char*);
PHPAPI const php_password_algo* php_password_algo_default(void);
PHPAPI zend_string *php_password_algo_extract_ident(const zend_string*);
PHPAPI const php_password_algo* php_password_algo_find(const zend_string*);

PHPAPI const php_password_algo* php_password_algo_identify_ex(const zend_string*, const php_password_algo*);
static inline const php_password_algo* php_password_algo_identify(const zend_string *hash) {
	return php_password_algo_identify_ex(hash, php_password_algo_default());
}


#endif
