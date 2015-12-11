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
#include "zend_dump.h"

#ifndef HAVE_DFA_PASS
# define HAVE_DFA_PASS 0
#endif

void optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	void *checkpoint;
	uint32_t flags;
	zend_cfg cfg;
	zend_ssa ssa;

#if !HAVE_DFA_PASS
	return;
#endif

    /* Build SSA */
	checkpoint = zend_arena_checkpoint(ctx->arena);

	if (zend_build_cfg(&ctx->arena, op_array, 0, 0, &cfg, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (flags & ZEND_FUNC_TOO_DYNAMIC) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &cfg) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_CFG) {
		zend_dump_op_array(op_array, &cfg, 0, "dfa cfg");
	}

	/* Compute Dominators Tree */
	if (zend_cfg_compute_dominators_tree(op_array, &cfg) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	/* Identify reducible and irreducible loops */
	if (zend_cfg_identify_loops(op_array, &cfg, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_DOMINATORS) {
		int j;

		fprintf(stderr, "DOMINATORS-TREE:\n");
		for (j = 0; j < cfg.blocks_count; j++) {
			zend_basic_block *b = cfg.blocks + j;
			if (b->flags & ZEND_BB_REACHABLE) {
				zend_dump_block_info(&cfg, j, 0);
			}
		}
	}

	if (zend_build_ssa(&ctx->arena, op_array, &cfg, 0, &ssa, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_DFA_PASS) {
		zend_dump_op_array(op_array, &cfg, ZEND_DUMP_UNREACHABLE, "before dfa pass");
	}

	//TODO: ???

	if (ctx->debug_level & ZEND_DUMP_AFTER_DFA_PASS) {
		zend_dump_op_array(op_array, &cfg, 0, "after dfa pass");
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
