/*
   +----------------------------------------------------------------------+
   | Zend OPcache, Escape Analysis                                        |
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
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_bitset.h"
#include "zend_cfg.h"
#include "zend_ssa.h"
#include "zend_inference.h"
#include "zend_dump.h"

/*
 * T. Kotzmann and H. Mossenbock. Escape analysis  in the context of dynamic
 * compilation and deoptimization. In Proceedings of the International
 * Conference on Virtual Execution Environments, pages 111-120, Chicago,
 * June 2005
 */

static zend_always_inline void union_find_init(int *parent, int *size, int count) /* {{{ */
{
	int i;

	for (i = 0; i < count; i++) {
		parent[i] = i;
		size[i] = 1;
	}
}
/* }}} */

static zend_always_inline int union_find_root(int *parent, int i) /* {{{ */
{
	int p = parent[i];

	while (i != p) {
		p = parent[p];
		parent[i] = p;
		i = p;
		p = parent[i];
	}
	return i;
}
/* }}} */

static zend_always_inline void union_find_unite(int *parent, int *size, int i, int j) /* {{{ */
{
	int r1 = union_find_root(parent, i);
	int r2 = union_find_root(parent, j);

	if (r1 != r2) {
		if (size[r1] < size[r2]) {
			parent[r1] = r2;
			size[r2] += size[r1];
		} else {
			parent[r2] = r1;
			size[r1] += size[r2];
		}
	}
}
/* }}} */

static int zend_build_equi_escape_sets(int *parent, zend_op_array *op_array, zend_ssa *ssa) /* {{{ */
{
	zend_ssa_var *ssa_vars = ssa->vars;
	int ssa_vars_count = ssa->vars_count;
	zend_ssa_phi *p;
	int i, j;
	int *size;
	ALLOCA_FLAG(use_heap)

	size = do_alloca(sizeof(int) * ssa_vars_count, use_heap);
	if (!size) {
		return FAILURE;
	}
	union_find_init(parent, size, ssa_vars_count);

	for (i = 0; i < ssa_vars_count; i++) {
		if (ssa_vars[i].definition_phi) {
			p = ssa_vars[i].definition_phi;
			if (p->pi >= 0) {
				union_find_unite(parent, size, i, p->sources[0]);
			} else {
				for (j = 0; j < ssa->cfg.blocks[p->block].predecessors_count; j++) {
					union_find_unite(parent, size, i, p->sources[j]);
				}
			}
		} else if (ssa_vars[i].definition >= 0) {
			int def = ssa_vars[i].definition;
			zend_ssa_op *op = ssa->ops + def;
			zend_op *opline =  op_array->opcodes + def;

			if (op->op1_def >= 0) {
				if (op->op1_use >= 0) {
					if (opline->opcode != ZEND_ASSIGN) {
						union_find_unite(parent, size, op->op1_def, op->op1_use);
					}
				}
				if (opline->opcode == ZEND_ASSIGN && op->op2_use >= 0) {
					union_find_unite(parent, size, op->op1_def, op->op2_use);
				}
			}
			if (op->op2_def >= 0) {
				if (op->op2_use >= 0) {
					union_find_unite(parent, size, op->op2_def, op->op2_use);
				}
			}
			if (op->result_def >= 0) {
				if (op->result_use >= 0) {
					if (opline->opcode != ZEND_QM_ASSIGN) {
						union_find_unite(parent, size, op->result_def, op->result_use);
					}
				}
				if (opline->opcode == ZEND_QM_ASSIGN && op->op1_use >= 0) {
					union_find_unite(parent, size, op->result_def, op->op1_use);
				}
				if (opline->opcode == ZEND_ASSIGN && op->op2_use >= 0) {
					union_find_unite(parent, size, op->result_def, op->op2_use);
				}
				if (opline->opcode == ZEND_ASSIGN && op->op1_def >= 0) {
					union_find_unite(parent, size, op->result_def, op->op1_def);
				}
			}
		}
	}

	for (i = 0; i < ssa_vars_count; i++) {
		parent[i] = union_find_root(parent, i);
	}

	free_alloca(size, use_heap);

	return SUCCESS;
}
/* }}} */

static inline zend_class_entry *get_class_entry(const zend_script *script, zend_string *lcname) /* {{{ */
{
	zend_class_entry *ce = script ? zend_hash_find_ptr(&script->class_table, lcname) : NULL;
	if (ce) {
		return ce;
	}

	ce = zend_hash_find_ptr(CG(class_table), lcname);
	if (ce && ce->type == ZEND_INTERNAL_CLASS) {
		return ce;
	}

	return NULL;
}
/* }}} */

static int is_allocation_def(zend_op_array *op_array, zend_ssa *ssa, int def, int var, const zend_script *script) /* {{{ */
{
	zend_ssa_op *ssa_op = ssa->ops + def;
	zend_op *opline = op_array->opcodes + def;

	if (ssa_op->result_def == var) {
		switch (opline->opcode) {
			case ZEND_INIT_ARRAY:
				return 1;
			case ZEND_NEW:
			    /* objects with destructors should escape */
				if (opline->op1_type == IS_CONST) {
					zend_class_entry *ce = get_class_entry(script, Z_STR_P(CRT_CONSTANT(opline->op1)+1));
					uint32_t forbidden_flags =
						/* These flags will always cause an exception */
						ZEND_ACC_IMPLICIT_ABSTRACT_CLASS | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS
						| ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT;
					if (ce && !ce->parent && !ce->create_object && !ce->constructor &&
					    !ce->destructor && !ce->__get && !ce->__set &&
					    !(ce->ce_flags & forbidden_flags) &&
						(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
						return 1;
					}
				}
				break;
			case ZEND_QM_ASSIGN:
				if (opline->op1_type == IS_CONST
				 && Z_TYPE_P(CRT_CONSTANT(opline->op1)) == IS_ARRAY) {
					return 1;
				}
				if (opline->op1_type == IS_CV && (OP1_INFO() & MAY_BE_ARRAY)) {
					return 1;
				}
				break;
			case ZEND_ASSIGN:
				if (opline->op1_type == IS_CV && (OP1_INFO() & MAY_BE_ARRAY)) {
					return 1;
				}
				break;
		}
    } else if (ssa_op->op1_def == var) {
		switch (opline->opcode) {
			case ZEND_ASSIGN:
				if (opline->op2_type == IS_CONST
				 && Z_TYPE_P(CRT_CONSTANT(opline->op2)) == IS_ARRAY) {
					return 1;
				}
				if (opline->op2_type == IS_CV && (OP2_INFO() & MAY_BE_ARRAY)) {
					return 1;
				}
				break;
			case ZEND_ASSIGN_DIM:
				if (OP1_INFO() & (MAY_BE_UNDEF | MAY_BE_NULL | MAY_BE_FALSE)) {
					/* implicit object/array allocation */
					return 1;
				}
				break;
		}
	}

    return 0;
}
/* }}} */

static int is_local_def(zend_op_array *op_array, zend_ssa *ssa, int def, int var, const zend_script *script) /* {{{ */
{
	zend_ssa_op *op = ssa->ops + def;
	zend_op *opline = op_array->opcodes + def;

	if (op->result_def == var) {
		switch (opline->opcode) {
			case ZEND_INIT_ARRAY:
			case ZEND_ADD_ARRAY_ELEMENT:
			case ZEND_QM_ASSIGN:
			case ZEND_ASSIGN:
				return 1;
			case ZEND_NEW:
				/* objects with destructors should escape */
				if (opline->op1_type == IS_CONST) {
					zend_class_entry *ce = get_class_entry(script, Z_STR_P(CRT_CONSTANT(opline->op1)+1));
					if (ce && !ce->create_object && !ce->constructor &&
					    !ce->destructor && !ce->__get && !ce->__set && !ce->parent) {
						return 1;
					}
				}
				break;
		}
	} else if (op->op1_def == var) {
		switch (opline->opcode) {
			case ZEND_ASSIGN:
			case ZEND_ASSIGN_DIM:
			case ZEND_ASSIGN_OBJ:
			case ZEND_ASSIGN_OBJ_REF:
			case ZEND_ASSIGN_DIM_OP:
			case ZEND_ASSIGN_OBJ_OP:
			case ZEND_PRE_INC_OBJ:
			case ZEND_PRE_DEC_OBJ:
			case ZEND_POST_INC_OBJ:
			case ZEND_POST_DEC_OBJ:
				return 1;
		}
	}

	return 0;
}
/* }}} */

static int is_escape_use(zend_op_array *op_array, zend_ssa *ssa, int use, int var) /* {{{ */
{
	zend_ssa_op *ssa_op = ssa->ops + use;
	zend_op *opline = op_array->opcodes + use;

	if (ssa_op->op1_use == var) {
		switch (opline->opcode) {
			case ZEND_ASSIGN:
				/* no_val */
				break;
			case ZEND_QM_ASSIGN:
				if (opline->op1_type == IS_CV) {
					if (OP1_INFO() & MAY_BE_OBJECT) {
						/* object aliasing */
						return 1;
					}
				}
				break;
			case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			case ZEND_ISSET_ISEMPTY_PROP_OBJ:
			case ZEND_FETCH_DIM_R:
			case ZEND_FETCH_OBJ_R:
			case ZEND_FETCH_DIM_IS:
			case ZEND_FETCH_OBJ_IS:
				break;
			case ZEND_ASSIGN_OP:
				return 1;
			case ZEND_ASSIGN_DIM_OP:
			case ZEND_ASSIGN_OBJ_OP:
			case ZEND_ASSIGN_STATIC_PROP_OP:
			case ZEND_ASSIGN_DIM:
			case ZEND_ASSIGN_OBJ:
			case ZEND_ASSIGN_OBJ_REF:
				break;
			case ZEND_PRE_INC_OBJ:
			case ZEND_PRE_DEC_OBJ:
			case ZEND_POST_INC_OBJ:
			case ZEND_POST_DEC_OBJ:
				break;
			case ZEND_INIT_ARRAY:
			case ZEND_ADD_ARRAY_ELEMENT:
				if (opline->extended_value & ZEND_ARRAY_ELEMENT_REF) {
					return 1;
				}
				if (OP1_INFO() & MAY_BE_OBJECT) {
					/* object aliasing */
					return 1;
				}
				/* reference dependencies processed separately */
				break;
			case ZEND_OP_DATA:
				if ((opline-1)->opcode != ZEND_ASSIGN_DIM
				 && (opline-1)->opcode != ZEND_ASSIGN_OBJ) {
					return 1;
				}
				if (OP1_INFO() & MAY_BE_OBJECT) {
					/* object aliasing */
					return 1;
				}
				opline--;
				ssa_op--;
				if (opline->op1_type != IS_CV
				 || (OP1_INFO() & MAY_BE_REF)
				 || (ssa_op->op1_def >= 0 && ssa->vars[ssa_op->op1_def].alias)) {
					/* assignment into escaping structure */
					return 1;
				}
				/* reference dependencies processed separately */
				break;
			default:
				return 1;
		}
	}

	if (ssa_op->op2_use == var) {
		switch (opline->opcode) {
			case ZEND_ASSIGN:
				if (opline->op1_type != IS_CV
				 || (OP1_INFO() & MAY_BE_REF)
				 || (ssa_op->op1_def >= 0 && ssa->vars[ssa_op->op1_def].alias)) {
					/* assignment into escaping variable */
					return 1;
				}
				if (opline->op2_type == IS_CV || opline->result_type != IS_UNUSED) {
					if (OP2_INFO() & MAY_BE_OBJECT) {
						/* object aliasing */
						return 1;
					}
				}
				break;
			default:
				return 1;
		}
	}

	if (ssa_op->result_use == var) {
		switch (opline->opcode) {
			case ZEND_ASSIGN:
			case ZEND_QM_ASSIGN:
			case ZEND_INIT_ARRAY:
			case ZEND_ADD_ARRAY_ELEMENT:
				break;
			default:
				return 1;
		}
	}

	return 0;
}
/* }}} */

int zend_ssa_escape_analysis(const zend_script *script, zend_op_array *op_array, zend_ssa *ssa) /* {{{ */
{
	zend_ssa_var *ssa_vars = ssa->vars;
	int ssa_vars_count = ssa->vars_count;
	int i, root, use;
	int *ees;
	zend_bool has_allocations;
	int num_non_escaped;
	ALLOCA_FLAG(use_heap)

	if (!ssa_vars) {
		return SUCCESS;
	}

	has_allocations = 0;
	for (i = op_array->last_var; i < ssa_vars_count; i++) {
		if (ssa_vars[i].definition >= 0
		  && (ssa->var_info[i].type & (MAY_BE_ARRAY|MAY_BE_OBJECT))
		  && is_allocation_def(op_array, ssa, ssa_vars[i].definition, i, script)) {
			has_allocations = 1;
			break;
		}
	}
	if (!has_allocations) {
		return SUCCESS;
	}


	/* 1. Build EES (Equi-Escape Sets) */
	ees = do_alloca(sizeof(int) * ssa_vars_count, use_heap);
	if (!ees) {
		return FAILURE;
	}

	if (zend_build_equi_escape_sets(ees, op_array, ssa) != SUCCESS) {
		return FAILURE;
	}

	/* 2. Identify Allocations */
	num_non_escaped = 0;
	for (i = op_array->last_var; i < ssa_vars_count; i++) {
		root = ees[i];
		if (ssa_vars[root].escape_state > ESCAPE_STATE_NO_ESCAPE) {
			/* already escape. skip */
		} else if (ssa_vars[i].alias && (ssa->var_info[i].type & MAY_BE_REF)) {
			if (ssa_vars[root].escape_state == ESCAPE_STATE_NO_ESCAPE) {
				num_non_escaped--;
			}
			ssa_vars[root].escape_state = ESCAPE_STATE_GLOBAL_ESCAPE;
		} else if (ssa_vars[i].definition >= 0
			 && (ssa->var_info[i].type & (MAY_BE_ARRAY|MAY_BE_OBJECT))) {
			if (!is_local_def(op_array, ssa, ssa_vars[i].definition, i, script)) {
				if (ssa_vars[root].escape_state == ESCAPE_STATE_NO_ESCAPE) {
					num_non_escaped--;
				}
				ssa_vars[root].escape_state = ESCAPE_STATE_GLOBAL_ESCAPE;
			} else if (ssa_vars[root].escape_state == ESCAPE_STATE_UNKNOWN
			 && is_allocation_def(op_array, ssa, ssa_vars[i].definition, i, script)) {
				ssa_vars[root].escape_state = ESCAPE_STATE_NO_ESCAPE;
				num_non_escaped++;
			}
		}
	}

	/* 3. Mark escaped EES */
	if (num_non_escaped) {
		for (i = 0; i < ssa_vars_count; i++) {
			if (ssa_vars[i].use_chain >= 0) {
				root = ees[i];
				if (ssa_vars[root].escape_state == ESCAPE_STATE_NO_ESCAPE) {
					FOREACH_USE(ssa_vars + i, use) {
						if (is_escape_use(op_array, ssa, use, i)) {
							ssa_vars[root].escape_state = ESCAPE_STATE_GLOBAL_ESCAPE;
							num_non_escaped--;
							if (num_non_escaped == 0) {
								i = ssa_vars_count;
							}
							break;
						}
					} FOREACH_USE_END();
				}
			}
		}
	}

	/* 4. Process referential dependencies */
	if (num_non_escaped) {
		zend_bool changed;

		do {
			changed = 0;
			for (i = 0; i < ssa_vars_count; i++) {
				if (ssa_vars[i].use_chain >= 0) {
					root = ees[i];
					if (ssa_vars[root].escape_state == ESCAPE_STATE_NO_ESCAPE) {
						FOREACH_USE(ssa_vars + i, use) {
							zend_ssa_op *op = ssa->ops + use;
							zend_op *opline = op_array->opcodes + use;
							int enclosing_root;

							if (opline->opcode == ZEND_OP_DATA &&
							    ((opline-1)->opcode == ZEND_ASSIGN_DIM ||
							     (opline-1)->opcode == ZEND_ASSIGN_OBJ ||
							     (opline-1)->opcode == ZEND_ASSIGN_OBJ_REF) &&
							    op->op1_use == i &&
							    (op-1)->op1_use >= 0) {
								enclosing_root = ees[(op-1)->op1_use];
							} else if ((opline->opcode == ZEND_INIT_ARRAY ||
							     opline->opcode == ZEND_ADD_ARRAY_ELEMENT) &&
							    op->op1_use == i &&
							    op->result_def >= 0) {
								enclosing_root = ees[op->result_def];
							} else {
								continue;
							}

							if (ssa_vars[enclosing_root].escape_state == ESCAPE_STATE_UNKNOWN ||
							    ssa_vars[enclosing_root].escape_state > ssa_vars[root].escape_state) {
							    if (ssa_vars[enclosing_root].escape_state == ESCAPE_STATE_UNKNOWN) {
									ssa_vars[root].escape_state = ESCAPE_STATE_GLOBAL_ESCAPE;
							    } else {
									ssa_vars[root].escape_state = ssa_vars[enclosing_root].escape_state;
								}
								if (ssa_vars[root].escape_state == ESCAPE_STATE_GLOBAL_ESCAPE) {
									num_non_escaped--;
									if (num_non_escaped == 0) {
										changed = 0;
									} else {
										changed = 1;
									}
									break;
								} else {
									changed = 1;
								}
							}
						} FOREACH_USE_END();
					}
				}
			}
		} while (changed);
	}

	/* 5. Propagate values of escape sets to variables */
	for (i = 0; i < ssa_vars_count; i++) {
		root = ees[i];
		if (i != root) {
			ssa_vars[i].escape_state = ssa_vars[root].escape_state;
		}
	}

	free_alloca(ees, use_heap);

	return SUCCESS;
}
/* }}} */
