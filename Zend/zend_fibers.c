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
#include "zend_weakrefs.h"

#include "zend_fibers.h"
#include "zend_fibers_arginfo.h"

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#ifdef __SANITIZE_ADDRESS__
# include <sanitizer/common_interface_defs.h>
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

#define ZEND_VM_STACK_MAIN_FIBER_SLOTS \
	((ZEND_MM_ALIGNED_SIZE(sizeof(zend_fiber) + sizeof(zend_fiber_executor)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval)));

#define ZEND_FIBER_GRACEFUL_EXIT_MAGIC ((zend_object *) -1)

ZEND_API zend_class_entry *zend_ce_fiber;
static zend_class_entry *zend_ce_fiber_error;

static zend_object_handlers zend_fiber_handlers;

typedef struct {
	void *from;
	zval *data;
} zend_fiber_transfer;

void *make_fcontext(void *stack, size_t stack_size, void (*fn)(zend_fiber_transfer));
zend_fiber_transfer jump_fcontext(void const *target, zval *transfer_data);

static size_t zend_fiber_get_page_size(void)
{
	static size_t page_size = 0;

	if (!page_size) {
		page_size = zend_get_page_size();
		if (!page_size || (page_size & (page_size - 1))) {
			/* anyway, we have to return a valid result */
			page_size = ZEND_FIBER_C_STACK_ALIGNMENT;
		}
	}

	return page_size;
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

	fiber->flags |= ZEND_FIBER_FLAG_DESTROYED;
	if (fiber->status != ZEND_FIBER_STATUS_SUSPENDED) {
		return;
	}

	zend_object *exception = EG(exception);
	EG(exception) = NULL;

	EG(fiber_exception) = ZEND_FIBER_GRACEFUL_EXIT_MAGIC;
	zend_fiber_jump(fiber, NULL, NULL);

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
	zval_ptr_dtor(&fiber->retval);

	zend_object_std_dtor(&fiber->std);
}

static ZEND_COLD void zend_fiber_status_error(const zend_fiber *fiber)
{
	switch (fiber->status) {
		case ZEND_FIBER_STATUS_RUNNING:
			zend_throw_error(zend_ce_fiber_error, "Fiber is running");
			break;
		case ZEND_FIBER_STATUS_INIT:
			zend_throw_error(zend_ce_fiber_error, "Fiber has not started");
			break;
		case ZEND_FIBER_STATUS_DEAD:
			zend_throw_error(zend_ce_fiber_error, "Fiber is dead");
			break;
		default:
			ZEND_UNREACHABLE();
			break;
	}
}

static ZEND_COLD void zend_fiber_handle_exception(zend_fiber *fiber)
{
	if (zend_is_graceful_exit(EG(exception)) || zend_is_unwind_exit(EG(exception))) {
		if (fiber->flags & ZEND_FIBER_FLAG_DESTROYED) {
			zend_clear_exception();
		}
	} else {
		fiber->flags |= ZEND_FIBER_FLAG_THREW;
	}
}

static ZEND_COLD void zend_fiber_handle_cross_exception(zend_fiber *fiber)
{
	zend_object *exception = EG(fiber_exception);

	EG(fiber_exception) = NULL;
	if (exception == ZEND_FIBER_GRACEFUL_EXIT_MAGIC) {
		zend_throw_graceful_exit();
	} else {
		zend_throw_exception_internal(exception);
	}
}

static void ZEND_STACK_ALIGNED zend_fiber_execute(zend_fiber_transfer transfer)
{
	zend_fiber *fiber = EG(current_fiber);

#ifdef __SANITIZE_ADDRESS__
	__sanitizer_finish_switch_fiber(NULL, &fiber->from->context.asan_stack_bottom, &fiber->from->context.asan_stack_size);
#endif

	/* update from context */
	fiber->from->context.ptr = transfer.from;

	/* prepare function call info (call from API) */
	if (transfer.data != NULL) {
		if (Z_TYPE_P(transfer.data) != IS_PTR) {
			fiber->fci.param_count = 1;
			fiber->fci.params = transfer.data;
			fiber->fci.named_params = NULL;
		} else {
			zend_fcall_info *fci = Z_PTR_P(transfer.data);
			fiber->fci.param_count = fci->param_count;
			fiber->fci.params = fci->params;
			fiber->fci.named_params = fci->named_params;
		}
	}
	fiber->fci.retval = &fiber->retval;

	zend_long error_reporting = INI_INT("error_reporting");
	if (!error_reporting && !INI_STR("error_reporting")) {
		error_reporting = E_ALL;
	}
	EG(error_reporting) = error_reporting;

	zend_try {
		zend_call_function(&fiber->fci, &fiber->fci_cache);
		if (UNEXPECTED(EG(exception))) {
			zend_fiber_handle_exception(fiber);
		}
		zval_ptr_dtor(&fiber->fci.function_name);
		if (UNEXPECTED(EG(exception))) {
			zend_fiber_handle_exception(fiber);
		}
		ZVAL_NULL(&fiber->fci.function_name);
	} zend_catch {
		fiber->flags |= ZEND_FIBER_FLAG_BAILOUT;
	} zend_end_try();

	/* release extra vm stack pages */
	zend_vm_stack stack = EG(vm_stack);
	while (stack != fiber->executor->vm_stack) {
		zend_vm_stack prev = stack->prev;
		efree(stack);
		stack = prev;
	}

	fiber->status = ZEND_FIBER_STATUS_DEAD;
	zend_fiber_jump(fiber->previous, NULL, NULL);

	abort();
}

ZEND_API zend_fiber *zend_fiber_create(const zend_fcall_info *fci, const zend_fcall_info_cache *fci_cache)
{
	zend_fiber *fiber = (zend_fiber *) zend_fiber_object_create(zend_ce_fiber);

	fiber->fci = *fci;
	fiber->fci_cache = *fci_cache;

	Z_TRY_ADDREF(fiber->fci.function_name);

	return fiber;
}

ZEND_API bool zend_fiber_start(zend_fiber *fiber, zval *data, zval *retval)
{
	if (fiber->status != ZEND_FIBER_STATUS_INIT) {
		zend_throw_error(zend_ce_fiber_error, "Cannot start a fiber that has already been started");
		return false;
	}

	size_t stack_size = EG(fiber_stack_size), user_stack_size;
	void *stack, *stack_start;
	zend_vm_stack vm_stack;
	uint32_t vm_stack_size = ZEND_FIBER_DEFAULT_VM_STACK_SIZE; // TODO: make it configurable?
	zend_fiber_executor *executor;
	zend_execute_data *execute_data;
	static const zend_function dummy_function = { ZEND_INTERNAL_FUNCTION }; // constify it to make sure it's readonly

#ifdef ZEND_WIN32
	stack = VirtualAlloc(0, stack_size, MEM_COMMIT, PAGE_READWRITE);

	if (!stack) {
		DWORD err = GetLastError();
		char *errmsg = php_win32_error_to_msg(err);
		zend_throw_exception_ex(NULL, 0, "Fiber make context failed: VirtualAlloc failed: [0x%08lx] %s", err, errmsg[0] ? errmsg : "Unknown");
		php_win32_error_msg_free(errmsg);
		return false;
	}

# if ZEND_FIBER_GUARD_PAGES
	DWORD protect;

	if (!VirtualProtect(stack, zend_fiber_get_page_size(), PAGE_READWRITE | PAGE_GUARD, &protect)) {
		DWORD err = GetLastError();
		char *errmsg = php_win32_error_to_msg(err);
		zend_throw_exception_ex(NULL, 0, "Fiber protect stack failed: VirtualProtect failed: [0x%08lx] %s", err, errmsg[0] ? errmsg : "Unknown");
		php_win32_error_msg_free(errmsg);
		VirtualFree(stack, 0, MEM_RELEASE);
		return false;
	}
# endif
#else
	stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

	if (stack == MAP_FAILED) {
		zend_throw_exception_ex(NULL, 0, "Fiber make context failed: mmap failed: %s (%d)", strerror(errno), errno);
		return false;
	}

# if ZEND_FIBER_GUARD_PAGES
	if (mprotect(stack, zend_fiber_get_page_size(), PROT_NONE) < 0) {
		zend_throw_exception_ex(NULL, 0, "Fiber protect stack failed: mmap failed: %s (%d)", strerror(errno), errno);
		munmap(stack, stack_size);
		return false;
	}
# endif
#endif
	user_stack_size = stack_size - (vm_stack_size + sizeof(*executor) + sizeof(*execute_data));
	stack_start = ((char *) stack) + user_stack_size;
	/* Use stack memory to store VM stack, executor and execute_data */
	vm_stack = (zend_vm_stack) stack_start;
	vm_stack->top = ZEND_VM_STACK_ELEMENTS(vm_stack);
	vm_stack->end = (zval*) ((char*) vm_stack + vm_stack_size);
	vm_stack->prev = NULL;
	memset(((char *) vm_stack) + vm_stack_size, 0, sizeof(*executor) + sizeof(*execute_data));
	executor = (zend_fiber_executor *) (((char *) stack_start) + vm_stack_size);
	executor->vm_stack = vm_stack;
	executor->vm_stack_top = vm_stack->top;
	executor->vm_stack_end = vm_stack->end;
	executor->vm_stack_page_size = vm_stack_size;
	execute_data = (zend_execute_data *) (((char *) stack_start) + vm_stack_size + sizeof(*executor));
	execute_data->func = (zend_function *) &dummy_function;
	executor->current_execute_data = execute_data;

	fiber->context.stack = stack;
	fiber->context.stack_size = stack_size;
	fiber->context.ptr = make_fcontext(stack_start, user_stack_size, zend_fiber_execute);
	ZEND_ASSERT(fiber->context.ptr != NULL && "make_fcontext() never returns NULL");
#ifdef VALGRIND_STACK_REGISTER
	fiber->context.valgrind_stack_id = VALGRIND_STACK_REGISTER(stack_start, stack);
#endif
#ifdef __SANITIZE_ADDRESS__
	fiber->context.asan_fake_stack = NULL;
	fiber->context.asan_stack_bottom = stack;
	fiber->context.asan_stack_size = user_stack_size;
#endif
	fiber->executor = executor;
	fiber->execute_data = fiber->executor->current_execute_data = execute_data;

	zend_fiber_jump(fiber, data, retval);

	return true;
}

ZEND_API void zend_fiber_jump(zend_fiber *fiber, zval *data, zval *retval)
{
	zend_fiber *current_fiber = EG(current_fiber);

	zend_observer_fiber_switch_notify(current_fiber, fiber);

	fiber->from = current_fiber;
	if (current_fiber->previous == fiber) {
		/* if it is suspend, update current status to waiting and break the previous */
		if (current_fiber->status == ZEND_FIBER_STATUS_RUNNING) {
			/* if it is not finished, set status to suspended */
			current_fiber->status = ZEND_FIBER_STATUS_SUSPENDED;
		}
		current_fiber->previous = NULL;
		if (current_fiber->execute_data != NULL) {
			current_fiber->execute_data->prev_execute_data = NULL;
		}
	} else {
		/* it is not suspend, current becomes target's previous */
		ZEND_ASSERT(fiber->previous == NULL);
		fiber->previous = current_fiber;
		if (fiber->execute_data != NULL && !(fiber->flags & ZEND_FIBER_FLAG_DESTROYED)) {
			fiber->execute_data->prev_execute_data = EG(current_execute_data);
		}
	}
	fiber->status = ZEND_FIBER_STATUS_RUNNING;
	EG(current_fiber) = fiber;

	/* ZendVM executor jump */
	memcpy(current_fiber->executor, &EG(bailout), ZEND_FIBER_EXECUTOR_UNALIGNED_SIZE);
	memcpy(&EG(bailout), fiber->executor, ZEND_FIBER_EXECUTOR_UNALIGNED_SIZE);

#ifdef __SANITIZE_ADDRESS__
    __sanitizer_start_switch_fiber(
        current_fiber->status != ZEND_FIBER_STATUS_DEAD ? &current_fiber->context.asan_fake_stack : NULL,
        fiber->context.asan_stack_bottom, fiber->context.asan_stack_size
    );
#endif

	/* C stack jump */
	zend_fiber_transfer transfer;
	transfer = jump_fcontext(fiber->context.ptr, data);

	/* update from context */
	fiber = current_fiber->from;
	fiber->context.ptr = transfer.from;

#ifdef __SANITIZE_ADDRESS__
    __sanitizer_finish_switch_fiber(current_fiber->context.asan_fake_stack, &fiber->context.asan_stack_bottom, &fiber->context.asan_stack_size);
#endif

	/* copy return data */
	if (retval != NULL) {
		if (transfer.data == NULL) {
			ZVAL_NULL(retval);
		} else {
			ZVAL_COPY(retval, transfer.data);
		}
	}

	/* close the fiber if it is finished */
	if (UNEXPECTED(fiber->status == ZEND_FIBER_STATUS_DEAD)) {
#ifdef VALGRIND_STACK_DEREGISTER
		VALGRIND_STACK_DEREGISTER(fiber->context.valgrind_stack_id);
#endif
#ifdef ZEND_WIN32
		VirtualFree(fiber->context.stack, 0, MEM_RELEASE);
#else
		munmap(fiber->context.stack, fiber->context.stack_size);
#endif
	} else if (UNEXPECTED(EG(fiber_exception) != NULL)) {
		zend_fiber_handle_cross_exception(current_fiber);
	}
}

ZEND_API bool zend_fiber_resume(zend_fiber *fiber, zval *data, zval *retval)
{
	if (fiber != EG(current_fiber)->previous) {
		if (UNEXPECTED(fiber->status != ZEND_FIBER_STATUS_SUSPENDED)) {
			zend_fiber_status_error(fiber);
			return false;
		}
	}

	zend_fiber_jump(fiber, data, retval);

	return true;
}

ZEND_API bool zend_fiber_suspend(zval *data, zval *retval)
{
	zend_fiber *fiber = EG(current_fiber)->previous;

	if (UNEXPECTED(fiber == NULL)) {
		zend_throw_error(zend_ce_fiber_error, "Fiber has nowhere to go");
		return false;
	}
	if (UNEXPECTED(EG(current_fiber)->flags & ZEND_FIBER_FLAG_DESTROYED)) {
		zend_throw_error(zend_ce_fiber_error, "Cannot suspend in a force-closed fiber");
		return false;
	}

	zend_fiber_jump(fiber, data, retval);

	return true;
}

ZEND_API bool zend_fiber_throw(zend_fiber *fiber, zend_object *exception, zval *retval)
{
	GC_ADDREF(exception);
	EG(fiber_exception) = exception;

	if (!zend_fiber_resume(fiber, NULL, retval)) {
		EG(fiber_exception) = NULL;
		GC_DELREF(exception);
		return false;
	}

	return true;
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
	zend_fcall_info *fci = &fiber->fci;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(fci->params, fci->param_count, fci->named_params);
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_fiber_start(fiber, NULL, return_value)) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(Fiber, resume)
{
	zend_fiber *fiber;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data);
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	zend_fiber_resume(fiber, data, return_value);
}

ZEND_METHOD(Fiber, suspend)
{
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data);
	ZEND_PARSE_PARAMETERS_END();

	zend_fiber_suspend(data, return_value);
}

ZEND_METHOD(Fiber, throw)
{
	zend_fiber *fiber;
	zend_object *exception;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(exception, zend_ce_throwable)
	ZEND_PARSE_PARAMETERS_END();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (!zend_fiber_throw(fiber, exception, return_value)) {
		RETURN_THROWS();
	}
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

	RETURN_BOOL(fiber->status == ZEND_FIBER_STATUS_DEAD);
}

ZEND_METHOD(Fiber, getReturn)
{
	zend_fiber *fiber;
	const char *error = NULL;

	ZEND_PARSE_PARAMETERS_NONE();

	fiber = (zend_fiber *) Z_OBJ_P(getThis());

	if (fiber->status != ZEND_FIBER_STATUS_DEAD) {
		if (fiber->status == ZEND_FIBER_STATUS_INIT) {
			error = "The fiber has not been started";
		} else {
			error = "The fiber has not returned";
		}
	} else {
		if (fiber->flags & ZEND_FIBER_FLAG_BAILOUT) {
			error = "The fiber exited with fatal error";
		} else if (fiber->flags & ZEND_FIBER_FLAG_THREW) {
			error = "The fiber threw an exception";
		}
	}
	if (error != NULL) {
		zend_throw_error(zend_ce_fiber_error, "Cannot get fiber return value: %s", error);
		RETURN_THROWS();
	}

	RETURN_COPY(&fiber->retval);
}

ZEND_METHOD(Fiber, this)
{
	ZEND_PARSE_PARAMETERS_NONE();

	/* Follow RFC for now */
	if (EG(current_fiber) == EG(main_fiber)) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(&EG(current_fiber)->std);
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

static zend_fiber *zend_fiber_main_create(void)
{
	zend_fiber *fiber = (zend_fiber *) EG(vm_stack_top);
	memset(fiber, 0, sizeof(*fiber) - sizeof(fiber->std));
	fiber->status = ZEND_FIBER_STATUS_RUNNING;
	fiber->executor =  (zend_fiber_executor *) (fiber + 1);

	zend_object *object = &fiber->std;
	GC_SET_REFCOUNT(object, 1);
	GC_TYPE_INFO(object) = GC_OBJECT;
	object->ce = zend_ce_fiber;
	object->properties = NULL;
	object->handlers = &zend_fiber_handlers;

	EG(vm_stack_top) += ZEND_VM_STACK_MAIN_FIBER_SLOTS;
	EG(main_fiber) = fiber;

	return fiber;
}

static void zend_fiber_main_free(zend_fiber *fiber)
{
	zend_object *object = &fiber->std;

	ZEND_ASSERT(fiber == EG(main_fiber));
	ZEND_ASSERT(GC_REFCOUNT(object) == 1);

	GC_DELREF(object);

	/* may be created by get_properties() */
	if (object->properties && GC_DELREF(object->properties) == 0) {
		zend_array_destroy(object->properties);
	}
	if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
		zend_weakrefs_notify(object);
	}
}

void zend_fiber_init(void)
{
	EG(current_fiber) = zend_fiber_main_create();
	EG(fiber_exception) = NULL;
}

void zend_fiber_shutdown(void)
{
	zend_fiber_main_free(EG(current_fiber));
	EG(current_fiber) = NULL;
}
