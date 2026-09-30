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

#ifndef PHP_URI_QUERY_H
#define PHP_URI_QUERY_H

#define PHP_URI_QUERY_PARAMS_DEFAULT_MAX_PARAM_COUNT 1000

#define php_uri_query_params_object_from_obj(object) ZEND_CONTAINER_OF(object, php_uri_query_params_object, std)

#define Z_URI_QUERY_PARAMS_OBJECT_P(zv) php_uri_query_params_object_from_obj(Z_OBJ_P((zv)))

typedef enum php_uri_query_params_parsing_method {
	PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC1866,
	PHP_URI_QUERY_PARAMS_PARSING_METHOD_RFC3986,
	PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL,
	PHP_URI_QUERY_PARAMS_PARSING_METHOD_WHATWG_URL_SEARCH_PARAMS,
} php_uri_query_params_parsing_method;

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 3) zend_result php_uri_query_params_parse(
	const char *query_str_first, size_t query_str_length, php_uri_query_params_object *query_params_object,
	php_uri_query_params_parsing_method parsing_method, bool silent, zend_long max_param_count
);

ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_object_query_params_create_object(zend_class_entry *ce);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_free(php_uri_query_params_object *uri_query_params_object);
ZEND_ATTRIBUTE_NONNULL void php_uri_query_params_object_handler_free(zend_object *object);
ZEND_ATTRIBUTE_NONNULL zend_object *php_uri_query_params_object_handler_clone(zend_object *object);
ZEND_ATTRIBUTE_NONNULL zend_object_iterator *php_uri_object_query_params_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

ZEND_ATTRIBUTE_NONNULL zend_string *php_uri_query_params_to_rfc3986_string(const php_uri_query_params_object *object);
ZEND_ATTRIBUTE_NONNULL zend_string *php_uri_query_params_to_whatwg_string(const php_uri_query_params_object *object);

#endif
