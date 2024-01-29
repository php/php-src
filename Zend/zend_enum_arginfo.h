/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7092f1d4ba651f077cff37050899f090f00abf22 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UnitEnum_cases, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BackedEnum_from, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BackedEnum_tryFrom, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()


static const zend_function_entry class_UnitEnum_methods[] = {
	ZEND_RAW_FENTRY("cases", NULL, arginfo_class_UnitEnum_cases, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_BackedEnum_methods[] = {
	ZEND_RAW_FENTRY("from", NULL, arginfo_class_BackedEnum_from, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("tryFrom", NULL, arginfo_class_BackedEnum_tryFrom, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_UnitEnum(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "UnitEnum", class_UnitEnum_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_BackedEnum(zend_class_entry *class_entry_UnitEnum)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "BackedEnum", class_BackedEnum_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_UnitEnum);

	return class_entry;
}
