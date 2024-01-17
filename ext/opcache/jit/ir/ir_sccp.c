/*
 * IR - Lightweight JIT Compilation Framework
 * (SCCP - Sparse Conditional Constant Propagation)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * The SCCP algorithm is based on M. N. Wegman and F. K. Zadeck publication
 * See:  M. N. Wegman and F. K. Zadeck. "Constant propagation with conditional branches"
 * ACM Transactions on Programming Languages and Systems, 13(2):181-210, April 1991
 */

#include "ir.h"
#include "ir_private.h"

#define IR_TOP                  IR_UNUSED
#define IR_BOTTOM               IR_LAST_OP

#define IR_MAKE_TOP(ref)        do {IR_ASSERT(ref > 0); _values[ref].optx = IR_TOP;} while (0)
#define IR_MAKE_BOTTOM(ref)     do {IR_ASSERT(ref > 0); _values[ref].optx = IR_BOTTOM;} while (0)

#define IR_IS_TOP(ref)          (ref >= 0 && _values[ref].optx == IR_TOP)
#define IR_IS_BOTTOM(ref)       (ref >= 0 && _values[ref].optx == IR_BOTTOM)
#define IR_IS_FEASIBLE(ref)     (ref >= 0 && _values[ref].optx != IR_TOP)

#define IR_COMBO_COPY_PROPAGATION 1

#if IR_COMBO_COPY_PROPAGATION
IR_ALWAYS_INLINE ir_ref ir_sccp_identity(ir_insn *_values, ir_ref a)
{
	if (a > 0 && _values[a].op == IR_COPY) {
		a = _values[a].op1;
		IR_ASSERT(a > 0 && _values[a].op != IR_COPY);
	}
	return a;
}
#endif

static ir_ref ir_sccp_fold(ir_ctx *ctx, ir_insn *_values, ir_ref res, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	ir_insn *op1_insn, *op2_insn, *op3_insn, *insn;

#if IR_COMBO_COPY_PROPAGATION
	op1 = ir_sccp_identity(_values, op1);
	op2 = ir_sccp_identity(_values, op2);
	op3 = ir_sccp_identity(_values, op3);
#endif

restart:
	op1_insn = (op1 > 0 && IR_IS_CONST_OP(_values[op1].op)) ? _values + op1 : ctx->ir_base + op1;
	op2_insn = (op2 > 0 && IR_IS_CONST_OP(_values[op2].op)) ? _values + op2 : ctx->ir_base + op2;
	op3_insn = (op3 > 0 && IR_IS_CONST_OP(_values[op3].op)) ? _values + op3 : ctx->ir_base + op3;

	switch (ir_folding(ctx, opt, op1, op2, op3, op1_insn, op2_insn, op3_insn)) {
		case IR_FOLD_DO_RESTART:
			opt = ctx->fold_insn.optx;
			op1 = ctx->fold_insn.op1;
			op2 = ctx->fold_insn.op2;
			op3 = ctx->fold_insn.op3;
			goto restart;
		case IR_FOLD_DO_EMIT:
			IR_MAKE_BOTTOM(res);
			return 1;
		case IR_FOLD_DO_COPY:
			op1 = ctx->fold_insn.op1;
#if IR_COMBO_COPY_PROPAGATION
			op1 = ir_sccp_identity(_values, op1);
#endif
			insn = (op1 > 0 && IR_IS_CONST_OP(_values[op1].op)) ? _values + op1 : ctx->ir_base + op1;
			if (IR_IS_CONST_OP(insn->op)) {
				/* pass */
#if IR_COMBO_COPY_PROPAGATION
			} else if (_values[res].optx == IR_TOP) {
				_values[res].optx = IR_OPT(IR_COPY, insn->type);
				_values[res].op1 = op1;
				return 1;
			} else if (_values[res].op == IR_COPY && _values[res].op1 == op1) {
				return 0; /* not changed */
			} else {
				IR_ASSERT(_values[res].optx != IR_BOTTOM);
				/* we don't check for widening */
				_values[res].optx = IR_OPT(IR_COPY, insn->type);
				_values[res].op1 = op1;
				return 1;
#else
			} else {
				IR_MAKE_BOTTOM(res);
				return 1;
#endif
			}
			break;
		case IR_FOLD_DO_CONST:
			insn = &ctx->fold_insn;
			break;
		default:
			IR_ASSERT(0);
			return 0;
	}

	if (IR_IS_TOP(res)) {
		_values[res].optx = IR_OPT(insn->type, insn->type);
		_values[res].val.u64 = insn->val.u64;
		return 1;
	} else if (_values[res].opt != IR_OPT(insn->type, insn->type) || _values[res].val.u64 != insn->val.u64) {
		IR_MAKE_BOTTOM(res);
		return 1;
	}
	return 0; /* not changed */
}

static bool ir_sccp_meet_phi(ir_ctx *ctx, ir_insn *_values, ir_ref i, ir_insn *insn, ir_bitqueue *worklist)
{
	ir_ref j, n, input, *merge_input, *p;
	ir_insn *v, *new_const = NULL;
#if IR_COMBO_COPY_PROPAGATION
	ir_ref new_copy;
#endif

	if (!IR_IS_FEASIBLE(insn->op1)) {
		return 0;
	}
	n = insn->inputs_count;
	if (n > 3 && _values[i].optx == IR_TOP) {
		for (j = 0; j < (n>>2); j++) {
			_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
		}
	}

	p = insn->ops + 2;
	merge_input = ctx->ir_base[insn->op1].ops + 1;
	for (; --n > 0; p++, merge_input++) {
		IR_ASSERT(*merge_input > 0);
		if (_values[*merge_input].optx == IR_TOP) {
			continue;
		}

		input = *p;
		if (IR_IS_CONST_REF(input)) {
			v = &ctx->ir_base[input];
		} else if (input == i) {
			continue;
		} else {
			v = &_values[input];
			if (v->op == IR_TOP) {
				/* do backward propagaton only once */
				if (!v->op1) {
					v->op1 = 1;
					ir_bitqueue_add(worklist, input);
				}
				continue;
#if IR_COMBO_COPY_PROPAGATION
			} else if (v->op == IR_COPY) {
				input = v->op1;
				IR_ASSERT(input > 0 && _values[input].op != IR_COPY);
				new_copy = input;
				goto next;
			} else if (v->op == IR_BOTTOM) {
				new_copy = input;
				goto next;
#else
			} else if (v->op == IR_BOTTOM) {
				IR_MAKE_BOTTOM(i);
				return 1;
#endif
			}
		}
		new_copy = IR_UNUSED;
		new_const = v;
		goto next;
	}

	IR_ASSERT(_values[i].optx == IR_TOP);
	return 0;

next:
	p++;
	merge_input++;
	/* for all live merge inputs */
	for (; --n > 0; p++, merge_input++) {
		IR_ASSERT(*merge_input > 0);
		if (_values[*merge_input].optx == IR_TOP) {
			continue;
		}

		input = *p;
		if (IR_IS_CONST_REF(input)) {
			v = &ctx->ir_base[input];
		} else if (input == i) {
			continue;
		} else {
			v = &_values[input];
			if (v->op == IR_TOP) {
				/* do backward propagaton only once */
				if (!v->op1) {
					v->op1 = 1;
					ir_bitqueue_add(worklist, input);
				}
				continue;
#if IR_COMBO_COPY_PROPAGATION
			} else if (v->op == IR_COPY) {
				input = v->op1;
				IR_ASSERT(input > 0 && _values[input].op != IR_COPY);
				if (new_copy == input) {
					continue;
				} else {
					IR_MAKE_BOTTOM(i);
					return 1;
				}
			} else if (v->op == IR_BOTTOM) {
				if (new_copy == input) {
					continue;
				} else {
					IR_MAKE_BOTTOM(i);
					return 1;
				}
#else
			} else if (v->op == IR_BOTTOM) {
				IR_MAKE_BOTTOM(i);
				return 1;
#endif
			}
		}
		if (!new_const || new_const->opt != v->opt || new_const->val.u64 != v->val.u64) {
			IR_MAKE_BOTTOM(i);
			return 1;
		}
	}

#if IR_COMBO_COPY_PROPAGATION
	if (new_copy) {
		if (_values[i].op == IR_COPY && _values[i].op1 == new_copy) {
			return 0; /* not changed */
		} else {
			IR_ASSERT(_values[i].optx != IR_BOTTOM);
			/* we don't check for widening */
			_values[i].optx = IR_OPT(IR_COPY, ctx->ir_base[new_copy].type);
			_values[i].op1 = new_copy;
			return 1;
		}
	}
#endif

	if (_values[i].optx == IR_TOP) {
		_values[i].optx = new_const->opt;
		_values[i].val.u64 = new_const->val.u64;
		return 1;
	} else if (_values[i].opt == new_const->opt && _values[i].val.u64 == new_const->val.u64) {
		return 0;
	} else {
		IR_MAKE_BOTTOM(i);
		return 1;
	}
}

static bool ir_sccp_is_true(ir_ctx *ctx, ir_insn *_values, ir_ref a)
{
	ir_insn *v = IR_IS_CONST_REF(a) ? &ctx->ir_base[a] : &_values[a];

	return ir_const_is_true(v);
}

static bool ir_sccp_is_equal(ir_ctx *ctx, ir_insn *_values, ir_ref a, ir_ref b)
{
	ir_insn *v1 = IR_IS_CONST_REF(a) ? &ctx->ir_base[a] : &_values[a];
	ir_insn *v2 = IR_IS_CONST_REF(b) ? &ctx->ir_base[b] : &_values[b];

	return v1->val.u64 == v2->val.u64;
}

static void ir_sccp_remove_from_use_list(ir_ctx *ctx, ir_ref from, ir_ref ref)
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
#if IR_COMBO_COPY_PROPAGATION
	if (skip) {
		do {
			*q = IR_UNUSED;
			q++;
		} while (--skip);
	}
#endif
}

static void ir_sccp_remove_from_use_list_1(ir_ctx *ctx, ir_ref from, ir_ref ref)
{
	ir_ref j, n, *p;
	ir_use_list *use_list = &ctx->use_lists[from];

	n = use_list->count;
	j = 0;
	p = &ctx->use_edges[use_list->refs];
	while (j < n) {
		if (*p == ref) {
			break;
		}
		j++;
	}

	if (j < n) {
		use_list->count--;
		j++;
		while (j < n) {
			*p = *(p+1);
			p++;
			j++;
		}
#if IR_COMBO_COPY_PROPAGATION
		*p = IR_UNUSED;
#endif
	}
}

#if IR_COMBO_COPY_PROPAGATION
static int ir_sccp_add_to_use_list(ir_ctx *ctx, ir_ref to, ir_ref ref)
{
	ir_use_list *use_list = &ctx->use_lists[to];
	ir_ref n = use_list->refs + use_list->count;

	if (n < ctx->use_edges_count && ctx->use_edges[n] == IR_UNUSED) {
		ctx->use_edges[n] = ref;
		use_list->count++;
		return 0;
	} else {
		/* Reallocate the whole edges buffer (this is inefficient) */
		ctx->use_edges = ir_mem_realloc(ctx->use_edges, (ctx->use_edges_count + use_list->count + 1) * sizeof(ir_ref));
		memcpy(ctx->use_edges + ctx->use_edges_count, ctx->use_edges + use_list->refs, use_list->count * sizeof(ir_ref));
		use_list->refs = ctx->use_edges_count;
		ctx->use_edges[use_list->refs + use_list->count] = ref;
		use_list->count++;
		ctx->use_edges_count += use_list->count;
		return 1;
	}
}
#endif

static void ir_sccp_make_nop(ir_ctx *ctx, ir_ref ref)
{
	ir_ref j, n, *p;
	ir_use_list *use_list = &ctx->use_lists[ref];
	ir_insn *insn;

	use_list->refs = 0;
	use_list->count = 0;

	insn = &ctx->ir_base[ref];
	n = insn->inputs_count;
	insn->opt = IR_NOP; /* keep "inputs_count" */
	for (j = 1, p = insn->ops + j; j <= n; j++, p++) {
		*p = IR_UNUSED;
	}
}

static void ir_sccp_remove_insn(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_bitqueue *worklist)
{
	ir_ref j, n, *p;
	ir_use_list *use_list = &ctx->use_lists[ref];
	ir_insn *insn;

	use_list->refs = 0;
	use_list->count = 0;

	insn = &ctx->ir_base[ref];
	n = insn->inputs_count;
	insn->opt = IR_NOP; /* keep "inputs_count" */
	for (j = 1, p = insn->ops + j; j <= n; j++, p++) {
		ir_ref input = *p;
		*p = IR_UNUSED;
		if (input > 0 && _values[input].op == IR_BOTTOM) {
			ir_sccp_remove_from_use_list(ctx, input, ref);
			/* schedule DCE */
			if ((IR_IS_FOLDABLE_OP(ctx->ir_base[input].op) && ctx->use_lists[input].count == 0)
			 || ((ir_op_flags[ctx->ir_base[input].op] & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)
					&& ctx->use_lists[input].count == 1)) {
				ir_bitqueue_add(worklist, input);
			}
		}
	}
}

static void ir_sccp_replace_insn(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_ref new_ref, ir_bitqueue *worklist)
{
	ir_ref j, n, *p, use, k, l;
	ir_insn *insn;
	ir_use_list *use_list;

	IR_ASSERT(ref != new_ref);

	insn = &ctx->ir_base[ref];
	n = insn->inputs_count;
	insn->opt = IR_NOP; /* keep "inputs_count" */
	for (j = 1, p = insn->ops + 1; j <= n; j++, p++) {
		ir_ref input = *p;
		*p = IR_UNUSED;
		if (input > 0) {
			ir_sccp_remove_from_use_list(ctx, input, ref);
			/* schedule DCE */
			if (worklist
			 && ((IR_IS_FOLDABLE_OP(ctx->ir_base[input].op) && ctx->use_lists[input].count == 0)
			  || ((ir_op_flags[ctx->ir_base[input].op] & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)
					&& ctx->use_lists[input].count == 1))) {
				ir_bitqueue_add(worklist, input);
			}
		}
	}

	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
		use = *p;
		if (IR_IS_FEASIBLE(use)) {
			insn = &ctx->ir_base[use];
			l = insn->inputs_count;
			for (k = 1; k <= l; k++) {
				if (ir_insn_op(insn, k) == ref) {
					ir_insn_set_op(insn, k, new_ref);
				}
			}
#if IR_COMBO_COPY_PROPAGATION
			if (new_ref > 0 && IR_IS_BOTTOM(use)) {
				if (ir_sccp_add_to_use_list(ctx, new_ref, use)) {
					/* restore after reallocation */
					use_list = &ctx->use_lists[ref];
					n = use_list->count;
					p = &ctx->use_edges[use_list->refs + j];
				}
			}
#endif
			/* schedule folding */
			if (worklist && _values[use].op == IR_BOTTOM) {
				ir_bitqueue_add(worklist, use);
			}
		}
	}

	use_list->refs = 0;
	use_list->count = 0;
}

static void ir_sccp_fold2(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_bitqueue *worklist)
{
	uint32_t opt;
	ir_ref op1, op2, op3;
	ir_insn *op1_insn, *op2_insn, *op3_insn, *insn;

	insn = &ctx->ir_base[ref];
	opt = insn->opt;
	op1 = insn->op1;
	op2 = insn->op2;
	op3 = insn->op3;

restart:
	op1_insn = ctx->ir_base + op1;
	op2_insn = ctx->ir_base + op2;
	op3_insn = ctx->ir_base + op3;

	switch (ir_folding(ctx, opt, op1, op2, op3, op1_insn, op2_insn, op3_insn)) {
		case IR_FOLD_DO_RESTART:
			opt = ctx->fold_insn.optx;
			op1 = ctx->fold_insn.op1;
			op2 = ctx->fold_insn.op2;
			op3 = ctx->fold_insn.op3;
			goto restart;
		case IR_FOLD_DO_EMIT:
			insn = &ctx->ir_base[ref];
			if (insn->opt != ctx->fold_insn.opt
			 || insn->op1 != ctx->fold_insn.op1
			 || insn->op2 != ctx->fold_insn.op2
			 || insn->op3 != ctx->fold_insn.op3) {

				ir_use_list *use_list;
				ir_ref n, j, *p, use;

				insn->optx = ctx->fold_insn.opt;
				IR_ASSERT(!IR_OP_HAS_VAR_INPUTS(ir_op_flags[opt & IR_OPT_OP_MASK]));
				insn->inputs_count = IR_INPUT_EDGES_COUNT(ir_op_flags[opt & IR_OPT_OP_MASK]);
				if (insn->op1 != ctx->fold_insn.op1) {
					if (!IR_IS_CONST_REF(insn->op1) && insn->op1 != ctx->fold_insn.op2 && insn->op1 != ctx->fold_insn.op3) {
						ir_sccp_remove_from_use_list(ctx, insn->op1, ref);
					}
					if (!IR_IS_CONST_REF(ctx->fold_insn.op1) && ctx->fold_insn.op1 != insn->op2 && ctx->fold_insn.op1 != insn->op3) {
						ir_sccp_add_to_use_list(ctx, ctx->fold_insn.op1, ref);
					}
				}
				if (insn->op2 != ctx->fold_insn.op2) {
					if (!IR_IS_CONST_REF(insn->op2) && insn->op2 != ctx->fold_insn.op1 && insn->op2 != ctx->fold_insn.op3) {
						ir_sccp_remove_from_use_list(ctx, insn->op2, ref);
					}
					if (!IR_IS_CONST_REF(ctx->fold_insn.op2) && ctx->fold_insn.op2 != insn->op1 && ctx->fold_insn.op2 != insn->op3) {
						ir_sccp_add_to_use_list(ctx, ctx->fold_insn.op2, ref);
					}
				}
				if (insn->op3 != ctx->fold_insn.op3) {
					if (!IR_IS_CONST_REF(insn->op3) && insn->op3 != ctx->fold_insn.op1 && insn->op3 != ctx->fold_insn.op2) {
						ir_sccp_remove_from_use_list(ctx, insn->op3, ref);
					}
					if (!IR_IS_CONST_REF(ctx->fold_insn.op3) && ctx->fold_insn.op3 != insn->op1 && ctx->fold_insn.op3 != insn->op2) {
						ir_sccp_add_to_use_list(ctx, ctx->fold_insn.op3, ref);
					}
				}
				insn->op1 = ctx->fold_insn.op1;
				insn->op2 = ctx->fold_insn.op2;
				insn->op3 = ctx->fold_insn.op3;

				use_list = &ctx->use_lists[ref];
				n = use_list->count;
				for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
					use = *p;
					if (_values[use].op == IR_BOTTOM) {
						ir_bitqueue_add(worklist, use);
					}
				}
			}
			break;
		case IR_FOLD_DO_COPY:
			op1 = ctx->fold_insn.op1;
			ir_sccp_replace_insn(ctx, _values, ref, op1, worklist);
			break;
		case IR_FOLD_DO_CONST:
			op1 = ir_const(ctx, ctx->fold_insn.val, ctx->fold_insn.type);
			ir_sccp_replace_insn(ctx, _values, ref, op1, worklist);
			break;
		default:
			IR_ASSERT(0);
			break;
	}
}

static void ir_sccp_replace_use(ir_ctx *ctx, ir_ref ref, ir_ref use, ir_ref new_use)
{
	ir_use_list *use_list = &ctx->use_lists[ref];
	ir_ref i, n, *p;

	n = use_list->count;
	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < n; i++, p++) {
		if (*p == use) {
			*p = new_use;
			break;
		}
	}
}

static void ir_sccp_remove_if(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_ref dst)
{
	ir_ref j, n, *p, use, next;
	ir_insn *insn, *next_insn;
	ir_use_list *use_list = &ctx->use_lists[ref];

	insn = &ctx->ir_base[ref];
	n = use_list->count;
	for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
		use = *p;
		if (use == dst) {
			next = ctx->use_edges[ctx->use_lists[use].refs];
			next_insn = &ctx->ir_base[next];
			/* remove IF and IF_TRUE/FALSE from double linked control list */
			next_insn->op1 = insn->op1;
			ir_sccp_replace_use(ctx, insn->op1, ref, next);
			/* remove IF and IF_TRUE/FALSE instructions */
			ir_sccp_make_nop(ctx, ref);
			ir_sccp_make_nop(ctx, use);
			break;
		}
	}
}

static void ir_sccp_remove_unfeasible_merge_inputs(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_ref unfeasible_inputs)
{
	ir_ref i, j, n, k, *p, use;
	ir_insn *insn, *use_insn;
	ir_use_list *use_list;
	ir_bitset life_inputs;

	insn = &ctx->ir_base[ref];
	IR_ASSERT(insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN);
	n = insn->inputs_count;
	if (n - unfeasible_inputs == 1) {
		/* remove MERGE completely */
		for (j = 1; j <= n; j++) {
			ir_ref input = ir_insn_op(insn, j);
			if (input && IR_IS_FEASIBLE(input)) {
				ir_insn *input_insn = &ctx->ir_base[input];

				IR_ASSERT(input_insn->op == IR_END || input_insn->op == IR_LOOP_END||
					input_insn->op == IR_IJMP || input_insn->op == IR_UNREACHABLE);
				if (input_insn->op == IR_END || input_insn->op == IR_LOOP_END) {
					if (input < ref) {
						ir_ref prev, next = IR_UNUSED;
						ir_insn *next_insn = NULL;

						prev = input_insn->op1;
						use_list = &ctx->use_lists[ref];
						for (k = 0, p = &ctx->use_edges[use_list->refs]; k < use_list->count; k++, p++) {
							use = *p;
							use_insn = &ctx->ir_base[use];
							IR_ASSERT((use_insn->op != IR_PHI) && "PHI must be already removed");
							if (ir_op_flags[use_insn->op] & IR_OP_FLAG_CONTROL) {
								next = use;
								next_insn = use_insn;
								break;
							}
						}
						IR_ASSERT(prev && next);
						/* remove MERGE and input END from double linked control list */
						next_insn->op1 = prev;
						ir_sccp_replace_use(ctx, prev, input, next);
						/* remove MERGE and input END instructions */
						ir_sccp_make_nop(ctx, ref);
						ir_sccp_make_nop(ctx, input);
					} else {
						for (i = 2; i <= n; i++) {
							ir_insn_set_op(insn, i, IR_UNUSED);
						}
						insn->op = IR_BEGIN;
						insn->op1 = input;
						input_insn->op = IR_END;
					}
					break;
				} else {
					for (i = 2; i <= n; i++) {
						ir_insn_set_op(insn, i, IR_UNUSED);
					}
					insn->op = IR_BEGIN;
					insn->op1 = input;
				}
			}
		}
	} else {
		n = insn->inputs_count;
		i = 1;
		life_inputs = ir_bitset_malloc(n + 1);
		for (j = 1; j <= n; j++) {
			ir_ref input = ir_insn_op(insn, j);

			if (input) {
				if (i != j) {
					ir_insn_set_op(insn, i, input);
				}
				ir_bitset_incl(life_inputs, j);
				i++;
			}
		}
		j = i;
		while (j < n) {
			ir_insn_set_op(insn, j, IR_UNUSED);
			j++;
		}
		i--;
		insn->inputs_count = i;

		n++;
		use_list = &ctx->use_lists[ref];
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
						} else if (!IR_IS_CONST_REF(input)) {
							ir_sccp_remove_from_use_list_1(ctx, input, use);
						}
					}
					while (i <= n) {
						ir_insn_set_op(use_insn, i, IR_UNUSED);
						i++;
					}
					use_insn->inputs_count = insn->inputs_count + 1;
				}
			}
		}
		ir_mem_free(life_inputs);
	}
}

int ir_sccp(ir_ctx *ctx)
{
	ir_ref i, j, n, *p, use;
	ir_use_list *use_list;
	ir_insn *insn, *use_insn, *value;
	uint32_t flags;
	ir_bitqueue worklist;
	ir_insn *_values = ir_mem_calloc(ctx->insns_count, sizeof(ir_insn));

	ctx->flags2 |= IR_OPT_IN_SCCP;

	/* A bit modified SCCP algorithm of M. N. Wegman and F. K. Zadeck */
	ir_bitqueue_init(&worklist, ctx->insns_count);
	worklist.pos = 0;
	ir_bitset_incl(worklist.set, 1);
	while ((i = ir_bitqueue_pop(&worklist)) >= 0) {
		insn = &ctx->ir_base[i];
		flags = ir_op_flags[insn->op];
		if (flags & IR_OP_FLAG_DATA) {
			if (insn->op == IR_PHI) {
				if (!ir_sccp_meet_phi(ctx, _values, i, insn, &worklist)) {
					continue;
				}
			} else if (ctx->use_lists[i].count == 0) {
				/* dead code */
				continue;
			} else if (EXPECTED(IR_IS_FOLDABLE_OP(insn->op))) {
				bool may_benefit = 0;
				bool has_top = 0;

				IR_ASSERT(!IR_OP_HAS_VAR_INPUTS(flags));
				n = IR_INPUT_EDGES_COUNT(flags);
				for (p = insn->ops + 1; n > 0; p++, n--) {
					ir_ref input = *p;
					if (input > 0) {
						if (_values[input].optx == IR_TOP) {
							has_top = 1;
							/* do backward propagaton only once */
							if (!_values[input].op1) {
								_values[input].op1 = 1;
								ir_bitqueue_add(&worklist, input);
							}
						} else if (_values[input].optx != IR_BOTTOM) {
							/* Perform folding only if some of direct inputs
							 * is going to be replaced by a constant or copy.
							 * This approach may miss some folding optimizations
							 * dependent on indirect inputs. e.g. reassociation.
							 */
							may_benefit = 1;
						}
					}
				}
				if (has_top) {
					continue;
				}
				if (!may_benefit) {
					IR_MAKE_BOTTOM(i);
				} else if (!ir_sccp_fold(ctx, _values, i, insn->opt, insn->op1, insn->op2, insn->op3)) {
					/* not changed */
					continue;
				}
			} else {
				IR_MAKE_BOTTOM(i);
			}
		} else if (flags & IR_OP_FLAG_BB_START) {
			if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				ir_ref unfeasible_inputs = 0;

				n = insn->inputs_count;
				if (n > 3 && _values[i].optx == IR_TOP) {
					for (j = 0; j < (n>>2); j++) {
						_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
					}
				}
				for (p = insn->ops + 1; n > 0; p++, n--) {
					ir_ref input = *p;
					IR_ASSERT(input > 0);
					if (_values[input].optx == IR_TOP) {
						unfeasible_inputs++;
					}
				}
				if (unfeasible_inputs == 0) {
					IR_MAKE_BOTTOM(i);
				} else if (_values[i].op1 != unfeasible_inputs) {
					_values[i].optx = IR_MERGE;
					_values[i].op1 = unfeasible_inputs;
				} else {
					continue;
				}
			} else {
				IR_ASSERT(insn->op == IR_START || IR_IS_FEASIBLE(insn->op1));
				IR_MAKE_BOTTOM(i);
			}
		} else {
			IR_ASSERT(insn->op1 > 0);
			if (_values[insn->op1].optx == IR_TOP) {
				/* control inpt is not feasible */
				continue;
			}
			if (insn->op == IR_IF) {
				if (IR_IS_TOP(insn->op2)) {
					/* do backward propagaton only once */
					if (!_values[insn->op2].op1) {
						_values[insn->op2].op1 = 1;
						ir_bitqueue_add(&worklist, insn->op2);
					}
					continue;
				}
				if (!IR_IS_BOTTOM(insn->op2)
#if IR_COMBO_COPY_PROPAGATION
				 && (IR_IS_CONST_REF(insn->op2) || _values[insn->op2].op != IR_COPY)
#endif
				) {
					bool b = ir_sccp_is_true(ctx, _values, insn->op2);
					use_list = &ctx->use_lists[i];
					IR_ASSERT(use_list->count == 2);
					p = &ctx->use_edges[use_list->refs];
					use = *p;
					use_insn = &ctx->ir_base[use];
					IR_ASSERT(use_insn->op == IR_IF_TRUE || use_insn->op == IR_IF_FALSE);
					if ((use_insn->op == IR_IF_TRUE) != b) {
						use = *(p+1);
						IR_ASSERT(ctx->ir_base[use].op == IR_IF_TRUE || ctx->ir_base[use].op == IR_IF_FALSE);
					}
					if (_values[i].optx == IR_TOP) {
						_values[i].optx = IR_IF;
						_values[i].op1 = use;
					} else if (_values[i].optx != IR_IF || _values[i].op1 != use) {
						IR_MAKE_BOTTOM(i);
					}
					if (!IR_IS_BOTTOM(use)) {
						ir_bitqueue_add(&worklist, use);
					}
					continue;
				}
				IR_MAKE_BOTTOM(i);
			} else if (insn->op == IR_SWITCH) {
				if (IR_IS_TOP(insn->op2)) {
					/* do backward propagaton only once */
					if (!_values[insn->op2].op1) {
						_values[insn->op2].op1 = 1;
						ir_bitqueue_add(&worklist, insn->op2);
					}
					continue;
				}
				if (!IR_IS_BOTTOM(insn->op2)
#if IR_COMBO_COPY_PROPAGATION
				 && (IR_IS_CONST_REF(insn->op2) || _values[insn->op2].op != IR_COPY)
#endif
				) {
					ir_ref use_case = IR_UNUSED;

					use_list = &ctx->use_lists[i];
					n = use_list->count;
					for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
						use = *p;
						IR_ASSERT(use > 0);
						use_insn = &ctx->ir_base[use];
						if (use_insn->op == IR_CASE_VAL) {
							if (ir_sccp_is_equal(ctx, _values, insn->op2, use_insn->op2)) {
								use_case = use;
								break;
							}
						} else if (use_insn->op == IR_CASE_DEFAULT) {
							use_case = use;
						}
					}
					if (use_case) {
						use_insn = &ctx->ir_base[use_case];
						if (_values[i].optx == IR_TOP) {
							_values[i].optx = IR_IF;
							_values[i].op1 = use_case;
						} else if (_values[i].optx != IR_IF || _values[i].op1 != use_case) {
							IR_MAKE_BOTTOM(i);
						}
						if (!IR_IS_BOTTOM(use_case)) {
							ir_bitqueue_add(&worklist, use_case);
						}
					}
					if (!IR_IS_BOTTOM(i)) {
						continue;
					}
				}
				IR_MAKE_BOTTOM(i);
			} else if ((flags & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)
					&& ctx->use_lists[i].count == 1) {
				/* dead load */
				_values[i].optx = IR_LOAD;
			} else {
				IR_MAKE_BOTTOM(i);

				/* control, call, load and store instructions may have unprocessed inputs */
				n = IR_INPUT_EDGES_COUNT(flags);
				if (IR_OP_HAS_VAR_INPUTS(flags) && (n = insn->inputs_count) > 3) {
					for (j = 0; j < (n>>2); j++) {
						_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
					}
					for (j = 2, p = insn->ops + j; j <= n; j++, p++) {
						IR_ASSERT(IR_OPND_KIND(flags, j) == IR_OPND_DATA);
						use = *p;
						if (use > 0 && UNEXPECTED(_values[use].optx == IR_TOP)) {
							ir_bitqueue_add(&worklist, use);
						}
					}
				} else if (n >= 2) {
					IR_ASSERT(IR_OPND_KIND(flags, 2) == IR_OPND_DATA);
					use = insn->op2;
					if (use > 0 && UNEXPECTED(_values[use].optx == IR_TOP)) {
						ir_bitqueue_add(&worklist, use);
					}
					if (n > 2) {
						IR_ASSERT(n == 3);
						IR_ASSERT(IR_OPND_KIND(flags, 3) == IR_OPND_DATA);
						use = insn->op3;
						if (use > 0 && UNEXPECTED(_values[use].optx == IR_TOP)) {
							ir_bitqueue_add(&worklist, use);
						}
					}
				}
			}
		}
		use_list = &ctx->use_lists[i];
		n = use_list->count;
		for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
			use = *p;
			if (_values[use].optx != IR_BOTTOM) {
				ir_bitqueue_add(&worklist, use);
			}
		}
	}

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_SCCP) {
		for (i = 1; i < ctx->insns_count; i++) {
			if (IR_IS_CONST_OP(_values[i].op)) {
				fprintf(stderr, "%d. CONST(", i);
				ir_print_const(ctx, &_values[i], stderr, true);
				fprintf(stderr, ")\n");
#if IR_COMBO_COPY_PROPAGATION
			} else if (_values[i].op == IR_COPY) {
				fprintf(stderr, "%d. COPY(%d)\n", i, _values[i].op1);
#endif
			} else if (IR_IS_TOP(i)) {
				fprintf(stderr, "%d. TOP\n", i);
			} else if (_values[i].op == IR_IF) {
				fprintf(stderr, "%d. IF(%d)\n", i, _values[i].op1);
			} else if (_values[i].op == IR_MERGE) {
				fprintf(stderr, "%d. MERGE(%d)\n", i, _values[i].op1);
			} else if (!IR_IS_BOTTOM(i)) {
				fprintf(stderr, "%d. %d\n", i, _values[i].op);
			}
		}
    }
#endif

	for (i = 1, value = _values + i; i < ctx->insns_count; value++, i++) {
		if (value->op == IR_BOTTOM) {
			continue;
		} else if (IR_IS_CONST_OP(value->op)) {
			/* replace instruction by constant */
			j = ir_const(ctx, value->val, value->type);
			ir_sccp_replace_insn(ctx, _values, i, j, &worklist);
#if IR_COMBO_COPY_PROPAGATION
		} else if (value->op == IR_COPY) {
			ir_sccp_replace_insn(ctx, _values, i, value->op1, &worklist);
#endif
		} else if (value->op == IR_TOP) {
			/* remove unreachable instruction */
			insn = &ctx->ir_base[i];
			if (ir_op_flags[insn->op] & (IR_OP_FLAG_DATA|IR_OP_FLAG_MEM)) {
				if (insn->op != IR_PARAM && insn->op != IR_VAR) {
					ir_sccp_remove_insn(ctx, _values, i, &worklist);
				}
			} else {
				if (ir_op_flags[insn->op] & IR_OP_FLAG_TERMINATOR) {
					/* remove from terminators list */
					ir_ref prev = ctx->ir_base[1].op1;
					if (prev == i) {
						ctx->ir_base[1].op1 = insn->op3;
					} else {
						while (prev) {
							if (ctx->ir_base[prev].op3 == i) {
								ctx->ir_base[prev].op3 = insn->op3;
								break;
							}
							prev = ctx->ir_base[prev].op3;
						}
					}
				}
				ir_sccp_replace_insn(ctx, _values, i, IR_UNUSED, NULL);
			}
		} else if (value->op == IR_IF) {
			/* remove one way IF/SWITCH */
			ir_sccp_remove_if(ctx, _values, i, value->op1);
		} else if (value->op == IR_MERGE) {
			/* schedule merge to remove unfeasible MERGE inputs */
			ir_bitqueue_add(&worklist, i);
		} else if (value->op == IR_LOAD) {
			/* schedule dead load elimination */
			ir_bitqueue_add(&worklist, i);
		}
	}

	while ((i = ir_bitqueue_pop(&worklist)) >= 0) {
		if (_values[i].op == IR_MERGE) {
			ir_sccp_remove_unfeasible_merge_inputs(ctx, _values, i, _values[i].op1);
		} else {
			insn = &ctx->ir_base[i];
			if (IR_IS_FOLDABLE_OP(insn->op)) {
				if (ctx->use_lists[i].count == 0) {
					ir_sccp_remove_insn(ctx, _values, i, &worklist);
				} else {
					ir_sccp_fold2(ctx, _values, i, &worklist);
				}
			} else if ((ir_op_flags[insn->op] & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)
					&& ctx->use_lists[i].count == 1) {
				/* dead load */
				ir_ref next = ctx->use_edges[ctx->use_lists[i].refs];

				/* remove LOAD from double linked control list */
				ctx->ir_base[next].op1 = insn->op1;
				ir_sccp_replace_use(ctx, insn->op1, i, next);
				insn->op1 = IR_UNUSED;
				ir_sccp_remove_insn(ctx, _values, i, &worklist);
			}
		}
	}

	ir_mem_free(_values);
	ir_bitqueue_free(&worklist);

	ctx->flags2 &= ~IR_OPT_IN_SCCP;
	ctx->flags2 |= IR_SCCP_DONE;

	return 1;
}
