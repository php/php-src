/* This is a generated file, edit serialization.stub.php instead.
 * Stub hash: 6ed457d7d6877ab0911b52a4c22e4c1f9c85200c */

#include "zend_attributes.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZendTestSleepObject___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, public, IS_MIXED, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, protected, IS_MIXED, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, private, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZendTestSleepObject___sleep, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZendTestSleepObject___wakeup, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZendTestSleepObjectWithCustomCreate___sleep arginfo_class_ZendTestSleepObject___sleep

static ZEND_METHOD(ZendTestSleepObject, __construct);
static ZEND_METHOD(ZendTestSleepObject, __sleep);
static ZEND_METHOD(ZendTestSleepObject, __wakeup);
static ZEND_METHOD(ZendTestSleepObjectWithCustomCreate, __sleep);

static const zend_function_entry class_ZendTestSleepObject_methods[] = {
	ZEND_ME(ZendTestSleepObject, __construct, arginfo_class_ZendTestSleepObject___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ZendTestSleepObject, __sleep, arginfo_class_ZendTestSleepObject___sleep, ZEND_ACC_PUBLIC)
	ZEND_ME(ZendTestSleepObject, __wakeup, arginfo_class_ZendTestSleepObject___wakeup, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestSleepObjectWithCustomCreate_methods[] = {
	ZEND_ME(ZendTestSleepObjectWithCustomCreate, __sleep, arginfo_class_ZendTestSleepObjectWithCustomCreate___sleep, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_ZendTestSleepObject(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestSleepObject", class_ZendTestSleepObject_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES);

	zval property_public_default_value;
	ZVAL_NULL(&property_public_default_value);
	zend_string *property_public_name = zend_string_init("public", sizeof("public") - 1, true);
	zend_declare_typed_property(class_entry, property_public_name, &property_public_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release_ex(property_public_name, true);

	zval property_protected_default_value;
	ZVAL_NULL(&property_protected_default_value);
	zend_string *property_protected_name = zend_string_init("protected", sizeof("protected") - 1, true);
	zend_declare_typed_property(class_entry, property_protected_name, &property_protected_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release_ex(property_protected_name, true);

	zval property_private_default_value;
	ZVAL_NULL(&property_private_default_value);
	zend_string *property_private_name = zend_string_init("private", sizeof("private") - 1, true);
	zend_declare_typed_property(class_entry, property_private_name, &property_private_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release_ex(property_private_name, true);

	zval property_typed_default_value;
	ZVAL_LONG(&property_typed_default_value, 0);
	zend_string *property_typed_name = zend_string_init("typed", sizeof("typed") - 1, true);
	zend_declare_typed_property(class_entry, property_typed_name, &property_typed_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_typed_name, true);

	zval property_wokenUp_default_value;
	ZVAL_FALSE(&property_wokenUp_default_value);
	zend_string *property_wokenUp_name = zend_string_init("wokenUp", sizeof("wokenUp") - 1, true);
	zend_declare_typed_property(class_entry, property_wokenUp_name, &property_wokenUp_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_wokenUp_name, true);

	zend_string *attribute_name_AllowDynamicProperties_class_ZendTestSleepObject_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, true);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class_ZendTestSleepObject_0, 0);
	zend_string_release_ex(attribute_name_AllowDynamicProperties_class_ZendTestSleepObject_0, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestSleepObjectWithCustomCreate(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestSleepObjectWithCustomCreate", class_ZendTestSleepObjectWithCustomCreate_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES);

	zval property_value_default_value;
	ZVAL_NULL(&property_value_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_VALUE), &property_value_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));

	zend_string *attribute_name_AllowDynamicProperties_class_ZendTestSleepObjectWithCustomCreate_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, true);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class_ZendTestSleepObjectWithCustomCreate_0, 0);
	zend_string_release_ex(attribute_name_AllowDynamicProperties_class_ZendTestSleepObjectWithCustomCreate_0, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestLegacySerializeObject(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestLegacySerializeObject", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zval property_data_default_value;
	ZVAL_NULL(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, true);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release_ex(property_data_name, true);

	return class_entry;
}
