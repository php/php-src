/*
 * IR - Lightweight JIT Compilation Framework
 * (IR saver)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

void ir_print_proto(const ir_ctx *ctx, ir_ref func_proto, FILE *f)
{
	ir_ref j;

	if (func_proto) {
		const ir_proto_t *proto = (const ir_proto_t *)ir_get_str(ctx, func_proto);

		fprintf(f, "(");
		if (proto->params_count > 0) {
			fprintf(f, "%s", ir_type_cname[proto->param_types[0]]);
			for (j = 1; j < proto->params_count; j++) {
				fprintf(f, ", %s", ir_type_cname[proto->param_types[j]]);
			}
			if (proto->flags & IR_VARARG_FUNC) {
				fprintf(f, ", ...");
			}
		} else if (proto->flags & IR_VARARG_FUNC) {
			fprintf(f, "...");
		}
		fprintf(f, "): %s", ir_type_cname[proto->ret_type]);
		if (proto->flags & IR_FASTCALL_FUNC) {
			fprintf(f, " __fastcall");
		} else if (proto->flags & IR_BUILTIN_FUNC) {
			fprintf(f, " __builtin");
		}
	} else {
		fprintf(f, "(): int32_t");
	}
}

static void ir_save_dessa_moves(const ir_ctx *ctx, int b, ir_block *bb, FILE *f)
{
	uint32_t succ;
	ir_block *succ_bb;
	ir_use_list *use_list;
	ir_ref k, i, *p, use_ref, input;
	ir_insn *use_insn;

	IR_ASSERT(bb->successors_count == 1);
	succ = ctx->cfg_edges[bb->successors];
	succ_bb = &ctx->cfg_blocks[succ];
	IR_ASSERT(succ_bb->predecessors_count > 1);
	use_list = &ctx->use_lists[succ_bb->start];
	k = ir_phi_input_number(ctx, succ_bb, b);

	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < use_list->count; i++, p++) {
		use_ref = *p;
		use_insn = &ctx->ir_base[use_ref];
		if (use_insn->op == IR_PHI) {
			input = ir_insn_op(use_insn, k);
			if (IR_IS_CONST_REF(input)) {
				fprintf(f, "\t# DESSA MOV c_%d", -input);
			} else if (ctx->vregs[input] != ctx->vregs[use_ref]) {
				fprintf(f, "\t# DESSA MOV d_%d {R%d}", input, ctx->vregs[input]);
			} else {
				continue;
			}
			if (ctx->regs) {
				int8_t *regs = ctx->regs[use_ref];
				int8_t reg = regs[k];
				if (reg != IR_REG_NONE) {
					fprintf(f, " {%%%s%s}", ir_reg_name(IR_REG_NUM(reg), ctx->ir_base[input].type),
						(reg & (IR_REG_SPILL_LOAD|IR_REG_SPILL_SPECIAL)) ? ":load" : "");
				}
			}
			fprintf(f, " -> d_%d {R%d}", use_ref, ctx->vregs[use_ref]);
			if (ctx->regs) {
				int8_t reg = ctx->regs[use_ref][0];
				if (reg != IR_REG_NONE) {
					fprintf(f, " {%%%s%s}", ir_reg_name(IR_REG_NUM(reg), ctx->ir_base[use_ref].type),
						(reg & (IR_REG_SPILL_STORE|IR_REG_SPILL_SPECIAL)) ? ":store" : "");
				}
			}
			fprintf(f, "\n");
		}
	}
}

void ir_save(const ir_ctx *ctx, uint32_t save_flags, FILE *f)
{
	ir_ref i, j, n, ref, *p;
	ir_insn *insn;
	uint32_t flags;
	bool first;

	fprintf(f, "{\n");
	for (i = IR_UNUSED + 1, insn = ctx->ir_base - i; i < ctx->consts_count; i++, insn--) {
		fprintf(f, "\t%s c_%d = ", ir_type_cname[insn->type], i);
		if (insn->op == IR_FUNC) {
			fprintf(f, "func %s", ir_get_str(ctx, insn->val.name));
			ir_print_proto(ctx, insn->proto, f);
		} else if (insn->op == IR_SYM) {
			fprintf(f, "sym(%s)", ir_get_str(ctx, insn->val.name));
		} else if (insn->op == IR_FUNC_ADDR) {
			fprintf(f, "func *");
			ir_print_const(ctx, insn, f, true);
			ir_print_proto(ctx, insn->proto, f);
		} else {
			ir_print_const(ctx, insn, f, true);
		}
		fprintf(f, ";\n");
	}

	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		flags = ir_op_flags[insn->op];

		if ((save_flags & IR_SAVE_CFG)
		 && ctx->cfg_map
		 && (int32_t)ctx->cfg_map[i] > 0 /* the node may be scheduled incompletely */
		 && ctx->cfg_blocks[ctx->cfg_map[i]].start == i) {
			uint32_t b = ctx->cfg_map[i];
			ir_block *bb = &ctx->cfg_blocks[b];
			fprintf(f, "#BB%d: end=l_%d", b, bb->end);
			if (bb->flags & IR_BB_UNREACHABLE) {
				fprintf(f, ", U");
			}
			if (bb->dom_parent > 0) {
				fprintf(f, ", idom=BB%d(%d)", bb->dom_parent, bb->dom_depth);
			}
			if (bb->loop_depth != 0) {
				if (bb->flags & IR_BB_LOOP_HEADER) {
					if (bb->loop_header > 0) {
						fprintf(f, ", loop=HDR,BB%d(%d)", bb->loop_header, bb->loop_depth);
					} else {
						IR_ASSERT(bb->loop_depth == 1);
						fprintf(f, ", loop=HDR(%d)", bb->loop_depth);
					}
				} else {
					IR_ASSERT(bb->loop_header > 0);
					fprintf(f, ", loop=BB%d(%d)", bb->loop_header, bb->loop_depth);
				}
			}
			if (bb->predecessors_count) {
				uint32_t i;

				fprintf(f, ", pred(%d)=[BB%d", bb->predecessors_count, ctx->cfg_edges[bb->predecessors]);
				for (i = 1; i < bb->predecessors_count; i++) {
					fprintf(f, ", BB%d", ctx->cfg_edges[bb->predecessors + i]);
				}
				fprintf(f, "]");
			}
			if (bb->successors_count) {
				uint32_t i;

				fprintf(f, ", succ(%d)=[BB%d", bb->successors_count, ctx->cfg_edges[bb->successors]);
				for (i = 1; i < bb->successors_count; i++) {
					fprintf(f, ", BB%d", ctx->cfg_edges[bb->successors + i]);
				}
				fprintf(f, "]");
			}
			fprintf(f, "\n");
		}

		if (flags & IR_OP_FLAG_CONTROL) {
			if (!(flags & IR_OP_FLAG_MEM) || insn->type == IR_VOID) {
				fprintf(f, "\tl_%d = ", i);
			} else {
				fprintf(f, "\t%s d_%d", ir_type_cname[insn->type], i);
				if (save_flags & IR_SAVE_REGS) {
					if (ctx->vregs && ctx->vregs[i]) {
						fprintf(f, " {R%d}", ctx->vregs[i]);
					}
					if (ctx->regs) {
						int8_t reg = ctx->regs[i][0];
						if (reg != IR_REG_NONE) {
							fprintf(f, " {%%%s%s}", ir_reg_name(IR_REG_NUM(reg), insn->type),
								(reg & (IR_REG_SPILL_STORE|IR_REG_SPILL_SPECIAL)) ? ":store" : "");
						}
					}
				}
				fprintf(f, ", l_%d = ", i);
			}
		} else {
			fprintf(f, "\t");
			if (flags & IR_OP_FLAG_DATA) {
				fprintf(f, "%s d_%d", ir_type_cname[insn->type], i);
				if (save_flags & IR_SAVE_REGS) {
					if (ctx->vregs && ctx->vregs[i]) {
						fprintf(f, " {R%d}", ctx->vregs[i]);
					}
					if (ctx->regs) {
						int8_t reg = ctx->regs[i][0];
						if (reg != IR_REG_NONE) {
							fprintf(f, " {%%%s%s}", ir_reg_name(IR_REG_NUM(reg), insn->type),
								(reg & (IR_REG_SPILL_STORE|IR_REG_SPILL_SPECIAL)) ? ":store" : "");
						}
					}
				}
				fprintf(f, " = ");
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
						if (save_flags & IR_SAVE_REGS) {
							if (ctx->vregs && ref > 0 && ctx->vregs[ref]) {
								fprintf(f, " {R%d}", ctx->vregs[ref]);
							}
							if (ctx->regs) {
								int8_t *regs = ctx->regs[i];
								int8_t reg = regs[j];
								if (reg != IR_REG_NONE) {
									fprintf(f, " {%%%s%s}", ir_reg_name(IR_REG_NUM(reg), ctx->ir_base[ref].type),
										(reg & (IR_REG_SPILL_LOAD|IR_REG_SPILL_SPECIAL)) ? ":load" : "");
								}
							}
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
					case IR_OPND_PROTO:
						fprintf(f, "%sfunc ", first ? "(" : ", ");
						ir_print_proto(ctx, ref, f);
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
			} else if (j != n &&
					(IR_IS_REF_OPND_KIND(opnd_kind) || (opnd_kind == IR_OPND_UNUSED && p[n-j]))) {
				fprintf(f, "%snull", first ? "(" : ", ");
				first = 0;
			}
		}
		if (first) {
			fprintf(f, ";");
		} else {
			fprintf(f, ");");
		}
		first = 1;
		if (((flags & IR_OP_FLAG_DATA) || ((flags & IR_OP_FLAG_MEM) && insn->type != IR_VOID)) && ctx->binding) {
			ir_ref var = ir_binding_find(ctx, i);
			if (var) {
				IR_ASSERT(var < 0);
				fprintf(f, " # BIND(0x%x);", -var);
				first = 0;
			}
		}

		if ((save_flags & IR_SAVE_CFG_MAP)
		 && ctx->cfg_map
		 && ctx->cfg_map[i] > 0) { /* the node may be scheduled incompletely */
			if (first) {
				fprintf(f, " #");
				first = 0;
			}
			fprintf(f, " BLOCK=BB%d;", ctx->cfg_map[i]);
		}

		if ((save_flags & IR_SAVE_RULES)
		 && ctx->rules) {
			uint32_t rule = ctx->rules[i];
			uint32_t id = rule & IR_RULE_MASK;

			if (first) {
				fprintf(f, " #");
				first = 0;
			}
			if (id < IR_LAST_OP) {
				fprintf(f, " RULE(%s", ir_op_name[id]);
			} else {
				IR_ASSERT(id > IR_LAST_OP /*&& id < IR_LAST_RULE*/);
				fprintf(f, " RULE(%s", ir_rule_name[id - IR_LAST_OP]);
			}
			if (rule & IR_FUSED) {
				fprintf(f, ":FUSED");
			}
			if (rule & IR_SKIPPED) {
				fprintf(f, ":SKIPPED");
			}
			if (rule & IR_SIMPLE) {
				fprintf(f, ":SIMPLE");
			}
			fprintf(f, ");");
		}

		if ((save_flags & IR_SAVE_USE_LISTS)
		 && ctx->use_lists
		 && ctx->use_lists[i].count) {
			ir_use_list *use_list = &ctx->use_lists[i];
			ir_ref n = use_list->count;
			ir_ref *p = ctx->use_edges + use_list->refs;

			if (first) {
				fprintf(f, " #");
				first = 0;
			}
			fprintf(f, " USE_LIST(%d)=[%05d", n, *p);
			for (p++, n--; n; p++, n--) {
				fprintf(f, ", %05d", *p);
			}
			fprintf(f, "];");
		}
		fprintf(f, "\n");

		if ((save_flags & (IR_SAVE_CFG|IR_SAVE_REGS)) == (IR_SAVE_CFG|IR_SAVE_REGS)
		 && ctx->cfg_map
		 && ctx->cfg_map[i]
		 && ctx->cfg_blocks[ctx->cfg_map[i]].end == i) {
			uint32_t b = ctx->cfg_map[i];
			ir_block *bb = &ctx->cfg_blocks[b];
			if (bb->flags & IR_BB_DESSA_MOVES) {
				ir_save_dessa_moves(ctx, b, bb, f);
			}
		}

		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}
	fprintf(f, "}\n");
}
