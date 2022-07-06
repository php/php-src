/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4135dc9df49d3fe20275d49bad428a2e3e0a8752 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_getDefault, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_setDefault, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_getPrimaryLanguage arginfo_class_Locale_setDefault

#define arginfo_class_Locale_getScript arginfo_class_Locale_setDefault

#define arginfo_class_Locale_getRegion arginfo_class_Locale_setDefault

#define arginfo_class_Locale_getKeywords arginfo_class_Locale_setDefault

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_getDisplayScript, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, displayLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_getDisplayRegion arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayName arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayLanguage arginfo_class_Locale_getDisplayScript

#define arginfo_class_Locale_getDisplayVariant arginfo_class_Locale_getDisplayScript

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_composeLocale, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, subtags, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_parseLocale arginfo_class_Locale_setDefault

#define arginfo_class_Locale_getAllVariants arginfo_class_Locale_setDefault

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_filterMatches, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_lookup, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, languageTag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defaultLocale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_canonicalize arginfo_class_Locale_setDefault

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_acceptFromHttp, 0, 0, 1)
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
	ZEND_ME_MAPPING(getDefault, locale_get_default, arginfo_class_Locale_getDefault, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(setDefault, locale_set_default, arginfo_class_Locale_setDefault, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getPrimaryLanguage, locale_get_primary_language, arginfo_class_Locale_getPrimaryLanguage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getScript, locale_get_script, arginfo_class_Locale_getScript, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getRegion, locale_get_region, arginfo_class_Locale_getRegion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getKeywords, locale_get_keywords, arginfo_class_Locale_getKeywords, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayScript, locale_get_display_script, arginfo_class_Locale_getDisplayScript, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayRegion, locale_get_display_region, arginfo_class_Locale_getDisplayRegion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayName, locale_get_display_name, arginfo_class_Locale_getDisplayName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayLanguage, locale_get_display_language, arginfo_class_Locale_getDisplayLanguage, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayVariant, locale_get_display_variant, arginfo_class_Locale_getDisplayVariant, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(composeLocale, locale_compose, arginfo_class_Locale_composeLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(parseLocale, locale_parse, arginfo_class_Locale_parseLocale, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getAllVariants, locale_get_all_variants, arginfo_class_Locale_getAllVariants, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(filterMatches, locale_filter_matches, arginfo_class_Locale_filterMatches, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(lookup, locale_lookup, arginfo_class_Locale_lookup, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(canonicalize, locale_canonicalize, arginfo_class_Locale_canonicalize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(acceptFromHttp, locale_accept_from_http, arginfo_class_Locale_acceptFromHttp, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};
