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
   | Authors: Ilija Tovilo <ilutov@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ENUM_H
#define ZEND_ENUM_H

#include "zend.h"
#include "zend_types.h"

#include <stdint.h>

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_unit_enum;
extern ZEND_API zend_class_entry *zend_ce_backed_enum;
extern ZEND_API zend_object_handlers zend_enum_object_handlers;

void zend_register_enum_ce(void);
void zend_enum_add_interfaces(zend_class_entry *ce);
zend_result zend_enum_build_backed_enum_table(zend_class_entry *ce);
zend_object *zend_enum_new(zval *result, zend_class_entry *ce, zend_string *case_name, zval *backing_value_zv);
void zend_verify_enum(zend_class_entry *ce);
void zend_enum_register_funcs(zend_class_entry *ce);
void zend_enum_register_props(zend_class_entry *ce);

ZEND_API zend_class_entry *zend_register_internal_enum(
	const char *name, uint8_t type, const zend_function_entry *functions);
ZEND_API void zend_enum_add_case(zend_class_entry *ce, zend_string *case_name, zval *value);
ZEND_API void zend_enum_add_case_cstr(zend_class_entry *ce, const char *name, zval *value);
ZEND_API zend_object *zend_enum_get_case(zend_class_entry *ce, zend_string *name);
ZEND_API zend_object *zend_enum_get_case_cstr(zend_class_entry *ce, const char *name);
ZEND_API zend_result zend_enum_get_case_by_value(zend_object **result, zend_class_entry *ce, zend_long long_key, zend_string *string_key, bool try_from);

static zend_always_inline zval *zend_enum_fetch_case_name(zend_object *zobj)
{
	ZEND_ASSERT(zobj->ce->ce_flags & ZEND_ACC_ENUM);
	return OBJ_PROP_NUM(zobj, 0);
}

static zend_always_inline zval *zend_enum_fetch_case_value(zend_object *zobj)
{
	ZEND_ASSERT(zobj->ce->ce_flags & ZEND_ACC_ENUM);
	ZEND_ASSERT(zobj->ce->enum_backing_type != IS_UNDEF);
	return OBJ_PROP_NUM(zobj, 1);
}

END_EXTERN_C()

#endif /* ZEND_ENUM_H */
