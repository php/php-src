/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
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

/* $Id: php_cli_readline.c 306939 2011-01-01 02:19:59Z felipe $ */

#include "php.h"

#if (HAVE_LIBREADLINE || HAVE_LIBEDIT) && !defined(COMPILE_DL_READLINE)

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
#include "zend_indent.h"

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

int cli_is_valid_code(char *code, int len, char **prompt TSRMLS_DC) /* {{{ */
{
	int valid_end = 1, last_valid_end;
	int brackets_count = 0;
	int brace_count = 0;
	int i;
	php_code_type code_type = body;
	char *heredoc_tag;
	int heredoc_len;

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
					case '%':
						if (!CG(asp_tags)) {
							valid_end = 0;
							break;
						}
						/* no break */
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
				||  (CG(asp_tags) && !strncmp(code+i-1, "<%", 2))
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
				*prompt = "php ( ";
			} else if (brackets_count) {
				*prompt = "php { ";
			} else {
				*prompt = "php > ";
			}
			break;
		case sstring:
		case sstring_esc:
			*prompt = "php ' ";
			break;
		case dstring:
		case dstring_esc:
			*prompt = "php \" ";
			break;
		case comment_block:
			*prompt = "/*  > ";
			break;
		case heredoc:
			*prompt = "<<< > ";
			break;
		case outside:
			*prompt = "    > ";
			break;
	}

	if (!valid_end || brackets_count) {
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

static char *cli_completion_generator_ht(const char *text, int textlen, int *state, HashTable *ht, void **pData TSRMLS_DC) /* {{{ */
{
	char *name;
	ulong number;

	if (!(*state % 2)) {
		zend_hash_internal_pointer_reset(ht);
		(*state)++;
	}
	while(zend_hash_has_more_elements(ht) == SUCCESS) {
		zend_hash_get_current_key(ht, &name, &number, 0);
		if (!textlen || !strncmp(name, text, textlen)) {
			if (pData) {
				zend_hash_get_current_data(ht, pData);
			}
			zend_hash_move_forward(ht);
			return name;
		}
		if (zend_hash_move_forward(ht) == FAILURE) {
			break;
		}
	}
	(*state)++;
	return NULL;
} /* }}} */

static char *cli_completion_generator_var(const char *text, int textlen, int *state TSRMLS_DC) /* {{{ */
{
	char *retval, *tmp;

	tmp = retval = cli_completion_generator_ht(text + 1, textlen - 1, state, EG(active_symbol_table), NULL TSRMLS_CC);
	if (retval) {
		retval = malloc(strlen(tmp) + 2);
		retval[0] = '$';
		strcpy(&retval[1], tmp);
		rl_completion_append_character = '\0';
	}
	return retval;
} /* }}} */

static char *cli_completion_generator_func(const char *text, int textlen, int *state, HashTable *ht TSRMLS_DC) /* {{{ */
{
	zend_function *func;
	char *retval = cli_completion_generator_ht(text, textlen, state, ht, (void**)&func TSRMLS_CC);
	if (retval) {
		rl_completion_append_character = '(';
		retval = strdup(func->common.function_name);
	}
	
	return retval;
} /* }}} */

static char *cli_completion_generator_class(const char *text, int textlen, int *state TSRMLS_DC) /* {{{ */
{
	zend_class_entry **pce;
	char *retval = cli_completion_generator_ht(text, textlen, state, EG(class_table), (void**)&pce TSRMLS_CC);
	if (retval) {
		rl_completion_append_character = '\0';
		retval = strdup((*pce)->name);
	}
	
	return retval;
} /* }}} */

static char *cli_completion_generator_define(const char *text, int textlen, int *state, HashTable *ht TSRMLS_DC) /* {{{ */
{
	zend_class_entry **pce;
	char *retval = cli_completion_generator_ht(text, textlen, state, ht, (void**)&pce TSRMLS_CC);
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
	int textlen = strlen(text);
	TSRMLS_FETCH();

	if (!index) {
		cli_completion_state = 0;
	}
	if (text[0] == '$') {
		retval = cli_completion_generator_var(text, textlen, &cli_completion_state TSRMLS_CC);
	} else {
		char *lc_text, *class_name, *class_name_end;
		int class_name_len;
		zend_class_entry **pce = NULL;
		
		class_name_end = strstr(text, "::");
		if (class_name_end) {
			class_name_len = class_name_end - text;
			class_name = zend_str_tolower_dup(text, class_name_len);
			class_name[class_name_len] = '\0'; /* not done automatically */
			if (zend_lookup_class(class_name, class_name_len, &pce TSRMLS_CC)==FAILURE) {
				efree(class_name);
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
				retval = cli_completion_generator_func(lc_text, textlen, &cli_completion_state, pce ? &(*pce)->function_table : EG(function_table) TSRMLS_CC);
				if (retval) {
					break;
				}
			case 2:
			case 3:
				retval = cli_completion_generator_define(text, textlen, &cli_completion_state, pce ? &(*pce)->constants_table : EG(zend_constants) TSRMLS_CC);
				if (retval || pce) {
					break;
				}
			case 4:
			case 5:
				retval = cli_completion_generator_class(lc_text, textlen, &cli_completion_state TSRMLS_CC);
				break;
			default:
				break;
		}
		efree(lc_text);
		if (class_name_end) {
			efree(class_name);
		}
		if (pce && retval) {
			int len = class_name_len + 2 + strlen(retval) + 1;
			char *tmp = malloc(len);
			
			snprintf(tmp, len, "%s::%s", (*pce)->name, retval);
			free(retval);
			retval = tmp;
		}
	}
	
	return retval;
} /* }}} */

char **cli_code_completion(const char *text, int start, int end) /* {{{ */
{
	return rl_completion_matches(text, cli_completion_generator);
}
/* }}} */

#endif /* HAVE_LIBREADLINE || HAVE_LIBEDIT */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
