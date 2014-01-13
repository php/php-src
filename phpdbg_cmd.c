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
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"
#include "phpdbg_set.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_API const char *phpdbg_get_param_type(const phpdbg_param_t *param TSRMLS_DC) /* {{{ */
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
		case NUMERIC_FUNCTION_PARAM:
			return "function opline";
		case NUMERIC_METHOD_PARAM:
			return "method opline";
		case FILE_PARAM:
			return "file or file opline";
		case STR_PARAM:
			return "string";
		default: /* this is bad */
			return "unknown";
	}
}

PHPDBG_API phpdbg_param_type phpdbg_parse_param(const char *str, size_t len, phpdbg_param_t *param TSRMLS_DC) /* {{{ */
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
		char *line_pos = strrchr(str, ':');

		if (line_pos && phpdbg_is_numeric(line_pos+1)) {
			if (strchr(str, ':') == line_pos) {
				char path[MAXPATHLEN];

				memcpy(path, str, line_pos - str);
				path[line_pos - str] = 0;
				*line_pos = 0;
				param->file.name = phpdbg_resolve_path(path TSRMLS_CC);
				param->file.line = strtol(line_pos+1, NULL, 0);
				param->type = FILE_PARAM;

				goto parsed;
			}
		}

		line_pos = strrchr(str, '#');

		if (line_pos && phpdbg_is_numeric(line_pos+1)) {
			if (strchr(str, '#') == line_pos) {
				param->num = strtol(line_pos + 1, NULL, 0);

				if (phpdbg_is_class_method(str, line_pos - str, &class_name, &func_name)) {
					param->method.class = class_name;
					param->method.name = func_name;
					param->type = NUMERIC_METHOD_PARAM;
				} else {
					param->len = line_pos - str;
					param->str = estrndup(str, param->len);
					param->type = NUMERIC_FUNCTION_PARAM;
				}

				goto parsed;
			}
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

PHPDBG_API void phpdbg_clear_param(phpdbg_param_t *param TSRMLS_DC) /* {{{ */
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

PHPDBG_API char* phpdbg_param_tostring(const phpdbg_param_t *param, char **pointer TSRMLS_DC) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			asprintf(pointer,
				"%s", param->str);
		break;

		case ADDR_PARAM:
			asprintf(pointer,
				"%#lx", param->addr);
		break;

		case NUMERIC_PARAM:
			asprintf(pointer,
				"%li",
				param->num);
		break;

		case METHOD_PARAM:
			asprintf(pointer,
				"%s::%s",
				param->method.class,
				param->method.name);
		break;

		case FILE_PARAM:
			if (param->num) {
				asprintf(pointer,
					"%s:%lu#%lu",
					param->file.name,
					param->file.line,
					param->num);
			} else {
				asprintf(pointer,
					"%s:%lu",
					param->file.name,
					param->file.line);
			}
		break;

		case NUMERIC_FUNCTION_PARAM:
			asprintf(pointer,
				"%s#%lu", param->str, param->num);
		break;

		case NUMERIC_METHOD_PARAM:
			asprintf(pointer,
				"%s::%s#%lu",
				param->method.class,
				param->method.name,
				param->num);
		break;

		default:
			asprintf(pointer,
				"%s", "unknown");
	}

	return *pointer;
} /* }}} */

PHPDBG_API void phpdbg_copy_param(const phpdbg_param_t* src, phpdbg_param_t* dest TSRMLS_DC) /* {{{ */
{
	switch ((dest->type = src->type)) {
		case STR_PARAM:
			dest->str = estrndup(src->str, src->len);
			dest->len = src->len;
		break;

		case ADDR_PARAM:
			dest->addr = src->addr;
		break;

		case NUMERIC_PARAM:
			dest->num = src->num;
		break;

		case METHOD_PARAM:
			dest->method.class = estrdup(src->method.class);
			dest->method.name = estrdup(src->method.name);
		break;

		case FILE_PARAM:
			dest->file.name = estrdup(src->file.name);
			dest->file.line = src->file.line;
			if (src->num)
				dest->num   = src->num;
		break;

		case NUMERIC_FUNCTION_PARAM:
			dest->str = estrndup(src->str, src->len);
			dest->num = src->num;
			dest->len = src->len;
		break;

		case NUMERIC_METHOD_PARAM:
			dest->method.class = estrdup(src->method.class);
			dest->method.name = estrdup(src->method.name);
			dest->num = src->num;
		break;

		case EMPTY_PARAM: { /* do nothing */ } break;
	}
} /* }}} */

PHPDBG_API zend_ulong phpdbg_hash_param(const phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	zend_ulong hash = param->type;

	switch (param->type) {
		case STR_PARAM:
			hash += zend_inline_hash_func(param->str, param->len);
		break;

		case METHOD_PARAM:
			hash += zend_inline_hash_func(param->method.class, strlen(param->method.class));
			hash += zend_inline_hash_func(param->method.name, strlen(param->method.name));
		break;

		case FILE_PARAM:
			hash += zend_inline_hash_func(param->file.name, strlen(param->file.name));
			hash += param->file.line;
			if (param->num)
				hash += param->num;
		break;

		case ADDR_PARAM:
			hash += param->addr;
		break;

		case NUMERIC_PARAM:
			hash += param->num;
		break;

		case NUMERIC_FUNCTION_PARAM:
			hash += zend_inline_hash_func(param->str, param->len);
			hash += param->num;
		break;

		case NUMERIC_METHOD_PARAM:
			hash += zend_inline_hash_func(param->method.class, strlen(param->method.class));
			hash += zend_inline_hash_func(param->method.name, strlen(param->method.name));
			if (param->num)
				hash+= param->num;
		break;

		case EMPTY_PARAM: { /* do nothing */ } break;
	}

	return hash;
} /* }}} */

PHPDBG_API zend_bool phpdbg_match_param(const phpdbg_param_t *l, const phpdbg_param_t *r TSRMLS_DC) /* {{{ */
{
	if (l && r) {
		if (l->type == r->type) {
			switch (l->type) {

				case NUMERIC_FUNCTION_PARAM:
					if (l->num != r->num) {
						break;
					}
				/* break intentionally omitted */

				case STR_PARAM:
					return (l->len == r->len) &&
							(memcmp(l->str, r->str, l->len) == SUCCESS);

				case NUMERIC_PARAM:
					return (l->num == r->num);

				case ADDR_PARAM:
					return (l->addr == r->addr);

				case FILE_PARAM: {
					if (l->file.line == r->file.line) {
						size_t lengths[2] = {
							strlen(l->file.name), strlen(r->file.name)};

						if (lengths[0] == lengths[1]) {
							if ((!l->num && !r->num) || (l->num == r->num)) {
								return (memcmp(
									l->file.name, r->file.name, lengths[0]) == SUCCESS);
							}
						}
					}
				} break;

				case NUMERIC_METHOD_PARAM:
					if (l->num != r->num) {
						break;
					}
				/* break intentionally omitted */

				case METHOD_PARAM: {
					size_t lengths[2] = {
						strlen(l->method.class), strlen(r->method.class)};
					if (lengths[0] == lengths[1]) {
						if (memcmp(l->method.class, r->method.class, lengths[0]) == SUCCESS) {
							lengths[0] = strlen(l->method.name);
							lengths[1] = strlen(r->method.name);

							if (lengths[0] == lengths[1]) {
								return (memcmp(
									l->method.name, r->method.name, lengths[0]) == SUCCESS);
							}
						}
					}
				} break;

				case EMPTY_PARAM:
					return 1;
			}
		}
	}
	return 0;
} /* }}} */

PHPDBG_API phpdbg_input_t **phpdbg_read_argv(char *buffer, int *argc TSRMLS_DC) /* {{{ */
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

	argv = (phpdbg_input_t**) emalloc(sizeof(phpdbg_input_t*));
	(*argc) = 0;

#define RESET_STATE() do { \
	phpdbg_input_t *arg = emalloc(sizeof(phpdbg_input_t)); \
	if (arg) { \
		b[l]=0; \
		arg->length = l; \
		arg->string = estrndup(b, arg->length); \
		arg->argv = NULL; \
		arg->argc = 0; \
		argv = (phpdbg_input_t**) erealloc(argv, sizeof(phpdbg_input_t*) * ((*argc)+1)); \
		argv[(*argc)++] = arg; \
		l = 0; \
	} \
	state = IN_BETWEEN; \
} while (0)

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
				if (c == '"') {
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
				"Malformed command line (unclosed quote) @ %ld: %s!",
				(p - buffer)-1, &buffer[(p - buffer)-1]);
		break;

		case IN_BETWEEN:
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

PHPDBG_API phpdbg_input_t *phpdbg_read_input(char *buffered TSRMLS_DC) /* {{{ */
{
	phpdbg_input_t *buffer = NULL;
	char *cmd = NULL;

	if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE) &&
			(buffered == NULL)) {
			fflush(PHPDBG_G(io)[PHPDBG_STDOUT]);
		}

		if (buffered == NULL) {
#ifndef HAVE_LIBREADLINE
			char buf[PHPDBG_MAX_CMD];
			if ((!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE) && !phpdbg_write(phpdbg_get_prompt(TSRMLS_C))) ||
				!fgets(buf, PHPDBG_MAX_CMD, PHPDBG_G(io)[PHPDBG_STDIN])) {
				/* the user has gone away */
				phpdbg_error("Failed to read console!");
				PHPDBG_G(flags) |= (PHPDBG_IS_QUITTING|PHPDBG_IS_DISCONNECTED);
				zend_bailout();
				return NULL;
			}

			cmd = buf;
#else
			if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
				char buf[PHPDBG_MAX_CMD];
				if (fgets(buf, PHPDBG_MAX_CMD, PHPDBG_G(io)[PHPDBG_STDIN])) {
					cmd = buf;
				} else cmd = NULL;
			} else cmd = readline(phpdbg_get_prompt(TSRMLS_C));

			if (!cmd) {
				/* the user has gone away */
				phpdbg_error("Failed to read console!");
				PHPDBG_G(flags) |= (PHPDBG_IS_QUITTING|PHPDBG_IS_DISCONNECTED);
				zend_bailout();
				return NULL;
			}

			if (!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
				add_history(cmd);
			}
#endif
		} else cmd = buffered;

		/* allocate and sanitize buffer */
		buffer = (phpdbg_input_t*) ecalloc(1, sizeof(phpdbg_input_t));
		if (!buffer) {
			return NULL;
		}

		buffer->string = phpdbg_trim(cmd, strlen(cmd), &buffer->length);

		/* store constant pointer to start of buffer */
		buffer->start = (char* const*) buffer->string;

		buffer->argv = phpdbg_read_argv(
			buffer->string, &buffer->argc TSRMLS_CC);

#ifdef PHPDBG_DEBUG
		if (buffer->argc) {
			int arg = 0;

			while (arg < buffer->argc) {
				phpdbg_debug(
					"argv %d=%s", arg, buffer->argv[arg]->string);
				arg++;
			}
		}
#endif

#ifdef HAVE_LIBREADLINE
		if (!buffered && cmd &&
			!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
			free(cmd);
		}
#endif

		return buffer;
	}

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_destroy_argv(phpdbg_input_t **argv, int argc TSRMLS_DC) /* {{{ */
{
	if (argv) {
		if (argc) {
			int arg;
			for (arg=0; arg<argc; arg++) {
				phpdbg_destroy_input(
					&argv[arg] TSRMLS_CC);
			}
		}
		efree(argv);
	}

} /* }}} */

PHPDBG_API void phpdbg_destroy_input(phpdbg_input_t **input TSRMLS_DC) /*{{{ */
{
	if (*input) {
		if ((*input)->string) {
			efree((*input)->string);
		}

		phpdbg_destroy_argv(
			(*input)->argv, (*input)->argc TSRMLS_CC);

		efree(*input);
	}
} /* }}} */

PHPDBG_API int phpdbg_do_cmd(const phpdbg_command_t *command, phpdbg_input_t *input TSRMLS_DC) /* {{{ */
{
	int rc = FAILURE;

	if (input->argc > 0) {
		while (command && command->name && command->handler) {
			if (((command->name_len == input->argv[0]->length) &&
				(memcmp(command->name, input->argv[0]->string, command->name_len) == SUCCESS)) ||
				(command->alias &&
				(input->argv[0]->length == 1) &&
				(command->alias == *input->argv[0]->string))) {

				phpdbg_param_t param;

				param.type = EMPTY_PARAM;

				if (input->argc > 1) {
					if (command->subs) {
						phpdbg_input_t sub = *input;

						sub.string += input->argv[0]->length;
						sub.length -= input->argv[0]->length;

						sub.string = phpdbg_trim(
							sub.string, sub.length, &sub.length);

						sub.argc--;
						sub.argv++;

						phpdbg_debug(
							"trying sub commands in \"%s\" for \"%s\" with %d arguments",
							command->name, sub.argv[0]->string, sub.argc-1);

						if (phpdbg_do_cmd(command->subs, &sub TSRMLS_CC) == SUCCESS) {
							efree(sub.string);
							return SUCCESS;
						}

						efree(sub.string);
					}

					/* no sub command found */
					{
						char *store = input->string;

						input->string += input->argv[0]->length;
						input->length -= input->argv[0]->length;

						input->string = phpdbg_trim(
							input->string, input->length, &input->length);

						efree(store);
					}

					/* pass parameter on */
					phpdbg_parse_param(
						input->string,
						input->length,
						&param TSRMLS_CC);
				}

				phpdbg_debug(
					"found command %s for %s with %d arguments",
					command->name, input->argv[0]->string, input->argc-1);
				{
					int arg;
					for (arg=1; arg<input->argc; arg++) {
						phpdbg_debug(
							"\t#%d: [%s=%zu]",
							arg,
							input->argv[arg]->string,
							input->argv[arg]->length);
					}
				}

				rc = command->handler(&param, input TSRMLS_CC);

				/* only set last command when it is worth it! */
				if ((rc != FAILURE) &&
					!(PHPDBG_G(flags) & PHPDBG_IS_INITIALIZING)) {
					PHPDBG_G(lcmd) = (phpdbg_command_t*) command;
					phpdbg_clear_param(
						&PHPDBG_G(lparam) TSRMLS_CC);
					PHPDBG_G(lparam) = param;
				}
				break;
			}
			command++;
		}
	} else {
		/* this should NEVER happen */
		phpdbg_error(
			"No function executed!!");
	}

	return rc;
} /* }}} */

