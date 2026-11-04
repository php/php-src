/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#include "collator.h"
#include "collator_class.h"
#include "php_intl.h"
#include "intl_error.h"

BEGIN_EXTERN_C()
#include "collator_arginfo.h"
END_EXTERN_C()
#include "collator_sort.h"
#include "collator_convert.h"

#include <unicode/ucol.h>


zend_class_entry *Collator_ce_ptr = nullptr;
static zend_object_handlers Collator_handlers;

/*
 * Auxiliary functions needed by objects of 'Collator' class
 */

/* Clean up mem allocted by internals of Collator_object */
static void collator_object_destroy( Collator_object* co )
{
	if( !co )
		return;

	if( co->ucoll )
	{
		ucol_close( co->ucoll );
		co->ucoll = nullptr;
	}

	intl_error_reset( COLLATOR_ERROR_P( co ) );
}

static void Collator_objects_free(zend_object *object )
{
	Collator_object* co = php_intl_collator_fetch_object(object);

	zend_object_std_dtor(&co->zo );

	collator_object_destroy(co );
}

/* {{{ Collator_object_create */
static zend_object *Collator_object_create(zend_class_entry *ce )
{
	Collator_object *intern = reinterpret_cast<Collator_object *>(zend_object_alloc(sizeof(Collator_object), ce));
	intl_error_init(COLLATOR_ERROR_P(intern));
	zend_object_std_init(&intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	return &intern->zo;
}
/* }}} */

/*
 * 'Collator' class registration structures & functions
 */

/* {{{ collator_register_Collator_symbols
 * Initialize 'Collator' class
 */
U_CFUNC void collator_register_Collator_symbols(int module_number)
{
	register_collator_symbols(module_number);

	/* Create and register 'Collator' class. */
	Collator_ce_ptr = register_class_Collator();
	Collator_ce_ptr->create_object = Collator_object_create;
	Collator_ce_ptr->default_object_handlers = &Collator_handlers;

	memcpy(&Collator_handlers, &std_object_handlers,
		sizeof Collator_handlers);
	/* Collator has no usable clone semantics - ucol_cloneBinary/ucol_openBinary require binary buffer
	   for which we don't have the place to keep */
	Collator_handlers.offset = offsetof(Collator_object, zo);
	Collator_handlers.clone_obj = nullptr;
	Collator_handlers.free_obj = Collator_objects_free;
}
/* }}} */
