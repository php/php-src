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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#include <unicode/unum.h>

#include "msgformat_class.h"
#include "php_intl.h"
#include "msgformat_data.h"
#include "msgformat_arginfo.h"

#include <zend_exceptions.h>

zend_class_entry *MessageFormatter_ce_ptr = NULL;
static zend_object_handlers MessageFormatter_handlers;

/*
 * Auxiliary functions needed by objects of 'MessageFormatter' class
 */

/* {{{ MessageFormatter_objects_free */
void MessageFormatter_object_free( zend_object *object )
{
	MessageFormatter_object* mfo = php_intl_messageformatter_fetch_object(object);

	zend_object_std_dtor( &mfo->zo );

	msgformat_data_free( &mfo->mf_data );
}
/* }}} */

/* {{{ MessageFormatter_object_create */
zend_object *MessageFormatter_object_create(zend_class_entry *ce)
{
	MessageFormatter_object*     intern;

	intern = zend_object_alloc(sizeof(MessageFormatter_object), ce);
	msgformat_data_init( &intern->mf_data );
	zend_object_std_init( &intern->zo, ce );
	object_properties_init(&intern->zo, ce);

	return &intern->zo;
}
/* }}} */

/* {{{ MessageFormatter_object_clone */
zend_object *MessageFormatter_object_clone(zend_object *object)
{
	MessageFormatter_object *mfo, *new_mfo;
	zend_object *new_obj;

	mfo = php_intl_messageformatter_fetch_object(object);
	intl_error_reset(INTL_DATA_ERROR_P(mfo));

	new_obj = MessageFormatter_ce_ptr->create_object(object->ce);
	new_mfo = php_intl_messageformatter_fetch_object(new_obj);
	/* clone standard parts */
	zend_objects_clone_members(&new_mfo->zo, &mfo->zo);

	/* clone formatter object */
	if (MSG_FORMAT_OBJECT(mfo) != NULL) {
		MSG_FORMAT_OBJECT(new_mfo) = umsg_clone(MSG_FORMAT_OBJECT(mfo),
				&INTL_DATA_ERROR_CODE(mfo));

		if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
			intl_errors_set(INTL_DATA_ERROR_P(mfo), INTL_DATA_ERROR_CODE(mfo),
					"Failed to clone MessageFormatter object", 0);
			zend_throw_exception_ex(NULL, 0, "Failed to clone MessageFormatter object");
		}
	} else {
		zend_throw_exception_ex(NULL, 0, "Cannot clone unconstructed MessageFormatter");
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
