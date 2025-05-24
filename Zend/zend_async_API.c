/*
+----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Edmond                                                       |
  +----------------------------------------------------------------------+
*/
#include "zend_async_API.h"
#include "zend_exceptions.h"

#ifdef ZTS
int zend_async_globals_id = 0;
size_t zend_async_globals_offset = 0;
#else
// The default data structure that is used when no other extensions are present.
zend_async_globals_t zend_async_globals = {0};
#endif

#define ASYNC_THROW_ERROR(error) zend_throw_error(NULL, error);

static zend_coroutine_t * spawn(zend_async_scope_t *scope, zend_object *scope_provider)
{
	ASYNC_THROW_ERROR("Async API is not enabled");
	return NULL;
}

static void suspend(bool from_main) {}

static zend_class_entry * get_exception_ce(zend_async_exception_type type)
{
	return zend_ce_exception;
}

static zend_string * scheduler_module_name = NULL;
zend_async_spawn_t zend_async_spawn_fn = spawn;
zend_async_new_coroutine_t zend_async_new_coroutine_fn = NULL;
zend_async_new_scope_t zend_async_new_scope_fn = NULL;
zend_async_suspend_t zend_async_suspend_fn = suspend;
zend_async_resume_t zend_async_resume_fn = NULL;
zend_async_cancel_t zend_async_cancel_fn = NULL;
zend_async_shutdown_t zend_async_shutdown_fn = NULL;
zend_async_get_coroutines_t zend_async_get_coroutines_fn = NULL;
zend_async_add_microtask_t zend_async_add_microtask_fn = NULL;
zend_async_get_awaiting_info_t zend_async_get_awaiting_info_fn = NULL;
zend_async_get_exception_ce_t zend_async_get_exception_ce_fn = get_exception_ce;

static zend_string * reactor_module_name = NULL;
zend_async_reactor_startup_t zend_async_reactor_startup_fn = NULL;
zend_async_reactor_shutdown_t zend_async_reactor_shutdown_fn = NULL;
zend_async_reactor_execute_t zend_async_reactor_execute_fn = NULL;
zend_async_reactor_loop_alive_t zend_async_reactor_loop_alive_fn = NULL;
zend_async_new_socket_event_t zend_async_new_socket_event_fn = NULL;
zend_async_new_poll_event_t zend_async_new_poll_event_fn = NULL;
zend_async_new_timer_event_t zend_async_new_timer_event_fn = NULL;
zend_async_new_signal_event_t zend_async_new_signal_event_fn = NULL;
zend_async_new_process_event_t zend_async_new_process_event_fn = NULL;
zend_async_new_thread_event_t zend_async_new_thread_event_fn = NULL;
zend_async_new_filesystem_event_t zend_async_new_filesystem_event_fn = NULL;

zend_async_getnameinfo_t zend_async_getnameinfo_fn = NULL;
zend_async_getaddrinfo_t zend_async_getaddrinfo_fn = NULL;

zend_async_new_exec_event_t zend_async_new_exec_event_fn = NULL;
zend_async_exec_t zend_async_exec_fn = NULL;

static zend_string * thread_pool_module_name = NULL;
zend_async_queue_task_t zend_async_queue_task_fn = NULL;

ZEND_API bool zend_async_is_enabled(void)
{
	return scheduler_module_name != NULL && reactor_module_name != NULL;
}

ZEND_API bool zend_scheduler_is_enabled(void)
{
	return scheduler_module_name != NULL;
}

ZEND_API bool zend_async_reactor_is_enabled(void)
{
	return reactor_module_name != NULL;
}

static void ts_globals_ctor(zend_async_globals_t * globals)
{
	globals->state = ZEND_ASYNC_OFF;
	zend_atomic_bool_store(&globals->heartbeat, false);
	globals->in_scheduler_context = false;
	globals->graceful_shutdown = false;
	globals->active_coroutine_count = 0;
	globals->active_event_count = 0;
	globals->coroutine = NULL;
	globals->scope = NULL;
	globals->scheduler = NULL;
	globals->exit_exception = NULL;
}

static void ts_globals_dtor(zend_async_globals_t * globals)
{

}

void zend_async_globals_ctor(void)
{
#ifdef ZTS
	ts_allocate_fast_id(
		&zend_async_globals_id,
		&zend_async_globals_offset,
		sizeof(zend_async_globals_t),
		(ts_allocate_ctor) ts_globals_ctor,
		(ts_allocate_dtor) ts_globals_dtor
	);

	ZEND_ASSERT(zend_async_globals_id != 0 && "zend_async_globals allocation failed");

#else
	ts_globals_ctor(zend_async_globals);
#endif
}

void zend_async_init(void)
{
}

void zend_async_globals_dtor(void)
{
#ifdef ZTS
#else
	ts_globals_dtor(zend_async_globals);
#endif
}

void zend_async_shutdown(void)
{
	zend_async_globals_dtor();
}

ZEND_API int zend_async_get_api_version_number(void)
{
	return ZEND_ASYNC_API_VERSION_NUMBER;
}

ZEND_API void zend_async_scheduler_register(
	zend_string *module,
	bool allow_override,
	zend_async_new_coroutine_t new_coroutine_fn,
	zend_async_new_scope_t new_scope_fn,
    zend_async_spawn_t spawn_fn,
    zend_async_suspend_t suspend_fn,
    zend_async_resume_t resume_fn,
    zend_async_cancel_t cancel_fn,
    zend_async_shutdown_t shutdown_fn,
    zend_async_get_coroutines_t get_coroutines_fn,
    zend_async_add_microtask_t add_microtask_fn,
    zend_async_get_awaiting_info_t get_awaiting_info_fn,
    zend_async_get_exception_ce_t get_exception_ce_fn
)
{
	if (scheduler_module_name != NULL && false == allow_override) {
		zend_error(
			E_CORE_ERROR, "The module %s is trying to override Scheduler, which was registered by the module %s.",
			ZSTR_VAL(module), ZSTR_VAL(scheduler_module_name)
		);
		return;
	}

	if (scheduler_module_name != NULL) {
		zend_string_release(scheduler_module_name);
	}

	scheduler_module_name = zend_string_copy(module);

	zend_async_new_coroutine_fn = new_coroutine_fn;
	zend_async_new_scope_fn = new_scope_fn;
    zend_async_spawn_fn = spawn_fn;
    zend_async_suspend_fn = suspend_fn;
    zend_async_resume_fn = resume_fn;
    zend_async_cancel_fn = cancel_fn;
    zend_async_shutdown_fn = shutdown_fn;
    zend_async_get_coroutines_fn = get_coroutines_fn;
    zend_async_add_microtask_fn = add_microtask_fn;
	zend_async_get_awaiting_info_fn = get_awaiting_info_fn;
	zend_async_get_exception_ce_fn = get_exception_ce_fn;
}

ZEND_API void zend_async_reactor_register(
	zend_string *module,
	bool allow_override,
	zend_async_reactor_startup_t reactor_startup_fn,
	zend_async_reactor_shutdown_t reactor_shutdown_fn,
	zend_async_reactor_execute_t reactor_execute_fn,
	zend_async_reactor_loop_alive_t reactor_loop_alive_fn,
    zend_async_new_socket_event_t new_socket_event_fn,
    zend_async_new_poll_event_t new_poll_event_fn,
    zend_async_new_timer_event_t new_timer_event_fn,
    zend_async_new_signal_event_t new_signal_event_fn,
    zend_async_new_process_event_t new_process_event_fn,
    zend_async_new_thread_event_t new_thread_event_fn,
    zend_async_new_filesystem_event_t new_filesystem_event_fn,
    zend_async_getnameinfo_t getnameinfo_fn,
    zend_async_getaddrinfo_t getaddrinfo_fn,
    zend_async_new_exec_event_t new_exec_event_fn,
    zend_async_exec_t exec_fn
)
{
	if (reactor_module_name != NULL && false == allow_override) {
		zend_error(
			E_CORE_ERROR, "The module %s is trying to override Reactor, which was registered by the module %s.",
			ZSTR_VAL(module), ZSTR_VAL(reactor_module_name)
		);
		return;
	}

	if (reactor_module_name != NULL) {
		zend_string_release(reactor_module_name);
	}

	reactor_module_name = zend_string_copy(module);

	zend_async_reactor_startup_fn = reactor_startup_fn;
	zend_async_reactor_shutdown_fn = reactor_shutdown_fn;
	zend_async_reactor_execute_fn = reactor_execute_fn;
	zend_async_reactor_loop_alive_fn = reactor_loop_alive_fn;

    zend_async_new_socket_event_fn = new_socket_event_fn;
    zend_async_new_poll_event_fn = new_poll_event_fn;
    zend_async_new_timer_event_fn = new_timer_event_fn;
    zend_async_new_signal_event_fn = new_signal_event_fn;

	zend_async_new_process_event_fn = new_process_event_fn;
    zend_async_new_thread_event_fn = new_thread_event_fn;
    zend_async_new_filesystem_event_fn = new_filesystem_event_fn;

	zend_async_getnameinfo_fn = getnameinfo_fn;
	zend_async_getaddrinfo_fn = getaddrinfo_fn;

	zend_async_new_exec_event_fn = new_exec_event_fn;
	zend_async_exec_fn = exec_fn;
}

ZEND_API void zend_async_thread_pool_register(zend_string *module, bool allow_override, zend_async_queue_task_t queue_task_fn)
{
	if (thread_pool_module_name != NULL && false == allow_override) {
		zend_error(
			E_CORE_ERROR, "The module %s is trying to override Thread Pool, which was registered by the module %s.",
			ZSTR_VAL(module), ZSTR_VAL(thread_pool_module_name)
		);
	}

	if (thread_pool_module_name != NULL) {
		zend_string_release(thread_pool_module_name);
	}

	thread_pool_module_name = zend_string_copy(module);
    zend_async_queue_task_fn = queue_task_fn;
}

ZEND_API zend_string* zend_coroutine_gen_info(zend_coroutine_t *coroutine, char *zend_coroutine_name)
{
	if (zend_coroutine_name == NULL) {
		zend_coroutine_name = "";
	}

	if (coroutine->waker != NULL) {
		return zend_strpprintf(0,
			"Coroutine spawned at %s:%d, suspended at %s:%d (%s)",
			coroutine->filename ? ZSTR_VAL(coroutine->filename) : "",
			coroutine->lineno,
			coroutine->waker->filename ? ZSTR_VAL(coroutine->waker->filename) : "",
			coroutine->waker->lineno,
			zend_coroutine_name
		);
	} else {
		return zend_strpprintf(0,
			"Coroutine spawned at %s:%d (%s)",
			coroutine->filename ? ZSTR_VAL(coroutine->filename) : "",
			coroutine->lineno,
			zend_coroutine_name
		);
	}
}

//////////////////////////////////////////////////////////////////////
/* Waker API */
//////////////////////////////////////////////////////////////////////

static void waker_events_dtor(zval *item)
{
	zend_async_waker_trigger_t * trigger = Z_PTR_P(item);

	trigger->event->del_callback(trigger->event, trigger->callback);

	ZEND_ASYNC_EVENT_RELEASE(trigger->event);

	efree(trigger);
}

static void waker_triggered_events_dtor(zval *item)
{
	zend_async_event_t * event = Z_PTR_P(item);

	ZEND_ASYNC_EVENT_RELEASE(event);
}

ZEND_API zend_async_waker_t *zend_async_waker_define(zend_coroutine_t *coroutine)
{
	if (coroutine == NULL) {
		coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
	}

	if (UNEXPECTED(coroutine == NULL)) {
		zend_error(E_CORE_ERROR, "Cannot create waker for a coroutine that is not running");
		return NULL;
	}

	if (UNEXPECTED(coroutine->waker != NULL)) {
		return coroutine->waker;
	}

	return zend_async_waker_new(coroutine);
}

ZEND_API zend_async_waker_t *zend_async_waker_new(zend_coroutine_t *coroutine)
{
	if (coroutine == NULL) {
		coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
	}

	if (UNEXPECTED(coroutine == NULL)) {
		zend_error(E_CORE_ERROR, "Cannot create waker for a coroutine that is not running");
		return NULL;
	}

	if (UNEXPECTED(coroutine->waker != NULL)) {
		coroutine->waker->dtor(coroutine);
		coroutine->waker = NULL;
	}

	zend_async_waker_t *waker = pecalloc(1, sizeof(zend_async_waker_t), 0);

	waker->triggered_events = NULL;
	waker->error = NULL;
	waker->dtor = NULL;
	ZVAL_UNDEF(&waker->result);

	zend_hash_init(&waker->events, 2, NULL, waker_events_dtor, 0);

	coroutine->waker = waker;

	return waker;
}

ZEND_API void zend_async_waker_destroy(zend_coroutine_t *coroutine)
{
	if (UNEXPECTED(coroutine->waker == NULL)) {
		return;
	}

	if (coroutine->waker->dtor != NULL) {
		coroutine->waker->dtor(coroutine);
	}

	zend_async_waker_t * waker = coroutine->waker;
	coroutine->waker = NULL;

	// default dtor
	if (waker->error != NULL) {
		zend_object_release(waker->error);
		waker->error = NULL;
	}

	if (waker->triggered_events != NULL) {
		zend_array_release(waker->triggered_events);
		waker->triggered_events = NULL;
	}

	if (waker->filename != NULL) {
		zend_string_release(waker->filename);
		waker->filename = NULL;
		waker->lineno = 0;
	}

	zval_ptr_dtor(&waker->result);
	zend_hash_destroy(&waker->events);
	efree(waker);
}

static void event_callback_dispose(zend_async_event_callback_t *callback, zend_async_event_t * event)
{
	if (callback->ref_count != 0) {
		return;
	}

	zend_async_waker_t * waker = ((zend_coroutine_event_callback_t *) callback)->coroutine->waker;

	if (event != NULL && waker != NULL) {
		// remove the event from the waker
		zend_hash_index_del(&waker->events, (zend_ulong)event);

		if (waker->triggered_events != NULL) {
			zend_hash_index_del(waker->triggered_events, (zend_ulong)event);
		}
	}

	efree(callback);
}

ZEND_API void zend_async_waker_add_triggered_event(zend_coroutine_t *coroutine, zend_async_event_t *event)
{
	if (UNEXPECTED(coroutine->waker == NULL)) {
		return;
	}

	if (coroutine->waker->triggered_events == NULL) {
		coroutine->waker->triggered_events = (HashTable *) malloc(sizeof(HashTable));
		zend_hash_init(coroutine->waker->triggered_events, 2, NULL, waker_triggered_events_dtor, 0);
	}

	if (EXPECTED(zend_hash_index_add_ptr(coroutine->waker->triggered_events, (zend_ulong)event, event) != NULL)) {
		ZEND_ASYNC_EVENT_ADD_REF(event);
	}
}

ZEND_API void zend_async_resume_when(
		zend_coroutine_t			*coroutine,
		zend_async_event_t			*event,
		const bool					trans_event,
		zend_async_event_callback_fn callback,
		zend_coroutine_event_callback_t *event_callback
	)
{
	if (UNEXPECTED(ZEND_ASYNC_EVENT_IS_CLOSED(event))) {
		zend_throw_error(NULL, "The event cannot be used after it has been terminated");
		return;
	}

	if (UNEXPECTED(callback == NULL && event_callback == NULL)) {
		zend_error(E_WARNING, "Callback cannot be NULL");

		if (trans_event) {
			event->dispose(event);
		}

		return;
	}

	if (event_callback == NULL) {
		event_callback = emalloc(sizeof(zend_coroutine_event_callback_t));
		event_callback->base.ref_count = 1;
		event_callback->base.callback = callback;
		event_callback->base.dispose = event_callback_dispose;
	}

	// Set up the default dispose function if not set
	if (event_callback->base.dispose == NULL) {
		event_callback->base.dispose = event_callback_dispose;
	}

	event_callback->coroutine = coroutine;
	event->add_callback(event, &event_callback->base);

	if (UNEXPECTED(EG(exception) != NULL)) {
		if (trans_event) {
			event->dispose(event);
		}

		return;
	}

	if (EXPECTED(coroutine->waker != NULL)) {
		zend_async_waker_trigger_t *trigger = emalloc(sizeof(zend_async_waker_trigger_t));
		trigger->event = event;
		trigger->callback = &event_callback->base;

		if (UNEXPECTED(zend_hash_index_add_ptr(&coroutine->waker->events, (zend_ulong)event, trigger) == NULL)) {
			zend_throw_error(NULL, "Failed to add event to the waker");
			return;
		}
	}

	if (false == trans_event) {
		ZEND_ASYNC_EVENT_ADD_REF(event);
	}
}

ZEND_API void zend_async_waker_callback_resolve(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
)
{
	zend_coroutine_t * coroutine = ((zend_coroutine_event_callback_t *) callback)->coroutine;

	if (exception == NULL && coroutine->waker != NULL) {

		if (coroutine->waker->triggered_events == NULL) {
			coroutine->waker->triggered_events = (HashTable *) malloc(sizeof(HashTable));
			zend_hash_init(coroutine->waker->triggered_events, 2, NULL, waker_triggered_events_dtor, 0);
		}

		if (EXPECTED(zend_hash_index_add_ptr(coroutine->waker->triggered_events, (zend_ulong)event, event) != NULL)) {
			ZEND_ASYNC_EVENT_ADD_REF(event);
		}

		// Copy the result to the waker if it is not NULL
		if (ZEND_ASYNC_EVENT_WILL_RESULT_USED(event) && result != NULL) {
			ZVAL_COPY(&coroutine->waker->result, result);
		}
	}

	ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, exception, false);
}

ZEND_API void zend_async_waker_callback_cancel(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
)
{
	zend_coroutine_t * coroutine = ((zend_coroutine_event_callback_t *) callback)->coroutine;

	if (UNEXPECTED(exception != NULL)) {
		ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, exception, false);
	} else {

		if (result == NULL) {
			exception = zend_async_new_exception(
				ZEND_ASYNC_EXCEPTION_CANCELLATION, "Operation has been cancelled"
			);
		} else {
			exception = result;
		}

		ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, exception, false);
	}
}

ZEND_API void zend_async_waker_callback_timeout(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
)
{
	if (UNEXPECTED(exception != NULL)) {
		ZEND_ASYNC_RESUME_WITH_ERROR(((zend_coroutine_event_callback_t *) callback)->coroutine, exception, false);
	} else {
		exception = zend_async_new_exception(
			ZEND_ASYNC_EXCEPTION_TIMEOUT, "Operation has been cancelled by timeout"
		);

		ZEND_ASYNC_RESUME_WITH_ERROR(((zend_coroutine_event_callback_t *) callback)->coroutine, exception, true);
	}
}

ZEND_API zend_async_waker_t * zend_async_waker_new_with_timeout(
	zend_coroutine_t * coroutine, const zend_ulong timeout, zend_async_event_t *cancellation
)
{
	if (coroutine == NULL) {
		coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
	}

	if (UNEXPECTED(coroutine == NULL)) {
		zend_error(E_CORE_ERROR, "Cannot create waker for a coroutine that is not running");
		return NULL;
	}

	zend_async_waker_t *waker = zend_async_waker_new(coroutine);

	if (timeout > 0) {
		zend_async_resume_when(
			coroutine,
			&ZEND_ASYNC_NEW_TIMER_EVENT(timeout, false)->base,
			true,
			zend_async_waker_callback_resolve,
			NULL
		);
	}

	if (cancellation != NULL) {
		zend_async_resume_when(coroutine, cancellation, false, zend_async_waker_callback_cancel, NULL);
	}

	return waker;
}

//////////////////////////////////////////////////////////////////////
/* Waker API end */
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/* Exception API */
//////////////////////////////////////////////////////////////////////

ZEND_API ZEND_COLD zend_object * zend_async_new_exception(zend_async_exception_type type, const char *format, ...)
{
	zend_class_entry *exception_ce = ZEND_ASYNC_GET_EXCEPTION_CE(type);
	zval exception, message_val;

	ZEND_ASSERT(instanceof_function(exception_ce, zend_ce_throwable)
		&& "Exceptions must implement Throwable");

	object_init_ex(&exception, exception_ce);

	va_list args;
	va_start(args, format);
	zend_string *message = zend_vstrpprintf(0, format, args);
	va_end(args);

	if (message) {
		ZVAL_STR(&message_val, message);
		zend_update_property_ex(exception_ce, Z_OBJ(exception), ZSTR_KNOWN(ZEND_STR_MESSAGE), &message_val);
	}

	zend_string_release(message);

	return Z_OBJ(exception);
}

ZEND_API ZEND_COLD zend_object * zend_async_throw(zend_async_exception_type type, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	zend_string *message = zend_vstrpprintf(0, format, args);
	va_end(args);

	zend_object *obj = zend_throw_exception(ZEND_ASYNC_GET_EXCEPTION_CE(type), ZSTR_VAL(message), 0);
	zend_string_release(message);
	return obj;
}

ZEND_API ZEND_COLD zend_object * zend_async_throw_cancellation(const char *format, ...)
{
	const zend_object *previous = EG(exception);

	if (format == NULL
		&& previous != NULL
		&& instanceof_function(previous->ce, ZEND_ASYNC_GET_EXCEPTION_CE(ZEND_ASYNC_EXCEPTION_TIMEOUT))) {
			format = "The operation was canceled by timeout";
		} else {
			format = format ? format : "The operation was canceled";
		}

	va_list args;
	va_start(args, format);

	zend_object *obj = zend_throw_exception_ex(
		ZEND_ASYNC_GET_EXCEPTION_CE(ZEND_ASYNC_EXCEPTION_CANCELLATION), 0, format, args
	);

	va_end(args);
	return obj;
}

ZEND_API ZEND_COLD zend_object * zend_async_throw_timeout(const char *format, const zend_long timeout)
{
	format = format ? format : "A timeout of %u microseconds occurred";

	return zend_throw_exception_ex(ZEND_ASYNC_GET_EXCEPTION_CE(ZEND_ASYNC_EXCEPTION_TIMEOUT), 0, format, timeout);
}

//////////////////////////////////////////////////////////////////////
/* Exception API end */
//////////////////////////////////////////////////////////////////////
