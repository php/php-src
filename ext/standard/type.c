/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_incomplete_class.h"

/* {{{ proto string gettype(mixed var)
   Returns the type of the variable */
PHP_FUNCTION(gettype)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			RETVAL_STRING("NULL", 1);
			break;

		case IS_BOOL:
			RETVAL_STRING("boolean", 1);
			break;

		case IS_LONG:
			RETVAL_STRING("integer", 1);
			break;

		case IS_DOUBLE:
			RETVAL_STRING("double", 1);
			break;
	
		case IS_STRING:
			RETVAL_STRING("string", 1);
			break;
	
		case IS_ARRAY:
			RETVAL_STRING("array", 1);
			break;

		case IS_OBJECT:
			RETVAL_STRING("object", 1);
		/*
		   {
		   char *result;
		   int res_len;

		   res_len = sizeof("object of type ")-1 + Z_OBJCE_P(arg)->name_length;
		   spprintf(&result, 0, "object of type %s", Z_OBJCE_P(arg)->name);
		   RETVAL_STRINGL(result, res_len, 0);
		   }
		 */
			break;

		case IS_RESOURCE:
			{
				const char *type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(arg) TSRMLS_CC);

				if (type_name) {
					RETVAL_STRING("resource", 1);
					break;
				}
			}

		default:
			RETVAL_STRING("unknown type", 1);
	}
}
/* }}} */

/* {{{ proto bool settype(mixed var, string type)
   Set the type of the variable */
PHP_FUNCTION(settype)
{
	zval **var;
	char *type;
	int type_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs", &var, &type, &type_len) == FAILURE) {
		return;
	}

	if (!strcasecmp(type, "integer")) {
		convert_to_long(*var);
	} else if (!strcasecmp(type, "int")) {
		convert_to_long(*var);
	} else if (!strcasecmp(type, "float")) {
		convert_to_double(*var);
	} else if (!strcasecmp(type, "double")) { /* deprecated */
		convert_to_double(*var);
	} else if (!strcasecmp(type, "string")) {
		convert_to_string(*var);
	} else if (!strcasecmp(type, "array")) {
		convert_to_array(*var);
	} else if (!strcasecmp(type, "object")) {
		convert_to_object(*var);
	} else if (!strcasecmp(type, "bool")) {
		convert_to_boolean(*var);
	} else if (!strcasecmp(type, "boolean")) {
		convert_to_boolean(*var);
	} else if (!strcasecmp(type, "null")) {
		convert_to_null(*var);
	} else if (!strcasecmp(type, "resource")) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot convert to resource type");
		RETURN_FALSE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type");
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int intval(mixed var [, int base])
   Get the integer value of a variable using the optional base for the conversion */
PHP_FUNCTION(intval)
{
	zval **num;
	long arg_base;
	int base;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &num) == FAILURE) {
				return;
			}
			base = 10;
			break;

		case 2:
			if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl", &num, &arg_base) == FAILURE) {
				return;
			}
			base = arg_base;
			break;

		default:
			WRONG_PARAM_COUNT;
	}

	RETVAL_ZVAL(*num, 1, 0);
	convert_to_long_base(return_value, base);
}
/* }}} */

/* {{{ proto float floatval(mixed var)
   Get the float value of a variable */
PHP_FUNCTION(floatval)
{
	zval **num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &num) == FAILURE) {
		return;
	}

	RETVAL_ZVAL(*num, 1, 0);
	convert_to_double(return_value);
}
/* }}} */

/* {{{ proto string strval(mixed var)
   Get the string value of a variable */
PHP_FUNCTION(strval)
{
	zval **num, *tmp;
	zval expr_copy;
	int use_copy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &num) == FAILURE) {
		return;
	}

	zend_make_printable_zval(*num, &expr_copy, &use_copy);
	if (use_copy) {
		tmp = &expr_copy;
		RETVAL_ZVAL(tmp, 0, 0);
	} else {
		RETVAL_ZVAL(*num, 1, 0);
	}
}
/* }}} */

static void php_is_type(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(arg) == type) {
		if (type == IS_OBJECT) {
			zend_class_entry *ce;
			if(Z_OBJ_HT_PP(arg)->get_class_entry == NULL) {
			/* if there's no get_class_entry it's not a PHP object, so it can't be INCOMPLETE_CLASS */
				RETURN_TRUE;
			}
			ce = Z_OBJCE_PP(arg);
			if (!strcmp(ce->name, INCOMPLETE_CLASS)) {
				RETURN_FALSE;
			}
		}
		if (type == IS_RESOURCE) {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(arg) TSRMLS_CC);
			if (!type_name) {
				RETURN_FALSE;
			}
		}
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


/* {{{ proto bool is_null(mixed var)
   Returns true if variable is null */
PHP_FUNCTION(is_null)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_NULL);
}
/* }}} */

/* {{{ proto bool is_resource(mixed var)
   Returns true if variable is a resource */
PHP_FUNCTION(is_resource)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_RESOURCE);
}
/* }}} */

/* {{{ proto bool is_bool(mixed var)
   Returns true if variable is a boolean */
PHP_FUNCTION(is_bool)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_BOOL);
}
/* }}} */

/* {{{ proto bool is_long(mixed var)
   Returns true if variable is a long (integer) */
PHP_FUNCTION(is_long)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG);
}
/* }}} */

/* {{{ proto bool is_float(mixed var)
   Returns true if variable is float point*/
PHP_FUNCTION(is_float)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE);
}
/* }}} */

/* {{{ proto bool is_string(mixed var)
   Returns true if variable is a string */
PHP_FUNCTION(is_string)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING);
}
/* }}} */

/* {{{ proto bool is_array(mixed var)
   Returns true if variable is an array */
PHP_FUNCTION(is_array)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY);
}
/* }}} */

/* {{{ proto bool is_object(mixed var)
   Returns true if variable is an object */
PHP_FUNCTION(is_object)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT);
}
/* }}} */

/* {{{ proto bool is_numeric(mixed value)
   Returns true if value is a number or a numeric string */
PHP_FUNCTION(is_numeric)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}

	switch (Z_TYPE_PP(arg)) {
		case IS_LONG:
		case IS_DOUBLE:
			RETURN_TRUE;
			break;

		case IS_STRING:
			if (is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), NULL, NULL, 0)) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool is_scalar(mixed value)
   Returns true if value is a scalar */
PHP_FUNCTION(is_scalar)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}

	switch (Z_TYPE_PP(arg)) {
		case IS_BOOL:
		case IS_DOUBLE:
		case IS_LONG:
		case IS_STRING:
			RETURN_TRUE;
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool is_callable(mixed var [, bool syntax_only [, string callable_name]]) 
   Returns true if var is callable. */
PHP_FUNCTION(is_callable)
{
	zval *var, **callable_name = NULL;
	char *name;
	char *error;
	zend_bool retval;
	zend_bool syntax_only = 0;
	int check_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|bZ", &var,
							  &syntax_only, &callable_name) == FAILURE) {
		return;
	}
	
	if (syntax_only) {
		check_flags |= IS_CALLABLE_CHECK_SYNTAX_ONLY;
	}
	if (ZEND_NUM_ARGS() > 2) {
		retval = zend_is_callable_ex(var, NULL, check_flags, &name, NULL, NULL, &error TSRMLS_CC);
		zval_dtor(*callable_name);
		ZVAL_STRING(*callable_name, name, 0);
	} else {
		retval = zend_is_callable_ex(var, NULL, check_flags, NULL, NULL, NULL, &error TSRMLS_CC);
	}
	if (error) {
		/* ignore errors */
		efree(error);
	}

	RETURN_BOOL(retval);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
