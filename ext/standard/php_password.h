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

PHP_MINIT_FUNCTION(password);

#define PHP_PASSWORD_DEFAULT    PHP_PASSWORD_BCRYPT
#define PHP_PASSWORD_BCRYPT_COST 10

#if HAVE_ARGON2LIB
#define PHP_PASSWORD_ARGON2_MEMORY_COST 1<<10
#define PHP_PASSWORD_ARGON2_TIME_COST 2
#define PHP_PASSWORD_ARGON2_THREADS 2
#endif

typedef enum {
    PHP_PASSWORD_UNKNOWN,
    PHP_PASSWORD_BCRYPT,
#if HAVE_ARGON2LIB
    PHP_PASSWORD_ARGON2I,
    PHP_PASSWORD_ARGON2ID,
#endif
} php_password_algo;

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
