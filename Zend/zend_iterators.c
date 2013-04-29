/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"

static zend_class_entry zend_iterator_class_entry;

static zend_object_handlers iterator_object_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	NULL, /* prop read */
	NULL, /* prop write */
	NULL, /* read dim */
	NULL, /* write dim */
	NULL,
	NULL, /* get */
	NULL, /* set */
	NULL, /* has prop */
	NULL, /* unset prop */
	NULL, /* has dim */
	NULL, /* unset dim */
	NULL, /* props get */
	NULL, /* method get */
	NULL, /* call */
	NULL, /* get ctor */
	NULL, /* get_ce */
	NULL, /* get class name */
	NULL, /* compare */
	NULL, /* cast */
	NULL  /* count */
};

ZEND_API void zend_register_iterator_wrapper(TSRMLS_D)
{
	INIT_CLASS_ENTRY(zend_iterator_class_entry, "__iterator_wrapper", NULL);
	str_free(zend_iterator_class_entry.name);
	zend_iterator_class_entry.name = "__iterator_wrapper";
}

static void iter_wrapper_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	zend_object_iterator *iter = (zend_object_iterator*)object;
	iter->funcs->dtor(iter TSRMLS_CC);
}

ZEND_API zval *zend_iterator_wrap(zend_object_iterator *iter TSRMLS_DC)
{
	zval *wrapped;

	MAKE_STD_ZVAL(wrapped);
	Z_TYPE_P(wrapped) = IS_OBJECT;
	Z_OBJ_HANDLE_P(wrapped) = zend_objects_store_put(iter, iter_wrapper_dtor, NULL, NULL TSRMLS_CC);
	Z_OBJ_HT_P(wrapped) = &iterator_object_handlers;

	return wrapped;
}

ZEND_API enum zend_object_iterator_kind zend_iterator_unwrap(
	zval *array_ptr, zend_object_iterator **iter TSRMLS_DC)
{
	switch (Z_TYPE_P(array_ptr)) {
		case IS_OBJECT:
			if (Z_OBJ_HT_P(array_ptr) == &iterator_object_handlers) {
				*iter = (zend_object_iterator *)zend_object_store_get_object(array_ptr TSRMLS_CC);
				return ZEND_ITER_OBJECT;
			}
			if (Z_OBJPROP_P(array_ptr)) {
				return ZEND_ITER_PLAIN_OBJECT;
			}
			return ZEND_ITER_INVALID;

		case IS_ARRAY:
			if (Z_ARRVAL_P(array_ptr)) {
				return ZEND_ITER_PLAIN_ARRAY;
			}
			return ZEND_ITER_INVALID;

		default:
			return ZEND_ITER_INVALID;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
