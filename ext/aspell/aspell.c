/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef COMPILE_DL_ASPELL
#include "phpdl.h"
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#if HAVE_ASPELL

#include "php_aspell.h"
#include <aspell-c.h>
#include "ext/standard/info.h"

/* {{{ aspell_functions[]
 */
function_entry aspell_functions[] = {
	PHP_FE(aspell_new,								NULL)
	PHP_FE(aspell_check,							NULL)
	PHP_FE(aspell_check_raw,						NULL)
	PHP_FE(aspell_suggest,							NULL)
	{NULL, NULL, NULL}
};
/* }}} */

static int le_aspell;

zend_module_entry aspell_module_entry = {
    STANDARD_MODULE_HEADER,
	"aspell", aspell_functions, PHP_MINIT(aspell), NULL, NULL, NULL, PHP_MINFO(aspell), NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ASPELL
ZEND_GET_MODULE(aspell)
#endif

/* {{{ php_aspell_close
 */
static void php_aspell_close(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	aspell *sc = (aspell *)rsrc->ptr;

	aspell_free(sc);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(aspell)
{
    le_aspell = zend_register_list_destructors_ex(php_aspell_close, NULL, "aspell", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ proto int aspell_new(string master [, string personal])
   Load a dictionary */
PHP_FUNCTION(aspell_new)
{
	pval **master, **personal;
	int argc;
	aspell *sc;
	int ind;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &master, &personal) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(master);
	if(argc==2)
	  {
		convert_to_string_ex(personal) ;
		sc=aspell_new(Z_STRVAL_PP(master), Z_STRVAL_PP(personal));
	  }
	else
	  sc=aspell_new(Z_STRVAL_PP(master), "");

	ind = zend_list_insert(sc, le_aspell);
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto array aspell_suggest(aspell int, string word)
   Return array of Suggestions */
PHP_FUNCTION(aspell_suggest)
{
	pval **scin, **word;
	int argc;
	aspell *sc;
	int ind, type;
	aspellSuggestions *sug;
	size_t i;

	
	argc = ZEND_NUM_ARGS();
	if (argc != 2 || zend_get_parameters_ex(argc, &scin, &word) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(scin);
	convert_to_string_ex(word);
	sc = (aspell *)zend_list_find(Z_LVAL_PP(scin), &type);
	if(!sc)
	  {
		php_error(E_WARNING, "%s(): %d is not an ASPELL result index", get_active_function_name(TSRMLS_C), Z_LVAL_PP(scin));
		RETURN_FALSE;
	  }

	if (array_init(return_value) == FAILURE) {
                RETURN_FALSE;
	}

	sug = aspell_suggest(sc, Z_STRVAL_PP(word));
	  for (i = 0; i != sug->size; ++i) {
                add_next_index_string(return_value, (char *)sug->data[i], 1);
	  }
	  aspell_free_suggestions(sug);
}
/* }}} */

/* {{{ proto int aspell_check(aspell int, string word)
   Return if word is valid */
PHP_FUNCTION(aspell_check)
{
   int type;
   pval **scin, **word;
   aspell *sc;

   int argc;
    argc = ZEND_NUM_ARGS();
    if (argc != 2 || zend_get_parameters_ex(argc, &scin, &word) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_long_ex(scin);
    convert_to_string_ex(word);
    sc= (aspell *) zend_list_find(Z_LVAL_PP(scin), &type);
    if(!sc)
      {
        php_error(E_WARNING, "%s(): %d is not an ASPELL result index", get_active_function_name(TSRMLS_C), Z_LVAL_PP(scin));
        RETURN_FALSE;
      }
    if (aspell_check(sc, Z_STRVAL_PP(word))) 
      {
	RETURN_TRUE;
      }
    else 
      {
  RETURN_FALSE;
      }
}
/* }}} */

/* {{{ proto int aspell_check_raw(aspell int, string word)
   Return if word is valid, ignoring case or trying to trim it in any way */
PHP_FUNCTION(aspell_check_raw)
{
  pval **scin, **word;
  int type;
  int argc;
  aspell *sc;

    argc = ZEND_NUM_ARGS();
    if (argc != 2 || zend_get_parameters_ex(argc, &scin, &word) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_long_ex(scin);
    convert_to_string_ex(word);
    sc = (aspell *)zend_list_find(Z_LVAL_PP(scin), &type);
    if(!sc)
      {
        php_error(E_WARNING, "%s(): %d is not an ASPELL result index", get_active_function_name(TSRMLS_C), Z_LVAL_PP(scin));
        RETURN_FALSE;
      }
	if (aspell_check_raw(sc, Z_STRVAL_PP(word))) 
	  {
	    RETURN_TRUE;
	  }
	else
	  {
	    RETURN_FALSE;
	      }
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(aspell)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "ASpell Support", "enabled");
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
