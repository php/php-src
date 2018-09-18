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
   | Authors: Sammy Kaye Powers <me@sammyk.me>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_RANDOM_H
#define PHP_RANDOM_H

PHP_FUNCTION(random_bytes);
PHP_FUNCTION(random_int);

PHP_MINIT_FUNCTION(random);
PHP_MSHUTDOWN_FUNCTION(random);

typedef struct {
	int fd;
} php_random_globals;

#define php_random_bytes_throw(b, s) php_random_bytes((b), (s), 1)
#define php_random_bytes_silent(b, s) php_random_bytes((b), (s), 0)

#define php_random_int_throw(min, max, result) \
	php_random_int((min), (max), (result), 1)
#define php_random_int_silent(min, max, result) \
	php_random_int((min), (max), (result), 0)

PHPAPI int php_random_bytes(void *bytes, size_t size, zend_bool should_throw);
PHPAPI int php_random_int(zend_long min, zend_long max, zend_long *result, zend_bool should_throw);

#ifdef ZTS
# define RANDOM_G(v) ZEND_TSRMG(random_globals_id, php_random_globals *, v)
extern PHPAPI int random_globals_id;
#else
# define RANDOM_G(v) random_globals.v
extern PHPAPI php_random_globals random_globals;
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
