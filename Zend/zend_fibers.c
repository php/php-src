/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Aaron Piotrowski <aaron@trowski.com>                        |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_vm.h"
#include "zend_exceptions.h"
#include "zend_builtin_functions.h"
#include "zend_observer.h"
#include "zend_mmap.h"
#include "zend_compile.h"
#include "zend_closures.h"

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
# include <sanitizer/common_interface_defs.h>
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
	const size_t minimum_stack_size = page_size + ZEND_FIBER_GUARD_PAGES * page_size;

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

#ifdef ZEND_WIN32
	VirtualFree(pointer, 0, MEM_RELEASE);
#else
	munmap(pointer, stack->size + ZEND_FIBER_GUARD_PAGES * page_size);
#endif

	efree(stack);
}
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

ZEND_API bool zend_fiber_init_context(zend_fiber_context *context, void *kind, zend_fiber_coroutine coroutine, size_t stack_size)
{
	context->stack = zend_fiber_stack_allocate(stack_size);

	if (UNEXPECTED(!context->stack)) {
		return false;
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

	context->handle = make_fcontext(stack, context->stack->size, zend_fiber_trampoline);
	ZEND_ASSERT(context->handle != NULL && "make_fcontext() never returns NULL");
#endif

	context->kind = kind;
	context->function = coroutine;

	// Set status in case memory has not been zeroed.
	context->status = ZEND_FIBER_STATUS_INIT;

	zend_observer_fiber_init_notify(context);

	return true;
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

static void zend_fiber_cleanup(zend_fiber_context *context)
{
	zend_fiber *fiber = zend_fiber_from_context(context);

	zend_vm_stack current_stack = EG(vm_stack);
	EG(vm_stack) = fiber->vm_stack;
	zend_vm_stack_destroy();
	EG(vm_stack) = current_stack;
	fiber->execute_data = NULL;
	fiber->stack_bottom = NULL;
	fiber->caller = NULL;
}

static ZEND_STACK_ALIGNED void zend_fiber_execute(zend_fiber_transfer *transfer)
{
	ZEND_ASSERT(Z_TYPE(transfer->value) == IS_NULL && "Initial transfer value to fiber context must be NULL");
	ZEND_ASSERT(!transfer->flags && "No flags should be set on initial transfer");

	zend_fiber *fiber = EG(active_fiber);

	/* Determine the current error_reporting ini setting. */
	zend_long error_reporting = INI_INT("error_reporting");
	/* If error_reporting is 0 and not explicitly set to 0, INI_STR returns a null pointer. */
	if (!error_reporting && !INI_STR("error_reporting")) {
		error_reporting = E_ALL;
	}

	EG(vm_stack) = NULL;

	zend_first_try {
		zend_vm_stack stack = zend_vm_stack_new_page(ZEND_FIBER_VM_STACK_SIZE, NULL);
		EG(vm_stack) = stack;
		EG(vm_stack_top) = stack->top + ZEND_CALL_FRAME_SLOT;
		EG(vm_stack_end) = stack->end;
		EG(vm_stack_page_size) = ZEND_FIBER_VM_STACK_SIZE;

		fiber->execute_data = (zend_execute_data *) stack->top;
		fiber->stack_bottom = fiber->execute_data;

		memset(fiber->execute_data, 0, sizeof(zend_execute_data));

		fiber->execute_data->func = &zend_fiber_function;
		fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

		EG(current_execute_data) = fiber->execute_data;
		EG(jit_trace_num) = 0;
		EG(error_reporting) = error_reporting;

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

	RETURN_COPY_VALUE(&transfer->value);
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

static zend_always_inline zend_fiber_transfer zend_fiber_resume(zend_fiber *fiber, zval *value, bool exception)
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

static zend_always_inline zend_fiber_transfer zend_fiber_suspend(zend_fiber *fiber, zval *value)
{
	ZEND_ASSERT(fiber->caller != NULL);

	zend_fiber_context *caller = fiber->caller;
	fiber->previous = EG(current_fiber_context);
	fiber->caller = NULL;
	fiber->execute_data = EG(current_execute_data);

	return zend_fiber_switch_to(caller, value, false);
}

static zend_object *zend_fiber_object_create(zend_class_entry *ce)
{
	zend_fiber *fiber = emalloc(sizeof(zend_fiber));

	memset(fiber, 0, sizeof(zend_fiber));

	zend_object_std_init(&fiber->std, ce);
	fiber->std.handlers = &zend_fiber_handlers;

	return &fiber->std;
}

static void zend_fiber_object_destroy(zend_object *object)
{
	zend_fiber *fiber = (zend_fiber *) object;

	if (fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED) {
		return;
	}

	zend_object *exception = EG(exception);
	EG(exception) = NULL;

	zval graceful_exit;
	ZVAL_OBJ(&graceful_exit, zend_create_graceful_exit());

	fiber->flags |= ZEND_FIBER_FLAG_DESTROYED;

	zend_fiber_transfer transfer = zend_fiber_resume(fiber, &graceful_exit, true);

	zval_ptr_dtor(&graceful_exit);

	if (transfer.flags & ZEND_FIBER_TRANSFER_FLAG_ERROR) {
		EG(exception) = Z_OBJ(transfer.value);

		if (!exception && EG(current_execute_data) && EG(current_execute_data)->func
				&& ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}

		zend_exception_set_previous(EG(exception), exception);

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

	zval_ptr_dtor(&fiber->fci.function_name);
	zval_ptr_dtor(&fiber->result);

	zend_object_std_dtor(&fiber->std);
}

static HashTable *zend_fiber_object_gc(zend_object *object, zval **table, int *num)
{
	zend_fiber *fiber = (zend_fiber *) object;
	zend_get_gc_buffer *buf = zend_get_gc_buffer_create();

	zend_get_gc_buffer_add_zval(buf, &fiber->fci.function_name);
	zend_get_gc_buffer_add_zval(buf, &fiber->result);

	if (fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED || fiber->caller != NULL) {
		zend_get_gc_buffer_use(buf, table, num);
		return NULL;
	}

	HashTable *lastSymTable = NULL;
	zend_execute_data *ex = fiber->execute_data;
	for (; ex; ex = ex->prev_execute_data) {
		HashTable *symTable = zend_unfinished_execution_gc_ex(ex, ex->func && ZEND_USER_CODE(ex->func->type) ? ex->call : NULL, buf, false);
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

	if (!zend_fiber_init_context(&fiber->context, zend_ce_fiber, zend_fiber_execute, EG(fiber_stack_size))) {
		RETURN_THROWS();
	}

	fiber->previous = &fiber->context;

	zend_fiber_transfer transfer = zend_fiber_resume(fiber, NULL, false);

	zend_fiber_delegate_transfer_result(&transfer, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Fiber, suspend)
{
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

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

	zend_fiber_transfer transfer = zend_fiber_suspend(fiber, value);

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

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume(fiber, value, false);

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

	fiber->stack_bottom->prev_execute_data = EG(current_execute_data);

	zend_fiber_transfer transfer = zend_fiber_resume(fiber, exception, true);

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
