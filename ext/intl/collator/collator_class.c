/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#include "collator_class.h"
#include "php_intl.h"
#include "collator_attr.h"
#include "collator_compare.h"
#include "collator_sort.h"
#include "collator_convert.h"
#include "collator_locale.h"
#include "collator_create.h"
#include "collator_error.h"
#include "collator_arginfo.h"
#include "intl_error.h"

#include <unicode/ucol.h>

zend_class_entry *Collator_ce_ptr = NULL;
static zend_object_handlers Collator_handlers;

/*
 * Auxiliary functions needed by objects of 'Collator' class
 */

/* {{{ Collator_objects_free */
void Collator_objects_free(zend_object *object )
{
	Collator_object* co = php_intl_collator_fetch_object(object);

	zend_object_std_dtor(&co->zo );

	collator_object_destroy(co );
}
/* }}} */

/* {{{ Collator_object_create */
zend_object *Collator_object_create(zend_class_entry *ce )
{
	Collator_object *intern = zend_object_alloc(sizeof(Collator_object), ce);
	intl_error_init(COLLATOR_ERROR_P(intern));
	zend_object_std_init(&intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	intern->zo.handlers = &Collator_handlers;

	return &intern->zo;
}
/* }}} */

/*
 * 'Collator' class registration structures & functions
 */

/* {{{ Collator_class_functions
 * Every 'Collator' class method has an entry in this table
 */

static const zend_function_entry Collator_class_functions[] = {
	PHP_ME( Collator, __construct, arginfo_class_Collator___construct, ZEND_ACC_PUBLIC )
	ZEND_FENTRY( create, ZEND_FN( collator_create ), arginfo_class_Collator_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC )
	PHP_NAMED_FE( compare, ZEND_FN( collator_compare ), arginfo_class_Collator_compare )
	PHP_NAMED_FE( sort, ZEND_FN( collator_sort ), arginfo_class_Collator_sort )
	PHP_NAMED_FE( sortWithSortKeys, ZEND_FN( collator_sort_with_sort_keys ), arginfo_class_Collator_sortWithSortKeys )
	PHP_NAMED_FE( asort, ZEND_FN( collator_asort ), arginfo_class_Collator_asort )
	PHP_NAMED_FE( getAttribute, ZEND_FN( collator_get_attribute ), arginfo_class_Collator_getAttribute )
	PHP_NAMED_FE( setAttribute, ZEND_FN( collator_set_attribute ), arginfo_class_Collator_setAttribute )
	PHP_NAMED_FE( getStrength, ZEND_FN( collator_get_strength ), arginfo_class_Collator_getStrength )
	PHP_NAMED_FE( setStrength, ZEND_FN( collator_set_strength ), arginfo_class_Collator_setStrength )
	PHP_NAMED_FE( getLocale, ZEND_FN( collator_get_locale ), arginfo_class_Collator_getLocale )
	PHP_NAMED_FE( getErrorCode, ZEND_FN( collator_get_error_code ), arginfo_class_Collator_getErrorCode )
	PHP_NAMED_FE( getErrorMessage, ZEND_FN( collator_get_error_message ), arginfo_class_Collator_getErrorMessage )
	PHP_NAMED_FE( getSortKey, ZEND_FN( collator_get_sort_key ), arginfo_class_Collator_getSortKey )
	PHP_FE_END
};
/* }}} */

/* {{{ collator_register_Collator_class
 * Initialize 'Collator' class
 */
void collator_register_Collator_class( void )
{
	zend_class_entry ce;

	/* Create and register 'Collator' class. */
	INIT_CLASS_ENTRY( ce, "Collator", Collator_class_functions );
	ce.create_object = Collator_object_create;
	Collator_ce_ptr = zend_register_internal_class( &ce );

	memcpy(&Collator_handlers, &std_object_handlers,
		sizeof Collator_handlers);
	/* Collator has no usable clone semantics - ucol_cloneBinary/ucol_openBinary require binary buffer
	   for which we don't have the place to keep */
	Collator_handlers.offset = XtOffsetOf(Collator_object, zo);
	Collator_handlers.clone_obj = NULL;
	Collator_handlers.free_obj = Collator_objects_free;

	/* Declare 'Collator' class properties. */
	if( !Collator_ce_ptr )
	{
		zend_error( E_ERROR,
			"Collator: attempt to create properties "
			"on a non-registered class." );
		return;
	}
}
/* }}} */

/* {{{ void collator_object_init( Collator_object* co )
 * Initialize internals of Collator_object.
 * Must be called before any other call to 'collator_object_...' functions.
 */
void collator_object_init( Collator_object* co )
{
	if( !co )
		return;

	intl_error_init( COLLATOR_ERROR_P( co ) );
}
/* }}} */

/* {{{ void collator_object_destroy( Collator_object* co )
 * Clean up mem allocted by internals of Collator_object
 */
void collator_object_destroy( Collator_object* co )
{
	if( !co )
		return;

	if( co->ucoll )
	{
		ucol_close( co->ucoll );
		co->ucoll = NULL;
	}

	intl_error_reset( COLLATOR_ERROR_P( co ) );
}
/* }}} */
