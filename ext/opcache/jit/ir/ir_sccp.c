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
		IR_ASSERT(a < 0 || _values[a].op != IR_COPY); /* this may be a copy of symbolic constant */
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
				IR_ASSERT(input < 0 || _values[input].op != IR_COPY);
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
				IR_ASSERT(input < 0 || _values[input].op != IR_COPY);
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

static bool ir_is_dead_load_ex(ir_ctx *ctx, ir_ref ref, uint32_t flags, ir_insn *insn)
{
	if ((flags & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)) {
		return ctx->use_lists[ref].count == 1;
	} else if (insn->op == IR_ALLOCA) {
		return ctx->use_lists[ref].count == 1;
	}
	return 0;
}

static bool ir_is_dead_load(ir_ctx *ctx, ir_ref ref)
{
	if (ctx->use_lists[ref].count == 1) {
		uint32_t flags = ir_op_flags[ctx->ir_base[ref].op];

		if ((flags & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)) {
			return 1;
		} else if (ctx->ir_base[ref].op == IR_ALLOCA) {
			return 1;
		}
	}
	return 0;
}

static bool ir_is_dead(ir_ctx *ctx, ir_ref ref)
{
	if (ctx->use_lists[ref].count == 0) {
		return IR_IS_FOLDABLE_OP(ctx->ir_base[ref].op);
	} else {
		return ir_is_dead_load(ctx, ref);
	}
	return 0;
}

static ir_ref ir_find1(ir_ctx *ctx, uint32_t optx, ir_ref op1)
{
	IR_ASSERT(!IR_IS_CONST_REF(op1));

	ir_use_list *use_list = &ctx->use_lists[op1];
	ir_ref *p, n = use_list->count;

	for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
		ir_ref use = *p;
		ir_insn *use_insn = &ctx->ir_base[use];

		if (use_insn->optx == optx) {
			IR_ASSERT(use_insn->op1 == op1);
			return use;
		}
	}
	return IR_UNUSED;
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

	IR_ASSERT(!IR_IS_SYM_CONST(v1->op));
	IR_ASSERT(!IR_IS_SYM_CONST(v2->op));
	return v1->val.u64 == v2->val.u64;
}

static void ir_sccp_make_nop(ir_ctx *ctx, ir_ref ref)
{
	ir_ref j, n, *p;
	ir_insn *insn;

	CLEAR_USES(ref);
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
	ir_insn *insn;

	CLEAR_USES(ref);
	insn = &ctx->ir_base[ref];
	n = insn->inputs_count;
	insn->opt = IR_NOP; /* keep "inputs_count" */
	for (j = 1, p = insn->ops + j; j <= n; j++, p++) {
		ir_ref input = *p;
		*p = IR_UNUSED;
		/* we may skip nodes that are going to be removed by SCCP (TOP, CONST and COPY) */
		if (input > 0 && _values[input].op > IR_COPY) {
			ir_use_list_remove_all(ctx, input, ref);
			if (ir_is_dead(ctx, input)) {
				/* schedule DCE */
				ir_bitqueue_add(worklist, input);
			}
		}
	}
}

static void ir_sccp_remove_insn2(ir_ctx *ctx, ir_ref ref, ir_bitqueue *worklist)
{
	ir_ref j, n, *p;
	ir_insn *insn;

	CLEAR_USES(ref);
	insn = &ctx->ir_base[ref];
	n = insn->inputs_count;
	insn->opt = IR_NOP; /* keep "inputs_count" */
	for (j = 1, p = insn->ops + j; j <= n; j++, p++) {
		ir_ref input = *p;
		*p = IR_UNUSED;
		if (input > 0) {
			ir_use_list_remove_all(ctx, input, ref);
			if (ir_is_dead(ctx, input)) {
				/* schedule DCE */
				ir_bitqueue_add(worklist, input);
			} else if (ctx->ir_base[input].op == IR_PHI && ctx->use_lists[input].count == 1) {
				/* try to optimize PHI into ABS/MIN/MAX/COND */
				ir_bitqueue_add(worklist, ctx->ir_base[input].op1);
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
		/* we may skip nodes that are going to be removed by SCCP (TOP, CONST and COPY) */
		if (input > 0 && _values[input].op > IR_COPY) {
			ir_use_list_remove_all(ctx, input, ref);
			if (ir_is_dead(ctx, input)) {
				/* schedule DCE */
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
				if (ir_use_list_add(ctx, new_ref, use)) {
					/* restore after reallocation */
					use_list = &ctx->use_lists[ref];
					n = use_list->count;
					p = &ctx->use_edges[use_list->refs + j];
				}
			}
#endif
			/* we may skip nodes that are going to be removed by SCCP (TOP, CONST and COPY) */
			if (worklist && _values[use].op > IR_COPY) {
				/* schedule folding */
				ir_bitqueue_add(worklist, use);
			}
		}
	}

	CLEAR_USES(ref);
}

static void ir_sccp_replace_insn2(ir_ctx *ctx, ir_ref ref, ir_ref new_ref, ir_bitqueue *worklist)
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
			ir_use_list_remove_all(ctx, input, ref);
			if (ir_is_dead(ctx, input)) {
				/* schedule DCE */
				ir_bitqueue_add(worklist, input);
			} else if (ctx->ir_base[input].op == IR_PHI && ctx->use_lists[input].count == 1) {
				/* try to optimize PHI into ABS/MIN/MAX/COND */
				ir_bitqueue_add(worklist, input);
			}
		}
	}

	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
		use = *p;
		insn = &ctx->ir_base[use];
		l = insn->inputs_count;
		for (k = 1; k <= l; k++) {
			if (ir_insn_op(insn, k) == ref) {
				ir_insn_set_op(insn, k, new_ref);
			}
		}
#if IR_COMBO_COPY_PROPAGATION
		if (new_ref > 0) {
			if (ir_use_list_add(ctx, new_ref, use)) {
				/* restore after reallocation */
				use_list = &ctx->use_lists[ref];
				n = use_list->count;
				p = &ctx->use_edges[use_list->refs + j];
			}
		}
#endif
		/* schedule folding */
		ir_bitqueue_add(worklist, use);
	}

	CLEAR_USES(ref);
}

static void ir_sccp_fold2(ir_ctx *ctx, ir_ref ref, ir_bitqueue *worklist)
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
					if (insn->op1 > 0) {
						ir_use_list_remove_one(ctx, insn->op1, ref);
					}
					if (ctx->fold_insn.op1 > 0) {
						ir_use_list_add(ctx, ctx->fold_insn.op1, ref);
					}
				}
				if (insn->op2 != ctx->fold_insn.op2) {
					if (insn->op2 > 0) {
						ir_use_list_remove_one(ctx, insn->op2, ref);
					}
					if (ctx->fold_insn.op2 > 0) {
						ir_use_list_add(ctx, ctx->fold_insn.op2, ref);
					}
				}
				if (insn->op3 != ctx->fold_insn.op3) {
					if (insn->op3 > 0) {
						ir_use_list_remove_one(ctx, insn->op3, ref);
					}
					if (ctx->fold_insn.op3 > 0) {
						ir_use_list_add(ctx, ctx->fold_insn.op3, ref);
					}
				}
				insn->op1 = ctx->fold_insn.op1;
				insn->op2 = ctx->fold_insn.op2;
				insn->op3 = ctx->fold_insn.op3;

				use_list = &ctx->use_lists[ref];
				n = use_list->count;
				for (j = 0, p = &ctx->use_edges[use_list->refs]; j < n; j++, p++) {
					use = *p;
					ir_bitqueue_add(worklist, use);
				}
			}
			break;
		case IR_FOLD_DO_COPY:
			op1 = ctx->fold_insn.op1;
			ir_sccp_replace_insn2(ctx, ref, op1, worklist);
			break;
		case IR_FOLD_DO_CONST:
			op1 = ir_const(ctx, ctx->fold_insn.val, ctx->fold_insn.type);
			ir_sccp_replace_insn2(ctx, ref, op1, worklist);
			break;
		default:
			IR_ASSERT(0);
			break;
	}
}

static void ir_sccp_remove_if(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_ref dst)
{
	ir_ref next;
	ir_insn *insn, *next_insn;

	insn = &ctx->ir_base[ref];
	if (ctx->use_lists[dst].count == 1) {
		next = ctx->use_edges[ctx->use_lists[dst].refs];
		next_insn = &ctx->ir_base[next];
		/* remove IF and IF_TRUE/FALSE from double linked control list */
		next_insn->op1 = insn->op1;
		ir_use_list_replace_one(ctx, insn->op1, ref, next);
		/* remove IF and IF_TRUE/FALSE instructions */
		ir_sccp_make_nop(ctx, ref);
		ir_sccp_make_nop(ctx, dst);
	} else {
		insn->op2 = IR_UNUSED;
		insn->optx = IR_OPTX(IR_END, IR_VOID, 1);
		next_insn = &ctx->ir_base[dst];
		next_insn->op = IR_BEGIN;
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
						if (use_list->count == 1) {
							next = ctx->use_edges[use_list->refs];
							next_insn = &ctx->ir_base[next];
						} else {
							for (k = 0, p = &ctx->use_edges[use_list->refs]; k < use_list->count; k++, p++) {
								use = *p;
								use_insn = &ctx->ir_base[use];
								IR_ASSERT((use_insn->op != IR_PHI) && "PHI must be already removed");
								if (ir_op_flags[use_insn->op] & IR_OP_FLAG_CONTROL) {
									IR_ASSERT(!next);
									next = use;
									next_insn = use_insn;
								} else if (use_insn->op != IR_NOP) {
									IR_ASSERT(use_insn->op1 == ref);
									use_insn->op1 = prev;
									ir_use_list_add(ctx, prev, use);
									p = &ctx->use_edges[use_list->refs + k];
								}
							}
						}
						IR_ASSERT(prev && next);
						/* remove MERGE and input END from double linked control list */
						next_insn->op1 = prev;
						ir_use_list_replace_one(ctx, prev, input, next);
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
		while (j <= n) {
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
							ir_use_list_remove_one(ctx, input, use);
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

static bool ir_may_promote_d2f(ir_ctx *ctx, ir_ref ref)
{
	ir_insn *insn = &ctx->ir_base[ref];

	IR_ASSERT(insn->type == IR_DOUBLE);
	if (IR_IS_CONST_REF(ref)) {
		return !IR_IS_SYM_CONST(insn->op) && insn->val.d == (double)(float)insn->val.d;
	} else {
		switch (insn->op) {
			case IR_FP2FP:
				return 1;
//			case IR_INT2FP:
//				return ctx->use_lists[ref].count == 1;
			case IR_NEG:
			case IR_ABS:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_d2f(ctx, insn->op1);
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_d2f(ctx, insn->op1) &&
					ir_may_promote_d2f(ctx, insn->op2);
			default:
				break;
		}
	}
	return 0;
}

static bool ir_may_promote_f2d(ir_ctx *ctx, ir_ref ref)
{
	ir_insn *insn = &ctx->ir_base[ref];

	IR_ASSERT(insn->type == IR_FLOAT);
	if (IR_IS_CONST_REF(ref)) {
		return !IR_IS_SYM_CONST(insn->op) && insn->val.f == (float)(double)insn->val.f;
	} else {
		switch (insn->op) {
			case IR_FP2FP:
				return 1;
			case IR_INT2FP:
				return ctx->use_lists[ref].count == 1;
			case IR_NEG:
			case IR_ABS:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_f2d(ctx, insn->op1);
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
//			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_f2d(ctx, insn->op1) &&
					ir_may_promote_f2d(ctx, insn->op2);
			default:
				break;
		}
	}
	return 0;
}

static ir_ref ir_promote_d2f(ir_ctx *ctx, ir_ref ref, ir_ref use)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;

	IR_ASSERT(insn->type == IR_DOUBLE);
	if (IR_IS_CONST_REF(ref)) {
		return ir_const_float(ctx, (float)insn->val.d);
	} else {
		switch (insn->op) {
			case IR_FP2FP:
				count = ctx->use_lists[ref].count;
				ir_use_list_remove_all(ctx, ref, use);
				if (ctx->use_lists[ref].count == 0) {
					ir_use_list_replace_one(ctx, insn->op1, ref, use);
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
					ref = insn->op1;
					MAKE_NOP(insn);
					return ref;
				} else {
					ir_use_list_add(ctx, insn->op1, use);
					count -= ctx->use_lists[ref].count;
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
				}
				return insn->op1;
//			case IR_INT2FP:
//				insn->type = IR_FLOAT;
//				return ref;
			case IR_NEG:
			case IR_ABS:
				insn->op1 = ir_promote_d2f(ctx, insn->op1, ref);
				insn->type = IR_FLOAT;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_d2f(ctx, insn->op1, ref);
				} else {
					insn->op1 = ir_promote_d2f(ctx, insn->op1, ref);
					insn->op2 = ir_promote_d2f(ctx, insn->op2, ref);
				}
				insn->type = IR_FLOAT;
				return ref;
			default:
				break;
		}
	}
	IR_ASSERT(0);
	return ref;
}

static ir_ref ir_promote_f2d(ir_ctx *ctx, ir_ref ref, ir_ref use)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;
	ir_ref old_ref;

	IR_ASSERT(insn->type == IR_FLOAT);
	if (IR_IS_CONST_REF(ref)) {
		return ir_const_double(ctx, (double)insn->val.f);
	} else {
		switch (insn->op) {
			case IR_FP2FP:
				count = ctx->use_lists[ref].count;
				ir_use_list_remove_all(ctx, ref, use);
				if (ctx->use_lists[ref].count == 0) {
					ir_use_list_replace_one(ctx, insn->op1, ref, use);
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
					ref = insn->op1;
					MAKE_NOP(insn);
					return ref;
				} else {
					ir_use_list_add(ctx, insn->op1, use);
					count -= ctx->use_lists[ref].count;
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
				}
				return insn->op1;
			case IR_INT2FP:
				old_ref = ir_find1(ctx, IR_OPTX(IR_INT2FP, IR_DOUBLE, 1), insn->op1);
				if (old_ref) {
					IR_ASSERT(ctx->use_lists[ref].count == 1);
					ir_use_list_remove_one(ctx, insn->op1, ref);
					CLEAR_USES(ref);
					MAKE_NOP(insn);
					ir_use_list_add(ctx, old_ref, use);
					return old_ref;
				}
				insn->type = IR_DOUBLE;
				return ref;
			case IR_NEG:
			case IR_ABS:
				insn->op1 = ir_promote_f2d(ctx, insn->op1, ref);
				insn->type = IR_DOUBLE;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
//			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_f2d(ctx, insn->op1, ref);
				} else {
					insn->op1 = ir_promote_f2d(ctx, insn->op1, ref);
					insn->op2 = ir_promote_f2d(ctx, insn->op2, ref);
				}
				insn->type = IR_DOUBLE;
				return ref;
			default:
				break;
		}
	}
	IR_ASSERT(0);
	return ref;
}

static bool ir_may_promote_i2i(ir_ctx *ctx, ir_type type, ir_ref ref)
{
	ir_insn *insn = &ctx->ir_base[ref];

	if (IR_IS_CONST_REF(ref)) {
		return !IR_IS_SYM_CONST(insn->op);
	} else {
		switch (insn->op) {
			case IR_ZEXT:
			case IR_SEXT:
				return ctx->ir_base[insn->op1].type == type;
			case IR_NEG:
			case IR_ABS:
			case IR_NOT:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_i2i(ctx, type, insn->op1);
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
//			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
			case IR_OR:
			case IR_AND:
			case IR_XOR:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_i2i(ctx, type, insn->op1) &&
					ir_may_promote_i2i(ctx, type, insn->op2);
			default:
				break;
		}
	}
	return 0;
}

static ir_ref ir_promote_i2i(ir_ctx *ctx, ir_type type, ir_ref ref, ir_ref use)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;

	if (IR_IS_CONST_REF(ref)) {
		return ir_const(ctx, insn->val, type);
	} else {
		switch (insn->op) {
			case IR_ZEXT:
			case IR_SEXT:
				count = ctx->use_lists[ref].count;
				ir_use_list_remove_all(ctx, ref, use);
				if (ctx->use_lists[ref].count == 0) {
					ir_use_list_replace_one(ctx, insn->op1, ref, use);
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
					ref = insn->op1;
					MAKE_NOP(insn);
					return ref;
				} else {
					ir_use_list_add(ctx, insn->op1, use);
					count -= ctx->use_lists[ref].count;
					if (count > 1) {
						do {
							ir_use_list_add(ctx, insn->op1, use);
						} while (--count > 1);
					}
				}
				return insn->op1;
			case IR_NEG:
			case IR_ABS:
			case IR_NOT:
				insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref);
				insn->type = type;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
//			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
			case IR_OR:
			case IR_AND:
			case IR_XOR:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref);
				} else {
					insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref);
					insn->op2 = ir_promote_i2i(ctx, type, insn->op2, ref);
				}
				insn->type = type;
				return ref;
			default:
				break;
		}
	}
	IR_ASSERT(0);
	return ref;
}

static ir_ref ir_ext_const(ir_ctx *ctx, ir_insn *val_insn, ir_op op, ir_type type)
{
	ir_val new_val;

	switch (val_insn->type) {
		default:
			IR_ASSERT(0);
		case IR_I8:
		case IR_U8:
		case IR_BOOL:
			if (op == IR_SEXT) {
				new_val.i64 = (int64_t)val_insn->val.i8;
			} else {
				new_val.u64 = (uint64_t)val_insn->val.u8;
			}
			break;
		case IR_I16:
		case IR_U16:
			if (op == IR_SEXT) {
				new_val.i64 = (int64_t)val_insn->val.i16;
			} else {
				new_val.u64 = (uint64_t)val_insn->val.u16;
			}
			break;
		case IR_I32:
		case IR_U32:
			if (op == IR_SEXT) {
				new_val.i64 = (int64_t)val_insn->val.i32;
			} else {
				new_val.u64 = (uint64_t)val_insn->val.u32;
			}
			break;
	}
	return ir_const(ctx, new_val, type);
}

static ir_ref ir_ext_ref(ir_ctx *ctx, ir_ref var_ref, ir_ref src_ref, ir_op op, ir_type type, ir_bitqueue *worklist)
{
	uint32_t optx = IR_OPTX(op, type, 1);
	ir_ref ref;

	if (!IR_IS_CONST_REF(src_ref)) {
		ref = ir_find1(ctx, optx, src_ref);
		if (ref) {
			ir_use_list_add(ctx, ref, var_ref);
			if (!IR_IS_CONST_REF(src_ref)) {
				ir_use_list_remove_one(ctx, src_ref, var_ref);
			}
			ir_bitqueue_add(worklist, ref);
			return ref;
		}
	}

	ref = ir_emit1(ctx, optx, src_ref);
	ctx->use_lists = ir_mem_realloc(ctx->use_lists, ctx->insns_count * sizeof(ir_use_list));
	ctx->use_lists[ref].count = 0;
	ctx->use_lists[ref].refs = IR_UNUSED;
	ir_use_list_add(ctx, ref, var_ref);
	if (!IR_IS_CONST_REF(src_ref)) {
		ir_use_list_replace_one(ctx, src_ref, var_ref, ref);
	}
	ir_bitqueue_grow(worklist, ref + 1);
	ir_bitqueue_add(worklist, ref);
	return ref;
}

static bool ir_try_promote_ext(ir_ctx *ctx, ir_ref ext_ref, ir_insn *insn, ir_bitqueue *worklist)
{
	ir_type type = insn->type;
	ir_op op = insn->op;
	ir_ref ref = insn->op1;
	ir_insn *phi_insn = &ctx->ir_base[ref];
	ir_insn *op_insn;
	ir_use_list *use_list;
	ir_ref n, *p, use, op_ref;

	/* Check for simple induction variable in the form: x2 = PHI(loop, x1, x3); x3 = ADD(x2, _); */
	if (phi_insn->op != IR_PHI
	 || phi_insn->inputs_count != 3 /* (2 values) */
	 || ctx->ir_base[phi_insn->op1].op != IR_LOOP_BEGIN) {
		return 0;
	}

	op_ref = phi_insn->op3;
	op_insn = &ctx->ir_base[op_ref];
	if ((op_insn->op != IR_ADD && op_insn->op != IR_SUB && op_insn->op != IR_MUL)
	 || (op_insn->op1 != ref && op_insn->op2 != ref)
	 || ctx->use_lists[op_ref].count != 1) {
		return 0;
	}

	/* Check if we may change the type of the induction variable */
	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
		use = *p;
		if (use == op_ref || use == ext_ref) {
			continue;
		} else {
			ir_insn *use_insn = &ctx->ir_base[use];

			if ((use_insn->op >= IR_EQ && use_insn->op <= IR_UGT)
			 && (use_insn->op1 == ref || use_insn->op2 == ref)) {
				continue;
			} else if (use_insn->op == IR_IF) {
				continue;
			} else {
				return 0;
			}
		}
	}

	phi_insn->type = insn->type;
	op_insn->type = insn->type;

	for (n = 0; n < ctx->use_lists[ref].count; n++) {
		/* "use_lists" may be reallocated by ir_ext_ref() */
		use = ctx->use_edges[ctx->use_lists[ref].refs + n];
		if (use == ext_ref) {
			continue;
		} else {
			ir_insn *use_insn = &ctx->ir_base[use];

			if (use_insn->op == IR_IF) {
				continue;
			}
			IR_ASSERT(((use_insn->op >= IR_EQ && use_insn->op <= IR_UGT)
			  || use_insn->op == IR_ADD || use_insn->op == IR_SUB || use_insn->op == IR_MUL)
			 && (use_insn->op1 == ref || use_insn->op2 == ref));
			if (use_insn->op1 != ref) {
				if (IR_IS_CONST_REF(use_insn->op1)
				 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op1].op)) {
					ctx->ir_base[use].op1 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op1], op, type);
				} else {
					ctx->ir_base[use].op1 = ir_ext_ref(ctx, use, use_insn->op1, op, type, worklist);
				}
			}
			if (use_insn->op2 != ref) {
				if (IR_IS_CONST_REF(use_insn->op2)
				 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op2].op)) {
					ctx->ir_base[use].op2 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op2], op, type);
				} else {
					ctx->ir_base[use].op2 = ir_ext_ref(ctx, use, use_insn->op2, op, type, worklist);
				}
			}
		}
	}

	ir_sccp_replace_insn2(ctx, ext_ref, ref, worklist);

	phi_insn = &ctx->ir_base[ref];
	if (IR_IS_CONST_REF(phi_insn->op2)
	 && !IR_IS_SYM_CONST(ctx->ir_base[phi_insn->op2].op)) {
		ctx->ir_base[ref].op2 = ir_ext_const(ctx, &ctx->ir_base[phi_insn->op2], op, type);
	} else {
		ctx->ir_base[ref].op2 = ir_ext_ref(ctx, ref, phi_insn->op2, op, type, worklist);
	}

	return 1;
}

static void ir_get_true_false_refs(const ir_ctx *ctx, ir_ref if_ref, ir_ref *if_true_ref, ir_ref *if_false_ref)
{
	ir_use_list *use_list = &ctx->use_lists[if_ref];
	ir_ref *p = &ctx->use_edges[use_list->refs];

	IR_ASSERT(use_list->count == 2);
	if (ctx->ir_base[*p].op == IR_IF_TRUE) {
		IR_ASSERT(ctx->ir_base[*(p + 1)].op == IR_IF_FALSE);
		*if_true_ref = *p;
		*if_false_ref = *(p + 1);
	} else {
		IR_ASSERT(ctx->ir_base[*p].op == IR_IF_FALSE);
		IR_ASSERT(ctx->ir_base[*(p + 1)].op == IR_IF_TRUE);
		*if_false_ref = *p;
		*if_true_ref = *(p + 1);
	}
}

static void ir_merge_blocks(ir_ctx *ctx, ir_ref end, ir_ref begin, ir_bitqueue *worklist2)
{
	ir_ref prev, next;
	ir_use_list *use_list;

	IR_ASSERT(ctx->ir_base[begin].op == IR_BEGIN);
	IR_ASSERT(ctx->ir_base[end].op == IR_END);
	IR_ASSERT(ctx->ir_base[begin].op1 == end);
	IR_ASSERT(ctx->use_lists[end].count == 1);

	prev = ctx->ir_base[end].op1;

	use_list = &ctx->use_lists[begin];
	IR_ASSERT(use_list->count == 1);
	next = ctx->use_edges[use_list->refs];

	/* remove BEGIN and END */
	MAKE_NOP(&ctx->ir_base[begin]); CLEAR_USES(begin);
	MAKE_NOP(&ctx->ir_base[end]);   CLEAR_USES(end);

	/* connect their predecessor and successor */
	ctx->ir_base[next].op1 = prev;
	ir_use_list_replace_all(ctx, prev, end, next);

	if (ctx->ir_base[prev].op == IR_BEGIN || ctx->ir_base[prev].op == IR_MERGE) {
		ir_bitqueue_add(worklist2, prev);
	}
}

static bool ir_try_remove_empty_diamond(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	if (insn->inputs_count == 2) {
		ir_ref end1_ref = insn->op1, end2_ref = insn->op2;
		ir_insn *end1 = &ctx->ir_base[end1_ref];
		ir_insn *end2 = &ctx->ir_base[end2_ref];

		if (end1->op != IR_END || end2->op != IR_END) {
			return 0;
		}

		ir_ref start1_ref = end1->op1, start2_ref = end2->op1;
		ir_insn *start1 = &ctx->ir_base[start1_ref];
		ir_insn *start2 = &ctx->ir_base[start2_ref];

		if (start1->op1 != start2->op1) {
			return 0;
		}

		ir_ref root_ref = start1->op1;
		ir_insn *root = &ctx->ir_base[root_ref];

		if (root->op != IR_IF
		 && !(root->op == IR_SWITCH && ctx->use_lists[root_ref].count == 2)) {
			return 0;
		}

		/* Empty Diamond
		 *
		 *    prev                     prev
		 *    |  condition             |  condition
		 *    | /                      |
		 *    IF                       |
		 *    | \                      |
		 *    |  +-----+               |
		 *    |        IF_FALSE        |
		 *    IF_TRUE  |           =>  |
		 *    |        END             |
		 *    END     /                |
		 *    |  +---+                 |
		 *    | /                      |
		 *    MERGE                    |
		 *    |                        |
		 *    next                     next
		 */

		ir_ref next_ref = ctx->use_edges[ctx->use_lists[ref].refs];
		ir_insn *next = &ctx->ir_base[next_ref];

		IR_ASSERT(ctx->use_lists[start1_ref].count == 1);
		IR_ASSERT(ctx->use_lists[start2_ref].count == 1);

		next->op1 = root->op1;
		ir_use_list_replace_one(ctx, root->op1, root_ref, next_ref);
		if (!IR_IS_CONST_REF(root->op2)) {
			ir_use_list_remove_all(ctx, root->op2, root_ref);
			if (ir_is_dead(ctx, root->op2)) {
				ir_bitqueue_add(worklist, root->op2);
			}
		}

		MAKE_NOP(root);   CLEAR_USES(root_ref);
		MAKE_NOP(start1); CLEAR_USES(start1_ref);
		MAKE_NOP(start2); CLEAR_USES(start2_ref);
		MAKE_NOP(end1);   CLEAR_USES(end1_ref);
		MAKE_NOP(end2);   CLEAR_USES(end2_ref);
		MAKE_NOP(insn);   CLEAR_USES(ref);

		if (ctx->ir_base[next->op1].op == IR_BEGIN || ctx->ir_base[next->op1].op == IR_MERGE) {
			ir_bitqueue_add(worklist, next->op1);
		}

		return 1;
	} else {
		ir_ref i, count = insn->inputs_count, *ops = insn->ops + 1;
		ir_ref root_ref = IR_UNUSED;

		for (i = 0; i < count; i++) {
			ir_ref end_ref, start_ref;
			ir_insn *end, *start;

			end_ref = ops[i];
			end = &ctx->ir_base[end_ref];
			if (end->op != IR_END) {
				return 0;
			}
			start_ref = end->op1;
			start = &ctx->ir_base[start_ref];
			if (start->op != IR_CASE_VAL && start->op != IR_CASE_DEFAULT) {
				return 0;
			}
			IR_ASSERT(ctx->use_lists[start_ref].count == 1);
			if (!root_ref) {
				root_ref = start->op1;
				if (ctx->use_lists[root_ref].count != count) {
					return 0;
				}
			} else if (start->op1 != root_ref) {
				return 0;
			}
		}

		/* Empty N-Diamond */
		ir_ref next_ref = ctx->use_edges[ctx->use_lists[ref].refs];
		ir_insn *next = &ctx->ir_base[next_ref];
		ir_insn *root = &ctx->ir_base[root_ref];

		next->op1 = root->op1;
		ir_use_list_replace_one(ctx, root->op1, root_ref, next_ref);

		if (!IR_IS_CONST_REF(root->op2)) {
			ir_use_list_remove_all(ctx, root->op2, root_ref);
			if (ir_is_dead(ctx, root->op2)) {
				ir_bitqueue_add(worklist, root->op2);
			}
		}

		MAKE_NOP(root);   CLEAR_USES(root_ref);

		for (i = 0; i < count; i++) {
			ir_ref end_ref = ops[i];
			ir_insn *end = &ctx->ir_base[end_ref];
			ir_ref start_ref = end->op1;
			ir_insn *start = &ctx->ir_base[start_ref];

			MAKE_NOP(start); CLEAR_USES(start_ref);
			MAKE_NOP(end);   CLEAR_USES(end_ref);
		}

		MAKE_NOP(insn);   CLEAR_USES(ref);

		if (ctx->ir_base[next->op1].op == IR_BEGIN || ctx->ir_base[next->op1].op == IR_MERGE) {
			ir_bitqueue_add(worklist, next->op1);
		}

		return 1;
	}
}

static bool ir_is_zero(ir_ctx *ctx, ir_ref ref)
{
	return IR_IS_CONST_REF(ref)
		&& !IR_IS_SYM_CONST(ctx->ir_base[ref].op)
		&& ctx->ir_base[ref].val.u32 == 0;
}

static bool ir_optimize_phi(ir_ctx *ctx, ir_ref merge_ref, ir_insn *merge, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	IR_ASSERT(insn->inputs_count == 3);
	IR_ASSERT(ctx->use_lists[merge_ref].count == 2);

	ir_ref end1_ref = merge->op1, end2_ref = merge->op2;
	ir_insn *end1 = &ctx->ir_base[end1_ref];
	ir_insn *end2 = &ctx->ir_base[end2_ref];

	if (end1->op == IR_END && end2->op == IR_END) {
		ir_ref start1_ref = end1->op1, start2_ref = end2->op1;
		ir_insn *start1 = &ctx->ir_base[start1_ref];
		ir_insn *start2 = &ctx->ir_base[start2_ref];

		if (start1->op1 == start2->op1) {
			ir_ref root_ref = start1->op1;
			ir_insn *root = &ctx->ir_base[root_ref];

			if (root->op == IR_IF && ctx->use_lists[root->op2].count == 1) {
				ir_ref cond_ref = root->op2;
				ir_insn *cond = &ctx->ir_base[cond_ref];
				ir_type type = insn->type;
				bool is_cmp, is_less;

				if (IR_IS_TYPE_FP(type)) {
					is_cmp = (cond->op == IR_LT || cond->op == IR_LE || cond->op == IR_GT || cond->op == IR_GE ||
						cond->op == IR_ULT || cond->op == IR_ULE || cond->op == IR_UGT || cond->op == IR_UGE);
					is_less = (cond->op == IR_LT || cond->op == IR_LE ||
						cond->op == IR_ULT || cond->op == IR_ULE);
				} else if (IR_IS_TYPE_SIGNED(type)) {
					is_cmp = (cond->op == IR_LT || cond->op == IR_LE || cond->op == IR_GT || cond->op == IR_GE);
					is_less = (cond->op == IR_LT || cond->op == IR_LE);
				} else {
					IR_ASSERT(IR_IS_TYPE_UNSIGNED(type));
					is_cmp = (cond->op == IR_ULT || cond->op == IR_ULE || cond->op == IR_UGT || cond->op == IR_UGE);
					is_less = (cond->op == IR_ULT || cond->op == IR_ULE);
				}

				if (is_cmp
				 && ((insn->op2 == cond->op1 && insn->op3 == cond->op2)
				   || (insn->op2 == cond->op2 && insn->op3 == cond->op1))) {
					/* MAX/MIN
					 *
					 *    prev                     prev
					 *    |  LT(A, B)              |
					 *    | /                      |
					 *    IF                       |
					 *    | \                      |
					 *    |  +-----+               |
					 *    |        IF_FALSE        |
					 *    IF_TRUE  |           =>  |
					 *    |        END             |
					 *    END     /                |
					 *    |  +---+                 |
					 *    | /                      |
					 *    MERGE                    |
					 *    |    \                   |
					 *    |     PHI(A, B)          |    MIN(A, B)
					 *    next                     next
					 */
					ir_ref next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs];
					ir_insn *next;

					if (next_ref == ref) {
						next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs + 1];
					}
					next = &ctx->ir_base[next_ref];

					IR_ASSERT(ctx->use_lists[start1_ref].count == 1);
					IR_ASSERT(ctx->use_lists[start2_ref].count == 1);

					insn->op = (
						(is_less ? cond->op1 : cond->op2)
						==
						((start1->op == IR_IF_TRUE) ? insn->op2 : insn->op3)
						) ? IR_MIN : IR_MAX;
					insn->inputs_count = 2;
					if (insn->op2 > insn->op3) {
						insn->op1 = insn->op2;
						insn->op2 = insn->op3;
					} else {
						insn->op1 = insn->op3;
					}
					insn->op3 = IR_UNUSED;

					next->op1 = root->op1;
					ir_use_list_replace_one(ctx, root->op1, root_ref, next_ref);
					if (!IR_IS_CONST_REF(insn->op1)) {
						ir_use_list_remove_all(ctx, insn->op1, cond_ref);
					}
					if (!IR_IS_CONST_REF(insn->op2)) {
						ir_use_list_remove_all(ctx, insn->op2, cond_ref);
					}

					MAKE_NOP(cond);   CLEAR_USES(cond_ref);
					MAKE_NOP(root);   CLEAR_USES(root_ref);
					MAKE_NOP(start1); CLEAR_USES(start1_ref);
					MAKE_NOP(start2); CLEAR_USES(start2_ref);
					MAKE_NOP(end1);   CLEAR_USES(end1_ref);
					MAKE_NOP(end2);   CLEAR_USES(end2_ref);
					MAKE_NOP(merge);  CLEAR_USES(merge_ref);

					if (ctx->ir_base[next->op1].op == IR_BEGIN || ctx->ir_base[next->op1].op == IR_MERGE) {
						ir_bitqueue_add(worklist, next->op1);
					}

					return 1;
				} else if (is_cmp
						&& ((ctx->ir_base[insn->op2].op == IR_NEG
						  && ctx->use_lists[insn->op2].count == 1
						  && ctx->ir_base[insn->op2].op1 == insn->op3
						  && ((cond->op1 == insn->op3
						    && ir_is_zero(ctx, cond->op2)
						    && is_less == (start1->op == IR_IF_TRUE))
						   || (cond->op2 == insn->op3
						    && ir_is_zero(ctx, cond->op1)
						    && is_less != (start1->op == IR_IF_TRUE))))
						 || (ctx->ir_base[insn->op3].op == IR_NEG
						  && ctx->use_lists[insn->op3].count == 1
						  && ctx->ir_base[insn->op3].op1 == insn->op2
						  && ((cond->op1 == insn->op2
						    && ir_is_zero(ctx, cond->op2)
						    && is_less != (start1->op == IR_IF_TRUE))
						   || (cond->op2 == insn->op2
						    && ir_is_zero(ctx, cond->op1)
						    && is_less == (start1->op == IR_IF_TRUE)))))) {
					/* ABS
					 *
					 *    prev                     prev
					 *    |  LT(A, 0)              |
					 *    | /                      |
					 *    IF                       |
					 *    | \                      |
					 *    |  +-----+               |
					 *    |        IF_FALSE        |
					 *    IF_TRUE  |           =>  |
					 *    |        END             |
					 *    END     /                |
					 *    |  +---+                 |
					 *    | /                      |
					 *    MERGE                    |
					 *    |    \                   |
					 *    |     PHI(A, NEG(A))     |    ABS(A)
					 *    next                     next
					 */
					ir_ref neg_ref;
					ir_ref next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs];
					ir_insn *next;

					if (next_ref == ref) {
						next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs + 1];
					}
					next = &ctx->ir_base[next_ref];

					IR_ASSERT(ctx->use_lists[start1_ref].count == 1);
					IR_ASSERT(ctx->use_lists[start2_ref].count == 1);

					insn->op = IR_ABS;
					insn->inputs_count = 1;
					if (ctx->ir_base[insn->op2].op == IR_NEG) {
						neg_ref = insn->op2;
						insn->op1 = insn->op3;
					} else {
						neg_ref = insn->op3;
						insn->op1 = insn->op2;
					}
					insn->op2 = IR_UNUSED;
					insn->op3 = IR_UNUSED;

					next->op1 = root->op1;
					ir_use_list_replace_one(ctx, root->op1, root_ref, next_ref);
					ir_use_list_remove_one(ctx, insn->op1, neg_ref);
					if (!IR_IS_CONST_REF(insn->op1)) {
						ir_use_list_remove_all(ctx, insn->op1, cond_ref);
					}

					MAKE_NOP(cond);   CLEAR_USES(cond_ref);
					MAKE_NOP(root);   CLEAR_USES(root_ref);
					MAKE_NOP(start1); CLEAR_USES(start1_ref);
					MAKE_NOP(start2); CLEAR_USES(start2_ref);
					MAKE_NOP(end1);   CLEAR_USES(end1_ref);
					MAKE_NOP(end2);   CLEAR_USES(end2_ref);
					MAKE_NOP(merge);  CLEAR_USES(merge_ref);
					MAKE_NOP(&ctx->ir_base[neg_ref]); CLEAR_USES(neg_ref);

					if (ctx->ir_base[next->op1].op == IR_BEGIN || ctx->ir_base[next->op1].op == IR_MERGE) {
						ir_bitqueue_add(worklist, next->op1);
					}

					return 1;
#if 0
				} else {
					/* COND
					 *
					 *    prev                     prev
					 *    |  cond                  |
					 *    | /                      |
					 *    IF                       |
					 *    | \                      |
					 *    |  +-----+               |
					 *    |        IF_FALSE        |
					 *    IF_TRUE  |           =>  |
					 *    |        END             |
					 *    END     /                |
					 *    |  +---+                 |
					 *    | /                      |
					 *    MERGE                    |
					 *    |    \                   |
					 *    |     PHI(A, B)          |    COND(cond, A, B)
					 *    next                     next
					 */
					ir_ref next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs];
					ir_insn *next;

					if (next_ref == ref) {
						next_ref = ctx->use_edges[ctx->use_lists[merge_ref].refs + 1];
					}
					next = &ctx->ir_base[next_ref];

					IR_ASSERT(ctx->use_lists[start1_ref].count == 1);
					IR_ASSERT(ctx->use_lists[start2_ref].count == 1);

					insn->op = IR_COND;
					insn->inputs_count = 3;
					insn->op1 = cond_ref;
					if (start1->op == IR_IF_FALSE) {
						SWAP_REFS(insn->op2, insn->op3);
					}

					next->op1 = root->op1;
					ir_use_list_replace_one(ctx, cond_ref, root_ref, ref);
					ir_use_list_replace_one(ctx, root->op1, root_ref, next_ref);
					ir_use_list_remove_all(ctx, root->op2, root_ref);

					MAKE_NOP(root);   CLEAR_USES(root_ref);
					MAKE_NOP(start1); CLEAR_USES(start1_ref);
					MAKE_NOP(start2); CLEAR_USES(start2_ref);
					MAKE_NOP(end1);   CLEAR_USES(end1_ref);
					MAKE_NOP(end2);   CLEAR_USES(end2_ref);
					MAKE_NOP(merge);  CLEAR_USES(merge_ref);

					if (ctx->ir_base[next->op1].op == IR_BEGIN || ctx->ir_base[next->op1].op == IR_MERGE) {
						ir_bitqueue_add(worklist, next->op1);
					}

					return 1;
#endif
				}
			}
		}
	}

	return 0;
}

static bool ir_cmp_is_true(ir_op op, ir_insn *op1, ir_insn *op2)
{
	IR_ASSERT(op1->type == op2->type);
	if (IR_IS_TYPE_INT(op1->type)) {
		if (op == IR_EQ) {
			return op1->val.u64 == op2->val.u64;
		} else if (op == IR_NE) {
			return op1->val.u64 != op2->val.u64;
		} else if (op == IR_LT) {
			if (IR_IS_TYPE_SIGNED(op1->type)) {
				return op1->val.i64 < op2->val.i64;
			} else {
				return op1->val.u64 < op2->val.u64;
			}
		} else if (op == IR_GE) {
			if (IR_IS_TYPE_SIGNED(op1->type)) {
				return op1->val.i64 >= op2->val.i64;
			} else {
				return op1->val.u64 >= op2->val.u64;
			}
		} else if (op == IR_LE) {
			if (IR_IS_TYPE_SIGNED(op1->type)) {
				return op1->val.i64 <= op2->val.i64;
			} else {
				return op1->val.u64 <= op2->val.u64;
			}
		} else if (op == IR_GT) {
			if (IR_IS_TYPE_SIGNED(op1->type)) {
				return op1->val.i64 > op2->val.i64;
			} else {
				return op1->val.u64 > op2->val.u64;
			}
		} else if (op == IR_ULT) {
			return op1->val.u64 < op2->val.u64;
		} else if (op == IR_UGE) {
			return op1->val.u64 >= op2->val.u64;
		} else if (op == IR_ULE) {
			return op1->val.u64 <= op2->val.u64;
		} else if (op == IR_UGT) {
			return op1->val.u64 > op2->val.u64;
		} else {
			IR_ASSERT(0);
			return 0;
		}
	} else if (op1->type == IR_DOUBLE) {
		if (op == IR_EQ) {
			return op1->val.d == op2->val.d;
		} else if (op == IR_NE) {
			return op1->val.d != op2->val.d;
		} else if (op == IR_LT) {
			return op1->val.d < op2->val.d;
		} else if (op == IR_GE) {
			return op1->val.d >= op2->val.d;
		} else if (op == IR_LE) {
			return op1->val.d <= op2->val.d;
		} else if (op == IR_GT) {
			return op1->val.d > op2->val.d;
		} else if (op == IR_ULT) {
			return !(op1->val.d >= op2->val.d);
		} else if (op == IR_UGE) {
			return !(op1->val.d < op2->val.d);
		} else if (op == IR_ULE) {
			return !(op1->val.d > op2->val.d);
		} else if (op == IR_UGT) {
			return !(op1->val.d <= op2->val.d);
		} else {
			IR_ASSERT(0);
			return 0;
		}
	} else {
		IR_ASSERT(op1->type == IR_FLOAT);
		if (op == IR_EQ) {
			return op1->val.f == op2->val.f;
		} else if (op == IR_NE) {
			return op1->val.f != op2->val.f;
		} else if (op == IR_LT) {
			return op1->val.f < op2->val.f;
		} else if (op == IR_GE) {
			return op1->val.f >= op2->val.f;
		} else if (op == IR_LE) {
			return op1->val.f <= op2->val.f;
		} else if (op == IR_GT) {
			return op1->val.f > op2->val.f;
		} else if (op == IR_ULT) {
			return !(op1->val.f >= op2->val.f);
		} else if (op == IR_UGE) {
			return !(op1->val.f < op2->val.f);
		} else if (op == IR_ULE) {
			return !(op1->val.f > op2->val.f);
		} else if (op == IR_UGT) {
			return !(op1->val.f <= op2->val.f);
		} else {
			IR_ASSERT(0);
			return 0;
		}
	}
}

static bool ir_try_split_if(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	ir_ref cond_ref = insn->op2;
	ir_insn *cond = &ctx->ir_base[cond_ref];

	if (cond->op == IR_PHI
	 && cond->inputs_count == 3
	 && cond->op1 == insn->op1
	 && ((IR_IS_CONST_REF(cond->op2) && !IR_IS_SYM_CONST(ctx->ir_base[cond->op2].op))
	  || (IR_IS_CONST_REF(cond->op3) && !IR_IS_SYM_CONST(ctx->ir_base[cond->op3].op)))) {
		ir_ref merge_ref = insn->op1;
		ir_insn *merge = &ctx->ir_base[merge_ref];

		if (ctx->use_lists[merge_ref].count == 2) {
			ir_ref end1_ref = merge->op1, end2_ref = merge->op2;
			ir_insn *end1 = &ctx->ir_base[end1_ref];
			ir_insn *end2 = &ctx->ir_base[end2_ref];

			if (end1->op == IR_END && end2->op == IR_END) {
				ir_ref if_true_ref, if_false_ref;
				ir_insn *if_true, *if_false;
				ir_op op = IR_IF_FALSE;

				ir_get_true_false_refs(ctx, ref, &if_true_ref, &if_false_ref);

				if (!IR_IS_CONST_REF(cond->op2) || IR_IS_SYM_CONST(ctx->ir_base[cond->op2].op)) {
					IR_ASSERT(IR_IS_CONST_REF(cond->op3));
					SWAP_REFS(cond->op2, cond->op3);
					SWAP_REFS(merge->op1, merge->op2);
					SWAP_REFS(end1_ref, end2_ref);
					SWAP_INSNS(end1, end2);
				}
				if (ir_const_is_true(&ctx->ir_base[cond->op2])) {
					SWAP_REFS(if_true_ref, if_false_ref);
					op = IR_IF_TRUE;
				}
				if_true = &ctx->ir_base[if_true_ref];
				if_false = &ctx->ir_base[if_false_ref];

				if (IR_IS_CONST_REF(cond->op3) && !IR_IS_SYM_CONST(ctx->ir_base[cond->op3].op)) {
					if (ir_const_is_true(&ctx->ir_base[cond->op3]) ^ (op == IR_IF_TRUE)) {
						/* Simple IF Split
						 *
						 *    |        |               |        |
						 *    |        END             |        END
						 *    END     /                END       \
						 *    |  +---+                 |          +
						 *    | /                      |          |
						 *    MERGE                    |          |
						 *    | \                      |          |
						 *    |  PHI(false, true)      |          |
						 *    | /                      |          |
						 *    IF                   =>  |          |
						 *    | \                      |          |
						 *    |  +------+              |          |
						 *    |         IF_TRUE        |          BEGIN
						 *    IF_FALSE  |              BEGIN
						 *    |                        |
						 */
						ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
						ir_use_list_replace_one(ctx, end2_ref, merge_ref, if_true_ref);

						MAKE_NOP(merge); CLEAR_USES(merge_ref);
						MAKE_NOP(cond);  CLEAR_USES(cond_ref);
						MAKE_NOP(insn);  CLEAR_USES(ref);

						if_false->optx = IR_OPTX(IR_BEGIN, IR_VOID, 1);
						if_false->op1 = end1_ref;

						if_true->optx = IR_OPTX(IR_BEGIN, IR_VOID, 1);
						if_true->op1 = end2_ref;

						ir_bitqueue_add(worklist, if_false_ref);
						ir_bitqueue_add(worklist, if_true_ref);

						return 1;
					} else {
						/* Simple IF Split
						 *
						 *    |        |               |        |
						 *    |        END             |        END
						 *    END     /                END      |
						 *    |  +---+                 |        |
						 *    | /                      |        |
						 *    MERGE                    |        +
						 *    | \                      |       /
						 *    |  PHI(false, false)     |      /
						 *    | /                      |     /
						 *    IF                   =>  |    /
						 *    | \                      |   /
						 *    |  +------+              |  /
						 *    |         IF_TRUE        | /        BEGIN(unreachable)
						 *    IF_FALSE  |              MERGE
						 *    |                        |
						 */
						ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
						ir_use_list_replace_one(ctx, end2_ref, merge_ref, if_false_ref);

						MAKE_NOP(merge); CLEAR_USES(merge_ref);
						MAKE_NOP(cond);  CLEAR_USES(cond_ref);
						MAKE_NOP(insn);  CLEAR_USES(ref);

						if_false->optx = IR_OPTX(IR_MERGE, IR_VOID, 2);
						if_false->op1 = end1_ref;
						if_false->op2 = end2_ref;

						if_true->optx = IR_BEGIN;
						if_true->op1 = IR_UNUSED;

						ctx->flags2 &= ~IR_CFG_REACHABLE;

						ir_bitqueue_add(worklist, if_false_ref);

						return 1;
					}
				}

				/* Simple IF Split
				 *
				 *    |        |               |        |
				 *    |        END             |        IF(X)
				 *    END     /                END     / \
				 *    |  +---+                 |   +--+   +
				 *    | /                      |  /       |
				 *    MERGE                    | IF_FALSE |
				 *    | \                      | |        |
				 *    |  PHI(false, X)         | |        |
				 *    | /                      | |        |
				 *    IF                   =>  | END      |
				 *    | \                      | |        |
				 *    |  +------+              | |        |
				 *    |         IF_TRUE        | |        IF_TRUE
				 *    IF_FALSE  |              MERGE
				 *    |                        |
				 */
				ir_use_list_remove_all(ctx, merge_ref, cond_ref);
				ir_use_list_remove_all(ctx, ref, if_true_ref);
				if (!IR_IS_CONST_REF(cond->op3)) {
					ir_use_list_replace_one(ctx, cond->op3, cond_ref, end2_ref);
				}
				ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
				ir_use_list_add(ctx, end2_ref, if_true_ref);

				end2->optx = IR_OPTX(IR_IF, IR_VOID, 2);
				end2->op2 = cond->op3;

				merge->optx = IR_OPTX(op, IR_VOID, 1);
				merge->op1 = end2_ref;
				merge->op2 = IR_UNUSED;

				MAKE_NOP(cond);
				CLEAR_USES(cond_ref);

				insn->optx = IR_OPTX(IR_END, IR_VOID, 1);
				insn->op1 = merge_ref;
				insn->op2 = IR_UNUSED;

				if_true->op1 = end2_ref;

				if_false->optx = IR_OPTX(IR_MERGE, IR_VOID, 2);
				if_false->op1 = end1_ref;
				if_false->op2 = ref;

				ir_bitqueue_add(worklist, if_false_ref);
				if (ctx->ir_base[end2->op1].op == IR_BEGIN || ctx->ir_base[end2->op1].op == IR_MERGE) {
					ir_bitqueue_add(worklist, end2->op1);
				}

				return 1;
			}
		}
	}

	return 0;
}

static bool ir_try_split_if_cmp(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	ir_ref cond_ref = insn->op2;
	ir_insn *cond = &ctx->ir_base[cond_ref];

	if (cond->op >= IR_EQ && cond->op <= IR_UGT
	 && IR_IS_CONST_REF(cond->op2)
	 && !IR_IS_SYM_CONST(ctx->ir_base[cond->op2].op)
	 && ctx->use_lists[insn->op2].count == 1) {
		ir_ref phi_ref = cond->op1;
		ir_insn *phi = &ctx->ir_base[phi_ref];

		if (phi->op == IR_PHI
		 && phi->inputs_count == 3
		 && phi->op1 == insn->op1
		 && ctx->use_lists[phi_ref].count == 1
		 && ((IR_IS_CONST_REF(phi->op2) && !IR_IS_SYM_CONST(ctx->ir_base[phi->op2].op))
		  || (IR_IS_CONST_REF(phi->op3) && !IR_IS_SYM_CONST(ctx->ir_base[phi->op3].op)))) {
			ir_ref merge_ref = insn->op1;
			ir_insn *merge = &ctx->ir_base[merge_ref];

			if (ctx->use_lists[merge_ref].count == 2) {
				ir_ref end1_ref = merge->op1, end2_ref = merge->op2;
				ir_insn *end1 = &ctx->ir_base[end1_ref];
				ir_insn *end2 = &ctx->ir_base[end2_ref];

				if (end1->op == IR_END && end2->op == IR_END) {
					ir_ref if_true_ref, if_false_ref;
					ir_insn *if_true, *if_false;
					ir_op op = IR_IF_FALSE;

					ir_get_true_false_refs(ctx, ref, &if_true_ref, &if_false_ref);

					if (!IR_IS_CONST_REF(phi->op2) || IR_IS_SYM_CONST(ctx->ir_base[phi->op2].op)) {
						IR_ASSERT(IR_IS_CONST_REF(phi->op3));
						SWAP_REFS(phi->op2, phi->op3);
						SWAP_REFS(merge->op1, merge->op2);
						SWAP_REFS(end1_ref, end2_ref);
						SWAP_INSNS(end1, end2);
					}
					if (ir_cmp_is_true(cond->op, &ctx->ir_base[phi->op2], &ctx->ir_base[cond->op2])) {
						SWAP_REFS(if_true_ref, if_false_ref);
						op = IR_IF_TRUE;
					}
					if_true = &ctx->ir_base[if_true_ref];
					if_false = &ctx->ir_base[if_false_ref];

					if (IR_IS_CONST_REF(phi->op3) && !IR_IS_SYM_CONST(ctx->ir_base[phi->op3].op)) {
						if (ir_cmp_is_true(cond->op, &ctx->ir_base[phi->op3], &ctx->ir_base[cond->op2]) ^ (op == IR_IF_TRUE)) {
							/* IF Split
							 *
							 *    |        |               |        |
							 *    |        END             |        END
							 *    END     /                END      |
							 *    |  +---+                 |        |
							 *    | /                      |        |
							 *    MERGE                    |        |
							 *    | \                      |        |
							 *    |  PHI(C1, X)            |        |
							 *    |  |                     |        |
							 *    |  CMP(_, C2)            |        |
							 *    | /                      |        |
							 *    IF                   =>  |        |
							 *    | \                      |        |
							 *    |  +------+              |        |
							 *    |         IF_TRUE        |        BEGIN
							 *    IF_FALSE  |              BEGIN    |
							 *    |                        |
							 */

							ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
							ir_use_list_replace_one(ctx, end2_ref, merge_ref, if_true_ref);

							MAKE_NOP(merge); CLEAR_USES(merge_ref);
							MAKE_NOP(phi);   CLEAR_USES(phi_ref);
							MAKE_NOP(cond);  CLEAR_USES(cond_ref);
							MAKE_NOP(insn);  CLEAR_USES(ref);

							if_false->optx = IR_OPTX(IR_BEGIN, IR_VOID, 1);
							if_false->op1 = end1_ref;

							if_true->optx = IR_OPTX(IR_BEGIN, IR_VOID, 1);
							if_true->op1 = end2_ref;

							ir_bitqueue_add(worklist, if_false_ref);
							ir_bitqueue_add(worklist, if_true_ref);

							return 1;
						} else {
							/* IF Split
							 *
							 *    |        |               |        |
							 *    |        END             |        END
							 *    END     /                END      |
							 *    |  +---+                 |        |
							 *    | /                      |        |
							 *    MERGE                    |        |
							 *    | \                      |        |
							 *    |  PHI(C1, X)            |        |
							 *    |  |                     |        +
							 *    |  CMP(_, C2)            |       /
							 *    | /                      |      /
							 *    IF                   =>  |     /
							 *    | \                      |    /
							 *    |  +------+              |   /
							 *    |         IF_TRUE        |  /      BEGIN(unreachable)
							 *    IF_FALSE  |              MERGE     |
							 *    |                        |
							 */

							ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
							ir_use_list_replace_one(ctx, end2_ref, merge_ref, if_false_ref);

							MAKE_NOP(merge); CLEAR_USES(merge_ref);
							MAKE_NOP(phi);   CLEAR_USES(phi_ref);
							MAKE_NOP(cond);  CLEAR_USES(cond_ref);
							MAKE_NOP(insn);  CLEAR_USES(ref);

							if_false->optx = IR_OPTX(IR_MERGE, IR_VOID, 2);
							if_false->op1 = end1_ref;
							if_false->op2 = end2_ref;

							if_true->optx = IR_BEGIN;
							if_true->op1 = IR_UNUSED;

							ctx->flags2 &= ~IR_CFG_REACHABLE;

							ir_bitqueue_add(worklist, if_false_ref);

							return 1;
						}
					} else {
						/* IF Split
						 *
						 *    |        |               |        |
						 *    |        END             |        IF<----+
						 *    END     /                END     / \     |
						 *    |  +---+                 |   +--+   +    |
						 *    | /                      |  /       |    |
						 *    MERGE                    | IF_FALSE |    |
						 *    | \                      | |        |    |
						 *    |  PHI(C1, X)            | |        |    |
						 *    |  |                     | |        |    |
						 *    |  CMP(_, C2)            | |        |    CMP(X, C2)
						 *    | /                      | |        |
						 *    IF                   =>  | END      |
						 *    | \                      | |        |
						 *    |  +------+              | |        |
						 *    |         IF_TRUE        | |        IF_TRUE
						 *    IF_FALSE  |              MERGE
						 *    |                        |
						 */

						ir_use_list_remove_all(ctx, merge_ref, phi_ref);
						ir_use_list_remove_all(ctx, ref, if_true_ref);
						if (!IR_IS_CONST_REF(phi->op3)) {
							ir_use_list_replace_one(ctx, phi->op3, phi_ref, insn->op2);
						}
						ir_use_list_replace_one(ctx, end1_ref, merge_ref, if_false_ref);
						ir_use_list_replace_one(ctx, cond_ref, ref, end2_ref);
						ir_use_list_add(ctx, end2_ref, if_true_ref);

						end2->optx = IR_OPTX(IR_IF, IR_VOID, 2);
						end2->op2 = insn->op2;

						merge->optx = IR_OPTX(op, IR_VOID, 1);
						merge->op1 = end2_ref;
						merge->op2 = IR_UNUSED;

						cond->op1 = phi->op3;
						MAKE_NOP(phi);
						CLEAR_USES(phi_ref);

						insn->optx = IR_OPTX(IR_END, IR_VOID, 1);
						insn->op1 = merge_ref;
						insn->op2 = IR_UNUSED;

						if_true->op1 = end2_ref;

						if_false->optx = IR_OPTX(IR_MERGE, IR_VOID, 2);
						if_false->op1 = end1_ref;
						if_false->op2 = ref;

						ir_bitqueue_add(worklist, if_false_ref);
						if (ctx->ir_base[end2->op1].op == IR_BEGIN || ctx->ir_base[end2->op1].op == IR_MERGE) {
							ir_bitqueue_add(worklist, end2->op1);
						}

						return 1;
					}
				}
			}
		}
	}

	return 0;
}

static void ir_optimize_merge(ir_ctx *ctx, ir_ref merge_ref, ir_insn *merge, ir_bitqueue *worklist)
{
	ir_use_list *use_list = &ctx->use_lists[merge_ref];

	if (use_list->count == 1) {
		ir_try_remove_empty_diamond(ctx, merge_ref, merge, worklist);
	} else if (use_list->count == 2) {
		if (merge->inputs_count == 2) {
			ir_ref phi_ref = ctx->use_edges[use_list->refs];
			ir_insn *phi = &ctx->ir_base[phi_ref];

			ir_ref next_ref = ctx->use_edges[use_list->refs + 1];
			ir_insn *next = &ctx->ir_base[next_ref];
			IR_ASSERT(next->op != IR_PHI);

			if (phi->op == IR_PHI) {
				if (next->op == IR_IF && next->op1 == merge_ref && ctx->use_lists[phi_ref].count == 1) {
					if (next->op2 == phi_ref) {
						if (ir_try_split_if(ctx, next_ref, next, worklist)) {
							return;
						}
					} else {
						ir_insn *cmp = &ctx->ir_base[next->op2];

						if (cmp->op >= IR_EQ && cmp->op <= IR_UGT
						 && cmp->op1 == phi_ref
						 && IR_IS_CONST_REF(cmp->op2)
						 && !IR_IS_SYM_CONST(ctx->ir_base[cmp->op2].op)
						 && ctx->use_lists[next->op2].count == 1) {
							if (ir_try_split_if_cmp(ctx, next_ref, next, worklist)) {
								return;
							}
						}
					}
				}
				ir_optimize_phi(ctx, merge_ref, merge, phi_ref, phi, worklist);
			}
		}
	}
}

int ir_sccp(ir_ctx *ctx)
{
	ir_ref i, j, n, *p, use;
	ir_use_list *use_list;
	ir_insn *insn, *use_insn, *value;
	uint32_t flags;
	ir_bitqueue worklist, worklist2;
	ir_insn *_values = ir_mem_calloc(ctx->insns_count, sizeof(ir_insn));

	ctx->flags2 |= IR_OPT_IN_SCCP;

	/* A bit modified SCCP algorithm of M. N. Wegman and F. K. Zadeck */
	ir_bitqueue_init(&worklist2, ctx->insns_count);
	ir_bitqueue_init(&worklist, ctx->insns_count);
	worklist.pos = 0;
	ir_bitset_incl(worklist.set, 1);
	while ((i = ir_bitqueue_pop(&worklist)) >= 0) {
		insn = &ctx->ir_base[i];
		flags = ir_op_flags[insn->op];
		if (flags & IR_OP_FLAG_DATA) {
			if (ctx->use_lists[i].count == 0) {
				/* dead code */
				continue;
			} else if (insn->op == IR_PHI) {
				if (!ir_sccp_meet_phi(ctx, _values, i, insn, &worklist)) {
					continue;
				}
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
					if (insn->op == IR_FP2FP || insn->op == IR_FP2INT || insn->op == IR_TRUNC
					 || insn->op == IR_ZEXT || insn->op == IR_SEXT) {
						ir_bitqueue_add(&worklist2, i);
					}
				} else if (!ir_sccp_fold(ctx, _values, i, insn->opt, insn->op1, insn->op2, insn->op3)) {
					/* not changed */
					continue;
				} else if (_values[i].optx == IR_BOTTOM) {
					insn = &ctx->ir_base[i];
					if (insn->op == IR_FP2FP || insn->op == IR_FP2INT || insn->op == IR_TRUNC
					 || insn->op == IR_ZEXT || insn->op == IR_SEXT) {
						ir_bitqueue_add(&worklist2, i);
					}
				}
			} else {
				IR_MAKE_BOTTOM(i);
			}
		} else if (flags & IR_OP_FLAG_BB_START) {
			if (insn->op == IR_MERGE || insn->op == IR_BEGIN) {
				ir_bitqueue_add(&worklist2, i);
			}
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
			} else if (ir_is_dead_load_ex(ctx, i, flags, insn)) {
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
			if (IR_IS_CONST_OP(_values[i].op) || IR_IS_SYM_CONST(_values[i].op)) {
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
			ir_sccp_replace_insn(ctx, _values, i, j, &worklist2);
		} else if (IR_IS_SYM_CONST(value->op)) {
			/* replace instruction by constant */
			j = ir_const_ex(ctx, value->val, value->type, value->optx);
			ir_sccp_replace_insn(ctx, _values, i, j, &worklist2);
#if IR_COMBO_COPY_PROPAGATION
		} else if (value->op == IR_COPY) {
			ir_sccp_replace_insn(ctx, _values, i, value->op1, &worklist2);
#endif
		} else if (value->op == IR_TOP) {
			/* remove unreachable instruction */
			insn = &ctx->ir_base[i];
			if (ir_op_flags[insn->op] & (IR_OP_FLAG_DATA|IR_OP_FLAG_MEM)) {
				if (insn->op != IR_PARAM && (insn->op != IR_VAR || _values[insn->op1].op == IR_TOP)) {
					ir_sccp_remove_insn(ctx, _values, i, &worklist2);
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
				ir_sccp_replace_insn(ctx, _values, i, IR_UNUSED, &worklist2);
			}
		} else if (value->op == IR_IF) {
			/* remove one way IF/SWITCH */
			ir_sccp_remove_if(ctx, _values, i, value->op1);
		} else if (value->op == IR_MERGE) {
			/* schedule merge to remove unfeasible MERGE inputs */
			ir_bitqueue_add(&worklist, i);
		} else if (value->op == IR_LOAD) {
			/* schedule dead load elimination */
			ir_bitqueue_add(&worklist2, i);
		}
	}

	while ((i = ir_bitqueue_pop(&worklist)) >= 0) {
		IR_ASSERT(_values[i].op == IR_MERGE);
		ir_sccp_remove_unfeasible_merge_inputs(ctx, _values, i, _values[i].op1);
	}

	ctx->flags2 |= IR_CFG_REACHABLE;

	while ((i = ir_bitqueue_pop(&worklist2)) >= 0) {
		insn = &ctx->ir_base[i];
		if (IR_IS_FOLDABLE_OP(insn->op)) {
			if (ctx->use_lists[i].count == 0) {
				if (insn->op == IR_PHI) {
					ir_bitqueue_add(&worklist2, insn->op1);
				}
				ir_sccp_remove_insn2(ctx, i, &worklist2);
			} else {
				insn = &ctx->ir_base[i];
				switch (insn->op) {
					case IR_FP2FP:
						if (insn->type == IR_FLOAT) {
							if (ir_may_promote_d2f(ctx, insn->op1)) {
								ir_ref ref = ir_promote_d2f(ctx, insn->op1, i);
								insn->op1 = ref;
								ir_sccp_replace_insn2(ctx, i, ref, &worklist2);
								break;
							}
						} else {
							if (ir_may_promote_f2d(ctx, insn->op1)) {
								ir_ref ref = ir_promote_f2d(ctx, insn->op1, i);
								insn->op1 = ref;
								ir_sccp_replace_insn2(ctx, i, ref, &worklist2);
								break;
							}
						}
						goto folding;
					case IR_FP2INT:
						if (ctx->ir_base[insn->op1].type == IR_DOUBLE) {
							if (ir_may_promote_d2f(ctx, insn->op1)) {
								insn->op1 = ir_promote_d2f(ctx, insn->op1, i);
							}
						} else {
							if (ir_may_promote_f2d(ctx, insn->op1)) {
								insn->op1 = ir_promote_f2d(ctx, insn->op1, i);
							}
						}
						goto folding;
					case IR_TRUNC:
						if (ir_may_promote_i2i(ctx, insn->type, insn->op1)) {
							ir_ref ref = ir_promote_i2i(ctx, insn->type, insn->op1, i);
							insn->op1 = ref;
							ir_sccp_replace_insn2(ctx, i, ref, &worklist2);
							break;
						}
						goto folding;
					case IR_SEXT:
					case IR_ZEXT:
						if (ir_try_promote_ext(ctx, i, insn, &worklist2)) {
							break;
						}
						goto folding;
					case IR_PHI:
						break;
					default:
folding:
						ir_sccp_fold2(ctx, i, &worklist2);
						break;
				}
			}
		} else if (ir_op_flags[insn->op] & IR_OP_FLAG_BB_START) {
			if (!(ctx->flags & IR_OPT_CFG)) {
				/* pass */
			} else if (insn->op == IR_BEGIN) {
				if (ctx->ir_base[insn->op1].op == IR_END
				 && ctx->use_lists[i].count == 1) {
					ir_merge_blocks(ctx, insn->op1, i, &worklist2);
				}
			} else if (insn->op == IR_MERGE) {
				ir_optimize_merge(ctx, i, insn, &worklist2);
			}
		} else if (ir_is_dead_load(ctx, i)) {
			ir_ref next = ctx->use_edges[ctx->use_lists[i].refs];

			/* remove LOAD from double linked control list */
			ctx->ir_base[next].op1 = insn->op1;
			ir_use_list_replace_one(ctx, insn->op1, i, next);
			insn->op1 = IR_UNUSED;
			ir_sccp_remove_insn2(ctx, i, &worklist2);
		}
	}

	ir_mem_free(_values);
	ir_bitqueue_free(&worklist);
	ir_bitqueue_free(&worklist2);

	ctx->flags2 &= ~IR_OPT_IN_SCCP;
	ctx->flags2 |= IR_SCCP_DONE;

	return 1;
}
