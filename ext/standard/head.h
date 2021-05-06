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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef HEAD_H
#define HEAD_H

#define COOKIE_EXPIRES    "; expires="
#define COOKIE_MAX_AGE    "; Max-Age="
#define COOKIE_DOMAIN     "; domain="
#define COOKIE_PATH       "; path="
#define COOKIE_SECURE     "; secure"
#define COOKIE_HTTPONLY   "; HttpOnly"
#define COOKIE_SAMESITE   "; SameSite="

extern PHP_RINIT_FUNCTION(head);

PHPAPI int php_header(void);
PHPAPI zend_result php_setcookie(zend_string *name, zend_string *value, time_t expires,
	zend_string *path, zend_string *domain, bool secure, bool httponly,
	zend_string *samesite, bool url_encode);

#endif
