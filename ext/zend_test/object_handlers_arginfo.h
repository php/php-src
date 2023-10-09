/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 164cdd464289c8db351f4ec49979a66d44ba3e87 */

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
