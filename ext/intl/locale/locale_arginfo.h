/* This is a generated file, edit the .stub.php file instead. */

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
	ZEND_ARG_TYPE_INFO(0, in_locale, IS_STRING, 1)
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
	ZEND_ARG_TYPE_INFO(0, langtag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, canonicalize, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_lookup, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, langtag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, canonicalize, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, def, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_canonicalize arginfo_class_Locale_setDefault

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_acceptFromHttp, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_get_default, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_set_default, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_get_primary_language, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_locale_get_script arginfo_locale_get_primary_language

#define arginfo_locale_get_region arginfo_locale_get_primary_language

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_get_keywords, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_get_display_script, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, in_locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_locale_get_display_region arginfo_locale_get_display_script

#define arginfo_locale_get_display_name arginfo_locale_get_display_script

#define arginfo_locale_get_display_language arginfo_locale_get_display_script

#define arginfo_locale_get_display_variant arginfo_locale_get_display_script

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_compose, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, subtags, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_parse, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_locale_get_all_variants arginfo_locale_parse

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_filter_matches, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, langtag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, canonicalize, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_locale_canonicalize arginfo_locale_get_primary_language

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_locale_lookup, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, langtag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, canonicalize, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, def, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_locale_accept_from_http, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()
