/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
	ZREG_ZVAL_COPY_R0,
} zend_reg;

typedef struct _zend_jit_registers_buf {
#if defined(__x86_64__) || defined(_WIN64)
	uint64_t r[16];
	double xmm[16];
#else
	uint32_t r[8];
	double xmm[8];
#endif
} zend_jit_registers_buf;

#define ZREG_RAX ZREG_R0
#define ZREG_RCX ZREG_R1
#define ZREG_RDX ZREG_R2
#define ZREG_RBX ZREG_R3
#define ZREG_RSP ZREG_R4
#define ZREG_RBP ZREG_R5
#define ZREG_RSI ZREG_R6
#define ZREG_RDI ZREG_R7

#ifdef _WIN64
# define ZREG_FP      ZREG_R14
# define ZREG_IP      ZREG_R15
# define ZREG_RX      ZREG_IP
# define ZREG_FCARG1a ZREG_RCX
# define ZREG_FCARG2a ZREG_RDX
#elif defined(__x86_64__)
# define ZREG_FP      ZREG_R14
# define ZREG_IP      ZREG_R15
# define ZREG_RX      ZREG_IP
# define ZREG_FCARG1a ZREG_RDI
# define ZREG_FCARG2a ZREG_RSI
#else
# define ZREG_FP      ZREG_RSI
# define ZREG_IP      ZREG_RDI
# define ZREG_RX      ZREG_IP
# define ZREG_FCARG1a ZREG_RCX
# define ZREG_FCARG2a ZREG_RDX
#endif

extern const char *zend_reg_name[];

typedef uint32_t zend_regset;

#define ZEND_REGSET_EMPTY 0

#define ZEND_REGSET_IS_EMPTY(regset) \
	(regset == ZEND_REGSET_EMPTY)

#define ZEND_REGSET(reg) \
	(1u << (reg))

#define ZEND_REGSET_INTERVAL(reg1, reg2) \
	(((1u << ((reg2) - (reg1) + 1)) - 1) << (reg1))

#define ZEND_REGSET_IN(regset, reg) \
	(((regset) & ZEND_REGSET(reg)) != 0)

#define ZEND_REGSET_INCL(regset, reg) \
	(regset) |= ZEND_REGSET(reg)

#define ZEND_REGSET_EXCL(regset, reg) \
	(regset) &= ~ZEND_REGSET(reg)

#define ZEND_REGSET_UNION(set1, set2) \
	((set1) | (set2))

#define ZEND_REGSET_INTERSECTION(set1, set2) \
	((set1) & (set2))

#define ZEND_REGSET_DIFFERENCE(set1, set2) \
	((set1) & ~(set2))

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

#ifndef _WIN32
#define ZEND_REGSET_FIRST(set) ((zend_reg)__builtin_ctz(set))
#define ZEND_REGSET_LAST(set)  ((zend_reg)(__builtin_clz(set)^31)))
#else
#include <intrin.h>
uint32_t __inline __zend_jit_ctz( uint32_t value ) {
	DWORD trailing_zero = 0;
	if (_BitScanForward(&trailing_zero, value)) {
		return trailing_zero;
	}
	return 32;
}
uint32_t __inline __zend_jit_clz(uint32_t value) {
	DWORD leading_zero = 0;
	if (_BitScanReverse(&leading_zero, value)) {
		return 31 - leading_zero;
	}
	return 32;
}
#define ZEND_REGSET_FIRST(set) ((zend_reg)__zend_jit_ctz(set))
#define ZEND_REGSET_LAST(set)  ((zend_reg)(__zend_jit_clz(set)^31)))
#endif

#define ZEND_REGSET_FOREACH(set, reg) \
	do { \
		zend_regset _tmp = (set); \
		while (!ZEND_REGSET_IS_EMPTY(_tmp)) { \
			zend_reg _reg = ZEND_REGSET_FIRST(_tmp); \
			ZEND_REGSET_EXCL(_tmp, _reg); \
			reg = _reg; \

#define ZEND_REGSET_FOREACH_END() \
		} \
	} while (0)

typedef uintptr_t zend_jit_addr;

#define IS_CONST_ZVAL            0
#define IS_MEM_ZVAL              1
#define IS_REG                   2

#define _ZEND_ADDR_MODE_MASK     0x3
#define _ZEND_ADDR_REG_SHIFT     2
#define _ZEND_ADDR_REG_MASK      0x3f
#define _ZEND_ADDR_OFFSET_SHIFT  8
#define _ZEND_ADDR_REG_STORE_BIT 8
#define _ZEND_ADDR_REG_LOAD_BIT  9
#define _ZEND_ADDR_REG_LAST_USE_BIT  10

#define ZEND_ADDR_CONST_ZVAL(zv) \
	(((zend_jit_addr)(uintptr_t)(zv)) | IS_CONST_ZVAL)
#define ZEND_ADDR_MEM_ZVAL(reg, offset) \
	((((zend_jit_addr)(uintptr_t)(offset)) << _ZEND_ADDR_OFFSET_SHIFT) | \
	(((zend_jit_addr)(uintptr_t)(reg)) << _ZEND_ADDR_REG_SHIFT) | \
	IS_MEM_ZVAL)
#define ZEND_ADDR_REG(reg) \
	((((zend_jit_addr)(uintptr_t)(reg)) << _ZEND_ADDR_REG_SHIFT) | \
	IS_REG)

#define Z_MODE(addr)     (((addr) & _ZEND_ADDR_MODE_MASK))
#define Z_ZV(addr)       ((zval*)(addr))
#define Z_OFFSET(addr)   ((uint32_t)((addr)>>_ZEND_ADDR_OFFSET_SHIFT))
#define Z_REG(addr)      ((zend_reg)(((addr)>>_ZEND_ADDR_REG_SHIFT) & _ZEND_ADDR_REG_MASK))
#define Z_STORE(addr)    ((zend_reg)(((addr)>>_ZEND_ADDR_REG_STORE_BIT) & 1))
#define Z_LOAD(addr)     ((zend_reg)(((addr)>>_ZEND_ADDR_REG_LOAD_BIT) & 1))
#define Z_LAST_USE(addr) ((zend_reg)(((addr)>>_ZEND_ADDR_REG_LAST_USE_BIT) & 1))

#define OP_REG_EX(reg, store, load, last_use) \
	((reg) | \
	 ((store) ? (1 << (_ZEND_ADDR_REG_STORE_BIT-_ZEND_ADDR_REG_SHIFT)) : 0) | \
	 ((load) ? (1 << (_ZEND_ADDR_REG_LOAD_BIT-_ZEND_ADDR_REG_SHIFT)) : 0) | \
	 ((last_use) ? (1 << (_ZEND_ADDR_REG_LAST_USE_BIT-_ZEND_ADDR_REG_SHIFT)) : 0) \
	)

#define OP_REG(ssa_op, op) \
	(ra && ssa_op->op >= 0 && ra[ssa_op->op] ? \
		OP_REG_EX(ra[ssa_op->op]->reg, \
			(ra[ssa_op->op]->flags & ZREG_STORE), \
			(ra[ssa_op->op]->flags & ZREG_LOAD), \
			zend_ival_is_last_use(ra[ssa_op->op], ssa_op - ssa->ops) \
		) : ZREG_NONE)

static zend_always_inline zend_jit_addr _zend_jit_decode_op(zend_uchar op_type, znode_op op, const zend_op *opline, zend_reg reg)
{
	if (op_type == IS_CONST) {
#if ZEND_USE_ABS_CONST_ADDR
		return ZEND_ADDR_CONST_ZVAL(op.zv);
#else
		return ZEND_ADDR_CONST_ZVAL(RT_CONSTANT(opline, op));
#endif
	} else {
		ZEND_ASSERT(op_type & (IS_CV|IS_TMP_VAR|IS_VAR));
		if (reg != ZREG_NONE) {
			return ZEND_ADDR_REG(reg);
		} else {
			return ZEND_ADDR_MEM_ZVAL(ZREG_FP, op.var);
		}
	}
}

#define OP_ADDR(opline, type, op) \
	_zend_jit_decode_op((opline)->type, (opline)->op, opline, ZREG_NONE)

#define OP1_ADDR() \
	OP_ADDR(opline, op1_type, op1)
#define OP2_ADDR() \
	OP_ADDR(opline, op2_type, op2)
#define RES_ADDR() \
	OP_ADDR(opline, result_type, result)
#define OP1_DATA_ADDR() \
	OP_ADDR(opline + 1, op1_type, op1)

#define OP_REG_ADDR(opline, type, _op, _ssa_op) \
	_zend_jit_decode_op((opline)->type, (opline)->_op, opline, \
		OP_REG(ssa_op, _ssa_op))

#define OP1_REG_ADDR() \
	OP_REG_ADDR(opline, op1_type, op1, op1_use)
#define OP2_REG_ADDR() \
	OP_REG_ADDR(opline, op2_type, op2, op2_use)
#define RES_REG_ADDR() \
	OP_REG_ADDR(opline, result_type, result, result_def)
#define OP1_DATA_REG_ADDR() \
	OP_REG_ADDR(opline + 1, op1_type, op1, op1_use)

#define OP1_DEF_REG_ADDR() \
	OP_REG_ADDR(opline, op1_type, op1, op1_def)
#define OP2_DEF_REG_ADDR() \
	OP_REG_ADDR(opline, op2_type, op2, op2_def)
#define RES_USE_REG_ADDR() \
	OP_REG_ADDR(opline, result_type, result, result_use)
#define OP1_DATA_DEF_REG_ADDR() \
	OP_REG_ADDR(opline + 1, op1_type, op1, op1_def)

static zend_always_inline zend_bool zend_jit_same_addr(zend_jit_addr addr1, zend_jit_addr addr2)
{
	if (addr1 == addr2) {
		return 1;
	} else if (Z_MODE(addr1) == IS_REG && Z_MODE(addr2) == IS_REG) {
		return Z_REG(addr1) == Z_REG(addr2);
	}
	return 0;
}

#endif /* ZEND_JIT_X86_H */
