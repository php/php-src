/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 869c6e60a8f2b153ef79f28a08e165ff3ec2bc14 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collator_create arginfo_class_Collator___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_compare, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_sort, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Collator::SORT_REGULAR")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_sortWithSortKeys, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Collator_asort arginfo_class_Collator_sort

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Collator_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Collator, __construct);
ZEND_FUNCTION(collator_create);
ZEND_FUNCTION(collator_compare);
ZEND_FUNCTION(collator_sort);
ZEND_FUNCTION(collator_sort_with_sort_keys);
ZEND_FUNCTION(collator_asort);
ZEND_FUNCTION(collator_get_attribute);
ZEND_FUNCTION(collator_set_attribute);
ZEND_FUNCTION(collator_get_strength);
ZEND_FUNCTION(collator_set_strength);
ZEND_FUNCTION(collator_get_locale);
ZEND_FUNCTION(collator_get_error_code);
ZEND_FUNCTION(collator_get_error_message);
ZEND_FUNCTION(collator_get_sort_key);


static const zend_function_entry class_Collator_methods[] = {
	ZEND_ME(Collator, __construct, arginfo_class_Collator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, collator_create, arginfo_class_Collator_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(compare, collator_compare, arginfo_class_Collator_compare, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(sort, collator_sort, arginfo_class_Collator_sort, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(sortWithSortKeys, collator_sort_with_sort_keys, arginfo_class_Collator_sortWithSortKeys, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(asort, collator_asort, arginfo_class_Collator_asort, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getAttribute, collator_get_attribute, arginfo_class_Collator_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setAttribute, collator_set_attribute, arginfo_class_Collator_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getStrength, collator_get_strength, arginfo_class_Collator_getStrength, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setStrength, collator_set_strength, arginfo_class_Collator_setStrength, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocale, collator_get_locale, arginfo_class_Collator_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, collator_get_error_code, arginfo_class_Collator_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, collator_get_error_message, arginfo_class_Collator_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getSortKey, collator_get_sort_key, arginfo_class_Collator_getSortKey, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
