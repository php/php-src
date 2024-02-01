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
   | Authors: Benjamin Eberlei <kontakt@beberlei.de>                      |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ATTRIBUTES_H
#define ZEND_ATTRIBUTES_H

#define ZEND_ATTRIBUTE_TARGET_CLASS			(1<<0)
#define ZEND_ATTRIBUTE_TARGET_FUNCTION		(1<<1)
#define ZEND_ATTRIBUTE_TARGET_METHOD		(1<<2)
#define ZEND_ATTRIBUTE_TARGET_PROPERTY		(1<<3)
#define ZEND_ATTRIBUTE_TARGET_CLASS_CONST	(1<<4)
#define ZEND_ATTRIBUTE_TARGET_PARAMETER		(1<<5)
#define ZEND_ATTRIBUTE_TARGET_ALL			((1<<6) - 1)
#define ZEND_ATTRIBUTE_IS_REPEATABLE		(1<<6)
#define ZEND_ATTRIBUTE_FLAGS				((1<<7) - 1)

/* Flags for zend_attribute.flags */
#define ZEND_ATTRIBUTE_PERSISTENT   (1<<0)
#define ZEND_ATTRIBUTE_STRICT_TYPES (1<<1)

#define ZEND_ATTRIBUTE_SIZE(argc) \
	(sizeof(zend_attribute) + sizeof(zend_attribute_arg) * (argc) - sizeof(zend_attribute_arg))

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_attribute;
extern ZEND_API zend_class_entry *zend_ce_allow_dynamic_properties;
extern ZEND_API zend_class_entry *zend_ce_sensitive_parameter;
extern ZEND_API zend_class_entry *zend_ce_sensitive_parameter_value;
extern ZEND_API zend_class_entry *zend_ce_override;

typedef struct {
	zend_string *name;
	zval value;
} zend_attribute_arg;

typedef struct _zend_attribute {
	zend_string *name;
	zend_string *lcname;
	uint32_t flags;
	uint32_t lineno;
	/* Parameter offsets start at 1, everything else uses 0. */
	uint32_t offset;
	uint32_t argc;
	zend_attribute_arg args[1] ZEND_ELEMENT_COUNT(argc);
} zend_attribute;

typedef struct _zend_internal_attribute {
	zend_class_entry *ce;
	uint32_t flags;
	void (*validator)(zend_attribute *attr, uint32_t target, zend_class_entry *scope);
} zend_internal_attribute;

ZEND_API zend_attribute *zend_get_attribute(HashTable *attributes, zend_string *lcname);
ZEND_API zend_attribute *zend_get_attribute_str(HashTable *attributes, const char *str, size_t len);

ZEND_API zend_attribute *zend_get_parameter_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset);
ZEND_API zend_attribute *zend_get_parameter_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset);

ZEND_API zend_result zend_get_attribute_value(zval *ret, zend_attribute *attr, uint32_t i, zend_class_entry *scope);

ZEND_API zend_string *zend_get_attribute_target_names(uint32_t targets);
ZEND_API bool zend_is_attribute_repeated(HashTable *attributes, zend_attribute *attr);

ZEND_API zend_internal_attribute *zend_mark_internal_attribute(zend_class_entry *ce);
ZEND_API zend_internal_attribute *zend_internal_attribute_register(zend_class_entry *ce, uint32_t flags);
ZEND_API zend_internal_attribute *zend_internal_attribute_get(zend_string *lcname);

ZEND_API zend_attribute *zend_add_attribute(
		HashTable **attributes, zend_string *name, uint32_t argc,
		uint32_t flags, uint32_t offset, uint32_t lineno);

END_EXTERN_C()

static zend_always_inline zend_attribute *zend_add_class_attribute(zend_class_entry *ce, zend_string *name, uint32_t argc)
{
	uint32_t flags = ce->type != ZEND_USER_CLASS ? ZEND_ATTRIBUTE_PERSISTENT : 0;
	return zend_add_attribute(&ce->attributes, name, argc, flags, 0, 0);
}

static zend_always_inline zend_attribute *zend_add_function_attribute(zend_function *func, zend_string *name, uint32_t argc)
{
	uint32_t flags = func->common.type != ZEND_USER_FUNCTION ? ZEND_ATTRIBUTE_PERSISTENT : 0;
	return zend_add_attribute(&func->common.attributes, name, argc, flags, 0, 0);
}

static zend_always_inline zend_attribute *zend_add_parameter_attribute(zend_function *func, uint32_t offset, zend_string *name, uint32_t argc)
{
	uint32_t flags = func->common.type != ZEND_USER_FUNCTION ? ZEND_ATTRIBUTE_PERSISTENT : 0;
	return zend_add_attribute(&func->common.attributes, name, argc, flags, offset + 1, 0);
}

static zend_always_inline zend_attribute *zend_add_property_attribute(zend_class_entry *ce, zend_property_info *info, zend_string *name, uint32_t argc)
{
	uint32_t flags = ce->type != ZEND_USER_CLASS ? ZEND_ATTRIBUTE_PERSISTENT : 0;
	return zend_add_attribute(&info->attributes, name, argc, flags, 0, 0);
}

static zend_always_inline zend_attribute *zend_add_class_constant_attribute(zend_class_entry *ce, zend_class_constant *c, zend_string *name, uint32_t argc)
{
	uint32_t flags = ce->type != ZEND_USER_CLASS ? ZEND_ATTRIBUTE_PERSISTENT : 0;
	return zend_add_attribute(&c->attributes, name, argc, flags, 0, 0);
}

void zend_register_attribute_ce(void);
void zend_attributes_shutdown(void);

#endif
