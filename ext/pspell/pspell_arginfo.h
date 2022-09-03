/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8d35f61a0b48c5422b31e78f587d9258fd3e8e37 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pspell_new, 0, 1, PSpell\\Dictionary, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pspell_new_personal, 0, 2, PSpell\\Dictionary, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pspell_new_config, 0, 1, PSpell\\Dictionary, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, PSpell\\Dictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_suggest, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, dictionary, PSpell\\Dictionary, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_store_replacement, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, PSpell\\Dictionary, 0)
	ZEND_ARG_TYPE_INFO(0, misspelled, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, correct, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_add_to_personal arginfo_pspell_check

#define arginfo_pspell_add_to_session arginfo_pspell_check

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_clear_session, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, dictionary, PSpell\\Dictionary, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_save_wordlist arginfo_pspell_clear_session

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_pspell_config_create, 0, 1, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_runtogether, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, allow, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_mode, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_ignore, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, min_length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_personal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_dict_dir, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_config_data_dir arginfo_pspell_config_dict_dir

#define arginfo_pspell_config_repl arginfo_pspell_config_personal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_save_repl, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, config, PSpell\\Config, 0)
	ZEND_ARG_TYPE_INFO(0, save, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(pspell_new);
ZEND_FUNCTION(pspell_new_personal);
ZEND_FUNCTION(pspell_new_config);
ZEND_FUNCTION(pspell_check);
ZEND_FUNCTION(pspell_suggest);
ZEND_FUNCTION(pspell_store_replacement);
ZEND_FUNCTION(pspell_add_to_personal);
ZEND_FUNCTION(pspell_add_to_session);
ZEND_FUNCTION(pspell_clear_session);
ZEND_FUNCTION(pspell_save_wordlist);
ZEND_FUNCTION(pspell_config_create);
ZEND_FUNCTION(pspell_config_runtogether);
ZEND_FUNCTION(pspell_config_mode);
ZEND_FUNCTION(pspell_config_ignore);
ZEND_FUNCTION(pspell_config_personal);
ZEND_FUNCTION(pspell_config_dict_dir);
ZEND_FUNCTION(pspell_config_data_dir);
ZEND_FUNCTION(pspell_config_repl);
ZEND_FUNCTION(pspell_config_save_repl);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(pspell_new, arginfo_pspell_new)
	ZEND_FE(pspell_new_personal, arginfo_pspell_new_personal)
	ZEND_FE(pspell_new_config, arginfo_pspell_new_config)
	ZEND_FE(pspell_check, arginfo_pspell_check)
	ZEND_FE(pspell_suggest, arginfo_pspell_suggest)
	ZEND_FE(pspell_store_replacement, arginfo_pspell_store_replacement)
	ZEND_FE(pspell_add_to_personal, arginfo_pspell_add_to_personal)
	ZEND_FE(pspell_add_to_session, arginfo_pspell_add_to_session)
	ZEND_FE(pspell_clear_session, arginfo_pspell_clear_session)
	ZEND_FE(pspell_save_wordlist, arginfo_pspell_save_wordlist)
	ZEND_FE(pspell_config_create, arginfo_pspell_config_create)
	ZEND_FE(pspell_config_runtogether, arginfo_pspell_config_runtogether)
	ZEND_FE(pspell_config_mode, arginfo_pspell_config_mode)
	ZEND_FE(pspell_config_ignore, arginfo_pspell_config_ignore)
	ZEND_FE(pspell_config_personal, arginfo_pspell_config_personal)
	ZEND_FE(pspell_config_dict_dir, arginfo_pspell_config_dict_dir)
	ZEND_FE(pspell_config_data_dir, arginfo_pspell_config_data_dir)
	ZEND_FE(pspell_config_repl, arginfo_pspell_config_repl)
	ZEND_FE(pspell_config_save_repl, arginfo_pspell_config_save_repl)
	ZEND_FE_END
};


static const zend_function_entry class_PSpell_Dictionary_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_PSpell_Config_methods[] = {
	ZEND_FE_END
};

static void register_pspell_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("PSPELL_FAST", PSPELL_FAST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSPELL_NORMAL", PSPELL_NORMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSPELL_BAD_SPELLERS", PSPELL_BAD_SPELLERS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSPELL_RUN_TOGETHER", PSPELL_RUN_TOGETHER, CONST_PERSISTENT);
}

static zend_class_entry *register_class_PSpell_Dictionary(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "PSpell", "Dictionary", class_PSpell_Dictionary_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_PSpell_Config(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "PSpell", "Config", class_PSpell_Config_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
