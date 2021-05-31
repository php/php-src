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
 *  |bchai|hiddn|   fpscr   |  PC |  CR | R14 | R15 |  *
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
 *  ------------------------|------------              *
 *  | 224 | 228 | 232 | 236 | 240 | 244 |              *
 *  ------------------------|------------              *
 *  |    F30    |    F31    |bchai|  LR |              *
 *  ------------------------|------------              *
 *                                                     *
 *******************************************************/

.file "make_ppc32_sysv_elf_gas.S"
.text
.globl make_fcontext
.align 2
.type make_fcontext,@function
make_fcontext:
    # save return address into R6
    mflr  %r6

    # first arg of make_fcontext() == top address of context-function
    # shift address in R3 to lower 16 byte boundary
    clrrwi  %r3, %r3, 4

    # reserve space on context-stack, including 16 bytes of linkage
    # and parameter area + 240 bytes of context-data (R1 % 16 == 0)
    subi  %r3, %r3, 16 + 240

    # third arg of make_fcontext() == address of context-function
#ifdef __linux__
    # save context-function as PC
    stw  %r5, 16(%r3)
#else
    # save context-function for trampoline
    stw  %r5, 248(%r3)
#endif

    # set back-chain to zero
    li   %r0, 0
    stw  %r0, 240(%r3)

    # copy FPSCR to new context
    mffs  %f0
    stfd  %f0, 8(%r3)

#ifdef __linux__
    # set hidden pointer for returning transfer_t
    la    %r0, 248(%r3)
    stw   %r0, 4(%r3)
#endif

    # load address of label 1 into R4
    bl  1f
1:  mflr  %r4
#ifndef __linux__
    # compute abs address of trampoline, use as PC
    addi  %r7, %r4, trampoline - 1b
    stw   %r7, 16(%r3)
#endif
    # compute abs address of label finish
    addi  %r4, %r4, finish - 1b
    # save address of finish as return-address for context-function
    # will be entered after context-function returns
    stw  %r4, 244(%r3)

    # restore return address from R6
    mtlr  %r6

    blr  # return pointer to context-data

#ifndef __linux__
trampoline:
    # On systems other than Linux, jump_fcontext is returning the
    # transfer_t in R3:R4, but we need to pass transfer_t * R3 to
    # our context-function.
    lwz   %r0, 8(%r1)   # address of context-function
    mtctr %r0
    stw   %r3, 8(%r1)
    stw   %r4, 12(%r1)
    la    %r3, 8(%r1)   # address of transfer_t
    bctr
#endif

finish:
    # Use the secure PLT for _exit(0).  If we use the insecure BSS PLT
    # here, then the linker may use the insecure BSS PLT even if the
    # C++ compiler wanted the secure PLT.

    # set R30 for secure PLT, large model
    bl     2f
2:  mflr   %r30
    addis  %r30, %r30, .Ltoc - 2b@ha
    addi   %r30, %r30, .Ltoc - 2b@l

    # call _exit(0) with special addend 0x8000 for large model
    li  %r3, 0
    bl  _exit + 0x8000@plt
.size make_fcontext, .-make_fcontext

/* Provide the GOT pointer for secure PLT, large model. */
.section .got2,"aw"
.Ltoc = . + 0x8000

/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
