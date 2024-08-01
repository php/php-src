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

#include "php_uri.h"
#include "php_uri_common.h"
#include "php_lexbor.h"
#include "php_uriparser.h"
#include "php_uri_arginfo.h"

zend_class_entry *uri_ce;
zend_class_entry *whatwg_error_ce;
ZEND_DECLARE_MODULE_GLOBALS(uri)

static inline zend_result uri_object_read_property(zend_object *object, zend_string *property_name, zval *value)
{
	ZVAL_COPY_VALUE(value, zend_read_property_ex(object->ce, object, property_name, false, NULL));
	if (EG(exception)) {
		return FAILURE;
	}

	return SUCCESS;
}

void uri_update_str_property(const zval *object, zend_string *name, zend_string *value)
{
	zval zv;

	if (value) {
		ZVAL_STR(&zv, value);
	} else {
		ZVAL_NULL(&zv);
	}

	zend_update_property_ex(uri_ce, Z_OBJ_P(object), name, &zv);
}

void uri_update_long_property(const zval *object, zend_string *name, zend_long value, bool value_is_null)
{
	zval zv;

	if (!value_is_null) {
		ZVAL_LONG(&zv, value);
	} else {
		ZVAL_NULL(&zv);
	}

	zend_update_property_ex(uri_ce, Z_OBJ_P(object), name, &zv);
}

#define UPDATE_STR_PROPERTY(o, n, v) \
	do { \
		uri_update_str_property(o, n, v); \
		if (EG(exception)) { \
			RETURN_THROWS(); \
		} \
	} while(0)

#define UPDATE_LONG_PROPERTY(o, n, v, vn) \
	do { \
		uri_update_long_property(o, n, v, vn); \
		if (EG(exception)) { \
			RETURN_THROWS(); \
		} \
	} while(0)


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

PHP_METHOD(Uri_Uri, fromRfc3986)
{
	zend_string *uri, *base_url = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH_STR(uri)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(base_url)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(uri) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (base_url && ZSTR_LEN(base_url) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	uriparser_parse_uri(uri, base_url, return_value);
}

PHP_METHOD(Uri_Uri, fromWhatWg)
{
	zend_string *uri, *base_url = NULL;
	zval *errors = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH_STR(uri)
		Z_PARAM_OPTIONAL
		Z_PARAM_PATH_STR_OR_NULL(base_url)
		Z_PARAM_ZVAL(errors)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(uri) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (base_url && ZSTR_LEN(base_url) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	lexbor_parse_url(uri, base_url, return_value, errors);
}

PHP_METHOD(Uri_Uri, __construct)
{
	zend_string *scheme = NULL, *host = NULL, *user = NULL, *password = NULL, *path = NULL, *query = NULL, *fragment = NULL;
	zend_long port;
	bool port_is_null = true;

	ZEND_PARSE_PARAMETERS_START(2, 8)
		Z_PARAM_STR_OR_NULL(scheme)
		Z_PARAM_STR_OR_NULL(host)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(port, port_is_null)
		Z_PARAM_STR_OR_NULL(user)
		Z_PARAM_STR_OR_NULL(password)
		Z_PARAM_STR_OR_NULL(path)
		Z_PARAM_STR_OR_NULL(query)
		Z_PARAM_STR_OR_NULL(fragment)
	ZEND_PARSE_PARAMETERS_END();

	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_SCHEME), scheme);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_HOST), host);
	UPDATE_LONG_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_PORT), port, port_is_null);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_USER), user);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_PASSWORD), password);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_PATH), path);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_QUERY), query);
	UPDATE_STR_PROPERTY(getThis(), ZSTR_KNOWN(ZEND_STR_FRAGMENT), fragment);
}

PHP_METHOD(Uri_Uri, getScheme)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (uri_object_read_property(Z_OBJ_P(getThis()), ZSTR_KNOWN(ZEND_STR_SCHEME), return_value) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_THROWS();
	}
}

PHP_METHOD(Uri_Uri, withScheme)
{
	zend_string *scheme;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_NULL(scheme)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, zend_get_called_scope(execute_data));

	zend_objects_clone_members(Z_OBJ_P(return_value), Z_OBJ_P(getThis()));

	zval *zv = OBJ_PROP_NUM(Z_OBJ_P(return_value), 0);
	Z_PROP_FLAG_P(zv) |= IS_PROP_REINITABLE;
	Z_PROP_FLAG_P(zv) |= IS_PROP_UNINIT;
	UPDATE_STR_PROPERTY(return_value, ZSTR_KNOWN(ZEND_STR_SCHEME), scheme);
	Z_PROP_FLAG_P(zv) &= ~IS_PROP_REINITABLE;
	Z_PROP_FLAG_P(zv) &= ~IS_PROP_UNINIT;
}

PHP_METHOD(Uri_Uri, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO Not implemented yet
	RETURN_EMPTY_STRING();
}

void uri_register_symbols(void)
{
	uri_ce = register_class_Uri_Uri(zend_ce_stringable);
	whatwg_error_ce = register_class_Uri_WhatWgError();
}

PHP_MINIT_FUNCTION(uri)
{
	uri_register_symbols();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(uri)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri)
{
#if defined(COMPILE_DL_URI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	URI_G(lexbor_parser) = lexbor_init_parser();
	URI_G(lexbor_urls) = 0;

	return URI_G(lexbor_parser) == NULL ? FAILURE : SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(uri)
{
	lexbor_destroy_parser(URI_G(lexbor_parser));

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
	NULL,                            /* PHP_MINFO - Module info */
	PHP_VERSION,                      /* Version */
	PHP_MODULE_GLOBALS(uri),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_URI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uri)
#endif
