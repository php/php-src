/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* If you change this file, please regenerate the zend_vm_execute.h and
 * zend_vm_opcodes.h files by running:
 * php zend_vm_gen.php
 */

ZEND_VM_HANDLER(1, ZEND_ADD, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	add_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(2, ZEND_SUB, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	sub_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(3, ZEND_MUL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mul_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(4, ZEND_DIV, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	div_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(5, ZEND_MOD, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	mod_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(6, ZEND_SL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_left_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(7, ZEND_SR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	shift_right_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(8, ZEND_CONCAT, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	concat_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(15, ZEND_IS_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_identical_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(16, ZEND_IS_NOT_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	is_identical_function(result,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	Z_LVAL_P(result) = !Z_LVAL_P(result);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(17, ZEND_IS_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	compare_function(result,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	ZVAL_BOOL(result, (Z_LVAL_P(result) == 0));
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(18, ZEND_IS_NOT_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	compare_function(result,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	ZVAL_BOOL(result, (Z_LVAL_P(result) != 0));
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(19, ZEND_IS_SMALLER, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	compare_function(result,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	ZVAL_BOOL(result, (Z_LVAL_P(result) < 0));
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(20, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	compare_function(result,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	ZVAL_BOOL(result, (Z_LVAL_P(result) <= 0));
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(9, ZEND_BW_OR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_or_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(10, ZEND_BW_AND, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_and_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(11, ZEND_BW_XOR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	bitwise_xor_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(14, ZEND_BOOL_XOR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	boolean_xor_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(12, ZEND_BW_NOT, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	bitwise_not_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(13, ZEND_BOOL_NOT, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	boolean_not_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_obj_helper, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op2, free_op_data1;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	znode *result = &opline->result;
	int have_get_ptr = 0;

	if (OP1_TYPE == IS_VAR && !object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP2();
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			EX_T(result->u.var).var.ptr = EG(uninitialized_zval_ptr);
			EX_T(result->u.var).var.ptr_ptr = NULL;
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(*zptr, *zptr, value TSRMLS_CC);
				if (!RETURN_VALUE_UNUSED(result)) {
					EX_T(result->u.var).var.ptr = *zptr;
					EX_T(result->u.var).var.ptr_ptr = NULL;
					PZVAL_LOCK(*zptr);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			if (opline->extended_value == ZEND_ASSIGN_OBJ) {
				if (Z_OBJ_HT_P(object)->read_property) {
					z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
				}
			} else /* if (opline->extended_value == ZEND_ASSIGN_DIM) */ {
				if (Z_OBJ_HT_P(object)->read_dimension) {
					z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
				}
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (Z_REFCOUNT_P(z) == 0) {
						GC_REMOVE_ZVAL_FROM_BUFFER(z);
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				Z_ADDREF_P(z);
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				if (opline->extended_value == ZEND_ASSIGN_OBJ) {
					Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
				} else /* if (opline->extended_value == ZEND_ASSIGN_DIM) */ {
					Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					EX_T(result->u.var).var.ptr = z;
					EX_T(result->u.var).var.ptr_ptr = NULL;
					PZVAL_LOCK(z);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
					EX_T(result->u.var).var.ptr = EG(uninitialized_zval_ptr);
					EX_T(result->u.var).var.ptr_ptr = NULL;
					PZVAL_LOCK(EG(uninitialized_zval_ptr));
				}
			}
		}

		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(&property);
		} else {
			FREE_OP2();
		}
		FREE_OP(free_op_data1);
	}

	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_helper, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_op_data2, free_op_data1;
	zval **var_ptr;
	zval *value;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

				if (OP1_TYPE == IS_VAR && !container) {
					zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
				} else if (Z_TYPE_PP(container) == IS_OBJECT) {
					if (OP1_TYPE == IS_VAR && !OP1_FREE) {
						Z_ADDREF_PP(container);  /* undo the effect of get_obj_zval_ptr_ptr() */
					}
					ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = _get_zval_ptr_ptr_var(&op_data->op2, EX(Ts), &free_op_data2 TSRMLS_CC);
					ZEND_VM_INC_OPCODE();
				}
			}
			break;
		default:
			value = GET_OP2_ZVAL_PTR(BP_VAR_R);
			var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);
			/* do nothing */
			break;
	}

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		Z_ADDREF_P(objval);
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		AI_SET_PTR(EX_T(opline->result.u.var).var, *var_ptr);
		PZVAL_LOCK(*var_ptr);
	}
	FREE_OP2();

	if (opline->extended_value == ZEND_ASSIGN_DIM) {
		FREE_OP(free_op_data1);
		FREE_OP_VAR_PTR(free_op_data2);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(23, ZEND_ASSIGN_ADD, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, add_function);
}

ZEND_VM_HANDLER(24, ZEND_ASSIGN_SUB, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, sub_function);
}

ZEND_VM_HANDLER(25, ZEND_ASSIGN_MUL, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mul_function);
}

ZEND_VM_HANDLER(26, ZEND_ASSIGN_DIV, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, div_function);
}

ZEND_VM_HANDLER(27, ZEND_ASSIGN_MOD, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mod_function);
}

ZEND_VM_HANDLER(28, ZEND_ASSIGN_SL, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_left_function);
}

ZEND_VM_HANDLER(29, ZEND_ASSIGN_SR, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_right_function);
}

ZEND_VM_HANDLER(30, ZEND_ASSIGN_CONCAT, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, concat_function);
}

ZEND_VM_HANDLER(31, ZEND_ASSIGN_BW_OR, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_or_function);
}

ZEND_VM_HANDLER(32, ZEND_ASSIGN_BW_AND, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_and_function);
}

ZEND_VM_HANDLER(33, ZEND_ASSIGN_BW_XOR, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_xor_function);
}

ZEND_VM_HELPER_EX(zend_pre_incdec_property_helper, VAR|UNUSED|CV, CONST|TMP|VAR|CV, incdec_t incdec_op)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **retval = &EX_T(opline->result.u.var).var.ptr;
	int have_get_ptr = 0;

	if (OP1_TYPE == IS_VAR && !object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(*zptr);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = *zptr;
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (Z_REFCOUNT_P(z) == 0) {
					GC_REMOVE_ZVAL_FROM_BUFFER(z);
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			Z_ADDREF_P(z);
			SEPARATE_ZVAL_IF_NOT_REF(&z);
			incdec_op(z);
			*retval = z;
			Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				*retval = EG(uninitialized_zval_ptr);
				PZVAL_LOCK(*retval);
			}
		}
	}

	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(132, ZEND_PRE_INC_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, incdec_op, increment_function);
}

ZEND_VM_HANDLER(133, ZEND_PRE_DEC_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, incdec_op, decrement_function);
}

ZEND_VM_HELPER_EX(zend_post_incdec_property_helper, VAR|UNUSED|CV, CONST|TMP|VAR|CV, incdec_t incdec_op)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *object;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *retval = &EX_T(opline->result.u.var).tmp_var;
	int have_get_ptr = 0;

	if (OP1_TYPE == IS_VAR && !object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}

	make_real_object(object_ptr TSRMLS_CC); /* this should modify object only if it's empty */
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		*retval = *EG(uninitialized_zval_ptr);
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval **zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			*retval = **zptr;
			zendi_zval_copy_ctor(*retval);

			incdec_op(*zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
			zval *z_copy;

			if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (Z_REFCOUNT_P(z) == 0) {
					GC_REMOVE_ZVAL_FROM_BUFFER(z);
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			*retval = *z;
			zendi_zval_copy_ctor(*retval);
			ALLOC_ZVAL(z_copy);
			*z_copy = *z;
			zendi_zval_copy_ctor(*z_copy);
			INIT_PZVAL(z_copy);
			incdec_op(z_copy);
			Z_ADDREF_P(z);
			Z_OBJ_HT_P(object)->write_property(object, property, z_copy TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(&z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			*retval = *EG(uninitialized_zval_ptr);
		}
	}

	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(134, ZEND_POST_INC_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, incdec_op, increment_function);
}

ZEND_VM_HANDLER(135, ZEND_POST_DEC_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, incdec_op, decrement_function);
}

ZEND_VM_HANDLER(34, ZEND_PRE_INC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && !var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && *var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		AI_SET_PTR(EX_T(opline->result.u.var).var, *var_ptr);
		PZVAL_LOCK(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && !var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && *var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		AI_SET_PTR(EX_T(opline->result.u.var).var, *var_ptr);
		PZVAL_LOCK(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(36, ZEND_POST_INC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && !var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && *var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(37, ZEND_POST_DEC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && !var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && *var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).tmp_var = *EG(uninitialized_zval_ptr);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	EX_T(opline->result.u.var).tmp_var = **var_ptr;
	zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(40, ZEND_ECHO, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *z = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(z) == IS_OBJECT) {
		INIT_PZVAL(z);
	}
	zend_print_variable(z);

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(41, ZEND_PRINT, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;

	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ECHO);
}

ZEND_VM_HELPER_EX(zend_fetch_var_address_helper, CONST|TMP|VAR|CV, ANY, int type)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varname = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval **retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

 	if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
		FREE_OP1();
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), type, varname TSRMLS_CC);
/*
		if (!target_symbol_table) {
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &retval) == FAILURE) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval_ptr);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W: {
						zval *new_zval = &EG(uninitialized_zval);

						Z_ADDREF_P(new_zval);
						zend_hash_update(target_symbol_table, varname->value.str.val, varname->value.str.len+1, &new_zval, sizeof(zval *), (void **) &retval);
					}
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->op2.u.EA.type) {
			case ZEND_FETCH_GLOBAL:
				if (OP1_TYPE != IS_TMP_VAR) {
					FREE_OP1();
				}
				break;
			case ZEND_FETCH_LOCAL:
				FREE_OP1();
				break;
			case ZEND_FETCH_STATIC:
				zval_update_constant(retval, (void*) 1 TSRMLS_CC);
				break;
			case ZEND_FETCH_GLOBAL_LOCK:
				if (OP1_TYPE == IS_VAR && !free_op1.var) {
					PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
				}
				break;
		}
	}


	if (OP1_TYPE != IS_CONST && varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		if (opline->extended_value & ZEND_FETCH_MAKE_REF) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(retval);
		}
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_SET_PTR(EX_T(opline->result.u.var).var, *retval);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				EX_T(opline->result.u.var).var.ptr_ptr = retval;
				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
				break;
			default:
				EX_T(opline->result.u.var).var.ptr_ptr = retval;
				break;
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(80, ZEND_FETCH_R, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(83, ZEND_FETCH_W, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(86, ZEND_FETCH_RW, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(92, ZEND_FETCH_FUNC_ARG, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type,
		ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), EX(opline)->extended_value)?BP_VAR_W:BP_VAR_R);
}

ZEND_VM_HANDLER(95, ZEND_FETCH_UNSET, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(89, ZEND_FETCH_IS, CONST|TMP|VAR|CV, ANY)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_IS);
}

ZEND_VM_HANDLER(81, ZEND_FETCH_DIM_R, VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container;

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    OP1_TYPE != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address_read(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_R TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(84, ZEND_FETCH_DIM_W, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_W TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();

	/* We are going to assign the result by reference */
	if (opline->extended_value && EX_T(opline->result.u.var).var.ptr_ptr) {
		Z_DELREF_PP(EX_T(opline->result.u.var).var.ptr_ptr);
		SEPARATE_ZVAL_TO_MAKE_IS_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		Z_ADDREF_PP(EX_T(opline->result.u.var).var.ptr_ptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_RW TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(90, ZEND_FETCH_DIM_IS, VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address_read(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_IS TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container;

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		if (OP1_TYPE == IS_VAR && !container) {
			zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
		}
		zend_fetch_dimension_address(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_W TSRMLS_CC);
		if (OP1_TYPE == IS_VAR && OP1_FREE &&
		    READY_TO_DESTROY(free_op1.var)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
		}
		container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
		if (OP1_TYPE == IS_VAR && !container) {
			zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
		}
		zend_fetch_dimension_address_read(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_R TSRMLS_CC);
	}
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(96, ZEND_FETCH_DIM_UNSET, VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_UNSET);
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

	/* Not needed in DIM_UNSET
	if (opline->extended_value == ZEND_FETCH_ADD_LOCK) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	*/
	if (OP1_TYPE == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address(&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_UNSET TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();
	if (EX_T(opline->result.u.var).var.ptr_ptr == NULL) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
	} else {
		zend_free_op free_res;

		PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
		if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		}
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		FREE_OP_VAR_PTR(free_res);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_fetch_property_address_read_helper, VAR|UNUSED|CV, CONST|TMP|VAR|CV, int type)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *container = GET_OP1_OBJ_ZVAL_PTR(type);
	zend_free_op free_op2;
	zval *offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
		FREE_OP2();
	} else {
		zval *retval;

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result)) {
			if (Z_REFCOUNT_P(retval) == 0) {
				GC_REMOVE_ZVAL_FROM_BUFFER(retval);
				zval_dtor(retval);
				FREE_ZVAL(retval);
			}
		} else {
			AI_SET_PTR(EX_T(opline->result.u.var).var, retval);
			PZVAL_LOCK(retval);
		}

		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(&offset);
		} else {
			FREE_OP2();
		}
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(82, ZEND_FETCH_OBJ_R, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container;

	if (OP1_TYPE == IS_VAR && (opline->extended_value & ZEND_FETCH_ADD_LOCK)) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}
	zend_fetch_property_address(&EX_T(opline->result.u.var), container, property, BP_VAR_W TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();

	/* We are going to assign the result by reference */
	if (opline->extended_value & ZEND_FETCH_MAKE_REF) {
		Z_DELREF_PP(EX_T(opline->result.u.var).var.ptr_ptr);
		SEPARATE_ZVAL_TO_MAKE_IS_REF(EX_T(opline->result.u.var).var.ptr_ptr);
		Z_ADDREF_PP(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}
	zend_fetch_property_address(&EX_T(opline->result.u.var), container, property, BP_VAR_RW TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(91, ZEND_FETCH_OBJ_IS, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_IS);
}

ZEND_VM_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
		zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		if (OP1_TYPE == IS_VAR && !container) {
			zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
		}
		zend_fetch_property_address(&EX_T(opline->result.u.var), container, property, BP_VAR_W TSRMLS_CC);
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(&property);
		} else {
			FREE_OP2();
		}
		if (OP1_TYPE == IS_VAR && OP1_FREE &&
		    READY_TO_DESTROY(free_op1.var)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
				SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
			}
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_property_address_read_helper, type, BP_VAR_R);
	}
}

ZEND_VM_HANDLER(97, ZEND_FETCH_OBJ_UNSET, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2, free_res;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_R);
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CV) {
		if (container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
	}
	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	if (OP1_TYPE == IS_VAR && !container) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}
	zend_fetch_property_address(&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    Z_REFCOUNT_PP(EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();

	PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
	if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
	}
	PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
	FREE_OP_VAR_PTR(free_res);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_DIM_TMP_VAR, CONST|TMP, CONST)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *container = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(container) != IS_ARRAY) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
	} else {
		zend_free_op free_op2;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		AI_SET_PTR(EX_T(opline->result.u.var).var, *zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, BP_VAR_R TSRMLS_CC));
		SELECTIVE_PZVAL_LOCK(EX_T(opline->result.u.var).var.ptr, &opline->result);
		FREE_OP2();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1, free_op2;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);
	zval *property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property_name);
	}
	if (OP1_TYPE == IS_VAR && !object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_assign_to_object(&opline->result, object_ptr, property_name, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property_name);
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(147, ZEND_ASSIGN_DIM, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && !object_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	if (Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_free_op free_op2;
		zval *property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		zend_assign_to_object(&opline->result, object_ptr, property_name, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(&property_name);
		} else {
			FREE_OP2();
		}
	} else {
		zend_free_op free_op2, free_op_data1, free_op_data2;
		zval *value;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
		zval **variable_ptr_ptr;

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, IS_OP2_TMP_FREE(), BP_VAR_W TSRMLS_CC);
		FREE_OP2();

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
		variable_ptr_ptr = _get_zval_ptr_ptr_var(&op_data->op2, EX(Ts), &free_op_data2 TSRMLS_CC);
		if (!variable_ptr_ptr) {
			if (zend_assign_to_string_offset(&EX_T(op_data->op2.u.var), value, op_data->op1.op_type TSRMLS_CC)) {
				if (!RETURN_VALUE_UNUSED(&opline->result)) {
					EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
					ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
					INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
					ZVAL_STRINGL(EX_T(opline->result.u.var).var.ptr, Z_STRVAL_P(EX_T(op_data->op2.u.var).str_offset.str)+EX_T(op_data->op2.u.var).str_offset.offset, 1, 1);
				}
			} else if (!RETURN_VALUE_UNUSED(&opline->result)) {
				AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
				PZVAL_LOCK(EG(uninitialized_zval_ptr));
			}
		} else {
		 	value = zend_assign_to_variable(variable_ptr_ptr, value, IS_TMP_FREE(free_op_data1) TSRMLS_CC);
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				AI_SET_PTR(EX_T(opline->result.u.var).var, value);
				PZVAL_LOCK(value);
			}
		}
		FREE_OP_VAR_PTR(free_op_data2);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	FREE_OP1_VAR_PTR();
	/* assign_dim has two opcodes! */
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(38, ZEND_ASSIGN, VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval **variable_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && !variable_ptr_ptr) {
		if (zend_assign_to_string_offset(&EX_T(opline->op1.u.var), value, OP2_TYPE TSRMLS_CC)) {
			if (!RETURN_VALUE_UNUSED(&opline->result)) {
				EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				ZVAL_STRINGL(EX_T(opline->result.u.var).var.ptr, Z_STRVAL_P(EX_T(opline->op1.u.var).str_offset.str)+EX_T(opline->op1.u.var).str_offset.offset, 1, 1);
			}
		} else if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, EG(uninitialized_zval_ptr));
			PZVAL_LOCK(EG(uninitialized_zval_ptr));
		}
	} else {
	 	value = zend_assign_to_variable(variable_ptr_ptr, value, IS_OP2_TMP_FREE() TSRMLS_CC);
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, value);
			PZVAL_LOCK(value);
		}
	}

	FREE_OP1_VAR_PTR();

	/* zend_assign_to_variable() always takes care of op2, never free it! */
 	FREE_OP2_IF_VAR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(39, ZEND_ASSIGN_REF, VAR|CV, VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **variable_ptr_ptr;
	zval **value_ptr_ptr = GET_OP2_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP2_TYPE == IS_VAR &&
	    value_ptr_ptr &&
	    !Z_ISREF_PP(value_ptr_ptr) &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (UNEXPECTED(EG(exception) != NULL)) {
			FREE_OP2_VAR_PTR();
			ZEND_VM_NEXT_OPCODE();
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ASSIGN);
	} else if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (OP1_TYPE == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error_noreturn(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
	if ((OP2_TYPE == IS_VAR && !value_ptr_ptr) ||
	    (OP1_TYPE == IS_VAR && !variable_ptr_ptr)) {
		zend_error_noreturn(E_ERROR, "Cannot create references to/from string offsets nor overloaded objects");
	}
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		Z_DELREF_PP(variable_ptr_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		AI_SET_PTR(EX_T(opline->result.u.var).var, *variable_ptr_ptr);
		PZVAL_LOCK(*variable_ptr_ptr);
	}

	FREE_OP1_VAR_PTR();
	FREE_OP2_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(42, ZEND_JMP, ANY, ANY)
{
#if DEBUG_ZEND>=2
	printf("Jumping to %d\n", EX(opline)->op1.u.opline_num);
#endif
	ZEND_VM_SET_OPCODE(EX(opline)->op1.u.jmp_addr);
	ZEND_VM_CONTINUE(); /* CHECK_ME */
}

ZEND_VM_HANDLER(43, ZEND_JMPZ, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *val = GET_OP1_ZVAL_PTR(BP_VAR_R);
	int ret;

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(val) == IS_BOOL) {
		ret = Z_LVAL_P(val);
	} else {
		ret = i_zend_is_true(val);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
	}
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(44, ZEND_JMPNZ, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *val = GET_OP1_ZVAL_PTR(BP_VAR_R);
	int ret;

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(val) == IS_BOOL) {
		ret = Z_LVAL_P(val);
	} else {
		ret = i_zend_is_true(val);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
	}
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(45, ZEND_JMPZNZ, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *val = GET_OP1_ZVAL_PTR(BP_VAR_R);
	int retval;

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(val) == IS_BOOL) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
	}
	if (EXPECTED(retval != 0)) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
}

ZEND_VM_HANDLER(46, ZEND_JMPZ_EX, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *val = GET_OP1_ZVAL_PTR(BP_VAR_R);
	int retval;

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(val) == IS_BOOL) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
	}
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *val = GET_OP1_ZVAL_PTR(BP_VAR_R);
	int retval;

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(val) == IS_BOOL) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
	}
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.u.jmp_addr);
		ZEND_VM_CONTINUE();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(70, ZEND_FREE, TMP, ANY)
{
	zendi_zval_dtor(EX_T(EX(opline)->op1.u.var).tmp_var);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(53, ZEND_INIT_STRING, ANY, ANY)
{
	zval *tmp = &EX_T(EX(opline)->result.u.var).tmp_var;

	tmp->value.str.val = emalloc(1);
	tmp->value.str.val[0] = 0;
	tmp->value.str.len = 0;
	Z_SET_REFCOUNT_P(tmp, 1);
	tmp->type = IS_STRING;
	Z_UNSET_ISREF_P(tmp);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(54, ZEND_ADD_CHAR, TMP|UNUSED, CONST)
{
	zend_op *opline = EX(opline);
	zval *str = &EX_T(opline->result.u.var).tmp_var;

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_char_to_string */
		Z_STRVAL_P(str) = NULL;
		Z_STRLEN_P(str) = 0;
		Z_TYPE_P(str) = IS_STRING;

		INIT_PZVAL(str);
	}

	add_char_to_string(str, str, &opline->op2.u.constant);

	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(55, ZEND_ADD_STRING, TMP|UNUSED, CONST)
{
	zend_op *opline = EX(opline);
	zval *str = &EX_T(opline->result.u.var).tmp_var;

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_string_to_string */
		Z_STRVAL_P(str) = NULL;
		Z_STRLEN_P(str) = 0;
		Z_TYPE_P(str) = IS_STRING;

		INIT_PZVAL(str);
	}

	add_string_to_string(str, str, &opline->op2.u.constant);

	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(56, ZEND_ADD_VAR, TMP|UNUSED, TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zval *str = &EX_T(opline->result.u.var).tmp_var;
	zval *var = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval var_copy;
	int use_copy = 0;

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_string_to_string */
		Z_STRVAL_P(str) = NULL;
		Z_STRLEN_P(str) = 0;
		Z_TYPE_P(str) = IS_STRING;

		INIT_PZVAL(str);
	}

	if (Z_TYPE_P(var) != IS_STRING) {
		zend_make_printable_zval(var, &var_copy, &use_copy);

		if (use_copy) {
			var = &var_copy;
		}
	}
	add_string_to_string(str, str, var);

	if (use_copy) {
		zval_dtor(var);
	}
	/* original comment, possibly problematic:
	 * FREE_OP is missing intentionally here - we're always working on the same temporary variable
	 * (Zeev):  I don't think it's problematic, we only use variables
	 * which aren't affected by FREE_OP(Ts, )'s anyway, unless they're
	 * string offsets or overloaded objects
	 */
	FREE_OP2();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(109, ZEND_FETCH_CLASS, ANY, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);


	if (OP2_TYPE == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_free_op free_op2;
		zval *class_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (OP2_TYPE != IS_CONST && Z_TYPE_P(class_name) == IS_OBJECT) {
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
		} else if (Z_TYPE_P(class_name) == IS_STRING) {
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
		}

		FREE_OP2();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(112, ZEND_INIT_METHOD_CALL, TMP|VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(called_scope));

	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(function_name)!=IS_STRING) {
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	function_name_strval = Z_STRVAL_P(function_name);
	function_name_strlen = Z_STRLEN_P(function_name);

	EX(object) = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);

	if (EX(object) && Z_TYPE_P(EX(object)) == IS_OBJECT) {
		if (Z_OBJ_HT_P(EX(object))->get_method == NULL) {
			zend_error_noreturn(E_ERROR, "Object does not support method calls");
		}

		/* First, locate the function. */
		EX(fbc) = Z_OBJ_HT_P(EX(object))->get_method(&EX(object), function_name_strval, function_name_strlen TSRMLS_CC);
		if (!EX(fbc)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(EX(object)), function_name_strval);
		}
		
		EX(called_scope) = Z_OBJCE_P(EX(object));
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}
	
	if ((EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) != 0) {
		EX(object) = NULL;
	} else {		
		if (!PZVAL_IS_REF(EX(object))) {
			Z_ADDREF_P(EX(object)); /* For $this pointer */
		} else {
			zval *this_ptr;
			ALLOC_ZVAL(this_ptr);
			INIT_PZVAL_COPY(this_ptr, EX(object));
			zval_copy_ctor(this_ptr);
			EX(object) = this_ptr;
		}
	}

	FREE_OP2();
	FREE_OP1_IF_VAR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, CONST|VAR, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(called_scope));

	if (OP1_TYPE == IS_CONST) {
		/* no function found. try a static method in class */
		ce = zend_fetch_class(Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant), opline->extended_value TSRMLS_CC);
		if (UNEXPECTED(EG(exception) != NULL)) {
			ZEND_VM_CONTINUE();
		}
		if (!ce) {
			zend_error_noreturn(E_ERROR, "Class '%s' not found", Z_STRVAL(opline->op1.u.constant));
		}
		EX(called_scope) = ce;
	} else {
		ce = EX_T(opline->op1.u.var).class_entry;
		
		if (opline->op1.u.EA.type == ZEND_FETCH_CLASS_PARENT || opline->op1.u.EA.type == ZEND_FETCH_CLASS_SELF) {
			EX(called_scope) = EG(called_scope);
		} else {
			EX(called_scope) = ce;
		}
	}
	if(OP2_TYPE != IS_UNUSED) {
		char *function_name_strval = NULL;
		int function_name_strlen = 0;
		zend_free_op free_op2;

		if (OP2_TYPE == IS_CONST) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			} else {
				function_name_strval = Z_STRVAL_P(function_name);
				function_name_strlen = Z_STRLEN_P(function_name);
 			}
		}

		if (function_name_strval) {
			if (ce->get_static_method) {
				EX(fbc) = ce->get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);
			} else {
				EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);
			}
			if (!EX(fbc)) {
				zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", ce->name, function_name_strval);
			}
		}

		if (OP2_TYPE != IS_CONST) {
			FREE_OP2();
		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Cannot call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			int severity;
			char *verb;
			if (EX(fbc)->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				verb = "should not";
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				severity = E_ERROR;
				verb = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from incompatible context", EX(fbc)->common.scope->name, EX(fbc)->common.function_name, verb);

		}
		if ((EX(object) = EG(This))) {
			Z_ADDREF_P(EX(object));
			EX(called_scope) = Z_OBJCE_P(EX(object));
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval, *lcname;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(called_scope));

	if (OP2_TYPE == IS_CONST) {
		if (zend_hash_quick_find(EG(function_table), Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant)+1, opline->extended_value, (void **) &EX(fbc)) == FAILURE) {
			zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL(opline->op2.u.constant));
		}
	} else {
		function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (OP2_TYPE != IS_CONST && OP2_TYPE != IS_TMP_VAR &&
		    Z_TYPE_P(function_name) == IS_OBJECT &&
			Z_OBJ_HANDLER_P(function_name, get_closure) &&
			Z_OBJ_HANDLER_P(function_name, get_closure)(function_name, &EX(called_scope), &EX(fbc), &EX(object) TSRMLS_CC) == SUCCESS) {
			if (EX(object)) {
				Z_ADDREF_P(EX(object));
			}
			if (OP2_TYPE == IS_VAR && OP2_FREE &&
			    EX(fbc)->common.fn_flags & ZEND_ACC_CLOSURE) {
				/* Delay closure destruction until its invocation */
				EX(fbc)->common.prototype = (zend_function*)function_name;
			} else {
				FREE_OP2();
			}
			ZEND_VM_NEXT_OPCODE();
		}

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = Z_STRVAL_P(function_name);
		function_name_strlen = Z_STRLEN_P(function_name);
		if (function_name_strval[0] == '\\') {

		    function_name_strlen -= 1;
			lcname = zend_str_tolower_dup(function_name_strval + 1, function_name_strlen);
		} else {
			lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
		}
		if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &EX(fbc)) == FAILURE) {
			zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
		}
		efree(lcname);
		FREE_OP2();
	}

	EX(object) = NULL;
	ZEND_VM_NEXT_OPCODE();
}


ZEND_VM_HANDLER(69, ZEND_INIT_NS_FCALL_BY_NAME, ANY, CONST)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline + 1;

	ZEND_VM_INC_OPCODE();
	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(called_scope));

	if (zend_hash_quick_find(EG(function_table), Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant)+1, opline->extended_value, (void **) &EX(fbc))==FAILURE) {
		char *short_name = Z_STRVAL(opline->op1.u.constant)+Z_LVAL(op_data->op1.u.constant);
		if (zend_hash_quick_find(EG(function_table), short_name, Z_STRLEN(opline->op1.u.constant)-Z_LVAL(op_data->op1.u.constant)+1, op_data->extended_value, (void **) &EX(fbc))==FAILURE) {
			zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL(opline->op2.u.constant));
		}
	}

	EX(object) = NULL;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_leave_helper, ANY, ANY)
{
	zend_bool nested;
	zend_op_array *op_array = EX(op_array);

	EG(current_execute_data) = EX(prev_execute_data);
	EG(opline_ptr) = NULL;
	if (!EG(active_symbol_table)) {
		zval ***cv = EX(CVs);
		zval ***end = cv + EX(op_array)->last_var;
		while (cv != end) {
			if (*cv) {
				zval_ptr_dtor(*cv);
			}
			cv++;
		}
	}
	
	if ((op_array->fn_flags & ZEND_ACC_CLOSURE) && op_array->prototype) {
		zval_ptr_dtor((zval**)&op_array->prototype);
	}

	nested = EX(nested);
	
	zend_vm_stack_free(execute_data TSRMLS_CC);

	if (nested) {
		execute_data = EG(current_execute_data);

		if (EX(call_opline)->opcode == ZEND_INCLUDE_OR_EVAL) {

			EX(function_state).function = (zend_function *) EX(op_array);
			EX(function_state).arguments = NULL;
			EX(object) = EX(current_object);

			if (RETURN_VALUE_USED(EX(call_opline))) {
				if (!EX_T(EX(call_opline)->result.u.var).var.ptr) { /* there was no return statement */
					ALLOC_ZVAL(EX_T(EX(call_opline)->result.u.var).var.ptr);
					INIT_PZVAL(EX_T(EX(call_opline)->result.u.var).var.ptr);
					Z_LVAL_P(EX_T(EX(call_opline)->result.u.var).var.ptr) = 1;
					Z_TYPE_P(EX_T(EX(call_opline)->result.u.var).var.ptr) = IS_BOOL;
				}
			}

			EG(opline_ptr) = &EX(opline);
			EG(active_op_array) = EX(op_array);
			EG(return_value_ptr_ptr) = EX(original_return_value);
			destroy_op_array(op_array TSRMLS_CC);
			efree(op_array);
			if (EG(exception)) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
			}

			EX(opline)++;
			ZEND_VM_LEAVE();
		} else {

			EG(opline_ptr) = &EX(opline);
			EG(active_op_array) = EX(op_array);
			EG(return_value_ptr_ptr) = EX(original_return_value);
			if (EG(active_symbol_table)) {
				if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
					zend_hash_destroy(EG(active_symbol_table));
					FREE_HASHTABLE(EG(active_symbol_table));
				} else {
					/* clean before putting into the cache, since clean
					   could call dtors, which could use cached hash */
					zend_hash_clean(EG(active_symbol_table));
					*(++EG(symtable_cache_ptr)) = EG(active_symbol_table);
				}
			}
			EG(active_symbol_table) = EX(symbol_table);

			EX(function_state).function = (zend_function *) EX(op_array);
			EX(function_state).arguments = NULL;

			if (EG(This)) {
				if (EG(exception) && IS_CTOR_CALL(EX(called_scope))) {
					if (IS_CTOR_USED(EX(called_scope))) {
						Z_DELREF_P(EG(This));
					}
					if (Z_REFCOUNT_P(EG(This)) == 1) {
						zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
					}
				}
				zval_ptr_dtor(&EG(This));
			}
			EG(This) = EX(current_this);
			EG(scope) = EX(current_scope);
			EG(called_scope) = EX(current_called_scope);

			EX(object) = EX(current_object);
			EX(called_scope) = DECODE_CTOR(EX(called_scope));

			zend_vm_stack_clear_multiple(TSRMLS_C);

			if (EG(exception)) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				if (RETURN_VALUE_USED(EX(call_opline)) && EX_T(EX(call_opline)->result.u.var).var.ptr) {
					zval_ptr_dtor(&EX_T(EX(call_opline)->result.u.var).var.ptr);
				}
			}

			EX(opline)++;
			ZEND_VM_LEAVE();
		}
	}
	ZEND_VM_RETURN();
}

ZEND_VM_HELPER(zend_do_fcall_common_helper, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zend_bool should_change_scope = 0;

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
			ZEND_VM_NEXT_OPCODE(); /* Never reached */
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name);
		}
	}
	if (EX(function_state).function->common.scope &&
		!(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC) &&
		!EX(object)) {

		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
			/* FIXME: output identifiers properly */
			zend_error(E_STRICT, "Non-static method %s::%s() should not be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		} else {
			/* FIXME: output identifiers properly */
			/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
			zend_error_noreturn(E_ERROR, "Non-static method %s::%s() cannot be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
		}
	}

	if (EX(function_state).function->type == ZEND_USER_FUNCTION ||
	    EX(function_state).function->common.scope) {
		should_change_scope = 1;
		EX(current_this) = EG(This);
		EX(current_scope) = EG(scope);
		EX(current_called_scope) = EG(called_scope);
		EG(This) = EX(object);
		EG(scope) = (EX(function_state).function->type == ZEND_USER_FUNCTION || !EX(object)) ? EX(function_state).function->common.scope : NULL;
		EG(called_scope) = EX(called_scope);
	}

	zend_arg_types_stack_3_pop(&EG(arg_types_stack), &EX(called_scope), &EX(current_object), &EX(fbc));
	EX(function_state).arguments = zend_vm_stack_push_args(opline->extended_value TSRMLS_CC);

	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		ALLOC_INIT_ZVAL(EX_T(opline->result.u.var).var.ptr);
		EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
		EX_T(opline->result.u.var).var.fcall_returned_reference = EX(function_state).function->common.return_reference;

		if (EX(function_state).function->common.arg_info) {
			zend_uint i=0;
			zval **p = (zval**)EX(function_state).arguments;
			ulong arg_count = opline->extended_value;

			while (arg_count>0) {
				zend_verify_arg_type(EX(function_state).function, ++i, *(p-arg_count), 0 TSRMLS_CC);
				arg_count--;
			}
		}
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			((zend_internal_function *) EX(function_state).function)->handler(opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(function_state).function->common.return_reference?&EX_T(opline->result.u.var).var.ptr:NULL, EX(object), RETURN_VALUE_USED(opline) TSRMLS_CC);
		} else {
			zend_execute_internal(EXECUTE_DATA, RETURN_VALUE_USED(opline) TSRMLS_CC);
		}

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	} else if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		EX(original_return_value) = EG(return_value_ptr_ptr);
		EG(active_symbol_table) = NULL;
		EG(active_op_array) = &EX(function_state).function->op_array;
		EG(return_value_ptr_ptr) = NULL;
		if (RETURN_VALUE_USED(opline)) {			
			EG(return_value_ptr_ptr) = &EX_T(opline->result.u.var).var.ptr;
			EX_T(opline->result.u.var).var.ptr = NULL;
			EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
			EX_T(opline->result.u.var).var.fcall_returned_reference = EX(function_state).function->common.return_reference;
		}

		if (zend_execute == execute && !EG(exception)) {
			EX(call_opline) = opline;
			ZEND_VM_ENTER();
		} else {
			zend_execute(EG(active_op_array) TSRMLS_CC);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(return_value_ptr_ptr) = EX(original_return_value);
		if (EG(active_symbol_table)) {
			if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
			} else {
				/* clean before putting into the cache, since clean
				   could call dtors, which could use cached hash */
				zend_hash_clean(EG(active_symbol_table));
				*(++EG(symtable_cache_ptr)) = EG(active_symbol_table);
			}
		}
		EG(active_symbol_table) = EX(symbol_table);
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_INIT_ZVAL(EX_T(opline->result.u.var).var.ptr);

			/* Not sure what should be done here if it's a static method */
		if (EX(object)) {
			Z_OBJ_HT_P(EX(object))->call_method(EX(function_state).function->common.function_name, opline->extended_value, EX_T(opline->result.u.var).var.ptr, &EX_T(opline->result.u.var).var.ptr, EX(object), RETURN_VALUE_USED(opline) TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree(EX(function_state).function->common.function_name);
		}
		efree(EX(function_state).function);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			Z_UNSET_ISREF_P(EX_T(opline->result.u.var).var.ptr);
			Z_SET_REFCOUNT_P(EX_T(opline->result.u.var).var.ptr, 1);
			EX_T(opline->result.u.var).var.fcall_returned_reference = 0;
			EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
		}
	}

	EX(function_state).function = (zend_function *) EX(op_array);
	EX(function_state).arguments = NULL;

	if (should_change_scope) {
		if (EG(This)) {
			if (EG(exception) && IS_CTOR_CALL(EX(called_scope))) {
				if (IS_CTOR_USED(EX(called_scope))) {
					Z_DELREF_P(EG(This));
				}
				if (Z_REFCOUNT_P(EG(This)) == 1) {
					zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
				}
			}
			zval_ptr_dtor(&EG(This));
		}
		EG(This) = EX(current_this);
		EG(scope) = EX(current_scope);
		EG(called_scope) = EX(current_called_scope);
	}

	EX(object) = EX(current_object);
	EX(called_scope) = DECODE_CTOR(EX(called_scope));

	zend_vm_stack_clear_multiple(TSRMLS_C);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (RETURN_VALUE_USED(opline) && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(61, ZEND_DO_FCALL_BY_NAME, ANY, ANY)
{
	EX(function_state).function = EX(fbc);
	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}

ZEND_VM_HANDLER(60, ZEND_DO_FCALL, CONST, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *fname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), EX(called_scope));

	if (zend_hash_quick_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, Z_LVAL(opline->op2.u.constant), (void **) &EX(function_state).function)==FAILURE) {
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", fname->value.str.val);
	}
	EX(object) = NULL;

	FREE_OP1();

	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}

ZEND_VM_HANDLER(62, ZEND_RETURN, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval *retval_ptr;
	zval **retval_ptr_ptr;
	zend_free_op free_op1;

	if (EG(active_op_array)->return_reference == ZEND_RETURN_REF) {

		if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
			ZEND_VM_C_GOTO(return_by_value);
		}

		retval_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

		if (OP1_TYPE == IS_VAR && !retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (OP1_TYPE == IS_VAR && !Z_ISREF_PP(retval_ptr_ptr)) {
			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			    EX_T(opline->op1.u.var).var.fcall_returned_reference) {
			} else if (EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
				if (OP1_TYPE == IS_VAR && !OP1_FREE) {
					PZVAL_LOCK(*retval_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
				}
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				ZEND_VM_C_GOTO(return_by_value);
			}
		}

		if (EG(return_value_ptr_ptr)) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
			Z_ADDREF_PP(retval_ptr_ptr);

			(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
		}
	} else {
ZEND_VM_C_LABEL(return_by_value):

		retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (!EG(return_value_ptr_ptr)) {
			if (OP1_TYPE == IS_TMP_VAR) {
				FREE_OP1();
			}
		} else if (!IS_OP1_TMP_FREE()) { /* Not a temp var */
			if (OP1_TYPE == IS_CONST ||
			    EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && Z_REFCOUNT_P(retval_ptr) > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else if ((OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) &&
			           retval_ptr == &EG(uninitialized_zval)) {
				zval *ret;

				ALLOC_INIT_ZVAL(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				Z_ADDREF_P(retval_ptr);
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_HANDLER(108, ZEND_THROW, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST || Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	zend_exception_save(TSRMLS_C);
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);
	zend_exception_restore(TSRMLS_C);
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, ANY, CV)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce;

	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore(TSRMLS_C);
	if (EG(exception) == NULL) {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
	ce = Z_OBJCE_P(EG(exception));
	if (ce != EX_T(opline->op1.u.var).class_entry) {
		if (!instanceof_function(ce, EX_T(opline->op1.u.var).class_entry TSRMLS_CC)) {
			if (opline->op1.u.EA.type) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				ZEND_VM_NEXT_OPCODE();
			}
			ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
			ZEND_VM_CONTINUE(); /* CHECK_ME */
		}
	}

	if (!EG(active_symbol_table)) {
		if (EX(CVs)[opline->op2.u.var]) {
			zval_ptr_dtor(EX(CVs)[opline->op2.u.var]);
		}
		EX(CVs)[opline->op2.u.var] = (zval**)EX(CVs) + (EX(op_array)->last_var + opline->op2.u.var);
		*EX(CVs)[opline->op2.u.var] = EG(exception);
	} else {
		zend_compiled_variable *cv = &CV_DEF_OF(opline->op2.u.var);
		zend_hash_quick_update(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value,
		    &EG(exception), sizeof(zval *), (void**)&EX(CVs)[opline->op2.u.var]);
	}
	EG(exception) = NULL;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(65, ZEND_SEND_VAL, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	if (opline->extended_value==ZEND_DO_FCALL_BY_NAME
		&& ARG_MUST_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", opline->op2.u.opline_num);
	}
	{
		zval *valptr;
		zval *value;
		zend_free_op free_op1;

		value = GET_OP1_ZVAL_PTR(BP_VAR_R);

		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, value);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(valptr);
		}
		zend_vm_stack_push(valptr TSRMLS_CC);
		FREE_OP1_IF_VAR();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_send_by_var_helper, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval *varptr;
	zend_free_op free_op1;
	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (varptr == &EG(uninitialized_zval)) {
		ALLOC_ZVAL(varptr);
		INIT_ZVAL(*varptr);
		Z_SET_REFCOUNT_P(varptr, 0);
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		Z_UNSET_ISREF_P(varptr);
		Z_SET_REFCOUNT_P(varptr, 0);
		zval_copy_ctor(varptr);
	}
	Z_ADDREF_P(varptr);
	zend_vm_stack_push(varptr TSRMLS_CC);
	FREE_OP1();  /* for string offsets */

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(106, ZEND_SEND_VAR_NO_REF, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *varptr;

	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
		}
	} else if (!ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
	}

	if (OP1_TYPE == IS_VAR &&
		(opline->extended_value & ZEND_ARG_SEND_FUNCTION) &&
		EX_T(opline->op1.u.var).var.fcall_returned_reference &&
		EX_T(opline->op1.u.var).var.ptr) {
		varptr = EX_T(opline->op1.u.var).var.ptr;
		PZVAL_UNLOCK_EX(varptr, &free_op1, 0);
	} else {
		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	}
	if ((!(opline->extended_value & ZEND_ARG_SEND_FUNCTION) ||
	     EX_T(opline->op1.u.var).var.fcall_returned_reference) &&
	    varptr != &EG(uninitialized_zval) &&
	    (PZVAL_IS_REF(varptr) ||
	     (Z_REFCOUNT_P(varptr) == 1 && (OP1_TYPE == IS_CV || free_op1.var)))) {
		Z_SET_ISREF_P(varptr);
		Z_ADDREF_P(varptr);
		zend_vm_stack_push(varptr TSRMLS_CC);
	} else {
		zval *valptr;

		if ((opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) ?
			!(opline->extended_value & ZEND_ARG_SEND_SILENT) :
			!ARG_MAY_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
			zend_error(E_STRICT, "Only variables should be passed by reference");
		}
		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, varptr);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(valptr);
		}
		zend_vm_stack_push(valptr TSRMLS_CC);
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(67, ZEND_SEND_REF, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **varptr_ptr;
	zval *varptr;
	varptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && !varptr_ptr) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	if (OP1_TYPE == IS_VAR && *varptr_ptr == EG(error_zval_ptr)) {
		ALLOC_INIT_ZVAL(varptr);
		zend_vm_stack_push(varptr TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION && !ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	Z_ADDREF_P(varptr);
	zend_vm_stack_push(varptr TSRMLS_CC);

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(66, ZEND_SEND_VAR, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);

	if ((opline->extended_value == ZEND_DO_FCALL_BY_NAME)
		&& ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->op2.u.opline_num)) {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
}

ZEND_VM_HANDLER(63, ZEND_RECV, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);
	zval **param = zend_vm_stack_get_arg(arg_num TSRMLS_CC);

	if (param == NULL) {
		char *space;
		char *class_name = get_active_class_name(&space TSRMLS_CC);
		zend_execute_data *ptr = EX(prev_execute_data);

		if (zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL, opline->extended_value TSRMLS_CC)) {
			if(ptr && ptr->op_array) {
				zend_error(E_WARNING, "Missing argument %ld for %s%s%s(), called in %s on line %d and defined", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C), ptr->op_array->filename, ptr->opline->lineno);
			} else {
				zend_error(E_WARNING, "Missing argument %ld for %s%s%s()", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C));
			}
		}
		if (opline->result.op_type == IS_VAR) {
			PZVAL_UNLOCK_FREE(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_res;
		zval **var_ptr;

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, *param, opline->extended_value TSRMLS_CC);
		var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);
		Z_DELREF_PP(var_ptr);
		*var_ptr = *param;
		Z_ADDREF_PP(var_ptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(64, ZEND_RECV_INIT, ANY, CONST)
{
	zend_op *opline = EX(opline);
	zval *assignment_value;
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);
	zend_free_op free_res;
	zval **param = zend_vm_stack_get_arg(arg_num TSRMLS_CC);
	zval **var_ptr;

	if (param == NULL) {
		ALLOC_ZVAL(assignment_value);
		*assignment_value = opline->op2.u.constant;
		if ((Z_TYPE(opline->op2.u.constant) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT || Z_TYPE(opline->op2.u.constant)==IS_CONSTANT_ARRAY) {
			Z_SET_REFCOUNT_P(assignment_value, 1);
			zval_update_constant(&assignment_value, 0 TSRMLS_CC);
		} else {
			zval_copy_ctor(assignment_value);
		}
		INIT_PZVAL(assignment_value);
	} else {
		assignment_value = *param;
		Z_ADDREF_P(assignment_value);
	}

	zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value, opline->extended_value TSRMLS_CC);
	var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);
	Z_DELREF_PP(var_ptr);
	*var_ptr = assignment_value;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(52, ZEND_BOOL, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	FREE_OP1();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(50, ZEND_BRK, ANY, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(GET_OP2_ZVAL_PTR(BP_VAR_R), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	FREE_OP2();
	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

ZEND_VM_HANDLER(51, ZEND_CONT, ANY, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op2;
	zend_brk_cont_element *el;

	el = zend_brk_cont(GET_OP2_ZVAL_PTR(BP_VAR_R), opline->op1.u.opline_num,
	                   EX(op_array), EX(Ts) TSRMLS_CC);
	FREE_OP2();
	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

ZEND_VM_HANDLER(100, ZEND_GOTO, ANY, CONST)
{
	zend_op *brk_opline;
	zend_op *opline = EX(opline);
	zend_brk_cont_element *el;

	el = zend_brk_cont(&opline->op2.u.constant, opline->extended_value,
 	                   EX(op_array), EX(Ts) TSRMLS_CC);

	brk_opline = EX(op_array)->opcodes + el->brk;

	switch (brk_opline->opcode) {
		case ZEND_SWITCH_FREE:
			if (brk_opline->op1.u.EA.type != EXT_TYPE_FREE_ON_RETURN) {
				zend_switch_free(&EX_T(brk_opline->op1.u.var), brk_opline->extended_value TSRMLS_CC);
			}
			break;
		case ZEND_FREE:
			if (brk_opline->op1.u.EA.type != EXT_TYPE_FREE_ON_RETURN) {
				zendi_zval_dtor(EX_T(brk_opline->op1.u.var).tmp_var);
			}
			break;
	}
	ZEND_VM_JMP(opline->op1.u.jmp_addr);
}

ZEND_VM_HANDLER(48, ZEND_CASE, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	int switch_expr_is_overloaded=0;
	zend_free_op free_op1, free_op2;

	if (OP1_TYPE==IS_VAR) {
		if (EX_T(opline->op1.u.var).var.ptr_ptr) {
			PZVAL_LOCK(EX_T(opline->op1.u.var).var.ptr);
		} else {
			switch_expr_is_overloaded = 1;
			Z_ADDREF_P(EX_T(opline->op1.u.var).str_offset.str);
		}
	}
	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
				 GET_OP1_ZVAL_PTR(BP_VAR_R),
				 GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);

	FREE_OP2();
	if (switch_expr_is_overloaded) {
		/* We only free op1 if this is a string offset,
		 * Since if it is a TMP_VAR, it'll be reused by
		 * other CASE opcodes (whereas string offsets
		 * are allocated at each get_zval_ptr())
		 */
		FREE_OP1();
		EX_T(opline->op1.u.var).var.ptr_ptr = NULL;
		EX_T(opline->op1.u.var).var.ptr = NULL;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(49, ZEND_SWITCH_FREE, VAR, ANY)
{
	zend_op *opline = EX(opline);

	zend_switch_free(&EX_T(opline->op1.u.var), opline->extended_value TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(68, ZEND_NEW, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zval *object_zval;
	zend_function *constructor;

	if (EX_T(opline->op1.u.var).class_entry->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *class_type;

		if (EX_T(opline->op1.u.var).class_entry->ce_flags & ZEND_ACC_INTERFACE) {
			class_type = "interface";
		} else {
			class_type = "abstract class";
		}
		zend_error_noreturn(E_ERROR, "Cannot instantiate %s %s", class_type,  EX_T(opline->op1.u.var).class_entry->name);
	}
	ALLOC_ZVAL(object_zval);
	object_init_ex(object_zval, EX_T(opline->op1.u.var).class_entry);
	INIT_PZVAL(object_zval);

	constructor = Z_OBJ_HT_P(object_zval)->get_constructor(object_zval TSRMLS_CC);

	if (constructor == NULL) {
		if (RETURN_VALUE_USED(opline)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, object_zval);
		} else {
			zval_ptr_dtor(&object_zval);
		}
		ZEND_VM_JMP(EX(op_array)->opcodes + opline->op2.u.opline_num);
	} else {
		if (RETURN_VALUE_USED(opline)) {
			AI_SET_PTR(EX_T(opline->result.u.var).var, object_zval);
			PZVAL_LOCK(object_zval);
		}

		zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), ENCODE_CTOR(EX(called_scope), RETURN_VALUE_USED(opline)));

		/* We are not handling overloaded classes right now */
		EX(object) = object_zval;
		EX(fbc) = constructor;
		EX(called_scope) = EX_T(opline->op1.u.var).class_entry;

		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(110, ZEND_CLONE, CONST|TMP|VAR|UNUSED|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *obj = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE == IS_VAR && !obj) ||
	    Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (!clone_call) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (ce != EG(scope)) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(clone->common.scope, EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name, EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (!EG(exception)) {
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		Z_OBJVAL_P(EX_T(opline->result.u.var).var.ptr) = clone_call(obj TSRMLS_CC);
		Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_OBJECT;
		Z_SET_REFCOUNT_P(EX_T(opline->result.u.var).var.ptr, 1);
		Z_SET_ISREF_P(EX_T(opline->result.u.var).var.ptr);
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(99, ZEND_FETCH_CONSTANT, VAR|CONST|UNUSED, CONST)
{
	zend_op *opline = EX(opline);

	if (OP1_TYPE == IS_UNUSED) {
		/* namespaced constant */
		if (!zend_get_constant_ex(Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant), &EX_T(opline->result.u.var).tmp_var, NULL, opline->extended_value TSRMLS_CC)) {
			if ((opline->extended_value & IS_CONSTANT_UNQUALIFIED) != 0) {
				char *actual = (char *)zend_memrchr(Z_STRVAL(opline->op2.u.constant), '\\', Z_STRLEN(opline->op2.u.constant));
				if(!actual) {
					actual = Z_STRVAL(opline->op2.u.constant);
				} else {
					actual++;
				}
				/* non-qualified constant - allow text substitution */
				zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'", actual, actual);
				ZVAL_STRINGL(&EX_T(opline->result.u.var).tmp_var, actual, Z_STRLEN(opline->op2.u.constant)-(actual - Z_STRVAL(opline->op2.u.constant)), 1);
			} else {
				zend_error_noreturn(E_ERROR, "Undefined constant '%s'",
							Z_STRVAL(opline->op2.u.constant), Z_STRVAL(opline->op2.u.constant));
			}
		}
		ZEND_VM_NEXT_OPCODE();
	} else {
		/* class constant */
		zend_class_entry *ce;
		zval **value;

		if (OP1_TYPE == IS_CONST) {

			ce = zend_fetch_class(Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant), opline->extended_value TSRMLS_CC);
			if (UNEXPECTED(EG(exception) != NULL)) {
				ZEND_VM_CONTINUE();
			}
			if (!ce) {
				zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", Z_STRVAL(opline->op2.u.constant));
			}
		} else {
			ce = EX_T(opline->op1.u.var).class_entry;
		}

		if (zend_hash_find(&ce->constants_table, Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant)+1, (void **) &value) == SUCCESS) {
			if (Z_TYPE_PP(value) == IS_CONSTANT_ARRAY ||
			    (Z_TYPE_PP(value) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
				zend_class_entry *old_scope = EG(scope);

				EG(scope) = ce;
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EG(scope) = old_scope;
			}
			EX_T(opline->result.u.var).tmp_var = **value;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		} else {
			zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", Z_STRVAL(opline->op2.u.constant));
		}

		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(72, ZEND_ADD_ARRAY_ELEMENT, CONST|TMP|VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *array_ptr = &EX_T(opline->result.u.var).tmp_var;
	zval *expr_ptr;
	zval *offset=GET_OP2_ZVAL_PTR(BP_VAR_R);

#if !defined(ZEND_VM_SPEC) || OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV
	zval **expr_ptr_ptr = NULL;

	if (opline->extended_value) {
		expr_ptr_ptr=GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		expr_ptr = *expr_ptr_ptr;
	} else {
		expr_ptr=GET_OP1_ZVAL_PTR(BP_VAR_R);
	}
#else
	expr_ptr=GET_OP1_ZVAL_PTR(BP_VAR_R);
#endif

	if (IS_OP1_TMP_FREE()) { /* temporary variable */
		zval *new_expr;

		ALLOC_ZVAL(new_expr);
		INIT_PZVAL_COPY(new_expr, expr_ptr);
		expr_ptr = new_expr;
	} else {
#if !defined(ZEND_VM_SPEC) || OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV
		if (opline->extended_value) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr_ptr);
			expr_ptr = *expr_ptr_ptr;
			Z_ADDREF_P(expr_ptr);
		} else
#endif
		if (OP1_TYPE == IS_CONST || PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			Z_ADDREF_P(expr_ptr);
		}
	}
	if (offset) {
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), zend_dval_to_lval(Z_DVAL_P(offset)), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_LONG:
			case IS_BOOL:
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), Z_LVAL_P(offset), &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_STRING:
				zend_symtable_update(Z_ARRVAL_P(array_ptr), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, &expr_ptr, sizeof(zval *), NULL);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(array_ptr), "", sizeof(""), &expr_ptr, sizeof(zval *), NULL);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(&expr_ptr);
				/* do nothing */
				break;
		}
		FREE_OP2();
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(array_ptr), &expr_ptr, sizeof(zval *), NULL);
	}
	if (opline->extended_value) {
		FREE_OP1_VAR_PTR();
	} else {
		FREE_OP1_IF_VAR();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);

	array_init(&EX_T(opline->result.u.var).tmp_var);
	if (OP1_TYPE == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if !defined(ZEND_VM_SPEC) || OP1_TYPE != IS_UNUSED
	} else {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ADD_ARRAY_ELEMENT);
#endif
	}
}

ZEND_VM_HANDLER(21, ZEND_CAST, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *result = &EX_T(opline->result.u.var).tmp_var;

	if (opline->extended_value != IS_STRING) {
		*result = *expr;
		if (!IS_OP1_TMP_FREE()) {
			zendi_zval_copy_ctor(*result);
		}
	}
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			break;
		case IS_BOOL:
			convert_to_boolean(result);
			break;
		case IS_LONG:
			convert_to_long(result);
			break;
		case IS_DOUBLE:
			convert_to_double(result);
			break;
		case IS_STRING: {
			zval var_copy;
			int use_copy;

			zend_make_printable_zval(expr, &var_copy, &use_copy);
			if (use_copy) {
				*result = var_copy;
				if (IS_OP1_TMP_FREE()) {
					FREE_OP1();
				}
			} else {
				*result = *expr;
				if (!IS_OP1_TMP_FREE()) {
					zendi_zval_copy_ctor(*result);
				}
			}
			break;
		}
		case IS_ARRAY:
			convert_to_array(result);
			break;
		case IS_OBJECT:
			convert_to_object(result);
			break;
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(73, ZEND_INCLUDE_OR_EVAL, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_op_array *new_op_array=NULL;
	int return_value_used;
	zend_free_op free_op1;
	zval *inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *tmp_inc_filename = NULL;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		MAKE_STD_ZVAL(tmp_inc_filename);
		*tmp_inc_filename = *inc_filename;
		zval_copy_ctor(tmp_inc_filename);
		convert_to_string(tmp_inc_filename);
		inc_filename = tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);
	
	if (Z_LVAL(opline->op2.u.constant) != ZEND_EVAL && strlen(Z_STRVAL_P(inc_filename)) != Z_STRLEN_P(inc_filename)) {
		if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE || Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE) {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
		} else {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
		}
	} else {
		switch (Z_LVAL(opline->op2.u.constant)) {
			case ZEND_INCLUDE_ONCE:
			case ZEND_REQUIRE_ONCE: {
					zend_file_handle file_handle;
					char *resolved_path;

					resolved_path = zend_resolve_path(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename) TSRMLS_CC);
					if (resolved_path) {
						failure_retval = zend_hash_exists(&EG(included_files), resolved_path, strlen(resolved_path)+1);
					} else {
						resolved_path = Z_STRVAL_P(inc_filename);
					}

					if (failure_retval) {
						/* do nothing, file already included */
					} else if (SUCCESS == zend_stream_open(resolved_path, &file_handle TSRMLS_CC)) {

						if (!file_handle.opened_path) {
							file_handle.opened_path = estrdup(resolved_path);
						}

						if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
							new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
							zend_destroy_file_handle(&file_handle TSRMLS_CC);
						} else {
							zend_file_handle_dtor(&file_handle TSRMLS_CC);
							failure_retval=1;
						}
					} else {
						if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
							zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
						} else {
							zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
						}
					}
					if (resolved_path != Z_STRVAL_P(inc_filename)) {
						efree(resolved_path);
					}
				}
				break;
			case ZEND_INCLUDE:
			case ZEND_REQUIRE:
				new_op_array = compile_filename(Z_LVAL(opline->op2.u.constant), inc_filename TSRMLS_CC);
				break;
			case ZEND_EVAL: {
					char *eval_desc = zend_make_compiled_string_description("eval()'d code" TSRMLS_CC);

					new_op_array = zend_compile_string(inc_filename, eval_desc TSRMLS_CC);
					efree(eval_desc);
				}
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	}
	if (tmp_inc_filename) {
		zval_ptr_dtor(&tmp_inc_filename);
	}
	FREE_OP1();
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array && !EG(exception)) {
		EX(original_return_value) = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = return_value_used ? EX_T(opline->result.u.var).var.ptr_ptr : NULL;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		EX(current_object) = EX(object);

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}

		if (zend_execute == execute) {
			EX(call_opline) = opline;
			ZEND_VM_ENTER();
		} else {
			zend_execute(new_op_array TSRMLS_CC);
		}

		EX(function_state).function = (zend_function *) EX(op_array);
		EX(object) = EX(current_object);

		if (return_value_used) {
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(return_value_ptr_ptr) = EX(original_return_value);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (EG(exception)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
		}
	} else {
		if (return_value_used) {
			ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
			INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = failure_retval;
			Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	if (OP1_TYPE == IS_CV && (opline->extended_value & ZEND_QUICK_SET)) {
		if (EG(active_symbol_table)) {
			zend_execute_data *ex = EX(prev_execute_data);
			zend_compiled_variable *cv = &CV_DEF_OF(opline->op1.u.var);

			if (zend_hash_quick_del(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value) == SUCCESS) {
				while (ex && ex->symbol_table == EG(active_symbol_table)) {
					int i;

					if (ex->op_array) {
						for (i = 0; i < ex->op_array->last_var; i++) {
							if (ex->op_array->vars[i].hash_value == cv->hash_value &&
								ex->op_array->vars[i].name_len == cv->name_len &&
								!memcmp(ex->op_array->vars[i].name, cv->name, cv->name_len)) {
								ex->CVs[i] = NULL;
								break;
							}
						}
					}
					ex = ex->prev_execute_data;
				}
			}
			EX(CVs)[opline->op1.u.var] = NULL;
		} else if (EX(CVs)[opline->op1.u.var]) {
			zval_ptr_dtor(EX(CVs)[opline->op1.u.var]);
			EX(CVs)[opline->op1.u.var] = NULL;
		}
		ZEND_VM_NEXT_OPCODE();
	}

	varname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		Z_ADDREF_P(varname);
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_quick_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1, hash_value) == SUCCESS) {
			zend_execute_data *ex = EXECUTE_DATA;

			do {
				int i;

				if (ex->op_array) {
					for (i = 0; i < ex->op_array->last_var; i++) {
						if (ex->op_array->vars[i].hash_value == hash_value &&
							ex->op_array->vars[i].name_len == varname->value.str.len &&
							!memcmp(ex->op_array->vars[i].name, varname->value.str.val, varname->value.str.len)) {
							ex->CVs[i] = NULL;
							break;
						}
					}
				}
				ex = ex->prev_execute_data;
			} while (ex && ex->symbol_table == target_symbol_table);
		}
	}

	if (varname == &tmp) {
		zval_dtor(&tmp);
	} else if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		zval_ptr_dtor(&varname);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(75, ZEND_UNSET_DIM, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	zval *offset;

	if (OP1_TYPE == IS_CV && container != &EG(uninitialized_zval_ptr)) {
		SEPARATE_ZVAL_IF_NOT_REF(container);
	}
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_VAR || container) {
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						zend_hash_index_del(ht, zend_dval_to_lval(Z_DVAL_P(offset)));
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						zend_hash_index_del(ht, Z_LVAL_P(offset));
						break;
					case IS_STRING:
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							Z_ADDREF_P(offset);
						}
						if (zend_symtable_del(ht, offset->value.str.val, offset->value.str.len+1) == SUCCESS &&
					    ht == &EG(symbol_table)) {
							zend_execute_data *ex;
							ulong hash_value = zend_inline_hash_func(offset->value.str.val, offset->value.str.len+1);

							for (ex = EXECUTE_DATA; ex; ex = ex->prev_execute_data) {
								if (ex->op_array && ex->symbol_table == ht) {
									int i;

									for (i = 0; i < ex->op_array->last_var; i++) {
										if (ex->op_array->vars[i].hash_value == hash_value &&
										    ex->op_array->vars[i].name_len == offset->value.str.len &&
										    !memcmp(ex->op_array->vars[i].name, offset->value.str.val, offset->value.str.len)) {
											ex->CVs[i] = NULL;
											break;
										}
									}
								}
							}
						}
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							zval_ptr_dtor(&offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, "", sizeof(""));
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				FREE_OP2();
				break;
			}
			case IS_OBJECT:
				if (!Z_OBJ_HT_P(*container)->unset_dimension) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (IS_OP2_TMP_FREE()) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(*container)->unset_dimension(*container, offset TSRMLS_CC);
				if (IS_OP2_TMP_FREE()) {
					zval_ptr_dtor(&offset);
				} else {
					FREE_OP2();
				}
				break;
			case IS_STRING:
				zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
				ZEND_VM_CONTINUE(); /* bailed out before */
			default:
				FREE_OP2();
				break;
		}
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_VAR || container) {
		if (OP1_TYPE == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (IS_OP2_TMP_FREE()) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (Z_OBJ_HT_P(*container)->unset_property) {
				Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
			} else {
				zend_error(E_NOTICE, "Trying to unset property of non-object");
			}
			if (IS_OP2_TMP_FREE()) {
				zval_ptr_dtor(&offset);
			} else {
				FREE_OP2();
			}
		} else {
			FREE_OP2();
		}
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(77, ZEND_FE_RESET, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array_ptr, **array_ptr_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		array_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
		if (array_ptr_ptr == NULL || array_ptr_ptr == &EG(uninitialized_zval_ptr)) {
			ALLOC_INIT_ZVAL(array_ptr);
		} else if (Z_TYPE_PP(array_ptr_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_PP(array_ptr_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() cannot iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				Z_ADDREF_PP(array_ptr_ptr);
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					Z_SET_ISREF_PP(array_ptr_ptr);
				}
			}
			array_ptr = *array_ptr_ptr;
			Z_ADDREF_P(array_ptr);
		}
	} else {
		array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (IS_OP1_TMP_FREE()) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
			if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
				ce = Z_OBJCE_P(array_ptr);
				if (ce && ce->get_iterator) {
					Z_DELREF_P(array_ptr);
				}
			}
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				Z_ADDREF_P(array_ptr);
			}
		} else if (OP1_TYPE == IS_CONST || 
		           ((OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) &&
		            !Z_ISREF_P(array_ptr) &&
		            Z_REFCOUNT_P(array_ptr) > 1)) {
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			zval_copy_ctor(tmp);
			array_ptr = tmp;
		} else {
			Z_ADDREF_P(array_ptr);
		}
	}

	if (ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (iter && !EG(exception)) {
			array_ptr = zend_iterator_wrap(iter TSRMLS_CC);
		} else {
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				FREE_OP1_VAR_PTR();
			} else {
				FREE_OP1_IF_VAR();
			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			ZEND_VM_NEXT_OPCODE();
		}
	}

	AI_SET_PTR(EX_T(opline->result.u.var).var, array_ptr);
	PZVAL_LOCK(array_ptr);

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				Z_DELREF_P(array_ptr);
				zval_ptr_dtor(&array_ptr);
				if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
					FREE_OP1_VAR_PTR();
				} else {
					FREE_OP1_IF_VAR();
				}
				ZEND_VM_NEXT_OPCODE();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (EG(exception)) {
			Z_DELREF_P(array_ptr);
			zval_ptr_dtor(&array_ptr);
			if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				FREE_OP1_VAR_PTR();
			} else {
				FREE_OP1_IF_VAR();
			}
			ZEND_VM_NEXT_OPCODE();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = zend_objects_get_address(array_ptr TSRMLS_CC);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				char *str_key;
				uint str_key_len;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTANT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
		zend_hash_get_pointer(fe_ht, &EX_T(opline->result.u.var).fe.fe_pos);
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		FREE_OP1_VAR_PTR();
	} else {
		FREE_OP1_IF_VAR();
	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(78, ZEND_FE_FETCH, VAR, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *array = EX_T(opline->op1.u.var).var.ptr;
	zval **value;
	char *str_key;
	uint str_key_len;
	ulong int_key;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	int key_type = 0;
	zend_bool use_key = (zend_bool)(opline->extended_value & ZEND_FE_FETCH_WITH_KEY);

	switch (zend_iterator_unwrap(array, &iter TSRMLS_CC)) {
		default:
		case ZEND_ITER_INVALID:
			zend_error(E_WARNING, "Invalid argument supplied for foreach()");
			ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);

		case ZEND_ITER_PLAIN_OBJECT: {
			char *class_name, *prop_name;
			zend_object *zobj = zend_objects_get_address(array TSRMLS_CC);

			fe_ht = HASH_OF(array);
			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			do {
				if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
					/* reached end of iteration */
					ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
				}
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 0, NULL);

				zend_hash_move_forward(fe_ht);
			} while (key_type == HASH_KEY_NON_EXISTANT ||
			         (key_type != HASH_KEY_IS_LONG &&
			          zend_check_property_access(zobj, str_key, str_key_len-1 TSRMLS_CC) != SUCCESS));
			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			if (use_key && key_type != HASH_KEY_IS_LONG) {
				zend_unmangle_property_name(str_key, str_key_len-1, &class_name, &prop_name);
				str_key_len = strlen(prop_name);
				str_key = estrndup(prop_name, str_key_len);
				str_key_len++;
			}
			break;
		}

		case ZEND_ITER_PLAIN_ARRAY:
			fe_ht = HASH_OF(array);
			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			if (zend_hash_get_current_data(fe_ht, (void **) &value)==FAILURE) {
				/* reached end of iteration */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			if (use_key) {
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &str_key_len, &int_key, 1, NULL);
			}
			zend_hash_move_forward(fe_ht);
			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.u.var).fe.fe_pos);
			break;

		case ZEND_ITER_OBJECT:
			/* !iter happens from exception */
			if (iter && ++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter TSRMLS_CC);
				if (EG(exception)) {
					Z_DELREF_P(array);
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
			}
			/* If index is zero we come from FE_RESET and checked valid() already. */
			if (!iter || (iter->index > 0 && iter->funcs->valid(iter TSRMLS_CC) == FAILURE)) {
				/* reached end of iteration */
				if (EG(exception)) {
					Z_DELREF_P(array);
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			iter->funcs->get_current_data(iter, &value TSRMLS_CC);
			if (EG(exception)) {
				Z_DELREF_P(array);
				zval_ptr_dtor(&array);
				ZEND_VM_NEXT_OPCODE();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			if (use_key) {
				if (iter->funcs->get_current_key) {
					key_type = iter->funcs->get_current_key(iter, &str_key, &str_key_len, &int_key TSRMLS_CC);
					if (EG(exception)) {
						Z_DELREF_P(array);
						zval_ptr_dtor(&array);
						ZEND_VM_NEXT_OPCODE();
					}
				} else {
					key_type = HASH_KEY_IS_LONG;
					int_key = iter->index;
				}
			}
			break;
	}

	if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
		SEPARATE_ZVAL_IF_NOT_REF(value);
		Z_SET_ISREF_PP(value);
		EX_T(opline->result.u.var).var.ptr_ptr = value;
		Z_ADDREF_PP(value);
	} else {
		AI_SET_PTR(EX_T(opline->result.u.var).var, *value);
		PZVAL_LOCK(*value);
	}

	if (use_key) {
		zend_op *op_data = opline+1;
		zval *key = &EX_T(op_data->result.u.var).tmp_var;

		switch (key_type) {
			case HASH_KEY_IS_STRING:
				Z_STRVAL_P(key) = str_key;
				Z_STRLEN_P(key) = str_key_len-1;
				Z_TYPE_P(key) = IS_STRING;
				break;
			case HASH_KEY_IS_LONG:
				Z_LVAL_P(key) = int_key;
				Z_TYPE_P(key) = IS_LONG;
				break;
			default:
			case HASH_KEY_NON_EXISTANT:
				ZVAL_NULL(key);
				break;
		}
	}

	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval **value;
	zend_bool isset = 1;

	if (OP1_TYPE == IS_CV && (opline->extended_value & ZEND_QUICK_SET)) {
		if (EX(CVs)[opline->op1.u.var]) {
			value = EX(CVs)[opline->op1.u.var];
		} else if (EG(active_symbol_table)) {
			zend_compiled_variable *cv = &CV_DEF_OF(opline->op1.u.var);

			if (zend_hash_quick_find(EG(active_symbol_table), cv->name, cv->name_len+1, cv->hash_value, (void **) &value) == FAILURE) {
				isset = 0;
			}
		} else {		
			isset = 0;
		}
	} else {
		HashTable *target_symbol_table;
		zend_free_op free_op1;
		zval tmp, *varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);

		if (Z_TYPE_P(varname) != IS_STRING) {
			tmp = *varname;
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
			varname = &tmp;
		}

		if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
			value = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 1 TSRMLS_CC);
			if (!value) {
				isset = 0;
			}
		} else {
			target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
			if (zend_hash_find(target_symbol_table, varname->value.str.val, varname->value.str.len+1, (void **) &value) == FAILURE) {
				isset = 0;
			}
		}

		if (varname == &tmp) {
			zval_dtor(&tmp);
		}
		FREE_OP1();
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value & ZEND_ISSET_ISEMPTY_MASK) {
		case ZEND_ISSET:
			if (isset && Z_TYPE_PP(value) == IS_NULL) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = isset;
			}
			break;
		case ZEND_ISEMPTY:
			if (!isset || !i_zend_is_true(*value)) {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 1;
			} else {
				Z_LVAL(EX_T(opline->result.u.var).tmp_var) = 0;
			}
			break;
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, VAR|UNUSED|CV, CONST|TMP|VAR|CV, int prop_dim)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_IS);
	zval **value = NULL;
	int result = 0;

	if (OP1_TYPE != IS_VAR || container) {
		zend_free_op free_op2;
		zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (Z_TYPE_PP(container) == IS_ARRAY && !prop_dim) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					if (zend_hash_index_find(ht, zend_dval_to_lval(Z_DVAL_P(offset)), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					if (zend_hash_index_find(ht, Z_LVAL_P(offset), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_STRING:
					if (zend_symtable_find(ht, offset->value.str.val, offset->value.str.len+1, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_NULL:
					if (zend_hash_find(ht, "", sizeof(""), (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				default:
					zend_error(E_WARNING, "Illegal offset type in isset or empty");

					break;
			}

			switch (opline->extended_value) {
				case ZEND_ISSET:
					if (isset && Z_TYPE_PP(value) == IS_NULL) {
						result = 0;
					} else {
						result = isset;
					}
					break;
				case ZEND_ISEMPTY:
					if (!isset || !i_zend_is_true(*value)) {
						result = 0;
					} else {
						result = 1;
					}
					break;
			}
			FREE_OP2();
		} else if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (IS_OP2_TMP_FREE()) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (prop_dim) {
				if (Z_OBJ_HT_P(*container)->has_property) {
					result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
				} else {
					zend_error(E_NOTICE, "Trying to check property of non-object");
					result = 0;
				}
			} else {
				if (Z_OBJ_HT_P(*container)->has_dimension) {
					result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
				} else {
					zend_error(E_NOTICE, "Trying to check element of non-array");
					result = 0;
				}
			}
			if (IS_OP2_TMP_FREE()) {
				zval_ptr_dtor(&offset);
			} else {
				FREE_OP2();
			}
		} else if ((*container)->type == IS_STRING && !prop_dim) { /* string offsets */
			zval tmp;

			if (Z_TYPE_P(offset) != IS_LONG) {
				tmp = *offset;
				zval_copy_ctor(&tmp);
				convert_to_long(&tmp);
				offset = &tmp;
			}
			if (Z_TYPE_P(offset) == IS_LONG) {
				switch (opline->extended_value) {
					case ZEND_ISSET:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container)) {
							result = 1;
						}
						break;
					case ZEND_ISEMPTY:
						if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_PP(container) && Z_STRVAL_PP(container)[offset->value.lval] != '0') {
							result = 1;
						}
						break;
				}
			}
			FREE_OP2();
		} else {
			FREE_OP2();
		}
	}

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
		case ZEND_ISSET:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = result;
			break;
		case ZEND_ISEMPTY:
			Z_LVAL(EX_T(opline->result.u.var).tmp_var) = !result;
			break;
	}

	FREE_OP1_VAR_PTR();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(115, ZEND_ISSET_ISEMPTY_DIM_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, prop_dim, 0);
}

ZEND_VM_HANDLER(148, ZEND_ISSET_ISEMPTY_PROP_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, prop_dim, 1);
}

ZEND_VM_HANDLER(79, ZEND_EXIT, CONST|TMP|VAR|UNUSED|CV, ANY)
{
#if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	zend_op *opline = EX(opline);
	if (OP1_TYPE != IS_UNUSED) {
		zend_free_op free_op1;
		zval *ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (Z_TYPE_P(ptr) == IS_LONG) {
			EG(exit_status) = Z_LVAL_P(ptr);
		} else {
			zend_print_variable(ptr);
		}
		FREE_OP1();
	}
#endif
	zend_bailout();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(57, ZEND_BEGIN_SILENCE, ANY, ANY)
{
	zend_op *opline = EX(opline);

	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = EG(error_reporting);
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_LONG;  /* shouldn't be necessary */
	if (EX(old_error_reporting) == NULL) {
		EX(old_error_reporting) = &EX_T(opline->result.u.var).tmp_var;
	}

	if (EG(error_reporting)) {
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1 TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(142, ZEND_RAISE_ABSTRACT_ERROR, ANY, ANY)
{
	zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EG(scope)->name, EX(op_array)->function_name);
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}

ZEND_VM_HANDLER(58, ZEND_END_SILENCE, TMP, ANY)
{
	zend_op *opline = EX(opline);
	zval restored_error_reporting;

	if (!EG(error_reporting) && Z_LVAL(EX_T(opline->op1.u.var).tmp_var) != 0) {
		Z_TYPE(restored_error_reporting) = IS_LONG;
		Z_LVAL(restored_error_reporting) = Z_LVAL(EX_T(opline->op1.u.var).tmp_var);
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1 TSRMLS_CC);
		zendi_zval_dtor(restored_error_reporting);
	}
	if (EX(old_error_reporting) == &EX_T(opline->op1.u.var).tmp_var) {
		EX(old_error_reporting) = NULL;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (i_zend_is_true(value)) {
		EX_T(opline->result.u.var).tmp_var = *value;
		zendi_zval_copy_ctor(EX_T(opline->result.u.var).tmp_var);
		FREE_OP1();
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(22, ZEND_QM_ASSIGN, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	EX_T(opline->result.u.var).tmp_var = *value;
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(101, ZEND_EXT_STMT, ANY, ANY)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(102, ZEND_EXT_FCALL_BEGIN, ANY, ANY)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(103, ZEND_EXT_FCALL_END, ANY, ANY)
{
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, EX(op_array) TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(139, ZEND_DECLARE_CLASS, ANY, ANY)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_class(opline, EG(class_table), 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(140, ZEND_DECLARE_INHERITED_CLASS, ANY, ANY)
{
	zend_op *opline = EX(opline);

	EX_T(opline->result.u.var).class_entry = do_bind_inherited_class(opline, EG(class_table), EX_T(opline->extended_value).class_entry, 0 TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_INHERITED_CLASS_DELAYED, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zend_class_entry **pce, **pce_orig;

	if (zend_hash_find(EG(class_table), Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant)+1, (void**)&pce) == FAILURE ||
	    (zend_hash_find(EG(class_table), Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant), (void**)&pce_orig) == SUCCESS &&
	     *pce != *pce_orig)) {
		do_bind_inherited_class(opline, EG(class_table), EX_T(opline->extended_value).class_entry, 0 TSRMLS_CC);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(141, ZEND_DECLARE_FUNCTION, ANY, ANY)
{
	do_bind_function(EX(opline), EG(function_table), 0);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(105, ZEND_TICKS, CONST, ANY)
{
	zend_op *opline = EX(opline);

	if (++EG(ticks_count)>=Z_LVAL(opline->op1.u.constant)) {
		EG(ticks_count)=0;
		if (zend_ticks_function) {
			zend_ticks_function(Z_LVAL(opline->op1.u.constant));
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval *expr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zend_bool result;

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), EX_T(opline->op2.u.var).class_entry TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(&EX_T(opline->result.u.var).tmp_var, result);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(104, ZEND_EXT_NOP, ANY, ANY)
{
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(0, ZEND_NOP, ANY, ANY)
{
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(144, ZEND_ADD_INTERFACE, ANY, CONST)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = EX_T(opline->op1.u.var).class_entry;
	zend_class_entry *iface = zend_fetch_class(Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant), opline->extended_value TSRMLS_CC);

	if (iface) {
		if (!(iface->ce_flags & ZEND_ACC_INTERFACE)) {
			zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name, iface->name);
		}
		zend_do_implement_interface(ce, iface TSRMLS_CC);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	zend_uint catch_op_num;
	int catched = 0;
	zval restored_error_reporting;
 
	void **stack_frame = (void**)(((char*)EX(Ts)) +
		(ZEND_MM_ALIGNED_SIZE(sizeof(temp_variable)) * EX(op_array)->T));

	while (zend_vm_stack_top(TSRMLS_C) != stack_frame) {
		zval *stack_zval_p = zend_vm_stack_pop(TSRMLS_C);
		zval_ptr_dtor(&stack_zval_p);
	}

	for (i=0; i<EG(active_op_array)->last_try_catch; i++) {
		if (EG(active_op_array)->try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (op_num >= EG(active_op_array)->try_catch_array[i].try_op
			&& op_num < EG(active_op_array)->try_catch_array[i].catch_op) {
			catch_op_num = EX(op_array)->try_catch_array[i].catch_op;
			catched = 1;
		}
	}

	while (EX(fbc)) {
		EX(called_scope) = (zend_class_entry*)zend_ptr_stack_pop(&EG(arg_types_stack));
		if (EX(object)) {
			if (IS_CTOR_CALL(EX(called_scope))) {
				if (IS_CTOR_USED(EX(called_scope))) {
					Z_DELREF_P(EX(object));
				}
				if (Z_REFCOUNT_P(EX(object)) == 1) {
					zend_object_store_ctor_failed(EX(object) TSRMLS_CC);
				}
			}
			zval_ptr_dtor(&EX(object));
		}
		EX(called_scope) = DECODE_CTOR(EX(called_scope));
		zend_arg_types_stack_2_pop(&EG(arg_types_stack), &EX(object), &EX(fbc));
	}

	for (i=0; i<EX(op_array)->last_brk_cont; i++) {
		if (EX(op_array)->brk_cont_array[i].start < 0) {
			continue;
		} else if (EX(op_array)->brk_cont_array[i].start > op_num) {
			/* further blocks will not be relevant... */
			break;
		} else if (op_num < EX(op_array)->brk_cont_array[i].brk) {
			if (!catched ||
			    catch_op_num >= EX(op_array)->brk_cont_array[i].brk) {
				zend_op *brk_opline = &EX(op_array)->opcodes[EX(op_array)->brk_cont_array[i].brk];

				switch (brk_opline->opcode) {
					case ZEND_SWITCH_FREE:
						if (brk_opline->op1.u.EA.type != EXT_TYPE_FREE_ON_RETURN) {
							zend_switch_free(&EX_T(brk_opline->op1.u.var), brk_opline->extended_value TSRMLS_CC);
						}
						break;
					case ZEND_FREE:
						if (brk_opline->op1.u.EA.type != EXT_TYPE_FREE_ON_RETURN) {
							zendi_zval_dtor(EX_T(brk_opline->op1.u.var).tmp_var);
						}
						break;
				}
			}
		}
	}

	/* restore previous error_reporting value */
	if (!EG(error_reporting) && EX(old_error_reporting) != NULL && Z_LVAL_P(EX(old_error_reporting)) != 0) {
		Z_TYPE(restored_error_reporting) = IS_LONG;
		Z_LVAL(restored_error_reporting) = Z_LVAL_P(EX(old_error_reporting));
		convert_to_string(&restored_error_reporting);
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1 TSRMLS_CC);
		zendi_zval_dtor(restored_error_reporting);
	}
	EX(old_error_reporting) = NULL;

	if (!catched) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
 	} else {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[catch_op_num]);
 		ZEND_VM_CONTINUE();
 	}
}

ZEND_VM_HANDLER(146, ZEND_VERIFY_ABSTRACT_CLASS, ANY, ANY)
{
	zend_verify_abstract_class(EX_T(EX(opline)->op1.u.var).class_entry TSRMLS_CC);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(150, ZEND_USER_OPCODE, ANY, ANY)
{
	int ret = zend_user_opcode_handlers[EX(opline)->opcode](ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_INTERNAL);

	switch (ret) {
		case ZEND_USER_OPCODE_CONTINUE:
			ZEND_VM_CONTINUE();
		case ZEND_USER_OPCODE_RETURN:
			ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
		case ZEND_USER_OPCODE_ENTER:
			ZEND_VM_ENTER();
		case ZEND_USER_OPCODE_LEAVE:
			ZEND_VM_LEAVE();
		case ZEND_USER_OPCODE_DISPATCH:
			ZEND_VM_DISPATCH(EX(opline)->opcode, EX(opline));
		default:
			ZEND_VM_DISPATCH((zend_uchar)(ret & 0xff), EX(opline));
	}
}

ZEND_VM_HANDLER(143, ZEND_DECLARE_CONST, CONST, CONST)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *name  = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *val   = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zend_constant c;

	if ((Z_TYPE_P(val) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT || Z_TYPE_P(val) == IS_CONSTANT_ARRAY) {
		zval tmp = *val;
		zval *tmp_ptr = &tmp;

		if (Z_TYPE_P(val) == IS_CONSTANT_ARRAY) {
			zval_copy_ctor(&tmp);
		}
		INIT_PZVAL(&tmp);
		zval_update_constant(&tmp_ptr, NULL TSRMLS_CC);
		c.value = *tmp_ptr;
	} else {
		c.value = *val;
		zval_copy_ctor(&c.value);
	}
	c.flags = CONST_CS; /* non persistent, case sensetive */
	c.name = zend_strndup(Z_STRVAL_P(name), Z_STRLEN_P(name));
	c.name_len = Z_STRLEN_P(name)+1;
	c.module_number = PHP_USER_CONSTANT;

	if (zend_register_constant(&c TSRMLS_CC) == FAILURE) {
	}

	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(153, ZEND_DECLARE_LAMBDA_FUNCTION, CONST, CONST)
{
	zend_op *opline = EX(opline);
	zend_function *op_array;

	if (zend_hash_quick_find(EG(function_table), Z_STRVAL(opline->op1.u.constant), Z_STRLEN(opline->op1.u.constant), Z_LVAL(opline->op2.u.constant), (void *) &op_array) == FAILURE ||
	    op_array->type != ZEND_USER_FUNCTION) {
		zend_error_noreturn(E_ERROR, "Base lambda function for closure not found");
	}

	zend_create_closure(&EX_T(opline->result.u.var).tmp_var, op_array TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_EXPORT_HELPER(zend_do_fcall, zend_do_fcall_common_helper)
