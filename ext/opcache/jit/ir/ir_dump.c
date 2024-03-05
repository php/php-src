/*
 * IR - Lightweight JIT Compilation Framework
 * (debug dumps)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#include "ir.h"
#include "ir_private.h"

void ir_dump(const ir_ctx *ctx, FILE *f)
{
	ir_ref i, j, n, ref, *p;
	ir_insn *insn;
	uint32_t flags;

	for (i = 1 - ctx->consts_count, insn = ctx->ir_base + i; i < IR_UNUSED; i++, insn++) {
		fprintf(f, "%05d %s %s(", i, ir_op_name[insn->op], ir_type_name[insn->type]);
		ir_print_const(ctx, insn, f, true);
		fprintf(f, ")\n");
	}

	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count; i++, insn++) {
		flags = ir_op_flags[insn->op];
		fprintf(f, "%05d %s", i, ir_op_name[insn->op]);
		if ((flags & IR_OP_FLAG_DATA) || ((flags & IR_OP_FLAG_MEM) && insn->type != IR_VOID)) {
			fprintf(f, " %s", ir_type_name[insn->type]);
		}
		n = ir_operands_count(ctx, insn);
		for (j = 1, p = insn->ops + 1; j <= 3; j++, p++) {
			ref = *p;
			if (ref) {
				fprintf(f, " %05d", ref);
			}
		}
		if (n > 3) {
			n -= 3;
			do {
				i++;
				insn++;
				fprintf(f, "\n%05d", i);
				for (j = 0; j < 4; j++, p++) {
					ref = *p;
					if (ref) {
						fprintf(f, " %05d", ref);
					}
				}
				n -= 4;
			} while (n > 0);
		}
		fprintf(f, "\n");
	}
}

void ir_dump_dot(const ir_ctx *ctx, const char *name, FILE *f)
{
	int DATA_WEIGHT    = 0;
	int CONTROL_WEIGHT = 5;
	int REF_WEIGHT     = 4;
	ir_ref i, j, n, ref, *p;
	ir_insn *insn;
	uint32_t flags;

	fprintf(f, "digraph %s {\n", name);
	fprintf(f, "\trankdir=TB;\n");
	for (i = 1 - ctx->consts_count, insn = ctx->ir_base + i; i < IR_UNUSED; i++, insn++) {
		fprintf(f, "\tc%d [label=\"C%d: CONST %s(", -i, -i, ir_type_name[insn->type]);
		/* FIXME(tony): We still cannot handle strings with escaped double quote inside */
		ir_print_const(ctx, insn, f, false);
		fprintf(f, ")\",style=filled,fillcolor=yellow];\n");
	}

	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		flags = ir_op_flags[insn->op];
		if (flags & IR_OP_FLAG_CONTROL) {
			if (insn->op == IR_START) {
				fprintf(f, "\t{rank=min; n%d [label=\"%d: %s\",shape=box,style=\"rounded,filled\",fillcolor=red];}\n", i, i, ir_op_name[insn->op]);
			} else if (insn->op == IR_ENTRY) {
				fprintf(f, "\t{n%d [label=\"%d: %s\",shape=box,style=\"rounded,filled\",fillcolor=red];}\n", i, i, ir_op_name[insn->op]);
			} else if (flags & IR_OP_FLAG_TERMINATOR) {
				fprintf(f, "\t{rank=max; n%d [label=\"%d: %s\",shape=box,style=\"rounded,filled\",fillcolor=red];}\n", i, i, ir_op_name[insn->op]);
			} else if (flags & IR_OP_FLAG_MEM) {
				fprintf(f, "\tn%d [label=\"%d: %s\",shape=box,style=filled,fillcolor=pink];\n", i, i, ir_op_name[insn->op]);
			} else {
				fprintf(f, "\tn%d [label=\"%d: %s\",shape=box,style=filled,fillcolor=lightcoral];\n", i, i, ir_op_name[insn->op]);
			}
		} else if (flags & IR_OP_FLAG_DATA) {
			if (IR_OPND_KIND(flags, 1) == IR_OPND_DATA) {
				/* not a leaf */
				fprintf(f, "\tn%d [label=\"%d: %s\"", i, i, ir_op_name[insn->op]);
				fprintf(f, ",shape=diamond,style=filled,fillcolor=deepskyblue];\n");
			} else {
				if (insn->op == IR_PARAM) {
					fprintf(f, "\tn%d [label=\"%d: %s %s \\\"%s\\\"\",style=filled,fillcolor=lightblue];\n",
						i, i, ir_op_name[insn->op], ir_type_name[insn->type], ir_get_str(ctx, insn->op2));
				} else if (insn->op == IR_VAR) {
					fprintf(f, "\tn%d [label=\"%d: %s %s \\\"%s\\\"\"];\n", i, i, ir_op_name[insn->op], ir_type_name[insn->type], ir_get_str(ctx, insn->op2));
				} else {
					fprintf(f, "\tn%d [label=\"%d: %s %s\",style=filled,fillcolor=deepskyblue];\n", i, i, ir_op_name[insn->op], ir_type_name[insn->type]);
				}
			}
		}
		n = ir_operands_count(ctx, insn);
		for (j = 1, p = insn->ops + 1; j <= n; j++, p++) {
			ref = *p;
			if (ref) {
				switch (IR_OPND_KIND(flags, j)) {
					case IR_OPND_DATA:
						if (IR_IS_CONST_REF(ref)) {
							fprintf(f, "\tc%d -> n%d [color=blue,weight=%d];\n", -ref, i, DATA_WEIGHT);
						} else if (insn->op == IR_PHI
								&& ctx->ir_base[insn->op1].op == IR_LOOP_BEGIN
								&& ctx->ir_base[ir_insn_op(&ctx->ir_base[insn->op1], j - 1)].op == IR_LOOP_END) {
							fprintf(f, "\tn%d -> n%d [color=blue,dir=back];\n", i, ref);
						} else {
							fprintf(f, "\tn%d -> n%d [color=blue,weight=%d];\n", ref, i, DATA_WEIGHT);
						}
						break;
					case IR_OPND_CONTROL:
						if (insn->op == IR_LOOP_BEGIN && ctx->ir_base[ref].op == IR_LOOP_END) {
							fprintf(f, "\tn%d -> n%d [style=bold,color=red,dir=back];\n", i, ref);
						} else if (insn->op == IR_ENTRY) {
							fprintf(f, "\tn%d -> n%d [style=bold,color=red,style=dashed,weight=%d];\n", ref, i, CONTROL_WEIGHT);
						} else {
							fprintf(f, "\tn%d -> n%d [style=bold,color=red,weight=%d];\n", ref, i, CONTROL_WEIGHT);
						}
						break;
					case IR_OPND_CONTROL_DEP:
					case IR_OPND_CONTROL_REF:
						fprintf(f, "\tn%d -> n%d [style=dashed,dir=back,weight=%d];\n", ref, i, REF_WEIGHT);
						break;
				}
			}
		}
		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}
	fprintf(f, "}\n");
}

void ir_dump_use_lists(const ir_ctx *ctx, FILE *f)
{
	ir_ref i, j, n, *p;
	ir_use_list *list;

	if (ctx->use_lists) {
		fprintf(f, "{ # Use Lists\n");
		for (i = 1, list = &ctx->use_lists[1]; i < ctx->insns_count; i++, list++) {
			n = list->count;
			if (n > 0) {
				p = &ctx->use_edges[list->refs];
				fprintf(f, "%05d(%d): [%05d", i, n, *p);
				p++;
				for (j = 1; j < n; j++, p++) {
					fprintf(f, ", %05d", *p);
				}
				fprintf(f, "]\n");
			}
		}
		fprintf(f, "}\n");
	}
}

static void ir_dump_dessa_moves(const ir_ctx *ctx, int b, ir_block *bb, FILE *f)
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

static void ir_dump_cfg_block(ir_ctx *ctx, FILE *f, uint32_t b, ir_block *bb)
{
	fprintf(f, "BB%d:\n", b);
	fprintf(f, "\tstart=%d\n", bb->start);
	fprintf(f, "\tend=%d\n", bb->end);
	if (bb->successors_count) {
		uint32_t i;

		fprintf(f, "\tsuccessors(%d) [BB%d", bb->successors_count, ctx->cfg_edges[bb->successors]);
		for (i = 1; i < bb->successors_count; i++) {
			fprintf(f, ", BB%d", ctx->cfg_edges[bb->successors + i]);
		}
		fprintf(f, "]\n");
	}
	if (bb->predecessors_count) {
		uint32_t i;

		fprintf(f, "\tpredecessors(%d) [BB%d", bb->predecessors_count, ctx->cfg_edges[bb->predecessors]);
		for (i = 1; i < bb->predecessors_count; i++) {
			fprintf(f, ", BB%d", ctx->cfg_edges[bb->predecessors + i]);
		}
		fprintf(f, "]\n");
	}
	if (bb->dom_parent > 0) {
		fprintf(f, "\tdom_parent=BB%d\n", bb->dom_parent);
	}
	fprintf(f, "\tdom_depth=%d\n", bb->dom_depth);
	if (bb->dom_child > 0) {
		int child = bb->dom_child;
		fprintf(f, "\tdom_children [BB%d", child);
		child = ctx->cfg_blocks[child].dom_next_child;
		while (child > 0) {
			fprintf(f, ", BB%d", child);
			child = ctx->cfg_blocks[child].dom_next_child;
		}
		fprintf(f, "]\n");
	}
	if (bb->flags & IR_BB_ENTRY) {
		fprintf(f, "\tENTRY\n");
	}
	if (bb->flags & IR_BB_UNREACHABLE) {
		fprintf(f, "\tUNREACHABLE\n");
	}
	if (bb->flags & IR_BB_LOOP_HEADER) {
		if (bb->flags & IR_BB_LOOP_WITH_ENTRY) {
			fprintf(f, "\tLOOP_HEADER, LOOP_WITH_ENTRY\n");
		} else {
			fprintf(f, "\tLOOP_HEADER\n");
		}
	}
	if (bb->flags & IR_BB_IRREDUCIBLE_LOOP) {
		fprintf(stderr, "\tIRREDUCIBLE_LOOP\n");
	}
	if (bb->loop_header > 0) {
		fprintf(f, "\tloop_header=BB%d\n", bb->loop_header);
	}
	if (bb->loop_depth != 0) {
		fprintf(f, "\tloop_depth=%d\n", bb->loop_depth);
	}
	if (bb->flags & IR_BB_OSR_ENTRY_LOADS) {
		ir_list *list = (ir_list*)ctx->osr_entry_loads;
		uint32_t pos = 0, i, count;

		IR_ASSERT(list);
		while (1) {
			i = ir_list_at(list, pos);
			if (b == i) {
				break;
			}
			IR_ASSERT(i != 0); /* end marker */
			pos++;
			count = ir_list_at(list, pos);
			pos += count + 1;
		}
		pos++;
		count = ir_list_at(list, pos);
		pos++;

		for (i = 0; i < count; i++, pos++) {
			ir_ref ref = ir_list_at(list, pos);
			fprintf(f, "\tOSR_ENTRY_LOAD=d_%d\n", ref);
		}
	}
	if (bb->flags & IR_BB_DESSA_MOVES) {
		ir_dump_dessa_moves(ctx, b, bb, f);
	}
}

void ir_dump_cfg(ir_ctx *ctx, FILE *f)
{
	if (ctx->cfg_blocks) {
		uint32_t  b, i, bb_count = ctx->cfg_blocks_count;
		ir_block *bb = ctx->cfg_blocks + 1;

		fprintf(f, "{ # CFG\n");
		if (ctx->cfg_schedule) {
			for (i = 1; i <= bb_count; i++) {
				b = ctx->cfg_schedule[i];
				bb = &ctx->cfg_blocks[b];
				ir_dump_cfg_block(ctx, f, b, bb);
			}
		} else {
			for (b = 1; b <= bb_count; b++, bb++) {
				ir_dump_cfg_block(ctx, f, b, bb);
			}
		}
		fprintf(f, "}\n");
	}
}

void ir_dump_cfg_map(const ir_ctx *ctx, FILE *f)
{
	ir_ref i;
    uint32_t *_blocks = ctx->cfg_map;

    if (_blocks) {
		fprintf(f, "{ # CFG map (insn -> bb)\n");
		for (i = IR_UNUSED + 1; i < ctx->insns_count; i++) {
			fprintf(f, "%d -> %d\n", i, _blocks[i]);
		}
		fprintf(f, "}\n");
	}
}

void ir_dump_live_ranges(const ir_ctx *ctx, FILE *f)
{
    ir_ref i, j, n;

	if (!ctx->live_intervals) {
		return;
	}
	fprintf(f, "{ # LIVE-RANGES (vregs_count=%d)\n", ctx->vregs_count);
	for (i = 0; i <= ctx->vregs_count; i++) {
		ir_live_interval *ival = ctx->live_intervals[i];

		if (ival) {
			ir_live_range *p;
			ir_use_pos *use_pos;

			if (i == 0) {
				fprintf(f, "TMP");
			} else {
				for (j = 1; j < ctx->insns_count; j++) {
					if (ctx->vregs[j] == (uint32_t)i) {
						break;
					}
				}
				fprintf(f, "R%d (d_%d", i, j);
				for (j++; j < ctx->insns_count; j++) {
					if (ctx->vregs[j] == (uint32_t)i) {
						fprintf(f, ", d_%d", j);
					}
				}
				fprintf(f, ")");
				if (ival->stack_spill_pos != -1) {
					if (ival->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL) {
						IR_ASSERT(ctx->spill_base >= 0);
						fprintf(f, " [SPILL=0x%x(%%%s)]", ival->stack_spill_pos, ir_reg_name(ctx->spill_base, IR_ADDR));
					} else {
						fprintf(f, " [SPILL=0x%x]", ival->stack_spill_pos);
					}
				}
			}
			if (ival->next) {
				fprintf(f, "\n\t");
			} else if (ival->reg != IR_REG_NONE) {
				fprintf(f, " ");
			}
			do {
				if (ival->reg != IR_REG_NONE) {
					fprintf(f, "[%%%s]", ir_reg_name(ival->reg, ival->type));
				}
				p = &ival->range;
				fprintf(f, ": [%d.%d-%d.%d)",
					IR_LIVE_POS_TO_REF(p->start), IR_LIVE_POS_TO_SUB_REF(p->start),
					IR_LIVE_POS_TO_REF(p->end), IR_LIVE_POS_TO_SUB_REF(p->end));
				if (i == 0) {
					/* This is a TMP register */
					if (ival->tmp_ref == IR_LIVE_POS_TO_REF(p->start)) {
						fprintf(f, "/%d", ival->tmp_op_num);
					} else {
						fprintf(f, "/%d.%d", ival->tmp_ref, ival->tmp_op_num);
					}
				} else {
					p = p->next;
					while (p) {
						fprintf(f, ", [%d.%d-%d.%d)",
							IR_LIVE_POS_TO_REF(p->start), IR_LIVE_POS_TO_SUB_REF(p->start),
							IR_LIVE_POS_TO_REF(p->end), IR_LIVE_POS_TO_SUB_REF(p->end));
						p = p->next;
					}
				}
				use_pos = ival->use_pos;
				while (use_pos) {
					if (use_pos->flags & IR_PHI_USE) {
						IR_ASSERT(use_pos->op_num > 0);
						fprintf(f, ", PHI_USE(%d.%d, phi=d_%d/%d)",
							IR_LIVE_POS_TO_REF(use_pos->pos), IR_LIVE_POS_TO_SUB_REF(use_pos->pos),
							-use_pos->hint_ref, use_pos->op_num);
					} else if (use_pos->flags & IR_FUSED_USE) {
						fprintf(f, ", USE(%d.%d/%d.%d",
							IR_LIVE_POS_TO_REF(use_pos->pos), IR_LIVE_POS_TO_SUB_REF(use_pos->pos),
							-use_pos->hint_ref, use_pos->op_num);
						if (use_pos->hint >= 0) {
							fprintf(f, ", hint=%%%s", ir_reg_name(use_pos->hint, ival->type));
						}
						fprintf(f, ")");
						if (use_pos->flags & IR_USE_MUST_BE_IN_REG) {
							fprintf(f, "!");
						}
					} else {
						if (!use_pos->op_num) {
							fprintf(f, ", DEF(%d.%d",
								IR_LIVE_POS_TO_REF(use_pos->pos), IR_LIVE_POS_TO_SUB_REF(use_pos->pos));
						} else {
							fprintf(f, ", USE(%d.%d/%d",
								IR_LIVE_POS_TO_REF(use_pos->pos), IR_LIVE_POS_TO_SUB_REF(use_pos->pos),
								use_pos->op_num);
						}
						if (use_pos->hint >= 0) {
							fprintf(f, ", hint=%%%s", ir_reg_name(use_pos->hint, ival->type));
						}
						if (use_pos->hint_ref) {
							fprintf(f, ", hint=R%d", ctx->vregs[use_pos->hint_ref]);
						}
						fprintf(f, ")");
						if (use_pos->flags & IR_USE_MUST_BE_IN_REG) {
							fprintf(f, "!");
						}
					}
					use_pos = use_pos->next;
				}
				if (ival->next) {
					fprintf(f, "\n\t");
				}
				ival = ival->next;
			} while (ival);
			fprintf(f, "\n");
		}
	}
#if 1
	n = ctx->vregs_count + ir_regs_number() + 2;
	for (i = ctx->vregs_count + 1; i <= n; i++) {
		ir_live_interval *ival = ctx->live_intervals[i];

		if (ival) {
			ir_live_range *p = &ival->range;
			fprintf(f, "[%%%s] : [%d.%d-%d.%d)",
				ir_reg_name(ival->reg, ival->type),
				IR_LIVE_POS_TO_REF(p->start), IR_LIVE_POS_TO_SUB_REF(p->start),
				IR_LIVE_POS_TO_REF(p->end), IR_LIVE_POS_TO_SUB_REF(p->end));
			p = p->next;
			while (p) {
				fprintf(f, ", [%d.%d-%d.%d)",
					IR_LIVE_POS_TO_REF(p->start), IR_LIVE_POS_TO_SUB_REF(p->start),
					IR_LIVE_POS_TO_REF(p->end), IR_LIVE_POS_TO_SUB_REF(p->end));
				p = p->next;
			}
			fprintf(f, "\n");
		}
	}
#endif
	fprintf(f, "}\n");
}

void ir_dump_codegen(const ir_ctx *ctx, FILE *f)
{
	ir_ref i, j, n, ref, *p;
	ir_insn *insn;
	uint32_t flags, _b, b;
	ir_block *bb;
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

	for (_b = 1; _b <= ctx->cfg_blocks_count; _b++) {
		if (ctx->cfg_schedule) {
			b = ctx->cfg_schedule[_b];
		} else {
			b = _b;
		}
		bb = &ctx->cfg_blocks[b];
		if ((bb->flags & (IR_BB_START|IR_BB_ENTRY|IR_BB_EMPTY)) == IR_BB_EMPTY) {
			continue;
		}
		fprintf(f, "#BB%d:\n", b);

		for (i = bb->start, insn = ctx->ir_base + i; i <= bb->end;) {
			flags = ir_op_flags[insn->op];
			if (flags & IR_OP_FLAG_CONTROL) {
				if (!(flags & IR_OP_FLAG_MEM) || insn->type == IR_VOID) {
					fprintf(f, "\tl_%d = ", i);
				} else {
					fprintf(f, "\t%s d_%d", ir_type_cname[insn->type], i);
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
					fprintf(f, ", l_%d = ", i);
				}
			} else {
				fprintf(f, "\t");
				if (flags & IR_OP_FLAG_DATA) {
					fprintf(f, "%s d_%d", ir_type_cname[insn->type], i);
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
			if (((flags & IR_OP_FLAG_DATA) || ((flags & IR_OP_FLAG_MEM) && insn->type != IR_VOID)) && ctx->binding) {
				ir_ref var = ir_binding_find(ctx, i);
				if (var) {
					IR_ASSERT(var < 0);
					fprintf(f, " # BIND(0x%x);", -var);
				}
			}
			if (ctx->rules) {
				uint32_t rule = ctx->rules[i];
				uint32_t id = rule & IR_RULE_MASK;

				if (id < IR_LAST_OP) {
					fprintf(f, " # RULE(%s", ir_op_name[id]);
				} else {
					IR_ASSERT(id > IR_LAST_OP /*&& id < IR_LAST_RULE*/);
					fprintf(f, " # RULE(%s", ir_rule_name[id - IR_LAST_OP]);
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
				fprintf(f, ")");
			}
			fprintf(f, "\n");
			n = ir_insn_inputs_to_len(n);
			i += n;
			insn += n;
		}

		if (bb->flags & IR_BB_DESSA_MOVES) {
			ir_dump_dessa_moves(ctx, b, bb, f);
		}

		insn = &ctx->ir_base[bb->end];
		if (insn->op == IR_END || insn->op == IR_LOOP_END) {
			uint32_t succ;

			if (bb->successors_count == 1) {
				succ = ctx->cfg_edges[bb->successors];
			} else {
				/* END may have a fake control edge to ENTRY */
				IR_ASSERT(bb->successors_count == 2);
				succ = ctx->cfg_edges[bb->successors];
				if (ctx->ir_base[ctx->cfg_blocks[succ].start].op == IR_ENTRY) {
					succ = ctx->cfg_edges[bb->successors + 1];
#ifdef IR_DEBUG
				} else {
					uint32_t fake_succ = ctx->cfg_edges[bb->successors + 1];
					IR_ASSERT(ctx->ir_base[ctx->cfg_blocks[fake_succ].start].op == IR_ENTRY);
#endif
				}
			}
			succ = ir_skip_empty_target_blocks(ctx, succ);
			if (succ != b + 1) {
				fprintf(f, "\t# GOTO BB%d\n", succ);
			}
		} else if (insn->op == IR_IF) {
			uint32_t true_block, false_block;

			ir_get_true_false_blocks(ctx, b, &true_block, &false_block);
			fprintf(f, "\t# IF_TRUE BB%d, IF_FALSE BB%d\n", true_block, false_block);
		} else if (insn->op == IR_SWITCH) {
			fprintf(f, "\t# SWITCH ...\n");
		}
	}
	fprintf(f, "}\n");
}
