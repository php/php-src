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

extern "C" {
#include "msgformat_class.h"
#include "php_intl.h"
#include "msgformat_data.h"
#include "msgformat_arginfo.h"
}

#include <zend_exceptions.h>

zend_class_entry *MessageFormatter_ce_ptr = NULL;
static zend_object_handlers MessageFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'MessageFormatter' class
 */

/* {{{ MessageFormatter_objects_free */
U_CFUNC void MessageFormatter_object_free( zend_object *object )
{
	MessageFormatter_object* mfo = php_intl_messageformatter_fetch_object(object);

	zend_object_std_dtor( &mfo->zo );

	msgformat_data_free( &mfo->mf_data );
}
/* }}} */

/* {{{ MessageFormatter_object_create */
U_CFUNC zend_object *MessageFormatter_object_create(zend_class_entry *ce)
{
	MessageFormatter_object*     intern;

	intern = reinterpret_cast<MessageFormatter_object *>(zend_object_alloc(sizeof(MessageFormatter_object), ce));
	msgformat_data_init( &intern->mf_data );
	zend_object_std_init( &intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	return &intern->zo;
}
/* }}} */

/* {{{ MessageFormatter_object_clone */
U_CFUNC zend_object *MessageFormatter_object_clone(zend_object *object)
{
	MessageFormatter_object     *mfo = php_intl_messageformatter_fetch_object(object);
	zend_object             *new_obj = MessageFormatter_ce_ptr->create_object(object->ce);
	MessageFormatter_object *new_mfo = php_intl_messageformatter_fetch_object(new_obj);

	/* clone standard parts */
	zend_objects_clone_members(&new_mfo->zo, &mfo->zo);

	/* clone formatter object */
	if (MSG_FORMAT_OBJECT(mfo) != NULL) {
		UErrorCode error = U_ZERO_ERROR;
		MSG_FORMAT_OBJECT(new_mfo) = reinterpret_cast<UMessageFormat *>(umsg_clone(MSG_FORMAT_OBJECT(mfo), &error));

		if (U_FAILURE(error)) {
			zend_throw_error(NULL, "Failed to clone MessageFormatter");
		}
	} else {
		zend_throw_error(NULL, "Cannot clone uninitialized MessageFormatter");
	}
	return new_obj;
}
/* }}} */

/*
 * 'MessageFormatter' class registration structures & functions
 */

/* {{{ msgformat_register_class
 * Initialize 'MessageFormatter' class
 */
void msgformat_register_class( void )
{
	/* Create and register 'MessageFormatter' class. */
	MessageFormatter_ce_ptr = register_class_MessageFormatter();
	MessageFormatter_ce_ptr->create_object = MessageFormatter_object_create;
	MessageFormatter_ce_ptr->default_object_handlers = &MessageFormatter_handlers;

	memcpy(&MessageFormatter_handlers, &std_object_handlers,
		sizeof MessageFormatter_handlers);
	MessageFormatter_handlers.offset = XtOffsetOf(MessageFormatter_object, zo);
	MessageFormatter_handlers.clone_obj = MessageFormatter_object_clone;
	MessageFormatter_handlers.free_obj = MessageFormatter_object_free;
}
/* }}} */
