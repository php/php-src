/*
 * IR - Lightweight JIT Compilation Framework
 * (CFG - Control Flow Graph)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

static ir_ref _ir_merge_blocks(ir_ctx *ctx, ir_ref end, ir_ref begin)
{
	ir_ref prev, next;
	ir_use_list *use_list;
	ir_ref n, *p;

	IR_ASSERT(ctx->ir_base[begin].op == IR_BEGIN);
	IR_ASSERT(ctx->ir_base[end].op == IR_END);
	IR_ASSERT(ctx->ir_base[begin].op1 == end);
	IR_ASSERT(ctx->use_lists[end].count == 1);

	prev = ctx->ir_base[end].op1;

	use_list = &ctx->use_lists[begin];
	IR_ASSERT(use_list->count == 1);
	next = ctx->use_edges[use_list->refs];

	/* remove BEGIN and END */
	ctx->ir_base[begin].op = IR_NOP;
	ctx->ir_base[begin].op1 = IR_UNUSED;
	ctx->use_lists[begin].count = 0;
	ctx->ir_base[end].op = IR_NOP;
	ctx->ir_base[end].op1 = IR_UNUSED;
	ctx->use_lists[end].count = 0;

	/* connect their predecessor and successor */
	ctx->ir_base[next].op1 = prev;
	use_list = &ctx->use_lists[prev];
	n = use_list->count;
	for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
		if (*p == end) {
			*p = next;
		}
	}

	return next;
}

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

		IR_ASSERT(IR_IS_BB_END(insn->op));
		/* Remember BB end */
		end = ref;
		/* Some successors of IF and SWITCH nodes may be inaccessible by backward DFS */
		use_list = &ctx->use_lists[end];
		n = use_list->count;
		if (n > 1) {
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
				if (insn->op == IR_BEGIN
				 && (ctx->flags & IR_OPT_CFG)
				 && ctx->ir_base[insn->op1].op == IR_END
				 && ctx->use_lists[ref].count == 1) {
					ref = _ir_merge_blocks(ctx, insn->op1, ref);
					ref = ctx->ir_base[ref].op1;
					continue;
				}
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
next_successor:
				if (IR_IS_BB_END(insn->op)) {
					if (insn->op == IR_END && (ctx->flags & IR_OPT_CFG)) {
						use_list = &ctx->use_lists[ref];
						IR_ASSERT(use_list->count == 1);
						next = ctx->use_edges[use_list->refs];

						if (ctx->ir_base[next].op == IR_BEGIN
						 && ctx->use_lists[next].count == 1) {
							ref = _ir_merge_blocks(ctx, ref, next);
							insn = &ctx->ir_base[ref];
							goto next_successor;
						}
					}
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
	bb_init_falgs = (ctx->flags2 & IR_SCCP_DONE) ? 0 : IR_BB_UNREACHABLE;
	IR_BITSET_FOREACH(bb_starts, len, start) {
		end = _blocks[start];
		_blocks[start] = b;
		_blocks[end] = b;
		insn = &ctx->ir_base[start];
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

	if (!(ctx->flags2 & IR_SCCP_DONE)) {
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

static void ir_remove_from_use_list(ir_ctx *ctx, ir_ref from, ir_ref ref)
{
	ir_ref j, n, *p, *q, use;
	ir_use_list *use_list = &ctx->use_lists[from];
	ir_ref skip = 0;

	n = use_list->count;
	for (j = 0, p = q = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
		use = *p;
		if (use == ref) {
			skip++;
		} else {
			if (p != q) {
				*q = use;
			}
			q++;
		}
	}
	use_list->count -= skip;
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
							ir_remove_from_use_list(ctx, input, use);
						}
					}
					use_insn->op = IR_COPY;
					use_insn->op2 = IR_UNUSED;
					use_insn->op3 = IR_UNUSED;
					ir_remove_from_use_list(ctx, merge, use);
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
							ir_remove_from_use_list(ctx, input, use);
						}
					}
				}
			}
		}
	}
	ir_mem_free(life_inputs);
	ir_remove_from_use_list(ctx, from, merge);
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

				IR_ASSERT(blocks[pred_b].idom > 0);
				if (bb->idom != pred_b) {
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
 * Cooper, Harvey and Kennedy, that relays on IR block ordering */
int ir_build_dominators_tree(ir_ctx *ctx)
{
	uint32_t blocks_count, b;
	ir_block *blocks, *bb;
	uint32_t *edges;

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

		if (UNEXPECTED(idom > b)) {
			/* In rare cases, LOOP_BEGIN.op1 may be a back-edge. Skip back-edges. */
			ctx->flags2 &= ~IR_NO_LOOPS;
			while (1) {
				k--;
				p++;
				idom = *p;
				if (idom < b) {
					break;
				}
				IR_ASSERT(k > 0);
			}
		}
		IR_ASSERT(blocks[idom].idom > 0);

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
				}
			}
		}
	}

	ir_mem_free(entry_times);
	ir_worklist_free(&work);

	return 1;
}

/* A variation of "Top-down Positioning" algorithm described by
 * Karl Pettis and Robert C. Hansen "Profile Guided Code Positioning"
 *
 * TODO: Switch to "Bottom-up Positioning" algorithm
 */
int ir_schedule_blocks(ir_ctx *ctx)
{
	ir_bitqueue blocks;
	uint32_t b, best_successor, j, last_non_empty;
	ir_block *bb, *best_successor_bb;
	ir_insn *insn;
	uint32_t *list, *map;
	uint32_t count = 0;
	bool reorder = 0;

	ir_bitqueue_init(&blocks, ctx->cfg_blocks_count + 1);
	blocks.pos = 0;
	list = ir_mem_malloc(sizeof(uint32_t) * (ctx->cfg_blocks_count + 1) * 2);
	map = list + (ctx->cfg_blocks_count + 1);
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
				map[predecessor] = count;
				if (predecessor != count) {
					reorder = 1;
				}
			}
			count++;
			list[count] = b;
			map[b] = count;
			if (b != count) {
				reorder = 1;
			}
			if (!(bb->flags & IR_BB_EMPTY)) {
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

	if (reorder) {
		ir_block *cfg_blocks = ir_mem_malloc(sizeof(ir_block) * (ctx->cfg_blocks_count + 1));

		memset(ctx->cfg_blocks, 0, sizeof(ir_block));
		for (b = 1, bb = cfg_blocks + 1; b <= count; b++, bb++) {
			*bb = ctx->cfg_blocks[list[b]];
			if (bb->dom_parent > 0) {
				bb->dom_parent = map[bb->dom_parent];
			}
			if (bb->dom_child > 0) {
				bb->dom_child = map[bb->dom_child];
			}
			if (bb->dom_next_child > 0) {
				bb->dom_next_child = map[bb->dom_next_child];
			}
			if (bb->loop_header > 0) {
				bb->loop_header = map[bb->loop_header];
			}
		}
		for (j = 0; j < ctx->cfg_edges_count; j++) {
			if (ctx->cfg_edges[j] > 0) {
				ctx->cfg_edges[j] = map[ctx->cfg_edges[j]];
			}
		}
		ir_mem_free(ctx->cfg_blocks);
		ctx->cfg_blocks = cfg_blocks;

		if (ctx->osr_entry_loads) {
			ir_list *list = (ir_list*)ctx->osr_entry_loads;
			uint32_t pos = 0, count;

			while (1) {
				b = ir_list_at(list, pos);
				if (b == 0) {
					break;
				}
				ir_list_set(list, pos, map[b]);
				pos++;
				count = ir_list_at(list, pos);
				pos += count + 1;
			}
		}

		if (ctx->cfg_map) {
			ir_ref i;

			for (i = IR_UNUSED + 1; i < ctx->insns_count; i++) {
				ctx->cfg_map[i] = map[ctx->cfg_map[i]];
			}
		}
	}

	ir_mem_free(list);
	ir_bitqueue_free(&blocks);

	return 1;
}

/* JMP target optimisation */
uint32_t ir_skip_empty_target_blocks(const ir_ctx *ctx, uint32_t b)
{
	ir_block *bb;

	while (1) {
		bb = &ctx->cfg_blocks[b];

		if ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY) {
			b = ctx->cfg_edges[bb->successors];
		} else {
			break;
		}
	}
	return b;
}

uint32_t ir_skip_empty_next_blocks(const ir_ctx *ctx, uint32_t b)
{
	ir_block *bb;

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

void ir_get_true_false_blocks(const ir_ctx *ctx, uint32_t b, uint32_t *true_block, uint32_t *false_block, uint32_t *next_block)
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
	*next_block = b == ctx->cfg_blocks_count ? 0 : ir_skip_empty_next_blocks(ctx, b + 1);
}
