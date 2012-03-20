/**********************************************************************

  eval.c -

  $Author$
  created at: Thu Jun 10 14:22:17 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#include "eval_intern.h"
#include "iseq.h"
#include "gc.h"
#include "ruby/vm.h"
#include "ruby/encoding.h"
#include "internal.h"
#include "vm_core.h"

#define numberof(array) (int)(sizeof(array) / sizeof((array)[0]))

NORETURN(void rb_raise_jump(VALUE));

VALUE rb_eLocalJumpError;
VALUE rb_eSysStackError;

#define exception_error GET_VM()->special_exceptions[ruby_error_reenter]

#include "eval_error.c"
#include "eval_jump.c"

/* initialize ruby */

void
ruby_init(void)
{
    static int initialized = 0;
    int state;

    if (initialized)
	return;
    initialized = 1;

    ruby_init_stack((void *)&state);
    Init_BareVM();
    Init_heap();

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	rb_call_inits();
	ruby_prog_init();
    }
    POP_TAG();

    if (state) {
	error_print();
	exit(EXIT_FAILURE);
    }
    GET_VM()->running = 1;
}

void *
ruby_options(int argc, char **argv)
{
    int state;
    void *volatile iseq = 0;

    ruby_init_stack((void *)&iseq);
    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(GET_THREAD(), iseq = ruby_process_options(argc, argv));
    }
    else {
	rb_clear_trace_func();
	state = error_handle(state);
	iseq = (void *)INT2FIX(state);
    }
    POP_TAG();
    return iseq;
}

static void
ruby_finalize_0(void)
{
    PUSH_TAG();
    if (EXEC_TAG() == 0) {
	rb_trap_exit();
    }
    POP_TAG();
    rb_exec_end_proc();
    rb_clear_trace_func();
}

static void
ruby_finalize_1(void)
{
    ruby_sig_finalize();
    GET_THREAD()->errinfo = Qnil;
    rb_gc_call_finalizer_at_exit();
}

void
ruby_finalize(void)
{
    ruby_finalize_0();
    ruby_finalize_1();
}

int
ruby_cleanup(volatile int ex)
{
    int state;
    volatile VALUE errs[2];
    rb_thread_t *th = GET_THREAD();
    int nerr;

    rb_threadptr_interrupt(th);
    rb_threadptr_check_signal(th);
    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(th, { RUBY_VM_CHECK_INTS(); });
    }
    POP_TAG();

    errs[1] = th->errinfo;
    th->safe_level = 0;
    ruby_init_stack(&errs[STACK_UPPER(errs, 0, 1)]);

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(th, ruby_finalize_0());
    }
    POP_TAG();

    errs[0] = th->errinfo;
    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(th, rb_thread_terminate_all());
    }
    else if (ex == 0) {
	ex = state;
    }
    th->errinfo = errs[1];
    ex = error_handle(ex);
    ruby_finalize_1();

    /* unlock again if finalizer took mutexes. */
    rb_threadptr_unlock_all_locking_mutexes(GET_THREAD());
    POP_TAG();
    rb_thread_stop_timer_thread(1);

#if EXIT_SUCCESS != 0 || EXIT_FAILURE != 1
    switch (ex) {
#if EXIT_SUCCESS != 0
      case 0: ex = EXIT_SUCCESS; break;
#endif
#if EXIT_FAILURE != 1
      case 1: ex = EXIT_FAILURE; break;
#endif
    }
#endif

    state = 0;
    for (nerr = 0; nerr < numberof(errs); ++nerr) {
	VALUE err = errs[nerr];

	if (!RTEST(err)) continue;

	/* th->errinfo contains a NODE while break'ing */
	if (RB_TYPE_P(err, T_NODE)) continue;

	if (rb_obj_is_kind_of(err, rb_eSystemExit)) {
	    ex = sysexit_status(err);
	    break;
	}
	else if (rb_obj_is_kind_of(err, rb_eSignal)) {
	    VALUE sig = rb_iv_get(err, "signo");
	    state = NUM2INT(sig);
	    break;
	}
	else if (ex == EXIT_SUCCESS) {
	    ex = EXIT_FAILURE;
	}
    }
    ruby_vm_destruct(GET_VM());
    if (state) ruby_default_signal(state);

    return ex;
}

static int
ruby_exec_internal(void *n)
{
    volatile int state;
    VALUE iseq = (VALUE)n;
    rb_thread_t *th = GET_THREAD();

    if (!n) return 0;

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(th, {
	    th->base_block = 0;
	    rb_iseq_eval_main(iseq);
	});
    }
    POP_TAG();
    return state;
}

void
ruby_stop(int ex)
{
    exit(ruby_cleanup(ex));
}

int
ruby_executable_node(void *n, int *status)
{
    VALUE v = (VALUE)n;
    int s;

    switch (v) {
      case Qtrue:  s = EXIT_SUCCESS; break;
      case Qfalse: s = EXIT_FAILURE; break;
      default:
	if (!FIXNUM_P(v)) return TRUE;
	s = FIX2INT(v);
    }
    if (status) *status = s;
    return FALSE;
}

int
ruby_run_node(void *n)
{
    int status;
    if (!ruby_executable_node(n, &status)) {
	ruby_cleanup(0);
	return status;
    }
    return ruby_cleanup(ruby_exec_node(n));
}

int
ruby_exec_node(void *n)
{
    ruby_init_stack((void *)&n);
    return ruby_exec_internal(n);
}

/*
 *  call-seq:
 *     Module.nesting    -> array
 *
 *  Returns the list of +Modules+ nested at the point of call.
 *
 *     module M1
 *       module M2
 *         $a = Module.nesting
 *       end
 *     end
 *     $a           #=> [M1::M2, M1]
 *     $a[0].name   #=> "M1::M2"
 */

static VALUE
rb_mod_nesting(void)
{
    VALUE ary = rb_ary_new();
    const NODE *cref = rb_vm_cref();

    while (cref && cref->nd_next) {
	VALUE klass = cref->nd_clss;
	if (!(cref->flags & NODE_FL_CREF_PUSHED_BY_EVAL) &&
	    !NIL_P(klass)) {
	    rb_ary_push(ary, klass);
	}
	cref = cref->nd_next;
    }
    return ary;
}

/*
 *  call-seq:
 *     Module.constants   -> array
 *     Module.constants(inherited)   -> array
 *
 *  In the first form, returns an array of the names of all
 *  constants accessible from the point of call.
 *  This list includes the names of all modules and classes
 *  defined in the global scope.
 *
 *     Module.constants.first(4)
 *        # => [:ARGF, :ARGV, :ArgumentError, :Array]
 *
 *     Module.constants.include?(:SEEK_SET)   # => false
 *
 *     class IO
 *       Module.constants.include?(:SEEK_SET) # => true
 *     end
 *
 *  The second form calls the instance method +constants+.
 */

static VALUE
rb_mod_s_constants(int argc, VALUE *argv, VALUE mod)
{
    const NODE *cref = rb_vm_cref();
    VALUE klass;
    VALUE cbase = 0;
    void *data = 0;

    if (argc > 0) {
	return rb_mod_constants(argc, argv, rb_cModule);
    }

    while (cref) {
	klass = cref->nd_clss;
	if (!(cref->flags & NODE_FL_CREF_PUSHED_BY_EVAL) &&
	    !NIL_P(klass)) {
	    data = rb_mod_const_at(cref->nd_clss, data);
	    if (!cbase) {
		cbase = klass;
	    }
	}
	cref = cref->nd_next;
    }

    if (cbase) {
	data = rb_mod_const_of(cbase, data);
    }
    return rb_const_list(data);
}

void
rb_frozen_class_p(VALUE klass)
{
    const char *desc = "something(?!)";

    if (OBJ_FROZEN(klass)) {
	if (FL_TEST(klass, FL_SINGLETON))
	    desc = "object";
	else {
	    switch (TYPE(klass)) {
	      case T_MODULE:
	      case T_ICLASS:
		desc = "module";
		break;
	      case T_CLASS:
		desc = "class";
		break;
	    }
	}
	rb_error_frozen(desc);
    }
}

NORETURN(static void rb_longjmp(int, volatile VALUE));

static void
setup_exception(rb_thread_t *th, int tag, volatile VALUE mesg)
{
    VALUE at;
    VALUE e;
    const char *file;
    volatile int line = 0;

    if (NIL_P(mesg)) {
	mesg = th->errinfo;
	if (INTERNAL_EXCEPTION_P(mesg)) JUMP_TAG(TAG_FATAL);
    }
    if (NIL_P(mesg)) {
	mesg = rb_exc_new(rb_eRuntimeError, 0, 0);
    }

    file = rb_sourcefile();
    if (file) line = rb_sourceline();
    if (file && !NIL_P(mesg)) {
	if (mesg == sysstack_error) {
	    at = rb_enc_sprintf(rb_usascii_encoding(), "%s:%d", file, line);
	    at = rb_ary_new3(1, at);
	    rb_iv_set(mesg, "bt", at);
	}
	else {
	    at = get_backtrace(mesg);
	    if (NIL_P(at)) {
		at = rb_make_backtrace();
		if (OBJ_FROZEN(mesg)) {
		    mesg = rb_obj_dup(mesg);
		}
		set_backtrace(mesg, at);
	    }
	}
    }
    if (!NIL_P(mesg)) {
	th->errinfo = mesg;
    }

    if (RTEST(ruby_debug) && !NIL_P(e = th->errinfo) &&
	!rb_obj_is_kind_of(e, rb_eSystemExit)) {
	int status;

	PUSH_TAG();
	if ((status = EXEC_TAG()) == 0) {
	    RB_GC_GUARD(e) = rb_obj_as_string(e);
	    if (file && line) {
		warn_printf("Exception `%s' at %s:%d - %s\n",
			    rb_obj_classname(th->errinfo),
			    file, line, RSTRING_PTR(e));
	    }
	    else if (file) {
		warn_printf("Exception `%s' at %s - %s\n",
			    rb_obj_classname(th->errinfo),
			    file, RSTRING_PTR(e));
	    }
	    else {
		warn_printf("Exception `%s' - %s\n",
			    rb_obj_classname(th->errinfo),
			    RSTRING_PTR(e));
	    }
	}
	POP_TAG();
	if (status == TAG_FATAL && th->errinfo == exception_error) {
	    th->errinfo = mesg;
	}
	else if (status) {
	    rb_threadptr_reset_raised(th);
	    JUMP_TAG(status);
	}
    }

    if (rb_threadptr_set_raised(th)) {
	th->errinfo = exception_error;
	rb_threadptr_reset_raised(th);
	JUMP_TAG(TAG_FATAL);
    }

    if (tag != TAG_FATAL) {
	EXEC_EVENT_HOOK(th, RUBY_EVENT_RAISE, th->cfp->self, 0, 0);
    }
}

static void
rb_longjmp(int tag, volatile VALUE mesg)
{
    rb_thread_t *th = GET_THREAD();
    setup_exception(th, tag, mesg);
    rb_thread_raised_clear(th);
    JUMP_TAG(tag);
}

static VALUE make_exception(int argc, VALUE *argv, int isstr);

void
rb_exc_raise(VALUE mesg)
{
    if (!NIL_P(mesg)) {
	mesg = make_exception(1, &mesg, FALSE);
    }
    rb_longjmp(TAG_RAISE, mesg);
}

void
rb_exc_fatal(VALUE mesg)
{
    if (!NIL_P(mesg)) {
	mesg = make_exception(1, &mesg, FALSE);
    }
    rb_longjmp(TAG_FATAL, mesg);
}

void
rb_interrupt(void)
{
    rb_raise(rb_eInterrupt, "%s", "");
}

static VALUE get_errinfo(void);

/*
 *  call-seq:
 *     raise
 *     raise(string)
 *     raise(exception [, string [, array]])
 *     fail
 *     fail(string)
 *     fail(exception [, string [, array]])
 *
 *  With no arguments, raises the exception in <code>$!</code> or raises
 *  a <code>RuntimeError</code> if <code>$!</code> is +nil+.
 *  With a single +String+ argument, raises a
 *  +RuntimeError+ with the string as a message. Otherwise,
 *  the first parameter should be the name of an +Exception+
 *  class (or an object that returns an +Exception+ object when sent
 *  an +exception+ message). The optional second parameter sets the
 *  message associated with the exception, and the third parameter is an
 *  array of callback information. Exceptions are caught by the
 *  +rescue+ clause of <code>begin...end</code> blocks.
 *
 *     raise "Failed to create socket"
 *     raise ArgumentError, "No parameters", caller
 */

static VALUE
rb_f_raise(int argc, VALUE *argv)
{
    VALUE err;
    if (argc == 0) {
	err = get_errinfo();
	if (!NIL_P(err)) {
	    argc = 1;
	    argv = &err;
	}
    }
    rb_raise_jump(rb_make_exception(argc, argv));
    return Qnil;		/* not reached */
}

static VALUE
make_exception(int argc, VALUE *argv, int isstr)
{
    VALUE mesg;
    ID exception;
    int n;

    mesg = Qnil;
    switch (argc) {
      case 0:
	break;
      case 1:
	if (NIL_P(argv[0]))
	    break;
	if (isstr) {
	    mesg = rb_check_string_type(argv[0]);
	    if (!NIL_P(mesg)) {
		mesg = rb_exc_new3(rb_eRuntimeError, mesg);
		break;
	    }
	}
	n = 0;
	goto exception_call;

      case 2:
      case 3:
	n = 1;
      exception_call:
	if (argv[0] == sysstack_error) return argv[0];
	CONST_ID(exception, "exception");
	mesg = rb_check_funcall(argv[0], exception, n, argv+1);
	if (mesg == Qundef) {
	    rb_raise(rb_eTypeError, "exception class/object expected");
	}
	break;
      default:
	rb_check_arity(argc, 0, 3);
	break;
    }
    if (argc > 0) {
	if (!rb_obj_is_kind_of(mesg, rb_eException))
	    rb_raise(rb_eTypeError, "exception object expected");
	if (argc > 2)
	    set_backtrace(mesg, argv[2]);
    }

    return mesg;
}

VALUE
rb_make_exception(int argc, VALUE *argv)
{
    return make_exception(argc, argv, TRUE);
}

void
rb_raise_jump(VALUE mesg)
{
    rb_thread_t *th = GET_THREAD();
    rb_control_frame_t *cfp = th->cfp;
    VALUE klass = cfp->me->klass;
    VALUE self = cfp->self;
    ID mid = cfp->me->called_id;

    th->cfp = RUBY_VM_PREVIOUS_CONTROL_FRAME(th->cfp);

    setup_exception(th, TAG_RAISE, mesg);

    EXEC_EVENT_HOOK(th, RUBY_EVENT_C_RETURN, self, mid, klass);
    rb_thread_raised_clear(th);
    JUMP_TAG(TAG_RAISE);
}

void
rb_jump_tag(int tag)
{
    JUMP_TAG(tag);
}

int
rb_block_given_p(void)
{
    rb_thread_t *th = GET_THREAD();

    if ((th->cfp->lfp[0] & 0x02) == 0 &&
	GC_GUARDED_PTR_REF(th->cfp->lfp[0])) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

int
rb_iterator_p(void)
{
    return rb_block_given_p();
}

VALUE rb_eThreadError;

void
rb_need_block(void)
{
    if (!rb_block_given_p()) {
	rb_vm_localjump_error("no block given", Qnil, 0);
    }
}

VALUE
rb_rescue2(VALUE (* b_proc) (ANYARGS), VALUE data1,
	   VALUE (* r_proc) (ANYARGS), VALUE data2, ...)
{
    int state;
    rb_thread_t *th = GET_THREAD();
    rb_control_frame_t *cfp = th->cfp;
    volatile VALUE result;
    volatile VALUE e_info = th->errinfo;
    va_list args;

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
      retry_entry:
	result = (*b_proc) (data1);
    }
    else {
	th->cfp = cfp; /* restore */

	if (state == TAG_RAISE) {
	    int handle = FALSE;
	    VALUE eclass;

	    va_init_list(args, data2);
	    while ((eclass = va_arg(args, VALUE)) != 0) {
		if (rb_obj_is_kind_of(th->errinfo, eclass)) {
		    handle = TRUE;
		    break;
		}
	    }
	    va_end(args);

	    if (handle) {
		if (r_proc) {
		    PUSH_TAG();
		    if ((state = EXEC_TAG()) == 0) {
			result = (*r_proc) (data2, th->errinfo);
		    }
		    POP_TAG();
		    if (state == TAG_RETRY) {
			state = 0;
			th->errinfo = Qnil;
			goto retry_entry;
		    }
		}
		else {
		    result = Qnil;
		    state = 0;
		}
		if (state == 0) {
		    th->errinfo = e_info;
		}
	    }
	}
    }
    POP_TAG();
    if (state)
	JUMP_TAG(state);

    return result;
}

VALUE
rb_rescue(VALUE (* b_proc)(ANYARGS), VALUE data1,
	  VALUE (* r_proc)(ANYARGS), VALUE data2)
{
    return rb_rescue2(b_proc, data1, r_proc, data2, rb_eStandardError,
		      (VALUE)0);
}

VALUE
rb_protect(VALUE (* proc) (VALUE), VALUE data, int * state)
{
    volatile VALUE result = Qnil;
    int status;
    rb_thread_t *th = GET_THREAD();
    rb_control_frame_t *cfp = th->cfp;
    struct rb_vm_protect_tag protect_tag;
    rb_jmpbuf_t org_jmpbuf;

    protect_tag.prev = th->protect_tag;

    PUSH_TAG();
    th->protect_tag = &protect_tag;
    MEMCPY(&org_jmpbuf, &(th)->root_jmpbuf, rb_jmpbuf_t, 1);
    if ((status = EXEC_TAG()) == 0) {
	SAVE_ROOT_JMPBUF(th, result = (*proc) (data));
    }
    MEMCPY(&(th)->root_jmpbuf, &org_jmpbuf, rb_jmpbuf_t, 1);
    th->protect_tag = protect_tag.prev;
    POP_TAG();

    if (state) {
	*state = status;
    }
    if (status != 0) {
	th->cfp = cfp;
	return Qnil;
    }

    return result;
}

VALUE
rb_ensure(VALUE (*b_proc)(ANYARGS), VALUE data1, VALUE (*e_proc)(ANYARGS), VALUE data2)
{
    int state;
    volatile VALUE result = Qnil;

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	result = (*b_proc) (data1);
    }
    POP_TAG();
    /* TODO: fix me */
    /* retval = prot_tag ? prot_tag->retval : Qnil; */     /* save retval */
    (*e_proc) (data2);
    if (state)
	JUMP_TAG(state);
    return result;
}

static const rb_method_entry_t *
method_entry_of_iseq(rb_control_frame_t *cfp, rb_iseq_t *iseq)
{
    rb_thread_t *th = GET_THREAD();
    rb_control_frame_t *cfp_limit;

    cfp_limit = (rb_control_frame_t *)(th->stack + th->stack_size);
    while (cfp_limit > cfp) {
	if (cfp->iseq == iseq)
	    return cfp->me;
	cfp = RUBY_VM_PREVIOUS_CONTROL_FRAME(cfp);
    }
    return 0;
}

static ID
frame_func_id(rb_control_frame_t *cfp)
{
    const rb_method_entry_t *me_local;
    rb_iseq_t *iseq = cfp->iseq;
    if (cfp->me) {
	return cfp->me->def->original_id;
    }
    while (iseq) {
	if (RUBY_VM_IFUNC_P(iseq)) {
	    NODE *ifunc = (NODE *)iseq;
	    if (ifunc->nd_aid) return ifunc->nd_aid;
	    return rb_intern("<ifunc>");
	}
	me_local = method_entry_of_iseq(cfp, iseq);
	if (me_local) {
	    cfp->me = me_local;
	    return me_local->def->original_id;
	}
	if (iseq->defined_method_id) {
	    return iseq->defined_method_id;
	}
	if (iseq->local_iseq == iseq) {
	    break;
	}
	iseq = iseq->parent_iseq;
    }
    return 0;
}

ID
rb_frame_this_func(void)
{
    return frame_func_id(GET_THREAD()->cfp);
}

ID
rb_frame_callee(void)
{
    return frame_func_id(GET_THREAD()->cfp);
}

static ID
rb_frame_caller(void)
{
    rb_thread_t *th = GET_THREAD();
    rb_control_frame_t *prev_cfp = RUBY_VM_PREVIOUS_CONTROL_FRAME(th->cfp);
    /* check if prev_cfp can be accessible */
    if ((void *)(th->stack + th->stack_size) == (void *)(prev_cfp)) {
        return 0;
    }
    return frame_func_id(prev_cfp);
}

void
rb_frame_pop(void)
{
    rb_thread_t *th = GET_THREAD();
    th->cfp = RUBY_VM_PREVIOUS_CONTROL_FRAME(th->cfp);
}

/*
 *  call-seq:
 *     append_features(mod)   -> mod
 *
 *  When this module is included in another, Ruby calls
 *  <code>append_features</code> in this module, passing it the
 *  receiving module in _mod_. Ruby's default implementation is
 *  to add the constants, methods, and module variables of this module
 *  to _mod_ if this module has not already been added to
 *  _mod_ or one of its ancestors. See also <code>Module#include</code>.
 */

static VALUE
rb_mod_append_features(VALUE module, VALUE include)
{
    switch (TYPE(include)) {
      case T_CLASS:
      case T_MODULE:
	break;
      default:
	Check_Type(include, T_CLASS);
	break;
    }
    rb_include_module(include, module);

    return module;
}

/*
 *  call-seq:
 *     include(module, ...)    -> self
 *
 *  Invokes <code>Module.append_features</code> on each parameter in reverse order.
 */

static VALUE
rb_mod_include(int argc, VALUE *argv, VALUE module)
{
    int i;

    for (i = 0; i < argc; i++)
	Check_Type(argv[i], T_MODULE);
    while (argc--) {
	rb_funcall(argv[argc], rb_intern("append_features"), 1, module);
	rb_funcall(argv[argc], rb_intern("included"), 1, module);
    }
    return module;
}

/*
 *  call-seq:
 *     mix(module, ...)    -> module
 *
 *  Mix +Module+> into self.
 */

static VALUE
rb_mod_mix_into(int argc, VALUE *argv, VALUE klass)
{
    VALUE module, tmp, constants = Qnil, methods = Qnil;
    st_table *const_tbl = 0, *method_tbl = 0;
    int i = 0;

    rb_check_arity(argc, 1, 3);
    module = argv[i++];

    switch (TYPE(module)) {
      case T_CLASS:
      case T_MODULE:
	break;
      default:
	Check_Type(module, T_CLASS);
	break;
    }
    if (i < argc) {
	constants = argv[i++];
	if (!NIL_P(tmp = rb_check_array_type(constants))) {
	    constants = tmp;
	}
	else if (!NIL_P(methods = rb_check_hash_type(constants))) {
	    constants = Qnil;
	}
	else {
	    Check_Type(constants, T_HASH);
	}
    }
    if (i < argc && NIL_P(methods)) {
	methods = argv[i++];
	if (NIL_P(tmp = rb_check_hash_type(methods))) {
	    Check_Type(methods, T_HASH);
	}
	methods = tmp;
    }
    if (i < argc) rb_raise(rb_eArgError, "wrong arguments");
    if (!NIL_P(constants)) {
	VALUE hash = rb_hash_new();
	for (i = 0; i < RARRAY_LEN(constants); ++i) {
	    rb_hash_update_by(hash, RARRAY_PTR(constants)[i], NULL);
	}
	const_tbl = RHASH_TBL(RB_GC_GUARD(constants) = hash);
    }
    if (!NIL_P(methods)) {
	method_tbl = RHASH_TBL(RB_GC_GUARD(methods));
    }

    rb_mix_module(klass, module, const_tbl, method_tbl);
    return module;
}

void
rb_obj_call_init(VALUE obj, int argc, VALUE *argv)
{
    PASS_PASSED_BLOCK();
    rb_funcall2(obj, idInitialize, argc, argv);
}

void
rb_extend_object(VALUE obj, VALUE module)
{
    rb_include_module(rb_singleton_class(obj), module);
}

/*
 *  call-seq:
 *     extend_object(obj)    -> obj
 *
 *  Extends the specified object by adding this module's constants and
 *  methods (which are added as singleton methods). This is the callback
 *  method used by <code>Object#extend</code>.
 *
 *     module Picky
 *       def Picky.extend_object(o)
 *         if String === o
 *           puts "Can't add Picky to a String"
 *         else
 *           puts "Picky added to #{o.class}"
 *           super
 *         end
 *       end
 *     end
 *     (s = Array.new).extend Picky  # Call Object.extend
 *     (s = "quick brown fox").extend Picky
 *
 *  <em>produces:</em>
 *
 *     Picky added to Array
 *     Can't add Picky to a String
 */

static VALUE
rb_mod_extend_object(VALUE mod, VALUE obj)
{
    rb_extend_object(obj, mod);
    return obj;
}

/*
 *  call-seq:
 *     obj.extend(module, ...)    -> obj
 *
 *  Adds to _obj_ the instance methods from each module given as a
 *  parameter.
 *
 *     module Mod
 *       def hello
 *         "Hello from Mod.\n"
 *       end
 *     end
 *
 *     class Klass
 *       def hello
 *         "Hello from Klass.\n"
 *       end
 *     end
 *
 *     k = Klass.new
 *     k.hello         #=> "Hello from Klass.\n"
 *     k.extend(Mod)   #=> #<Klass:0x401b3bc8>
 *     k.hello         #=> "Hello from Mod.\n"
 */

static VALUE
rb_obj_extend(int argc, VALUE *argv, VALUE obj)
{
    int i;

    rb_check_arity(argc, 1, UNLIMITED_ARGUMENTS);
    for (i = 0; i < argc; i++)
	Check_Type(argv[i], T_MODULE);
    while (argc--) {
	rb_funcall(argv[argc], rb_intern("extend_object"), 1, obj);
	rb_funcall(argv[argc], rb_intern("extended"), 1, obj);
    }
    return obj;
}

/*
 *  call-seq:
 *     include(module, ...)   -> self
 *
 *  Invokes <code>Module.append_features</code>
 *  on each parameter in turn. Effectively adds the methods and constants
 *  in each module to the receiver.
 */

static VALUE
top_include(int argc, VALUE *argv, VALUE self)
{
    rb_thread_t *th = GET_THREAD();

    rb_secure(4);
    if (th->top_wrapper) {
	rb_warning
	    ("main#include in the wrapped load is effective only in wrapper module");
	return rb_mod_include(argc, argv, th->top_wrapper);
    }
    return rb_mod_include(argc, argv, rb_cObject);
}

static VALUE *
errinfo_place(rb_thread_t *th)
{
    rb_control_frame_t *cfp = th->cfp;
    rb_control_frame_t *end_cfp = RUBY_VM_END_CONTROL_FRAME(th);

    while (RUBY_VM_VALID_CONTROL_FRAME_P(cfp, end_cfp)) {
	if (RUBY_VM_NORMAL_ISEQ_P(cfp->iseq)) {
	    if (cfp->iseq->type == ISEQ_TYPE_RESCUE) {
		return &cfp->dfp[-2];
	    }
	    else if (cfp->iseq->type == ISEQ_TYPE_ENSURE &&
		     TYPE(cfp->dfp[-2]) != T_NODE &&
		     !FIXNUM_P(cfp->dfp[-2])) {
		return &cfp->dfp[-2];
	    }
	}
	cfp = RUBY_VM_PREVIOUS_CONTROL_FRAME(cfp);
    }
    return 0;
}

static VALUE
get_thread_errinfo(rb_thread_t *th)
{
    VALUE *ptr = errinfo_place(th);
    if (ptr) {
	return *ptr;
    }
    else {
	return th->errinfo;
    }
}

static VALUE
get_errinfo(void)
{
    return get_thread_errinfo(GET_THREAD());
}

static VALUE
errinfo_getter(ID id)
{
    return get_errinfo();
}

#if 0
static void
errinfo_setter(VALUE val, ID id, VALUE *var)
{
    if (!NIL_P(val) && !rb_obj_is_kind_of(val, rb_eException)) {
	rb_raise(rb_eTypeError, "assigning non-exception to $!");
    }
    else {
	VALUE *ptr = errinfo_place(GET_THREAD());
	if (ptr) {
	    *ptr = val;
	}
	else {
	    rb_raise(rb_eRuntimeError, "errinfo_setter: not in rescue clause.");
	}
    }
}
#endif

VALUE
rb_errinfo(void)
{
    rb_thread_t *th = GET_THREAD();
    return th->errinfo;
}

void
rb_set_errinfo(VALUE err)
{
    if (!NIL_P(err) && !rb_obj_is_kind_of(err, rb_eException)) {
	rb_raise(rb_eTypeError, "assigning non-exception to $!");
    }
    GET_THREAD()->errinfo = err;
}

VALUE
rb_rubylevel_errinfo(void)
{
    return get_errinfo();
}

static VALUE
errat_getter(ID id)
{
    VALUE err = get_errinfo();
    if (!NIL_P(err)) {
	return get_backtrace(err);
    }
    else {
	return Qnil;
    }
}

static void
errat_setter(VALUE val, ID id, VALUE *var)
{
    VALUE err = get_errinfo();
    if (NIL_P(err)) {
	rb_raise(rb_eArgError, "$! not set");
    }
    set_backtrace(err, val);
}

/*
 *  call-seq:
 *     __method__         -> symbol
 *     __callee__         -> symbol
 *
 *  Returns the name of the current method as a Symbol.
 *  If called outside of a method, it returns <code>nil</code>.
 *
 */

static VALUE
rb_f_method_name(void)
{
    ID fname = rb_frame_caller(); /* need *caller* ID */

    if (fname) {
	return ID2SYM(fname);
    }
    else {
	return Qnil;
    }
}

void
Init_eval(void)
{
    rb_define_virtual_variable("$@", errat_getter, errat_setter);
    rb_define_virtual_variable("$!", errinfo_getter, 0);

    rb_define_global_function("raise", rb_f_raise, -1);
    rb_define_global_function("fail", rb_f_raise, -1);

    rb_define_global_function("global_variables", rb_f_global_variables, 0);	/* in variable.c */

    rb_define_global_function("__method__", rb_f_method_name, 0);
    rb_define_global_function("__callee__", rb_f_method_name, 0);

    rb_define_private_method(rb_cModule, "append_features", rb_mod_append_features, 1);
    rb_define_private_method(rb_cModule, "extend_object", rb_mod_extend_object, 1);
    rb_define_private_method(rb_cModule, "include", rb_mod_include, -1);
    rb_define_private_method(rb_cModule, "mix", rb_mod_mix_into, -1);

    rb_undef_method(rb_cClass, "module_function");

    Init_vm_eval();
    Init_eval_method();

    rb_define_singleton_method(rb_cModule, "nesting", rb_mod_nesting, 0);
    rb_define_singleton_method(rb_cModule, "constants", rb_mod_s_constants, -1);

    rb_define_singleton_method(rb_vm_top_self(), "include", top_include, -1);

    rb_define_method(rb_mKernel, "extend", rb_obj_extend, -1);

    rb_define_global_function("trace_var", rb_f_trace_var, -1);	/* in variable.c */
    rb_define_global_function("untrace_var", rb_f_untrace_var, -1);	/* in variable.c */

    exception_error = rb_exc_new3(rb_eFatal,
				  rb_obj_freeze(rb_str_new2("exception reentered")));
    OBJ_TAINT(exception_error);
    OBJ_FREEZE(exception_error);
}
