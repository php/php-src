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

/*
 Don't edit this file.
 It produces the specialized version of executor's opcode handlers and helpers,
 those are defined in zend_vm_handlers.h
*/

#define IS_ANY         (1<<7)
#define _ANY_CODE      0

#define SPEC2(X,Y)     SPEC_##X##Y
#define SPEC(X,Y)      SPEC2(X,Y)

#define SPEC2_0(X,Y)   SPEC_##X##Y()
#define SPEC_0(X,Y)    SPEC2_0(X,Y)

#define SPEC2_1(X,Y,Z) SPEC_##X##Y(Z)
#define SPEC_1(X,Y,Z)  SPEC2_1(X,Y,Z)

#define OP1_TYPE \
  SPEC(OP1_TYPE, OP1_TYPE_PREFIX)
#define OP1_TYPE_CODE \
	SPEC(OP1_TYPE_PREFIX, _CODE)
#define GET_OP1_ZVAL_PTR(T) \
  SPEC_1(GET_OP1_ZVAL_PTR, OP1_TYPE_PREFIX, T)
#define GET_OP1_ZVAL_PTR_PTR(T) \
  SPEC_1(GET_OP1_ZVAL_PTR_PTR, OP1_TYPE_PREFIX, T)
#define GET_OP1_OBJ_ZVAL_PTR(T) \
  SPEC_1(GET_OP1_OBJ_ZVAL_PTR, OP1_TYPE_PREFIX, T)
#define GET_OP1_OBJ_ZVAL_PTR_PTR(T) \
  SPEC_1(GET_OP1_OBJ_ZVAL_PTR_PTR, OP1_TYPE_PREFIX, T)
#define IS_OP1_TMP_FREE() \
  SPEC_0(IS_OP1_TMP_FREE, OP1_TYPE_PREFIX)
#define FREE_OP1() \
  SPEC_0(FREE_OP1, OP1_TYPE_PREFIX)
#define FREE_OP1_IF_VAR() \
  SPEC_0(FREE_OP1_IF_VAR, OP1_TYPE_PREFIX)
#define FREE_OP1_VAR_PTR() \
  SPEC_0(FREE_OP1_VAR_PTR, OP1_TYPE_PREFIX)

#define OP2_TYPE \
  SPEC(OP2_TYPE, OP2_TYPE_PREFIX)
#define OP2_TYPE_CODE \
	SPEC(OP2_TYPE_PREFIX, _CODE)
#define GET_OP2_ZVAL_PTR(T) \
  SPEC_1(GET_OP2_ZVAL_PTR, OP2_TYPE_PREFIX, T)
#define GET_OP2_ZVAL_PTR_PTR(T) \
  SPEC_1(GET_OP2_ZVAL_PTR_PTR, OP2_TYPE_PREFIX, T)
#define GET_OP2_OBJ_ZVAL_PTR(T) \
  SPEC_1(GET_OP2_OBJ_ZVAL_PTR, OP2_TYPE_PREFIX, T)
#define GET_OP2_OBJ_ZVAL_PTR_PTR(T) \
  SPEC_1(GET_OP2_OBJ_ZVAL_PTR_PTR, OP2_TYPE_PREFIX, T)
#define IS_OP2_TMP_FREE() \
  SPEC_0(IS_OP2_TMP_FREE, OP2_TYPE_PREFIX)
#define FREE_OP2() \
  SPEC_0(FREE_OP2, OP2_TYPE_PREFIX)
#define FREE_OP2_IF_VAR() \
  SPEC_0(FREE_OP2_IF_VAR, OP2_TYPE_PREFIX)
#define FREE_OP2_VAR_PTR() \
  SPEC_0(FREE_OP2_VAR_PTR, OP2_TYPE_PREFIX)

#define SPEC__ANY_CODE                          _ANY_CODE
#define SPEC__CONST_CODE                        _CONST_CODE
#define SPEC__TMP_CODE                          _TMP_CODE
#define SPEC__VAR_CODE                          _VAR_CODE
#define SPEC__UNUSED_CODE                       _UNUSED_CODE

#define SPEC_OP1_TYPE_ANY                       opline->op1.op_type
#define SPEC_OP1_TYPE_CONST                     IS_CONST
#define SPEC_OP1_TYPE_TMP                       IS_TMP_VAR
#define SPEC_OP1_TYPE_VAR                       IS_VAR
#define SPEC_OP1_TYPE_UNUSED                    IS_UNUSED

#define SPEC_GET_OP1_ZVAL_PTR_ANY(T)            _get_zval_ptr(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_CONST(T)          _get_zval_ptr_const(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_TMP(T)            _get_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_VAR(T)            _get_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_UNUSED(T)         _get_zval_ptr_unused(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)

#define SPEC_GET_OP1_ZVAL_PTR_PTR_ANY(T)        _get_zval_ptr_ptr(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_PTR_CONST(T)      _get_zval_ptr_ptr_const(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_PTR_TMP(T)        _get_zval_ptr_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_PTR_VAR(T)        _get_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_ZVAL_PTR_PTR_UNUSED(T)     _get_zval_ptr_ptr_unused(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)

#define SPEC_GET_OP1_OBJ_ZVAL_PTR_ANY(T)        _get_obj_zval_ptr(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_CONST(T)      _get_obj_zval_ptr_const(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_TMP(T)        _get_obj_zval_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_VAR(T)        _get_obj_zval_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_UNUSED(T)     _get_obj_zval_ptr_unused(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)

#define SPEC_GET_OP1_OBJ_ZVAL_PTR_PTR_ANY(T)    _get_obj_zval_ptr_ptr(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_PTR_CONST(T)  _get_obj_zval_ptr_ptr_const(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_PTR_TMP(T)    _get_obj_zval_ptr_ptr_tmp(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_PTR_VAR(T)    _get_obj_zval_ptr_ptr_var(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)
#define SPEC_GET_OP1_OBJ_ZVAL_PTR_PTR_UNUSED(T) _get_obj_zval_ptr_ptr_unused(&opline->op1, EX(Ts), &free_op1 TSRMLS_CC)

#define SPEC_IS_OP1_TMP_FREE_ANY()              IS_TMP_FREE(free_op1)
#define SPEC_IS_OP1_TMP_FREE_CONST()            0
#define SPEC_IS_OP1_TMP_FREE_TMP()              1
#define SPEC_IS_OP1_TMP_FREE_VAR()              0
#define SPEC_IS_OP1_TMP_FREE_UNUSED()           0

#define SPEC_FREE_OP1_ANY()                     FREE_OP(free_op1);
#define SPEC_FREE_OP1_CONST()
#define SPEC_FREE_OP1_TMP()                     zval_dtor(free_op1.var)
#define SPEC_FREE_OP1_VAR()                     FREE_OP_VAR_PTR(free_op1);
#define SPEC_FREE_OP1_UNUSED()

#define SPEC_FREE_OP1_IF_VAR_ANY()              FREE_OP_IF_VAR(free_op1);
#define SPEC_FREE_OP1_IF_VAR_CONST()
#define SPEC_FREE_OP1_IF_VAR_TMP()
#define SPEC_FREE_OP1_IF_VAR_VAR()              FREE_OP_VAR_PTR(free_op1);
#define SPEC_FREE_OP1_IF_VAR_UNUSED()

#define SPEC_FREE_OP1_VAR_PTR_ANY()             FREE_OP_VAR_PTR(free_op1);
#define SPEC_FREE_OP1_VAR_PTR_CONST()
#define SPEC_FREE_OP1_VAR_PTR_TMP()
#define SPEC_FREE_OP1_VAR_PTR_VAR()             FREE_OP_VAR_PTR(free_op1);
#define SPEC_FREE_OP1_VAR_PTR_UNUSED()

#define SPEC_OP2_TYPE_ANY                       opline->op2.op_type
#define SPEC_OP2_TYPE_CONST                     IS_CONST
#define SPEC_OP2_TYPE_TMP                       IS_TMP_VAR
#define SPEC_OP2_TYPE_VAR                       IS_VAR
#define SPEC_OP2_TYPE_UNUSED                    IS_UNUSED

#define SPEC_GET_OP2_ZVAL_PTR_ANY(T)            _get_zval_ptr(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_CONST(T)          _get_zval_ptr_const(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_TMP(T)            _get_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_VAR(T)            _get_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_UNUSED(T)         _get_zval_ptr_unused(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)

#define SPEC_GET_OP2_ZVAL_PTR_PTR_ANY(T)        _get_zval_ptr_ptr(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_PTR_CONST(T)      _get_zval_ptr_ptr_const(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_PTR_TMP(T)        _get_zval_ptr_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_PTR_VAR(T)        _get_zval_ptr_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_ZVAL_PTR_PTR_UNUSED(T)     _get_zval_ptr_ptr_unused(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)

#define SPEC_GET_OP2_OBJ_ZVAL_PTR_ANY(T)        _get_obj_zval_ptr(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_CONST(T)      _get_obj_zval_ptr_const(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_TMP(T)        _get_obj_zval_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_VAR(T)        _get_obj_zval_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_UNUSED(T)     _get_obj_zval_ptr_unused(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)

#define SPEC_GET_OP2_OBJ_ZVAL_PTR_PTR_ANY(T)    _get_obj_zval_ptr_ptr(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_PTR_CONST(T)  _get_obj_zval_ptr_ptr_const(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_PTR_TMP(T)    _get_obj_zval_ptr_ptr_tmp(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_PTR_VAR(T)    _get_obj_zval_ptr_ptr_var(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)
#define SPEC_GET_OP2_OBJ_ZVAL_PTR_PTR_UNUSED(T) _get_obj_zval_ptr_ptr_unused(&opline->op2, EX(Ts), &free_op2 TSRMLS_CC)

#define SPEC_IS_OP2_TMP_FREE_ANY()              IS_TMP_FREE(free_op2)
#define SPEC_IS_OP2_TMP_FREE_CONST()            0
#define SPEC_IS_OP2_TMP_FREE_TMP()              1
#define SPEC_IS_OP2_TMP_FREE_VAR()              0
#define SPEC_IS_OP2_TMP_FREE_UNUSED()           0

#define SPEC_FREE_OP2_ANY()                     FREE_OP(free_op2);
#define SPEC_FREE_OP2_CONST()
#define SPEC_FREE_OP2_TMP()                     zval_dtor(free_op2.var)
#define SPEC_FREE_OP2_VAR()                     FREE_OP_VAR_PTR(free_op2);
#define SPEC_FREE_OP2_UNUSED()

#define SPEC_FREE_OP2_IF_VAR_ANY()              FREE_OP_IF_VAR(free_op2);
#define SPEC_FREE_OP2_IF_VAR_CONST()
#define SPEC_FREE_OP2_IF_VAR_TMP()
#define SPEC_FREE_OP2_IF_VAR_VAR()              FREE_OP_VAR_PTR(free_op2);
#define SPEC_FREE_OP2_IF_VAR_UNUSED()

#define SPEC_FREE_OP2_VAR_PTR_ANY()             FREE_OP_VAR_PTR(free_op2);
#define SPEC_FREE_OP2_VAR_PTR_CONST()
#define SPEC_FREE_OP2_VAR_PTR_TMP()
#define SPEC_FREE_OP2_VAR_PTR_VAR()             FREE_OP_VAR_PTR(free_op2);
#define SPEC_FREE_OP2_VAR_PTR_UNUSED()

#ifndef ZEND_VM_SPEC

# define OP1_OP2_MASK(OP1T, OP2T)           1
# define HAVE_OP(OP)                        1

# define ZEND_VM_C_GOTO(LABEL)              goto LABEL
# define ZEND_VM_C_LABEL(LABEL)             LABEL

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

#else

# define M_ANY                  (IS_ANY)
# define M_CONST                (IS_CONST)
# define M_TMP                  (IS_TMP_VAR)
# define M_VAR                  (IS_VAR)
# define M_UNUSED               (IS_UNUSED)
# define M_CONST_TMP            (IS_CONST|IS_TMP_VAR)
# define M_CONST_VAR            (IS_CONST|IS_VAR)
# define M_CONST_UNUSED         (IS_CONST|IS_UNUSED)
# define M_CONST_TMP_VAR        (IS_CONST|IS_TMP_VAR|IS_VAR)
# define M_CONST_TMP_UNUSED     (IS_CONST|IS_TMP_VAR|IS_UNUSED)
# define M_CONST_VAR_UNUSED     (IS_CONST|IS_VAR|IS_UNUSED)
# define M_CONST_TMP_VAR_UNUSED (IS_CONST|IS_TMP_VAR|IS_VAR|IS_UNUSED)
# define M_TMP_VAR              (IS_TMP_VAR|IS_VAR)
# define M_TMP_UNUSED           (IS_TMP_VAR|IS_UNUSED)
# define M_TMP_VAR_UNUSED       (IS_TMP_VAR|IS_VAR|IS_UNUSED)
# define M_VAR_UNUSED           (IS_VAR|IS_UNUSED)

# define OP1_OP2_MASK(OP1T, OP2T) \
  ((OP1 & (OP1T)) && (OP2 & (OP2T)))

# define OPDEF(OP,OP1M,OP2M) \
  ((OP1 & (OP1M)) && (OP2 & (OP2M)))

# define HAVE_OP(OP) \
  OP##_SPEC()

# define ZEND_VM_SPEC_C_GOTO2(LABEL, OP1T, OP2T) \
  goto LABEL##OP1T##OP2T
# define ZEND_VM_SPEC_C_GOTO1(LABEL, OP1T, OP2T) \
  ZEND_VM_SPEC_C_GOTO2(LABEL, OP1T, OP2T)
# define ZEND_VM_C_GOTO(LABEL) \
  ZEND_VM_SPEC_C_GOTO1(LABEL, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX)

# define ZEND_VM_SPEC_C_LABEL2(LABEL, OP1T, OP2T) \
  LABEL##OP1T##OP2T
# define ZEND_VM_SPEC_C_LABEL1(LABEL, OP1T, OP2T) \
  ZEND_VM_SPEC_C_LABEL2(LABEL, OP1T, OP2T)
# define ZEND_VM_C_LABEL(LABEL) \
  ZEND_VM_SPEC_C_LABEL1(LABEL, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX)

# define ZEND_VM_SPEC_HANDLER2(OP, OP1T, OP2T, CODE) \
  ZEND_VM_SPEC_HANDLER(OP##OP1T##OP2T, CODE)
# define ZEND_VM_SPEC_HANDLER1(OP, OP1T, OP2T, CODE) \
  ZEND_VM_SPEC_HANDLER2(OP, OP1T, OP2T, CODE)
# undef ZEND_VM_HANDLER
# define ZEND_VM_HANDLER(OP) \
  ZEND_VM_SPEC_HANDLER1(OP##_SPEC, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX, ZEND_VM_CODE(OP,OP1_TYPE_CODE,OP2_TYPE_CODE))

# define ZEND_VM_SPEC_HANDLER_EX2(OP, OP1T, OP2T, CODE) \
  ZEND_VM_SPEC_HANDLER_EX(OP##OP1T##OP2T, CODE)
# define ZEND_VM_SPEC_HANDLER_EX1(OP, OP1T, OP2T, CODE) \
  ZEND_VM_SPEC_HANDLER_EX2(OP, OP1T, OP2T, CODE)
# undef ZEND_VM_HANDLER_EX
# define ZEND_VM_HANDLER_EX(OP) \
  ZEND_VM_SPEC_HANDLER_EX1(OP##_SPEC, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX, ZEND_VM_CODE(OP,OP1_TYPE_CODE,OP2_TYPE_CODE))

# define ZEND_VM_SPEC_HELPER2(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_HELPER(OP##OP1T##OP2T)
# define ZEND_VM_SPEC_HELPER1(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_HELPER2(OP, OP1T, OP2T)
# undef ZEND_VM_HELPER
# define ZEND_VM_HELPER(OP) \
  ZEND_VM_SPEC_HELPER1(OP, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX)

# define ZEND_VM_SPEC_HELPER_EX2(OP, OP1T, OP2T, PARAM) \
  ZEND_VM_SPEC_HELPER_EX(OP##OP1T##OP2T, PARAM)
# define ZEND_VM_SPEC_HELPER_EX1(OP, OP1T, OP2T, PARAM) \
  ZEND_VM_SPEC_HELPER_EX2(OP, OP1T, OP2T, PARAM)
# undef ZEND_VM_HELPER_EX
# define ZEND_VM_HELPER_EX(OP, PARAM) \
  ZEND_VM_SPEC_HELPER_EX1(OP, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX, PARAM)

# define ZEND_VM_SPEC_DISPATCH_TO_HANDLER2(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_DISPATCH_TO_HANDLER(OP##OP1T##OP2T)
# define ZEND_VM_SPEC_DISPATCH_TO_HANDLER1(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_DISPATCH_TO_HANDLER2(OP, OP1T, OP2T)
# undef ZEND_VM_DISPATCH_TO_HANDLER
# define ZEND_VM_DISPATCH_TO_HANDLER(OP) \
  ZEND_VM_SPEC_DISPATCH_TO_HANDLER1(OP##_SPEC, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX)

# define ZEND_VM_SPEC_DISPATCH_TO_HELPER2(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER(OP##OP1T##OP2T)
# define ZEND_VM_SPEC_DISPATCH_TO_HELPER1(OP, OP1T, OP2T) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER2(OP, OP1T, OP2T)
# undef ZEND_VM_DISPATCH_TO_HELPER
# define ZEND_VM_DISPATCH_TO_HELPER(OP) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER1(OP, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX)

# define ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX2(OP, OP1T, OP2T, VAR, VAL) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX(OP##OP1T##OP2T, VAR, VAL)
# define ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX1(OP, OP1T, OP2T, VAR, VAL) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX2(OP, OP1T, OP2T, VAR, VAL)
# undef ZEND_VM_DISPATCH_TO_HELPER_EX
# define ZEND_VM_DISPATCH_TO_HELPER_EX(OP, VAR, VAL) \
  ZEND_VM_SPEC_DISPATCH_TO_HELPER_EX1(OP, OP1_TYPE_PREFIX, OP2_TYPE_PREFIX, VAR, VAL)

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_CONST
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_TMP_VAR
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_VAR
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_UNUSED
# define OP2 IS_ANY
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_CONST
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_TMP_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_ANY
# define OP2 IS_UNUSED
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_UNUSED
# define OP2 IS_UNUSED
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_UNUSED
# define OP2 IS_CONST
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_UNUSED
# define OP2 IS_TMP_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_UNUSED
# define OP2 IS_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_CONST
# define OP2 IS_UNUSED
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_TMP_VAR
# define OP2 IS_UNUSED
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_VAR
# define OP2 IS_UNUSED
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_CONST
# define OP2 IS_CONST
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_CONST
# define OP2 IS_TMP_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_CONST
# define OP2 IS_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_TMP_VAR
# define OP2 IS_CONST
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_TMP_VAR
# define OP2 IS_TMP_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_TMP_VAR
# define OP2 IS_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_VAR
# define OP2 IS_CONST
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_VAR
# define OP2 IS_TMP_VAR
# include "zend_vm_handlers.h"

# undef  OP1
# undef  OP2
# define OP1 IS_VAR
# define OP2 IS_VAR
# include "zend_vm_handlers.h"

/* LABELS */

# undef M_ANY
# undef M_CONST
# undef M_TMP
# undef M_VAR
# undef M_UNUSED
# undef M_CONST_TMP
# undef M_CONST_VAR
# undef M_CONST_UNUSED
# undef M_CONST_TMP_VAR
# undef M_CONST_TMP_UNUSED
# undef M_CONST_VAR_UNUSED
# undef M_CONST_TMP_VAR_UNUSED
# undef M_TMP_VAR
# undef M_TMP_UNUSED
# undef M_TMP_VAR_UNUSED
# undef M_VAR_UNUSED

# undef OPDEF

# define OPDEF(OP, OP1M, OP2M) \
	SPEC_LABELS1_##OP1M(OP##_SPEC, OP, OP2M)

# define SPEC_LABELS1_M_ANY(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _ANY, _ANY, _ANY, _ANY)
# define SPEC_LABELS1_M_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _NULL, _NULL, _UNUSED)
# define SPEC_LABELS1_M_VAR(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _NULL, _VAR, _NULL)
# define SPEC_LABELS1_M_VAR_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _NULL, _VAR, _UNUSED)
# define SPEC_LABELS1_M_TMP(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _TMP, _NULL, _NULL)
# define SPEC_LABELS1_M_TMP_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _TMP, _NULL, _UNUSED)
# define SPEC_LABELS1_M_TMP_VAR(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _TMP, _VAR, _NULL)
# define SPEC_LABELS1_M_TMP_VAR_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _NULL, _TMP, _VAR, _UNUSED)
# define SPEC_LABELS1_M_CONST(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _NULL, _NULL, _NULL)
# define SPEC_LABELS1_M_CONST_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _NULL, _NULL, _UNUSED)
# define SPEC_LABELS1_M_CONST_VAR(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _NULL, _VAR, _NULL)
# define SPEC_LABELS1_M_CONST_VAR_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _NULL, _VAR, _UNUSED)
# define SPEC_LABELS1_M_CONST_TMP(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _TMP, _NULL, _NULL)
# define SPEC_LABELS1_M_CONST_TMP_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _TMP, _NULL, _UNUSED)
# define SPEC_LABELS1_M_CONST_TMP_VAR(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _TMP, _VAR, _NULL)
# define SPEC_LABELS1_M_CONST_TMP_VAR_UNUSED(OP, CODE, OP2M) \
  SPEC_LABELS2_##OP2M(OP, CODE, _CONST, _TMP, _VAR, _UNUSED)

# define SPEC_LABELS2_M_ANY(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _ANY, _ANY, _ANY, _ANY)
# define SPEC_LABELS2_M_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _NULL, _NULL, _UNUSED)
# define SPEC_LABELS2_M_VAR(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _NULL, _VAR, _NULL)
# define SPEC_LABELS2_M_VAR_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _NULL, _VAR, _UNUSED)
# define SPEC_LABELS2_M_TMP(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _TMP, _NULL, _NULL)
# define SPEC_LABELS2_M_TMP_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _TMP, _NULL, _UNUSED)
# define SPEC_LABELS2_M_TMP_VAR(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _TMP, _VAR, _NULL)
# define SPEC_LABELS2_M_TMP_VAR_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _NULL, _TMP, _VAR, _UNUSED)
# define SPEC_LABELS2_M_CONST(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _NULL, _NULL, _NULL)
# define SPEC_LABELS2_M_CONST_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _NULL, _NULL, _UNUSED)
# define SPEC_LABELS2_M_CONST_VAR(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _NULL, _VAR, _NULL)
# define SPEC_LABELS2_M_CONST_VAR_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _NULL, _VAR, _UNUSED)
# define SPEC_LABELS2_M_CONST_TMP(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _TMP, _NULL, _NULL)
# define SPEC_LABELS2_M_CONST_TMP_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _TMP, _NULL, _UNUSED)
# define SPEC_LABELS2_M_CONST_TMP_VAR(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _TMP, _VAR, _NULL)
# define SPEC_LABELS2_M_CONST_TMP_VAR_UNUSED(OP, CODE, OP1C, OP1T, OP1V, OP1U) \
  SPEC_LABELS(OP, CODE, OP1C, OP1T, OP1V, OP1U, _CONST, _TMP, _VAR, _UNUSED)

# define LABELS_1(OP,CODE,OP1,OP2C,OP2T,OP2V,OP2U) \
  OP1##OP2C(OP,CODE), \
  OP1##OP2T(OP,CODE), \
  OP1##OP2V(OP,CODE), \
  OP1##OP2U(OP,CODE)

# define SPEC_LABELS(OP,CODE,OP1C,OP1T,OP1V,OP1U,OP2C,OP2T,OP2V,OP2U) \
	LABELS_1(OP,CODE,SPEC_LABEL##OP1C,OP2C,OP2T,OP2V,OP2U), \
	LABELS_1(OP,CODE,SPEC_LABEL##OP1T,OP2C,OP2T,OP2V,OP2U), \
	LABELS_1(OP,CODE,SPEC_LABEL##OP1V,OP2C,OP2T,OP2V,OP2U), \
	LABELS_1(OP,CODE,SPEC_LABEL##OP1U,OP2C,OP2T,OP2V,OP2U)

# define SPEC_LABEL_CONST_NULL(OP,CODE)    ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_TMP_NULL(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_VAR_NULL(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_UNUSED_NULL(OP,CODE)   ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_ANY_NULL(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_CONST(OP,CODE)    ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_TMP(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_VAR(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_UNUSED(OP,CODE)   ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_ANY(OP,CODE)      ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_NULL_NULL(OP,CODE)     ZEND_VM_SPEC_NULL_LABEL
# define SPEC_LABEL_ANY_ANY(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_ANY_ANY, ZEND_VM_CODE(CODE, _ANY_CODE, _ANY_CODE))
# define SPEC_LABEL_ANY_CONST(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_ANY_CONST, ZEND_VM_CODE(CODE, _ANY_CODE, _CONST_CODE))
# define SPEC_LABEL_ANY_TMP(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_ANY_TMP, ZEND_VM_CODE(CODE, _ANY_CODE, _TMP_CODE))
# define SPEC_LABEL_ANY_VAR(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_ANY_VAR, ZEND_VM_CODE(CODE, _ANY_CODE, _VAR_CODE))
# define SPEC_LABEL_ANY_UNUSED(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_ANY_UNUSED, ZEND_VM_CODE(CODE, _ANY_CODE, _UNUSED_CODE))
# define SPEC_LABEL_CONST_ANY(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_CONST_ANY, ZEND_VM_CODE(CODE, _CONST_CODE, _ANY_CODE))
# define SPEC_LABEL_CONST_CONST(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_CONST_CONST, ZEND_VM_CODE(CODE, _CONST_CODE, _CONST_CODE))
# define SPEC_LABEL_CONST_TMP(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_CONST_TMP, ZEND_VM_CODE(CODE, _CONST_CODE, _TMP_CODE))
# define SPEC_LABEL_CONST_VAR(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_CONST_VAR, ZEND_VM_CODE(CODE, _CONST_CODE, _VAR_CODE))
# define SPEC_LABEL_CONST_UNUSED(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_CONST_UNUSED, ZEND_VM_CODE(CODE, _CONST_CODE, _UNUSED_CODE))
# define SPEC_LABEL_TMP_ANY(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_TMP_ANY, ZEND_VM_CODE(CODE, _TMP_CODE, _ANY_CODE))
# define SPEC_LABEL_TMP_CONST(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_TMP_CONST, ZEND_VM_CODE(CODE, _TMP_CODE, _CONST_CODE))
# define SPEC_LABEL_TMP_TMP(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_TMP_TMP, ZEND_VM_CODE(CODE, _TMP_CODE, _TMP_CODE))
# define SPEC_LABEL_TMP_VAR(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_TMP_VAR, ZEND_VM_CODE(CODE, _TMP_CODE, _VAR_CODE))
# define SPEC_LABEL_TMP_UNUSED(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_TMP_UNUSED, ZEND_VM_CODE(CODE, _TMP_CODE, _UNUSED_CODE))
# define SPEC_LABEL_VAR_ANY(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_VAR_ANY, ZEND_VM_CODE(CODE, _VAR_CODE, _ANY_CODE))
# define SPEC_LABEL_VAR_CONST(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_VAR_CONST, ZEND_VM_CODE(CODE, _VAR_CODE, _CONST_CODE))
# define SPEC_LABEL_VAR_TMP(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_VAR_TMP, ZEND_VM_CODE(CODE, _VAR_CODE, _TMP_CODE))
# define SPEC_LABEL_VAR_VAR(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_VAR_VAR, ZEND_VM_CODE(CODE, _VAR_CODE, _VAR_CODE))
# define SPEC_LABEL_VAR_UNUSED(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_VAR_UNUSED, ZEND_VM_CODE(CODE, _VAR_CODE, _UNUSED_CODE))
# define SPEC_LABEL_UNUSED_ANY(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_UNUSED_ANY, ZEND_VM_CODE(CODE, _UNUSED_CODE, _ANY_CODE))
# define SPEC_LABEL_UNUSED_CONST(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_UNUSED_CONST, ZEND_VM_CODE(CODE, _UNUSED_CODE, _CONST_CODE))
# define SPEC_LABEL_UNUSED_TMP(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_UNUSED_TMP, ZEND_VM_CODE(CODE, _UNUSED_CODE, _TMP_CODE))
# define SPEC_LABEL_UNUSED_VAR(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_UNUSED_VAR, ZEND_VM_CODE(CODE, _UNUSED_CODE, _VAR_CODE))
# define SPEC_LABEL_UNUSED_UNUSED(OP,CODE) \
  ZEND_VM_SPEC_LABEL(OP##_UNUSED_UNUSED, ZEND_VM_CODE(CODE, _UNUSED_CODE, _UNUSED_CODE))

# undef ZEND_VM_NULL_LABEL
# define ZEND_VM_NULL_LABEL \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL, \
  ZEND_VM_SPEC_NULL_LABEL

# undef ZEND_VM_LABEL
# define ZEND_VM_LABEL(OP) \
  OP##_SPEC()

#endif

ZEND_VM_NULL_HANDLER()
{
  zend_error_noreturn(E_ERROR, "Invalid opcode %d/%d/%d.", EX(opline)->opcode, EX(opline)->op1.op_type, EX(opline)->op2.op_type);
  ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}
