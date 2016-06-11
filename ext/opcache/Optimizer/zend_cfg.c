/*
   +----------------------------------------------------------------------+
   | Zend Engine, CFG - Control Flow Graph                                |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
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
#include "zend_func_info.h"
#include "zend_worklist.h"
#include "zend_optimizer.h"
#include "zend_optimizer_internal.h"

static void zend_mark_reachable(zend_op *opcodes, zend_basic_block *blocks, zend_basic_block *b) /* {{{ */
{
	zend_uchar opcode;
	zend_basic_block *b0;
	int successor_0, successor_1;

	while (1) {
		b->flags |= ZEND_BB_REACHABLE;
		successor_0 = b->successors[0];
		if (successor_0 >= 0) {
			successor_1 = b->successors[1];
			if (successor_1 >= 0) {
				b0 = blocks + successor_0;
				b0->flags |= ZEND_BB_TARGET;
				if (!(b0->flags & ZEND_BB_REACHABLE)) {
					zend_mark_reachable(opcodes, blocks, b0);
				}

				ZEND_ASSERT(b->len != 0);
				opcode = opcodes[b->start + b->len - 1].opcode;
				b = blocks + successor_1;
				if (opcode == ZEND_JMPZNZ) {
					b->flags |= ZEND_BB_TARGET;
				} else {
					b->flags |= ZEND_BB_FOLLOW;
				}
			} else if (b->len != 0) {
				opcode = opcodes[b->start + b->len - 1].opcode;
				b = blocks + successor_0;
				if (opcode == ZEND_JMP) {
					b->flags |= ZEND_BB_TARGET;
				} else {
					b->flags |= ZEND_BB_FOLLOW;

					//TODO: support for stackless CFG???
					if (0/*stackless*/) {
						if (opcode == ZEND_INCLUDE_OR_EVAL ||
						    opcode == ZEND_GENERATOR_CREATE ||
						    opcode == ZEND_YIELD ||
						    opcode == ZEND_YIELD_FROM ||
						    opcode == ZEND_DO_FCALL ||
						    opcode == ZEND_DO_UCALL ||
						    opcode == ZEND_DO_FCALL_BY_NAME) {
							b->flags |= ZEND_BB_ENTRY;
						}
					}
				}
			} else {
				b = blocks + successor_0;
				b->flags |= ZEND_BB_FOLLOW;
			}
			if (b->flags & ZEND_BB_REACHABLE) return;
		} else {
			b->flags |= ZEND_BB_EXIT;
			return;
		}
	}
}
/* }}} */

static void zend_mark_reachable_blocks(const zend_op_array *op_array, zend_cfg *cfg, int start) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;

	blocks[start].flags = ZEND_BB_START;
	zend_mark_reachable(op_array->opcodes, blocks, blocks + start);

	if (op_array->last_live_range || op_array->last_try_catch) {
		zend_basic_block *b;
		int j, changed;
		uint32_t *block_map = cfg->map;

		do {
			changed = 0;

			/* Add live range paths */
			for (j = 0; j < op_array->last_live_range; j++) {
				zend_live_range *live_range = &op_array->live_range[j];
				if (live_range->var == (uint32_t)-1) {
					/* this live range already removed */
					continue;
				}
				b = blocks + block_map[live_range->start];
				if (b->flags & ZEND_BB_REACHABLE) {
					while (b->len > 0 && op_array->opcodes[b->start].opcode == ZEND_NOP) {
						b->start++;
						b->len--;
					}
					if (b->len == 0 && b->successors[0] == block_map[live_range->end]) {
						/* mark as removed (empty live range) */
						live_range->var = (uint32_t)-1;
						continue;
					}
					b->flags |= ZEND_BB_GEN_VAR;
					b = blocks + block_map[live_range->end];
					b->flags |= ZEND_BB_KILL_VAR;
					if (!(b->flags & ZEND_BB_REACHABLE)) {
						if (cfg->split_at_live_ranges) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, blocks, b);
						} else {
							ZEND_ASSERT(!(b->flags & ZEND_BB_UNREACHABLE_FREE));
							ZEND_ASSERT(b->start == live_range->end);
							b->flags |= ZEND_BB_UNREACHABLE_FREE;
						}
					}
				} else {
					ZEND_ASSERT(!(blocks[block_map[live_range->end]].flags & ZEND_BB_REACHABLE));
				}
			}

			/* Add exception paths */
			for (j = 0; j < op_array->last_try_catch; j++) {

				/* check for jumps into the middle of try block */
				b = blocks + block_map[op_array->try_catch_array[j].try_op];
				if (!(b->flags & ZEND_BB_REACHABLE)) {
					zend_basic_block *end;

					if (op_array->try_catch_array[j].catch_op) {
						end = blocks + block_map[op_array->try_catch_array[j].catch_op];
						while (b != end) {
							if (b->flags & ZEND_BB_REACHABLE) {
								op_array->try_catch_array[j].try_op = b->start;
								break;
							}
							b++;
						}
					}
					b = blocks + block_map[op_array->try_catch_array[j].try_op];
					if (!(b->flags & ZEND_BB_REACHABLE)) {
						if (op_array->try_catch_array[j].finally_op) {
							end = blocks + block_map[op_array->try_catch_array[j].finally_op];
							while (b != end) {
								if (b->flags & ZEND_BB_REACHABLE) {
									op_array->try_catch_array[j].try_op = op_array->try_catch_array[j].catch_op;
									changed = 1;
									zend_mark_reachable(op_array->opcodes, blocks, blocks + block_map[op_array->try_catch_array[j].try_op]);
									break;
								}
								b++;
							}
						}
					}
				}

				b = blocks + block_map[op_array->try_catch_array[j].try_op];
				if (b->flags & ZEND_BB_REACHABLE) {
					b->flags |= ZEND_BB_TRY;
					if (op_array->try_catch_array[j].catch_op) {
						b = blocks + block_map[op_array->try_catch_array[j].catch_op];
						b->flags |= ZEND_BB_CATCH;
						if (!(b->flags & ZEND_BB_REACHABLE)) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, blocks, b);
						}
					}
					if (op_array->try_catch_array[j].finally_op) {
						b = blocks + block_map[op_array->try_catch_array[j].finally_op];
						b->flags |= ZEND_BB_FINALLY;
						if (!(b->flags & ZEND_BB_REACHABLE)) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, blocks, b);
						}
					}
					if (op_array->try_catch_array[j].finally_end) {
						b = blocks + block_map[op_array->try_catch_array[j].finally_end];
						b->flags |= ZEND_BB_FINALLY_END;
						if (!(b->flags & ZEND_BB_REACHABLE)) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, blocks, b);
						}
					}
				} else {
					if (op_array->try_catch_array[j].catch_op) {
						ZEND_ASSERT(!(blocks[block_map[op_array->try_catch_array[j].catch_op]].flags & ZEND_BB_REACHABLE));
					}
					if (op_array->try_catch_array[j].finally_op) {
						ZEND_ASSERT(!(blocks[block_map[op_array->try_catch_array[j].finally_op]].flags & ZEND_BB_REACHABLE));
					}
					if (op_array->try_catch_array[j].finally_end) {
						ZEND_ASSERT(!(blocks[block_map[op_array->try_catch_array[j].finally_end]].flags & ZEND_BB_REACHABLE));
					}
				}
			}
		} while (changed);
	}
}
/* }}} */

void zend_cfg_remark_reachable_blocks(const zend_op_array *op_array, zend_cfg *cfg) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;
	int i;
	int start = 0;

	for (i = 0; i < cfg->blocks_count; i++) {
		if (blocks[i].flags & ZEND_BB_REACHABLE) {
			start = i;
			i++;
			break;
		}
	}

	/* clear all flags */
	for (i = 0; i < cfg->blocks_count; i++) {
		blocks[i].flags = 0;
	}

	zend_mark_reachable_blocks(op_array, cfg, start);
}
/* }}} */

static void record_successor(zend_basic_block *blocks, int pred, int n, int succ)
{
	blocks[pred].successors[n] = succ;
}

static void initialize_block(zend_basic_block *block) {
	block->flags = 0;
	block->successors[0] = -1;
	block->successors[1] = -1;
	block->predecessors_count = 0;
	block->predecessor_offset = -1;
	block->idom = -1;
	block->loop_header = -1;
	block->level = -1;
	block->children = -1;
	block->next_child = -1;
}

#define BB_START(i) do { \
		if (!block_map[i]) { blocks_count++;} \
		block_map[i]++; \
	} while (0)

int zend_build_cfg(zend_arena **arena, const zend_op_array *op_array, uint32_t build_flags, zend_cfg *cfg, uint32_t *func_flags) /* {{{ */
{
	uint32_t flags = 0;
	uint32_t i;
	int j;
	uint32_t *block_map;
	zend_function *fn;
	int blocks_count = 0;
	zend_basic_block *blocks;
	zval *zv;
	zend_bool extra_entry_block = 0;

	cfg->split_at_live_ranges = (build_flags & ZEND_CFG_SPLIT_AT_LIVE_RANGES) != 0;
	cfg->map = block_map = zend_arena_calloc(arena, op_array->last, sizeof(uint32_t));
	if (!block_map) {
		return FAILURE;
	}

	/* Build CFG, Step 1: Find basic blocks starts, calculate number of blocks */
	BB_START(0);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = op_array->opcodes + i;
		switch(opline->opcode) {
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
			case ZEND_EXIT:
			case ZEND_THROW:
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_INCLUDE_OR_EVAL:
				flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
			case ZEND_GENERATOR_CREATE:
			case ZEND_YIELD:
			case ZEND_YIELD_FROM:
				if (build_flags & ZEND_CFG_STACKLESS) {
					BB_START(i + 1);
				}
				break;
			case ZEND_DO_FCALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
				flags |= ZEND_FUNC_HAS_CALLS;
				if (build_flags & ZEND_CFG_STACKLESS) {
					BB_START(i + 1);
				}
				break;
			case ZEND_DO_ICALL:
				flags |= ZEND_FUNC_HAS_CALLS;
				break;
			case ZEND_INIT_FCALL:
			case ZEND_INIT_NS_FCALL_BY_NAME:
				zv = CRT_CONSTANT(opline->op2);
				if (opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
					/* The third literal is the lowercased unqualified name */
					zv += 2;
				}
				if ((fn = zend_hash_find_ptr(EG(function_table), Z_STR_P(zv))) != NULL) {
					if (fn->type == ZEND_INTERNAL_FUNCTION) {
						flags |= zend_optimizer_classify_function(
							Z_STR_P(zv), opline->extended_value);
					}
				}
				break;
			case ZEND_FAST_CALL:
				BB_START(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_FAST_RET:
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_JMP:
				BB_START(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes);
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_JMPZNZ:
				BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
				BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_CATCH:
				if (!opline->result.num) {
					BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
				}
				BB_START(i + 1);
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
				BB_START(i + 1);
				break;
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
				BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_UNSET_VAR:
			case ZEND_ISSET_ISEMPTY_VAR:
				if (((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL) &&
				    !(opline->extended_value & ZEND_QUICK_SET)) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				} else if (((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL ||
				            (opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL_LOCK) &&
				           !op_array->function_name) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				}
				break;
			case ZEND_FETCH_R:
			case ZEND_FETCH_W:
			case ZEND_FETCH_RW:
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_IS:
			case ZEND_FETCH_UNSET:
				if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				} else if (((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL ||
				            (opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL_LOCK) &&
				           !op_array->function_name) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				}
				break;
		}
	}

	/* If the entry block has predecessors, we may need to split it */
	if ((build_flags & ZEND_CFG_NO_ENTRY_PREDECESSORS)
			&& op_array->last > 0 && block_map[0] > 1) {
		extra_entry_block = 1;
	}

	if (cfg->split_at_live_ranges) {
		for (j = 0; j < op_array->last_live_range; j++) {
			BB_START(op_array->live_range[j].start);
			BB_START(op_array->live_range[j].end);
		}
	}

	if (op_array->last_try_catch) {
		for (j = 0; j < op_array->last_try_catch; j++) {
			BB_START(op_array->try_catch_array[j].try_op);
			if (op_array->try_catch_array[j].catch_op) {
				BB_START(op_array->try_catch_array[j].catch_op);
			}
			if (op_array->try_catch_array[j].finally_op) {
				BB_START(op_array->try_catch_array[j].finally_op);
			}
			if (op_array->try_catch_array[j].finally_end) {
				BB_START(op_array->try_catch_array[j].finally_end);
			}
		}
	}

	blocks_count += extra_entry_block;
	cfg->blocks_count = blocks_count;

	/* Build CFG, Step 2: Build Array of Basic Blocks */
	cfg->blocks = blocks = zend_arena_calloc(arena, sizeof(zend_basic_block), blocks_count);
	if (!blocks) {
		return FAILURE;
	}

	blocks_count = -1;

	if (extra_entry_block) {
		initialize_block(&blocks[0]);
		blocks[0].start = 0;
		blocks[0].len = 0;
		blocks_count++;
	}

	for (i = 0; i < op_array->last; i++) {
		if (block_map[i]) {
			if (blocks_count >= 0) {
				blocks[blocks_count].len = i - blocks[blocks_count].start;
			}
			blocks_count++;
			initialize_block(&blocks[blocks_count]);
			blocks[blocks_count].start = i;
		}
		block_map[i] = blocks_count;
	}

	blocks[blocks_count].len = i - blocks[blocks_count].start;
	blocks_count++;

	/* Build CFG, Step 3: Calculate successors */
	for (j = 0; j < blocks_count; j++) {
		zend_op *opline;
		if (blocks[j].len == 0) {
			record_successor(blocks, j, 0, j + 1);
			continue;
		}

		opline = op_array->opcodes + blocks[j].start + blocks[j].len - 1;
		switch (opline->opcode) {
			case ZEND_FAST_RET:
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
			case ZEND_EXIT:
			case ZEND_THROW:
				break;
			case ZEND_JMP:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes]);
				break;
			case ZEND_JMPZNZ:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes]);
				record_successor(blocks, j, 1, block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes]);
				record_successor(blocks, j, 1, j + 1);
				break;
			case ZEND_CATCH:
				if (!opline->result.num) {
					record_successor(blocks, j, 0, block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
					record_successor(blocks, j, 1, j + 1);
				} else {
					record_successor(blocks, j, 0, j + 1);
				}
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				record_successor(blocks, j, 0, block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
				record_successor(blocks, j, 1, j + 1);
				break;
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes]);
				record_successor(blocks, j, 1, j + 1);
				break;
			case ZEND_FAST_CALL:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes]);
				record_successor(blocks, j, 1, j + 1);
				break;
			default:
				record_successor(blocks, j, 0, j + 1);
				break;
		}
	}

	/* Build CFG, Step 4, Mark Reachable Basic Blocks */
	zend_mark_reachable_blocks(op_array, cfg, 0);

	if (func_flags) {
		*func_flags |= flags;
	}

	return SUCCESS;
}
/* }}} */

int zend_cfg_build_predecessors(zend_arena **arena, zend_cfg *cfg) /* {{{ */
{
	int j, edges;
	zend_basic_block *b;
	zend_basic_block *blocks = cfg->blocks;
	zend_basic_block *end = blocks + cfg->blocks_count;
	int *predecessors;

	edges = 0;
	for (b = blocks; b < end; b++) {
		b->predecessors_count = 0;
	}
	for (b = blocks; b < end; b++) {
		if (!(b->flags & ZEND_BB_REACHABLE)) {
			b->successors[0] = -1;
			b->successors[1] = -1;
			b->predecessors_count = 0;
		} else {
			if (b->successors[0] >= 0) {
				edges++;
				blocks[b->successors[0]].predecessors_count++;
				if (b->successors[1] >= 0 && b->successors[1] != b->successors[0]) {
					edges++;
					blocks[b->successors[1]].predecessors_count++;
				}
			}
		}
	}

	cfg->predecessors = predecessors = (int*)zend_arena_calloc(arena, sizeof(int), edges);

	if (!predecessors) {
		return FAILURE;
	}

	edges = 0;
	for (b = blocks; b < end; b++) {
		if (b->flags & ZEND_BB_REACHABLE) {
			b->predecessor_offset = edges;
			edges += b->predecessors_count;
			b->predecessors_count = 0;
		}
	}

	for (j = 0; j < cfg->blocks_count; j++) {
		if (blocks[j].flags & ZEND_BB_REACHABLE) {
			if (blocks[j].successors[0] >= 0) {
				zend_basic_block *b = blocks + blocks[j].successors[0];
				predecessors[b->predecessor_offset + b->predecessors_count] = j;
				b->predecessors_count++;
				if (blocks[j].successors[1] >= 0
						&& blocks[j].successors[1] != blocks[j].successors[0]) {
					zend_basic_block *b = blocks + blocks[j].successors[1];
					predecessors[b->predecessor_offset + b->predecessors_count] = j;
					b->predecessors_count++;
				}
			}
		}
	}

	return SUCCESS;
}
/* }}} */

/* Computes a postorder numbering of the CFG */
static void compute_postnum_recursive(
		int *postnum, int *cur, const zend_cfg *cfg, int block_num) /* {{{ */
{
	zend_basic_block *block = &cfg->blocks[block_num];
	if (postnum[block_num] != -1) {
		return;
	}

	postnum[block_num] = -2; /* Marker for "currently visiting" */
	if (block->successors[0] >= 0) {
		compute_postnum_recursive(postnum, cur, cfg, block->successors[0]);
		if (block->successors[1] >= 0) {
			compute_postnum_recursive(postnum, cur, cfg, block->successors[1]);
		}
	}
	postnum[block_num] = (*cur)++;
}
/* }}} */

/* Computes dominator tree using algorithm from "A Simple, Fast Dominance Algorithm" by
 * Cooper, Harvey and Kennedy. */
int zend_cfg_compute_dominators_tree(const zend_op_array *op_array, zend_cfg *cfg) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;
	int blocks_count = cfg->blocks_count;
	int j, k, changed;

	ALLOCA_FLAG(use_heap)
	int *postnum = do_alloca(sizeof(int) * cfg->blocks_count, use_heap);
	memset(postnum, -1, sizeof(int) * cfg->blocks_count);
	j = 0;
	compute_postnum_recursive(postnum, &j, cfg, 0);

	/* FIXME: move declarations */
	blocks[0].idom = 0;
	do {
		changed = 0;
		/* Iterating in RPO here would converge faster */
		for (j = 1; j < blocks_count; j++) {
			int idom = -1;

			if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
				continue;
			}
			for (k = 0; k < blocks[j].predecessors_count; k++) {
				int pred = cfg->predecessors[blocks[j].predecessor_offset + k];

				if (idom < 0) {
					if (blocks[pred].idom >= 0)
						idom = pred;
					continue;
				}

				if (blocks[pred].idom >= 0) {
					while (idom != pred) {
						while (postnum[pred] < postnum[idom]) pred = blocks[pred].idom;
						while (postnum[idom] < postnum[pred]) idom = blocks[idom].idom;
					}
				}
			}

			if (idom >= 0 && blocks[j].idom != idom) {
				blocks[j].idom = idom;
				changed = 1;
			}
		}
	} while (changed);
	blocks[0].idom = -1;

	for (j = 1; j < blocks_count; j++) {
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		if (blocks[j].idom >= 0) {
			/* Sort by block number to traverse children in pre-order */
			if (blocks[blocks[j].idom].children < 0 ||
			    j < blocks[blocks[j].idom].children) {
				blocks[j].next_child = blocks[blocks[j].idom].children;
				blocks[blocks[j].idom].children = j;
			} else {
				int k = blocks[blocks[j].idom].children;
				while (blocks[k].next_child >=0 && j > blocks[k].next_child) {
					k = blocks[k].next_child;
				}
				blocks[j].next_child = blocks[k].next_child;
				blocks[k].next_child = j;
			}
		}
	}

	for (j = 0; j < blocks_count; j++) {
		int idom = blocks[j].idom, level = 0;
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		while (idom >= 0) {
			level++;
			if (blocks[idom].level >= 0) {
				level += blocks[idom].level;
				break;
			} else {
				idom = blocks[idom].idom;
			}
		}
		blocks[j].level = level;
	}

	free_alloca(postnum, use_heap);
	return SUCCESS;
}
/* }}} */

static int dominates(zend_basic_block *blocks, int a, int b) /* {{{ */
{
	while (blocks[b].level > blocks[a].level) {
		b = blocks[b].idom;
	}
	return a == b;
}
/* }}} */

int zend_cfg_identify_loops(const zend_op_array *op_array, zend_cfg *cfg, uint32_t *flags) /* {{{ */
{
	int i, j, k;
	int depth;
	zend_basic_block *blocks = cfg->blocks;
	int *dj_spanning_tree;
	zend_worklist work;
	int flag = ZEND_FUNC_NO_LOOPS;
	ALLOCA_FLAG(list_use_heap)
	ALLOCA_FLAG(tree_use_heap)

	ZEND_WORKLIST_ALLOCA(&work, cfg->blocks_count, list_use_heap);
	dj_spanning_tree = do_alloca(sizeof(int) * cfg->blocks_count, tree_use_heap);

	for (i = 0; i < cfg->blocks_count; i++) {
		dj_spanning_tree[i] = -1;
	}
	zend_worklist_push(&work, 0);
	while (zend_worklist_len(&work)) {
	next:
		i = zend_worklist_peek(&work);
		/* Visit blocks immediately dominated by i. */
		for (j = blocks[i].children; j >= 0; j = blocks[j].next_child) {
			if (zend_worklist_push(&work, j)) {
				dj_spanning_tree[j] = i;
				goto next;
			}
		}
		/* Visit join edges.  */
		for (j = 0; j < 2; j++) {
			int succ = blocks[i].successors[j];
			if (succ < 0) {
				continue;
			} else if (blocks[succ].idom == i) {
				continue;
			} else if (zend_worklist_push(&work, succ)) {
				dj_spanning_tree[succ] = i;
				goto next;
			}
		}
		zend_worklist_pop(&work);
	}

	/* Identify loops.  See Sreedhar et al, "Identifying Loops Using DJ
	   Graphs".  */

	for (i = 0, depth = 0; i < cfg->blocks_count; i++) {
		if (blocks[i].level > depth) {
			depth = blocks[i].level;
		}
	}
	for (; depth >= 0; depth--) {
		for (i = 0; i < cfg->blocks_count; i++) {
			if (blocks[i].level != depth) {
				continue;
			}
			zend_bitset_clear(work.visited, zend_bitset_len(cfg->blocks_count));
			for (j = 0; j < blocks[i].predecessors_count; j++) {
				int pred = cfg->predecessors[blocks[i].predecessor_offset + j];

				/* A join edge is one for which the predecessor does not
				   immediately dominate the successor.  */
				if (blocks[i].idom == pred) {
					continue;
				}

				/* In a loop back-edge (back-join edge), the successor dominates
				   the predecessor.  */
				if (dominates(blocks, i, pred)) {
					blocks[i].flags |= ZEND_BB_LOOP_HEADER;
					flag &= ~ZEND_FUNC_NO_LOOPS;
					zend_worklist_push(&work, pred);
				} else {
					/* Otherwise it's a cross-join edge.  See if it's a branch
					   to an ancestor on the dominator spanning tree.  */
					int dj_parent = pred;
					while (dj_parent >= 0) {
						if (dj_parent == i) {
							/* An sp-back edge: mark as irreducible.  */
							blocks[i].flags |= ZEND_BB_IRREDUCIBLE_LOOP;
							flag |= ZEND_FUNC_IRREDUCIBLE;
							flag &= ~ZEND_FUNC_NO_LOOPS;
							break;
						} else {
							dj_parent = dj_spanning_tree[dj_parent];
						}
					}
				}
			}
			while (zend_worklist_len(&work)) {
				j = zend_worklist_pop(&work);
				if (blocks[j].loop_header < 0 && j != i) {
					blocks[j].loop_header = i;
					for (k = 0; k < blocks[j].predecessors_count; k++) {
						zend_worklist_push(&work, cfg->predecessors[blocks[j].predecessor_offset + k]);
					}
				}
			}
		}
	}

	free_alloca(dj_spanning_tree, tree_use_heap);
	ZEND_WORKLIST_FREE_ALLOCA(&work, list_use_heap);
	*flags |= flag;

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
