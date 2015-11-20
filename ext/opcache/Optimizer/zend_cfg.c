/*
   +----------------------------------------------------------------------+
   | Zend Engine, CFG - Control Flow Graph                                |
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
#include "zend_compile.h"
#include "zend_cfg.h"

static void zend_mark_reachable(zend_op *opcodes, zend_basic_block *blocks, zend_basic_block *b)
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
				opcode = opcodes[b->end].opcode;
				b = blocks + successor_1;
				if (opcode == ZEND_JMPZNZ) {
					b->flags |= ZEND_BB_TARGET;
				} else {
					b->flags |= ZEND_BB_FOLLOW;
				}
			} else {
				opcode = opcodes[b->end].opcode;
				b = blocks + successor_0;
				if (opcode == ZEND_JMP) {
					b->flags |= ZEND_BB_TARGET;
				} else {
					b->flags |= ZEND_BB_FOLLOW;

					//TODO: support for stackless CFG???
					if (0/*stackless*/) {
						if (opcode == ZEND_INCLUDE_OR_EVAL ||
						    opcode == ZEND_YIELD ||
						    opcode == ZEND_YIELD_FROM ||
						    opcode == ZEND_DO_FCALL ||
						    opcode == ZEND_DO_UCALL ||
						    opcode == ZEND_DO_FCALL_BY_NAME) {
							b->flags |= ZEND_BB_ENTRY;
						}
					}
				}
			}
			if (b->flags & ZEND_BB_REACHABLE) return;
		} else {
			b->flags |= ZEND_BB_EXIT;
			return;
		}
	}
}

static void zend_mark_reachable_blocks(zend_op_array *op_array, zend_cfg *cfg, int start)
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

			/* Add brk/cont paths */
			for (j = 0; j < op_array->last_live_range; j++) {
				b = blocks + block_map[op_array->live_range[j].start];
				if (b->flags & ZEND_BB_REACHABLE) {
					b->flags |= ZEND_BB_GEN_VAR;
					b = blocks + block_map[op_array->live_range[j].end];
					b->flags |= ZEND_BB_KILL_VAR;
					if (!(b->flags & ZEND_BB_REACHABLE)) {
						changed = 1;
						zend_mark_reachable(op_array->opcodes, blocks, b);
					}
				} else {
					ZEND_ASSERT(!(blocks[block_map[op_array->live_range[j].end]].flags & ZEND_BB_REACHABLE));
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

void zend_remark_reachable_blocks(zend_op_array *op_array, zend_cfg *cfg)
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

static void record_successor(zend_basic_block *blocks, int pred, int n, int succ)
{
	blocks[pred].successors[n] = succ;
}

#define BB_START(i) do { \
		if (!block_map[i]) { blocks_count++;} \
		block_map[i] = 1; \
	} while (0)

int zend_build_cfg(zend_arena **arena, zend_op_array *op_array, int rt_constants, int stackless, zend_cfg *cfg, uint32_t *func_flags) /* {{{ */
{
	uint32_t flags = 0;
	uint32_t i;
	int j;
	uint32_t *block_map;
	zend_function *fn;
	int blocks_count = 0;
	zend_basic_block *blocks;
	zval *zv;

	cfg->map = block_map = zend_arena_calloc(arena, op_array->last, sizeof(uint32_t));
	if (!block_map) {
		return FAILURE;
	}

	/* Build CFG, Step 1: Find basic blocks starts, calculate number of blocks */
	BB_START(0);
	if ((op_array->fn_flags & ZEND_ACC_CLOSURE) && op_array->static_variables) {
		// FIXME: Really we should try to perform variable initialization
		flags |= ZEND_FUNC_TOO_DYNAMIC;
	}
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
			case ZEND_YIELD:
			case ZEND_YIELD_FROM:
				flags |= ZEND_FUNC_TOO_DYNAMIC;
				if (stackless) {
					BB_START(i + 1);
				}
				break;
			case ZEND_DO_FCALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
				flags |= ZEND_FUNC_HAS_CALLS;
				if (stackless) {
					BB_START(i + 1);
				}
				break;
			case ZEND_DO_ICALL:
				flags |= ZEND_FUNC_HAS_CALLS;
				break;
			case ZEND_INIT_FCALL:
				if (rt_constants) {
					zv = RT_CONSTANT(op_array, opline->op2);
				} else {
					zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
				}
				if ((fn = zend_hash_find_ptr(EG(function_table), Z_STR_P(zv))) != NULL) {
					if (fn->type == ZEND_INTERNAL_FUNCTION) {
						if (Z_STRLEN_P(zv) == sizeof("extract")-1 &&
						    memcmp(Z_STRVAL_P(zv), "extract", sizeof("extract")-1) == 0) {
							flags |= ZEND_FUNC_TOO_DYNAMIC;
						} else if (Z_STRLEN_P(zv) == sizeof("compact")-1 &&
						    memcmp(Z_STRVAL_P(zv), "compact", sizeof("compact")-1) == 0) {
							flags |= ZEND_FUNC_TOO_DYNAMIC;
						} else if (Z_STRLEN_P(zv) == sizeof("parse_str")-1 &&
						    memcmp(Z_STRVAL_P(zv), "parse_str", sizeof("parse_str")-1) == 0) {
							flags |= ZEND_FUNC_TOO_DYNAMIC;
						} else if (Z_STRLEN_P(zv) == sizeof("mb_parse_str")-1 &&
						    memcmp(Z_STRVAL_P(zv), "mb_parse_str", sizeof("mb_parse_str")-1) == 0) {
							flags |= ZEND_FUNC_TOO_DYNAMIC;
						} else if (Z_STRLEN_P(zv) == sizeof("get_defined_vars")-1 &&
						    memcmp(Z_STRVAL_P(zv), "get_defined_vars", sizeof("get_defined_vars")-1) == 0) {
							flags |= ZEND_FUNC_TOO_DYNAMIC;
						} else if (Z_STRLEN_P(zv) == sizeof("func_num_args")-1 &&
						    memcmp(Z_STRVAL_P(zv), "func_num_args", sizeof("func_num_args")-1) == 0) {
							flags |= ZEND_FUNC_VARARG;
						} else if (Z_STRLEN_P(zv) == sizeof("func_get_arg")-1 &&
						    memcmp(Z_STRVAL_P(zv), "func_get_arg", sizeof("func_get_arg")-1) == 0) {
							flags |= ZEND_FUNC_VARARG;
						} else if (Z_STRLEN_P(zv) == sizeof("func_get_args")-1 &&
						    memcmp(Z_STRVAL_P(zv), "func_get_args", sizeof("func_get_args")-1) == 0) {
							flags |= ZEND_FUNC_VARARG;
						}
					}
				}
				break;
			case ZEND_FAST_CALL:
				flags |= ZEND_FUNC_TOO_DYNAMIC;
				BB_START(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_FAST_RET:
				flags |= ZEND_FUNC_TOO_DYNAMIC;
				if (i + 1 < op_array->last) {
					BB_START(i + 1);
				}
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
				BB_START(OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes);
				BB_START(i + 1);
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
				flags |= ZEND_FUNC_TOO_DYNAMIC;
				if (!opline->result.num) {
					BB_START(ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
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
			case ZEND_NEW:
				BB_START(OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes);
				BB_START(i + 1);
				break;
			case ZEND_DECLARE_LAMBDA_FUNCTION: {
//???					zend_op_array *lambda_op_array;
//???
//???					if (rt_constants) {
//???						zv = RT_CONSTANT(op_array, opline->op1);
//???					} else {
//???						zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
//???					}
//???					if (ctx->main_script &&
//???					    (lambda_op_array = zend_hash_find_ptr(&ctx->main_script->function_table, Z_STR_P(zv))) != NULL) {
//???						if (lambda_op_array->type == ZEND_USER_FUNCTION &&
//???						    lambda_op_array->static_variables) {
//???							// FIXME: Really we should try to perform alias
//???							// analysis on variables used by the closure
//???							info->flags |= ZEND_FUNC_TOO_DYNAMIC;
//???						}
//???					} else {
//???						// FIXME: how to find the lambda function?
						flags |= ZEND_FUNC_TOO_DYNAMIC;
//???					}
				}
				break;
			case ZEND_UNSET_VAR:
				if (!(opline->extended_value & ZEND_QUICK_SET)) {
					flags |= ZEND_FUNC_TOO_DYNAMIC;
				}
				break;
			case ZEND_FETCH_R:
			case ZEND_FETCH_W:
			case ZEND_FETCH_RW:
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_IS:
			case ZEND_FETCH_UNSET:
				if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL) {
					flags |= ZEND_FUNC_TOO_DYNAMIC;
				} else if (((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL ||
				            (opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_GLOBAL_LOCK) &&
				           !op_array->function_name) {
					flags |= ZEND_FUNC_TOO_DYNAMIC;
				}
				break;
		}
	}
	for (j = 0; j < op_array->last_live_range; j++) {
		BB_START(op_array->live_range[j].start);
		BB_START(op_array->live_range[j].end);
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

	cfg->blocks_count = blocks_count;

	/* Build CFG, Step 2: Build Array of Basic Blocks */
	cfg->blocks = blocks = zend_arena_calloc(arena, sizeof(zend_basic_block), blocks_count);
	if (!blocks) {
		return FAILURE;
	}

	for (i = 0, blocks_count = -1; i < op_array->last; i++) {
		if (block_map[i]) {
			if (blocks_count >= 0) {
				blocks[blocks_count].end = i - 1;
			}
			blocks_count++;
			blocks[blocks_count].flags = block_map[i];
			blocks[blocks_count].start = i;
			blocks[blocks_count].successors[0] = -1;
			blocks[blocks_count].successors[1] = -1;
			blocks[blocks_count].predecessors_count = 0;
			blocks[blocks_count].predecessor_offset = -1;
			blocks[blocks_count].idom = -1;
			blocks[blocks_count].loop_header = -1;
			blocks[blocks_count].level = -1;
			blocks[blocks_count].children = -1;
			blocks[blocks_count].next_child = -1;
			block_map[i] = blocks_count;
		} else {
			block_map[i] = (uint32_t)-1;
		}
	}

	blocks[blocks_count].end = i - 1;
	blocks_count++;

	/* Build CFG, Step 3: Calculate successors */
	for (j = 0; j < blocks_count; j++) {
		zend_op *opline = op_array->opcodes + blocks[j].end;
		switch(opline->opcode) {
			case ZEND_FAST_RET:
			case ZEND_RETURN:
			case ZEND_RETURN_BY_REF:
			case ZEND_GENERATOR_RETURN:
			case ZEND_EXIT:
			case ZEND_THROW:
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
				record_successor(blocks, j, 0, block_map[OP_JMP_ADDR(opline, opline->op1) - op_array->opcodes]);
				record_successor(blocks, j, 1, j + 1);
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
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				record_successor(blocks, j, 0, block_map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
				record_successor(blocks, j, 1, j + 1);
				break;
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
			case ZEND_NEW:
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
		*func_flags = flags;
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
		if (!(b->flags & ZEND_BB_REACHABLE)) {
			b->successors[0] = -1;
			b->successors[1] = -1;
			b->predecessors_count = 0;
		} else {
			if (b->successors[0] >= 0) {
				edges++;
				blocks[b->successors[0]].predecessors_count++;
				if (b->successors[0] >= 0) {
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
		if ((blocks[j].flags & ZEND_BB_REACHABLE)) {
			if (blocks[j].successors[0] >= 0) {
				zend_basic_block *b = blocks + blocks[j].successors[0];
				predecessors[b->predecessor_offset + b->predecessors_count] = j;
				b->predecessors_count++;
				if (blocks[j].successors[1] >= 0) {
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
