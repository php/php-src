/* -*-c-*- */
/**********************************************************************

  vm_exec.c -

  $Author$

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#include <math.h>

#if VMDEBUG > 0
#define DECL_SC_REG(type, r, reg) register type reg_##r

#elif __GNUC__ && __x86_64__
#define DECL_SC_REG(type, r, reg) register type reg_##r __asm__("r" reg)

#elif __GNUC__ && __i386__
#define DECL_SC_REG(type, r, reg) register type reg_##r __asm__("e" reg)

#else
#define DECL_SC_REG(type, r, reg) register type reg_##r
#endif
/* #define DECL_SC_REG(r, reg) VALUE reg_##r */

#if OPT_STACK_CACHING
static VALUE finish_insn_seq[1] = { BIN(finish_SC_ax_ax) };
#elif OPT_CALL_THREADED_CODE
static VALUE const finish_insn_seq[1] = { 0 };
#else
static VALUE finish_insn_seq[1] = { BIN(finish) };
#endif

#if !OPT_CALL_THREADED_CODE
static VALUE
vm_exec_core(rb_thread_t *th, VALUE initial)
{

#if OPT_STACK_CACHING
#if 0
#elif __GNUC__ && __x86_64__
    DECL_SC_REG(VALUE, a, "12");
    DECL_SC_REG(VALUE, b, "13");
#else
    register VALUE reg_a;
    register VALUE reg_b;
#endif
#endif

#if __GNUC__ && __i386__
    DECL_SC_REG(VALUE *, pc, "di");
    DECL_SC_REG(rb_control_frame_t *, cfp, "si");
#define USE_MACHINE_REGS 1

#elif __GNUC__ && __x86_64__
    DECL_SC_REG(VALUE *, pc, "14");
    DECL_SC_REG(rb_control_frame_t *, cfp, "15");
#define USE_MACHINE_REGS 1

#else
    register rb_control_frame_t *reg_cfp;
    VALUE *reg_pc;
#endif

#if USE_MACHINE_REGS

#undef  RESTORE_REGS
#define RESTORE_REGS() \
{ \
  REG_CFP = th->cfp; \
  reg_pc  = reg_cfp->pc; \
}

#undef  REG_PC
#define REG_PC reg_pc
#undef  GET_PC
#define GET_PC() (reg_pc)
#undef  SET_PC
#define SET_PC(x) (reg_cfp->pc = REG_PC = (x))
#endif

#if OPT_TOKEN_THREADED_CODE || OPT_DIRECT_THREADED_CODE
#include "vmtc.inc"
    if (UNLIKELY(th == 0)) {
#if OPT_STACK_CACHING
	finish_insn_seq[0] = (VALUE)&&LABEL (finish_SC_ax_ax);
#else
	finish_insn_seq[0] = (VALUE)&&LABEL (finish);
#endif
	return (VALUE)insns_address_table;
    }
#endif
    reg_cfp = th->cfp;
    reg_pc = reg_cfp->pc;

#if OPT_STACK_CACHING
    reg_a = initial;
    reg_b = 0;
#endif

  first:
    INSN_DISPATCH();
/*****************/
 #include "vm.inc"
/*****************/
    END_INSNS_DISPATCH();

    /* unreachable */
    rb_bug("vm_eval: unreachable");
    goto first;
}

const void **
rb_vm_get_insns_address_table(void)
{
    return (const void **)vm_exec_core(0, 0);
}

#else

#include "vm.inc"
#include "vmtc.inc"

const void *const *
rb_vm_get_insns_address_table(void)
{
    return insns_address_table;
}

static VALUE
vm_exec_core(rb_thread_t *th, VALUE initial)
{
    register rb_control_frame_t *reg_cfp = th->cfp;
    VALUE ret;

    while (*GET_PC()) {
	reg_cfp = ((rb_insn_func_t) (*GET_PC()))(th, reg_cfp);

	if (reg_cfp == 0) {
	    VALUE err = th->errinfo;
	    th->errinfo = Qnil;
	    return err;
	}
    }

    if (VM_FRAME_TYPE(th->cfp) != VM_FRAME_MAGIC_FINISH) {
	rb_bug("cfp consistency error");
    }

    ret = *(th->cfp->sp-1); /* pop */
    th->cfp++; /* pop cf */
    return ret;
}
#endif
