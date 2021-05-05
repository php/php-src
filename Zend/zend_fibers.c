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
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_builtin_functions.h"
#include "zend_observer.h"

#include "zend_fibers.h"
#include "zend_fibers_arginfo.h"

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#ifndef PHP_WIN32
# include <unistd.h>
# include <sys/mman.h>
# include <limits.h>
#endif

ZEND_API zend_class_entry *zend_ce_fiber;
static zend_class_entry *zend_ce_fiber_error;

static zend_object_handlers zend_fiber_handlers;

static zend_function zend_fiber_function = { ZEND_INTERNAL_FUNCTION };

typedef void *fcontext_t;

typedef struct _transfer_t {
	fcontext_t context;
	void *data;
} transfer_t;

extern fcontext_t make_fcontext(void *sp, size_t size, void (*fn)(transfer_t));
extern transfer_t jump_fcontext(fcontext_t to, void *vp);

#define ZEND_FIBER_DEFAULT_PAGE_SIZE 4096

#define ZEND_FIBER_BACKUP_EG(stack, stack_page_size, execute_data, error_reporting, trace_num) do { \
	stack = EG(vm_stack); \
	stack->top = EG(vm_stack_top); \
	stack->end = EG(vm_stack_end); \
	stack_page_size = EG(vm_stack_page_size); \
	execute_data = EG(current_execute_data); \
	error_reporting = EG(error_reporting); \
	trace_num = EG(jit_trace_num); \
} while (0)

#define ZEND_FIBER_RESTORE_EG(stack, stack_page_size, execute_data, error_reporting, trace_num) do { \
	EG(vm_stack) = stack; \
	EG(vm_stack_top) = stack->top; \
	EG(vm_stack_end) = stack->end; \
	EG(vm_stack_page_size) = stack_page_size; \
	EG(current_execute_data) = execute_data; \
	EG(error_reporting) = error_reporting; \
	EG(jit_trace_num) = trace_num; \
} while (0)

#if defined(MAP_STACK) && !defined(__FreeBSD__) && !defined(__FreeBSD_kernel__)
# define ZEND_FIBER_STACK_FLAGS (MAP_PRIVATE | MAP_ANON | MAP_STACK)
#else
# define ZEND_FIBER_STACK_FLAGS (MAP_PRIVATE | MAP_ANON)
#endif



static size_t zend_fiber_page_size()
{
#if _POSIX_MAPPED_FILES
	static size_t page_size;

	if (!page_size) {
		page_size = sysconf(_SC_PAGESIZE);
	}

	return page_size;
#else
	return ZEND_FIBER_DEFAULT_PAGE_SIZE;
#endif
}

static bool zend_fiber_stack_allocate(zend_fiber_stack *stack, size_t size)
{
	void *pointer;
	const size_t page_size = zend_fiber_page_size();

	ZEND_ASSERT(size >= page_size + ZEND_FIBER_GUARD_PAGES * page_size);

	stack->size = (size + page_size - 1) / page_size * page_size;
	const size_t msize = stack->size + ZEND_FIBER_GUARD_PAGES * page_size;

#ifdef PHP_WIN32
	pointer = VirtualAlloc(0, msize, MEM_COMMIT, PAGE_READWRITE);

	if (!pointer) {
		return false;
	}

# if ZEND_FIBER_GUARD_PAGES
	DWORD protect;

	if (!VirtualProtect(pointer, ZEND_FIBER_GUARD_PAGES * page_size, PAGE_READWRITE | PAGE_GUARD, &protect)) {
		VirtualFree(pointer, 0, MEM_RELEASE);
		return false;
	}
# endif
#else
	pointer = mmap(NULL, msize, PROT_READ | PROT_WRITE, ZEND_FIBER_STACK_FLAGS, -1, 0);

	if (pointer == MAP_FAILED) {
		return false;
	}

# if ZEND_FIBER_GUARD_PAGES
	if (mprotect(pointer, ZEND_FIBER_GUARD_PAGES * page_size, PROT_NONE) < 0) {
		munmap(pointer, msize);
		return false;
	}
# endif
#endif

	stack->pointer = (void *) ((uintptr_t) pointer + ZEND_FIBER_GUARD_PAGES * page_size);

#ifdef VALGRIND_STACK_REGISTER
	uintptr_t base = (uintptr_t) stack->pointer;
	stack->valgrind = VALGRIND_STACK_REGISTER(base, base + stack->size);
#endif

	return true;
}

static void zend_fiber_stack_free(zend_fiber_stack *stack)
{
	if (!stack->pointer) {
		return;
	}

#ifdef VALGRIND_STACK_DEREGISTER
	VALGRIND_STACK_DEREGISTER(stack->valgrind);
#endif

	const size_t page_size = zend_fiber_page_size();

	void *pointer = (void *) ((uintptr_t) stack->pointer - ZEND_FIBER_GUARD_PAGES * page_size);

#ifdef PHP_WIN32
	VirtualFree(pointer, 0, MEM_RELEASE);
#else
	munmap(pointer, stack->size + ZEND_FIBER_GUARD_PAGES * page_size);
#endif

	stack->pointer = NULL;
}

static ZEND_NORETURN void zend_fiber_trampoline(transfer_t transfer)
{
	zend_fiber_context *context = transfer.data;

	context->caller = transfer.context;

	context->function(context);

	context->self = NULL;

	zend_fiber_suspend_context(context);

	abort();
}

ZEND_API bool zend_fiber_init_context(zend_fiber_context *context, zend_fiber_coroutine coroutine, size_t stack_size)
{
	if (UNEXPECTED(!zend_fiber_stack_allocate(&context->stack, stack_size))) {
		return false;
	}

	// Stack grows down, calculate the top of the stack. make_fcontext then shifts pointer to lower 16-byte boundary.
	void *stack = (void *) ((uintptr_t) context->stack.pointer + context->stack.size);

	context->self = make_fcontext(stack, context->stack.size, zend_fiber_trampoline);

	if (UNEXPECTED(!context->self)) {
		zend_fiber_stack_free(&context->stack);
		return false;
	}

	context->function = coroutine;
	context->caller = NULL;

	return true;
}

ZEND_API void zend_fiber_destroy_context(zend_fiber_context *context)
{
	zend_fiber_stack_free(&context->stack);
}

ZEND_API void zend_fiber_switch_context(zend_fiber_context *to)
{
	ZEND_ASSERT(to && to->self && to->stack.pointer && "Invalid fiber context");

	transfer_t transfer = jump_fcontext(to->self, to);

	to->self = transfer.context;
}

ZEND_API void zend_fiber_suspend_context(zend_fiber_context *current)
{
	ZEND_ASSERT(current && current->caller && current->stack.pointer && "Invalid fiber context");

	transfer_t transfer = jump_fcontext(current->caller, NULL);

	current->caller = transfer.context;
}

static void zend_fiber_suspend(zend_fiber *fiber)
{
	zend_vm_stack stack;
	size_t stack_page_size;
	zend_execute_data *execute_data;
	int error_reporting;
	uint32_t jit_trace_num;

	ZEND_FIBER_BACKUP_EG(stack, stack_page_size, execute_data, error_reporting, jit_trace_num);

	zend_fiber_suspend_context(&fiber->context);

	ZEND_FIBER_RESTORE_EG(stack, stack_page_size, execute_data, error_reporting, jit_trace_num);
}

static void zend_fiber_switch_to(zend_fiber *fiber)
{
	zend_fiber *previous;
	zend_vm_stack stack;
	size_t stack_page_size;
	zend_execute_data *execute_data;
	int error_reporting;
	uint32_t jit_trace_num;

	previous = EG(current_fiber);

	zend_observer_fiber_switch_notify(previous, fiber);

	ZEND_FIBER_BACKUP_EG(stack, stack_page_size, execute_data, error_reporting, jit_trace_num);

	EG(current_fiber) = fiber;

	zend_fiber_switch_context(&fiber->context);

	EG(current_fiber) = previous;

	ZEND_FIBER_RESTORE_EG(stack, stack_page_size, execute_data, error_reporting, jit_trace_num);

	zend_observer_fiber_switch_notify(fiber, previous);

	if (UNEXPECTED(EG(fiber_error)) && fiber->status != ZEND_FIBER_STATUS_SHUTDOWN) {
		if (previous) {
			zend_fiber_suspend(previous); // Still in fiber, suspend again until in {main}.
			abort(); // This fiber should never be resumed.
		}

		zend_error_info *error = EG(fiber_error);
		zend_error_zstr_at(error->type, error->filename, error->lineno, error->message);
	}
}

ZEND_COLD void zend_error_suspend_fiber(
		int orig_type, zend_string *error_filename, uint32_t error_lineno, zend_string *message)
{
	ZEND_ASSERT(EG(current_fiber) && "Must be within an active fiber!");
	ZEND_ASSERT(orig_type & E_FATAL_ERRORS && "Error type must be fatal");

	zend_error_info *error = emalloc(sizeof(zend_error_info));

	error->type = orig_type;
	error->filename = error_filename;
	error->lineno = error_lineno;
	error->message = message;

	EG(fiber_error) = error;

	zend_fiber_suspend(EG(current_fiber));

	abort(); // This fiber should never be resumed.
}

static zend_always_inline zend_vm_stack zend_fiber_vm_stack_alloc(size_t size)
{
	zend_vm_stack page = emalloc(size);

	page->top = ZEND_VM_STACK_ELEMENTS(page);
	page->end = (zval *) ((uintptr_t) page + size);
	page->prev = NULL;

	return page;
}

static void ZEND_STACK_ALIGNED zend_fiber_execute(zend_fiber_context *context)
{
	zend_fiber *fiber = EG(current_fiber);
	ZEND_ASSERT(fiber);

	zend_long error_reporting = INI_INT("error_reporting");
	if (!error_reporting && !INI_STR("error_reporting")) {
		error_reporting = E_ALL;
	}

	zend_vm_stack stack = zend_fiber_vm_stack_alloc(ZEND_FIBER_VM_STACK_SIZE);
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

	fiber->fci.retval = &fiber->value;

	fiber->status = ZEND_FIBER_STATUS_RUNNING;

	zend_call_function(&fiber->fci, &fiber->fci_cache);

	zval_ptr_dtor(&fiber->fci.function_name);

	if (EG(exception)) {
		if (fiber->status == ZEND_FIBER_STATUS_SHUTDOWN) {
			if (EXPECTED(zend_is_graceful_exit(EG(exception)) || zend_is_unwind_exit(EG(exception)))) {
				zend_clear_exception();
			}
		} else {
			fiber->status = ZEND_FIBER_STATUS_THREW;
		}
	} else {
		fiber->status = ZEND_FIBER_STATUS_RETURNED;
	}

	zend_vm_stack_destroy();
	fiber->execute_data = NULL;
	fiber->stack_bottom = NULL;
}

static zend_object *zend_fiber_object_create(zend_class_entry *ce)
{
	zend_fiber *fiber;

	fiber = emalloc(sizeof(zend_fiber));
	memset(fiber, 0, sizeof(zend_fiber));

	zend_object_std_init(&fiber->std, ce);
	fiber->std.handlers = &zend_fiber_handlers;

	return &fiber->std;
}

static void zend_fiber_object_destroy(zend_object *object)
{
	zend_fiber *fiber = (zend_fiber *) object;

	if (fiber->status != ZEND_FIBER_STATUS_SUSPENDED) {
		return;
	}

	zend_object *exception = EG(exception);
	EG(exception) = NULL;

	fiber->status = ZEND_FIBER_STATUS_SHUTDOWN;

	zend_fiber_switch_to(fiber);

	if (EG(exception)) {
		if (!exception && EG(current_execute_data) && EG(current_execute_data)->func
				&& ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}

		zend_exception_set_previous(EG(exception), exception);

		if (!EG(current_execute_data)) {
			zend_exception_error(EG(exception), E_ERROR);
		}
	} else {
		EG(exception) = exception;
	}
}

static void zend_fiber_object_free(zend_object *object)
{
	zend_fiber *fiber = (zend_fiber *) object;

	if (fiber->status == ZEND_FIBER_STATUS_INIT) {
		// Fiber was never started, so we need to release the reference to the callback.
		zval_ptr_dtor(&fiber->fci.function_name);
	}

	zval_ptr_dtor(&fiber->value);

	zend_fiber_destroy_context(&fiber->context);

	zend_object_std_dtor(&fiber->std);
}

ZEND_METHOD(Fiber, __construct)
{
	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(getThis());

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fiber->fci, fiber->fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	// Keep a reference to closures or callable objects while the fiber is running.
	Z_TRY_ADDREF(fiber->fci.function_name);
}

ZEND_METHOD(Fiber, start)
{
	zend_fiber *fiber = (zend_fiber *) Z_OBJ_P(getThis());
	zval *params;
	uint32_t param_count;
	zend_array *named_params;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(params, param_count, named_params);
	ZEND_PARSE_PARAMETERS_END();

	if (fiber->status != ZEND_FIBER_STATUS_INIT) {
		zend_throw_error(zend_ce_fiber_error, "Cannot start a fiber that has already been started");
		RETURN_THROWS();
	}

	fiber->fci.params = params;
	fiber->fci.param_count = param_count;
	fiber->fci.named_params = named_params;

	if (!zend_fiber_init_context(&fiber->context, zend_fiber_execute, EG(fiber_stack_size))) {
		zend_throw_exception(NULL, "Could not create fiber context", 0);
		RETURN_THROWS();
	}

	zend_fiber_switch_to(fiber);

	if (fiber->status & ZEND_FIBER_STATUS_FINISHED) {
		RETURN_NULL();
	}

	RETVAL_COPY_VALUE(&fiber->value);
	ZVAL_UNDEF(&fiber->value);
}

ZEND_METHOD(Fiber, suspend)
{
	zend_fiber *fiber = EG(current_fiber);
	zval *exception, *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(!fiber)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot suspend outside of a fiber");
		RETURN_THROWS();
	}

	if (UNEXPECTED(fiber->status == ZEND_FIBER_STATUS_SHUTDOWN)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot suspend in a force-closed fiber");
		RETURN_THROWS();
	}

	ZEND_ASSERT(fiber->status == ZEND_FIBER_STATUS_RUNNING);

	if (value) {
		ZVAL_COPY(&fiber->value, value);
	} else {
		ZVAL_NULL(&fiber->value);
	}

	fiber->execute_data = execute_data;
	fiber->status = ZEND_FIBER_STATUS_SUSPENDED;
	fiber->stack_bottom->prev_execute_data = NULL;

	zend_fiber_suspend(fiber);

	if (fiber->status == ZEND_FIBER_STATUS_SHUTDOWN) {
		// This occurs when the fiber is GC'ed while suspended.
		if (EG(fiber_error)) {
			// Throw UnwindExit so finally blocks are not executed on fatal error.
			zend_throw_unwind_exit();
		} else {
			// Otherwise throw GracefulExit to execute finally blocks.
			zend_throw_graceful_exit();
		}
		RETURN_THROWS();
	}

	fiber->status = ZEND_FIBER_STATUS_RUNNING;

	if (fiber->exception) {
		exception = fiber->exception;
		fiber->exception = NULL;

		zend_throw_exception_object(exception);
		RETURN_THROWS();
	}

	RETVAL_COPY_VALUE(&fiber->value);
	ZVAL_UNDEF(&fiber->value);
}

ZEND_METHOD(Fiber, resume)
{
	zend_fiber *fiber;
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (UNEXPECTED(fiber->status != ZEND_FIBER_STATUS_SUSPENDED)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot resume a fiber that is not suspended");
		RETURN_THROWS();
	}

	if (value) {
		ZVAL_COPY(&fiber->value, value);
	} else {
		ZVAL_NULL(&fiber->value);
	}

	fiber->status = ZEND_FIBER_STATUS_RUNNING;
	fiber->stack_bottom->prev_execute_data = execute_data;

	zend_fiber_switch_to(fiber);

	if (fiber->status & ZEND_FIBER_STATUS_FINISHED) {
		RETURN_NULL();
	}

	RETVAL_COPY_VALUE(&fiber->value);
	ZVAL_UNDEF(&fiber->value);
}

ZEND_METHOD(Fiber, throw)
{
	zend_fiber *fiber;
	zval *exception;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(exception, zend_ce_throwable)
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (UNEXPECTED(fiber->status != ZEND_FIBER_STATUS_SUSPENDED)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot resume a fiber that is not suspended");
		RETURN_THROWS();
	}

	Z_ADDREF_P(exception);
	fiber->exception = exception;

	fiber->status = ZEND_FIBER_STATUS_RUNNING;
	fiber->stack_bottom->prev_execute_data = execute_data;

	zend_fiber_switch_to(fiber);

	if (fiber->status & ZEND_FIBER_STATUS_FINISHED) {
		RETURN_NULL();
	}

	RETVAL_COPY_VALUE(&fiber->value);
	ZVAL_UNDEF(&fiber->value);
}

ZEND_METHOD(Fiber, isStarted)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	RETURN_BOOL(fiber->status != ZEND_FIBER_STATUS_INIT);
}

ZEND_METHOD(Fiber, isSuspended)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	RETURN_BOOL(fiber->status == ZEND_FIBER_STATUS_SUSPENDED);
}

ZEND_METHOD(Fiber, isRunning)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	RETURN_BOOL(fiber->status == ZEND_FIBER_STATUS_RUNNING);
}

ZEND_METHOD(Fiber, isTerminated)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	RETURN_BOOL(fiber->status & ZEND_FIBER_STATUS_FINISHED);
}

ZEND_METHOD(Fiber, getReturn)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (fiber->status != ZEND_FIBER_STATUS_RETURNED) {
		const char *message;

		if (fiber->status == ZEND_FIBER_STATUS_INIT) {
			message = "The fiber has not been started";
		} else if (fiber->status == ZEND_FIBER_STATUS_THREW) {
			message = "The fiber threw an exception";
		} else {
			message = "The fiber has not returned";
		}

		zend_throw_error(zend_ce_fiber_error, "Cannot get fiber return value: %s", message);
		RETURN_THROWS();
	}

	RETURN_COPY(&fiber->value);
}

ZEND_METHOD(Fiber, this)
{
	zend_fiber *fiber;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = EG(current_fiber);

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
		ZSTR_VAL(Z_OBJCE_P(getThis())->name)
	);
}


void zend_register_fiber_ce(void)
{
	zend_ce_fiber = register_class_Fiber();
	zend_ce_fiber->create_object = zend_fiber_object_create;
	zend_ce_fiber->serialize = zend_class_serialize_deny;
	zend_ce_fiber->unserialize = zend_class_unserialize_deny;

	zend_fiber_handlers = std_object_handlers;
	zend_fiber_handlers.dtor_obj = zend_fiber_object_destroy;
	zend_fiber_handlers.free_obj = zend_fiber_object_free;
	zend_fiber_handlers.clone_obj = NULL;

	zend_ce_fiber_error = register_class_FiberError(zend_ce_error);
	zend_ce_fiber_error->create_object = zend_ce_error->create_object;
}

void zend_fiber_init(void)
{
	EG(current_fiber) = NULL;
	EG(fiber_error) = NULL;
}
