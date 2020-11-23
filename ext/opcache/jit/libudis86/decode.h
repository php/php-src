/* udis86 - libudis86/decode.h
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
#ifndef UD_DECODE_H
#define UD_DECODE_H

#include "types.h"
#include "udint.h"
#include "itab.h"

#define MAX_INSN_LENGTH 15

/* itab prefix bits */
#define P_none          ( 0 )

#define P_inv64         ( 1 << 0 )
#define P_INV64(n)      ( ( n >> 0 ) & 1 )
#define P_def64         ( 1 << 1 )
#define P_DEF64(n)      ( ( n >> 1 ) & 1 )

#define P_oso           ( 1 << 2 )
#define P_OSO(n)        ( ( n >> 2 ) & 1 )
#define P_aso           ( 1 << 3 )
#define P_ASO(n)        ( ( n >> 3 ) & 1 )

#define P_rexb          ( 1 << 4 )
#define P_REXB(n)       ( ( n >> 4 ) & 1 )
#define P_rexw          ( 1 << 5 )
#define P_REXW(n)       ( ( n >> 5 ) & 1 )
#define P_rexr          ( 1 << 6 )
#define P_REXR(n)       ( ( n >> 6 ) & 1 )
#define P_rexx          ( 1 << 7 )
#define P_REXX(n)       ( ( n >> 7 ) & 1 )

#define P_seg           ( 1 << 8 )
#define P_SEG(n)        ( ( n >> 8 ) & 1 )

#define P_vexl          ( 1 << 9 )
#define P_VEXL(n)       ( ( n >> 9 ) & 1 )
#define P_vexw          ( 1 << 10 )
#define P_VEXW(n)       ( ( n >> 10 ) & 1 )

#define P_str           ( 1 << 11 )
#define P_STR(n)        ( ( n >> 11 ) & 1 )
#define P_strz          ( 1 << 12 )
#define P_STR_ZF(n)     ( ( n >> 12 ) & 1 )

/* operand type constants -- order is important! */

enum ud_operand_code {
    OP_NONE,

    OP_A,      OP_E,      OP_M,       OP_G,       
    OP_I,      OP_F,

    OP_R0,     OP_R1,     OP_R2,      OP_R3,
    OP_R4,     OP_R5,     OP_R6,      OP_R7,

    OP_AL,     OP_CL,     OP_DL,
    OP_AX,     OP_CX,     OP_DX,
    OP_eAX,    OP_eCX,    OP_eDX,
    OP_rAX,    OP_rCX,    OP_rDX,

    OP_ES,     OP_CS,     OP_SS,      OP_DS,  
    OP_FS,     OP_GS,

    OP_ST0,    OP_ST1,    OP_ST2,     OP_ST3,
    OP_ST4,    OP_ST5,    OP_ST6,     OP_ST7,

    OP_J,      OP_S,      OP_O,          
    OP_I1,     OP_I3,     OP_sI,

    OP_V,      OP_W,      OP_Q,       OP_P, 
    OP_U,      OP_N,      OP_MU,      OP_H,
    OP_L,

    OP_R,      OP_C,      OP_D,       

    OP_MR
} UD_ATTR_PACKED;


/*
 * Operand size constants
 *
 *  Symbolic constants for various operand sizes. Some of these constants
 *  are given a value equal to the width of the data (SZ_B == 8), such
 *  that they maybe used interchangeably in the internals. Modifying them
 *  will most certainly break things!
 */
typedef uint16_t ud_operand_size_t;

#define SZ_NA  0
#define SZ_Z   1
#define SZ_V   2
#define SZ_Y   3
#define SZ_X   4
#define SZ_RDQ 7
#define SZ_B   8
#define SZ_W   16
#define SZ_D   32
#define SZ_Q   64
#define SZ_T   80
#define SZ_O   12
#define SZ_DQ  128 /* double quad */
#define SZ_QQ  256 /* quad quad */

/*
 * Complex size types; that encode sizes for operands of type MR (memory or
 * register); for internal use only. Id space above 256.
 */
#define SZ_BD  ((SZ_B << 8) | SZ_D)
#define SZ_BV  ((SZ_B << 8) | SZ_V)
#define SZ_WD  ((SZ_W << 8) | SZ_D)
#define SZ_WV  ((SZ_W << 8) | SZ_V)
#define SZ_WY  ((SZ_W << 8) | SZ_Y)
#define SZ_DY  ((SZ_D << 8) | SZ_Y)
#define SZ_WO  ((SZ_W << 8) | SZ_O)
#define SZ_DO  ((SZ_D << 8) | SZ_O)
#define SZ_QO  ((SZ_Q << 8) | SZ_O)


/* resolve complex size type.
 */
static UD_INLINE ud_operand_size_t
Mx_mem_size(ud_operand_size_t size)
{
  return (size >> 8) & 0xff;
}

static UD_INLINE ud_operand_size_t
Mx_reg_size(ud_operand_size_t size)
{
  return size & 0xff;
}

/* A single operand of an entry in the instruction table. 
 * (internal use only)
 */
struct ud_itab_entry_operand 
{
  enum ud_operand_code type;
  ud_operand_size_t size;
};


/* A single entry in an instruction table. 
 *(internal use only)
 */
struct ud_itab_entry 
{
  enum ud_mnemonic_code         mnemonic;
  struct ud_itab_entry_operand  operand1;
  struct ud_itab_entry_operand  operand2;
  struct ud_itab_entry_operand  operand3;
  struct ud_itab_entry_operand  operand4;
  uint32_t                      prefix;
};

struct ud_lookup_table_list_entry {
    const uint16_t *table;
    enum ud_table_type type;
    const char *meta;
};
     
extern struct ud_itab_entry ud_itab[];
extern struct ud_lookup_table_list_entry ud_lookup_table_list[];

#endif /* UD_DECODE_H */

/* vim:cindent
 * vim:expandtab
 * vim:ts=4
 * vim:sw=4
 */
