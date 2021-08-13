/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef HAVE_JIT_X86_H
#define HAVE_JIT_X86_H

typedef enum _zend_reg {
	ZREG_NONE = -1,

	ZREG_R0,
	ZREG_R1,
	ZREG_R2,
	ZREG_R3,
	ZREG_R4,
	ZREG_R5,
	ZREG_R6,
	ZREG_R7,

#if defined(__x86_64__) || defined(_WIN64)
	ZREG_R8,
	ZREG_R9,
	ZREG_R10,
	ZREG_R11,
	ZREG_R12,
	ZREG_R13,
	ZREG_R14,
	ZREG_R15,
#endif

	ZREG_XMM0,
	ZREG_XMM1,
	ZREG_XMM2,
	ZREG_XMM3,
	ZREG_XMM4,
	ZREG_XMM5,
	ZREG_XMM6,
	ZREG_XMM7,

#if defined(__x86_64__) || defined(_WIN64)
	ZREG_XMM8,
	ZREG_XMM9,
	ZREG_XMM10,
	ZREG_XMM11,
	ZREG_XMM12,
	ZREG_XMM13,
	ZREG_XMM14,
	ZREG_XMM15,
#endif

	ZREG_NUM,

	ZREG_THIS, /* used for delayed FETCH_THIS deoptimization */

	/* pseudo constants used by deoptimizer */
	ZREG_LONG_MIN_MINUS_1,
	ZREG_LONG_MIN,
	ZREG_LONG_MAX,
	ZREG_LONG_MAX_PLUS_1,
	ZREG_NULL,

	ZREG_ZVAL_TRY_ADDREF,
	ZREG_ZVAL_COPY_GPR0,
} zend_reg;

typedef struct _zend_jit_registers_buf {
#if defined(__x86_64__) || defined(_WIN64)
	uint64_t gpr[16]; /* general purpose integer register */
	double   fpr[16]; /* floating point registers */
#else
	uint32_t gpr[8]; /* general purpose integer register */
	double   fpr[8]; /* floating point registers */
#endif
} zend_jit_registers_buf;

#define ZREG_FIRST_FPR ZREG_XMM0
#define ZREG_COPY      ZREG_R0

#define ZREG_RAX ZREG_R0
#define ZREG_RCX ZREG_R1
#define ZREG_RDX ZREG_R2
#define ZREG_RBX ZREG_R3
#define ZREG_RSP ZREG_R4
#define ZREG_RBP ZREG_R5
#define ZREG_RSI ZREG_R6
#define ZREG_RDI ZREG_R7

#ifdef _WIN64
# define ZREG_FP ZREG_R14
# define ZREG_IP ZREG_R15
#elif defined(__x86_64__)
# define ZREG_FP ZREG_R14
# define ZREG_IP ZREG_R15
#else
# define ZREG_FP ZREG_RSI
# define ZREG_IP ZREG_RDI
#endif

#define ZREG_RX  ZREG_IP

typedef uint32_t zend_regset;

#define ZEND_REGSET_64BIT 0

#ifdef _WIN64
# define ZEND_REGSET_FIXED \
	(ZEND_REGSET(ZREG_RSP) | ZEND_REGSET(ZREG_R14) | ZEND_REGSET(ZREG_R15))
# define ZEND_REGSET_GP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_R0, ZREG_R15), ZEND_REGSET_FIXED)
# define ZEND_REGSET_FP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_XMM0, ZREG_XMM15), ZEND_REGSET_FIXED)
# define ZEND_REGSET_SCRATCH \
	(ZEND_REGSET(ZREG_RAX) | ZEND_REGSET(ZREG_RDX) | ZEND_REGSET(ZREG_RCX) | ZEND_REGSET_INTERVAL(ZREG_R8, ZREG_R11) | ZEND_REGSET_FP)
# define ZEND_REGSET_PRESERVED \
	(ZEND_REGSET(ZREG_RBX) | ZEND_REGSET(ZREG_RBP) | ZEND_REGSET(ZREG_R12) | ZEND_REGSET(ZREG_R13) | ZEND_REGSET(ZREG_RDI) | ZEND_REGSET(ZREG_RSI))
#elif defined(__x86_64__)
# define ZEND_REGSET_FIXED \
	(ZEND_REGSET(ZREG_RSP) | ZEND_REGSET(ZREG_R14) | ZEND_REGSET(ZREG_R15))
# define ZEND_REGSET_GP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_R0, ZREG_R15), ZEND_REGSET_FIXED)
# define ZEND_REGSET_FP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_XMM0, ZREG_XMM15), ZEND_REGSET_FIXED)
# define ZEND_REGSET_SCRATCH \
	(ZEND_REGSET(ZREG_RAX) | ZEND_REGSET(ZREG_RDI) | ZEND_REGSET(ZREG_RSI) | ZEND_REGSET(ZREG_RDX) | ZEND_REGSET(ZREG_RCX) | ZEND_REGSET_INTERVAL(ZREG_R8, ZREG_R11) | ZEND_REGSET_FP)
# define ZEND_REGSET_PRESERVED \
	(ZEND_REGSET(ZREG_RBX) | ZEND_REGSET(ZREG_RBP) | ZEND_REGSET(ZREG_R12) | ZEND_REGSET(ZREG_R13))
#else
# define ZEND_REGSET_FIXED \
	(ZEND_REGSET(ZREG_RSP) | ZEND_REGSET(ZREG_RSI) | ZEND_REGSET(ZREG_RDI))
# define ZEND_REGSET_GP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_R0, ZREG_R7), ZEND_REGSET_FIXED)
# define ZEND_REGSET_FP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_XMM0, ZREG_XMM7), ZEND_REGSET_FIXED)
# define ZEND_REGSET_SCRATCH \
	(ZEND_REGSET(ZREG_RAX) | ZEND_REGSET(ZREG_RCX) | ZEND_REGSET(ZREG_RDX) | ZEND_REGSET_FP)
# define ZEND_REGSET_PRESERVED \
	(ZEND_REGSET(ZREG_RBX) | ZEND_REGSET(ZREG_RBP))
#endif

#define ZEND_REGSET_LOW_PRIORITY \
	(ZEND_REGSET(ZREG_R0) | ZEND_REGSET(ZREG_R1) | ZEND_REGSET(ZREG_XMM0) | ZEND_REGSET(ZREG_XMM1))

#endif /* ZEND_JIT_X86_H */
