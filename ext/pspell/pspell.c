/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Vlad Krupin <phpdevel@echospace.com>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define IS_EXT_MODULE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#if HAVE_PSPELL

/* this will enforce compatibility in .12 version (broken after .11.2) */
#define USE_ORIGINAL_MANAGER_FUNCS

#include "php_pspell.h"
#include <pspell.h>
#include "ext/standard/info.h"

#define PSPELL_FAST 1L
#define PSPELL_NORMAL 2L
#define PSPELL_BAD_SPELLERS 3L
#define PSPELL_SPEED_MASK_INTERNAL 3L
#define PSPELL_RUN_TOGETHER 8L

/* Largest ignored word can be 999 characters (this seems sane enough),
 * and it takes 3 bytes to represent that (see pspell_config_ignore)
 */
#define PSPELL_LARGEST_WORD 3

static PHP_MINIT_FUNCTION(pspell);
static PHP_MINFO_FUNCTION(pspell);
static PHP_FUNCTION(pspell_new);
static PHP_FUNCTION(pspell_new_personal);
static PHP_FUNCTION(pspell_new_config);
static PHP_FUNCTION(pspell_check);
static PHP_FUNCTION(pspell_suggest);
static PHP_FUNCTION(pspell_store_replacement);
static PHP_FUNCTION(pspell_add_to_personal);
static PHP_FUNCTION(pspell_add_to_session);
static PHP_FUNCTION(pspell_clear_session);
static PHP_FUNCTION(pspell_save_wordlist);
static PHP_FUNCTION(pspell_config_create);
static PHP_FUNCTION(pspell_config_runtogether);
static PHP_FUNCTION(pspell_config_mode);
static PHP_FUNCTION(pspell_config_ignore);
static PHP_FUNCTION(pspell_config_personal);
static PHP_FUNCTION(pspell_config_dict_dir);
static PHP_FUNCTION(pspell_config_data_dir);
static PHP_FUNCTION(pspell_config_repl);
static PHP_FUNCTION(pspell_config_save_repl);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_new, 0, 0, 1)
	ZEND_ARG_INFO(0, language)
	ZEND_ARG_INFO(0, spelling)
	ZEND_ARG_INFO(0, jargon)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_new_personal, 0, 0, 2)
	ZEND_ARG_INFO(0, personal)
	ZEND_ARG_INFO(0, language)
	ZEND_ARG_INFO(0, spelling)
	ZEND_ARG_INFO(0, jargon)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_new_config, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_check, 0, 0, 2)
	ZEND_ARG_INFO(0, pspell)
	ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_suggest, 0, 0, 2)
	ZEND_ARG_INFO(0, pspell)
	ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_store_replacement, 0, 0, 3)
	ZEND_ARG_INFO(0, pspell)
	ZEND_ARG_INFO(0, misspell)
	ZEND_ARG_INFO(0, correct)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_add_to_personal, 0, 0, 2)
	ZEND_ARG_INFO(0, pspell)
	ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_add_to_session, 0, 0, 2)
	ZEND_ARG_INFO(0, pspell)
	ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_clear_session, 0, 0, 1)
	ZEND_ARG_INFO(0, pspell)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_save_wordlist, 0, 0, 1)
	ZEND_ARG_INFO(0, pspell)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_create, 0, 0, 1)
	ZEND_ARG_INFO(0, language)
	ZEND_ARG_INFO(0, spelling)
	ZEND_ARG_INFO(0, jargon)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_runtogether, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, runtogether)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_mode, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_ignore, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, ignore)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_personal, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, personal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_dict_dir, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_data_dir, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_repl, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, repl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pspell_config_save_repl, 0, 0, 2)
	ZEND_ARG_INFO(0, conf)
	ZEND_ARG_INFO(0, save)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ pspell_functions[]
 */
static const zend_function_entry pspell_functions[] = {
	PHP_FE(pspell_new,					arginfo_pspell_new)
	PHP_FE(pspell_new_personal,			arginfo_pspell_new_personal)
	PHP_FE(pspell_new_config,			arginfo_pspell_new_config)
	PHP_FE(pspell_check,				arginfo_pspell_check)
	PHP_FE(pspell_suggest,				arginfo_pspell_suggest)
	PHP_FE(pspell_store_replacement,	arginfo_pspell_store_replacement)
	PHP_FE(pspell_add_to_personal,		arginfo_pspell_add_to_personal)
	PHP_FE(pspell_add_to_session,		arginfo_pspell_add_to_session)
	PHP_FE(pspell_clear_session,		arginfo_pspell_clear_session)
	PHP_FE(pspell_save_wordlist,		arginfo_pspell_save_wordlist)
	PHP_FE(pspell_config_create,		arginfo_pspell_config_create)
	PHP_FE(pspell_config_runtogether,	arginfo_pspell_config_runtogether)
	PHP_FE(pspell_config_mode,			arginfo_pspell_config_mode)
	PHP_FE(pspell_config_ignore,		arginfo_pspell_config_ignore)
	PHP_FE(pspell_config_personal,		arginfo_pspell_config_personal)
	PHP_FE(pspell_config_dict_dir,		arginfo_pspell_config_dict_dir)
	PHP_FE(pspell_config_data_dir,		arginfo_pspell_config_data_dir)
	PHP_FE(pspell_config_repl,			arginfo_pspell_config_repl)
	PHP_FE(pspell_config_save_repl,		arginfo_pspell_config_save_repl)
	PHP_FE_END
};
/* }}} */

static int le_pspell, le_pspell_config;

zend_module_entry pspell_module_entry = {
    STANDARD_MODULE_HEADER,
	"pspell", pspell_functions, PHP_MINIT(pspell), NULL, NULL, NULL, PHP_MINFO(pspell), PHP_PSPELL_VERSION, STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PSPELL
ZEND_GET_MODULE(pspell)
#endif

static void php_pspell_close(zend_resource *rsrc)
{
	PspellManager *manager = (PspellManager *)rsrc->ptr;

	delete_pspell_manager(manager);
}

static void php_pspell_close_config(zend_resource *rsrc)
{
	PspellConfig *config = (PspellConfig *)rsrc->ptr;

	delete_pspell_config(config);
}

#define PSPELL_FETCH_CONFIG  do { \
	zval *res = zend_hash_index_find(&EG(regular_list), conf); \
	if (res == NULL || Z_RES_P(res)->type != le_pspell_config) { \
		php_error_docref(NULL, E_WARNING, "%ld is not a PSPELL config index", conf); \
		RETURN_FALSE; \
	} \
	config = (PspellConfig *)Z_RES_P(res)->ptr; \
} while (0)

#define PSPELL_FETCH_MANAGER do { \
	zval *res = zend_hash_index_find(&EG(regular_list), scin); \
	if (res == NULL || Z_RES_P(res)->type != le_pspell) { \
		php_error_docref(NULL, E_WARNING, "%ld is not a PSPELL result index", scin); \
		RETURN_FALSE; \
	} \
	manager = (PspellManager *)Z_RES_P(res)->ptr; \
} while (0);

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(pspell)
{
	REGISTER_LONG_CONSTANT("PSPELL_FAST", PSPELL_FAST, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("PSPELL_NORMAL", PSPELL_NORMAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("PSPELL_BAD_SPELLERS", PSPELL_BAD_SPELLERS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("PSPELL_RUN_TOGETHER", PSPELL_RUN_TOGETHER, CONST_PERSISTENT | CONST_CS);
	le_pspell = zend_register_list_destructors_ex(php_pspell_close, NULL, "pspell", module_number);
	le_pspell_config = zend_register_list_destructors_ex(php_pspell_close_config, NULL, "pspell config", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ proto int pspell_new(string language [, string spelling [, string jargon [, string encoding [, int mode]]]])
   Load a dictionary */
static PHP_FUNCTION(pspell_new)
{
	char *language, *spelling = NULL, *jargon = NULL, *encoding = NULL;
	size_t language_len, spelling_len = 0, jargon_len = 0, encoding_len = 0;
	zend_long mode = Z_L(0),  speed = Z_L(0);
	int argc = ZEND_NUM_ARGS();
	zval *ind;

#ifdef PHP_WIN32
	TCHAR aspell_dir[200];
	TCHAR data_dir[220];
	TCHAR dict_dir[220];
	HKEY hkey;
	DWORD dwType,dwLen;
#endif

	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;

	if (zend_parse_parameters(argc, "s|sssl", &language, &language_len, &spelling, &spelling_len,
		&jargon, &jargon_len, &encoding, &encoding_len, &mode) == FAILURE) {
		return;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
	/* If aspell was installed using installer, we should have a key
	 * pointing to the location of the dictionaries
	 */
	if (0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if (result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	pspell_config_replace(config, "language-tag", language);

	if (spelling_len) {
		pspell_config_replace(config, "spelling", spelling);
	}

	if (jargon_len) {
		pspell_config_replace(config, "jargon", jargon);
	}

	if (encoding_len) {
		pspell_config_replace(config, "encoding", encoding);
	}

	if (argc > 4) {
		speed = mode & PSPELL_SPEED_MASK_INTERNAL;

		/* First check what mode we want (how many suggestions) */
		if (speed == PSPELL_FAST) {
			pspell_config_replace(config, "sug-mode", "fast");
		} else if (speed == PSPELL_NORMAL) {
			pspell_config_replace(config, "sug-mode", "normal");
		} else if (speed == PSPELL_BAD_SPELLERS) {
			pspell_config_replace(config, "sug-mode", "bad-spellers");
		}

		/* Then we see if run-together words should be treated as valid components */
		if (mode & PSPELL_RUN_TOGETHER) {
			pspell_config_replace(config, "run-together", "true");
		}
	}

	ret = new_pspell_manager(config);
	delete_pspell_config(config);

	if (pspell_error_number(ret) != 0) {
		php_error_docref(NULL, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}

	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(Z_RES_HANDLE_P(ind));
}
/* }}} */

/* {{{ proto int pspell_new_personal(string personal, string language [, string spelling [, string jargon [, string encoding [, int mode]]]])
   Load a dictionary with a personal wordlist*/
static PHP_FUNCTION(pspell_new_personal)
{
	char *personal, *language, *spelling = NULL, *jargon = NULL, *encoding = NULL;
	size_t personal_len, language_len, spelling_len = 0, jargon_len = 0, encoding_len = 0;
	zend_long mode = Z_L(0),  speed = Z_L(0);
	int argc = ZEND_NUM_ARGS();
	zval *ind;

#ifdef PHP_WIN32
	TCHAR aspell_dir[200];
	TCHAR data_dir[220];
	TCHAR dict_dir[220];
	HKEY hkey;
	DWORD dwType,dwLen;
#endif

	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;

	if (zend_parse_parameters(argc, "ps|sssl", &personal, &personal_len, &language, &language_len,
		&spelling, &spelling_len, &jargon, &jargon_len, &encoding, &encoding_len, &mode) == FAILURE) {
		return;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
	/* If aspell was installed using installer, we should have a key
	 * pointing to the location of the dictionaries
	 */
	if (0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if (result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	if (php_check_open_basedir(personal)) {
		delete_pspell_config(config);
		RETURN_FALSE;
	}

	pspell_config_replace(config, "personal", personal);
	pspell_config_replace(config, "save-repl", "false");

	pspell_config_replace(config, "language-tag", language);

	if (spelling_len) {
		pspell_config_replace(config, "spelling", spelling);
	}

	if (jargon_len) {
		pspell_config_replace(config, "jargon", jargon);
	}

	if (encoding_len) {
		pspell_config_replace(config, "encoding", encoding);
	}

	if (argc > 5) {
		speed = mode & PSPELL_SPEED_MASK_INTERNAL;

		/* First check what mode we want (how many suggestions) */
		if (speed == PSPELL_FAST) {
			pspell_config_replace(config, "sug-mode", "fast");
		} else if (speed == PSPELL_NORMAL) {
			pspell_config_replace(config, "sug-mode", "normal");
		} else if (speed == PSPELL_BAD_SPELLERS) {
			pspell_config_replace(config, "sug-mode", "bad-spellers");
		}

		/* Then we see if run-together words should be treated as valid components */
		if (mode & PSPELL_RUN_TOGETHER) {
			pspell_config_replace(config, "run-together", "true");
		}
	}

	ret = new_pspell_manager(config);
	delete_pspell_config(config);

	if (pspell_error_number(ret) != 0) {
		php_error_docref(NULL, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}

	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(Z_RES_HANDLE_P(ind));
}
/* }}} */

/* {{{ proto int pspell_new_config(int config)
   Load a dictionary based on the given config */
static PHP_FUNCTION(pspell_new_config)
{
	zend_long conf;
	zval *ind;
	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &conf) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	ret = new_pspell_manager(config);

	if (pspell_error_number(ret) != 0) {
		php_error_docref(NULL, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}

	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(Z_RES_HANDLE_P(ind));
}
/* }}} */

/* {{{ proto bool pspell_check(int pspell, string word)
   Returns true if word is valid */
static PHP_FUNCTION(pspell_check)
{
	size_t word_len;
	zend_long scin;
	char *word;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &scin, &word, &word_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	if (pspell_manager_check(manager, word)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array pspell_suggest(int pspell, string word)
   Returns array of suggestions */
static PHP_FUNCTION(pspell_suggest)
{
	zend_long scin;
	char *word;
	size_t word_len;
	PspellManager *manager;
	const PspellWordList *wl;
	const char *sug;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &scin, &word, &word_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	array_init(return_value);

	wl = pspell_manager_suggest(manager, word);
	if (wl) {
		PspellStringEmulation *els = pspell_word_list_elements(wl);
		while ((sug = pspell_string_emulation_next(els)) != 0) {
			add_next_index_string(return_value,(char *)sug);
		}
		delete_pspell_string_emulation(els);
	} else {
		php_error_docref(NULL, E_WARNING, "PSPELL had a problem. details: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_store_replacement(int pspell, string misspell, string correct)
   Notify the dictionary of a user-selected replacement */
static PHP_FUNCTION(pspell_store_replacement)
{
	size_t miss_len, corr_len;
	zend_long scin;
	char *miss, *corr;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lss", &scin, &miss, &miss_len, &corr, &corr_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	pspell_manager_store_replacement(manager, miss, corr);
	if (pspell_manager_error_number(manager) == 0) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "pspell_store_replacement() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_add_to_personal(int pspell, string word)
   Adds a word to a personal list */
static PHP_FUNCTION(pspell_add_to_personal)
{
	size_t word_len;
	zend_long scin;
	char *word;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &scin, &word, &word_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	/*If the word is empty, we have to return; otherwise we'll segfault! ouch!*/
	if (word_len == 0) {
		RETURN_FALSE;
	}

	pspell_manager_add_to_personal(manager, word);
	if (pspell_manager_error_number(manager) == 0) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "pspell_add_to_personal() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_add_to_session(int pspell, string word)
   Adds a word to the current session */
static PHP_FUNCTION(pspell_add_to_session)
{
	size_t word_len;
	zend_long scin;
	char *word;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &scin, &word, &word_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	/*If the word is empty, we have to return; otherwise we'll segfault! ouch!*/
	if (word_len == 0) {
		RETURN_FALSE;
	}

	pspell_manager_add_to_session(manager, word);
	if (pspell_manager_error_number(manager) == 0) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "pspell_add_to_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_clear_session(int pspell)
   Clears the current session */
static PHP_FUNCTION(pspell_clear_session)
{
	zend_long scin;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &scin) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	pspell_manager_clear_session(manager);
	if (pspell_manager_error_number(manager) == 0) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "pspell_clear_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_save_wordlist(int pspell)
   Saves the current (personal) wordlist */
static PHP_FUNCTION(pspell_save_wordlist)
{
	zend_long scin;
	PspellManager *manager;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &scin) == FAILURE) {
		return;
	}

	PSPELL_FETCH_MANAGER;

	pspell_manager_save_all_word_lists(manager);

	if (pspell_manager_error_number(manager) == 0) {
		RETURN_TRUE;
	} else {
		php_error_docref(NULL, E_WARNING, "pspell_save_wordlist() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto int pspell_config_create(string language [, string spelling [, string jargon [, string encoding]]])
   Create a new config to be used later to create a manager */
static PHP_FUNCTION(pspell_config_create)
{
	char *language, *spelling = NULL, *jargon = NULL, *encoding = NULL;
	size_t language_len, spelling_len = 0, jargon_len = 0, encoding_len = 0;
	zval *ind;
	PspellConfig *config;

#ifdef PHP_WIN32
	TCHAR aspell_dir[200];
	TCHAR data_dir[220];
	TCHAR dict_dir[220];
	HKEY hkey;
	DWORD dwType,dwLen;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sss", &language, &language_len, &spelling, &spelling_len,
		&jargon, &jargon_len, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
    /* If aspell was installed using installer, we should have a key
     * pointing to the location of the dictionaries
     */
	if (0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if (result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	pspell_config_replace(config, "language-tag", language);

 	if (spelling_len) {
		pspell_config_replace(config, "spelling", spelling);
	}

	if (jargon_len) {
		pspell_config_replace(config, "jargon", jargon);
	}

	if (encoding_len) {
		pspell_config_replace(config, "encoding", encoding);
	}

	/* By default I do not want to write anything anywhere because it'll try to write to $HOME
	which is not what we want */
	pspell_config_replace(config, "save-repl", "false");

	ind = zend_list_insert(config, le_pspell_config);
	RETURN_LONG(Z_RES_HANDLE_P(ind));
}
/* }}} */

/* {{{ proto bool pspell_config_runtogether(int conf, bool runtogether)
   Consider run-together words as valid components */
static PHP_FUNCTION(pspell_config_runtogether)
{
	zend_long conf;
	zend_bool runtogether;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lb", &conf, &runtogether) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	pspell_config_replace(config, "run-together", runtogether ? "true" : "false");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_mode(int conf, long mode)
   Select mode for config (PSPELL_FAST, PSPELL_NORMAL or PSPELL_BAD_SPELLERS) */
static PHP_FUNCTION(pspell_config_mode)
{
	zend_long conf, mode;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &conf, &mode) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	/* First check what mode we want (how many suggestions) */
	if (mode == PSPELL_FAST) {
		pspell_config_replace(config, "sug-mode", "fast");
	} else if (mode == PSPELL_NORMAL) {
		pspell_config_replace(config, "sug-mode", "normal");
	} else if (mode == PSPELL_BAD_SPELLERS) {
		pspell_config_replace(config, "sug-mode", "bad-spellers");
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_ignore(int conf, int ignore)
   Ignore words <= n chars */
static PHP_FUNCTION(pspell_config_ignore)
{
	char ignore_str[MAX_LENGTH_OF_LONG + 1];
	zend_long conf, ignore = 0L;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &conf, &ignore) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	snprintf(ignore_str, sizeof(ignore_str), "%ld", ignore);

	pspell_config_replace(config, "ignore", ignore_str);
	RETURN_TRUE;
}
/* }}} */

static void pspell_config_path(INTERNAL_FUNCTION_PARAMETERS, char *option)
{
	zend_long conf;
	char *value;
	size_t value_len;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lp", &conf, &value, &value_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	if (php_check_open_basedir(value)) {
		RETURN_FALSE;
	}

	pspell_config_replace(config, option, value);

	RETURN_TRUE;
}

/* {{{ proto bool pspell_config_personal(int conf, string personal)
   Use a personal dictionary for this config */
static PHP_FUNCTION(pspell_config_personal)
{
	pspell_config_path(INTERNAL_FUNCTION_PARAM_PASSTHRU, "personal");
}
/* }}} */

/* {{{ proto bool pspell_config_dict_dir(int conf, string directory)
   location of the main word list */
static PHP_FUNCTION(pspell_config_dict_dir)
{
	pspell_config_path(INTERNAL_FUNCTION_PARAM_PASSTHRU, "dict-dir");
}
/* }}} */

/* {{{ proto bool pspell_config_data_dir(int conf, string directory)
    location of language data files */
static PHP_FUNCTION(pspell_config_data_dir)
{
	pspell_config_path(INTERNAL_FUNCTION_PARAM_PASSTHRU, "data-dir");
}
/* }}} */

/* {{{ proto bool pspell_config_repl(int conf, string repl)
   Use a personal dictionary with replacement pairs for this config */
static PHP_FUNCTION(pspell_config_repl)
{
	zend_long conf;
	char *repl;
	size_t repl_len;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lp", &conf, &repl, &repl_len) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	pspell_config_replace(config, "save-repl", "true");

	if (php_check_open_basedir(repl)) {
		RETURN_FALSE;
	}

	pspell_config_replace(config, "repl", repl);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_save_repl(int conf, bool save)
   Save replacement pairs when personal list is saved for this config */
static PHP_FUNCTION(pspell_config_save_repl)
{
	zend_long conf;
	zend_bool save;
	PspellConfig *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lb", &conf, &save) == FAILURE) {
		return;
	}

	PSPELL_FETCH_CONFIG;

	pspell_config_replace(config, "save-repl", save ? "true" : "false");

	RETURN_TRUE;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(pspell)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PSpell Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
