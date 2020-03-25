/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, spelling, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, jargon, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new_personal, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, personal, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, spelling, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, jargon, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_new_config, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, config, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_check, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pspell, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pspell_suggest, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, pspell, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_store_replacement, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pspell, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, misspell, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, correct, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_add_to_personal arginfo_pspell_check

#define arginfo_pspell_add_to_session arginfo_pspell_check

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_clear_session, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pspell, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_save_wordlist arginfo_pspell_clear_session

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_create, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, language, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, spelling, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, jargon, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_runtogether, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, runtogether, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_mode, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_ignore, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ignore, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_personal, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, personal, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_dict_dir, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pspell_config_data_dir arginfo_pspell_config_dict_dir

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_repl, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, repl, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pspell_config_save_repl, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, conf, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, save, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
