/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_VM_H
#define ZEND_VM_H

#define ZEND_VM_HAVE_OLD_EXECUTOR

#define ZEND_VM_KIND_CALL   1
#define ZEND_VM_KIND_SWITCH 2
#define ZEND_VM_KIND_GOTO   3

#ifndef ZEND_VM_OLD_EXECUTOR
/*#  define ZEND_VM_KIND ZEND_VM_KIND_CALL */
#  define ZEND_VM_SPEC
#endif

/* don't edit the rest of the file */

#ifdef ZEND_VM_HAVE_OLD_EXECUTOR
ZEND_API void zend_vm_use_old_executor();
void zend_vm_set_opcode_handler(zend_op* opcode);
#endif

#define _CONST_CODE  0
#define _TMP_CODE    1
#define _VAR_CODE    2
#define _UNUSED_CODE 3

#ifndef ZEND_VM_KIND
#  ifdef __GNUC__
#    define ZEND_VM_KIND           ZEND_VM_KIND_GOTO
#  else
#    define ZEND_VM_KIND           ZEND_VM_KIND_CALL
#  endif
#endif

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(ZEND_VM_OLD_EXECUTOR)
#  define ZEND_VM_ALWAYS_INLINE  __attribute__ ((always_inline))
void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((alias("zend_error"),noreturn));
/*extern void zend_error_noreturn(int type, const char *format, ...) __asm__("zend_error") __attribute__ ((noreturn));*/
#else
#  define ZEND_VM_ALWAYS_INLINE
#  define zend_error_noreturn zend_error
#endif

#ifndef ZEND_VM_SPEC
#  define ZEND_VM_CODE(opcode, op1, op2) opcode
#  define ZEND_VM_SPEC_OPCODE(opcode, op1, op2) opcode
#  ifdef ZEND_VM_HAVE_OLD_EXECUTOR
#    define ZEND_VM_SET_OPCODE_HANDLER(opline) \
	     zend_vm_set_opcode_handler(opline)
#  else
#    define ZEND_VM_SET_OPCODE_HANDLER(opline) \
	     opline->handler = zend_opcode_handlers[opline->opcode]
#  endif
#else
static const int zend_vm_decode[] = {
	_UNUSED_CODE, /* 0              */
	_CONST_CODE,  /* 1 = IS_CONST   */
	_TMP_CODE,    /* 2 = IS_TMP_VAR */
	_UNUSED_CODE, /* 3              */
	_VAR_CODE,    /* 4 = IS_VAR     */
	_UNUSED_CODE, /* 5              */
	_UNUSED_CODE, /* 6              */
	_UNUSED_CODE, /* 7              */
	_UNUSED_CODE  /* 8 = IS_UNUSED  */
};

#  define ZEND_VM_CODE(opcode, op1, op2) \
     opcode * 16 + op1 * 4 + op2
#  define ZEND_VM_SPEC_OPCODE(opcode, op1, op2) \
     ZEND_VM_CODE(opcode, zend_vm_decode[op1], zend_vm_decode[op2])
#  ifdef ZEND_VM_HAVE_OLD_EXECUTOR
#    define ZEND_VM_SET_OPCODE_HANDLER(opline) \
	     zend_vm_set_opcode_handler(opline)
#  else
#    define ZEND_VM_SET_OPCODE_HANDLER(opline) \
	     opline->handler = zend_opcode_handlers[ZEND_VM_SPEC_OPCODE(opline->opcode, opline->op1.op_type, opline->op2.op_type)]
#  endif
#endif


#if ZEND_VM_KIND == ZEND_VM_KIND_CALL

#  define EXECUTE_DATA execute_data

#  define ZEND_VM_HELPER_VAR(X)

#  define ZEND_VM_DISPATCH() \
     if (EX(opline)->handler(&execute_data TSRMLS_CC) > 0)

#  define ZEND_VM_CONTINUE_LABEL

#  define ZEND_VM_HANDLER(OP) \
     static int OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_HANDLER_EX(OP) \
     static int OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_HELPER(NAME) \
     static int NAME(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_HELPER_EX(NAME,PARAM) \
     static int NAME(PARAM, ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_SPEC_HANDLER(OP, CODE) \
     static int OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_SPEC_HANDLER_EX(OP, CODE) \
     static int OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_SPEC_HELPER(NAME) \
     static int NAME(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_SPEC_HELPER_EX(NAME,PARAM) \
     static int NAME(PARAM, ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_NULL_HANDLER() \
     int ZEND_VM_NULL(ZEND_OPCODE_HANDLER_ARGS)

#  define ZEND_VM_DISPATCH_TO_HANDLER(OP) \
     return OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_DISPATCH_TO_HELPER(NAME) \
     return NAME(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     return NAME(VAL, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_SPEC_DISPATCH_TO_HANDLER(OP) \
     return OP##_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER(NAME) \
     return NAME(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     return NAME(VAL, ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)

#  define ZEND_VM_CONTINUE() \
     return 0

#  define ZEND_VM_NEXT_OPCODE() \
     CHECK_SYMBOL_TABLES() \
     EX(opline)++; \
     return 0

#  define ZEND_VM_SET_OPCODE(new_op) \
     CHECK_SYMBOL_TABLES() \
     EX(opline) = new_op

#  define ZEND_VM_INC_OPCODE() \
     if (!EG(exception)) { \
       CHECK_SYMBOL_TABLES() \
       EX(opline)++; \
     }

#  define ZEND_VM_RETURN_FROM_EXECUTE_LOOP() \
     if (EX(op_array)->T < TEMP_VAR_STACK_LIMIT) { \
       free_alloca(EX(Ts)); \
     } else { \
       efree(EX(Ts)); \
     } \
     EG(in_execution) = EX(original_in_execution); \
     EG(current_execute_data) = EX(prev_execute_data); \
     return 1;

#  define ZEND_VM_LABEL(OP)            OP##_HANDLER
#  define ZEND_VM_NULL_LABEL           ZEND_VM_NULL
#  define ZEND_VM_SPEC_LABEL(OP,CODE)  OP##_HANDLER
#  define ZEND_VM_SPEC_NULL_LABEL      ZEND_VM_NULL

#elif ZEND_VM_KIND == ZEND_VM_KIND_SWITCH

#  define EXECUTE_DATA &execute_data

#  define ZEND_VM_HELPER_VAR(X) X;

#  define ZEND_VM_DISPATCH() \
     switch ((int)EX(opline)->handler)

#  define ZEND_VM_CONTINUE_LABEL \
     zend_vm_continue:

#  define ZEND_VM_HANDLER(OP) \
     case OP:

#  define ZEND_VM_HANDLER_EX(OP) \
     case OP: \
     OP##_LABEL:

#  define ZEND_VM_HELPER(NAME) \
     NAME:

#  define ZEND_VM_HELPER_EX(NAME,PARAM) \
     NAME:

#  define ZEND_VM_SPEC_HANDLER(OP, CODE) \
     case CODE:

#  define ZEND_VM_SPEC_HANDLER_EX(OP, CODE) \
     case CODE: \
     OP##_LABEL:

#  define ZEND_VM_SPEC_HELPER(NAME) \
     NAME:

#  define ZEND_VM_SPEC_HELPER_EX(NAME,PARAM) \
     NAME:

#  define ZEND_VM_NULL_HANDLER() \
     default:

#  define ZEND_VM_DISPATCH_TO_HANDLER(OP) \
     goto OP##_LABEL

#  define ZEND_VM_DISPATCH_TO_HELPER(NAME) \
     goto NAME

#  define ZEND_VM_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     VAR = VAL; \
     goto NAME

#  define ZEND_VM_SPEC_DISPATCH_TO_HANDLER(OP) \
     goto OP##_LABEL

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER(NAME) \
     goto NAME

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     VAR = VAL; \
     goto NAME

#  define ZEND_VM_CONTINUE() \
     goto zend_vm_continue

#  define ZEND_VM_NEXT_OPCODE() \
     CHECK_SYMBOL_TABLES() \
     EX(opline)++; \
     ZEND_VM_CONTINUE()

#  define ZEND_VM_SET_OPCODE(new_op) \
     CHECK_SYMBOL_TABLES() \
     EX(opline) = new_op

#  define ZEND_VM_INC_OPCODE() \
     if (!EG(exception)) { \
       CHECK_SYMBOL_TABLES() \
       EX(opline)++; \
     }

#  define ZEND_VM_RETURN_FROM_EXECUTE_LOOP() \
     if (EX(op_array)->T < TEMP_VAR_STACK_LIMIT) { \
       free_alloca(EX(Ts)); \
     } else { \
       efree(EX(Ts)); \
     } \
     EG(in_execution) = EX(original_in_execution); \
     EG(current_execute_data) = EX(prev_execute_data); \
     return;

#  define ZEND_VM_LABEL(OP)            (opcode_handler_t)OP
#  define ZEND_VM_NULL_LABEL           (opcode_handler_t)-1
#  define ZEND_VM_SPEC_LABEL(OP,CODE)  (opcode_handler_t)(CODE)
#  define ZEND_VM_SPEC_NULL_LABEL      (opcode_handler_t)-1

#elif ZEND_VM_KIND == ZEND_VM_KIND_GOTO

#  define EXECUTE_DATA &execute_data

#  define ZEND_VM_HELPER_VAR(X) X;

#  define ZEND_VM_DISPATCH() \
     goto *(void**)(EX(opline)->handler);

#  define ZEND_VM_CONTINUE_LABEL

#  define ZEND_VM_HANDLER(OP) \
     OP##_HANDLER:

#  define ZEND_VM_HANDLER_EX(OP) \
     OP##_HANDLER:

#  define ZEND_VM_HELPER(NAME) \
     NAME:

#  define ZEND_VM_HELPER_EX(NAME,PARAM) \
     NAME:

#  define ZEND_VM_SPEC_HANDLER(OP, CODE) \
     OP##_HANDLER:

#  define ZEND_VM_SPEC_HANDLER_EX(OP, CODE) \
     OP##_HANDLER:

#  define ZEND_VM_SPEC_HELPER(NAME) \
     NAME:

#  define ZEND_VM_SPEC_HELPER_EX(NAME,PARAM) \
     NAME:

#  define ZEND_VM_NULL_HANDLER() \
     ZEND_VM_NULL:

#  define ZEND_VM_DISPATCH_TO_HANDLER(OP) \
     goto OP##_HANDLER

#  define ZEND_VM_DISPATCH_TO_HELPER(NAME) \
     goto NAME

#  define ZEND_VM_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     VAR = VAL; \
     goto NAME

#  define ZEND_VM_SPEC_DISPATCH_TO_HANDLER(OP) \
     goto OP##_HANDLER

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER(NAME) \
     goto NAME

#  define ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX(NAME,VAR,VAL) \
     VAR = VAL; \
     goto NAME

#  define ZEND_VM_CONTINUE() \
     goto *(void**)(EX(opline)->handler)

#  define ZEND_VM_NEXT_OPCODE() \
     CHECK_SYMBOL_TABLES() \
     EX(opline)++; \
     ZEND_VM_CONTINUE()

#  define ZEND_VM_SET_OPCODE(new_op) \
     CHECK_SYMBOL_TABLES() \
     EX(opline) = new_op

#  define ZEND_VM_INC_OPCODE() \
     if (!EG(exception)) { \
       CHECK_SYMBOL_TABLES() \
       EX(opline)++; \
     }

#  define ZEND_VM_RETURN_FROM_EXECUTE_LOOP() \
     if (EX(op_array)->T < TEMP_VAR_STACK_LIMIT) { \
       free_alloca(EX(Ts)); \
     } else { \
       efree(EX(Ts)); \
     } \
     EG(in_execution) = EX(original_in_execution); \
     EG(current_execute_data) = EX(prev_execute_data); \
     return;

#  define ZEND_VM_LABEL(OP)            (opcode_handler_t)&&OP##_HANDLER
#  define ZEND_VM_NULL_LABEL           &&ZEND_VM_NULL
#  define ZEND_VM_SPEC_LABEL(OP,CODE)  (opcode_handler_t)&&OP##_HANDLER
#  define ZEND_VM_SPEC_NULL_LABEL      &&ZEND_VM_NULL

#else
#  error "Unknown ZEND_VM_KIND"
#endif

#define ZEND_VM_CONTINUE_JMP() ZEND_VM_CONTINUE()

#endif
