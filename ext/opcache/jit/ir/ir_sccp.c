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

#define IR_COMBO_COPY_PROPAGATION 1

#define IR_TOP                  IR_UNUSED
#define IR_BOTTOM               IR_LAST_OP

#define IR_MAKE_TOP(ref)        do {IR_ASSERT(ref > 0); _values[ref].optx = IR_TOP;} while (0)
#define IR_MAKE_BOTTOM(ref)     do {IR_ASSERT(ref > 0); _values[ref].optx = IR_BOTTOM;} while (0)

#define IR_IS_TOP(ref)          (ref >= 0 && _values[ref].op == IR_TOP)
#define IR_IS_BOTTOM(ref)       (ref >= 0 && _values[ref].op == IR_BOTTOM)
#define IR_IS_REACHABLE(ref)    _ir_is_reachable_ctrl(ctx, _values, ref)
#define IR_IS_CONST(ref)        (IR_IS_CONST_REF(ref) || IR_IS_CONST_OP(_values[ref].op))

IR_ALWAYS_INLINE bool _ir_is_reachable_ctrl(ir_ctx *ctx, ir_insn *_values, ir_ref ref)
{
	IR_ASSERT(!IR_IS_CONST_REF(ref));
	IR_ASSERT(ir_op_flags[ctx->ir_base[ref].op] & IR_OP_FLAG_CONTROL);
	return _values[ref].op != IR_TOP; /* BOTTOM, IF or MERGE */
}

IR_ALWAYS_INLINE void ir_sccp_add_uses(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref)
{
	ir_use_list *use_list;
	ir_ref n, *p, use;

	IR_ASSERT(!IR_IS_CONST_REF(ref));
	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
		use = *p;
		if (_values[use].op != IR_BOTTOM) {
			ir_bitqueue_add(worklist, use);
		}
	}
}

IR_ALWAYS_INLINE void ir_sccp_add_input(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref)
{
	IR_ASSERT(!IR_IS_CONST_REF(ref));
	IR_ASSERT(_values[ref].op == IR_TOP);
	/* do backward propagaton only once */
	if (!_values[ref].op1) {
		_values[ref].op1 = 1;
		ir_bitqueue_add(worklist, ref);
	}
}

#if IR_COMBO_COPY_PROPAGATION
IR_ALWAYS_INLINE ir_ref ir_sccp_identity(ir_ctx *ctx, ir_insn *_values, ir_ref a)
{
	if (a > 0 && _values[a].op == IR_COPY) {
		do {
			a = _values[a].op1;
			IR_ASSERT(a > 0);
		} while (_values[a].op == IR_COPY);
		IR_ASSERT(_values[a].op == IR_BOTTOM);
	}
	return a;
}

#if 0
static void CHECK_LIST(ir_insn *_values, ir_ref ref)
{
	ir_ref member = _values[ref].op2;
	while (member != ref) {
		IR_ASSERT(_values[_values[member].op2].op3 == member);
		member = _values[member].op2;
	}
	IR_ASSERT(_values[_values[ref].op2].op3 == ref);
}
#else
# define CHECK_LIST(_values, ref)
#endif

static void ir_sccp_add_identity(ir_ctx *ctx, ir_insn *_values, ir_ref src, ir_ref dst)
{
	IR_ASSERT(dst > 0 && _values[dst].op != IR_BOTTOM && _values[dst].op != IR_COPY);
	IR_ASSERT((src > 0 && (_values[src].op == IR_BOTTOM || _values[src].op == IR_COPY)));
	IR_ASSERT(ir_sccp_identity(ctx, _values, src) != dst);

	_values[dst].optx = IR_COPY;
	_values[dst].op1 = src;

	if (_values[src].op == IR_BOTTOM) {
		/* initialize empty double-linked list */
		if (_values[src].op1 != src) {
			_values[src].op1 = src;
			_values[src].op2 = src;
			_values[src].op3 = src;
		}
	} else {
		src = ir_sccp_identity(ctx, _values, src);
	}

	/* insert into circular double-linked list */
	ir_ref prev = _values[src].op3;
	_values[dst].op2 = src;
	_values[dst].op3 = prev;
	_values[src].op3 = dst;
	_values[prev].op2 = dst;
	CHECK_LIST(_values, dst);
}

static void ir_sccp_split_partition(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref)
{
	ir_ref member, head, tail, next, prev;

	CHECK_LIST(_values, ref);
	IR_MAKE_BOTTOM(ref);
	_values[ref].op1 = ref;

	member = _values[ref].op2;
	head = tail = IR_UNUSED;
	while (member != ref) {
		if (_values[member].op != IR_BOTTOM) {
			ir_bitqueue_add(worklist, member);
		}
		ir_sccp_add_uses(ctx, _values, worklist, member);

		next = _values[member].op2;
		if (ir_sccp_identity(ctx, _values, member) == ref) {
			/* remove "member" from the old circular double-linked list */
			prev = _values[member].op3;
			_values[prev].op2 = next;
			_values[next].op3 = prev;

			/* insert "member" into the new double-linked list */
			if (!head) {
				head = tail = member;
			} else {
				_values[tail].op2 = member;
				_values[member].op3 = tail;
				tail = member;
			}
		}
		member = next;
	}

	/* remove "ref" from the old circular double-linked list */
	next = _values[ref].op2;
	prev = _values[ref].op3;
	_values[prev].op2 = next;
	_values[next].op3 = prev;
	CHECK_LIST(_values, next);

	/* close the new circle */
	if (head) {
		_values[ref].op2 = head;
		_values[ref].op3 = tail;
		_values[tail].op2 = ref;
		_values[head].op3 = ref;
	} else {
		_values[ref].op2 = ref;
		_values[ref].op3 = ref;
	}
	CHECK_LIST(_values, ref);
}

IR_ALWAYS_INLINE void ir_sccp_make_bottom_ex(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref)
{
	if (_values[ref].op == IR_COPY) {
		ir_sccp_split_partition(ctx, _values, worklist, ref);
	} else {
		IR_MAKE_BOTTOM(ref);
	}
}

# define IR_MAKE_BOTTOM_EX(ref) ir_sccp_make_bottom_ex(ctx, _values, worklist, ref)
#else
# define ir_sccp_identity(_ctx, _values, ref) (ref)
# define IR_MAKE_BOTTOM_EX(ref) IR_MAKE_BOTTOM(ref)
#endif

IR_ALWAYS_INLINE bool ir_sccp_meet_const(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref, ir_insn *val_insn)
{
	IR_ASSERT(IR_IS_CONST_OP(val_insn->op) || IR_IS_SYM_CONST(val_insn->op));

	if (_values[ref].op == IR_TOP) {
		/* TOP meet NEW_CONST => NEW_CONST */
		_values[ref].optx = val_insn->opt;
		_values[ref].val.u64 = val_insn->val.u64;
		return 1;
	} else if (_values[ref].opt == val_insn->opt) {
		/* OLD_CONST meet NEW_CONST => (OLD_CONST == NEW_CONST) ? OLD_CONST : BOTTOM */
		if (_values[ref].val.u64 == val_insn->val.u64) {
			return 0;
		}
	}

	IR_MAKE_BOTTOM_EX(ref);
	return 1;
}

IR_ALWAYS_INLINE bool ir_sccp_meet(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref, ir_ref val)
{
	ir_ref val_identity = ir_sccp_identity(ctx, _values, val);
	ir_insn *val_insn;

	if (IR_IS_CONST_REF(val_identity)) {
		val_insn = &ctx->ir_base[val_identity];
	} else {
		val_insn = &_values[val_identity];

		if (!IR_IS_CONST_OP(val_insn->op) && !IR_IS_SYM_CONST(val_insn->op)) {
#if IR_COMBO_COPY_PROPAGATION
			if (_values[ref].op == IR_COPY) {
				/* COPY(OLD_VAL) meet COPY(NEW_VAL) =>
				 *   (IDENTITY(OLD_VAL) == IDENTITY(NEW_VAL) ? COPY(OLD_VAL) ? BOTTOM */
				if (ir_sccp_identity(ctx, _values, ref) == val_identity) {
					return 0; /* not changed */
				}
				ir_sccp_split_partition(ctx, _values, worklist, ref);
				return 1;
			} else {
				IR_ASSERT(_values[ref].op != IR_BOTTOM);
				/* TOP       meet COPY(NEW_VAL) -> COPY(NEW_VAL) */
				/* OLD_CONST meet COPY(NEW_VAL) -> COPY(NEW_VAL) */
				ir_sccp_add_identity(ctx, _values, val, ref);
				return 1;
			}
#endif

			IR_MAKE_BOTTOM(ref);
			return 1;
		}
	}

	return ir_sccp_meet_const(ctx, _values, worklist, ref, val_insn);
}

static ir_ref ir_sccp_fold(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref ref, ir_insn *insn)
{
	ir_insn *op1_insn, *op2_insn, *op3_insn;
	ir_ref op1, op2, op3, copy;
	uint32_t opt = insn->opt;

	op1 = ir_sccp_identity(ctx, _values, insn->op1);
	op2 = ir_sccp_identity(ctx, _values, insn->op2);
	op3 = ir_sccp_identity(ctx, _values, insn->op3);

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
		case IR_FOLD_DO_CSE:
		case IR_FOLD_DO_EMIT:
			IR_MAKE_BOTTOM_EX(ref);
			return 1;
		case IR_FOLD_DO_COPY:
			copy = ctx->fold_insn.op1;
			return ir_sccp_meet(ctx, _values, worklist, ref, copy);
		case IR_FOLD_DO_CONST:
			return ir_sccp_meet_const(ctx, _values, worklist, ref, &ctx->fold_insn);
		default:
			IR_ASSERT(0);
			return 0;
	}
}

static bool ir_sccp_analyze_phi(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_ref i, ir_insn *insn)
{
	ir_ref j, n, input, *merge_input, *p;
	ir_insn *v, *new_const = NULL;
#if IR_COMBO_COPY_PROPAGATION
	ir_ref new_copy = IR_UNUSED;
	ir_ref new_copy_identity = IR_UNUSED;
	ir_ref phi_identity = ir_sccp_identity(ctx, _values, i);
#endif

	if (!IR_IS_REACHABLE(insn->op1)) {
		return 0;
	}
	n = insn->inputs_count;
	if (n > 3 && _values[i].op == IR_TOP) {
		for (j = 0; j < (n>>2); j++) {
			_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
		}
	}

	p = insn->ops + 2;
	merge_input = ctx->ir_base[insn->op1].ops + 1;
	for (; --n > 0; p++, merge_input++) {
		IR_ASSERT(*merge_input > 0);
		if (!IR_IS_REACHABLE(*merge_input)) {
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
				ir_sccp_add_input(ctx, _values, worklist, input);
				continue;
#if IR_COMBO_COPY_PROPAGATION
			} else if (v->op == IR_COPY) {
				input = v->op1;
				new_copy_identity = ir_sccp_identity(ctx, _values, input);
				if (new_copy_identity == phi_identity) {
					new_copy_identity = IR_UNUSED;
					continue;
				}
				new_copy = input;
				goto next;
#endif
			} else if (v->op == IR_BOTTOM) {
#if IR_COMBO_COPY_PROPAGATION
				if (input == phi_identity) {
					continue;
				}
				new_copy = new_copy_identity = input;
				goto next;
#else
				goto make_bottom;
#endif
			}
		}
		new_const = v;
		goto next;
	}

	return 0;

next:
	p++;
	merge_input++;
	/* for all live merge inputs */
	for (; --n > 0; p++, merge_input++) {
		IR_ASSERT(*merge_input > 0);
		if (!IR_IS_REACHABLE(*merge_input)) {
			continue;
		}

		input = *p;
		if (IR_IS_CONST_REF(input)) {
#if IR_COMBO_COPY_PROPAGATION
			if (new_copy) {
				goto make_bottom;
			}
#endif
			v = &ctx->ir_base[input];
		} else if (input == i) {
			continue;
		} else {
			v = &_values[input];
			if (v->op == IR_TOP) {
				ir_sccp_add_input(ctx, _values, worklist, input);
				continue;
#if IR_COMBO_COPY_PROPAGATION
			} else if (v->op == IR_COPY) {
				ir_ref identity = ir_sccp_identity(ctx, _values, v->op1);

				if (identity == phi_identity || identity == new_copy_identity) {
					continue;
				}
				goto make_bottom;
#endif
			} else if (v->op == IR_BOTTOM) {
#if IR_COMBO_COPY_PROPAGATION
				if (input  == phi_identity || input == new_copy_identity) {
					continue;
				}
#endif
				goto make_bottom;
			}
		}
		if (!new_const || new_const->opt != v->opt || new_const->val.u64 != v->val.u64) {
			goto make_bottom;
		}
	}

#if IR_COMBO_COPY_PROPAGATION
	if (new_copy) {
		return ir_sccp_meet(ctx, _values, worklist, i, new_copy);
	}
#endif

	return ir_sccp_meet_const(ctx, _values, worklist, i, new_const);

make_bottom:
	IR_MAKE_BOTTOM_EX(i);
	return 1;
}

static bool ir_is_dead_load_ex(ir_ctx *ctx, ir_ref ref, uint32_t flags, ir_insn *insn)
{
	if ((flags & (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_MASK)) == (IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)) {
		return ctx->use_lists[ref].count == 1;
	} else if (insn->op == IR_ALLOCA || insn->op == IR_BLOCK_BEGIN) {
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

#ifdef IR_SCCP_TRACE
static void ir_sccp_trace_val(ir_ctx *ctx, ir_insn *_values, ir_ref i)
{
	if (IR_IS_BOTTOM(i)) {
		fprintf(stderr, "BOTTOM");
	} else if (IR_IS_CONST_OP(_values[i].op) || IR_IS_SYM_CONST(_values[i].op)) {
		fprintf(stderr, "CONST(");
		ir_print_const(ctx, &_values[i], stderr, true);
		fprintf(stderr, ")");
#if IR_COMBO_COPY_PROPAGATION
	} else if (_values[i].op == IR_COPY) {
		fprintf(stderr, "COPY(%d)", _values[i].op1);
#endif
	} else if (IR_IS_TOP(i)) {
		fprintf(stderr, "TOP");
	} else if (_values[i].op == IR_IF) {
		fprintf(stderr, "IF(%d)", _values[i].op1);
	} else if (_values[i].op == IR_MERGE) {
		fprintf(stderr, "MERGE(%d)", _values[i].op1);
	} else {
		fprintf(stderr, "%d", _values[i].op);
	}
}

static void ir_sccp_trace_start(ir_ctx *ctx, ir_insn *_values, ir_ref i)
{
	fprintf(stderr, "%d. ", i);
	ir_sccp_trace_val(ctx, _values, i);
}

static void ir_sccp_trace_end(ir_ctx *ctx, ir_insn *_values, ir_ref i)
{
	fprintf(stderr, " -> ");
	ir_sccp_trace_val(ctx, _values, i);
	fprintf(stderr, "\n");
}
#else
# define ir_sccp_trace_start(c, v, i)
# define ir_sccp_trace_end(c, v, i)
#endif

static IR_NEVER_INLINE void ir_sccp_analyze(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_bitqueue *iter_worklist)
{
	ir_ref i, j, n, *p, use;
	ir_use_list *use_list;
	ir_insn *insn, *use_insn;
	uint32_t flags;

	/* A bit modified SCCP algorithm of M. N. Wegman and F. K. Zadeck */
	worklist->pos = 0;
	ir_bitset_incl(worklist->set, 1);
	for (; (i = ir_bitqueue_pop(worklist)) >= 0; ir_sccp_trace_end(ctx, _values, i)) {
		IR_ASSERT(_values[i].op != IR_BOTTOM);
		ir_sccp_trace_start(ctx, _values, i);
		insn = &ctx->ir_base[i];
		flags = ir_op_flags[insn->op];
		if (flags & IR_OP_FLAG_DATA) {
			if (ctx->use_lists[i].count == 0) {
				/* dead code */
				continue;
			} else if (insn->op == IR_PHI) {
				if (!ir_sccp_analyze_phi(ctx, _values, worklist, i, insn)) {
					continue;
				}
			} else if (EXPECTED(IR_IS_FOLDABLE_OP(insn->op))) {
				bool may_benefit = 0;
				bool has_top = 0;

				if (_values[i].op != IR_TOP) {
					may_benefit = 1;
				}

				IR_ASSERT(!IR_OP_HAS_VAR_INPUTS(flags));
				n = IR_INPUT_EDGES_COUNT(flags);
				for (p = insn->ops + 1; n > 0; p++, n--) {
					ir_ref input = *p;
					if (input > 0) {
						if (_values[input].op == IR_TOP) {
							has_top = 1;
							ir_sccp_add_input(ctx, _values, worklist, input);
						} else if (_values[input].op != IR_BOTTOM) {
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
					IR_MAKE_BOTTOM_EX(i);
					if (insn->op == IR_FP2FP || insn->op == IR_FP2INT || insn->op == IR_TRUNC) {
						ir_bitqueue_add(iter_worklist, i);
					}
				} else if (!ir_sccp_fold(ctx, _values, worklist, i, insn)) {
					/* not changed */
					continue;
				} else if (_values[i].op == IR_BOTTOM) {
					insn = &ctx->ir_base[i];
					if (insn->op == IR_FP2FP || insn->op == IR_FP2INT || insn->op == IR_TRUNC) {
						ir_bitqueue_add(iter_worklist, i);
					}
				}
			} else {
				IR_MAKE_BOTTOM_EX(i);
			}
		} else if (flags & IR_OP_FLAG_BB_START) {
			if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN || insn->op == IR_BEGIN) {
				ir_bitqueue_add(iter_worklist, i);
			}
			if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				ir_ref unfeasible_inputs = 0;

				n = insn->inputs_count;
				if (n > 3 && _values[i].op == IR_TOP) {
					for (j = 0; j < (n>>2); j++) {
						_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
					}
				}
				for (p = insn->ops + 1; n > 0; p++, n--) {
					ir_ref input = *p;
					IR_ASSERT(input > 0);
					if (!IR_IS_REACHABLE(input)) {
						unfeasible_inputs++;
					}
				}
				if (unfeasible_inputs == 0) {
					IR_MAKE_BOTTOM(i);
				} else if (_values[i].op != IR_MERGE || _values[i].op1 != unfeasible_inputs) {
					_values[i].optx = IR_MERGE;
					_values[i].op1 = unfeasible_inputs;
				} else {
					continue;
				}
				if (ctx->flags2 & IR_MEM2SSA_VARS) {
					/* MEM2SSA puts new PHI at the bottom, but we like to process them now */
					use_list = &ctx->use_lists[i];
					n = use_list->count;
					for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
						use = *p;
						if (_values[use].op != IR_BOTTOM) {
							if (ctx->ir_base[use].op == IR_PHI) {
								ir_bitqueue_del(worklist, use);
								if (ctx->use_lists[use].count != 0) {
									if (ir_sccp_analyze_phi(ctx, _values, worklist, use, &ctx->ir_base[use])) {
										ir_sccp_add_uses(ctx, _values, worklist, use);
									}
								}
							} else {
								ir_bitqueue_add(worklist, use);
							}
						}
					}
					continue;
				}
			} else {
				IR_ASSERT(insn->op == IR_START || IR_IS_REACHABLE(insn->op1));
				IR_MAKE_BOTTOM(i);
			}
		} else {
			IR_ASSERT(insn->op1 > 0);
			if (!IR_IS_REACHABLE(insn->op1)) {
				/* control inpt is not feasible */
				continue;
			}
			if (insn->op == IR_IF) {
				if (IR_IS_TOP(insn->op2)) {
					ir_sccp_add_input(ctx, _values, worklist, insn->op2);
					continue;
				}
				if (IR_IS_CONST(insn->op2)) {
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
					if (_values[i].op == IR_TOP) {
						_values[i].optx = IR_IF;
						_values[i].op1 = use;
						ir_bitqueue_add(worklist, use);
						continue;
					} else if (_values[i].op == IR_IF && _values[i].op1 == use) {
						continue;
					}
				}
				IR_MAKE_BOTTOM(i);
				ir_bitqueue_add(iter_worklist, i);
			} else if (insn->op == IR_SWITCH) {
				if (IR_IS_TOP(insn->op2)) {
					ir_sccp_add_input(ctx, _values, worklist, insn->op2);
					continue;
				}
				if (IR_IS_CONST(insn->op2)) {
					ir_ref use_case = IR_UNUSED;

					use_list = &ctx->use_lists[i];
					n = use_list->count;
					for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
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
						if (_values[i].op == IR_TOP) {
							_values[i].optx = IR_IF;
							_values[i].op1 = use_case;
							ir_bitqueue_add(worklist, use_case);
							continue;
						} else if (_values[i].op == IR_IF || _values[i].op1 == use_case) {
							continue;
						}
					}
				}
				IR_MAKE_BOTTOM(i);
			} else if (ir_is_dead_load_ex(ctx, i, flags, insn)) {
				/* schedule dead load elimination */
				ir_bitqueue_add(iter_worklist, i);
				IR_MAKE_BOTTOM(i);
			} else {
				if (_values[i].op == IR_TOP) {
					bool has_top = 0;

					/* control, call, load and store instructions may have unprocessed inputs */
					n = IR_INPUT_EDGES_COUNT(flags);
					if (IR_OP_HAS_VAR_INPUTS(flags) && (n = insn->inputs_count) > 3) {
						for (j = 0; j < (n>>2); j++) {
							_values[i+j+1].optx = IR_BOTTOM; /* keep the tail of a long multislot instruction */
						}
						for (j = 2, p = insn->ops + j; j <= n; j++, p++) {
							IR_ASSERT(IR_OPND_KIND(flags, j) == IR_OPND_DATA);
							use = *p;
							if (use > 0 && _values[use].op == IR_TOP) {
								has_top = 1;
								ir_sccp_add_input(ctx, _values, worklist, use);
							}
						}
					} else if (n >= 2) {
						IR_ASSERT(IR_OPND_KIND(flags, 2) == IR_OPND_DATA);
						use = insn->op2;
						if (use > 0 && _values[use].op == IR_TOP) {
							has_top = 1;
							ir_sccp_add_input(ctx, _values, worklist, use);
						}
						if (n > 2) {
							IR_ASSERT(n == 3);
							IR_ASSERT(IR_OPND_KIND(flags, 3) == IR_OPND_DATA);
							use = insn->op3;
							if (use > 0 && _values[use].op == IR_TOP) {
								has_top = 1;
								ir_sccp_add_input(ctx, _values, worklist, use);
							}
						}
					}

					if (has_top && !(flags & IR_OP_FLAG_BB_END)) {
						use = ir_next_control(ctx, i);
						if (_values[use].op == IR_TOP) {
							has_top = 1;
							/* do forward control propagaton only once */
							if (!_values[use].op1) {
								_values[use].op1 = 1;
								ir_bitqueue_add(worklist, use);
							}
						}
						continue;
					}
				}
				IR_MAKE_BOTTOM(i);
			}
		}
		ir_sccp_add_uses(ctx, _values, worklist, i);
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
}

/**********************/
/* SCCP trasformation */
/**********************/

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

static void ir_sccp_replace_insn(ir_ctx *ctx, ir_insn *_values, ir_ref ref, ir_ref new_ref, ir_bitqueue *worklist)
{
	ir_ref j, n, *p, use, i;
	ir_insn *insn;
	ir_use_list *use_list;

	IR_ASSERT(ref != new_ref);

	insn = &ctx->ir_base[ref];

#if IR_COMBO_COPY_PROPAGATION
	if ((ir_op_flags[insn->op] & IR_OP_FLAG_MEM) && IR_IS_REACHABLE(insn->op1)) {
		/* remove from control list */
		ir_ref prev = insn->op1;
		ir_ref next = ir_next_control(ctx, ref);
		ctx->ir_base[next].op1 = prev;
		ir_use_list_remove_one(ctx, ref, next);
		ir_use_list_replace_one(ctx, prev, ref, next);
		insn->op1 = IR_UNUSED;
	}
#endif

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
	p = &ctx->use_edges[use_list->refs];
	if (new_ref <= 0) {
		/* constant or IR_UNUSED */
		for (; n; p++, n--) {
			use = *p;
			/* we may skip nodes that are going to be removed by SCCP (TOP, CONST and COPY) */
			if (_values[use].op > IR_COPY) {
				insn = &ctx->ir_base[use];
				i = ir_insn_find_op(insn, ref);
				if (!i) continue;
				IR_ASSERT(i > 0);
				ir_insn_set_op(insn, i, new_ref);
				/* schedule folding */
				ir_bitqueue_add(worklist, use);
			}
		}
	} else {
		for (j = 0; j < n; j++, p++) {
			use = *p;
			/* we may skip nodes that are going to be removed by SCCP (TOP, CONST and COPY) */
			if (_values[use].op == IR_BOTTOM) {
				insn = &ctx->ir_base[use];
				i = ir_insn_find_op(insn, ref);
				IR_ASSERT(i > 0);
				ir_insn_set_op(insn, i, new_ref);
				if (ir_use_list_add(ctx, new_ref, use)) {
					/* restore after reallocation */
					use_list = &ctx->use_lists[ref];
					n = use_list->count;
					p = &ctx->use_edges[use_list->refs + j];
				}
				/* schedule folding */
				ir_bitqueue_add(worklist, use);
			}
		}
	}
	CLEAR_USES(ref);
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

static bool ir_sccp_remove_unfeasible_merge_inputs(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	ir_ref old_merge_inputs, new_merge_inputs, i, *p;
	ir_use_list *use_list;
	ir_bitset life_inputs;
	ir_bitset_base_t holder = 0;

	IR_ASSERT(insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN);
	old_merge_inputs = insn->inputs_count;
	new_merge_inputs = 0;
	life_inputs = (old_merge_inputs < IR_BITSET_BITS) ? &holder : ir_bitset_malloc(old_merge_inputs + 1);

	for (i = 1; i <= old_merge_inputs; i++) {
		ir_ref input = ir_insn_op(insn, i);

		if (input) {
			new_merge_inputs++;
			if (new_merge_inputs != i) {
				ir_insn_set_op(insn, new_merge_inputs, input);
			}
			ir_bitset_incl(life_inputs, i);
		}
	}

	if (new_merge_inputs == old_merge_inputs) {
		/* All inputs are feasible */
		if (life_inputs != &holder) {
			ir_mem_free(life_inputs);
		}
		return 0;
	}

	for (i = new_merge_inputs + 1; i <= old_merge_inputs; i++) {
		ir_insn_set_op(insn, i, IR_UNUSED);
	}

	if (new_merge_inputs <= 1) {
#if 0
		if (new_merge_inputs == 1
		 && insn->op == IR_LOOP_BEGIN
		 && insn->op1 > ref) { // TODO: check dominance instead of order
			/* dead loop */
			ir_use_list_remove_one(ctx, insn->op1, ref);
			insn->op1 = IR_UNUSED;
			new_merge_inputs = 0;
		}
#endif
		insn->optx = IR_OPTX(IR_BEGIN, IR_VOID, 1);
		ir_bitqueue_add(worklist, ref);
	} else {
		insn->inputs_count = new_merge_inputs;
	}

	/* Update PHIs */
	use_list = &ctx->use_lists[ref];
	if (use_list->count > 1) {
		ir_ref use_count = 0;
		ir_ref *q;

		for (i = 0, p = q = &ctx->use_edges[use_list->refs]; i < use_list->count; p++, i++) {
			ir_ref use = *p;
			ir_insn *use_insn = &ctx->ir_base[use];

			if (use_insn->op == IR_PHI) {
				ir_ref j, k;

				/* compress PHI */
				for (j = k = 1; j <= old_merge_inputs; j++) {
					ir_ref input = ir_insn_op(use_insn, j + 1);

					if (ir_bitset_in(life_inputs, j)) {
						IR_ASSERT(input);
						if (k != j) {
							ir_insn_set_op(use_insn, k + 1, input);
						}
						k++;
					} else if (input > 0) {
						ir_use_list_remove_one(ctx, input, use);
					}
				}
				while (k <= old_merge_inputs) {
					k++;
					ir_insn_set_op(use_insn, k, IR_UNUSED);
				}

				if (new_merge_inputs == 0) {
					/* remove PHI */
#if 0
					use_insn->op1 = IR_UNUSED;
					ir_iter_remove_insn(ctx, use, worklist);
#else
					IR_ASSERT(0);
#endif
					continue;
				} else if (new_merge_inputs == 1) {
					/* replace PHI by COPY */
					use_insn->optx = IR_OPTX(IR_COPY, use_insn->type, 1);
					use_insn->op1 = use_insn->op2;
					use_insn->op2 = IR_UNUSED;
					ir_bitqueue_add(worklist, use);
					continue;
				} else {
					use_insn->inputs_count = new_merge_inputs + 1;
				}
			}
			if (p != q) {
				*q = use;
			}
			q++;
			use_count++;
		}
		for (i = use_count; i < use_list->count; q++, i++) {
			*q = IR_UNUSED;
		}
		use_list->count = use_count;
	}

	if (life_inputs != &holder) {
		ir_mem_free(life_inputs);
	}

	return 1;
}

static IR_NEVER_INLINE void ir_sccp_transform(ir_ctx *ctx, ir_insn *_values, ir_bitqueue *worklist, ir_bitqueue *iter_worklist)
{
	ir_ref i, j;
	ir_insn *value;

	for (i = 1, value = _values + i; i < ctx->insns_count; value++, i++) {
		if (value->op == IR_BOTTOM) {
			continue;
		} else if (IR_IS_CONST_OP(value->op)) {
			/* replace instruction by constant */
			j = ir_const(ctx, value->val, value->type);
			ir_sccp_replace_insn(ctx, _values, i, j, iter_worklist);
		} else if (IR_IS_SYM_CONST(value->op)) {
			/* replace instruction by constant */
			j = ir_const_ex(ctx, value->val, value->type, value->optx);
			ir_sccp_replace_insn(ctx, _values, i, j, iter_worklist);
#if IR_COMBO_COPY_PROPAGATION
		} else if (value->op == IR_COPY) {
			ir_sccp_replace_insn(ctx, _values, i, ir_sccp_identity(ctx, _values, value->op1), iter_worklist);
#endif
		} else if (value->op == IR_TOP) {
			/* remove unreachable instruction */
			ir_insn *insn = &ctx->ir_base[i];

			if (insn->op == IR_NOP) {
				/* already removed */
			} else if (ir_op_flags[insn->op] & (IR_OP_FLAG_DATA|IR_OP_FLAG_MEM)) {
				if (insn->op != IR_PARAM) {
					ir_sccp_remove_insn(ctx, _values, i, iter_worklist);
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
				ir_sccp_replace_insn(ctx, _values, i, IR_UNUSED, iter_worklist);
			}
		} else if (value->op == IR_IF) {
			/* remove one way IF/SWITCH */
			ir_sccp_remove_if(ctx, _values, i, value->op1);
		} else if (value->op == IR_MERGE) {
			/* schedule merge to remove unfeasible MERGE inputs */
			ir_bitqueue_add(worklist, i);
		}
	}

	while ((i = ir_bitqueue_pop(worklist)) >= 0) {
		IR_ASSERT(_values[i].op == IR_MERGE);
		ir_sccp_remove_unfeasible_merge_inputs(ctx, i, &ctx->ir_base[i], iter_worklist);
	}
}

/***************************/
/* Iterative Optimizations */
/***************************/

/* Modification of some instruction may open new optimization oprtunities for other
 * instructions that use this one.
 *
 * For example, let "a = ADD(x, y)" became "a = ADD(x, C1)". In case we also have
 * "b = ADD(a, C2)" we may optimize it into "b = ADD(x, C1 + C2)" and then might
 * also remove "a".
 *
 * This implementation supports only few optimization of combinations from ir_fold.h
 *
 * TODO: Think abput a more general solution ???
 */
static void ir_iter_add_related_uses(ir_ctx *ctx, ir_ref ref, ir_bitqueue *worklist)
{
	ir_insn *insn = &ctx->ir_base[ref];

	if (insn->op == IR_ADD || insn->op == IR_SUB) {
		ir_use_list *use_list = &ctx->use_lists[ref];

		if (use_list->count == 1) {
			ir_ref use = ctx->use_edges[use_list->refs];
			ir_insn *use_insn = &ctx->ir_base[ref];

			if (use_insn->op == IR_ADD || use_insn->op == IR_SUB) {
				ir_bitqueue_add(worklist, use);
			}
		}
	}
}

static void ir_iter_remove_insn(ir_ctx *ctx, ir_ref ref, ir_bitqueue *worklist)
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

void ir_iter_replace(ir_ctx *ctx, ir_ref ref, ir_ref new_ref, ir_bitqueue *worklist)
{
	ir_ref i, j, n, *p, use;
	ir_insn *insn;
	ir_use_list *use_list;

	IR_ASSERT(ref != new_ref);

	use_list = &ctx->use_lists[ref];
	n = use_list->count;
	p = &ctx->use_edges[use_list->refs];
	if (new_ref <= 0) {
		/* constant or IR_UNUSED */
		for (; n; p++, n--) {
			use = *p;
			IR_ASSERT(use != ref);
			insn = &ctx->ir_base[use];
			i = ir_insn_find_op(insn, ref);
			IR_ASSERT(i > 0);
			ir_insn_set_op(insn, i, new_ref);
			/* schedule folding */
			ir_bitqueue_add(worklist, use);
			ir_iter_add_related_uses(ctx, use, worklist);
		}
	} else {
		for (j = 0; j < n; j++, p++) {
			use = *p;
			IR_ASSERT(use != ref);
			insn = &ctx->ir_base[use];
			i = ir_insn_find_op(insn, ref);
			IR_ASSERT(i > 0);
			ir_insn_set_op(insn, i, new_ref);
			if (ir_use_list_add(ctx, new_ref, use)) {
				/* restore after reallocation */
				use_list = &ctx->use_lists[ref];
				n = use_list->count;
				p = &ctx->use_edges[use_list->refs + j];
			}
			/* schedule folding */
			ir_bitqueue_add(worklist, use);
		}
	}
}

static void ir_iter_replace_insn(ir_ctx *ctx, ir_ref ref, ir_ref new_ref, ir_bitqueue *worklist)
{
	ir_ref j, n, *p;
	ir_insn *insn;

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
				ir_bitqueue_add(worklist, ctx->ir_base[input].op1);
			}
		}
	}

	ir_iter_replace(ctx, ref, new_ref, worklist);

	CLEAR_USES(ref);
}

void ir_iter_update_op(ir_ctx *ctx, ir_ref ref, uint32_t idx, ir_ref new_val, ir_bitqueue *worklist)
{
	ir_insn *insn = &ctx->ir_base[ref];
	ir_ref old_val = ir_insn_op(insn, idx);

	IR_ASSERT(old_val != new_val);
	if (!IR_IS_CONST_REF(new_val)) {
		ir_use_list_add(ctx, new_val, ref);
	}
	ir_insn_set_op(insn, idx, new_val);
	if (!IR_IS_CONST_REF(old_val)) {
		ir_use_list_remove_one(ctx, old_val, ref);
		if (ir_is_dead(ctx, old_val)) {
			/* schedule DCE */
			ir_bitqueue_add(worklist, old_val);
		}
	}
}

static ir_ref ir_iter_find_cse1(ir_ctx *ctx, uint32_t optx, ir_ref op1)
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

static ir_ref ir_iter_find_cse(ir_ctx *ctx, ir_ref ref, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3, ir_bitqueue *worklist)
{
	uint32_t n = IR_INPUT_EDGES_COUNT(ir_op_flags[opt & IR_OPT_OP_MASK]);
	ir_use_list *use_list = NULL;
	ir_ref *p, use;
	ir_insn *use_insn;

	if (n == 2) {
		if (!IR_IS_CONST_REF(op1)) {
			use_list = &ctx->use_lists[op1];
		}
		if (!IR_IS_CONST_REF(op2) && (!use_list || use_list->count > ctx->use_lists[op2].count)) {
			use_list = &ctx->use_lists[op2];
		}
		if (use_list) {
			n = use_list->count;
			for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
				use = *p;
				if (use != ref) {
					use_insn = &ctx->ir_base[use];
					if (use_insn->opt == opt && use_insn->op1 == op1 && use_insn->op2 == op2) {
						IR_ASSERT(use_insn->op3 == op3);
						if (use < ref) {
							return use;
						} else {
							ir_bitqueue_add(worklist, use);
						}
					}
				}
			}
		}
	 } else if (n < 2) {
		IR_ASSERT(n == 1);
		if (!IR_IS_CONST_REF(op1)) {
			use_list = &ctx->use_lists[op1];
			n = use_list->count;
			for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
				use = *p;
				if (use != ref) {
					use_insn = &ctx->ir_base[use];
					if (use_insn->opt == opt) {
						IR_ASSERT(use_insn->op1 == op1);
						IR_ASSERT(use_insn->op2 == op2);
						IR_ASSERT(use_insn->op3 == op3);
						if (use < ref) {
							return use;
						} else {
							ir_bitqueue_add(worklist, use);
						}
					}
				}
			}
		}
	} else {
		IR_ASSERT(n == 3);
		if (!IR_IS_CONST_REF(op1)) {
			use_list = &ctx->use_lists[op1];
		}
		if (!IR_IS_CONST_REF(op2) && (!use_list || use_list->count > ctx->use_lists[op2].count)) {
			use_list = &ctx->use_lists[op2];
		}
		if (!IR_IS_CONST_REF(op3) && (!use_list || use_list->count > ctx->use_lists[op3].count)) {
			use_list = &ctx->use_lists[op3];
		}
		if (use_list) {
			n = use_list->count;
			for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
				use = *p;
				if (use != ref) {
					use_insn = &ctx->ir_base[use];
					if (use_insn->opt == opt && use_insn->op1 == op1 && use_insn->op2 == op2 && use_insn->op3 == op3) {
						if (use < ref) {
							return use;
						} else {
							ir_bitqueue_add(worklist, use);
						}
					}
				}
			}
		}
	}
	return IR_UNUSED;
}

static void ir_iter_fold(ir_ctx *ctx, ir_ref ref, ir_bitqueue *worklist)
{
	uint32_t opt;
	ir_ref op1, op2, op3, copy;
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
		case IR_FOLD_DO_CSE:
			copy = ir_iter_find_cse(ctx, ref, ctx->fold_insn.opt,
				ctx->fold_insn.op1, ctx->fold_insn.op2, ctx->fold_insn.op3, worklist);
			if (copy) {
				ir_iter_replace_insn(ctx, ref, copy, worklist);
				break;
			}
			IR_FALLTHROUGH;
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
			ir_iter_replace_insn(ctx, ref, op1, worklist);
			break;
		case IR_FOLD_DO_CONST:
			op1 = ir_const(ctx, ctx->fold_insn.val, ctx->fold_insn.type);
			ir_iter_replace_insn(ctx, ref, op1, worklist);
			break;
		default:
			IR_ASSERT(0);
			break;
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

static ir_ref ir_promote_d2f(ir_ctx *ctx, ir_ref ref, ir_ref use, ir_bitqueue *worklist)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;

	IR_ASSERT(insn->type == IR_DOUBLE);
	if (IR_IS_CONST_REF(ref)) {
		return ir_const_float(ctx, (float)insn->val.d);
	} else {
		ir_bitqueue_add(worklist, ref);
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
				insn->op1 = ir_promote_d2f(ctx, insn->op1, ref, worklist);
				insn->type = IR_FLOAT;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_d2f(ctx, insn->op1, ref, worklist);
				} else {
					insn->op1 = ir_promote_d2f(ctx, insn->op1, ref, worklist);
					insn->op2 = ir_promote_d2f(ctx, insn->op2, ref, worklist);
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

static ir_ref ir_promote_f2d(ir_ctx *ctx, ir_ref ref, ir_ref use, ir_bitqueue *worklist)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;
	ir_ref old_ref;

	IR_ASSERT(insn->type == IR_FLOAT);
	if (IR_IS_CONST_REF(ref)) {
		return ir_const_double(ctx, (double)insn->val.f);
	} else {
		ir_bitqueue_add(worklist, ref);
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
				old_ref = ir_iter_find_cse1(ctx, IR_OPTX(IR_INT2FP, IR_DOUBLE, 1), insn->op1);
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
				insn->op1 = ir_promote_f2d(ctx, insn->op1, ref, worklist);
				insn->type = IR_DOUBLE;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
//			case IR_DIV:
			case IR_MIN:
			case IR_MAX:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_f2d(ctx, insn->op1, ref, worklist);
				} else {
					insn->op1 = ir_promote_f2d(ctx, insn->op1, ref, worklist);
					insn->op2 = ir_promote_f2d(ctx, insn->op2, ref, worklist);
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

static bool ir_may_promote_trunc(ir_ctx *ctx, ir_type type, ir_ref ref)
{
	ir_insn *insn = &ctx->ir_base[ref];
	ir_ref *p, n, input;

	if (IR_IS_CONST_REF(ref)) {
		return !IR_IS_SYM_CONST(insn->op);
	} else {
		switch (insn->op) {
			case IR_ZEXT:
			case IR_SEXT:
			case IR_TRUNC:
				return ctx->ir_base[insn->op1].type == type || ctx->use_lists[ref].count == 1;
			case IR_NEG:
			case IR_ABS:
			case IR_NOT:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_trunc(ctx, type, insn->op1);
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_MIN:
			case IR_MAX:
			case IR_OR:
			case IR_AND:
			case IR_XOR:
			case IR_SHL:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_trunc(ctx, type, insn->op1) &&
					ir_may_promote_trunc(ctx, type, insn->op2);
//			case IR_SHR:
//			case IR_SAR:
//			case IR_DIV:
//			case IR_MOD:
//			case IR_FP2INT:
//				TODO: ???
			case IR_COND:
				return ctx->use_lists[ref].count == 1 &&
					ir_may_promote_trunc(ctx, type, insn->op2) &&
					ir_may_promote_trunc(ctx, type, insn->op3);
			case IR_PHI:
				if (ctx->use_lists[ref].count != 1) {
					ir_use_list *use_list = &ctx->use_lists[ref];
					ir_ref count = 0;

					for (p = &ctx->use_edges[use_list->refs], n = use_list->count; n > 0; p++, n--) {
						if (*p != ref) {
							if (count) {
								return 0;
							}
							count = 1;
						}
					}
				}
				for (p = insn->ops + 2, n = insn->inputs_count - 1; n > 0; p++, n--) {
					input = *p;
					if (input != ref) {
						if (!ir_may_promote_trunc(ctx, type, input)) {
							return 0;
						}
					}
				}
				return 1;
			default:
				break;
		}
	}
	return 0;
}

static ir_ref ir_promote_i2i(ir_ctx *ctx, ir_type type, ir_ref ref, ir_ref use, ir_bitqueue *worklist)
{
	ir_insn *insn = &ctx->ir_base[ref];
	uint32_t count;
	ir_ref *p, n, input;

	if (IR_IS_CONST_REF(ref)) {
		ir_val val;

		switch (type) {
			case IR_I8:  val.i64 = insn->val.i8; break;
			case IR_U8:  val.u64 = insn->val.u8; break;
			case IR_I16: val.i64 = insn->val.i16; break;
			case IR_U16: val.u64 = insn->val.u16; break;
			case IR_I32: val.i64 = insn->val.i32; break;
			case IR_U32: val.u64 = insn->val.u32; break;
			case IR_CHAR:val.i64 = insn->val.i8; break;
			case IR_BOOL:val.u64 = insn->val.u8 != 0; break;
			default: IR_ASSERT(0); val.u64 = 0;
		}
		return ir_const(ctx, val, type);
	} else {
		ir_bitqueue_add(worklist, ref);
		switch (insn->op) {
			case IR_ZEXT:
			case IR_SEXT:
			case IR_TRUNC:
				if (ctx->ir_base[insn->op1].type != type) {
					ir_type src_type = ctx->ir_base[insn->op1].type;
					if (ir_type_size[src_type] == ir_type_size[type]) {
						insn->op = IR_BITCAST;
					} else if (ir_type_size[src_type] > ir_type_size[type]) {
						insn->op = IR_TRUNC;
					} else {
						if (insn->op != IR_SEXT && insn->op != IR_ZEXT) {
							insn->op = IR_IS_TYPE_SIGNED(type) ? IR_SEXT : IR_ZEXT;
						}
					}
					insn->type = type;
					return ref;
				}

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
				insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref, worklist);
				insn->type = type;
				return ref;
			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_MIN:
			case IR_MAX:
			case IR_OR:
			case IR_AND:
			case IR_XOR:
			case IR_SHL:
				if (insn->op1 == insn->op2) {
					insn->op2 = insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref, worklist);
				} else {
					insn->op1 = ir_promote_i2i(ctx, type, insn->op1, ref, worklist);
					insn->op2 = ir_promote_i2i(ctx, type, insn->op2, ref, worklist);
				}
				insn->type = type;
				return ref;
//			case IR_DIV:
//			case IR_MOD:
//			case IR_SHR:
//			case IR_SAR:
//			case IR_FP2INT:
//				TODO: ???
			case IR_COND:
				if (insn->op2 == insn->op3) {
					insn->op3 = insn->op2 = ir_promote_i2i(ctx, type, insn->op2, ref, worklist);
				} else {
					insn->op2 = ir_promote_i2i(ctx, type, insn->op2, ref, worklist);
					insn->op3 = ir_promote_i2i(ctx, type, insn->op3, ref, worklist);
				}
				insn->type = type;
				return ref;
			case IR_PHI:
				for (p = insn->ops + 2, n = insn->inputs_count - 1; n > 0; p++, n--) {
					input = *p;
					if (input != ref) {
						*p = ir_promote_i2i(ctx, type, input, ref, worklist);
					}
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
		ref = ir_iter_find_cse1(ctx, optx, src_ref);
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
	ir_use_list_add(ctx, ref, var_ref);
	if (!IR_IS_CONST_REF(src_ref)) {
		ir_use_list_replace_one(ctx, src_ref, var_ref, ref);
	}
	ir_bitqueue_grow(worklist, ref + 1);
	ir_bitqueue_add(worklist, ref);
	return ref;
}

static uint32_t _ir_estimated_control(ir_ctx *ctx, ir_ref val)
{
	ir_insn *insn;
	ir_ref n, *p, input, result, ctrl;

	if (IR_IS_CONST_REF(val)) {
		return 1; /* IR_START */
	}

	insn = &ctx->ir_base[val];
	if (ir_op_flags[insn->op] & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM)) {
		return val;
	}

	IR_ASSERT(ir_op_flags[insn->op] & IR_OP_FLAG_DATA);
	if (IR_OPND_KIND(ir_op_flags[insn->op], 1) == IR_OPND_CONTROL_DEP) {
		return insn->op1;
	}

	n = insn->inputs_count;
	p = insn->ops + 1;

	result = 1;
	for (; n > 0; p++, n--) {
		input = *p;
		ctrl = _ir_estimated_control(ctx, input);
		if (ctrl > result) { // TODO: check dominance depth instead of order
			result = ctrl;
		}
	}
	return result;
}

static bool ir_is_loop_invariant(ir_ctx *ctx, ir_ref ref, ir_ref loop)
{
	ref = _ir_estimated_control(ctx, ref);
	return ref < loop; // TODO: check dominance instead of order
}

static bool ir_is_cheaper_ext(ir_ctx *ctx, ir_ref ref, ir_ref loop, ir_ref ext_ref, ir_op op)
{
	if (IR_IS_CONST_REF(ref)) {
		return 1;
	} else {
		ir_insn *insn = &ctx->ir_base[ref];

		if (insn->op == IR_LOAD) {
			if (ir_is_loop_invariant(ctx, ref, loop)) {
				return 1;
			} else {
				/* ZEXT(LOAD(_, _)) costs the same as LOAD(_, _) */
				if (ctx->use_lists[ref].count == 2) {
					return 1;
				} else if (ctx->use_lists[ref].count == 3) {
					ir_use_list *use_list = &ctx->use_lists[ref];
					ir_ref *p, n, use;

					for (p = &ctx->use_edges[use_list->refs], n = use_list->count; n > 0; p++, n--) {
						use = *p;
						if (use != ext_ref) {
							ir_insn *use_insn = &ctx->ir_base[use];

							if (use_insn->op != op
							 && (!(ir_op_flags[use_insn->op] & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM))
							  || use_insn->op1 != ref)) {
								return 0;
							}
						}
					}
					return 1;
				}
			}
			return 0;
		} else {
			return ir_is_loop_invariant(ctx, ref, loop);
		}
	}
}

static bool ir_try_promote_induction_var_ext(ir_ctx *ctx, ir_ref ext_ref, ir_ref phi_ref, ir_ref op_ref, ir_bitqueue *worklist)
{
	ir_op op = ctx->ir_base[ext_ref].op;
	ir_type type = ctx->ir_base[ext_ref].type;
	ir_insn *phi_insn;
	ir_use_list *use_list;
	ir_ref n, *p, use, ext_ref_2 = IR_UNUSED;

	/* Check if we may change the type of the induction variable */
	use_list = &ctx->use_lists[phi_ref];
	n = use_list->count;
	if (n > 1) {
		for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
			use = *p;
			if (use == op_ref || use == ext_ref) {
				continue;
			} else {
				ir_insn *use_insn = &ctx->ir_base[use];

				if (use_insn->op >= IR_EQ && use_insn->op <= IR_UGT) {
					if (use_insn->op1 == phi_ref) {
						if (IR_IS_TYPE_SIGNED(type) != IR_IS_TYPE_SIGNED(ctx->ir_base[use_insn->op2].type)) {
							return 0;
						}
						if (ir_is_cheaper_ext(ctx, use_insn->op2, ctx->ir_base[phi_ref].op1, ext_ref, op)) {
							continue;
					    }
					} else if (use_insn->op2 == phi_ref) {
						if (IR_IS_TYPE_SIGNED(type) != IR_IS_TYPE_SIGNED(ctx->ir_base[use_insn->op1].type)) {
							return 0;
						}
						if (ir_is_cheaper_ext(ctx, use_insn->op1, ctx->ir_base[phi_ref].op1, ext_ref, op)) {
							continue;
					    }
					}
					return 0;
				} else if (use_insn->op == IR_IF) {
					continue;
				} else if (!ext_ref_2 && use_insn->op == op && use_insn->type == type) {
					ext_ref_2 = use;
					continue;
				} else {
					return 0;
				}
			}
		}
	}

	use_list = &ctx->use_lists[op_ref];
	n = use_list->count;
	if (n > 1) {
		for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
			use = *p;
			if (use == phi_ref || use == ext_ref) {
				continue;
			} else {
				ir_insn *use_insn = &ctx->ir_base[use];

				if (use_insn->op >= IR_EQ && use_insn->op <= IR_UGT) {
					if (use_insn->op1 == phi_ref) {
						if (IR_IS_TYPE_SIGNED(type) != IR_IS_TYPE_SIGNED(ctx->ir_base[use_insn->op2].type)) {
							return 0;
						}
						if (ir_is_cheaper_ext(ctx, use_insn->op2, ctx->ir_base[phi_ref].op1, ext_ref, op)) {
							continue;
					    }
					} else if (use_insn->op2 == phi_ref) {
						if (IR_IS_TYPE_SIGNED(type) != IR_IS_TYPE_SIGNED(ctx->ir_base[use_insn->op1].type)) {
							return 0;
						}
						if (ir_is_cheaper_ext(ctx, use_insn->op1, ctx->ir_base[phi_ref].op1, ext_ref, op)) {
							continue;
					    }
					}
					return 0;
				} else if (use_insn->op == IR_IF) {
					continue;
				} else if (!ext_ref_2 && use_insn->op == op && use_insn->type == type) {
					ext_ref_2 = use;
					continue;
				} else {
					return 0;
				}
			}
		}
	}

	for (n = 0; n < ctx->use_lists[phi_ref].count; n++) {
		/* "use_lists" may be reallocated by ir_ext_ref() */
		use = ctx->use_edges[ctx->use_lists[phi_ref].refs + n];
		if (use == ext_ref) {
			continue;
		} else {
			ir_insn *use_insn = &ctx->ir_base[use];

			if (use_insn->op == IR_IF) {
				continue;
			} else if (use_insn->op == op) {
				IR_ASSERT(ext_ref_2 == use);
				continue;
			}
			IR_ASSERT(((use_insn->op >= IR_EQ && use_insn->op <= IR_UGT)
			  || use_insn->op == IR_ADD || use_insn->op == IR_SUB || use_insn->op == IR_MUL)
			 && (use_insn->op1 == phi_ref || use_insn->op2 == phi_ref));
			if (use_insn->op1 != phi_ref) {
				if (IR_IS_CONST_REF(use_insn->op1)
				 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op1].op)) {
					ctx->ir_base[use].op1 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op1], op, type);
				} else {
					ctx->ir_base[use].op1 = ir_ext_ref(ctx, use, use_insn->op1, op, type, worklist);
				}
				ir_bitqueue_add(worklist, use);
			}
			if (use_insn->op2 != phi_ref) {
				if (IR_IS_CONST_REF(use_insn->op2)
				 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op2].op)) {
					ctx->ir_base[use].op2 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op2], op, type);
				} else {
					ctx->ir_base[use].op2 = ir_ext_ref(ctx, use, use_insn->op2, op, type, worklist);
				}
				ir_bitqueue_add(worklist, use);
			}
		}
	}

	if (ctx->use_lists[op_ref].count > 1) {
		for (n = 0; n < ctx->use_lists[op_ref].count; n++) {
			/* "use_lists" may be reallocated by ir_ext_ref() */
			use = ctx->use_edges[ctx->use_lists[op_ref].refs + n];
			if (use == ext_ref || use == phi_ref) {
				continue;
			} else {
				ir_insn *use_insn = &ctx->ir_base[use];

				if (use_insn->op == IR_IF) {
					continue;
				} else if (use_insn->op == op) {
					IR_ASSERT(ext_ref_2 == use);
					continue;
				}
				IR_ASSERT(use_insn->op >= IR_EQ && use_insn->op <= IR_UGT);
				if (use_insn->op1 != op_ref) {
					if (IR_IS_CONST_REF(use_insn->op1)
					 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op1].op)) {
						ctx->ir_base[use].op1 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op1], op, type);
					} else {
						ctx->ir_base[use].op1 = ir_ext_ref(ctx, use, use_insn->op1, op, type, worklist);
					}
					ir_bitqueue_add(worklist, use);
				}
				if (use_insn->op2 != op_ref) {
					if (IR_IS_CONST_REF(use_insn->op2)
					 && !IR_IS_SYM_CONST(ctx->ir_base[use_insn->op2].op)) {
						ctx->ir_base[use].op2 = ir_ext_const(ctx, &ctx->ir_base[use_insn->op2], op, type);
					} else {
						ctx->ir_base[use].op2 = ir_ext_ref(ctx, use, use_insn->op2, op, type, worklist);
					}
					ir_bitqueue_add(worklist, use);
				}
			}
		}
	}

	ir_iter_replace_insn(ctx, ext_ref, ctx->ir_base[ext_ref].op1, worklist);

	if (ext_ref_2) {
		ir_iter_replace_insn(ctx, ext_ref_2, ctx->ir_base[ext_ref_2].op1, worklist);
	}

	ctx->ir_base[op_ref].type = type;

	phi_insn = &ctx->ir_base[phi_ref];
	phi_insn->type = type;
	if (IR_IS_CONST_REF(phi_insn->op2)
	 && !IR_IS_SYM_CONST(ctx->ir_base[phi_insn->op2].op)) {
		ctx->ir_base[phi_ref].op2 = ir_ext_const(ctx, &ctx->ir_base[phi_insn->op2], op, type);
	} else {
		ctx->ir_base[phi_ref].op2 = ir_ext_ref(ctx, phi_ref, phi_insn->op2, op, type, worklist);
	}

	return 1;
}

static bool ir_try_promote_ext(ir_ctx *ctx, ir_ref ext_ref, ir_insn *insn, ir_bitqueue *worklist)
 {
	ir_ref ref = insn->op1;

	/* Check for simple induction variable in the form: x2 = PHI(loop, x1, x3); x3 = ADD(x2, _); */
	insn = &ctx->ir_base[ref];
	if (insn->op == IR_PHI
	 && insn->inputs_count == 3 /* (2 values) */
	 && ctx->ir_base[insn->op1].op == IR_LOOP_BEGIN) {
		ir_ref op_ref = insn->op3;
		ir_insn *op_insn = &ctx->ir_base[op_ref];

		if (op_insn->op == IR_ADD || op_insn->op == IR_SUB || op_insn->op == IR_MUL) {
			if (op_insn->op1 == ref) {
				if (ir_is_loop_invariant(ctx, op_insn->op2, insn->op1)) {
					return ir_try_promote_induction_var_ext(ctx, ext_ref, ref, op_ref, worklist);
				}
			} else if (op_insn->op2 == ref) {
				if (ir_is_loop_invariant(ctx, op_insn->op1, insn->op1)) {
					return ir_try_promote_induction_var_ext(ctx, ext_ref, ref, op_ref, worklist);
				}
			}
		}
	} else if (insn->op == IR_ADD || insn->op == IR_SUB || insn->op == IR_MUL) {
		if (!IR_IS_CONST_REF(insn->op1)
		 && ctx->ir_base[insn->op1].op == IR_PHI
		 && ctx->ir_base[insn->op1].inputs_count == 3 /* (2 values) */
		 && ctx->ir_base[insn->op1].op3 == ref
		 && ctx->ir_base[ctx->ir_base[insn->op1].op1].op == IR_LOOP_BEGIN
		 && ir_is_loop_invariant(ctx, insn->op2, ctx->ir_base[insn->op1].op1)) {
			return ir_try_promote_induction_var_ext(ctx, ext_ref, insn->op1, ref, worklist);
		} else if (!IR_IS_CONST_REF(insn->op2)
		 && ctx->ir_base[insn->op2].op == IR_PHI
		 && ctx->ir_base[insn->op2].inputs_count == 3 /* (2 values) */
		 && ctx->ir_base[insn->op2].op3 == ref
		 && ctx->ir_base[ctx->ir_base[insn->op2].op1].op == IR_LOOP_BEGIN
		 && ir_is_loop_invariant(ctx, insn->op1, ctx->ir_base[insn->op2].op1)) {
			return ir_try_promote_induction_var_ext(ctx, ext_ref, insn->op2, ref, worklist);
		}
	}

	return 0;
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

static void ir_merge_blocks(ir_ctx *ctx, ir_ref end, ir_ref begin, ir_bitqueue *worklist)
{
	ir_ref prev, next;
	ir_use_list *use_list;

	if (ctx->use_lists[begin].count > 1) {
		ir_ref *p, n, i, use;
		ir_insn *use_insn;
		ir_ref region = end;
		ir_ref next = IR_UNUSED;

		while (!IR_IS_BB_START(ctx->ir_base[region].op)) {
			region = ctx->ir_base[region].op1;
		}

		use_list = &ctx->use_lists[begin];
		n = use_list->count;
		for (p = &ctx->use_edges[use_list->refs], i = 0; i < n; p++, i++) {
			use = *p;
			use_insn = &ctx->ir_base[use];
			if (ir_op_flags[use_insn->op] & IR_OP_FLAG_CONTROL) {
				IR_ASSERT(!next);
				next = use;
			} else {
				IR_ASSERT(use_insn->op == IR_VAR);
				IR_ASSERT(use_insn->op1 == begin);
				use_insn->op1 = region;
				if (ir_use_list_add(ctx, region, use)) {
					/* restore after reallocation */
					use_list = &ctx->use_lists[begin];
					n = use_list->count;
					p = &ctx->use_edges[use_list->refs + i];
				}
			}
		}

		IR_ASSERT(next);
		ctx->use_edges[use_list->refs] = next;
		use_list->count = 1;
	}

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
	ir_use_list_replace_one(ctx, prev, end, next);

	if (ctx->ir_base[prev].op == IR_BEGIN || ctx->ir_base[prev].op == IR_MERGE) {
		ir_bitqueue_add(worklist, prev);
	}
}

static void ir_remove_unused_vars(ir_ctx *ctx, ir_ref start, ir_ref end)
{
	ir_use_list *use_list = &ctx->use_lists[start];
	ir_ref *p, use, n = use_list->count;

	for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
		use = *p;
		if (use != end) {
			ir_insn *use_insn = &ctx->ir_base[use];
			IR_ASSERT(use_insn->op == IR_VAR);
			IR_ASSERT(ctx->use_lists[use].count == 0);
			MAKE_NOP(use_insn);
		}
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

		if (ctx->use_lists[start1_ref].count != 1) {
			ir_remove_unused_vars(ctx, start1_ref, end1_ref);
		}
		if (ctx->use_lists[start2_ref].count != 1) {
			ir_remove_unused_vars(ctx, start2_ref, end2_ref);
		}

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
			if (ctx->use_lists[start_ref].count != 1) {
				ir_remove_unused_vars(ctx, start_ref, end_ref);
			}
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

			if (root->op == IR_IF && !IR_IS_CONST_REF(root->op2) && ctx->use_lists[root->op2].count == 1) {
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

					if (ctx->use_lists[start1_ref].count != 1) {
						ir_remove_unused_vars(ctx, start1_ref, end1_ref);
					}
					if (ctx->use_lists[start2_ref].count != 1) {
						ir_remove_unused_vars(ctx, start2_ref, end2_ref);
					}

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

					if (ctx->use_lists[start1_ref].count != 1) {
						ir_remove_unused_vars(ctx, start1_ref, end1_ref);
					}
					if (ctx->use_lists[start2_ref].count != 1) {
						ir_remove_unused_vars(ctx, start2_ref, end2_ref);
					}

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

					if (ctx->use_lists[start1_ref].count != 1) {
						ir_remove_unused_vars(ctx, start1_ref, end1_ref);
					}
					if (ctx->use_lists[start2_ref].count != 1) {
						ir_remove_unused_vars(ctx, start2_ref, end2_ref);
					}

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

static void ir_iter_optimize_merge(ir_ctx *ctx, ir_ref merge_ref, ir_insn *merge, ir_bitqueue *worklist)
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

			if (next->op == IR_PHI) {
				SWAP_REFS(phi_ref, next_ref);
				SWAP_INSNS(phi, next);
			}

			if (phi->op == IR_PHI && next->op != IR_PHI) {
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

static ir_ref ir_find_ext_use(ir_ctx *ctx, ir_ref ref)
{
	ir_use_list *use_list = &ctx->use_lists[ref];
	ir_ref *p, n, use;
	ir_insn *use_insn;

	for (p = &ctx->use_edges[use_list->refs], n = use_list->count; n > 0; p++, n--) {
		use = *p;
		use_insn = &ctx->ir_base[use];
		if (use_insn->op == IR_SEXT || use_insn->op == IR_ZEXT) {
			return use;
		}
	}
	return IR_UNUSED;
}

static void ir_iter_optimize_induction_var(ir_ctx *ctx, ir_ref phi_ref, ir_ref op_ref, ir_bitqueue *worklist)
{
	ir_ref ext_ref;

	ext_ref = ir_find_ext_use(ctx, phi_ref);
	if (!ext_ref) {
		ext_ref = ir_find_ext_use(ctx, op_ref);
	}
	if (ext_ref) {
		ir_try_promote_induction_var_ext(ctx, ext_ref, phi_ref, op_ref, worklist);
	}
}

static void ir_iter_optimize_loop(ir_ctx *ctx, ir_ref loop_ref, ir_insn *loop, ir_bitqueue *worklist)
{
	ir_ref n;

	if (loop->inputs_count != 2 || ctx->use_lists[loop_ref].count <= 1) {
		return;
	}

	/* Check for simple induction variable in the form: x2 = PHI(loop, x1, x3); x3 = ADD(x2, _); */
	for (n = 0; n < ctx->use_lists[loop_ref].count; n++) {
		/* "use_lists" may be reallocated by ir_ext_ref() */
		ir_ref use = ctx->use_edges[ctx->use_lists[loop_ref].refs + n];
		ir_insn *use_insn = &ctx->ir_base[use];

		if (use_insn->op == IR_PHI) {
			ir_ref op_ref = use_insn->op3;
			ir_insn *op_insn = &ctx->ir_base[op_ref];

			if (op_insn->op == IR_ADD || op_insn->op == IR_SUB || op_insn->op == IR_MUL) {
				if (op_insn->op1 == use) {
					if (ir_is_loop_invariant(ctx, op_insn->op2, loop_ref)) {
						ir_iter_optimize_induction_var(ctx, use, op_ref, worklist);
					}
				} else if (op_insn->op2 == use) {
					if (ir_is_loop_invariant(ctx, op_insn->op1, loop_ref)) {
						ir_iter_optimize_induction_var(ctx, use, op_ref, worklist);
					}
				}
		    }
		}
	}
}

static ir_ref ir_iter_optimize_condition(ir_ctx *ctx, ir_ref control, ir_ref condition, bool *swap)
{
	ir_insn *condition_insn = &ctx->ir_base[condition];

	while ((condition_insn->op == IR_BITCAST
	  || condition_insn->op == IR_ZEXT
	  || condition_insn->op == IR_SEXT)
	 && ctx->use_lists[condition].count == 1) {
		condition = condition_insn->op1;
		condition_insn = &ctx->ir_base[condition];
	}

	if (condition_insn->opt == IR_OPT(IR_NOT, IR_BOOL)) {
		*swap = 1;
		condition = condition_insn->op1;
		condition_insn = &ctx->ir_base[condition];
	}

	if (condition_insn->op == IR_NE && IR_IS_CONST_REF(condition_insn->op2)) {
		ir_insn *val_insn = &ctx->ir_base[condition_insn->op2];

		if (IR_IS_TYPE_INT(val_insn->type) && val_insn->val.u64 == 0) {
			condition = condition_insn->op1;
			condition_insn = &ctx->ir_base[condition];
		}
	} else if (condition_insn->op == IR_EQ && IR_IS_CONST_REF(condition_insn->op2)) {
		ir_insn *val_insn = &ctx->ir_base[condition_insn->op2];

		if (condition_insn->op2 == IR_TRUE) {
			condition = condition_insn->op1;
			condition_insn = &ctx->ir_base[condition];
		} else if (IR_IS_TYPE_INT(val_insn->type) && val_insn->val.u64 == 0) {
			condition = condition_insn->op1;
			condition_insn = &ctx->ir_base[condition];
			*swap = !*swap;
		}
	}

	while ((condition_insn->op == IR_BITCAST
	  || condition_insn->op == IR_ZEXT
	  || condition_insn->op == IR_SEXT)
	 && ctx->use_lists[condition].count == 1) {
		condition = condition_insn->op1;
		condition_insn = &ctx->ir_base[condition];
	}

	if (condition_insn->op == IR_ALLOCA || condition_insn->op == IR_VADDR) {
		return IR_TRUE;
	}

	if (!IR_IS_CONST_REF(condition) && ctx->use_lists[condition].count > 1) {
		condition = ir_check_dominating_predicates(ctx, control, condition);
	}

	return condition;
}

static void ir_iter_optimize_if(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	bool swap = 0;
	ir_ref condition = ir_iter_optimize_condition(ctx, insn->op1, insn->op2, &swap);

	if (swap) {
		ir_use_list *use_list = &ctx->use_lists[ref];
		ir_ref *p, use;

		IR_ASSERT(use_list->count == 2);
		p = ctx->use_edges + use_list->refs;
		use = *p;
		if (ctx->ir_base[use].op == IR_IF_TRUE) {
			ctx->ir_base[use].op = IR_IF_FALSE;
			use = *(p+1);
			ctx->ir_base[use].op = IR_IF_TRUE;
		} else {
			ctx->ir_base[use].op = IR_IF_TRUE;
			use = *(p+1);
			ctx->ir_base[use].op = IR_IF_FALSE;
		}
	}

	if (IR_IS_CONST_REF(condition)) {
		/*
		 *    |                        |
		 *    IF(TRUE)             =>  END
		 *    | \                      |
		 *    |  +------+              |
		 *    |         IF_TRUE        |        BEGIN(unreachable)
		 *    IF_FALSE  |              BEGIN
		 *    |                        |
		 */
		ir_ref if_true_ref, if_false_ref;
		ir_insn *if_true, *if_false;

		insn->optx = IR_OPTX(IR_END, IR_VOID, 1);
		if (!IR_IS_CONST_REF(insn->op2)) {
			ir_use_list_remove_one(ctx, insn->op2, ref);
			ir_bitqueue_add(worklist, insn->op2);
		}
		insn->op2 = IR_UNUSED;

		ir_get_true_false_refs(ctx, ref, &if_true_ref, &if_false_ref);
		if_true = &ctx->ir_base[if_true_ref];
		if_false = &ctx->ir_base[if_false_ref];
		if_true->op = IR_BEGIN;
		if_false->op = IR_BEGIN;
		if (ir_ref_is_true(ctx, condition)) {
			if_false->op1 = IR_UNUSED;
			ir_use_list_remove_one(ctx, ref, if_false_ref);
			ir_bitqueue_add(worklist, if_true_ref);
		} else {
			if_true->op1 = IR_UNUSED;
			ir_use_list_remove_one(ctx, ref, if_true_ref);
			ir_bitqueue_add(worklist, if_false_ref);
		}
		ctx->flags2 &= ~IR_CFG_REACHABLE;
	} else if (insn->op2 != condition) {
		ir_iter_update_op(ctx, ref, 2, condition, worklist);
	}
}

static void ir_iter_optimize_guard(ir_ctx *ctx, ir_ref ref, ir_insn *insn, ir_bitqueue *worklist)
{
	bool swap = 0;
	ir_ref condition = ir_iter_optimize_condition(ctx, insn->op1, insn->op2, &swap);

	if (swap) {
		if (insn->op == IR_GUARD) {
			insn->op = IR_GUARD_NOT;
		} else {
			insn->op = IR_GUARD;
		}
	}

	if (IR_IS_CONST_REF(condition)) {
		if (insn->op == IR_GUARD) {
			if (ir_ref_is_true(ctx, condition)) {
				ir_ref prev, next;

remove_guard:
				prev = insn->op1;
				next = ir_next_control(ctx, ref);
				ctx->ir_base[next].op1 = prev;
				ir_use_list_remove_one(ctx, ref, next);
				ir_use_list_replace_one(ctx, prev, ref, next);
				insn->op1 = IR_UNUSED;

				if (!IR_IS_CONST_REF(insn->op2)) {
					ir_use_list_remove_one(ctx, insn->op2, ref);
					if (ir_is_dead(ctx, insn->op2)) {
						/* schedule DCE */
						ir_bitqueue_add(worklist, insn->op2);
					}
				}

				if (insn->op3) {
					/* SNAPSHOT */
					ir_iter_remove_insn(ctx, insn->op3, worklist);
				}

				MAKE_NOP(insn);
				return;
			} else {
				condition = IR_FALSE;
			}
		} else {
			if (ir_ref_is_true(ctx, condition)) {
				condition = IR_TRUE;
			} else {
				goto remove_guard;
			}
		}
	}

	if (insn->op2 != condition) {
		ir_iter_update_op(ctx, ref, 2, condition, worklist);
	}
}

void ir_iter_opt(ir_ctx *ctx, ir_bitqueue *worklist)
{
	ir_ref i, val;
	ir_insn *insn;

	while ((i = ir_bitqueue_pop(worklist)) >= 0) {
		insn = &ctx->ir_base[i];
		if (IR_IS_FOLDABLE_OP(insn->op)) {
			if (ctx->use_lists[i].count == 0) {
				if (insn->op == IR_PHI) {
					ir_bitqueue_add(worklist, insn->op1);
				}
				ir_iter_remove_insn(ctx, i, worklist);
			} else {
				insn = &ctx->ir_base[i];
				switch (insn->op) {
					case IR_FP2FP:
						if (insn->type == IR_FLOAT) {
							if (ir_may_promote_d2f(ctx, insn->op1)) {
								ir_ref ref = ir_promote_d2f(ctx, insn->op1, i, worklist);
								insn->op1 = ref;
								ir_iter_replace_insn(ctx, i, ref, worklist);
								break;
							}
						} else {
							if (ir_may_promote_f2d(ctx, insn->op1)) {
								ir_ref ref = ir_promote_f2d(ctx, insn->op1, i, worklist);
								insn->op1 = ref;
								ir_iter_replace_insn(ctx, i, ref, worklist);
								break;
							}
						}
						goto folding;
					case IR_FP2INT:
						if (ctx->ir_base[insn->op1].type == IR_DOUBLE) {
							if (ir_may_promote_d2f(ctx, insn->op1)) {
								insn->op1 = ir_promote_d2f(ctx, insn->op1, i, worklist);
							}
						} else {
							if (ir_may_promote_f2d(ctx, insn->op1)) {
								insn->op1 = ir_promote_f2d(ctx, insn->op1, i, worklist);
							}
						}
						goto folding;
					case IR_TRUNC:
						if (ir_may_promote_trunc(ctx, insn->type, insn->op1)) {
							ir_ref ref = ir_promote_i2i(ctx, insn->type, insn->op1, i, worklist);
							insn->op1 = ref;
							ir_iter_replace_insn(ctx, i, ref, worklist);
							break;
						}
						goto folding;
					case IR_SEXT:
					case IR_ZEXT:
						if (ir_try_promote_ext(ctx, i, insn, worklist)) {
							break;
						}
						goto folding;
					case IR_PHI:
						break;
					default:
folding:
						ir_iter_fold(ctx, i, worklist);
						break;
				}
			}
		} else if (ir_op_flags[insn->op] & IR_OP_FLAG_BB_START) {
			if (!(ctx->flags & IR_OPT_CFG)) {
				/* pass */
			} else if (insn->op == IR_BEGIN) {
				if (insn->op1 && ctx->ir_base[insn->op1].op == IR_END) {
					ir_merge_blocks(ctx, insn->op1, i, worklist);
				}
			} else if (insn->op == IR_MERGE) {
				ir_iter_optimize_merge(ctx, i, insn, worklist);
			} else if (insn->op == IR_LOOP_BEGIN) {
				ir_iter_optimize_loop(ctx, i, insn, worklist);
			}
		} else if (ir_is_dead_load(ctx, i)) {
			ir_ref next;

			/* remove LOAD from double linked control list */
remove_mem_insn:
			next = ctx->use_edges[ctx->use_lists[i].refs];
			IR_ASSERT(ctx->use_lists[i].count == 1);
			ctx->ir_base[next].op1 = insn->op1;
			ir_use_list_replace_one(ctx, insn->op1, i, next);
			insn->op1 = IR_UNUSED;
			ir_iter_remove_insn(ctx, i, worklist);
		} else if (insn->op == IR_LOAD) {
			val = ir_find_aliasing_load(ctx, insn->op1, insn->type, insn->op2);
			if (val) {
				ir_insn *val_insn;
				ir_ref prev, next;

remove_aliased_load:
				prev = insn->op1;
				next = ir_next_control(ctx, i);
				ctx->ir_base[next].op1 = prev;
				ir_use_list_remove_one(ctx, i, next);
				ir_use_list_replace_one(ctx, prev, i, next);
				insn->op1 = IR_UNUSED;

				val_insn = &ctx->ir_base[val];
				if (val_insn->type == insn->type) {
					ir_iter_replace_insn(ctx, i, val, worklist);
				} else {
					if (!IR_IS_CONST_REF(insn->op2)) {
						ir_use_list_remove_one(ctx, insn->op2, i);
						if (ir_is_dead(ctx, insn->op2)) {
							/* schedule DCE */
							ir_bitqueue_add(worklist, insn->op2);
						}
					}
					if (!IR_IS_CONST_REF(val)) {
						ir_use_list_add(ctx, val, i);
					}
					if (ir_type_size[val_insn->type] == ir_type_size[insn->type]) {
						/* load forwarding with bitcast (L2L) */
						insn->optx = IR_OPTX(IR_BITCAST, insn->type, 1);
					} else {
						/* partial load forwarding (L2L) */
						insn->optx = IR_OPTX(IR_TRUNC, insn->type, 1);
					}
					insn->op1 = val;
					insn->op2 = IR_UNUSED;
					ir_bitqueue_add(worklist, i);
				}
			}
		} else if (insn->op == IR_STORE) {
			if (ir_find_aliasing_store(ctx, insn->op1, insn->op2, insn->op3)) {
				goto remove_mem_insn;
			} else {
				ir_insn *val_insn;

remove_bitcast:
				val = insn->op3;
				val_insn = &ctx->ir_base[val];
				if (val_insn->op == IR_BITCAST
				 && ir_type_size[val_insn->type] == ir_type_size[ctx->ir_base[val_insn->op1].type]) {
					insn->op3 = val_insn->op1;
					ir_use_list_remove_one(ctx, val, i);
					if (ctx->use_lists[val].count == 0) {
						if (!IR_IS_CONST_REF(val_insn->op1)) {
							ir_use_list_replace_one(ctx, val_insn->op1, val, i);
						}
						ir_iter_remove_insn(ctx, val, worklist);
					} else {
						if (!IR_IS_CONST_REF(val_insn->op1)) {
							ir_use_list_add(ctx, val_insn->op1, i);
						}
					}
				}
			}
		} else if (insn->op == IR_VLOAD) {
			val = ir_find_aliasing_vload(ctx, insn->op1, insn->type, insn->op2);
			if (val) {
				goto remove_aliased_load;
			}
		} else if (insn->op == IR_VSTORE) {
			if (ir_find_aliasing_vstore(ctx, insn->op1, insn->op2, insn->op3)) {
				goto remove_mem_insn;
			} else {
				goto remove_bitcast;
			}
		} else if (insn->op == IR_IF) {
			ir_iter_optimize_if(ctx, i, insn, worklist);
		} else if (insn->op == IR_GUARD || insn->op == IR_GUARD_NOT) {
			ir_iter_optimize_guard(ctx, i, insn, worklist);
		}
	}
}

int ir_sccp(ir_ctx *ctx)
{
	ir_bitqueue sccp_worklist, iter_worklist;
	ir_insn *_values;

	ir_bitqueue_init(&iter_worklist, ctx->insns_count);
	ir_bitqueue_init(&sccp_worklist, ctx->insns_count);
	_values = ir_mem_calloc(ctx->insns_count, sizeof(ir_insn));

	ctx->flags2 |= IR_OPT_IN_SCCP;
	ir_sccp_analyze(ctx, _values, &sccp_worklist, &iter_worklist);
	ir_sccp_transform(ctx, _values, &sccp_worklist, &iter_worklist);
	ctx->flags2 &= ~IR_OPT_IN_SCCP;

	ir_mem_free(_values);
	ir_bitqueue_free(&sccp_worklist);

	ctx->flags2 |= IR_CFG_REACHABLE;

	ir_iter_opt(ctx, &iter_worklist);

	ir_bitqueue_free(&iter_worklist);

	return 1;
}
