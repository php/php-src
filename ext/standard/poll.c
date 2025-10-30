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
   | Author: Jakub Zelenka <bukka@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "php_network.h"
#include "php_poll.h"
#include "poll_arginfo.h"

/* Class entries */
static zend_class_entry *php_poll_backend_class_entry;
static zend_class_entry *php_poll_context_class_entry;
static zend_class_entry *php_poll_watcher_class_entry;
static zend_class_entry *php_poll_handle_class_entry;
static zend_class_entry *php_stream_poll_handle_class_entry;
static zend_class_entry *php_poll_exception_class_entry;

/* Object handlers */
static zend_object_handlers php_poll_context_object_handlers;
static zend_object_handlers php_poll_watcher_object_handlers;
static zend_object_handlers php_poll_handle_object_handlers;

/* Watcher object structure */
typedef struct {
	php_poll_handle_object *handle;
	uint32_t watched_events;
	uint32_t triggered_events;
	zval data;
	bool active;
	php_poll_ctx *poll_ctx; /* Back reference to poll context */
	zend_object std;
} php_poll_watcher_object;

/* Context object structure */
typedef struct {
	php_poll_ctx *ctx;
	HashTable *watchers; /* Maps handle pointer -> watcher object */
	zend_object std;
} php_poll_context_object;

/* Stream poll handle specific data */
typedef struct {
	php_stream *stream;
} php_stream_poll_handle_data;

/* Accessor macros */
#define PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(_obj) \
	((php_poll_context_object *) ((char *) (_obj) - XtOffsetOf(php_poll_context_object, std)))

#define PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(_obj) \
	((php_poll_watcher_object *) ((char *) (_obj) - XtOffsetOf(php_poll_watcher_object, std)))

#define PHP_POLL_WATCHER_OBJ_FROM_ZV(_zv) PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))
#define PHP_POLL_CONTEXT_OBJ_FROM_ZV(_zv) PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))

/* === Stream Poll Handle Implementation === */

static php_socket_t php_stream_poll_handle_get_fd(php_poll_handle_object *handle)
{
	php_stream_poll_handle_data *data = (php_stream_poll_handle_data *) handle->handle_data;
	php_socket_t fd;

	if (!data || !data->stream) {
		return SOCK_ERR;
	}

	if (php_stream_cast(data->stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd,
				1) != SUCCESS
			|| fd == -1) {
		return SOCK_ERR;
	}

	return fd;
}

static int php_stream_poll_handle_is_valid(php_poll_handle_object *handle)
{
	php_stream_poll_handle_data *data = (php_stream_poll_handle_data *) handle->handle_data;
	return data && data->stream && !php_stream_eof(data->stream);
}

static void php_stream_poll_handle_cleanup(php_poll_handle_object *handle)
{
	php_stream_poll_handle_data *data = (php_stream_poll_handle_data *) handle->handle_data;
	if (data) {
		/* Don't close the stream - user still owns it */
		efree(data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops php_stream_poll_handle_ops = {
	.get_fd = php_stream_poll_handle_get_fd,
	.is_valid = php_stream_poll_handle_is_valid,
	.cleanup = php_stream_poll_handle_cleanup
};

/* === Object Creation Functions === */

static zend_object *php_poll_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_poll_handle_default_ops);
	return &intern->std;
}

static zend_object *php_stream_poll_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_stream_poll_handle_ops);
	return &intern->std;
}

static zend_object *php_poll_watcher_create_object(zend_class_entry *ce)
{
	php_poll_watcher_object *intern = zend_object_alloc(sizeof(php_poll_watcher_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->handle = NULL;
	intern->watched_events = 0;
	intern->triggered_events = 0;
	intern->active = false;
	intern->poll_ctx = NULL;
	ZVAL_NULL(&intern->data);

	return &intern->std;
}

static zend_object *php_poll_context_create_object(zend_class_entry *ce)
{
	php_poll_context_object *intern = zend_object_alloc(sizeof(php_poll_context_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->ctx = NULL;
	intern->watchers = NULL;

	return &intern->std;
}

/* === Object Destruction Functions === */

static void php_poll_watcher_free_object(zend_object *obj)
{
	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(obj);

	zval_ptr_dtor(&intern->data);

	if (intern->handle) {
		OBJ_RELEASE(&intern->handle->std);
	}

	zend_object_std_dtor(&intern->std);
}

static void php_poll_context_free_object(zend_object *obj)
{
	php_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(obj);

	if (intern->ctx) {
		php_poll_destroy(intern->ctx);
	}

	if (intern->watchers) {
		zend_hash_destroy(intern->watchers);
		efree(intern->watchers);
	}

	zend_object_std_dtor(&intern->std);
}

/* === Utility functions === */

static zend_always_inline zend_ulong php_poll_compute_ptr_key(void *ptr)
{
	zend_ulong key = (zend_ulong) (uintptr_t) ptr;
	return (key >> 3) | (key << ((sizeof(key) * 8) - 3));
}

static zend_result php_poll_watcher_modify_events(php_poll_watcher_object *watcher, uint32_t events)
{
	if (!watcher->active || !watcher->poll_ctx) {
		zend_throw_exception(php_poll_exception_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	php_socket_t fd = php_poll_handle_get_fd(watcher->handle);
	if (fd == SOCK_ERR) {
		zend_throw_exception(php_poll_exception_class_entry, "Invalid handle for polling", 0);
		return FAILURE;
	}

	/* Modify in poll context */
	if (php_poll_modify(watcher->poll_ctx, (int) fd, events, watcher) != SUCCESS) {
		zend_throw_exception(
				php_poll_exception_class_entry, "Failed to modify watcher in polling system", 0);
		return FAILURE;
	}

	/* Update watcher state */
	watcher->watched_events = events;

	return SUCCESS;
}

static zend_result php_poll_watcher_modify_data(php_poll_watcher_object *watcher, zval *data)
{
	if (!watcher->active) {
		zend_throw_exception(php_poll_exception_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	/* Update user data */
	zval_ptr_dtor(&watcher->data);
	ZVAL_COPY(&watcher->data, data);

	return SUCCESS;
}

/* === PHP Method Implementations === */

PHP_METHOD(PollHandle, getFileDescriptor)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(getThis());
	php_socket_t fd = php_poll_handle_get_fd(intern);

	if (fd == SOCK_ERR) {
		RETURN_LONG(0);
	}

	RETURN_LONG((zend_long) fd);
}

PHP_METHOD(StreamPollHandle, __construct)
{
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		PHP_Z_PARAM_STREAM(stream)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(getThis());

	/* Set up stream-specific data */
	php_stream_poll_handle_data *data = emalloc(sizeof(php_stream_poll_handle_data));
	data->stream = stream;
	intern->handle_data = data;

	/* Add reference to stream */
	GC_ADDREF(stream->res);
}

PHP_METHOD(StreamPollHandle, getStream)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(getThis());
	php_stream_poll_handle_data *data = (php_stream_poll_handle_data *) intern->handle_data;

	if (!data || !data->stream) {
		RETURN_NULL();
	}

	GC_ADDREF(data->stream->res);
	php_stream_to_zval(data->stream, return_value);
}

PHP_METHOD(StreamPollHandle, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(getThis());
	RETURN_BOOL(intern->ops->is_valid(intern));
}

PHP_METHOD(PollWatcher, __construct)
{
	zval *handle_obj;
	zend_long events;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(handle_obj, php_poll_handle_class_entry)
		Z_PARAM_LONG(events)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	intern->handle = PHP_POLL_HANDLE_OBJ_FROM_ZV(handle_obj);
	intern->watched_events = (uint32_t) events;
	intern->triggered_events = 0;
	intern->active = false;

	GC_ADDREF(&intern->handle->std);

	if (data) {
		ZVAL_COPY(&intern->data, data);
	} else {
		ZVAL_NULL(&intern->data);
	}
}

PHP_METHOD(PollWatcher, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	if (!intern->handle) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(&intern->handle->std);
}

PHP_METHOD(PollWatcher, getWatchedEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_LONG((zend_long) intern->watched_events);
}

PHP_METHOD(PollWatcher, getTriggeredEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_LONG((zend_long) intern->triggered_events);
}

PHP_METHOD(PollWatcher, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	ZVAL_COPY(return_value, &intern->data);
}

PHP_METHOD(PollWatcher, hasTriggered)
{
	zend_long events;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(events)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_BOOL((intern->triggered_events & (uint32_t) events) != 0);
}

PHP_METHOD(PollWatcher, isActive)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_BOOL(intern->active);
}

PHP_METHOD(PollWatcher, modify)
{
	zend_long events;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(events)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	/* Modify events first */
	if (php_poll_watcher_modify_events(intern, (uint32_t) events) != SUCCESS) {
		RETURN_THROWS();
	}

	/* Then modify data if provided */
	if (data) {
		if (php_poll_watcher_modify_data(intern, data) != SUCCESS) {
			RETURN_THROWS();
		}
	}
}

PHP_METHOD(PollWatcher, modifyEvents)
{
	zend_long events;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(events)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (php_poll_watcher_modify_events(intern, (uint32_t) events) != SUCCESS) {
		RETURN_THROWS();
	}
}

PHP_METHOD(PollWatcher, modifyData)
{
	zval *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (php_poll_watcher_modify_data(intern, data) != SUCCESS) {
		RETURN_THROWS();
	}
}

PHP_METHOD(PollWatcher, remove)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (!intern->active || !intern->poll_ctx) {
		zend_throw_exception(php_poll_exception_class_entry, "Cannot remove inactive watcher", 0);
		RETURN_THROWS();
	}

	php_socket_t fd = php_poll_handle_get_fd(intern->handle);
	if (fd != SOCK_ERR) {
		php_poll_remove(intern->poll_ctx, (int) fd);
	}

	intern->active = false;
	intern->poll_ctx = NULL;
}

PHP_METHOD(PollContext, __construct)
{
	zval *backend_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(backend_obj, php_poll_backend_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());

	if (backend_obj == NULL) {
		intern->ctx = php_poll_create(PHP_POLL_BACKEND_AUTO, 0);
	} else {
		const zend_string *backend_str = Z_STR_P(zend_enum_fetch_case_value(Z_OBJ_P(backend_obj)));
		intern->ctx = php_poll_create_by_name(ZSTR_VAL(backend_str), 0);
	}

	if (!intern->ctx) {
		zend_throw_exception(php_poll_exception_class_entry, "Failed to create polling context", 0);
		RETURN_THROWS();
	}

	if (php_poll_init(intern->ctx) != SUCCESS) {
		php_poll_destroy(intern->ctx);
		intern->ctx = NULL;
		zend_throw_exception(
				php_poll_exception_class_entry, "Failed to initialize polling context", 0);
		RETURN_THROWS();
	}

	intern->watchers = emalloc(sizeof(HashTable));
	zend_hash_init(intern->watchers, 8, NULL, ZVAL_PTR_DTOR, 0);
}

PHP_METHOD(PollContext, add)
{
	zval *handle_obj;
	zend_long events;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(handle_obj, php_poll_handle_class_entry)
		Z_PARAM_LONG(events)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZV(handle_obj);

	/* Get file descriptor */
	php_socket_t fd = php_poll_handle_get_fd(handle);
	if (fd == SOCK_ERR) {
		zend_throw_exception(php_poll_exception_class_entry, "Invalid handle for polling", 0);
		RETURN_THROWS();
	}

	/* Create watcher object */
	object_init_ex(return_value, php_poll_watcher_class_entry);
	php_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZV(return_value);

	watcher->handle = handle;
	watcher->watched_events = (uint32_t) events;
	watcher->triggered_events = 0;
	watcher->active = true;
	watcher->poll_ctx = intern->ctx;

	GC_ADDREF(&handle->std);

	if (data) {
		ZVAL_COPY(&watcher->data, data);
	} else {
		ZVAL_NULL(&watcher->data);
	}

	/* Add to poll context */
	if (php_poll_add(intern->ctx, (int) fd, (uint32_t) events, watcher) != SUCCESS) {
		if (php_poll_get_error(intern->ctx) == PHP_POLL_ERR_EXISTS) {
			zend_throw_exception(
					php_poll_exception_class_entry, "Handle already added", 0);
		} else {
			zend_throw_exception(
				php_poll_exception_class_entry, "Failed to add handle", 0);
		}
		RETURN_THROWS();
	}

	/* Now mark as active */
	watcher->active = true;
	watcher->poll_ctx = intern->ctx;

	/* Store in our watchers map using shifted pointer as key */
	zval watcher_zv;
	ZVAL_OBJ(&watcher_zv, &watcher->std);
	GC_ADDREF(&watcher->std);

	zend_ulong hash_key = php_poll_compute_ptr_key(handle);
	zend_hash_index_add(intern->watchers, hash_key, &watcher_zv);
}

PHP_METHOD(PollContext, wait)
{
	zend_long timeout = -1;
	zend_long max_events = -1;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(timeout)
		Z_PARAM_LONG(max_events)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());

	if (max_events <= 0) {
		max_events = php_poll_get_suitable_max_events(intern->ctx);
		if (max_events <= 0) {
			max_events = 64;
		}
	}

	php_poll_event *events = emalloc(sizeof(php_poll_event) * max_events);
	int num_events = php_poll_wait(intern->ctx, events, max_events, (int) timeout);

	if (num_events < 0) {
		efree(events);
		zend_throw_exception(php_poll_exception_class_entry, "Poll wait failed", 0);
		RETURN_THROWS();
	}

	array_init(return_value);

	for (int i = 0; i < num_events; i++) {
		php_poll_watcher_object *watcher = (php_poll_watcher_object *) events[i].data;
		if (watcher) {
			watcher->triggered_events = events[i].revents;

			zval watcher_zv;
			ZVAL_OBJ(&watcher_zv, &watcher->std);
			GC_ADDREF(&watcher->std);

			add_next_index_zval(return_value, &watcher_zv);
		}
	}

	efree(events);
}

PHP_METHOD(PollContext, getBackend)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());
	const char *backend_name = php_poll_backend_name(intern->ctx);
	const char *entryname = NULL;

	if (!strcmp(backend_name, "epoll")) {
		entryname = "Epoll";
	} else if (!strcmp(backend_name, "kqueu")) {
		entryname = "Kqueue";
	} else if (!strcmp(backend_name, "wsapoll")) {
		entryname = "WSAPoll";
	} else if (!strcmp(backend_name, "eventport")) {
		entryname = "EventPorts";
	} else if (!strcmp(backend_name, "poll")) {
		entryname = "Poll";
	} else {
		entryname = "Auto"; /* This should never happen */
	}

	RETURN_OBJ_COPY(zend_enum_get_case_cstr(php_poll_backend_class_entry, entryname));
}

/* Initialize the stream poll classes - add to PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(poll)
{
	/* Register symbols */
	register_poll_symbols(module_number);

	/* Register backend */
	php_poll_backend_class_entry = register_class_PollBackend();

	/* Register base PollHandle class */
	php_poll_handle_class_entry = register_class_PollHandle();
	php_poll_handle_class_entry->create_object = php_poll_handle_create_object;

	memcpy(&php_poll_handle_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_poll_handle_object_handlers.offset = XtOffsetOf(php_poll_handle_object, std);
	php_poll_handle_object_handlers.free_obj = php_poll_handle_object_free;
	php_poll_handle_class_entry->default_object_handlers = &php_poll_handle_object_handlers;

	/* Register StreamPollHandle class */
	php_stream_poll_handle_class_entry
			= register_class_StreamPollHandle(php_poll_handle_class_entry);
	php_stream_poll_handle_class_entry->create_object = php_stream_poll_handle_create_object;

	/* Register PollWatcher class */
	php_poll_watcher_class_entry = register_class_PollWatcher();
	php_poll_watcher_class_entry->create_object = php_poll_watcher_create_object;

	memcpy(&php_poll_watcher_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_poll_watcher_object_handlers.offset = XtOffsetOf(php_poll_watcher_object, std);
	php_poll_watcher_object_handlers.free_obj = php_poll_watcher_free_object;
	php_poll_watcher_class_entry->default_object_handlers = &php_poll_watcher_object_handlers;

	/* Register PollContext class */
	php_poll_context_class_entry = register_class_PollContext();
	php_poll_context_class_entry->create_object = php_poll_context_create_object;

	memcpy(&php_poll_context_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_poll_context_object_handlers.offset = XtOffsetOf(php_poll_context_object, std);
	php_poll_context_object_handlers.free_obj = php_poll_context_free_object;
	php_poll_context_class_entry->default_object_handlers = &php_poll_context_object_handlers;

	/* Register exception class */
	php_poll_exception_class_entry = register_class_PollException(zend_ce_exception);

	/* Initialize polling backends */
	php_poll_register_backends();

	return SUCCESS;
}
