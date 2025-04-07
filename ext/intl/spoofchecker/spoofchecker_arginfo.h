/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4834be57a3f0cb74dbc4422e609846139f09f6cb */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_isSuspicious, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorCode, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_areConfusable, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorCode, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_setAllowedLocales, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, locales, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_setChecks, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, checks, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_setRestrictionLevel, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Spoofchecker_setAllowedChars, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, patternOptions, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_METHOD(Spoofchecker, __construct);
ZEND_METHOD(Spoofchecker, isSuspicious);
ZEND_METHOD(Spoofchecker, areConfusable);
ZEND_METHOD(Spoofchecker, setAllowedLocales);
ZEND_METHOD(Spoofchecker, setChecks);
#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_METHOD(Spoofchecker, setRestrictionLevel);
#endif
ZEND_METHOD(Spoofchecker, setAllowedChars);

static const zend_function_entry class_Spoofchecker_methods[] = {
	ZEND_ME(Spoofchecker, __construct, arginfo_class_Spoofchecker___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, isSuspicious, arginfo_class_Spoofchecker_isSuspicious, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, areConfusable, arginfo_class_Spoofchecker_areConfusable, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, setAllowedLocales, arginfo_class_Spoofchecker_setAllowedLocales, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, setChecks, arginfo_class_Spoofchecker_setChecks, ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM >= 58
	ZEND_ME(Spoofchecker, setRestrictionLevel, arginfo_class_Spoofchecker_setRestrictionLevel, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(Spoofchecker, setAllowedChars, arginfo_class_Spoofchecker_setAllowedChars, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Spoofchecker(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Spoofchecker", class_Spoofchecker_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);

	zval const_SINGLE_SCRIPT_CONFUSABLE_value;
	ZVAL_LONG(&const_SINGLE_SCRIPT_CONFUSABLE_value, USPOOF_SINGLE_SCRIPT_CONFUSABLE);
	zend_string *const_SINGLE_SCRIPT_CONFUSABLE_name = zend_string_init_interned("SINGLE_SCRIPT_CONFUSABLE", sizeof("SINGLE_SCRIPT_CONFUSABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SINGLE_SCRIPT_CONFUSABLE_name, &const_SINGLE_SCRIPT_CONFUSABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SINGLE_SCRIPT_CONFUSABLE_name);

	zval const_MIXED_SCRIPT_CONFUSABLE_value;
	ZVAL_LONG(&const_MIXED_SCRIPT_CONFUSABLE_value, USPOOF_MIXED_SCRIPT_CONFUSABLE);
	zend_string *const_MIXED_SCRIPT_CONFUSABLE_name = zend_string_init_interned("MIXED_SCRIPT_CONFUSABLE", sizeof("MIXED_SCRIPT_CONFUSABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MIXED_SCRIPT_CONFUSABLE_name, &const_MIXED_SCRIPT_CONFUSABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MIXED_SCRIPT_CONFUSABLE_name);

	zval const_WHOLE_SCRIPT_CONFUSABLE_value;
	ZVAL_LONG(&const_WHOLE_SCRIPT_CONFUSABLE_value, USPOOF_WHOLE_SCRIPT_CONFUSABLE);
	zend_string *const_WHOLE_SCRIPT_CONFUSABLE_name = zend_string_init_interned("WHOLE_SCRIPT_CONFUSABLE", sizeof("WHOLE_SCRIPT_CONFUSABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WHOLE_SCRIPT_CONFUSABLE_name, &const_WHOLE_SCRIPT_CONFUSABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WHOLE_SCRIPT_CONFUSABLE_name);

	zval const_ANY_CASE_value;
	ZVAL_LONG(&const_ANY_CASE_value, USPOOF_ANY_CASE);
	zend_string *const_ANY_CASE_name = zend_string_init_interned("ANY_CASE", sizeof("ANY_CASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ANY_CASE_name, &const_ANY_CASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ANY_CASE_name);

	zval const_SINGLE_SCRIPT_value;
	ZVAL_LONG(&const_SINGLE_SCRIPT_value, USPOOF_SINGLE_SCRIPT);
	zend_string *const_SINGLE_SCRIPT_name = zend_string_init_interned("SINGLE_SCRIPT", sizeof("SINGLE_SCRIPT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SINGLE_SCRIPT_name, &const_SINGLE_SCRIPT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SINGLE_SCRIPT_name);

	zval const_INVISIBLE_value;
	ZVAL_LONG(&const_INVISIBLE_value, USPOOF_INVISIBLE);
	zend_string *const_INVISIBLE_name = zend_string_init_interned("INVISIBLE", sizeof("INVISIBLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_INVISIBLE_name, &const_INVISIBLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_INVISIBLE_name);

	zval const_CHAR_LIMIT_value;
	ZVAL_LONG(&const_CHAR_LIMIT_value, USPOOF_CHAR_LIMIT);
	zend_string *const_CHAR_LIMIT_name = zend_string_init_interned("CHAR_LIMIT", sizeof("CHAR_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_LIMIT_name, &const_CHAR_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_LIMIT_name);
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_ASCII_value;
	ZVAL_LONG(&const_ASCII_value, USPOOF_ASCII);
	zend_string *const_ASCII_name = zend_string_init_interned("ASCII", sizeof("ASCII") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ASCII_name, &const_ASCII_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ASCII_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_HIGHLY_RESTRICTIVE_value;
	ZVAL_LONG(&const_HIGHLY_RESTRICTIVE_value, USPOOF_HIGHLY_RESTRICTIVE);
	zend_string *const_HIGHLY_RESTRICTIVE_name = zend_string_init_interned("HIGHLY_RESTRICTIVE", sizeof("HIGHLY_RESTRICTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HIGHLY_RESTRICTIVE_name, &const_HIGHLY_RESTRICTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HIGHLY_RESTRICTIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_MODERATELY_RESTRICTIVE_value;
	ZVAL_LONG(&const_MODERATELY_RESTRICTIVE_value, USPOOF_MODERATELY_RESTRICTIVE);
	zend_string *const_MODERATELY_RESTRICTIVE_name = zend_string_init_interned("MODERATELY_RESTRICTIVE", sizeof("MODERATELY_RESTRICTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MODERATELY_RESTRICTIVE_name, &const_MODERATELY_RESTRICTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MODERATELY_RESTRICTIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_MINIMALLY_RESTRICTIVE_value;
	ZVAL_LONG(&const_MINIMALLY_RESTRICTIVE_value, USPOOF_MINIMALLY_RESTRICTIVE);
	zend_string *const_MINIMALLY_RESTRICTIVE_name = zend_string_init_interned("MINIMALLY_RESTRICTIVE", sizeof("MINIMALLY_RESTRICTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MINIMALLY_RESTRICTIVE_name, &const_MINIMALLY_RESTRICTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MINIMALLY_RESTRICTIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_UNRESTRICTIVE_value;
	ZVAL_LONG(&const_UNRESTRICTIVE_value, USPOOF_UNRESTRICTIVE);
	zend_string *const_UNRESTRICTIVE_name = zend_string_init_interned("UNRESTRICTIVE", sizeof("UNRESTRICTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UNRESTRICTIVE_name, &const_UNRESTRICTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_UNRESTRICTIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_SINGLE_SCRIPT_RESTRICTIVE_value;
	ZVAL_LONG(&const_SINGLE_SCRIPT_RESTRICTIVE_value, USPOOF_SINGLE_SCRIPT_RESTRICTIVE);
	zend_string *const_SINGLE_SCRIPT_RESTRICTIVE_name = zend_string_init_interned("SINGLE_SCRIPT_RESTRICTIVE", sizeof("SINGLE_SCRIPT_RESTRICTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SINGLE_SCRIPT_RESTRICTIVE_name, &const_SINGLE_SCRIPT_RESTRICTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SINGLE_SCRIPT_RESTRICTIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 58

	zval const_MIXED_NUMBERS_value;
	ZVAL_LONG(&const_MIXED_NUMBERS_value, USPOOF_MIXED_NUMBERS);
	zend_string *const_MIXED_NUMBERS_name = zend_string_init_interned("MIXED_NUMBERS", sizeof("MIXED_NUMBERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MIXED_NUMBERS_name, &const_MIXED_NUMBERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MIXED_NUMBERS_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 62

	zval const_HIDDEN_OVERLAY_value;
	ZVAL_LONG(&const_HIDDEN_OVERLAY_value, USPOOF_HIDDEN_OVERLAY);
	zend_string *const_HIDDEN_OVERLAY_name = zend_string_init_interned("HIDDEN_OVERLAY", sizeof("HIDDEN_OVERLAY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HIDDEN_OVERLAY_name, &const_HIDDEN_OVERLAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HIDDEN_OVERLAY_name);
#endif

	zval const_IGNORE_SPACE_value;
	ZVAL_LONG(&const_IGNORE_SPACE_value, USET_IGNORE_SPACE);
	zend_string *const_IGNORE_SPACE_name = zend_string_init_interned("IGNORE_SPACE", sizeof("IGNORE_SPACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IGNORE_SPACE_name, &const_IGNORE_SPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IGNORE_SPACE_name);

	zval const_CASE_INSENSITIVE_value;
	ZVAL_LONG(&const_CASE_INSENSITIVE_value, USET_CASE_INSENSITIVE);
	zend_string *const_CASE_INSENSITIVE_name = zend_string_init_interned("CASE_INSENSITIVE", sizeof("CASE_INSENSITIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CASE_INSENSITIVE_name, &const_CASE_INSENSITIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CASE_INSENSITIVE_name);

	zval const_ADD_CASE_MAPPINGS_value;
	ZVAL_LONG(&const_ADD_CASE_MAPPINGS_value, USET_ADD_CASE_MAPPINGS);
	zend_string *const_ADD_CASE_MAPPINGS_name = zend_string_init_interned("ADD_CASE_MAPPINGS", sizeof("ADD_CASE_MAPPINGS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ADD_CASE_MAPPINGS_name, &const_ADD_CASE_MAPPINGS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ADD_CASE_MAPPINGS_name);
#if U_ICU_VERSION_MAJOR_NUM >= 73

	zval const_SIMPLE_CASE_INSENSITIVE_value;
	ZVAL_LONG(&const_SIMPLE_CASE_INSENSITIVE_value, USET_SIMPLE_CASE_INSENSITIVE);
	zend_string *const_SIMPLE_CASE_INSENSITIVE_name = zend_string_init_interned("SIMPLE_CASE_INSENSITIVE", sizeof("SIMPLE_CASE_INSENSITIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SIMPLE_CASE_INSENSITIVE_name, &const_SIMPLE_CASE_INSENSITIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SIMPLE_CASE_INSENSITIVE_name);
#endif

	return class_entry;
}
