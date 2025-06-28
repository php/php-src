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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#include <unicode/unum.h>

#include "formatter_class.h"
extern "C" {
#include "php_intl.h"
#include "formatter_data.h"
#include "formatter_format.h"
}

#include <zend_exceptions.h>
#include "Zend/zend_attributes.h"
#include "Zend/zend_interfaces.h"

extern "C" {
#include "formatter_arginfo.h"
}

zend_class_entry *NumberFormatter_ce_ptr = NULL;
static zend_object_handlers NumberFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'NumberFormatter' class
 */

/* {{{ NumberFormatter_objects_free */
U_CFUNC void NumberFormatter_object_free( zend_object *object )
{
	NumberFormatter_object* nfo = php_intl_number_format_fetch_object(object);

	zend_object_std_dtor( &nfo->zo );

	formatter_data_free( &nfo->nf_data );
}
/* }}} */

/* {{{ NumberFormatter_object_create */
U_CFUNC zend_object *NumberFormatter_object_create(zend_class_entry *ce)
{
	NumberFormatter_object*     intern;

	intern = reinterpret_cast<NumberFormatter_object *>(zend_object_alloc(sizeof(NumberFormatter_object), ce));
	formatter_data_init( &intern->nf_data );
	zend_object_std_init( &intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	return &intern->zo;
}
/* }}} */

/* {{{ NumberFormatter_object_clone */
U_CFUNC zend_object *NumberFormatter_object_clone(zend_object *object)
{
	NumberFormatter_object     *nfo = php_intl_number_format_fetch_object(object);
	zend_object            *new_obj = NumberFormatter_ce_ptr->create_object(object->ce);
	NumberFormatter_object *new_nfo = php_intl_number_format_fetch_object(new_obj);

	/* clone standard parts */
	zend_objects_clone_members(&new_nfo->zo, &nfo->zo);

	/* clone formatter object. It may fail, the destruction code must handle this case */
	if (FORMATTER_OBJECT(nfo) != NULL) {
		UErrorCode error = U_ZERO_ERROR;
		FORMATTER_OBJECT(new_nfo) = unum_clone(FORMATTER_OBJECT(nfo), &error);
		if (U_FAILURE(error)) {
			zend_throw_error(NULL, "Failed to clone NumberFormatter");
		}
	} else {
		zend_throw_error(NULL, "Cannot clone uninitialized NumberFormatter");
	}
	return new_obj;
}
/* }}} */

/*
 * 'NumberFormatter' class registration structures & functions
 */

/* {{{ formatter_register_class
 * Initialize 'NumberFormatter' class
 */
U_CFUNC void formatter_register_class( void )
{
	/* Create and register 'NumberFormatter' class. */
	NumberFormatter_ce_ptr = register_class_NumberFormatter();
	NumberFormatter_ce_ptr->create_object = NumberFormatter_object_create;
	NumberFormatter_ce_ptr->default_object_handlers = &NumberFormatter_handlers;

	memcpy(&NumberFormatter_handlers, &std_object_handlers,
		sizeof(NumberFormatter_handlers));
	NumberFormatter_handlers.offset = XtOffsetOf(NumberFormatter_object, zo);
	NumberFormatter_handlers.clone_obj = NumberFormatter_object_clone;
	NumberFormatter_handlers.free_obj = NumberFormatter_object_free;
}
/* }}} */
