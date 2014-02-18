/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
#include "phpdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

PHPDBG_API const char *phpdbg_get_param_type(const phpdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	switch (param->type) {
		case STACK_PARAM:
			return "stack";
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
		case STACK_PARAM:
			/* nope */
		break;
		
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
		case STACK_PARAM:
			/* nope */
		break;
		
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
				case STACK_PARAM:
					/* nope, or yep */
					return 1;
				break;
				
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

/* {{{ */
PHPDBG_API void phpdbg_param_debug(const phpdbg_param_t *param, const char *msg) {
	if (param && param->type) {
		switch (param->type) {
			case STR_PARAM:
				fprintf(stderr, "%s STR_PARAM(%s=%d)\n", msg, param->str, param->len);
			break;
			
			case ADDR_PARAM:
				fprintf(stderr, "%s ADDR_PARAM(%lu)\n", msg, param->addr);
			break;
			
			case FILE_PARAM:
				fprintf(stderr, "%s FILE_PARAM(%s:%d)\n", msg, param->file.name, param->file.line);
			break;
			
			case METHOD_PARAM:
				fprintf(stderr, "%s METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_METHOD_PARAM:
				fprintf(stderr, "%s NUMERIC_METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_FUNCTION_PARAM:
				fprintf(stderr, "%s NUMERIC_FUNCTION_PARAM(%s::%s)\n", msg, param->str, param->num);
			break;
			
			case NUMERIC_PARAM:
				fprintf(stderr, "%s NUMERIC_PARAM(%ld)\n", msg, param->num);
			break;
			
			case COND_PARAM:
				fprintf(stderr, "%s COND_PARAM(%s=%d)\n", msg, param->str, param->len);
			break;
		}
	}
} /* }}} */

/* {{{ */
PHPDBG_API void phpdbg_stack_free(phpdbg_param_t *stack) {
	if (stack && stack->next) {
		phpdbg_param_t *remove = stack->next;
		
		while (remove) {
			phpdbg_param_t *next = NULL;
			
			if (remove->next)
				next = remove->next;
			
			switch (remove->type) {
				case STR_PARAM: 
					if (remove->str) {
						free(remove->str);	
					}
				break;
				
				case FILE_PARAM:
					if (remove->file.name) {
						free(remove->file.name);
					}
				break;
			}
			
			free(remove);
			
			if (next) 
				remove = next; 
			else break;
		}
	}
} /* }}} */

/* {{{ */
PHPDBG_API void phpdbg_stack_push(phpdbg_param_t *stack, phpdbg_param_t *param) {
	phpdbg_param_t *next = calloc(1, sizeof(phpdbg_param_t));
	
	if (!next)
		return;
	
	*(next) = *(param);

	if (stack->top == NULL) {
		stack->top = next;
		stack->next = next;
	} else {
		stack->top->next = next;
		next->top = stack->top;
		stack->top = next;
	}
	
	stack->len++;
} /* }}} */

/* {{{ */
PHPDBG_API phpdbg_command_t* phpdbg_stack_resolve(const phpdbg_command_t *commands, phpdbg_param_t **top, char **why) {
	const phpdbg_command_t *command = commands;
	phpdbg_param_t *name = *top;
	phpdbg_command_t *matched[3] = {NULL, NULL, NULL};
	ulong matches = 0L;
	
	while (command && command->name && command->handler) {
		if (command->name_len >= name->len) {
			if (memcmp(command->name, name->str, name->len) == SUCCESS) {
				if (matches < 3) {
					matched[matches] = command;
					matches++;
				} else break;
			}
		}
		command++;
	}
	
	switch (matches) {
		case 0: { 
			asprintf(
				why,
				"The command %s could not be found", 
				name->str);
		} break;
		
		case 1: {
			(*top) = (*top)->next;
			if (matched[0]->subs && (*top) && ((*top)->type == STR_PARAM)) {
				command = phpdbg_stack_resolve(matched[0]->subs, top, why);
				if (command) {
					return command;
				}
			}
			
			return matched[0];
		} break;
		
		default: {
			asprintf(
				why,
				"The command %s is ambigious, matching %d commands", 
				name->str, matches);
		}
	}
	
	return NULL;
} /* }}} */

/* {{{ */
PHPDBG_API int phpdbg_stack_execute(phpdbg_param_t *stack, char **why) {
	phpdbg_param_t *command = NULL,
				   *params = NULL;
	phpdbg_command_t *handler = NULL;
	
	if (stack->type != STACK_PARAM) {
		asprintf(
			why, "the passed argument was not a stack !!");
		return FAILURE;
	}
	
	if (!stack->len) {
		asprintf(
			why, "the stack contains nothing !!");
		return FAILURE;
	}
	
	command = (phpdbg_param_t*) stack->next;
	
	switch (command->type) {
		case EVAL_PARAM:
			return PHPDBG_COMMAND_HANDLER(eval)(command, NULL TSRMLS_CC);
		
		case SHELL_PARAM:
			return PHPDBG_COMMAND_HANDLER(shell)(command, NULL TSRMLS_CC);
		
		case STR_PARAM: {
			handler = phpdbg_stack_resolve(
				phpdbg_prompt_commands, &command, why);
			
			if (handler) {
				return handler->handler(command, NULL TSRMLS_CC);
			} else {
				return FAILURE;
			}
		} break;
		
		default:
			asprintf(
				why, "the first parameter makes no sense !!");
			return FAILURE;
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_API char* phpdbg_read_input(char *buffered TSRMLS_DC) /* {{{ */
{
	char *cmd = NULL;
#ifndef HAVE_LIBREADLINE
	char buf[PHPDBG_MAX_CMD];
#endif
	char *buffer = NULL;

	if (!(PHPDBG_G(flags) & PHPDBG_IS_QUITTING)) {
		if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE) &&
			(buffered == NULL)) {
			fflush(PHPDBG_G(io)[PHPDBG_STDOUT]);
		}

		if (buffered == NULL) {
disconnect:
			if (0) {
				PHPDBG_G(flags) |= (PHPDBG_IS_QUITTING|PHPDBG_IS_DISCONNECTED);
				zend_bailout();
				return NULL;
			}

#ifndef HAVE_LIBREADLINE
			if (!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
				if (!phpdbg_write("%s", phpdbg_get_prompt(TSRMLS_C))) {
					goto disconnect;
				}
			}
			
			/* note: EOF is ignored */
readline:	
			if (!fgets(buf, PHPDBG_MAX_CMD, PHPDBG_G(io)[PHPDBG_STDIN])) {
				/* the user has gone away */
				if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
					goto disconnect;
				} else goto readline;
			}

			cmd = buf;
#else
			/* note: EOF makes readline write prompt again in local console mode */
readline:
			if ((PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
				char buf[PHPDBG_MAX_CMD];
				if (fgets(buf, PHPDBG_MAX_CMD, PHPDBG_G(io)[PHPDBG_STDIN])) {
					cmd = buf;
				} else goto disconnect;
			} else cmd = readline(phpdbg_get_prompt(TSRMLS_C));

			if (!cmd) {
				goto readline;
			}

			if (!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
				add_history(cmd);
			}
#endif
		} else cmd = buffered;
		
		buffer = estrdup(cmd);

#ifdef HAVE_LIBREADLINE
		if (!buffered && cmd &&
			!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
			free(cmd);
		}
#endif
	}

	return buffer;
} /* }}} */

PHPDBG_API void phpdbg_destroy_input(char **input TSRMLS_DC) /*{{{ */
{
	efree(*input);
} /* }}} */

