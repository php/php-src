/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1d201126072be285aa2dd3c0136cb7cc261f5841 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new_personal, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new_config, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, dictionary, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_suggest, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dictionary, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_store_replacement, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, dictionary, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, misspelled, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, correct, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_add_to_personal arginfo_pspell_check

#define arginfo_pspell_add_to_session arginfo_pspell_check

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_clear_session, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, dictionary, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_save_wordlist arginfo_pspell_clear_session

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_create, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, spelling, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, jargon, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_runtogether, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, allow, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_mode, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_ignore, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min_length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_personal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_dict_dir, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_config_data_dir arginfo_pspell_config_dict_dir

#define arginfo_pspell_config_repl arginfo_pspell_config_personal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_save_repl, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
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
