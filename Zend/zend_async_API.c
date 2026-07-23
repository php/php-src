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
#include "zend_async_API.h"
#include "zend_exceptions.h"

///////////////////////////////////////////////////////////////////
/// Globals
///////////////////////////////////////////////////////////////////

#ifdef ZTS
ZEND_API int zend_async_globals_id = 0;
ZEND_API size_t zend_async_globals_offset = 0;
#else
ZEND_API zend_async_globals_t zend_async_globals_api = { 0 };
#endif

static void internal_globals_ctor(zend_async_globals_t *globals)
{
	memset(globals, 0, sizeof(zend_async_globals_t));
}

static void internal_globals_dtor(zend_async_globals_t *globals)
{
	(void) globals;
}

#ifdef ZTS
static MUTEX_T scheduler_mutex = NULL;
#endif

void zend_async_globals_ctor(void)
{
#ifdef ZTS
	scheduler_mutex = tsrm_mutex_alloc();

	ts_allocate_fast_id(&zend_async_globals_id, &zend_async_globals_offset,
			sizeof(zend_async_globals_t), (ts_allocate_ctor) internal_globals_ctor,
			(ts_allocate_dtor) internal_globals_dtor);

	ZEND_ASSERT(zend_async_globals_id != 0 && "zend_async_globals allocation failed");
#else
	internal_globals_ctor(&zend_async_globals_api);
#endif
}

void zend_async_globals_dtor(void)
{
#ifndef ZTS
	internal_globals_dtor(&zend_async_globals_api);
#endif
}

///////////////////////////////////////////////////////////////////
/// Internal context
///////////////////////////////////////////////////////////////////

/*
 * The key registry maps a static C-string name to a process-unique numeric
 * key. Keys are allocated once per process (typically at MINIT), hence the
 * persistent memory and, under ZTS, the mutex.
 */
static HashTable *internal_context_key_names = NULL;
static uint32_t internal_context_next_key = 1;
#ifdef ZTS
static MUTEX_T internal_context_mutex = NULL;
#endif

ZEND_API uint32_t zend_async_internal_context_key_alloc(const char *key_name)
{
#ifdef ZTS
	if (internal_context_mutex == NULL) {
		internal_context_mutex = tsrm_mutex_alloc();
	}
	tsrm_mutex_lock(internal_context_mutex);
#endif

	if (internal_context_key_names == NULL) {
		internal_context_key_names = pemalloc(sizeof(HashTable), 1);
		/* Values are static C strings owned by the callers — no destructor. */
		zend_hash_init(internal_context_key_names, 8, NULL, NULL, 1);
	}

	/* The same static name gets the same key: a repeated alloc (a module
	 * started twice per process) must not mint a second identity. */
	zend_ulong existing_key;
	void *existing_name;
	ZEND_HASH_FOREACH_NUM_KEY_PTR(internal_context_key_names, existing_key, existing_name)
	{
		if ((const char *) existing_name == key_name) {
#ifdef ZTS
			tsrm_mutex_unlock(internal_context_mutex);
#endif
			return (uint32_t) existing_key;
		}
	}
	ZEND_HASH_FOREACH_END();

	const uint32_t key = internal_context_next_key++;
	zend_hash_index_add_new_ptr(internal_context_key_names, key, (void *) key_name);

#ifdef ZTS
	tsrm_mutex_unlock(internal_context_mutex);
#endif

	return key;
}

static void internal_context_keys_shutdown(void)
{
	if (internal_context_key_names != NULL) {
		zend_hash_destroy(internal_context_key_names);
		pefree(internal_context_key_names, 1);
		internal_context_key_names = NULL;
	}

	internal_context_next_key = 1;

#ifdef ZTS
	if (internal_context_mutex != NULL) {
		tsrm_mutex_free(internal_context_mutex);
		internal_context_mutex = NULL;
	}
#endif
}

static zend_always_inline zend_coroutine_t *internal_context_coroutine(
		zend_coroutine_t *coroutine)
{
	return coroutine != NULL ? coroutine : ZEND_ASYNC_CURRENT_COROUTINE;
}

ZEND_API zval *zend_async_internal_context_find(zend_coroutine_t *coroutine, uint32_t key)
{
	coroutine = internal_context_coroutine(coroutine);

	if (coroutine == NULL) {
		return NULL;
	}

	return zend_hash_index_find(&coroutine->internal_context, key);
}

ZEND_API bool zend_async_internal_context_set(
		zend_coroutine_t *coroutine, uint32_t key, zval *value)
{
	coroutine = internal_context_coroutine(coroutine);

	if (coroutine == NULL) {
		return false;
	}

	Z_TRY_ADDREF_P(value);
	zend_hash_index_update(&coroutine->internal_context, key, value);
	return true;
}

ZEND_API bool zend_async_internal_context_unset(zend_coroutine_t *coroutine, uint32_t key)
{
	coroutine = internal_context_coroutine(coroutine);

	if (coroutine == NULL) {
		return false;
	}

	return zend_hash_index_del(&coroutine->internal_context, key) == SUCCESS;
}

ZEND_API void zend_async_internal_context_init(zend_coroutine_t *coroutine)
{
	/* The table is embedded to spare an allocation; zend_hash_init defers the
	 * bucket array to the first insert, so an unused context costs nothing. */
	zend_hash_init(&coroutine->internal_context, 8, NULL, ZVAL_PTR_DTOR, false);
}

ZEND_API void zend_async_internal_context_destroy(zend_coroutine_t *coroutine)
{
	zend_hash_destroy(&coroutine->internal_context);
}

///////////////////////////////////////////////////////////////////
/// Userland context
///////////////////////////////////////////////////////////////////

ZEND_API zend_async_new_context_t zend_async_new_context_fn = NULL;

typedef struct {
	zval key;
	zval value;
} async_context_entry_t;

static void async_context_entry_dtor(zval *zv)
{
	async_context_entry_t *entry = Z_PTR_P(zv);

	zval_ptr_dtor(&entry->key);
	zval_ptr_dtor(&entry->value);
	efree(entry);
}

ZEND_API void zend_async_context_tables_init(zend_async_context_t *context)
{
	zend_hash_init(&context->string_keys, 8, NULL, ZVAL_PTR_DTOR, false);
	zend_hash_init(&context->object_keys, 8, NULL, async_context_entry_dtor, false);
}

ZEND_API void zend_async_context_tables_destroy(zend_async_context_t *context)
{
	zend_hash_destroy(&context->string_keys);
	zend_hash_destroy(&context->object_keys);
}

ZEND_API zval *zend_async_context_entry_find(
		zend_async_context_t *context, zend_string *skey, zend_object *okey)
{
	if (skey != NULL) {
		return zend_hash_find(&context->string_keys, skey);
	}

	async_context_entry_t *entry = zend_hash_index_find_ptr(&context->object_keys, okey->handle);

	return entry != NULL ? &entry->value : NULL;
}

ZEND_API void zend_async_context_entry_set(
		zend_async_context_t *context, zend_string *skey, zend_object *okey, zval *value)
{
	if (skey != NULL) {
		Z_TRY_ADDREF_P(value);
		zend_hash_update(&context->string_keys, skey, value);
		return;
	}

	async_context_entry_t *entry = zend_hash_index_find_ptr(&context->object_keys, okey->handle);

	if (entry != NULL) {
		/* The key object is already owned by the entry: only the value moves. */
		zval old_value;
		ZVAL_COPY_VALUE(&old_value, &entry->value);
		ZVAL_COPY(&entry->value, value);
		zval_ptr_dtor(&old_value);
		return;
	}

	entry = emalloc(sizeof(*entry));
	ZVAL_OBJ_COPY(&entry->key, okey);
	ZVAL_COPY(&entry->value, value);
	zend_hash_index_add_new_ptr(&context->object_keys, okey->handle, entry);
}

ZEND_API bool zend_async_context_entry_unset(
		zend_async_context_t *context, zend_string *skey, zend_object *okey)
{
	if (skey != NULL) {
		return zend_hash_del(&context->string_keys, skey) == SUCCESS;
	}

	return zend_hash_index_del(&context->object_keys, okey->handle) == SUCCESS;
}

ZEND_API void zend_async_context_entry_gc(zend_async_context_t *context, zend_get_gc_buffer *buf)
{
	zval *value;

	ZEND_HASH_FOREACH_VAL(&context->string_keys, value)
	{
		zend_get_gc_buffer_add_zval(buf, value);
	}
	ZEND_HASH_FOREACH_END();

	async_context_entry_t *entry;

	ZEND_HASH_FOREACH_PTR(&context->object_keys, entry)
	{
		zend_get_gc_buffer_add_zval(buf, &entry->key);
		zend_get_gc_buffer_add_zval(buf, &entry->value);
	}
	ZEND_HASH_FOREACH_END();
}

ZEND_API zend_object *zend_async_context_get(zend_coroutine_t *coroutine)
{
	if (coroutine == NULL) {
		coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
	}

	/* No coroutine (concurrency off) or no class provider: no context. */
	if (coroutine == NULL || zend_async_new_context_fn == NULL) {
		return NULL;
	}

	if (coroutine->context == NULL) {
		coroutine->context = zend_async_new_context_fn();
	}

	return coroutine->context;
}

/* Split a string-or-object key zval; any other type is a programming error
 * of the C caller. */
static bool async_context_key_split(zval *key, zend_string **skey, zend_object **okey)
{
	if (Z_TYPE_P(key) == IS_STRING) {
		*skey = Z_STR_P(key);
		return true;
	}

	if (Z_TYPE_P(key) == IS_OBJECT) {
		*okey = Z_OBJ_P(key);
		return true;
	}

	return false;
}

ZEND_API zval *zend_async_context_find(zend_coroutine_t *coroutine, zval *key)
{
	zend_string *skey = NULL;
	zend_object *okey = NULL;

	if (!async_context_key_split(key, &skey, &okey)) {
		return NULL;
	}

	zend_object *store = zend_async_context_get(coroutine);

	if (store == NULL) {
		return NULL;
	}

	return zend_async_context_entry_find(ZEND_ASYNC_CONTEXT_FROM_OBJ(store), skey, okey);
}

ZEND_API bool zend_async_context_set(zend_coroutine_t *coroutine, zval *key, zval *value)
{
	zend_string *skey = NULL;
	zend_object *okey = NULL;

	if (!async_context_key_split(key, &skey, &okey)) {
		return false;
	}

	zend_object *store = zend_async_context_get(coroutine);

	if (store == NULL) {
		return false;
	}

	zend_async_context_entry_set(ZEND_ASYNC_CONTEXT_FROM_OBJ(store), skey, okey, value);
	return true;
}

ZEND_API bool zend_async_context_unset(zend_coroutine_t *coroutine, zval *key)
{
	zend_string *skey = NULL;
	zend_object *okey = NULL;

	if (!async_context_key_split(key, &skey, &okey)) {
		return false;
	}

	zend_object *store = zend_async_context_get(coroutine);

	if (store == NULL) {
		return false;
	}

	return zend_async_context_entry_unset(ZEND_ASYNC_CONTEXT_FROM_OBJ(store), skey, okey);
}

ZEND_API void zend_async_context_destroy(zend_coroutine_t *coroutine)
{
	if (coroutine == NULL) {
		coroutine = ZEND_ASYNC_CURRENT_COROUTINE;

		if (coroutine == NULL) {
			return;
		}
	}

	if (coroutine->context != NULL) {
		OBJ_RELEASE(coroutine->context);
		coroutine->context = NULL;
	}
}

///////////////////////////////////////////////////////////////////
/// Default slot implementations (no scheduler registered)
///////////////////////////////////////////////////////////////////

static ZEND_COLD void throw_no_scheduler(void)
{
	zend_throw_error(NULL, "The Async API requires a scheduler implementation to be registered");
}

/* Reachable without a scheduler: the engine and userland may hit these
 * slots on a plain script. The rest of the table (new_coroutine) speaks
 * about a live coroutine, which cannot exist while no scheduler is
 * registered — those slots stay NULL, so a caller that reaches them is a
 * bug and dies loudly instead of getting a polite exception. */

static bool enqueue_coroutine_stub(
		zend_coroutine_t *coroutine, zend_object *error, bool transfer_error)
{
	(void) coroutine;

	if (error != NULL && transfer_error) {
		OBJ_RELEASE(error);
	}

	throw_no_scheduler();
	return false;
}

static bool suspend_stub(bool from_main, bool is_bailout)
{
	(void) from_main;
	(void) is_bailout;
	throw_no_scheduler();
	return false;
}

static bool await_stub(zend_coroutine_t *coroutine)
{
	(void) coroutine;
	throw_no_scheduler();
	return false;
}

static bool cancel_stub(
		zend_coroutine_t *coroutine, zend_object *error, bool transfer_error, const bool is_safely)
{
	(void) coroutine;
	(void) is_safely;

	if (error != NULL && transfer_error) {
		OBJ_RELEASE(error);
	}

	zend_throw_error(NULL, "The Async scheduler does not support coroutine cancellation");
	return false;
}

static zend_coroutine_t *launch_stub(void)
{
	return NULL;
}

static bool defer_stub(zend_async_microtask_t *task)
{
	(void) task;
	throw_no_scheduler();
	return false;
}

static bool shutdown_stub(void)
{
	return false;
}

static zend_class_entry *get_class_ce_default(zend_async_class type)
{
	/* Without a scheduler there are no Async classes; every exception type
	 * degrades to the base \Exception so error paths still work. */
	if (type >= ZEND_ASYNC_EXCEPTION_DEFAULT) {
		return zend_ce_exception;
	}

	return NULL;
}

static void default_call_on_main_stack(void (*fn)(void *), void *arg)
{
	fn(arg);
}

ZEND_API zend_async_new_coroutine_t zend_async_new_coroutine_fn = NULL;
ZEND_API zend_async_gc_new_coroutine_t zend_async_gc_new_coroutine_fn = NULL;
ZEND_API zend_async_enqueue_coroutine_t zend_async_enqueue_coroutine_fn = enqueue_coroutine_stub;
ZEND_API zend_async_suspend_t zend_async_suspend_fn = suspend_stub;
ZEND_API zend_async_cancel_t zend_async_cancel_fn = cancel_stub;
ZEND_API zend_async_scheduler_launch_t zend_async_scheduler_launch_fn = launch_stub;
ZEND_API zend_async_shutdown_t zend_async_shutdown_fn = shutdown_stub;
ZEND_API zend_async_get_class_ce_t zend_async_get_class_ce_fn = get_class_ce_default;
ZEND_API zend_async_call_on_main_stack_t zend_async_call_on_main_stack_fn =
		default_call_on_main_stack;
ZEND_API zend_async_defer_t zend_async_defer_fn = defer_stub;
ZEND_API zend_async_coroutine_from_object_t zend_async_coroutine_from_object_fn = NULL;
ZEND_API zend_async_intercept_fiber_t zend_async_intercept_fiber_fn = NULL;
ZEND_API zend_async_coroutine_execute_data_t zend_async_coroutine_execute_data_fn = NULL;
ZEND_API zend_async_coroutine_add_switch_handler_t zend_async_coroutine_add_switch_handler_fn = NULL;
ZEND_API zend_async_coroutine_remove_switch_handler_t zend_async_coroutine_remove_switch_handler_fn = NULL;
ZEND_API zend_async_coroutine_add_finish_handler_t zend_async_coroutine_add_finish_handler_fn = NULL;
ZEND_API zend_async_coroutine_remove_finish_handler_t zend_async_coroutine_remove_finish_handler_fn = NULL;
ZEND_API zend_async_coroutine_await_t zend_async_coroutine_await_fn = await_stub;
ZEND_API zend_async_coroutine_add_awaiting_info_t zend_async_coroutine_add_awaiting_info_fn = NULL;
ZEND_API zend_async_coroutine_remove_awaiting_info_t zend_async_coroutine_remove_awaiting_info_fn = NULL;
ZEND_API zend_async_coroutine_get_awaiting_info_t zend_async_coroutine_get_awaiting_info_fn = NULL;

///////////////////////////////////////////////////////////////////
/// Registration
///////////////////////////////////////////////////////////////////

static const char *scheduler_module_name = NULL;

/* True when the field lies within the size the provider was compiled against. */
#define API_PROVIDES(api, field) \
	((api)->size >= offsetof(zend_async_scheduler_api_t, field) + sizeof((api)->field) \
			&& (api)->field != NULL)

ZEND_API bool zend_async_scheduler_register(
		const char *module, const zend_async_scheduler_api_t *api)
{
	if (api == NULL || module == NULL) {
		return false;
	}

#ifdef ZTS
	tsrm_mutex_lock(scheduler_mutex);
#endif

	/* A scheduler is registered once per process. */
	if (scheduler_module_name != NULL) {
		const char *owner = scheduler_module_name;

#ifdef ZTS
		tsrm_mutex_unlock(scheduler_mutex);
#endif
		zend_error(E_CORE_WARNING,
				"The module %s cannot register an Async scheduler: %s already did",
				module,
				owner);
		return false;
	}

	if (API_PROVIDES(api, new_coroutine)) {
		zend_async_new_coroutine_fn = api->new_coroutine;
	}

	if (API_PROVIDES(api, gc_new_coroutine)) {
		zend_async_gc_new_coroutine_fn = api->gc_new_coroutine;
	}

	if (API_PROVIDES(api, enqueue_coroutine)) {
		zend_async_enqueue_coroutine_fn = api->enqueue_coroutine;
	}

	if (API_PROVIDES(api, suspend)) {
		zend_async_suspend_fn = api->suspend;
	}

	if (API_PROVIDES(api, cancel)) {
		zend_async_cancel_fn = api->cancel;
	}

	if (API_PROVIDES(api, launch)) {
		zend_async_scheduler_launch_fn = api->launch;
	}

	if (API_PROVIDES(api, shutdown)) {
		zend_async_shutdown_fn = api->shutdown;
	}

	if (API_PROVIDES(api, get_class_ce)) {
		zend_async_get_class_ce_fn = api->get_class_ce;
	}

	if (API_PROVIDES(api, call_on_main_stack)) {
		zend_async_call_on_main_stack_fn = api->call_on_main_stack;
	}

	if (API_PROVIDES(api, defer)) {
		zend_async_defer_fn = api->defer;
	}

	if (API_PROVIDES(api, coroutine_from_object)) {
		zend_async_coroutine_from_object_fn = api->coroutine_from_object;
	}

	if (API_PROVIDES(api, intercept_fiber)) {
		zend_async_intercept_fiber_fn = api->intercept_fiber;
	}

	if (API_PROVIDES(api, coroutine_execute_data)) {
		zend_async_coroutine_execute_data_fn = api->coroutine_execute_data;
	}

	if (API_PROVIDES(api, add_switch_handler)) {
		zend_async_coroutine_add_switch_handler_fn = api->add_switch_handler;
	}

	if (API_PROVIDES(api, remove_switch_handler)) {
		zend_async_coroutine_remove_switch_handler_fn = api->remove_switch_handler;
	}

	if (API_PROVIDES(api, add_finish_handler)) {
		zend_async_coroutine_add_finish_handler_fn = api->add_finish_handler;
	}

	if (API_PROVIDES(api, remove_finish_handler)) {
		zend_async_coroutine_remove_finish_handler_fn = api->remove_finish_handler;
	}

	if (API_PROVIDES(api, await)) {
		zend_async_coroutine_await_fn = api->await;
	}

	if (API_PROVIDES(api, add_awaiting_info)) {
		zend_async_coroutine_add_awaiting_info_fn = api->add_awaiting_info;
	}

	if (API_PROVIDES(api, remove_awaiting_info)) {
		zend_async_coroutine_remove_awaiting_info_fn = api->remove_awaiting_info;
	}

	if (API_PROVIDES(api, get_awaiting_info)) {
		zend_async_coroutine_get_awaiting_info_fn = api->get_awaiting_info;
	}

	/* The caller's string may be request-local: keep a process copy. */
	scheduler_module_name = pestrdup(module, 1);

#ifdef ZTS
	tsrm_mutex_unlock(scheduler_mutex);
#endif

	ZEND_ASYNC_INITIALIZE;

	return true;
}

/* Withdraw the registration and reset every slot to its default. The slots
 * are process-wide: this runs at process shutdown, or when a just-registered
 * scheduler fails to launch — never per request. The internal-context key
 * registry is NOT touched here. */
ZEND_API void zend_async_scheduler_unregister(void)
{
#ifdef ZTS
	tsrm_mutex_lock(scheduler_mutex);
#endif

	if (scheduler_module_name != NULL) {
		pefree((char *) scheduler_module_name, 1);
		scheduler_module_name = NULL;
	}

	zend_async_new_coroutine_fn = NULL;
	zend_async_gc_new_coroutine_fn = NULL;
	zend_async_enqueue_coroutine_fn = enqueue_coroutine_stub;
	zend_async_suspend_fn = suspend_stub;
	zend_async_cancel_fn = cancel_stub;
	zend_async_scheduler_launch_fn = launch_stub;
	zend_async_shutdown_fn = shutdown_stub;
	zend_async_get_class_ce_fn = get_class_ce_default;
	zend_async_call_on_main_stack_fn = default_call_on_main_stack;
	zend_async_defer_fn = defer_stub;
	zend_async_coroutine_from_object_fn = NULL;
	zend_async_intercept_fiber_fn = NULL;
	zend_async_coroutine_execute_data_fn = NULL;
	zend_async_coroutine_add_switch_handler_fn = NULL;
	zend_async_coroutine_remove_switch_handler_fn = NULL;
	zend_async_coroutine_add_finish_handler_fn = NULL;
	zend_async_coroutine_remove_finish_handler_fn = NULL;
	zend_async_coroutine_await_fn = await_stub;
	zend_async_coroutine_add_awaiting_info_fn = NULL;
	zend_async_coroutine_remove_awaiting_info_fn = NULL;
	zend_async_coroutine_get_awaiting_info_fn = NULL;

#ifdef ZTS
	tsrm_mutex_unlock(scheduler_mutex);
#endif
}

void zend_async_api_shutdown(void)
{
	zend_async_globals_dtor();
	zend_async_scheduler_unregister();
	internal_context_keys_shutdown();

#ifdef ZTS
	if (scheduler_mutex != NULL) {
		tsrm_mutex_free(scheduler_mutex);
		scheduler_mutex = NULL;
	}
#endif
}

ZEND_API bool zend_async_is_enabled(void)
{
	return scheduler_module_name != NULL;
}

ZEND_API const char *zend_async_get_scheduler_module(void)
{
	return scheduler_module_name;
}

ZEND_API zend_coroutine_t *zend_async_coroutine_from_object(zend_object *object)
{
	if (zend_async_coroutine_from_object_fn == NULL) {
		return NULL;
	}

	return zend_async_coroutine_from_object_fn(object);
}

///////////////////////////////////////////////////////////////////
/// Launch
///////////////////////////////////////////////////////////////////

ZEND_API bool zend_async_scheduler_launch(void)
{
	if (scheduler_module_name == NULL) {
		return true;
	}

	zend_coroutine_t *main_coroutine = zend_async_scheduler_launch_fn();

	if (main_coroutine == NULL) {
		zend_throw_error(
				NULL, "The scheduler failed to launch: no main coroutine for the top-level script");
		return false;
	}

	/* The top-level script runs inside a coroutine from here on: the engine
	 * knows the main flow instead of discovering it at its first yield. */
	ZEND_COROUTINE_SET_MAIN(main_coroutine);
	ZEND_COROUTINE_SET_STATUS(main_coroutine, ZEND_COROUTINE_STATUS_RUNNING);
	ZEND_ASYNC_MAIN_COROUTINE = main_coroutine;
	ZEND_ASYNC_CURRENT_COROUTINE = main_coroutine;
	ZEND_ASYNC_ACTIVATE;

	return EG(exception) == NULL;
}
