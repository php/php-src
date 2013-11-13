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

#include "zend_vm_opcodes.h"
#include "zend_compile.h"
#include "phpdbg_opcode.h"

const char *phpdbg_decode_opcode(zend_uchar opcode) /* {{{ */
{
#define CASE(s) case s: return #s
	switch (opcode) {
		CASE(ZEND_NOP);
		CASE(ZEND_ADD);
		CASE(ZEND_SUB);
		CASE(ZEND_MUL);
		CASE(ZEND_DIV);
		CASE(ZEND_MOD);
		CASE(ZEND_SL);
		CASE(ZEND_SR);
		CASE(ZEND_CONCAT);
		CASE(ZEND_BW_OR);
		CASE(ZEND_BW_AND);
		CASE(ZEND_BW_XOR);
		CASE(ZEND_BW_NOT);
		CASE(ZEND_BOOL_NOT);
		CASE(ZEND_BOOL_XOR);
		CASE(ZEND_IS_IDENTICAL);
		CASE(ZEND_IS_NOT_IDENTICAL);
		CASE(ZEND_IS_EQUAL);
		CASE(ZEND_IS_NOT_EQUAL);
		CASE(ZEND_IS_SMALLER);
		CASE(ZEND_IS_SMALLER_OR_EQUAL);
		CASE(ZEND_CAST);
		CASE(ZEND_QM_ASSIGN);
		CASE(ZEND_ASSIGN_ADD);
		CASE(ZEND_ASSIGN_SUB);
		CASE(ZEND_ASSIGN_MUL);
		CASE(ZEND_ASSIGN_DIV);
		CASE(ZEND_ASSIGN_MOD);
		CASE(ZEND_ASSIGN_SL);
		CASE(ZEND_ASSIGN_SR);
		CASE(ZEND_ASSIGN_CONCAT);
		CASE(ZEND_ASSIGN_BW_OR);
		CASE(ZEND_ASSIGN_BW_AND);
		CASE(ZEND_ASSIGN_BW_XOR);
		CASE(ZEND_PRE_INC);
		CASE(ZEND_PRE_DEC);
		CASE(ZEND_POST_INC);
		CASE(ZEND_POST_DEC);
		CASE(ZEND_ASSIGN);
		CASE(ZEND_ASSIGN_REF);
		CASE(ZEND_ECHO);
		CASE(ZEND_PRINT);
		CASE(ZEND_JMP);
		CASE(ZEND_JMPZ);
		CASE(ZEND_JMPNZ);
		CASE(ZEND_JMPZNZ);
		CASE(ZEND_JMPZ_EX);
		CASE(ZEND_JMPNZ_EX);
		CASE(ZEND_CASE);
		CASE(ZEND_SWITCH_FREE);
		CASE(ZEND_BRK);
		CASE(ZEND_CONT);
		CASE(ZEND_BOOL);
		CASE(ZEND_INIT_STRING);
		CASE(ZEND_ADD_CHAR);
		CASE(ZEND_ADD_STRING);
		CASE(ZEND_ADD_VAR);
		CASE(ZEND_BEGIN_SILENCE);
		CASE(ZEND_END_SILENCE);
		CASE(ZEND_INIT_FCALL_BY_NAME);
		CASE(ZEND_DO_FCALL);
		CASE(ZEND_DO_FCALL_BY_NAME);
		CASE(ZEND_RETURN);
		CASE(ZEND_RECV);
		CASE(ZEND_RECV_INIT);
		CASE(ZEND_SEND_VAL);
		CASE(ZEND_SEND_VAR);
		CASE(ZEND_SEND_REF);
		CASE(ZEND_NEW);
		CASE(ZEND_INIT_NS_FCALL_BY_NAME);
		CASE(ZEND_FREE);
		CASE(ZEND_INIT_ARRAY);
		CASE(ZEND_ADD_ARRAY_ELEMENT);
		CASE(ZEND_INCLUDE_OR_EVAL);
		CASE(ZEND_UNSET_VAR);
		CASE(ZEND_UNSET_DIM);
		CASE(ZEND_UNSET_OBJ);
		CASE(ZEND_FE_RESET);
		CASE(ZEND_FE_FETCH);
		CASE(ZEND_EXIT);
		CASE(ZEND_FETCH_R);
		CASE(ZEND_FETCH_DIM_R);
		CASE(ZEND_FETCH_OBJ_R);
		CASE(ZEND_FETCH_W);
		CASE(ZEND_FETCH_DIM_W);
		CASE(ZEND_FETCH_OBJ_W);
		CASE(ZEND_FETCH_RW);
		CASE(ZEND_FETCH_DIM_RW);
		CASE(ZEND_FETCH_OBJ_RW);
		CASE(ZEND_FETCH_IS);
		CASE(ZEND_FETCH_DIM_IS);
		CASE(ZEND_FETCH_OBJ_IS);
		CASE(ZEND_FETCH_FUNC_ARG);
		CASE(ZEND_FETCH_DIM_FUNC_ARG);
		CASE(ZEND_FETCH_OBJ_FUNC_ARG);
		CASE(ZEND_FETCH_UNSET);
		CASE(ZEND_FETCH_DIM_UNSET);
		CASE(ZEND_FETCH_OBJ_UNSET);
		CASE(ZEND_FETCH_DIM_TMP_VAR);
		CASE(ZEND_FETCH_CONSTANT);
		CASE(ZEND_GOTO);
		CASE(ZEND_EXT_STMT);
		CASE(ZEND_EXT_FCALL_BEGIN);
		CASE(ZEND_EXT_FCALL_END);
		CASE(ZEND_EXT_NOP);
		CASE(ZEND_TICKS);
		CASE(ZEND_SEND_VAR_NO_REF);
		CASE(ZEND_CATCH);
		CASE(ZEND_THROW);
		CASE(ZEND_FETCH_CLASS);
		CASE(ZEND_CLONE);
		CASE(ZEND_RETURN_BY_REF);
		CASE(ZEND_INIT_METHOD_CALL);
		CASE(ZEND_INIT_STATIC_METHOD_CALL);
		CASE(ZEND_ISSET_ISEMPTY_VAR);
		CASE(ZEND_ISSET_ISEMPTY_DIM_OBJ);
		CASE(ZEND_PRE_INC_OBJ);
		CASE(ZEND_PRE_DEC_OBJ);
		CASE(ZEND_POST_INC_OBJ);
		CASE(ZEND_POST_DEC_OBJ);
		CASE(ZEND_ASSIGN_OBJ);
		CASE(ZEND_INSTANCEOF);
		CASE(ZEND_DECLARE_CLASS);
		CASE(ZEND_DECLARE_INHERITED_CLASS);
		CASE(ZEND_DECLARE_FUNCTION);
		CASE(ZEND_RAISE_ABSTRACT_ERROR);
		CASE(ZEND_DECLARE_CONST);
		CASE(ZEND_ADD_INTERFACE);
		CASE(ZEND_DECLARE_INHERITED_CLASS_DELAYED);
		CASE(ZEND_VERIFY_ABSTRACT_CLASS);
		CASE(ZEND_ASSIGN_DIM);
		CASE(ZEND_ISSET_ISEMPTY_PROP_OBJ);
		CASE(ZEND_HANDLE_EXCEPTION);
		CASE(ZEND_USER_OPCODE);
#ifdef ZEND_JMP_SET
		CASE(ZEND_JMP_SET);
#endif
		CASE(ZEND_DECLARE_LAMBDA_FUNCTION);
#ifdef ZEND_ADD_TRAIT
		CASE(ZEND_ADD_TRAIT);
#endif
#ifdef ZEND_BIND_TRAITS
		CASE(ZEND_BIND_TRAITS);
#endif
#ifdef ZEND_SEPARATE
		CASE(ZEND_SEPARATE);
#endif
#ifdef ZEND_QM_ASSIGN_VAR
		CASE(ZEND_QM_ASSIGN_VAR);
#endif
#ifdef ZEND_JMP_SET_VAR
		CASE(ZEND_JMP_SET_VAR);
#endif
		CASE(ZEND_DISCARD_EXCEPTION);
		CASE(ZEND_YIELD);
		CASE(ZEND_GENERATOR_RETURN);
#ifdef ZEND_FAST_CALL
		CASE(ZEND_FAST_CALL);
#endif
#ifdef ZEND_FAST_RET
		CASE(ZEND_FAST_RET);
#endif
#ifdef ZEND_RECV_VARIADIC
		CASE(ZEND_RECV_VARIADIC);
#endif
		CASE(ZEND_OP_DATA);
		default: 
		    return "UNKNOWN";
	}
} /* }}} */
