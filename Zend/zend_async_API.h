/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Edmond <edmondifthen@proton.me>                             |
   +----------------------------------------------------------------------+
*/
#ifndef ZEND_ASYNC_API_H
#define ZEND_ASYNC_API_H

#include "zend_API.h"
#include "zend_gc.h"

/*
 * Async Core ABI.
 *
 * This header is intentionally thin: it contains only the coroutine data
 * structure and the function-pointer slots a scheduler implementation
 * fills in. It contains NO policy: no scheduler, no reactor, no event
 * system. How a coroutine waits — and for what — is entirely the
 * provider's business; the core only offers the awaiting-info handler
 * slots so that the wait state can be inspected for diagnostics.
 *
 * Execution belongs to the provider as well: fiber contexts, their pool
 * and the entry point a coroutine starts on live in the scheduler, not
 * here.
 */

typedef struct _zend_coroutine_s zend_coroutine_t;
typedef struct _zend_fcall_s zend_fcall_t;
typedef struct _zend_fiber zend_fiber;
typedef void (*zend_coroutine_entry_t)(void);

/* Class/exception registry keys resolved through zend_async_get_class_ce_fn. */
typedef enum {
	ZEND_ASYNC_CLASS_NO = 0,
	ZEND_ASYNC_CLASS_COROUTINE = 1,

	ZEND_ASYNC_EXCEPTION_DEFAULT = 30,
	ZEND_ASYNC_EXCEPTION_CANCELLATION = 31,
} zend_async_class;

struct _zend_fcall_s {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
};

///////////////////////////////////////////////////////////////////
/// Coroutine
///////////////////////////////////////////////////////////////////

typedef void (*zend_coroutine_dispose_fn)(zend_coroutine_t *coroutine);

/**
 * Awaiting-info handler: describes ONE thing the coroutine is waiting for
 * ("poll: socket 12, readable"). Registered by whoever suspends the
 * coroutine, with `data` handed back verbatim. Registrations form a vector;
 * the scheduler wipes it whole when the coroutine is enqueued — every
 * description dies with the wait. Diagnostics only: no scheduling effect.
 * The caller owns the returned string; NULL when there is nothing to say.
 */
typedef zend_string *(*zend_coroutine_awaiting_info_fn)(zend_coroutine_t *coroutine, void *data);

/*
 * Fired synchronously the moment the scheduler switches a coroutine out
 * (is_enter=false) or back in (is_enter=true). Synchronous on purpose: a
 * microtask runs only on the next tick, which this late in shutdown may never
 * come, so the handler has to react at the switch itself. Return false to drop
 * the handler after this call, true to keep it armed.
 *
 * Finishing is a separate mechanism (zend_coroutine_finish_handler_fn): one
 * coroutine may have many awaiters, which doesn't fit a switch handler. Both
 * lists live with the scheduler, not on zend_coroutine_t.
 */
typedef bool (*zend_coroutine_switch_handler_fn)(zend_coroutine_t *coroutine, bool is_enter);

/* Fires exactly once, however the coroutine ends — return, exception,
 * cancellation, bailout unwind. Clearing ->exception here marks it handled.
 * waiter/data are stored at registration and handed back verbatim: waiter is
 * the coroutine parked on this one (NULL when it hides behind data), data is
 * the consumer's context. is_bailout=true: the scheduler is dying — clean own
 * state only, do not schedule anything. */
typedef bool (*zend_coroutine_finish_handler_fn)(
		zend_coroutine_t *coroutine, zend_coroutine_t *waiter, void *data, bool is_bailout);

/**
 * Coroutine lifecycle. The single source of truth, managed by the
 * scheduler.
 */
typedef enum {
	ZEND_COROUTINE_STATUS_CREATED = 0, /* spawned, never executed */
	ZEND_COROUTINE_STATUS_QUEUED, /* ready, waiting in the run queue */
	ZEND_COROUTINE_STATUS_RUNNING, /* currently executing */
	ZEND_COROUTINE_STATUS_SUSPENDED, /* waiting; see awaiting_info */
	ZEND_COROUTINE_STATUS_FINISHED /* completed; result or exception is set */
} zend_coroutine_status;

struct _zend_coroutine_s {
	/* Bits 0-3: zend_coroutine_status (the scheduler is the only writer);
	 * bits 4+: ZEND_COROUTINE_F_* modifiers. */
	uint32_t flags;
	/* Offset of the wrapping zend_object within the allocation, when the
	 * coroutine is embedded in one (single-allocation pattern: the object
	 * and the coroutine share one block, reached via container_of). 0 for
	 * a plain C coroutine with no PHP object. */
	uint32_t object_offset;
	/* Userland entry point. NULL for internal coroutines. */
	zend_fcall_t *fcall;
	/* C entry point. NULL for userland coroutines. */
	zend_coroutine_entry_t internal_entry;
	/* Custom data of the scheduler/extension. Nullable. */
	void *extended_data;
	/* Completion result. */
	zval result;
	/* Completion exception. Nullable. */
	zend_object *exception;
	/* Spawn location (diagnostics). */
	zend_string *filename;
	uint32_t lineno;
	/* Extended dispose handler. Nullable. */
	zend_coroutine_dispose_fn extended_dispose;
	/* Coroutine-local storage; the provider initialises and destroys both at
	 * the coroutine's birth and death. */
	HashTable internal_context; /* C extensions, numeric keys; PHP never sees it */
	zend_object *context; /* the Async\Context object, lazy */
};

/* The lifecycle status is packed into the low 4 bits of `flags`. */
#define ZEND_COROUTINE_STATUS_MASK 0xFu

#define ZEND_COROUTINE_STATUS(coroutine) \
	((zend_coroutine_status) ((coroutine)->flags & ZEND_COROUTINE_STATUS_MASK))
#define ZEND_COROUTINE_SET_STATUS(coroutine, _status) \
	((coroutine)->flags = \
			((coroutine)->flags & ~ZEND_COROUTINE_STATUS_MASK) | (uint32_t) (_status))

/* Orthogonal modifiers packed above the status bits. */
#define ZEND_COROUTINE_F_CANCELLED (1u << 4) /* cancellation was requested */
#define ZEND_COROUTINE_F_MAIN (1u << 5) /* the main coroutine */
#define ZEND_COROUTINE_F_FIBER (1u << 6) /* runs a fiber; extended_data -> zend_fiber */
/* object_offset points at a stored zend_object* instead of an embedded
 * object, for a provider whose coroutine and object live in separate
 * allocations. */
#define ZEND_COROUTINE_F_OBJ_REF (1u << 7)

#define ZEND_COROUTINE_IS_CANCELLED(coroutine) \
	(((coroutine)->flags & ZEND_COROUTINE_F_CANCELLED) != 0)
#define ZEND_COROUTINE_SET_CANCELLED(coroutine) \
	((coroutine)->flags |= ZEND_COROUTINE_F_CANCELLED)

#define ZEND_COROUTINE_IS_MAIN(coroutine) (((coroutine)->flags & ZEND_COROUTINE_F_MAIN) != 0)
#define ZEND_COROUTINE_SET_MAIN(coroutine) ((coroutine)->flags |= ZEND_COROUTINE_F_MAIN)

#define ZEND_COROUTINE_IS_FIBER(coroutine) (((coroutine)->flags & ZEND_COROUTINE_F_FIBER) != 0)
#define ZEND_COROUTINE_SET_FIBER(coroutine) ((coroutine)->flags |= ZEND_COROUTINE_F_FIBER)

/* The zend_object of a coroutine, or NULL for a plain C coroutine.
 * Embedded model: the object lives at object_offset within the same
 * allocation. OBJ_REF model: a zend_object* is stored at object_offset. */
#define ZEND_COROUTINE_OBJECT(coroutine) \
	((coroutine)->object_offset == 0 \
					? NULL \
					: ((coroutine)->flags & ZEND_COROUTINE_F_OBJ_REF) \
							? *(zend_object **) ((char *) (coroutine) + (coroutine)->object_offset) \
							: (zend_object *) ((char *) (coroutine) + (coroutine)->object_offset))

/* Shared ownership of a coroutine goes through its zend_object. */
#define ZEND_COROUTINE_ADD_REF(coroutine) \
	do { \
		zend_object *_object = ZEND_COROUTINE_OBJECT(coroutine); \
		ZEND_ASSERT(_object != NULL && "A coroutine is backed by a zend_object"); \
		GC_ADDREF(_object); \
	} while (0)

#define ZEND_COROUTINE_RELEASE(coroutine) \
	do { \
		zend_object *_object = ZEND_COROUTINE_OBJECT(coroutine); \
		ZEND_ASSERT(_object != NULL && "A coroutine is backed by a zend_object"); \
		OBJ_RELEASE(_object); \
	} while (0)

/* Lifecycle predicates over the packed status. */
#define ZEND_COROUTINE_IS_STARTED(coroutine) \
	(ZEND_COROUTINE_STATUS(coroutine) != ZEND_COROUTINE_STATUS_CREATED)
#define ZEND_COROUTINE_IS_QUEUED(coroutine) \
	(ZEND_COROUTINE_STATUS(coroutine) == ZEND_COROUTINE_STATUS_QUEUED)
#define ZEND_COROUTINE_IS_RUNNING(coroutine) \
	(ZEND_COROUTINE_STATUS(coroutine) == ZEND_COROUTINE_STATUS_RUNNING)
#define ZEND_COROUTINE_IS_SUSPENDED(coroutine) \
	(ZEND_COROUTINE_STATUS(coroutine) == ZEND_COROUTINE_STATUS_SUSPENDED)
#define ZEND_COROUTINE_IS_FINISHED(coroutine) \
	(ZEND_COROUTINE_STATUS(coroutine) == ZEND_COROUTINE_STATUS_FINISHED)

/**
 * Build a zend_fcall_t from PHP function parameters
 * (Z_PARAM_FUNC + Z_PARAM_VARIADIC_WITH_NAMED).
 */
#define ZEND_ASYNC_FCALL_DEFINE(_fcall_var, _src_fci, _src_fcc, _src_args, _src_args_count, _src_named_args) \
	zend_fcall_t *_fcall_var = ecalloc(1, sizeof(zend_fcall_t)); \
	_fcall_var->fci = _src_fci; \
	_fcall_var->fci_cache = _src_fcc; \
	if (_src_args_count) { \
		_fcall_var->fci.param_count = _src_args_count; \
		_fcall_var->fci.params = safe_emalloc(_src_args_count, sizeof(zval), 0); \
		for (uint32_t _fcall_i = 0; _fcall_i < _src_args_count; _fcall_i++) { \
			ZVAL_COPY(&_fcall_var->fci.params[_fcall_i], &_src_args[_fcall_i]); \
		} \
	} \
	if (_src_named_args) { \
		_fcall_var->fci.named_params = _src_named_args; \
		GC_ADDREF(_src_named_args); \
	} \
	Z_TRY_ADDREF(_fcall_var->fci.function_name);

/* The inverse of ZEND_ASYNC_FCALL_DEFINE: releases everything the macro
 * copied and frees the zend_fcall_t. Safe on NULL. */
#define ZEND_ASYNC_FCALL_FREE(_fcall_var) \
	do { \
		zend_fcall_t *_fcall = (_fcall_var); \
		if (_fcall != NULL) { \
			zend_fcall_info_args_clear(&_fcall->fci, true); \
			if (_fcall->fci.named_params != NULL) { \
				zend_array_release(_fcall->fci.named_params); \
			} \
			zval_ptr_dtor(&_fcall->fci.function_name); \
			efree(_fcall); \
		} \
	} while (0)

///////////////////////////////////////////////////////////////////
/// Scheduler API slots
///////////////////////////////////////////////////////////////////

/* Allocate a coroutine in STATUS_CREATED; extra_size bytes are appended
 * for the caller. */
typedef zend_coroutine_t *(*zend_async_new_coroutine_t)(size_t extra_size);
/* Allocate a coroutine for the engine's own GC bookkeeping (running
 * zend_gc_collect_cycles() and its destructor phase). A separate slot lets a
 * scheduler treat these specially — priority, concurrency limits — if it cares.
 * NULL falls back to new_coroutine(0); see ZEND_ASYNC_GC_NEW_COROUTINE(). */
typedef zend_coroutine_t *(*zend_async_gc_new_coroutine_t)(void);
/* Put a CREATED/SUSPENDED coroutine into the run queue (-> STATUS_QUEUED).
 * Enqueuing a fresh coroutine and resuming a suspended one are the same
 * operation. A non-NULL `error` is thrown at the suspension point when the
 * coroutine runs — how cancellation and IO/timeout failures reach waiting
 * code; transfer_error passes ownership of the reference. */
typedef bool (*zend_async_enqueue_coroutine_t)(
		zend_coroutine_t *coroutine, zend_object *error, bool transfer_error);
/* Yield the current coroutine (-> STATUS_SUSPENDED) and give control to the
 * scheduler. Returns after somebody enqueues the coroutine; a delivered error
 * is rethrown inside it, so the caller checks EG(exception) as usual.
 * `from_main` = true is the after-main handoff: the main script (or its
 * destructors) has finished and remaining coroutines get to run;
 * `is_bailout` tells the scheduler the main flow ended with a bailout. */
typedef bool (*zend_async_suspend_t)(bool from_main, bool is_bailout);
/* Request cancellation: sets F_CANCELLED and wakes the coroutine with the
 * error. `is_safely` defers delivery until a cancellation-safe point. */
typedef bool (*zend_async_cancel_t)(
		zend_coroutine_t *coroutine, zend_object *error, bool transfer_error, const bool is_safely);
/* Start the scheduler and hand the engine the main coroutine: the top-level
 * script is a coroutine from its first opcode, not a plain flow that becomes
 * one at its first yield. The scheduler creates it (it is the scheduler's own
 * object) and the engine records it as the main and the current coroutine.
 * Returning NULL is a failure: without a main coroutine there is no flow to
 * run the script in. */
typedef zend_coroutine_t *(*zend_async_scheduler_launch_t)(void);
typedef bool (*zend_async_shutdown_t)(void);
typedef zend_class_entry *(*zend_async_get_class_ce_t)(zend_async_class type);
/* Run fn(arg) on the main coroutine's OS-thread stack (FFI/JNI etc.). */
typedef void (*zend_async_call_on_main_stack_t)(void (*fn)(void *), void *arg);

/*
 * Microtask: a one-shot task executed on the next scheduler tick.
 *
 * A structure with a lifetime: refcount ownership, cancellable at any
 * moment. The consumer embeds it in its own container (container_of).
 * The queue is OWNED BY THE PROVIDER; the core only routes the pointer
 * through the defer slot. Provider tick contract:
 *
 *     if (!ZEND_ASYNC_MICROTASK_IS_CANCELLED(task)) task->handler(task);
 *     ZEND_ASYNC_MICROTASK_RELEASE(task);
 */
typedef struct _zend_async_microtask_s zend_async_microtask_t;
typedef void (*zend_microtask_handler_fn)(zend_async_microtask_t *task);

struct _zend_async_microtask_s {
	/* Runs on the tick; NOT invoked for a cancelled task. */
	zend_microtask_handler_fn handler;
	/* Releases the container's resources when the last reference dies.
	 * NULL means a plain efree of the task. */
	zend_microtask_handler_fn dtor;
	/* A full 32-bit reference counter. */
	uint32_t ref_count;
	/* 32 bits of named flags. */
	uint32_t is_cancelled : 1;
	uint32_t reserved : 31;
};

#define ZEND_ASYNC_MICROTASK_IS_CANCELLED(task) ((task)->is_cancelled != 0)
#define ZEND_ASYNC_MICROTASK_CANCEL(task) ((task)->is_cancelled = 1)

#define ZEND_ASYNC_MICROTASK_ADDREF(task) ((task)->ref_count++)

#define ZEND_ASYNC_MICROTASK_RELEASE(task) \
	do { \
		if (--(task)->ref_count == 0) { \
			if ((task)->dtor != NULL) { \
				(task)->dtor(task); \
			} \
			efree(task); \
		} \
	} while (0)

/* Queue the task on the provider's microtask queue. */
typedef bool (*zend_async_defer_t)(zend_async_microtask_t *task);

/* Park the current coroutine until `coroutine` finishes. A slot, not a series
 * of calls, because awaiting is a scheduling decision: the provider owns the
 * waiter bookkeeping (it lives on the awaited coroutine), may wake the waiter
 * with a direct switch instead of the run queue, and marks the outcome as
 * observed. False when the wait was aborted — a cancellation delivered to the
 * waiter, or misuse (no current coroutine, awaiting itself). */
typedef bool (*zend_async_coroutine_await_t)(zend_coroutine_t *coroutine);

/*
 * Resolve the provider's coroutine object to its coroutine.
 *
 * The coroutine object belongs to the scheduler (its class, its allocation
 * model), so only the scheduler can walk that edge: the core keeps no
 * registry of coroutines. Returns NULL when the object is not one of the
 * scheduler's coroutines.
 */
typedef zend_coroutine_t *(*zend_async_coroutine_from_object_t)(zend_object *object);

/*
 * Where the engine offers a starting fiber to the scheduler.
 *
 * A fiber can be one of two things. A low-level one switches contexts by
 * itself (that is how a userland event loop drives its own fibers) and the
 * scheduler must not touch it. A high-level one is application code, and
 * under a scheduler it is a coroutine like any other: it is queued, and its
 * body runs on the context the scheduler hands it, never on a context of its
 * own.
 *
 * Called on Fiber::start() while the API is active. The scheduler answers
 * with a coroutine to run the fiber as, or with NULL to leave the fiber on
 * the engine's legacy path. Only the scheduler can tell its own fibers from
 * the application's, which is also what keeps it from adopting itself.
 */
typedef zend_coroutine_t *(*zend_async_intercept_fiber_t)(zend_fiber *fiber);

/* The frame a suspended coroutine is parked in, or NULL when it is not
 * suspended (or the provider does not track it). The stack a coroutine runs
 * on belongs to the scheduler, so this is the only way for the engine to
 * reach it — the garbage collector needs it to see the variables alive on
 * that stack, and a backtrace needs it to walk past the coroutine. */
typedef zend_execute_data *(*zend_async_coroutine_execute_data_t)(zend_coroutine_t *coroutine);

/*
 * Switch/finish handler storage lives with the scheduler (it already owns the
 * coroutine's allocation); the core only adds and removes. Add returns a handle
 * for remove(); 0 means the add failed (or the API is inactive), with nothing
 * to remove.
 */
typedef uint32_t (*zend_async_coroutine_add_switch_handler_t)(
		zend_coroutine_t *coroutine, zend_coroutine_switch_handler_fn handler);
typedef bool (*zend_async_coroutine_remove_switch_handler_t)(
		zend_coroutine_t *coroutine, uint32_t handler_id);
typedef uint32_t (*zend_async_coroutine_add_finish_handler_t)(
		zend_coroutine_t *coroutine, zend_coroutine_finish_handler_fn handler,
		zend_coroutine_t *waiter, void *data);
typedef bool (*zend_async_coroutine_remove_finish_handler_t)(
		zend_coroutine_t *coroutine, uint32_t handler_id);

/*
 * Awaiting-info storage follows the same model: a vector owned by the
 * scheduler, add/remove by handle (0 = the add did nothing). An explicit
 * remove is rarely needed — enqueue wipes the vector whole; it is for a
 * reason that disappears while the coroutine stays parked.
 */
typedef uint32_t (*zend_async_coroutine_add_awaiting_info_t)(
		zend_coroutine_t *coroutine, zend_coroutine_awaiting_info_fn handler, void *data);
typedef bool (*zend_async_coroutine_remove_awaiting_info_t)(
		zend_coroutine_t *coroutine, uint32_t handler_id);
/* Ask every registered handler and collect the non-NULL descriptions into a
 * packed array of strings. NULL when the coroutine is not waiting for
 * anything it can name. The caller owns the array. */
typedef zend_array *(*zend_async_coroutine_get_awaiting_info_t)(zend_coroutine_t *coroutine);

/**
 * Scheduler API bundle. A provider fills the struct and calls
 * zend_async_scheduler_register(). New slots are appended at the end only;
 * `size` lets the core detect how much of the struct the provider knows.
 * ABI compatibility rides on the standard PHP module API (ZEND_MODULE_API_NO),
 * enforced when the provider extension is loaded — there is no separate
 * Async API version.
 */
typedef struct _zend_async_scheduler_api_s {
	size_t size; /* sizeof(zend_async_scheduler_api_t) at provider build time */

	zend_async_new_coroutine_t new_coroutine;
	zend_async_gc_new_coroutine_t gc_new_coroutine;
	zend_async_enqueue_coroutine_t enqueue_coroutine;
	zend_async_suspend_t suspend;
	zend_async_cancel_t cancel;
	zend_async_scheduler_launch_t launch;
	zend_async_shutdown_t shutdown;
	zend_async_get_class_ce_t get_class_ce;
	zend_async_call_on_main_stack_t call_on_main_stack;
	zend_async_defer_t defer;
	zend_async_coroutine_from_object_t coroutine_from_object;
	zend_async_intercept_fiber_t intercept_fiber;
	zend_async_coroutine_execute_data_t coroutine_execute_data;
	zend_async_coroutine_add_switch_handler_t add_switch_handler;
	zend_async_coroutine_remove_switch_handler_t remove_switch_handler;
	zend_async_coroutine_add_finish_handler_t add_finish_handler;
	zend_async_coroutine_remove_finish_handler_t remove_finish_handler;
	zend_async_coroutine_await_t await;
	zend_async_coroutine_add_awaiting_info_t add_awaiting_info;
	zend_async_coroutine_remove_awaiting_info_t remove_awaiting_info;
	zend_async_coroutine_get_awaiting_info_t get_awaiting_info;
} zend_async_scheduler_api_t;

BEGIN_EXTERN_C()

ZEND_API extern zend_async_new_coroutine_t zend_async_new_coroutine_fn;
ZEND_API extern zend_async_gc_new_coroutine_t zend_async_gc_new_coroutine_fn;
ZEND_API extern zend_async_enqueue_coroutine_t zend_async_enqueue_coroutine_fn;
ZEND_API extern zend_async_suspend_t zend_async_suspend_fn;
ZEND_API extern zend_async_cancel_t zend_async_cancel_fn;
ZEND_API extern zend_async_scheduler_launch_t zend_async_scheduler_launch_fn;
ZEND_API extern zend_async_shutdown_t zend_async_shutdown_fn;
ZEND_API extern zend_async_get_class_ce_t zend_async_get_class_ce_fn;
ZEND_API extern zend_async_call_on_main_stack_t zend_async_call_on_main_stack_fn;
ZEND_API extern zend_async_defer_t zend_async_defer_fn;
ZEND_API extern zend_async_coroutine_from_object_t zend_async_coroutine_from_object_fn;
ZEND_API extern zend_async_intercept_fiber_t zend_async_intercept_fiber_fn;
ZEND_API extern zend_async_coroutine_execute_data_t zend_async_coroutine_execute_data_fn;
ZEND_API extern zend_async_coroutine_add_switch_handler_t zend_async_coroutine_add_switch_handler_fn;
ZEND_API extern zend_async_coroutine_remove_switch_handler_t zend_async_coroutine_remove_switch_handler_fn;
ZEND_API extern zend_async_coroutine_add_finish_handler_t zend_async_coroutine_add_finish_handler_fn;
ZEND_API extern zend_async_coroutine_remove_finish_handler_t zend_async_coroutine_remove_finish_handler_fn;
ZEND_API extern zend_async_coroutine_await_t zend_async_coroutine_await_fn;
ZEND_API extern zend_async_coroutine_add_awaiting_info_t zend_async_coroutine_add_awaiting_info_fn;
ZEND_API extern zend_async_coroutine_remove_awaiting_info_t zend_async_coroutine_remove_awaiting_info_fn;
ZEND_API extern zend_async_coroutine_get_awaiting_info_t zend_async_coroutine_get_awaiting_info_fn;

/* Resolve a coroutine object to its coroutine through the provider's slot.
 * NULL when no scheduler is registered, when it provides no resolver, or
 * when the object is not one of its coroutines. */
ZEND_API zend_coroutine_t *zend_async_coroutine_from_object(zend_object *object);

/* Launch the scheduler: calls the launch slot, marks the coroutine it returns
 * as the main one and records it as current. False when no scheduler is
 * registered or it failed to produce a main coroutine. */
ZEND_API bool zend_async_scheduler_launch(void);

ZEND_API bool zend_async_scheduler_register(
		const char *module, const zend_async_scheduler_api_t *api);
/* Withdraw the registration and reset every slot to its default. */
ZEND_API void zend_async_scheduler_unregister(void);

ZEND_API bool zend_async_is_enabled(void);
/* The module name of the registered scheduler, or NULL when none. */
ZEND_API const char *zend_async_get_scheduler_module(void);

/* The internal (C-extension) context: engine-owned per-coroutine storage
 * under process-unique numeric keys, allocated once per process from a
 * static C-string name (typically at MINIT). NULL coroutine = the current
 * one. Values may hold raw C data, so the store is structurally
 * unreachable from PHP. */
ZEND_API uint32_t zend_async_internal_context_key_alloc(const char *key_name);
ZEND_API zval *zend_async_internal_context_find(zend_coroutine_t *coroutine, uint32_t key);
ZEND_API bool zend_async_internal_context_set(
		zend_coroutine_t *coroutine, uint32_t key, zval *value);
ZEND_API bool zend_async_internal_context_unset(zend_coroutine_t *coroutine, uint32_t key);
/* The embedded table's birth and death: the provider calls init when it
 * creates the coroutine and destroy when the coroutine dies. */
ZEND_API void zend_async_internal_context_init(zend_coroutine_t *coroutine);
ZEND_API void zend_async_internal_context_destroy(zend_coroutine_t *coroutine);

/*
 * The userland context: engine storage with no engine class. The core owns
 * the layout and the operations; a provider extension registers the PHP
 * class (Async\Context) and hands the factory through
 * zend_async_new_context_fn. An extension may wrap the struct with its own
 * fields in front — std sits at a fixed offset, so an object always maps
 * back to the base with ZEND_ASYNC_CONTEXT_FROM_OBJ.
 *
 * String keys live in string_keys; object keys in object_keys, indexed by
 * handle, where the entry owns the key object as well as the value —
 * handles are reused once an object dies, and a stored key that outlived
 * its object would alias whatever takes its handle next.
 */
typedef struct {
	HashTable string_keys;
	HashTable object_keys;
	zend_object std;
} zend_async_context_t;

#define ZEND_ASYNC_CONTEXT_FROM_OBJ(object) \
	((zend_async_context_t *) ((char *) (object) - offsetof(zend_async_context_t, std)))

/* Mints a context instance (the provider's class). NULL while no provider
 * registered a class: the userland context is then unavailable. */
typedef zend_object *(*zend_async_new_context_t)(void);
ZEND_API extern zend_async_new_context_t zend_async_new_context_fn;

/* The struct-level operations: the provider's class methods and free/GC
 * handlers are thin wrappers over these. Keys are a zend_string OR a
 * zend_object, exactly one non-NULL. */
ZEND_API void zend_async_context_tables_init(zend_async_context_t *context);
ZEND_API void zend_async_context_tables_destroy(zend_async_context_t *context);
ZEND_API zval *zend_async_context_entry_find(
		zend_async_context_t *context, zend_string *skey, zend_object *okey);
ZEND_API void zend_async_context_entry_set(
		zend_async_context_t *context, zend_string *skey, zend_object *okey, zval *value);
ZEND_API bool zend_async_context_entry_unset(
		zend_async_context_t *context, zend_string *skey, zend_object *okey);
ZEND_API void zend_async_context_entry_gc(zend_async_context_t *context, zend_get_gc_buffer *buf);

/* The coroutine-level view (NULL coroutine = the current one). NULL when
 * there is no coroutine (concurrency off) or no class provider. Keys here
 * are string-or-object zvals; any other type is the C caller's bug. */
ZEND_API zend_object *zend_async_context_get(zend_coroutine_t *coroutine);
ZEND_API zval *zend_async_context_find(zend_coroutine_t *coroutine, zval *key);
ZEND_API bool zend_async_context_set(zend_coroutine_t *coroutine, zval *key, zval *value);
ZEND_API bool zend_async_context_unset(zend_coroutine_t *coroutine, zval *key);
ZEND_API void zend_async_context_destroy(zend_coroutine_t *coroutine);

END_EXTERN_C()

/* The userland context (NULL coroutine = current, main included). */
#define ZEND_ASYNC_CONTEXT_GET(coroutine) zend_async_context_get(coroutine)
#define ZEND_ASYNC_CONTEXT_FIND(coroutine, key) zend_async_context_find((coroutine), (key))
#define ZEND_ASYNC_CONTEXT_SET(coroutine, key, value) \
	zend_async_context_set((coroutine), (key), (value))
#define ZEND_ASYNC_CONTEXT_UNSET(coroutine, key) zend_async_context_unset((coroutine), (key))
#define ZEND_ASYNC_CONTEXT_DESTROY(coroutine) zend_async_context_destroy(coroutine)

/* The internal context (NULL coroutine = current). */
#define ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC(name) zend_async_internal_context_key_alloc(name)
#define ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, key) \
	zend_async_internal_context_find((coroutine), (key))
#define ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, key, value) \
	zend_async_internal_context_set((coroutine), (key), (value))
#define ZEND_ASYNC_INTERNAL_CONTEXT_UNSET(coroutine, key) \
	zend_async_internal_context_unset((coroutine), (key))
#define ZEND_ASYNC_INTERNAL_CONTEXT_DESTROY(coroutine) \
	zend_async_internal_context_destroy(coroutine)

#define ZEND_ASYNC_NEW_COROUTINE() zend_async_new_coroutine_fn(0)
#define ZEND_ASYNC_NEW_COROUTINE_EX(extra_size) zend_async_new_coroutine_fn(extra_size)
/* A coroutine for the engine's own GC bookkeeping. Falls back to the plain
 * new_coroutine slot when the scheduler does not distinguish them; NULL when
 * the provider cannot mint C coroutines at all. */
#define ZEND_ASYNC_GC_NEW_COROUTINE() \
	(zend_async_gc_new_coroutine_fn != NULL \
					? zend_async_gc_new_coroutine_fn() \
					: zend_async_new_coroutine_fn != NULL ? zend_async_new_coroutine_fn(0) : NULL)
#define ZEND_ASYNC_ENQUEUE_COROUTINE(coroutine) \
	zend_async_enqueue_coroutine_fn((coroutine), NULL, false)
/* Enqueue-with-error: the resume/cancellation delivery channel. */
#define ZEND_ASYNC_ENQUEUE_WITH_ERROR(coroutine, error, transfer_error) \
	zend_async_enqueue_coroutine_fn((coroutine), (error), (transfer_error))
#define ZEND_ASYNC_SUSPEND() zend_async_suspend_fn(false, false)
/* Park the current coroutine until `coroutine` finishes. */
#define ZEND_ASYNC_AWAIT(coroutine) zend_async_coroutine_await_fn(coroutine)
/* Hand control to the scheduler one last time after the main flow ends.
 * Safe to call unconditionally: a no-op while the Async API is inactive. */
#define ZEND_ASYNC_RUN_SCHEDULER_AFTER_MAIN(is_bailout) \
	do { \
		if (ZEND_ASYNC_IS_ACTIVE) { \
			zend_async_suspend_fn(true, (is_bailout)); \
		} \
	} while (0)
#define ZEND_ASYNC_CANCEL(coroutine, error, transfer_error) \
	zend_async_cancel_fn((coroutine), (error), (transfer_error), false)
/* Starts the scheduler and installs the main coroutine it returns. */
#define ZEND_ASYNC_SCHEDULER_LAUNCH() zend_async_scheduler_launch()
#define ZEND_ASYNC_SHUTDOWN() zend_async_shutdown_fn()
#define ZEND_ASYNC_GET_CE(type) zend_async_get_class_ce_fn(type)
#define ZEND_ASYNC_GET_EXCEPTION_CE(type) zend_async_get_class_ce_fn(type)
#define ZEND_ASYNC_CALL_ON_MAIN_STACK(fn, arg) zend_async_call_on_main_stack_fn((fn), (arg))
#define ZEND_ASYNC_DEFER(task) zend_async_defer_fn(task)

/* The frame a suspended coroutine is parked in, or NULL. */
#define ZEND_ASYNC_COROUTINE_EXECUTE_DATA(coroutine) \
	(zend_async_coroutine_execute_data_fn != NULL \
					? zend_async_coroutine_execute_data_fn(coroutine) \
					: NULL)

/* The coroutine to run a starting fiber as, or NULL for the legacy path. A
 * scheduler with no intercept_fiber slot leaves every fiber alone. */
#define ZEND_ASYNC_INTERCEPT_FIBER(fiber) \
	((ZEND_ASYNC_IS_ACTIVE && zend_async_intercept_fiber_fn != NULL) \
					? zend_async_intercept_fiber_fn(fiber) \
					: NULL)

/* Watch `coroutine` switching in/out (ZEND_ASYNC_ADD_SWITCH_HANDLER) or
 * finishing (ZEND_ASYNC_ADD_FINISH_HANDLER). The scheduler owns the list;
 * these only add/remove by handle. 0 means the add did nothing (no
 * scheduler, or the provider does not support it) — there is nothing to
 * remove in that case. */
#define ZEND_ASYNC_ADD_SWITCH_HANDLER(coroutine, handler) \
	(zend_async_coroutine_add_switch_handler_fn != NULL \
					? zend_async_coroutine_add_switch_handler_fn((coroutine), (handler)) \
					: 0)
#define ZEND_ASYNC_REMOVE_SWITCH_HANDLER(coroutine, handler_id) \
	((void) ((handler_id) != 0 && zend_async_coroutine_remove_switch_handler_fn != NULL \
					&& zend_async_coroutine_remove_switch_handler_fn((coroutine), (handler_id))))
#define ZEND_ASYNC_ADD_FINISH_HANDLER(coroutine, handler, waiter, data) \
	(zend_async_coroutine_add_finish_handler_fn != NULL \
					? zend_async_coroutine_add_finish_handler_fn((coroutine), (handler), (waiter), (data)) \
					: 0)
#define ZEND_ASYNC_REMOVE_FINISH_HANDLER(coroutine, handler_id) \
	((void) ((handler_id) != 0 && zend_async_coroutine_remove_finish_handler_fn != NULL \
					&& zend_async_coroutine_remove_finish_handler_fn((coroutine), (handler_id))))

/* Describe what `coroutine` waits for (ZEND_ASYNC_ADD_AWAITING_INFO) and
 * fetch the collected descriptions (ZEND_ASYNC_GET_AWAITING_INFO — a packed
 * array of strings owned by the caller, or NULL). The scheduler clears the
 * registrations itself when the coroutine is enqueued. */
#define ZEND_ASYNC_ADD_AWAITING_INFO(coroutine, handler, data) \
	(zend_async_coroutine_add_awaiting_info_fn != NULL \
					? zend_async_coroutine_add_awaiting_info_fn((coroutine), (handler), (data)) \
					: 0)
#define ZEND_ASYNC_REMOVE_AWAITING_INFO(coroutine, handler_id) \
	((void) ((handler_id) != 0 && zend_async_coroutine_remove_awaiting_info_fn != NULL \
					&& zend_async_coroutine_remove_awaiting_info_fn((coroutine), (handler_id))))
#define ZEND_ASYNC_GET_AWAITING_INFO(coroutine) \
	(zend_async_coroutine_get_awaiting_info_fn != NULL \
					? zend_async_coroutine_get_awaiting_info_fn(coroutine) \
					: NULL)

///////////////////////////////////////////////////////////////////
/// Globals
///////////////////////////////////////////////////////////////////

typedef enum {
	ZEND_ASYNC_OFF,
	ZEND_ASYNC_READY,
	ZEND_ASYNC_ACTIVE
} zend_async_state_t;

typedef struct {
	zend_async_state_t state;
	/* Currently executing coroutine. NULL outside coroutine context.
	 * Written by the scheduler before every switch: this slot, not the
	 * fiber context, is what tells a starting coroutine who it is. */
	zend_coroutine_t *coroutine;
	/* The main coroutine (top-level script on the OS thread stack). */
	zend_coroutine_t *main_coroutine;
	/* Number of live (not finished) coroutines. */
	unsigned int active_coroutine_count;
	/* True while the scheduler's own machinery runs. */
	bool in_scheduler_context;
	/* Uncaught exception carried out of the shutdown drain. */
	zend_object *exit_exception;
} zend_async_globals_t;

BEGIN_EXTERN_C()
#ifdef ZTS
ZEND_API extern int zend_async_globals_id;
ZEND_API extern size_t zend_async_globals_offset;
#define ZEND_ASYNC_G(v) ZEND_TSRMG_FAST(zend_async_globals_offset, zend_async_globals_t *, v)
#else
ZEND_API extern zend_async_globals_t zend_async_globals_api;
#define ZEND_ASYNC_G(v) (zend_async_globals_api.v)
#endif

void zend_async_globals_ctor(void);
void zend_async_globals_dtor(void);
void zend_async_api_shutdown(void);

END_EXTERN_C()

#define ZEND_ASYNC_ON (ZEND_ASYNC_G(state) > ZEND_ASYNC_OFF)
#define ZEND_ASYNC_IS_ACTIVE (ZEND_ASYNC_G(state) == ZEND_ASYNC_ACTIVE)
#define ZEND_ASYNC_IS_OFF (ZEND_ASYNC_G(state) == ZEND_ASYNC_OFF)
#define ZEND_ASYNC_IS_READY (ZEND_ASYNC_G(state) == ZEND_ASYNC_READY)
#define ZEND_ASYNC_ACTIVATE ZEND_ASYNC_G(state) = ZEND_ASYNC_ACTIVE
#define ZEND_ASYNC_INITIALIZE ZEND_ASYNC_G(state) = ZEND_ASYNC_READY
#define ZEND_ASYNC_DEACTIVATE ZEND_ASYNC_G(state) = ZEND_ASYNC_OFF

#define ZEND_ASYNC_CURRENT_COROUTINE ZEND_ASYNC_G(coroutine)
#define ZEND_ASYNC_MAIN_COROUTINE ZEND_ASYNC_G(main_coroutine)
#define ZEND_ASYNC_EXIT_EXCEPTION ZEND_ASYNC_G(exit_exception)
#define ZEND_ASYNC_ACTIVE_COROUTINE_COUNT ZEND_ASYNC_G(active_coroutine_count)
#define ZEND_ASYNC_IN_SCHEDULER_CONTEXT ZEND_ASYNC_G(in_scheduler_context)

#endif /* ZEND_ASYNC_API_H */
