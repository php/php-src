/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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

/* {{{ proto string gettype(mixed var) U
   Returns the type of the variable */
PHP_FUNCTION(gettype)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			RETVAL_ASCII_STRING("NULL", 1);
			break;

		case IS_BOOL:
			RETVAL_ASCII_STRING("boolean", 1);
			break;

		case IS_LONG:
			RETVAL_ASCII_STRING("integer", 1);
			break;

		case IS_DOUBLE:
			RETVAL_ASCII_STRING("double", 1);
			break;
	
		case IS_STRING:
			RETVAL_ASCII_STRING("string", 1);
			break;
	
		case IS_UNICODE:
			RETVAL_ASCII_STRING("unicode", 1);
			break;

		case IS_ARRAY:
			RETVAL_ASCII_STRING("array", 1);
			break;

		case IS_OBJECT:
			RETVAL_ASCII_STRING("object", 1);
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
				char *type_name;
				type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(arg) TSRMLS_CC);
				if (type_name) {
					RETVAL_ASCII_STRING("resource", 1);
					break;
				}
			}

		default:
			RETVAL_ASCII_STRING("unknown type", 1);
	}
}
/* }}} */

/* {{{ proto bool settype(mixed var, string type) U
   Set the type of the variable */
PHP_FUNCTION(settype)
{
	zval **var;
	char *new_type;
	int new_type_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs", &var, &new_type, &new_type_len) == FAILURE) {
		return;
	}

	if (!strcasecmp(new_type, "integer")) {
		convert_to_long(*var);
	} else if (!strcasecmp(new_type, "int")) {
		convert_to_long(*var);
	} else if (!strcasecmp(new_type, "float")) {
		convert_to_double(*var);
	} else if (!strcasecmp(new_type, "double")) { /* deprecated */
		convert_to_double(*var);
	} else if (!strcasecmp(new_type, "binary")) { /* explicit binary cast */
		convert_to_string(*var);
	} else if (!strcasecmp(new_type, "string")) { /* runtime string type */
		if (UG(unicode)) {
			convert_to_unicode(*var);
		} else {
			convert_to_string(*var);
		}
	} else if (!strcasecmp(new_type, "unicode")) { /* explicit unicode cast */
		convert_to_unicode(*var);
	} else if (!strcasecmp(new_type, "array")) {
		convert_to_array(*var);
	} else if (!strcasecmp(new_type, "object")) {
		convert_to_object(*var);
	} else if (!strcasecmp(new_type, "bool")) {
		convert_to_boolean(*var);
	} else if (!strcasecmp(new_type, "boolean")) {
		convert_to_boolean(*var);
	} else if (!strcasecmp(new_type, "null")) {
		convert_to_null(*var);
	} else if (!strcasecmp(new_type, "resource")) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot convert to resource type");
		RETURN_FALSE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid type");
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int intval(mixed var [, int base]) U
   Get the integer value of a variable using the optional base for the conversion */
PHP_FUNCTION(intval)
{
	zval **num;
	long arg_base = 0;
	int base = 10;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|l", &num, &arg_base) == FAILURE) {
		return;
	}
	
	if (ZEND_NUM_ARGS() == 2) {
		base = arg_base;
	}

	RETVAL_ZVAL(*num, 1, 0);
	convert_to_long_base(return_value, base);
}
/* }}} */

/* {{{ proto float floatval(mixed var) U
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

/* {{{ proto string strval(mixed var) U
   Get the string value of a variable */
PHP_FUNCTION(strval)
{
	zval **num, *tmp;
	zval expr_copy;
	int use_copy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &num) == FAILURE) {
		return;
	}

	if (UG(unicode)) {
		zend_make_unicode_zval(*num, &expr_copy, &use_copy);
	} else {
		zend_make_string_zval(*num, &expr_copy, &use_copy);
	}
	if (use_copy) {
		tmp = &expr_copy;
		RETVAL_ZVAL(tmp, 0, 0);
	} else {
		RETVAL_ZVAL(*num, 1, 0);
	}
}
/* }}} */

static void php_is_type(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(arg) == type) {
		if (type == IS_OBJECT) {
			zend_class_entry *ce;

			if(Z_OBJ_HT_P(arg)->get_class_entry == NULL) {
				/* if there's no get_class_entry it's not a PHP object, so it can't be INCOMPLETE_CLASS */
				RETURN_TRUE;
			}
			ce = Z_OBJCE_P(arg);

			if (ce->name_length != sizeof(INCOMPLETE_CLASS) - 1) {
				/* We can get away with this because INCOMPLETE_CLASS is ascii and has a 1:1 relationship with unicode */
				RETURN_TRUE;
			} else if (UG(unicode)) {
#ifndef PHP_WIN32
				U_STRING_DECL(uIncompleteClass, (INCOMPLETE_CLASS), sizeof(INCOMPLETE_CLASS) - 1);
				U_STRING_INIT(uIncompleteClass, (INCOMPLETE_CLASS), sizeof(INCOMPLETE_CLASS) - 1);

				if (!memcmp(ce->name.u, uIncompleteClass, UBYTES(sizeof(INCOMPLETE_CLASS)))) {
					RETURN_FALSE;
				}
#else /* WIN32 -- U_STRING_DECL breaks under Win32 with string macros */
				char *ascii_name = zend_unicode_to_ascii(ce->name.u, ce->name_length TSRMLS_CC);

				if (ascii_name) {
					if (memcmp(INCOMPLETE_CLASS, ascii_name, sizeof(INCOMPLETE_CLASS) - 1) == 0) {
						efree(ascii_name);
						RETURN_FALSE;
					}
					efree(ascii_name);
				}
				/* Non-ascii class name means it can't be INCOMPLETE_CLASS and is therefore okay */
#endif
			} else {
				if (!memcmp(ce->name.s, INCOMPLETE_CLASS, sizeof(INCOMPLETE_CLASS))) {
					RETURN_FALSE;
				}
			}
		}
		if (type == IS_RESOURCE) {
			char *type_name;
			type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_P(arg) TSRMLS_CC);
			if (!type_name) {
				RETURN_FALSE;
			}
		}
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool is_null(mixed var) U
   Returns true if variable is null */
PHP_FUNCTION(is_null)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_NULL);
}
/* }}} */

/* {{{ proto bool is_resource(mixed var) U
   Returns true if variable is a resource */
PHP_FUNCTION(is_resource)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_RESOURCE);
}
/* }}} */

/* {{{ proto bool is_bool(mixed var) U
   Returns true if variable is a boolean */
PHP_FUNCTION(is_bool)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_BOOL);
}
/* }}} */

/* {{{ proto bool is_long(mixed var) U
   Returns true if variable is a long (integer) */
PHP_FUNCTION(is_long)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG);
}
/* }}} */

/* {{{ proto bool is_float(mixed var) U
   Returns true if variable is float point*/
PHP_FUNCTION(is_float)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE);
}
/* }}} */

/* {{{ proto bool is_binary(mixed var) U
   Returns true if variable is a native (binary) string */
PHP_FUNCTION(is_binary)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING);
}
/* }}} */

/* {{{ proto bool is_string(mixed var) U
   Returns true if variable is a Unicode or binary string */
PHP_FUNCTION(is_string)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(arg) == IS_UNICODE || Z_TYPE_P(arg) == IS_STRING) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool is_unicode(mixed var) U
   Returns true if variable is a unicode string */
PHP_FUNCTION(is_unicode)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_UNICODE);
}
/* }}} */

/* {{{ proto bool is_buffer(mixed var) U
   Returns true if variable is a native, unicode or binary string */
PHP_FUNCTION(is_buffer)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(arg) == IS_STRING || Z_TYPE_P(arg) == IS_UNICODE) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool is_array(mixed var) U
   Returns true if variable is an array */
PHP_FUNCTION(is_array)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY);
}
/* }}} */

/* {{{ proto bool is_object(mixed var) U
   Returns true if variable is an object */
PHP_FUNCTION(is_object)
{
	php_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT);
}
/* }}} */

/* {{{ proto bool is_numeric(mixed value) U
   Returns true if value is a number or a numeric string */
PHP_FUNCTION(is_numeric)
{
	zval **arg;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}

	switch (Z_TYPE_PP(arg)) {
		case IS_LONG:
		case IS_DOUBLE:
			RETURN_TRUE;
			break;

		case IS_STRING:
			result = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), NULL, NULL, 0);
			if (result == IS_LONG || result == IS_DOUBLE) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		case IS_UNICODE:
			result = is_numeric_unicode(Z_USTRVAL_PP(arg), Z_USTRLEN_PP(arg), NULL, NULL, 0);
			if (result == IS_LONG || result == IS_DOUBLE) {
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

/* {{{ proto bool is_scalar(mixed value) U
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
		case IS_UNICODE:
			RETURN_TRUE;
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool is_callable(mixed var [, bool syntax_only [, string callable_name]])  U
   Returns true if var is callable. */
PHP_FUNCTION(is_callable)
{
	zval *var, **callable_name = NULL;
	zval name;
	zend_bool retval;
	zend_bool syntax_only = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|bZ", &var,
							  &syntax_only, &callable_name) == FAILURE) {
		return;
	}

	syntax_only = syntax_only ? IS_CALLABLE_CHECK_SYNTAX_ONLY : 0;
	if (ZEND_NUM_ARGS() > 2) {
		retval = zend_is_callable(var, syntax_only, &name TSRMLS_CC);
		REPLACE_ZVAL_VALUE(callable_name, &name, 0);
	} else {
		retval = zend_is_callable(var, syntax_only, NULL TSRMLS_CC);
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
