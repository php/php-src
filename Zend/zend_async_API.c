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
zend_async_globals_t zend_async_globals_api = {0};
#endif

#define ASYNC_THROW_ERROR(error) zend_throw_error(NULL, error);

static zend_coroutine_t * spawn(zend_async_scope_t *scope, zend_object *scope_provider)
{
	ASYNC_THROW_ERROR("Async API is not enabled");
	return NULL;
}

static void suspend(bool from_main) {}

static void enqueue_coroutine(zend_coroutine_t *coroutine)
{
	ASYNC_THROW_ERROR("Async API is not enabled");
}

static zend_async_context_t * new_context(void)
{
	ASYNC_THROW_ERROR("Context API is not enabled");
	return NULL;
}

/* Internal context stubs removed - using direct implementation */

static zend_class_entry * get_class_ce(zend_async_class type)
{
	if (type == ZEND_ASYNC_EXCEPTION_DEFAULT
		|| type == ZEND_ASYNC_EXCEPTION_DNS
		|| type == ZEND_ASYNC_EXCEPTION_POLL
		|| type == ZEND_ASYNC_EXCEPTION_TIMEOUT
		|| type == ZEND_ASYNC_EXCEPTION_INPUT_OUTPUT) {
		return zend_ce_exception;
	} else if (type == ZEND_ASYNC_EXCEPTION_CANCELLATION) {
		return zend_ce_cancellation_exception;
	}

	return NULL;
}

static zend_atomic_bool scheduler_lock = {0};
static char * scheduler_module_name = NULL;
zend_async_spawn_t zend_async_spawn_fn = spawn;
zend_async_new_coroutine_t zend_async_new_coroutine_fn = NULL;
zend_async_new_scope_t zend_async_new_scope_fn = NULL;
zend_async_suspend_t zend_async_suspend_fn = suspend;
zend_async_enqueue_coroutine_t zend_async_enqueue_coroutine_fn = enqueue_coroutine;
zend_async_resume_t zend_async_resume_fn = NULL;
zend_async_cancel_t zend_async_cancel_fn = NULL;
zend_async_shutdown_t zend_async_shutdown_fn = NULL;
zend_async_get_coroutines_t zend_async_get_coroutines_fn = NULL;
zend_async_add_microtask_t zend_async_add_microtask_fn = NULL;
zend_async_get_awaiting_info_t zend_async_get_awaiting_info_fn = NULL;
zend_async_get_class_ce_t zend_async_get_class_ce_fn = get_class_ce;

static zend_atomic_bool reactor_lock = {0};
static char * reactor_module_name = NULL;
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
zend_async_freeaddrinfo_t zend_async_freeaddrinfo_fn = NULL;

zend_async_new_exec_event_t zend_async_new_exec_event_fn = NULL;
zend_async_exec_t zend_async_exec_fn = NULL;

static zend_string * thread_pool_module_name = NULL;
zend_async_queue_task_t zend_async_queue_task_fn = NULL;

/* Context API */
zend_async_new_context_t zend_async_new_context_fn = new_context;

/* Internal Context API - now uses direct functions */

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
	globals->main_scope = NULL;
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
	ts_globals_ctor(&zend_async_globals_api);
#endif
}

void zend_async_init(void)
{
}

void zend_async_globals_dtor(void)
{
#ifdef ZTS
#else
	ts_globals_dtor(&zend_async_globals_api);
#endif
}

void zend_async_shutdown(void)
{
	zend_async_globals_dtor();
	zend_async_internal_context_api_shutdown();
}

ZEND_API int zend_async_get_api_version_number(void)
{
	return ZEND_ASYNC_API_VERSION_NUMBER;
}

ZEND_API bool zend_async_scheduler_register(
	char *module,
	bool allow_override,
	zend_async_new_coroutine_t new_coroutine_fn,
	zend_async_new_scope_t new_scope_fn,
	zend_async_new_context_t new_context_fn,
    zend_async_spawn_t spawn_fn,
    zend_async_suspend_t suspend_fn,
    zend_async_enqueue_coroutine_t enqueue_coroutine_fn,
    zend_async_resume_t resume_fn,
    zend_async_cancel_t cancel_fn,
    zend_async_shutdown_t shutdown_fn,
    zend_async_get_coroutines_t get_coroutines_fn,
    zend_async_add_microtask_t add_microtask_fn,
    zend_async_get_awaiting_info_t get_awaiting_info_fn,
    zend_async_get_class_ce_t get_class_ce_fn
)
{
	if (zend_atomic_bool_exchange(&scheduler_lock, 1)) {
		return false;
	}

	if (scheduler_module_name == module) {
		return true;
	}

	if (scheduler_module_name != NULL && false == allow_override) {
		zend_atomic_bool_store(&scheduler_lock, 0);
		zend_error(
			E_CORE_ERROR, "The module %s is trying to override Scheduler, which was registered by the module %s.",
			module, scheduler_module_name
		);
		return false;
	}

	scheduler_module_name = module;

	zend_async_new_coroutine_fn = new_coroutine_fn;
	zend_async_new_scope_fn = new_scope_fn;
	zend_async_new_context_fn = new_context_fn;
    zend_async_spawn_fn = spawn_fn;
    zend_async_suspend_fn = suspend_fn;
    zend_async_enqueue_coroutine_fn = enqueue_coroutine_fn;
    zend_async_resume_fn = resume_fn;
    zend_async_cancel_fn = cancel_fn;
    zend_async_shutdown_fn = shutdown_fn;
    zend_async_get_coroutines_fn = get_coroutines_fn;
    zend_async_add_microtask_fn = add_microtask_fn;
	zend_async_get_awaiting_info_fn = get_awaiting_info_fn;
	zend_async_get_class_ce_fn = get_class_ce_fn;

	zend_atomic_bool_store(&scheduler_lock, 0);

	return true;
}

ZEND_API bool zend_async_reactor_register(
	char *module,
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
    zend_async_freeaddrinfo_t freeaddrinfo_fn,
    zend_async_new_exec_event_t new_exec_event_fn,
    zend_async_exec_t exec_fn
)
{
	if (zend_atomic_bool_exchange(&reactor_lock, 1)) {
		return false;
	}

	if (reactor_module_name == module) {
		return true;
	}

	if (reactor_module_name != NULL && false == allow_override) {
		zend_error(
			E_CORE_ERROR, "The module %s is trying to override Reactor, which was registered by the module %s.",
			module, reactor_module_name
		);
		return false;
	}

	reactor_module_name = module;

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
	zend_async_freeaddrinfo_fn = freeaddrinfo_fn;

	zend_async_new_exec_event_fn = new_exec_event_fn;
	zend_async_exec_fn = exec_fn;

	zend_atomic_bool_store(&reactor_lock, 0);

	return true;
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

	if (ZEND_COROUTINE_SUSPENDED(coroutine)) {
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

static void event_callback_dispose(zend_async_event_callback_t *callback, zend_async_event_t * event)
{
	if (callback->ref_count > 1) {
		// If the callback is still referenced, we cannot dispose it yet
		callback->ref_count--;
		return;
	}

	callback->ref_count = 0;
	efree(callback);
}

ZEND_API zend_async_event_callback_t * zend_async_event_callback_new(zend_async_event_callback_fn callback, size_t size)
{
	zend_async_event_callback_t * event_callback = ecalloc(1, size == 0 ? size : sizeof(zend_async_event_callback_t));

	event_callback->ref_count = 1;
	event_callback->callback = callback;
	event_callback->dispose = event_callback_dispose;

	return event_callback;
}

static void coroutine_event_callback_dispose(zend_async_event_callback_t *callback, zend_async_event_t * event);

ZEND_API zend_coroutine_event_callback_t * zend_async_coroutine_event_new(
	zend_coroutine_t * coroutine, zend_async_event_callback_fn callback, size_t size
)
{
	zend_coroutine_event_callback_t * coroutine_callback = ecalloc(
		1, size != 0 ? size : sizeof(zend_coroutine_event_callback_t)
	);

	coroutine_callback->base.ref_count = 1;
	coroutine_callback->base.callback = callback;
	coroutine_callback->coroutine = coroutine;
	coroutine_callback->base.dispose = coroutine_event_callback_dispose;

	return coroutine_callback;
}

//////////////////////////////////////////////////////////////////////
/* Waker API */
//////////////////////////////////////////////////////////////////////

static void waker_events_dtor(zval *item)
{
	zend_async_waker_trigger_t * trigger = Z_PTR_P(item);
	zend_async_event_t *event = trigger->event;
	trigger->event = NULL;

	printf("waker_events_dtor event %p\n", trigger);

	if (event != NULL) {
		event->del_callback(event, trigger->callback);
		//
		// At this point, we explicitly stop the event because it is no longer being listened to by our handlers.
		// However, this does not mean the object is destroyed—it may remain in memory if something still holds a reference to it.
		//
		event->stop(event);
		ZEND_ASYNC_EVENT_RELEASE(event);
	}

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
		zend_async_waker_destroy(coroutine);
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
		zend_hash_destroy(waker->triggered_events);
		efree(waker->triggered_events);
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

static void coroutine_event_callback_dispose(zend_async_event_callback_t *callback, zend_async_event_t * event)
{
	if (callback->ref_count > 1) {
		// If the callback is still referenced, we cannot dispose it yet
		callback->ref_count--;
		return;
	}

	callback->ref_count = 0;

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
		coroutine->waker->triggered_events = (HashTable *) emalloc(sizeof(HashTable));
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
		event_callback->base.dispose = coroutine_event_callback_dispose;
	}

	// Set up the default dispose function if not set
	if (event_callback->base.dispose == NULL) {
		event_callback->base.dispose = coroutine_event_callback_dispose;
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
			coroutine->waker->triggered_events = (HashTable *) emalloc(sizeof(HashTable));
			zend_hash_init(coroutine->waker->triggered_events, 2, NULL, waker_triggered_events_dtor, 0);
		}

		if (EXPECTED(zend_hash_index_add_ptr(coroutine->waker->triggered_events, (zend_ulong)event, event) != NULL)) {
			ZEND_ASYNC_EVENT_ADD_REF(event);
		}

		// Copy the result to the waker if it is not NULL
		if (ZEND_ASYNC_EVENT_WILL_ZVAL_RESULT(event) && result != NULL) {
			ZVAL_COPY(&coroutine->waker->result, result);
		}
	}

	if (exception != NULL) {
		//
		// This handler always captures the exception as handled because it passes it to another coroutine.
		// As a result, the exception will not propagate further.
		//
		ZEND_ASYNC_EVENT_SET_EXCEPTION_HANDLED(event);
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
		ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, zend_async_new_exception(
			ZEND_ASYNC_EXCEPTION_CANCELLATION, "Operation has been cancelled"
		), true);
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

ZEND_API ZEND_COLD zend_object * zend_async_new_exception(zend_async_class type, const char *format, ...)
{
	if (type == ZEND_ASYNC_CLASS_NO) {
		type = ZEND_ASYNC_EXCEPTION_DEFAULT;
	}

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

ZEND_API ZEND_COLD zend_object * zend_async_throw(zend_async_class type, const char *format, ...)
{
	if (type == ZEND_ASYNC_CLASS_NO) {
		type = ZEND_ASYNC_EXCEPTION_DEFAULT;
	}

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

//////////////////////////////////////////////////////////////////////
/// ### Internal Coroutine Context
///
/// The internal coroutine context is intended for use in PHP-extensions.
/// The flow is as follows:
///
/// 1. The extension registers a unique numeric index by `zend_async_internal_context_key_alloc`,
/// described by a static C string.
/// 2. The **TrueAsync API** verifies the uniqueness of the mapping between the index and the C string address.
/// 3. The extension uses `zend_async_internal_context_*` functions to get, set, or unset keys.
/// 4. Keys are automatically destroyed when the coroutine completes.
//////////////////////////////////////////////////////////////////////

// Global variables for internal context key management
static HashTable *zend_async_context_key_names = NULL;

#ifdef ZTS
static MUTEX_T zend_async_context_mutex = NULL;
#endif

void zend_async_init_internal_context_api(void) 
{
#ifdef ZTS
    zend_async_context_mutex = tsrm_mutex_alloc();
#endif
    // Initialize key names table - stores string pointers directly
    zend_async_context_key_names = pemalloc(sizeof(HashTable), 1);
    zend_hash_init(zend_async_context_key_names, 8, NULL, NULL, 1);  // No destructor - we don't own the strings
}

uint32_t zend_async_internal_context_key_alloc(const char *key_name) 
{
#ifdef ZTS
    tsrm_mutex_lock(zend_async_context_mutex);
#endif
    
    // Check if this string address already has a key
    const char **existing_ptr;
    ZEND_HASH_FOREACH_NUM_KEY_PTR(zend_async_context_key_names, zend_ulong existing_key, existing_ptr) {
        if (*existing_ptr == key_name) {
            // Found existing key for this string address
#ifdef ZTS
            tsrm_mutex_unlock(zend_async_context_mutex);
#endif
            return (uint32_t)existing_key;
        }
    } ZEND_HASH_FOREACH_END();
    
    // Use next available index as key
    uint32_t key = zend_hash_num_elements(zend_async_context_key_names) + 1;
    
    // Store string pointer directly
    zend_hash_index_add_ptr(zend_async_context_key_names, key, (void*)key_name);
    
#ifdef ZTS
    tsrm_mutex_unlock(zend_async_context_mutex);
#endif
    
    return key;
}

const char* zend_async_internal_context_key_name(uint32_t key) 
{
    if (zend_async_context_key_names == NULL) {
        return NULL;
    }
    
#ifdef ZTS
    tsrm_mutex_lock(zend_async_context_mutex);
#endif
    
    const char *name = zend_hash_index_find_ptr(zend_async_context_key_names, key);
    
#ifdef ZTS
    tsrm_mutex_unlock(zend_async_context_mutex);
#endif
    
    return name;
}

zval* zend_async_internal_context_find(zend_coroutine_t *coroutine, uint32_t key)
{
    if (coroutine == NULL || coroutine->internal_context == NULL) {
        return NULL;
    }
    
    return zend_hash_index_find(coroutine->internal_context, key);
}

void zend_async_internal_context_set(zend_coroutine_t *coroutine, uint32_t key, zval *value) 
{
    if (coroutine == NULL) {
        return;
    }
    
    // Initialize internal_context if needed
    if (coroutine->internal_context == NULL) {
        coroutine->internal_context = zend_new_array(0);
    }
    
    // Set the value
    zval copy;
    ZVAL_COPY(&copy, value);
    zend_hash_index_update(coroutine->internal_context, key, &copy);
}

bool zend_async_internal_context_unset(zend_coroutine_t *coroutine, uint32_t key) 
{
    if (coroutine == NULL || coroutine->internal_context == NULL) {
        return false;
    }
    
    return zend_hash_index_del(coroutine->internal_context, key) == SUCCESS;
}

void zend_async_coroutine_internal_context_dispose(zend_coroutine_t *coroutine)
{
    if (coroutine->internal_context != NULL) {
    	zend_array_release(coroutine->internal_context);
        coroutine->internal_context = NULL;
    }
}

void zend_async_internal_context_api_shutdown(void)
{
#ifdef ZTS
    if (zend_async_context_mutex != NULL) {
        tsrm_mutex_lock(zend_async_context_mutex);
    }
#endif
    
    if (zend_async_context_key_names != NULL) {
        zend_hash_destroy(zend_async_context_key_names);
        pefree(zend_async_context_key_names, 1);
        zend_async_context_key_names = NULL;
    }
    
#ifdef ZTS
    if (zend_async_context_mutex != NULL) {
        tsrm_mutex_unlock(zend_async_context_mutex);
        tsrm_mutex_free(zend_async_context_mutex);
        zend_async_context_mutex = NULL;
    }
#endif
}

void zend_async_coroutine_internal_context_init(zend_coroutine_t *coroutine)
{
    coroutine->internal_context = NULL;
}
//////////////////////////////////////////////////////////////////////
/* Internal Context API end */
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/* Callback Vector Implementation */
//////////////////////////////////////////////////////////////////////

/* Private helper functions for iterator management - static inline for performance */

/* Register the single iterator - prevents concurrent iterations */
static zend_always_inline bool
zend_async_callbacks_register_iterator(zend_async_callbacks_vector_t *vector, uint32_t *iterator_index)
{
	if (vector->current_iterator != NULL) {
		// Concurrent iteration detected - this is not allowed
		return false;
	}
	
	vector->current_iterator = iterator_index;
	return true;
}

/* Unregister the iterator after iteration completes */
static zend_always_inline void
zend_async_callbacks_unregister_iterator(zend_async_callbacks_vector_t *vector)
{
	vector->current_iterator = NULL;
}

/* Adjust the active iterator when an element is removed */
static zend_always_inline void
zend_async_callbacks_adjust_iterator(zend_async_callbacks_vector_t *vector, uint32_t removed_index)
{
	if (vector->current_iterator != NULL && *vector->current_iterator > removed_index) {
		(*vector->current_iterator)--;
	}
}

/* Public API implementations */

/* Remove a specific callback; order is NOT preserved, but iterator is safely adjusted */
ZEND_API void
zend_async_callbacks_remove(zend_async_event_t *event, zend_async_event_callback_t *callback)
{
	zend_async_callbacks_vector_t *vector = &event->callbacks;

	for (uint32_t i = 0; i < vector->length; ++i) {
		if (vector->data[i] == callback) {
			// Adjust the active iterator before performing the removal
			zend_async_callbacks_adjust_iterator(vector, i);
			
			// O(1) removal: move last element to current position
			vector->data[i] = vector->length > 0 ? vector->data[--vector->length] : NULL;
			callback->dispose(callback, event);
			return;
		}
	}
}

/* Call all callbacks with safe iterator tracking to handle concurrent modifications */
ZEND_API void
zend_async_callbacks_notify(zend_async_event_t *event, void *result, zend_object *exception)
{
	// If pre-notify returns false, we stop notifying callbacks
	if (event->before_notify != NULL) {
		if (false == event->before_notify(event, &result, &exception)) {
			return;
		}
	}

	if (event->callbacks.data == NULL || event->callbacks.length == 0) {
		return;
	}

	zend_async_callbacks_vector_t *vector = &event->callbacks;
	uint32_t current_index = 0;
	
	// Register iterator - prevents concurrent iterations
	if (!zend_async_callbacks_register_iterator(vector, &current_index)) {
		zend_error(E_CORE_WARNING,
			"Concurrent callback iteration detected - nested notify() calls are not allowed"
		);
		return;
	}

	// Iterate through callbacks with safe index tracking
	while (current_index < vector->length) {
		// Store current callback (it might be moved during execution)
		zend_async_event_callback_t *callback = vector->data[current_index];

		// Move to next callback
		// Note: current_index may have been adjusted by zend_async_callbacks_adjust_iterator
		// if a callback was removed during this iteration
		current_index++;

		// Execute callback
		callback->callback(event, callback, result, exception);
		
		if (UNEXPECTED(EG(exception) != NULL)) {
			break;
		}
	}

	// Unregister iterator
	zend_async_callbacks_unregister_iterator(vector);
}

/* Call all callbacks and close the event (Like future) */
ZEND_API void
zend_async_callbacks_notify_and_close(zend_async_event_t *event, void *result, zend_object *exception)
{
	ZEND_ASYNC_EVENT_SET_CLOSED(event);
	zend_async_callbacks_notify(event, result, exception);
}

/* Free the vector's memory including iterator tracking */
ZEND_API void
zend_async_callbacks_free(zend_async_event_t *event)
{
	if (event->callbacks.data == NULL) {
		return;
	}

	zend_async_callbacks_vector_t *vector = &event->callbacks;
	uint32_t current_index = 0;
	
	// Register iterator - prevents concurrent iterations
	if (!zend_async_callbacks_register_iterator(vector, &current_index)) {
		zend_error(E_CORE_WARNING,
			"Concurrent callback iteration detected - nested free() calls are not allowed"
		);
		return;
	}

	// Dispose all callbacks
	while (current_index < vector->length) {
		zend_async_event_callback_t *callback = vector->data[current_index];
		current_index++;
		callback->dispose(callback, event);
	}

	// Free memory
	efree(vector->data);

	// Unregister iterator
	zend_async_callbacks_unregister_iterator(vector);

	// Reset all fields
	vector->data            = NULL;
	vector->length          = 0;
	vector->capacity        = 0;
	vector->current_iterator = NULL;
}
