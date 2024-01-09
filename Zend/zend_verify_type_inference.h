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
*/

#include "zend_type_info.h"
#include "zend_compile.h"

#define VM_TRACE_START()
#define VM_TRACE_END()
#define VM_TRACE(op) zend_verify_inference_use(execute_data, OPLINE); \
	{ \
		zend_execute_data *__current_ex = NULL; \
		const zend_op *__current_op = NULL; \
		if (OPLINE->opcode != ZEND_GENERATOR_RETURN) { \
			__current_ex = execute_data; __current_op = OPLINE; \
		}
#define VM_TRACE_OP_END(op) \
		if (__current_ex && __current_op) { \
			zend_verify_inference_def(__current_ex, __current_op); \
		} \
	}

#define ZEND_VERIFY_TYPE_INFERENCE_ERROR(msg, ...) \
	do { \
		fprintf(stderr, "Inference verification failed at %04d %s (" msg ")\n", (int)(opline - EX(func)->op_array.opcodes), operand, __VA_ARGS__); \
		exit(139); \
	} while (0)

static void zend_verify_type_inference(zval *value, uint32_t type_mask, uint8_t op_type, zend_execute_data *execute_data, const zend_op *opline, const char *operand)
{
	if (type_mask == MAY_BE_CLASS) {
		return;
	}

	if (Z_TYPE_P(value) == IS_INDIRECT) {
		if (!(type_mask & MAY_BE_INDIRECT)) {
			ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x mising MAY_BE_INDIRECT", type_mask);
		}
		value = Z_INDIRECT_P(value);
	}

	/* Verifying RC inference is currently not possible because type information is based on the SSA
	 * built without ZEND_SSA_RC_INFERENCE, which is missing various definitions for RC-modifying
	 * operations. Support could be added by repeating SSA-construction and type inference with the
	 * given flag. */
	// if (Z_REFCOUNTED_P(value)) {
	// 	if (Z_REFCOUNT_P(value) == 1 && !(type_mask & MAY_BE_RC1)) {
	// 		ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_RC1", type_mask);
	// 	}
	// 	if (Z_REFCOUNT_P(value) > 1 && !(type_mask & MAY_BE_RCN)) {
	// 		ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_RCN", type_mask);
	// 	}
	// }

	if (Z_TYPE_P(value) == IS_REFERENCE) {
		if (!(type_mask & MAY_BE_REF)) {
			ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_REF", type_mask);
		}
		value = Z_REFVAL_P(value);
	}

	if (!(type_mask & (1u << Z_TYPE_P(value)))) {
		if (Z_TYPE_P(value) == IS_UNUSED && op_type == IS_VAR && (type_mask & MAY_BE_NULL)) {
			/* FETCH_OBJ_* for typed property may return IS_UNDEF. This is an exception. */
		} else {
			ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing type %d", type_mask, Z_TYPE_P(value));
		}
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		HashTable *ht = Z_ARRVAL_P(value);
		uint32_t num_checked = 0;
		zend_string *str;
		zval *val;
		if (HT_IS_INITIALIZED(ht)) {
			if (HT_IS_PACKED(ht) && !MAY_BE_PACKED(type_mask)) {
				ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_ARRAY_PACKED", type_mask);
			}
			if (!HT_IS_PACKED(ht) && !MAY_BE_HASH(type_mask)) {
				ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_ARRAY_HASH", type_mask);
			}
		} else {
			if (!(type_mask & MAY_BE_ARRAY_EMPTY)) {
				ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_ARRAY_EMPTY", type_mask);
			}
		}
		ZEND_HASH_FOREACH_STR_KEY_VAL(ht, str, val) {
			if (str) {
				if (!(type_mask & MAY_BE_ARRAY_KEY_STRING)) {
					ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_ARRAY_KEY_STRING", type_mask);
					break;
				}
			} else {
				if (!(type_mask & MAY_BE_ARRAY_KEY_LONG)) {
					ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing MAY_BE_ARRAY_KEY_LONG", type_mask);
					break;
				}
			}

			uint32_t array_type = 1u << (Z_TYPE_P(val) + MAY_BE_ARRAY_SHIFT);
			if (!(type_mask & array_type)) {
				ZEND_VERIFY_TYPE_INFERENCE_ERROR("mask 0x%x missing array type %d", type_mask, Z_TYPE_P(val));
				break;
			}

			/* Don't check all elements of large arrays. */
			if (++num_checked > 16) {
				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
}

/* Clang reports false positive unused warnings. */
#ifdef __clang__
__attribute__((unused))
#endif
static void zend_verify_inference_use(zend_execute_data *execute_data, const zend_op *opline)
{
	if (opline->op1_use_type
	 && (opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV))
	 && opline->opcode != ZEND_ROPE_ADD
	 && opline->opcode != ZEND_ROPE_END) {
		zend_verify_type_inference(EX_VAR(opline->op1.var), opline->op1_use_type, opline->op1_type, execute_data, opline, "op1_use");
	}
	if (opline->op2_use_type
	 && (opline->op2_type & (IS_TMP_VAR|IS_VAR|IS_CV))) {
		zend_verify_type_inference(EX_VAR(opline->op2.var), opline->op2_use_type, opline->op2_type, execute_data, opline, "op2_use");
	}
	if (opline->result_use_type
	 && (opline->result_type & (IS_TMP_VAR|IS_VAR|IS_CV))) {
		zend_verify_type_inference(EX_VAR(opline->result.var), opline->result_use_type, opline->result_type, execute_data, opline, "result_use");
	}
}

/* Clang reports false positive unused warnings. */
#ifdef __clang__
__attribute__((unused))
#endif
static void zend_verify_inference_def(zend_execute_data *execute_data, const zend_op *opline)
{
	if (EG(exception)) {
		return;
	}
	if (opline->op1_def_type
	 && (opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV))
	 // array is actually changed by the the following instruction(s)
	 && opline->opcode != ZEND_FETCH_DIM_W
	 && opline->opcode != ZEND_FETCH_DIM_RW
	 && opline->opcode != ZEND_FETCH_DIM_FUNC_ARG
	 && opline->opcode != ZEND_FETCH_LIST_W) {
		zend_verify_type_inference(EX_VAR(opline->op1.var), opline->op1_def_type, opline->op1_type, execute_data, opline, "op1_def");
	}
	if (opline->op2_def_type
	 && (opline->op2_type & (IS_TMP_VAR|IS_VAR|IS_CV))
	 /* ZEND_FE_FETCH_R[W] does not define a result in the last iteration. */
	 && opline->opcode != ZEND_FE_FETCH_R
	 && opline->opcode != ZEND_FE_FETCH_RW) {
		zend_verify_type_inference(EX_VAR(opline->op2.var), opline->op2_def_type, opline->op2_type, execute_data, opline, "op2_def");
	}
	if (opline->result_def_type
	 && (opline->result_type & (IS_TMP_VAR|IS_VAR|IS_CV))
	 && opline->opcode != ZEND_ROPE_INIT
	 && opline->opcode != ZEND_ROPE_ADD
	 /* Some jump opcode handlers don't set result when it's never read. */
	 && opline->opcode != ZEND_JMP_SET
	 && opline->opcode != ZEND_JMP_NULL
	 && opline->opcode != ZEND_COALESCE
	 && opline->opcode != ZEND_ASSERT_CHECK
	 /* Smart branches may not declare result. */
	 && !zend_is_smart_branch(opline)
	 /* Calls only initialize result when returning from the called function. */
	 && opline->opcode != ZEND_DO_FCALL
	 && opline->opcode != ZEND_DO_ICALL
	 && opline->opcode != ZEND_DO_UCALL
	 && opline->opcode != ZEND_DO_FCALL_BY_NAME
	 /* ZEND_FE_FETCH_R[W] does not define a result in the last iteration. */
	 && opline->opcode != ZEND_FE_FETCH_R
	 && opline->opcode != ZEND_FE_FETCH_RW) {
		zend_verify_type_inference(EX_VAR(opline->result.var), opline->result_def_type, opline->result_type, execute_data, opline, "result_def");

		/* Verify return value in the context of caller. */
		if ((opline->opcode == ZEND_RETURN || opline->opcode == ZEND_RETURN_BY_REF)
		 && execute_data->prev_execute_data
		 && execute_data->prev_execute_data->func
		 && ZEND_USER_CODE(execute_data->prev_execute_data->func->type)) {
			zend_execute_data *prev_execute_data = execute_data->prev_execute_data;
			const zend_op *opline = execute_data->prev_execute_data->opline;
			zend_verify_type_inference(ZEND_CALL_VAR(prev_execute_data, opline->result.var), opline->result_def_type, opline->result_type, prev_execute_data, opline, "result_def");
		}
	}
}
