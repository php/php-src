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

#ifndef PHP_URI_COMMON_H
#define PHP_URI_COMMON_H

extern zend_class_entry *php_uri_ce_rfc3986_uri;
extern zend_class_entry *php_uri_ce_whatwg_url;
extern zend_class_entry *php_uri_ce_comparison_mode;
extern zend_class_entry *php_uri_ce_exception;
extern zend_class_entry *php_uri_ce_error;
extern zend_class_entry *php_uri_ce_invalid_uri_exception;
extern zend_class_entry *php_uri_ce_whatwg_invalid_url_exception;
extern zend_class_entry *php_uri_ce_whatwg_url_validation_error_type;
extern zend_class_entry *php_uri_ce_whatwg_url_validation_error;

typedef enum php_uri_recomposition_mode {
	PHP_URI_RECOMPOSITION_MODE_RAW_ASCII,
	PHP_URI_RECOMPOSITION_MODE_RAW_UNICODE,
	PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII,
	PHP_URI_RECOMPOSITION_MODE_NORMALIZED_UNICODE,
} php_uri_recomposition_mode;

typedef enum php_uri_component_read_mode {
	PHP_URI_COMPONENT_READ_MODE_RAW,
	PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII,
	PHP_URI_COMPONENT_READ_MODE_NORMALIZED_UNICODE,
} php_uri_component_read_mode;

typedef zend_result (*php_uri_property_handler_read)(void *uri, php_uri_component_read_mode read_mode, zval *retval);

typedef zend_result (*php_uri_property_handler_write)(void *uri, zval *value, zval *errors);

typedef enum php_uri_property_name {
	PHP_URI_PROPERTY_NAME_SCHEME,
	PHP_URI_PROPERTY_NAME_USERNAME,
	PHP_URI_PROPERTY_NAME_PASSWORD,
	PHP_URI_PROPERTY_NAME_HOST,
	PHP_URI_PROPERTY_NAME_PORT,
	PHP_URI_PROPERTY_NAME_PATH,
	PHP_URI_PROPERTY_NAME_QUERY,
	PHP_URI_PROPERTY_NAME_FRAGMENT,
} php_uri_property_name;

typedef struct php_uri_property_handler {
	php_uri_property_handler_read read;
	php_uri_property_handler_write write;
} php_uri_property_handler;

typedef struct php_uri_parser {
	/**
	 * Name (the FQCN) of the URI parser. The "" name is reserved for the handler of the legacy parse_url().
	 */
	const char *name;

	/**
	 * Parses a URI string into a URI.
	 *
	 * If the URI string is valid, a URI is returned. In case of failure, NULL is
	 * returned.
	 *
	 * The errors by-ref parameter can contain errors that occurred during parsing.
	 * If the input value is NULL, or there were no errors, the errors parameter should
	 * not be modified.
	 *
	 * If the URI string is valid and the base_url URI is not NULL, the URI object
	 * is resolved against the base_url.
	 *
	 * If the silent parameter is true, a Uri\InvalidUriException instance must be thrown.
	 * If the parameter is false, the possible errors should be handled by the caller.
	 *
	 * @param uri_str The input string that is going to be parsed
	 * @param uri_str_len Length of the input string
	 * @param base_url The base URI if reference resolution should be performed, otherwise NULL
	 * @param errors An out parameter that stores additional error information
	 * @param silent Whether to throw a Uri\InvalidUriException in case of failure
	 */
	void *(*parse)(const char *uri_str, size_t uri_str_len, const void *base_url, zval *errors, bool silent);

	/**
	 * Clones a URI to a new URI.
	 *
	 * A deep-clone must be performed that copies all pointer members to a new memory address.
	 * @param uri The input URI
	 * @return The cloned URI
	 */
	void *(*clone)(void *uri);

	/**
	 * Recomposes a URI as a string according to the recomposition_mode and exclude_fragment parameters.
	 * The returned zend_string must not be persistent.
	 *
	 * Recomposition_mode can be one of the following:
	 * - PHP_URI_RECOMPOSITION_MODE_RAW_ASCII: Recomposes the raw, non-normalized variant of the URI as a string that must only contain ASCII characters
	 * - PHP_URI_RECOMPOSITION_MODE_RAW_UNICODE: Recomposes the raw, non-normalized variant of the URI as a string that may contain Unicode codepoints
	 * - PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII: Recomposes the normalized variant of the URI as a string that must only contain ASCII characters
	 * - PHP_URI_RECOMPOSITION_MODE_NORMALIZED_UNICODE: Recomposes the normalized variant of the URI as a string that may contain Unicode codepoints
	 *
	 * @param uri The input URI
	 * @param recomposition_mode The type of recomposition
	 * @param exclude_fragment Whether the fragment component should be part of the recomposed URI
	 * @return The recomposed URI as a non-persistent zend_string
	 */
	zend_string *(*to_string)(void *uri, php_uri_recomposition_mode recomposition_mode, bool exclude_fragment);

	/**
	 * Destroy (free) the provided URI.
	 *
	 * @param uri The URI to free. Must do nothing if NULL.
	 */
	void (*destroy)(void *uri);

	struct {
		php_uri_property_handler scheme;
		php_uri_property_handler username;
		php_uri_property_handler password;
		php_uri_property_handler host;
		php_uri_property_handler port;
		php_uri_property_handler path;
		php_uri_property_handler query;
		php_uri_property_handler fragment;
	} property_handler;
} php_uri_parser;

typedef struct php_uri_internal {
	const php_uri_parser *parser;
	void *uri;
} php_uri_internal;

typedef struct php_uri_object {
	const php_uri_parser *parser;
	void *uri;
	zend_object std;
} php_uri_object;

static inline php_uri_object *php_uri_object_from_obj(zend_object *object) {
	return (php_uri_object*)((char*)(object) - XtOffsetOf(php_uri_object, std));
}

#define Z_URI_OBJECT_P(zv) php_uri_object_from_obj(Z_OBJ_P((zv)))

PHPAPI php_uri_object *php_uri_object_create(zend_class_entry *class_type, const php_uri_parser *parser);
PHPAPI void php_uri_object_handler_free(zend_object *object);
PHPAPI zend_object *php_uri_object_handler_clone(zend_object *object);

#define PHP_URI_PARSER_RFC3986 "Uri\\Rfc3986\\Uri"
#define PHP_URI_PARSER_WHATWG "Uri\\WhatWg\\Url"
#define PHP_URI_PARSER_PHP_PARSE_URL "parse_url"
#define PHP_URI_SERIALIZE_URI_FIELD_NAME "uri"

static inline const php_uri_property_handler *php_uri_parser_property_handler_by_name(const php_uri_parser *parser, php_uri_property_name property_name)
{
	switch (property_name) {
		case PHP_URI_PROPERTY_NAME_SCHEME:
			return &parser->property_handler.scheme;
		case PHP_URI_PROPERTY_NAME_USERNAME:
			return &parser->property_handler.username;
		case PHP_URI_PROPERTY_NAME_PASSWORD:
			return &parser->property_handler.password;
		case PHP_URI_PROPERTY_NAME_HOST:
			return &parser->property_handler.host;
		case PHP_URI_PROPERTY_NAME_PORT:
			return &parser->property_handler.port;
		case PHP_URI_PROPERTY_NAME_PATH:
			return &parser->property_handler.path;
		case PHP_URI_PROPERTY_NAME_QUERY:
			return &parser->property_handler.query;
		case PHP_URI_PROPERTY_NAME_FRAGMENT:
			return &parser->property_handler.fragment;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

void php_uri_property_read_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name, php_uri_component_read_mode component_read_mode);
void php_uri_property_write_str_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name);
void php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name);
void php_uri_property_write_long_or_null_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name);

#endif
