/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "ext/standard/php_array.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "php_unicode.h"
#include <unicode/ucol.h>


zend_function_entry collator_funcs_collator[] = {
	ZEND_ME(collator, __construct, NULL, ZEND_ACC_PUBLIC)
	ZEND_NAMED_FE(compare, ZEND_FN(collator_compare), NULL)
	ZEND_NAMED_FE(setStrength, ZEND_FN(collator_set_strength), NULL)
	ZEND_NAMED_FE(setAttribute, ZEND_FN(collator_set_attribute), NULL)
	ZEND_NAMED_FE(getStrength, ZEND_FN(collator_get_strength), NULL)
	ZEND_NAMED_FE(getAttribute, ZEND_FN(collator_get_attribute), NULL)
	ZEND_NAMED_FE(sort, ZEND_FN(collator_sort), NULL)
	{NULL, NULL, NULL}
};

zend_class_entry *unicode_ce_collator;

static zend_object_handlers unicode_object_handlers_collator;

typedef struct _php_collator_obj php_collator_obj;

struct _php_collator_obj {
	zend_object   std;
	UCollator *col;
};

#define COLLATOR_SET_CONTEXT \
	zval *object; \
	object = getThis(); \
   
#define COLLATOR_FETCH_OBJECT	\
	php_collator_obj *obj;	\
	COLLATOR_SET_CONTEXT; \
	if (object) {	\
		if (ZEND_NUM_ARGS()) {	\
			WRONG_PARAM_COUNT;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, unicode_ce_collator) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);

static zend_object_value collator_object_new(zend_class_entry *class_type TSRMLS_DC);
static void collator_object_free_storage(void *object TSRMLS_DC);

static void collator_register_class(TSRMLS_D)
{
	zend_class_entry ce_collator;

	INIT_CLASS_ENTRY(ce_collator, "Collator", collator_funcs_collator);
	ce_collator.create_object = collator_object_new;
	unicode_ce_collator = zend_register_internal_class_ex(&ce_collator, NULL, NULL TSRMLS_CC);
	memcpy(&unicode_object_handlers_collator, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	unicode_object_handlers_collator.clone_obj = NULL;

#define REGISTER_COLLATOR_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(unicode_ce_collator, const_name, sizeof(const_name)-1, value, sizeof(value)-1 TSRMLS_CC);
#define REGISTER_COLLATOR_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(unicode_ce_collator, const_name, sizeof(const_name)-1, value TSRMLS_CC);

	/* Attributes */
	REGISTER_COLLATOR_CLASS_CONST_LONG("FRENCH_COLLATION", UCOL_FRENCH_COLLATION);
	REGISTER_COLLATOR_CLASS_CONST_LONG("ALTERNATE_HANDLING", UCOL_ALTERNATE_HANDLING);
	REGISTER_COLLATOR_CLASS_CONST_LONG("CASE_FIRST", UCOL_CASE_FIRST);
	REGISTER_COLLATOR_CLASS_CONST_LONG("CASE_LEVEL", UCOL_CASE_LEVEL);
	REGISTER_COLLATOR_CLASS_CONST_LONG("NORMALIZATION_MODE", UCOL_NORMALIZATION_MODE);
	REGISTER_COLLATOR_CLASS_CONST_LONG("STRENGTH", UCOL_STRENGTH);
	REGISTER_COLLATOR_CLASS_CONST_LONG("HIRAGANA_QUARTERNARY_MODE", UCOL_HIRAGANA_QUATERNARY_MODE);
	REGISTER_COLLATOR_CLASS_CONST_LONG("NUMERIC_COLLATION", UCOL_NUMERIC_COLLATION);

	
	/* Attribute value constants */
	REGISTER_COLLATOR_CLASS_CONST_LONG("DEFAULT_VALUE", UCOL_DEFAULT);
	REGISTER_COLLATOR_CLASS_CONST_LONG("PRIMARY", UCOL_PRIMARY);
	REGISTER_COLLATOR_CLASS_CONST_LONG("SECONDARY", UCOL_SECONDARY);
	REGISTER_COLLATOR_CLASS_CONST_LONG("TERTIARY", UCOL_TERTIARY);
	REGISTER_COLLATOR_CLASS_CONST_LONG("DEFAULT_STRENGHT", UCOL_DEFAULT_STRENGTH);
	REGISTER_COLLATOR_CLASS_CONST_LONG("QUARTERNARY", UCOL_QUATERNARY);
	REGISTER_COLLATOR_CLASS_CONST_LONG("IDENTICAL", UCOL_IDENTICAL);
	REGISTER_COLLATOR_CLASS_CONST_LONG("ON", UCOL_ON);
	REGISTER_COLLATOR_CLASS_CONST_LONG("OFF", UCOL_OFF);
	REGISTER_COLLATOR_CLASS_CONST_LONG("SHIFTED", UCOL_SHIFTED);
	REGISTER_COLLATOR_CLASS_CONST_LONG("NON_IGNORABLE", UCOL_NON_IGNORABLE);
	REGISTER_COLLATOR_CLASS_CONST_LONG("LOWER_FIRST", UCOL_LOWER_FIRST);
	REGISTER_COLLATOR_CLASS_CONST_LONG("UPPER_FIRST", UCOL_UPPER_FIRST);
}


void php_init_collation(TSRMLS_D)
{
	collator_register_class(TSRMLS_C);
}


static zend_object_value collator_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	php_collator_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_collator_obj));
	memset(intern, 0, sizeof(php_collator_obj));
	intern->std.ce = class_type;
	
	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) collator_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &unicode_object_handlers_collator;
	
	return retval;
}

static void collator_object_free_storage(void *object TSRMLS_DC)
{
	php_collator_obj *intern = (php_collator_obj *)object;

	if (intern->col) {
		ucol_close(intern->col);
	}

	if (intern->std.properties) {
		zend_hash_destroy(intern->std.properties);
		efree(intern->std.properties);
		intern->std.properties = NULL;
	}
	
	efree(object);
}

static zval* collator_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

PHP_METHOD(collator, __construct)
{
	zif_collator_create(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(collator_create)
{
	php_collator_obj *collatorobj;
	UErrorCode        error;
	char             *collator_name;
	int               collator_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &collator_name, &collator_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	collator_instanciate(unicode_ce_collator, return_value TSRMLS_CC);
	collatorobj = (php_collator_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	error = U_ZERO_ERROR;
	collatorobj->col = ucol_open(collator_name, &error);
}

PHP_FUNCTION(collator_compare)
{
	zval             *object;
	php_collator_obj *collatorobj;
	UChar            *string1, *string2;
	int               string1_len, string2_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ouu", &object, unicode_ce_collator, &string1, &string1_len, &string2, &string2_len) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_LONG(ucol_strcoll(collatorobj->col, string1, string1_len, string2, string2_len));
}

PHP_FUNCTION(collator_sort)
{
	zval             *object;
	php_collator_obj *collatorobj;
	zval             *array;
	HashTable        *target_hash;
	UCollator        *orig_collator;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/", &object, unicode_ce_collator, &array) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);

	target_hash = HASH_OF(array);
	php_set_compare_func(SORT_LOCALE_STRING TSRMLS_CC);
	orig_collator = UG(default_collator);
	UG(default_collator) = collatorobj->col;
	if (zend_hash_sort(target_hash, zend_qsort, php_array_data_compare, 1 TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		RETVAL_ZVAL(array, 1, 0);
	}
	UG(default_collator) = orig_collator;
}

PHP_FUNCTION(collator_set_strength)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              strength;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, unicode_ce_collator, &strength) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	ucol_setStrength(collatorobj->col, strength);
}

PHP_FUNCTION(collator_get_strength)
{
	zval             *object;
	php_collator_obj *collatorobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, unicode_ce_collator) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_LONG(ucol_getStrength(collatorobj->col));
}

PHP_FUNCTION(collator_set_attribute)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              attribute, value;
	UErrorCode        error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll", &object, unicode_ce_collator, &attribute, &value) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	error = U_ZERO_ERROR;
	ucol_setAttribute(collatorobj->col, attribute, value, &error);
	RETURN_BOOL(error == U_ZERO_ERROR ? 1 : 0);
}

PHP_FUNCTION(collator_get_attribute)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              attribute, value;
	UErrorCode        error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, unicode_ce_collator, &attribute) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	error = U_ZERO_ERROR;
	value = ucol_getAttribute(collatorobj->col, attribute, &error);
	if (error != U_ZERO_ERROR) {
		RETURN_FALSE;
	}
	RETURN_LONG(value);
}
