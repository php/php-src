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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

#ifndef HEAD_H
#define HEAD_H

#define COOKIE_EXPIRES     "; expires="
#define COOKIE_MAX_AGE     "; Max-Age="
#define COOKIE_DOMAIN      "; domain="
#define COOKIE_PATH        "; path="
#define COOKIE_SECURE      "; secure"
#define COOKIE_HTTPONLY    "; HttpOnly"
#define COOKIE_SAMESITE    "; SameSite="
#define COOKIE_PARTITIONED "; Partitioned"

extern PHP_RINIT_FUNCTION(head);

PHPAPI bool php_header(void);
PHPAPI zend_result php_setcookie(zend_string *name, zend_string *value, time_t expires,
	zend_string *path, zend_string *domain, bool secure, bool httponly,
	zend_string *samesite, bool partitioned, bool url_encode);

#endif
