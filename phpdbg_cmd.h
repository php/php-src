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

#ifndef PHPDBG_CMD_H
#define PHPDBG_CMD_H

#include "TSRM.h"

typedef struct _phpdbg_command_t phpdbg_command_t;

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
	METHOD_PARAM,
	STR_PARAM,
	NUMERIC_PARAM,
	NUMERIC_FUNCTION_PARAM,
	NUMERIC_METHOD_PARAM
} phpdbg_param_type;

typedef struct _phpdbg_input_t phpdbg_input_t;

struct _phpdbg_input_t {
	char * const *start;
	char *string;
	size_t length;
	phpdbg_input_t **argv;
	int argc;
};

typedef struct _phpdbg_param {
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
} phpdbg_param_t;

typedef int (*phpdbg_command_handler_t)(const phpdbg_param_t*, const phpdbg_input_t* TSRMLS_DC);

struct _phpdbg_command_t {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	const char *tip;                    /* Menu tip */
	size_t tip_len;                     /* Menu tip length */
	char alias;                         /* Alias */
	phpdbg_command_handler_t handler;   /* Command handler */
	const phpdbg_command_t *subs;       /* Sub Commands */
	char arg_type;                      /* Accept args? */
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
* 1) read input
*	input takes the line from console, creates argc/argv
* 2) parse parameters into suitable types based on arg_type
*	takes input from 1) and arg_type and creates parameters
* 3) do command
*	executes commands
* 4) destroy parameters
*	cleans up what was allocated by creation of parameters
* 5) destroy input
*	cleans up what was allocated by creation of input
*/

/*
* Input Management
*/
PHPDBG_API phpdbg_input_t* phpdbg_read_input(char *buffered TSRMLS_DC);
PHPDBG_API void phpdbg_destroy_input(phpdbg_input_t** TSRMLS_DC);

/*
* Argument Management
*/
PHPDBG_API phpdbg_input_t** phpdbg_read_argv(char *buffer, int *argc TSRMLS_DC);
PHPDBG_API void phpdbg_destroy_argv(phpdbg_input_t **argv, int argc TSRMLS_DC);
#define phpdbg_argv_is(n, s) \
	(memcmp(input->argv[n]->string, s, input->argv[n]->length) == SUCCESS)

/*
* Parameter Management
*/
PHPDBG_API phpdbg_param_type phpdbg_parse_param(const char*, size_t, phpdbg_param_t* TSRMLS_DC);
PHPDBG_API void phpdbg_clear_param(phpdbg_param_t* TSRMLS_DC);
PHPDBG_API void phpdbg_copy_param(const phpdbg_param_t*, phpdbg_param_t* TSRMLS_DC);
PHPDBG_API zend_bool phpdbg_match_param(const phpdbg_param_t *, const phpdbg_param_t * TSRMLS_DC);
PHPDBG_API zend_ulong phpdbg_hash_param(const phpdbg_param_t * TSRMLS_DC);
PHPDBG_API const char* phpdbg_get_param_type(const phpdbg_param_t* TSRMLS_DC);
PHPDBG_API char* phpdbg_param_tostring(const phpdbg_param_t *param, char **pointer TSRMLS_DC);

/*
* Command Executor
*/
PHPDBG_API int phpdbg_do_cmd(const phpdbg_command_t*, phpdbg_input_t* TSRMLS_DC);

/**
 * Command Declarators
 */
#define PHPDBG_COMMAND_HANDLER(name) phpdbg_do_##name

#define PHPDBG_COMMAND_D_EX(name, tip, alias, handler, children, has_args) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##handler, children, has_args}

#define PHPDBG_COMMAND_D(name, tip, alias, children, has_args) \
	{PHPDBG_STRL(#name), tip, sizeof(tip)-1, alias, phpdbg_do_##name, children, has_args}

#define PHPDBG_COMMAND(name) int phpdbg_do_##name(const phpdbg_param_t *param, const phpdbg_input_t *input TSRMLS_DC)

#define PHPDBG_COMMAND_ARGS param, input TSRMLS_CC

#define PHPDBG_END_COMMAND {NULL, 0, NULL, 0, '\0', NULL, NULL, '\0'}

/*
* Default Switch Case
*/
#define phpdbg_default_switch_case() \
	default: \
		phpdbg_error("Unsupported parameter type (%s) for command", phpdbg_get_param_type(param TSRMLS_CC)); \
	break

#endif /* PHPDBG_CMD_H */
