/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#if defined(COMPILE_DL)
#include "phpdl.h"
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#if HAVE_ASPELL

#include "php3_aspell.h"
#if APACHE
#  ifndef DEBUG
#  undef palloc
#  endif
#endif
#include <aspell-c.h>

function_entry aspell_functions[] = {
	{"aspell_new",		php3_aspell_new,		NULL},
	{"aspell_check",		php3_aspell_check,		NULL},
	{"aspell_check_raw",		php3_aspell_check_raw,		NULL},
	{"aspell_suggest",		php3_aspell_suggest,		NULL},
	{NULL, NULL, NULL}
};

static int le_aspell;

php3_module_entry aspell_module_entry = {
	"Aspell", aspell_functions, php3_minit_aspell, NULL, NULL, NULL, php3_info_aspell, STANDARD_MODULE_PROPERTIES
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &aspell_module_entry; }
#endif

int php3_minit_aspell(INIT_FUNC_ARGS)
{
    le_aspell = register_list_destructors(php3_aspell_close,NULL);
	return SUCCESS;

}
void php3_aspell_close(aspell *sc)
{
	aspell_free(sc);
}

/* {{{ proto int aspell_new(string master[, string personal])
   Load a dictionary */
void php3_aspell_new(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *master, *personal;
	int argc;
	aspell *sc;
	int ind;
	
	argc = ARG_COUNT(ht);
	if (argc < 1 || argc > 2 || getParameters(ht, argc, &master,&personal) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(master);
	if(argc==2)
	  {
		convert_to_string(personal) ;
	sc=aspell_new(master->value.str.val,personal->value.str.val);
	  }
	else
	  sc=aspell_new(master->value.str.val,"");

	ind = php3_list_insert(sc, le_aspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto array aspell_suggest(aspell int,string word)
   Return array of Suggestions */
void php3_aspell_suggest(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *scin, *word;
	int argc;
	aspell *sc;
	int ind,type;
	aspellSuggestions *sug;
	size_t i;

	
	argc = ARG_COUNT(ht);
	if (argc != 2 || getParameters(ht, argc, &scin,&word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(scin);
	convert_to_string(word);
	sc= (aspell *) php3_list_find(scin->value.lval, &type);
	if(!sc)
	  {
		php3_error(E_WARNING, "%d is not a ASPELL result index", scin->value.lval);
		RETURN_FALSE;
	  }

	if (array_init(return_value) == FAILURE) {
                RETURN_FALSE;
	}

	sug = aspell_suggest(sc, word->value.str.val);
	  for (i = 0; i != sug->size; ++i) {
                add_next_index_string(return_value,(char *)sug->data[i],1);
	  }
	  aspell_free_suggestions(sug);
}
/* }}} */

/* {{{ proto int aspell_check(aspell int,string word)
   Return if word is valid */
void php3_aspell_check(INTERNAL_FUNCTION_PARAMETERS)
{
   int type;
   pval *scin,*word;
   aspell *sc;

   int argc;
    argc = ARG_COUNT(ht);
    if (argc != 2 || getParameters(ht, argc, &scin,&word) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_long(scin);
    convert_to_string(word);
    sc= (aspell *) php3_list_find(scin->value.lval, &type);
    if(!sc)
      {
        php3_error(E_WARNING, "%d is not a ASPELL result index", scin->value.lval);
        RETURN_FALSE;
      }
    if (aspell_check(sc, word->value.str.val)) 
      {
	RETURN_TRUE;
      }
    else 
      {
  RETURN_FALSE;
      }
}
/* }}} */

/* {{{ proto int aspell_check_raw(aspell int,string word)
   Return if word is valid, ignoring case or trying to trim it in any way*/
void php3_aspell_check_raw(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *scin,*word;
  int type;
  int argc;
  aspell *sc;

    argc = ARG_COUNT(ht);
    if (argc != 2 || getParameters(ht, argc, &scin,&word) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_long(scin);
    convert_to_string(word);
    sc= (aspell *) php3_list_find(scin->value.lval, &type);
    if(!sc)
      {
        php3_error(E_WARNING, "%d is not a ASPELL result index", scin->value.lval);
        RETURN_FALSE;
      }
	if (aspell_check_raw(sc, word->value.str.val)) 
	  {
	    RETURN_TRUE;
	  }
	else
	  {
	    RETURN_FALSE;
	      }
}
/* }}} */

void php3_info_aspell(ZEND_MODULE_INFO_FUNC_ARGS)
{
	php3_printf("ASpell support enabled");

}

#endif
