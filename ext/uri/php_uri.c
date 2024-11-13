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
#include "Zend/zend_exceptions.h"
#include "main/php_ini.h"

#include "php_uri.h"
#include "php_uri_common.h"
#include "php_lexbor.h"
#include "php_uriparser.h"
#include "php_uri_arginfo.h"

zend_class_entry *uri_interface_ce;
zend_class_entry *rfc3986_uri_ce;
zend_object_handlers rfc3986_uri_object_handlers;
zend_class_entry *whatwg_uri_ce;
zend_object_handlers whatwg_uri_object_handlers;
zend_class_entry *uri_exception_ce;
zend_class_entry *uninitialized_uri_exception_ce;
zend_class_entry *uri_operation_exception_ce;
zend_class_entry *invalid_uri_exception_ce;
zend_class_entry *whatwg_error_ce;

static zend_array uri_handlers;

static zend_object *uri_clone_obj_handler(zend_object *object);

static uri_handler_t *uri_handler_by_name(const char *handler_name, size_t handler_name_len)
{
	return zend_hash_str_find_ptr(&uri_handlers, handler_name, handler_name_len);
}

static HashTable *uri_get_properties(zend_object *object, bool is_debug)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	ZEND_ASSERT(internal_uri != NULL);
	if (UNEXPECTED(internal_uri->uri == NULL)) {
		if (!is_debug) {
			zend_throw_error(uninitialized_uri_exception_ce, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name));
		}
		return NULL;
	}

	zend_string *string_key;
	uri_property_handler_t *property_handler;

	HashTable *std_properties = zend_std_get_properties(object);
	HashTable *result = zend_array_dup(std_properties);

	zend_string *object_str = is_debug ? ZSTR_INIT_LITERAL("(object value omitted)", false) : NULL;

	const HashTable *property_handlers = internal_uri->handler->property_handlers;
	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(property_handlers, string_key, property_handler) {
		zval value;

		ZEND_ASSERT(string_key != NULL);

		if (property_handler->read_func(internal_uri, &value) == FAILURE) {
			continue;
		}

		if (is_debug && Z_TYPE(value) == IS_OBJECT) {
			zval_ptr_dtor(&value);
			ZVAL_NEW_STR(&value, object_str);
			zend_string_addref(object_str);
		}

		zend_hash_update(result, string_key, &value);
	} ZEND_HASH_FOREACH_END();

	if (is_debug) {
		zend_string_release_ex(object_str, false);
	}

	return result;
}

PHPAPI uri_handler_t *php_uri_get_handler(const zend_string *uri_handler_name)
{
	if (uri_handler_name == NULL) {
		return uri_handler_by_name("parse_url", sizeof("parse_url") - 1);
	}

	return uri_handler_by_name(ZSTR_VAL(uri_handler_name), ZSTR_LEN(uri_handler_name));
}

PHPAPI uri_internal_t *php_uri_parse(const uri_handler_t *uri_handler, zend_string *uri_str, zval *errors)
{
	ZEND_ASSERT(uri_handler != NULL);

	if (ZSTR_LEN(uri_str) == 0) {
		return NULL;
	}

	uri_internal_t *internal_uri = emalloc(sizeof(uri_internal_t));
	internal_uri->handler = uri_handler;
	internal_uri->uri = uri_handler->parse_uri(uri_str, NULL, errors);

	if (UNEXPECTED(internal_uri->uri == NULL)) {
		efree(internal_uri);
		return NULL;
	}

	return internal_uri;
}

static zend_result php_uri_get_property(const uri_internal_t *internal_uri, zend_string *name, zval *zv)
{
	uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, name);
	if (property_handler == NULL) {
		return FAILURE;
	}

	return property_handler->read_func(internal_uri, zv);
}

PHPAPI zend_result php_uri_get_scheme(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_SCHEME), zv);
}

PHPAPI zend_result php_uri_get_user(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_USER), zv);
}

PHPAPI zend_result php_uri_get_password(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PASSWORD), zv);
}

PHPAPI zend_result php_uri_get_host(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_HOST), zv);
}

PHPAPI zend_result php_uri_get_port(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PORT), zv);
}

PHPAPI zend_result php_uri_get_path(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PATH), zv);
}

PHPAPI zend_result php_uri_get_query(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_QUERY), zv);
}

PHPAPI zend_result php_uri_get_fragment(const uri_internal_t *internal_uri, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_FRAGMENT), zv);
}

PHPAPI void php_uri_free(uri_internal_t *internal_uri)
{
	internal_uri->handler->free_uri(internal_uri->uri);
	internal_uri->uri = NULL;
	internal_uri->handler = NULL;
	efree(internal_uri);
}

PHP_METHOD(Uri_WhatWgError, __construct)
{
	zend_string *uri, *position;
	zend_long error;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(uri)
		Z_PARAM_STR(position)
		Z_PARAM_LONG(error)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(whatwg_error_ce, Z_OBJ_P(ZEND_THIS), "uri", sizeof("uri") - 1, uri);
	zend_update_property_str(whatwg_error_ce, Z_OBJ_P(ZEND_THIS), "position", sizeof("position") - 1, position);
	zend_update_property_long(whatwg_error_ce, Z_OBJ_P(ZEND_THIS), "error", sizeof("error") - 1, error);
}

PHPAPI void php_uri_instantiate_uri(
	INTERNAL_FUNCTION_PARAMETERS, const uri_handler_t *handler, const zend_string *uri_str, const zend_string *base_url_str,
	bool is_constructor, bool return_errors
) {
	zval errors;
	ZVAL_UNDEF(&errors);

	void *uri = handler->parse_uri(uri_str, base_url_str, &errors);
	if (UNEXPECTED(uri == NULL)) {
		if (is_constructor) {
			throw_invalid_uri_exception(&errors);
			zval_ptr_dtor(&errors);
			RETURN_THROWS();
		} else {
			if (return_errors && Z_TYPE(errors) == IS_ARRAY) {
				RETURN_ZVAL(&errors, false, false);
			}

			zval_ptr_dtor(&errors);

			RETURN_NULL();
		}
	}

	ZEND_ASSERT(Z_TYPE(errors) == IS_UNDEF);

	if (!is_constructor) {
		object_init_ex(return_value, handler->get_uri_ce());
	}

	uri_object_t *uri_object = Z_URI_OBJECT_P(is_constructor ? ZEND_THIS : return_value);
	uri_object->internal.handler = handler;
	uri_object->internal.uri = uri;
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

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &uriparser_uri_handler, uri_str, base_url_str, is_constructor, false);
}

PHP_METHOD(Uri_Rfc3986Uri, create)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Uri_Rfc3986Uri, __construct)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static void create_whatwg_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
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

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &lexbor_uri_handler, uri_str, base_url_str, is_constructor, true);
}

PHP_METHOD(Uri_WhatWgUri, create)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Uri_WhatWgUri, __construct)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(Uri_Rfc3986Uri, getScheme)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_Rfc3986Uri, withScheme)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_Rfc3986Uri, getUser)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USER));
}

PHP_METHOD(Uri_Rfc3986Uri, withUser)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_USER));
}

PHP_METHOD(Uri_Rfc3986Uri, getPassword)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PASSWORD));
}

PHP_METHOD(Uri_Rfc3986Uri, withPassword)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_PASSWORD));
}

PHP_METHOD(Uri_Rfc3986Uri, getHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST));
}

PHP_METHOD(Uri_Rfc3986Uri, withHost)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_HOST));
}

PHP_METHOD(Uri_Rfc3986Uri, getPort)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PORT));
}

PHP_METHOD(Uri_Rfc3986Uri, withPort)
{
	URI_WITHER_LONG(ZSTR_KNOWN(ZEND_STR_PORT));
}

PHP_METHOD(Uri_Rfc3986Uri, getPath)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PATH));
}

PHP_METHOD(Uri_Rfc3986Uri, withPath)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_PATH));
}

PHP_METHOD(Uri_Rfc3986Uri, getQuery)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_QUERY));
}

PHP_METHOD(Uri_Rfc3986Uri, withQuery)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_QUERY));
}

PHP_METHOD(Uri_Rfc3986Uri, getFragment)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_FRAGMENT));
}

PHP_METHOD(Uri_Rfc3986Uri, withFragment)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_FRAGMENT));
}

PHP_METHOD(Uri_Rfc3986Uri, equalsTo)
{
	zend_object *that_object;
	bool exclude_fragment = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(that_object, uri_interface_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(exclude_fragment)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *this_internal_uri = uri_internal_from_obj(this_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(this_internal_uri, this_object);

	uri_internal_t *that_internal_uri = uri_internal_from_obj(that_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(that_internal_uri, that_object);

	if (this_object->ce != that_object->ce &&
		!instanceof_function(this_object->ce, that_object->ce) &&
		!instanceof_function(that_object->ce, this_object->ce)
	) {
		RETURN_FALSE;
	}

	zend_string *this_str = this_internal_uri->handler->uri_to_string(this_internal_uri->uri, exclude_fragment);
	zend_string *that_str = that_internal_uri->handler->uri_to_string(that_internal_uri->uri, exclude_fragment);

	RETVAL_BOOL(zend_string_equals(this_str, that_str));

	zend_string_release(this_str);
	zend_string_release(that_str);
}

PHP_METHOD(Uri_Rfc3986Uri, normalize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, this_object);

	zend_object *new_object = uri_clone_obj_handler(this_object);
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_object_release(new_object);
		RETURN_THROWS();
	}

	uri_internal_t *new_internal_uri = uri_internal_from_obj(new_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(new_internal_uri, new_object); /* TODO fix memory leak of new_object */

	if (UNEXPECTED(internal_uri->handler->normalize_uri(new_internal_uri->uri) == FAILURE)) {
		zend_throw_error(uri_operation_exception_ce, "Failed to normalize %s", ZSTR_VAL(this_object->ce->name));
		zend_object_release(new_object);
		RETURN_THROWS();
	}

	ZVAL_OBJ(return_value, new_object);
}

PHP_METHOD(Uri_Rfc3986Uri, toNormalizedString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, object);

	void *new_uri = internal_uri->handler->clone_uri(internal_uri->uri);
	if (UNEXPECTED(new_uri == NULL)) {
		zend_throw_error(uri_operation_exception_ce, "Failed to normalize %s", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	if (UNEXPECTED(internal_uri->handler->normalize_uri(new_uri) == FAILURE)) {
		zend_throw_error(uri_operation_exception_ce, "Failed to normalize %s", ZSTR_VAL(object->ce->name));
		internal_uri->handler->free_uri(new_uri);
		RETURN_THROWS();
	}

	RETVAL_STR(internal_uri->handler->uri_to_string(new_uri, false));
	internal_uri->handler->free_uri(new_uri);
}

PHP_METHOD(Uri_Rfc3986Uri, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, this_object);

	RETURN_STR(internal_uri->handler->uri_to_string(internal_uri->uri, false));
}

PHP_METHOD(Uri_Rfc3986Uri, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_CHECK_INITIALIZATION_RETURN_THROWS(internal_uri, this_object);

	HashTable *ht = uri_get_properties(this_object, false);
	ZVAL_ARR(return_value, ht);
}

static void uri_restore_custom_properties(zend_object *object, uri_internal_t *internal_uri, HashTable *ht)
{
	zend_string *prop_name;
	zval *prop_val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(ht, prop_name, prop_val) {
		if (!prop_name || Z_TYPE_P(prop_val) == IS_REFERENCE || uri_property_handler_from_internal_uri(internal_uri, prop_name)) {
			continue;
		}

		zend_update_property_ex(object->ce, object, prop_name, prop_val);
		if (UNEXPECTED(EG(exception) != NULL)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Uri_Rfc3986Uri, __unserialize)
{
	HashTable *ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(ht)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);

	zend_string *str = zend_string_init("https://example.com", sizeof("https://example.com") - 1, false);

	zval errors;
	ZVAL_UNDEF(&errors);

	internal_uri->handler = uri_handler_by_name(URI_PARSER_RFC3986, sizeof(URI_PARSER_RFC3986) - 1);
	internal_uri->uri = internal_uri->handler->parse_uri(str, NULL, &errors);
	if (internal_uri->uri == NULL) {
		throw_invalid_uri_exception(&errors);
		zval_ptr_dtor(&errors);
		zend_string_release(str);
		RETURN_THROWS();
	}
	ZEND_ASSERT(Z_TYPE(errors) == IS_UNDEF);
	//zend_string_release(str); TODO Fix memory leak

	uri_restore_custom_properties(object, internal_uri, ht);
}

PHP_METHOD(Uri_WhatWgUri, __unserialize)
{
	HashTable *ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(ht)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);

	zend_string *str = zend_string_init("https://example.com", sizeof("https://example.com") - 1, false);

	zval errors;
	ZVAL_UNDEF(&errors);

	internal_uri->handler = uri_handler_by_name(URI_PARSER_WHATWG, sizeof(URI_PARSER_WHATWG) - 1);
	internal_uri->uri = internal_uri->handler->parse_uri(str, NULL, &errors);
	if (internal_uri->uri == NULL) {
		throw_invalid_uri_exception(&errors);
		zval_ptr_dtor(&errors);
		zend_string_release(str);
		RETURN_THROWS();
	}
	ZEND_ASSERT(Z_TYPE(errors) == IS_UNDEF);
	//zend_string_release(str); TODO Fix memory leak

	uri_restore_custom_properties(object, internal_uri, ht);
}

static zend_object *uri_create_object_handler(zend_class_entry *class_type)
{
	uri_object_t *uri_object = zend_object_alloc(sizeof(uri_object_t), class_type);

	uri_object->internal.uri = NULL;
	uri_object->internal.handler = NULL;

	zend_object_std_init(&uri_object->std, class_type);
	object_properties_init(&uri_object->std, class_type);

	return &uri_object->std;
}

static void uri_free_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	ZEND_ASSERT(uri_object != NULL);

	if (UNEXPECTED(uri_object->internal.uri != NULL)) {
		uri_object->internal.handler->free_uri(uri_object->internal.uri);
		uri_object->internal.handler = NULL;
		uri_object->internal.uri = NULL;
	}

	zend_object_std_dtor(&uri_object->std);
}

static int uri_has_property_handler(zend_object *object, zend_string *name, int check_empty, void **cache_slot)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	ZEND_ASSERT(internal_uri != NULL);
	if (UNEXPECTED(internal_uri->uri == NULL)) {
		if (check_empty == ZEND_PROPERTY_EXISTS) {
			zend_throw_error(uninitialized_uri_exception_ce, "%s object is not correctly initialized", ZSTR_VAL(object->ce->name));
		}
		return false;
	}

	uri_property_handler_t *property_handler = zend_hash_find_ptr(internal_uri->handler->property_handlers, name);
	if (UNEXPECTED(property_handler == NULL)) {
		return zend_std_has_property(object, name, check_empty, cache_slot);
	}

	if (check_empty == ZEND_PROPERTY_EXISTS) {
		return true;
	}

	zval tmp;

	if (property_handler->read_func(internal_uri, &tmp) == FAILURE) {
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
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(internal_uri, object, &EG(uninitialized_zval));

	uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, name);
	if (UNEXPECTED(property_handler == NULL)) {
		return zend_std_read_property(object, name, type, cache_slot, rv);
	}

	if (type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return &EG(uninitialized_zval);
	}

	zval *retval;
	zend_result result = property_handler->read_func(internal_uri, rv);
	if (result == SUCCESS) {
		retval = rv;
	} else {
		retval = &EG(uninitialized_zval);
	}

	return retval;
}

static zval *uri_write_property_handler(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(internal_uri, object, &EG(error_zval));

	uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, name);
	if (UNEXPECTED(property_handler == NULL)) {
		return zend_std_write_property(object, name, value, cache_slot);
	}

	zend_readonly_property_modification_error_ex(ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
	return &EG(error_zval);
}

static zval *uri_get_property_ptr_ptr_handler(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	const uri_internal_t *internal_uri = uri_internal_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN(internal_uri, object, NULL);

	if (UNEXPECTED(uri_property_handler_from_internal_uri(internal_uri, name) == NULL)) {
		return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
	}

	zend_readonly_property_modification_error_ex(ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
	return &EG(error_zval);
}

static void uri_unset_property_handler(zend_object *object, zend_string *name, void **cache_slot)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	URI_CHECK_INITIALIZATION_RETURN_VOID(internal_uri, object);

	if (UNEXPECTED(uri_property_handler_from_internal_uri(internal_uri, name) == NULL)) {
		zend_std_unset_property(object, name, cache_slot);
	}

	zend_throw_error(NULL, "Cannot unset readonly property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
}

static zend_object *uri_clone_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);

	zend_object *new_object = uri_create_object_handler(object->ce);
	ZEND_ASSERT(new_object != NULL);
	uri_object_t *new_uri_object = uri_object_from_obj(new_object);

	URI_CHECK_INITIALIZATION_RETURN(internal_uri, object, &new_uri_object->std);

	new_uri_object->internal.handler = internal_uri->handler;

	void *uri = internal_uri->handler->clone_uri(internal_uri->uri);
	if (UNEXPECTED(uri == NULL)) {
		zend_throw_error(uri_operation_exception_ce, "Failed to clone %s", ZSTR_VAL(object->ce->name));
		return &new_uri_object->std;
	}

	new_uri_object->internal.uri = uri;

	zend_objects_clone_members(&new_uri_object->std, &uri_object->std);

	return &new_uri_object->std;
}

static HashTable *uri_get_debug_info_handler(zend_object *object, int *is_temp)
{
	*is_temp = 1;

	return uri_get_properties(object, true);
}

static HashTable *uri_get_gc_handler(zend_object *object, zval **table, int *n)
{
	*table = NULL;
	*n = 0;

	return zend_std_get_properties(object);
}

static HashTable *uri_get_properties_for_handler(zend_object *object, zend_prop_purpose purpose)
{
	switch (purpose) {
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
			return uri_get_properties(object, false);
		default:
			return zend_std_get_properties_for(object, purpose);
	}
}

PHPAPI void php_uri_implementation_set_object_handlers(zend_class_entry *ce, zend_object_handlers *object_handlers)
{
	ce->create_object = uri_create_object_handler;
	ce->default_object_handlers = object_handlers;
	memcpy(object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	object_handlers->offset = XtOffsetOf(uri_object_t, std);
	object_handlers->free_obj = uri_free_obj_handler;
	object_handlers->has_property = uri_has_property_handler;
	object_handlers->get_property_ptr_ptr = uri_get_property_ptr_ptr_handler;
	object_handlers->read_property = uri_read_property_handler;
	object_handlers->write_property = uri_write_property_handler;
	object_handlers->unset_property = uri_unset_property_handler;
	object_handlers->clone_obj = uri_clone_obj_handler;
	object_handlers->get_debug_info = uri_get_debug_info_handler;
	object_handlers->get_gc = uri_get_gc_handler;
	object_handlers->get_properties_for = uri_get_properties_for_handler;
}

zend_result uri_handler_register(const uri_handler_t *uri_handler)
{
	zend_string *key = zend_string_init_interned(uri_handler->name, strlen(uri_handler->name), 1);

	ZEND_ASSERT(uri_handler->name != NULL);
	ZEND_ASSERT(uri_handler->init_parser != NULL);
	ZEND_ASSERT(uri_handler->parse_uri != NULL);
	ZEND_ASSERT(uri_handler->get_uri_ce != NULL);
	ZEND_ASSERT(uri_handler->clone_uri != NULL);
	ZEND_ASSERT(uri_handler->normalize_uri != NULL);
	ZEND_ASSERT(uri_handler->uri_to_string != NULL);
	ZEND_ASSERT(uri_handler->free_uri != NULL);
	ZEND_ASSERT(uri_handler->destroy_parser != NULL);
	ZEND_ASSERT(uri_handler->property_handlers != NULL);

	return zend_hash_add_ptr(&uri_handlers, key, (void *) uri_handler) ? SUCCESS : FAILURE;
}

static PHP_MINIT_FUNCTION(uri)
{
	register_php_uri_symbols(module_number);

	uri_interface_ce = register_class_Uri_UriInterface(zend_ce_stringable);

	rfc3986_uri_ce = register_class_Uri_Rfc3986Uri(uri_interface_ce);
	php_uri_implementation_set_object_handlers(rfc3986_uri_ce, &rfc3986_uri_object_handlers);

	whatwg_uri_ce = register_class_Uri_WhatWgUri(uri_interface_ce);
	php_uri_implementation_set_object_handlers(whatwg_uri_ce, &whatwg_uri_object_handlers);

	uri_exception_ce = register_class_Uri_UriException(zend_ce_exception);
	uninitialized_uri_exception_ce = register_class_Uri_UninitializedUriException(uri_exception_ce);
	uri_operation_exception_ce = register_class_Uri_UriOperationException(uri_exception_ce);
	invalid_uri_exception_ce = register_class_Uri_InvalidUriException(uri_exception_ce);
	whatwg_error_ce = register_class_Uri_WhatWgError();

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
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_URI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uri)
#endif
