/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   |         Johannes Schlueter <johannes@php.net>                        |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifndef HAVE_RL_COMPLETION_MATCHES
#define rl_completion_matches completion_matches
#endif

#include "php_globals.h"
#include "php_variables.h"
#include "zend_hash.h"
#include "zend_modules.h"

#include "SAPI.h"

#if HAVE_SETLOCALE
#include <locale.h>
#endif
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "zend_smart_str.h"

#ifdef __riscos__
#include <unixlib/local.h>
#endif

#if HAVE_LIBEDIT
#include <editline/readline.h>
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_exceptions.h"

#include "sapi/cli/cli.h"
#include "readline_cli.h"

#if defined(COMPILE_DL_READLINE) && !defined(PHP_WIN32)
#include <dlfcn.h>
#endif

#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT NULL
#endif

#define DEFAULT_PROMPT "\\b \\> "

ZEND_DECLARE_MODULE_GLOBALS(cli_readline);

static char php_last_char = '\0';
static FILE *pager_pipe = NULL;

static size_t readline_shell_write(const char *str, size_t str_length) /* {{{ */
{
	if (CLIR_G(prompt_str)) {
		smart_str_appendl(CLIR_G(prompt_str), str, str_length);
		return str_length;
	}

	if (CLIR_G(pager) && *CLIR_G(pager) && !pager_pipe) {
		pager_pipe = VCWD_POPEN(CLIR_G(pager), "w");
	}
	if (pager_pipe) {
		return fwrite(str, 1, MIN(str_length, 16384), pager_pipe);
	}

	return -1;
}
/* }}} */

static size_t readline_shell_ub_write(const char *str, size_t str_length) /* {{{ */
{
	/* We just store the last char here and then pass back to the
	   caller (sapi_cli_single_write in sapi/cli) which will actually
	   write due to -1 return code */
	php_last_char = str[str_length-1];

	return (size_t) -1;
}
/* }}} */

static void cli_readline_init_globals(zend_cli_readline_globals *rg)
{
	rg->pager = NULL;
	rg->prompt = NULL;
	rg->prompt_str = NULL;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("cli.pager", "", PHP_INI_ALL, OnUpdateString, pager, zend_cli_readline_globals, cli_readline_globals)
	STD_PHP_INI_ENTRY("cli.prompt", DEFAULT_PROMPT, PHP_INI_ALL, OnUpdateString, prompt, zend_cli_readline_globals, cli_readline_globals)
PHP_INI_END()



typedef enum {
	body,
	sstring,
	dstring,
	sstring_esc,
	dstring_esc,
	comment_line,
	comment_block,
	heredoc_start,
	heredoc,
	outside,
} php_code_type;

static zend_string *cli_get_prompt(char *block, char prompt) /* {{{ */
{
	smart_str retval = {0};
	char *prompt_spec = CLIR_G(prompt) ? CLIR_G(prompt) : DEFAULT_PROMPT;

	do {
		if (*prompt_spec == '\\') {
			switch (prompt_spec[1]) {
			case '\\':
				smart_str_appendc(&retval, '\\');
				prompt_spec++;
				break;
			case 'n':
				smart_str_appendc(&retval, '\n');
				prompt_spec++;
				break;
			case 't':
				smart_str_appendc(&retval, '\t');
				prompt_spec++;
				break;
			case 'e':
				smart_str_appendc(&retval, '\033');
				prompt_spec++;
				break;


			case 'v':
				smart_str_appends(&retval, PHP_VERSION);
				prompt_spec++;
				break;
			case 'b':
				smart_str_appends(&retval, block);
				prompt_spec++;
				break;
			case '>':
				smart_str_appendc(&retval, prompt);
				prompt_spec++;
				break;
			case '`':
				smart_str_appendc(&retval, '`');
				prompt_spec++;
				break;
			default:
				smart_str_appendc(&retval, '\\');
				break;
			}
		} else if (*prompt_spec == '`') {
			char *prompt_end = strstr(prompt_spec + 1, "`");
			char *code;

			if (prompt_end) {
				code = estrndup(prompt_spec + 1, prompt_end - prompt_spec - 1);

				CLIR_G(prompt_str) = &retval;
				zend_try {
					zend_eval_stringl(code, prompt_end - prompt_spec - 1, NULL, "php prompt code");
				} zend_end_try();
				CLIR_G(prompt_str) = NULL;
				efree(code);
				prompt_spec = prompt_end;
			}
		} else {
			smart_str_appendc(&retval, *prompt_spec);
		}
	} while (++prompt_spec && *prompt_spec);
	smart_str_0(&retval);
	return retval.s;
}
/* }}} */

static int cli_is_valid_code(char *code, size_t len, zend_string **prompt) /* {{{ */
{
	int valid_end = 1, last_valid_end;
	int brackets_count = 0;
	int brace_count = 0;
	size_t i;
	php_code_type code_type = body;
	char *heredoc_tag;
	size_t heredoc_len;

	for (i = 0; i < len; ++i) {
		switch(code_type) {
			default:
				switch(code[i]) {
					case '{':
						brackets_count++;
						valid_end = 0;
						break;
					case '}':
						if (brackets_count > 0) {
							brackets_count--;
						}
						valid_end = brackets_count ? 0 : 1;
						break;
					case '(':
						brace_count++;
						valid_end = 0;
						break;
					case ')':
						if (brace_count > 0) {
							brace_count--;
						}
						valid_end = 0;
						break;
					case ';':
						valid_end = brace_count == 0 && brackets_count == 0;
						break;
					case ' ':
					case '\r':
					case '\n':
					case '\t':
						break;
					case '\'':
						code_type = sstring;
						break;
					case '"':
						code_type = dstring;
						break;
					case '#':
						code_type = comment_line;
						break;
					case '/':
						if (code[i+1] == '/') {
							i++;
							code_type = comment_line;
							break;
						}
						if (code[i+1] == '*') {
							last_valid_end = valid_end;
							valid_end = 0;
							code_type = comment_block;
							i++;
							break;
						}
						valid_end = 0;
						break;
					case '?':
						if (code[i+1] == '>') {
							i++;
							code_type = outside;
							break;
						}
						valid_end = 0;
						break;
					case '<':
						valid_end = 0;
						if (i + 2 < len && code[i+1] == '<' && code[i+2] == '<') {
							i += 2;
							code_type = heredoc_start;
							heredoc_len = 0;
						}
						break;
					default:
						valid_end = 0;
						break;
				}
				break;
			case sstring:
				if (code[i] == '\\') {
					code_type = sstring_esc;
				} else {
					if (code[i] == '\'') {
						code_type = body;
					}
				}
				break;
			case sstring_esc:
				code_type = sstring;
				break;
			case dstring:
				if (code[i] == '\\') {
					code_type = dstring_esc;
				} else {
					if (code[i] == '"') {
						code_type = body;
					}
				}
				break;
			case dstring_esc:
				code_type = dstring;
				break;
			case comment_line:
				if (code[i] == '\n') {
					code_type = body;
				}
				break;
			case comment_block:
				if (code[i-1] == '*' && code[i] == '/') {
					code_type = body;
					valid_end = last_valid_end;
				}
				break;
			case heredoc_start:
				switch(code[i]) {
					case ' ':
					case '\t':
					case '\'':
						break;
					case '\r':
					case '\n':
						code_type = heredoc;
						break;
					default:
						if (!heredoc_len) {
							heredoc_tag = code+i;
						}
						heredoc_len++;
						break;
				}
				break;
			case heredoc:
				if (code[i - (heredoc_len + 1)] == '\n' && !strncmp(code + i - heredoc_len, heredoc_tag, heredoc_len) && code[i] == '\n') {
					code_type = body;
				} else if (code[i - (heredoc_len + 2)] == '\n' && !strncmp(code + i - heredoc_len - 1, heredoc_tag, heredoc_len) && code[i-1] == ';' && code[i] == '\n') {
					code_type = body;
					valid_end = 1;
				}
				break;
			case outside:
				if ((CG(short_tags) && !strncmp(code+i-1, "<?", 2))
				||  (i > 3 && !strncmp(code+i-4, "<?php", 5))
				) {
					code_type = body;
				}
				break;
		}
	}

	switch (code_type) {
		default:
			if (brace_count) {
				*prompt = cli_get_prompt("php", '(');
			} else if (brackets_count) {
				*prompt = cli_get_prompt("php", '{');
			} else {
				*prompt = cli_get_prompt("php", '>');
			}
			break;
		case sstring:
		case sstring_esc:
			*prompt = cli_get_prompt("php", '\'');
			break;
		case dstring:
		case dstring_esc:
			*prompt = cli_get_prompt("php", '"');
			break;
		case comment_block:
			*prompt = cli_get_prompt("/* ", '>');
			break;
		case heredoc:
			*prompt = cli_get_prompt("<<<", '>');
			break;
		case outside:
			*prompt = cli_get_prompt("   ", '>');
			break;
	}

	if (!valid_end || brackets_count) {
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

static char *cli_completion_generator_ht(const char *text, size_t textlen, int *state, HashTable *ht, void **pData) /* {{{ */
{
	zend_string *name;
	zend_ulong number;

	if (!(*state % 2)) {
		zend_hash_internal_pointer_reset(ht);
		(*state)++;
	}
	while(zend_hash_has_more_elements(ht) == SUCCESS) {
		zend_hash_get_current_key(ht, &name, &number);
		if (!textlen || !strncmp(ZSTR_VAL(name), text, textlen)) {
			if (pData) {
				*pData = zend_hash_get_current_data_ptr(ht);
			}
			zend_hash_move_forward(ht);
			return ZSTR_VAL(name);
		}
		if (zend_hash_move_forward(ht) == FAILURE) {
			break;
		}
	}
	(*state)++;
	return NULL;
} /* }}} */

static char *cli_completion_generator_var(const char *text, size_t textlen, int *state) /* {{{ */
{
	char *retval, *tmp;
	zend_array *symbol_table = &EG(symbol_table);

	tmp = retval = cli_completion_generator_ht(text + 1, textlen - 1, state, symbol_table, NULL);
	if (retval) {
		retval = malloc(strlen(tmp) + 2);
		retval[0] = '$';
		strcpy(&retval[1], tmp);
		rl_completion_append_character = '\0';
	}
	return retval;
} /* }}} */

static char *cli_completion_generator_ini(const char *text, size_t textlen, int *state) /* {{{ */
{
	char *retval, *tmp;

	tmp = retval = cli_completion_generator_ht(text + 1, textlen - 1, state, EG(ini_directives), NULL);
	if (retval) {
		retval = malloc(strlen(tmp) + 2);
		retval[0] = '#';
		strcpy(&retval[1], tmp);
		rl_completion_append_character = '=';
	}
	return retval;
} /* }}} */

static char *cli_completion_generator_func(const char *text, size_t textlen, int *state, HashTable *ht) /* {{{ */
{
	zend_function *func;
	char *retval = cli_completion_generator_ht(text, textlen, state, ht, (void**)&func);
	if (retval) {
		rl_completion_append_character = '(';
		retval = strdup(ZSTR_VAL(func->common.function_name));
	}

	return retval;
} /* }}} */

static char *cli_completion_generator_class(const char *text, size_t textlen, int *state) /* {{{ */
{
	zend_class_entry *ce;
	char *retval = cli_completion_generator_ht(text, textlen, state, EG(class_table), (void**)&ce);
	if (retval) {
		rl_completion_append_character = '\0';
		retval = strdup(ZSTR_VAL(ce->name));
	}

	return retval;
} /* }}} */

static char *cli_completion_generator_define(const char *text, size_t textlen, int *state, HashTable *ht) /* {{{ */
{
	zend_class_entry **pce;
	char *retval = cli_completion_generator_ht(text, textlen, state, ht, (void**)&pce);
	if (retval) {
		rl_completion_append_character = '\0';
		retval = strdup(retval);
	}

	return retval;
} /* }}} */

static int cli_completion_state;

static char *cli_completion_generator(const char *text, int index) /* {{{ */
{
/*
TODO:
- constants
- maybe array keys
- language constructs and other things outside a hashtable (echo, try, function, class, ...)
- object/class members

- future: respect scope ("php > function foo() { $[tab]" should only expand to local variables...)
*/
	char *retval = NULL;
	size_t textlen = strlen(text);

	if (!index) {
		cli_completion_state = 0;
	}
	if (text[0] == '$') {
		retval = cli_completion_generator_var(text, textlen, &cli_completion_state);
	} else if (text[0] == '#') {
		retval = cli_completion_generator_ini(text, textlen, &cli_completion_state);
	} else {
		char *lc_text, *class_name_end;
		size_t class_name_len;
		zend_string *class_name;
		zend_class_entry *ce = NULL;

		class_name_end = strstr(text, "::");
		if (class_name_end) {
			class_name_len = class_name_end - text;
			class_name = zend_string_alloc(class_name_len, 0);
			zend_str_tolower_copy(ZSTR_VAL(class_name), text, class_name_len);
			if ((ce = zend_lookup_class(class_name)) == NULL) {
				zend_string_release_ex(class_name, 0);
				return NULL;
			}
			lc_text = zend_str_tolower_dup(class_name_end + 2, textlen - 2 - class_name_len);
			textlen -= (class_name_len + 2);
		} else {
			lc_text = zend_str_tolower_dup(text, textlen);
		}

		switch (cli_completion_state) {
			case 0:
			case 1:
				retval = cli_completion_generator_func(lc_text, textlen, &cli_completion_state, ce ? &ce->function_table : EG(function_table));
				if (retval) {
					break;
				}
			case 2:
			case 3:
				retval = cli_completion_generator_define(text, textlen, &cli_completion_state, ce ? &ce->constants_table : EG(zend_constants));
				if (retval || ce) {
					break;
				}
			case 4:
			case 5:
				retval = cli_completion_generator_class(lc_text, textlen, &cli_completion_state);
				break;
			default:
				break;
		}
		efree(lc_text);
		if (class_name_end) {
			zend_string_release_ex(class_name, 0);
		}
		if (ce && retval) {
			size_t len = class_name_len + 2 + strlen(retval) + 1;
			char *tmp = malloc(len);

			snprintf(tmp, len, "%s::%s", ZSTR_VAL(ce->name), retval);
			free(retval);
			retval = tmp;
		}
	}

	return retval;
} /* }}} */

static char **cli_code_completion(const char *text, int start, int end) /* {{{ */
{
	return rl_completion_matches(text, cli_completion_generator);
}
/* }}} */

static int readline_shell_run(void) /* {{{ */
{
	char *line;
	size_t size = 4096, pos = 0, len;
	char *code = emalloc(size);
	zend_string *prompt = cli_get_prompt("php", '>');
	char *history_file;
	int history_lines_to_write = 0;

	if (PG(auto_prepend_file) && PG(auto_prepend_file)[0]) {
		zend_file_handle *prepend_file_p;
		zend_file_handle prepend_file;

		memset(&prepend_file, 0, sizeof(prepend_file));
		prepend_file.filename = PG(auto_prepend_file);
		prepend_file.opened_path = NULL;
		prepend_file.free_filename = 0;
		prepend_file.type = ZEND_HANDLE_FILENAME;
		prepend_file_p = &prepend_file;

		zend_execute_scripts(ZEND_REQUIRE, NULL, 1, prepend_file_p);
	}

#ifndef PHP_WIN32
	history_file = tilde_expand("~/.php_history");
#else
	spprintf(&history_file, MAX_PATH, "%s/.php_history", getenv("USERPROFILE"));
#endif
	rl_attempted_completion_function = cli_code_completion;
#ifndef PHP_WIN32
	rl_special_prefixes = "$";
#endif
	read_history(history_file);

	EG(exit_status) = 0;
	while ((line = readline(ZSTR_VAL(prompt))) != NULL) {
		if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
			free(line);
			break;
		}

		if (!pos && !*line) {
			free(line);
			continue;
		}

		len = strlen(line);

		if (line[0] == '#') {
			char *param = strstr(&line[1], "=");
			if (param) {
				zend_string *cmd;
				param++;
				cmd = zend_string_init(&line[1], param - &line[1] - 1, 0);

				zend_alter_ini_entry_chars_ex(cmd, param, strlen(param), PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
				zend_string_release_ex(cmd, 0);
				add_history(line);

				zend_string_release_ex(prompt, 0);
				/* TODO: This might be wrong! */
				prompt = cli_get_prompt("php", '>');
				continue;
			}
		}

		if (pos + len + 2 > size) {
			size = pos + len + 2;
			code = erealloc(code, size);
		}
		memcpy(&code[pos], line, len);
		pos += len;
		code[pos] = '\n';
		code[++pos] = '\0';

		if (*line) {
			add_history(line);
			history_lines_to_write += 1;
		}

		free(line);
		zend_string_release_ex(prompt, 0);

		if (!cli_is_valid_code(code, pos, &prompt)) {
			continue;
		}

		if (history_lines_to_write) {
#if HAVE_LIBEDIT
			write_history(history_file);
#else
			append_history(history_lines_to_write, history_file);
#endif
			history_lines_to_write = 0;
		}

		zend_try {
			zend_eval_stringl(code, pos, NULL, "php shell code");
		} zend_end_try();

		pos = 0;

		if (!pager_pipe && php_last_char != '\0' && php_last_char != '\n') {
			php_write("\n", 1);
		}

		if (EG(exception)) {
			zend_exception_error(EG(exception), E_WARNING);
		}

		if (pager_pipe) {
			fclose(pager_pipe);
			pager_pipe = NULL;
		}

		php_last_char = '\0';
	}
#ifdef PHP_WIN32
	efree(history_file);
#else
	free(history_file);
#endif
	efree(code);
	zend_string_release_ex(prompt, 0);
	return EG(exit_status);
}
/* }}} */

#ifdef PHP_WIN32
typedef cli_shell_callbacks_t *(__cdecl *get_cli_shell_callbacks)(void);
#define GET_SHELL_CB(cb) \
	do { \
		get_cli_shell_callbacks get_callbacks; \
		HMODULE hMod = GetModuleHandle("php.exe"); \
		(cb) = NULL; \
		if (strlen(sapi_module.name) >= 3 && 0 == strncmp("cli", sapi_module.name, 3)) { \
			get_callbacks = (get_cli_shell_callbacks)GetProcAddress(hMod, "php_cli_get_shell_callbacks"); \
			if (get_callbacks) { \
				(cb) = get_callbacks(); \
			} \
		} \
	} while(0)

#else
/*
#ifdef COMPILE_DL_READLINE
This dlsym() is always used as even the CGI SAPI is linked against "CLI"-only
extensions. If that is being changed dlsym() should only be used when building
this extension sharedto offer compatibility.
*/
#define GET_SHELL_CB(cb) \
	do { \
		(cb) = NULL; \
		cli_shell_callbacks_t *(*get_callbacks)(); \
		get_callbacks = dlsym(RTLD_DEFAULT, "php_cli_get_shell_callbacks"); \
		if (get_callbacks) { \
			(cb) = get_callbacks(); \
		} \
	} while(0)
/*#else
#define GET_SHELL_CB(cb) (cb) = php_cli_get_shell_callbacks()
#endif*/
#endif

PHP_MINIT_FUNCTION(cli_readline)
{
	cli_shell_callbacks_t *cb;

	ZEND_INIT_MODULE_GLOBALS(cli_readline, cli_readline_init_globals, NULL);
	REGISTER_INI_ENTRIES();

#if HAVE_LIBEDIT
	REGISTER_STRING_CONSTANT("READLINE_LIB", "libedit", CONST_CS|CONST_PERSISTENT);
#else
	REGISTER_STRING_CONSTANT("READLINE_LIB", "readline", CONST_CS|CONST_PERSISTENT);
#endif

	GET_SHELL_CB(cb);
	if (cb) {
		cb->cli_shell_write = readline_shell_write;
		cb->cli_shell_ub_write = readline_shell_ub_write;
		cb->cli_shell_run = readline_shell_run;
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cli_readline)
{
	cli_shell_callbacks_t *cb;

	UNREGISTER_INI_ENTRIES();

	GET_SHELL_CB(cb);
	if (cb) {
		cb->cli_shell_write = NULL;
		cb->cli_shell_ub_write = NULL;
		cb->cli_shell_run = NULL;
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(cli_readline)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Readline Support", "enabled");
#ifdef PHP_WIN32
	php_info_print_table_row(2, "Readline library", "WinEditLine");
#else
	php_info_print_table_row(2, "Readline library", (rl_library_version ? rl_library_version : "Unknown"));
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
