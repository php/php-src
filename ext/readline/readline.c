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
   | Authors: Thies C. Arntzen (thies@digicol.de)                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ includes & prototypes */

#include "php.h"
#include "php_readline.h"

#if HAVE_LIBREADLINE

#include <readline/readline.h>
#include <readline/history.h>

PHP_FUNCTION(readline);
PHP_FUNCTION(readline_version);

PHP_FUNCTION(readline_add_history);
PHP_FUNCTION(readline_list_history);
PHP_FUNCTION(readline_read_history);
PHP_FUNCTION(readline_write_history);

PHP_MINIT_FUNCTION(readline);

/* }}} */
/* {{{ module stuff */

static zend_function_entry php_readline_functions[] = {
	PHP_FE(readline,	   		        NULL)
	PHP_FE(readline_version,   	        NULL)
	PHP_FE(readline_add_history, 		NULL)
	PHP_FE(readline_list_history, 		NULL)
	PHP_FE(readline_read_history, 		NULL)
	PHP_FE(readline_write_history, 		NULL)
	{NULL, NULL, NULL}
};

zend_module_entry readline_module_entry = { 
	"PHP-Readline", 
	php_readline_functions, 
	PHP_MINIT(readline), 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	STANDARD_MODULE_PROPERTIES
};

PHP_MINIT_FUNCTION(readline)
{
	return SUCCESS;
}

/* }}} */
/* {{{ proto string readline([string prompt]) */

PHP_FUNCTION(readline)
{
	char *result;
	pval **arg;
	int ac = ARG_COUNT(ht);

	if (ac < 0 || ac > 1 || getParametersEx(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	result = readline((*arg)->value.str.val);

	if (! result) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(result,1);
	}
}

/* }}} */
/* {{{ proto string readline_version() */

PHP_FUNCTION(readline_version)
{
	int ac = ARG_COUNT(ht);

	if (ac) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING(rl_library_version?rl_library_version:"unknown",1);
}

/* }}} */
/* {{{ proto void readline_add_history([string prompt]) */

PHP_FUNCTION(readline_add_history)
{
	pval **arg;
	int ac = ARG_COUNT(ht);

	if (ac < 1 || ac > 1 || getParametersEx(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	add_history((*arg)->value.str.val);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto array readline_list_history() */

PHP_FUNCTION(readline_list_history)
{
	HIST_ENTRY **history;
	int ac = ARG_COUNT(ht);

	if (ac) {
		WRONG_PARAM_COUNT;
	}

	history = history_list();
	
	array_init(return_value);

	if (history) {
		int i;
		for (i = 0; history[i]; i++) {
			add_next_index_string(return_value,history[i]->line,1);
		}
	}
}

/* }}} */
/* {{{ proto int readline_read_history([string filename][,int from][,int to]) */

PHP_FUNCTION(readline_read_history)
{
	pval **arg;
	char *filename = NULL;
	int ac = ARG_COUNT(ht);

	if (ac < 0 || ac > 1 || getParametersEx(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* XXX from & to NYI */

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = (*arg)->value.str.val;
	}

	if (read_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ proto int readline_write_history([string filename]) */

PHP_FUNCTION(readline_write_history)
{
	pval **arg;
	char *filename = NULL;
	int ac = ARG_COUNT(ht);

	if (ac < 0 || ac > 1 || getParametersEx(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = (*arg)->value.str.val;
	}

	if (write_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */

#endif /* HAVE_LIBREADLINE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
