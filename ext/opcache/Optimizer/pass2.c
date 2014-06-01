/* pass 2:
 * - convert non-numeric constants to numeric constants in numeric operators
 * - optimize constant conditional JMPs
 * - optimize static BRKs and CONTs
 */

if (ZEND_OPTIMIZER_PASS_2 & OPTIMIZATION_LEVEL) {
	zend_op *opline;
	zend_op *end = op_array->opcodes + op_array->last;

	opline = op_array->opcodes;
	while (opline < end) {
		switch (opline->opcode) {
			case ZEND_ADD:
			case ZEND_SUB:
			case ZEND_MUL:
			case ZEND_DIV:
				if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					if (ZEND_OP1_LITERAL(opline).type == IS_STRING) {
						convert_scalar_to_number(&ZEND_OP1_LITERAL(opline) TSRMLS_CC);
					}
				}
				/* break missing *intentionally* - the assign_op's may only optimize op2 */
			case ZEND_ASSIGN_ADD:
			case ZEND_ASSIGN_SUB:
			case ZEND_ASSIGN_MUL:
			case ZEND_ASSIGN_DIV:
				if (opline->extended_value != 0) {
					/* object tristate op - don't attempt to optimize it! */
					break;
				}
				if (ZEND_OP2_TYPE(opline) == IS_CONST) {
					if (ZEND_OP2_LITERAL(opline).type == IS_STRING) {
						convert_scalar_to_number(&ZEND_OP2_LITERAL(opline) TSRMLS_CC);
					}
				}
				break;

			case ZEND_MOD:
			case ZEND_SL:
			case ZEND_SR:
				if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					if (ZEND_OP1_LITERAL(opline).type != IS_LONG) {
						convert_to_long(&ZEND_OP1_LITERAL(opline));
					}
				}
				/* break missing *intentionally - the assign_op's may only optimize op2 */
			case ZEND_ASSIGN_MOD:
			case ZEND_ASSIGN_SL:
			case ZEND_ASSIGN_SR:
				if (opline->extended_value != 0) {
					/* object tristate op - don't attempt to optimize it! */
					break;
				}
				if (ZEND_OP2_TYPE(opline) == IS_CONST) {
					if (ZEND_OP2_LITERAL(opline).type != IS_LONG) {
						convert_to_long(&ZEND_OP2_LITERAL(opline));
					}
				}
				break;

			case ZEND_CONCAT:
				if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					if (ZEND_OP1_LITERAL(opline).type != IS_STRING) {
						convert_to_string(&ZEND_OP1_LITERAL(opline));
					}
				}
				/* break missing *intentionally - the assign_op's may only optimize op2 */
			case ZEND_ASSIGN_CONCAT:
				if (opline->extended_value != 0) {
					/* object tristate op - don't attempt to optimize it! */
					break;
				}
				if (ZEND_OP2_TYPE(opline) == IS_CONST) {
					if (ZEND_OP2_LITERAL(opline).type != IS_STRING) {
						convert_to_string(&ZEND_OP2_LITERAL(opline));
					}
				}
				break;

			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
				/* convert Ti = JMPZ_EX(Ti, L) to JMPZ(Ti, L) */
				if (0 && /* FIXME: temporary disable unsafe pattern */
				    ZEND_OP1_TYPE(opline) == IS_TMP_VAR &&
				    ZEND_RESULT_TYPE(opline) == IS_TMP_VAR &&
				    ZEND_OP1(opline).var == ZEND_RESULT(opline).var) {
					opline->opcode -= 3;
				/* convert Ti = JMPZ_EX(C, L) => Ti = QM_ASSIGN(C)
				   in case we know it wouldn't jump */
				} else if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					int should_jmp = zend_is_true(&ZEND_OP1_LITERAL(opline));
					if (opline->opcode == ZEND_JMPZ_EX) {
						should_jmp = !should_jmp;
					}
					if (!should_jmp) {
						opline->opcode = ZEND_QM_ASSIGN;
						SET_UNUSED(opline->op2);
					}
				}
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
				if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					int should_jmp = zend_is_true(&ZEND_OP1_LITERAL(opline));

					if (opline->opcode == ZEND_JMPZ) {
						should_jmp = !should_jmp;
					}
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					ZEND_OP1_TYPE(opline) = IS_UNUSED;
					if (should_jmp) {
						opline->opcode = ZEND_JMP;
						COPY_NODE(opline->op1, opline->op2);
					} else {
						MAKE_NOP(opline);
					}
					break;
				}
				if ((opline + 1)->opcode == ZEND_JMP) {
					/* JMPZ(X, L1), JMP(L2) => JMPZNZ(X, L1, L2) */
					/* JMPNZ(X, L1), JMP(L2) => JMPZNZ(X, L2, L1) */
					if (ZEND_OP2(opline).opline_num == ZEND_OP1(opline + 1).opline_num) {
						/* JMPZ(X, L1), JMP(L1) => NOP, JMP(L1) */
						MAKE_NOP(opline);
					} else {
						if (opline->opcode == ZEND_JMPZ) {
							opline->extended_value = ZEND_OP1(opline + 1).opline_num;
						} else {
							opline->extended_value = ZEND_OP2(opline).opline_num;
							COPY_NODE(opline->op2, (opline + 1)->op1);
						}
						opline->opcode = ZEND_JMPZNZ;
					}
				}
				break;

			case ZEND_JMPZNZ:
				if (ZEND_OP1_TYPE(opline) == IS_CONST) {
					int opline_num;

					if (zend_is_true(&ZEND_OP1_LITERAL(opline))) {
						opline_num = opline->extended_value; /* JMPNZ */
					} else {
						opline_num = ZEND_OP2(opline).opline_num; /* JMPZ */
					}
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					ZEND_OP1(opline).opline_num = opline_num;
					ZEND_OP1_TYPE(opline) = IS_UNUSED;
					opline->opcode = ZEND_JMP;
				}
				break;

			case ZEND_BRK:
			case ZEND_CONT:
				{
				    zend_brk_cont_element *jmp_to;
					int array_offset;
					int nest_levels;
					int dont_optimize = 0;

					if (ZEND_OP2_TYPE(opline) != IS_CONST) {
						break;
					}
					convert_to_long(&ZEND_OP2_LITERAL(opline));
					nest_levels = ZEND_OP2_LITERAL(opline).value.lval;

					array_offset = ZEND_OP1(opline).opline_num;
					while (1) {
						if (array_offset == -1) {
							dont_optimize = 1; /* don't optimize this bogus break/continue, let the executor shout */
							break;
						}
						jmp_to = &op_array->brk_cont_array[array_offset];
						array_offset = jmp_to->parent;
						if (--nest_levels > 0) {
							if (opline->opcode == ZEND_BRK &&
							    (op_array->opcodes[jmp_to->brk].opcode == ZEND_FREE ||
							     op_array->opcodes[jmp_to->brk].opcode == ZEND_SWITCH_FREE)) {
								dont_optimize = 1;
								break;
							}
						} else {
							break;
						}
					}

					if (dont_optimize) {
						break;
					}

					/* optimize - convert to a JMP */
					switch (opline->opcode) {
						case ZEND_BRK:
							MAKE_NOP(opline);
							ZEND_OP1(opline).opline_num = jmp_to->brk;
							break;
						case ZEND_CONT:
							MAKE_NOP(opline);
							ZEND_OP1(opline).opline_num = jmp_to->cont;
							break;
					}
					opline->opcode = ZEND_JMP;
					/* MAKE_NOP() already set op1 and op2 to IS_UNUSED */
				}
				break;
		}
		opline++;
	}
}
