/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7029642524e32984e893e1e050a5e0bbf275c416 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlNumberRangeFormatter___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_IntlNumberRangeFormatter_createFromSkeleton, 0, 4, IntlNumberRangeFormatter, 0)
	ZEND_ARG_TYPE_INFO(0, skeleton, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, collapse, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, identityFallback, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlNumberRangeFormatter_format, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_DOUBLE|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_DOUBLE|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlNumberRangeFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlNumberRangeFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(IntlNumberRangeFormatter, __construct);
ZEND_METHOD(IntlNumberRangeFormatter, createFromSkeleton);
ZEND_METHOD(IntlNumberRangeFormatter, format);
ZEND_METHOD(IntlNumberRangeFormatter, getErrorCode);
ZEND_METHOD(IntlNumberRangeFormatter, getErrorMessage);

static const zend_function_entry class_IntlNumberRangeFormatter_methods[] = {
	ZEND_ME(IntlNumberRangeFormatter, __construct, arginfo_class_IntlNumberRangeFormatter___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(IntlNumberRangeFormatter, createFromSkeleton, arginfo_class_IntlNumberRangeFormatter_createFromSkeleton, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlNumberRangeFormatter, format, arginfo_class_IntlNumberRangeFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlNumberRangeFormatter, getErrorCode, arginfo_class_IntlNumberRangeFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlNumberRangeFormatter, getErrorMessage, arginfo_class_IntlNumberRangeFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlNumberRangeFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlNumberRangeFormatter", class_IntlNumberRangeFormatter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
#if U_ICU_VERSION_MAJOR_NUM >= 63

	zval const_COLLAPSE_AUTO_value;
	ZVAL_LONG(&const_COLLAPSE_AUTO_value, UNUM_RANGE_COLLAPSE_AUTO);
	zend_string *const_COLLAPSE_AUTO_name = zend_string_init_interned("COLLAPSE_AUTO", sizeof("COLLAPSE_AUTO") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_AUTO_name, &const_COLLAPSE_AUTO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_AUTO_name, true);

	zval const_COLLAPSE_NONE_value;
	ZVAL_LONG(&const_COLLAPSE_NONE_value, UNUM_RANGE_COLLAPSE_NONE);
	zend_string *const_COLLAPSE_NONE_name = zend_string_init_interned("COLLAPSE_NONE", sizeof("COLLAPSE_NONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_NONE_name, &const_COLLAPSE_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_NONE_name, true);

	zval const_COLLAPSE_UNIT_value;
	ZVAL_LONG(&const_COLLAPSE_UNIT_value, UNUM_RANGE_COLLAPSE_UNIT);
	zend_string *const_COLLAPSE_UNIT_name = zend_string_init_interned("COLLAPSE_UNIT", sizeof("COLLAPSE_UNIT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_UNIT_name, &const_COLLAPSE_UNIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_UNIT_name, true);

	zval const_COLLAPSE_ALL_value;
	ZVAL_LONG(&const_COLLAPSE_ALL_value, UNUM_RANGE_COLLAPSE_ALL);
	zend_string *const_COLLAPSE_ALL_name = zend_string_init_interned("COLLAPSE_ALL", sizeof("COLLAPSE_ALL") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_ALL_name, &const_COLLAPSE_ALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_ALL_name, true);

	zval const_IDENTITY_FALLBACK_SINGLE_VALUE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_SINGLE_VALUE_value, UNUM_IDENTITY_FALLBACK_SINGLE_VALUE);
	zend_string *const_IDENTITY_FALLBACK_SINGLE_VALUE_name = zend_string_init_interned("IDENTITY_FALLBACK_SINGLE_VALUE", sizeof("IDENTITY_FALLBACK_SINGLE_VALUE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_SINGLE_VALUE_name, &const_IDENTITY_FALLBACK_SINGLE_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_SINGLE_VALUE_name, true);

	zval const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value, UNUM_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE);
	zend_string *const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name = zend_string_init_interned("IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE", sizeof("IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name, &const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name, true);

	zval const_IDENTITY_FALLBACK_APPROXIMATELY_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_APPROXIMATELY_value, UNUM_IDENTITY_FALLBACK_APPROXIMATELY);
	zend_string *const_IDENTITY_FALLBACK_APPROXIMATELY_name = zend_string_init_interned("IDENTITY_FALLBACK_APPROXIMATELY", sizeof("IDENTITY_FALLBACK_APPROXIMATELY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_APPROXIMATELY_name, &const_IDENTITY_FALLBACK_APPROXIMATELY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_APPROXIMATELY_name, true);

	zval const_IDENTITY_FALLBACK_RANGE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_RANGE_value, UNUM_IDENTITY_FALLBACK_RANGE);
	zend_string *const_IDENTITY_FALLBACK_RANGE_name = zend_string_init_interned("IDENTITY_FALLBACK_RANGE", sizeof("IDENTITY_FALLBACK_RANGE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_RANGE_name, &const_IDENTITY_FALLBACK_RANGE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_RANGE_name, true);
#endif
#if !(U_ICU_VERSION_MAJOR_NUM >= 63)

	zval const_COLLAPSE_AUTO_value;
	ZVAL_LONG(&const_COLLAPSE_AUTO_value, 0);
	zend_string *const_COLLAPSE_AUTO_name = zend_string_init_interned("COLLAPSE_AUTO", sizeof("COLLAPSE_AUTO") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_AUTO_name, &const_COLLAPSE_AUTO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_AUTO_name, true);

	zval const_COLLAPSE_NONE_value;
	ZVAL_LONG(&const_COLLAPSE_NONE_value, 1);
	zend_string *const_COLLAPSE_NONE_name = zend_string_init_interned("COLLAPSE_NONE", sizeof("COLLAPSE_NONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_NONE_name, &const_COLLAPSE_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_NONE_name, true);

	zval const_COLLAPSE_UNIT_value;
	ZVAL_LONG(&const_COLLAPSE_UNIT_value, 2);
	zend_string *const_COLLAPSE_UNIT_name = zend_string_init_interned("COLLAPSE_UNIT", sizeof("COLLAPSE_UNIT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_UNIT_name, &const_COLLAPSE_UNIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_UNIT_name, true);

	zval const_COLLAPSE_ALL_value;
	ZVAL_LONG(&const_COLLAPSE_ALL_value, 3);
	zend_string *const_COLLAPSE_ALL_name = zend_string_init_interned("COLLAPSE_ALL", sizeof("COLLAPSE_ALL") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_COLLAPSE_ALL_name, &const_COLLAPSE_ALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_COLLAPSE_ALL_name, true);

	zval const_IDENTITY_FALLBACK_SINGLE_VALUE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_SINGLE_VALUE_value, 0);
	zend_string *const_IDENTITY_FALLBACK_SINGLE_VALUE_name = zend_string_init_interned("IDENTITY_FALLBACK_SINGLE_VALUE", sizeof("IDENTITY_FALLBACK_SINGLE_VALUE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_SINGLE_VALUE_name, &const_IDENTITY_FALLBACK_SINGLE_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_SINGLE_VALUE_name, true);

	zval const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value, 1);
	zend_string *const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name = zend_string_init_interned("IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE", sizeof("IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name, &const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_APPROXIMATELY_OR_SINGLE_VALUE_name, true);

	zval const_IDENTITY_FALLBACK_APPROXIMATELY_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_APPROXIMATELY_value, 2);
	zend_string *const_IDENTITY_FALLBACK_APPROXIMATELY_name = zend_string_init_interned("IDENTITY_FALLBACK_APPROXIMATELY", sizeof("IDENTITY_FALLBACK_APPROXIMATELY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_APPROXIMATELY_name, &const_IDENTITY_FALLBACK_APPROXIMATELY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_APPROXIMATELY_name, true);

	zval const_IDENTITY_FALLBACK_RANGE_value;
	ZVAL_LONG(&const_IDENTITY_FALLBACK_RANGE_value, 3);
	zend_string *const_IDENTITY_FALLBACK_RANGE_name = zend_string_init_interned("IDENTITY_FALLBACK_RANGE", sizeof("IDENTITY_FALLBACK_RANGE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_IDENTITY_FALLBACK_RANGE_name, &const_IDENTITY_FALLBACK_RANGE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_IDENTITY_FALLBACK_RANGE_name, true);
#endif

	return class_entry;
}
