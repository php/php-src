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
#include "Zend/zend_attributes.h"
#include "Zend/zend_enum.h"
#include "main/php_ini.h"

#include "php_uri.h"
#include "php_uri_common.h"
#include "php_lexbor.h"
#include "php_uriparser.h"
#include "php_uri_arginfo.h"

zend_class_entry *rfc3986_uri_ce;
zend_object_handlers rfc3986_uri_object_handlers;
zend_class_entry *whatwg_url_ce;
zend_object_handlers whatwg_uri_object_handlers;
zend_class_entry *uri_comparison_mode_ce;
zend_class_entry *uri_exception_ce;
zend_class_entry *invalid_uri_exception_ce;
zend_class_entry *whatwg_invalid_url_exception_ce;
zend_class_entry *whatwg_url_validation_error_type_ce;
zend_class_entry *whatwg_url_validation_error_ce;

static const zend_module_dep uri_deps[] = {
	ZEND_MOD_REQUIRED("lexbor")
	ZEND_MOD_END
};

static zend_array uri_handlers;

static zend_object *uri_clone_obj_handler(zend_object *object);

static uri_handler_t *uri_handler_by_name(const char *handler_name, size_t handler_name_len)
{
	return zend_hash_str_find_ptr(&uri_handlers, handler_name, handler_name_len);
}

static HashTable *uri_get_debug_properties(zend_object *object)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	ZEND_ASSERT(internal_uri != NULL);

	zend_string *string_key;
	uri_property_handler_t *property_handler;

	HashTable *std_properties = zend_std_get_properties(object);
	HashTable *result = zend_array_dup(std_properties);

	if (UNEXPECTED(internal_uri->uri == NULL)) {
		return result;
	}

	zend_string *object_str = ZSTR_INIT_LITERAL("(object value omitted)", false);

	const HashTable *property_handlers = internal_uri->handler->property_handlers;
	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(property_handlers, string_key, property_handler) {
		zval value;

		ZEND_ASSERT(string_key != NULL);

		if (property_handler->read_func(internal_uri, URI_COMPONENT_READ_RAW, &value) == FAILURE) {
			continue;
		}

		if (Z_TYPE(value) == IS_OBJECT) {
			zval_ptr_dtor(&value);
			ZVAL_NEW_STR(&value, object_str);
			zend_string_addref(object_str);
		}

		zend_hash_update(result, string_key, &value);
	} ZEND_HASH_FOREACH_END();

	zend_string_release_ex(object_str, false);

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

static zend_result php_uri_get_property(const uri_internal_t *internal_uri, zend_string *name, uri_component_read_mode_t read_mode, zval *zv)
{
	uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, name);
	if (property_handler == NULL) {
		return FAILURE;
	}

	return property_handler->read_func(internal_uri, read_mode, zv);
}

PHPAPI zend_result php_uri_get_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_SCHEME), read_mode, zv);
}

PHPAPI zend_result php_uri_get_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_USERNAME), read_mode, zv);
}

PHPAPI zend_result php_uri_get_password(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PASSWORD), read_mode, zv);
}

PHPAPI zend_result php_uri_get_host(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_HOST), read_mode, zv);
}

PHPAPI zend_result php_uri_get_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PORT), read_mode, zv);
}

PHPAPI zend_result php_uri_get_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_PATH), read_mode, zv);
}

PHPAPI zend_result php_uri_get_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_QUERY), read_mode, zv);
}

PHPAPI zend_result php_uri_get_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *zv)
{
	return php_uri_get_property(internal_uri, ZSTR_KNOWN(ZEND_STR_FRAGMENT), read_mode, zv);
}

PHPAPI void php_uri_free(uri_internal_t *internal_uri)
{
	internal_uri->handler->free_uri(internal_uri->uri);
	internal_uri->uri = NULL;
	internal_uri->handler = NULL;
	efree(internal_uri);
}

PHPAPI php_uri *php_uri_parse_to_struct(const uri_handler_t *uri_handler, zend_string *uri_str, uri_component_read_mode_t read_mode, zval *errors)
{
	uri_internal_t *uri_internal = php_uri_parse(uri_handler, uri_str, errors);
	if (uri_internal == NULL) {
		return NULL;
	}

	php_uri *uri = ecalloc(1, sizeof(php_uri));
	zval tmp;
	ZVAL_UNDEF(&tmp);
	zend_result result;

	result = php_uri_get_scheme(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->scheme = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_username(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->user = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_password(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->password = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_host(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->host = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_port(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_LONG && Z_LVAL(tmp) >= 0) {
		uri->port = Z_LVAL(tmp);
	}

	result = php_uri_get_path(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->path = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_query(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->query = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	result = php_uri_get_fragment(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		php_uri_free(uri_internal);
		php_uri_struct_free(uri);
		return NULL;
	}
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		uri->fragment = zend_string_copy(Z_STR(tmp));
		zval_ptr_dtor(&tmp);
	}

	php_uri_free(uri_internal);

	return uri;
}

PHPAPI void php_uri_struct_free(php_uri *uri)
{
	if (uri->scheme) {
		zend_string_release(uri->scheme);
	}
	if (uri->user) {
		zend_string_release(uri->user);
	}
	if (uri->password) {
		zend_string_release(uri->password);
	}
	if (uri->host) {
		zend_string_release(uri->host);
	}
	if (uri->path) {
		zend_string_release(uri->path);
	}
	if (uri->query) {
		zend_string_release(uri->query);
	}
	if (uri->fragment) {
		zend_string_release(uri->fragment);
	}

	efree(uri);
}

PHP_METHOD(Uri_WhatWg_UrlValidationError, __construct)
{
	zend_string *context;
	zval *type;
	bool failure;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(context)
		Z_PARAM_OBJECT_OF_CLASS(type, whatwg_url_validation_error_type_ce)
		Z_PARAM_BOOL(failure)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), "context", sizeof("context") - 1, context);
	zend_update_property(whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), "type", sizeof("type") - 1, type);
	zval failure_zv;
	ZVAL_BOOL(&failure_zv, failure);
	zend_update_property(whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), "failure", sizeof("failure") - 1, &failure_zv);
	zval_ptr_dtor(&failure_zv);
}

static zend_result pass_errors_by_ref(zval *errors_zv, zval *errors)
{
	ZEND_ASSERT(Z_TYPE_P(errors) == IS_UNDEF || Z_TYPE_P(errors) == IS_ARRAY);

	if (Z_TYPE_P(errors) != IS_ARRAY) {
		return SUCCESS;
	}

	if (errors_zv != NULL) {
		errors_zv = zend_try_array_init(errors_zv);
		if (!errors_zv) {
			return FAILURE;
		}

		zval *error;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(errors), error) {
			Z_ADDREF_P(error);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(errors_zv), error);
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;
}

PHPAPI void php_uri_instantiate_uri(
	INTERNAL_FUNCTION_PARAMETERS, const uri_handler_t *handler, const zend_string *uri_str, const zend_object *base_url_object,
	bool should_throw, bool should_update_this_object, zval *errors_zv
) {
	zval errors;
	ZVAL_UNDEF(&errors);

	void *base_url = NULL;
	if (base_url_object != NULL) {
		uri_internal_t *internal_base_url = uri_internal_from_obj(base_url_object);
		URI_ASSERT_INITIALIZATION(internal_base_url);
		base_url = internal_base_url->uri;
	}

	void *uri = handler->parse_uri(uri_str, base_url, should_throw || errors_zv != NULL ? &errors : NULL);
	if (UNEXPECTED(uri == NULL)) {
		if (should_throw) {
			throw_invalid_uri_exception(handler, &errors);
			zval_ptr_dtor(&errors);
			RETURN_THROWS();
		} else {
			pass_errors_by_ref(errors_zv, &errors);
			zval_ptr_dtor(&errors);
			RETURN_NULL();
		}
	}

	pass_errors_by_ref(errors_zv, &errors);
	zval_ptr_dtor(&errors);

	uri_object_t *uri_object;
	if (should_update_this_object) {
		uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	} else {
		if (EX(func)->common.fn_flags & ZEND_ACC_STATIC) {
			object_init_ex(return_value, Z_CE_P(ZEND_THIS));
		} else {
			object_init_ex(return_value, Z_OBJCE_P(ZEND_THIS));
		}
		uri_object = Z_URI_OBJECT_P(return_value);
	}

	uri_object->internal.handler = handler;
	uri_object->internal.uri = uri;
}

static void create_rfc3986_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str;
	zend_object *base_url_object = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, rfc3986_uri_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &uriparser_uri_handler, uri_str, base_url_object, is_constructor, is_constructor, NULL);
}

PHP_METHOD(Uri_Rfc3986_Uri, parse)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Uri_Rfc3986_Uri, __construct)
{
	create_rfc3986_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static void create_whatwg_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str;
	zend_object *base_url_object = NULL;
	zval *errors = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, whatwg_url_ce)
		Z_PARAM_ZVAL(errors)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, &lexbor_uri_handler, uri_str, base_url_object, is_constructor, is_constructor, errors);
}

PHP_METHOD(Uri_WhatWg_Url, parse)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Uri_WhatWg_Url, __construct)
{
	create_whatwg_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(Uri_Rfc3986_Uri, getScheme)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_SCHEME), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawScheme)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_SCHEME), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withScheme)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_Rfc3986_Uri, getUserInfo)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USERINFO), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUserInfo)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USERINFO), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withUserInfo)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_USERINFO));
}

PHP_METHOD(Uri_Rfc3986_Uri, getUsername)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USERNAME), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUsername)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_USERNAME), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPassword)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PASSWORD), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPassword)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PASSWORD), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withHost)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_HOST));
}

PHP_METHOD(Uri_Rfc3986_Uri, getPort)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PORT), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, withPort)
{
	URI_WITHER_LONG_OR_NULL(ZSTR_KNOWN(ZEND_STR_PORT));
}

PHP_METHOD(Uri_Rfc3986_Uri, getPath)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PATH), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPath)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_PATH), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withPath)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_PATH));
}

PHP_METHOD(Uri_Rfc3986_Uri, getQuery)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_QUERY), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawQuery)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_QUERY), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withQuery)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_QUERY));
}

PHP_METHOD(Uri_Rfc3986_Uri, getFragment)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_FRAGMENT), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawFragment)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_FRAGMENT), URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withFragment)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_FRAGMENT));
}

static void uri_equals(INTERNAL_FUNCTION_PARAMETERS, zend_object *that_object, zend_object *comparison_mode)
{
	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *this_internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(this_internal_uri);

	uri_internal_t *that_internal_uri = uri_internal_from_obj(that_object);
	URI_ASSERT_INITIALIZATION(that_internal_uri);

	if (this_object->ce != that_object->ce &&
		!instanceof_function(this_object->ce, that_object->ce) &&
		!instanceof_function(that_object->ce, this_object->ce)
	) {
		RETURN_FALSE;
	}

	bool exclude_fragment = true;
	if (comparison_mode) {
		zval *case_name = zend_enum_fetch_case_name(comparison_mode);
		zend_string *comparison_mode_name = Z_STR_P(case_name);

		exclude_fragment = ZSTR_VAL(comparison_mode_name)[0] + ZSTR_LEN(comparison_mode_name) == 'E' + sizeof("ExcludeFragment") - 1;
	}

	zend_string *this_str = this_internal_uri->handler->uri_to_string(
		this_internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, exclude_fragment);
	if (this_str == NULL) {
		zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to string", ZSTR_VAL(this_object->ce->name));
		RETURN_THROWS();
	}

	zend_string *that_str = that_internal_uri->handler->uri_to_string(
		that_internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, exclude_fragment);
	if (that_str == NULL) {
		zend_string_release(this_str);
		zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to string", ZSTR_VAL(that_object->ce->name));
		RETURN_THROWS();
	}

	RETVAL_BOOL(zend_string_equals(this_str, that_str));

	zend_string_release(this_str);
	zend_string_release(that_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, equals)
{
	zend_object *that_object;
	zend_object *comparison_mode = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(that_object, rfc3986_uri_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(comparison_mode, uri_comparison_mode_ce);
	ZEND_PARSE_PARAMETERS_END();

	uri_equals(INTERNAL_FUNCTION_PARAM_PASSTHRU, that_object, comparison_mode);
}

PHP_METHOD(Uri_Rfc3986_Uri, toRawString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	zend_string *uri_str = internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_RAW_ASCII, false);
	if (uri_str == NULL) {
		zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to string", ZSTR_VAL(this_object->ce->name));
		RETURN_THROWS();
	}

	RETVAL_STR(uri_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	zend_string *uri_str = internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, false);
	if (uri_str == NULL) {
		zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to string", ZSTR_VAL(this_object->ce->name));
		RETURN_THROWS();
	}

	RETVAL_STR(uri_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, resolve)
{
	zend_string *uri_str;

	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_PATH_STR(uri_str) \
	ZEND_PARSE_PARAMETERS_END(); \

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal_uri->handler, uri_str, this_object, true, false, NULL);
}

PHP_METHOD(Uri_Rfc3986_Uri, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	/* Serialize state: "uri" key in the first array */
	zend_string *uri_str = internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_RAW_ASCII, false);
	if (uri_str == NULL) {
		zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to string", ZSTR_VAL(this_object->ce->name));
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_STR(&tmp, uri_str);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), URI_SERIALIZED_PROPERTY_NAME, sizeof(URI_SERIALIZED_PROPERTY_NAME) - 1, &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_ARR(&arr, this_object->handlers->get_properties(this_object));
	Z_TRY_ADDREF(arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

static void uri_unserialize(INTERNAL_FUNCTION_PARAMETERS, const char *handler_name)
{
	HashTable *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *object = Z_OBJ_P(ZEND_THIS);

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	/* Unserialize state: "uri" key in the first array */
	zval *arr = zend_hash_index_find(data, 0);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	zval *uri_zv = zend_hash_str_find_ind(Z_ARRVAL_P(arr), URI_SERIALIZED_PROPERTY_NAME, sizeof(URI_SERIALIZED_PROPERTY_NAME) - 1);
	if (uri_zv == NULL || Z_TYPE_P(uri_zv) != IS_STRING) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	zval errors;
	ZVAL_UNDEF(&errors);

	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	internal_uri->handler = uri_handler_by_name(handler_name, strlen(handler_name));
	if (internal_uri->uri != NULL) {
		internal_uri->handler->free_uri(internal_uri->uri);
	}
	internal_uri->uri = internal_uri->handler->parse_uri(Z_STR_P(uri_zv), NULL, &errors);
	if (internal_uri->uri == NULL) {
		throw_invalid_uri_exception(internal_uri->handler, &errors);
		zval_ptr_dtor(&errors);
		RETURN_THROWS();
	}
	zval_ptr_dtor(&errors);

	/* Unserialize regular properties: second array */
	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	object_properties_load(object, Z_ARRVAL_P(arr));
	if (EG(exception)) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Rfc3986_Uri, __unserialize)
{
	uri_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PARSER_RFC3986);
}

PHP_METHOD(Uri_Rfc3986_Uri, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *object = Z_OBJ_P(ZEND_THIS);

	RETURN_ARR(uri_get_debug_properties(object));
}

PHP_METHOD(Uri_WhatWg_Url, getScheme)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_SCHEME), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, withScheme)
{
	URI_WITHER_STR(ZSTR_KNOWN(ZEND_STR_SCHEME));
}

PHP_METHOD(Uri_WhatWg_Url, withUsername)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_USERNAME));
}

PHP_METHOD(Uri_WhatWg_Url, withPassword)
{
	URI_WITHER_STR_OR_NULL(ZSTR_KNOWN(ZEND_STR_PASSWORD));
}

PHP_METHOD(Uri_WhatWg_Url, getAsciiHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST), URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, getUnicodeHost)
{
	URI_GETTER(ZSTR_KNOWN(ZEND_STR_HOST), URI_COMPONENT_READ_NORMALIZED_UNICODE);
}

PHP_METHOD(Uri_WhatWg_Url, withHost)
{
	zend_string *value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	if (value == NULL) {
		ZVAL_NULL(&zv);
	} else {
		ZVAL_STR_COPY(&zv, value);
	}

	URI_WITHER_COMMON(ZSTR_KNOWN(ZEND_STR_HOST), &zv, return_value);
}

PHP_METHOD(Uri_WhatWg_Url, equals)
{
	zend_object *that_object;
	zend_object *comparison_mode = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(that_object, whatwg_url_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(comparison_mode, uri_comparison_mode_ce);
	ZEND_PARSE_PARAMETERS_END();

	uri_equals(INTERNAL_FUNCTION_PARAM_PASSTHRU, that_object, comparison_mode);
}

PHP_METHOD(Uri_WhatWg_Url, toUnicodeString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	RETURN_STR(internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_RAW_UNICODE, false));
}

PHP_METHOD(Uri_WhatWg_Url, toAsciiString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	RETURN_STR(internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_RAW_ASCII, false));
}

PHP_METHOD(Uri_WhatWg_Url, resolve)
{
	zend_string *uri_str;
	zval *errors = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(errors)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU, internal_uri->handler, uri_str, this_object, true, false, errors);
}

PHP_METHOD(Uri_WhatWg_Url, __unserialize)
{
	uri_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PARSER_WHATWG);
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

static zend_object *uri_clone_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);

	zend_object *new_object = uri_create_object_handler(object->ce);
	ZEND_ASSERT(new_object != NULL);
	uri_object_t *new_uri_object = uri_object_from_obj(new_object);

	URI_ASSERT_INITIALIZATION(internal_uri);

	new_uri_object->internal.handler = internal_uri->handler;

	void *uri = internal_uri->handler->clone_uri(internal_uri->uri);
	ZEND_ASSERT(uri != NULL);

	new_uri_object->internal.uri = uri;

	zend_objects_clone_members(&new_uri_object->std, &uri_object->std);

	return &new_uri_object->std;
}

PHPAPI void php_uri_implementation_set_object_handlers(zend_class_entry *ce, zend_object_handlers *object_handlers)
{
	ce->create_object = uri_create_object_handler;
	ce->default_object_handlers = object_handlers;
	memcpy(object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	object_handlers->offset = XtOffsetOf(uri_object_t, std);
	object_handlers->free_obj = uri_free_obj_handler;
	object_handlers->clone_obj = uri_clone_obj_handler;
}

zend_result uri_handler_register(const uri_handler_t *uri_handler)
{
	zend_string *key = zend_string_init_interned(uri_handler->name, strlen(uri_handler->name), 1);

	ZEND_ASSERT(uri_handler->name != NULL);
	ZEND_ASSERT(uri_handler->init_parser != NULL);
	ZEND_ASSERT(uri_handler->parse_uri != NULL);
	ZEND_ASSERT(uri_handler->create_invalid_uri_exception != NULL);
	ZEND_ASSERT(uri_handler->clone_uri != NULL);
	ZEND_ASSERT(uri_handler->uri_to_string != NULL);
	ZEND_ASSERT(uri_handler->free_uri != NULL);
	ZEND_ASSERT(uri_handler->destroy_parser != NULL);
	ZEND_ASSERT(uri_handler->property_handlers != NULL);

	return zend_hash_add_ptr(&uri_handlers, key, (void *) uri_handler) ? SUCCESS : FAILURE;
}

static PHP_MINIT_FUNCTION(uri)
{
	rfc3986_uri_ce = register_class_Uri_Rfc3986_Uri();
	php_uri_implementation_set_object_handlers(rfc3986_uri_ce, &rfc3986_uri_object_handlers);

	whatwg_url_ce = register_class_Uri_WhatWg_Url();
	php_uri_implementation_set_object_handlers(whatwg_url_ce, &whatwg_uri_object_handlers);

	uri_comparison_mode_ce = register_class_Uri_UriComparisonMode();
	uri_exception_ce = register_class_Uri_UriException(zend_ce_exception);
	invalid_uri_exception_ce = register_class_Uri_InvalidUriException(uri_exception_ce);
	whatwg_invalid_url_exception_ce = register_class_Uri_WhatWg_InvalidUrlException(invalid_uri_exception_ce);
	whatwg_url_validation_error_ce = register_class_Uri_WhatWg_UrlValidationError();
	whatwg_url_validation_error_type_ce = register_class_Uri_WhatWg_UrlValidationErrorType();

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
	STANDARD_MODULE_HEADER_EX, NULL,
	uri_deps,
	"uri",                          /* Extension name */
	NULL,                           /* zend_function_entry */
	PHP_MINIT(uri),                 /* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(uri),             /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(uri),                 /* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(uri),             /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(uri),                 /* PHP_MINFO - Module info */
	PHP_VERSION,                    /* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_URI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uri)
#endif
