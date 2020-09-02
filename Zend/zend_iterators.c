/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
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

#include "zend.h"
#include "zend_API.h"

static zend_class_entry zend_iterator_class_entry;

static void iter_wrapper_free(zend_object *object);
static void iter_wrapper_dtor(zend_object *object);
static HashTable *iter_wrapper_get_gc(zend_object *object, zval **table, int *n);

static const zend_object_handlers iterator_object_handlers = {
	0,
	iter_wrapper_free,
	iter_wrapper_dtor,
	NULL, /* clone_obj */
	NULL, /* prop read */
	NULL, /* prop write */
	NULL, /* read dim */
	NULL, /* write dim */
	NULL, /* get_property_ptr_ptr */
	NULL, /* has prop */
	NULL, /* unset prop */
	NULL, /* has dim */
	NULL, /* unset dim */
	NULL, /* props get */
	NULL, /* method get */
	NULL, /* get ctor */
	NULL, /* get class name */
	NULL, /* cast */
	NULL, /* count */
	NULL, /* get_debug_info */
	NULL, /* get_closure */
	iter_wrapper_get_gc,
	NULL, /* do_operation */
	NULL, /* compare */
	NULL  /* get_properties_for */
};

ZEND_API void zend_register_iterator_wrapper(void)
{
	INIT_CLASS_ENTRY(zend_iterator_class_entry, "__iterator_wrapper", NULL);
}

static void iter_wrapper_free(zend_object *object)
{
	zend_object_iterator *iter = (zend_object_iterator*)object;
	iter->funcs->dtor(iter);
}

static void iter_wrapper_dtor(zend_object *object)
{
}

static HashTable *iter_wrapper_get_gc(zend_object *object, zval **table, int *n) {
	zend_object_iterator *iter = (zend_object_iterator*)object;
	if (iter->funcs->get_gc) {
		return iter->funcs->get_gc(iter, table, n);
	}

	*table = NULL;
	*n = 0;
	return NULL;
}

ZEND_API void zend_iterator_init(zend_object_iterator *iter)
{
	zend_object_std_init(&iter->std, &zend_iterator_class_entry);
	iter->std.handlers = &iterator_object_handlers;
}

ZEND_API void zend_iterator_dtor(zend_object_iterator *iter)
{
	if (GC_DELREF(&iter->std) > 0) {
		return;
	}

	zend_objects_store_del(&iter->std);
}

ZEND_API zend_object_iterator* zend_iterator_unwrap(zval *array_ptr)
{
	ZEND_ASSERT(Z_TYPE_P(array_ptr) == IS_OBJECT);
	if (Z_OBJ_HT_P(array_ptr) == &iterator_object_handlers) {
		return (zend_object_iterator *)Z_OBJ_P(array_ptr);
	}
	return NULL;
}
