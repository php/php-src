/* udis86 - libudis86/udis86.c
 *
 * Copyright (c) 2002-2013 Vivek Thampi
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

#include "udint.h"
#include "extern.h"
#include "decode.h"

#if !defined(__UD_STANDALONE__)
# if HAVE_STRING_H
#  include <string.h>
# endif
#endif /* !__UD_STANDALONE__ */

static void ud_inp_init(struct ud *u);

/* =============================================================================
 * ud_init
 *    Initializes ud_t object.
 * =============================================================================
 */
extern void 
ud_init(struct ud* u)
{
  memset((void*)u, 0, sizeof(struct ud));
  ud_set_mode(u, 16);
  u->mnemonic = UD_Iinvalid;
  ud_set_pc(u, 0);
#ifndef __UD_STANDALONE__
  ud_set_input_file(u, stdin);
#endif /* __UD_STANDALONE__ */

  ud_set_asm_buffer(u, u->asm_buf_int, sizeof(u->asm_buf_int));
}


/* =============================================================================
 * ud_disassemble
 *    Disassembles one instruction and returns the number of 
 *    bytes disassembled. A zero means end of disassembly.
 * =============================================================================
 */
extern unsigned int
ud_disassemble(struct ud* u)
{
  int len;
  if (u->inp_end) {
    return 0;
  }
  if ((len = ud_decode(u)) > 0) {
    if (u->translator != NULL) {
      u->asm_buf[0] = '\0';
      u->translator(u);
    }
  }
  return len;
}


/* =============================================================================
 * ud_set_mode() - Set Disassemly Mode.
 * =============================================================================
 */
extern void 
ud_set_mode(struct ud* u, uint8_t m)
{
  switch(m) {
  case 16:
  case 32:
  case 64: u->dis_mode = m ; return;
  default: u->dis_mode = 16; return;
  }
}

/* =============================================================================
 * ud_set_vendor() - Set vendor.
 * =============================================================================
 */
extern void 
ud_set_vendor(struct ud* u, unsigned v)
{
  switch(v) {
  case UD_VENDOR_INTEL:
    u->vendor = v;
    break;
  case UD_VENDOR_ANY:
    u->vendor = v;
    break;
  default:
    u->vendor = UD_VENDOR_AMD;
  }
}

/* =============================================================================
 * ud_set_pc() - Sets code origin. 
 * =============================================================================
 */
extern void 
ud_set_pc(struct ud* u, uint64_t o)
{
  u->pc = o;
}

/* =============================================================================
 * ud_set_syntax() - Sets the output syntax.
 * =============================================================================
 */
extern void 
ud_set_syntax(struct ud* u, void (*t)(struct ud*))
{
  u->translator = t;
}

/* =============================================================================
 * ud_insn() - returns the disassembled instruction
 * =============================================================================
 */
const char* 
ud_insn_asm(const struct ud* u) 
{
  return u->asm_buf;
}

/* =============================================================================
 * ud_insn_offset() - Returns the offset.
 * =============================================================================
 */
uint64_t
ud_insn_off(const struct ud* u) 
{
  return u->insn_offset;
}


/* =============================================================================
 * ud_insn_hex() - Returns hex form of disassembled instruction.
 * =============================================================================
 */
const char* 
ud_insn_hex(struct ud* u) 
{
  u->insn_hexcode[0] = 0;
  if (!u->error) {
    unsigned int i;
    const unsigned char *src_ptr = ud_insn_ptr(u);
    char* src_hex;
    src_hex = (char*) u->insn_hexcode;
    /* for each byte used to decode instruction */
    for (i = 0; i < ud_insn_len(u) && i < sizeof(u->insn_hexcode) / 2;
         ++i, ++src_ptr) {
      sprintf(src_hex, "%02x", *src_ptr & 0xFF);
      src_hex += 2;
    }
  }
  return u->insn_hexcode;
}


/* =============================================================================
 * ud_insn_ptr
 *    Returns a pointer to buffer containing the bytes that were
 *    disassembled.
 * =============================================================================
 */
extern const uint8_t* 
ud_insn_ptr(const struct ud* u) 
{
  return (u->inp_buf == NULL) ? 
            u->inp_sess : u->inp_buf + (u->inp_buf_index - u->inp_ctr);
}


/* =============================================================================
 * ud_insn_len
 *    Returns the count of bytes disassembled.
 * =============================================================================
 */
extern unsigned int 
ud_insn_len(const struct ud* u) 
{
  return u->inp_ctr;
}


/* =============================================================================
 * ud_insn_get_opr
 *    Return the operand struct representing the nth operand of
 *    the currently disassembled instruction. Returns NULL if
 *    there's no such operand.
 * =============================================================================
 */
const struct ud_operand*
ud_insn_opr(const struct ud *u, unsigned int n)
{
  if (n > 3 || u->operand[n].type == UD_NONE) {
    return NULL; 
  } else {
    return &u->operand[n];
  }
}


/* =============================================================================
 * ud_opr_is_sreg
 *    Returns non-zero if the given operand is of a segment register type.
 * =============================================================================
 */
int
ud_opr_is_sreg(const struct ud_operand *opr)
{
  return opr->type == UD_OP_REG && 
         opr->base >= UD_R_ES   &&
         opr->base <= UD_R_GS;
}


/* =============================================================================
 * ud_opr_is_sreg
 *    Returns non-zero if the given operand is of a general purpose
 *    register type.
 * =============================================================================
 */
int
ud_opr_is_gpr(const struct ud_operand *opr)
{
  return opr->type == UD_OP_REG && 
         opr->base >= UD_R_AL   &&
         opr->base <= UD_R_R15;
}


/* =============================================================================
 * ud_set_user_opaque_data
 * ud_get_user_opaque_data
 *    Get/set user opaqute data pointer
 * =============================================================================
 */
void
ud_set_user_opaque_data(struct ud * u, void* opaque)
{
  u->user_opaque_data = opaque;
}

void*
ud_get_user_opaque_data(const struct ud *u)
{
  return u->user_opaque_data;
}


/* =============================================================================
 * ud_set_asm_buffer
 *    Allow the user to set an assembler output buffer. If `buf` is NULL,
 *    we switch back to the internal buffer.
 * =============================================================================
 */
void
ud_set_asm_buffer(struct ud *u, char *buf, size_t size)
{
  if (buf == NULL) {
    ud_set_asm_buffer(u, u->asm_buf_int, sizeof(u->asm_buf_int));
  } else {
    u->asm_buf = buf;
    u->asm_buf_size = size;
  }
}


/* =============================================================================
 * ud_set_sym_resolver
 *    Set symbol resolver for relative targets used in the translation
 *    phase.
 *
 *    The resolver is a function that takes a uint64_t address and returns a
 *    symbolic name for the that address. The function also takes a second
 *    argument pointing to an integer that the client can optionally set to a
 *    non-zero value for offsetted targets. (symbol+offset) The function may
 *    also return NULL, in which case the translator only prints the target
 *    address.
 *
 *    The function pointer maybe NULL which resets symbol resolution.
 * =============================================================================
 */
void
ud_set_sym_resolver(struct ud *u, const char* (*resolver)(struct ud*, 
                                                          uint64_t addr,
                                                          int64_t *offset))
{
  u->sym_resolver = resolver;
}


/* =============================================================================
 * ud_insn_mnemonic
 *    Return the current instruction mnemonic.
 * =============================================================================
 */
enum ud_mnemonic_code
ud_insn_mnemonic(const struct ud *u)
{
  return u->mnemonic;
}


/* =============================================================================
 * ud_lookup_mnemonic
 *    Looks up mnemonic code in the mnemonic string table.
 *    Returns NULL if the mnemonic code is invalid.
 * =============================================================================
 */
const char*
ud_lookup_mnemonic(enum ud_mnemonic_code c)
{
  if (c < UD_MAX_MNEMONIC_CODE) {
    return ud_mnemonics_str[c];
  } else {
    return NULL;
  }
}


/* 
 * ud_inp_init
 *    Initializes the input system.
 */
static void
ud_inp_init(struct ud *u)
{
  u->inp_hook      = NULL;
  u->inp_buf       = NULL;
  u->inp_buf_size  = 0;
  u->inp_buf_index = 0;
  u->inp_curr      = 0;
  u->inp_ctr       = 0;
  u->inp_end       = 0;
  u->inp_peek      = UD_EOI;
  UD_NON_STANDALONE(u->inp_file = NULL);
}


/* =============================================================================
 * ud_inp_set_hook
 *    Sets input hook.
 * =============================================================================
 */
void 
ud_set_input_hook(register struct ud* u, int (*hook)(struct ud*))
{
  ud_inp_init(u);
  u->inp_hook = hook;
}

/* =============================================================================
 * ud_inp_set_buffer
 *    Set buffer as input.
 * =============================================================================
 */
void 
ud_set_input_buffer(register struct ud* u, const uint8_t* buf, size_t len)
{
  ud_inp_init(u);
  u->inp_buf = buf;
  u->inp_buf_size = len;
  u->inp_buf_index = 0;
}


#ifndef __UD_STANDALONE__
/* =============================================================================
 * ud_input_set_file
 *    Set FILE as input.
 * =============================================================================
 */
static int 
inp_file_hook(struct ud* u)
{
  return fgetc(u->inp_file);
}

void 
ud_set_input_file(register struct ud* u, FILE* f)
{
  ud_inp_init(u);
  u->inp_hook = inp_file_hook;
  u->inp_file = f;
}
#endif /* __UD_STANDALONE__ */


/* =============================================================================
 * ud_input_skip
 *    Skip n input bytes.
 * ============================================================================
 */
void 
ud_input_skip(struct ud* u, size_t n)
{
  if (u->inp_end) {
    return;
  }
  if (u->inp_buf == NULL) {
    while (n--) {
      int c = u->inp_hook(u);
      if (c == UD_EOI) {
        goto eoi;
      }
    }
    return;
  } else {
    if (n > u->inp_buf_size ||
        u->inp_buf_index > u->inp_buf_size - n) {
      u->inp_buf_index = u->inp_buf_size; 
      goto eoi;
    }
    u->inp_buf_index += n; 
    return;
  }
eoi:
  u->inp_end = 1;
  UDERR(u, "cannot skip, eoi received\b");
  return;
}


/* =============================================================================
 * ud_input_end
 *    Returns non-zero on end-of-input.
 * =============================================================================
 */
int
ud_input_end(const struct ud *u)
{
  return u->inp_end;
}

/* vim:set ts=2 sw=2 expandtab */
