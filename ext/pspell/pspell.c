/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

/* {{{ pspell_functions[]
 */
static zend_function_entry pspell_functions[] = {
	PHP_FE(pspell_new,		NULL)
	PHP_FE(pspell_new_personal,		NULL)
	PHP_FE(pspell_new_config,		NULL)
	PHP_FE(pspell_check,		NULL)
	PHP_FE(pspell_suggest,		NULL)
	PHP_FE(pspell_store_replacement,		NULL)
	PHP_FE(pspell_add_to_personal,		NULL)
	PHP_FE(pspell_add_to_session,		NULL)
	PHP_FE(pspell_clear_session,		NULL)
	PHP_FE(pspell_save_wordlist,		NULL)
	PHP_FE(pspell_config_create,		NULL)
	PHP_FE(pspell_config_runtogether,		NULL)
	PHP_FE(pspell_config_mode,		NULL)
	PHP_FE(pspell_config_ignore,		NULL)
	PHP_FE(pspell_config_personal,		NULL)
	PHP_FE(pspell_config_dict_dir,		NULL)
	PHP_FE(pspell_config_data_dir,		NULL)
	PHP_FE(pspell_config_repl,		NULL)
	PHP_FE(pspell_config_save_repl,		NULL)
	{NULL, NULL, NULL} 
};
/* }}} */

static int le_pspell, le_pspell_config;

zend_module_entry pspell_module_entry = {
    STANDARD_MODULE_HEADER,
	"pspell", pspell_functions, PHP_MINIT(pspell), NULL, NULL, NULL, PHP_MINFO(pspell), NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PSPELL
ZEND_GET_MODULE(pspell)
#endif

static void php_pspell_close(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PspellManager *manager = (PspellManager *)rsrc->ptr;

	delete_pspell_manager(manager);
}

static void php_pspell_close_config(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PspellConfig *config = (PspellConfig *)rsrc->ptr;

	delete_pspell_config(config);
}

#define PSPELL_FETCH_CONFIG \
	convert_to_long_ex(conf);	\
	config = (PspellConfig *) zend_list_find(Z_LVAL_PP(conf), &type);	\
	if (config == NULL || type != le_pspell_config) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%ld is not a PSPELL config index", Z_LVAL_PP(conf));	\
		RETURN_FALSE;	\
	}	\

#define PSPELL_FETCH_MANAGER \
	convert_to_long_ex(scin);	\
	manager = (PspellManager *) zend_list_find(Z_LVAL_PP(scin), &type);	\
	if (!manager || type != le_pspell) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%ld is not a PSPELL result index", Z_LVAL_PP(scin));	\
		RETURN_FALSE;	\
	}	\

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
	zval **language,**spelling,**jargon,**encoding,**pmode;
	long mode = 0L,  speed = 0L;
	int argc;
	int ind;

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
	
	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 5 || zend_get_parameters_ex(argc,&language,&spelling,&jargon,&encoding,&pmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
	/* If aspell was installed using installer, we should have a key
	 * pointing to the location of the dictionaries
	 */
	if(0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if(result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	convert_to_string_ex(language);
	pspell_config_replace(config, "language-tag", Z_STRVAL_PP(language));

	if(argc > 1){
		convert_to_string_ex(spelling);
	 	if(Z_STRLEN_PP(spelling) > 0){
			pspell_config_replace(config, "spelling", Z_STRVAL_PP(spelling));
		}
	}

	if(argc > 2){
		convert_to_string_ex(jargon);
		if(Z_STRLEN_PP(jargon) > 0){
			pspell_config_replace(config, "jargon", Z_STRVAL_PP(jargon));
		}
	}

	if(argc > 3){
		convert_to_string_ex(encoding);
		if(Z_STRLEN_PP(encoding) > 0){
			pspell_config_replace(config, "encoding", Z_STRVAL_PP(encoding));
		}
	}

	if(argc > 4){
		convert_to_long_ex(pmode);
		mode = Z_LVAL_PP(pmode);
		speed = mode & PSPELL_SPEED_MASK_INTERNAL;

		/* First check what mode we want (how many suggestions) */
		if(speed == PSPELL_FAST){
			pspell_config_replace(config, "sug-mode", "fast");
		}else if(speed == PSPELL_NORMAL){
			pspell_config_replace(config, "sug-mode", "normal");
		}else if(speed == PSPELL_BAD_SPELLERS){
			pspell_config_replace(config, "sug-mode", "bad-spellers");
		}
		
		/* Then we see if run-together words should be treated as valid components */
		if(mode & PSPELL_RUN_TOGETHER){
			pspell_config_replace(config, "run-together", "true");
		}
	}

	ret = new_pspell_manager(config);
	delete_pspell_config(config);

	if(pspell_error_number(ret) != 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s ", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}
	
	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int pspell_new_personal(string personal, string language [, string spelling [, string jargon [, string encoding [, int mode]]]])
   Load a dictionary with a personal wordlist*/
static PHP_FUNCTION(pspell_new_personal)
{
	zval **personal, **language,**spelling,**jargon,**encoding,**pmode;
	long mode = 0L,  speed = 0L;
	int argc;
	int ind;

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
	
	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 6 || zend_get_parameters_ex(argc,&personal,&language,&spelling,&jargon,&encoding,&pmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
	/* If aspell was installed using installer, we should have a key
	 * pointing to the location of the dictionaries
	 */
	if(0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if(result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	convert_to_string_ex(personal);

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_PP(personal), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		delete_pspell_config(config);
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(personal) TSRMLS_CC)) {
		delete_pspell_config(config);
		RETURN_FALSE;
	}

	pspell_config_replace(config, "personal", Z_STRVAL_PP(personal));
	pspell_config_replace(config, "save-repl", "false");

	convert_to_string_ex(language);
	pspell_config_replace(config, "language-tag", Z_STRVAL_PP(language));

	if(argc > 2){
		convert_to_string_ex(spelling);
	 	if(Z_STRLEN_PP(spelling) > 0){
			pspell_config_replace(config, "spelling", Z_STRVAL_PP(spelling));
		}
	}

	if(argc > 3){
		convert_to_string_ex(jargon);
		if(Z_STRLEN_PP(jargon) > 0){
			pspell_config_replace(config, "jargon", Z_STRVAL_PP(jargon));
		}
	}

	if(argc > 4){
		convert_to_string_ex(encoding);
		if(Z_STRLEN_PP(encoding) > 0){
			pspell_config_replace(config, "encoding", Z_STRVAL_PP(encoding));
		}
	}

	if(argc > 5){
		convert_to_long_ex(pmode);
		mode = Z_LVAL_PP(pmode);
		speed = mode & PSPELL_SPEED_MASK_INTERNAL;

		/* First check what mode we want (how many suggestions) */
		if(speed == PSPELL_FAST){
			pspell_config_replace(config, "sug-mode", "fast");
		}else if(speed == PSPELL_NORMAL){
			pspell_config_replace(config, "sug-mode", "normal");
		}else if(speed == PSPELL_BAD_SPELLERS){
			pspell_config_replace(config, "sug-mode", "bad-spellers");
		}
		
		/* Then we see if run-together words should be treated as valid components */
		if(mode & PSPELL_RUN_TOGETHER){
			pspell_config_replace(config, "run-together", "true");
		}
	}

	ret = new_pspell_manager(config);
	delete_pspell_config(config);

	if(pspell_error_number(ret) != 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s ", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}
	
	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int pspell_new_config(int config)
   Load a dictionary based on the given config */
static PHP_FUNCTION(pspell_new_config)
{
	int type;
	zval **conf;
	int argc;
	int ind;
	
	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc,&conf) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	ret = new_pspell_manager(config);

	if(pspell_error_number(ret) != 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PSPELL couldn't open the dictionary. reason: %s ", pspell_error_message(ret));
		delete_pspell_can_have_error(ret);
		RETURN_FALSE;
	}
	
	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto bool pspell_check(int pspell, string word)
   Returns true if word is valid */
static PHP_FUNCTION(pspell_check)
{
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(word);

	PSPELL_FETCH_MANAGER;

	if(pspell_manager_check(manager, Z_STRVAL_PP(word))){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array pspell_suggest(int pspell, string word)
   Returns array of suggestions */
static PHP_FUNCTION(pspell_suggest)
{
	zval **scin, **word;
	int argc;
	PspellManager *manager;
	int type;
	const PspellWordList *wl;
	const char *sug;

	argc = ZEND_NUM_ARGS();
	if(argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(word);
	PSPELL_FETCH_MANAGER;

	array_init(return_value);

	wl = pspell_manager_suggest(manager, Z_STRVAL_PP(word));
	if(wl){
		PspellStringEmulation *els = pspell_word_list_elements(wl);
		while((sug = pspell_string_emulation_next(els)) != 0){
			add_next_index_string(return_value,(char *)sug,1);
		}
		delete_pspell_string_emulation(els);
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "PSPELL had a problem. details: %s ", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_store_replacement(int pspell, string misspell, string correct)
   Notify the dictionary of a user-selected replacement */
static PHP_FUNCTION(pspell_store_replacement)
{
	int type;
	zval **scin,**miss,**corr;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 3 || zend_get_parameters_ex(argc, &scin,&miss,&corr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(miss);
	convert_to_string_ex(corr);
	PSPELL_FETCH_MANAGER;

	pspell_manager_store_replacement(manager, Z_STRVAL_PP(miss), Z_STRVAL_PP(corr));
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "pspell_store_replacement() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_add_to_personal(int pspell, string word)
   Adds a word to a personal list */
static PHP_FUNCTION(pspell_add_to_personal)
{
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(word);
	PSPELL_FETCH_MANAGER;

	/*If the word is empty, we have to return; otherwise we'll segfault! ouch!*/
	if(Z_STRLEN_PP(word) == 0){
		RETURN_FALSE;
	}
	
	pspell_manager_add_to_personal(manager, Z_STRVAL_PP(word));
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "pspell_add_to_personal() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_add_to_session(int pspell, string word)
   Adds a word to the current session */
static PHP_FUNCTION(pspell_add_to_session)
{
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_string_ex(word);
	PSPELL_FETCH_MANAGER;

	/*If the word is empty, we have to return; otherwise we'll segfault! ouch!*/
	if(Z_STRLEN_PP(word) == 0){
		RETURN_FALSE;
	}

	pspell_manager_add_to_session(manager, Z_STRVAL_PP(word));
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "pspell_add_to_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_clear_session(int pspell)
   Clears the current session */
static PHP_FUNCTION(pspell_clear_session)
{
	int type;
	zval **scin;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &scin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	PSPELL_FETCH_MANAGER;	

	pspell_manager_clear_session(manager);
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "pspell_clear_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pspell_save_wordlist(int pspell)
   Saves the current (personal) wordlist */
static PHP_FUNCTION(pspell_save_wordlist)
{
	int type;
	zval **scin;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &scin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	PSPELL_FETCH_MANAGER;	

	pspell_manager_save_all_word_lists(manager);

	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "pspell_save_wordlist() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto int pspell_config_create(string language [, string spelling [, string jargon [, string encoding]]])
   Create a new config to be used later to create a manager */
static PHP_FUNCTION(pspell_config_create)
{
	zval **language,**spelling,**jargon,**encoding;
	int argc;
	int ind;

	PspellConfig *config;

#ifdef PHP_WIN32
	TCHAR aspell_dir[200];
	TCHAR data_dir[220];
	TCHAR dict_dir[220];
	HKEY hkey;
	DWORD dwType,dwLen;
#endif
	
	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 4 || zend_get_parameters_ex(argc,&language,&spelling,&jargon,&encoding) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = new_pspell_config();

#ifdef PHP_WIN32
    /* If aspell was installed using installer, we should have a key
     * pointing to the location of the dictionaries
     */
	if(0 == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Aspell", &hkey)) {
		LONG result;
		dwLen = sizeof(aspell_dir) - 1;
		result = RegQueryValueEx(hkey, "", NULL, &dwType, (LPBYTE)&aspell_dir, &dwLen);
		RegCloseKey(hkey);
		if(result == ERROR_SUCCESS) {
			strlcpy(data_dir, aspell_dir, sizeof(data_dir));
			strlcat(data_dir, "\\data", sizeof(data_dir));
			strlcpy(dict_dir, aspell_dir, sizeof(dict_dir));
			strlcat(dict_dir, "\\dict", sizeof(dict_dir));

			pspell_config_replace(config, "data-dir", data_dir);
			pspell_config_replace(config, "dict-dir", dict_dir);
		}
	}
#endif

	convert_to_string_ex(language);
	pspell_config_replace(config, "language-tag", Z_STRVAL_PP(language));

	if(argc > 1){
		convert_to_string_ex(spelling);
	 	if(Z_STRLEN_PP(spelling) > 0){
			pspell_config_replace(config, "spelling", Z_STRVAL_PP(spelling));
		}
	}

	if(argc > 2){
		convert_to_string_ex(jargon);
		if(Z_STRLEN_PP(jargon) > 0){
			pspell_config_replace(config, "jargon", Z_STRVAL_PP(jargon));
		}
	}

	if(argc > 3){
		convert_to_string_ex(encoding);
		if(Z_STRLEN_PP(encoding) > 0){
			pspell_config_replace(config, "encoding", Z_STRVAL_PP(encoding));
		}
	}

	/* By default I do not want to write anything anywhere because it'll try to write to $HOME
	which is not what we want */
	pspell_config_replace(config, "save-repl", "false");

	ind = zend_list_insert(config, le_pspell_config);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto bool pspell_config_runtogether(int conf, bool runtogether)
   Consider run-together words as valid components */
static PHP_FUNCTION(pspell_config_runtogether)
{
	int type;
	zval **conf, **runtogether;
	int argc;

	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc,&conf,&runtogether) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;	

	convert_to_boolean_ex(runtogether);
	pspell_config_replace(config, "run-together", Z_LVAL_PP(runtogether) ? "true" : "false");
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_mode(int conf, long mode)
   Select mode for config (PSPELL_FAST, PSPELL_NORMAL or PSPELL_BAD_SPELLERS) */
static PHP_FUNCTION(pspell_config_mode)
{
	int type;
	zval **conf, **mode;
	int argc;

	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc,&conf,&mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	convert_to_long_ex(mode);

	/* First check what mode we want (how many suggestions) */
	if(Z_LVAL_PP(mode) == PSPELL_FAST){
		pspell_config_replace(config, "sug-mode", "fast");
	}else if(Z_LVAL_PP(mode) == PSPELL_NORMAL){
		pspell_config_replace(config, "sug-mode", "normal");
	}else if(Z_LVAL_PP(mode) == PSPELL_BAD_SPELLERS){
		pspell_config_replace(config, "sug-mode", "bad-spellers");
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_ignore(int conf, int ignore)
   Ignore words <= n chars */
static PHP_FUNCTION(pspell_config_ignore)
{
	int type;
	zval **conf, **pignore;
	int argc;

	char ignore_str[MAX_LENGTH_OF_LONG + 1];	
	long ignore = 0L;

	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc,&conf,&pignore) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	convert_to_long_ex(pignore);
	ignore = Z_LVAL_PP(pignore);

	snprintf(ignore_str, sizeof(ignore_str), "%ld", ignore);

	pspell_config_replace(config, "ignore", ignore_str);
	RETURN_TRUE;
}
/* }}} */

static void pspell_config_path(INTERNAL_FUNCTION_PARAMETERS, char *option)
{
	int type;
	zval **conf, **value;
	int argc;
	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &conf, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	convert_to_string_ex(value);

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_PP(value), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(value) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	pspell_config_replace(config, option, Z_STRVAL_PP(value));

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
	int type;
	zval **conf, **repl;
	int argc;

	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc,&conf,&repl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	pspell_config_replace(config, "save-repl", "true");

	convert_to_string_ex(repl);

	if (PG(safe_mode) && (!php_checkuid(Z_STRVAL_PP(repl), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(Z_STRVAL_PP(repl) TSRMLS_CC)) {
		RETURN_FALSE;
	}

	pspell_config_replace(config, "repl", Z_STRVAL_PP(repl));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pspell_config_save_repl(int conf, bool save)
   Save replacement pairs when personal list is saved for this config */
static PHP_FUNCTION(pspell_config_save_repl)
{
	int type;
	zval **conf, **save;
	int argc;

	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc,&conf,&save) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSPELL_FETCH_CONFIG;

	convert_to_boolean_ex(save);
	pspell_config_replace(config, "save-repl", Z_LVAL_PP(save) ? "true" : "false");

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
