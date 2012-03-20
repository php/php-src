/**********************************************************************

  thread.c -

  $Author$

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

/*
  YARV Thread Design

  model 1: Userlevel Thread
    Same as traditional ruby thread.

  model 2: Native Thread with Global VM lock
    Using pthread (or Windows thread) and Ruby threads run concurrent.

  model 3: Native Thread with fine grain lock
    Using pthread and Ruby threads run concurrent or parallel.

------------------------------------------------------------------------

  model 2:
    A thread has mutex (GVL: Global VM Lock or Giant VM Lock) can run.
    When thread scheduling, running thread release GVL.  If running thread
    try blocking operation, this thread must release GVL and another
    thread can continue this flow.  After blocking operation, thread
    must check interrupt (RUBY_VM_CHECK_INTS).

    Every VM can run parallel.

    Ruby threads are scheduled by OS thread scheduler.

------------------------------------------------------------------------

  model 3:
    Every threads run concurrent or parallel and to access shared object
    exclusive access control is needed.  For example, to access String
    object or Array object, fine grain lock must be locked every time.
 */


/* for model 2 */

#include "eval_intern.h"
#include "gc.h"
#include "internal.h"
#include "ruby/io.h"

#ifndef USE_NATIVE_THREAD_PRIORITY
#define USE_NATIVE_THREAD_PRIORITY 0
#define RUBY_THREAD_PRIORITY_MAX 3
#define RUBY_THREAD_PRIORITY_MIN -3
#endif

#ifndef THREAD_DEBUG
#define THREAD_DEBUG 0
#endif

VALUE rb_cMutex;
VALUE rb_cBarrier;

static void sleep_timeval(rb_thread_t *th, struct timeval time);
static void sleep_wait_for_interrupt(rb_thread_t *th, double sleepsec);
static void sleep_forever(rb_thread_t *th, int nodeadlock);
static double timeofday(void);
static int rb_threadptr_dead(rb_thread_t *th);

static void rb_check_deadlock(rb_vm_t *vm);

#define eKillSignal INT2FIX(0)
#define eTerminateSignal INT2FIX(1)
static volatile int system_working = 1;

#define closed_stream_error GET_VM()->special_exceptions[ruby_error_closed_stream]

inline static void
st_delete_wrap(st_table *table, st_data_t key)
{
    st_delete(table, &key, 0);
}

/********************************************************************************/

#define THREAD_SYSTEM_DEPENDENT_IMPLEMENTATION

struct rb_blocking_region_buffer {
    enum rb_thread_status prev_status;
    struct rb_unblock_callback oldubf;
};

static void set_unblock_function(rb_thread_t *th, rb_unblock_function_t *func, void *arg,
				 struct rb_unblock_callback *old);
static void reset_unblock_function(rb_thread_t *th, const struct rb_unblock_callback *old);

static inline void blocking_region_end(rb_thread_t *th, struct rb_blocking_region_buffer *region);

#define RB_GC_SAVE_MACHINE_CONTEXT(th) \
  do { \
    rb_gc_save_machine_context(th); \
    SET_MACHINE_STACK_END(&(th)->machine_stack_end); \
  } while (0)

#define GVL_UNLOCK_BEGIN() do { \
  rb_thread_t *_th_stored = GET_THREAD(); \
  RB_GC_SAVE_MACHINE_CONTEXT(_th_stored); \
  gvl_release(_th_stored->vm);

#define GVL_UNLOCK_END() \
  gvl_acquire(_th_stored->vm, _th_stored); \
  rb_thread_set_current(_th_stored); \
} while(0)

#define blocking_region_begin(th, region, func, arg) \
  do { \
    (region)->prev_status = (th)->status; \
    set_unblock_function((th), (func), (arg), &(region)->oldubf); \
    (th)->blocking_region_buffer = (region); \
    (th)->status = THREAD_STOPPED; \
    thread_debug("enter blocking region (%p)\n", (void *)(th)); \
    RB_GC_SAVE_MACHINE_CONTEXT(th); \
    gvl_release((th)->vm); \
  } while (0)

#define BLOCKING_REGION(exec, ubf, ubfarg) do { \
    rb_thread_t *__th = GET_THREAD(); \
    struct rb_blocking_region_buffer __region; \
    blocking_region_begin(__th, &__region, (ubf), (ubfarg)); \
    exec; \
    blocking_region_end(__th, &__region); \
    RUBY_VM_CHECK_INTS(); \
} while(0)

#if THREAD_DEBUG
#ifdef HAVE_VA_ARGS_MACRO
void rb_thread_debug(const char *file, int line, const char *fmt, ...);
#define thread_debug(fmt, ...) rb_thread_debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define POSITION_FORMAT "%s:%d:"
#define POSITION_ARGS ,file, line
#else
void rb_thread_debug(const char *fmt, ...);
#define thread_debug rb_thread_debug
#define POSITION_FORMAT
#define POSITION_ARGS
#endif

# if THREAD_DEBUG < 0
static int rb_thread_debug_enabled;

/*
 *  call-seq:
 *     Thread.DEBUG     -> num
 *
 *  Returns the thread debug level.  Available only if compiled with
 *  THREAD_DEBUG=-1.
 */

static VALUE
rb_thread_s_debug(void)
{
    return INT2NUM(rb_thread_debug_enabled);
}

/*
 *  call-seq:
 *     Thread.DEBUG = num
 *
 *  Sets the thread debug level.  Available only if compiled with
 *  THREAD_DEBUG=-1.
 */

static VALUE
rb_thread_s_debug_set(VALUE self, VALUE val)
{
    rb_thread_debug_enabled = RTEST(val) ? NUM2INT(val) : 0;
    return val;
}
# else
# define rb_thread_debug_enabled THREAD_DEBUG
# endif
#else
#define thread_debug if(0)printf
#endif

#ifndef __ia64
#define thread_start_func_2(th, st, rst) thread_start_func_2(th, st)
#endif
NOINLINE(static int thread_start_func_2(rb_thread_t *th, VALUE *stack_start,
					VALUE *register_stack_start));
static void timer_thread_function(void *);

#if   defined(_WIN32)
#include "thread_win32.c"

#define DEBUG_OUT() \
  WaitForSingleObject(&debug_mutex, INFINITE); \
  printf(POSITION_FORMAT"%p - %s" POSITION_ARGS, GetCurrentThreadId(), buf); \
  fflush(stdout); \
  ReleaseMutex(&debug_mutex);

#elif defined(HAVE_PTHREAD_H)
#include "thread_pthread.c"

#define DEBUG_OUT() \
  pthread_mutex_lock(&debug_mutex); \
  printf(POSITION_FORMAT"%#"PRIxVALUE" - %s" POSITION_ARGS, (VALUE)pthread_self(), buf); \
  fflush(stdout); \
  pthread_mutex_unlock(&debug_mutex);

#else
#error "unsupported thread type"
#endif

#if THREAD_DEBUG
static int debug_mutex_initialized = 1;
static rb_thread_lock_t debug_mutex;

void
rb_thread_debug(
#ifdef HAVE_VA_ARGS_MACRO
    const char *file, int line,
#endif
    const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZ];

    if (!rb_thread_debug_enabled) return;

    if (debug_mutex_initialized == 1) {
	debug_mutex_initialized = 0;
	native_mutex_initialize(&debug_mutex);
    }

    va_start(args, fmt);
    vsnprintf(buf, BUFSIZ, fmt, args);
    va_end(args);

    DEBUG_OUT();
}
#endif

void
rb_vm_gvl_destroy(rb_vm_t *vm)
{
    gvl_release(vm);
    gvl_destroy(vm);
}

void
rb_thread_lock_unlock(rb_thread_lock_t *lock)
{
    native_mutex_unlock(lock);
}

void
rb_thread_lock_destroy(rb_thread_lock_t *lock)
{
    native_mutex_destroy(lock);
}

static void
set_unblock_function(rb_thread_t *th, rb_unblock_function_t *func, void *arg,
		     struct rb_unblock_callback *old)
{
  check_ints:
    RUBY_VM_CHECK_INTS(); /* check signal or so */
    native_mutex_lock(&th->interrupt_lock);
    if (th->interrupt_flag) {
	native_mutex_unlock(&th->interrupt_lock);
	goto check_ints;
    }
    else {
	if (old) *old = th->unblock;
	th->unblock.func = func;
	th->unblock.arg = arg;
    }
    native_mutex_unlock(&th->interrupt_lock);
}

static void
reset_unblock_function(rb_thread_t *th, const struct rb_unblock_callback *old)
{
    native_mutex_lock(&th->interrupt_lock);
    th->unblock = *old;
    native_mutex_unlock(&th->interrupt_lock);
}

void
rb_threadptr_interrupt(rb_thread_t *th)
{
    native_mutex_lock(&th->interrupt_lock);
    RUBY_VM_SET_INTERRUPT(th);
    if (th->unblock.func) {
	(th->unblock.func)(th->unblock.arg);
    }
    else {
	/* none */
    }
    native_mutex_unlock(&th->interrupt_lock);
}


static int
terminate_i(st_data_t key, st_data_t val, rb_thread_t *main_thread)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    if (th != main_thread) {
	thread_debug("terminate_i: %p\n", (void *)th);
	rb_threadptr_interrupt(th);
	th->thrown_errinfo = eTerminateSignal;
	th->status = THREAD_TO_KILL;
    }
    else {
	thread_debug("terminate_i: main thread (%p)\n", (void *)th);
    }
    return ST_CONTINUE;
}

typedef struct rb_mutex_struct
{
    rb_thread_lock_t lock;
    rb_thread_cond_t cond;
    struct rb_thread_struct volatile *th;
    int cond_waiting;
    struct rb_mutex_struct *next_mutex;
} rb_mutex_t;

static void rb_mutex_abandon_all(rb_mutex_t *mutexes);
static const char* rb_mutex_unlock_th(rb_mutex_t *mutex, rb_thread_t volatile *th);

void
rb_threadptr_unlock_all_locking_mutexes(rb_thread_t *th)
{
    const char *err;
    rb_mutex_t *mutex;
    rb_mutex_t *mutexes = th->keeping_mutexes;

    while (mutexes) {
	mutex = mutexes;
	/* rb_warn("mutex #<%p> remains to be locked by terminated thread",
		mutexes); */
	mutexes = mutex->next_mutex;
	err = rb_mutex_unlock_th(mutex, th);
	if (err) rb_bug("invalid keeping_mutexes: %s", err);
    }
}

void
rb_thread_terminate_all(void)
{
    rb_thread_t *th = GET_THREAD(); /* main thread */
    rb_vm_t *vm = th->vm;

    if (vm->main_thread != th) {
	rb_bug("rb_thread_terminate_all: called by child thread (%p, %p)",
	       (void *)vm->main_thread, (void *)th);
    }

    /* unlock all locking mutexes */
    rb_threadptr_unlock_all_locking_mutexes(th);

    thread_debug("rb_thread_terminate_all (main thread: %p)\n", (void *)th);
    st_foreach(vm->living_threads, terminate_i, (st_data_t)th);
    vm->inhibit_thread_creation = 1;

    while (!rb_thread_alone()) {
	PUSH_TAG();
	if (EXEC_TAG() == 0) {
	    rb_thread_schedule();
	}
	else {
	    /* ignore exception */
	}
	POP_TAG();
    }
}

static void
thread_cleanup_func_before_exec(void *th_ptr)
{
    rb_thread_t *th = th_ptr;
    th->status = THREAD_KILLED;
    th->machine_stack_start = th->machine_stack_end = 0;
#ifdef __ia64
    th->machine_register_stack_start = th->machine_register_stack_end = 0;
#endif
}

static void
thread_cleanup_func(void *th_ptr, int atfork)
{
    rb_thread_t *th = th_ptr;

    th->locking_mutex = Qfalse;
    thread_cleanup_func_before_exec(th_ptr);

    /*
     * Unfortunately, we can't release native threading resource at fork
     * because libc may have unstable locking state therefore touching
     * a threading resource may cause a deadlock.
     */
    if (atfork)
	return;

    native_mutex_destroy(&th->interrupt_lock);
    native_thread_destroy(th);
}

static VALUE rb_threadptr_raise(rb_thread_t *, int, VALUE *);

void
ruby_thread_init_stack(rb_thread_t *th)
{
    native_thread_init_stack(th);
}

static int
thread_start_func_2(rb_thread_t *th, VALUE *stack_start, VALUE *register_stack_start)
{
    int state;
    VALUE args = th->first_args;
    rb_proc_t *proc;
    rb_thread_t *join_th;
    rb_thread_t *main_th;
    VALUE errinfo = Qnil;
# ifdef USE_SIGALTSTACK
    void rb_register_sigaltstack(rb_thread_t *th);

    rb_register_sigaltstack(th);
# endif

    ruby_thread_set_native(th);

    th->machine_stack_start = stack_start;
#ifdef __ia64
    th->machine_register_stack_start = register_stack_start;
#endif
    thread_debug("thread start: %p\n", (void *)th);

    gvl_acquire(th->vm, th);
    {
	thread_debug("thread start (get lock): %p\n", (void *)th);
	rb_thread_set_current(th);

	TH_PUSH_TAG(th);
	if ((state = EXEC_TAG()) == 0) {
	    SAVE_ROOT_JMPBUF(th, {
		if (!th->first_func) {
		    GetProcPtr(th->first_proc, proc);
		    th->errinfo = Qnil;
		    th->local_lfp = proc->block.lfp;
		    th->local_svar = Qnil;
		    th->value = rb_vm_invoke_proc(th, proc, proc->block.self,
						  (int)RARRAY_LEN(args), RARRAY_PTR(args), 0);
		}
		else {
		    th->value = (*th->first_func)((void *)args);
		}
	    });
	}
	else {
	    errinfo = th->errinfo;
	    if (NIL_P(errinfo)) errinfo = rb_errinfo();
	    if (state == TAG_FATAL) {
		/* fatal error within this thread, need to stop whole script */
	    }
	    else if (rb_obj_is_kind_of(errinfo, rb_eSystemExit)) {
		if (th->safe_level >= 4) {
		    th->errinfo = rb_exc_new3(rb_eSecurityError,
					      rb_sprintf("Insecure exit at level %d", th->safe_level));
		    errinfo = Qnil;
		}
	    }
	    else if (th->safe_level < 4 &&
		     (th->vm->thread_abort_on_exception ||
		      th->abort_on_exception || RTEST(ruby_debug))) {
		/* exit on main_thread */
	    }
	    else {
		errinfo = Qnil;
	    }
	    th->value = Qnil;
	}

	th->status = THREAD_KILLED;
	thread_debug("thread end: %p\n", (void *)th);

	main_th = th->vm->main_thread;
	if (th != main_th) {
	    if (RB_TYPE_P(errinfo, T_OBJECT)) {
		/* treat with normal error object */
		rb_threadptr_raise(main_th, 1, &errinfo);
	    }
	}
	TH_POP_TAG();

	/* locking_mutex must be Qfalse */
	if (th->locking_mutex != Qfalse) {
	    rb_bug("thread_start_func_2: locking_mutex must not be set (%p:%"PRIxVALUE")",
		   (void *)th, th->locking_mutex);
	}

	/* delete self other than main thread from living_threads */
	if (th != main_th) {
	    st_delete_wrap(th->vm->living_threads, th->self);
	}

	/* wake up joining threads */
	join_th = th->join_list_head;
	while (join_th) {
	    if (join_th == main_th) errinfo = Qnil;
	    rb_threadptr_interrupt(join_th);
	    switch (join_th->status) {
	      case THREAD_STOPPED: case THREAD_STOPPED_FOREVER:
		join_th->status = THREAD_RUNNABLE;
	      default: break;
	    }
	    join_th = join_th->join_list_next;
	}

	rb_threadptr_unlock_all_locking_mutexes(th);
	if (th != main_th) rb_check_deadlock(th->vm);

	if (!th->root_fiber) {
	    rb_thread_recycle_stack_release(th->stack);
	    th->stack = 0;
	}
    }
    if (th->vm->main_thread == th) {
	ruby_cleanup(state);
    }
    else {
	thread_cleanup_func(th, FALSE);
	gvl_release(th->vm);
    }

    return 0;
}

static VALUE
thread_create_core(VALUE thval, VALUE args, VALUE (*fn)(ANYARGS))
{
    rb_thread_t *th;
    int err;

    if (OBJ_FROZEN(GET_THREAD()->thgroup)) {
	rb_raise(rb_eThreadError,
		 "can't start a new thread (frozen ThreadGroup)");
    }
    GetThreadPtr(thval, th);

    /* setup thread environment */
    th->first_func = fn;
    th->first_proc = fn ? Qfalse : rb_block_proc();
    th->first_args = args; /* GC: shouldn't put before above line */

    th->priority = GET_THREAD()->priority;
    th->thgroup = GET_THREAD()->thgroup;

    native_mutex_initialize(&th->interrupt_lock);
    if (GET_VM()->event_hooks != NULL)
	th->event_flags |= RUBY_EVENT_VM;

    /* kick thread */
    st_insert(th->vm->living_threads, thval, (st_data_t) th->thread_id);
    err = native_thread_create(th);
    if (err) {
	st_delete_wrap(th->vm->living_threads, th->self);
	th->status = THREAD_KILLED;
	rb_raise(rb_eThreadError, "can't create Thread (%d)", err);
    }
    return thval;
}

/* :nodoc: */
static VALUE
thread_s_new(int argc, VALUE *argv, VALUE klass)
{
    rb_thread_t *th;
    VALUE thread = rb_thread_alloc(klass);

    if (GET_VM()->inhibit_thread_creation)
	rb_raise(rb_eThreadError, "can't alloc thread");

    rb_obj_call_init(thread, argc, argv);
    GetThreadPtr(thread, th);
    if (!th->first_args) {
	rb_raise(rb_eThreadError, "uninitialized thread - check `%s#initialize'",
		 rb_class2name(klass));
    }
    return thread;
}

/*
 *  call-seq:
 *     Thread.start([args]*) {|args| block }   -> thread
 *     Thread.fork([args]*) {|args| block }    -> thread
 *
 *  Basically the same as <code>Thread::new</code>. However, if class
 *  <code>Thread</code> is subclassed, then calling <code>start</code> in that
 *  subclass will not invoke the subclass's <code>initialize</code> method.
 */

static VALUE
thread_start(VALUE klass, VALUE args)
{
    return thread_create_core(rb_thread_alloc(klass), args, 0);
}

/* :nodoc: */
static VALUE
thread_initialize(VALUE thread, VALUE args)
{
    rb_thread_t *th;
    if (!rb_block_given_p()) {
	rb_raise(rb_eThreadError, "must be called with a block");
    }
    GetThreadPtr(thread, th);
    if (th->first_args) {
	VALUE proc = th->first_proc, line, loc;
	const char *file;
        if (!proc || !RTEST(loc = rb_proc_location(proc))) {
            rb_raise(rb_eThreadError, "already initialized thread");
        }
	file = RSTRING_PTR(RARRAY_PTR(loc)[0]);
	if (NIL_P(line = RARRAY_PTR(loc)[1])) {
	    rb_raise(rb_eThreadError, "already initialized thread - %s",
		     file);
	}
        rb_raise(rb_eThreadError, "already initialized thread - %s:%d",
                 file, NUM2INT(line));
    }
    return thread_create_core(thread, args, 0);
}

VALUE
rb_thread_create(VALUE (*fn)(ANYARGS), void *arg)
{
    return thread_create_core(rb_thread_alloc(rb_cThread), (VALUE)arg, fn);
}


/* +infty, for this purpose */
#define DELAY_INFTY 1E30

struct join_arg {
    rb_thread_t *target, *waiting;
    double limit;
    int forever;
};

static VALUE
remove_from_join_list(VALUE arg)
{
    struct join_arg *p = (struct join_arg *)arg;
    rb_thread_t *target_th = p->target, *th = p->waiting;

    if (target_th->status != THREAD_KILLED) {
	rb_thread_t **pth = &target_th->join_list_head;

	while (*pth) {
	    if (*pth == th) {
		*pth = th->join_list_next;
		break;
	    }
	    pth = &(*pth)->join_list_next;
	}
    }

    return Qnil;
}

static VALUE
thread_join_sleep(VALUE arg)
{
    struct join_arg *p = (struct join_arg *)arg;
    rb_thread_t *target_th = p->target, *th = p->waiting;
    double now, limit = p->limit;

    while (target_th->status != THREAD_KILLED) {
	if (p->forever) {
	    sleep_forever(th, 1);
	}
	else {
	    now = timeofday();
	    if (now > limit) {
		thread_debug("thread_join: timeout (thid: %p)\n",
			     (void *)target_th->thread_id);
		return Qfalse;
	    }
	    sleep_wait_for_interrupt(th, limit - now);
	}
	thread_debug("thread_join: interrupted (thid: %p)\n",
		     (void *)target_th->thread_id);
    }
    return Qtrue;
}

static VALUE
thread_join(rb_thread_t *target_th, double delay)
{
    rb_thread_t *th = GET_THREAD();
    struct join_arg arg;

    arg.target = target_th;
    arg.waiting = th;
    arg.limit = timeofday() + delay;
    arg.forever = delay == DELAY_INFTY;

    thread_debug("thread_join (thid: %p)\n", (void *)target_th->thread_id);

    if (target_th->status != THREAD_KILLED) {
	th->join_list_next = target_th->join_list_head;
	target_th->join_list_head = th;
	if (!rb_ensure(thread_join_sleep, (VALUE)&arg,
		       remove_from_join_list, (VALUE)&arg)) {
	    return Qnil;
	}
    }

    thread_debug("thread_join: success (thid: %p)\n",
		 (void *)target_th->thread_id);

    if (target_th->errinfo != Qnil) {
	VALUE err = target_th->errinfo;

	if (FIXNUM_P(err)) {
	    /* */
	}
	else if (TYPE(target_th->errinfo) == T_NODE) {
	    rb_exc_raise(rb_vm_make_jump_tag_but_local_jump(
		GET_THROWOBJ_STATE(err), GET_THROWOBJ_VAL(err)));
	}
	else {
	    /* normal exception */
	    rb_exc_raise(err);
	}
    }
    return target_th->self;
}

/*
 *  call-seq:
 *     thr.join          -> thr
 *     thr.join(limit)   -> thr
 *
 *  The calling thread will suspend execution and run <i>thr</i>. Does not
 *  return until <i>thr</i> exits or until <i>limit</i> seconds have passed. If
 *  the time limit expires, <code>nil</code> will be returned, otherwise
 *  <i>thr</i> is returned.
 *
 *  Any threads not joined will be killed when the main program exits.  If
 *  <i>thr</i> had previously raised an exception and the
 *  <code>abort_on_exception</code> and <code>$DEBUG</code> flags are not set
 *  (so the exception has not yet been processed) it will be processed at this
 *  time.
 *
 *     a = Thread.new { print "a"; sleep(10); print "b"; print "c" }
 *     x = Thread.new { print "x"; Thread.pass; print "y"; print "z" }
 *     x.join # Let x thread finish, a will be killed on exit.
 *
 *  <em>produces:</em>
 *
 *     axyz
 *
 *  The following example illustrates the <i>limit</i> parameter.
 *
 *     y = Thread.new { 4.times { sleep 0.1; puts 'tick... ' }}
 *     puts "Waiting" until y.join(0.15)
 *
 *  <em>produces:</em>
 *
 *     tick...
 *     Waiting
 *     tick...
 *     Waitingtick...
 *
 *
 *     tick...
 */

static VALUE
thread_join_m(int argc, VALUE *argv, VALUE self)
{
    rb_thread_t *target_th;
    double delay = DELAY_INFTY;
    VALUE limit;

    GetThreadPtr(self, target_th);

    rb_scan_args(argc, argv, "01", &limit);
    if (!NIL_P(limit)) {
	delay = rb_num2dbl(limit);
    }

    return thread_join(target_th, delay);
}

/*
 *  call-seq:
 *     thr.value   -> obj
 *
 *  Waits for <i>thr</i> to complete (via <code>Thread#join</code>) and returns
 *  its value.
 *
 *     a = Thread.new { 2 + 2 }
 *     a.value   #=> 4
 */

static VALUE
thread_value(VALUE self)
{
    rb_thread_t *th;
    GetThreadPtr(self, th);
    thread_join(th, DELAY_INFTY);
    return th->value;
}

/*
 * Thread Scheduling
 */

static struct timeval
double2timeval(double d)
{
    struct timeval time;

    time.tv_sec = (int)d;
    time.tv_usec = (int)((d - (int)d) * 1e6);
    if (time.tv_usec < 0) {
	time.tv_usec += (int)1e6;
	time.tv_sec -= 1;
    }
    return time;
}

static void
sleep_forever(rb_thread_t *th, int deadlockable)
{
    enum rb_thread_status prev_status = th->status;
    enum rb_thread_status status = deadlockable ? THREAD_STOPPED_FOREVER : THREAD_STOPPED;

    th->status = status;
    do {
	if (deadlockable) {
	    th->vm->sleeper++;
	    rb_check_deadlock(th->vm);
	}
	native_sleep(th, 0);
	if (deadlockable) {
	    th->vm->sleeper--;
	}
	RUBY_VM_CHECK_INTS();
    } while (th->status == status);
    th->status = prev_status;
}

static void
getclockofday(struct timeval *tp)
{
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
	tp->tv_sec = ts.tv_sec;
	tp->tv_usec = ts.tv_nsec / 1000;
    } else
#endif
    {
        gettimeofday(tp, NULL);
    }
}

static void
sleep_timeval(rb_thread_t *th, struct timeval tv)
{
    struct timeval to, tvn;
    enum rb_thread_status prev_status = th->status;

    getclockofday(&to);
    to.tv_sec += tv.tv_sec;
    if ((to.tv_usec += tv.tv_usec) >= 1000000) {
	to.tv_sec++;
	to.tv_usec -= 1000000;
    }

    th->status = THREAD_STOPPED;
    do {
	native_sleep(th, &tv);
	RUBY_VM_CHECK_INTS();
	getclockofday(&tvn);
	if (to.tv_sec < tvn.tv_sec) break;
	if (to.tv_sec == tvn.tv_sec && to.tv_usec <= tvn.tv_usec) break;
	thread_debug("sleep_timeval: %ld.%.6ld > %ld.%.6ld\n",
		     (long)to.tv_sec, (long)to.tv_usec,
		     (long)tvn.tv_sec, (long)tvn.tv_usec);
	tv.tv_sec = to.tv_sec - tvn.tv_sec;
	if ((tv.tv_usec = to.tv_usec - tvn.tv_usec) < 0) {
	    --tv.tv_sec;
	    tv.tv_usec += 1000000;
	}
    } while (th->status == THREAD_STOPPED);
    th->status = prev_status;
}

void
rb_thread_sleep_forever(void)
{
    thread_debug("rb_thread_sleep_forever\n");
    sleep_forever(GET_THREAD(), 0);
}

static void
rb_thread_sleep_deadly(void)
{
    thread_debug("rb_thread_sleep_deadly\n");
    sleep_forever(GET_THREAD(), 1);
}

static double
timeofday(void)
{
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
        return (double)tp.tv_sec + (double)tp.tv_nsec * 1e-9;
    } else
#endif
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
    }
}

static void
sleep_wait_for_interrupt(rb_thread_t *th, double sleepsec)
{
    sleep_timeval(th, double2timeval(sleepsec));
}

static void
sleep_for_polling(rb_thread_t *th)
{
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 100 * 1000;	/* 0.1 sec */
    sleep_timeval(th, time);
}

void
rb_thread_wait_for(struct timeval time)
{
    rb_thread_t *th = GET_THREAD();
    sleep_timeval(th, time);
}

void
rb_thread_polling(void)
{
    RUBY_VM_CHECK_INTS();
    if (!rb_thread_alone()) {
	rb_thread_t *th = GET_THREAD();
	sleep_for_polling(th);
    }
}

/*
 * CAUTION: This function causes thread switching.
 *          rb_thread_check_ints() check ruby's interrupts.
 *          some interrupt needs thread switching/invoke handlers,
 *          and so on.
 */

void
rb_thread_check_ints(void)
{
    RUBY_VM_CHECK_INTS();
}

/*
 * Hidden API for tcl/tk wrapper.
 * There is no guarantee to perpetuate it.
 */
int
rb_thread_check_trap_pending(void)
{
    return rb_signal_buff_size() != 0;
}

/* This function can be called in blocking region. */
int
rb_thread_interrupted(VALUE thval)
{
    rb_thread_t *th;
    GetThreadPtr(thval, th);
    return RUBY_VM_INTERRUPTED(th);
}

void
rb_thread_sleep(int sec)
{
    rb_thread_wait_for(rb_time_timeval(INT2FIX(sec)));
}

static void rb_threadptr_execute_interrupts_common(rb_thread_t *);

static void
rb_thread_schedule_limits(unsigned long limits_us)
{
    thread_debug("rb_thread_schedule\n");
    if (!rb_thread_alone()) {
	rb_thread_t *th = GET_THREAD();

	if (th->running_time_us >= limits_us) {
	    thread_debug("rb_thread_schedule/switch start\n");
	    RB_GC_SAVE_MACHINE_CONTEXT(th);
	    gvl_yield(th->vm, th);
	    rb_thread_set_current(th);
	    thread_debug("rb_thread_schedule/switch done\n");
	}
    }
}

void
rb_thread_schedule(void)
{
    rb_thread_schedule_limits(0);

    if (UNLIKELY(GET_THREAD()->interrupt_flag)) {
	rb_threadptr_execute_interrupts_common(GET_THREAD());
    }
}

/* blocking region */

static inline void
blocking_region_end(rb_thread_t *th, struct rb_blocking_region_buffer *region)
{
    gvl_acquire(th->vm, th);
    rb_thread_set_current(th);
    thread_debug("leave blocking region (%p)\n", (void *)th);
    remove_signal_thread_list(th);
    th->blocking_region_buffer = 0;
    reset_unblock_function(th, &region->oldubf);
    if (th->status == THREAD_STOPPED) {
	th->status = region->prev_status;
    }
}

struct rb_blocking_region_buffer *
rb_thread_blocking_region_begin(void)
{
    rb_thread_t *th = GET_THREAD();
    struct rb_blocking_region_buffer *region = ALLOC(struct rb_blocking_region_buffer);
    blocking_region_begin(th, region, ubf_select, th);
    return region;
}

void
rb_thread_blocking_region_end(struct rb_blocking_region_buffer *region)
{
    int saved_errno = errno;
    rb_thread_t *th = GET_THREAD();
    blocking_region_end(th, region);
    xfree(region);
    RUBY_VM_CHECK_INTS();
    errno = saved_errno;
}

/*
 * rb_thread_blocking_region - permit concurrent/parallel execution.
 *
 * This function does:
 *   (1) release GVL.
 *       Other Ruby threads may run in parallel.
 *   (2) call func with data1.
 *   (3) acquire GVL.
 *       Other Ruby threads can not run in parallel any more.
 *
 *   If another thread interrupts this thread (Thread#kill, signal delivery,
 *   VM-shutdown request, and so on), `ubf()' is called (`ubf()' means
 *   "un-blocking function").  `ubf()' should interrupt `func()' execution.
 *
 *   There are built-in ubfs and you can specify these ubfs.
 *   However, we can not guarantee our built-in ubfs interrupt
 *   your `func()' correctly.  Be careful to use rb_thread_blocking_region().
 *
 *     * RUBY_UBF_IO: ubf for IO operation
 *     * RUBY_UBF_PROCESS: ubf for process operation
 *
 *   NOTE: You can not execute most of Ruby C API and touch Ruby
 *         objects in `func()' and `ubf()', including raising an
 *         exception, because current thread doesn't acquire GVL
 *         (cause synchronization problem).  If you need to do it,
 *         read source code of C APIs and confirm by yourself.
 *
 *   NOTE: In short, this API is difficult to use safely.  I recommend you
 *         use other ways if you have.  We lack experiences to use this API.
 *         Please report your problem related on it.
 *
 *   Safe C API:
 *     * rb_thread_interrupted() - check interrupt flag
 *     * ruby_xalloc(), ruby_xrealloc(), ruby_xfree() -
 *         if they called without GVL, acquire GVL automatically.
 */
VALUE
rb_thread_blocking_region(
    rb_blocking_function_t *func, void *data1,
    rb_unblock_function_t *ubf, void *data2)
{
    VALUE val;
    rb_thread_t *th = GET_THREAD();
    int saved_errno = 0;

    th->waiting_fd = -1;
    if (ubf == RUBY_UBF_IO || ubf == RUBY_UBF_PROCESS) {
	ubf = ubf_select;
	data2 = th;
    }

    BLOCKING_REGION({
	val = func(data1);
	saved_errno = errno;
    }, ubf, data2);
    errno = saved_errno;

    return val;
}

VALUE
rb_thread_io_blocking_region(rb_blocking_function_t *func, void *data1, int fd)
{
    VALUE val;
    rb_thread_t *th = GET_THREAD();
    int saved_errno = 0;

    th->waiting_fd = fd;
    BLOCKING_REGION({
	val = func(data1);
	saved_errno = errno;
    }, ubf_select, th);
    th->waiting_fd = -1;
    errno = saved_errno;

    return val;
}

/* alias of rb_thread_blocking_region() */

VALUE
rb_thread_call_without_gvl(
    rb_blocking_function_t *func, void *data1,
    rb_unblock_function_t *ubf, void *data2)
{
    return rb_thread_blocking_region(func, data1, ubf, data2);
}

/*
 * rb_thread_call_with_gvl - re-enter into Ruby world while releasing GVL.
 *
 ***
 *** This API is EXPERIMENTAL!
 *** We do not guarantee that this API remains in ruby 1.9.2 or later.
 ***
 *
 * While releasing GVL using rb_thread_blocking_region() or
 * rb_thread_call_without_gvl(), you can not access Ruby values or invoke methods.
 * If you need to access it, you must use this function rb_thread_call_with_gvl().
 *
 * This function rb_thread_call_with_gvl() does:
 * (1) acquire GVL.
 * (2) call passed function `func'.
 * (3) release GVL.
 * (4) return a value which is returned at (2).
 *
 * NOTE: You should not return Ruby object at (2) because such Object
 *       will not marked.
 *
 * NOTE: If an exception is raised in `func', this function "DOES NOT"
 *       protect (catch) the exception.  If you have any resources
 *       which should free before throwing exception, you need use
 *       rb_protect() in `func' and return a value which represents
 *       exception is raised.
 *
 * NOTE: This functions should not be called by a thread which
 *       is not created as Ruby thread (created by Thread.new or so).
 *       In other words, this function *DOES NOT* associate
 *       NON-Ruby thread to Ruby thread.
 */
void *
rb_thread_call_with_gvl(void *(*func)(void *), void *data1)
{
    rb_thread_t *th = ruby_thread_from_native();
    struct rb_blocking_region_buffer *brb;
    struct rb_unblock_callback prev_unblock;
    void *r;

    if (th == 0) {
	/* Error is occurred, but we can't use rb_bug()
	 * because this thread is not Ruby's thread.
         * What should we do?
	 */

	fprintf(stderr, "[BUG] rb_thread_call_with_gvl() is called by non-ruby thread\n");
	exit(EXIT_FAILURE);
    }

    brb = (struct rb_blocking_region_buffer *)th->blocking_region_buffer;
    prev_unblock = th->unblock;

    if (brb == 0) {
	rb_bug("rb_thread_call_with_gvl: called by a thread which has GVL.");
    }

    blocking_region_end(th, brb);
    /* enter to Ruby world: You can access Ruby values, methods and so on. */
    r = (*func)(data1);
    /* leave from Ruby world: You can not access Ruby values, etc. */
    blocking_region_begin(th, brb, prev_unblock.func, prev_unblock.arg);
    return r;
}

/*
 * ruby_thread_has_gvl_p - check if current native thread has GVL.
 *
 ***
 *** This API is EXPERIMENTAL!
 *** We do not guarantee that this API remains in ruby 1.9.2 or later.
 ***
 */

int
ruby_thread_has_gvl_p(void)
{
    rb_thread_t *th = ruby_thread_from_native();

    if (th && th->blocking_region_buffer == 0) {
	return 1;
    }
    else {
	return 0;
    }
}

/*
 * call-seq:
 *    Thread.pass   -> nil
 *
 * Give the thread scheduler a hint to pass execution to another thread.
 * A running thread may or may not switch, it depends on OS and processor.
 */

static VALUE
thread_s_pass(VALUE klass)
{
    rb_thread_schedule();
    return Qnil;
}

/*
 *
 */

static void
rb_threadptr_execute_interrupts_common(rb_thread_t *th)
{
    rb_atomic_t interrupt;

    if (th->raised_flag) return;

    while ((interrupt = ATOMIC_EXCHANGE(th->interrupt_flag, 0)) != 0) {
	enum rb_thread_status status = th->status;
	int timer_interrupt = interrupt & 0x01;
	int finalizer_interrupt = interrupt & 0x04;
	int sig;

	th->status = THREAD_RUNNABLE;

	/* signal handling */
	if (th == th->vm->main_thread) {
	    while ((sig = rb_get_next_signal()) != 0) {
		rb_signal_exec(th, sig);
	    }
	}

	/* exception from another thread */
	if (th->thrown_errinfo) {
	    VALUE err = th->thrown_errinfo;
	    th->thrown_errinfo = 0;
	    thread_debug("rb_thread_execute_interrupts: %"PRIdVALUE"\n", err);

	    if (err == eKillSignal || err == eTerminateSignal) {
		th->errinfo = INT2FIX(TAG_FATAL);
		TH_JUMP_TAG(th, TAG_FATAL);
	    }
	    else {
		rb_exc_raise(err);
	    }
	}
	th->status = status;

	if (finalizer_interrupt) {
	    rb_gc_finalize_deferred();
	}

	if (timer_interrupt) {
	    unsigned long limits_us = 250 * 1000;

	    if (th->priority > 0)
		limits_us <<= th->priority;
	    else
		limits_us >>= -th->priority;

	    if (status == THREAD_RUNNABLE)
		th->running_time_us += TIME_QUANTUM_USEC;

	    EXEC_EVENT_HOOK(th, RUBY_EVENT_SWITCH, th->cfp->self, 0, 0);

	    rb_thread_schedule_limits(limits_us);
	}
    }
}

void
rb_threadptr_execute_interrupts(rb_thread_t *th)
{
    rb_threadptr_execute_interrupts_common(th);
}

void
rb_thread_execute_interrupts(VALUE thval)
{
    rb_thread_t *th;
    GetThreadPtr(thval, th);
    rb_threadptr_execute_interrupts_common(th);
}

void
rb_gc_mark_threads(void)
{
    rb_bug("deprecated function rb_gc_mark_threads is called");
}

/*****************************************************/

static void
rb_threadptr_ready(rb_thread_t *th)
{
    rb_threadptr_interrupt(th);
}

static VALUE
rb_threadptr_raise(rb_thread_t *th, int argc, VALUE *argv)
{
    VALUE exc;

  again:
    if (rb_threadptr_dead(th)) {
	return Qnil;
    }

    if (th->thrown_errinfo != 0 || th->raised_flag) {
	rb_thread_schedule();
	goto again;
    }

    exc = rb_make_exception(argc, argv);
    th->thrown_errinfo = exc;
    rb_threadptr_ready(th);
    return Qnil;
}

void
rb_threadptr_signal_raise(rb_thread_t *th, int sig)
{
    VALUE argv[2];

    argv[0] = rb_eSignal;
    argv[1] = INT2FIX(sig);
    rb_threadptr_raise(th->vm->main_thread, 2, argv);
}

void
rb_threadptr_signal_exit(rb_thread_t *th)
{
    VALUE argv[2];

    argv[0] = rb_eSystemExit;
    argv[1] = rb_str_new2("exit");
    rb_threadptr_raise(th->vm->main_thread, 2, argv);
}

#if defined(POSIX_SIGNAL) && defined(SIGSEGV) && defined(HAVE_SIGALTSTACK)
#define USE_SIGALTSTACK
#endif

void
ruby_thread_stack_overflow(rb_thread_t *th)
{
    th->raised_flag = 0;
#ifdef USE_SIGALTSTACK
    rb_exc_raise(sysstack_error);
#else
    th->errinfo = sysstack_error;
    TH_JUMP_TAG(th, TAG_RAISE);
#endif
}

int
rb_threadptr_set_raised(rb_thread_t *th)
{
    if (th->raised_flag & RAISED_EXCEPTION) {
	return 1;
    }
    th->raised_flag |= RAISED_EXCEPTION;
    return 0;
}

int
rb_threadptr_reset_raised(rb_thread_t *th)
{
    if (!(th->raised_flag & RAISED_EXCEPTION)) {
	return 0;
    }
    th->raised_flag &= ~RAISED_EXCEPTION;
    return 1;
}

#define THREAD_IO_WAITING_P(th) (			\
	((th)->status == THREAD_STOPPED ||		\
	 (th)->status == THREAD_STOPPED_FOREVER) &&	\
	(th)->blocking_region_buffer &&			\
	(th)->unblock.func == ubf_select &&		\
	1)

static int
thread_fd_close_i(st_data_t key, st_data_t val, st_data_t data)
{
    int fd = (int)data;
    rb_thread_t *th;
    GetThreadPtr((VALUE)key, th);

    if (THREAD_IO_WAITING_P(th)) {
	native_mutex_lock(&th->interrupt_lock);
	if (THREAD_IO_WAITING_P(th) && th->waiting_fd == fd) {
	    th->thrown_errinfo = th->vm->special_exceptions[ruby_error_closed_stream];
	    RUBY_VM_SET_INTERRUPT(th);
	    (th->unblock.func)(th->unblock.arg);
	}
	native_mutex_unlock(&th->interrupt_lock);
    }
    return ST_CONTINUE;
}

void
rb_thread_fd_close(int fd)
{
    st_foreach(GET_THREAD()->vm->living_threads, thread_fd_close_i, (st_index_t)fd);
}

/*
 *  call-seq:
 *     thr.raise
 *     thr.raise(string)
 *     thr.raise(exception [, string [, array]])
 *
 *  Raises an exception (see <code>Kernel::raise</code>) from <i>thr</i>. The
 *  caller does not have to be <i>thr</i>.
 *
 *     Thread.abort_on_exception = true
 *     a = Thread.new { sleep(200) }
 *     a.raise("Gotcha")
 *
 *  <em>produces:</em>
 *
 *     prog.rb:3: Gotcha (RuntimeError)
 *     	from prog.rb:2:in `initialize'
 *     	from prog.rb:2:in `new'
 *     	from prog.rb:2
 */

static VALUE
thread_raise_m(int argc, VALUE *argv, VALUE self)
{
    rb_thread_t *th;
    GetThreadPtr(self, th);
    rb_threadptr_raise(th, argc, argv);
    return Qnil;
}


/*
 *  call-seq:
 *     thr.exit        -> thr or nil
 *     thr.kill        -> thr or nil
 *     thr.terminate   -> thr or nil
 *
 *  Terminates <i>thr</i> and schedules another thread to be run. If this thread
 *  is already marked to be killed, <code>exit</code> returns the
 *  <code>Thread</code>. If this is the main thread, or the last thread, exits
 *  the process.
 */

VALUE
rb_thread_kill(VALUE thread)
{
    rb_thread_t *th;

    GetThreadPtr(thread, th);

    if (th != GET_THREAD() && th->safe_level < 4) {
	rb_secure(4);
    }
    if (th->status == THREAD_TO_KILL || th->status == THREAD_KILLED) {
	return thread;
    }
    if (th == th->vm->main_thread) {
	rb_exit(EXIT_SUCCESS);
    }

    thread_debug("rb_thread_kill: %p (%p)\n", (void *)th, (void *)th->thread_id);

    rb_threadptr_interrupt(th);
    th->thrown_errinfo = eKillSignal;
    th->status = THREAD_TO_KILL;

    return thread;
}


/*
 *  call-seq:
 *     Thread.kill(thread)   -> thread
 *
 *  Causes the given <em>thread</em> to exit (see <code>Thread::exit</code>).
 *
 *     count = 0
 *     a = Thread.new { loop { count += 1 } }
 *     sleep(0.1)       #=> 0
 *     Thread.kill(a)   #=> #<Thread:0x401b3d30 dead>
 *     count            #=> 93947
 *     a.alive?         #=> false
 */

static VALUE
rb_thread_s_kill(VALUE obj, VALUE th)
{
    return rb_thread_kill(th);
}


/*
 *  call-seq:
 *     Thread.exit   -> thread
 *
 *  Terminates the currently running thread and schedules another thread to be
 *  run. If this thread is already marked to be killed, <code>exit</code>
 *  returns the <code>Thread</code>. If this is the main thread, or the last
 *  thread, exit the process.
 */

static VALUE
rb_thread_exit(void)
{
    return rb_thread_kill(GET_THREAD()->self);
}


/*
 *  call-seq:
 *     thr.wakeup   -> thr
 *
 *  Marks <i>thr</i> as eligible for scheduling (it may still remain blocked on
 *  I/O, however). Does not invoke the scheduler (see <code>Thread#run</code>).
 *
 *     c = Thread.new { Thread.stop; puts "hey!" }
 *     sleep 0.1 while c.status!='sleep'
 *     c.wakeup
 *     c.join
 *
 *  <em>produces:</em>
 *
 *     hey!
 */

VALUE
rb_thread_wakeup(VALUE thread)
{
    if (!RTEST(rb_thread_wakeup_alive(thread))) {
	rb_raise(rb_eThreadError, "killed thread");
    }
    return thread;
}

VALUE
rb_thread_wakeup_alive(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (th->status == THREAD_KILLED) {
	return Qnil;
    }
    rb_threadptr_ready(th);
    if (th->status != THREAD_TO_KILL) {
	th->status = THREAD_RUNNABLE;
    }
    return thread;
}


/*
 *  call-seq:
 *     thr.run   -> thr
 *
 *  Wakes up <i>thr</i>, making it eligible for scheduling.
 *
 *     a = Thread.new { puts "a"; Thread.stop; puts "c" }
 *     sleep 0.1 while a.status!='sleep'
 *     puts "Got here"
 *     a.run
 *     a.join
 *
 *  <em>produces:</em>
 *
 *     a
 *     Got here
 *     c
 */

VALUE
rb_thread_run(VALUE thread)
{
    rb_thread_wakeup(thread);
    rb_thread_schedule();
    return thread;
}


/*
 *  call-seq:
 *     Thread.stop   -> nil
 *
 *  Stops execution of the current thread, putting it into a ``sleep'' state,
 *  and schedules execution of another thread.
 *
 *     a = Thread.new { print "a"; Thread.stop; print "c" }
 *     sleep 0.1 while a.status!='sleep'
 *     print "b"
 *     a.run
 *     a.join
 *
 *  <em>produces:</em>
 *
 *     abc
 */

VALUE
rb_thread_stop(void)
{
    if (rb_thread_alone()) {
	rb_raise(rb_eThreadError,
		 "stopping only thread\n\tnote: use sleep to stop forever");
    }
    rb_thread_sleep_deadly();
    return Qnil;
}

static int
thread_list_i(st_data_t key, st_data_t val, void *data)
{
    VALUE ary = (VALUE)data;
    rb_thread_t *th;
    GetThreadPtr((VALUE)key, th);

    switch (th->status) {
      case THREAD_RUNNABLE:
      case THREAD_STOPPED:
      case THREAD_STOPPED_FOREVER:
      case THREAD_TO_KILL:
	rb_ary_push(ary, th->self);
      default:
	break;
    }
    return ST_CONTINUE;
}

/********************************************************************/

/*
 *  call-seq:
 *     Thread.list   -> array
 *
 *  Returns an array of <code>Thread</code> objects for all threads that are
 *  either runnable or stopped.
 *
 *     Thread.new { sleep(200) }
 *     Thread.new { 1000000.times {|i| i*i } }
 *     Thread.new { Thread.stop }
 *     Thread.list.each {|t| p t}
 *
 *  <em>produces:</em>
 *
 *     #<Thread:0x401b3e84 sleep>
 *     #<Thread:0x401b3f38 run>
 *     #<Thread:0x401b3fb0 sleep>
 *     #<Thread:0x401bdf4c run>
 */

VALUE
rb_thread_list(void)
{
    VALUE ary = rb_ary_new();
    st_foreach(GET_THREAD()->vm->living_threads, thread_list_i, ary);
    return ary;
}

VALUE
rb_thread_current(void)
{
    return GET_THREAD()->self;
}

/*
 *  call-seq:
 *     Thread.current   -> thread
 *
 *  Returns the currently executing thread.
 *
 *     Thread.current   #=> #<Thread:0x401bdf4c run>
 */

static VALUE
thread_s_current(VALUE klass)
{
    return rb_thread_current();
}

VALUE
rb_thread_main(void)
{
    return GET_THREAD()->vm->main_thread->self;
}

/*
 *  call-seq:
 *     Thread.main   -> thread
 *
 *  Returns the main thread.
 */

static VALUE
rb_thread_s_main(VALUE klass)
{
    return rb_thread_main();
}


/*
 *  call-seq:
 *     Thread.abort_on_exception   -> true or false
 *
 *  Returns the status of the global ``abort on exception'' condition.  The
 *  default is <code>false</code>. When set to <code>true</code>, or if the
 *  global <code>$DEBUG</code> flag is <code>true</code> (perhaps because the
 *  command line option <code>-d</code> was specified) all threads will abort
 *  (the process will <code>exit(0)</code>) if an exception is raised in any
 *  thread. See also <code>Thread::abort_on_exception=</code>.
 */

static VALUE
rb_thread_s_abort_exc(void)
{
    return GET_THREAD()->vm->thread_abort_on_exception ? Qtrue : Qfalse;
}


/*
 *  call-seq:
 *     Thread.abort_on_exception= boolean   -> true or false
 *
 *  When set to <code>true</code>, all threads will abort if an exception is
 *  raised. Returns the new state.
 *
 *     Thread.abort_on_exception = true
 *     t1 = Thread.new do
 *       puts  "In new thread"
 *       raise "Exception from thread"
 *     end
 *     sleep(1)
 *     puts "not reached"
 *
 *  <em>produces:</em>
 *
 *     In new thread
 *     prog.rb:4: Exception from thread (RuntimeError)
 *     	from prog.rb:2:in `initialize'
 *     	from prog.rb:2:in `new'
 *     	from prog.rb:2
 */

static VALUE
rb_thread_s_abort_exc_set(VALUE self, VALUE val)
{
    rb_secure(4);
    GET_THREAD()->vm->thread_abort_on_exception = RTEST(val);
    return val;
}


/*
 *  call-seq:
 *     thr.abort_on_exception   -> true or false
 *
 *  Returns the status of the thread-local ``abort on exception'' condition for
 *  <i>thr</i>. The default is <code>false</code>. See also
 *  <code>Thread::abort_on_exception=</code>.
 */

static VALUE
rb_thread_abort_exc(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);
    return th->abort_on_exception ? Qtrue : Qfalse;
}


/*
 *  call-seq:
 *     thr.abort_on_exception= boolean   -> true or false
 *
 *  When set to <code>true</code>, causes all threads (including the main
 *  program) to abort if an exception is raised in <i>thr</i>. The process will
 *  effectively <code>exit(0)</code>.
 */

static VALUE
rb_thread_abort_exc_set(VALUE thread, VALUE val)
{
    rb_thread_t *th;
    rb_secure(4);

    GetThreadPtr(thread, th);
    th->abort_on_exception = RTEST(val);
    return val;
}


/*
 *  call-seq:
 *     thr.group   -> thgrp or nil
 *
 *  Returns the <code>ThreadGroup</code> which contains <i>thr</i>, or nil if
 *  the thread is not a member of any group.
 *
 *     Thread.main.group   #=> #<ThreadGroup:0x4029d914>
 */

VALUE
rb_thread_group(VALUE thread)
{
    rb_thread_t *th;
    VALUE group;
    GetThreadPtr(thread, th);
    group = th->thgroup;

    if (!group) {
	group = Qnil;
    }
    return group;
}

static const char *
thread_status_name(enum rb_thread_status status)
{
    switch (status) {
      case THREAD_RUNNABLE:
	return "run";
      case THREAD_STOPPED:
      case THREAD_STOPPED_FOREVER:
	return "sleep";
      case THREAD_TO_KILL:
	return "aborting";
      case THREAD_KILLED:
	return "dead";
      default:
	return "unknown";
    }
}

static int
rb_threadptr_dead(rb_thread_t *th)
{
    return th->status == THREAD_KILLED;
}


/*
 *  call-seq:
 *     thr.status   -> string, false or nil
 *
 *  Returns the status of <i>thr</i>: ``<code>sleep</code>'' if <i>thr</i> is
 *  sleeping or waiting on I/O, ``<code>run</code>'' if <i>thr</i> is executing,
 *  ``<code>aborting</code>'' if <i>thr</i> is aborting, <code>false</code> if
 *  <i>thr</i> terminated normally, and <code>nil</code> if <i>thr</i>
 *  terminated with an exception.
 *
 *     a = Thread.new { raise("die now") }
 *     b = Thread.new { Thread.stop }
 *     c = Thread.new { Thread.exit }
 *     d = Thread.new { sleep }
 *     d.kill                  #=> #<Thread:0x401b3678 aborting>
 *     a.status                #=> nil
 *     b.status                #=> "sleep"
 *     c.status                #=> false
 *     d.status                #=> "aborting"
 *     Thread.current.status   #=> "run"
 */

static VALUE
rb_thread_status(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (rb_threadptr_dead(th)) {
	if (!NIL_P(th->errinfo) && !FIXNUM_P(th->errinfo)
	    /* TODO */ ) {
	    return Qnil;
	}
	return Qfalse;
    }
    return rb_str_new2(thread_status_name(th->status));
}


/*
 *  call-seq:
 *     thr.alive?   -> true or false
 *
 *  Returns <code>true</code> if <i>thr</i> is running or sleeping.
 *
 *     thr = Thread.new { }
 *     thr.join                #=> #<Thread:0x401b3fb0 dead>
 *     Thread.current.alive?   #=> true
 *     thr.alive?              #=> false
 */

static VALUE
rb_thread_alive_p(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (rb_threadptr_dead(th))
	return Qfalse;
    return Qtrue;
}

/*
 *  call-seq:
 *     thr.stop?   -> true or false
 *
 *  Returns <code>true</code> if <i>thr</i> is dead or sleeping.
 *
 *     a = Thread.new { Thread.stop }
 *     b = Thread.current
 *     a.stop?   #=> true
 *     b.stop?   #=> false
 */

static VALUE
rb_thread_stop_p(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (rb_threadptr_dead(th))
	return Qtrue;
    if (th->status == THREAD_STOPPED || th->status == THREAD_STOPPED_FOREVER)
	return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     thr.safe_level   -> integer
 *
 *  Returns the safe level in effect for <i>thr</i>. Setting thread-local safe
 *  levels can help when implementing sandboxes which run insecure code.
 *
 *     thr = Thread.new { $SAFE = 3; sleep }
 *     Thread.current.safe_level   #=> 0
 *     thr.safe_level              #=> 3
 */

static VALUE
rb_thread_safe_level(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    return INT2NUM(th->safe_level);
}

/*
 * call-seq:
 *   thr.inspect   -> string
 *
 * Dump the name, id, and status of _thr_ to a string.
 */

static VALUE
rb_thread_inspect(VALUE thread)
{
    const char *cname = rb_obj_classname(thread);
    rb_thread_t *th;
    const char *status;
    VALUE str;

    GetThreadPtr(thread, th);
    status = thread_status_name(th->status);
    str = rb_sprintf("#<%s:%p %s>", cname, (void *)thread, status);
    OBJ_INFECT(str, thread);

    return str;
}

VALUE
rb_thread_local_aref(VALUE thread, ID id)
{
    rb_thread_t *th;
    st_data_t val;

    GetThreadPtr(thread, th);
    if (rb_safe_level() >= 4 && th != GET_THREAD()) {
	rb_raise(rb_eSecurityError, "Insecure: thread locals");
    }
    if (!th->local_storage) {
	return Qnil;
    }
    if (st_lookup(th->local_storage, id, &val)) {
	return (VALUE)val;
    }
    return Qnil;
}

/*
 *  call-seq:
 *      thr[sym]   -> obj or nil
 *
 *  Attribute Reference---Returns the value of a fiber-local variable (current thread's root fiber
 *  if not explicitely inside a Fiber), using either a symbol or a string name.
 *  If the specified variable does not exist, returns <code>nil</code>.
 *
 *     [
 *       Thread.new { Thread.current["name"] = "A" },
 *       Thread.new { Thread.current[:name]  = "B" },
 *       Thread.new { Thread.current["name"] = "C" }
 *     ].each do |th|
 *       th.join
 *       puts "#{th.inspect}: #{th[:name]}"
 *     end
 *
 *  <em>produces:</em>
 *
 *     #<Thread:0x00000002a54220 dead>: A
 *     #<Thread:0x00000002a541a8 dead>: B
 *     #<Thread:0x00000002a54130 dead>: C
 */

static VALUE
rb_thread_aref(VALUE thread, VALUE id)
{
    return rb_thread_local_aref(thread, rb_to_id(id));
}

VALUE
rb_thread_local_aset(VALUE thread, ID id, VALUE val)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (rb_safe_level() >= 4 && th != GET_THREAD()) {
	rb_raise(rb_eSecurityError, "Insecure: can't modify thread locals");
    }
    if (OBJ_FROZEN(thread)) {
	rb_error_frozen("thread locals");
    }
    if (!th->local_storage) {
	th->local_storage = st_init_numtable();
    }
    if (NIL_P(val)) {
	st_delete_wrap(th->local_storage, id);
	return Qnil;
    }
    st_insert(th->local_storage, id, val);
    return val;
}

/*
 *  call-seq:
 *      thr[sym] = obj   -> obj
 *
 *  Attribute Assignment---Sets or creates the value of a fiber-local variable,
 *  using either a symbol or a string. See also <code>Thread#[]</code>.
 */

static VALUE
rb_thread_aset(VALUE self, VALUE id, VALUE val)
{
    return rb_thread_local_aset(self, rb_to_id(id), val);
}

/*
 *  call-seq:
 *     thr.key?(sym)   -> true or false
 *
 *  Returns <code>true</code> if the given string (or symbol) exists as a
 *  fiber-local variable.
 *
 *     me = Thread.current
 *     me[:oliver] = "a"
 *     me.key?(:oliver)    #=> true
 *     me.key?(:stanley)   #=> false
 */

static VALUE
rb_thread_key_p(VALUE self, VALUE key)
{
    rb_thread_t *th;
    ID id = rb_to_id(key);

    GetThreadPtr(self, th);

    if (!th->local_storage) {
	return Qfalse;
    }
    if (st_lookup(th->local_storage, id, 0)) {
	return Qtrue;
    }
    return Qfalse;
}

static int
thread_keys_i(ID key, VALUE value, VALUE ary)
{
    rb_ary_push(ary, ID2SYM(key));
    return ST_CONTINUE;
}

static int
vm_living_thread_num(rb_vm_t *vm)
{
    return vm->living_threads->num_entries;
}

int
rb_thread_alone(void)
{
    int num = 1;
    if (GET_THREAD()->vm->living_threads) {
	num = vm_living_thread_num(GET_THREAD()->vm);
	thread_debug("rb_thread_alone: %d\n", num);
    }
    return num == 1;
}

/*
 *  call-seq:
 *     thr.keys   -> array
 *
 *  Returns an an array of the names of the fiber-local variables (as Symbols).
 *
 *     thr = Thread.new do
 *       Thread.current[:cat] = 'meow'
 *       Thread.current["dog"] = 'woof'
 *     end
 *     thr.join   #=> #<Thread:0x401b3f10 dead>
 *     thr.keys   #=> [:dog, :cat]
 */

static VALUE
rb_thread_keys(VALUE self)
{
    rb_thread_t *th;
    VALUE ary = rb_ary_new();
    GetThreadPtr(self, th);

    if (th->local_storage) {
	st_foreach(th->local_storage, thread_keys_i, ary);
    }
    return ary;
}

/*
 *  call-seq:
 *     thr.priority   -> integer
 *
 *  Returns the priority of <i>thr</i>. Default is inherited from the
 *  current thread which creating the new thread, or zero for the
 *  initial main thread; higher-priority thread will run more frequently
 *  than lower-priority threads (but lower-priority threads can also run).
 *
 *  This is just hint for Ruby thread scheduler.  It may be ignored on some
 *  platform.
 *
 *     Thread.current.priority   #=> 0
 */

static VALUE
rb_thread_priority(VALUE thread)
{
    rb_thread_t *th;
    GetThreadPtr(thread, th);
    return INT2NUM(th->priority);
}


/*
 *  call-seq:
 *     thr.priority= integer   -> thr
 *
 *  Sets the priority of <i>thr</i> to <i>integer</i>. Higher-priority threads
 *  will run more frequently than lower-priority threads (but lower-priority
 *  threads can also run).
 *
 *  This is just hint for Ruby thread scheduler.  It may be ignored on some
 *  platform.
 *
 *     count1 = count2 = 0
 *     a = Thread.new do
 *           loop { count1 += 1 }
 *         end
 *     a.priority = -1
 *
 *     b = Thread.new do
 *           loop { count2 += 1 }
 *         end
 *     b.priority = -2
 *     sleep 1   #=> 1
 *     count1    #=> 622504
 *     count2    #=> 5832
 */

static VALUE
rb_thread_priority_set(VALUE thread, VALUE prio)
{
    rb_thread_t *th;
    int priority;
    GetThreadPtr(thread, th);

    rb_secure(4);

#if USE_NATIVE_THREAD_PRIORITY
    th->priority = NUM2INT(prio);
    native_thread_apply_priority(th);
#else
    priority = NUM2INT(prio);
    if (priority > RUBY_THREAD_PRIORITY_MAX) {
	priority = RUBY_THREAD_PRIORITY_MAX;
    }
    else if (priority < RUBY_THREAD_PRIORITY_MIN) {
	priority = RUBY_THREAD_PRIORITY_MIN;
    }
    th->priority = priority;
#endif
    return INT2NUM(th->priority);
}

/* for IO */

#if defined(NFDBITS) && defined(HAVE_RB_FD_INIT)

/*
 * several Unix platforms support file descriptors bigger than FD_SETSIZE
 * in select(2) system call.
 *
 * - Linux 2.2.12 (?)
 * - NetBSD 1.2 (src/sys/kern/sys_generic.c:1.25)
 *   select(2) documents how to allocate fd_set dynamically.
 *   http://netbsd.gw.com/cgi-bin/man-cgi?select++NetBSD-4.0
 * - FreeBSD 2.2 (src/sys/kern/sys_generic.c:1.19)
 * - OpenBSD 2.0 (src/sys/kern/sys_generic.c:1.4)
 *   select(2) documents how to allocate fd_set dynamically.
 *   http://www.openbsd.org/cgi-bin/man.cgi?query=select&manpath=OpenBSD+4.4
 * - HP-UX documents how to allocate fd_set dynamically.
 *   http://docs.hp.com/en/B2355-60105/select.2.html
 * - Solaris 8 has select_large_fdset
 * - Mac OS X 10.7 (Lion)
 *   select(2) returns EINVAL if nfds is greater than FD_SET_SIZE and
 *   _DARWIN_UNLIMITED_SELECT (or _DARWIN_C_SOURCE) isn't defined.
 *   http://developer.apple.com/library/mac/#releasenotes/Darwin/SymbolVariantsRelNotes/_index.html
 *
 * When fd_set is not big enough to hold big file descriptors,
 * it should be allocated dynamically.
 * Note that this assumes fd_set is structured as bitmap.
 *
 * rb_fd_init allocates the memory.
 * rb_fd_term free the memory.
 * rb_fd_set may re-allocates bitmap.
 *
 * So rb_fd_set doesn't reject file descriptors bigger than FD_SETSIZE.
 */

void
rb_fd_init(rb_fdset_t *fds)
{
    fds->maxfd = 0;
    fds->fdset = ALLOC(fd_set);
    FD_ZERO(fds->fdset);
}

void
rb_fd_init_copy(rb_fdset_t *dst, rb_fdset_t *src)
{
    size_t size = howmany(rb_fd_max(src), NFDBITS) * sizeof(fd_mask);

    if (size < sizeof(fd_set))
	size = sizeof(fd_set);
    dst->maxfd = src->maxfd;
    dst->fdset = xmalloc(size);
    memcpy(dst->fdset, src->fdset, size);
}

void
rb_fd_term(rb_fdset_t *fds)
{
    if (fds->fdset) xfree(fds->fdset);
    fds->maxfd = 0;
    fds->fdset = 0;
}

void
rb_fd_zero(rb_fdset_t *fds)
{
    if (fds->fdset)
	MEMZERO(fds->fdset, fd_mask, howmany(fds->maxfd, NFDBITS));
}

static void
rb_fd_resize(int n, rb_fdset_t *fds)
{
    size_t m = howmany(n + 1, NFDBITS) * sizeof(fd_mask);
    size_t o = howmany(fds->maxfd, NFDBITS) * sizeof(fd_mask);

    if (m < sizeof(fd_set)) m = sizeof(fd_set);
    if (o < sizeof(fd_set)) o = sizeof(fd_set);

    if (m > o) {
	fds->fdset = xrealloc(fds->fdset, m);
	memset((char *)fds->fdset + o, 0, m - o);
    }
    if (n >= fds->maxfd) fds->maxfd = n + 1;
}

void
rb_fd_set(int n, rb_fdset_t *fds)
{
    rb_fd_resize(n, fds);
    FD_SET(n, fds->fdset);
}

void
rb_fd_clr(int n, rb_fdset_t *fds)
{
    if (n >= fds->maxfd) return;
    FD_CLR(n, fds->fdset);
}

int
rb_fd_isset(int n, const rb_fdset_t *fds)
{
    if (n >= fds->maxfd) return 0;
    return FD_ISSET(n, fds->fdset) != 0; /* "!= 0" avoids FreeBSD PR 91421 */
}

void
rb_fd_copy(rb_fdset_t *dst, const fd_set *src, int max)
{
    size_t size = howmany(max, NFDBITS) * sizeof(fd_mask);

    if (size < sizeof(fd_set)) size = sizeof(fd_set);
    dst->maxfd = max;
    dst->fdset = xrealloc(dst->fdset, size);
    memcpy(dst->fdset, src, size);
}

static void
rb_fd_rcopy(fd_set *dst, rb_fdset_t *src)
{
    size_t size = howmany(rb_fd_max(src), NFDBITS) * sizeof(fd_mask);

    if (size > sizeof(fd_set)) {
	rb_raise(rb_eArgError, "too large fdsets");
    }
    memcpy(dst, rb_fd_ptr(src), sizeof(fd_set));
}

void
rb_fd_dup(rb_fdset_t *dst, const rb_fdset_t *src)
{
    size_t size = howmany(rb_fd_max(src), NFDBITS) * sizeof(fd_mask);

    if (size < sizeof(fd_set))
	size = sizeof(fd_set);
    dst->maxfd = src->maxfd;
    dst->fdset = xrealloc(dst->fdset, size);
    memcpy(dst->fdset, src->fdset, size);
}

int
rb_fd_select(int n, rb_fdset_t *readfds, rb_fdset_t *writefds, rb_fdset_t *exceptfds, struct timeval *timeout)
{
    fd_set *r = NULL, *w = NULL, *e = NULL;
    if (readfds) {
        rb_fd_resize(n - 1, readfds);
        r = rb_fd_ptr(readfds);
    }
    if (writefds) {
        rb_fd_resize(n - 1, writefds);
        w = rb_fd_ptr(writefds);
    }
    if (exceptfds) {
        rb_fd_resize(n - 1, exceptfds);
        e = rb_fd_ptr(exceptfds);
    }
    return select(n, r, w, e, timeout);
}

#undef FD_ZERO
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET

#define FD_ZERO(f)	rb_fd_zero(f)
#define FD_SET(i, f)	rb_fd_set((i), (f))
#define FD_CLR(i, f)	rb_fd_clr((i), (f))
#define FD_ISSET(i, f)	rb_fd_isset((i), (f))

#elif defined(_WIN32)

void
rb_fd_init(rb_fdset_t *set)
{
    set->capa = FD_SETSIZE;
    set->fdset = ALLOC(fd_set);
    FD_ZERO(set->fdset);
}

void
rb_fd_init_copy(rb_fdset_t *dst, rb_fdset_t *src)
{
    rb_fd_init(dst);
    rb_fd_dup(dst, src);
}

static void
rb_fd_rcopy(fd_set *dst, rb_fdset_t *src)
{
    int max = rb_fd_max(src);

    /* we assume src is the result of select() with dst, so dst should be
     * larger or equal than src. */
    if (max > FD_SETSIZE || (UINT)max > dst->fd_count) {
	rb_raise(rb_eArgError, "too large fdsets");
    }

    memcpy(dst->fd_array, src->fdset->fd_array, max);
    dst->fd_count = max;
}

void
rb_fd_term(rb_fdset_t *set)
{
    xfree(set->fdset);
    set->fdset = NULL;
    set->capa = 0;
}

void
rb_fd_set(int fd, rb_fdset_t *set)
{
    unsigned int i;
    SOCKET s = rb_w32_get_osfhandle(fd);

    for (i = 0; i < set->fdset->fd_count; i++) {
        if (set->fdset->fd_array[i] == s) {
            return;
        }
    }
    if (set->fdset->fd_count >= (unsigned)set->capa) {
	set->capa = (set->fdset->fd_count / FD_SETSIZE + 1) * FD_SETSIZE;
	set->fdset = xrealloc(set->fdset, sizeof(unsigned int) + sizeof(SOCKET) * set->capa);
    }
    set->fdset->fd_array[set->fdset->fd_count++] = s;
}

#undef FD_ZERO
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET

#define FD_ZERO(f)	rb_fd_zero(f)
#define FD_SET(i, f)	rb_fd_set((i), (f))
#define FD_CLR(i, f)	rb_fd_clr((i), (f))
#define FD_ISSET(i, f)	rb_fd_isset((i), (f))

#else
#define rb_fd_rcopy(d, s) (*(d) = *(s))
#endif

static int
do_select(int n, rb_fdset_t *read, rb_fdset_t *write, rb_fdset_t *except,
	  struct timeval *timeout)
{
    int result, lerrno;
    rb_fdset_t UNINITIALIZED_VAR(orig_read);
    rb_fdset_t UNINITIALIZED_VAR(orig_write);
    rb_fdset_t UNINITIALIZED_VAR(orig_except);
    double limit = 0;
    struct timeval wait_rest;
    rb_thread_t *th = GET_THREAD();

    if (timeout) {
	limit = timeofday();
	limit += (double)timeout->tv_sec+(double)timeout->tv_usec*1e-6;
	wait_rest = *timeout;
	timeout = &wait_rest;
    }

    if (read)
	rb_fd_init_copy(&orig_read, read);
    if (write)
	rb_fd_init_copy(&orig_write, write);
    if (except)
	rb_fd_init_copy(&orig_except, except);

  retry:
    lerrno = 0;

    BLOCKING_REGION({
	    result = native_fd_select(n, read, write, except, timeout, th);
	    if (result < 0) lerrno = errno;
	}, ubf_select, th);
    errno = lerrno;

    if (result < 0) {
	switch (errno) {
	  case EINTR:
#ifdef ERESTART
	  case ERESTART:
#endif
	    if (read)
		rb_fd_dup(read, &orig_read);
	    if (write)
		rb_fd_dup(write, &orig_write);
	    if (except)
		rb_fd_dup(except, &orig_except);

	    if (timeout) {
		double d = limit - timeofday();

		wait_rest.tv_sec = (time_t)d;
		wait_rest.tv_usec = (int)((d-(double)wait_rest.tv_sec)*1e6);
		if (wait_rest.tv_sec < 0)  wait_rest.tv_sec = 0;
		if (wait_rest.tv_usec < 0) wait_rest.tv_usec = 0;
	    }

	    goto retry;
	  default:
	    break;
	}
    }

    if (read)
	rb_fd_term(&orig_read);
    if (write)
	rb_fd_term(&orig_write);
    if (except)
	rb_fd_term(&orig_except);

    return result;
}

static void
rb_thread_wait_fd_rw(int fd, int read)
{
    int result = 0;
    int events = read ? RB_WAITFD_IN : RB_WAITFD_OUT;

    thread_debug("rb_thread_wait_fd_rw(%d, %s)\n", fd, read ? "read" : "write");

    if (fd < 0) {
	rb_raise(rb_eIOError, "closed stream");
    }
    if (rb_thread_alone()) return;
    while (result <= 0) {
	result = rb_wait_for_single_fd(fd, events, NULL);

	if (result < 0) {
	    rb_sys_fail(0);
	}
    }

    thread_debug("rb_thread_wait_fd_rw(%d, %s): done\n", fd, read ? "read" : "write");
}

void
rb_thread_wait_fd(int fd)
{
    rb_thread_wait_fd_rw(fd, 1);
}

int
rb_thread_fd_writable(int fd)
{
    rb_thread_wait_fd_rw(fd, 0);
    return TRUE;
}

int
rb_thread_select(int max, fd_set * read, fd_set * write, fd_set * except,
		 struct timeval *timeout)
{
    rb_fdset_t fdsets[3];
    rb_fdset_t *rfds = NULL;
    rb_fdset_t *wfds = NULL;
    rb_fdset_t *efds = NULL;
    int retval;

    if (read) {
	rfds = &fdsets[0];
	rb_fd_init(rfds);
	rb_fd_copy(rfds, read, max);
    }
    if (write) {
	wfds = &fdsets[1];
	rb_fd_init(wfds);
	rb_fd_copy(wfds, write, max);
    }
    if (except) {
	efds = &fdsets[2];
	rb_fd_init(efds);
	rb_fd_copy(efds, except, max);
    }

    retval = rb_thread_fd_select(max, rfds, wfds, efds, timeout);

    if (rfds) {
	rb_fd_rcopy(read, rfds);
	rb_fd_term(rfds);
    }
    if (wfds) {
	rb_fd_rcopy(write, wfds);
	rb_fd_term(wfds);
    }
    if (efds) {
	rb_fd_rcopy(except, efds);
	rb_fd_term(efds);
    }

    return retval;
}

int
rb_thread_fd_select(int max, rb_fdset_t * read, rb_fdset_t * write, rb_fdset_t * except,
		    struct timeval *timeout)
{
    if (!read && !write && !except) {
	if (!timeout) {
	    rb_thread_sleep_forever();
	    return 0;
	}
	rb_thread_wait_for(*timeout);
	return 0;
    }

    if (read) {
	rb_fd_resize(max - 1, read);
    }
    if (write) {
	rb_fd_resize(max - 1, write);
    }
    if (except) {
	rb_fd_resize(max - 1, except);
    }
    return do_select(max, read, write, except, timeout);
}

/*
 * poll() is supported by many OSes, but so far Linux is the only
 * one we know of that supports using poll() in all places select()
 * would work.
 */
#if defined(HAVE_POLL) && defined(__linux__)
#  define USE_POLL
#endif

#ifdef USE_POLL

/* The same with linux kernel. TODO: make platform independent definition. */
#define POLLIN_SET (POLLRDNORM | POLLRDBAND | POLLIN | POLLHUP | POLLERR)
#define POLLOUT_SET (POLLWRBAND | POLLWRNORM | POLLOUT | POLLERR)
#define POLLEX_SET (POLLPRI)

#define TIMET_MAX (~(time_t)0 <= 0 ? (time_t)((~(unsigned_time_t)0) >> 1) : (time_t)(~(unsigned_time_t)0))
#define TIMET_MIN (~(time_t)0 <= 0 ? (time_t)(((unsigned_time_t)1) << (sizeof(time_t) * CHAR_BIT - 1)) : (time_t)0)

#ifndef HAVE_PPOLL
/* TODO: don't ignore sigmask */
int ppoll(struct pollfd *fds, nfds_t nfds,
	  const struct timespec *ts, const sigset_t *sigmask)
{
    int timeout_ms;

    if (ts) {
	int tmp, tmp2;

	if (ts->tv_sec > TIMET_MAX/1000)
	    timeout_ms = -1;
	else {
	    tmp = ts->tv_sec * 1000;
	    tmp2 = ts->tv_nsec / (1000 * 1000);
	    if (TIMET_MAX - tmp < tmp2)
		timeout_ms = -1;
	    else
		timeout_ms = tmp + tmp2;
	}
    } else
	timeout_ms = -1;

    return poll(fds, nfds, timeout_ms);
}
#endif

/*
 * returns a mask of events
 */
int
rb_wait_for_single_fd(int fd, int events, struct timeval *tv)
{
    struct pollfd fds;
    int result, lerrno;
    double limit = 0;
    struct timespec ts;
    struct timespec *timeout = NULL;

    if (tv) {
	ts.tv_sec = tv->tv_sec;
	ts.tv_nsec = tv->tv_usec * 1000;
	limit = timeofday();
	limit += (double)tv->tv_sec + (double)tv->tv_usec * 1e-6;
	timeout = &ts;
    }

    fds.fd = fd;
    fds.events = (short)events;

retry:
    lerrno = 0;
    BLOCKING_REGION({
	result = ppoll(&fds, 1, timeout, NULL);
	if (result < 0) lerrno = errno;
    }, ubf_select, GET_THREAD());

    if (result < 0) {
	errno = lerrno;
	switch (errno) {
	  case EINTR:
#ifdef ERESTART
	  case ERESTART:
#endif
	    if (timeout) {
		double d = limit - timeofday();

		ts.tv_sec = (long)d;
		ts.tv_nsec = (long)((d - (double)ts.tv_sec) * 1e9);
		if (ts.tv_sec < 0)
		    ts.tv_sec = 0;
		if (ts.tv_nsec < 0)
		    ts.tv_nsec = 0;
	    }
	    goto retry;
	}
	return -1;
    }

    if (fds.revents & POLLNVAL) {
	errno = EBADF;
	return -1;
    }

    /*
     * POLLIN, POLLOUT have a different meanings from select(2)'s read/write bit.
     * Therefore we need fix it up.
     */
    result = 0;
    if (fds.revents & POLLIN_SET)
	result |= RB_WAITFD_IN;
    if (fds.revents & POLLOUT_SET)
	result |= RB_WAITFD_OUT;
    if (fds.revents & POLLEX_SET)
	result |= RB_WAITFD_PRI;

    return result;
}
#else /* ! USE_POLL - implement rb_io_poll_fd() using select() */
static rb_fdset_t *init_set_fd(int fd, rb_fdset_t *fds)
{
    rb_fd_init(fds);
    rb_fd_set(fd, fds);

    return fds;
}

struct select_args {
    union {
	int fd;
	int error;
    } as;
    rb_fdset_t *read;
    rb_fdset_t *write;
    rb_fdset_t *except;
    struct timeval *tv;
};

static VALUE
select_single(VALUE ptr)
{
    struct select_args *args = (struct select_args *)ptr;
    int r;

    r = rb_thread_fd_select(args->as.fd + 1,
                            args->read, args->write, args->except, args->tv);
    if (r == -1)
	args->as.error = errno;
    if (r > 0) {
	r = 0;
	if (args->read && rb_fd_isset(args->as.fd, args->read))
	    r |= RB_WAITFD_IN;
	if (args->write && rb_fd_isset(args->as.fd, args->write))
	    r |= RB_WAITFD_OUT;
	if (args->except && rb_fd_isset(args->as.fd, args->except))
	    r |= RB_WAITFD_PRI;
    }
    return (VALUE)r;
}

static VALUE
select_single_cleanup(VALUE ptr)
{
    struct select_args *args = (struct select_args *)ptr;

    if (args->read) rb_fd_term(args->read);
    if (args->write) rb_fd_term(args->write);
    if (args->except) rb_fd_term(args->except);

    return (VALUE)-1;
}

int
rb_wait_for_single_fd(int fd, int events, struct timeval *tv)
{
    rb_fdset_t rfds, wfds, efds;
    struct select_args args;
    int r;
    VALUE ptr = (VALUE)&args;

    args.as.fd = fd;
    args.read = (events & RB_WAITFD_IN) ? init_set_fd(fd, &rfds) : NULL;
    args.write = (events & RB_WAITFD_OUT) ? init_set_fd(fd, &wfds) : NULL;
    args.except = (events & RB_WAITFD_PRI) ? init_set_fd(fd, &efds) : NULL;
    args.tv = tv;

    r = (int)rb_ensure(select_single, ptr, select_single_cleanup, ptr);
    if (r == -1)
	errno = args.as.error;

    return r;
}
#endif /* ! USE_POLL */

/*
 * for GC
 */

#ifdef USE_CONSERVATIVE_STACK_END
void
rb_gc_set_stack_end(VALUE **stack_end_p)
{
    VALUE stack_end;
    *stack_end_p = &stack_end;
}
#endif

void
rb_gc_save_machine_context(rb_thread_t *th)
{
    FLUSH_REGISTER_WINDOWS;
#ifdef __ia64
    th->machine_register_stack_end = rb_ia64_bsp();
#endif
    setjmp(th->machine_regs);
}

/*
 *
 */

void
rb_threadptr_check_signal(rb_thread_t *mth)
{
    /* mth must be main_thread */
    if (rb_signal_buff_size() > 0) {
	/* wakeup main thread */
	rb_threadptr_interrupt(mth);
    }
}

static void
timer_thread_function(void *arg)
{
    rb_vm_t *vm = GET_VM(); /* TODO: fix me for Multi-VM */

    /* for time slice */
    RUBY_VM_SET_TIMER_INTERRUPT(vm->running_thread);

    /* check signal */
    rb_threadptr_check_signal(vm->main_thread);

#if 0
    /* prove profiler */
    if (vm->prove_profile.enable) {
	rb_thread_t *th = vm->running_thread;

	if (vm->during_gc) {
	    /* GC prove profiling */
	}
    }
#endif
}

void
rb_thread_stop_timer_thread(int close_anyway)
{
    if (timer_thread_id && native_stop_timer_thread(close_anyway)) {
	native_reset_timer_thread();
    }
}

void
rb_thread_reset_timer_thread(void)
{
    native_reset_timer_thread();
}

void
rb_thread_start_timer_thread(void)
{
    system_working = 1;
    rb_thread_create_timer_thread();
}

static int
clear_coverage_i(st_data_t key, st_data_t val, st_data_t dummy)
{
    int i;
    VALUE lines = (VALUE)val;

    for (i = 0; i < RARRAY_LEN(lines); i++) {
	if (RARRAY_PTR(lines)[i] != Qnil) {
	    RARRAY_PTR(lines)[i] = INT2FIX(0);
	}
    }
    return ST_CONTINUE;
}

static void
clear_coverage(void)
{
    VALUE coverages = rb_get_coverages();
    if (RTEST(coverages)) {
	st_foreach(RHASH_TBL(coverages), clear_coverage_i, 0);
    }
}

static void
rb_thread_atfork_internal(int (*atfork)(st_data_t, st_data_t, st_data_t))
{
    rb_thread_t *th = GET_THREAD();
    rb_vm_t *vm = th->vm;
    VALUE thval = th->self;
    vm->main_thread = th;

    gvl_atfork(th->vm);
    st_foreach(vm->living_threads, atfork, (st_data_t)th);
    st_clear(vm->living_threads);
    st_insert(vm->living_threads, thval, (st_data_t)th->thread_id);
    vm->sleeper = 0;
    clear_coverage();
}

static int
terminate_atfork_i(st_data_t key, st_data_t val, st_data_t current_th)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    if (th != (rb_thread_t *)current_th) {
	if (th->keeping_mutexes) {
	    rb_mutex_abandon_all(th->keeping_mutexes);
	}
	th->keeping_mutexes = NULL;
	thread_cleanup_func(th, TRUE);
    }
    return ST_CONTINUE;
}

void
rb_thread_atfork(void)
{
    rb_thread_atfork_internal(terminate_atfork_i);
    GET_THREAD()->join_list_head = 0;

    /* We don't want reproduce CVE-2003-0900. */
    rb_reset_random_seed();
}

static int
terminate_atfork_before_exec_i(st_data_t key, st_data_t val, st_data_t current_th)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    if (th != (rb_thread_t *)current_th) {
	thread_cleanup_func_before_exec(th);
    }
    return ST_CONTINUE;
}

void
rb_thread_atfork_before_exec(void)
{
    rb_thread_atfork_internal(terminate_atfork_before_exec_i);
}

struct thgroup {
    int enclosed;
    VALUE group;
};

static size_t
thgroup_memsize(const void *ptr)
{
    return ptr ? sizeof(struct thgroup) : 0;
}

static const rb_data_type_t thgroup_data_type = {
    "thgroup",
    {NULL, RUBY_TYPED_DEFAULT_FREE, thgroup_memsize,},
};

/*
 * Document-class: ThreadGroup
 *
 *  <code>ThreadGroup</code> provides a means of keeping track of a number of
 *  threads as a group. A <code>Thread</code> can belong to only one
 *  <code>ThreadGroup</code> at a time; adding a thread to a new group will
 *  remove it from any previous group.
 *
 *  Newly created threads belong to the same group as the thread from which they
 *  were created.
 */

static VALUE
thgroup_s_alloc(VALUE klass)
{
    VALUE group;
    struct thgroup *data;

    group = TypedData_Make_Struct(klass, struct thgroup, &thgroup_data_type, data);
    data->enclosed = 0;
    data->group = group;

    return group;
}

struct thgroup_list_params {
    VALUE ary;
    VALUE group;
};

static int
thgroup_list_i(st_data_t key, st_data_t val, st_data_t data)
{
    VALUE thread = (VALUE)key;
    VALUE ary = ((struct thgroup_list_params *)data)->ary;
    VALUE group = ((struct thgroup_list_params *)data)->group;
    rb_thread_t *th;
    GetThreadPtr(thread, th);

    if (th->thgroup == group) {
	rb_ary_push(ary, thread);
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     thgrp.list   -> array
 *
 *  Returns an array of all existing <code>Thread</code> objects that belong to
 *  this group.
 *
 *     ThreadGroup::Default.list   #=> [#<Thread:0x401bdf4c run>]
 */

static VALUE
thgroup_list(VALUE group)
{
    VALUE ary = rb_ary_new();
    struct thgroup_list_params param;

    param.ary = ary;
    param.group = group;
    st_foreach(GET_THREAD()->vm->living_threads, thgroup_list_i, (st_data_t) & param);
    return ary;
}


/*
 *  call-seq:
 *     thgrp.enclose   -> thgrp
 *
 *  Prevents threads from being added to or removed from the receiving
 *  <code>ThreadGroup</code>. New threads can still be started in an enclosed
 *  <code>ThreadGroup</code>.
 *
 *     ThreadGroup::Default.enclose        #=> #<ThreadGroup:0x4029d914>
 *     thr = Thread::new { Thread.stop }   #=> #<Thread:0x402a7210 sleep>
 *     tg = ThreadGroup::new               #=> #<ThreadGroup:0x402752d4>
 *     tg.add thr
 *
 *  <em>produces:</em>
 *
 *     ThreadError: can't move from the enclosed thread group
 */

static VALUE
thgroup_enclose(VALUE group)
{
    struct thgroup *data;

    TypedData_Get_Struct(group, struct thgroup, &thgroup_data_type, data);
    data->enclosed = 1;

    return group;
}


/*
 *  call-seq:
 *     thgrp.enclosed?   -> true or false
 *
 *  Returns <code>true</code> if <em>thgrp</em> is enclosed. See also
 *  ThreadGroup#enclose.
 */

static VALUE
thgroup_enclosed_p(VALUE group)
{
    struct thgroup *data;

    TypedData_Get_Struct(group, struct thgroup, &thgroup_data_type, data);
    if (data->enclosed)
	return Qtrue;
    return Qfalse;
}


/*
 *  call-seq:
 *     thgrp.add(thread)   -> thgrp
 *
 *  Adds the given <em>thread</em> to this group, removing it from any other
 *  group to which it may have previously belonged.
 *
 *     puts "Initial group is #{ThreadGroup::Default.list}"
 *     tg = ThreadGroup.new
 *     t1 = Thread.new { sleep }
 *     t2 = Thread.new { sleep }
 *     puts "t1 is #{t1}"
 *     puts "t2 is #{t2}"
 *     tg.add(t1)
 *     puts "Initial group now #{ThreadGroup::Default.list}"
 *     puts "tg group now #{tg.list}"
 *
 *  <em>produces:</em>
 *
 *     Initial group is #<Thread:0x401bdf4c>
 *     t1 is #<Thread:0x401b3c90>
 *     t2 is #<Thread:0x401b3c18>
 *     Initial group now #<Thread:0x401b3c18>#<Thread:0x401bdf4c>
 *     tg group now #<Thread:0x401b3c90>
 */

static VALUE
thgroup_add(VALUE group, VALUE thread)
{
    rb_thread_t *th;
    struct thgroup *data;

    rb_secure(4);
    GetThreadPtr(thread, th);

    if (OBJ_FROZEN(group)) {
	rb_raise(rb_eThreadError, "can't move to the frozen thread group");
    }
    TypedData_Get_Struct(group, struct thgroup, &thgroup_data_type, data);
    if (data->enclosed) {
	rb_raise(rb_eThreadError, "can't move to the enclosed thread group");
    }

    if (!th->thgroup) {
	return Qnil;
    }

    if (OBJ_FROZEN(th->thgroup)) {
	rb_raise(rb_eThreadError, "can't move from the frozen thread group");
    }
    TypedData_Get_Struct(th->thgroup, struct thgroup, &thgroup_data_type, data);
    if (data->enclosed) {
	rb_raise(rb_eThreadError,
		 "can't move from the enclosed thread group");
    }

    th->thgroup = group;
    return group;
}


/*
 *  Document-class: Mutex
 *
 *  Mutex implements a simple semaphore that can be used to coordinate access to
 *  shared data from multiple concurrent threads.
 *
 *  Example:
 *
 *    require 'thread'
 *    semaphore = Mutex.new
 *
 *    a = Thread.new {
 *      semaphore.synchronize {
 *        # access shared resource
 *      }
 *    }
 *
 *    b = Thread.new {
 *      semaphore.synchronize {
 *        # access shared resource
 *      }
 *    }
 *
 */

#define GetMutexPtr(obj, tobj) \
    TypedData_Get_Struct((obj), rb_mutex_t, &mutex_data_type, (tobj))

static const char *rb_mutex_unlock_th(rb_mutex_t *mutex, rb_thread_t volatile *th);

#define mutex_mark NULL

static void
mutex_free(void *ptr)
{
    if (ptr) {
	rb_mutex_t *mutex = ptr;
	if (mutex->th) {
	    /* rb_warn("free locked mutex"); */
	    const char *err = rb_mutex_unlock_th(mutex, mutex->th);
	    if (err) rb_bug("%s", err);
	}
	native_mutex_destroy(&mutex->lock);
	native_cond_destroy(&mutex->cond);
    }
    ruby_xfree(ptr);
}

static size_t
mutex_memsize(const void *ptr)
{
    return ptr ? sizeof(rb_mutex_t) : 0;
}

static const rb_data_type_t mutex_data_type = {
    "mutex",
    {mutex_mark, mutex_free, mutex_memsize,},
};

VALUE
rb_obj_is_mutex(VALUE obj)
{
    if (rb_typeddata_is_kind_of(obj, &mutex_data_type)) {
	return Qtrue;
    }
    else {
	return Qfalse;
    }
}

static VALUE
mutex_alloc(VALUE klass)
{
    VALUE volatile obj;
    rb_mutex_t *mutex;

    obj = TypedData_Make_Struct(klass, rb_mutex_t, &mutex_data_type, mutex);
    native_mutex_initialize(&mutex->lock);
    native_cond_initialize(&mutex->cond, RB_CONDATTR_CLOCK_MONOTONIC);
    return obj;
}

/*
 *  call-seq:
 *     Mutex.new   -> mutex
 *
 *  Creates a new Mutex
 */
static VALUE
mutex_initialize(VALUE self)
{
    return self;
}

VALUE
rb_mutex_new(void)
{
    return mutex_alloc(rb_cMutex);
}

/*
 * call-seq:
 *    mutex.locked?  -> true or false
 *
 * Returns +true+ if this lock is currently held by some thread.
 */
VALUE
rb_mutex_locked_p(VALUE self)
{
    rb_mutex_t *mutex;
    GetMutexPtr(self, mutex);
    return mutex->th ? Qtrue : Qfalse;
}

static void
mutex_locked(rb_thread_t *th, VALUE self)
{
    rb_mutex_t *mutex;
    GetMutexPtr(self, mutex);

    if (th->keeping_mutexes) {
	mutex->next_mutex = th->keeping_mutexes;
    }
    th->keeping_mutexes = mutex;
}

/*
 * call-seq:
 *    mutex.try_lock  -> true or false
 *
 * Attempts to obtain the lock and returns immediately. Returns +true+ if the
 * lock was granted.
 */
VALUE
rb_mutex_trylock(VALUE self)
{
    rb_mutex_t *mutex;
    VALUE locked = Qfalse;
    GetMutexPtr(self, mutex);

    native_mutex_lock(&mutex->lock);
    if (mutex->th == 0) {
	mutex->th = GET_THREAD();
	locked = Qtrue;

	mutex_locked(GET_THREAD(), self);
    }
    native_mutex_unlock(&mutex->lock);

    return locked;
}

static int
lock_func(rb_thread_t *th, rb_mutex_t *mutex, int timeout_ms)
{
    int interrupted = 0;
    int err = 0;

    mutex->cond_waiting++;
    for (;;) {
	if (!mutex->th) {
	    mutex->th = th;
	    break;
	}
	if (RUBY_VM_INTERRUPTED(th)) {
	    interrupted = 1;
	    break;
	}
	if (err == ETIMEDOUT) {
	    interrupted = 2;
	    break;
	}

	if (timeout_ms) {
	    struct timespec timeout_rel;
	    struct timespec timeout;

	    timeout_rel.tv_sec = 0;
	    timeout_rel.tv_nsec = timeout_ms * 1000 * 1000;
	    timeout = native_cond_timeout(&mutex->cond, timeout_rel);
	    err = native_cond_timedwait(&mutex->cond, &mutex->lock, &timeout);
	}
	else {
	    native_cond_wait(&mutex->cond, &mutex->lock);
	    err = 0;
	}
    }
    mutex->cond_waiting--;

    return interrupted;
}

static void
lock_interrupt(void *ptr)
{
    rb_mutex_t *mutex = (rb_mutex_t *)ptr;
    native_mutex_lock(&mutex->lock);
    if (mutex->cond_waiting > 0)
	native_cond_broadcast(&mutex->cond);
    native_mutex_unlock(&mutex->lock);
}

/*
 * call-seq:
 *    mutex.lock  -> self
 *
 * Attempts to grab the lock and waits if it isn't available.
 * Raises +ThreadError+ if +mutex+ was locked by the current thread.
 */
VALUE
rb_mutex_lock(VALUE self)
{

    if (rb_mutex_trylock(self) == Qfalse) {
	rb_mutex_t *mutex;
	rb_thread_t *th = GET_THREAD();
	GetMutexPtr(self, mutex);

	if (mutex->th == GET_THREAD()) {
	    rb_raise(rb_eThreadError, "deadlock; recursive locking");
	}

	while (mutex->th != th) {
	    int interrupted;
	    enum rb_thread_status prev_status = th->status;
	    int timeout_ms = 0;
	    struct rb_unblock_callback oldubf;

	    set_unblock_function(th, lock_interrupt, mutex, &oldubf);
	    th->status = THREAD_STOPPED_FOREVER;
	    th->locking_mutex = self;

	    native_mutex_lock(&mutex->lock);
	    th->vm->sleeper++;
	    /*
	     * Carefully! while some contended threads are in lock_func(),
	     * vm->sleepr is unstable value. we have to avoid both deadlock
	     * and busy loop.
	     */
	    if (vm_living_thread_num(th->vm) == th->vm->sleeper) {
		timeout_ms = 100;
	    }
	    GVL_UNLOCK_BEGIN();
	    interrupted = lock_func(th, mutex, timeout_ms);
	    native_mutex_unlock(&mutex->lock);
	    GVL_UNLOCK_END();

	    reset_unblock_function(th, &oldubf);

	    th->locking_mutex = Qfalse;
	    if (mutex->th && interrupted == 2) {
		rb_check_deadlock(th->vm);
	    }
	    if (th->status == THREAD_STOPPED_FOREVER) {
		th->status = prev_status;
	    }
	    th->vm->sleeper--;

	    if (mutex->th == th) mutex_locked(th, self);

	    if (interrupted) {
		RUBY_VM_CHECK_INTS();
	    }
	}
    }
    return self;
}

static const char *
rb_mutex_unlock_th(rb_mutex_t *mutex, rb_thread_t volatile *th)
{
    const char *err = NULL;

    native_mutex_lock(&mutex->lock);

    if (mutex->th == 0) {
	err = "Attempt to unlock a mutex which is not locked";
    }
    else if (mutex->th != th) {
	err = "Attempt to unlock a mutex which is locked by another thread";
    }
    else {
	mutex->th = 0;
	if (mutex->cond_waiting > 0)
	    native_cond_signal(&mutex->cond);
    }

    native_mutex_unlock(&mutex->lock);

    if (!err) {
	rb_mutex_t *volatile *th_mutex = &th->keeping_mutexes;
	while (*th_mutex != mutex) {
	    th_mutex = &(*th_mutex)->next_mutex;
	}
	*th_mutex = mutex->next_mutex;
	mutex->next_mutex = NULL;
    }

    return err;
}

/*
 * call-seq:
 *    mutex.unlock    -> self
 *
 * Releases the lock.
 * Raises +ThreadError+ if +mutex+ wasn't locked by the current thread.
 */
VALUE
rb_mutex_unlock(VALUE self)
{
    const char *err;
    rb_mutex_t *mutex;
    GetMutexPtr(self, mutex);

    err = rb_mutex_unlock_th(mutex, GET_THREAD());
    if (err) rb_raise(rb_eThreadError, "%s", err);

    return self;
}

static void
rb_mutex_abandon_all(rb_mutex_t *mutexes)
{
    rb_mutex_t *mutex;

    while (mutexes) {
	mutex = mutexes;
	mutexes = mutex->next_mutex;
	mutex->th = 0;
	mutex->next_mutex = 0;
    }
}

static VALUE
rb_mutex_sleep_forever(VALUE time)
{
    rb_thread_sleep_deadly();
    return Qnil;
}

static VALUE
rb_mutex_wait_for(VALUE time)
{
    const struct timeval *t = (struct timeval *)time;
    rb_thread_wait_for(*t);
    return Qnil;
}

VALUE
rb_mutex_sleep(VALUE self, VALUE timeout)
{
    time_t beg, end;
    struct timeval t;

    if (!NIL_P(timeout)) {
        t = rb_time_interval(timeout);
    }
    rb_mutex_unlock(self);
    beg = time(0);
    if (NIL_P(timeout)) {
	rb_ensure(rb_mutex_sleep_forever, Qnil, rb_mutex_lock, self);
    }
    else {
	rb_ensure(rb_mutex_wait_for, (VALUE)&t, rb_mutex_lock, self);
    }
    end = time(0) - beg;
    return INT2FIX(end);
}

/*
 * call-seq:
 *    mutex.sleep(timeout = nil)    -> number
 *
 * Releases the lock and sleeps +timeout+ seconds if it is given and
 * non-nil or forever.  Raises +ThreadError+ if +mutex+ wasn't locked by
 * the current thread.
 */
static VALUE
mutex_sleep(int argc, VALUE *argv, VALUE self)
{
    VALUE timeout;

    rb_scan_args(argc, argv, "01", &timeout);
    return rb_mutex_sleep(self, timeout);
}

/*
 * call-seq:
 *    mutex.synchronize { ... }    -> result of the block
 *
 * Obtains a lock, runs the block, and releases the lock when the block
 * completes.  See the example under +Mutex+.
 */

VALUE
rb_mutex_synchronize(VALUE mutex, VALUE (*func)(VALUE arg), VALUE arg)
{
    rb_mutex_lock(mutex);
    return rb_ensure(func, arg, rb_mutex_unlock, mutex);
}

/*
 * Document-class: Barrier
 */
static void
barrier_mark(void *ptr)
{
    rb_gc_mark((VALUE)ptr);
}

static const rb_data_type_t barrier_data_type = {
    "barrier",
    {barrier_mark, 0, 0,},
};

static VALUE
barrier_alloc(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &barrier_data_type, (void *)mutex_alloc(0));
}

#define GetBarrierPtr(obj) ((VALUE)rb_check_typeddata((obj), &barrier_data_type))
#define BARRIER_WAITING_MASK (FL_USER0|FL_USER1|FL_USER2|FL_USER3|FL_USER4|FL_USER5|FL_USER6|FL_USER7|FL_USER8|FL_USER9|FL_USER10|FL_USER11|FL_USER12|FL_USER13|FL_USER14|FL_USER15|FL_USER16|FL_USER17|FL_USER18|FL_USER19)
#define BARRIER_WAITING_SHIFT (FL_USHIFT)
#define rb_barrier_waiting(b) (int)((RBASIC(b)->flags&BARRIER_WAITING_MASK)>>BARRIER_WAITING_SHIFT)
#define rb_barrier_waiting_inc(b) do { \
    int w = rb_barrier_waiting(b); \
    w++; \
    RBASIC(b)->flags &= ~BARRIER_WAITING_MASK; \
    RBASIC(b)->flags |= ((VALUE)w << BARRIER_WAITING_SHIFT);	\
} while (0)
#define rb_barrier_waiting_dec(b) do { \
    int w = rb_barrier_waiting(b); \
    w--; \
    RBASIC(b)->flags &= ~BARRIER_WAITING_MASK; \
    RBASIC(b)->flags |= ((VALUE)w << BARRIER_WAITING_SHIFT); \
} while (0)

VALUE
rb_barrier_new(void)
{
    VALUE barrier = barrier_alloc(rb_cBarrier);
    rb_mutex_lock((VALUE)DATA_PTR(barrier));
    return barrier;
}

/*
 * Wait a barrier.
 *
 * Returns
 *  true:  acquired the barrier
 *  false: the barrier was destroyed and no other threads waiting
 *  nil:   the barrier was destroyed but still in use
 */
VALUE
rb_barrier_wait(VALUE self)
{
    VALUE mutex = GetBarrierPtr(self);
    rb_mutex_t *m;

    if (!mutex) return Qfalse;
    GetMutexPtr(mutex, m);
    if (m->th == GET_THREAD()) return Qnil;
    rb_barrier_waiting_inc(self);
    rb_mutex_lock(mutex);
    rb_barrier_waiting_dec(self);
    if (DATA_PTR(self)) return Qtrue;
    rb_mutex_unlock(mutex);
    return rb_barrier_waiting(self) > 0 ? Qnil : Qfalse;
}

/*
 * Release a barrrier, and return true if it has waiting threads.
 */
VALUE
rb_barrier_release(VALUE self)
{
    VALUE mutex = GetBarrierPtr(self);
    rb_mutex_unlock(mutex);
    return rb_barrier_waiting(self) > 0 ? Qtrue : Qfalse;
}

/*
 * Release and destroy a barrrier, and return true if it has waiting threads.
 */
VALUE
rb_barrier_destroy(VALUE self)
{
    VALUE mutex = GetBarrierPtr(self);
    DATA_PTR(self) = 0;
    rb_mutex_unlock(mutex);
    return rb_barrier_waiting(self) > 0 ? Qtrue : Qfalse;
}

/* variables for recursive traversals */
static ID recursive_key;

/*
 * Returns the current "recursive list" used to detect recursion.
 * This list is a hash table, unique for the current thread and for
 * the current __callee__.
 */

static VALUE
recursive_list_access(void)
{
    volatile VALUE hash = rb_thread_local_aref(rb_thread_current(), recursive_key);
    VALUE sym = ID2SYM(rb_frame_this_func());
    VALUE list;
    if (NIL_P(hash) || !RB_TYPE_P(hash, T_HASH)) {
	hash = rb_hash_new();
	OBJ_UNTRUST(hash);
	rb_thread_local_aset(rb_thread_current(), recursive_key, hash);
	list = Qnil;
    }
    else {
	list = rb_hash_aref(hash, sym);
    }
    if (NIL_P(list) || !RB_TYPE_P(list, T_HASH)) {
	list = rb_hash_new();
	OBJ_UNTRUST(list);
	rb_hash_aset(hash, sym, list);
    }
    return list;
}

/*
 * Returns Qtrue iff obj_id (or the pair <obj, paired_obj>) is already
 * in the recursion list.
 * Assumes the recursion list is valid.
 */

static VALUE
recursive_check(VALUE list, VALUE obj_id, VALUE paired_obj_id)
{
    VALUE pair_list = rb_hash_lookup2(list, obj_id, Qundef);
    if (pair_list == Qundef)
	return Qfalse;
    if (paired_obj_id) {
	if (!RB_TYPE_P(pair_list, T_HASH)) {
	if (pair_list != paired_obj_id)
	    return Qfalse;
	}
	else {
	if (NIL_P(rb_hash_lookup(pair_list, paired_obj_id)))
	    return Qfalse;
	}
    }
    return Qtrue;
}

/*
 * Pushes obj_id (or the pair <obj_id, paired_obj_id>) in the recursion list.
 * For a single obj_id, it sets list[obj_id] to Qtrue.
 * For a pair, it sets list[obj_id] to paired_obj_id if possible,
 * otherwise list[obj_id] becomes a hash like:
 *   {paired_obj_id_1 => true, paired_obj_id_2 => true, ... }
 * Assumes the recursion list is valid.
 */

static void
recursive_push(VALUE list, VALUE obj, VALUE paired_obj)
{
    VALUE pair_list;

    if (!paired_obj) {
	rb_hash_aset(list, obj, Qtrue);
    }
    else if ((pair_list = rb_hash_lookup2(list, obj, Qundef)) == Qundef) {
	rb_hash_aset(list, obj, paired_obj);
    }
    else {
	if (!RB_TYPE_P(pair_list, T_HASH)){
	    VALUE other_paired_obj = pair_list;
	    pair_list = rb_hash_new();
	    OBJ_UNTRUST(pair_list);
	    rb_hash_aset(pair_list, other_paired_obj, Qtrue);
	    rb_hash_aset(list, obj, pair_list);
	}
	rb_hash_aset(pair_list, paired_obj, Qtrue);
    }
}

/*
 * Pops obj_id (or the pair <obj_id, paired_obj_id>) from the recursion list.
 * For a pair, if list[obj_id] is a hash, then paired_obj_id is
 * removed from the hash and no attempt is made to simplify
 * list[obj_id] from {only_one_paired_id => true} to only_one_paired_id
 * Assumes the recursion list is valid.
 */

static void
recursive_pop(VALUE list, VALUE obj, VALUE paired_obj)
{
    if (paired_obj) {
	VALUE pair_list = rb_hash_lookup2(list, obj, Qundef);
	if (pair_list == Qundef) {
	    VALUE symname = rb_inspect(ID2SYM(rb_frame_this_func()));
	    VALUE thrname = rb_inspect(rb_thread_current());
	    rb_raise(rb_eTypeError, "invalid inspect_tbl pair_list for %s in %s",
		     StringValuePtr(symname), StringValuePtr(thrname));
	}
	if (RB_TYPE_P(pair_list, T_HASH)) {
	    rb_hash_delete(pair_list, paired_obj);
	    if (!RHASH_EMPTY_P(pair_list)) {
		return; /* keep hash until is empty */
	    }
	}
    }
    rb_hash_delete(list, obj);
}

struct exec_recursive_params {
    VALUE (*func) (VALUE, VALUE, int);
    VALUE list;
    VALUE obj;
    VALUE objid;
    VALUE pairid;
    VALUE arg;
};

static VALUE
exec_recursive_i(VALUE tag, struct exec_recursive_params *p)
{
    VALUE result = Qundef;
    int state;

    recursive_push(p->list, p->objid, p->pairid);
    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	result = (*p->func)(p->obj, p->arg, FALSE);
    }
    POP_TAG();
    recursive_pop(p->list, p->objid, p->pairid);
    if (state)
	JUMP_TAG(state);
    return result;
}

/*
 * Calls func(obj, arg, recursive), where recursive is non-zero if the
 * current method is called recursively on obj, or on the pair <obj, pairid>
 * If outer is 0, then the innermost func will be called with recursive set
 * to Qtrue, otherwise the outermost func will be called. In the latter case,
 * all inner func are short-circuited by throw.
 * Implementation details: the value thrown is the recursive list which is
 * proper to the current method and unlikely to be catched anywhere else.
 * list[recursive_key] is used as a flag for the outermost call.
 */

static VALUE
exec_recursive(VALUE (*func) (VALUE, VALUE, int), VALUE obj, VALUE pairid, VALUE arg, int outer)
{
    VALUE result = Qundef;
    struct exec_recursive_params p;
    int outermost;
    p.list = recursive_list_access();
    p.objid = rb_obj_id(obj);
    p.obj = obj;
    p.pairid = pairid;
    p.arg = arg;
    outermost = outer && !recursive_check(p.list, ID2SYM(recursive_key), 0);

    if (recursive_check(p.list, p.objid, pairid)) {
	if (outer && !outermost) {
	    rb_throw_obj(p.list, p.list);
	}
	return (*func)(obj, arg, TRUE);
    }
    else {
	p.func = func;

	if (outermost) {
	    recursive_push(p.list, ID2SYM(recursive_key), 0);
	    result = rb_catch_obj(p.list, exec_recursive_i, (VALUE)&p);
	    recursive_pop(p.list, ID2SYM(recursive_key), 0);
	    if (result == p.list) {
		result = (*func)(obj, arg, TRUE);
	    }
	}
	else {
	    result = exec_recursive_i(0, &p);
	}
    }
    *(volatile struct exec_recursive_params *)&p;
    return result;
}

/*
 * Calls func(obj, arg, recursive), where recursive is non-zero if the
 * current method is called recursively on obj
 */

VALUE
rb_exec_recursive(VALUE (*func) (VALUE, VALUE, int), VALUE obj, VALUE arg)
{
    return exec_recursive(func, obj, 0, arg, 0);
}

/*
 * Calls func(obj, arg, recursive), where recursive is non-zero if the
 * current method is called recursively on the ordered pair <obj, paired_obj>
 */

VALUE
rb_exec_recursive_paired(VALUE (*func) (VALUE, VALUE, int), VALUE obj, VALUE paired_obj, VALUE arg)
{
    return exec_recursive(func, obj, rb_obj_id(paired_obj), arg, 0);
}

/*
 * If recursion is detected on the current method and obj, the outermost
 * func will be called with (obj, arg, Qtrue). All inner func will be
 * short-circuited using throw.
 */

VALUE
rb_exec_recursive_outer(VALUE (*func) (VALUE, VALUE, int), VALUE obj, VALUE arg)
{
    return exec_recursive(func, obj, 0, arg, 1);
}

/* tracer */
#define RUBY_EVENT_REMOVED 0x1000000

enum {
    EVENT_RUNNING_NOTHING,
    EVENT_RUNNING_TRACE = 1,
    EVENT_RUNNING_THREAD = 2,
    EVENT_RUNNING_VM = 4,
    EVENT_RUNNING_EVENT_MASK = EVENT_RUNNING_VM|EVENT_RUNNING_THREAD
};

static VALUE thread_suppress_tracing(rb_thread_t *th, int ev, VALUE (*func)(VALUE, int), VALUE arg, int always);

struct event_call_args {
    rb_thread_t *th;
    VALUE klass;
    VALUE self;
    VALUE proc;
    ID id;
    rb_event_flag_t event;
};

static rb_event_hook_t *
alloc_event_hook(rb_event_hook_func_t func, rb_event_flag_t events, VALUE data)
{
    rb_event_hook_t *hook = ALLOC(rb_event_hook_t);
    hook->func = func;
    hook->flag = events;
    hook->data = data;
    return hook;
}

static void
thread_reset_event_flags(rb_thread_t *th)
{
    rb_event_hook_t *hook = th->event_hooks;
    rb_event_flag_t flag = th->event_flags & RUBY_EVENT_VM;

    while (hook) {
	if (!(flag & RUBY_EVENT_REMOVED))
	    flag |= hook->flag;
	hook = hook->next;
    }
    th->event_flags = flag;
}

static void
rb_threadptr_add_event_hook(rb_thread_t *th,
			 rb_event_hook_func_t func, rb_event_flag_t events, VALUE data)
{
    rb_event_hook_t *hook = alloc_event_hook(func, events, data);
    hook->next = th->event_hooks;
    th->event_hooks = hook;
    thread_reset_event_flags(th);
}

static rb_thread_t *
thval2thread_t(VALUE thval)
{
    rb_thread_t *th;
    GetThreadPtr(thval, th);
    return th;
}

void
rb_thread_add_event_hook(VALUE thval,
			 rb_event_hook_func_t func, rb_event_flag_t events, VALUE data)
{
    rb_threadptr_add_event_hook(thval2thread_t(thval), func, events, data);
}

static int
set_threads_event_flags_i(st_data_t key, st_data_t val, st_data_t flag)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    if (flag) {
	th->event_flags |= RUBY_EVENT_VM;
    }
    else {
	th->event_flags &= (~RUBY_EVENT_VM);
    }
    return ST_CONTINUE;
}

static void
set_threads_event_flags(int flag)
{
    st_foreach(GET_VM()->living_threads, set_threads_event_flags_i, (st_data_t) flag);
}

static inline int
exec_event_hooks(const rb_event_hook_t *hook, rb_event_flag_t flag, VALUE self, ID id, VALUE klass)
{
    int removed = 0;
    for (; hook; hook = hook->next) {
	if (hook->flag & RUBY_EVENT_REMOVED) {
	    removed++;
	    continue;
	}
	if (flag & hook->flag) {
	    (*hook->func)(flag, hook->data, self, id, klass);
	}
    }
    return removed;
}

static int remove_defered_event_hook(rb_event_hook_t **root);

static VALUE
thread_exec_event_hooks(VALUE args, int running)
{
    struct event_call_args *argp = (struct event_call_args *)args;
    rb_thread_t *th = argp->th;
    rb_event_flag_t flag = argp->event;
    VALUE self = argp->self;
    ID id = argp->id;
    VALUE klass = argp->klass;
    const rb_event_flag_t wait_event = th->event_flags;
    int removed;

    if (self == rb_mRubyVMFrozenCore) return 0;

    if ((wait_event & flag) && !(running & EVENT_RUNNING_THREAD)) {
	th->tracing |= EVENT_RUNNING_THREAD;
	removed = exec_event_hooks(th->event_hooks, flag, self, id, klass);
	th->tracing &= ~EVENT_RUNNING_THREAD;
	if (removed) {
	    remove_defered_event_hook(&th->event_hooks);
	}
    }
    if (wait_event & RUBY_EVENT_VM) {
	if (th->vm->event_hooks == NULL) {
	    th->event_flags &= (~RUBY_EVENT_VM);
	}
	else if (!(running & EVENT_RUNNING_VM)) {
	    th->tracing |= EVENT_RUNNING_VM;
	    removed = exec_event_hooks(th->vm->event_hooks, flag, self, id, klass);
	    th->tracing &= ~EVENT_RUNNING_VM;
	    if (removed) {
		remove_defered_event_hook(&th->vm->event_hooks);
	    }
	}
    }
    return 0;
}

void
rb_threadptr_exec_event_hooks(rb_thread_t *th, rb_event_flag_t flag, VALUE self, ID id, VALUE klass)
{
    const VALUE errinfo = th->errinfo;
    struct event_call_args args;
    args.th = th;
    args.event = flag;
    args.self = self;
    args.id = id;
    args.klass = klass;
    args.proc = 0;
    thread_suppress_tracing(th, EVENT_RUNNING_EVENT_MASK, thread_exec_event_hooks, (VALUE)&args, FALSE);
    th->errinfo = errinfo;
}

void
rb_add_event_hook(rb_event_hook_func_t func, rb_event_flag_t events, VALUE data)
{
    rb_event_hook_t *hook = alloc_event_hook(func, events, data);
    rb_vm_t *vm = GET_VM();

    hook->next = vm->event_hooks;
    vm->event_hooks = hook;

    set_threads_event_flags(1);
}

static int
defer_remove_event_hook(rb_event_hook_t *hook, rb_event_hook_func_t func)
{
    while (hook) {
	if (func == 0 || hook->func == func) {
	    hook->flag |= RUBY_EVENT_REMOVED;
	}
	hook = hook->next;
    }
    return -1;
}

static int
remove_event_hook(rb_event_hook_t **root, rb_event_hook_func_t func)
{
    rb_event_hook_t *hook = *root, *next;

    while (hook) {
	next = hook->next;
	if (func == 0 || hook->func == func || (hook->flag & RUBY_EVENT_REMOVED)) {
	    *root = next;
	    xfree(hook);
	}
	else {
	    root = &hook->next;
	}
	hook = next;
    }
    return -1;
}

static int
remove_defered_event_hook(rb_event_hook_t **root)
{
    rb_event_hook_t *hook = *root, *next;

    while (hook) {
	next = hook->next;
	if (hook->flag & RUBY_EVENT_REMOVED) {
	    *root = next;
	    xfree(hook);
	}
	else {
	    root = &hook->next;
	}
	hook = next;
    }
    return -1;
}

static int
rb_threadptr_remove_event_hook(rb_thread_t *th, rb_event_hook_func_t func)
{
    int ret;
    if (th->tracing & EVENT_RUNNING_THREAD) {
	ret = defer_remove_event_hook(th->event_hooks, func);
    }
    else {
	ret = remove_event_hook(&th->event_hooks, func);
    }
    thread_reset_event_flags(th);
    return ret;
}

int
rb_thread_remove_event_hook(VALUE thval, rb_event_hook_func_t func)
{
    return rb_threadptr_remove_event_hook(thval2thread_t(thval), func);
}

static rb_event_hook_t *
search_live_hook(rb_event_hook_t *hook)
{
    while (hook) {
	if (!(hook->flag & RUBY_EVENT_REMOVED))
	    return hook;
	hook = hook->next;
    }
    return NULL;
}

static int
running_vm_event_hooks(st_data_t key, st_data_t val, st_data_t data)
{
    rb_thread_t *th = thval2thread_t((VALUE)key);
    if (!(th->tracing & EVENT_RUNNING_VM)) return ST_CONTINUE;
    *(rb_thread_t **)data = th;
    return ST_STOP;
}

static rb_thread_t *
vm_event_hooks_running_thread(rb_vm_t *vm)
{
    rb_thread_t *found = NULL;
    st_foreach(vm->living_threads, running_vm_event_hooks, (st_data_t)&found);
    return found;
}

int
rb_remove_event_hook(rb_event_hook_func_t func)
{
    rb_vm_t *vm = GET_VM();
    rb_event_hook_t *hook = search_live_hook(vm->event_hooks);
    int ret;

    if (vm_event_hooks_running_thread(vm)) {
	ret = defer_remove_event_hook(vm->event_hooks, func);
    }
    else {
	ret = remove_event_hook(&vm->event_hooks, func);
    }

    if (hook && !search_live_hook(vm->event_hooks)) {
	set_threads_event_flags(0);
    }

    return ret;
}

static int
clear_trace_func_i(st_data_t key, st_data_t val, st_data_t flag)
{
    rb_thread_t *th;
    GetThreadPtr((VALUE)key, th);
    rb_threadptr_remove_event_hook(th, 0);
    return ST_CONTINUE;
}

void
rb_clear_trace_func(void)
{
    st_foreach(GET_VM()->living_threads, clear_trace_func_i, (st_data_t) 0);
    rb_remove_event_hook(0);
}

static void call_trace_func(rb_event_flag_t, VALUE data, VALUE self, ID id, VALUE klass);

/*
 *  call-seq:
 *     set_trace_func(proc)    -> proc
 *     set_trace_func(nil)     -> nil
 *
 *  Establishes _proc_ as the handler for tracing, or disables
 *  tracing if the parameter is +nil+. _proc_ takes up
 *  to six parameters: an event name, a filename, a line number, an
 *  object id, a binding, and the name of a class. _proc_ is
 *  invoked whenever an event occurs. Events are: <code>c-call</code>
 *  (call a C-language routine), <code>c-return</code> (return from a
 *  C-language routine), <code>call</code> (call a Ruby method),
 *  <code>class</code> (start a class or module definition),
 *  <code>end</code> (finish a class or module definition),
 *  <code>line</code> (execute code on a new line), <code>raise</code>
 *  (raise an exception), and <code>return</code> (return from a Ruby
 *  method). Tracing is disabled within the context of _proc_.
 *
 *      class Test
 *	def test
 *	  a = 1
 *	  b = 2
 *	end
 *      end
 *
 *      set_trace_func proc { |event, file, line, id, binding, classname|
 *	   printf "%8s %s:%-2d %10s %8s\n", event, file, line, id, classname
 *      }
 *      t = Test.new
 *      t.test
 *
 *	  line prog.rb:11               false
 *      c-call prog.rb:11        new    Class
 *      c-call prog.rb:11 initialize   Object
 *    c-return prog.rb:11 initialize   Object
 *    c-return prog.rb:11        new    Class
 *	  line prog.rb:12               false
 *  	  call prog.rb:2        test     Test
 *	  line prog.rb:3        test     Test
 *	  line prog.rb:4        test     Test
 *      return prog.rb:4        test     Test
 */

static VALUE
set_trace_func(VALUE obj, VALUE trace)
{
    rb_remove_event_hook(call_trace_func);

    if (NIL_P(trace)) {
	GET_THREAD()->tracing = EVENT_RUNNING_NOTHING;
	return Qnil;
    }

    if (!rb_obj_is_proc(trace)) {
	rb_raise(rb_eTypeError, "trace_func needs to be Proc");
    }

    rb_add_event_hook(call_trace_func, RUBY_EVENT_ALL, trace);
    return trace;
}

static void
thread_add_trace_func(rb_thread_t *th, VALUE trace)
{
    if (!rb_obj_is_proc(trace)) {
	rb_raise(rb_eTypeError, "trace_func needs to be Proc");
    }

    rb_threadptr_add_event_hook(th, call_trace_func, RUBY_EVENT_ALL, trace);
}

/*
 *  call-seq:
 *     thr.add_trace_func(proc)    -> proc
 *
 *  Adds _proc_ as a handler for tracing.
 *  See <code>Thread#set_trace_func</code> and +set_trace_func+.
 */

static VALUE
thread_add_trace_func_m(VALUE obj, VALUE trace)
{
    rb_thread_t *th;
    GetThreadPtr(obj, th);
    thread_add_trace_func(th, trace);
    return trace;
}

/*
 *  call-seq:
 *     thr.set_trace_func(proc)    -> proc
 *     thr.set_trace_func(nil)     -> nil
 *
 *  Establishes _proc_ on _thr_ as the handler for tracing, or
 *  disables tracing if the parameter is +nil+.
 *  See +set_trace_func+.
 */

static VALUE
thread_set_trace_func_m(VALUE obj, VALUE trace)
{
    rb_thread_t *th;
    GetThreadPtr(obj, th);
    rb_threadptr_remove_event_hook(th, call_trace_func);

    if (NIL_P(trace)) {
	th->tracing = EVENT_RUNNING_NOTHING;
	return Qnil;
    }
    thread_add_trace_func(th, trace);
    return trace;
}

static const char *
get_event_name(rb_event_flag_t event)
{
    switch (event) {
      case RUBY_EVENT_LINE:
	return "line";
      case RUBY_EVENT_CLASS:
	return "class";
      case RUBY_EVENT_END:
	return "end";
      case RUBY_EVENT_CALL:
	return "call";
      case RUBY_EVENT_RETURN:
	return "return";
      case RUBY_EVENT_C_CALL:
	return "c-call";
      case RUBY_EVENT_C_RETURN:
	return "c-return";
      case RUBY_EVENT_RAISE:
	return "raise";
      default:
	return "unknown";
    }
}

static VALUE
call_trace_proc(VALUE args, int tracing)
{
    struct event_call_args *p = (struct event_call_args *)args;
    const char *srcfile = rb_sourcefile();
    VALUE eventname = rb_str_new2(get_event_name(p->event));
    VALUE filename = srcfile ? rb_str_new2(srcfile) : Qnil;
    VALUE argv[6];
    int line = rb_sourceline();
    ID id = 0;
    VALUE klass = 0;

    if (p->klass != 0) {
	id = p->id;
	klass = p->klass;
    }
    else {
	rb_thread_method_id_and_class(p->th, &id, &klass);
    }
    if (id == ID_ALLOCATOR)
      return Qnil;
    if (klass) {
	if (RB_TYPE_P(klass, T_ICLASS)) {
	    klass = RBASIC(klass)->klass;
	}
	else if (FL_TEST(klass, FL_SINGLETON)) {
	    klass = rb_iv_get(klass, "__attached__");
	}
    }

    argv[0] = eventname;
    argv[1] = filename;
    argv[2] = INT2FIX(line);
    argv[3] = id ? ID2SYM(id) : Qnil;
    argv[4] = (p->self && srcfile) ? rb_binding_new() : Qnil;
    argv[5] = klass ? klass : Qnil;

    return rb_proc_call_with_block(p->proc, 6, argv, Qnil);
}

static void
call_trace_func(rb_event_flag_t event, VALUE proc, VALUE self, ID id, VALUE klass)
{
    struct event_call_args args;

    args.th = GET_THREAD();
    args.event = event;
    args.proc = proc;
    args.self = self;
    args.id = id;
    args.klass = klass;
    ruby_suppress_tracing(call_trace_proc, (VALUE)&args, FALSE);
}

VALUE
ruby_suppress_tracing(VALUE (*func)(VALUE, int), VALUE arg, int always)
{
    rb_thread_t *th = GET_THREAD();
    return thread_suppress_tracing(th, EVENT_RUNNING_TRACE, func, arg, always);
}

static VALUE
thread_suppress_tracing(rb_thread_t *th, int ev, VALUE (*func)(VALUE, int), VALUE arg, int always)
{
    int state, tracing = th->tracing, running = tracing & ev;
    volatile int raised;
    volatile int outer_state;
    VALUE result = Qnil;

    if (running == ev && !always) {
	return Qnil;
    }
    else {
	th->tracing |= ev;
    }

    raised = rb_threadptr_reset_raised(th);
    outer_state = th->state;
    th->state = 0;

    PUSH_TAG();
    if ((state = EXEC_TAG()) == 0) {
	result = (*func)(arg, running);
    }

    if (raised) {
	rb_threadptr_set_raised(th);
    }
    POP_TAG();

    th->tracing = tracing;
    if (state) {
	JUMP_TAG(state);
    }
    th->state = outer_state;

    return result;
}

/*
 *  call-seq:
 *     thr.backtrace    -> array
 *
 *  Returns the current back trace of the _thr_.
 */

static VALUE
rb_thread_backtrace_m(VALUE thval)
{
    return rb_thread_backtrace(thval);
}

/*
 *  Document-class: ThreadError
 *
 *  Raised when an invalid operation is attempted on a thread.
 *
 *  For example, when no other thread has been started:
 *
 *     Thread.stop
 *
 *  <em>raises the exception:</em>
 *
 *     ThreadError: stopping only thread
 */

/*
 *  +Thread+ encapsulates the behavior of a thread of
 *  execution, including the main thread of the Ruby script.
 *
 *  In the descriptions of the methods in this class, the parameter _sym_
 *  refers to a symbol, which is either a quoted string or a
 *  +Symbol+ (such as <code>:name</code>).
 */

void
Init_Thread(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    VALUE cThGroup;
    rb_thread_t *th = GET_THREAD();

    rb_define_singleton_method(rb_cThread, "new", thread_s_new, -1);
    rb_define_singleton_method(rb_cThread, "start", thread_start, -2);
    rb_define_singleton_method(rb_cThread, "fork", thread_start, -2);
    rb_define_singleton_method(rb_cThread, "main", rb_thread_s_main, 0);
    rb_define_singleton_method(rb_cThread, "current", thread_s_current, 0);
    rb_define_singleton_method(rb_cThread, "stop", rb_thread_stop, 0);
    rb_define_singleton_method(rb_cThread, "kill", rb_thread_s_kill, 1);
    rb_define_singleton_method(rb_cThread, "exit", rb_thread_exit, 0);
    rb_define_singleton_method(rb_cThread, "pass", thread_s_pass, 0);
    rb_define_singleton_method(rb_cThread, "list", rb_thread_list, 0);
    rb_define_singleton_method(rb_cThread, "abort_on_exception", rb_thread_s_abort_exc, 0);
    rb_define_singleton_method(rb_cThread, "abort_on_exception=", rb_thread_s_abort_exc_set, 1);
#if THREAD_DEBUG < 0
    rb_define_singleton_method(rb_cThread, "DEBUG", rb_thread_s_debug, 0);
    rb_define_singleton_method(rb_cThread, "DEBUG=", rb_thread_s_debug_set, 1);
#endif

    rb_define_method(rb_cThread, "initialize", thread_initialize, -2);
    rb_define_method(rb_cThread, "raise", thread_raise_m, -1);
    rb_define_method(rb_cThread, "join", thread_join_m, -1);
    rb_define_method(rb_cThread, "value", thread_value, 0);
    rb_define_method(rb_cThread, "kill", rb_thread_kill, 0);
    rb_define_method(rb_cThread, "terminate", rb_thread_kill, 0);
    rb_define_method(rb_cThread, "exit", rb_thread_kill, 0);
    rb_define_method(rb_cThread, "run", rb_thread_run, 0);
    rb_define_method(rb_cThread, "wakeup", rb_thread_wakeup, 0);
    rb_define_method(rb_cThread, "[]", rb_thread_aref, 1);
    rb_define_method(rb_cThread, "[]=", rb_thread_aset, 2);
    rb_define_method(rb_cThread, "key?", rb_thread_key_p, 1);
    rb_define_method(rb_cThread, "keys", rb_thread_keys, 0);
    rb_define_method(rb_cThread, "priority", rb_thread_priority, 0);
    rb_define_method(rb_cThread, "priority=", rb_thread_priority_set, 1);
    rb_define_method(rb_cThread, "status", rb_thread_status, 0);
    rb_define_method(rb_cThread, "alive?", rb_thread_alive_p, 0);
    rb_define_method(rb_cThread, "stop?", rb_thread_stop_p, 0);
    rb_define_method(rb_cThread, "abort_on_exception", rb_thread_abort_exc, 0);
    rb_define_method(rb_cThread, "abort_on_exception=", rb_thread_abort_exc_set, 1);
    rb_define_method(rb_cThread, "safe_level", rb_thread_safe_level, 0);
    rb_define_method(rb_cThread, "group", rb_thread_group, 0);
    rb_define_method(rb_cThread, "backtrace", rb_thread_backtrace_m, 0);

    rb_define_method(rb_cThread, "inspect", rb_thread_inspect, 0);

    closed_stream_error = rb_exc_new2(rb_eIOError, "stream closed");
    OBJ_TAINT(closed_stream_error);
    OBJ_FREEZE(closed_stream_error);

    cThGroup = rb_define_class("ThreadGroup", rb_cObject);
    rb_define_alloc_func(cThGroup, thgroup_s_alloc);
    rb_define_method(cThGroup, "list", thgroup_list, 0);
    rb_define_method(cThGroup, "enclose", thgroup_enclose, 0);
    rb_define_method(cThGroup, "enclosed?", thgroup_enclosed_p, 0);
    rb_define_method(cThGroup, "add", thgroup_add, 1);

    {
	th->thgroup = th->vm->thgroup_default = rb_obj_alloc(cThGroup);
	rb_define_const(cThGroup, "Default", th->thgroup);
    }

    rb_cMutex = rb_define_class("Mutex", rb_cObject);
    rb_define_alloc_func(rb_cMutex, mutex_alloc);
    rb_define_method(rb_cMutex, "initialize", mutex_initialize, 0);
    rb_define_method(rb_cMutex, "locked?", rb_mutex_locked_p, 0);
    rb_define_method(rb_cMutex, "try_lock", rb_mutex_trylock, 0);
    rb_define_method(rb_cMutex, "lock", rb_mutex_lock, 0);
    rb_define_method(rb_cMutex, "unlock", rb_mutex_unlock, 0);
    rb_define_method(rb_cMutex, "sleep", mutex_sleep, -1);

    recursive_key = rb_intern("__recursive_key__");
    rb_eThreadError = rb_define_class("ThreadError", rb_eStandardError);

    /* trace */
    rb_define_global_function("set_trace_func", set_trace_func, 1);
    rb_define_method(rb_cThread, "set_trace_func", thread_set_trace_func_m, 1);
    rb_define_method(rb_cThread, "add_trace_func", thread_add_trace_func_m, 1);

    /* init thread core */
    {
	/* main thread setting */
	{
	    /* acquire global vm lock */
	    gvl_init(th->vm);
	    gvl_acquire(th->vm, th);
	    native_mutex_initialize(&th->interrupt_lock);
	}
    }

    rb_thread_create_timer_thread();

    /* suppress warnings on cygwin, mingw and mswin.*/
    (void)native_mutex_trylock;
}

int
ruby_native_thread_p(void)
{
    rb_thread_t *th = ruby_thread_from_native();

    return th != 0;
}

static int
check_deadlock_i(st_data_t key, st_data_t val, int *found)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    if (th->status != THREAD_STOPPED_FOREVER || RUBY_VM_INTERRUPTED(th)) {
	*found = 1;
    }
    else if (th->locking_mutex) {
	rb_mutex_t *mutex;
	GetMutexPtr(th->locking_mutex, mutex);

	native_mutex_lock(&mutex->lock);
	if (mutex->th == th || (!mutex->th && mutex->cond_waiting)) {
	    *found = 1;
	}
	native_mutex_unlock(&mutex->lock);
    }

    return (*found) ? ST_STOP : ST_CONTINUE;
}

#ifdef DEBUG_DEADLOCK_CHECK
static int
debug_i(st_data_t key, st_data_t val, int *found)
{
    VALUE thval = key;
    rb_thread_t *th;
    GetThreadPtr(thval, th);

    printf("th:%p %d %d", th, th->status, th->interrupt_flag);
    if (th->locking_mutex) {
	rb_mutex_t *mutex;
	GetMutexPtr(th->locking_mutex, mutex);

	native_mutex_lock(&mutex->lock);
	printf(" %p %d\n", mutex->th, mutex->cond_waiting);
	native_mutex_unlock(&mutex->lock);
    }
    else
	puts("");

    return ST_CONTINUE;
}
#endif

static void
rb_check_deadlock(rb_vm_t *vm)
{
    int found = 0;

    if (vm_living_thread_num(vm) > vm->sleeper) return;
    if (vm_living_thread_num(vm) < vm->sleeper) rb_bug("sleeper must not be more than vm_living_thread_num(vm)");

    st_foreach(vm->living_threads, check_deadlock_i, (st_data_t)&found);

    if (!found) {
	VALUE argv[2];
	argv[0] = rb_eFatal;
	argv[1] = rb_str_new2("deadlock detected");
#ifdef DEBUG_DEADLOCK_CHECK
	printf("%d %d %p %p\n", vm->living_threads->num_entries, vm->sleeper, GET_THREAD(), vm->main_thread);
	st_foreach(vm->living_threads, debug_i, (st_data_t)0);
#endif
	vm->sleeper--;
	rb_threadptr_raise(vm->main_thread, 2, argv);
    }
}

static void
update_coverage(rb_event_flag_t event, VALUE proc, VALUE self, ID id, VALUE klass)
{
    VALUE coverage = GET_THREAD()->cfp->iseq->coverage;
    if (coverage && RBASIC(coverage)->klass == 0) {
	long line = rb_sourceline() - 1;
	long count;
	if (RARRAY_PTR(coverage)[line] == Qnil) {
	    return;
	}
	count = FIX2LONG(RARRAY_PTR(coverage)[line]) + 1;
	if (POSFIXABLE(count)) {
	    RARRAY_PTR(coverage)[line] = LONG2FIX(count);
	}
    }
}

VALUE
rb_get_coverages(void)
{
    return GET_VM()->coverages;
}

void
rb_set_coverages(VALUE coverages)
{
    GET_VM()->coverages = coverages;
    rb_add_event_hook(update_coverage, RUBY_EVENT_COVERAGE, Qnil);
}

void
rb_reset_coverages(void)
{
    GET_VM()->coverages = Qfalse;
    rb_remove_event_hook(update_coverage);
}

