/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* If you change this file, please regenerate the zend_vm_execute.h and
 * zend_vm_opcodes.h files by running:
 * php zend_vm_gen.php
 */

ZEND_VM_HELPER(zend_add_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	add_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(1, ZEND_ADD, CONST|TMPVARCV, CONST|TMPVARCV)
{
	USE_OPLINE
	zval *op1, *op2, *result;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			fast_long_add_function(result, op1, op2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(add_double);
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(add_double):
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, d1 + d2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(add_double);
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_add_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_sub_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	sub_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(2, ZEND_SUB, CONST|TMPVARCV, CONST|TMPVARCV)
{
	USE_OPLINE
	zval *op1, *op2, *result;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			fast_long_sub_function(result, op1, op2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(sub_double);
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(sub_double):
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, d1 - d2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(sub_double);
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_sub_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_mul_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	mul_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(3, ZEND_MUL, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			zend_long overflow;

			result = EX_VAR(opline->result.var);
			ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1), Z_LVAL_P(op2), Z_LVAL_P(result), Z_DVAL_P(result), overflow);
			Z_TYPE_INFO_P(result) = overflow ? IS_DOUBLE : IS_LONG;
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(mul_double);
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(mul_double):
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, d1 * d2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(mul_double);
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_mul_helper, op_1, op1, op_2, op2);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(4, ZEND_DIV, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	div_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_mod_by_zero_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
	ZVAL_UNDEF(EX_VAR(opline->result.var));
	HANDLE_EXCEPTION();
}

ZEND_VM_HELPER(zend_mod_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	mod_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(5, ZEND_MOD, CONST|TMPVARCV, CONST|TMPVARCV)
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			if (UNEXPECTED(Z_LVAL_P(op2) == 0)) {
				ZEND_VM_DISPATCH_TO_HELPER(zend_mod_by_zero_helper);
			} else if (UNEXPECTED(Z_LVAL_P(op2) == -1)) {
				/* Prevent overflow error/crash if op1==ZEND_LONG_MIN */
				ZVAL_LONG(result, 0);
			} else {
				ZVAL_LONG(result, Z_LVAL_P(op1) % Z_LVAL_P(op2));
			}
			ZEND_VM_NEXT_OPCODE();
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_mod_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_shift_left_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	shift_left_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(6, ZEND_SL, CONST|TMPVARCV, CONST|TMPVARCV)
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)
			&& EXPECTED((zend_ulong)Z_LVAL_P(op2) < SIZEOF_ZEND_LONG * 8)) {
		/* Perform shift on unsigned numbers to get well-defined wrap behavior. */
		ZVAL_LONG(EX_VAR(opline->result.var),
			(zend_long) ((zend_ulong) Z_LVAL_P(op1) << Z_LVAL_P(op2)));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_shift_left_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_shift_right_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	shift_right_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(7, ZEND_SR, CONST|TMPVARCV, CONST|TMPVARCV)
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)
			&& EXPECTED((zend_ulong)Z_LVAL_P(op2) < SIZEOF_ZEND_LONG * 8)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) >> Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_shift_right_helper, op_1, op1, op_2, op2);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(12, ZEND_POW, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	pow_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(8, ZEND_CONCAT, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if ((OP1_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op1) == IS_STRING)) &&
	    (OP2_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op2) == IS_STRING))) {
		zend_string *op1_str = Z_STR_P(op1);
		zend_string *op2_str = Z_STR_P(op2);
		zend_string *str;
		uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(op1_str, op2_str);

		if (OP1_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
			if (OP2_TYPE == IS_CONST || OP2_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op2_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
			}
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op1_str, 0);
			}
		} else if (OP2_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op1_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		} else if (OP1_TYPE != IS_CONST && OP1_TYPE != IS_CV &&
		    !ZSTR_IS_INTERNED(op1_str) && GC_REFCOUNT(op1_str) == 1) {
			size_t len = ZSTR_LEN(op1_str);

			if (UNEXPECTED(len > ZSTR_MAX_LEN - ZSTR_LEN(op2_str))) {
				zend_error_noreturn(E_ERROR, "Integer overflow in memory allocation");
			}
			str = zend_string_extend(op1_str, len + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str) + len, ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			GC_ADD_FLAGS(str, flags);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		} else {
			str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
			memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			GC_ADD_FLAGS(str, flags);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op1_str, 0);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		}
		ZEND_VM_NEXT_OPCODE();
	} else {
		SAVE_OPLINE();

		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			op1 = ZVAL_UNDEFINED_OP1();
		}
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
			op2 = ZVAL_UNDEFINED_OP2();
		}
		concat_function(EX_VAR(opline->result.var), op1, op2);
		FREE_OP1();
		FREE_OP2();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
}

ZEND_VM_COLD_CONSTCONST_HANDLER(16, ZEND_IS_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
	result = fast_is_identical_function(op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_HANDLER(196, ZEND_CASE_STRICT, TMP|VAR, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
	result = fast_is_identical_function(op1, op2);
	FREE_OP2();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(17, ZEND_IS_NOT_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
	result = fast_is_not_identical_function(op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_HELPER(zend_is_equal_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	int ret;
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	ret = zend_compare(op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_SMART_BRANCH(ret == 0, 1);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(18, ZEND_IS_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(SMART_BRANCH,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			if (EXPECTED(Z_LVAL_P(op1) == Z_LVAL_P(op2))) {
ZEND_VM_C_LABEL(is_equal_true):
				ZEND_VM_SMART_BRANCH_TRUE();
			} else {
ZEND_VM_C_LABEL(is_equal_false):
				ZEND_VM_SMART_BRANCH_FALSE();
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(is_equal_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(is_equal_double):
			if (d1 == d2) {
				ZEND_VM_C_GOTO(is_equal_true);
			} else {
				ZEND_VM_C_GOTO(is_equal_false);
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(is_equal_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
			bool result = zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zval_ptr_dtor_str(op1);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zval_ptr_dtor_str(op2);
			}
			if (result) {
				ZEND_VM_C_GOTO(is_equal_true);
			} else {
				ZEND_VM_C_GOTO(is_equal_false);
			}
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_is_equal_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_is_not_equal_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	int ret;
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	ret = zend_compare(op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_SMART_BRANCH(ret != 0, 1);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(19, ZEND_IS_NOT_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(SMART_BRANCH,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			if (EXPECTED(Z_LVAL_P(op1) != Z_LVAL_P(op2))) {
ZEND_VM_C_LABEL(is_not_equal_true):
				ZEND_VM_SMART_BRANCH_TRUE();
			} else {
ZEND_VM_C_LABEL(is_not_equal_false):
				ZEND_VM_SMART_BRANCH_FALSE();
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(is_not_equal_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(is_not_equal_double):
			if (d1 != d2) {
				ZEND_VM_C_GOTO(is_not_equal_true);
			} else {
				ZEND_VM_C_GOTO(is_not_equal_false);
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(is_not_equal_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
			bool result = zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zval_ptr_dtor_str(op1);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zval_ptr_dtor_str(op2);
			}
			if (!result) {
				ZEND_VM_C_GOTO(is_not_equal_true);
			} else {
				ZEND_VM_C_GOTO(is_not_equal_false);
			}
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_is_not_equal_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_is_smaller_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	int ret;
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	ret = zend_compare(op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_SMART_BRANCH(ret < 0, 1);
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(20, ZEND_IS_SMALLER, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH))
{
	USE_OPLINE
	zval *op1, *op2;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			if (EXPECTED(Z_LVAL_P(op1) < Z_LVAL_P(op2))) {
ZEND_VM_C_LABEL(is_smaller_true):
				ZEND_VM_SMART_BRANCH_TRUE();
			} else {
ZEND_VM_C_LABEL(is_smaller_false):
				ZEND_VM_SMART_BRANCH_FALSE();
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(is_smaller_double);
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(is_smaller_double):
			if (d1 < d2) {
				ZEND_VM_C_GOTO(is_smaller_true);
			} else {
				ZEND_VM_C_GOTO(is_smaller_false);
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(is_smaller_double);
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_is_smaller_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_is_smaller_or_equal_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	int ret;
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	ret = zend_compare(op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_SMART_BRANCH(ret <= 0, 1);
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(21, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH))
{
	USE_OPLINE
	zval *op1, *op2;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			if (EXPECTED(Z_LVAL_P(op1) <= Z_LVAL_P(op2))) {
ZEND_VM_C_LABEL(is_smaller_or_equal_true):
				ZEND_VM_SMART_BRANCH_TRUE();
				ZVAL_TRUE(EX_VAR(opline->result.var));
				ZEND_VM_NEXT_OPCODE();
			} else {
ZEND_VM_C_LABEL(is_smaller_or_equal_false):
				ZEND_VM_SMART_BRANCH_FALSE();
				ZVAL_FALSE(EX_VAR(opline->result.var));
				ZEND_VM_NEXT_OPCODE();
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(is_smaller_or_equal_double);
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(is_smaller_or_equal_double):
			if (d1 <= d2) {
				ZEND_VM_C_GOTO(is_smaller_or_equal_true);
			} else {
				ZEND_VM_C_GOTO(is_smaller_or_equal_false);
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(is_smaller_or_equal_double);
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_is_smaller_or_equal_helper, op_1, op1, op_2, op2);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(170, ZEND_SPACESHIP, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	compare_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_bw_or_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	bitwise_or_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(9, ZEND_BW_OR, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) | Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_bw_or_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_bw_and_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	bitwise_and_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(10, ZEND_BW_AND, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) & Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_bw_and_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HELPER(zend_bw_xor_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	bitwise_xor_function(EX_VAR(opline->result.var), op_1, op_2);
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_1);
	}
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONSTCONST_HANDLER(11, ZEND_BW_XOR, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (ZEND_VM_SPEC && OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
		/* pass */
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) ^ Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_bw_xor_helper, op_1, op1, op_2, op2);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(15, ZEND_BOOL_XOR, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	boolean_xor_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_bw_not_helper, ANY, ANY, zval *op_1)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	bitwise_not_function(EX_VAR(opline->result.var), op_1);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONST_HANDLER(13, ZEND_BW_NOT, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zval *op1;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), ~Z_LVAL_P(op1));
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_bw_not_helper, op_1, op1);
}

ZEND_VM_COLD_CONST_HANDLER(14, ZEND_BOOL_NOT, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		/* The result and op1 can be the same cv zval */
		const uint32_t orig_val_type = Z_TYPE_INFO_P(val);
		ZVAL_TRUE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(orig_val_type == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	} else {
		SAVE_OPLINE();
		ZVAL_BOOL(EX_VAR(opline->result.var), !i_zend_is_true(val));
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HELPER(zend_this_not_in_object_context_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Using $this when not in object context");
	UNDEF_RESULT();
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_undefined_function_helper, ANY, ANY)
{
	USE_OPLINE
	zval *function_name;

	SAVE_OPLINE();
	function_name = RT_CONSTANT(opline, opline->op2);
	zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(function_name));
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(28, ZEND_ASSIGN_OBJ_OP, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, OP)
{
	USE_OPLINE
	zval *object;
	zval *property;
	zval *value;
	zval *zptr;
	void **cache_slot;
	zend_property_info *prop_info;
	zend_object *zobj;
	zend_string *name, *tmp_name;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object) && Z_TYPE_P(Z_REFVAL_P(object)) == IS_OBJECT) {
				object = Z_REFVAL_P(object);
				ZEND_VM_C_GOTO(assign_op_object);
			}
			if (OP1_TYPE == IS_CV
			 && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			zend_throw_non_object_error(object, property OPLINE_CC EXECUTE_DATA_CC);
			break;
		}

ZEND_VM_C_LABEL(assign_op_object):
		/* here we are sure we are dealing with an object */
		zobj = Z_OBJ_P(object);
		if (OP2_TYPE == IS_CONST) {
			name = Z_STR_P(property);
		} else {
			name = zval_try_get_tmp_string(property, &tmp_name);
			if (UNEXPECTED(!name)) {
				UNDEF_RESULT();
				break;
			}
		}
		cache_slot = (OP2_TYPE == IS_CONST) ? CACHE_ADDR((opline+1)->extended_value) : NULL;
		if (EXPECTED((zptr = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			} else {
				zval *orig_zptr = zptr;
				zend_reference *ref;

				do {
					if (UNEXPECTED(Z_ISREF_P(zptr))) {
						ref = Z_REF_P(zptr);
						zptr = Z_REFVAL_P(zptr);
						if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
							zend_binary_assign_op_typed_ref(ref, value OPLINE_CC EXECUTE_DATA_CC);
							break;
						}
					}

					if (OP2_TYPE == IS_CONST) {
						prop_info = (zend_property_info*)CACHED_PTR_EX(cache_slot + 2);
					} else {
						prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(object), orig_zptr);
					}
					if (UNEXPECTED(prop_info)) {
						/* special case for typed properties */
						zend_binary_assign_op_typed_prop(prop_info, zptr, value OPLINE_CC EXECUTE_DATA_CC);
					} else {
						zend_binary_op(zptr, zptr, value OPLINE_CC);
					}
				} while (0);

				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
				}
			}
		} else {
			zend_assign_op_overloaded_property(zobj, name, cache_slot, value OPLINE_CC EXECUTE_DATA_CC);
		}
		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}
	} while (0);

	FREE_OP_DATA();
	FREE_OP2();
	FREE_OP1();
	/* assign_obj has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

/* No specialization for op_types (CONST|TMP|VAR|CV, UNUSED|CONST|TMPVAR) */
ZEND_VM_HANDLER(29, ZEND_ASSIGN_STATIC_PROP_OP, ANY, ANY, OP)
{
	/* This helper actually never will receive IS_VAR as second op, and has the same handling for VAR and TMP in the first op, but for interoperability with the other binary_assign_op helpers, it is necessary to "include" it */

	USE_OPLINE
	zval *prop, *value;
	zend_property_info *prop_info;
	zend_reference *ref;

	SAVE_OPLINE();

	if (UNEXPECTED(zend_fetch_static_property_address(&prop, &prop_info, (opline+1)->extended_value, BP_VAR_RW, 0 OPLINE_CC EXECUTE_DATA_CC) != SUCCESS)) {
		UNDEF_RESULT();
		FREE_OP_DATA();
		HANDLE_EXCEPTION();
	}

	value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

	do {
		if (UNEXPECTED(Z_ISREF_P(prop))) {
			ref = Z_REF_P(prop);
			prop = Z_REFVAL_P(prop);
			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_binary_assign_op_typed_ref(ref, value OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}

		if (UNEXPECTED(ZEND_TYPE_IS_SET(prop_info->type))) {
			/* special case for typed properties */
			zend_binary_assign_op_typed_prop(prop_info, prop, value OPLINE_CC EXECUTE_DATA_CC);
		} else {
			zend_binary_op(prop, prop, value OPLINE_CC);
		}
	} while (0);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), prop);
	}

	FREE_OP_DATA();
	/* assign_static_prop has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(27, ZEND_ASSIGN_DIM_OP, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, OP)
{
	USE_OPLINE
	zval *var_ptr;
	zval *value, *container, *dim;
	HashTable *ht;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(assign_dim_op_array):
		SEPARATE_ARRAY(container);
		ht = Z_ARRVAL_P(container);
ZEND_VM_C_LABEL(assign_dim_op_new_array):
		dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (OP2_TYPE == IS_UNUSED) {
			var_ptr = zend_hash_next_index_insert(ht, &EG(uninitialized_zval));
			if (UNEXPECTED(!var_ptr)) {
				zend_cannot_add_element();
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
		} else {
			if (OP2_TYPE == IS_CONST) {
				var_ptr = zend_fetch_dimension_address_inner_RW_CONST(ht, dim EXECUTE_DATA_CC);
			} else {
				var_ptr = zend_fetch_dimension_address_inner_RW(ht, dim EXECUTE_DATA_CC);
			}
			if (UNEXPECTED(!var_ptr)) {
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
		}

		value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1);

		do {
			if (OP2_TYPE != IS_UNUSED && UNEXPECTED(Z_ISREF_P(var_ptr))) {
				zend_reference *ref = Z_REF_P(var_ptr);
				var_ptr = Z_REFVAL_P(var_ptr);
				if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
					zend_binary_assign_op_typed_ref(ref, value OPLINE_CC EXECUTE_DATA_CC);
					break;
				}
			}
			zend_binary_op(var_ptr, var_ptr, value OPLINE_CC);
		} while (0);

		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
		}
		FREE_OP((opline+1)->op1_type, (opline+1)->op1.var);
	} else {
		if (EXPECTED(Z_ISREF_P(container))) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(assign_dim_op_array);
			}
		}

		if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
			zend_object *obj = Z_OBJ_P(container);

			dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			zend_binary_assign_op_obj_dim(obj, dim OPLINE_CC EXECUTE_DATA_CC);
		} else if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
			uint8_t old_type;

			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(container) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			ht = zend_new_array(8);
			old_type = Z_TYPE_P(container);
			ZVAL_ARR(container, ht);
			if (UNEXPECTED(old_type == IS_FALSE)) {
				GC_ADDREF(ht);
				zend_false_to_array_deprecated();
				if (UNEXPECTED(GC_DELREF(ht) == 0)) {
					zend_array_destroy(ht);
					ZEND_VM_C_GOTO(assign_dim_op_ret_null);
				}
			}
			ZEND_VM_C_GOTO(assign_dim_op_new_array);
		} else {
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
			zend_binary_assign_op_dim_slow(container, dim OPLINE_CC EXECUTE_DATA_CC);
ZEND_VM_C_LABEL(assign_dim_op_ret_null):
			FREE_OP_DATA();
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		}
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(26, ZEND_ASSIGN_OP, VAR|CV, CONST|TMPVAR|CV, OP)
{
	USE_OPLINE
	zval *var_ptr;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	do {
		if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_REFERENCE)) {
			zend_reference *ref = Z_REF_P(var_ptr);
			var_ptr = Z_REFVAL_P(var_ptr);
			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_binary_assign_op_typed_ref(ref, value OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}
		zend_binary_op(var_ptr, var_ptr, value OPLINE_CC);
	} while (0);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(132, ZEND_PRE_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *object;
	zval *property;
	zval *zptr;
	void **cache_slot;
	zend_property_info *prop_info;
	zend_object *zobj;
	zend_string *name, *tmp_name;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object) && Z_TYPE_P(Z_REFVAL_P(object)) == IS_OBJECT) {
				object = Z_REFVAL_P(object);
				ZEND_VM_C_GOTO(pre_incdec_object);
			}
			if (OP1_TYPE == IS_CV
			 && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			zend_throw_non_object_error(object, property OPLINE_CC EXECUTE_DATA_CC);
			break;
		}

ZEND_VM_C_LABEL(pre_incdec_object):
		/* here we are sure we are dealing with an object */
		zobj = Z_OBJ_P(object);
		if (OP2_TYPE == IS_CONST) {
			name = Z_STR_P(property);
		} else {
			name = zval_try_get_tmp_string(property, &tmp_name);
			if (UNEXPECTED(!name)) {
				UNDEF_RESULT();
				break;
			}
		}
		cache_slot = (OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL;
		if (EXPECTED((zptr = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			} else {
				if (OP2_TYPE == IS_CONST) {
					prop_info = (zend_property_info *) CACHED_PTR_EX(cache_slot + 2);
				} else {
					prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(object), zptr);
				}
				zend_pre_incdec_property_zval(zptr, prop_info OPLINE_CC EXECUTE_DATA_CC);
			}
		} else {
			zend_pre_incdec_overloaded_property(zobj, name, cache_slot OPLINE_CC EXECUTE_DATA_CC);
		}
		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(133, ZEND_PRE_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_PRE_INC_OBJ);
}

ZEND_VM_HANDLER(134, ZEND_POST_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *object;
	zval *property;
	zval *zptr;
	void **cache_slot;
	zend_property_info *prop_info;
	zend_object *zobj;
	zend_string *name, *tmp_name;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object) && Z_TYPE_P(Z_REFVAL_P(object)) == IS_OBJECT) {
				object = Z_REFVAL_P(object);
				ZEND_VM_C_GOTO(post_incdec_object);
			}
			if (OP1_TYPE == IS_CV
			 && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			zend_throw_non_object_error(object, property OPLINE_CC EXECUTE_DATA_CC);
			break;
		}

ZEND_VM_C_LABEL(post_incdec_object):
		/* here we are sure we are dealing with an object */
		zobj = Z_OBJ_P(object);
		if (OP2_TYPE == IS_CONST) {
			name = Z_STR_P(property);
		} else {
			name = zval_try_get_tmp_string(property, &tmp_name);
			if (UNEXPECTED(!name)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				break;
			}
		}
		cache_slot = (OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL;
		if (EXPECTED((zptr = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			} else {
				if (OP2_TYPE == IS_CONST) {
					prop_info = (zend_property_info*)CACHED_PTR_EX(cache_slot + 2);
				} else {
					prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(object), zptr);
				}

				zend_post_incdec_property_zval(zptr, prop_info OPLINE_CC EXECUTE_DATA_CC);
			}
		} else {
			zend_post_incdec_overloaded_property(zobj, name, cache_slot OPLINE_CC EXECUTE_DATA_CC);
		}
		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(135, ZEND_POST_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_POST_INC_OBJ);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(38, ZEND_PRE_INC_STATIC_PROP, ANY, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zval *prop;
	zend_property_info *prop_info;

	SAVE_OPLINE();

	if (zend_fetch_static_property_address(&prop, &prop_info, opline->extended_value, BP_VAR_RW, 0 OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	zend_pre_incdec_property_zval(prop,
		ZEND_TYPE_IS_SET(prop_info->type) ? prop_info : NULL OPLINE_CC EXECUTE_DATA_CC);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(39, ZEND_PRE_DEC_STATIC_PROP, ANY, ANY, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_PRE_INC_STATIC_PROP);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(40, ZEND_POST_INC_STATIC_PROP, ANY, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zval *prop;
	zend_property_info *prop_info;

	SAVE_OPLINE();

	if (zend_fetch_static_property_address(&prop, &prop_info, opline->extended_value, BP_VAR_RW, 0 OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	zend_post_incdec_property_zval(prop,
		ZEND_TYPE_IS_SET(prop_info->type) ? prop_info : NULL OPLINE_CC EXECUTE_DATA_CC);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(41, ZEND_POST_DEC_STATIC_PROP, ANY, ANY, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HANDLER(ZEND_POST_INC_STATIC_PROP);
}

ZEND_VM_HELPER(zend_pre_inc_helper, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(var_ptr);
	}

	do {
		if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_REFERENCE)) {
			zend_reference *ref = Z_REF_P(var_ptr);
			var_ptr = Z_REFVAL_P(var_ptr);
			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_incdec_typed_ref(ref, NULL OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}
		increment_function(var_ptr);
	} while (0);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(34, ZEND_PRE_INC, VAR|CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_increment_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_pre_inc_helper);
}

ZEND_VM_HELPER(zend_pre_dec_helper, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(var_ptr);
	}

	do {
		if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_REFERENCE)) {
			zend_reference *ref = Z_REF_P(var_ptr);
			var_ptr = Z_REFVAL_P(var_ptr);

			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_incdec_typed_ref(ref, NULL OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}
		decrement_function(var_ptr);
	} while (0);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_decrement_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_pre_dec_helper);
}

ZEND_VM_HELPER(zend_post_inc_helper, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(var_ptr);
	}

	do {
		if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_REFERENCE)) {
			zend_reference *ref = Z_REF_P(var_ptr);
			var_ptr = Z_REFVAL_P(var_ptr);

			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_incdec_typed_ref(ref, EX_VAR(opline->result.var) OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);

		increment_function(var_ptr);
	} while (0);

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(36, ZEND_POST_INC, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_increment_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_post_inc_helper);
}

ZEND_VM_HELPER(zend_post_dec_helper, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(var_ptr);
	}

	do {
		if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_REFERENCE)) {
			zend_reference *ref = Z_REF_P(var_ptr);
			var_ptr = Z_REFVAL_P(var_ptr);

			if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
				zend_incdec_typed_ref(ref, EX_VAR(opline->result.var) OPLINE_CC EXECUTE_DATA_CC);
				break;
			}
		}
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);

		decrement_function(var_ptr);
	} while (0);

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(37, ZEND_POST_DEC, VAR|CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_decrement_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_post_dec_helper);
}

ZEND_VM_HANDLER(136, ZEND_ECHO, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *z;

	SAVE_OPLINE();
	z = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_P(z) == IS_STRING) {
		zend_string *str = Z_STR_P(z);

		if (ZSTR_LEN(str) != 0) {
			zend_write(ZSTR_VAL(str), ZSTR_LEN(str));
		}
	} else {
		zend_string *str = zval_get_string_func(z);

		if (ZSTR_LEN(str) != 0) {
			zend_write(ZSTR_VAL(str), ZSTR_LEN(str));
		} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(z) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		zend_string_release_ex(str, 0);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_fetch_var_address_helper, CONST|TMPVAR|CV, UNUSED, int type)
{
	USE_OPLINE
	zval *varname;
	zval *retval;
	zend_string *name, *tmp_name;
	HashTable *target_symbol_table;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
		tmp_name = NULL;
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		name = zval_try_get_tmp_string(varname, &tmp_name);
		if (UNEXPECTED(!name)) {
			if (!(opline->extended_value & ZEND_FETCH_GLOBAL_LOCK)) {
				FREE_OP1();
			}
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		}
	}

	target_symbol_table = zend_get_target_symbol_table(opline->extended_value EXECUTE_DATA_CC);
	retval = zend_hash_find_ex(target_symbol_table, name, OP1_TYPE == IS_CONST);
	if (retval == NULL) {
		if (UNEXPECTED(zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_THIS)))) {
ZEND_VM_C_LABEL(fetch_this):
			zend_fetch_this_var(type OPLINE_CC EXECUTE_DATA_CC);
			if (OP1_TYPE != IS_CONST) {
				zend_tmp_string_release(tmp_name);
			}
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
		if (type == BP_VAR_W) {
			retval = zend_hash_add_new(target_symbol_table, name, &EG(uninitialized_zval));
		} else if (type == BP_VAR_IS || type == BP_VAR_UNSET) {
			retval = &EG(uninitialized_zval);
		} else {
			if (OP1_TYPE == IS_CV) {
				/* Keep name alive in case an error handler tries to free it. */
				zend_string_addref(name);
			}
			zend_error_unchecked(E_WARNING, "Undefined %svariable $%S",
				(opline->extended_value & ZEND_FETCH_GLOBAL ? "global " : ""), name);
			if (type == BP_VAR_RW && !EG(exception)) {
				retval = zend_hash_update(target_symbol_table, name, &EG(uninitialized_zval));
			} else {
				retval = &EG(uninitialized_zval);
			}
			if (OP1_TYPE == IS_CV) {
				zend_string_release(name);
			}
		}
	/* GLOBAL or $$name variable may be an INDIRECT pointer to CV */
	} else if (Z_TYPE_P(retval) == IS_INDIRECT) {
		retval = Z_INDIRECT_P(retval);
		if (Z_TYPE_P(retval) == IS_UNDEF) {
			if (UNEXPECTED(zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_THIS)))) {
				ZEND_VM_C_GOTO(fetch_this);
			}
			if (type == BP_VAR_W) {
				ZVAL_NULL(retval);
			} else if (type == BP_VAR_IS || type == BP_VAR_UNSET) {
				retval = &EG(uninitialized_zval);
			} else {
				zend_error_unchecked(E_WARNING, "Undefined %svariable $%S",
					(opline->extended_value & ZEND_FETCH_GLOBAL ? "global " : ""), name);
				if (type == BP_VAR_RW && !EG(exception)) {
					ZVAL_NULL(retval);
				} else {
					retval = &EG(uninitialized_zval);
				}
			}
		}
	}

	if (!(opline->extended_value & ZEND_FETCH_GLOBAL_LOCK)) {
		FREE_OP1();
	}

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}

	ZEND_ASSERT(retval != NULL);
	if (type == BP_VAR_R || type == BP_VAR_IS) {
		ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
	} else {
		ZVAL_INDIRECT(EX_VAR(opline->result.var), retval);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(80, ZEND_FETCH_R, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(83, ZEND_FETCH_W, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(86, ZEND_FETCH_RW, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(92, ZEND_FETCH_FUNC_ARG, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	int fetch_type =
		(UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) ?
			BP_VAR_W : BP_VAR_R;
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, fetch_type);
}

ZEND_VM_HANDLER(95, ZEND_FETCH_UNSET, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(89, ZEND_FETCH_IS, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_IS);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HELPER(zend_fetch_static_prop_helper, ANY, ANY, int type)
{
	USE_OPLINE
	zval *prop;

	SAVE_OPLINE();

	if (UNEXPECTED(zend_fetch_static_property_address(&prop, NULL, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS, type, opline->extended_value OPLINE_CC EXECUTE_DATA_CC) != SUCCESS)) {
		ZEND_ASSERT(EG(exception) || (type == BP_VAR_IS));
		prop = &EG(uninitialized_zval);
	}

	if (type == BP_VAR_R || type == BP_VAR_IS) {
		ZVAL_COPY_DEREF(EX_VAR(opline->result.var), prop);
	} else {
		ZVAL_INDIRECT(EX_VAR(opline->result.var), prop);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(173, ZEND_FETCH_STATIC_PROP_R, ANY, CLASS_FETCH, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_R);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(174, ZEND_FETCH_STATIC_PROP_W, ANY, CLASS_FETCH, FETCH_REF|DIM_WRITE|CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_W);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(175, ZEND_FETCH_STATIC_PROP_RW, ANY, CLASS_FETCH, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_RW);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(177, ZEND_FETCH_STATIC_PROP_FUNC_ARG, ANY, CLASS_FETCH, FETCH_REF|CACHE_SLOT)
{
	int fetch_type =
		(UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) ?
			BP_VAR_W : BP_VAR_R;
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, fetch_type);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(178, ZEND_FETCH_STATIC_PROP_UNSET, ANY, CLASS_FETCH, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_UNSET);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(176, ZEND_FETCH_STATIC_PROP_IS, ANY, CLASS_FETCH, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_IS);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(81, ZEND_FETCH_DIM_R, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container, *dim, *value;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE != IS_CONST) {
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(fetch_dim_r_array):
			value = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, OP2_TYPE, BP_VAR_R EXECUTE_DATA_CC);
			ZVAL_COPY_DEREF(EX_VAR(opline->result.var), value);
		} else if (EXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(fetch_dim_r_array);
			} else {
				ZEND_VM_C_GOTO(fetch_dim_r_slow);
			}
		} else {
ZEND_VM_C_LABEL(fetch_dim_r_slow):
			if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			zend_fetch_dimension_address_read_R_slow(container, dim OPLINE_CC EXECUTE_DATA_CC);
		}
	} else {
		zend_fetch_dimension_address_read_R(container, dim, OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	}
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(84, ZEND_FETCH_DIM_W, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
	USE_OPLINE
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	zend_fetch_dimension_address_W(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
	USE_OPLINE
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	zend_fetch_dimension_address_RW(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(90, ZEND_FETCH_DIM_IS, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_IS);
	zend_fetch_dimension_address_read_IS(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_use_tmp_in_write_context_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot use temporary expression in write context");
	FREE_OP2();
	FREE_OP1();
	ZVAL_UNDEF(EX_VAR(opline->result.var));
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_use_undef_in_read_context_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot use [] for reading");
	FREE_OP2();
	FREE_OP1();
	ZVAL_UNDEF(EX_VAR(opline->result.var));
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
#if !ZEND_VM_SPEC
	USE_OPLINE
#endif

	if (UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) {
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_use_tmp_in_write_context_helper);
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_DIM_W);
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_use_undef_in_read_context_helper);
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_DIM_R);
	}
}

ZEND_VM_HANDLER(96, ZEND_FETCH_DIM_UNSET, VAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);
	zend_fetch_dimension_address_UNSET(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_OBJ_HANDLER(82, ZEND_FETCH_OBJ_R, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *container;
	void **cache_slot = NULL;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		do {
			if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
					break;
				}
			}
			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
			}
			zend_wrong_property_read(container, GET_OP2_ZVAL_PTR(BP_VAR_R));
			ZVAL_NULL(EX_VAR(opline->result.var));
			ZEND_VM_C_GOTO(fetch_obj_r_finish);
		} while (0);
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zend_string *name, *tmp_name;
		zval *retval;

		if (OP2_TYPE == IS_CONST) {
			cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_REF /* FUNC_ARG fetch may contain it */);

			if (EXPECTED(zobj->ce == CACHED_PTR_EX(cache_slot))) {
				uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);

				if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
					retval = OBJ_PROP(zobj, prop_offset);
					if (EXPECTED(Z_TYPE_INFO_P(retval) != IS_UNDEF)) {
						if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
							ZEND_VM_C_GOTO(fetch_obj_r_copy);
						} else {
ZEND_VM_C_LABEL(fetch_obj_r_fast_copy):
							ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
							ZEND_VM_NEXT_OPCODE();
						}
					}
				} else if (EXPECTED(zobj->properties != NULL)) {
					name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
					if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
						uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

						if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
							Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

							if (EXPECTED(p->key == name) ||
							    (EXPECTED(p->h == ZSTR_H(name)) &&
							     EXPECTED(p->key != NULL) &&
							     EXPECTED(zend_string_equal_content(p->key, name)))) {
								retval = &p->val;
								if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
									ZEND_VM_C_GOTO(fetch_obj_r_copy);
								} else {
									ZEND_VM_C_GOTO(fetch_obj_r_fast_copy);
								}
							}
						}
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
					}
					retval = zend_hash_find_known_hash(zobj->properties, name);
					if (EXPECTED(retval)) {
						uintptr_t idx = (char*)retval - (char*)zobj->properties->arData;
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
						if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
							ZEND_VM_C_GOTO(fetch_obj_r_copy);
						} else {
							ZEND_VM_C_GOTO(fetch_obj_r_fast_copy);
						}
					}
				}
			}
			name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
		} else {
			name = zval_try_get_tmp_string(GET_OP2_ZVAL_PTR(BP_VAR_R), &tmp_name);
			if (UNEXPECTED(!name)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				break;
			}
		}

#if ZEND_DEBUG
		/* For non-standard object handlers, verify a declared property type in debug builds.
		 * Fetch prop_info before calling read_property(), as it may deallocate the object. */
		zend_property_info *prop_info = NULL;
		if (zobj->handlers->read_property != zend_std_read_property) {
			prop_info = zend_get_property_info(zobj->ce, name, /* silent */ true);
		}
#endif
		retval = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, EX_VAR(opline->result.var));
#if ZEND_DEBUG
		if (!EG(exception) && prop_info && prop_info != ZEND_WRONG_PROPERTY_INFO
				&& ZEND_TYPE_IS_SET(prop_info->type)) {
			ZVAL_OPT_DEREF(retval);
			zend_verify_property_type(prop_info, retval, /* strict */ true);
		}
#endif

		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}

		if (retval != EX_VAR(opline->result.var)) {
ZEND_VM_C_LABEL(fetch_obj_r_copy):
			ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
		} else if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_unwrap_reference(retval);
		}
	} while (0);

ZEND_VM_C_LABEL(fetch_obj_r_finish):
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, FETCH_REF|DIM_WRITE|CACHE_SLOT)
{
	USE_OPLINE
	zval *property, *container, *result;

	SAVE_OPLINE();

	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(
		result, container, OP1_TYPE, property, OP2_TYPE,
		((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) : NULL),
		BP_VAR_W, opline->extended_value OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *property, *container, *result;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(result, container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), BP_VAR_RW, 0 OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(91, ZEND_FETCH_OBJ_IS, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *container;
	void **cache_slot = NULL;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		do {
			if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
					break;
				}
			}
			if (OP2_TYPE == IS_CV && Z_TYPE_P(EX_VAR(opline->op2.var)) == IS_UNDEF) {
				ZVAL_UNDEFINED_OP2();
			}
			ZVAL_NULL(EX_VAR(opline->result.var));
			ZEND_VM_C_GOTO(fetch_obj_is_finish);
		} while (0);
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zend_string *name, *tmp_name;
		zval *retval;

		if (OP2_TYPE == IS_CONST) {
			cache_slot = CACHE_ADDR(opline->extended_value);

			if (EXPECTED(zobj->ce == CACHED_PTR_EX(cache_slot))) {
				uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);

				if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
					retval = OBJ_PROP(zobj, prop_offset);
					if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
						if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
							ZEND_VM_C_GOTO(fetch_obj_is_copy);
						} else {
ZEND_VM_C_LABEL(fetch_obj_is_fast_copy):
							ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
							ZEND_VM_NEXT_OPCODE();
						}
					}
				} else if (EXPECTED(zobj->properties != NULL)) {
					name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
					if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
						uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

						if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
							Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

							if (EXPECTED(p->key == name) ||
							    (EXPECTED(p->h == ZSTR_H(name)) &&
							     EXPECTED(p->key != NULL) &&
							     EXPECTED(zend_string_equal_content(p->key, name)))) {
								retval = &p->val;
								if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
									ZEND_VM_C_GOTO(fetch_obj_is_copy);
								} else {
									ZEND_VM_C_GOTO(fetch_obj_is_fast_copy);
								}
							}
						}
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
					}
					retval = zend_hash_find_known_hash(zobj->properties, name);
					if (EXPECTED(retval)) {
						uintptr_t idx = (char*)retval - (char*)zobj->properties->arData;
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
						if (!ZEND_VM_SPEC || (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) != 0) {
							ZEND_VM_C_GOTO(fetch_obj_is_copy);
						} else {
							ZEND_VM_C_GOTO(fetch_obj_is_fast_copy);
						}
					}
				}
			}
			name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
		} else {
			name = zval_try_get_tmp_string(GET_OP2_ZVAL_PTR(BP_VAR_R), &tmp_name);
			if (UNEXPECTED(!name)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				break;
			}
		}

		retval = zobj->handlers->read_property(zobj, name, BP_VAR_IS, cache_slot, EX_VAR(opline->result.var));

		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}

		if (retval != EX_VAR(opline->result.var)) {
ZEND_VM_C_LABEL(fetch_obj_is_copy):
			ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
		} else if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_unwrap_reference(retval);
		}
	} while (0);

ZEND_VM_C_LABEL(fetch_obj_is_finish):
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, CONST|TMP|VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, FETCH_REF|CACHE_SLOT)
{
#if !ZEND_VM_SPEC
	USE_OPLINE
#endif

	if (UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) {
		/* Behave like FETCH_OBJ_W */
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_use_tmp_in_write_context_helper);
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_OBJ_W);
	} else {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_OBJ_R);
	}
}

ZEND_VM_HANDLER(97, ZEND_FETCH_OBJ_UNSET, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *container, *property, *result;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(result, container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), BP_VAR_UNSET, 0 OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(opline->op1.var);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_LIST_R, CONST|TMPVARCV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	zend_fetch_dimension_address_LIST_r(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(155, ZEND_FETCH_LIST_W, VAR, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container, *dim;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_VAR
		&& Z_TYPE_P(EX_VAR(opline->op1.var)) != IS_INDIRECT
		&& UNEXPECTED(!Z_ISREF_P(container))
	) {
		zend_error(E_NOTICE, "Attempting to set reference to non referenceable value");
		zend_fetch_dimension_address_LIST_r(container, dim, OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	} else {
		zend_fetch_dimension_address_W(container, dim, OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	}

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(24, ZEND_ASSIGN_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zval *object, *value, tmp;
	zend_object *zobj;
	zend_string *name, *tmp_name;
	zend_refcounted *garbage = NULL;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		if (Z_ISREF_P(object) && Z_TYPE_P(Z_REFVAL_P(object)) == IS_OBJECT) {
			object = Z_REFVAL_P(object);
			ZEND_VM_C_GOTO(assign_object);
		}
		zend_throw_non_object_error(object, GET_OP2_ZVAL_PTR(BP_VAR_R) OPLINE_CC EXECUTE_DATA_CC);
		value = &EG(uninitialized_zval);
		ZEND_VM_C_GOTO(free_and_exit_assign_obj);
	}

ZEND_VM_C_LABEL(assign_object):
	zobj = Z_OBJ_P(object);
	if (OP2_TYPE == IS_CONST) {
		if (EXPECTED(zobj->ce == CACHED_PTR(opline->extended_value))) {
			void **cache_slot = CACHE_ADDR(opline->extended_value);
			uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);
			zval *property_val;

			if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
				property_val = OBJ_PROP(zobj, prop_offset);
				if (Z_TYPE_P(property_val) != IS_UNDEF) {
					zend_property_info *prop_info = (zend_property_info*) CACHED_PTR_EX(cache_slot + 2);

					if (UNEXPECTED(prop_info != NULL)) {
						value = zend_assign_to_typed_prop(prop_info, property_val, value, &garbage EXECUTE_DATA_CC);
						ZEND_VM_C_GOTO(free_and_exit_assign_obj);
					} else {
ZEND_VM_C_LABEL(fast_assign_obj):
						value = zend_assign_to_variable_ex(property_val, value, OP_DATA_TYPE, EX_USES_STRICT_TYPES(), &garbage);
						if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
							ZVAL_COPY(EX_VAR(opline->result.var), value);
						}
						ZEND_VM_C_GOTO(exit_assign_obj);
					}
				}
			} else {
				name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
				if (EXPECTED(zobj->properties != NULL)) {
					if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
						if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
							GC_DELREF(zobj->properties);
						}
						zobj->properties = zend_array_dup(zobj->properties);
					}
					property_val = zend_hash_find_known_hash(zobj->properties, name);
					if (property_val) {
						ZEND_VM_C_GOTO(fast_assign_obj);
					}
				}

				if (!zobj->ce->__set && (zobj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
					if (EXPECTED(zobj->properties == NULL)) {
						rebuild_object_properties(zobj);
					}
					if (OP_DATA_TYPE == IS_CONST) {
						if (UNEXPECTED(Z_OPT_REFCOUNTED_P(value))) {
							Z_ADDREF_P(value);
						}
					} else if (OP_DATA_TYPE != IS_TMP_VAR) {
						if (Z_ISREF_P(value)) {
							if (OP_DATA_TYPE == IS_VAR) {
								zend_reference *ref = Z_REF_P(value);
								if (GC_DELREF(ref) == 0) {
									ZVAL_COPY_VALUE(&tmp, Z_REFVAL_P(value));
									efree_size(ref, sizeof(zend_reference));
									value = &tmp;
								} else {
									value = Z_REFVAL_P(value);
									Z_TRY_ADDREF_P(value);
								}
							} else {
								value = Z_REFVAL_P(value);
								Z_TRY_ADDREF_P(value);
							}
						} else if (OP_DATA_TYPE == IS_CV) {
							Z_TRY_ADDREF_P(value);
						}
						}
					zend_hash_add_new(zobj->properties, name, value);
					if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
						ZVAL_COPY(EX_VAR(opline->result.var), value);
					}
					ZEND_VM_C_GOTO(exit_assign_obj);
				}
			}
		}
		name = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
	} else {
		name = zval_try_get_tmp_string(GET_OP2_ZVAL_PTR(BP_VAR_R), &tmp_name);
		if (UNEXPECTED(!name)) {
			FREE_OP_DATA();
			UNDEF_RESULT();
			ZEND_VM_C_GOTO(exit_assign_obj);
		}
	}

	if (OP_DATA_TYPE == IS_CV || OP_DATA_TYPE == IS_VAR) {
		ZVAL_DEREF(value);
	}

	value = zobj->handlers->write_property(zobj, name, value, (OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL);

	if (OP2_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}

ZEND_VM_C_LABEL(free_and_exit_assign_obj):
	if (UNEXPECTED(RETURN_VALUE_USED(opline)) && value) {
		ZVAL_COPY_DEREF(EX_VAR(opline->result.var), value);
	}
	FREE_OP_DATA();
ZEND_VM_C_LABEL(exit_assign_obj):
	if (garbage) {
		GC_DTOR_NO_REF(garbage);
	}
	FREE_OP2();
	FREE_OP1();
	/* assign_obj has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(25, ZEND_ASSIGN_STATIC_PROP, ANY, ANY, CACHE_SLOT, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zval *prop, *value;
	zend_property_info *prop_info;
	zend_refcounted *garbage = NULL;

	SAVE_OPLINE();

	if (zend_fetch_static_property_address(&prop, &prop_info, opline->extended_value, BP_VAR_W, 0 OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
		FREE_OP_DATA();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(ZEND_TYPE_IS_SET(prop_info->type))) {
		value = zend_assign_to_typed_prop(prop_info, prop, value, &garbage EXECUTE_DATA_CC);
		FREE_OP_DATA();
	} else {
		value = zend_assign_to_variable_ex(prop, value, OP_DATA_TYPE, EX_USES_STRICT_TYPES(), &garbage);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	}

	if (garbage) {
		GC_DTOR_NO_REF(garbage);
	}

	/* assign_static_prop has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(23, ZEND_ASSIGN_DIM, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zval *object_ptr, *orig_object_ptr;
	zval *value;
	zval *variable_ptr;
	zval *dim;
	zend_refcounted *garbage = NULL;

	SAVE_OPLINE();
	orig_object_ptr = object_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
ZEND_VM_C_LABEL(try_assign_dim_array):
		SEPARATE_ARRAY(object_ptr);
		if (OP2_TYPE == IS_UNUSED) {
			value = GET_OP_DATA_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP_DATA_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
				HashTable *ht = Z_ARRVAL_P(object_ptr);
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				value = zval_undefined_cv((opline+1)->op1.var EXECUTE_DATA_CC);
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
					zend_array_destroy(ht);
					ZEND_VM_C_GOTO(assign_dim_error);
				}
			}
			if (OP_DATA_TYPE == IS_CV || OP_DATA_TYPE == IS_VAR) {
				ZVAL_DEREF(value);
			}
			value = zend_hash_next_index_insert(Z_ARRVAL_P(object_ptr), value);
			if (UNEXPECTED(value == NULL)) {
				zend_cannot_add_element();
				ZEND_VM_C_GOTO(assign_dim_error);
			} else if (OP_DATA_TYPE == IS_CV) {
				if (Z_REFCOUNTED_P(value)) {
					Z_ADDREF_P(value);
				}
			} else if (OP_DATA_TYPE == IS_VAR) {
				zval *free_op_data = EX_VAR((opline+1)->op1.var);
				if (Z_ISREF_P(free_op_data)) {
					if (Z_REFCOUNTED_P(value)) {
						Z_ADDREF_P(value);
					}
					zval_ptr_dtor_nogc(free_op_data);
				}
			} else if (OP_DATA_TYPE == IS_CONST) {
				if (UNEXPECTED(Z_REFCOUNTED_P(value))) {
					Z_ADDREF_P(value);
				}
			}
		} else {
			dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP2_TYPE == IS_CONST) {
				variable_ptr = zend_fetch_dimension_address_inner_W_CONST(Z_ARRVAL_P(object_ptr), dim EXECUTE_DATA_CC);
			} else {
				variable_ptr = zend_fetch_dimension_address_inner_W(Z_ARRVAL_P(object_ptr), dim EXECUTE_DATA_CC);
			}
			if (UNEXPECTED(variable_ptr == NULL)) {
				ZEND_VM_C_GOTO(assign_dim_error);
			}
			value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);
			value = zend_assign_to_variable_ex(variable_ptr, value, OP_DATA_TYPE, EX_USES_STRICT_TYPES(), &garbage);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
		if (garbage) {
			GC_DTOR_NO_REF(garbage);
		}
	} else {
		if (EXPECTED(Z_ISREF_P(object_ptr))) {
			object_ptr = Z_REFVAL_P(object_ptr);
			if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(try_assign_dim_array);
			}
		}
		if (EXPECTED(Z_TYPE_P(object_ptr) == IS_OBJECT)) {
			zend_object *obj = Z_OBJ_P(object_ptr);

			GC_ADDREF(obj);
			dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_ISUNDEF_P(dim))) {
				dim = ZVAL_UNDEFINED_OP2();
			} else if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}

			value = GET_OP_DATA_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP_DATA_TYPE == IS_CV && UNEXPECTED(Z_ISUNDEF_P(value))) {
				value = zval_undefined_cv((opline+1)->op1.var EXECUTE_DATA_CC);
			} else if (OP_DATA_TYPE & (IS_CV|IS_VAR)) {
				ZVAL_DEREF(value);
			}

			zend_assign_to_object_dim(obj, dim, value OPLINE_CC EXECUTE_DATA_CC);

			FREE_OP_DATA();
			if (UNEXPECTED(GC_DELREF(obj) == 0)) {
				zend_objects_store_del(obj);
			}
		} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
			if (OP2_TYPE == IS_UNUSED) {
				zend_use_new_element_for_string();
				FREE_OP_DATA();
				UNDEF_RESULT();
			} else {
				dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
				value = GET_OP_DATA_ZVAL_PTR_UNDEF(BP_VAR_R);
				zend_assign_to_string_offset(object_ptr, dim, value OPLINE_CC EXECUTE_DATA_CC);
				FREE_OP_DATA();
			}
		} else if (EXPECTED(Z_TYPE_P(object_ptr) <= IS_FALSE)) {
			if (Z_ISREF_P(orig_object_ptr)
			 && ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(orig_object_ptr))
			 && !zend_verify_ref_array_assignable(Z_REF_P(orig_object_ptr))) {
				dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
				FREE_OP_DATA();
				UNDEF_RESULT();
			} else {
				HashTable *ht = zend_new_array(8);
				uint8_t old_type = Z_TYPE_P(object_ptr);

				ZVAL_ARR(object_ptr, ht);
				if (UNEXPECTED(old_type == IS_FALSE)) {
					GC_ADDREF(ht);
					zend_false_to_array_deprecated();
					if (UNEXPECTED(GC_DELREF(ht) == 0)) {
						zend_array_destroy(ht);
						ZEND_VM_C_GOTO(assign_dim_error);
					}
				}
				ZEND_VM_C_GOTO(try_assign_dim_array);
			}
		} else {
			zend_use_scalar_as_array();
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
ZEND_VM_C_LABEL(assign_dim_error):
			FREE_OP_DATA();
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		}
	}
	if (OP2_TYPE != IS_UNUSED) {
		FREE_OP2();
	}
	FREE_OP1();
	/* assign_dim has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(22, ZEND_ASSIGN, VAR|CV, CONST|TMP|VAR|CV, SPEC(RETVAL))
{
	USE_OPLINE
	zval *value;
	zval *variable_ptr;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (!ZEND_VM_SPEC || UNEXPECTED(RETURN_VALUE_USED(opline))) {
		zend_refcounted *garbage = NULL;

		value = zend_assign_to_variable_ex(variable_ptr, value, OP2_TYPE, EX_USES_STRICT_TYPES(), &garbage);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
		if (garbage) {
			GC_DTOR_NO_REF(garbage);
		}
	} else {
		value = zend_assign_to_variable(variable_ptr, value, OP2_TYPE, EX_USES_STRICT_TYPES());
	}
	FREE_OP1();
	/* zend_assign_to_variable() always takes care of op2, never free it! */

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(30, ZEND_ASSIGN_REF, VAR|CV, VAR|CV, SRC)
{
	USE_OPLINE
	zval *variable_ptr;
	zval *value_ptr;
	zend_refcounted *garbage = NULL;

	SAVE_OPLINE();
	value_ptr = GET_OP2_ZVAL_PTR_PTR(BP_VAR_W);
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR &&
	           UNEXPECTED(Z_TYPE_P(EX_VAR(opline->op1.var)) != IS_INDIRECT)) {

		zend_throw_error(NULL, "Cannot assign by reference to an array dimension of an object");
		variable_ptr = &EG(uninitialized_zval);
	} else if (OP2_TYPE == IS_VAR &&
	           opline->extended_value == ZEND_RETURNS_FUNCTION &&
			   UNEXPECTED(!Z_ISREF_P(value_ptr))) {

		variable_ptr = zend_wrong_assign_to_variable_reference(
			variable_ptr, value_ptr, &garbage OPLINE_CC EXECUTE_DATA_CC);
	} else {
		zend_assign_to_variable_reference(variable_ptr, value_ptr, &garbage);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}

	if (garbage) {
		GC_DTOR(garbage);
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(32, ZEND_ASSIGN_OBJ_REF, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT|SRC, SPEC(OP_DATA=VAR|CV))
{
	USE_OPLINE
	zval *property, *container, *value_ptr;

	SAVE_OPLINE();

	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	value_ptr = GET_OP_DATA_ZVAL_PTR_PTR(BP_VAR_W);

	if (ZEND_VM_SPEC) {
		if (OP1_TYPE == IS_UNUSED) {
			if (OP2_TYPE == IS_CONST) {
				zend_assign_to_property_reference_this_const(container, property, value_ptr OPLINE_CC EXECUTE_DATA_CC);
			} else {
				zend_assign_to_property_reference_this_var(container, property, value_ptr OPLINE_CC EXECUTE_DATA_CC);
			}
		} else {
			if (OP2_TYPE == IS_CONST) {
				zend_assign_to_property_reference_var_const(container, property, value_ptr OPLINE_CC EXECUTE_DATA_CC);
			} else {
				zend_assign_to_property_reference_var_var(container, property, value_ptr OPLINE_CC EXECUTE_DATA_CC);
			}
		}
	} else {
		zend_assign_to_property_reference(container, OP1_TYPE, property, OP2_TYPE, value_ptr OPLINE_CC EXECUTE_DATA_CC);
	}

	FREE_OP1();
	FREE_OP2();
	FREE_OP_DATA();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CONST|VAR) */
ZEND_VM_HANDLER(33, ZEND_ASSIGN_STATIC_PROP_REF, ANY, ANY, CACHE_SLOT|SRC)
{
	USE_OPLINE
	zval *prop, *value_ptr;
	zend_property_info *prop_info;
	zend_refcounted *garbage = NULL;

	SAVE_OPLINE();

	if (zend_fetch_static_property_address(&prop, &prop_info, opline->extended_value & ~ZEND_RETURNS_FUNCTION, BP_VAR_W, 0 OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
		FREE_OP_DATA();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	value_ptr = GET_OP_DATA_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP_DATA_TYPE == IS_VAR && (opline->extended_value & ZEND_RETURNS_FUNCTION) && UNEXPECTED(!Z_ISREF_P(value_ptr))) {
		if (UNEXPECTED(!zend_wrong_assign_to_variable_reference(prop, value_ptr, &garbage OPLINE_CC EXECUTE_DATA_CC))) {
			prop = &EG(uninitialized_zval);
		}
	} else if (UNEXPECTED(ZEND_TYPE_IS_SET(prop_info->type))) {
		prop = zend_assign_to_typed_property_reference(prop_info, prop, value_ptr, &garbage EXECUTE_DATA_CC);
	} else {
		zend_assign_to_variable_reference(prop, value_ptr, &garbage);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), prop);
	}

	if (garbage) {
		GC_DTOR(garbage);
	}

	FREE_OP_DATA();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HOT_HELPER(zend_leave_helper, ANY, ANY)
{
	zend_execute_data *old_execute_data;
	uint32_t call_info = EX_CALL_INFO();
	SAVE_OPLINE();

	if (EXPECTED((call_info & (ZEND_CALL_CODE|ZEND_CALL_TOP|ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_ALLOCATED|ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) == 0)) {
		EG(current_execute_data) = EX(prev_execute_data);
		i_free_compiled_variables(execute_data);

#ifdef ZEND_PREFER_RELOAD
		call_info = EX_CALL_INFO();
#endif
		if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
			OBJ_RELEASE(Z_OBJ(execute_data->This));
		} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
		}
		EG(vm_stack_top) = (zval*)execute_data;
		execute_data = EX(prev_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			HANDLE_EXCEPTION_LEAVE();
		}

		LOAD_NEXT_OPLINE();
		ZEND_VM_LEAVE();
	} else if (EXPECTED((call_info & (ZEND_CALL_CODE|ZEND_CALL_TOP)) == 0)) {
		EG(current_execute_data) = EX(prev_execute_data);
		i_free_compiled_variables(execute_data);

#ifdef ZEND_PREFER_RELOAD
		call_info = EX_CALL_INFO();
#endif
		if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_clean_and_cache_symbol_table(EX(symbol_table));
		}

		if (UNEXPECTED(call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			zend_free_extra_named_params(EX(extra_named_params));
		}

		/* Free extra args before releasing the closure,
		 * as that may free the op_array. */
		zend_vm_stack_free_extra_args_ex(call_info, execute_data);

		if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
			OBJ_RELEASE(Z_OBJ(execute_data->This));
		} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
		}

		old_execute_data = execute_data;
		execute_data = EX(prev_execute_data);
		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			HANDLE_EXCEPTION_LEAVE();
		}

		LOAD_NEXT_OPLINE();
		ZEND_VM_LEAVE();
	} else if (EXPECTED((call_info & ZEND_CALL_TOP) == 0)) {
		if (EX(func)->op_array.last_var > 0) {
			zend_detach_symbol_table(execute_data);
			call_info |= ZEND_CALL_NEEDS_REATTACH;
		}
		zend_destroy_static_vars(&EX(func)->op_array);
		destroy_op_array(&EX(func)->op_array);
		efree_size(EX(func), sizeof(zend_op_array));
		old_execute_data = execute_data;
		execute_data = EG(current_execute_data) = EX(prev_execute_data);
		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		if (call_info & ZEND_CALL_NEEDS_REATTACH) {
			if (EX(func)->op_array.last_var > 0) {
				zend_attach_symbol_table(execute_data);
			} else {
				ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_NEEDS_REATTACH);
			}
		}
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			HANDLE_EXCEPTION_LEAVE();
		}

		LOAD_NEXT_OPLINE();
		ZEND_VM_LEAVE();
	} else {
		if (EXPECTED((call_info & ZEND_CALL_CODE) == 0)) {
			EG(current_execute_data) = EX(prev_execute_data);
			i_free_compiled_variables(execute_data);
#ifdef ZEND_PREFER_RELOAD
			call_info = EX_CALL_INFO();
#endif
			if (UNEXPECTED(call_info & (ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_HAS_EXTRA_NAMED_PARAMS))) {
				if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					zend_clean_and_cache_symbol_table(EX(symbol_table));
				}
				zend_vm_stack_free_extra_args_ex(call_info, execute_data);
				if (UNEXPECTED(call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
					zend_free_extra_named_params(EX(extra_named_params));
				}
			}
			if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
				OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
			}
			ZEND_VM_RETURN();
		} else /* if (call_kind == ZEND_CALL_TOP_CODE) */ {
			zend_array *symbol_table = EX(symbol_table);

			if (EX(func)->op_array.last_var > 0) {
				zend_detach_symbol_table(execute_data);
				call_info |= ZEND_CALL_NEEDS_REATTACH;
			}
			if (call_info & ZEND_CALL_NEEDS_REATTACH) {
				old_execute_data = EX(prev_execute_data);
				while (old_execute_data) {
					if (old_execute_data->func && (ZEND_CALL_INFO(old_execute_data) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
						if (old_execute_data->symbol_table == symbol_table) {
							if (old_execute_data->func->op_array.last_var > 0) {
								zend_attach_symbol_table(old_execute_data);
							} else {
								ZEND_ADD_CALL_FLAG(old_execute_data, ZEND_CALL_NEEDS_REATTACH);
							}
						}
						break;
					}
					old_execute_data = old_execute_data->prev_execute_data;
				}
			}
			EG(current_execute_data) = EX(prev_execute_data);
			ZEND_VM_RETURN();
		}
	}
}

ZEND_VM_HOT_HANDLER(42, ZEND_JMP, JMP_ADDR, ANY)
{
	USE_OPLINE

	ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op1), 0);
}

ZEND_VM_HOT_NOCONST_HANDLER(43, ZEND_JMPZ, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *val;
	uint8_t op1_type;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_NEXT_OPCODE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	SAVE_OPLINE();
	op1_type = OP1_TYPE;
	if (i_zend_is_true(val)) {
		opline++;
	} else {
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	if (op1_type & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(val);
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HOT_NOCONST_HANDLER(44, ZEND_JMPNZ, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *val;
	uint8_t op1_type;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	op1_type = OP1_TYPE;
	if (i_zend_is_true(val)) {
		opline = OP_JMP_ADDR(opline, opline->op2);
	} else {
		opline++;
	}
	if (op1_type & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(val);
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_COLD_CONST_HANDLER(46, ZEND_JMPZ_EX, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *val;
	bool ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_NEXT_OPCODE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	SAVE_OPLINE();
	ret = i_zend_is_true(val);
	FREE_OP1();
	if (ret) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		opline++;
	} else {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_COLD_CONST_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *val;
	bool ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	ret = i_zend_is_true(val);
	FREE_OP1();
	if (ret) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		opline = OP_JMP_ADDR(opline, opline->op2);
	} else {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		opline++;
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(70, ZEND_FREE, TMPVAR, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(127, ZEND_FE_FREE, TMPVAR, ANY)
{
	zval *var;
	USE_OPLINE

	var = EX_VAR(opline->op1.var);
	if (Z_TYPE_P(var) != IS_ARRAY) {
		SAVE_OPLINE();
		if (Z_FE_ITER_P(var) != (uint32_t)-1) {
			zend_hash_iterator_del(Z_FE_ITER_P(var));
		}
		zval_ptr_dtor_nogc(var);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	/* This is freeing an array. Use an inlined version of zval_ptr_dtor_nogc. */
	/* PHP only needs to save the opline and check for an exception if the last reference to the array was garbage collected (destructors of elements in the array could throw an exception) */
	if (Z_REFCOUNTED_P(var) && !Z_DELREF_P(var)) {
		SAVE_OPLINE();
		rc_dtor_func(Z_COUNTED_P(var));
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(53, ZEND_FAST_CONCAT, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;
	zend_string *op1_str, *op2_str, *str;


	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if ((OP1_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op1) == IS_STRING)) &&
	    (OP2_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op2) == IS_STRING))) {
		zend_string *op1_str = Z_STR_P(op1);
		zend_string *op2_str = Z_STR_P(op2);
		zend_string *str;
		uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(op1_str, op2_str);

		if (OP1_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
			if (OP2_TYPE == IS_CONST || OP2_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op2_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
			}
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op1_str, 0);
			}
		} else if (OP2_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op1_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		} else if (OP1_TYPE != IS_CONST && OP1_TYPE != IS_CV &&
		    !ZSTR_IS_INTERNED(op1_str) && GC_REFCOUNT(op1_str) == 1) {
			size_t len = ZSTR_LEN(op1_str);

			str = zend_string_extend(op1_str, len + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str) + len, ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			GC_ADD_FLAGS(str, flags);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		} else {
			str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
			memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			GC_ADD_FLAGS(str, flags);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op1_str, 0);
			}
			if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
				zend_string_release_ex(op2_str, 0);
			}
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CONST) {
		op1_str = Z_STR_P(op1);
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		op1_str = zend_string_copy(Z_STR_P(op1));
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		op1_str = zval_get_string_func(op1);
	}
	if (OP2_TYPE == IS_CONST) {
		op2_str = Z_STR_P(op2);
	} else if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		op2_str = zend_string_copy(Z_STR_P(op2));
	} else {
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP2();
		}
		op2_str = zval_get_string_func(op2);
	}
	do {
		if (OP1_TYPE != IS_CONST) {
			if (UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
				if (OP2_TYPE == IS_CONST) {
					if (UNEXPECTED(Z_REFCOUNTED_P(op2))) {
						GC_ADDREF(op2_str);
					}
				}
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
				zend_string_release_ex(op1_str, 0);
				break;
			}
		}
		if (OP2_TYPE != IS_CONST) {
			if (UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
				if (OP1_TYPE == IS_CONST) {
					if (UNEXPECTED(Z_REFCOUNTED_P(op1))) {
						GC_ADDREF(op1_str);
					}
				}
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
				zend_string_release_ex(op2_str, 0);
				break;
			}
		}
		str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
		memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
		memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);

		ZSTR_COPY_CONCAT_PROPERTIES_BOTH(str, op1_str, op2_str);
		ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
		if (OP1_TYPE != IS_CONST) {
			zend_string_release_ex(op1_str, 0);
		}
		if (OP2_TYPE != IS_CONST) {
			zend_string_release_ex(op2_str, 0);
		}
	} while (0);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(54, ZEND_ROPE_INIT, UNUSED, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zend_string **rope;
	zval *var;

	/* Compiler allocates the necessary number of zval slots to keep the rope */
	rope = (zend_string**)EX_VAR(opline->result.var);
	if (OP2_TYPE == IS_CONST) {
		var = GET_OP2_ZVAL_PTR(BP_VAR_R);
		rope[0] = Z_STR_P(var);
		if (UNEXPECTED(Z_REFCOUNTED_P(var))) {
			Z_ADDREF_P(var);
		}
	} else {
		var = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (EXPECTED(Z_TYPE_P(var) == IS_STRING)) {
			if (OP2_TYPE == IS_CV) {
				rope[0] = zend_string_copy(Z_STR_P(var));
			} else {
				rope[0] = Z_STR_P(var);
			}
		} else {
			SAVE_OPLINE();
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP2();
			}
			rope[0] = zval_get_string_func(var);
			FREE_OP2();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(55, ZEND_ROPE_ADD, TMP, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zend_string **rope;
	zval *var;

	/* op1 and result are the same */
	rope = (zend_string**)EX_VAR(opline->op1.var);
	if (OP2_TYPE == IS_CONST) {
		var = GET_OP2_ZVAL_PTR(BP_VAR_R);
		rope[opline->extended_value] = Z_STR_P(var);
		if (UNEXPECTED(Z_REFCOUNTED_P(var))) {
			Z_ADDREF_P(var);
		}
	} else {
		var = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (EXPECTED(Z_TYPE_P(var) == IS_STRING)) {
			if (OP2_TYPE == IS_CV) {
				rope[opline->extended_value] = zend_string_copy(Z_STR_P(var));
			} else {
				rope[opline->extended_value] = Z_STR_P(var);
			}
		} else {
			SAVE_OPLINE();
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP2();
			}
			rope[opline->extended_value] = zval_get_string_func(var);
			FREE_OP2();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(56, ZEND_ROPE_END, TMP, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zend_string **rope;
	zval *var, *ret;
	uint32_t i;

	rope = (zend_string**)EX_VAR(opline->op1.var);
	if (OP2_TYPE == IS_CONST) {
		var = GET_OP2_ZVAL_PTR(BP_VAR_R);
		rope[opline->extended_value] = Z_STR_P(var);
		if (UNEXPECTED(Z_REFCOUNTED_P(var))) {
			Z_ADDREF_P(var);
		}
	} else {
		var = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (EXPECTED(Z_TYPE_P(var) == IS_STRING)) {
			if (OP2_TYPE == IS_CV) {
				rope[opline->extended_value] = zend_string_copy(Z_STR_P(var));
			} else {
				rope[opline->extended_value] = Z_STR_P(var);
			}
		} else {
			SAVE_OPLINE();
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP2();
			}
			rope[opline->extended_value] = zval_get_string_func(var);
			FREE_OP2();
			if (UNEXPECTED(EG(exception))) {
				for (i = 0; i <= opline->extended_value; i++) {
					zend_string_release_ex(rope[i], 0);
				}
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		}
	}

	size_t len = 0;
	uint32_t flags = ZSTR_COPYABLE_CONCAT_PROPERTIES;
	for (i = 0; i <= opline->extended_value; i++) {
		flags &= ZSTR_GET_COPYABLE_CONCAT_PROPERTIES(rope[i]);
		len += ZSTR_LEN(rope[i]);
	}
	ret = EX_VAR(opline->result.var);
	ZVAL_STR(ret, zend_string_alloc(len, 0));
	GC_ADD_FLAGS(Z_STR_P(ret), flags);

	char *target = Z_STRVAL_P(ret);
	for (i = 0; i <= opline->extended_value; i++) {
		memcpy(target, ZSTR_VAL(rope[i]), ZSTR_LEN(rope[i]));
		target += ZSTR_LEN(rope[i]);
		zend_string_release_ex(rope[i], 0);
	}
	*target = '\0';

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(109, ZEND_FETCH_CLASS, UNUSED|CLASS_FETCH, CONST|TMPVAR|UNUSED|CV, CACHE_SLOT)
{
	zval *class_name;
	USE_OPLINE

	SAVE_OPLINE();
	if (OP2_TYPE == IS_UNUSED) {
		Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(NULL, opline->op1.num);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else if (OP2_TYPE == IS_CONST) {
		zend_class_entry *ce = CACHED_PTR(opline->extended_value);

		if (UNEXPECTED(ce == NULL)) {
			class_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
			ce = zend_fetch_class_by_name(Z_STR_P(class_name), Z_STR_P(class_name + 1), opline->op1.num);
			CACHE_PTR(opline->extended_value, ce);
		}
		Z_CE_P(EX_VAR(opline->result.var)) = ce;
	} else {
		class_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
ZEND_VM_C_LABEL(try_class_name):
		if (Z_TYPE_P(class_name) == IS_OBJECT) {
			Z_CE_P(EX_VAR(opline->result.var)) = Z_OBJCE_P(class_name);
		} else if (Z_TYPE_P(class_name) == IS_STRING) {
			Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(Z_STR_P(class_name), opline->op1.num);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(class_name) == IS_REFERENCE) {
			class_name = Z_REFVAL_P(class_name);
			ZEND_VM_C_GOTO(try_class_name);
		} else {
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(class_name) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP2();
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "Class name must be a valid object or a string");
		}
	}

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_OBJ_HANDLER(112, ZEND_INIT_METHOD_CALL, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zval *function_name;
	zval *object;
	zend_function *fbc;
	zend_class_entry *called_scope;
	zend_object *obj;
	zend_execute_data *call;
	uint32_t call_info;

	SAVE_OPLINE();

	object = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP2_TYPE != IS_CONST) {
		function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	}

	if (OP2_TYPE != IS_CONST &&
	    UNEXPECTED(Z_TYPE_P(function_name) != IS_STRING)) {
		do {
			if ((OP2_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(function_name)) {
				function_name = Z_REFVAL_P(function_name);
				if (EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
					break;
				}
			} else if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(function_name) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP2();
				if (UNEXPECTED(EG(exception) != NULL)) {
					FREE_OP1();
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "Method name must be a string");
			FREE_OP2();
			FREE_OP1();
			HANDLE_EXCEPTION();
		} while (0);
	}

	if (OP1_TYPE == IS_UNUSED) {
		obj = Z_OBJ_P(object);
	} else {
		do {
			if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
				obj = Z_OBJ_P(object);
			} else {
				if ((OP1_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(object))) {
					zend_reference *ref = Z_REF_P(object);

					object = &ref->val;
					if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
						obj = Z_OBJ_P(object);
						if (OP1_TYPE & IS_VAR) {
							if (UNEXPECTED(GC_DELREF(ref) == 0)) {
								efree_size(ref, sizeof(zend_reference));
							} else {
								Z_ADDREF_P(object);
							}
						}
						break;
					}
				}
				if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
					object = ZVAL_UNDEFINED_OP1();
					if (UNEXPECTED(EG(exception) != NULL)) {
						if (OP2_TYPE != IS_CONST) {
							FREE_OP2();
						}
						HANDLE_EXCEPTION();
					}
				}
				if (OP2_TYPE == IS_CONST) {
					function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
				}
				zend_invalid_method_call(object, function_name);
				FREE_OP2();
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} while (0);
	}

	called_scope = obj->ce;

	if (OP2_TYPE == IS_CONST &&
	    EXPECTED(CACHED_PTR(opline->result.num) == called_scope)) {
		fbc = CACHED_PTR(opline->result.num + sizeof(void*));
	} else {
		zend_object *orig_obj = obj;

		if (OP2_TYPE == IS_CONST) {
			function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		}

		/* First, locate the function. */
		fbc = obj->handlers->get_method(&obj, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (RT_CONSTANT(opline, opline->op2) + 1) : NULL));
		if (UNEXPECTED(fbc == NULL)) {
			if (EXPECTED(!EG(exception))) {
				zend_undefined_method(obj->ce, Z_STR_P(function_name));
			}
			FREE_OP2();
			if ((OP1_TYPE & (IS_VAR|IS_TMP_VAR)) && GC_DELREF(orig_obj) == 0) {
				zend_objects_store_del(orig_obj);
			}
			HANDLE_EXCEPTION();
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE))) &&
		    EXPECTED(obj == orig_obj)) {
			CACHE_POLYMORPHIC_PTR(opline->result.num, called_scope, fbc);
		}
		if ((OP1_TYPE & (IS_VAR|IS_TMP_VAR)) && UNEXPECTED(obj != orig_obj)) {
			GC_ADDREF(obj); /* For $this pointer */
			if (GC_DELREF(orig_obj) == 0) {
				zend_objects_store_del(orig_obj);
			}
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	if (OP2_TYPE != IS_CONST) {
		FREE_OP2();
	}

	call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_HAS_THIS;
	if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0)) {
		if ((OP1_TYPE & (IS_VAR|IS_TMP_VAR)) && GC_DELREF(obj) == 0) {
			zend_objects_store_del(obj);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		/* call static method */
		obj = (zend_object*)called_scope;
		call_info = ZEND_CALL_NESTED_FUNCTION;
	} else if (OP1_TYPE & (IS_VAR|IS_TMP_VAR|IS_CV)) {
		if (OP1_TYPE == IS_CV) {
			GC_ADDREF(obj); /* For $this pointer */
		}
		/* CV may be changed indirectly (e.g. when it's a reference) */
		call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, obj);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, UNUSED|CLASS_FETCH|CONST|VAR, CONST|TMPVAR|UNUSED|CONSTRUCTOR|CV, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zval *function_name;
	zend_class_entry *ce;
	uint32_t call_info;
	zend_function *fbc;
	zend_execute_data *call;

	SAVE_OPLINE();

	if (OP1_TYPE == IS_CONST) {
		/* no function found. try a static method in class */
		ce = CACHED_PTR(opline->result.num);
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
			if (OP2_TYPE != IS_CONST) {
				CACHE_PTR(opline->result.num, ce);
			}
		}
	} else if (OP1_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op1.num);
		if (UNEXPECTED(ce == NULL)) {
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
	} else {
		ce = Z_CE_P(EX_VAR(opline->op1.var));
	}

	if (OP1_TYPE == IS_CONST &&
	    OP2_TYPE == IS_CONST &&
	    EXPECTED((fbc = CACHED_PTR(opline->result.num + sizeof(void*))) != NULL)) {
		/* nothing to do */
	} else if (OP1_TYPE != IS_CONST &&
	           OP2_TYPE == IS_CONST &&
	           EXPECTED(CACHED_PTR(opline->result.num) == ce)) {
		fbc = CACHED_PTR(opline->result.num + sizeof(void*));
	} else if (OP2_TYPE != IS_UNUSED) {
		function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (OP2_TYPE != IS_CONST) {
			if (UNEXPECTED(Z_TYPE_P(function_name) != IS_STRING)) {
				do {
					if (OP2_TYPE & (IS_VAR|IS_CV) && Z_ISREF_P(function_name)) {
						function_name = Z_REFVAL_P(function_name);
						if (EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
							break;
						}
					} else if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(function_name) == IS_UNDEF)) {
						ZVAL_UNDEFINED_OP2();
						if (UNEXPECTED(EG(exception) != NULL)) {
							HANDLE_EXCEPTION();
						}
					}
					zend_throw_error(NULL, "Method name must be a string");
					FREE_OP2();
					HANDLE_EXCEPTION();
				} while (0);
			}
		}

		if (ce->get_static_method) {
			fbc = ce->get_static_method(ce, Z_STR_P(function_name));
		} else {
			fbc = zend_std_get_static_method(ce, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (RT_CONSTANT(opline, opline->op2) + 1) : NULL));
		}
		if (UNEXPECTED(fbc == NULL)) {
			if (EXPECTED(!EG(exception))) {
				zend_undefined_method(ce, Z_STR_P(function_name));
			}
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE))) &&
			EXPECTED(!(fbc->common.scope->ce_flags & ZEND_ACC_TRAIT))) {
			CACHE_POLYMORPHIC_PTR(opline->result.num, ce, fbc);
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		if (OP2_TYPE != IS_CONST) {
			FREE_OP2();
		}
	} else {
		if (UNEXPECTED(ce->constructor == NULL)) {
			zend_throw_error(NULL, "Cannot call constructor");
			HANDLE_EXCEPTION();
		}
		if (Z_TYPE(EX(This)) == IS_OBJECT && Z_OBJ(EX(This))->ce != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_throw_error(NULL, "Cannot call private %s::__construct()", ZSTR_VAL(ce->name));
			HANDLE_EXCEPTION();
		}
		fbc = ce->constructor;
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
		if (Z_TYPE(EX(This)) == IS_OBJECT && instanceof_function(Z_OBJCE(EX(This)), ce)) {
			ce = (zend_class_entry*)Z_OBJ(EX(This));
			call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_HAS_THIS;
		} else {
			zend_non_static_method_call(fbc);
			HANDLE_EXCEPTION();
		}
	} else {
		/* previous opcode is ZEND_FETCH_CLASS */
		if (OP1_TYPE == IS_UNUSED
		 && ((opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_PARENT ||
		     (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF)) {
			if (Z_TYPE(EX(This)) == IS_OBJECT) {
				ce = Z_OBJCE(EX(This));
			} else {
				ce = Z_CE(EX(This));
			}
		}
		call_info = ZEND_CALL_NESTED_FUNCTION;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, ce);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zend_function *fbc;
	zval *function_name, *func;
	zend_execute_data *call;

	fbc = CACHED_PTR(opline->result.num);
	if (UNEXPECTED(fbc == NULL)) {
		function_name = (zval*)RT_CONSTANT(opline, opline->op2);
		func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(function_name+1));
		if (UNEXPECTED(func == NULL)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_undefined_function_helper);
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		CACHE_PTR(opline->result.num, fbc);
	}
	call = _zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(128, ZEND_INIT_DYNAMIC_CALL, ANY, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zval *function_name;
	zend_execute_data *call;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_function_name):
	if (OP2_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
		call = zend_init_dynamic_call_string(Z_STR_P(function_name), opline->extended_value);
	} else if (OP2_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(function_name) == IS_OBJECT)) {
		call = zend_init_dynamic_call_object(Z_OBJ_P(function_name), opline->extended_value);
	} else if (EXPECTED(Z_TYPE_P(function_name) == IS_ARRAY)) {
		call = zend_init_dynamic_call_array(Z_ARRVAL_P(function_name), opline->extended_value);
	} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_TYPE_P(function_name) == IS_REFERENCE)) {
		function_name = Z_REFVAL_P(function_name);
		ZEND_VM_C_GOTO(try_function_name);
	} else {
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(function_name) == IS_UNDEF)) {
			function_name = ZVAL_UNDEFINED_OP2();
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
		zend_throw_error(NULL, "Value of type %s is not callable",
			zend_zval_type_name(function_name));
		call = NULL;
	}

	if (OP2_TYPE & (IS_VAR|IS_TMP_VAR)) {
		FREE_OP2();
		if (UNEXPECTED(EG(exception))) {
			if (call) {
				 if (call->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
					zend_string_release_ex(call->func->common.function_name, 0);
					zend_free_trampoline(call->func);
				}
				zend_vm_stack_free_call_frame(call);
			}
			HANDLE_EXCEPTION();
		}
	} else if (!call) {
		HANDLE_EXCEPTION();
	}

	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(118, ZEND_INIT_USER_CALL, CONST, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zval *function_name;
	zend_fcall_info_cache fcc;
	char *error = NULL;
	zend_function *func;
	void *object_or_called_scope;
	zend_execute_data *call;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);
	if (zend_is_callable_ex(function_name, NULL, 0, NULL, &fcc, &error)) {
		ZEND_ASSERT(!error);
		func = fcc.function_handler;
		object_or_called_scope = fcc.called_scope;
		if (func->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
			call_info |= ZEND_CALL_CLOSURE;
			if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
				call_info |= ZEND_CALL_FAKE_CLOSURE;
			}
			if (fcc.object) {
				object_or_called_scope = fcc.object;
				call_info |= ZEND_CALL_HAS_THIS;
			}
		} else if (fcc.object) {
			GC_ADDREF(fcc.object); /* For $this pointer */
			object_or_called_scope = fcc.object;
			call_info |= ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_THIS;
		}

		FREE_OP2();
		if ((OP2_TYPE & (IS_TMP_VAR|IS_VAR)) && UNEXPECTED(EG(exception))) {
			if (call_info & ZEND_CALL_CLOSURE) {
				zend_object_release(ZEND_CLOSURE_OBJECT(func));
			} else if (call_info & ZEND_CALL_RELEASE_THIS) {
				zend_object_release(fcc.object);
			}
			HANDLE_EXCEPTION();
		}

		if (EXPECTED(func->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&func->op_array))) {
			init_func_run_time_cache(&func->op_array);
		}
	} else {
		zend_type_error("%s(): Argument #1 ($callback) must be a valid callback, %s", Z_STRVAL_P(RT_CONSTANT(opline, opline->op1)), error);
		efree(error);
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

	call = zend_vm_stack_push_call_frame(call_info,
		func, opline->extended_value, object_or_called_scope);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(69, ZEND_INIT_NS_FCALL_BY_NAME, ANY, CONST, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zval *func_name;
	zval *func;
	zend_function *fbc;
	zend_execute_data *call;

	fbc = CACHED_PTR(opline->result.num);
	if (UNEXPECTED(fbc == NULL)) {
		func_name = (zval *)RT_CONSTANT(opline, opline->op2);
		func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(func_name + 1));
		if (func == NULL) {
			func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(func_name + 2));
			if (UNEXPECTED(func == NULL)) {
				ZEND_VM_DISPATCH_TO_HELPER(zend_undefined_function_helper);
			}
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		CACHE_PTR(opline->result.num, fbc);
	}

	call = _zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(61, ZEND_INIT_FCALL, NUM, CONST, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zval *fname;
	zval *func;
	zend_function *fbc;
	zend_execute_data *call;

	fbc = CACHED_PTR(opline->result.num);
	if (UNEXPECTED(fbc == NULL)) {
		fname = (zval*)RT_CONSTANT(opline, opline->op2);
		func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(fname));
		ZEND_ASSERT(func != NULL && "Function existence must be checked at compile time");
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		CACHE_PTR(opline->result.num, fbc);
	}

	call = _zend_vm_stack_push_call_frame_ex(
		opline->op1.num, ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(129, ZEND_DO_ICALL, ANY, ANY, SPEC(RETVAL,OBSERVER))
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;
	zval retval;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	call->prev_execute_data = execute_data;
	EG(current_execute_data) = call;

#if ZEND_DEBUG
	bool should_throw = zend_internal_call_should_throw(fbc, call);
#endif

	ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
	ZVAL_NULL(ret);

	ZEND_OBSERVER_FCALL_BEGIN(call);
	fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
	if (!EG(exception) && call->func) {
		if (should_throw) {
			zend_internal_call_arginfo_violation(call->func);
		}
		ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
			zend_verify_internal_return_type(call->func, ret));
		ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
			? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
		zend_verify_internal_func_info(call->func, ret);
	}
#endif
	ZEND_OBSERVER_FCALL_END(call, EG(exception) ? NULL : ret);

	EG(current_execute_data) = execute_data;
	zend_vm_stack_free_args(call);

	uint32_t call_info = ZEND_CALL_INFO(call);
	if (UNEXPECTED(call_info & (ZEND_CALL_HAS_EXTRA_NAMED_PARAMS|ZEND_CALL_ALLOCATED))) {
		if (call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
			zend_free_extra_named_params(call->extra_named_params);
		}
		zend_vm_stack_free_call_frame_ex(call_info, call);
	} else {
		EG(vm_stack_top) = (zval*)call;
	}

	if (!RETURN_VALUE_USED(opline)) {
		i_zval_ptr_dtor(ret);
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION();
	}

	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HOT_HANDLER(130, ZEND_DO_UCALL, ANY, ANY, SPEC(RETVAL,OBSERVER))
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	ret = NULL;
	if (RETURN_VALUE_USED(opline)) {
		ret = EX_VAR(opline->result.var);
	}

	call->prev_execute_data = execute_data;
	execute_data = call;
	i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
	LOAD_OPLINE_EX();
	ZEND_OBSERVER_SAVE_OPLINE();
	ZEND_OBSERVER_FCALL_BEGIN(execute_data);

	ZEND_VM_ENTER_EX();
}

ZEND_VM_HOT_HANDLER(131, ZEND_DO_FCALL_BY_NAME, ANY, ANY, SPEC(RETVAL,OBSERVER))
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		ret = NULL;
		if (RETURN_VALUE_USED(opline)) {
			ret = EX_VAR(opline->result.var);
		}

		call->prev_execute_data = execute_data;
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
		LOAD_OPLINE_EX();
		ZEND_OBSERVER_SAVE_OPLINE();
		ZEND_OBSERVER_FCALL_BEGIN(execute_data);

		ZEND_VM_ENTER_EX();
	} else {
		zval retval;
		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);
		if (ZEND_OBSERVER_ENABLED) {
			ret = NULL;
		}

		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_deprecated_function(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				if (!RETURN_VALUE_USED(opline)) {
					ret = &retval;
					ZVAL_UNDEF(ret);
				}
				ZEND_VM_C_GOTO(fcall_by_name_end);
			}
		}

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

#if ZEND_DEBUG
		bool should_throw = zend_internal_call_should_throw(fbc, call);
#endif

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
		ZVAL_NULL(ret);

		ZEND_OBSERVER_FCALL_BEGIN(call);
		fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			if (should_throw) {
				zend_internal_call_arginfo_violation(call->func);
			}
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
			zend_verify_internal_func_info(call->func, ret);
		}
#endif
		ZEND_OBSERVER_FCALL_END(call, EG(exception) ? NULL : ret);

		EG(current_execute_data) = execute_data;

ZEND_VM_C_LABEL(fcall_by_name_end):
		zend_vm_stack_free_args(call);

		uint32_t call_info = ZEND_CALL_INFO(call);
		if (UNEXPECTED(call_info & (ZEND_CALL_HAS_EXTRA_NAMED_PARAMS|ZEND_CALL_ALLOCATED))) {
			if (call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
				zend_free_extra_named_params(call->extra_named_params);
			}
			zend_vm_stack_free_call_frame_ex(call_info, call);
		} else {
			EG(vm_stack_top) = (zval*)call;
		}

		if (!RETURN_VALUE_USED(opline)) {
			i_zval_ptr_dtor(ret);
		}
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION();
	}
	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HOT_HANDLER(60, ZEND_DO_FCALL, ANY, ANY, SPEC(RETVAL,OBSERVER))
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		ret = NULL;
		if (RETURN_VALUE_USED(opline)) {
			ret = EX_VAR(opline->result.var);
		}

		call->prev_execute_data = execute_data;
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 1 EXECUTE_DATA_CC);

		if (EXPECTED(zend_execute_ex == execute_ex)) {
			LOAD_OPLINE_EX();
			ZEND_OBSERVER_SAVE_OPLINE();
			ZEND_OBSERVER_FCALL_BEGIN(execute_data);
			ZEND_VM_ENTER_EX();
		} else {
			SAVE_OPLINE_EX();
			ZEND_OBSERVER_FCALL_BEGIN(execute_data);
			execute_data = EX(prev_execute_data);
			LOAD_OPLINE();
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
		}
	} else {
		zval retval;
		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);
		if (ZEND_OBSERVER_ENABLED) {
			ret = NULL;
		}

		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_deprecated_function(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				if (!RETURN_VALUE_USED(opline)) {
					ret = &retval;
					ZVAL_UNDEF(ret);
				}
				ZEND_VM_C_GOTO(fcall_end);
			}
		}

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

#if ZEND_DEBUG
		bool should_throw = zend_internal_call_should_throw(fbc, call);
#endif

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
		ZVAL_NULL(ret);

		ZEND_OBSERVER_FCALL_BEGIN(call);
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			fbc->internal_function.handler(call, ret);
		} else {
			zend_execute_internal(call, ret);
		}

#if ZEND_DEBUG
		if (!EG(exception) && call->func && !(call->func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE)) {
			if (should_throw) {
				zend_internal_call_arginfo_violation(call->func);
			}
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
			zend_verify_internal_func_info(call->func, ret);
		}
#endif
		ZEND_OBSERVER_FCALL_END(call, EG(exception) ? NULL : ret);

		EG(current_execute_data) = execute_data;

ZEND_VM_C_LABEL(fcall_end):
		zend_vm_stack_free_args(call);
		if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			zend_free_extra_named_params(call->extra_named_params);
		}

		if (!RETURN_VALUE_USED(opline)) {
			i_zval_ptr_dtor(ret);
		}
	}

	if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS)) {
		OBJ_RELEASE(Z_OBJ(call->This));
	}

	zend_vm_stack_free_call_frame(call);
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION();
	}

	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_COLD_CONST_HANDLER(124, ZEND_VERIFY_RETURN_TYPE, CONST|TMP|VAR|UNUSED|CV, UNUSED|CACHE_SLOT)
{
	if (OP1_TYPE == IS_UNUSED) {
		SAVE_OPLINE();
		zend_verify_missing_return_type(EX(func));
		HANDLE_EXCEPTION();
	} else {
/* prevents "undefined variable opline" errors */
#if !ZEND_VM_SPEC || (OP1_TYPE != IS_UNUSED)
		USE_OPLINE
		zval *retval_ref, *retval_ptr;
		zend_arg_info *ret_info = EX(func)->common.arg_info - 1;
		retval_ref = retval_ptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

		if (OP1_TYPE == IS_CONST) {
			ZVAL_COPY(EX_VAR(opline->result.var), retval_ptr);
			retval_ref = retval_ptr = EX_VAR(opline->result.var);
		} else if (OP1_TYPE == IS_VAR) {
			if (UNEXPECTED(Z_TYPE_P(retval_ptr) == IS_INDIRECT)) {
				retval_ref = retval_ptr = Z_INDIRECT_P(retval_ptr);
			}
			ZVAL_DEREF(retval_ptr);
		} else if (OP1_TYPE == IS_CV) {
			ZVAL_DEREF(retval_ptr);
		}

		if (EXPECTED(ZEND_TYPE_CONTAINS_CODE(ret_info->type, Z_TYPE_P(retval_ptr)))) {
			ZEND_VM_NEXT_OPCODE();
		}

		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_ISUNDEF_P(retval_ptr))) {
			SAVE_OPLINE();
			retval_ref = retval_ptr = ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
			if (ZEND_TYPE_FULL_MASK(ret_info->type) & MAY_BE_NULL) {
				ZEND_VM_NEXT_OPCODE();
			}
		}

		zend_reference *ref = NULL;
		void *cache_slot = CACHE_ADDR(opline->op2.num);
		if (UNEXPECTED(retval_ref != retval_ptr)) {
			if (UNEXPECTED(EX(func)->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
				ref = Z_REF_P(retval_ref);
			} else {
				/* A cast might happen - unwrap the reference if this is a by-value return */
				if (Z_REFCOUNT_P(retval_ref) == 1) {
					ZVAL_UNREF(retval_ref);
				} else {
					Z_DELREF_P(retval_ref);
					ZVAL_COPY(retval_ref, retval_ptr);
				}
				retval_ptr = retval_ref;
			}
		}

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_check_type_slow(&ret_info->type, retval_ptr, ref, cache_slot, 1, 0))) {
			zend_verify_return_error(EX(func), retval_ptr);
			HANDLE_EXCEPTION();
		}
		ZEND_VM_NEXT_OPCODE();
#endif
	}
}

ZEND_VM_COLD_HANDLER(201, ZEND_VERIFY_NEVER_TYPE, UNUSED, UNUSED)
{
	SAVE_OPLINE();
	zend_verify_never_error(EX(func));
	HANDLE_EXCEPTION();
}

ZEND_VM_INLINE_HANDLER(62, ZEND_RETURN, CONST|TMP|VAR|CV, ANY, SPEC(OBSERVER))
{
	USE_OPLINE
	zval *retval_ptr;
	zval *return_value;
	ZEND_OBSERVER_USE_RETVAL;

	retval_ptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	return_value = EX(return_value);
	ZEND_OBSERVER_SET_RETVAL();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(retval_ptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		retval_ptr = ZVAL_UNDEFINED_OP1();
		if (return_value) {
			ZVAL_NULL(return_value);
		}
	} else if (!return_value) {
		if (OP1_TYPE & (IS_VAR|IS_TMP_VAR)) {
			if (Z_REFCOUNTED_P(retval_ptr) && !Z_DELREF_P(retval_ptr)) {
				SAVE_OPLINE();
				rc_dtor_func(Z_COUNTED_P(retval_ptr));
			}
		}
	} else {
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
			ZVAL_COPY_VALUE(return_value, retval_ptr);
			if (OP1_TYPE == IS_CONST) {
				if (UNEXPECTED(Z_OPT_REFCOUNTED_P(return_value))) {
					Z_ADDREF_P(return_value);
				}
			}
		} else if (OP1_TYPE == IS_CV) {
			do {
				if (Z_OPT_REFCOUNTED_P(retval_ptr)) {
					if (EXPECTED(!Z_OPT_ISREF_P(retval_ptr))) {
						if (EXPECTED(!(EX_CALL_INFO() & (ZEND_CALL_CODE|ZEND_CALL_OBSERVED)))) {
							zend_refcounted *ref = Z_COUNTED_P(retval_ptr);
							ZVAL_COPY_VALUE(return_value, retval_ptr);
							if (GC_MAY_LEAK(ref)) {
								SAVE_OPLINE();
								gc_possible_root(ref);
							}
							ZVAL_NULL(retval_ptr);
							break;
						} else {
							Z_ADDREF_P(retval_ptr);
						}
					} else {
						retval_ptr = Z_REFVAL_P(retval_ptr);
						if (Z_OPT_REFCOUNTED_P(retval_ptr)) {
							Z_ADDREF_P(retval_ptr);
						}
					}
				}
				ZVAL_COPY_VALUE(return_value, retval_ptr);
			} while (0);
		} else /* if (OP1_TYPE == IS_VAR) */ {
			if (UNEXPECTED(Z_ISREF_P(retval_ptr))) {
				zend_refcounted *ref = Z_COUNTED_P(retval_ptr);

				retval_ptr = Z_REFVAL_P(retval_ptr);
				ZVAL_COPY_VALUE(return_value, retval_ptr);
				if (UNEXPECTED(GC_DELREF(ref) == 0)) {
					efree_size(ref, sizeof(zend_reference));
				} else if (Z_OPT_REFCOUNTED_P(retval_ptr)) {
					Z_ADDREF_P(retval_ptr);
				}
			} else {
				ZVAL_COPY_VALUE(return_value, retval_ptr);
			}
		}
	}
	ZEND_OBSERVER_SAVE_OPLINE();
	ZEND_OBSERVER_FCALL_END(execute_data, return_value);
	ZEND_OBSERVER_FREE_RETVAL();
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_COLD_CONST_HANDLER(111, ZEND_RETURN_BY_REF, CONST|TMP|VAR|CV, ANY, SRC, SPEC(OBSERVER))
{
	USE_OPLINE
	zval *retval_ptr;
	zval *return_value;
	ZEND_OBSERVER_USE_RETVAL;

	SAVE_OPLINE();

	return_value = EX(return_value);
	ZEND_OBSERVER_SET_RETVAL();
	do {
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR)) ||
		    (OP1_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_VALUE)) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");

			retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
			if (!return_value) {
				FREE_OP1();
			} else {
				if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISREF_P(retval_ptr))) {
					ZVAL_COPY_VALUE(return_value, retval_ptr);
					break;
				}

				ZVAL_NEW_REF(return_value, retval_ptr);
				if (OP1_TYPE == IS_CONST) {
					Z_TRY_ADDREF_P(retval_ptr);
				}
			}
			break;
		}

		retval_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

		if (OP1_TYPE == IS_VAR) {
			ZEND_ASSERT(retval_ptr != &EG(uninitialized_zval));
			if (opline->extended_value == ZEND_RETURNS_FUNCTION && !Z_ISREF_P(retval_ptr)) {
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				if (return_value) {
					ZVAL_NEW_REF(return_value, retval_ptr);
				} else {
					FREE_OP1();
				}
				break;
			}
		}

		if (return_value) {
			if (Z_ISREF_P(retval_ptr)) {
				Z_ADDREF_P(retval_ptr);
			} else {
				ZVAL_MAKE_REF_EX(retval_ptr, 2);
			}
			ZVAL_REF(return_value, Z_REF_P(retval_ptr));
		}

		FREE_OP1();
	} while (0);

	ZEND_OBSERVER_FCALL_END(execute_data, return_value);
	ZEND_OBSERVER_FREE_RETVAL();
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_HANDLER(139, ZEND_GENERATOR_CREATE, ANY, ANY)
{
	zval *return_value = EX(return_value);

	if (EXPECTED(return_value)) {
		USE_OPLINE
		zend_generator *generator;
		zend_execute_data *gen_execute_data;
		uint32_t num_args, used_stack, call_info;

		SAVE_OPLINE();
		object_init_ex(return_value, zend_ce_generator);

		/*
		 * Normally the execute_data is allocated on the VM stack (because it does
		 * not actually do any allocation and thus is faster). For generators
		 * though this behavior would be suboptimal, because the (rather large)
		 * structure would have to be copied back and forth every time execution is
		 * suspended or resumed. That's why for generators the execution context
		 * is allocated on heap.
		 */
		num_args = EX_NUM_ARGS();
		if (EXPECTED(num_args <= EX(func)->op_array.num_args)) {
			used_stack = (ZEND_CALL_FRAME_SLOT + EX(func)->op_array.last_var + EX(func)->op_array.T) * sizeof(zval);
			gen_execute_data = (zend_execute_data*)emalloc(used_stack);
			used_stack = (ZEND_CALL_FRAME_SLOT + EX(func)->op_array.last_var) * sizeof(zval);
		} else {
			used_stack = (ZEND_CALL_FRAME_SLOT + num_args + EX(func)->op_array.last_var + EX(func)->op_array.T - EX(func)->op_array.num_args) * sizeof(zval);
			gen_execute_data = (zend_execute_data*)emalloc(used_stack);
		}
		memcpy(gen_execute_data, execute_data, used_stack);

		/* Save execution context in generator object. */
		generator = (zend_generator *) Z_OBJ_P(EX(return_value));
		generator->execute_data = gen_execute_data;
		generator->frozen_call_stack = NULL;
		generator->execute_fake.opline = NULL;
		generator->execute_fake.func = NULL;
		generator->execute_fake.prev_execute_data = NULL;
		ZVAL_OBJ(&generator->execute_fake.This, (zend_object *) generator);

		gen_execute_data->opline = opline + 1;
		/* EX(return_value) keeps pointer to zend_object (not a real zval) */
		gen_execute_data->return_value = (zval*)generator;
		call_info = Z_TYPE_INFO(EX(This));
		if ((call_info & Z_TYPE_MASK) == IS_OBJECT
		 && (!(call_info & (ZEND_CALL_CLOSURE|ZEND_CALL_RELEASE_THIS))
			 /* Bug #72523 */
			|| UNEXPECTED(zend_execute_ex != execute_ex))) {
			ZEND_ADD_CALL_FLAG_EX(call_info, ZEND_CALL_RELEASE_THIS);
			Z_ADDREF(gen_execute_data->This);
		}
		ZEND_ADD_CALL_FLAG_EX(call_info, (ZEND_CALL_TOP_FUNCTION | ZEND_CALL_ALLOCATED | ZEND_CALL_GENERATOR));
		Z_TYPE_INFO(gen_execute_data->This) = call_info;
		gen_execute_data->prev_execute_data = NULL;

		call_info = EX_CALL_INFO();
		EG(current_execute_data) = EX(prev_execute_data);
		if (EXPECTED(!(call_info & (ZEND_CALL_TOP|ZEND_CALL_ALLOCATED)))) {
			EG(vm_stack_top) = (zval*)execute_data;
			execute_data = EX(prev_execute_data);
			LOAD_NEXT_OPLINE();
			ZEND_VM_LEAVE();
		} else if (EXPECTED(!(call_info & ZEND_CALL_TOP))) {
			zend_execute_data *old_execute_data = execute_data;
			execute_data = EX(prev_execute_data);
			zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);
			LOAD_NEXT_OPLINE();
			ZEND_VM_LEAVE();
		} else {
			ZEND_VM_RETURN();
		}
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
	}
}

ZEND_VM_HANDLER(161, ZEND_GENERATOR_RETURN, CONST|TMP|VAR|CV, ANY, SPEC(OBSERVER))
{
	USE_OPLINE
	zval *retval;

	zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);

	SAVE_OPLINE();
	retval = GET_OP1_ZVAL_PTR(BP_VAR_R);

	/* Copy return value into generator->retval */
	if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
		ZVAL_COPY_VALUE(&generator->retval, retval);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_REFCOUNTED(generator->retval))) {
				Z_ADDREF(generator->retval);
			}
		}
	} else if (OP1_TYPE == IS_CV) {
		ZVAL_COPY_DEREF(&generator->retval, retval);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_refcounted *ref = Z_COUNTED_P(retval);

			retval = Z_REFVAL_P(retval);
			ZVAL_COPY_VALUE(&generator->retval, retval);
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				efree_size(ref, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(retval)) {
				Z_ADDREF_P(retval);
			}
		} else {
			ZVAL_COPY_VALUE(&generator->retval, retval);
		}
	}

	ZEND_OBSERVER_FCALL_END(generator->execute_data, &generator->retval);

	EG(current_execute_data) = EX(prev_execute_data);

	/* Close the generator to free up resources */
	zend_generator_close(generator, 1);

	/* Pass execution back to handling code */
	ZEND_VM_RETURN();
}

ZEND_VM_COLD_CONST_HANDLER(108, ZEND_THROW, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *value;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	do {
		if (OP1_TYPE == IS_CONST || UNEXPECTED(Z_TYPE_P(value) != IS_OBJECT)) {
			if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(value)) {
				value = Z_REFVAL_P(value);
				if (EXPECTED(Z_TYPE_P(value) == IS_OBJECT)) {
					break;
				}
			}
			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "Can only throw objects");
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} while (0);

	zend_exception_save();
	Z_TRY_ADDREF_P(value);
	zend_throw_exception_object(value);
	zend_exception_restore();
	FREE_OP1();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, CONST, JMP_ADDR, LAST_CATCH|CACHE_SLOT)
{
	USE_OPLINE
	zend_class_entry *ce, *catch_ce;
	zend_object *exception;

	SAVE_OPLINE();
	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore();
	if (EG(exception) == NULL) {
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}
	catch_ce = CACHED_PTR(opline->extended_value & ~ZEND_LAST_CATCH);
	if (UNEXPECTED(catch_ce == NULL)) {
		catch_ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1), ZEND_FETCH_CLASS_NO_AUTOLOAD | ZEND_FETCH_CLASS_SILENT);

		CACHE_PTR(opline->extended_value & ~ZEND_LAST_CATCH, catch_ce);
	}
	ce = EG(exception)->ce;

#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_CAUGHT_ENABLED()) {
		DTRACE_EXCEPTION_CAUGHT((char *)ce->name);
	}
#endif /* HAVE_DTRACE */

	if (ce != catch_ce) {
		if (!catch_ce || !instanceof_function(ce, catch_ce)) {
			if (opline->extended_value & ZEND_LAST_CATCH) {
				zend_rethrow_exception(execute_data);
				HANDLE_EXCEPTION();
			}
			ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
		}
	}

	exception = EG(exception);
	EG(exception) = NULL;
	if (RETURN_VALUE_USED(opline)) {
		/* Always perform a strict assignment. There is a reasonable expectation that if you
		 * write "catch (Exception $e)" then $e will actually be instanceof Exception. As such,
		 * we should not permit coercion to string here. */
		zval tmp;
		ZVAL_OBJ(&tmp, exception);
		zend_assign_to_variable(EX_VAR(opline->result.var), &tmp, IS_TMP_VAR, /* strict */ 1);
	} else {
		OBJ_RELEASE(exception);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(65, ZEND_SEND_VAL, CONST|TMPVAR, CONST|UNUSED|NUM)
{
	USE_OPLINE
	zval *value, *arg;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		uint32_t arg_num;
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	}

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	ZVAL_COPY_VALUE(arg, value);
	if (OP1_TYPE == IS_CONST) {
		if (UNEXPECTED(Z_OPT_REFCOUNTED_P(arg))) {
			Z_ADDREF_P(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HELPER(zend_cannot_pass_by_ref_helper, ANY, ANY, uint32_t _arg_num, zval *_arg)
{
	USE_OPLINE

	SAVE_OPLINE();

	zend_cannot_pass_by_reference(_arg_num);
	FREE_OP1();
	ZVAL_UNDEF(_arg);
	HANDLE_EXCEPTION();
}

ZEND_VM_HOT_SEND_HANDLER(116, ZEND_SEND_VAL_EX, CONST|TMP, CONST|UNUSED|NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *value, *arg;
	uint32_t arg_num;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		arg_num = opline->op2.num;
	}

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_val_by_ref);
		}
	} else if (ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
ZEND_VM_C_LABEL(send_val_by_ref):
		ZEND_VM_DISPATCH_TO_HELPER(zend_cannot_pass_by_ref_helper, _arg_num, arg_num, _arg, arg);
	}
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	ZVAL_COPY_VALUE(arg, value);
	if (OP1_TYPE == IS_CONST) {
		if (UNEXPECTED(Z_OPT_REFCOUNTED_P(arg))) {
			Z_ADDREF_P(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(117, ZEND_SEND_VAR, VAR|CV, CONST|UNUSED|NUM)
{
	USE_OPLINE
	zval *varptr, *arg;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		uint32_t arg_num;
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(varptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(arg);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY_DEREF(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(varptr))) {
			zend_refcounted *ref = Z_COUNTED_P(varptr);

			varptr = Z_REFVAL_P(varptr);
			ZVAL_COPY_VALUE(arg, varptr);
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				efree_size(ref, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(arg)) {
				Z_ADDREF_P(arg);
			}
		} else {
			ZVAL_COPY_VALUE(arg, varptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(106, ZEND_SEND_VAR_NO_REF, VAR, CONST|UNUSED|NUM)
{
	USE_OPLINE
	zval *varptr, *arg;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		uint32_t arg_num;
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	}

	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	ZVAL_COPY_VALUE(arg, varptr);

	if (EXPECTED(Z_ISREF_P(varptr))) {
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	ZVAL_NEW_REF(arg, arg);
	zend_error(E_NOTICE, "Only variables should be passed by reference");
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_SEND_HANDLER(50, ZEND_SEND_VAR_NO_REF_EX, VAR, CONST|UNUSED|NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *varptr, *arg;
	uint32_t arg_num;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		arg_num = opline->op2.num;
	}

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (!QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_var);
		}

		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		ZVAL_COPY_VALUE(arg, varptr);

		if (EXPECTED(Z_ISREF_P(varptr) ||
		    QUICK_ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num))) {
			ZEND_VM_NEXT_OPCODE();
		}
	} else {
		if (!ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_var);
		}

		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		ZVAL_COPY_VALUE(arg, varptr);

		if (EXPECTED(Z_ISREF_P(varptr) ||
		    ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num))) {
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	ZVAL_NEW_REF(arg, arg);
	zend_error(E_NOTICE, "Only variables should be passed by reference");
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();

ZEND_VM_C_LABEL(send_var):
	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	if (UNEXPECTED(Z_ISREF_P(varptr))) {
		zend_refcounted *ref = Z_COUNTED_P(varptr);

		varptr = Z_REFVAL_P(varptr);
		ZVAL_COPY_VALUE(arg, varptr);
		if (UNEXPECTED(GC_DELREF(ref) == 0)) {
			efree_size(ref, sizeof(zend_reference));
		} else if (Z_OPT_REFCOUNTED_P(arg)) {
			Z_ADDREF_P(arg);
		}
	} else {
		ZVAL_COPY_VALUE(arg, varptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(67, ZEND_SEND_REF, VAR|CV, CONST|UNUSED|NUM)
{
	USE_OPLINE
	zval *varptr, *arg;

	SAVE_OPLINE();
	if (OP2_TYPE == IS_CONST) {
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		uint32_t arg_num;
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	}

	varptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
	if (Z_ISREF_P(varptr)) {
		Z_ADDREF_P(varptr);
	} else {
		ZVAL_MAKE_REF_EX(varptr, 2);
	}
	ZVAL_REF(arg, Z_REF_P(varptr));

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_SEND_HANDLER(66, ZEND_SEND_VAR_EX, VAR|CV, CONST|UNUSED|NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *varptr, *arg;
	uint32_t arg_num;

	if (OP2_TYPE == IS_CONST) {
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		arg_num = opline->op2.num;
	}

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_var_by_ref);
		}
	} else if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
ZEND_VM_C_LABEL(send_var_by_ref):
		varptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		if (Z_ISREF_P(varptr)) {
			Z_ADDREF_P(varptr);
		} else {
			ZVAL_MAKE_REF_EX(varptr, 2);
		}
		ZVAL_REF(arg, Z_REF_P(varptr));

		FREE_OP1();
		ZEND_VM_NEXT_OPCODE();
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(varptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(arg);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY_DEREF(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(varptr))) {
			zend_refcounted *ref = Z_COUNTED_P(varptr);

			varptr = Z_REFVAL_P(varptr);
			ZVAL_COPY_VALUE(arg, varptr);
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				efree_size(ref, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(arg)) {
				Z_ADDREF_P(arg);
			}
		} else {
			ZVAL_COPY_VALUE(arg, varptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_SEND_HANDLER(100, ZEND_CHECK_FUNC_ARG, UNUSED, CONST|UNUSED|NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	uint32_t arg_num;

	if (OP2_TYPE == IS_CONST) {
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		arg_num = zend_get_arg_offset_by_name(
			EX(call)->func, arg_name, CACHE_ADDR(opline->result.num)) + 1;
		if (UNEXPECTED(arg_num == 0)) {
			/* Treat this as a by-value argument, and throw an error during SEND. */
			ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
			ZEND_VM_NEXT_OPCODE();
		}
	} else {
		arg_num = opline->op2.num;
	}

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_ADD_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		} else {
			ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		}
	} else if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		ZEND_ADD_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
	} else {
		ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(185, ZEND_SEND_FUNC_ARG, VAR, CONST|UNUSED|NUM)
{
	USE_OPLINE
	zval *varptr, *arg;

	if (OP2_TYPE == IS_CONST) {
		// TODO: Would it make sense to share the cache slot with CHECK_FUNC_ARG?
		SAVE_OPLINE();
		zend_string *arg_name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		uint32_t arg_num;
		arg = zend_handle_named_arg(&EX(call), arg_name, &arg_num, CACHE_ADDR(opline->result.num));
		if (UNEXPECTED(!arg)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	}

	if (UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) {
		varptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		if (Z_ISREF_P(varptr)) {
			Z_ADDREF_P(varptr);
		} else {
			ZVAL_MAKE_REF_EX(varptr, 2);
		}
		ZVAL_REF(arg, Z_REF_P(varptr));

		FREE_OP1();
		ZEND_VM_NEXT_OPCODE();
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (UNEXPECTED(Z_ISREF_P(varptr))) {
		zend_refcounted *ref = Z_COUNTED_P(varptr);

		varptr = Z_REFVAL_P(varptr);
		ZVAL_COPY_VALUE(arg, varptr);
		if (UNEXPECTED(GC_DELREF(ref) == 0)) {
			efree_size(ref, sizeof(zend_reference));
		} else if (Z_OPT_REFCOUNTED_P(arg)) {
			Z_ADDREF_P(arg);
		}
	} else {
		ZVAL_COPY_VALUE(arg, varptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(165, ZEND_SEND_UNPACK, ANY, ANY)
{
	USE_OPLINE
	zval *args;
	uint32_t arg_num;

	SAVE_OPLINE();
	args = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg_num = ZEND_CALL_NUM_ARGS(EX(call)) + 1;

ZEND_VM_C_LABEL(send_again):
	if (EXPECTED(Z_TYPE_P(args) == IS_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(args);
		zval *arg, *top;
		zend_string *name;
		bool have_named_params = 0;

		zend_vm_stack_extend_call_frame(&EX(call), arg_num - 1, zend_hash_num_elements(ht));

		// TODO: Speed this up using a flag that specifies whether there are any ref parameters.
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_REFCOUNT_P(args) > 1) {
			uint32_t tmp_arg_num = arg_num;
			bool separate = 0;

			/* check if any of arguments are going to be passed by reference */
			ZEND_HASH_FOREACH_STR_KEY_VAL(ht, name, arg) {
				if (UNEXPECTED(name)) {
					void *cache_slot[2] = {NULL, NULL};
					tmp_arg_num = zend_get_arg_offset_by_name(
						EX(call)->func, name, cache_slot) + 1;
				}
				if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, tmp_arg_num)) {
					separate = 1;
					break;
				}
				tmp_arg_num++;
			} ZEND_HASH_FOREACH_END();
			if (separate) {
				SEPARATE_ARRAY(args);
				ht = Z_ARRVAL_P(args);
			}
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, name, arg) {
			if (UNEXPECTED(name)) {
				void *cache_slot[2] = {NULL, NULL};
				have_named_params = 1;
				top = zend_handle_named_arg(&EX(call), name, &arg_num, cache_slot);
				if (UNEXPECTED(!top)) {
					FREE_OP1();
					HANDLE_EXCEPTION();
				}
			} else {
				if (have_named_params) {
					zend_throw_error(NULL,
						"Cannot use positional argument after named argument during unpacking");
					FREE_OP1();
					HANDLE_EXCEPTION();
				}

				top = ZEND_CALL_ARG(EX(call), arg_num);
				ZEND_CALL_NUM_ARGS(EX(call))++;
			}

			if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
				if (Z_ISREF_P(arg)) {
					Z_ADDREF_P(arg);
					ZVAL_REF(top, Z_REF_P(arg));
				} else if (OP1_TYPE & (IS_VAR|IS_CV)) {
					/* array is already separated above */
					ZVAL_MAKE_REF_EX(arg, 2);
					ZVAL_REF(top, Z_REF_P(arg));
				} else {
					Z_TRY_ADDREF_P(arg);
					ZVAL_NEW_REF(top, arg);
				}
			} else {
				ZVAL_COPY_DEREF(top, arg);
			}

			arg_num++;
		} ZEND_HASH_FOREACH_END();

	} else if (EXPECTED(Z_TYPE_P(args) == IS_OBJECT)) {
		zend_class_entry *ce = Z_OBJCE_P(args);
		zend_object_iterator *iter;
		bool have_named_params = 0;

		if (!ce || !ce->get_iterator) {
			zend_type_error("Only arrays and Traversables can be unpacked");
		} else {

			iter = ce->get_iterator(ce, args, 0);
			if (UNEXPECTED(!iter)) {
				FREE_OP1();
				if (!EG(exception)) {
					zend_throw_exception_ex(
						NULL, 0, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name)
					);
				}
				HANDLE_EXCEPTION();
			}

			const zend_object_iterator_funcs *funcs = iter->funcs;
			if (funcs->rewind) {
				funcs->rewind(iter);
			}

			for (; funcs->valid(iter) == SUCCESS; ++arg_num) {
				zval *arg, *top;

				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				arg = funcs->get_current_data(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				zend_string *name = NULL;
				if (funcs->get_current_key) {
					zval key;
					funcs->get_current_key(iter, &key);
					if (UNEXPECTED(EG(exception) != NULL)) {
						break;
					}

					if (UNEXPECTED(Z_TYPE(key) != IS_LONG)) {
						if (UNEXPECTED(Z_TYPE(key) != IS_STRING)) {
							zend_throw_error(NULL,
								"Keys must be of type int|string during argument unpacking");
							zval_ptr_dtor(&key);
							break;
						}

						name = Z_STR_P(&key);
					}
				}

				if (UNEXPECTED(name)) {
					void *cache_slot[2] = {NULL, NULL};
					have_named_params = 1;
					top = zend_handle_named_arg(&EX(call), name, &arg_num, cache_slot);
					if (UNEXPECTED(!top)) {
						zend_string_release(name);
						break;
					}

					ZVAL_DEREF(arg);
					Z_TRY_ADDREF_P(arg);

					if (ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
						zend_error(
							E_WARNING, "Cannot pass by-reference argument %d of %s%s%s()"
							" by unpacking a Traversable, passing by-value instead", arg_num,
							EX(call)->func->common.scope ? ZSTR_VAL(EX(call)->func->common.scope->name) : "",
							EX(call)->func->common.scope ? "::" : "",
							ZSTR_VAL(EX(call)->func->common.function_name)
						);
						ZVAL_NEW_REF(top, arg);
					} else {
						ZVAL_COPY_VALUE(top, arg);
					}

					zend_string_release(name);
				} else {
					if (have_named_params) {
						zend_throw_error(NULL,
							"Cannot use positional argument after named argument during unpacking");
						break;
					}

					zend_vm_stack_extend_call_frame(&EX(call), arg_num - 1, 1);
					top = ZEND_CALL_ARG(EX(call), arg_num);
					ZVAL_DEREF(arg);
					Z_TRY_ADDREF_P(arg);

					if (ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
						zend_error(
							E_WARNING, "Cannot pass by-reference argument %d of %s%s%s()"
							" by unpacking a Traversable, passing by-value instead", arg_num,
							EX(call)->func->common.scope ? ZSTR_VAL(EX(call)->func->common.scope->name) : "",
							EX(call)->func->common.scope ? "::" : "",
							ZSTR_VAL(EX(call)->func->common.function_name)
						);
						ZVAL_NEW_REF(top, arg);
					} else {
						ZVAL_COPY_VALUE(top, arg);
					}

					ZEND_CALL_NUM_ARGS(EX(call))++;
				}

				funcs->move_forward(iter);
			}

			zend_iterator_dtor(iter);
		}
	} else if (EXPECTED(Z_ISREF_P(args))) {
		args = Z_REFVAL_P(args);
		ZEND_VM_C_GOTO(send_again);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(args) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		zend_type_error("Only arrays and Traversables can be unpacked");
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(119, ZEND_SEND_ARRAY, ANY, ANY, NUM)
{
	USE_OPLINE
	zval *args;

	SAVE_OPLINE();
	args = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(Z_TYPE_P(args) != IS_ARRAY)) {
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(args)) {
			args = Z_REFVAL_P(args);
			if (EXPECTED(Z_TYPE_P(args) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(send_array);
			}
		}
		zend_type_error("call_user_func_array(): Argument #2 ($args) must be of type array, %s given", zend_zval_value_name(args));
		FREE_OP2();
		FREE_OP1();
		HANDLE_EXCEPTION();
	} else {
		uint32_t arg_num;
		HashTable *ht;
		zval *arg, *param;

ZEND_VM_C_LABEL(send_array):
		ht = Z_ARRVAL_P(args);
		if (OP2_TYPE != IS_UNUSED) {
			/* We don't need to handle named params in this case,
			 * because array_slice() is called with $preserve_keys == false. */
			zval *op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
			uint32_t skip = opline->extended_value;
			uint32_t count = zend_hash_num_elements(ht);
			zend_long len;
			if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				len = Z_LVAL_P(op2);
			} else if (Z_TYPE_P(op2) == IS_NULL) {
				len = count - skip;
			} else if (EX_USES_STRICT_TYPES()
					|| !zend_parse_arg_long_weak(op2, &len, /* arg_num */ 3)) {
				zend_type_error(
					"array_slice(): Argument #3 ($length) must be of type ?int, %s given",
					zend_zval_value_name(op2));
				FREE_OP2();
				FREE_OP1();
				HANDLE_EXCEPTION();
			}

			if (len < 0) {
				len += (zend_long)(count - skip);
			}
			if (skip < count && len > 0) {
				if (len > (zend_long)(count - skip)) {
					len = (zend_long)(count - skip);
				}
				zend_vm_stack_extend_call_frame(&EX(call), 0, len);
				arg_num = 1;
				param = ZEND_CALL_ARG(EX(call), 1);
				ZEND_HASH_FOREACH_VAL(ht, arg) {
					bool must_wrap = 0;
					if (skip > 0) {
						skip--;
						continue;
					} else if ((zend_long)(arg_num - 1) >= len) {
						break;
					} else if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
						if (UNEXPECTED(!Z_ISREF_P(arg))) {
							if (!ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
								/* By-value send is not allowed -- emit a warning,
								 * but still perform the call. */
								zend_param_must_be_ref(EX(call)->func, arg_num);
								must_wrap = 1;
							}
						}
					} else {
						if (Z_ISREF_P(arg) &&
						    !(EX(call)->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
							/* don't separate references for __call */
							arg = Z_REFVAL_P(arg);
						}
					}
					if (EXPECTED(!must_wrap)) {
						ZVAL_COPY(param, arg);
					} else {
						Z_TRY_ADDREF_P(arg);
						ZVAL_NEW_REF(param, arg);
					}
					ZEND_CALL_NUM_ARGS(EX(call))++;
					arg_num++;
					param++;
				} ZEND_HASH_FOREACH_END();
			}
			FREE_OP2();
		} else {
			zend_string *name;
			bool have_named_params;
			zend_vm_stack_extend_call_frame(&EX(call), 0, zend_hash_num_elements(ht));
			arg_num = 1;
			param = ZEND_CALL_ARG(EX(call), 1);
			have_named_params = 0;
			ZEND_HASH_FOREACH_STR_KEY_VAL(ht, name, arg) {
				if (name) {
					void *cache_slot[2] = {NULL, NULL};
					have_named_params = 1;
					param = zend_handle_named_arg(&EX(call), name, &arg_num, cache_slot);
					if (!param) {
						FREE_OP1();
						HANDLE_EXCEPTION();
					}
				} else if (have_named_params) {
					zend_throw_error(NULL,
						"Cannot use positional argument after named argument");
					FREE_OP1();
					HANDLE_EXCEPTION();
				}

				bool must_wrap = 0;
				if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
					if (UNEXPECTED(!Z_ISREF_P(arg))) {
						if (!ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
							/* By-value send is not allowed -- emit a warning,
							 * but still perform the call. */
							zend_param_must_be_ref(EX(call)->func, arg_num);
							must_wrap = 1;
						}
					}
				} else {
					if (Z_ISREF_P(arg) &&
					    !(EX(call)->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
						/* don't separate references for __call */
						arg = Z_REFVAL_P(arg);
					}
				}

				if (EXPECTED(!must_wrap)) {
					ZVAL_COPY(param, arg);
				} else {
					Z_TRY_ADDREF_P(arg);
					ZVAL_NEW_REF(param, arg);
				}
				if (!name) {
					ZEND_CALL_NUM_ARGS(EX(call))++;
					arg_num++;
					param++;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(120, ZEND_SEND_USER, CONST|TMP|VAR|CV, NUM)
{
	USE_OPLINE
	zval *arg, *param;

	SAVE_OPLINE();

	arg = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	param = ZEND_CALL_VAR(EX(call), opline->result.var);
	if (UNEXPECTED(ARG_MUST_BE_SENT_BY_REF(EX(call)->func, opline->op2.num))) {
		zend_param_must_be_ref(EX(call)->func, opline->op2.num);
		Z_TRY_ADDREF_P(arg);
		ZVAL_NEW_REF(param, arg);
	} else {
		ZVAL_COPY(param, arg);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(199, ZEND_CHECK_UNDEF_ARGS, UNUSED, UNUSED)
{
	USE_OPLINE

	zend_execute_data *call = execute_data->call;
	if (EXPECTED(!(ZEND_CALL_INFO(call) & ZEND_CALL_MAY_HAVE_UNDEF))) {
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	zend_handle_undef_args(call);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_missing_arg_helper, ANY, ANY)
{
#ifdef ZEND_VM_IP_GLOBAL_REG
	USE_OPLINE

	SAVE_OPLINE();
#endif
	zend_missing_arg_error(execute_data);
	HANDLE_EXCEPTION();
}

ZEND_VM_HELPER(zend_verify_recv_arg_type_helper, ANY, ANY, zval *op_1)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(!zend_verify_recv_arg_type(EX(func), opline->op1.num, op_1, CACHE_ADDR(opline->extended_value)))) {
		HANDLE_EXCEPTION();
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(63, ZEND_RECV, NUM, UNUSED, CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;
	zval *param;

	if (UNEXPECTED(arg_num > EX_NUM_ARGS())) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_missing_arg_helper);
	}

	param = EX_VAR(opline->result.var);

	if (UNEXPECTED(!(opline->op2.num & (1u << Z_TYPE_P(param))))) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_verify_recv_arg_type_helper, op_1, param);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_RECV, op->op2.num == MAY_BE_ANY, ZEND_RECV_NOTYPE, NUM, NUM, CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;

	if (UNEXPECTED(arg_num > EX_NUM_ARGS())) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_missing_arg_helper);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(64, ZEND_RECV_INIT, NUM, CONST, CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num;
	zval *param;

	ZEND_VM_REPEATABLE_OPCODE

	arg_num = opline->op1.num;
	param = EX_VAR(opline->result.var);
	if (arg_num > EX_NUM_ARGS()) {
		zval *default_value = RT_CONSTANT(opline, opline->op2);

		if (Z_OPT_TYPE_P(default_value) == IS_CONSTANT_AST) {
			zval *cache_val = (zval*)CACHE_ADDR(Z_CACHE_SLOT_P(default_value));

			/* we keep in cache only not refcounted values */
			if (Z_TYPE_P(cache_val) != IS_UNDEF) {
				ZVAL_COPY_VALUE(param, cache_val);
			} else {
				SAVE_OPLINE();
				ZVAL_COPY(param, default_value);
				zend_ast_evaluate_ctx ctx = {0};
				if (UNEXPECTED(zval_update_constant_with_ctx(param, EX(func)->op_array.scope, &ctx) != SUCCESS)) {
					zval_ptr_dtor_nogc(param);
					ZVAL_UNDEF(param);
					HANDLE_EXCEPTION();
				}
				if (!Z_REFCOUNTED_P(param) && !ctx.had_side_effects) {
					ZVAL_COPY_VALUE(cache_val, param);
				}
			}
			ZEND_VM_C_GOTO(recv_init_check_type);
		} else {
			ZVAL_COPY(param, default_value);
		}
	} else {
ZEND_VM_C_LABEL(recv_init_check_type):
		if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
			SAVE_OPLINE();
			if (UNEXPECTED(!zend_verify_recv_arg_type(EX(func), arg_num, param, CACHE_ADDR(opline->extended_value)))) {
				HANDLE_EXCEPTION();
			}
		}
	}

	ZEND_VM_REPEAT_OPCODE(ZEND_RECV_INIT);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(164, ZEND_RECV_VARIADIC, NUM, UNUSED, CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;
	uint32_t arg_count = EX_NUM_ARGS();
	zval *params;

	SAVE_OPLINE();

	params = EX_VAR(opline->result.var);

	if (arg_num <= arg_count) {
		ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_VARIADIC);
		ZEND_ASSERT(EX(func)->common.num_args == arg_num - 1);
		zend_arg_info *arg_info = &EX(func)->common.arg_info[arg_num - 1];

		array_init_size(params, arg_count - arg_num + 1);
		zend_hash_real_init_packed(Z_ARRVAL_P(params));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(params)) {
			zval *param = EX_VAR_NUM(EX(func)->op_array.last_var + EX(func)->op_array.T);
			if (UNEXPECTED(ZEND_TYPE_IS_SET(arg_info->type))) {
				ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_FREE_EXTRA_ARGS);
				do {
					if (UNEXPECTED(!zend_verify_variadic_arg_type(EX(func), arg_info, arg_num, param, CACHE_ADDR(opline->extended_value)))) {
						ZEND_HASH_FILL_FINISH();
						HANDLE_EXCEPTION();
					}

					if (Z_OPT_REFCOUNTED_P(param)) Z_ADDREF_P(param);
					ZEND_HASH_FILL_ADD(param);
					param++;
				} while (++arg_num <= arg_count);
			} else {
				do {
					if (Z_OPT_REFCOUNTED_P(param)) Z_ADDREF_P(param);
					ZEND_HASH_FILL_ADD(param);
					param++;
				} while (++arg_num <= arg_count);
			}
		} ZEND_HASH_FILL_END();
	} else {
		ZVAL_EMPTY_ARRAY(params);
	}

	if (EX_CALL_INFO() & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
		zend_string *name;
		zval *param;
		zend_arg_info *arg_info = &EX(func)->common.arg_info[EX(func)->common.num_args];
		if (ZEND_TYPE_IS_SET(arg_info->type)) {
			SEPARATE_ARRAY(params);
			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(EX(extra_named_params), name, param) {
				if (UNEXPECTED(!zend_verify_variadic_arg_type(EX(func), arg_info, arg_num, param, CACHE_ADDR(opline->extended_value)))) {
					HANDLE_EXCEPTION();
				}
				Z_TRY_ADDREF_P(param);
				zend_hash_add_new(Z_ARRVAL_P(params), name, param);
			} ZEND_HASH_FOREACH_END();
		} else if (zend_hash_num_elements(Z_ARRVAL_P(params)) == 0) {
			GC_ADDREF(EX(extra_named_params));
			ZVAL_ARR(params, EX(extra_named_params));
		} else {
			SEPARATE_ARRAY(params);
			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(EX(extra_named_params), name, param) {
				Z_TRY_ADDREF_P(param);
				zend_hash_add_new(Z_ARRVAL_P(params), name, param);
			} ZEND_HASH_FOREACH_END();
		}
	}

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(52, ZEND_BOOL, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		/* The result and op1 can be the same cv zval */
		const uint32_t orig_val_type = Z_TYPE_INFO_P(val);
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(orig_val_type == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	} else {
		SAVE_OPLINE();
		ZVAL_BOOL(EX_VAR(opline->result.var), i_zend_is_true(val));
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_case_helper, ANY, ANY, zval *op_1, zval *op_2)
{
	int ret;
	USE_OPLINE

	SAVE_OPLINE();
	if (UNEXPECTED(Z_TYPE_INFO_P(op_1) == IS_UNDEF)) {
		op_1 = ZVAL_UNDEFINED_OP1();
	}
	if (UNEXPECTED(Z_TYPE_INFO_P(op_2) == IS_UNDEF)) {
		op_2 = ZVAL_UNDEFINED_OP2();
	}
	ret = zend_compare(op_1, op_2);
	if (OP2_TYPE & (IS_TMP_VAR|IS_VAR)) {
		zval_ptr_dtor_nogc(op_2);
	}
	ZEND_VM_SMART_BRANCH(ret == 0, 1);
}

ZEND_VM_HANDLER(48, ZEND_CASE, TMPVAR, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *op1, *op2;
	double d1, d2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			if (EXPECTED(Z_LVAL_P(op1) == Z_LVAL_P(op2))) {
ZEND_VM_C_LABEL(case_true):
				ZEND_VM_SMART_BRANCH_TRUE();
			} else {
ZEND_VM_C_LABEL(case_false):
				ZEND_VM_SMART_BRANCH_FALSE();
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = (double)Z_LVAL_P(op1);
			d2 = Z_DVAL_P(op2);
			ZEND_VM_C_GOTO(case_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			d1 = Z_DVAL_P(op1);
			d2 = Z_DVAL_P(op2);
ZEND_VM_C_LABEL(case_double):
			if (d1 == d2) {
				ZEND_VM_C_GOTO(case_true);
			} else {
				ZEND_VM_C_GOTO(case_false);
			}
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			d1 = Z_DVAL_P(op1);
			d2 = (double)Z_LVAL_P(op2);
			ZEND_VM_C_GOTO(case_double);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
			bool result = zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
			FREE_OP2();
			if (result) {
				ZEND_VM_C_GOTO(case_true);
			} else {
				ZEND_VM_C_GOTO(case_false);
			}
		}
	}
	ZEND_VM_DISPATCH_TO_HELPER(zend_case_helper, op_1, op1, op_2, op2);
}

ZEND_VM_HANDLER(68, ZEND_NEW, UNUSED|CLASS_FETCH|CONST|VAR, UNUSED|CACHE_SLOT, NUM)
{
	USE_OPLINE
	zval *result;
	zend_function *constructor;
	zend_class_entry *ce;
	zend_execute_data *call;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CONST) {
		ce = CACHED_PTR(opline->op2.num);
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
			CACHE_PTR(opline->op2.num, ce);
		}
	} else if (OP1_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op1.num);
		if (UNEXPECTED(ce == NULL)) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		}
	} else {
		ce = Z_CE_P(EX_VAR(opline->op1.var));
	}

	result = EX_VAR(opline->result.var);
	if (UNEXPECTED(object_init_ex(result, ce) != SUCCESS)) {
		ZVAL_UNDEF(result);
		HANDLE_EXCEPTION();
	}

	constructor = Z_OBJ_HT_P(result)->get_constructor(Z_OBJ_P(result));
	if (constructor == NULL) {
		if (UNEXPECTED(EG(exception))) {
			HANDLE_EXCEPTION();
		}

		/* If there are no arguments, skip over the DO_FCALL opcode. We check if the next
		 * opcode is DO_FCALL in case EXT instructions are used. */
		if (EXPECTED(opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL)) {
			ZEND_VM_NEXT_OPCODE_EX(1, 2);
		}

		/* Perform a dummy function call */
		call = zend_vm_stack_push_call_frame(
			ZEND_CALL_FUNCTION, (zend_function *) &zend_pass_function,
			opline->extended_value, NULL);
	} else {
		if (EXPECTED(constructor->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&constructor->op_array))) {
			init_func_run_time_cache(&constructor->op_array);
		}
		/* We are not handling overloaded classes right now */
		call = zend_vm_stack_push_call_frame(
			ZEND_CALL_FUNCTION | ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_THIS,
			constructor,
			opline->extended_value,
			Z_OBJ_P(result));
		Z_ADDREF_P(result);
	}

	call->prev_execute_data = EX(call);
	EX(call) = call;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(110, ZEND_CLONE, CONST|TMPVAR|UNUSED|THIS|CV, ANY)
{
	USE_OPLINE
	zval *obj;
	zend_object *zobj;
	zend_class_entry *ce, *scope;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	SAVE_OPLINE();
	obj = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	do {
		if (OP1_TYPE == IS_CONST ||
		    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(obj) != IS_OBJECT))) {
			if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(obj)) {
				obj = Z_REFVAL_P(obj);
				if (EXPECTED(Z_TYPE_P(obj) == IS_OBJECT)) {
					break;
				}
			}
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(obj) == IS_UNDEF)) {
				ZVAL_UNDEFINED_OP1();
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "__clone method called on non-object");
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} while (0);

	zobj = Z_OBJ_P(obj);
	ce = zobj->ce;
	clone = ce->clone;
	clone_call = zobj->handlers->clone_obj;
	if (UNEXPECTED(clone_call == NULL)) {
		zend_throw_error(NULL, "Trying to clone an uncloneable object of class %s", ZSTR_VAL(ce->name));
		FREE_OP1();
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		HANDLE_EXCEPTION();
	}

	if (clone && !(clone->common.fn_flags & ZEND_ACC_PUBLIC)) {
		scope = EX(func)->op_array.scope;
		if (clone->common.scope != scope) {
			if (UNEXPECTED(clone->common.fn_flags & ZEND_ACC_PRIVATE)
			 || UNEXPECTED(!zend_check_protected(zend_get_function_root_class(clone), scope))) {
				zend_wrong_clone_call(clone, scope);
				FREE_OP1();
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		}
	}

	ZVAL_OBJ(EX_VAR(opline->result.var), clone_call(zobj));

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(99, ZEND_FETCH_CONSTANT, UNUSED|CONST_FETCH, CONST, CACHE_SLOT)
{
	USE_OPLINE
	zend_constant *c;

	c = CACHED_PTR(opline->extended_value);
	if (EXPECTED(c != NULL) && EXPECTED(!IS_SPECIAL_CACHE_VAL(c))) {
		ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value);
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	zend_quick_get_constant(RT_CONSTANT(opline, opline->op2) + 1, opline->op1.num OPLINE_CC EXECUTE_DATA_CC);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(181, ZEND_FETCH_CLASS_CONSTANT, VAR|CONST|UNUSED|CLASS_FETCH, CONST|TMPVARCV, CACHE_SLOT)
{
	zend_class_entry *ce, *scope;
	zend_class_constant *c;
	zval *value, *zv, *constant_zv;
	zend_string *constant_name;
	USE_OPLINE

	SAVE_OPLINE();

	do {
		if (OP1_TYPE == IS_CONST && OP2_TYPE == IS_CONST) {
			if (EXPECTED(CACHED_PTR(opline->extended_value + sizeof(void*)))) {
				value = CACHED_PTR(opline->extended_value + sizeof(void*));
				break;
			}
		}
		if (OP1_TYPE == IS_CONST) {
			if (EXPECTED(CACHED_PTR(opline->extended_value))) {
				ce = CACHED_PTR(opline->extended_value);
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), Z_STR_P(RT_CONSTANT(opline, opline->op1) + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
				if (UNEXPECTED(ce == NULL)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
				CACHE_PTR(opline->extended_value, ce);
			}
		} else if (OP1_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op1.num);
			if (UNEXPECTED(ce == NULL)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op1.var));
		}
		if (OP1_TYPE != IS_CONST
			&& OP2_TYPE == IS_CONST
			&& EXPECTED(CACHED_PTR(opline->extended_value) == ce)) {
			value = CACHED_PTR(opline->extended_value + sizeof(void*));
			break;
		}

		constant_zv = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
		if (UNEXPECTED(Z_TYPE_P(constant_zv) != IS_STRING)) {
			zend_invalid_class_constant_type_error(Z_TYPE_P(constant_zv));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		constant_name = Z_STR_P(constant_zv);
		/* Magic 'class' for constant OP2 is caught at compile-time */
		if (OP2_TYPE != IS_CONST && UNEXPECTED(zend_string_equals_literal_ci(constant_name, "class"))) {
			ZVAL_STR_COPY(EX_VAR(opline->result.var), ce->name);
			FREE_OP2();
			ZEND_VM_NEXT_OPCODE();
		}
		zv = OP2_TYPE == IS_CONST
			? zend_hash_find_known_hash(CE_CONSTANTS_TABLE(ce), constant_name)
			: zend_hash_find(CE_CONSTANTS_TABLE(ce), constant_name);

		if (EXPECTED(zv != NULL)) {
			c = Z_PTR_P(zv);
			scope = EX(func)->op_array.scope;
			if (!zend_verify_const_access(c, scope)) {
				zend_throw_error(NULL, "Cannot access %s constant %s::%s", zend_visibility_string(ZEND_CLASS_CONST_FLAGS(c)), ZSTR_VAL(ce->name), ZSTR_VAL(constant_name));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				FREE_OP2();
				HANDLE_EXCEPTION();
			}

			if (ce->ce_flags & ZEND_ACC_TRAIT) {
				zend_throw_error(NULL, "Cannot access trait constant %s::%s directly", ZSTR_VAL(ce->name), ZSTR_VAL(constant_name));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				FREE_OP2();
				HANDLE_EXCEPTION();
			}

			bool is_constant_deprecated = ZEND_CLASS_CONST_FLAGS(c) & ZEND_ACC_DEPRECATED;
			if (UNEXPECTED(is_constant_deprecated)) {
				zend_error(E_DEPRECATED, "Constant %s::%s is deprecated", ZSTR_VAL(ce->name), ZSTR_VAL(constant_name));

				if (EG(exception)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
			}

			value = &c->value;
			// Enums require loading of all class constants to build the backed enum table
			if (ce->ce_flags & ZEND_ACC_ENUM && ce->enum_backing_type != IS_UNDEF && ce->type == ZEND_USER_CLASS && !(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
				if (UNEXPECTED(zend_update_class_constants(ce) == FAILURE)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
			}
			if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
				if (UNEXPECTED(zend_update_class_constant(c, constant_name, c->ce) != SUCCESS)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
			}
			if (OP2_TYPE == IS_CONST && !is_constant_deprecated) {
				CACHE_POLYMORPHIC_PTR(opline->extended_value, ce, value);
			}
		} else {
			zend_throw_error(NULL, "Undefined constant %s::%s",
				ZSTR_VAL(ce->name), ZSTR_VAL(constant_name));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
	} while (0);

	ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), value);

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(72, ZEND_ADD_ARRAY_ELEMENT, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, REF)
{
	USE_OPLINE
	zval *expr_ptr, new_expr;

	SAVE_OPLINE();
	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) &&
	    UNEXPECTED(opline->extended_value & ZEND_ARRAY_ELEMENT_REF)) {
		expr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		if (Z_ISREF_P(expr_ptr)) {
			Z_ADDREF_P(expr_ptr);
		} else {
			ZVAL_MAKE_REF_EX(expr_ptr, 2);
		}
		FREE_OP1();
	} else {
		expr_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (OP1_TYPE == IS_TMP_VAR) {
			/* pass */
		} else if (OP1_TYPE == IS_CONST) {
			Z_TRY_ADDREF_P(expr_ptr);
		} else if (OP1_TYPE == IS_CV) {
			ZVAL_DEREF(expr_ptr);
			Z_TRY_ADDREF_P(expr_ptr);
		} else /* if (OP1_TYPE == IS_VAR) */ {
			if (UNEXPECTED(Z_ISREF_P(expr_ptr))) {
				zend_refcounted *ref = Z_COUNTED_P(expr_ptr);

				expr_ptr = Z_REFVAL_P(expr_ptr);
				if (UNEXPECTED(GC_DELREF(ref) == 0)) {
					ZVAL_COPY_VALUE(&new_expr, expr_ptr);
					expr_ptr = &new_expr;
					efree_size(ref, sizeof(zend_reference));
				} else if (Z_OPT_REFCOUNTED_P(expr_ptr)) {
					Z_ADDREF_P(expr_ptr);
				}
			}
		}
	}

	if (OP2_TYPE != IS_UNUSED) {
		zval *offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		zend_string *str;
		zend_ulong hval;

ZEND_VM_C_LABEL(add_again):
		if (EXPECTED(Z_TYPE_P(offset) == IS_STRING)) {
			str = Z_STR_P(offset);
			if (OP2_TYPE != IS_CONST) {
				if (ZEND_HANDLE_NUMERIC(str, hval)) {
					ZEND_VM_C_GOTO(num_index);
				}
			}
ZEND_VM_C_LABEL(str_index):
			zend_hash_update(Z_ARRVAL_P(EX_VAR(opline->result.var)), str, expr_ptr);
		} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index):
			zend_hash_index_update(Z_ARRVAL_P(EX_VAR(opline->result.var)), hval, expr_ptr);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_TYPE_P(offset) == IS_REFERENCE)) {
			offset = Z_REFVAL_P(offset);
			ZEND_VM_C_GOTO(add_again);
		} else if (Z_TYPE_P(offset) == IS_NULL) {
			str = ZSTR_EMPTY_ALLOC();
			ZEND_VM_C_GOTO(str_index);
		} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
			hval = zend_dval_to_lval_safe(Z_DVAL_P(offset));
			ZEND_VM_C_GOTO(num_index);
		} else if (Z_TYPE_P(offset) == IS_FALSE) {
			hval = 0;
			ZEND_VM_C_GOTO(num_index);
		} else if (Z_TYPE_P(offset) == IS_TRUE) {
			hval = 1;
			ZEND_VM_C_GOTO(num_index);
		} else if (Z_TYPE_P(offset) == IS_RESOURCE) {
			zend_use_resource_as_offset(offset);
			hval = Z_RES_HANDLE_P(offset);
			ZEND_VM_C_GOTO(num_index);
		} else if (OP2_TYPE == IS_CV && Z_TYPE_P(offset) == IS_UNDEF) {
			ZVAL_UNDEFINED_OP2();
			str = ZSTR_EMPTY_ALLOC();
			ZEND_VM_C_GOTO(str_index);
		} else {
			zend_illegal_array_offset_access(offset);
			zval_ptr_dtor_nogc(expr_ptr);
		}
		FREE_OP2();
	} else {
		if (!zend_hash_next_index_insert(Z_ARRVAL_P(EX_VAR(opline->result.var)), expr_ptr)) {
			zend_cannot_add_element();
			zval_ptr_dtor_nogc(expr_ptr);
		}
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(147, ZEND_ADD_ARRAY_UNPACK, ANY, ANY)
{
	USE_OPLINE
	zval *op1;
	HashTable *result_ht;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	result_ht = Z_ARRVAL_P(EX_VAR(opline->result.var));

ZEND_VM_C_LABEL(add_unpack_again):
	if (EXPECTED(Z_TYPE_P(op1) == IS_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(op1);
		zval *val;

		if (HT_IS_PACKED(ht) && (zend_hash_num_elements(result_ht) == 0 || HT_IS_PACKED(result_ht))) {
			zend_hash_extend(result_ht, result_ht->nNumUsed + zend_hash_num_elements(ht), 1);
			ZEND_HASH_FILL_PACKED(result_ht) {
				ZEND_HASH_PACKED_FOREACH_VAL(ht, val) {
					if (UNEXPECTED(Z_ISREF_P(val)) &&
						UNEXPECTED(Z_REFCOUNT_P(val) == 1)) {
						val = Z_REFVAL_P(val);
					}
					Z_TRY_ADDREF_P(val);
					ZEND_HASH_FILL_ADD(val);
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FILL_END();
		} else {
			zend_string *key;

			ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, val) {
				if (UNEXPECTED(Z_ISREF_P(val)) &&
					UNEXPECTED(Z_REFCOUNT_P(val) == 1)) {
					val = Z_REFVAL_P(val);
				}
				Z_TRY_ADDREF_P(val);
				if (key) {
					zend_hash_update(result_ht, key, val);
				} else {
					if (!zend_hash_next_index_insert(result_ht, val)) {
						zend_cannot_add_element();
						zval_ptr_dtor_nogc(val);
						break;
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_OBJECT)) {
		zend_class_entry *ce = Z_OBJCE_P(op1);
		zend_object_iterator *iter;

		if (!ce || !ce->get_iterator) {
			zend_type_error("Only arrays and Traversables can be unpacked");
		} else {
			iter = ce->get_iterator(ce, op1, 0);
			if (UNEXPECTED(!iter)) {
				FREE_OP1();
				if (!EG(exception)) {
					zend_throw_exception_ex(
						NULL, 0, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name)
					);
				}
				HANDLE_EXCEPTION();
			}

			const zend_object_iterator_funcs *funcs = iter->funcs;
			if (funcs->rewind) {
				funcs->rewind(iter);
			}

			for (; funcs->valid(iter) == SUCCESS; ) {
				zval *val;

				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				val = funcs->get_current_data(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				zval key;
				if (funcs->get_current_key) {
					funcs->get_current_key(iter, &key);
					if (UNEXPECTED(EG(exception) != NULL)) {
						break;
					}

					if (UNEXPECTED(Z_TYPE(key) != IS_LONG && Z_TYPE(key) != IS_STRING)) {
						zend_throw_error(NULL,
							"Keys must be of type int|string during array unpacking");
						zval_ptr_dtor(&key);
						break;
					}
				} else {
					ZVAL_UNDEF(&key);
				}

				ZVAL_DEREF(val);
				Z_TRY_ADDREF_P(val);

				zend_ulong num_key;
				if (Z_TYPE(key) == IS_STRING && !ZEND_HANDLE_NUMERIC(Z_STR(key), num_key)) {
					zend_hash_update(result_ht, Z_STR(key), val);
					zval_ptr_dtor_str(&key);
				} else {
					zval_ptr_dtor(&key);
					if (!zend_hash_next_index_insert(result_ht, val)) {
						zend_cannot_add_element();
						zval_ptr_dtor_nogc(val);
						break;
					}
				}

				funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception))) {
					break;
				}
			}

			zend_iterator_dtor(iter);
		}
	} else if (EXPECTED(Z_ISREF_P(op1))) {
		op1 = Z_REFVAL_P(op1);
		ZEND_VM_C_GOTO(add_unpack_again);
	} else {
		zend_throw_error(NULL, "Only arrays and Traversables can be unpacked");
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|CV|UNUSED, CONST|TMPVAR|UNUSED|NEXT|CV, ARRAY_INIT|REF)
{
	zval *array;
	uint32_t size;
	USE_OPLINE

	array = EX_VAR(opline->result.var);
	if (OP1_TYPE != IS_UNUSED) {
		size = opline->extended_value >> ZEND_ARRAY_SIZE_SHIFT;
		ZVAL_ARR(array, zend_new_array(size));
		/* Explicitly initialize array as not-packed if flag is set */
		if (opline->extended_value & ZEND_ARRAY_NOT_PACKED) {
			zend_hash_real_init_mixed(Z_ARRVAL_P(array));
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ADD_ARRAY_ELEMENT);
	} else {
		ZVAL_ARR(array, zend_new_array(0));
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_COLD_CONST_HANDLER(51, ZEND_CAST, CONST|TMP|VAR|CV, ANY, TYPE)
{
	USE_OPLINE
	zval *expr;
	zval *result = EX_VAR(opline->result.var);
	HashTable *ht;

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	switch (opline->extended_value) {
		case IS_LONG:
			ZVAL_LONG(result, zval_get_long(expr));
			break;
		case IS_DOUBLE:
			ZVAL_DOUBLE(result, zval_get_double(expr));
			break;
		case IS_STRING:
			ZVAL_STR(result, zval_get_string(expr));
			break;
		default:
			ZEND_ASSERT(opline->extended_value != _IS_BOOL && "Must use ZEND_BOOL instead");
			if (OP1_TYPE & (IS_VAR|IS_CV)) {
				ZVAL_DEREF(expr);
			}
			/* If value is already of correct type, return it directly */
			if (Z_TYPE_P(expr) == opline->extended_value) {
				ZVAL_COPY_VALUE(result, expr);
				if (OP1_TYPE == IS_CONST) {
					if (UNEXPECTED(Z_OPT_REFCOUNTED_P(result))) Z_ADDREF_P(result);
				} else if (OP1_TYPE != IS_TMP_VAR) {
					if (Z_OPT_REFCOUNTED_P(result)) Z_ADDREF_P(result);
				}

				FREE_OP1_IF_VAR();
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}

			if (opline->extended_value == IS_ARRAY) {
				if (OP1_TYPE == IS_CONST || Z_TYPE_P(expr) != IS_OBJECT || Z_OBJCE_P(expr) == zend_ce_closure) {
					if (Z_TYPE_P(expr) != IS_NULL) {
						ZVAL_ARR(result, zend_new_array(1));
						expr = zend_hash_index_add_new(Z_ARRVAL_P(result), 0, expr);
						if (OP1_TYPE == IS_CONST) {
							if (UNEXPECTED(Z_OPT_REFCOUNTED_P(expr))) Z_ADDREF_P(expr);
						} else {
							if (Z_OPT_REFCOUNTED_P(expr)) Z_ADDREF_P(expr);
						}
					} else {
						ZVAL_EMPTY_ARRAY(result);
					}
				} else if (Z_OBJ_P(expr)->properties == NULL
				 && Z_OBJ_HT_P(expr)->get_properties_for == NULL
				 && Z_OBJ_HT_P(expr)->get_properties == zend_std_get_properties) {
					/* Optimized version without rebuilding properties HashTable */
					ZVAL_ARR(result, zend_std_build_object_properties_array(Z_OBJ_P(expr)));
				} else {
					HashTable *obj_ht = zend_get_properties_for(expr, ZEND_PROP_PURPOSE_ARRAY_CAST);
					if (obj_ht) {
						/* fast copy */
						ZVAL_ARR(result, zend_proptable_to_symtable(obj_ht,
							(Z_OBJCE_P(expr)->default_properties_count ||
							 Z_OBJ_P(expr)->handlers != &std_object_handlers ||
							 GC_IS_RECURSIVE(obj_ht))));
						zend_release_properties(obj_ht);
					} else {
						ZVAL_EMPTY_ARRAY(result);
					}
				}
			} else {
				ZEND_ASSERT(opline->extended_value == IS_OBJECT);
				ZVAL_OBJ(result, zend_objects_new(zend_standard_class_def));
				if (Z_TYPE_P(expr) == IS_ARRAY) {
					ht = zend_symtable_to_proptable(Z_ARR_P(expr));
					if (GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) {
						/* TODO: try not to duplicate immutable arrays as well ??? */
						ht = zend_array_dup(ht);
					}
					Z_OBJ_P(result)->properties = ht;
				} else if (Z_TYPE_P(expr) != IS_NULL) {
					Z_OBJ_P(result)->properties = ht = zend_new_array(1);
					expr = zend_hash_add_new(ht, ZSTR_KNOWN(ZEND_STR_SCALAR), expr);
					if (OP1_TYPE == IS_CONST) {
						if (UNEXPECTED(Z_OPT_REFCOUNTED_P(expr))) Z_ADDREF_P(expr);
					} else {
						if (Z_OPT_REFCOUNTED_P(expr)) Z_ADDREF_P(expr);
					}
				}
			}
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(73, ZEND_INCLUDE_OR_EVAL, CONST|TMPVAR|CV, ANY, EVAL, SPEC(OBSERVER))
{
	USE_OPLINE
	zend_op_array *new_op_array;
	zval *inc_filename;

	SAVE_OPLINE();
	inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);
	new_op_array = zend_include_or_eval(inc_filename, opline->extended_value);
	if (UNEXPECTED(EG(exception) != NULL)) {
		FREE_OP1();
		if (new_op_array != ZEND_FAKE_OP_ARRAY && new_op_array != NULL) {
			destroy_op_array(new_op_array);
			efree_size(new_op_array, sizeof(zend_op_array));
		}
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	} else if (new_op_array == ZEND_FAKE_OP_ARRAY) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_TRUE(EX_VAR(opline->result.var));
		}
	} else if (UNEXPECTED(new_op_array == NULL)) {
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_FALSE(EX_VAR(opline->result.var));
		}
	} else if (new_op_array->last == 1
			&& new_op_array->opcodes[0].opcode == ZEND_RETURN
			&& new_op_array->opcodes[0].op1_type == IS_CONST
			&& EXPECTED(zend_execute_ex == execute_ex)) {
		if (RETURN_VALUE_USED(opline)) {
			const zend_op *op = new_op_array->opcodes;

			ZVAL_COPY(EX_VAR(opline->result.var), RT_CONSTANT(op, op->op1));
		}
		zend_destroy_static_vars(new_op_array);
		destroy_op_array(new_op_array);
		efree_size(new_op_array, sizeof(zend_op_array));
	} else {
		zval *return_value = NULL;
		zend_execute_data *call;
		if (RETURN_VALUE_USED(opline)) {
			return_value = EX_VAR(opline->result.var);
		}

		new_op_array->scope = EX(func)->op_array.scope;

		call = zend_vm_stack_push_call_frame(
			(Z_TYPE_INFO(EX(This)) & ZEND_CALL_HAS_THIS) | ZEND_CALL_NESTED_CODE | ZEND_CALL_HAS_SYMBOL_TABLE,
			(zend_function*)new_op_array, 0,
			Z_PTR(EX(This)));

		if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
			call->symbol_table = EX(symbol_table);
		} else {
			call->symbol_table = zend_rebuild_symbol_table();
		}

		call->prev_execute_data = execute_data;
		i_init_code_execute_data(call, new_op_array, return_value);
		ZEND_OBSERVER_FCALL_BEGIN(call);
		if (EXPECTED(zend_execute_ex == execute_ex)) {
			FREE_OP1();
			ZEND_VM_ENTER();
		} else {
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
			zend_vm_stack_free_call_frame(call);
		}

		zend_destroy_static_vars(new_op_array);
		destroy_op_array(new_op_array);
		efree_size(new_op_array, sizeof(zend_op_array));
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			FREE_OP1();
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(153, ZEND_UNSET_CV, CV, UNUSED)
{
	USE_OPLINE
	zval *var = EX_VAR(opline->op1.var);

	if (Z_REFCOUNTED_P(var)) {
		zend_refcounted *garbage = Z_COUNTED_P(var);

		ZVAL_UNDEF(var);
		SAVE_OPLINE();
		GC_DTOR(garbage);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		ZVAL_UNDEF(var);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMPVAR|CV, UNUSED, VAR_FETCH)
{
	USE_OPLINE
	zval *varname;
	zend_string *name, *tmp_name;
	HashTable *target_symbol_table;

	SAVE_OPLINE();

	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
		tmp_name = NULL;
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = ZVAL_UNDEFINED_OP1();
		}
		name = zval_try_get_tmp_string(varname, &tmp_name);
		if (UNEXPECTED(!name)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	}

	target_symbol_table = zend_get_target_symbol_table(opline->extended_value EXECUTE_DATA_CC);
	zend_hash_del_ind(target_symbol_table, name);

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_COLD_HANDLER(179, ZEND_UNSET_STATIC_PROP, ANY, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zval *varname;
	zend_string *name, *tmp_name = NULL;
	zend_class_entry *ce;

	SAVE_OPLINE();

	if (OP2_TYPE == IS_CONST) {
		ce = CACHED_PTR(opline->extended_value);
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)), Z_STR_P(RT_CONSTANT(opline, opline->op2) + 1), ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
			/*CACHE_PTR(opline->extended_value, ce);*/
		}
	} else if (OP2_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op2.num);
		if (UNEXPECTED(ce == NULL)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		ce = Z_CE_P(EX_VAR(opline->op2.var));
	}

	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = ZVAL_UNDEFINED_OP1();
		}
		name = zval_try_get_tmp_string(varname, &tmp_name);
		if (UNEXPECTED(!name)) {
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	}

	zend_std_unset_static_property(ce, name);

	zend_tmp_string_release(tmp_name);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(75, ZEND_UNSET_DIM, VAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zval *container;
	zval *offset;
	zend_ulong hval;
	zend_string *key;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);
	offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	do {
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			HashTable *ht;

ZEND_VM_C_LABEL(unset_dim_array):
			SEPARATE_ARRAY(container);
			ht = Z_ARRVAL_P(container);
ZEND_VM_C_LABEL(offset_again):
			if (EXPECTED(Z_TYPE_P(offset) == IS_STRING)) {
				key = Z_STR_P(offset);
				if (OP2_TYPE != IS_CONST) {
					if (ZEND_HANDLE_NUMERIC(key, hval)) {
						ZEND_VM_C_GOTO(num_index_dim);
					}
				}
ZEND_VM_C_LABEL(str_index_dim):
				ZEND_ASSERT(ht != &EG(symbol_table));
				zend_hash_del(ht, key);
			} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
				hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_dim):
				zend_hash_index_del(ht, hval);
			} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_TYPE_P(offset) == IS_REFERENCE)) {
				offset = Z_REFVAL_P(offset);
				ZEND_VM_C_GOTO(offset_again);
			} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
				hval = zend_dval_to_lval_safe(Z_DVAL_P(offset));
				ZEND_VM_C_GOTO(num_index_dim);
			} else if (Z_TYPE_P(offset) == IS_NULL) {
				key = ZSTR_EMPTY_ALLOC();
				ZEND_VM_C_GOTO(str_index_dim);
			} else if (Z_TYPE_P(offset) == IS_FALSE) {
				hval = 0;
				ZEND_VM_C_GOTO(num_index_dim);
			} else if (Z_TYPE_P(offset) == IS_TRUE) {
				hval = 1;
				ZEND_VM_C_GOTO(num_index_dim);
			} else if (Z_TYPE_P(offset) == IS_RESOURCE) {
				zend_use_resource_as_offset(offset);
				hval = Z_RES_HANDLE_P(offset);
				ZEND_VM_C_GOTO(num_index_dim);
			} else if (OP2_TYPE == IS_CV && Z_TYPE_P(offset) == IS_UNDEF) {
				ZVAL_UNDEFINED_OP2();
				key = ZSTR_EMPTY_ALLOC();
				ZEND_VM_C_GOTO(str_index_dim);
			} else {
				zend_illegal_array_offset_unset(offset);
			}
			break;
		} else if (Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(unset_dim_array);
			}
		}
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			container = ZVAL_UNDEFINED_OP1();
		}
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
			offset = ZVAL_UNDEFINED_OP2();
		}
		if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
			if (OP2_TYPE == IS_CONST && Z_EXTRA_P(offset) == ZEND_EXTRA_VALUE) {
				offset++;
			}
			Z_OBJ_HT_P(container)->unset_dimension(Z_OBJ_P(container), offset);
		} else if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
			zend_throw_error(NULL, "Cannot unset string offsets");
		} else if (UNEXPECTED(Z_TYPE_P(container) > IS_FALSE)) {
			zend_throw_error(NULL, "Cannot unset offset in a non-array variable");
		} else if (UNEXPECTED(Z_TYPE_P(container) == IS_FALSE)) {
			zend_false_to_array_deprecated();
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zval *container;
	zval *offset;
	zend_string *name, *tmp_name;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
			if (Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (Z_TYPE_P(container) != IS_OBJECT) {
					if (OP1_TYPE == IS_CV
					 && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
						ZVAL_UNDEFINED_OP1();
					}
					break;
				}
			} else {
				break;
			}
		}
		if (OP2_TYPE == IS_CONST) {
			name = Z_STR_P(offset);
		} else {
			name = zval_try_get_tmp_string(offset, &tmp_name);
			if (UNEXPECTED(!name)) {
				break;
			}
		}
		Z_OBJ_HT_P(container)->unset_property(Z_OBJ_P(container), name, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL));
		if (OP2_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(77, ZEND_FE_RESET_R, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *array_ptr, *result;

	SAVE_OPLINE();

	array_ptr = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(array_ptr) == IS_ARRAY)) {
		result = EX_VAR(opline->result.var);
		ZVAL_COPY_VALUE(result, array_ptr);
		if (OP1_TYPE != IS_TMP_VAR && Z_OPT_REFCOUNTED_P(result)) {
			Z_ADDREF_P(array_ptr);
		}
		Z_FE_POS_P(result) = 0;

		FREE_OP1_IF_VAR();
		ZEND_VM_NEXT_OPCODE();
	} else if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(array_ptr) == IS_OBJECT)) {
		zend_object *zobj = Z_OBJ_P(array_ptr);
		if (!zobj->ce->get_iterator) {
			HashTable *properties = zobj->properties;
			if (properties) {
				if (UNEXPECTED(GC_REFCOUNT(properties) > 1)) {
					if (EXPECTED(!(GC_FLAGS(properties) & IS_ARRAY_IMMUTABLE))) {
						GC_DELREF(properties);
					}
					properties = zobj->properties = zend_array_dup(properties);
				}
			} else {
				properties = zobj->handlers->get_properties(zobj);
			}

			result = EX_VAR(opline->result.var);
			ZVAL_COPY_VALUE(result, array_ptr);
			if (OP1_TYPE != IS_TMP_VAR) {
				Z_ADDREF_P(array_ptr);
			}

			if (zend_hash_num_elements(properties) == 0) {
				Z_FE_ITER_P(result) = (uint32_t) -1;
				FREE_OP1_IF_VAR();
				ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
			}

			Z_FE_ITER_P(result) = zend_hash_iterator_add(properties, 0);
			FREE_OP1_IF_VAR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			bool is_empty = zend_fe_reset_iterator(array_ptr, 0 OPLINE_CC EXECUTE_DATA_CC);

			FREE_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			} else if (is_empty) {
				ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
			} else {
				ZEND_VM_NEXT_OPCODE();
			}
		}
	} else {
		zend_error(E_WARNING, "foreach() argument must be of type array|object, %s given", zend_zval_value_name(array_ptr));
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
		FREE_OP1();
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}
}

ZEND_VM_COLD_CONST_HANDLER(125, ZEND_FE_RESET_RW, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *array_ptr, *array_ref;

	SAVE_OPLINE();

	if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
		array_ref = array_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_R);
		if (Z_ISREF_P(array_ref)) {
			array_ptr = Z_REFVAL_P(array_ref);
		}
	} else {
		array_ref = array_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	}

	if (EXPECTED(Z_TYPE_P(array_ptr) == IS_ARRAY)) {
		if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
			if (array_ptr == array_ref) {
				ZVAL_NEW_REF(array_ref, array_ref);
				array_ptr = Z_REFVAL_P(array_ref);
			}
			Z_ADDREF_P(array_ref);
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), array_ref);
		} else {
			array_ref = EX_VAR(opline->result.var);
			ZVAL_NEW_REF(array_ref, array_ptr);
			array_ptr = Z_REFVAL_P(array_ref);
		}
		if (OP1_TYPE == IS_CONST) {
			ZVAL_ARR(array_ptr, zend_array_dup(Z_ARRVAL_P(array_ptr)));
		} else {
			SEPARATE_ARRAY(array_ptr);
		}
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(Z_ARRVAL_P(array_ptr), 0);

		FREE_OP1_IF_VAR();
		ZEND_VM_NEXT_OPCODE();
	} else if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(array_ptr) == IS_OBJECT)) {
		if (!Z_OBJCE_P(array_ptr)->get_iterator) {
			HashTable *properties;
			if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
				if (array_ptr == array_ref) {
					ZVAL_NEW_REF(array_ref, array_ref);
					array_ptr = Z_REFVAL_P(array_ref);
				}
				Z_ADDREF_P(array_ref);
				ZVAL_COPY_VALUE(EX_VAR(opline->result.var), array_ref);
			} else {
				array_ptr = EX_VAR(opline->result.var);
				ZVAL_COPY_VALUE(array_ptr, array_ref);
			}
			if (Z_OBJ_P(array_ptr)->properties
			 && UNEXPECTED(GC_REFCOUNT(Z_OBJ_P(array_ptr)->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(Z_OBJ_P(array_ptr)->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(Z_OBJ_P(array_ptr)->properties);
				}
				Z_OBJ_P(array_ptr)->properties = zend_array_dup(Z_OBJ_P(array_ptr)->properties);
			}

			properties = Z_OBJPROP_P(array_ptr);
			if (zend_hash_num_elements(properties) == 0) {
				Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t) -1;
				FREE_OP1_IF_VAR();
				ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
			}

			Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(properties, 0);
			FREE_OP1_IF_VAR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			bool is_empty = zend_fe_reset_iterator(array_ptr, 1 OPLINE_CC EXECUTE_DATA_CC);
			FREE_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			} else if (is_empty) {
				ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
			} else {
				ZEND_VM_NEXT_OPCODE();
			}
		}
	} else {
		zend_error(E_WARNING, "foreach() argument must be of type array|object, %s given", zend_zval_value_name(array_ptr));
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
		FREE_OP1();
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}
}

ZEND_VM_HELPER(zend_fe_fetch_object_helper, ANY, ANY)
{
	USE_OPLINE
	zval *array;
	zval *value;
	uint32_t value_type;
	HashTable *fe_ht;
	HashPosition pos;
	Bucket *p;
	zend_object_iterator *iter;

	array = EX_VAR(opline->op1.var);
	SAVE_OPLINE();

	ZEND_ASSERT(Z_TYPE_P(array) == IS_OBJECT);
	if ((iter = zend_iterator_unwrap(array)) == NULL) {
		/* plain object */

		fe_ht = Z_OBJPROP_P(array);
		pos = zend_hash_iterator_pos(Z_FE_ITER_P(array), fe_ht);
		p = fe_ht->arData + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_C_GOTO(fe_fetch_r_exit);
			}
			pos++;
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			if (EXPECTED(value_type != IS_UNDEF)) {
				if (UNEXPECTED(value_type == IS_INDIRECT)) {
					value = Z_INDIRECT_P(value);
					value_type = Z_TYPE_INFO_P(value);
					if (EXPECTED(value_type != IS_UNDEF)
					 && EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key, 0) == SUCCESS)) {
						break;
					}
				} else if (EXPECTED(Z_OBJCE_P(array)->default_properties_count == 0)
						|| !p->key
						|| zend_check_property_access(Z_OBJ_P(array), p->key, 1) == SUCCESS) {
					break;
				}
			}
			p++;
		}
		EG(ht_iterators)[Z_FE_ITER_P(array)].pos = pos;
		if (RETURN_VALUE_USED(opline)) {
			if (UNEXPECTED(!p->key)) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else if (ZSTR_VAL(p->key)[0]) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			} else {
				const char *class_name, *prop_name;
				size_t prop_name_len;
				zend_unmangle_property_name_ex(
					p->key, &class_name, &prop_name, &prop_name_len);
				ZVAL_STRINGL(EX_VAR(opline->result.var), prop_name, prop_name_len);
			}
		}
	} else {
		const zend_object_iterator_funcs *funcs = iter->funcs;
		if (EXPECTED(++iter->index > 0)) {
			/* This could cause an endless loop if index becomes zero again.
			 * In case that ever happens we need an additional flag. */
			funcs->move_forward(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
			if (UNEXPECTED(funcs->valid(iter) == FAILURE)) {
				/* reached end of iteration */
				if (UNEXPECTED(EG(exception) != NULL)) {
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
ZEND_VM_C_LABEL(fe_fetch_r_exit):
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
		}
		value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception) != NULL)) {
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}
		if (!value) {
			/* failure in get_current_data */
			ZEND_VM_C_GOTO(fe_fetch_r_exit);
		}
		if (RETURN_VALUE_USED(opline)) {
			if (funcs->get_current_key) {
				funcs->get_current_key(iter, EX_VAR(opline->result.var));
				if (UNEXPECTED(EG(exception) != NULL)) {
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
			} else {
				ZVAL_LONG(EX_VAR(opline->result.var), iter->index);
			}
		}
		value_type = Z_TYPE_INFO_P(value);
	}

	if (EXPECTED(OP2_TYPE == IS_CV)) {
		zval *variable_ptr = EX_VAR(opline->op2.var);
		zend_assign_to_variable(variable_ptr, value, IS_CV, EX_USES_STRICT_TYPES());
	} else {
		zval *res = EX_VAR(opline->op2.var);
		zend_refcounted *gc = Z_COUNTED_P(value);

		ZVAL_COPY_VALUE_EX(res, value, gc, value_type);
		if (Z_TYPE_INFO_REFCOUNTED(value_type)) {
			GC_ADDREF(gc);
		}
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(78, ZEND_FE_FETCH_R, VAR, ANY, JMP_ADDR)
{
	USE_OPLINE
	zval *array;
	zval *value;
	uint32_t value_type;
	HashTable *fe_ht;
	HashPosition pos;

	array = EX_VAR(opline->op1.var);
	if (UNEXPECTED(Z_TYPE_P(array) != IS_ARRAY)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fe_fetch_object_helper);
	}
	fe_ht = Z_ARRVAL_P(array);
	pos = Z_FE_POS_P(array);
	if (HT_IS_PACKED(fe_ht)) {
		value = fe_ht->arPacked + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
			value_type = Z_TYPE_INFO_P(value);
			ZEND_ASSERT(value_type != IS_INDIRECT);
			if (EXPECTED(value_type != IS_UNDEF)) {
				break;
			}
			pos++;
			value++;
		}
		Z_FE_POS_P(array) = pos + 1;
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_LONG(EX_VAR(opline->result.var), pos);
		}
	} else {
		Bucket *p;

		p = fe_ht->arData + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
			pos++;
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			ZEND_ASSERT(value_type != IS_INDIRECT);
			if (EXPECTED(value_type != IS_UNDEF)) {
				break;
			}
			p++;
		}
		Z_FE_POS_P(array) = pos;
		if (RETURN_VALUE_USED(opline)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
	}
	if (EXPECTED(OP2_TYPE == IS_CV)) {
		zval *variable_ptr = EX_VAR(opline->op2.var);
		SAVE_OPLINE();
		zend_assign_to_variable(variable_ptr, value, IS_CV, EX_USES_STRICT_TYPES());
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		zval *res = EX_VAR(opline->op2.var);
		zend_refcounted *gc = Z_COUNTED_P(value);

		ZVAL_COPY_VALUE_EX(res, value, gc, value_type);
		if (Z_TYPE_INFO_REFCOUNTED(value_type)) {
			GC_ADDREF(gc);
		}
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(126, ZEND_FE_FETCH_RW, VAR, ANY, JMP_ADDR)
{
	USE_OPLINE
	zval *array;
	zval *value;
	uint32_t value_type;
	HashTable *fe_ht;
	HashPosition pos;
	Bucket *p;

	array = EX_VAR(opline->op1.var);
	SAVE_OPLINE();

	ZVAL_DEREF(array);
	if (EXPECTED(Z_TYPE_P(array) == IS_ARRAY)) {
		pos = zend_hash_iterator_pos_ex(Z_FE_ITER_P(EX_VAR(opline->op1.var)), array);
		fe_ht = Z_ARRVAL_P(array);
		if (HT_IS_PACKED(fe_ht)) {
			value = fe_ht->arPacked + pos;
			while (1) {
				if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
					/* reached end of iteration */
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
				value_type = Z_TYPE_INFO_P(value);
				ZEND_ASSERT(value_type != IS_INDIRECT);
				if (EXPECTED(value_type != IS_UNDEF)) {
					break;
				}
				pos++;
				value++;
			}
			EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos + 1;
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_LONG(EX_VAR(opline->result.var), pos);
			}
		} else {
			p = fe_ht->arData + pos;
			while (1) {
				if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
					/* reached end of iteration */
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
				pos++;
				value = &p->val;
				value_type = Z_TYPE_INFO_P(value);
				ZEND_ASSERT(value_type != IS_INDIRECT);
				if (EXPECTED(value_type != IS_UNDEF)) {
					break;
				}
				p++;
			}
			EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos;
			if (RETURN_VALUE_USED(opline)) {
				if (!p->key) {
					ZVAL_LONG(EX_VAR(opline->result.var), p->h);
				} else {
					ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
				}
			}
		}
	} else if (EXPECTED(Z_TYPE_P(array) == IS_OBJECT)) {
		zend_object_iterator *iter;

		if ((iter = zend_iterator_unwrap(array)) == NULL) {
			/* plain object */

			fe_ht = Z_OBJPROP_P(array);
			pos = zend_hash_iterator_pos(Z_FE_ITER_P(EX_VAR(opline->op1.var)), fe_ht);
			p = fe_ht->arData + pos;
			while (1) {
				if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
					/* reached end of iteration */
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
				pos++;
				value = &p->val;
				value_type = Z_TYPE_INFO_P(value);
				if (EXPECTED(value_type != IS_UNDEF)) {
					if (UNEXPECTED(value_type == IS_INDIRECT)) {
						value = Z_INDIRECT_P(value);
						value_type = Z_TYPE_INFO_P(value);
						if (EXPECTED(value_type != IS_UNDEF)
						 && EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key, 0) == SUCCESS)) {
							if ((value_type & Z_TYPE_MASK) != IS_REFERENCE) {
								zend_property_info *prop_info =
									zend_get_property_info_for_slot(Z_OBJ_P(array), value);
								if (UNEXPECTED(prop_info)) {
									if (UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
										zend_throw_error(NULL,
											"Cannot acquire reference to readonly property %s::$%s",
											ZSTR_VAL(prop_info->ce->name), ZSTR_VAL(p->key));
										UNDEF_RESULT();
										HANDLE_EXCEPTION();
									}
									if (ZEND_TYPE_IS_SET(prop_info->type)) {
										ZVAL_NEW_REF(value, value);
										ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(value), prop_info);
										value_type = IS_REFERENCE_EX;
									}
								}
							}
							break;
						}
					} else if (EXPECTED(Z_OBJCE_P(array)->default_properties_count == 0)
							|| !p->key
							|| zend_check_property_access(Z_OBJ_P(array), p->key, 1) == SUCCESS) {
						break;
					}
				}
				p++;
			}
			EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos;
			if (RETURN_VALUE_USED(opline)) {
				if (UNEXPECTED(!p->key)) {
					ZVAL_LONG(EX_VAR(opline->result.var), p->h);
				} else if (ZSTR_VAL(p->key)[0]) {
					ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
				} else {
					const char *class_name, *prop_name;
					size_t prop_name_len;
					zend_unmangle_property_name_ex(
						p->key, &class_name, &prop_name, &prop_name_len);
					ZVAL_STRINGL(EX_VAR(opline->result.var), prop_name, prop_name_len);
				}
			}
		} else {
			const zend_object_iterator_funcs *funcs = iter->funcs;
			if (++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						UNDEF_RESULT();
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
			}
			value = funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_w_exit);
			}
			if (RETURN_VALUE_USED(opline)) {
				if (funcs->get_current_key) {
					funcs->get_current_key(iter, EX_VAR(opline->result.var));
					if (UNEXPECTED(EG(exception) != NULL)) {
						UNDEF_RESULT();
						HANDLE_EXCEPTION();
					}
				} else {
					ZVAL_LONG(EX_VAR(opline->result.var), iter->index);
				}
			}
			value_type = Z_TYPE_INFO_P(value);
		}
	} else {
		zend_error(E_WARNING, "foreach() argument must be of type array|object, %s given", zend_zval_value_name(array));
		if (UNEXPECTED(EG(exception))) {
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}
ZEND_VM_C_LABEL(fe_fetch_w_exit):
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}

	if (EXPECTED((value_type & Z_TYPE_MASK) != IS_REFERENCE)) {
		zend_refcounted *gc = Z_COUNTED_P(value);
		zval *ref;
		ZVAL_NEW_EMPTY_REF(value);
		ref = Z_REFVAL_P(value);
		ZVAL_COPY_VALUE_EX(ref, value, gc, value_type);
	}
	if (EXPECTED(OP2_TYPE == IS_CV)) {
		zval *variable_ptr = EX_VAR(opline->op2.var);
		if (EXPECTED(variable_ptr != value)) {
			zend_reference *ref;

			ref = Z_REF_P(value);
			GC_ADDREF(ref);
			i_zval_ptr_dtor(variable_ptr);
			ZVAL_REF(variable_ptr, ref);
		}
	} else {
		Z_ADDREF_P(value);
		ZVAL_REF(EX_VAR(opline->op2.var), Z_REF_P(value));
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(154, ZEND_ISSET_ISEMPTY_CV, CV, UNUSED, ISSET, SPEC(ISSET))
{
	USE_OPLINE
	zval *value;

	value = EX_VAR(opline->op1.var);
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		if (Z_TYPE_P(value) > IS_NULL &&
		    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL)) {
			ZEND_VM_SMART_BRANCH_TRUE();
		} else {
			ZEND_VM_SMART_BRANCH_FALSE();
		}
	} else {
		bool result;

		SAVE_OPLINE();
		result = !i_zend_is_true(value);
		ZEND_VM_SMART_BRANCH(result, 1);
	}
}

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMPVAR|CV, UNUSED, VAR_FETCH|ISSET)
{
	USE_OPLINE
	zval *value;
	/* Should be bool result? as below got: result = (opline->extended_value & ZEND_ISEMPTY) */
	int result;
	zval *varname;
	zend_string *name, *tmp_name;
	HashTable *target_symbol_table;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else {
		name = zval_get_tmp_string(varname, &tmp_name);
	}

	target_symbol_table = zend_get_target_symbol_table(opline->extended_value EXECUTE_DATA_CC);
	value = zend_hash_find_ex(target_symbol_table, name, OP1_TYPE == IS_CONST);

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
	FREE_OP1();

	if (!value) {
		result = (opline->extended_value & ZEND_ISEMPTY);
	} else {
		if (Z_TYPE_P(value) == IS_INDIRECT) {
			value = Z_INDIRECT_P(value);
		}
		if (!(opline->extended_value & ZEND_ISEMPTY)) {
			if (Z_ISREF_P(value)) {
				value = Z_REFVAL_P(value);
			}
			result = Z_TYPE_P(value) > IS_NULL;
		} else {
			result = !i_zend_is_true(value);
		}
	}

	ZEND_VM_SMART_BRANCH(result, 1);
}

/* No specialization for op_types (CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR) */
ZEND_VM_HANDLER(180, ZEND_ISSET_ISEMPTY_STATIC_PROP, ANY, CLASS_FETCH, ISSET|CACHE_SLOT)
{
	USE_OPLINE
	zval *value;
	zend_result fetch_result;
	bool result;

	SAVE_OPLINE();

	fetch_result = zend_fetch_static_property_address(&value, NULL, opline->extended_value & ~ZEND_ISEMPTY, BP_VAR_IS, 0 OPLINE_CC EXECUTE_DATA_CC);

	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		result = fetch_result == SUCCESS && Z_TYPE_P(value) > IS_NULL &&
		    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
	} else {
		result = fetch_result != SUCCESS || !i_zend_is_true(value);
	}

	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(115, ZEND_ISSET_ISEMPTY_DIM_OBJ, CONST|TMPVAR|CV, CONST|TMPVAR|CV, ISSET)
{
	USE_OPLINE
	zval *container;
	bool result;
	zend_ulong hval;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_IS);
	offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
		HashTable *ht;
		zval *value;
		zend_string *str;

ZEND_VM_C_LABEL(isset_dim_obj_array):
		ht = Z_ARRVAL_P(container);
ZEND_VM_C_LABEL(isset_again):
		if (EXPECTED(Z_TYPE_P(offset) == IS_STRING)) {
			str = Z_STR_P(offset);
			if (OP2_TYPE != IS_CONST) {
				if (ZEND_HANDLE_NUMERIC(str, hval)) {
					ZEND_VM_C_GOTO(num_index_prop);
				}
			}
			value = zend_hash_find_ex(ht, str, OP2_TYPE == IS_CONST);
		} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_prop):
			value = zend_hash_index_find(ht, hval);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(offset))) {
			offset = Z_REFVAL_P(offset);
			ZEND_VM_C_GOTO(isset_again);
		} else {
			value = zend_find_array_dim_slow(ht, offset EXECUTE_DATA_CC);
			if (UNEXPECTED(EG(exception))) {
				result = 0;
				ZEND_VM_C_GOTO(isset_dim_obj_exit);
			}
		}

		if (!(opline->extended_value & ZEND_ISEMPTY)) {
			/* > IS_NULL means not IS_UNDEF and not IS_NULL */
			result = value != NULL && Z_TYPE_P(value) > IS_NULL &&
			    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);

			if (OP1_TYPE & (IS_CONST|IS_CV)) {
				/* avoid exception check */
				FREE_OP2();
				ZEND_VM_SMART_BRANCH(result, 0);
			}
		} else {
			result = (value == NULL || !i_zend_is_true(value));
		}
		ZEND_VM_C_GOTO(isset_dim_obj_exit);
	} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(container))) {
		container = Z_REFVAL_P(container);
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			ZEND_VM_C_GOTO(isset_dim_obj_array);
		}
	}

	if (OP2_TYPE == IS_CONST && Z_EXTRA_P(offset) == ZEND_EXTRA_VALUE) {
		offset++;
	}
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		result = zend_isset_dim_slow(container, offset EXECUTE_DATA_CC);
	} else {
		result = zend_isempty_dim_slow(container, offset EXECUTE_DATA_CC);
	}

ZEND_VM_C_LABEL(isset_dim_obj_exit):
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_COLD_CONST_HANDLER(148, ZEND_ISSET_ISEMPTY_PROP_OBJ, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, ISSET|CACHE_SLOT)
{
	USE_OPLINE
	zval *container;
	int result;
	zval *offset;
	zend_string *name, *tmp_name;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
				result = (opline->extended_value & ZEND_ISEMPTY);
				ZEND_VM_C_GOTO(isset_object_finish);
			}
		} else {
			result = (opline->extended_value & ZEND_ISEMPTY);
			ZEND_VM_C_GOTO(isset_object_finish);
		}
	}

	if (OP2_TYPE == IS_CONST) {
		name = Z_STR_P(offset);
	} else {
		name = zval_try_get_tmp_string(offset, &tmp_name);
		if (UNEXPECTED(!name)) {
			result = 0;
			ZEND_VM_C_GOTO(isset_object_finish);
		}
	}

	result =
		(opline->extended_value & ZEND_ISEMPTY) ^
		Z_OBJ_HT_P(container)->has_property(Z_OBJ_P(container), name, (opline->extended_value & ZEND_ISEMPTY), ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value & ~ZEND_ISEMPTY) : NULL));

	if (OP2_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}

ZEND_VM_C_LABEL(isset_object_finish):
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_HANDLER(194, ZEND_ARRAY_KEY_EXISTS, CV|TMPVAR|CONST, CV|TMPVAR|CONST)
{
	USE_OPLINE

	zval *key, *subject;
	HashTable *ht;
	bool result;

	SAVE_OPLINE();

	key = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	subject = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (EXPECTED(Z_TYPE_P(subject) == IS_ARRAY)) {
ZEND_VM_C_LABEL(array_key_exists_array):
		ht = Z_ARRVAL_P(subject);
		result = zend_array_key_exists_fast(ht, key OPLINE_CC EXECUTE_DATA_CC);
	} else {
		if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(subject))) {
			subject = Z_REFVAL_P(subject);
			if (EXPECTED(Z_TYPE_P(subject) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(array_key_exists_array);
			}
		}
		zend_array_key_exists_error(subject, key OPLINE_CC EXECUTE_DATA_CC);
		result = 0;
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
}

/* No specialization for op_types (CONST|TMPVAR|UNUSED|CV, ANY) */
ZEND_VM_COLD_HANDLER(79, ZEND_EXIT, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE != IS_UNUSED) {
		zval *ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		do {
			if (Z_TYPE_P(ptr) == IS_LONG) {
				EG(exit_status) = Z_LVAL_P(ptr);
			} else {
				if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(ptr)) {
					ptr = Z_REFVAL_P(ptr);
					if (Z_TYPE_P(ptr) == IS_LONG) {
						EG(exit_status) = Z_LVAL_P(ptr);
						break;
					}
				}
				zend_print_zval(ptr, 0);
			}
		} while (0);
		FREE_OP1();
	}

	if (!EG(exception)) {
		zend_throw_unwind_exit();
	}
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(57, ZEND_BEGIN_SILENCE, ANY, ANY)
{
	USE_OPLINE

	ZVAL_LONG(EX_VAR(opline->result.var), EG(error_reporting));

	if (!E_HAS_ONLY_FATAL_ERRORS(EG(error_reporting))) {
		do {
			/* Do not silence fatal errors */
			EG(error_reporting) &= E_FATAL_ERRORS;
			if (!EG(error_reporting_ini_entry)) {
				zval *zv = zend_hash_find_known_hash(EG(ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING));
				if (zv) {
					EG(error_reporting_ini_entry) = (zend_ini_entry *)Z_PTR_P(zv);
				} else {
					break;
				}
			}
			if (!EG(error_reporting_ini_entry)->modified) {
				if (!EG(modified_ini_directives)) {
					ALLOC_HASHTABLE(EG(modified_ini_directives));
					zend_hash_init(EG(modified_ini_directives), 8, NULL, NULL, 0);
				}
				if (EXPECTED(zend_hash_add_ptr(EG(modified_ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING), EG(error_reporting_ini_entry)) != NULL)) {
					EG(error_reporting_ini_entry)->orig_value = EG(error_reporting_ini_entry)->value;
					EG(error_reporting_ini_entry)->orig_modifiable = EG(error_reporting_ini_entry)->modifiable;
					EG(error_reporting_ini_entry)->modified = 1;
				}
			}
		} while (0);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(58, ZEND_END_SILENCE, TMP, ANY)
{
	USE_OPLINE

	if (E_HAS_ONLY_FATAL_ERRORS(EG(error_reporting))
			&& !E_HAS_ONLY_FATAL_ERRORS(Z_LVAL_P(EX_VAR(opline->op1.var)))) {
		EG(error_reporting) = Z_LVAL_P(EX_VAR(opline->op1.var));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *value;
	zend_reference *ref = NULL;
	bool ret;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && Z_ISREF_P(value)) {
		if (OP1_TYPE == IS_VAR) {
			ref = Z_REF_P(value);
		}
		value = Z_REFVAL_P(value);
	}

	ret = i_zend_is_true(value);

	if (UNEXPECTED(EG(exception))) {
		FREE_OP1();
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		HANDLE_EXCEPTION();
	}

	if (ret) {
		zval *result = EX_VAR(opline->result.var);

		ZVAL_COPY_VALUE(result, value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_REFCOUNTED_P(result))) Z_ADDREF_P(result);
		} else if (OP1_TYPE == IS_CV) {
			if (Z_OPT_REFCOUNTED_P(result)) Z_ADDREF_P(result);
		} else if (OP1_TYPE == IS_VAR && ref) {
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				efree_size(ref, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(169, ZEND_COALESCE, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *value;
	zend_reference *ref = NULL;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_IS);

	if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(value)) {
		if (OP1_TYPE & IS_VAR) {
			ref = Z_REF_P(value);
		}
		value = Z_REFVAL_P(value);
	}

	if (Z_TYPE_P(value) > IS_NULL) {
		zval *result = EX_VAR(opline->result.var);
		ZVAL_COPY_VALUE(result, value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_REFCOUNTED_P(result))) Z_ADDREF_P(result);
		} else if (OP1_TYPE == IS_CV) {
			if (Z_OPT_REFCOUNTED_P(result)) Z_ADDREF_P(result);
		} else if ((OP1_TYPE & IS_VAR) && ref) {
			if (UNEXPECTED(GC_DELREF(ref) == 0)) {
				efree_size(ref, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	if ((OP1_TYPE & IS_VAR) && ref) {
		if (UNEXPECTED(GC_DELREF(ref) == 0)) {
			efree_size(ref, sizeof(zend_reference));
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_NOCONST_HANDLER(198, ZEND_JMP_NULL, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zval *val, *result;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_P(val) > IS_NULL) {
		do {
			if ((OP1_TYPE == IS_CV || OP1_TYPE == IS_VAR) && Z_TYPE_P(val) == IS_REFERENCE) {
				val = Z_REFVAL_P(val);
				if (Z_TYPE_P(val) <= IS_NULL) {
					FREE_OP1();
					break;
				}
			}
			ZEND_VM_NEXT_OPCODE();
		} while (0);
	}

	result = EX_VAR(opline->result.var);
	uint32_t short_circuiting_type = opline->extended_value & ZEND_SHORT_CIRCUITING_CHAIN_MASK;
	if (EXPECTED(short_circuiting_type == ZEND_SHORT_CIRCUITING_CHAIN_EXPR)) {
		ZVAL_NULL(result);
		if (OP1_TYPE == IS_CV
			&& UNEXPECTED(Z_TYPE_P(val) == IS_UNDEF)
			&& (opline->extended_value & ZEND_JMP_NULL_BP_VAR_IS) == 0
		) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
	} else if (short_circuiting_type == ZEND_SHORT_CIRCUITING_CHAIN_ISSET) {
		ZVAL_FALSE(result);
	} else {
		ZEND_ASSERT(short_circuiting_type == ZEND_SHORT_CIRCUITING_CHAIN_EMPTY);
		ZVAL_TRUE(result);
	}

	ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
}

ZEND_VM_HOT_HANDLER(31, ZEND_QM_ASSIGN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *value;
	zval *result = EX_VAR(opline->result.var);

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		SAVE_OPLINE();
		ZVAL_UNDEFINED_OP1();
		ZVAL_NULL(result);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY_DEREF(result, value);
	} else if (OP1_TYPE == IS_VAR) {
		if (UNEXPECTED(Z_ISREF_P(value))) {
			ZVAL_COPY_VALUE(result, Z_REFVAL_P(value));
			if (UNEXPECTED(Z_DELREF_P(value) == 0)) {
				efree_size(Z_REF_P(value), sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		} else {
			ZVAL_COPY_VALUE(result, value);
		}
	} else {
		ZVAL_COPY_VALUE(result, value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_REFCOUNTED_P(result))) {
				Z_ADDREF_P(result);
			}
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HANDLER(101, ZEND_EXT_STMT, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HANDLER(102, ZEND_EXT_FCALL_BEGIN, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HANDLER(103, ZEND_EXT_FCALL_END, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(144, ZEND_DECLARE_CLASS, CONST, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	do_bind_class(RT_CONSTANT(opline, opline->op1), (OP2_TYPE == IS_CONST) ? Z_STR_P(RT_CONSTANT(opline, opline->op2)) : NULL);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_CLASS_DELAYED, CONST, CONST)
{
	USE_OPLINE

	zend_class_entry *ce = CACHED_PTR(opline->extended_value);
	if (ce == NULL) {
		zval *lcname = RT_CONSTANT(opline, opline->op1);
		zval *zv = zend_hash_find_known_hash(EG(class_table), Z_STR_P(lcname + 1));
		if (zv) {
			SAVE_OPLINE();
			ce = zend_bind_class_in_slot(zv, lcname, Z_STR_P(RT_CONSTANT(opline, opline->op2)));
			if (!ce) {
				HANDLE_EXCEPTION();
			}
		}
		CACHE_PTR(opline->extended_value, ce);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(146, ZEND_DECLARE_ANON_CLASS, ANY, ANY, CACHE_SLOT)
{
	zval *zv;
	zend_class_entry *ce;
	USE_OPLINE

	ce = CACHED_PTR(opline->extended_value);
	if (UNEXPECTED(ce == NULL)) {
		zend_string *rtd_key = Z_STR_P(RT_CONSTANT(opline, opline->op1));
		zv = zend_hash_find_known_hash(EG(class_table), rtd_key);
		ZEND_ASSERT(zv != NULL);
		ce = Z_CE_P(zv);
		if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
			SAVE_OPLINE();
			ce = zend_do_link_class(ce, (OP2_TYPE == IS_CONST) ? Z_STR_P(RT_CONSTANT(opline, opline->op2)) : NULL, rtd_key);
			if (!ce) {
				HANDLE_EXCEPTION();
			}
		}
		CACHE_PTR(opline->extended_value, ce);
	}
	Z_CE_P(EX_VAR(opline->result.var)) = ce;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(141, ZEND_DECLARE_FUNCTION, ANY, NUM)
{
	zend_function *func;
	USE_OPLINE

	SAVE_OPLINE();
	func = (zend_function *) EX(func)->op_array.dynamic_func_defs[opline->op2.num];
	do_bind_function(func, RT_CONSTANT(opline, opline->op1));
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(105, ZEND_TICKS, ANY, ANY, NUM)
{
	USE_OPLINE

	if ((uint32_t)++EG(ticks_count) >= opline->extended_value) {
		EG(ticks_count) = 0;
		if (zend_ticks_function) {
			SAVE_OPLINE();
			zend_fiber_switch_block();
			zend_ticks_function(opline->extended_value);
			zend_fiber_switch_unblock();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	USE_OPLINE
	zval *expr;
	bool result;

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_instanceof):
	if (Z_TYPE_P(expr) == IS_OBJECT) {
		zend_class_entry *ce;

		if (OP2_TYPE == IS_CONST) {
			ce = CACHED_PTR(opline->extended_value);
			if (UNEXPECTED(ce == NULL)) {
				ce = zend_lookup_class_ex(Z_STR_P(RT_CONSTANT(opline, opline->op2)), Z_STR_P(RT_CONSTANT(opline, opline->op2) + 1), ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (EXPECTED(ce)) {
					CACHE_PTR(opline->extended_value, ce);
				}
			}
		} else if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				FREE_OP1();
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		result = ce && instanceof_function(Z_OBJCE_P(expr), ce);
	} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(expr) == IS_REFERENCE) {
		expr = Z_REFVAL_P(expr);
		ZEND_VM_C_GOTO(try_instanceof);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(expr) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		result = 0;
	}
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
}

ZEND_VM_HOT_HANDLER(104, ZEND_EXT_NOP, ANY, ANY)
{
	USE_OPLINE

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(0, ZEND_NOP, ANY, ANY)
{
	USE_OPLINE

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HELPER(zend_dispatch_try_catch_finally_helper, ANY, ANY, uint32_t try_catch_offset, uint32_t op_num)
{
	/* May be NULL during generator closing (only finally blocks are executed) */
	zend_object *ex = EG(exception);

	/* Walk try/catch/finally structures upwards, performing the necessary actions */
	for (; try_catch_offset != (uint32_t) -1; try_catch_offset--) {
		zend_try_catch_element *try_catch =
			&EX(func)->op_array.try_catch_array[try_catch_offset];

		if (op_num < try_catch->catch_op && ex) {
			/* Go to catch block */
			cleanup_live_vars(execute_data, op_num, try_catch->catch_op);
			ZEND_VM_JMP_EX(&EX(func)->op_array.opcodes[try_catch->catch_op], 0);

		} else if (op_num < try_catch->finally_op) {
			if (ex && zend_is_unwind_exit(ex)) {
				/* Don't execute finally blocks on exit (for now) */
				continue;
			}

			/* Go to finally block */
			zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[try_catch->finally_end].op1.var);
			cleanup_live_vars(execute_data, op_num, try_catch->finally_op);
			Z_OBJ_P(fast_call) = EG(exception);
			EG(exception) = NULL;
			Z_OPLINE_NUM_P(fast_call) = (uint32_t)-1;
			ZEND_VM_JMP_EX(&EX(func)->op_array.opcodes[try_catch->finally_op], 0);

		} else if (op_num < try_catch->finally_end) {
			zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[try_catch->finally_end].op1.var);

			/* cleanup incomplete RETURN statement */
			if (Z_OPLINE_NUM_P(fast_call) != (uint32_t)-1
			 && (EX(func)->op_array.opcodes[Z_OPLINE_NUM_P(fast_call)].op2_type & (IS_TMP_VAR | IS_VAR))) {
				zval *return_value = EX_VAR(EX(func)->op_array.opcodes[Z_OPLINE_NUM_P(fast_call)].op2.var);

				zval_ptr_dtor(return_value);
			}

			/* Chain potential exception from wrapping finally block */
			if (Z_OBJ_P(fast_call)) {
				if (ex) {
					if (zend_is_unwind_exit(ex) || zend_is_graceful_exit(ex)) {
						/* discard the previously thrown exception */
						OBJ_RELEASE(Z_OBJ_P(fast_call));
					} else {
						zend_exception_set_previous(ex, Z_OBJ_P(fast_call));
					}
				} else {
					ex = EG(exception) = Z_OBJ_P(fast_call);
				}
			}
		}
	}

	/* Uncaught exception */
	if (zend_observer_fcall_op_array_extension != -1) {
		zend_observer_fcall_end(execute_data, NULL);
	}
	cleanup_live_vars(execute_data, op_num, 0);
	if (UNEXPECTED((EX_CALL_INFO() & ZEND_CALL_GENERATOR) != 0)) {
		zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);
		EG(current_execute_data) = EX(prev_execute_data);
		zend_generator_close(generator, 1);
		ZEND_VM_RETURN();
	} else {
		/* We didn't execute RETURN, and have to initialize return_value */
		if (EX(return_value)) {
			ZVAL_UNDEF(EX(return_value));
		}
		ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
	}
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	const zend_op *throw_op = EG(opline_before_exception);

	/* Exception was thrown before executing any op */
	if (UNEXPECTED(!throw_op)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_dispatch_try_catch_finally_helper, try_catch_offset, -1, 0, 0);
	}

	uint32_t throw_op_num = throw_op - EX(func)->op_array.opcodes;
	int i, current_try_catch_offset = -1;

	if ((throw_op->opcode == ZEND_FREE || throw_op->opcode == ZEND_FE_FREE)
		&& throw_op->extended_value & ZEND_FREE_ON_RETURN) {
		/* exceptions thrown because of loop var destruction on return/break/...
		 * are logically thrown at the end of the foreach loop, so adjust the
		 * throw_op_num.
		 */
		const zend_live_range *range = find_live_range(
			&EX(func)->op_array, throw_op_num, throw_op->op1.var);
		/* free op1 of the corresponding RETURN */
		for (i = throw_op_num; i < range->end; i++) {
			if (EX(func)->op_array.opcodes[i].opcode == ZEND_FREE
			 || EX(func)->op_array.opcodes[i].opcode == ZEND_FE_FREE) {
				/* pass */
			} else {
				if (EX(func)->op_array.opcodes[i].opcode == ZEND_RETURN
				 && (EX(func)->op_array.opcodes[i].op1_type & (IS_VAR|IS_TMP_VAR))) {
					zval_ptr_dtor(EX_VAR(EX(func)->op_array.opcodes[i].op1.var));
				}
				break;
			}
		}
		throw_op_num = range->end;
	}

	/* Find the innermost try/catch/finally the exception was thrown in */
	for (i = 0; i < EX(func)->op_array.last_try_catch; i++) {
		zend_try_catch_element *try_catch = &EX(func)->op_array.try_catch_array[i];
		if (try_catch->try_op > throw_op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		if (throw_op_num < try_catch->catch_op || throw_op_num < try_catch->finally_end) {
			current_try_catch_offset = i;
		}
	}

	cleanup_unfinished_calls(execute_data, throw_op_num);

	if (throw_op->result_type & (IS_VAR | IS_TMP_VAR)) {
		switch (throw_op->opcode) {
			case ZEND_ADD_ARRAY_ELEMENT:
			case ZEND_ADD_ARRAY_UNPACK:
			case ZEND_ROPE_INIT:
			case ZEND_ROPE_ADD:
				break; /* exception while building structures, live range handling will free those */

			case ZEND_FETCH_CLASS:
			case ZEND_DECLARE_ANON_CLASS:
				break; /* return value is zend_class_entry pointer */

			default:
				/* smart branch opcodes may not initialize result */
				if (!zend_is_smart_branch(throw_op)) {
					zval_ptr_dtor_nogc(EX_VAR(throw_op->result.var));
				}
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_dispatch_try_catch_finally_helper, try_catch_offset, current_try_catch_offset, op_num, throw_op_num);
}

ZEND_VM_HANDLER(150, ZEND_USER_OPCODE, ANY, ANY)
{
	USE_OPLINE
	int ret;

	SAVE_OPLINE();
	ret = zend_user_opcode_handlers[opline->opcode](execute_data);
	opline = EX(opline);

	switch (ret) {
		case ZEND_USER_OPCODE_CONTINUE:
			ZEND_VM_CONTINUE();
		case ZEND_USER_OPCODE_RETURN:
			if (UNEXPECTED((EX_CALL_INFO() & ZEND_CALL_GENERATOR) != 0)) {
				zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);
				EG(current_execute_data) = EX(prev_execute_data);
				zend_generator_close(generator, 1);
				ZEND_VM_RETURN();
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
			ZEND_VM_DISPATCH((uint8_t)(ret & 0xff), opline);
	}
}

ZEND_VM_HANDLER(143, ZEND_DECLARE_CONST, CONST, CONST)
{
	USE_OPLINE
	zval *name;
	zval *val;
	zend_constant c;

	SAVE_OPLINE();
	name  = GET_OP1_ZVAL_PTR(BP_VAR_R);
	val   = GET_OP2_ZVAL_PTR(BP_VAR_R);

	ZVAL_COPY(&c.value, val);
	if (Z_OPT_CONSTANT(c.value)) {
		if (UNEXPECTED(zval_update_constant_ex(&c.value, EX(func)->op_array.scope) != SUCCESS)) {
			zval_ptr_dtor_nogc(&c.value);
			FREE_OP1();
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
	}
	/* non persistent, case sensitive */
	ZEND_CONSTANT_SET_FLAGS(&c, 0, PHP_USER_CONSTANT);
	c.name = zend_string_copy(Z_STR_P(name));

	if (zend_register_constant(&c) == FAILURE) {
	}

	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(142, ZEND_DECLARE_LAMBDA_FUNCTION, CONST, NUM)
{
	USE_OPLINE
	zend_function *func;
	zval *object;
	zend_class_entry *called_scope;

	func = (zend_function *) EX(func)->op_array.dynamic_func_defs[opline->op2.num];
	if (Z_TYPE(EX(This)) == IS_OBJECT) {
		called_scope = Z_OBJCE(EX(This));
		if (UNEXPECTED((func->common.fn_flags & ZEND_ACC_STATIC) ||
				(EX(func)->common.fn_flags & ZEND_ACC_STATIC))) {
			object = NULL;
		} else {
			object = &EX(This);
		}
	} else {
		called_scope = Z_CE(EX(This));
		object = NULL;
	}
	SAVE_OPLINE();
	zend_create_closure(EX_VAR(opline->result.var), func,
		EX(func)->op_array.scope, called_scope, object);

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(156, ZEND_SEPARATE, VAR, UNUSED)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = EX_VAR(opline->op1.var);
	if (UNEXPECTED(Z_ISREF_P(var_ptr))) {
		if (UNEXPECTED(Z_REFCOUNT_P(var_ptr) == 1)) {
			ZVAL_UNREF(var_ptr);
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HELPER(zend_yield_in_closed_generator_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot yield from finally in a force-closed generator");
	FREE_OP2();
	FREE_OP1();
	UNDEF_RESULT();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(160, ZEND_YIELD, CONST|TMP|VAR|CV|UNUSED, CONST|TMPVAR|CV|UNUSED, SRC)
{
	USE_OPLINE

	zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);

	SAVE_OPLINE();
	if (UNEXPECTED(generator->flags & ZEND_GENERATOR_FORCED_CLOSE)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_yield_in_closed_generator_helper);
	}

	/* Destroy the previously yielded value */
	zval_ptr_dtor(&generator->value);

	/* Destroy the previously yielded key */
	zval_ptr_dtor(&generator->key);

	/* Set the new yielded value */
	if (OP1_TYPE != IS_UNUSED) {
		if (UNEXPECTED(EX(func)->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
			/* Constants and temporary variables aren't yieldable by reference,
			 * but we still allow them with a notice. */
			if (OP1_TYPE & (IS_CONST|IS_TMP_VAR)) {
				zval *value;

				zend_error(E_NOTICE, "Only variable references should be yielded by reference");

				value = GET_OP1_ZVAL_PTR(BP_VAR_R);
				ZVAL_COPY_VALUE(&generator->value, value);
				if (OP1_TYPE == IS_CONST) {
					if (UNEXPECTED(Z_OPT_REFCOUNTED(generator->value))) {
						Z_ADDREF(generator->value);
					}
				}
			} else {
				zval *value_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

				/* If a function call result is yielded and the function did
				 * not return by reference we throw a notice. */
				do {
					if (OP1_TYPE == IS_VAR) {
						ZEND_ASSERT(value_ptr != &EG(uninitialized_zval));
						if (opline->extended_value == ZEND_RETURNS_FUNCTION
						 && !Z_ISREF_P(value_ptr)) {
							zend_error(E_NOTICE, "Only variable references should be yielded by reference");
							ZVAL_COPY(&generator->value, value_ptr);
							break;
						}
					}
					if (Z_ISREF_P(value_ptr)) {
						Z_ADDREF_P(value_ptr);
					} else {
						ZVAL_MAKE_REF_EX(value_ptr, 2);
					}
					ZVAL_REF(&generator->value, Z_REF_P(value_ptr));
				} while (0);

				FREE_OP1();
			}
		} else {
			zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);

			/* Consts, temporary variables and references need copying */
			if (OP1_TYPE == IS_CONST) {
				ZVAL_COPY_VALUE(&generator->value, value);
				if (UNEXPECTED(Z_OPT_REFCOUNTED(generator->value))) {
					Z_ADDREF(generator->value);
				}
			} else if (OP1_TYPE == IS_TMP_VAR) {
				ZVAL_COPY_VALUE(&generator->value, value);
			} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(value)) {
				ZVAL_COPY(&generator->value, Z_REFVAL_P(value));
				FREE_OP1_IF_VAR();
			} else {
				ZVAL_COPY_VALUE(&generator->value, value);
				if (OP1_TYPE == IS_CV) {
					if (Z_OPT_REFCOUNTED_P(value)) Z_ADDREF_P(value);
				}
			}
		}
	} else {
		/* If no value was specified yield null */
		ZVAL_NULL(&generator->value);
	}

	/* Set the new yielded key */
	if (OP2_TYPE != IS_UNUSED) {
		zval *key = GET_OP2_ZVAL_PTR(BP_VAR_R);
		if ((OP2_TYPE & (IS_CV|IS_VAR)) && UNEXPECTED(Z_TYPE_P(key) == IS_REFERENCE)) {
			key = Z_REFVAL_P(key);
		}
		ZVAL_COPY(&generator->key, key);
		FREE_OP2();

		if (Z_TYPE(generator->key) == IS_LONG
		    && Z_LVAL(generator->key) > generator->largest_used_integer_key
		) {
			generator->largest_used_integer_key = Z_LVAL(generator->key);
		}
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

ZEND_VM_HANDLER(166, ZEND_YIELD_FROM, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);
	zval *val;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (UNEXPECTED(generator->flags & ZEND_GENERATOR_FORCED_CLOSE)) {
		zend_throw_error(NULL, "Cannot use \"yield from\" in a force-closed generator");
		FREE_OP1();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

ZEND_VM_C_LABEL(yield_from_try_again):
	if (Z_TYPE_P(val) == IS_ARRAY) {
		ZVAL_COPY_VALUE(&generator->values, val);
		if (Z_OPT_REFCOUNTED_P(val)) {
			Z_ADDREF_P(val);
		}
		Z_FE_POS(generator->values) = 0;
		FREE_OP1();
	} else if (OP1_TYPE != IS_CONST && Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val)->get_iterator) {
		zend_class_entry *ce = Z_OBJCE_P(val);
		if (ce == zend_ce_generator) {
			zend_generator *new_gen = (zend_generator *) Z_OBJ_P(val);

			Z_ADDREF_P(val);
			FREE_OP1();

			if (UNEXPECTED(new_gen->execute_data == NULL)) {
				zend_throw_error(NULL, "Generator passed to yield from was aborted without proper return and is unable to continue");
				zval_ptr_dtor(val);
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			} else if (Z_ISUNDEF(new_gen->retval)) {
				if (UNEXPECTED(zend_generator_get_current(new_gen) == generator)) {
					zend_throw_error(NULL, "Impossible to yield from the Generator being currently run");
					zval_ptr_dtor(val);
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				} else {
					zend_generator_yield_from(generator, new_gen);
				}
			} else {
				if (RETURN_VALUE_USED(opline)) {
					ZVAL_COPY(EX_VAR(opline->result.var), &new_gen->retval);
				}
				ZEND_VM_NEXT_OPCODE();
			}
		} else {
			zend_object_iterator *iter = ce->get_iterator(ce, val, 0);
			FREE_OP1();

			if (UNEXPECTED(!iter) || UNEXPECTED(EG(exception))) {
				if (!EG(exception)) {
					zend_throw_error(NULL, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name));
				}
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}

			iter->index = 0;
			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					OBJ_RELEASE(&iter->std);
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
			}

			ZVAL_OBJ(&generator->values, &iter->std);
		}
	} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(val) == IS_REFERENCE) {
		val = Z_REFVAL_P(val);
		ZEND_VM_C_GOTO(yield_from_try_again);
	} else {
		zend_throw_error(NULL, "Can use \"yield from\" only with arrays and Traversables");
		FREE_OP1();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	/* This is the default return value
	 * when the expression is a Generator, it will be overwritten in zend_generator_resume() */
	if (RETURN_VALUE_USED(opline)) {
		ZVAL_NULL(EX_VAR(opline->result.var));
	}

	/* This generator has no send target (though the generator we delegate to might have one) */
	generator->send_target = NULL;

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
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->op1.var);
	SAVE_OPLINE();

	/* cleanup incomplete RETURN statement */
	if (Z_OPLINE_NUM_P(fast_call) != (uint32_t)-1
	 && (EX(func)->op_array.opcodes[Z_OPLINE_NUM_P(fast_call)].op2_type & (IS_TMP_VAR | IS_VAR))) {
		zval *return_value = EX_VAR(EX(func)->op_array.opcodes[Z_OPLINE_NUM_P(fast_call)].op2.var);

		zval_ptr_dtor(return_value);
	}

	/* cleanup delayed exception */
	if (Z_OBJ_P(fast_call) != NULL) {
		/* discard the previously thrown exception */
		OBJ_RELEASE(Z_OBJ_P(fast_call));
		Z_OBJ_P(fast_call) = NULL;
	}

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(162, ZEND_FAST_CALL, JMP_ADDR, ANY)
{
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->result.var);

	Z_OBJ_P(fast_call) = NULL;
	/* set return address */
	Z_OPLINE_NUM_P(fast_call) = opline - EX(func)->op_array.opcodes;
	ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op1), 0);
}

ZEND_VM_HANDLER(163, ZEND_FAST_RET, ANY, TRY_CATCH)
{
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->op1.var);
	uint32_t current_try_catch_offset, current_op_num;

	if (Z_OPLINE_NUM_P(fast_call) != (uint32_t)-1) {
		const zend_op *fast_ret = EX(func)->op_array.opcodes + Z_OPLINE_NUM_P(fast_call);

		ZEND_VM_JMP_EX(fast_ret + 1, 0);
	}

	/* special case for unhandled exceptions */
	EG(exception) = Z_OBJ_P(fast_call);
	Z_OBJ_P(fast_call) = NULL;
	current_try_catch_offset = opline->op2.num;
	current_op_num = opline - EX(func)->op_array.opcodes;
	ZEND_VM_DISPATCH_TO_HELPER(zend_dispatch_try_catch_finally_helper, try_catch_offset, current_try_catch_offset, op_num, current_op_num);
}

ZEND_VM_HOT_HANDLER(168, ZEND_BIND_GLOBAL, CV, CONST, CACHE_SLOT)
{
	USE_OPLINE
	zend_string *varname;
	zval *value;
	zval *variable_ptr;
	uintptr_t idx;
	zend_reference *ref;

	ZEND_VM_REPEATABLE_OPCODE

	varname = Z_STR_P(GET_OP2_ZVAL_PTR(BP_VAR_R));

	/* We store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
	idx = (uintptr_t)CACHED_PTR(opline->extended_value) - 1;
	if (EXPECTED(idx < EG(symbol_table).nNumUsed * sizeof(Bucket))) {
		Bucket *p = (Bucket*)((char*)EG(symbol_table).arData + idx);

		if (EXPECTED(p->key == varname) ||
		    (EXPECTED(p->h == ZSTR_H(varname)) &&
		     EXPECTED(p->key != NULL) &&
		     EXPECTED(zend_string_equal_content(p->key, varname)))) {

			value = (zval*)p; /* value = &p->val; */
			ZEND_VM_C_GOTO(check_indirect);
		}
	}

	value = zend_hash_find_known_hash(&EG(symbol_table), varname);
	if (UNEXPECTED(value == NULL)) {
		value = zend_hash_add_new(&EG(symbol_table), varname, &EG(uninitialized_zval));
		idx = (char*)value - (char*)EG(symbol_table).arData;
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(opline->extended_value, (void*)(idx + 1));
	} else {
		idx = (char*)value - (char*)EG(symbol_table).arData;
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(opline->extended_value, (void*)(idx + 1));
ZEND_VM_C_LABEL(check_indirect):
		/* GLOBAL variable may be an INDIRECT pointer to CV */
		if (UNEXPECTED(Z_TYPE_P(value) == IS_INDIRECT)) {
			value = Z_INDIRECT_P(value);
			if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
				ZVAL_NULL(value);
			}
		}
	}

	if (UNEXPECTED(!Z_ISREF_P(value))) {
		ZVAL_MAKE_REF_EX(value, 2);
		ref = Z_REF_P(value);
	} else {
		ref = Z_REF_P(value);
		GC_ADDREF(ref);
	}

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr))) {
		zend_refcounted *garbage = Z_COUNTED_P(variable_ptr);

		ZVAL_REF(variable_ptr, ref);
		SAVE_OPLINE();
		if (GC_DELREF(garbage) == 0) {
			rc_dtor_func(garbage);
			if (UNEXPECTED(EG(exception))) {
				ZVAL_NULL(variable_ptr);
				HANDLE_EXCEPTION();
			}
		} else {
			gc_check_possible_root(garbage);
		}
	} else {
		ZVAL_REF(variable_ptr, ref);
	}

	ZEND_VM_REPEAT_OPCODE(ZEND_BIND_GLOBAL);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(121, ZEND_STRLEN, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(value) == IS_STRING)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_STRLEN_P(value));
		if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
			zval_ptr_dtor_str(value);
		}
		ZEND_VM_NEXT_OPCODE();
	} else {
		bool strict;

		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(value) == IS_REFERENCE) {
			value = Z_REFVAL_P(value);
			if (EXPECTED(Z_TYPE_P(value) == IS_STRING)) {
				ZVAL_LONG(EX_VAR(opline->result.var), Z_STRLEN_P(value));
				FREE_OP1();
				ZEND_VM_NEXT_OPCODE();
			}
		}

		SAVE_OPLINE();
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
			value = ZVAL_UNDEFINED_OP1();
		}
		strict = EX_USES_STRICT_TYPES();
		do {
			if (EXPECTED(!strict)) {
				zend_string *str;
				zval tmp;

				if (UNEXPECTED(Z_TYPE_P(value) == IS_NULL)) {
					zend_error(E_DEPRECATED,
						"strlen(): Passing null to parameter #1 ($string) of type string is deprecated");
					ZVAL_LONG(EX_VAR(opline->result.var), 0);
					if (UNEXPECTED(EG(exception))) {
						HANDLE_EXCEPTION();
					}
					break;
				}

				ZVAL_COPY(&tmp, value);
				if (zend_parse_arg_str_weak(&tmp, &str, 1)) {
					ZVAL_LONG(EX_VAR(opline->result.var), ZSTR_LEN(str));
					zval_ptr_dtor(&tmp);
					break;
				}
				zval_ptr_dtor(&tmp);
			}
			if (!EG(exception)) {
				zend_type_error("strlen(): Argument #1 ($string) must be of type string, %s given", zend_zval_value_name(value));
			}
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		} while (0);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_NOCONST_HANDLER(123, ZEND_TYPE_CHECK, CONST|TMPVAR|CV, ANY, TYPE_MASK)
{
	USE_OPLINE
	zval *value;
	int result = 0;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if ((opline->extended_value >> (uint32_t)Z_TYPE_P(value)) & 1) {
ZEND_VM_C_LABEL(type_check_resource):
		if (opline->extended_value != MAY_BE_RESOURCE
		 || EXPECTED(NULL != zend_rsrc_list_get_rsrc_type(Z_RES_P(value)))) {
			result = 1;
		}
	} else if ((OP1_TYPE & (IS_CV|IS_VAR)) && Z_ISREF_P(value)) {
		value = Z_REFVAL_P(value);
		if ((opline->extended_value >> (uint32_t)Z_TYPE_P(value)) & 1) {
			ZEND_VM_C_GOTO(type_check_resource);
		}
	} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		result = ((1 << IS_NULL) & opline->extended_value) != 0;
		SAVE_OPLINE();
		ZVAL_UNDEFINED_OP1();
		if (UNEXPECTED(EG(exception))) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		}
	}
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		SAVE_OPLINE();
		FREE_OP1();
		ZEND_VM_SMART_BRANCH(result, 1);
	} else {
		ZEND_VM_SMART_BRANCH(result, 0);
	}
}

ZEND_VM_HOT_HANDLER(122, ZEND_DEFINED, CONST, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zend_constant *c;

	c = CACHED_PTR(opline->extended_value);
	if (EXPECTED(c != NULL)) {
		if (!IS_SPECIAL_CACHE_VAL(c)) {
ZEND_VM_C_LABEL(defined_true):
			ZEND_VM_SMART_BRANCH_TRUE();
		} else if (EXPECTED(zend_hash_num_elements(EG(zend_constants)) == DECODE_SPECIAL_CACHE_NUM(c))) {
ZEND_VM_C_LABEL(defined_false):
			ZEND_VM_SMART_BRANCH_FALSE();
		}
	}
	if (zend_quick_check_constant(RT_CONSTANT(opline, opline->op1) OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
		CACHE_PTR(opline->extended_value, ENCODE_SPECIAL_CACHE_NUM(zend_hash_num_elements(EG(zend_constants))));
		ZEND_VM_C_GOTO(defined_false);
	} else {
		ZEND_VM_C_GOTO(defined_true);
	}
}

ZEND_VM_HANDLER(151, ZEND_ASSERT_CHECK, ANY, JMP_ADDR)
{
	USE_OPLINE

	if (EG(assertions) <= 0) {
		zend_op *target = OP_JMP_ADDR(opline, opline->op2);
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_TRUE(EX_VAR(opline->result.var));
		}
		ZEND_VM_JMP_EX(target, 0);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(157, ZEND_FETCH_CLASS_NAME, CV|TMPVAR|UNUSED|CLASS_FETCH, ANY)
{
	uint32_t fetch_type;
	zend_class_entry *called_scope, *scope;
	USE_OPLINE

	if (OP1_TYPE != IS_UNUSED) {
		SAVE_OPLINE();
		zval *op = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (UNEXPECTED(Z_TYPE_P(op) != IS_OBJECT)) {
			ZVAL_DEREF(op);
			if (Z_TYPE_P(op) != IS_OBJECT) {
				zend_type_error("Cannot use \"::class\" on %s", zend_zval_value_name(op));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		}

		ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_OBJCE_P(op)->name);
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	fetch_type = opline->op1.num;
	scope = EX(func)->op_array.scope;
	if (UNEXPECTED(scope == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot use \"%s\" in the global scope",
			fetch_type == ZEND_FETCH_CLASS_SELF ? "self" :
			fetch_type == ZEND_FETCH_CLASS_PARENT ? "parent" : "static");
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		HANDLE_EXCEPTION();
	}

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			ZVAL_STR_COPY(EX_VAR(opline->result.var), scope->name);
			break;
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(scope->parent == NULL)) {
				SAVE_OPLINE();
				zend_throw_error(NULL,
					"Cannot use \"parent\" when current class scope has no parent");
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
			ZVAL_STR_COPY(EX_VAR(opline->result.var), scope->parent->name);
			break;
		case ZEND_FETCH_CLASS_STATIC:
			if (Z_TYPE(EX(This)) == IS_OBJECT) {
				called_scope = Z_OBJCE(EX(This));
			} else {
				called_scope = Z_CE(EX(This));
			}
			ZVAL_STR_COPY(EX_VAR(opline->result.var), called_scope->name);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(158, ZEND_CALL_TRAMPOLINE, ANY, ANY, SPEC(OBSERVER))
{
	zend_array *args = NULL;
	zend_function *fbc = EX(func);
	zval *ret = EX(return_value);
	uint32_t call_info = EX_CALL_INFO() & (ZEND_CALL_NESTED | ZEND_CALL_TOP | ZEND_CALL_RELEASE_THIS | ZEND_CALL_HAS_EXTRA_NAMED_PARAMS);
	uint32_t num_args = EX_NUM_ARGS();
	zend_execute_data *call;

	SAVE_OPLINE();

	if (num_args) {
		zval *p = ZEND_CALL_ARG(execute_data, 1);
		zval *end = p + num_args;

		args = zend_new_array(num_args);
		zend_hash_real_init_packed(args);
		ZEND_HASH_FILL_PACKED(args) {
			do {
				ZEND_HASH_FILL_ADD(p);
				p++;
			} while (p != end);
		} ZEND_HASH_FILL_END();
	}

	call = execute_data;
	execute_data = EG(current_execute_data) = EX(prev_execute_data);

	call->func = (fbc->op_array.fn_flags & ZEND_ACC_STATIC) ? fbc->op_array.scope->__callstatic : fbc->op_array.scope->__call;
	ZEND_ASSERT(zend_vm_calc_used_stack(2, call->func) <= (size_t)(((char*)EG(vm_stack_end)) - (char*)call));
	ZEND_CALL_NUM_ARGS(call) = 2;

	ZVAL_STR(ZEND_CALL_ARG(call, 1), fbc->common.function_name);

	zval *call_args = ZEND_CALL_ARG(call, 2);
	if (args) {
		ZVAL_ARR(call_args, args);
	} else {
		ZVAL_EMPTY_ARRAY(call_args);
	}
	if (UNEXPECTED(call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
		if (zend_hash_num_elements(Z_ARRVAL_P(call_args)) == 0) {
			GC_ADDREF(call->extra_named_params);
			ZVAL_ARR(call_args, call->extra_named_params);
		} else {
			SEPARATE_ARRAY(call_args);
			zend_hash_copy(Z_ARRVAL_P(call_args), call->extra_named_params, zval_add_ref);
		}
	}
	zend_free_trampoline(fbc);
	fbc = call->func;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		if (UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
			init_func_run_time_cache(&fbc->op_array);
		}
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
		if (EXPECTED(zend_execute_ex == execute_ex)) {
			LOAD_OPLINE_EX();
			ZEND_OBSERVER_SAVE_OPLINE();
			ZEND_OBSERVER_FCALL_BEGIN(execute_data);
			ZEND_VM_ENTER_EX();
		} else {
			SAVE_OPLINE_EX();
			ZEND_OBSERVER_FCALL_BEGIN(execute_data);
			execute_data = EX(prev_execute_data);
			if (execute_data) {
				LOAD_OPLINE();
			}
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
		}
	} else {
		zval retval;

		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);

		EG(current_execute_data) = call;

#if ZEND_DEBUG
		bool should_throw = zend_internal_call_should_throw(fbc, call);
#endif

		if (ret == NULL) {
			ret = &retval;
		}

		ZVAL_NULL(ret);
		ZEND_OBSERVER_FCALL_BEGIN(call);
		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			fbc->internal_function.handler(call, ret);
		} else {
			zend_execute_internal(call, ret);
		}

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			if (should_throw) {
				zend_internal_call_arginfo_violation(call->func);
			}
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
			zend_verify_internal_func_info(call->func, ret);
		}
#endif
		ZEND_OBSERVER_FCALL_END(call, EG(exception) ? NULL : ret);

		EG(current_execute_data) = call->prev_execute_data;

		zend_vm_stack_free_args(call);
		if (UNEXPECTED(call_info & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			zend_free_extra_named_params(call->extra_named_params);
		}
		if (ret == &retval) {
			zval_ptr_dtor(ret);
		}
	}

	execute_data = EG(current_execute_data);

	if (!execute_data || !EX(func) || !ZEND_USER_CODE(EX(func)->type) || (call_info & ZEND_CALL_TOP)) {
		ZEND_VM_RETURN();
	}

	if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
		zend_object *object = Z_OBJ(call->This);
		OBJ_RELEASE(object);
	}
	zend_vm_stack_free_call_frame(call);

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION_LEAVE();
	}

	LOAD_OPLINE();
	ZEND_VM_INC_OPCODE();
	ZEND_VM_LEAVE();
}

ZEND_VM_HANDLER(182, ZEND_BIND_LEXICAL, TMP, CV, REF)
{
	USE_OPLINE
	zval *closure, *var;

	closure = GET_OP1_ZVAL_PTR(BP_VAR_R);
	if (opline->extended_value & ZEND_BIND_REF) {
		/* By-ref binding */
		var = GET_OP2_ZVAL_PTR(BP_VAR_W);
		if (Z_ISREF_P(var)) {
			Z_ADDREF_P(var);
		} else {
			ZVAL_MAKE_REF_EX(var, 2);
		}
	} else {
		var = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (UNEXPECTED(Z_ISUNDEF_P(var)) && !(opline->extended_value & ZEND_BIND_IMPLICIT)) {
			SAVE_OPLINE();
			var = ZVAL_UNDEFINED_OP2();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZVAL_DEREF(var);
		Z_TRY_ADDREF_P(var);
	}

	zend_closure_bind_var_ex(closure,
		(opline->extended_value & ~(ZEND_BIND_REF|ZEND_BIND_IMPLICIT)), var);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(183, ZEND_BIND_STATIC, CV, ANY, REF)
{
	USE_OPLINE
	HashTable *ht;
	zval *value;
	zval *variable_ptr;

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	SAVE_OPLINE();

	ht = ZEND_MAP_PTR_GET(EX(func)->op_array.static_variables_ptr);
	if (!ht) {
		ht = zend_array_dup(EX(func)->op_array.static_variables);
		ZEND_MAP_PTR_SET(EX(func)->op_array.static_variables_ptr, ht);
	}
	ZEND_ASSERT(GC_REFCOUNT(ht) == 1);

	value = (zval*)((char*)ht->arData + (opline->extended_value & ~(ZEND_BIND_REF|ZEND_BIND_IMPLICIT|ZEND_BIND_EXPLICIT)));

	if (opline->extended_value & ZEND_BIND_REF) {
		i_zval_ptr_dtor(variable_ptr);
		if (UNEXPECTED(!Z_ISREF_P(value))) {
			zend_reference *ref = (zend_reference*)emalloc(sizeof(zend_reference));
			GC_SET_REFCOUNT(ref, 2);
			GC_TYPE_INFO(ref) = GC_REFERENCE;
			if (OP2_TYPE == IS_UNUSED) {
				ZVAL_COPY_VALUE(&ref->val, value);
			} else {
				ZEND_ASSERT(!Z_REFCOUNTED_P(value));
				ZVAL_COPY(&ref->val, GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R));
				FREE_OP2();
			}
			ref->sources.ptr = NULL;
			Z_REF_P(value) = ref;
			Z_TYPE_INFO_P(value) = IS_REFERENCE_EX;
			ZVAL_REF(variable_ptr, ref);
		} else {
			Z_ADDREF_P(value);
			ZVAL_REF(variable_ptr, Z_REF_P(value));
			if (OP2_TYPE != IS_UNUSED) {
				FREE_OP2();
			}
		}
	} else {
		i_zval_ptr_dtor(variable_ptr);
		ZVAL_COPY(variable_ptr, value);
	}

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(203, ZEND_BIND_INIT_STATIC_OR_JMP, CV, JMP_ADDR)
{
	USE_OPLINE
	HashTable *ht;
	zval *value;
	zval *variable_ptr;

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	ht = ZEND_MAP_PTR_GET(EX(func)->op_array.static_variables_ptr);
	if (!ht) {
		ZEND_VM_NEXT_OPCODE();
	}
	ZEND_ASSERT(GC_REFCOUNT(ht) == 1);

	value = (zval*)((char*)ht->arData + opline->extended_value);
	if (Z_TYPE_EXTRA_P(value) & IS_STATIC_VAR_UNINITIALIZED) {
		ZEND_VM_NEXT_OPCODE();
	} else {
		SAVE_OPLINE();
		zval_ptr_dtor(variable_ptr);
		ZEND_ASSERT(Z_TYPE_P(value) == IS_REFERENCE);
		Z_ADDREF_P(value);
		ZVAL_REF(variable_ptr, Z_REF_P(value));
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 1);
	}
}

ZEND_VM_HOT_HANDLER(184, ZEND_FETCH_THIS, UNUSED, UNUSED)
{
	USE_OPLINE

	if (EXPECTED(Z_TYPE(EX(This)) == IS_OBJECT)) {
		zval *result = EX_VAR(opline->result.var);

		ZVAL_OBJ(result, Z_OBJ(EX(This)));
		Z_ADDREF_P(result);
		ZEND_VM_NEXT_OPCODE();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}
}

ZEND_VM_HANDLER(200, ZEND_FETCH_GLOBALS, UNUSED, UNUSED)
{
	USE_OPLINE

	/* For symbol tables we need to deal with exactly the same problems as for property tables. */
	ZVAL_ARR(EX_VAR(opline->result.var),
		zend_proptable_to_symtable(&EG(symbol_table), /* always_duplicate */ 1));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(186, ZEND_ISSET_ISEMPTY_THIS, UNUSED, UNUSED)
{
	USE_OPLINE

	ZVAL_BOOL(EX_VAR(opline->result.var),
		(opline->extended_value & ZEND_ISEMPTY) ^
		 (Z_TYPE(EX(This)) == IS_OBJECT));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(49, ZEND_CHECK_VAR, CV, UNUSED)
{
	USE_OPLINE
	zval *op1 = EX_VAR(opline->op1.var);

	if (UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		SAVE_OPLINE();
		ZVAL_UNDEFINED_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(140, ZEND_MAKE_REF, VAR|CV, UNUSED)
{
	USE_OPLINE
	zval *op1 = EX_VAR(opline->op1.var);

	if (OP1_TYPE == IS_CV) {
		if (UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			ZVAL_NEW_EMPTY_REF(op1);
			Z_SET_REFCOUNT_P(op1, 2);
			ZVAL_NULL(Z_REFVAL_P(op1));
			ZVAL_REF(EX_VAR(opline->result.var), Z_REF_P(op1));
		} else {
			if (Z_ISREF_P(op1)) {
				Z_ADDREF_P(op1);
			} else {
				ZVAL_MAKE_REF_EX(op1, 2);
			}
			ZVAL_REF(EX_VAR(opline->result.var), Z_REF_P(op1));
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_INDIRECT)) {
		op1 = Z_INDIRECT_P(op1);
		if (EXPECTED(!Z_ISREF_P(op1))) {
			ZVAL_MAKE_REF_EX(op1, 2);
		} else {
			GC_ADDREF(Z_REF_P(op1));
		}
		ZVAL_REF(EX_VAR(opline->result.var), Z_REF_P(op1));
	} else {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), op1);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(187, ZEND_SWITCH_LONG, CONST|TMPVARCV, CONST, JMP_ADDR)
{
	USE_OPLINE
	zval *op, *jump_zv;
	HashTable *jumptable;

	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_P(op) != IS_LONG) {
		ZVAL_DEREF(op);
		if (Z_TYPE_P(op) != IS_LONG) {
			/* Wrong type, fall back to ZEND_CASE chain */
			ZEND_VM_NEXT_OPCODE();
		}
	}

	jumptable = Z_ARRVAL_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
	jump_zv = zend_hash_index_find(jumptable, Z_LVAL_P(op));
	if (jump_zv != NULL) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, Z_LVAL_P(jump_zv));
		ZEND_VM_CONTINUE();
	} else {
		/* default */
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}
}

ZEND_VM_COLD_CONSTCONST_HANDLER(188, ZEND_SWITCH_STRING, CONST|TMPVARCV, CONST, JMP_ADDR)
{
	USE_OPLINE
	zval *op, *jump_zv;
	HashTable *jumptable;

	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_P(op) != IS_STRING) {
		if (OP1_TYPE == IS_CONST) {
			/* Wrong type, fall back to ZEND_CASE chain */
			ZEND_VM_NEXT_OPCODE();
		} else {
			ZVAL_DEREF(op);
			if (Z_TYPE_P(op) != IS_STRING) {
				/* Wrong type, fall back to ZEND_CASE chain */
				ZEND_VM_NEXT_OPCODE();
			}
		}
	}

	jumptable = Z_ARRVAL_P(GET_OP2_ZVAL_PTR(BP_VAR_R));
	jump_zv = zend_hash_find_ex(jumptable, Z_STR_P(op), OP1_TYPE == IS_CONST);
	if (jump_zv != NULL) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, Z_LVAL_P(jump_zv));
		ZEND_VM_CONTINUE();
	} else {
		/* default */
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}
}

ZEND_VM_COLD_CONSTCONST_HANDLER(195, ZEND_MATCH, CONST|TMPVARCV, CONST, JMP_ADDR)
{
	USE_OPLINE
	zval *op, *jump_zv;
	HashTable *jumptable;

	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	jumptable = Z_ARRVAL_P(GET_OP2_ZVAL_PTR(BP_VAR_R));

ZEND_VM_C_LABEL(match_try_again):
	if (Z_TYPE_P(op) == IS_LONG) {
		jump_zv = zend_hash_index_find(jumptable, Z_LVAL_P(op));
	} else if (Z_TYPE_P(op) == IS_STRING) {
		jump_zv = zend_hash_find_ex(jumptable, Z_STR_P(op), OP1_TYPE == IS_CONST);
	} else if (Z_TYPE_P(op) == IS_REFERENCE) {
		op = Z_REFVAL_P(op);
		ZEND_VM_C_GOTO(match_try_again);
	} else {
		if (UNEXPECTED((OP1_TYPE & IS_CV) && Z_TYPE_P(op) == IS_UNDEF)) {
			SAVE_OPLINE();
			op = ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
			ZEND_VM_C_GOTO(match_try_again);
		}

		ZEND_VM_C_GOTO(default_branch);
	}

	if (jump_zv != NULL) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, Z_LVAL_P(jump_zv));
		ZEND_VM_CONTINUE();
	} else {
ZEND_VM_C_LABEL(default_branch):
		/* default */
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}
}

ZEND_VM_COLD_CONST_HANDLER(197, ZEND_MATCH_ERROR, CONST|TMPVARCV, UNUSED)
{
	USE_OPLINE
	zval *op;

	SAVE_OPLINE();
	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	zend_match_unhandled_error(op);
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(189, ZEND_IN_ARRAY, CONST|TMP|VAR|CV, CONST, NUM)
{
	USE_OPLINE
	zval *op1;
	HashTable *ht = Z_ARRVAL_P(RT_CONSTANT(opline, opline->op2));
	zval *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		result = zend_hash_find_ex(ht, Z_STR_P(op1), OP1_TYPE == IS_CONST);
		if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
			zval_ptr_dtor_str(op1);
		}
		ZEND_VM_SMART_BRANCH(result, 0);
	}

	if (opline->extended_value) {
		if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
			result = zend_hash_index_find(ht, Z_LVAL_P(op1));
			ZEND_VM_SMART_BRANCH(result, 0);
		}
		SAVE_OPLINE();
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(op1) == IS_REFERENCE) {
			op1 = Z_REFVAL_P(op1);
			if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
				result = zend_hash_find(ht, Z_STR_P(op1));
				FREE_OP1();
				ZEND_VM_SMART_BRANCH(result, 0);
			} else if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
				result = zend_hash_index_find(ht, Z_LVAL_P(op1));
				FREE_OP1();
				ZEND_VM_SMART_BRANCH(result, 0);
			}
		} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
	} else if (Z_TYPE_P(op1) <= IS_FALSE) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			SAVE_OPLINE();
			ZVAL_UNDEFINED_OP1();
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
		result = zend_hash_find_known_hash(ht, ZSTR_EMPTY_ALLOC());
		ZEND_VM_SMART_BRANCH(result, 0);
	} else {
		zend_string *key;
		zval key_tmp;

		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(op1) == IS_REFERENCE) {
			op1 = Z_REFVAL_P(op1);
			if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
				result = zend_hash_find(ht, Z_STR_P(op1));
				FREE_OP1();
				ZEND_VM_SMART_BRANCH(result, 0);
			}
		}

		SAVE_OPLINE();
		ZEND_HASH_MAP_FOREACH_STR_KEY(ht, key) {
			ZVAL_STR(&key_tmp, key);
			if (zend_compare(op1, &key_tmp) == 0) {
				FREE_OP1();
				ZEND_VM_SMART_BRANCH(1, 1);
			}
		} ZEND_HASH_FOREACH_END();
	}
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(0, 1);
}

ZEND_VM_COLD_CONST_HANDLER(190, ZEND_COUNT, CONST|TMPVAR|CV, UNUSED)
{
	USE_OPLINE
	zval *op1;
	zend_long count;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	while (1) {
		if (Z_TYPE_P(op1) == IS_ARRAY) {
			count = zend_hash_num_elements(Z_ARRVAL_P(op1));
			break;
		} else if (Z_TYPE_P(op1) == IS_OBJECT) {
			zend_object *zobj = Z_OBJ_P(op1);

			/* first, we check if the handler is defined */
			if (zobj->handlers->count_elements) {
				if (SUCCESS == zobj->handlers->count_elements(zobj, &count)) {
					break;
				}
				if (UNEXPECTED(EG(exception))) {
					count = 0;
					break;
				}
			}

			/* if not and the object implements Countable we call its count() method */
			if (zend_class_implements_interface(zobj->ce, zend_ce_countable)) {
				zval retval;

				zend_function *count_fn = zend_hash_find_ptr(&zobj->ce->function_table, ZSTR_KNOWN(ZEND_STR_COUNT));
				zend_call_known_instance_method_with_0_params(count_fn, zobj, &retval);
				count = zval_get_long(&retval);
				zval_ptr_dtor(&retval);
				break;
			}

			/* If There's no handler and it doesn't implement Countable then emit a TypeError */
		} else if ((OP1_TYPE & (IS_VAR|IS_CV)) != 0 && Z_TYPE_P(op1) == IS_REFERENCE) {
			op1 = Z_REFVAL_P(op1);
			continue;
		} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			ZVAL_UNDEFINED_OP1();
		}
		count = 0;
		zend_type_error("%s(): Argument #1 ($value) must be of type Countable|array, %s given", opline->extended_value ? "sizeof" : "count", zend_zval_value_name(op1));
		break;
	}

	ZVAL_LONG(EX_VAR(opline->result.var), count);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_COUNT, (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) == MAY_BE_ARRAY, ZEND_COUNT_ARRAY, CV|TMPVAR, UNUSED)
{
	USE_OPLINE
	zend_array *ht = Z_ARRVAL_P(GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R));
	ZVAL_LONG(EX_VAR(opline->result.var), zend_hash_num_elements(ht));
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR) && !(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
		SAVE_OPLINE();
		zend_array_destroy(ht);
		if (EG(exception)) {
			HANDLE_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(191, ZEND_GET_CLASS, UNUSED|CONST|TMPVAR|CV, UNUSED)
{
	USE_OPLINE

	if (OP1_TYPE == IS_UNUSED) {
		SAVE_OPLINE();
		if (UNEXPECTED(!EX(func)->common.scope)) {
			zend_throw_error(NULL, "get_class() without arguments must be called from within a class");
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		} else {
			zend_error(E_DEPRECATED, "Calling get_class() without arguments is deprecated");
			ZVAL_STR_COPY(EX_VAR(opline->result.var), EX(func)->common.scope->name);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
			ZEND_VM_NEXT_OPCODE();
		}
	} else {
		zval *op1;

		SAVE_OPLINE();
		op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
		while (1) {
			if (Z_TYPE_P(op1) == IS_OBJECT) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_OBJCE_P(op1)->name);
			} else if ((OP1_TYPE & (IS_VAR|IS_CV)) != 0 && Z_TYPE_P(op1) == IS_REFERENCE) {
				op1 = Z_REFVAL_P(op1);
				continue;
			} else {
				if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
					ZVAL_UNDEFINED_OP1();
				}
				zend_type_error("get_class(): Argument #1 ($object) must be of type object, %s given", zend_zval_value_name(op1));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
			break;
		}
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
}

ZEND_VM_HANDLER(192, ZEND_GET_CALLED_CLASS, UNUSED, UNUSED)
{
	USE_OPLINE

	if (Z_TYPE(EX(This)) == IS_OBJECT) {
		ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_OBJCE(EX(This))->name);
	} else if (Z_CE(EX(This))) {
		ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_CE(EX(This))->name);
	} else {
		ZEND_ASSERT(!EX(func)->common.scope);
		SAVE_OPLINE();
		zend_throw_error(NULL, "get_called_class() must be called from within a class");
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		HANDLE_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(193, ZEND_GET_TYPE, CONST|TMP|VAR|CV, UNUSED)
{
	USE_OPLINE
	zval *op1;
	zend_string *type;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	type = zend_zval_get_legacy_type(op1);
	if (EXPECTED(type)) {
		ZVAL_INTERNED_STR(EX_VAR(opline->result.var), type);
	} else {
		ZVAL_STRING(EX_VAR(opline->result.var), "unknown type");
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(171, ZEND_FUNC_NUM_ARGS, UNUSED, UNUSED)
{
	USE_OPLINE

	ZVAL_LONG(EX_VAR(opline->result.var), EX_NUM_ARGS());
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(172, ZEND_FUNC_GET_ARGS, UNUSED|CONST, UNUSED)
{
	USE_OPLINE
	zend_array *ht;
	uint32_t arg_count, result_size, skip;

	arg_count = EX_NUM_ARGS();
	if (OP1_TYPE == IS_CONST) {
		skip = Z_LVAL_P(RT_CONSTANT(opline, opline->op1));
		if (arg_count < skip) {
			result_size = 0;
		} else {
			result_size = arg_count - skip;
		}
	} else {
		skip = 0;
		result_size = arg_count;
	}

	if (result_size) {
		SAVE_OPLINE();
		uint32_t first_extra_arg = EX(func)->op_array.num_args;

		ht = zend_new_array(result_size);
		ZVAL_ARR(EX_VAR(opline->result.var), ht);
		zend_hash_real_init_packed(ht);
		ZEND_HASH_FILL_PACKED(ht) {
			zval *p, *q;
			uint32_t i = skip;
			p = EX_VAR_NUM(i);
			if (arg_count > first_extra_arg) {
				while (i < first_extra_arg) {
					q = p;
					if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
						ZVAL_DEREF(q);
						if (Z_OPT_REFCOUNTED_P(q)) {
							Z_ADDREF_P(q);
						}
						ZEND_HASH_FILL_SET(q);
					} else {
						ZEND_HASH_FILL_SET_NULL();
					}
					ZEND_HASH_FILL_NEXT();
					p++;
					i++;
				}
				if (skip < first_extra_arg) {
					skip = 0;
				} else {
					skip -= first_extra_arg;
				}
				p = EX_VAR_NUM(EX(func)->op_array.last_var + EX(func)->op_array.T + skip);
			}
			while (i < arg_count) {
				q = p;
				if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
					ZVAL_DEREF(q);
					if (Z_OPT_REFCOUNTED_P(q)) {
						Z_ADDREF_P(q);
					}
					ZEND_HASH_FILL_SET(q);
				} else {
					ZEND_HASH_FILL_SET_NULL();
				}
				ZEND_HASH_FILL_NEXT();
				p++;
				i++;
			}
		} ZEND_HASH_FILL_END();
		ht->nNumOfElements = result_size;
	} else {
		ZVAL_EMPTY_ARRAY(EX_VAR(opline->result.var));
	}
	ZEND_VM_NEXT_OPCODE();
}

/* Contrary to what its name indicates, ZEND_COPY_TMP may receive and define references. */
ZEND_VM_HANDLER(167, ZEND_COPY_TMP, TMPVAR, UNUSED)
{
	USE_OPLINE
	zval *value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zval *result = EX_VAR(opline->result.var);
	ZVAL_COPY(result, value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(202, ZEND_CALLABLE_CONVERT, UNUSED, UNUSED)
{
	USE_OPLINE
	zend_execute_data *call = EX(call);

	zend_closure_from_frame(EX_VAR(opline->result.var), call);

	if (ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS) {
		OBJ_RELEASE(Z_OBJ(call->This));
	}

	EX(call) = call->prev_execute_data;

	zend_vm_stack_free_call_frame(call);

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_JMP, (OP_JMP_ADDR(op, op->op1) > op), ZEND_JMP_FORWARD, JMP_ADDR, ANY)
{
	USE_OPLINE

	OPLINE = OP_JMP_ADDR(opline, opline->op1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_ADD, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_ADD_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) + Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_ADD, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_ADD_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	fast_long_add_function(result, op1, op2);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_ADD, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_ADD_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SUB, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_SUB_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) - Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SUB, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_SUB_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	fast_long_sub_function(result, op1, op2);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SUB, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_SUB_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_MUL, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_MUL_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) * Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_MUL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_MUL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;
	zend_long overflow;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1), Z_LVAL_P(op2), Z_LVAL_P(result), Z_DVAL_P(result), overflow);
	Z_TYPE_INFO_P(result) = overflow ? IS_DOUBLE : IS_LONG;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_MUL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_MUL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL|ZEND_IS_IDENTICAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) == Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL|ZEND_IS_IDENTICAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) == Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL|ZEND_IS_NOT_IDENTICAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_NOT_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) != Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL|ZEND_IS_NOT_IDENTICAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_NOT_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) != Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_IDENTICAL, op->op1_type == IS_CV && (op->op2_type & (IS_CONST|IS_CV)) && !(op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) && !(op2_info & (MAY_BE_UNDEF|MAY_BE_REF)), ZEND_IS_IDENTICAL_NOTHROW, CV, CONST|CV, SPEC(COMMUTATIVE))
{
	/* This is declared below the specializations for MAY_BE_LONG/MAY_BE_DOUBLE so those will be used instead if possible. */
	/* This optimizes $x === SOME_CONST_EXPR and $x === $y for non-refs and non-undef, which can't throw. */
	/* (Infinite recursion when comparing arrays is an uncatchable fatal error) */
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = fast_is_identical_function(op1, op2);
	/* Free is a no-op for const/cv */
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_NOT_IDENTICAL, op->op1_type == IS_CV && (op->op2_type & (IS_CONST|IS_CV)) && !(op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) && !(op2_info & (MAY_BE_UNDEF|MAY_BE_REF)), ZEND_IS_NOT_IDENTICAL_NOTHROW, CV, CONST|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = fast_is_identical_function(op1, op2);
	/* Free is a no-op for const/cv */
	ZEND_VM_SMART_BRANCH(!result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) < Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) < Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_OR_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) <= Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_OR_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	bool result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) <= Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG_NO_OVERFLOW, CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	Z_LVAL_P(var_ptr)++;
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG, CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	fast_long_increment_function(var_ptr);
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG_NO_OVERFLOW, CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	Z_LVAL_P(var_ptr)--;
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG, CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	fast_long_decrement_function(var_ptr);
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG_NO_OVERFLOW, CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)++;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_INC, (op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG, CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_increment_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG_NO_OVERFLOW, CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)--;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG, CV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_decrement_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, (op1_info == MAY_BE_LONG), ZEND_QM_ASSIGN_LONG, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(value));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, (op1_info == MAY_BE_DOUBLE), ZEND_QM_ASSIGN_DOUBLE, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_DOUBLE(EX_VAR(opline->result.var), Z_DVAL_P(value));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, ((op->op1_type == IS_CONST) ? !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)) : (!(op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))))), ZEND_QM_ASSIGN_NOREF, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_FETCH_DIM_R, (!(op2_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))), ZEND_FETCH_DIM_R_INDEX, CONST|TMPVAR|CV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *container, *dim, *value;
	zend_long offset;
	HashTable *ht;

	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(fetch_dim_r_index_array):
		if (EXPECTED(Z_TYPE_P(dim) == IS_LONG)) {
			offset = Z_LVAL_P(dim);
		} else {
			SAVE_OPLINE();
			zend_fetch_dimension_address_read_R(container, dim, OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
			FREE_OP1();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
		ht = Z_ARRVAL_P(container);
		ZEND_HASH_INDEX_FIND(ht, offset, value, ZEND_VM_C_LABEL(fetch_dim_r_index_undef));
		ZVAL_COPY_DEREF(EX_VAR(opline->result.var), value);
		if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
			SAVE_OPLINE();
			FREE_OP1();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			ZEND_VM_NEXT_OPCODE();
		}
	} else if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
		container = Z_REFVAL_P(container);
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			ZEND_VM_C_GOTO(fetch_dim_r_index_array);
		} else {
			ZEND_VM_C_GOTO(fetch_dim_r_index_slow);
		}
	} else {
ZEND_VM_C_LABEL(fetch_dim_r_index_slow):
		SAVE_OPLINE();
		if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
			dim++;
		}
		zend_fetch_dimension_address_read_R_slow(container, dim OPLINE_CC EXECUTE_DATA_CC);
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

ZEND_VM_C_LABEL(fetch_dim_r_index_undef):
	ZVAL_NULL(EX_VAR(opline->result.var));
	SAVE_OPLINE();
	zend_undefined_offset(offset);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAR, op->op2_type == IS_UNUSED && (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_SIMPLE, CV|VAR, NUM)
{
	USE_OPLINE
	zval *varptr, *arg;

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		ZVAL_COPY_VALUE(arg, varptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAR_EX, op->op2_type == IS_UNUSED && op->op2.num <= MAX_ARG_FLAG_NUM && (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_EX_SIMPLE, CV|VAR, UNUSED|NUM)
{
	USE_OPLINE
	zval *varptr, *arg;
	uint32_t arg_num = opline->op2.num;

	if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		ZVAL_COPY_VALUE(arg, varptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAL, op->op1_type == IS_CONST && op->op2_type == IS_UNUSED && !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)), ZEND_SEND_VAL_SIMPLE, CONST, NUM)
{
	USE_OPLINE
	zval *value, *arg;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAL_EX, op->op2_type == IS_UNUSED && op->op2.num <= MAX_ARG_FLAG_NUM && op->op1_type == IS_CONST && !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)), ZEND_SEND_VAL_EX_SIMPLE, CONST, NUM)
{
	USE_OPLINE
	zval *value, *arg;
	uint32_t arg_num = opline->op2.num;

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	if (QUICK_ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_cannot_pass_by_ref_helper, _arg_num, arg_num, _arg, arg);
	}
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	ZVAL_COPY_VALUE(arg, value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_FE_FETCH_R, op->op2_type == IS_CV && (op1_info & (MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_ARRAY, ZEND_FE_FETCH_R_SIMPLE, VAR, CV, JMP_ADDR, SPEC(RETVAL))
{
	USE_OPLINE
	zval *array;
	zval *value, *variable_ptr;
	uint32_t value_type;
	HashTable *fe_ht;
	HashPosition pos;

	array = EX_VAR(opline->op1.var);
	SAVE_OPLINE();
	fe_ht = Z_ARRVAL_P(array);
	pos = Z_FE_POS_P(array);
	if (HT_IS_PACKED(fe_ht)) {
		value = fe_ht->arPacked + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
			value_type = Z_TYPE_INFO_P(value);
			ZEND_ASSERT(value_type != IS_INDIRECT);
			if (EXPECTED(value_type != IS_UNDEF)) {
				break;
			}
			pos++;
			value++;
		}
		Z_FE_POS_P(array) = pos + 1;
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_LONG(EX_VAR(opline->result.var), pos);
		}
	} else {
		Bucket *p;

		p = fe_ht->arData + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
			pos++;
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			ZEND_ASSERT(value_type != IS_INDIRECT);
			if (EXPECTED(value_type != IS_UNDEF)) {
				break;
			}
			p++;
		}
		Z_FE_POS_P(array) = pos;
		if (RETURN_VALUE_USED(opline)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
	}

	variable_ptr = EX_VAR(opline->op2.var);
	zend_assign_to_variable(variable_ptr, value, IS_CV, EX_USES_STRICT_TYPES());
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_DEFINE_OP(137, ZEND_OP_DATA);

ZEND_VM_HELPER(zend_interrupt_helper, ANY, ANY)
{
	zend_atomic_bool_store_ex(&EG(vm_interrupt), false);
	SAVE_OPLINE();
	if (zend_atomic_bool_load_ex(&EG(timed_out))) {
		zend_timeout();
	} else if (zend_interrupt_function) {
		zend_interrupt_function(execute_data);
		if (EG(exception)) {
			/* We have to UNDEF result, because ZEND_HANDLE_EXCEPTION is going to free it */
			const zend_op *throw_op = EG(opline_before_exception);

			if (throw_op
			 && throw_op->result_type & (IS_TMP_VAR|IS_VAR)
			 && throw_op->opcode != ZEND_ADD_ARRAY_ELEMENT
			 && throw_op->opcode != ZEND_ADD_ARRAY_UNPACK
			 && throw_op->opcode != ZEND_ROPE_INIT
			 && throw_op->opcode != ZEND_ROPE_ADD) {
				ZVAL_UNDEF(ZEND_CALL_VAR(EG(current_execute_data), throw_op->result.var));

			}
		}
		ZEND_VM_ENTER();
	}
	ZEND_VM_CONTINUE();
}
