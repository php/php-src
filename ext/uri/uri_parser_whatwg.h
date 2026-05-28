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

#ifndef PHP_URI_PARSER_WHATWG_H
#define PHP_URI_PARSER_WHATWG_H

#include "php_uri_common.h"
#include "lexbor/url/url.h"

PHPAPI extern const php_uri_parser php_uri_parser_whatwg;

ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_whatwg_is_special(const lxb_url_t *lexbor_uri);
ZEND_ATTRIBUTE_NONNULL void php_uri_parser_whatwg_host_type_read(const lxb_url_t *lexbor_uri, zval *retval);

lxb_url_t *php_uri_parser_whatwg_parse_ex(const char *uri_str, size_t uri_str_len, const lxb_url_t *lexbor_base_url, zval *errors, bool silent);

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_none(const zend_string *component);
ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_scheme(const zend_string *scheme);
ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_port(zend_long port);

PHP_RINIT_FUNCTION(uri_parser_whatwg);

ZEND_ATTRIBUTE_NONNULL_ARGS(2, 3, 4, 5, 6, 7, 8, 9) lxb_url_t *php_uri_parser_whatwg_build_from_zval(
	lxb_url_t *lexbor_base_url, const zval *scheme, const zval *username, const zval *password,
	const zval *host, const zval *port, const zval *path, const zval *query, const zval *fragment,
	zval *errors_zv
);

ZEND_MODULE_POST_ZEND_DEACTIVATE_D(uri_parser_whatwg);

#endif
