#ifndef ZEND_ATTRIBUTES_H
#define ZEND_ATTRIBUTES_H

#define ZEND_ATTRIBUTE_TARGET_CLASS			(1<<0)
#define ZEND_ATTRIBUTE_TARGET_FUNCTION		(1<<1)
#define ZEND_ATTRIBUTE_TARGET_METHOD		(1<<2)
#define ZEND_ATTRIBUTE_TARGET_PROPERTY		(1<<3)
#define ZEND_ATTRIBUTE_TARGET_CLASS_CONST	(1<<4)
#define ZEND_ATTRIBUTE_TARGET_PARAMETER		(1<<5)
#define ZEND_ATTRIBUTE_TARGET_ALL			(1<<6)

#define ZEND_ATTRIBUTE_SIZE(argc) (sizeof(zend_attribute) + sizeof(zval) * (argc) - sizeof(zval))

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_php_attribute;

typedef struct _zend_attribute {
	zend_string *name;
	zend_string *lcname;
	/* Parameter offsets start at 1, everything else uses 0. */
	uint32_t offset;
	uint32_t argc;
	zval argv[1];
} zend_attribute;

typedef void (*zend_attributes_internal_validator)(zend_attribute *attr, int target);

ZEND_API zend_attribute *zend_get_attribute(HashTable *attributes, zend_string *lcname);
ZEND_API zend_attribute *zend_get_attribute_str(HashTable *attributes, const char *str, size_t len);

ZEND_API zend_attribute *zend_get_parameter_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset);
ZEND_API zend_attribute *zend_get_parameter_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset);

ZEND_API void zend_compiler_attribute_register(zend_class_entry *ce, zend_attributes_internal_validator validator);
ZEND_API zend_attributes_internal_validator zend_attribute_get_validator(zend_string *lcname);

ZEND_API zend_attribute *zend_add_attribute(HashTable **attributes, zend_bool persistent, uint32_t offset, zend_string *name, uint32_t argc);

END_EXTERN_C()

static zend_always_inline zend_attribute *zend_add_class_attribute(zend_class_entry *ce, zend_string *name, uint32_t argc)
{
	return zend_add_attribute(&ce->attributes, ce->type != ZEND_USER_CLASS, 0, name, argc);
}

static zend_always_inline zend_attribute *zend_add_function_attribute(zend_function *func, zend_string *name, uint32_t argc)
{
	return zend_add_attribute(&func->common.attributes, func->common.type != ZEND_USER_FUNCTION, 0, name, argc);
}

static zend_always_inline zend_attribute *zend_add_parameter_attribute(zend_function *func, uint32_t offset, zend_string *name, uint32_t argc)
{
	return zend_add_attribute(&func->common.attributes, func->common.type != ZEND_USER_FUNCTION, offset + 1, name, argc);
}

static zend_always_inline zend_attribute *zend_add_property_attribute(zend_class_entry *ce, zend_property_info *info, zend_string *name, uint32_t argc)
{
	return zend_add_attribute(&info->attributes, ce->type != ZEND_USER_CLASS, 0, name, argc);
}

static zend_always_inline zend_attribute *zend_add_class_constant_attribute(zend_class_entry *ce, zend_class_constant *c, zend_string *name, uint32_t argc)
{
	return zend_add_attribute(&c->attributes, ce->type != ZEND_USER_CLASS, 0, name, argc);
}

void zend_register_attribute_ce(void);
void zend_attributes_shutdown(void);

#endif
