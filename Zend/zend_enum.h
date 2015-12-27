/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Bob Weinand <bobwei9@hotmail.com>                           |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"

#ifndef ZEND_ENUM_H
#define ZEND_ENUM_H

typedef struct _zend_enum_entry {
	zend_class_entry std;

	uint32_t enum_handle;
	zend_string **handle_map;
} zend_enum_entry;

static inline void zend_enum_add(zend_enum_entry *ee, zend_string *name) {
	uint32_t elements = zend_hash_num_elements(&ee->std.constants_table);
	zval value;
	ZVAL_ENUM(&value, ee->enum_handle, elements);
	zend_declare_class_constant_ex(&ee->std, name, &value, ZEND_ACC_PUBLIC, NULL);
	ee->handle_map = (zend_string **) perealloc(ee->handle_map, sizeof(zend_string *) * (elements + 1), ee->std.type == ZEND_INTERNAL_CLASS);
	ee->handle_map[elements] = name;
}

static inline zend_bool zend_enum_equals(zval *op1, zval *op2) {
	return Z_ENUM_P(op1) == Z_ENUM_P(op2);
}

static inline zend_class_entry *zend_enum_ce(zval *zv) {
	return (zend_class_entry *) zend_hash_index_find_ptr(&CG(enums), Z_ENUM_CLASS_P(zv));
}

static inline zend_string *zend_enum_name(zval *zv) {
	zend_enum_entry *ee = (zend_enum_entry *) zend_enum_ce(zv);
	return ee->handle_map[Z_ENUM_HANDLE_P(zv)];
}

#endif
