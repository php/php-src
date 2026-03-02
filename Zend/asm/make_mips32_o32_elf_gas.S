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

.file "make_mips32_o32_elf_gas.S"
.text
.globl make_fcontext
.align 2
.type make_fcontext,@function
.ent make_fcontext
make_fcontext:
#ifdef __PIC__
.set    noreorder
.cpload $t9
.set    reorder
#endif
    # shift address in A0 to lower 16 byte boundary
    li $v1, -16 # 0xfffffffffffffff0
    and $v0, $v1, $a0

    # reserve space for context-data on context-stack
    #  includes an extra 32 bytes for:
    #  - 16-byte incoming argument area required by mips ABI used when
    #    jump_context calls the initial function
    #  - 4 bytes to save our GP register used in finish
    #  - 8 bytes to as space for transfer_t returned to finish
    #  - 4 bytes for alignment
    addiu $v0, $v0, -128

    # third arg of make_fcontext() == address of context-function
    sw  $a2, 92($v0)
    # save global pointer in context-data
    sw  $gp, 112($v0)

    # compute address of returned transfer_t
    addiu $t0, $v0, 116
    sw  $t0, 84($v0)

    # compute abs address of label finish
    la  $t9, finish
    # save address of finish as return-address for context-function
    # will be entered after context-function returns
    sw  $t9, 88($v0)

    jr  $ra # return pointer to context-data

finish:
    # reload our gp register (needed for la)
    lw $gp, 16($sp)

    # call _exit(0)
    #  the previous function should have left the 16 bytes incoming argument
    #  area on the stack which we reuse for calling _exit
    la $t9, _exit
    move $a0, $zero
    jr $t9
.end make_fcontext
.size make_fcontext, .-make_fcontext

/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
