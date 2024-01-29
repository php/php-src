/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e446ff14fc14d08a6ac225af78cde931e381ee5b */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Locale_getDefault, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_setDefault, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Locale_getPrimaryLanguage, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_getScript arginfo_class_Locale_getPrimaryLanguage

#define arginfo_class_Locale_getRegion arginfo_class_Locale_getPrimaryLanguage

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Locale_getKeywords, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Locale_getDisplayScript, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, displayLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_getDisplayRegion arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayName arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayLanguage arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayVariant arginfo_class_Locale_getDisplayScript

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Locale_composeLocale, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, subtags, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Locale_parseLocale, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_getAllVariants arginfo_class_Locale_parseLocale

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Locale_filterMatches, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Locale_lookup, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defaultLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_canonicalize arginfo_class_Locale_getPrimaryLanguage

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Locale_acceptFromHttp, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(locale_get_default);
ZEND_FUNCTION(locale_set_default);
ZEND_FUNCTION(locale_get_primary_language);
ZEND_FUNCTION(locale_get_script);
ZEND_FUNCTION(locale_get_region);
ZEND_FUNCTION(locale_get_keywords);
ZEND_FUNCTION(locale_get_display_script);
ZEND_FUNCTION(locale_get_display_region);
ZEND_FUNCTION(locale_get_display_name);
ZEND_FUNCTION(locale_get_display_language);
ZEND_FUNCTION(locale_get_display_variant);
ZEND_FUNCTION(locale_compose);
ZEND_FUNCTION(locale_parse);
ZEND_FUNCTION(locale_get_all_variants);
ZEND_FUNCTION(locale_filter_matches);
ZEND_FUNCTION(locale_lookup);
ZEND_FUNCTION(locale_canonicalize);
ZEND_FUNCTION(locale_accept_from_http);

static const zend_function_entry class_Locale_methods[] = {
	ZEND_RAW_FENTRY("getDefault", zif_locale_get_default, arginfo_class_Locale_getDefault, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("setDefault", zif_locale_set_default, arginfo_class_Locale_setDefault, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getPrimaryLanguage", zif_locale_get_primary_language, arginfo_class_Locale_getPrimaryLanguage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getScript", zif_locale_get_script, arginfo_class_Locale_getScript, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getRegion", zif_locale_get_region, arginfo_class_Locale_getRegion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getKeywords", zif_locale_get_keywords, arginfo_class_Locale_getKeywords, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getDisplayScript", zif_locale_get_display_script, arginfo_class_Locale_getDisplayScript, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getDisplayRegion", zif_locale_get_display_region, arginfo_class_Locale_getDisplayRegion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getDisplayName", zif_locale_get_display_name, arginfo_class_Locale_getDisplayName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getDisplayLanguage", zif_locale_get_display_language, arginfo_class_Locale_getDisplayLanguage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getDisplayVariant", zif_locale_get_display_variant, arginfo_class_Locale_getDisplayVariant, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("composeLocale", zif_locale_compose, arginfo_class_Locale_composeLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("parseLocale", zif_locale_parse, arginfo_class_Locale_parseLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getAllVariants", zif_locale_get_all_variants, arginfo_class_Locale_getAllVariants, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("filterMatches", zif_locale_filter_matches, arginfo_class_Locale_filterMatches, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("lookup", zif_locale_lookup, arginfo_class_Locale_lookup, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("canonicalize", zif_locale_canonicalize, arginfo_class_Locale_canonicalize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("acceptFromHttp", zif_locale_accept_from_http, arginfo_class_Locale_acceptFromHttp, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_Locale(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Locale", class_Locale_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval const_ACTUAL_LOCALE_value;
	ZVAL_LONG(&const_ACTUAL_LOCALE_value, ULOC_ACTUAL_LOCALE);
	zend_string *const_ACTUAL_LOCALE_name = zend_string_init_interned("ACTUAL_LOCALE", sizeof("ACTUAL_LOCALE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ACTUAL_LOCALE_name, &const_ACTUAL_LOCALE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ACTUAL_LOCALE_name);

	zval const_VALID_LOCALE_value;
	ZVAL_LONG(&const_VALID_LOCALE_value, ULOC_VALID_LOCALE);
	zend_string *const_VALID_LOCALE_name = zend_string_init_interned("VALID_LOCALE", sizeof("VALID_LOCALE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_VALID_LOCALE_name, &const_VALID_LOCALE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_VALID_LOCALE_name);

	zval const_DEFAULT_LOCALE_value;
	ZVAL_NULL(&const_DEFAULT_LOCALE_value);
	zend_string *const_DEFAULT_LOCALE_name = zend_string_init_interned("DEFAULT_LOCALE", sizeof("DEFAULT_LOCALE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DEFAULT_LOCALE_name, &const_DEFAULT_LOCALE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_NULL));
	zend_string_release(const_DEFAULT_LOCALE_name);

	zval const_LANG_TAG_value;
	zend_string *const_LANG_TAG_value_str = zend_string_init(LOC_LANG_TAG, strlen(LOC_LANG_TAG), 1);
	ZVAL_STR(&const_LANG_TAG_value, const_LANG_TAG_value_str);
	zend_string *const_LANG_TAG_name = zend_string_init_interned("LANG_TAG", sizeof("LANG_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LANG_TAG_name, &const_LANG_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_LANG_TAG_name);

	zval const_EXTLANG_TAG_value;
	zend_string *const_EXTLANG_TAG_value_str = zend_string_init(LOC_EXTLANG_TAG, strlen(LOC_EXTLANG_TAG), 1);
	ZVAL_STR(&const_EXTLANG_TAG_value, const_EXTLANG_TAG_value_str);
	zend_string *const_EXTLANG_TAG_name = zend_string_init_interned("EXTLANG_TAG", sizeof("EXTLANG_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXTLANG_TAG_name, &const_EXTLANG_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_EXTLANG_TAG_name);

	zval const_SCRIPT_TAG_value;
	zend_string *const_SCRIPT_TAG_value_str = zend_string_init(LOC_SCRIPT_TAG, strlen(LOC_SCRIPT_TAG), 1);
	ZVAL_STR(&const_SCRIPT_TAG_value, const_SCRIPT_TAG_value_str);
	zend_string *const_SCRIPT_TAG_name = zend_string_init_interned("SCRIPT_TAG", sizeof("SCRIPT_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SCRIPT_TAG_name, &const_SCRIPT_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_SCRIPT_TAG_name);

	zval const_REGION_TAG_value;
	zend_string *const_REGION_TAG_value_str = zend_string_init(LOC_REGION_TAG, strlen(LOC_REGION_TAG), 1);
	ZVAL_STR(&const_REGION_TAG_value, const_REGION_TAG_value_str);
	zend_string *const_REGION_TAG_name = zend_string_init_interned("REGION_TAG", sizeof("REGION_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_REGION_TAG_name, &const_REGION_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_REGION_TAG_name);

	zval const_VARIANT_TAG_value;
	zend_string *const_VARIANT_TAG_value_str = zend_string_init(LOC_VARIANT_TAG, strlen(LOC_VARIANT_TAG), 1);
	ZVAL_STR(&const_VARIANT_TAG_value, const_VARIANT_TAG_value_str);
	zend_string *const_VARIANT_TAG_name = zend_string_init_interned("VARIANT_TAG", sizeof("VARIANT_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_VARIANT_TAG_name, &const_VARIANT_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_VARIANT_TAG_name);

	zval const_GRANDFATHERED_LANG_TAG_value;
	zend_string *const_GRANDFATHERED_LANG_TAG_value_str = zend_string_init(LOC_GRANDFATHERED_LANG_TAG, strlen(LOC_GRANDFATHERED_LANG_TAG), 1);
	ZVAL_STR(&const_GRANDFATHERED_LANG_TAG_value, const_GRANDFATHERED_LANG_TAG_value_str);
	zend_string *const_GRANDFATHERED_LANG_TAG_name = zend_string_init_interned("GRANDFATHERED_LANG_TAG", sizeof("GRANDFATHERED_LANG_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GRANDFATHERED_LANG_TAG_name, &const_GRANDFATHERED_LANG_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_GRANDFATHERED_LANG_TAG_name);

	zval const_PRIVATE_TAG_value;
	zend_string *const_PRIVATE_TAG_value_str = zend_string_init(LOC_PRIVATE_TAG, strlen(LOC_PRIVATE_TAG), 1);
	ZVAL_STR(&const_PRIVATE_TAG_value, const_PRIVATE_TAG_value_str);
	zend_string *const_PRIVATE_TAG_name = zend_string_init_interned("PRIVATE_TAG", sizeof("PRIVATE_TAG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PRIVATE_TAG_name, &const_PRIVATE_TAG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_PRIVATE_TAG_name);

	return class_entry;
}
