/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
#include "readline_cli.h"

#if HAVE_LIBREADLINE || HAVE_LIBEDIT

#ifndef HAVE_RL_COMPLETION_MATCHES
#define rl_completion_matches completion_matches
#endif

#ifdef HAVE_LIBEDIT
#include <editline/readline.h>
#else
#include <readline/readline.h>
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
PHP_MSHUTDOWN_FUNCTION(readline);
PHP_RSHUTDOWN_FUNCTION(readline);
PHP_MINFO_FUNCTION(readline);

/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_readline, 0, 0, 0)
	ZEND_ARG_INFO(0, prompt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_info, 0, 0, 0)
	ZEND_ARG_INFO(0, varname)
	ZEND_ARG_INFO(0, newvalue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_add_history, 0, 0, 1)
	ZEND_ARG_INFO(0, prompt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_readline_clear_history, 0)
ZEND_END_ARG_INFO()

#ifndef HAVE_LIBEDIT
ZEND_BEGIN_ARG_INFO(arginfo_readline_list_history, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_read_history, 0, 0, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_write_history, 0, 0, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_completion_function, 0, 0, 1)
	ZEND_ARG_INFO(0, funcname)
ZEND_END_ARG_INFO()

#if HAVE_RL_CALLBACK_READ_CHAR
ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_callback_handler_install, 0, 0, 2)
	ZEND_ARG_INFO(0, prompt)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_readline_callback_read_char, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_readline_callback_handler_remove, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_readline_redisplay, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_readline_on_new_line, 0)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ module stuff */
static const zend_function_entry php_readline_functions[] = {
	PHP_FE(readline,	   		        arginfo_readline)
	PHP_FE(readline_info,  	            arginfo_readline_info)
	PHP_FE(readline_add_history, 		arginfo_readline_add_history)
	PHP_FE(readline_clear_history, 		arginfo_readline_clear_history)
#ifndef HAVE_LIBEDIT
	PHP_FE(readline_list_history, 		arginfo_readline_list_history)
#endif
	PHP_FE(readline_read_history, 		arginfo_readline_read_history)
	PHP_FE(readline_write_history, 		arginfo_readline_write_history)
	PHP_FE(readline_completion_function,arginfo_readline_completion_function)
#if HAVE_RL_CALLBACK_READ_CHAR
	PHP_FE(readline_callback_handler_install, arginfo_readline_callback_handler_install)
	PHP_FE(readline_callback_read_char,			arginfo_readline_callback_read_char)
	PHP_FE(readline_callback_handler_remove,	arginfo_readline_callback_handler_remove)
	PHP_FE(readline_redisplay, arginfo_readline_redisplay)
#endif
#if HAVE_RL_ON_NEW_LINE
	PHP_FE(readline_on_new_line, arginfo_readline_on_new_line)
#endif
	PHP_FE_END
};

zend_module_entry readline_module_entry = { 
	STANDARD_MODULE_HEADER,
	"readline", 
	php_readline_functions, 
	PHP_MINIT(readline), 
	PHP_MSHUTDOWN(readline),
	NULL,
	PHP_RSHUTDOWN(readline),
	PHP_MINFO(readline), 
	PHP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_READLINE
ZEND_GET_MODULE(readline)
#endif

PHP_MINIT_FUNCTION(readline)
{
#if HAVE_LIBREADLINE
		/* libedit don't need this call which set the tty in cooked mode */
		using_history();
#endif
    	return PHP_MINIT(cli_readline)(INIT_FUNC_ARGS_PASSTHRU);
}

PHP_MSHUTDOWN_FUNCTION(readline)
{
	return PHP_MSHUTDOWN(cli_readline)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
}

PHP_RSHUTDOWN_FUNCTION(readline)
{
	if (_readline_completion) {
		zval_dtor(_readline_completion);
		FREE_ZVAL(_readline_completion);
	}
#if HAVE_RL_CALLBACK_READ_CHAR
	if (_prepped_callback) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
		_prepped_callback = 0;
	}
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(readline)
{
	PHP_MINFO(cli_readline)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
}

/* }}} */

/* {{{ proto string readline([string prompt]) 
   Reads a line */
PHP_FUNCTION(readline)
{
	char *prompt = NULL;
	int prompt_len;
	char *result;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &prompt, &prompt_len)) {
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

#define SAFE_STRING(s) ((s)?(char*)(s):"")

/* {{{ proto mixed readline_info([string varname [, string newvalue]]) 
   Gets/sets various internal readline variables. */
PHP_FUNCTION(readline_info)
{
	char *what = NULL;
	zval **value = NULL;
	int what_len, oldval;
	char *oldstr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sZ", &what, &what_len, &value) == FAILURE) {
		return;
	}

	if (!what) {
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
		add_assoc_long(return_value,"attempted_completion_over",rl_attempted_completion_over);
	} else {
		if (!strcasecmp(what,"line_buffer")) {
			oldstr = rl_line_buffer;
			if (value) {
				/* XXX if (rl_line_buffer) free(rl_line_buffer); */
				convert_to_string_ex(value);
				rl_line_buffer = strdup(Z_STRVAL_PP(value));
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} else if (!strcasecmp(what, "point")) {
			RETVAL_LONG(rl_point);
		} else if (!strcasecmp(what, "end")) {
			RETVAL_LONG(rl_end);
#ifdef HAVE_LIBREADLINE
		} else if (!strcasecmp(what, "mark")) {
			RETVAL_LONG(rl_mark);
		} else if (!strcasecmp(what, "done")) {
			oldval = rl_done;
			if (value) {
				convert_to_long_ex(value);
				rl_done = Z_LVAL_PP(value);
			}
			RETVAL_LONG(oldval);
		} else if (!strcasecmp(what, "pending_input")) {
			oldval = rl_pending_input;
			if (value) {
				convert_to_string_ex(value);
				rl_pending_input = Z_STRVAL_PP(value)[0];
			}
			RETVAL_LONG(oldval);
		} else if (!strcasecmp(what, "prompt")) {
			RETVAL_STRING(SAFE_STRING(rl_prompt),1);
		} else if (!strcasecmp(what, "terminal_name")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_terminal_name),1);
#endif
#if HAVE_ERASE_EMPTY_LINE
		} else if (!strcasecmp(what, "erase_empty_line")) {
			oldval = rl_erase_empty_line;
			if (value) {
				convert_to_long_ex(value);
				rl_erase_empty_line = Z_LVAL_PP(value);
			}
			RETVAL_LONG(oldval);
#endif
		} else if (!strcasecmp(what,"library_version")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_library_version),1);
		} else if (!strcasecmp(what, "readline_name")) {
			oldstr = (char*)rl_readline_name;
			if (value) {
				/* XXX if (rl_readline_name) free(rl_readline_name); */
				convert_to_string_ex(value);
				rl_readline_name = strdup(Z_STRVAL_PP(value));;
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} else if (!strcasecmp(what, "attempted_completion_over")) {
			oldval = rl_attempted_completion_over;
			if (value) {
				convert_to_long_ex(value);
				rl_attempted_completion_over = Z_LVAL_PP(value);
			}
			RETVAL_LONG(oldval);
		}
	}
}

/* }}} */
/* {{{ proto bool readline_add_history(string prompt) 
   Adds a line to the history */
PHP_FUNCTION(readline_add_history)
{
	char *arg;
	int arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	add_history(arg);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto bool readline_clear_history(void) 
   Clears the history */
PHP_FUNCTION(readline_clear_history)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
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
/* {{{ proto bool readline_read_history([string filename]) 
   Reads the history */
PHP_FUNCTION(readline_read_history)
{
	char *arg = NULL;
	int arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|p", &arg, &arg_len) == FAILURE) {
		return;
	}

	if (php_check_open_basedir(arg TSRMLS_CC)) {
		RETURN_FALSE;
	}

	/* XXX from & to NYI */
	if (read_history(arg)) {
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
	char *arg = NULL;
	int arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|p", &arg, &arg_len) == FAILURE) {
		return;
	}

	if (php_check_open_basedir(arg TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (write_history(arg)) {
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
	
	if (!state) {
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
				if (!matches) {
					return NULL;
				}
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

	if (!zend_is_callable(arg, 0, &name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not callable", name);
		efree(name);
		RETURN_FALSE;
	}
	efree(name);

	if (_readline_completion) {
		zval_dtor(_readline_completion);
		FREE_ZVAL(_readline_completion);
	}

	MAKE_STD_ZVAL(_readline_completion);
	*_readline_completion = *arg;
	zval_copy_ctor(_readline_completion);

	rl_attempted_completion_function = _readline_completion_cb;
	if (rl_attempted_completion_function == NULL) {
		efree(name);
		RETURN_FALSE;
	}
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

	if (!zend_is_callable(callback, 0, &name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s is not callable", name);
		efree(name);
		RETURN_FALSE;
	}
	efree(name);

	if (_prepped_callback) {
		rl_callback_handler_remove();
		zval_dtor(_prepped_callback);
		FREE_ZVAL(_prepped_callback);
	}

	ALLOC_ZVAL(_prepped_callback);
	MAKE_COPY_ZVAL(&callback, _prepped_callback);

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
		zval_dtor(_prepped_callback);
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

#endif

#if HAVE_RL_ON_NEW_LINE
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
