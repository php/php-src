/* udis86 - libudis86/syn-att.c
 *
 * Copyright (c) 2002-2009 Vivek Thampi
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, 
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, 
 *       this list of conditions and the following disclaimer in the documentation 
 *       and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "types.h"
#include "extern.h"
#include "decode.h"
#include "itab.h"
#include "syn.h"
#include "udint.h"

/* -----------------------------------------------------------------------------
 * opr_cast() - Prints an operand cast.
 * -----------------------------------------------------------------------------
 */
static void 
opr_cast(struct ud* u, struct ud_operand* op)
{
  switch(op->size) {
  case 16 : case 32 :
    ud_asmprintf(u, "*");   break;
  default: break;
  }
}

/* -----------------------------------------------------------------------------
 * gen_operand() - Generates assembly output for each operand.
 * -----------------------------------------------------------------------------
 */
static void 
gen_operand(struct ud* u, struct ud_operand* op)
{
  switch(op->type) {
  case UD_OP_CONST:
    ud_asmprintf(u, "$0x%x", op->lval.udword);
    break;

  case UD_OP_REG:
    ud_asmprintf(u, "%%%s", ud_reg_tab[op->base - UD_R_AL]);
    break;

  case UD_OP_MEM:
    if (u->br_far) {
        opr_cast(u, op);
    }
    if (u->pfx_seg) {
      ud_asmprintf(u, "%%%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
    }
    if (op->offset != 0) { 
      ud_syn_print_mem_disp(u, op, 0);
    }
    if (op->base) {
      ud_asmprintf(u, "(%%%s", ud_reg_tab[op->base - UD_R_AL]);
    }
    if (op->index) {
      if (op->base) {
        ud_asmprintf(u, ",");
      } else {
        ud_asmprintf(u, "(");
      }
      ud_asmprintf(u, "%%%s", ud_reg_tab[op->index - UD_R_AL]);
    }
    if (op->scale) {
      ud_asmprintf(u, ",%d", op->scale);
    }
    if (op->base || op->index) {
      ud_asmprintf(u, ")");
    }
    break;

  case UD_OP_IMM:
    ud_asmprintf(u, "$");
    ud_syn_print_imm(u, op);
    break;

  case UD_OP_JIMM:
    ud_syn_print_addr(u, ud_syn_rel_target(u, op));
    break;

  case UD_OP_PTR:
    switch (op->size) {
      case 32:
        ud_asmprintf(u, "$0x%x, $0x%x", op->lval.ptr.seg, 
          op->lval.ptr.off & 0xFFFF);
        break;
      case 48:
        ud_asmprintf(u, "$0x%x, $0x%x", op->lval.ptr.seg, 
          op->lval.ptr.off);
        break;
    }
    break;
      
  default: return;
  }
}

/* =============================================================================
 * translates to AT&T syntax 
 * =============================================================================
 */
extern void 
ud_translate_att(struct ud *u)
{
  int size = 0;
  int star = 0;

  /* check if P_OSO prefix is used */
  if (! P_OSO(u->itab_entry->prefix) && u->pfx_opr) {
  switch (u->dis_mode) {
    case 16: 
      ud_asmprintf(u, "o32 ");
      break;
    case 32:
    case 64:
      ud_asmprintf(u, "o16 ");
      break;
  }
  }

  /* check if P_ASO prefix was used */
  if (! P_ASO(u->itab_entry->prefix) && u->pfx_adr) {
  switch (u->dis_mode) {
    case 16: 
      ud_asmprintf(u, "a32 ");
      break;
    case 32:
      ud_asmprintf(u, "a16 ");
      break;
    case 64:
      ud_asmprintf(u, "a32 ");
      break;
  }
  }

  if (u->pfx_lock)
    ud_asmprintf(u,  "lock ");
  if (u->pfx_rep) {
    ud_asmprintf(u, "rep ");
  } else if (u->pfx_repe) {
    ud_asmprintf(u, "repe ");
  } else if (u->pfx_repne) {
    ud_asmprintf(u, "repne ");
  }

  /* special instructions */
  switch (u->mnemonic) {
  case UD_Iretf: 
    ud_asmprintf(u, "lret "); 
    break;
  case UD_Idb:
    ud_asmprintf(u, ".byte 0x%x", u->operand[0].lval.ubyte);
    return;
  case UD_Ijmp:
  case UD_Icall:
    if (u->br_far) ud_asmprintf(u,  "l");
        if (u->operand[0].type == UD_OP_REG) {
          star = 1;
        }
    ud_asmprintf(u, "%s", ud_lookup_mnemonic(u->mnemonic));
    break;
  case UD_Ibound:
  case UD_Ienter:
    if (u->operand[0].type != UD_NONE)
      gen_operand(u, &u->operand[0]);
    if (u->operand[1].type != UD_NONE) {
      ud_asmprintf(u, ",");
      gen_operand(u, &u->operand[1]);
    }
    return;
  default:
    ud_asmprintf(u, "%s", ud_lookup_mnemonic(u->mnemonic));
  }

  if (size == 8) {
    ud_asmprintf(u, "b");
  } else if (size == 16) {
    ud_asmprintf(u, "w");
  } else if (size == 64) {
    ud_asmprintf(u, "q");
  }

  if (star) {
    ud_asmprintf(u, " *");
  } else {
    ud_asmprintf(u, " ");
  }

  if (u->operand[3].type != UD_NONE) {
    gen_operand(u, &u->operand[3]);
    ud_asmprintf(u, ", ");
  }
  if (u->operand[2].type != UD_NONE) {
    gen_operand(u, &u->operand[2]);
    ud_asmprintf(u, ", ");
  }
  if (u->operand[1].type != UD_NONE) {
    gen_operand(u, &u->operand[1]);
    ud_asmprintf(u, ", ");
  }
  if (u->operand[0].type != UD_NONE) {
    gen_operand(u, &u->operand[0]);
  }
}

/*
vim: set ts=2 sw=2 expandtab
*/
