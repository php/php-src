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
#include "io_poll_arginfo.h"

/* Class entries */
static zend_class_entry *php_io_poll_backend_class_entry;
static zend_class_entry *php_io_poll_event_class_entry;
static zend_class_entry *php_io_poll_context_class_entry;
static zend_class_entry *php_io_poll_watcher_class_entry;
static zend_class_entry *php_io_poll_handle_class_entry;
static zend_class_entry *php_io_poll_exception_class_entry;
static zend_class_entry *php_stream_poll_handle_class_entry;

/* Object handlers */
static zend_object_handlers php_io_poll_context_object_handlers;
static zend_object_handlers php_io_poll_watcher_object_handlers;
static zend_object_handlers php_io_poll_handle_object_handlers;

/* Watcher object structure */
typedef struct {
	php_poll_handle_object *handle;
	uint32_t watched_events;
	uint32_t triggered_events;
	zval data;
	bool active;
	php_poll_ctx *poll_ctx; /* Back reference to poll context */
	zend_object std;
} php_io_poll_watcher_object;

/* Context object structure */
typedef struct {
	php_poll_ctx *ctx;
	HashTable *watchers; /* Maps handle pointer -> watcher object */
	zend_object std;
} php_io_poll_context_object;

/* Stream poll handle specific data */
typedef struct {
	php_stream *stream;
} php_stream_poll_handle_data;

/* Accessor macros */
#define PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(_obj) \
	((php_io_poll_context_object *) ((char *) (_obj) - XtOffsetOf(php_io_poll_context_object, std)))

#define PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(_obj) \
	((php_io_poll_watcher_object *) ((char *) (_obj) - XtOffsetOf(php_io_poll_watcher_object, std)))

#define PHP_POLL_WATCHER_OBJ_FROM_ZV(_zv) PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))
#define PHP_POLL_CONTEXT_OBJ_FROM_ZV(_zv) PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))

/* Event enum to bit mask mapping */
static uint32_t php_io_poll_event_enum_to_bit(zend_object *event_enum)
{
	zval *case_name = zend_enum_fetch_case_name(event_enum);
	const char *name = Z_STRVAL_P(case_name);

	if (strcmp(name, "Read") == 0) {
		return PHP_POLL_READ;
	} else if (strcmp(name, "Write") == 0) {
		return PHP_POLL_WRITE;
	} else if (strcmp(name, "Error") == 0) {
		return PHP_POLL_ERROR;
	} else if (strcmp(name, "HangUp") == 0) {
		return PHP_POLL_HUP;
	} else if (strcmp(name, "ReadHangUp") == 0) {
		return PHP_POLL_RDHUP;
	} else if (strcmp(name, "OneShot") == 0) {
		return PHP_POLL_ONESHOT;
	} else if (strcmp(name, "EdgeTriggered") == 0) {
		return PHP_POLL_ET;
	}

	return 0;
}

static uint32_t php_io_poll_event_enums_to_events(zval *event_enums)
{
	HashTable *ht;
	zval *entry;
	uint32_t events = 0;

	if (Z_TYPE_P(event_enums) != IS_ARRAY) {
		return 0;
	}

	ht = Z_ARRVAL_P(event_enums);

	ZEND_HASH_FOREACH_VAL(ht, entry) {
		if (Z_TYPE_P(entry) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(entry), php_io_poll_event_class_entry)) {
			return 0;
		}
		events |= php_io_poll_event_enum_to_bit(Z_OBJ_P(entry));
	} ZEND_HASH_FOREACH_END();

	return events;
}

static zend_result php_io_poll_events_to_event_enums(uint32_t events, zval *event_enums)
{
	zval enum_case;
	
	array_init(event_enums);

	if (events & PHP_POLL_READ) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "Read"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_WRITE) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "Write"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ERROR) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "Error"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_HUP) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "HangUp"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_RDHUP) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "ReadHangUp"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ONESHOT) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "OneShot"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ET) {
		ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_event_class_entry, "EdgeTriggered"));
		GC_ADDREF(Z_OBJ(enum_case));
		add_next_index_zval(event_enums, &enum_case);
	}

	return SUCCESS;
}

/* Backend enum name to backend type mapping */
static php_poll_backend_type php_io_poll_backend_enum_to_type(zend_object *backend_enum)
{
	zval *case_name = zend_enum_fetch_case_name(backend_enum);
	const char *name = Z_STRVAL_P(case_name);

	if (strcmp(name, "Auto") == 0) {
		return PHP_POLL_BACKEND_AUTO;
	} else if (strcmp(name, "Poll") == 0) {
		return PHP_POLL_BACKEND_POLL;
	} else if (strcmp(name, "Epoll") == 0) {
		return PHP_POLL_BACKEND_EPOLL;
	} else if (strcmp(name, "Kqueue") == 0) {
		return PHP_POLL_BACKEND_KQUEUE;
	} else if (strcmp(name, "EventPorts") == 0) {
		return PHP_POLL_BACKEND_EVENTPORT;
	} else if (strcmp(name, "WSAPoll") == 0) {
		return PHP_POLL_BACKEND_WSAPOLL;
	}

	return PHP_POLL_BACKEND_AUTO;
}

static const char *php_io_poll_backend_type_to_name(php_poll_backend_type type)
{
	switch (type) {
		case PHP_POLL_BACKEND_POLL:
			return "Poll";
		case PHP_POLL_BACKEND_EPOLL:
			return "Epoll";
		case PHP_POLL_BACKEND_KQUEUE:
			return "Kqueue";
		case PHP_POLL_BACKEND_EVENTPORT:
			return "EventPorts";
		case PHP_POLL_BACKEND_WSAPOLL:
			return "WSAPoll";
		case PHP_POLL_BACKEND_AUTO:
		default:
			return "Auto";
	}
}

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

static zend_object *php_io_poll_handle_create_object(zend_class_entry *ce)
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

static zend_object *php_io_poll_watcher_create_object(zend_class_entry *ce)
{
	php_io_poll_watcher_object *intern = zend_object_alloc(sizeof(php_io_poll_watcher_object), ce);

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

static zend_object *php_io_poll_context_create_object(zend_class_entry *ce)
{
	php_io_poll_context_object *intern = zend_object_alloc(sizeof(php_io_poll_context_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->ctx = NULL;
	intern->watchers = NULL;

	return &intern->std;
}

/* === Object Destruction Functions === */

static void php_io_poll_watcher_free_object(zend_object *obj)
{
	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(obj);

	zval_ptr_dtor(&intern->data);

	if (intern->handle) {
		OBJ_RELEASE(&intern->handle->std);
	}

	zend_object_std_dtor(&intern->std);
}

static void php_io_poll_context_free_object(zend_object *obj)
{
	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(obj);

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

static zend_always_inline zend_ulong php_io_poll_compute_ptr_key(void *ptr)
{
	zend_ulong key = (zend_ulong) (uintptr_t) ptr;
	return (key >> 3) | (key << ((sizeof(key) * 8) - 3));
}

static zend_result php_io_poll_watcher_modify_events(php_io_poll_watcher_object *watcher, uint32_t events)
{
	if (!watcher->active || !watcher->poll_ctx) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	php_socket_t fd = php_poll_handle_get_fd(watcher->handle);
	if (fd == SOCK_ERR) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Invalid handle for polling", 0);
		return FAILURE;
	}

	/* Modify in poll context */
	if (php_poll_modify(watcher->poll_ctx, (int) fd, events, watcher) != SUCCESS) {
		zend_throw_exception(
				php_io_poll_exception_class_entry, "Failed to modify watcher in polling system", 0);
		return FAILURE;
	}

	/* Update watcher state */
	watcher->watched_events = events;

	return SUCCESS;
}

static zend_result php_io_poll_watcher_modify_data(php_io_poll_watcher_object *watcher, zval *data)
{
	if (!watcher->active) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	/* Update user data */
	zval_ptr_dtor(&watcher->data);
	ZVAL_COPY(&watcher->data, data);

	return SUCCESS;
}

/* === PHP Method Implementations === */

PHP_METHOD(Io_Poll_Backend, getAvailableBackends)
{
	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	/* Check each backend type for availability */
	php_poll_backend_type backends[] = {
		PHP_POLL_BACKEND_POLL,
		PHP_POLL_BACKEND_EPOLL,
		PHP_POLL_BACKEND_KQUEUE,
		PHP_POLL_BACKEND_EVENTPORT,
		PHP_POLL_BACKEND_WSAPOLL
	};

	for (size_t i = 0; i < sizeof(backends) / sizeof(backends[0]); i++) {
		if (php_poll_is_backend_available(backends[i])) {
			const char *name = php_io_poll_backend_type_to_name(backends[i]);
			zval enum_case;
			ZVAL_OBJ(&enum_case, zend_enum_get_case_cstr(php_io_poll_backend_class_entry, name));
			add_next_index_zval(return_value, &enum_case);
		}
	}
}

PHP_METHOD(Io_Poll_Backend, isAvailable)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *enum_obj = Z_OBJ_P(ZEND_THIS);
	php_poll_backend_type type = php_io_poll_backend_enum_to_type(enum_obj);

	RETURN_BOOL(php_poll_is_backend_available(type));
}

PHP_METHOD(Io_Poll_Backend, supportsEdgeTriggering)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_object *enum_obj = Z_OBJ_P(ZEND_THIS);
	php_poll_backend_type type = php_io_poll_backend_enum_to_type(enum_obj);

	RETURN_BOOL(php_poll_backend_supports_edge_triggering(type));
}

PHP_METHOD(Io_Poll_Handle, getFileDescriptor)
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

PHP_METHOD(Io_Poll_Watcher, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Watcher, use Context::add");
}

PHP_METHOD(Io_Poll_Watcher, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	if (!intern->handle) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(&intern->handle->std);
}

PHP_METHOD(Io_Poll_Watcher, getWatchedEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	php_io_poll_events_to_event_enums(intern->watched_events, return_value);
}

PHP_METHOD(Io_Poll_Watcher, getTriggeredEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	php_io_poll_events_to_event_enums(intern->triggered_events, return_value);
}

PHP_METHOD(Io_Poll_Watcher, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	ZVAL_COPY(return_value, &intern->data);
}

PHP_METHOD(Io_Poll_Watcher, hasTriggered)
{
	zval *event_enum;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(event_enum, php_io_poll_event_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	uint32_t event_bit = php_io_poll_event_enum_to_bit(Z_OBJ_P(event_enum));

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_BOOL((intern->triggered_events & event_bit) != 0);
}

PHP_METHOD(Io_Poll_Watcher, isActive)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());
	RETURN_BOOL(intern->active);
}

PHP_METHOD(Io_Poll_Watcher, modify)
{
	zval *event_enums;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(event_enums)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	uint32_t events = php_io_poll_event_enums_to_events(event_enums);
	if (!events) {
		zend_argument_value_error(1, "must be array of Event enums");
		RETURN_THROWS();
	}

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	/* Modify events first */
	if (php_io_poll_watcher_modify_events(intern, events) != SUCCESS) {
		RETURN_THROWS();
	}

	/* Then modify data if provided */
	if (data) {
		if (php_io_poll_watcher_modify_data(intern, data) != SUCCESS) {
			RETURN_THROWS();
		}
	}
}

PHP_METHOD(Io_Poll_Watcher, modifyEvents)
{
	zval *event_enums;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(event_enums)
	ZEND_PARSE_PARAMETERS_END();

	uint32_t events = php_io_poll_event_enums_to_events(event_enums);
	if (!events) {
		zend_argument_value_error(1, "must be array of Event enums");
		RETURN_THROWS();
	}

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (php_io_poll_watcher_modify_events(intern, events) != SUCCESS) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_Watcher, modifyData)
{
	zval *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (php_io_poll_watcher_modify_data(intern, data) != SUCCESS) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_Watcher, remove)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(getThis());

	if (!intern->active || !intern->poll_ctx) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Cannot remove inactive watcher", 0);
		RETURN_THROWS();
	}

	php_socket_t fd = php_poll_handle_get_fd(intern->handle);
	if (fd != SOCK_ERR) {
		php_poll_remove(intern->poll_ctx, (int) fd);
	}

	intern->active = false;
	intern->poll_ctx = NULL;
}

PHP_METHOD(Io_Poll_Context, __construct)
{
	zval *backend_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(backend_obj, php_io_poll_backend_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());

	php_poll_backend_type backend_type = PHP_POLL_BACKEND_AUTO;
	if (backend_obj != NULL) {
		backend_type = php_io_poll_backend_enum_to_type(Z_OBJ_P(backend_obj));
	}

	intern->ctx = php_poll_create(backend_type, 0);

	if (!intern->ctx) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Failed to create polling context", 0);
		RETURN_THROWS();
	}

	if (php_poll_init(intern->ctx) != SUCCESS) {
		php_poll_destroy(intern->ctx);
		intern->ctx = NULL;
		zend_throw_exception(
				php_io_poll_exception_class_entry, "Failed to initialize polling context", 0);
		RETURN_THROWS();
	}

	intern->watchers = emalloc(sizeof(HashTable));
	zend_hash_init(intern->watchers, 8, NULL, ZVAL_PTR_DTOR, 0);
}

PHP_METHOD(Io_Poll_Context, add)
{
	zval *handle_obj, *event_enums;
	uint32_t events;
	zval *data = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(handle_obj, php_io_poll_handle_class_entry)
		Z_PARAM_ARRAY(event_enums)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZV(handle_obj);

	/* Get file descriptor */
	php_socket_t fd = php_poll_handle_get_fd(handle);
	if (fd == SOCK_ERR) {
		zend_throw_exception(php_io_poll_exception_class_entry, "Invalid handle for polling", 0);
		RETURN_THROWS();
	}

	/* Create watcher object */
	object_init_ex(return_value, php_io_poll_watcher_class_entry);
	php_io_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZV(return_value);

	events = php_io_poll_event_enums_to_events(event_enums);
	if (!events) {
		zend_argument_value_error(2, "must be array of Event enums");
		RETURN_THROWS();
	}

	watcher->handle = handle;
	watcher->watched_events = events;
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
	if (php_poll_add(intern->ctx, (int) fd, events, watcher) != SUCCESS) {
		if (php_poll_get_error(intern->ctx) == PHP_POLL_ERR_EXISTS) {
			zend_throw_exception(
					php_io_poll_exception_class_entry, "Handle already added", 0);
		} else {
			zend_throw_exception(
				php_io_poll_exception_class_entry, "Failed to add handle", 0);
		}
		RETURN_THROWS();
	}

	/* Store in our watchers map using shifted pointer as key */
	zval watcher_zv;
	ZVAL_OBJ(&watcher_zv, &watcher->std);
	GC_ADDREF(&watcher->std);

	zend_ulong hash_key = php_io_poll_compute_ptr_key(handle);
	zend_hash_index_add(intern->watchers, hash_key, &watcher_zv);
}

PHP_METHOD(Io_Poll_Context, wait)
{
	zend_long timeout = -1;
	zend_long max_events = -1;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(timeout)
		Z_PARAM_LONG(max_events)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());

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
		zend_throw_exception(php_io_poll_exception_class_entry, "Poll wait failed", 0);
		RETURN_THROWS();
	}

	array_init(return_value);

	for (int i = 0; i < num_events; i++) {
		php_io_poll_watcher_object *watcher = (php_io_poll_watcher_object *) events[i].data;
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

PHP_METHOD(Io_Poll_Context, getBackend)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(getThis());
	php_poll_backend_type backend_type = php_poll_get_backend_type(intern->ctx);
	const char *backend_name = php_io_poll_backend_type_to_name(backend_type);

	RETURN_OBJ_COPY(zend_enum_get_case_cstr(php_io_poll_backend_class_entry, backend_name));
}

/* Initialize the stream poll classes - add to PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(poll)
{
	/* Register backend enum */
	php_io_poll_backend_class_entry = register_class_Io_Poll_Backend();

	/* Register event enum */
	php_io_poll_event_class_entry = register_class_Io_Poll_Event();

	/* Register base Handle class */
	php_io_poll_handle_class_entry = register_class_Io_Poll_Handle();
	php_io_poll_handle_class_entry->create_object = php_io_poll_handle_create_object;

	memcpy(&php_io_poll_handle_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_poll_handle_object_handlers.offset = XtOffsetOf(php_poll_handle_object, std);
	php_io_poll_handle_object_handlers.free_obj = php_poll_handle_object_free;
	php_io_poll_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	/* Register StreamPollHandle class */
	php_stream_poll_handle_class_entry
			= register_class_StreamPollHandle(php_io_poll_handle_class_entry);
	php_stream_poll_handle_class_entry->create_object = php_stream_poll_handle_create_object;

	/* Register Watcher class */
	php_io_poll_watcher_class_entry = register_class_Io_Poll_Watcher();
	php_io_poll_watcher_class_entry->create_object = php_io_poll_watcher_create_object;

	memcpy(&php_io_poll_watcher_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_poll_watcher_object_handlers.offset = XtOffsetOf(php_io_poll_watcher_object, std);
	php_io_poll_watcher_object_handlers.free_obj = php_io_poll_watcher_free_object;
	php_io_poll_watcher_class_entry->default_object_handlers = &php_io_poll_watcher_object_handlers;

	/* Register Context class */
	php_io_poll_context_class_entry = register_class_Io_Poll_Context();
	php_io_poll_context_class_entry->create_object = php_io_poll_context_create_object;

	memcpy(&php_io_poll_context_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_poll_context_object_handlers.offset = XtOffsetOf(php_io_poll_context_object, std);
	php_io_poll_context_object_handlers.free_obj = php_io_poll_context_free_object;
	php_io_poll_context_class_entry->default_object_handlers = &php_io_poll_context_object_handlers;

	/* Register exception class */
	php_io_poll_exception_class_entry = register_class_Io_Poll_PollException(zend_ce_exception);

	/* Initialize polling backends */
	php_poll_register_backends();

	return SUCCESS;
}
