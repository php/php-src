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
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "transliterator_class.h"
extern "C" {
#include "php_intl.h"
#include "intl_error.h"
#include "intl_convert.h"
#include "intl_data.h"
#include "transliterator_arginfo.h"
}

#include <unicode/utrans.h>

zend_class_entry *Transliterator_ce_ptr = nullptr;

zend_object_handlers Transliterator_handlers;

/* {{{ int transliterator_object_construct( zval *object, UTransliterator *utrans, UErrorCode *status )
 * Initialize internals of Transliterator_object.
 */
U_CFUNC int transliterator_object_construct( zval *object,
									 UTransliterator *utrans,
									 UErrorCode *status )
{
	const UChar           *ustr_id;
	int32_t               ustr_id_len;
	zend_string           *u8str;
	zval                  tmp;
	Transliterator_object *to;

	TRANSLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	assert( to->utrans == nullptr );
	/* this assignment must happen before any return with failure because the
	 * caller relies on it always being made (so it can just destroy the object
	 * to close the transliterator) */
	to->utrans = utrans;

	ustr_id = utrans_getUnicodeID( utrans, &ustr_id_len );
	u8str = intl_convert_utf16_to_utf8(ustr_id, (int ) ustr_id_len, status );
	if( !u8str )
	{
		return FAILURE;
	}

	ZVAL_NEW_STR(&tmp, u8str);
	zend_update_property(Transliterator_ce_ptr, Z_OBJ_P(object),
		"id", sizeof( "id" ) - 1, &tmp );
	GC_DELREF(u8str);
	return SUCCESS;
}
/* }}} */

/*
 * Auxiliary functions needed by objects of 'Transliterator' class
 */

/* {{{ void transliterator_object_init( Transliterator_object* to )
 * Initialize internals of Transliterator_object.
 */
static void transliterator_object_init( Transliterator_object* to )
{
	if( !to )
		return;

	intl_error_init( TRANSLITERATOR_ERROR_P( to ) );
}
/* }}} */

/* {{{ void transliterator_object_destroy( Transliterator_object* to )
 * Clean up mem allocted by internals of Transliterator_object
 */
static void transliterator_object_destroy( Transliterator_object* to )
{
	if( !to )
		return;

	if( to->utrans )
	{
		utrans_close( to->utrans );
		to->utrans = nullptr;
	}

	intl_error_reset( TRANSLITERATOR_ERROR_P( to ) );
}
/* }}} */

/* {{{ Transliterator_objects_free */
static void Transliterator_objects_free( zend_object *object )
{
	Transliterator_object* to = php_intl_transliterator_fetch_object(object);

	zend_object_std_dtor( &to->zo );

	transliterator_object_destroy( to );
}
/* }}} */

/* {{{ Transliterator_object_create */
static zend_object *Transliterator_object_create( zend_class_entry *ce )
{
	Transliterator_object* intern;

	intern = reinterpret_cast<Transliterator_object *>(zend_object_alloc(sizeof(Transliterator_object), ce));

	zend_object_std_init( &intern->zo, ce );
	object_properties_init( &intern->zo, ce );
	transliterator_object_init( intern );

	return &intern->zo;
}
/* }}} */

/*
 * Object handlers for Transliterator class (and subclasses)
 */

/* {{{ clone handler for Transliterator */
static zend_object *Transliterator_clone_obj( zend_object *object )
{
	Transliterator_object *to_orig = php_intl_transliterator_fetch_object(object);
	zend_object           *ret_val = Transliterator_ce_ptr->create_object(object->ce);
	Transliterator_object  *to_new = php_intl_transliterator_fetch_object(ret_val);

	zend_objects_clone_members( &to_new->zo, &to_orig->zo );
	if (to_orig->utrans != nullptr) {
		/* guaranteed to return nullptr if it fails */
		UErrorCode error = U_ZERO_ERROR;
		UTransliterator *utrans = utrans_clone( to_orig->utrans, &error);

		if (U_FAILURE(error)) {
			if (utrans != nullptr) {
				transliterator_object_destroy(to_new);
			}
			zend_throw_error(nullptr, "Failed to clone Transliterator");
		} else {
			to_new->utrans = utrans;
		}
	} else {
		/* We shouldn't have unconstructed objects in the first place */
		zend_throw_error(nullptr, "Cannot clone uninitialized Transliterator");
	}

	return ret_val;
}
/* }}} */

/* {{{ transliterator_register_Transliterator_class
 * Initialize 'Transliterator' class
 */
U_CFUNC void transliterator_register_Transliterator_class( void )
{
	/* Create and register 'Transliterator' class. */
	Transliterator_ce_ptr = register_class_Transliterator();
	Transliterator_ce_ptr->create_object = Transliterator_object_create;
	Transliterator_ce_ptr->default_object_handlers = &Transliterator_handlers;
	memcpy( &Transliterator_handlers, &std_object_handlers, sizeof Transliterator_handlers );
	Transliterator_handlers.offset = XtOffsetOf(Transliterator_object, zo);
	Transliterator_handlers.free_obj = Transliterator_objects_free;
	Transliterator_handlers.clone_obj = Transliterator_clone_obj;
}
/* }}} */
