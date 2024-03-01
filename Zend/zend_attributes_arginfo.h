/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 32f0458c20f04099e353a8300ffb19e40bc38f69 */

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

#define arginfo_class_Override___construct arginfo_class_ReturnTypeWillChange___construct

ZEND_METHOD(Attribute, __construct);
ZEND_METHOD(ReturnTypeWillChange, __construct);
ZEND_METHOD(AllowDynamicProperties, __construct);
ZEND_METHOD(SensitiveParameter, __construct);
ZEND_METHOD(SensitiveParameterValue, __construct);
ZEND_METHOD(SensitiveParameterValue, getValue);
ZEND_METHOD(SensitiveParameterValue, __debugInfo);
ZEND_METHOD(Override, __construct);

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

static const zend_function_entry class_Override_methods[] = {
	ZEND_ME(Override, __construct, arginfo_class_Override___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Attribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Attribute", class_Attribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval const_TARGET_CLASS_value;
	ZVAL_LONG(&const_TARGET_CLASS_value, ZEND_ATTRIBUTE_TARGET_CLASS);
	zend_string *const_TARGET_CLASS_name = zend_string_init_interned("TARGET_CLASS", sizeof("TARGET_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_CLASS_name, &const_TARGET_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_CLASS_name);

	zval const_TARGET_FUNCTION_value;
	ZVAL_LONG(&const_TARGET_FUNCTION_value, ZEND_ATTRIBUTE_TARGET_FUNCTION);
	zend_string *const_TARGET_FUNCTION_name = zend_string_init_interned("TARGET_FUNCTION", sizeof("TARGET_FUNCTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_FUNCTION_name, &const_TARGET_FUNCTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_FUNCTION_name);

	zval const_TARGET_METHOD_value;
	ZVAL_LONG(&const_TARGET_METHOD_value, ZEND_ATTRIBUTE_TARGET_METHOD);
	zend_string *const_TARGET_METHOD_name = zend_string_init_interned("TARGET_METHOD", sizeof("TARGET_METHOD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_METHOD_name, &const_TARGET_METHOD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_METHOD_name);

	zval const_TARGET_PROPERTY_value;
	ZVAL_LONG(&const_TARGET_PROPERTY_value, ZEND_ATTRIBUTE_TARGET_PROPERTY);
	zend_string *const_TARGET_PROPERTY_name = zend_string_init_interned("TARGET_PROPERTY", sizeof("TARGET_PROPERTY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_PROPERTY_name, &const_TARGET_PROPERTY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_PROPERTY_name);

	zval const_TARGET_CLASS_CONSTANT_value;
	ZVAL_LONG(&const_TARGET_CLASS_CONSTANT_value, ZEND_ATTRIBUTE_TARGET_CLASS_CONST);
	zend_string *const_TARGET_CLASS_CONSTANT_name = zend_string_init_interned("TARGET_CLASS_CONSTANT", sizeof("TARGET_CLASS_CONSTANT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_CLASS_CONSTANT_name, &const_TARGET_CLASS_CONSTANT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_CLASS_CONSTANT_name);

	zval const_TARGET_PARAMETER_value;
	ZVAL_LONG(&const_TARGET_PARAMETER_value, ZEND_ATTRIBUTE_TARGET_PARAMETER);
	zend_string *const_TARGET_PARAMETER_name = zend_string_init_interned("TARGET_PARAMETER", sizeof("TARGET_PARAMETER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_PARAMETER_name, &const_TARGET_PARAMETER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_PARAMETER_name);

	zval const_TARGET_ALL_value;
	ZVAL_LONG(&const_TARGET_ALL_value, ZEND_ATTRIBUTE_TARGET_ALL);
	zend_string *const_TARGET_ALL_name = zend_string_init_interned("TARGET_ALL", sizeof("TARGET_ALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TARGET_ALL_name, &const_TARGET_ALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TARGET_ALL_name);

	zval const_IS_REPEATABLE_value;
	ZVAL_LONG(&const_IS_REPEATABLE_value, ZEND_ATTRIBUTE_IS_REPEATABLE);
	zend_string *const_IS_REPEATABLE_name = zend_string_init_interned("IS_REPEATABLE", sizeof("IS_REPEATABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IS_REPEATABLE_name, &const_IS_REPEATABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IS_REPEATABLE_name);

	zval property_flags_default_value;
	ZVAL_UNDEF(&property_flags_default_value);
	zend_string *property_flags_name = zend_string_init("flags", sizeof("flags") - 1, 1);
	zend_declare_typed_property(class_entry, property_flags_name, &property_flags_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_flags_name);

	zend_string *attribute_name_Attribute_class_Attribute_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, 1);
	zend_attribute *attribute_Attribute_class_Attribute_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_Attribute_0, 1);
	zend_string_release(attribute_name_Attribute_class_Attribute_0);
	zval attribute_Attribute_class_Attribute_0_arg0;
	ZVAL_LONG(&attribute_Attribute_class_Attribute_0_arg0, ZEND_ATTRIBUTE_TARGET_CLASS);
	ZVAL_COPY_VALUE(&attribute_Attribute_class_Attribute_0->args[0].value, &attribute_Attribute_class_Attribute_0_arg0);

	return class_entry;
}

static zend_class_entry *register_class_ReturnTypeWillChange(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ReturnTypeWillChange", class_ReturnTypeWillChange_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zend_string *attribute_name_Attribute_class_ReturnTypeWillChange_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, 1);
	zend_attribute *attribute_Attribute_class_ReturnTypeWillChange_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_ReturnTypeWillChange_0, 1);
	zend_string_release(attribute_name_Attribute_class_ReturnTypeWillChange_0);
	zval attribute_Attribute_class_ReturnTypeWillChange_0_arg0;
	ZVAL_LONG(&attribute_Attribute_class_ReturnTypeWillChange_0_arg0, ZEND_ATTRIBUTE_TARGET_METHOD);
	ZVAL_COPY_VALUE(&attribute_Attribute_class_ReturnTypeWillChange_0->args[0].value, &attribute_Attribute_class_ReturnTypeWillChange_0_arg0);

	return class_entry;
}

static zend_class_entry *register_class_AllowDynamicProperties(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "AllowDynamicProperties", class_AllowDynamicProperties_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zend_string *attribute_name_Attribute_class_AllowDynamicProperties_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, 1);
	zend_attribute *attribute_Attribute_class_AllowDynamicProperties_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_AllowDynamicProperties_0, 1);
	zend_string_release(attribute_name_Attribute_class_AllowDynamicProperties_0);
	zval attribute_Attribute_class_AllowDynamicProperties_0_arg0;
	ZVAL_LONG(&attribute_Attribute_class_AllowDynamicProperties_0_arg0, ZEND_ATTRIBUTE_TARGET_CLASS);
	ZVAL_COPY_VALUE(&attribute_Attribute_class_AllowDynamicProperties_0->args[0].value, &attribute_Attribute_class_AllowDynamicProperties_0_arg0);

	return class_entry;
}

static zend_class_entry *register_class_SensitiveParameter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SensitiveParameter", class_SensitiveParameter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_Attribute_class_SensitiveParameter_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, 1);
	zend_attribute *attribute_Attribute_class_SensitiveParameter_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_SensitiveParameter_0, 1);
	zend_string_release(attribute_name_Attribute_class_SensitiveParameter_0);
	zval attribute_Attribute_class_SensitiveParameter_0_arg0;
	ZVAL_LONG(&attribute_Attribute_class_SensitiveParameter_0_arg0, ZEND_ATTRIBUTE_TARGET_PARAMETER);
	ZVAL_COPY_VALUE(&attribute_Attribute_class_SensitiveParameter_0->args[0].value, &attribute_Attribute_class_SensitiveParameter_0_arg0);

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

static zend_class_entry *register_class_Override(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Override", class_Override_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_Attribute_class_Override_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, 1);
	zend_attribute *attribute_Attribute_class_Override_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_Override_0, 1);
	zend_string_release(attribute_name_Attribute_class_Override_0);
	zval attribute_Attribute_class_Override_0_arg0;
	ZVAL_LONG(&attribute_Attribute_class_Override_0_arg0, ZEND_ATTRIBUTE_TARGET_METHOD);
	ZVAL_COPY_VALUE(&attribute_Attribute_class_Override_0->args[0].value, &attribute_Attribute_class_Override_0_arg0);

	return class_entry;
}
