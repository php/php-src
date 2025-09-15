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
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

void uri_read_component(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name, php_uri_component_read_mode component_read_mode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS);
	URI_ASSERT_INITIALIZATION(internal_uri);

	const php_uri_property_handler *property_handler = php_uri_parser_property_handler_by_name(internal_uri->parser, property_name);

	if (UNEXPECTED(property_handler->read(internal_uri->uri, component_read_mode, return_value) == FAILURE)) {
		zend_throw_exception_ex(uri_error_ce, 0, "The %s component cannot be retrieved", ZSTR_VAL(get_known_string_by_property_name(property_name)));
		RETURN_THROWS();
	}
}

static void uri_write_component_ex(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name, zval *property_zv)
{
	zend_object *old_object = Z_OBJ_P(ZEND_THIS);
	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS);
	URI_ASSERT_INITIALIZATION(internal_uri);

	zend_object *new_object = old_object->handlers->clone_obj(old_object);
	if (new_object == NULL) {
		RETURN_THROWS();
	}

	/* Assign the object early. The engine will take care of destruction in
	 * case of an exception being thrown. */
	RETVAL_OBJ(new_object);

	const php_uri_property_handler *property_handler = php_uri_parser_property_handler_by_name(internal_uri->parser, property_name);

	uri_internal_t *new_internal_uri = uri_internal_from_obj(new_object);
	URI_ASSERT_INITIALIZATION(new_internal_uri);
	if (UNEXPECTED(property_handler->write == NULL)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(old_object->ce->name),
			ZSTR_VAL(get_known_string_by_property_name(property_name)));
		RETURN_THROWS();
	}

	zval errors;
	ZVAL_UNDEF(&errors);
	if (UNEXPECTED(property_handler->write(new_internal_uri->uri, property_zv, &errors) == FAILURE)) {
		zval_ptr_dtor(&errors);
		RETURN_THROWS();
	}

	ZEND_ASSERT(Z_ISUNDEF(errors));
}

void uri_write_component_str(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
{
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	ZVAL_STR(&zv, value);

	uri_write_component_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
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

	uri_write_component_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void uri_write_component_long_or_null(INTERNAL_FUNCTION_PARAMETERS, php_uri_property_name property_name)
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

	uri_write_component_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}
