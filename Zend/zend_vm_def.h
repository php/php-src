/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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

				str = zend_string_realloc(op1_str, len + ZSTR_LEN(op2_str), 0);
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
		if (UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
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

ZEND_VM_HELPER_EX(zend_binary_assign_op_obj_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data1;
	zval *object;
	zval *property;
	zval *value;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(object) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(object == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an object");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

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
			if (UNEXPECTED(zptr == &EG(error_zval))) {
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

ZEND_VM_HELPER_EX(zend_binary_assign_op_dim_helper, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2, free_op_data1;
	zval *var_ptr, rv;
	zval *value, *container, *dim;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	dim = GET_OP2_ZVAL_PTR(BP_VAR_R);

	do {
		if (OP1_TYPE == IS_UNUSED || UNEXPECTED(Z_TYPE_P(container) != IS_ARRAY)) {
			if (OP1_TYPE != IS_UNUSED) {
				ZVAL_DEREF(container);
			}
			if (OP1_TYPE == IS_UNUSED || EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
				value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
				zend_binary_assign_op_obj_dim(container, dim, value, UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL, binary_op);
				break;
			}
		}

		zend_fetch_dimension_address_RW(&rv, container, dim, OP2_TYPE);
		value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
		ZEND_ASSERT(Z_TYPE(rv) == IS_INDIRECT);
		var_ptr = Z_INDIRECT(rv);

		if (UNEXPECTED(var_ptr == NULL)) {
			zend_throw_error(NULL, "Cannot use assign-op operators with overloaded objects nor string offsets");
			FREE_OP2();
			FREE_OP(free_op_data1);
			FREE_OP1_VAR_PTR();
			HANDLE_EXCEPTION();
		}

		if (UNEXPECTED(var_ptr == &EG(error_zval))) {
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
	} while (0);

	FREE_OP2();
	FREE_OP(free_op_data1);
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HELPER_EX(zend_binary_assign_op_helper, VAR|CV, CONST|TMPVAR|CV, binary_op_type binary_op)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *var_ptr;
	zval *value;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	var_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == NULL)) {
		zend_throw_error(NULL, "Cannot use assign-op operators with overloaded objects nor string offsets");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
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

ZEND_VM_HANDLER(23, ZEND_ASSIGN_ADD, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, add_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, add_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, add_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, add_function);
#endif
}

ZEND_VM_HANDLER(24, ZEND_ASSIGN_SUB, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, sub_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, sub_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, sub_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, sub_function);
#endif
}

ZEND_VM_HANDLER(25, ZEND_ASSIGN_MUL, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mul_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, mul_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, mul_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, mul_function);
#endif
}

ZEND_VM_HANDLER(26, ZEND_ASSIGN_DIV, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, div_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, div_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, div_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, div_function);
#endif
}

ZEND_VM_HANDLER(27, ZEND_ASSIGN_MOD, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, mod_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, mod_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, mod_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, mod_function);
#endif
}

ZEND_VM_HANDLER(28, ZEND_ASSIGN_SL, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_left_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, shift_left_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, shift_left_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, shift_left_function);
#endif
}

ZEND_VM_HANDLER(29, ZEND_ASSIGN_SR, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, shift_right_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, shift_right_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, shift_right_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, shift_right_function);
#endif
}

ZEND_VM_HANDLER(30, ZEND_ASSIGN_CONCAT, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, concat_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, concat_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, concat_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, concat_function);
#endif
}

ZEND_VM_HANDLER(31, ZEND_ASSIGN_BW_OR, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_or_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_or_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, bitwise_or_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_or_function);
#endif
}

ZEND_VM_HANDLER(32, ZEND_ASSIGN_BW_AND, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_and_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_and_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, bitwise_and_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_and_function);
#endif
}

ZEND_VM_HANDLER(33, ZEND_ASSIGN_BW_XOR, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, bitwise_xor_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_xor_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, bitwise_xor_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, bitwise_xor_function);
#endif
}

ZEND_VM_HANDLER(167, ZEND_ASSIGN_POW, VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
{
#if !defined(ZEND_VM_SPEC) || (OP2_TYPE != IS_UNUSED)
	USE_OPLINE

# if !defined(ZEND_VM_SPEC) || (OP1_TYPE != IS_UNUSED)
	if (EXPECTED(opline->extended_value == 0)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_helper, binary_op, pow_function);
	}
# endif
	if (EXPECTED(opline->extended_value == ZEND_ASSIGN_DIM)) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, pow_function);
	} else /* if (EXPECTED(opline->extended_value == ZEND_ASSIGN_OBJ)) */ {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_obj_helper, binary_op, pow_function);
	}
#else
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_binary_assign_op_dim_helper, binary_op, pow_function);
#endif
}

ZEND_VM_HELPER_EX(zend_pre_incdec_property_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, int inc)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(object) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(object == NULL)) {
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

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
			if (UNEXPECTED(zptr == &EG(error_zval))) {
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

ZEND_VM_HANDLER(132, ZEND_PRE_INC_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(133, ZEND_PRE_DEC_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_pre_incdec_property_helper, inc, 0);
}

ZEND_VM_HELPER_EX(zend_post_incdec_property_helper, VAR|UNUSED|CV, CONST|TMPVAR|CV, int inc)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property;
	zval *zptr;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(object) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(object == NULL)) {
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

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
			if (UNEXPECTED(zptr == &EG(error_zval))) {
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

ZEND_VM_HANDLER(134, ZEND_POST_INC_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, inc, 1);
}

ZEND_VM_HANDLER(135, ZEND_POST_DEC_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_post_incdec_property_helper, inc, 0);
}

ZEND_VM_HANDLER(34, ZEND_PRE_INC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		HANDLE_EXCEPTION();
	}

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_increment_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
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

ZEND_VM_HANDLER(35, ZEND_PRE_DEC, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *var_ptr;

	var_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		HANDLE_EXCEPTION();
	}

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		fast_long_decrement_function(var_ptr);
		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		}
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
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

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		HANDLE_EXCEPTION();
	}

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		fast_long_increment_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
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

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == NULL)) {
		SAVE_OPLINE();
		zend_throw_error(NULL, "Cannot increment/decrement overloaded objects nor string offsets");
		HANDLE_EXCEPTION();
	}

	if (EXPECTED(Z_TYPE_P(var_ptr) == IS_LONG)) {
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), var_ptr);
		fast_long_decrement_function(var_ptr);
		ZEND_VM_NEXT_OPCODE();
	}

	if (OP1_TYPE == IS_VAR && UNEXPECTED(var_ptr == &EG(error_zval))) {
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

ZEND_VM_HELPER_EX(zend_fetch_var_address_helper, CONST|TMPVAR|CV, UNUSED|CONST|VAR, int type)
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

	if (OP2_TYPE != IS_UNUSED) {
		zend_class_entry *ce;

		if (OP2_TYPE == IS_CONST) {
			if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)))) != NULL)) {
				retval = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)) + sizeof(void*));

				/* check if static properties were destoyed */
				if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
					if (type == BP_VAR_IS) {
						retval = &EG(uninitialized_zval);
					} else {
						zend_throw_error(NULL, "Access to undeclared static property: %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
						FREE_OP1();
						HANDLE_EXCEPTION();
					}
				}

				ZEND_VM_C_GOTO(fetch_var_return);
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
			ce = Z_CE_P(EX_VAR(opline->op2.var));
			if (OP1_TYPE == IS_CONST &&
			    (retval = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce)) != NULL) {
				
				/* check if static properties were destoyed */
				if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
					if (type == BP_VAR_IS) {
						retval = &EG(uninitialized_zval);
					} else {
						zend_throw_error(NULL, "Access to undeclared static property: %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
						FREE_OP1();
						HANDLE_EXCEPTION();
					}
				}

				ZEND_VM_C_GOTO(fetch_var_return);
			}
		}
		retval = zend_std_get_static_property(ce, name, type == BP_VAR_IS);
		if (UNEXPECTED(EG(exception))) {
			if (OP1_TYPE != IS_CONST) {
				zend_string_release(name);
			}
			FREE_OP1();
			HANDLE_EXCEPTION();
		}
		if (EXPECTED(retval)) {
			if (OP1_TYPE == IS_CONST) {
				CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce, retval);
			}
		} else {
			retval = &EG(uninitialized_zval);
		}

		FREE_OP1();
	} else {
		target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
		retval = zend_hash_find(target_symbol_table, name);
		if (retval == NULL) {
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
		if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_STATIC) {
			if (Z_CONSTANT_P(retval)) {
				if (UNEXPECTED(zval_update_constant_ex(retval, 1, NULL) != SUCCESS)) {
					FREE_OP1();
					HANDLE_EXCEPTION();
				}
			}
		} else if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) != ZEND_FETCH_GLOBAL_LOCK) {
			FREE_OP1();
		}
	}

	if (OP1_TYPE != IS_CONST) {
		zend_string_release(name);
	}

ZEND_VM_C_LABEL(fetch_var_return):
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

ZEND_VM_HANDLER(80, ZEND_FETCH_R, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_R);
}

ZEND_VM_HANDLER(83, ZEND_FETCH_W, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_W);
}

ZEND_VM_HANDLER(86, ZEND_FETCH_RW, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_RW);
}

ZEND_VM_HANDLER(92, ZEND_FETCH_FUNC_ARG, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_W);
	} else {
		ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_R);
	}
}

ZEND_VM_HANDLER(95, ZEND_FETCH_UNSET, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_UNSET);
}

ZEND_VM_HANDLER(89, ZEND_FETCH_IS, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	ZEND_VM_DISPATCH_TO_HELPER_EX(zend_fetch_var_address_helper, type, BP_VAR_IS);
}

ZEND_VM_HANDLER(81, ZEND_FETCH_DIM_R, CONST|TMPVAR|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR(BP_VAR_R);
	zend_fetch_dimension_address_read_R(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(84, ZEND_FETCH_DIM_W, VAR|CV, CONST|TMPVAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		HANDLE_EXCEPTION();
	}
	zend_fetch_dimension_address_W(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 1);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(87, ZEND_FETCH_DIM_RW, VAR|CV, CONST|TMPVAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		HANDLE_EXCEPTION();
	}
	zend_fetch_dimension_address_RW(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 1);
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
	container = GET_OP1_ZVAL_PTR(BP_VAR_IS);
	zend_fetch_dimension_address_read_IS(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(93, ZEND_FETCH_DIM_FUNC_ARG, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|CV)
{
	USE_OPLINE
	zval *container;
	zend_free_op free_op1, free_op2;

	SAVE_OPLINE();

	if (zend_is_by_ref_func_arg_fetch(opline, EX(call))) {
        if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
            zend_throw_error(NULL, "Cannot use temporary expression in write context");
			FREE_UNFETCHED_OP2();
			FREE_UNFETCHED_OP1();
			HANDLE_EXCEPTION();
        }
		container = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
		if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
			zend_throw_error(NULL, "Cannot use string offset as an array");
			FREE_UNFETCHED_OP2();
			HANDLE_EXCEPTION();
		}
		zend_fetch_dimension_address_W(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
		if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
			EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 1);
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
		container = GET_OP1_ZVAL_PTR(BP_VAR_R);
		zend_fetch_dimension_address_read_R(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
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
	container = GET_OP1_ZVAL_PTR_PTR(BP_VAR_UNSET);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}
	zend_fetch_dimension_address_UNSET(EX_VAR(opline->result.var), container, GET_OP2_ZVAL_PTR(BP_VAR_R), OP2_TYPE);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 1);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(82, ZEND_FETCH_OBJ_R, CONST|TMP|VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
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

ZEND_VM_HANDLER(85, ZEND_FETCH_OBJ_W, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	container = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an object");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_W);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 0);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(88, ZEND_FETCH_OBJ_RW, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *property;
	zval *container;

	SAVE_OPLINE();
	property = GET_OP2_ZVAL_PTR(BP_VAR_R);
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_RW);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an object");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}
	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_RW);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 0);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(91, ZEND_FETCH_OBJ_IS, CONST|TMPVAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;
	zend_free_op free_op2;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
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

ZEND_VM_HANDLER(94, ZEND_FETCH_OBJ_FUNC_ARG, CONST|TMP|VAR|UNUSED|CV, CONST|TMPVAR|CV)
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

		if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
			zend_throw_error(NULL, "Using $this when not in object context");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
			zend_throw_error(NULL, "Cannot use temporary expression in write context");
			FREE_OP2();
			FREE_OP1_VAR_PTR();
			HANDLE_EXCEPTION();
		}
		if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
			zend_throw_error(NULL, "Cannot use string offset as an object");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
		zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_W);
		FREE_OP2();
		if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
			EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 0);
		}
		FREE_OP1_VAR_PTR();
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else {
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_FETCH_OBJ_R);
	}
}

ZEND_VM_HANDLER(97, ZEND_FETCH_OBJ_UNSET, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container, *property;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	property = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an object");
		FREE_OP2();
		HANDLE_EXCEPTION();
	}
	zend_fetch_property_address(EX_VAR(opline->result.var), container, OP1_TYPE, property, OP2_TYPE, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property)) : NULL), BP_VAR_UNSET);
	FREE_OP2();
	if (OP1_TYPE == IS_VAR && READY_TO_DESTROY(free_op1)) {
		EXTRACT_ZVAL_PTR(EX_VAR(opline->result.var), 0);
	}
	FREE_OP1_VAR_PTR();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(98, ZEND_FETCH_LIST, CONST|TMPVAR|CV, CONST)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *container;

	SAVE_OPLINE();
	container = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_fetch_list):
	if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
		zval *value = zend_hash_index_find(Z_ARRVAL_P(container), Z_LVAL_P(EX_CONSTANT(opline->op2)));

		if (UNEXPECTED(value == NULL)) {
			zend_error(E_NOTICE,"Undefined offset: " ZEND_ULONG_FMT, Z_LVAL_P(EX_CONSTANT(opline->op2)));
			ZVAL_NULL(EX_VAR(opline->result.var));
		} else {
			ZVAL_COPY(EX_VAR(opline->result.var), value);
		}
	} else if (OP1_TYPE != IS_CONST &&
	           UNEXPECTED(Z_TYPE_P(container) == IS_OBJECT) &&
	           EXPECTED(Z_OBJ_HT_P(container)->read_dimension)) {
		zval *result = EX_VAR(opline->result.var);
		zval *retval = Z_OBJ_HT_P(container)->read_dimension(container, EX_CONSTANT(opline->op2), BP_VAR_R, result);

		if (retval) {
			if (result != retval) {
				ZVAL_COPY(result, retval);
			}
		} else {
			ZVAL_NULL(result);
		}
	} else if ((OP1_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(container) == IS_REFERENCE) {
		container = Z_REFVAL_P(container);
		ZEND_VM_C_GOTO(try_fetch_list);
	} else {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(container) == IS_UNDEF)) {
			GET_OP1_UNDEF_CV(container, BP_VAR_R);
		}
		ZVAL_NULL(EX_VAR(opline->result.var));
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(136, ZEND_ASSIGN_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *object;
	zval *property_name;

	SAVE_OPLINE();
	object = GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(object) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		HANDLE_EXCEPTION();
	}

	property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(object == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		FREE_OP2();
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		HANDLE_EXCEPTION();
	}
	zend_assign_to_object(UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL, object, OP1_TYPE, property_name, OP2_TYPE, (opline+1)->op1_type, (opline+1)->op1, execute_data, ((OP2_TYPE == IS_CONST) ? CACHE_ADDR(Z_CACHE_SLOT_P(property_name)) : NULL));
	FREE_OP2();
	FREE_OP1_VAR_PTR();
	/* assign_obj has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(147, ZEND_ASSIGN_DIM, VAR|CV, CONST|TMPVAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *object_ptr;
	zend_free_op free_op2, free_op_data1;
	zval *value;
	zval *variable_ptr;
	zval *dim;

	SAVE_OPLINE();
	object_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(object_ptr == NULL)) {
		zend_throw_error(NULL, "Cannot use string offset as an array");
		FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
ZEND_VM_C_LABEL(try_assign_dim_array):
		if (OP2_TYPE == IS_UNUSED) {
			SEPARATE_ARRAY(object_ptr);
			variable_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(object_ptr), &EG(uninitialized_zval));
			if (UNEXPECTED(variable_ptr == NULL)) {
				zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
				variable_ptr = &EG(error_zval);
			}
		} else {
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
			SEPARATE_ARRAY(object_ptr);
			variable_ptr = zend_fetch_dimension_address_inner(Z_ARRVAL_P(object_ptr), dim, OP2_TYPE, BP_VAR_W);
			FREE_OP2();
		}
		value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
		if (UNEXPECTED(variable_ptr == &EG(error_zval))) {
			FREE_OP(free_op_data1);
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		} else {
			value = zend_assign_to_variable(variable_ptr, value, (opline+1)->op1_type);
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_COPY(EX_VAR(opline->result.var), value);
			}
		}
	} else {
		if (EXPECTED(Z_ISREF_P(object_ptr))) {
			object_ptr = Z_REFVAL_P(object_ptr);
			if (EXPECTED(Z_TYPE_P(object_ptr) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(try_assign_dim_array);
			}
		}
		if (EXPECTED(Z_TYPE_P(object_ptr) == IS_OBJECT)) {
			zend_free_op free_op2;
			zval *property_name = GET_OP2_ZVAL_PTR(BP_VAR_R);

			zend_assign_to_object_dim(UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL, object_ptr, property_name, (opline+1)->op1_type, (opline+1)->op1, execute_data);
			FREE_OP2();
		} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
			if (EXPECTED(Z_STRLEN_P(object_ptr) != 0)) {
				if (OP2_TYPE == IS_UNUSED) {
					zend_throw_error(NULL, "[] operator not supported for strings");
					FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var);
					FREE_OP1_VAR_PTR();
					HANDLE_EXCEPTION();
				} else {
					zend_long offset;

					dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
					offset = zend_fetch_string_offset(object_ptr, dim, BP_VAR_W);
					FREE_OP2();
					value = get_zval_ptr_r_deref((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
					zend_assign_to_string_offset(object_ptr, offset, value, (UNEXPECTED(RETURN_VALUE_USED(opline)) ? EX_VAR(opline->result.var) : NULL));
					FREE_OP(free_op_data1);
				}
			} else {
				zval_ptr_dtor_nogc(object_ptr);
ZEND_VM_C_LABEL(assign_dim_convert_to_array):
				ZVAL_NEW_ARR(object_ptr);
				zend_hash_init(Z_ARRVAL_P(object_ptr), 8, NULL, ZVAL_PTR_DTOR, 0);
				ZEND_VM_C_GOTO(try_assign_dim_array);
			}
		} else if (EXPECTED(Z_TYPE_P(object_ptr) <= IS_FALSE)) {
			if (OP1_TYPE == IS_VAR && UNEXPECTED(object_ptr == &EG(error_zval))) {
				ZEND_VM_C_GOTO(assign_dim_clean);
			}
			ZEND_VM_C_GOTO(assign_dim_convert_to_array);
		} else {
			zend_error(E_WARNING, "Cannot use a scalar value as an array");
ZEND_VM_C_LABEL(assign_dim_clean):
			dim = GET_OP2_ZVAL_PTR(BP_VAR_R);
			FREE_OP2();
			value = get_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data1);
			FREE_OP(free_op_data1);
			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
				ZVAL_NULL(EX_VAR(opline->result.var));
			}
		}
	}
	FREE_OP1_VAR_PTR();
	/* assign_dim has two opcodes! */
	ZEND_VM_NEXT_OPCODE_EX(1, 2);
}

ZEND_VM_HANDLER(38, ZEND_ASSIGN, VAR|CV, CONST|TMP|VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *value;
	zval *variable_ptr;

	SAVE_OPLINE();
	value = GET_OP2_ZVAL_PTR(BP_VAR_R);
	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(variable_ptr == &EG(error_zval))) {
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

ZEND_VM_HANDLER(39, ZEND_ASSIGN_REF, VAR|CV, VAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *variable_ptr;
	zval *value_ptr;

	SAVE_OPLINE();
	value_ptr = GET_OP2_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP2_TYPE == IS_VAR && UNEXPECTED(value_ptr == NULL)) {
		zend_throw_error(NULL, "Cannot create references to/from string offsets nor overloaded objects");
		FREE_UNFETCHED_OP1();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR &&
	    UNEXPECTED(Z_TYPE_P(EX_VAR(opline->op1.var)) != IS_INDIRECT) &&
	    UNEXPECTED(!Z_ISREF_P(EX_VAR(opline->op1.var)))) {
		zend_throw_error(NULL, "Cannot assign by reference to overloaded object");
		FREE_UNFETCHED_OP1();
		FREE_OP2_VAR_PTR();
		HANDLE_EXCEPTION();
	}
	if (OP2_TYPE == IS_VAR &&
	    (value_ptr == &EG(uninitialized_zval) ||
	     (opline->extended_value == ZEND_RETURNS_FUNCTION &&
	      !(Z_VAR_FLAGS_P(value_ptr) & IS_VAR_RET_REF)))) {
		if (!OP2_FREE && UNEXPECTED(Z_TYPE_P(EX_VAR(opline->op2.var)) != IS_INDIRECT)) { /* undo the effect of get_zval_ptr_ptr() */
			Z_TRY_ADDREF_P(value_ptr);
		}
		zend_error(E_NOTICE, "Only variables should be assigned by reference");
		if (UNEXPECTED(EG(exception) != NULL)) {
			FREE_OP2_VAR_PTR();
			HANDLE_EXCEPTION();
		}
		ZEND_VM_DISPATCH_TO_HANDLER(ZEND_ASSIGN);
	}

	variable_ptr = GET_OP1_ZVAL_PTR_PTR_UNDEF(BP_VAR_W);
	if (OP1_TYPE == IS_VAR && UNEXPECTED(variable_ptr == NULL)) {
		zend_throw_error(NULL, "Cannot create references to/from string offsets nor overloaded objects");
		FREE_OP2_VAR_PTR();
		HANDLE_EXCEPTION();
	}
	if ((OP1_TYPE == IS_VAR && UNEXPECTED(variable_ptr == &EG(error_zval))) ||
	    (OP2_TYPE == IS_VAR && UNEXPECTED(value_ptr == &EG(error_zval)))) {
		variable_ptr = &EG(uninitialized_zval);
	} else {
		zend_assign_to_variable_reference(variable_ptr, value_ptr);
	}

	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
		ZVAL_COPY(EX_VAR(opline->result.var), variable_ptr);
	}

	FREE_OP1_VAR_PTR();
	FREE_OP2_VAR_PTR();

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HELPER(zend_leave_helper, ANY, ANY)
{
	zend_execute_data *old_execute_data;
	uint32_t call_info = EX_CALL_INFO();

	if (EXPECTED(ZEND_CALL_KIND_EX(call_info) == ZEND_CALL_NESTED_FUNCTION)) {
		zend_object *object;

		i_free_compiled_variables(execute_data);
		if (UNEXPECTED(EX(symbol_table) != NULL)) {
			zend_clean_and_cache_symbol_table(EX(symbol_table));
		}
		zend_vm_stack_free_extra_args_ex(call_info, execute_data);
		old_execute_data = execute_data;
		execute_data = EG(current_execute_data) = EX(prev_execute_data);
		if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
			OBJ_RELEASE((zend_object*)old_execute_data->func->op_array.prototype);
		}
		if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
			object = Z_OBJ(old_execute_data->This);
#if 0
			if (UNEXPECTED(EG(exception) != NULL) && (EX(opline)->op1.num & ZEND_CALL_CTOR)) {
				if (!(EX(opline)->op1.num & ZEND_CALL_CTOR_RESULT_UNUSED)) {
#else
			if (UNEXPECTED(EG(exception) != NULL) && (call_info & ZEND_CALL_CTOR)) {
				if (!(call_info & ZEND_CALL_CTOR_RESULT_UNUSED)) {
#endif
					GC_REFCOUNT(object)--;
				}
				if (GC_REFCOUNT(object) == 1) {
					zend_object_store_ctor_failed(object);
				}
			}
			OBJ_RELEASE(object);
		}
		EG(scope) = EX(func)->op_array.scope;

		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			const zend_op *old_opline = EX(opline);
			zend_throw_exception_internal(NULL);
			if (old_opline->opcode != ZEND_HANDLE_EXCEPTION && RETURN_VALUE_USED(old_opline)) {
				zval_ptr_dtor(EX_VAR(old_opline->result.var));
			}
			HANDLE_EXCEPTION_LEAVE();
		}

		LOAD_NEXT_OPLINE();
		ZEND_VM_LEAVE();
	}
	if (EXPECTED((ZEND_CALL_KIND_EX(call_info) & ZEND_CALL_TOP) == 0)) {
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
		if (ZEND_CALL_KIND_EX(call_info) == ZEND_CALL_TOP_FUNCTION) {
			i_free_compiled_variables(execute_data);
			if (UNEXPECTED(EX(symbol_table) != NULL)) {
				zend_clean_and_cache_symbol_table(EX(symbol_table));
			}
			zend_vm_stack_free_extra_args_ex(call_info, execute_data);
			EG(current_execute_data) = EX(prev_execute_data);
			if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
				OBJ_RELEASE((zend_object*)EX(func)->op_array.prototype);
			}
		} else /* if (call_kind == ZEND_CALL_TOP_CODE) */ {
			zend_array *symbol_table = EX(symbol_table);

			zend_detach_symbol_table(execute_data);
			old_execute_data = EX(prev_execute_data);
			while (old_execute_data) {
				if (old_execute_data->func && ZEND_USER_CODE(old_execute_data->func->op_array.type)) {
					if (old_execute_data->symbol_table == symbol_table) {
						zend_attach_symbol_table(old_execute_data);
					}
					break;
				}
				old_execute_data = old_execute_data->prev_execute_data;
			}
			EG(current_execute_data) = EX(prev_execute_data);
		}

		ZEND_VM_RETURN();
	}
}

ZEND_VM_HANDLER(42, ZEND_JMP, ANY, ANY)
{
	USE_OPLINE

	ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op1));
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(43, ZEND_JMPZ, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(44, ZEND_JMPNZ, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(45, ZEND_JMPZNZ, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(46, ZEND_JMPZ_EX, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(47, ZEND_JMPNZ_EX, CONST|TMPVAR|CV, ANY)
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

ZEND_VM_HANDLER(70, ZEND_FREE, TMPVAR, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(127, ZEND_FE_FREE, TMPVAR, ANY)
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

ZEND_VM_HANDLER(54, ZEND_ROPE_INIT, UNUSED, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(55, ZEND_ROPE_ADD, TMP, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(56, ZEND_ROPE_END, TMP, CONST|TMPVAR|CV)
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

ZEND_VM_HANDLER(109, ZEND_FETCH_CLASS, ANY, CONST|TMPVAR|UNUSED|CV)
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

ZEND_VM_HANDLER(112, ZEND_INIT_METHOD_CALL, CONST|TMPVAR|UNUSED|CV, CONST|TMPVAR|CV)
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

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(object) == NULL)) {
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
	}

	call_info = ZEND_CALL_NESTED_FUNCTION;
	if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0)) {
		obj = NULL;
	} else if (OP1_TYPE & (IS_VAR|IS_TMP_VAR|IS_CV)) {
		/* CV may be changed indirectly (e.g. when it's a reference) */
		call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_RELEASE_THIS;
		GC_REFCOUNT(obj)++; /* For $this pointer */
	}

	FREE_OP2();
	FREE_OP1();

	if ((OP1_TYPE & (IS_VAR|IS_TMP_VAR)) && UNEXPECTED(EG(exception))) {
		HANDLE_EXCEPTION();
	}

	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, called_scope, obj);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(113, ZEND_INIT_STATIC_METHOD_CALL, CONST|VAR, CONST|TMPVAR|UNUSED|CV)
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
		if (OP2_TYPE != IS_CONST) {
			FREE_OP2();
		}
	} else {
		if (UNEXPECTED(ce->constructor == NULL)) {
			zend_throw_error(NULL, "Cannot call constructor");
			HANDLE_EXCEPTION();
		}
		if (Z_OBJ(EX(This)) && Z_OBJ(EX(This))->ce != ce->constructor->common.scope && (ce->constructor->common.fn_flags & ZEND_ACC_PRIVATE)) {
			zend_throw_error(NULL, "Cannot call private %s::__construct()", ZSTR_VAL(ce->name));
			HANDLE_EXCEPTION();
		}
		fbc = ce->constructor;
	}

	object = NULL;
	if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
		if (Z_OBJ(EX(This)) && instanceof_function(Z_OBJCE(EX(This)), ce)) {
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

	if (OP1_TYPE != IS_CONST) {
		/* previous opcode is ZEND_FETCH_CLASS */
		if (((opline-1)->extended_value & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_PARENT || 
		    ((opline-1)->extended_value & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_SELF) {
			ce = EX(called_scope);
		}
	}

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, ce, object);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(59, ZEND_INIT_FCALL_BY_NAME, ANY, CONST)
{
	USE_OPLINE
	zend_function *fbc;
	zval *function_name, *func;
	zend_execute_data *call;

	fbc = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
	if (UNEXPECTED(fbc == NULL)) {
		function_name = (zval*)(EX_CONSTANT(opline->op2)+1);
		func = zend_hash_find(EG(function_table), Z_STR_P(function_name));
		if (UNEXPECTED(func == NULL)) {
			SAVE_OPLINE();
			zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(EX_CONSTANT(opline->op2)));
			HANDLE_EXCEPTION();
		}
		fbc = Z_FUNC_P(func);
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), fbc);
	}
	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(128, ZEND_INIT_DYNAMIC_CALL, ANY, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_function *fbc;
	zval *function_name, *func;
	zend_string *lcname;
	zend_free_op free_op2;
	zend_class_entry *called_scope;
	zend_object *object;
	zend_execute_data *call;
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

ZEND_VM_C_LABEL(try_function_name):
	if (OP2_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(function_name) == IS_STRING)) {
		const char *colon;
		
		if ((colon = zend_memrchr(Z_STRVAL_P(function_name), ':', Z_STRLEN_P(function_name))) != NULL &&
			colon > Z_STRVAL_P(function_name) &&
			*(colon-1) == ':'
		) {
			zend_string *mname;
			size_t cname_length = colon - Z_STRVAL_P(function_name) - 1;
			size_t mname_length = Z_STRLEN_P(function_name) - cname_length - (sizeof("::") - 1);
			
			lcname = zend_string_init(Z_STRVAL_P(function_name), cname_length, 0);
			
			object = NULL;
			called_scope = zend_fetch_class_by_name(lcname, NULL, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(called_scope == NULL)) {
				zend_string_release(lcname);
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}
			
			mname = zend_string_init(Z_STRVAL_P(function_name) + (cname_length + sizeof("::") - 1), mname_length, 0);
			
			if (called_scope->get_static_method) {
				fbc = called_scope->get_static_method(called_scope, mname);
			} else {
				fbc = zend_std_get_static_method(called_scope, mname, NULL);
			}
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(called_scope->name), ZSTR_VAL(mname));
				}
				zend_string_release(lcname);
				zend_string_release(mname);
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
			
			zend_string_release(lcname);
			zend_string_release(mname);
			
			if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
				if (fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					zend_error(E_DEPRECATED,
						"Non-static method %s::%s() should not be called statically",
						ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
					if (UNEXPECTED(EG(exception) != NULL)) {
						HANDLE_EXCEPTION();
					}
				} else {
					zend_throw_error(
						zend_ce_error,
						"Non-static method %s::%s() cannot be called statically",
						ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
			}
		} else {
			if (Z_STRVAL_P(function_name)[0] == '\\') {
				lcname = zend_string_alloc(Z_STRLEN_P(function_name) - 1, 0);
				zend_str_tolower_copy(ZSTR_VAL(lcname), Z_STRVAL_P(function_name) + 1, Z_STRLEN_P(function_name) - 1);
			} else {
				lcname = zend_string_tolower(Z_STR_P(function_name));
			}
			if (UNEXPECTED((func = zend_hash_find(EG(function_table), lcname)) == NULL)) {
				zend_throw_error(NULL, "Call to undefined function %s()", Z_STRVAL_P(function_name));
				zend_string_release(lcname);
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
			zend_string_release(lcname);

			fbc = Z_FUNC_P(func);
			called_scope = NULL;
			object = NULL;
		}
	} else if (OP2_TYPE != IS_CONST &&
	    EXPECTED(Z_TYPE_P(function_name) == IS_OBJECT) &&
		Z_OBJ_HANDLER_P(function_name, get_closure) &&
		Z_OBJ_HANDLER_P(function_name, get_closure)(function_name, &called_scope, &fbc, &object) == SUCCESS) {
		if (fbc->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			ZEND_ASSERT(GC_TYPE((zend_object*)fbc->common.prototype) == IS_OBJECT);
			GC_REFCOUNT((zend_object*)fbc->common.prototype)++;
			call_info |= ZEND_CALL_CLOSURE;
		} else if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS;
			GC_REFCOUNT(object)++; /* For $this pointer */
		}
	} else if (EXPECTED(Z_TYPE_P(function_name) == IS_ARRAY) &&
			zend_hash_num_elements(Z_ARRVAL_P(function_name)) == 2) {
		zval *obj;
		zval *method;
		obj = zend_hash_index_find(Z_ARRVAL_P(function_name), 0);
		method = zend_hash_index_find(Z_ARRVAL_P(function_name), 1);

		if (!obj || !method) {
			zend_throw_error(NULL, "Array callback has to contain indices 0 and 1");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}

		ZVAL_DEREF(obj);
		if (Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) {
			zend_throw_error(NULL, "First array member is not a valid class name or object");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}

		ZVAL_DEREF(method);
		if (Z_TYPE_P(method) != IS_STRING) {
			zend_throw_error(NULL, "Second array member is not a valid method");
			FREE_OP2();
			HANDLE_EXCEPTION();
		}

		if (Z_TYPE_P(obj) == IS_STRING) {
			object = NULL;
			called_scope = zend_fetch_class_by_name(Z_STR_P(obj), NULL, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
			if (UNEXPECTED(called_scope == NULL)) {
				ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
			}

			if (called_scope->get_static_method) {
				fbc = called_scope->get_static_method(called_scope, Z_STR_P(method));
			} else {
				fbc = zend_std_get_static_method(called_scope, Z_STR_P(method), NULL);
			}
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(called_scope->name), Z_STRVAL_P(method));
				}
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
			if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
				if (fbc->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					zend_error(E_DEPRECATED,
						"Non-static method %s::%s() should not be called statically",
						ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
					if (UNEXPECTED(EG(exception) != NULL)) {
						HANDLE_EXCEPTION();
					}
				} else {
					zend_throw_error(
						zend_ce_error,
						"Non-static method %s::%s() cannot be called statically",
						ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
					FREE_OP2();
					HANDLE_EXCEPTION();
				}
			}
		} else {
			called_scope = Z_OBJCE_P(obj);
			object = Z_OBJ_P(obj);

			fbc = Z_OBJ_HT_P(obj)->get_method(&object, Z_STR_P(method), NULL);
			if (UNEXPECTED(fbc == NULL)) {
				if (EXPECTED(!EG(exception))) {
					zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(object->ce->name), Z_STRVAL_P(method));
				}
				FREE_OP2();
				HANDLE_EXCEPTION();
			}

			if ((fbc->common.fn_flags & ZEND_ACC_STATIC) != 0) {
				object = NULL;
			} else {
				call_info |= ZEND_CALL_RELEASE_THIS;
				GC_REFCOUNT(object)++; /* For $this pointer */
			}
		}
	} else if ((OP2_TYPE & (IS_VAR|IS_CV)) && Z_TYPE_P(function_name) == IS_REFERENCE) {
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
		FREE_OP2();
		HANDLE_EXCEPTION();
	}

	FREE_OP2();
	if ((OP2_TYPE & (IS_VAR|IS_TMP_VAR)) && UNEXPECTED(EG(exception))) {
		if (call_info & ZEND_CALL_RELEASE_THIS) {
			zend_object_release(object);
		}
		if (fbc->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
			zend_string_release(fbc->common.function_name);
			zend_free_trampoline(fbc);
		}
		HANDLE_EXCEPTION();
	}
	call = zend_vm_stack_push_call_frame(call_info,
		fbc, opline->extended_value, called_scope, object);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(118, ZEND_INIT_USER_CALL, CONST, CONST|TMPVAR|CV)
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
	uint32_t call_info = ZEND_CALL_NESTED_FUNCTION;

	SAVE_OPLINE();
	function_name = GET_OP2_ZVAL_PTR(BP_VAR_R);
	if (zend_is_callable_ex(function_name, NULL, 0, NULL, &fcc, &error)) {
		func = fcc.function_handler;
		called_scope = fcc.called_scope;
		object = fcc.object;
		if (error) {
			efree(error);
			/* This is the only soft error is_callable() can generate */
			zend_error(E_DEPRECATED,
				"Non-static method %s::%s() should not be called statically",
				ZSTR_VAL(func->common.scope->name), ZSTR_VAL(func->common.function_name));
			if (UNEXPECTED(EG(exception) != NULL)) {
				FREE_OP2();
				HANDLE_EXCEPTION();
			}
		}
		if (func->common.fn_flags & ZEND_ACC_CLOSURE) {
			/* Delay closure destruction until its invocation */
			if (OP2_TYPE & (IS_VAR|IS_CV)) {
				ZVAL_DEREF(function_name);
			}
			ZEND_ASSERT(GC_TYPE((zend_object*)func->common.prototype) == IS_OBJECT);
			GC_REFCOUNT((zend_object*)func->common.prototype)++;
			call_info |= ZEND_CALL_CLOSURE;
		}
		if (object) {
			call_info |= ZEND_CALL_RELEASE_THIS;
			GC_REFCOUNT(object)++; /* For $this pointer */
		}

		FREE_OP2();
		if ((OP2_TYPE & (IS_TMP_VAR|IS_VAR)) && UNEXPECTED(EG(exception))) {
			if (call_info & ZEND_CALL_CLOSURE) {
				zend_object_release((zend_object*)func->common.prototype);
			}
			if (call_info & ZEND_CALL_RELEASE_THIS) {
				zend_object_release(object);
			}
			HANDLE_EXCEPTION();
		}
	} else {
		zend_internal_type_error(EX_USES_STRICT_TYPES(), "%s() expects parameter 1 to be a valid callback, %s", Z_STRVAL_P(EX_CONSTANT(opline->op1)), error);
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

ZEND_VM_HANDLER(69, ZEND_INIT_NS_FCALL_BY_NAME, ANY, CONST)
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
	}

	call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(61, ZEND_INIT_FCALL, ANY, CONST)
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
	}

	call = zend_vm_stack_push_call_frame_ex(
		opline->op1.num, ZEND_CALL_NESTED_FUNCTION,
		fbc, opline->extended_value, NULL, NULL);
	call->prev_execute_data = EX(call);
	EX(call) = call;

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(129, ZEND_DO_ICALL, ANY, ANY)
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	call->prev_execute_data = execute_data;
	EG(current_execute_data) = call;

	ret = EX_VAR(opline->result.var);
	ZVAL_NULL(ret);
	Z_VAR_FLAGS_P(ret) = 0;

	fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
	ZEND_ASSERT(
		EG(exception) || !call->func ||
		!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
		zend_verify_internal_return_type(call->func, EX_VAR(opline->result.var)));
#endif

	EG(current_execute_data) = call->prev_execute_data;
	zend_vm_stack_free_args(call);
	zend_vm_stack_free_call_frame(call);

	if (!RETURN_VALUE_USED(opline)) {
		zval_ptr_dtor(EX_VAR(opline->result.var));
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}

	ZEND_VM_INTERRUPT_CHECK();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(130, ZEND_DO_UCALL, ANY, ANY)
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	EG(scope) = NULL;
	ret = NULL;
	call->symbol_table = NULL;
	if (RETURN_VALUE_USED(opline)) {
		ret = EX_VAR(opline->result.var);
		ZVAL_NULL(ret);
		Z_VAR_FLAGS_P(ret) = 0;
	}

	call->prev_execute_data = execute_data;
	i_init_func_execute_data(call, &fbc->op_array, ret, 0);

	ZEND_VM_ENTER();
}

ZEND_VM_HANDLER(131, ZEND_DO_FCALL_BY_NAME, ANY, ANY)
{
	USE_OPLINE
	zend_execute_data *call = EX(call);
	zend_function *fbc = call->func;
	zval *ret;

	SAVE_OPLINE();
	EX(call) = call->prev_execute_data;

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION)) {
		EG(scope) = NULL;
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			if (EXPECTED(RETURN_VALUE_USED(opline))) {
				ret = EX_VAR(opline->result.var);
				zend_generator_create_zval(call, &fbc->op_array, ret);
				Z_VAR_FLAGS_P(ret) = 0;
			} else {
				zend_vm_stack_free_args(call);
			}

			zend_vm_stack_free_call_frame(call);
		} else {
			ret = NULL;
			call->symbol_table = NULL;
			if (RETURN_VALUE_USED(opline)) {
				ret = EX_VAR(opline->result.var);
				ZVAL_NULL(ret);
				Z_VAR_FLAGS_P(ret) = 0;
			}

			call->prev_execute_data = execute_data;
			i_init_func_execute_data(call, &fbc->op_array, ret, 0);

			ZEND_VM_ENTER();
		}
		EG(scope) = EX(func)->op_array.scope;
	} else {
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

		if (fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
			uint32_t i;
			uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
			zval *p = ZEND_CALL_ARG(call, 1);

			for (i = 0; i < num_args; ++i) {
				if (UNEXPECTED(!zend_verify_internal_arg_type(fbc, i + 1, p))) {
					EG(current_execute_data) = call->prev_execute_data;
					zend_vm_stack_free_args(call);
					zend_vm_stack_free_call_frame(call);
					zend_throw_exception_internal(NULL);
					HANDLE_EXCEPTION();
				}
				p++;
			}
		}

		ret = EX_VAR(opline->result.var);
		ZVAL_NULL(ret);
		Z_VAR_FLAGS_P(ret) = (fbc->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0 ? IS_VAR_RET_REF : 0;

		fbc->internal_function.handler(call, ret);

#if ZEND_DEBUG
		ZEND_ASSERT(
			EG(exception) || !call->func ||
			!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
			zend_verify_internal_return_type(call->func, EX_VAR(opline->result.var)));
#endif

		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);
		zend_vm_stack_free_call_frame(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}
	ZEND_VM_INTERRUPT_CHECK();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(60, ZEND_DO_FCALL, ANY, ANY)
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
		EG(scope) = fbc->common.scope;
		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			if (EXPECTED(RETURN_VALUE_USED(opline))) {
				ret = EX_VAR(opline->result.var);
				zend_generator_create_zval(call, &fbc->op_array, ret);
				Z_VAR_FLAGS_P(ret) = 0;
			} else {
				if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_CLOSURE)) {
					OBJ_RELEASE((zend_object*)fbc->op_array.prototype);
				}
				zend_vm_stack_free_args(call);
			}
		} else {
			ret = NULL;
			call->symbol_table = NULL;
			if (RETURN_VALUE_USED(opline)) {
				ret = EX_VAR(opline->result.var);
				ZVAL_NULL(ret);
				Z_VAR_FLAGS_P(ret) = 0;
			}

			call->prev_execute_data = execute_data;
			i_init_func_execute_data(call, &fbc->op_array, ret, 1);

			if (EXPECTED(zend_execute_ex == execute_ex)) {
				ZEND_VM_ENTER();
			} else {
				ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
				zend_execute_ex(call);
			}
		}
	} else if (EXPECTED(fbc->type < ZEND_USER_FUNCTION)) {
		int should_change_scope = 0;

		if (fbc->common.scope) {
			should_change_scope = 1;
			EG(scope) = fbc->common.scope;
		}

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;

		if (fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
			uint32_t i;
			uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
			zval *p = ZEND_CALL_ARG(call, 1);

			for (i = 0; i < num_args; ++i) {
				if (UNEXPECTED(!zend_verify_internal_arg_type(fbc, i + 1, p))) {
					EG(current_execute_data) = call->prev_execute_data;
					zend_vm_stack_free_args(call);
					if (RETURN_VALUE_USED(opline)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					}
					if (UNEXPECTED(should_change_scope)) {
						ZEND_VM_C_GOTO(fcall_end_change_scope);
					} else {
						ZEND_VM_C_GOTO(fcall_end);
					}
				}
				p++;
			}
		}

		ret = EX_VAR(opline->result.var);
		ZVAL_NULL(ret);
		Z_VAR_FLAGS_P(ret) = (fbc->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0 ? IS_VAR_RET_REF : 0;

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
			zend_verify_internal_return_type(call->func, EX_VAR(opline->result.var)));
#endif

		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}

		if (UNEXPECTED(should_change_scope)) {
			ZEND_VM_C_GOTO(fcall_end_change_scope);
		} else {
			ZEND_VM_C_GOTO(fcall_end);
		}
	} else { /* ZEND_OVERLOADED_FUNCTION */
		/* Not sure what should be done here if it's a static method */
		object = Z_OBJ(call->This);
		if (UNEXPECTED(object == NULL)) {
			zend_vm_stack_free_args(call);
			if (fbc->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
				zend_string_release(fbc->common.function_name);
			}
			efree(fbc);
			zend_vm_stack_free_call_frame(call);

			zend_throw_error(NULL, "Cannot call overloaded function for non-object");
			HANDLE_EXCEPTION();
		}

		EG(scope) = fbc->common.scope;

		ZVAL_NULL(EX_VAR(opline->result.var));

		call->prev_execute_data = execute_data;
		EG(current_execute_data) = call;
		object->handlers->call_method(fbc->common.function_name, object, call, EX_VAR(opline->result.var));
		EG(current_execute_data) = call->prev_execute_data;

		zend_vm_stack_free_args(call);

		if (fbc->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
			zend_string_release(fbc->common.function_name);
		}
		efree(fbc);

		if (!RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		} else {
			Z_VAR_FLAGS_P(EX_VAR(opline->result.var)) = 0;
		}
	}

ZEND_VM_C_LABEL(fcall_end_change_scope):
	if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS)) {
		object = Z_OBJ(call->This);
#if 0
		if (UNEXPECTED(EG(exception) != NULL) && (opline->op1.num & ZEND_CALL_CTOR)) {
			if (!(opline->op1.num & ZEND_CALL_CTOR_RESULT_UNUSED)) {
#else
		if (UNEXPECTED(EG(exception) != NULL) && (ZEND_CALL_INFO(call) & ZEND_CALL_CTOR)) {
			if (!(ZEND_CALL_INFO(call) & ZEND_CALL_CTOR_RESULT_UNUSED)) {
#endif
				GC_REFCOUNT(object)--;
			}
			if (GC_REFCOUNT(object) == 1) {
				zend_object_store_ctor_failed(object);
			}
		}
		OBJ_RELEASE(object);
	}
	EG(scope) = EX(func)->op_array.scope;

ZEND_VM_C_LABEL(fcall_end):
	zend_vm_stack_free_call_frame(call);
	if (UNEXPECTED(EG(exception) != NULL)) {
		zend_throw_exception_internal(NULL);
		if (RETURN_VALUE_USED(opline)) {
			zval_ptr_dtor(EX_VAR(opline->result.var));
		}
		HANDLE_EXCEPTION();
	}

	ZEND_VM_INTERRUPT_CHECK();
	ZEND_VM_NEXT_OPCODE();
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
			} else {
				FREE_OP1();
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
	zend_free_op free_op1;

	retval_ptr = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);
	if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_INFO_P(retval_ptr) == IS_UNDEF)) {
		SAVE_OPLINE();
		retval_ptr = GET_OP1_UNDEF_CV(retval_ptr, BP_VAR_R);
		if (EX(return_value)) {
			ZVAL_NULL(EX(return_value));
		}
	} else if (!EX(return_value)) {
		if (OP1_TYPE == IS_VAR || OP1_TYPE == IS_TMP_VAR ) {
			if (Z_REFCOUNTED_P(free_op1) && !Z_DELREF_P(free_op1)) {
				SAVE_OPLINE();
				zval_dtor_func_for_ptr(Z_COUNTED_P(free_op1));
			}
		}
	} else {
		if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
			ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
			if (OP1_TYPE == IS_CONST) {
				if (UNEXPECTED(Z_OPT_COPYABLE_P(EX(return_value)))) {
					zval_copy_ctor_func(EX(return_value));
				}
			}
		} else if (OP1_TYPE == IS_CV) {
			ZVAL_DEREF(retval_ptr);
			ZVAL_COPY(EX(return_value), retval_ptr);
		} else /* if (OP1_TYPE == IS_VAR) */ {
			if (UNEXPECTED(Z_ISREF_P(retval_ptr))) {
				zend_refcounted *ref = Z_COUNTED_P(retval_ptr);

				retval_ptr = Z_REFVAL_P(retval_ptr);
				ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
				if (UNEXPECTED(--GC_REFCOUNT(ref) == 0)) {
					efree_size(ref, sizeof(zend_reference));
				} else if (Z_OPT_REFCOUNTED_P(retval_ptr)) {
					Z_ADDREF_P(retval_ptr);
				}
			} else {
				ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
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
			} else {
				ZVAL_COPY_VALUE(EX(return_value), retval_ptr);
				Z_VAR_FLAGS_P(EX(return_value)) = IS_VAR_RET_REF;
				if (OP1_TYPE != IS_TMP_VAR) {
					zval_opt_copy_ctor_no_imm(EX(return_value));
				}
			}
			break;
		}

		retval_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

		if (OP1_TYPE == IS_VAR && UNEXPECTED(retval_ptr == NULL)) {
			zend_throw_error(NULL, "Cannot return string offsets by reference");
			HANDLE_EXCEPTION();
		}

		if (OP1_TYPE == IS_VAR) {
			if (retval_ptr == &EG(uninitialized_zval) ||
			    (opline->extended_value == ZEND_RETURNS_FUNCTION &&
			     !(Z_VAR_FLAGS_P(retval_ptr) & IS_VAR_RET_REF))) {
				zend_error(E_NOTICE, "Only variable references should be returned by reference");
				if (EX(return_value)) {
					ZVAL_NEW_REF(EX(return_value), retval_ptr);
					Z_VAR_FLAGS_P(EX(return_value)) = IS_VAR_RET_REF;
					if (Z_REFCOUNTED_P(retval_ptr)) Z_ADDREF_P(retval_ptr);
				}
				break;
			}
		}

		if (EX(return_value)) {
			ZVAL_MAKE_REF(retval_ptr);
			Z_ADDREF_P(retval_ptr);
			ZVAL_REF(EX(return_value), Z_REF_P(retval_ptr));
			Z_VAR_FLAGS_P(EX(return_value)) = IS_VAR_RET_REF;
		}
	} while (0);

	FREE_OP1_VAR_PTR();
	ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
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
	if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
		ZVAL_COPY_VALUE(&generator->retval, retval);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_COPYABLE(generator->retval))) {
				zval_copy_ctor_func(&generator->retval);
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

ZEND_VM_HANDLER(107, ZEND_CATCH, CONST, CV)
{
	USE_OPLINE
	zend_class_entry *ce, *catch_ce;
	zend_object *exception;
	zval *ex;

	SAVE_OPLINE();
	/* Check whether an exception has been thrown, if not, jump over code */
	zend_exception_restore();
	if (EG(exception) == NULL) {
		ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[opline->extended_value]);
		ZEND_VM_CONTINUE(); /* CHECK_ME */
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
			ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[opline->extended_value]);
			ZEND_VM_CONTINUE(); /* CHECK_ME */
		}
	}

	exception = EG(exception);
	ex = EX_VAR(opline->op2.var);
	if (UNEXPECTED(Z_ISREF_P(ex))) {
		ex = Z_REFVAL_P(ex);
	}
	zval_ptr_dtor(ex);
	ZVAL_OBJ(ex, EG(exception));
	if (UNEXPECTED(EG(exception) != exception)) {
		GC_REFCOUNT(EG(exception))++;
		HANDLE_EXCEPTION();
	} else {
		EG(exception) = NULL;
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(65, ZEND_SEND_VAL, CONST|TMP, ANY)
{
	USE_OPLINE
	zval *value, *arg;
	zend_free_op free_op1;

	value = GET_OP1_ZVAL_PTR(BP_VAR_R);
	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, value);
	if (OP1_TYPE == IS_CONST) {
		if (UNEXPECTED(Z_OPT_COPYABLE_P(arg))) {
			zval_copy_ctor_func(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(116, ZEND_SEND_VAL_EX, CONST|TMP, ANY)
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
		if (UNEXPECTED(Z_OPT_COPYABLE_P(arg))) {
			zval_copy_ctor_func(arg);
		}
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(117, ZEND_SEND_VAR, VAR|CV, ANY)
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

ZEND_VM_HANDLER(106, ZEND_SEND_VAR_NO_REF, VAR, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr, *arg;

	if (!(opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND)) {
		if (!ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, opline->op2.num)) {
			ZEND_VM_DISPATCH_TO_HANDLER(ZEND_SEND_VAR);
		}
	}

	varptr = GET_OP1_ZVAL_PTR(BP_VAR_R);

	if (EXPECTED(Z_ISREF_P(varptr) ||
	    ((opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) ?
	     (opline->extended_value & ZEND_ARG_SEND_SILENT) :
	     ARG_MAY_BE_SENT_BY_REF(EX(call)->func, opline->op2.num
	    )))) {
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_COPY_VALUE(arg, varptr);

		ZEND_VM_NEXT_OPCODE();
	}

	SAVE_OPLINE();
	zend_error(E_NOTICE, "Only variables should be passed by reference");

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	ZVAL_COPY_VALUE(arg, varptr);

	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(67, ZEND_SEND_REF, VAR|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *varptr, *arg;

	SAVE_OPLINE();
	varptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

	if (OP1_TYPE == IS_VAR && UNEXPECTED(varptr == NULL)) {
		zend_throw_error(NULL, "Only variables can be passed by reference");
		arg = ZEND_CALL_VAR(EX(call), opline->result.var);
		ZVAL_UNDEF(arg);
		HANDLE_EXCEPTION();
	}

	arg = ZEND_CALL_VAR(EX(call), opline->result.var);
	if (OP1_TYPE == IS_VAR && UNEXPECTED(varptr == &EG(error_zval))) {
		ZVAL_NEW_REF(arg, &EG(uninitialized_zval));
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

ZEND_VM_HANDLER(66, ZEND_SEND_VAR_EX, VAR|CV, ANY)
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
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}
			}

			for (; iter->funcs->valid(iter) == SUCCESS; ++arg_num) {
				zval *arg, *top;

				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}

				arg = iter->funcs->get_current_data(iter);
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}

				if (iter->funcs->get_current_key) {
					zval key;
					iter->funcs->get_current_key(iter, &key);
					if (UNEXPECTED(EG(exception) != NULL)) {
						ZEND_VM_C_GOTO(unpack_iter_dtor);
					}

					if (Z_TYPE(key) == IS_STRING) {
						zend_throw_error(NULL,
							"Cannot unpack Traversable with string keys");
						zend_string_release(Z_STR(key));
						ZEND_VM_C_GOTO(unpack_iter_dtor);
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
				if (UNEXPECTED(EG(exception) != NULL)) {
					ZEND_VM_C_GOTO(unpack_iter_dtor);
				}
			}

ZEND_VM_C_LABEL(unpack_iter_dtor):
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
		if (Z_OBJ(EX(call)->This)) {
			OBJ_RELEASE(Z_OBJ(EX(call)->This));
		}
		EX(call)->func = (zend_function*)&zend_pass_function;
		EX(call)->called_scope = NULL;
		Z_OBJ(EX(call)->This) = NULL;
		ZEND_SET_CALL_INFO(EX(call), ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);
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
						if (Z_OBJ(EX(call)->This)) {
							OBJ_RELEASE(Z_OBJ(EX(call)->This));
						}
						EX(call)->func = (zend_function*)&zend_pass_function;
						EX(call)->called_scope = NULL;
						Z_OBJ(EX(call)->This) = NULL;
						ZEND_SET_CALL_INFO(EX(call), ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);
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

ZEND_VM_HANDLER(120, ZEND_SEND_USER, VAR|CV, ANY)
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
				if (Z_OBJ(EX(call)->This)) {
					OBJ_RELEASE(Z_OBJ(EX(call)->This));
				}
				ZVAL_UNDEF(param);
				EX(call)->func = (zend_function*)&zend_pass_function;
				EX(call)->called_scope = NULL;
				Z_OBJ(EX(call)->This) = NULL;
				ZEND_SET_CALL_INFO(EX(call), ZEND_CALL_INFO(EX(call)) & ~ZEND_CALL_RELEASE_THIS);

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

ZEND_VM_HANDLER(63, ZEND_RECV, ANY, ANY)
{
	USE_OPLINE
	uint32_t arg_num = opline->op1.num;

	if (UNEXPECTED(arg_num > EX_NUM_ARGS())) {
		SAVE_OPLINE();
		zend_verify_missing_arg(execute_data, arg_num, CACHE_ADDR(opline->op2.num));
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	} else if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0)) {
		zval *param = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->result.var);

		SAVE_OPLINE();
		if (UNEXPECTED(!zend_verify_arg_type(EX(func), arg_num, param, NULL, CACHE_ADDR(opline->op2.num)) || EG(exception))) {
			HANDLE_EXCEPTION();
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(64, ZEND_RECV_INIT, ANY, CONST)
{
	USE_OPLINE
	uint32_t arg_num;
	zval *param;

	ZEND_VM_REPEATABLE_OPCODE

	arg_num = opline->op1.num;
	param = _get_zval_ptr_cv_undef_BP_VAR_W(execute_data, opline->result.var);
	if (arg_num > EX_NUM_ARGS()) {
		ZVAL_COPY_VALUE(param, EX_CONSTANT(opline->op2));
		if (Z_OPT_CONSTANT_P(param)) {
			SAVE_OPLINE();
			if (UNEXPECTED(zval_update_constant_ex(param, 0, NULL) != SUCCESS)) {
				ZVAL_UNDEF(param);
				HANDLE_EXCEPTION();
			}
		} else {
			/* IS_CONST can't be IS_OBJECT, IS_RESOURCE or IS_REFERENCE */
			if (UNEXPECTED(Z_OPT_COPYABLE_P(param))) {
				zval_copy_ctor_func(param);
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

ZEND_VM_HANDLER(164, ZEND_RECV_VARIADIC, ANY, ANY)
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
		if (UNEXPECTED(Z_TYPE_INFO_P(val) == IS_UNDEF)) {
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

ZEND_VM_HANDLER(68, ZEND_NEW, CONST|VAR, ANY)
{
	USE_OPLINE
	zval object_zval;
	zend_function *constructor;
	zend_class_entry *ce;

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
	} else {
		ce = Z_CE_P(EX_VAR(opline->op1.var));
	}
	if (UNEXPECTED(object_init_ex(&object_zval, ce) != SUCCESS)) {
		HANDLE_EXCEPTION();
	}
	constructor = Z_OBJ_HT(object_zval)->get_constructor(Z_OBJ(object_zval));

	if (constructor == NULL) {
		if (EXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY_VALUE(EX_VAR(opline->result.var), &object_zval);
		} else {
			OBJ_RELEASE(Z_OBJ(object_zval));
		}
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	} else {
		/* We are not handling overloaded classes right now */
		zend_execute_data *call = zend_vm_stack_push_call_frame(
				ZEND_CALL_FUNCTION | ZEND_CALL_RELEASE_THIS | ZEND_CALL_CTOR |
				(EXPECTED(RETURN_VALUE_USED(opline)) ? 0 : ZEND_CALL_CTOR_RESULT_UNUSED),
			constructor,
			opline->extended_value,
			ce,
			Z_OBJ(object_zval));
		call->prev_execute_data = EX(call);
		EX(call) = call;

		if (EXPECTED(RETURN_VALUE_USED(opline))) {
			ZVAL_COPY(EX_VAR(opline->result.var), &object_zval);
		}

		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(110, ZEND_CLONE, CONST|TMPVAR|UNUSED|CV, ANY)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *obj;
	zend_class_entry *ce;
	zend_function *clone;
	zend_object_clone_obj_t clone_call;

	SAVE_OPLINE();
	obj = GET_OP1_OBJ_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(obj) == NULL)) {
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
	clone = ce ? ce->clone : NULL;
	clone_call =  Z_OBJ_HT_P(obj)->clone_obj;
	if (UNEXPECTED(clone_call == NULL)) {
		if (ce) {
			zend_throw_error(NULL, "Trying to clone an uncloneable object of class %s", ZSTR_VAL(ce->name));
		} else {
			zend_throw_error(NULL, "Trying to clone an uncloneable object");
		}
		FREE_OP1();
		HANDLE_EXCEPTION();
	}

	if (ce && clone) {
		if (clone->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (UNEXPECTED(ce != EG(scope))) {
				zend_throw_error(NULL, "Call to private %s::__clone() from context '%s'", ZSTR_VAL(ce->name), EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		} else if ((clone->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (UNEXPECTED(!zend_check_protected(zend_get_function_root_class(clone), EG(scope)))) {
				zend_throw_error(NULL, "Call to protected %s::__clone() from context '%s'", ZSTR_VAL(ce->name), EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
				FREE_OP1();
				HANDLE_EXCEPTION();
			}
		}
	}

	if (EXPECTED(EG(exception) == NULL)) {
		ZVAL_OBJ(EX_VAR(opline->result.var), clone_call(obj));
		if (UNEXPECTED(!RETURN_VALUE_USED(opline)) || UNEXPECTED(EG(exception) != NULL)) {
			OBJ_RELEASE(Z_OBJ_P(EX_VAR(opline->result.var)));
		}
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(99, ZEND_FETCH_CONSTANT, VAR|CONST|UNUSED, CONST)
{
	USE_OPLINE

	SAVE_OPLINE();
	if (OP1_TYPE == IS_UNUSED) {
		zend_constant *c;

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
	} else {
		/* class constant */
		zend_class_entry *ce;
		zval *value;

		do {
			if (OP1_TYPE == IS_CONST) {
				if (EXPECTED(CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2))))) {
					value = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)));
					ZVAL_DEREF(value);
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
				ce = Z_CE_P(EX_VAR(opline->op1.var));
				if ((value = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce)) != NULL) {
					ZVAL_DEREF(value);
					break;
				}
			}

			if (EXPECTED((value = zend_hash_find(&ce->constants_table, Z_STR_P(EX_CONSTANT(opline->op2)))) != NULL)) {
				ZVAL_DEREF(value);
				if (Z_CONSTANT_P(value)) {
					EG(scope) = ce;
					zval_update_constant_ex(value, 1, NULL);
					EG(scope) = EX(func)->op_array.scope;
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
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(72, ZEND_ADD_ARRAY_ELEMENT, CONST|TMP|VAR|CV, CONST|TMPVAR|UNUSED|CV)
{
	USE_OPLINE
	zend_free_op free_op1;
	zval *expr_ptr, new_expr;

	SAVE_OPLINE();
	if ((OP1_TYPE == IS_VAR || OP1_TYPE == IS_CV) &&
	    UNEXPECTED(opline->extended_value & ZEND_ARRAY_ELEMENT_REF)) {
		expr_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);
		if (OP1_TYPE == IS_VAR && UNEXPECTED(expr_ptr == NULL)) {
			zend_throw_error(NULL, "Cannot create references to/from string offsets");
			zend_array_destroy(Z_ARRVAL_P(EX_VAR(opline->result.var)));
			HANDLE_EXCEPTION();
		}
		ZVAL_MAKE_REF(expr_ptr);
		Z_ADDREF_P(expr_ptr);
		FREE_OP1_VAR_PTR();
	} else {
		expr_ptr = GET_OP1_ZVAL_PTR(BP_VAR_R);
		if (OP1_TYPE == IS_TMP_VAR) {
			/* pass */
		} else if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_COPYABLE_P(expr_ptr))) {
				ZVAL_COPY_VALUE(&new_expr, expr_ptr);
				zval_copy_ctor_func(&new_expr);
				expr_ptr = &new_expr;
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
		if (!zend_hash_next_index_insert(Z_ARRVAL_P(EX_VAR(opline->result.var)), expr_ptr)) {
			zend_error(E_WARNING, "Cannot add element to the array as the next element is already occupied");
			zval_ptr_dtor(expr_ptr);
		}
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(71, ZEND_INIT_ARRAY, CONST|TMP|VAR|UNUSED|CV, CONST|TMPVAR|UNUSED|CV)
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

ZEND_VM_HANDLER(21, ZEND_CAST, CONST|TMP|VAR|CV, ANY)
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
					if (UNEXPECTED(Z_OPT_COPYABLE_P(result))) {
						zval_copy_ctor_func(result);
					}
				} else if (OP1_TYPE != IS_TMP_VAR) {
					if (Z_OPT_REFCOUNTED_P(expr)) Z_ADDREF_P(expr);
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
							if (UNEXPECTED(Z_OPT_COPYABLE_P(expr))) {
								zval_copy_ctor_func(expr);
							}
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
						expr = zend_hash_str_add_new(Z_OBJPROP_P(result), "scalar", sizeof("scalar")-1, expr);
						if (OP1_TYPE == IS_CONST) {
							if (UNEXPECTED(Z_OPT_COPYABLE_P(expr))) {
								zval_copy_ctor_func(expr);
							}
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

ZEND_VM_HANDLER(73, ZEND_INCLUDE_OR_EVAL, CONST|TMPVAR|CV, ANY)
{
	USE_OPLINE
	zend_op_array *new_op_array=NULL;
	zend_free_op free_op1;
	zval *inc_filename;
	zval tmp_inc_filename;
	zend_bool failure_retval=0;

	SAVE_OPLINE();
	inc_filename = GET_OP1_ZVAL_PTR_UNDEF(BP_VAR_R);

	ZVAL_UNDEF(&tmp_inc_filename);
	if (Z_TYPE_P(inc_filename) != IS_STRING) {
		if (OP1_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(inc_filename) == IS_UNDEF)) {
			inc_filename = GET_OP1_UNDEF_CV(inc_filename, BP_VAR_R);
		}
		ZVAL_STR(&tmp_inc_filename, zval_get_string(inc_filename));
		inc_filename = &tmp_inc_filename;
	}

	if (opline->extended_value != ZEND_EVAL && strlen(Z_STRVAL_P(inc_filename)) != Z_STRLEN_P(inc_filename)) {
		if (opline->extended_value == ZEND_INCLUDE_ONCE || opline->extended_value == ZEND_INCLUDE) {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
		} else {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
		}
	} else {
		switch (opline->extended_value) {
			case ZEND_INCLUDE_ONCE:
			case ZEND_REQUIRE_ONCE: {
					zend_file_handle file_handle;
					zend_string *resolved_path;

					resolved_path = zend_resolve_path(Z_STRVAL_P(inc_filename), (int)Z_STRLEN_P(inc_filename));
					if (resolved_path) {
						failure_retval = zend_hash_exists(&EG(included_files), resolved_path);
					} else {
						resolved_path = zend_string_copy(Z_STR_P(inc_filename));
					}

					if (failure_retval) {
						/* do nothing, file already included */
					} else if (SUCCESS == zend_stream_open(ZSTR_VAL(resolved_path), &file_handle)) {

						if (!file_handle.opened_path) {
							file_handle.opened_path = zend_string_copy(resolved_path);
						}

						if (zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path)) {
							new_op_array = zend_compile_file(&file_handle, (opline->extended_value==ZEND_INCLUDE_ONCE?ZEND_INCLUDE:ZEND_REQUIRE));
							zend_destroy_file_handle(&file_handle);
						} else {
							zend_file_handle_dtor(&file_handle);
							failure_retval=1;
						}
					} else {
						if (opline->extended_value == ZEND_INCLUDE_ONCE) {
							zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, Z_STRVAL_P(inc_filename));
						} else {
							zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, Z_STRVAL_P(inc_filename));
						}
					}
					zend_string_release(resolved_path);
				}
				break;
			case ZEND_INCLUDE:
			case ZEND_REQUIRE:
				new_op_array = compile_filename(opline->extended_value, inc_filename);
				break;
			case ZEND_EVAL: {
					char *eval_desc = zend_make_compiled_string_description("eval()'d code");

					new_op_array = zend_compile_string(inc_filename, eval_desc);
					efree(eval_desc);
				}
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	}
	if (Z_TYPE(tmp_inc_filename) != IS_UNDEF) {
		zend_string_release(Z_STR(tmp_inc_filename));
	}
	FREE_OP1();
	if (UNEXPECTED(EG(exception) != NULL)) {
		if (new_op_array != NULL) {
			destroy_op_array(new_op_array);
			efree_size(new_op_array, sizeof(zend_op_array));
		}
		HANDLE_EXCEPTION();
	} else if (EXPECTED(new_op_array != NULL)) {
		zval *return_value = NULL;
		zend_execute_data *call;

		if (RETURN_VALUE_USED(opline)) {
			return_value = EX_VAR(opline->result.var);
		}

		new_op_array->scope = EG(scope);

		call = zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_CODE,
			(zend_function*)new_op_array, 0, EX(called_scope), Z_OBJ(EX(This)));

		if (EX(symbol_table)) {
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
		ZVAL_BOOL(EX_VAR(opline->result.var), failure_retval);
	}
	ZEND_VM_INTERRUPT_CHECK();
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(74, ZEND_UNSET_VAR, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE
	zval tmp, *varname;
	HashTable *target_symbol_table;
	zend_free_op free_op1;

	SAVE_OPLINE();
	if (OP1_TYPE == IS_CV &&
	    OP2_TYPE == IS_UNUSED &&
	    (opline->extended_value & ZEND_QUICK_SET)) {
		zval *var = EX_VAR(opline->op1.var);

		if (Z_REFCOUNTED_P(var)) {
			zend_refcounted *garbage = Z_COUNTED_P(var);

			if (!--GC_REFCOUNT(garbage)) {
				ZVAL_UNDEF(var);
				zval_dtor_func_for_ptr(garbage);
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

	if (OP2_TYPE != IS_UNUSED) {
		zend_class_entry *ce;

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
		} else {
			ce = Z_CE_P(EX_VAR(opline->op2.var));
		}
		zend_std_unset_static_property(ce, Z_STR_P(varname));
	} else {
		target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
		zend_hash_del_ind(target_symbol_table, Z_STR_P(varname));
	}

	if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
		zend_string_release(Z_STR(tmp));
	}
	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(75, ZEND_UNSET_DIM, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;
	zend_ulong hval;
	zend_string *key;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot unset string offsets");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}
	offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	do {
		if (OP1_TYPE != IS_UNUSED && EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
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
		} else if (OP1_TYPE != IS_UNUSED && Z_ISREF_P(container)) {
			container = Z_REFVAL_P(container);
			if (EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
				ZEND_VM_C_GOTO(unset_dim_array);
			}
		}
		if (OP2_TYPE == IS_CV && UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
			offset = GET_OP2_UNDEF_CV(offset, BP_VAR_R);
		}
		if (OP1_TYPE == IS_UNUSED || EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
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

ZEND_VM_HANDLER(76, ZEND_UNSET_OBJ, VAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR_PTR(BP_VAR_UNSET);
	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}
	if (OP1_TYPE == IS_VAR && UNEXPECTED(container == NULL)) {
		zend_throw_error(NULL, "Cannot unset string offsets");
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

ZEND_VM_HANDLER(77, ZEND_FE_RESET_R, CONST|TMP|VAR|CV, ANY)
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

ZEND_VM_HANDLER(125, ZEND_FE_RESET_RW, CONST|TMP|VAR|CV, ANY)
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
		if (OP1_TYPE == IS_VAR && UNEXPECTED(array_ref == NULL)) {
			zend_throw_error(NULL, "Cannot iterate on string offsets by reference");
			ZVAL_UNDEF(EX_VAR(opline->result.var));
			Z_FE_ITER_P(EX_VAR(opline->result.var)) = (uint32_t)-1;
			HANDLE_EXCEPTION();
		}
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

ZEND_VM_HANDLER(78, ZEND_FE_FETCH_R, VAR, ANY)
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
		if (opline->result_type == IS_TMP_VAR) {
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
			if (opline->result_type == IS_TMP_VAR) {
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
					zval_ptr_dtor(array);
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						zval_ptr_dtor(array);
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_r_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				zval_ptr_dtor(array);
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_r_exit);
			}
			if (opline->result_type == IS_TMP_VAR) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
					if (UNEXPECTED(EG(exception) != NULL)) {
						zval_ptr_dtor(array);
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

ZEND_VM_HANDLER(126, ZEND_FE_FETCH_RW, VAR, ANY)
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
		if (opline->result_type == IS_TMP_VAR) {
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
			if (opline->result_type == IS_TMP_VAR) {
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
					zval_ptr_dtor(array);
					HANDLE_EXCEPTION();
				}
				if (UNEXPECTED(iter->funcs->valid(iter) == FAILURE)) {
					/* reached end of iteration */
					if (UNEXPECTED(EG(exception) != NULL)) {
						zval_ptr_dtor(array);
						HANDLE_EXCEPTION();
					}
					ZEND_VM_C_GOTO(fe_fetch_w_exit);
				}
			}
			value = iter->funcs->get_current_data(iter);
			if (UNEXPECTED(EG(exception) != NULL)) {
				zval_ptr_dtor(array);
				HANDLE_EXCEPTION();
			}
			if (!value) {
				/* failure in get_current_data */
				ZEND_VM_C_GOTO(fe_fetch_w_exit);
			}
			if (opline->result_type == IS_TMP_VAR) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, EX_VAR(opline->result.var));
					if (UNEXPECTED(EG(exception) != NULL)) {
						zval_ptr_dtor(array);
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

ZEND_VM_HANDLER(114, ZEND_ISSET_ISEMPTY_VAR, CONST|TMPVAR|CV, UNUSED|CONST|VAR)
{
	USE_OPLINE
	zval *value;
	int result;

	if (OP1_TYPE == IS_CV &&
	    OP2_TYPE == IS_UNUSED &&
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

		SAVE_OPLINE();
		varname = GET_OP1_ZVAL_PTR(BP_VAR_IS);
		ZVAL_UNDEF(&tmp);
		if (OP1_TYPE != IS_CONST && Z_TYPE_P(varname) != IS_STRING) {
			ZVAL_STR(&tmp, zval_get_string(varname));
			varname = &tmp;
		}

		if (OP2_TYPE != IS_UNUSED) {
			zend_class_entry *ce;

			if (OP2_TYPE == IS_CONST) {
				if (OP1_TYPE == IS_CONST && EXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)))) != NULL)) {
					value = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)) + sizeof(void*));

					/* check if static properties were destoyed */
					if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
						value = NULL;
					}

					ZEND_VM_C_GOTO(is_var_return);
				} else if (UNEXPECTED((ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)))) == NULL)) {
					ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(opline->op2)), EX_CONSTANT(opline->op2) + 1, ZEND_FETCH_CLASS_DEFAULT | ZEND_FETCH_CLASS_EXCEPTION);
					if (UNEXPECTED(ce == NULL)) {
						ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
					}
					CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op2)), ce);
				}
			} else {
				ce = Z_CE_P(EX_VAR(opline->op2.var));
				if (OP1_TYPE == IS_CONST &&
				    (value = CACHED_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce)) != NULL) {
				
					/* check if static properties were destoyed */
					if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
						value = NULL;
					}

					ZEND_VM_C_GOTO(is_var_return);
				}
			}

			value = zend_std_get_static_property(ce, Z_STR_P(varname), 1);

			if (OP1_TYPE == IS_CONST && value) {
				CACHE_POLYMORPHIC_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), ce, value);
			}		
		} else {
			HashTable *target_symbol_table = zend_get_target_symbol_table(execute_data, opline->extended_value & ZEND_FETCH_TYPE_MASK);
			value = zend_hash_find_ind(target_symbol_table, Z_STR_P(varname));
		}

		if (OP1_TYPE != IS_CONST && Z_TYPE(tmp) != IS_UNDEF) {
			zend_string_release(Z_STR(tmp));
		}
		FREE_OP1();

ZEND_VM_C_LABEL(is_var_return):
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

ZEND_VM_HANDLER(115, ZEND_ISSET_ISEMPTY_DIM_OBJ, CONST|TMPVAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	int result;
	zend_ulong hval;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
		zend_throw_error(NULL, "Using $this when not in object context");
		FREE_UNFETCHED_OP2();
		HANDLE_EXCEPTION();
	}

	offset = GET_OP2_ZVAL_PTR_UNDEF(BP_VAR_R);

	if (OP1_TYPE != IS_UNUSED && EXPECTED(Z_TYPE_P(container) == IS_ARRAY)) {
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

	if (OP1_TYPE == IS_UNUSED ||
	    (OP1_TYPE != IS_CONST && EXPECTED(Z_TYPE_P(container) == IS_OBJECT))) {
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

ZEND_VM_HANDLER(148, ZEND_ISSET_ISEMPTY_PROP_OBJ, CONST|TMPVAR|UNUSED|CV, CONST|TMPVAR|CV)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *container;
	int result;
	zval *offset;

	SAVE_OPLINE();
	container = GET_OP1_OBJ_ZVAL_PTR(BP_VAR_IS);

	if (OP1_TYPE == IS_UNUSED && UNEXPECTED(Z_OBJ_P(container) == NULL)) {
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

ZEND_VM_HANDLER(152, ZEND_JMP_SET, CONST|TMP|VAR|CV, ANY)
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
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_COPYABLE_P(value))) {
				zval_copy_ctor_func(EX_VAR(opline->result.var));
			}
		} else if (OP1_TYPE == IS_CV) {
			if (Z_OPT_REFCOUNTED_P(value)) Z_ADDREF_P(value);
		} else if (OP1_TYPE == IS_VAR && ref) {
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(--GC_REFCOUNT(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(value)) {
				Z_ADDREF_P(value);
			}
		}
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op2));
	}

	FREE_OP1();
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(169, ZEND_COALESCE, CONST|TMP|VAR|CV, ANY)
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
		ZVAL_COPY_VALUE(EX_VAR(opline->result.var), value);
		if (OP1_TYPE == IS_CONST) {
			if (UNEXPECTED(Z_OPT_COPYABLE_P(value))) {
				zval_copy_ctor_func(EX_VAR(opline->result.var));
			}
		} else if (OP1_TYPE == IS_CV) {
			if (Z_OPT_REFCOUNTED_P(value)) Z_ADDREF_P(value);
		} else if (OP1_TYPE == IS_VAR && ref) {
			zend_reference *r = Z_REF_P(ref);

			if (UNEXPECTED(--GC_REFCOUNT(r) == 0)) {
				efree_size(r, sizeof(zend_reference));
			} else if (Z_OPT_REFCOUNTED_P(value)) {
				Z_ADDREF_P(value);
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
			if (UNEXPECTED(Z_OPT_COPYABLE_P(value))) {
				zval_copy_ctor_func(result);
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
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_statement_handler, EX(func));
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(102, ZEND_EXT_FCALL_BEGIN, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_begin_handler, EX(func));
		ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
	}
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(103, ZEND_EXT_FCALL_END, ANY, ANY)
{
	USE_OPLINE

	if (!EG(no_extensions)) {
		SAVE_OPLINE();
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_fcall_end_handler, EX(func));
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

ZEND_VM_HANDLER(140, ZEND_DECLARE_INHERITED_CLASS, ANY, ANY)
{
	USE_OPLINE

	SAVE_OPLINE();
	Z_CE_P(EX_VAR(opline->result.var)) = do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), Z_CE_P(EX_VAR(opline->extended_value)), 0);
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(145, ZEND_DECLARE_INHERITED_CLASS_DELAYED, ANY, ANY)
{
	USE_OPLINE
	zval *zce, *orig_zce;

	SAVE_OPLINE();
	if ((zce = zend_hash_find(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op2)))) == NULL ||
	    ((orig_zce = zend_hash_find(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op1)))) != NULL &&
	     Z_CE_P(zce) != Z_CE_P(orig_zce))) {
		do_bind_inherited_class(&EX(func)->op_array, opline, EG(class_table), Z_CE_P(EX_VAR(opline->extended_value)), 0);
	}
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(171, ZEND_DECLARE_ANON_CLASS, ANY, ANY)
{
	zend_class_entry *ce;
	USE_OPLINE

	SAVE_OPLINE();
	ce = zend_hash_find_ptr(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op2)));
	Z_CE_P(EX_VAR(opline->result.var)) = ce;
	ZEND_ASSERT(ce != NULL);

	if (ce->ce_flags & ZEND_ACC_ANON_BOUND) {
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op1));
	}

	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
		zend_verify_abstract_class(ce);
	}
	ce->ce_flags |= ZEND_ACC_ANON_BOUND;
	ZEND_VM_NEXT_OPCODE_CHECK_EXCEPTION();
}

ZEND_VM_HANDLER(172, ZEND_DECLARE_ANON_INHERITED_CLASS, ANY, ANY)
{
	zend_class_entry *ce;
	USE_OPLINE

	SAVE_OPLINE();
	ce = zend_hash_find_ptr(EG(class_table), Z_STR_P(EX_CONSTANT(opline->op2)));
	Z_CE_P(EX_VAR(opline->result.var)) = ce;
	ZEND_ASSERT(ce != NULL);

	if (ce->ce_flags & ZEND_ACC_ANON_BOUND) {
		ZEND_VM_JMP(OP_JMP_ADDR(opline, opline->op1));
	}

	zend_do_inheritance(ce, Z_CE_P(EX_VAR(opline->extended_value)));
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

ZEND_VM_HANDLER(105, ZEND_TICKS, ANY, ANY)
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

ZEND_VM_HANDLER(138, ZEND_INSTANCEOF, TMPVAR|CV, CONST|VAR)
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

ZEND_VM_HANDLER(149, ZEND_HANDLE_EXCEPTION, ANY, ANY)
{
	uint32_t op_num = EG(opline_before_exception) - EX(func)->op_array.opcodes;
	int i;
	uint32_t catch_op_num = 0, finally_op_num = 0, finally_op_end = 0;
	int in_finally = 0;

	ZEND_VM_INTERRUPT_CHECK();

	{
		const zend_op *exc_opline = EG(opline_before_exception);
		if ((exc_opline->opcode == ZEND_FREE || exc_opline->opcode == ZEND_FE_FREE)
			&& exc_opline->extended_value & ZEND_FREE_ON_RETURN) {
			/* exceptions thrown because of loop var destruction on return/break/...
			 * are logically thrown at the end of the foreach loop, so adjust the
			 * op_num.
			 */
			op_num = EX(func)->op_array.brk_cont_array[exc_opline->op2.num].brk;
		}
	}

	for (i = 0; i < EX(func)->op_array.last_try_catch; i++) {
		if (EX(func)->op_array.try_catch_array[i].try_op > op_num) {
			/* further blocks will not be relevant... */
			break;
		}
		in_finally = 0;
		if (op_num < EX(func)->op_array.try_catch_array[i].catch_op) {
			catch_op_num = EX(func)->op_array.try_catch_array[i].catch_op;
		}
		if (op_num < EX(func)->op_array.try_catch_array[i].finally_op) {
			finally_op_num = EX(func)->op_array.try_catch_array[i].finally_op;
			finally_op_end = EX(func)->op_array.try_catch_array[i].finally_end;
		}
		if (op_num >= EX(func)->op_array.try_catch_array[i].finally_op &&
				op_num < EX(func)->op_array.try_catch_array[i].finally_end) {
			finally_op_end = EX(func)->op_array.try_catch_array[i].finally_end;
			in_finally = 1;
		}
	}

	cleanup_unfinished_calls(execute_data, op_num);

	if (finally_op_num && (!catch_op_num || catch_op_num >= finally_op_num)) {
		zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[finally_op_end].op1.var);

		cleanup_live_vars(execute_data, op_num, finally_op_num);
		if (in_finally && Z_OBJ_P(fast_call)) {
			zend_exception_set_previous(EG(exception), Z_OBJ_P(fast_call));
		}
		Z_OBJ_P(fast_call) = EG(exception);
		EG(exception) = NULL;
		fast_call->u2.lineno = (uint32_t)-1;
		ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[finally_op_num]);
		ZEND_VM_CONTINUE();
	} else {
		cleanup_live_vars(execute_data, op_num, catch_op_num);
		if (in_finally) {
			/* we are going out of current finally scope */
			zval *fast_call = EX_VAR(EX(func)->op_array.opcodes[finally_op_end].op1.var);

			if (Z_OBJ_P(fast_call)) {
				zend_exception_set_previous(EG(exception), Z_OBJ_P(fast_call));
				Z_OBJ_P(fast_call) = NULL;
			}
		}
		if (catch_op_num) {
			ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[catch_op_num]);
			ZEND_VM_CONTINUE();
		} else if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			zend_generator *generator = zend_get_running_generator(execute_data);
			zend_generator_close(generator, 1);
			ZEND_VM_RETURN();
		} else {
			ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
		}
	}
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
			if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
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

	ZVAL_COPY_VALUE(&c.value, val);
	if (Z_OPT_CONSTANT(c.value)) {
		if (UNEXPECTED(zval_update_constant_ex(&c.value, 0, NULL) != SUCCESS)) {
			FREE_OP1();
			FREE_OP2();
			HANDLE_EXCEPTION();
		}
	} else {
		/* IS_CONST can't be IS_OBJECT, IS_RESOURCE or IS_REFERENCE */
		if (UNEXPECTED(Z_OPT_COPYABLE(c.value))) {
			zval_copy_ctor_func(&c.value);
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

	SAVE_OPLINE();

	zfunc = zend_hash_find(EG(function_table), Z_STR_P(EX_CONSTANT(opline->op1)));
	ZEND_ASSERT(zfunc != NULL && Z_FUNC_P(zfunc)->type == ZEND_USER_FUNCTION);

	if (UNEXPECTED((Z_FUNC_P(zfunc)->common.fn_flags & ZEND_ACC_STATIC) ||
				(EX(func)->common.fn_flags & ZEND_ACC_STATIC))) {
		zend_create_closure(EX_VAR(opline->result.var), Z_FUNC_P(zfunc),
			EG(scope), EX(called_scope), NULL);
	} else {
		zend_create_closure(EX_VAR(opline->result.var), Z_FUNC_P(zfunc),
			EG(scope), EX(called_scope), Z_OBJ(EX(This)) ? &EX(This) : NULL);
	}

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
		} else if (!(Z_VAR_FLAGS_P(var_ptr) & IS_VAR_RET_REF)) {
			Z_DELREF_P(var_ptr);
			ZVAL_COPY(var_ptr, Z_REFVAL_P(var_ptr));
		}
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(160, ZEND_YIELD, CONST|TMP|VAR|CV|UNUSED, CONST|TMP|VAR|CV|UNUSED)
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
			if (OP1_TYPE == IS_CONST || OP1_TYPE == IS_TMP_VAR) {
				zval *value;

				zend_error(E_NOTICE, "Only variable references should be yielded by reference");

				value = GET_OP1_ZVAL_PTR(BP_VAR_R);
				ZVAL_COPY_VALUE(&generator->value, value);
				if (OP1_TYPE == IS_CONST) {
					if (UNEXPECTED(Z_OPT_COPYABLE(generator->value))) {
						zval_copy_ctor_func(&generator->value);
					}
				}
			} else {
				zval *value_ptr = GET_OP1_ZVAL_PTR_PTR(BP_VAR_W);

				if (OP1_TYPE == IS_VAR && UNEXPECTED(value_ptr == NULL)) {
					zend_throw_error(NULL, "Cannot yield string offsets by reference");
					FREE_UNFETCHED_OP2();
					HANDLE_EXCEPTION();
				}

				/* If a function call result is yielded and the function did
				 * not return by reference we throw a notice. */
				if (OP1_TYPE == IS_VAR &&
				    (value_ptr == &EG(uninitialized_zval) ||
				     (opline->extended_value == ZEND_RETURNS_FUNCTION &&
				      !(Z_VAR_FLAGS_P(value_ptr) & IS_VAR_RET_REF)))) {
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
				if (UNEXPECTED(Z_OPT_COPYABLE(generator->value))) {
					zval_copy_ctor_func(&generator->value);
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
			if (UNEXPECTED(Z_OPT_COPYABLE(generator->key))) {
				zval_copy_ctor_func(&generator->key);
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
					zval_ptr_dtor(val);
					HANDLE_EXCEPTION();
				} else {
					zend_generator_yield_from(generator, new_gen);
				}
			} else if (UNEXPECTED(new_gen->execute_data == NULL)) {
				zend_throw_error(NULL, "Generator passed to yield from was aborted without proper return and is unable to continue");
				zval_ptr_dtor(val);
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

	/* check for delayed exception */
	if (Z_OBJ_P(fast_call) != NULL) {
		SAVE_OPLINE();
		/* discard the previously thrown exception */
		OBJ_RELEASE(Z_OBJ_P(fast_call));
		Z_OBJ_P(fast_call) = NULL;
	}

	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(162, ZEND_FAST_CALL, ANY, ANY)
{
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->result.var);

	if (opline->extended_value == ZEND_FAST_CALL_FROM_FINALLY && UNEXPECTED(Z_OBJ_P(fast_call) != NULL)) {
		fast_call->u2.lineno = (uint32_t)-1;
	} else {
		Z_OBJ_P(fast_call) = NULL;
		/* set return address */
		fast_call->u2.lineno = opline - EX(func)->op_array.opcodes;
	}
	ZEND_VM_SET_OPCODE(OP_JMP_ADDR(opline, opline->op1));
	ZEND_VM_CONTINUE();
}

ZEND_VM_HANDLER(163, ZEND_FAST_RET, ANY, ANY)
{
	USE_OPLINE
	zval *fast_call = EX_VAR(opline->op1.var);

	if (fast_call->u2.lineno != (uint32_t)-1) {
		const zend_op *fast_ret = EX(func)->op_array.opcodes + fast_call->u2.lineno;
		ZEND_VM_SET_OPCODE(fast_ret + 1);
		if (fast_ret->extended_value & ZEND_FAST_CALL_FROM_FINALLY) {
			fast_call->u2.lineno = fast_ret->op2.opline_num;
		}
		ZEND_VM_CONTINUE();
	} else {
		/* special case for unhandled exceptions */
		USE_OPLINE

		if (opline->extended_value == ZEND_FAST_RET_TO_FINALLY) {
			cleanup_live_vars(execute_data, opline - EX(func)->op_array.opcodes, opline->op2.opline_num);
			ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[opline->op2.opline_num]);
			ZEND_VM_CONTINUE();
		} else {
			EG(exception) = Z_OBJ_P(fast_call);
			Z_OBJ_P(fast_call) = NULL;
			if (opline->extended_value == ZEND_FAST_RET_TO_CATCH) {
				cleanup_live_vars(execute_data, opline - EX(func)->op_array.opcodes, opline->op2.opline_num);
				ZEND_VM_SET_OPCODE(&EX(func)->op_array.opcodes[opline->op2.opline_num]);
				ZEND_VM_CONTINUE();
			} else {
				cleanup_live_vars(execute_data, opline - EX(func)->op_array.opcodes, 0);
				if (UNEXPECTED((EX(func)->op_array.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
					zend_generator *generator = zend_get_running_generator(execute_data);
					zend_generator_close(generator, 1);
					ZEND_VM_RETURN();
				} else {
					ZEND_VM_DISPATCH_TO_HELPER(zend_leave_helper);
				}
			}
		}
	}
}

ZEND_VM_HANDLER(168, ZEND_BIND_GLOBAL, CV, CONST)
{
	USE_OPLINE
	zend_free_op free_op1, free_op2;
	zval *varname;
	zval *value;
	zval *variable_ptr;
	uint32_t idx;

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

	do {
		zend_reference *ref;

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

			if (UNEXPECTED(variable_ptr == value)) {
				break;
			}
			if (refcnt == 0) {
				SAVE_OPLINE();
				zval_dtor_func_for_ptr(Z_COUNTED_P(variable_ptr));
				if (UNEXPECTED(EG(exception))) {
					ZVAL_NULL(variable_ptr);
					HANDLE_EXCEPTION();
				}
			} else {
				GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr);
			}
		}
		ZVAL_REF(variable_ptr, ref);
	} while (0);

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

ZEND_VM_HANDLER(123, ZEND_TYPE_CHECK, CONST|TMP|VAR|CV, ANY)
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

			if (UNEXPECTED(ZSTR_LEN(ce->name) != sizeof("__PHP_Incomplete_Class") - 1) ||
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
		ZVAL_FALSE(EX_VAR(opline->result.var));
	} else {
		CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(opline->op1)), c);
		result = 1;
	}
	ZEND_VM_SMART_BRANCH(result, 0);
	ZVAL_BOOL(EX_VAR(opline->result.var), result);
	ZEND_VM_NEXT_OPCODE();
}

ZEND_VM_HANDLER(151, ZEND_ASSERT_CHECK, ANY, ANY)
{
	USE_OPLINE

	if (EG(assertions) <= 0) {
		zend_op *target = OP_JMP_ADDR(opline, opline->op2);
		zend_op *result = target - 1;
		SKIP_EXT_OPLINE(result);
		if (RETURN_VALUE_USED(result)) {
			ZVAL_TRUE(EX_VAR(result->result.var));
		}
		ZEND_VM_SET_OPCODE(target);
		ZEND_VM_CONTINUE();
	} else {
		ZEND_VM_NEXT_OPCODE();
	}
}

ZEND_VM_HANDLER(157, ZEND_FETCH_CLASS_NAME, ANY, ANY)
{
	uint32_t fetch_type;
	USE_OPLINE

	SAVE_OPLINE();
	fetch_type = opline->extended_value;

	if (UNEXPECTED(EG(scope) == NULL)) {
		zend_throw_error(NULL, "Cannot use \"%s\" when no class scope is active",
			fetch_type == ZEND_FETCH_CLASS_SELF ? "self" :
			fetch_type == ZEND_FETCH_CLASS_PARENT ? "parent" : "static");
		HANDLE_EXCEPTION();
	}

	switch (fetch_type) {
		case ZEND_FETCH_CLASS_SELF:
			ZVAL_STR_COPY(EX_VAR(opline->result.var), EG(scope)->name);
			break;
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(EG(scope)->parent == NULL)) {
				zend_throw_error(NULL,
					"Cannot use \"parent\" when current class scope has no parent");
				HANDLE_EXCEPTION();
			}
			ZVAL_STR_COPY(EX_VAR(opline->result.var), EG(scope)->parent->name);
			break;
		case ZEND_FETCH_CLASS_STATIC:
			ZVAL_STR_COPY(EX_VAR(opline->result.var), EX(called_scope)->name);
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

		if (UNEXPECTED((fbc->common.fn_flags & ZEND_ACC_GENERATOR) != 0)) {
			if (ret) {
				zend_generator_create_zval(call, &fbc->op_array, ret);
				Z_VAR_FLAGS_P(ret) = 0;
			} else {
				if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_CLOSURE)) {
					OBJ_RELEASE((zend_object*)fbc->op_array.prototype);
				}
				zend_vm_stack_free_args(call);
			}
		} else {
			call->symbol_table = NULL;
			i_init_func_execute_data(call, &fbc->op_array,
					ret, (fbc->common.fn_flags & ZEND_ACC_STATIC) == 0);

			if (EXPECTED(zend_execute_ex == execute_ex)) {
				ZEND_VM_ENTER();
			} else {
				ZEND_ADD_CALL_FLAG(call, ZEND_CALL_TOP);
				zend_execute_ex(call);
			}
		}
	} else {
		zval retval;

		ZEND_ASSERT(fbc->type == ZEND_INTERNAL_FUNCTION);

		EG(current_execute_data) = call;

		if (fbc->common.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
			uint32_t i;
			uint32_t num_args = ZEND_CALL_NUM_ARGS(call);
			zval *p = ZEND_CALL_ARG(call, 1);

			EG(current_execute_data) = call;

			for (i = 0; i < num_args; ++i) {
				if (UNEXPECTED(!zend_verify_internal_arg_type(fbc, i + 1, p))) {
					EG(current_execute_data) = call->prev_execute_data;
					zend_vm_stack_free_args(call);
					zend_vm_stack_free_call_frame(call);
					if (ret) {
						ZVAL_UNDEF(ret);
					}
					ZEND_VM_C_GOTO(call_trampoline_end);
				}
				p++;
			}
		}

		if (ret == NULL) {
			ZVAL_NULL(&retval);
			ret = &retval;
		}
		Z_VAR_FLAGS_P(ret) = (fbc->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0 ? IS_VAR_RET_REF : 0;

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
			zend_verify_internal_return_type(call->func, EX_VAR(opline->result.var)));
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
	EG(scope) = EX(func)->op_array.scope;
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

ZEND_VM_DEFINE_OP(137, ZEND_OP_DATA);
