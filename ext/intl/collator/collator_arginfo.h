/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collator_create arginfo_class_Collator___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_compare, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_sort, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, arr, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, sort_flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_sortWithSortKeys, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_asort, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, arr, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, sort_flag, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, val, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_getStrength, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_setStrength, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, strength, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_getLocale, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collator_getErrorCode arginfo_class_Collator_getStrength

#define arginfo_class_Collator_getErrorMessage arginfo_class_Collator_getStrength

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_getSortKey, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()
