/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

#ifndef PHPDBG_CMD_H
#define PHPDBG_CMD_H

#include "TSRM.h"

/* {{{ Command and Parameter */
enum {
	NO_ARG = 0,
	REQUIRED_ARG,
	OPTIONAL_ARG
};

typedef enum {
	EMPTY_PARAM = 0,
	ADDR_PARAM,
	FILE_PARAM,
	NUMERIC_FILE_PARAM,
	METHOD_PARAM,
	STR_PARAM,
	NUMERIC_PARAM,
	NUMERIC_FUNCTION_PARAM,
	NUMERIC_METHOD_PARAM,
	STACK_PARAM,
	EVAL_PARAM,
	SHELL_PARAM,
	COND_PARAM,
	OP_PARAM,
	ORIG_PARAM,
	RUN_PARAM
} phpdbg_param_type;

typedef struct _phpdbg_param phpdbg_param_t;
struct _phpdbg_param {
	phpdbg_param_type type;
	long num;
	zend_ulong addr;
	struct {
		char *name;
		long line;
	} file;
	struct {
		char *class;
		char *name;
	} method;
	char *str;
	size_t len;
	phpdbg_param_t *next;
	phpdbg_param_t *top;
};

#define phpdbg_init_param(v, t) do{ \
	(v)->type = (t); \
	(v)->addr = 0; \
	(v)->num = 0; \
	(v)->file.name = NULL; \
	(v)->file.line = 0; \
	(v)->method.class = NULL; \
	(v)->method.name = NULL; \
	(v)->str = NULL; \
	(v)->len = 0; \
	(v)->next = NULL; \
	(v)->top = NULL; \
} while(0)

#ifndef YYSTYPE
#define YYSTYPE phpdbg_param_t
#endif

#define PHPDBG_ASYNC_SAFE 1

typedef int (*phpdbg_command_handler_t)(const phpdbg_param_t*);

typedef struct _phpdbg_command_t phpdbg_command_t;
struct _phpdbg_command_t {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	const char *tip;                    /* Menu tip */
	size_t tip_len;                     /* Menu tip length */
	char alias;                         /* Alias */
	phpdbg_command_handler_t handler;   /* Command handler */
	const phpdbg_command_t *subs;       /* Sub Commands */
	char *args;                         /* Argument Spec */
	const phpdbg_command_t *parent;     /* Parent Command */
	zend_bool flags;                    /* General flags */
};
/* }}} */

/* {{{ misc */
#define PHPDBG_STRL(s) s, sizeof(s)-1
#define PHPDBG_MAX_CMD 500
#define PHPDBG_FRAME(v) (PHPDBG_G(frame).v)
#define PHPDBG_EX(v) (EG(current_execute_data)->v)

typedef struct {
	int num;
	zend_execute_data *execute_data;
} phpdbg_frame_t;
/* }}} */

/*
* Workflow:
* 1) the lexer/parser creates a stack of commands and arguments from input
* 2) the commands at the top of the stack are resolved sensibly using aliases, abbreviations and case insensitive matching
* 3) the remaining arguments in the stack are verified (optionally) against the handlers declared argument specification
* 4) the handler is called passing the top of the stack as the only parameter
* 5) the stack is destroyed upon return from the handler
*/

/*
* Input Management
*/
PHPDBG_API char* phpdbg_read_input(char *buffered);
PHPDBG_API void phpdbg_destroy_input(char**);
PHPDBG_API int phpdbg_ask_user_permission(const char *question);

/**
 * Stack Management
 */
PHPDBG_API void phpdbg_stack_push(phpdbg_param_t *stack, phpdbg_param_t *param);
PHPDBG_API void phpdbg_stack_separate(phpdbg_param_t *param);
PHPDBG_API const phpdbg_command_t *phpdbg_stack_resolve(const phpdbg_command_t *commands, const phpdbg_command_t *parent, phpdbg_param_t **top);
PHPDBG_API int phpdbg_stack_verify(const phpdbg_command_t *command, phpdbg_param_t **stack);
PHPDBG_API int phpdbg_stack_execute(phpdbg_param_t *stack, zend_bool allow_async_unsafe);
PHPDBG_API void phpdbg_stack_free(phpdbg_param_t *stack);

/*
* Parameter Management
*/
PHPDBG_API void phpdbg_clear_param(phpdbg_param_t*);
PHPDBG_API void phpdbg_copy_param(const phpdbg_param_t*, phpdbg_param_t*);
PHPDBG_API zend_bool phpdbg_match_param(const phpdbg_param_t *, const phpdbg_param_t *);
PHPDBG_API zend_ulong phpdbg_hash_param(const phpdbg_param_t *);
PHPDBG_API const char* phpdbg_get_param_type(const phpdbg_param_t*);
PHPDBG_API char* phpdbg_param_tostring(const phpdbg_param_t *param, char **pointer);
PHPDBG_API void phpdbg_param_debug(const phpdbg_param_t *param, const char *msg);

/**
 * Command Declarators
 */
#define PHPDBG_COMMAND_HANDLER(name) phpdbg_do_##name

#define PHPDBG_COMMAND_D_EXP(name, tip, alias, handler, children, args, parent, flags) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##handler, children, args, parent, flags}

#define PHPDBG_COMMAND_D_EX(name, tip, alias, handler, children, args, flags) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##handler, children, args, NULL, flags}

#define PHPDBG_COMMAND_D(name, tip, alias, children, args, flags) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##name, children, args, NULL, flags}

#define PHPDBG_COMMAND(name) int phpdbg_do_##name(const phpdbg_param_t *param)

#define PHPDBG_COMMAND_ARGS param

#define PHPDBG_END_COMMAND {NULL, 0, NULL, 0, '\0', NULL, NULL, NULL, NULL, 0}

/*
* Default Switch Case
*/
#define phpdbg_default_switch_case() \
	default: \
		phpdbg_error("command", "type=\"wrongarg\" got=\"%s\"", "Unsupported parameter type (%s) for command", phpdbg_get_param_type(param)); \
	break

#endif /* PHPDBG_CMD_H */
