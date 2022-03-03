/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************
 *                                                     *
 *  -------------------------------------------------  *
 *  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  *
 *  -------------------------------------------------  *
 *  |  0  |  4  |  8  |  12 |  16 |  20 |  24 |  28 |  *
 *  -------------------------------------------------  *
 *  |    F20    |    F22    |    F24    |    F26    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
 *  -------------------------------------------------  *
 *  |  32 |  36 |  40 |  44 |  48 |  52 |  56 |  60 |  *
 *  -------------------------------------------------  *
 *  |    F28    |    F30    |  S0 |  S1 |  S2 |  S3 |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
 *  -------------------------------------------------  *
 *  |  64 |  68 |  72 |  76 |  80 |  84 |  88 |  92 |  *
 *  -------------------------------------------------  *
 *  |  S4 |  S5 |  S6 |  S7 |  FP |hiddn|  RA |  PC |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  *
 *  -------------------------------------------------  *
 *  |  96 | 100 | 104 | 108 | 112 | 116 | 120 | 124 |  *
 *  -------------------------------------------------  *
 *  |       ABI ARGS        |  GP | FCTX| DATA|     |  *
 *  -------------------------------------------------  *
 *                                                     *
 * *****************************************************/

.file "jump_mips32_o32_elf_gas.S"
.text
.globl jump_fcontext
.align 2
.type jump_fcontext,@function
.ent jump_fcontext
jump_fcontext:
    # reserve space on stack
    addiu $sp, $sp, -96

    sw  $s0, 48($sp)  # save S0
    sw  $s1, 52($sp)  # save S1
    sw  $s2, 56($sp)  # save S2
    sw  $s3, 60($sp)  # save S3
    sw  $s4, 64($sp)  # save S4
    sw  $s5, 68($sp)  # save S5
    sw  $s6, 72($sp)  # save S6
    sw  $s7, 76($sp)  # save S7
    sw  $fp, 80($sp)  # save FP
    sw  $a0, 84($sp)  # save hidden, address of returned transfer_t
    sw  $ra, 88($sp)  # save RA
    sw  $ra, 92($sp)  # save RA as PC

#if defined(__mips_hard_float)
    s.d  $f20, ($sp)  # save F20
    s.d  $f22, 8($sp)  # save F22
    s.d  $f24, 16($sp)  # save F24
    s.d  $f26, 24($sp)  # save F26
    s.d  $f28, 32($sp)  # save F28
    s.d  $f30, 40($sp)  # save F30
#endif

    # store SP (pointing to context-data) in A0
    move  $a0, $sp

    # restore SP (pointing to context-data) from A1
    move  $sp, $a1

#if defined(__mips_hard_float)
    l.d  $f20, ($sp)  # restore F20
    l.d  $f22, 8($sp)  # restore F22
    l.d  $f24, 16($sp)  # restore F24
    l.d  $f26, 24($sp)  # restore F26
    l.d  $f28, 32($sp)  # restore F28
    l.d  $f30, 40($sp)  # restore F30
#endif

    lw  $s0, 48($sp)  # restore S0
    lw  $s1, 52($sp)  # restore S1
    lw  $s2, 56($sp)  # restore S2
    lw  $s3, 60($sp)  # restore S3
    lw  $s4, 64($sp)  # restore S4
    lw  $s5, 68($sp)  # restore S5
    lw  $s6, 72($sp)  # restore S6
    lw  $s7, 76($sp)  # restore S7
    lw  $fp, 80($sp)  # restore FP
    lw  $v0, 84($sp)  # restore hidden, address of returned transfer_t
    lw  $ra, 88($sp)  # restore RA

    # load PC
    lw  $t9, 92($sp)

    # adjust stack
    addiu $sp, $sp, 96
    
    # return transfer_t from jump
    sw  $a0, ($v0)  # fctx of transfer_t
    sw  $a2, 4($v0) # data of transfer_t
    # pass transfer_t as first arg in context function
    # A0 == fctx, A1 == data
    move  $a1, $a2 

    # jump to context
    jr  $t9
.end jump_fcontext
.size jump_fcontext, .-jump_fcontext

/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
