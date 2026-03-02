/*
 * IR - Lightweight JIT Compilation Framework
 * (x86/x86_64 CPU specific definitions)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_X86_H
#define IR_X86_H

#if defined(IR_TARGET_X64)
# define IR_GP_REGS(_) \
	_(R0,   rax,   eax,   ax,   al, ah) \
	_(R1,   rcx,   ecx,   cx,   cl, ch) \
	_(R2,   rdx,   edx,   dx,   dl, dh) \
	_(R3,   rbx,   ebx,   bx,   bl, bh) \
	_(R4,   rsp,   esp,   __,   __, __) \
	_(R5,   rbp,   ebp,   bp,  r5b, __) \
	_(R6,   rsi,   esi,   si,  r6b, __) \
	_(R7,   rdi,   edi,   di,  r7b, __) \
	_(R8,   r8,    r8d,  r8w,  r8b, __) \
	_(R9,   r9,    r9d,  r9w,  r9b, __) \
	_(R10,  r10,  r10d, r10w, r10b, __) \
	_(R11,  r11,  r11d, r11w, r11b, __) \
	_(R12,  r12,  r12d, r12w, r12b, __) \
	_(R13,  r13,  r13d, r13w, r13b, __) \
	_(R14,  r14,  r14d, r14w, r14b, __) \
	_(R15,  r15,  r15d, r15w, r15b, __) \

# define IR_FP_REGS(_) \
	_(XMM0,  xmm0) \
	_(XMM1,  xmm1) \
	_(XMM2,  xmm2) \
	_(XMM3,  xmm3) \
	_(XMM4,  xmm4) \
	_(XMM5,  xmm5) \
	_(XMM6,  xmm6) \
	_(XMM7,  xmm7) \
	_(XMM8,  xmm8) \
	_(XMM9,  xmm9) \
	_(XMM10, xmm10) \
	_(XMM11, xmm11) \
	_(XMM12, xmm12) \
	_(XMM13, xmm13) \
	_(XMM14, xmm14) \
	_(XMM15, xmm15) \

#elif defined(IR_TARGET_X86)

# define IR_GP_REGS(_) \
	_(R0,   ___,   eax,   ax,   al, ah) \
	_(R1,   ___,   ecx,   cx,   cl, ch) \
	_(R2,   ___,   edx,   dx,   dl, dh) \
	_(R3,   ___,   ebx,   bx,   bl, bh) \
	_(R4,   ___,   esp,   __,   __, __) \
	_(R5,   ___,   ebp,   bp,   __, __) \
	_(R6,   ___,   esi,   si,   __, __) \
	_(R7,   ___,   edi,   di,   __, __) \

# define IR_FP_REGS(_) \
	_(XMM0,  xmm0) \
	_(XMM1,  xmm1) \
	_(XMM2,  xmm2) \
	_(XMM3,  xmm3) \
	_(XMM4,  xmm4) \
	_(XMM5,  xmm5) \
	_(XMM6,  xmm6) \
	_(XMM7,  xmm7) \

#else
# error "Unsupported target architecture"
#endif

#define IR_GP_REG_ENUM(code, name64, name32, name16, name8, name8h) \
	IR_REG_ ## code,

#define IR_FP_REG_ENUM(code, name) \
	IR_REG_ ## code,

enum _ir_reg {
	_IR_REG_NONE = -1,
	IR_GP_REGS(IR_GP_REG_ENUM)
	IR_FP_REGS(IR_FP_REG_ENUM)
	IR_REG_NUM,
	IR_REG_ALL = IR_REG_NUM, /* special name for regset */
	IR_REG_SET_1,            /* special name for regset */
	IR_REG_SET_2,            /* special name for regset */
	IR_REG_SET_3,            /* special name for regset */
	IR_REG_SET_NUM,
};

#define IR_REG_GP_FIRST IR_REG_R0
#define IR_REG_FP_FIRST IR_REG_XMM0
#define IR_REG_GP_LAST  (IR_REG_FP_FIRST - 1)
#define IR_REG_FP_LAST  (IR_REG_NUM - 1)

#define IR_REGSET_64BIT 0

#define IR_REG_STACK_POINTER \
	IR_REG_RSP
#define IR_REG_FRAME_POINTER \
	IR_REG_RBP
#define IR_REGSET_FIXED \
	(IR_REGSET(IR_REG_RSP))
#define IR_REGSET_GP \
	IR_REGSET_DIFFERENCE(IR_REGSET_INTERVAL(IR_REG_GP_FIRST, IR_REG_GP_LAST), IR_REGSET_FIXED)
#define IR_REGSET_FP \
	IR_REGSET_DIFFERENCE(IR_REGSET_INTERVAL(IR_REG_FP_FIRST, IR_REG_FP_LAST), IR_REGSET_FIXED)

#define IR_REG_RAX IR_REG_R0
#define IR_REG_RCX IR_REG_R1
#define IR_REG_RDX IR_REG_R2
#define IR_REG_RBX IR_REG_R3
#define IR_REG_RSP IR_REG_R4
#define IR_REG_RBP IR_REG_R5
#define IR_REG_RSI IR_REG_R6
#define IR_REG_RDI IR_REG_R7

#endif /* IR_X86_H */
