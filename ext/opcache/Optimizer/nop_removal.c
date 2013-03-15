/* pass 10:
 * - remove NOPs
 */

static void nop_removal(zend_op_array *op_array)
{
	zend_op *end, *opline;
	zend_uint new_count, i, shift;
	int j;
	zend_uint *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (zend_uint *)DO_ALLOCA(sizeof(zend_uint) * op_array->last);
	i = new_count = shift = 0;
	end = op_array->opcodes + op_array->last;
	for (opline = op_array->opcodes; opline < end; opline++) {

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		/* GOTO target is unresolved yet. We can't optimize. */
		if (opline->opcode == ZEND_GOTO &&
			Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_LONG) {
			/* TODO: in general we can avoid this restriction */
			FREE_ALLOCA(shiftlist);
			return;
		}
#endif

		/* Kill JMP-over-NOP-s */
		if (opline->opcode == ZEND_JMP && ZEND_OP1(opline).opline_num > i) {
			/* check if there are only NOPs under the branch */
			zend_op *target = op_array->opcodes + ZEND_OP1(opline).opline_num - 1;

			while (target->opcode == ZEND_NOP) {
				target--;
			}
			if (target == opline) {
				/* only NOPs */
				opline->opcode = ZEND_NOP;
			}
		}

		shiftlist[i++] = shift;
		if (opline->opcode == ZEND_NOP) {
			shift++;
		} else {
			if (shift) {
				op_array->opcodes[new_count] = *opline;
			}
			new_count++;
		}
	}

	if (shift) {
		op_array->last = new_count;
		end = op_array->opcodes + op_array->last;

		/* update JMPs */
		for (opline = op_array->opcodes; opline<end; opline++) {
			switch (opline->opcode) {
				case ZEND_JMP:
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
				case ZEND_GOTO:
#endif
#if ZEND_EXTENSION_API_NO > PHP_5_4_X_API_NO
				case ZEND_FAST_CALL:
#endif
					ZEND_OP1(opline).opline_num -= shiftlist[ZEND_OP1(opline).opline_num];
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_FE_FETCH:
				case ZEND_FE_RESET:
				case ZEND_NEW:
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
				case ZEND_JMP_SET:
#endif
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
				case ZEND_JMP_SET_VAR:
#endif
					ZEND_OP2(opline).opline_num -= shiftlist[ZEND_OP2(opline).opline_num];
					break;
				case ZEND_JMPZNZ:
					ZEND_OP2(opline).opline_num -= shiftlist[ZEND_OP2(opline).opline_num];
					opline->extended_value -= shiftlist[opline->extended_value];
					break;
				case ZEND_CATCH:
					opline->extended_value -= shiftlist[opline->extended_value];
					break;
			}
		}

		/* update brk/cont array */
		for (j = 0; j < op_array->last_brk_cont; j++) {
			op_array->brk_cont_array[j].brk -= shiftlist[op_array->brk_cont_array[j].brk];
			op_array->brk_cont_array[j].cont -= shiftlist[op_array->brk_cont_array[j].cont];
			op_array->brk_cont_array[j].start -= shiftlist[op_array->brk_cont_array[j].start];
		}

		/* update try/catch array */
		for (j = 0; j < op_array->last_try_catch; j++) {
			op_array->try_catch_array[j].try_op -= shiftlist[op_array->try_catch_array[j].try_op];
			op_array->try_catch_array[j].catch_op -= shiftlist[op_array->try_catch_array[j].catch_op];
#if ZEND_EXTENSION_API_NO > PHP_5_4_X_API_NO
			if (op_array->try_catch_array[j].finally_op) {
				op_array->try_catch_array[j].finally_op -= shiftlist[op_array->try_catch_array[j].finally_op];
				op_array->try_catch_array[j].finally_end -= shiftlist[op_array->try_catch_array[j].finally_end];
			}
#endif
		}

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
		/* update early binding list */
		if (op_array->early_binding != (zend_uint)-1) {
			zend_uint *opline_num = &op_array->early_binding;

			do {
				*opline_num -= shiftlist[*opline_num];
				opline_num = &ZEND_RESULT(&op_array->opcodes[*opline_num]).opline_num;
			} while (*opline_num != (zend_uint)-1);
		}
#endif
	}
	FREE_ALLOCA(shiftlist);
}
