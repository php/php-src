#include "zend.h"
#include "zend_API.h"
#include "zend_attributes.h"

ZEND_API zend_class_entry *zend_ce_php_attribute;

static HashTable internal_validators;

void zend_attribute_validate_phpattribute(zend_attribute *attr, int target)
{
	if (target != ZEND_ATTRIBUTE_TARGET_CLASS) {
		zend_error(E_COMPILE_ERROR, "Only classes can be marked with <<PhpAttribute>>");
	}
}

ZEND_API zend_attributes_internal_validator zend_attribute_get_validator(zend_string *lcname)
{
	return zend_hash_find_ptr(&internal_validators, lcname);
}

ZEND_API void zend_attribute_free(zend_attribute *attr, int persistent)
{
	uint32_t i;

	zend_string_release(attr->name);
	zend_string_release(attr->lcname);

	for (i = 0; i < attr->argc; i++) {
		zval_ptr_dtor(&attr->argv[i]);
	}

	pefree(attr, persistent);
}

static zend_attribute *get_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && zend_string_equals(attr->lcname, lcname)) {
				return attr;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

static zend_attribute *get_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && ZSTR_LEN(attr->lcname) == len) {
				if (0 == memcmp(ZSTR_VAL(attr->lcname), str, len)) {
					return attr;
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

ZEND_API zend_attribute *zend_get_attribute(HashTable *attributes, zend_string *lcname)
{
	return get_attribute(attributes, lcname, 0);
}

ZEND_API zend_attribute *zend_get_attribute_str(HashTable *attributes, const char *str, size_t len)
{
	return get_attribute_str(attributes, str, len, 0);
}

ZEND_API zend_attribute *zend_get_parameter_attribute(HashTable *attributes, zend_string *lcname, uint32_t offset)
{
	return get_attribute(attributes, lcname, offset + 1);
}

ZEND_API zend_attribute *zend_get_parameter_attribute_str(HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	return get_attribute_str(attributes, str, len, offset + 1);
}

static void attribute_ptr_dtor(zval *v)
{
	zend_attribute_free((zend_attribute *) Z_PTR_P(v), 1);
}

ZEND_API void zend_compiler_attribute_register(zend_class_entry *ce, zend_attributes_internal_validator validator)
{
	if (ce->type != ZEND_INTERNAL_CLASS) {
		zend_error_noreturn(E_ERROR, "Only internal classes can be registered as compiler attribute");
	}

	zend_string *lcname = zend_string_tolower_ex(ce->name, 1);

	zend_hash_update_ptr(&internal_validators, lcname, validator);
	zend_string_release(lcname);

	if (ce->attributes == NULL) {
		ce->attributes = pemalloc(sizeof(HashTable), 1);
		zend_hash_init(ce->attributes, 8, NULL, attribute_ptr_dtor, 1);
	}

	zend_attribute *attr = pemalloc(ZEND_ATTRIBUTE_SIZE(0), 1);

	attr->name = zend_string_copy(zend_ce_php_attribute->name);
	attr->lcname = zend_string_tolower_ex(attr->name, 1);
	attr->offset = 0;
	attr->argc = 0;

	zend_hash_next_index_insert_ptr(ce->attributes, attr);
}

void zend_register_attribute_ce(void)
{
	zend_hash_init(&internal_validators, 8, NULL, NULL, 1);

	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PhpAttribute", NULL);
	zend_ce_php_attribute = zend_register_internal_class(&ce);
	zend_ce_php_attribute->ce_flags |= ZEND_ACC_FINAL;

	zend_compiler_attribute_register(zend_ce_php_attribute, zend_attribute_validate_phpattribute);
}
