/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 5a67e10da7a70552110e807118a33d1d7bb028da */

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
	ZEND_ARG_TYPE_INFO(0, name, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_enchant_broker_get_dict_path, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, broker, EnchantBroker, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_LONG, 0)
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
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
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
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, suggestions, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_suggest, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_enchant_dict_add_to_personal arginfo_enchant_dict_add

#define arginfo_enchant_dict_add_to_session arginfo_enchant_dict_add

#define arginfo_enchant_dict_is_added arginfo_enchant_dict_check

#define arginfo_enchant_dict_is_in_session arginfo_enchant_dict_check

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_store_replacement, 0, 3, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
	ZEND_ARG_TYPE_INFO(0, mis, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cor, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_enchant_dict_get_error, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enchant_dict_describe, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, dict, EnchantDictionary, 0)
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
	ZEND_DEP_FE(enchant_broker_free, arginfo_enchant_broker_free)
	ZEND_FE(enchant_broker_get_error, arginfo_enchant_broker_get_error)
	ZEND_DEP_FE(enchant_broker_set_dict_path, arginfo_enchant_broker_set_dict_path)
	ZEND_DEP_FE(enchant_broker_get_dict_path, arginfo_enchant_broker_get_dict_path)
	ZEND_FE(enchant_broker_list_dicts, arginfo_enchant_broker_list_dicts)
	ZEND_FE(enchant_broker_request_dict, arginfo_enchant_broker_request_dict)
	ZEND_FE(enchant_broker_request_pwl_dict, arginfo_enchant_broker_request_pwl_dict)
	ZEND_DEP_FE(enchant_broker_free_dict, arginfo_enchant_broker_free_dict)
	ZEND_FE(enchant_broker_dict_exists, arginfo_enchant_broker_dict_exists)
	ZEND_FE(enchant_broker_set_ordering, arginfo_enchant_broker_set_ordering)
	ZEND_FE(enchant_broker_describe, arginfo_enchant_broker_describe)
	ZEND_FE(enchant_dict_quick_check, arginfo_enchant_dict_quick_check)
	ZEND_FE(enchant_dict_check, arginfo_enchant_dict_check)
	ZEND_FE(enchant_dict_suggest, arginfo_enchant_dict_suggest)
	ZEND_FE(enchant_dict_add, arginfo_enchant_dict_add)
	ZEND_DEP_FALIAS(enchant_dict_add_to_personal, enchant_dict_add, arginfo_enchant_dict_add_to_personal)
	ZEND_FE(enchant_dict_add_to_session, arginfo_enchant_dict_add_to_session)
	ZEND_FE(enchant_dict_is_added, arginfo_enchant_dict_is_added)
	ZEND_DEP_FALIAS(enchant_dict_is_in_session, enchant_dict_is_added, arginfo_enchant_dict_is_in_session)
	ZEND_FE(enchant_dict_store_replacement, arginfo_enchant_dict_store_replacement)
	ZEND_FE(enchant_dict_get_error, arginfo_enchant_dict_get_error)
	ZEND_FE(enchant_dict_describe, arginfo_enchant_dict_describe)
	ZEND_FE_END
};


static const zend_function_entry class_EnchantBroker_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_EnchantDictionary_methods[] = {
	ZEND_FE_END
};
