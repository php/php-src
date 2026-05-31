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

ZEND_ATTRIBUTE_NONNULL void php_uri_parser_rfc3986_uri_type_read(php_uri_parser_rfc3986_uris *uri, zval *retval);
ZEND_ATTRIBUTE_NONNULL void php_uri_parser_rfc3986_host_type_read(php_uri_parser_rfc3986_uris *uri, zval *retval);

zend_result php_uri_parser_rfc3986_userinfo_read(php_uri_parser_rfc3986_uris *uri, php_uri_component_read_mode read_mode, zval *retval);
zend_result php_uri_parser_rfc3986_userinfo_write(php_uri_parser_rfc3986_uris *uri, const zval *value, zval *errors);

php_uri_parser_rfc3986_uris *php_uri_parser_rfc3986_parse_ex(const char *uri_str, size_t uri_str_len, const php_uri_parser_rfc3986_uris *uriparser_base_url, bool silent);

ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_scheme(const zend_string *scheme);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_userinfo(const zend_string *userinfo);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_host(const zend_string *host);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_port(zend_long port);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_path(const zend_string *path);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_query(const zend_string *query);
ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_rfc3986_validate_fragment(const zend_string *fragment);

ZEND_ATTRIBUTE_NONNULL_ARGS(2,3,4,5,6,7,8) php_uri_parser_rfc3986_uris *php_uri_parser_rfc3986_build_from_zval(
	const php_uri_parser_rfc3986_uris *uriparser_base_uris,
	const zval *scheme, const zval *userinfo, const zval *host, const zval *port,
	const zval *path, const zval *query, const zval *fragment
);

#endif
