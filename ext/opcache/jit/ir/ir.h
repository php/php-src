/*
 * IR - Lightweight JIT Compilation Framework
 * (Public API)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_H
#define IR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define IR_VERSION "0.0.1"

#ifdef _WIN32
/* TODO Handle ARM, too. */
# if defined(_M_X64)
#  define __SIZEOF_SIZE_T__ 8
# elif defined(_M_IX86)
#  define __SIZEOF_SIZE_T__ 4
# endif
/* Only supported is little endian for any arch on Windows,
   so just fake the same for all. */
# define __ORDER_LITTLE_ENDIAN__ 1
# define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
# ifndef __has_builtin
#  define __has_builtin(arg) (0)
# endif
#endif

#if defined(IR_TARGET_X86)
# define IR_TARGET "x86"
#elif defined(IR_TARGET_X64)
# ifdef _WIN64
#  define IR_TARGET "Windows-x86_64" /* 64-bit Windows use different ABI and calling convention */
# else
#  define IR_TARGET "x86_64"
# endif
#elif defined(IR_TARGET_AARCH64)
# define IR_TARGET "aarch64"
#else
# error "Unknown IR target"
#endif

#if defined(__SIZEOF_SIZE_T__)
# if __SIZEOF_SIZE_T__ == 8
#  define IR_64 1
# elif __SIZEOF_SIZE_T__ != 4
#  error "Unknown addr size"
# endif
#else
# error "Unknown addr size"
#endif

#if defined(__BYTE_ORDER__)
# if defined(__ORDER_LITTLE_ENDIAN__)
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define IR_STRUCT_LOHI(lo, hi) struct {lo; hi;}
#  endif
# endif
# if defined(__ORDER_BIG_ENDIAN__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define IR_STRUCT_LOHI(lo, hi) struct {hi; lo;}
#  endif
# endif
#endif
#ifndef IR_STRUCT_LOHI
# error "Unknown byte order"
#endif

#ifdef __has_attribute
# if __has_attribute(always_inline)
#  define IR_ALWAYS_INLINE static inline __attribute__((always_inline))
# endif
# if __has_attribute(noinline)
#  define IR_NEVER_INLINE __attribute__((noinline))
# endif
#else
# define __has_attribute(x) 0
#endif

#ifndef IR_ALWAYS_INLINE
# define IR_ALWAYS_INLINE static inline
#endif
#ifndef IR_NEVER_INLINE
# define IR_NEVER_INLINE
#endif

#ifdef IR_PHP
# include "ir_php.h"
#endif

/* IR Type flags (low 4 bits are used for type size) */
#define IR_TYPE_SIGNED     (1<<4)
#define IR_TYPE_UNSIGNED   (1<<5)
#define IR_TYPE_FP         (1<<6)
#define IR_TYPE_SPECIAL    (1<<7)
#define IR_TYPE_BOOL       (IR_TYPE_SPECIAL|IR_TYPE_UNSIGNED)
#define IR_TYPE_ADDR       (IR_TYPE_SPECIAL|IR_TYPE_UNSIGNED)
#define IR_TYPE_CHAR       (IR_TYPE_SPECIAL|IR_TYPE_SIGNED)

/* List of IR types */
#define IR_TYPES(_) \
	_(BOOL,   bool,      b,    IR_TYPE_BOOL)     \
	_(U8,     uint8_t,   u8,   IR_TYPE_UNSIGNED) \
	_(U16,    uint16_t,  u16,  IR_TYPE_UNSIGNED) \
	_(U32,    uint32_t,  u32,  IR_TYPE_UNSIGNED) \
	_(U64,    uint64_t,  u64,  IR_TYPE_UNSIGNED) \
	_(ADDR,   uintptr_t, addr, IR_TYPE_ADDR)     \
	_(CHAR,   char,      c,    IR_TYPE_CHAR)     \
	_(I8,     int8_t,    i8,   IR_TYPE_SIGNED)   \
	_(I16,    int16_t,   i16,  IR_TYPE_SIGNED)   \
	_(I32,    int32_t,   i32,  IR_TYPE_SIGNED)   \
	_(I64,    int64_t,   i64,  IR_TYPE_SIGNED)   \
	_(DOUBLE, double,    d,    IR_TYPE_FP)       \
	_(FLOAT,  float,     f,    IR_TYPE_FP)       \

#define IR_IS_TYPE_UNSIGNED(t) ((t) < IR_CHAR)
#define IR_IS_TYPE_SIGNED(t)   ((t) >= IR_CHAR && (t) < IR_DOUBLE)
#define IR_IS_TYPE_INT(t)      ((t) < IR_DOUBLE)
#define IR_IS_TYPE_FP(t)       ((t) >= IR_DOUBLE)

#define IR_TYPE_ENUM(name, type, field, flags) IR_ ## name,

typedef enum _ir_type {
	IR_VOID,
	IR_TYPES(IR_TYPE_ENUM)
	IR_LAST_TYPE
} ir_type;

#ifdef IR_64
# define IR_SIZE_T     IR_U64
# define IR_SSIZE_T    IR_I64
# define IR_UINTPTR_T  IR_U64
# define IR_INTPTR_T   IR_I64
#else
# define IR_SIZE_T     IR_U32
# define IR_SSIZE_T    IR_I32
# define IR_UINTPTR_T  IR_U32
# define IR_INTPTR_T   IR_I32
#endif

/* List of IR opcodes
 * ==================
 *
 * Each instruction is described by a type (opcode, flags, op1_type, op2_type, op3_type)
 *
 * flags
 * -----
 * v     - void
 * d     - data      IR_OP_FLAG_DATA
 * r     - ref       IR_OP_FLAG_DATA alias
 * p     - pinned    IR_OP_FLAG_DATA + IR_OP_FLAG_PINNED
 * c     - control   IR_OP_FLAG_CONTROL
 * S     - control   IR_OP_FLAG_CONTROL + IR_OP_FLAG_BB_START
 * E     - control   IR_OP_FLAG_CONTROL + IR_OP_FLAG_BB_END
 * T     - control   IR_OP_FLAG_CONTROL + IR_OP_FLAG_BB_END + IR_OP_FLAG_TERMINATOR
 * l     - load      IR_OP_FLAG_MEM + IR_OP_FLAG_MEM_LOAD
 * s     - store     IR_OP_FLAG_MEM + IR_OP_FLAG_STORE
 * x     - call      IR_OP_FLAG_MEM + IR_OP_FLAG_CALL
 * a     - alloc     IR_OP_FLAG_MEM + IR_OP_FLAG_ALLOC
 * 0-3   - number of input edges
 * N     - number of arguments is defined in the insn->inputs_count (MERGE, PHI, CALL)
 * X1-X3 - number of extra data ops
 * C     - commutative operation ("d2C" => IR_OP_FLAG_DATA + IR_OP_FLAG_COMMUTATIVE)
 *
 * operand types
 * -------------
 * ___ - unused
 * def - reference to a definition op (data-flow use-def dependency edge)
 * ref - memory reference (data-flow use-def dependency edge)
 * var - variable reference (data-flow use-def dependency edge)
 * arg - argument reference CALL/TAILCALL/CARG->CARG
 * src - reference to a previous control region (IF, IF_TRUE, IF_FALSE, MERGE, LOOP_BEGIN, LOOP_END, RETURN)
 * reg - data-control dependency on region (PHI, VAR, PARAM)
 * ret - reference to a previous RETURN instruction (RETURN)
 * str - string: variable/argument name (VAR, PARAM, CALL, TAILCALL)
 * num - number: argument number (PARAM)
 * prb - branch probability 1-99 (0 - unspecified): (IF_TRUE, IF_FALSE, CASE_VAL, CASE_DEFAULT)
 * opt - optional number
 * pro - function prototype
 *
 * The order of IR opcodes is carefully selected for efficient folding.
 * - foldable instruction go first
 * - NOP is never used (code 0 is used as ANY pattern)
 * - CONST is the most often used instruction (encode with 1 bit)
 * - equality inversion:  EQ <-> NE                         => op =^ 1
 * - comparison inversion: [U]LT <-> [U]GT, [U]LE <-> [U]GE  => op =^ 3
 */

#define IR_OPS(_) \
	/* special op (must be the first !!!)                               */ \
	_(NOP,          v,    ___, ___, ___) /* empty instruction           */ \
	\
	/* constants reference                                              */ \
	_(C_BOOL,       r0,   ___, ___, ___) /* constant                    */ \
	_(C_U8,         r0,   ___, ___, ___) /* constant                    */ \
	_(C_U16,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_U32,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_U64,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_ADDR,       r0,   ___, ___, ___) /* constant                    */ \
	_(C_CHAR,       r0,   ___, ___, ___) /* constant                    */ \
	_(C_I8,         r0,   ___, ___, ___) /* constant                    */ \
	_(C_I16,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_I32,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_I64,        r0,   ___, ___, ___) /* constant                    */ \
	_(C_DOUBLE,     r0,   ___, ___, ___) /* constant                    */ \
	_(C_FLOAT,      r0,   ___, ___, ___) /* constant                    */ \
	\
	/* equality ops  */                                                    \
	_(EQ,           d2C,  def, def, ___) /* equal                       */ \
	_(NE,           d2C,  def, def, ___) /* not equal                   */ \
	\
	/* comparison ops (order matters, LT must be a modulo of 4 !!!)     */ \
	_(LT,           d2,   def, def, ___) /* less                        */ \
	_(GE,           d2,   def, def, ___) /* greater or equal            */ \
	_(LE,           d2,   def, def, ___) /* less or equal               */ \
	_(GT,           d2,   def, def, ___) /* greater                     */ \
	_(ULT,          d2,   def, def, ___) /* unsigned less               */ \
	_(UGE,          d2,   def, def, ___) /* unsigned greater or equal   */ \
	_(ULE,          d2,   def, def, ___) /* unsigned less or equal      */ \
	_(UGT,          d2,   def, def, ___) /* unsigned greater            */ \
	\
	/* arithmetic ops                                                   */ \
	_(ADD,          d2C,  def, def, ___) /* addition                    */ \
	_(SUB,          d2,   def, def, ___) /* subtraction (must be ADD+1) */ \
	_(MUL,          d2C,  def, def, ___) /* multiplication              */ \
	_(DIV,          d2,   def, def, ___) /* division                    */ \
	_(MOD,          d2,   def, def, ___) /* modulo                      */ \
	_(NEG,          d1,   def, ___, ___) /* change sign                 */ \
	_(ABS,          d1,   def, ___, ___) /* absolute value              */ \
	/* (LDEXP, MIN, MAX, FPMATH)                                        */ \
	\
	/* type conversion ops                                              */ \
	_(SEXT,         d1,   def, ___, ___) /* sign extension              */ \
	_(ZEXT,         d1,   def, ___, ___) /* zero extension              */ \
	_(TRUNC,        d1,   def, ___, ___) /* truncates to int type       */ \
	_(BITCAST,      d1,   def, ___, ___) /* binary representation       */ \
	_(INT2FP,       d1,   def, ___, ___) /* int to float conversion     */ \
	_(FP2INT,       d1,   def, ___, ___) /* float to int conversion     */ \
	_(FP2FP,        d1,   def, ___, ___) /* float to float conversion   */ \
	_(PROTO,        d1X1, def, pro, ___) /* apply function prototype    */ \
	\
	/* overflow-check                                                   */ \
	_(ADD_OV,       d2C,  def, def, ___) /* addition                    */ \
	_(SUB_OV,       d2,   def, def, ___) /* subtraction                 */ \
	_(MUL_OV,       d2C,  def, def, ___) /* multiplication              */ \
	_(OVERFLOW,     d1,   def, ___, ___) /* overflow check add/sub/mul  */ \
	\
	/* bitwise and shift ops                                            */ \
	_(NOT,          d1,   def, ___, ___) /* bitwise NOT                 */ \
	_(OR,           d2C,  def, def, ___) /* bitwise OR                  */ \
	_(AND,          d2C,  def, def, ___) /* bitwise AND                 */ \
	_(XOR,          d2C,  def, def, ___) /* bitwise XOR                 */ \
	_(SHL,	        d2,   def, def, ___) /* logic shift left            */ \
	_(SHR,	        d2,   def, def, ___) /* logic shift right           */ \
	_(SAR,	        d2,   def, def, ___) /* arithmetic shift right      */ \
	_(ROL,	        d2,   def, def, ___) /* rotate left                 */ \
	_(ROR,	        d2,   def, def, ___) /* rotate right                */ \
	_(BSWAP,        d1,   def, ___, ___) /* byte swap                   */ \
	_(CTPOP,        d1,   def, ___, ___) /* count population            */ \
	_(CTLZ,         d1,   def, ___, ___) /* count leading zeros         */ \
	_(CTTZ,         d1,   def, ___, ___) /* count trailing zeros        */ \
	\
	/* branch-less conditional ops                                      */ \
	_(MIN,	        d2C,  def, def, ___) /* min(op1, op2)               */ \
	_(MAX,	        d2C,  def, def, ___) /* max(op1, op2)               */ \
	_(COND,	        d3,   def, def, def) /* op1 ? op2 : op3             */ \
	\
	/* data-flow and miscellaneous ops                                  */ \
	_(PHI,          pN,   reg, def, def) /* SSA Phi function            */ \
	_(COPY,         d1X1, def, opt, ___) /* COPY (last foldable op)     */ \
	_(PI,           p2,   reg, def, ___) /* e-SSA Pi constraint ???     */ \
	_(FRAME_ADDR,   d0,   ___, ___, ___) /* function frame address      */ \
	/* (USE, RENAME)                                                    */ \
	\
	/* data ops                                                         */ \
	_(PARAM,        p1X2, reg, str, num) /* incoming parameter proj.    */ \
	_(VAR,	        p1X1, reg, str, ___) /* local variable              */ \
	_(FUNC_ADDR,    r0,   ___, ___, ___) /* constant func ref           */ \
	_(FUNC,         r0,   ___, ___, ___) /* constant func ref           */ \
	_(SYM,          r0,   ___, ___, ___) /* constant symbol ref         */ \
	_(STR,          r0,   ___, ___, ___) /* constant str ref            */ \
	\
	/* call ops                                                         */ \
	_(CALL,         xN,   src, def, def) /* CALL(src, func, args...)    */ \
	_(TAILCALL,     xN,   src, def, def) /* CALL+RETURN                 */ \
	\
	/* memory reference and load/store ops                              */ \
	_(ALLOCA,       a2,   src, def, ___) /* alloca(def)                 */ \
	_(AFREE,        a2,   src, def, ___) /* revert alloca(def)          */ \
	_(VADDR,        d1,   var, ___, ___) /* load address of local var   */ \
	_(VLOAD,        l2,   src, var, ___) /* load value of local var     */ \
	_(VSTORE,       s3,   src, var, def) /* store value to local var    */ \
	_(RLOAD,        l1X2, src, num, opt) /* load value from register    */ \
	_(RSTORE,       s2X1, src, def, num) /* store value into register   */ \
	_(LOAD,         l2,   src, ref, ___) /* load from memory            */ \
	_(STORE,        s3,   src, ref, def) /* store to memory             */ \
	_(TLS,          l1X2, src, num, num) /* thread local variable       */ \
	_(TRAP,         x1,   src, ___, ___) /* DebugBreak                  */ \
	/* memory reference ops (A, H, U, S, TMP, STR, NEW, X, V) ???       */ \
	\
	/* va_args                                                          */ \
	_(VA_START,     x2,   src, def, ___) /* va_start(va_list)           */ \
	_(VA_END,       x2,   src, def, ___) /* va_end(va_list)             */ \
	_(VA_COPY,      x3,   src, def, def) /* va_copy(dst, stc)           */ \
	_(VA_ARG,       x2,   src, def, ___) /* va_arg(va_list)             */ \
	\
	/* guards                                                           */ \
	_(GUARD,        c3,   src, def, def) /* IF without second successor */ \
	_(GUARD_NOT  ,  c3,   src, def, def) /* IF without second successor */ \
	\
	/* deoptimization                                                   */ \
	_(SNAPSHOT,     xN,   src, def, def) /* SNAPSHOT(src, args...)      */ \
	\
	/* control-flow nodes                                               */ \
	_(START,        S0X1, ret, ___, ___) /* function start              */ \
	_(ENTRY,        S1X1, src, num, ___) /* entry with a fake src edge  */ \
	_(BEGIN,        S1,   src, ___, ___) /* block start                 */ \
	_(IF_TRUE,      S1X1, src, prb, ___) /* IF TRUE proj.               */ \
	_(IF_FALSE,     S1X1, src, prb, ___) /* IF FALSE proj.              */ \
	_(CASE_VAL,     S2X1, src, def, prb) /* switch proj.                */ \
	_(CASE_DEFAULT, S1X1, src, prb, ___) /* switch proj.                */ \
	_(MERGE,        SN,   src, src, src) /* control merge               */ \
	_(LOOP_BEGIN,   SN,   src, src, src) /* loop start                  */ \
	_(END,          E1,   src, ___, ___) /* block end                   */ \
	_(LOOP_END,     E1,   src, ___, ___) /* loop end                    */ \
	_(IF,           E2,   src, def, ___) /* conditional control split   */ \
	_(SWITCH,       E2,   src, def, ___) /* multi-way control split     */ \
	_(RETURN,       T2X1, src, def, ret) /* function return             */ \
	_(IJMP,         T2X1, src, def, ret) /* computed goto               */ \
	_(UNREACHABLE,  T1X2, src, ___, ret) /* unreachable (tailcall, etc) */ \
	\
	/* deoptimization helper                                            */ \
	_(EXITCALL,     x2,   src, def, ___) /* save CPU regs and call op2  */ \


#define IR_OP_ENUM(name, flags, op1, op2, op3) IR_ ## name,

typedef enum _ir_op {
	IR_OPS(IR_OP_ENUM)
#ifdef IR_PHP
	IR_PHP_OPS(IR_OP_ENUM)
#endif
	IR_LAST_OP
} ir_op;

/* IR Opcode and Type Union */
#define IR_OPT_OP_MASK       0x00ff
#define IR_OPT_TYPE_MASK     0xff00
#define IR_OPT_TYPE_SHIFT    8
#define IR_OPT_INPUTS_SHIFT  16

#define IR_OPT(op, type)     ((uint16_t)(op) | ((uint16_t)(type) << IR_OPT_TYPE_SHIFT))
#define IR_OPTX(op, type, n) ((uint32_t)(op) | ((uint32_t)(type) << IR_OPT_TYPE_SHIFT) | ((uint32_t)(n) << IR_OPT_INPUTS_SHIFT))
#define IR_OPT_TYPE(opt)     (((opt) & IR_OPT_TYPE_MASK) >> IR_OPT_TYPE_SHIFT)

/* IR References */
typedef int32_t ir_ref;

#define IR_IS_CONST_REF(ref) ((ref) < 0)

/* IR Constant Value */
#define IR_UNUSED            0
#define IR_NULL              (-1)
#define IR_FALSE             (-2)
#define IR_TRUE              (-3)
#define IR_LAST_FOLDABLE_OP  IR_COPY

#define IR_CONSTS_LIMIT_MIN (-(IR_TRUE - 1))
#define IR_INSNS_LIMIT_MIN (IR_UNUSED + 1)

#ifndef IR_64
# define ADDR_MEMBER            uintptr_t                  addr;
#else
# define ADDR_MEMBER
#endif
typedef union _ir_val {
	double                             d;
	uint64_t                           u64;
	int64_t                            i64;
#ifdef IR_64
	uintptr_t                          addr;
#endif
	IR_STRUCT_LOHI(
		union {
			uint32_t                   u32;
			int32_t                    i32;
			float                      f;
			ADDR_MEMBER
			ir_ref                     name;
			ir_ref                     str;
			IR_STRUCT_LOHI(
				union {
					uint16_t           u16;
					int16_t            i16;
					IR_STRUCT_LOHI(
						union {
							uint8_t    u8;
							int8_t     i8;
							bool       b;
							char       c;
						},
						uint8_t        u8_hi
					);
				},
				uint16_t               u16_hi
			);
		},
		uint32_t                       u32_hi
	);
} ir_val;
#undef ADDR_MEMBER

/* IR Instruction */
typedef struct _ir_insn {
	IR_STRUCT_LOHI(
		union {
			IR_STRUCT_LOHI(
				union {
					IR_STRUCT_LOHI(
						uint8_t        op,
						uint8_t        type
					);
					uint16_t           opt;
				},
				union {
					uint16_t           inputs_count;       /* number of input control edges for MERGE, PHI, CALL, TAILCALL */
					uint16_t           prev_insn_offset;   /* 16-bit backward offset from current instruction for CSE */
					uint16_t           proto;
				}
			);
			uint32_t                   optx;
			ir_ref                     ops[1];
		},
		union {
			ir_ref                     op1;
			ir_ref                     prev_const;
		}
	);
	union {
		IR_STRUCT_LOHI(
			ir_ref                     op2,
			ir_ref                     op3
		);
		ir_val                         val;
	};
} ir_insn;

/* IR Hash Tables API (private) */
typedef struct _ir_hashtab ir_hashtab;

/* IR String Tables API (implementation in ir_strtab.c) */
typedef struct _ir_strtab {
	void       *data;
	uint32_t    mask;
	uint32_t    size;
	uint32_t    count;
	uint32_t    pos;
	char       *buf;
	uint32_t    buf_size;
	uint32_t    buf_top;
} ir_strtab;

#define ir_strtab_count(strtab) (strtab)->count

typedef void (*ir_strtab_apply_t)(const char *str, uint32_t len, ir_ref val);

void ir_strtab_init(ir_strtab *strtab, uint32_t count, uint32_t buf_size);
ir_ref ir_strtab_lookup(ir_strtab *strtab, const char *str, uint32_t len, ir_ref val);
ir_ref ir_strtab_find(const ir_strtab *strtab, const char *str, uint32_t len);
ir_ref ir_strtab_update(ir_strtab *strtab, const char *str, uint32_t len, ir_ref val);
const char *ir_strtab_str(const ir_strtab *strtab, ir_ref idx);
const char *ir_strtab_strl(const ir_strtab *strtab, ir_ref idx, size_t *len);
void ir_strtab_apply(const ir_strtab *strtab, ir_strtab_apply_t func);
void ir_strtab_free(ir_strtab *strtab);

/* IR Context Flags */
#define IR_FUNCTION            (1<<0) /* Generate a function. */
#define IR_FASTCALL_FUNC       (1<<1) /* Generate a function with fastcall calling convention, x86 32-bit only. */
#define IR_VARARG_FUNC         (1<<2)
#define IR_BUILTIN_FUNC        (1<<3)
#define IR_STATIC              (1<<4)
#define IR_EXTERN              (1<<5)
#define IR_CONST               (1<<6)

#define IR_SKIP_PROLOGUE       (1<<8) /* Don't generate function prologue. */
#define IR_USE_FRAME_POINTER   (1<<9)
#define IR_PREALLOCATED_STACK  (1<<10)
#define IR_NO_STACK_COMBINE    (1<<11)
#define IR_START_BR_TARGET     (1<<12)
#define IR_ENTRY_BR_TARGET     (1<<13)
#define IR_GEN_ENDBR           (1<<14)
#define IR_MERGE_EMPTY_ENTRIES (1<<15)

#define IR_OPT_FOLDING         (1<<16)
#define IR_OPT_CFG             (1<<17) /* merge BBs, by remove END->BEGIN nodes during CFG construction */
#define IR_OPT_CODEGEN         (1<<18)
#define IR_GEN_NATIVE          (1<<19)
#define IR_GEN_CODE            (1<<20) /* C or LLVM */

#define IR_GEN_CACHE_DEMOTE    (1<<21) /* Demote the generated code from closest CPU caches */

/* debug related */
#ifdef IR_DEBUG
# define IR_DEBUG_SCCP         (1<<27)
# define IR_DEBUG_GCM          (1<<28)
# define IR_DEBUG_SCHEDULE     (1<<29)
# define IR_DEBUG_RA           (1<<30)
#endif

typedef struct _ir_ctx           ir_ctx;
typedef struct _ir_use_list      ir_use_list;
typedef struct _ir_block         ir_block;
typedef struct _ir_arena         ir_arena;
typedef struct _ir_live_interval ir_live_interval;
typedef struct _ir_live_range    ir_live_range;
typedef struct _ir_loader        ir_loader;
typedef int8_t ir_regs[4];

typedef void (*ir_snapshot_create_t)(ir_ctx *ctx, ir_ref addr);

#if defined(IR_TARGET_AARCH64)
typedef const void *(*ir_get_exit_addr_t)(uint32_t exit_num);
typedef const void *(*ir_get_veneer_t)(ir_ctx *ctx, const void *addr);
typedef bool (*ir_set_veneer_t)(ir_ctx *ctx, const void *addr, const void *veneer);
#endif

typedef struct _ir_code_buffer {
	void *start;
	void *end;
	void *pos;
} ir_code_buffer;

struct _ir_ctx {
	ir_insn           *ir_base;                 /* two directional array - instructions grow down, constants grow up */
	ir_ref             insns_count;             /* number of instructions stored in instructions buffer */
	ir_ref             insns_limit;             /* size of allocated instructions buffer (it's extended when overflow) */
	ir_ref             consts_count;            /* number of constants stored in constants buffer */
	ir_ref             consts_limit;            /* size of allocated constants buffer (it's extended when overflow) */
	uint32_t           flags;                   /* IR context flags (see IR_* defines above) */
	uint32_t           flags2;                  /* IR context private flags (see IR_* defines in ir_private.h) */
	ir_type            ret_type;                /* Function return type */
	uint32_t           mflags;                  /* CPU specific flags (see IR_X86_... macros below) */
	int32_t            status;                  /* non-zero error code (see IR_ERROR_... macros), app may use negative codes */
	ir_ref             fold_cse_limit;          /* CSE finds identical insns backward from "insn_count" to "fold_cse_limit" */
	ir_insn            fold_insn;               /* temporary storage for folding engine */
	ir_hashtab        *binding;
	ir_use_list       *use_lists;               /* def->use lists for each instruction */
	ir_ref            *use_edges;               /* the actual uses: use = ctx->use_edges[ctx->use_lists[def].refs + n] */
	ir_ref             use_edges_count;         /* number of elements in use_edges[] array */
	uint32_t           cfg_blocks_count;        /* number of elements in cfg_blocks[] array */
	uint32_t           cfg_edges_count;         /* number of elements in cfg_edges[] array */
	ir_block          *cfg_blocks;              /* list of basic blocks (starts from 1) */
	uint32_t          *cfg_edges;               /* the actual basic blocks predecessors and successors edges */
	uint32_t          *cfg_map;                 /* map of instructions to basic block number */
	uint32_t          *rules;                   /* array of target specific code-generation rules (for each instruction) */
	uint32_t          *vregs;
	ir_ref             vregs_count;
	int32_t            spill_base;              /* base register for special spill area (e.g. PHP VM frame pointer) */
	uint64_t           fixed_regset;            /* fixed registers, excluded for regular register allocation */
	int32_t            fixed_stack_red_zone;    /* reusable stack allocated by caller (default 0) */
	int32_t            fixed_stack_frame_size;  /* fixed stack allocated by generated code for spills and registers save/restore */
	int32_t            fixed_call_stack_size;   /* fixed preallocated stack for parameter passing (default 0) */
	uint64_t           fixed_save_regset;       /* registers that always saved/restored in prologue/epilogue */
	uint32_t           locals_area_size;
	uint32_t           gp_reg_params;
	uint32_t           fp_reg_params;
	int32_t            param_stack_size;
	ir_live_interval **live_intervals;
	ir_arena          *arena;
	ir_live_range     *unused_ranges;
	ir_regs           *regs;
	ir_ref            *prev_ref;
	union {
		void          *data;
		ir_ref         control;                 /* used by IR construction API (see ir_builder.h) */
		ir_ref         bb_start;                /* used by target CPU instruction matcher */
		ir_ref         vars;                    /* list of VARs (used by register allocator) */
	};
	ir_snapshot_create_t   snapshot_create;
	int32_t            stack_frame_alignment;
	int32_t            stack_frame_size;        /* spill stack frame size (used by register allocator and code generator) */
	int32_t            call_stack_size;         /* stack for parameter passing (used by register allocator and code generator) */
	uint64_t           used_preserved_regs;
#ifdef IR_TARGET_X86
	int32_t            ret_slot;
#endif
	uint32_t           rodata_offset;
	uint32_t           jmp_table_offset;
	uint32_t           entries_count;
	uint32_t          *entries;                /* array of ENTRY blocks */
	void              *osr_entry_loads;
	ir_code_buffer    *code_buffer;
#if defined(IR_TARGET_AARCH64)
	int32_t            deoptimization_exits;
	const void        *deoptimization_exits_base;
	ir_get_exit_addr_t get_exit_addr;
	ir_get_veneer_t    get_veneer;
	ir_set_veneer_t    set_veneer;
#endif
	ir_loader         *loader;
	ir_strtab          strtab;
	ir_ref             prev_insn_chain[IR_LAST_FOLDABLE_OP + 1];
	ir_ref             prev_const_chain[IR_LAST_TYPE];
};

/* Basic IR Construction API (implementation in ir.c) */
void ir_init(ir_ctx *ctx, uint32_t flags, ir_ref consts_limit, ir_ref insns_limit);
void ir_free(ir_ctx *ctx);
void ir_truncate(ir_ctx *ctx);

ir_ref ir_const(ir_ctx *ctx, ir_val val, uint8_t type);
ir_ref ir_const_i8(ir_ctx *ctx, int8_t c);
ir_ref ir_const_i16(ir_ctx *ctx, int16_t c);
ir_ref ir_const_i32(ir_ctx *ctx, int32_t c);
ir_ref ir_const_i64(ir_ctx *ctx, int64_t c);
ir_ref ir_const_u8(ir_ctx *ctx, uint8_t c);
ir_ref ir_const_u16(ir_ctx *ctx, uint16_t c);
ir_ref ir_const_u32(ir_ctx *ctx, uint32_t c);
ir_ref ir_const_u64(ir_ctx *ctx, uint64_t c);
ir_ref ir_const_bool(ir_ctx *ctx, bool c);
ir_ref ir_const_char(ir_ctx *ctx, char c);
ir_ref ir_const_float(ir_ctx *ctx, float c);
ir_ref ir_const_double(ir_ctx *ctx, double c);
ir_ref ir_const_addr(ir_ctx *ctx, uintptr_t c);

ir_ref ir_const_func_addr(ir_ctx *ctx, uintptr_t c, ir_ref proto);
ir_ref ir_const_func(ir_ctx *ctx, ir_ref str, ir_ref proto);
ir_ref ir_const_sym(ir_ctx *ctx, ir_ref str);
ir_ref ir_const_str(ir_ctx *ctx, ir_ref str);

ir_ref ir_unique_const_addr(ir_ctx *ctx, uintptr_t c);

void ir_print_const(const ir_ctx *ctx, const ir_insn *insn, FILE *f, bool quoted);

ir_ref ir_str(ir_ctx *ctx, const char *s);
ir_ref ir_strl(ir_ctx *ctx, const char *s, size_t len);
const char *ir_get_str(const ir_ctx *ctx, ir_ref idx);
const char *ir_get_strl(const ir_ctx *ctx, ir_ref idx, size_t *len);

#define IR_MAX_PROTO_PARAMS 255

typedef struct _ir_proto_t {
	uint8_t flags;
	uint8_t ret_type;
	uint8_t params_count;
	uint8_t param_types[5];
} ir_proto_t;

ir_ref ir_proto_0(ir_ctx *ctx, uint8_t flags, ir_type ret_type);
ir_ref ir_proto_1(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1);
ir_ref ir_proto_2(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2);
ir_ref ir_proto_3(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3);
ir_ref ir_proto_4(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3,
                                                                ir_type t4);
ir_ref ir_proto_5(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3,
                                                                ir_type t4, ir_type t5);
ir_ref ir_proto(ir_ctx *ctx, uint8_t flags, ir_type ret_type, uint32_t params_counts, uint8_t *param_types);

ir_ref ir_emit(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3);

ir_ref ir_emit0(ir_ctx *ctx, uint32_t opt);
ir_ref ir_emit1(ir_ctx *ctx, uint32_t opt, ir_ref op1);
ir_ref ir_emit2(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2);
ir_ref ir_emit3(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3);

ir_ref ir_emit_N(ir_ctx *ctx, uint32_t opt, int32_t count);
void   ir_set_op(ir_ctx *ctx, ir_ref ref, int32_t n, ir_ref val);

IR_ALWAYS_INLINE void ir_set_op1(ir_ctx *ctx, ir_ref ref, ir_ref val)
{
	ctx->ir_base[ref].op1 = val;
}

IR_ALWAYS_INLINE void ir_set_op2(ir_ctx *ctx, ir_ref ref, ir_ref val)
{
	ctx->ir_base[ref].op2 = val;
}

IR_ALWAYS_INLINE void ir_set_op3(ir_ctx *ctx, ir_ref ref, ir_ref val)
{
	ctx->ir_base[ref].op3 = val;
}

IR_ALWAYS_INLINE ir_ref ir_insn_op(const ir_insn *insn, int32_t n)
{
	const ir_ref *p = insn->ops + n;
	return *p;
}

IR_ALWAYS_INLINE void ir_insn_set_op(ir_insn *insn, int32_t n, ir_ref val)
{
	ir_ref *p = insn->ops + n;
	*p = val;
}

ir_ref ir_fold(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3);

ir_ref ir_fold0(ir_ctx *ctx, uint32_t opt);
ir_ref ir_fold1(ir_ctx *ctx, uint32_t opt, ir_ref op1);
ir_ref ir_fold2(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2);
ir_ref ir_fold3(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3);

ir_ref ir_param(ir_ctx *ctx, ir_type type, ir_ref region, const char *name, int pos);
ir_ref ir_var(ir_ctx *ctx, ir_type type, ir_ref region, const char *name);
ir_ref ir_bind(ir_ctx *ctx, ir_ref var, ir_ref def);

/* Def -> Use lists */
void ir_build_def_use_lists(ir_ctx *ctx);

/* CFG - Control Flow Graph (implementation in ir_cfg.c) */
int ir_build_cfg(ir_ctx *ctx);
int ir_remove_unreachable_blocks(ir_ctx *ctx);
int ir_build_dominators_tree(ir_ctx *ctx);
int ir_find_loops(ir_ctx *ctx);
int ir_schedule_blocks(ir_ctx *ctx);
void ir_build_prev_refs(ir_ctx *ctx);

/* SCCP - Sparse Conditional Constant Propagation (implementation in ir_sccp.c) */
int ir_sccp(ir_ctx *ctx);

/* GCM - Global Code Motion and scheduling (implementation in ir_gcm.c) */
int ir_gcm(ir_ctx *ctx);
int ir_schedule(ir_ctx *ctx);

/* Liveness & Register Allocation (implementation in ir_ra.c) */
#define IR_REG_NONE          -1
#define IR_REG_SPILL_LOAD    (1<<6)
#define IR_REG_SPILL_STORE   (1<<6)
#define IR_REG_SPILL_SPECIAL (1<<7)
#define IR_REG_SPILLED(r) \
	((r) & (IR_REG_SPILL_LOAD|IR_REG_SPILL_STORE|IR_REG_SPILL_SPECIAL))
#define IR_REG_NUM(r) \
	((int8_t)((r) == IR_REG_NONE ? IR_REG_NONE : ((r) & ~(IR_REG_SPILL_LOAD|IR_REG_SPILL_STORE|IR_REG_SPILL_SPECIAL))))

int ir_assign_virtual_registers(ir_ctx *ctx);
int ir_compute_live_ranges(ir_ctx *ctx);
int ir_coalesce(ir_ctx *ctx);
int ir_compute_dessa_moves(ir_ctx *ctx);
int ir_reg_alloc(ir_ctx *ctx);

int ir_regs_number(void);
bool ir_reg_is_int(int32_t reg);
const char *ir_reg_name(int8_t reg, ir_type type);
int32_t ir_get_spill_slot_offset(ir_ctx *ctx, ir_ref ref);

/* Target CPU instruction selection and code generation (see ir_x86.c) */
int ir_match(ir_ctx *ctx);
void *ir_emit_code(ir_ctx *ctx, size_t *size);

bool ir_needs_thunk(ir_code_buffer *code_buffer, void *addr);
void *ir_emit_thunk(ir_code_buffer *code_buffer, void *addr, size_t *size_ptr);
void ir_fix_thunk(void *thunk_entry, void *addr);

/* Target address resolution (implementation in ir_emit.c) */
void *ir_resolve_sym_name(const char *name);

/* Target CPU disassembler (implementation in ir_disasm.c) */
int  ir_disasm_init(void);
void ir_disasm_free(void);
void ir_disasm_add_symbol(const char *name, uint64_t addr, uint64_t size);
const char* ir_disasm_find_symbol(uint64_t addr, int64_t *offset);
int  ir_disasm(const char *name,
               const void *start,
               size_t      size,
               bool        asm_addr,
               ir_ctx     *ctx,
               FILE       *f);

/* Linux perf interface (implementation in ir_perf.c) */
int ir_perf_jitdump_open(void);
int ir_perf_jitdump_close(void);
int ir_perf_jitdump_register(const char *name, const void *start, size_t size);
void ir_perf_map_register(const char *name, const void *start, size_t size);

/* GDB JIT interface (implementation in ir_gdb.c) */
int ir_gdb_register(const char    *name,
                    const void    *start,
                    size_t         size,
                    uint32_t       sp_offset,
                    uint32_t       sp_adjustment);
void ir_gdb_unregister_all(void);
bool ir_gdb_present(void);

/* IR load API (implementation in ir_load.c) */
struct _ir_loader {
	uint32_t default_func_flags;
	bool (*init_module)       (ir_loader *loader, const char *name, const char *filename, const char *target);
	bool (*external_sym_dcl)  (ir_loader *loader, const char *name, uint32_t flags);
	bool (*external_func_dcl) (ir_loader *loader, const char *name,
                               uint32_t flags, ir_type ret_type, uint32_t params_count, const uint8_t *param_types);
	bool (*forward_func_dcl)  (ir_loader *loader, const char *name,
                               uint32_t flags, ir_type ret_type, uint32_t params_count, const uint8_t *param_types);
	bool (*sym_dcl)           (ir_loader *loader, const char *name, uint32_t flags, size_t size, bool has_data);
	bool (*sym_data)          (ir_loader *loader, ir_type type, uint32_t count, const void *data);
	bool (*sym_data_pad)      (ir_loader *loader, size_t offset);
	bool (*sym_data_ref)      (ir_loader *loader, ir_op op, const char *ref, uintptr_t offset);
	bool (*sym_data_end)      (ir_loader *loader);
	bool (*func_init)         (ir_loader *loader, ir_ctx *ctx, const char *name);
	bool (*func_process)      (ir_loader *loader, ir_ctx *ctx, const char *name);
	void*(*resolve_sym_name)  (ir_loader *loader, const char *name, bool add_thunk);
	bool (*has_sym)           (ir_loader *loader, const char *name);
	bool (*add_sym)           (ir_loader *loader, const char *name, void *addr);
};

void ir_loader_init(void);
void ir_loader_free(void);
int ir_load(ir_loader *loader, FILE *f);

/* IR LLVM load API (implementation in ir_load_llvm.c) */
int ir_load_llvm_bitcode(ir_loader *loader, const char *filename);
int ir_load_llvm_asm(ir_loader *loader, const char *filename);

/* IR save API (implementation in ir_save.c) */
void ir_print_proto(const ir_ctx *ctx, ir_ref proto, FILE *f);
void ir_save(const ir_ctx *ctx, FILE *f);

/* IR debug dump API (implementation in ir_dump.c) */
void ir_dump(const ir_ctx *ctx, FILE *f);
void ir_dump_dot(const ir_ctx *ctx, FILE *f);
void ir_dump_use_lists(const ir_ctx *ctx, FILE *f);
void ir_dump_cfg(ir_ctx *ctx, FILE *f);
void ir_dump_cfg_map(const ir_ctx *ctx, FILE *f);
void ir_dump_live_ranges(const ir_ctx *ctx, FILE *f);
void ir_dump_codegen(const ir_ctx *ctx, FILE *f);

/* IR to C conversion (implementation in ir_emit_c.c) */
int ir_emit_c(ir_ctx *ctx, const char *name, FILE *f);
void ir_emit_c_func_decl(const char *name, uint32_t flags, ir_type ret_type, uint32_t params_count, const uint8_t *param_types, FILE *f);
void ir_emit_c_sym_decl(const char *name, uint32_t flags, bool has_data, FILE *f);

/* IR to LLVM conversion (implementation in ir_emit_llvm.c) */
int ir_emit_llvm(ir_ctx *ctx, const char *name, FILE *f);
void ir_emit_llvm_func_decl(const char *name, uint32_t flags, ir_type ret_type, uint32_t params_count, const uint8_t *param_types, FILE *f);
void ir_emit_llvm_sym_decl(const char *name, uint32_t flags, bool has_data, FILE *f);

/* IR verification API (implementation in ir_check.c) */
bool ir_check(const ir_ctx *ctx);
void ir_consistency_check(void);

/* Code patching (implementation in ir_patch.c) */
int ir_patch(const void *code, size_t size, uint32_t jmp_table_size, const void *from_addr, const void *to_addr);

/* CPU information (implementation in ir_cpuinfo.c) */
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# define IR_X86_SSE2     (1<<0)
# define IR_X86_SSE3     (1<<1)
# define IR_X86_SSSE3    (1<<2)
# define IR_X86_SSE41    (1<<3)
# define IR_X86_SSE42    (1<<4)
# define IR_X86_AVX      (1<<5)
# define IR_X86_AVX2     (1<<6)
# define IR_X86_BMI1     (1<<7)
# define IR_X86_CLDEMOTE (1<<8)
#endif

uint32_t ir_cpuinfo(void);

/* Deoptimization helpers */
const void *ir_emit_exitgroup(uint32_t first_exit_point, uint32_t exit_points_per_group, const void *exit_addr, ir_code_buffer *code_buffer, size_t *size_ptr);

/* A reference IR JIT compiler */
IR_ALWAYS_INLINE void *ir_jit_compile(ir_ctx *ctx, int opt_level, size_t *size)
{
	if (opt_level == 0) {
		if (ctx->flags & IR_OPT_FOLDING) {
			// IR_ASSERT(0 && "IR_OPT_FOLDING is incompatible with -O0");
			return NULL;
		}
		ctx->flags &= ~(IR_OPT_CFG | IR_OPT_CODEGEN);

		ir_build_def_use_lists(ctx);

		if (!ir_build_cfg(ctx)
		 || !ir_match(ctx)
		 || !ir_assign_virtual_registers(ctx)
		 || !ir_compute_dessa_moves(ctx)) {
			return NULL;
		}

		return ir_emit_code(ctx, size);
	} else if (opt_level == 1 || opt_level == 2) {
		if (!(ctx->flags & IR_OPT_FOLDING)) {
			// IR_ASSERT(0 && "IR_OPT_FOLDING must be set in ir_init() for -O1 and -O2");
			return NULL;
		}
		ctx->flags |= IR_OPT_CFG | IR_OPT_CODEGEN;

		ir_build_def_use_lists(ctx);

		if (opt_level == 2
		 && !ir_sccp(ctx)) {
			return NULL;
		}

		if (!ir_build_cfg(ctx)
		 || !ir_build_dominators_tree(ctx)
		 || !ir_find_loops(ctx)
		 || !ir_gcm(ctx)
		 || !ir_schedule(ctx)
		 || !ir_match(ctx)
		 || !ir_assign_virtual_registers(ctx)
		 || !ir_compute_live_ranges(ctx)
		 || !ir_coalesce(ctx)
		 || !ir_reg_alloc(ctx)
		 || !ir_schedule_blocks(ctx)) {
			return NULL;
		}

		return ir_emit_code(ctx, size);
	} else {
		// IR_ASSERT(0 && "wrong optimization level");
		return NULL;
	}
}

#define IR_ERROR_CODE_MEM_OVERFLOW               1
#define IR_ERROR_FIXED_STACK_FRAME_OVERFLOW      2
#define IR_ERROR_UNSUPPORTED_CODE_RULE           3
#define IR_ERROR_LINK                            4
#define IR_ERROR_ENCODE                          5

/* IR Memmory Allocation */
#ifndef ir_mem_malloc
# define ir_mem_malloc   malloc
#endif
#ifndef ir_mem_calloc
# define ir_mem_calloc   calloc
#endif
#ifndef ir_mem_realloc
# define ir_mem_realloc  realloc
#endif
#ifndef ir_mem_free
# define ir_mem_free     free
#endif

#ifndef ir_mem_pmalloc
# define ir_mem_pmalloc  malloc
#endif
#ifndef ir_mem_pcalloc
# define ir_mem_pcalloc  calloc
#endif
#ifndef ir_mem_prealloc
# define ir_mem_prealloc realloc
#endif
#ifndef ir_mem_pfree
# define ir_mem_pfree    free
#endif

void *ir_mem_mmap(size_t size);
int ir_mem_unmap(void *ptr, size_t size);
int ir_mem_protect(void *ptr, size_t size);
int ir_mem_unprotect(void *ptr, size_t size);
int ir_mem_flush(void *ptr, size_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IR_H */
