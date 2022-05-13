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
   |          Hao Sun <hao.sun@arm.com>                                   |
   +----------------------------------------------------------------------+
*/

#ifndef HAVE_JIT_ARM64_H
#define HAVE_JIT_ARM64_H

typedef enum _zend_reg {
	ZREG_NONE = -1,

	ZREG_X0,
	ZREG_X1,
	ZREG_X2,
	ZREG_X3,
	ZREG_X4,
	ZREG_X5,
	ZREG_X6,
	ZREG_X7,
	ZREG_X8,
	ZREG_X9,
	ZREG_X10,
	ZREG_X11,
	ZREG_X12,
	ZREG_X13,
	ZREG_X14,
	ZREG_X15,
	ZREG_X16,
	ZREG_X17,
	ZREG_X18,
	ZREG_X19,
	ZREG_X20,
	ZREG_X21,
	ZREG_X22,
	ZREG_X23,
	ZREG_X24,
	ZREG_X25,
	ZREG_X26,
	ZREG_X27,
	ZREG_X28,
	ZREG_X29,
	ZREG_X30,
	ZREG_X31,

	ZREG_V0,
	ZREG_V1,
	ZREG_V2,
	ZREG_V3,
	ZREG_V4,
	ZREG_V5,
	ZREG_V6,
	ZREG_V7,
	ZREG_V8,
	ZREG_V9,
	ZREG_V10,
	ZREG_V11,
	ZREG_V12,
	ZREG_V13,
	ZREG_V14,
	ZREG_V15,
	ZREG_V16,
	ZREG_V17,
	ZREG_V18,
	ZREG_V19,
	ZREG_V20,
	ZREG_V21,
	ZREG_V22,
	ZREG_V23,
	ZREG_V24,
	ZREG_V25,
	ZREG_V26,
	ZREG_V27,
	ZREG_V28,
	ZREG_V29,
	ZREG_V30,
	ZREG_V31,

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
	uint64_t gpr[32];   /* general purpose integer register */
	double   fpr[32];   /* floating point registers */
} zend_jit_registers_buf;

#define ZREG_RSP   ZREG_X31
#define ZREG_RLR   ZREG_X30
#define ZREG_RFP   ZREG_X29
#define ZREG_RPR   ZREG_X18

#define ZREG_FP    ZREG_X27
#define ZREG_IP    ZREG_X28
#define ZREG_RX    ZREG_IP

#define ZREG_REG0  ZREG_X8
#define ZREG_REG1  ZREG_X9
#define ZREG_REG2  ZREG_X10

#define ZREG_FPR0  ZREG_V0
#define ZREG_FPR1  ZREG_V1

#define ZREG_TMP1  ZREG_X15
#define ZREG_TMP2  ZREG_X16
#define ZREG_TMP3  ZREG_X17
#define ZREG_FPTMP ZREG_V16

#define ZREG_COPY      ZREG_REG0
#define ZREG_FIRST_FPR ZREG_V0

typedef uint64_t zend_regset;

#define ZEND_REGSET_64BIT 1

# define ZEND_REGSET_FIXED \
	(ZEND_REGSET(ZREG_RSP) | ZEND_REGSET(ZREG_RLR) | ZEND_REGSET(ZREG_RFP) | \
	ZEND_REGSET(ZREG_RPR) | ZEND_REGSET(ZREG_FP) | ZEND_REGSET(ZREG_IP) | \
	ZEND_REGSET_INTERVAL(ZREG_TMP1, ZREG_TMP3) | ZEND_REGSET(ZREG_FPTMP))
# define ZEND_REGSET_GP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_X0, ZREG_X30), ZEND_REGSET_FIXED)
# define ZEND_REGSET_FP \
	ZEND_REGSET_DIFFERENCE(ZEND_REGSET_INTERVAL(ZREG_V0, ZREG_V31), ZEND_REGSET_FIXED)
# define ZEND_REGSET_SCRATCH \
	(ZEND_REGSET_INTERVAL(ZREG_X0, ZREG_X17) | ZEND_REGSET_INTERVAL(ZREG_V0, ZREG_V7) | \
	ZEND_REGSET_INTERVAL(ZREG_V16, ZREG_V31))
# define ZEND_REGSET_PRESERVED \
	(ZEND_REGSET_INTERVAL(ZREG_X19, ZREG_X28) | ZEND_REGSET_INTERVAL(ZREG_V8, ZREG_V15))

#define ZEND_REGSET_LOW_PRIORITY \
	(ZEND_REGSET(ZREG_REG0) | ZEND_REGSET(ZREG_REG1) | ZEND_REGSET(ZREG_FPR0) | ZEND_REGSET(ZREG_FPR1))

#endif /* ZEND_JIT_ARM64_H */
