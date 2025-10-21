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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_URI_PARSER_RFC3986_H
#define PHP_URI_PARSER_RFC3986_H

#include "php_uri_common.h"

PHPAPI extern const php_uri_parser php_uri_parser_rfc3986;

typedef struct php_uri_parser_rfc3986_uris php_uri_parser_rfc3986_uris;

zend_result php_uri_parser_rfc3986_userinfo_read(void *uri, php_uri_component_read_mode read_mode, zval *retval);
zend_result php_uri_parser_rfc3986_userinfo_write(void *uri, zval *value, zval *errors);

php_uri_parser_rfc3986_uris *php_uri_parser_rfc3986_parse_ex(const char *uri_str, size_t uri_str_len, const php_uri_parser_rfc3986_uris *uriparser_base_url, bool silent);

#endif
