/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ includes & prototypes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_readline.h"

#if HAVE_LIBREADLINE || HAVE_LIBEDIT

#ifndef HAVE_RL_COMPLETION_MATCHES
#define rl_completion_matches completion_matches
#endif

#include <readline/readline.h>
#ifndef HAVE_LIBEDIT
#include <readline/history.h>
#endif

PHP_FUNCTION(readline);
PHP_FUNCTION(readline_add_history);
PHP_FUNCTION(readline_info);
PHP_FUNCTION(readline_clear_history);
#ifndef HAVE_LIBEDIT
PHP_FUNCTION(readline_list_history);
#endif
PHP_FUNCTION(readline_read_history);
PHP_FUNCTION(readline_write_history);
PHP_FUNCTION(readline_completion_function);

#if HAVE_RL_CALLBACK_READ_CHAR
PHP_FUNCTION(readline_callback_handler_install);
PHP_FUNCTION(readline_callback_read_char);
PHP_FUNCTION(readline_callback_handler_remove);
PHP_FUNCTION(readline_redisplay);
PHP_FUNCTION(readline_on_new_line);

static zval *_prepped_callback = NULL;

#endif

static zval *_readline_completion = NULL;
static zval _readline_array;

PHP_MINIT_FUNCTION(readline);
PHP_RSHUTDOWN_FUNCTION(readline);

/* }}} */
/* {{{ module stuff */

static zend_function_entry php_readline_functions[] = {
	PHP_FE(readline,	   		        NULL)
	PHP_FE(readline_info,  	            NULL)
	PHP_FE(readline_add_history, 		NULL)
	PHP_FE(readline_clear_history, 		NULL)
#ifndef HAVE_LIBEDIT
	PHP_FE(readline_list_history, 		NULL)
#endif
	PHP_FE(readline_read_history, 		NULL)
	PHP_FE(readline_write_history, 		NULL)
	PHP_FE(readline_completion_function,NULL)
#if HAVE_RL_CALLBACK_READ_CHAR
	PHP_FE(readline_callback_handler_install, NULL)
	PHP_FE(readline_callback_read_char,			NULL)
	PHP_FE(readline_callback_handler_remove,	NULL)
	PHP_FE(readline_redisplay, NULL)
	PHP_FE(readline_on_new_line, NULL)
#endif
	{NULL, NULL, NULL}
};

zend_module_entry readline_module_entry = { 
	STANDARD_MODULE_HEADER,
	"readline", 
	php_readline_functions, 
	PHP_MINIT(readline), 
	NULL,
	NULL,
	PHP_RSHUTDOWN(readline),
	NULL, 
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_READLINE
ZEND_GET_MODULE(readline)
#endif

PHP_MINIT_FUNCTION(readline)
{
    	using_history();
    	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(readline)
{
	if (_readline_completion) 
		FREE_ZVAL(_readline_completion);
#if HAVE_RL_CALLBACK_READ_CHAR
	if (_prepped_callback) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
		_prepped_callback = 0;
	}
#endif

	return SUCCESS;
}

/* }}} */

/* {{{ proto string readline([string prompt]) 
   Reads a line */
PHP_FUNCTION(readline)
{
	char *prompt = NULL;
	int prompt_len;
	char *result;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &prompt, &prompt_len)) {
		RETURN_FALSE;
	}

	result = readline(prompt);

	if (! result) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(result,1);
		free(result);
	}
}

/* }}} */

/* {{{ proto mixed readline_info([string varname] [, string newvalue]) 
   Gets/sets various internal readline variables. */

#define SAFE_STRING(s) ((s)?(char*)(s):"")

PHP_FUNCTION(readline_info)
{
	zval **what;
	zval **value;
	int oldval;
	char *oldstr;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 2 || zend_get_parameters_ex(ac, &what, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 0) {
		array_init(return_value);
		add_assoc_string(return_value,"line_buffer",SAFE_STRING(rl_line_buffer),1);
		add_assoc_long(return_value,"point",rl_point);
		add_assoc_long(return_value,"end",rl_end);
#ifdef HAVE_LIBREADLINE
		add_assoc_long(return_value,"mark",rl_mark);
		add_assoc_long(return_value,"done",rl_done);
		add_assoc_long(return_value,"pending_input",rl_pending_input);
		add_assoc_string(return_value,"prompt",SAFE_STRING(rl_prompt),1);
		add_assoc_string(return_value,"terminal_name",(char *)SAFE_STRING(rl_terminal_name),1);
#endif
#if HAVE_ERASE_EMPTY_LINE
		add_assoc_long(return_value,"erase_empty_line",rl_erase_empty_line);
#endif
		add_assoc_string(return_value,"library_version",(char *)SAFE_STRING(rl_library_version),1);
		add_assoc_string(return_value,"readline_name",(char *)SAFE_STRING(rl_readline_name),1);
	} else {
		convert_to_string_ex(what);

		if (! strcasecmp(Z_STRVAL_PP(what),"line_buffer")) {
			oldstr = rl_line_buffer;
			if (ac == 2) {
				/* XXX if (rl_line_buffer) free(rl_line_buffer); */
				convert_to_string_ex(value);
				rl_line_buffer = strdup(Z_STRVAL_PP(value));
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"point")) {
			RETVAL_LONG(rl_point);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"end")) {
			RETVAL_LONG(rl_end);
#ifdef HAVE_LIBREADLINE
		} else if (! strcasecmp(Z_STRVAL_PP(what),"mark")) {
			RETVAL_LONG(rl_mark);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"done")) {
			oldval = rl_done;
			if (ac == 2) {
				convert_to_long_ex(value);
				rl_done = Z_LVAL_PP(value);
			}
			RETVAL_LONG(oldval);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"pending_input")) {
			oldval = rl_pending_input;
			if (ac == 2) {
				convert_to_string_ex(value);
				rl_pending_input = Z_STRVAL_PP(value)[0];
			}
			RETVAL_LONG(oldval);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"prompt")) {
			RETVAL_STRING(SAFE_STRING(rl_prompt),1);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"terminal_name")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_terminal_name),1);
#endif
#if HAVE_ERASE_EMPTY_LINE
		} else if (! strcasecmp(Z_STRVAL_PP(what),"erase_empty_line")) {
			oldval = rl_erase_empty_line;
			if (ac == 2) {
				convert_to_long_ex(value);
				rl_erase_empty_line = Z_LVAL_PP(value);
			}
			RETVAL_LONG(oldval);
#endif
		} else if (! strcasecmp(Z_STRVAL_PP(what),"library_version")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_library_version),1);
		} else if (! strcasecmp(Z_STRVAL_PP(what),"readline_name")) {
			oldstr = (char*)rl_readline_name;
			if (ac == 2) {
				/* XXX if (rl_readline_name) free(rl_readline_name); */
				convert_to_string_ex(value);
				rl_readline_name = strdup(Z_STRVAL_PP(value));;
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} 
	}
}

/* }}} */
/* {{{ proto bool readline_add_history([string prompt]) 
   Adds a line to the history */
PHP_FUNCTION(readline_add_history)
{
	zval **arg;
	int ac = ZEND_NUM_ARGS();

	if (ac != 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	add_history(Z_STRVAL_PP(arg));

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto bool readline_clear_history(void) 
   Clears the history */
PHP_FUNCTION(readline_clear_history)
{
	int ac = ZEND_NUM_ARGS();

	if (ac != 0) {
		WRONG_PARAM_COUNT;
	}

	clear_history();

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto array readline_list_history(void) 
   Lists the history */
#ifndef HAVE_LIBEDIT
PHP_FUNCTION(readline_list_history)
{
	HIST_ENTRY **history;
	int ac = ZEND_NUM_ARGS();

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
#endif
/* }}} */
/* {{{ proto bool readline_read_history([string filename] [, int from] [,int to]) 
   Reads the history */
PHP_FUNCTION(readline_read_history)
{
	zval **arg;
	char *filename = NULL;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* XXX from & to NYI */

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = Z_STRVAL_PP(arg);
	}

	if (read_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ proto bool readline_write_history([string filename]) 
   Writes the history */
PHP_FUNCTION(readline_write_history)
{
	zval **arg;
	char *filename = NULL;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = Z_STRVAL_PP(arg);
	}

	if (write_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ proto bool readline_completion_function(string funcname) 
   Readline completion function? */

static char *_readline_command_generator(const char *text, int state)
{
	HashTable  *myht = Z_ARRVAL(_readline_array);
	zval **entry;
	
	if (! state)	{
		zend_hash_internal_pointer_reset(myht);
	}
	
	while (zend_hash_get_current_data(myht, (void **)&entry) == SUCCESS) {
		zend_hash_move_forward(myht);

		convert_to_string_ex(entry);
		if (strncmp (Z_STRVAL_PP(entry), text, strlen(text)) == 0) {
			return (strdup(Z_STRVAL_PP(entry)));
		}
	}

	return NULL;
}

static zval *_readline_string_zval(const char *str)
{
	zval *ret;
	int len;
	
	MAKE_STD_ZVAL(ret);
	
	if (str) {
		len = strlen(str);
		ZVAL_STRINGL(ret, (char*)str, len, 1);
	} else {
		ZVAL_NULL(ret);
	}

	return ret;
}

static zval *_readline_long_zval(long l)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);

	Z_TYPE_P(ret) = IS_LONG;
	Z_LVAL_P(ret) = l;
	return ret;
}

static char **_readline_completion_cb(const char *text, int start, int end)
{ 
	zval *params[3];
	int i;
	char **matches = NULL;
	TSRMLS_FETCH();

	params[0]=_readline_string_zval(text);
	params[1]=_readline_long_zval(start);
	params[2]=_readline_long_zval(end);

	if (call_user_function(CG(function_table), NULL, _readline_completion, &_readline_array, 3, params TSRMLS_CC) == SUCCESS) {
		if (Z_TYPE(_readline_array) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL(_readline_array))) {
				matches = rl_completion_matches(text,_readline_command_generator);
			} else {
				matches = malloc(sizeof(char *) * 2);
				matches[0] = strdup("");
				matches[1] = '\0';
			}
		}
	}
	
	for (i = 0; i < 3; i++) {
		zval_ptr_dtor(&params[i]);
	}
	zval_dtor(&_readline_array);
	
	return matches; 
}

PHP_FUNCTION(readline_completion_function)
{
	zval *arg = NULL;
	char *name = NULL;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg)) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(arg, 0, &name)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not callable", name);
		efree(name);
		RETURN_FALSE;
	}

	if (_readline_completion)
		FREE_ZVAL(_readline_completion);

	MAKE_STD_ZVAL(_readline_completion);
	*_readline_completion = *arg;
	zval_copy_ctor(_readline_completion);

	rl_attempted_completion_function = _readline_completion_cb;

	RETURN_TRUE;
}

/* }}} */

#if HAVE_RL_CALLBACK_READ_CHAR

static void php_rl_callback_handler(char *the_line)
{
	zval *params[1];
	zval dummy;
	TSRMLS_FETCH();

	ZVAL_NULL(&dummy);

	params[0] = _readline_string_zval(the_line);

	call_user_function(CG(function_table), NULL, _prepped_callback, &dummy, 1, params TSRMLS_CC);

	zval_ptr_dtor(&params[0]);
	zval_dtor(&dummy);
}

/* {{{ proto void readline_callback_handler_install(string prompt, mixed callback)
   Initializes the readline callback interface and terminal, prints the prompt and returns immediately */
PHP_FUNCTION(readline_callback_handler_install)
{
	zval *callback;
	char *name = NULL;
	char *prompt;
	int prompt_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &prompt, &prompt_len, &callback)) {
		return;
	}

	if (!zend_is_callable(callback, 0, &name)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not callable", name);
		efree(name);
		RETURN_FALSE;
	}
	efree(name);

	if (_prepped_callback) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
	}

	MAKE_STD_ZVAL(_prepped_callback);
	*_prepped_callback = *callback;
	zval_copy_ctor(_prepped_callback);

	rl_callback_handler_install(prompt, php_rl_callback_handler);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void readline_callback_read_char()
   Informs the readline callback interface that a character is ready for input */
PHP_FUNCTION(readline_callback_read_char)
{
	if (_prepped_callback) {
		rl_callback_read_char();
	}
}
/* }}} */

/* {{{ proto bool readline_callback_handler_remove()
   Removes a previously installed callback handler and restores terminal settings */
PHP_FUNCTION(readline_callback_handler_remove)
{
	if (_prepped_callback) {
		rl_callback_handler_remove();
		FREE_ZVAL(_prepped_callback);
		_prepped_callback = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void readline_redisplay(void)
   Ask readline to redraw the display */
PHP_FUNCTION(readline_redisplay)
{
	rl_redisplay();
}
/* }}} */

/* {{{ proto void readline_on_new_line(void)
   Inform readline that the cursor has moved to a new line */
PHP_FUNCTION(readline_on_new_line)
{
	rl_on_new_line();
}
/* }}} */

#endif


#endif /* HAVE_LIBREADLINE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
