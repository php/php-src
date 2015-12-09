/*
   +----------------------------------------------------------------------+
   | Zend Engine, Bytecode Visualisation                                  |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_compile.h"
#include "zend_cfg.h"
#include "zend_dump.h"

static void zend_dump_const(const zval *zv)
{
	switch (Z_TYPE_P(zv)) {
		case IS_NULL:
			fprintf(stderr, " null");
			break;
		case IS_FALSE:
			fprintf(stderr, " bool(false)");
			break;
		case IS_TRUE:
			fprintf(stderr, " bool(true)");
			break;
		case IS_LONG:
			fprintf(stderr, " int(" ZEND_LONG_FMT ")", Z_LVAL_P(zv));
			break;
		case IS_DOUBLE:
			fprintf(stderr, " float(%g)", Z_DVAL_P(zv));
			break;
		case IS_STRING:
			fprintf(stderr, " string(\"%s\")", Z_STRVAL_P(zv));
			break;
		case IS_ARRAY:
			fprintf(stderr, " array(...)");
			break;
		default:
			fprintf(stderr, " ???");
			break;
	}
}

static void zend_dump_op(const zend_op_array *op_array, const zend_basic_block *b, const zend_op *opline)
{
	const char *name = zend_get_opcode_name(opline->opcode);
	uint32_t flags = zend_get_opcode_flags(opline->opcode);
	uint32_t n = 0;

	if (!b) {
		fprintf(stderr, "L%u:", (uint32_t)(opline - op_array->opcodes));
	}
	fprintf(stderr, "\t%s", name ? (name + 5) : "???");
	if (ZEND_VM_OP1_JMP_ADDR & flags) {
		if (b) {
			fprintf(stderr, " BB%d", b->successors[n++]);
		} else {
			fprintf(stderr, " L%u", (uint32_t)(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes));
		}
	} else if (ZEND_VM_OP1_NUM & flags) {
		fprintf(stderr, " %u", opline->op1.num);
	} else if (opline->op1_type == IS_CONST) {
		zend_dump_const(CT_CONSTANT_EX(op_array, opline->op1.constant));
	} else if (opline->op1_type == IS_CV) {
		fprintf(stderr, " CV%u", EX_VAR_TO_NUM(opline->op1.var));
	} else if (opline->op1_type == IS_VAR) {
		fprintf(stderr, " V%u", EX_VAR_TO_NUM(opline->op1.var));
	} else if ( opline->op1_type == IS_TMP_VAR) {
		fprintf(stderr, " T%u", EX_VAR_TO_NUM(opline->op1.var));
	}
	if (ZEND_VM_OP2_JMP_ADDR & flags) {
		if (b) {
			fprintf(stderr, " BB%d", b->successors[n++]);
		} else {
			fprintf(stderr, " L%u", (uint32_t)(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes));
		}
	} else if (ZEND_VM_OP2_NUM & flags) {
		fprintf(stderr, " %u", opline->op2.num);
	} else if (opline->op2_type == IS_CONST) {
		zend_dump_const(CT_CONSTANT_EX(op_array, opline->op2.constant));
	} else if (opline->op2_type == IS_CV) {
		fprintf(stderr, " CV%u", EX_VAR_TO_NUM(opline->op2.var));
	} else if (opline->op2_type == IS_VAR) {
		fprintf(stderr, " V%u", EX_VAR_TO_NUM(opline->op2.var));
	} else if ( opline->op2_type == IS_TMP_VAR) {
		fprintf(stderr, " T%u", EX_VAR_TO_NUM(opline->op2.var));
	}
	if (ZEND_VM_EXT_JMP_ADDR & flags) {
		if (opline->opcode != ZEND_CATCH || !opline->result.num) {
			if (b) {
				fprintf(stderr, " BB%d", b->successors[n++]);
			} else {
				fprintf(stderr, " L%u", (uint32_t)ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
			}
		}
	}
	if (opline->result_type == IS_CONST) {
		zend_dump_const(CT_CONSTANT_EX(op_array, opline->result.constant));
	} else if (opline->result_type == IS_CV) {
		fprintf(stderr, " -> CV%u", EX_VAR_TO_NUM(opline->result.var));
	} else if (opline->result_type & IS_VAR) {
		if (opline->result_type & EXT_TYPE_UNUSED) {
			fprintf(stderr, " -> U%u", EX_VAR_TO_NUM(opline->result.var));
		} else {
			fprintf(stderr, " -> V%u", EX_VAR_TO_NUM(opline->result.var));
		}
	} else if ( opline->result_type == IS_TMP_VAR) {
		fprintf(stderr, " -> T%u", EX_VAR_TO_NUM(opline->result.var));
	}
	fprintf(stderr, "\n");
}

void zend_dump_op_array(const zend_op_array *op_array, const zend_cfg *cfg, uint32_t dump_flags, const char *msg)
{
	int i;

	if (op_array->function_name) {
		if (op_array->scope && op_array->scope->name) {
			fprintf(stderr, "\n%s::%s", op_array->scope->name->val, op_array->function_name->val);
		} else {
			fprintf(stderr, "\n%s", op_array->function_name->val);
		}
	} else {
		fprintf(stderr, "\n%s", "$_main");
	}
	fprintf(stderr, ": ; (lines=%d, args=%d, vars=%d, tmps=%d)\n",
		op_array->last,
		op_array->num_args,
		op_array->last_var,
		op_array->T);
	if (msg) {
		fprintf(stderr, "    ; (%s)\n", msg);
	}
	fprintf(stderr, "    ; %s:%u-%u\n", op_array->filename->val, op_array->line_start, op_array->line_end);

	if (cfg) {
		int n;
		zend_basic_block *b;

		for (n = 0; n < cfg->blocks_count; n++) {
			b = cfg->blocks + n;
			if ((dump_flags & ZEND_DUMP_UNREACHABLE) || (b->flags & ZEND_BB_REACHABLE)) {
				const zend_op *opline;
				const zend_op *end;
				int printed = 0;

				fprintf(stderr, "BB%d:", n);
				if (b->flags & ZEND_BB_START) {
					fprintf(stderr, " start");
				}
				if (b->flags & ZEND_BB_FOLLOW) {
					fprintf(stderr, " follow");
				}
				if (b->flags & ZEND_BB_TARGET) {
					fprintf(stderr, " target");
				}
				if (b->flags & ZEND_BB_EXIT) {
					fprintf(stderr, " exit");
				}
				if (b->flags & ZEND_BB_ENTRY) {
					fprintf(stderr, " entry");
				}
				if (b->flags & ZEND_BB_TRY) {
					fprintf(stderr, " try");
				}
				if (b->flags & ZEND_BB_CATCH) {
					fprintf(stderr, " catch");
				}
				if (b->flags & ZEND_BB_FINALLY) {
					fprintf(stderr, " finally");
				}
				if (b->flags & ZEND_BB_FINALLY_END) {
					fprintf(stderr, " finally_end");
				}
				if (b->flags & ZEND_BB_GEN_VAR) {
					fprintf(stderr, " gen_var");
				}
				if (b->flags & ZEND_BB_KILL_VAR) {
					fprintf(stderr, " kill_var");
				}
				if ((dump_flags & ZEND_DUMP_UNREACHABLE) & !(b->flags & ZEND_BB_REACHABLE)) {
					fprintf(stderr, " unreachable");
				}

				if (b->predecessors_count) {
					int *p = cfg->predecessors + b->predecessor_offset;
					int *end = p + b->predecessors_count;

					fprintf(stderr, " from=(BB%d", *p);
					for (p++; p < end; p++) {
						fprintf(stderr, ", BB%d", *p);
					}
					fprintf(stderr, ")");
				}

				if (b->successors[0] != -1) {
					fprintf(stderr, " to=(BB%d", b->successors[0]);
					printed = 1;
					if (b->successors[1] != -1) {
						fprintf(stderr, ", BB%d", b->successors[1]);
					}
				}
				if (printed) {
					fprintf(stderr, ")");
				}
				fprintf(stderr, "\n");

				if (!(b->flags & ZEND_BB_EMPTY)) {
					opline = op_array->opcodes + b->start;
					end = op_array->opcodes + b->end + 1;
					while (opline < end) {
						zend_dump_op(op_array, b, opline);
						opline++;
					}
				}
			}
		}
		if (op_array->last_live_range) {
			fprintf(stderr, "LIVE RANGES:\n");
			for (i = 0; i < op_array->last_live_range; i++) {
				fprintf(stderr, "\t%u: BB%u - BB%u\n",
					EX_VAR_TO_NUM(op_array->live_range[i].var & ~ZEND_LIVE_MASK),
					cfg->map[op_array->live_range[i].start],
					cfg->map[op_array->live_range[i].end]);
			}
		}
		if (op_array->last_try_catch) {
			fprintf(stderr, "EXCEPTION TABLE:\n");
			for (i = 0; i < op_array->last_try_catch; i++) {
				fprintf(stderr, "\tBB%u",
					cfg->map[op_array->try_catch_array[i].try_op]);
				if (op_array->try_catch_array[i].catch_op) {
					fprintf(stderr, ", BB%u",
						cfg->map[op_array->try_catch_array[i].catch_op]);
				} else {
					fprintf(stderr, ", -");
				}
				if (op_array->try_catch_array[i].finally_op) {
					fprintf(stderr, ", BB%u",
						cfg->map[op_array->try_catch_array[i].finally_op]);
				} else {
					fprintf(stderr, ", -");
				}
				if (op_array->try_catch_array[i].finally_end) {
					fprintf(stderr, ", BB%u\n",
						cfg->map[op_array->try_catch_array[i].finally_end]);
				} else {
					fprintf(stderr, ", -\n");
				}
			}
		}
	} else {
		const zend_op *opline = op_array->opcodes;
		const zend_op *end = opline + op_array->last;

		while (opline < end) {
			zend_dump_op(op_array, NULL, opline);
			opline++;
		}
		if (op_array->last_live_range) {
			fprintf(stderr, "LIVE RANGES:\n");
			for (i = 0; i < op_array->last_live_range; i++) {
				fprintf(stderr, "\t%u: L%u - L%u\n",
					EX_VAR_TO_NUM(op_array->live_range[i].var & ~ZEND_LIVE_MASK),
					op_array->live_range[i].start,
					op_array->live_range[i].end);
			}
		}
		if (op_array->last_try_catch) {
			fprintf(stderr, "EXCEPTION TABLE:\n");
			for (i = 0; i < op_array->last_try_catch; i++) {
				fprintf(stderr, "\tL%u",
					op_array->try_catch_array[i].try_op);
				if (op_array->try_catch_array[i].catch_op) {
					fprintf(stderr, ", L%u",
						op_array->try_catch_array[i].catch_op);
				} else {
					fprintf(stderr, ", -");
				}
				if (op_array->try_catch_array[i].finally_op) {
					fprintf(stderr, ", L%u",
						op_array->try_catch_array[i].finally_op);
				} else {
					fprintf(stderr, ", -");
				}
				if (op_array->try_catch_array[i].finally_end) {
					fprintf(stderr, ", L%u\n",
						op_array->try_catch_array[i].finally_end);
				} else {
					fprintf(stderr, ", -\n");
				}
			}
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
