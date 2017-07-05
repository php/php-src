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

static void mark_edge_feasible(scdf_ctx *ctx, int from, int to) {
	uint32_t edge = scdf_edge(&ctx->ssa->cfg, from, to);

	if (zend_bitset_in(ctx->feasible_edges, edge)) {
		/* We already handled this edge */
		return;
	}

	DEBUG_PRINT("Marking edge %d->%d feasible\n", from, to);
	zend_bitset_incl(ctx->feasible_edges, edge);

	if (!zend_bitset_in(ctx->executable_blocks, to)) {
		if (!zend_bitset_in(ctx->block_worklist, to)) {
			DEBUG_PRINT("Adding block %d to worklist\n", to);
		}
		zend_bitset_incl(ctx->block_worklist, to);
	} else {
		/* Block is already executable, only a new edge became feasible.
		 * Reevaluate phi nodes to account for changed source operands. */
		zend_ssa_block *ssa_block = &ctx->ssa->blocks[to];
		zend_ssa_phi *phi;
		for (phi = ssa_block->phis; phi; phi = phi->next) {
			zend_bitset_excl(ctx->phi_var_worklist, phi->ssa_var);
			ctx->handlers.visit_phi(ctx, ctx->ctx, phi);
		}
	}
}

/* Returns whether there is a successor */
static inline zend_bool get_feasible_successors(
		scdf_ctx *ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc) {
	/* Terminal block without successors */
	if (block->successors_count == 0) {
		return 0;
	}

	/* Unconditional jump */
	if (block->successors_count == 1) {
		suc[0] = 1;
		return 1;
	}

	return ctx->handlers.get_feasible_successors(ctx, ctx->ctx, block, opline, ssa_op, suc);
}

static void handle_instr(scdf_ctx *ctx, int block_num, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_basic_block *block = &ctx->ssa->cfg.blocks[block_num];
	ctx->handlers.visit_instr(ctx, ctx->ctx, opline, ssa_op);

	if (opline - ctx->op_array->opcodes == block->start + block->len - 1) {
		if (opline->opcode == ZEND_SWITCH_LONG || opline->opcode == ZEND_SWITCH_STRING) {
			// TODO For now consider all edges feasible
			int s;
			for (s = 0; s < block->successors_count; s++) {
				mark_edge_feasible(ctx, block_num, block->successors[s]);
			}
		} else {
			zend_bool suc[2] = {0};
			if (get_feasible_successors(ctx, block, opline, ssa_op, suc)) {
				if (suc[0]) {
					mark_edge_feasible(ctx, block_num, block->successors[0]);
				}
				if (suc[1]) {
					mark_edge_feasible(ctx, block_num, block->successors[1]);
				}
			}
		}
	}
}

void scdf_init(scdf_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa, void *extra_ctx) {
	uint32_t edges_count = 0;
	int b;

	for (b = 0; b < ssa->cfg.blocks_count; b++) {
		edges_count += ssa->cfg.blocks[b].predecessors_count;
	}

	ctx->op_array = op_array;
	ctx->ssa = ssa;
	ctx->ctx = extra_ctx;

	ctx->instr_worklist_len = zend_bitset_len(op_array->last);
	ctx->phi_var_worklist_len = zend_bitset_len(ssa->vars_count);
	ctx->block_worklist_len = zend_bitset_len(ssa->cfg.blocks_count);

	ctx->instr_worklist = ecalloc(
		ctx->instr_worklist_len + ctx->phi_var_worklist_len + 2 * ctx->block_worklist_len + zend_bitset_len(edges_count),
		sizeof(zend_ulong));

	ctx->phi_var_worklist = ctx->instr_worklist + ctx->instr_worklist_len;
	ctx->block_worklist = ctx->phi_var_worklist + ctx->phi_var_worklist_len;
	ctx->executable_blocks = ctx->block_worklist + ctx->block_worklist_len;
	ctx->feasible_edges = ctx->executable_blocks + ctx->block_worklist_len;

	zend_bitset_incl(ctx->block_worklist, 0);
	zend_bitset_incl(ctx->executable_blocks, 0);
}

void scdf_free(scdf_ctx *ctx) {
	efree(ctx->instr_worklist);
}

void scdf_solve(scdf_ctx *ctx, const char *name) {
	zend_ssa *ssa = ctx->ssa;
	DEBUG_PRINT("Start SCDF solve (%s)\n", name);
	while (!zend_bitset_empty(ctx->instr_worklist, ctx->instr_worklist_len)
		|| !zend_bitset_empty(ctx->phi_var_worklist, ctx->phi_var_worklist_len)
		|| !zend_bitset_empty(ctx->block_worklist, ctx->block_worklist_len)
	) {
		int i;
		while ((i = zend_bitset_pop_first(ctx->phi_var_worklist, ctx->phi_var_worklist_len)) >= 0) {
			zend_ssa_phi *phi = ssa->vars[i].definition_phi;
			ZEND_ASSERT(phi);
			if (zend_bitset_in(ctx->executable_blocks, phi->block)) {
				ctx->handlers.visit_phi(ctx, ctx->ctx, phi);
			}
		}

		while ((i = zend_bitset_pop_first(ctx->instr_worklist, ctx->instr_worklist_len)) >= 0) {
			int block_num = ssa->cfg.map[i];
			if (zend_bitset_in(ctx->executable_blocks, block_num)) {
				handle_instr(ctx, block_num, &ctx->op_array->opcodes[i], &ssa->ops[i]);
			}
		}

		while ((i = zend_bitset_pop_first(ctx->block_worklist, ctx->block_worklist_len)) >= 0) {
			/* This block is now live. Interpret phis and instructions in it. */
			zend_basic_block *block = &ssa->cfg.blocks[i];
			zend_ssa_block *ssa_block = &ssa->blocks[i];

			DEBUG_PRINT("Pop block %d from worklist\n", i);
			zend_bitset_incl(ctx->executable_blocks, i);

			{
				zend_ssa_phi *phi;
				for (phi = ssa_block->phis; phi; phi = phi->next) {
					zend_bitset_excl(ctx->phi_var_worklist, phi->ssa_var);
					ctx->handlers.visit_phi(ctx, ctx->ctx, phi);
				}
			}

			{
				int j, end = block->start + block->len;
				for (j = block->start; j < end; j++) {
					zend_bitset_excl(ctx->instr_worklist, j);
					handle_instr(ctx, i, &ctx->op_array->opcodes[j], &ssa->ops[j]);
				}
			}

			if (block->len == 0) {
				/* Zero length blocks don't have a last instruction that would normally do this */
				mark_edge_feasible(ctx, i, block->successors[0]);
			}
		}
	}
}

/* If a live range starts in a reachable block and ends in an unreachable block, we should
 * not eliminate the latter. While it cannot be reached, the FREE opcode of the loop var
 * is necessary for the correctness of temporary compaction. */
static zend_bool kept_alive_by_live_range(scdf_ctx *scdf, uint32_t block) {
	uint32_t i;
	const zend_op_array *op_array = scdf->op_array;
	const zend_cfg *cfg = &scdf->ssa->cfg;
	for (i = 0; i < op_array->last_live_range; i++) {
		zend_live_range *live_range = &op_array->live_range[i];
		uint32_t start_block = cfg->map[live_range->start];
		uint32_t end_block = cfg->map[live_range->end];

		if (end_block == block && start_block != block
				&& zend_bitset_in(scdf->executable_blocks, start_block)) {
			return 1;
		}
	}
	return 0;
}

/* Removes unreachable blocks. This will remove both the instructions (and phis) in the
 * blocks, as well as remove them from the successor / predecessor lists and mark them
 * unreachable. Blocks already marked unreachable are not removed. */
void scdf_remove_unreachable_blocks(scdf_ctx *scdf) {
	zend_ssa *ssa = scdf->ssa;
	int i;
	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!zend_bitset_in(scdf->executable_blocks, i)
				&& (ssa->cfg.blocks[i].flags & ZEND_BB_REACHABLE)
				&& !kept_alive_by_live_range(scdf, i)) {
			zend_ssa_remove_block(scdf->op_array, ssa, i);
		}
	}
}
