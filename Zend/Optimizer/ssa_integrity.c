/*
   +----------------------------------------------------------------------+
   | Zend Engine, SSA validation                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "Optimizer/zend_optimizer_internal.h"

/* The ssa_verify_integrity() function ensures that that certain invariants of the SSA form and
 * CFG are upheld and prints messages to stderr if this is not the case. */

static inline bool is_in_use_chain(zend_ssa *ssa, int var, int check) {
	int use;
	FOREACH_USE(&ssa->vars[var], use) {
		if (use == check) {
			return 1;
		}
	} FOREACH_USE_END();
	return 0;
}

static inline bool is_in_phi_use_chain(zend_ssa *ssa, int var, zend_ssa_phi *check) {
	zend_ssa_phi *phi;
	FOREACH_PHI_USE(&ssa->vars[var], phi) {
		if (phi == check) {
			return 1;
		}
	} FOREACH_PHI_USE_END();
	return 0;
}

static inline bool is_used_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_use == check)
		|| (ssa_op->op2_use == check)
		|| (ssa_op->result_use == check);
}

static inline bool is_defined_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_def == check)
		|| (ssa_op->op2_def == check)
		|| (ssa_op->result_def == check);
}

static inline bool is_in_phi_sources(zend_ssa *ssa, zend_ssa_phi *phi, int check) {
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (source == check) {
			return 1;
		}
	} FOREACH_PHI_SOURCE_END();
	return 0;
}

static inline bool is_in_predecessors(zend_cfg *cfg, zend_basic_block *block, int check) {
	int i, *predecessors = &cfg->predecessors[block->predecessor_offset];
	for (i = 0; i < block->predecessors_count; i++) {
		if (predecessors[i] == check) {
			return 1;
		}
	}
	return 0;
}

static inline bool is_in_successors(zend_basic_block *block, int check) {
	int s;
	for (s = 0; s < block->successors_count; s++) {
		if (block->successors[s] == check) {
			return 1;
		}
	}
	return 0;
}

static inline bool is_var_type(uint8_t type) {
	return (type & (IS_CV|IS_VAR|IS_TMP_VAR)) != 0;
}

static inline bool is_defined(const zend_ssa *ssa, const zend_op_array *op_array, int var) {
	const zend_ssa_var *ssa_var = &ssa->vars[var];
	return ssa_var->definition >= 0 || ssa_var->definition_phi || var < op_array->last_var;
}

#define FAIL(...) do { \
	if (status == SUCCESS) { \
		fprintf(stderr, "\nIn function %s::%s (%s):\n", \
			op_array->scope ? ZSTR_VAL(op_array->scope->name) : "", \
			op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}", extra); \
	} \
	fprintf(stderr, __VA_ARGS__); \
	status = FAILURE; \
} while (0)

#define VARFMT "%d (%s%s)"
#define VAR(i) \
	(i), (ssa->vars[i].var < op_array->last_var ? "CV $" : "TMP"), \
	(ssa->vars[i].var < op_array->last_var ? ZSTR_VAL(op_array->vars[ssa->vars[i].var]) : "")

#define INSTRFMT "%d (%s)"
#define INSTR(i) \
	(i), (zend_get_opcode_name(op_array->opcodes[i].opcode))

void ssa_verify_integrity(zend_op_array *op_array, zend_ssa *ssa, const char *extra) {
	zend_cfg *cfg = &ssa->cfg;
	zend_ssa_phi *phi;
	int i;
	zend_result status = SUCCESS;

	/* Vars */
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		int use, c;
		uint32_t type = ssa->var_info[i].type;

		if (var->definition < 0 && !var->definition_phi && i > op_array->last_var) {
			if (var->use_chain >= 0) {
				FAIL("var " VARFMT " without def has op uses\n", VAR(i));
			}
			if (var->phi_use_chain) {
				FAIL("var " VARFMT " without def has phi uses\n", VAR(i));
			}
		}
		if (var->definition >= 0 && var->definition_phi) {
			FAIL("var " VARFMT " has both def and def_phi\n", VAR(i));
		}
		if (var->definition >= 0) {
			if (!is_defined_by_op(ssa, var->definition, i)) {
				FAIL("var " VARFMT " not defined by op " INSTRFMT "\n",
						VAR(i), INSTR(var->definition));
			}
		}
		if (var->definition_phi) {
			if (var->definition_phi->ssa_var != i) {
				FAIL("var " VARFMT " not defined by given phi\n", VAR(i));
			}
		}

		c = 0;
		FOREACH_USE(var, use) {
			if (++c > 10000) {
				FAIL("cycle in uses of " VARFMT "\n", VAR(i));
				goto finish;
			}
			if (!is_used_by_op(ssa, use, i)) {
				fprintf(stderr, "var " VARFMT " not in uses of op %d\n", VAR(i), use);
			}
		} FOREACH_USE_END();

		c = 0;
		FOREACH_PHI_USE(var, phi) {
			if (++c > 10000) {
				FAIL("cycle in phi uses of " VARFMT "\n", VAR(i));
				goto finish;
			}
			if (!is_in_phi_sources(ssa, phi, i)) {
				FAIL("var " VARFMT " not in phi sources of %d\n", VAR(i), phi->ssa_var);
			}
		} FOREACH_PHI_USE_END();

		if ((type & MAY_BE_ARRAY_KEY_ANY) && !(type & MAY_BE_ARRAY_OF_ANY)) {
			FAIL("var " VARFMT " has array key type but not value type\n", VAR(i));
		}
		if ((type & MAY_BE_ARRAY_OF_ANY) && !(type & MAY_BE_ARRAY_KEY_ANY)) {
			FAIL("var " VARFMT " has array value type but not key type\n", VAR(i));
		}
		if ((type & MAY_BE_REF) && ssa->var_info[i].ce) {
			FAIL("var " VARFMT " may be ref but has ce\n", VAR(i));
		}
	}

	/* Instructions */
	FOREACH_INSTR_NUM(i) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		zend_op *opline = &op_array->opcodes[i];
		if (is_var_type(opline->op1_type)) {
			if (ssa_op->op1_use < 0 && ssa_op->op1_def < 0) {
				FAIL("var op1 of " INSTRFMT " does not use/def an ssa var\n", INSTR(i));
			}
		} else {
			if (ssa_op->op1_use >= 0 || ssa_op->op1_def >= 0) {
				FAIL("non-var op1 of " INSTRFMT " uses or defs an ssa var\n", INSTR(i));
			}
		}
		if (is_var_type(opline->op2_type)) {
			if (ssa_op->op2_use < 0 && ssa_op->op2_def < 0) {
				FAIL("var op2 of " INSTRFMT " does not use/def an ssa var\n", INSTR(i));
			}
		} else {
			if (ssa_op->op2_use >= 0 || ssa_op->op2_def >= 0) {
				FAIL("non-var op2 of " INSTRFMT " uses or defs an ssa var\n", INSTR(i));
			}
		}
		if (is_var_type(opline->result_type)) {
			if (ssa_op->result_use < 0 && ssa_op->result_def < 0) {
				FAIL("var result of " INSTRFMT " does not use/def an ssa var\n", INSTR(i));
			}
		} else {
			if (ssa_op->result_use >= 0 || ssa_op->result_def >= 0) {
				FAIL("non-var result of " INSTRFMT " uses or defs an ssa var\n", INSTR(i));
			}
		}

		if (ssa_op->op1_use >= 0) {
			if (ssa_op->op1_use >= ssa->vars_count) {
				FAIL("op1 use %d out of range\n", ssa_op->op1_use);
			}
			if (!is_defined(ssa, op_array, ssa_op->op1_use)) {
				FAIL("op1 use of " VARFMT " in " INSTRFMT " is not defined\n",
						VAR(ssa_op->op1_use), INSTR(i));
			}
			if (!is_in_use_chain(ssa, ssa_op->op1_use, i)) {
				FAIL("op1 use of " VARFMT " in " INSTRFMT " not in use chain\n",
						VAR(ssa_op->op1_use), INSTR(i));
			}
			if (VAR_NUM(opline->op1.var) != ssa->vars[ssa_op->op1_use].var) {
				FAIL("op1 use of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->op1_use), VAR_NUM(opline->op1.var), INSTR(i));
			}
		}
		if (ssa_op->op2_use >= 0) {
			if (ssa_op->op2_use >= ssa->vars_count) {
				FAIL("op2 use %d out of range\n", ssa_op->op2_use);
			}
			if (!is_defined(ssa, op_array, ssa_op->op2_use)) {
				FAIL("op2 use of " VARFMT " in " INSTRFMT " is not defined\n",
						VAR(ssa_op->op2_use), INSTR(i));
			}
			if (!is_in_use_chain(ssa, ssa_op->op2_use, i)) {
				FAIL("op2 use of " VARFMT " in " INSTRFMT " not in use chain\n",
						VAR(ssa_op->op2_use), INSTR(i));
			}
			if (VAR_NUM(opline->op2.var) != ssa->vars[ssa_op->op2_use].var) {
				FAIL("op2 use of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->op2_use), VAR_NUM(opline->op2.var), INSTR(i));
			}
		}
		if (ssa_op->result_use >= 0) {
			if (ssa_op->result_use >= ssa->vars_count) {
				FAIL("result use %d out of range\n", ssa_op->result_use);
			}
			if (!is_defined(ssa, op_array, ssa_op->result_use)) {
				FAIL("result use of " VARFMT " in " INSTRFMT " is not defined\n",
						VAR(ssa_op->result_use), INSTR(i));
			}
			if (!is_in_use_chain(ssa, ssa_op->result_use, i)) {
				FAIL("result use of " VARFMT " in " INSTRFMT " not in use chain\n",
					VAR(ssa_op->result_use), INSTR(i));
			}
			if (VAR_NUM(opline->result.var) != ssa->vars[ssa_op->result_use].var) {
				FAIL("result use of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->result_use), VAR_NUM(opline->result.var), INSTR(i));
			}
		}
		if (ssa_op->op1_def >= 0) {
			if (ssa_op->op1_def >= ssa->vars_count) {
				FAIL("op1 def %d out of range\n", ssa_op->op1_def);
			}
			if (ssa->vars[ssa_op->op1_def].definition != i) {
				FAIL("op1 def of " VARFMT " in " INSTRFMT " invalid\n",
						VAR(ssa_op->op1_def), INSTR(i));
			}
			if (VAR_NUM(opline->op1.var) != ssa->vars[ssa_op->op1_def].var) {
				FAIL("op1 def of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->op1_def), VAR_NUM(opline->op1.var), INSTR(i));
			}
		}
		if (ssa_op->op2_def >= 0) {
			if (ssa_op->op2_def >= ssa->vars_count) {
				FAIL("op2 def %d out of range\n", ssa_op->op2_def);
			}
			if (ssa->vars[ssa_op->op2_def].definition != i) {
				FAIL("op2 def of " VARFMT " in " INSTRFMT " invalid\n",
						VAR(ssa_op->op2_def), INSTR(i));
			}
			if (VAR_NUM(opline->op2.var) != ssa->vars[ssa_op->op2_def].var) {
				FAIL("op2 def of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->op2_def), VAR_NUM(opline->op2.var), INSTR(i));
			}
		}
		if (ssa_op->result_def >= 0) {
			if (ssa_op->result_def >= ssa->vars_count) {
				FAIL("result def %d out of range\n", ssa_op->result_def);
			}
			if (ssa->vars[ssa_op->result_def].definition != i) {
				FAIL("result def of " VARFMT " in " INSTRFMT " invalid\n",
						VAR(ssa_op->result_def), INSTR(i));
			}
			if (VAR_NUM(opline->result.var) != ssa->vars[ssa_op->result_def].var) {
				FAIL("result def of " VARFMT " does not match op %d of " INSTRFMT "\n",
						VAR(ssa_op->result_def), VAR_NUM(opline->result.var), INSTR(i));
			}
		}
	} FOREACH_INSTR_NUM_END();

	/* Phis */
	FOREACH_PHI(phi) {
		unsigned num_sources = NUM_PHI_SOURCES(phi);
		for (i = 0; i < num_sources; i++) {
			int source = phi->sources[i];
			if (source < 0) {
				FAIL(VARFMT " negative source\n", VAR(phi->ssa_var));
			}
			if (!is_in_phi_use_chain(ssa, source, phi)) {
				FAIL(VARFMT " not in phi use chain of %d\n", VAR(phi->ssa_var), source);
			}
			if (ssa->vars[source].var != ssa->vars[phi->ssa_var].var) {
				FAIL(VARFMT " source of phi for " VARFMT "\n", VAR(source), VAR(phi->ssa_var));
			}
			if (phi->use_chains[i]) {
				int j;
				for (j = i + 1; j < num_sources; j++) {
					if (phi->sources[j] == source && phi->use_chains[j]) {
						FAIL("use chain for source " VARFMT " of phi " VARFMT
							" at %d despite earlier use\n", VAR(source), VAR(phi->ssa_var), j);
					}
				}
			}
		}
		if (ssa->vars[phi->ssa_var].definition_phi != phi) {
			FAIL(VARFMT " does not define this phi\n", VAR(phi->ssa_var));
		}
	} FOREACH_PHI_END();

	/* Blocks */
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		int *predecessors = &cfg->predecessors[block->predecessor_offset];
		int s, j;

		if (i != 0 && block->start < (block-1)->start + (block-1)->len) {
			FAIL("Block %d start %d smaller previous end %d\n",
				i, block->start, (block-1)->start + (block-1)->len);
		}
		if (i != cfg->blocks_count-1 && block->start + block->len > (block+1)->start) {
			FAIL("Block %d end %d greater next start %d\n",
				i, block->start + block->len, (block+1)->start);
		}

		for (j = block->start; j < block->start + block->len; j++) {
			if (cfg->map[j] != i) {
				FAIL("Instr " INSTRFMT " not associated with block %d\n", INSTR(j), i);
			}
		}

		if (!(block->flags & ZEND_BB_REACHABLE)) {
			if (ssa->blocks[i].phis) {
				FAIL("Unreachable block %d has phis\n", i);
			}
			continue;
		}

		for (s = 0; s < block->successors_count; s++) {
			zend_basic_block *next_block;
			if (block->successors[s] < 0) {
				FAIL("Successor number %d of %d negative", s, i);
			}
			next_block = &cfg->blocks[block->successors[s]];
			if (!(next_block->flags & ZEND_BB_REACHABLE)) {
				FAIL("Successor %d of %d not reachable\n", block->successors[s], i);
			}
			if (!is_in_predecessors(cfg, next_block, i)) {
				FAIL("Block %d predecessors missing %d\n", block->successors[s], i);
			}
		}

		for (j = 0; j < block->predecessors_count; j++) {
			if (predecessors[j] >= 0) {
				int k;
				zend_basic_block *prev_block = &cfg->blocks[predecessors[j]];
				if (!(prev_block->flags & ZEND_BB_REACHABLE)) {
					FAIL("Predecessor %d of %d not reachable\n", predecessors[j], i);
				}
				if (!is_in_successors(prev_block, i)) {
					FAIL("Block %d successors missing %d\n", predecessors[j], i);
				}
				for (k = 0; k < block->predecessors_count; k++) {
					if (k != j && predecessors[k] == predecessors[j]) {
						FAIL("Block %d has duplicate predecessor %d\n", i, predecessors[j]);
					}
				}
			}
		}
	}

finish:
	if (status == FAILURE) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "at SSA integrity verification", ssa);
		ZEND_ASSERT(0 && "SSA integrity verification failed");
	}
}
