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
   | Authors: Derick Rethans <derick@php.net>                             |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"

ZEND_API zend_object_handlers zend_collection_object_handlers;

void zend_collection_register_handlers(zend_class_entry *ce)
{
	memcpy(&zend_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_collection_object_handlers.clone_obj = NULL;
	zend_collection_object_handlers.compare = zend_objects_not_comparable;

	ce->default_object_handlers = &zend_collection_object_handlers;
}

void zend_collection_register_props(zend_class_entry *ce)
{
	ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zval name_default_value;
	ZVAL_UNDEF(&name_default_value);
	zend_type name_type = ZEND_TYPE_INIT_CODE(IS_ARRAY, 0, 0);
	zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_VALUE), &name_default_value, ZEND_ACC_PUBLIC | ZEND_ACC_READONLY, NULL, name_type);
}
