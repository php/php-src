/*
   +----------------------------------------------------------------------+
   | Zend Engine, CFG - Control Flow Graph                                |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_compile.h"
#include "zend_cfg.h"
#include "zend_func_info.h"
#include "zend_worklist.h"
#include "zend_optimizer.h"
#include "zend_optimizer_internal.h"
#include "zend_sort.h"

static void zend_mark_reachable(zend_op *opcodes, zend_cfg *cfg, zend_basic_block *b) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;

	while (1) {
		int i;

		b->flags |= ZEND_BB_REACHABLE;
		if (b->successors_count == 0) {
			b->flags |= ZEND_BB_EXIT;
			return;
		}

		for (i = 0; i < b->successors_count; i++) {
			zend_basic_block *succ = blocks + b->successors[i];

			if (b->len != 0) {
				uint8_t opcode = opcodes[b->start + b->len - 1].opcode;
				if (opcode == ZEND_MATCH) {
					succ->flags |= ZEND_BB_TARGET;
				} else if (opcode == ZEND_SWITCH_LONG || opcode == ZEND_SWITCH_STRING) {
					if (i == b->successors_count - 1) {
						succ->flags |= ZEND_BB_FOLLOW | ZEND_BB_TARGET;
					} else {
						succ->flags |= ZEND_BB_TARGET;
					}
				} else if (b->successors_count == 1) {
					if (opcode == ZEND_JMP) {
						succ->flags |= ZEND_BB_TARGET;
					} else {
						succ->flags |= ZEND_BB_FOLLOW;

						if ((cfg->flags & ZEND_CFG_STACKLESS)) {
							if (opcode == ZEND_INCLUDE_OR_EVAL ||
								opcode == ZEND_GENERATOR_CREATE ||
								opcode == ZEND_YIELD ||
								opcode == ZEND_YIELD_FROM ||
								opcode == ZEND_DO_FCALL ||
								opcode == ZEND_DO_UCALL ||
								opcode == ZEND_DO_FCALL_BY_NAME) {
								succ->flags |= ZEND_BB_ENTRY;
							}
						}
						if ((cfg->flags & ZEND_CFG_RECV_ENTRY)) {
							if (opcode == ZEND_RECV ||
								opcode == ZEND_RECV_INIT) {
								succ->flags |= ZEND_BB_RECV_ENTRY;
							}
						}
					}
				} else {
					ZEND_ASSERT(b->successors_count == 2);
					if (i == 0) {
						succ->flags |= ZEND_BB_TARGET;
					} else {
						succ->flags |= ZEND_BB_FOLLOW;
					}
				}
			} else {
				succ->flags |= ZEND_BB_FOLLOW;
			}

			if (i == b->successors_count - 1) {
				/* Tail call optimization */
				if (succ->flags & ZEND_BB_REACHABLE) {
					return;
				}

				b = succ;
				break;
			} else {
				/* Recursively check reachability */
				if (!(succ->flags & ZEND_BB_REACHABLE)) {
					zend_mark_reachable(opcodes, cfg, succ);
				}
			}
		}
	}
}
/* }}} */

static void zend_mark_reachable_blocks(const zend_op_array *op_array, zend_cfg *cfg, int start) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;

	blocks[start].flags = ZEND_BB_START;
	zend_mark_reachable(op_array->opcodes, cfg, blocks + start);

	if (op_array->last_try_catch) {
		zend_basic_block *b;
		int j, changed;
		uint32_t *block_map = cfg->map;

		do {
			changed = 0;

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
									zend_mark_reachable(op_array->opcodes, cfg, blocks + block_map[op_array->try_catch_array[j].try_op]);
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
							zend_mark_reachable(op_array->opcodes, cfg, b);
						}
					}
					if (op_array->try_catch_array[j].finally_op) {
						b = blocks + block_map[op_array->try_catch_array[j].finally_op];
						b->flags |= ZEND_BB_FINALLY;
						if (!(b->flags & ZEND_BB_REACHABLE)) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, cfg, b);
						}
					}
					if (op_array->try_catch_array[j].finally_end) {
						b = blocks + block_map[op_array->try_catch_array[j].finally_end];
						b->flags |= ZEND_BB_FINALLY_END;
						if (!(b->flags & ZEND_BB_REACHABLE)) {
							changed = 1;
							zend_mark_reachable(op_array->opcodes, cfg, b);
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

	if (cfg->flags & ZEND_FUNC_FREE_LOOP_VAR) {
		zend_basic_block *b;
		int j;
		uint32_t *block_map = cfg->map;

		/* Mark blocks that are unreachable, but free a loop var created in a reachable block. */
		for (b = blocks; b < blocks + cfg->blocks_count; b++) {
			if (b->flags & ZEND_BB_REACHABLE) {
				continue;
			}

			for (j = b->start; j < b->start + b->len; j++) {
				zend_op *opline = &op_array->opcodes[j];
				if (zend_optimizer_is_loop_var_free(opline)) {
					zend_op *def_opline = zend_optimizer_get_loop_var_def(op_array, opline);
					if (def_opline) {
						uint32_t def_block = block_map[def_opline - op_array->opcodes];
						if (blocks[def_block].flags & ZEND_BB_REACHABLE) {
							b->flags |= ZEND_BB_UNREACHABLE_FREE;
							break;
						}
					}
				}
			}
		}
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

static void initialize_block(zend_basic_block *block) {
	block->flags = 0;
	block->successors = block->successors_storage;
	block->successors_count = 0;
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

ZEND_API void zend_build_cfg(zend_arena **arena, const zend_op_array *op_array, uint32_t build_flags, zend_cfg *cfg) /* {{{ */
{
	uint32_t flags = 0;
	uint32_t i;
	int j;
	uint32_t *block_map;
	zend_function *fn;
	int blocks_count = 0;
	zend_basic_block *blocks;
	zval *zv;
	bool extra_entry_block = 0;

	cfg->flags = build_flags & (ZEND_CFG_STACKLESS|ZEND_CFG_RECV_ENTRY);

	cfg->map = block_map = zend_arena_calloc(arena, op_array->last, sizeof(uint32_t));

	/* Build CFG, Step 1: Find basic blocks starts, calculate number of blocks */
	BB_START(0);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = op_array->opcodes + i;
		switch (opline->opcode) {
			case ZEND_RECV:
			case ZEND_RECV_INIT:
				if (build_flags & ZEND_CFG_RECV_ENTRY) {
					BB_START(i + 1);
				}
				break;
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
			case ZEND_VERIFY_NEVER_TYPE:
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_MATCH_ERROR:
			case ZEND_EXIT:
			case ZEND_THROW:
				/* Don't treat THROW as terminator if it's used in expression context,
				 * as we may lose live ranges when eliminating unreachable code. */
				if (opline->extended_value != ZEND_THROW_IS_EXPR && i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_INCLUDE_OR_EVAL:
				flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				ZEND_FALLTHROUGH;
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
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
			case ZEND_BIND_INIT_STATIC_OR_JMP:
				BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_CATCH:
				if (!(opline->extended_value & ZEND_LAST_CATCH)) {
					BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				}
				BB_START(i + 1);
				break;
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
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			case ZEND_MATCH:
			{
				HashTable *jumptable = Z_ARRVAL_P(CRT_CONSTANT(opline->op2));
				zval *zv;
				ZEND_HASH_FOREACH_VAL(jumptable, zv) {
					BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv)));
				} ZEND_HASH_FOREACH_END();
				BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
				BB_START(i + 1);
				break;
			}
			case ZEND_FETCH_R:
			case ZEND_FETCH_W:
			case ZEND_FETCH_RW:
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_IS:
			case ZEND_FETCH_UNSET:
			case ZEND_UNSET_VAR:
			case ZEND_ISSET_ISEMPTY_VAR:
				if (opline->extended_value & ZEND_FETCH_LOCAL) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				} else if ((opline->extended_value & (ZEND_FETCH_GLOBAL | ZEND_FETCH_GLOBAL_LOCK)) &&
				           !op_array->function_name) {
					flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
				}
				break;
			case ZEND_FUNC_GET_ARGS:
				flags |= ZEND_FUNC_VARARG;
				break;
			case ZEND_EXT_STMT:
				flags |= ZEND_FUNC_HAS_EXTENDED_STMT;
				break;
			case ZEND_EXT_FCALL_BEGIN:
			case ZEND_EXT_FCALL_END:
				flags |= ZEND_FUNC_HAS_EXTENDED_FCALL;
				break;
			case ZEND_FREE:
			case ZEND_FE_FREE:
				if (zend_optimizer_is_loop_var_free(opline)
				 && ((opline-1)->opcode != ZEND_MATCH_ERROR
				  || (opline-1)->extended_value != ZEND_THROW_IS_EXPR)) {
					BB_START(i);
					flags |= ZEND_FUNC_FREE_LOOP_VAR;
				}
				break;
		}
	}

	/* If the entry block has predecessors, we may need to split it */
	if ((build_flags & ZEND_CFG_NO_ENTRY_PREDECESSORS)
			&& op_array->last > 0 && block_map[0] > 1) {
		extra_entry_block = 1;
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
		zend_basic_block *block = &blocks[j];
		zend_op *opline;
		if (block->len == 0) {
			block->successors_count = 1;
			block->successors[0] = j + 1;
			continue;
		}

		opline = op_array->opcodes + block->start + block->len - 1;
		switch (opline->opcode) {
			case ZEND_FAST_RET:
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
			case ZEND_EXIT:
			case ZEND_THROW:
			case ZEND_MATCH_ERROR:
			case ZEND_VERIFY_NEVER_TYPE:
				break;
			case ZEND_JMP:
				block->successors_count = 1;
				block->successors[0] = block_map[OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes];
				break;
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
			case ZEND_BIND_INIT_STATIC_OR_JMP:
				block->successors_count = 2;
				block->successors[0] = block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes];
				block->successors[1] = j + 1;
				break;
			case ZEND_CATCH:
				if (!(opline->extended_value & ZEND_LAST_CATCH)) {
					block->successors_count = 2;
					block->successors[0] = block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes];
					block->successors[1] = j + 1;
				} else {
					block->successors_count = 1;
					block->successors[0] = j + 1;
				}
				break;
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				block->successors_count = 2;
				block->successors[0] = block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)];
				block->successors[1] = j + 1;
				break;
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
				block->successors_count = 2;
				block->successors[0] = block_map[OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes];
				block->successors[1] = j + 1;
				break;
			case ZEND_FAST_CALL:
				block->successors_count = 2;
				block->successors[0] = block_map[OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes];
				block->successors[1] = j + 1;
				break;
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			case ZEND_MATCH:
			{
				HashTable *jumptable = Z_ARRVAL_P(CRT_CONSTANT(opline->op2));
				zval *zv;
				uint32_t s = 0;

				block->successors_count = (opline->opcode == ZEND_MATCH ? 1 : 2) + zend_hash_num_elements(jumptable);
				block->successors = zend_arena_calloc(arena, block->successors_count, sizeof(int));

				ZEND_HASH_FOREACH_VAL(jumptable, zv) {
					block->successors[s++] = block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv))];
				} ZEND_HASH_FOREACH_END();

				block->successors[s++] = block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)];
				if (opline->opcode != ZEND_MATCH) {
					block->successors[s++] = j + 1;
				}
				break;
			}
			default:
				block->successors_count = 1;
				block->successors[0] = j + 1;
				break;
		}
	}

	/* Build CFG, Step 4, Mark Reachable Basic Blocks */
	cfg->flags |= flags;
	zend_mark_reachable_blocks(op_array, cfg, 0);
}
/* }}} */

ZEND_API void zend_cfg_build_predecessors(zend_arena **arena, zend_cfg *cfg) /* {{{ */
{
	int j, s, edges;
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
			b->successors_count = 0;
			b->predecessors_count = 0;
		} else {
			for (s = 0; s < b->successors_count; s++) {
				edges++;
				blocks[b->successors[s]].predecessors_count++;
			}
		}
	}

	cfg->edges_count = edges;
	cfg->predecessors = predecessors = (int*)zend_arena_calloc(arena, sizeof(int), edges);

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
			/* SWITCH_STRING/LONG may have few identical successors */
			for (s = 0; s < blocks[j].successors_count; s++) {
				int duplicate = 0;
				int p;

				for (p = 0; p < s; p++) {
					if (blocks[j].successors[p] == blocks[j].successors[s]) {
						duplicate = 1;
						break;
					}
				}
				if (!duplicate) {
					zend_basic_block *b = blocks + blocks[j].successors[s];

					predecessors[b->predecessor_offset + b->predecessors_count] = j;
					b->predecessors_count++;
				}
			}
		}
	}
}
/* }}} */

/* Computes a postorder numbering of the CFG */
static void compute_postnum_recursive(
		int *postnum, int *cur, const zend_cfg *cfg, int block_num) /* {{{ */
{
	int s;
	zend_basic_block *block = &cfg->blocks[block_num];
	if (postnum[block_num] != -1) {
		return;
	}

	postnum[block_num] = -2; /* Marker for "currently visiting" */
	for (s = 0; s < block->successors_count; s++) {
		compute_postnum_recursive(postnum, cur, cfg, block->successors[s]);
	}
	postnum[block_num] = (*cur)++;
}
/* }}} */

/* Computes dominator tree using algorithm from "A Simple, Fast Dominance Algorithm" by
 * Cooper, Harvey and Kennedy. */
ZEND_API void zend_cfg_compute_dominators_tree(const zend_op_array *op_array, zend_cfg *cfg) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;
	int blocks_count = cfg->blocks_count;
	int j, k, changed;

	if (cfg->blocks_count == 1) {
		blocks[0].level = 0;
		return;
	}

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

				if (blocks[pred].idom >= 0) {
					if (idom < 0) {
						idom = pred;
					} else {
						while (idom != pred) {
							while (postnum[pred] < postnum[idom]) pred = blocks[pred].idom;
							while (postnum[idom] < postnum[pred]) idom = blocks[idom].idom;
						}
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
}
/* }}} */

static bool dominates(zend_basic_block *blocks, int a, int b) /* {{{ */
{
	while (blocks[b].level > blocks[a].level) {
		b = blocks[b].idom;
	}
	return a == b;
}
/* }}} */

ZEND_API void zend_cfg_identify_loops(const zend_op_array *op_array, zend_cfg *cfg) /* {{{ */
{
	int i, j, k, n;
	int time;
	zend_basic_block *blocks = cfg->blocks;
	int *entry_times, *exit_times;
	zend_worklist work;
	int flag = ZEND_FUNC_NO_LOOPS;
	int *sorted_blocks;
	ALLOCA_FLAG(list_use_heap)
	ALLOCA_FLAG(tree_use_heap)

	if (cfg->blocks_count == 1) {
		cfg->flags |= flag;
		return;
	}

	ZEND_WORKLIST_ALLOCA(&work, cfg->blocks_count, list_use_heap);

	/* We don't materialize the DJ spanning tree explicitly, as we are only interested in ancestor
	 * queries. These are implemented by checking entry/exit times of the DFS search. */
	entry_times = do_alloca(3 * sizeof(int) * cfg->blocks_count, tree_use_heap);
	exit_times = entry_times + cfg->blocks_count;
	sorted_blocks = exit_times + cfg->blocks_count;
	memset(entry_times, -1, 2 * sizeof(int) * cfg->blocks_count);

	zend_worklist_push(&work, 0);
	time = 0;
	while (zend_worklist_len(&work)) {
	next:
		i = zend_worklist_peek(&work);
		if (entry_times[i] == -1) {
			entry_times[i] = time++;
		}
		/* Visit blocks immediately dominated by i. */
		for (j = blocks[i].children; j >= 0; j = blocks[j].next_child) {
			if (zend_worklist_push(&work, j)) {
				goto next;
			}
		}
		/* Visit join edges.  */
		for (j = 0; j < blocks[i].successors_count; j++) {
			int succ = blocks[i].successors[j];
			if (blocks[succ].idom == i) {
				continue;
			} else if (zend_worklist_push(&work, succ)) {
				goto next;
			}
		}
		exit_times[i] = time++;
		zend_worklist_pop(&work);
	}

	/* Sort blocks by level, which is the opposite order in which we want to process them */
	sorted_blocks[0] = 0;
	j = 0;
	n = 1;
	while (j != n) {
		i = j;
		j = n;
		for (; i < j; i++) {
			int child;
			for (child = blocks[sorted_blocks[i]].children; child >= 0; child = blocks[child].next_child) {
				sorted_blocks[n++] = child;
			}
		}
	}

	/* Identify loops. See Sreedhar et al, "Identifying Loops Using DJ Graphs". */
	while (n > 0) {
		i = sorted_blocks[--n];

		if (blocks[i].predecessors_count < 2) {
		    /* loop header has at least two input edges */
			continue;
		}

		for (j = 0; j < blocks[i].predecessors_count; j++) {
			int pred = cfg->predecessors[blocks[i].predecessor_offset + j];

			/* A join edge is one for which the predecessor does not
			   immediately dominate the successor. */
			if (blocks[i].idom == pred) {
				continue;
			}

			/* In a loop back-edge (back-join edge), the successor dominates
			   the predecessor.  */
			if (dominates(blocks, i, pred)) {
				blocks[i].flags |= ZEND_BB_LOOP_HEADER;
				flag &= ~ZEND_FUNC_NO_LOOPS;
				if (!zend_worklist_len(&work)) {
					zend_bitset_clear(work.visited, zend_bitset_len(cfg->blocks_count));
				}
				zend_worklist_push(&work, pred);
			} else {
				/* Otherwise it's a cross-join edge.  See if it's a branch
				   to an ancestor on the DJ spanning tree.  */
				if (entry_times[pred] > entry_times[i] && exit_times[pred] < exit_times[i]) {
					blocks[i].flags |= ZEND_BB_IRREDUCIBLE_LOOP;
					flag |= ZEND_FUNC_IRREDUCIBLE;
					flag &= ~ZEND_FUNC_NO_LOOPS;
				}
			}
		}
		while (zend_worklist_len(&work)) {
			j = zend_worklist_pop(&work);
			while (blocks[j].loop_header >= 0) {
				j = blocks[j].loop_header;
			}
			if (j != i) {
				if (blocks[j].idom < 0 && j != 0) {
					/* Ignore blocks that are unreachable or only abnormally reachable. */
					continue;
				}
				blocks[j].loop_header = i;
				for (k = 0; k < blocks[j].predecessors_count; k++) {
					zend_worklist_push(&work, cfg->predecessors[blocks[j].predecessor_offset + k]);
				}
			}
		}
	}

	free_alloca(entry_times, tree_use_heap);
	ZEND_WORKLIST_FREE_ALLOCA(&work, list_use_heap);

	cfg->flags |= flag;
}
/* }}} */
