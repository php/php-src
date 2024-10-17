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

extern zend_class_entry *uri_ce;
extern zend_class_entry *rfc3986_uri_ce;
extern zend_class_entry *whatwg_uri_ce;
extern zend_object_handlers uri_object_handlers;
extern zend_class_entry *whatwg_error_ce;

typedef struct uri_handler_t {
	const char *name;

	zend_result (*init_parser)(void);
	void *(*parse_uri)(const zend_string *uri_str, const zend_string *base_url_str, zval *errors);
	zend_class_entry *(*get_uri_ce)(void);
	void *(*clone_uri)(void *uri_object_internal);
	zend_string *(*uri_to_string)(void *uri_object_internal);
	void (*free_uri)(void *uri_object_internal);
	zend_result (*destroy_parser)(void);
	HashTable *property_handlers;
} uri_handler_t;

typedef struct uri_object_t {
	const uri_handler_t *handler;
	void *uri;
	zend_object std;
} uri_object_t;

static inline uri_object_t *uri_object_from_obj(zend_object *obj) {
	return (uri_object_t*)((char*)(obj) - XtOffsetOf(uri_object_t, std));
}

#define Z_URI_OBJECT_P(zv) uri_object_from_obj(Z_OBJ_P((zv)))

typedef zend_result (*uri_read_t)(void *uri_object_internal, zval *retval);

typedef zend_result (*uri_write_t)(void *uri_object_internal, zval *value);

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

#endif
