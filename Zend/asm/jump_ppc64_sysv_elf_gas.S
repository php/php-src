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
 *  |    TOC    |    R14    |    R15    |    R16    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
 *  -------------------------------------------------  *
 *  |  32 |  36 |  40 |  44 |  48 |  52 |  56 |  60 |  *
 *  -------------------------------------------------  *
 *  |    R17    |    R18    |     R19   |    R20    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
 *  -------------------------------------------------  *
 *  |  64 |  68 |  72 |  76 |  80 |  84 |  88 |  92 |  *
 *  -------------------------------------------------  *
 *  |    R21    |    R22    |    R23    |    R24    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  *
 *  -------------------------------------------------  *
 *  |  96 | 100 | 104 | 108 | 112 | 116 | 120 | 124 |  *
 *  -------------------------------------------------  *
 *  |    R25    |    R26    |    R27    |    R28    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  32 |  33 |  34 |  35 |  36 |  37 |  38 |  39 |  *
 *  -------------------------------------------------  *
 *  | 128 | 132 | 136 | 140 | 144 | 148 | 152 | 156 |  *
 *  -------------------------------------------------  *
 *  |    R29    |    R30    |    R31    |   hidden  |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  40 |  41 |  42 |  43 |  44 |  45 |  46 |  47 |  *
 *  -------------------------------------------------  *
 *  | 160 | 164 | 168 | 172 | 176 | 180 | 184 | 188 |  *
 *  -------------------------------------------------  *
 *  |     CR    |     LR    |     PC    | back-chain|  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  48 |  49 |  50 |  51 |  52 |  53 |  54 |  55 |  *
 *  -------------------------------------------------  *
 *  | 192 | 196 | 200 | 204 | 208 | 212 | 216 | 220 |  *
 *  -------------------------------------------------  *
 *  |  cr saved |  lr saved |  compiler |   linker  |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  56 |  57 |  58 |  59 |  60 |  61 |  62 |  63 |  *
 *  -------------------------------------------------  *
 *  | 224 | 228 | 232 | 236 | 240 | 244 | 248 | 252 |  *
 *  -------------------------------------------------  *
 *  | TOC saved |    FCTX   |    DATA   |           |  *
 *  -------------------------------------------------  *
 *                                                     *
 *******************************************************/

.file "jump_ppc64_sysv_elf_gas.S"
.globl jump_fcontext
#if _CALL_ELF == 2
	.text
	.align 2
jump_fcontext:
        addis   %r2, %r12, .TOC.-jump_fcontext@ha
        addi    %r2, %r2, .TOC.-jump_fcontext@l
        .localentry jump_fcontext, . - jump_fcontext
#else
	.section ".opd","aw"
	.align 3
jump_fcontext:
# ifdef _CALL_LINUX
        .quad   .L.jump_fcontext,.TOC.@tocbase,0
        .type   jump_fcontext,@function
        .text
        .align 2
.L.jump_fcontext:
# else
        .hidden .jump_fcontext
        .globl  .jump_fcontext
        .quad   .jump_fcontext,.TOC.@tocbase,0
        .size   jump_fcontext,24
        .type   .jump_fcontext,@function
        .text
        .align 2
.jump_fcontext:
# endif
#endif
    # reserve space on stack
    subi  %r1, %r1, 184

#if _CALL_ELF != 2
    std  %r2,  0(%r1)  # save TOC
#endif
    std  %r14, 8(%r1)  # save R14
    std  %r15, 16(%r1)  # save R15
    std  %r16, 24(%r1)  # save R16
    std  %r17, 32(%r1)  # save R17
    std  %r18, 40(%r1)  # save R18
    std  %r19, 48(%r1)  # save R19
    std  %r20, 56(%r1)  # save R20
    std  %r21, 64(%r1)  # save R21
    std  %r22, 72(%r1)  # save R22
    std  %r23, 80(%r1)  # save R23
    std  %r24, 88(%r1)  # save R24
    std  %r25, 96(%r1)  # save R25
    std  %r26, 104(%r1)  # save R26
    std  %r27, 112(%r1)  # save R27
    std  %r28, 120(%r1)  # save R28
    std  %r29, 128(%r1)  # save R29
    std  %r30, 136(%r1)  # save R30
    std  %r31, 144(%r1)  # save R31
#if _CALL_ELF != 2
    std  %r3,  152(%r1)  # save hidden
#endif

    # save CR
    mfcr  %r0
    std   %r0, 160(%r1)
    # save LR
    mflr  %r0
    std   %r0, 168(%r1)
    # save LR as PC
    std   %r0, 176(%r1)

    # store RSP (pointing to context-data) in R6
    mr  %r6, %r1

#if _CALL_ELF == 2
    # restore RSP (pointing to context-data) from R3
    mr  %r1, %r3
#else
    # restore RSP (pointing to context-data) from R4
    mr  %r1, %r4

    ld  %r2,  0(%r1)  # restore TOC
#endif
    ld  %r14, 8(%r1)  # restore R14
    ld  %r15, 16(%r1)  # restore R15
    ld  %r16, 24(%r1)  # restore R16
    ld  %r17, 32(%r1)  # restore R17
    ld  %r18, 40(%r1)  # restore R18
    ld  %r19, 48(%r1)  # restore R19
    ld  %r20, 56(%r1)  # restore R20
    ld  %r21, 64(%r1)  # restore R21
    ld  %r22, 72(%r1)  # restore R22
    ld  %r23, 80(%r1)  # restore R23
    ld  %r24, 88(%r1)  # restore R24
    ld  %r25, 96(%r1)  # restore R25
    ld  %r26, 104(%r1)  # restore R26
    ld  %r27, 112(%r1)  # restore R27
    ld  %r28, 120(%r1)  # restore R28
    ld  %r29, 128(%r1)  # restore R29
    ld  %r30, 136(%r1)  # restore R30
    ld  %r31, 144(%r1)  # restore R31
#if _CALL_ELF != 2
    ld  %r3,  152(%r1)  # restore hidden
#endif

    # restore CR
    ld  %r0, 160(%r1)
    mtcr  %r0
    # restore LR
    ld  %r0, 168(%r1)
    mtlr  %r0

    # load PC
    ld  %r12, 176(%r1)
    # restore CTR
    mtctr  %r12

    # adjust stack
    addi  %r1, %r1, 184

#if _CALL_ELF == 2
    # copy transfer_t into transfer_fn arg registers
    mr  %r3, %r6
    # arg pointer already in %r4

    # jump to context
    bctr
	.size jump_fcontext, .-jump_fcontext
#else
    # zero in r3 indicates first jump to context-function
    cmpdi %r3, 0
    beq use_entry_arg

    # return transfer_t
    std  %r6, 0(%r3)
    std  %r5, 8(%r3)

    # jump to context
    bctr

use_entry_arg:
    # copy transfer_t into transfer_fn arg registers
    mr  %r3, %r6
    mr  %r4, %r5

    # jump to context
    bctr
# ifdef _CALL_LINUX
	.size .jump_fcontext, .-.L.jump_fcontext
# else
	.size .jump_fcontext, .-.jump_fcontext
# endif
#endif


/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
