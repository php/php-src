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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

const char *phpdbg_get_param_type(const phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	switch (param->type) {
		case EMPTY_PARAM:
			return "empty";
		case ADDR_PARAM:
			return "address";
		case NUMERIC_PARAM:
			return "numeric";
		case METHOD_PARAM:
			return "method";
		case FILE_PARAM:
			return "file";
		case STR_PARAM:
			return "string";
		default: /* this is bad */
			return "unknown";
    }
}

phpdbg_param_type phpdbg_parse_param(const char *str, size_t len, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	char *class_name, *func_name;

	if (len == 0) {
		param->type = EMPTY_PARAM;
		goto parsed;
	}

	if (phpdbg_is_addr(str)) {

		param->addr = strtoul(str, 0, 16);
		param->type = ADDR_PARAM;
		goto parsed;

	} else if (phpdbg_is_numeric(str)) {

		param->num = strtol(str, NULL, 0);
		param->type = NUMERIC_PARAM;
        goto parsed;

	} else if (phpdbg_is_class_method(str, len+1, &class_name, &func_name)) {

		param->method.class = class_name;
		param->method.name = func_name;
		param->type = METHOD_PARAM;
		goto parsed;

	} else {
		const char *line_pos = strchr(str, ':');

		if (line_pos && phpdbg_is_numeric(line_pos+1)) {
			char path[MAXPATHLEN];

			memcpy(path, str, line_pos - str);
			path[line_pos - str] = 0;

			param->file.name = phpdbg_resolve_path(path TSRMLS_CC);
			param->file.line = strtol(line_pos+1, NULL, 0);
			param->type = FILE_PARAM;
			goto parsed;
		}
	}

	param->str = estrndup(str, len);
	param->len = len;
	param->type = STR_PARAM;

parsed:
    phpdbg_debug("phpdbg_parse_param(\"%s\", %lu): %s",
		str, len, phpdbg_get_param_type(param TSRMLS_CC));
	return param->type;
} /* }}} */

void phpdbg_clear_param(phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	if (param) {
		switch (param->type) {
			case FILE_PARAM:
				efree(param->file.name);
				break;
			case METHOD_PARAM:
				efree(param->method.class);
				efree(param->method.name);
				break;
			case STR_PARAM:
				efree(param->str);
				break;
			default:
				break;
		}
	}

} /* }}} */

static inline phpdbg_input_t** phpdbg_read_argv(char *buffer, int *argc TSRMLS_DC) /* {{{ */
{
	char *p;
	char b[PHPDBG_MAX_CMD];
	int l=0;
	enum states {
		IN_BETWEEN,
		IN_WORD,
		IN_STRING
	} state = IN_BETWEEN;
	phpdbg_input_t **argv = NULL;

	argv = (phpdbg_input_t**) emalloc(sizeof(phpdbg_input_t**));
	(*argc) = 0;

#define RESET_STATE() do {\
	phpdbg_input_t *arg = emalloc(sizeof(phpdbg_input_t));\
    if (arg) {\
    	b[l]=0;\
    	arg->length = l;\
    	arg->string = estrndup(b, arg->length);\
    	arg->argv=NULL;\
    	arg->argc=0;\
    	argv = (phpdbg_input_t**) erealloc(argv, sizeof(phpdbg_input_t*) * ((*argc)+1));\
    	argv[(*argc)++] = arg;\
    	l=0;\
    }\
    state = IN_BETWEEN;\
} while(0)

	for (p = buffer; *p != '\0'; p++) {
		int c = (unsigned char) *p;
		switch (state) {
			case IN_BETWEEN:
				if (isspace(c)) {
				    continue;
				}
				if (c == '"') {
				    state = IN_STRING;
				    continue;
				}
				state = IN_WORD;
				b[l++]=c;
				continue;

			case IN_STRING:
				if ((c == '"')) {
					if (buffer[(p - buffer)-1] == '\\') {
						b[l-1]=c;
						continue;
					}
				    RESET_STATE();
				} else {
					b[l++]=c;
				}
				continue;

			case IN_WORD:
				if (isspace(c)) {
				    RESET_STATE();
				} else {
					b[l++]=c;
				}
				continue;
		}
	}

	switch (state) {
		case IN_WORD: {
			RESET_STATE();
		} break;

		case IN_STRING:
			phpdbg_error(
				"Malformed command line (unclosed quote) @ %d: %s!",
				(p - buffer)-1, &buffer[(p - buffer)-1]);
		break;
	}

	if ((*argc) == 0) {
		/* not needed */
		efree(argv);

		/* to be sure */
		return NULL;
	}

	return argv;
} /* }}} */

phpdbg_input_t *phpdbg_read_input(TSRMLS_D) /* {{{ */
{
	if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		phpdbg_input_t *buffer = NULL;
		size_t cmd_len = 0L;

#ifndef HAVE_LIBREADLINE
		char *cmd = NULL;
		char buf[PHPDBG_MAX_CMD];
		if (!phpdbg_write(PROMPT) ||
			!fgets(buf, PHPDBG_MAX_CMD, stdin)) {
			/* the user has gone away */
			phpdbg_error("Failed to read console !");
			PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
			zend_bailout();
			return NULL;
		}

		cmd = buf;
#else
		char *cmd = readline(PROMPT);
		if (!cmd) {
			/* the user has gone away */
			phpdbg_error("Failed to read console !");
			PHPDBG_G(flags) |= PHPDBG_IS_QUITTING;
			zend_bailout();
			return NULL;
		}

		add_history(cmd);
#endif

		/* allocate and sanitize buffer */
		buffer = (phpdbg_input_t*) emalloc(sizeof(phpdbg_input_t));
		if (!buffer) {
			return NULL;
		}

		buffer->string = phpdbg_trim(cmd, strlen(cmd), &buffer->length);

		if (buffer->string) {
			/* temporary, when we switch to argv/argc handling
				will be unnecessary */
			char *store = (char*) estrdup(buffer->string);

			/* store constant pointer to start of buffer */
			buffer->start = (char* const*) buffer->string;

			buffer->argv = phpdbg_read_argv(
				store, &buffer->argc TSRMLS_CC);

			if (buffer->argc) {
				int arg = 0;

				while (arg < buffer->argc) {
					phpdbg_debug(
						"argv %d=%s", arg, buffer->argv[arg]->string);
					arg++;
				}
			}

			efree(store);
		}

#ifdef HAVE_LIBREADLINE
		if (cmd) {
			free(cmd);
		}
#endif

		return buffer;
	}

	return NULL;
} /* }}} */

void phpdbg_destroy_input(phpdbg_input_t **input TSRMLS_DC) /*{{{ */
{
	if (*input) {

		if ((*input)->string) {
			efree((*input)->string);
		}

		if ((*input)->argc > 0) {
			int arg;
			for (arg=0; arg<(*input)->argc; arg++) {
				phpdbg_destroy_input(
					&(*input)->argv[arg] TSRMLS_CC);
			}
		}

		if ((*input)->argv) {
			efree((*input)->argv);
		}

		efree(*input);
	}
} /* }}} */

int phpdbg_do_cmd_ex(const phpdbg_command_t *command, phpdbg_input_t *input TSRMLS_DC) /* {{{ */
{
	int rc = FAILURE;

	if (input->argc > 0) {
		while (command && command->name && command->handler) {
			if (((command->name_len == input->argv[0]->length) &&
				(memcmp(command->name, input->argv[0]->string, command->name_len) == SUCCESS)) ||
				(command->alias &&
				(input->argv[0]->length == 1) &&
				(command->alias == *input->argv[0]->string))) {
				if (command->subs && input->argc > 1) {
					phpdbg_input_t sub;

					sub.argc = input->argc-1;
					sub.argv = &input->argv[1];

					return phpdbg_do_cmd_ex(command->subs, &sub TSRMLS_CC);
				}

				phpdbg_debug(
					"found command %s for %s with %d arguments",
					command->name, input->argv[0]->string, input->argc-1);
				{
					int arg;
					for (arg=1; arg<input->argc; arg++) {
						phpdbg_debug(
							"\t#%d: [%s=%d]",
							arg,
							input->argv[arg]->string,
							input->argv[arg]->length);
					}
				}
				break;
			}
			command++;
		}
	} else {
		/* this should NEVER happen */
		phpdbg_error(
			"No function executed !!");
	}

	return rc;
} /* }}} */

int phpdbg_do_cmd(const phpdbg_command_t *command, char *cmd_line, size_t cmd_len TSRMLS_DC) /* {{{ */
{
	int rc = FAILURE;
	char *expr = NULL;
#ifndef _WIN32
	const char *cmd = strtok_r(cmd_line, " ", &expr);
#else
	const char *cmd = strtok_s(cmd_line, " ", &expr);
#endif
	size_t expr_len = (cmd != NULL) ? strlen(cmd) : 0;

	while (command && command->name && command->handler) {
		if ((command->name_len == expr_len && memcmp(cmd, command->name, expr_len) == 0)
			|| (expr_len == 1 && command->alias && command->alias == cmd_line[0])) {
			phpdbg_param_t param;
			phpdbg_parse_param(
				expr,
				(cmd_len - expr_len) ? (((cmd_len - expr_len) - sizeof(" "))+1) : 0,
				&param TSRMLS_CC);

			if (command->subs && param.type == STR_PARAM) {
				if (phpdbg_do_cmd(command->subs, param.str, param.len TSRMLS_CC) == SUCCESS) {
					rc = SUCCESS;
					goto done;
				}
			}

			if (command->arg_type == REQUIRED_ARG && param.type == EMPTY_PARAM) {
				phpdbg_error("This command requires argument!");
				rc = FAILURE;
			} else if (command->arg_type == NO_ARG && param.type != EMPTY_PARAM) {
				phpdbg_error("This command does not expect argument!");
				rc = FAILURE;
			} else {
				PHPDBG_G(lcmd) = (phpdbg_command_t*) command;
				phpdbg_clear_param(
					&PHPDBG_G(lparam) TSRMLS_CC);
				PHPDBG_G(lparam) = param;
				rc = command->handler(&param TSRMLS_CC);
			}
			break;
		}
		++command;
	}

done:
	return rc;
} /* }}} */
