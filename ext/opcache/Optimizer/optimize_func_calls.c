/* pass 4
 * - optimize INIT_FCALL_BY_NAME to DO_FCALL
 */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO

typedef struct _optimizer_call_info {
	zend_function *func;
	zend_op       *opline;
} optimizer_call_info;

static void optimize_func_calls(zend_op_array *op_array, zend_persistent_script *script TSRMLS_DC) {
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	int call = 0;
#if ZEND_EXTENSION_API_NO > PHP_5_4_X_API_NO
	optimizer_call_info *call_stack = ecalloc(op_array->nested_calls + 1, sizeof(optimizer_call_info));
#else
	int stack_size = 4;
	optimizer_call_info *call_stack = ecalloc(stack_size, sizeof(optimizer_call_info));
#endif

	while (opline < end) {
		switch (opline->opcode) {
			case ZEND_INIT_FCALL_BY_NAME:
			case ZEND_INIT_NS_FCALL_BY_NAME:
				if (ZEND_OP2_TYPE(opline) == IS_CONST) {
					zend_function *func;
					zval *function_name = &op_array->literals[opline->op2.constant + 1].constant;
					if ((zend_hash_quick_find(&script->function_table,
							Z_STRVAL_P(function_name), Z_STRLEN_P(function_name) + 1,
							Z_HASH_P(function_name), (void **)&func) == SUCCESS)) {
						call_stack[call].func = func;
					}
				}
				/* break missing intentionally */
			case ZEND_NEW:
			case ZEND_INIT_METHOD_CALL:
			case ZEND_INIT_STATIC_METHOD_CALL:
				call_stack[call].opline = opline;
				call++;
#if ZEND_EXTENSION_API_NO < PHP_5_5_X_API_NO
				if (call == stack_size) {
					stack_size += 4;
					call_stack = erealloc(call_stack, sizeof(optimizer_call_info) * stack_size);
					memset(call_stack + 4, 0, 4 * sizeof(optimizer_call_info));
				}
#endif
				break;
			case ZEND_DO_FCALL_BY_NAME:
				call--;
				if (call_stack[call].func && call_stack[call].opline) {
					zend_op *fcall = call_stack[call].opline;

					opline->opcode = ZEND_DO_FCALL;
					ZEND_OP1_TYPE(opline) = IS_CONST;
					opline->op1.constant = fcall->op2.constant + 1;
					op_array->literals[fcall->op2.constant + 1].cache_slot = op_array->literals[fcall->op2.constant].cache_slot;
					literal_dtor(&ZEND_OP2_LITERAL(fcall));
					if (fcall->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
						literal_dtor(&op_array->literals[fcall->op2.constant + 2].constant);
					}
					MAKE_NOP(fcall);
				} else if (opline->extended_value == 0 &&
				           call_stack[call].opline &&
				           call_stack[call].opline->opcode == ZEND_INIT_FCALL_BY_NAME &&
				           ZEND_OP2_TYPE(call_stack[call].opline) == IS_CONST) {

					zend_op *fcall = call_stack[call].opline;

					opline->opcode = ZEND_DO_FCALL;
					ZEND_OP1_TYPE(opline) = IS_CONST;
					opline->op1.constant = fcall->op2.constant + 1;
					op_array->literals[fcall->op2.constant + 1].cache_slot = op_array->literals[fcall->op2.constant].cache_slot;
					literal_dtor(&ZEND_OP2_LITERAL(fcall));
					MAKE_NOP(fcall);
				}
				call_stack[call].func = NULL;
				call_stack[call].opline = NULL;
				break;
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_OBJ_FUNC_ARG:
			case ZEND_FETCH_DIM_FUNC_ARG:
				if (call_stack[call - 1].func) {
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, (opline->extended_value & ZEND_FETCH_ARG_MASK))) {
						opline->extended_value &= ZEND_FETCH_TYPE_MASK;
						opline->opcode -= 9;
					} else {
						opline->extended_value &= ZEND_FETCH_TYPE_MASK;
						opline->opcode -= 12;
					}
				}
				break;
			case ZEND_SEND_VAL:
				if (opline->extended_value == ZEND_DO_FCALL_BY_NAME && call_stack[call - 1].func) {
					if (ARG_MUST_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
						/* We won't convert it into_DO_FCALL to emit error at run-time */
						call_stack[call - 1].opline = NULL;
					} else {
						opline->extended_value = ZEND_DO_FCALL;
					}
				}
				break;
			case ZEND_SEND_VAR:
				if (opline->extended_value == ZEND_DO_FCALL_BY_NAME && call_stack[call - 1].func) {
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
						opline->opcode = ZEND_SEND_REF;
					}
					opline->extended_value = ZEND_DO_FCALL;
				}
				break;
			case ZEND_SEND_VAR_NO_REF:
				if (!(opline->extended_value & ZEND_ARG_COMPILE_TIME_BOUND) && call_stack[call - 1].func) {
					if (ARG_SHOULD_BE_SENT_BY_REF(call_stack[call - 1].func, opline->op2.num)) {
						opline->extended_value |= ZEND_ARG_COMPILE_TIME_BOUND | ZEND_ARG_SEND_BY_REF;
					} else if (opline->extended_value) {
						opline->extended_value |= ZEND_ARG_COMPILE_TIME_BOUND;
					} else {
						opline->opcode = ZEND_SEND_VAR;
						opline->extended_value = ZEND_DO_FCALL;
					}
				}
				break;
			case ZEND_SEND_REF:
				if (opline->extended_value == ZEND_DO_FCALL_BY_NAME && call_stack[call - 1].func) {
					/* We won't handle run-time pass by reference */
					call_stack[call - 1].opline = NULL;
				}
				break;
#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
			case ZEND_SEND_UNPACK:
				call_stack[call - 1].func = NULL;
				call_stack[call - 1].opline = NULL;
				break;
#endif
			default:
				break;
		}
		opline++;
	}

	efree(call_stack);
}
#endif
