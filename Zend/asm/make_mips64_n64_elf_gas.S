/*
            Copyright Jiaxun Yang 2018.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************
 *                                                     *
 *  -------------------------------------------------  *
 *  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  *
 *  -------------------------------------------------  *
 *  |     0     |     8     |    16     |     24    |  *
 *  -------------------------------------------------  *
 *  |    F24    |    F25    |    F26    |    F27    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
 *  -------------------------------------------------  *
 *  |     32    |    40     |     48    |     56    |  *
 *  -------------------------------------------------  *
 *  |    F28    |    F29    |    F30    |    F31    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
 *  -------------------------------------------------  *
 *  |     64    |    72     |     80    |     88    |  *
 *  -------------------------------------------------  *
 *  |    S0     |    S1     |     S2    |     S3    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  *
 *  -------------------------------------------------  *
 *  |  96 | 100 | 104 | 108 | 112 | 116 | 120 | 124 |  *
 *  -------------------------------------------------  *
 *  |    S4     |    S5     |     S6    |     S7    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  32 |  33 |  34 |  35 |  36 |  37 |  38 |  39 |  *
 *  -------------------------------------------------  *
 *  | 128 | 132 | 136 | 140 | 144 | 148 | 152 | 156 |  *
 *  -------------------------------------------------  *
 *  |    FP     |    GP     |     RA    |     PC    |  *
 *  -------------------------------------------------  *
 *                                                     *
 * *****************************************************/

.file "make_mips64_n64_elf_gas.S"
.text
.globl make_fcontext
.align 3
.type make_fcontext,@function
.ent make_fcontext
make_fcontext:
#ifdef __PIC__
.set    noreorder
.cpload $t9
.set    reorder
#endif
    # shift address in A0 to lower 16 byte boundary
    li $v1, 0xfffffffffffffff0
    and $v0, $v1, $a0

    # reserve space for context-data on context-stack
    daddiu $v0, $v0, -160

    # third arg of make_fcontext() == address of context-function
    sd  $a2, 152($v0)
    # save global pointer in context-data
    sd  $gp, 136($v0)

    # psudo instruction compute abs address of label finish based on GP
    dla  $t9, finish

    # save address of finish as return-address for context-function
    # will be entered after context-function returns
    sd  $t9, 144($v0)

    jr  $ra # return pointer to context-data

finish:
    # reload our gp register (needed for la)
    daddiu $t0, $sp, -160
    ld $gp, 136($t0)

    # call _exit(0)
    #  the previous function should have left the 16 bytes incoming argument
    #  area on the stack which we reuse for calling _exit
    dla $t9, _exit
    move $a0, $zero
    jr $t9
.end make_fcontext
.size make_fcontext, .-make_fcontext

/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
