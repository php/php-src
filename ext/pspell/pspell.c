/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vlad Krupin <phpdevel@echospace.com>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define IS_EXT_MODULE

#include "php.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#if HAVE_PSPELL

#include "php_pspell.h"
#include <pspell/pspell.h>
#include "ext/standard/info.h"

#define PSPELL_FAST 1L
#define PSPELL_NORMAL 2L
#define PSPELL_BAD_SPELLERS 3L
#define PSPELL_SPEED_MASK_INTERNAL 3L
#define PSPELL_RUN_TOGETHER 8L

function_entry pspell_functions[] = {
	PHP_FE(pspell_new,		NULL)
	PHP_FE(pspell_check,		NULL)
	PHP_FE(pspell_suggest,		NULL)
	PHP_FE(pspell_store_replacement,		NULL)
	PHP_FE(pspell_add_to_personal,		NULL)
	PHP_FE(pspell_add_to_session,		NULL)
	PHP_FE(pspell_clear_session,		NULL)
};

static int le_pspell;

zend_module_entry pspell_module_entry = {
	"pspell", pspell_functions, PHP_MINIT(pspell), NULL, NULL, NULL, PHP_MINFO(pspell), STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PSPELL
ZEND_GET_MODULE(pspell)
#endif

static void php_pspell_close(PspellManager *manager){
	delete_pspell_manager(manager);
}


PHP_MINIT_FUNCTION(pspell){
	REGISTER_MAIN_LONG_CONSTANT("PSPELL_FAST", PSPELL_FAST, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PSPELL_NORMAL", PSPELL_NORMAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PSPELL_BAD_SPELLERS", PSPELL_BAD_SPELLERS, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PSPELL_RUN_TOGETHER", PSPELL_RUN_TOGETHER, CONST_PERSISTENT | CONST_CS);
	le_pspell = register_list_destructors(php_pspell_close,NULL);
	return SUCCESS;
}

/* {{{ proto int pspell_new(string language [, string spelling [, string jargon [, string encoding [, int mode]]]])
   Load a dictionary */
PHP_FUNCTION(pspell_new){
	zval **language,**spelling,**jargon,**encoding,**pmode;
	long mode = 0L,  speed = 0L;
	int argc;
	int ind;

	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 5 || zend_get_parameters_ex(argc,&language,&spelling,&jargon,&encoding,&pmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = new_pspell_config();
	convert_to_string_ex(language);
	pspell_config_replace(config, "language-tag", (*language)->value.str.val);

	if(argc > 1){
		convert_to_string_ex(spelling);
	 	if((*spelling)->value.str.len > 0){
			pspell_config_replace(config, "spelling", (*spelling)->value.str.val);
		}
	}

	if(argc > 2){
		convert_to_string_ex(jargon);
		if((*jargon)->value.str.len > 0){
			pspell_config_replace(config, "jargon", (*jargon)->value.str.val);
		}
	}

	if(argc > 3){
		convert_to_string_ex(encoding);
		if((*encoding)->value.str.len > 0){
			pspell_config_replace(config, "encoding", (*encoding)->value.str.val);
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
		php_error(E_WARNING, "PSPELL couldn't open the dictionary. reason: %s ", pspell_error_message(ret));
		RETURN_FALSE;
	}
	
	manager = to_pspell_manager(ret);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int pspell_check(int pspell, string word)
   Returns true if word is valid */
PHP_FUNCTION(pspell_check){
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_long_ex(scin);
	convert_to_string_ex(word);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}

	if(pspell_manager_check(manager, (*word)->value.str.val)){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array pspell_suggest(int pspell, string word)
   Returns array of suggestions */
PHP_FUNCTION(pspell_suggest)
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
    
	convert_to_long_ex(scin);
	convert_to_string_ex(word);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
	RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE){
		RETURN_FALSE;
	}

	wl = pspell_manager_suggest(manager, (*word)->value.str.val);
	if(wl){
		PspellStringEmulation *els = pspell_word_list_elements(wl);
		while((sug = pspell_string_emulation_next(els)) != 0){
			add_next_index_string(return_value,(char *)sug,1);
		}
		delete_pspell_string_emulation(els);
	}else{
		php_error(E_WARNING, "PSPELL had a problem. details: %s ", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int pspell_store_replacement(int pspell, string misspell, string correct)
   Notify the dictionary of a user-selected replacement */
PHP_FUNCTION(pspell_store_replacement)
{
	int type;
	zval **scin,**miss,**corr;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 3 || zend_get_parameters_ex(argc, &scin,&miss,&corr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_long_ex(scin);
	convert_to_string_ex(miss);
	convert_to_string_ex(corr);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}

	pspell_manager_store_replacement(manager, (*miss)->value.str.val, (*corr)->value.str.val);
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error(E_WARNING, "pspell_store_replacement() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int pspell_add_to_personal(int pspell, string word)
   Adds a word to a personal list */
PHP_FUNCTION(pspell_add_to_personal)
{
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_long_ex(scin);
	convert_to_string_ex(word);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}

	pspell_manager_add_to_personal(manager, (*word)->value.str.val);
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error(E_WARNING, "pspell_add_to_personal() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int pspell_add_to_session(int pspell, string word)
   Adds a word to the current session */
PHP_FUNCTION(pspell_add_to_session)
{
	int type;
	zval **scin,**word;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_long_ex(scin);
	convert_to_string_ex(word);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}

	pspell_manager_add_to_session(manager, (*word)->value.str.val);
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error(E_WARNING, "pspell_add_to_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto int pspell_clear_session(int pspell)
   Clears the current session */
PHP_FUNCTION(pspell_clear_session)
{
	int type;
	zval **scin;
	PspellManager *manager;

	int argc;
	argc = ZEND_NUM_ARGS();
	if (argc != 1 || zend_get_parameters_ex(argc, &scin) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
    
	convert_to_long_ex(scin);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}

	pspell_manager_clear_session(manager);
	if(pspell_manager_error_number(manager) == 0){
		RETURN_TRUE;
	}else{
		php_error(E_WARNING, "pspell_clear_session() gave error: %s", pspell_manager_error_message(manager));
		RETURN_FALSE;
	}
}
/* }}} */


PHP_MINFO_FUNCTION(pspell)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PSpell Support", "enabled");
	php_info_print_table_end();
}

#endif
