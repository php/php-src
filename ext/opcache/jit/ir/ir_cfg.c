/*
 * IR - Lightweight JIT Compilation Framework
 * (CFG - Control Flow Graph)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

IR_ALWAYS_INLINE void _ir_add_successors(const ir_ctx *ctx, ir_ref ref, ir_worklist *worklist)
{
	ir_use_list *use_list = &ctx->use_lists[ref];
	ir_ref *p, use, n = use_list->count;

	if (n < 2) {
		if (n == 1) {
			use = ctx->use_edges[use_list->refs];
			IR_ASSERT(ir_op_flags[ctx->ir_base[use].op] & IR_OP_FLAG_CONTROL);
			ir_worklist_push(worklist, use);
		}
	} else {
		p = &ctx->use_edges[use_list->refs];
		if (n == 2) {
			use = *p;
			IR_ASSERT(ir_op_flags[ctx->ir_base[use].op] & IR_OP_FLAG_CONTROL);
			ir_worklist_push(worklist, use);
			use = *(p + 1);
			IR_ASSERT(ir_op_flags[ctx->ir_base[use].op] & IR_OP_FLAG_CONTROL);
			ir_worklist_push(worklist, use);
		} else {
			for (; n > 0; p++, n--) {
				use = *p;
				IR_ASSERT(ir_op_flags[ctx->ir_base[use].op] & IR_OP_FLAG_CONTROL);
				ir_worklist_push(worklist, use);
			}
		}
	}
}

IR_ALWAYS_INLINE void _ir_add_predecessors(const ir_insn *insn, ir_worklist *worklist)
{
	ir_ref n, ref;
	const ir_ref *p;

	if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
		n = insn->inputs_count;
		for (p = insn->ops + 1; n > 0; p++, n--) {
			ref = *p;
			IR_ASSERT(ref);
			ir_worklist_push(worklist, ref);
		}
	} else if (insn->op != IR_START) {
		if (EXPECTED(insn->op1)) {
			ir_worklist_push(worklist, insn->op1);
		}
	}
}

int ir_build_cfg(ir_ctx *ctx)
{
	ir_ref n, *p, ref, start, end, next;
	uint32_t b;
	ir_insn *insn;
	ir_worklist worklist;
	uint32_t bb_init_falgs;
	uint32_t count, bb_count = 0;
	uint32_t edges_count = 0;
	ir_block *blocks, *bb;
	uint32_t *_blocks, *edges;
	ir_use_list *use_list;
	uint32_t len = ir_bitset_len(ctx->insns_count);
	ir_bitset bb_starts = ir_mem_calloc(len * 2, IR_BITSET_BITS / 8);
	ir_bitset bb_leaks = bb_starts + len;
	_blocks = ir_mem_calloc(ctx->insns_count, sizeof(uint32_t));
	ir_worklist_init(&worklist, ctx->insns_count);

	/* First try to perform backward DFS search starting from "stop" nodes */

	/* Add all "stop" nodes */
	ref = ctx->ir_base[1].op1;
	while (ref) {
		ir_worklist_push(&worklist, ref);
		ref = ctx->ir_base[ref].op3;
	}

	while (ir_worklist_len(&worklist)) {
		ref = ir_worklist_pop(&worklist);
		insn = &ctx->ir_base[ref];

		if (insn->op == IR_NOP) {
			continue;
		}
		IR_ASSERT(IR_IS_BB_END(insn->op));
		/* Remember BB end */
		end = ref;
		/* Some successors of IF and SWITCH nodes may be inaccessible by backward DFS */
		use_list = &ctx->use_lists[end];
		n = use_list->count;
		if (n > 1 || (n == 1 && (ir_op_flags[insn->op] & IR_OP_FLAG_TERMINATOR) != 0)) {
			for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
				/* Remember possible inaccessible successors */
				ir_bitset_incl(bb_leaks, *p);
			}
		}
		/* Skip control nodes untill BB start */
		ref = insn->op1;
		while (1) {
			insn = &ctx->ir_base[ref];
			if (IR_IS_BB_START(insn->op)) {
				break;
			}
			ref = insn->op1; // follow connected control blocks untill BB start
		}
		/* Mark BB Start */
		bb_count++;
		_blocks[ref] = end;
		ir_bitset_incl(bb_starts, ref);
		/* Add predecessors */
		_ir_add_predecessors(insn, &worklist);
	}

	/* Backward DFS way miss some branches ending by infinite loops.  */
	/* Try forward DFS. (in most cases all nodes are already proceed). */

	/* START node may be inaccessible from "stop" nodes */
	ir_bitset_incl(bb_leaks, 1);

	/* Add not processed START and successor of IF and SWITCH */
	IR_BITSET_FOREACH_DIFFERENCE(bb_leaks, bb_starts, len, start) {
		ir_worklist_push(&worklist, start);
	} IR_BITSET_FOREACH_END();

	if (ir_worklist_len(&worklist)) {
		ir_bitset_union(worklist.visited, bb_starts, len);
		do {
			ref = ir_worklist_pop(&worklist);
			insn = &ctx->ir_base[ref];

			if (insn->op == IR_NOP) {
				continue;
			}
			IR_ASSERT(IR_IS_BB_START(insn->op));
			/* Remember BB start */
			start = ref;
			/* Skip control nodes untill BB end */
			while (1) {
				use_list = &ctx->use_lists[ref];
				n = use_list->count;
				next = IR_UNUSED;
				for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
					next = *p;
					insn = &ctx->ir_base[next];
					if ((ir_op_flags[insn->op] & IR_OP_FLAG_CONTROL) && insn->op1 == ref) {
						break;
					}
				}
				IR_ASSERT(next != IR_UNUSED);
				ref = next;
				if (IR_IS_BB_END(insn->op)) {
					break;
				}
			}
			/* Mark BB Start */
			bb_count++;
			_blocks[start] = ref;
			ir_bitset_incl(bb_starts, start);
			/* Add successors */
			_ir_add_successors(ctx, ref, &worklist);
		} while (ir_worklist_len(&worklist));
	}

	IR_ASSERT(bb_count > 0);

	/* Create array of basic blocks and count successor/predecessors edges for each BB */
	blocks = ir_mem_malloc((bb_count + 1) * sizeof(ir_block));
	b = 1;
	bb = blocks + 1;
	count = 0;
	/* SCCP already removed UNREACHABKE blocks, otherwise all blocks are marked as UNREACHABLE first */
	bb_init_falgs = (ctx->flags2 & IR_CFG_REACHABLE) ? 0 : IR_BB_UNREACHABLE;
	IR_BITSET_FOREACH(bb_starts, len, start) {
		insn = &ctx->ir_base[start];
		if (insn->op == IR_NOP) {
			_blocks[start] = 0;
			continue;
		}
		end = _blocks[start];
		_blocks[start] = b;
		_blocks[end] = b;
		IR_ASSERT(IR_IS_BB_START(insn->op));
		IR_ASSERT(end > start);
		bb->start = start;
		bb->end = end;
		bb->successors = count;
		count += ctx->use_lists[end].count;
		bb->successors_count = 0;
		bb->predecessors = count;
		bb->dom_parent = 0;
		bb->dom_depth = 0;
		bb->dom_child = 0;
		bb->dom_next_child = 0;
		bb->loop_header = 0;
		bb->loop_depth = 0;
		if (insn->op == IR_START) {
			bb->flags = IR_BB_START;
			bb->predecessors_count = 0;
		} else {
			bb->flags = bb_init_falgs;
			if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				n = insn->inputs_count;
				bb->predecessors_count = n;
				edges_count += n;
				count += n;
			} else if (EXPECTED(insn->op1)) {
				if (insn->op == IR_ENTRY) {
					bb->flags |= IR_BB_ENTRY;
					ctx->entries_count++;
				}
				bb->predecessors_count = 1;
				edges_count++;
				count++;
			} else {
				IR_ASSERT(insn->op == IR_BEGIN); /* start of unreachable block */
				bb->predecessors_count = 0;
			}
		}
		b++;
		bb++;
	} IR_BITSET_FOREACH_END();
	bb_count = b - 1;
	IR_ASSERT(count == edges_count * 2);
	ir_mem_free(bb_starts);

	/* Create an array of successor/predecessors control edges */
	edges = ir_mem_malloc(edges_count * 2 * sizeof(uint32_t));
	bb = blocks + 1;
	for (b = 1; b <= bb_count; b++, bb++) {
		insn = &ctx->ir_base[bb->start];
		if (bb->predecessors_count > 1) {
			uint32_t *q = edges + bb->predecessors;
			n = insn->inputs_count;
			for (p = insn->ops + 1; n > 0; p++, q++, n--) {
				ref = *p;
				IR_ASSERT(ref);
				ir_ref pred_b = _blocks[ref];
				ir_block *pred_bb = &blocks[pred_b];
				IR_ASSERT(pred_b > 0);
				*q = pred_b;
				edges[pred_bb->successors + pred_bb->successors_count++] = b;
			}
		} else if (bb->predecessors_count == 1) {
			ref = insn->op1;
			IR_ASSERT(ref);
			IR_ASSERT(IR_OPND_KIND(ir_op_flags[insn->op], 1) == IR_OPND_CONTROL);
			ir_ref pred_b = _blocks[ref];
			ir_block *pred_bb = &blocks[pred_b];
			edges[bb->predecessors] = pred_b;
			edges[pred_bb->successors + pred_bb->successors_count++] = b;
		}
	}

	ctx->cfg_blocks_count = bb_count;
	ctx->cfg_edges_count = edges_count * 2;
	ctx->cfg_blocks = blocks;
	ctx->cfg_edges = edges;
	ctx->cfg_map = _blocks;

	if (!(ctx->flags2 & IR_CFG_REACHABLE)) {
		uint32_t reachable_count = 0;

		/* Mark reachable blocks */
		ir_worklist_clear(&worklist);
		ir_worklist_push(&worklist, 1);
		while (ir_worklist_len(&worklist) != 0) {
			uint32_t *p;

			reachable_count++;
			b = ir_worklist_pop(&worklist);
			bb = &blocks[b];
			bb->flags &= ~IR_BB_UNREACHABLE;
			n = bb->successors_count;
			if (n > 1) {
				for (p = edges + bb->successors; n > 0; p++, n--) {
					ir_worklist_push(&worklist, *p);
				}
			} else if (n == 1) {
				ir_worklist_push(&worklist, edges[bb->successors]);
			}
		}
		if (reachable_count != ctx->cfg_blocks_count) {
			ir_remove_unreachable_blocks(ctx);
		}
	}

	ir_worklist_free(&worklist);

	return 1;
}

static void ir_remove_predecessor(ir_ctx *ctx, ir_block *bb, uint32_t from)
{
	uint32_t i, *p, *q, n = 0;

	p = q = &ctx->cfg_edges[bb->predecessors];
	for (i = 0; i < bb->predecessors_count; i++, p++) {
		if (*p != from) {
			if (p != q) {
				*q = *p;
			}
			q++;
			n++;
		}
	}
	IR_ASSERT(n != bb->predecessors_count);
	bb->predecessors_count = n;
}

static void ir_remove_merge_input(ir_ctx *ctx, ir_ref merge, ir_ref from)
{
	ir_ref i, j, n, k, *p, use;
	ir_insn *use_insn;
	ir_use_list *use_list;
	ir_bitset life_inputs;
	ir_insn *insn = &ctx->ir_base[merge];

	IR_ASSERT(insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN);
	n = insn->inputs_count;
	i = 1;
	life_inputs = ir_bitset_malloc(n + 1);
	for (j = 1; j <= n; j++) {
		ir_ref input = ir_insn_op(insn, j);

		if (input != from) {
			if (i != j) {
				ir_insn_set_op(insn, i, input);
			}
			ir_bitset_incl(life_inputs, j);
			i++;
		}
	}
	i--;
	if (i == 1) {
		insn->op = IR_BEGIN;
		insn->inputs_count = 1;
		use_list = &ctx->use_lists[merge];
		if (use_list->count > 1) {
			for (k = 0, p = &ctx->use_edges[use_list->refs]; k < use_list->count; k++, p++) {
				use = *p;
				use_insn = &ctx->ir_base[use];
				if (use_insn->op == IR_PHI) {
					/* Convert PHI to COPY */
					i = 2;
					for (j = 2; j <= n; j++) {
						ir_ref input = ir_insn_op(use_insn, j);

						if (ir_bitset_in(life_inputs, j - 1)) {
							use_insn->op1 = ir_insn_op(use_insn, j);
						} else if (input > 0) {
							ir_use_list_remove_all(ctx, input, use);
						}
					}
					use_insn->op = IR_COPY;
					use_insn->op2 = IR_UNUSED;
					use_insn->op3 = IR_UNUSED;
					ir_use_list_remove_all(ctx, merge, use);
				}
			}
		}
	} else {
		insn->inputs_count = i;

		n++;
		use_list = &ctx->use_lists[merge];
		if (use_list->count > 1) {
			for (k = 0, p = &ctx->use_edges[use_list->refs]; k < use_list->count; k++, p++) {
				use = *p;
				use_insn = &ctx->ir_base[use];
				if (use_insn->op == IR_PHI) {
					i = 2;
					for (j = 2; j <= n; j++) {
						ir_ref input = ir_insn_op(use_insn, j);

						if (ir_bitset_in(life_inputs, j - 1)) {
							IR_ASSERT(input);
							if (i != j) {
								ir_insn_set_op(use_insn, i, input);
							}
							i++;
						} else if (input > 0) {
							ir_use_list_remove_all(ctx, input, use);
						}
					}
				}
			}
		}
	}
	ir_mem_free(life_inputs);
	ir_use_list_remove_all(ctx, from, merge);
}

/* CFG constructed after SCCP pass doesn't have unreachable BBs, otherwise they should be removed */
int ir_remove_unreachable_blocks(ir_ctx *ctx)
{
	uint32_t b, *p, i;
	uint32_t unreachable_count = 0;
	uint32_t bb_count = ctx->cfg_blocks_count;
	ir_block *bb = ctx->cfg_blocks + 1;

	for (b = 1; b <= bb_count; b++, bb++) {
		if (bb->flags & IR_BB_UNREACHABLE) {
#if 0
			do {if (!unreachable_count) ir_dump_cfg(ctx, stderr);} while(0);
#endif
			if (bb->successors_count) {
				for (i = 0, p = &ctx->cfg_edges[bb->successors]; i < bb->successors_count; i++, p++) {
					ir_block *succ_bb = &ctx->cfg_blocks[*p];

					if (!(succ_bb->flags & IR_BB_UNREACHABLE)) {
						ir_remove_predecessor(ctx, succ_bb, b);
						ir_remove_merge_input(ctx, succ_bb->start, bb->end);
					}
				}
			} else {
				ir_ref prev, ref = bb->end;
				ir_insn *insn = &ctx->ir_base[ref];

				IR_ASSERT(ir_op_flags[insn->op] & IR_OP_FLAG_TERMINATOR);
				/* remove from terminators list */
				prev = ctx->ir_base[1].op1;
				if (prev == ref) {
					ctx->ir_base[1].op1 = insn->op3;
				} else {
					while (prev) {
						if (ctx->ir_base[prev].op3 == ref) {
							ctx->ir_base[prev].op3 = insn->op3;
							break;
						}
						prev = ctx->ir_base[prev].op3;
					}
				}
			}
			ctx->cfg_map[bb->start] = 0;
			ctx->cfg_map[bb->end] = 0;
			unreachable_count++;
		}
	}

	if (unreachable_count) {
		ir_block *dst_bb;
		uint32_t n = 1;
		uint32_t *edges;

		dst_bb = bb = ctx->cfg_blocks + 1;
		for (b = 1; b <= bb_count; b++, bb++) {
			if (!(bb->flags & IR_BB_UNREACHABLE)) {
				if (dst_bb != bb) {
					memcpy(dst_bb, bb, sizeof(ir_block));
					ctx->cfg_map[dst_bb->start] = n;
					ctx->cfg_map[dst_bb->end] = n;
				}
				dst_bb->successors_count = 0;
				dst_bb++;
				n++;
			}
		}
		ctx->cfg_blocks_count = bb_count = n - 1;

		/* Rebuild successor/predecessors control edges */
		edges = ctx->cfg_edges;
		bb = ctx->cfg_blocks + 1;
		for (b = 1; b <= bb_count; b++, bb++) {
			ir_insn *insn = &ctx->ir_base[bb->start];
			ir_ref *p, ref;

			n = bb->predecessors_count;
			if (n > 1) {
				uint32_t *q = edges + bb->predecessors;

				IR_ASSERT(n == insn->inputs_count);
				for (p = insn->ops + 1; n > 0; p++, q++, n--) {
					ref = *p;
					IR_ASSERT(ref);
					ir_ref pred_b = ctx->cfg_map[ref];
					ir_block *pred_bb = &ctx->cfg_blocks[pred_b];
					*q = pred_b;
					edges[pred_bb->successors + pred_bb->successors_count++] = b;
				}
			} else if (n == 1) {
				ref = insn->op1;
				IR_ASSERT(ref);
				IR_ASSERT(IR_OPND_KIND(ir_op_flags[insn->op], 1) == IR_OPND_CONTROL);
				ir_ref pred_b = ctx->cfg_map[ref];
				ir_block *pred_bb = &ctx->cfg_blocks[pred_b];
				edges[bb->predecessors] = pred_b;
				edges[pred_bb->successors + pred_bb->successors_count++] = b;
			}
		}
	}

	return 1;
}

#if 0
static void compute_postnum(const ir_ctx *ctx, uint32_t *cur, uint32_t b)
{
	uint32_t i, *p;
	ir_block *bb = &ctx->cfg_blocks[b];

	if (bb->postnum != 0) {
		return;
	}

	if (bb->successors_count) {
		bb->postnum = -1; /* Marker for "currently visiting" */
		p = ctx->cfg_edges + bb->successors;
		i = bb->successors_count;
		do {
			compute_postnum(ctx, cur, *p);
			p++;
		} while (--i);
	}
	bb->postnum = (*cur)++;
}

/* Computes dominator tree using algorithm from "A Simple, Fast Dominance Algorithm" by
 * Cooper, Harvey and Kennedy. */
int ir_build_dominators_tree(ir_ctx *ctx)
{
	uint32_t blocks_count, b, postnum;
	ir_block *blocks, *bb;
	uint32_t *edges;
	bool changed;

	ctx->flags2 &= ~IR_NO_LOOPS;

	postnum = 1;
	compute_postnum(ctx, &postnum, 1);

	/* Find immediate dominators */
	blocks = ctx->cfg_blocks;
	edges  = ctx->cfg_edges;
	blocks_count = ctx->cfg_blocks_count;
	blocks[1].idom = 1;
	do {
		changed = 0;
		/* Iterating in Reverse Post Order */
		for (b = 2, bb = &blocks[2]; b <= blocks_count; b++, bb++) {
			IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
			if (bb->predecessors_count == 1) {
				uint32_t pred_b = edges[bb->predecessors];

				if (blocks[pred_b].idom <= 0) {
					//IR_ASSERT("Wrong blocks order: BB is before its single predecessor");
				} else if (bb->idom != pred_b) {
					bb->idom = pred_b;
					changed = 1;
				}
			} else if (bb->predecessors_count) {
				uint32_t idom = 0;
				uint32_t k = bb->predecessors_count;
				uint32_t *p = edges + bb->predecessors;

				do {
					uint32_t pred_b = *p;
					ir_block *pred_bb = &blocks[pred_b];
					ir_block *idom_bb;

					if (pred_bb->idom > 0) {
						idom = pred_b;
						idom_bb = &blocks[idom];

						while (--k > 0) {
							pred_b = *(++p);
							pred_bb = &blocks[pred_b];
							if (pred_bb->idom > 0) {
								while (idom != pred_b) {
									while (pred_bb->postnum < idom_bb->postnum) {
										pred_b = pred_bb->idom;
										pred_bb = &blocks[pred_b];
									}
									while (idom_bb->postnum < pred_bb->postnum) {
										idom = idom_bb->idom;
										idom_bb = &blocks[idom];
									}
								}
							}
						}

						if (bb->idom != idom) {
							bb->idom = idom;
							changed = 1;
						}
						break;
					}
					p++;
				} while (--k > 0);
			}
		}
	} while (changed);
	blocks[1].idom = 0;
	blocks[1].dom_depth = 0;

	/* Construct dominators tree */
	for (b = 2, bb = &blocks[2]; b <= blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		if (bb->idom > 0) {
			ir_block *idom_bb = &blocks[bb->idom];

			bb->dom_depth = idom_bb->dom_depth + 1;
			/* Sort by block number to traverse children in pre-order */
			if (idom_bb->dom_child == 0) {
				idom_bb->dom_child = b;
			} else if (b < idom_bb->dom_child) {
				bb->dom_next_child = idom_bb->dom_child;
				idom_bb->dom_child = b;
			} else {
				int child = idom_bb->dom_child;
				ir_block *child_bb = &blocks[child];

				while (child_bb->dom_next_child > 0 && b > child_bb->dom_next_child) {
					child = child_bb->dom_next_child;
					child_bb = &blocks[child];
				}
				bb->dom_next_child = child_bb->dom_next_child;
				child_bb->dom_next_child = b;
			}
		}
	}

	return 1;
}
#else
/* A single pass modification of "A Simple, Fast Dominance Algorithm" by
 * Cooper, Harvey and Kennedy, that relays on IR block ordering.
 * It may fallback to the general slow fixed-point algorithm.  */
static int ir_build_dominators_tree_iterative(ir_ctx *ctx);
int ir_build_dominators_tree(ir_ctx *ctx)
{
	uint32_t blocks_count, b;
	ir_block *blocks, *bb;
	uint32_t *edges;
	ir_list worklist;

	ir_list_init(&worklist, ctx->cfg_blocks_count / 2);

	ctx->flags2 |= IR_NO_LOOPS;

	/* Find immediate dominators */
	blocks = ctx->cfg_blocks;
	edges  = ctx->cfg_edges;
	blocks_count = ctx->cfg_blocks_count;
	blocks[1].idom = 1;
	blocks[1].dom_depth = 0;

	/* Iterating in Reverse Post Order */
	for (b = 2, bb = &blocks[2]; b <= blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		IR_ASSERT(bb->predecessors_count > 0);
		uint32_t k = bb->predecessors_count;
		uint32_t *p = edges + bb->predecessors;
		uint32_t idom = *p;
		ir_block *idom_bb;

		if (UNEXPECTED(idom >= b)) {
			/* In rare cases, LOOP_BEGIN.op1 may be a back-edge. Skip back-edges. */
			ctx->flags2 &= ~IR_NO_LOOPS;
			IR_ASSERT(k > 1 && "Wrong blocks order: BB is before its single predecessor");
			ir_list_push(&worklist, idom);
			while (1) {
				k--;
				p++;
				idom = *p;
				if (idom < b) {
					break;
				}
				IR_ASSERT(k > 0);
				ir_list_push(&worklist, idom);
			}
		}
		IR_ASSERT(blocks[idom].idom > 0);
		IR_ASSERT(k != 0);

		while (--k > 0) {
			uint32_t pred_b = *(++p);

			if (pred_b < b) {
				IR_ASSERT(blocks[pred_b].idom > 0);
				while (idom != pred_b) {
					while (pred_b > idom) {
						pred_b = blocks[pred_b].idom;
					}
					while (idom > pred_b) {
						idom = blocks[idom].idom;
					}
				}
			} else {
				ctx->flags2 &= ~IR_NO_LOOPS;
				IR_ASSERT(bb->predecessors_count > 1);
				ir_list_push(&worklist, pred_b);
			}
		}
		bb->idom = idom;
		idom_bb = &blocks[idom];

		bb->dom_depth = idom_bb->dom_depth + 1;
		/* Sort by block number to traverse children in pre-order */
		if (idom_bb->dom_child == 0) {
			idom_bb->dom_child = b;
		} else if (b < idom_bb->dom_child) {
			bb->dom_next_child = idom_bb->dom_child;
			idom_bb->dom_child = b;
		} else {
			int child = idom_bb->dom_child;
			ir_block *child_bb = &blocks[child];

			while (child_bb->dom_next_child > 0 && b > child_bb->dom_next_child) {
				child = child_bb->dom_next_child;
				child_bb = &blocks[child];
			}
			bb->dom_next_child = child_bb->dom_next_child;
			child_bb->dom_next_child = b;
		}
	}

	blocks[1].idom = 0;

	if (ir_list_len(&worklist) != 0) {
		uint32_t dom_depth;
		uint32_t succ_b;
		bool complete = 1;

		/* Check if all the back-edges lead to the loop headers */
		do {
			b = ir_list_pop(&worklist);
			bb = &blocks[b];
			succ_b = ctx->cfg_edges[bb->successors];
			if (bb->successors_count != 1) {
				/* LOOP_END/END may be linked with the following ENTRY by a fake edge */
				IR_ASSERT(bb->successors_count == 2);
				if (blocks[succ_b].flags & IR_BB_ENTRY) {
					succ_b = ctx->cfg_edges[bb->successors + 1];
				} else {
					IR_ASSERT(blocks[ctx->cfg_edges[bb->successors + 1]].flags & IR_BB_ENTRY);
				}
			}
			dom_depth = blocks[succ_b].dom_depth;;
			while (bb->dom_depth > dom_depth) {
				b = bb->dom_parent;
				bb = &blocks[b];
			}
			if (UNEXPECTED(b != succ_b)) {
				complete = 0;
				break;
			}
		} while (ir_list_len(&worklist) != 0);

		if (UNEXPECTED(!complete)) {
			ir_list_free(&worklist);
			return ir_build_dominators_tree_iterative(ctx);
		}
	}

	ir_list_free(&worklist);

	return 1;
}

static int ir_build_dominators_tree_iterative(ir_ctx *ctx)
{
	bool changed;
	uint32_t blocks_count, b;
	ir_block *blocks, *bb;
	uint32_t *edges;

	/* Find immediate dominators */
	blocks = ctx->cfg_blocks;
	edges  = ctx->cfg_edges;
	blocks_count = ctx->cfg_blocks_count;

	/* Clear the dominators tree, but keep already found dominators */
	for (b = 0, bb = &blocks[0]; b <= blocks_count; b++, bb++) {
		bb->dom_depth = 0;
		bb->dom_child = 0;
		bb->dom_next_child = 0;
	}

	/* Find immediate dominators by iterative fixed-point algorithm */
	blocks[1].idom = 1;
	do {
		changed = 0;

		for (b = 2, bb = &blocks[2]; b <= blocks_count; b++, bb++) {
			IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
			IR_ASSERT(bb->predecessors_count > 0);
			uint32_t k = bb->predecessors_count;
			uint32_t *p = edges + bb->predecessors;
			uint32_t idom = *p;

			if (blocks[idom].idom == 0) {
				while (1) {
					k--;
					p++;
					idom = *p;
					if (blocks[idom].idom > 0) {
						break;
					}
					IR_ASSERT(k > 0);
				}
			}
			IR_ASSERT(k != 0);
			while (--k > 0) {
				uint32_t pred_b = *(++p);

				if (blocks[pred_b].idom > 0) {
					IR_ASSERT(blocks[pred_b].idom > 0);
					while (idom != pred_b) {
						while (pred_b > idom) {
							pred_b = blocks[pred_b].idom;
						}
						while (idom > pred_b) {
							idom = blocks[idom].idom;
						}
					}
				}
			}
			if (bb->idom != idom) {
				bb->idom = idom;
				changed = 1;
			}
		}
	} while (changed);

	/* Build dominators tree */
	blocks[1].idom = 0;
	blocks[1].dom_depth = 0;
	for (b = 2, bb = &blocks[2]; b <= blocks_count; b++, bb++) {
		uint32_t idom = bb->idom;
		ir_block *idom_bb = &blocks[idom];

		bb->dom_depth = idom_bb->dom_depth + 1;
		/* Sort by block number to traverse children in pre-order */
		if (idom_bb->dom_child == 0) {
			idom_bb->dom_child = b;
		} else if (b < idom_bb->dom_child) {
			bb->dom_next_child = idom_bb->dom_child;
			idom_bb->dom_child = b;
		} else {
			int child = idom_bb->dom_child;
			ir_block *child_bb = &blocks[child];

			while (child_bb->dom_next_child > 0 && b > child_bb->dom_next_child) {
				child = child_bb->dom_next_child;
				child_bb = &blocks[child];
			}
			bb->dom_next_child = child_bb->dom_next_child;
			child_bb->dom_next_child = b;
		}
	}

	return 1;
}
#endif

static bool ir_dominates(const ir_block *blocks, uint32_t b1, uint32_t b2)
{
	uint32_t b1_depth = blocks[b1].dom_depth;
	const ir_block *bb2 = &blocks[b2];

	while (bb2->dom_depth > b1_depth) {
		b2 = bb2->dom_parent;
		bb2 = &blocks[b2];
	}
	return b1 == b2;
}

int ir_find_loops(ir_ctx *ctx)
{
	uint32_t i, j, n, count;
	uint32_t *entry_times, *exit_times, *sorted_blocks, time = 1;
	ir_block *blocks = ctx->cfg_blocks;
	uint32_t *edges = ctx->cfg_edges;
	ir_worklist work;

	if (ctx->flags2 & IR_NO_LOOPS) {
		return 1;
	}

	/* We don't materialize the DJ spanning tree explicitly, as we are only interested in ancestor
	 * queries. These are implemented by checking entry/exit times of the DFS search. */
	ir_worklist_init(&work, ctx->cfg_blocks_count + 1);
	entry_times = ir_mem_malloc((ctx->cfg_blocks_count + 1) * 3 * sizeof(uint32_t));
	exit_times = entry_times + ctx->cfg_blocks_count + 1;
	sorted_blocks = exit_times + ctx->cfg_blocks_count + 1;

	memset(entry_times, 0, (ctx->cfg_blocks_count + 1) * sizeof(uint32_t));

	ir_worklist_push(&work, 1);
	while (ir_worklist_len(&work)) {
		ir_block *bb;
		int child;

next:
		i = ir_worklist_peek(&work);
		if (!entry_times[i]) {
			entry_times[i] = time++;
		}

		/* Visit blocks immediately dominated by i. */
		bb = &blocks[i];
		for (child = bb->dom_child; child > 0; child = blocks[child].dom_next_child) {
			if (ir_worklist_push(&work, child)) {
				goto next;
			}
		}

		/* Visit join edges. */
		if (bb->successors_count) {
			uint32_t *p = edges + bb->successors;
			for (j = 0; j < bb->successors_count; j++,p++) {
				uint32_t succ = *p;

				if (blocks[succ].idom == i) {
					continue;
				} else if (ir_worklist_push(&work, succ)) {
					goto next;
				}
			}
		}
		exit_times[i] = time++;
		ir_worklist_pop(&work);
	}

	/* Sort blocks by level, which is the opposite order in which we want to process them */
	sorted_blocks[1] = 1;
	j = 1;
	n = 2;
	while (j != n) {
		i = j;
		j = n;
		for (; i < j; i++) {
			int child;
			for (child = blocks[sorted_blocks[i]].dom_child; child > 0; child = blocks[child].dom_next_child) {
				sorted_blocks[n++] = child;
			}
		}
	}
	count = n;

	/* Identify loops. See Sreedhar et al, "Identifying Loops Using DJ Graphs". */
	while (n > 1) {
		i = sorted_blocks[--n];
		ir_block *bb = &blocks[i];

		if (bb->predecessors_count > 1) {
			bool irreducible = 0;
			uint32_t *p = &edges[bb->predecessors];

			j = bb->predecessors_count;
			do {
				uint32_t pred = *p;

				/* A join edge is one for which the predecessor does not
				   immediately dominate the successor.  */
				if (bb->idom != pred) {
					/* In a loop back-edge (back-join edge), the successor dominates
					   the predecessor.  */
					if (ir_dominates(blocks, i, pred)) {
						if (!ir_worklist_len(&work)) {
							ir_bitset_clear(work.visited, ir_bitset_len(ir_worklist_capasity(&work)));
						}
						blocks[pred].loop_header = 0; /* support for merged loops */
						ir_worklist_push(&work, pred);
					} else {
						/* Otherwise it's a cross-join edge.  See if it's a branch
						   to an ancestor on the DJ spanning tree.  */
						if (entry_times[pred] > entry_times[i] && exit_times[pred] < exit_times[i]) {
							irreducible = 1;
						}
					}
				}
				p++;
			} while (--j);

			if (UNEXPECTED(irreducible)) {
				// TODO: Support for irreducible loops ???
				bb->flags |= IR_BB_IRREDUCIBLE_LOOP;
				ctx->flags2 |= IR_IRREDUCIBLE_CFG;
				while (ir_worklist_len(&work)) {
					ir_worklist_pop(&work);
				}
			} else if (ir_worklist_len(&work)) {
				bb->flags |= IR_BB_LOOP_HEADER;
				ctx->flags2 |= IR_CFG_HAS_LOOPS;
				bb->loop_depth = 1;
				while (ir_worklist_len(&work)) {
					j = ir_worklist_pop(&work);
					while (blocks[j].loop_header > 0) {
						j = blocks[j].loop_header;
					}
					if (j != i) {
						ir_block *bb = &blocks[j];
						if (bb->idom == 0 && j != 1) {
							/* Ignore blocks that are unreachable or only abnormally reachable. */
							continue;
						}
						bb->loop_header = i;
						if (bb->predecessors_count) {
							uint32_t *p = &edges[bb->predecessors];
							j = bb->predecessors_count;
							do {
								ir_worklist_push(&work, *p);
								p++;
							} while (--j);
						}
					}
				}
			}
		}
	}

	if (ctx->flags2 & IR_CFG_HAS_LOOPS) {
		for (n = 1; n < count; n++) {
			i = sorted_blocks[n];
			ir_block *bb = &blocks[i];
			if (bb->loop_header > 0) {
				ir_block *loop = &blocks[bb->loop_header];
				uint32_t loop_depth = loop->loop_depth;

				if (bb->flags & IR_BB_LOOP_HEADER) {
					loop_depth++;
				}
				bb->loop_depth = loop_depth;
				if (bb->flags & (IR_BB_ENTRY|IR_BB_LOOP_WITH_ENTRY)) {
					loop->flags |= IR_BB_LOOP_WITH_ENTRY;
					if (loop_depth > 1) {
						/* Set IR_BB_LOOP_WITH_ENTRY flag for all the enclosing loops */
						bb = &blocks[loop->loop_header];
						while (1) {
							if (bb->flags & IR_BB_LOOP_WITH_ENTRY) {
								break;
							}
							bb->flags |= IR_BB_LOOP_WITH_ENTRY;
							if (bb->loop_depth == 1) {
								break;
							}
							bb = &blocks[loop->loop_header];
						}
					}
				}
			}
		}
	}

	ir_mem_free(entry_times);
	ir_worklist_free(&work);

	return 1;
}

static uint32_t _ir_skip_empty_blocks(const ir_ctx *ctx, uint32_t b)
{
	while (1) {
		ir_block *bb = &ctx->cfg_blocks[b];

		if ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY) {
			IR_ASSERT(bb->successors_count == 1);
			b = ctx->cfg_edges[bb->successors];
		} else {
			return b;
		}
	}
}

/* A variation of "Bottom-up Positioning" algorithm described by
 * Karl Pettis and Robert C. Hansen "Profile Guided Code Positioning"
 */
typedef struct _ir_edge_info {
	uint32_t from;
	uint32_t to;
	float    freq;
} ir_edge_info;

typedef struct _ir_chain {
	uint32_t head;
	uint32_t next;
	union {
		uint32_t prev;
		uint32_t tail;
	};
} ir_chain;

static int ir_edge_info_cmp(const void *b1, const void *b2)
{
	ir_edge_info *e1 = (ir_edge_info*)b1;
	ir_edge_info *e2 = (ir_edge_info*)b2;

	if (e1->freq != e2->freq) {
		return e1->freq < e2->freq ? 1 : -1;
	}
	/* In case of equal frequencies, try to avoid penalization of one of the "equal" paths by
	 * preferring the first RPO successor (in conditional branches) and the last RPO predecessor
	 * (in merge points).
	 *
	 * See "Static Basic Block Reordering Heuristics for Implicit Control Flow in Baseline JITs"
	 * Polito Guillermo, Ducasse Stephane, and Tesone Pablo (2021)
	 */
	if (e1->from != e2->from) {
		return e2->from - e1->from;
	} else {
		return e1->to - e2->to;
	}
}

static IR_NEVER_INLINE uint32_t ir_chain_head_path_compress(ir_chain *chains, uint32_t src, uint32_t head)
{
	IR_ASSERT(head != 0);
	do {
		head = chains[head].head;
	} while (chains[head].head != head);
	do {
		uint32_t next = chains[src].head;
		chains[src].head = head;
		src = next;
	} while (chains[src].head != head);
	return head;
}

IR_ALWAYS_INLINE uint32_t ir_chain_head(ir_chain *chains, uint32_t src)
{
	uint32_t head = chains[src].head;
	if (chains[head].head == head) {
		return head;
	} else {
		return ir_chain_head_path_compress(chains, src, head);
	}
}

static void ir_join_chains(ir_chain *chains, uint32_t src, uint32_t dst)
{
	uint32_t dst_tail = chains[dst].tail;
	uint32_t src_tail = chains[src].tail;

	chains[dst_tail].next = src;
	chains[dst].prev = src_tail;
	chains[src_tail].next = dst;
	chains[src].tail = dst_tail;
	chains[dst].head = src;
}

static void ir_insert_chain_before(ir_chain *chains, uint32_t c, uint32_t before)
{
	ir_chain *this = &chains[c];
	ir_chain *next = &chains[before];

	IR_ASSERT(chains[c].head == c);
	if (chains[before].head != before) {
		this->head = next->head;
	} else {
		next->head = c;
	}
	this->next = before;
	this->prev = next->prev;
	next->prev = c;
	chains[this->prev].next = c;
}

#ifndef IR_DEBUG_BB_SCHEDULE_GRAPH
# ifdef IR_DEBUG
#  define IR_DEBUG_BB_SCHEDULE_GRAPH 1
# else
#  define IR_DEBUG_BB_SCHEDULE_GRAPH 0
# endif
#endif

#if IR_DEBUG_BB_SCHEDULE_GRAPH
static void ir_dump_cfg_freq_graph(ir_ctx *ctx, float *bb_freq, uint32_t edges_count, ir_edge_info *edges, ir_chain *chains)
{
	uint32_t b, i;
	ir_block *bb;
	uint8_t c, *colors;
	bool is_head, is_empty;
	uint32_t max_colors = 12;

	colors = alloca(sizeof(uint8_t) * (ctx->cfg_blocks_count + 1));
	memset(colors, 0, sizeof(uint8_t) * (ctx->cfg_blocks_count + 1));
	i = 0;
	for (b = 1; b < ctx->cfg_blocks_count + 1; b++) {
		if (chains[b].head == b) {
			colors[b] = (i % max_colors) + 1;
			i++;
		}
	}

	fprintf(stderr, "digraph {\n");
	fprintf(stderr, "\trankdir=TB;\n");
	for (b = 1; b <= ctx->cfg_blocks_count; b++) {
		bb = &ctx->cfg_blocks[b];
		c = (chains[b].head) ? colors[ir_chain_head(chains, b)] : 0;
		is_head = chains[b].head == b;
		is_empty = (bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY;
		if (c) {
			fprintf(stderr, "\tBB%d [label=\"BB%d: (%d),%0.3f\\n%s\\n%s\",colorscheme=set312,fillcolor=%d%s%s]\n", b, b,
				bb->loop_depth, bb_freq[b],
				ir_op_name[ctx->ir_base[bb->start].op], ir_op_name[ctx->ir_base[bb->end].op],
				c,
				is_head ? ",penwidth=3" : "",
				is_empty ? ",style=\"dotted,filled\"" : ",style=\"filled\"");
		} else {
			fprintf(stderr, "\tBB%d [label=\"BB%d: (%d),%0.3f\\n%s\\n%s\"%s%s]\n", b, b,
				bb->loop_depth, bb_freq[b],
				ir_op_name[ctx->ir_base[bb->start].op], ir_op_name[ctx->ir_base[bb->end].op],
				is_head ? ",penwidth=3" : "",
				is_empty ? ",style=\"dotted\"" : "");
		}
	}
	fprintf(stderr, "\n");

	for (i = 0; i < edges_count; i++) {
		fprintf(stderr, "\tBB%d -> BB%d [label=\"%0.3f\"]\n", edges[i].from, edges[i].to, edges[i].freq);
	}
	fprintf(stderr, "}\n");
}
#endif

#ifdef IR_DEBUG
static void ir_dump_edges(ir_ctx *ctx, uint32_t edges_count, ir_edge_info *edges)
{
	uint32_t i;

	fprintf(stderr, "Edges:\n");
	for (i = 0; i < edges_count; i++) {
		fprintf(stderr, "\tBB%d -> BB%d %0.3f\n", edges[i].from, edges[i].to, edges[i].freq);
	}
}

static void ir_dump_chains(ir_ctx *ctx, ir_chain *chains)
{
	uint32_t b, tail, i;

	fprintf(stderr, "Chains:\n");
	for (b = 1; b < ctx->cfg_blocks_count + 1; b++) {
		if (chains[b].head == b) {
			tail = chains[b].tail;
			i = b;
			fprintf(stderr, "(BB%d", i);
			while (i != tail) {
				i = chains[i].next;
				fprintf(stderr, ", BB%d", i);
			}
			fprintf(stderr, ")\n");
		}
	}
}
#endif

static int ir_schedule_blocks_bottom_up(ir_ctx *ctx)
{
	uint32_t max_edges_count = ctx->cfg_edges_count / 2;
	uint32_t edges_count = 0;
	uint32_t b, i, loop_depth;
	float *bb_freq, freq;
	ir_block *bb;
	ir_edge_info *edges, *e;
	ir_chain *chains;
	ir_bitqueue worklist;
	ir_bitset visited;
	uint32_t *schedule_end, count;

	ctx->cfg_schedule = ir_mem_malloc(sizeof(uint32_t) * (ctx->cfg_blocks_count + 2));
	schedule_end = ctx->cfg_schedule + ctx->cfg_blocks_count;

	/* 1. Create initial chains for each BB */
	chains = ir_mem_malloc(sizeof(ir_chain) * (ctx->cfg_blocks_count + 1));
	chains[0].head = 0;
	chains[0].next = 0;
	chains[0].prev = 0;
	for (b = 1; b <= ctx->cfg_blocks_count; b++) {
		chains[b].head = b;
		chains[b].next = b;
		chains[b].prev = b;
	}

	/* 2. Collect information about BBs and EDGEs frequencies */
	edges = ir_mem_malloc(sizeof(ir_edge_info) * max_edges_count);
	bb_freq = ir_mem_calloc(ctx->cfg_blocks_count + 1, sizeof(float));
	bb_freq[1] = 1.0f;
	visited = ir_bitset_malloc(ctx->cfg_blocks_count + 1);
	ir_bitqueue_init(&worklist, ctx->cfg_blocks_count + 1);
	ir_bitqueue_add(&worklist, 1);
	while ((b = ir_bitqueue_pop(&worklist)) != (uint32_t)-1) {
restart:
		bb = &ctx->cfg_blocks[b];
		if (bb->predecessors_count) {
			uint32_t n = bb->predecessors_count;
			uint32_t *p = ctx->cfg_edges + bb->predecessors;
			for (; n > 0; p++, n--) {
				uint32_t predecessor = *p;
				/* Basic Blocks are ordered in a way that usual predecessors ids are less than successors.
				 * So we may compare blocks ids (predecessor < b) instead of a more expensive check for back edge
				 * (b != predecessor && ctx->cfg_blocks[predecessor].loop_header != b)
				 */
				if (predecessor < b) {
					if (!ir_bitset_in(visited, predecessor)) {
						b = predecessor;
						ir_bitqueue_del(&worklist, b);
						goto restart;
					}
				} else if (b != predecessor && ctx->cfg_blocks[predecessor].loop_header != b) {
					ir_dump_cfg(ctx, stderr);
					IR_ASSERT(b == predecessor || ctx->cfg_blocks[predecessor].loop_header == b);
				}
			}
		}

		ir_bitset_incl(visited, b);

		if ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY) {
			uint32_t successor = ctx->cfg_edges[bb->successors];

			/* move empty blocks to the end */
			IR_ASSERT(chains[b].head == b);
			chains[b].head = 0;
			*schedule_end = b;
			schedule_end--;

			if (successor > b) {
				bb_freq[successor] += bb_freq[b];
				b = successor;
				ir_bitqueue_del(&worklist, b);
				goto restart;
			} else {
				continue;
			}
		}

		loop_depth = bb->loop_depth;
		if (bb->flags & IR_BB_LOOP_HEADER) {
			// TODO: Estimate the loop iterations count
			bb_freq[b] *= 10;
		}

		if (bb->successors_count) {
			uint32_t n = bb->successors_count;
			uint32_t *p = ctx->cfg_edges + bb->successors;

			if (n == 1) {
				uint32_t successor = *p;

				IR_ASSERT(edges_count < max_edges_count);
				freq = bb_freq[b];
				if (successor > b) {
					IR_ASSERT(!ir_bitset_in(visited, successor));
					bb_freq[successor] += freq;
					ir_bitqueue_add(&worklist, successor);
				}
				successor = _ir_skip_empty_blocks(ctx, successor);
				edges[edges_count].from = b;
				edges[edges_count].to = successor;
				edges[edges_count].freq = freq;
				edges_count++;
			} else if (n == 2 && ctx->ir_base[bb->end].op == IR_IF) {
				uint32_t successor1 = *p;
				ir_block *successor1_bb = &ctx->cfg_blocks[successor1];
				ir_insn *insn1 = &ctx->ir_base[successor1_bb->start];
				uint32_t successor2 = *(p + 1);
				ir_block *successor2_bb = &ctx->cfg_blocks[successor2];
				ir_insn *insn2 = &ctx->ir_base[successor2_bb->start];
				int prob1, prob2, probN = 100;

				if (insn1->op2) {
					prob1 = insn1->op2;
					if (insn2->op2) {
						prob2 = insn2->op2;
						probN = prob1 + prob2;
					} else {
						if (prob1 > 100) {
							prob1 = 100;
						}
						prob2 = 100 - prob1;
					}

				} else if (insn2->op2) {
					prob2 = insn2->op2;
					if (prob2 > 100) {
						prob2 = 100;
					}
					prob1 = 100 - prob2;
				} else if (successor1_bb->loop_depth >= loop_depth
						&& successor2_bb->loop_depth < loop_depth) {
					prob1 = 90;
					prob2 = 10;
				} else if (successor1_bb->loop_depth < loop_depth
						&& successor2_bb->loop_depth >= loop_depth) {
					prob1 = 10;
					prob2 = 90;
				} else if (successor2_bb->flags & IR_BB_EMPTY) {
					prob1 = 51;
					prob2 = 49;
				} else if (successor1_bb->flags & IR_BB_EMPTY) {
					prob1 = 49;
					prob2 = 51;
				} else {
					prob1 = prob2 = 50;
				}
				do {
					freq = bb_freq[b] * (float)prob1 / (float)probN;
					if (successor1 > b) {
						IR_ASSERT(!ir_bitset_in(visited, successor1));
						bb_freq[successor1] += freq;
						if (successor1_bb->successors_count == 0 && insn1->op2 == 1) {
							/* move cold block without successors to the end */
							IR_ASSERT(chains[successor1].head == successor1);
							chains[successor1].head = 0;
							*schedule_end = successor1;
							schedule_end--;
							break;
						} else {
							ir_bitqueue_add(&worklist, successor1);
						}
					}
					/* try to join edges early to reduce number of edges and the cost of their sorting */
					if (prob1 > prob2
					 && (successor1_bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) != IR_BB_EMPTY) {
						uint32_t src = chains[b].next;
						IR_ASSERT(chains[src].head == src);
						IR_ASSERT(src == ir_chain_head(chains, b));
						IR_ASSERT(chains[successor1].head == successor1);
						ir_join_chains(chains, src, successor1);
						if (!IR_DEBUG_BB_SCHEDULE_GRAPH) break;
					}
					successor1 = _ir_skip_empty_blocks(ctx, successor1);
					IR_ASSERT(edges_count < max_edges_count);
					edges[edges_count].from = b;
					edges[edges_count].to = successor1;
					edges[edges_count].freq = freq;
					edges_count++;
				} while (0);
				do {
					freq = bb_freq[b] * (float)prob2 / (float)probN;
					if (successor2 > b) {
						IR_ASSERT(!ir_bitset_in(visited, successor2));
						bb_freq[successor2] += freq;
						if (successor2_bb->successors_count == 0 && insn2->op2 == 1) {
							/* move cold block without successors to the end */
							IR_ASSERT(chains[successor2].head == successor2);
							chains[successor2].head = 0;
							*schedule_end = successor2;
							schedule_end--;
							break;
						} else {
							ir_bitqueue_add(&worklist, successor2);
						}
					}
					if (prob2 > prob1
					 && (successor2_bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) != IR_BB_EMPTY) {
						uint32_t src = chains[b].next;
						IR_ASSERT(chains[src].head == src);
						IR_ASSERT(src == ir_chain_head(chains, b));
						IR_ASSERT(chains[successor2].head == successor2);
						ir_join_chains(chains, src, successor2);
						if (!IR_DEBUG_BB_SCHEDULE_GRAPH) break;
					}
					successor2 = _ir_skip_empty_blocks(ctx, successor2);
					IR_ASSERT(edges_count < max_edges_count);
					edges[edges_count].from = b;
					edges[edges_count].to = successor2;
					edges[edges_count].freq = freq;
					edges_count++;
				} while (0);
			} else {
				int prob;

				for (; n > 0; p++, n--) {
					uint32_t successor = *p;
					ir_block *successor_bb = &ctx->cfg_blocks[successor];
					ir_insn *insn = &ctx->ir_base[successor_bb->start];

					if (insn->op == IR_CASE_DEFAULT) {
						prob = insn->op2;
						if (!prob) {
							prob = 100 / bb->successors_count;
						}
					} else if (insn->op == IR_CASE_VAL) {
						prob = insn->op3;
						if (!prob) {
							prob = 100 / bb->successors_count;
						}
					} else if (insn->op == IR_ENTRY) {
						if ((ctx->flags & IR_MERGE_EMPTY_ENTRIES) && (successor_bb->flags & IR_BB_EMPTY)) {
							prob = 99; /* prefer empty ENTRY block to go first */
						} else {
							prob = 1;
						}
					} else {
						prob = 100 / bb->successors_count;
					}
					freq = bb_freq[b] * (float)prob / 100.0f;
					if (successor > b) {
						IR_ASSERT(!ir_bitset_in(visited, successor));
						bb_freq[successor] += freq;
						ir_bitqueue_add(&worklist, successor);
					}
					successor = _ir_skip_empty_blocks(ctx, successor);
					IR_ASSERT(edges_count < max_edges_count);
					edges[edges_count].from = b;
					edges[edges_count].to = successor;
					edges[edges_count].freq = freq;
					edges_count++;
				}
			}
		}
	}
	ir_bitqueue_free(&worklist);
	ir_mem_free(visited);

	/* 2. Sort EDGEs according to their frequencies */
	qsort(edges, edges_count, sizeof(ir_edge_info), ir_edge_info_cmp);

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_BB_SCHEDULE) {
		ir_dump_edges(ctx, edges_count, edges);
	}
#endif

	/* 3. Process EDGEs in the decreasing frequency order and join the connected chains */
	for (e = edges, i = edges_count; i > 0; e++, i--) {
		uint32_t dst = chains[e->to].head;
		if (dst == e->to) {
			uint32_t src = chains[e->from].next;
			if (chains[src].head == src) {
				IR_ASSERT(src == ir_chain_head(chains, e->from) && chains[src].tail == e->from);
				if (src != dst) {
					ir_join_chains(chains, src, dst);
				} else if (ctx->cfg_blocks[e->from].successors_count < 2) {
					/* Try to rotate loop chian to finish it with a conditional branch */
					uint32_t tail = e->from;
					uint32_t prev = src;
					uint32_t next = chains[prev].next;
					uint32_t best = 0;

					while (prev != tail) {
						if (ctx->ir_base[ctx->cfg_blocks[prev].end].op == IR_IF) {
							if (ctx->ir_base[ctx->cfg_blocks[prev].start].op == IR_LOOP_BEGIN
							 && ctx->cfg_blocks[prev].loop_depth > 1) {
								best = next;
								break;
							} else if (!best || bb_freq[next] < bb_freq[best]) {
								/* Find the successor of IF with the least frequency */
								best = next;
							}
						}
						prev = next;
						next = chains[next].next;
					}
					if (best) {
						/* change the head of the chain */
						chains[src].head = best;
						chains[best].head = best;
					}
				}
#if !IR_DEBUG_BB_SCHEDULE_GRAPH
				e->from = 0; /* reset "from" to avoid check on step #5 */
#endif
			}
		}
	}

#if IR_DEBUG_BB_SCHEDULE_GRAPH
	if (ctx->flags & IR_DEBUG_BB_SCHEDULE) {
		ir_dump_cfg_freq_graph(ctx, bb_freq, edges_count, edges, chains);
	}
#endif

	ir_mem_free(bb_freq);

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_BB_SCHEDULE) {
		ir_dump_chains(ctx, chains);
	}
#endif

	/* 4. Merge empty entry blocks */
	if ((ctx->flags & IR_MERGE_EMPTY_ENTRIES) && ctx->entries_count) {
		for (i = 0; i < ctx->entries_count; i++) {
			b = ctx->entries[i];
			IR_ASSERT(ctx->cfg_blocks[b].flags & IR_BB_ENTRY);
			if (b && chains[b].head == b && chains[b].tail == b) {
				bb = &ctx->cfg_blocks[b];
				if (bb->flags & IR_BB_EMPTY) {
					uint32_t successor;

					do {
						IR_ASSERT(bb->successors_count == 1);
						successor = ctx->cfg_edges[bb->successors];
						bb = &ctx->cfg_blocks[successor];
					} while ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY);
					IR_ASSERT(chains[successor].head);
					ir_insert_chain_before(chains, b, successor);
				}
			}
		}

#ifdef IR_DEBUG
		if (ctx->flags & IR_DEBUG_BB_SCHEDULE) {
			ir_dump_chains(ctx, chains);
		}
#endif
	}

	/* 5. Align loop headers */
	for (b = 1; b <= ctx->cfg_blocks_count; b++) {
		if (chains[b].head == b) {
			bb = &ctx->cfg_blocks[b];
			if (bb->loop_depth) {
				if ((bb->flags & IR_BB_LOOP_HEADER) || ir_chain_head(chains, bb->loop_header) == b) {
					bb->flags |= IR_BB_ALIGN_LOOP;
				}
			}
		}
	}

	/* 6. Group chains according to the most frequent edge between them */
	// TODO: Try to find a better heuristic
	for (e = edges, i = edges_count; i > 0; e++, i--) {
#if !IR_DEBUG_BB_SCHEDULE_GRAPH
		if (!e->from) continue;
#endif
		uint32_t src = ir_chain_head(chains, e->from);
		uint32_t dst = ir_chain_head(chains, e->to);
		if (src != dst) {
			if (dst == 1) {
				ir_join_chains(chains, dst, src);
			} else {
				ir_join_chains(chains, src, dst);
			}
		}
	}

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_BB_SCHEDULE) {
		ir_dump_chains(ctx, chains);
	}
#endif

	/* 7. Form a final BB order */
	count = 0;
	for (b = 1; b <= ctx->cfg_blocks_count; b++) {
		if (chains[b].head == b) {
			uint32_t tail = chains[b].tail;
			uint32_t i = b;
			while (1) {
				count++;
				ctx->cfg_schedule[count] = i;
				if (i == tail) break;
				i = chains[i].next;
			}
		}
	}

	IR_ASSERT(ctx->cfg_schedule + count == schedule_end);
	ctx->cfg_schedule[ctx->cfg_blocks_count + 1] = 0;

	ir_mem_free(edges);
	ir_mem_free(chains);

	return 1;
}

/* A variation of "Top-down Positioning" algorithm described by
 * Karl Pettis and Robert C. Hansen "Profile Guided Code Positioning"
 */
static int ir_schedule_blocks_top_down(ir_ctx *ctx)
{
	ir_bitqueue blocks;
	uint32_t b, best_successor, last_non_empty;
	ir_block *bb, *best_successor_bb;
	ir_insn *insn;
	uint32_t *list, *schedule_end;
	uint32_t count = 0;

	ir_bitqueue_init(&blocks, ctx->cfg_blocks_count + 1);
	blocks.pos = 0;
	list = ir_mem_malloc(sizeof(uint32_t) * (ctx->cfg_blocks_count + 2));
	list[ctx->cfg_blocks_count + 1] = 0;
	schedule_end = list + ctx->cfg_blocks_count;
	for (b = 1; b <= ctx->cfg_blocks_count; b++) {
		ir_bitset_incl(blocks.set, b);
	}

	while ((b = ir_bitqueue_pop(&blocks)) != (uint32_t)-1) {
		bb = &ctx->cfg_blocks[b];
		/* Start trace */
		last_non_empty = 0;
		do {
			if (UNEXPECTED(bb->flags & IR_BB_PREV_EMPTY_ENTRY) && ir_bitqueue_in(&blocks, b - 1)) {
				/* Schedule the previous empty ENTRY block before this one */
				uint32_t predecessor = b - 1;

				ir_bitqueue_del(&blocks, predecessor);
				count++;
				list[count] = predecessor;
			}
			if ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY) {
				/* move empty blocks to the end */
				*schedule_end = b;
				schedule_end--;
			} else {
				count++;
				list[count] = b;
				last_non_empty = b;
			}
			best_successor_bb = NULL;
			if (bb->successors_count == 1) {
				best_successor = ctx->cfg_edges[bb->successors];
				if (ir_bitqueue_in(&blocks, best_successor)) {
					best_successor_bb = &ctx->cfg_blocks[best_successor];
				}
			} else if (bb->successors_count > 1) {
				uint32_t prob, best_successor_prob;
				uint32_t *p, successor;
				ir_block *successor_bb;

				for (b = 0, p = &ctx->cfg_edges[bb->successors]; b < bb->successors_count; b++, p++) {
					successor = *p;
					if (ir_bitqueue_in(&blocks, successor)) {
						successor_bb = &ctx->cfg_blocks[successor];
						insn = &ctx->ir_base[successor_bb->start];
						if (insn->op == IR_IF_TRUE || insn->op == IR_IF_FALSE) {
							prob = insn->op2;
							if (!prob) {
								prob = 100 / bb->successors_count;
								if (!(successor_bb->flags & IR_BB_EMPTY)) {
									prob++;
								}
							}
						} else if (insn->op == IR_CASE_DEFAULT) {
							prob = insn->op2;
							if (!prob) {
								prob = 100 / bb->successors_count;
							}
						} else if (insn->op == IR_CASE_VAL) {
							prob = insn->op3;
							if (!prob) {
								prob = 100 / bb->successors_count;
							}
						} else if (insn->op == IR_ENTRY) {
							if ((ctx->flags & IR_MERGE_EMPTY_ENTRIES) && (successor_bb->flags & IR_BB_EMPTY)) {
								prob = 99; /* prefer empty ENTRY block to go first */
							} else {
								prob = 1;
							}
						} else {
							prob = 100 / bb->successors_count;
						}
						if (!best_successor_bb
						 || successor_bb->loop_depth > best_successor_bb->loop_depth
						 || prob > best_successor_prob) {
							best_successor = successor;
							best_successor_bb = successor_bb;
							best_successor_prob = prob;
						}
					}
				}
			}
			if (!best_successor_bb) {
				/* Try to continue trace using the other successor of the last IF */
				if ((bb->flags & IR_BB_EMPTY) && last_non_empty) {
					bb = &ctx->cfg_blocks[last_non_empty];
					if (bb->successors_count == 2 && ctx->ir_base[bb->end].op == IR_IF) {
						b = ctx->cfg_edges[bb->successors];

						if (!ir_bitqueue_in(&blocks, b)) {
							b = ctx->cfg_edges[bb->successors + 1];
						}
						if (ir_bitqueue_in(&blocks, b)) {
							bb = &ctx->cfg_blocks[b];
							ir_bitqueue_del(&blocks, b);
							continue;
						}
					}
				}
				/* End trace */
				break;
			}
			b = best_successor;
			bb = best_successor_bb;
			ir_bitqueue_del(&blocks, b);
		} while (1);
	}

	IR_ASSERT(list + count == schedule_end);
	ctx->cfg_schedule = list;
	ir_bitqueue_free(&blocks);

	return 1;
}

int ir_schedule_blocks(ir_ctx *ctx)
{
	ir_ref ref;

	if (ctx->cfg_blocks_count <= 2) {
		return 1;
	}

	/* Mark "stop" blocks termintated with UNREACHABLE as "unexpected" */
	ref = ctx->ir_base[1].op1;
	while (ref) {
		ir_insn *insn = &ctx->ir_base[ref];

		if (insn->op == IR_UNREACHABLE && ctx->ir_base[insn->op1].op != IR_TAILCALL) {
			ir_block *bb = &ctx->cfg_blocks[ctx->cfg_map[ref]];
			uint32_t n = bb->predecessors_count;

			if (n == 1) {
				ir_insn *start_insn = &ctx->ir_base[bb->start];
				if (start_insn->op == IR_IF_TRUE
				 || start_insn->op == IR_IF_FALSE
				 || start_insn->op == IR_CASE_DEFAULT) {
					if (!start_insn->op2) start_insn->op2 = 1;
				} else if (start_insn->op == IR_CASE_VAL) {
					if (!start_insn->op3) start_insn->op3 = 1;
				}
			} else if (n > 1) {
				uint32_t *p = &ctx->cfg_edges[bb->predecessors];

				for (; n > 0; p++, n--) {
					bb = &ctx->cfg_blocks[*p];
					if (bb->predecessors_count == 1) {
						ir_insn *start_insn = &ctx->ir_base[bb->start];
						if (start_insn->op == IR_IF_TRUE
						 || start_insn->op == IR_IF_FALSE
						 || start_insn->op == IR_CASE_DEFAULT) {
							if (!start_insn->op2) start_insn->op2 = 1;
						} else if (start_insn->op == IR_CASE_VAL) {
							if (!start_insn->op3) start_insn->op3 = 1;
						}
					}
				}
			}
		}
		ref = insn->op3;
	}

	/* The bottom-up Pettis-Hansen algorithm is expensive - O(n^3),
	 * use it only for relatively small functions.
	 *
	 * TODO: make the choice between top-down and bottom-up algorithm configurable
	 */
	if (UNEXPECTED(ctx->flags2 & IR_IRREDUCIBLE_CFG) || ctx->cfg_blocks_count > 256) {
		return ir_schedule_blocks_top_down(ctx);
	} else {
		return ir_schedule_blocks_bottom_up(ctx);
	}
}

/* JMP target optimisation */
uint32_t ir_skip_empty_target_blocks(const ir_ctx *ctx, uint32_t b)
{
	return _ir_skip_empty_blocks(ctx, b);
}

uint32_t ir_next_block(const ir_ctx *ctx, uint32_t b)
{
	ir_block *bb;

	if (ctx->cfg_schedule) {
		uint32_t ret = ctx->cfg_schedule[++b];

		/* Check for empty ENTRY block */
		while (ret && ((ctx->cfg_blocks[ret].flags & (IR_BB_START|IR_BB_EMPTY)) == IR_BB_EMPTY)) {
			ret = ctx->cfg_schedule[++b];
		}
		return ret;
	}

	b++;
	while (1) {
		if (b > ctx->cfg_blocks_count) {
			return 0;
		}

		bb = &ctx->cfg_blocks[b];

		if ((bb->flags & (IR_BB_START|IR_BB_EMPTY)) == IR_BB_EMPTY) {
			b++;
		} else {
			break;
		}
	}
	return b;
}

void ir_get_true_false_blocks(const ir_ctx *ctx, uint32_t b, uint32_t *true_block, uint32_t *false_block)
{
	ir_block *bb;
	uint32_t *p, use_block;

	*true_block = 0;
	*false_block = 0;
	bb = &ctx->cfg_blocks[b];
	IR_ASSERT(ctx->ir_base[bb->end].op == IR_IF);
	IR_ASSERT(bb->successors_count == 2);
	p = &ctx->cfg_edges[bb->successors];
	use_block = *p;
	if (ctx->ir_base[ctx->cfg_blocks[use_block].start].op == IR_IF_TRUE) {
		*true_block = ir_skip_empty_target_blocks(ctx, use_block);
		use_block = *(p+1);
		IR_ASSERT(ctx->ir_base[ctx->cfg_blocks[use_block].start].op == IR_IF_FALSE);
		*false_block = ir_skip_empty_target_blocks(ctx, use_block);
	} else {
		IR_ASSERT(ctx->ir_base[ctx->cfg_blocks[use_block].start].op == IR_IF_FALSE);
		*false_block = ir_skip_empty_target_blocks(ctx, use_block);
		use_block = *(p+1);
		IR_ASSERT(ctx->ir_base[ctx->cfg_blocks[use_block].start].op == IR_IF_TRUE);
		*true_block = ir_skip_empty_target_blocks(ctx, use_block);
	}
	IR_ASSERT(*true_block && *false_block);
}
