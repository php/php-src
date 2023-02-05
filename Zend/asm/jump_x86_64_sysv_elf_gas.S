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

# if defined __CET__
#  include <cet.h>
#  define SHSTK_ENABLED (__CET__ & 0x2)
#  define BOOST_CONTEXT_SHADOW_STACK (SHSTK_ENABLED && SHADOW_STACK_SYSCALL)
# else
#  define _CET_ENDBR
# endif
.file "jump_x86_64_sysv_elf_gas.S"
.text
.globl jump_fcontext
.type jump_fcontext,@function
.align 16
jump_fcontext:
    _CET_ENDBR
    leaq  -0x40(%rsp), %rsp /* prepare stack */

#if !defined(BOOST_USE_TSX)
    stmxcsr  (%rsp)     /* save MMX control- and status-word */
    fnstcw   0x4(%rsp)  /* save x87 control-word */
#endif

#if defined(BOOST_CONTEXT_TLS_STACK_PROTECTOR)
    movq  %fs:0x28, %rcx    /* read stack guard from TLS record */
    movq  %rcx, 0x8(%rsp)   /* save stack guard */
#endif

    movq  %r12, 0x10(%rsp)  /* save R12 */
    movq  %r13, 0x18(%rsp)  /* save R13 */
    movq  %r14, 0x20(%rsp)  /* save R14 */
    movq  %r15, 0x28(%rsp)  /* save R15 */
    movq  %rbx, 0x30(%rsp)  /* save RBX */
    movq  %rbp, 0x38(%rsp)  /* save RBP */

#if BOOST_CONTEXT_SHADOW_STACK
    /* grow the stack to reserve space for shadow stack pointer(SSP) */
    leaq  -0x8(%rsp), %rsp
    /* read the current SSP and store it */
    rdsspq  %rcx
    movq  %rcx, (%rsp)
#endif

    /* store RSP (pointing to context-data) in RAX */
    movq  %rsp, %rax

    /* restore RSP (pointing to context-data) from RDI */
    movq  %rdi, %rsp

#if BOOST_CONTEXT_SHADOW_STACK
    /* first 8 bytes are SSP */
    movq  (%rsp), %rcx
    leaq  0x8(%rsp), %rsp

    /* Restore target(new) shadow stack */
    rstorssp  -8(%rcx)
    /* restore token for previous shadow stack is pushed */
    /* on previous shadow stack after saveprevssp */
    saveprevssp

    /* when return, jump_fcontext jump to restored return address */
    /* (r8) instead of RET. This miss of RET implies us to unwind */
    /* shadow stack accordingly. Otherwise mismatch occur */
    movq  $1, %rcx
    incsspq  %rcx
#endif

    movq  0x40(%rsp), %r8  /* restore return-address */

#if !defined(BOOST_USE_TSX)
    ldmxcsr  (%rsp)     /* restore MMX control- and status-word */
    fldcw    0x4(%rsp)  /* restore x87 control-word */
#endif

#if defined(BOOST_CONTEXT_TLS_STACK_PROTECTOR)
    movq  0x8(%rsp), %rdx  /* load stack guard */
    movq  %rdx, %fs:0x28   /* restore stack guard to TLS record */
#endif

    movq  0x10(%rsp), %r12  /* restore R12 */
    movq  0x18(%rsp), %r13  /* restore R13 */
    movq  0x20(%rsp), %r14  /* restore R14 */
    movq  0x28(%rsp), %r15  /* restore R15 */
    movq  0x30(%rsp), %rbx  /* restore RBX */
    movq  0x38(%rsp), %rbp  /* restore RBP */

    leaq  0x48(%rsp), %rsp /* prepare stack */

    /* return transfer_t from jump */
#if !defined(_ILP32)
    /* RAX == fctx, RDX == data */
    movq  %rsi, %rdx
#else
    /* RAX == data:fctx */
    salq  $32, %rsi
    orq   %rsi, %rax
#endif
    /* pass transfer_t as first arg in context function */
#if !defined(_ILP32)
    /* RDI == fctx, RSI == data */
#else
    /* RDI == data:fctx */
#endif
    movq  %rax, %rdi

    /* indirect jump to context */
    jmp  *%r8
.size jump_fcontext,.-jump_fcontext

/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
