/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */


/*
 * This module maps the VARIANT datastructure into PHP so that it can be used to
 * pass values to COM and DOTNET Objects by reference and not only by value.
 *
 * harald
 */

#ifdef PHP_WIN32

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_VARIANT.h"

#include <unknwn.h> 

static int do_VARIANT_propset(VARIANT *var_arg, pval *arg_property, pval *value TSRMLS_DC);
static int php_VARIANT_set_property_handler(zend_property_reference *property_reference, pval *value);
static pval php_VARIANT_get_property_handler(zend_property_reference *property_reference);
static void php_VARIANT_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
static void php_VARIANT_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void php_register_VARIANT_class(TSRMLS_D);

static int le_variant;
static int codepage;
static zend_class_entry VARIANT_class_entry;


function_entry VARIANT_functions[] = {
	{NULL, NULL, NULL}
};


static PHP_MINFO_FUNCTION(VARIANT)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "VARIANT support", "enabled");
	php_info_print_table_end();
}


zend_module_entry VARIANT_module_entry = {
	"variant", VARIANT_functions, PHP_MINIT(VARIANT), PHP_MSHUTDOWN(VARIANT), NULL, NULL, PHP_MINFO(VARIANT), STANDARD_MODULE_PROPERTIES
};


PHP_MINIT_FUNCTION(VARIANT)
{
	le_variant = zend_register_list_destructors_ex(php_VARIANT_destructor, NULL, "VARIANT", module_number);

	/* variant datatypes */
	REGISTER_LONG_CONSTANT("VT_NULL", VT_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_EMPTY", VT_EMPTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI1", VT_UI1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I2", VT_I2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I4", VT_I4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_R4", VT_R4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_R8", VT_R8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BOOL", VT_BOOL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_ERROR", VT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_CY", VT_CY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DATE", VT_CY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BSTR", VT_BSTR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DECIMAL", VT_DECIMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UNKNOWN", VT_UNKNOWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DISPATCH", VT_DISPATCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_VARIANT", VT_VARIANT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I1", VT_I1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI2", VT_UI2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI4", VT_UI4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_INT", VT_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UINT", VT_UINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_ARRAY", VT_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BYREF", VT_BYREF, CONST_CS | CONST_PERSISTENT);

	/* codepages */
	REGISTER_LONG_CONSTANT("CP_ACP", CP_ACP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_MACCP", CP_MACCP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_OEMCP", CP_OEMCP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_UTF7", CP_UTF7, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_UTF8", CP_UTF8, CONST_CS | CONST_PERSISTENT);
#ifdef CP_SYMBOL
	REGISTER_LONG_CONSTANT("CP_SYMBOL", CP_SYMBOL, CONST_CS | CONST_PERSISTENT);
#else
#	error	"CP_SYMBOL undefined"
#endif
#ifdef CP_THREAD_ACP
	REGISTER_LONG_CONSTANT("CP_THREAD_ACP", CP_THREAD_ACP, CONST_CS | CONST_PERSISTENT);
#else
#	error	"CP_THREAD_ACP undefined"
#endif

	php_register_VARIANT_class(TSRMLS_C);
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(VARIANT)
{
	return SUCCESS;
}


PHPAPI int php_VARIANT_get_le_variant()
{
	return le_variant;
}


static void php_VARIANT_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	pval *object = property_reference->object;
	zend_overloaded_element *function_name = (zend_overloaded_element *) property_reference->elements_list->tail->data;
	VARIANT *pVar;

	if ((zend_llist_count(property_reference->elements_list)==1) && !strcmp(Z_STRVAL(function_name->element), "variant")) {
		/* constructor */
		pval *object_handle, *data, *type, *code_page;

		ALLOC_VARIANT(pVar);
		VariantInit(pVar);

		switch (ZEND_NUM_ARGS()) {
			case 0:
				/* nothing to do */
				break;
			case 1:
				zend_get_parameters(ht, 1, &data);
				php_pval_to_variant(data, pVar, codepage TSRMLS_CC);
				codepage = CP_ACP;
				break;
			case 2:
				zend_get_parameters(ht, 2, &data, &type);
				php_pval_to_variant_ex(data, pVar, type, codepage TSRMLS_CC);
				codepage = CP_ACP;
				break;
			case 3:
				zend_get_parameters(ht, 3, &data, &type, &code_page);
				php_pval_to_variant_ex(data, pVar, type, codepage TSRMLS_CC);
				convert_to_long(code_page);
				codepage = code_page->value.lval;
				break;
			default:
				ZEND_WRONG_PARAM_COUNT();
				break;
		}

		RETVAL_LONG(zend_list_insert(pVar, IS_VARIANT));

		if (!zend_is_true(return_value)) {
			ZVAL_FALSE(object);
			return;
		}

		ALLOC_ZVAL(object_handle);
		*object_handle = *return_value;
		pval_copy_constructor(object_handle);
		INIT_PZVAL(object_handle);
		zend_hash_index_update(Z_OBJPROP_P(object), 0, &object_handle, sizeof(pval *), NULL);
		zval_dtor(&function_name->element);
	}
}


static pval php_VARIANT_get_property_handler(zend_property_reference *property_reference)
{
	zend_overloaded_element *overloaded_property;
	int type;
	TSRMLS_FETCH();

	pval result, **var_handle, *object = property_reference->object;
	VARIANT *var_arg;

	/* fetch the VARIANT structure */
	zend_hash_index_find(Z_OBJPROP_P(object), 0, (void **) &var_handle);
	var_arg = zend_list_find(Z_LVAL_PP(var_handle), &type);

	if (!var_arg || (type != IS_VARIANT)) {
		ZVAL_FALSE(&result);
	} else {
		overloaded_property = (zend_overloaded_element *) property_reference->elements_list->head->data;
		switch (overloaded_property->type) {
			case OE_IS_ARRAY:
				ZVAL_FALSE(&result);
				break;

			case OE_IS_OBJECT:
				if (!strcmp(overloaded_property->element.value.str.val, "value")) {
					php_variant_to_pval(var_arg, &result, 0, codepage TSRMLS_CC);
				} else if (!strcmp(Z_STRVAL(overloaded_property->element), "type")) {
					ZVAL_LONG(&result, V_VT(var_arg))
				} else {
					ZVAL_FALSE(&result);
					php_error(E_WARNING, "Unknown member.");
				}
				break;
			case OE_IS_METHOD:
				ZVAL_FALSE(&result);
				php_error(E_WARNING, "Unknown method.");
				break;

				zval_dtor(&overloaded_property->element);
		}
	}

	return result;
}


static int php_VARIANT_set_property_handler(zend_property_reference *property_reference, pval *value)
{
	zend_overloaded_element *overloaded_property;
	int type;
	TSRMLS_FETCH();

	pval **var_handle, *object = property_reference->object;
	VARIANT *var_arg;

	/* fetch the VARIANT structure */
	zend_hash_index_find(Z_OBJPROP_P(object), 0, (void **) &var_handle);
	var_arg = zend_list_find(Z_LVAL_PP(var_handle), &type);

	if (!var_arg || (type != IS_VARIANT)) {
		return FAILURE;
	}

	overloaded_property = (zend_overloaded_element *) property_reference->elements_list->head->data;
	do_VARIANT_propset(var_arg, &overloaded_property->element, value TSRMLS_CC);
	zval_dtor(&overloaded_property->element);
	return SUCCESS;
}


static int do_VARIANT_propset(VARIANT *var_arg, pval *arg_property, pval *value TSRMLS_DC)
{
	pval type;

	Z_TYPE(type) = IS_STRING;

	if (!strcmp(Z_STRVAL_P(arg_property), "bVal")) {
		Z_LVAL(type) = VT_UI1;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "iVal")) { 
		Z_LVAL(type) = VT_I2;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "lVal")) {
		Z_LVAL(type) = VT_I4;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "fltVal")) {
		Z_LVAL(type) = VT_R4;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "dblVal")) {
		Z_LVAL(type) = VT_R8;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "boolVal")) {
		Z_LVAL(type) = VT_BOOL;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "scode")) {
		Z_LVAL(type) = VT_ERROR;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "cyVal")) {
		Z_LVAL(type) = VT_CY;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "date")) {
		Z_LVAL(type) = VT_DATE;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "bstrVal")) {
		Z_LVAL(type) = VT_BSTR;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pdecVal")) {
		Z_LVAL(type) = VT_DECIMAL|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "punkVal")) {
		Z_LVAL(type) = VT_UNKNOWN;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pdispVal")) {
		Z_LVAL(type) = VT_DISPATCH;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "parray")) {
		Z_LVAL(type) = VT_ARRAY;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pbVal")) {
		Z_LVAL(type) = VT_UI1|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "piVal")) {
		Z_LVAL(type) = VT_I2|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "plVal")) {
		Z_LVAL(type) = VT_I4|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pfltVal")) {
		Z_LVAL(type) = VT_R4|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pdblVal")) {
		Z_LVAL(type) = VT_R8|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pboolVal")) {
		Z_LVAL(type) = VT_BOOL|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pscode")) {
		Z_LVAL(type) = VT_ERROR|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pcyVal")) {
		Z_LVAL(type) = VT_CY|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pdate")) {
		Z_LVAL(type) = VT_DATE|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pbstrVal")) {
		Z_LVAL(type) = VT_BSTR|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "ppunkVal")) {
		Z_LVAL(type) = VT_UNKNOWN|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "ppdispVal")) {
		Z_LVAL(type) = VT_DISPATCH|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pparray")) {
		Z_LVAL(type) = VT_ARRAY|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pvarVal")) {
		Z_LVAL(type) = VT_VARIANT|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "byref")) {
		Z_LVAL(type) = VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "cVal")) {
		Z_LVAL(type) = VT_I1;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "uiVal")) {
		Z_LVAL(type) = VT_UI2;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "ulVal")) {
		Z_LVAL(type) = VT_UI4;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "intVal")) {
		Z_LVAL(type) = VT_INT;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "uintVal")) {
		Z_LVAL(type) = VT_UINT|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pcVal")) {
		Z_LVAL(type) = VT_I1|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "puiVal")) {
		Z_LVAL(type) = VT_UI2|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pulVal")) {
		Z_LVAL(type) = VT_UI4|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "pintVal")) {
		Z_LVAL(type) = VT_INT|VT_BYREF;
	} else if (!strcmp(Z_STRVAL_P(arg_property), "puintVal")) {
		Z_LVAL(type) = VT_UINT|VT_BYREF;
	} else {
		php_error(E_WARNING, "Unknown member.");
		return FAILURE;
	}

	php_pval_to_variant_ex(value, var_arg, &type, codepage TSRMLS_CC);

	return SUCCESS;
}


static void php_VARIANT_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	FREE_VARIANT(rsrc->ptr);
}

static void php_register_VARIANT_class(TSRMLS_D)
{
	INIT_OVERLOADED_CLASS_ENTRY(VARIANT_class_entry, "VARIANT", NULL,
								php_VARIANT_call_function_handler,
								php_VARIANT_get_property_handler,
								php_VARIANT_set_property_handler);

	zend_register_internal_class(&VARIANT_class_entry TSRMLS_CC);
}

#endif /* PHP_WIN32 */
