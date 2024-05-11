/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 81be60f2c465ffe5c036739d072ab80d9c388907 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DoOperationNoCast___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, val, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_LongCastableNoOperations___construct arginfo_class_DoOperationNoCast___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FloatCastableNoOperations___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, val, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_NumericCastableNoOperations___construct, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, val, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
ZEND_END_ARG_INFO()

static ZEND_METHOD(DoOperationNoCast, __construct);
static ZEND_METHOD(LongCastableNoOperations, __construct);
static ZEND_METHOD(FloatCastableNoOperations, __construct);
static ZEND_METHOD(NumericCastableNoOperations, __construct);

static const zend_function_entry class_DoOperationNoCast_methods[] = {
	ZEND_ME(DoOperationNoCast, __construct, arginfo_class_DoOperationNoCast___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_LongCastableNoOperations_methods[] = {
	ZEND_ME(LongCastableNoOperations, __construct, arginfo_class_LongCastableNoOperations___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_FloatCastableNoOperations_methods[] = {
	ZEND_ME(FloatCastableNoOperations, __construct, arginfo_class_FloatCastableNoOperations___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_NumericCastableNoOperations_methods[] = {
	ZEND_ME(NumericCastableNoOperations, __construct, arginfo_class_NumericCastableNoOperations___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DimensionHandlersNoArrayAccess_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_DoOperationNoCast(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DoOperationNoCast", class_DoOperationNoCast_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_val_default_value;
	ZVAL_UNDEF(&property_val_default_value);
	zend_string *property_val_name = zend_string_init("val", sizeof("val") - 1, 1);
	zend_declare_typed_property(class_entry, property_val_name, &property_val_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_val_name);

	return class_entry;
}

static zend_class_entry *register_class_LongCastableNoOperations(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LongCastableNoOperations", class_LongCastableNoOperations_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_val_default_value;
	ZVAL_UNDEF(&property_val_default_value);
	zend_string *property_val_name = zend_string_init("val", sizeof("val") - 1, 1);
	zend_declare_typed_property(class_entry, property_val_name, &property_val_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_val_name);

	return class_entry;
}

static zend_class_entry *register_class_FloatCastableNoOperations(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FloatCastableNoOperations", class_FloatCastableNoOperations_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_val_default_value;
	ZVAL_UNDEF(&property_val_default_value);
	zend_string *property_val_name = zend_string_init("val", sizeof("val") - 1, 1);
	zend_declare_typed_property(class_entry, property_val_name, &property_val_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(property_val_name);

	return class_entry;
}

static zend_class_entry *register_class_NumericCastableNoOperations(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "NumericCastableNoOperations", class_NumericCastableNoOperations_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_val_default_value;
	ZVAL_UNDEF(&property_val_default_value);
	zend_string *property_val_name = zend_string_init("val", sizeof("val") - 1, 1);
	zend_declare_typed_property(class_entry, property_val_name, &property_val_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_DOUBLE));
	zend_string_release(property_val_name);

	return class_entry;
}

static zend_class_entry *register_class_DimensionHandlersNoArrayAccess(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DimensionHandlersNoArrayAccess", class_DimensionHandlersNoArrayAccess_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_read_default_value;
	ZVAL_FALSE(&property_read_default_value);
	zend_string *property_read_name = zend_string_init("read", sizeof("read") - 1, 1);
	zend_declare_typed_property(class_entry, property_read_name, &property_read_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_read_name);

	zval property_write_default_value;
	ZVAL_FALSE(&property_write_default_value);
	zend_string *property_write_name = zend_string_init("write", sizeof("write") - 1, 1);
	zend_declare_typed_property(class_entry, property_write_name, &property_write_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_write_name);

	zval property_has_default_value;
	ZVAL_FALSE(&property_has_default_value);
	zend_string *property_has_name = zend_string_init("has", sizeof("has") - 1, 1);
	zend_declare_typed_property(class_entry, property_has_name, &property_has_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_has_name);

	zval property_unset_default_value;
	ZVAL_FALSE(&property_unset_default_value);
	zend_string *property_unset_name = zend_string_init("unset", sizeof("unset") - 1, 1);
	zend_declare_typed_property(class_entry, property_unset_name, &property_unset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_unset_name);

	zval property_readType_default_value;
	ZVAL_UNDEF(&property_readType_default_value);
	zend_string *property_readType_name = zend_string_init("readType", sizeof("readType") - 1, 1);
	zend_declare_typed_property(class_entry, property_readType_name, &property_readType_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_readType_name);

	zval property_hasOffset_default_value;
	ZVAL_FALSE(&property_hasOffset_default_value);
	zend_string *property_hasOffset_name = zend_string_init("hasOffset", sizeof("hasOffset") - 1, 1);
	zend_declare_typed_property(class_entry, property_hasOffset_name, &property_hasOffset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_hasOffset_name);

	zval property_checkEmpty_default_value;
	ZVAL_UNDEF(&property_checkEmpty_default_value);
	zend_string *property_checkEmpty_name = zend_string_init("checkEmpty", sizeof("checkEmpty") - 1, 1);
	zend_declare_typed_property(class_entry, property_checkEmpty_name, &property_checkEmpty_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_checkEmpty_name);

	zval property_offset_default_value;
	ZVAL_UNDEF(&property_offset_default_value);
	zend_string *property_offset_name = zend_string_init("offset", sizeof("offset") - 1, 1);
	zend_declare_typed_property(class_entry, property_offset_name, &property_offset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_offset_name);

	return class_entry;
}
