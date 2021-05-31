/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c34871965e0d58bb15beda1787b7cdd0754bb28f */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_create, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_createFromRules, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, rules, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

#define arginfo_class_Transliterator_createInverse arginfo_class_Transliterator___construct

#define arginfo_class_Transliterator_listIDs arginfo_class_Transliterator___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_transliterate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_Transliterator_getErrorCode arginfo_class_Transliterator___construct

#define arginfo_class_Transliterator_getErrorMessage arginfo_class_Transliterator___construct


ZEND_METHOD(Transliterator, __construct);
ZEND_FUNCTION(transliterator_create);
ZEND_FUNCTION(transliterator_create_from_rules);
ZEND_FUNCTION(transliterator_create_inverse);
ZEND_FUNCTION(transliterator_list_ids);
ZEND_FUNCTION(transliterator_transliterate);
ZEND_FUNCTION(transliterator_get_error_code);
ZEND_FUNCTION(transliterator_get_error_message);


static const zend_function_entry class_Transliterator_methods[] = {
	ZEND_ME(Transliterator, __construct, arginfo_class_Transliterator___construct, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME_MAPPING(create, transliterator_create, arginfo_class_Transliterator_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createFromRules, transliterator_create_from_rules, arginfo_class_Transliterator_createFromRules, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createInverse, transliterator_create_inverse, arginfo_class_Transliterator_createInverse, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(listIDs, transliterator_list_ids, arginfo_class_Transliterator_listIDs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(transliterate, transliterator_transliterate, arginfo_class_Transliterator_transliterate, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorCode, transliterator_get_error_code, arginfo_class_Transliterator_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, transliterator_get_error_message, arginfo_class_Transliterator_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Transliterator(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Transliterator", class_Transliterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_id_default_value;
	ZVAL_UNDEF(&property_id_default_value);
	zend_string *property_id_name = zend_string_init("id", sizeof("id") - 1, 1);
	zend_declare_typed_property(class_entry, property_id_name, &property_id_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_id_name);

	return class_entry;
}
