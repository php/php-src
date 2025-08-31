/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************
 *                                                     *
 *  -------------------------------------------------  *
 *  |  0  |  4  |  8  |  12 |  16 |  20 |  24 |  28 |  *
 *  -------------------------------------------------  *
 *  |bchai|  CR | LR  |compl| link| TOC | R14 | R15 |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  32 |  36 |  40 |  44 |  48 |  52 |  56 |  60 |  *
 *  -------------------------------------------------  *
 *  | R16 | R17 | R18 | R19 | R20 | R21 | R22 | R23 |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  64 |  68 |  72 |  76 |  80 |  84 |  88 |  92 |  *
 *  -------------------------------------------------  *
 *  | R24 | R25 | R26 | R27 | R28 | R29 | R30 | R31 |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  96 | 100 | 104 | 108 | 112 | 116 | 120 | 124 |  *
 *  -------------------------------------------------  *
 *  |    F14    |    F15    |    F16    |    F17    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  | 128 | 132 | 136 | 140 | 144 | 148 | 152 | 156 |  *
 *  -------------------------------------------------  *
 *  |    F18    |    F19    |    F20    |    F21    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  | 160 | 164 | 168 | 172 | 176 | 180 | 184 | 188 |  *
 *  -------------------------------------------------  *
 *  |    F22    |    F23    |    F24    |    F25    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  | 192 | 196 | 200 | 204 | 208 | 212 | 216 | 220 |  *
 *  -------------------------------------------------  *
 *  |    F26    |    F27    |    F28    |    F29    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  | 224 | 228 | 232 | 236 | 240 | 244 | 248 | 252 |  *
 *  -------------------------------------------------  *
 *  |    F30    |    F31    |  PC |hiddn|  fpscr    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  | 256 | 260 | 264 | 268 | 272 | 276 | 280 | 284 |  *
 *  -------------------------------------------------  *
 *  |bchai|savCR|savLR|compl| link|svTOC| FCTX| DATA|  *
 *  -------------------------------------------------  *
 *                                                     *
 *******************************************************/

    .file "jump_ppc32_sysv_xcoff_gas.S"
    .toc
    .csect .text[PR], 5
    .globl  jump_fcontext[DS]
    .globl .jump_fcontext
    .csect  jump_fcontext[DS]
jump_fcontext:
    .long .jump_fcontext[PR], TOC[tc0], 0
    .csect .text[PR], 5
.jump_fcontext:
    # reserve space on stack
    subi  1, 1, 256

    # save CR
    mfcr  0
    stw   0, 4(1)
    # save LR
    mflr  0
    stw   0, 8(1)
    # save LR as PC
    stw   0, 240(1)
    # save TOC
    stw   2, 20(1)

    # Save registers R14 to R31.
    stw   14, 24(1)
    stw   15, 28(1)
    stw   16, 32(1)
    stw   17, 36(1)
    stw   18, 40(1)
    stw   19, 44(1)
    stw   20, 48(1)
    stw   21, 52(1)
    stw   22, 56(1)
    stw   23, 60(1)
    stw   24, 64(1)
    stw   25, 68(1)
    stw   26, 72(1)
    stw   27, 76(1)
    stw   28, 80(1)
    stw   29, 84(1)
    stw   30, 88(1)
    stw   31, 92(1)

    # Save registers F14 to F31 in slots with 8-byte alignment.
    # 4-byte alignment may stall the pipeline of some processors.
    # Less than 4 may cause alignment traps.
    stfd  14, 96(1)
    stfd  15, 104(1)
    stfd  16, 112(1)
    stfd  17, 120(1)
    stfd  18, 128(1)
    stfd  19, 136(1)
    stfd  20, 144(1)
    stfd  21, 152(1)
    stfd  22, 160(1)
    stfd  23, 168(1)
    stfd  24, 176(1)
    stfd  25, 184(1)
    stfd  26, 192(1)
    stfd  27, 200(1)
    stfd  28, 208(1)
    stfd  29, 216(1)
    stfd  30, 224(1)
    stfd  31, 232(1)

    # hidden pointer
    stw   3, 244(1)

    mffs  0  # load FPSCR
    stfd  0, 248(1)  # save FPSCR


    # store RSP (pointing to context-data) in R6
    mr  6, 1

    # restore RSP (pointing to context-data) from R4
    mr  1, 4

    # restore CR
    lwz   0, 4(1)
    mtcr  0
    # restore LR
    lwz   0, 8(1)
    mtlr  0
    # load PC
    lwz   0, 240(1)
    mtctr 0

    # restore TOC
    lwz   2, 20(1)

    # restore R14 to R31
    lwz  14, 24(1)
    lwz  15, 28(1)
    lwz  16, 32(1)
    lwz  17, 36(1)
    lwz  18, 40(1)
    lwz  19, 44(1)
    lwz  20, 48(1)
    lwz  21, 52(1)
    lwz  22, 56(1)
    lwz  23, 60(1)
    lwz  24, 64(1)
    lwz  25, 68(1)
    lwz  26, 72(1)
    lwz  27, 76(1)
    lwz  28, 80(1)
    lwz  29, 84(1)
    lwz  30, 88(1)
    lwz  31, 92(1)

    # restore F14 to F31
    lfd  14, 96(1)
    lfd  15, 104(1)
    lfd  16, 112(1)
    lfd  17, 120(1)
    lfd  18, 128(1)
    lfd  19, 136(1)
    lfd  20, 144(1)
    lfd  21, 152(1)
    lfd  22, 160(1)
    lfd  23, 168(1)
    lfd  24, 176(1)
    lfd  25, 184(1)
    lfd  26, 192(1)
    lfd  27, 200(1)
    lfd  28, 208(1)
    lfd  29, 216(1)
    lfd  30, 224(1)
    lfd  31, 232(1)

    # hidden pointer
    lwz   3, 244(1)

    lfd  0,  248(1)  # load FPSCR
    mtfsf  0xff, 0  # restore FPSCR

    # adjust stack
    addi  1, 1, 256

    # zero in r3 indicates first jump to context-function
    cmpdi 3, 0
    beq use_entry_arg

    # return transfer_t
    stw  6, 0(3)
    stw  5, 4(3)

    # jump to context
    bctr

use_entry_arg:
    # copy transfer_t into transfer_fn arg registers
    mr  3, 6
    mr  4, 5

    # jump to context
    bctr
