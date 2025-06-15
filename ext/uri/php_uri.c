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
# include <config.h>
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_attributes.h"
#include "Zend/zend_enum.h"
#include "ext/standard/info.h"

#include "php_uri_common.h"
#include "php_lexbor.h"
#include "php_uriparser.h"
#include "php_uri_arginfo.h"
#include "uriparser/src/UriConfig.h"

zend_class_entry *uri_rfc3986_uri_ce;
zend_object_handlers uri_rfc3986_uri_object_handlers;
zend_class_entry *uri_whatwg_url_ce;
zend_object_handlers uri_whatwg_uri_object_handlers;
zend_class_entry *uri_comparison_mode_ce;
zend_class_entry *uri_exception_ce;
zend_class_entry *uri_invalid_uri_exception_ce;
zend_class_entry *uri_whatwg_invalid_url_exception_ce;
zend_class_entry *uri_whatwg_url_validation_error_type_ce;
zend_class_entry *uri_whatwg_url_validation_error_ce;

#define URIPARSER_VERSION PACKAGE_VERSION

static const zend_module_dep uri_deps[] = {
	ZEND_MOD_REQUIRED("lexbor")
	ZEND_MOD_END
};

static zend_array uri_handlers;

static uri_handler_t *uri_handler_by_name(const char *handler_name, size_t handler_name_len)
{
	return zend_hash_str_find_ptr(&uri_handlers, handler_name, handler_name_len);
}

static HashTable *uri_get_debug_properties(zend_object *object)
{
	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	ZEND_ASSERT(internal_uri != NULL);

	HashTable *std_properties = zend_std_get_properties(object);
	HashTable *result = zend_array_dup(std_properties);

	if (UNEXPECTED(internal_uri->uri == NULL)) {
		return result;
	}

	const uri_property_handlers_t property_handlers = internal_uri->handler->property_handlers;

	zval tmp;
	if (property_handlers.scheme.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}

	if (property_handlers.username.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_USERNAME), &tmp);
	}

	if (property_handlers.password.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp);
	}

	if (property_handlers.host.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}

	if (property_handlers.port.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}

	if (property_handlers.path.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}

	if (property_handlers.query.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}

	if (property_handlers.fragment.read_func(internal_uri, URI_COMPONENT_READ_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}

	return result;
}

/**
 * Pass the errors parameter by ref to errors_zv for userland, and frees it if
 * it is not not needed anymore.
 */
static zend_result pass_errors_by_ref_and_free(zval *errors_zv, zval *errors)
{
	ZEND_ASSERT(Z_TYPE_P(errors) == IS_UNDEF || Z_TYPE_P(errors) == IS_ARRAY);

	/* There was no error during parsing */
	if (Z_ISUNDEF_P(errors)) {
		return SUCCESS;
	}

	/* The errors parameter is an array, but the pass-by ref argument stored by
	 * errors_zv was not passed - the URI implementation either doesn't support
	 * returning additional error information, or the caller is not interested in it */
	if (errors_zv == NULL) {
		zval_ptr_dtor(errors);
		return SUCCESS;
	}

	ZEND_TRY_ASSIGN_REF_ARR(errors_zv, Z_ARRVAL_P(errors));
	if (EG(exception)) {
		zval_ptr_dtor(errors);
		return FAILURE;
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

	void *uri = handler->parse_uri(uri_str, base_url, should_throw || errors_zv != NULL ? &errors : NULL, !should_throw);
	if (UNEXPECTED(uri == NULL)) {
		if (should_throw) {
			zval_ptr_dtor(&errors);
			RETURN_THROWS();
		} else {
			if (pass_errors_by_ref_and_free(errors_zv, &errors) == FAILURE) {
				RETURN_THROWS();
			}
			RETURN_NULL();
		}
	}

	if (pass_errors_by_ref_and_free(errors_zv, &errors) == FAILURE) {
		RETURN_THROWS();
	}

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
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, uri_rfc3986_uri_ce)
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

PHP_METHOD(Uri_WhatWg_InvalidUrlException, __construct)
{
	zend_string *message = NULL;
	zval *errors = NULL;
	zend_long code = 0;
	zval *previous = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 4)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(message)
		Z_PARAM_ARRAY(errors)
		Z_PARAM_LONG(code)
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(previous, zend_ce_throwable)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_update_exception_properties(INTERNAL_FUNCTION_PARAM_PASSTHRU, message, code, previous) == FAILURE) {
		RETURN_THROWS();
	}

	if (errors == NULL) {
		zval tmp;
		ZVAL_EMPTY_ARRAY(&tmp);
		zend_update_property(uri_whatwg_invalid_url_exception_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("errors"), &tmp);
	} else {
		zend_update_property(uri_whatwg_invalid_url_exception_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("errors"), errors);
	}
	if (EG(exception)) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_WhatWg_UrlValidationError, __construct)
{
	zend_string *context;
	zval *type;
	bool failure;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(context)
		Z_PARAM_OBJECT_OF_CLASS(type, uri_whatwg_url_validation_error_type_ce)
		Z_PARAM_BOOL(failure)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(uri_whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("context"), context);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	zend_update_property_ex(uri_whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_TYPE), type);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	zval failure_zv;
	ZVAL_BOOL(&failure_zv, failure);
	zend_update_property(uri_whatwg_url_validation_error_ce, Z_OBJ_P(ZEND_THIS), ZEND_STRL("failure"), &failure_zv);
	if (EG(exception)) {
		RETURN_THROWS();
	}
}

static void create_whatwg_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str;
	zend_object *base_url_object = NULL;
	zval *errors = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, uri_whatwg_url_ce)
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
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_SCHEME, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawScheme)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_SCHEME, URI_COMPONENT_READ_RAW);
}

static void read_uriparser_userinfo(INTERNAL_FUNCTION_PARAMETERS, uri_component_read_mode_t read_mode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS);
	URI_ASSERT_INITIALIZATION(internal_uri);

	if (UNEXPECTED(uriparser_read_userinfo(internal_uri, read_mode, return_value) == FAILURE)) {
		zend_throw_error(NULL, "The userinfo component cannot be retrieved");
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Rfc3986_Uri, getUserInfo)
{
	read_uriparser_userinfo(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUserInfo)
{
	read_uriparser_userinfo(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getUsername)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_USERNAME, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUsername)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_USERNAME, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPassword)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PASSWORD, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPassword)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PASSWORD, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getHost)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_HOST, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawHost)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_HOST, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPort)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PORT, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPath)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PATH, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPath)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PATH, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getQuery)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_QUERY, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawQuery)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_QUERY, URI_COMPONENT_READ_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getFragment)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_FRAGMENT, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawFragment)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_FRAGMENT, URI_COMPONENT_READ_RAW);
}

static void throw_cannot_recompose_uri_to_string(zend_object *object)
{
	zend_throw_exception_ex(NULL, 0, "Cannot recompose %s to a string", ZSTR_VAL(object->ce->name));
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
		exclude_fragment = zend_string_equals_literal(Z_STR_P(case_name), "ExcludeFragment");
	}

	zend_string *this_str = this_internal_uri->handler->uri_to_string(
		this_internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, exclude_fragment);
	if (this_str == NULL) {
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}

	zend_string *that_str = that_internal_uri->handler->uri_to_string(
		that_internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, exclude_fragment);
	if (that_str == NULL) {
		zend_string_release(this_str);
		throw_cannot_recompose_uri_to_string(that_object);
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
		Z_PARAM_OBJ_OF_CLASS(that_object, uri_rfc3986_uri_ce)
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
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}

	RETURN_STR(uri_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	zend_string *uri_str = internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_NORMALIZED_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}

	RETURN_STR(uri_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, resolve)
{
	zend_string *uri_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(uri_str)
	ZEND_PARSE_PARAMETERS_END();

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
		throw_cannot_recompose_uri_to_string(this_object);
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

	/* Verify the expected number of elements inside the first array, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) != 1) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	zval *uri_zv = zend_hash_str_find_ind(Z_ARRVAL_P(arr), ZEND_STRL(URI_SERIALIZED_PROPERTY_NAME));
	if (uri_zv == NULL || Z_TYPE_P(uri_zv) != IS_STRING) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	uri_internal_t *internal_uri = uri_internal_from_obj(object);
	internal_uri->handler = uri_handler_by_name(handler_name, strlen(handler_name));
	if (internal_uri->uri != NULL) {
		internal_uri->handler->free_uri(internal_uri->uri);
	}
	internal_uri->uri = internal_uri->handler->parse_uri(Z_STR_P(uri_zv), NULL, NULL, true);
	if (internal_uri->uri == NULL) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	/* Unserialize regular properties: second array */
	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(object->ce->name));
		RETURN_THROWS();
	}

	/* Verify that there is no regular property in the second array, because the URI classes have no properties and they are final. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) > 0) {
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
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_SCHEME, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, withScheme)
{
	uri_write_component_str(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_SCHEME);
}

PHP_METHOD(Uri_WhatWg_Url, withUsername)
{
	uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_USERNAME);
}

PHP_METHOD(Uri_WhatWg_Url, withPassword)
{
	uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PASSWORD);
}

PHP_METHOD(Uri_WhatWg_Url, getAsciiHost)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_HOST, URI_COMPONENT_READ_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, getUnicodeHost)
{
	uri_read_component(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_HOST, URI_COMPONENT_READ_NORMALIZED_UNICODE);
}

PHP_METHOD(Uri_WhatWg_Url, withHost)
{
	uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_HOST);
}

PHP_METHOD(Uri_WhatWg_Url, withPort)
{
	uri_write_component_long_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PORT);
}

PHP_METHOD(Uri_WhatWg_Url, withPath)
{
	uri_write_component_str(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_PATH);
}

PHP_METHOD(Uri_WhatWg_Url, withQuery)
{
	uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_QUERY);
}

PHP_METHOD(Uri_WhatWg_Url, withFragment)
{
	uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PROPERTY_NAME_FRAGMENT);
}

PHP_METHOD(Uri_WhatWg_Url, equals)
{
	zend_object *that_object;
	zend_object *comparison_mode = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(that_object, uri_whatwg_url_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(comparison_mode, uri_comparison_mode_ce)
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

PHP_METHOD(Uri_WhatWg_Url, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = uri_internal_from_obj(this_object);
	URI_ASSERT_INITIALIZATION(internal_uri);

	/* Serialize state: "uri" key in the first array */
	zend_string *uri_str = internal_uri->handler->uri_to_string(internal_uri->uri, URI_RECOMPOSITION_RAW_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_STR(&tmp, uri_str);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL(URI_SERIALIZED_PROPERTY_NAME), &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_ARR(&arr, this_object->handlers->get_properties(this_object));
	Z_ADDREF(arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

PHP_METHOD(Uri_WhatWg_Url, __unserialize)
{
	uri_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU, URI_PARSER_WHATWG);
}

PHP_METHOD(Uri_WhatWg_Url, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *object = Z_OBJ_P(ZEND_THIS);

	RETURN_ARR(uri_get_debug_properties(object));
}

static zend_object *uri_create_object_handler(zend_class_entry *class_type)
{
	uri_object_t *uri_object = zend_object_alloc(sizeof(uri_object_t), class_type);

	zend_object_std_init(&uri_object->std, class_type);
	object_properties_init(&uri_object->std, class_type);

	return &uri_object->std;
}

static void uri_free_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);

	if (UNEXPECTED(uri_object->internal.uri != NULL)) {
		uri_object->internal.handler->free_uri(uri_object->internal.uri);
		uri_object->internal.handler = NULL;
		uri_object->internal.uri = NULL;
	}

	zend_object_std_dtor(&uri_object->std);
}

zend_object *uri_clone_obj_handler(zend_object *object)
{
	uri_object_t *uri_object = uri_object_from_obj(object);
	uri_internal_t *internal_uri = uri_internal_from_obj(object);

	URI_ASSERT_INITIALIZATION(internal_uri);

	zend_object *new_object = uri_create_object_handler(object->ce);
	ZEND_ASSERT(new_object != NULL);
	uri_object_t *new_uri_object = uri_object_from_obj(new_object);

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
	ZEND_ASSERT(uri_handler->parse_uri != NULL);
	ZEND_ASSERT(uri_handler->clone_uri != NULL);
	ZEND_ASSERT(uri_handler->uri_to_string != NULL);
	ZEND_ASSERT(uri_handler->free_uri != NULL);

	zend_result result = zend_hash_add_ptr(&uri_handlers, key, (void *) uri_handler) != NULL ? SUCCESS : FAILURE;

	zend_string_release_ex(key, true);

	return result;
}

static PHP_MINIT_FUNCTION(uri)
{
	uri_rfc3986_uri_ce = register_class_Uri_Rfc3986_Uri();
	php_uri_implementation_set_object_handlers(uri_rfc3986_uri_ce, &uri_rfc3986_uri_object_handlers);

	uri_whatwg_url_ce = register_class_Uri_WhatWg_Url();
	php_uri_implementation_set_object_handlers(uri_whatwg_url_ce, &uri_whatwg_uri_object_handlers);

	uri_comparison_mode_ce = register_class_Uri_UriComparisonMode();
	uri_exception_ce = register_class_Uri_UriException(zend_ce_exception);
	uri_invalid_uri_exception_ce = register_class_Uri_InvalidUriException(uri_exception_ce);
	uri_whatwg_invalid_url_exception_ce = register_class_Uri_WhatWg_InvalidUrlException(uri_invalid_uri_exception_ce);
	uri_whatwg_url_validation_error_ce = register_class_Uri_WhatWg_UrlValidationError();
	uri_whatwg_url_validation_error_type_ce = register_class_Uri_WhatWg_UrlValidationErrorType();

	zend_hash_init(&uri_handlers, 4, NULL, NULL, true);

	if (PHP_MINIT(uri_uriparser)(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}

	if (uri_handler_register(&lexbor_uri_handler) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(uri)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "uri support", "active");
	php_info_print_table_row(2, "uriparser library version", URIPARSER_VERSION);
	php_info_print_table_end();
}

static PHP_MSHUTDOWN_FUNCTION(uri)
{
	zend_hash_destroy(&uri_handlers);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri)
{
	if (lexbor_request_init() == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(uri)
{
	lexbor_request_shutdown();

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
