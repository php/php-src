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

extern zend_class_entry *whatwg_url_ce;
extern zend_object_handlers whatwg_uri_object_handlers;
extern zend_class_entry *uri_comparison_mode_ce;
extern zend_class_entry *uri_exception_ce;
extern zend_class_entry *invalid_uri_exception_ce;
extern zend_class_entry *whatwg_invalid_url_exception_ce;
extern zend_class_entry *whatwg_url_validation_error_type_ce;
extern zend_class_entry *whatwg_url_validation_error_ce;

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

#define URI_ASSERT_INITIALIZATION(internal_uri) do { \
	ZEND_ASSERT(internal_uri != NULL && internal_uri->uri != NULL); \
} while (0)

#define URI_GETTER(property_name, component_read_mode) do { \
	ZEND_PARSE_PARAMETERS_NONE(); \
	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS); \
	URI_ASSERT_INITIALIZATION(internal_uri); \
	const uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, property_name); \
	ZEND_ASSERT(property_handler != NULL); \
	if (UNEXPECTED(property_handler->read_func(internal_uri, component_read_mode, return_value) == FAILURE)) { \
		zend_throw_error(NULL, "%s::$%s property cannot be retrieved", ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name)); \
		RETURN_THROWS(); \
	} \
} while (0)

#define URI_WITHER_COMMON(property_name, property_zv, return_value) \
	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS); \
	URI_ASSERT_INITIALIZATION(internal_uri); \
	const uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, property_name); \
	ZEND_ASSERT(property_handler != NULL); \
	zend_object *new_object = uri_clone_obj_handler(Z_OBJ_P(ZEND_THIS)); \
	if (UNEXPECTED(EG(exception) != NULL)) { \
		zend_object_release(new_object); \
		zval_ptr_dtor(property_zv); \
		RETURN_THROWS(); \
	} \
	uri_internal_t *new_internal_uri = uri_internal_from_obj(new_object); \
	URI_ASSERT_INITIALIZATION(new_internal_uri); /* TODO fix memory leak of new_object */ \
	if (property_handler->write_func == NULL) { \
		zend_readonly_property_modification_error_ex(ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name)); \
		zend_object_release(new_object); \
		zval_ptr_dtor(property_zv); \
		RETURN_THROWS(); \
	} \
	zval errors; \
	ZVAL_UNDEF(&errors); \
	if (property_handler->write_func(new_internal_uri, property_zv, &errors) == FAILURE) { \
		throw_invalid_uri_exception(new_internal_uri->handler, &errors); \
		zval_ptr_dtor(&errors); \
		zend_object_release(new_object); \
		zval_ptr_dtor(property_zv); \
		RETURN_THROWS(); \
	} \
	ZEND_ASSERT(Z_TYPE(errors) == IS_UNDEF); \
	ZVAL_OBJ(return_value, new_object); \
	zval_ptr_dtor(property_zv);

#define URI_WITHER_STR(property_name) do { \
	zend_string *value; \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_PATH_STR(value) \
	ZEND_PARSE_PARAMETERS_END(); \
	zval zv; \
	ZVAL_STR_COPY(&zv, value); \
	URI_WITHER_COMMON(property_name, &zv, return_value) \
} while (0)

#define URI_WITHER_STR_OR_NULL(property_name) do { \
	zend_string *value; \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_PATH_STR_OR_NULL(value) \
	ZEND_PARSE_PARAMETERS_END(); \
	zval zv; \
	if (value == NULL) { \
		ZVAL_NULL(&zv); \
	} else { \
		ZVAL_STR_COPY(&zv, value); \
	} \
	URI_WITHER_COMMON(property_name, &zv, return_value) \
} while (0)

#define URI_WITHER_LONG_OR_NULL(property_name) do { \
	zend_long value; \
	bool value_is_null; \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG_OR_NULL(value, value_is_null) \
	ZEND_PARSE_PARAMETERS_END(); \
	zval zv; \
	if (value_is_null) {\
		ZVAL_NULL(&zv); \
	} else { \
		ZVAL_LONG(&zv, value); \
	} \
	URI_WITHER_COMMON(property_name, &zv, return_value); \
} while (0)

#endif
