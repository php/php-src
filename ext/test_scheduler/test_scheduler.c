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

/*
 * A reference scheduler for the Async Core hooks: the smallest thing that
 * can run coroutines, and nothing more. No reactor, no timers, no context
 * pool — the point is to show where the seams of the core API are.
 *
 * The execution model is the one the core prescribes:
 *
 *   - a coroutine owns its fiber context, embedded by value, so the engine
 *     never allocates anything on the scheduler's behalf;
 *   - the top-level script becomes the main coroutine by adopting the
 *     context the engine already built for it (a copy of
 *     *EG(main_fiber_context)), not by growing a second kind of object;
 *   - the loop lives in a coroutine of its own: every switch is then just a
 *     switch between two coroutines, main included;
 *   - who is running is recorded in ZEND_ASYNC_CURRENT_COROUTINE before
 *     every switch, and that is how an entry point learns which coroutine
 *     it is.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "zend_async_API.h"
#include "zend_exceptions.h"
#include "zend_fibers.h"
#include "zend_ini.h"
#include "zend_observer.h"

#include "php_test_scheduler.h"
#include "test_scheduler_arginfo.h"

typedef struct _ts_coroutine_s ts_coroutine_t;

/*
 * Growable handler vectors. Capacity doubles on overflow, add() dedupes,
 * call() compacts in place by keeping only the handlers that stay armed
 * (return true). in_execution guards against a handler mutating the vector
 * being iterated. Finish handlers carry their registration context
 * (waiter, data) by value; the vector dies with the coroutine.
 */
typedef void (*ts_handler_fn)(void);

typedef struct {
	uint32_t length;
	uint32_t capacity;
	ts_handler_fn *data;
	bool in_execution;
} ts_handlers_vector_t;

typedef struct {
	zend_coroutine_finish_handler_fn handler;
	zend_coroutine_t *waiter;
	void *data;
} ts_finish_handler_t;

typedef struct {
	uint32_t length;
	uint32_t capacity;
	ts_finish_handler_t *data;
	bool in_execution;
} ts_finish_handlers_vector_t;

/* One "what am I waiting for" registration. The vector is cleared whole at
 * enqueue: a runnable coroutine's wait descriptions are all stale. */
typedef struct {
	zend_coroutine_awaiting_info_fn handler;
	void *data;
} ts_awaiting_info_t;

typedef struct {
	uint32_t length;
	uint32_t capacity;
	ts_awaiting_info_t *data;
	bool in_execution;
} ts_awaiting_info_vector_t;

/*
 * `coro` stays first: the engine speaks zend_coroutine_t* and the two
 * pointers are interchangeable. The fiber context is embedded by value, as
 * in zend_fiber itself.
 */
struct _ts_coroutine_s {
	zend_coroutine_t coro;
	zend_fiber_context context;
	/* The context is a copy of the engine's main context: the stack behind
	 * it is the OS thread stack, and destroying it would free memory we do
	 * not own. */
	bool context_is_main;
	bool context_created;
	/* Thrown into the coroutine at the next switch into it. Owned. */
	zend_object *pending_error;
	/* The body's VM stack, captured when it returns. */
	zend_vm_stack vm_stack;
	/* The frame the coroutine is parked in while suspended: the engine reaches
	 * the stack through it (garbage collection, backtraces). */
	zend_execute_data *execute_data;
	/* All three lazily allocated (NULL until the first add) — the common case
	 * is nobody watching a given coroutine at all. */
	ts_handlers_vector_t *switch_handlers;
	ts_finish_handlers_vector_t *finish_handlers;
	ts_awaiting_info_vector_t *awaiting_info;
	/* Coroutines parked in await() on this one, woken when it finishes.
	 * Borrowed pointers: an awaiter cannot go away while suspended — the live
	 * table holds a reference. Lazily allocated. */
	struct _ts_coroutine_s **waiters;
	uint32_t waiters_length;
	uint32_t waiters_capacity;
	/* Somebody awaited the outcome: the exception is theirs, not "unhandled". */
	bool exception_observed;
	zend_object std;
};

/* The run queue: a ring buffer of ready coroutines. spawn()/resume() append,
 * the loop takes from the head; FIFO is the whole policy. Raw pointers — the
 * live table owns the reference, the queue only borrows it. */
typedef struct {
	ts_coroutine_t **data;
	size_t head;
	size_t count;
	size_t capacity;
} ts_fifo_t;

/* Same shape, for microtasks: ZEND_ASYNC_DEFER() queues here, and the loop
 * drains it once per tick, before running the next ready coroutine. */
typedef struct {
	zend_async_microtask_t **data;
	size_t head;
	size_t count;
	size_t capacity;
} ts_microtask_fifo_t;

ZEND_BEGIN_MODULE_GLOBALS(test_scheduler)
	ts_fifo_t queue;
	ts_microtask_fifo_t microtasks;
	/* Every coroutine that has not finished, one reference each. */
	HashTable coroutines;
	/* The loop. It is not a task, so it lives here and not in the table. */
	ts_coroutine_t *scheduler;
ZEND_END_MODULE_GLOBALS(test_scheduler)

ZEND_DECLARE_MODULE_GLOBALS(test_scheduler)

#define TSG(v) ZEND_MODULE_GLOBALS_ACCESSOR(test_scheduler, v)

/* Off by default: the extension claims the process-wide scheduler slots, and
 * a binary carrying it must still be able to test other providers. Tests opt
 * in with test_scheduler.enable=1. */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("test_scheduler.enable", "0", PHP_INI_SYSTEM, NULL)
PHP_INI_END()

/* False when disabled: MINIT registered nothing. */
static bool ts_registered = false;

static zend_class_entry *ts_ce_coroutine;
static zend_class_entry *ts_ce_cancellation_error;
static zend_class_entry *ts_ce_deadlock_error;
static zend_object_handlers ts_coroutine_handlers;

/* The bottom frame of every coroutine's VM stack. Nameless on purpose: a
 * frame with no function name is a dummy frame, and backtraces skip it — the
 * engine's own fiber root frame is built exactly this way. */
static zend_function ts_root_function = { ZEND_INTERNAL_FUNCTION };

static zend_always_inline ts_coroutine_t *ts_from_obj(zend_object *object)
{
	return (ts_coroutine_t *) ((char *) object - offsetof(ts_coroutine_t, std));
}

static zend_always_inline ts_coroutine_t *ts_from_coro(zend_coroutine_t *coro)
{
	return (ts_coroutine_t *) coro;
}

static bool ts_enqueue(zend_coroutine_t *coroutine, zend_object *error, bool transfer_error);

///////////////////////////////////////////////////////////////////
/// Switch/finish handler vectors
///////////////////////////////////////////////////////////////////

static uint32_t ts_handlers_add(ts_handlers_vector_t **vector_ptr, ts_handler_fn handler, const char *what)
{
	if (*vector_ptr == NULL) {
		*vector_ptr = ecalloc(1, sizeof(ts_handlers_vector_t));
	}

	ts_handlers_vector_t *vector = *vector_ptr;

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot add a %s handler while handlers are running", what);
		return 0;
	}

	for (uint32_t i = 0; i < vector->length; i++) {
		if (vector->data[i] == handler) {
			return i + 1;
		}
	}

	if (vector->length == vector->capacity) {
		vector->capacity = vector->capacity ? vector->capacity * 2 : 4;
		vector->data = safe_erealloc(vector->data, vector->capacity, sizeof(ts_handler_fn), 0);
	}

	vector->data[vector->length] = handler;
	vector->length++;

	/* 1-based: 0 is reserved for "nothing to remove" (see the ZEND_ASYNC_ADD_*
	 * failure case). */
	return vector->length;
}

static bool ts_handlers_remove(ts_handlers_vector_t *vector, uint32_t handler_id, const char *what)
{
	if (vector == NULL || handler_id == 0 || handler_id > vector->length) {
		return false;
	}

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot remove a %s handler while handlers are running", what);
		return false;
	}

	for (uint32_t i = handler_id - 1; i < vector->length - 1; i++) {
		vector->data[i] = vector->data[i + 1];
	}

	vector->length--;

	return true;
}

static void ts_handlers_free(ts_handlers_vector_t *vector)
{
	if (vector == NULL) {
		return;
	}

	if (vector->data != NULL) {
		efree(vector->data);
	}

	efree(vector);
}

/* Ask each handler whether it wants to stay armed; the ones that return false
 * are dropped, in place, in the same pass. Only the invocation differs between
 * the two lists, so the storage helpers above are shared and these are not. */
static void ts_coroutine_call_switch_handlers(ts_coroutine_t *ts, bool is_enter)
{
	ts_handlers_vector_t *vector = ts->switch_handlers;

	if (vector == NULL || vector->length == 0) {
		return;
	}

	vector->in_execution = true;

	uint32_t write_index = 0;

	for (uint32_t read_index = 0; read_index < vector->length; read_index++) {
		zend_coroutine_switch_handler_fn handler =
				(zend_coroutine_switch_handler_fn) vector->data[read_index];

		if (handler(&ts->coro, is_enter)) {
			vector->data[write_index++] = vector->data[read_index];
		}
	}

	vector->length = write_index;
	vector->in_execution = false;
}

/* Every way a coroutine can end comes through here; each entry runs once. */
static void ts_coroutine_call_finish_handlers(ts_coroutine_t *ts, const bool is_bailout)
{
	ts_finish_handlers_vector_t *vector = ts->finish_handlers;

	if (vector == NULL || vector->length == 0) {
		return;
	}

	vector->in_execution = true;

	uint32_t write_index = 0;

	for (uint32_t read_index = 0; read_index < vector->length; read_index++) {
		const ts_finish_handler_t *entry = &vector->data[read_index];

		if (entry->handler(&ts->coro, entry->waiter, entry->data, is_bailout)) {
			vector->data[write_index++] = vector->data[read_index];
		}
	}

	vector->length = write_index;
	vector->in_execution = false;
}

/* Slot functions for the core API: (coroutine, handler) -> handle. */
static uint32_t ts_add_switch_handler(zend_coroutine_t *coroutine, zend_coroutine_switch_handler_fn handler)
{
	return ts_handlers_add(&ts_from_coro(coroutine)->switch_handlers, (ts_handler_fn) handler, "switch");
}

static bool ts_remove_switch_handler(zend_coroutine_t *coroutine, uint32_t handler_id)
{
	return ts_handlers_remove(ts_from_coro(coroutine)->switch_handlers, handler_id, "switch");
}

static uint32_t ts_add_finish_handler(zend_coroutine_t *coroutine, zend_coroutine_finish_handler_fn handler,
		zend_coroutine_t *waiter, void *data)
{
	ts_coroutine_t *ts = ts_from_coro(coroutine);

	if (ts->finish_handlers == NULL) {
		ts->finish_handlers = ecalloc(1, sizeof(ts_finish_handlers_vector_t));
	}

	ts_finish_handlers_vector_t *vector = ts->finish_handlers;

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot add a finish handler while handlers are running");
		return 0;
	}

	for (uint32_t i = 0; i < vector->length; i++) {
		if (vector->data[i].handler == handler && vector->data[i].waiter == waiter
				&& vector->data[i].data == data) {
			return i + 1;
		}
	}

	if (vector->length == vector->capacity) {
		vector->capacity = vector->capacity ? vector->capacity * 2 : 4;
		vector->data = safe_erealloc(vector->data, vector->capacity, sizeof(ts_finish_handler_t), 0);
	}

	vector->data[vector->length] = (ts_finish_handler_t) { handler, waiter, data };
	vector->length++;

	/* 1-based, as in ts_handlers_add. */
	return vector->length;
}

static bool ts_remove_finish_handler(zend_coroutine_t *coroutine, uint32_t handler_id)
{
	ts_finish_handlers_vector_t *vector = ts_from_coro(coroutine)->finish_handlers;

	if (vector == NULL || handler_id == 0 || handler_id > vector->length) {
		return false;
	}

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot remove a finish handler while handlers are running");
		return false;
	}

	for (uint32_t i = handler_id - 1; i < vector->length - 1; i++) {
		vector->data[i] = vector->data[i + 1];
	}

	vector->length--;

	return true;
}

///////////////////////////////////////////////////////////////////
/// Awaiting info
///////////////////////////////////////////////////////////////////

static uint32_t ts_add_awaiting_info(
		zend_coroutine_t *coroutine, zend_coroutine_awaiting_info_fn handler, void *data)
{
	ts_coroutine_t *ts = ts_from_coro(coroutine);

	if (ts->awaiting_info == NULL) {
		ts->awaiting_info = ecalloc(1, sizeof(ts_awaiting_info_vector_t));
	}

	ts_awaiting_info_vector_t *vector = ts->awaiting_info;

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot add awaiting info while it is being collected");
		return 0;
	}

	for (uint32_t i = 0; i < vector->length; i++) {
		if (vector->data[i].handler == handler && vector->data[i].data == data) {
			return i + 1;
		}
	}

	if (vector->length == vector->capacity) {
		vector->capacity = vector->capacity ? vector->capacity * 2 : 4;
		vector->data = safe_erealloc(vector->data, vector->capacity, sizeof(ts_awaiting_info_t), 0);
	}

	vector->data[vector->length] = (ts_awaiting_info_t) { handler, data };
	vector->length++;

	/* 1-based, as in ts_handlers_add. */
	return vector->length;
}

static bool ts_remove_awaiting_info(zend_coroutine_t *coroutine, uint32_t handler_id)
{
	ts_awaiting_info_vector_t *vector = ts_from_coro(coroutine)->awaiting_info;

	if (vector == NULL || handler_id == 0 || handler_id > vector->length) {
		return false;
	}

	if (UNEXPECTED(vector->in_execution)) {
		zend_error(E_WARNING, "Cannot remove awaiting info while it is being collected");
		return false;
	}

	for (uint32_t i = handler_id - 1; i < vector->length - 1; i++) {
		vector->data[i] = vector->data[i + 1];
	}

	vector->length--;

	return true;
}

/* The full cleanup: the wait is over, every description goes with it. */
static void ts_awaiting_info_clear(ts_coroutine_t *ts)
{
	if (ts->awaiting_info != NULL) {
		ts->awaiting_info->length = 0;
	}
}

static zend_array *ts_get_awaiting_info(zend_coroutine_t *coroutine)
{
	ts_awaiting_info_vector_t *vector = ts_from_coro(coroutine)->awaiting_info;

	if (vector == NULL || vector->length == 0) {
		return NULL;
	}

	zend_array *info = zend_new_array(vector->length);

	vector->in_execution = true;

	for (uint32_t i = 0; i < vector->length; i++) {
		const ts_awaiting_info_t *entry = &vector->data[i];
		zend_string *description = entry->handler(coroutine, entry->data);

		if (description != NULL) {
			zval item;
			ZVAL_STR(&item, description);
			zend_hash_next_index_insert(info, &item);
		}
	}

	vector->in_execution = false;

	if (zend_hash_num_elements(info) == 0) {
		zend_array_destroy(info);
		return NULL;
	}

	return info;
}

///////////////////////////////////////////////////////////////////
/// Awaiters
///////////////////////////////////////////////////////////////////

static void ts_waiters_add(ts_coroutine_t *target, ts_coroutine_t *waiter)
{
	for (uint32_t i = 0; i < target->waiters_length; i++) {
		if (target->waiters[i] == waiter) {
			return;
		}
	}

	if (target->waiters_length == target->waiters_capacity) {
		target->waiters_capacity = target->waiters_capacity ? target->waiters_capacity * 2 : 4;
		target->waiters = safe_erealloc(
				target->waiters, target->waiters_capacity, sizeof(ts_coroutine_t *), 0);
	}

	target->waiters[target->waiters_length++] = waiter;
}

static void ts_waiters_remove(ts_coroutine_t *target, const ts_coroutine_t *waiter)
{
	for (uint32_t i = 0; i < target->waiters_length; i++) {
		if (target->waiters[i] == waiter) {
			target->waiters[i] = target->waiters[--target->waiters_length];
			return;
		}
	}
}

/* The awaiting-info handler ts_await() registers on the waiter: `data` is
 * the awaited coroutine (alive for the whole wait — ts_await() holds a
 * reference). */
static zend_string *ts_await_awaiting_info(zend_coroutine_t *coroutine, void *data)
{
	(void) coroutine;

	return zend_strpprintf(0, "await: coroutine #%u", ((ts_coroutine_t *) data)->std.handle);
}

/* Park the current coroutine until `coroutine` finishes. The wait holds its
 * own reference: the last outside handle may die while we are parked. */
static bool ts_await(zend_coroutine_t *coroutine)
{
	ts_coroutine_t *target = ts_from_coro(coroutine);
	zend_coroutine_t *self = ZEND_ASYNC_CURRENT_COROUTINE;

	if (UNEXPECTED(self == NULL || ZEND_ASYNC_IN_SCHEDULER_CONTEXT)) {
		zend_throw_error(NULL, "await() requires a running coroutine");
		return false;
	}

	if (UNEXPECTED(self == coroutine)) {
		zend_throw_error(NULL, "Cannot await a coroutine from within itself");
		return false;
	}

	ZEND_COROUTINE_ADD_REF(coroutine);

	/* A stray resume() can wake us early: park again until it is really over.
	 * The wake wiped the awaiting info, so each lap registers it anew. */
	while (!ZEND_COROUTINE_IS_FINISHED(coroutine)) {
		ts_waiters_add(target, ts_from_coro(self));
		ts_add_awaiting_info(self, ts_await_awaiting_info, target);

		if (!ZEND_ASYNC_SUSPEND()) {
			/* Cancelled while waiting: the outcome is no longer ours. */
			ts_waiters_remove(target, ts_from_coro(self));
			ZEND_COROUTINE_RELEASE(coroutine);
			return false;
		}
	}

	ZEND_COROUTINE_RELEASE(coroutine);

	return true;
}

///////////////////////////////////////////////////////////////////
/// Queue
///////////////////////////////////////////////////////////////////

static void ts_fifo_push(ts_fifo_t *fifo, ts_coroutine_t *ts)
{
	if (fifo->count == fifo->capacity) {
		const size_t capacity = fifo->capacity ? fifo->capacity * 2 : 8;
		ts_coroutine_t **data = safe_emalloc(capacity, sizeof(ts_coroutine_t *), 0);

		for (size_t i = 0; i < fifo->count; i++) {
			data[i] = fifo->data[(fifo->head + i) % fifo->capacity];
		}

		if (fifo->data != NULL) {
			efree(fifo->data);
		}

		fifo->data = data;
		fifo->head = 0;
		fifo->capacity = capacity;
	}

	fifo->data[(fifo->head + fifo->count) % fifo->capacity] = ts;
	fifo->count++;
}

static ts_coroutine_t *ts_fifo_shift(ts_fifo_t *fifo)
{
	if (fifo->count == 0) {
		return NULL;
	}

	ts_coroutine_t *ts = fifo->data[fifo->head];
	fifo->head = (fifo->head + 1) % fifo->capacity;
	fifo->count--;

	return ts;
}

static void ts_microtask_fifo_push(ts_microtask_fifo_t *fifo, zend_async_microtask_t *task)
{
	if (fifo->count == fifo->capacity) {
		const size_t capacity = fifo->capacity ? fifo->capacity * 2 : 8;
		zend_async_microtask_t **data = safe_emalloc(capacity, sizeof(zend_async_microtask_t *), 0);

		for (size_t i = 0; i < fifo->count; i++) {
			data[i] = fifo->data[(fifo->head + i) % fifo->capacity];
		}

		if (fifo->data != NULL) {
			efree(fifo->data);
		}

		fifo->data = data;
		fifo->head = 0;
		fifo->capacity = capacity;
	}

	fifo->data[(fifo->head + fifo->count) % fifo->capacity] = task;
	fifo->count++;
}

static zend_async_microtask_t *ts_microtask_fifo_shift(ts_microtask_fifo_t *fifo)
{
	if (fifo->count == 0) {
		return NULL;
	}

	zend_async_microtask_t *task = fifo->data[fifo->head];
	fifo->head = (fifo->head + 1) % fifo->capacity;
	fifo->count--;

	return task;
}

/* Run every microtask queued so far — the provider tick contract from
 * zend_async_API.h. Called once per scheduler loop iteration, so a
 * microtask that defers another one runs on the next tick, not this one. */
static void ts_run_microtasks(void)
{
	zend_async_microtask_t *task;

	while ((task = ts_microtask_fifo_shift(&TSG(microtasks))) != NULL) {
		if (!ZEND_ASYNC_MICROTASK_IS_CANCELLED(task)) {
			task->handler(task);
		}

		ZEND_ASYNC_MICROTASK_RELEASE(task);
	}
}

static bool ts_defer(zend_async_microtask_t *task)
{
	ts_microtask_fifo_push(&TSG(microtasks), task);

	return true;
}

///////////////////////////////////////////////////////////////////
/// The coroutine object
///////////////////////////////////////////////////////////////////

static zend_object *ts_coroutine_object_create(zend_class_entry *ce)
{
	/* Plain emalloc, not zend_object_alloc(): a 0-property internal class
	 * (no ZEND_ACC_USE_GUARDS) makes zend_object_properties_size() underflow.
	 * Fiber's object_create() does the same for the same reason. */
	ts_coroutine_t *ts = emalloc(sizeof(ts_coroutine_t));

	memset(ts, 0, offsetof(ts_coroutine_t, std));

	zend_object_std_init(&ts->std, ce);
	object_properties_init(&ts->std, ce);
	ts->std.handlers = &ts_coroutine_handlers;

	ts->coro.object_offset = offsetof(ts_coroutine_t, std);
	ZVAL_UNDEF(&ts->coro.result);
	zend_async_internal_context_init(&ts->coro);

	return &ts->std;
}

static void ts_coroutine_object_free(zend_object *object)
{
	ts_coroutine_t *ts = ts_from_obj(object);

	/* Left only when a context was created but never entered: one that ran is
	 * destroyed by the engine, and the main context is a borrowed copy. */
	if (ts->context_created && !ts->context_is_main
		&& ts->context.status != ZEND_FIBER_STATUS_DEAD) {
		zend_fiber_destroy_context(&ts->context);
	}

	if (ts->vm_stack != NULL) {
		zend_vm_stack current_stack = EG(vm_stack);

		EG(vm_stack) = ts->vm_stack;
		zend_vm_stack_destroy();
		EG(vm_stack) = current_stack;
		ts->vm_stack = NULL;
	}

	if (ts->pending_error != NULL) {
		OBJ_RELEASE(ts->pending_error);
	}

	if (ts->coro.exception != NULL) {
		OBJ_RELEASE(ts->coro.exception);
	}

	if (ts->coro.fcall != NULL) {
		ZEND_ASYNC_FCALL_FREE(ts->coro.fcall);
		ts->coro.fcall = NULL;
	}

	/* Whoever attached itself to this coroutine (a fiber, say) gets to let go
	 * of it now. */
	if (ts->coro.extended_dispose != NULL) {
		ts->coro.extended_dispose(&ts->coro);
	}

	zend_async_internal_context_destroy(&ts->coro);
	zend_async_context_destroy(&ts->coro);

	ts_handlers_free(ts->switch_handlers);

	if (ts->finish_handlers != NULL) {
		if (ts->finish_handlers->data != NULL) {
			efree(ts->finish_handlers->data);
		}
		efree(ts->finish_handlers);
	}

	if (ts->awaiting_info != NULL) {
		if (ts->awaiting_info->data != NULL) {
			efree(ts->awaiting_info->data);
		}
		efree(ts->awaiting_info);
	}

	if (ts->waiters != NULL) {
		efree(ts->waiters);
	}

	zval_ptr_dtor(&ts->coro.result);
	zend_object_std_dtor(object);
}

static HashTable *ts_coroutine_object_gc(zend_object *object, zval **table, int *num)
{
	ts_coroutine_t *ts = ts_from_obj(object);
	zend_get_gc_buffer *buf = zend_get_gc_buffer_create();

	zend_get_gc_buffer_add_zval(buf, &ts->coro.result);

	/* Owned object references: an exception's trace can point back at this
	 * very coroutine (await($self) in its arguments, say) — without these
	 * edges such a cycle never collects. */
	if (ts->coro.exception != NULL) {
		zend_get_gc_buffer_add_obj(buf, ts->coro.exception);
	}

	if (ts->pending_error != NULL) {
		zend_get_gc_buffer_add_obj(buf, ts->pending_error);
	}

	if (ts->coro.fcall != NULL) {
		zend_get_gc_buffer_add_zval(buf, &ts->coro.fcall->fci.function_name);

		for (uint32_t i = 0; i < ts->coro.fcall->fci.param_count; i++) {
			zend_get_gc_buffer_add_zval(buf, &ts->coro.fcall->fci.params[i]);
		}

		if (ts->coro.fcall->fci.named_params != NULL) {
			zend_get_gc_buffer_add_ht(buf, ts->coro.fcall->fci.named_params);
		}
	}

	/* The parked stack is deliberately NOT walked. References held by its
	 * frames (live temporaries included) are invisible to the collector, but
	 * that is safe by construction: trial deletion keeps anything whose
	 * refcount it cannot fully explain, so a frame-held object can never be
	 * collected out from under the coroutine — and the scheduler's table
	 * reference anchors the coroutine itself, so exposing frame edges cannot
	 * enable any collection either. */
	zend_get_gc_buffer_use(buf, table, num);

	return NULL;
}

static zend_coroutine_t *ts_coroutine_from_object(zend_object *object)
{
	if (object->ce != ts_ce_coroutine) {
		return NULL;
	}

	return &ts_from_obj(object)->coro;
}

static ts_coroutine_t *ts_coroutine_new(void)
{
	zend_object *object = ts_coroutine_object_create(ts_ce_coroutine);
	ts_coroutine_t *ts = ts_from_obj(object);

	/* The scheduler owns every live coroutine (userland may drop the object at
	 * once, the body still runs): its birth reference belongs to the live table. */
	zend_hash_index_add_ptr(&TSG(coroutines), object->handle, ts);

	return ts;
}

/* Drop the scheduler's reference to a finished coroutine. Never from inside
 * the coroutine itself — the object owns the fiber context, so freeing it
 * there would destroy the stack underfoot. Retire it from the loop instead,
 * once control is back on the scheduler's stack. */
static void ts_coroutine_retire(ts_coroutine_t *ts)
{
	zend_hash_index_del(&TSG(coroutines), ts->std.handle);
}

///////////////////////////////////////////////////////////////////
/// Fiber contexts
///////////////////////////////////////////////////////////////////

static ZEND_STACK_ALIGNED void ts_coroutine_entry(zend_fiber_transfer *transfer);
static ZEND_STACK_ALIGNED void ts_scheduler_entry(zend_fiber_transfer *transfer);

/* The scheduler loop follows the userland fiber.stack_size ini, but never
 * below this floor — a script may shrink the ini to nothing, which must not
 * starve the scheduler itself. */
#define TS_SCHEDULER_MIN_STACK_SIZE (128 * 1024)

static bool ts_context_create(ts_coroutine_t *ts, zend_fiber_coroutine entry, size_t stack_size)
{
	if (zend_fiber_init_context(&ts->context, ts_ce_coroutine, entry, stack_size)
		== FAILURE) {
		return false;
	}

	ts->context_created = true;

	return true;
}

/* Install the VM stack a coroutine body runs on. Mirrors what the engine
 * does for a fiber, with this scheduler's root frame at the bottom. */
static zend_execute_data *ts_vm_stack_start(ts_coroutine_t *ts)
{
	zend_long error_reporting = zend_ini_long_literal("error_reporting");

	/* NULL or an empty string means "never configured"; only "0" keeps 0. */
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
	execute_data->func = &ts_root_function;

	EG(current_execute_data) = execute_data;
	EG(jit_trace_num) = 0;
	EG(error_reporting) = (int) error_reporting;

#ifdef ZEND_CHECK_STACK_LIMIT
	EG(stack_base) = zend_fiber_stack_base(ts->context.stack);
	EG(stack_limit) = zend_fiber_stack_limit(ts->context.stack);
#endif

	return execute_data;
}

static zend_object *ts_new_error(zend_class_entry *ce, const char *message)
{
	zval error;

	object_init_ex(&error, ce);
	zend_update_property_string(ce, Z_OBJ(error), ZEND_STRL("message"), message);

	return Z_OBJ(error);
}

/* Fold into the exit exception: `exception` becomes the head, the previous one
 * hangs off it as $previous. Takes ownership. */
static void ts_exit_exception_fold(zend_object *exception)
{
	if (ZEND_ASYNC_EXIT_EXCEPTION != NULL) {
		zend_exception_set_previous(exception, ZEND_ASYNC_EXIT_EXCEPTION);
	}

	ZEND_ASYNC_EXIT_EXCEPTION = exception;
}

/* A coroutine died of an exception nobody awaited: it ends the request. In the
 * after-main drain it joins the exit exception, surfaced once the drain ends. */
static void ts_report_unhandled(ts_coroutine_t *ts)
{
	zend_object *exception = ts->coro.exception;

	ts->coro.exception = NULL;

	if (ZEND_ASYNC_MAIN_COROUTINE == NULL) {
		ts_exit_exception_fold(exception);
		return;
	}

	zend_exception_error(exception, E_ERROR);
}

/* Switch into `target`, which the caller has taken out of the queue. The
 * current-coroutine slot is written before the switch: that slot is how the
 * target's entry point learns who it is. */
static void ts_switch_into(ts_coroutine_t *target)
{
	zend_object *error = target->pending_error;
	target->pending_error = NULL;

	ZEND_COROUTINE_SET_STATUS(&target->coro, ZEND_COROUTINE_STATUS_RUNNING);
	ZEND_ASYNC_CURRENT_COROUTINE = &target->coro;
	ZEND_ASYNC_IN_SCHEDULER_CONTEXT = false;

	zend_fiber_transfer transfer = { .context = &target->context, .flags = 0 };

	if (error != NULL) {
		ZVAL_OBJ(&transfer.value, error);
		transfer.flags = ZEND_FIBER_TRANSFER_FLAG_ERROR;
	} else {
		ZVAL_NULL(&transfer.value);
	}

	zend_fiber_switch_context(&transfer);

	ZEND_ASYNC_IN_SCHEDULER_CONTEXT = true;
	ZEND_ASYNC_CURRENT_COROUTINE = &TSG(scheduler)->coro;

	zval_ptr_dtor(&transfer.value);

	if (UNEXPECTED(transfer.flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT)) {
		zend_bailout();
	}

	/* The context is dead and the engine has already destroyed it: now, on
	 * our own stack, it is safe to let the object go. */
	if (ZEND_COROUTINE_IS_FINISHED(&target->coro)) {
		/* While the main flow lives, an exception is "unhandled" only when
		 * nobody could ever look at it: no awaiter took it and no reference
		 * to the coroutine object remains (the live table holds the last
		 * one). A held object still offers await()/getException(). In the
		 * after-main drain nobody will come anymore — report everything. */
		if (target->coro.exception != NULL && !target->exception_observed
				&& !instanceof_function(target->coro.exception->ce, ts_ce_cancellation_error)
				&& (ZEND_ASYNC_MAIN_COROUTINE == NULL || GC_REFCOUNT(&target->std) == 1)) {
			ts_report_unhandled(target);
		}

		ts_coroutine_retire(target);
	}
}

///////////////////////////////////////////////////////////////////
/// Entry points
///////////////////////////////////////////////////////////////////

/* The first entry into a coroutine's context: build a VM stack, run the
 * body, record the outcome, hand control back to the scheduler. */
static ZEND_STACK_ALIGNED void ts_coroutine_entry(zend_fiber_transfer *transfer)
{
	ts_coroutine_t *ts = ts_from_coro(ZEND_ASYNC_CURRENT_COROUTINE);
	bool bailout = false;

	ZEND_ASSERT(ts != NULL && "A coroutine must be current when its context starts");

	if (UNEXPECTED(transfer->flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT)) {
		/* Unwound by ts_bailout_all() before the body ever ran: skip it. */
		bailout = true;
		zval_ptr_dtor(&transfer->value);
		ZVAL_UNDEF(&transfer->value);
	} else if (UNEXPECTED(transfer->flags & ZEND_FIBER_TRANSFER_FLAG_ERROR)) {
		/* Cancelled before the body ever ran. */
		ts->coro.exception = Z_OBJ(transfer->value);
		ZVAL_UNDEF(&transfer->value);
	} else {
		zval_ptr_dtor(&transfer->value);
		ZVAL_UNDEF(&transfer->value);

		EG(vm_stack) = NULL;

		zend_first_try {
			ts_vm_stack_start(ts);

			if (ts->coro.internal_entry != NULL) {
				ts->coro.internal_entry();
			} else {
				ts->coro.fcall->fci.retval = &ts->coro.result;
				zend_call_function(&ts->coro.fcall->fci, &ts->coro.fcall->fci_cache);
			}

			if (UNEXPECTED(EG(exception) != NULL)) {
				/* An unwind or a graceful exit is how a coroutine is told to
				 * stop, not something the body produced: it ends here. */
				if (!zend_is_unwind_exit(EG(exception)) && !zend_is_graceful_exit(EG(exception))) {
					ts->coro.exception = EG(exception);
					GC_ADDREF(ts->coro.exception);
				}

				zend_clear_exception();
			}
		} zend_catch {
			bailout = true;
		} zend_end_try();

		ts->vm_stack = EG(vm_stack);
	}

	ZEND_COROUTINE_SET_STATUS(&ts->coro, ZEND_COROUTINE_STATUS_FINISHED);

	/* Whoever is watching this coroutine finds out it is gone for good —
	 * distinct from the switch-handler LEAVE call, which fires for an
	 * ordinary suspend that is coming back. */
	ts_coroutine_call_finish_handlers(ts, bailout);

	/* Everybody parked in await() runs again; the exception, if any, is
	 * theirs to rethrow rather than an unhandled one. On a bailout the queue
	 * is dead — ts_bailout_all() unwinds the waiters itself. */
	if (!bailout && ts->waiters_length > 0) {
		ts->exception_observed = true;

		for (uint32_t i = 0; i < ts->waiters_length; i++) {
			ZEND_ASYNC_ENQUEUE_COROUTINE(&ts->waiters[i]->coro);
		}

		ts->waiters_length = 0;
	}

	ZEND_ASYNC_CURRENT_COROUTINE = &TSG(scheduler)->coro;

	transfer->context = &TSG(scheduler)->context;
	transfer->flags = bailout ? ZEND_FIBER_TRANSFER_FLAG_BAILOUT : 0;
	ZVAL_NULL(&transfer->value);
}

/* A bailout tears the request down; a coroutine parked on its own stack won't
 * unwind by itself. Switch into each once with the bailout flag so its
 * zend_first_try catches and unwinds, then control returns here. */
static void ts_bailout_all(void)
{
	for (;;) {
		ts_coroutine_t *ts = NULL;
		zval *item;

		ZEND_HASH_FOREACH_VAL(&TSG(coroutines), item) {
			ts_coroutine_t *candidate = Z_PTR_P(item);

			if (candidate->context_created && !candidate->context_is_main
				&& ZEND_COROUTINE_IS_STARTED(&candidate->coro)
				&& !ZEND_COROUTINE_IS_FINISHED(&candidate->coro)) {
				ts = candidate;
				break;
			}
		}
		ZEND_HASH_FOREACH_END();

		if (ts == NULL) {
			/* Never-started coroutines end here: their finish handlers still
			 * fire their one time. */
			ZEND_HASH_FOREACH_VAL(&TSG(coroutines), item) {
				ts_coroutine_t *candidate = Z_PTR_P(item);

				if (!ZEND_COROUTINE_IS_STARTED(&candidate->coro)
					&& !ZEND_COROUTINE_IS_FINISHED(&candidate->coro)) {
					ZEND_COROUTINE_SET_STATUS(&candidate->coro, ZEND_COROUTINE_STATUS_FINISHED);
					ts_coroutine_call_finish_handlers(candidate, /* is_bailout */ true);
				}
			}
			ZEND_HASH_FOREACH_END();

			return;
		}

		ZEND_COROUTINE_SET_STATUS(&ts->coro, ZEND_COROUTINE_STATUS_RUNNING);
		ZEND_ASYNC_CURRENT_COROUTINE = &ts->coro;
		ZEND_ASYNC_IN_SCHEDULER_CONTEXT = false;

		zend_fiber_transfer transfer = { .context = &ts->context,
			.flags = ZEND_FIBER_TRANSFER_FLAG_BAILOUT };
		ZVAL_NULL(&transfer.value);

		zend_fiber_switch_context(&transfer);

		ZEND_ASYNC_IN_SCHEDULER_CONTEXT = true;
		ZEND_ASYNC_CURRENT_COROUTINE = &TSG(scheduler)->coro;

		zval_ptr_dtor(&transfer.value);

		/* The entry's tail has already run the finish handlers. */
		ZEND_COROUTINE_SET_STATUS(&ts->coro, ZEND_COROUTINE_STATUS_FINISHED);
		ts_coroutine_retire(ts);
	}
}

/* The loop. It runs in a coroutine of its own, so handing control to the
 * scheduler is an ordinary coroutine switch — the main flow needs no special
 * case. Leaving this function ends the script: control goes back to the
 * context the engine started on. */
static ZEND_STACK_ALIGNED void ts_scheduler_entry(zend_fiber_transfer *transfer)
{
	ts_coroutine_t *self = TSG(scheduler);
	bool bailout = (transfer->flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT) != 0;

	EG(vm_stack) = NULL;

	zend_first_try {
		ts_vm_stack_start(self);

		ZEND_ASYNC_IN_SCHEDULER_CONTEXT = true;

		/* The main flow bailed out: there is nothing left to schedule, only
		 * stacks to unwind. */
		if (UNEXPECTED(bailout)) {
			ts_bailout_all();
			goto done;
		}

		for (;;) {
			ts_run_microtasks();

			ts_coroutine_t *next = ts_fifo_shift(&TSG(queue));

			if (next == NULL) {
				if (zend_hash_num_elements(&TSG(coroutines)) == 0) {
					break;
				}

				/* Nothing runnable, yet coroutines are still alive: they
				 * wait for something that will never happen, because this
				 * scheduler has no reactor to deliver it. */
				zend_coroutine_t *main_coroutine = ZEND_ASYNC_MAIN_COROUTINE;

				if (main_coroutine == NULL) {
					/* No main to report a deadlock to: this is the after-main
					 * shutdown drain. Force-close whatever is still parked so it
					 * unwinds through its finally blocks while the scheduler is
					 * live — after this async deactivates and no later teardown
					 * could run it. */
					bool cancelled_any = false;
					zval *item;

					ZEND_HASH_FOREACH_VAL(&TSG(coroutines), item) {
						ts_coroutine_t *leftover = Z_PTR_P(item);

						if (ZEND_COROUTINE_IS_STARTED(&leftover->coro)
								&& !ZEND_COROUTINE_IS_FINISHED(&leftover->coro)) {
							ZEND_ASYNC_CANCEL(&leftover->coro, zend_create_graceful_exit(), true);
							cancelled_any = true;
						}
					} ZEND_HASH_FOREACH_END();

					if (!cancelled_any) {
						break;
					}

					continue;
				}

				/* Terminal: DeadlockError becomes the exit exception; each
				 * parked coroutine gets a catchable cancellation. */
				uint32_t waiting = 0;
				zval *parked_item;

				ZEND_HASH_FOREACH_VAL(&TSG(coroutines), parked_item) {
					ts_coroutine_t *parked = Z_PTR_P(parked_item);

					if (ZEND_COROUTINE_IS_SUSPENDED(&parked->coro)) {
						waiting++;
					}
				}
				ZEND_HASH_FOREACH_END();

				char message[128];
				snprintf(message, sizeof(message),
						"Deadlock detected: no active coroutines, %u coroutines in waiting",
						waiting);
				ts_exit_exception_fold(ts_new_error(ts_ce_deadlock_error, message));

				ZEND_HASH_FOREACH_VAL(&TSG(coroutines), parked_item) {
					ts_coroutine_t *parked = Z_PTR_P(parked_item);

					if (ZEND_COROUTINE_IS_SUSPENDED(&parked->coro)) {
						ZEND_ASYNC_CANCEL(&parked->coro,
								ts_new_error(ts_ce_cancellation_error, "Deadlock detected"),
								true);
					}
				}
				ZEND_HASH_FOREACH_END();

				continue;
			}

			/* Keep EG(exception) clean before the graceful exit is delivered
			 * into `next`: thrown on top of a pending error it would chain onto
			 * the internal marker (bogus dynamic-property notice) and be lost.
			 * A cancellation is the scheduler's own doing — dropped. */
			if (ZEND_ASYNC_MAIN_COROUTINE == NULL && EG(exception) != NULL) {
				if (instanceof_function(EG(exception)->ce, ts_ce_cancellation_error)) {
					OBJ_RELEASE(EG(exception));
				} else {
					ts_exit_exception_fold(EG(exception));
				}
				EG(exception) = NULL;
			}

			ts_switch_into(next);
		}

		/* Drain done: surface the collected chain once. Graceful-exit and
		 * cancellation remnants on EG are dropped in its favour. */
		if (EG(exception) != NULL) {
			if (zend_is_graceful_exit(EG(exception)) || zend_is_unwind_exit(EG(exception))
					|| instanceof_function(EG(exception)->ce, ts_ce_cancellation_error)) {
				OBJ_RELEASE(EG(exception));
			} else {
				ts_exit_exception_fold(EG(exception));
			}
			EG(exception) = NULL;
		}

		if (ZEND_ASYNC_EXIT_EXCEPTION != NULL) {
			zend_object *exception = ZEND_ASYNC_EXIT_EXCEPTION;
			ZEND_ASYNC_EXIT_EXCEPTION = NULL;
			zend_exception_error(exception, E_ERROR);
		}

done:;
	} zend_catch {
		/* A coroutine bailed out and the flag travelled here: unwind the rest
		 * before the request goes down. */
		bailout = true;
		ts_bailout_all();
	} zend_end_try();

	self->vm_stack = EG(vm_stack);
	ZEND_COROUTINE_SET_STATUS(&self->coro, ZEND_COROUTINE_STATUS_FINISHED);

	/* A bailout has to be re-raised on the stack that owns the request. If the
	 * main coroutine is still parked in a suspend, that is where it goes; once
	 * main is gone, the context the engine started on is the only one left. */
	zend_coroutine_t *main_coroutine = ZEND_ASYNC_MAIN_COROUTINE;

	if (bailout && main_coroutine != NULL && !ZEND_COROUTINE_IS_FINISHED(main_coroutine)) {
		ZEND_ASYNC_CURRENT_COROUTINE = main_coroutine;
		ZEND_COROUTINE_SET_STATUS(main_coroutine, ZEND_COROUTINE_STATUS_RUNNING);
		transfer->context = &ts_from_coro(main_coroutine)->context;
	} else {
		ZEND_ASYNC_CURRENT_COROUTINE = NULL;
		transfer->context = EG(main_fiber_context);
	}

	transfer->flags = bailout ? ZEND_FIBER_TRANSFER_FLAG_BAILOUT : 0;
	ZVAL_NULL(&transfer->value);
}

///////////////////////////////////////////////////////////////////
/// Launch: the script becomes a coroutine
///////////////////////////////////////////////////////////////////

/* Ensure a loop coroutine to switch into. It runs exactly once and its context
 * dies with it, so a request needing another (destructors that spawn, the
 * post-bailout drain) rebuilds it on demand. Not a task: it stays out of the
 * live table, or it would count among the coroutines it waits for. */
static bool ts_scheduler_ensure(void)
{
	ts_coroutine_t *scheduler = TSG(scheduler);

	if (scheduler != NULL) {
		if (scheduler->context.status != ZEND_FIBER_STATUS_DEAD
			&& !ZEND_COROUTINE_IS_FINISHED(&scheduler->coro)) {
			return true;
		}

		TSG(scheduler) = NULL;
		OBJ_RELEASE(&scheduler->std);
	}

	scheduler = ts_from_obj(ts_coroutine_object_create(ts_ce_coroutine));

	size_t scheduler_stack_size = EG(fiber_stack_size) < TS_SCHEDULER_MIN_STACK_SIZE
			? TS_SCHEDULER_MIN_STACK_SIZE
			: EG(fiber_stack_size);

	if (!ts_context_create(scheduler, ts_scheduler_entry, scheduler_stack_size)) {
		OBJ_RELEASE(&scheduler->std);
		zend_throw_error(NULL, "Failed to create the scheduler's fiber context");
		return false;
	}

	TSG(scheduler) = scheduler;

	return true;
}

/* Wrap the engine's top-level context (EG(main_fiber_context), the OS thread
 * stack) in a coroutine. A copy, so the coroutine owns a switchable handle
 * while the engine's original stays the context execution lands back on.
 * Called at first launch and again from ts_main_suspend() for shutdown. */
static ts_coroutine_t *ts_adopt_main_context(void)
{
	ts_coroutine_t *main_coro = ts_coroutine_new();

	zend_fiber_context *zero_context = EG(main_fiber_context);

	main_coro->context = *zero_context;
	main_coro->context_is_main = true;
	main_coro->context_created = true;

	EG(current_fiber_context) = &main_coro->context;

	main_coro->context.status = ZEND_FIBER_STATUS_INIT;
	zend_observer_fiber_switch_notify(zero_context, &main_coro->context);
	main_coro->context.status = ZEND_FIBER_STATUS_RUNNING;

	/* ts_main_suspend() calls this after the loop coroutine's run, which
	 * leaves this flag set; the adopted context is never the loop's own. */
	ZEND_ASYNC_IN_SCHEDULER_CONTEXT = false;

	return main_coro;
}

/* Called once per request. The main flow's identity is never relaunched here
 * again — when index.php's coroutine finishes, ts_main_suspend() mints the
 * replacement in place instead. */
static zend_coroutine_t *ts_launch(void)
{
	ts_coroutine_t *main_coro = ts_adopt_main_context();

	if (!ts_scheduler_ensure()) {
		return NULL;
	}

	return &main_coro->coro;
}

///////////////////////////////////////////////////////////////////
/// The Async Core slots
///////////////////////////////////////////////////////////////////

static zend_coroutine_t *ts_new_coroutine(size_t extra_size)
{
	(void) extra_size;

	return &ts_coroutine_new()->coro;
}

/* This reference scheduler treats a GC coroutine exactly like any other:
 * the FIFO run queue has no notion of priority to give it. A scheduler that
 * does would tell them apart here. */
static zend_coroutine_t *ts_gc_new_coroutine(void)
{
	return &ts_coroutine_new()->coro;
}

static bool ts_enqueue(zend_coroutine_t *coroutine, zend_object *error, bool transfer_error)
{
	ts_coroutine_t *ts = ts_from_coro(coroutine);

	/* Finished: nothing to run, the enqueue is a no-op. */
	if (UNEXPECTED(ZEND_COROUTINE_IS_FINISHED(coroutine))) {
		if (error != NULL && transfer_error) {
			OBJ_RELEASE(error);
		}

		return true;
	}

	/* Thrown at the suspension point when the coroutine runs. */
	if (error != NULL) {
		if (ts->pending_error != NULL) {
			OBJ_RELEASE(ts->pending_error);
		}

		ts->pending_error = error;

		if (!transfer_error) {
			GC_ADDREF(error);
		}
	}

	/* The script may be long over: a destructor cancelling a suspended fiber,
	 * or a fresh Fiber::start() from one, still needs a live loop. Only the
	 * loop is rebuilt on demand (the main identity never is — see ts_launch()). */
	if (UNEXPECTED(!ts_scheduler_ensure())) {
		return false;
	}

	if (ZEND_COROUTINE_IS_QUEUED(coroutine)) {
		return true;
	}

	if (!ts->context_created && !ts_context_create(ts, ts_coroutine_entry, EG(fiber_stack_size))) {
		/* zend_fiber_init_context() already threw (e.g. a stack too small):
		 * don't bury that with a second error the caller can't catch. */
		if (!EG(exception)) {
			zend_throw_error(NULL, "Failed to create a fiber context for the coroutine");
		}
		return false;
	}

	/* Runnable again: every wait description is stale — drop them all. */
	ts_awaiting_info_clear(ts);

	ZEND_COROUTINE_SET_STATUS(coroutine, ZEND_COROUTINE_STATUS_QUEUED);
	ts_fifo_push(&TSG(queue), ts);

	return true;
}

static zend_execute_data *ts_coroutine_execute_data(zend_coroutine_t *coroutine)
{
	ts_coroutine_t *ts = ts_from_coro(coroutine);

	return ZEND_COROUTINE_IS_SUSPENDED(coroutine) ? ts->execute_data : NULL;
}

/* Cancellation is a resume with an error: the coroutine wakes inside the
 * suspend it is parked in, the error is thrown there, and the body unwinds
 * through its own finally blocks. */
static bool ts_cancel(
		zend_coroutine_t *coroutine, zend_object *error, bool transfer_error, const bool is_safely)
{
	(void) is_safely;

	/* Nothing to unwind, or a cancellation is already in flight. The last case
	 * matters most: a fiber destroyed from inside its own force-close re-enters
	 * here for the coroutine it is already unwinding — re-enqueuing it leaves a
	 * stale entry that the loop later switches into after the context is gone.
	 * Cancellation is idempotent: the first graceful exit wins. */
	if (ZEND_COROUTINE_IS_FINISHED(coroutine) || !ZEND_COROUTINE_IS_STARTED(coroutine)
			|| ZEND_COROUTINE_IS_CANCELLED(coroutine)) {
		if (error != NULL && transfer_error) {
			OBJ_RELEASE(error);
		}

		return true;
	}

	ZEND_COROUTINE_SET_CANCELLED(coroutine);

	return ts_enqueue(coroutine, error, transfer_error);
}

/* index.php is over; main hands off to the loop, which drains and returns
 * here. The old main is then replaced in place (a fresh one re-adopts
 * EG(main_fiber_context)) rather than reset to READY for a lazy relaunch:
 * this late, ts_launch() would copy a mid-unwind context (crash: "Invalid
 * fiber context"). Async stays ACTIVE until the single ZEND_ASYNC_DEACTIVATE
 * in php_request_shutdown(). */
static bool ts_main_suspend(bool is_bailout)
{
	ts_coroutine_t *main_coro = ts_from_coro(ZEND_ASYNC_MAIN_COROUTINE);

	/* The loop may already have run to completion (a bailout it handed to
	 * main, say): what is left to drain needs a live one. */
	if (!ts_scheduler_ensure()) {
		return false;
	}

	ts_coroutine_t *scheduler = TSG(scheduler);

	ZEND_COROUTINE_SET_STATUS(&main_coro->coro, ZEND_COROUTINE_STATUS_FINISHED);
	ts_coroutine_call_finish_handlers(main_coro, is_bailout);

	/* Main finishes here, not in ts_coroutine_entry: wake its awaiters too. */
	if (!is_bailout && main_coro->waiters_length > 0) {
		for (uint32_t i = 0; i < main_coro->waiters_length; i++) {
			ZEND_ASYNC_ENQUEUE_COROUTINE(&main_coro->waiters[i]->coro);
		}

		main_coro->waiters_length = 0;
	}

	ZEND_ASYNC_MAIN_COROUTINE = NULL;
	ts_coroutine_retire(main_coro);

	/* Back on the context the engine owns: the copy the main coroutine ran
	 * on describes the same stack, and it dies with the object. */
	EG(current_fiber_context) = EG(main_fiber_context);
	ZEND_ASYNC_CURRENT_COROUTINE = &scheduler->coro;

	zend_fiber_transfer transfer = { .context = &scheduler->context,
		.flags = is_bailout ? ZEND_FIBER_TRANSFER_FLAG_BAILOUT : 0 };
	ZVAL_NULL(&transfer.value);

	zend_fiber_switch_context(&transfer);

	/* The loop is done and has handed control back to EG(main_fiber_context)
	 * — this stack, right here. Its own coroutine is spent either way. */
	TSG(scheduler) = NULL;
	OBJ_RELEASE(&scheduler->std);

	/* A live main coroutine is mandatory past this point: mint the replacement
	 * before anything else (the bailout re-raise below) runs without one. */
	ts_coroutine_t *shutdown_coro = ts_adopt_main_context();
	ZEND_COROUTINE_SET_MAIN(&shutdown_coro->coro);
	ZEND_COROUTINE_SET_STATUS(&shutdown_coro->coro, ZEND_COROUTINE_STATUS_RUNNING);
	ZEND_ASYNC_MAIN_COROUTINE = &shutdown_coro->coro;
	ZEND_ASYNC_CURRENT_COROUTINE = &shutdown_coro->coro;

	const bool bailout = (transfer.flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT) != 0;

	zval_ptr_dtor(&transfer.value);

	if (UNEXPECTED(bailout)) {
		zend_bailout();
	}

	return EG(exception) == NULL;
}

static bool ts_suspend(bool from_main, bool is_bailout)
{
	if (from_main) {
		return ts_main_suspend(is_bailout);
	}

	ts_coroutine_t *self = ts_from_coro(ZEND_ASYNC_CURRENT_COROUTINE);

	if (UNEXPECTED(self == NULL)) {
		zend_throw_error(NULL, "There is no coroutine to suspend");
		return false;
	}

	if (UNEXPECTED(ZEND_ASYNC_IN_SCHEDULER_CONTEXT)) {
		zend_throw_error(NULL, "A coroutine cannot be suspended from the scheduler context");
		return false;
	}

	/* Cancelled: never park again, nothing will wake it. */
	if (UNEXPECTED(ZEND_COROUTINE_IS_CANCELLED(&self->coro))) {
		zend_throw_exception(ts_ce_cancellation_error, "The coroutine has been cancelled", 0);
		return false;
	}

	ZEND_COROUTINE_SET_STATUS(&self->coro, ZEND_COROUTINE_STATUS_SUSPENDED);
	self->execute_data = EG(current_execute_data);
	ZEND_ASYNC_CURRENT_COROUTINE = &TSG(scheduler)->coro;

	/* Whoever is watching this coroutine (see zend_coroutine_switch_handler_fn)
	 * finds out right here, synchronously, that it is leaving — not on some
	 * later tick that may never come. */
	ts_coroutine_call_switch_handlers(self, false);

	zend_fiber_transfer transfer = { .context = &TSG(scheduler)->context, .flags = 0 };
	ZVAL_NULL(&transfer.value);

	zend_fiber_switch_context(&transfer);

	/* The request is going down: re-raise the bailout on this stack so it
	 * unwinds like any other. */
	if (UNEXPECTED(transfer.flags & ZEND_FIBER_TRANSFER_FLAG_BAILOUT)) {
		zval_ptr_dtor(&transfer.value);
		zend_bailout();
	}

	ts_coroutine_call_switch_handlers(self, true);

	/* Resumed: the loop has recorded us as current again, and an error the
	 * resumer handed us is thrown at this exact point. */
	if (UNEXPECTED(transfer.flags & ZEND_FIBER_TRANSFER_FLAG_ERROR)) {
		zend_throw_exception_internal(Z_OBJ(transfer.value));
		return false;
	}

	zval_ptr_dtor(&transfer.value);

	return EG(exception) == NULL;
}

static bool ts_shutdown(void)
{
	php_printf("[scheduler] shutdown requested\n");

	return true;
}

/* Every application fiber becomes a coroutine here — this scheduler keeps no
 * fibers of its own out, so the answer is always yes. */
static zend_coroutine_t *ts_intercept_fiber(zend_fiber *fiber)
{
	(void) fiber;

	return &ts_coroutine_new()->coro;
}

static zend_class_entry *ts_get_class_ce(zend_async_class type)
{
	switch (type) {
		case ZEND_ASYNC_CLASS_COROUTINE:
			return ts_ce_coroutine;
		case ZEND_ASYNC_EXCEPTION_CANCELLATION:
			return ts_ce_cancellation_error;
		default:
			return zend_ce_exception;
	}
}

///////////////////////////////////////////////////////////////////
/// Userland API
///////////////////////////////////////////////////////////////////

PHP_FUNCTION(TestScheduler_spawn)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	uint32_t args_count = 0;
	zval *args = NULL;
	HashTable *named_args = NULL;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_VARIADIC_WITH_NAMED(args, args_count, named_args)
	ZEND_PARSE_PARAMETERS_END();

	/* Without a live scheduler (the CLI's -r never launches one, and the
	 * request's final drain may be over) the coroutine would sit in the
	 * queue forever. Another provider's scheduler is not ours either. */
	if (UNEXPECTED(!ts_registered || !ZEND_ASYNC_IS_ACTIVE)) {
		zend_throw_error(NULL, "The scheduler is not running");
		RETURN_THROWS();
	}

	ts_coroutine_t *ts = ts_coroutine_new();

	ZEND_ASYNC_FCALL_DEFINE(fcall, fci, fcc, args, args_count, named_args);
	ts->coro.fcall = fcall;

	if (!ts_enqueue(&ts->coro, NULL, false)) {
		ts_coroutine_retire(ts);
		RETURN_THROWS();
	}

	RETURN_OBJ_COPY(&ts->std);
}

PHP_FUNCTION(TestScheduler_suspend)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (!ZEND_ASYNC_SUSPEND()) {
		RETURN_THROWS();
	}
}

PHP_FUNCTION(TestScheduler_resume)
{
	zend_object *object = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(object, ts_ce_coroutine)
	ZEND_PARSE_PARAMETERS_END();

	ts_coroutine_t *ts = ts_from_obj(object);

	if (!ZEND_COROUTINE_IS_SUSPENDED(&ts->coro)) {
		zend_throw_error(NULL, "Cannot resume a coroutine that is not suspended");
		RETURN_THROWS();
	}

	if (!ZEND_ASYNC_ENQUEUE_COROUTINE(&ts->coro)) {
		RETURN_THROWS();
	}
}

PHP_FUNCTION(TestScheduler_cancel)
{
	zend_object *object = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(object, ts_ce_coroutine)
	ZEND_PARSE_PARAMETERS_END();

	ts_coroutine_t *ts = ts_from_obj(object);

	/* Self-cancel: no suspend point to deliver into, throw in place. */
	if (&ts->coro == ZEND_ASYNC_CURRENT_COROUTINE) {
		ZEND_COROUTINE_SET_CANCELLED(&ts->coro);
		zend_throw_exception(ts_ce_cancellation_error, "The coroutine has been cancelled", 0);
		RETURN_THROWS();
	}

	if (!ZEND_ASYNC_CANCEL(&ts->coro,
			ts_new_error(ts_ce_cancellation_error, "The coroutine has been cancelled"), true)) {
		RETURN_THROWS();
	}
}

PHP_FUNCTION(TestScheduler_await)
{
	zend_object *object = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(object, ts_ce_coroutine)
	ZEND_PARSE_PARAMETERS_END();

	ts_coroutine_t *target = ts_from_obj(object);

	if (!ts_await(&target->coro)) {
		RETURN_THROWS();
	}

	if (target->coro.exception != NULL) {
		/* Still on the coroutine, so it was never reported as unhandled:
		 * every awaiter rethrows its own reference. */
		target->exception_observed = true;

		GC_ADDREF(target->coro.exception);
		zend_throw_exception_internal(target->coro.exception);
		RETURN_THROWS();
	}

	if (Z_ISUNDEF(target->coro.result)) {
		RETURN_NULL();
	}

	RETURN_COPY(&target->coro.result);
}

PHP_FUNCTION(TestScheduler_current)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_coroutine_t *coroutine = ZEND_ASYNC_CURRENT_COROUTINE;

	if (coroutine == NULL) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(ZEND_COROUTINE_OBJECT(coroutine));
}

PHP_METHOD(TestScheduler_Coroutine, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(NULL, "A coroutine is created through TestScheduler\\spawn()");
}

#define TS_STATUS_METHOD(name, predicate) \
	PHP_METHOD(TestScheduler_Coroutine, name) \
	{ \
		ZEND_PARSE_PARAMETERS_NONE(); \
		RETURN_BOOL(predicate(&ts_from_obj(Z_OBJ_P(ZEND_THIS))->coro)); \
	}

TS_STATUS_METHOD(isStarted, ZEND_COROUTINE_IS_STARTED)
TS_STATUS_METHOD(isRunning, ZEND_COROUTINE_IS_RUNNING)
TS_STATUS_METHOD(isSuspended, ZEND_COROUTINE_IS_SUSPENDED)
TS_STATUS_METHOD(isFinished, ZEND_COROUTINE_IS_FINISHED)

PHP_METHOD(TestScheduler_Coroutine, getResult)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ts_coroutine_t *ts = ts_from_obj(Z_OBJ_P(ZEND_THIS));

	if (Z_ISUNDEF(ts->coro.result)) {
		RETURN_NULL();
	}

	RETURN_COPY(&ts->coro.result);
}

PHP_METHOD(TestScheduler_Coroutine, getException)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ts_coroutine_t *ts = ts_from_obj(Z_OBJ_P(ZEND_THIS));

	if (ts->coro.exception == NULL) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(ts->coro.exception);
}

PHP_METHOD(TestScheduler_Coroutine, getAwaitingInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_array *info = ZEND_ASYNC_GET_AWAITING_INFO(&ts_from_obj(Z_OBJ_P(ZEND_THIS))->coro);

	if (info == NULL) {
		RETURN_EMPTY_ARRAY();
	}

	RETURN_ARR(info);
}

///////////////////////////////////////////////////////////////////
/// Module
///////////////////////////////////////////////////////////////////

static const zend_async_scheduler_api_t ts_scheduler_api = {
	.size = sizeof(zend_async_scheduler_api_t),
	.new_coroutine = ts_new_coroutine,
	.gc_new_coroutine = ts_gc_new_coroutine,
	.enqueue_coroutine = ts_enqueue,
	.suspend = ts_suspend,
	.launch = ts_launch,
	.shutdown = ts_shutdown,
	.cancel = ts_cancel,
	.get_class_ce = ts_get_class_ce,
	.coroutine_from_object = ts_coroutine_from_object,
	.intercept_fiber = ts_intercept_fiber,
	.coroutine_execute_data = ts_coroutine_execute_data,
	.defer = ts_defer,
	.add_switch_handler = ts_add_switch_handler,
	.remove_switch_handler = ts_remove_switch_handler,
	.add_finish_handler = ts_add_finish_handler,
	.remove_finish_handler = ts_remove_finish_handler,
	.await = ts_await,
	.add_awaiting_info = ts_add_awaiting_info,
	.remove_awaiting_info = ts_remove_awaiting_info,
	.get_awaiting_info = ts_get_awaiting_info,
};

static PHP_GINIT_FUNCTION(test_scheduler)
{
#if defined(COMPILE_DL_TEST_SCHEDULER) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(test_scheduler_globals, 0, sizeof(*test_scheduler_globals));
}

static void ts_coroutine_table_dtor(zval *item)
{
	ts_coroutine_t *ts = Z_PTR_P(item);

	OBJ_RELEASE(&ts->std);
}

PHP_MINIT_FUNCTION(test_scheduler)
{
	REGISTER_INI_ENTRIES();

	if (!zend_ini_long(ZEND_STRL("test_scheduler.enable"), 0)) {
		return SUCCESS;
	}

	ts_ce_coroutine = register_class_TestScheduler_Coroutine();
	ts_ce_coroutine->create_object = ts_coroutine_object_create;

	ts_ce_cancellation_error = register_class_TestScheduler_CancellationError(zend_ce_error);
	ts_ce_deadlock_error = register_class_TestScheduler_DeadlockError(zend_ce_error);

	memcpy(&ts_coroutine_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	ts_coroutine_handlers.offset = offsetof(ts_coroutine_t, std);
	ts_coroutine_handlers.free_obj = ts_coroutine_object_free;
	ts_coroutine_handlers.get_gc = ts_coroutine_object_gc;
	ts_coroutine_handlers.clone_obj = NULL;

	if (!zend_async_scheduler_register("test_scheduler", &ts_scheduler_api)) {
		return FAILURE;
	}

	ts_registered = true;

	return SUCCESS;
}

PHP_RINIT_FUNCTION(test_scheduler)
{
#if defined(ZTS) && defined(COMPILE_DL_TEST_SCHEDULER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	if (!ts_registered) {
		return SUCCESS;
	}

	zend_hash_init(&TSG(coroutines), 8, NULL, ts_coroutine_table_dtor, 0);

	ZEND_ASYNC_INITIALIZE;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(test_scheduler)
{
	if (!ts_registered) {
		return SUCCESS;
	}

	if (TSG(queue).data != NULL) {
		efree(TSG(queue).data);
		TSG(queue).data = NULL;
	}

	TSG(queue).head = 0;
	TSG(queue).count = 0;
	TSG(queue).capacity = 0;

	/* Anything still queued never got its tick: release it without running
	 * the handler, same as a cancelled task. */
	zend_async_microtask_t *task;
	while ((task = ts_microtask_fifo_shift(&TSG(microtasks))) != NULL) {
		ZEND_ASYNC_MICROTASK_RELEASE(task);
	}

	if (TSG(microtasks).data != NULL) {
		efree(TSG(microtasks).data);
		TSG(microtasks).data = NULL;
	}

	TSG(microtasks).head = 0;
	TSG(microtasks).count = 0;
	TSG(microtasks).capacity = 0;

	/* Only a bailout that cut the drain short can leave this set. */
	if (ZEND_ASYNC_EXIT_EXCEPTION != NULL) {
		OBJ_RELEASE(ZEND_ASYNC_EXIT_EXCEPTION);
		ZEND_ASYNC_EXIT_EXCEPTION = NULL;
	}

	zend_hash_destroy(&TSG(coroutines));

	return SUCCESS;
}

zend_module_entry test_scheduler_module_entry = {
	STANDARD_MODULE_HEADER,
	"test_scheduler",
	ext_functions,
	PHP_MINIT(test_scheduler),
	NULL,
	PHP_RINIT(test_scheduler),
	PHP_RSHUTDOWN(test_scheduler),
	NULL,
	PHP_TEST_SCHEDULER_VERSION,
	PHP_MODULE_GLOBALS(test_scheduler),
	PHP_GINIT(test_scheduler),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX,
};

#ifdef COMPILE_DL_TEST_SCHEDULER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(test_scheduler)
#endif
