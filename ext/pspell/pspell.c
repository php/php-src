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

#define PSPELL_FAST 1
#define PSPELL_NORMAL 2
#define PSPELL_BAD_SPELLERS 3

function_entry pspell_functions[] = {
	PHP_FE(pspell_new,		NULL)
	PHP_FE(pspell_mode,		NULL)
	PHP_FE(pspell_runtogether,	NULL)
	PHP_FE(pspell_check,		NULL)
	PHP_FE(pspell_suggest,		NULL)
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
	le_pspell = register_list_destructors(php_pspell_close,NULL);
	return SUCCESS;
}

/* {{{ proto int pspell_new(string language [, string spelling [, string jargon [, string encoding]]])
   Load a dictionary */
PHP_FUNCTION(pspell_new){
	zval **language,**spelling,**jargon,**encoding;
	int argc;
	int ind;

	PspellCanHaveError *ret;
	PspellManager *manager;
	PspellConfig *config;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 4 || zend_get_parameters_ex(argc,&language,&spelling,&jargon,&encoding) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = new_pspell_config();
	convert_to_string_ex(language);
	if(argc > 1){
		convert_to_string_ex(spelling) ;
		pspell_config_replace(config, "spelling", (*spelling)->value.str.val);
	}

	if(argc > 2){
		convert_to_string_ex(jargon) ;
		pspell_config_replace(config, "jargon", (*jargon)->value.str.val);
	}

	if(argc > 3){
		convert_to_string_ex(encoding) ;
		pspell_config_replace(config, "encoding", (*encoding)->value.str.val);
	}

	ret = new_pspell_manager(config);
	delete_pspell_config(config);

	if(pspell_error_number(ret) != 0){
		php_error(E_WARNING, "PSPELL couldn't open the dictionary. reason: %s ", pspell_error_message(ret));
		RETURN_FALSE;
	}
	
	manager = to_pspell_manager(ret);
	config = pspell_manager_config(manager);
	ind = zend_list_insert(manager, le_pspell);
	RETURN_LONG(ind);
}
/* }}} */


/* {{{ proto int pspell_mode(string mode)
   Change the mode between 'fast', 'normal' and 'bad-spellers' */
PHP_FUNCTION(pspell_mode)
{
	int type;
	zval **scin, **pmode;
	int argc;
	long mode = 0L;
	PspellManager *manager;
	PspellConfig *config;

	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin, &pmode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(scin);
	convert_to_long_ex(pmode);
	mode = Z_LVAL_PP(pmode);
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}
	config = pspell_manager_config(manager);

	if(mode == PSPELL_FAST){
		pspell_config_replace(config, "sug-mode", "fast");
	}else if(mode == PSPELL_NORMAL){
		pspell_config_replace(config, "sug-mode", "normal");
	}else if(mode == PSPELL_BAD_SPELLERS){
		pspell_config_replace(config, "sug-mode", "bad-spellers");
	}else{
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pspell_runtogether(string mode)
   Change the mode between whether we want to treat run-together words as valid */
PHP_FUNCTION(pspell_runtogether)
{
	int type;
	zval **scin, **pruntogether;
	int argc;
	int runtogether;
	PspellManager *manager;
	PspellConfig *config;

	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin, &pruntogether) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(scin);
	convert_to_boolean_ex(pruntogether);
	runtogether = (*pruntogether)->value.lval;
	manager = (PspellManager *) zend_list_find((*scin)->value.lval, &type);
	if(!manager){
		php_error(E_WARNING, "%d is not an PSPELL result index",(*scin)->value.lval);
		RETURN_FALSE;
	}
	config = pspell_manager_config(manager);

	if(runtogether){
		pspell_config_replace(config, "run-together", "true");
	}else{
		pspell_config_replace(config, "run-together", "false");
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pspell_check(aspell int, string word)
   Return if word is valid */
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

/* {{{ proto array pspell_suggest(aspell int, string word)
   Return array of Suggestions */
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

PHP_MINFO_FUNCTION(pspell)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PSpell Support", "enabled");
	php_info_print_table_end();
}

#endif
