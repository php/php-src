/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: bcd5195c8a1eea47d881747a57f1b1342ef0bbd1 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_enchant_broker_init, 0, 0, EnchantBroker, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_free, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_enchant_broker_get_error, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_set_dict_path, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_enchant_broker_get_dict_path, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_list_dicts, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_enchant_broker_request_dict, 0, 2, EnchantDictionary, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_enchant_broker_request_pwl_dict, 0, 2, EnchantDictionary, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_free_dict, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_dict_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_broker_set_ordering, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ordering, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_enchant_broker_describe arginfo_enchant_broker_list_dicts

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_quick_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, suggestions, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_suggest, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_enchant_dict_add_to_personal arginfo_enchant_dict_add

#define arginfo_enchant_dict_add_to_session arginfo_enchant_dict_add

#define arginfo_enchant_dict_is_added arginfo_enchant_dict_check

#define arginfo_enchant_dict_is_in_session arginfo_enchant_dict_check

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_store_replacement, 0, 3, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, misspelled, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, correct, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_enchant_dict_get_error, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_describe, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, EnchantDictionary, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(enchant_broker_init);
ZEND_FUNCTION(enchant_broker_free);
ZEND_FUNCTION(enchant_broker_get_error);
ZEND_FUNCTION(enchant_broker_set_dict_path);
ZEND_FUNCTION(enchant_broker_get_dict_path);
ZEND_FUNCTION(enchant_broker_list_dicts);
ZEND_FUNCTION(enchant_broker_request_dict);
ZEND_FUNCTION(enchant_broker_request_pwl_dict);
ZEND_FUNCTION(enchant_broker_free_dict);
ZEND_FUNCTION(enchant_broker_dict_exists);
ZEND_FUNCTION(enchant_broker_set_ordering);
ZEND_FUNCTION(enchant_broker_describe);
ZEND_FUNCTION(enchant_dict_quick_check);
ZEND_FUNCTION(enchant_dict_check);
ZEND_FUNCTION(enchant_dict_suggest);
ZEND_FUNCTION(enchant_dict_add);
ZEND_FUNCTION(enchant_dict_add_to_session);
ZEND_FUNCTION(enchant_dict_is_added);
ZEND_FUNCTION(enchant_dict_store_replacement);
ZEND_FUNCTION(enchant_dict_get_error);
ZEND_FUNCTION(enchant_dict_describe);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(enchant_broker_init, arginfo_enchant_broker_init)
	ZEND_RAW_FENTRY("enchant_broker_free", zif_enchant_broker_free, arginfo_enchant_broker_free, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(enchant_broker_get_error, arginfo_enchant_broker_get_error)
	ZEND_RAW_FENTRY("enchant_broker_set_dict_path", zif_enchant_broker_set_dict_path, arginfo_enchant_broker_set_dict_path, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_RAW_FENTRY("enchant_broker_get_dict_path", zif_enchant_broker_get_dict_path, arginfo_enchant_broker_get_dict_path, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(enchant_broker_list_dicts, arginfo_enchant_broker_list_dicts)
	ZEND_FE(enchant_broker_request_dict, arginfo_enchant_broker_request_dict)
	ZEND_FE(enchant_broker_request_pwl_dict, arginfo_enchant_broker_request_pwl_dict)
	ZEND_RAW_FENTRY("enchant_broker_free_dict", zif_enchant_broker_free_dict, arginfo_enchant_broker_free_dict, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(enchant_broker_dict_exists, arginfo_enchant_broker_dict_exists)
	ZEND_FE(enchant_broker_set_ordering, arginfo_enchant_broker_set_ordering)
	ZEND_FE(enchant_broker_describe, arginfo_enchant_broker_describe)
	ZEND_FE(enchant_dict_quick_check, arginfo_enchant_dict_quick_check)
	ZEND_FE(enchant_dict_check, arginfo_enchant_dict_check)
	ZEND_FE(enchant_dict_suggest, arginfo_enchant_dict_suggest)
	ZEND_FE(enchant_dict_add, arginfo_enchant_dict_add)
	ZEND_RAW_FENTRY("enchant_dict_add_to_personal", zif_enchant_dict_add, arginfo_enchant_dict_add_to_personal, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(enchant_dict_add_to_session, arginfo_enchant_dict_add_to_session)
	ZEND_FE(enchant_dict_is_added, arginfo_enchant_dict_is_added)
	ZEND_RAW_FENTRY("enchant_dict_is_in_session", zif_enchant_dict_is_added, arginfo_enchant_dict_is_in_session, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(enchant_dict_store_replacement, arginfo_enchant_dict_store_replacement)
	ZEND_FE(enchant_dict_get_error, arginfo_enchant_dict_get_error)
	ZEND_FE(enchant_dict_describe, arginfo_enchant_dict_describe)
	ZEND_FE_END
};

static void register_enchant_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("ENCHANT_MYSPELL", PHP_ENCHANT_MYSPELL, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("ENCHANT_ISPELL", PHP_ENCHANT_ISPELL, CONST_PERSISTENT | CONST_DEPRECATED);
#if defined(HAVE_ENCHANT_GET_VERSION)
	REGISTER_STRING_CONSTANT("LIBENCHANT_VERSION", PHP_ENCHANT_GET_VERSION, CONST_PERSISTENT);
#endif


	zend_attribute *attribute_Deprecated_func_enchant_broker_free_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_broker_free", sizeof("enchant_broker_free") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_enchant_broker_free_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_broker_free_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_free_0_arg0, attribute_Deprecated_func_enchant_broker_free_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_free_0->args[0].value, &attribute_Deprecated_func_enchant_broker_free_0_arg0);
	attribute_Deprecated_func_enchant_broker_free_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_enchant_broker_free_0_arg1;
	zend_string *attribute_Deprecated_func_enchant_broker_free_0_arg1_str = zend_string_init("as EnchantBroker objects are freed automatically", strlen("as EnchantBroker objects are freed automatically"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_free_0_arg1, attribute_Deprecated_func_enchant_broker_free_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_free_0->args[1].value, &attribute_Deprecated_func_enchant_broker_free_0_arg1);
	attribute_Deprecated_func_enchant_broker_free_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);

	zend_attribute *attribute_Deprecated_func_enchant_broker_set_dict_path_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_broker_set_dict_path", sizeof("enchant_broker_set_dict_path") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 1);
	zval attribute_Deprecated_func_enchant_broker_set_dict_path_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_broker_set_dict_path_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_set_dict_path_0_arg0, attribute_Deprecated_func_enchant_broker_set_dict_path_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_set_dict_path_0->args[0].value, &attribute_Deprecated_func_enchant_broker_set_dict_path_0_arg0);
	attribute_Deprecated_func_enchant_broker_set_dict_path_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);

	zend_attribute *attribute_Deprecated_func_enchant_broker_get_dict_path_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_broker_get_dict_path", sizeof("enchant_broker_get_dict_path") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 1);
	zval attribute_Deprecated_func_enchant_broker_get_dict_path_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_broker_get_dict_path_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_get_dict_path_0_arg0, attribute_Deprecated_func_enchant_broker_get_dict_path_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_get_dict_path_0->args[0].value, &attribute_Deprecated_func_enchant_broker_get_dict_path_0_arg0);
	attribute_Deprecated_func_enchant_broker_get_dict_path_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);

	zend_attribute *attribute_Deprecated_func_enchant_broker_free_dict_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_broker_free_dict", sizeof("enchant_broker_free_dict") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_enchant_broker_free_dict_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_broker_free_dict_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_free_dict_0_arg0, attribute_Deprecated_func_enchant_broker_free_dict_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_free_dict_0->args[0].value, &attribute_Deprecated_func_enchant_broker_free_dict_0_arg0);
	attribute_Deprecated_func_enchant_broker_free_dict_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_enchant_broker_free_dict_0_arg1;
	zend_string *attribute_Deprecated_func_enchant_broker_free_dict_0_arg1_str = zend_string_init("as EnchantDictionary objects are freed automatically", strlen("as EnchantDictionary objects are freed automatically"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_broker_free_dict_0_arg1, attribute_Deprecated_func_enchant_broker_free_dict_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_broker_free_dict_0->args[1].value, &attribute_Deprecated_func_enchant_broker_free_dict_0_arg1);
	attribute_Deprecated_func_enchant_broker_free_dict_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);

	zend_attribute *attribute_Deprecated_func_enchant_dict_add_to_personal_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_dict_add_to_personal", sizeof("enchant_dict_add_to_personal") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg0, attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_dict_add_to_personal_0->args[0].value, &attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg0);
	attribute_Deprecated_func_enchant_dict_add_to_personal_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg1;
	zend_string *attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg1_str = zend_string_init("use enchant_dict_add() instead", strlen("use enchant_dict_add() instead"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg1, attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_dict_add_to_personal_0->args[1].value, &attribute_Deprecated_func_enchant_dict_add_to_personal_0_arg1);
	attribute_Deprecated_func_enchant_dict_add_to_personal_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);

	zend_attribute *attribute_Deprecated_func_enchant_dict_is_in_session_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "enchant_dict_is_in_session", sizeof("enchant_dict_is_in_session") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_enchant_dict_is_in_session_0_arg0;
	zend_string *attribute_Deprecated_func_enchant_dict_is_in_session_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_dict_is_in_session_0_arg0, attribute_Deprecated_func_enchant_dict_is_in_session_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_dict_is_in_session_0->args[0].value, &attribute_Deprecated_func_enchant_dict_is_in_session_0_arg0);
	attribute_Deprecated_func_enchant_dict_is_in_session_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_enchant_dict_is_in_session_0_arg1;
	zend_string *attribute_Deprecated_func_enchant_dict_is_in_session_0_arg1_str = zend_string_init("use enchant_dict_is_added() instead", strlen("use enchant_dict_is_added() instead"), 1);
	ZVAL_STR(&attribute_Deprecated_func_enchant_dict_is_in_session_0_arg1, attribute_Deprecated_func_enchant_dict_is_in_session_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_enchant_dict_is_in_session_0->args[1].value, &attribute_Deprecated_func_enchant_dict_is_in_session_0_arg1);
	attribute_Deprecated_func_enchant_dict_is_in_session_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);
}

static zend_class_entry *register_class_EnchantBroker(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "EnchantBroker", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_EnchantDictionary(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "EnchantDictionary", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
