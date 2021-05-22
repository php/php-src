/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

/* {{{ includes & prototypes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_readline.h"
#include "readline_cli.h"
#include "readline_arginfo.h"

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

#if HAVE_RL_CALLBACK_READ_CHAR

static zval _prepped_callback;

#endif

static zval _readline_completion;
static zval _readline_array;

PHP_MINIT_FUNCTION(readline);
PHP_MSHUTDOWN_FUNCTION(readline);
PHP_RSHUTDOWN_FUNCTION(readline);
PHP_MINFO_FUNCTION(readline);

/* }}} */

/* {{{ module stuff */
zend_module_entry readline_module_entry = {
	STANDARD_MODULE_HEADER,
	"readline",
	ext_functions,
	PHP_MINIT(readline),
	PHP_MSHUTDOWN(readline),
	NULL,
	PHP_RSHUTDOWN(readline),
	PHP_MINFO(readline),
	PHP_READLINE_VERSION,
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
	ZVAL_UNDEF(&_readline_completion);
#if HAVE_RL_CALLBACK_READ_CHAR
	ZVAL_UNDEF(&_prepped_callback);
#endif
   	return PHP_MINIT(cli_readline)(INIT_FUNC_ARGS_PASSTHRU);
}

PHP_MSHUTDOWN_FUNCTION(readline)
{
	return PHP_MSHUTDOWN(cli_readline)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
}

PHP_RSHUTDOWN_FUNCTION(readline)
{
	zval_ptr_dtor(&_readline_completion);
	ZVAL_UNDEF(&_readline_completion);
#if HAVE_RL_CALLBACK_READ_CHAR
	if (Z_TYPE(_prepped_callback) != IS_UNDEF) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
		ZVAL_UNDEF(&_prepped_callback);
	}
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(readline)
{
	PHP_MINFO(cli_readline)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
}

/* }}} */

/* {{{ Reads a line */
PHP_FUNCTION(readline)
{
	char *prompt = NULL;
	size_t prompt_len;
	char *result;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "|s!", &prompt, &prompt_len)) {
		RETURN_THROWS();
	}

	result = readline(prompt);

	if (! result) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(result);
		free(result);
	}
}

/* }}} */

#define SAFE_STRING(s) ((s)?(char*)(s):"")

/* {{{ Gets/sets various internal readline variables. */
PHP_FUNCTION(readline_info)
{
	char *what = NULL;
	zval *value = NULL;
	size_t what_len, oldval;
	char *oldstr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!z!", &what, &what_len, &value) == FAILURE) {
		RETURN_THROWS();
	}

	if (!what) {
		array_init(return_value);
		add_assoc_string(return_value,"line_buffer",SAFE_STRING(rl_line_buffer));
		add_assoc_long(return_value,"point",rl_point);
#ifndef PHP_WIN32
		add_assoc_long(return_value,"end",rl_end);
#endif
#ifdef HAVE_LIBREADLINE
		add_assoc_long(return_value,"mark",rl_mark);
		add_assoc_long(return_value,"done",rl_done);
		add_assoc_long(return_value,"pending_input",rl_pending_input);
		add_assoc_string(return_value,"prompt",SAFE_STRING(rl_prompt));
		add_assoc_string(return_value,"terminal_name",(char *)SAFE_STRING(rl_terminal_name));
		add_assoc_str(return_value, "completion_append_character",
			rl_completion_append_character == 0
				? ZSTR_EMPTY_ALLOC()
				: ZSTR_CHAR(rl_completion_append_character));
		add_assoc_bool(return_value,"completion_suppress_append",rl_completion_suppress_append);
#endif
#if HAVE_ERASE_EMPTY_LINE
		add_assoc_long(return_value,"erase_empty_line",rl_erase_empty_line);
#endif
#ifndef PHP_WIN32
		add_assoc_string(return_value,"library_version",(char *)SAFE_STRING(rl_library_version));
#endif
		add_assoc_string(return_value,"readline_name",(char *)SAFE_STRING(rl_readline_name));
		add_assoc_long(return_value,"attempted_completion_over",rl_attempted_completion_over);
	} else {
		if (!strcasecmp(what,"line_buffer")) {
			oldstr = rl_line_buffer;
			if (value) {
				/* XXX if (rl_line_buffer) free(rl_line_buffer); */
				if (!try_convert_to_string(value)) {
					RETURN_THROWS();
				}
				rl_line_buffer = strdup(Z_STRVAL_P(value));
			}
			RETVAL_STRING(SAFE_STRING(oldstr));
		} else if (!strcasecmp(what, "point")) {
			RETVAL_LONG(rl_point);
#ifndef PHP_WIN32
		} else if (!strcasecmp(what, "end")) {
			RETVAL_LONG(rl_end);
#endif
#ifdef HAVE_LIBREADLINE
		} else if (!strcasecmp(what, "mark")) {
			RETVAL_LONG(rl_mark);
		} else if (!strcasecmp(what, "done")) {
			oldval = rl_done;
			if (value) {
				rl_done = zval_get_long(value);
			}
			RETVAL_LONG(oldval);
		} else if (!strcasecmp(what, "pending_input")) {
			oldval = rl_pending_input;
			if (value) {
				if (!try_convert_to_string(value)) {
					RETURN_THROWS();
				}
				rl_pending_input = Z_STRVAL_P(value)[0];
			}
			RETVAL_LONG(oldval);
		} else if (!strcasecmp(what, "prompt")) {
			RETVAL_STRING(SAFE_STRING(rl_prompt));
		} else if (!strcasecmp(what, "terminal_name")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_terminal_name));
		} else if (!strcasecmp(what, "completion_suppress_append")) {
			oldval = rl_completion_suppress_append;
			if (value) {
				rl_completion_suppress_append = zend_is_true(value);
			}
			RETVAL_BOOL(oldval);
		} else if (!strcasecmp(what, "completion_append_character")) {
			oldval = rl_completion_append_character;
			if (value) {
				if (!try_convert_to_string(value)) {
					RETURN_THROWS();
				}
				rl_completion_append_character = (int)Z_STRVAL_P(value)[0];
			}
			RETVAL_INTERNED_STR(
				oldval == 0 ? ZSTR_EMPTY_ALLOC() : ZSTR_CHAR(oldval));
#endif
#if HAVE_ERASE_EMPTY_LINE
		} else if (!strcasecmp(what, "erase_empty_line")) {
			oldval = rl_erase_empty_line;
			if (value) {
				rl_erase_empty_line = zval_get_long(value);
			}
			RETVAL_LONG(oldval);
#endif
#ifndef PHP_WIN32
		} else if (!strcasecmp(what,"library_version")) {
			RETVAL_STRING((char *)SAFE_STRING(rl_library_version));
#endif
		} else if (!strcasecmp(what, "readline_name")) {
			oldstr = (char*)rl_readline_name;
			if (value) {
				/* XXX if (rl_readline_name) free(rl_readline_name); */
				if (!try_convert_to_string(value)) {
					RETURN_THROWS();
				}
				rl_readline_name = strdup(Z_STRVAL_P(value));
			}
			RETVAL_STRING(SAFE_STRING(oldstr));
		} else if (!strcasecmp(what, "attempted_completion_over")) {
			oldval = rl_attempted_completion_over;
			if (value) {
				rl_attempted_completion_over = zval_get_long(value);
			}
			RETVAL_LONG(oldval);
		}
	}
}

/* }}} */
/* {{{ Adds a line to the history */
PHP_FUNCTION(readline_add_history)
{
	char *arg;
	size_t arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	add_history(arg);

	RETURN_TRUE;
}

/* }}} */
/* {{{ Clears the history */
PHP_FUNCTION(readline_clear_history)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

#if HAVE_LIBEDIT
	/* clear_history is the only function where rl_initialize
	   is not call to ensure correct allocation */
	using_history();
#endif
	clear_history();

	RETURN_TRUE;
}

/* }}} */

#ifdef HAVE_HISTORY_LIST
/* {{{ Lists the history */
PHP_FUNCTION(readline_list_history)
{
	HIST_ENTRY **history;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

#if defined(HAVE_LIBEDIT) && defined(PHP_WIN32) /* Winedit on Windows */
	history = history_list();

	if (history) {
		int i, n = history_length();
		for (i = 0; i < n; i++) {
				add_next_index_string(return_value, history[i]->line);
		}
	}

#elif defined(HAVE_LIBEDIT) /* libedit */
    {
		HISTORY_STATE *hs;
		int i;

		using_history();
		hs = history_get_history_state();
		if (hs && hs->length) {
			history = history_list();
			if (history) {
				for (i = 0; i < hs->length; i++) {
					add_next_index_string(return_value, history[i]->line);
				}
			}
		}
		free(hs);
    }

#else /* readline */
	history = history_list();

	if (history) {
		int i;
		for (i = 0; history[i]; i++) {
			add_next_index_string(return_value, history[i]->line);
		}
	}
#endif
}
/* }}} */
#endif

/* {{{ Reads the history */
PHP_FUNCTION(readline_read_history)
{
	char *arg = NULL;
	size_t arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|p!", &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (arg && php_check_open_basedir(arg)) {
		RETURN_FALSE;
	}

	/* XXX from & to NYI */
	if (read_history(arg)) {
		/* If filename is NULL, then read from `~/.history' */
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ Writes the history */
PHP_FUNCTION(readline_write_history)
{
	char *arg = NULL;
	size_t arg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|p!", &arg, &arg_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (arg && php_check_open_basedir(arg)) {
		RETURN_FALSE;
	}

	if (write_history(arg)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ Readline completion function? */

static char *_readline_command_generator(const char *text, int state)
{
	HashTable  *myht = Z_ARRVAL(_readline_array);
	zval *entry;

	if (!state) {
		zend_hash_internal_pointer_reset(myht);
	}

	while ((entry = zend_hash_get_current_data(myht)) != NULL) {
		zend_hash_move_forward(myht);

		convert_to_string_ex(entry);
		if (strncmp (Z_STRVAL_P(entry), text, strlen(text)) == 0) {
			return (strdup(Z_STRVAL_P(entry)));
		}
	}

	return NULL;
}

static void _readline_string_zval(zval *ret, const char *str)
{
	if (str) {
		ZVAL_STRING(ret, (char*)str);
	} else {
		ZVAL_NULL(ret);
	}
}

static void _readline_long_zval(zval *ret, long l)
{
	ZVAL_LONG(ret, l);
}

char **php_readline_completion_cb(const char *text, int start, int end)
{
	zval params[3];
	char **matches = NULL;

	_readline_string_zval(&params[0], text);
	_readline_long_zval(&params[1], start);
	_readline_long_zval(&params[2], end);

	if (call_user_function(NULL, NULL, &_readline_completion, &_readline_array, 3, params) == SUCCESS) {
		if (Z_TYPE(_readline_array) == IS_ARRAY) {
			SEPARATE_ARRAY(&_readline_array);
			if (zend_hash_num_elements(Z_ARRVAL(_readline_array))) {
				matches = rl_completion_matches(text,_readline_command_generator);
			} else {
				/* libedit will read matches[2] */
				matches = calloc(sizeof(char *), 3);
				if (!matches) {
					return NULL;
				}
				matches[0] = strdup("");
			}
		}
	}

	zval_ptr_dtor(&params[0]);
	zval_ptr_dtor(&_readline_array);

	return matches;
}

PHP_FUNCTION(readline_completion_function)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "f", &fci, &fcc)) {
		RETURN_THROWS();
	}

	zval_ptr_dtor(&_readline_completion);
	ZVAL_COPY(&_readline_completion, &fci.function_name);

	/* NOTE: The rl_attempted_completion_function variable (and others) are part of the readline library, not php */
	rl_attempted_completion_function = php_readline_completion_cb;
	if (rl_attempted_completion_function == NULL) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

#if HAVE_RL_CALLBACK_READ_CHAR

static void php_rl_callback_handler(char *the_line)
{
	zval params[1];
	zval dummy;

	ZVAL_NULL(&dummy);

	_readline_string_zval(&params[0], the_line);

	call_user_function(NULL, NULL, &_prepped_callback, &dummy, 1, params);

	zval_ptr_dtor(&params[0]);
	zval_ptr_dtor(&dummy);
}

/* {{{ Initializes the readline callback interface and terminal, prints the prompt and returns immediately */
PHP_FUNCTION(readline_callback_handler_install)
{
	char *prompt;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	size_t prompt_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "sf", &prompt, &prompt_len, &fci, &fcc)) {
		RETURN_THROWS();
	}

	if (Z_TYPE(_prepped_callback) != IS_UNDEF) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
	}

	ZVAL_COPY(&_prepped_callback, &fci.function_name);

	rl_callback_handler_install(prompt, php_rl_callback_handler);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Informs the readline callback interface that a character is ready for input */
PHP_FUNCTION(readline_callback_read_char)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_TYPE(_prepped_callback) != IS_UNDEF) {
		rl_callback_read_char();
	}
}
/* }}} */

/* {{{ Removes a previously installed callback handler and restores terminal settings */
PHP_FUNCTION(readline_callback_handler_remove)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_TYPE(_prepped_callback) != IS_UNDEF) {
		rl_callback_handler_remove();
		zval_ptr_dtor(&_prepped_callback);
		ZVAL_UNDEF(&_prepped_callback);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Ask readline to redraw the display */
PHP_FUNCTION(readline_redisplay)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

#if HAVE_LIBEDIT
	/* seems libedit doesn't take care of rl_initialize in rl_redisplay
	 * see bug #72538 */
	using_history();
#endif
	rl_redisplay();
}
/* }}} */

#endif

#if HAVE_RL_ON_NEW_LINE
/* {{{ Inform readline that the cursor has moved to a new line */
PHP_FUNCTION(readline_on_new_line)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	rl_on_new_line();
}
/* }}} */

#endif


#endif /* HAVE_LIBREADLINE */
