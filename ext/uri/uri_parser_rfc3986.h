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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_URI_PARSER_RFC3986_H
#define PHP_URI_PARSER_RFC3986_H

#include "php_uri_common.h"

PHPAPI extern const php_uri_parser php_uri_parser_rfc3986;

typedef struct php_uri_parser_rfc3986_uris php_uri_parser_rfc3986_uris;

zend_result php_uri_parser_rfc3986_userinfo_read(php_uri_parser_rfc3986_uris *uri, php_uri_component_read_mode read_mode, zval *retval);
zend_result php_uri_parser_rfc3986_userinfo_write(php_uri_parser_rfc3986_uris *uri, zval *value, zval *errors);

php_uri_parser_rfc3986_uris *php_uri_parser_rfc3986_parse_ex(const char *uri_str, size_t uri_str_len, const php_uri_parser_rfc3986_uris *uriparser_base_url, bool silent);

#endif
