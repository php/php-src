/*
   +----------------------------------------------------------------------+
   | Zend Engine, Call Graph                                              |
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

#ifndef _SCDF_H
#define _SCDF_H

#include "zend_bitset.h"

typedef struct _scdf_ctx {
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_bitset instr_worklist;
	/* Represent phi-instructions through the defining var */
	zend_bitset phi_var_worklist;
	zend_bitset block_worklist;
	zend_bitset executable_blocks;
	/* 1 bit per edge, see scdf_edge(cfg, from, to) */
	zend_bitset feasible_edges;
	uint32_t instr_worklist_len;
	uint32_t phi_var_worklist_len;
	uint32_t block_worklist_len;

	struct {
		void (*visit_instr)(
			struct _scdf_ctx *scdf, zend_op *opline, zend_ssa_op *ssa_op);
		void (*visit_phi)(
			struct _scdf_ctx *scdf, zend_ssa_phi *phi);
		void (*mark_feasible_successors)(
			struct _scdf_ctx *scdf, int block_num, zend_basic_block *block,
			zend_op *opline, zend_ssa_op *ssa_op);
	} handlers;
} scdf_ctx;

void scdf_init(zend_optimizer_ctx *ctx, scdf_ctx *scdf, zend_op_array *op_array, zend_ssa *ssa);
void scdf_solve(scdf_ctx *scdf, const char *name);

uint32_t scdf_remove_unreachable_blocks(scdf_ctx *scdf);

/* Add uses to worklist */
static inline void scdf_add_to_worklist(scdf_ctx *scdf, int var_num) {
	const zend_ssa *ssa = scdf->ssa;
	const zend_ssa_var *var = &ssa->vars[var_num];
	int use;
	zend_ssa_phi *phi;
	FOREACH_USE(var, use) {
		zend_bitset_incl(scdf->instr_worklist, use);
	} FOREACH_USE_END();
	FOREACH_PHI_USE(var, phi) {
		zend_bitset_incl(scdf->phi_var_worklist, phi->ssa_var);
	} FOREACH_PHI_USE_END();
}

/* This should usually not be necessary, however it's used for type narrowing. */
static inline void scdf_add_def_to_worklist(scdf_ctx *scdf, int var_num) {
	const zend_ssa_var *var = &scdf->ssa->vars[var_num];
	if (var->definition >= 0) {
		zend_bitset_incl(scdf->instr_worklist, var->definition);
	} else if (var->definition_phi) {
		zend_bitset_incl(scdf->phi_var_worklist, var_num);
	}
}

static inline uint32_t scdf_edge(const zend_cfg *cfg, int from, int to) {
	const zend_basic_block *to_block = cfg->blocks + to;
	int i;

	for (i = 0; i < to_block->predecessors_count; i++) {
		uint32_t edge = to_block->predecessor_offset + i;

		if (cfg->predecessors[edge] == from) {
			return edge;
		}
	}
	ZEND_UNREACHABLE();
}

static inline bool scdf_is_edge_feasible(const scdf_ctx *scdf, int from, int to) {
	uint32_t edge = scdf_edge(&scdf->ssa->cfg, from, to);
	return zend_bitset_in(scdf->feasible_edges, edge);
}

void scdf_mark_edge_feasible(scdf_ctx *scdf, int from, int to);

#endif
