/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5d9a092c1f0da5f32d9a161cc5166ed794ffe8e9 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Attribute___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Attribute::TARGET_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ReturnTypeWillChange___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_AllowDynamicProperties___construct arginfo_class_ReturnTypeWillChange___construct

#define arginfo_class_SensitiveParameter___construct arginfo_class_ReturnTypeWillChange___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SensitiveParameterValue___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SensitiveParameterValue_getValue, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SensitiveParameterValue___debugInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Attribute, __construct);
ZEND_METHOD(ReturnTypeWillChange, __construct);
ZEND_METHOD(AllowDynamicProperties, __construct);
ZEND_METHOD(SensitiveParameter, __construct);
ZEND_METHOD(SensitiveParameterValue, __construct);
ZEND_METHOD(SensitiveParameterValue, getValue);
ZEND_METHOD(SensitiveParameterValue, __debugInfo);


static const zend_function_entry class_Attribute_methods[] = {
	ZEND_ME(Attribute, __construct, arginfo_class_Attribute___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ReturnTypeWillChange_methods[] = {
	ZEND_ME(ReturnTypeWillChange, __construct, arginfo_class_ReturnTypeWillChange___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_AllowDynamicProperties_methods[] = {
	ZEND_ME(AllowDynamicProperties, __construct, arginfo_class_AllowDynamicProperties___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SensitiveParameter_methods[] = {
	ZEND_ME(SensitiveParameter, __construct, arginfo_class_SensitiveParameter___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SensitiveParameterValue_methods[] = {
	ZEND_ME(SensitiveParameterValue, __construct, arginfo_class_SensitiveParameterValue___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SensitiveParameterValue, getValue, arginfo_class_SensitiveParameterValue_getValue, ZEND_ACC_PUBLIC)
	ZEND_ME(SensitiveParameterValue, __debugInfo, arginfo_class_SensitiveParameterValue___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Attribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Attribute", class_Attribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_flags_default_value;
	ZVAL_UNDEF(&property_flags_default_value);
	zend_string *property_flags_name = zend_string_init("flags", sizeof("flags") - 1, 1);
	zend_declare_typed_property(class_entry, property_flags_name, &property_flags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_flags_name);

	return class_entry;
}

static zend_class_entry *register_class_ReturnTypeWillChange(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ReturnTypeWillChange", class_ReturnTypeWillChange_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_AllowDynamicProperties(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "AllowDynamicProperties", class_AllowDynamicProperties_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_SensitiveParameter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SensitiveParameter", class_SensitiveParameter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return class_entry;
}

static zend_class_entry *register_class_SensitiveParameterValue(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SensitiveParameterValue", class_SensitiveParameterValue_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_value_name);

	return class_entry;
}
