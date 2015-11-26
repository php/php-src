/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef URL_H
#define URL_H

typedef struct php_url {
	char *scheme;
	char *user;
	char *pass;
	char *host;
	unsigned short port;
	char *path;
	char *query;
	char *fragment;
} php_url;

PHPAPI void php_url_free(php_url *theurl);
PHPAPI php_url *php_url_parse(char const *str);
PHPAPI php_url *php_url_parse_ex(char const *str, size_t length);
PHPAPI size_t php_url_decode(char *str, size_t len); /* return value: length of decoded string */
PHPAPI size_t php_raw_url_decode(char *str, size_t len); /* return value: length of decoded string */
PHPAPI zend_string *php_url_encode(char const *s, size_t len);
PHPAPI zend_string *php_raw_url_encode(char const *s, size_t len);
PHPAPI char *php_replace_controlchars_ex(char *str, size_t len);

PHP_FUNCTION(parse_url);
PHP_FUNCTION(urlencode);
PHP_FUNCTION(urldecode);
PHP_FUNCTION(rawurlencode);
PHP_FUNCTION(rawurldecode);
PHP_FUNCTION(get_headers);

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
