/*
   +----------------------------------------------------------------------+
   | Zend Engine, Sparse Conditional Data Flow Propagation Framework      |
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
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/scdf.h"

/* This defines a generic framework for sparse conditional dataflow propagation. The algorithm is
 * based on "Sparse conditional constant propagation" by Wegman and Zadeck. We're using a
 * generalized implementation as described in chapter 8.3 of the SSA book.
 *
 * Every SSA variable is associated with an element on a finite-height lattice, those value can only
 * ever be lowered during the operation of the algorithm. If a value is lowered all instructions and
 * phis using that value need to be reconsidered (this is done by adding the variable to a
 * worklist). For phi functions the result is computed by applying the meet operation to the
 * operands. This continues until a fixed point is reached.
 *
 * The algorithm is control-flow sensitive: All blocks except the start block are initially assumed
 * to be unreachable. When considering a branch instruction, we determine the feasible successors
 * based on the current state of the variable lattice. If a new edge becomes feasible we either have
 * to mark the successor block executable and consider all instructions in it, or, if the target is
 * already executable, we only have to reconsider the phi functions (as we only consider phi
 * operands which are associated with a feasible edge).
 *
 * The generic framework requires the definition of three functions:
 * * visit_instr() should recompute the lattice values of all SSA variables defined by an
 *   instruction.
 * * visit_phi() should recompute the lattice value of the SSA variable defined by the phi. While
 *   doing this it should only consider operands for which scfg_is_edge_feasible() returns true.
 * * get_feasible_successors() should determine the feasible successors for a branch instruction.
 *   Note that this callback only needs to handle conditional branches (with two successors).
 */

#if 0
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void scdf_mark_edge_feasible(scdf_ctx *scdf, int from, int to) {
	uint32_t edge = scdf_edge(&scdf->ssa->cfg, from, to);

	if (zend_bitset_in(scdf->feasible_edges, edge)) {
		/* We already handled this edge */
		return;
	}

	DEBUG_PRINT("Marking edge %d->%d feasible\n", from, to);
	zend_bitset_incl(scdf->feasible_edges, edge);

	if (!zend_bitset_in(scdf->executable_blocks, to)) {
		if (!zend_bitset_in(scdf->block_worklist, to)) {
			DEBUG_PRINT("Adding block %d to worklist\n", to);
		}
		zend_bitset_incl(scdf->block_worklist, to);
	} else {
		/* Block is already executable, only a new edge became feasible.
		 * Reevaluate phi nodes to account for changed source operands. */
		zend_ssa_block *ssa_block = &scdf->ssa->blocks[to];
		zend_ssa_phi *phi;
		for (phi = ssa_block->phis; phi; phi = phi->next) {
			zend_bitset_excl(scdf->phi_var_worklist, phi->ssa_var);
			scdf->handlers.visit_phi(scdf, phi);
		}
	}
}

void scdf_init(zend_optimizer_ctx *ctx, scdf_ctx *scdf, zend_op_array *op_array, zend_ssa *ssa) {
	scdf->op_array = op_array;
	scdf->ssa = ssa;

	scdf->instr_worklist_len = zend_bitset_len(op_array->last);
	scdf->phi_var_worklist_len = zend_bitset_len(ssa->vars_count);
	scdf->block_worklist_len = zend_bitset_len(ssa->cfg.blocks_count);

	scdf->instr_worklist = zend_arena_calloc(&ctx->arena,
		scdf->instr_worklist_len + scdf->phi_var_worklist_len + 2 * scdf->block_worklist_len + zend_bitset_len(ssa->cfg.edges_count),
		sizeof(zend_ulong));

	scdf->phi_var_worklist = scdf->instr_worklist + scdf->instr_worklist_len;
	scdf->block_worklist = scdf->phi_var_worklist + scdf->phi_var_worklist_len;
	scdf->executable_blocks = scdf->block_worklist + scdf->block_worklist_len;
	scdf->feasible_edges = scdf->executable_blocks + scdf->block_worklist_len;

	zend_bitset_incl(scdf->block_worklist, 0);
	zend_bitset_incl(scdf->executable_blocks, 0);
}

void scdf_solve(scdf_ctx *scdf, const char *name) {
	zend_ssa *ssa = scdf->ssa;
	DEBUG_PRINT("Start SCDF solve (%s)\n", name);
	while (!zend_bitset_empty(scdf->instr_worklist, scdf->instr_worklist_len)
		|| !zend_bitset_empty(scdf->phi_var_worklist, scdf->phi_var_worklist_len)
		|| !zend_bitset_empty(scdf->block_worklist, scdf->block_worklist_len)
	) {
		int i;
		while ((i = zend_bitset_pop_first(scdf->phi_var_worklist, scdf->phi_var_worklist_len)) >= 0) {
			zend_ssa_phi *phi = ssa->vars[i].definition_phi;
			ZEND_ASSERT(phi);
			if (zend_bitset_in(scdf->executable_blocks, phi->block)) {
				scdf->handlers.visit_phi(scdf, phi);
			}
		}

		while ((i = zend_bitset_pop_first(scdf->instr_worklist, scdf->instr_worklist_len)) >= 0) {
			int block_num = ssa->cfg.map[i];
			if (zend_bitset_in(scdf->executable_blocks, block_num)) {
				zend_basic_block *block = &ssa->cfg.blocks[block_num];
				zend_op *opline = &scdf->op_array->opcodes[i];
				zend_ssa_op *ssa_op = &ssa->ops[i];
				if (opline->opcode == ZEND_OP_DATA) {
					opline--;
					ssa_op--;
				}
				scdf->handlers.visit_instr(scdf, opline, ssa_op);
				if (i == block->start + block->len - 1) {
					if (block->successors_count == 1) {
						scdf_mark_edge_feasible(scdf, block_num, block->successors[0]);
					} else if (block->successors_count > 1) {
						scdf->handlers.mark_feasible_successors(scdf, block_num, block, opline, ssa_op);
					}
				}
			}
		}

		while ((i = zend_bitset_pop_first(scdf->block_worklist, scdf->block_worklist_len)) >= 0) {
			/* This block is now live. Interpret phis and instructions in it. */
			zend_basic_block *block = &ssa->cfg.blocks[i];
			zend_ssa_block *ssa_block = &ssa->blocks[i];

			DEBUG_PRINT("Pop block %d from worklist\n", i);
			zend_bitset_incl(scdf->executable_blocks, i);

			{
				zend_ssa_phi *phi;
				for (phi = ssa_block->phis; phi; phi = phi->next) {
					zend_bitset_excl(scdf->phi_var_worklist, phi->ssa_var);
					scdf->handlers.visit_phi(scdf, phi);
				}
			}

			if (block->len == 0) {
				/* Zero length blocks don't have a last instruction that would normally do this */
				scdf_mark_edge_feasible(scdf, i, block->successors[0]);
			} else {
				zend_op *opline = NULL;
				int j, end = block->start + block->len;
				for (j = block->start; j < end; j++) {
					opline = &scdf->op_array->opcodes[j];
					zend_bitset_excl(scdf->instr_worklist, j);
					if (opline->opcode != ZEND_OP_DATA) {
						scdf->handlers.visit_instr(scdf, opline, &ssa->ops[j]);
					}
				}
				if (block->successors_count == 1) {
					scdf_mark_edge_feasible(scdf, i, block->successors[0]);
				} else if (block->successors_count > 1) {
					ZEND_ASSERT(opline && "Should have opline in non-empty block");
					if (opline->opcode == ZEND_OP_DATA) {
						opline--;
						j--;
					}
					scdf->handlers.mark_feasible_successors(scdf, i, block, opline, &ssa->ops[j-1]);
				}
			}
		}
	}
}

/* If a live range starts in a reachable block and ends in an unreachable block, we should
 * not eliminate the latter. While it cannot be reached, the FREE opcode of the loop var
 * is necessary for the correctness of temporary compaction. */
static zend_bool kept_alive_by_loop_var_free(scdf_ctx *scdf, uint32_t block_idx) {
	uint32_t i;
	const zend_op_array *op_array = scdf->op_array;
	const zend_cfg *cfg = &scdf->ssa->cfg;
	const zend_basic_block *block = &cfg->blocks[block_idx];
	if (!(cfg->flags & ZEND_FUNC_FREE_LOOP_VAR)) {
		return 0;
	}
	for (i = block->start; i < block->start + block->len; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (zend_optimizer_is_loop_var_free(opline)) {
			int ssa_var = scdf->ssa->ops[i].op1_use;
			if (ssa_var >= 0) {
				int op_num = scdf->ssa->vars[ssa_var].definition;
				uint32_t def_block;
				ZEND_ASSERT(op_num >= 0);
				def_block = cfg->map[op_num];
				if (zend_bitset_in(scdf->executable_blocks, def_block)) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/* Removes unreachable blocks. This will remove both the instructions (and phis) in the
 * blocks, as well as remove them from the successor / predecessor lists and mark them
 * unreachable. Blocks already marked unreachable are not removed. */
int scdf_remove_unreachable_blocks(scdf_ctx *scdf) {
	zend_ssa *ssa = scdf->ssa;
	int i;
	int removed_ops = 0;
	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!zend_bitset_in(scdf->executable_blocks, i)
				&& (ssa->cfg.blocks[i].flags & ZEND_BB_REACHABLE)
				&& !kept_alive_by_loop_var_free(scdf, i)) {
			removed_ops += ssa->cfg.blocks[i].len;
			zend_ssa_remove_block(scdf->op_array, ssa, i);
		}
	}
	return removed_ops;
}
