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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef HEAD_H
#define HEAD_H

#define COOKIE_EXPIRES    "; expires="
#define COOKIE_MAX_AGE    "; Max-Age="
#define COOKIE_DOMAIN     "; domain="
#define COOKIE_PATH       "; path="
#define COOKIE_SECURE     "; secure"
#define COOKIE_HTTPONLY   "; HttpOnly"

extern PHP_RINIT_FUNCTION(head);
PHP_FUNCTION(header);
PHP_FUNCTION(header_remove);
PHP_FUNCTION(setcookie);
PHP_FUNCTION(setrawcookie);
PHP_FUNCTION(headers_sent);
PHP_FUNCTION(headers_list);
PHP_FUNCTION(http_response_code);

PHPAPI int php_header(void);
PHPAPI int php_setcookie(zend_string *name, zend_string *value, time_t expires, zend_string *path, zend_string *domain, int secure, int url_encode, int httponly);

#endif
