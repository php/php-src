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

#ifndef ZEND_COLLECTION_H
#define ZEND_COLLECTION_H

#include "zend.h"

#include <stdint.h>

BEGIN_EXTERN_C()

extern ZEND_API zend_object_handlers zend_collection_object_handlers;
extern ZEND_API zend_class_entry *zend_ce_collection;

void zend_register_collection_ce(void);
void zend_collection_add_interfaces(zend_class_entry *ce);

void zend_collection_register_handlers(zend_class_entry *ce);
void zend_collection_register_props(zend_class_entry *ce);

void zend_collection_add_item(zend_object *object, zval *offset, zval *value);
zval *zend_collection_read_item(zend_object *object, zval *offset);

END_EXTERN_C()

#endif /* ZEND_COLLECTION_H */
