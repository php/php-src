/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PASSWORD_H
#define PHP_PASSWORD_H

PHP_FUNCTION(password_create);
PHP_FUNCTION(password_verify);
PHP_FUNCTION(password_make_salt);

PHP_MINIT_FUNCTION(password);

#define PHP_PASSWORD_DEFAULT	"2y"
#define PHP_PASSWORD_BCRYPT	"2y"
#define PHP_PASSWORD_MD5	"1"
#define PHP_PASSWORD_SHA256	"5"
#define PHP_PASSWORD_SHA512	"6"

#define PHP_PASSWORD_BCRYPT_DEFAULT_COST 14;
#define PHP_PASSWORD_SHA_DEFAULT_ROUNDS 5000;


#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
