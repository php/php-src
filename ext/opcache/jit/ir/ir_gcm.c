/*
 * IR - Lightweight JIT Compilation Framework
 * (GCM - Global Code Motion and Scheduler)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * The GCM algorithm is based on Cliff Click's publication
 * See: C. Click. "Global code motion, global value numbering" Submitted to PLDI'95.
 */

#include "ir.h"
#include "ir_private.h"

static int32_t ir_gcm_schedule_early(ir_ctx *ctx, int32_t *_blocks, ir_ref ref, ir_list *queue_rest)
{
	ir_ref n, *p, input;
	ir_insn *insn;
	uint32_t dom_depth;
	int32_t b, result;
	bool reschedule_late = 1;

	insn = &ctx->ir_base[ref];

	IR_ASSERT(insn->op != IR_PARAM && insn->op != IR_VAR);
	IR_ASSERT(insn->op != IR_PHI && insn->op != IR_PI);

	result = 1;
	dom_depth = 0;

	n = insn->inputs_count;
	for (p = insn->ops + 1; n > 0; p++, n--) {
		input = *p;
		if (input > 0) {
			b = _blocks[input];
			if (b == 0) {
				b = ir_gcm_schedule_early(ctx, _blocks, input, queue_rest);
			} else if (b < 0) {
				b = -b;
			}
			if (dom_depth < ctx->cfg_blocks[b].dom_depth) {
				dom_depth = ctx->cfg_blocks[b].dom_depth;
				result = b;
			}
			reschedule_late = 0;
		}
	}
	_blocks[ref] = -result;

	if (UNEXPECTED(reschedule_late)) {
		/* Floating nodes that don't depend on other nodes
		 * (e.g. only on constants), have to be scheduled to the
		 * last common ancestor. Otherwise they always go to the
		 * first block.
		 */
		ir_list_push_unchecked(queue_rest, ref);
	}
	return result;
}

/* Last Common Ancestor */
static int32_t ir_gcm_find_lca(ir_ctx *ctx, int32_t b1, int32_t b2)
{
	uint32_t dom_depth;

	dom_depth = ctx->cfg_blocks[b2].dom_depth;
	while (ctx->cfg_blocks[b1].dom_depth > dom_depth) {
		b1 = ctx->cfg_blocks[b1].dom_parent;
	}
	dom_depth = ctx->cfg_blocks[b1].dom_depth;
	while (ctx->cfg_blocks[b2].dom_depth > dom_depth) {
		b2 = ctx->cfg_blocks[b2].dom_parent;
	}
	while (b1 != b2) {
		b1 = ctx->cfg_blocks[b1].dom_parent;
		b2 = ctx->cfg_blocks[b2].dom_parent;
	}
	return b2;
}

static void ir_gcm_schedule_late(ir_ctx *ctx, int32_t *_blocks, ir_ref ref)
{
	ir_ref n, *p, use;
	ir_insn *insn;
	ir_use_list *use_list;

	IR_ASSERT(_blocks[ref] < 0);
	_blocks[ref] = -_blocks[ref];
	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	if (n) {
		int32_t lca, b;

		insn = &ctx->ir_base[ref];
		IR_ASSERT(insn->op != IR_PARAM && insn->op != IR_VAR);
		IR_ASSERT(insn->op != IR_PHI && insn->op != IR_PI);

		lca = 0;
		for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
			use = *p;
			b = _blocks[use];
			if (!b) {
				continue;
			} else if (b < 0) {
				ir_gcm_schedule_late(ctx, _blocks, use);
				b = _blocks[use];
				IR_ASSERT(b != 0);
			}
			insn = &ctx->ir_base[use];
			if (insn->op == IR_PHI) {
				ir_ref *p = insn->ops + 2; /* PHI data inputs */
				ir_ref *q = ctx->ir_base[insn->op1].ops + 1; /* MERGE inputs */
				ir_ref n = insn->inputs_count - 1;

				for (;n > 0; p++, q++, n--) {
					if (*p == ref) {
						b = _blocks[*q];
						lca = !lca ? b : ir_gcm_find_lca(ctx, lca, b);
					}
				}
			} else {
				lca = !lca ? b : ir_gcm_find_lca(ctx, lca, b);
			}
		}
		IR_ASSERT(lca != 0 && "No Common Ancestor");
		b = lca;

		if (b != _blocks[ref]) {
			ir_block *bb = &ctx->cfg_blocks[b];
			uint32_t loop_depth = bb->loop_depth;

			if (loop_depth) {
				uint32_t flags;

				use_list = &ctx->use_lists[ref];
				if (use_list->count == 1) {
					use = ctx->use_edges[use_list->refs];
					insn = &ctx->ir_base[use];
					if (insn->op == IR_IF || insn->op == IR_GUARD || insn->op == IR_GUARD_NOT) {
						_blocks[ref] = b;
						return;
					}
				}

				flags = (bb->flags & IR_BB_LOOP_HEADER) ? bb->flags : ctx->cfg_blocks[bb->loop_header].flags;
				if ((flags & IR_BB_LOOP_WITH_ENTRY)
				 && !(ctx->binding && ir_binding_find(ctx, ref))) {
					/* Don't move loop invariant code across an OSR ENTRY if we can't restore it */
				} else {
					do {
						lca = bb->dom_parent;
						bb = &ctx->cfg_blocks[lca];
						if (bb->loop_depth < loop_depth) {
							if (!bb->loop_depth) {
								b = lca;
								break;
							}
							flags = (bb->flags & IR_BB_LOOP_HEADER) ? bb->flags : ctx->cfg_blocks[bb->loop_header].flags;
							if ((flags & IR_BB_LOOP_WITH_ENTRY)
							 && !(ctx->binding && ir_binding_find(ctx, ref))) {
								break;
							}
							loop_depth = bb->loop_depth;
							b = lca;
						}
					} while (lca != _blocks[ref]);
				}
			}
			_blocks[ref] = b;
			if (ctx->ir_base[ref + 1].op == IR_OVERFLOW) {
				/* OVERFLOW is a projection and must be scheduled together with previous ADD/SUB/MUL_OV */
				_blocks[ref + 1] = b;
			}
		}
	}
}

static void ir_gcm_schedule_rest(ir_ctx *ctx, int32_t *_blocks, ir_ref ref)
{
	ir_ref n, *p, use;
	ir_insn *insn;

	IR_ASSERT(_blocks[ref] < 0);
	_blocks[ref] = -_blocks[ref];
	n = ctx->use_lists[ref].count;
	if (n) {
		uint32_t lca;
		int32_t b;

		insn = &ctx->ir_base[ref];
		IR_ASSERT(insn->op != IR_PARAM && insn->op != IR_VAR);
		IR_ASSERT(insn->op != IR_PHI && insn->op != IR_PI);

		lca = 0;
		for (p = &ctx->use_edges[ctx->use_lists[ref].refs]; n > 0; p++, n--) {
			use = *p;
			b = _blocks[use];
			if (!b) {
				continue;
			} else if (b < 0) {
				ir_gcm_schedule_late(ctx, _blocks, use);
				b = _blocks[use];
				IR_ASSERT(b != 0);
			}
			insn = &ctx->ir_base[use];
			if (insn->op == IR_PHI) {
				ir_ref *p = insn->ops + 2; /* PHI data inputs */
				ir_ref *q = ctx->ir_base[insn->op1].ops + 1; /* MERGE inputs */

				ir_ref n = insn->inputs_count - 1;

				for (;n > 0; p++, q++, n--) {
					if (*p == ref) {
						b = _blocks[*q];
						lca = !lca ? b : ir_gcm_find_lca(ctx, lca, b);
					}
				}
			} else {
				lca = !lca ? b : ir_gcm_find_lca(ctx, lca, b);
			}
		}
		IR_ASSERT(lca != 0 && "No Common Ancestor");
		b = lca;
		_blocks[ref] = b;
		if (ctx->ir_base[ref + 1].op == IR_OVERFLOW) {
			/* OVERFLOW is a projection and must be scheduled together with previous ADD/SUB/MUL_OV */
			_blocks[ref + 1] = b;
		}
	}
}

int ir_gcm(ir_ctx *ctx)
{
	ir_ref k, n, *p, ref;
	ir_block *bb;
	ir_list queue_early;
	ir_list queue_late;
	ir_list queue_rest;
	int32_t *_blocks, b;
	ir_insn *insn, *use_insn;
	ir_use_list *use_list;

	IR_ASSERT(ctx->cfg_map);
	_blocks = (int32_t*)ctx->cfg_map;

	ir_list_init(&queue_early, ctx->insns_count);

	if (ctx->cfg_blocks_count == 1) {
		ref = ctx->cfg_blocks[1].end;
		do {
			insn = &ctx->ir_base[ref];
			_blocks[ref] = 1; /* pin to block */
			if (insn->inputs_count > 1) {
				/* insn has input data edges */
				ir_list_push_unchecked(&queue_early, ref);
			}
			ref = insn->op1; /* control predecessor */
		} while (ref != 1); /* IR_START */
		_blocks[1] = 1; /* pin to block */

		use_list = &ctx->use_lists[1];
		n = use_list->count;
		for (p = &ctx->use_edges[use_list->refs]; n > 0; n--, p++) {
			ref = *p;
			use_insn = &ctx->ir_base[ref];
			if (use_insn->op == IR_PARAM || use_insn->op == IR_VAR) {
				ctx->cfg_blocks[1].flags |= (use_insn->op == IR_PARAM) ? IR_BB_HAS_PARAM : IR_BB_HAS_VAR;
				_blocks[ref] = 1; /* pin to block */
			}
		}

		/* Place all live nodes to the first block */
		while (ir_list_len(&queue_early)) {
			ref = ir_list_pop(&queue_early);
			insn = &ctx->ir_base[ref];
			n = insn->inputs_count;
			for (p = insn->ops + 1; n > 0; p++, n--) {
				ref = *p;
				if (ref > 0 && _blocks[ref] == 0) {
					_blocks[ref] = 1;
					ir_list_push_unchecked(&queue_early, ref);
				}
			}
		}

		ir_list_free(&queue_early);

		return 1;
	}

	ir_list_init(&queue_late, ctx->insns_count);

	/* pin and collect control and control depended (PARAM, VAR, PHI, PI) instructions */
	b = ctx->cfg_blocks_count;
	for (bb = ctx->cfg_blocks + b; b > 0; bb--, b--) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		ref = bb->end;

		/* process the last instruction of the block */
		insn = &ctx->ir_base[ref];
		_blocks[ref] = b; /* pin to block */
		if (insn->inputs_count > 1) {
			/* insn has input data edges */
			ir_list_push_unchecked(&queue_early, ref);
		}
		ref = insn->op1; /* control predecessor */

		while (ref != bb->start) {
			insn = &ctx->ir_base[ref];
			_blocks[ref] = b; /* pin to block */
			if (insn->inputs_count > 1) {
				/* insn has input data edges */
				ir_list_push_unchecked(&queue_early, ref);
			}
			if (insn->type != IR_VOID) {
				IR_ASSERT(ir_op_flags[insn->op] & IR_OP_FLAG_MEM);
				ir_list_push_unchecked(&queue_late, ref);
			}
			ref = insn->op1; /* control predecessor */
		}

		/* process the first instruction of the block */
		_blocks[ref] = b; /* pin to block */

		use_list = &ctx->use_lists[ref];
		n = use_list->count;
		if (n > 1) {
			for (p = &ctx->use_edges[use_list->refs]; n > 0; n--, p++) {
				ref = *p;
				use_insn = &ctx->ir_base[ref];
				if (use_insn->op == IR_PHI || use_insn->op == IR_PI) {
					bb->flags |= (use_insn->op == IR_PHI) ? IR_BB_HAS_PHI : IR_BB_HAS_PI;
					if (EXPECTED(ctx->use_lists[ref].count != 0)) {
						_blocks[ref] = b; /* pin to block */
						ir_list_push_unchecked(&queue_early, ref);
						ir_list_push_unchecked(&queue_late, ref);
					}
				} else if (use_insn->op == IR_PARAM) {
					bb->flags |= IR_BB_HAS_PARAM;
					_blocks[ref] = b; /* pin to block */
					if (EXPECTED(ctx->use_lists[ref].count != 0)) {
						ir_list_push_unchecked(&queue_late, ref);
					}
				} else if (use_insn->op == IR_VAR) {
					bb->flags |= IR_BB_HAS_VAR;
					_blocks[ref] = b; /* pin to block */
					if (EXPECTED(ctx->use_lists[ref].count != 0)) {
						/* This is necessary only for VADDR */
						ir_list_push_unchecked(&queue_late, ref);
					}
				}
			}
		}
	}

	ir_list_init(&queue_rest, ctx->insns_count);

	n = ir_list_len(&queue_early);
	while (n > 0) {
		n--;
		ref = ir_list_at(&queue_early, n);
		insn = &ctx->ir_base[ref];
		k = insn->inputs_count - 1;
		for (p = insn->ops + 2; k > 0; p++, k--) {
			ref = *p;
			if (ref > 0 && _blocks[ref] == 0) {
				ir_gcm_schedule_early(ctx, _blocks, ref, &queue_rest);
			}
		}
	}

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_GCM) {
		fprintf(stderr, "GCM Schedule Early\n");
		for (n = 1; n < ctx->insns_count; n++) {
			fprintf(stderr, "%d -> %d\n", n, _blocks[n]);
		}
	}
#endif

	n = ir_list_len(&queue_late);
	while (n > 0) {
		n--;
		ref = ir_list_at(&queue_late, n);
		use_list = &ctx->use_lists[ref];
		k = use_list->count;
		for (p = &ctx->use_edges[use_list->refs]; k > 0; p++, k--) {
			ref = *p;
			if (_blocks[ref] < 0) {
				ir_gcm_schedule_late(ctx, _blocks, ref);
			}
		}
	}

	n = ir_list_len(&queue_rest);
	while (n > 0) {
		n--;
		ref = ir_list_at(&queue_rest, n);
		ir_gcm_schedule_rest(ctx, _blocks, ref);
	}

	ir_list_free(&queue_early);
	ir_list_free(&queue_late);
	ir_list_free(&queue_rest);

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_GCM) {
		fprintf(stderr, "GCM Schedule Late\n");
		for (n = 1; n < ctx->insns_count; n++) {
			fprintf(stderr, "%d -> %d\n", n, _blocks[n]);
		}
	}
#endif

	return 1;
}

static void ir_xlat_binding(ir_ctx *ctx, ir_ref *_xlat)
{
	uint32_t n1, n2, pos;
	ir_ref key;
	ir_hashtab_bucket *b1, *b2;
	ir_hashtab *binding = ctx->binding;
	uint32_t hash_size = (uint32_t)(-(int32_t)binding->mask);

	memset((char*)binding->data - (hash_size * sizeof(uint32_t)), -1, hash_size * sizeof(uint32_t));
	n1 = binding->count;
	n2 = 0;
	pos = 0;
	b1 = binding->data;
	b2 = binding->data;
	while (n1 > 0) {
		key = b1->key;
		IR_ASSERT(key < ctx->insns_count);
		if (_xlat[key]) {
			key = _xlat[key];
			b2->key = key;
			if (b1->val > 0) {
				IR_ASSERT(_xlat[b1->val]);
				b2->val = _xlat[b1->val];
			} else {
				b2->val = b1->val;
			}
			key |= binding->mask;
			b2->next = ((uint32_t*)binding->data)[key];
			((uint32_t*)binding->data)[key] = pos;
			pos += sizeof(ir_hashtab_bucket);
			b2++;
			n2++;
		}
		b1++;
		n1--;
	}
	binding->count = n2;
}

IR_ALWAYS_INLINE ir_ref ir_count_constant(ir_ref *_xlat, ir_ref ref)
{
	if (!_xlat[ref]) {
		_xlat[ref] = ref; /* this is only a "used constant" marker */
		return 1;
	}
	return 0;
}

int ir_schedule(ir_ctx *ctx)
{
	ir_ctx new_ctx;
	ir_ref i, j, k, n, *p, *q, ref, new_ref, prev_ref, insns_count, consts_count, use_edges_count;
	ir_ref *_xlat;
	ir_ref *edges;
	uint32_t b, prev_b;
	uint32_t *_blocks = ctx->cfg_map;
	ir_ref *_next = ir_mem_malloc(ctx->insns_count * sizeof(ir_ref));
	ir_ref *_prev = ir_mem_malloc(ctx->insns_count * sizeof(ir_ref));
	ir_ref _move_down = 0;
	ir_block *bb;
	ir_insn *insn, *new_insn;
	ir_use_list *lists, *use_list, *new_list;

	/* Create a double-linked list of nodes ordered by BB, respecting BB->start and BB->end */
	prev_b = _blocks[1];
	IR_ASSERT(prev_b);
	_prev[1] = 0;
	_prev[ctx->cfg_blocks[1].end] = 0;
	for (i = 2, j = 1; i < ctx->insns_count; i++) {
		b = _blocks[i];
		IR_ASSERT((int32_t)b >= 0);
		if (b == prev_b) {
			/* add to the end of the list */
			_next[j] = i;
			_prev[i] = j;
			j = i;
		} else if (b > prev_b) {
			bb = &ctx->cfg_blocks[b];
			if (i == bb->start) {
				IR_ASSERT(bb->end > bb->start);
				prev_b = b;
				_prev[bb->end] = 0;
				/* add to the end of the list */
				_next[j] = i;
				_prev[i] = j;
				j = i;
			} else {
				IR_ASSERT(i != bb->end);
				/* move down late (see the following loop) */
				_next[i] = _move_down;
				_move_down = i;
			}
		} else if (b) {
			bb = &ctx->cfg_blocks[b];
			IR_ASSERT(i != bb->start);
			if (_prev[bb->end]) {
				/* move up, insert before the end of the already scheduled BB */
				k = bb->end;
			} else {
				/* move up, insert at the end of the block */
				k = ctx->cfg_blocks[b + 1].start;
			}
			/* insert before "k" */
			_prev[i] = _prev[k];
			_next[i] = k;
			_next[_prev[k]] = i;
			_prev[k] = i;
		}
	}
	_next[j] = 0;

	while (_move_down) {
		i = _move_down;
		_move_down = _next[i];
		b = _blocks[i];
		bb = &ctx->cfg_blocks[b];
		k = _next[bb->start];

		if (bb->flags & (IR_BB_HAS_PHI|IR_BB_HAS_PI|IR_BB_HAS_PARAM|IR_BB_HAS_VAR)) {
			/* insert after the start of the block and all PARAM, VAR, PI, PHI */
			insn = &ctx->ir_base[k];
			while (insn->op == IR_PHI || insn->op == IR_PARAM || insn->op == IR_VAR || insn->op == IR_PI) {
				k = _next[k];
				insn = &ctx->ir_base[k];
			}
		}

		/* insert before "k" */
		_prev[i] = _prev[k];
		_next[i] = k;
		_next[_prev[k]] = i;
		_prev[k] = i;
	}

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_SCHEDULE) {
		fprintf(stderr, "Before Schedule\n");
		for (i = 1; i != 0; i = _next[i]) {
			fprintf(stderr, "%d -> %d\n", i, _blocks[i]);
		}
	}
#endif

	_xlat = ir_mem_calloc((ctx->consts_count + ctx->insns_count), sizeof(ir_ref));
	_xlat += ctx->consts_count;
	_xlat[IR_TRUE] = IR_TRUE;
	_xlat[IR_FALSE] = IR_FALSE;
	_xlat[IR_NULL] = IR_NULL;
	_xlat[IR_UNUSED] = IR_UNUSED;
	insns_count = 1;
	consts_count = -(IR_TRUE - 1);

	/* Topological sort according dependencies inside each basic block */
	for (b = 1, bb = ctx->cfg_blocks + 1; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		/* Schedule BB start */
		i = bb->start;
		_xlat[i] = bb->start = insns_count;
		insn = &ctx->ir_base[i];
		if (insn->op == IR_CASE_VAL) {
			IR_ASSERT(insn->op2 < IR_TRUE);
			consts_count += ir_count_constant(_xlat, insn->op2);
		}
		n = insn->inputs_count;
		insns_count += ir_insn_inputs_to_len(n);
		i = _next[i];
		insn = &ctx->ir_base[i];
		if (bb->flags & (IR_BB_HAS_PHI|IR_BB_HAS_PI|IR_BB_HAS_PARAM|IR_BB_HAS_VAR)) {
			/* Schedule PARAM, VAR, PI */
			while (insn->op == IR_PARAM || insn->op == IR_VAR || insn->op == IR_PI) {
				_xlat[i] = insns_count;
				insns_count += 1;
				i = _next[i];
				insn = &ctx->ir_base[i];
			}
			/* Schedule PHIs */
			while (insn->op == IR_PHI) {
				ir_ref j, *p, input;

				_xlat[i] = insns_count;
				/* Reuse "n" from MERGE and skip first input */
				insns_count += ir_insn_inputs_to_len(n + 1);
				for (j = n, p = insn->ops + 2; j > 0; p++, j--) {
					input = *p;
					if (input < IR_TRUE) {
						consts_count += ir_count_constant(_xlat, input);
					}
				}
				i = _next[i];
				insn = &ctx->ir_base[i];
			}
		}
		while (i != bb->end) {
			ir_ref n, j, *p, input;

restart:
			n = insn->inputs_count;
			for (j = n, p = insn->ops + 1; j > 0; p++, j--) {
				input = *p;
				if (!_xlat[input]) {
					/* input is not scheduled yet */
					if (input > 0) {
						if (_blocks[input] == b) {
							/* "input" should be before "i" to satisfy dependency */
#ifdef IR_DEBUG
							if (ctx->flags & IR_DEBUG_SCHEDULE) {
								fprintf(stderr, "Wrong dependency %d:%d -> %d\n", b, input, i);
							}
#endif
							/* remove "input" */
							_prev[_next[input]] = _prev[input];
							_next[_prev[input]] = _next[input];
							/* insert before "i" */
							_prev[input] = _prev[i];
							_next[input] = i;
							_next[_prev[i]] = input;
							_prev[i] = input;
							/* restart from "input" */
							i = input;
							insn = &ctx->ir_base[i];
							goto restart;
						}
					} else if (input < IR_TRUE) {
						consts_count += ir_count_constant(_xlat, input);
					}
				}
			}
			_xlat[i] = insns_count;
			insns_count += ir_insn_inputs_to_len(n);
			i = _next[i];
			insn = &ctx->ir_base[i];
		}
		/* Schedule BB end */
		_xlat[i] = bb->end = insns_count;
		insns_count++;
		if (IR_INPUT_EDGES_COUNT(ir_op_flags[insn->op]) == 2) {
			if (insn->op2 < IR_TRUE) {
				consts_count += ir_count_constant(_xlat, insn->op2);
			}
		}
	}

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_SCHEDULE) {
		fprintf(stderr, "After Schedule\n");
		for (i = 1; i != 0; i = _next[i]) {
			fprintf(stderr, "%d -> %d\n", i, _blocks[i]);
		}
	}
#endif

#if 1
	/* Check if scheduling didn't make any modifications */
	if (consts_count == ctx->consts_count && insns_count == ctx->insns_count) {
		bool changed = 0;

		for (i = 1; i != 0; i = _next[i]) {
			if (_xlat[i] != i) {
				changed = 1;
				break;
			}
		}
		if (!changed) {
			_xlat -= ctx->consts_count;
			ir_mem_free(_xlat);
			ir_mem_free(_next);

			ctx->prev_ref = _prev;
			ctx->flags2 |= IR_LINEAR;
			ir_truncate(ctx);

			return 1;
		}
	}
#endif

	ir_mem_free(_prev);

	ir_init(&new_ctx, ctx->flags, consts_count, insns_count);
	new_ctx.insns_count = insns_count;
	new_ctx.flags2 = ctx->flags2;
	new_ctx.ret_type = ctx->ret_type;
	new_ctx.mflags = ctx->mflags;
	new_ctx.spill_base = ctx->spill_base;
	new_ctx.fixed_stack_red_zone = ctx->fixed_stack_red_zone;
	new_ctx.fixed_stack_frame_size = ctx->fixed_stack_frame_size;
	new_ctx.fixed_call_stack_size = ctx->fixed_call_stack_size;
	new_ctx.fixed_regset = ctx->fixed_regset;
	new_ctx.fixed_save_regset = ctx->fixed_save_regset;
	new_ctx.entries_count = ctx->entries_count;
#if defined(IR_TARGET_AARCH64)
	new_ctx.deoptimization_exits = ctx->deoptimization_exits;
	new_ctx.get_exit_addr = ctx->get_exit_addr;
	new_ctx.get_veneer = ctx->get_veneer;
	new_ctx.set_veneer = ctx->set_veneer;
#endif
	new_ctx.loader = ctx->loader;

	/* Copy constants */
	if (consts_count == ctx->consts_count) {
		new_ctx.consts_count = consts_count;
		ref = 1 - consts_count;
		insn = &ctx->ir_base[ref];
		new_insn = &new_ctx.ir_base[ref];

		memcpy(new_insn, insn, sizeof(ir_insn) * (IR_TRUE - ref));
		if (ctx->strtab.data) {
			while (ref != IR_TRUE) {
				if (new_insn->op == IR_FUNC || new_insn->op == IR_SYM || new_insn->op == IR_STR) {
					new_insn->val.addr = ir_str(&new_ctx, ir_get_str(ctx, new_insn->val.i32));
				}
				new_insn++;
				ref++;
			}
		}
	} else {
		new_ref = -new_ctx.consts_count;
		new_insn = &new_ctx.ir_base[new_ref];
		for (ref = IR_TRUE - 1, insn = &ctx->ir_base[ref]; ref > -ctx->consts_count; insn--, ref--) {
			if (!_xlat[ref]) {
				continue;
			}
			new_insn->optx = insn->optx;
			new_insn->prev_const = 0;
			if (insn->op == IR_FUNC || insn->op == IR_SYM || insn->op == IR_STR) {
				new_insn->val.addr = ir_str(&new_ctx, ir_get_str(ctx, insn->val.i32));
			} else {
				new_insn->val.u64 = insn->val.u64;
			}
			_xlat[ref] = new_ref;
			new_ref--;
			new_insn--;
		}
		new_ctx.consts_count = -new_ref;
	}

	new_ctx.cfg_map = ir_mem_calloc(ctx->insns_count, sizeof(uint32_t));
	new_ctx.prev_ref = _prev = ir_mem_malloc(insns_count * sizeof(ir_ref));
	new_ctx.use_lists = lists = ir_mem_malloc(insns_count * sizeof(ir_use_list));
	new_ctx.use_edges = edges = ir_mem_malloc(ctx->use_edges_count * sizeof(ir_ref));

	/* Copy instructions, use lists and use edges */
	prev_ref = 0;
	use_edges_count = 0;
	for (i = 1; i != 0; i = _next[i]) {
		new_ref = _xlat[i];
		new_ctx.cfg_map[new_ref] = _blocks[i];
		_prev[new_ref] = prev_ref;
		prev_ref = new_ref;

		use_list = &ctx->use_lists[i];
		n = use_list->count;
		k = 0;
		if (n == 1) {
			ref = ctx->use_edges[use_list->refs];
			if (_xlat[ref]) {
				*edges = _xlat[ref];
				edges++;
				k = 1;
			}
		} else {
			p = &ctx->use_edges[use_list->refs];
			while (n--) {
				ref = *p;
				if (_xlat[ref]) {
					*edges = _xlat[ref];
					edges++;
					k++;
				}
				p++;
			}
		}
		new_list = &lists[new_ref];
		new_list->refs = use_edges_count;
		use_edges_count += k;
		new_list->count = k;

		insn = &ctx->ir_base[i];
		new_insn = &new_ctx.ir_base[new_ref];

		new_insn->optx = insn->optx;
		n = new_insn->inputs_count;
		switch (n) {
			case 0:
				new_insn->op1 = insn->op1;
				new_insn->op2 = insn->op2;
				new_insn->op3 = insn->op3;
				break;
			case 1:
				new_insn->op1 = _xlat[insn->op1];
				if (new_insn->op == IR_PARAM || insn->op == IR_VAR) {
					new_insn->op2 = ir_str(&new_ctx, ir_get_str(ctx, insn->op2));
				} else {
					new_insn->op2 = insn->op2;
				}
				new_insn->op3 = insn->op3;
				break;
			case 2:
				new_insn->op1 = _xlat[insn->op1];
				new_insn->op2 = _xlat[insn->op2];
				new_insn->op3 = insn->op3;
				break;
			case 3:
				new_insn->op1 = _xlat[insn->op1];
				new_insn->op2 = _xlat[insn->op2];
				new_insn->op3 = _xlat[insn->op3];
				break;
			default:
				for (j = n, p = insn->ops + 1, q = new_insn->ops + 1; j > 0; p++, q++, j--) {
					*q = _xlat[*p];
				}
				break;
		}
	}

	/* Update list of terminators (IR_OPND_CONTROL_REF) */
	insn = &new_ctx.ir_base[1];
	ref = insn->op1;
	if (ref) {
		insn->op1 = ref = _xlat[ref];
		while (1) {
			insn = &new_ctx.ir_base[ref];
			ref = insn->op3;
			if (!ref) {
				break;
			}
			insn->op3 = ref = _xlat[ref];
		}
	}

	IR_ASSERT(ctx->use_edges_count >= use_edges_count);
	new_ctx.use_edges_count = use_edges_count;
	new_ctx.use_edges = ir_mem_realloc(new_ctx.use_edges, use_edges_count * sizeof(ir_ref));

	if (ctx->binding) {
		ir_xlat_binding(ctx, _xlat);
		new_ctx.binding = ctx->binding;
		ctx->binding = NULL;
	}

	_xlat -= ctx->consts_count;
	ir_mem_free(_xlat);

	new_ctx.cfg_blocks_count = ctx->cfg_blocks_count;
	new_ctx.cfg_edges_count = ctx->cfg_edges_count;
	new_ctx.cfg_blocks = ctx->cfg_blocks;
	new_ctx.cfg_edges = ctx->cfg_edges;
	ctx->cfg_blocks = NULL;
	ctx->cfg_edges = NULL;

	ir_free(ctx);
	IR_ASSERT(new_ctx.consts_count == new_ctx.consts_limit);
	IR_ASSERT(new_ctx.insns_count == new_ctx.insns_limit);
	memcpy(ctx, &new_ctx, sizeof(ir_ctx));
	ctx->flags2 |= IR_LINEAR;

	ir_mem_free(_next);

	return 1;
}

void ir_build_prev_refs(ir_ctx *ctx)
{
	uint32_t b;
	ir_block *bb;
	ir_ref i, n, prev;
	ir_insn *insn;

	ctx->prev_ref = ir_mem_malloc(ctx->insns_count * sizeof(ir_ref));
	prev = 0;
	for (b = 1, bb = ctx->cfg_blocks + b; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		for (i = bb->start, insn = ctx->ir_base + i; i < bb->end;) {
			ctx->prev_ref[i] = prev;
			n = ir_insn_len(insn);
			prev = i;
			i += n;
			insn += n;
		}
		ctx->prev_ref[i] = prev;
	}
}
