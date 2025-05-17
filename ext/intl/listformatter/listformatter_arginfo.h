/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f64f4171cfe4f66f976b9350b0a0e22269301ce5 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlListFormatter___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlListFormatter::TYPE_AND")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, width, IS_LONG, 0, "IntlListFormatter::WIDTH_WIDE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlListFormatter_format, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, strings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlListFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlListFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(IntlListFormatter, __construct);
ZEND_METHOD(IntlListFormatter, format);
ZEND_METHOD(IntlListFormatter, getErrorCode);
ZEND_METHOD(IntlListFormatter, getErrorMessage);

static const zend_function_entry class_IntlListFormatter_methods[] = {
	ZEND_ME(IntlListFormatter, __construct, arginfo_class_IntlListFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlListFormatter, format, arginfo_class_IntlListFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlListFormatter, getErrorCode, arginfo_class_IntlListFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlListFormatter, getErrorMessage, arginfo_class_IntlListFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlListFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlListFormatter", class_IntlListFormatter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval const_TYPE_AND_value;
	ZVAL_LONG(&const_TYPE_AND_value, ULISTFMT_TYPE_AND);
	zend_string *const_TYPE_AND_name = zend_string_init_interned("TYPE_AND", sizeof("TYPE_AND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_AND_name, &const_TYPE_AND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_AND_name);
#if U_ICU_VERSION_MAJOR_NUM >= 67

	zval const_TYPE_OR_value;
	ZVAL_LONG(&const_TYPE_OR_value, ULISTFMT_TYPE_OR);
	zend_string *const_TYPE_OR_name = zend_string_init_interned("TYPE_OR", sizeof("TYPE_OR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_OR_name, &const_TYPE_OR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_OR_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 67

	zval const_TYPE_UNITS_value;
	ZVAL_LONG(&const_TYPE_UNITS_value, ULISTFMT_TYPE_UNITS);
	zend_string *const_TYPE_UNITS_name = zend_string_init_interned("TYPE_UNITS", sizeof("TYPE_UNITS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_UNITS_name, &const_TYPE_UNITS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_UNITS_name);
#endif

	zval const_WIDTH_WIDE_value;
	ZVAL_LONG(&const_WIDTH_WIDE_value, ULISTFMT_WIDTH_WIDE);
	zend_string *const_WIDTH_WIDE_name = zend_string_init_interned("WIDTH_WIDE", sizeof("WIDTH_WIDE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WIDTH_WIDE_name, &const_WIDTH_WIDE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WIDTH_WIDE_name);
#if U_ICU_VERSION_MAJOR_NUM >= 67

	zval const_WIDTH_SHORT_value;
	ZVAL_LONG(&const_WIDTH_SHORT_value, ULISTFMT_WIDTH_SHORT);
	zend_string *const_WIDTH_SHORT_name = zend_string_init_interned("WIDTH_SHORT", sizeof("WIDTH_SHORT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WIDTH_SHORT_name, &const_WIDTH_SHORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WIDTH_SHORT_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 67

	zval const_WIDTH_NARROW_value;
	ZVAL_LONG(&const_WIDTH_NARROW_value, ULISTFMT_WIDTH_NARROW);
	zend_string *const_WIDTH_NARROW_name = zend_string_init_interned("WIDTH_NARROW", sizeof("WIDTH_NARROW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WIDTH_NARROW_name, &const_WIDTH_NARROW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WIDTH_NARROW_name);
#endif

	return class_entry;
}
