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


const zend_function_entry collator_funcs_collator[] = {
	ZEND_ME(collator, __construct, NULL, ZEND_ACC_PUBLIC)
	ZEND_FENTRY(getDefault, 	ZEND_FN(collator_get_default), NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FENTRY(setDefault, 	ZEND_FN(collator_set_default), NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_NAMED_FE(compare, 		ZEND_FN(collator_compare), NULL)
	ZEND_NAMED_FE(setStrength, 	ZEND_FN(collator_set_strength), NULL)
	ZEND_NAMED_FE(setAttribute, ZEND_FN(collator_set_attribute), NULL)
	ZEND_NAMED_FE(getStrength, 	ZEND_FN(collator_get_strength), NULL)
	ZEND_NAMED_FE(getAttribute, ZEND_FN(collator_get_attribute), NULL)
	ZEND_NAMED_FE(sort,			ZEND_FN(collator_sort), NULL)
	{NULL, NULL, NULL}
};

zend_class_entry *unicode_collator_ce;

static zend_object_handlers unicode_object_handlers_collator;

typedef struct _php_collator_obj php_collator_obj;

struct _php_collator_obj {
	zend_object      std;
	zend_collator *zcoll;
};

#define COLLATOR_SET_CONTEXT \
	zval *object; \
	object = getThis(); \
   
#define COLLATOR_FETCH_OBJECT	\
	php_collator_obj *obj;	\
	COLLATOR_SET_CONTEXT; \
	if (object) {	\
		if (zend_parse_parameters_none() == FAILURE) {	\
			return;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, unicode_ce_collator) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);

static zend_object_value collator_object_create(zend_class_entry *class_type TSRMLS_DC);
static void collator_object_free_storage(void *object TSRMLS_DC);

static void collator_register_class(TSRMLS_D)
{
	zend_class_entry ce_collator;

	INIT_CLASS_ENTRY(ce_collator, "Collator", collator_funcs_collator);
	ce_collator.create_object = collator_object_create;
	unicode_collator_ce = zend_register_internal_class_ex(&ce_collator, NULL, NULL TSRMLS_CC);
	memcpy(&unicode_object_handlers_collator, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	unicode_object_handlers_collator.clone_obj = NULL;

#define REGISTER_COLLATOR_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(unicode_collator_ce, const_name, sizeof(const_name)-1, value, sizeof(value)-1 TSRMLS_CC);
#define REGISTER_COLLATOR_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(unicode_collator_ce, const_name, sizeof(const_name)-1, value TSRMLS_CC);

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


static zend_object_value collator_object_create(zend_class_entry *ce TSRMLS_DC)
{
	php_collator_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_collator_obj));
	memset(intern, 0, sizeof(php_collator_obj));

	zend_object_std_init(&intern->std, ce TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) collator_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &unicode_object_handlers_collator;
	
	return retval;
}

static void collator_object_free_storage(void *object TSRMLS_DC)
{
	php_collator_obj *intern = (php_collator_obj *)object;

	if (intern->zcoll) {
		zend_collator_destroy(intern->zcoll);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	
	efree(object);
}

static zval* collator_set_wrapper(zval *object, zend_collator *zcoll TSRMLS_DC)
{
	php_collator_obj *coll_obj;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		object_init_ex(object, unicode_collator_ce);
	}

	coll_obj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	coll_obj->zcoll = zcoll;

	return object;
}

/* {{{ proto Collator::__construct(string locale) U
   Create a new Collator object */
PHP_METHOD(collator, __construct)
{
	zif_collator_create(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto Collator collator_create(string locale) U
   Create a new Collator object */
PHP_FUNCTION(collator_create)
{
	UErrorCode        status = U_ZERO_ERROR;
	char             *collator_name;
	int               collator_name_len;
	zval 			 *object;
	UCollator		 *ucoll;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &collator_name, &collator_name_len) == FAILURE) {
		RETURN_FALSE;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if ((object = getThis()) == NULL) {
		object = return_value;
	}
	ucoll = ucol_open(collator_name, &status);
	if (U_FAILURE(status)) {
		/* UTODO handle error case properly */
		zend_error(E_ERROR, "Could not open collator for locale %s", collator_name);
		return;
	}
	collator_set_wrapper(object, zend_collator_create(ucoll) TSRMLS_CC);
}
/* }}} */

/* {{{ proto int Collator::compare(string str1, string str2) U
   Compare two strings using collation }}} */
/* {{{ proto int collator_compare(Collator coll, string str1, string str2) U
   Compare two strings using collation */
PHP_FUNCTION(collator_compare)
{
	zval             *object;
	php_collator_obj *collatorobj;
	UChar            *string1, *string2;
	int               string1_len, string2_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ouu", &object, unicode_collator_ce, &string1, &string1_len, &string2, &string2_len) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_LONG(ucol_strcoll(collatorobj->zcoll->coll, string1, string1_len, string2, string2_len));
}
/* }}} */

/* {{{ proto array Collator::sort(array input) U
   Sort an array using collation }}} */
/* {{{ proto array collator_sort(Collator coll, array input) U
   Sort an array using collation */
PHP_FUNCTION(collator_sort)
{
	zval             *object;
	php_collator_obj *collatorobj;
	zval             *array;
	HashTable        *target_hash;
	zend_collator    *orig_collator;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/", &object, unicode_collator_ce, &array) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);

	target_hash = HASH_OF(array);
	php_set_compare_func(PHP_SORT_LOCALE_STRING TSRMLS_CC);
	orig_collator = UG(default_collator);
	UG(default_collator) = collatorobj->zcoll;
	if (zend_hash_sort(target_hash, zend_qsort, php_array_data_compare, 1 TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		RETVAL_ZVAL(array, 1, 0);
	}
	UG(default_collator) = orig_collator;
}
/* }}} */

/* {{{ proto void Collator::setStrength(int strength) U
   Set the collation strength }}} */
/* {{{ proto void collator_set_strength(Collator coll, int strength) U
   Set the collation strength */
PHP_FUNCTION(collator_set_strength)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              strength;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, unicode_collator_ce, &strength) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	ucol_setStrength(collatorobj->zcoll->coll, strength);
}
/* }}} */

/* {{{ proto int Collator::getStrength() U
   Returns the current collation strength }}} */
/* {{{ proto int collator_get_strength(Collator coll) U
   Returns the current collation strength */
PHP_FUNCTION(collator_get_strength)
{
	zval             *object;
	php_collator_obj *collatorobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, unicode_collator_ce) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_LONG(ucol_getStrength(collatorobj->zcoll->coll));
}
/* }}} */

/* {{{ proto bool Collator::setAttribute(int attribute, int value) U
   Set a collation attribute }}} */
/* {{{ proto bool collator_set_attribute(Collator coll, int attribute, int value) U
   Set a collation attribute */
PHP_FUNCTION(collator_set_attribute)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              attribute, value;
	UErrorCode        error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll", &object, unicode_collator_ce, &attribute, &value) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	error = U_ZERO_ERROR;
	ucol_setAttribute(collatorobj->zcoll->coll, attribute, value, &error);
	RETURN_BOOL(error == U_ZERO_ERROR ? 1 : 0);
}
/* }}} */


/* {{{ proto int Collator::getAttribute(int attribute) U
   Returns a collation attribute }}} */
/* {{{ proto int collator_get_attribute(Collator coll, int attribute) U
   Returns a collation attribute */
PHP_FUNCTION(collator_get_attribute)
{
	zval             *object;
	php_collator_obj *collatorobj;
	long              attribute, value;
	UErrorCode        error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, unicode_collator_ce, &attribute) == FAILURE) {
		RETURN_FALSE;
	}
	collatorobj = (php_collator_obj *) zend_object_store_get_object(object TSRMLS_CC);
	error = U_ZERO_ERROR;
	value = ucol_getAttribute(collatorobj->zcoll->coll, attribute, &error);
	if (error != U_ZERO_ERROR) {
		RETURN_FALSE;
	}
	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto Collator collator_get_default(void) U
   Returns default collator */
PHP_FUNCTION(collator_get_default)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	UG(default_collator)->refcount++;
	collator_set_wrapper(return_value, UG(default_collator) TSRMLS_CC);
}
/* }}} */

/* {{{ proto void collator_set_default(Collator coll) U
   Sets default collator */
PHP_FUNCTION(collator_set_default)
{
	zval *coll;
	php_collator_obj *coll_obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &coll, unicode_collator_ce) == FAILURE) {
		RETURN_FALSE;
	}

	coll_obj = (php_collator_obj *) zend_object_store_get_object(coll TSRMLS_CC);
	zend_collator_destroy(UG(default_collator));
	coll_obj->zcoll->refcount++;
	UG(default_collator) = coll_obj->zcoll;

	RETURN_TRUE;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
