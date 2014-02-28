/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	fast_add_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(2, ZEND_SUB, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	fast_sub_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(3, ZEND_MUL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	fast_mul_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(4, ZEND_DIV, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	fast_div_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R),
		GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(5, ZEND_MOD, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	fast_mod_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(6, ZEND_SL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	shift_left_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(7, ZEND_SR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	shift_right_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(8, ZEND_CONCAT, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	concat_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(15, ZEND_IS_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	is_identical_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(16, ZEND_IS_NOT_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	is_identical_function(result,
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	Z_LVAL_P(result) = !Z_LVAL_P(result);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(17, ZEND_IS_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	ZVAL_BOOL(result, fast_equal_function(result,
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC));
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(18, ZEND_IS_NOT_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	ZVAL_BOOL(result, fast_not_equal_function(result,
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC));
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(19, ZEND_IS_SMALLER, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	ZVAL_BOOL(result, fast_is_smaller_function(result,
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC));
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(20, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	ZVAL_BOOL(result, fast_is_smaller_or_equal_function(result,
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC));
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(9, ZEND_BW_OR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	bitwise_or_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(10, ZEND_BW_AND, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	bitwise_and_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(11, ZEND_BW_XOR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	bitwise_xor_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(14, ZEND_BOOL_XOR, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	boolean_xor_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R),
		GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(12, ZEND_BW_NOT, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;

	SAVE_OPLINE();
	bitwise_not_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(13, ZEND_BOOL_NOT, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;

	SAVE_OPLINE();
	boolean_not_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R) TSRMLS_CC);
	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_obj_helper, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data1;
	zval *object = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_RW);
	zval *property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *value = get_zval_ptr((opline+1)->op1_type, &(opline+1)->op1, execute_data, &free_op_data1, BP_VAR_R);
	int have_get_ptr = 0;

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(object) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}

	make_real_object(object TSRMLS_CC);

	if (UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		FREE_OP2();
		FREE_OP(free_op_data1);

		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	} else {
		/* here we are sure we are dealing with an object */
		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property);
		}

		/* here property is a string */
		if (opline->extended_value == ZEND_ASSIGN_OBJ
			&& Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
			zval *zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
			if (zptr != NULL) { 			/* NULL means no success in getting PTR */
				SEPARATE_ZVAL_IF_NOT_REF(zptr);

				have_get_ptr = 1;
				binary_op(zptr, zptr, value TSRMLS_CC);
				if (RETURN_VALUE_USED(opline)) {
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
				}
			}
		}

		if (!have_get_ptr) {
			zval *z = NULL;
			zval rv;

			if (opline->extended_value == ZEND_ASSIGN_OBJ) {
				if (Z_OBJ_HT_P(object)->read_property) {
					z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), &rv TSRMLS_CC);
				}
			} else /* if (opline->extended_value == ZEND_ASSIGN_DIM) */ {
				if (Z_OBJ_HT_P(object)->read_dimension) {
					z = Z_OBJ_HT_P(object)->read_dimension(object, property, BP_VAR_R, &rv TSRMLS_CC);
				}
			}
			if (z) {
				if (Z_TYPE_P(z) == IS_OBJECT && Z_OBJ_HT_P(z)->get) {
					zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

					if (Z_REFCOUNT_P(z) == 0) {
						GC_REMOVE_ZVAL_FROM_BUFFER(z);
						zval_dtor(z);
					}
					z = value;
				}
				if (Z_REFCOUNTED_P(z)) Z_ADDREF_P(z);
				SEPARATE_ZVAL_IF_NOT_REF(z);
				binary_op(z, z, value TSRMLS_CC);
				if (opline->extended_value == ZEND_ASSIGN_OBJ) {
					Z_OBJ_HT_P(object)->write_property(object, property, z, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
				} else /* if (opline->extended_value == ZEND_ASSIGN_DIM) */ {
					Z_OBJ_HT_P(object)->write_dimension(object, property, z TSRMLS_CC);
				}
				if (RETURN_VALUE_USED(opline)) {
					ZVAL_COPY(EX_VAR(opline->result.var), z);
				}
				zval_ptr_dtor(z);
			} else {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (RETURN_VALUE_USED(opline)) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			}
		}

		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(property);
		} else {
			FREE_OP2();
		}
		FREE_OP(free_op_data1);
	}

	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	CHECK_EXCEPTION();
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_helper, VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV, int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data2, free_op_data1;
	zval *var_ptr;
	zval *value;

	SAVE_OPLINE();
	switch (opline->extended_value) {
		case ZEND_ASSIGN_OBJ:
			ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
			break;
		case ZEND_ASSIGN_DIM: {
				zval *container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_RW);

				if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
					zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
				} else if (UNEXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
					if (OP1_TYPE == IS_VAR && !OP1_FREE) {
						Z_ADDREF_P(container);  /* undo the effect of get_obj_zval_ptr_ptr() */
					}
					ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, binary_op);
				} else {
					zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

					zend_fetch_dimension_address(EX_VAR((opline+1)->op2.var), container, dim, OP2_TYPE, BP_VAR_RW TSRMLS_CC);
					value = get_zval_ptr((opline+1)->op1_type, &(opline+1)->op1, execute_data, &free_op_data1, BP_VAR_R);
					var_ptr = _get_zval_ptr_var((opline+1)->op2.var, execute_data, &free_op_data2 TSRMLS_CC);
				}
			}
			break;
		default:
			value = GET_OP2_ZVAL_PTR(BP_VAR_R);
			var_ptr = GET_OP1_ZVAL_PTR(BP_VAR_RW);
			/* do nothing */
			break;
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use assign-op operators with overloaded objects nor string offsets");
	}

	if (UNEXPECTED(var_ptr == &EG(error_zval))) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		if (opline->extended_value == ZEND_ASSIGN_DIM) {
			ZEND_VM_INC_OPCODE();
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SEPARATE_ZVAL_IF_NOT_REF(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_OBJECT)
	   && Z_OBJ_HANDLER_P(var_ptr, get)
	   && Z_OBJ_HANDLER_P(var_ptr, set)) {
		/* proxy object */
		zval *objval = Z_OBJ_HANDLER_P(var_ptr, get)(var_ptr TSRMLS_CC);
		Z_ADDREF_P(objval);
		binary_op(objval, objval, value TSRMLS_CC);
		Z_OBJ_HANDLER_P(var_ptr, set)(var_ptr, objval TSRMLS_CC);
		zval_ptr_dtor(objval);
	} else {
//???deref
	    if (Z_ISREF_P(var_ptr)) {
	    	var_ptr = Z_REFVAL_P(var_ptr);
		}
		binary_op(var_ptr, var_ptr, value TSRMLS_CC);
	}

	if (RETURN_VALUE_USED(opline)) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}
	FREE_OP2();

	if (opline->extended_value == ZEND_ASSIGN_DIM) {
		FREE_OP(free_op_data1);
		if (var_ptr != free_op_data2.var) {
			FREE_OP_VAR_PTR(free_op_data2);
		}
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_INC_OPCODE();
	} else {
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
	}
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
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *retval;
	int have_get_ptr = 0;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	retval = EX_VAR(opline->result.var);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(object) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}

	make_real_object(object TSRMLS_CC); /* this should modify object only if it's empty */

	if (UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(retval);
		}
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval *zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			have_get_ptr = 1;
			incdec_op(zptr);
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_COPY(retval, zptr);
			}
		}
	}

	if (!have_get_ptr) {
		zval rv;

		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), &rv TSRMLS_CC);

			if (UNEXPECTED(Z_TYPE_P(z) == IS_OBJECT) && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (Z_REFCOUNT_P(z) == 0) {
					GC_REMOVE_ZVAL_FROM_BUFFER(z);
					zval_dtor(z);
				}
				z = value;
			}
			if (Z_REFCOUNTED_P(z)) Z_ADDREF_P(z);
			SEPARATE_ZVAL_IF_NOT_REF(z);
			incdec_op(z);
			ZVAL_COPY_VALUE(retval, z);
			Z_OBJ_HT_P(object)->write_property(object, property, z, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
			SELECTIVE_PZVAL_LOCK(retval, opline);
			zval_ptr_dtor(z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_NULL(retval);
			}
		}
	}

	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property);
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
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
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *retval;
	int have_get_ptr = 0;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	retval = EX_VAR(opline->result.var);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(object) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}

	make_real_object(object TSRMLS_CC); /* this should modify object only if it's empty */

	if (UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
		FREE_OP2();
		ZVAL_NULL(retval);
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	/* here we are sure we are dealing with an object */

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}

	if (Z_OBJ_HT_P(object)->get_property_ptr_ptr) {
		zval *zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
		if (zptr != NULL) { 			/* NULL means no success in getting PTR */
			have_get_ptr = 1;
			SEPARATE_ZVAL_IF_NOT_REF(zptr);

			ZVAL_DUP(retval, zptr);

			incdec_op(zptr);

		}
	}

	if (!have_get_ptr) {
		if (Z_OBJ_HT_P(object)->read_property && Z_OBJ_HT_P(object)->write_property) {
			zval rv;
			zval *z = Z_OBJ_HT_P(object)->read_property(object, property, BP_VAR_R, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), &rv TSRMLS_CC);
			zval z_copy;

			if (UNEXPECTED(Z_TYPE_P(z) == IS_OBJECT) && Z_OBJ_HT_P(z)->get) {
				zval *value = Z_OBJ_HT_P(z)->get(z TSRMLS_CC);

				if (Z_REFCOUNT_P(z) == 0) {
					GC_REMOVE_ZVAL_FROM_BUFFER(z);
					zval_dtor(z);
				}
				z = value;
			}
			ZVAL_DUP(retval, z);
			ZVAL_DUP(&z_copy, z);
			incdec_op(&z_copy);
			if (Z_REFCOUNTED_P(z)) Z_ADDREF_P(z);
			Z_OBJ_HT_P(object)->write_property(object, property, &z_copy, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
			zval_ptr_dtor(&z_copy);
			zval_ptr_dtor(z);
		} else {
			zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
			ZVAL_NULL(retval);
		}
	}

	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property);
	} else {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
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
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	SAVE_OPLINE();
	var_ptr = GET_OP1_ZVAL_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	if (Z_TYPE_P(var_ptr) == IS_REFERENCE) {
		var_ptr = Z_REFVAL_P(var_ptr);
	} else {
		SEPARATE_ZVAL(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_OBJECT)
	   && Z_OBJ_HANDLER_P(var_ptr, get)
	   && Z_OBJ_HANDLER_P(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_P(var_ptr, get)(var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		fast_increment_function(val);
		Z_OBJ_HANDLER_P(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(val);
	} else {
		fast_increment_function(var_ptr);
	}

	if (RETURN_VALUE_USED(opline)) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	SAVE_OPLINE();
	var_ptr = GET_OP1_ZVAL_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	if (Z_TYPE_P(var_ptr) == IS_REFERENCE) {
		var_ptr = Z_REFVAL_P(var_ptr);
	} else {
		SEPARATE_ZVAL(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_OBJECT)
	   && Z_OBJ_HANDLER_P(var_ptr, get)
	   && Z_OBJ_HANDLER_P(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_P(var_ptr, get)(var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		fast_decrement_function(val);
		Z_OBJ_HANDLER_P(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(val);
	} else {
		fast_decrement_function(var_ptr);
	}

	if (RETURN_VALUE_USED(opline)) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(36, ZEND_POST_INC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr, *retval;

	SAVE_OPLINE();
	var_ptr = GET_OP1_ZVAL_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	retval = EX_VAR(opline->result.var);

	if (Z_TYPE_P(var_ptr) == IS_REFERENCE) {
		var_ptr = Z_REFVAL_P(var_ptr);
		ZVAL_DUP(retval, var_ptr);
	} else {
		ZVAL_DUP(retval, var_ptr);
		SEPARATE_ZVAL(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_OBJECT)
	   && Z_OBJ_HANDLER_P(var_ptr, get)
	   && Z_OBJ_HANDLER_P(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_P(var_ptr, get)(var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		fast_increment_function(val);
		Z_OBJ_HANDLER_P(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(val);
	} else {
		fast_increment_function(var_ptr);
	}

	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(37, ZEND_POST_DEC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr, *retval;

	SAVE_OPLINE();
	var_ptr = GET_OP1_ZVAL_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot increment/decrement overloaded objects nor string offsets");
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	retval = EX_VAR(opline->result.var);

	if (Z_TYPE_P(var_ptr) == IS_REFERENCE) {
		var_ptr = Z_REFVAL_P(var_ptr);
		ZVAL_DUP(retval, var_ptr);
	} else {
		ZVAL_DUP(retval, var_ptr);
		SEPARATE_ZVAL(var_ptr);
	}

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_OBJECT)
	   && Z_OBJ_HANDLER_P(var_ptr, get)
	   && Z_OBJ_HANDLER_P(var_ptr, set)) {
		/* proxy object */
		zval *val = Z_OBJ_HANDLER_P(var_ptr, get)(var_ptr TSRMLS_CC);
		Z_ADDREF_P(val);
		fast_decrement_function(val);
		Z_OBJ_HANDLER_P(var_ptr, set)(var_ptr, val TSRMLS_CC);
		zval_ptr_dtor(val);
	} else {
		fast_decrement_function(var_ptr);
	}

	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(40, ZEND_ECHO, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *z;

	SAVE_OPLINE();
	z = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && Z_TYPE_P(z) == IS_OBJECT) {
		Z_SET_REFCOUNT_P(z, 1);
	}

	if (UNEXPECTED(Z_ISREF_P(z))) {
		z = Z_REFVAL_P(z);
	}

	zend_print_variable(z);

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(41, ZEND_PRINT, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE

	ZVAL_LONG(EX_VAR(opline->result.var), 1);
	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ECHO);
}

ZEND_VM_HELPER_EX(zend_fetch_var_address_helper, CONST|TMP|VAR|CV, UNUSED|CONST|VAR, int type)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varname;
	zval *retval;
	zval tmp_varname;
	HashTable *target_symbol_table;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR(BP_VAR_R);

 	if (OP1_TYPE != IS_CONST && UNEXPECTED(Z_TYPE_P(varname) != IS_STRING)) {
		ZVAL_DUP_DEREF(&tmp_varname, varname);
		convert_to_string(&tmp_varname);
		varname = &tmp_varname;
	}

	if (OP2_TYPE != IS_UNUSED) {
		zend_class_entry *ce;

		if (OP2_TYPE == IS_CONST) {
			if (CACHED_PTR(opline->op2.literal->cache_slot)) {
				ce = CACHED_PTR(opline->op2.literal->cache_slot);
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(opline->op2.zv), opline->op2.literal + 1, 0 TSRMLS_CC);
				if (UNEXPECTED(ce == NULL)) {
					if (OP1_TYPE != IS_CONST && varname == &tmp_varname) {
						zval_dtor(&tmp_varname);
					}
					FREE_OP1();
					CHECK_EXCEPTION();
					ZEND_VM_NEXT_OPCODE();
				}
				CACHE_PTR(opline->op2.literal->cache_slot, ce);
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		retval = zend_std_get_static_property(ce, Z_STR_P(varname), 0, ((OP1_TYPE == IS_CONST) ? opline->op1.literal : NULL) TSRMLS_CC);
		FREE_OP1();
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline->extended_value & ZEND_FETCH_TYPE_MASK TSRMLS_CC);
//???: STRING may become INDIRECT
		if (Z_TYPE_P(varname) == IS_INDIRECT) {
			varname = Z_INDIRECT_P(varname);
		}
/*
		if (!target_symbol_table) {
			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		}
*/
		if ((retval = zend_hash_find(target_symbol_table, Z_STR_P(varname))) == NULL) {
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_IS:
					//???
					//ZVAL_NULL(retval);
					ZVAL_NULL(EX_VAR(opline->result.var));
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", Z_STRVAL_P(varname));
					/* break missing intentionally */
				case BP_VAR_W:
					retval = zend_hash_update(target_symbol_table, Z_STR_P(varname), &EG(uninitialized_zval));
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
		switch (opline->extended_value & ZEND_FETCH_TYPE_MASK) {
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
					Z_ADDREF_P(EX_VAR(opline->op1.var));
				}
				break;
		}
	}


	if (OP1_TYPE != IS_CONST && varname == &tmp_varname) {
		zval_dtor(&tmp_varname);
	}

	if (opline->extended_value & ZEND_FETCH_MAKE_REF) {
		SEPARATE_ZVAL_TO_MAKE_IS_REF(retval);
	}

	if (EXPECTED(retval != NULL)) {
		if (IS_REFCOUNTED(Z_TYPE_P(retval))) Z_ADDREF_P(retval);
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_IS:
				ZVAL_COPY_VALUE(EX_VAR(opline->result.var), retval);
				break;
			case BP_VAR_UNSET: {
	//???			zend_free_op free_res;
	//???
	//???			PZVAL_UNLOCK(*retval, &free_res);
	//???			if (retval != &EG(uninitialized_zval_ptr)) {
	//???				SEPARATE_ZVAL_IF_NOT_REF(retval);
	//???			}
	//???			PZVAL_LOCK(*retval);
	//???			FREE_OP_VAR_PTR(free_res);
			}
			/* break missing intentionally */
			default:
//???				if (Z_REFCOUNTED_P(retval)) Z_ADDREF_P(retval);				
				ZVAL_INDIRECT(EX_VAR(opline->result.var), retval);
				break;
		} 
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(80, ZEND_FETCH_R, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(83, ZEND_FETCH_W, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(86, ZEND_FETCH_RW, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(92, ZEND_FETCH_FUNC_ARG, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE

	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type,
		zend_is_by_ref_func_arg_fetch(opline, EX(call) TSRMLS_CC) ? BP_VAR_W : BP_VAR_R);
}

ZEND_VM_HANDLER(95, ZEND_FETCH_UNSET, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(89, ZEND_FETCH_IS, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_IS);
}

ZEND_VM_HANDLER(81, ZEND_FETCH_DIM_R, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zend_fetch_dimension_address_read(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_R TSRMLS_CC);
	FREE_OP2();
	if (OP1_TYPE != IS_VAR || !(opline->extended_value & ZEND_FETCH_ADD_LOCK)) {
		FREE_OP1();
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(84, ZEND_FETCH_DIM_W, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
//???
	int do_free = 1;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
//??? we must not free implicitly created array (it should be fixed in another way)
	if (OP1_TYPE == IS_VAR && !Z_REFCOUNTED_P(container)) {
		do_free = 0;
	}
	zend_fetch_dimension_address(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_W TSRMLS_CC);
	FREE_OP2();
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
//???	}

//???
	if (OP1_TYPE == IS_VAR && do_free) {
		FREE_OP1_VAR_PTR();
	}

	/* We are going to assign the result by reference */
	if (UNEXPECTED(opline->extended_value != 0)) {
		zval *retval_ptr = EX_VAR(opline->result.var);

		if (Z_TYPE_P(retval_ptr) == IS_INDIRECT) {
			retval_ptr = Z_INDIRECT_P(retval_ptr);
		}
		if (!Z_ISREF_P(retval_ptr)) {
			if (Z_REFCOUNTED_P(retval_ptr)) {
				Z_DELREF_P(retval_ptr);
				SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr);
				Z_ADDREF_P(retval_ptr);
			} else {
				ZVAL_NEW_REF(retval_ptr, retval_ptr);
			}
		}
	}

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_RW TSRMLS_CC);
	FREE_OP2();
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???	}
	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(90, ZEND_FETCH_DIM_IS, VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	zend_fetch_dimension_address_read(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_IS TSRMLS_CC);
	FREE_OP2();
	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zval *container;
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call) TSRMLS_CC)) {
		container = GET_OP1_ZVAL_PTR(BP_VAR_W);
		if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
			zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
		}
		zend_fetch_dimension_address(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_W TSRMLS_CC);
//???		if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???			EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			zend_error_noreturn(E_ERROR, "Cannot use [] for reading");
		}
		container = GET_OP1_ZVAL_PTR(BP_VAR_R);
		zend_fetch_dimension_address_read(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_R TSRMLS_CC);
		FREE_OP2();
		FREE_OP1();
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(96, ZEND_FETCH_DIM_UNSET, VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_UNSET);

	if (OP1_TYPE == IS_CV) {
		SEPARATE_ZVAL_IF_NOT_REF(container);
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_fetch_dimension_address(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_UNSET TSRMLS_CC);
	FREE_OP2();
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???	}
	FREE_OP1_VAR_PTR();
	if (UNEXPECTED(Z_TYPE_P(EX_VAR(opline->result.var)) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot unset string offsets");
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_free_op free_res;
		zval *retval_ptr = EX_VAR(opline->result.var);

		if (Z_TYPE_P(retval_ptr) == IS_INDIRECT) {
			retval_ptr = Z_INDIRECT_P(retval_ptr);
		}
		PZVAL_UNLOCK(retval_ptr, &free_res);
		SEPARATE_ZVAL_IF_NOT_REF(retval_ptr);
		PZVAL_LOCK(retval_ptr);
		FREE_OP_VAR_PTR(free_res);
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HELPER(zend_fetch_property_address_read_helper, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);
	offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
		container = Z_REFVAL_P(container);
	}

	if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT) ||
	    UNEXPECTED(Z_OBJ_HT_P(container)->read_property == NULL)) {
		zend_error(E_NOTICE, "Trying to get property of non-object");
		ZVAL_NULL(EX_VAR(opline->result.var));
		FREE_OP2();
	} else {
		zval *retval;

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		retval = Z_OBJ_HT_P(container)->read_property(container, offset, BP_VAR_R, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), EX_VAR(opline->result.var) TSRMLS_CC);

		if (retval != EX_VAR(opline->result.var)) {
			ZVAL_COPY(EX_VAR(opline->result.var), retval);
		}

		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(offset);
		} else {
			FREE_OP2();
		}
	}

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(82, ZEND_FETCH_OBJ_R, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_property_address_read_helper);
}

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_W);
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}

	zend_fetch_property_address(EX_VAR(opline->result.var), container, property, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), BP_VAR_W TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property);
	} else {
		FREE_OP2();
	}
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???	}
	FREE_OP1_VAR_PTR();

	/* We are going to assign the result by reference */
	if (opline->extended_value & ZEND_FETCH_MAKE_REF) {
		zval *retval_ptr = EX_VAR(opline->result.var);

		if (Z_TYPE_P(retval_ptr) == IS_INDIRECT) {
			retval_ptr = Z_INDIRECT_P(retval_ptr);
		}
		if (!Z_ISREF_P(retval_ptr)) {
			if (Z_REFCOUNTED_P(retval_ptr)) {
				Z_DELREF_P(retval_ptr);
				SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr);
				Z_ADDREF_P(retval_ptr);
			} else {
				ZVAL_NEW_REF(retval_ptr, retval_ptr);
			}
		}
	}

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_RW);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}
	zend_fetch_property_address(EX_VAR(opline->result.var), container, property, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), BP_VAR_RW TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property);
	} else {
		FREE_OP2();
	}
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???	}
	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(91, ZEND_FETCH_OBJ_IS, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);
	offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT) ||
	    UNEXPECTED(Z_OBJ_HT_P(container)->read_property == NULL)) {
		ZVAL_NULL(EX_VAR(opline->result.var));
		FREE_OP2();
	} else {
		zval *retval;

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(offset);
		}

		/* here we are sure we are dealing with an object */
		retval = Z_OBJ_HT_P(container)->read_property(container, offset, BP_VAR_IS, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), EX_VAR(opline->result.var) TSRMLS_CC);

		if (retval != EX_VAR(opline->result.var)) {
			ZVAL_COPY(EX_VAR(opline->result.var), retval);
		}

		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(offset);
		} else {
			FREE_OP2();
		}
	}

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zval *container;

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call) TSRMLS_CC)) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property;

		SAVE_OPLINE();
		property = GET_OP2_ZVAL_PTR(BP_VAR_R);
		container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_W);

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property);
		}
		if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
			zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
		}
		zend_fetch_property_address(EX_VAR(opline->result.var), container, property, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), BP_VAR_W TSRMLS_CC);
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(property);
		} else {
			FREE_OP2();
		}
//???		if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???			EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???		}
		FREE_OP1_VAR_PTR();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_property_address_read_helper);
	}
}

ZEND_VM_HANDLER(97, ZEND_FETCH_OBJ_UNSET, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_res;
	zval *container, *property, *retval_ptr;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_UNSET);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CV) {
		SEPARATE_ZVAL_IF_NOT_REF(container);
	}
	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property);
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(container) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an object");
	}
	zend_fetch_property_address(EX_VAR(opline->result.var), container, property, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL), BP_VAR_UNSET TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property);
	} else {
		FREE_OP2();
	}
//???	if (OP1_TYPE == IS_VAR && OP1_FREE && READY_TO_DESTROY(free_op1.var)) {
//???		EXTRACT_ZVAL_PTR(&EX_T(opline->result.var));
//???	}
	FREE_OP1_VAR_PTR();

	retval_ptr = EX_VAR(opline->result.var);
	if (Z_TYPE_P(retval_ptr) == IS_INDIRECT) {
		retval_ptr = Z_INDIRECT_P(retval_ptr);
	}
	PZVAL_UNLOCK(retval_ptr, &free_res);
	SEPARATE_ZVAL_IF_NOT_REF(retval_ptr);
	PZVAL_LOCK(retval_ptr);
	FREE_OP_VAR_PTR(free_res);
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_DIM_TMP_VAR, CONST|TMP, CONST)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(Z_TYPE_P(container) != IS_ARRAY)) {
		ZVAL_NULL(EX_VAR(opline->result.var));
	} else {
		zend_free_op free_op2;
		zval *value = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE, BP_VAR_R TSRMLS_CC);

		ZVAL_COPY(EX_VAR(opline->result.var), value);
		FREE_OP2();
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property_name;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_W);
	property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (IS_OP2_TMP_FREE()) {
		MAKE_REAL_ZVAL_PTR(property_name);
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(object) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	zend_assign_to_object(RETURN_VALUE_USED(opline)?EX_VAR(opline->result.var):NULL, object, property_name, (opline+1)->op1_type, &(opline+1)->op1, execute_data, ZEND_ASSIGN_OBJ, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
	if (IS_OP2_TMP_FREE()) {
		zval_ptr_dtor(property_name);
	} else {
		FREE_OP2();
	}
//???	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	CHECK_EXCEPTION();
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(147, ZEND_ASSIGN_DIM, VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *object_ptr;

	SAVE_OPLINE();
	object_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(object_ptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Cannot use string offset as an array");
	}
	if (Z_TYPE_P(object_ptr) == IS_OBJECT) {
		zend_free_op free_op2;
		zval *property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(property_name);
		}
		zend_assign_to_object(RETURN_VALUE_USED(opline)?EX_VAR(opline->result.var):NULL, object_ptr, property_name, (opline+1)->op1_type, &(opline+1)->op1, execute_data, ZEND_ASSIGN_DIM, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(property_name);
		} else {
			FREE_OP2();
		}
	} else {
		zend_free_op free_op2, free_op_data1, free_op_data2;
		zval *value;
		zval *dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
		zval *variable_ptr;

		zend_fetch_dimension_address(EX_VAR((opline+1)->op2.var), object_ptr, dim, OP2_TYPE, BP_VAR_W TSRMLS_CC);
		FREE_OP2();

		value = get_zval_ptr((opline+1)->op1_type, &(opline+1)->op1, execute_data, &free_op_data1, BP_VAR_R);
		variable_ptr = _get_zval_ptr_var((opline+1)->op2.var, execute_data, &free_op_data2 TSRMLS_CC);
		if (UNEXPECTED(Z_TYPE_P(variable_ptr) == IS_STR_OFFSET)) {
			if (zend_assign_to_string_offset(EX_VAR((opline+1)->op2.var), value, (opline+1)->op1_type TSRMLS_CC)) {
				if (RETURN_VALUE_USED(opline)) {
					ZVAL_STRINGL(EX_VAR(opline->result.var), Z_STR_OFFSET_P(EX_VAR((opline+1)->op2.var))->str->val + Z_STR_OFFSET_P(EX_VAR((opline+1)->op2.var))->offset, 1);
				}
			} else if (RETURN_VALUE_USED(opline)) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		} else if (UNEXPECTED(variable_ptr == &EG(error_zval))) {
			if (IS_TMP_FREE(free_op_data1)) {
				zval_dtor(value);
			}
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		} else {
			if ((opline+1)->op1_type == IS_TMP_VAR) {
			 	value = zend_assign_tmp_to_variable(variable_ptr, value TSRMLS_CC);
			} else if ((opline+1)->op1_type == IS_CONST) {
			 	value = zend_assign_const_to_variable(variable_ptr, value TSRMLS_CC);
			} else {
			 	value = zend_assign_to_variable(variable_ptr, value TSRMLS_CC);
			}
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_COPY(EX_VAR(opline->result.var), value);
			}
		}
//???		FREE_OP_VAR_PTR(free_op_data2);
	 	FREE_OP_IF_VAR(free_op_data1);
	}
 	FREE_OP1_VAR_PTR();
	/* assign_dim has two opcodes! */
	CHECK_EXCEPTION();
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(38, ZEND_ASSIGN, VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *value;
	zval *variable_ptr;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	variable_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(variable_ptr) == IS_STR_OFFSET)) {
		if (zend_assign_to_string_offset(EX_VAR(opline->op1.var), value, OP2_TYPE TSRMLS_CC)) {
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_STRINGL(EX_VAR(opline->result.var), Z_STR_OFFSET_P(EX_VAR(opline->op1.var))->str->val + Z_STR_OFFSET_P(EX_VAR(opline->op1.var))->offset, 1);
			}
		} else if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	} else if (OP1_TYPE == IS_VAR && UNEXPECTED(variable_ptr == &EG(error_zval))) {
		if (IS_OP2_TMP_FREE()) {
			zval_dtor(value);
		}
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	} else {
		if (OP2_TYPE == IS_TMP_VAR) {
		 	value = zend_assign_tmp_to_variable(variable_ptr, value TSRMLS_CC);
		} else if (OP2_TYPE == IS_CONST) {
		 	value = zend_assign_const_to_variable(variable_ptr, value TSRMLS_CC);
		} else {
		 	value = zend_assign_to_variable(variable_ptr, value TSRMLS_CC);
		}
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
	}

	FREE_OP1_VAR_PTR();

	/* zend_assign_to_variable() always takes care of op2, never free it! */
 	FREE_OP2_IF_VAR();

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(39, ZEND_ASSIGN_REF, VAR|CV, VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *variable_ptr;
	zval *value_ptr;

	SAVE_OPLINE();
	value_ptr = GET_OP2_ZVAL_PTR(BP_VAR_W);

	if (OP2_TYPE == IS_VAR &&
	    value_ptr &&
	    !Z_ISREF_P(value_ptr) &&
	    opline->extended_value == ZEND_RETURNS_FUNCTION /*???&&
	    !EX_T(opline->op2.var).var.fcall_returned_reference*/) {
		if (free_op2.var == NULL) {
			PZVAL_LOCK(value_ptr); /* undo the effect of get_zval_ptr_ptr() */
		}
		zend_error(E_STRICT, "Only variables should be assigned by reference");
		if (UNEXPECTED(EG(exception) != NULL)) {
			FREE_OP2_VAR_PTR();
			HANDLE_EXCEPTION();
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ASSIGN);
	} else if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		PZVAL_LOCK(value_ptr);
	}
//???	if (OP1_TYPE == IS_VAR && UNEXPECTED(EX_T(opline->op1.var).var.ptr_ptr == &EX_T(opline->op1.var).var.ptr)) {
//???		zend_error_noreturn(E_ERROR, "Cannot assign by reference to overloaded object");
//???	}

	variable_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);
	if ((OP2_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(value_ptr) == IS_STR_OFFSET)) ||
	    (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(variable_ptr) == IS_STR_OFFSET))) {
		zend_error_noreturn(E_ERROR, "Cannot create references to/from string offsets nor overloaded objects");
	}
//???: instead of FREE_OP2_VAR_PTR
	if (OP2_TYPE == IS_VAR && free_op2.var) {
		if (Z_REFCOUNTED_P(free_op2.var)) Z_DELREF_P(free_op2.var);
		free_op2.var = NULL;
	}
	zend_assign_to_variable_reference(variable_ptr, value_ptr TSRMLS_CC);

	if (OP2_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_NEW) {
		Z_DELREF_P(variable_ptr);
	}

	if (RETURN_VALUE_USED(opline)) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}

//???	FREE_OP1_VAR_PTR();
	FREE_OP2_VAR_PTR();

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_leave_helper, ANY, ANY)
{
	zend_bool nested = EX(nested);
	zend_op_array *op_array = EX(op_array);

	EG(current_execute_data) = EX(prev_execute_data);
	EG(opline_ptr) = NULL;
	if (!EG(active_symbol_table)) {
		i_free_compiled_variables(execute_data TSRMLS_CC);
	}

	zend_vm_stack_free((char*)execute_data TSRMLS_CC);

	if ((op_array->fn_flags & ZEND_ACC_CLOSURE) && op_array->prototype) {
		zval_ptr_dtor((zval*)op_array->prototype);
	}

	if (nested) {
		execute_data = EG(current_execute_data);
	}
	if (nested) {
		USE_OPLINE

		LOAD_REGS();
		LOAD_OPLINE();
		if (UNEXPECTED(opline->opcode == ZEND_INCLUDE_OR_EVAL)) {

			EX(function_state).function = (zend_function *) EX(op_array);
			EX(function_state).arguments = NULL;

			EG(opline_ptr) = &EX(opline);
			EG(active_op_array) = EX(op_array);
			destroy_op_array(op_array TSRMLS_CC);
			efree(op_array);
			if (UNEXPECTED(EG(exception) != NULL)) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				HANDLE_EXCEPTION_LEAVE();
			}

			ZEND_VM_INC_OPCODE();
			ZEND_VM_LEAVE();
		} else {
			EG(opline_ptr) = &EX(opline);
			EG(active_op_array) = EX(op_array);
			if (EG(active_symbol_table)) {
				zend_clean_and_cache_symbol_table(EG(active_symbol_table) TSRMLS_CC);
			}
			EG(active_symbol_table) = EX(symbol_table);

			EX(function_state).function = (zend_function *) EX(op_array);
			EX(function_state).arguments = NULL;

			if (Z_TYPE(EG(This)) != IS_UNDEF) {
				if (UNEXPECTED(EG(exception) != NULL) && EX(call)->is_ctor_call) {
					if (EX(call)->is_ctor_result_used) {
						Z_DELREF(EG(This));
					}
					if (Z_REFCOUNT(EG(This)) == 1) {
						zend_object_store_ctor_failed(&EG(This) TSRMLS_CC);
					}
				}
				zval_ptr_dtor(&EG(This));
			}
			ZVAL_COPY_VALUE(&EG(This), &EX(current_this));
			EG(scope) = EX(current_scope);
			EG(called_scope) = EX(current_called_scope);

			EX(call)--;

			zend_vm_stack_clear_multiple(1 TSRMLS_CC);

			if (UNEXPECTED(EG(exception) != NULL)) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				if (RETURN_VALUE_USED(opline) /*???&& EX_VAR(opline->result.var) */) {
					zval_ptr_dtor(EX_VAR(opline->result.var));
				}
				HANDLE_EXCEPTION_LEAVE();
			}

			ZEND_VM_INC_OPCODE();
			ZEND_VM_LEAVE();
		}
	}
	ZEND_VM_RETURN();
}

ZEND_VM_HELPER(zend_do_fcall_common_helper, ANY, ANY)
{
	USE_OPLINE
	zend_bool should_change_scope = 0;
	zend_function *fbc = EX(function_state).function;
	zend_uint num_args;

	SAVE_OPLINE();
	ZVAL_COPY_VALUE(&EX(object), &EX(call)->object);
	if (UNEXPECTED((fbc->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) != 0)) {
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_ABSTRACT) != 0)) {
			zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", fbc->common.scope->name->val, fbc->common.function_name->val);
		}
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				fbc->common.scope ? fbc->common.scope->name->val : "",
				fbc->common.scope ? "::" : "",
				fbc->common.function_name->val);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
	}
	if (fbc->common.scope &&
		!(fbc->common.fn_flags & ZEND_ACC_STATIC) &&
		Z_TYPE(EX(object)) == IS_UNDEF) {

		if (fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
			/* FIXME: output identifiers properly */
			zend_error(E_STRICT, "Non-static method %s::%s() should not be called statically", fbc->common.scope->name->val, fbc->common.function_name->val);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		} else {
			/* FIXME: output identifiers properly */
			/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
			zend_error_noreturn(E_ERROR, "Non-static method %s::%s() cannot be called statically", fbc->common.scope->name->val, fbc->common.function_name->val);
		}
	}

	if (fbc->type == ZEND_USER_FUNCTION || fbc->common.scope) {
		should_change_scope = 1;
		ZVAL_COPY_VALUE(&EX(current_this), &EG(This));
		EX(current_scope) = EG(scope);
		EX(current_called_scope) = EG(called_scope);
		EG(This) = EX(object);
		EG(scope) = (fbc->type == ZEND_USER_FUNCTION || Z_TYPE(EX(object)) == IS_UNDEF) ? fbc->common.scope : NULL;
		EG(called_scope) = EX(call)->called_scope;
	}

	num_args = opline->extended_value + EX(call)->num_additional_args;
	if (EX(call)->num_additional_args) {
		EX(function_state).arguments = zend_vm_stack_push_args(num_args TSRMLS_CC);
	} else {
		zval tmp;

		ZVAL_LONG(&tmp, num_args);
		EX(function_state).arguments = zend_vm_stack_top(TSRMLS_C);
		zend_vm_stack_push(&tmp TSRMLS_CC);
	}
	LOAD_OPLINE();

	if (fbc->type == ZEND_INTERNAL_FUNCTION) {
		if (fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
			zend_uint i;
			zval *p = EX(function_state).arguments - num_args;

			for (i = 0; i < num_args; ++i, ++p) {
				zend_verify_arg_type(fbc, i + 1, p, 0 TSRMLS_CC);
			}
		}

		if (EXPECTED(EG(exception) == NULL)) {
			zval *ret = EX_VAR(opline->result.var);

			ZVAL_NULL(ret);
//???			ret->var.ptr_ptr = &ret->var.ptr;
//???			ret->var.fcall_returned_reference = (fbc->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;

			if (!zend_execute_internal) {
				/* saves one function call if zend_execute_internal is not used */
				fbc->internal_function.handler(num_args, ret, &EX(object), RETURN_VALUE_USED(opline) TSRMLS_CC);
			} else {
				zend_execute_internal(execute_data, NULL, RETURN_VALUE_USED(opline) TSRMLS_CC);
			}

			if (!RETURN_VALUE_USED(opline)) {
				zval_ptr_dtor(ret);
			}
		} else if (RETURN_VALUE_USED(opline)) {			
//???			EX_T(opline->result.var).var.ptr = NULL;
		}
	} else if (fbc->type == ZEND_USER_FUNCTION) {
		zval *return_value = NULL;

		EG(active_symbol_table) = NULL;
		EG(active_op_array) = &fbc->op_array;
		if (RETURN_VALUE_USED(opline)) {
			return_value = EX_VAR(opline->result.var);

			ZVAL_NULL(return_value);
//???			ret->var.ptr_ptr = &ret->var.ptr;
//???			ret->var.fcall_returned_reference = (fbc->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0;
		}

		if (UNEXPECTED((EG(active_op_array)->fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			if (RETURN_VALUE_USED(opline)) {
				zend_generator_create_zval(EG(active_op_array), EX_VAR(opline->result.var) TSRMLS_CC);
			}
		} else if (EXPECTED(zend_execute_ex == execute_ex)) {
			if (EXPECTED(EG(exception) == NULL)) {
				i_create_execute_data_from_op_array(EG(active_op_array), return_value, 1 TSRMLS_CC);
				ZEND_VM_ENTER();
			}
		} else {
			zend_execute(EG(active_op_array), return_value TSRMLS_CC);
		}

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		if (EG(active_symbol_table)) {
			zend_clean_and_cache_symbol_table(EG(active_symbol_table) TSRMLS_CC);
		}
		EG(active_symbol_table) = EX(symbol_table);
	} else { /* ZEND_OVERLOADED_FUNCTION */
		ZVAL_NULL(EX_VAR(opline->result.var));

		/* Not sure what should be done here if it's a static method */
		if (EXPECTED(Z_TYPE(EX(object)) != IS_UNDEF)) {
			Z_OBJ_HT_P(&EX(object))->call_method(fbc->common.function_name, num_args, EX_VAR(opline->result.var), &EX(object), RETURN_VALUE_USED(opline) TSRMLS_CC);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot call overloaded function for non-object");
		}

		if (fbc->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			STR_RELEASE(fbc->common.function_name);
		}
		efree(fbc);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		} else {
//???			Z_UNSET_ISREF_P(EX_T(opline->result.var).var.ptr);
//???			Z_SET_REFCOUNT_P(EX_T(opline->result.var).var.ptr, 1);
//???			EX_T(opline->result.var).var.fcall_returned_reference = 0;
//???			EX_T(opline->result.var).var.ptr_ptr = &EX_T(opline->result.var).var.ptr;
		}
	}

	EX(function_state).function = (zend_function *) EX(op_array);
	EX(function_state).arguments = NULL;

	if (should_change_scope) {
		if (Z_TYPE(EG(This)) != IS_UNDEF) {
			if (UNEXPECTED(EG(exception) != NULL) && EX(call)->is_ctor_call) {
				if (EX(call)->is_ctor_result_used) {
					Z_DELREF(EG(This));
				}
				if (Z_REFCOUNT(EG(This)) == 1) {
					zend_object_store_ctor_failed(&EG(This) TSRMLS_CC);
				}
			}
			zval_ptr_dtor(&EG(This));
		}
		ZVAL_COPY_VALUE(&EG(This), &EX(current_this));
		EG(scope) = EX(current_scope);
		EG(called_scope) = EX(current_called_scope);
	}

	EX(call)--;

	zend_vm_stack_clear_multiple(1 TSRMLS_CC);

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL TSRMLS_CC);
		if (RETURN_VALUE_USED(opline) /*???&& EX_T(opline->result.var).var.ptr*/) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(42, ZEND_JMP, ANY, ANY)
{
	USE_OPLINE

#if DEBUG_ZEND>=2
	printf("Jumping to %d\n", opline->op1.opline_num);
#endif
	ZEND_VM_SET_OPCODE(opline->op1.jmp_addr);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(43, ZEND_JMPZ, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int ret;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && EXPECTED(Z_TYPE_P(val) == IS_BOOL)) {
		ret = Z_LVAL_P(val);
	} else {
		ret = i_zend_is_true(val TSRMLS_CC);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
	}
	if (!ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.jmp_addr);
		ZEND_VM_CONTINUE();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(44, ZEND_JMPNZ, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int ret;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && EXPECTED(Z_TYPE_P(val) == IS_BOOL)) {
		ret = Z_LVAL_P(val);
	} else {
		ret = i_zend_is_true(val TSRMLS_CC);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
	}
	if (ret) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.jmp_addr);
		ZEND_VM_CONTINUE();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(45, ZEND_JMPZNZ, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int retval;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && EXPECTED(Z_TYPE_P(val) == IS_BOOL)) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val TSRMLS_CC);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
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
		printf("Conditional jmp on false to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.opline_num]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
}

ZEND_VM_HANDLER(46, ZEND_JMPZ_EX, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int retval;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && EXPECTED(Z_TYPE_P(val) == IS_BOOL)) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val TSRMLS_CC);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
	}
	ZVAL_BOOL(EX_VAR(opline->result.var), retval);
	if (!retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.jmp_addr);
		ZEND_VM_CONTINUE();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int retval;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (OP1_TYPE == IS_TMP_VAR && EXPECTED(Z_TYPE_P(val) == IS_BOOL)) {
		retval = Z_LVAL_P(val);
	} else {
		retval = i_zend_is_true(val TSRMLS_CC);
		FREE_OP1();
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
	}
	ZVAL_BOOL(EX_VAR(opline->result.var), retval);
	if (retval) {
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_SET_OPCODE(opline->op2.jmp_addr);
		ZEND_VM_CONTINUE();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(70, ZEND_FREE, TMP|VAR, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE == IS_TMP_VAR) {
		zval_dtor(EX_VAR(opline->op1.var));
	} else {
		zval_ptr_dtor(EX_VAR(opline->op1.var));
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(53, ZEND_INIT_STRING, ANY, ANY)
{
	USE_OPLINE
	zval *tmp = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	ZVAL_EMPTY_STRING(tmp);
	/*CHECK_EXCEPTION();*/
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(54, ZEND_ADD_CHAR, TMP|UNUSED, CONST)
{
	USE_OPLINE
	zval *str = EX_VAR(opline->result.var);

	SAVE_OPLINE();

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_char_to_string */
		ZVAL_STR(str, STR_EMPTY_ALLOC());
	}

	add_char_to_string(str, str, opline->op2.zv);

	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	/*CHECK_EXCEPTION();*/
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(55, ZEND_ADD_STRING, TMP|UNUSED, CONST)
{
	USE_OPLINE
	zval *str = EX_VAR(opline->result.var);

	SAVE_OPLINE();

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_string_to_string */
		ZVAL_STR(str, STR_EMPTY_ALLOC());
	}

	add_string_to_string(str, str, opline->op2.zv);

	/* FREE_OP is missing intentionally here - we're always working on the same temporary variable */
	/*CHECK_EXCEPTION();*/
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(56, ZEND_ADD_VAR, TMP|UNUSED, TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op2;
	zval *str = EX_VAR(opline->result.var);
	zval *var;
	zval var_copy;
	int use_copy = 0;

	SAVE_OPLINE();
	var = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED) {
		/* Initialize for erealloc in add_string_to_string */
		ZVAL_STR(str, STR_EMPTY_ALLOC());
	}

	if (Z_TYPE_P(var) != IS_STRING) {
		if (Z_TYPE_P(var) == IS_REFERENCE) {
			var = Z_REFVAL_P(var);
		}
		if (Z_TYPE_P(var) != IS_STRING) {
			zend_make_printable_zval(var, &var_copy, &use_copy);

			if (use_copy) {
				var = &var_copy;
			}
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

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(109, ZEND_FETCH_CLASS, ANY, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (EG(exception)) {
		zend_exception_save(TSRMLS_C);
	}
	if (OP2_TYPE == IS_UNUSED) {
		Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(NULL, opline->extended_value TSRMLS_CC);
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_free_op free_op2;
		zval *class_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (OP2_TYPE == IS_CONST) {
			if (CACHED_PTR(opline->op2.literal->cache_slot)) {
				Z_CE_P(EX_VAR(opline->result.var)) = CACHED_PTR(opline->op2.literal->cache_slot);
			} else {
				Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class_by_name(Z_STR_P(class_name), opline->op2.literal + 1, opline->extended_value TSRMLS_CC);
				CACHE_PTR(opline->op2.literal->cache_slot, Z_CE_P(EX_VAR(opline->result.var)));
			}
		} else if (Z_TYPE_P(class_name) == IS_OBJECT) {
			Z_CE_P(EX_VAR(opline->result.var)) = Z_OBJCE_P(class_name);
		} else if (Z_TYPE_P(class_name) == IS_STRING) {
			Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(Z_STR_P(class_name), opline->extended_value TSRMLS_CC);
		} else {
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
			zend_error_noreturn(E_ERROR, "Class name must be a valid object or a string");
		}

		FREE_OP2();
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(112, ZEND_INIT_METHOD_CALL, TMP|VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zval *function_name;
	zend_free_op free_op1, free_op2;
	call_slot *call = EX(call_slots) + opline->result.num;

	SAVE_OPLINE();

	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP2_TYPE != IS_CONST &&
	    UNEXPECTED(Z_TYPE_P(function_name) != IS_STRING)) {
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
		zend_error_noreturn(E_ERROR, "Method name must be a string");
	}

	ZVAL_COPY_VALUE(&call->object, GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R));

	if (EXPECTED(Z_TYPE(call->object) != IS_UNDEF) &&
	    EXPECTED(Z_TYPE(call->object) == IS_OBJECT)) {
		call->called_scope = Z_OBJCE(call->object);

		if (OP2_TYPE != IS_CONST ||
		    (call->fbc = CACHED_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, call->called_scope)) == NULL) {
		    zend_object *object = Z_OBJ(call->object);

			if (UNEXPECTED(Z_OBJ_HT(call->object)->get_method == NULL)) {
				zend_error_noreturn(E_ERROR, "Object does not support method calls");
			}

			/* First, locate the function. */
			call->fbc = Z_OBJ_HT(call->object)->get_method(&call->object, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (opline->op2.literal + 1) : NULL) TSRMLS_CC);
			if (UNEXPECTED(call->fbc == NULL)) {
				zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(&call->object), Z_STRVAL_P(function_name));
			}
			if (OP2_TYPE == IS_CONST &&
			    EXPECTED(call->fbc->type <= ZEND_USER_FUNCTION) &&
			    EXPECTED((call->fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_HANDLER|ZEND_ACC_NEVER_CACHE)) == 0) &&
			    EXPECTED(Z_OBJ(call->object) == object)) {
				CACHE_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, call->called_scope, call->fbc);
			}
		}
	} else {
		if (UNEXPECTED(EG(exception) != NULL)) {
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		zend_error_noreturn(E_ERROR, "Call to a member function %s() on a non-object", Z_STRVAL_P(function_name));
	}

	if ((call->fbc->common.fn_flags & ZEND_ACC_STATIC) != 0) {
		ZVAL_UNDEF(&call->object);
	} else {
		if (!Z_ISREF(call->object)) {
			Z_ADDREF(call->object); /* For $this pointer */
		} else {
			ZVAL_DUP(&call->object, Z_REFVAL(call->object));
		}
	}

	call->num_additional_args = 0;
	call->is_ctor_call = 0;
	EX(call) = call;

	FREE_OP2();
	FREE_OP1_IF_VAR();

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, CONST|VAR, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zval *function_name;
	zend_class_entry *ce;
	call_slot *call = EX(call_slots) + opline->result.num;

	SAVE_OPLINE();

	if (OP1_TYPE == IS_CONST) {
		/* no function found. try a static method in class */
		if (CACHED_PTR(opline->op1.literal->cache_slot)) {
			ce = CACHED_PTR(opline->op1.literal->cache_slot);
		} else {
			ce = zend_fetch_class_by_name(Z_STR_P(opline->op1.zv), opline->op1.literal + 1, opline->extended_value TSRMLS_CC);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
			if (UNEXPECTED(ce == NULL)) {
				zend_error_noreturn(E_ERROR, "Class '%s' not found", Z_STRVAL_P(opline->op1.zv));
			}
			CACHE_PTR(opline->op1.literal->cache_slot, ce);
		}
		call->called_scope = ce;
	} else {
		ce = Z_CE_P(EX_VAR(opline->op1.var));

		if (opline->extended_value == ZEND_FETCH_CLASS_PARENT || opline->extended_value == ZEND_FETCH_CLASS_SELF) {
			call->called_scope = EG(called_scope);
		} else {
			call->called_scope = ce;
		}
	}

	if (OP1_TYPE == IS_CONST &&
	    OP2_TYPE == IS_CONST &&
	    CACHED_PTR(opline->op2.literal->cache_slot)) {
		call->fbc = CACHED_PTR(opline->op2.literal->cache_slot);
	} else if (OP1_TYPE != IS_CONST &&
	           OP2_TYPE == IS_CONST &&
	           (call->fbc = CACHED_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, ce))) {
		/* do nothing */
	} else if (OP2_TYPE != IS_UNUSED) {
		zend_free_op free_op2;

		function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);
		if (OP2_TYPE != IS_CONST) {
			if (UNEXPECTED(Z_TYPE_P(function_name) != IS_STRING)) {
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
				zend_error_noreturn(E_ERROR, "Function name must be a string");
 			}
		}

		if (ce->get_static_method) {
			call->fbc = ce->get_static_method(ce, Z_STR_P(function_name) TSRMLS_CC);
		} else {
			call->fbc = zend_std_get_static_method(ce, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (opline->op2.literal + 1) : NULL) TSRMLS_CC);
		}
		if (UNEXPECTED(call->fbc == NULL)) {
			zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", ce->name->val, Z_STRVAL_P(function_name));
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(call->fbc->type <= ZEND_USER_FUNCTION) &&
		    EXPECTED((call->fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_HANDLER|ZEND_ACC_NEVER_CACHE)) == 0)) {
			if (OP1_TYPE == IS_CONST) {
				CACHE_PTR(opline->op2.literal->cache_slot, call->fbc);
			} else {
				CACHE_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, ce, call->fbc);
			}
		}
		if (OP2_TYPE != IS_CONST) {
			FREE_OP2();
		}
	} else {
		if (UNEXPECTED(ce->constructor == NULL)) {
			zend_error_noreturn(E_ERROR, "Cannot call constructor");
		}
		if (Z_TYPE(EG(This)) != IS_UNDEF && Z_OBJCE(EG(This)) != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_error_noreturn(E_ERROR, "Cannot call private %s::__construct()", ce->name->val);
		}
		call->fbc = ce->constructor;
	}

	if (call->fbc->common.fn_flags & ZEND_ACC_STATIC) {
		ZVAL_UNDEF(&call->object);
	} else {
		if (Z_TYPE(EG(This)) != IS_UNDEF &&
		    Z_OBJ_HT(EG(This))->get_class_entry &&
		    !instanceof_function(Z_OBJCE(EG(This)), ce TSRMLS_CC)) {
		    /* We are calling method of the other (incompatible) class,
		       but passing $this. This is done for compatibility with php-4. */
			if (call->fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				zend_error(E_DEPRECATED, "Non-static method %s::%s() should not be called statically, assuming $this from incompatible context", call->fbc->common.scope->name->val, call->fbc->common.function_name->val);
			} else {
				/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
				zend_error_noreturn(E_ERROR, "Non-static method %s::%s() cannot be called statically, assuming $this from incompatible context", call->fbc->common.scope->name->val, call->fbc->common.function_name->val);
			}
		}
		ZVAL_COPY(&call->object, &EG(This));
	}

	call->num_additional_args = 0;
	call->is_ctor_call = 0;
	EX(call) = call;

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zval *function_name, *func;
	call_slot *call = EX(call_slots) + opline->result.num;

	if (OP2_TYPE == IS_CONST) {
		function_name = (zval*)(opline->op2.literal+1);
		if (CACHED_PTR(opline->op2.literal->cache_slot)) {
			call->fbc = CACHED_PTR(opline->op2.literal->cache_slot);
		} else if (UNEXPECTED((func = zend_hash_find(EG(function_table), Z_STR_P(function_name))) == NULL)) {
			SAVE_OPLINE();
			zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL_P(opline->op2.zv));
		} else {
			call->fbc = Z_FUNC_P(func);
			CACHE_PTR(opline->op2.literal->cache_slot, call->fbc);
		}

		ZVAL_UNDEF(&call->object);
		call->called_scope = NULL;
		call->num_additional_args = 0;
		call->is_ctor_call = 0;
		EX(call) = call;

		/*CHECK_EXCEPTION();*/
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_string *lcname;
		zend_free_op free_op2;

		SAVE_OPLINE();
		function_name = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);

		if (EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
			if (Z_STRVAL_P(function_name)[0] == '\\') {
				lcname = STR_ALLOC(Z_STRLEN_P(function_name) - 1, 0);
				zend_str_tolower_copy(lcname->val, Z_STRVAL_P(function_name) + 1, Z_STRLEN_P(function_name) - 1);
			} else {
				lcname = STR_ALLOC(Z_STRLEN_P(function_name), 0);
				zend_str_tolower_copy(lcname->val, Z_STRVAL_P(function_name), Z_STRLEN_P(function_name));
			}
			if (UNEXPECTED((func = zend_hash_find(EG(function_table), lcname)) == NULL)) {
				zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL_P(function_name));
			}
			STR_FREE(lcname);
			FREE_OP2();

			call->fbc = Z_FUNC_P(func);
			ZVAL_UNDEF(&call->object);
			call->called_scope = NULL;
			call->num_additional_args = 0;
			call->is_ctor_call = 0;
			EX(call) = call;

			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		} else if (OP2_TYPE != IS_CONST && OP2_TYPE != IS_TMP_VAR &&
		    EXPECTED(Z_TYPE_P(function_name) == IS_OBJECT) &&
			Z_OBJ_HANDLER_P(function_name, get_closure) &&
			Z_OBJ_HANDLER_P(function_name, get_closure)(function_name, &call->called_scope, &call->fbc, &call->object TSRMLS_CC) == SUCCESS) {
			if (Z_REFCOUNTED(call->object)) {
				Z_ADDREF(call->object);
			}
			if (OP2_TYPE == IS_VAR && OP2_FREE && Z_REFCOUNT_P(function_name) == 1 &&
			    call->fbc->common.fn_flags & ZEND_ACC_CLOSURE) {
				/* Delay closure destruction until its invocation */
				call->fbc->common.prototype = (zend_function*)function_name;
			} else {
				FREE_OP2();
			}

			call->num_additional_args = 0;
			call->is_ctor_call = 0;
			EX(call) = call;

			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		} else if (OP2_TYPE != IS_CONST &&
				EXPECTED(Z_TYPE_P(function_name) == IS_ARRAY) &&
				zend_hash_num_elements(Z_ARRVAL_P(function_name)) == 2) {
			zval *obj;
			zval *method;

			obj = zend_hash_index_find(Z_ARRVAL_P(function_name), 0);
			method = zend_hash_index_find(Z_ARRVAL_P(function_name), 1);

			if (!obj || !method) {
				zend_error_noreturn(E_ERROR, "Array callback has to contain indices 0 and 1");
			}

			if (Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) {
				zend_error_noreturn(E_ERROR, "First array member is not a valid class name or object");
			}

			if (Z_TYPE_P(method) != IS_STRING) {
				zend_error_noreturn(E_ERROR, "Second array member is not a valid method");
			}

			if (Z_TYPE_P(obj) == IS_STRING) {
				ZVAL_UNDEF(&call->object);
				call->called_scope = zend_fetch_class_by_name(Z_STR_P(obj), NULL, 0 TSRMLS_CC);
				if (UNEXPECTED(call->called_scope == NULL)) {
					CHECK_EXCEPTION();
					ZEND_VM_NEXT_OPCODE();
				}

				if (call->called_scope->get_static_method) {
					call->fbc = call->called_scope->get_static_method(call->called_scope, Z_STR_P(method) TSRMLS_CC);
				} else {
					call->fbc = zend_std_get_static_method(call->called_scope, Z_STR_P(method), NULL TSRMLS_CC);
				}
				if (UNEXPECTED(call->fbc == NULL)) {
					zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", call->called_scope->name->val, Z_STRVAL_P(method));
				}
			} else {
				call->called_scope = Z_OBJCE_P(obj);
				ZVAL_COPY_VALUE(&call->object, obj);

				call->fbc = Z_OBJ_HT_P(obj)->get_method(obj, Z_STR_P(method), NULL TSRMLS_CC);
				if (UNEXPECTED(call->fbc == NULL)) {
					zend_error_noreturn(E_ERROR, "Call to undefined method %s::%s()", Z_OBJ_CLASS_NAME_P(obj), Z_STRVAL_P(method));
				}

				if ((call->fbc->common.fn_flags & ZEND_ACC_STATIC) != 0) {
					ZVAL_UNDEF(&call->object);
				} else {
					if (!Z_ISREF(call->object)) {
						Z_ADDREF(call->object); /* For $this pointer */
					} else {
						ZVAL_DUP(&call->object, Z_REFVAL(call->object));
					}
				}
			}

			call->num_additional_args = 0;
			call->is_ctor_call = 0;
			EX(call) = call;

			FREE_OP2();
			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		} else {
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
			zend_error_noreturn(E_ERROR, "Function name must be a string");
			ZEND_VM_NEXT_OPCODE(); /* Never reached */
		}
	}
}


ZEND_VM_HANDLER(69, ZEND_INIT_NS_FCALL_BY_NAME, ANY, CONST)
{
	USE_OPLINE
	zend_literal *func_name;
	zval *func;
	call_slot *call = EX(call_slots) + opline->result.num;

	func_name = opline->op2.literal + 1;
	if (CACHED_PTR(opline->op2.literal->cache_slot)) {
		call->fbc = CACHED_PTR(opline->op2.literal->cache_slot);
	} else if ((func = zend_hash_find(EG(function_table), Z_STR(func_name->constant))) == NULL) {
		func_name++;
		if (UNEXPECTED((func = zend_hash_find(EG(function_table), Z_STR(func_name->constant))) == NULL)) {
			SAVE_OPLINE();
			zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL_P(opline->op2.zv));
		} else {
			call->fbc = Z_FUNC_P(func);
			CACHE_PTR(opline->op2.literal->cache_slot, call->fbc);
		}
	} else {
		call->fbc = Z_FUNC_P(func);
		CACHE_PTR(opline->op2.literal->cache_slot, call->fbc);
	}

	ZVAL_UNDEF(&call->object);
	call->called_scope = NULL;
	call->num_additional_args = 0;
	call->is_ctor_call = 0;
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(61, ZEND_DO_FCALL_BY_NAME, ANY, ANY)
{
	EX(function_state).function = EX(call)->fbc;
	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}

ZEND_VM_HANDLER(60, ZEND_DO_FCALL, CONST, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *fname = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *func;
	call_slot *call = EX(call_slots) + opline->op2.num;

	if (CACHED_PTR(opline->op1.literal->cache_slot)) {
		EX(function_state).function = CACHED_PTR(opline->op1.literal->cache_slot);
	} else if (UNEXPECTED((func = zend_hash_find(EG(function_table), Z_STR_P(fname))) == NULL)) {
	    SAVE_OPLINE();
		zend_error_noreturn(E_ERROR, "Call to undefined function %s()", Z_STRVAL_P(fname));
	} else {
		EX(function_state).function = Z_FUNC_P(func);
		CACHE_PTR(opline->op1.literal->cache_slot, EX(function_state).function);
	}

	call->fbc = EX(function_state).function;
	ZVAL_UNDEF(&call->object);
	call->called_scope = NULL;
	call->num_additional_args = 0;
	call->is_ctor_call = 0;
	EX(call) = call;

	FREE_OP1();

	ZEND_VM_DISPATCH_TO_HELPER(zend_do_fcall_common_helper);
}

ZEND_VM_HANDLER(62, ZEND_RETURN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *retval_ptr;
	zend_free_op free_op1;

	SAVE_OPLINE();
	retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (!EX(return_value)) {
		FREE_OP1();
	} else {
		if (OP1_TYPE == IS_CONST ||
		    OP1_TYPE == IS_TMP_VAR ||
		    Z_ISREF_P(retval_ptr)) {
			zval ret;

			ZVAL_COPY_VALUE(&ret, retval_ptr);
			if (OP1_TYPE != IS_TMP_VAR) {
				zval_copy_ctor(&ret);
			}
			ZVAL_COPY_VALUE(EX(return_value), &ret);
			FREE_OP1_IF_VAR();
		} else {
			ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
			if (OP1_TYPE == IS_CV) {
				if (IS_REFCOUNTED(Z_TYPE_P(retval_ptr))) Z_ADDREF_P(retval_ptr);
			}
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_HANDLER(111, ZEND_RETURN_BY_REF, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *retval_ptr;
	zend_free_op free_op1;

	SAVE_OPLINE();

	do {
		if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR || 
		    (OP1_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_VALUE)) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");

			retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
			if (!EX(return_value)) {
				if (OP1_TYPE == IS_TMP_VAR) {
					FREE_OP1();
				}
			} else if (!IS_OP1_TMP_FREE()) { /* Not a temp var */
				ZVAL_DUP(EX(return_value), retval_ptr);
			} else {
				ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
			}
			break;
		}

		retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);

		if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(retval_ptr) == IS_STR_OFFSET)) {
			zend_error_noreturn(E_ERROR, "Cannot return string offsets by reference");
		}

		if (OP1_TYPE == IS_VAR && !Z_ISREF_P(retval_ptr)) {
//???			if (opline->extended_value == ZEND_RETURNS_FUNCTION &&
//???			    EX_T(opline->op1.var).var.fcall_returned_reference) {
//???			} else if (EX_T(opline->op1.var).var.ptr_ptr == &EX_T(opline->op1.var).var.ptr) {
			zend_error(E_NOTICE, "Only variable references should be returned by reference");
//???				if (EX(return_value)) {
//???					ZVAL_DUP(EX(return_value), retval_ptr);
//???				}
//???				break;
//???			}
		}

		if (EX(return_value)) {
			SEPARATE_ZVAL_TO_MAKE_IS_REF(retval_ptr);
			ZVAL_COPY(EX(return_value), retval_ptr);
		}
	} while (0);

	FREE_OP1_VAR_PTR();
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_HANDLER(161, ZEND_GENERATOR_RETURN, ANY, ANY)
{
	/* The generator object is stored in EX(return_value) */
	zend_generator *generator = (zend_generator *) EX(return_value);

	/* Close the generator to free up resources */
	zend_generator_close(generator, 1 TSRMLS_CC);

	/* Pass execution back to handling code */
	ZEND_VM_RETURN();
}

ZEND_VM_HANDLER(108, ZEND_THROW, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *value;
	zval exception;
	zend_free_op free_op1;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST || UNEXPECTED(Z_TYPE_P(value) != IS_OBJECT)) {
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
		zend_error_noreturn(E_ERROR, "Can only throw objects");
	}

	zend_exception_save(TSRMLS_C);
	/* Not sure if a complete copy is what we want here */
	ZVAL_COPY_VALUE(&exception, value);
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(&exception);
	}

	zend_throw_exception_object(&exception TSRMLS_CC);
	zend_exception_restore(TSRMLS_C);
	FREE_OP1_IF_VAR();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, CONST, CV)
{
	USE_OPLINE
	zend_class_entry *ce, *catch_ce;
	zend_object *exception;

	SAVE_OPLINE();
	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore(TSRMLS_C);
	if (EG(exception) == NULL) {
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
	}
	if (CACHED_PTR(opline->op1.literal->cache_slot)) {
		catch_ce = CACHED_PTR(opline->op1.literal->cache_slot);
	} else {
		catch_ce = zend_fetch_class_by_name(Z_STR_P(opline->op1.zv), opline->op1.literal + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD TSRMLS_CC);

		CACHE_PTR(opline->op1.literal->cache_slot, catch_ce);
	}
//???	ce = Z_OBJCE_P(EG(exception));
	ce = EG(exception)->ce;

#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_CAUGHT_ENABLED()) {
		DTRACE_EXCEPTION_CAUGHT((char *)ce->name);
	}
#endif /* HAVE_DTRACE */

	if (ce != catch_ce) {
		if (!instanceof_function(ce, catch_ce TSRMLS_CC)) {
			if (opline->result.num) {
				zend_throw_exception_internal(NULL TSRMLS_CC);
				HANDLE_EXCEPTION();
			}
			ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->extended_value]);
			ZEND_VM_CONTINUE(); /* CHECK_ME */
		}
	}

	exception = EG(exception);
	if (Z_REFCOUNTED_P(EX_VAR_NUM(opline->op2.var))) {
		zval_ptr_dtor(EX_VAR_NUM(opline->op2.var));
	}
	ZVAL_OBJ(EX_VAR_NUM(opline->op2.var), EG(exception));
	if (EG(active_symbol_table)) {
		zend_string *cv = CV_DEF_OF(opline->op2.var);
		zval *zv = zend_hash_update(EG(active_symbol_table), cv, EX_VAR_NUM(opline->op2.var));
		ZVAL_INDIRECT(EX_VAR_NUM(opline->op2.var), zv);
	}
	if (UNEXPECTED(EG(exception) != exception)) {
		EG(exception)->gc.refcount++;
		HANDLE_EXCEPTION();
	} else {
		EG(exception) = NULL;
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(65, ZEND_SEND_VAL, CONST|TMP, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (opline->extended_value == ZEND_DO_FCALL_BY_NAME) {
		int arg_num = opline->op2.num + EX(call)->num_additional_args;
		if (ARG_MUST_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
			zend_error_noreturn(E_ERROR, "Cannot pass parameter %d by reference", arg_num);
		}
	}

	{
		zval valptr;
		zval *value;
		zend_free_op free_op1;

		value = GET_OP1_ZVAL_PTR(BP_VAR_R);

		ZVAL_COPY_VALUE(&valptr, value);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(&valptr);
		}
		zend_vm_stack_push(&valptr TSRMLS_CC);
		FREE_OP1_IF_VAR();
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_send_by_var_helper, VAR|CV, ANY)
{
	USE_OPLINE
	zval *varptr, var;
	zend_free_op free_op1;

	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	if (Z_ISREF_P(varptr)) {
//???		if (OP1_TYPE == IS_CV ||
//???		    (OP1_TYPE == IS_VAR && Z_REFCOUNT_P(varptr) > 2)) {
			ZVAL_DUP(&var, Z_REFVAL_P(varptr));
			varptr = &var;
			FREE_OP1();
//???		} else {
//???			varptr = Z_REFVAL_P(varptr);
//???		}
	} else if (OP1_TYPE == IS_CV) {
		if (IS_REFCOUNTED(Z_TYPE_P(varptr))) Z_ADDREF_P(varptr);
	}
	zend_vm_stack_push(varptr TSRMLS_CC);

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(106, ZEND_SEND_VAR_NO_REF, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr;
	int arg_num;

	SAVE_OPLINE();
	if (opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) { /* Had function_ptr at compile_time */
		if (!(opline->extended_value & ZEND_ARG_SEND_BY_REF)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
		}
	} else {
		arg_num = opline->op2.num + EX(call)->num_additional_args;
		if (!ARG_SHOULD_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
		}
	}

	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
//???	if ((!(opline->extended_value & ZEND_ARG_SEND_FUNCTION) ||
//???	     EX_T(opline->op1.var).var.fcall_returned_reference) &&
//???	    (Z_ISREF_P(varptr) || Z_REFCOUNT_P(varptr) == 1)) {
//???		Z_SET_ISREF_P(varptr);
//???		if (OP1_TYPE == IS_CV) {
//???			Z_ADDREF_P(varptr);
//???		}
//???		zend_vm_stack_push(varptr TSRMLS_CC);
//???	} else {
		zval val;

		if ((opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) ?
			!(opline->extended_value & ZEND_ARG_SEND_SILENT) :
			!ARG_MAY_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
			zend_error(E_STRICT, "Only variables should be passed by reference");
		}
		ZVAL_COPY_VALUE(&val, varptr);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(&val);
		}
		FREE_OP1_IF_VAR();
		zend_vm_stack_push(&val TSRMLS_CC);
//???	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(67, ZEND_SEND_REF, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr;

	SAVE_OPLINE();
	varptr = GET_OP1_ZVAL_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(varptr) == IS_STR_OFFSET)) {
		zend_error_noreturn(E_ERROR, "Only variables can be passed by reference");
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(varptr == &EG(error_zval))) {
		zend_vm_stack_push(&EG(uninitialized_zval) TSRMLS_CC);
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	if (opline->extended_value == ZEND_DO_FCALL_BY_NAME &&
	    EX(function_state).function->type == ZEND_INTERNAL_FUNCTION) { 
		int arg_num = opline->op2.num + EX(call)->num_additional_args;
		if (!ARG_SHOULD_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
		}
	}

	SEPARATE_ZVAL_TO_MAKE_IS_REF(varptr);
	Z_ADDREF_P(varptr);
	zend_vm_stack_push(varptr TSRMLS_CC);

	FREE_OP1_VAR_PTR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(66, ZEND_SEND_VAR, VAR|CV, ANY)
{
	USE_OPLINE

	if (opline->extended_value == ZEND_DO_FCALL_BY_NAME) {
		int arg_num = opline->op2.num + EX(call)->num_additional_args;
		if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
			ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
		}
	}
	SAVE_OPLINE();
	ZEND_VM_DISPATCH_TO_HELPER(zend_send_by_var_helper);
}

ZEND_VM_HANDLER(165, ZEND_SEND_UNPACK, ANY, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *args;
	int arg_num;
	SAVE_OPLINE();

	args = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg_num = opline->op2.num + EX(call)->num_additional_args + 1;

	switch (Z_TYPE_P(args)) {
		case IS_ARRAY: {
			HashTable *ht = Z_ARRVAL_P(args);
			HashPosition pos;
			zval *arg;

			ZEND_VM_STACK_GROW_IF_NEEDED(zend_hash_num_elements(ht));

			for (zend_hash_internal_pointer_reset_ex(ht, &pos);
			     (arg = zend_hash_get_current_data_ex(ht, &pos)) != NULL;
			     zend_hash_move_forward_ex(ht, &pos), ++arg_num
			) {
				zend_string *name;
				zend_ulong index;

				if (zend_hash_get_current_key_ex(ht, &name, &index, 0, &pos) == HASH_KEY_IS_STRING) {
					zend_error(E_RECOVERABLE_ERROR, "Cannot unpack array with string keys");
					FREE_OP1();
					CHECK_EXCEPTION();
					ZEND_VM_NEXT_OPCODE();
				}

				if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(arg);
					Z_ADDREF_P(arg);
				} else if (Z_ISREF_P(arg)) {
					ZVAL_DUP(arg, Z_REFVAL_P(arg));
				} else {
					if (Z_REFCOUNTED_P(arg)) Z_ADDREF_P(arg);
				}

				zend_vm_stack_push(arg TSRMLS_CC);
				EX(call)->num_additional_args++;
			}
			break;
		}
		case IS_OBJECT: {
			zend_class_entry *ce = Z_OBJCE_P(args);
			zend_object_iterator *iter;

			if (!ce || !ce->get_iterator) {
				zend_error(E_WARNING, "Only arrays and Traversables can be unpacked");
				break;
			}

			iter = ce->get_iterator(ce, args, 0 TSRMLS_CC);
			if (UNEXPECTED(!iter)) {
				FREE_OP1();
				if (!EG(exception)) {
					zend_throw_exception_ex(
						NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name->val
					);
				}
				HANDLE_EXCEPTION();
			}

			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}
			}

			for (; iter->funcs->valid(iter TSRMLS_CC) == SUCCESS; ++arg_num) {
				zval *arg;

				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}

				arg = iter->funcs->get_current_data(iter TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}

				if (iter->funcs->get_current_key) {
					zval key;
					iter->funcs->get_current_key(iter, &key TSRMLS_CC);
					if (UNEXPECTED(EG(exception) != NULL)) {
						ZEND_VM_C_GOTO(unpack_iter_dtor);
					}

					if (Z_TYPE(key) == IS_STRING) {
						zend_error(E_RECOVERABLE_ERROR,
							"Cannot unpack Traversable with string keys");
						zval_dtor(&key);
						ZEND_VM_C_GOTO(unpack_iter_dtor);
					}

					zval_dtor(&key);
				}

				if (ARG_MUST_BE_SENT_BY_REF(EX(call)->fbc, arg_num)) {
					zend_error(
						E_WARNING, "Cannot pass by-reference argument %d of %s%s%s()"
						" by unpacking a Traversable, passing by-value instead", arg_num,
						EX(call)->fbc->common.scope ? EX(call)->fbc->common.scope->name->val : "",
						EX(call)->fbc->common.scope ? "::" : "",
						EX(call)->fbc->common.function_name->val
					);
				}
				
				if (Z_ISREF_P(arg)) {
//???					ALLOC_ZVAL(arg);
//???					MAKE_COPY_ZVAL(arg_ptr, arg);
					ZVAL_DUP(arg, Z_REFVAL_P(arg));
				} else {
					if (Z_REFCOUNTED_P(arg)) Z_ADDREF_P(arg);
				}

				ZEND_VM_STACK_GROW_IF_NEEDED(1);
				zend_vm_stack_push(arg TSRMLS_CC);
				EX(call)->num_additional_args++;

				iter->funcs->move_forward(iter TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}
			}

ZEND_VM_C_LABEL(unpack_iter_dtor):
			iter->funcs->dtor(iter TSRMLS_CC);
			break;
		}
		default:
			zend_error(E_WARNING, "Only arrays and Traversables can be unpacked");
	}

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(63, ZEND_RECV, ANY, ANY)
{
	USE_OPLINE
	zend_uint arg_num = opline->op1.num;
	zval *param = zend_vm_stack_get_arg(arg_num TSRMLS_CC);

	SAVE_OPLINE();
	if (UNEXPECTED(param == NULL)) {
		if (zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, NULL, opline->extended_value TSRMLS_CC)) {
			const char *space;
			const char *class_name;
			zend_execute_data *ptr;

			if (EG(active_op_array)->scope) {
				class_name = EG(active_op_array)->scope->name->val;
				space = "::";
			} else {
				class_name = space = "";
			}
			ptr = EX(prev_execute_data);

			if(ptr && ptr->op_array) {
				zend_error(E_WARNING, "Missing argument %u for %s%s%s(), called in %s on line %d and defined", opline->op1.num, class_name, space, get_active_function_name(TSRMLS_C), ptr->op_array->filename->val, ptr->opline->lineno);
			} else {
				zend_error(E_WARNING, "Missing argument %u for %s%s%s()", opline->op1.num, class_name, space, get_active_function_name(TSRMLS_C));
			}
		}
	} else {
		zval *var_ptr;

		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, param, opline->extended_value TSRMLS_CC);
		var_ptr = _get_zval_ptr_cv_BP_VAR_W(execute_data, opline->result.var TSRMLS_CC);
		if (IS_REFCOUNTED(Z_TYPE_P(var_ptr))) Z_DELREF_P(var_ptr);
		ZVAL_COPY(var_ptr, param);
	}

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(64, ZEND_RECV_INIT, ANY, CONST)
{
	USE_OPLINE
	zend_uint arg_num = opline->op1.num;
	zval *param = zend_vm_stack_get_arg(arg_num TSRMLS_CC);
	zval *var_ptr;

	SAVE_OPLINE();
	var_ptr = _get_zval_ptr_cv_BP_VAR_W(execute_data, opline->result.var TSRMLS_CC);
	zval_ptr_dtor(var_ptr);
	if (param == NULL) {
		ZVAL_COPY_VALUE(var_ptr, opline->op2.zv);
		if (IS_CONSTANT_TYPE(Z_TYPE_P(var_ptr))) {
			zval_update_constant(var_ptr, 0 TSRMLS_CC);
		} else {
			zval_copy_ctor(var_ptr);
		}
	} else {
		ZVAL_COPY(var_ptr, param);
	}

	zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, var_ptr, opline->extended_value TSRMLS_CC);

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(164, ZEND_RECV_VARIADIC, ANY, ANY)
{
	USE_OPLINE
	zend_uint arg_num = opline->op1.num;
	zend_uint arg_count = zend_vm_stack_get_args_count(TSRMLS_C);
	zval *params;

	SAVE_OPLINE();

	params = _get_zval_ptr_cv_BP_VAR_W(execute_data, opline->result.var TSRMLS_CC);
	if (Z_REFCOUNTED_P(params)) Z_DELREF_P(params);

	if (arg_num <= arg_count) {
		array_init_size(params, arg_count - arg_num + 1);
	} else {
		array_init(params);
	}

	for (; arg_num <= arg_count; ++arg_num) {
		zval *param = zend_vm_stack_get_arg(arg_num TSRMLS_CC);
		zend_verify_arg_type((zend_function *) EG(active_op_array), arg_num, param, opline->extended_value TSRMLS_CC);
//??? "params" may became IS_INDIRECT because of symtable initialization in zend_error
		if (Z_TYPE_P(params) == IS_INDIRECT) {
			params = Z_INDIRECT_P(params);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(params), param);
		if (Z_REFCOUNTED_P(param)) {
			Z_ADDREF_P(param);
		}
	}

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(52, ZEND_BOOL, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *retval = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	/* PHP 3.0 returned "" for false and 1 for true, here we use 0 and 1 for now */
	ZVAL_BOOL(retval, i_zend_is_true(GET_OP1_ZVAL_PTR(BP_VAR_R) TSRMLS_CC));
	FREE_OP1();

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(50, ZEND_BRK, ANY, CONST)
{
	USE_OPLINE
	zend_brk_cont_element *el;

	SAVE_OPLINE();
	el = zend_brk_cont(Z_LVAL_P(opline->op2.zv), opline->op1.opline_num,
	                   EX(op_array), execute_data TSRMLS_CC);
	ZEND_VM_JMP(EX(op_array)->opcodes + el->brk);
}

ZEND_VM_HANDLER(51, ZEND_CONT, ANY, CONST)
{
	USE_OPLINE
	zend_brk_cont_element *el;

	SAVE_OPLINE();
	el = zend_brk_cont(Z_LVAL_P(opline->op2.zv), opline->op1.opline_num,
	                   EX(op_array), execute_data TSRMLS_CC);
	ZEND_VM_JMP(EX(op_array)->opcodes + el->cont);
}

ZEND_VM_HANDLER(100, ZEND_GOTO, ANY, CONST)
{
	zend_op *brk_opline;
	USE_OPLINE
	zend_brk_cont_element *el;

	SAVE_OPLINE();
	el = zend_brk_cont(Z_LVAL_P(opline->op2.zv), opline->extended_value,
 	                   EX(op_array), execute_data TSRMLS_CC);

	brk_opline = EX(op_array)->opcodes + el->brk;

	switch (brk_opline->opcode) {
		case ZEND_SWITCH_FREE:
			if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
				zval_ptr_dtor(EX_VAR(brk_opline->op1.var));
			}
			break;
		case ZEND_FREE:
			if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
				zval_dtor(EX_VAR(brk_opline->op1.var));
			}
			break;
	}
	ZEND_VM_JMP(opline->op1.jmp_addr);
}

ZEND_VM_HANDLER(48, ZEND_CASE, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();
	is_equal_function(EX_VAR(opline->result.var),
				 GET_OP1_ZVAL_PTR(BP_VAR_R),
				 GET_OP2_ZVAL_PTR(BP_VAR_R) TSRMLS_CC);

	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(49, ZEND_SWITCH_FREE, VAR, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zval_ptr_dtor(EX_VAR(opline->op1.var));
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(68, ZEND_NEW, ANY, ANY)
{
	USE_OPLINE
	zval object_zval;
	zend_function *constructor;

	SAVE_OPLINE();
	if (UNEXPECTED((Z_CE_P(EX_VAR(opline->op1.var))->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) != 0)) {
		if (Z_CE_P(EX_VAR(opline->op1.var))->ce_flags & ZEND_ACC_INTERFACE) {
			zend_error_noreturn(E_ERROR, "Cannot instantiate interface %s", Z_CE_P(EX_VAR(opline->op1.var))->name->val);
		} else if ((Z_CE_P(EX_VAR(opline->op1.var))->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
			zend_error_noreturn(E_ERROR, "Cannot instantiate trait %s", Z_CE_P(EX_VAR(opline->op1.var))->name->val);
		} else {
			zend_error_noreturn(E_ERROR, "Cannot instantiate abstract class %s", Z_CE_P(EX_VAR(opline->op1.var))->name->val);
		}
	}
	object_init_ex(&object_zval, Z_CE_P(EX_VAR(opline->op1.var)));
	constructor = Z_OBJ_HT(object_zval)->get_constructor(&object_zval TSRMLS_CC);

	if (constructor == NULL) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), &object_zval);
		} else {
			zval_ptr_dtor(&object_zval);
		}
		ZEND_VM_JMP(EX(op_array)->opcodes + opline->op2.opline_num);
	} else {
		call_slot *call = EX(call_slots) + opline->extended_value;

		if (RETURN_VALUE_USED(opline)) {
			ZVAL_COPY(EX_VAR(opline->result.var), &object_zval);
		}

		/* We are not handling overloaded classes right now */
		call->fbc = constructor;
		ZVAL_COPY_VALUE(&call->object, &object_zval);
		call->called_scope = Z_CE_P(EX_VAR(opline->op1.var));
		call->num_additional_args = 0;
		call->is_ctor_call = 1;
		call->is_ctor_result_used = RETURN_VALUE_USED(opline);
		EX(call) = call;

		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(110, ZEND_CLONE, CONST|TMP|VAR|UNUSED|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *obj;
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	SAVE_OPLINE();
	obj = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    UNEXPECTED(Z_TYPE_P(obj) != IS_OBJECT)) {
		if (UNEXPECTED(EG(exception) != NULL)) {
			HANDLE_EXCEPTION();
		}
		zend_error_noreturn(E_ERROR, "__clone method called on non-object");
	}

	ce = Z_OBJCE_P(obj);
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (UNEXPECTED(clone_call == NULL)) {
		if (ce) {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object of class %s", ce->name->val);
		} else {
			zend_error_noreturn(E_ERROR, "Trying to clone an uncloneable object");
		}
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (UNEXPECTED(ce != EG(scope))) {
				zend_error_noreturn(E_ERROR, "Call to private %s::__clone() from context '%s'", ce->name->val, EG(scope) ? EG(scope)->name->val : "");
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (UNEXPECTED(!zend_check_protected(zend_get_function_root_class(clone), EG(scope)))) {
				zend_error_noreturn(E_ERROR, "Call to protected %s::__clone() from context '%s'", ce->name->val, EG(scope) ? EG(scope)->name->val : "");
			}
		}
	}

	if (EXPECTED(EG(exception) == NULL)) {
		ZVAL_OBJ(EX_VAR(opline->result.var), clone_call(obj TSRMLS_CC));
//???		Z_SET_ISREF_P(retval);
		if (!RETURN_VALUE_USED(opline) || UNEXPECTED(EG(exception) != NULL)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
	}
	FREE_OP1_IF_VAR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(99, ZEND_FETCH_CONSTANT, VAR|CONST|UNUSED, CONST)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE == IS_UNUSED) {
		zend_constant *c;
		zval *retval;

		if (CACHED_PTR(opline->op2.literal->cache_slot)) {
			c = CACHED_PTR(opline->op2.literal->cache_slot);
		} else if ((c = zend_quick_get_constant(opline->op2.literal + 1, opline->extended_value TSRMLS_CC)) == NULL) {
			if ((opline->extended_value & IS_CONSTANT_UNQUALIFIED) != 0) {
				char *actual = (char *)zend_memrchr(Z_STRVAL_P(opline->op2.zv), '\\', Z_STRLEN_P(opline->op2.zv));
				if(!actual) {
					actual = Z_STRVAL_P(opline->op2.zv);
				} else {
					actual++;
				}
				/* non-qualified constant - allow text substitution */
				zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'", actual, actual);
				ZVAL_STRINGL(EX_VAR(opline->result.var), actual, Z_STRLEN_P(opline->op2.zv)-(actual - Z_STRVAL_P(opline->op2.zv)));
				CHECK_EXCEPTION();
				ZEND_VM_NEXT_OPCODE();
			} else {
				zend_error_noreturn(E_ERROR, "Undefined constant '%s'", Z_STRVAL_P(opline->op2.zv));
			}
		} else {
			CACHE_PTR(opline->op2.literal->cache_slot, c);
		}
		retval = EX_VAR(opline->result.var);
		ZVAL_DUP(retval, &c->value);
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	} else {
		/* class constant */
		zend_class_entry *ce;
		zval *value;

		if (OP1_TYPE == IS_CONST) {
			if (CACHED_PTR(opline->op2.literal->cache_slot)) {
				value = CACHED_PTR(opline->op2.literal->cache_slot);
				ZVAL_DUP(EX_VAR(opline->result.var), value);
				CHECK_EXCEPTION();
				ZEND_VM_NEXT_OPCODE();
			} else if (CACHED_PTR(opline->op1.literal->cache_slot)) {
				ce = CACHED_PTR(opline->op1.literal->cache_slot);
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(opline->op1.zv), opline->op1.literal + 1, opline->extended_value TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(ce == NULL)) {
					zend_error_noreturn(E_ERROR, "Class '%s' not found", Z_STRVAL_P(opline->op1.zv));
				}
				CACHE_PTR(opline->op1.literal->cache_slot, ce);
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op1.var));
			if ((value = CACHED_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, ce)) != NULL) {
				ZVAL_DUP(EX_VAR(opline->result.var), value);
				CHECK_EXCEPTION();
				ZEND_VM_NEXT_OPCODE();
			}
		}

		if (EXPECTED((value = zend_hash_find(&ce->constants_table, Z_STR_P(opline->op2.zv))) != NULL)) {
			if (IS_CONSTANT_TYPE(Z_TYPE_P(value))) {
				zend_class_entry *old_scope = EG(scope);

				EG(scope) = ce;
				zval_update_constant(value, (void *) 1 TSRMLS_CC);
				EG(scope) = old_scope;
			}
			if (OP1_TYPE == IS_CONST) {
				CACHE_PTR(opline->op2.literal->cache_slot, value);
			} else {
				CACHE_POLYMORPHIC_PTR(opline->op2.literal->cache_slot, ce, value);
			}
			ZVAL_DUP(EX_VAR(opline->result.var), value);
		} else if (Z_STRLEN_P(opline->op2.zv) == sizeof("class")-1 && memcmp(Z_STRVAL_P(opline->op2.zv), "class", sizeof("class") - 1) == 0) {
			/* "class" is assigned as a case-sensitive keyword from zend_do_resolve_class_name */
			ZVAL_STR(EX_VAR(opline->result.var), ce->name);
			STR_ADDREF(ce->name);
		} else {
			zend_error_noreturn(E_ERROR, "Undefined class constant '%s'", Z_STRVAL_P(opline->op2.zv));
		}

		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(72, ZEND_ADD_ARRAY_ELEMENT, CONST|TMP|VAR|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr_ptr;

	SAVE_OPLINE();
	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && opline->extended_value) {
		expr_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);
		if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(expr_ptr) == IS_STR_OFFSET)) {
			zend_error_noreturn(E_ERROR, "Cannot create references to/from string offsets");
		}
		SEPARATE_ZVAL_TO_MAKE_IS_REF(expr_ptr);
		if (Z_REFCOUNTED_P(expr_ptr)) {
			Z_ADDREF_P(expr_ptr);
		}
	} else {
		expr_ptr=GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
		if (IS_OP1_TMP_FREE()) { /* temporary variable */
			zval new_expr;

			ZVAL_COPY_VALUE(&new_expr, expr_ptr);
			expr_ptr = &new_expr;
		} else if (OP1_TYPE == IS_CONST || Z_ISREF_P(expr_ptr)) {
			zval new_expr;
            
            ZVAL_DUP(&new_expr, expr_ptr);
			expr_ptr = &new_expr;
			FREE_OP1_IF_VAR();
		} else if (OP1_TYPE == IS_CV && Z_REFCOUNTED_P(expr_ptr)) {
			Z_ADDREF_P(expr_ptr);
		}
	}

	if (OP2_TYPE != IS_UNUSED) {
		zend_free_op free_op2;
		zval *offset = GET_OP2_ZVAL_PTR(BP_VAR_R);
		ulong hval;

		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				hval = zend_dval_to_lval(Z_DVAL_P(offset));
				ZEND_VM_C_GOTO(num_index);
			case IS_LONG:
			case IS_BOOL:
				hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index):
				zend_hash_index_update(Z_ARRVAL_P(EX_VAR(opline->result.var)), hval, expr_ptr);
				break;
			case IS_STRING:
				if (OP2_TYPE != IS_CONST) {
					ZEND_HANDLE_NUMERIC_EX(Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, hval, ZEND_VM_C_GOTO(num_index));
				}
				zend_hash_update(Z_ARRVAL_P(EX_VAR(opline->result.var)), Z_STR_P(offset), expr_ptr);
				break;
			case IS_NULL:
				zend_hash_update(Z_ARRVAL_P(EX_VAR(opline->result.var)), STR_EMPTY_ALLOC(), expr_ptr);
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				zval_ptr_dtor(expr_ptr);
				/* do nothing */
				break;
		}
		FREE_OP2();
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(EX_VAR(opline->result.var)), expr_ptr);
	}
	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && opline->extended_value) {
		FREE_OP1_VAR_PTR();
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|UNUSED|CV, CONST|TMP|VAR|UNUSED|CV)
{
	USE_OPLINE

	array_init(EX_VAR(opline->result.var));
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
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (opline->extended_value != IS_STRING) {
		ZVAL_COPY_VALUE(result, expr);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(result);
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
				ZVAL_COPY_VALUE(result, &var_copy);
				if (IS_OP1_TMP_FREE()) {
					FREE_OP1();
				}
			} else {
				ZVAL_COPY_VALUE(result, expr);
				if (!IS_OP1_TMP_FREE()) {
					zval_copy_ctor(result);
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
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(73, ZEND_INCLUDE_OR_EVAL, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_op_array *new_op_array=NULL;
	zend_free_op free_op1;
	zval *inc_filename;
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	SAVE_OPLINE();
	inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);

	ZVAL_UNDEF(&tmp_inc_filename);
	if (inc_filename->type!=IS_STRING) {
		ZVAL_DUP(&tmp_inc_filename, inc_filename);
		convert_to_string(&tmp_inc_filename);
		inc_filename = &tmp_inc_filename;
	}

	if (opline->extended_value != ZEND_EVAL && strlen(Z_STRVAL_P(inc_filename)) != Z_STRLEN_P(inc_filename)) {
		if (opline->extended_value == ZEND_INCLUDE_ONCE || opline->extended_value == ZEND_INCLUDE) {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
		} else {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename) TSRMLS_CC);
		}
	} else {
		switch (opline->extended_value) {
			case ZEND_INCLUDE_ONCE:
			case ZEND_REQUIRE_ONCE: {
					zend_file_handle file_handle;
					char *resolved_path;

					resolved_path = zend_resolve_path(Z_STRVAL_P(inc_filename), Z_STRLEN_P(inc_filename) TSRMLS_CC);
					if (resolved_path) {
						failure_retval = zend_hash_str_exists(&EG(included_files), resolved_path, strlen(resolved_path));
					} else {
						resolved_path = Z_STRVAL_P(inc_filename);
					}

					if (failure_retval) {
						/* do nothing, file already included */
					} else if (SUCCESS == zend_stream_open(resolved_path, &file_handle TSRMLS_CC)) {

						if (!file_handle.opened_path) {
							file_handle.opened_path = estrdup(resolved_path);
						}

						if (zend_hash_str_add_empty_element(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path))) {
							new_op_array = zend_compile_file(&file_handle, (opline->extended_value==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE) TSRMLS_CC);
							zend_destroy_file_handle(&file_handle TSRMLS_CC);
						} else {
							zend_file_handle_dtor(&file_handle TSRMLS_CC);
							failure_retval=1;
						}
					} else {
						if (opline->extended_value == ZEND_INCLUDE_ONCE) {
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
				new_op_array = compile_filename(opline->extended_value, inc_filename TSRMLS_CC);
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
	if (Z_TYPE(tmp_inc_filename) != IS_UNDEF) {
		zval_ptr_dtor(&tmp_inc_filename);
	}
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	} else if (EXPECTED(new_op_array != NULL)) {
		zval *return_value = NULL;

		EG(active_op_array) = new_op_array;
		if (RETURN_VALUE_USED(opline)) {
			return_value = EX_VAR(opline->result.var);
		}

		EX(function_state).function = (zend_function *) new_op_array;
		ZVAL_UNDEF(&EX(object));

		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}

		if (EXPECTED(zend_execute_ex == execute_ex)) {
		    i_create_execute_data_from_op_array(new_op_array, return_value, 1 TSRMLS_CC);
			ZEND_VM_ENTER();
		} else {
			zend_execute(new_op_array, return_value TSRMLS_CC);
		}

		EX(function_state).function = (zend_function *) EX(op_array);

		EG(opline_ptr) = &EX(opline);
		EG(active_op_array) = EX(op_array);
		destroy_op_array(new_op_array TSRMLS_CC);
		efree(new_op_array);
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_throw_exception_internal(NULL TSRMLS_CC);
			HANDLE_EXCEPTION();
		}

	} else if (RETURN_VALUE_USED(opline)) {
		ZVAL_BOOL(EX_VAR(opline->result.var), failure_retval);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV &&
	    OP2_TYPE == IS_UNUSED &&
	    (opline->extended_value & ZEND_QUICK_SET)) {
		if (EG(active_symbol_table)) {
			zend_string *cv = CV_DEF_OF(opline->op1.var);

			zend_delete_variable(EX(prev_execute_data), EG(active_symbol_table), cv TSRMLS_CC);
			ZVAL_UNDEF(EX_VAR_NUM(opline->op1.var));
		} else if (Z_TYPE_P(EX_VAR_NUM(opline->op1.var)) != IS_UNDEF) {
			zval_ptr_dtor(EX_VAR_NUM(opline->op1.var));
			ZVAL_UNDEF(EX_VAR_NUM(opline->op1.var));
		}
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}

	varname = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
		ZVAL_DUP(&tmp, varname);
		convert_to_string(&tmp);
		varname = &tmp;
	} else if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		Z_ADDREF_P(varname);
	}

	if (OP2_TYPE != IS_UNUSED) {
		zend_class_entry *ce;

		if (OP2_TYPE == IS_CONST) {
			if (CACHED_PTR(opline->op2.literal->cache_slot)) {
				ce = CACHED_PTR(opline->op2.literal->cache_slot);
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(opline->op2.zv), opline->op2.literal + 1, 0 TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					if (OP1_TYPE != IS_CONST && varname == &tmp) {
						zval_dtor(&tmp);
					} else if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
						zval_ptr_dtor(varname);
					}
					FREE_OP1();
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(ce == NULL)) {
					zend_error_noreturn(E_ERROR, "Class '%s' not found", Z_STRVAL_P(opline->op2.zv));
				}
				CACHE_PTR(opline->op2.literal->cache_slot, ce);
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		zend_std_unset_static_property(ce, Z_STR_P(varname), ((OP1_TYPE == IS_CONST) ? opline->op1.literal : NULL) TSRMLS_CC);
	} else {
		target_symbol_table = zend_get_target_symbol_table(opline->extended_value & ZEND_FETCH_TYPE_MASK TSRMLS_CC);
//???: STRING may become INDIRECT
		if (Z_TYPE_P(varname) == IS_INDIRECT) {
			varname = Z_INDIRECT_P(varname);
		}
		zend_delete_variable(execute_data, target_symbol_table, Z_STR_P(varname) TSRMLS_CC);
	}

	if (OP1_TYPE != IS_CONST && varname == &tmp) {
		zval_dtor(&tmp);
	} else if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		zval_ptr_dtor(varname);
	}
	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(75, ZEND_UNSET_DIM, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;
	ulong hval;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_UNSET);
	if (OP1_TYPE == IS_CV) {
		SEPARATE_ZVAL_IF_NOT_REF(container);
	}
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_VAR || container) {
//???deref
		if (Z_TYPE_P(container) == IS_REFERENCE) {
			container = Z_REFVAL_P(container);
		}
		switch (Z_TYPE_P(container)) {
			case IS_ARRAY: {
				HashTable *ht = Z_ARRVAL_P(container);

				switch (Z_TYPE_P(offset)) {
					case IS_DOUBLE:
						hval = zend_dval_to_lval(Z_DVAL_P(offset));
						zend_hash_index_del(ht, hval);
						break;
					case IS_RESOURCE:
					case IS_BOOL:
					case IS_LONG:
						hval = Z_LVAL_P(offset);
						zend_hash_index_del(ht, hval);
						break;
					case IS_STRING:
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							Z_ADDREF_P(offset);
						}
						if (OP2_TYPE != IS_CONST) {
							ZEND_HANDLE_NUMERIC_EX(Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, hval, ZEND_VM_C_GOTO(num_index_dim));
						}
						if (ht == &EG(symbol_table).ht) {
							zend_delete_global_variable(Z_STR_P(offset) TSRMLS_CC);
						} else {
							zend_hash_del(ht, Z_STR_P(offset));
						}
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							zval_ptr_dtor(offset);
						}
						break;
ZEND_VM_C_LABEL(num_index_dim):
						zend_hash_index_del(ht, hval);
						if (OP2_TYPE == IS_CV || OP2_TYPE == IS_VAR) {
							zval_ptr_dtor(offset);
						}
						break;
					case IS_NULL:
						zend_hash_del(ht, STR_EMPTY_ALLOC());
						break;
					default:
						zend_error(E_WARNING, "Illegal offset type in unset");
						break;
				}
				FREE_OP2();
				break;
			}
			case IS_OBJECT:
				if (UNEXPECTED(Z_OBJ_HT_P(container)->unset_dimension == NULL)) {
					zend_error_noreturn(E_ERROR, "Cannot use object as array");
				}
				if (IS_OP2_TMP_FREE()) {
					MAKE_REAL_ZVAL_PTR(offset);
				}
				Z_OBJ_HT_P(container)->unset_dimension(container, offset TSRMLS_CC);
				if (IS_OP2_TMP_FREE()) {
					zval_ptr_dtor(offset);
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

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_UNSET);
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_VAR || container) {
		if (OP1_TYPE == IS_CV) {
			SEPARATE_ZVAL_IF_NOT_REF(container);
		}
		if (Z_TYPE_P(container) == IS_OBJECT) {
			if (IS_OP2_TMP_FREE()) {
				MAKE_REAL_ZVAL_PTR(offset);
			}
			if (Z_OBJ_HT_P(container)->unset_property) {
				Z_OBJ_HT_P(container)->unset_property(container, offset, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
			} else {
				zend_error(E_NOTICE, "Trying to unset property of non-object");
			}
			if (IS_OP2_TMP_FREE()) {
				zval_ptr_dtor(offset);
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

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(77, ZEND_FE_RESET, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *array_ptr;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;
	zend_class_entry *ce = NULL;
	zend_bool is_empty = 0;
	zval *array_ref = NULL;

	SAVE_OPLINE();

	if ((OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) &&
	    (opline->extended_value & ZEND_FE_RESET_VARIABLE)) {
		array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (Z_ISREF_P(array_ptr)) {
			array_ref = array_ptr;
			array_ptr = Z_REFVAL_P(array_ptr);
		}
		if (Z_TYPE_P(array_ptr) == IS_NULL) {
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			if(Z_OBJ_HT_P(array_ptr)->get_class_entry == NULL) {
				zend_error(E_WARNING, "foreach() cannot iterate over objects without PHP class");
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
			}

			ce = Z_OBJCE_P(array_ptr);
			if (!ce || ce->get_iterator == NULL) {
				if (!array_ref) {
					SEPARATE_ZVAL(array_ptr);
				}
				Z_ADDREF_P(array_ptr);
			}
		} else {
			if (Z_TYPE_P(array_ptr) == IS_ARRAY) {
				if (!array_ref) {
					SEPARATE_ZVAL(array_ptr);
					if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
						ZVAL_NEW_REF(array_ptr, array_ptr);
						array_ptr = Z_REFVAL_P(array_ptr);						
					}
				}
			}
			if (Z_REFCOUNTED_P(array_ptr)) Z_ADDREF_P(array_ptr);
		}
	} else {
		array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (Z_ISREF_P(array_ptr)) {
			array_ref = array_ptr;
			array_ptr = Z_REFVAL_P(array_ptr);
		}
		if (IS_OP1_TMP_FREE()) { /* IS_TMP_VAR */
			zval tmp;

			ZVAL_COPY_VALUE(&tmp, array_ptr);
			array_ptr = &tmp;
			if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
				ce = Z_OBJCE_P(array_ptr);
				if (ce && ce->get_iterator) {
					Z_DELREF_P(array_ptr);
				}
			}
		} else if (Z_TYPE_P(array_ptr) == IS_OBJECT) {
			ce = Z_OBJCE_P(array_ptr);
			if (!ce || !ce->get_iterator) {
				if (OP1_TYPE == IS_CV) {
					Z_ADDREF_P(array_ptr);
				}
			}
		} else if (Z_REFCOUNTED_P(array_ptr)) {
			if (OP1_TYPE == IS_CONST ||
			           (OP1_TYPE == IS_CV && 
			            (array_ref == NULL) &&
			            Z_REFCOUNT_P(array_ptr) > 1) ||
			           (OP1_TYPE == IS_VAR &&
			            (array_ref == NULL) &&
			            Z_REFCOUNT_P(array_ptr) > 2)) {
				zval tmp;

				if (OP1_TYPE == IS_VAR) {
					Z_DELREF_P(array_ptr);
				}
				ZVAL_DUP(&tmp, array_ptr);
				array_ptr = &tmp;
			} else if (OP1_TYPE == IS_CV) {
				Z_ADDREF_P(array_ptr);
			}
		}
	}

	if (ce && ce->get_iterator) {
		iter = ce->get_iterator(ce, array_ptr, opline->extended_value & ZEND_FE_RESET_REFERENCE TSRMLS_CC);

		if (OP1_TYPE == IS_VAR && !(opline->extended_value & ZEND_FE_RESET_VARIABLE)) {
			FREE_OP1_IF_VAR();
		}
		if (iter && EXPECTED(EG(exception) == NULL)) {
			zval iterator;

			array_ptr = &iterator;
			ZVAL_OBJ(array_ptr, &iter->std);
		} else {
			if (OP1_TYPE == IS_VAR && opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				FREE_OP1_VAR_PTR();
			}
			if (!EG(exception)) {
				zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Object of type %s did not create an Iterator", ce->name->val);
			}
			zend_throw_exception_internal(NULL TSRMLS_CC);
			HANDLE_EXCEPTION();
		}
	}

	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), array_ptr);

	if (iter) {
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter TSRMLS_CC);
			if (UNEXPECTED(EG(exception) != NULL)) {
				zval_ptr_dtor(array_ptr);
				if (OP1_TYPE == IS_VAR && opline->extended_value & ZEND_FE_RESET_VARIABLE) {
					FREE_OP1_VAR_PTR();
				}
				HANDLE_EXCEPTION();
			}
		}
		is_empty = iter->funcs->valid(iter TSRMLS_CC) != SUCCESS;
		if (UNEXPECTED(EG(exception) != NULL)) {
			zval_ptr_dtor(array_ptr);
			if (OP1_TYPE == IS_VAR && opline->extended_value & ZEND_FE_RESET_VARIABLE) {
				FREE_OP1_VAR_PTR();
			}
			HANDLE_EXCEPTION();
		}
		iter->index = -1; /* will be set to 0 before using next handler */
	} else if ((fe_ht = HASH_OF(array_ptr)) != NULL) {
		zend_hash_internal_pointer_reset(fe_ht);
		if (ce) {
			zend_object *zobj = Z_OBJ_P(array_ptr);
			while (zend_hash_has_more_elements(fe_ht) == SUCCESS) {
				zend_string *str_key;
				ulong int_key;
				zend_uchar key_type;

				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &int_key, 0, NULL);
				if (key_type != HASH_KEY_NON_EXISTENT &&
					(key_type == HASH_KEY_IS_LONG ||
				     zend_check_property_access(zobj, str_key TSRMLS_CC) == SUCCESS)) {
					break;
				}
				zend_hash_move_forward(fe_ht);
			}
		}
		is_empty = zend_hash_has_more_elements(fe_ht) != SUCCESS;
//???		zend_hash_get_pointer(fe_ht, EX_VAR(opline->result.var));
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		is_empty = 1;
	}

	if (OP1_TYPE == IS_VAR && opline->extended_value & ZEND_FE_RESET_VARIABLE) {
		FREE_OP1_VAR_PTR();
	}
	if (is_empty) {
		ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
	} else {
		CHECK_EXCEPTION();
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(78, ZEND_FE_FETCH, VAR, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *array = EX_VAR(opline->op1.var);
	zval *value;
	HashTable *fe_ht;
	zend_object_iterator *iter = NULL;

	zval *key = NULL;
	if (opline->extended_value & ZEND_FE_FETCH_WITH_KEY) {
		key = EX_VAR((opline+1)->result.var);
	}

	SAVE_OPLINE();

	switch (zend_iterator_unwrap(array, &iter TSRMLS_CC)) {
		default:
		case ZEND_ITER_INVALID:
			zend_error(E_WARNING, "Invalid argument supplied for foreach()");
			ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);

		case ZEND_ITER_PLAIN_OBJECT: {
			zend_object *zobj = Z_OBJ_P(array);
			int key_type;
			zend_string *str_key;
			zend_ulong int_key;

			fe_ht = Z_OBJPROP_P(array);
//???			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.var).fe.fe_pos);
			do {
				if ((value = zend_hash_get_current_data(fe_ht)) == NULL) {
					/* reached end of iteration */
					ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
				}
				key_type = zend_hash_get_current_key_ex(fe_ht, &str_key, &int_key, 0, NULL);

				zend_hash_move_forward(fe_ht);
			} while (key_type != HASH_KEY_IS_LONG &&
			         zend_check_property_access(zobj, str_key TSRMLS_CC) != SUCCESS);

			if (key) {
				if (key_type == HASH_KEY_IS_LONG) {
					ZVAL_LONG(key, int_key);
				} else {
					const char *class_name, *prop_name;
					int prop_name_len;
					zend_unmangle_property_name_ex(
						str_key->val, str_key->len, &class_name, &prop_name, &prop_name_len
					);
					ZVAL_STRINGL(key, prop_name, prop_name_len);
				}
			}

//???			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.var).fe.fe_pos);
			break;
		}

		case ZEND_ITER_PLAIN_ARRAY:
			fe_ht = Z_ARRVAL_P(array);
//???			zend_hash_set_pointer(fe_ht, &EX_T(opline->op1.var).fe.fe_pos);
			if ((value = zend_hash_get_current_data(fe_ht)) == NULL) {
				/* reached end of iteration */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
			}
			if (key) {
				zend_hash_get_current_key_zval(fe_ht, key);
			}
			zend_hash_move_forward(fe_ht);
//???			zend_hash_get_pointer(fe_ht, &EX_T(opline->op1.var).fe.fe_pos);
			break;

		case ZEND_ITER_OBJECT:
			/* !iter happens from exception */
			if (iter && ++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter TSRMLS_CC);
				if (UNEXPECTED(EG(exception) != NULL)) {
					zval_ptr_dtor(array);
					HANDLE_EXCEPTION();
				}
			}
			/* If index is zero we come from FE_RESET and checked valid() already. */
			if (!iter || (iter->index > 0 && iter->funcs->valid(iter TSRMLS_CC) == FAILURE)) {
				/* reached end of iteration */
				if (UNEXPECTED(EG(exception) != NULL)) {
					zval_ptr_dtor(array);
					HANDLE_EXCEPTION();
				}
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
			}
			value = iter->funcs->get_current_data(iter TSRMLS_CC);
			if (UNEXPECTED(EG(exception) != NULL)) {
				zval_ptr_dtor(array);
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_JMP(EX(op_array)->opcodes+opline->op2.opline_num);
			}
			if (key) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, key TSRMLS_CC);
					if (UNEXPECTED(EG(exception) != NULL)) {
						zval_ptr_dtor(array);
						HANDLE_EXCEPTION();
					}
				} else {
					ZVAL_LONG(key, iter->index);
				}
			}
			break;
	}

	if (opline->extended_value & ZEND_FE_FETCH_BYREF) {
		SEPARATE_ZVAL_TO_MAKE_IS_REF(value);
	}
	ZVAL_COPY(EX_VAR(opline->result.var), value);

	CHECK_EXCEPTION();
	ZEND_VM_INC_OPCODE();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMP|VAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE
	zval *value;
	zend_bool isset = 1;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV &&
	    OP2_TYPE == IS_UNUSED &&
	    (opline->extended_value & ZEND_QUICK_SET)) {
		if (Z_TYPE_P(EX_VAR_NUM(opline->op1.var)) != IS_UNDEF) {
			value = EX_VAR_NUM(opline->op1.var);
			if (Z_TYPE_P(value) == IS_INDIRECT) {
				value = Z_INDIRECT_P(value);
			}
			if (Z_TYPE_P(value) == IS_REFERENCE) {
				value = Z_REFVAL_P(value);
			}
		} else if (EG(active_symbol_table)) {
			zend_string *cv = CV_DEF_OF(opline->op1.var);

			if ((value = zend_hash_find(EG(active_symbol_table), cv)) == NULL) {
				isset = 0;
			}
		} else {
			isset = 0;
		}
	} else {
		HashTable *target_symbol_table;
		zend_free_op free_op1;
		zval tmp, *varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);

		if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
			ZVAL_DUP(&tmp, varname);
			convert_to_string(&tmp);
			varname = &tmp;
		}

		if (OP2_TYPE != IS_UNUSED) {
			zend_class_entry *ce;

			if (OP2_TYPE == IS_CONST) {
				if (CACHED_PTR(opline->op2.literal->cache_slot)) {
					ce = CACHED_PTR(opline->op2.literal->cache_slot);
				} else {
					ce = zend_fetch_class_by_name(Z_STR_P(opline->op2.zv), opline->op2.literal + 1, 0 TSRMLS_CC);
					if (UNEXPECTED(ce == NULL)) {
						CHECK_EXCEPTION();
						ZEND_VM_NEXT_OPCODE();
					}
					CACHE_PTR(opline->op2.literal->cache_slot, ce);
				}
			} else {
				ce = Z_CE_P(EX_VAR(opline->op2.var));
			}
			value = zend_std_get_static_property(ce, Z_STR_P(varname), 1, ((OP1_TYPE == IS_CONST) ? opline->op1.literal : NULL) TSRMLS_CC);
			if (!value) {
				isset = 0;
			}
		} else {
			target_symbol_table = zend_get_target_symbol_table(opline->extended_value & ZEND_FETCH_TYPE_MASK TSRMLS_CC);
//???: STRING may become INDIRECT
			if (Z_TYPE_P(varname) == IS_INDIRECT) {
				varname = Z_INDIRECT_P(varname);
			}
			if ((value = zend_hash_find(target_symbol_table, Z_STR_P(varname))) == NULL) {
				isset = 0;
			}
		}

		if (OP1_TYPE != IS_CONST && varname == &tmp) {
			zval_dtor(&tmp);
		}
		FREE_OP1();
	}

	if (opline->extended_value & ZEND_ISSET) {
		if (isset && Z_TYPE_P(value) != IS_NULL) {
			ZVAL_BOOL(EX_VAR(opline->result.var), 1);
		} else {
			ZVAL_BOOL(EX_VAR(opline->result.var), 0);
		}
	} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
		if (!isset || !i_zend_is_true(value TSRMLS_CC)) {
			ZVAL_BOOL(EX_VAR(opline->result.var), 1);
		} else {
			ZVAL_BOOL(EX_VAR(opline->result.var), 0);
		}
	}

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER_EX(zend_isset_isempty_dim_prop_obj_handler, VAR|UNUSED|CV, CONST|TMP|VAR|CV, int prop_dim)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *value;
	int result = 0;
	ulong hval;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(container) == IS_REFERENCE) {
		container = Z_REFVAL_P(container);
	}
	if (Z_TYPE_P(container) == IS_ARRAY && !prop_dim) {
		HashTable *ht;
		int isset = 0;

		ht = Z_ARRVAL_P(container);

		switch (Z_TYPE_P(offset)) {
			case IS_DOUBLE:
				hval = zend_dval_to_lval(Z_DVAL_P(offset));
				ZEND_VM_C_GOTO(num_index_prop);
			case IS_RESOURCE:
			case IS_BOOL:
			case IS_LONG:
				hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_prop):
				if ((value = zend_hash_index_find(ht, hval)) != NULL) {
					isset = 1;
				}
				break;
			case IS_STRING:
				if (OP2_TYPE != IS_CONST) {
					ZEND_HANDLE_NUMERIC_EX(Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, hval, ZEND_VM_C_GOTO(num_index_prop));
				}
				if ((value = zend_hash_find(ht, Z_STR_P(offset))) != NULL) {
					isset = 1;
				}
				break;
			case IS_NULL:
				if ((value = zend_hash_find(ht, STR_EMPTY_ALLOC())) != NULL) {
					isset = 1;
				}
				break;
			default:
				zend_error(E_WARNING, "Illegal offset type in isset or empty");
				break;
		}

		if (opline->extended_value & ZEND_ISSET) {
			if (isset && Z_TYPE_P(value) == IS_NULL) {
				result = 0;
			} else {
				result = isset;
			}
		} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
			if (!isset || !i_zend_is_true(value TSRMLS_CC)) {
				result = 0;
			} else {
				result = 1;
			}
		}
		FREE_OP2();
	} else if (Z_TYPE_P(container) == IS_OBJECT) {
		if (IS_OP2_TMP_FREE()) {
			MAKE_REAL_ZVAL_PTR(offset);
		}
		if (prop_dim) {
			if (Z_OBJ_HT_P(container)->has_property) {
				result = Z_OBJ_HT_P(container)->has_property(container, offset, (opline->extended_value & ZEND_ISEMPTY) != 0, ((OP2_TYPE == IS_CONST) ? opline->op2.literal : NULL) TSRMLS_CC);
			} else {
				zend_error(E_NOTICE, "Trying to check property of non-object");
				result = 0;
			}
		} else {
			if (Z_OBJ_HT_P(container)->has_dimension) {
				result = Z_OBJ_HT_P(container)->has_dimension(container, offset, (opline->extended_value & ZEND_ISEMPTY) != 0 TSRMLS_CC);
			} else {
				zend_error(E_NOTICE, "Trying to check element of non-array");
				result = 0;
			}
		}
		if (IS_OP2_TMP_FREE()) {
			zval_ptr_dtor(offset);
		} else {
			FREE_OP2();
		}
	} else if (Z_TYPE_P(container) == IS_STRING && !prop_dim) { /* string offsets */
		zval tmp;

		if (Z_TYPE_P(offset) != IS_LONG) {
			if (!Z_REFCOUNTED_P(offset) /* simple scalar types */
					|| (Z_TYPE_P(offset) == IS_STRING /* or numeric string */
						&& IS_LONG == is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), NULL, NULL, 0))) {
				ZVAL_DUP(&tmp, offset);
				convert_to_long(&tmp);
				offset = &tmp;
			} else {
				/* can not be converted to proper offset, return "not set" */
				result = 0;
			}
		}
		if (Z_TYPE_P(offset) == IS_LONG) {
			if (opline->extended_value & ZEND_ISSET) {
				if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_P(container)) {
					result = 1;
				}
			} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
				if (offset->value.lval >= 0 && offset->value.lval < Z_STRLEN_P(container) && Z_STRVAL_P(container)[offset->value.lval] != '0') {
					result = 1;
				}
			}
		}
		FREE_OP2();
	} else {
		FREE_OP2();
	}

	if (opline->extended_value & ZEND_ISSET) {
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
	} else {
		ZVAL_BOOL(EX_VAR(opline->result.var), !result);
	}

	FREE_OP1_IF_VAR();

	CHECK_EXCEPTION();
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
	USE_OPLINE

	SAVE_OPLINE();
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
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}

ZEND_VM_HANDLER(57, ZEND_BEGIN_SILENCE, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	ZVAL_LONG(EX_VAR(opline->result.var), EG(error_reporting));
	if (Z_TYPE(EX(old_error_reporting)) == IS_UNDEF) {
		ZVAL_LONG(&EX(old_error_reporting), EG(error_reporting));
	}

	if (EG(error_reporting)) {
		do {
			EG(error_reporting) = 0;
			if (!EG(error_reporting_ini_entry)) {
				zend_ini_entry *p = zend_hash_str_find_ptr(EG(ini_directives), "error_reporting", sizeof("error_reporting")-1);
				if (p) {
					EG(error_reporting_ini_entry) = p;
				} else {
					break;
				}
			}
			if (!EG(error_reporting_ini_entry)->modified) {
				if (!EG(modified_ini_directives)) {
					ALLOC_HASHTABLE(EG(modified_ini_directives));
					zend_hash_init(EG(modified_ini_directives), 8, NULL, NULL, 0);
				}
				if (EXPECTED(zend_hash_str_add_ptr(EG(modified_ini_directives), "error_reporting", sizeof("error_reporting")-1, EG(error_reporting_ini_entry)) != NULL)) {
					EG(error_reporting_ini_entry)->orig_value = EG(error_reporting_ini_entry)->value;
					EG(error_reporting_ini_entry)->orig_value_length = EG(error_reporting_ini_entry)->value_length;
					EG(error_reporting_ini_entry)->orig_modifiable = EG(error_reporting_ini_entry)->modifiable;
					EG(error_reporting_ini_entry)->modified = 1;
				}
			} else if (EG(error_reporting_ini_entry)->value != EG(error_reporting_ini_entry)->orig_value) {
				efree(EG(error_reporting_ini_entry)->value);
			}
			EG(error_reporting_ini_entry)->value = estrndup("0", sizeof("0")-1);
			EG(error_reporting_ini_entry)->value_length = sizeof("0")-1;
		} while (0);
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(142, ZEND_RAISE_ABSTRACT_ERROR, ANY, ANY)
{
	SAVE_OPLINE();
	zend_error_noreturn(E_ERROR, "Cannot call abstract method %s::%s()", EG(scope)->name->val, EX(op_array)->function_name->val);
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}

ZEND_VM_HANDLER(58, ZEND_END_SILENCE, TMP, ANY)
{
	USE_OPLINE
	zval restored_error_reporting;

	SAVE_OPLINE();
	if (!EG(error_reporting) && Z_LVAL_P(EX_VAR(opline->op1.var)) != 0) {
		ZVAL_LONG(&restored_error_reporting, Z_LVAL_P(EX_VAR(opline->op1.var)));
		EG(error_reporting) = Z_LVAL(restored_error_reporting);
		convert_to_string(&restored_error_reporting);
		if (EXPECTED(EG(error_reporting_ini_entry) != NULL)) {
			if (EXPECTED(EG(error_reporting_ini_entry)->modified &&
			    EG(error_reporting_ini_entry)->value != EG(error_reporting_ini_entry)->orig_value)) {
				efree(EG(error_reporting_ini_entry)->value);
			}
			EG(error_reporting_ini_entry)->value = estrndup(Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting));
			EG(error_reporting_ini_entry)->value_length = Z_STRLEN(restored_error_reporting);
		} 
		zval_dtor(&restored_error_reporting);
	}
//???	if (EX(old_error_reporting) == EX_VAR(opline->op1.var)) {
//???		EX(old_error_reporting) = NULL;
//???	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (i_zend_is_true(value TSRMLS_CC)) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(EX_VAR(opline->result.var));
		}
		FREE_OP1_IF_VAR();
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.jmp_addr);
	}

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(158, ZEND_JMP_SET_VAR, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (i_zend_is_true(value TSRMLS_CC)) {
		if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		} else {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
			if (!IS_OP1_TMP_FREE()) {
				zval_copy_ctor(EX_VAR(opline->result.var));
			}
		}
		FREE_OP1_IF_VAR();
#if DEBUG_ZEND>=2
		printf("Conditional jmp to %d\n", opline->op2.opline_num);
#endif
		ZEND_VM_JMP(opline->op2.jmp_addr);
	}

	FREE_OP1();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(22, ZEND_QM_ASSIGN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
	if (!IS_OP1_TMP_FREE()) {
		zval_copy_ctor(EX_VAR(opline->result.var));
	}
	FREE_OP1_IF_VAR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(157, ZEND_QM_ASSIGN_VAR, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	} else {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
		if (!IS_OP1_TMP_FREE()) {
			zval_copy_ctor(EX_VAR(opline->result.var));
		}
	}

	FREE_OP1_IF_VAR();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(101, ZEND_EXT_STMT, ANY, ANY)
{
	SAVE_OPLINE();
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, EX(op_array) TSRMLS_CC);
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(102, ZEND_EXT_FCALL_BEGIN, ANY, ANY)
{
	SAVE_OPLINE();
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, EX(op_array) TSRMLS_CC);
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(103, ZEND_EXT_FCALL_END, ANY, ANY)
{
	SAVE_OPLINE();
	if (!EG(no_extensions)) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, EX(op_array) TSRMLS_CC);
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(139, ZEND_DECLARE_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_class(EX(op_array), opline, EG(class_table), 0 TSRMLS_CC);
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(140, ZEND_DECLARE_INHERITED_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_inherited_class(EX(op_array), opline, EG(class_table), Z_CE_P(EX_VAR(opline->extended_value)), 0 TSRMLS_CC);
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_INHERITED_CLASS_DELAYED, ANY, ANY)
{
	USE_OPLINE
	zval *zce, *orig_zce;

	SAVE_OPLINE();
	if ((zce = zend_hash_find(EG(class_table), Z_STR_P(opline->op2.zv))) == NULL ||
	    ((orig_zce = zend_hash_find(EG(class_table), Z_STR_P(opline->op1.zv))) != NULL &&
	     Z_CE_P(zce) != Z_CE_P(orig_zce))) {
		do_bind_inherited_class(EX(op_array), opline, EG(class_table), Z_CE_P(EX_VAR(opline->extended_value)), 0 TSRMLS_CC);
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(141, ZEND_DECLARE_FUNCTION, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	do_bind_function(EX(op_array), opline, EG(function_table), 0);
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(105, ZEND_TICKS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (++EG(ticks_count)>=opline->extended_value) {
		EG(ticks_count)=0;
		if (zend_ticks_function) {
			zend_ticks_function(opline->extended_value TSRMLS_CC);
		}
	}
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr;
	zend_bool result;

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->get_class_entry) {
		result = instanceof_function(Z_OBJCE_P(expr), Z_CE_P(EX_VAR(opline->op2.var)) TSRMLS_CC);
	} else {
		result = 0;
	}
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	FREE_OP1();
	CHECK_EXCEPTION();
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
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *iface;

	SAVE_OPLINE();
	if (CACHED_PTR(opline->op2.literal->cache_slot)) {
		iface = CACHED_PTR(opline->op2.literal->cache_slot);
	} else {
		iface = zend_fetch_class_by_name(Z_STR_P(opline->op2.zv), opline->op2.literal + 1, opline->extended_value TSRMLS_CC);
		if (UNEXPECTED(iface == NULL)) {
			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		}
		CACHE_PTR(opline->op2.literal->cache_slot, iface);
	}

	if (UNEXPECTED((iface->ce_flags & ZEND_ACC_INTERFACE) == 0)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ce->name->val, iface->name->val);
	}
	zend_do_implement_interface(ce, iface TSRMLS_CC);

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(154, ZEND_ADD_TRAIT, ANY, ANY)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *trait;

	SAVE_OPLINE();
	if (CACHED_PTR(opline->op2.literal->cache_slot)) {
		trait = CACHED_PTR(opline->op2.literal->cache_slot);
	} else {
		trait = zend_fetch_class_by_name(Z_STR_P(opline->op2.zv),
		                                 opline->op2.literal + 1,
		                                 opline->extended_value TSRMLS_CC);
		if (UNEXPECTED(trait == NULL)) {
			CHECK_EXCEPTION();
			ZEND_VM_NEXT_OPCODE();
		}
		if (!((trait->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT)) {
			zend_error_noreturn(E_ERROR, "%s cannot use %s - it is not a trait", ce->name->val, trait->name->val);
		}
		CACHE_PTR(opline->op2.literal->cache_slot, trait);
	}

	zend_do_implement_trait(ce, trait TSRMLS_CC);

 	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(155, ZEND_BIND_TRAITS, ANY, ANY)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));

	SAVE_OPLINE();
	zend_do_bind_traits(ce TSRMLS_CC);
 	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	zend_uint op_num = EG(opline_before_exception)-EG(active_op_array)->opcodes;
	int i;
	zend_uint catch_op_num = 0, finally_op_num = 0, finally_op_end = 0;
	zval *stack_frame;

	/* Figure out where the next stack frame (which maybe contains pushed
	 * arguments that have to be dtor'ed) starts */
	stack_frame = zend_vm_stack_frame_base(execute_data);

	/* If the exception was thrown during a function call there might be
	 * arguments pushed to the stack that have to be dtor'ed. */
	while (zend_vm_stack_top(TSRMLS_C) != stack_frame) {
		zval *stack_zval_p = zend_vm_stack_pop(TSRMLS_C);
		zval_ptr_dtor(stack_zval_p);
	}

	for (i=0; i<EG(active_op_array)->last_try_catch; i++) {
		if (EG(active_op_array)->try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (op_num < EG(active_op_array)->try_catch_array[i].catch_op) {
			catch_op_num = EX(op_array)->try_catch_array[i].catch_op;
		}
		if (op_num < EG(active_op_array)->try_catch_array[i].finally_op) {
			finally_op_num = EX(op_array)->try_catch_array[i].finally_op;
		}
		if (op_num >= EG(active_op_array)->try_catch_array[i].finally_op &&
				op_num < EG(active_op_array)->try_catch_array[i].finally_end) {
			finally_op_end = EG(active_op_array)->try_catch_array[i].finally_end;
		}
	}

	if (EX(call) >= EX(call_slots)) {
		call_slot *call = EX(call);
		do {
			if (Z_TYPE(call->object) != IS_UNDEF) {
				if (call->is_ctor_call) {
					if (call->is_ctor_result_used) {
						Z_DELREF(call->object);
					}
					if (Z_REFCOUNT(call->object) == 1) {
						zend_object_store_ctor_failed(&call->object TSRMLS_CC);
					}
				}
				zval_ptr_dtor(&call->object);
			}
			call--;
		} while (call >= EX(call_slots));
		EX(call) = NULL;
	}

	for (i=0; i<EX(op_array)->last_brk_cont; i++) {
		if (EX(op_array)->brk_cont_array[i].start < 0) {
			continue;
		} else if (EX(op_array)->brk_cont_array[i].start > op_num) {
			/* further blocks will not be relevant... */
			break;
		} else if (op_num < EX(op_array)->brk_cont_array[i].brk) {
			if (!catch_op_num ||
			    catch_op_num >= EX(op_array)->brk_cont_array[i].brk) {
				zend_op *brk_opline = &EX(op_array)->opcodes[EX(op_array)->brk_cont_array[i].brk];

				switch (brk_opline->opcode) {
					case ZEND_SWITCH_FREE:
						if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
							zval_ptr_dtor(EX_VAR(brk_opline->op1.var));
						}
						break;
					case ZEND_FREE:
						if (!(brk_opline->extended_value & EXT_TYPE_FREE_ON_RETURN)) {
							zval_dtor(EX_VAR(brk_opline->op1.var));
						}
						break;
				}
			}
		}
	}

	/* restore previous error_reporting value */
	if (!EG(error_reporting) && Z_TYPE(EX(old_error_reporting)) != IS_UNDEF && Z_LVAL(EX(old_error_reporting)) != 0) {
		zval restored_error_reporting;
		zend_string *key;

		ZVAL_LONG(&restored_error_reporting, Z_LVAL(EX(old_error_reporting)));
		convert_to_string(&restored_error_reporting);
		key = STR_INIT("error_reporting", sizeof("error_reporting")-1, 0);
		zend_alter_ini_entry_ex(key, Z_STRVAL(restored_error_reporting), Z_STRLEN(restored_error_reporting), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 1 TSRMLS_CC);
		STR_FREE(key);
		zval_dtor(&restored_error_reporting);
	}
	ZVAL_UNDEF(&EX(old_error_reporting));

	if (finally_op_num && (!catch_op_num || catch_op_num >= finally_op_num)) {
		if (EX(delayed_exception)) {
			zend_exception_set_previous(EG(exception), EX(delayed_exception) TSRMLS_CC);
		} 
		EX(delayed_exception) = EG(exception);
		EG(exception) = NULL;
		EX(fast_ret) = NULL;
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[finally_op_num]);
		ZEND_VM_CONTINUE();
	} else if (catch_op_num) {
		if (finally_op_end && catch_op_num > finally_op_end) {
			/* we are going out of current finally scope */
			if (EX(delayed_exception)) {
				zend_exception_set_previous(EG(exception), EX(delayed_exception) TSRMLS_CC);
				EX(delayed_exception) = NULL;
			}
		}
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[catch_op_num]);
		ZEND_VM_CONTINUE();
	} else {
		if (EX(delayed_exception)) {
			zend_exception_set_previous(EG(exception), EX(delayed_exception) TSRMLS_CC);
			EX(delayed_exception) = NULL;
		}
		if (UNEXPECTED((EX(op_array)->fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			ZEND_VM_DISPATCH_TO_HANDLER(ZEND_GENERATOR_RETURN);
		} else {
			ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
		}
	}
}

ZEND_VM_HANDLER(146, ZEND_VERIFY_ABSTRACT_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_verify_abstract_class(Z_CE_P(EX_VAR(opline->op1.var)) TSRMLS_CC);
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(150, ZEND_USER_OPCODE, ANY, ANY)
{
	USE_OPLINE
	int ret;

	SAVE_OPLINE();
	ret = zend_user_opcode_handlers[opline->opcode](ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_INTERNAL);
	LOAD_OPLINE();

	switch (ret) {
		case ZEND_USER_OPCODE_CONTINUE:
			ZEND_VM_CONTINUE();
		case ZEND_USER_OPCODE_RETURN:
			if (UNEXPECTED((EX(op_array)->fn_flags & ZEND_ACC_GENERATOR) != 0)) {
				ZEND_VM_DISPATCH_TO_HANDLER(ZEND_GENERATOR_RETURN);
			} else {
				ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
			}
		case ZEND_USER_OPCODE_ENTER:
			ZEND_VM_ENTER();
		case ZEND_USER_OPCODE_LEAVE:
			ZEND_VM_LEAVE();
		case ZEND_USER_OPCODE_DISPATCH:
			ZEND_VM_DISPATCH(opline->opcode, opline);
		default:
			ZEND_VM_DISPATCH((zend_uchar)(ret & 0xff), opline);
	}
}

ZEND_VM_HANDLER(143, ZEND_DECLARE_CONST, CONST, CONST)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *name;
	zval *val;
	zend_constant c;

	SAVE_OPLINE();
	name  = GET_OP1_ZVAL_PTR(BP_VAR_R);
	val   = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (IS_CONSTANT_TYPE(Z_TYPE_P(val))) {
		ZVAL_COPY_VALUE(&c.value, val);
		if (Z_TYPE_P(val) == IS_CONSTANT_ARRAY) {
			zval_copy_ctor(&c.value);
		}
		zval_update_constant(&c.value, NULL TSRMLS_CC);
	} else {
		ZVAL_DUP(&c.value, val);
	}
	c.flags = CONST_CS; /* non persistent, case sensetive */
	c.name = STR_DUP(Z_STR_P(name), 0);
	c.module_number = PHP_USER_CONSTANT;

	if (zend_register_constant(&c TSRMLS_CC) == FAILURE) {
	}

	FREE_OP1();
	FREE_OP2();
	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(153, ZEND_DECLARE_LAMBDA_FUNCTION, CONST, UNUSED)
{
	USE_OPLINE
	zval *zfunc;

	SAVE_OPLINE();

	if (UNEXPECTED((zfunc = zend_hash_find(EG(function_table), Z_STR_P(opline->op1.zv))) == NULL) ||
	    UNEXPECTED(Z_FUNC_P(zfunc)->type != ZEND_USER_FUNCTION)) {
		zend_error_noreturn(E_ERROR, "Base lambda function for closure not found");
	}

	zend_create_closure(EX_VAR(opline->result.var), Z_FUNC_P(zfunc), EG(scope), &EG(This) TSRMLS_CC);

	CHECK_EXCEPTION();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(156, ZEND_SEPARATE, VAR, UNUSED)
{
	USE_OPLINE
	zval *var_ptr;

	SAVE_OPLINE();
	var_ptr = EX_VAR(opline->op1.var);
	if (Z_TYPE_P(var_ptr) != IS_OBJECT && !Z_ISREF_P(var_ptr)) {
		if (Z_REFCOUNTED_P(var_ptr) &&
				Z_REFCOUNT_P(var_ptr) > 1) {
			Z_DELREF_P(var_ptr);
		}
		ZVAL_DUP(EX_VAR(opline->op1.var), var_ptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(160, ZEND_YIELD, CONST|TMP|VAR|CV|UNUSED, CONST|TMP|VAR|CV|UNUSED)
{
	USE_OPLINE

	/* The generator object is stored in EX(return_value) */
	zend_generator *generator = (zend_generator *) EX(return_value);

	if (generator->flags & ZEND_GENERATOR_FORCED_CLOSE) {
		zend_error_noreturn(E_ERROR, "Cannot yield from finally in a force-closed generator");
	}

	/* Destroy the previously yielded value */
	zval_ptr_dtor(&generator->value);

	/* Destroy the previously yielded key */
	zval_ptr_dtor(&generator->key);

	/* Set the new yielded value */
	if (OP1_TYPE != IS_UNUSED) {
		zend_free_op free_op1;

		if (EX(op_array)->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
			/* Constants and temporary variables aren't yieldable by reference,
			 * but we still allow them with a notice. */
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
				zval *value;

				zend_error(E_NOTICE, "Only variable references should be yielded by reference");

				value = GET_OP1_ZVAL_PTR(BP_VAR_R);
				ZVAL_COPY_VALUE(&generator->value, value);
				Z_SET_REFCOUNT(generator->value, 1);

				/* Temporary variables don't need ctor copying */
				if (!IS_OP1_TMP_FREE()) {
					zval_copy_ctor(&generator->value);
				}
			} else {
				zval *value_ptr = GET_OP1_ZVAL_PTR(BP_VAR_W);

				if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_TYPE_P(value_ptr) == IS_STR_OFFSET)) {
					zend_error_noreturn(E_ERROR, "Cannot yield string offsets by reference");
				}

				/* If a function call result is yielded and the function did
				 * not return by reference we throw a notice. */
				if (OP1_TYPE == IS_VAR && !Z_ISREF_P(value_ptr)
//???				    && !(opline->extended_value == ZEND_RETURNS_FUNCTION
//???				         && EX_T(opline->op1.var).var.fcall_returned_reference)
//???				    && EX_T(opline->op1.var).var.ptr_ptr == &EX_T(opline->op1.var).var.ptr) {
) {
					zend_error(E_NOTICE, "Only variable references should be yielded by reference");
				} else {
					SEPARATE_ZVAL_TO_MAKE_IS_REF(value_ptr);
				}
				ZVAL_COPY(&generator->value, value_ptr);

				FREE_OP1_VAR_PTR();
			}
		} else {
			zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);

			/* Consts, temporary variables and references need copying */
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR
				|| Z_ISREF_P(value)
			) {
//???				INIT_PZVAL_COPY(copy, value);
				ZVAL_COPY_VALUE(&generator->value, value);
				if (Z_REFCOUNTED(generator->value)) Z_SET_REFCOUNT(generator->value, 1);

				/* Temporary variables don't need ctor copying */
				if (!IS_OP1_TMP_FREE()) {
					zval_copy_ctor(&generator->value);
				}

				FREE_OP1_IF_VAR();
			} else {
				if (OP1_TYPE == IS_CV) {
					if (Z_REFCOUNTED_P(value)) Z_ADDREF_P(value);
				}
				ZVAL_COPY_VALUE(&generator->value, value);
			}
		}
	} else {
		/* If no value was specified yield null */
		ZVAL_NULL(&generator->value);
	}

	/* Set the new yielded key */
	if (OP2_TYPE != IS_UNUSED) {
		zend_free_op free_op2;
		zval *key = GET_OP2_ZVAL_PTR(BP_VAR_R);

		/* Consts, temporary variables and references need copying */
		if (OP2_TYPE == IS_CONST || OP2_TYPE == IS_TMP_VAR
			|| (Z_ISREF_P(key) && Z_REFCOUNT_P(key) > 0)
		) {
//???			INIT_PZVAL_COPY(copy, key);
			ZVAL_COPY_VALUE(&generator->key, key);
			if (Z_REFCOUNTED(generator->key)) Z_SET_REFCOUNT(generator->key, 1);

			/* Temporary variables don't need ctor copying */
			if (!IS_OP2_TMP_FREE()) {
				zval_copy_ctor(&generator->key);
			}
		} else {
			ZVAL_COPY(&generator->key, key);
		}

		if (Z_TYPE(generator->key) == IS_LONG
		    && Z_LVAL(generator->key) > generator->largest_used_integer_key
		) {
			generator->largest_used_integer_key = Z_LVAL(generator->key);
		}

		FREE_OP2_IF_VAR();
	} else {
		/* If no key was specified we use auto-increment keys */
		generator->largest_used_integer_key++;
		ZVAL_LONG(&generator->key, generator->largest_used_integer_key);
	}

	if (RETURN_VALUE_USED(opline)) {
		/* If the return value of yield is used set the send
		 * target and initialize it to NULL */
		generator->send_target = EX_VAR(opline->result.var);
		ZVAL_NULL(generator->send_target);
	} else {
		generator->send_target = NULL;
	}

	/* We increment to the next op, so we are at the correct position when the
	 * generator is resumed. */
	ZEND_VM_INC_OPCODE();

	/* The GOTO VM uses a local opline variable. We need to set the opline
	 * variable in execute_data so we don't resume at an old position. */
	SAVE_OPLINE();

	ZEND_VM_RETURN();
}

ZEND_VM_HANDLER(159, ZEND_DISCARD_EXCEPTION, ANY, ANY)
{
	if (EX(delayed_exception) != NULL) {
		/* discard the previously thrown exception */
//???		zval_ptr_dtor(EX(delayed_exception));
		EX(delayed_exception) = NULL;
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(162, ZEND_FAST_CALL, ANY, ANY)
{
	USE_OPLINE

	if (opline->extended_value &&
	    UNEXPECTED(EG(prev_exception) != NULL)) {
	    /* in case of unhandled exception jump to catch block instead of finally */
		ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.opline_num]);
		ZEND_VM_CONTINUE();
	}
	EX(fast_ret) = opline + 1;
	EX(delayed_exception) = NULL;
	ZEND_VM_SET_OPCODE(opline->op1.jmp_addr);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(163, ZEND_FAST_RET, ANY, ANY)
{
	if (EX(fast_ret)) {
		ZEND_VM_SET_OPCODE(EX(fast_ret));
		ZEND_VM_CONTINUE();
	} else {
		/* special case for unhandled exceptions */
		USE_OPLINE

		if (opline->extended_value == ZEND_FAST_RET_TO_FINALLY) {
			ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.opline_num]);
			ZEND_VM_CONTINUE();
		} else {
			EG(exception) = EX(delayed_exception);
			EX(delayed_exception) = NULL;
			if (opline->extended_value == ZEND_FAST_RET_TO_CATCH) {
				ZEND_VM_SET_OPCODE(&EX(op_array)->opcodes[opline->op2.opline_num]);
				ZEND_VM_CONTINUE();
			} else if (UNEXPECTED((EX(op_array)->fn_flags & ZEND_ACC_GENERATOR) != 0)) {
				ZEND_VM_DISPATCH_TO_HANDLER(ZEND_GENERATOR_RETURN);
			} else {
				ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
			}
		}
	}
}

ZEND_VM_EXPORT_HELPER(zend_do_fcall, zend_do_fcall_common_helper)
