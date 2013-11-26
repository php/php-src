/* pass 1
 * - substitute persistent constants (true, false, null, etc)
 * - perform compile-time evaluation of constant binary and unary operations
 * - optimize series of ADD_STRING and/or ADD_CHAR
 * - convert CAST(IS_BOOL,x) into BOOL(x)
 * - convert INTI_FCALL_BY_NAME, DO_FCALL_BY_NAME into DO_FCALL
 */

if (ZEND_OPTIMIZER_PASS_1 & OPTIMIZATION_LEVEL) {
	int i = 0;
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;

	while (opline < end) {
		switch (opline->opcode) {
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
		case ZEND_MOD:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_CONCAT:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_BOOL_XOR:
			if (ZEND_OP1_TYPE(opline) == IS_CONST &&
				ZEND_OP2_TYPE(opline) == IS_CONST) {
				/* binary operation with constant operands */
				int (*binary_op)(zval *result, zval *op1, zval *op2 TSRMLS_DC) = get_binary_op(opline->opcode);
				zend_uint tv = ZEND_RESULT(opline).var;		/* temporary variable */
				zval result;
				int er;

				if (opline->opcode == ZEND_DIV &&
					Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_LONG &&
					Z_LVAL(ZEND_OP2_LITERAL(opline)) == 0) {
					/* div by 0 */
					break;
				}
				er = EG(error_reporting);
				EG(error_reporting) = 0;
				/* evaluate constant expression */
				if (binary_op(&result, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline) TSRMLS_CC) != SUCCESS) {
					EG(error_reporting) = er;
					break;
				}
				EG(error_reporting) = er;
				PZ_SET_REFCOUNT_P(&result, 1);
				PZ_UNSET_ISREF_P(&result);

				literal_dtor(&ZEND_OP1_LITERAL(opline));
				literal_dtor(&ZEND_OP2_LITERAL(opline));
				MAKE_NOP(opline);

				replace_tmp_by_const(op_array, opline + 1, tv, &result TSRMLS_CC);
			}
			break;

		case ZEND_CAST:
			if (ZEND_OP1_TYPE(opline) == IS_CONST &&
				opline->extended_value != IS_ARRAY &&
				opline->extended_value != IS_OBJECT &&
				opline->extended_value != IS_RESOURCE) {
				/* cast of constant operand */
				zend_uint tv = ZEND_RESULT(opline).var;		/* temporary variable */
				zval res;
				res = ZEND_OP1_LITERAL(opline);
				zval_copy_ctor(&res);
				switch (opline->extended_value) {
					case IS_NULL:
						convert_to_null(&res);
						break;
					case IS_BOOL:
						convert_to_boolean(&res);
						break;
					case IS_LONG:
						convert_to_long(&res);
						break;
					case IS_DOUBLE:
						convert_to_double(&res);
						break;
					case IS_STRING:
						convert_to_string(&res);
						break;
				}

				literal_dtor(&ZEND_OP1_LITERAL(opline));
				MAKE_NOP(opline);

				replace_tmp_by_const(op_array, opline + 1, tv, &res TSRMLS_CC);
			} else if (opline->extended_value == IS_BOOL) {
				/* T = CAST(X, IS_BOOL) => T = BOOL(X) */
				opline->opcode = ZEND_BOOL;
				opline->extended_value = 0;
			}
			break;

		case ZEND_BW_NOT:
		case ZEND_BOOL_NOT:
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				/* unary operation on constant operand */
				unary_op_type unary_op = get_unary_op(opline->opcode);
				zval result;
				zend_uint tv = ZEND_RESULT(opline).var;		/* temporary variable */
				int er;

				er = EG(error_reporting);
				EG(error_reporting) = 0;
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
				if (unary_op(&result, &ZEND_OP1_LITERAL(opline)) != SUCCESS) {
#else
				if (unary_op(&result, &ZEND_OP1_LITERAL(opline) TSRMLS_CC) != SUCCESS) {
#endif
					EG(error_reporting) = er;
					break;
				}
				EG(error_reporting) = er;
				PZ_SET_REFCOUNT_P(&result, 1);
				PZ_UNSET_ISREF_P(&result);

				literal_dtor(&ZEND_OP1_LITERAL(opline));
				MAKE_NOP(opline);

				replace_tmp_by_const(op_array, opline + 1, tv, &result TSRMLS_CC);
			}
			break;

		case ZEND_ADD_STRING:
		case ZEND_ADD_CHAR:
			{
				zend_op *next_op = opline + 1;
				int requires_conversion = (opline->opcode == ZEND_ADD_CHAR? 1 : 0);
				size_t final_length = 0;
				char *ptr;
				zend_op *last_op;

				/* There is always a ZEND_RETURN at the end
				if (next_op>=end) {
					break;
				}
				*/
				while (next_op->opcode == ZEND_ADD_STRING || next_op->opcode == ZEND_ADD_CHAR) {
					if (ZEND_RESULT(opline).var != ZEND_RESULT(next_op).var) {
						break;
					}
					if (next_op->opcode == ZEND_ADD_CHAR) {
						final_length += 1;
					} else { /* ZEND_ADD_STRING */
						final_length += ZEND_OP2_LITERAL(next_op).value.str.len;
					}
					next_op++;
				}
				if (final_length == 0) {
					break;
				}
				last_op = next_op;
				final_length += (requires_conversion? 1 : ZEND_OP2_LITERAL(opline).value.str.len);
				ptr = (char *)emalloc(final_length + 1);
				ptr[final_length] = '\0';
				if (requires_conversion) { /* ZEND_ADD_CHAR */
					char chval = (char)ZEND_OP2_LITERAL(opline).value.lval;

					ZEND_OP2_LITERAL(opline).value.str.val = ptr;
					ptr[0] = chval;
					ZEND_OP2_LITERAL(opline).type = IS_STRING;
					opline->opcode = ZEND_ADD_STRING;
					ptr++;
				} else { /* ZEND_ADD_STRING */
					memcpy(ptr, Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)));
					if (!IS_INTERNED(Z_STRVAL(ZEND_OP2_LITERAL(opline)))) {
						efree(Z_STRVAL(ZEND_OP2_LITERAL(opline)));
					}
					Z_STRVAL(ZEND_OP2_LITERAL(opline)) = ptr;
					ptr += Z_STRLEN(ZEND_OP2_LITERAL(opline));
				}
				ZEND_OP2_LITERAL(opline).value.str.len = final_length;
				next_op = opline + 1;
				while (next_op < last_op) {
					if (next_op->opcode == ZEND_ADD_STRING) {
						memcpy(ptr, ZEND_OP2_LITERAL(next_op).value.str.val, ZEND_OP2_LITERAL(next_op).value.str.len);
						ptr += ZEND_OP2_LITERAL(next_op).value.str.len;
						literal_dtor(&ZEND_OP2_LITERAL(next_op));
					} else { /* ZEND_ADD_CHAR */
						*ptr = (char)ZEND_OP2_LITERAL(next_op).value.lval;
						ptr++;
					}
					MAKE_NOP(next_op);
					next_op++;
				}
				if (!((ZEND_OPTIMIZER_PASS_5|ZEND_OPTIMIZER_PASS_10) & OPTIMIZATION_LEVEL)) {
					/* NOP removal is disabled => insert JMP over NOPs */
					if (last_op-opline >= 3) { /* If we have more than 2 NOPS then JMP over them */
						(opline + 1)->opcode = ZEND_JMP;
						ZEND_OP1(opline + 1).opline_num = last_op - op_array->opcodes; /* that's OK even for ZE2, since opline_num's are resolved in pass 2 later */
					}
				}
			}
			break;

		case ZEND_FETCH_CONSTANT:
			if (ZEND_OP1_TYPE(opline) == IS_UNUSED &&
				ZEND_OP2_TYPE(opline) == IS_CONST &&
				Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING &&
				Z_STRLEN(ZEND_OP2_LITERAL(opline)) == sizeof("__COMPILER_HALT_OFFSET__") - 1 &&
				memcmp(Z_STRVAL(ZEND_OP2_LITERAL(opline)), "__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1) == 0) {
				/* substitute __COMPILER_HALT_OFFSET__ constant */
				zend_bool orig_in_execution = EG(in_execution);
				zend_op_array *orig_op_array = EG(active_op_array);
				zval offset;

				EG(in_execution) = 1;
				EG(active_op_array) = op_array;
				if (zend_get_constant("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1, &offset TSRMLS_CC)) {
					zend_uint tv = ZEND_RESULT(opline).var;

					literal_dtor(&ZEND_OP2_LITERAL(opline));
					MAKE_NOP(opline);
					replace_tmp_by_const(op_array, opline, tv, &offset TSRMLS_CC);
				}
				EG(active_op_array) = orig_op_array;
				EG(in_execution) = orig_in_execution;
				break;
			}

			if (ZEND_OP1_TYPE(opline) == IS_UNUSED &&
				ZEND_OP2_TYPE(opline) == IS_CONST &&
				ZEND_OP2_LITERAL(opline).type == IS_STRING) {
				/* substitute persistent constants */
				zend_uint tv = ZEND_RESULT(opline).var;
				zval c;

				if (!zend_get_persistent_constant(Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)), &c, 1 TSRMLS_CC)) {
					break;
				}
				literal_dtor(&ZEND_OP2_LITERAL(opline));
				MAKE_NOP(opline);
				replace_tmp_by_const(op_array, opline, tv, &c TSRMLS_CC);
			}
			break;

		case ZEND_INIT_FCALL_BY_NAME:
			if (opline->extended_value == 0 /* not method */ &&
				ZEND_OP1_TYPE(opline) == IS_UNUSED &&
				ZEND_OP2_TYPE(opline) == IS_CONST) {
				if ((opline + 1)->opcode == ZEND_DO_FCALL_BY_NAME &&
					(opline + 1)->extended_value == 0) {
					(opline + 1)->opcode = ZEND_DO_FCALL;
					COPY_NODE((opline + 1)->op1, opline->op2);
					zend_str_tolower(Z_STRVAL(ZEND_OP1_LITERAL(opline + 1)), Z_STRLEN(ZEND_OP1_LITERAL(opline + 1)));
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
					Z_HASH_P(&ZEND_OP1_LITERAL(opline + 1)) = zend_hash_func(Z_STRVAL(ZEND_OP1_LITERAL(opline + 1)), Z_STRLEN(ZEND_OP1_LITERAL(opline + 1)) + 1);
					op_array->literals[(opline + 1)->op1.constant].cache_slot = op_array->last_cache_slot++;
#endif
					MAKE_NOP(opline);
				}
			}
			break;

#if ZEND_EXTENSION_API_NO >= PHP_5_5_X_API_NO
		case ZEND_FETCH_R:
		case ZEND_FETCH_W:
		case ZEND_FETCH_RW:
		case ZEND_FETCH_FUNC_ARG:
		case ZEND_FETCH_IS:
		case ZEND_FETCH_UNSET:
			if (opline != op_array->opcodes &&
			    (opline-1)->opcode == ZEND_BEGIN_SILENCE &&
			    (opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL &&
				opline->op1_type == IS_CONST &&
			    opline->op2_type == IS_UNUSED &&
			    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
			    (Z_STRLEN(ZEND_OP1_LITERAL(opline)) != sizeof("this")-1 ||
			     memcmp(Z_STRVAL(ZEND_OP1_LITERAL(opline)), "this", sizeof("this")) != 0)) {

			    int var = opline->result.var;
			    int level = 0;
				zend_op *op = opline + 1;
				zend_op *use = NULL;

				while (op < end) {
					if (op->opcode == ZEND_BEGIN_SILENCE) {
						level++;
					} else if (op->opcode == ZEND_END_SILENCE) {
						if (level == 0) {
							break;
						} else {
							level--;
						}
					}
					if (op->op1_type == IS_VAR && op->op1.var == var) {
						if (use) {
							/* used more than once */
							use = NULL;
							break;
						}
						use = op;
					} else if (op->op2_type == IS_VAR && op->op2.var == var) {
						if (use) {
							/* used more than once */
							use = NULL;
							break;
						}
						use = op;
					}
					op++;
				}
				if (use) {
					if (use->op1_type == IS_VAR && use->op1.var == var) {
						use->op1_type = IS_CV;
						use->op1.var = zend_optimizer_lookup_cv(op_array,
							Z_STRVAL(ZEND_OP1_LITERAL(opline)),
							Z_STRLEN(ZEND_OP1_LITERAL(opline)));
						MAKE_NOP(opline);
					} else if (use->op2_type == IS_VAR && use->op2.var == var) {
						use->op2_type = IS_CV;
						use->op2.var = zend_optimizer_lookup_cv(op_array,
							Z_STRVAL(ZEND_OP1_LITERAL(opline)),
							Z_STRLEN(ZEND_OP1_LITERAL(opline)));
						MAKE_NOP(opline);
					}
				}
			}
			break;
#endif
		
		}
		opline++;
		i++;
	}
}
