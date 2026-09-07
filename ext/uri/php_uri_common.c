/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "php_uri_common.h"

static zend_string *get_known_string_by_property_name(php_uri_property_name property_name)
{
	switch (property_name) {
		case PHP_URI_PROPERTY_NAME_SCHEME:
			return ZSTR_KNOWN(ZEND_STR_SCHEME);
		case PHP_URI_PROPERTY_NAME_USERNAME:
			return ZSTR_KNOWN(ZEND_STR_USERNAME);
		case PHP_URI_PROPERTY_NAME_PASSWORD:
			return ZSTR_KNOWN(ZEND_STR_PASSWORD);
		case PHP_URI_PROPERTY_NAME_HOST:
			return ZSTR_KNOWN(ZEND_STR_HOST);
		case PHP_URI_PROPERTY_NAME_PORT:
			return ZSTR_KNOWN(ZEND_STR_PORT);
		case PHP_URI_PROPERTY_NAME_PATH:
			return ZSTR_KNOWN(ZEND_STR_PATH);
		case PHP_URI_PROPERTY_NAME_QUERY:
			return ZSTR_KNOWN(ZEND_STR_QUERY);
		case PHP_URI_PROPERTY_NAME_FRAGMENT:
			return ZSTR_KNOWN(ZEND_STR_FRAGMENT);
		default: ZEND_UNREACHABLE();
	}
}

zend_result php_uri_pass_errors_by_ref_and_free(zval *errors_zv, zval *errors)
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

void php_uri_property_read_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name, php_uri_component_read_mode component_read_mode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_uri_object *uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(uri_object->uri != NULL);

	const php_uri_property_handler *property_handler = php_uri_parser_property_handler_by_name(uri_object->parser, property_name);

	if (UNEXPECTED(property_handler->read(uri_object->uri, component_read_mode, return_value) == FAILURE)) {
		zend_throw_exception_ex(php_uri_ce_error, 0, "The %s component cannot be retrieved", ZSTR_VAL(get_known_string_by_property_name(property_name)));
		RETURN_THROWS();
	}
}

static void php_uri_property_write_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name, const zval *property_zv)
{
	php_uri_object *old_uri_object = Z_URI_OBJECT_P(ZEND_THIS);
	ZEND_ASSERT(old_uri_object->uri != NULL);

	zend_object *new_object = old_uri_object->std.handlers->clone_obj(&old_uri_object->std);
	if (new_object == NULL) {
		RETURN_THROWS();
	}

	/* Assign the object early. The engine will take care of destruction in
	 * case of an exception being thrown. */
	RETVAL_OBJ(new_object);

	const php_uri_property_handler *property_handler = php_uri_parser_property_handler_by_name(old_uri_object->parser, property_name);

	php_uri_object *new_uri_object = php_uri_object_from_obj(new_object);
	ZEND_ASSERT(new_uri_object->uri != NULL);
	if (UNEXPECTED(property_handler->write == NULL)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(old_uri_object->std.ce->name),
			ZSTR_VAL(get_known_string_by_property_name(property_name)));
		RETURN_THROWS();
	}

	zval errors;
	ZVAL_UNDEF(&errors);
	if (UNEXPECTED(property_handler->write(new_uri_object->uri, property_zv, &errors) == FAILURE)) {
		zval_ptr_dtor(&errors);
		RETURN_THROWS();
	}

	ZEND_ASSERT(Z_ISUNDEF(errors));
}

void php_uri_property_write_str_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
{
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	ZVAL_STR(&zv, value);

	php_uri_property_write_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void php_uri_property_write_str_or_null_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
{
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	if (value == NULL) {
		ZVAL_NULL(&zv);
	} else {
		ZVAL_STR(&zv, value);
	}

	php_uri_property_write_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void php_uri_property_write_long_or_null_helper(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
{
	zend_long value;
	bool value_is_null;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG_OR_NULL(value, value_is_null)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	if (value_is_null) {
		ZVAL_NULL(&zv);
	} else {
		ZVAL_LONG(&zv, value);
	}

	php_uri_property_write_helper(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}
