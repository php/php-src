/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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

	is_not_identical_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(17, ZEND_IS_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(18, ZEND_IS_NOT_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_not_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(19, ZEND_IS_SMALLER, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(20, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;

	is_smaller_or_equal_function(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
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
	zval **retval = &EX_T(result->u.var).var.ptr;
	int have_get_ptr = 0;

	EX_T(result->u.var).var.ptr_ptr = NULL;
	make_real_object(object_ptr TSRMLS_CC);
	object = *object_ptr;

	if (Z_TYPE_P(object) != IS_OBJECT) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP2();
		FREE_OP(free_op_data1);

		if (!RETURN_VALUE_UNUSED(result)) {
			*retval = EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*retval);
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
					*retval = *zptr;
					PZVAL_LOCK(*retval);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;

			switch (opline->extended_value) {
				case ZEND_ASSIGN_OBJ:
					if (Z_OBJ_HT_P(object)->read_property) {
						z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
				case ZEND_ASSIGN_DIM:
					if (Z_OBJ_HT_P(object)->read_dimension) {
						z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R TSRMLS_CC);
					}
					break;
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (z->refcount == 0) {
						zval_dtor(z);
						FREE_ZVAL(z);
					}
					z = value;
				}
				z->refcount++;
				SEPARATE_ZVAL_IF_NOT_REF(&z);
				binary_op(z, z, value TSRMLS_CC);
				switch (opline->extended_value) {
					case ZEND_ASSIGN_OBJ:
						Z_OBJ_HT_P(object)->write_property(object, property, z TSRMLS_CC);
						break;
					case ZEND_ASSIGN_DIM:
						Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
						break;
				}
				if (!RETURN_VALUE_UNUSED(result)) {
					*retval = z;
					PZVAL_LOCK(*retval);
				}
				zval_ptr_dtor(&z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (!RETURN_VALUE_UNUSED(result)) {
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
	zend_bool increment_opline = 0;

	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
			break;
		case ZEND_ASSIGN_DIM: {
				zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);

				if (object_ptr && OP1_TYPE != IS_CV && !OP1_FREE) {
					(*object_ptr)->refcount++;  /* undo the effect of get_obj_zval_ptr_ptr() */
				}

				if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
					ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
				} else {
					zend_op *op_data = opline+1;
					zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

					zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW), dim, IS_OP2_TMP_FREE(), BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
					var_ptr = get_zval_ptr_ptr(&op_data->op2, EX(Ts), &free_op_data2, BP_VAR_RW);
					increment_opline = 1;
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
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
		if (increment_opline) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		objval->refcount++;
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(&objval);
	} else {
		binary_op(*var_ptr, *var_ptr, value TSRMLS_CC);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}
	FREE_OP2();

	if (increment_opline) {
		ZEND_VM_INC_OPCODE();
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

				if (z->refcount == 0) {
					zval_dtor(z);
					FREE_ZVAL(z);
				}
				z = value;
			}
			z->refcount++;
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

				if (z->refcount == 0) {
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
			z->refcount++;
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

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		increment_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		increment_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if(Z_TYPE_PP(var_ptr) == IS_OBJECT && Z_OBJ_HANDLER_PP(var_ptr, get)
	   && Z_OBJ_HANDLER_PP(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_PP(var_ptr, get)(*var_ptr TSRMLS_CC);
		val->refcount++;
		decrement_function(val);
		Z_OBJ_HANDLER_PP(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(&val);
	} else {
		decrement_function(*var_ptr);
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = var_ptr;
		PZVAL_LOCK(*var_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(36, ZEND_POST_INC, VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
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
		val->refcount++;
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

	if (!var_ptr) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (*var_ptr == EG(error_zval_ptr)) {
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
		val->refcount++;
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
	zval z_copy;
	zval *z = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get_method != NULL &&
		zend_std_cast_object_tostring(z, &z_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
		zend_print_variable(&z_copy);
		zval_dtor(&z_copy);
	} else {
		zend_print_variable(z);
	}

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

 	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp_varname = *varname;
		zval_copy_ctor(&tmp_varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		retval = zend_std_get_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname), 0 TSRMLS_CC);
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

						new_zval->refcount++;
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


	if (varname == &tmp_varname) {
		zval_dtor(varname);
	}
	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = retval;
		PZVAL_LOCK(*retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				AI_USE_PTR(EX_T(opline->result.u.var).var);
				break;
			case BP_VAR_UNSET: {
				zend_free_op free_res;

				PZVAL_UNLOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &free_res);
				if (EX_T(opline->result.u.var).var.ptr_ptr != &EG(uninitialized_zval_ptr)) {
					SEPARATE_ZVAL_IF_NOT_REF(EX_T(opline->result.u.var).var.ptr_ptr);
				}
				PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
				FREE_OP_VAR_PTR(free_res);
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

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK &&
	    OP1_TYPE != IS_CV &&
	    EX_T(opline->op1.u.var).var.ptr_ptr) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
	}
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_R), dim, IS_OP2_TMP_FREE(), BP_VAR_R TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(84, ZEND_FETCH_DIM_W, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_W), dim, IS_OP2_TMP_FREE(), BP_VAR_W TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW), dim, IS_OP2_TMP_FREE(), BP_VAR_RW TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
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

	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(BP_VAR_IS), dim, IS_OP2_TMP_FREE(), BP_VAR_IS TSRMLS_CC);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	int type = ARG_SHOULD_BE_SENT_BY_REF(EX(fbc), opline->extended_value)?BP_VAR_W:BP_VAR_R;
	zval *dim;

	if (OP2_TYPE == IS_UNUSED && type == BP_VAR_R) {
		zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
	}
	dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_ZVAL_PTR_PTR(type), dim, IS_OP2_TMP_FREE(), type TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && type == BP_VAR_W && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
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
	zend_fetch_dimension_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, dim, IS_OP2_TMP_FREE(), BP_VAR_UNSET TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
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
	zval *container;
	zval **retval;
	zend_free_op free_op1;
	zend_free_op free_op2;
	zval *offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);

	retval = &EX_T(opline->result.u.var).var.ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = retval;

	container = GET_OP1_OBJ_ZVAL_PTR(type);

	if (container == EG(error_zval_ptr)) {
		if (!RETURN_VALUE_UNUSED(&opline->result)) {
			*retval = EG(error_zval_ptr);
			PZVAL_LOCK(*retval);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
		}
		FREE_OP2();
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE();
	}


	if (Z_TYPE_P(container) != IS_OBJECT || !Z_OBJ_HT_P(container)->read_property) {
		if (type != BP_VAR_IS) {
			zend_error(E_NOTICE, "Trying to get property of non-object");
		}
		*retval = EG(uninitialized_zval_ptr);
		SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		FREE_OP2();
	} else {
		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		*retval = Z_OBJ_HT_P(container)->read_property(container, offset, type TSRMLS_CC);

		if (RETURN_VALUE_UNUSED(&opline->result) && ((*retval)->refcount == 0)) {
			zval_dtor(*retval);
			FREE_ZVAL(*retval);
		} else {
			SELECTIVE_PZVAL_LOCK(*retval, &opline->result);
			AI_USE_PTR(EX_T(opline->result.u.var).var);
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

	if (opline->extended_value == ZEND_FETCH_ADD_LOCK && OP1_TYPE != IS_CV) {
		PZVAL_LOCK(*EX_T(opline->op1.u.var).var.ptr_ptr);
		EX_T(opline->op1.u.var).var.ptr = *EX_T(opline->op1.u.var).var.ptr_ptr;
	}

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W), property, BP_VAR_W TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
			SEPARATE_ZVAL(EX_T(opline->result.u.var).var.ptr_ptr);
		}
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW), property, BP_VAR_RW TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
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

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W), property, BP_VAR_W TSRMLS_CC);
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(&property);
		} else {
			FREE_OP2();
		}
		if (OP1_TYPE == IS_VAR && OP1_FREE &&
		    READY_TO_DESTROY(free_op1.var) &&
		    !RETURN_VALUE_UNUSED(&opline->result)) {
			AI_USE_PTR(EX_T(opline->result.u.var).var);
			if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
			    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
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
	zend_fetch_property_address(RETURN_VALUE_UNUSED(&opline->result)?NULL:&EX_T(opline->result.u.var), container, property, BP_VAR_UNSET TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(&property);
	} else {
		FREE_OP2();
	}
	if (OP1_TYPE == IS_VAR && OP1_FREE &&
	    READY_TO_DESTROY(free_op1.var) &&
	    !RETURN_VALUE_UNUSED(&opline->result)) {
		AI_USE_PTR(EX_T(opline->result.u.var).var);
		if (!PZVAL_IS_REF(*EX_T(opline->result.u.var).var.ptr_ptr) &&
		    ZVAL_REFCOUNT(*EX_T(opline->result.u.var).var.ptr_ptr) > 2) {
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
			EX_T(opline->result.u.var).var.ptr_ptr = &EG(uninitialized_zval_ptr);
			PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_op2;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		EX_T(opline->result.u.var).var.ptr_ptr = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, BP_VAR_R TSRMLS_CC);
		SELECTIVE_PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr, &opline->result);
		FREE_OP2();
	}
	AI_USE_PTR(EX_T(opline->result.u.var).var);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_op *op_data = opline+1;
	zend_free_op free_op1;
	zval **object_ptr = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_W);

	zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_OBJ TSRMLS_CC);
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
	zval **object_ptr;

	if (OP1_TYPE == IS_CV || EX_T(opline->op1.u.var).var.ptr_ptr) {
		/* not an array offset */
		object_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
	} else {
		object_ptr = NULL;
	}

	if (object_ptr && Z_TYPE_PP(object_ptr) == IS_OBJECT) {
		zend_assign_to_object(&opline->result, object_ptr, &opline->op2, &op_data->op1, EX(Ts), ZEND_ASSIGN_DIM TSRMLS_CC);
	} else {
		zend_free_op free_op2, free_op_data1;
		zval *value;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		zend_fetch_dimension_address(&EX_T(op_data->op2.u.var), object_ptr, dim, IS_OP2_TMP_FREE(), BP_VAR_W TSRMLS_CC);
		FREE_OP2();

		value = get_zval_ptr(&op_data->op1, EX(Ts), &free_op_data1, BP_VAR_R);
	 	zend_assign_to_variable(&opline->result, &op_data->op2, &op_data->op1, value, (IS_TMP_FREE(free_op_data1)?IS_TMP_VAR:op_data->op1.op_type), EX(Ts) TSRMLS_CC);
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
	zend_free_op free_op2;
	zval *value = GET_OP2_ZVAL_PTR(BP_VAR_R);

 	zend_assign_to_variable(&opline->result, &opline->op1, &opline->op2, value, (IS_OP2_TMP_FREE()?IS_TMP_VAR:OP2_TYPE), EX(Ts) TSRMLS_CC);
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
	    !(*value_ptr_ptr)->is_ref &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION &&
	    !EX_T(opline->op2.u.var).var.fcall_returned_reference) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(*value_ptr_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (EG(exception)) {
			FREE_OP2_VAR_PTR();
			ZEND_VM_NEXT_OPCODE();
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ASSIGN);
	} else if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(*value_ptr_ptr);
	}
	if (OP1_TYPE == IS_VAR && EX_T(opline->op1.u.var).var.ptr_ptr == &EX_T(opline->op1.u.var).var.ptr) {
		zend_error(E_ERROR, "Cannot assign by reference to overloaded object");
	}

	variable_ptr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
	zend_assign_to_variable_reference(variable_ptr_ptr, value_ptr_ptr TSRMLS_CC);

	if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		(*variable_ptr_ptr)->refcount--;
	}

	if (!RETURN_VALUE_UNUSED(&opline->result)) {
		EX_T(opline->result.u.var).var.ptr_ptr = variable_ptr_ptr;
		PZVAL_LOCK(*variable_ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
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
	int ret = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(44, ZEND_JMPNZ, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int ret = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(45, ZEND_JMPZNZ, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on true to %d\n", opline->extended_value);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->extended_value]);
	} else {
#if DEBUG_ZEND>=2
		printf("Conditional jmp on false to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(&EX(op_array)->opcodes[opline->op2.u.opline_num]);
	}
}

ZEND_VM_HANDLER(46, ZEND_JMPZ_EX, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	int retval = i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R));

	FREE_OP1();
	Z_LVAL(EX_T(opline->result.u.var).tmp_var) = retval;
	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.u.jmp_addr);
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
	tmp->refcount = 1;
	tmp->type = IS_STRING;
	tmp->is_ref = 0;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(54, ZEND_ADD_CHAR, TMP, CONST)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_char_to_string(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(55, ZEND_ADD_STRING, TMP, CONST)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;

	add_string_to_string(&EX_T(opline->result.u.var).tmp_var,
		GET_OP1_ZVAL_PTR(BP_VAR_NA),
		&opline->op2.u.constant);
	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(56, ZEND_ADD_VAR, TMP, TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1, free_op2;
	zval *var = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval var_copy;
	int use_copy = 0;

	if (Z_TYPE_P(var) != IS_STRING) {
		zend_make_printable_zval(var, &var_copy, &use_copy);

		if (use_copy) {
			var = &var_copy;
		}
	}
	add_string_to_string(	&EX_T(opline->result.u.var).tmp_var,
							GET_OP1_ZVAL_PTR(BP_VAR_NA),
							var);
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
	zval *class_name;
	zend_free_op free_op2;


	if (OP2_TYPE == IS_UNUSED) {
		EX_T(opline->result.u.var).class_entry = zend_fetch_class(NULL, 0, opline->extended_value TSRMLS_CC);
		ZEND_VM_NEXT_OPCODE();
	}

	class_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	switch (Z_TYPE_P(class_name)) {
		case IS_OBJECT:
			EX_T(opline->result.u.var).class_entry = Z_OBJCE_P(class_name);
			break;
		case IS_STRING:
			EX_T(opline->result.u.var).class_entry = zend_fetch_class(Z_STRVAL_P(class_name), Z_STRLEN_P(class_name), opline->extended_value TSRMLS_CC);
			break;
		default:
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
			break;
	}

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(112, ZEND_INIT_METHOD_CALL, TMP|VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	char *function_name_strval;
	int function_name_strlen;
	zend_free_op free_op1, free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

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
	} else {
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", function_name_strval);
	}

	if (!EX(object) || (EX(fbc) && (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) != 0)) {
		EX(object) = NULL;
	} else {
		if (!PZVAL_IS_REF(EX(object))) {
			EX(object)->refcount++; /* For $this pointer */
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

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, ANY, CONST|TMP|VAR|UNUSED|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_class_entry *ce;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	ce = EX_T(opline->op1.u.var).class_entry;
	if(OP2_TYPE != IS_UNUSED) {
		char *function_name_strval;
		int function_name_strlen;
		zend_bool is_const = (OP2_TYPE == IS_CONST);
		zend_free_op free_op2;

		if (is_const) {
			function_name_strval = Z_STRVAL(opline->op2.u.constant);
			function_name_strlen = Z_STRLEN(opline->op2.u.constant);
		} else {
			function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

			if (Z_TYPE_P(function_name) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Function name must be a string");
			}
			function_name_strval = zend_str_tolower_dup(function_name->value.str.val, function_name->value.str.len);
			function_name_strlen = function_name->value.str.len;
		}

		EX(fbc) = zend_std_get_static_method(ce, function_name_strval, function_name_strlen TSRMLS_CC);

		if (!is_const) {
			efree(function_name_strval);
			FREE_OP2();
		}
	} else {
		if(!ce->constructor) {
			zend_error_noreturn(E_ERROR, "Can not call constructor");
		}
		if (EG(This) && Z_OBJCE_P(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error(E_COMPILE_ERROR, "Cannot call private %s::__construct()", ce->name);
		}
		EX(fbc) = ce->constructor;
	}

	if (EX(fbc)->common.fn_flags & ZEND_ACC_STATIC) {
		EX(object) = NULL;
	} else {
		if (OP2_TYPE != IS_UNUSED &&
		    EG(This) &&
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
			EX(object)->refcount++;
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST|TMP|VAR|CV)
{
	zend_op *opline = EX(opline);
	zval *function_name;
	zend_function *function;
	char *function_name_strval, *lcname;
	int function_name_strlen;
	zend_free_op free_op2;

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (OP2_TYPE == IS_CONST) {
		function_name_strval = opline->op2.u.constant.value.str.val;
		function_name_strlen = opline->op2.u.constant.value.str.len;
	} else {
		function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (Z_TYPE_P(function_name) != IS_STRING) {
			zend_error_noreturn(E_ERROR, "Function name must be a string");
		}
		function_name_strval = function_name->value.str.val;
		function_name_strlen = function_name->value.str.len;
	}

	lcname = zend_str_tolower_dup(function_name_strval, function_name_strlen);
	if (zend_hash_find(EG(function_table), lcname, function_name_strlen+1, (void **) &function)==FAILURE) {
		efree(lcname);
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", function_name_strval);
	}

	efree(lcname);
	if (OP2_TYPE != IS_CONST) {
		FREE_OP2();
	}

	EX(object) = NULL;

	EX(fbc) = function;

	ZEND_VM_NEXT_OPCODE();
}


ZEND_VM_HELPER(zend_do_fcall_common_helper, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zval **original_return_value;
	zend_class_entry *current_scope = NULL;
	zval *current_this = NULL;
	int return_value_used = RETURN_VALUE_USED(opline);
	zend_bool should_change_scope;
	zend_op *ctor_opline;

	if (EX(function_state).function->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) {
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name);
			ZEND_VM_NEXT_OPCODE(); /* Never reached */
		}
		if (EX(function_state).function->common.fn_flags & ZEND_ACC_DEPRECATED) {
			zend_error(E_STRICT, "Function %s%s%s() is deprecated", 
				EX(function_state).function->common.scope ? EX(function_state).function->common.scope->name : "",
				EX(function_state).function->common.scope ? "::" : "",
				EX(function_state).function->common.function_name);
		}
	}

	zend_ptr_stack_2_push(&EG(argument_stack), (void *)(zend_uintptr_t)opline->extended_value, NULL);

	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (EX(function_state).function->type == ZEND_USER_FUNCTION
		|| EX(function_state).function->common.scope) {
		should_change_scope = 1;
		current_this = EG(This);
		EG(This) = EX(object);
		current_scope = EG(scope);
		EG(scope) = (EX(function_state).function->type == ZEND_USER_FUNCTION || !EX(object)) ? EX(function_state).function->common.scope : NULL;
	} else {
		should_change_scope = 0;
	}

	EX_T(opline->result.u.var).var.fcall_returned_reference = 0;

	if (EX(function_state).function->common.scope) {
		if (!EG(This) && !(EX(function_state).function->common.fn_flags & ZEND_ACC_STATIC)) {
			int severity;
			char *severity_word;
			if (EX(function_state).function->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				severity = E_STRICT;
				severity_word = "should not";
			} else {
				severity = E_ERROR;
				severity_word = "cannot";
			}
			zend_error(severity, "Non-static method %s::%s() %s be called statically", EX(function_state).function->common.scope->name, EX(function_state).function->common.function_name, severity_word);
		}
	}
	if (EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) {
		unsigned char return_reference = EX(function_state).function->common.return_reference;

		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

		if (EX(function_state).function->common.arg_info) {
			zend_uint i=0;
			zval **p;
			ulong arg_count;

			p = (zval **) EG(argument_stack).top_element-2;
			arg_count = (ulong)(zend_uintptr_t) *p;

			while (arg_count>0) {
				zend_verify_arg_type(EX(function_state).function, ++i, *(p-arg_count) TSRMLS_CC);
				arg_count--;
			}
		}
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			((zend_internal_function *) EX(function_state).function)->handler(opline->extended_value, EX_T(opline->result.u.var).var.ptr, EX(function_state).function->common.return_reference?&EX_T(opline->result.u.var).var.ptr:NULL, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_execute_internal(EXECUTE_DATA, return_value_used TSRMLS_CC);
		}

		EG(current_execute_data) = EXECUTE_DATA;

/*	We shouldn't fix bad extensions here,
    because it can break proper ones (Bug #34045)
		if (!EX(function_state).function->common.return_reference) {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
*/
		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.fcall_returned_reference = return_reference;
		}
	} else if (EX(function_state).function->type == ZEND_USER_FUNCTION) {
		EX_T(opline->result.u.var).var.ptr = NULL;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache)) {
			/*printf("Cache hit!  Reusing %x\n", symtable_cache[symtable_cache_ptr]);*/
			EX(function_state).function_symbol_table = *(EG(symtable_cache_ptr)--);
		} else {
			ALLOC_HASHTABLE(EX(function_state).function_symbol_table);
			zend_hash_init(EX(function_state).function_symbol_table, 0, NULL, ZVAL_PTR_DTOR, 0);
			/*printf("Cache miss!  Initialized %x\n", function_state.function_symbol_table);*/
		}
		EG(active_symbol_table) = EX(function_state).function_symbol_table;
		original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = (zend_op_array *) EX(function_state).function;

		zend_execute(EG(active_op_array) TSRMLS_CC);
		EX_T(opline->result.u.var).var.fcall_returned_reference = EG(active_op_array)->return_reference;

		if (return_value_used && !EX_T(opline->result.u.var).var.ptr) {
			if (!EG(exception)) {
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_ZVAL(*EX_T(opline->result.u.var).var.ptr);
			}
		} else if (!return_value_used && EX_T(opline->result.u.var).var.ptr) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(return_value_ptr_ptr)=original_return_value;
		if (EG(symtable_cache_ptr)>=EG(symtable_cache_limit)) {
			zend_hash_destroy(EX(function_state).function_symbol_table);
			FREE_HASHTABLE(EX(function_state).function_symbol_table);
		} else {
			/* clean before putting into the cache, since clean
			   could call dtors, which could use cached hash */
			zend_hash_clean(EX(function_state).function_symbol_table);
			*(++EG(symtable_cache_ptr)) = EX(function_state).function_symbol_table;
		}
		EG(active_symbol_table) = EX(symbol_table);
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
		INIT_ZVAL(*(EX_T(opline->result.u.var).var.ptr));

			/* Not sure what should be done here if it's a static method */
		if (EX(object)) {
			Z_OBJ_HT_P(EX(object))->call_method(EX(fbc)->common.function_name, opline->extended_value, EX_T(opline->result.u.var).var.ptr, &EX_T(opline->result.u.var).var.ptr, EX(object), return_value_used TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (EX(function_state).function->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			efree(EX(function_state).function->common.function_name);
		}
		efree(EX(fbc));

		if (!return_value_used) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		} else {
			EX_T(opline->result.u.var).var.ptr->is_ref = 0;
			EX_T(opline->result.u.var).var.ptr->refcount = 1;
		}
	}

	EX(function_state).function = (zend_function *) EX(op_array);
	EG(function_state_ptr) = &EX(function_state);
	ctor_opline = (zend_op*)zend_ptr_stack_pop(&EG(arg_types_stack));

	if (EG(This)) {
		if (EG(exception) && ctor_opline) {
			if (RETURN_VALUE_USED(ctor_opline)) {
				EG(This)->refcount--;
			}
			if (EG(This)->refcount == 1) {
				zend_object_store_ctor_failed(EG(This) TSRMLS_CC);
			}
		}
		if (should_change_scope) {
			zval_ptr_dtor(&EG(This));
		}
	}

	if (should_change_scope) {
		EG(This) = current_this;
		EG(scope) = current_scope;
	}
	zend_ptr_stack_2_pop(&EG(arg_types_stack), (void**)&EX(object), (void**)&EX(fbc));

	zend_ptr_stack_clear_multiple(TSRMLS_C);

	if (EG(exception)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (return_value_used && EX_T(opline->result.u.var).var.ptr) {
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

	zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), NULL);

	if (zend_hash_find(EG(function_table), fname->value.str.val, fname->value.str.len+1, (void **) &EX(function_state).function)==FAILURE) {
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

		if (!retval_ptr_ptr) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (OP1_TYPE == IS_VAR && !(*retval_ptr_ptr)->is_ref) {
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

		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr_ptr);
		(*retval_ptr_ptr)->refcount++;

		(*EG(return_value_ptr_ptr)) = (*retval_ptr_ptr);
	} else {
ZEND_VM_C_LABEL(return_by_value):

		retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (EG(ze1_compatibility_mode) && Z_TYPE_P(retval_ptr) == IS_OBJECT) {
			zval *ret;
			char *class_name;
			zend_uint class_name_len;
			int dup;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			dup = zend_get_object_classname(retval_ptr, &class_name, &class_name_len TSRMLS_CC);
			if (Z_OBJ_HT_P(retval_ptr)->clone_obj == NULL) {
				zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s",  class_name);
			}
			zend_error(E_STRICT, "Implicit cloning object of class '%s' because of 'zend.ze1_compatibility_mode'", class_name);
			ret->value.obj = Z_OBJ_HT_P(retval_ptr)->clone_obj(retval_ptr TSRMLS_CC);
			*EG(return_value_ptr_ptr) = ret;
			if (!dup) {
				efree(class_name);
			}
		} else if (!IS_OP1_TMP_FREE()) { /* Not a temp var */
			if (EG(active_op_array)->return_reference == ZEND_RETURN_REF ||
			    (PZVAL_IS_REF(retval_ptr) && retval_ptr->refcount > 0)) {
				zval *ret;

				ALLOC_ZVAL(ret);
				INIT_PZVAL_COPY(ret, retval_ptr);
				zval_copy_ctor(ret);
				*EG(return_value_ptr_ptr) = ret;
			} else {
				*EG(return_value_ptr_ptr) = retval_ptr;
				retval_ptr->refcount++;
			}
		} else {
			zval *ret;

			ALLOC_ZVAL(ret);
			INIT_PZVAL_COPY(ret, retval_ptr);
			*EG(return_value_ptr_ptr) = ret;
		}
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
}

ZEND_VM_HANDLER(108, ZEND_THROW, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval *value;
	zval *exception;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(value) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}
	/* Not sure if a complete copy is what we want here */
	ALLOC_ZVAL(exception);
	INIT_PZVAL_COPY(exception, value);
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(exception);
	}

	zend_throw_exception_object(exception TSRMLS_CC);
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce;

	/* Check whether an exception has been thrown, if not, jump over code */
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

	zend_hash_update(EG(active_symbol_table), opline->op2.u.constant.value.str.val,
		opline->op2.u.constant.value.str.len+1, &EG(exception), sizeof(zval *), (void **) NULL);
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
		zend_ptr_stack_push(&EG(argument_stack), valptr);
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
		varptr->refcount = 0;
	} else if (PZVAL_IS_REF(varptr)) {
		zval *original_var = varptr;

		ALLOC_ZVAL(varptr);
		*varptr = *original_var;
		varptr->is_ref = 0;
		varptr->refcount = 0;
		zval_copy_ctor(varptr);
	}
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);
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
	     (varptr->refcount == 1 && (OP1_TYPE == IS_CV || free_op1.var)))) {
		varptr->is_ref = 1;
		varptr->refcount++;
		zend_ptr_stack_push(&EG(argument_stack), varptr);
	} else {
		zval *valptr;

		if (!(opline->extended_value & ZEND_ARG_SEND_SILENT)) {
			zend_error(E_STRICT, "Only variables should be passed by reference");
		}
		ALLOC_ZVAL(valptr);
		INIT_PZVAL_COPY(valptr, varptr);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(valptr);
		}
		zend_ptr_stack_push(&EG(argument_stack), valptr);
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

	if (!varptr_ptr) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr_ptr);
	varptr = *varptr_ptr;
	varptr->refcount++;
	zend_ptr_stack_push(&EG(argument_stack), varptr);

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
	zval **param;
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		char *space;
		char *class_name = get_active_class_name(&space TSRMLS_CC);
		zend_execute_data *ptr = EX(prev_execute_data);

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL TSRMLS_CC);
		if(ptr && ptr->op_array) {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s(), called in %s on line %d and defined", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C), ptr->op_array->filename, ptr->opline->lineno);
		} else {
			zend_error(E_WARNING, "Missing argument %ld for %s%s%s()", opline->op1.u.constant.value.lval, class_name, space, get_active_function_name(TSRMLS_C));
		}
		if (opline->result.op_type == IS_VAR) {
			PZVAL_UNLOCK_FREE(*EX_T(opline->result.u.var).var.ptr_ptr);
		}
	} else {
		zend_free_op free_res;
		zval **var_ptr;

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, *param TSRMLS_CC);
		var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);
		if (PZVAL_IS_REF(*param)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, *param TSRMLS_CC);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(64, ZEND_RECV_INIT, ANY, CONST)
{
	zend_op *opline = EX(opline);
	zval **param, *assignment_value;
	zend_uint arg_num = Z_LVAL(opline->op1.u.constant);
	zend_free_op free_res;

	if (zend_ptr_stack_get_arg(arg_num, (void **) &param TSRMLS_CC)==FAILURE) {
		if (Z_TYPE(opline->op2.u.constant) == IS_CONSTANT || Z_TYPE(opline->op2.u.constant)==IS_CONSTANT_ARRAY) {
			zval *default_value;

			ALLOC_ZVAL(default_value);
			*default_value = opline->op2.u.constant;
			default_value->refcount=1;
			zval_update_constant(&default_value, 0 TSRMLS_CC);
			default_value->refcount=0;
			default_value->is_ref=0;
			param = &default_value;
			assignment_value = default_value;
		} else {
			param = NULL;
			assignment_value = &opline->op2.u.constant;
		}
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		zend_assign_to_variable(NULL, &opline->result, NULL, assignment_value, IS_VAR, EX(Ts) TSRMLS_CC);
	} else {
		zval **var_ptr = get_zval_ptr_ptr(&opline->result, EX(Ts), &free_res, BP_VAR_W);

		assignment_value = *param;
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, assignment_value TSRMLS_CC);
		if (PZVAL_IS_REF(assignment_value)) {
			zend_assign_to_variable_reference(var_ptr, param TSRMLS_CC);
		} else {
			zend_receive(var_ptr, assignment_value TSRMLS_CC);
		}
	}

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
			EX_T(opline->op1.u.var).str_offset.str->refcount++;
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
		AI_USE_PTR(EX_T(opline->op1.u.var).var);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(49, ZEND_SWITCH_FREE, TMP|VAR, ANY)
{
	zend_switch_free(EX(opline), EX(Ts) TSRMLS_CC);
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
			EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
			EX_T(opline->result.u.var).var.ptr = object_zval;
		} else {
			zval_ptr_dtor(&object_zval);
		}
		ZEND_VM_JMP(EX(op_array)->opcodes + opline->op2.u.opline_num);
	} else {
		SELECTIVE_PZVAL_LOCK(object_zval, &opline->result);
		EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
		EX_T(opline->result.u.var).var.ptr = object_zval;

		zend_ptr_stack_3_push(&EG(arg_types_stack), EX(fbc), EX(object), opline);

		/* We are not handling overloaded classes right now */
		EX(object) = object_zval;
		EX(fbc) = constructor;

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

	if (!obj || Z_TYPE_P(obj) != IS_OBJECT) {
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
		FREE_OP1_IF_VAR();
		ZEND_VM_NEXT_OPCODE();
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
		EX_T(opline->result.u.var).var.ptr = EG(error_zval_ptr);
		EX_T(opline->result.u.var).var.ptr->refcount++;
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
		EX_T(opline->result.u.var).var.ptr->refcount=1;
		EX_T(opline->result.u.var).var.ptr->is_ref=1;
		if (!RETURN_VALUE_USED(opline) || EG(exception)) {
			zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
		}
	}
	FREE_OP1_IF_VAR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(99, ZEND_FETCH_CONSTANT, CONST|UNUSED, CONST)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = NULL;
	zval **value;

	if (OP1_TYPE == IS_UNUSED) {
/* This seems to be a reminant of namespaces
		if (EG(scope)) {
			ce = EG(scope);
			if (zend_hash_find(&ce->constants_table, Z_STRVAL(opline->op2.u.constant), Z_STRLEN(opline->op2.u.constant)+1, (void **) &value) == SUCCESS) {
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EX_T(opline->result.u.var).tmp_var = **value;
				zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
				ZEND_VM_NEXT_OPCODE();
			}
		}
*/
		if (!zend_get_constant(opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len, &EX_T(opline->result.u.var).tmp_var TSRMLS_CC)) {
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
						opline->op2.u.constant.value.str.val,
						opline->op2.u.constant.value.str.val);
			EX_T(opline->result.u.var).tmp_var = opline->op2.u.constant;
			zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ce = EX_T(opline->op1.u.var).class_entry;

	if (zend_hash_find(&ce->constants_table, opline->op2.u.constant.value.str.val, opline->op2.u.constant.value.str.len+1, (void **) &value) == SUCCESS) {
		zend_class_entry *old_scope = EG(scope);

		EG(scope) = ce;
		zval_update_constant(value, (void *) 1 TSRMLS_CC);
		EG(scope) = old_scope;
		EX_T(opline->result.u.var).tmp_var = **value;
		zval_copy_ctor(&EX_T(opline->result.u.var).tmp_var);
	} else {
		zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", opline->op2.u.constant.value.str.val);
	}

	ZEND_VM_NEXT_OPCODE();
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
			expr_ptr->refcount++;
		} else 
#endif
		if (PZVAL_IS_REF(expr_ptr)) {
			zval *new_expr;

			ALLOC_ZVAL(new_expr);
			INIT_PZVAL_COPY(new_expr, expr_ptr);
			expr_ptr = new_expr;
			zendi_zval_copy_ctor(*expr_ptr);
		} else {
			expr_ptr->refcount++;
		}
	}
	if (offset) {
	  	long l;
		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
			  	DVAL_TO_LVAL(Z_DVAL_P(offset), l);
				zend_hash_index_update(Z_ARRVAL_P(array_ptr), l, &expr_ptr, sizeof(zval *), NULL);
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
	zval **original_return_value = EG(return_value_ptr_ptr);
	int return_value_used;
	zend_free_op free_op1;
	zval *inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	if (inc_filename->type!=IS_STRING) {
		tmp_inc_filename = *inc_filename;
		zval_copy_ctor(&tmp_inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	return_value_used = RETURN_VALUE_USED(opline);

	switch (Z_LVAL(opline->op2.u.constant)) {
		case ZEND_INCLUDE_ONCE:
		case ZEND_REQUIRE_ONCE: {
				zend_file_handle file_handle;

				if (IS_ABSOLUTE_PATH(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename))) {
					cwd_state state;
	
					state.cwd_length = 0;
					state.cwd = malloc(1);
					state.cwd[0] = 0;

					failure_retval = (!virtual_file_ex(&state, Z_STRVAL_P(inc_filename), NULL, 1) &&
						zend_hash_exists(&EG(included_files), state.cwd, state.cwd_length+1));

					free(state.cwd);
				}

				if (failure_retval) {
					/* do nothing */
				} else if (SUCCESS == zend_stream_open(Z_STRVAL_P(inc_filename), &file_handle TSRMLS_CC)) {

					if (!file_handle.opened_path) {
						file_handle.opened_path = estrndup(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename));
					}

					if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1)==SUCCESS) {
						new_op_array = zend_compile_file(&file_handle, (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
						zend_destroy_file_handle(&file_handle TSRMLS_CC);
					} else {
						zend_file_handle_dtor(&file_handle);
						failure_retval=1;
					}
				} else {
					if (Z_LVAL(opline->op2.u.constant)==ZEND_INCLUDE_ONCE) {
						zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
					} else {
						zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
					}
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
	if (inc_filename==&tmp_inc_filename) {
		zval_dtor(&tmp_inc_filename);
	}
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;
	if (new_op_array) {
		zval *saved_object;
		zend_function *saved_function;

		EG(return_value_ptr_ptr) = EX_T(opline->result.u.var).var.ptr_ptr;
		EG(active_op_array) = new_op_array;
		EX_T(opline->result.u.var).var.ptr = NULL;

		saved_object = EX(object);
		saved_function = EX(function_state).function;

		EX(function_state).function = (zend_function *) new_op_array;
		EX(object) = NULL;

		zend_execute(new_op_array TSRMLS_CC);

		EX(function_state).function = saved_function;
		EX(object) = saved_object;

		if (!return_value_used) {
			if (EX_T(opline->result.u.var).var.ptr) {
				zval_ptr_dtor(&EX_T(opline->result.u.var).var.ptr);
			}
		} else { /* return value is used */
			if (!EX_T(opline->result.u.var).var.ptr) { /* there was no return statement */
				ALLOC_ZVAL(EX_T(opline->result.u.var).var.ptr);
				INIT_PZVAL(EX_T(opline->result.u.var).var.ptr);
				Z_LVAL_P(EX_T(opline->result.u.var).var.ptr) = 1;
				Z_TYPE_P(EX_T(opline->result.u.var).var.ptr) = IS_BOOL;
			}
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		EG(function_state_ptr) = &EX(function_state);
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
	FREE_OP1();
	EG(return_value_ptr_ptr) = original_return_value;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMP|VAR|CV, ANY)
{
	zend_op *opline = EX(opline);
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	varname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(varname) != IS_STRING) {
		tmp = *varname;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) {
		varname->refcount++;
	}

	if (opline->op2.u.EA.type == ZEND_FETCH_STATIC_MEMBER) {
		zend_std_unset_static_property(EX_T(opline->op2.u.var).class_entry, Z_STRVAL_P(varname), Z_STRLEN_P(varname) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline, EX(Ts), BP_VAR_IS, varname TSRMLS_CC);
		if (zend_hash_del(target_symbol_table, varname->value.str.val, varname->value.str.len+1) == SUCCESS) {		
			zend_execute_data *ex = EXECUTE_DATA; 
			ulong hash_value = zend_inline_hash_func(varname->value.str.val, varname->value.str.len+1);

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
	} else if (OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) {
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
	zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);
	long index;

	if (container) {
		if (OP1_TYPE == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		switch (Z_TYPE_PP(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_PP(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						index = (long) Z_DVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						index = Z_LVAL_P(offset);
						zend_hash_index_del(ht, index);
						break;
					case IS_STRING:
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							offset->refcount++;
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

	if (container) {
		if (OP1_TYPE == IS_CV && container != &EG(uninitialized_zval_ptr)) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_PP(container) == IS_OBJECT) {
			if (IS_OP2_TMP_FREE()) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			Z_OBJ_HT_P(*container)->unset_property(*container, offset TSRMLS_CC);
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
				zend_error(E_WARNING, "foreach() can not iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}

			ce = Z_OBJCE_PP(array_ptr_ptr);
			if (!ce || ce->get_iterator == NULL) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				(*array_ptr_ptr)->refcount++;
			}
			array_ptr = *array_ptr_ptr;
		} else {
			if (Z_TYPE_PP(array_ptr_ptr) == IS_ARRAY) {
				SEPARATE_ZVAL_IF_NOT_REF(array_ptr_ptr);
				if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
					(*array_ptr_ptr)->is_ref = 1;
				}
			}
			array_ptr = *array_ptr_ptr;
			array_ptr->refcount++;
		}
	} else {
		array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (IS_OP1_TMP_FREE()) { /* IS_TMP_VAR */
			zval *tmp;

			ALLOC_ZVAL(tmp);
			INIT_PZVAL_COPY(tmp, array_ptr);
			array_ptr = tmp;
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				array_ptr->refcount++;
			}
		} else {
			if ((OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) &&
			    !array_ptr->is_ref &&
			    array_ptr->refcount > 1) {
				zval *tmp;

				ALLOC_ZVAL(tmp);
				INIT_PZVAL_COPY(tmp, array_ptr);
				zval_copy_ctor(tmp);
				array_ptr = tmp;
			} else {
				array_ptr->refcount++;
			}
		}
	}

	if (OP1_TYPE != IS_TMP_VAR && ce && ce->get_iterator) {
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

	PZVAL_LOCK(array_ptr);
	EX_T(opline->result.u.var).var.ptr = array_ptr;
	EX_T(opline->result.u.var).var.ptr_ptr = &EX_T(opline->result.u.var).var.ptr;

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (EG(exception)) {
				array_ptr->refcount--;
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
			array_ptr->refcount--;
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
	zval *array = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval **value;
	char *str_key;
	uint str_key_len;
	ulong int_key;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	int key_type = 0;
	zend_bool use_key = (zend_bool)(opline->extended_value & ZEND_FE_FETCH_WITH_KEY);

	PZVAL_LOCK(array);

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
					array->refcount--;
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
			}
			/* If index is zero we come from FE_RESET and checked valid() already. */
			if (!iter || (iter->index > 0 && iter->funcs->valid(iter TSRMLS_CC) == FAILURE)) {
				/* reached end of iteration */
				if (EG(exception)) {
					array->refcount--;
					zval_ptr_dtor(&array);
					ZEND_VM_NEXT_OPCODE();
				}
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.u.opline_num);
			}
			iter->funcs->get_current_data(iter, &value TSRMLS_CC);
			if (EG(exception)) {
				array->refcount--;
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
						array->refcount--;
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
		(*value)->is_ref = 1;
		EX_T(opline->result.u.var).var.ptr_ptr = value;
		(*value)->refcount++;
	} else {
		EX_T(opline->result.u.var).var.ptr_ptr = value;
		PZVAL_LOCK(*EX_T(opline->result.u.var).var.ptr_ptr);
		AI_USE_PTR(EX_T(opline->result.u.var).var);
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
	zend_free_op free_op1;
	zval tmp, *varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	zval **value;
	zend_bool isset = 1;
	HashTable *target_symbol_table;

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

	Z_TYPE(EX_T(opline->result.u.var).tmp_var) = IS_BOOL;

	switch (opline->extended_value) {
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

	if (varname == &tmp) {
		zval_dtor(&tmp);
	}
	FREE_OP1();

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, VAR|UNUSED|CV, CONST|TMP|VAR|CV, int prop_dim)
{
	zend_op *opline = EX(opline);
	zend_free_op free_op1;
	zval **container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_IS);
	zval **value = NULL;
	int result = 0;
	long index;

	if (container) {
		zend_free_op free_op2;
		zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (Z_TYPE_PP(container) == IS_ARRAY) {
			HashTable *ht;
			int isset = 0;

			ht = Z_ARRVAL_PP(container);

			switch (Z_TYPE_P(offset)) {
				case IS_DOUBLE:
					index = (long) Z_DVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
						isset = 1;
					}
					break;
				case IS_RESOURCE:
				case IS_BOOL:
				case IS_LONG:
					index = Z_LVAL_P(offset);
					if (zend_hash_index_find(ht, index, (void **) &value) == SUCCESS) {
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
				result = Z_OBJ_HT_P(*container)->has_property(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
			} else {
				result = Z_OBJ_HT_P(*container)->has_dimension(*container, offset, (opline->extended_value == ZEND_ISEMPTY) TSRMLS_CC);
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
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
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
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
		zendi_zval_dtor(restored_error_reporting);
	}
	if (EX(old_error_reporting) == &EX_T(opline->op1.u.var).tmp_var) {
		EX(old_error_reporting) = NULL;
	}
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

ZEND_VM_HANDLER(144, ZEND_ADD_INTERFACE, ANY, ANY)
{
	zend_op *opline = EX(opline);
	zend_class_entry *ce = EX_T(opline->op1.u.var).class_entry;
	zend_class_entry *iface = EX_T(opline->op2.u.var).class_entry;

	if (!(iface->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name, iface->name);
	}

	zend_do_implement_interface(ce, iface TSRMLS_CC);

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	zend_uint catch_op_num;
	int catched = 0;
	zval **stack_zval_pp;
	zval restored_error_reporting;

	stack_zval_pp = (zval **) EG(argument_stack).top_element - 1;
	while (*stack_zval_pp != NULL) {
		zval_ptr_dtor(stack_zval_pp);
		EG(argument_stack).top_element--;
		EG(argument_stack).top--;
		stack_zval_pp--;
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
		zend_op *ctor_opline = (zend_op*)zend_ptr_stack_pop(&EG(arg_types_stack));

		if (EX(object)) {
			if (ctor_opline && RETURN_VALUE_USED(ctor_opline)) {
				EX(object)->refcount--;
			}
			zval_ptr_dtor(&EX(object));
		}
		zend_ptr_stack_2_pop(&EG(arg_types_stack), (void**)&EX(object), (void**)&EX(fbc));
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
						zend_switch_free(brk_opline, EX(Ts) TSRMLS_CC);
						break;
					case ZEND_FREE:
						zendi_zval_dtor(EX_T(brk_opline->op1.u.var).tmp_var);
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
		zend_alter_ini_entry_ex("error_reporting", sizeof("error_reporting"), Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1);
		zendi_zval_dtor(restored_error_reporting);
	}
	EX(old_error_reporting) = NULL;

	if (!catched) {
 		ZEND_VM_RETURN_FROM_EXECUTE_LOOP();
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
			ZEND_VM_RETURN();
		case ZEND_USER_OPCODE_DISPATCH:
			ZEND_VM_DISPATCH(EX(opline)->opcode, EX(opline));
		default:
			ZEND_VM_DISPATCH(ret & 0xff, EX(opline));
	}
}

ZEND_VM_EXPORT_HELPER(zend_do_fcall, zend_do_fcall_common_helper)
