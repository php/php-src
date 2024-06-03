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

extern zend_class_entry *uri_interface_ce;
extern zend_class_entry *rfc3986_uri_ce;
extern zend_object_handlers rfc3986_uri_object_handlers;
extern zend_class_entry *whatwg_uri_ce;
extern zend_object_handlers whatwg_uri_object_handlers;
extern zend_class_entry *whatwg_error_ce;

typedef struct uri_handler_t {
	const char *name;

	zend_result (*init_parser)(void);
	void *(*parse_uri)(const zend_string *uri_str, const zend_string *base_url_str, zval *errors);
	zend_class_entry *(*get_uri_ce)(void);
	void *(*clone_uri)(void *uri);
	zend_string *(*uri_to_string)(void *uri);
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

static inline uri_object_t *uri_object_from_obj(zend_object *object) {
	return (uri_object_t*)((char*)(object) - XtOffsetOf(uri_object_t, std));
}

static inline uri_internal_t *uri_internal_from_obj(zend_object *object) {
	return &(uri_object_from_obj(object)->internal);
}

#define Z_URI_OBJECT_P(zv) uri_object_from_obj(Z_OBJ_P((zv)))
#define Z_URI_INTERNAL_P(zv) uri_internal_from_obj(Z_OBJ_P((zv)))

typedef zend_result (*uri_read_t)(const uri_internal_t *internal_uri, zval *retval);

typedef zend_result (*uri_write_t)(uri_internal_t *internal_uri, zval *value);

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

uri_property_handler_t *uri_property_handler_from_uri_handler(const uri_handler_t *uri_handler, zend_string *name);

#define URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, object) do { \
    ZEND_ASSERT(internal_uri != NULL); \
	if (UNEXPECTED(internal_uri->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	RETURN_THROWS(); \
    } \
} while (0)

#define URI_CHECK_INITIALIZATION_RETURN(internal_uri, object, return_on_failure) do { \
    ZEND_ASSERT(internal_uri != NULL); \
	if (UNEXPECTED(internal_uri->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	return return_on_failure; \
    } \
} while (0)

#define URI_CHECK_INITIALIZATION_RETURN_VOID(internal_uri, object) do { \
    ZEND_ASSERT(internal_uri != NULL); \
	if (UNEXPECTED(internal_uri->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	return; \
    } \
} while (0)

#define URI_GETTER(property_name) do { \
    ZEND_PARSE_PARAMETERS_NONE(); \
    uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, Z_OBJ_P(ZEND_THIS)); \
    const uri_property_handler_t *property_handler = uri_property_handler_from_uri_handler(internal_uri->handler, property_name); \
    ZEND_ASSERT(property_handler != NULL); \
    if (property_handler->read_func(internal_uri, return_value) == FAILURE) { \
    	zend_throw_error(NULL, "%s::$%s property cannot be retrieved", ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name)); \
    	RETURN_THROWS(); \
    } \
} while (0)

#define URI_WITHER_COMMON(property_name, property_zv, return_value) \
	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, Z_OBJ_P(ZEND_THIS)); \
    const uri_property_handler_t *property_handler = uri_property_handler_from_uri_handler(internal_uri->handler, property_name); \
    ZEND_ASSERT(property_handler != NULL); \
    zend_object *new_object = uri_clone_obj_handler(Z_OBJ_P(ZEND_THIS)); \
    uri_internal_t *new_internal_uri = uri_internal_from_obj(new_object); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(new_internal_uri, Z_OBJ_P(ZEND_THIS)); \
    if (property_handler->write_func == NULL || property_handler->write_func(new_internal_uri, property_zv) == FAILURE) { \
        zend_readonly_property_modification_error_ex(ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name)); \
    	RETURN_THROWS(); \
    } \
	ZVAL_OBJ(return_value, new_object);

#define URI_WITHER_STR(property_name) do { \
    zend_string *value; \
    ZEND_PARSE_PARAMETERS_START(1, 1) \
    	Z_PARAM_STR_OR_NULL(value) \
    ZEND_PARSE_PARAMETERS_END(); \
	zval zv; \
    if (value == NULL) { \
    	ZVAL_NULL(&zv); \
	} else { \
        ZVAL_STR_COPY(&zv, value); \
    } \
	URI_WITHER_COMMON(property_name, &zv, return_value) \
} while (0)

#define URI_WITHER_LONG(property_name) do { \
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
