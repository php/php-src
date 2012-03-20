/**********************************************************************

  cont.c -

  $Author$
  created at: Thu May 23 09:03:43 2007

  Copyright (C) 2007 Koichi Sasada

**********************************************************************/

#include "ruby/ruby.h"
#include "internal.h"
#include "vm_core.h"
#include "gc.h"
#include "eval_intern.h"

#if ((defined(_WIN32) && _WIN32_WINNT >= 0x0400) || (defined(HAVE_GETCONTEXT) && defined(HAVE_SETCONTEXT))) && !defined(__NetBSD__) && !defined(__sun) && !defined(FIBER_USE_NATIVE)
#define FIBER_USE_NATIVE 1

/* FIBER_USE_NATIVE enables Fiber performance improvement using system
 * dependent method such as make/setcontext on POSIX system or
 * CreateFiber() API on Windows.
 * This hack make Fiber context switch faster (x2 or more).
 * However, it decrease maximum number of Fiber.  For example, on the
 * 32bit POSIX OS, ten or twenty thousands Fiber can be created.
 *
 * Details is reported in the paper "A Fast Fiber Implementation for Ruby 1.9"
 * in Proc. of 51th Programming Symposium, pp.21--28 (2010) (in Japanese).
 */

/* On our experience, NetBSD doesn't support using setcontext() and pthread
 * simultaneously.  This is because pthread_self(), TLS and other information
 * are represented by stack pointer (higher bits of stack pointer).
 * TODO: check such constraint on configure.
 */
#elif !defined(FIBER_USE_NATIVE)
#define FIBER_USE_NATIVE 0
#endif

#if FIBER_USE_NATIVE
#ifndef _WIN32
#include <unistd.h>
#include <sys/mman.h>
#include <ucontext.h>
#endif
#define RB_PAGE_SIZE (pagesize)
#define RB_PAGE_MASK (~(RB_PAGE_SIZE - 1))
static long pagesize;
#define FIBER_MACHINE_STACK_ALLOCATION_SIZE  (0x10000)
#endif

#define CAPTURE_JUST_VALID_VM_STACK 1

enum context_type {
    CONTINUATION_CONTEXT = 0,
    FIBER_CONTEXT = 1,
    ROOT_FIBER_CONTEXT = 2
};

typedef struct rb_context_struct {
    enum context_type type;
    VALUE self;
    int argc;
    VALUE value;
    VALUE *vm_stack;
#ifdef CAPTURE_JUST_VALID_VM_STACK
    size_t vm_stack_slen;  /* length of stack (head of th->stack) */
    size_t vm_stack_clen;  /* length of control frames (tail of th->stack) */
#endif
    VALUE *machine_stack;
    VALUE *machine_stack_src;
#ifdef __ia64
    VALUE *machine_register_stack;
    VALUE *machine_register_stack_src;
    int machine_register_stack_size;
#endif
    rb_thread_t saved_thread;
    rb_jmpbuf_t jmpbuf;
    size_t machine_stack_size;
} rb_context_t;

enum fiber_status {
    CREATED,
    RUNNING,
    TERMINATED
};

#if FIBER_USE_NATIVE && !defined(_WIN32)
#define MAX_MAHINE_STACK_CACHE  10
static int machine_stack_cache_index = 0;
typedef struct machine_stack_cache_struct {
    void *ptr;
    size_t size;
} machine_stack_cache_t;
static machine_stack_cache_t machine_stack_cache[MAX_MAHINE_STACK_CACHE];
static machine_stack_cache_t terminated_machine_stack;
#endif

typedef struct rb_fiber_struct {
    rb_context_t cont;
    VALUE prev;
    enum fiber_status status;
    struct rb_fiber_struct *prev_fiber;
    struct rb_fiber_struct *next_fiber;
    /* If a fiber invokes "transfer",
     * then this fiber can't "resume" any more after that.
     * You shouldn't mix "transfer" and "resume".
     */
    int transfered;

#if FIBER_USE_NATIVE
#ifdef _WIN32
    void *fib_handle;
#else
    ucontext_t context;
#endif
#endif
} rb_fiber_t;

static const rb_data_type_t cont_data_type, fiber_data_type;
static VALUE rb_cContinuation;
static VALUE rb_cFiber;
static VALUE rb_eFiberError;

#define GetContPtr(obj, ptr)  \
    TypedData_Get_Struct((obj), rb_context_t, &cont_data_type, (ptr))

#define GetFiberPtr(obj, ptr)  do {\
    TypedData_Get_Struct((obj), rb_fiber_t, &fiber_data_type, (ptr)); \
    if (!(ptr)) rb_raise(rb_eFiberError, "uninitialized fiber"); \
} while(0)

NOINLINE(static VALUE cont_capture(volatile int *stat));

#define THREAD_MUST_BE_RUNNING(th) do { \
	if (!(th)->tag) rb_raise(rb_eThreadError, "not running thread");	\
    } while (0)

static void
cont_mark(void *ptr)
{
    RUBY_MARK_ENTER("cont");
    if (ptr) {
	rb_context_t *cont = ptr;
	rb_gc_mark(cont->value);
	rb_thread_mark(&cont->saved_thread);
	rb_gc_mark(cont->saved_thread.self);

	if (cont->vm_stack) {
#ifdef CAPTURE_JUST_VALID_VM_STACK
	    rb_gc_mark_locations(cont->vm_stack,
				 cont->vm_stack + cont->vm_stack_slen + cont->vm_stack_clen);
#else
	    rb_gc_mark_localtion(cont->vm_stack,
				 cont->vm_stack, cont->saved_thread.stack_size);
#endif
	}

	if (cont->machine_stack) {
	    if (cont->type == CONTINUATION_CONTEXT) {
		/* cont */
		rb_gc_mark_locations(cont->machine_stack,
				     cont->machine_stack + cont->machine_stack_size);
            }
            else {
		/* fiber */
		rb_thread_t *th;
                rb_fiber_t *fib = (rb_fiber_t*)cont;
		GetThreadPtr(cont->saved_thread.self, th);
		if ((th->fiber != cont->self) && fib->status == RUNNING) {
		    rb_gc_mark_locations(cont->machine_stack,
					 cont->machine_stack + cont->machine_stack_size);
		}
	    }
	}
#ifdef __ia64
	if (cont->machine_register_stack) {
	    rb_gc_mark_locations(cont->machine_register_stack,
				 cont->machine_register_stack + cont->machine_register_stack_size);
	}
#endif
    }
    RUBY_MARK_LEAVE("cont");
}

static void
cont_free(void *ptr)
{
    RUBY_FREE_ENTER("cont");
    if (ptr) {
	rb_context_t *cont = ptr;
	RUBY_FREE_UNLESS_NULL(cont->saved_thread.stack); fflush(stdout);
#if FIBER_USE_NATIVE
	if (cont->type == CONTINUATION_CONTEXT) {
	    /* cont */
	    RUBY_FREE_UNLESS_NULL(cont->machine_stack);
	}
	else {
	    /* fiber */
#ifdef _WIN32
	    if (GET_THREAD()->fiber != cont->self && cont->type != ROOT_FIBER_CONTEXT) {
		/* don't delete root fiber handle */
		rb_fiber_t *fib = (rb_fiber_t*)cont;
		if (fib->fib_handle) {
		    DeleteFiber(fib->fib_handle);
		}
	    }
#else /* not WIN32 */
	    if (GET_THREAD()->fiber != cont->self) {
                rb_fiber_t *fib = (rb_fiber_t*)cont;
                if (fib->context.uc_stack.ss_sp) {
                    if (cont->type == ROOT_FIBER_CONTEXT) {
			rb_bug("Illegal root fiber parameter");
                    }
		    munmap((void*)fib->context.uc_stack.ss_sp, fib->context.uc_stack.ss_size);
		}
	    }
            else {
		/* It may reached here when finalize */
		/* TODO examine whether it is a bug */
                /* rb_bug("cont_free: release self"); */
            }
#endif
	}
#else /* not FIBER_USE_NATIVE */
	RUBY_FREE_UNLESS_NULL(cont->machine_stack);
#endif
#ifdef __ia64
	RUBY_FREE_UNLESS_NULL(cont->machine_register_stack);
#endif
	RUBY_FREE_UNLESS_NULL(cont->vm_stack);

	/* free rb_cont_t or rb_fiber_t */
	ruby_xfree(ptr);
    }
    RUBY_FREE_LEAVE("cont");
}

static size_t
cont_memsize(const void *ptr)
{
    const rb_context_t *cont = ptr;
    size_t size = 0;
    if (cont) {
	size = sizeof(*cont);
	if (cont->vm_stack) {
#ifdef CAPTURE_JUST_VALID_VM_STACK
	    size_t n = (cont->vm_stack_slen + cont->vm_stack_clen);
#else
	    size_t n = cont->saved_thread.stack_size;
#endif
	    size += n * sizeof(*cont->vm_stack);
	}

	if (cont->machine_stack) {
	    size += cont->machine_stack_size * sizeof(*cont->machine_stack);
	}
#ifdef __ia64
	if (cont->machine_register_stack) {
	    size += cont->machine_register_stack_size * sizeof(*cont->machine_register_stack);
	}
#endif
    }
    return size;
}

static void
fiber_mark(void *ptr)
{
    RUBY_MARK_ENTER("cont");
    if (ptr) {
	rb_fiber_t *fib = ptr;
	rb_gc_mark(fib->prev);
	cont_mark(&fib->cont);
    }
    RUBY_MARK_LEAVE("cont");
}

static void
fiber_link_join(rb_fiber_t *fib)
{
    VALUE current_fibval = rb_fiber_current();
    rb_fiber_t *current_fib;
    GetFiberPtr(current_fibval, current_fib);

    /* join fiber link */
    fib->next_fiber = current_fib->next_fiber;
    fib->prev_fiber = current_fib;
    current_fib->next_fiber->prev_fiber = fib;
    current_fib->next_fiber = fib;
}

static void
fiber_link_remove(rb_fiber_t *fib)
{
    fib->prev_fiber->next_fiber = fib->next_fiber;
    fib->next_fiber->prev_fiber = fib->prev_fiber;
}

static void
fiber_free(void *ptr)
{
    RUBY_FREE_ENTER("fiber");
    if (ptr) {
	rb_fiber_t *fib = ptr;
	if (fib->cont.type != ROOT_FIBER_CONTEXT &&
	    fib->cont.saved_thread.local_storage) {
	    st_free_table(fib->cont.saved_thread.local_storage);
	}
	fiber_link_remove(fib);

	cont_free(&fib->cont);
    }
    RUBY_FREE_LEAVE("fiber");
}

static size_t
fiber_memsize(const void *ptr)
{
    const rb_fiber_t *fib = ptr;
    size_t size = 0;
    if (ptr) {
	size = sizeof(*fib);
	if (fib->cont.type != ROOT_FIBER_CONTEXT) {
	    size += st_memsize(fib->cont.saved_thread.local_storage);
	}
	size += cont_memsize(&fib->cont);
    }
    return size;
}

VALUE
rb_obj_is_fiber(VALUE obj)
{
    if (rb_typeddata_is_kind_of(obj, &fiber_data_type)) {
	return Qtrue;
    }
    else {
	return Qfalse;
    }
}

static void
cont_save_machine_stack(rb_thread_t *th, rb_context_t *cont)
{
    size_t size;

    SET_MACHINE_STACK_END(&th->machine_stack_end);
#ifdef __ia64
    th->machine_register_stack_end = rb_ia64_bsp();
#endif

    if (th->machine_stack_start > th->machine_stack_end) {
	size = cont->machine_stack_size = th->machine_stack_start - th->machine_stack_end;
	cont->machine_stack_src = th->machine_stack_end;
    }
    else {
	size = cont->machine_stack_size = th->machine_stack_end - th->machine_stack_start;
	cont->machine_stack_src = th->machine_stack_start;
    }

    if (cont->machine_stack) {
	REALLOC_N(cont->machine_stack, VALUE, size);
    }
    else {
	cont->machine_stack = ALLOC_N(VALUE, size);
    }

    FLUSH_REGISTER_WINDOWS;
    MEMCPY(cont->machine_stack, cont->machine_stack_src, VALUE, size);

#ifdef __ia64
    rb_ia64_flushrs();
    size = cont->machine_register_stack_size = th->machine_register_stack_end - th->machine_register_stack_start;
    cont->machine_register_stack_src = th->machine_register_stack_start;
    if (cont->machine_register_stack) {
	REALLOC_N(cont->machine_register_stack, VALUE, size);
    }
    else {
	cont->machine_register_stack = ALLOC_N(VALUE, size);
    }

    MEMCPY(cont->machine_register_stack, cont->machine_register_stack_src, VALUE, size);
#endif
}

static const rb_data_type_t cont_data_type = {
    "continuation",
    {cont_mark, cont_free, cont_memsize,},
};

static void
cont_save_thread(rb_context_t *cont, rb_thread_t *th)
{
    /* save thread context */
    cont->saved_thread = *th;
    /* saved_thread->machine_stack_(start|end) should be NULL */
    /* because it may happen GC afterward */
    cont->saved_thread.machine_stack_start = 0;
    cont->saved_thread.machine_stack_end = 0;
#ifdef __ia64
    cont->saved_thread.machine_register_stack_start = 0;
    cont->saved_thread.machine_register_stack_end = 0;
#endif
}

static void
cont_init(rb_context_t *cont, rb_thread_t *th)
{
    /* save thread context */
    cont_save_thread(cont, th);
    cont->saved_thread.local_storage = 0;
}

static rb_context_t *
cont_new(VALUE klass)
{
    rb_context_t *cont;
    volatile VALUE contval;
    rb_thread_t *th = GET_THREAD();

    THREAD_MUST_BE_RUNNING(th);
    contval = TypedData_Make_Struct(klass, rb_context_t, &cont_data_type, cont);
    cont->self = contval;
    cont_init(cont, th);
    return cont;
}

static VALUE
cont_capture(volatile int *stat)
{
    rb_context_t *cont;
    rb_thread_t *th = GET_THREAD(), *sth;
    volatile VALUE contval;

    THREAD_MUST_BE_RUNNING(th);
    rb_vm_stack_to_heap(th);
    cont = cont_new(rb_cContinuation);
    contval = cont->self;
    sth = &cont->saved_thread;

#ifdef CAPTURE_JUST_VALID_VM_STACK
    cont->vm_stack_slen = th->cfp->sp + th->mark_stack_len - th->stack;
    cont->vm_stack_clen = th->stack + th->stack_size - (VALUE*)th->cfp;
    cont->vm_stack = ALLOC_N(VALUE, cont->vm_stack_slen + cont->vm_stack_clen);
    MEMCPY(cont->vm_stack, th->stack, VALUE, cont->vm_stack_slen);
    MEMCPY(cont->vm_stack + cont->vm_stack_slen, (VALUE*)th->cfp, VALUE, cont->vm_stack_clen);
#else
    cont->vm_stack = ALLOC_N(VALUE, th->stack_size);
    MEMCPY(cont->vm_stack, th->stack, VALUE, th->stack_size);
#endif
    sth->stack = 0;

    cont_save_machine_stack(th, cont);

    if (ruby_setjmp(cont->jmpbuf)) {
	volatile VALUE value;

	value = cont->value;
	if (cont->argc == -1) rb_exc_raise(value);
	cont->value = Qnil;
	*stat = 1;
	return value;
    }
    else {
	*stat = 0;
	return contval;
    }
}

static void
cont_restore_thread(rb_context_t *cont)
{
    rb_thread_t *th = GET_THREAD(), *sth = &cont->saved_thread;

    /* restore thread context */
    if (cont->type == CONTINUATION_CONTEXT) {
	/* continuation */
	VALUE fib;

	th->fiber = sth->fiber;
	fib = th->fiber ? th->fiber : th->root_fiber;

	if (fib) {
	    rb_fiber_t *fcont;
	    GetFiberPtr(fib, fcont);
	    th->stack_size = fcont->cont.saved_thread.stack_size;
	    th->stack = fcont->cont.saved_thread.stack;
	}
#ifdef CAPTURE_JUST_VALID_VM_STACK
	MEMCPY(th->stack, cont->vm_stack, VALUE, cont->vm_stack_slen);
	MEMCPY(th->stack + sth->stack_size - cont->vm_stack_clen,
	       cont->vm_stack + cont->vm_stack_slen, VALUE, cont->vm_stack_clen);
#else
	MEMCPY(th->stack, cont->vm_stack, VALUE, sth->stack_size);
#endif
    }
    else {
	/* fiber */
	th->stack = sth->stack;
	th->stack_size = sth->stack_size;
	th->local_storage = sth->local_storage;
	th->fiber = cont->self;
    }

    th->cfp = sth->cfp;
    th->safe_level = sth->safe_level;
    th->raised_flag = sth->raised_flag;
    th->state = sth->state;
    th->status = sth->status;
    th->tag = sth->tag;
    th->protect_tag = sth->protect_tag;
    th->errinfo = sth->errinfo;
    th->first_proc = sth->first_proc;
}

#if FIBER_USE_NATIVE
#ifdef _WIN32
static void
fiber_set_stack_location(void)
{
    rb_thread_t *th = GET_THREAD();
    VALUE *ptr;

    SET_MACHINE_STACK_END(&ptr);
    th->machine_stack_start = (void*)(((VALUE)ptr & RB_PAGE_MASK) + STACK_UPPER((void *)&ptr, 0, RB_PAGE_SIZE));
}

static VOID CALLBACK
fiber_entry(void *arg)
{
    fiber_set_stack_location();
    rb_fiber_start();
}
#else /* _WIN32 */

/*
 * FreeBSD require a first (i.e. addr) argument of mmap(2) is not NULL
 * if MAP_STACK is passed.
 * http://www.FreeBSD.org/cgi/query-pr.cgi?pr=158755
 */
#if defined(MAP_STACK) && !defined(__FreeBSD__) && !defined(__FreeBSD_kernel__)
#define FIBER_STACK_FLAGS (MAP_PRIVATE | MAP_ANON | MAP_STACK)
#else
#define FIBER_STACK_FLAGS (MAP_PRIVATE | MAP_ANON)
#endif

static char*
fiber_machine_stack_alloc(size_t size)
{
    char *ptr;

    if (machine_stack_cache_index > 0) {
	if (machine_stack_cache[machine_stack_cache_index - 1].size == (size / sizeof(VALUE))) {
	    ptr = machine_stack_cache[machine_stack_cache_index - 1].ptr;
	    machine_stack_cache_index--;
	    machine_stack_cache[machine_stack_cache_index].ptr = NULL;
	    machine_stack_cache[machine_stack_cache_index].size = 0;
	}
	else{
            /* TODO handle multiple machine stack size */
	    rb_bug("machine_stack_cache size is not canonicalized");
	}
    }
    else {
	void *page;
	STACK_GROW_DIR_DETECTION;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, FIBER_STACK_FLAGS, -1, 0);
	if (ptr == MAP_FAILED) {
	    rb_raise(rb_eFiberError, "can't alloc machine stack to fiber");
	}

	/* guard page setup */
	page = ptr + STACK_DIR_UPPER(size - RB_PAGE_SIZE, 0);
	if (mprotect(page, RB_PAGE_SIZE, PROT_NONE) < 0) {
	    rb_raise(rb_eFiberError, "mprotect failed");
	}
    }

    return ptr;
}
#endif

static void
fiber_initialize_machine_stack_context(rb_fiber_t *fib, size_t size)
{
    rb_thread_t *sth = &fib->cont.saved_thread;

#ifdef _WIN32
    fib->fib_handle = CreateFiberEx(size - 1, size, 0, fiber_entry, NULL);
    if (!fib->fib_handle) {
	/* try to release unnecessary fibers & retry to create */
	rb_gc();
	fib->fib_handle = CreateFiberEx(size - 1, size, 0, fiber_entry, NULL);
	if (!fib->fib_handle) {
	    rb_raise(rb_eFiberError, "can't create fiber");
	}
    }
    sth->machine_stack_maxsize = size;
#else /* not WIN32 */
    ucontext_t *context = &fib->context;
    char *ptr;
    STACK_GROW_DIR_DETECTION;

    getcontext(context);
    ptr = fiber_machine_stack_alloc(size);
    context->uc_link = NULL;
    context->uc_stack.ss_sp = ptr;
    context->uc_stack.ss_size = size;
    makecontext(context, rb_fiber_start, 0);
    sth->machine_stack_start = (VALUE*)(ptr + STACK_DIR_UPPER(0, size));
    sth->machine_stack_maxsize = size - RB_PAGE_SIZE;
#endif
#ifdef __ia64
    sth->machine_register_stack_maxsize = sth->machine_stack_maxsize;
#endif
}

NOINLINE(static void fiber_setcontext(rb_fiber_t *newfib, rb_fiber_t *oldfib));

static void
fiber_setcontext(rb_fiber_t *newfib, rb_fiber_t *oldfib)
{
    rb_thread_t *th = GET_THREAD(), *sth = &newfib->cont.saved_thread;

    if (newfib->status != RUNNING) {
	fiber_initialize_machine_stack_context(newfib, FIBER_MACHINE_STACK_ALLOCATION_SIZE);
    }

    /* restore thread context */
    cont_restore_thread(&newfib->cont);
    th->machine_stack_maxsize = sth->machine_stack_maxsize;
    if (sth->machine_stack_end && (newfib != oldfib)) {
	rb_bug("fiber_setcontext: sth->machine_stack_end has non zero value");
    }

    /* save  oldfib's machine stack */
    if (oldfib->status != TERMINATED) {
	STACK_GROW_DIR_DETECTION;
	SET_MACHINE_STACK_END(&th->machine_stack_end);
	if (STACK_DIR_UPPER(0, 1)) {
	    oldfib->cont.machine_stack_size = th->machine_stack_start - th->machine_stack_end;
	    oldfib->cont.machine_stack = th->machine_stack_end;
	}
	else {
	    oldfib->cont.machine_stack_size = th->machine_stack_end - th->machine_stack_start;
	    oldfib->cont.machine_stack = th->machine_stack_start;
	}
    }
    /* exchange machine_stack_start between oldfib and newfib */
    oldfib->cont.saved_thread.machine_stack_start = th->machine_stack_start;
    th->machine_stack_start = sth->machine_stack_start;
    /* oldfib->machine_stack_end should be NULL */
    oldfib->cont.saved_thread.machine_stack_end = 0;
#ifndef _WIN32
    if (!newfib->context.uc_stack.ss_sp && th->root_fiber != newfib->cont.self) {
	rb_bug("non_root_fiber->context.uc_stac.ss_sp should not be NULL");
    }
#endif

    /* swap machine context */
#ifdef _WIN32
    SwitchToFiber(newfib->fib_handle);
#else
    swapcontext(&oldfib->context, &newfib->context);
#endif
}
#endif

NOINLINE(NORETURN(static void cont_restore_1(rb_context_t *)));

static void
cont_restore_1(rb_context_t *cont)
{
    cont_restore_thread(cont);

    /* restore machine stack */
#ifdef _M_AMD64
    {
	/* workaround for x64 SEH */
	jmp_buf buf;
	setjmp(buf);
	((_JUMP_BUFFER*)(&cont->jmpbuf))->Frame =
	    ((_JUMP_BUFFER*)(&buf))->Frame;
    }
#endif
    if (cont->machine_stack_src) {
	FLUSH_REGISTER_WINDOWS;
	MEMCPY(cont->machine_stack_src, cont->machine_stack,
		VALUE, cont->machine_stack_size);
    }

#ifdef __ia64
    if (cont->machine_register_stack_src) {
	MEMCPY(cont->machine_register_stack_src, cont->machine_register_stack,
	       VALUE, cont->machine_register_stack_size);
    }
#endif

    ruby_longjmp(cont->jmpbuf, 1);
}

NORETURN(NOINLINE(static void cont_restore_0(rb_context_t *, VALUE *)));

#ifdef __ia64
#define C(a) rse_##a##0, rse_##a##1, rse_##a##2, rse_##a##3, rse_##a##4
#define E(a) rse_##a##0= rse_##a##1= rse_##a##2= rse_##a##3= rse_##a##4
static volatile int C(a), C(b), C(c), C(d), C(e);
static volatile int C(f), C(g), C(h), C(i), C(j);
static volatile int C(k), C(l), C(m), C(n), C(o);
static volatile int C(p), C(q), C(r), C(s), C(t);
#if 0
{/* the above lines make cc-mode.el confused so much */}
#endif
int rb_dummy_false = 0;
NORETURN(NOINLINE(static void register_stack_extend(rb_context_t *, VALUE *, VALUE *)));
static void
register_stack_extend(rb_context_t *cont, VALUE *vp, VALUE *curr_bsp)
{
    if (rb_dummy_false) {
        /* use registers as much as possible */
        E(a) = E(b) = E(c) = E(d) = E(e) =
        E(f) = E(g) = E(h) = E(i) = E(j) =
        E(k) = E(l) = E(m) = E(n) = E(o) =
        E(p) = E(q) = E(r) = E(s) = E(t) = 0;
        E(a) = E(b) = E(c) = E(d) = E(e) =
        E(f) = E(g) = E(h) = E(i) = E(j) =
        E(k) = E(l) = E(m) = E(n) = E(o) =
        E(p) = E(q) = E(r) = E(s) = E(t) = 0;
    }
    if (curr_bsp < cont->machine_register_stack_src+cont->machine_register_stack_size) {
        register_stack_extend(cont, vp, (VALUE*)rb_ia64_bsp());
    }
    cont_restore_0(cont, vp);
}
#undef C
#undef E
#endif

static void
cont_restore_0(rb_context_t *cont, VALUE *addr_in_prev_frame)
{
    if (cont->machine_stack_src) {
#ifdef HAVE_ALLOCA
#define STACK_PAD_SIZE 1
#else
#define STACK_PAD_SIZE 1024
#endif
	VALUE space[STACK_PAD_SIZE];

#if !STACK_GROW_DIRECTION
	if (addr_in_prev_frame > &space[0]) {
	    /* Stack grows downward */
#endif
#if STACK_GROW_DIRECTION <= 0
	    volatile VALUE *const end = cont->machine_stack_src;
	    if (&space[0] > end) {
# ifdef HAVE_ALLOCA
		volatile VALUE *sp = ALLOCA_N(VALUE, &space[0] - end);
		space[0] = *sp;
# else
		cont_restore_0(cont, &space[0]);
# endif
	    }
#endif
#if !STACK_GROW_DIRECTION
	}
	else {
	    /* Stack grows upward */
#endif
#if STACK_GROW_DIRECTION >= 0
	    volatile VALUE *const end = cont->machine_stack_src + cont->machine_stack_size;
	    if (&space[STACK_PAD_SIZE] < end) {
# ifdef HAVE_ALLOCA
		volatile VALUE *sp = ALLOCA_N(VALUE, end - &space[STACK_PAD_SIZE]);
		space[0] = *sp;
# else
		cont_restore_0(cont, &space[STACK_PAD_SIZE-1]);
# endif
	    }
#endif
#if !STACK_GROW_DIRECTION
	}
#endif
    }
    cont_restore_1(cont);
}
#ifdef __ia64
#define cont_restore_0(cont, vp) register_stack_extend((cont), (vp), (VALUE*)rb_ia64_bsp());
#endif

/*
 *  Document-class: Continuation
 *
 *  Continuation objects are generated by <code>Kernel#callcc</code>,
 *  after having <code>require</code>d <i>continuation</i>. They hold
 *  a return address and execution context, allowing a nonlocal return
 *  to the end of the <code>callcc</code> block from anywhere within a
 *  program.  Continuations are somewhat analogous to a structured
 *  version of C's <code>setjmp/longjmp</code> (although they contain
 *  more state, so you might consider them closer to threads).
 *
 *  For instance:
 *
 *     require "continuation"
 *     arr = [ "Freddie", "Herbie", "Ron", "Max", "Ringo" ]
 *     callcc{|cc| $cc = cc}
 *     puts(message = arr.shift)
 *     $cc.call unless message =~ /Max/
 *
 *  <em>produces:</em>
 *
 *     Freddie
 *     Herbie
 *     Ron
 *     Max
 *
 *  This (somewhat contrived) example allows the inner loop to abandon
 *  processing early:
 *
 *     require "continuation"
 *     callcc {|cont|
 *       for i in 0..4
 *         print "\n#{i}: "
 *         for j in i*5...(i+1)*5
 *           cont.call() if j == 17
 *           printf "%3d", j
 *         end
 *       end
 *     }
 *     puts
 *
 *  <em>produces:</em>
 *
 *     0:   0  1  2  3  4
 *     1:   5  6  7  8  9
 *     2:  10 11 12 13 14
 *     3:  15 16
 */

/*
 *  call-seq:
 *     callcc {|cont| block }   ->  obj
 *
 *  Generates a <code>Continuation</code> object, which it passes to
 *  the associated block. You need to <code>require
 *  'continuation'</code> before using this method. Performing a
 *  <em>cont</em><code>.call</code> will cause the <code>callcc</code>
 *  to return (as will falling through the end of the block). The
 *  value returned by the <code>callcc</code> is the value of the
 *  block, or the value passed to <em>cont</em><code>.call</code>. See
 *  class <code>Continuation</code> for more details. Also see
 *  <code>Kernel::throw</code> for an alternative mechanism for
 *  unwinding a call stack.
 */

static VALUE
rb_callcc(VALUE self)
{
    volatile int called;
    volatile VALUE val = cont_capture(&called);

    if (called) {
	return val;
    }
    else {
	return rb_yield(val);
    }
}

static VALUE
make_passing_arg(int argc, VALUE *argv)
{
    switch(argc) {
      case 0:
	return Qnil;
      case 1:
	return argv[0];
      default:
	return rb_ary_new4(argc, argv);
    }
}

/*
 *  call-seq:
 *     cont.call(args, ...)
 *     cont[args, ...]
 *
 *  Invokes the continuation. The program continues from the end of the
 *  <code>callcc</code> block. If no arguments are given, the original
 *  <code>callcc</code> returns <code>nil</code>. If one argument is
 *  given, <code>callcc</code> returns it. Otherwise, an array
 *  containing <i>args</i> is returned.
 *
 *     callcc {|cont|  cont.call }           #=> nil
 *     callcc {|cont|  cont.call 1 }         #=> 1
 *     callcc {|cont|  cont.call 1, 2, 3 }   #=> [1, 2, 3]
 */

static VALUE
rb_cont_call(int argc, VALUE *argv, VALUE contval)
{
    rb_context_t *cont;
    rb_thread_t *th = GET_THREAD();
    GetContPtr(contval, cont);

    if (cont->saved_thread.self != th->self) {
	rb_raise(rb_eRuntimeError, "continuation called across threads");
    }
    if (cont->saved_thread.protect_tag != th->protect_tag) {
	rb_raise(rb_eRuntimeError, "continuation called across stack rewinding barrier");
    }
    if (cont->saved_thread.fiber) {
	rb_fiber_t *fcont;
	GetFiberPtr(cont->saved_thread.fiber, fcont);

	if (th->fiber != cont->saved_thread.fiber) {
	    rb_raise(rb_eRuntimeError, "continuation called across fiber");
	}
    }

    cont->argc = argc;
    cont->value = make_passing_arg(argc, argv);

    cont_restore_0(cont, &contval);
    return Qnil; /* unreachable */
}

/*********/
/* fiber */
/*********/

/*
 *  Document-class: Fiber
 *
 *  Fibers are primitives for implementing light weight cooperative
 *  concurrency in Ruby. Basically they are a means of creating code blocks
 *  that can be paused and resumed, much like threads. The main difference
 *  is that they are never preempted and that the scheduling must be done by
 *  the programmer and not the VM.
 *
 *  As opposed to other stackless light weight concurrency models, each fiber
 *  comes with a small 4KB stack. This enables the fiber to be paused from deeply
 *  nested function calls within the fiber block.
 *
 *  When a fiber is created it will not run automatically. Rather it must be
 *  be explicitly asked to run using the <code>Fiber#resume</code> method.
 *  The code running inside the fiber can give up control by calling
 *  <code>Fiber.yield</code> in which case it yields control back to caller
 *  (the caller of the <code>Fiber#resume</code>).
 *
 *  Upon yielding or termination the Fiber returns the value of the last
 *  executed expression
 *
 *  For instance:
 *
 *    fiber = Fiber.new do
 *      Fiber.yield 1
 *      2
 *    end
 *
 *    puts fiber.resume
 *    puts fiber.resume
 *    puts fiber.resume
 *
 *  <em>produces</em>
 *
 *    1
 *    2
 *    FiberError: dead fiber called
 *
 *  The <code>Fiber#resume</code> method accepts an arbitrary number of
 *  parameters, if it is the first call to <code>resume</code> then they
 *  will be passed as block arguments. Otherwise they will be the return
 *  value of the call to <code>Fiber.yield</code>
 *
 *  Example:
 *
 *    fiber = Fiber.new do |first|
 *      second = Fiber.yield first + 2
 *    end
 *
 *    puts fiber.resume 10
 *    puts fiber.resume 14
 *    puts fiber.resume 18
 *
 *  <em>produces</em>
 *
 *    12
 *    14
 *    FiberError: dead fiber called
 *
 */

#define FIBER_VM_STACK_SIZE (4 * 1024)

static const rb_data_type_t fiber_data_type = {
    "fiber",
    {fiber_mark, fiber_free, fiber_memsize,},
};

static VALUE
fiber_alloc(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &fiber_data_type, 0);
}

static rb_fiber_t*
fiber_t_alloc(VALUE fibval)
{
    rb_fiber_t *fib;
    rb_thread_t *th = GET_THREAD();

    if (DATA_PTR(fibval) != 0) {
	rb_raise(rb_eRuntimeError, "cannot initialize twice");
    }

    THREAD_MUST_BE_RUNNING(th);
    fib = ALLOC(rb_fiber_t);
    memset(fib, 0, sizeof(rb_fiber_t));
    fib->cont.self = fibval;
    fib->cont.type = FIBER_CONTEXT;
    cont_init(&fib->cont, th);
    fib->prev = Qnil;
    fib->status = CREATED;

    DATA_PTR(fibval) = fib;

    return fib;
}

static VALUE
fiber_init(VALUE fibval, VALUE proc)
{
    rb_fiber_t *fib = fiber_t_alloc(fibval);
    rb_context_t *cont = &fib->cont;
    rb_thread_t *th = &cont->saved_thread;

    /* initialize cont */
    cont->vm_stack = 0;

    th->stack = 0;
    th->stack_size = 0;

    fiber_link_join(fib);

    th->stack_size = FIBER_VM_STACK_SIZE;
    th->stack = ALLOC_N(VALUE, th->stack_size);

    th->cfp = (void *)(th->stack + th->stack_size);
    th->cfp--;
    th->cfp->pc = 0;
    th->cfp->sp = th->stack + 1;
    th->cfp->bp = 0;
    th->cfp->lfp = th->stack;
    *th->cfp->lfp = 0;
    th->cfp->dfp = th->stack;
    th->cfp->self = Qnil;
    th->cfp->flag = 0;
    th->cfp->iseq = 0;
    th->cfp->proc = 0;
    th->cfp->block_iseq = 0;
    th->cfp->me = 0;
    th->tag = 0;
    th->local_storage = st_init_numtable();

    th->first_proc = proc;

#if !FIBER_USE_NATIVE
    MEMCPY(&cont->jmpbuf, &th->root_jmpbuf, rb_jmpbuf_t, 1);
#endif

    return fibval;
}

/* :nodoc: */
static VALUE
rb_fiber_init(VALUE fibval)
{
    return fiber_init(fibval, rb_block_proc());
}

VALUE
rb_fiber_new(VALUE (*func)(ANYARGS), VALUE obj)
{
    return fiber_init(fiber_alloc(rb_cFiber), rb_proc_new(func, obj));
}

static VALUE
return_fiber(void)
{
    rb_fiber_t *fib;
    VALUE curr = rb_fiber_current();
    GetFiberPtr(curr, fib);

    if (fib->prev == Qnil) {
	rb_thread_t *th = GET_THREAD();

	if (th->root_fiber != curr) {
	    return th->root_fiber;
	}
	else {
	    rb_raise(rb_eFiberError, "can't yield from root fiber");
	}
    }
    else {
	VALUE prev = fib->prev;
	fib->prev = Qnil;
	return prev;
    }
}

VALUE rb_fiber_transfer(VALUE fib, int argc, VALUE *argv);

static void
rb_fiber_terminate(rb_fiber_t *fib)
{
    VALUE value = fib->cont.value;
    fib->status = TERMINATED;
#if FIBER_USE_NATIVE && !defined(_WIN32)
    /* Ruby must not switch to other thread until storing terminated_machine_stack */
    terminated_machine_stack.ptr = fib->context.uc_stack.ss_sp;
    terminated_machine_stack.size = fib->context.uc_stack.ss_size / sizeof(VALUE);
    fib->context.uc_stack.ss_sp = NULL;
    fib->cont.machine_stack = NULL;
    fib->cont.machine_stack_size = 0;
#endif
    rb_fiber_transfer(return_fiber(), 1, &value);
}

void
rb_fiber_start(void)
{
    rb_thread_t *th = GET_THREAD();
    rb_fiber_t *fib;
    rb_context_t *cont;
    rb_proc_t *proc;
    int state;

    GetFiberPtr(th->fiber, fib);
    cont = &fib->cont;

    TH_PUSH_TAG(th);
    if ((state = EXEC_TAG()) == 0) {
	int argc;
	VALUE *argv, args;
	GetProcPtr(cont->saved_thread.first_proc, proc);
	args = cont->value;
	argv = (argc = cont->argc) > 1 ? RARRAY_PTR(args) : &args;
	cont->value = Qnil;
	th->errinfo = Qnil;
	th->local_lfp = proc->block.lfp;
	th->local_svar = Qnil;

	fib->status = RUNNING;
	cont->value = rb_vm_invoke_proc(th, proc, proc->block.self, argc, argv, 0);
    }
    TH_POP_TAG();

    if (state) {
	if (state == TAG_RAISE) {
	    th->thrown_errinfo = th->errinfo;
	}
	else {
	    th->thrown_errinfo =
	      rb_vm_make_jump_tag_but_local_jump(state, th->errinfo);
	}
	RUBY_VM_SET_INTERRUPT(th);
    }

    rb_fiber_terminate(fib);
    rb_bug("rb_fiber_start: unreachable");
}

static rb_fiber_t *
root_fiber_alloc(rb_thread_t *th)
{
    rb_fiber_t *fib;
    /* no need to allocate vm stack */
    fib = fiber_t_alloc(fiber_alloc(rb_cFiber));
    fib->cont.type = ROOT_FIBER_CONTEXT;
#if FIBER_USE_NATIVE
#ifdef _WIN32
    fib->fib_handle = ConvertThreadToFiber(0);
#endif
#endif
    fib->status = RUNNING;
    fib->prev_fiber = fib->next_fiber = fib;

    return fib;
}

VALUE
rb_fiber_current(void)
{
    rb_thread_t *th = GET_THREAD();
    if (th->fiber == 0) {
	/* save root */
	rb_fiber_t *fib = root_fiber_alloc(th);
	th->root_fiber = th->fiber = fib->cont.self;
    }
    return th->fiber;
}

static VALUE
fiber_store(rb_fiber_t *next_fib)
{
    rb_thread_t *th = GET_THREAD();
    rb_fiber_t *fib;

    if (th->fiber) {
	GetFiberPtr(th->fiber, fib);
	cont_save_thread(&fib->cont, th);
    }
    else {
	/* create current fiber */
	fib = root_fiber_alloc(th);
	th->root_fiber = th->fiber = fib->cont.self;
    }

#if !FIBER_USE_NATIVE
    cont_save_machine_stack(th, &fib->cont);
#endif

    if (FIBER_USE_NATIVE || ruby_setjmp(fib->cont.jmpbuf)) {
#if FIBER_USE_NATIVE
	fiber_setcontext(next_fib, fib);
#ifndef _WIN32
	if (terminated_machine_stack.ptr) {
	    if (machine_stack_cache_index < MAX_MAHINE_STACK_CACHE) {
		machine_stack_cache[machine_stack_cache_index].ptr = terminated_machine_stack.ptr;
		machine_stack_cache[machine_stack_cache_index].size = terminated_machine_stack.size;
		machine_stack_cache_index++;
	    }
	    else {
		if (terminated_machine_stack.ptr != fib->cont.machine_stack) {
		    munmap((void*)terminated_machine_stack.ptr, terminated_machine_stack.size * sizeof(VALUE));
		}
		else {
		    rb_bug("terminated fiber resumed");
		}
	    }
	    terminated_machine_stack.ptr = NULL;
	    terminated_machine_stack.size = 0;
	}
#endif
#endif
	/* restored */
	GetFiberPtr(th->fiber, fib);
	if (fib->cont.argc == -1) rb_exc_raise(fib->cont.value);
	return fib->cont.value;
    }
#if !FIBER_USE_NATIVE
    else {
	return Qundef;
    }
#endif
}

static inline VALUE
fiber_switch(VALUE fibval, int argc, VALUE *argv, int is_resume)
{
    VALUE value;
    rb_fiber_t *fib;
    rb_context_t *cont;
    rb_thread_t *th = GET_THREAD();

    GetFiberPtr(fibval, fib);
    cont = &fib->cont;

    if (th->fiber == fibval) {
	/* ignore fiber context switch
         * because destination fiber is same as current fiber
	 */
	return make_passing_arg(argc, argv);
    }

    if (cont->saved_thread.self != th->self) {
	rb_raise(rb_eFiberError, "fiber called across threads");
    }
    else if (cont->saved_thread.protect_tag != th->protect_tag) {
	rb_raise(rb_eFiberError, "fiber called across stack rewinding barrier");
    }
    else if (fib->status == TERMINATED) {
	value = rb_exc_new2(rb_eFiberError, "dead fiber called");
	if (th->fiber != fibval) {
	    GetFiberPtr(th->fiber, fib);
	    if (fib->status != TERMINATED) rb_exc_raise(value);
	    fibval = th->root_fiber;
	}
	else {
	    fibval = fib->prev;
	    if (NIL_P(fibval)) fibval = th->root_fiber;
	}
	GetFiberPtr(fibval, fib);
	cont = &fib->cont;
	cont->argc = -1;
	cont->value = value;
#if FIBER_USE_NATIVE
	{
	    VALUE oldfibval;
	    rb_fiber_t *oldfib;
	    oldfibval = rb_fiber_current();
	    GetFiberPtr(oldfibval, oldfib);
	    fiber_setcontext(fib, oldfib);
	}
#else
	cont_restore_0(cont, &value);
#endif
    }

    if (is_resume) {
	fib->prev = rb_fiber_current();
    }

    cont->argc = argc;
    cont->value = make_passing_arg(argc, argv);

    value = fiber_store(fib);
#if !FIBER_USE_NATIVE
    if (value == Qundef) {
	cont_restore_0(cont, &value);
	rb_bug("rb_fiber_resume: unreachable");
    }
#endif
    RUBY_VM_CHECK_INTS();

    return value;
}

VALUE
rb_fiber_transfer(VALUE fib, int argc, VALUE *argv)
{
    return fiber_switch(fib, argc, argv, 0);
}

VALUE
rb_fiber_resume(VALUE fibval, int argc, VALUE *argv)
{
    rb_fiber_t *fib;
    GetFiberPtr(fibval, fib);

    if (fib->prev != Qnil || fib->cont.type == ROOT_FIBER_CONTEXT) {
	rb_raise(rb_eFiberError, "double resume");
    }
    if (fib->transfered != 0) {
	rb_raise(rb_eFiberError, "cannot resume transferred Fiber");
    }

    return fiber_switch(fibval, argc, argv, 1);
}

VALUE
rb_fiber_yield(int argc, VALUE *argv)
{
    return rb_fiber_transfer(return_fiber(), argc, argv);
}

void
rb_fiber_reset_root_local_storage(VALUE thval)
{
    rb_thread_t *th;
    rb_fiber_t	*fib;

    GetThreadPtr(thval, th);
    if (th->root_fiber && th->root_fiber != th->fiber) {
	GetFiberPtr(th->root_fiber, fib);
	th->local_storage = fib->cont.saved_thread.local_storage;
    }
}

/*
 *  call-seq:
 *     fiber.alive? -> true or false
 *
 *  Returns true if the fiber can still be resumed (or transferred
 *  to). After finishing execution of the fiber block this method will
 *  always return false. You need to <code>require 'fiber'</code>
 *  before using this method.
 */
VALUE
rb_fiber_alive_p(VALUE fibval)
{
    rb_fiber_t *fib;
    GetFiberPtr(fibval, fib);
    return fib->status != TERMINATED ? Qtrue : Qfalse;
}

/*
 *  call-seq:
 *     fiber.resume(args, ...) -> obj
 *
 *  Resumes the fiber from the point at which the last <code>Fiber.yield</code>
 *  was called, or starts running it if it is the first call to
 *  <code>resume</code>. Arguments passed to resume will be the value of
 *  the <code>Fiber.yield</code> expression or will be passed as block
 *  parameters to the fiber's block if this is the first <code>resume</code>.
 *
 *  Alternatively, when resume is called it evaluates to the arguments passed
 *  to the next <code>Fiber.yield</code> statement inside the fiber's block
 *  or to the block value if it runs to completion without any
 *  <code>Fiber.yield</code>
 */
static VALUE
rb_fiber_m_resume(int argc, VALUE *argv, VALUE fib)
{
    return rb_fiber_resume(fib, argc, argv);
}

/*
 *  call-seq:
 *     fiber.transfer(args, ...) -> obj
 *
 *  Transfer control to another fiber, resuming it from where it last
 *  stopped or starting it if it was not resumed before. The calling
 *  fiber will be suspended much like in a call to
 *  <code>Fiber.yield</code>. You need to <code>require 'fiber'</code>
 *  before using this method.
 *
 *  The fiber which receives the transfer call is treats it much like
 *  a resume call. Arguments passed to transfer are treated like those
 *  passed to resume.
 *
 *  You cannot resume a fiber that transferred control to another one.
 *  This will cause a double resume error. You need to transfer control
 *  back to this fiber before it can yield and resume.
 */
static VALUE
rb_fiber_m_transfer(int argc, VALUE *argv, VALUE fibval)
{
    rb_fiber_t *fib;
    GetFiberPtr(fibval, fib);
    fib->transfered = 1;
    return rb_fiber_transfer(fibval, argc, argv);
}

/*
 *  call-seq:
 *     Fiber.yield(args, ...) -> obj
 *
 *  Yields control back to the context that resumed the fiber, passing
 *  along any arguments that were passed to it. The fiber will resume
 *  processing at this point when <code>resume</code> is called next.
 *  Any arguments passed to the next <code>resume</code> will be the
 *  value that this <code>Fiber.yield</code> expression evaluates to.
 */
static VALUE
rb_fiber_s_yield(int argc, VALUE *argv, VALUE klass)
{
    return rb_fiber_yield(argc, argv);
}

/*
 *  call-seq:
 *     Fiber.current() -> fiber
 *
 *  Returns the current fiber. You need to <code>require 'fiber'</code>
 *  before using this method. If you are not running in the context of
 *  a fiber this method will return the root fiber.
 */
static VALUE
rb_fiber_s_current(VALUE klass)
{
    return rb_fiber_current();
}



/*
 *  Document-class: FiberError
 *
 *  Raised when an invalid operation is attempted on a Fiber, in
 *  particular when attempting to call/resume a dead fiber,
 *  attempting to yield from the root fiber, or calling a fiber across
 *  threads.
 *
 *     fiber = Fiber.new{}
 *     fiber.resume #=> nil
 *     fiber.resume #=> FiberError: dead fiber called
 */

void
Init_Cont(void)
{
#if FIBER_USE_NATIVE
    rb_thread_t *th = GET_THREAD();

#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    pagesize = info.dwPageSize;
#else /* not WIN32 */
    pagesize = sysconf(_SC_PAGESIZE);
#endif
    SET_MACHINE_STACK_END(&th->machine_stack_end);
#endif

    rb_cFiber = rb_define_class("Fiber", rb_cObject);
    rb_define_alloc_func(rb_cFiber, fiber_alloc);
    rb_eFiberError = rb_define_class("FiberError", rb_eStandardError);
    rb_define_singleton_method(rb_cFiber, "yield", rb_fiber_s_yield, -1);
    rb_define_method(rb_cFiber, "initialize", rb_fiber_init, 0);
    rb_define_method(rb_cFiber, "resume", rb_fiber_m_resume, -1);
}

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

void
ruby_Init_Continuation_body(void)
{
    rb_cContinuation = rb_define_class("Continuation", rb_cObject);
    rb_undef_alloc_func(rb_cContinuation);
    rb_undef_method(CLASS_OF(rb_cContinuation), "new");
    rb_define_method(rb_cContinuation, "call", rb_cont_call, -1);
    rb_define_method(rb_cContinuation, "[]", rb_cont_call, -1);
    rb_define_global_function("callcc", rb_callcc, 0);
}

void
ruby_Init_Fiber_as_Coroutine(void)
{
    rb_define_method(rb_cFiber, "transfer", rb_fiber_m_transfer, -1);
    rb_define_method(rb_cFiber, "alive?", rb_fiber_alive_p, 0);
    rb_define_singleton_method(rb_cFiber, "current", rb_fiber_s_current, 0);
}

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif
