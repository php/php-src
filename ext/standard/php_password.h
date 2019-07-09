/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Anthony Ferrara <ircmaxell@php.net>                         |
   |          Charles R. Portwood II <charlesportwoodii@erianna.com>      |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_PASSWORD_H
#define PHP_PASSWORD_H

PHP_FUNCTION(password_hash);
PHP_FUNCTION(password_verify);
PHP_FUNCTION(password_needs_rehash);
PHP_FUNCTION(password_get_info);
PHP_FUNCTION(password_algos);

PHP_MINIT_FUNCTION(password);
PHP_MSHUTDOWN_FUNCTION(password);

#define PHP_PASSWORD_DEFAULT    PHP_PASSWORD_BCRYPT
#define PHP_PASSWORD_BCRYPT_COST 10

#if HAVE_ARGON2LIB
/**
 * When updating these values, synchronize ext/sodium/sodium_pwhash.c values.
 * Note that libargon expresses memlimit in KB, while libsoidum uses bytes.
 */
#define PHP_PASSWORD_ARGON2_MEMORY_COST (64 << 10)
#define PHP_PASSWORD_ARGON2_TIME_COST 4
#define PHP_PASSWORD_ARGON2_THREADS 1
#endif

typedef struct _php_password_algo {
	const char *name;
	zend_string *(*hash)(const zend_string *password, zend_array *options);
	zend_bool (*verify)(const zend_string *password, const zend_string *hash);
	zend_bool (*needs_rehash)(const zend_string *password, zend_array *options);
	int (*get_info)(zval *return_value, const zend_string *hash);
	zend_bool (*valid)(const zend_string *hash);
} php_password_algo;

extern const php_password_algo php_password_algo_bcrypt;
#if HAVE_ARGON2LIB
extern const php_password_algo php_password_algo_argon2i;
extern const php_password_algo php_password_algo_argon2id;
#endif

PHPAPI int php_password_algo_register(const char*, const php_password_algo*);
PHPAPI void php_password_algo_unregister(const char*);
PHPAPI const php_password_algo* php_password_algo_default();
PHPAPI zend_string *php_password_algo_extract_ident(const zend_string*);
PHPAPI const php_password_algo* php_password_algo_find(const zend_string*);

PHPAPI const php_password_algo* php_password_algo_identify_ex(const zend_string*, const php_password_algo*);
static inline const php_password_algo* php_password_algo_identify(const zend_string *hash) {
	return php_password_algo_identify_ex(hash, php_password_algo_default());
}


#endif
