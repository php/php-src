/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
   |         Alan Brown   <abrown@pobox.com>                              |
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
#include "conversion.h"
#include "variant.h"

#include <unknwn.h> 

static zend_object_value variant_objects_new(zend_class_entry * TSRMLS_DC);
static void variant_objects_delete(void *, zend_object_handle TSRMLS_DC);

/* object handler */
static zval* variant_read(zval *, zval * TSRMLS_DC);
static void variant_write(zval *, zval *, zval * TSRMLS_DC);
static union _zend_function* variant_get_constructor(zval * TSRMLS_DC);
static zend_class_entry* variant_get_class_entry(zval * TSRMLS_DC);
/**/

static HashTable types;
static zend_class_entry *variant_class_entry;
static zend_function *variant_ctor;
static zend_object_handlers variant_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,

	variant_read,
	variant_write,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	variant_get_constructor,
	variant_get_class_entry,
	NULL,
	NULL
};

/**/
void php_variant_init(int module_number TSRMLS_DC)
{
	zend_internal_function *zif;
	zend_class_entry ce;
	int type;

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
	REGISTER_LONG_CONSTANT("VT_DATE", VT_DATE, CONST_CS | CONST_PERSISTENT);
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
#error "CP_SYMBOL undefined"
#endif
#ifdef CP_THREAD_ACP
	REGISTER_LONG_CONSTANT("CP_THREAD_ACP", CP_THREAD_ACP, CONST_CS | CONST_PERSISTENT);
#else
#error "CP_THREAD_ACP undefined"
#endif

	INIT_CLASS_ENTRY(ce, "variant", NULL);
	ce.create_object = variant_objects_new;
	variant_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	zif = (zend_internal_function *) emalloc(sizeof(zend_internal_function));

	zif->type = ZEND_INTERNAL_FUNCTION;
	zif->function_name = variant_class_entry->name;
	zif->scope = variant_class_entry;
	zif->arg_types = NULL;
	zif->handler = ZEND_FN(variant_load);

	/* add new constructor to the method table */
	zend_hash_add(&(variant_class_entry->function_table), variant_class_entry->name,
					variant_class_entry->name_length + 1, zif, sizeof(zend_function), &variant_ctor);
	efree(zif);

	zend_hash_init(&types, 0, NULL, NULL, TRUE);

	type = VT_UI1;
	zend_hash_add(&types, "bVal",		5,  (void *) &type, sizeof(int), NULL);
	type = VT_UI2;
	zend_hash_add(&types, "uiVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_UI4;
	zend_hash_add(&types, "ulVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_UINT;
	zend_hash_add(&types, "uintVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_I1;
	zend_hash_add(&types, "cVal",		5,  (void *) &type, sizeof(int), NULL);
	type = VT_I2;
	zend_hash_add(&types, "iVal",		5,  (void *) &type, sizeof(int), NULL);
	type = VT_I4;
	zend_hash_add(&types, "lVal",		5,  (void *) &type, sizeof(int), NULL);
	type = VT_INT;
	zend_hash_add(&types, "intVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_R4;
	zend_hash_add(&types, "fltVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_R8;
	zend_hash_add(&types, "dblVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BOOL;
	zend_hash_add(&types, "boolVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_ERROR;
	zend_hash_add(&types, "scode",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_CY;
	zend_hash_add(&types, "cyVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_DATE;
	zend_hash_add(&types, "date",		5,  (void *) &type, sizeof(int), NULL);
	type = VT_BSTR;
	zend_hash_add(&types, "bstrVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_UNKNOWN;
	zend_hash_add(&types, "punkVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_DISPATCH;
	zend_hash_add(&types, "pdispVal",	9,  (void *) &type, sizeof(int), NULL);
	type = VT_ARRAY;
	zend_hash_add(&types, "parray",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF;
	zend_hash_add(&types, "byref",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_UI1;
	zend_hash_add(&types, "pbVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_UI2;
	zend_hash_add(&types, "puiVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_UI4;
	zend_hash_add(&types, "pulVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_UINT;
	zend_hash_add(&types, "puintVal",	9,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_I1;
	zend_hash_add(&types, "pcVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_I2;
	zend_hash_add(&types, "piVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_I4;
	zend_hash_add(&types, "plVal",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_INT;
	zend_hash_add(&types, "pintVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_R4;
	zend_hash_add(&types, "pfltVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_R8;
	zend_hash_add(&types, "pdblVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_BOOL;
	zend_hash_add(&types, "pboolVal",	9,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_ERROR;
	zend_hash_add(&types, "pscode",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_CY;
	zend_hash_add(&types, "pcyVal",		7,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_DATE;
	zend_hash_add(&types, "pdate",		6,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_BSTR;
	zend_hash_add(&types, "pbstrVal",	9,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_DECIMAL;
	zend_hash_add(&types, "pdecVal",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_UNKNOWN;
	zend_hash_add(&types, "ppunkVal",	9,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_DISPATCH;
	zend_hash_add(&types, "ppdispVal",	10, (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_ARRAY;
	zend_hash_add(&types, "pparray",	8,  (void *) &type, sizeof(int), NULL);
	type = VT_BYREF|VT_VARIANT;
	zend_hash_add(&types, "pvarVal",	8,  (void *) &type, sizeof(int), NULL);
}

void php_variant_shutdown(TSRMLS_D)
{
	zend_hash_destroy(&types);
}

static zend_object_value variant_objects_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value *zov;
	variantval *var;

	ALLOC_VARIANT(var);
	
	/* set up the object value struct */
	zov = (zend_object_value*) emalloc(sizeof(zend_object_value));
	zov->handlers = &variant_handlers;
	zov->handle = zend_objects_store_put(var, variant_objects_delete, NULL TSRMLS_CC);

	return *zov;
}

static void variant_objects_delete(void *object, zend_object_handle handle TSRMLS_DC)
{
	FREE_VARIANT((variantval *)object);
}

static zval* variant_read(zval *object, zval *member TSRMLS_DC)
{
	variantval *var;
	zval *result;

	ALLOC_ZVAL(result);

	if ((var = zend_object_store_get_object(object TSRMLS_CC)) == NULL) {
		/* exception */
	}

	if (!strcmp(Z_STRVAL_P(member), "value")) {
		/* var_arg can't be an idispatch, so we don't care for the implicit AddRef() call here */
		php_variant_to_zval(var->var, result, var->codepage);
	} else if (!strcmp(Z_STRVAL_P(member), "type")) {
		ZVAL_LONG(result, V_VT(var->var));
	} else {
		ZVAL_FALSE(result);
		rpc_error(NULL TSRMLS_CC, E_WARNING, "Unknown member.");
	}

	return result;
}

static void variant_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
	int *type;
	variantval *var;

	if ((var = zend_object_store_get_object(object TSRMLS_CC)) == NULL) {
		/* exception */
	}

	if (!strcmp(Z_STRVAL_P(member), "value")) {
		php_zval_to_variant(value, var->var, var->codepage);
	} else if (zend_hash_find(&types, Z_STRVAL_P(member), Z_STRLEN_P(member) + 1, (void **) &type) == SUCCESS) {
		php_zval_to_variant_ex(value, var->var, *type, var->codepage);
	} else {
		rpc_error(NULL TSRMLS_CC, E_WARNING, "Unknown member.");
	}
}

static union _zend_function* variant_get_constructor(zval *object TSRMLS_DC)
{
	return variant_ctor;
}

static zend_class_entry* variant_get_class_entry(zval *object TSRMLS_DC)
{
	return variant_class_entry;
}

/**/
/* constructor */
ZEND_FUNCTION(variant_load)
{
	zval *value = NULL, *object = getThis();
	variantval *var;
	long type = 0;

	if (!object || ((var = zend_object_store_get_object(object TSRMLS_CC)) == NULL)) {
		/*TODO exception */
	}

	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zll", &value, &type, &(var->codepage));

	if (value) {
		if (type) {
			php_zval_to_variant_ex(value, var->var, type, var->codepage);
		} else {
			php_zval_to_variant(value, var->var, var->codepage);
		}
	}
}

#endif /* PHP_WIN32 */
