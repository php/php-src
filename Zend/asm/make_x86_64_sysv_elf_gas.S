/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

/****************************************************************************************
 *                                                                                      *
 *  ----------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6    |    7    |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18  |   0x1c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  | fc_mxcsr|fc_x87_cw|       guard       |         R12        |        R13        |  *
 *  ----------------------------------------------------------------------------------  *
 *  ----------------------------------------------------------------------------------  *
 *  |    8    |    9    |   10    |   11    |    12    |    13   |    14   |    15   |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x20  |   0x24  |   0x28  |  0x2c   |   0x30   |   0x34  |   0x38  |   0x3c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        R14        |        R15        |         RBX        |        RBP        |  *
 *  ----------------------------------------------------------------------------------  *
 *  ----------------------------------------------------------------------------------  *
 *  |   16    |   17    |   18    |   19    |    20    |    21   |    22   |    23   |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x40  |   0x44  |                                                            |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        RIP        |                                                            |  *
 *  ----------------------------------------------------------------------------------  *
 *                                                                                      *
 ****************************************************************************************/

# ifdef __i386__
#  include "make_i386_sysv_elf_gas.S"
# else
#  if defined __CET__
#   include <cet.h>
#   define SHSTK_ENABLED (__CET__ & 0x2)
#   define BOOST_CONTEXT_SHADOW_STACK (SHSTK_ENABLED && SHADOW_STACK_SYSCALL)
#  else
#   define _CET_ENDBR
#  endif
.file "make_x86_64_sysv_elf_gas.S"
.text
.globl make_fcontext
.type make_fcontext,@function
.align 16
make_fcontext:
    _CET_ENDBR

#if BOOST_CONTEXT_SHADOW_STACK
    /* the new shadow stack pointer (SSP) */
    movq  -0x8(%rdi), %r9
#endif

    /* first arg of make_fcontext() == top of context-stack */
    movq  %rdi, %rax

    /* shift address in RAX to lower 16 byte boundary */
    andq  $-16, %rax

    /* reserve space for context-data on context-stack */
    /* on context-function entry: (RSP -0x8) % 16 == 0 */
    leaq  -0x48(%rax), %rax

    /* third arg of make_fcontext() == address of context-function */
    /* stored in RBX */
    movq  %rdx, 0x30(%rax)

    /* save MMX control- and status-word */
    stmxcsr  (%rax)
    /* save x87 control-word */
    fnstcw   0x4(%rax)

#if defined(BOOST_CONTEXT_TLS_STACK_PROTECTOR)
    /* save stack guard */
    movq  %fs:0x28, %rcx    /* read stack guard from TLS record */
    movq  %rcx, 0x8(%rsp)   /* save stack guard */
#endif

    /* compute abs address of label trampoline */
    leaq  trampoline(%rip), %rcx
    /* save address of trampoline as return-address for context-function */
    /* will be entered after calling jump_fcontext() first time */
    movq  %rcx, 0x40(%rax)

    /* compute abs address of label finish */
    leaq  finish(%rip), %rcx
    /* save address of finish as return-address for context-function */
    /* will be entered after context-function returns */
    movq  %rcx, 0x38(%rax)

#if BOOST_CONTEXT_SHADOW_STACK
    /* Populate the shadow stack and normal stack */
    /* get original SSP */
    rdsspq  %r8
    /* restore new shadow stack */
    rstorssp  -0x8(%r9)
    /* save the restore token on the original shadow stack */
    saveprevssp
    /* push the address of "jmp trampoline" to the new shadow stack */
    /* as well as the stack */
    call  1f
    jmp  trampoline
1:
    /* save address of "jmp trampoline" as return-address */
    /* for context-function */
    pop 0x38(%rax)
    /* Get the new SSP.  */
    rdsspq  %r9
    /* restore original shadow stack */
    rstorssp  -0x8(%r8)
    /* save the restore token on the new shadow stack.  */
    saveprevssp

    /* reserve space for the new SSP */
    leaq  -0x8(%rax), %rax
    /* save the new SSP to this fcontext */
    movq  %r9, (%rax)
#endif

#if BOOST_CONTEXT_SHADOW_STACK
    /* Populate the shadow stack */

    /* get original SSP */
    rdsspq  %r8
    /* restore new shadow stack */
    rstorssp  -0x8(%r9)
    /* save the restore token on the original shadow stack */
    saveprevssp
    /* push the address of "jmp trampoline" to the new shadow stack */
    /* as well as the stack */
    call  1f
    jmp  trampoline
1:
    /* save address of "jmp trampoline" as return-address */
    /* for context-function */
    pop 0x38(%rax)
    /* Get the new SSP.  */
    rdsspq  %r9
    /* restore original shadow stack */
    rstorssp  -0x8(%r8)
    /* save the restore token on the new shadow stack.  */
    saveprevssp

    /* now the new shadow stack looks like:
        base->  +------------------------------+
                | address of "jmp  trampoline" |
        SSP->   +------------------------------+
                |         restore token        |
                +------------------------------+
    */

    /* reserve space for the new SSP */
    leaq  -0x8(%rax), %rax
    /* save the new SSP to this fcontext */
    movq  %r9, (%rax)
#endif

    ret /* return pointer to context-data */

trampoline:
    /* store return address on stack */
    /* fix stack alignment */
    _CET_ENDBR
#if BOOST_CONTEXT_SHADOW_STACK
    /* save address of "jmp *%rbp" as return-address */
    /* on stack and shadow stack */
    call  2f
    jmp  *%rbp
2:
#else
    push %rbp
#endif
    /* jump to context-function */
    jmp *%rbx

finish:
    _CET_ENDBR
    /* exit code is zero */
    xorq  %rdi, %rdi
    /* exit application */
    call  _exit@PLT
    hlt
.size make_fcontext,.-make_fcontext

/* Mark that we don't need executable stack. */
.section .note.GNU-stack,"",%progbits
# endif
