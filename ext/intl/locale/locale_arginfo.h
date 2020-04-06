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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, in_locale, IS_STRING, 1, "null")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_lookup, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, langtag, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, canonicalize, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, def, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Locale_canonicalize arginfo_class_Locale_setDefault

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Locale_acceptFromHttp, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()
