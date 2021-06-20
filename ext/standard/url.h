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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
 */

#ifndef URL_H
#define URL_H

typedef struct php_url {
	zend_string *scheme;
	zend_string *user;
	zend_string *pass;
	zend_string *host;
	unsigned short port;
	zend_string *path;
	zend_string *query;
	zend_string *fragment;
} php_url;

PHPAPI void php_url_free(php_url *theurl);
PHPAPI php_url *php_url_parse(char const *str);
PHPAPI php_url *php_url_parse_ex(char const *str, size_t length);
PHPAPI php_url *php_url_parse_ex2(char const *str, size_t length, bool *has_port);
PHPAPI size_t php_url_decode(char *str, size_t len); /* return value: length of decoded string */
PHPAPI size_t php_raw_url_decode(char *str, size_t len); /* return value: length of decoded string */
PHPAPI zend_string *php_url_encode(char const *s, size_t len);
PHPAPI zend_string *php_raw_url_encode(char const *s, size_t len);
PHPAPI char *php_replace_controlchars_ex(char *str, size_t len);

#define PHP_URL_SCHEME 0
#define PHP_URL_HOST 1
#define PHP_URL_PORT 2
#define PHP_URL_USER 3
#define PHP_URL_PASS 4
#define PHP_URL_PATH 5
#define PHP_URL_QUERY 6
#define PHP_URL_FRAGMENT 7

#define PHP_QUERY_RFC1738 1
#define PHP_QUERY_RFC3986 2

#endif /* URL_H */
