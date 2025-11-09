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

#include "php_uri.h"
#include "uri_parser_whatwg.h"
#include "uri_parser_rfc3986.h"
#include "uri_parser_php_parse_url.h"
#include "php_uri_arginfo.h"
#include "uriparser/UriBase.h"

zend_class_entry *php_uri_ce_rfc3986_uri;
zend_class_entry *php_uri_ce_whatwg_url;
zend_class_entry *php_uri_ce_comparison_mode;
zend_class_entry *php_uri_ce_exception;
zend_class_entry *php_uri_ce_error;
zend_class_entry *php_uri_ce_invalid_uri_exception;
zend_class_entry *php_uri_ce_whatwg_invalid_url_exception;
zend_class_entry *php_uri_ce_whatwg_url_validation_error_type;
zend_class_entry *php_uri_ce_whatwg_url_validation_error;

static zend_object_handlers object_handlers_rfc3986_uri;
static zend_object_handlers object_handlers_whatwg_uri;

static const zend_module_dep uri_deps[] = {
	ZEND_MOD_REQUIRED("lexbor")
	ZEND_MOD_END
};

static zend_array uri_parsers;

static HashTable *uri_get_debug_properties(php_uri_object *object)
{
	const HashTable *std_properties = zend_std_get_properties(&object->std);
	HashTable *result = zend_array_dup(std_properties);

	const php_uri_parser * const parser = object->parser;
	void * const uri = object->uri;

	if (UNEXPECTED(uri == NULL)) {
		return result;
	}

	zval tmp;
	if (parser->property_handler.scheme.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}

	if (parser->property_handler.username.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_USERNAME), &tmp);
	}

	if (parser->property_handler.password.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp);
	}

	if (parser->property_handler.host.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}

	if (parser->property_handler.port.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}

	if (parser->property_handler.path.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}

	if (parser->property_handler.query.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}

	if (parser->property_handler.fragment.read(uri, PHP_URI_COMPONENT_READ_MODE_RAW, &tmp) == SUCCESS) {
		zend_hash_update(result, ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}

	return result;
}

PHPAPI const php_uri_parser *php_uri_get_parser(zend_string *uri_parser_name)
{
	if (uri_parser_name == NULL) {
		return zend_hash_str_find_ptr(&uri_parsers, PHP_URI_PARSER_PHP_PARSE_URL, sizeof(PHP_URI_PARSER_PHP_PARSE_URL) - 1);
	}

	return zend_hash_find_ptr(&uri_parsers, uri_parser_name);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI php_uri_internal *php_uri_parse(const php_uri_parser *uri_parser, const char *uri_str, size_t uri_str_len, bool silent)
{
	void *uri = uri_parser->parse(uri_str, uri_str_len, NULL, NULL, silent);

	if (uri == NULL) {
		return NULL;
	}

	php_uri_internal *internal_uri = emalloc(sizeof(*internal_uri));
	internal_uri->parser = uri_parser;
	internal_uri->uri = uri;

	return internal_uri;
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_scheme(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.scheme.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_username(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.username.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_password(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.password.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_host(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.host.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_port(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.port.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_path(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.path.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_query(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.query.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI zend_result php_uri_get_fragment(const php_uri_internal *internal_uri, php_uri_component_read_mode read_mode, zval *zv)
{
	return internal_uri->parser->property_handler.fragment.read(internal_uri->uri, read_mode, zv);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI void php_uri_free(php_uri_internal *internal_uri)
{
	internal_uri->parser->destroy(internal_uri->uri);
	internal_uri->uri = NULL;
	internal_uri->parser = NULL;
	efree(internal_uri);
}

ZEND_ATTRIBUTE_NONNULL PHPAPI php_uri *php_uri_parse_to_struct(
	const php_uri_parser *uri_parser, const char *uri_str, size_t uri_str_len, php_uri_component_read_mode read_mode, bool silent
) {
	php_uri_internal *uri_internal = php_uri_parse(uri_parser, uri_str, uri_str_len, silent);
	if (uri_internal == NULL) {
		return NULL;
	}

	php_uri *uri = ecalloc(1, sizeof(*uri));
	zval tmp;
	zend_result result;

	result = php_uri_get_scheme(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->scheme = Z_STR(tmp);
	}

	result = php_uri_get_username(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->user = Z_STR(tmp);
	}

	result = php_uri_get_password(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->password = Z_STR(tmp);
	}

	result = php_uri_get_host(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->host = Z_STR(tmp);
	}

	result = php_uri_get_port(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_LONG) {
		uri->port = Z_LVAL(tmp);
	}

	result = php_uri_get_path(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->path = Z_STR(tmp);
	}

	result = php_uri_get_query(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->query = Z_STR(tmp);
	}

	result = php_uri_get_fragment(uri_internal, read_mode, &tmp);
	if (result == FAILURE) {
		goto error;
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		uri->fragment = Z_STR(tmp);
	}

	php_uri_free(uri_internal);

	return uri;

error:
	php_uri_free(uri_internal);
	php_uri_struct_free(uri);

	return NULL;
}

ZEND_ATTRIBUTE_NONNULL PHPAPI void php_uri_struct_free(php_uri *uri)
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

	ZEND_TRY_ASSIGN_REF_TMP(errors_zv, errors);
	if (EG(exception)) {
		return FAILURE;
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL_ARGS(1, 2) PHPAPI void php_uri_instantiate_uri(
		INTERNAL_FUNCTION_PARAMETERS, const zend_string *uri_str, const php_uri_object *base_url_object,
		bool should_throw, bool should_update_this_object, zval *errors_zv
) {

	php_uri_object *uri_object;
	if (should_update_this_object) {
		uri_object = Z_URI_OBJECT_P(ZEND_THIS);
		if (uri_object->uri != NULL) {
			zend_throw_error(NULL, "Cannot modify readonly object of class %s", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
			RETURN_THROWS();
		}
	} else {
		if (EX(func)->common.fn_flags & ZEND_ACC_STATIC) {
			object_init_ex(return_value, Z_CE_P(ZEND_THIS));
		} else {
			object_init_ex(return_value, Z_OBJCE_P(ZEND_THIS));
		}
		uri_object = Z_URI_OBJECT_P(return_value);
	}

	const php_uri_parser *uri_parser = uri_object->parser;

	zval errors;
	ZVAL_UNDEF(&errors);

	void *base_url = NULL;
	if (base_url_object != NULL) {
		ZEND_ASSERT(base_url_object->std.ce == uri_object->std.ce);
		ZEND_ASSERT(base_url_object->uri != NULL);
		ZEND_ASSERT(base_url_object->parser == uri_parser);
		base_url = base_url_object->uri;
	}

	void *uri = uri_parser->parse(ZSTR_VAL(uri_str), ZSTR_LEN(uri_str), base_url, errors_zv != NULL ? &errors : NULL, !should_throw);
	if (UNEXPECTED(uri == NULL)) {
		if (should_throw) {
			zval_ptr_dtor(&errors);
			RETURN_THROWS();
		} else {
			if (pass_errors_by_ref_and_free(errors_zv, &errors) == FAILURE) {
				RETURN_THROWS();
			}
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}
	}

	if (pass_errors_by_ref_and_free(errors_zv, &errors) == FAILURE) {
		uri_parser->destroy(uri);
		RETURN_THROWS();
	}

	uri_object->uri = uri;
}

static void create_rfc3986_uri(INTERNAL_FUNCTION_PARAMETERS, bool is_constructor)
{
	zend_string *uri_str;
	zend_object *base_url_object = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(uri_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, php_uri_ce_rfc3986_uri)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		uri_str, base_url_object ? php_uri_object_from_obj(base_url_object) : NULL, is_constructor, is_constructor, NULL);
}

static bool is_list_of_whatwg_validation_errors(const HashTable *array)
{
	if (!zend_array_is_list(array)) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL(array, zval *val) {
		/* Do not allow references as they may change types after checking. */

		if (Z_TYPE_P(val) != IS_OBJECT) {
			return false;
		}

		if (!instanceof_function(Z_OBJCE_P(val), php_uri_ce_whatwg_url_validation_error)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
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
		zend_update_property(php_uri_ce_whatwg_invalid_url_exception, Z_OBJ_P(ZEND_THIS), ZEND_STRL("errors"), &tmp);
	} else {
		if (!is_list_of_whatwg_validation_errors(Z_ARR_P(errors))) {
			zend_argument_value_error(2, "must be a list of %s", ZSTR_VAL(php_uri_ce_whatwg_url_validation_error->name));
			RETURN_THROWS();
		}

		zend_update_property(php_uri_ce_whatwg_invalid_url_exception, Z_OBJ_P(ZEND_THIS), ZEND_STRL("errors"), errors);
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
		Z_PARAM_OBJECT_OF_CLASS(type, php_uri_ce_whatwg_url_validation_error_type)
		Z_PARAM_BOOL(failure)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(php_uri_ce_whatwg_url_validation_error, Z_OBJ_P(ZEND_THIS), ZEND_STRL("context"), context);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	zend_update_property_ex(php_uri_ce_whatwg_url_validation_error, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_TYPE), type);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	zval failure_zv;
	ZVAL_BOOL(&failure_zv, failure);
	zend_update_property(php_uri_ce_whatwg_url_validation_error, Z_OBJ_P(ZEND_THIS), ZEND_STRL("failure"), &failure_zv);
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
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(base_url_object, php_uri_ce_whatwg_url)
		Z_PARAM_ZVAL(errors)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		uri_str, base_url_object ? php_uri_object_from_obj(base_url_object) : NULL, is_constructor, is_constructor, errors);
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
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_SCHEME, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawScheme)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_SCHEME, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withScheme)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_SCHEME);
}

static void rfc3986_userinfo_read(INTERNAL_FUNCTION_PARAMETERS, php_uri_component_read_mode read_mode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uri_object->uri != NULL);

	if (UNEXPECTED(php_uri_parser_rfc3986_userinfo_read(uri_object->uri, read_mode, return_value) == FAILURE)) {
		zend_throw_error(NULL, "The userinfo component cannot be retrieved");
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Rfc3986_Uri, getUserInfo)
{
	rfc3986_userinfo_read(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUserInfo)
{
	rfc3986_userinfo_read(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withUserInfo)
{
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	if (value == NULL) {
		ZVAL_NULL(&zv);
	} else {
		ZVAL_STR(&zv, value);
	}

	php_uri_object *old_uri_object = php_uri_object_from_obj(Z_OBJ_P(ZEND_THIS));
	ZEND_ASSERT(old_uri_object->uri != NULL);

	zend_object *new_object = old_uri_object->std.handlers->clone_obj(&old_uri_object->std);
	if (new_object == NULL) {
		RETURN_THROWS();
	}

	/* Assign the object early. The engine will take care of destruction in
	 * case of an exception being thrown. */
	RETVAL_OBJ(new_object);

	php_uri_object *new_uri_object = php_uri_object_from_obj(new_object);
	ZEND_ASSERT(new_uri_object->uri != NULL);

	if (UNEXPECTED(php_uri_parser_rfc3986_userinfo_write(new_uri_object->uri, &zv, NULL) == FAILURE)) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Rfc3986_Uri, getUsername)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_USERNAME, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawUsername)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_USERNAME, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPassword)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PASSWORD, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPassword)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PASSWORD, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, getHost)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_HOST, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawHost)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_HOST, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withHost)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_HOST);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPort)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PORT, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withPort)
{
	php_uri_property_write_long_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PORT);
}

PHP_METHOD(Uri_Rfc3986_Uri, getPath)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PATH, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawPath)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PATH, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withPath)
{
	php_uri_property_write_str_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PATH);
}

PHP_METHOD(Uri_Rfc3986_Uri, getQuery)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_QUERY, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawQuery)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_QUERY, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withQuery)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_QUERY);
}

PHP_METHOD(Uri_Rfc3986_Uri, getFragment)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_FRAGMENT, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_Rfc3986_Uri, getRawFragment)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_FRAGMENT, PHP_URI_COMPONENT_READ_MODE_RAW);
}

PHP_METHOD(Uri_Rfc3986_Uri, withFragment)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_FRAGMENT);
}

static void throw_cannot_recompose_uri_to_string(php_uri_object *object)
{
	zend_throw_exception_ex(php_uri_ce_error, 0, "Cannot recompose %s to a string", ZSTR_VAL(object->std.ce->name));
}

static void uri_equals(INTERNAL_FUNCTION_PARAMETERS, php_uri_object *that_object, zend_object *comparison_mode)
{
	php_uri_object *this_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(this_object->uri != NULL);
	ZEND_ASSERT(that_object->uri != NULL);

	if (this_object->std.ce != that_object->std.ce &&
		!instanceof_function(this_object->std.ce, that_object->std.ce) &&
		!instanceof_function(that_object->std.ce, this_object->std.ce)
	) {
		RETURN_FALSE;
	}

	bool exclude_fragment = true;
	if (comparison_mode) {
		zval *case_name = zend_enum_fetch_case_name(comparison_mode);
		exclude_fragment = zend_string_equals_literal(Z_STR_P(case_name), "ExcludeFragment");
	}

	zend_string *this_str = this_object->parser->to_string(
		this_object->uri, PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII, exclude_fragment);
	if (this_str == NULL) {
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}

	zend_string *that_str = that_object->parser->to_string(
		that_object->uri, PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII, exclude_fragment);
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
		Z_PARAM_OBJ_OF_CLASS(that_object, php_uri_ce_rfc3986_uri)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(comparison_mode, php_uri_ce_comparison_mode)
	ZEND_PARSE_PARAMETERS_END();

	uri_equals(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_uri_object_from_obj(that_object), comparison_mode);
}

PHP_METHOD(Uri_Rfc3986_Uri, toRawString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uri_object->uri != NULL);

	zend_string *uri_str = uri_object->parser->to_string(uri_object->uri, PHP_URI_RECOMPOSITION_MODE_RAW_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(uri_object);
		RETURN_THROWS();
	}

	RETURN_STR(uri_str);
}

PHP_METHOD(Uri_Rfc3986_Uri, toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uri_object->uri != NULL);

	zend_string *uri_str = uri_object->parser->to_string(uri_object->uri, PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(uri_object);
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

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		uri_str, Z_URI_OBJECT_P(ZEND_THIS), true, false, NULL);
}

PHP_METHOD(Uri_Rfc3986_Uri, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uri_object->uri != NULL);

	/* Serialize state: "uri" key in the first array */
	zend_string *uri_str = uri_object->parser->to_string(uri_object->uri, PHP_URI_RECOMPOSITION_MODE_RAW_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(uri_object);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_STR(&tmp, uri_str);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), PHP_URI_SERIALIZE_URI_FIELD_NAME, sizeof(PHP_URI_SERIALIZE_URI_FIELD_NAME) - 1, &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_EMPTY_ARRAY(&arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

static void uri_unserialize(INTERNAL_FUNCTION_PARAMETERS)
{
	HashTable *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	php_uri_object *uri_object = php_uri_object_from_obj(Z_OBJ_P(ZEND_THIS));
	if (uri_object->uri != NULL) {
		/* Intentionally throw two exceptions for proper chaining. */
		zend_throw_error(NULL, "Cannot modify readonly object of class %s", ZSTR_VAL(uri_object->std.ce->name));
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Unserialize state: "uri" key in the first array */
	zval *arr = zend_hash_index_find(data, 0);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify the expected number of elements inside the first array, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) != 1) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	zval *uri_zv = zend_hash_str_find(Z_ARRVAL_P(arr), ZEND_STRL(PHP_URI_SERIALIZE_URI_FIELD_NAME));
	if (uri_zv == NULL || Z_TYPE_P(uri_zv) != IS_STRING) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	uri_object->uri = uri_object->parser->parse(Z_STRVAL_P(uri_zv), Z_STRLEN_P(uri_zv), NULL, NULL, true);
	if (uri_object->uri == NULL) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Unserialize regular properties: second array */
	arr = zend_hash_index_find(data, 1);
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}

	/* Verify that there is no regular property in the second array, because the URI classes have no properties and they are final. */
	if (zend_hash_num_elements(Z_ARRVAL_P(arr)) > 0) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(uri_object->std.ce->name));
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Rfc3986_Uri, __unserialize)
{
	uri_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Uri_Rfc3986_Uri, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);

	RETURN_ARR(uri_get_debug_properties(uri_object));
}

PHP_METHOD(Uri_WhatWg_Url, getScheme)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_SCHEME, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, withScheme)
{
	php_uri_property_write_str_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_SCHEME);
}

PHP_METHOD(Uri_WhatWg_Url, withUsername)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_USERNAME);
}

PHP_METHOD(Uri_WhatWg_Url, withPassword)
{
	php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_PASSWORD);
}

PHP_METHOD(Uri_WhatWg_Url, getAsciiHost)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_HOST, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII);
}

PHP_METHOD(Uri_WhatWg_Url, getUnicodeHost)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_HOST, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_UNICODE);
}

PHP_METHOD(Uri_WhatWg_Url, getFragment)
{
	php_uri_property_read_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_URI_PROPERTY_NAME_FRAGMENT, PHP_URI_COMPONENT_READ_MODE_NORMALIZED_UNICODE);
}

PHP_METHOD(Uri_WhatWg_Url, equals)
{
	zend_object *that_object;
	zend_object *comparison_mode = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(that_object, php_uri_ce_whatwg_url)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(comparison_mode, php_uri_ce_comparison_mode)
	ZEND_PARSE_PARAMETERS_END();

	uri_equals(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_uri_object_from_obj(that_object), comparison_mode);
}

PHP_METHOD(Uri_WhatWg_Url, toUnicodeString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	php_uri_object *uri_object = php_uri_object_from_obj(this_object);
	ZEND_ASSERT(uri_object->uri != NULL);

	RETURN_STR(uri_object->parser->to_string(uri_object->uri, PHP_URI_RECOMPOSITION_MODE_RAW_UNICODE, false));
}

PHP_METHOD(Uri_WhatWg_Url, toAsciiString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *this_object = Z_OBJ_P(ZEND_THIS);
	php_uri_object *uri_object = php_uri_object_from_obj(this_object);
	ZEND_ASSERT(uri_object->uri != NULL);

	RETURN_STR(uri_object->parser->to_string(uri_object->uri, PHP_URI_RECOMPOSITION_MODE_RAW_ASCII, false));
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

	php_uri_instantiate_uri(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		uri_str, Z_URI_OBJECT_P(ZEND_THIS), true, false, errors);
}

PHP_METHOD(Uri_WhatWg_Url, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *this_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(this_object->uri != NULL);

	/* Serialize state: "uri" key in the first array */
	zend_string *uri_str = this_object->parser->to_string(this_object->uri, PHP_URI_RECOMPOSITION_MODE_RAW_ASCII, false);
	if (uri_str == NULL) {
		throw_cannot_recompose_uri_to_string(this_object);
		RETURN_THROWS();
	}
	zval tmp;
	ZVAL_STR(&tmp, uri_str);

	array_init(return_value);

	zval arr;
	array_init(&arr);
	zend_hash_str_add_new(Z_ARRVAL(arr), ZEND_STRL(PHP_URI_SERIALIZE_URI_FIELD_NAME), &tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);

	/* Serialize regular properties: second array */
	ZVAL_EMPTY_ARRAY(&arr);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &arr);
}

PHP_METHOD(Uri_WhatWg_Url, __unserialize)
{
	uri_unserialize(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Uri_WhatWg_Url, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);

	RETURN_ARR(uri_get_debug_properties(uri_object));
}

PHPAPI php_uri_object *php_uri_object_create(zend_class_entry *class_type, const php_uri_parser *parser)
{
	php_uri_object *uri_object = zend_object_alloc(sizeof(*uri_object), class_type);

	zend_object_std_init(&uri_object->std, class_type);
	object_properties_init(&uri_object->std, class_type);

	uri_object->parser = parser;
	uri_object->uri = NULL;

	return uri_object;
}

static zend_object *php_uri_object_create_rfc3986(zend_class_entry *ce)
{
	return &php_uri_object_create(ce, &php_uri_parser_rfc3986)->std;
}

static zend_object *php_uri_object_create_whatwg(zend_class_entry *ce)
{
	return &php_uri_object_create(ce, &php_uri_parser_whatwg)->std;
}

PHPAPI void php_uri_object_handler_free(zend_object *object)
{
	php_uri_object *uri_object = php_uri_object_from_obj(object);

	uri_object->parser->destroy(uri_object->uri);
	zend_object_std_dtor(&uri_object->std);
}

PHPAPI zend_object *php_uri_object_handler_clone(zend_object *object)
{
	php_uri_object *uri_object = php_uri_object_from_obj(object);

	ZEND_ASSERT(uri_object->uri != NULL);

	php_uri_object *new_uri_object = php_uri_object_from_obj(object->ce->create_object(object->ce));
	ZEND_ASSERT(new_uri_object->parser == uri_object->parser);

	void *uri = uri_object->parser->clone(uri_object->uri);
	ZEND_ASSERT(uri != NULL);

	new_uri_object->uri = uri;

	zend_objects_clone_members(&new_uri_object->std, &uri_object->std);

	return &new_uri_object->std;
}

PHPAPI zend_result php_uri_parser_register(const php_uri_parser *uri_parser)
{
	zend_string *key = zend_string_init_interned(uri_parser->name, strlen(uri_parser->name), true);

	ZEND_ASSERT(uri_parser->name != NULL);
	ZEND_ASSERT(uri_parser->parse != NULL);
	ZEND_ASSERT(uri_parser->clone != NULL || strcmp(uri_parser->name, PHP_URI_PARSER_PHP_PARSE_URL) == 0);
	ZEND_ASSERT(uri_parser->to_string != NULL || strcmp(uri_parser->name, PHP_URI_PARSER_PHP_PARSE_URL) == 0);
	ZEND_ASSERT(uri_parser->destroy != NULL);

	zend_result result = zend_hash_add_ptr(&uri_parsers, key, (void *) uri_parser) != NULL ? SUCCESS : FAILURE;

	zend_string_release_ex(key, true);

	return result;
}

static PHP_MINIT_FUNCTION(uri)
{
	php_uri_ce_rfc3986_uri = register_class_Uri_Rfc3986_Uri();
	php_uri_ce_rfc3986_uri->create_object = php_uri_object_create_rfc3986;
	php_uri_ce_rfc3986_uri->default_object_handlers = &object_handlers_rfc3986_uri;
	memcpy(&object_handlers_rfc3986_uri, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	object_handlers_rfc3986_uri.offset = XtOffsetOf(php_uri_object, std);
	object_handlers_rfc3986_uri.free_obj = php_uri_object_handler_free;
	object_handlers_rfc3986_uri.clone_obj = php_uri_object_handler_clone;

	php_uri_ce_whatwg_url = register_class_Uri_WhatWg_Url();
	php_uri_ce_whatwg_url->create_object = php_uri_object_create_whatwg;
	php_uri_ce_whatwg_url->default_object_handlers = &object_handlers_whatwg_uri;
	memcpy(&object_handlers_whatwg_uri, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	object_handlers_whatwg_uri.offset = XtOffsetOf(php_uri_object, std);
	object_handlers_whatwg_uri.free_obj = php_uri_object_handler_free;
	object_handlers_whatwg_uri.clone_obj = php_uri_object_handler_clone;

	php_uri_ce_comparison_mode = register_class_Uri_UriComparisonMode();
	php_uri_ce_exception = register_class_Uri_UriException(zend_ce_exception);
	php_uri_ce_error = register_class_Uri_UriError(zend_ce_error);
	php_uri_ce_invalid_uri_exception = register_class_Uri_InvalidUriException(php_uri_ce_exception);
	php_uri_ce_whatwg_invalid_url_exception = register_class_Uri_WhatWg_InvalidUrlException(php_uri_ce_invalid_uri_exception);
	php_uri_ce_whatwg_url_validation_error = register_class_Uri_WhatWg_UrlValidationError();
	php_uri_ce_whatwg_url_validation_error_type = register_class_Uri_WhatWg_UrlValidationErrorType();

	zend_hash_init(&uri_parsers, 4, NULL, NULL, true);

	if (php_uri_parser_register(&php_uri_parser_rfc3986) == FAILURE) {
		return FAILURE;
	}

	if (php_uri_parser_register(&php_uri_parser_whatwg) == FAILURE) {
		return FAILURE;
	}

	if (php_uri_parser_register(&php_uri_parser_php_parse_url) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(uri)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "URI support", "active");
#ifdef URI_STATIC_BUILD
	php_info_print_table_row(2, "uriparser bundled version", URI_VER_ANSI);
#else
	php_info_print_table_row(2, "uriparser compiled version", URI_VER_ANSI);
	php_info_print_table_row(2, "uriparser loaded version", uriBaseRuntimeVersionA());
#endif
	php_info_print_table_end();
}

static PHP_MSHUTDOWN_FUNCTION(uri)
{
	zend_hash_destroy(&uri_parsers);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri)
{
	if (PHP_RINIT(uri_parser_whatwg)(INIT_FUNC_ARGS_PASSTHRU) == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

ZEND_MODULE_POST_ZEND_DEACTIVATE_D(uri)
{
	if (ZEND_MODULE_POST_ZEND_DEACTIVATE_N(uri_parser_whatwg)() == FAILURE) {
		return FAILURE;
	}

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
	NULL,                           /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(uri),                 /* PHP_MINFO - Module info */
	PHP_VERSION,                    /* Version */
	NO_MODULE_GLOBALS,
	ZEND_MODULE_POST_ZEND_DEACTIVATE_N(uri),
	STANDARD_MODULE_PROPERTIES_EX
};
