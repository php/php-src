/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3fd949e1b9f49666bed3081ed1e8e711acd9f49c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Attribute___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Attribute::TARGET_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ReturnTypeWillChange___construct, 0, 0, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Attribute, __construct);
ZEND_METHOD(ReturnTypeWillChange, __construct);


static const zend_function_entry class_Attribute_methods[] = {
	ZEND_ME(Attribute, __construct, arginfo_class_Attribute___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ReturnTypeWillChange_methods[] = {
	ZEND_ME(ReturnTypeWillChange, __construct, arginfo_class_ReturnTypeWillChange___construct, ZEND_ACC_PUBLIC)
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
