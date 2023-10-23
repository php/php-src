/*
 * IR - Lightweight JIT Compilation Framework
 * (Aarch64 CPU specific definitions)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_AARCH64_H
#define IR_AARCH64_H

#define IR_GP_REGS(_) \
	_(X0,    x0,  w0) \
	_(X1,    x1,  w1) \
	_(X2,    x2,  w2) \
	_(X3,    x3,  w3) \
	_(X4,    x4,  w4) \
	_(X5,    x5,  w5) \
	_(X6,    x6,  w6) \
	_(X7,    x7,  w7) \
	_(X8,    x8,  w8) \
	_(X9,    x9,  w9) \
	_(X10,  x10,  w10) \
	_(X11,  x11,  w11) \
	_(X12,  x12,  w12) \
	_(X13,  x13,  w13) \
	_(X14,  x14,  w14) \
	_(X15,  x15,  w15) \
	_(X16,  x16,  w16) \
	_(X17,  x17,  w17) \
	_(X18,  x18,  w18) \
	_(X19,  x19,  w18) \
	_(X20,  x20,  w20) \
	_(X21,  x21,  w21) \
	_(X22,  x22,  w22) \
	_(X23,  x23,  w23) \
	_(X24,  x24,  w24) \
	_(X25,  x25,  w25) \
	_(X26,  x26,  w26) \
	_(X27,  x27,  w27) \
	_(X28,  x28,  w28) \
	_(X29,  x29,  w29) \
	_(X30,  x30,  w30) \
	_(X31,  x31,  w31) \

# define IR_FP_REGS(_) \
	_(V0,   d0,   s0,   h0,   b0) \
	_(V1,   d1,   s1,   h1,   b1) \
	_(V2,   d2,   s2,   h2,   b2) \
	_(V3,   d3,   s3,   h3,   b3) \
	_(V4,   d4,   s4,   h4,   b4) \
	_(V5,   d5,   s5,   h5,   b5) \
	_(V6,   d6,   s6,   h6,   b6) \
	_(V7,   d7,   s7,   h7,   b7) \
	_(V8,   d8,   s8,   h8,   b8) \
	_(V9,   d9,   s9,   h9,   b9) \
	_(V10,  d10,  s10,  h10,  b10) \
	_(V11,  d11,  s11,  h11,  b11) \
	_(V12,  d12,  s12,  h12,  b12) \
	_(V13,  d13,  s13,  h13,  b13) \
	_(V14,  d14,  s14,  h14,  b14) \
	_(V15,  d15,  s15,  h15,  b15) \
	_(V16,  d16,  s16,  h16,  b16) \
	_(V17,  d17,  s17,  h17,  b17) \
	_(V18,  d18,  s18,  h18,  b18) \
	_(V19,  d19,  s19,  h19,  b18) \
	_(V20,  d20,  s20,  h20,  b20) \
	_(V21,  d21,  s21,  h21,  b21) \
	_(V22,  d22,  s22,  h22,  b22) \
	_(V23,  d23,  s23,  h23,  b23) \
	_(V24,  d24,  s24,  h24,  b24) \
	_(V25,  d25,  s25,  h25,  b25) \
	_(V26,  d26,  s26,  h26,  b26) \
	_(V27,  d27,  s27,  h27,  b27) \
	_(V28,  d28,  s28,  h28,  b28) \
	_(V29,  d29,  s29,  h29,  b29) \
	_(V30,  d30,  s30,  h30,  b30) \
	_(V31,  d31,  s31,  h31,  b31) \

#define IR_GP_REG_ENUM(code, name64, name32) \
	IR_REG_ ## code,

#define IR_FP_REG_ENUM(code, name64, name32, name16, name8) \
	IR_REG_ ## code,

enum _ir_reg {
	_IR_REG_NONE = -1,
	IR_GP_REGS(IR_GP_REG_ENUM)
	IR_FP_REGS(IR_FP_REG_ENUM)
	IR_REG_NUM,
};

#define IR_REG_GP_FIRST IR_REG_X0
#define IR_REG_FP_FIRST IR_REG_V0
#define IR_REG_GP_LAST  (IR_REG_FP_FIRST - 1)
#define IR_REG_FP_LAST  (IR_REG_NUM - 1)
#define IR_REG_SCRATCH  (IR_REG_NUM)        /* special name for regset */
#define IR_REG_ALL      (IR_REG_NUM + 1)    /* special name for regset */

#define IR_REGSET_64BIT 1

#define IR_REG_INT_TMP  IR_REG_X17 /* reserved temporary register used by code-generator */

#define IR_REG_STACK_POINTER \
	IR_REG_X31
#define IR_REG_FRAME_POINTER \
	IR_REG_X29
#define IR_REGSET_FIXED \
	( IR_REGSET(IR_REG_INT_TMP) \
	| IR_REGSET(IR_REG_X18) /* platform specific register */ \
	| IR_REGSET_INTERVAL(IR_REG_X29, IR_REG_X31))
#define IR_REGSET_GP \
	IR_REGSET_DIFFERENCE(IR_REGSET_INTERVAL(IR_REG_GP_FIRST, IR_REG_GP_LAST), IR_REGSET_FIXED)
#define IR_REGSET_FP \
	IR_REGSET_DIFFERENCE(IR_REGSET_INTERVAL(IR_REG_FP_FIRST, IR_REG_FP_LAST), IR_REGSET_FIXED)

#define IR_REG_IP0 IR_REG_X16
#define IR_REG_IP1 IR_REG_X17
#define IR_REG_PR  IR_REG_X18
#define IR_REG_LR  IR_REG_X30
#define IR_REG_ZR  IR_REG_X31

/* Calling Convention */
#define IR_REG_INT_RET1 IR_REG_X0
#define IR_REG_FP_RET1  IR_REG_V0
#define IR_REG_INT_ARGS 8
#define IR_REG_FP_ARGS  8
#define IR_REG_INT_ARG1 IR_REG_X0
#define IR_REG_INT_ARG2 IR_REG_X1
#define IR_REG_INT_ARG3 IR_REG_X2
#define IR_REG_INT_ARG4 IR_REG_X3
#define IR_REG_INT_ARG5 IR_REG_X4
#define IR_REG_INT_ARG6 IR_REG_X5
#define IR_REG_INT_ARG7 IR_REG_X6
#define IR_REG_INT_ARG8 IR_REG_X7
#define IR_REG_FP_ARG1  IR_REG_V0
#define IR_REG_FP_ARG2  IR_REG_V1
#define IR_REG_FP_ARG3  IR_REG_V2
#define IR_REG_FP_ARG4  IR_REG_V3
#define IR_REG_FP_ARG5  IR_REG_V4
#define IR_REG_FP_ARG6  IR_REG_V5
#define IR_REG_FP_ARG7  IR_REG_V6
#define IR_REG_FP_ARG8  IR_REG_V7
#define IR_MAX_REG_ARGS 16
#define IR_SHADOW_ARGS  0

# define IR_REGSET_SCRATCH \
	(IR_REGSET_INTERVAL(IR_REG_X0, IR_REG_X18) \
	| IR_REGSET_INTERVAL(IR_REG_V0, IR_REG_V7) \
	| IR_REGSET_INTERVAL(IR_REG_V16, IR_REG_V31))

# define IR_REGSET_PRESERVED \
	(IR_REGSET_INTERVAL(IR_REG_X19, IR_REG_X30) \
	| IR_REGSET_INTERVAL(IR_REG_V8, IR_REG_V15))

typedef struct _ir_tmp_reg {
	union {
		uint8_t num;
		int8_t  reg;
	};
	uint8_t     type;
	uint8_t     start;
	uint8_t     end;
} ir_tmp_reg;

struct _ir_target_constraints {
	int8_t      def_reg;
	uint8_t     tmps_count;
	uint8_t     hints_count;
	ir_tmp_reg  tmp_regs[3];
	int8_t      hints[IR_MAX_REG_ARGS + 3];
};

#endif /* IR_AARCH64_H */
