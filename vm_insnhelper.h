/**********************************************************************

  insnhelper.h - helper macros to implement each instructions

  $Author$
  created at: 04/01/01 15:50:34 JST

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#ifndef RUBY_INSNHELPER_H
#define RUBY_INSNHELPER_H

/**
 * VM Debug Level
 *
 * debug level:
 *  0: no debug output
 *  1: show instruction name
 *  2: show stack frame when control stack frame is changed
 *  3: show stack status
 *  4: show register
 *  5:
 * 10: gc check
 */

#ifndef VMDEBUG
#define VMDEBUG 0
#endif

#if 0
#undef  VMDEBUG
#define VMDEBUG 3
#endif

enum {
  BOP_PLUS,
  BOP_MINUS,
  BOP_MULT,
  BOP_DIV,
  BOP_MOD,
  BOP_EQ,
  BOP_EQQ,
  BOP_LT,
  BOP_LE,
  BOP_LTLT,
  BOP_AREF,
  BOP_ASET,
  BOP_LENGTH,
  BOP_SIZE,
  BOP_SUCC,
  BOP_GT,
  BOP_GE,
  BOP_NOT,
  BOP_NEQ,

  BOP_LAST_
};

extern char ruby_vm_redefined_flag[BOP_LAST_];
extern VALUE ruby_vm_const_missing_count;


/**********************************************************/
/* deal with stack                                        */
/**********************************************************/

#define PUSH(x) (SET_SV(x), INC_SP(1))
#define TOPN(n) (*(GET_SP()-(n)-1))
#define POPN(n) (DEC_SP(n))
#define POP()   (DEC_SP(1))
#define STACK_ADDR_FROM_TOP(n) (GET_SP()-(n))

#define GET_TOS()  (tos)	/* dummy */

/**********************************************************/
/* deal with registers                                    */
/**********************************************************/

#define REG_CFP (reg_cfp)
#define REG_PC  (REG_CFP->pc)
#define REG_SP  (REG_CFP->sp)
#define REG_LFP (REG_CFP->lfp)
#define REG_DFP (REG_CFP->dfp)

#define RESTORE_REGS() do { \
  REG_CFP = th->cfp; \
} while (0)

#define REG_A   reg_a
#define REG_B   reg_b

#ifdef COLLECT_USAGE_ANALYSIS
#define USAGE_ANALYSIS_REGISTER_HELPER(a, b, v) \
  (USAGE_ANALYSIS_REGISTER((a), (b)), (v))
#else
#define USAGE_ANALYSIS_REGISTER_HELPER(a, b, v) (v)
#endif

/* PC */
#define GET_PC()           (USAGE_ANALYSIS_REGISTER_HELPER(0, 0, REG_PC))
#define SET_PC(x)          (REG_PC = (USAGE_ANALYSIS_REGISTER_HELPER(0, 1, (x))))
#define GET_CURRENT_INSN() (*GET_PC())
#define GET_OPERAND(n)     (GET_PC()[(n)])
#define ADD_PC(n)          (SET_PC(REG_PC + (n)))

#define GET_PC_COUNT()     (REG_PC - GET_ISEQ()->iseq_encoded)
#define JUMP(dst)          (REG_PC += (dst))

/* FP */
#define GET_CFP()  (USAGE_ANALYSIS_REGISTER_HELPER(2, 0, REG_CFP))
#define GET_LFP()  (USAGE_ANALYSIS_REGISTER_HELPER(3, 0, REG_LFP))
#define SET_LFP(x) (REG_LFP = (USAGE_ANALYSIS_REGISTER_HELPER(3, 1, (x))))
#define GET_DFP()  (USAGE_ANALYSIS_REGISTER_HELPER(4, 0, REG_DFP))
#define SET_DFP(x) (REG_DFP = (USAGE_ANALYSIS_REGISTER_HELPER(4, 1, (x))))

/* SP */
#define GET_SP()   (USAGE_ANALYSIS_REGISTER_HELPER(1, 0, REG_SP))
#define SET_SP(x)  (REG_SP  = (USAGE_ANALYSIS_REGISTER_HELPER(1, 1, (x))))
#define INC_SP(x)  (REG_SP += (USAGE_ANALYSIS_REGISTER_HELPER(1, 1, (x))))
#define DEC_SP(x)  (REG_SP -= (USAGE_ANALYSIS_REGISTER_HELPER(1, 1, (x))))
#define SET_SV(x)  (*GET_SP() = (x))
  /* set current stack value as x */

#define GET_SP_COUNT() (REG_SP - th->stack)

/* instruction sequence C struct */
#define GET_ISEQ() (GET_CFP()->iseq)

/**********************************************************/
/* deal with variables                                    */
/**********************************************************/

#define GET_PREV_DFP(dfp)                ((VALUE *)((dfp)[0] & ~0x03))

#define GET_GLOBAL(entry)       rb_gvar_get((struct rb_global_entry*)(entry))
#define SET_GLOBAL(entry, val)  rb_gvar_set((struct rb_global_entry*)(entry), (val))

#define GET_CONST_INLINE_CACHE(dst) ((IC) * (GET_PC() + (dst) + 2))

/**********************************************************/
/* deal with values                                       */
/**********************************************************/

#define GET_SELF() (USAGE_ANALYSIS_REGISTER_HELPER(5, 0, GET_CFP()->self))

/**********************************************************/
/* deal with control flow 2: method/iterator              */
/**********************************************************/

#define COPY_CREF(c1, c2) do {  \
  NODE *__tmp_c2 = (c2); \
  (c1)->nd_clss = __tmp_c2->nd_clss; \
  (c1)->nd_visi = __tmp_c2->nd_visi;\
  (c1)->nd_next = __tmp_c2->nd_next; \
  if (__tmp_c2->flags & NODE_FL_CREF_PUSHED_BY_EVAL) { \
      (c1)->flags |= NODE_FL_CREF_PUSHED_BY_EVAL; \
  } \
} while (0)

#define CALL_METHOD(num, blockptr, flag, id, me, recv) do { \
    VALUE v = vm_call_method(th, GET_CFP(), (num), (blockptr), (flag), (id), (me), (recv)); \
    if (v == Qundef) { \
	RESTORE_REGS(); \
	NEXT_INSN(); \
    } \
    else { \
	val = v; \
    } \
} while (0)

#define GET_BLOCK_PTR() \
  ((rb_block_t *)(GC_GUARDED_PTR_REF(GET_LFP()[0] & \
				     ((GET_LFP()[0] & 0x02) - 0x02))))

/**********************************************************/
/* deal with control flow 3: exception                    */
/**********************************************************/


/**********************************************************/
/* others                                                 */
/**********************************************************/

/* optimize insn */
#define FIXNUM_REDEFINED_OP_FLAG (1 << 0)
#define FLOAT_REDEFINED_OP_FLAG  (1 << 1)
#define STRING_REDEFINED_OP_FLAG (1 << 2)
#define ARRAY_REDEFINED_OP_FLAG  (1 << 3)
#define HASH_REDEFINED_OP_FLAG   (1 << 4)
#define BIGNUM_REDEFINED_OP_FLAG (1 << 5)
#define SYMBOL_REDEFINED_OP_FLAG (1 << 6)
#define TIME_REDEFINED_OP_FLAG   (1 << 7)

#define FIXNUM_2_P(a, b) ((a) & (b) & 1)
#define BASIC_OP_UNREDEFINED_P(op, klass) (LIKELY((ruby_vm_redefined_flag[(op)]&(klass)) == 0))
#define HEAP_CLASS_OF(obj) RBASIC(obj)->klass

#ifndef USE_IC_FOR_SPECIALIZED_METHOD
#define USE_IC_FOR_SPECIALIZED_METHOD 1
#endif

#if USE_IC_FOR_SPECIALIZED_METHOD

#define CALL_SIMPLE_METHOD(num, id, recv) do { \
    VALUE klass = CLASS_OF(recv); \
    CALL_METHOD((num), 0, 0, (id), vm_method_search((id), klass, ic), (recv)); \
} while (0)

#else

#define CALL_SIMPLE_METHOD(num, id, recv) do { \
    VALUE klass = CLASS_OF(recv); \
    CALL_METHOD((num), 0, 0, (id), rb_method_entry(klass, (id)), (recv)); \
} while (0)

#endif

static VALUE ruby_vm_global_state_version = 1;

#define GET_VM_STATE_VERSION() (ruby_vm_global_state_version)
#define INC_VM_STATE_VERSION() do { \
    ruby_vm_global_state_version = (ruby_vm_global_state_version + 1); \
    if (ruby_vm_global_state_version == 0) vm_clear_all_cache(); \
} while (0)
static void vm_clear_all_cache(void);

static VALUE make_no_method_exception(VALUE exc, const char *format,
				      VALUE obj, int argc, const VALUE *argv);


#endif /* RUBY_INSNHELPER_H */
