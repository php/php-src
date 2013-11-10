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

#include <stdio.h>
#include <string.h>
#include "zend.h"
#include "zend_compile.h"
#include "phpdbg.h"
#include "phpdbg_help.h"
#include "phpdbg_bp.h"

static const phpdbg_command_t phpdbg_prompt_commands[];

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static PHPDBG_COMMAND(exec) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		printf("Unsetting old execution context: %s\n", PHPDBG_G(exec));
		efree(PHPDBG_G(exec));
		PHPDBG_G(exec) = NULL;
	}

	if (PHPDBG_G(ops)) {
		printf("Destroying compiled opcodes\n");
		destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
		efree(PHPDBG_G(ops));
		PHPDBG_G(ops) = NULL;
	}

	PHPDBG_G(exec) = estrndup(expr, PHPDBG_G(exec_len) = expr_len);

	printf("Set execution context: %s\n", PHPDBG_G(exec));

	return SUCCESS;
} /* }}} */

static inline int phpdbg_compile(TSRMLS_D) /* {{{ */
{
	zend_file_handle fh;

	printf("Attempting compilation of %s\n", PHPDBG_G(exec));

	if (php_stream_open_for_zend_ex(PHPDBG_G(exec), &fh,
		USE_PATH|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC) == SUCCESS) {
		PHPDBG_G(ops) = zend_compile_file(&fh, ZEND_INCLUDE TSRMLS_CC);
		zend_destroy_file_handle(&fh TSRMLS_CC);
		printf("Success\n");
		return SUCCESS;
    }

	printf("Could not open file %s\n", PHPDBG_G(exec));
	return FAILURE;
} /* }}} */

static PHPDBG_COMMAND(compile) /* {{{ */
{
	if (PHPDBG_G(exec)) {
		if (PHPDBG_G(ops)) {
			printf("Destroying compiled opcodes\n");
			destroy_op_array(PHPDBG_G(ops) TSRMLS_CC);
			efree(PHPDBG_G(ops));
		}

		return phpdbg_compile(TSRMLS_C);
	} else {
		printf("No execution context\n");
		return FAILURE;
	}
} /* }}} */

static PHPDBG_COMMAND(step) /* {{{ */
{
	PHPDBG_G(stepping) = atoi(expr);
	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(next) /* {{{ */
{
	return PHPDBG_NEXT;
} /* }}} */

static PHPDBG_COMMAND(cont) /* {{{ */
{
	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(run) /* {{{ */
{
	if (PHPDBG_G(ops) || PHPDBG_G(exec)) {
		if (!PHPDBG_G(ops)) {
			if (phpdbg_compile(TSRMLS_C) == FAILURE) {
				printf("Failed to compile %s, cannot run\n", PHPDBG_G(exec));
				return FAILURE;
			}
		}

		EG(active_op_array) = PHPDBG_G(ops);
		EG(return_value_ptr_ptr) = &PHPDBG_G(retval);

		zend_try {
			zend_execute(EG(active_op_array) TSRMLS_CC);
		} zend_catch {
			if (!PHPDBG_G(quitting)) {
				printf("Caught excetion in VM\n");
				return FAILURE;
			} else return SUCCESS;
		} zend_end_try();

		return SUCCESS;
	} else {
		printf("Nothing to execute !\n");
		return FAILURE;
	}
} /* }}} */

static PHPDBG_COMMAND(eval) /* {{{ */
{
	zval retval;

	if (expr) {
		if (zend_eval_stringl((char*)expr, expr_len-1, &retval, "eval()'d code" TSRMLS_CC) == SUCCESS) {
			printf("Success: ");
			zend_print_zval_r(&retval, 0 TSRMLS_CC);
			printf("\n");
			zval_dtor(&retval);
		}
	} else {
		printf("No expression provided !\n");
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(back) /* {{{ */
{
	if (!EG(in_execution)) {
		printf("Not executing !\n");
		return FAILURE;
	}
	zval zbacktrace;
	zval **tmp;
	HashPosition position;
	int i = 0, limit = (expr != NULL) ? atoi(expr) : 0;

	zend_fetch_debug_backtrace(&zbacktrace, 0, 0, limit TSRMLS_CC);

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
		zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), (void**)&tmp, &position) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position)) {
		if (i++) {
			printf(",\n");
		}
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
	}

	printf("\n");
	zval_dtor(&zbacktrace);

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(print) /* {{{ */
{
	if (expr_len) {
		printf("%s\n", expr);
		return SUCCESS;
	}

	printf("Showing Execution Context Information:\n");
	printf("Exec\t\t%s\n", PHPDBG_G(exec) ? PHPDBG_G(exec) : "none");
	printf("Compiled\t%s\n", PHPDBG_G(ops) ? "yes" : "no");
	printf("Stepping\t%s\n", PHPDBG_G(stepping) ? "on" : "off");

	if (PHPDBG_G(ops)) {
		printf("Opcodes\t\t%d\n", PHPDBG_G(ops)->last);

		if (PHPDBG_G(ops)->last_var) {
			printf("Variables\t%d\n", PHPDBG_G(ops)->last_var-1);
		} else {
			printf("Variables\tNone\n");
		}
	}
	printf("Executing\t%s\n", EG(in_execution) ? "yes" : "no");

	if (EG(in_execution)) {
		printf("VM Return\t%d\n", PHPDBG_G(vmret));
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(break) /* {{{ */
{
	const char *line_pos = zend_memrchr(expr, ':', expr_len);

	if (line_pos) {
		phpdbg_set_breakpoint_file(expr, line_pos TSRMLS_CC);
	} else {
		const char *opline_num_pos = zend_memrchr(expr, '#', expr_len);

		phpdbg_set_breakpoint_symbol(expr, opline_num_pos TSRMLS_CC);
	}

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(quit) /* {{{ */
{
    PHPDBG_G(quitting)=1;

	zend_bailout();

	return SUCCESS;
} /* }}} */

static PHPDBG_COMMAND(help) /* {{{ */
{
	printf("Welcome to phpdbg, the interactive PHP debugger.\n");

	if (!expr_len) {
		const phpdbg_command_t *prompt_command = phpdbg_prompt_commands;
		const phpdbg_command_t *help_command = phpdbg_help_commands;

		printf("To get help regarding a specific command type \"help command\"\n");

		printf("Commands:\n");
		while (prompt_command && prompt_command->name) {
			printf("\t%s\t%s\n", prompt_command->name, prompt_command->tip);
			++prompt_command;
		}

		printf("Helpers Loaded:\n");
		while (help_command && help_command->name) {
			printf("\t%s\t%s\n", help_command->name, help_command->tip);
			++help_command;
		}
	} else {
		if (phpdbg_do_cmd(phpdbg_help_commands, expr, expr_len TSRMLS_CC) == FAILURE) {
			printf("failed to find help command: %s\n", expr);
		}
	}
	printf("Please report bugs to <https://github.com/krakjoe/phpdbg/issues>\n");

	return SUCCESS;
} /* }}} */

static const phpdbg_command_t phpdbg_prompt_commands[] = {
	PHPDBG_COMMAND_D(exec,      "set execution context"),
	PHPDBG_COMMAND_D(compile,   "attempt to pre-compile execution context"),
	PHPDBG_COMMAND_D(step,      "step through execution"),
	PHPDBG_COMMAND_D(next,      "next opcode"),
	PHPDBG_COMMAND_D(run,       "attempt execution"),
	PHPDBG_COMMAND_D(eval,      "evaluate some code"),
	PHPDBG_COMMAND_D(print,     "print something"),
	PHPDBG_COMMAND_D(break,     "set breakpoint"),
	PHPDBG_COMMAND_D(cont,      "continue execution"),
	PHPDBG_COMMAND_D(back,      "show backtrace"),
	PHPDBG_COMMAND_D(help,      "show help menu"),
	PHPDBG_COMMAND_D(quit,      "exit phpdbg"),
	{NULL, 0, 0}
};

int phpdbg_do_cmd(const phpdbg_command_t *command, char *cmd_line, size_t cmd_len TSRMLS_DC) /* {{{ */
{
	char *params = NULL;
	const char *cmd = strtok_r(cmd_line, " ", &params);
	size_t expr_len = cmd != NULL ? strlen(cmd) : 0;

	while (command && command->name) {
		if (command->name_len == expr_len
			&& memcmp(cmd, command->name, expr_len) == 0) {
			return command->handler(params, cmd_len - expr_len TSRMLS_CC);
		}
		++command;
	}

	return FAILURE;
} /* }}} */

int phpdbg_interactive(int argc, char **argv TSRMLS_DC) /* {{{ */
{
	char cmd[PHPDBG_MAX_CMD];

	printf("phpdbg> ");

	while (!PHPDBG_G(quitting) &&
	       fgets(cmd, PHPDBG_MAX_CMD, stdin) != NULL) {
		size_t cmd_len = strlen(cmd) - 1;

		if (cmd[cmd_len] == '\n') {
			cmd[cmd_len] = 0;
		}

		if (cmd_len) {
		    switch (phpdbg_do_cmd(phpdbg_prompt_commands, cmd, cmd_len TSRMLS_CC)) {
		        case FAILURE:
		            if (!PHPDBG_G(quitting)) {
		                printf(
		                    "Failed to execute %s !\n", cmd);
		            }
		        break;

		        case PHPDBG_NEXT: if (PHPDBG_G(stepping)) {
		            return PHPDBG_NEXT;
		        }

		    }
		}

		if (!PHPDBG_G(quitting)) {
		    printf("phpdbg> ");
		}
	}

	return SUCCESS;
} /* }}} */

static const char *phpdbg_decode_opcode(zend_uchar opcode TSRMLS_DC) { /* {{{ */
    switch (opcode) {
        case ZEND_NOP: return "ZEND_NOP";
        case ZEND_ADD: return "ZEND_ADD";
        case ZEND_SUB: return "ZEND_SUB";
        case ZEND_MUL: return "ZEND_MUL";
        case ZEND_DIV: return "ZEND_DIV";
        case ZEND_MOD: return "ZEND_MOD";
        case ZEND_SL: return "ZEND_SL";
        case ZEND_SR: return "ZEND_SR";
        case ZEND_CONCAT: return "ZEND_CONCAT";
        case ZEND_BW_OR: return "ZEND_BW_OR";
        case ZEND_BW_AND: return "ZEND_BW_AND";
        case ZEND_BW_XOR: return "ZEND_BW_XOR";
        case ZEND_BW_NOT: return "ZEND_BW_NOT";
        case ZEND_BOOL_NOT: return "ZEND_BOOL_NOT";
        case ZEND_BOOL_XOR: return "ZEND_BOOL_XOR";
        case ZEND_IS_IDENTICAL: return "ZEND_IS_IDENTICAL";
        case ZEND_IS_NOT_IDENTICAL: return "ZEND_IS_NOT_IDENTICAL";
        case ZEND_IS_EQUAL: return "ZEND_IS_EQUAL";
        case ZEND_IS_NOT_EQUAL: return "ZEND_IS_NOT_EQUAL";
        case ZEND_IS_SMALLER: return "ZEND_IS_SMALLER";
        case ZEND_IS_SMALLER_OR_EQUAL: return "ZEND_IS_SMALLER_OR_EQUAL";
        case ZEND_CAST: return "ZEND_CAST";
        case ZEND_QM_ASSIGN: return "ZEND_QM_ASSIGN";
        case ZEND_ASSIGN_ADD: return "ZEND_ASSIGN_ADD";
        case ZEND_ASSIGN_SUB: return "ZEND_ASSIGN_SUB";
        case ZEND_ASSIGN_MUL: return "ZEND_ASSIGN_MUL";
        case ZEND_ASSIGN_DIV: return "ZEND_ASSIGN_DIV";
        case ZEND_ASSIGN_MOD: return "ZEND_ASSIGN_MOD";
        case ZEND_ASSIGN_SL: return "ZEND_ASSIGN_SL";
        case ZEND_ASSIGN_SR: return "ZEND_ASSIGN_SR";
        case ZEND_ASSIGN_CONCAT: return "ZEND_ASSIGN_CONCAT";
        case ZEND_ASSIGN_BW_OR: return "ZEND_ASSIGN_BW_OR";
        case ZEND_ASSIGN_BW_AND: return "ZEND_ASSIGN_BW_AND";
        case ZEND_ASSIGN_BW_XOR: return "ZEND_ASSIGN_BW_XOR";
        case ZEND_PRE_INC: return "ZEND_PRE_INC";
        case ZEND_PRE_DEC: return "ZEND_PRE_DEC";
        case ZEND_POST_INC: return "ZEND_POST_INC";
        case ZEND_POST_DEC: return "ZEND_POST_DEC";
        case ZEND_ASSIGN: return "ZEND_ASSIGN";
        case ZEND_ASSIGN_REF: return "ZEND_ASSIGN_REF";
        case ZEND_ECHO: return "ZEND_ECHO";
        case ZEND_PRINT: return "ZEND_PRINT";
        case ZEND_JMP: return "ZEND_JMP";
        case ZEND_JMPZ: return "ZEND_JMPZ";
        case ZEND_JMPNZ: return "ZEND_JMPNZ";
        case ZEND_JMPZNZ: return "ZEND_JMPZNZ";
        case ZEND_JMPZ_EX: return "ZEND_JMPZ_EX";
        case ZEND_JMPNZ_EX: return "ZEND_JMPNZ_EX";
        case ZEND_CASE: return "ZEND_CASE";
        case ZEND_SWITCH_FREE: return "ZEND_SWITCH_FREE";
        case ZEND_BRK: return "ZEND_BRK";
        case ZEND_CONT: return "ZEND_CONT";
        case ZEND_BOOL: return "ZEND_BOOL";
        case ZEND_INIT_STRING: return "ZEND_INIT_STRING";
        case ZEND_ADD_CHAR: return "ZEND_ADD_CHAR";
        case ZEND_ADD_STRING: return "ZEND_ADD_STRING";
        case ZEND_ADD_VAR: return "ZEND_ADD_VAR";
        case ZEND_BEGIN_SILENCE: return "ZEND_BEGIN_SILENCE";
        case ZEND_END_SILENCE: return "ZEND_END_SILENCE";
        case ZEND_INIT_FCALL_BY_NAME: return "ZEND_INIT_FCALL_BY_NAME";
        case ZEND_DO_FCALL: return "ZEND_DO_FCALL";
        case ZEND_DO_FCALL_BY_NAME: return "ZEND_DO_FCALL_BY_NAME";
        case ZEND_RETURN: return "ZEND_RETURN";
        case ZEND_RECV: return "ZEND_RECV";
        case ZEND_RECV_INIT: return "ZEND_RECV_INIT";
        case ZEND_SEND_VAL: return "ZEND_SEND_VAL";
        case ZEND_SEND_VAR: return "ZEND_SEND_VAR";
        case ZEND_SEND_REF: return "ZEND_SEND_REF";
        case ZEND_NEW: return "ZEND_NEW";
        case ZEND_INIT_NS_FCALL_BY_NAME: return "ZEND_INIT_NS_FCALL_BY_NAME";
        case ZEND_FREE: return "ZEND_FREE";
        case ZEND_INIT_ARRAY: return "ZEND_INIT_ARRAY";
        case ZEND_ADD_ARRAY_ELEMENT: return "ZEND_ADD_ARRAY_ELEMENT";
        case ZEND_INCLUDE_OR_EVAL: return "ZEND_INCLUDE_OR_EVAL";
        case ZEND_UNSET_VAR: return "ZEND_UNSET_VAR";
        case ZEND_UNSET_DIM: return "ZEND_UNSET_DIM";
        case ZEND_UNSET_OBJ: return "ZEND_UNSET_OBJ";
        case ZEND_FE_RESET: return "ZEND_FE_RESET";
        case ZEND_FE_FETCH: return "ZEND_FE_FETCH";
        case ZEND_EXIT: return "ZEND_EXIT";
        case ZEND_FETCH_R: return "ZEND_FETCH_R";
        case ZEND_FETCH_DIM_R: return "ZEND_FETCH_DIM_R";
        case ZEND_FETCH_OBJ_R: return "ZEND_FETCH_OBJ_R";
        case ZEND_FETCH_W: return "ZEND_FETCH_W";
        case ZEND_FETCH_DIM_W: return "ZEND_FETCH_DIM_W";
        case ZEND_FETCH_OBJ_W: return "ZEND_FETCH_OBJ_W";
        case ZEND_FETCH_RW: return "ZEND_FETCH_RW";
        case ZEND_FETCH_DIM_RW: return "ZEND_FETCH_DIM_RW";
        case ZEND_FETCH_OBJ_RW: return "ZEND_FETCH_OBJ_RW";
        case ZEND_FETCH_IS: return "ZEND_FETCH_IS";
        case ZEND_FETCH_DIM_IS: return "ZEND_FETCH_DIM_IS";
        case ZEND_FETCH_OBJ_IS: return "ZEND_FETCH_OBJ_IS";
        case ZEND_FETCH_FUNC_ARG: return "ZEND_FETCH_FUNC_ARG";
        case ZEND_FETCH_DIM_FUNC_ARG: return "ZEND_FETCH_DIM_FUNC_ARG";
        case ZEND_FETCH_OBJ_FUNC_ARG: return "ZEND_FETCH_OBJ_FUNC_ARG";
        case ZEND_FETCH_UNSET: return "ZEND_FETCH_UNSET";
        case ZEND_FETCH_DIM_UNSET: return "ZEND_FETCH_DIM_UNSET";
        case ZEND_FETCH_OBJ_UNSET: return "ZEND_FETCH_OBJ_UNSET";
        case ZEND_FETCH_DIM_TMP_VAR: return "ZEND_FETCH_DIM_TMP_VAR";
        case ZEND_FETCH_CONSTANT: return "ZEND_FETCH_CONSTANT";
        case ZEND_GOTO: return "ZEND_GOTO";
        case ZEND_EXT_STMT: return "ZEND_EXT_STMT";
        case ZEND_EXT_FCALL_BEGIN: return "ZEND_EXT_FCALL_BEGIN";
        case ZEND_EXT_FCALL_END: return "ZEND_EXT_FCALL_END";
        case ZEND_EXT_NOP: return "ZEND_EXT_NOP";
        case ZEND_TICKS: return "ZEND_TICKS";
        case ZEND_SEND_VAR_NO_REF: return "ZEND_SEND_VAR_NO_REF";
        case ZEND_CATCH: return "ZEND_CATCH";
        case ZEND_THROW: return "ZEND_THROW";
        case ZEND_FETCH_CLASS: return "ZEND_FETCH_CLASS";
        case ZEND_CLONE: return "ZEND_CLONE";
        case ZEND_RETURN_BY_REF: return "ZEND_RETURN_BY_REF";
        case ZEND_INIT_METHOD_CALL: return "ZEND_INIT_METHOD_CALL";
        case ZEND_INIT_STATIC_METHOD_CALL: return "ZEND_INIT_STATIC_METHOD_CALL";
        case ZEND_ISSET_ISEMPTY_VAR: return "ZEND_ISSET_ISEMPTY_VAR";
        case ZEND_ISSET_ISEMPTY_DIM_OBJ: return "ZEND_ISSET_ISEMPTY_DIM_OBJ";
        case ZEND_PRE_INC_OBJ: return "ZEND_PRE_INC_OBJ";
        case ZEND_PRE_DEC_OBJ: return "ZEND_PRE_DEC_OBJ";
        case ZEND_POST_INC_OBJ: return "ZEND_POST_INC_OBJ";
        case ZEND_POST_DEC_OBJ: return "ZEND_POST_DEC_OBJ";
        case ZEND_ASSIGN_OBJ: return "ZEND_ASSIGN_OBJ";
        case ZEND_INSTANCEOF: return "ZEND_INSTANCEOF";
        case ZEND_DECLARE_CLASS: return "ZEND_DECLARE_CLASS";
        case ZEND_DECLARE_INHERITED_CLASS: return "ZEND_DECLARE_INHERITED_CLASS";
        case ZEND_DECLARE_FUNCTION: return "ZEND_DECLARE_FUNCTION";
        case ZEND_RAISE_ABSTRACT_ERROR: return "ZEND_RAISE_ABSTRACT_ERROR";
        case ZEND_DECLARE_CONST: return "ZEND_DECLARE_CONST";
        case ZEND_ADD_INTERFACE: return "ZEND_ADD_INTERFACE";
        case ZEND_DECLARE_INHERITED_CLASS_DELAYED: return "ZEND_DECLARE_INHERITED_CLASS_DELAYED";
        case ZEND_VERIFY_ABSTRACT_CLASS: return "ZEND_VERIFY_ABSTRACT_CLASS";
        case ZEND_ASSIGN_DIM: return "ZEND_ASSIGN_DIM";
        case ZEND_ISSET_ISEMPTY_PROP_OBJ: return "ZEND_ISSET_ISEMPTY_PROP_OBJ";
        case ZEND_HANDLE_EXCEPTION: return "ZEND_HANDLE_EXCEPTION";
        case ZEND_USER_OPCODE: return "ZEND_USER_OPCODE";
#ifdef ZEND_JMP_SET
        case ZEND_JMP_SET: return "ZEND_JMP_SET";
#endif
        case ZEND_DECLARE_LAMBDA_FUNCTION: return "ZEND_DECLARE_LAMBDA_FUNCTION";
#ifdef ZEND_ADD_TRAIT
        case ZEND_ADD_TRAIT: return "ZEND_ADD_TRAIT";
#endif
#ifdef ZEND_BIND_TRAITS
        case ZEND_BIND_TRAITS: return "ZEND_BIND_TRAITS";
#endif
#ifdef ZEND_SEPARATE
        case ZEND_SEPARATE: return "ZEND_SEPARATE";
#endif
#ifdef ZEND_QM_ASSIGN_VAR
        case ZEND_QM_ASSIGN_VAR: return "ZEND_QM_ASSIGN_VAR";
#endif
#ifdef ZEND_JMP_SET_VAR
        case ZEND_JMP_SET_VAR: return "ZEND_JMP_SET_VAR";
#endif
        case ZEND_DISCARD_EXCEPTION: return "ZEND_DISCARD_EXCEPTION";
        case ZEND_YIELD: return "ZEND_YIELD";
        case ZEND_GENERATOR_RETURN: return "ZEND_GENERATOR_RETURN";
#ifdef ZEND_FAST_CALL
        case ZEND_FAST_CALL: return "ZEND_FAST_CALL";
#endif
#ifdef ZEND_FAST_RET
        case ZEND_FAST_RET: return "ZEND_FAST_RET";
#endif
#ifdef ZEND_RECV_VARIADIC
        case ZEND_RECV_VARIADIC: return "ZEND_RECV_VARIADIC";
#endif
        case ZEND_OP_DATA: return "ZEND_OP_DATA";
        
        default: return "UNKNOWN";
    }
} /* }}} */

static void phpdbg_print_opline(zend_execute_data *execute_data TSRMLS_DC) { /* {{{ */
    zend_op *opline = execute_data->opline;

    printf(
        "[OPLINE: %p:%s]\n", opline, phpdbg_decode_opcode(opline->opcode TSRMLS_CC));
} /* }}} */

void phpdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	zend_bool original_in_execution = EG(in_execution);

	EG(in_execution) = 1;

	if (0) {
zend_vm_enter:
		execute_data = zend_create_execute_data_from_op_array(EG(active_op_array), 1 TSRMLS_CC);
	}

	while (1) {
#ifdef ZEND_WIN32
		if (EG(timed_out)) {
			zend_timeout(0);
		}
#endif

        phpdbg_print_opline(execute_data TSRMLS_CC);

        if (PHPDBG_G(has_file_bp)
			&& phpdbg_breakpoint_file(execute_data->op_array TSRMLS_CC) == SUCCESS) {
			while (phpdbg_interactive(0, NULL TSRMLS_CC) != PHPDBG_NEXT) {
				continue;
			}
		}

		if (PHPDBG_G(has_sym_bp)
			&& (execute_data->opline->opcode == ZEND_DO_FCALL || execute_data->opline->opcode == ZEND_DO_FCALL_BY_NAME)
			&& phpdbg_breakpoint_symbol(execute_data->function_state.function TSRMLS_CC) == SUCCESS) {
			while (phpdbg_interactive(0, NULL TSRMLS_CC) != PHPDBG_NEXT) {
				continue;
			}
		}

		PHPDBG_G(vmret) = execute_data->opline->handler(execute_data TSRMLS_CC);

		if (PHPDBG_G(stepping)) {
			while (phpdbg_interactive(
				0, NULL TSRMLS_CC) != PHPDBG_NEXT) {
				continue;
			}
		}

		if (PHPDBG_G(vmret) > 0) {
			switch (PHPDBG_G(vmret)) {
				case 1:
					EG(in_execution) = original_in_execution;
					return;
				case 2:
					goto zend_vm_enter;
					break;
				case 3:
					execute_data = EG(current_execute_data);
					break;
				default:
					break;
			}
		}

	}
	zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
} /* }}} */
