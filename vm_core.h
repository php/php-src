/**********************************************************************

  vm_core.h -

  $Author$
  created at: 04/01/01 19:41:38 JST

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#ifndef RUBY_VM_CORE_H
#define RUBY_VM_CORE_H

#define RUBY_VM_THREAD_MODEL 2

#include "ruby/ruby.h"
#include "ruby/st.h"

#include "node.h"
#include "debug.h"
#include "vm_opts.h"
#include "id.h"
#include "method.h"
#include "atomic.h"

#if   defined(_WIN32)
#include "thread_win32.h"
#elif defined(HAVE_PTHREAD_H)
#include "thread_pthread.h"
#else
#error "unsupported thread type"
#endif

#ifndef ENABLE_VM_OBJSPACE
#ifdef _WIN32
/*
 * TODO: object space independent st_table.
 * socklist needs st_table in rb_w32_sysinit(), before object space
 * initialization.
 * It is too early now to change st_hash_type, since it breaks binary
 * compatibility.
 */
#define ENABLE_VM_OBJSPACE 0
#else
#define ENABLE_VM_OBJSPACE 1
#endif
#endif

#include <setjmp.h>
#include <signal.h>

#ifndef NSIG
# define NSIG (_SIGMAX + 1)      /* For QNX */
#endif

#define RUBY_NSIG NSIG

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start((a),(b))
#else
#include <varargs.h>
#define va_init_list(a,b) va_start((a))
#endif

#if defined(SIGSEGV) && defined(HAVE_SIGALTSTACK) && defined(SA_SIGINFO) && !defined(__NetBSD__)
#define USE_SIGALTSTACK
#endif

/*****************/
/* configuration */
/*****************/

/* gcc ver. check */
#if defined(__GNUC__) && __GNUC__ >= 2

#if OPT_TOKEN_THREADED_CODE
#if OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif
#endif

#else /* defined(__GNUC__) && __GNUC__ >= 2 */

/* disable threaded code options */
#if OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif
#if OPT_TOKEN_THREADED_CODE
#undef OPT_TOKEN_THREADED_CODE
#endif
#endif

/* call threaded code */
#if    OPT_CALL_THREADED_CODE
#if    OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif /* OPT_DIRECT_THREADED_CODE */
#if    OPT_STACK_CACHING
#undef OPT_STACK_CACHING
#endif /* OPT_STACK_CACHING */
#endif /* OPT_CALL_THREADED_CODE */

/* likely */
#if __GNUC__ >= 3
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else /* __GNUC__ >= 3 */
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif /* __GNUC__ >= 3 */

#if __GNUC__ >= 3
#define UNINITIALIZED_VAR(x) x = x
#else
#define UNINITIALIZED_VAR(x) x
#endif

typedef unsigned long rb_num_t;

/* iseq data type */

struct iseq_compile_data_ensure_node_stack;

typedef struct rb_compile_option_struct rb_compile_option_t;

struct iseq_inline_cache_entry {
    VALUE ic_vmstat;
    VALUE ic_class;
    union {
	VALUE value;
	rb_method_entry_t *method;
	long index;
    } ic_value;
};

#if 1
#define GetCoreDataFromValue(obj, type, ptr) do { \
    (ptr) = (type*)DATA_PTR(obj); \
} while (0)
#else
#define GetCoreDataFromValue(obj, type, ptr) Data_Get_Struct((obj), type, (ptr))
#endif

#define GetISeqPtr(obj, ptr) \
  GetCoreDataFromValue((obj), rb_iseq_t, (ptr))

struct rb_iseq_struct;

struct rb_iseq_struct {
    /***************/
    /* static data */
    /***************/

    enum iseq_type {
	ISEQ_TYPE_TOP,
	ISEQ_TYPE_METHOD,
	ISEQ_TYPE_BLOCK,
	ISEQ_TYPE_CLASS,
	ISEQ_TYPE_RESCUE,
	ISEQ_TYPE_ENSURE,
	ISEQ_TYPE_EVAL,
	ISEQ_TYPE_MAIN,
	ISEQ_TYPE_DEFINED_GUARD
    } type;              /* instruction sequence type */

    VALUE name;	         /* String: iseq name */
    VALUE filename;      /* file information where this sequence from */
    VALUE filepath;      /* real file path or nil */
    VALUE *iseq;         /* iseq (insn number and operands) */
    VALUE *iseq_encoded; /* encoded iseq */
    unsigned long iseq_size;
    VALUE mark_ary;	/* Array: includes operands which should be GC marked */
    VALUE coverage;     /* coverage array */
    unsigned short line_no;

    /* insn info, must be freed */
    struct iseq_line_info_entry *line_info_table;
    size_t line_info_size;

    ID *local_table;		/* must free */
    int local_table_size;

    /* method, class frame: sizeof(vars) + 1, block frame: sizeof(vars) */
    int local_size;

    struct iseq_inline_cache_entry *ic_entries;
    int ic_size;

    /**
     * argument information
     *
     *  def m(a1, a2, ..., aM,                    # mandatory
     *        b1=(...), b2=(...), ..., bN=(...),  # optional
     *        *c,                                 # rest
     *        d1, d2, ..., dO,                    # post
     *        &e)                                 # block
     * =>
     *
     *  argc           = M
     *  arg_rest       = M+N+1 // or -1 if no rest arg
     *  arg_opts       = N+1   // or 0  if no optional arg
     *  arg_opt_table  = [ (arg_opts entries) ]
     *  arg_post_len   = O // 0 if no post arguments
     *  arg_post_start = M+N+2
     *  arg_block      = M+N + 1 + O + 1 // -1 if no block arg
     *  arg_simple     = 0 if not simple arguments.
     *                 = 1 if no opt, rest, post, block.
     *                 = 2 if ambiguous block parameter ({|a|}).
     *  arg_size       = argument size.
     */

    int argc;
    int arg_simple;
    int arg_rest;
    int arg_block;
    int arg_opts;
    int arg_post_len;
    int arg_post_start;
    int arg_size;
    VALUE *arg_opt_table;
    int arg_keyword;
    int arg_keyword_check; /* if this is true, raise an ArgumentError when unknown keyword argument is passed */
    int arg_keywords;
    ID *arg_keyword_table;

    size_t stack_max; /* for stack overflow check */

    /* catch table */
    struct iseq_catch_table_entry *catch_table;
    int catch_table_size;

    /* for child iseq */
    struct rb_iseq_struct *parent_iseq;
    struct rb_iseq_struct *local_iseq;

    /****************/
    /* dynamic data */
    /****************/

    VALUE self;
    VALUE orig;			/* non-NULL if its data have origin */

    /* block inlining */
    /*
     * NODE *node;
     * void *special_block_builder;
     * void *cached_special_block_builder;
     * VALUE cached_special_block;
     */

    /* klass/module nest information stack (cref) */
    NODE *cref_stack;
    VALUE klass;

    /* misc */
    ID defined_method_id;	/* for define_method */

    /* used at compile time */
    struct iseq_compile_data *compile_data;
};

enum ruby_special_exceptions {
    ruby_error_reenter,
    ruby_error_nomemory,
    ruby_error_sysstack,
    ruby_error_closed_stream,
    ruby_special_error_count
};

#define GetVMPtr(obj, ptr) \
  GetCoreDataFromValue((obj), rb_vm_t, (ptr))

#if defined(ENABLE_VM_OBJSPACE) && ENABLE_VM_OBJSPACE
struct rb_objspace;
void rb_objspace_free(struct rb_objspace *);
#endif

typedef struct rb_vm_struct {
    VALUE self;

    rb_global_vm_lock_t gvl;

    struct rb_thread_struct *main_thread;
    struct rb_thread_struct *running_thread;

    st_table *living_threads;
    VALUE thgroup_default;

    int running;
    int inhibit_thread_creation;
    int thread_abort_on_exception;
    unsigned long trace_flag;
    volatile int sleeper;

    /* object management */
    VALUE mark_object_ary;

    VALUE special_exceptions[ruby_special_error_count];

    /* load */
    VALUE top_self;
    VALUE load_path;
    VALUE loaded_features;
    struct st_table *loading_table;

    /* signal */
    struct {
	VALUE cmd;
	int safe;
    } trap_list[RUBY_NSIG];

    /* hook */
    rb_event_hook_t *event_hooks;

    int src_encoding_index;

    VALUE verbose, debug, progname;
    VALUE coverages;

    struct unlinked_method_entry_list_entry *unlinked_method_entry_list;

#if defined(ENABLE_VM_OBJSPACE) && ENABLE_VM_OBJSPACE
    struct rb_objspace *objspace;
#endif

    /*
     * @shyouhei notes that this is not for storing normal Ruby
     * objects so do *NOT* mark this when you GC.
     */
    struct RArray at_exit;
} rb_vm_t;

typedef struct {
    VALUE *pc;			/* cfp[0] */
    VALUE *sp;			/* cfp[1] */
    VALUE *bp;			/* cfp[2] */
    rb_iseq_t *iseq;		/* cfp[3] */
    VALUE flag;			/* cfp[4] */
    VALUE self;			/* cfp[5] / block[0] */
    VALUE *lfp;			/* cfp[6] / block[1] */
    VALUE *dfp;			/* cfp[7] / block[2] */
    rb_iseq_t *block_iseq;	/* cfp[8] / block[3] */
    VALUE proc;			/* cfp[9] / block[4] */
    const rb_method_entry_t *me;/* cfp[10] */
} rb_control_frame_t;

typedef struct rb_block_struct {
    VALUE self;			/* share with method frame if it's only block */
    VALUE *lfp;			/* share with method frame if it's only block */
    VALUE *dfp;			/* share with method frame if it's only block */
    rb_iseq_t *iseq;
    VALUE proc;
} rb_block_t;

extern const rb_data_type_t ruby_threadptr_data_type;

#define GetThreadPtr(obj, ptr) \
    TypedData_Get_Struct((obj), rb_thread_t, &ruby_threadptr_data_type, (ptr))

enum rb_thread_status {
    THREAD_TO_KILL,
    THREAD_RUNNABLE,
    THREAD_STOPPED,
    THREAD_STOPPED_FOREVER,
    THREAD_KILLED
};

typedef RUBY_JMP_BUF rb_jmpbuf_t;

struct rb_vm_tag {
    rb_jmpbuf_t buf;
    VALUE tag;
    VALUE retval;
    struct rb_vm_tag *prev;
};

struct rb_vm_protect_tag {
    struct rb_vm_protect_tag *prev;
};

struct rb_unblock_callback {
    rb_unblock_function_t *func;
    void *arg;
};

struct rb_mutex_struct;

#ifdef MINSIGSTKSZ
#define ALT_STACK_SIZE (MINSIGSTKSZ*2)
#else
#define ALT_STACK_SIZE (4*1024)
#endif

typedef struct rb_thread_struct {
    VALUE self;
    rb_vm_t *vm;

    /* execution information */
    VALUE *stack;		/* must free, must mark */
    unsigned long stack_size;
    rb_control_frame_t *cfp;
    int safe_level;
    int raised_flag;
    VALUE last_status; /* $? */

    /* passing state */
    int state;

    int waiting_fd;

    /* for rb_iterate */
    const rb_block_t *passed_block;

    /* for bmethod */
    const rb_method_entry_t *passed_me;

    /* for load(true) */
    VALUE top_self;
    VALUE top_wrapper;

    /* eval env */
    rb_block_t *base_block;

    VALUE *local_lfp;
    VALUE local_svar;

    /* thread control */
    rb_thread_id_t thread_id;
    enum rb_thread_status status;
    int priority;

    native_thread_data_t native_thread_data;
    void *blocking_region_buffer;

    VALUE thgroup;
    VALUE value;

    VALUE errinfo;
    VALUE thrown_errinfo;

    rb_atomic_t interrupt_flag;
    rb_thread_lock_t interrupt_lock;
    struct rb_unblock_callback unblock;
    VALUE locking_mutex;
    struct rb_mutex_struct *keeping_mutexes;

    struct rb_vm_tag *tag;
    struct rb_vm_protect_tag *protect_tag;

    int parse_in_eval;
    int mild_compile_error;

    /* storage */
    st_table *local_storage;

    struct rb_thread_struct *join_list_next;
    struct rb_thread_struct *join_list_head;

    VALUE first_proc;
    VALUE first_args;
    VALUE (*first_func)(ANYARGS);

    /* for GC */
    VALUE *machine_stack_start;
    VALUE *machine_stack_end;
    size_t machine_stack_maxsize;
#ifdef __ia64
    VALUE *machine_register_stack_start;
    VALUE *machine_register_stack_end;
    size_t machine_register_stack_maxsize;
#endif
    jmp_buf machine_regs;
    int mark_stack_len;

    /* statistics data for profiler */
    VALUE stat_insn_usage;

    /* tracer */
    rb_event_hook_t *event_hooks;
    rb_event_flag_t event_flags;
    int tracing;

    /* fiber */
    VALUE fiber;
    VALUE root_fiber;
    rb_jmpbuf_t root_jmpbuf;

    /* misc */
    int method_missing_reason;
    int abort_on_exception;
#ifdef USE_SIGALTSTACK
    void *altstack;
#endif
    unsigned long running_time_us;
} rb_thread_t;

/* iseq.c */
#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif
VALUE rb_iseq_new(NODE*, VALUE, VALUE, VALUE, VALUE, enum iseq_type);
VALUE rb_iseq_new_top(NODE *node, VALUE name, VALUE filename, VALUE filepath, VALUE parent);
VALUE rb_iseq_new_main(NODE *node, VALUE filename, VALUE filepath);
VALUE rb_iseq_new_with_bopt(NODE*, VALUE, VALUE, VALUE, VALUE, VALUE, enum iseq_type, VALUE);
VALUE rb_iseq_new_with_opt(NODE*, VALUE, VALUE, VALUE, VALUE, VALUE, enum iseq_type, const rb_compile_option_t*);
VALUE rb_iseq_compile(VALUE src, VALUE file, VALUE line);
VALUE rb_iseq_compile_with_option(VALUE src, VALUE file, VALUE filepath, VALUE line, VALUE opt);
VALUE rb_iseq_disasm(VALUE self);
int rb_iseq_disasm_insn(VALUE str, VALUE *iseqval, size_t pos, rb_iseq_t *iseq, VALUE child);
const char *ruby_node_name(int node);
int rb_iseq_first_lineno(const rb_iseq_t *iseq);

RUBY_EXTERN VALUE rb_cISeq;
RUBY_EXTERN VALUE rb_cRubyVM;
RUBY_EXTERN VALUE rb_cEnv;
RUBY_EXTERN VALUE rb_mRubyVMFrozenCore;
#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

/* each thread has this size stack : 128KB */
#define RUBY_VM_THREAD_STACK_SIZE (128 * 1024)

#define GetProcPtr(obj, ptr) \
  GetCoreDataFromValue((obj), rb_proc_t, (ptr))

typedef struct {
    rb_block_t block;

    VALUE envval;		/* for GC mark */
    VALUE blockprocval;
    int safe_level;
    int is_from_method;
    int is_lambda;
} rb_proc_t;

#define GetEnvPtr(obj, ptr) \
  GetCoreDataFromValue((obj), rb_env_t, (ptr))

typedef struct {
    VALUE *env;
    int env_size;
    int local_size;
    VALUE prev_envval;		/* for GC mark */
    rb_block_t block;
} rb_env_t;

#define GetBindingPtr(obj, ptr) \
  GetCoreDataFromValue((obj), rb_binding_t, (ptr))

typedef struct {
    VALUE env;
    VALUE filename;
    unsigned short line_no;
} rb_binding_t;

/* used by compile time and send insn */
#define VM_CALL_ARGS_SPLAT_BIT     (0x01 << 1)
#define VM_CALL_ARGS_BLOCKARG_BIT  (0x01 << 2)
#define VM_CALL_FCALL_BIT          (0x01 << 3)
#define VM_CALL_VCALL_BIT          (0x01 << 4)
#define VM_CALL_TAILCALL_BIT       (0x01 << 5)
#define VM_CALL_TAILRECURSION_BIT  (0x01 << 6)
#define VM_CALL_SUPER_BIT          (0x01 << 7)
#define VM_CALL_OPT_SEND_BIT       (0x01 << 8)

enum vm_special_object_type {
    VM_SPECIAL_OBJECT_VMCORE = 1,
    VM_SPECIAL_OBJECT_CBASE,
    VM_SPECIAL_OBJECT_CONST_BASE
};

#define VM_FRAME_MAGIC_METHOD 0x11
#define VM_FRAME_MAGIC_BLOCK  0x21
#define VM_FRAME_MAGIC_CLASS  0x31
#define VM_FRAME_MAGIC_TOP    0x41
#define VM_FRAME_MAGIC_FINISH 0x51
#define VM_FRAME_MAGIC_CFUNC  0x61
#define VM_FRAME_MAGIC_PROC   0x71
#define VM_FRAME_MAGIC_IFUNC  0x81
#define VM_FRAME_MAGIC_EVAL   0x91
#define VM_FRAME_MAGIC_LAMBDA 0xa1
#define VM_FRAME_MAGIC_MASK_BITS   8
#define VM_FRAME_MAGIC_MASK   (~(~0<<VM_FRAME_MAGIC_MASK_BITS))

#define VM_FRAME_TYPE(cfp) ((cfp)->flag & VM_FRAME_MAGIC_MASK)

/* other frame flag */
#define VM_FRAME_FLAG_PASSED 0x0100

#define RUBYVM_CFUNC_FRAME_P(cfp) \
  (VM_FRAME_TYPE(cfp) == VM_FRAME_MAGIC_CFUNC)

/* inline cache */
typedef struct iseq_inline_cache_entry *IC;

void rb_vm_change_state(void);

typedef VALUE CDHASH;

#ifndef FUNC_FASTCALL
#define FUNC_FASTCALL(x) x
#endif

typedef rb_control_frame_t *
  (FUNC_FASTCALL(*rb_insn_func_t))(rb_thread_t *, rb_control_frame_t *);

#define GC_GUARDED_PTR(p)     ((VALUE)((VALUE)(p) | 0x01))
#define GC_GUARDED_PTR_REF(p) ((void *)(((VALUE)(p)) & ~0x03))
#define GC_GUARDED_PTR_P(p)   (((VALUE)(p)) & 0x01)

#define RUBY_VM_PREVIOUS_CONTROL_FRAME(cfp) ((cfp)+1)
#define RUBY_VM_NEXT_CONTROL_FRAME(cfp) ((cfp)-1)
#define RUBY_VM_END_CONTROL_FRAME(th) \
  ((rb_control_frame_t *)((th)->stack + (th)->stack_size))
#define RUBY_VM_VALID_CONTROL_FRAME_P(cfp, ecfp) \
  ((void *)(ecfp) > (void *)(cfp))
#define RUBY_VM_CONTROL_FRAME_STACK_OVERFLOW_P(th, cfp) \
  (!RUBY_VM_VALID_CONTROL_FRAME_P((cfp), RUBY_VM_END_CONTROL_FRAME(th)))

#define RUBY_VM_IFUNC_P(ptr)        (BUILTIN_TYPE(ptr) == T_NODE)
#define RUBY_VM_NORMAL_ISEQ_P(ptr) \
  ((ptr) && !RUBY_VM_IFUNC_P(ptr))

#define RUBY_VM_GET_BLOCK_PTR_IN_CFP(cfp) ((rb_block_t *)(&(cfp)->self))
#define RUBY_VM_GET_CFP_FROM_BLOCK_PTR(b) \
  ((rb_control_frame_t *)((VALUE *)(b) - 5))

/* VM related object allocate functions */
VALUE rb_thread_alloc(VALUE klass);
VALUE rb_proc_alloc(VALUE klass);

/* for debug */
extern void rb_vmdebug_stack_dump_raw(rb_thread_t *, rb_control_frame_t *);
#define SDR() rb_vmdebug_stack_dump_raw(GET_THREAD(), GET_THREAD()->cfp)
#define SDR2(cfp) rb_vmdebug_stack_dump_raw(GET_THREAD(), (cfp))
void rb_vm_bugreport(void);

/* functions about thread/vm execution */
#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif
VALUE rb_iseq_eval(VALUE iseqval);
VALUE rb_iseq_eval_main(VALUE iseqval);
#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif
int rb_thread_method_id_and_class(rb_thread_t *th, ID *idp, VALUE *klassp);

VALUE rb_vm_invoke_proc(rb_thread_t *th, rb_proc_t *proc, VALUE self,
			int argc, const VALUE *argv, const rb_block_t *blockptr);
VALUE rb_vm_make_proc(rb_thread_t *th, const rb_block_t *block, VALUE klass);
VALUE rb_vm_make_env_object(rb_thread_t *th, rb_control_frame_t *cfp);
void rb_vm_rewrite_dfp_in_errinfo(rb_thread_t *th, rb_control_frame_t *cfp);
void rb_vm_inc_const_missing_count(void);
void rb_vm_gvl_destroy(rb_vm_t *vm);
VALUE rb_vm_call(rb_thread_t *th, VALUE recv, VALUE id, int argc,
                 const VALUE *argv, const rb_method_entry_t *me);
void rb_unlink_method_entry(rb_method_entry_t *me);
void rb_gc_mark_unlinked_live_method_entries(void *pvm);

void rb_thread_start_timer_thread(void);
void rb_thread_stop_timer_thread(int);
void rb_thread_reset_timer_thread(void);
void rb_thread_wakeup_timer_thread(void);

int ruby_thread_has_gvl_p(void);
VALUE rb_make_backtrace(void);
typedef int rb_backtrace_iter_func(void *, VALUE, int, VALUE);
int rb_backtrace_each(rb_backtrace_iter_func *iter, void *arg);
rb_control_frame_t *rb_vm_get_ruby_level_next_cfp(rb_thread_t *th, rb_control_frame_t *cfp);
int rb_vm_get_sourceline(const rb_control_frame_t *);
VALUE rb_name_err_mesg_new(VALUE obj, VALUE mesg, VALUE recv, VALUE method);
void rb_vm_stack_to_heap(rb_thread_t *th);
void ruby_thread_init_stack(rb_thread_t *th);

NOINLINE(void rb_gc_save_machine_context(rb_thread_t *));
void rb_gc_mark_machine_stack(rb_thread_t *th);

int rb_autoloading_value(VALUE mod, ID id, VALUE* value);

#define sysstack_error GET_VM()->special_exceptions[ruby_error_sysstack]

/* for thread */

#if RUBY_VM_THREAD_MODEL == 2
extern rb_thread_t *ruby_current_thread;
extern rb_vm_t *ruby_current_vm;

#define GET_VM() ruby_current_vm
#define GET_THREAD() ruby_current_thread
#define rb_thread_set_current_raw(th) (void)(ruby_current_thread = (th))
#define rb_thread_set_current(th) do { \
    if ((th)->vm->running_thread != (th)) { \
	(th)->vm->running_thread->running_time_us = 0; \
    } \
    rb_thread_set_current_raw(th); \
    (th)->vm->running_thread = (th); \
} while (0)

#else
#error "unsupported thread model"
#endif

#define RUBY_VM_SET_TIMER_INTERRUPT(th)		ATOMIC_OR((th)->interrupt_flag, 0x01)
#define RUBY_VM_SET_INTERRUPT(th)		ATOMIC_OR((th)->interrupt_flag, 0x02)
#define RUBY_VM_SET_FINALIZER_INTERRUPT(th)	ATOMIC_OR((th)->interrupt_flag, 0x04)
#define RUBY_VM_INTERRUPTED(th) ((th)->interrupt_flag & 0x02)

int rb_signal_buff_size(void);
void rb_signal_exec(rb_thread_t *th, int sig);
void rb_threadptr_check_signal(rb_thread_t *mth);
void rb_threadptr_signal_raise(rb_thread_t *th, int sig);
void rb_threadptr_signal_exit(rb_thread_t *th);
void rb_threadptr_execute_interrupts(rb_thread_t *);
void rb_threadptr_interrupt(rb_thread_t *th);
void rb_threadptr_unlock_all_locking_mutexes(rb_thread_t *th);

void rb_thread_lock_unlock(rb_thread_lock_t *);
void rb_thread_lock_destroy(rb_thread_lock_t *);

#define RUBY_VM_CHECK_INTS_TH(th) do { \
    if (UNLIKELY((th)->interrupt_flag)) { \
	rb_threadptr_execute_interrupts(th); \
    } \
} while (0)

#define RUBY_VM_CHECK_INTS() \
  RUBY_VM_CHECK_INTS_TH(GET_THREAD())

/* tracer */
void
rb_threadptr_exec_event_hooks(rb_thread_t *th, rb_event_flag_t flag, VALUE self, ID id, VALUE klass);

#define EXEC_EVENT_HOOK(th, flag, self, id, klass) do { \
    rb_event_flag_t wait_event__ = (th)->event_flags; \
    if (UNLIKELY(wait_event__)) { \
	if (wait_event__ & ((flag) | RUBY_EVENT_VM)) { \
	    rb_threadptr_exec_event_hooks((th), (flag), (self), (id), (klass)); \
	} \
    } \
} while (0)

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

int rb_thread_check_trap_pending(void);

extern VALUE rb_get_coverages(void);
extern void rb_set_coverages(VALUE);
extern void rb_reset_coverages(void);

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* RUBY_VM_CORE_H */
