/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Aaron Piotrowski <aaron@trowski.com>                        |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_gc.h"
#include "zend_ini.h"
#include "zend_variables.h"
#include "zend_vm.h"
#include "zend_exceptions.h"
#include "zend_builtin_functions.h"
#include "zend_observer.h"
#include "zend_mmap.h"
#include "zend_compile.h"
#include "zend_closures.h"
#include "zend_generators.h"

#include "zend_fibers.h"
#include "zend_fibers_arginfo.h"

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#ifdef ZEND_FIBER_UCONTEXT
# include <ucontext.h>
#endif

#ifndef ZEND_WIN32
# include <unistd.h>
# include <sys/mman.h>
# include <limits.h>

# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#  define MAP_ANONYMOUS MAP_ANON
# endif

/* FreeBSD require a first (i.e. addr) argument of mmap(2) is not NULL
 * if MAP_STACK is passed.
 * http://www.FreeBSD.org/cgi/query-pr.cgi?pr=158755 */
# if !defined(MAP_STACK) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#  undef MAP_STACK
#  define MAP_STACK 0
# endif

# ifndef MAP_FAILED
#  define MAP_FAILED ((void * ) -1)
# endif
#endif

#ifdef __SANITIZE_ADDRESS__
# include <sanitizer/asan_interface.h>
# include <sanitizer/common_interface_defs.h>
#endif

# if defined __CET__
#  include <cet.h>
#  define SHSTK_ENABLED (__CET__ & 0x2)
#  define BOOST_CONTEXT_SHADOW_STACK (SHSTK_ENABLED && SHADOW_STACK_SYSCALL)
#  define __NR_map_shadow_stack 451
# ifndef SHADOW_STACK_SET_TOKEN
#  define SHADOW_STACK_SET_TOKEN 0x1
#endif
#endif

/* Encapsulates the fiber C stack with extension for debugging tools. */
struct _zend_fiber_stack {
	void *pointer;
	size_t size;

#ifdef HAVE_VALGRIND
	unsigned int valgrind_stack_id;
#endif

#ifdef __SANITIZE_ADDRESS__
	const void *asan_pointer;
	size_t asan_size;
#endif

#ifdef ZEND_FIBER_UCONTEXT
	/* Embedded ucontext to avoid unnecessary memory allocations. */
	ucontext_t ucontext;
#elif BOOST_CONTEXT_SHADOW_STACK
	/* Shadow stack: base, size */
	void *ss_base;
	size_t ss_size;
#endif
};

/* Zend VM state that needs to be captured / restored during fiber context switch. */
typedef struct _zend_fiber_vm_state {
	zend_vm_stack vm_stack;
	zval *vm_stack_top;
	zval *vm_stack_end;
	size_t vm_stack_page_size;
	zend_execute_data *current_execute_data;
	int error_reporting;
	uint32_t jit_trace_num;
	JMP_BUF *bailout;
	zend_fiber *active_fiber;
#ifdef ZEND_CHECK_STACK_LIMIT
	void *stack_base;
	void *stack_limit;
#endif
} zend_fiber_vm_state;

static zend_always_inline void zend_fiber_capture_vm_state(zend_fiber_vm_state *state)
{
	state->vm_stack = EG(vm_stack);
	state->vm_stack_top = EG(vm_stack_top);
	state->vm_stack_end = EG(vm_stack_end);
	state->vm_stack_page_size = EG(vm_stack_page_size);
	state->current_execute_data = EG(current_execute_data);
	state->error_reporting = EG(error_reporting);
	state->jit_trace_num = EG(jit_trace_num);
	state->bailout = EG(bailout);
	state->active_fiber = EG(active_fiber);
#ifdef ZEND_CHECK_STACK_LIMIT
	state->stack_base = EG(stack_base);
	state->stack_limit = EG(stack_limit);
#endif
}

static zend_always_inline void zend_fiber_restore_vm_state(zend_fiber_vm_state *state)
{
	EG(vm_stack) = state->vm_stack;
	EG(vm_stack_top) = state->vm_stack_top;
	EG(vm_stack_end) = state->vm_stack_end;
	EG(vm_stack_page_size) = state->vm_stack_page_size;
	EG(current_execute_data) = state->current_execute_data;
	EG(error_reporting) = state->error_reporting;
	EG(jit_trace_num) = state->jit_trace_num;
	EG(bailout) = state->bailout;
	EG(active_fiber) = state->active_fiber;
#ifdef ZEND_CHECK_STACK_LIMIT
	EG(stack_base) = state->stack_base;
	EG(stack_limit) = state->stack_limit;
#endif
}

#ifdef ZEND_FIBER_UCONTEXT
ZEND_TLS zend_fiber_transfer *transfer_data;
#else
/* boost_context_data is our customized definition of struct transfer_t as
 * provided by boost.context in fcontext.hpp:
 *
 * typedef void* fcontext_t;
 *
 * struct transfer_t {
 *     fcontext_t fctx;
 *     void *data;
 * }; */

typedef struct {
	void *handle;
	zend_fiber_transfer *transfer;
} boost_context_data;

/* These functions are defined in assembler files provided by boost.context (located in "Zend/asm"). */
extern void *make_fcontext(void *sp, size_t size, void (*fn)(boost_context_data));
extern ZEND_INDIRECT_RETURN boost_context_data jump_fcontext(void *to, zend_fiber_transfer *transfer);
#endif

ZEND_API zend_class_entry *zend_ce_fiber;
static zend_class_entry *zend_ce_fiber_error;

static zend_object_handlers zend_fiber_handlers;

static zend_function zend_fiber_function = { ZEND_INTERNAL_FUNCTION };

ZEND_TLS uint32_t zend_fiber_switch_blocking = 0;

#define ZEND_FIBER_DEFAULT_PAGE_SIZE 4096

static size_t zend_fiber_get_page_size(void)
{
	static size_t page_size = 0;

	if (!page_size) {
		page_size = zend_get_page_size();
		if (!page_size || (page_size & (page_size - 1))) {
			/* anyway, we have to return a valid result */
			page_size = ZEND_FIBER_DEFAULT_PAGE_SIZE;
		}
	}

	return page_size;
}

static zend_fiber_stack *zend_fiber_stack_allocate(size_t size)
{
	void *pointer;
	const size_t page_size = zend_fiber_get_page_size();
	const size_t minimum_stack_size = page_size + ZEND_FIBER_GUARD_PAGES * page_size
#ifdef __SANITIZE_ADDRESS__
	// necessary correction due to ASAN redzones
	* 6
#endif
	;

	if (size < minimum_stack_size) {
		zend_throw_exception_ex(NULL, 0, "Fiber stack size is too small, it needs to be at least %zu bytes", minimum_stack_size);
		return NULL;
	}

	const size_t stack_size = (size + page_size - 1) / page_size * page_size;
	const size_t alloc_size = stack_size + ZEND_FIBER_GUARD_PAGES * page_size;

#ifdef ZEND_WIN32
	pointer = VirtualAlloc(0, alloc_size, MEM_COMMIT, PAGE_READWRITE);

	if (!pointer) {
		DWORD err = GetLastError();
		char *errmsg = php_win32_error_to_msg(err);
		zend_throw_exception_ex(NULL, 0, "Fiber stack allocate failed: VirtualAlloc failed: [0x%08lx] %s", err, errmsg[0] ? errmsg : "Unknown");
		php_win32_error_msg_free(errmsg);
		return NULL;
	}

# if ZEND_FIBER_GUARD_PAGES
	DWORD protect;

	if (!VirtualProtect(pointer, ZEND_FIBER_GUARD_PAGES * page_size, PAGE_READWRITE | PAGE_GUARD, &protect)) {
		DWORD err = GetLastError();
		char *errmsg = php_win32_error_to_msg(err);
		zend_throw_exception_ex(NULL, 0, "Fiber stack protect failed: VirtualProtect failed: [0x%08lx] %s", err, errmsg[0] ? errmsg : "Unknown");
		php_win32_error_msg_free(errmsg);
		VirtualFree(pointer, 0, MEM_RELEASE);
		return NULL;
	}
# endif
#else
	pointer = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

	if (pointer == MAP_FAILED) {
		zend_throw_exception_ex(NULL, 0, "Fiber stack allocate failed: mmap failed: %s (%d)", strerror(errno), errno);
		return NULL;
	}

#if defined(MADV_NOHUGEPAGE)
	/* Multiple reasons to fail, ignore all errors only needed
	 * for linux < 6.8 */
	(void) madvise(pointer, alloc_size, MADV_NOHUGEPAGE);
#endif

	zend_mmap_set_name(pointer, alloc_size, "zend_fiber_stack");

# if ZEND_FIBER_GUARD_PAGES
	if (mprotect(pointer, ZEND_FIBER_GUARD_PAGES * page_size, PROT_NONE) < 0) {
		zend_throw_exception_ex(NULL, 0, "Fiber stack protect failed: mprotect failed: %s (%d)", strerror(errno), errno);
		munmap(pointer, alloc_size);
		return NULL;
	}
# endif
#endif

	zend_fiber_stack *stack = emalloc(sizeof(zend_fiber_stack));

	stack->pointer = (void *) ((uintptr_t) pointer + ZEND_FIBER_GUARD_PAGES * page_size);
	stack->size = stack_size;

#if !defined(ZEND_FIBER_UCONTEXT) && BOOST_CONTEXT_SHADOW_STACK
	/* shadow stack saves ret address only, need less space */
	stack->ss_size= stack_size >> 5;

	/* align shadow stack to 8 bytes. */
	stack->ss_size = (stack->ss_size + 7) & ~7;

	/* issue syscall to create shadow stack for the new fcontext */
	/* SHADOW_STACK_SET_TOKEN option will put "restore token" on the new shadow stack */
	stack->ss_base = (void *)syscall(__NR_map_shadow_stack, 0, stack->ss_size, SHADOW_STACK_SET_TOKEN);

	if (stack->ss_base == MAP_FAILED) {
		zend_throw_exception_ex(NULL, 0, "Fiber shadow stack allocate failed: mmap failed: %s (%d)", strerror(errno), errno);
		return NULL;
	}
#endif

#ifdef VALGRIND_STACK_REGISTER
	uintptr_t base = (uintptr_t) stack->pointer;
	stack->valgrind_stack_id = VALGRIND_STACK_REGISTER(base, base + stack->size);
#endif

#ifdef __SANITIZE_ADDRESS__
	stack->asan_pointer = stack->pointer;
	stack->asan_size = stack->size;
#endif

	return stack;
}

static void zend_fiber_stack_free(zend_fiber_stack *stack)
{
#ifdef VALGRIND_STACK_DEREGISTER
	VALGRIND_STACK_DEREGISTER(stack->valgrind_stack_id);
#endif

	const size_t page_size = zend_fiber_get_page_size();

	void *pointer = (void *) ((uintptr_t) stack->pointer - ZEND_FIBER_GUARD_PAGES * page_size);

#ifdef __SANITIZE_ADDRESS__
	/* If another mmap happens after unmapping, it may trigger the stale stack red zones
	 * so we have to unpoison it before unmapping. */
	ASAN_UNPOISON_MEMORY_REGION(pointer, stack->size + ZEND_FIBER_GUARD_PAGES * page_size);
#endif

#ifdef ZEND_WIN32
	VirtualFree(pointer, 0, MEM_RELEASE);
#else
	munmap(pointer, stack->size + ZEND_FIBER_GUARD_PAGES * page_size);
#endif

#if !defined(ZEND_FIBER_UCONTEXT) && BOOST_CONTEXT_SHADOW_STACK
	munmap(stack->ss_base, stack->ss_size);
#endif

	efree(stack);
}

#ifdef ZEND_CHECK_STACK_LIMIT
ZEND_API void* zend_fiber_stack_limit(zend_fiber_stack *stack)
{
	zend_ulong reserve = EG(reserved_stack_size);

#ifdef __APPLE__
	/* On Apple Clang, the stack probing function ___chkstk_darwin incorrectly
	 * probes a location that is twice the entered function's stack usage away
	 * from the stack pointer, when using an alternative stack.
	 * https://openradar.appspot.com/radar?id=5497722702397440
	 */
	reserve = reserve * 2;
#endif

	/* stack->pointer is the end of the stack */
	return (int8_t*)stack->pointer + reserve;
}

ZEND_API void* zend_fiber_stack_base(zend_fiber_stack *stack)
{
	return (void*)((uintptr_t)stack->pointer + stack->size);
}
#endif

#ifdef ZEND_FIBER_UCONTEXT
static ZEND_NORETURN void zend_fiber_trampoline(void)
#else
static ZEND_NORETURN void zend_fiber_trampoline(boost_context_data data)
#endif
{
	/* Initialize transfer struct with a copy of passed data. */
#ifdef ZEND_FIBER_UCONTEXT
	zend_fiber_transfer transfer = *transfer_data;
#else
	zend_fiber_transfer transfer = *data.transfer;
#endif

	zend_fiber_context *from = transfer.context;

#ifdef __SANITIZE_ADDRESS__
	__sanitizer_finish_switch_fiber(NULL, &from->stack->asan_pointer, &from->stack->asan_size);
#endif

#ifndef ZEND_FIBER_UCONTEXT
	/* Get the context that resumed us and update its handle to allow for symmetric coroutines. */
	from->handle = data.handle;
#endif

	/* Ensure that previous fiber will be cleaned up (needed by symmetric coroutines). */
	if (from->status == ZEND_FIBER_STATUS_DEAD) {
		zend_fiber_destroy_context(from);
	}

	zend_fiber_context *context = EG(current_fiber_context);

	context->function(&transfer);
	context->status = ZEND_FIBER_STATUS_DEAD;

	/* Final context switch, the fiber must not be resumed afterwards! */
	zend_fiber_switch_context(&transfer);

	/* Abort here because we are in an inconsistent program state. */
	abort();
}

ZEND_API void zend_fiber_switch_block(void)
{
	++zend_fiber_switch_blocking;
}

ZEND_API void zend_fiber_switch_unblock(void)
{
	ZEND_ASSERT(zend_fiber_switch_blocking && "Fiber switching was not blocked");
	--zend_fiber_switch_blocking;
}

ZEND_API bool zend_fiber_switch_blocked(void)
{
	return zend_fiber_switch_blocking;
}

ZEND_API zend_result zend_fiber_init_context(zend_fiber_context *context, void *kind, zend_fiber_coroutine coroutine, size_t stack_size)
{
	context->stack = zend_fiber_stack_allocate(stack_size);

	if (UNEXPECTED(!context->stack)) {
		return FAILURE;
	}

#ifdef ZEND_FIBER_UCONTEXT
	ucontext_t *handle = &context->stack->ucontext;

	getcontext(handle);

	handle->uc_stack.ss_size = context->stack->size;
	handle->uc_stack.ss_sp = context->stack->pointer;
	handle->uc_stack.ss_flags = 0;
	handle->uc_link = NULL;

	makecontext(handle, (void (*)(void)) zend_fiber_trampoline, 0);

	context->handle = handle;
#else
	// Stack grows down, calculate the top of the stack. make_fcontext then shifts pointer to lower 16-byte boundary.
	void *stack = (void *) ((uintptr_t) context->stack->pointer + context->stack->size);

#if BOOST_CONTEXT_SHADOW_STACK
	// pass the shadow stack pointer to make_fcontext
	// i.e., link the new shadow stack with the new fcontext
	// TODO should be a better way?
	*((unsigned long*) (stack - 8)) = (unsigned long)context->stack->ss_base + context->stack->ss_size;
#endif

	context->handle = make_fcontext(stack, context->stack->size, zend_fiber_trampoline);
	ZEND_ASSERT(context->handle != NULL && "make_fcontext() never returns NULL");
#endif

	context->kind = kind;
	context->function = coroutine;

	// Set status in case memory has not been zeroed.
	context->status = ZEND_FIBER_STATUS_INIT;

	zend_observer_fiber_init_notify(context);

	return SUCCESS;
}

ZEND_API void zend_fiber_destroy_context(zend_fiber_context *context)
{
	zend_observer_fiber_destroy_notify(context);

	if (context->cleanup) {
		context->cleanup(context);
	}

	zend_fiber_stack_free(context->stack);
}

ZEND_API void zend_fiber_switch_context(zend_fiber_transfer *transfer)
{
	zend_fiber_context *from = EG(current_fiber_context);
	zend_fiber_context *to = transfer->context;
	zend_fiber_vm_state state;

	ZEND_ASSERT(to && to->handle && to->status != ZEND_FIBER_STATUS_DEAD && "Invalid fiber context");
	ZEND_ASSERT(from && "From fiber context must be present");
	ZEND_ASSERT(to != from && "Cannot switch into the running fiber context");

	/* Assert that all error transfers hold a Throwable value. */
	ZEND_ASSERT((
		!(transfer->flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) ||
		(Z_TYPE(transfer->value) == IS_OBJECT && (
			zend_is_unwind_exit(Z_OBJ(transfer->value)) ||
			zend_is_graceful_exit(Z_OBJ(transfer->value)) ||
			instanceof_function(Z_OBJCE(transfer->value), zend_ce_throwable)
		))
	) && "Error transfer requires a throwable value");

	zend_observer_fiber_switch_notify(from, to);

	zend_fiber_capture_vm_state(&state);

	to->status = ZEND_FIBER_STATUS_RUNNING;

	if (EXPECTED(from->status == ZEND_FIBER_STATUS_RUNNING)) {
		from->status = ZEND_FIBER_STATUS_SUSPENDED;
	}

	/* Update transfer context with the current fiber before switching. */
	transfer->context = from;

	EG(current_fiber_context) = to;

#ifdef __SANITIZE_ADDRESS__
	void *fake_stack = NULL;
	__sanitizer_start_switch_fiber(
		from->status != ZEND_FIBER_STATUS_DEAD ? &fake_stack : NULL,
		to->stack->asan_pointer,
		to->stack->asan_size);
#endif

#ifdef ZEND_FIBER_UCONTEXT
	transfer_data = transfer;

	swapcontext(from->handle, to->handle);

	/* Copy transfer struct because it might live on the other fiber's stack that will eventually be destroyed. */
	*transfer = *transfer_data;
#else
	boost_context_data data = jump_fcontext(to->handle, transfer);

	/* Copy transfer struct because it might live on the other fiber's stack that will eventually be destroyed. */
	*transfer = *data.transfer;
#endif

	to = transfer->context;

#ifndef ZEND_FIBER_UCONTEXT
	/* Get the context that resumed us and update its handle to allow for symmetric coroutines. */
	to->handle = data.handle;
#endif

#ifdef __SANITIZE_ADDRESS__
	__sanitizer_finish_switch_fiber(fake_stack, &to->stack->asan_pointer, &to->stack->asan_size);
#endif

	EG(current_fiber_context) = from;

	zend_fiber_restore_vm_state(&state);

	/* Destroy prior context if it has been marked as dead. */
	if (to->status == ZEND_FIBER_STATUS_DEAD) {
		zend_fiber_destroy_context(to);
	}
}

ZEND_API zend_execute_data *zend_fiber_vm_stack_start(
		zend_fiber_context *context, zend_function *root_function)
{
	/* Determine the current error_reporting ini setting. NULL or an empty
	 * string means "never configured"; only an explicit "0" keeps 0. */
	zend_long error_reporting = zend_ini_long_literal("error_reporting");
	if (!error_reporting) {
		zend_string *value = zend_ini_str_literal("error_reporting");

		if (value == NULL || ZSTR_LEN(value) == 0) {
			error_reporting = E_ALL;
		}
	}

	zend_vm_stack stack = zend_vm_stack_new_page(ZEND_FIBER_VM_STACK_SIZE, NULL);

	EG(vm_stack) = stack;
	EG(vm_stack_top) = stack->top + ZEND_CALL_FRAME_SLOT;
	EG(vm_stack_end) = stack->end;
	EG(vm_stack_page_size) = ZEND_FIBER_VM_STACK_SIZE;

	zend_execute_data *execute_data = (zend_execute_data *) stack->top;

	memset(execute_data, 0, sizeof(zend_execute_data));
	execute_data->func = root_function;
	execute_data->prev_execute_data = EG(current_execute_data);

	EG(current_execute_data) = execute_data;
	EG(jit_trace_num) = 0;
	EG(error_reporting) = (int) error_reporting;

#ifdef ZEND_CHECK_STACK_LIMIT
	EG(stack_base) = zend_fiber_stack_base(context->stack);
	EG(stack_limit) = zend_fiber_stack_limit(context->stack);
#else
	(void) context;
#endif

	return execute_data;
}

ZEND_API void zend_fiber_vm_stack_free(zend_vm_stack stack)
{
	zend_vm_stack current_stack = EG(vm_stack);

	EG(vm_stack) = stack;
	zend_vm_stack_destroy();
	EG(vm_stack) = current_stack;
}

static void zend_fiber_cleanup(zend_fiber_context *context)
{
	zend_fiber *fiber = zend_fiber_from_context(context);

	zend_fiber_vm_stack_free(fiber->vm_stack);
	fiber->execute_data = NULL;
	fiber->stack_bottom = NULL;
	fiber->caller = NULL;
}

static ZEND_STACK_ALIGNED void zend_fiber_execute(zend_fiber_transfer *transfer)
{
	ZEND_ASSERT(Z_TYPE(transfer->value) == IS_NULL && "Initial transfer value to fiber context must be NULL");
	ZEND_ASSERT(!transfer->flags && "No flags should be set on initial transfer");

	zend_fiber *fiber = EG(active_fiber);

	EG(vm_stack) = NULL;

	zend_first_try {
		fiber->execute_data = zend_fiber_vm_stack_start(&fiber->context, &zend_fiber_function);
		fiber->stack_bottom = fiber->execute_data;

		fiber->fci.retval = &fiber->result;

		zend_call_function(&fiber->fci, &fiber->fci_cache);

		/* Cleanup callback and unset field to prevent GC / duplicate dtor issues. */
		zval_ptr_dtor(&fiber->fci.function_name);
		ZVAL_UNDEF(&fiber->fci.function_name);

		if (EG(exception)) {
			if (!(fiber->flags & ZEND_FIBER_FLAG_DESTROYED)
				|| !(zend_is_graceful_exit(EG(exception)) || zend_is_unwind_exit(EG(exception)))
			) {
				fiber->flags |= ZEND_FIBER_FLAG_THREW;
				transfer->flags = ZEND_FIBER_TRANSFER_FLAG_ERROR;

				ZVAL_OBJ_COPY(&transfer->value, EG(exception));
			}

			zend_clear_exception();
		}
	} zend_catch {
		fiber->flags |= ZEND_FIBER_FLAG_BAILOUT;
		transfer->flags = ZEND_FIBER_TRANSFER_FLAG_BAILOUT;
	} zend_end_try();

	fiber->context.cleanup = &zend_fiber_cleanup;
	fiber->vm_stack = EG(vm_stack);

	transfer->context = fiber->caller;
}

/* Handles forwarding of result / error from a transfer into the running fiber. */
static zend_always_inline void zend_fiber_delegate_transfer_result(
	zend_fiber_transfer *transfer, INTERNAL_FUNCTION_PARAMETERS
) {
	if (transfer->flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) {
		/* Use internal throw to skip the Throwable-check that would fail for (graceful) exit. */
		zend_throw_exception_internal(Z_OBJ(transfer->value));
		RETURN_THROWS();
	}

	if (return_value != NULL) {
		RETURN_COPY_VALUE(&transfer->value);
	} else {
		zval_ptr_dtor(&transfer->value);
	}
}

static zend_always_inline zend_fiber_transfer zend_fiber_switch_to(
	zend_fiber_context *context, zval *value, bool exception
) {
	zend_fiber_transfer transfer = {
		.context = context,
		.flags = exception ? ZEND_FIBER_TRANSFER_FLAG_ERROR : 0,
	};

	if (value) {
		ZVAL_COPY(&transfer.value, value);
	} else {
		ZVAL_NULL(&transfer.value);
	}

	zend_fiber_switch_context(&transfer);

	/* Forward bailout into current fiber. */
	if (UNEXPECTED(transfer.flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT)) {
		EG(active_fiber) = NULL;
		zend_bailout();
	}

	return transfer;
}

static zend_always_inline zend_fiber_transfer zend_fiber_resume_internal(zend_fiber *fiber, zval *value, bool exception)
{
	zend_fiber *previous = EG(active_fiber);

	if (previous) {
		previous->execute_data = EG(current_execute_data);
	}

	fiber->caller = EG(current_fiber_context);
	EG(active_fiber) = fiber;

	zend_fiber_transfer transfer = zend_fiber_switch_to(fiber->previous, value, exception);

	EG(active_fiber) = previous;

	return transfer;
}

static zend_always_inline zend_fiber_transfer zend_fiber_suspend_internal(zend_fiber *fiber, zval *value)
{
	ZEND_ASSERT(!(fiber->flags & ZEND_FIBER_FLAG_DESTROYED));
	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_RUNNING || fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED);
	ZEND_ASSERT(fiber->caller != NULL);

	zend_fiber_context *caller = fiber->caller;
	fiber->previous = EG(current_fiber_context);
	fiber->caller = NULL;
	fiber->execute_data = EG(current_execute_data);

	return zend_fiber_switch_to(caller, value, false);
}

/////////////////////////////////////////////////////////////////////
/// Coroutine mode: the fiber as a coroutine of the scheduler
/////////////////////////////////////////////////////////////////////

/* A fiber under a scheduler has no context of its own: the scheduler runs its
 * body through this entry point. start()/resume()/throw() wake the coroutine
 * and wait; Fiber::suspend() wakes the waiter and parks. */
static void zend_fiber_coroutine_entry(void);

/* The coroutine is going away while the fiber still points at it. */
static void zend_fiber_coroutine_dispose(zend_coroutine_t *coroutine)
{
	zend_fiber *fiber = coroutine->extended_data;

	if (fiber == NULL) {
		return;
	}

	coroutine->extended_data = NULL;
	fiber->coroutine = NULL;
}

/* The fiber lets go of its coroutine. Ownership is one-way (fiber → coroutine,
 * never back), which keeps the fiber collectable. A fiber that dies while its
 * body is still parked cancels it — the body unwinds through its finally
 * blocks, like a force-closed legacy fiber. */
static void zend_fiber_release_coroutine(zend_fiber *fiber)
{
	zend_coroutine_t *coroutine = fiber->coroutine;

	if (coroutine == NULL) {
		return;
	}

	zend_object *coroutine_object = ZEND_COROUTINE_OBJECT(coroutine);

	coroutine->extended_data = NULL;
	fiber->coroutine = NULL;

	if (ZEND_ASYNC_ON && ZEND_COROUTINE_IS_STARTED(coroutine)
		&& !ZEND_COROUTINE_IS_FINISHED(coroutine)) {
		ZEND_ASYNC_CANCEL(coroutine, zend_create_graceful_exit(), true);
	}

	if (coroutine_object != NULL) {
		OBJ_RELEASE(coroutine_object);
	}
}

static void zend_fiber_coroutine_entry(void)
{
	zend_coroutine_t *coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
	zend_fiber *fiber = coroutine->extended_data;
	zend_fiber *previous_fiber = EG(active_fiber);

	ZEND_ASSERT(fiber != NULL && "A fiber coroutine must know its fiber");

	/* Inside the body, Fiber::suspend() and Fiber::getCurrent() must see this
	 * fiber, exactly as on the legacy path. */
	EG(active_fiber) = fiber;
	fiber->context.status = ZEND_FIBER_STATUS_RUNNING;

	/* The body runs on a stack the scheduler owns, whose bottom frame belongs
	 * to nobody. Link it to the frame that started us, or a backtrace taken
	 * inside the fiber would stop at the fiber and never reach the caller. */
	fiber->stack_bottom = EG(current_execute_data);

	if (fiber->execute_data != NULL) {
		fiber->stack_bottom->prev_execute_data = fiber->execute_data;
		fiber->execute_data = NULL;
	}

	bool is_bailout = false;

	zend_try {
		coroutine->fcall->fci.retval = &fiber->result;
		zend_call_function(&coroutine->fcall->fci, &coroutine->fcall->fci_cache);
	} zend_catch {
		is_bailout = true;
	} zend_end_try();

	EG(active_fiber) = previous_fiber;

	/* A fatal error in the body is the fiber's own outcome: getReturn() has to
	 * be able to tell it apart from a fiber that simply never returned. The
	 * unwinding then goes on to the scheduler. */
	if (UNEXPECTED(is_bailout)) {
		fiber = coroutine->extended_data;

		if (fiber != NULL) {
			fiber->flags |= ZEND_FIBER_FLAG_BAILOUT;
			fiber->stack_bottom = NULL;
			fiber->context.status = ZEND_FIBER_STATUS_DEAD;
		}

		zend_bailout();
	}

	/* The Fiber object may have been dropped while the body was parked: it is
	 * the coroutine that owns the body, not the object. */
	fiber = coroutine->extended_data;

	zend_coroutine_t *caller = NULL;

	if (fiber != NULL) {
		/* The caller's frame may be gone by the time anything looks at this
		 * stack again. */
		if (fiber->stack_bottom != NULL) {
			fiber->stack_bottom->prev_execute_data = NULL;
			fiber->stack_bottom = NULL;
		}

		fiber->context.status = ZEND_FIBER_STATUS_DEAD;

		caller = fiber->caller_coroutine;
		fiber->caller_coroutine = NULL;
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		/* exit(): the scheduler decides what happens to the rest. A graceful
		 * exit is a disposal unwind and must not shut anything down. */
		if (zend_is_unwind_exit(EG(exception))) {
			ZEND_ASYNC_SHUTDOWN();
		}

		if (fiber != NULL) {
			fiber->flags |= ZEND_FIBER_FLAG_THREW;
		}

		/* Nobody is waiting: the exception belongs to the coroutine, and the
		 * scheduler decides what to do with it (a graceful exit from a
		 * force-closed fiber included). */
		if (caller == NULL) {
			return;
		}

		zend_object *exception = EG(exception);
		GC_ADDREF(exception);
		zend_clear_exception();

		ZEND_ASYNC_ENQUEUE_WITH_ERROR(caller, exception, true);
		return;
	}

	if (caller != NULL) {
		ZEND_ASYNC_ENQUEUE_COROUTINE(caller);
	}
}

/* Fiber::suspend() in coroutine mode: hand the value to whoever is waiting,
 * wake them, and park until somebody resumes us. */
static void zend_fiber_coroutine_yield(zend_fiber *fiber, zval *value, zval *return_value)
{
	zend_coroutine_t *coroutine = fiber->coroutine;

	if (value != NULL) {
		ZVAL_COPY(&fiber->transfer, value);
	} else {
		ZVAL_NULL(&fiber->transfer);
	}

	zend_coroutine_t *caller = fiber->caller_coroutine;
	fiber->caller_coroutine = NULL;
	fiber->context.status = ZEND_FIBER_STATUS_SUSPENDED;

	/* Sever now, not when the caller wakes: GC may walk this parked stack
	 * in between, and it must end at our own root frame. */
	if (fiber->stack_bottom != NULL) {
		fiber->stack_bottom->prev_execute_data = NULL;
	}

	if (caller != NULL) {
		ZEND_ASYNC_ENQUEUE_COROUTINE(caller);
	}

	const bool resumed = ZEND_ASYNC_SUSPEND();

	/* Parked all this time; the Fiber object may not have survived — a fiber
	 * nobody holds is force-closed, arriving as a graceful exit thrown here.
	 * Keep EG(active_fiber) off a fiber that is gone. */
	fiber = coroutine->extended_data;
	EG(active_fiber) = fiber;

	if (fiber != NULL) {
		fiber->context.status = ZEND_FIBER_STATUS_RUNNING;
	}

	/* throw(), or the cancellation of a dropped fiber, lands here. */
	if (UNEXPECTED(!resumed)) {
		return;
	}

	if (UNEXPECTED(fiber == NULL) || Z_ISUNDEF(fiber->transfer)) {
		ZVAL_NULL(return_value);
		return;
	}

	ZVAL_COPY_VALUE(return_value, &fiber->transfer);
	ZVAL_UNDEF(&fiber->transfer);
}

/* Wait for the fiber to yield or finish. Returns what it yielded; NULL when
 * it finished (the return value is getReturn()'s business, as ever). */
static zend_result zend_fiber_await(zend_fiber *fiber, zval *return_value)
{
	zend_coroutine_t *caller = ZEND_ASYNC_CURRENT_COROUTINE;

	ZEND_ASSERT(caller != NULL && "A scheduler is active, so there is a current coroutine");

	if (UNEXPECTED(caller == fiber->coroutine)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot await a fiber from within itself");
		return FAILURE;
	}

	fiber->caller_coroutine = caller;

	/* Hand the body the frame it was started from, so a backtrace taken
	 * inside the fiber continues into its caller. Before the first run there
	 * is no stack to link to yet, and the entry point does it instead. */
	if (fiber->stack_bottom != NULL) {
		fiber->stack_bottom->prev_execute_data = EG(current_execute_data);
	} else {
		fiber->execute_data = EG(current_execute_data);
	}

	ZVAL_NULL(return_value);

	/* Parked until the fiber hands control back. An exception that escaped the
	 * fiber is delivered here, at the start()/resume() call site. */
	if (UNEXPECTED(!ZEND_ASYNC_SUSPEND())) {
		fiber->caller_coroutine = NULL;

		if (fiber->stack_bottom != NULL) {
			fiber->stack_bottom->prev_execute_data = NULL;
		}

		return FAILURE;
	}

	fiber->caller_coroutine = NULL;

	/* Our frame stops being the fiber's parent the moment it parks again. */
	if (fiber->stack_bottom != NULL) {
		fiber->stack_bottom->prev_execute_data = NULL;
	}

	if (!Z_ISUNDEF(fiber->transfer)) {
		ZVAL_COPY_VALUE(return_value, &fiber->transfer);
		ZVAL_UNDEF(&fiber->transfer);
	}

	return SUCCESS;
}

/* Offer the starting fiber to the scheduler. It answers with a coroutine to
 * run the fiber as, or with NULL to leave it on the legacy path (that is how
 * a scheduler keeps its own fibers to itself). */
static bool zend_fiber_adopt(zend_fiber *fiber)
{
	if (!ZEND_ASYNC_ON) {
		return false;
	}

	/* A registered scheduler launches once, before the script's first line, so
	 * it is never still READY here; ZEND_ASYNC_INTERCEPT_FIBER() falls back to
	 * the legacy path on its own if needed. */
	zend_coroutine_t *coroutine = ZEND_ASYNC_INTERCEPT_FIBER(fiber);

	if (coroutine == NULL) {
		return false;
	}

	coroutine->internal_entry = zend_fiber_coroutine_entry;
	coroutine->extended_data = fiber;
	coroutine->extended_dispose = zend_fiber_coroutine_dispose;
	ZEND_COROUTINE_SET_FIBER(coroutine);

	/* The fiber shares ownership of the coroutine with the scheduler. Never
	 * the other way round: see zend_fiber_release_coroutine(). */
	zend_object *coroutine_object = ZEND_COROUTINE_OBJECT(coroutine);

	if (coroutine_object != NULL) {
		GC_ADDREF(coroutine_object);
	}

	fiber->coroutine = coroutine;

	return true;
}

/* Queue the body and wait for the first yield. */
static void zend_fiber_coroutine_start(zend_fiber *fiber, zval *return_value)
{
	ZEND_ASYNC_FCALL_DEFINE(fcall,
			fiber->fci,
			fiber->fci_cache,
			fiber->fci.params,
			fiber->fci.param_count,
			fiber->fci.named_params);

	fiber->coroutine->fcall = fcall;

	if (UNEXPECTED(!ZEND_ASYNC_ENQUEUE_COROUTINE(fiber->coroutine))) {
		zend_fiber_release_coroutine(fiber);
		return;
	}

	zend_fiber_await(fiber, return_value);
}

ZEND_API zend_result zend_fiber_start(zend_fiber *fiber, zval *return_value)
{
	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_INIT);

	if (zend_fiber_init_context(&fiber->context, zend_ce_fiber, zend_fiber_execute, EG(fiber_stack_size)) == FAILURE) {
		return FAILURE;
	}

	fiber->previous = &fiber->context;

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, NULL, false);

	zend_fiber_delegate_transfer_result(&transfer, EG(current_execute_data), return_value);

	return SUCCESS;
}

ZEND_API void zend_fiber_resume(zend_fiber *fiber, zval *value, zval *return_value)
{
	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED && fiber->caller == NULL);

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, value, /* exception */ false);

	zend_fiber_delegate_transfer_result(&transfer, EG(current_execute_data), return_value);
}

ZEND_API void zend_fiber_resume_exception(zend_fiber *fiber, zval *exception, zval *return_value)
{
	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED && fiber->caller == NULL);

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, exception, /* exception */ true);

	zend_fiber_delegate_transfer_result(&transfer, EG(current_execute_data), return_value);
}

ZEND_API void zend_fiber_suspend(zend_fiber *fiber, zval *value, zval *return_value)
{
	fiber->stack_bottom->prev_execute_data = NULL;

	zend_fiber_transfer transfer = zend_fiber_suspend_internal(fiber, value);

	zend_fiber_delegate_transfer_result(&transfer, EG(current_execute_data), return_value);
}

static zend_object *zend_fiber_object_create(zend_class_entry *ce)
{
	zend_fiber *fiber = emalloc(sizeof(zend_fiber));
	memset(fiber, 0, sizeof(zend_fiber));

	zend_object_std_init(&fiber->std, ce);
	return &fiber->std;
}

static void zend_fiber_object_destroy(zend_object *object)
{
	zend_fiber *fiber = (zend_fiber *) object;

	/* Coroutine mode: release unconditionally — even a normally-finished (DEAD)
	 * fiber still holds the scheduler-shared reference and nothing else drops
	 * it. The legacy path below never applies (fiber->context uninitialized). */
	if (fiber->coroutine != NULL) {
		fiber->flags |= ZEND_FIBER_FLAG_DESTROYED;
		zend_fiber_release_coroutine(fiber);
		return;
	}

	if (fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED) {
		return;
	}

	zend_object *exception = EG(exception);
	EG(exception) = NULL;

	zval graceful_exit;
	ZVAL_OBJ(&graceful_exit, zend_create_graceful_exit());

	fiber->flags |= ZEND_FIBER_FLAG_DESTROYED;

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, &graceful_exit, true);

	zval_ptr_dtor(&graceful_exit);

	if (transfer.flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) {
		EG(exception) = Z_OBJ(transfer.value);

		if (!exception && EG(current_execute_data) && EG(current_execute_data)->func
				&& ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}

		/* A graceful/unwind exit is an internal marker, not a Throwable that
		 * carries a $previous — chaining onto it creates a bogus dynamic
		 * property (and a deprecation notice). */
		if (!zend_is_graceful_exit(EG(exception)) && !zend_is_unwind_exit(EG(exception))) {
			zend_exception_set_previous(EG(exception), exception);
		} else if (exception != NULL) {
			OBJ_RELEASE(exception);
		}

		if (!EG(current_execute_data)) {
			zend_exception_error(EG(exception), E_ERROR);
		}
	} else {
		zval_ptr_dtor(&transfer.value);
		EG(exception) = exception;
	}
}

static void zend_fiber_object_free(zend_object *object)
{
	zend_fiber *fiber = (zend_fiber *) object;
	/* A fiber that was never suspended (so dtor_obj had nothing to close) can
	 * still own a coroutine: one that never started, or one that already
	 * finished. */
	zend_fiber_release_coroutine(fiber);

	zval_ptr_dtor(&fiber->fci.function_name);
	zval_ptr_dtor(&fiber->result);
	zval_ptr_dtor(&fiber->transfer);

	zend_object_std_dtor(&fiber->std);
}

/* Walk the frames of a stack parked at a clean Fiber::suspend(): their
 * variables take part in cycle collection. Returns the last symbol table met. */
static HashTable *zend_fiber_frames_gc(zend_execute_data *ex, zend_get_gc_buffer *buf)
{
	HashTable *lastSymTable = NULL;

	for (; ex; ex = ex->prev_execute_data) {
		HashTable *symTable;
		if (ZEND_CALL_INFO(ex) & ZEND_CALL_GENERATOR) {
			/* The generator object is stored in ex->return_value */
			zend_generator *generator = (zend_generator*)ex->return_value;
			/* There are two cases to consider:
			 * - If the generator is currently running, the Generator's GC
			 *   handler will ignore it because it is not collectable. However,
			 *   in this context the generator is suspended in Fiber::suspend()
			 *   and may be collectable, so we can inspect it.
			 * - If the generator is not running, the Generator's GC handler
			 *   will inspect it. In this case we have to skip the frame.
			 */
			if (!(generator->flags & ZEND_GENERATOR_CURRENTLY_RUNNING)) {
				continue;
			}
			symTable = zend_generator_frame_gc(buf, generator);
		} else {
			symTable = zend_unfinished_execution_gc_ex(ex, ex->func && ZEND_USER_CODE(ex->func->type) ? ex->call : NULL, buf, false);
		}
		if (symTable) {
			if (lastSymTable) {
				zval *val;
				ZEND_HASH_FOREACH_VAL(lastSymTable, val) {
					if (EXPECTED(Z_TYPE_P(val) == IS_INDIRECT)) {
						val = Z_INDIRECT_P(val);
					}
					zend_get_gc_buffer_add_zval(buf, val);
				} ZEND_HASH_FOREACH_END();
			}
			lastSymTable = symTable;
		}
	}

	return lastSymTable;
}

static HashTable *zend_fiber_object_gc(zend_object *object, zval **table, int *num)
{
	zend_fiber *fiber = (zend_fiber *) object;
	zend_get_gc_buffer *buf = zend_get_gc_buffer_create();

	/* Coroutine-mode fiber. Running or awaiting inside the body — a GC root:
	 * the stack is mid-operation, scanning it is unsafe. Parked at a clean
	 * Fiber::suspend() (context.status is set only by the yield path) — the
	 * stack is passive and its frames join cycle collection, like a legacy
	 * suspended fiber. */
	if (fiber->coroutine != NULL) {
		if (ZEND_COROUTINE_IS_FINISHED(fiber->coroutine)) {
			zend_get_gc_buffer_add_zval(buf, &fiber->fci.function_name);
			zend_get_gc_buffer_add_zval(buf, &fiber->result);
			zend_get_gc_buffer_add_zval(buf, &fiber->transfer);

			zend_object *coroutine_object = ZEND_COROUTINE_OBJECT(fiber->coroutine);

			if (coroutine_object != NULL) {
				zend_get_gc_buffer_add_obj(buf, coroutine_object);
			}

			zend_get_gc_buffer_use(buf, table, num);
			return NULL;
		}

		if (fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED) {
			zend_get_gc_buffer_add_zval(buf, &fiber->fci.function_name);
			zend_get_gc_buffer_add_zval(buf, &fiber->transfer);

			/* The coroutine object stays out of the buffer: the scheduler
			 * holds references GC cannot see, exposing it would miscount. */
			HashTable *lastSymTable = zend_fiber_frames_gc(
					ZEND_ASYNC_COROUTINE_EXECUTE_DATA(fiber->coroutine), buf);

			zend_get_gc_buffer_use(buf, table, num);
			return lastSymTable;
		}

		zend_get_gc_buffer_use(buf, table, num);
		return NULL;
	}

	zend_get_gc_buffer_add_zval(buf, &fiber->fci.function_name);
	zend_get_gc_buffer_add_zval(buf, &fiber->result);
	zend_get_gc_buffer_add_zval(buf, &fiber->transfer);

	zend_execute_data *ex;

	if (fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED && fiber->caller == NULL) {
		ex = fiber->execute_data;
	} else {
		ex = NULL;
	}

	if (ex == NULL) {
		zend_get_gc_buffer_use(buf, table, num);
		return NULL;
	}

	HashTable *lastSymTable = zend_fiber_frames_gc(ex, buf);

	zend_get_gc_buffer_use(buf, table, num);

	return lastSymTable;
}

ZEND_METHOD(Fiber, __construct)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	if (UNEXPECTED(fiber->context.status != ZEND_FIBER_STATUS_INIT || Z_TYPE(fiber->fci.function_name) != IS_UNDEF)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot call constructor twice");
		RETURN_THROWS();
	}

	fiber->fci = fci;
	fiber->fci_cache = fcc;

	// Keep a reference to closures or callable objects while the fiber is running.
	Z_TRY_ADDREF(fiber->fci.function_name);
}

ZEND_METHOD(Fiber, start)
{
	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(fiber->fci.params, fiber->fci.param_count, fiber->fci.named_params);
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(zend_fiber_switch_blocked())) {
		zend_throw_error(zend_ce_fiber_error, "Cannot switch fibers in current execution context");
		RETURN_THROWS();
	}

	if (fiber->context.status != ZEND_FIBER_STATUS_INIT) {
		zend_throw_error(zend_ce_fiber_error, "Cannot start a fiber that has already been started");
		RETURN_THROWS();
	}

	/* A scheduler gets to claim the fiber here, once, and then it is a
	 * coroutine: no context of its own is ever created. */
	if (zend_fiber_adopt(fiber)) {
		zend_fiber_coroutine_start(fiber, return_value);

		if (UNEXPECTED(EG(exception) != NULL)) {
			RETURN_THROWS();
		}

		return;
	}

	if (UNEXPECTED(EG(exception) != NULL)) {
		RETURN_THROWS();
	}

	if (zend_fiber_init_context(&fiber->context, zend_ce_fiber, zend_fiber_execute, EG(fiber_stack_size)) == FAILURE) {
		RETURN_THROWS();
	}

	fiber->previous = &fiber->context;

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, NULL, false);

	zend_fiber_delegate_transfer_result(&transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Fiber, suspend)
{
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	const zend_coroutine_t *current = ZEND_ASYNC_CURRENT_COROUTINE;
	const zend_fiber *active = EG(active_fiber);
	const bool in_legacy_fiber = active != NULL && active->coroutine == NULL;

	if (!in_legacy_fiber && current != NULL) {
		if (UNEXPECTED(!ZEND_COROUTINE_IS_FIBER(current))) {
			zend_throw_error(zend_ce_fiber_error, "Cannot suspend outside of a fiber");
			RETURN_THROWS();
		}

		if (UNEXPECTED(zend_fiber_switch_blocked())) {
			zend_throw_error(zend_ce_fiber_error, "Cannot switch fibers in current execution context");
			RETURN_THROWS();
		}

		if (UNEXPECTED(current->extended_data == NULL || ZEND_COROUTINE_IS_CANCELLED(current))) {
			zend_throw_error(zend_ce_fiber_error, "Cannot suspend in a force-closed fiber");
			RETURN_THROWS();
		}

		zend_fiber_coroutine_yield(current->extended_data, value, return_value);

		return;
	}

	zend_fiber *fiber = EG(active_fiber);

	if (UNEXPECTED(!fiber)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot suspend outside of a fiber");
		RETURN_THROWS();
	}

	if (UNEXPECTED(fiber->flags & ZEND_FIBER_FLAG_DESTROYED)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot suspend in a force-closed fiber");
		RETURN_THROWS();
	}

	if (UNEXPECTED(zend_fiber_switch_blocked())) {
		zend_throw_error(zend_ce_fiber_error, "Cannot switch fibers in current execution context");
		RETURN_THROWS();
	}

	ZEND_ASSERT(fiber->context.status == ZEND_FIBER_STATUS_RUNNING || fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED);

	fiber->stack_bottom->prev_execute_data = NULL;

	zend_fiber_transfer transfer = zend_fiber_suspend_internal(fiber, value);

	zend_fiber_delegate_transfer_result(&transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Fiber, resume)
{
	zend_fiber *fiber;
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(zend_fiber_switch_blocked())) {
		zend_throw_error(zend_ce_fiber_error, "Cannot switch fibers in current execution context");
		RETURN_THROWS();
	}

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	if (UNEXPECTED(fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED || fiber->caller != NULL)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot resume a fiber that is not suspended");
		RETURN_THROWS();
	}

	if (fiber->coroutine != NULL) {
		if (value != NULL) {
			ZVAL_COPY(&fiber->transfer, value);
		} else {
			ZVAL_NULL(&fiber->transfer);
		}

		if (UNEXPECTED(!ZEND_ASYNC_ENQUEUE_COROUTINE(fiber->coroutine))
			|| UNEXPECTED(zend_fiber_await(fiber, return_value) == FAILURE)) {
			RETURN_THROWS();
		}

		return;
	}

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, value, false);

	zend_fiber_delegate_transfer_result(&transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Fiber, throw)
{
	zend_fiber *fiber;
	zval *exception;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(exception, zend_ce_throwable)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(zend_fiber_switch_blocked())) {
		zend_throw_error(zend_ce_fiber_error, "Cannot switch fibers in current execution context");
		RETURN_THROWS();
	}

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	if (UNEXPECTED(fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED || fiber->caller != NULL)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot resume a fiber that is not suspended");
		RETURN_THROWS();
	}

	if (fiber->coroutine != NULL) {
		if (UNEXPECTED(!ZEND_ASYNC_ENQUEUE_WITH_ERROR(fiber->coroutine, Z_OBJ_P(exception), false))
			|| UNEXPECTED(zend_fiber_await(fiber, return_value) == FAILURE)) {
			RETURN_THROWS();
		}

		return;
	}

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume_internal(fiber, exception, true);

	zend_fiber_delegate_transfer_result(&transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Fiber, isStarted)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	RETURN_BOOL(fiber->context.status != ZEND_FIBER_STATUS_INIT);
}

ZEND_METHOD(Fiber, isSuspended)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	RETURN_BOOL(fiber->context.status == ZEND_FIBER_STATUS_SUSPENDED && fiber->caller == NULL);
}

ZEND_METHOD(Fiber, isRunning)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	RETURN_BOOL(fiber->context.status == ZEND_FIBER_STATUS_RUNNING || fiber->caller != NULL);
}

ZEND_METHOD(Fiber, isTerminated)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	RETURN_BOOL(fiber->context.status == ZEND_FIBER_STATUS_DEAD);
}

ZEND_METHOD(Fiber, getReturn)
{
	zend_fiber *fiber;
	const char *message;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(ZEND_THIS);

	if (fiber->context.status == ZEND_FIBER_STATUS_DEAD) {
		if (fiber->flags & ZEND_FIBER_FLAG_THREW) {
			message = "The fiber threw an exception";
		} else if (fiber->flags & ZEND_FIBER_FLAG_BAILOUT) {
			message = "The fiber exited with a fatal error";
		} else {
			RETURN_COPY_DEREF(&fiber->result);
		}
	} else if (fiber->context.status == ZEND_FIBER_STATUS_INIT) {
		message = "The fiber has not been started";
	} else {
		message = "The fiber has not returned";
	}

	zend_throw_error(zend_ce_fiber_error, "Cannot get fiber return value: %s", message);
	RETURN_THROWS();
}

ZEND_METHOD(Fiber, getCurrent)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_fiber *fiber = EG(active_fiber);

	if (!fiber) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(&fiber->std);
}

ZEND_METHOD(FiberError, __construct)
{
	zend_throw_error(
		NULL,
		"The \"%s\" class is reserved for internal use and cannot be manually instantiated",
		ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name)
	);
}


void zend_register_fiber_ce(void)
{
	zend_ce_fiber = register_class_Fiber();
	zend_ce_fiber->create_object = zend_fiber_object_create;
	zend_ce_fiber->default_object_handlers = &zend_fiber_handlers;

	zend_fiber_handlers = std_object_handlers;
	zend_fiber_handlers.dtor_obj = zend_fiber_object_destroy;
	zend_fiber_handlers.free_obj = zend_fiber_object_free;
	zend_fiber_handlers.get_gc = zend_fiber_object_gc;
	zend_fiber_handlers.clone_obj = NULL;

	zend_ce_fiber_error = register_class_FiberError(zend_ce_error);
	zend_ce_fiber_error->create_object = zend_ce_error->create_object;
}

void zend_fiber_init(void)
{
	zend_fiber_context *context = ecalloc(1, sizeof(zend_fiber_context));

#if defined(__SANITIZE_ADDRESS__) || defined(ZEND_FIBER_UCONTEXT)
	// Main fiber stack is only needed if ASan or ucontext is enabled.
	context->stack = emalloc(sizeof(zend_fiber_stack));

#ifdef ZEND_FIBER_UCONTEXT
	context->handle = &context->stack->ucontext;
#endif
#endif

	context->status = ZEND_FIBER_STATUS_RUNNING;

	EG(main_fiber_context) = context;
	EG(current_fiber_context) = context;
	EG(active_fiber) = NULL;

	zend_fiber_switch_blocking = 0;
}

void zend_fiber_shutdown(void)
{
#if defined(__SANITIZE_ADDRESS__) || defined(ZEND_FIBER_UCONTEXT)
	efree(EG(main_fiber_context)->stack);
#endif

	efree(EG(main_fiber_context));

	zend_fiber_switch_block();
}
