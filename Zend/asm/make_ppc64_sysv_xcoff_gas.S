/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/
	.globl	make_fcontext[DS]
	.globl .make_fcontext[PR]
	.align 2 
	.csect .make_fcontext[PR], 3
	.globl _make_fcontext
#._make_fcontext:
    # save return address into R6
    mflr  6

    # first arg of make_fcontext() == top address of context-function
    # shift address in R3 to lower 16 byte boundary
    clrrdi  3, 3, 4

    # reserve space for context-data on context-stack
    # including 64 byte of linkage + parameter area (R1 % 16 == 0)
    subi  3, 3, 248

    # third arg of make_fcontext() == address of context-function
    # (upstream boost ignored the fact this is a function descriptor)
    ld   4, 0(5)
    std  4, 176(3)
    # save TOC of context-function
    ld   4, 8(5)
    std  4, 0(3)

    # set back-chain to zero
    li   0, 0
    std  0, 184(3)

    # zero in r3 indicates first jump to context-function
    std  0, 152(3)

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
    std  4, 168(3)

    # restore return address from R6
    mtlr  6

    blr  # return pointer to context-data

.L_finish:
    # save return address into R0
    mflr  0
    # save return address on stack, set up stack frame
    std  0, 8(1)
    # allocate stack space, R1 % 16 == 0
    stdu  1, -32(1)

    # exit code is zero
    li  3, 0
    # exit application
    bl  ._exit
    nop
