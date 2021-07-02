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
#include "zend_API.h"
#include "zend_types.h"

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_unit_enum;
extern ZEND_API zend_class_entry *zend_ce_backed_enum;

void zend_register_enum_ce(void);
void zend_enum_add_interfaces(zend_class_entry *ce);
zend_object *zend_enum_new(zval *result, zend_class_entry *ce, zend_string *case_name, zval *backing_value_zv);
void zend_verify_enum(zend_class_entry *ce);
void zend_enum_register_funcs(zend_class_entry *ce);
void zend_enum_register_props(zend_class_entry *ce);

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

ZEND_API zend_class_entry *zend_register_internal_enum(const char *name);
ZEND_API zend_class_entry *zend_register_internal_enum_ex(const char *name, const zend_function_entry *functions);
ZEND_API zend_class_entry *zend_register_internal_backed_enum(const char *name, uint32_t backing_type);
ZEND_API zend_class_entry *zend_register_internal_backed_enum_ex(const char *name, uint32_t backing_type, const zend_function_entry *functions);

#define zend_add_enum_case(ce, name) zend_add_enum_case_ex(ce, zend_string_init(name, sizeof(name) - 1, 1))
ZEND_API zend_object *zend_add_enum_case_ex(zend_class_entry *ce, zend_string *name);

#define zend_add_enum_case_long(ce, name, long) zend_add_enum_case_long_ex(ce, zend_string_init(name, sizeof(name) - 1, 1), long)
ZEND_API zend_object *zend_add_enum_case_long_ex(zend_class_entry *ce, zend_string *name, zend_long value);

#define zend_add_enum_case_string(ce, name, str) zend_add_enum_case_string_ex(ce, zend_string_init(name, sizeof(name) - 1, 1), str)
#define zend_add_enum_case_str(ce, name, str) zend_add_enum_case_str_ex(ce, zend_string_init(name, sizeof(name) - 1, 1), str)
#define zend_add_enum_case_string_ex(ce, name, str) zend_add_enum_case_str_ex(ce, name, zend_string_init(str, sizeof(str) - 1, 1))
ZEND_API zend_object *zend_add_enum_case_str_ex(zend_class_entry *ce, zend_string *name, zend_string *str);

END_EXTERN_C()

#endif /* ZEND_ENUM_H */
