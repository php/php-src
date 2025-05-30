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

void uri_register_property_handler(HashTable *property_handlers, zend_string *name, const uri_property_handler_t *handler)
{
	zend_hash_add_new_ptr(property_handlers, name, (void *) handler);
}

uri_property_handler_t *uri_property_handler_from_internal_uri(const uri_internal_t *internal_uri, zend_string *name)
{
	return zend_hash_find_ptr(internal_uri->handler->property_handlers, name);
}

void throw_invalid_uri_exception(const uri_handler_t *uri_handler, zval *errors)
{
	zval exception_zv;

	uri_handler->create_invalid_uri_exception(&exception_zv, errors);

	zend_throw_exception_object(&exception_zv);
}

void uri_read_component(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name, uri_component_read_mode_t component_read_mode)
{
	ZEND_PARSE_PARAMETERS_NONE();

	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS);
	URI_ASSERT_INITIALIZATION(internal_uri);

	const uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, property_name);
	ZEND_ASSERT(property_handler != NULL);

	if (UNEXPECTED(property_handler->read_func(internal_uri, component_read_mode, return_value) == FAILURE)) {
		zend_throw_error(NULL, "%s::$%s property cannot be retrieved", ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name));
		RETURN_THROWS();
	}
}

static void uri_write_component_ex(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name, zval *property_zv)
{
	uri_internal_t *internal_uri = Z_URI_INTERNAL_P(ZEND_THIS);
	URI_ASSERT_INITIALIZATION(internal_uri);

	const uri_property_handler_t *property_handler = uri_property_handler_from_internal_uri(internal_uri, property_name);
	ZEND_ASSERT(property_handler != NULL);

	zend_object *new_object = uri_clone_obj_handler(Z_OBJ_P(ZEND_THIS));
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_object_release(new_object);
		zval_ptr_dtor(property_zv);
		RETURN_THROWS();
	}

	uri_internal_t *new_internal_uri = uri_internal_from_obj(new_object);
	URI_ASSERT_INITIALIZATION(new_internal_uri);
	if (property_handler->write_func == NULL) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(Z_OBJ_P(ZEND_THIS)->ce->name), ZSTR_VAL(property_name));
		zend_object_release(new_object);
		zval_ptr_dtor(property_zv);
		RETURN_THROWS();
	}

	zval errors;
	ZVAL_UNDEF(&errors);
	if (property_handler->write_func(new_internal_uri, property_zv, &errors) == FAILURE) {
		throw_invalid_uri_exception(new_internal_uri->handler, &errors);
		zval_ptr_dtor(&errors);
		zend_object_release(new_object);
		zval_ptr_dtor(property_zv);
		RETURN_THROWS();
	}

	ZEND_ASSERT(Z_ISUNDEF(errors));
	RETVAL_OBJ(new_object);
	zval_ptr_dtor(property_zv);
}

void uri_write_component_str(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name)
{
	zend_string *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	zval zv;
	ZVAL_STR_COPY(&zv, value);

	uri_write_component_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void uri_write_component_str_or_null(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name)
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

	uri_write_component_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, property_name, &zv);
}

void uri_write_component_long_or_null(INTERNAL_FUNCTION_PARAMETERS, zend_string *property_name)
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
