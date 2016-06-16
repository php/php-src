/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_VM_HANDLER(1, ZEND_ADD, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(2, ZEND_SUB, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(3, ZEND_MUL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(4, ZEND_DIV, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(5, ZEND_MOD, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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
				SAVE_OPLINE();
				zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
				HANDLE_EXCEPTION();
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

ZEND_VM_HANDLER(6, ZEND_SL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	shift_left_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(7, ZEND_SR, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	shift_right_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(166, ZEND_POW, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(8, ZEND_CONCAT, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	do {
		if ((OP1_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op1) == IS_STRING)) &&
		    (OP2_TYPE == IS_CONST || EXPECTED(Z_TYPE_P(op2) == IS_STRING))) {
			zend_string *op1_str = Z_STR_P(op1);
			zend_string *op2_str = Z_STR_P(op2);
			zend_string *str;

			if (OP1_TYPE != IS_CONST) {
				if (UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
					ZVAL_STR_COPY(EX_VAR(opline->result.var), op2_str);
					FREE_OP1();
					break;
				}
			}
			if (OP2_TYPE != IS_CONST) {
				if (UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
					ZVAL_STR_COPY(EX_VAR(opline->result.var), op1_str);
					FREE_OP1();
					break;
				}
			}
			if (OP1_TYPE != IS_CONST && OP1_TYPE != IS_CV &&
			    !ZSTR_IS_INTERNED(op1_str) && GC_REFCOUNT(op1_str) == 1) {
			    size_t len = ZSTR_LEN(op1_str);

				str = zend_string_extend(op1_str, len + ZSTR_LEN(op2_str), 0);
				memcpy(ZSTR_VAL(str) + len, ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
				ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
				break;
			} else {
				str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
				memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
				memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
				ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
			}
		} else {
			if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
				op1 = GET_OP1_UNDEF_CV(op1, BP_VAR_R);
			}
			if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
				op2 = GET_OP2_UNDEF_CV(op2, BP_VAR_R);
			}
			concat_function(EX_VAR(opline->result.var), op1, op2);
		}
		FREE_OP1();
	} while (0);
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(15, ZEND_IS_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
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

ZEND_VM_HANDLER(16, ZEND_IS_NOT_IDENTICAL, CONST|TMP|VAR|CV, CONST|TMP|VAR|CV)
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

ZEND_VM_HANDLER(17, ZEND_IS_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					result = 1;
				} else if (Z_STRVAL_P(op1)[0] > '9' || Z_STRVAL_P(op2)[0] > '9') {
					if (Z_STRLEN_P(op1) != Z_STRLEN_P(op2)) {
						result = 0;
					} else {
						result = (memcmp(Z_STRVAL_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op1)) == 0);
					}
				} else {
					result = (zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2)) == 0);
				}
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

ZEND_VM_HANDLER(18, ZEND_IS_NOT_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					result = 0;
				} else if (Z_STRVAL_P(op1)[0] > '9' || Z_STRVAL_P(op2)[0] > '9') {
					if (Z_STRLEN_P(op1) != Z_STRLEN_P(op2)) {
						result = 1;
					} else {
						result = (memcmp(Z_STRVAL_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op1)) != 0);
					}
				} else {
					result = (zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2)) != 0);
				}
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

ZEND_VM_HANDLER(19, ZEND_IS_SMALLER, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(20, ZEND_IS_SMALLER_OR_EQUAL, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(170, ZEND_SPACESHIP, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(9, ZEND_BW_OR, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	bitwise_or_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(10, ZEND_BW_AND, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	bitwise_and_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(11, ZEND_BW_XOR, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR(BP_VAR_R);
	bitwise_xor_function(EX_VAR(opline->result.var), op1, op2);
	FREE_OP1();
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(14, ZEND_BOOL_XOR, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(12, ZEND_BW_NOT, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;

	SAVE_OPLINE();
	bitwise_not_function(EX_VAR(opline->result.var),
		GET_OP1_ZVAL_PTR(BP_VAR_R));
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(13, ZEND_BOOL_NOT, CONST|TMPVAR|CV, ANY)
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
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);

		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			ZVAL_DEREF(object);
			if (UNEXPECTED(!make_real_object(object))) {
				zend_error(E_WARNING, "Attempt to assign property of non-object");
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
				break;
			}
		}

		/* here we are sure we are dealing with an object */
		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL))) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			} else {
				ZVAL_DEREF(zptr);
				SEPARATE_ZVAL_NOREF(zptr);

				binary_op(zptr, zptr, value);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), zptr);
				}
			}
		} else {
			zend_assign_op_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), value, binary_op, (UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL));
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
		if (OP2_TYPE == IS_UNUSED) {
			var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			if (UNEXPECTED(!var_ptr)) {
				zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
		} else {
			dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

			if (OP2_TYPE == IS_CONST) {
				var_ptr = zend_fetch_dimension_address_inner_RW_CONST(Z_ARRVAL_P(container), dim);
			} else {
				var_ptr = zend_fetch_dimension_address_inner_RW(Z_ARRVAL_P(container), dim);
			}
			if (UNEXPECTED(!var_ptr)) {
				ZEND_VM_C_GOTO(assign_dim_op_ret_null);
			}
			ZVAL_DEREF(var_ptr);
			SEPARATE_ZVAL_NOREF(var_ptr);
		}

		value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);

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
			ZVAL_NEW_ARR(container);
			zend_hash_init(Z_ARRVAL_P(container), 8, NULL, ZVAL_PTR_DTOR, 0);
			ZEND_VM_C_GOTO(assign_dim_op_new_array);
		}

		dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

		if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
			value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
			zend_binary_assign_op_obj_dim(container, dim, value, UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL, binary_op);
		} else {
			if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
				if (OP2_TYPE == IS_UNUSED) {
					zend_throw_error(NULL, "[] operator not supported for strings");
				} else {
					zend_check_string_offset(dim, BP_VAR_RW);
					zend_wrong_string_offset();
				}
			} else if (EXPECTED(Z_TYPE_P(container) <= IS_FALSE)) {
				ZEND_VM_C_GOTO(assign_dim_op_convert_to_array);
			} else {
				if (UNEXPECTED(OP1_TYPE != IS_VAR || EXPECTED(!Z_ISERROR_P(container)))) {
					zend_error(E_WARNING, "Cannot use a scalar value as an array");
				}
ZEND_VM_C_LABEL(assign_dim_op_ret_null):
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
			}
			value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
		}
	}

	FREE_OP2();
	FREE_OP(free_op_data1);
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HELPER(zend_binary_assign_op_helper, VAR|CV, CONST|TMPVAR|CV, binary_op_type binary_op)
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
		SEPARATE_ZVAL_NOREF(var_ptr);

		binary_op(var_ptr, var_ptr, value);

		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), var_ptr);
		}
	}

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(23, ZEND_ASSIGN_ADD, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, add_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, add_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, add_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, add_function);
#endif
}

ZEND_VM_HANDLER(24, ZEND_ASSIGN_SUB, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, sub_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, sub_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, sub_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, sub_function);
#endif
}

ZEND_VM_HANDLER(25, ZEND_ASSIGN_MUL, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, mul_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, mul_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, mul_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, mul_function);
#endif
}

ZEND_VM_HANDLER(26, ZEND_ASSIGN_DIV, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, div_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, div_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, div_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, div_function);
#endif
}

ZEND_VM_HANDLER(27, ZEND_ASSIGN_MOD, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, mod_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, mod_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, mod_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, mod_function);
#endif
}

ZEND_VM_HANDLER(28, ZEND_ASSIGN_SL, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, shift_left_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, shift_left_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, shift_left_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, shift_left_function);
#endif
}

ZEND_VM_HANDLER(29, ZEND_ASSIGN_SR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, shift_right_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, shift_right_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, shift_right_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, shift_right_function);
#endif
}

ZEND_VM_HANDLER(30, ZEND_ASSIGN_CONCAT, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, concat_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, concat_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, concat_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, concat_function);
#endif
}

ZEND_VM_HANDLER(31, ZEND_ASSIGN_BW_OR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_or_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_or_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, bitwise_or_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_or_function);
#endif
}

ZEND_VM_HANDLER(32, ZEND_ASSIGN_BW_AND, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_and_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_and_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, bitwise_and_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_and_function);
#endif
}

ZEND_VM_HANDLER(33, ZEND_ASSIGN_BW_XOR, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, bitwise_xor_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_xor_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, bitwise_xor_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, bitwise_xor_function);
#endif
}

ZEND_VM_HANDLER(167, ZEND_ASSIGN_POW, VAR|UNUSED|THIS|CV, CONST|TMPVAR|UNUSED|NEXT|CV, DIM_OBJ, SPEC(DIM_OBJ))
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_helper, binary_op, pow_function);
	}
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, pow_function);
	}
# endif
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_obj_helper, binary_op, pow_function);
#else
	ZEND_VM_DISPATCH_TO_HELPER(zend_binary_assign_op_dim_helper, binary_op, pow_function);
#endif
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
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			ZVAL_DEREF(object);
			if (UNEXPECTED(!make_real_object(object))) {
				zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
				break;
			}
		}

		/* here we are sure we are dealing with an object */
		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL))) != NULL)) {
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
					SEPARATE_ZVAL_NOREF(zptr);

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
			zend_pre_incdec_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), inc, (UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL));
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(132, ZEND_PRE_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_pre_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(133, ZEND_PRE_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
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
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
			ZVAL_DEREF(object);
			if (UNEXPECTED(!make_real_object(object))) {
				zend_error(E_WARNING, "Attempt to increment/decrement property of non-object");
				ZVAL_NULL(EX_VAR(opline->result.var));
				break;
			}
		}

		/* here we are sure we are dealing with an object */

		if (EXPECTED(Z_OBJ_HT_P(object)->get_property_ptr_ptr)
			&& EXPECTED((zptr = Z_OBJ_HT_P(object)->get_property_ptr_ptr(object, property, BP_VAR_RW, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL))) != NULL)) {
			if (UNEXPECTED(Z_ISERROR_P(zptr))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			} else {
				if (EXPECTED(Z_TYPE_P(zptr) == IS_LONG)) {
					ZVAL_COPY_VALUE(EX_VAR(opline->result.var), zptr);
					if (inc) {
						fast_long_increment_function(zptr);
					} else {
						fast_long_decrement_function(zptr);
					}
				} else {
					ZVAL_DEREF(zptr);
					ZVAL_COPY_VALUE(EX_VAR(opline->result.var), zptr);
					zval_opt_copy_ctor(zptr);
					if (inc) {
						increment_function(zptr);
					} else {
						decrement_function(zptr);
					}
				}
			}
		} else {
			zend_post_incdec_overloaded_property(object, property, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), inc, EX_VAR(opline->result.var));
		}
	} while (0);

	FREE_OP2();
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(134, ZEND_POST_INC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_post_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(135, ZEND_POST_DEC_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
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
	SEPARATE_ZVAL_NOREF(var_ptr);

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
	SEPARATE_ZVAL_NOREF(var_ptr);

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
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
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
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	zval_opt_copy_ctor(var_ptr);

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
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
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
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	zval_opt_copy_ctor(var_ptr);

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
		zend_string *str = _zval_get_string_func(z);

		if (ZSTR_LEN(str) != 0) {
			zend_write(ZSTR_VAL(str), ZSTR_LEN(str));
		} else if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(z) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(z, BP_VAR_R);
		}
		zend_string_release(str);
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
	zend_string *name;
	HashTable *target_symbol_table;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

 	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
		zend_string_addref(name);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		name = zval_get_string(varname);
	}

	target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
	retval = zend_hash_find(target_symbol_table, name);
	if (retval == NULL) {
		if (UNEXPECTED(zend_string_equals(name, CG(known_strings)[ZEND_STR_THIS]))) {
			zval *result;

ZEND_VM_C_LABEL(fetch_this):
			result = EX_VAR(opline->result.var);
			switch (type) {
				case BP_VAR_R:
					if (EXPECTED(Z_TYPE(EX(This)) == IS_OBJECT)) {
						ZVAL_OBJ(result, Z_OBJ(EX(This)));
						Z_ADDREF_P(result);
					} else {
						ZVAL_NULL(result);
						zend_error(E_NOTICE,"Undefined variable: this");
					}
					break;
				case BP_VAR_IS:
					if (EXPECTED(Z_TYPE(EX(This)) == IS_OBJECT)) {
						ZVAL_OBJ(result, Z_OBJ(EX(This)));
						Z_ADDREF_P(result);
					} else {
						ZVAL_NULL(result);
					}
					break;
				case BP_VAR_RW:
				case BP_VAR_W:
					zend_throw_error(NULL, "Cannot re-assign $this");
					break;
				case BP_VAR_UNSET:
					zend_throw_error(NULL, "Cannot unset $this");
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
			if (OP1_TYPE != IS_CONST) {
				zend_string_release(name);
			}
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
		switch (type) {
			case BP_VAR_R:
			case BP_VAR_UNSET:
				zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
				/* break missing intentionally */
			case BP_VAR_IS:
				retval = &EG(uninitialized_zval);
				break;
			case BP_VAR_RW:
				zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
				retval = zend_hash_update(target_symbol_table, name, &EG(uninitialized_zval));
				break;
			case BP_VAR_W:
				retval = zend_hash_add_new(target_symbol_table, name, &EG(uninitialized_zval));
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	/* GLOBAL or $$name variable may be an INDIRECT pointer to CV */
	} else if (Z_TYPE_P(retval) == IS_INDIRECT) {
		retval = Z_INDIRECT_P(retval);
		if (Z_TYPE_P(retval) == IS_UNDEF) {
			if (UNEXPECTED(zend_string_equals(name, CG(known_strings)[ZEND_STR_THIS]))) {
				ZEND_VM_C_GOTO(fetch_this);
			}
			switch (type) {
				case BP_VAR_R:
				case BP_VAR_UNSET:
					zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
					/* break missing intentionally */
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined variable: %s", ZSTR_VAL(name));
					/* break missing intentionally */
				case BP_VAR_W:
					ZVAL_NULL(retval);
					break;
				EMPTY_SWITCH_DEFAULT_CASE()
			}
		}
	}

	if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) != ZEND_FETCH_GLOBAL_LOCK) {
		FREE_OP1();
	}

	if (OP1_TYPE != IS_CONST) {
		zend_string_release(name);
	}

	ZEND_ASSERT(retval != NULL);
	if (type == BP_VAR_R || type == BP_VAR_IS) {
		if (/*type == BP_VAR_R &&*/ Z_ISREF_P(retval) && Z_REFCOUNT_P(retval) == 1) {
			ZVAL_UNREF(retval);
		}
		ZVAL_COPY(EX_VAR(opline->result.var), retval);
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

ZEND_VM_HANDLER(92, ZEND_FETCH_FUNC_ARG, CONST|TMPVAR|CV, UNUSED, VAR_FETCH|ARG_NUM)
{
	USE_OPLINE

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_W);
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_var_address_helper, type, BP_VAR_R);
	}
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
	zend_string *name;
	zend_class_entry *ce;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

 	if (OP1_TYPE == IS_CONST) {
		name = Z_STR_P(varname);
	} else if (EXPECTED(Z_TYPE_P(varname) == IS_STRING)) {
		name = Z_STR_P(varname);
		zend_string_addref(name);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		name = zval_get_string(varname);
	}

	if (OP2_TYPE == IS_CONST) {
		if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)))) != NULL)) {
			retval = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)) + sizeof(void*));

			/* check if static properties were destoyed */
			if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
				zend_throw_error(NULL, "Access to undeclared static property: %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
				FREE_OP1();
				HANDLE_EXCEPTION();
			}

			ZEND_VM_C_GOTO(fetch_static_prop_return);
		} else if (UNEXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)))) == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				if (OP1_TYPE != IS_CONST) {
					zend_string_release(name);
				}
				FREE_OP1();
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}
			CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce);
		}
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				if (OP1_TYPE != IS_CONST) {
					zend_string_release(name);
				}
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		if (OP1_TYPE == IS_CONST &&
		    (retval = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce)) != NULL) {
				
			/* check if static properties were destoyed */
			if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
				zend_throw_error(NULL, "Access to undeclared static property: %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
				FREE_OP1();
				HANDLE_EXCEPTION();
			}

			ZEND_VM_C_GOTO(fetch_static_prop_return);
		}
	}
	retval = zend_std_get_static_property(ce, name, 0);
	if (UNEXPECTED(EG(exception))) {
		if (OP1_TYPE != IS_CONST) {
			zend_string_release(name);
		}
		FREE_OP1();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_CONST && retval) {
		CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce, retval);
	}

	FREE_OP1();

	if (OP1_TYPE != IS_CONST) {
		zend_string_release(name);
	}

ZEND_VM_C_LABEL(fetch_static_prop_return):
	ZEND_ASSERT(retval != NULL);
	if (type == BP_VAR_R || type == BP_VAR_IS) {
		if (/*type == BP_VAR_R &&*/ Z_ISREF_P(retval) && Z_REFCOUNT_P(retval) == 1) {
			ZVAL_UNREF(retval);
		}
		ZVAL_COPY(EX_VAR(opline->result.var), retval);
	} else {
		ZVAL_INDIRECT(EX_VAR(opline->result.var), retval);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(173, ZEND_FETCH_STATIC_PROP_R, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(174, ZEND_FETCH_STATIC_PROP_W, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(175, ZEND_FETCH_STATIC_PROP_RW, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(177, ZEND_FETCH_STATIC_PROP_FUNC_ARG, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, NUM)
{
	USE_OPLINE

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_W);
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_R);
	}
}

ZEND_VM_HANDLER(178, ZEND_FETCH_STATIC_PROP_UNSET, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(176, ZEND_FETCH_STATIC_PROP_IS, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER(zend_fetch_static_prop_helper, type, BP_VAR_IS);
}

ZEND_VM_HANDLER(81, ZEND_FETCH_DIM_R, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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
			value = zend_fetch_dimension_address_inner(Z_ARRVAL_P(container), dim, OP2_TYPE, BP_VAR_R);
			result = EX_VAR(opline->result.var);
			ZVAL_COPY(result, value);
		} else if (EXPECTED(Z_TYPE_P(container) == IS_REFERENCE)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(fetch_dim_r_array);
			} else {
				ZEND_VM_C_GOTO(fetch_dim_r_slow);
			}
		} else {
ZEND_VM_C_LABEL(fetch_dim_r_slow):
			result = EX_VAR(opline->result.var);
			zend_fetch_dimension_address_read_R_slow(result, container, dim);
		}
	} else {
		result = EX_VAR(opline->result.var);
		zend_fetch_dimension_address_read_R(result, container, dim, OP2_TYPE);
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

	zend_fetch_dimension_address_W(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	zend_fetch_dimension_address_RW(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(90, ZEND_FETCH_DIM_IS, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_IS);
	zend_fetch_dimension_address_read_IS(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|NEXT|CV, NUM)
{
	USE_OPLINE
	zval *container;
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
        if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
            zend_throw_error(NULL, "Cannot use temporary expression in write context");
			FREE_UNFETCHED_OP2();
			FREE_UNFETCHED_OP1();
			HANDLE_EXCEPTION();
        }
		container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
		zend_fetch_dimension_address_W(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
		if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
			EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
		}
		FREE_OP2();
		FREE_OP1_VAR_PTR();
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			zend_throw_error(NULL, "Cannot use [] for reading");
			FREE_UNFETCHED_OP2();
			FREE_UNFETCHED_OP1();
			HANDLE_EXCEPTION();
		}
		container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
		zend_fetch_dimension_address_read_R(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
		FREE_OP2();
		FREE_OP1();
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(96, ZEND_FETCH_DIM_UNSET, VAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_UNSET);

	zend_fetch_dimension_address_UNSET(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(82, ZEND_FETCH_OBJ_R, CONST|TMP|VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	offset = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
				ZEND_VM_C_GOTO(fetch_obj_r_no_object);
			}
		} else {
			ZEND_VM_C_GOTO(fetch_obj_r_no_object);
		}
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zval *retval;

		if (OP2_TYPE == IS_CONST &&
			EXPECTED(zobj->ce == CACHED_PTR(Z_CACHE_SLOT_P(offset)))) {
			uint32_t prop_offset = (uint32_t)(intptr_t)CACHED_PTR(Z_CACHE_SLOT_P(offset) + sizeof(void*));

			if (EXPECTED(prop_offset != (uint32_t)ZEND_DYNAMIC_PROPERTY_OFFSET)) {
				retval = OBJ_PROP(zobj, prop_offset);
				if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
					ZVAL_COPY(EX_VAR(opline->result.var), retval);
					break;
				}
			} else if (EXPECTED(zobj->properties != NULL)) {
				retval = zend_hash_find(zobj->properties, Z_STR_P(offset));
				if (EXPECTED(retval)) {
					ZVAL_COPY(EX_VAR(opline->result.var), retval);
					break;
				}
			}
		}

		if (UNEXPECTED(zobj->handlers->read_property == NULL)) {
ZEND_VM_C_LABEL(fetch_obj_r_no_object):
			zend_error(E_NOTICE, "Trying to get property of non-object");
			ZVAL_NULL(EX_VAR(opline->result.var));
		} else {
			retval = zobj->handlers->read_property(container, offset, BP_VAR_R, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(offset)) : NULL), EX_VAR(opline->result.var));

			if (retval != EX_VAR(opline->result.var)) {
				ZVAL_COPY(EX_VAR(opline->result.var), retval);
			}
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_W);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}
	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_RW);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(91, ZEND_FETCH_OBJ_IS, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	offset  = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_CONST ||
	    (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT))) {
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (UNEXPECTED(Z_TYPE_P(container) != IS_OBJECT)) {
				ZEND_VM_C_GOTO(fetch_obj_is_no_object);
			}
		} else {
			ZEND_VM_C_GOTO(fetch_obj_is_no_object);
		}
	}

	/* here we are sure we are dealing with an object */
	do {
		zend_object *zobj = Z_OBJ_P(container);
		zval *retval;

		if (OP2_TYPE == IS_CONST &&
			EXPECTED(zobj->ce == CACHED_PTR(Z_CACHE_SLOT_P(offset)))) {
			uint32_t prop_offset = (uint32_t)(intptr_t)CACHED_PTR(Z_CACHE_SLOT_P(offset) + sizeof(void*));

			if (EXPECTED(prop_offset != (uint32_t)ZEND_DYNAMIC_PROPERTY_OFFSET)) {
				retval = OBJ_PROP(zobj, prop_offset);
				if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
					ZVAL_COPY(EX_VAR(opline->result.var), retval);
					break;
				}
			} else if (EXPECTED(zobj->properties != NULL)) {
				retval = zend_hash_find(zobj->properties, Z_STR_P(offset));
				if (EXPECTED(retval)) {
					ZVAL_COPY(EX_VAR(opline->result.var), retval);
					break;
				}
			}
		}

		if (UNEXPECTED(zobj->handlers->read_property == NULL)) {
ZEND_VM_C_LABEL(fetch_obj_is_no_object):
			ZVAL_NULL(EX_VAR(opline->result.var));
		} else {

			retval = zobj->handlers->read_property(container, offset, BP_VAR_IS, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(offset)) : NULL), EX_VAR(opline->result.var));

			if (retval != EX_VAR(opline->result.var)) {
				ZVAL_COPY(EX_VAR(opline->result.var), retval);
			}
		}
	} while (0);

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, CONST|TMP|VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, NUM)
{
	USE_OPLINE
	zval *container;

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
		/* Behave like FETCH_OBJ_W */
		zend_free_op free_op1, free_op2;
		zval *property;

		SAVE_OPLINE();
		property = GET_OP2_ZVAL_PTR(BP_VAR_R);
		container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

		if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			zend_throw_error(NULL, "Using $this when not in object context");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		if ((OP1_TYPE & (IS_CONST|IS_TMP_VAR))) {
			zend_throw_error(NULL, "Cannot use temporary expression in write context");
			FREE_OP2();
			FREE_OP1_VAR_PTR();
			HANDLE_EXCEPTION();
		}
		zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_W);
		FREE_OP2();
		if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
			EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_OBJ_R);
	}
}

ZEND_VM_HANDLER(97, ZEND_FETCH_OBJ_UNSET, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container, *property;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_UNSET);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var));
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_LIST, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	zend_fetch_dimension_address_read_LIST(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R));
	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, SPEC(OP_DATA=CONST|TMP|VAR|CV))
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data;
	zval *object, *property_name, *value, tmp;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);
	value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE != IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) != IS_OBJECT)) {
		do {
			if (Z_ISREF_P(object)) {
				object = Z_REFVAL_P(object);
				if (EXPECTED(Z_TYPE_P(object) == IS_OBJECT)) {
					break;
				}
			}
			if (EXPECTED(Z_TYPE_P(object) <= IS_FALSE ||
			    (Z_TYPE_P(object) == IS_STRING && Z_STRLEN_P(object) == 0))) {
				zend_object *obj;

				zval_ptr_dtor(object);
				object_init(object);
				Z_ADDREF_P(object);
				obj = Z_OBJ_P(object);
				zend_error(E_WARNING, "Creating default object from empty value");
				if (GC_REFCOUNT(obj) == 1) {
					/* the enclosing container was deleted, obj is unreferenced */
					if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
					FREE_OP_DATA();
					OBJ_RELEASE(obj);
					ZEND_VM_C_GOTO(exit_assign_obj);
				}
				Z_DELREF_P(object);
			} else {
				if (OP1_TYPE != IS_VAR || EXPECTED(!Z_ISERROR_P(object))) {
					zend_error(E_WARNING, "Attempt to assign property of non-object");
				}
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_NULL(EX_VAR(opline->result.var));
				}
				FREE_OP_DATA();
				ZEND_VM_C_GOTO(exit_assign_obj);
			}
		} while (0);
	}

	if (OP2_TYPE == IS_CONST &&
	    EXPECTED(Z_OBJCE_P(object) == CACHED_PTR(Z_CACHE_SLOT_P(property_name)))) {
		uint32_t prop_offset = (uint32_t)(intptr_t)CACHED_PTR(Z_CACHE_SLOT_P(property_name) + sizeof(void*));
		zend_object *zobj = Z_OBJ_P(object);
		zval *property;

		if (EXPECTED(prop_offset != (uint32_t)ZEND_DYNAMIC_PROPERTY_OFFSET)) {
			property = OBJ_PROP(zobj, prop_offset);
			if (Z_TYPE_P(property) != IS_UNDEF) {
ZEND_VM_C_LABEL(fast_assign_obj):
				value = zend_assign_to_variable(property, value, OP_DATA_TYPE);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), value);
				}
				ZEND_VM_C_GOTO(exit_assign_obj);
			}
		} else {
			if (EXPECTED(zobj->properties != NULL)) {
				if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
					if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
						GC_REFCOUNT(zobj->properties)--;
					}
					zobj->properties = zend_array_dup(zobj->properties);
				}
				property = zend_hash_find(zobj->properties, Z_STR_P(property_name));
				if (property) {
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
							if (--GC_REFCOUNT(ref) == 0) {
								ZVAL_COPY_VALUE(&tmp, Z_REFVAL_P(value));
								efree_size(ref, sizeof(zend_reference));
								value = &tmp;
							} else {
								value = Z_REFVAL_P(value);
								if (Z_REFCOUNTED_P(value)) {
									Z_ADDREF_P(value);
								}
							}
						} else {
							value = Z_REFVAL_P(value);
							if (Z_REFCOUNTED_P(value)) {
								Z_ADDREF_P(value);
							}
						}
					} else if (OP_DATA_TYPE == IS_CV && Z_REFCOUNTED_P(value)) {
						Z_ADDREF_P(value);
					}
				}
				zend_hash_add_new(zobj->properties, Z_STR_P(property_name), value);
				if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
					ZVAL_COPY(EX_VAR(opline->result.var), value);
				}
				ZEND_VM_C_GOTO(exit_assign_obj);
			}
		}
	}

	if (!Z_OBJ_HT_P(object)->write_property) {
		zend_error(E_WARNING, "Attempt to assign property of non-object");
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_NULL(EX_VAR(opline->result.var));
		}
		FREE_OP_DATA();
		ZEND_VM_C_GOTO(exit_assign_obj);
	}

	if (OP_DATA_TYPE == IS_CV || OP_DATA_TYPE == IS_VAR) {
		ZVAL_DEREF(value);
	}

	Z_OBJ_HT_P(object)->write_property(object, property_name, value, (OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property_name)) : NULL);

	if (UNEXPECTED(RETURN_VALUE_USED(opline)) && EXPECTED(!EG(exception))) {
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
			variable_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(object_ptr), &EG(uninitialized_zval));
			if (UNEXPECTED(variable_ptr == NULL)) {
				zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
				ZEND_VM_C_GOTO(assign_dim_error);
			}
		} else {
			dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
			if (OP2_TYPE == IS_CONST) {
				variable_ptr = zend_fetch_dimension_address_inner_W_CONST(Z_ARRVAL_P(object_ptr), dim);
			} else {
				variable_ptr = zend_fetch_dimension_address_inner_W(Z_ARRVAL_P(object_ptr), dim);
			}
			if (UNEXPECTED(variable_ptr == NULL)) {
				ZEND_VM_C_GOTO(assign_dim_error);
			}
		}
		value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);
		value = zend_assign_to_variable(variable_ptr, value, OP_DATA_TYPE);
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
			value = GET_OP_DATA_ZVAL_PTR(BP_VAR_R);

			zend_assign_to_object_dim(object_ptr, dim, value);

			if (UNEXPECTED(RETURN_VALUE_USED(opline)) && EXPECTED(!EG(exception))) {
				ZVAL_COPY(EX_VAR(opline->result.var), value);
			}

			FREE_OP_DATA();
		} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
			if (OP2_TYPE == IS_UNUSED) {
				zend_throw_error(NULL, "[] operator not supported for strings");
				FREE_UNFETCHED_OP_DATA();
				FREE_OP1_VAR_PTR();
				HANDLE_EXCEPTION();
			} else {
				dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
				value = GET_OP_DATA_ZVAL_PTR_DEREF(BP_VAR_R);
				zend_assign_to_string_offset(object_ptr, dim, value, (UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL));
				FREE_OP_DATA();
			}
		} else if (EXPECTED(Z_TYPE_P(object_ptr) <= IS_FALSE)) {
			ZVAL_NEW_ARR(object_ptr);
			zend_hash_init(Z_ARRVAL_P(object_ptr), 8, NULL, ZVAL_PTR_DTOR, 0);
			ZEND_VM_C_GOTO(try_assign_dim_array);
		} else {
			if (OP1_TYPE != IS_VAR || EXPECTED(!Z_ISERROR_P(object_ptr))) {
				zend_error(E_WARNING, "Cannot use a scalar value as an array");
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

	if (OP1_TYPE == IS_VAR &&
	    UNEXPECTED(Z_TYPE_P(EX_VAR(opline->op1.var)) != IS_INDIRECT) &&
	    UNEXPECTED(!Z_ISREF_P(EX_VAR(opline->op1.var))) &&
	    UNEXPECTED(!Z_ISERROR_P(EX_VAR(opline->op1.var)))) {

		zend_throw_error(NULL, "Cannot assign by reference to overloaded object");
		FREE_OP2_VAR_PTR();
		HANDLE_EXCEPTION();

	} else if (OP2_TYPE == IS_VAR &&
	           opline->extended_value == ZEND_RETURNS_FUNCTION &&
			   UNEXPECTED(!Z_ISREF_P(value_ptr))) {
		zend_error(E_NOTICE, "Only variables should be assigned by reference");
		if (UNEXPECTED(EG(exception) != NULL)) {
			FREE_OP2_VAR_PTR();
			HANDLE_EXCEPTION();
		}

		value_ptr = zend_assign_to_variable(variable_ptr, value_ptr, OP2_TYPE);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), value_ptr);
		}
		/* zend_assign_to_variable() always takes care of op2, never free it! */

	} else {

		if ((OP1_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(variable_ptr))) ||
		    (OP2_TYPE == IS_VAR && UNEXPECTED(Z_ISERROR_P(value_ptr)))) {
			variable_ptr = &EG(uninitialized_zval);
		} else {
			zend_assign_to_variable_reference(variable_ptr, value_ptr);
		}

		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
		}

		FREE_OP2_VAR_PTR();
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_leave_helper, ANY, ANY)
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
				GC_REFCOUNT(object)--;
				if (GC_REFCOUNT(object) == 1) {
					zend_object_store_ctor_failed(object);
				}
			}
			OBJ_RELEASE(object);
		} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE((zend_object*)execute_data->func->op_array.prototype);
		}
		EG(vm_stack_top) = (zval*)execute_data;
		execute_data = EX(prev_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			const zend_op *old_opline = EX(opline);
			zend_throw_exception_internal(NULL);
			if (RETURN_VALUE_USED(old_opline)) {
				zval_ptr_dtor(EX_VAR(old_opline->result.var));
			}
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
				GC_REFCOUNT(object)--;
				if (GC_REFCOUNT(object) == 1) {
					zend_object_store_ctor_failed(object);
				}
			}
			OBJ_RELEASE(object);
		} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE((zend_object*)execute_data->func->op_array.prototype);
		}

		zend_vm_stack_free_extra_args_ex(call_info, execute_data);
		old_execute_data = execute_data;
		execute_data = EX(prev_execute_data);
		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			const zend_op *old_opline = EX(opline);
			zend_throw_exception_internal(NULL);
			if (RETURN_VALUE_USED(old_opline)) {
				zval_ptr_dtor(EX_VAR(old_opline->result.var));
			}
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
			zend_throw_exception_internal(NULL);
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
				OBJ_RELEASE((zend_object*)EX(func)->op_array.prototype);
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

ZEND_VM_HANDLER(42, ZEND_JMP, JMP_ADDR, ANY)
{
	USE_OPLINE

	ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op1));
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(43, ZEND_JMPZ, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	
	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_SET_NEXT_OPCODE(opline + 1);
		ZEND_VM_CONTINUE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
			ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
		} else {
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op2));
			ZEND_VM_CONTINUE();
		}
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline++;
	} else {
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(44, ZEND_JMPNZ, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op2));
		ZEND_VM_CONTINUE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
			SAVE_OPLINE();
			GET_OP1_UNDEF_CV(val, BP_VAR_R);
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			ZEND_VM_NEXT_OPCODE();
		}
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline = OP_JMP_ADDR(opline, opline->op2);
	} else {
		opline++;
	}
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(45, ZEND_JMPZNZ, CONST|TMPVAR|CV, JMP_ADDR, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (EXPECTED(Z_TYPE_INFO_P(val) == IS_TRUE)) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		if (OP1_TYPE == IS_CV) {
			if (UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
				SAVE_OPLINE();
				GET_OP1_UNDEF_CV(val, BP_VAR_R);
			}
			ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
		} else {
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op2));
			ZEND_VM_CONTINUE();
		}
	}

	SAVE_OPLINE();
	if (i_zend_is_true(val)) {
		opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
	} else {
		opline = OP_JMP_ADDR(opline, opline->op2);
	}
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(46, ZEND_JMPZ_EX, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_SET_NEXT_OPCODE(opline + 1);
		ZEND_VM_CONTINUE();
	} else if (EXPECTED(Z_TYPE_INFO_P(val) <= IS_TRUE)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
		if (OP1_TYPE == IS_CV) {
			if (UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
				SAVE_OPLINE();
				GET_OP1_UNDEF_CV(val, BP_VAR_R);
			}
			ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
		} else {
			ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op2));
			ZEND_VM_CONTINUE();
		}
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
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMPVAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *val;
	int ret;

	val = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (Z_TYPE_INFO_P(val) == IS_TRUE) {
		ZVAL_TRUE(EX_VAR(opline->result.var));
		ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op2));
		ZEND_VM_CONTINUE();
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
	if (UNEXPECTED(EG(exception) != NULL)) {
		HANDLE_EXCEPTION();
	}
	ZEND_VM_JMP(opline);
}

ZEND_VM_HANDLER(70, ZEND_FREE, TMPVAR, LIVE_RANGE)
{
	USE_OPLINE

	SAVE_OPLINE();
	zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(127, ZEND_FE_FREE, TMPVAR, LIVE_RANGE)
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

ZEND_VM_HANDLER(53, ZEND_FAST_CONCAT, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *op1, *op2;
	zend_string *op1_str, *op2_str, *str;

	SAVE_OPLINE();
	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CONST) {
		op1_str = Z_STR_P(op1);
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		op1_str = zend_string_copy(Z_STR_P(op1));
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op1) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(op1, BP_VAR_R);
		}
		op1_str = _zval_get_string_func(op1);
	}
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP2_TYPE == IS_CONST) {
		op2_str = Z_STR_P(op2);
	} else if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		op2_str = zend_string_copy(Z_STR_P(op2));
	} else {
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(op2) == IS_UNDEF)) {
			GET_OP2_UNDEF_CV(op2, BP_VAR_R);
		}
		op2_str = _zval_get_string_func(op2);
	}
	do {
		if (OP1_TYPE != IS_CONST) {
			if (UNEXPECTED(ZSTR_LEN(op1_str) == 0)) {
				if (OP2_TYPE == IS_CONST) {
					zend_string_addref(op2_str);
				}
				ZVAL_STR(EX_VAR(opline->result.var), op2_str);
				zend_string_release(op1_str);
				break;
			}
		}
		if (OP2_TYPE != IS_CONST) {
			if (UNEXPECTED(ZSTR_LEN(op2_str) == 0)) {
				if (OP1_TYPE == IS_CONST) {
					zend_string_addref(op1_str);
				}
				ZVAL_STR(EX_VAR(opline->result.var), op1_str);
				zend_string_release(op2_str);
				break;
			}
		}
		str = zend_string_alloc(ZSTR_LEN(op1_str) + ZSTR_LEN(op2_str), 0);
		memcpy(ZSTR_VAL(str), ZSTR_VAL(op1_str), ZSTR_LEN(op1_str));
		memcpy(ZSTR_VAL(str) + ZSTR_LEN(op1_str), ZSTR_VAL(op2_str), ZSTR_LEN(op2_str)+1);
		ZVAL_NEW_STR(EX_VAR(opline->result.var), str);
		if (OP1_TYPE != IS_CONST) {
			zend_string_release(op1_str);
		}
		if (OP2_TYPE != IS_CONST) {
			zend_string_release(op2_str);
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
		rope[0] = zend_string_copy(Z_STR_P(var));
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
			rope[0] = _zval_get_string_func(var);
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
		rope[opline->extended_value] = zend_string_copy(Z_STR_P(var));
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
			rope[opline->extended_value] = _zval_get_string_func(var);
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
		rope[opline->extended_value] = zend_string_copy(Z_STR_P(var));
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
			rope[opline->extended_value] = _zval_get_string_func(var);
			FREE_OP2();
			if (UNEXPECTED(EG(exception))) {
				for (i = 0; i <= opline->extended_value; i++) {
					zend_string_release(rope[i]);
				}
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
		zend_string_release(rope[i]);
	}
	*target = '\0';

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(109, ZEND_FETCH_CLASS, ANY, CONST|TMPVAR|UNUSED|CV, CLASS_FETCH)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP2_TYPE == IS_UNUSED) {
		Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(NULL, opline->extended_value);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		zend_free_op free_op2;
		zval *class_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_class_name):
		if (OP2_TYPE == IS_CONST) {
			zend_class_entry *ce = CACHED_PTR(Z_CACHE_SLOT_P(class_name));

			if (UNEXPECTED(ce == NULL)) {
				ce = zend_fetch_class_by_name(Z_STR_P(class_name), EX_CONSTANT(opline->op2) + 1, opline->extended_value);
				CACHE_PTR(Z_CACHE_SLOT_P(class_name), ce);
			}
			Z_CE_P(EX_VAR(opline->result.var)) = ce;
		} else if (Z_TYPE_P(class_name) == IS_OBJECT) {
			Z_CE_P(EX_VAR(opline->result.var)) = Z_OBJCE_P(class_name);
		} else if (Z_TYPE_P(class_name) == IS_STRING) {
			Z_CE_P(EX_VAR(opline->result.var)) = zend_fetch_class(Z_STR_P(class_name), opline->extended_value);
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

		FREE_OP2();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
}

ZEND_VM_HANDLER(112, ZEND_INIT_METHOD_CALL, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, NUM)
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

	function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

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
					HANDLE_EXCEPTION();
				}
			}
			zend_throw_error(NULL, "Method name must be a string");
			FREE_OP2();
			FREE_UNFETCHED_OP1();
			HANDLE_EXCEPTION();
		} while (0);
	}

	object = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(object) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_OP2();
		HANDLE_EXCEPTION();
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
						FREE_OP2();
						HANDLE_EXCEPTION();
					}
				}
				zend_throw_error(NULL, "Call to a member function %s() on %s", Z_STRVAL_P(function_name), zend_get_type_by_const(Z_TYPE_P(object)));
				FREE_OP2();
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} while (0);
	}

	obj = Z_OBJ_P(object);
	called_scope = obj->ce;

	if (OP2_TYPE != IS_CONST ||
	    UNEXPECTED((fbc = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(function_name), called_scope)) == NULL)) {
	    zend_object *orig_obj = obj;

		if (UNEXPECTED(obj->handlers->get_method == NULL)) {
			zend_throw_error(NULL, "Object does not support method calls");
			FREE_OP2();
			FREE_OP1();
			HANDLE_EXCEPTION();
		}

		/* First, locate the function. */
		fbc = obj->handlers->get_method(&obj, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (EX_CONSTANT(opline->op2) + 1) : NULL));
		if (UNEXPECTED(fbc == NULL)) {
			if (EXPECTED(!EG(exception))) {
				zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(obj->ce->name), Z_STRVAL_P(function_name));
			}
			FREE_OP2();
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(fbc->type <= ZEND_USER_FUNCTION) &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE))) &&
		    EXPECTED(obj == orig_obj)) {
			CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(function_name), called_scope, fbc);
		}
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	call_info = ZEND_CALL_NESTED_FUNCTION;
	if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0)) {
		obj = NULL;
	} else if (OP1_TYPE & (IS_VAR|IS_TMP_VAR|IS_CV)) {
		/* CV may be changed indirectly (e.g. when it's a reference) */
		call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_RELEASE_THIS;
		GC_REFCOUNT(obj)++; /* For $this pointer */
	}

	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, called_scope, obj);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	FREE_OP2();
	FREE_OP1();

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, UNUSED|CLASS_FETCH|CONST|VAR, CONST|TMPVAR|UNUSED|CONSTRUCTOR|CV, NUM)
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
		ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)));
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op1)), EX_CONSTANT(opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT |  ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
				zend_throw_error(NULL, "Class '%s' not found", Z_STRVAL_P(EX_CONSTANT(opline->op1)));
				HANDLE_EXCEPTION();
			}
			CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce);
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
	    EXPECTED((fbc = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)))) != NULL)) {
		/* nothing to do */
	} else if (OP1_TYPE != IS_CONST &&
	           OP2_TYPE == IS_CONST &&
	           (fbc = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce))) {
		/* do nothing */
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
			fbc = zend_std_get_static_method(ce, Z_STR_P(function_name), ((OP2_TYPE == IS_CONST) ? (EX_CONSTANT(opline->op2) + 1) : NULL));
		}
		if (UNEXPECTED(fbc == NULL)) {
			if (EXPECTED(!EG(exception))) {
				zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(ce->name), Z_STRVAL_P(function_name));
			}
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		if (OP2_TYPE == IS_CONST &&
		    EXPECTED(fbc->type <= ZEND_USER_FUNCTION) &&
		    EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE)))) {
			if (OP1_TYPE == IS_CONST) {
				CACHE_PTR(Z_CACHE_SLOT_P(function_name), fbc);
			} else {
				CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(function_name), ce, fbc);
			}
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
			if (fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
				/* Allowed for PHP 4 compatibility. */
				zend_error(
					E_DEPRECATED,
					"Non-static method %s::%s() should not be called statically",
					ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			} else {
				/* An internal function assumes $this is present and won't check that.
				 * So PHP would crash by allowing the call. */
				zend_throw_error(
					zend_ce_error,
					"Non-static method %s::%s() cannot be called statically",
					ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
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

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST, NUM)
{
	USE_OPLINE
	zend_function *fbc;
	zval *function_name, *func;
	zend_execute_data *call;

	function_name = (zval*)EX_CONSTANT(opline->op2);
	fbc = CACHED_PTR(Z_CACHE_SLOT_P(function_name));
	if (UNEXPECTED(fbc == NULL)) {
		func = zend_hash_find(EG(function_table), Z_STR_P(function_name+1));
		if (UNEXPECTED(func == NULL)) {
			SAVE_OPLINE();
			zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(function_name));
			HANDLE_EXCEPTION();
		}
		fbc = Z_FUNC_P(func);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
		CACHE_PTR(Z_CACHE_SLOT_P(function_name), fbc);
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

	FREE_OP2();

	if (UNEXPECTED(!call)) {
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
		if (func->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			if (OP2_TYPE & (IS_VAR|IS_CV)) {
				ZVAL_DEREF(function_name);
			}
			ZEND_ASSERT(GC_TYPE((zend_object*)func->common.prototype) == IS_OBJECT);
			GC_REFCOUNT((zend_object*)func->common.prototype)++;
			call_info |= ZEND_CALL_CLOSURE;
		} else if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS;
			GC_REFCOUNT(object)++; /* For $this pointer */
		}
		if (error) {
			efree(error);
			/* This is the only soft error is_callable() can generate */
			zend_error(E_DEPRECATED,
				"Non-static method %s::%s() should not be called statically",
				ZSTR_VAL(func->common.scope->name), ZSTR_VAL(func->common.function_name));
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
		if (EXPECTED(func->type == ZEND_USER_FUNCTION) && UNEXPECTED(!func->op_array.run_time_cache)) {
			init_func_run_time_cache(&func->op_array);
		}
	} else {
		zend_internal_type_error(EX_USES_STRICT_TYPES(), "%s() expects parameter 1 to be a valid callback, %s", Z_STRVAL_P(EX_CONSTANT(opline->op1)), error);
		efree(error);
		func = (zend_function*)&zend_pass_function;
		called_scope = NULL;
		object = NULL;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		func, opline->extended_value, called_scope, object);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	FREE_OP2();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(69, ZEND_INIT_NS_FCALL_BY_NAME, ANY, CONST, NUM)
{
	USE_OPLINE
	zval *func_name;
	zval *func;
	zend_function *fbc;
	zend_execute_data *call;

	func_name = EX_CONSTANT(opline->op2) + 1;
	fbc = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
	if (UNEXPECTED(fbc == NULL)) {
		func = zend_hash_find(EG(function_table), Z_STR_P(func_name));
		if (func == NULL) {
			func_name++;
			func = zend_hash_find(EG(function_table), Z_STR_P(func_name));
			if (UNEXPECTED(func == NULL)) {
				SAVE_OPLINE();
				zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(EX_CONSTANT(opline->op2)));
				HANDLE_EXCEPTION();
			}
		}
		fbc = Z_FUNC_P(func);
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), fbc);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(61, ZEND_INIT_FCALL, NUM, CONST, NUM)
{
	USE_OPLINE
	zend_free_op free_op2;
	zval *fname = GET_OP2_ZVAL_PTR(BP_VAR_R);
	zval *func;
	zend_function *fbc;
	zend_execute_data *call;

	fbc = CACHED_PTR(Z_CACHE_SLOT_P(fname));
	if (UNEXPECTED(fbc == NULL)) {
		func = zend_hash_find(EG(function_table), Z_STR_P(fname));
		if (UNEXPECTED(func == NULL)) {
		    SAVE_OPLINE();
			zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(fname));
			HANDLE_EXCEPTION();
		}
		fbc = Z_FUNC_P(func);
		CACHE_PTR(Z_CACHE_SLOT_P(fname), fbc);
		if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
	}

	call = zend_vm_stack_push_call_frame_ex(
		opline->op1.num, ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(129, ZEND_DO_ICALL, ANY, ANY, SPEC(RETVAL))
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
	ZEND_ASSERT(
		EG(exception) || !call->func ||
		!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
		zend_verify_internal_return_type(call->func, ret));
	ZEND_ASSERT(!Z_ISREF_P(ret));
#endif

	EG(current_execute_data) = call->prev_execute_data;
	zend_vm_stack_free_args(call);
	zend_vm_stack_free_call_frame(call);

	if (!RETURN_VALUE_USED(opline)) {
		zval_ptr_dtor(ret);
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}

	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(130, ZEND_DO_UCALL, ANY, ANY, SPEC(RETVAL))
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
	i_init_func_execute_data(call, &fbc->op_array, ret);

	ZEND_VM_ENTER();
}

ZEND_VM_HANDLER(131, ZEND_DO_FCALL_BY_NAME, ANY, ANY, SPEC(RETVAL))
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
		i_init_func_execute_data(call, &fbc->op_array, ret);

		ZEND_VM_ENTER();
	} else {
		zval retval;
		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);

		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
				fbc->common.scope ? "::" : "",
				ZSTR_VAL(fbc->common.function_name));
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
		 && UNEXPECTED(!zend_verify_internal_arg_types(fbc, call))) {
			zend_vm_stack_free_call_frame(call);
			zend_throw_exception_internal(NULL);
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

		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);
		zend_vm_stack_free_call_frame(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(ret);
		}
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}
	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(60, ZEND_DO_FCALL, ANY, ANY, SPEC(RETVAL))
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
			zend_throw_error(NULL, "Cannot call abstract method %s::%s()", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
			HANDLE_EXCEPTION();
		}
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_DEPRECATED) != 0)) {
			zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
				fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
				fbc->common.scope ? "::" : "",
				ZSTR_VAL(fbc->common.function_name));
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
		}
	}

	LOAD_OPLINE();

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		ret = NULL;
		if (RETURN_VALUE_USED(opline)) {
			ret = EX_VAR(opline->result.var);
			ZVAL_NULL(ret);
		}

		call->prev_execute_data = execute_data;
		i_init_func_execute_data(call, &fbc->op_array, ret);

		if (EXPECTED(zend_execute_ex == execute_ex)) {
			ZEND_VM_ENTER();
		} else {
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
			zend_execute_ex(call);
		}
	} else if (EXPECTED(fbc->type < ZEND_USER_FUNCTION)) {
		zval retval;

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS)
		  && UNEXPECTED(!zend_verify_internal_arg_types(fbc, call))) {
			if (RETURN_VALUE_USED(opline)) {
				ZVAL_UNDEF(EX_VAR(opline->result.var));
			}
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

		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(ret);
		}

	} else { /* ZEND_OVERLOADED_FUNCTION */
		zval retval;

		ret = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : &retval;

		call->prev_execute_data = execute_data;

		if (UNEXPECTED(!zend_do_fcall_overloaded(fbc, call, ret))) {
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
			GC_REFCOUNT(object)--;
			if (GC_REFCOUNT(object) == 1) {
				zend_object_store_ctor_failed(object);
			}
		}
		OBJ_RELEASE(object);
	}

	zend_vm_stack_free_call_frame(call);
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}

	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(124, ZEND_VERIFY_RETURN_TYPE, CONST|TMP|VAR|UNUSED|CV, UNUSED)
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

		if (UNEXPECTED(!ret_info->class_name
			&& ret_info->type_hint != IS_CALLABLE
			&& !ZEND_SAME_FAKE_TYPE(ret_info->type_hint, Z_TYPE_P(retval_ptr))
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

		if (UNEXPECTED(EG(exception) != NULL)) {
			if (OP1_TYPE == IS_CONST) {
				zval_ptr_dtor_nogc(retval_ptr);
			}
		}
#endif
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(62, ZEND_RETURN, CONST|TMP|VAR|CV, ANY)
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
				zval_dtor_func(Z_COUNTED_P(free_op1));
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
				if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
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

ZEND_VM_HANDLER(111, ZEND_RETURN_BY_REF, CONST|TMP|VAR|CV, ANY, SRC)
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
					if (Z_REFCOUNTED_P(retval_ptr)) Z_ADDREF_P(retval_ptr);
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
			ZVAL_MAKE_REF(retval_ptr);
			Z_ADDREF_P(retval_ptr);
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
		used_stack = (ZEND_CALL_FRAME_SLOT + num_args + EX(func)->op_array.last_var + EX(func)->op_array.T - MIN(EX(func)->op_array.num_args, num_args)) * sizeof(zval);
		gen_execute_data = (zend_execute_data*)emalloc(used_stack);
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
		 && !(call_info & ((ZEND_CALL_CLOSURE|ZEND_CALL_RELEASE_THIS) << ZEND_CALL_INFO_SHIFT))) {
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

	zend_generator *generator = zend_get_running_generator(execute_data);

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
		ZVAL_DEREF(retval);
		ZVAL_COPY(&generator->retval, retval);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_refcounted *ref = Z_COUNTED_P(retval);

			retval = Z_REFVAL_P(retval);
			ZVAL_COPY_VALUE(&generator->retval, retval);
			if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
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

ZEND_VM_HANDLER(108, ZEND_THROW, CONST|TMP|VAR|CV, ANY)
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
		if (Z_REFCOUNTED_P(value)) Z_ADDREF_P(value);
	}

	zend_throw_exception_object(value);
	zend_exception_restore();
	FREE_OP1_IF_VAR();
	HANDLE_EXCEPTION();
}

ZEND_VM_HANDLER(107, ZEND_CATCH, CONST, CV, JMP_ADDR)
{
	USE_OPLINE
	zend_class_entry *ce, *catch_ce;
	zend_object *exception;

	SAVE_OPLINE();
	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore();
	if (EG(exception) == NULL) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}
	catch_ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)));
	if (UNEXPECTED(catch_ce == NULL)) {
		catch_ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op1)), EX_CONSTANT(opline->op1) + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD);

		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), catch_ce);
	}
	ce = EG(exception)->ce;

#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_CAUGHT_ENABLED()) {
		DTRACE_EXCEPTION_CAUGHT((char *)ce->name);
	}
#endif /* HAVE_DTRACE */

	if (ce != catch_ce) {
		if (!catch_ce || !instanceof_function(ce, catch_ce)) {
			if (opline->result.num) {
				zend_throw_exception_internal(NULL);
				HANDLE_EXCEPTION();
			}
			ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
			ZEND_VM_CONTINUE();
		}
	}

	exception = EG(exception);
	zval_ptr_dtor(EX_VAR(opline->op2.var));
	ZVAL_OBJ(EX_VAR(opline->op2.var), EG(exception));
	if (UNEXPECTED(EG(exception) != exception)) {
		GC_REFCOUNT(EG(exception))++;
		HANDLE_EXCEPTION();
	} else {
		EG(exception) = NULL;
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(65, ZEND_SEND_VAL, CONST|TMPVAR, NUM)
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

ZEND_VM_HANDLER(116, ZEND_SEND_VAL_EX, CONST|TMP, NUM, SPEC(QUICK_ARG))
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
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot pass parameter %d by reference", arg_num);
		FREE_UNFETCHED_OP1();
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_UNDEF(arg);
		HANDLE_EXCEPTION();
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

ZEND_VM_HANDLER(117, ZEND_SEND_VAR, VAR|CV, NUM)
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
		ZVAL_OPT_DEREF(varptr);
		ZVAL_COPY(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(varptr))) {
			zend_refcounted *ref = Z_COUNTED_P(varptr);

			varptr = Z_REFVAL_P(varptr);
			ZVAL_COPY_VALUE(arg, varptr);
			if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
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

ZEND_VM_HANDLER(50, ZEND_SEND_VAR_NO_REF_EX, VAR, NUM, SPEC(QUICK_ARG))
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
		ZVAL_COPY_VALUE(arg, varptr);
	} else {
		ZVAL_NEW_REF(arg, varptr);
		Z_ADDREF_P(arg);
		ZVAL_REF(varptr, Z_REF_P(arg));
	}

	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(66, ZEND_SEND_VAR_EX, VAR|CV, NUM, SPEC(QUICK_ARG))
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
		ZVAL_OPT_DEREF(varptr);
		ZVAL_COPY(arg, varptr);
	} else /* if (OP1_TYPE == IS_VAR) */ {
		if (UNEXPECTED(Z_ISREF_P(varptr))) {
			zend_refcounted *ref = Z_COUNTED_P(varptr);

			varptr = Z_REFVAL_P(varptr);
			ZVAL_COPY_VALUE(arg, varptr);
			if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
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
				if (Z_REFCOUNT_P(args) == 1) {
					ZVAL_MAKE_REF(arg);
					Z_ADDREF_P(arg);
					ZVAL_REF(top, Z_REF_P(arg));
				} else {
					ZVAL_DUP(top, arg);
				}
			} else if (Z_ISREF_P(arg)) {
				ZVAL_COPY(top, Z_REFVAL_P(arg));
			} else {
				ZVAL_COPY(top, arg);
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

					if (Z_TYPE(key) == IS_STRING) {
						zend_throw_error(NULL,
							"Cannot unpack Traversable with string keys");
						zend_string_release(Z_STR(key));
						break;
					}

					zval_dtor(&key);
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

				if (Z_ISREF_P(arg)) {
					ZVAL_DUP(arg, Z_REFVAL_P(arg));
				} else {
					if (Z_REFCOUNTED_P(arg)) Z_ADDREF_P(arg);
				}

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

ZEND_VM_HANDLER(119, ZEND_SEND_ARRAY, ANY, ANY)
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
			OBJ_RELEASE((zend_object*)EX(call)->func->common.prototype);
		}
		if (Z_TYPE(EX(call)->This) == IS_OBJECT) {
			OBJ_RELEASE(Z_OBJ(EX(call)->This));
		}
		EX(call)->func = (zend_function*)&zend_pass_function;
		Z_OBJ(EX(call)->This) = NULL;
		ZEND_SET_CALL_INFO(EX(call), 0, ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);
	} else {
		uint32_t arg_num;
		HashTable *ht;
		zval *arg, *param;

ZEND_VM_C_LABEL(send_array):
		ht = Z_ARRVAL_P(args);
		zend_vm_stack_extend_call_frame(&EX(call), 0, zend_hash_num_elements(ht));

		arg_num = 1;
		param = ZEND_CALL_ARG(EX(call), 1);
		ZEND_HASH_FOREACH_VAL(ht, arg) {
			if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
				if (UNEXPECTED(!Z_ISREF_P(arg))) {
					if (!ARG_MAY_BE_SENT_BY_REF(EX(call)->func, arg_num)) {

						zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
							arg_num,
							EX(call)->func->common.scope ? ZSTR_VAL(EX(call)->func->common.scope->name) : "",
							EX(call)->func->common.scope ? "::" : "",
							ZSTR_VAL(EX(call)->func->common.function_name));

						if (ZEND_CALL_INFO(EX(call)) & ZEND_CALL_CLOSURE) {
							OBJ_RELEASE((zend_object*)EX(call)->func->common.prototype);
						}
						if (Z_TYPE(EX(call)->This) == IS_OBJECT) {
							OBJ_RELEASE(Z_OBJ(EX(call)->This));
						}
						EX(call)->func = (zend_function*)&zend_pass_function;
						Z_OBJ(EX(call)->This) = NULL;
						ZEND_SET_CALL_INFO(EX(call), 0, ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);
						break;
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
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(120, ZEND_SEND_USER, VAR|CV, NUM)
{
	USE_OPLINE
	zval *arg, *param;
	zend_free_op free_op1;

	SAVE_OPLINE();
	arg = GET_OP1_ZVAL_PTR(BP_VAR_R);
	param = ZEND_CALL_VAR(EX(call), opline->result.var);

	if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, opline->op2.num)) {
		if (UNEXPECTED(!Z_ISREF_P(arg))) {
			if (!ARG_MAY_BE_SENT_BY_REF(EX(call)->func, opline->op2.num)) {

				zend_error(E_WARNING, "Parameter %d to %s%s%s() expected to be a reference, value given",
					opline->op2.num,
					EX(call)->func->common.scope ? ZSTR_VAL(EX(call)->func->common.scope->name) : "",
					EX(call)->func->common.scope ? "::" : "",
					ZSTR_VAL(EX(call)->func->common.function_name));

				if (ZEND_CALL_INFO(EX(call)) & ZEND_CALL_CLOSURE) {
					OBJ_RELEASE((zend_object*)EX(call)->func->common.prototype);
				}
				if (Z_TYPE(EX(call)->This) == IS_OBJECT) {
					OBJ_RELEASE(Z_OBJ(EX(call)->This));
				}
				ZVAL_UNDEF(param);
				EX(call)->func = (zend_function*)&zend_pass_function;
				Z_OBJ(EX(call)->This) = NULL;
				ZEND_SET_CALL_INFO(EX(call), 0, ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);

				FREE_OP1();
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(63, ZEND_RECV, NUM, ANY)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;

	if (UNEXPECTED(arg_num > EX_NUM_ARGS())) {
		SAVE_OPLINE();
		zend_missing_arg_error(execute_data);
		HANDLE_EXCEPTION();
	} else if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
		zval *param = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->result.var);

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_verify_arg_type(EX(func), arg_num, param, NULL, CACHE_ADDR(opline->op2.num)) || EG(exception))) {
			HANDLE_EXCEPTION();
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(64, ZEND_RECV_INIT, NUM, CONST)
{
	USE_OPLINE
	uint32_t arg_num;
	zval *param;

	ZEND_VM_REPEATABLE_OPCODE

	arg_num = opline->op1.num;
	param = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->result.var);
	if (arg_num > EX_NUM_ARGS()) {
		ZVAL_COPY(param, EX_CONSTANT(opline->op2));
		if (Z_OPT_CONSTANT_P(param)) {
			SAVE_OPLINE();
			if (UNEXPECTED(zval_update_constant_ex(param, EX(func)->op_array.scope) != SUCCESS)) {
				ZVAL_UNDEF(param);
				HANDLE_EXCEPTION();
			}
		}
	}

	if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
		zval *default_value = EX_CONSTANT(opline->op2);

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_verify_arg_type(EX(func), arg_num, param, default_value, CACHE_ADDR(Z_CACHE_SLOT_P(default_value))) || EG(exception))) {
			HANDLE_EXCEPTION();
		}
	}

	ZEND_VM_REPEAT_OPCODE(ZEND_RECV_INIT);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(164, ZEND_RECV_VARIADIC, NUM, ANY)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;
	uint32_t arg_count = EX_NUM_ARGS();
	zval *params;

	SAVE_OPLINE();

	params = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->result.var);

	if (arg_num <= arg_count) {
		zval *param;

		array_init_size(params, arg_count - arg_num + 1);
		zend_hash_real_init(Z_ARRVAL_P(params), 1);
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
		array_init(params);
	}

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(52, ZEND_BOOL, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(48, ZEND_CASE, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
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
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					result = 1;
				} else if (Z_STRVAL_P(op1)[0] > '9' || Z_STRVAL_P(op2)[0] > '9') {
					if (Z_STRLEN_P(op1) != Z_STRLEN_P(op2)) {
						result = 0;
					} else {
						result = (memcmp(Z_STRVAL_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op1)) == 0);
					}
				} else {
					result = (zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2)) == 0);
				}
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
	} else if ((OP1_TYPE & IS_VAR) && UNEXPECTED(Z_ISREF_P(op1))) {
		/* Don't keep lock on reference, lock the value instead */
		if (UNEXPECTED(Z_REFCOUNT_P(op1) == 1)) {
			ZVAL_UNREF(op1);
		} else {
			Z_DELREF_P(op1);
			ZVAL_COPY(op1, Z_REFVAL_P(op1));
		}
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

ZEND_VM_HANDLER(68, ZEND_NEW, UNUSED|CLASS_FETCH|CONST|VAR, ANY, NUM)
{
	USE_OPLINE
	zval *result;
	zend_function *constructor;
	zend_class_entry *ce;
	zend_execute_data *call;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CONST) {
		ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)));
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op1)), EX_CONSTANT(opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}
			CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce);
		}
	} else if (OP1_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op1.num);
		if (UNEXPECTED(ce == NULL)) {
			ZEND_ASSERT(EG(exception));
			HANDLE_EXCEPTION();
		}
	} else {
		ce = Z_CE_P(EX_VAR(opline->op1.var));
	}

	result = EX_VAR(opline->result.var);
	if (UNEXPECTED(object_init_ex(result, ce) != SUCCESS)) {
		HANDLE_EXCEPTION();
	}

	constructor = Z_OBJ_HT_P(result)->get_constructor(Z_OBJ_P(result));
	if (constructor == NULL) {
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
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(110, ZEND_CLONE, CONST|TMPVAR|UNUSED|THIS|CV, ANY)
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
		zend_throw_error(NULL, "Using $this when not in object context");
		HANDLE_EXCEPTION();
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
		HANDLE_EXCEPTION();
	}

	if (clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			scope = EX(func)->op_array.scope;
			if (UNEXPECTED(ce != scope)) {
				zend_throw_error(NULL, "Call to private %s::__clone() from context '%s'", ZSTR_VAL(ce->name), scope ? ZSTR_VAL(scope->name) : "");
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			scope = EX(func)->op_array.scope;
			if (UNEXPECTED(!zend_check_protected(zend_get_function_root_class(clone), scope))) {
				zend_throw_error(NULL, "Call to protected %s::__clone() from context '%s'", ZSTR_VAL(ce->name), scope ? ZSTR_VAL(scope->name) : "");
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		}
	}

	ZVAL_OBJ(EX_VAR(opline->result.var), clone_call(obj));
	if (UNEXPECTED(EG(exception) != NULL)) {
		OBJ_RELEASE(Z_OBJ_P(EX_VAR(opline->result.var)));
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(99, ZEND_FETCH_CONSTANT, UNUSED, CONST, CONST_FETCH)
{
	USE_OPLINE
	zend_constant *c;

	SAVE_OPLINE();

	if (EXPECTED(CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2))))) {
		c = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
	} else if ((c = zend_quick_get_constant(EX_CONSTANT(opline->op2) + 1, opline->extended_value)) == NULL) {
		if ((opline->extended_value & IS_CONSTANT_UNQUALIFIED) != 0) {
			char *actual = (char *)zend_memrchr(Z_STRVAL_P(EX_CONSTANT(opline->op2)), '\\', Z_STRLEN_P(EX_CONSTANT(opline->op2)));
			if (!actual) {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), Z_STR_P(EX_CONSTANT(opline->op2)));
			} else {
				actual++;
				ZVAL_STRINGL(EX_VAR(opline->result.var),
						actual, Z_STRLEN_P(EX_CONSTANT(opline->op2)) - (actual - Z_STRVAL_P(EX_CONSTANT(opline->op2))));
			}
			/* non-qualified constant - allow text substitution */
			zend_error(E_NOTICE, "Use of undefined constant %s - assumed '%s'",
					Z_STRVAL_P(EX_VAR(opline->result.var)), Z_STRVAL_P(EX_VAR(opline->result.var)));
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			zend_throw_error(NULL, "Undefined constant '%s'", Z_STRVAL_P(EX_CONSTANT(opline->op2)));
			HANDLE_EXCEPTION();
		}
	} else {
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), c);
	}

#ifdef ZTS
	if (c->flags & CONST_PERSISTENT) {
		ZVAL_DUP(EX_VAR(opline->result.var), &c->value);
	} else {
		ZVAL_COPY(EX_VAR(opline->result.var), &c->value);
	}
#else
	ZVAL_COPY(EX_VAR(opline->result.var), &c->value);
#endif

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(181, ZEND_FETCH_CLASS_CONSTANT, VAR|CONST|UNUSED|CLASS_FETCH, CONST)
{
	zend_class_entry *ce, *scope;
	zend_class_constant *c;
	zval *value;
	USE_OPLINE

	SAVE_OPLINE();

	do {
		if (OP1_TYPE == IS_CONST) {
			if (EXPECTED(CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2))))) {
				value = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
#ifdef ZTS
				ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)));
#endif
				break;
			} else if (EXPECTED(CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1))))) {
				ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)));
			} else {
				ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op1)), EX_CONSTANT(opline->op1) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
				if (UNEXPECTED(ce == NULL)) {
					if (EXPECTED(!EG(exception))) {
						zend_throw_error(NULL, "Class '%s' not found", Z_STRVAL_P(EX_CONSTANT(opline->op1)));
					}
					HANDLE_EXCEPTION();
				}
				CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce);
			}
		} else {
			if (OP1_TYPE == IS_UNUSED) {
				ce = zend_fetch_class(NULL, opline->op1.num);
				if (UNEXPECTED(ce == NULL)) {
					ZEND_ASSERT(EG(exception));
					HANDLE_EXCEPTION();
				}
			} else {
				ce = Z_CE_P(EX_VAR(opline->op1.var));
			}
			if ((value = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce)) != NULL) {
				break;
			}
		}

		if (EXPECTED((c = zend_hash_find_ptr(&ce->constants_table, Z_STR_P(EX_CONSTANT(opline->op2)))) != NULL)) {
			scope = EX(func)->op_array.scope;
			if (!zend_verify_const_access(c, scope)) {
				zend_throw_error(NULL, "Cannot access %s const %s::%s", zend_visibility_string(Z_ACCESS_FLAGS(c->value)), ZSTR_VAL(ce->name), Z_STRVAL_P(EX_CONSTANT(opline->op2)));
				HANDLE_EXCEPTION();
			}
			value = &c->value;
			if (Z_CONSTANT_P(value)) {
				zval_update_constant_ex(value, ce);
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
			}
			if (OP1_TYPE == IS_CONST) {
				CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), value);
			} else {
				CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce, value);
			}
		} else {
			zend_throw_error(NULL, "Undefined class constant '%s'", Z_STRVAL_P(EX_CONSTANT(opline->op2)));
			HANDLE_EXCEPTION();
		}
	} while (0);

#ifdef ZTS
	if (ce->type == ZEND_INTERNAL_CLASS) {
		ZVAL_DUP(EX_VAR(opline->result.var), value);
	} else {
		ZVAL_COPY(EX_VAR(opline->result.var), value);
	}
#else
	ZVAL_COPY(EX_VAR(opline->result.var), value);
#endif

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
		ZVAL_MAKE_REF(expr_ptr);
		Z_ADDREF_P(expr_ptr);
		FREE_OP1_VAR_PTR();
	} else {
		expr_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (OP1_TYPE == IS_TMP_VAR) {
			/* pass */
		} else if (OP1_TYPE == IS_CONST) {
			if (Z_REFCOUNTED_P(expr_ptr)) {
				Z_ADDREF_P(expr_ptr);
			}
		} else if (OP1_TYPE == IS_CV) {
			ZVAL_DEREF(expr_ptr);
			if (Z_REFCOUNTED_P(expr_ptr)) {
				Z_ADDREF_P(expr_ptr);
			}
		} else /* if (OP1_TYPE == IS_VAR) */ {
			if (UNEXPECTED(Z_ISREF_P(expr_ptr))) {
				zend_refcounted *ref = Z_COUNTED_P(expr_ptr);

				expr_ptr = Z_REFVAL_P(expr_ptr);
				if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
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
			zend_error(E_WARNING, "Illegal offset type");
			zval_ptr_dtor(expr_ptr);
		}
		FREE_OP2();
	} else {
		zend_hash_next_index_insert(Z_ARRVAL_P(EX_VAR(opline->result.var)), expr_ptr);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|NEXT|CV, ARRAY_INIT|REF)
{
	zval *array;
	uint32_t size;
	USE_OPLINE

	array = EX_VAR(opline->result.var);
	if (OP1_TYPE != IS_UNUSED) {
		size = opline->extended_value >> ZEND_ARRAY_SIZE_SHIFT;
	} else {
		size = 0;
	}
	ZVAL_NEW_ARR(array);
	zend_hash_init(Z_ARRVAL_P(array), size, NULL, ZVAL_PTR_DTOR, 0);

	if (OP1_TYPE != IS_UNUSED) {
		/* Explicitly initialize array as not-packed if flag is set */
		if (opline->extended_value & ZEND_ARRAY_NOT_PACKED) {
			zend_hash_real_init(Z_ARRVAL_P(array), 0);
		}
	}

	if (OP1_TYPE == IS_UNUSED) {
		ZEND_VM_NEXT_OPCODE();
#if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	} else {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ADD_ARRAY_ELEMENT);
#endif
	}
}

ZEND_VM_HANDLER(21, ZEND_CAST, CONST|TMP|VAR|CV, ANY, TYPE)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr;
	zval *result = EX_VAR(opline->result.var);

	SAVE_OPLINE();
	expr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	switch (opline->extended_value) {
		case IS_NULL:
			/* This code is taken from convert_to_null. However, it does not seems very useful,
			 * because a conversion to null always results in the same value. This could only
			 * be relevant if a cast_object handler for IS_NULL has some kind of side-effect. */
#if 0
			if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) {
				ZVAL_DEREF(expr);
			}
			if (Z_TYPE_P(expr) == IS_OBJECT && Z_OBJ_HT_P(expr)->cast_object) {
				if (Z_OBJ_HT_P(expr)->cast_object(expr, result, IS_NULL) == SUCCESS) {
					break;
				}
			}
#endif

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
				if (Z_TYPE_P(expr) != IS_OBJECT) {
					ZVAL_NEW_ARR(result);
					zend_hash_init(Z_ARRVAL_P(result), 8, NULL, ZVAL_PTR_DTOR, 0);
					if (Z_TYPE_P(expr) != IS_NULL) {
						expr = zend_hash_index_add_new(Z_ARRVAL_P(result), 0, expr);
						if (OP1_TYPE == IS_CONST) {
							if (UNEXPECTED(Z_OPT_REFCOUNTED_P(expr))) Z_ADDREF_P(expr);
						} else {
							if (Z_OPT_REFCOUNTED_P(expr)) Z_ADDREF_P(expr);
						}
					}
				} else {
					ZVAL_COPY_VALUE(result, expr);
					Z_ADDREF_P(result);
					convert_to_array(result);
				}
			} else {
				if (Z_TYPE_P(expr) != IS_ARRAY) {
					object_init(result);
					if (Z_TYPE_P(expr) != IS_NULL) {
						expr = zend_hash_add_new(Z_OBJPROP_P(result), CG(known_strings)[ZEND_STR_SCALAR], expr);
						if (OP1_TYPE == IS_CONST) {
							if (UNEXPECTED(Z_OPT_REFCOUNTED_P(expr))) Z_ADDREF_P(expr);
						} else {
							if (Z_OPT_REFCOUNTED_P(expr)) Z_ADDREF_P(expr);
						}
					}
				} else {
					ZVAL_COPY(result, expr);
					convert_to_object(result);
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
			zend_throw_exception_internal(NULL);
			HANDLE_EXCEPTION();
		}
	} else if (RETURN_VALUE_USED(opline)) {
		ZVAL_FALSE(EX_VAR(opline->result.var));
	}
	ZEND_VM_SET_OPCODE(opline + 1);
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMPVAR|CV, UNUSED, VAR_FETCH|ISSET)
{
	USE_OPLINE
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV &&
	    (opline->extended_value & ZEND_QUICK_SET)) {
		zval *var = EX_VAR(opline->op1.var);

		if (Z_REFCOUNTED_P(var)) {
			zend_refcounted *garbage = Z_COUNTED_P(var);

			if (!--GC_REFCOUNT(garbage)) {
				ZVAL_UNDEF(var);
				zval_dtor_func(garbage);
			} else {
				zval *z = var;
				ZVAL_DEREF(z);
				if (Z_COLLECTABLE_P(z) && UNEXPECTED(!Z_GC_INFO_P(z))) {
					ZVAL_UNDEF(var);
					gc_possible_root(Z_COUNTED_P(z));
				} else {
					ZVAL_UNDEF(var);
				}
			}
		} else {
			ZVAL_UNDEF(var);
		}
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	ZVAL_UNDEF(&tmp);
	if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		ZVAL_STR(&tmp, zval_get_string(varname));
		varname = &tmp;
	}

	target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
	zend_hash_del_ind(target_symbol_table, Z_STR_P(varname));

	if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
		zend_string_release(Z_STR(tmp));
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(179, ZEND_UNSET_STATIC_PROP, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
{
	USE_OPLINE
	zval tmp, *varname;
	zend_class_entry *ce;
	zend_free_op free_op1;

	SAVE_OPLINE();

	varname = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	ZVAL_UNDEF(&tmp);
	if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(varname) == IS_UNDEF)) {
			varname = GET_OP1_UNDEF_CV(varname, BP_VAR_R);
		}
		ZVAL_STR(&tmp, zval_get_string(varname));
		varname = &tmp;
	}

	if (OP2_TYPE == IS_CONST) {
		ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
		if (UNEXPECTED(ce == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_throw_error(NULL, "Class '%s' not found", Z_STRVAL_P(EX_CONSTANT(opline->op2)));
				}
				if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
					zend_string_release(Z_STR(tmp));
				}
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
			CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce);
		}
	} else if (OP2_TYPE == IS_UNUSED) {
		ce = zend_fetch_class(NULL, opline->op2.num);
		if (UNEXPECTED(ce == NULL)) {
			ZEND_ASSERT(EG(exception));
			if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
				zend_string_release(Z_STR(tmp));
			}
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
	} else {
		ce = Z_CE_P(EX_VAR(opline->op2.var));
	}
	zend_std_unset_static_property(ce, Z_STR_P(varname));

	if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
		zend_string_release(Z_STR(tmp));
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
				zend_throw_error(NULL, "Cannot use object as array");
			} else {
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

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|THIS|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
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
			Z_OBJ_HT_P(container)->unset_property(container, offset, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(offset)) : NULL));
		} else {
			zend_error(E_NOTICE, "Trying to unset property of non-object");
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
	HashTable *fe_ht;

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
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else if (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(array_ptr) == IS_OBJECT)) {
		if (!Z_OBJCE_P(array_ptr)->get_iterator) {
			HashPosition pos = 0;
			Bucket *p;

			result = EX_VAR(opline->result.var);
			ZVAL_COPY_VALUE(result, array_ptr);
			if (OP1_TYPE != IS_TMP_VAR) {
				Z_ADDREF_P(array_ptr);
			}
			if (Z_OBJ_P(array_ptr)->properties
			 && UNEXPECTED(GC_REFCOUNT(Z_OBJ_P(array_ptr)->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(Z_OBJ_P(array_ptr)->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_REFCOUNT(Z_OBJ_P(array_ptr)->properties)--;
				}
				Z_OBJ_P(array_ptr)->properties = zend_array_dup(Z_OBJ_P(array_ptr)->properties);
			}
			fe_ht = Z_OBJPROP_P(array_ptr);
			pos = 0;
			p = fe_ht->arData;
			while (1) {
				if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
					FREE_OP1_IF_VAR();
					Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
					ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
				}
				if ((EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
				     (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
				      EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) &&
				    (UNEXPECTED(!p->key) ||
				     EXPECTED(zend_check_property_access(Z_OBJ_P(array_ptr), p->key) == SUCCESS))) {
					break;
				}
				pos++;
				p++;
			}
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(fe_ht, pos);

			FREE_OP1_IF_VAR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			zend_class_entry *ce = Z_OBJCE_P(array_ptr);
			zend_object_iterator *iter = ce->get_iterator(ce, array_ptr, 0);
			zend_bool is_empty;

			if (UNEXPECTED(!iter) || UNEXPECTED(EG(exception))) {
				FREE_OP1();
				if (!EG(exception)) {
					zend_throw_exception_ex(NULL, 0, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name));
				}
				zend_throw_exception_internal(NULL);
				HANDLE_EXCEPTION();
			}

			iter->index = 0;
			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					OBJ_RELEASE(&iter->std);
					FREE_OP1();
					HANDLE_EXCEPTION();
				}
			}

			is_empty = iter->funcs->valid(iter) != SUCCESS;

			if (UNEXPECTED(EG(exception) != NULL)) {
				OBJ_RELEASE(&iter->std);
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
			iter->index = -1; /* will be set to 0 before using next handler */

			ZVAL_OBJ(EX_VAR(opline->result.var), &iter->std);
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;

			FREE_OP1();
			if (is_empty) {
				ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
			} else {
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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

ZEND_VM_HANDLER(125, ZEND_FE_RESET_RW, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *array_ptr, *array_ref;
	HashTable *fe_ht;
	HashPosition pos = 0;
	Bucket *p;

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
			zval_copy_ctor_func(array_ptr);
		} else {
			SEPARATE_ARRAY(array_ptr);
		}
		fe_ht = Z_ARRVAL_P(array_ptr);
		p = fe_ht->arData;
		while (1) {
			if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
				FREE_OP1_VAR_PTR();
				Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
				ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
			}
			if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
			    (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
			     EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) {
				break;
			}
			pos++;
			p++;
		}
		Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(fe_ht, pos);

		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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
					GC_REFCOUNT(Z_OBJ_P(array_ptr)->properties)--;
				}
				Z_OBJ_P(array_ptr)->properties = zend_array_dup(Z_OBJ_P(array_ptr)->properties);
			}
			fe_ht = Z_OBJPROP_P(array_ptr);
			p = fe_ht->arData;
			while (1) {
				if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
					FREE_OP1_VAR_PTR();
					Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
					ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
				}
				if ((EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
				     (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
				      EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) &&
				    (UNEXPECTED(!p->key) ||
				     EXPECTED(zend_check_property_access(Z_OBJ_P(array_ptr), p->key) == SUCCESS))) {
					break;
				}
				pos++;
				p++;
			}
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = zend_hash_iterator_add(fe_ht, pos);

			FREE_OP1_VAR_PTR();
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		} else {
			zend_class_entry *ce = Z_OBJCE_P(array_ptr);
			zend_object_iterator *iter = ce->get_iterator(ce, array_ptr, 1);
			zend_bool is_empty;

			if (UNEXPECTED(!iter) || UNEXPECTED(EG(exception))) {
				if (OP1_TYPE == IS_VAR) {
					FREE_OP1_VAR_PTR();
				} else {
					FREE_OP1();
				}
				if (!EG(exception)) {
					zend_throw_exception_ex(NULL, 0, "Object of type %s did not create an Iterator", ZSTR_VAL(ce->name));
				}
				zend_throw_exception_internal(NULL);
				HANDLE_EXCEPTION();
			}

			iter->index = 0;
			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					OBJ_RELEASE(&iter->std);
					if (OP1_TYPE == IS_VAR) {
						FREE_OP1_VAR_PTR();
					} else {
						FREE_OP1();
					}
					HANDLE_EXCEPTION();
				}
			}

			is_empty = iter->funcs->valid(iter) != SUCCESS;

			if (UNEXPECTED(EG(exception) != NULL)) {
				OBJ_RELEASE(&iter->std);
				if (OP1_TYPE == IS_VAR) {
					FREE_OP1_VAR_PTR();
				} else {
					FREE_OP1();
				}
				HANDLE_EXCEPTION();
			}
			iter->index = -1; /* will be set to 0 before using next handler */

			ZVAL_OBJ(EX_VAR(opline->result.var), &iter->std);
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;

			if (OP1_TYPE == IS_VAR) {
				FREE_OP1_VAR_PTR();
			} else {
				FREE_OP1();
			}
			if (is_empty) {
				ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
			} else {
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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
				ZEND_VM_C_GOTO(fe_fetch_r_exit);
			}
			value = &p->val;
			value_type = Z_TYPE_INFO_P(value);
			if (value_type == IS_UNDEF) {
				pos++;
				p++;
				continue;
			} else if (UNEXPECTED(value_type == IS_INDIRECT)) {
				value = Z_INDIRECT_P(value);
				value_type = Z_TYPE_INFO_P(value);
				if (UNEXPECTED(value_type == IS_UNDEF)) {
					pos++;
					p++;
					continue;
				}
			}
			break;
		}
		Z_FE_POS_P(array) = pos + 1;
		if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
	} else if (EXPECTED(Z_TYPE_P(array) == IS_OBJECT)) {
		zend_object_iterator *iter;

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
				if (UNEXPECTED(value_type == IS_UNDEF)) {
					pos++;
					p++;
					continue;
				} else if (UNEXPECTED(value_type == IS_INDIRECT)) {
					value = Z_INDIRECT_P(value);
					value_type = Z_TYPE_INFO_P(value);
					if (UNEXPECTED(value_type == IS_UNDEF)) {
						pos++;
						p++;
						continue;
					}
				}
				if (UNEXPECTED(!p->key) ||
				    EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS)) {
					break;
				}
				pos++;
				p++;
			}
			if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
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
			while (1) {
				pos++;
				if (pos >= fe_ht->nNumUsed) {
					pos = HT_INVALID_IDX;
					break;
				}
				p++;
				if ((EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
				     (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
				      EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) &&
				    (UNEXPECTED(!p->key) ||
				     EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS))) {
					break;
				}
			}
			EG(ht_iterators)[Z_FE_ITER_P(array)].pos = pos;
		} else {
			if (EXPECTED(++iter->index > 0)) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_r_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_r_exit);
			}
			if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
					if (UNEXPECTED(EG(exception) != NULL)) {
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
			HANDLE_EXCEPTION();
		}
ZEND_VM_C_LABEL(fe_fetch_r_exit):
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}

	if (EXPECTED(OP2_TYPE == IS_CV)) {
		zval *variable_ptr = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->op2.var);
		zend_assign_to_variable(variable_ptr, value, IS_CV);
	} else {
		zval *res = EX_VAR(opline->op2.var);
		zend_refcounted *gc = Z_COUNTED_P(value);

		ZVAL_COPY_VALUE_EX(res, value, gc, value_type);
		if (EXPECTED((value_type & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) != 0)) {
			GC_REFCOUNT(gc)++;
		}
	}
	ZEND_VM_NEXT_OPCODE();
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
			if (UNEXPECTED(value_type == IS_UNDEF)) {
				pos++;
				p++;
				continue;
			} else if (UNEXPECTED(value_type == IS_INDIRECT)) {
				value = Z_INDIRECT_P(value);
				value_type = Z_TYPE_INFO_P(value);
				if (UNEXPECTED(value_type == IS_UNDEF)) {
					pos++;
					p++;
					continue;
				}
			}
			break;
		}
		if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
			if (!p->key) {
				ZVAL_LONG(EX_VAR(opline->result.var), p->h);
			} else {
				ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
			}
		}
		while (1) {
			pos++;
			if (pos >= fe_ht->nNumUsed) {
				pos = HT_INVALID_IDX;
				break;
			}
			p++;
			if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
			    (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
			     EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) {
				break;
			}
		}
		EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos;
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
				if (UNEXPECTED(value_type == IS_UNDEF)) {
					pos++;
					p++;
					continue;
				} else if (UNEXPECTED(value_type == IS_INDIRECT)) {
					value = Z_INDIRECT_P(value);
					value_type = Z_TYPE_INFO_P(value);
					if (UNEXPECTED(value_type == IS_UNDEF)) {
						pos++;
						p++;
						continue;
					}
				}
				if (UNEXPECTED(!p->key) ||
				    EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS)) {
					break;
				}
				pos++;
				p++;
			}
			if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
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
			while (1) {
				pos++;
				if (pos >= fe_ht->nNumUsed) {
					pos = HT_INVALID_IDX;
					break;
				}
				p++;
				if ((EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
				     (EXPECTED(Z_TYPE(p->val) != IS_INDIRECT) ||
				      EXPECTED(Z_TYPE_P(Z_INDIRECT(p->val)) != IS_UNDEF))) &&
				    (UNEXPECTED(!p->key) ||
				     EXPECTED(zend_check_property_access(Z_OBJ_P(array), p->key) == SUCCESS))) {
					break;
				}
			}
			EG(ht_iterators)[Z_FE_ITER_P(EX_VAR(opline->op1.var))].pos = pos;
		} else {
			if (++iter->index > 0) {
				/* This could cause an endless loop if index becomes zero again.
				 * In case that ever happens we need an additional flag. */
				iter->funcs->move_forward(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_w_exit);
			}
			if (opline->result_type & (IS_TMP_VAR|IS_CV)) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
					if (UNEXPECTED(EG(exception) != NULL)) {
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
		zval *variable_ptr = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->op2.var);
		if (EXPECTED(variable_ptr != value)) {
			zend_reference *ref;

			ref = Z_REF_P(value);
			GC_REFCOUNT(ref)++;
			zval_ptr_dtor(variable_ptr);
			ZVAL_REF(variable_ptr, ref);
		}
	} else {
		Z_ADDREF_P(value);
		ZVAL_REF(EX_VAR(opline->op2.var), Z_REF_P(value));
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMPVAR|CV, UNUSED, VAR_FETCH|ISSET)
{
	USE_OPLINE
	zval *value;
	int result;

	if (OP1_TYPE == IS_CV &&
	    (opline->extended_value & ZEND_QUICK_SET)) {
		value = EX_VAR(opline->op1.var);
		if (opline->extended_value & ZEND_ISSET) {
			result =
				Z_TYPE_P(value) > IS_NULL &&
			    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
		} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
			SAVE_OPLINE();
			result = !i_zend_is_true(value);
			if (UNEXPECTED(EG(exception))) {
				HANDLE_EXCEPTION();
			}
		}
		ZEND_VM_SMART_BRANCH(result, 0);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_SET_NEXT_OPCODE(opline + 1);
		ZEND_VM_CONTINUE();
	} else {
		zend_free_op free_op1;
		zval tmp, *varname;
		HashTable *target_symbol_table;

		SAVE_OPLINE();
		varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
		ZVAL_UNDEF(&tmp);
		if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
			ZVAL_STR(&tmp, zval_get_string(varname));
			varname = &tmp;
		}

		target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
		value = zend_hash_find_ind(target_symbol_table, Z_STR_P(varname));

		if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
			zend_string_release(Z_STR(tmp));
		}
		FREE_OP1();

		if (opline->extended_value & ZEND_ISSET) {
			result = value && Z_TYPE_P(value) > IS_NULL &&
			    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
		} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
			result = !value || !i_zend_is_true(value);
		}

		ZEND_VM_SMART_BRANCH(result, 1);
		ZVAL_BOOL(EX_VAR(opline->result.var), result);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
}

ZEND_VM_HANDLER(180, ZEND_ISSET_ISEMPTY_STATIC_PROP, CONST|TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR, ISSET)
{
	USE_OPLINE
	zval *value;
	int result;
	zend_free_op free_op1;
	zval tmp, *varname;
	zend_class_entry *ce;

	SAVE_OPLINE();
	varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	ZVAL_UNDEF(&tmp);
	if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
		ZVAL_STR(&tmp, zval_get_string(varname));
		varname = &tmp;
	}

	if (OP2_TYPE == IS_CONST) {
		if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)))) != NULL)) {
			value = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)) + sizeof(void*));

			/* check if static properties were destoyed */
			if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
				value = NULL;
			}

			ZEND_VM_C_GOTO(is_static_prop_return);
		} else if (UNEXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)))) == NULL)) {
			ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}
			CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce);
		}
	} else {
		if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
					zend_string_release(Z_STR(tmp));
				}
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		if (OP1_TYPE == IS_CONST &&
		    (value = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce)) != NULL) {

			/* check if static properties were destoyed */
			if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
				value = NULL;
			}

			ZEND_VM_C_GOTO(is_static_prop_return);
		}
	}

	value = zend_std_get_static_property(ce, Z_STR_P(varname), 1);

	if (OP1_TYPE == IS_CONST && value) {
		CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce, value);
	}		

	if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
		zend_string_release(Z_STR(tmp));
	}
	FREE_OP1();

ZEND_VM_C_LABEL(is_static_prop_return):
	if (opline->extended_value & ZEND_ISSET) {
		result = value && Z_TYPE_P(value) > IS_NULL &&
		    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
	} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
		result = !value || !i_zend_is_true(value);
	}

	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(115, ZEND_ISSET_ISEMPTY_DIM_OBJ, CONST|TMPVAR|CV, CONST|TMPVAR|CV, ISSET)
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
ZEND_VM_C_LABEL(str_index_prop):
			value = zend_hash_find_ind(ht, str);
		} else if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			hval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(num_index_prop):
			value = zend_hash_index_find(ht, hval);
		} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && EXPECTED(Z_ISREF_P(offset))) {
			offset = Z_REFVAL_P(offset);
			ZEND_VM_C_GOTO(isset_again);
		} else if (Z_TYPE_P(offset) == IS_DOUBLE) {
			hval = zend_dval_to_lval(Z_DVAL_P(offset));
			ZEND_VM_C_GOTO(num_index_prop);
		} else if (Z_TYPE_P(offset) == IS_NULL) {
			str = ZSTR_EMPTY_ALLOC();
			ZEND_VM_C_GOTO(str_index_prop);
		} else if (Z_TYPE_P(offset) == IS_FALSE) {
			hval = 0;
			ZEND_VM_C_GOTO(num_index_prop);
		} else if (Z_TYPE_P(offset) == IS_TRUE) {
			hval = 1;
			ZEND_VM_C_GOTO(num_index_prop);
		} else if (Z_TYPE_P(offset) == IS_RESOURCE) {
			hval = Z_RES_HANDLE_P(offset);
			ZEND_VM_C_GOTO(num_index_prop);
		} else if (OP2_TYPE == IS_CV && Z_TYPE_P(offset) == IS_UNDEF) {
			GET_OP2_UNDEF_CV(offset, BP_VAR_R);
			str = ZSTR_EMPTY_ALLOC();
			ZEND_VM_C_GOTO(str_index_prop);
		} else {
			zend_error(E_WARNING, "Illegal offset type in isset or empty");
			ZEND_VM_C_GOTO(isset_not_found);
		}

		if (opline->extended_value & ZEND_ISSET) {
			/* > IS_NULL means not IS_UNDEF and not IS_NULL */
			result = value != NULL && Z_TYPE_P(value) > IS_NULL &&
			    (!Z_ISREF_P(value) || Z_TYPE_P(Z_REFVAL_P(value)) != IS_NULL);
		} else /* if (opline->extended_value & ZEND_ISEMPTY) */ {
			result = (value == NULL || !i_zend_is_true(value));
		}
		ZEND_VM_C_GOTO(isset_dim_obj_exit);
	} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_ISREF_P(container)) {
		container = Z_REFVAL_P(container);
		if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
			ZEND_VM_C_GOTO(isset_dim_obj_array);
		}
	}

	if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		offset = GET_OP2_UNDEF_CV(offset, BP_VAR_R);
	}

	if ((OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(container) == IS_OBJECT))) {
		if (EXPECTED(Z_OBJ_HT_P(container)->has_dimension)) {
			result =
				((opline->extended_value & ZEND_ISSET) == 0) ^
				Z_OBJ_HT_P(container)->has_dimension(container, offset, (opline->extended_value & ZEND_ISSET) == 0);
		} else {
			zend_error(E_NOTICE, "Trying to check element of non-array");
			ZEND_VM_C_GOTO(isset_not_found);
		}
	} else if (EXPECTED(Z_TYPE_P(container) == IS_STRING)) { /* string offsets */
		zend_long lval;

		if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			lval = Z_LVAL_P(offset);
ZEND_VM_C_LABEL(isset_str_offset):
			if (UNEXPECTED(lval < 0)) { /* Handle negative offset */
				lval += (zend_long)Z_STRLEN_P(container);
			}
			if (EXPECTED(lval >= 0) && (size_t)lval < Z_STRLEN_P(container)) {
				if (opline->extended_value & ZEND_ISSET) {
					result = 1;
				} else {
					result = (Z_STRVAL_P(container)[lval] == '0');
				}
			} else {
				ZEND_VM_C_GOTO(isset_not_found);
			}
		} else {
			if (OP2_TYPE & (IS_CV|IS_VAR)) {
				ZVAL_DEREF(offset);
			}
			if (Z_TYPE_P(offset) < IS_STRING /* simple scalar types */
					|| (Z_TYPE_P(offset) == IS_STRING /* or numeric string */
						&& IS_LONG == is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), NULL, NULL, 0))) {
				lval = zval_get_long(offset);
				ZEND_VM_C_GOTO(isset_str_offset);
			}
			ZEND_VM_C_GOTO(isset_not_found);
		}
	} else {
ZEND_VM_C_LABEL(isset_not_found):
		result = ((opline->extended_value & ZEND_ISSET) == 0);
	}

ZEND_VM_C_LABEL(isset_dim_obj_exit):
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(148, ZEND_ISSET_ISEMPTY_PROP_OBJ, CONST|TMPVAR|UNUSED|THIS|CV, CONST|TMPVAR|CV, ISSET)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	int result;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
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
		zend_error(E_NOTICE, "Trying to check property of non-object");
ZEND_VM_C_LABEL(isset_no_object):
		result = ((opline->extended_value & ZEND_ISSET) == 0);
	} else {
		result =
			((opline->extended_value & ZEND_ISSET) == 0) ^
			Z_OBJ_HT_P(container)->has_property(container, offset, (opline->extended_value & ZEND_ISSET) == 0, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(offset)) : NULL));
	}

	FREE_OP2();
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(79, ZEND_EXIT, CONST|TMPVAR|UNUSED|CV, ANY)
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
				zend_print_variable(ptr);
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
				zend_ini_entry *p = zend_hash_find_ptr(EG(ini_directives), CG(known_strings)[ZEND_STR_ERROR_REPORTING]);
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
				if (EXPECTED(zend_hash_add_ptr(EG(modified_ini_directives), CG(known_strings)[ZEND_STR_ERROR_REPORTING], EG(error_reporting_ini_entry)) != NULL)) {
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

ZEND_VM_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;
	zval *ref = NULL;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && Z_ISREF_P(value)) {
		if (OP1_TYPE == IS_VAR) {
			ref = value;
		}
		value = Z_REFVAL_P(value);
	}
	if (i_zend_is_true(value)) {
		zval *result = EX_VAR(opline->result.var);

		ZVAL_COPY_VALUE(result, value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_REFCOUNTED_P(result))) Z_ADDREF_P(result);
		} else if (OP1_TYPE == IS_CV) {
			if (Z_OPT_REFCOUNTED_P(result)) Z_ADDREF_P(result);
		} else if (OP1_TYPE == IS_VAR && ref) {
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(--GC_REFCOUNT(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(169, ZEND_COALESCE, CONST|TMP|VAR|CV, JMP_ADDR)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;
	zval *ref = NULL;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR(BP_VAR_IS);

	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) && Z_ISREF_P(value)) {
		if (OP1_TYPE == IS_VAR) {
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
		} else if (OP1_TYPE == IS_VAR && ref) {
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(--GC_REFCOUNT(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(result)) {
				Z_ADDREF_P(result);
			}
		}
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(22, ZEND_QM_ASSIGN, CONST|TMP|VAR|CV, ANY)
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
		ZVAL_DEREF(value);
		ZVAL_COPY(result, value);
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

ZEND_VM_HANDLER(101, ZEND_EXT_STMT, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(102, ZEND_EXT_FCALL_BEGIN, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(103, ZEND_EXT_FCALL_END, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, execute_data);
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(139, ZEND_DECLARE_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_class(&EX(func)->op_array, opline, EG(class_table), 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(140, ZEND_DECLARE_INHERITED_CLASS, ANY, VAR)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), Z_CE_P(EX_VAR(opline->op2.var)), 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_INHERITED_CLASS_DELAYED, ANY, VAR)
{
	USE_OPLINE
	zval *zce, *orig_zce;

	SAVE_OPLINE();
	if ((zce = zend_hash_find(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op1)))) == NULL ||
	    ((orig_zce = zend_hash_find(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op1)+1))) != NULL &&
	     Z_CE_P(zce) != Z_CE_P(orig_zce))) {
		do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), Z_CE_P(EX_VAR(opline->op2.var)), 0);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(171, ZEND_DECLARE_ANON_CLASS, ANY, ANY, JMP_ADDR)
{
	zend_class_entry *ce;
	USE_OPLINE

	SAVE_OPLINE();
	ce = zend_hash_find_ptr(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op1)));
	Z_CE_P(EX_VAR(opline->result.var)) = ce;
	ZEND_ASSERT(ce != NULL);

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

ZEND_VM_HANDLER(172, ZEND_DECLARE_ANON_INHERITED_CLASS, ANY, VAR, JMP_ADDR)
{
	zend_class_entry *ce;
	USE_OPLINE

	SAVE_OPLINE();
	ce = zend_hash_find_ptr(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op1)));
	Z_CE_P(EX_VAR(opline->result.var)) = ce;
	ZEND_ASSERT(ce != NULL);

	if (ce->ce_flags & ZEND_ACC_ANON_BOUND) {
		ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		ZEND_VM_CONTINUE();
	}

	zend_do_inheritance(ce, Z_CE_P(EX_VAR(opline->op2.var)));
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

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMPVAR|CV, UNUSED|CLASS_FETCH|CONST|VAR)
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
			ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
			if (UNEXPECTED(ce == NULL)) {
				ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (UNEXPECTED(ce == NULL)) {
					ZVAL_FALSE(EX_VAR(opline->result.var));
					FREE_OP1();
					ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
				}
				CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce);
			}
		} else if (OP2_TYPE == IS_UNUSED) {
			ce = zend_fetch_class(NULL, opline->op2.num);
			if (UNEXPECTED(ce == NULL)) {
				ZEND_ASSERT(EG(exception));
				FREE_OP1();
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

ZEND_VM_HANDLER(104, ZEND_EXT_NOP, ANY, ANY)
{
	USE_OPLINE

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(0, ZEND_NOP, ANY, ANY)
{
	USE_OPLINE

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(144, ZEND_ADD_INTERFACE, ANY, CONST)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *iface;

	SAVE_OPLINE();
	iface = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
	if (UNEXPECTED(iface == NULL)) {
		iface = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_INTERFACE);
		if (UNEXPECTED(iface == NULL)) {
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), iface);
	}

	if (UNEXPECTED((iface->ce_flags & ZEND_ACC_INTERFACE) == 0)) {
		zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
	}
	zend_do_implement_interface(ce, iface);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(154, ZEND_ADD_TRAIT, ANY, ANY)
{
	USE_OPLINE
	zend_class_entry *ce = Z_CE_P(EX_VAR(opline->op1.var));
	zend_class_entry *trait;

	SAVE_OPLINE();
	trait = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
	if (UNEXPECTED(trait == NULL)) {
		trait = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)),
		                                 EX_CONSTANT(opline->op2) + 1,
		                                 ZEND_FETCH_CLASS_TRAIT);
		if (UNEXPECTED(trait == NULL)) {
			ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
		}
		if (!(trait->ce_flags & ZEND_ACC_TRAIT)) {
			zend_error_noreturn(E_ERROR, "%s cannot use %s - it is not a trait", ZSTR_VAL(ce->name), ZSTR_VAL(trait->name));
		}
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), trait);
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
			ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[try_catch->catch_op]);
			ZEND_VM_CONTINUE();

		} else if (op_num < try_catch->finally_op) {
			/* Go to finally block */
			zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[try_catch->finally_end].op1.var);
			cleanup_live_vars(execute_data, op_num, try_catch->finally_op);
			Z_OBJ_P(fast_call) = EG(exception);
			EG(exception) = NULL;
			fast_call->u2.lineno = (uint32_t)-1;
			ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[try_catch->finally_op]);
			ZEND_VM_CONTINUE();

		} else if (op_num < try_catch->finally_end) {
			zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[try_catch->finally_end].op1.var);

			/* cleanup incomplete RETURN statement */
			if (fast_call->u2.lineno != (uint32_t)-1
			 && (EX(func)->op_array.opcodes[fast_call->u2.lineno].op2_type & (IS_TMP_VAR | IS_VAR))) {
				zval *return_value = EX_VAR(EX(func)->op_array.opcodes[fast_call->u2.lineno].op2.var);

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
		zend_generator *generator = zend_get_running_generator(execute_data);
		zend_generator_close(generator, 1);
		ZEND_VM_RETURN();
	} else {
		ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
	}
}

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	uint32_t throw_op_num = EG(opline_before_exception) - EX(func)->op_array.opcodes;
	uint32_t i, current_try_catch_offset = (uint32_t) -1;

	{
		const zend_op *exc_opline = EG(opline_before_exception);
		if ((exc_opline->opcode == ZEND_FREE || exc_opline->opcode == ZEND_FE_FREE)
			&& exc_opline->extended_value & ZEND_FREE_ON_RETURN) {
			/* exceptions thrown because of loop var destruction on return/break/...
			 * are logically thrown at the end of the foreach loop, so adjust the
			 * throw_op_num.
			 */
			throw_op_num = EX(func)->op_array.live_range[exc_opline->op2.num].end;
		}
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
				zend_generator *generator = zend_get_running_generator(execute_data);
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
			FREE_OP1();
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
	}
	c.flags = CONST_CS; /* non persistent, case sensetive */
	c.name = zend_string_dup(Z_STR_P(name), 0);
	c.module_number = PHP_USER_CONSTANT;

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

	SAVE_OPLINE();

	zfunc = zend_hash_find(EG(function_table), Z_STR_P(EX_CONSTANT(opline->op1)));
	ZEND_ASSERT(zfunc != NULL && Z_FUNC_P(zfunc)->type == ZEND_USER_FUNCTION);

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

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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

ZEND_VM_HANDLER(160, ZEND_YIELD, CONST|TMP|VAR|CV|UNUSED, CONST|TMP|VAR|CV|UNUSED, SRC)
{
	USE_OPLINE

	zend_generator *generator = zend_get_running_generator(execute_data);

	SAVE_OPLINE();
	if (UNEXPECTED(generator->flags & ZEND_GENERATOR_FORCED_CLOSE)) {
		zend_throw_error(NULL, "Cannot yield from finally in a force-closed generator");
		FREE_UNFETCHED_OP2();
		FREE_UNFETCHED_OP1();
		HANDLE_EXCEPTION();
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
				} else {
					ZVAL_MAKE_REF(value_ptr);
				}
				ZVAL_COPY(&generator->value, value_ptr);

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

	zend_generator *generator = zend_get_running_generator(execute_data);

	zval *val;
	zend_free_op free_op1;

	SAVE_OPLINE();
	val = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);

	if (UNEXPECTED(generator->flags & ZEND_GENERATOR_FORCED_CLOSE)) {
		zend_throw_error(NULL, "Cannot use \"yield from\" in a force-closed generator");
		FREE_OP1();
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
					HANDLE_EXCEPTION();
				} else {
					zend_generator_yield_from(generator, new_gen);
				}
			} else if (UNEXPECTED(new_gen->execute_data == NULL)) {
				zend_throw_error(NULL, "Generator passed to yield from was aborted without proper return and is unable to continue");
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
				HANDLE_EXCEPTION();
			}

			iter->index = 0;
			if (iter->funcs->rewind) {
				iter->funcs->rewind(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					OBJ_RELEASE(&iter->std);
					HANDLE_EXCEPTION();
				}
			}

			ZVAL_OBJ(&generator->values, &iter->std);
		}
	} else {
		zend_throw_error(NULL, "Can use \"yield from\" only with arrays and Traversables", 0);
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
	if (fast_call->u2.lineno != (uint32_t)-1
	 && (EX(func)->op_array.opcodes[fast_call->u2.lineno].op2_type & (IS_TMP_VAR | IS_VAR))) {
		zval *return_value = EX_VAR(EX(func)->op_array.opcodes[fast_call->u2.lineno].op2.var);

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
	fast_call->u2.lineno = opline - EX(func)->op_array.opcodes;
	ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op1));
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(163, ZEND_FAST_RET, ANY, TRY_CATCH)
{
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->op1.var);
	uint32_t current_try_catch_offset, current_op_num;

	if (fast_call->u2.lineno != (uint32_t)-1) {
		const zend_op *fast_ret = EX(func)->op_array.opcodes + fast_call->u2.lineno;

		ZEND_VM_SET_OPCODE(fast_ret + 1);
		ZEND_VM_CONTINUE();
	}

	/* special case for unhandled exceptions */
	EG(exception) = Z_OBJ_P(fast_call);
	Z_OBJ_P(fast_call) = NULL;
	current_try_catch_offset = opline->op2.num;
	current_op_num = opline - EX(func)->op_array.opcodes;
	ZEND_VM_DISPATCH_TO_HELPER(zend_dispatch_try_catch_finally_helper, try_catch_offset, current_try_catch_offset, op_num, current_op_num);
}

ZEND_VM_HANDLER(168, ZEND_BIND_GLOBAL, CV, CONST)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *varname;
	zval *value;
	zval *variable_ptr;
	uint32_t idx;
	zend_reference *ref;

	ZEND_VM_REPEATABLE_OPCODE

	varname = GET_OP2_ZVAL_PTR(BP_VAR_R);

	/* We store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
	idx = (uint32_t)(uintptr_t)CACHED_PTR(Z_CACHE_SLOT_P(varname)) - 1;
	if (EXPECTED(idx < EG(symbol_table).nNumUsed)) {
		Bucket *p = EG(symbol_table).arData + idx;

		if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF) &&
	        (EXPECTED(p->key == Z_STR_P(varname)) ||
	         (EXPECTED(p->h == ZSTR_H(Z_STR_P(varname))) &&
	          EXPECTED(p->key != NULL) &&
	          EXPECTED(ZSTR_LEN(p->key) == Z_STRLEN_P(varname)) &&
	          EXPECTED(memcmp(ZSTR_VAL(p->key), Z_STRVAL_P(varname), Z_STRLEN_P(varname)) == 0)))) {

			value = &EG(symbol_table).arData[idx].val;
			ZEND_VM_C_GOTO(check_indirect);
		}
	}

	value = zend_hash_find(&EG(symbol_table), Z_STR_P(varname));
	if (UNEXPECTED(value == NULL)) {
		value = zend_hash_add_new(&EG(symbol_table), Z_STR_P(varname), &EG(uninitialized_zval));
		idx = ((char*)value - (char*)EG(symbol_table).arData) / sizeof(Bucket);
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(Z_CACHE_SLOT_P(varname), (void*)(uintptr_t)(idx + 1));
	} else {
		idx = ((char*)value - (char*)EG(symbol_table).arData) / sizeof(Bucket);
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(Z_CACHE_SLOT_P(varname), (void*)(uintptr_t)(idx + 1));
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
		ref = (zend_reference*)emalloc(sizeof(zend_reference));
		GC_REFCOUNT(ref) = 2;
		GC_TYPE_INFO(ref) = IS_REFERENCE;
		ZVAL_COPY_VALUE(&ref->val, value);
		Z_REF_P(value) = ref;
		Z_TYPE_INFO_P(value) = IS_REFERENCE_EX;
	} else {
		ref = Z_REF_P(value);
		GC_REFCOUNT(ref)++;
	}

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr))) {
		uint32_t refcnt = Z_DELREF_P(variable_ptr);

		if (EXPECTED(variable_ptr != value)) {
			if (refcnt == 0) {
				SAVE_OPLINE();
				zval_dtor_func(Z_COUNTED_P(variable_ptr));
				if (UNEXPECTED(EG(exception))) {
					ZVAL_NULL(variable_ptr);
					HANDLE_EXCEPTION();
				}
			} else {
				GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
			}
		}
	}
	ZVAL_REF(variable_ptr, ref);

	ZEND_VM_REPEAT_OPCODE(ZEND_BIND_GLOBAL);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(121, ZEND_STRLEN, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zval *value;
	zend_free_op free_op1;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
ZEND_VM_C_LABEL(try_strlen):
	if (EXPECTED(Z_TYPE_P(value) == IS_STRING)) {
		ZVAL_LONG(EX_VAR(opline->result.var), Z_STRLEN_P(value));
	} else {
		zend_bool strict;

		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
			value = GET_OP1_UNDEF_CV(value, BP_VAR_R);
		}
		if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(value) == IS_REFERENCE) {
			value = Z_REFVAL_P(value);
			ZEND_VM_C_GOTO(try_strlen);
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

ZEND_VM_HANDLER(123, ZEND_TYPE_CHECK, CONST|TMP|VAR|CV, ANY, TYPE)
{
	USE_OPLINE
	zval *value;
	int result = 0;
	zend_free_op free_op1;

	SAVE_OPLINE();
	value = GET_OP1_ZVAL_PTR_DEREF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(value) == opline->extended_value)) {
		if (OP1_TYPE != IS_CONST && UNEXPECTED(Z_TYPE_P(value) == IS_OBJECT)) {
			zend_class_entry *ce = Z_OBJCE_P(value);

			if (EXPECTED(ZSTR_LEN(ce->name) != sizeof("__PHP_Incomplete_Class") - 1) ||
			    EXPECTED(memcmp(ZSTR_VAL(ce->name), "__PHP_Incomplete_Class", sizeof("__PHP_Incomplete_Class") - 1) != 0)) {
				result = 1;
			}
		} else if (UNEXPECTED(Z_TYPE_P(value) == IS_RESOURCE)) {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(value));

			if (EXPECTED(type_name != NULL)) {
				result = 1;
			}
		} else {
			result = 1;
		}
	} else if (UNEXPECTED(opline->extended_value == _IS_BOOL) &&
			   EXPECTED(Z_TYPE_P(value) == IS_TRUE || Z_TYPE_P(value) == IS_FALSE)) {
		result = 1;
	}
	FREE_OP1();
	ZEND_VM_SMART_BRANCH(result, 1);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(122, ZEND_DEFINED, CONST, ANY)
{
	USE_OPLINE
	zend_constant *c;
	int result;

	if (EXPECTED(CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1))))) {
		result = 1;
	} else if ((c = zend_quick_get_constant(EX_CONSTANT(opline->op1), 0)) == NULL) {
		result = 0;
	} else {
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), c);
		result = 1;
	}
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
		ZEND_VM_JMP(target);
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(157, ZEND_FETCH_CLASS_NAME, ANY, ANY, CLASS_FETCH)
{
	uint32_t fetch_type;
	zend_class_entry *called_scope, *scope;
	USE_OPLINE

	SAVE_OPLINE();
	fetch_type = opline->extended_value;

	scope = EX(func)->op_array.scope;
	if (UNEXPECTED(scope == NULL)) {
		zend_throw_error(NULL, "Cannot use \"%s\" when no class scope is active",
			fetch_type == ZEND_FETCH_CLASS_SELF ? "self" :
			fetch_type == ZEND_FETCH_CLASS_PARENT ? "parent" : "static");
		HANDLE_EXCEPTION();
	}

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			ZVAL_STR_COPY(EX_VAR(opline->result.var), scope->name);
			break;
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(scope->parent == NULL)) {
				zend_throw_error(NULL,
					"Cannot use \"parent\" when current class scope has no parent");
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
	USE_OPLINE

	args = emalloc(sizeof(zend_array));
	zend_hash_init(args, num_args, NULL, ZVAL_PTR_DTOR, 0);
	if (num_args) {
		zval *p = ZEND_CALL_ARG(execute_data, 1);
		zval *end = p + num_args;

		zend_hash_real_init(args, 1);
		ZEND_HASH_FILL_PACKED(args) {
			do {
				ZEND_HASH_FILL_ADD(p);
				p++;
			} while (p != end);
		} ZEND_HASH_FILL_END();
	}

	SAVE_OPLINE();
	call = execute_data;
	execute_data = EG(current_execute_data) = EX(prev_execute_data);

	ZEND_ASSERT(zend_vm_calc_used_stack(2, fbc->common.prototype) <= (size_t)(((char*)EG(vm_stack_end)) - (char*)call));

	call->func = fbc->common.prototype;
	ZEND_CALL_NUM_ARGS(call) = 2;

	ZVAL_STR(ZEND_CALL_ARG(call, 1), fbc->common.function_name);
	ZVAL_ARR(ZEND_CALL_ARG(call, 2), args);
	zend_free_trampoline(fbc);
	fbc = call->func;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {

		ZEND_ASSERT(!(fbc->common.fn_flags & ZEND_ACC_GENERATOR));

		if (UNEXPECTED(!fbc->op_array.run_time_cache)) {
			init_func_run_time_cache(&fbc->op_array);
		}
		i_init_func_execute_data(call, &fbc->op_array, ret);
		if (EXPECTED(zend_execute_ex == execute_ex)) {
			ZEND_VM_ENTER();
		} else {
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
		ZEND_ASSERT(
			EG(exception) || !call->func ||
			!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
			zend_verify_internal_return_type(call->func, ret));
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

	opline = EX(opline);

	if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
		zend_object *object = Z_OBJ(call->This);
		OBJ_RELEASE(object);
	}
	zend_vm_stack_free_call_frame(call);

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION_LEAVE();
	}

	ZEND_VM_INC_OPCODE();
	ZEND_VM_LEAVE();
}

ZEND_VM_HANDLER(182, ZEND_BIND_LEXICAL, TMP, CV, REF)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *closure, *var;
	zend_string *var_name;

	closure = GET_OP1_ZVAL_PTR(BP_VAR_R);
	if (opline->extended_value) {
		/* By-ref binding */
		var = GET_OP2_ZVAL_PTR(BP_VAR_W);
		ZVAL_MAKE_REF(var);
		Z_ADDREF_P(var);
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

	var_name = CV_DEF_OF(EX_VAR_TO_NUM(opline->op2.var));
	zend_closure_bind_var(closure, var_name, var);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(183, ZEND_BIND_STATIC, CV, CONST, REF)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	HashTable *ht;
	zval *varname;
	zval *value;
	zval *variable_ptr;

	SAVE_OPLINE();
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	zval_ptr_dtor(variable_ptr);

	ht = EX(func)->op_array.static_variables;
	ZEND_ASSERT(ht != NULL);
	if (GC_REFCOUNT(ht) > 1) {
		if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
			GC_REFCOUNT(ht)--;
		}
		EX(func)->op_array.static_variables = ht = zend_array_dup(ht);
	}

	varname = GET_OP2_ZVAL_PTR(BP_VAR_R);
	value = zend_hash_find(ht, Z_STR_P(varname));

	if (opline->extended_value) {
		if (Z_CONSTANT_P(value)) {
			if (UNEXPECTED(zval_update_constant_ex(value, EX(func)->op_array.scope) != SUCCESS)) {
				ZVAL_NULL(variable_ptr);
				HANDLE_EXCEPTION();
			}
		}
		if (UNEXPECTED(!Z_ISREF_P(value))) {
			zend_reference *ref = (zend_reference*)emalloc(sizeof(zend_reference));
			GC_REFCOUNT(ref) = 2;
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

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
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
		SAVE_OPLINE();
		zend_throw_error(NULL, "Using $this when not in object context");
		HANDLE_EXCEPTION();
	}
}

ZEND_VM_HANDLER(186, ZEND_ISSET_ISEMPTY_THIS, UNUSED, UNUSED)
{
	USE_OPLINE

	ZVAL_BOOL(EX_VAR(opline->result.var),
		(opline->extended_value & ZEND_ISSET) ?
			 (Z_TYPE(EX(This)) == IS_OBJECT) :
			 (Z_TYPE(EX(This)) != IS_OBJECT));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_ADD, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_ADD_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) + Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_ADD, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_ADD_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	fast_long_add_function(result, op1, op2);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_ADD, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_ADD_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_SUB, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_SUB_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) - Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_SUB, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_SUB_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	fast_long_sub_function(result, op1, op2);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_SUB, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_SUB_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_MUL, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_MUL_LONG_NO_OVERFLOW, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_LONG(result, Z_LVAL_P(op1) * Z_LVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_MUL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_MUL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_MUL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_MUL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(NO_CONST_CONST,COMMUTATIVE))
{
	USE_OPLINE
	zval *op1, *op2, *result;

	op1 = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	op2 = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	result = EX_VAR(opline->result.var);
	ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_NOT_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_NOT_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_NOT_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST,COMMUTATIVE))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_LONG && op2_info == MAY_BE_LONG), ZEND_IS_SMALLER_OR_EQUAL_LONG, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_IS_SMALLER_OR_EQUAL, (op1_info == MAY_BE_DOUBLE && op2_info == MAY_BE_DOUBLE), ZEND_IS_SMALLER_OR_EQUAL_DOUBLE, CONST|TMPVARCV, CONST|TMPVARCV, SPEC(SMART_BRANCH,NO_CONST_CONST))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG_NO_OVERFLOW, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (op1_info == MAY_BE_LONG), ZEND_PRE_INC_LONG, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_INC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_PRE_INC_LONG_OR_DOUBLE, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG_NO_OVERFLOW, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (op1_info == MAY_BE_LONG), ZEND_PRE_DEC_LONG, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_PRE_DEC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_PRE_DEC_LONG_OR_DOUBLE, TMPVARCV, ANY, SPEC(RETVAL))
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_INC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG_NO_OVERFLOW, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)++;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_INC, (op1_info == MAY_BE_LONG), ZEND_POST_INC_LONG, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_increment_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_INC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_POST_INC_LONG_OR_DOUBLE, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_increment_function(var_ptr);
	} else {
		Z_DVAL_P(var_ptr)++;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (res_info == MAY_BE_LONG && op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG_NO_OVERFLOW, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	Z_LVAL_P(var_ptr)--;
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (op1_info == MAY_BE_LONG), ZEND_POST_DEC_LONG, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_LONG(EX_VAR(opline->result.var), Z_LVAL_P(var_ptr));
	fast_long_decrement_function(var_ptr);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_POST_DEC, (op1_info == (MAY_BE_LONG|MAY_BE_DOUBLE)), ZEND_POST_DEC_LONG_OR_DOUBLE, TMPVARCV, ANY)
{
	USE_OPLINE
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_decrement_function(var_ptr);
	} else {
		Z_DVAL_P(var_ptr)--;
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, (op1_info == MAY_BE_DOUBLE), ZEND_QM_ASSIGN_DOUBLE, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_DOUBLE(EX_VAR(opline->result.var), Z_DVAL_P(value));
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_QM_ASSIGN, (!(op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE)))), ZEND_QM_ASSIGN_NOREF, CONST|TMPVARCV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *value;

	value = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_FETCH_DIM_R, (!(op2_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))), ZEND_FETCH_DIM_R_INDEX, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container, *dim, *value;
	zend_long offset;

	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	dim = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
ZEND_VM_C_LABEL(fetch_dim_r_index_array):
		if (EXPECTED(Z_TYPE_P(dim) == IS_LONG)) {
			offset = Z_LVAL_P(dim);
		} else {
			offset = zval_get_long(dim);
		}
		ZEND_HASH_INDEX_FIND(Z_ARRVAL_P(container), offset, value, ZEND_VM_C_LABEL(fetch_dim_r_index_undef));
		ZVAL_COPY(EX_VAR(opline->result.var), value);
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
		zend_fetch_dimension_address_read_R_slow(EX_VAR(opline->result.var), container, dim);
		FREE_OP1();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}

ZEND_VM_C_LABEL(fetch_dim_r_index_undef):
	ZVAL_NULL(EX_VAR(opline->result.var));
	SAVE_OPLINE();
	zend_error(E_NOTICE, "Undefined offset: " ZEND_LONG_FMT, offset);
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_SEND_VAR, (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_SIMPLE, CV|VAR, NUM)
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

ZEND_VM_TYPE_SPEC_HANDLER(ZEND_SEND_VAR_EX, (op1_info & (MAY_BE_UNDEF|MAY_BE_REF)) == 0, ZEND_SEND_VAR_EX_SIMPLE, CV|VAR, NUM, SPEC(QUICK_ARG))
{
	USE_OPLINE
	zval *varptr, *arg;
	zend_free_op free_op1;
	uint32_t arg_num = opline->op2.num;

	if (EXPECTED(arg_num <= MAX_ARG_FLAG_NUM)) {
		if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
			ZEND_VM_C_GOTO(send_var_by_ref_simple);
		}
	} else if (ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
ZEND_VM_C_LABEL(send_var_by_ref_simple):
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

ZEND_VM_DEFINE_OP(137, ZEND_OP_DATA);
