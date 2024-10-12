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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "main/php_ini.h"

#include "php_uri.h"
#include "php_uri_common.h"
#include "php_lexbor.h"
#include "php_uriparser.h"
#include "php_uri_arginfo.h"

zend_class_entry *uri_ce;
zend_class_entry *rfc3986_uri_ce;
zend_class_entry *whatwg_uri_ce;
zend_object_handlers uri_object_handlers;
zend_class_entry *whatwg_error_ce;

static zend_array uri_handlers;

static zend_object *uri_clone_obj_handler(zend_object *obj);

ZEND_DECLARE_MODULE_GLOBALS(uri)

static uri_handler_t *uri_handler_by_name(const zend_string *handler_name)
{
	return zend_hash_str_find_ptr(&uri_handlers, ZSTR_VAL(handler_name), ZSTR_LEN(handler_name));
}

static uri_property_handler_t *uri_property_handler_from_uri_handler(const uri_handler_t *uri_handler, zend_string *name)
{
	return zend_hash_find_ptr(uri_handler->property_handlers, name);
}

static uri_property_handler_t *uri_property_handler_from_uri_object(const uri_object_t *uri_object, zend_string *name)
{
	ZEND_ASSERT(uri_object != NULL && uri_object->handler != NULL);

	return uri_property_handler_from_uri_handler(uri_object->handler, name);
}

#define URI_CHECK_INITIALIZATION_RETURN_THROWS(uri_object, object) do { \
    ZEND_ASSERT(uri_object != NULL); \
	if (UNEXPECTED(uri_object->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	RETURN_THROWS(); \
    } \
} while (0)

#define URI_CHECK_INITIALIZATION_RETURN(uri_object, object, return_on_failure) do { \
    ZEND_ASSERT(uri_object != NULL); \
	if (UNEXPECTED(uri_object->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	return return_on_failure; \
    } \
} while (0)

#define URI_CHECK_INITIALIZATION_RETURN_VOID(uri_object, object) do { \
    ZEND_ASSERT(uri_object != NULL); \
	if (UNEXPECTED(uri_object->uri == NULL)) { \
        zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name)); \
    	return; \
    } \
} while (0)

#define URI_GETTER(property_name) do { \
    ZEND_PARSE_PARAMETERS_NONE(); \
    uri_object_t *uri_object = Z_URI_OBJECT_P(ZEND_THIS); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(uri_object, Z_OBJ_P(ZEND_THIS)); \
    const uri_property_handler_t *property_handler = uri_property_handler_from_uri_object(uri_object, property_name); \
    ZEND_ASSERT(property_handler != NULL); \
    if (property_handler->read_func(uri_object->uri, return_value) == FAILURE) { \
    	zend_throw_error(NULL, "%s::$%s property cannot be retrieved", ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name)); \
    	RETURN_THROWS(); \
    } \
} while (0)

#define URI_WITHER_COMMON(property_name, property_zv, return_value) \
	uri_object_t *uri_object = Z_URI_OBJECT_P(ZEND_THIS); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(uri_object, Z_OBJ_P(ZEND_THIS)); \
    const uri_property_handler_t *property_handler = uri_property_handler_from_uri_object(uri_object, property_name); \
    ZEND_ASSERT(property_handler != NULL); \
    zend_object *new_object = uri_clone_obj_handler(Z_OBJ_P(ZEND_THIS)); \
    uri_object_t *new_uri_object = uri_object_from_obj(new_object); \
    URI_CHECK_INITIALIZATION_RETURN_THROWS(new_uri_object, Z_OBJ_P(ZEND_THIS)); \
    if (property_handler->write_func == NULL || property_handler->write_func(new_uri_object->uri, property_zv) == FAILURE) { \
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

PHPAPI uri_handler_t *php_uri_get_handler(const zend_string *handler_name)
{
	uri_handler_t *uri_handler = NULL;

	if (handler_name == NULL) {
		uri_handler = URI_G(default_handler);
	}

	if (uri_handler == NULL) {
		uri_handler = uri_handler_by_name(handler_name ?: URI_G(default_handler_name));
	}

	ZEND_ASSERT(uri_handler);

	return uri_handler;
}

PHPAPI void *php_uri_parse(uri_handler_t *uri_handler, zend_string *uri_str)
{
	if (ZSTR_LEN(uri_str) == 0) {
		return NULL;
	}

	return uri_handler->parse_uri(uri_str, NULL, NULL);
}

static zend_result php_uri_get_property(const uri_handler_t *uri_handler, void *uri, zend_string *name, zval *zv)
{
	uri_property_handler_t *property_handler = uri_property_handler_from_uri_handler(uri_handler, name);
	if (property_handler == NULL) {
		return FAILURE;
	}

	return property_handler->read_func(uri, zv);
}

PHPAPI zend_result php_uri_get_scheme(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_SCHEME), zv);
}

PHPAPI zend_result php_uri_get_user(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_USER), zv);
}

PHPAPI zend_result php_uri_get_password(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_PASSWORD), zv);
}

PHPAPI zend_result php_uri_get_host(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_HOST), zv);
}

PHPAPI zend_result php_uri_get_port(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_PORT), zv);
}

PHPAPI zend_result php_uri_get_path(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_PATH), zv);
}

PHPAPI zend_result php_uri_get_query(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_QUERY), zv);
}

PHPAPI zend_result php_uri_get_fragment(const uri_handler_t *uri_handler, void *uri, zval *zv)
{
	return php_uri_get_property(uri_handler, uri, ZSTR_KNOWN(ZEND_STR_FRAGMENT), zv);
}

PHPAPI void php_uri_free(uri_handler_t *uri_handler, void *uri)
{
	uri_handler->free_uri(uri);
}

ZEND_INI_MH(OnUpdateDefaultHandler)
{
	URI_G(default_handler_name) = zend_string_copy(new_value);
	URI_G(default_handler) = NULL;

	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("uri.default_handler", "parse_url", PHP_INI_ALL, OnUpdateDefaultHandler, default_handler_name, zend_uri_globals, uri_globals)
PHP_INI_END()

PHP_METHOD(Uri_WhatWgError, __construct)
{
	zend_string *position;
	zend_long error;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(position)
		Z_PARAM_LONG(error)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(whatwg_error_ce, Z_OBJ_P(ZEND_THIS), "position", sizeof("position") - 1, position);
	zend_update_property_long(whatwg_error_ce, Z_OBJ_P(ZEND_THIS), "error", sizeof("error") - 1, error);
}

static void instantiate_uri(INTERNAL_FUNCTION_PARAMETERS, const uri_handler_t *handler,
	const zend_string *uri_str, const zend_string *base_url_str, zval *errors, bool is_constructor
) {
	void *uri = handler->parse_uri(uri_str, base_url_str, errors);
	if (UNEXPECTED(uri == NULL)) {
		if (is_constructor) {
			zend_argument_value_error(1, "must be a valid URI");
			RETURN_THROWS();
		} else {
			RETURN_NULL();
		}
	}

	if (!is_constructor) {
		object_init_ex(return_value, handler->get_uri_ce());
	}

	uri_object_t *uri_object = Z_URI_OBJECT_P(is_constructor ? ZEND_THIS : return_value);
	uri_object->handler = handler;
	uri_object->uri = uri;
}

static void create_rfc3986_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str, *base_url_str = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(base_url_str)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(uri_str) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (base_url_str && ZSTR_LEN(base_url_str) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &uriparser_uri_handler, uri_str, base_url_str, NULL, is_constructor);
}

PHP_METHOD(Uri_Uri, fromRfc3986)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

static void create_whatwg_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str, *base_url_str = NULL;
	zval *errors = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(base_url_str)
		Z_PARAM_ZVAL(errors)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(uri_str) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (base_url_str && ZSTR_LEN(base_url_str) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &lexbor_uri_handler, uri_str, base_url_str, errors, is_constructor);
}

PHP_METHOD(Uri_Uri, fromWhatWg)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Uri_Uri, getScheme)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_Uri, withScheme)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_Uri, getUser)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USER));
}

PHP_METHOD(Uri_Uri, withUser)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_USER));
}

PHP_METHOD(Uri_Uri, getPassword)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PASSWORD));
}

PHP_METHOD(Uri_Uri, withPassword)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_PASSWORD));
}

PHP_METHOD(Uri_Uri, getHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST));
}

PHP_METHOD(Uri_Uri, withHost)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_HOST));
}

PHP_METHOD(Uri_Uri, getPort)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PORT));
}

PHP_METHOD(Uri_Uri, withPort)
{
	URI_WITHER_LONG(ZSTR_KNOWN(ZEND_STR_PORT));
}

PHP_METHOD(Uri_Uri, getPath)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PATH));
}

PHP_METHOD(Uri_Uri, withPath)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_PATH));
}

PHP_METHOD(Uri_Uri, getQuery)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_QUERY));
}

PHP_METHOD(Uri_Uri, withQuery)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_QUERY));
}

PHP_METHOD(Uri_Uri, getFragment)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_FRAGMENT));
}

PHP_METHOD(Uri_Uri, withFragment)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_FRAGMENT));
}

PHP_METHOD(Uri_Uri, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_obj = Z_OBJ_P(ZEND_THIS);
	uri_object_t *uri_object = uri_object_from_obj(this_obj);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(uri_object, this_obj);

	RETURN_STR(uri_object->handler->uri_to_string(uri_object->uri));
}

PHP_METHOD(Uri_Rfc3986Uri, __construct)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(Uri_WhatWgUri, __construct)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static zend_object *uri_create_object_handler(zend_class_entry *class_type)
{
	uri_object_t *uri_object = zend_object_alloc(sizeof(uri_object_t), class_type);

	uri_object->uri = NULL;
	uri_object->handler = NULL;

	zend_object_std_init(&uri_object->std, class_type);
	object_properties_init(&uri_object->std, class_type);

	return &uri_object->std;
}

static void uri_free_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	ZEND_ASSERT(uri_object != NULL);
	if (UNEXPECTED(uri_object->uri != NULL)) {
		uri_object->handler->free_uri(uri_object->uri);
		uri_object->handler = NULL;
		uri_object->uri = NULL;
	}

	zend_object_std_dtor(&uri_object->std);
}

static int uri_has_property_handler(zend_object *object, zend_string *name, int check_empty, void **cache_slot)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	ZEND_ASSERT(uri_object != NULL);
	if (UNEXPECTED(uri_object->uri == NULL)) {
		if (check_empty == ZEND_PROPERTY_EXISTS) {
			zend_throw_error(NULL, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name));
		}
		return false;
	}

	uri_property_handler_t *property_handler = zend_hash_find_ptr(uri_object->handler->property_handlers, name);
	if (!property_handler) {
		return zend_std_has_property(object, name, check_empty, cache_slot);
	}

	if (check_empty == ZEND_PROPERTY_EXISTS) {
		return true;
	}

	zval tmp;

	if (property_handler->read_func(uri_object->uri, &tmp) == FAILURE) {
		return false;
	}

	bool retval;

	if (check_empty == ZEND_PROPERTY_NOT_EMPTY) {
		retval = zend_is_true(&tmp);
	} else if (check_empty == ZEND_PROPERTY_ISSET) {
		retval = (Z_TYPE(tmp) != IS_NULL);
	} else {
		ZEND_UNREACHABLE();
	}

	zval_ptr_dtor(&tmp);

	return retval;
}

static zval *uri_read_property_handler(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(uri_object, object, &EG(uninitialized_zval));

	uri_property_handler_t *property_handler = uri_property_handler_from_uri_object(uri_object, name);
	if (UNEXPECTED(property_handler == NULL)) {
		return zend_std_read_property(object, name, type, cache_slot, rv);
	}

	zval *retval;
	zend_result result = property_handler->read_func(uri_object->uri, rv);
	if (result == SUCCESS) {
		retval = rv;
	} else {
		retval = &EG(uninitialized_zval);
	}

	return retval;
}

static zval *uri_write_property_handler(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(uri_object, object, &EG(uninitialized_zval));

	uri_property_handler_t *property_handler = uri_property_handler_from_uri_object(uri_object, name);
	if (UNEXPECTED(property_handler == NULL)) {
		return zend_std_write_property(object, name, value, cache_slot);
	}

	if (UNEXPECTED(property_handler->write_func == NULL)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return &EG(uninitialized_zval);
	}

	zval tmp;
	ZVAL_COPY(&tmp, value);

	// TODO: Check type

	zend_result result = property_handler->write_func(uri_object->uri, &tmp);
	zval_ptr_dtor(&tmp);
	if (result == FAILURE) {
		return &EG(uninitialized_zval);
	}

	return value;
}

static zval *uri_get_property_ptr_ptr_handler(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	const uri_object_t *uri_object = uri_object_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(uri_object, object, NULL);

	if (zend_hash_exists(uri_object->handler->property_handlers, name)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return &EG(error_zval);
	}

	return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
}

static void uri_unset_property_handler(zend_object *object, zend_string *name, void **cache_slot)
{
	const uri_object_t *uri_object = uri_object_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN_VOID(uri_object, object);

	if (zend_hash_exists(uri_object->handler->property_handlers, name)) {
		zend_throw_error(NULL, "Cannot unset readonly property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return;
	}

	zend_std_unset_property(object, name, cache_slot);
}

static zend_object *uri_clone_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);

	zend_object *new_object = uri_create_object_handler(object->ce);
	ZEND_ASSERT(new_object != NULL);
	uri_object_t *new_uri_object = uri_object_from_obj(new_object);

	URI_CHECK_INITIALIZATION_RETURN(uri_object, object, &new_uri_object->std);

	new_uri_object->handler = uri_object->handler;

	void *uri = uri_object->handler->clone_uri(uri_object->uri);
	if (UNEXPECTED(uri == NULL)) {
		zend_throw_error(NULL, "Failed to clone %s", ZSTR_VAL(object->ce->name));
		return &new_uri_object->std;
	}

	new_uri_object->uri = uri;

	zend_objects_clone_members(&new_uri_object->std, &uri_object->std);

	return &new_uri_object->std;
}

static HashTable *uri_get_debug_info_handler(zend_object *obj, int *is_temp)
{
	HashTable *debug_info, *std_properties;

	uri_object_t *uri_object = uri_object_from_obj(obj);
	ZEND_ASSERT(uri_object != NULL);
	if (UNEXPECTED(uri_object->uri == NULL)) {
		return NULL;
	}

	const HashTable *property_handlers = uri_object->handler->property_handlers;
	zend_string *string_key;
	uri_property_handler_t *property_handler;

	*is_temp = 1;

	std_properties = zend_std_get_properties(obj);
	debug_info = zend_array_dup(std_properties);

	zend_string *object_str = ZSTR_INIT_LITERAL("(object value omitted)", false);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(property_handlers, string_key, property_handler) {
		zval value;

		ZEND_ASSERT(string_key != NULL);

		if (property_handler->read_func(uri_object->uri, &value) == FAILURE) {
			continue;
		}

		if (Z_TYPE(value) == IS_OBJECT) {
			zval_ptr_dtor(&value);
			ZVAL_NEW_STR(&value, object_str);
			zend_string_addref(object_str);
		}

		zend_hash_update(debug_info, string_key, &value);
	} ZEND_HASH_FOREACH_END();

	zend_string_release_ex(object_str, false);

	return debug_info;
}

static HashTable *uri_get_gc_handler(zend_object *object, zval **table, int *n)
{
	*table = NULL;
	*n = 0;

	return zend_std_get_properties(object);
}

void uri_register_symbols(void)
{
	uri_ce = register_class_Uri_Uri(zend_ce_stringable);

	uri_ce->create_object = uri_create_object_handler;
	uri_ce->default_object_handlers = &uri_object_handlers;
	memcpy(&uri_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	uri_object_handlers.offset = XtOffsetOf(uri_object_t, std);
	uri_object_handlers.free_obj = uri_free_obj_handler;
	uri_object_handlers.has_property = uri_has_property_handler;
	uri_object_handlers.get_property_ptr_ptr = uri_get_property_ptr_ptr_handler;
	uri_object_handlers.read_property = uri_read_property_handler;
	uri_object_handlers.write_property = uri_write_property_handler;
	uri_object_handlers.unset_property = uri_unset_property_handler;
	uri_object_handlers.clone_obj = uri_clone_obj_handler;
	uri_object_handlers.get_debug_info = uri_get_debug_info_handler;
	uri_object_handlers.get_gc = uri_get_gc_handler;

	rfc3986_uri_ce = register_class_Uri_Rfc3986Uri(uri_ce);
	whatwg_uri_ce = register_class_Uri_WhatWgUri(uri_ce);

	whatwg_error_ce = register_class_Uri_WhatWgError();
}

zend_result uri_handler_register(const uri_handler_t *uri_handler)
{
	zend_string *key = zend_string_init_interned(uri_handler->name, strlen(uri_handler->name), 1);

	ZEND_ASSERT(uri_handler->name != NULL);
	ZEND_ASSERT(uri_handler->init_parser != NULL);
	ZEND_ASSERT(uri_handler->parse_uri != NULL);
	ZEND_ASSERT(uri_handler->get_uri_ce != NULL);
	ZEND_ASSERT(uri_handler->clone_uri != NULL);
	ZEND_ASSERT(uri_handler->uri_to_string != NULL);
	ZEND_ASSERT(uri_handler->free_uri != NULL);
	ZEND_ASSERT(uri_handler->destroy_parser != NULL);
	ZEND_ASSERT(uri_handler->property_handlers != NULL);

	return zend_hash_add_ptr(&uri_handlers, key, (void *) uri_handler) ? SUCCESS : FAILURE;
}

static PHP_MINIT_FUNCTION(uri)
{
	REGISTER_INI_ENTRIES();

	uri_register_symbols();

	zend_hash_init(&uri_handlers, 4, NULL, ZVAL_PTR_DTOR, true);

	if (uri_handler_register(&uriparser_uri_handler) == FAILURE) {
		return FAILURE;
	}

	if (uri_handler_register(&lexbor_uri_handler) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(uri)
{
	DISPLAY_INI_ENTRIES();
}

static PHP_MSHUTDOWN_FUNCTION(uri)
{
	UNREGISTER_INI_ENTRIES();

	zend_hash_destroy(&uri_handlers);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri)
{
#if defined(COMPILE_DL_URI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	uri_handler_t *handler;

	ZEND_HASH_MAP_FOREACH_PTR(&uri_handlers, handler) {
		if (handler->init_parser() == FAILURE) {
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

static PHP_GINIT_FUNCTION(uri)
{
}

static PHP_GSHUTDOWN_FUNCTION(uri)
{
	zend_string_release(uri_globals->default_handler_name);
	uri_globals->default_handler = NULL;
}

PHP_RSHUTDOWN_FUNCTION(uri)
{
	uri_handler_t *handler;

	ZEND_HASH_MAP_FOREACH_PTR(&uri_handlers, handler) {
		if (handler->destroy_parser() == FAILURE) {
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

zend_module_entry uri_module_entry = {
	STANDARD_MODULE_HEADER,
	"uri",                             /* Extension name */
	NULL,                           /* zend_function_entry */
	PHP_MINIT(uri),         /* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(uri),   /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(uri),         /* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(uri),   /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(uri),                  /* PHP_MINFO - Module info */
	PHP_VERSION,                      /* Version */
	PHP_MODULE_GLOBALS(uri),
	PHP_GINIT(uri),
	PHP_GSHUTDOWN(uri),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_URI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uri)
#endif
