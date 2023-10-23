/*
 * IR - Lightweight JIT Compilation Framework
 * (IR saver)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

void ir_save(const ir_ctx *ctx, FILE *f)
{
	ir_ref i, j, n, ref, *p;
	ir_insn *insn;
	uint32_t flags;
	bool first;

	fprintf(f, "{\n");
	for (i = IR_UNUSED + 1, insn = ctx->ir_base - i; i < ctx->consts_count; i++, insn--) {
		fprintf(f, "\t%s c_%d = ", ir_type_cname[insn->type], i);
		if (insn->op == IR_FUNC) {
			if (!insn->const_flags) {
				fprintf(f, "func(%s)", ir_get_str(ctx, insn->val.i32));
			} else {
				fprintf(f, "func(%s, %d)", ir_get_str(ctx, insn->val.i32), insn->const_flags);
			}
		} else if (insn->op == IR_SYM) {
			fprintf(f, "sym(%s)", ir_get_str(ctx, insn->val.i32));
		} else if (insn->op == IR_FUNC_ADDR) {
			fprintf(f, "func_addr(");
			ir_print_const(ctx, insn, f, true);
			if (insn->const_flags) {
				fprintf(f, ", %d", insn->const_flags);
			}
			fprintf(f, ")");
		} else {
			ir_print_const(ctx, insn, f, true);
		}
		fprintf(f, ";\n");
	}

	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		flags = ir_op_flags[insn->op];
		if (flags & IR_OP_FLAG_CONTROL) {
			if (!(flags & IR_OP_FLAG_MEM) || insn->type == IR_VOID) {
				fprintf(f, "\tl_%d = ", i);
			} else {
				fprintf(f, "\t%s d_%d, l_%d = ", ir_type_cname[insn->type], i, i);
			}
		} else {
			fprintf(f, "\t");
			if (flags & IR_OP_FLAG_DATA) {
				fprintf(f, "%s d_%d = ", ir_type_cname[insn->type], i);
			}
		}
		fprintf(f, "%s", ir_op_name[insn->op]);
		n = ir_operands_count(ctx, insn);
		if ((insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) && n != 2) {
			fprintf(f, "/%d", n);
		} else if ((insn->op == IR_CALL || insn->op == IR_TAILCALL) && n != 2) {
			fprintf(f, "/%d", n - 2);
		} else if (insn->op == IR_PHI && n != 3) {
			fprintf(f, "/%d", n - 1);
		} else if (insn->op == IR_SNAPSHOT) {
			fprintf(f, "/%d", n - 1);
		}
		first = 1;
		for (j = 1, p = insn->ops + 1; j <= n; j++, p++) {
			uint32_t opnd_kind = IR_OPND_KIND(flags, j);

			ref = *p;
			if (ref) {
				switch (opnd_kind) {
					case IR_OPND_DATA:
						if (IR_IS_CONST_REF(ref)) {
							fprintf(f, "%sc_%d", first ? "(" : ", ", -ref);
						} else {
							fprintf(f, "%sd_%d", first ? "(" : ", ", ref);
						}
						first = 0;
						break;
					case IR_OPND_CONTROL:
					case IR_OPND_CONTROL_DEP:
					case IR_OPND_CONTROL_REF:
						fprintf(f, "%sl_%d", first ? "(" : ", ", ref);
						first = 0;
						break;
					case IR_OPND_STR:
						fprintf(f, "%s\"%s\"", first ? "(" : ", ", ir_get_str(ctx, ref));
						first = 0;
						break;
					case IR_OPND_PROB:
						if (ref == 0) {
							break;
						}
						IR_FALLTHROUGH;
					case IR_OPND_NUM:
						fprintf(f, "%s%d", first ? "(" : ", ", ref);
						first = 0;
						break;
				}
			} else if (opnd_kind == IR_OPND_NUM) {
				fprintf(f, "%s%d", first ? "(" : ", ", ref);
				first = 0;
			} else if (IR_IS_REF_OPND_KIND(opnd_kind) && j != n) {
				fprintf(f, "%snull", first ? "(" : ", ");
				first = 0;
			}
		}
		if (first) {
			fprintf(f, ";");
		} else {
			fprintf(f, ");");
		}
		if (((flags & IR_OP_FLAG_DATA) || ((flags & IR_OP_FLAG_MEM) && insn->type != IR_VOID)) && ctx->binding) {
			ir_ref var = ir_binding_find(ctx, i);
			if (var) {
				IR_ASSERT(var < 0);
				fprintf(f, " # BIND(0x%x);", -var);
			}
		}
		fprintf(f, "\n");
		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}
	fprintf(f, "}\n");
}
