/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

static inline const char *phpdbg_command_name(const phpdbg_command_t *command, char *buffer) {
	size_t pos = 0;

	if (command->parent) {
		memcpy(&buffer[pos], command->parent->name, command->parent->name_len);
		pos += command->parent->name_len;
		memcpy(&buffer[pos], " ", sizeof(" ")-1);
		pos += (sizeof(" ")-1);
	}

	memcpy(&buffer[pos], command->name, command->name_len);
	pos += command->name_len;
	buffer[pos] = 0;
	
	return buffer;
}

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
		
		case OP_PARAM:
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

		case NUMERIC_FILE_PARAM:
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
		
		default: {
			/* not yet */
		}
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
		
		default: {
			/* not yet */
		}
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
					
				default: {
					/* not yet */
				}
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
				fprintf(stderr, "%s STR_PARAM(%s=%lu)\n", msg, param->str, param->len);
			break;
			
			case ADDR_PARAM:
				fprintf(stderr, "%s ADDR_PARAM(%lu)\n", msg, param->addr);
			break;
			
			case NUMERIC_FILE_PARAM:
				fprintf(stderr, "%s NUMERIC_FILE_PARAM(%s:#%lu)\n", msg, param->file.name, param->file.line);
			break;
			
			case FILE_PARAM:
				fprintf(stderr, "%s FILE_PARAM(%s:%lu)\n", msg, param->file.name, param->file.line);
			break;
			
			case METHOD_PARAM:
				fprintf(stderr, "%s METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_METHOD_PARAM:
				fprintf(stderr, "%s NUMERIC_METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_FUNCTION_PARAM:
				fprintf(stderr, "%s NUMERIC_FUNCTION_PARAM(%s::%ld)\n", msg, param->str, param->num);
			break;
			
			case NUMERIC_PARAM:
				fprintf(stderr, "%s NUMERIC_PARAM(%ld)\n", msg, param->num);
			break;
			
			case COND_PARAM:
				fprintf(stderr, "%s COND_PARAM(%s=%lu)\n", msg, param->str, param->len);
			break;
			
			case OP_PARAM:
				fprintf(stderr, "%s OP_PARAM(%s=%lu)\n", msg, param->str, param->len);
			break;
			
			default: {
				/* not yet */
			}
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
				case NUMERIC_METHOD_PARAM:
				case METHOD_PARAM:
					if (remove->method.class)
						free(remove->method.class);
					if (remove->method.name)
						free(remove->method.name);
				break;

				case NUMERIC_FUNCTION_PARAM:
				case STR_PARAM:
				case OP_PARAM:
					if (remove->str)
						free(remove->str);	
				break;
				
				case NUMERIC_FILE_PARAM:
				case FILE_PARAM:
					if (remove->file.name)
						free(remove->file.name);
				break;
				
				default: {
					/* nothing */
				}
			}
			
			free(remove);
			remove = NULL;
			
			if (next)
				remove = next; 
			else break;
		}
	}
	
	stack->next = NULL;
} /* }}} */

/* {{{ */
PHPDBG_API void phpdbg_stack_push(phpdbg_param_t *stack, phpdbg_param_t *param) {
	phpdbg_param_t *next = calloc(1, sizeof(phpdbg_param_t));

	if (!next)
		return;

	*(next) = *(param);

	next->next = NULL;

	if (stack->top == NULL) {
		stack->top = next;
		next->top = NULL;
		stack->next = next;
	} else {
		stack->top->next = next;
		next->top = stack->top;
		stack->top = next;
	}

	stack->len++;
} /* }}} */

PHPDBG_API int phpdbg_stack_verify(const phpdbg_command_t *command, phpdbg_param_t **stack, char **why TSRMLS_DC) {
	if (command) {
		char buffer[128] = {0,};
		const phpdbg_param_t *top = (stack != NULL) ? *stack : NULL;
		const char *arg = command->args;
		size_t least = 0L,
			   received = 0L,
			   current = 0L;
		zend_bool optional = 0;
		
		/* check for arg spec */
		if (!(arg) || !(*arg)) {
			if (!top) {
				return SUCCESS;
			}
			
			asprintf(why,
				"The command \"%s\" expected no arguments", 
				phpdbg_command_name(command, buffer));
			return FAILURE;
		}
		
		least = 0L;
		
		/* count least amount of arguments */
		while (arg && *arg) {
			if (arg[0] == '|') {
				break;
			}
			least++;
			arg++;
		}
		
		arg = command->args;

#define verify_arg(e, a, t) if (!(a)) { \
	if (!optional) { \
		asprintf(why, \
			"The command \"%s\" expected %s and got nothing at parameter %lu", \
			phpdbg_command_name(command, buffer), \
			(e), \
			current); \
		return FAILURE;\
	} \
} else if ((a)->type != (t)) { \
	asprintf(why, \
		"The command \"%s\" expected %s and got %s at parameter %lu", \
		phpdbg_command_name(command, buffer), \
		(e),\
		phpdbg_get_param_type((a) TSRMLS_CC), \
		current); \
	return FAILURE; \
}

		while (arg && *arg) {
			current++;
			
			switch (*arg) {
				case '|': {
					current--;
					optional = 1;
					arg++;
				} continue;
				
				case 'i': verify_arg("raw input", top, STR_PARAM); break;
				case 's': verify_arg("string", top, STR_PARAM); break;
				case 'n': verify_arg("number", top, NUMERIC_PARAM); break;
				case 'm': verify_arg("method", top, METHOD_PARAM); break;
				case 'a': verify_arg("address", top, ADDR_PARAM); break;
				case 'f': verify_arg("file:line", top, FILE_PARAM); break;
				case 'c': verify_arg("condition", top, COND_PARAM); break;
				case 'o': verify_arg("opcode", top, OP_PARAM); break;
				case 'b': verify_arg("boolean", top, NUMERIC_PARAM); break;
				
				case '*': { /* do nothing */ } break;
			}
			
			if (top ) {
				top = top->next;
			} else break;
			
			received++;
			arg++;
		}

#undef verify_arg

		if ((received < least)) {
			asprintf(why,
				"The command \"%s\" expected at least %lu arguments (%s) and received %lu",
				phpdbg_command_name(command, buffer),
				least,
				command->args, 
				received);
			return FAILURE;
		}
	}
	
	return SUCCESS;
}

/* {{{ */
PHPDBG_API const phpdbg_command_t* phpdbg_stack_resolve(const phpdbg_command_t *commands, const phpdbg_command_t *parent, phpdbg_param_t **top, char **why) {
	const phpdbg_command_t *command = commands;
	phpdbg_param_t *name = *top;
	const phpdbg_command_t *matched[3] = {NULL, NULL, NULL};
	ulong matches = 0L;
	
	while (command && command->name && command->handler) {
		if ((name->len == 1) || (command->name_len >= name->len)) {
			/* match single letter alias */
			if (command->alias && (name->len == 1)) {
				if (command->alias == (*name->str)) {
					matched[matches] = command;
					matches++;
				}
			} else {

				/* match full, case insensitive, command name */
				if (strncasecmp(command->name, name->str, name->len) == SUCCESS) {
					if (matches < 3) {
						
						/* only allow abbreviating commands that can be aliased */
						if (((name->len != command->name_len) && command->alias) ||
							(name->len == command->name_len)) {
							matched[matches] = command;
							matches++;
						}
						
						
						/* exact match */
						if (name->len == command->name_len)
							break;
					} else break;
				}
			}
		}
		
		command++;
	}
	
	switch (matches) {
		case 0: {
			if (parent) {
				asprintf(
				why,
				"The command \"%s %s\" could not be found", 
				parent->name, name->str);
			} else asprintf(
				why,
				"The command \"%s\" could not be found", 
				name->str);
		} return parent;
		
		case 1: {
			(*top) = (*top)->next;

			command = matched[0];
		} break;
		
		default: {
			char *list = NULL;
			zend_uint it = 0;
			size_t pos = 0;
			
			while (it < matches) {
				if (!list) {
					list = malloc(
						matched[it]->name_len + 1 + 
						((it+1) < matches ? sizeof(", ")-1 : 0));
				} else {
					list = realloc(list, 
						(pos + matched[it]->name_len) + 1  + 
						((it+1) < matches ? sizeof(", ")-1 : 0));
				}
				memcpy(&list[pos], matched[it]->name, matched[it]->name_len);
				pos += matched[it]->name_len;
				if ((it+1) < matches) {
					memcpy(&list[pos], ", ", sizeof(", ")-1);
					pos += (sizeof(", ") - 1);
				}
				
				list[pos] = 0;
				it++;
			}
			
			asprintf(
				why,
				"The command \"%s\" is ambigious, matching %lu commands (%s)", 
				name->str, matches, list);
			free(list);
		} return NULL;
	}

	if (command->subs && (*top) && ((*top)->type == STR_PARAM)) {
		return phpdbg_stack_resolve(command->subs, command, top, why);
	} else {
		return command;
	}

	return NULL;
} /* }}} */

/* {{{ */
PHPDBG_API int phpdbg_stack_execute(phpdbg_param_t *stack, char **why TSRMLS_DC) {
	phpdbg_param_t *top = NULL;
	const phpdbg_command_t *handler = NULL;
	
	if (stack->type != STACK_PARAM) {
		asprintf(
			why, "The passed argument was not a stack !!");
		return FAILURE;
	}
	
	if (!stack->len) {
		asprintf(
			why, "The stack contains nothing !!");
		return FAILURE;
	}
	
	top = (phpdbg_param_t*) stack->next;
	
	switch (top->type) {
		case EVAL_PARAM:
			return PHPDBG_COMMAND_HANDLER(ev)(top TSRMLS_CC);

		case RUN_PARAM:
			return PHPDBG_COMMAND_HANDLER(run)(top TSRMLS_CC);
		
		case SHELL_PARAM:
			return PHPDBG_COMMAND_HANDLER(sh)(top TSRMLS_CC);
		
		case STR_PARAM: {
			handler = phpdbg_stack_resolve(
				phpdbg_prompt_commands, NULL, &top, why);
			
			if (handler) {
				if (phpdbg_stack_verify(handler, &top, why TSRMLS_CC) == SUCCESS) {
					return handler->handler(top TSRMLS_CC);
				}
			}
		} return FAILURE;
		
		default:
			asprintf(
				why, "The first parameter makes no sense !!");
			return FAILURE;
	}
	
	return SUCCESS;
} /* }}} */

PHPDBG_API char* phpdbg_read_input(char *buffered TSRMLS_DC) /* {{{ */
{
	char *cmd = NULL;
#if !defined(HAVE_LIBREADLINE) && !defined(HAVE_LIBEDIT)
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

#if !defined(HAVE_LIBREADLINE) && !defined(HAVE_LIBEDIT)
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

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDIT)
		if (!buffered && cmd &&
			!(PHPDBG_G(flags) & PHPDBG_IS_REMOTE)) {
			free(cmd);
		}
#endif
	}

	if (buffer && isspace(*buffer)) {
		char *trimmed = buffer;
		while (isspace(*trimmed))
			trimmed++;

		trimmed = estrdup(trimmed);
		efree(buffer);
		buffer = trimmed;
	}

	if (buffer && strlen(buffer)) {
		if (PHPDBG_G(buffer)) {
			efree(PHPDBG_G(buffer));
		}
		PHPDBG_G(buffer) = estrdup(buffer);
	} else {
		if (PHPDBG_G(buffer)) {
			buffer = estrdup(PHPDBG_G(buffer));
		}
	}
	
	return buffer;
} /* }}} */

PHPDBG_API void phpdbg_destroy_input(char **input TSRMLS_DC) /*{{{ */
{
	efree(*input);
} /* }}} */

