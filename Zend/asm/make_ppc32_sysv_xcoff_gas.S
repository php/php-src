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
 *  |bchai|savLR|savLR|compl| link|svTOC| FCTX| DATA|  *
 *  -------------------------------------------------  *
 *                                                     *
 *******************************************************/
    .file "make_ppc32_sysv_xcoff_xas.S"
    .toc
    .csect .text[PR]
    .align 2
    .globl  make_fcontext[DS]
    .globl .make_fcontext
    .csect  make_fcontext[DS]
make_fcontext:
    .long .make_fcontext[PR], TOC[tc0], 0
    .csect .text[PR], 5
.make_fcontext:
    # save return address into R6
    mflr  6

    # first arg of make_fcontext() == top address of context-function
    # shift address in R3 to lower 16 byte boundary
    clrrwi  3, 3, 4

    # reserve space for context-data on context-stack
    # including 32 byte of linkage + parameter area (R1 % 16 == 0)
    subi  3, 3, 288

    # third arg of make_fcontext() == address of context-function descriptor
    lwz   4, 0(5)
    stw  4, 240(3)
    # save TOC of context-function
    lwz   4, 4(5)
    stw  4, 20(3)

    # set back-chain to zero
    li   0, 0
    stw  0, 256(3)

    # zero in r3 indicates first jump to context-function
    std  0, 244(3)

    # load LR
    mflr  0
    # jump to label 1
    bl  .Label
.Label:
    # load LR into R4
    mflr  4
    # compute abs address of label .L_finish
    addi  4, 4, .L_finish - .Label
    # restore LR
    mtlr  0
    # save address of finish as return-address for context-function
    # will be entered after context-function returns
    stw  4, 8(3)

    # restore return address from R6
    mtlr  6

    blr  # return pointer to context-data

.L_finish:
    # save return address into R0
    mflr  0
    # save return address on stack, set up stack frame
    stw  0, 4(1)
    # allocate stack space, R1 % 16 == 0
    stwu  1, -16(1)

    # exit code is zero
    li  3, 0
    # exit application
    bl  ._exit
    nop
