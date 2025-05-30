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

typedef struct uri_handler_t {
	const char *name;

	zend_result (*init_parser)(void);
	void *(*parse_uri)(const zend_string *uri_str, const void *base_url, zval *errors);
	void (*create_invalid_uri_exception)(zval *exception_zv, zval *errors);
	void *(*clone_uri)(void *uri);
	zend_string *(*uri_to_string)(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment);
	void (*free_uri)(void *uri);
	zend_result (*destroy_parser)(void);
	HashTable *property_handlers;
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

#define URI_PARSER_WHATWG "Uri\\WhatWg\\Url"
#define URI_SERIALIZED_PROPERTY_NAME "uri"

typedef zend_result (*uri_read_t)(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval);

typedef zend_result (*uri_write_t)(uri_internal_t *internal_uri, zval *value, zval *errors);

typedef struct uri_property_handler_t {
	uri_read_t read_func;
	uri_write_t write_func;
} uri_property_handler_t;

#define URI_REGISTER_PROPERTY_READ_HANDLER(property_handlers, name, property_read_func) do { \
	static const uri_property_handler_t handler = {.read_func = property_read_func, .write_func = NULL}; \
	uri_register_property_handler(property_handlers, name, &handler); \
} while (0)

#define URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(property_handlers, name, property_read_func, property_write_func) do { \
	static const uri_property_handler_t handler = {.read_func = property_read_func, .write_func = property_write_func}; \
	uri_register_property_handler(property_handlers, name, &handler); \
} while (0)

void uri_register_property_handler(HashTable *property_handlers, zend_string *name, const uri_property_handler_t *handler);
zend_result uri_handler_register(const uri_handler_t *uri_handler);
uri_property_handler_t *uri_property_handler_from_internal_uri(const uri_internal_t *internal_uri, zend_string *name);
void throw_invalid_uri_exception(const uri_handler_t *uri_handler, zval *errors);
void uri_read_component(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name, uri_component_read_mode_t component_read_mode);
void uri_write_component_str(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name);
void uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name);
void uri_write_component_long_or_null(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name);

#define URI_ASSERT_INITIALIZATION(internal_uri) do { \
	ZEND_ASSERT(internal_uri != NULL && internal_uri->uri != NULL); \
} while (0)

#endif
