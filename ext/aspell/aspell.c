/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
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
	TLS_VARS;
	
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

	TLS_VARS;
	
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
   TLS_VARS;
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
   TLS_VARS;

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

void php3_info_aspell(void)
{
	TLS_VARS;
	php3_printf("ASpell support enabled");

}

#endif
