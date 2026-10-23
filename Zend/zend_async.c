/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:          |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

/* A minimal async/await runtime built on top of Fibers.
 *
 * Every call to Async\async() (and therefore every call of an
 * "async function") runs its callback in a new Fiber and returns an
 * Async\Future. The fiber starts eagerly and runs until it awaits a Future
 * that is not complete yet; at that point it suspends and the caller gets
 * back a pending Future.
 *
 * Awaiting from inside a task suspends the task. Awaiting from outside of a
 * task (e.g. the top-level script) drives the scheduler until the awaited
 * Future completes: ready tasks are resumed in FIFO order and the process
 * sleeps until the next Async\delay() timer is due when nothing is ready. */

#include "zend.h"
#include "zend_API.h"
#include "zend_ast.h"
#include "zend_async.h"
#include "zend_exceptions.h"
#include "zend_fibers.h"
#include "zend_hrtime.h"
#include "zend_interfaces.h"
#include "zend_async_arginfo.h"

#ifdef ZEND_WIN32
# include <windows.h>
#else
# include <time.h>
#endif

ZEND_API zend_class_entry *zend_ce_future;

static zend_object_handlers zend_future_handlers;

typedef enum {
	ZEND_FUTURE_PENDING,
	ZEND_FUTURE_FULFILLED,
	ZEND_FUTURE_REJECTED,
} zend_future_state;

typedef struct _zend_future {
	/* Fiber running the task, UNDEF once it finished or for timer futures. */
	zval fiber;
	/* Return value or exception once the future completed. */
	zval result;
	/* Task futures suspended in await() on this future. */
	HashTable *waiters;
	/* Deadline of Async\delay() futures. */
	zend_hrtime_t deadline;
	zend_future_state state;
	zend_object std;
} zend_future;

static zend_always_inline zend_future *zend_future_from_obj(zend_object *obj)
{
	return (zend_future *) ((char *) obj - offsetof(zend_future, std));
}

#define Z_FUTURE_P(zv) zend_future_from_obj(Z_OBJ_P(zv))

/* Scheduler state. */
ZEND_TLS HashTable *zend_async_tasks;  /* fiber handle => zend_future* (weak) */
ZEND_TLS HashTable *zend_async_ready;  /* list of futures whose task can resume */
ZEND_TLS HashTable *zend_async_timers; /* list of pending Async\delay() futures */

static HashTable *zend_async_new_list(void)
{
	HashTable *ht = zend_new_array(0);
	zend_hash_real_init_packed(ht);
	return ht;
}

void zend_async_init(void)
{
	zend_async_tasks = NULL;
	zend_async_ready = NULL;
	zend_async_timers = NULL;
}

void zend_async_shutdown(void)
{
	/* Releasing the queues destroys pending tasks: their fibers are
	 * destroyed the same way as any other suspended fiber. */
	if (zend_async_ready) {
		HashTable *ready = zend_async_ready;
		zend_async_ready = NULL;
		zend_array_destroy(ready);
	}
	if (zend_async_timers) {
		HashTable *timers = zend_async_timers;
		zend_async_timers = NULL;
		zend_array_destroy(timers);
	}
	if (zend_async_tasks) {
		HashTable *tasks = zend_async_tasks;
		zend_async_tasks = NULL;
		zend_hash_destroy(tasks);
		FREE_HASHTABLE(tasks);
	}
}

/* Returns the task future of the currently running fiber, if it is one. */
static zend_future *zend_async_current_task(void)
{
	if (!EG(active_fiber) || !zend_async_tasks) {
		return NULL;
	}
	return zend_hash_index_find_ptr(zend_async_tasks, EG(active_fiber)->std.handle);
}

static void zend_async_enqueue(zend_future *future)
{
	zval zv;

	if (!zend_async_ready) {
		zend_async_ready = zend_async_new_list();
	}
	ZVAL_OBJ_COPY(&zv, &future->std);
	zend_hash_next_index_insert_new(zend_async_ready, &zv);
}

/* Takes ownership of value. */
static void zend_future_settle(zend_future *future, zval *value, zend_future_state state)
{
	ZEND_ASSERT(future->state == ZEND_FUTURE_PENDING);

	future->state = state;
	ZVAL_COPY_VALUE(&future->result, value);

	if (future->waiters) {
		HashTable *waiters = future->waiters;
		zval *waiter;

		future->waiters = NULL;
		ZEND_HASH_FOREACH_VAL(waiters, waiter) {
			zend_async_enqueue(Z_FUTURE_P(waiter));
		} ZEND_HASH_FOREACH_END();
		zend_array_destroy(waiters);
	}
}

/* Must be called after control returns from a task's fiber. Completes the
 * future once the fiber is dead. */
static void zend_async_task_switched(zend_future *task)
{
	zend_fiber *fiber;
	zval value;

	if (Z_TYPE(task->fiber) != IS_OBJECT) {
		return;
	}

	fiber = (zend_fiber *) Z_OBJ(task->fiber);
	if (fiber->context.status != ZEND_FIBER_STATUS_DEAD) {
		return;
	}

	if (EG(exception)) {
		zend_object *exception = EG(exception);

		if (zend_is_unwind_exit(exception) || zend_is_graceful_exit(exception)) {
			/* exit() or fiber destruction: let it propagate. */
			return;
		}

		GC_ADDREF(exception);
		zend_clear_exception();
		ZVAL_OBJ(&value, exception);
		zend_future_settle(task, &value, ZEND_FUTURE_REJECTED);
	} else {
		ZVAL_COPY_DEREF(&value, &fiber->result);
		zend_future_settle(task, &value, ZEND_FUTURE_FULFILLED);
	}

	if (zend_async_tasks) {
		zend_hash_index_del(zend_async_tasks, fiber->std.handle);
	}
	zval_ptr_dtor(&task->fiber);
	ZVAL_UNDEF(&task->fiber);
}

static void zend_async_resume(zend_future *task)
{
	zend_fiber *fiber;
	zval retval;

	if (Z_TYPE(task->fiber) != IS_OBJECT) {
		return;
	}

	fiber = (zend_fiber *) Z_OBJ(task->fiber);
	if (fiber->context.status != ZEND_FIBER_STATUS_SUSPENDED || fiber->caller != NULL) {
		return;
	}

	ZVAL_UNDEF(&retval);
	zend_fiber_resume(fiber, NULL, &retval);
	zval_ptr_dtor(&retval);
	zend_async_task_switched(task);
}

static void zend_async_sleep(zend_hrtime_t ns)
{
#ifdef ZEND_WIN32
	Sleep((DWORD) (ns / 1000000));
#else
	struct timespec ts;
	ts.tv_sec = (time_t) (ns / 1000000000);
	ts.tv_nsec = (long) (ns % 1000000000);
	while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
#endif
}

/* Completes all due timers. If may_sleep is set and no timer is due yet,
 * sleeps until the earliest one is. */
static void zend_async_run_timers(bool may_sleep)
{
	zend_hrtime_t now, earliest = 0;
	HashTable *timers, *remaining;
	zval *zv;

	if (!zend_async_timers || zend_hash_num_elements(zend_async_timers) == 0) {
		return;
	}

	now = zend_hrtime();

	if (may_sleep) {
		ZEND_HASH_FOREACH_VAL(zend_async_timers, zv) {
			zend_hrtime_t deadline = Z_FUTURE_P(zv)->deadline;
			if (earliest == 0 || deadline < earliest) {
				earliest = deadline;
			}
		} ZEND_HASH_FOREACH_END();

		if (earliest > now) {
			zend_async_sleep(earliest - now);
			now = zend_hrtime();
		}
	}

	timers = zend_async_timers;
	remaining = zend_async_new_list();
	zend_async_timers = remaining;

	ZEND_HASH_FOREACH_VAL(timers, zv) {
		zend_future *timer = Z_FUTURE_P(zv);

		if (timer->deadline <= now) {
			zval value;
			ZVAL_NULL(&value);
			zend_future_settle(timer, &value, ZEND_FUTURE_FULFILLED);
		} else {
			Z_ADDREF_P(zv);
			zend_hash_next_index_insert_new(remaining, zv);
		}
	} ZEND_HASH_FOREACH_END();

	zend_array_destroy(timers);
}

/* Resumes every task that is ready right now. */
static void zend_async_run_once(void)
{
	HashTable *batch;
	zval *zv;

	zend_async_run_timers(false);

	if (!zend_async_ready || zend_hash_num_elements(zend_async_ready) == 0) {
		return;
	}

	batch = zend_async_ready;
	zend_async_ready = NULL;

	ZEND_HASH_FOREACH_VAL(batch, zv) {
		if (EG(exception)) {
			/* Keep the remaining tasks scheduled. */
			zend_async_enqueue(Z_FUTURE_P(zv));
			continue;
		}
		zend_async_resume(Z_FUTURE_P(zv));
	} ZEND_HASH_FOREACH_END();

	zend_array_destroy(batch);
}

/* Drives the scheduler until the given future is complete. */
static zend_result zend_async_run_until(zend_future *target)
{
	zend_result result = SUCCESS;

	GC_ADDREF(&target->std);

	while (target->state == ZEND_FUTURE_PENDING) {
		if (zend_async_ready && zend_hash_num_elements(zend_async_ready) > 0) {
			zend_async_run_once();
		} else if (zend_async_timers && zend_hash_num_elements(zend_async_timers) > 0) {
			zend_async_run_timers(true);
		} else {
			zend_throw_error(NULL, "Deadlock detected: the awaited Future can never complete");
			result = FAILURE;
			break;
		}

		if (EG(exception)) {
			result = FAILURE;
			break;
		}
	}

	OBJ_RELEASE(&target->std);
	return result;
}

static zend_object *zend_future_create_object(zend_class_entry *ce)
{
	zend_future *future = zend_object_alloc(sizeof(zend_future), ce);

	ZVAL_UNDEF(&future->fiber);
	ZVAL_UNDEF(&future->result);
	future->waiters = NULL;
	future->deadline = 0;
	future->state = ZEND_FUTURE_PENDING;

	zend_object_std_init(&future->std, ce);
	object_properties_init(&future->std, ce);

	return &future->std;
}

static void zend_future_free_obj(zend_object *object)
{
	zend_future *future = zend_future_from_obj(object);

	if (Z_TYPE(future->fiber) == IS_OBJECT) {
		if (zend_async_tasks) {
			zend_hash_index_del(zend_async_tasks, Z_OBJ(future->fiber)->handle);
		}
		zval_ptr_dtor(&future->fiber);
		ZVAL_UNDEF(&future->fiber);
	}

	zval_ptr_dtor(&future->result);

	if (future->waiters) {
		zend_array_destroy(future->waiters);
		future->waiters = NULL;
	}

	zend_object_std_dtor(&future->std);
}

static HashTable *zend_future_get_gc(zend_object *object, zval **table, int *num)
{
	zend_future *future = zend_future_from_obj(object);
	zend_get_gc_buffer *buf = zend_get_gc_buffer_create();

	zend_get_gc_buffer_add_zval(buf, &future->fiber);
	zend_get_gc_buffer_add_zval(buf, &future->result);

	if (future->waiters) {
		zval *zv;
		ZEND_HASH_FOREACH_VAL(future->waiters, zv) {
			zend_get_gc_buffer_add_zval(buf, zv);
		} ZEND_HASH_FOREACH_END();
	}

	zend_get_gc_buffer_use(buf, table, num);

	return NULL;
}

ZEND_METHOD(Async_Future, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(Async_Future, isDone)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(Z_FUTURE_P(ZEND_THIS)->state != ZEND_FUTURE_PENDING);
}

ZEND_FUNCTION(Async_async)
{
	zval *callable, *args = NULL;
	uint32_t argc = 0;
	HashTable *named_args = NULL;
	zend_future *task;
	zend_function *start;
	zval fiber, retval;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_ZVAL(callable)
		Z_PARAM_VARIADIC_WITH_NAMED(args, argc, named_args)
	ZEND_PARSE_PARAMETERS_END();

	/* Let Fiber::__construct() validate the callable. */
	object_init_ex(&fiber, zend_ce_fiber);
	zend_call_known_instance_method_with_1_params(zend_ce_fiber->constructor, Z_OBJ(fiber), NULL, callable);
	if (EG(exception)) {
		zval_ptr_dtor(&fiber);
		RETURN_THROWS();
	}

	object_init_ex(return_value, zend_ce_future);
	task = Z_FUTURE_P(return_value);
	ZVAL_COPY_VALUE(&task->fiber, &fiber);

	if (!zend_async_tasks) {
		ALLOC_HASHTABLE(zend_async_tasks);
		zend_hash_init(zend_async_tasks, 8, NULL, NULL, 0);
	}
	zend_hash_index_add_new_ptr(zend_async_tasks, Z_OBJ(fiber)->handle, task);

	/* Run the task eagerly until its first suspension point. */
	start = zend_hash_str_find_ptr(&zend_ce_fiber->function_table, ZEND_STRL("start"));
	ZEND_ASSERT(start != NULL);
	ZVAL_UNDEF(&retval);
	zend_call_known_function(start, Z_OBJ(fiber), zend_ce_fiber, &retval, argc, args, named_args);
	zval_ptr_dtor(&retval);

	if (EG(exception) && ((zend_fiber *) Z_OBJ(fiber))->context.status != ZEND_FIBER_STATUS_DEAD) {
		/* The fiber could not be started at all. */
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);
		RETURN_THROWS();
	}

	zend_async_task_switched(task);

	if (EG(exception)) {
		RETURN_THROWS();
	}
}

ZEND_FUNCTION(Async_wait)
{
	zend_object *obj;
	zend_future *target;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS(obj, zend_ce_future)
	ZEND_PARSE_PARAMETERS_END();

	target = zend_future_from_obj(obj);

	if (target->state == ZEND_FUTURE_PENDING) {
		zend_future *self = zend_async_current_task();

		if (self) {
			zval waiter, value;

			if (self == target) {
				zend_throw_error(NULL, "A task cannot await its own Future");
				RETURN_THROWS();
			}

			if (UNEXPECTED(zend_fiber_switch_blocked())) {
				zend_throw_error(NULL, "Cannot switch fibers in current execution context");
				RETURN_THROWS();
			}

			if (UNEXPECTED(EG(active_fiber)->flags & ZEND_FIBER_FLAG_DESTROYED)) {
				zend_throw_error(NULL, "Cannot await in a force-closed fiber");
				RETURN_THROWS();
			}

			if (!target->waiters) {
				target->waiters = zend_async_new_list();
			}
			ZVAL_OBJ_COPY(&waiter, &self->std);
			zend_hash_next_index_insert_new(target->waiters, &waiter);

			ZVAL_UNDEF(&value);
			zend_fiber_suspend(EG(active_fiber), NULL, &value);
			zval_ptr_dtor(&value);

			if (EG(exception)) {
				RETURN_THROWS();
			}
			ZEND_ASSERT(target->state != ZEND_FUTURE_PENDING);
		} else if (zend_async_run_until(target) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (target->state == ZEND_FUTURE_REJECTED) {
		zval exception;
		ZVAL_COPY(&exception, &target->result);
		zend_throw_exception_object(&exception);
		RETURN_THROWS();
	}

	RETURN_COPY(&target->result);
}

ZEND_FUNCTION(Async_delay)
{
	double seconds;
	zend_future *timer;
	zval zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(seconds)
	ZEND_PARSE_PARAMETERS_END();

	if (!(seconds >= 0.0) || !zend_finite(seconds)) {
		zend_argument_value_error(1, "must be a finite number greater than or equal to 0");
		RETURN_THROWS();
	}

	object_init_ex(return_value, zend_ce_future);
	timer = Z_FUTURE_P(return_value);
	timer->deadline = zend_hrtime() + (zend_hrtime_t) (seconds * 1e9);

	if (!zend_async_timers) {
		zend_async_timers = zend_async_new_list();
	}
	ZVAL_COPY(&zv, return_value);
	zend_hash_next_index_insert_new(zend_async_timers, &zv);
}

ZEND_FUNCTION(Async_suspend)
{
	zend_future *self;

	ZEND_PARSE_PARAMETERS_NONE();

	self = zend_async_current_task();

	if (!self) {
		/* Outside of a task: let every ready task make progress. */
		zend_async_run_once();
		return;
	}

	if (UNEXPECTED(zend_fiber_switch_blocked())) {
		zend_throw_error(NULL, "Cannot switch fibers in current execution context");
		RETURN_THROWS();
	}

	zval value;
	zend_async_enqueue(self);
	ZVAL_UNDEF(&value);
	zend_fiber_suspend(EG(active_fiber), NULL, &value);
	zval_ptr_dtor(&value);
}

void zend_register_async_ce(void)
{
	zend_ce_future = register_class_Async_Future();
	zend_ce_future->create_object = zend_future_create_object;
	zend_ce_future->default_object_handlers = &zend_future_handlers;

	memcpy(&zend_future_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_future_handlers.offset = offsetof(zend_future, std);
	zend_future_handlers.free_obj = zend_future_free_obj;
	zend_future_handlers.get_gc = zend_future_get_gc;
	zend_future_handlers.clone_obj = NULL;

	zend_register_functions(NULL, ext_functions, NULL, MODULE_PERSISTENT);
}

/* {{{ Parser helpers */

static zend_ast *zend_ast_create_fq_name(const char *name, size_t len)
{
	zval zv;
	ZVAL_STR(&zv, zend_string_init(name, len, 0));
	return zend_ast_create_zval_ex(&zv, ZEND_NAME_FQ);
}

static zend_ast *zend_ast_create_fq_call(const char *name, size_t len, zend_ast *arg)
{
	return zend_ast_create(ZEND_AST_CALL,
		zend_ast_create_fq_name(name, len),
		zend_ast_create_list(1, ZEND_AST_ARG_LIST, arg));
}

zend_ast *zend_ast_create_await(zend_ast *expr)
{
	return zend_ast_create_fq_call(ZEND_STRL("Async\\wait"), expr);
}

static void zend_async_add_use(zend_ast **uses, zend_string *name)
{
	zval zv;
	zend_ast *use;

	ZVAL_STR_COPY(&zv, name);
	use = zend_ast_create_zval_ex(&zv, ZEND_BIND_REF);

	if (*uses) {
		*uses = zend_ast_list_add(*uses, use);
	} else {
		*uses = zend_ast_create_list(1, ZEND_AST_CLOSURE_USES, use);
	}
}

/* Rewrites
 *     async function f($a, ...$b): T { body }
 * into
 *     function f($a, ...$b): \Async\Future {
 *         return \Async\async(function () use (&$a, &$b): T { body });
 *     }
 * and
 *     async fn($a): T => expr
 * into
 *     fn($a): \Async\Future => \Async\async(fn(): T => expr)
 */
zend_ast *zend_ast_create_async_function(zend_ast *ast)
{
	zend_ast_decl *decl = (zend_ast_decl *) ast;
	zend_ast *inner, *uses = NULL;
	uint32_t inner_flags = decl->flags & ZEND_ACC_STATIC;
	const char *what = decl->kind == ZEND_AST_METHOD ? "method" : "function";

	if (!decl->child[2]) {
		zend_error_noreturn(E_COMPILE_ERROR, "Async %s %s() must have a body",
			what, decl->name ? ZSTR_VAL(decl->name) : "{closure}");
	}
	if (decl->flags & ZEND_ACC_RETURN_REFERENCE) {
		zend_error_noreturn(E_COMPILE_ERROR, "Async functions cannot return by reference");
	}
	if (decl->flags & ZEND_ACC_GENERATOR) {
		zend_error_noreturn(E_COMPILE_ERROR, "Async functions cannot be generators");
	}
	if (decl->kind == ZEND_AST_METHOD
	 && (zend_string_equals_literal_ci(decl->name, "__construct")
	  || zend_string_equals_literal_ci(decl->name, "__destruct")
	  || zend_string_equals_literal_ci(decl->name, "__clone"))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Method %s() cannot be async", ZSTR_VAL(decl->name));
	}

	if (decl->kind == ZEND_AST_ARROW_FUNC) {
		/* Arrow functions capture the outer parameters automatically. */
		inner = zend_ast_create_decl(ZEND_AST_ARROW_FUNC, inner_flags, decl->start_lineno, NULL,
			NULL, zend_ast_create_list(0, ZEND_AST_PARAM_LIST), NULL, decl->child[2], decl->child[3], NULL);
	} else {
		zend_ast_list *params = zend_ast_get_list(decl->child[0]);
		for (uint32_t i = 0; i < params->children; i++) {
			zend_async_add_use(&uses, zend_ast_get_str(params->child[i]->child[1]));
		}
		if (decl->child[1]) {
			zend_ast_list *outer_uses = zend_ast_get_list(decl->child[1]);
			for (uint32_t i = 0; i < outer_uses->children; i++) {
				zend_async_add_use(&uses, zend_ast_get_str(outer_uses->child[i]));
			}
		}
		inner = zend_ast_create_decl(ZEND_AST_CLOSURE, inner_flags, decl->start_lineno, NULL,
			NULL, zend_ast_create_list(0, ZEND_AST_PARAM_LIST), uses, decl->child[2], decl->child[3], NULL);
	}

	zend_ast *call = zend_ast_create_fq_call(ZEND_STRL("Async\\async"), inner);

	if (decl->kind == ZEND_AST_ARROW_FUNC) {
		decl->child[2] = call;
	} else {
		decl->child[2] = zend_ast_create_list(1, ZEND_AST_STMT_LIST, zend_ast_create(ZEND_AST_RETURN, call));
	}
	decl->child[3] = zend_ast_create_fq_name(ZEND_STRL("Async\\Future"));

	return ast;
}

/* }}} */
