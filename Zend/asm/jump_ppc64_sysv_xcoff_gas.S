
/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

.align 2
.globl .jump_fcontext
.jump_fcontext:
    # reserve space on stack
    subi  1, 1, 184

    std  13, 0(1)  # save R13
    std  14, 8(1)  # save R14
    std  15, 16(1)  # save R15
    std  16, 24(1)  # save R16
    std  17, 32(1)  # save R17
    std  18, 40(1)  # save R18
    std  19, 48(1)  # save R19
    std  20, 56(1)  # save R20
    std  21, 64(1)  # save R21
    std  22, 72(1)  # save R22
    std  23, 80(1)  # save R23
    std  24, 88(1)  # save R24
    std  25, 96(1)  # save R25
    std  26, 104(1)  # save R26
    std  27, 112(1)  # save R27
    std  29, 120(1)  # save R28
    std  29, 128(1)  # save R29
    std  30, 136(1)  # save R30
    std  31, 144(1)  # save R31
    std  3,  152(1)  # save hidden

    # save CR
    mfcr  0
    std  0, 160(1)
    # save LR
    mflr  0
    std  0, 168(1)
    # save LR as PC
    std  0, 176(1)

    # store RSP (pointing to context-data) in R6
    mr  6, 1

    # restore RSP (pointing to context-data) from R4
    mr  1, 4

    ld  13, 0(1)  # restore R13
    ld  14, 8(1)  # restore R14
    ld  15, 16(1)  # restore R15
    ld  16, 24(1)  # restore R16
    ld  17, 32(1)  # restore R17
    ld  18, 40(1)  # restore R18
    ld  19, 48(1)  # restore R19
    ld  20, 56(1)  # restore R20
    ld  21, 64(1)  # restore R21
    ld  22, 72(1)  # restore R22
    ld  23, 80(1)  # restore R23
    ld  24, 88(1)  # restore R24
    ld  25, 96(1)  # restore R25
    ld  26, 104(1)  # restore R26
    ld  27, 112(1)  # restore R27
    ld  28, 120(1)  # restore R28
    ld  29, 128(1)  # restore R29
    ld  30, 136(1)  # restore R30
    ld  31, 144(1)  # restore R31
    ld  3,  152(1)  # restore hidden

    # restore CR
    ld  0, 160(1)
    mtcr  0
    # restore LR
    ld  0, 168(1)
    mtlr  0

    # load PC
    ld  0, 176(1)
    # restore CTR
    mtctr  0

    # adjust stack
    addi  1, 1, 184

    # return transfer_t
    std  6, 0(3)
    std  5, 8(3)

    # jump to context
    bctr
