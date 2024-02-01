/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#include "collator.h"
#include "collator_class.h"
#include "php_intl.h"
#include "collator_sort.h"
#include "collator_convert.h"
#include "intl_error.h"

#include <unicode/ucol.h>

#include "collator_arginfo.h"

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

/* {{{ collator_register_Collator_symbols
 * Initialize 'Collator' class
 */
void collator_register_Collator_symbols(int module_number)
{
	register_collator_symbols(module_number);

	/* Create and register 'Collator' class. */
	Collator_ce_ptr = register_class_Collator();
	Collator_ce_ptr->create_object = Collator_object_create;

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
