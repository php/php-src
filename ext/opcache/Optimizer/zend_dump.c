/*
   +----------------------------------------------------------------------+
   | Zend Engine, Bytecode Visualisation                                  |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_compile.h"
#include "zend_cfg.h"
#include "zend_ssa.h"
#include "zend_inference.h"
#include "zend_func_info.h"
#include "zend_call_graph.h"
#include "zend_dump.h"

void zend_dump_ht(HashTable *ht)
{
	zend_ulong index;
	zend_string *key;
	zval *val;
	int first = 1;

	ZEND_HASH_FOREACH_KEY_VAL(ht, index, key, val) {
		if (first) {
			first = 0;
		} else {
			fprintf(stderr, ", ");
		}
		if (key) {
			fprintf(stderr, "\"%s\"", ZSTR_VAL(key));
		} else {
			fprintf(stderr, ZEND_LONG_FMT, index);
		}
		fprintf(stderr, " =>");
		zend_dump_const(val);
	} ZEND_HASH_FOREACH_END();
}

void zend_dump_const(const zval *zv)
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
			fprintf(stderr, " zval(type=%d)", Z_TYPE_P(zv));
			break;
	}
}

static void zend_dump_class_fetch_type(uint32_t fetch_type)
{
	switch (fetch_type & ZEND_FETCH_CLASS_MASK) {
		case ZEND_FETCH_CLASS_SELF:
			fprintf(stderr, " (self)");
			break;
		case ZEND_FETCH_CLASS_PARENT:
			fprintf(stderr, " (parent)");
			break;
		case ZEND_FETCH_CLASS_STATIC:
			fprintf(stderr, " (static)");
			break;
		case ZEND_FETCH_CLASS_AUTO:
			fprintf(stderr, " (auto)");
			break;
		case ZEND_FETCH_CLASS_INTERFACE:
			fprintf(stderr, " (interface)");
			break;
		case ZEND_FETCH_CLASS_TRAIT:
			fprintf(stderr, " (trait)");
			break;
	}
	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
			fprintf(stderr, " (no-autolod)");
	}
	if (fetch_type & ZEND_FETCH_CLASS_SILENT) {
			fprintf(stderr, " (silent)");
	}
	if (fetch_type & ZEND_FETCH_CLASS_EXCEPTION) {
			fprintf(stderr, " (exception)");
	}
}

static void zend_dump_unused_op(const zend_op *opline, znode_op op, uint32_t flags) {
	if (ZEND_VM_OP_NUM == (flags & ZEND_VM_OP_MASK)) {
		fprintf(stderr, " %u", op.num);
	} else if (ZEND_VM_OP_TRY_CATCH == (flags & ZEND_VM_OP_MASK)) {
		if (op.num != (uint32_t)-1) {
			fprintf(stderr, " try-catch(%u)", op.num);
		}
	} else if (ZEND_VM_OP_THIS == (flags & ZEND_VM_OP_MASK)) {
		fprintf(stderr, " THIS");
	} else if (ZEND_VM_OP_NEXT == (flags & ZEND_VM_OP_MASK)) {
		fprintf(stderr, " NEXT");
	} else if (ZEND_VM_OP_CLASS_FETCH == (flags & ZEND_VM_OP_MASK)) {
		zend_dump_class_fetch_type(op.num);
	} else if (ZEND_VM_OP_CONSTRUCTOR == (flags & ZEND_VM_OP_MASK)) {
		fprintf(stderr, " CONSTRUCTOR");
	} else if (ZEND_VM_OP_CONST_FETCH == (flags & ZEND_VM_OP_MASK)) {
		if (op.num & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
			fprintf(stderr, " (unqualified-in-namespace)");
		}
	}
}

void zend_dump_var(const zend_op_array *op_array, zend_uchar var_type, int var_num)
{
	if (var_type == IS_CV && var_num < op_array->last_var) {
		fprintf(stderr, "CV%d($%s)", var_num, op_array->vars[var_num]->val);
	} else if (var_type == IS_VAR) {
		fprintf(stderr, "V%d", var_num);
	} else if ((var_type & (IS_VAR|IS_TMP_VAR)) == IS_TMP_VAR) {
		fprintf(stderr, "T%d", var_num);
	} else {
		fprintf(stderr, "X%d", var_num);
	}
}

static void zend_dump_range(const zend_ssa_range *r)
{
	if (r->underflow && r->overflow) {
		return;
	}
	fprintf(stderr, " RANGE[");
	if (r->underflow) {
		fprintf(stderr, "--..");
	} else {
		fprintf(stderr, ZEND_LONG_FMT "..", r->min);
	}
	if (r->overflow) {
		fprintf(stderr, "++]");
	} else {
		fprintf(stderr, ZEND_LONG_FMT "]", r->max);
	}
}

static void zend_dump_type_info(uint32_t info, zend_class_entry *ce, int is_instanceof, uint32_t dump_flags)
{
	int first = 1;

	fprintf(stderr, " [");
	if (info & MAY_BE_UNDEF) {
		if (first) first = 0; else fprintf(stderr, ", ");
		fprintf(stderr, "undef");
	}
	if (info & MAY_BE_REF) {
		if (first) first = 0; else fprintf(stderr, ", ");
		fprintf(stderr, "ref");
	}
	if (dump_flags & ZEND_DUMP_RC_INFERENCE) {
		if (info & MAY_BE_RC1) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "rc1");
		}
		if (info & MAY_BE_RCN) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "rcn");
		}
	}
	if (info & MAY_BE_CLASS) {
		if (first) first = 0; else fprintf(stderr, ", ");
		fprintf(stderr, "class");
		if (ce) {
			if (is_instanceof) {
				fprintf(stderr, " (instanceof %s)", ce->name->val);
			} else {
				fprintf(stderr, " (%s)", ce->name->val);
			}
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_ANY) {
		if (first) first = 0; else fprintf(stderr, ", ");
		fprintf(stderr, "any");
	} else {
		if (info & MAY_BE_NULL) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "null");
		}
		if ((info & MAY_BE_FALSE) && (info & MAY_BE_TRUE)) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "bool");
		} else if (info & MAY_BE_FALSE) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "false");
		} else if (info & MAY_BE_TRUE) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "true");
		}
		if (info & MAY_BE_LONG) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "long");
		}
		if (info & MAY_BE_DOUBLE) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "double");
		}
		if (info & MAY_BE_STRING) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "string");
		}
		if (info & MAY_BE_ARRAY) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "array");
			if ((info & MAY_BE_ARRAY_KEY_ANY) != 0 &&
			    (info & MAY_BE_ARRAY_KEY_ANY) != MAY_BE_ARRAY_KEY_ANY) {
				int afirst = 1;
				fprintf(stderr, " [");
				if (info & MAY_BE_ARRAY_KEY_LONG) {
					if (afirst) afirst = 0; else fprintf(stderr, ", ");
					fprintf(stderr, "long");
				}
				if (info & MAY_BE_ARRAY_KEY_STRING) {
					if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "string");
					}
				fprintf(stderr, "]");
			}
			if (info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF)) {
				int afirst = 1;
				fprintf(stderr, " of [");
				if ((info & MAY_BE_ARRAY_OF_ANY) == MAY_BE_ARRAY_OF_ANY) {
					if (afirst) afirst = 0; else fprintf(stderr, ", ");
					fprintf(stderr, "any");
				} else {
					if (info & MAY_BE_ARRAY_OF_NULL) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "null");
					}
					if (info & MAY_BE_ARRAY_OF_FALSE) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "false");
					}
					if (info & MAY_BE_ARRAY_OF_TRUE) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "true");
					}
					if (info & MAY_BE_ARRAY_OF_LONG) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "long");
					}
					if (info & MAY_BE_ARRAY_OF_DOUBLE) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "double");
					}
					if (info & MAY_BE_ARRAY_OF_STRING) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "string");
					}
					if (info & MAY_BE_ARRAY_OF_ARRAY) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "array");
					}
					if (info & MAY_BE_ARRAY_OF_OBJECT) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "object");
					}
					if (info & MAY_BE_ARRAY_OF_RESOURCE) {
						if (afirst) afirst = 0; else fprintf(stderr, ", ");
						fprintf(stderr, "resource");
					}
				}
				if (info & MAY_BE_ARRAY_OF_REF) {
					if (afirst) afirst = 0; else fprintf(stderr, ", ");
					fprintf(stderr, "ref");
				}
				fprintf(stderr, "]");
			}
		}
		if (info & MAY_BE_OBJECT) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "object");
			if (ce) {
				if (is_instanceof) {
					fprintf(stderr, " (instanceof %s)", ce->name->val);
				} else {
					fprintf(stderr, " (%s)", ce->name->val);
				}
			}
		}
		if (info & MAY_BE_RESOURCE) {
			if (first) first = 0; else fprintf(stderr, ", ");
			fprintf(stderr, "resource");
		}
	}
//TODO: this is useful only for JIT???
	if (info & MAY_BE_IN_REG) {
		if (first) first = 0; else fprintf(stderr, ", ");
		fprintf(stderr, "reg");
	}
	fprintf(stderr, "]");
}

static void zend_dump_ssa_var_info(const zend_ssa *ssa, int ssa_var_num, uint32_t dump_flags)
{
	zend_dump_type_info(
		ssa->var_info[ssa_var_num].type,
		ssa->var_info[ssa_var_num].ce,
		ssa->var_info[ssa_var_num].ce ?
			ssa->var_info[ssa_var_num].is_instanceof : 0,
		dump_flags);
}

static void zend_dump_ssa_var(const zend_op_array *op_array, const zend_ssa *ssa, int ssa_var_num, zend_uchar var_type, int var_num, uint32_t dump_flags)
{
	if (ssa_var_num >= 0) {
		fprintf(stderr, "#%d.", ssa_var_num);
	} else {
		fprintf(stderr, "#?.");
	}
	zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : var_type), var_num);

	if (ssa_var_num >= 0 && ssa->vars) {
		if (ssa->vars[ssa_var_num].no_val) {
			fprintf(stderr, " NOVAL");
		}
		if (ssa->vars[ssa_var_num].escape_state == ESCAPE_STATE_NO_ESCAPE) {
			fprintf(stderr, " NOESC");
		}
		if (ssa->var_info) {
			zend_dump_ssa_var_info(ssa, ssa_var_num, dump_flags);
			if (ssa->var_info[ssa_var_num].has_range) {
				zend_dump_range(&ssa->var_info[ssa_var_num].range);
			}
		}
	}
}

static void zend_dump_type_constraint(const zend_op_array *op_array, const zend_ssa *ssa, const zend_ssa_type_constraint *constraint, uint32_t dump_flags)
{
	fprintf(stderr, " TYPE");
	zend_dump_type_info(constraint->type_mask, constraint->ce, 1, dump_flags);
}

static void zend_dump_range_constraint(const zend_op_array *op_array, const zend_ssa *ssa, const zend_ssa_range_constraint *r, uint32_t dump_flags)
{
	if (r->range.underflow && r->range.overflow) {
		return;
	}
	fprintf(stderr, " RANGE");
	if (r->negative) {
		fprintf(stderr, "~");
	}
	fprintf(stderr, "[");
	if (r->range.underflow) {
		fprintf(stderr, "-- .. ");
	} else {
		if (r->min_ssa_var >= 0) {
			zend_dump_ssa_var(op_array, ssa, r->min_ssa_var, (r->min_var < op_array->last_var ? IS_CV : 0), r->min_var, dump_flags);
			if (r->range.min > 0) {
				fprintf(stderr, " + " ZEND_LONG_FMT, r->range.min);
			} else if (r->range.min < 0) {
				fprintf(stderr, " - " ZEND_LONG_FMT, -r->range.min);
			}
			fprintf(stderr, " .. ");
		} else {
			fprintf(stderr, ZEND_LONG_FMT " .. ", r->range.min);
		}
	}
	if (r->range.overflow) {
		fprintf(stderr, "++]");
	} else {
		if (r->max_ssa_var >= 0) {
			zend_dump_ssa_var(op_array, ssa, r->max_ssa_var, (r->max_var < op_array->last_var ? IS_CV : 0), r->max_var, dump_flags);
			if (r->range.max > 0) {
				fprintf(stderr, " + " ZEND_LONG_FMT, r->range.max);
			} else if (r->range.max < 0) {
				fprintf(stderr, " - " ZEND_LONG_FMT, -r->range.max);
			}
			fprintf(stderr, "]");
		} else {
			fprintf(stderr, ZEND_LONG_FMT "]", r->range.max);
		}
	}
}

static void zend_dump_op(const zend_op_array *op_array, const zend_basic_block *b, const zend_op *opline, uint32_t dump_flags, const void *data)
{
	const char *name = zend_get_opcode_name(opline->opcode);
	uint32_t flags = zend_get_opcode_flags(opline->opcode);
	uint32_t n = 0;
	int len = 0;
	const zend_ssa *ssa = NULL;

	if (dump_flags & ZEND_DUMP_SSA) {
		ssa = (const zend_ssa*)data;
	}

	if (!b) {
		len = fprintf(stderr, "L%u (%u):", (uint32_t)(opline - op_array->opcodes), opline->lineno);
	}
	fprintf(stderr, "%*c", 12-len, ' ');

	if (!ssa || !ssa->ops || ssa->ops[opline - op_array->opcodes].result_use < 0) {
		if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			if (ssa && ssa->ops && ssa->ops[opline - op_array->opcodes].result_def >= 0) {
				int ssa_var_num = ssa->ops[opline - op_array->opcodes].result_def;
				zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->result_type, EX_VAR_TO_NUM(opline->result.var), dump_flags);
			} else {
				zend_dump_var(op_array, opline->result_type, EX_VAR_TO_NUM(opline->result.var));
			}
			fprintf(stderr, " = ");
		}
	}

	if (name) {
		fprintf(stderr, "%s", (name + 5));
	} else {
		fprintf(stderr, "OP_%d", (int)opline->opcode);
	}

	if (ZEND_VM_EXT_NUM == (flags & ZEND_VM_EXT_MASK)) {
		fprintf(stderr, " %u", opline->extended_value);
	} else if (ZEND_VM_EXT_OP == (flags & ZEND_VM_EXT_MASK)) {
		fprintf(stderr, " (%s)", zend_get_opcode_name(opline->extended_value) + 5);
	} else if (ZEND_VM_EXT_TYPE == (flags & ZEND_VM_EXT_MASK)) {
		switch (opline->extended_value) {
			case IS_NULL:
				fprintf(stderr, " (null)");
				break;
			case IS_FALSE:
				fprintf(stderr, " (false)");
				break;
			case IS_TRUE:
				fprintf(stderr, " (true)");
				break;
			case IS_LONG:
				fprintf(stderr, " (long)");
				break;
			case IS_DOUBLE:
				fprintf(stderr, " (double)");
				break;
			case IS_STRING:
				fprintf(stderr, " (string)");
				break;
			case IS_ARRAY:
				fprintf(stderr, " (array)");
				break;
			case IS_OBJECT:
				fprintf(stderr, " (object)");
				break;
			case IS_RESOURCE:
				fprintf(stderr, " (resource)");
				break;
			case _IS_BOOL:
				fprintf(stderr, " (bool)");
				break;
			case IS_CALLABLE:
				fprintf(stderr, " (callable)");
				break;
			case IS_VOID:
				fprintf(stderr, " (void)");
				break;
			default:
				fprintf(stderr, " (\?\?\?)");
				break;
		}
	} else if (ZEND_VM_EXT_TYPE_MASK == (flags & ZEND_VM_EXT_MASK)) {
		switch (opline->extended_value) {
			case (1<<IS_NULL):
				fprintf(stderr, " (null)");
				break;
			case (1<<IS_FALSE):
				fprintf(stderr, " (false)");
				break;
			case (1<<IS_TRUE):
				fprintf(stderr, " (true)");
				break;
			case (1<<IS_LONG):
				fprintf(stderr, " (long)");
				break;
			case (1<<IS_DOUBLE):
				fprintf(stderr, " (double)");
				break;
			case (1<<IS_STRING):
				fprintf(stderr, " (string)");
				break;
			case (1<<IS_ARRAY):
				fprintf(stderr, " (array)");
				break;
			case (1<<IS_OBJECT):
				fprintf(stderr, " (object)");
				break;
			case (1<<IS_RESOURCE):
				fprintf(stderr, " (resource)");
				break;
			case ((1<<IS_FALSE)|(1<<IS_TRUE)):
				fprintf(stderr, " (bool)");
				break;
			default:
				fprintf(stderr, " (\?\?\?)");
				break;
		}
	} else if (ZEND_VM_EXT_EVAL == (flags & ZEND_VM_EXT_MASK)) {
		switch (opline->extended_value) {
			case ZEND_EVAL:
				fprintf(stderr, " (eval)");
				break;
			case ZEND_INCLUDE:
				fprintf(stderr, " (include)");
				break;
			case ZEND_INCLUDE_ONCE:
				fprintf(stderr, " (include_once)");
				break;
			case ZEND_REQUIRE:
				fprintf(stderr, " (require)");
				break;
			case ZEND_REQUIRE_ONCE:
				fprintf(stderr, " (require_once)");
				break;
			default:
				fprintf(stderr, " (\?\?\?)");
				break;
		}
	} else if (ZEND_VM_EXT_SRC == (flags & ZEND_VM_EXT_MASK)) {
		if (opline->extended_value == ZEND_RETURNS_VALUE) {
			fprintf(stderr, " (value)");
		} else if (opline->extended_value & ZEND_RETURNS_FUNCTION) {
			fprintf(stderr, " (function)");
		}
	} else {
		if (ZEND_VM_EXT_VAR_FETCH & flags) {
			if (opline->extended_value & ZEND_FETCH_GLOBAL) {
				fprintf(stderr, " (global)");
			} else if (opline->extended_value & ZEND_FETCH_LOCAL) {
				fprintf(stderr, " (local)");
			} else if (opline->extended_value & ZEND_FETCH_GLOBAL_LOCK) {
				fprintf(stderr, " (global+lock)");
			}
		}
		if (ZEND_VM_EXT_ISSET & flags) {
			if (!(opline->extended_value & ZEND_ISEMPTY)) {
				fprintf(stderr, " (isset)");
			} else {
				fprintf(stderr, " (empty)");
			}
		}
		if (ZEND_VM_EXT_ARRAY_INIT & flags) {
			fprintf(stderr, " %u", opline->extended_value >> ZEND_ARRAY_SIZE_SHIFT);
			if (!(opline->extended_value & ZEND_ARRAY_NOT_PACKED)) {
				fprintf(stderr, " (packed)");
			}
		}
		if (ZEND_VM_EXT_REF & flags) {
			if (opline->extended_value & ZEND_ARRAY_ELEMENT_REF) {
				fprintf(stderr, " (ref)");
			}
		}
		if ((ZEND_VM_EXT_DIM_WRITE|ZEND_VM_EXT_FETCH_REF) & flags) {
			uint32_t obj_flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;
			if (obj_flags == ZEND_FETCH_REF) {
				fprintf(stderr, " (ref)");
			} else if (obj_flags == ZEND_FETCH_DIM_WRITE) {
				fprintf(stderr, " (dim write)");
			}
		}
	}

	if (opline->op1_type == IS_CONST) {
		zend_dump_const(CRT_CONSTANT(opline->op1));
	} else if (opline->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
		if (ssa && ssa->ops) {
			int ssa_var_num = ssa->ops[opline - op_array->opcodes].op1_use;
			if (ssa_var_num >= 0) {
				fprintf(stderr, " ");
				zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->op1_type, EX_VAR_TO_NUM(opline->op1.var), dump_flags);
			} else if (ssa->ops[opline - op_array->opcodes].op1_def < 0) {
				fprintf(stderr, " ");
				zend_dump_var(op_array, opline->op1_type, EX_VAR_TO_NUM(opline->op1.var));
			}
		} else {
			fprintf(stderr, " ");
			zend_dump_var(op_array, opline->op1_type, EX_VAR_TO_NUM(opline->op1.var));
		}
		if (ssa && ssa->ops) {
			int ssa_var_num = ssa->ops[opline - op_array->opcodes].op1_def;
			if (ssa_var_num >= 0) {
				fprintf(stderr, " -> ");
				zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->op1_type, EX_VAR_TO_NUM(opline->op1.var), dump_flags);
			}
		}
	} else {
		uint32_t op1_flags = ZEND_VM_OP1_FLAGS(flags);
		if (ZEND_VM_OP_JMP_ADDR == (op1_flags & ZEND_VM_OP_MASK)) {
			if (b) {
				fprintf(stderr, " BB%d", b->successors[n++]);
			} else {
				fprintf(stderr, " L%u", (uint32_t)(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes));
			}
		} else {
			zend_dump_unused_op(opline, opline->op1, op1_flags);
		}
	}

	if (opline->op2_type == IS_CONST) {
		zval *op = CRT_CONSTANT(opline->op2);
		if (opline->opcode == ZEND_SWITCH_LONG || opline->opcode == ZEND_SWITCH_STRING) {
			HashTable *jumptable = Z_ARRVAL_P(op);
			zend_string *key;
			zend_ulong num_key;
			zval *zv;
			ZEND_HASH_FOREACH_KEY_VAL(jumptable, num_key, key, zv) {
				if (key) {
					fprintf(stderr, " \"%s\":", ZSTR_VAL(key));
				} else {
					fprintf(stderr, " " ZEND_LONG_FMT ":", num_key);
				}
				if (b) {
					fprintf(stderr, " BB%d,", b->successors[n++]);
				} else {
					fprintf(stderr, " L%u,", (uint32_t)ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv)));
				}
			} ZEND_HASH_FOREACH_END();
			fprintf(stderr, " default:");
		} else {
			zend_dump_const(op);
		}
	} else if (opline->op2_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
		if (ssa && ssa->ops) {
			int ssa_var_num = ssa->ops[opline - op_array->opcodes].op2_use;
			if (ssa_var_num >= 0) {
				fprintf(stderr, " ");
				zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->op2_type, EX_VAR_TO_NUM(opline->op2.var), dump_flags);
			} else if (ssa->ops[opline - op_array->opcodes].op2_def < 0) {
				fprintf(stderr, " ");
				zend_dump_var(op_array, opline->op2_type, EX_VAR_TO_NUM(opline->op2.var));
			}
		} else {
			fprintf(stderr, " ");
			zend_dump_var(op_array, opline->op2_type, EX_VAR_TO_NUM(opline->op2.var));
		}
		if (ssa && ssa->ops) {
			int ssa_var_num = ssa->ops[opline - op_array->opcodes].op2_def;
			if (ssa_var_num >= 0) {
				fprintf(stderr, " -> ");
				zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->op2_type, EX_VAR_TO_NUM(opline->op2.var), dump_flags);
			}
		}
	} else {
		uint32_t op2_flags = ZEND_VM_OP2_FLAGS(flags);
		if (ZEND_VM_OP_JMP_ADDR == (op2_flags & ZEND_VM_OP_MASK)) {
			if (opline->opcode != ZEND_CATCH || !(opline->extended_value & ZEND_LAST_CATCH)) {
				if (b) {
					fprintf(stderr, " BB%d", b->successors[n++]);
				} else {
					fprintf(stderr, " L%u", (uint32_t)(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes));
				}
			}
		} else {
			zend_dump_unused_op(opline, opline->op2, op2_flags);
		}
	}

	if (ZEND_VM_EXT_JMP_ADDR == (flags & ZEND_VM_EXT_MASK)) {
		if (b) {
			fprintf(stderr, " BB%d", b->successors[n++]);
		} else {
			fprintf(stderr, " L%u", (uint32_t)ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
		}
	}
	if (opline->result_type == IS_CONST) {
		zend_dump_const(CRT_CONSTANT(opline->result));
#if 0
	} else if (opline->result_type & IS_SMART_BRANCH_JMPZ) {
		fprintf(stderr, " jmpz");
	} else if (opline->result_type & IS_SMART_BRANCH_JMPNZ) {
		fprintf(stderr, " jmpnz");
#endif
	} else if (ssa && ssa->ops && ssa->ops[opline - op_array->opcodes].result_use >= 0) {
		if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			if (ssa && ssa->ops) {
				int ssa_var_num = ssa->ops[opline - op_array->opcodes].result_use;
				if (ssa_var_num >= 0) {
					fprintf(stderr, " ");
					zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->result_type, EX_VAR_TO_NUM(opline->result.var), dump_flags);
				}
			} else {
				fprintf(stderr, " ");
				zend_dump_var(op_array, opline->result_type, EX_VAR_TO_NUM(opline->result.var));
			}
			if (ssa && ssa->ops) {
				int ssa_var_num = ssa->ops[opline - op_array->opcodes].result_def;
				if (ssa_var_num >= 0) {
					fprintf(stderr, " -> ");
					zend_dump_ssa_var(op_array, ssa, ssa_var_num, opline->result_type, EX_VAR_TO_NUM(opline->result.var), dump_flags);
				}
			}
		}
	}
	fprintf(stderr, "\n");
}

static void zend_dump_block_info(const zend_cfg *cfg, int n, uint32_t dump_flags)
{
	zend_basic_block *b = cfg->blocks + n;

	fprintf(stderr, "BB%d:", n);
	if (b->flags & ZEND_BB_START) {
		fprintf(stderr, " start");
	}
	if (b->flags & ZEND_BB_RECV_ENTRY) {
		fprintf(stderr, " recv");
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
	if (b->flags & (ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
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
	if (!(dump_flags & ZEND_DUMP_HIDE_UNREACHABLE) && !(b->flags & ZEND_BB_REACHABLE)) {
		fprintf(stderr, " unreachable");
	}
	if (b->flags & ZEND_BB_UNREACHABLE_FREE) {
		fprintf(stderr, " unreachable_free");
	}
	if (b->flags & ZEND_BB_LOOP_HEADER) {
		fprintf(stderr, " loop_header");
	}
	if (b->flags & ZEND_BB_IRREDUCIBLE_LOOP) {
		fprintf(stderr, " irreducible");
	}
	if (b->len != 0) {
		fprintf(stderr, " lines=[%d-%d]", b->start, b->start + b->len - 1);
	} else {
		fprintf(stderr, " empty");
	}
	fprintf(stderr, "\n");

	if (b->predecessors_count) {
		int *p = cfg->predecessors + b->predecessor_offset;
		int *end = p + b->predecessors_count;

		fprintf(stderr, "    ; from=(BB%d", *p);
		for (p++; p < end; p++) {
			fprintf(stderr, ", BB%d", *p);
		}
		fprintf(stderr, ")\n");
	}

	if (b->successors_count > 0) {
		int s;
		fprintf(stderr, "    ; to=(BB%d", b->successors[0]);
		for (s = 1; s < b->successors_count; s++) {
			fprintf(stderr, ", BB%d", b->successors[s]);
		}
		fprintf(stderr, ")\n");
	}

	if (b->idom >= 0) {
		fprintf(stderr, "    ; idom=BB%d\n", b->idom);
	}
	if (b->level >= 0) {
		fprintf(stderr, "    ; level=%d\n", b->level);
	}
	if (b->loop_header >= 0) {
		fprintf(stderr, "    ; loop_header=%d\n", b->loop_header);
	}
	if (b->children >= 0) {
		int j = b->children;
		fprintf(stderr, "    ; children=(BB%d", j);
		j = cfg->blocks[j].next_child;
		while (j >= 0) {
			fprintf(stderr, ", BB%d", j);
			j = cfg->blocks[j].next_child;
		}
		fprintf(stderr, ")\n");
	}
}

static void zend_dump_block_header(const zend_cfg *cfg, const zend_op_array *op_array, const zend_ssa *ssa, int n, uint32_t dump_flags)
{
	zend_dump_block_info(cfg, n, dump_flags);
	if (ssa && ssa->blocks && ssa->blocks[n].phis) {
		zend_ssa_phi *p = ssa->blocks[n].phis;

		do {
			int j;

			fprintf(stderr, "        ");
			zend_dump_ssa_var(op_array, ssa, p->ssa_var, 0, p->var, dump_flags);
			if (p->pi < 0) {
				fprintf(stderr, " = Phi(");
				for (j = 0; j < cfg->blocks[n].predecessors_count; j++) {
					if (j > 0) {
						fprintf(stderr, ", ");
					}
					zend_dump_ssa_var(op_array, ssa, p->sources[j], 0, p->var, dump_flags);
				}
				fprintf(stderr, ")\n");
			} else {
				fprintf(stderr, " = Pi<BB%d>(", p->pi);
				zend_dump_ssa_var(op_array, ssa, p->sources[0], 0, p->var, dump_flags);
				fprintf(stderr, " &");
				if (p->has_range_constraint) {
					zend_dump_range_constraint(op_array, ssa, &p->constraint.range, dump_flags);
				} else {
					zend_dump_type_constraint(op_array, ssa, &p->constraint.type, dump_flags);
				}
				fprintf(stderr, ")\n");
			}
			p = p->next;
		} while (p);
	}
}

void zend_dump_op_array_name(const zend_op_array *op_array)
{
	zend_func_info *func_info = NULL;

	func_info = ZEND_FUNC_INFO(op_array);
	if (op_array->function_name) {
		if (op_array->scope && op_array->scope->name) {
			fprintf(stderr, "%s::%s", op_array->scope->name->val, op_array->function_name->val);
		} else {
			fprintf(stderr, "%s", op_array->function_name->val);
		}
	} else {
		fprintf(stderr, "%s", "$_main");
	}
	if (func_info && func_info->clone_num > 0) {
		fprintf(stderr, "_@_clone_%d", func_info->clone_num);
	}
}

void zend_dump_op_array(const zend_op_array *op_array, uint32_t dump_flags, const char *msg, const void *data)
{
	int i;
	const zend_cfg *cfg = NULL;
	const zend_ssa *ssa = NULL;
	zend_func_info *func_info = NULL;
	uint32_t func_flags = 0;

	if (dump_flags & (ZEND_DUMP_CFG|ZEND_DUMP_SSA)) {
		cfg = (const zend_cfg*)data;
		if (!cfg->blocks) {
			cfg = data = NULL;
		}
	}
	if (dump_flags & ZEND_DUMP_SSA) {
		ssa = (const zend_ssa*)data;
	}

	func_info = ZEND_FUNC_INFO(op_array);
	if (func_info) {
		func_flags = func_info->flags;
	}

	fprintf(stderr, "\n");
	zend_dump_op_array_name(op_array);
	fprintf(stderr, ": ; (lines=%d, args=%d",
		op_array->last,
		op_array->num_args);
	if (func_info && func_info->num_args >= 0) {
		fprintf(stderr, "/%d", func_info->num_args);
	}
	fprintf(stderr, ", vars=%d, tmps=%d", op_array->last_var, op_array->T);
	if (ssa) {
		fprintf(stderr, ", ssa_vars=%d", ssa->vars_count);
	}
	if (func_flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) {
		fprintf(stderr, ", dynamic");
	}
	if (func_flags & ZEND_FUNC_RECURSIVE) {
		fprintf(stderr, ", recursive");
		if (func_flags & ZEND_FUNC_RECURSIVE_DIRECTLY) {
			fprintf(stderr, " directly");
		}
		if (func_flags & ZEND_FUNC_RECURSIVE_INDIRECTLY) {
			fprintf(stderr, " indirectly");
		}
	}
	if (func_flags & ZEND_FUNC_IRREDUCIBLE) {
		fprintf(stderr, ", irreducable");
	}
	if (func_flags & ZEND_FUNC_NO_LOOPS) {
		fprintf(stderr, ", no_loops");
	}
	if (func_flags & ZEND_FUNC_HAS_EXTENDED_STMT) {
		fprintf(stderr, ", extended_stmt");
	}
	if (func_flags & ZEND_FUNC_HAS_EXTENDED_FCALL) {
		fprintf(stderr, ", extended_fcall");
	}
//TODO: this is useful only for JIT???
#if 0
	if (info->flags & ZEND_JIT_FUNC_NO_IN_MEM_CVS) {
		fprintf(stderr, ", no_in_mem_cvs");
	}
	if (info->flags & ZEND_JIT_FUNC_NO_USED_ARGS) {
		fprintf(stderr, ", no_used_args");
	}
	if (info->flags & ZEND_JIT_FUNC_NO_SYMTAB) {
		fprintf(stderr, ", no_symtab");
	}
	if (info->flags & ZEND_JIT_FUNC_NO_FRAME) {
		fprintf(stderr, ", no_frame");
	}
	if (info->flags & ZEND_JIT_FUNC_INLINE) {
		fprintf(stderr, ", inline");
	}
#endif
	if (func_info && func_info->return_value_used == 0) {
		fprintf(stderr, ", no_return_value");
	} else if (func_info && func_info->return_value_used == 1) {
		fprintf(stderr, ", return_value");
	}
	fprintf(stderr, ")\n");
	if (msg) {
		fprintf(stderr, "    ; (%s)\n", msg);
	}
	fprintf(stderr, "    ; %s:%u-%u\n", op_array->filename->val, op_array->line_start, op_array->line_end);

	if (func_info && func_info->num_args > 0) {
		uint32_t j;

		for (j = 0; j < MIN(op_array->num_args, func_info->num_args ); j++) {
			fprintf(stderr, "    ; arg %d ", j);
			zend_dump_type_info(func_info->arg_info[j].info.type, func_info->arg_info[j].info.ce, func_info->arg_info[j].info.is_instanceof, dump_flags);
			zend_dump_range(&func_info->arg_info[j].info.range);
			fprintf(stderr, "\n");
		}
	}

	if (func_info) {
		fprintf(stderr, "    ; return ");
		zend_dump_type_info(func_info->return_info.type, func_info->return_info.ce, func_info->return_info.is_instanceof, dump_flags);
		zend_dump_range(&func_info->return_info.range);
		fprintf(stderr, "\n");
	}

	if (ssa && ssa->var_info) {
		for (i = 0; i < op_array->last_var; i++) {
			fprintf(stderr, "    ; ");
			zend_dump_ssa_var(op_array, ssa, i, IS_CV, i, dump_flags);
			fprintf(stderr, "\n");
		}
	}

	if (cfg) {
		int n;
		zend_basic_block *b;

		for (n = 0; n < cfg->blocks_count; n++) {
			b = cfg->blocks + n;
			if (!(dump_flags & ZEND_DUMP_HIDE_UNREACHABLE) || (b->flags & ZEND_BB_REACHABLE)) {
				const zend_op *opline;
				const zend_op *end;

				zend_dump_block_header(cfg, op_array, ssa, n, dump_flags);
				opline = op_array->opcodes + b->start;
				end = opline + b->len;
				while (opline < end) {
					zend_dump_op(op_array, b, opline, dump_flags, data);
					opline++;
				}
			}
		}
		if (op_array->last_live_range && (dump_flags & ZEND_DUMP_LIVE_RANGES)) {
			fprintf(stderr, "LIVE RANGES:\n");
			for (i = 0; i < op_array->last_live_range; i++) {
				fprintf(stderr, "        %u: L%u - L%u ",
					EX_VAR_TO_NUM(op_array->live_range[i].var & ~ZEND_LIVE_MASK),
					op_array->live_range[i].start,
					op_array->live_range[i].end);
				switch (op_array->live_range[i].var & ZEND_LIVE_MASK) {
					case ZEND_LIVE_TMPVAR:
						fprintf(stderr, "(tmp/var)\n");
						break;
					case ZEND_LIVE_LOOP:
						fprintf(stderr, "(loop)\n");
						break;
					case ZEND_LIVE_SILENCE:
						fprintf(stderr, "(silence)\n");
						break;
					case ZEND_LIVE_ROPE:
						fprintf(stderr, "(rope)\n");
						break;
					case ZEND_LIVE_NEW:
						fprintf(stderr, "(new)\n");
						break;
				}
			}
		}
		if (op_array->last_try_catch) {
			fprintf(stderr, "EXCEPTION TABLE:\n");
			for (i = 0; i < op_array->last_try_catch; i++) {
				fprintf(stderr, "        BB%u",
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
			zend_dump_op(op_array, NULL, opline, dump_flags, data);
			opline++;
		}
		if (op_array->last_live_range && (dump_flags & ZEND_DUMP_LIVE_RANGES)) {
			fprintf(stderr, "LIVE RANGES:\n");
			for (i = 0; i < op_array->last_live_range; i++) {
				fprintf(stderr, "        %u: L%u - L%u ",
					EX_VAR_TO_NUM(op_array->live_range[i].var & ~ZEND_LIVE_MASK),
					op_array->live_range[i].start,
					op_array->live_range[i].end);
				switch (op_array->live_range[i].var & ZEND_LIVE_MASK) {
					case ZEND_LIVE_TMPVAR:
						fprintf(stderr, "(tmp/var)\n");
						break;
					case ZEND_LIVE_LOOP:
						fprintf(stderr, "(loop)\n");
						break;
					case ZEND_LIVE_SILENCE:
						fprintf(stderr, "(silence)\n");
						break;
					case ZEND_LIVE_ROPE:
						fprintf(stderr, "(rope)\n");
						break;
					case ZEND_LIVE_NEW:
						fprintf(stderr, "(new)\n");
						break;
				}
			}
		}
		if (op_array->last_try_catch) {
			fprintf(stderr, "EXCEPTION TABLE:\n");
			for (i = 0; i < op_array->last_try_catch; i++) {
				fprintf(stderr, "        L%u",
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

void zend_dump_dominators(const zend_op_array *op_array, const zend_cfg *cfg)
{
	int j;

	fprintf(stderr, "\nDOMINATORS-TREE for \"");
	zend_dump_op_array_name(op_array);
	fprintf(stderr, "\"\n");
	for (j = 0; j < cfg->blocks_count; j++) {
		zend_basic_block *b = cfg->blocks + j;
		if (b->flags & ZEND_BB_REACHABLE) {
			zend_dump_block_info(cfg, j, 0);
		}
	}
}

void zend_dump_variables(const zend_op_array *op_array)
{
	int j;

	fprintf(stderr, "\nCV Variables for \"");
	zend_dump_op_array_name(op_array);
	fprintf(stderr, "\"\n");
	for (j = 0; j < op_array->last_var; j++) {
		fprintf(stderr, "    ");
		zend_dump_var(op_array, IS_CV, j);
		fprintf(stderr, "\n");
	}
}

void zend_dump_ssa_variables(const zend_op_array *op_array, const zend_ssa *ssa, uint32_t dump_flags)
{
	int j;

	if (ssa->vars) {
		fprintf(stderr, "\nSSA Variable for \"");
		zend_dump_op_array_name(op_array);
		fprintf(stderr, "\"\n");

		for (j = 0; j < ssa->vars_count; j++) {
			fprintf(stderr, "    ");
			zend_dump_ssa_var(op_array, ssa, j, IS_CV, ssa->vars[j].var, dump_flags);
			if (ssa->vars[j].scc >= 0) {
				if (ssa->vars[j].scc_entry) {
					fprintf(stderr, " *");
				}  else {
					fprintf(stderr, "  ");
				}
				fprintf(stderr, "SCC=%d", ssa->vars[j].scc);
			}
			fprintf(stderr, "\n");
		}
	}
}

static void zend_dump_var_set(const zend_op_array *op_array, const char *name, zend_bitset set)
{
	int first = 1;
	uint32_t i;

	fprintf(stderr, "    ; %s = {", name);
	for (i = 0; i < op_array->last_var + op_array->T; i++) {
		if (zend_bitset_in(set, i)) {
			if (first) {
				first = 0;
			} else {
				fprintf(stderr, ", ");
			}
			zend_dump_var(op_array, IS_CV, i);
		}
	}
	fprintf(stderr, "}\n");
}

void zend_dump_dfg(const zend_op_array *op_array, const zend_cfg *cfg, const zend_dfg *dfg)
{
	int j;
	fprintf(stderr, "\nVariable Liveness for \"");
	zend_dump_op_array_name(op_array);
	fprintf(stderr, "\"\n");

	for (j = 0; j < cfg->blocks_count; j++) {
		fprintf(stderr, "  BB%d:\n", j);
		zend_dump_var_set(op_array, "def", DFG_BITSET(dfg->def, dfg->size, j));
		zend_dump_var_set(op_array, "use", DFG_BITSET(dfg->use, dfg->size, j));
		zend_dump_var_set(op_array, "in ", DFG_BITSET(dfg->in,  dfg->size, j));
		zend_dump_var_set(op_array, "out", DFG_BITSET(dfg->out, dfg->size, j));
	}
}

void zend_dump_phi_placement(const zend_op_array *op_array, const zend_ssa *ssa)
{
	int j;
	zend_ssa_block *ssa_blocks = ssa->blocks;
	int blocks_count = ssa->cfg.blocks_count;

	fprintf(stderr, "\nSSA Phi() Placement for \"");
	zend_dump_op_array_name(op_array);
	fprintf(stderr, "\"\n");
	for (j = 0; j < blocks_count; j++) {
		if (ssa_blocks && ssa_blocks[j].phis) {
			zend_ssa_phi *p = ssa_blocks[j].phis;
			int first = 1;

			fprintf(stderr, "  BB%d:\n", j);
			if (p->pi >= 0) {
				fprintf(stderr, "    ; pi={");
			} else {
				fprintf(stderr, "    ; phi={");
			}
			do {
				if (first) {
					first = 0;
				} else {
					fprintf(stderr, ", ");
				}
				zend_dump_var(op_array, IS_CV, p->var);
				p = p->next;
			} while (p);
			fprintf(stderr, "}\n");
		}
	}
}
