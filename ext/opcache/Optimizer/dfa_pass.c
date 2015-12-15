/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"
#include "zend_bitset.h"
#include "zend_cfg.h"
#include "zend_ssa.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#include "zend_dump.h"

#ifndef HAVE_DFA_PASS
# define HAVE_DFA_PASS 0
#endif

void optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	void *checkpoint;
	uint32_t build_flags;
	uint32_t flags = 0;
	zend_ssa ssa;

#if !HAVE_DFA_PASS
	return;
#endif

    /* Build SSA */
	memset(&ssa, 0, sizeof(ssa));
	checkpoint = zend_arena_checkpoint(ctx->arena);

	if (zend_build_cfg(&ctx->arena, op_array, 0, &ssa.cfg, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (flags & ZEND_FUNC_TOO_DYNAMIC) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &ssa.cfg) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_CFG) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG | ZEND_DUMP_HIDE_UNUSED_VARS, "dfa cfg", &ssa.cfg);
	}

	/* Compute Dominators Tree */
	if (zend_cfg_compute_dominators_tree(op_array, &ssa.cfg) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	/* Identify reducible and irreducible loops */
	if (zend_cfg_identify_loops(op_array, &ssa.cfg, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_DOMINATORS) {
		zend_dump_dominators(op_array, &ssa.cfg);
	}

	build_flags = 0;
	if (ctx->debug_level & ZEND_DUMP_DFA_LIVENESS) {
		build_flags |= ZEND_SSA_DEBUG_LIVENESS;
	}
	if (ctx->debug_level & ZEND_DUMP_DFA_PHI) {
		build_flags |= ZEND_SSA_DEBUG_PHI_PLACEMENT;
	}
	if (zend_build_ssa(&ctx->arena, op_array, build_flags, &ssa, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "before dfa pass", &ssa);
	}


	if (zend_ssa_compute_use_def_chains(&ctx->arena, op_array, &ssa) != SUCCESS){
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (zend_ssa_find_false_dependencies(op_array, &ssa) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (zend_ssa_find_sccs(op_array, &ssa) != SUCCESS){
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, &ssa) != SUCCESS) {
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA_VARS) {
		zend_dump_ssa_variables(op_array, &ssa);
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "before dfa pass", &ssa);
	}

	//TODO: Add optimization???

	if (ctx->debug_level & ZEND_DUMP_AFTER_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "after dfa pass", &ssa);
	}

	/* Destroy SSA */
	zend_arena_release(&ctx->arena, checkpoint);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
