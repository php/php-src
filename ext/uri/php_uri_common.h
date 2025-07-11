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

extern zend_class_entry *uri_rfc3986_uri_ce;
extern zend_object_handlers uri_rfc3986_uri_object_handlers;
extern zend_class_entry *uri_whatwg_url_ce;
extern zend_object_handlers uri_whatwg_uri_object_handlers;
extern zend_class_entry *uri_comparison_mode_ce;
extern zend_class_entry *uri_exception_ce;
extern zend_class_entry *uri_invalid_uri_exception_ce;
extern zend_class_entry *uri_whatwg_invalid_url_exception_ce;
extern zend_class_entry *uri_whatwg_url_validation_error_type_ce;
extern zend_class_entry *uri_whatwg_url_validation_error_ce;
extern zend_object *uri_clone_obj_handler(zend_object *object);

typedef enum {
	URI_RECOMPOSITION_RAW_ASCII,
	URI_RECOMPOSITION_RAW_UNICODE,
	URI_RECOMPOSITION_NORMALIZED_ASCII,
	URI_RECOMPOSITION_NORMALIZED_UNICODE,
} uri_recomposition_mode_t;

typedef enum {
	URI_COMPONENT_READ_RAW,
	URI_COMPONENT_READ_NORMALIZED_ASCII,
	URI_COMPONENT_READ_NORMALIZED_UNICODE,
} uri_component_read_mode_t;

struct uri_internal_t;

typedef zend_result (*uri_read_t)(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval);

typedef zend_result (*uri_write_t)(struct uri_internal_t *internal_uri, zval *value, zval *errors);

typedef enum {
	URI_PROPERTY_NAME_SCHEME,
	URI_PROPERTY_NAME_USERNAME,
	URI_PROPERTY_NAME_PASSWORD,
	URI_PROPERTY_NAME_HOST,
	URI_PROPERTY_NAME_PORT,
	URI_PROPERTY_NAME_PATH,
	URI_PROPERTY_NAME_QUERY,
	URI_PROPERTY_NAME_FRAGMENT,
} uri_property_name_t;

typedef struct uri_property_handler_t {
	uri_read_t read_func;
	uri_write_t write_func;
} uri_property_handler_t;

typedef struct uri_property_handlers_t {
	uri_property_handler_t scheme;
	uri_property_handler_t username;
	uri_property_handler_t password;
	uri_property_handler_t host;
	uri_property_handler_t port;
	uri_property_handler_t path;
	uri_property_handler_t query;
	uri_property_handler_t fragment;
} uri_property_handlers_t;

typedef struct uri_handler_t {
	const char *name;

	/**
	 * Parse a URI string into a URI.
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
	 */
	void *(*parse_uri)(const zend_string *uri_str, const void *base_url, zval *errors, bool silent);
	void *(*clone_uri)(void *uri);
	zend_string *(*uri_to_string)(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment);
	void (*free_uri)(void *uri);

	const uri_property_handlers_t property_handlers;
} uri_handler_t;

typedef struct uri_internal_t {
	const uri_handler_t *handler;
	void *uri;
} uri_internal_t;

typedef struct uri_object_t {
	uri_internal_t internal;
	zend_object std;
} uri_object_t;

static inline uri_object_t *uri_object_from_obj(const zend_object *object) {
	return (uri_object_t*)((char*)(object) - XtOffsetOf(uri_object_t, std));
}

static inline uri_internal_t *uri_internal_from_obj(const zend_object *object) {
	return &(uri_object_from_obj(object)->internal);
}

#define Z_URI_OBJECT_P(zv) uri_object_from_obj(Z_OBJ_P((zv)))
#define Z_URI_INTERNAL_P(zv) uri_internal_from_obj(Z_OBJ_P((zv)))

#define URI_PARSER_RFC3986 "Uri\\Rfc3986\\Uri"
#define URI_PARSER_WHATWG "Uri\\WhatWg\\Url"
#define URI_SERIALIZED_PROPERTY_NAME "uri"

zend_result uri_handler_register(const uri_handler_t *uri_handler);
const uri_property_handler_t *uri_property_handler_from_internal_uri(const uri_internal_t *internal_uri, uri_property_name_t property_name);
void uri_read_component(INTERNAL_FUNCTION_PARAMETERS, uri_property_name_t property_name, uri_component_read_mode_t component_read_mode);
void uri_write_component_str(INTERNAL_FUNCTION_PARAMETERS, uri_property_name_t property_name);
void uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAMETERS, uri_property_name_t property_name);
void uri_write_component_long_or_null(INTERNAL_FUNCTION_PARAMETERS, uri_property_name_t property_name);

#define URI_ASSERT_INITIALIZATION(internal_uri) do { \
	ZEND_ASSERT(internal_uri != NULL && internal_uri->uri != NULL); \
} while (0)

#endif
