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
   | Authors: Stig Bakken <ssb@php.net>                                   |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_CRYPT_H
#define PHP_CRYPT_H

PHPAPI zend_string *php_crypt(const char *password, const int pass_len, const char *salt, int salt_len, bool quiet);
PHP_MINIT_FUNCTION(crypt);
PHP_MSHUTDOWN_FUNCTION(crypt);
PHP_RINIT_FUNCTION(crypt);

/* sha512 crypt has the maximal salt length of 123 characters */
#define PHP_MAX_SALT_LEN 123

#endif
