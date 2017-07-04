/*
   +----------------------------------------------------------------------+
   | Zend Engine, Call Graph                                              |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
	void *ctx;
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_bitset instr_worklist;
	/* Represent phi-instructions through the defining var */
	zend_bitset phi_var_worklist;
	zend_bitset block_worklist;
	zend_bitset executable_blocks;
	/* Edge encoding: 2 bits per block, one for each successor */
	zend_bitset feasible_edges;
	/* If there are more than two successors, an HT is used instead */
	HashTable *feasible_edges_ht;
	uint32_t instr_worklist_len;
	uint32_t phi_var_worklist_len;
	uint32_t block_worklist_len;

	struct {
		void (*visit_instr)(
			struct _scdf_ctx *scdf, void *ctx, zend_op *opline, zend_ssa_op *ssa_op);
		void (*visit_phi)(
			struct _scdf_ctx *scdf, void *ctx, zend_ssa_phi *phi);
		zend_bool (*get_feasible_successors)(
			struct _scdf_ctx *scdf, void *ctx, zend_basic_block *block,
			zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc);
	} handlers;
} scdf_ctx;

void scdf_init(scdf_ctx *scdf, zend_op_array *op_array, zend_ssa *ssa, void *ctx);
void scdf_solve(scdf_ctx *scdf, const char *name);
void scdf_free(scdf_ctx *scdf);

void scdf_remove_unreachable_blocks(scdf_ctx *scdf);

/* Add uses to worklist */
static inline void scdf_add_to_worklist(scdf_ctx *scdf, int var_num) {
	zend_ssa *ssa = scdf->ssa;
	zend_ssa_var *var = &ssa->vars[var_num];
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
	zend_ssa_var *var = &scdf->ssa->vars[var_num];
	if (var->definition >= 0) {
		zend_bitset_incl(scdf->instr_worklist, var->definition);
	} else if (var->definition_phi) {
		zend_bitset_incl(scdf->phi_var_worklist, var_num);
	}
}

static inline zend_bool scdf_is_edge_feasible(scdf_ctx *scdf, int from, int to) {
	zend_basic_block *block = &scdf->ssa->cfg.blocks[from];
	int s;
	for (s = 0; s < block->successors_count; s++) {
		if (block->successors[s] == to) {
			if (s < 2) {
				return zend_bitset_in(scdf->feasible_edges, 2 * from + s);
			} else {
				return scdf->feasible_edges_ht
					&& zend_hash_index_exists(scdf->feasible_edges_ht,
							(zend_long) (intptr_t) &block->successors[s]);
			}
		}
	}
	ZEND_ASSERT(0);
}

#endif
