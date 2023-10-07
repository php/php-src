/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 82af60e0faf01941fbf580da8957a867eda46384 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_Transliterator_create, 0, 1, Transliterator, 1)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_Transliterator_createFromRules, 0, 1, Transliterator, 1)
	ZEND_ARG_TYPE_INFO(0, rules, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, direction, IS_LONG, 0, "Transliterator::FORWARD")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_Transliterator_createInverse, 0, 0, Transliterator, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Transliterator_listIDs, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Transliterator_transliterate, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Transliterator_getErrorCode, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_Transliterator_getErrorMessage, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()


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
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_FORWARD_value;
	ZVAL_LONG(&const_FORWARD_value, TRANSLITERATOR_FORWARD);
	zend_string *const_FORWARD_name = zend_string_init_interned("FORWARD", sizeof("FORWARD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FORWARD_name, &const_FORWARD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FORWARD_name);

	zval const_REVERSE_value;
	ZVAL_LONG(&const_REVERSE_value, TRANSLITERATOR_REVERSE);
	zend_string *const_REVERSE_name = zend_string_init_interned("REVERSE", sizeof("REVERSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_REVERSE_name, &const_REVERSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_REVERSE_name);

	zval property_id_default_value;
	ZVAL_UNDEF(&property_id_default_value);
	zend_string *property_id_name = zend_string_init("id", sizeof("id") - 1, 1);
	zend_declare_typed_property(class_entry, property_id_name, &property_id_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_id_name);

	return class_entry;
}
