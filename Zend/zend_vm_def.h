/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_VM_COLD_CONSTCONST_HANDLER(1, ZEND_ADD, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			fast_long_add_function(result, op1, op2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) + Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) + ((double)Z_LVAL_P(op2)));
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	add_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(2, ZEND_SUB, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			fast_long_sub_function(result, op1, op2);
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) - Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) - ((double)Z_LVAL_P(op2)));
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	sub_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(3, ZEND_MUL, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			zend_long overflow;

			result = EX_VAR(opline->result.var);
			ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1), Z_LVAL_P(op2), Z_LVAL_P(result), Z_DVAL_P(result), overflow);
			Z_TYPE_INFO_P(result) = overflow ? IS_DOUBLE : IS_LONG;
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) * Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		}
	} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
			ZEND_VM_NEXT_OPCODE();
		} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
			result = EX_VAR(opline->result.var);
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) * ((double)Z_LVAL_P(op2)));
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	mul_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(4, ZEND_DIV, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	fast_div_function(EX_VAR(opline->result.var), op1, op2);
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

ZEND_VM_COLD_CONSTCONST_HANDLER(5, ZEND_MOD, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
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

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	mod_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(6, ZEND_SL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)
			&& EXPECTED((zend_ulong)Z_LVAL_P(op2) < SIZEOF_ZEND_LONG * 8)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) << Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	shift_left_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(7, ZEND_SR, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)
			&& EXPECTED((zend_ulong)Z_LVAL_P(op2) < SIZEOF_ZEND_LONG * 8)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) >> Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	shift_right_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(166, ZEND_POW, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
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
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if ((OP1_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op1) == IS_STRING)) &&
	    (OP2_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op2) == IS_STRING))) {
		zend_string *op1_str = Z_STR_P(op1);
		zend_string *op2_str = Z_STR_P(op2);
		zend_string *str;

		if (OP1_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
			if (OP2_TYPE == IS_CONST || OP2_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op2_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
			}
			FREE_OP1();
		} else if (OP2_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op1_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
			}
			FREE_OP2();
		} else if (OP1_TYPE != IS_CONST && OP1_TYPE != IS_CV &&
		    !ZSTR_IS_INTERNED(op1_str) && GC_REFCOUNT(op1_str) == 1) {
		    size_t len = ZSTR_LEN(op1_str);

			str = zend_string_extend(op1_str, len + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str) + len, ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			FREE_OP2();
		} else {
			str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
			memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			FREE_OP1();
			FREE_OP2();
		}
		ZEND_VM_NEXT_OPCODE();
	} else {
		SAVE_OPLINE();

		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
		}
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
			op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
		}
		concat_function(EX_VAR(opline->result.var), op1, op2);
		FREE_OP1();
		FREE_OP2();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
}

ZEND_VM_COLD_CONSTCONST_HANDLER(15, ZEND_IS_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;
	int result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
	result = fast_is_identical_function(op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(16, ZEND_IS_NOT_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;
	int result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
	result = fast_is_not_identical_function(op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(17, ZEND_IS_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	do {
		int result;

		if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_LVAL_P(op1) == Z_LVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = ((double)Z_LVAL_P(op1) == Z_DVAL_P(op2));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = (Z_DVAL_P(op1) == Z_DVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_DVAL_P(op1) == ((double)Z_LVAL_P(op2)));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
				result = zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
				FREE_OP1();
				FREE_OP2();
			} else {
				break;
			}
		} else {
			break;
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	} while (0);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	result = EX_VAR(opline->result.var);
	compare_function(result, op1, op2);
	ZVAL_BOOL(result, Z_LVAL_P(result) == 0);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(18, ZEND_IS_NOT_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	do {
		int result;

		if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_LVAL_P(op1) != Z_LVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = ((double)Z_LVAL_P(op1) != Z_DVAL_P(op2));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = (Z_DVAL_P(op1) != Z_DVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_DVAL_P(op1) != ((double)Z_LVAL_P(op2)));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
				result = !zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
				FREE_OP1();
				FREE_OP2();
			} else {
				break;
			}
		} else {
			break;
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	} while (0);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	result = EX_VAR(opline->result.var);
	compare_function(result, op1, op2);
	ZVAL_BOOL(result, Z_LVAL_P(result) != 0);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(19, ZEND_IS_SMALLER, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	do {
		int result;

		if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
			if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
				result = (Z_LVAL_P(op1) < Z_LVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
				result = ((double)Z_LVAL_P(op1) < Z_DVAL_P(op2));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
			if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
				result = (Z_DVAL_P(op1) < Z_DVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
				result = (Z_DVAL_P(op1) < ((double)Z_LVAL_P(op2)));
			} else {
				break;
			}
		} else {
			break;
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	} while (0);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	result = EX_VAR(opline->result.var);
	compare_function(result, op1, op2);
	ZVAL_BOOL(result, Z_LVAL_P(result) < 0);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(20, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	do {
		int result;

		if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
			if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
				result = (Z_LVAL_P(op1) <= Z_LVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
				result = ((double)Z_LVAL_P(op1) <= Z_DVAL_P(op2));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_DOUBLE)) {
			if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_DOUBLE)) {
				result = (Z_DVAL_P(op1) <= Z_DVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
				result = (Z_DVAL_P(op1) <= ((double)Z_LVAL_P(op2)));
			} else {
				break;
			}
		} else {
			break;
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	} while (0);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	result = EX_VAR(opline->result.var);
	compare_function(result, op1, op2);
	ZVAL_BOOL(result, Z_LVAL_P(result) <= 0);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(170, ZEND_SPACESHIP, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	compare_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(9, ZEND_BW_OR, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) | Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	bitwise_or_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(10, ZEND_BW_AND, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) & Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	bitwise_and_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(11, ZEND_BW_XOR, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)
			&& EXPECTED(Z_TYPE_INFO_P(op2) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(op1) ^ Z_LVAL_P(op2));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	bitwise_xor_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(14, ZEND_BOOL_XOR, CONST|TMPVAR|CV, CONST|TMPVAR|CV, SPEC(COMMUTATIVE))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	boolean_xor_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(12, ZEND_BW_NOT, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *op1;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_INFO_P(op1) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), ~Z_LVAL_P(op1));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	bitwise_not_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R));
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(13, ZEND_BOOL_NOT, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *val;
	zend_free_op free_op1;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
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
	if ((opline+1)->opcode == ZEND_OP_DATA) {
		FREE_UNFETCHED_OP_DATA();
	}
	FREE_UNFETCHED_OP2();
	UNDEF_RESULT();
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_abstract_method_helper, ANY, ANY, zend_function *fbc)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot call abstract method %s::%s()", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
	UNDEF_RESULT();
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_undefined_function_helper, ANY, ANY, zval *function_name)
{
	SAVE_OPLINE();
	zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(function_name));
	HANDLE_EXCEPTION();
}

ZEND_VM_HELPER(zend_binary_assign_op_obj_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data1;
	zval *object;
	zval *property;
	zval *value;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);

		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object)) {
				object = Z_REFVAL_P(object);
				if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
					ZEND_VM_C_GOTO(assign_op_object);
				}
			}
			if (UNEXPECTED(!make_real_object(object, property OPLINE_CC EXECUTE_DATA_CC))) {
				break;
			}
		}

		/* here we are sure we are dealing with an object */
ZEND_VM_C_LABEL(assign_op_object):
		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR((opline+1)->extended_value) : NULL))) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			} else {
				ZVAL_DEREF(zptr);

				binary_op(zptr, zptr, value);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
				}
			}
		} else {
			zend_assign_op_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR((opline+1)->extended_value) : NULL), value, binary_op OPLINE_CC EXECUTE_DATA_CC);
		}
	} while (0);

	FREE_OP(free_op_data1);
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HELPER(zend_binary_assign_op_dim_helper, VAR|CV, CONST|TMPVAR|UNUSED|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data1;
	zval *var_ptr;
	zval *value, *container, *dim;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(assign_dim_op_array):
		SEPARATE_ARRAY(container);
ZEND_VM_C_LABEL(assign_dim_op_new_array):
		dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (OP2_TYPE == IS_UNUSED) {
			var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			if (UNEXPECTED(!var_ptr)) {
				zend_cannot_add_element();
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
		} else {
			if (OP2_TYPE == IS_CONST) {
				var_ptr = zend_fetch_dimension_address_inner_RW_CONST(Z_ARRVAL_P(container), dim EXECUTE_DATA_CC);
			} else {
				var_ptr = zend_fetch_dimension_address_inner_RW(Z_ARRVAL_P(container), dim EXECUTE_DATA_CC);
			}
			if (UNEXPECTED(!var_ptr)) {
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
			ZVAL_DEREF(var_ptr);
		}

		value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);

		binary_op(var_ptr, var_ptr, value);

		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
		}
	} else {
		if (EXPECTED(Z_ISREF_P(container))) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(assign_dim_op_array);
			}
		} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(container) == IS_UNDEF)) {
			container = GET_OP1_UNDEF_CV(container, BP_VAR_RW);
ZEND_VM_C_LABEL(assign_dim_op_convert_to_array):
			ZVAL_ARR(container, zend_new_array(8));
			ZEND_VM_C_GOTO(assign_dim_op_new_array);
		}

		dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
			value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);
			if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			zend_binary_assign_op_obj_dim(container, dim, value, UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL, binary_op EXECUTE_DATA_CC);
		} else {
			if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
				if (OP2_TYPE == IS_UNUSED) {
					zend_use_new_element_for_string();
				} else {
					zend_check_string_offset(dim, BP_VAR_RW EXECUTE_DATA_CC);
					zend_wrong_string_offset(EXECUTE_DATA_C);
				}
				UNDEF_RESULT();
			} else if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
				ZEND_VM_C_GOTO(assign_dim_op_convert_to_array);
			} else {
				if (UNEXPECTED(OP1_TYPE != IS_VAR || EXPECTED(!Z_ISERROR_P(container)))) {
					zend_use_scalar_as_array();
				}
ZEND_VM_C_LABEL(assign_dim_op_ret_null):
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			}
			value = get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, &free_op_data1);
		}
	}

	FREE_OP2();
	FREE_OP(free_op_data1);
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HELPER(zend_binary_assign_op_simple_helper, VAR|CV, CONST|TMPVAR|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *var_ptr;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(var_ptr))) {
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	} else {
		ZVAL_DEREF(var_ptr);

		binary_op(var_ptr, var_ptr, value);

		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
		}
	}

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_INLINE_HELPER(zend_binary_assign_op_helper, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, SPEC(DIM_OBJ), binary_op_type binary_op)
{
#if defined(ZEND_VM_SPEC) && OP2_TYPE == IS_UNUSED
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, binary_op);
#else
# if !defined(ZEND_VM_SPEC) || OP1_TYPE != IS_UNUSED
#  if !defined(ZEND_VM_SPEC)
	/* opline->extended_value checks are specialized, don't need opline */
	USE_OPLINE
#  endif

	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_simple_helper, binary_op, binary_op);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, binary_op);
	}
# endif

	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, binary_op);
#endif
}

ZEND_VM_HANDLER(23, ZEND_ASSIGN_ADD, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, add_function);
}

ZEND_VM_HANDLER(24, ZEND_ASSIGN_SUB, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, sub_function);
}

ZEND_VM_HANDLER(25, ZEND_ASSIGN_MUL, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, mul_function);
}

ZEND_VM_HANDLER(26, ZEND_ASSIGN_DIV, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, div_function);
}

ZEND_VM_HANDLER(27, ZEND_ASSIGN_MOD, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, mod_function);
}

ZEND_VM_HANDLER(28, ZEND_ASSIGN_SL, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, shift_left_function);
}

ZEND_VM_HANDLER(29, ZEND_ASSIGN_SR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, shift_right_function);
}

ZEND_VM_HANDLER(30, ZEND_ASSIGN_CONCAT, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, concat_function);
}

ZEND_VM_HANDLER(31, ZEND_ASSIGN_BW_OR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_or_function);
}

ZEND_VM_HANDLER(32, ZEND_ASSIGN_BW_AND, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_and_function);
}

ZEND_VM_HANDLER(33, ZEND_ASSIGN_BW_XOR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_xor_function);
}

ZEND_VM_HANDLER(167, ZEND_ASSIGN_POW, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ|CACHE_SLOT, SPEC(DIM_OBJ))
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, pow_function);
}

ZEND_VM_HELPER(zend_pre_incdec_property_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, int inc)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object)) {
				object = Z_REFVAL_P(object);
				if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
					ZEND_VM_C_GOTO(pre_incdec_object);
				}
			}
			if (UNEXPECTED(!make_real_object(object, property OPLINE_CC EXECUTE_DATA_CC))) {
				break;
			}
		}

		/* here we are sure we are dealing with an object */
ZEND_VM_C_LABEL(pre_incdec_object):
		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL))) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			} else {
				if (EXPECTED(Z_TYPE_P(zptr) == IS_LONG)) {
					if (inc) {
						fast_long_increment_function(zptr);
					} else {
						fast_long_decrement_function(zptr);
					}
				} else {
					ZVAL_DEREF(zptr);

					if (inc) {
						increment_function(zptr);
					} else {
						decrement_function(zptr);
					}
				}
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
				}
			}
		} else {
			zend_pre_incdec_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), inc OPLINE_CC EXECUTE_DATA_CC);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(132, ZEND_PRE_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_pre_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(133, ZEND_PRE_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_pre_incdec_property_helper, inc, 0);
}

ZEND_VM_HELPER(zend_post_incdec_property_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, int inc)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			if (Z_ISREF_P(object)) {
				object = Z_REFVAL_P(object);
				if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
					ZEND_VM_C_GOTO(post_incdec_object);
				}
			}
			if (UNEXPECTED(!make_real_object(object, property OPLINE_CC EXECUTE_DATA_CC))) {
				break;
			}
		}

		/* here we are sure we are dealing with an object */
ZEND_VM_C_LABEL(post_incdec_object):
		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL))) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			} else {
				if (EXPECTED(Z_TYPE_P(zptr) == IS_LONG)) {
					ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(zptr));
					if (inc) {
						fast_long_increment_function(zptr);
					} else {
						fast_long_decrement_function(zptr);
					}
				} else {
					ZVAL_DEREF(zptr);
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
					if (inc) {
						increment_function(zptr);
					} else {
						decrement_function(zptr);
					}
				}
			}
		} else {
			zend_post_incdec_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), inc OPLINE_CC EXECUTE_DATA_CC);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(134, ZEND_POST_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_post_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(135, ZEND_POST_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_post_incdec_property_helper, inc, 0);
}

ZEND_VM_HANDLER(34, ZEND_PRE_INC, VAR|CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_increment_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(var_ptr))) {
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		var_ptr = GET_OP1_UNDEF_CV(var_ptr, BP_VAR_RW);
	}
	ZVAL_DEREF(var_ptr);

	increment_function(var_ptr);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_decrement_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(var_ptr))) {
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		var_ptr = GET_OP1_UNDEF_CV(var_ptr, BP_VAR_RW);
	}
	ZVAL_DEREF(var_ptr);

	decrement_function(var_ptr);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(36, ZEND_POST_INC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_increment_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(var_ptr))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		var_ptr = GET_OP1_UNDEF_CV(var_ptr, BP_VAR_RW);
	}
	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);

	increment_function(var_ptr);

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(37, ZEND_POST_DEC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_decrement_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(var_ptr))) {
		ZVAL_NULL(EX_VAR(opline->result.var));
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(var_ptr) == IS_UNDEF)) {
		var_ptr = GET_OP1_UNDEF_CV(var_ptr, BP_VAR_RW);
	}
	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);

	decrement_function(var_ptr);

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(40, ZEND_ECHO, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
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
			GET_OP1_UNDEF_CV(z, BP_VAR_R);
		}
		zend_string_release_ex(str, 0);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_fetch_var_address_helper, CONST|TMPVAR|CV, UNUSED, int type)
{
	USE_OPLINE
	zend_free_op free_op1;
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
			GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		name = zval_get_tmp_string(varname, &tmp_name);
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
		} else if (type == BP_VAR_IS) {
			retval = &EG(uninitialized_zval);
		} else {
			zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
			if (type == BP_VAR_RW) {
				retval = zend_hash_update(target_symbol_table, name, &EG(uninitialized_zval));
			} else {
				retval = &EG(uninitialized_zval);
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
			} else if (type == BP_VAR_IS) {
				retval = &EG(uninitialized_zval);
			} else {
				zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
				if (type == BP_VAR_RW) {
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

ZEND_VM_HELPER(zend_fetch_static_prop_helper, CONST|TMPVAR|CV, UNUSED|CONST|VAR, int type)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varname;
	zval *retval;
	zend_string *name, *tmp_name;
	zend_class_entry *ce;

	SAVE_OPLINE();

	do {
		if (OP2_TYPE == IS_CONST) {
			if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(opline->extended_value)) != NULL)) {
				retval = CACHED_PTR(opline->extended_value + sizeof(void*));
				break;
			} else {
				zval *class_name = RT_CONSTANT(opline, opline->op2);

				if (UNEXPECTED((ce = CACHED_PTR(opline->extended_value)) == NULL)) {
					ce = zend_fetch_class_by_name(Z_STR_P(class_name), class_name + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
					if (UNEXPECTED(ce == NULL)) {
						FREE_UNFETCHED_OP1();
						retval = NULL;
						break;
					}
					if (OP1_TYPE != IS_CONST) {
						CACHE_PTR(opline->extended_value, ce);
					}
				}
			}
		} else {
			if (OP2_TYPE == IS_UNUSED) {
				ce = zend_fetch_class(NULL, opline->op2.num);
				if (UNEXPECTED(ce == NULL)) {
					FREE_UNFETCHED_OP1();
					retval = NULL;
					break;
				}
			} else {
				ce = Z_CE_P(EX_VAR(opline->op2.var));
			}
			if (OP1_TYPE == IS_CONST &&
			    EXPECTED(CACHED_PTR(opline->extended_value) == ce)) {
				retval = CACHED_PTR(opline->extended_value + sizeof(void*));
				break;
			}
		}

		varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
		if (OP1_TYPE == IS_CONST) {
			name = Z_STR_P(varname);
		} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
			name = Z_STR_P(varname);
			tmp_name = NULL;
		} else {
			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
				zval_undefined_cv(EX(opline)->op1.var EXECUTE_DATA_CC);
			}
			name = zval_get_tmp_string(varname, &tmp_name);
		}

		retval = zend_std_get_static_property(ce, name, type == BP_VAR_IS);

		if (OP1_TYPE != IS_CONST) {
			zend_tmp_string_release(tmp_name);
		}

		if (OP1_TYPE == IS_CONST && EXPECTED(retval)) {
			CACHE_POLYMORPHIC_PTR(opline->extended_value, ce, retval);
		}

		FREE_OP1();
	} while (0);

	if (UNEXPECTED(retval == NULL)) {
		if (EG(exception)) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		} else {
			ZEND_ASSERT(type == BP_VAR_IS);
			retval = &EG(uninitialized_zval);
		}
	}

	if (type == BP_VAR_R || type == BP_VAR_IS) {
		ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
	} else {
		ZVAL_INDIRECT(EX_VAR(opline->result.var), retval);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(173, ZEND_FETCH_STATIC_PROP_R, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(174, ZEND_FETCH_STATIC_PROP_W, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(175, ZEND_FETCH_STATIC_PROP_RW, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(177, ZEND_FETCH_STATIC_PROP_FUNC_ARG, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	int fetch_type =
		(UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) ?
			BP_VAR_W : BP_VAR_R;
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, fetch_type);
}

ZEND_VM_HANDLER(178, ZEND_FETCH_STATIC_PROP_UNSET, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(176, ZEND_FETCH_STATIC_PROP_IS, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_IS);
}

ZEND_VM_COLD_CONSTCONST_HANDLER(81, ZEND_FETCH_DIM_R, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container, *dim, *value, *result;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE != IS_CONST) {
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(fetch_dim_r_array):
			value = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, OP2_TYPE, BP_VAR_R EXECUTE_DATA_CC);
			result = EX_VAR(opline->result.var);
			ZVAL_COPY_DEREF(result, value);
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
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	zend_fetch_dimension_address_W(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		zval *result = EX_VAR(opline->result.var);
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	zend_fetch_dimension_address_RW(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		zval *result = EX_VAR(opline->result.var);
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(90, ZEND_FETCH_DIM_IS, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
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
	FREE_UNFETCHED_OP2();
	FREE_UNFETCHED_OP1();
	ZVAL_UNDEF(EX_VAR(opline->result.var));
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_HELPER(zend_use_undef_in_read_context_helper, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot use [] for reading");
	FREE_UNFETCHED_OP2();
	FREE_UNFETCHED_OP1();
	ZVAL_UNDEF(EX_VAR(opline->result.var));
	HANDLE_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
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
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);
	zend_fetch_dimension_address_UNSET(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		zval *result = EX_VAR(opline->result.var);
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_OBJ_HANDLER(82, ZEND_FETCH_OBJ_R, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;
	void **cache_slot = NULL;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

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
				GET_OP1_UNDEF_CV(container, BP_VAR_R);
			}
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
				GET_OP2_UNDEF_CV(offset, BP_VAR_R);
			}
			ZEND_VM_C_GOTO(fetch_obj_r_no_object);
		} while (0);
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zval *retval;

		if (OP2_TYPE == IS_CONST) {
			cache_slot = CACHE_ADDR(opline->extended_value);

			if (EXPECTED(zobj->ce == CACHED_PTR_EX(cache_slot))) {
				uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);

				if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
					retval = OBJ_PROP(zobj, prop_offset);
					if (EXPECTED(Z_TYPE_INFO_P(retval) != IS_UNDEF)) {
						ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
						break;
					}
				} else if (EXPECTED(zobj->properties != NULL)) {
					if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
						uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

						if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
							Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

							if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
						        (EXPECTED(p->key == Z_STR_P(offset)) ||
						         (EXPECTED(p->h == ZSTR_H(Z_STR_P(offset))) &&
						          EXPECTED(p->key != NULL) &&
						          EXPECTED(zend_string_equal_content(p->key, Z_STR_P(offset)))))) {
								ZVAL_COPY_DEREF(EX_VAR(opline->result.var), &p->val);
								break;
							}
						}
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
					}
					retval = zend_hash_find_ex(zobj->properties, Z_STR_P(offset), 1);
					if (EXPECTED(retval)) {
						uintptr_t idx = (char*)retval - (char*)zobj->properties->arData;
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
						ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
						break;
					}
				}
			}
		} else if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(offset) == IS_UNDEF)) {
			GET_OP2_UNDEF_CV(offset, BP_VAR_R);
		}

		if (UNEXPECTED(zobj->handlers->read_property == NULL)) {
ZEND_VM_C_LABEL(fetch_obj_r_no_object):
			zend_wrong_property_read(offset);
			ZVAL_NULL(EX_VAR(opline->result.var));
		} else {
			retval = zobj->handlers->read_property(container, offset, BP_VAR_R, cache_slot, EX_VAR(opline->result.var));

			if (retval != EX_VAR(opline->result.var)) {
				ZVAL_COPY_DEREF(EX_VAR(opline->result.var), retval);
			} else if (UNEXPECTED(Z_ISREF_P(retval))) {
				zend_unwrap_reference(retval);
			}
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property, *container, *result;

	SAVE_OPLINE();

	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(result, container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), BP_VAR_W OPLINE_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property, *container, *result;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(result, container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), BP_VAR_RW OPLINE_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(91, ZEND_FETCH_OBJ_IS, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;
	void **cache_slot = NULL;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		do {
			if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
					break;
				}
			}
			ZEND_VM_C_GOTO(fetch_obj_is_no_object);
		} while (0);
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zval *retval;

		if (OP2_TYPE == IS_CONST) {
			cache_slot = CACHE_ADDR(opline->extended_value);

			if (EXPECTED(zobj->ce == CACHED_PTR_EX(cache_slot))) {
				uintptr_t prop_offset = (uintptr_t)CACHED_PTR_EX(cache_slot + 1);

				if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
					retval = OBJ_PROP(zobj, prop_offset);
					if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
						ZVAL_COPY(EX_VAR(opline->result.var), retval);
						break;
					}
				} else if (EXPECTED(zobj->properties != NULL)) {
					if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
						uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

						if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
							Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

							if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
						        (EXPECTED(p->key == Z_STR_P(offset)) ||
						         (EXPECTED(p->h == ZSTR_H(Z_STR_P(offset))) &&
						          EXPECTED(p->key != NULL) &&
						          EXPECTED(zend_string_equal_content(p->key, Z_STR_P(offset)))))) {
								ZVAL_COPY(EX_VAR(opline->result.var), &p->val);
								break;
							}
						}
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
					}
					retval = zend_hash_find_ex(zobj->properties, Z_STR_P(offset), 1);
					if (EXPECTED(retval)) {
						uintptr_t idx = (char*)retval - (char*)zobj->properties->arData;
						CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
						ZVAL_COPY(EX_VAR(opline->result.var), retval);
						break;
					}
				}
			}
		}

		if (UNEXPECTED(zobj->handlers->read_property == NULL)) {
ZEND_VM_C_LABEL(fetch_obj_is_no_object):
			ZVAL_NULL(EX_VAR(opline->result.var));
		} else {

			retval = zobj->handlers->read_property(container, offset, BP_VAR_IS, cache_slot, EX_VAR(opline->result.var));

			if (retval != EX_VAR(opline->result.var)) {
				ZVAL_COPY(EX_VAR(opline->result.var), retval);
			}
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, CONST|TMP|VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
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
	zend_free_op free_op1, free_op2;
	zval *container, *property, *result;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	zend_fetch_property_address(result, container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL), BP_VAR_UNSET OPLINE_CC);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR) {
		FREE_VAR_PTR_AND_EXTRACT_RESULT_IF_NECESSARY(free_op1, result);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_LIST_R, CONST|TMPVARCV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	zend_fetch_dimension_address_LIST_r(container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE OPLINE_CC EXECUTE_DATA_CC);
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(198, ZEND_FETCH_LIST_W, VAR, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
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

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data;
	zval *object, *property, *value, tmp;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		if (Z_ISREF_P(object)) {
			object = Z_REFVAL_P(object);
			if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
				ZEND_VM_C_GOTO(assign_object);
			}
		}
		if (UNEXPECTED(!make_real_object(object, property OPLINE_CC EXECUTE_DATA_CC))) {
			FREE_OP_DATA();
			ZEND_VM_C_GOTO(exit_assign_obj);
		}
	}

ZEND_VM_C_LABEL(assign_object):
	if (OP2_TYPE == IS_CONST &&
	    EXPECTED(Z_OBJCE_P(object) == CACHED_PTR(opline->extended_value))) {
		uintptr_t prop_offset = (uintptr_t)CACHED_PTR(opline->extended_value + sizeof(void*));
		zend_object *zobj = Z_OBJ_P(object);
		zval *property_val;

		if (EXPECTED(IS_VALID_PROPERTY_OFFSET(prop_offset))) {
			property_val = OBJ_PROP(zobj, prop_offset);
			if (Z_TYPE_P(property_val) != IS_UNDEF) {
ZEND_VM_C_LABEL(fast_assign_obj):
				value = zend_assign_to_variable(property_val, value, OP_DATA_TYPE);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), value);
				}
				ZEND_VM_C_GOTO(exit_assign_obj);
			}
		} else {
			if (EXPECTED(zobj->properties != NULL)) {
				if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
					if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
						GC_DELREF(zobj->properties);
					}
					zobj->properties = zend_array_dup(zobj->properties);
				}
				property_val = zend_hash_find_ex(zobj->properties, Z_STR_P(property), 1);
				if (property_val) {
					ZEND_VM_C_GOTO(fast_assign_obj);
				}
			}

			if (!zobj->ce->__set) {

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
				zend_hash_add_new(zobj->properties, Z_STR_P(property), value);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), value);
				}
				ZEND_VM_C_GOTO(exit_assign_obj);
			}
		}
	}

	if (!Z_OBJ_HT_P(object)->write_property) {
		zend_wrong_property_assignment(property OPLINE_CC EXECUTE_DATA_CC);
		FREE_OP_DATA();
		ZEND_VM_C_GOTO(exit_assign_obj);
	}

	if (OP_DATA_TYPE == IS_CV || OP_DATA_TYPE == IS_VAR) {
		ZVAL_DEREF(value);
	}

	Z_OBJ_HT_P(object)->write_property(object, property, value, (OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL);

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	}
	FREE_OP_DATA();
ZEND_VM_C_LABEL(exit_assign_obj):
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(147, ZEND_ASSIGN_DIM, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *object_ptr;
	zend_free_op free_op2, free_op_data;
	zval *value;
	zval *variable_ptr;
	zval *dim;

	SAVE_OPLINE();
	object_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
ZEND_VM_C_LABEL(try_assign_dim_array):
		SEPARATE_ARRAY(object_ptr);
		if (OP2_TYPE == IS_UNUSED) {
			value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);
			if (OP_DATA_TYPE == IS_CV || OP_DATA_TYPE == IS_VAR) {
				ZVAL_DEREF(value);
			}
			variable_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(object_ptr), value);
			if (UNEXPECTED(variable_ptr == NULL)) {
				FREE_OP_DATA();
				zend_cannot_add_element();
				ZEND_VM_C_GOTO(assign_dim_error);
			} else if (OP_DATA_TYPE == IS_CV) {
				if (Z_REFCOUNTED_P(value)) {
					Z_ADDREF_P(value);
				}
			} else if (OP_DATA_TYPE == IS_VAR) {
				if (value != free_op_data) {
					if (Z_REFCOUNTED_P(value)) {
						Z_ADDREF_P(value);
					}
					FREE_OP_DATA();
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
			value = zend_assign_to_variable(variable_ptr, value, OP_DATA_TYPE);
		}
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
	} else {
		if (EXPECTED(Z_ISREF_P(object_ptr))) {
			object_ptr = Z_REFVAL_P(object_ptr);
			if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(try_assign_dim_array);
			}
		}
		if (EXPECTED(Z_TYPE_P(object_ptr) == IS_OBJECT)) {
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
			value = GET_OP_DATA_ZVAL_PTR_DEREF(BP_VAR_R);

			if (OP2_TYPE == IS_CONST && Z_EXTRA_P(dim) == ZEND_EXTRA_VALUE) {
				dim++;
			}
			zend_assign_to_object_dim(object_ptr, dim, value OPLINE_CC EXECUTE_DATA_CC);

			FREE_OP_DATA();
		} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
			if (OP2_TYPE == IS_UNUSED) {
				zend_use_new_element_for_string();
				FREE_UNFETCHED_OP_DATA();
				FREE_OP1_VAR_PTR();
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			} else {
				dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
				value = GET_OP_DATA_ZVAL_PTR_DEREF(BP_VAR_R);
				zend_assign_to_string_offset(object_ptr, dim, value OPLINE_CC EXECUTE_DATA_CC);
				FREE_OP_DATA();
			}
		} else if (EXPECTED(Z_TYPE_P(object_ptr) <= IS_FALSE)) {
			ZVAL_ARR(object_ptr, zend_new_array(8));
			ZEND_VM_C_GOTO(try_assign_dim_array);
		} else {
			if (OP1_TYPE != IS_VAR || EXPECTED(!Z_ISERROR_P(object_ptr))) {
				zend_use_scalar_as_array();
			}
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
ZEND_VM_C_LABEL(assign_dim_error):
			FREE_UNFETCHED_OP_DATA();
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		}
	}
	if (OP2_TYPE != IS_UNUSED) {
		FREE_OP2();
	}
	FREE_OP1_VAR_PTR();
	/* assign_dim has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(38, ZEND_ASSIGN, VAR|CV, CONST|TMP|VAR|CV, SPEC(RETVAL))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *value;
	zval *variable_ptr;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(variable_ptr))) {
		FREE_OP2();
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
	} else {
		value = zend_assign_to_variable(variable_ptr, value, OP2_TYPE);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
		FREE_OP1_VAR_PTR();
		/* zend_assign_to_variable() always takes care of op2, never free it! */
	}

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(39, ZEND_ASSIGN_REF, VAR|CV, VAR|CV, SRC)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *variable_ptr;
	zval *value_ptr;

	SAVE_OPLINE();
	value_ptr = GET_OP2_ZVAL_PTR_PTR(BP_VAR_W);
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(variable_ptr))) {
		variable_ptr = &EG(uninitialized_zval);
	} else if (OP1_TYPE == IS_VAR &&
	           UNEXPECTED(Z_TYPE_P(EX_VAR(opline->op1.var)) != IS_INDIRECT)) {

		zend_throw_error(NULL, "Cannot assign by reference to overloaded object");
		FREE_OP1_VAR_PTR();
		FREE_OP2_VAR_PTR();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	} else if (OP2_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(value_ptr))) {
		variable_ptr = &EG(uninitialized_zval);
	} else if (OP2_TYPE == IS_VAR &&
	           opline->extended_value == ZEND_RETURNS_FUNCTION &&
			   UNEXPECTED(!Z_ISREF_P(value_ptr))) {

		if (UNEXPECTED(!zend_wrong_assign_to_variable_reference(variable_ptr, value_ptr, OP2_TYPE OPLINE_CC EXECUTE_DATA_CC))) {
			FREE_OP2_VAR_PTR();
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}

		/* op2 freed by assign_to_variable */
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_assign_to_variable_reference(variable_ptr, value_ptr);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}

	FREE_OP2_VAR_PTR();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HELPER(zend_leave_helper, ANY, ANY)
{
	zend_execute_data *old_execute_data;
	uint32_t call_info = EX_CALL_INFO();

	if (EXPECTED((call_info & (ZEND_CALL_CODE|ZEND_CALL_TOP|ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_ALLOCATED)) == 0)) {
		i_free_compiled_variables(execute_data);

		EG(current_execute_data) = EX(prev_execute_data);
		if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
			zend_object *object = Z_OBJ(execute_data->This);
#if 0
			if (UNEXPECTED(EG(exception) != NULL) && (EX(opline)->op1.num & ZEND_CALL_CTOR)) {
#else
			if (UNEXPECTED(EG(exception) != NULL) && (call_info & ZEND_CALL_CTOR)) {
#endif
				GC_DELREF(object);
				zend_object_store_ctor_failed(object);
			}
			OBJ_RELEASE(object);
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
		i_free_compiled_variables(execute_data);

		if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_clean_and_cache_symbol_table(EX(symbol_table));
		}
		EG(current_execute_data) = EX(prev_execute_data);
		if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
			zend_object *object = Z_OBJ(execute_data->This);
#if 0
			if (UNEXPECTED(EG(exception) != NULL) && (EX(opline)->op1.num & ZEND_CALL_CTOR)) {
#else
			if (UNEXPECTED(EG(exception) != NULL) && (call_info & ZEND_CALL_CTOR)) {
#endif
				GC_DELREF(object);
				zend_object_store_ctor_failed(object);
			}
			OBJ_RELEASE(object);
		} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
		}

		zend_vm_stack_free_extra_args_ex(call_info, execute_data);
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
		zend_detach_symbol_table(execute_data);
		destroy_op_array(&EX(func)->op_array);
		efree_size(EX(func), sizeof(zend_op_array));
		old_execute_data = execute_data;
		execute_data = EG(current_execute_data) = EX(prev_execute_data);
		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		zend_attach_symbol_table(execute_data);
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			HANDLE_EXCEPTION_LEAVE();
		}

		LOAD_NEXT_OPLINE();
		ZEND_VM_LEAVE();
	} else {
		if (EXPECTED((call_info & ZEND_CALL_CODE) == 0)) {
			i_free_compiled_variables(execute_data);
			if (UNEXPECTED(call_info & (ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS))) {
				if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					zend_clean_and_cache_symbol_table(EX(symbol_table));
				}
				zend_vm_stack_free_extra_args_ex(call_info, execute_data);
			}
			EG(current_execute_data) = EX(prev_execute_data);
			if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
				OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
			}
			ZEND_VM_RETURN();
		} else /* if (call_kind == ZEND_CALL_TOP_CODE) */ {
			zend_array *symbol_table = EX(symbol_table);

			zend_detach_symbol_table(execute_data);
			old_execute_data = EX(prev_execute_data);
			while (old_execute_data) {
				if (old_execute_data->func && (ZEND_CALL_INFO(old_execute_data) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					if (old_execute_data->symbol_table == symbol_table) {
						zend_attach_symbol_table(old_execute_data);
					}
					break;
				}
				old_execute_data = old_execute_data->prev_execute_data;
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
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_NEXT_OPCODE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline++;
	} else {
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	FREE_OP1();
	ZEND_VM_JMP(opline);
}

ZEND_VM_HOT_NOCONST_HANDLER(44, ZEND_JMPNZ, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline = OP_JMP_ADDR(opline, opline->op2);
	} else {
		opline++;
	}
	FREE_OP1();
	ZEND_VM_JMP(opline);
}

ZEND_VM_HOT_NOCONST_HANDLER(45, ZEND_JMPZNZ, CONST|TMPVAR|CV, JMP_ADDR, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (EXPECTED(Z_TYPE_INFO_P(val) == IS_TRUE)) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
	} else {
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	FREE_OP1();
	ZEND_VM_JMP(opline);
}

ZEND_VM_COLD_CONST_HANDLER(46, ZEND_JMPZ_EX, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_NEXT_OPCODE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
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
	zend_free_op free_op1;
	zval *val;
	int ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
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

	SAVE_OPLINE();
	var = EX_VAR(opline->op1.var);
	if (Z_TYPE_P(var) != IS_ARRAY && Z_FE_ITER_P(var) != (uint32_t)-1) {
		zend_hash_iterator_del(Z_FE_ITER_P(var));
	}
	zval_ptr_dtor_nogc(var);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(53, ZEND_FAST_CONCAT, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;
	zend_string *op1_str, *op2_str, *str;


	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if ((OP1_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op1) == IS_STRING)) &&
	    (OP2_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op2) == IS_STRING))) {
		zend_string *op1_str = Z_STR_P(op1);
		zend_string *op2_str = Z_STR_P(op2);
		zend_string *str;

		if (OP1_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
			if (OP2_TYPE == IS_CONST || OP2_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op2_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
			}
			FREE_OP1();
		} else if (OP2_TYPE != IS_CONST && UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_CV) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), op1_str);
			} else {
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
			}
			FREE_OP2();
		} else if (OP1_TYPE != IS_CONST && OP1_TYPE != IS_CV &&
		    !ZSTR_IS_INTERNED(op1_str) && GC_REFCOUNT(op1_str) == 1) {
		    size_t len = ZSTR_LEN(op1_str);

			str = zend_string_extend(op1_str, len + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str) + len, ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			FREE_OP2();
		} else {
			str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
			memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
			memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
			ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			FREE_OP1();
			FREE_OP2();
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
			GET_OP1_UNDEF_CV(op1, BP_VAR_R);
		}
		op1_str = zval_get_string_func(op1);
	}
	if (OP2_TYPE == IS_CONST) {
		op2_str = Z_STR_P(op2);
	} else if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		op2_str = zend_string_copy(Z_STR_P(op2));
	} else {
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
			GET_OP2_UNDEF_CV(op2, BP_VAR_R);
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
	zend_free_op free_op2;
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
				GET_OP2_UNDEF_CV(var, BP_VAR_R);
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
	zend_free_op free_op2;
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
				GET_OP2_UNDEF_CV(var, BP_VAR_R);
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
	zend_free_op free_op2;
	zend_string **rope;
	zval *var, *ret;
	uint32_t i;
	size_t len = 0;
	char *target;

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
				GET_OP2_UNDEF_CV(var, BP_VAR_R);
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
	for (i = 0; i <= opline->extended_value; i++) {
		len += ZSTR_LEN(rope[i]);
	}
	ret = EX_VAR(opline->result.var);
	ZVAL_STR(ret, zend_string_alloc(len, 0));
	target = Z_STRVAL_P(ret);
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
	zend_free_op free_op2;
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
			ce = zend_fetch_class_by_name(Z_STR_P(class_name), class_name + 1, opline->op1.num);
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
				GET_OP2_UNDEF_CV(class_name, BP_VAR_R);
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
	zend_free_op free_op1, free_op2;
	zval *object;
	zend_function *fbc;
	zend_class_entry *called_scope;
	zend_object *obj;
	zend_execute_data *call;
	uint32_t call_info;

	SAVE_OPLINE();

	object = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

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
				GET_OP2_UNDEF_CV(function_name, BP_VAR_R);
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

	if (OP1_TYPE != IS_UNUSED) {
		do {
			if (OP1_TYPE == IS_CONST || UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
				if ((OP1_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(object))) {
					object = Z_REFVAL_P(object);
					if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
						break;
					}
				}
				if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
					object = GET_OP1_UNDEF_CV(object, BP_VAR_R);
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

	obj = Z_OBJ_P(object);
	called_scope = obj->ce;

	if (OP2_TYPE == IS_CONST &&
	    EXPECTED(CACHED_PTR(opline->result.num) == called_scope)) {
	    fbc = CACHED_PTR(opline->result.num + sizeof(void*));
	} else {
	    zend_object *orig_obj = obj;

		if (UNEXPECTED(obj->handlers->get_method == NULL)) {
			zend_throw_error(NULL, "Object does not support method calls");
			FREE_OP2();
			FREE_OP1();
			HANDLE_EXCEPTION();
		}

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
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(fbc->type <= ZEND_USER_FUNCTION) &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE))) &&
		    EXPECTED(obj == orig_obj)) {
			CACHE_POLYMORPHIC_PTR(opline->result.num, called_scope, fbc);
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	if (OP2_TYPE != IS_CONST) {
		FREE_OP2();
	}

	call_info = ZEND_CALL_NESTED_FUNCTION;
	if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0)) {
		obj = NULL;
		FREE_OP1();

		if ((OP1_TYPE & (IS_VAR|IS_TMP_VAR)) && UNEXPECTED(EG(exception))) {
			HANDLE_EXCEPTION();
		}
	} else if (OP1_TYPE & (IS_VAR|IS_TMP_VAR|IS_CV)) {
		/* CV may be changed indirectly (e.g. when it's a reference) */
		call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_RELEASE_THIS;
		if (OP1_TYPE == IS_CV) {
			GC_ADDREF(obj); /* For $this pointer */
		} else if (free_op1 != object) {
			GC_ADDREF(obj); /* For $this pointer */
			FREE_OP1();
		}
	}

	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, called_scope, obj);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, UNUSED|CLASS_FETCH|CONST|VAR, CONST|TMPVAR|UNUSED|CONSTRUCTOR|CV, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zval *function_name;
	zend_class_entry *ce;
	zend_object *object;
	zend_function *fbc;
	zend_execute_data *call;

	SAVE_OPLINE();

	if (OP1_TYPE == IS_CONST) {
		/* no function found. try a static method in class */
		ce = CACHED_PTR(opline->result.num);
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), RT_CONSTANT(opline, opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT |  ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				HANDLE_EXCEPTION();
			}
			if (OP2_TYPE != IS_CONST) {
				CACHE_PTR(opline->result.num, ce);
			}
		}
	} else if (OP1_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op1.num);
		if (UNEXPECTED(ce == NULL)) {
			ZEND_ASSERT(EG(exception));
			FREE_UNFETCHED_OP2();
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
		zend_free_op free_op2;

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
						GET_OP2_UNDEF_CV(function_name, BP_VAR_R);
						if (UNEXPECTED(EG(exception) != NULL)) {
							HANDLE_EXCEPTION();
						}
					}
					zend_throw_error(NULL, "Function name must be a string");
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
		    EXPECTED(fbc->type <= ZEND_USER_FUNCTION) &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE)))) {
			CACHE_POLYMORPHIC_PTR(opline->result.num, ce, fbc);
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
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
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	object = NULL;
	if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
		if (Z_TYPE(EX(This)) == IS_OBJECT && instanceof_function(Z_OBJCE(EX(This)), ce)) {
			object = Z_OBJ(EX(This));
			ce = object->ce;
		} else {
			zend_non_static_method_call(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
	}

	if (OP1_TYPE == IS_UNUSED) {
		/* previous opcode is ZEND_FETCH_CLASS */
		if ((opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_PARENT ||
		    (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) {
			if (Z_TYPE(EX(This)) == IS_OBJECT) {
				ce = Z_OBJCE(EX(This));
			} else {
				ce = Z_CE(EX(This));
			}
		}
	}

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, ce, object);
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
		func = zend_hash_find_ex(EG(function_table), Z_STR_P(function_name+1), 1);
		if (UNEXPECTED(func == NULL)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_undefined_function_helper, function_name, function_name);
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = init_func_run_time_cache_ex(func);
		}
		CACHE_PTR(opline->result.num, fbc);
	}
	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(128, ZEND_INIT_DYNAMIC_CALL, ANY, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zend_free_op free_op2;
	zval *function_name;
	zend_execute_data *call;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_function_name):
	if (OP2_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
		call = zend_init_dynamic_call_string(Z_STR_P(function_name), opline->extended_value);
	} else if (OP2_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(function_name) == IS_OBJECT)) {
		call = zend_init_dynamic_call_object(function_name, opline->extended_value);
	} else if (EXPECTED(Z_TYPE_P(function_name) == IS_ARRAY)) {
		call = zend_init_dynamic_call_array(Z_ARRVAL_P(function_name), opline->extended_value);
	} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_TYPE_P(function_name) == IS_REFERENCE)) {
		function_name = Z_REFVAL_P(function_name);
		ZEND_VM_C_GOTO(try_function_name);
	} else {
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(function_name) == IS_UNDEF)) {
			GET_OP2_UNDEF_CV(function_name, BP_VAR_R);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
		zend_throw_error(NULL, "Function name must be a string");
		call = NULL;
	}

	if (UNEXPECTED(!call)) {
		HANDLE_EXCEPTION();
	}

	FREE_OP2();
	if (OP2_TYPE & (IS_VAR|IS_TMP_VAR)) {
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
	} else if (UNEXPECTED(!call)) {
		HANDLE_EXCEPTION();
	}

	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(118, ZEND_INIT_USER_CALL, CONST, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zend_free_op free_op2;
	zval *function_name;
	zend_fcall_info_cache fcc;
	char *error = NULL;
	zend_function *func;
	zend_class_entry *called_scope;
	zend_object *object;
	zend_execute_data *call;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);
	if (zend_is_callable_ex(function_name, NULL, 0, NULL, &fcc, &error)) {
		func = fcc.function_handler;
		called_scope = fcc.called_scope;
		object = fcc.object;
		if (error) {
			efree(error);
			/* This is the only soft error is_callable() can generate */
			zend_non_static_method_call(func);
			if (UNEXPECTED(EG(exception) != NULL)) {
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
		}
		if (func->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
			call_info |= ZEND_CALL_CLOSURE;
			if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
				call_info |= ZEND_CALL_FAKE_CLOSURE;
			}
		} else if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS;
			GC_ADDREF(object); /* For $this pointer */
		}

		FREE_OP2();
		if ((OP2_TYPE & (IS_TMP_VAR|IS_VAR)) && UNEXPECTED(EG(exception))) {
			if (call_info & ZEND_CALL_CLOSURE) {
				zend_object_release(ZEND_CLOSURE_OBJECT(func));
			}
			if (call_info & ZEND_CALL_RELEASE_THIS) {
				zend_object_release(object);
			}
			HANDLE_EXCEPTION();
		}

		if (EXPECTED(func->type == ZEND_USER_FUNCTION) && UNEXPECTED(!func->op_array.run_time_cache)) {
			init_func_run_time_cache(&func->op_array);
		}
	} else {
		zend_internal_type_error(EX_USES_STRICT_TYPES(), "%s() expects parameter 1 to be a valid callback, %s", Z_STRVAL_P(RT_CONSTANT(opline, opline->op1)), error);
		efree(error);
		FREE_OP2();
		if (UNEXPECTED(EG(exception))) {
			HANDLE_EXCEPTION();
		}
		func = (zend_function*)&zend_pass_function;
		called_scope = NULL;
		object = NULL;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		func, opline->extended_value, called_scope, object);
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
		func_name = RT_CONSTANT(opline, opline->op2) + 1;
		func = zend_hash_find_ex(EG(function_table), Z_STR_P(func_name), 1);
		if (func == NULL) {
			func_name++;
			func = zend_hash_find_ex(EG(function_table), Z_STR_P(func_name), 1);
			if (UNEXPECTED(func == NULL)) {
				ZEND_VM_DISPATCH_TO_HELPER(zend_undefined_function_helper, function_name, func_name);
			}
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = init_func_run_time_cache_ex(func);
		}
		CACHE_PTR(opline->result.num, fbc);
	}

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(61, ZEND_INIT_FCALL, NUM, CONST, NUM|CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op2;
	zval *fname;
	zval *func;
	zend_function *fbc;
	zend_execute_data *call;

	fbc = CACHED_PTR(opline->result.num);
	if (UNEXPECTED(fbc == NULL)) {
		fname = GET_OP2_ZVAL_PTR(BP_VAR_R);
		func = zend_hash_find_ex(EG(function_table), Z_STR_P(fname), 1);
		if (UNEXPECTED(func == NULL)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_undefined_function_helper, function_name, fname);
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			fbc = init_func_run_time_cache_ex(func);
		}
		CACHE_PTR(opline->result.num, fbc);
	}

	call = zend_vm_stack_push_call_frame_ex(
		opline->op1.num, ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(129, ZEND_DO_ICALL, ANY, ANY, SPEC(RETVAL))
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

	ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
	ZVAL_NULL(ret);

	fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
	if (!EG(exception) && call->func) {
		ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
			zend_verify_internal_return_type(call->func, ret));
		ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
			? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
	}
#endif

	EG(current_execute_data) = execute_data;
	zend_vm_stack_free_args(call);
	zend_vm_stack_free_call_frame(call);

	if (!RETURN_VALUE_USED(opline)) {
		zval_ptr_dtor(ret);
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION();
	}

	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HOT_HANDLER(130, ZEND_DO_UCALL, ANY, ANY, SPEC(RETVAL))
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
		ZVAL_NULL(ret);
	}

	call->prev_execute_data = execute_data;
	execute_data = call;
	i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
	LOAD_OPLINE();

	ZEND_VM_ENTER_EX();
}

ZEND_VM_HOT_HANDLER(131, ZEND_DO_FCALL_BY_NAME, ANY, ANY, SPEC(RETVAL))
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
			ZVAL_NULL(ret);
		}

		call->prev_execute_data = execute_data;
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
		LOAD_OPLINE();

		ZEND_VM_ENTER_EX();
	} else {
		zval retval;
		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);

		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_deprecated_function(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
			    UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
		}

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
		 && UNEXPECTED(!zend_verify_internal_arg_types(fbc, call))) {
			zend_vm_stack_free_call_frame(call);
			zend_rethrow_exception(execute_data);
		    UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
		ZVAL_NULL(ret);

		fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
		}
#endif

		EG(current_execute_data) = execute_data;
		zend_vm_stack_free_args(call);
		zend_vm_stack_free_call_frame(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(ret);
		}
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_rethrow_exception(execute_data);
		HANDLE_EXCEPTION();
	}
	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HOT_HANDLER(60, ZEND_DO_FCALL, ANY, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zend_object *object;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;
	if (UNEXPECTED((fbc->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_DEPRECATED)) != 0)) {
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_ABSTRACT) != 0)) {
			ZEND_VM_DISPATCH_TO_HELPER(zend_abstract_method_helper, fbc, fbc);
		}
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_deprecated_function(fbc);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
		}
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		ret = NULL;
		if (RETURN_VALUE_USED(opline)) {
			ret = EX_VAR(opline->result.var);
			ZVAL_NULL(ret);
		}

		call->prev_execute_data = execute_data;
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 1 EXECUTE_DATA_CC);

		if (EXPECTED(zend_execute_ex == execute_ex)) {
			LOAD_OPLINE();
			ZEND_VM_ENTER_EX();
		} else {
			execute_data = EX(prev_execute_data);
			LOAD_OPLINE();
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
		}
	} else if (EXPECTED(fbc->type < ZEND_USER_FUNCTION)) {
		zval retval;

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
		  && UNEXPECTED(!zend_verify_internal_arg_types(fbc, call))) {
			UNDEF_RESULT();
			ZEND_VM_C_GOTO(fcall_end);
		}

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;
		ZVAL_NULL(ret);

		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			fbc->internal_function.handler(call, ret);
		} else {
			zend_execute_internal(call, ret);
		}

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
		}
#endif

		EG(current_execute_data) = execute_data;
		zend_vm_stack_free_args(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(ret);
		}

	} else { /* ZEND_OVERLOADED_FUNCTION */
		zval retval;

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;

		call->prev_execute_data = execute_data;

		if (UNEXPECTED(!zend_do_fcall_overloaded(call, ret))) {
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(ret);
		}
	}

ZEND_VM_C_LABEL(fcall_end):
	if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS)) {
		object = Z_OBJ(call->This);
#if 0
		if (UNEXPECTED(EG(exception) != NULL) && (opline->op1.num & ZEND_CALL_CTOR)) {
#else
		if (UNEXPECTED(EG(exception) != NULL) && (ZEND_CALL_INFO(call) & ZEND_CALL_CTOR)) {
#endif
			GC_DELREF(object);
			zend_object_store_ctor_failed(object);
		}
		OBJ_RELEASE(object);
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
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE == IS_UNUSED) {
		zend_verify_missing_return_type(EX(func), CACHE_ADDR(opline->op2.num));
	} else {
/* prevents "undefined variable opline" errors */
#if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
		zval *retval_ref, *retval_ptr;
		zend_free_op free_op1;
		zend_arg_info *ret_info = EX(func)->common.arg_info - 1;

		retval_ref = retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

		if (OP1_TYPE == IS_CONST) {
			ZVAL_COPY(EX_VAR(opline->result.var), retval_ptr);
			retval_ref = retval_ptr = EX_VAR(opline->result.var);
		} else if (OP1_TYPE == IS_VAR) {
			if (UNEXPECTED(Z_TYPE_P(retval_ptr) == IS_INDIRECT)) {
				retval_ptr = Z_INDIRECT_P(retval_ptr);
			}
			ZVAL_DEREF(retval_ptr);
		} else if (OP1_TYPE == IS_CV) {
			ZVAL_DEREF(retval_ptr);
		}

		if (UNEXPECTED(!ZEND_TYPE_IS_CLASS(ret_info->type)
			&& ZEND_TYPE_CODE(ret_info->type) != IS_CALLABLE
			&& ZEND_TYPE_CODE(ret_info->type) != IS_ITERABLE
			&& !ZEND_SAME_FAKE_TYPE(ZEND_TYPE_CODE(ret_info->type), Z_TYPE_P(retval_ptr))
			&& !(EX(func)->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE)
			&& retval_ref != retval_ptr)
		) {
			/* A cast might happen - unwrap the reference if this is a by-value return */
			if (Z_REFCOUNT_P(retval_ref) == 1) {
				ZVAL_UNREF(retval_ref);
			} else {
				Z_DELREF_P(retval_ref);
				ZVAL_COPY(retval_ref, retval_ptr);
			}
			retval_ptr = retval_ref;
		}
		zend_verify_return_type(EX(func), retval_ptr, CACHE_ADDR(opline->op2.num));
#endif
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(62, ZEND_RETURN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *retval_ptr;
	zval *return_value;
	zend_free_op free_op1;

	retval_ptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	return_value = EX(return_value);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(retval_ptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		retval_ptr = GET_OP1_UNDEF_CV(retval_ptr, BP_VAR_R);
		if (return_value) {
			ZVAL_NULL(return_value);
		}
	} else if (!return_value) {
		if (OP1_TYPE & (IS_VAR|IS_TMP_VAR)) {
			if (Z_REFCOUNTED_P(free_op1) && !Z_DELREF_P(free_op1)) {
				SAVE_OPLINE();
				rc_dtor_func(Z_COUNTED_P(free_op1));
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
			if (Z_OPT_REFCOUNTED_P(retval_ptr)) {
				if (EXPECTED(!Z_OPT_ISREF_P(retval_ptr))) {
					ZVAL_COPY_VALUE(return_value, retval_ptr);
					if (EXPECTED(!(EX_CALL_INFO() & ZEND_CALL_CODE))) {
						ZVAL_NULL(retval_ptr);
					} else {
						Z_ADDREF_P(return_value);
					}
				} else {
					retval_ptr = Z_REFVAL_P(retval_ptr);
					ZVAL_COPY(return_value, retval_ptr);
				}
			} else {
				ZVAL_COPY_VALUE(return_value, retval_ptr);
			}
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
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_COLD_CONST_HANDLER(111, ZEND_RETURN_BY_REF, CONST|TMP|VAR|CV, ANY, SRC)
{
	USE_OPLINE
	zval *retval_ptr;
	zend_free_op free_op1;

	SAVE_OPLINE();

	do {
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR)) ||
		    (OP1_TYPE == IS_VAR && opline->extended_value == ZEND_RETURNS_VALUE)) {
			/* Not supposed to happen, but we'll allow it */
			zend_error(E_NOTICE, "Only variable references should be returned by reference");

			retval_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
			if (!EX(return_value)) {
				FREE_OP1();
			} else {
				if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISREF_P(retval_ptr))) {
					ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
					break;
				}

				ZVAL_NEW_REF(EX(return_value), retval_ptr);
				if (OP1_TYPE == IS_CONST) {
					Z_TRY_ADDREF_P(retval_ptr);
				}
			}
			break;
		}

		retval_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

		if (OP1_TYPE == IS_VAR) {
			if (retval_ptr == &EG(uninitialized_zval) ||
			    (opline->extended_value == ZEND_RETURNS_FUNCTION && !Z_ISREF_P(retval_ptr))) {
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				if (EX(return_value)) {
					ZVAL_NEW_REF(EX(return_value), retval_ptr);
				} else {
					FREE_OP1_VAR_PTR();
				}
				break;
			}
		}

		if (EX(return_value)) {
			if (Z_ISREF_P(retval_ptr)) {
				Z_ADDREF_P(retval_ptr);
			} else {
				ZVAL_MAKE_REF_EX(retval_ptr, 2);
			}
			ZVAL_REF(EX(return_value), Z_REF_P(retval_ptr));
		}

		FREE_OP1_VAR_PTR();
	} while (0);

	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
}

ZEND_VM_HANDLER(41, ZEND_GENERATOR_CREATE, ANY, ANY)
{
	zval *return_value = EX(return_value);

	if (EXPECTED(return_value)) {
		USE_OPLINE
		zend_generator *generator;
		zend_execute_data *gen_execute_data;
		uint32_t num_args, used_stack, call_info;

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
		 && (!(call_info & ((ZEND_CALL_CLOSURE|ZEND_CALL_RELEASE_THIS) << ZEND_CALL_INFO_SHIFT))
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

ZEND_VM_HANDLER(161, ZEND_GENERATOR_RETURN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *retval;
	zend_free_op free_op1;

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

	/* Close the generator to free up resources */
	zend_generator_close(generator, 1);

	/* Pass execution back to handling code */
	ZEND_VM_RETURN();
}

ZEND_VM_COLD_CONST_HANDLER(108, ZEND_THROW, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zval *value;
	zend_free_op free_op1;

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
				GET_OP1_UNDEF_CV(value, BP_VAR_R);
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
	if (OP1_TYPE != IS_TMP_VAR) {
		Z_TRY_ADDREF_P(value);
	}

	zend_throw_exception_object(value);
	zend_exception_restore();
	FREE_OP1_IF_VAR();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, CONST, JMP_ADDR, LAST_CATCH|CACHE_SLOT)
{
	USE_OPLINE
	zend_class_entry *ce, *catch_ce;
	zend_object *exception;
	zval *ex;

	SAVE_OPLINE();
	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore();
	if (EG(exception) == NULL) {
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}
	catch_ce = CACHED_PTR(opline->extended_value & ~ZEND_LAST_CATCH);
	if (UNEXPECTED(catch_ce == NULL)) {
		catch_ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), RT_CONSTANT(opline, opline->op1) + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD);

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
	ex = EX_VAR(opline->result.var);
	if (UNEXPECTED(Z_ISREF_P(ex))) {
		ex = Z_REFVAL_P(ex);
	}
	zval_ptr_dtor(ex);
	ZVAL_OBJ(ex, EG(exception));
	if (UNEXPECTED(EG(exception) != exception)) {
		GC_ADDREF(EG(exception));
		HANDLE_EXCEPTION();
	} else {
		EG(exception) = NULL;
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HOT_HANDLER(65, ZEND_SEND_VAL, CONST|TMPVAR, NUM)
{
	USE_OPLINE
	zval *value, *arg;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	if (OP1_TYPE == IS_CONST) {
		if (UNEXPECTED(Z_OPT_REFCOUNTED_P(arg))) {
			Z_ADDREF_P(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_HELPER(zend_cannot_pass_by_ref_helper, ANY, ANY)
{
	USE_OPLINE
	zval *arg;
	uint32_t arg_num = opline->op2.num;

	SAVE_OPLINE();
	zend_throw_error(NULL, "Cannot pass parameter %d by reference", arg_num);
	FREE_UNFETCHED_OP1();
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_UNDEF(arg);
	HANDLE_EXCEPTION();
}

ZEND_VM_HOT_SEND_HANDLER(116, ZEND_SEND_VAL_EX, CONST|TMP, NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *value, *arg;
	zend_free_op free_op1;
	uint32_t arg_num = opline->op2.num;

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_val_by_ref);
		}
	} else if (ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
ZEND_VM_C_LABEL(send_val_by_ref):
		ZEND_VM_DISPATCH_TO_HELPER(zend_cannot_pass_by_ref_helper);
	}
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	if (OP1_TYPE == IS_CONST) {
		if (UNEXPECTED(Z_OPT_REFCOUNTED_P(arg))) {
			Z_ADDREF_P(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(117, ZEND_SEND_VAR, VAR|CV, NUM)
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(varptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		GET_OP1_UNDEF_CV(varptr, BP_VAR_R);
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_NULL(arg);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

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

ZEND_VM_HANDLER(106, ZEND_SEND_VAR_NO_REF, VAR, NUM)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr, *arg;

	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, varptr);

	if (EXPECTED(Z_ISREF_P(varptr))) {
		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	zend_error(E_NOTICE, "Only variables should be passed by reference");
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_SEND_HANDLER(50, ZEND_SEND_VAR_NO_REF_EX, VAR, NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr, *arg;
	uint32_t arg_num = opline->op2.num;

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (!QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_VAR);
		}

		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_COPY_VALUE(arg, varptr);

		if (EXPECTED(Z_ISREF_P(varptr) ||
		    QUICK_ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num))) {
			ZEND_VM_NEXT_OPCODE();
		}
	} else {
		if (!ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_VAR);
		}

		varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_COPY_VALUE(arg, varptr);

		if (EXPECTED(Z_ISREF_P(varptr) ||
		    ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num))) {
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	zend_error(E_NOTICE, "Only variables should be passed by reference");
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(67, ZEND_SEND_REF, VAR|CV, NUM)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr, *arg;

	SAVE_OPLINE();
	varptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	if (OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(varptr))) {
		ZVAL_NEW_EMPTY_REF(arg);
		ZVAL_NULL(Z_REFVAL_P(arg));
		ZEND_VM_NEXT_OPCODE();
	}

	if (Z_ISREF_P(varptr)) {
		Z_ADDREF_P(varptr);
	} else {
		ZVAL_MAKE_REF_EX(varptr, 2);
	}
	ZVAL_REF(arg, Z_REF_P(varptr));

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_SEND_HANDLER(66, ZEND_SEND_VAR_EX, VAR|CV, NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;
	uint32_t arg_num = opline->op2.num;

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_var_by_ref);
		}
	} else if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
ZEND_VM_C_LABEL(send_var_by_ref):
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(varptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		GET_OP1_UNDEF_CV(varptr, BP_VAR_R);
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_NULL(arg);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

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

ZEND_VM_HOT_SEND_HANDLER(100, ZEND_CHECK_FUNC_ARG, UNUSED, NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	uint32_t arg_num = opline->op2.num;

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

ZEND_VM_HOT_HANDLER(185, ZEND_SEND_FUNC_ARG, VAR, NUM)
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;

	if (UNEXPECTED(ZEND_CALL_INFO(EX(call)) & ZEND_CALL_SEND_ARG_BY_REF)) {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_REF);
	}

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

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
	zend_free_op free_op1;
	zval *args;
	int arg_num;

	SAVE_OPLINE();
	args = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg_num = ZEND_CALL_NUM_ARGS(EX(call)) + 1;

ZEND_VM_C_LABEL(send_again):
	if (EXPECTED(Z_TYPE_P(args) == IS_ARRAY)) {
		HashTable *ht = Z_ARRVAL_P(args);
		zval *arg, *top;
		zend_string *name;

		zend_vm_stack_extend_call_frame(&EX(call), arg_num - 1, zend_hash_num_elements(ht));

		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_REFCOUNT_P(args) > 1) {
			uint32_t i;
			int separate = 0;

			/* check if any of arguments are going to be passed by reference */
			for (i = 0; i < zend_hash_num_elements(ht); i++) {
				if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num + i)) {
					separate = 1;
					break;
				}
			}
			if (separate) {
				SEPARATE_ARRAY(args);
				ht = Z_ARRVAL_P(args);
			}
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, name, arg) {
			if (name) {
				zend_throw_error(NULL, "Cannot unpack array with string keys");
				FREE_OP1();
				HANDLE_EXCEPTION();
			}

			top = ZEND_CALL_ARG(EX(call), arg_num);
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

			ZEND_CALL_NUM_ARGS(EX(call))++;
			arg_num++;
		} ZEND_HASH_FOREACH_END();

	} else if (EXPECTED(Z_TYPE_P(args) == IS_OBJECT)) {
		zend_class_entry *ce = Z_OBJCE_P(args);
		zend_object_iterator *iter;

		if (!ce || !ce->get_iterator) {
			zend_error(E_WARNING, "Only arrays and Traversables can be unpacked");
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

			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter);
			}

			for (; iter->funcs->valid(iter) == SUCCESS; ++arg_num) {
				zval *arg, *top;

				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				arg = iter->funcs->get_current_data(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					break;
				}

				if (iter->funcs->get_current_key) {
					zval key;
					iter->funcs->get_current_key(iter, &key);
					if (UNEXPECTED(EG(exception) != NULL)) {
						break;
					}

					if (UNEXPECTED(Z_TYPE(key) != IS_LONG)) {
						zend_throw_error(NULL,
							(Z_TYPE(key) == IS_STRING) ?
								"Cannot unpack Traversable with string keys" :
								"Cannot unpack Traversable with non-integer keys");
						zval_ptr_dtor(&key);
						break;
					}
				}

				if (ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
					zend_error(
						E_WARNING, "Cannot pass by-reference argument %d of %s%s%s()"
						" by unpacking a Traversable, passing by-value instead", arg_num,
						EX(call)->func->common.scope ? ZSTR_VAL(EX(call)->func->common.scope->name) : "",
						EX(call)->func->common.scope ? "::" : "",
						ZSTR_VAL(EX(call)->func->common.function_name)
					);
				}

				ZVAL_DEREF(arg);
				Z_TRY_ADDREF_P(arg);

				zend_vm_stack_extend_call_frame(&EX(call), arg_num - 1, 1);
				top = ZEND_CALL_ARG(EX(call), arg_num);
				ZVAL_COPY_VALUE(top, arg);
				ZEND_CALL_NUM_ARGS(EX(call))++;

				iter->funcs->move_forward(iter);
			}

			zend_iterator_dtor(iter);
		}
	} else if (EXPECTED(Z_ISREF_P(args))) {
		args = Z_REFVAL_P(args);
		ZEND_VM_C_GOTO(send_again);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(args) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(args, BP_VAR_R);
		}
		zend_error(E_WARNING, "Only arrays and Traversables can be unpacked");
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(119, ZEND_SEND_ARRAY, ANY, ANY, NUM)
{
	USE_OPLINE
	zend_free_op free_op1;
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
		zend_internal_type_error(EX_USES_STRICT_TYPES(), "call_user_func_array() expects parameter 2 to be array, %s given", zend_get_type_by_const(Z_TYPE_P(args)));
		if (ZEND_CALL_INFO(EX(call)) & ZEND_CALL_CLOSURE) {
			OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(call)->func));
		}
		if (Z_TYPE(EX(call)->This) == IS_OBJECT) {
			OBJ_RELEASE(Z_OBJ(EX(call)->This));
		}
		EX(call)->func = (zend_function*)&zend_pass_function;
		Z_OBJ(EX(call)->This) = NULL;
		ZEND_SET_CALL_INFO(EX(call), 0, ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);
		FREE_UNFETCHED_OP2();
	} else {
		uint32_t arg_num;
		HashTable *ht;
		zval *arg, *param;


ZEND_VM_C_LABEL(send_array):
		ht = Z_ARRVAL_P(args);
		if (OP2_TYPE != IS_UNUSED) {
			zend_free_op free_op2;
			zval *op2 = GET_OP2_ZVAL_PTR_DEREF(BP_VAR_R);
			uint32_t skip = opline->extended_value;
			uint32_t count = zend_hash_num_elements(ht);
			zend_long len = zval_get_long(op2);

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
							}
						}
					} else {
						if (Z_ISREF_P(arg) &&
						    !(EX(call)->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
							/* don't separate references for __call */
							arg = Z_REFVAL_P(arg);
						}
					}
					ZVAL_COPY(param, arg);
					ZEND_CALL_NUM_ARGS(EX(call))++;
					arg_num++;
					param++;
				} ZEND_HASH_FOREACH_END();
			}
			FREE_OP2();
		} else {
			zend_vm_stack_extend_call_frame(&EX(call), 0, zend_hash_num_elements(ht));
			arg_num = 1;
			param = ZEND_CALL_ARG(EX(call), 1);
			ZEND_HASH_FOREACH_VAL(ht, arg) {
				if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
					if (UNEXPECTED(!Z_ISREF_P(arg))) {
						if (!ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
							/* By-value send is not allowed -- emit a warning,
							 * but still perform the call. */
							zend_param_must_be_ref(EX(call)->func, arg_num);
						}
					}
				} else {
					if (Z_ISREF_P(arg) &&
					    !(EX(call)->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
						/* don't separate references for __call */
						arg = Z_REFVAL_P(arg);
					}
				}
				ZVAL_COPY(param, arg);
				ZEND_CALL_NUM_ARGS(EX(call))++;
				arg_num++;
				param++;
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
	zend_free_op free_op1;

	SAVE_OPLINE();
	arg = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	param = ZEND_CALL_VAR(EX(call), opline->result.var);

	if (UNEXPECTED(ARG_MUST_BE_SENT_BY_REF(EX(call)->func, opline->op2.num))) {
		zend_param_must_be_ref(EX(call)->func, opline->op2.num);
	}

	ZVAL_COPY(param, arg);

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(63, ZEND_RECV, NUM, UNUSED|CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;

	if (UNEXPECTED(arg_num > EX_NUM_ARGS())) {
		SAVE_OPLINE();
		zend_missing_arg_error(execute_data);
		HANDLE_EXCEPTION();
	} else if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
		zval *param = EX_VAR(opline->result.var);

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_verify_arg_type(EX(func), arg_num, param, NULL, CACHE_ADDR(opline->op2.num)) || EG(exception))) {
			HANDLE_EXCEPTION();
		}
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
				if (UNEXPECTED(zval_update_constant_ex(param, EX(func)->op_array.scope) != SUCCESS)) {
					zval_ptr_dtor_nogc(param);
					ZVAL_UNDEF(param);
					HANDLE_EXCEPTION();
				}
				if (!Z_REFCOUNTED_P(param)) {
					ZVAL_COPY_VALUE(cache_val, param);
				}
			}
		} else {
			ZVAL_COPY(param, default_value);
		}
	}

	if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
		zval *default_value = RT_CONSTANT(opline, opline->op2);

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_verify_arg_type(EX(func), arg_num, param, default_value, CACHE_ADDR(opline->extended_value)) || EG(exception))) {
			HANDLE_EXCEPTION();
		}
	}

	ZEND_VM_REPEAT_OPCODE(ZEND_RECV_INIT);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(164, ZEND_RECV_VARIADIC, NUM, UNUSED|CACHE_SLOT)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;
	uint32_t arg_count = EX_NUM_ARGS();
	zval *params;

	SAVE_OPLINE();

	params = EX_VAR(opline->result.var);

	if (arg_num <= arg_count) {
		zval *param;

		array_init_size(params, arg_count - arg_num + 1);
		zend_hash_real_init_packed(Z_ARRVAL_P(params));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(params)) {
			param = EX_VAR_NUM(EX(func)->op_array.last_var + EX(func)->op_array.T);
			if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
				do {
					zend_verify_arg_type(EX(func), arg_num, param, NULL, CACHE_ADDR(opline->op2.num));
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

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(52, ZEND_BOOL, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *val;
	zend_free_op free_op1;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
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

ZEND_VM_HANDLER(48, ZEND_CASE, TMPVAR, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	do {
		int result;

		if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_LVAL_P(op1) == Z_LVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = ((double)Z_LVAL_P(op1) == Z_DVAL_P(op2));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
				result = (Z_DVAL_P(op1) == Z_DVAL_P(op2));
			} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
				result = (Z_DVAL_P(op1) == ((double)Z_LVAL_P(op2)));
			} else {
				break;
			}
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
			if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
				result = zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
				FREE_OP2();
			} else {
				break;
			}
		} else {
			break;
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	} while (0);

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
		op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
	}
	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
		op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
	}
	result = EX_VAR(opline->result.var);
	compare_function(result, op1, op2);
	ZVAL_BOOL(result, Z_LVAL_P(result) == 0);
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), RT_CONSTANT(opline, opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
			CACHE_PTR(opline->op2.num, ce);
		}
	} else if (OP1_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op1.num);
		if (UNEXPECTED(ce == NULL)) {
			ZEND_ASSERT(EG(exception));
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
			opline->extended_value, NULL, NULL);
	} else {
		if (EXPECTED(constructor->type == ZEND_USER_FUNCTION) && UNEXPECTED(!constructor->op_array.run_time_cache)) {
			init_func_run_time_cache(&constructor->op_array);
		}
		/* We are not handling overloaded classes right now */
		call = zend_vm_stack_push_call_frame(
			ZEND_CALL_FUNCTION | ZEND_CALL_RELEASE_THIS | ZEND_CALL_CTOR,
			constructor,
			opline->extended_value,
			ce,
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
	zend_free_op free_op1;
	zval *obj;
	zend_class_entry *ce, *scope;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	SAVE_OPLINE();
	obj = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(obj) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

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
				GET_OP1_UNDEF_CV(obj, BP_VAR_R);
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "__clone method called on non-object");
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} while (0);

	ce = Z_OBJCE_P(obj);
	clone = ce->clone;
	clone_call = Z_OBJ_HT_P(obj)->clone_obj;
	if (UNEXPECTED(clone_call == NULL)) {
		zend_throw_error(NULL, "Trying to clone an uncloneable object of class %s", ZSTR_VAL(ce->name));
		FREE_OP1();
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		HANDLE_EXCEPTION();
	}

	if (clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			scope = EX(func)->op_array.scope;
			if (!zend_check_private(clone, scope, clone->common.function_name)) {
				zend_throw_error(NULL, "Call to private %s::__clone() from context '%s'", ZSTR_VAL(clone->common.scope->name), scope ? ZSTR_VAL(scope->name) : "");
				FREE_OP1();
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			scope = EX(func)->op_array.scope;
			if (UNEXPECTED(!zend_check_protected(zend_get_function_root_class(clone), scope))) {
				zend_throw_error(NULL, "Call to protected %s::__clone() from context '%s'", ZSTR_VAL(clone->common.scope->name), scope ? ZSTR_VAL(scope->name) : "");
				FREE_OP1();
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		}
	}

	ZVAL_OBJ(EX_VAR(opline->result.var), clone_call(obj));

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

ZEND_VM_HANDLER(181, ZEND_FETCH_CLASS_CONSTANT, VAR|CONST|UNUSED|CLASS_FETCH, CONST, CACHE_SLOT)
{
	zend_class_entry *ce, *scope;
	zend_class_constant *c;
	zval *value, *zv;
	USE_OPLINE

	SAVE_OPLINE();

	do {
		if (OP1_TYPE == IS_CONST) {
			if (EXPECTED(CACHED_PTR(opline->extended_value + sizeof(void*)))) {
				value = CACHED_PTR(opline->extended_value + sizeof(void*));
				break;
			} else if (EXPECTED(CACHED_PTR(opline->extended_value))) {
				ce = CACHED_PTR(opline->extended_value);
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op1)), RT_CONSTANT(opline, opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
				if (UNEXPECTED(ce == NULL)) {
					ZEND_ASSERT(EG(exception));
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					HANDLE_EXCEPTION();
				}
			}
		} else {
			if (OP1_TYPE == IS_UNUSED) {
				ce = zend_fetch_class(NULL, opline->op1.num);
				if (UNEXPECTED(ce == NULL)) {
					ZEND_ASSERT(EG(exception));
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					HANDLE_EXCEPTION();
				}
			} else {
				ce = Z_CE_P(EX_VAR(opline->op1.var));
			}
			if (EXPECTED(CACHED_PTR(opline->extended_value) == ce)) {
				value = CACHED_PTR(opline->extended_value + sizeof(void*));
				break;
			}
		}

		zv = zend_hash_find_ex(&ce->constants_table, Z_STR_P(RT_CONSTANT(opline, opline->op2)), 1);
		if (EXPECTED(zv != NULL)) {
			c = Z_PTR_P(zv);
			scope = EX(func)->op_array.scope;
			if (!zend_verify_const_access(c, scope)) {
				zend_throw_error(NULL, "Cannot access %s const %s::%s", zend_visibility_string(Z_ACCESS_FLAGS(c->value)), ZSTR_VAL(ce->name), Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
			value = &c->value;
			if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
				zval_update_constant_ex(value, c->ce);
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
					HANDLE_EXCEPTION();
				}
			}
			CACHE_POLYMORPHIC_PTR(opline->extended_value, ce, value);
		} else {
			zend_throw_error(NULL, "Undefined class constant '%s'", Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		}
	} while (0);

	ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), value);

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(72, ZEND_ADD_ARRAY_ELEMENT, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, REF)
{
	USE_OPLINE
	zend_free_op free_op1;
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
		FREE_OP1_VAR_PTR();
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
		zend_free_op free_op2;
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
			hval = zend_dval_to_lval(Z_DVAL_P(offset));
			ZEND_VM_C_GOTO(num_index);
		} else if (Z_TYPE_P(offset) == IS_FALSE) {
			hval = 0;
			ZEND_VM_C_GOTO(num_index);
		} else if (Z_TYPE_P(offset) == IS_TRUE) {
			hval = 1;
			ZEND_VM_C_GOTO(num_index);
		} else if (OP2_TYPE == IS_CV && Z_TYPE_P(offset) == IS_UNDEF) {
			GET_OP2_UNDEF_CV(offset, BP_VAR_R);
			str = ZSTR_EMPTY_ALLOC();
			ZEND_VM_C_GOTO(str_index);
		} else {
			zend_illegal_offset();
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

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, ARRAY_INIT|REF)
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
		ZVAL_EMPTY_ARRAY(array);
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_COLD_CONST_HANDLER(21, ZEND_CAST, CONST|TMP|VAR|CV, ANY, TYPE)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr;
	zval *result = EX_VAR(opline->result.var);
	HashTable *ht;

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	switch (opline->extended_value) {
		case IS_NULL:
			ZVAL_NULL(result);
			break;
		case _IS_BOOL:
			ZVAL_BOOL(result, zend_is_true(expr));
			break;
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
				} else if (Z_OBJ_HT_P(expr)->get_properties) {
					HashTable *obj_ht = Z_OBJ_HT_P(expr)->get_properties(expr);
					if (obj_ht) {
						/* fast copy */
						obj_ht = zend_proptable_to_symtable(obj_ht,
							(Z_OBJCE_P(expr)->default_properties_count ||
							 Z_OBJ_P(expr)->handlers != &std_object_handlers ||
							 GC_IS_RECURSIVE(obj_ht)));
						ZVAL_ARR(result, obj_ht);
					} else {
						ZVAL_EMPTY_ARRAY(result);
					}
				} else {
					ZVAL_COPY_VALUE(result, expr);
					Z_ADDREF_P(result);
					convert_to_array(result);
				}
			} else {
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

ZEND_VM_HANDLER(73, ZEND_INCLUDE_OR_EVAL, CONST|TMPVAR|CV, ANY, EVAL)
{
	USE_OPLINE
	zend_op_array *new_op_array;
	zend_free_op free_op1;
	zval *inc_filename;

	SAVE_OPLINE();
	inc_filename = GET_OP1_ZVAL_PTR(BP_VAR_R);
	new_op_array = zend_include_or_eval(inc_filename, opline->extended_value);
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
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
	} else if (EXPECTED(new_op_array != NULL)) {
		zval *return_value = NULL;
		zend_execute_data *call;

		if (RETURN_VALUE_USED(opline)) {
			return_value = EX_VAR(opline->result.var);
			ZVAL_NULL(return_value);
		}

		new_op_array->scope = EX(func)->op_array.scope;

		call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_CODE | ZEND_CALL_HAS_SYMBOL_TABLE,
			(zend_function*)new_op_array, 0,
			Z_TYPE(EX(This)) != IS_OBJECT ? Z_CE(EX(This)) : NULL,
			Z_TYPE(EX(This)) == IS_OBJECT ? Z_OBJ(EX(This)) : NULL);

		if (EX_CALL_INFO() & ZEND_CALL_HAS_SYMBOL_TABLE) {
			call->symbol_table = EX(symbol_table);
		} else {
			call->symbol_table = zend_rebuild_symbol_table();
		}

		call->prev_execute_data = execute_data;
		i_init_code_execute_data(call, new_op_array, return_value);
		if (EXPECTED(zend_execute_ex == execute_ex)) {
			ZEND_VM_ENTER();
		} else {
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
			zend_vm_stack_free_call_frame(call);
		}

		destroy_op_array(new_op_array);
		efree_size(new_op_array, sizeof(zend_op_array));
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_rethrow_exception(execute_data);
			UNDEF_RESULT();
			HANDLE_EXCEPTION();
		}
	} else if (RETURN_VALUE_USED(opline)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(196, ZEND_UNSET_CV, CV, UNUSED)
{
	USE_OPLINE
	zval *var = EX_VAR(opline->op1.var);

	if (Z_REFCOUNTED_P(var)) {
		zend_refcounted *garbage = Z_COUNTED_P(var);

		ZVAL_UNDEF(var);
		SAVE_OPLINE();
		if (!GC_DELREF(garbage)) {
			rc_dtor_func(garbage);
		} else {
			gc_check_possible_root(garbage);
		}
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
	zend_free_op free_op1;

	SAVE_OPLINE();

	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
		tmp_name = NULL;
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		name = zval_get_tmp_string(varname, &tmp_name);
	}

	target_symbol_table = zend_get_target_symbol_table(opline->extended_value EXECUTE_DATA_CC);
	zend_hash_del_ind(target_symbol_table, name);

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_HANDLER(179, ZEND_UNSET_STATIC_PROP, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	USE_OPLINE
	zval *varname;
	zend_string *name, *tmp_name;
	zend_class_entry *ce;
	zend_free_op free_op1;

	SAVE_OPLINE();

	if (OP2_TYPE == IS_CONST) {
		ce = CACHED_PTR(opline->extended_value);
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)), RT_CONSTANT(opline, opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				FREE_UNFETCHED_OP1();
				HANDLE_EXCEPTION();
			}
			/*CACHE_PTR(opline->extended_value, ce);*/
		}
	} else if (OP2_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op2.num);
		if (UNEXPECTED(ce == NULL)) {
			ZEND_ASSERT(EG(exception));
			FREE_UNFETCHED_OP1();
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
		tmp_name = NULL;
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		name = zval_get_tmp_string(varname, &tmp_name);
	}

	zend_std_unset_static_property(ce, name);

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(75, ZEND_UNSET_DIM, VAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
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
				if (ht == &EG(symbol_table)) {
					zend_delete_global_variable(key);
				} else {
					zend_hash_del(ht, key);
				}
			} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
				hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_dim):
				zend_hash_index_del(ht, hval);
			} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_TYPE_P(offset) == IS_REFERENCE)) {
				offset = Z_REFVAL_P(offset);
				ZEND_VM_C_GOTO(offset_again);
			} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
				hval = zend_dval_to_lval(Z_DVAL_P(offset));
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
				hval = Z_RES_HANDLE_P(offset);
				ZEND_VM_C_GOTO(num_index_dim);
			} else if (OP2_TYPE == IS_CV && Z_TYPE_P(offset) == IS_UNDEF) {
				GET_OP2_UNDEF_CV(offset, BP_VAR_R);
				key = ZSTR_EMPTY_ALLOC();
				ZEND_VM_C_GOTO(str_index_dim);
			} else {
				zend_error(E_WARNING, "Illegal offset type in unset");
			}
			break;
		} else if (Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(unset_dim_array);
			}
		}
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			container = GET_OP1_UNDEF_CV(container, BP_VAR_R);
		}
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
			offset = GET_OP2_UNDEF_CV(offset, BP_VAR_R);
		}
		if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
			if (UNEXPECTED(Z_OBJ_HT_P(container)->unset_dimension == NULL)) {
				zend_use_object_as_array();
			} else {
				if (OP2_TYPE == IS_CONST && Z_EXTRA_P(offset) == ZEND_EXTRA_VALUE) {
					offset++;
				}
				Z_OBJ_HT_P(container)->unset_dimension(container, offset);
			}
		} else if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
			zend_throw_error(NULL, "Cannot unset string offsets");
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}
	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
			if (Z_ISREF_P(container)) {
				container = Z_REFVAL_P(container);
				if (Z_TYPE_P(container) != IS_OBJECT) {
					break;
				}
			} else {
				break;
			}
		}
		if (Z_OBJ_HT_P(container)->unset_property) {
			Z_OBJ_HT_P(container)->unset_property(container, offset, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value) : NULL));
		} else {
			zend_wrong_property_unset(offset);
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(77, ZEND_FE_RESET_R, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
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
		if (!Z_OBJCE_P(array_ptr)->get_iterator) {
			result = EX_VAR(opline->result.var);
			ZVAL_COPY_VALUE(result, array_ptr);
			if (OP1_TYPE != IS_TMP_VAR) {
				Z_ADDREF_P(array_ptr);
			}
			if (Z_OBJ_P(array_ptr)->properties
			 && UNEXPECTED(GC_REFCOUNT(Z_OBJ_P(array_ptr)->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(Z_OBJ_P(array_ptr)->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(Z_OBJ_P(array_ptr)->properties);
				}
				Z_OBJ_P(array_ptr)->properties = zend_array_dup(Z_OBJ_P(array_ptr)->properties);
			}
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(Z_OBJPROP_P(array_ptr), 0);

			FREE_OP1_IF_VAR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			zend_bool is_empty = zend_fe_reset_iterator(array_ptr, 0 OPLINE_CC EXECUTE_DATA_CC);

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
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
		FREE_OP1();
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}
}

ZEND_VM_COLD_CONST_HANDLER(125, ZEND_FE_RESET_RW, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
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

		if (OP1_TYPE == IS_VAR) {
			FREE_OP1_VAR_PTR();
		}
		ZEND_VM_NEXT_OPCODE();
	} else if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(array_ptr) == IS_OBJECT)) {
		if (!Z_OBJCE_P(array_ptr)->get_iterator) {
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
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(Z_OBJPROP_P(array_ptr), 0);

			FREE_OP1_VAR_PTR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			zend_bool is_empty = zend_fe_reset_iterator(array_ptr, 1 OPLINE_CC EXECUTE_DATA_CC);

			if (OP1_TYPE == IS_VAR) {
				FREE_OP1_VAR_PTR();
			} else {
				FREE_OP1();
			}
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			} else if (is_empty) {
				ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
			} else {
				ZEND_VM_NEXT_OPCODE();
			}
		}
	} else {
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
		ZVAL_UNDEF(EX_VAR(opline->result.var));
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
		if (OP1_TYPE == IS_VAR) {
			FREE_OP1_VAR_PTR();
		} else {
			FREE_OP1();
		}
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}
}

ZEND_VM_HANDLER(78, ZEND_FE_FETCH_R, VAR, ANY, JMP_ADDR)
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
	if (EXPECTED(Z_TYPE_P(array) == IS_ARRAY)) {
		fe_ht = Z_ARRVAL_P(array);
		pos = Z_FE_POS_P(array);
		p = fe_ht->arData + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
ZEND_VM_C_LABEL(fe_fetch_r_exit):
				ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
				ZEND_VM_CONTINUE();
			}
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			if (EXPECTED(value_type != IS_UNDEF)) {
				if (UNEXPECTED(value_type == IS_INDIRECT)) {
					value = Z_INDIRECT_P(value);
					value_type = Z_TYPE_INFO_P(value);
					if (EXPECTED(value_type != IS_UNDEF)) {
						break;
					}
				} else {
					break;
				}
			}
			pos++;
			p++;
		}
		Z_FE_POS_P(array) = pos + 1;
		if (RETURN_VALUE_USED(opline)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
	} else {
		zend_object_iterator *iter;

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

				value = &p->val;
				value_type = Z_TYPE_INFO_P(value);
				if (EXPECTED(value_type != IS_UNDEF)) {
					if (UNEXPECTED(value_type == IS_INDIRECT)) {
						value = Z_INDIRECT_P(value);
						value_type = Z_TYPE_INFO_P(value);
						if (EXPECTED(value_type != IS_UNDEF)
						 && EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS)) {
							break;
						}
					} else {
						break;
					}
				}
				pos++;
				p++;
			}
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
			EG(ht_iterators)[Z_FE_ITER_P(array)].pos = pos + 1;
		} else {
			if (EXPECTED(++iter->index > 0)) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						UNDEF_RESULT();
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_r_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_r_exit);
			}
			if (RETURN_VALUE_USED(opline)) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
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
	}

	if (EXPECTED(OP2_TYPE == IS_CV)) {
		zval *variable_ptr = EX_VAR(opline->op2.var);
		zend_assign_to_variable(variable_ptr, value, IS_CV);
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
		p = fe_ht->arData + pos;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				/* reached end of iteration */
				ZEND_VM_C_GOTO(fe_fetch_w_exit);
			}
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			if (EXPECTED(value_type != IS_UNDEF)) {
				if (UNEXPECTED(value_type == IS_INDIRECT)) {
					value = Z_INDIRECT_P(value);
					value_type = Z_TYPE_INFO_P(value);
					if (EXPECTED(value_type != IS_UNDEF)) {
						break;
					}
				} else {
					break;
				}
			}
			pos++;
			p++;
		}
		if (RETURN_VALUE_USED(opline)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
		EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos + 1;
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

				value = &p->val;
				value_type = Z_TYPE_INFO_P(value);
				if (EXPECTED(value_type != IS_UNDEF)) {
					if (UNEXPECTED(value_type == IS_INDIRECT)) {
						value = Z_INDIRECT_P(value);
						value_type = Z_TYPE_INFO_P(value);
						if (EXPECTED(value_type != IS_UNDEF)
						 && EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS)) {
							break;
						}
					} else {
						break;
					}
				}
				pos++;
				p++;
			}
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
			EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos + 1;
		} else {
			if (++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						UNDEF_RESULT();
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_w_exit);
			}
			if (RETURN_VALUE_USED(opline)) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
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
		zend_error(E_WARNING, "Invalid argument supplied for foreach()");
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
			zval_ptr_dtor(variable_ptr);
			ZVAL_REF(variable_ptr, ref);
		}
	} else {
		Z_ADDREF_P(value);
		ZVAL_REF(EX_VAR(opline->op2.var), Z_REF_P(value));
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HOT_HANDLER(197, ZEND_ISSET_ISEMPTY_CV, CV, UNUSED, ISSET, SPEC(ISSET))
{
	USE_OPLINE
	zval *value;
	int result;

	value = EX_VAR(opline->op1.var);
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		result =
			Z_TYPE_P(value) > IS_NULL &&
		    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
	} else {
		SAVE_OPLINE();
		result = !i_zend_is_true(value);
		if (UNEXPECTED(EG(exception))) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			HANDLE_EXCEPTION();
		}
	}
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMPVAR|CV, UNUSED, VAR_FETCH|ISSET)
{
	USE_OPLINE
	zval *value;
	int result;
	zend_free_op free_op1;
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
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(180, ZEND_ISSET_ISEMPTY_STATIC_PROP, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, ISSET|CACHE_SLOT)
{
	USE_OPLINE
	zval *value;
	int result;
	zend_free_op free_op1;
	zval *varname;
	zend_string *name, *tmp_name;
	zend_class_entry *ce;

	SAVE_OPLINE();
	if (OP2_TYPE == IS_CONST) {
		if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(opline->extended_value & ~ZEND_ISEMPTY)) != NULL)) {
			value = CACHED_PTR((opline->extended_value & ~ZEND_ISEMPTY) + sizeof(void*));
			ZEND_VM_C_GOTO(is_static_prop_return);
		} else if (UNEXPECTED((ce = CACHED_PTR(opline->extended_value & ~ZEND_ISEMPTY)) == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)), RT_CONSTANT(opline, opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
			if (OP1_TYPE != IS_CONST) {
				CACHE_PTR(opline->extended_value & ~ZEND_ISEMPTY, ce);
			}
		}
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				FREE_UNFETCHED_OP1();
				ZVAL_UNDEF(EX_VAR(opline->result.var));
				HANDLE_EXCEPTION();
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		if (OP1_TYPE == IS_CONST &&
		    EXPECTED(CACHED_PTR(opline->extended_value & ~ZEND_ISEMPTY) == ce)) {

			value = CACHED_PTR((opline->extended_value & ~ZEND_ISEMPTY) + sizeof(void*));
			ZEND_VM_C_GOTO(is_static_prop_return);
		}
	}

	varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else {
		name = zval_get_tmp_string(varname, &tmp_name);
	}

	value = zend_std_get_static_property(ce, name, 1);

	if (OP1_TYPE == IS_CONST && value) {
		CACHE_POLYMORPHIC_PTR(opline->extended_value & ~ZEND_ISEMPTY, ce, value);
	}

	if (OP1_TYPE != IS_CONST) {
		zend_tmp_string_release(tmp_name);
	}
	FREE_OP1();

ZEND_VM_C_LABEL(is_static_prop_return):
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		result = value && Z_TYPE_P(value) > IS_NULL &&
		    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
	} else {
		result = !value || !i_zend_is_true(value);
	}

	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONSTCONST_HANDLER(115, ZEND_ISSET_ISEMPTY_DIM_OBJ, CONST|TMPVAR|CV, CONST|TMPVAR|CV, ISSET)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	int result;
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
			value = zend_hash_find_ex_ind(ht, str, OP2_TYPE == IS_CONST);
		} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_prop):
			value = zend_hash_index_find(ht, hval);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(offset))) {
			offset = Z_REFVAL_P(offset);
			ZEND_VM_C_GOTO(isset_again);
		} else {
			value = zend_find_array_dim_slow(ht, offset EXECUTE_DATA_CC);
		}

		if (!(opline->extended_value & ZEND_ISEMPTY)) {
			/* > IS_NULL means not IS_UNDEF and not IS_NULL */
			result = value != NULL && Z_TYPE_P(value) > IS_NULL &&
			    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
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
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(148, ZEND_ISSET_ISEMPTY_PROP_OBJ, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, ISSET|CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	int result;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_this_not_in_object_context_helper);
	}

	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
				ZEND_VM_C_GOTO(isset_no_object);
			}
		} else {
			ZEND_VM_C_GOTO(isset_no_object);
		}
	}
	if (UNEXPECTED(!Z_OBJ_HT_P(container)->has_property)) {
		zend_wrong_property_check(offset);
ZEND_VM_C_LABEL(isset_no_object):
		result = (opline->extended_value & ZEND_ISEMPTY);
	} else {
		result =
			(opline->extended_value & ZEND_ISEMPTY) ^
			Z_OBJ_HT_P(container)->has_property(container, offset, (opline->extended_value & ZEND_ISEMPTY), ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(opline->extended_value & ~ZEND_ISEMPTY) : NULL));
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_HANDLER(79, ZEND_EXIT, CONST|TMPVAR|UNUSED|CV, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE != IS_UNUSED) {
		zend_free_op free_op1;
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
	zend_bailout();
	ZEND_VM_NEXT_OPCODE(); /* Never reached */
}

ZEND_VM_HANDLER(57, ZEND_BEGIN_SILENCE, ANY, ANY)
{
	USE_OPLINE

	ZVAL_LONG(EX_VAR(opline->result.var), EG(error_reporting));

	if (EG(error_reporting)) {
		do {
			EG(error_reporting) = 0;
			if (!EG(error_reporting_ini_entry)) {
				zval *zv = zend_hash_find_ex(EG(ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING), 1);
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

	if (!EG(error_reporting) && Z_LVAL_P(EX_VAR(opline->op1.var)) != 0) {
		EG(error_reporting) = Z_LVAL_P(EX_VAR(opline->op1.var));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;
	zval *ref = NULL;
	int ret;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && Z_ISREF_P(value)) {
		if (OP1_TYPE == IS_VAR) {
			ref = value;
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
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(GC_DELREF(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(169, ZEND_COALESCE, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;
	zval *ref = NULL;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_IS);

	if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(value)) {
		if (OP1_TYPE & IS_VAR) {
			ref = value;
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
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(GC_DELREF(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP_EX(OP_JMP_ADDR(opline, opline->op2), 0);
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_HANDLER(22, ZEND_QM_ASSIGN, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;
	zval *result = EX_VAR(opline->result.var);

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		SAVE_OPLINE();
		GET_OP1_UNDEF_CV(value, BP_VAR_R);
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

ZEND_VM_HANDLER(139, ZEND_DECLARE_CLASS, CONST, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_class(&EX(func)->op_array, opline, EG(class_table), 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(140, ZEND_DECLARE_INHERITED_CLASS, CONST, CONST)
{
	zend_class_entry *parent;
	USE_OPLINE

	SAVE_OPLINE();
	parent = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)),
	                                 RT_CONSTANT(opline, opline->op2) + 1,
	                                 ZEND_FETCH_CLASS_EXCEPTION);
	if (UNEXPECTED(parent == NULL)) {
		ZEND_ASSERT(EG(exception));
		HANDLE_EXCEPTION();
	}
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), parent, 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_INHERITED_CLASS_DELAYED, CONST, CONST)
{
	USE_OPLINE
	zval *zce, *orig_zce;
	zend_class_entry *parent;

	SAVE_OPLINE();
	if ((zce = zend_hash_find_ex(EG(class_table), Z_STR_P(RT_CONSTANT(opline, opline->op1)), 1)) == NULL ||
	    ((orig_zce = zend_hash_find_ex(EG(class_table), Z_STR_P(RT_CONSTANT(opline, opline->op1)+1), 1)) != NULL &&
	     Z_CE_P(zce) != Z_CE_P(orig_zce))) {
		parent = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)),
	                                 RT_CONSTANT(opline, opline->op2) + 1,
	                                 ZEND_FETCH_CLASS_EXCEPTION);
		if (UNEXPECTED(parent == NULL)) {
			ZEND_ASSERT(EG(exception));
			HANDLE_EXCEPTION();
		}
		do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), parent, 0);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(171, ZEND_DECLARE_ANON_CLASS, ANY, ANY, JMP_ADDR)
{
	zval *zv;
	zend_class_entry *ce;
	USE_OPLINE

	SAVE_OPLINE();
	zv = zend_hash_find_ex(EG(class_table), Z_STR_P(RT_CONSTANT(opline, opline->op1)), 1);
	ZEND_ASSERT(zv != NULL);
	ce = Z_CE_P(zv);
	Z_CE_P(EX_VAR(opline->result.var)) = ce;

	if (ce->ce_flags & ZEND_ACC_ANON_BOUND) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}

	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
		zend_verify_abstract_class(ce);
	}
	ce->ce_flags |= ZEND_ACC_ANON_BOUND;
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(172, ZEND_DECLARE_ANON_INHERITED_CLASS, CONST, CONST, JMP_ADDR)
{
	zval *zv;
	zend_class_entry *ce, *parent;
	USE_OPLINE

	SAVE_OPLINE();
	zv = zend_hash_find_ex(EG(class_table), Z_STR_P(RT_CONSTANT(opline, opline->op1)), 1);
	ZEND_ASSERT(zv != NULL);
	ce = Z_CE_P(zv);
	Z_CE_P(EX_VAR(opline->result.var)) = ce;

	if (ce->ce_flags & ZEND_ACC_ANON_BOUND) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}

	parent = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)),
	                                 RT_CONSTANT(opline, opline->op2) + 1,
	                                 ZEND_FETCH_CLASS_EXCEPTION);
	if (UNEXPECTED(parent == NULL)) {
		ZEND_ASSERT(EG(exception));
		HANDLE_EXCEPTION();
	}

	zend_do_inheritance(ce, parent);
	ce->ce_flags |= ZEND_ACC_ANON_BOUND;
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(141, ZEND_DECLARE_FUNCTION, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	do_bind_function(&EX(func)->op_array, opline, EG(function_table), 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(105, ZEND_TICKS, ANY, ANY, NUM)
{
	USE_OPLINE

	if ((uint32_t)++EG(ticks_count) >= opline->extended_value) {
		EG(ticks_count) = 0;
		if (zend_ticks_function) {
			SAVE_OPLINE();
			zend_ticks_function(opline->extended_value);
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, CACHE_SLOT)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr;
	zend_bool result;

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_instanceof):
	if (Z_TYPE_P(expr) == IS_OBJECT) {
		zend_class_entry *ce;

		if (OP2_TYPE == IS_CONST) {
			ce = CACHED_PTR(opline->extended_value);
			if (UNEXPECTED(ce == NULL)) {
				ce = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)), RT_CONSTANT(opline, opline->op2) + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (EXPECTED(ce)) {
					CACHE_PTR(opline->extended_value, ce);
				}
			}
		} else if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
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
			GET_OP1_UNDEF_CV(expr, BP_VAR_R);
		}
		result = 0;
	}
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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

ZEND_VM_HANDLER(144, ZEND_ADD_INTERFACE, ANY, CONST, CACHE_SLOT)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *iface;

	SAVE_OPLINE();
	iface = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)), RT_CONSTANT(opline, opline->op2) + 1, ZEND_FETCH_CLASS_INTERFACE);
	if (UNEXPECTED(iface == NULL)) {
		ZEND_ASSERT(EG(exception));
		HANDLE_EXCEPTION();
	}

	if (UNEXPECTED((iface->ce_flags & ZEND_ACC_INTERFACE) == 0)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
	}
	zend_do_implement_interface(ce, iface);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(154, ZEND_ADD_TRAIT, ANY, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *trait;

	SAVE_OPLINE();
	trait = zend_fetch_class_by_name(Z_STR_P(RT_CONSTANT(opline, opline->op2)),
	                                 RT_CONSTANT(opline, opline->op2) + 1,
	                                 ZEND_FETCH_CLASS_TRAIT);
	if (UNEXPECTED(trait == NULL)) {
		ZEND_ASSERT(EG(exception));
		HANDLE_EXCEPTION();
	}
	if (!(trait->ce_flags & ZEND_ACC_TRAIT)) {
		zend_error_noreturn(E_ERROR, "%s cannot use %s - it is not a trait", ZSTR_VAL(ce->name), ZSTR_VAL(trait->name));
	}

	zend_do_implement_trait(ce, trait);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(155, ZEND_BIND_TRAITS, ANY, ANY)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));

	SAVE_OPLINE();
	zend_do_bind_traits(ce);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_dispatch_try_catch_finally_helper, ANY, ANY, uint32_t try_catch_offset, uint32_t op_num)
{
	/* May be NULL during generator closing (only finally blocks are executed) */
	zend_object *ex = EG(exception);

	/* Walk try/catch/finally structures upwards, performing the necessary actions */
	while (try_catch_offset != (uint32_t) -1) {
		zend_try_catch_element *try_catch =
			&EX(func)->op_array.try_catch_array[try_catch_offset];

		if (op_num < try_catch->catch_op && ex) {
			/* Go to catch block */
			cleanup_live_vars(execute_data, op_num, try_catch->catch_op);
			ZEND_VM_JMP_EX(&EX(func)->op_array.opcodes[try_catch->catch_op], 0);

		} else if (op_num < try_catch->finally_op) {
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
					zend_exception_set_previous(ex, Z_OBJ_P(fast_call));
				} else {
					EG(exception) = Z_OBJ_P(fast_call);
				}
				ex = Z_OBJ_P(fast_call);
			}
		}

		try_catch_offset--;
	}

	/* Uncaught exception */
	cleanup_live_vars(execute_data, op_num, 0);
	if (UNEXPECTED((EX_CALL_INFO() & ZEND_CALL_GENERATOR) != 0)) {
		zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);
		zend_generator_close(generator, 1);
		ZEND_VM_RETURN();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
	}
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	const zend_op *throw_op = EG(opline_before_exception);
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
			case ZEND_ROPE_INIT:
			case ZEND_ROPE_ADD:
				break; /* exception while building structures, live range handling will free those */

			case ZEND_FETCH_CLASS:
			case ZEND_DECLARE_CLASS:
			case ZEND_DECLARE_INHERITED_CLASS:
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
				break; /* return value is zend_class_entry pointer */

			default:
				zval_ptr_dtor_nogc(EX_VAR(throw_op->result.var));
		}
	}

	ZEND_VM_DISPATCH_TO_HELPER(zend_dispatch_try_catch_finally_helper, try_catch_offset, current_try_catch_offset, op_num, throw_op_num);
}

ZEND_VM_HANDLER(146, ZEND_VERIFY_ABSTRACT_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zend_verify_abstract_class(Z_CE_P(EX_VAR(opline->op1.var)));
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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
	ZEND_CONSTANT_SET_FLAGS(&c, CONST_CS, PHP_USER_CONSTANT);
	c.name = zend_string_copy(Z_STR_P(name));

	if (zend_register_constant(&c) == FAILURE) {
	}

	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(153, ZEND_DECLARE_LAMBDA_FUNCTION, CONST, UNUSED)
{
	USE_OPLINE
	zval *zfunc;
	zval *object;
	zend_class_entry *called_scope;
	zend_function *fbc;

	zfunc = zend_hash_find_ex(EG(function_table), Z_STR_P(RT_CONSTANT(opline, opline->op1)), 1);
	ZEND_ASSERT(zfunc != NULL && Z_FUNC_P(zfunc)->type == ZEND_USER_FUNCTION);

	fbc = Z_PTR_P(zfunc);
	if (fbc->common.fn_flags & ZEND_ACC_IMMUTABLE) {
		zend_function *new_func = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));

		memcpy(new_func, fbc, sizeof(zend_op_array));
		new_func->common.fn_flags &= ~ZEND_ACC_IMMUTABLE;
		Z_PTR_P(zfunc) = fbc = new_func;
	}

	if (Z_TYPE(EX(This)) == IS_OBJECT) {
		called_scope = Z_OBJCE(EX(This));
		if (UNEXPECTED((Z_FUNC_P(zfunc)->common.fn_flags & ZEND_ACC_STATIC) ||
				(EX(func)->common.fn_flags & ZEND_ACC_STATIC))) {
			object = NULL;
		} else {
			object = &EX(This);
		}
	} else {
		called_scope = Z_CE(EX(This));
		object = NULL;
	}
	zend_create_closure(EX_VAR(opline->result.var), Z_FUNC_P(zfunc),
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
	FREE_UNFETCHED_OP2();
	FREE_UNFETCHED_OP1();
	UNDEF_RESULT();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(160, ZEND_YIELD, CONST|TMP|VAR|CV|UNUSED, CONST|TMP|VAR|CV|UNUSED, SRC)
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
		zend_free_op free_op1;

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
				if (OP1_TYPE == IS_VAR &&
				    (value_ptr == &EG(uninitialized_zval) ||
				     (opline->extended_value == ZEND_RETURNS_FUNCTION &&
				      !Z_ISREF_P(value_ptr)))) {
					zend_error(E_NOTICE, "Only variable references should be yielded by reference");
					ZVAL_COPY(&generator->value, value_ptr);
				} else {
					if (Z_ISREF_P(value_ptr)) {
						Z_ADDREF_P(value_ptr);
					} else {
						ZVAL_MAKE_REF_EX(value_ptr, 2);
					}
					ZVAL_REF(&generator->value, Z_REF_P(value_ptr));
				}

				FREE_OP1_VAR_PTR();
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
		zend_free_op free_op2;
		zval *key = GET_OP2_ZVAL_PTR(BP_VAR_R);

		/* Consts, temporary variables and references need copying */
		if (OP2_TYPE == IS_CONST) {
			ZVAL_COPY_VALUE(&generator->key, key);
			if (UNEXPECTED(Z_OPT_REFCOUNTED(generator->key))) {
				Z_ADDREF(generator->key);
			}
		} else if (OP2_TYPE == IS_TMP_VAR) {
			ZVAL_COPY_VALUE(&generator->key, key);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(key)) {
			ZVAL_COPY(&generator->key, Z_REFVAL_P(key));
			FREE_OP2_IF_VAR();
		} else {
			ZVAL_COPY_VALUE(&generator->key, key);
			if (OP2_TYPE == IS_CV) {
				if (Z_OPT_REFCOUNTED_P(key)) Z_ADDREF_P(key);
			}
		}

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

ZEND_VM_HANDLER(142, ZEND_YIELD_FROM, CONST|TMP|VAR|CV, ANY)
{
	USE_OPLINE

	zend_generator *generator = zend_get_running_generator(EXECUTE_DATA_C);

	zval *val;
	zend_free_op free_op1;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (UNEXPECTED(generator->flags & ZEND_GENERATOR_FORCED_CLOSE)) {
		zend_throw_error(NULL, "Cannot use \"yield from\" in a force-closed generator");
		FREE_OP1();
		UNDEF_RESULT();
		HANDLE_EXCEPTION();
	}

	if (Z_TYPE_P(val) == IS_ARRAY) {
		ZVAL_COPY_VALUE(&generator->values, val);
		if (OP1_TYPE != IS_TMP_VAR && Z_OPT_REFCOUNTED_P(val)) {
			Z_ADDREF_P(val);
		}
		Z_FE_POS(generator->values) = 0;

		FREE_OP1_IF_VAR();
	} else if (OP1_TYPE != IS_CONST && Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val)->get_iterator) {
		zend_class_entry *ce = Z_OBJCE_P(val);
		if (ce == zend_ce_generator) {
			zend_generator *new_gen = (zend_generator *) Z_OBJ_P(val);

			if (OP1_TYPE != IS_TMP_VAR) {
				Z_ADDREF_P(val);
			}
			FREE_OP1_IF_VAR();

			if (Z_ISUNDEF(new_gen->retval)) {
				if (UNEXPECTED(zend_generator_get_current(new_gen) == generator)) {
					zend_throw_error(NULL, "Impossible to yield from the Generator being currently run");
					zval_ptr_dtor(val);
					UNDEF_RESULT();
					HANDLE_EXCEPTION();
				} else {
					zend_generator_yield_from(generator, new_gen);
				}
			} else if (UNEXPECTED(new_gen->execute_data == NULL)) {
				zend_throw_error(NULL, "Generator passed to yield from was aborted without proper return and is unable to continue");
				zval_ptr_dtor(val);
				UNDEF_RESULT();
				HANDLE_EXCEPTION();
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
	} else {
		zend_throw_error(NULL, "Can use \"yield from\" only with arrays and Traversables");
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
	zend_free_op free_op1, free_op2;
	zval *varname;
	zval *value;
	zval *variable_ptr;
	uintptr_t idx;
	zend_reference *ref;

	ZEND_VM_REPEATABLE_OPCODE

	varname = GET_OP2_ZVAL_PTR(BP_VAR_R);

	/* We store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
	idx = (uintptr_t)CACHED_PTR(opline->extended_value) - 1;
	if (EXPECTED(idx < EG(symbol_table).nNumUsed * sizeof(Bucket))) {
		Bucket *p = (Bucket*)((char*)EG(symbol_table).arData + idx);

		if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
	        (EXPECTED(p->key == Z_STR_P(varname)) ||
	         (EXPECTED(p->h == ZSTR_H(Z_STR_P(varname))) &&
	          EXPECTED(p->key != NULL) &&
	          EXPECTED(zend_string_equal_content(p->key, Z_STR_P(varname)))))) {

			value = (zval*)p; /* value = &p->val; */
			ZEND_VM_C_GOTO(check_indirect);
		}
	}

	value = zend_hash_find_ex(&EG(symbol_table), Z_STR_P(varname), 1);
	if (UNEXPECTED(value == NULL)) {
		value = zend_hash_add_new(&EG(symbol_table), Z_STR_P(varname), &EG(uninitialized_zval));
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
		zend_refcounted *ref = Z_COUNTED_P(variable_ptr);
		uint32_t refcnt = GC_DELREF(ref);

		if (EXPECTED(variable_ptr != value)) {
			if (refcnt == 0) {
				SAVE_OPLINE();
				rc_dtor_func(ref);
				if (UNEXPECTED(EG(exception))) {
					ZVAL_NULL(variable_ptr);
					HANDLE_EXCEPTION();
				}
			} else {
				gc_check_possible_root(ref);
			}
		}
	}
	ZVAL_REF(variable_ptr, ref);

	ZEND_VM_REPEAT_OPCODE(ZEND_BIND_GLOBAL);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(121, ZEND_STRLEN, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *value;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(value) == IS_STRING)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_STRLEN_P(value));
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE();
	} else {
		zend_bool strict;

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
			value = GET_OP1_UNDEF_CV(value, BP_VAR_R);
		}
		strict = EX_USES_STRICT_TYPES();
		do {
			if (EXPECTED(!strict)) {
				zend_string *str;
				zval tmp;

				ZVAL_COPY(&tmp, value);
				if (zend_parse_arg_str_weak(&tmp, &str)) {
					ZVAL_LONG(EX_VAR(opline->result.var), ZSTR_LEN(str));
					zval_ptr_dtor(&tmp);
					break;
				}
				zval_ptr_dtor(&tmp);
			}
			zend_internal_type_error(strict, "strlen() expects parameter 1 to be string, %s given", zend_get_type_by_const(Z_TYPE_P(value)));
			ZVAL_NULL(EX_VAR(opline->result.var));
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
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if ((opline->extended_value >> (uint32_t)Z_TYPE_P(value)) & 1) {
ZEND_VM_C_LABEL(type_check_resource):
		if (EXPECTED(Z_TYPE_P(value) != IS_RESOURCE)
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
		GET_OP1_UNDEF_CV(value, BP_VAR_R);
		ZEND_VM_SMART_BRANCH(result, 1);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	if (OP1_TYPE & (IS_TMP_VAR|IS_VAR)) {
		SAVE_OPLINE();
		FREE_OP1();
		ZEND_VM_SMART_BRANCH(result, 1);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HOT_HANDLER(122, ZEND_DEFINED, CONST, ANY, CACHE_SLOT)
{
	USE_OPLINE
	zend_constant *c;
	int result;

	c = CACHED_PTR(opline->extended_value);
	do {
		if (EXPECTED(c != NULL)) {
			if (!IS_SPECIAL_CACHE_VAL(c)) {
				result = 1;
				break;
			} else if (EXPECTED(zend_hash_num_elements(EG(zend_constants)) == DECODE_SPECIAL_CACHE_NUM(c))) {
				result = 0;
				break;
			}
		}
		if (zend_quick_check_constant(RT_CONSTANT(opline, opline->op1) OPLINE_CC EXECUTE_DATA_CC) != SUCCESS) {
			CACHE_PTR(opline->extended_value, ENCODE_SPECIAL_CACHE_NUM(zend_hash_num_elements(EG(zend_constants))));
			result = 0;
		} else {
			result = 1;
		}
	} while (0);
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
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

ZEND_VM_HANDLER(157, ZEND_FETCH_CLASS_NAME, UNUSED|CLASS_FETCH, ANY)
{
	uint32_t fetch_type;
	zend_class_entry *called_scope, *scope;
	USE_OPLINE

	fetch_type = opline->op1.num;

	scope = EX(func)->op_array.scope;
	if (UNEXPECTED(scope == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot use \"%s\" when no class scope is active",
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

ZEND_VM_HANDLER(158, ZEND_CALL_TRAMPOLINE, ANY, ANY)
{
	zend_array *args;
	zend_function *fbc = EX(func);
	zval *ret = EX(return_value);
	uint32_t call_info = EX_CALL_INFO() & (ZEND_CALL_NESTED | ZEND_CALL_TOP | ZEND_CALL_RELEASE_THIS);
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
	if (num_args) {
		ZVAL_ARR(ZEND_CALL_ARG(call, 2), args);
	} else {
		ZVAL_EMPTY_ARRAY(ZEND_CALL_ARG(call, 2));
	}
	zend_free_trampoline(fbc);
	fbc = call->func;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		if (UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
		execute_data = call;
		i_init_func_execute_data(&fbc->op_array, ret, 0 EXECUTE_DATA_CC);
		if (EXPECTED(zend_execute_ex == execute_ex)) {
			LOAD_OPLINE();
			ZEND_VM_ENTER_EX();
		} else {
			execute_data = EX(prev_execute_data);
			LOAD_OPLINE();
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
		}
	} else {
		zval retval;

		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);

		EG(current_execute_data) = call;

		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
		 && UNEXPECTED(!zend_verify_internal_arg_types(fbc, call))) {
			zend_vm_stack_free_call_frame(call);
			if (ret) {
				ZVAL_UNDEF(ret);
			}
			ZEND_VM_C_GOTO(call_trampoline_end);
		}

		if (ret == NULL) {
			ZVAL_NULL(&retval);
			ret = &retval;
		}

		if (!zend_execute_internal) {
			/* saves one function call if zend_execute_internal is not used */
			fbc->internal_function.handler(call, ret);
		} else {
			zend_execute_internal(call, ret);
		}

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, ret));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(ret) : !Z_ISREF_P(ret));
		}
#endif

		EG(current_execute_data) = call->prev_execute_data;

		zend_vm_stack_free_args(call);

		if (ret == &retval) {
			zval_ptr_dtor(ret);
		}
	}

ZEND_VM_C_LABEL(call_trampoline_end):
	execute_data = EG(current_execute_data);

	if (!EX(func) || !ZEND_USER_CODE(EX(func)->type) || (call_info & ZEND_CALL_TOP)) {
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
	zend_free_op free_op1, free_op2;
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
		if (UNEXPECTED(Z_ISUNDEF_P(var))) {
			SAVE_OPLINE();
			var = GET_OP2_UNDEF_CV(var, BP_VAR_R);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZVAL_DEREF(var);
		Z_TRY_ADDREF_P(var);
	}

	zend_closure_bind_var_ex(closure, (opline->extended_value & ~ZEND_BIND_REF), var);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(183, ZEND_BIND_STATIC, CV, CONST, REF)
{
	USE_OPLINE
	zend_free_op free_op1;
	HashTable *ht;
	zval *value;
	zval *variable_ptr;

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	zval_ptr_dtor(variable_ptr);

	ht = EX(func)->op_array.static_variables;
	ZEND_ASSERT(ht != NULL);
	if (GC_REFCOUNT(ht) > 1) {
		if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
			GC_DELREF(ht);
		}
		EX(func)->op_array.static_variables = ht = zend_array_dup(ht);
	}

	value = (zval*)((char*)ht->arData + (opline->extended_value & ~ZEND_BIND_REF));

	if (opline->extended_value & ZEND_BIND_REF) {
		if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
			SAVE_OPLINE();
			if (UNEXPECTED(zval_update_constant_ex(value, EX(func)->op_array.scope) != SUCCESS)) {
				ZVAL_NULL(variable_ptr);
				HANDLE_EXCEPTION();
			}
		}
		if (UNEXPECTED(!Z_ISREF_P(value))) {
			zend_reference *ref = (zend_reference*)emalloc(sizeof(zend_reference));
			GC_SET_REFCOUNT(ref, 2);
			GC_TYPE_INFO(ref) = IS_REFERENCE;
			ZVAL_COPY_VALUE(&ref->val, value);
			Z_REF_P(value) = ref;
			Z_TYPE_INFO_P(value) = IS_REFERENCE_EX;
			ZVAL_REF(variable_ptr, ref);
		} else {
			Z_ADDREF_P(value);
			ZVAL_REF(variable_ptr, Z_REF_P(value));
		}
	} else {
		ZVAL_COPY(variable_ptr, value);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(184, ZEND_FETCH_THIS, UNUSED, UNUSED)
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
		GET_OP1_UNDEF_CV(op1, BP_VAR_R);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(51, ZEND_MAKE_REF, VAR|CV, UNUSED)
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
	zend_free_op free_op1, free_op2;
	zval *op, *jump_zv;
	HashTable *jumptable;

	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	jumptable = Z_ARRVAL_P(GET_OP2_ZVAL_PTR(BP_VAR_R));

	if (Z_TYPE_P(op) != IS_LONG) {
		ZVAL_DEREF(op);
		if (Z_TYPE_P(op) != IS_LONG) {
			/* Wrong type, fall back to ZEND_CASE chain */
			ZEND_VM_NEXT_OPCODE();
		}
	}

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
	zend_free_op free_op1, free_op2;
	zval *op, *jump_zv;
	HashTable *jumptable;

	op = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	jumptable = Z_ARRVAL_P(GET_OP2_ZVAL_PTR(BP_VAR_R));

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

ZEND_VM_COLD_CONSTCONST_HANDLER(189, ZEND_IN_ARRAY, CONST|TMP|VAR|CV, CONST, NUM)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *op1;
	HashTable *ht = Z_ARRVAL_P(RT_CONSTANT(opline, opline->op2));
	int result;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		result = zend_hash_exists(ht, Z_STR_P(op1));
	} else if (opline->extended_value) {
		if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
			result = zend_hash_index_exists(ht, Z_LVAL_P(op1));
		} else {
			result = 0;
		}
	} else if (Z_TYPE_P(op1) <= IS_FALSE) {
		result = zend_hash_exists(ht, ZSTR_EMPTY_ALLOC());
	} else {
		zend_string *key;
		zval key_tmp, result_tmp;

		result = 0;
		ZEND_HASH_FOREACH_STR_KEY(ht, key) {
			ZVAL_STR(&key_tmp, key);
			compare_function(&result_tmp, op1, &key_tmp);
			if (Z_LVAL(result_tmp) == 0) {
				result = 1;
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(190, ZEND_COUNT, CONST|TMP|VAR|CV, UNUSED)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *op1;
	zend_long count;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	do {
		if (Z_TYPE_P(op1) == IS_ARRAY) {
			count = zend_array_count(Z_ARRVAL_P(op1));
			break;
		} else if (Z_TYPE_P(op1) == IS_OBJECT) {
			/* first, we check if the handler is defined */
			if (Z_OBJ_HT_P(op1)->count_elements) {
				if (SUCCESS == Z_OBJ_HT_P(op1)->count_elements(op1, &count)) {
					break;
				}
			}

			/* if not and the object implements Countable we call its count() method */
			if (instanceof_function(Z_OBJCE_P(op1), zend_ce_countable)) {
				zval retval;

				zend_call_method_with_0_params(op1, NULL, NULL, "count", &retval);
				count = zval_get_long(&retval);
				zval_ptr_dtor(&retval);
				break;
			}

			/* If There's no handler and it doesn't implement Countable then add a warning */
			count = 1;
		} else if (Z_TYPE_P(op1) == IS_NULL) {
			count = 0;
		} else {
			count = 1;
		}
		zend_error(E_WARNING, "count(): Parameter must be an array or an object that implements Countable");
	} while (0);

	ZVAL_LONG(EX_VAR(opline->result.var), count);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_COLD_CONST_HANDLER(191, ZEND_GET_CLASS, UNUSED|CONST|TMP|VAR|CV, UNUSED)
{
	USE_OPLINE

	if (OP1_TYPE == IS_UNUSED) {
		if (UNEXPECTED(!EX(func)->common.scope)) {
			SAVE_OPLINE();
			zend_error(E_WARNING, "get_class() called without object from outside a class");
			ZVAL_FALSE(EX_VAR(opline->result.var));
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			ZVAL_STR_COPY(EX_VAR(opline->result.var), EX(func)->common.scope->name);
			ZEND_VM_NEXT_OPCODE();
		}
	} else {
		zend_free_op free_op1;
		zval *op1;

		SAVE_OPLINE();
		op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
		if (Z_TYPE_P(op1) == IS_OBJECT) {
			ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_OBJCE_P(op1)->name);
		} else {
			zend_error(E_WARNING, "get_class() expects parameter 1 to be object, %s given", zend_get_type_by_const(Z_TYPE_P(op1)));
			ZVAL_FALSE(EX_VAR(opline->result.var));
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
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (UNEXPECTED(!EX(func)->common.scope)) {
			SAVE_OPLINE();
			zend_error(E_WARNING, "get_called_class() called from outside a class");
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_COLD_CONST_HANDLER(193, ZEND_GET_TYPE, CONST|TMP|VAR|CV, UNUSED)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *op1;
	zend_string *type;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	type = zend_zval_get_type(op1);
	if (EXPECTED(type)) {
		ZVAL_INTERNED_STR(EX_VAR(opline->result.var), type);
	} else {
		ZVAL_STRING(EX_VAR(opline->result.var), "unknown type");
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(194, ZEND_FUNC_NUM_ARGS, UNUSED, UNUSED)
{
	USE_OPLINE

	ZVAL_LONG(EX_VAR(opline->result.var), EX_NUM_ARGS());
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(195, ZEND_FUNC_GET_ARGS, UNUSED|CONST, UNUSED)
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
					} else {
						q = &EG(uninitialized_zval);
					}
					ZEND_HASH_FILL_ADD(q);
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
				} else {
					q = &EG(uninitialized_zval);
				}
				ZEND_HASH_FILL_ADD(q);
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) == Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) == Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_NOT_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) != Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_NOT_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) != Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) < Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) < Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_OR_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_LVAL_P(op1) <= Z_LVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_OR_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2;
	int result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = (Z_DVAL_P(op1) <= Z_DVAL_P(op2));
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG_NO_OVERFLOW, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_PRE_INC_LONG_OR_DOUBLE, TMPVARCV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_increment_function(var_ptr);
	} else {
		Z_DVAL_P(var_ptr)++;
	}
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG_NO_OVERFLOW, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_PRE_DEC_LONG_OR_DOUBLE, TMPVARCV, ANY, SPEC(RETVAL))
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_decrement_function(var_ptr);
	} else {
		Z_DVAL_P(var_ptr)--;
	}
	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG_NO_OVERFLOW, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)++;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_INC, (op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_increment_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_INC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_POST_INC_LONG_OR_DOUBLE, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_increment_function(var_ptr);
	} else {
		ZVAL_DOUBLE(EX_VAR(opline->result.var), Z_DVAL_P(var_ptr));
		Z_DVAL_P(var_ptr)++;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG_NO_OVERFLOW, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)--;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_decrement_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_POST_DEC_LONG_OR_DOUBLE, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
		fast_long_decrement_function(var_ptr);
	} else {
		ZVAL_DOUBLE(EX_VAR(opline->result.var), Z_DVAL_P(var_ptr));
		Z_DVAL_P(var_ptr)--;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, (op1_info == MAY_BE_DOUBLE), ZEND_QM_ASSIGN_DOUBLE, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_DOUBLE(EX_VAR(opline->result.var), Z_DVAL_P(value));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, ((op->op1_type == IS_CONST) ? !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)) : (!(op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))))), ZEND_QM_ASSIGN_NOREF, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_FETCH_DIM_R, (!(op2_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))), ZEND_FETCH_DIM_R_INDEX, CONST|TMPVAR|CV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
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
			offset = zval_get_long(dim);
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAR, (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_SIMPLE, CV|VAR, NUM)
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;

	varptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);

	if (OP1_TYPE == IS_CV) {
		ZVAL_COPY(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		ZVAL_COPY_VALUE(arg, varptr);
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAR_EX, op->op2.num <= MAX_ARG_FLAG_NUM && (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_EX_SIMPLE, CV|VAR, NUM)
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;
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

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAL, op->op1_type == IS_CONST && !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)), ZEND_SEND_VAL_SIMPLE, CONST, NUM)
{
	USE_OPLINE
	zval *value, *arg;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_SEND_VAL_EX, op->op2.num <= MAX_ARG_FLAG_NUM && op->op1_type == IS_CONST && !Z_REFCOUNTED_P(RT_CONSTANT(op, op->op1)), ZEND_SEND_VAL_EX_SIMPLE, CONST, NUM)
{
	USE_OPLINE
	zval *value, *arg;
	zend_free_op free_op1;
	uint32_t arg_num = opline->op2.num;

	if (QUICK_ARG_MUST_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_cannot_pass_by_ref_helper);
	}
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HOT_TYPE_SPEC_HANDLER(ZEND_FE_FETCH_R, op->op2_type == IS_CV && (op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_ARRAY, ZEND_FE_FETCH_R_SIMPLE, VAR, CV, JMP_ADDR, SPEC(RETVAL))
{
	USE_OPLINE
	zval *array;
	zval *value, *variable_ptr;
	uint32_t value_type;
	HashTable *fe_ht;
	HashPosition pos;
	Bucket *p;

	array = EX_VAR(opline->op1.var);
	SAVE_OPLINE();
	fe_ht = Z_ARRVAL_P(array);
	pos = Z_FE_POS_P(array);
	p = fe_ht->arData + pos;
	while (1) {
		if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
			/* reached end of iteration */
			ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
			ZEND_VM_CONTINUE();
		}
		value = &p->val;
		value_type = Z_TYPE_INFO_P(value);
		if (EXPECTED(value_type != IS_UNDEF)) {
			if (UNEXPECTED(value_type == IS_INDIRECT)) {
				value = Z_INDIRECT_P(value);
				value_type = Z_TYPE_INFO_P(value);
				if (EXPECTED(value_type != IS_UNDEF)) {
					break;
				}
			} else {
				break;
			}
		}
		pos++;
		p++;
	}
	Z_FE_POS_P(array) = pos + 1;
	if (RETURN_VALUE_USED(opline)) {
		if (!p->key) {
			ZVAL_LONG(EX_VAR(opline->result.var), p->h);
		} else {
			ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
		}
	}

	variable_ptr = EX_VAR(opline->op2.var);
	zend_assign_to_variable(variable_ptr, value, IS_CV);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_DEFINE_OP(137, ZEND_OP_DATA);

ZEND_VM_HELPER(zend_interrupt_helper, ANY, ANY)
{
	EG(vm_interrupt) = 0;
	if (EG(timed_out)) {
		zend_timeout(0);
	} else if (zend_interrupt_function) {
		SAVE_OPLINE();
		zend_interrupt_function(execute_data);
		ZEND_VM_ENTER();
	}
	ZEND_VM_CONTINUE();
}
