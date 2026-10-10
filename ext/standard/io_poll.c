/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Jakub Zelenka <bukka@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "SAPI.h"
#include "php_network.h"
#include "php_poll.h"
#include "io_poll.h"
#include "io_poll_arginfo.h"
#include "io_poll_decl.h"
#include "ext/date/time_duration.h"
#include "zend_interfaces.h"

#include "php_io.h"
#include "ext/standard/proc_open.h"

#include <fcntl.h>
#include <signal.h>
#ifdef PHP_WIN32
# include "win32/sockets.h"
# include "win32/winutil.h"
#endif
#ifndef PHP_WIN32
# include <sys/wait.h>
#endif
#ifdef __linux__
# include <sys/eventfd.h>
#endif

/* Class entries */
static zend_class_entry *php_io_poll_backend_class_entry;
zend_class_entry *php_io_poll_event_class_entry;
static zend_class_entry *php_io_poll_context_class_entry;
static zend_class_entry *php_io_poll_watcher_class_entry;
PHPAPI zend_class_entry *php_io_poll_handle_class_entry;
PHPAPI zend_class_entry *php_io_poll_weak_handle_class_entry;
static zend_class_entry *php_io_poll_timer_handle_class_entry;
static zend_class_entry *php_io_poll_notify_handle_class_entry;
static zend_class_entry *php_io_poll_process_handle_class_entry;
static zend_class_entry *php_io_poll_signal_handle_class_entry;
PHPAPI zend_class_entry *php_io_exception_class_entry;
static zend_class_entry *php_io_poll_exception_class_entry;
static zend_class_entry *php_io_poll_failed_backend_unavailable_class_entry;
static zend_class_entry *php_io_poll_failed_operation_class_entry;
static zend_class_entry *php_io_poll_failed_context_init_class_entry;
static zend_class_entry *php_io_poll_failed_handle_add_class_entry;
static zend_class_entry *php_io_poll_failed_watcher_mod_class_entry;
static zend_class_entry *php_io_poll_failed_wait_class_entry;
static zend_class_entry *php_io_poll_inactive_watcher_class_entry;
static zend_class_entry *php_io_poll_handle_already_watched_class_entry;
static zend_class_entry *php_io_poll_invalid_handle_class_entry;
PHPAPI zend_class_entry *php_stream_poll_handle_class_entry;

/* Object handlers */
static zend_object_handlers php_io_poll_context_object_handlers;
static zend_object_handlers php_io_poll_watcher_object_handlers;
static zend_object_handlers php_io_poll_handle_object_handlers;

typedef struct php_io_poll_context_object php_io_poll_context_object;

PHPAPI void php_io_poll_handle_remove_from_all_contexts(zend_object *handle_obj);

/* Watcher object structure */
typedef struct php_io_poll_watcher_object {
	php_poll_handle_object *handle;
	uint32_t watched_events;
	uint32_t triggered_events;
	zval data;
	bool active;
	bool closed; /* Deactivated because its stream was closed */
	php_io_poll_context_object *context; /* Back reference to Context object */
	php_socket_t fd; /* Registered fd, SOCK_ERR when inactive */
	php_stream *stream; /* Watched stream, NULL when not registered in its watcher list */
	php_poll_timer *timer; /* TimerHandle watchers only */
	zend_object std;
} php_io_poll_watcher_object;

/* Context object structure */
struct php_io_poll_context_object {
	php_poll_ctx *ctx;
	HashTable *watchers; /* Maps fd -> watcher object */
	HashTable *timer_watchers; /* Maps watcher pointer key -> watcher object */
	HashTable *removed; /* Watchers awaiting onWatcherRemoved() */
	zend_fcall_info_cache on_watcher_removed_fcc;
	zend_object std;
};

/* Stream poll handle specific data */
typedef struct php_stream_poll_handle_data {
	zend_resource *res;
} php_stream_poll_handle_data;

/* Accessor macros */
#define PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(_obj) ZEND_CONTAINER_OF(_obj, php_io_poll_context_object, std)

#define PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(_obj) ZEND_CONTAINER_OF(_obj, php_io_poll_watcher_object, std)

#define PHP_POLL_WATCHER_OBJ_FROM_ZV(_zv) PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))
#define PHP_POLL_CONTEXT_OBJ_FROM_ZV(_zv) PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(Z_OBJ_P(_zv))

/* Helper to throw failed operation exceptions with error code */
static inline void php_io_poll_throw_failed_operation(
		zend_class_entry *ce, const char *message, php_poll_error error)
{
	zend_throw_exception(ce, message, (zend_long) error);
}

PHPAPI void php_io_poll_throw_failed_wait(const char *message, php_poll_error error)
{
	php_io_poll_throw_failed_operation(php_io_poll_failed_wait_class_entry, message, error);
}

/* Event enum to bit mask mapping */
static uint32_t php_io_poll_event_enum_to_bit(zend_object *event_enum)
{
	return 1 << (zend_enum_fetch_case_id(event_enum) - 1);
}

PHPAPI uint32_t php_io_poll_event_enums_to_events(zval *event_enums)
{
	HashTable *ht;
	uint32_t events = 0;

	if (Z_TYPE_P(event_enums) != IS_ARRAY) {
		return 0;
	}

	ht = Z_ARRVAL_P(event_enums);

	ZEND_HASH_FOREACH_VAL(ht, zval *entry) {
		if (Z_TYPE_P(entry) != IS_OBJECT
				|| !instanceof_function(Z_OBJCE_P(entry), php_io_poll_event_class_entry)) {
			return 0;
		}
		events |= php_io_poll_event_enum_to_bit(Z_OBJ_P(entry));
	}
	ZEND_HASH_FOREACH_END();

	return events;
}

PHPAPI zend_result php_io_poll_events_to_event_enums(uint32_t events, zval *event_enums)
{
	zval enum_case;

	array_init(event_enums);

	if (events & PHP_POLL_READ) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Read));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_WRITE) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Write));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ERROR) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Error));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_HUP) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_HangUp));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_RDHUP) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_ReadHangUp));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ONESHOT) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_OneShot));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_ET) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_EdgeTriggered));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_PRI) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Priority));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_TIMER) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Timer));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_NOTIFY) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Notify));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_SIGNAL) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Signal));
		add_next_index_zval(event_enums, &enum_case);
	}
	if (events & PHP_POLL_PROCESS) {
		ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_by_id(php_io_poll_event_class_entry, ZEND_ENUM_Io_Poll_Event_Process));
		add_next_index_zval(event_enums, &enum_case);
	}

	return SUCCESS;
}

/* Backend enum name to backend type mapping */
static php_poll_backend_type php_io_poll_backend_enum_to_type(zend_object *backend_enum)
{
	return zend_enum_fetch_case_id(backend_enum) - 2;
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

/* Stream Poll Handle Implementation */

/* The stream is resolved from the resource on every use: fclose() frees the stream
 * while the resource, which the handle holds a reference to, stays as a closed one. */
static php_stream *php_stream_poll_handle_get_stream(php_poll_handle_object *handle)
{
	php_stream_poll_handle_data *data = handle->handle_data;

	if (!data) {
		return NULL;
	}

	return zend_fetch_resource2(data->res, NULL, php_file_le_stream(), php_file_le_pstream());
}

static php_socket_t php_stream_poll_handle_get_fd(php_poll_handle_object *handle)
{
	php_stream *stream = php_stream_poll_handle_get_stream(handle);
	php_socket_t fd;

	if (!stream) {
		return SOCK_ERR;
	}

	if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_POLL | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd, 1)
					!= SUCCESS
			|| fd == -1) {
		return SOCK_ERR;
	}

	return fd;
}

static int php_stream_poll_handle_is_valid(php_poll_handle_object *handle)
{
	php_stream *stream = php_stream_poll_handle_get_stream(handle);
	return stream && !php_stream_eof(stream);
}

static void php_stream_poll_handle_cleanup(php_poll_handle_object *handle)
{
	php_stream_poll_handle_data *data = handle->handle_data;
	if (data) {
		if (data->res) {
			zend_list_delete(data->res);
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops php_stream_poll_handle_ops = {
	.get_fd = php_stream_poll_handle_get_fd,
	.is_valid = php_stream_poll_handle_is_valid,
	.cleanup = php_stream_poll_handle_cleanup
};

PHPAPI void php_stream_poll_handle_from_stream(zval *dest, php_stream *stream)
{
	object_init_ex(dest, php_stream_poll_handle_class_entry);

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(dest);
	php_stream_poll_handle_data *data = emalloc(sizeof(php_stream_poll_handle_data));
	data->res = stream->res;
	intern->handle_data = data;

	GC_ADDREF(stream->res);
}

/* StreamPollWeakHandle: like StreamPollHandle but does not hold a refcount on the stream.
 * The stream holds the one reference the core needs and releases it in php_stream_free(),
 * after the stream pointer is zeroed and the handle is removed from every Context
 * (php_stream_poll_weak_handle_notify); userland references keep it as an invalid handle. */

static zend_class_entry *php_stream_poll_weak_handle_class_entry;

typedef struct php_stream_poll_weak_handle_data {
	php_stream *stream; /* Not referenced, NULL once closed */
} php_stream_poll_weak_handle_data;

static php_socket_t php_stream_poll_weak_handle_get_fd(php_poll_handle_object *handle)
{
	php_stream_poll_weak_handle_data *data = handle->handle_data;
	if (!data || !data->stream) {
		return SOCK_ERR;
	}
	php_socket_t fd;
	if (php_stream_cast(data->stream, PHP_STREAM_AS_FD_FOR_POLL | PHP_STREAM_CAST_INTERNAL,
				(void *) &fd, 1) != SUCCESS || fd == -1) {
		return SOCK_ERR;
	}
	return fd;
}

/* No liveness check: that is IO, and the handle is asked from inside the stream's operations */
static int php_stream_poll_weak_handle_is_valid(php_poll_handle_object *handle)
{
	php_stream_poll_weak_handle_data *data = handle->handle_data;
	return data && data->stream && !data->stream->eof;
}

static void php_stream_poll_weak_handle_cleanup(php_poll_handle_object *handle)
{
	php_stream_poll_weak_handle_data *data = handle->handle_data;
	if (data) {
		if (data->stream) {
			data->stream->weak_poll_handle = NULL;
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

/* Called from php_stream_free() while the fd is still open */
PHPAPI void php_stream_poll_weak_handle_notify(zend_object *handle_obj)
{
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle_obj);
	php_stream_poll_weak_handle_data *data = handle->handle_data;
	data->stream = NULL;
	if (handle->watching) {
		php_io_poll_handle_remove_from_all_contexts(handle_obj);
	}
}

static php_poll_handle_ops php_stream_poll_weak_handle_ops = {
	.get_fd   = php_stream_poll_weak_handle_get_fd,
	.is_valid = php_stream_poll_weak_handle_is_valid,
	.cleanup  = php_stream_poll_weak_handle_cleanup,
};

static zend_object *php_stream_poll_weak_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_stream_poll_weak_handle_ops);
	intern->std.handlers = &php_io_poll_handle_object_handlers;
	return &intern->std;
}

PHPAPI zend_object *php_stream_get_poll_handle(php_stream *stream, bool expose)
{
	php_poll_handle_object *intern;

	if (stream->weak_poll_handle) {
		intern = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(stream->weak_poll_handle);
	} else {
		zval handle_zv;
		object_init_ex(&handle_zv, php_stream_poll_weak_handle_class_entry);
		intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(&handle_zv);

		php_stream_poll_weak_handle_data *data = emalloc(sizeof(php_stream_poll_weak_handle_data));
		data->stream = stream;
		intern->handle_data = data;

		/* The reference is the stream's */
		stream->weak_poll_handle = Z_OBJ(handle_zv);
	}
	if (expose) {
		intern->flags |= PHP_POLL_HANDLE_F_EXPOSED;
	}
	return &intern->std;
}

PHP_METHOD(StreamPollWeakHandle, __construct)
{
	zend_throw_error(NULL, "Direct instantiation of %pS is not allowed, use %pS::create instead",
			Z_OBJCE_P(ZEND_THIS)->name, Z_OBJCE_P(ZEND_THIS)->name);
}

PHP_METHOD(StreamPollWeakHandle, create)
{
	zval *stream_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(stream_zv)
	ZEND_PARSE_PARAMETERS_END();

	php_stream *stream = (php_stream *) zend_fetch_resource2(
		Z_RES_P(stream_zv), "stream", php_file_le_stream(), php_file_le_pstream());
	if (!stream) {
		RETURN_THROWS();
	}

	RETURN_OBJ_COPY(php_stream_get_poll_handle(stream, true));
}

/* Not for an internal stream (never handed to a script) or a frozen one (in an operation) */
PHP_METHOD(StreamPollWeakHandle, getStream)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	php_stream_poll_weak_handle_data *data = intern->handle_data;

	if (!data || !data->stream || !(intern->flags & PHP_POLL_HANDLE_F_EXPOSED)
			|| (data->stream->flags & PHP_STREAM_FLAG_IN_USE)) {
		RETURN_NULL();
	}

	GC_ADDREF(data->stream->res);
	php_stream_to_zval(data->stream, return_value);
}

PHP_METHOD(StreamPollWeakHandle, isExposed)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL(intern->flags & PHP_POLL_HANDLE_F_EXPOSED);
}

PHP_METHOD(StreamPollWeakHandle, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL(intern->ops->is_valid(intern));
}

/* TimerHandle: a deadline in the context, no descriptor anywhere */

typedef struct php_io_poll_timer_handle_data {
	zend_hrtime_t timeout;
	bool periodic;
} php_io_poll_timer_handle_data;

static php_socket_t php_io_poll_timer_handle_get_fd(php_poll_handle_object *handle)
{
	return SOCK_ERR;
}

static int php_io_poll_timer_handle_is_valid(php_poll_handle_object *handle)
{
	return handle->handle_data != NULL;
}

static void php_io_poll_timer_handle_cleanup(php_poll_handle_object *handle)
{
	if (handle->handle_data) {
		efree(handle->handle_data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops php_io_poll_timer_handle_ops = {
	.get_fd   = php_io_poll_timer_handle_get_fd,
	.is_valid = php_io_poll_timer_handle_is_valid,
	.cleanup  = php_io_poll_timer_handle_cleanup,
};

static zend_object *php_io_poll_timer_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_io_poll_timer_handle_ops);
	intern->std.handlers = &php_io_poll_handle_object_handlers;
	return &intern->std;
}

static void php_io_poll_timer_handle_init(php_poll_handle_object *handle, zend_hrtime_t timeout, bool periodic)
{
	php_io_poll_timer_handle_data *data = emalloc(sizeof(*data));
	data->timeout = timeout;
	data->periodic = periodic;
	handle->handle_data = data;
}

PHPAPI void php_io_poll_timer_handle_create(zval *dest, zend_hrtime_t timeout_ns, bool periodic)
{
	object_init_ex(dest, php_io_poll_timer_handle_class_entry);
	php_io_poll_timer_handle_init(PHP_POLL_HANDLE_OBJ_FROM_ZV(dest), timeout_ns, periodic);
}

static bool php_io_poll_duration_to_ns(php_date_time_duration *d, uint32_t arg_num, zend_hrtime_t *ns)
{
	if (d->duration.negative) {
		zend_argument_value_error(arg_num, "must not be negative");
		return false;
	}
	if (d->duration.seconds >= ZEND_HRTIME_T_MAX / ZEND_NANO_IN_SEC) {
		zend_argument_value_error(arg_num, "is too large");
		return false;
	}
	*ns = (zend_hrtime_t) d->duration.seconds * ZEND_NANO_IN_SEC + d->duration.nanoseconds;
	return true;
}

static void php_io_poll_ns_to_duration(zval *rv, zend_hrtime_t ns)
{
	zval arg;
	ZVAL_LONG(&arg, (zend_long) MIN(ns, (zend_hrtime_t) ZEND_LONG_MAX));
	zend_call_method_with_1_params(NULL, php_date_ce_time_duration, NULL, "fromnanoseconds", rv, &arg);
}

PHP_METHOD(Io_Poll_TimerHandle, __construct)
{
	php_date_time_duration *timeout;
	bool periodic = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_DATE_TIME_DURATION(timeout)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(periodic)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}

	zend_hrtime_t ns;
	if (!php_io_poll_duration_to_ns(timeout, 1, &ns)) {
		RETURN_THROWS();
	}
	if (periodic && ns == 0) {
		zend_argument_value_error(1, "must not be zero for a periodic timer");
		RETURN_THROWS();
	}
	php_io_poll_timer_handle_init(intern, ns, periodic);
}

PHP_METHOD(Io_Poll_TimerHandle, getTimeout)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_timer_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	php_io_poll_ns_to_duration(return_value, data->timeout);
}

PHP_METHOD(Io_Poll_TimerHandle, isPeriodic)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_timer_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	RETURN_BOOL(data->periodic);
}


/* Handles that stand for something other than descriptor readiness report
 * one event of their own and are watched as READ on their descriptor */
static const char *php_io_poll_virtual_event_name(uint32_t event)
{
	switch (event) {
		case PHP_POLL_NOTIFY: return "Event::Notify for a NotifyHandle";
		case PHP_POLL_SIGNAL: return "Event::Signal for a SignalHandle";
		case PHP_POLL_PROCESS: return "Event::Process for a ProcessHandle";
		default: return "";
	}
}

#define PHP_IO_POLL_VIRTUAL_EVENTS (PHP_POLL_TIMER | PHP_POLL_NOTIFY | PHP_POLL_SIGNAL | PHP_POLL_PROCESS)

/* Maps the requested events of a handle to what the backend watches; a
 * ValueError on the given argument when they do not fit the handle */
static zend_result php_io_poll_handle_backend_events(php_poll_handle_object *handle,
		uint32_t events, uint32_t arg_num, uint32_t *backend_events)
{
	uint32_t virtual = handle->ops->event;
	if (virtual) {
		if ((events & ~(virtual | PHP_POLL_ONESHOT)) || !(events & virtual)) {
			zend_argument_value_error(arg_num, "must be %s", php_io_poll_virtual_event_name(virtual));
			return FAILURE;
		}
		*backend_events = (events & ~virtual) | PHP_POLL_READ;
	} else if (events & PHP_IO_POLL_VIRTUAL_EVENTS) {
		zend_argument_value_error(arg_num,
				"must not contain Event::Timer, Event::Notify, Event::Signal or Event::Process for this handle");
		return FAILURE;
	} else {
		*backend_events = events;
	}
	return SUCCESS;
}

/* The descriptor of a virtual handle fired: its event only when the
 * source had something to take */
static uint32_t php_io_poll_handle_fired(php_poll_handle_object *handle, uint32_t revents)
{
	uint32_t virtual = handle->ops->event;
	if (!virtual) {
		return revents;
	}
	bool took = (revents & PHP_POLL_READ) && (!handle->ops->fired || handle->ops->fired(handle));
	return (revents & (PHP_POLL_ERROR | PHP_POLL_HUP)) | (took ? virtual : 0);
}

/* NotifyHandle: an eventfd, or a pipe where there is none. Level: readable
 * from notify() until clear(). */

#ifdef PHP_WIN32
/* A SOCKET of a loopback pair; INVALID_SOCKET is -1 as a signed value */
typedef intptr_t php_io_poll_notify_fd;
# define PHP_IO_POLL_NOTIFY_CLOSE(fd) closesocket((php_socket_t) (fd))
#else
typedef int php_io_poll_notify_fd;
# define PHP_IO_POLL_NOTIFY_CLOSE(fd) close(fd)
#endif

typedef struct php_io_poll_notify_handle_data {
	php_io_poll_notify_fd read_fd;
	php_io_poll_notify_fd write_fd; /* -1 when external */
	bool owned;
	/* External descriptors only */
	void (*clear)(void *arg);
	void *clear_arg;
	zend_object *owner;
} php_io_poll_notify_handle_data;

static php_socket_t php_io_poll_notify_handle_get_fd(php_poll_handle_object *handle)
{
	php_io_poll_notify_handle_data *data = handle->handle_data;
	return data ? (php_socket_t) data->read_fd : SOCK_ERR;
}

static int php_io_poll_notify_handle_is_valid(php_poll_handle_object *handle)
{
	php_io_poll_notify_handle_data *data = handle->handle_data;
	return data && data->read_fd >= 0;
}

static void php_io_poll_notify_handle_cleanup(php_poll_handle_object *handle)
{
	php_io_poll_notify_handle_data *data = handle->handle_data;
	if (data) {
		if (data->owned) {
			if (data->write_fd >= 0 && data->write_fd != data->read_fd) {
				PHP_IO_POLL_NOTIFY_CLOSE(data->write_fd);
			}
			if (data->read_fd >= 0) {
				PHP_IO_POLL_NOTIFY_CLOSE(data->read_fd);
			}
		}
		if (data->owner) {
			OBJ_RELEASE(data->owner);
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops php_io_poll_notify_handle_ops = {
	.get_fd   = php_io_poll_notify_handle_get_fd,
	.is_valid = php_io_poll_notify_handle_is_valid,
	.cleanup  = php_io_poll_notify_handle_cleanup,
	.event    = PHP_POLL_NOTIFY,
};

static zend_object *php_io_poll_notify_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_io_poll_notify_handle_ops);
	intern->std.handlers = &php_io_poll_handle_object_handlers;
	return &intern->std;
}

static zend_result php_io_poll_notify_handle_open(php_io_poll_notify_handle_data *data)
{
#ifdef __linux__
	int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (fd < 0) {
		return FAILURE;
	}
	data->read_fd = data->write_fd = fd;
	return SUCCESS;
#elif defined(PHP_WIN32)
	SOCKET socks[2];
	if (socketpair(AF_INET, SOCK_STREAM, 0, socks) != 0) {
		return FAILURE;
	}
	u_long nonblock = 1;
	ioctlsocket(socks[0], FIONBIO, &nonblock);
	ioctlsocket(socks[1], FIONBIO, &nonblock);
	data->read_fd = (php_io_poll_notify_fd) socks[0];
	data->write_fd = (php_io_poll_notify_fd) socks[1];
	return SUCCESS;
#else
	int fds[2];
	if (pipe(fds) != 0) {
		return FAILURE;
	}
	for (int i = 0; i < 2; i++) {
		fcntl(fds[i], F_SETFL, fcntl(fds[i], F_GETFL) | O_NONBLOCK);
		fcntl(fds[i], F_SETFD, FD_CLOEXEC);
	}
	data->read_fd = fds[0];
	data->write_fd = fds[1];
	return SUCCESS;
#endif
}

PHPAPI void php_poll_notify(zend_object *handle_obj)
{
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle_obj);
	php_io_poll_notify_handle_data *data = handle->handle_data;
	if (!data || data->write_fd < 0) {
		return;
	}
#ifdef __linux__
	uint64_t one = 1;
	ssize_t n = write(data->write_fd, &one, sizeof(one));
#elif defined(PHP_WIN32)
	char one = 1;
	int n = send((php_socket_t) data->write_fd, &one, sizeof(one), 0);
#else
	char one = 1;
	ssize_t n = write(data->write_fd, &one, sizeof(one));
#endif
	/* A full counter or pipe is still readable, so nothing is lost */
	(void) n;
}


/* ProcessHandle: the platform's process source (php_poll_process_source_open)
 * where there is one. When it fires the exit status is read and recorded;
 * the child stays waitable for whoever collects it. */

typedef struct php_io_poll_process_handle_data {
	pid_t pid;
	int fd;
	bool has_status;
	bool exited; /* with a status, or collected elsewhere */
	int status;
#ifdef PHP_WIN32
	HANDLE process; /* the process object: keeps the pid valid and answers the exit code */
#endif
} php_io_poll_process_handle_data;

#ifdef PHP_WIN32
/* The process object answers at any time whether the process exited, and its exit code */
static void php_io_poll_process_handle_probe(php_io_poll_process_handle_data *data)
{
	if (data->exited || !data->process) {
		return;
	}
	if (WaitForSingleObject(data->process, 0) == WAIT_OBJECT_0) {
		DWORD code = 0;
		if (GetExitCodeProcess(data->process, &code)) {
			data->status = (int) code;
			data->has_status = true;
		}
		data->exited = true;
	}
}
#endif

static php_socket_t php_io_poll_process_handle_get_fd(php_poll_handle_object *handle)
{
	php_io_poll_process_handle_data *data = handle->handle_data;
	return data ? (php_socket_t) data->fd : SOCK_ERR;
}

static int php_io_poll_process_handle_is_valid(php_poll_handle_object *handle)
{
	return handle->handle_data != NULL;
}

static void php_io_poll_process_handle_cleanup(php_poll_handle_object *handle)
{
	php_io_poll_process_handle_data *data = handle->handle_data;
	if (data) {
#ifdef PHP_WIN32
		if (data->process) {
			CloseHandle(data->process);
		}
#endif
		if (data->fd >= 0) {
			close(data->fd);
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

/* The exit is state: every context watching the handle reports it once */
static bool php_io_poll_process_handle_fired(php_poll_handle_object *handle)
{
	php_io_poll_process_handle_data *data = handle->handle_data;
	if (!data) {
		return false;
	}
#ifdef PHP_WIN32
	php_io_poll_process_handle_probe(data);
#else
	if (!data->exited) {
		int status;
		pid_t pid = php_poll_process_exit_probe(data->pid, data->fd, &status);
		if (pid == data->pid) {
			data->has_status = true;
			data->exited = true;
			data->status = status;
		} else if (pid == -1 && errno == ECHILD) {
			/* Not our child, or collected by someone else: gone all the same */
			data->exited = true;
		}
	}
#endif
	return data->exited;
}

static php_poll_handle_ops php_io_poll_process_handle_ops = {
	.get_fd   = php_io_poll_process_handle_get_fd,
	.is_valid = php_io_poll_process_handle_is_valid,
	.cleanup  = php_io_poll_process_handle_cleanup,
	.event    = PHP_POLL_PROCESS,
	.fired    = php_io_poll_process_handle_fired,
};

/* A process handle whose process is gone: its descriptor stays readable */
static bool php_io_poll_handle_exhausted(php_poll_handle_object *handle)
{
	if (handle->ops != &php_io_poll_process_handle_ops) {
		return false;
	}
	php_io_poll_process_handle_data *data = handle->handle_data;
	return data && data->exited;
}

static zend_object *php_io_poll_process_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_io_poll_process_handle_ops);
	intern->std.handlers = &php_io_poll_handle_object_handlers;
	return &intern->std;
}

static zend_result php_io_poll_process_handle_init(php_poll_handle_object *handle, pid_t pid, uint32_t arg_num)
{
	int fd = -1;
#ifdef PHP_WIN32
	HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD) pid);
	if (!process) {
		if (arg_num) {
			char *msg = php_win32_error_to_msg(GetLastError());
			zend_argument_value_error(arg_num, "must be the id of a running process: %s", msg);
			php_win32_error_msg_free(msg);
		}
		return FAILURE;
	}
#else
	fd = php_poll_process_source_open(pid);
	if (fd < 0 && errno != ENOSYS) {
		if (arg_num) {
			zend_argument_value_error(arg_num, "must be the id of a running process: %s", strerror(errno));
		}
		return FAILURE;
	}
#endif
	php_io_poll_process_handle_data *data = ecalloc(1, sizeof(*data));
	data->pid = pid;
	data->fd = fd;
#ifdef PHP_WIN32
	data->process = process;
#endif
	handle->handle_data = data;
	return SUCCESS;
}

PHPAPI void php_io_poll_process_handle_create(zval *dest, pid_t pid)
{
	object_init_ex(dest, php_io_poll_process_handle_class_entry);
	if (php_io_poll_process_handle_init(PHP_POLL_HANDLE_OBJ_FROM_ZV(dest), pid, 0) == FAILURE) {
		/* Not watchable, but still identity for the provider */
		php_io_poll_process_handle_data *data = ecalloc(1, sizeof(*data));
		data->pid = pid;
		data->fd = -1;
		PHP_POLL_HANDLE_OBJ_FROM_ZV(dest)->handle_data = data;
	}
}

PHP_METHOD(Io_Poll_ProcessHandle, __construct)
{
	zend_long pid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pid)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}
	if (pid <= 0) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}
	if (pid > INT_MAX) {
		zend_argument_value_error(1, "must be less than or equal to %d", INT_MAX);
		RETURN_THROWS();
	}
	if (php_io_poll_process_handle_init(intern, (pid_t) pid, 1) == FAILURE) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_ProcessHandle, fromProcess)
{
	zval *zproc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zproc)
	ZEND_PARSE_PARAMETERS_END();

	php_process_id_t pid;
	if (!php_proc_open_get_pid(zproc, &pid)) {
		RETURN_THROWS();
	}
	object_init_ex(return_value, php_io_poll_process_handle_class_entry);
	if (php_io_poll_process_handle_init(PHP_POLL_HANDLE_OBJ_FROM_ZV(return_value), (pid_t) pid, 1) == FAILURE) {
		/* The engine releases the result of a throwing call */
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_ProcessHandle, getPid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_process_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	RETURN_LONG((zend_long) data->pid);
}

PHP_METHOD(Io_Poll_ProcessHandle, getStatus)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_process_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
#ifdef PHP_WIN32
	php_io_poll_process_handle_probe(data);
#endif
	if (!data->has_status) {
		RETURN_NULL();
	}
	RETURN_LONG(data->status);
}

/* SignalHandle: the platform's signal source (php_poll_signal_source_open)
 * over the set where there is one. The signals are blocked while a context
 * watches the handle; when the source fires every pending delivery is taken
 * and its info recorded until an op or getDelivered() takes it. */

typedef struct php_io_poll_signal_handle_data {
	php_sigset_t set;
	int fd;
	uint32_t registrations; /* contexts watching the handle */
	php_siginfo_t *infos;
	uint32_t n_infos;
	uint32_t cap_infos;
} php_io_poll_signal_handle_data;

#ifndef PHP_WIN32
# ifdef ZTS
#  define php_io_poll_sigmask pthread_sigmask
# else
#  define php_io_poll_sigmask sigprocmask
# endif

/* Watched handles per signal, and the signals the handles blocked themselves:
 * a signal is unblocked again when the last watched handle for it is removed,
 * and never when the process had it blocked before any handle */
ZEND_TLS uint32_t php_io_poll_signal_handle_count[NSIG];
ZEND_TLS sigset_t php_io_poll_signals_blocked_by_handles;

PHPAPI void php_io_poll_signal_child_mask(sigset_t *mask)
{
	php_io_poll_sigmask(SIG_BLOCK, NULL, mask);
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&php_io_poll_signals_blocked_by_handles, signo) == 1) {
			sigdelset(mask, signo);
		}
	}
}

PHPAPI void php_io_poll_signal_watched_mask(sigset_t *set)
{
	sigemptyset(set);
	for (int signo = 1; signo < NSIG; signo++) {
		if (php_io_poll_signal_handle_count[signo]) {
			sigaddset(set, signo);
		}
	}
}
#endif

static php_socket_t php_io_poll_signal_handle_get_fd(php_poll_handle_object *handle)
{
	php_io_poll_signal_handle_data *data = handle->handle_data;
	return data ? (php_socket_t) data->fd : SOCK_ERR;
}

static int php_io_poll_signal_handle_is_valid(php_poll_handle_object *handle)
{
	return handle->handle_data != NULL;
}

static void php_io_poll_signal_handle_record(php_io_poll_signal_handle_data *data, const php_siginfo_t *info)
{
	if (data->n_infos == data->cap_infos) {
		data->cap_infos = data->cap_infos ? data->cap_infos * 2 : 4;
		data->infos = safe_erealloc(data->infos, data->cap_infos, sizeof(*data->infos), 0);
	}
	data->infos[data->n_infos++] = *info;
}

static void php_io_poll_signal_handle_block(php_io_poll_signal_handle_data *data)
{
#ifndef PHP_WIN32
	/* Block what is not blocked yet, so the signals queue for the source */
	sigset_t old;
	if (php_io_poll_sigmask(SIG_BLOCK, &data->set, &old) != 0) {
		return;
	}
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&data->set, signo) != 1) {
			continue;
		}
		if (php_io_poll_signal_handle_count[signo]++ == 0 && sigismember(&old, signo) == 0) {
			sigaddset(&php_io_poll_signals_blocked_by_handles, signo);
		}
	}
#endif
}

/* What arrived while the handle was watched is taken into its record before
 * the unblock, so nothing is delivered with its default action later */
static void php_io_poll_signal_handle_unblock(php_io_poll_signal_handle_data *data)
{
#ifndef PHP_WIN32
	sigset_t unblock;
	bool any = false;
	sigemptyset(&unblock);
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&data->set, signo) != 1 || php_io_poll_signal_handle_count[signo] == 0) {
			continue;
		}
		if (--php_io_poll_signal_handle_count[signo] == 0
				&& sigismember(&php_io_poll_signals_blocked_by_handles, signo) == 1) {
			sigdelset(&php_io_poll_signals_blocked_by_handles, signo);
			sigaddset(&unblock, signo);
			any = true;
		}
	}
	if (any) {
		siginfo_t info;
		while (php_poll_signal_take_pending(&unblock, &info) > 0) {
			php_io_poll_signal_handle_record(data, &info);
		}
		php_io_poll_sigmask(SIG_UNBLOCK, &unblock, NULL);
	}
#endif
}

static void php_io_poll_signal_handle_added(php_poll_handle_object *handle)
{
	php_io_poll_signal_handle_data *data = handle->handle_data;
	if (data && data->registrations++ == 0) {
		php_io_poll_signal_handle_block(data);
	}
}

static void php_io_poll_signal_handle_removed(php_poll_handle_object *handle)
{
	php_io_poll_signal_handle_data *data = handle->handle_data;
	if (data && data->registrations > 0 && --data->registrations == 0) {
		php_io_poll_signal_handle_unblock(data);
	}
}

static void php_io_poll_signal_handle_cleanup(php_poll_handle_object *handle)
{
	php_io_poll_signal_handle_data *data = handle->handle_data;
	if (data) {
		/* Freed by the cycle collector before a context that still watches it */
		if (data->registrations > 0) {
			data->registrations = 0;
			php_io_poll_signal_handle_unblock(data);
		}
		if (data->fd >= 0) {
			close(data->fd);
		}
		if (data->infos) {
			efree(data->infos);
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

static bool php_io_poll_signal_handle_fired(php_poll_handle_object *handle)
{
	php_io_poll_signal_handle_data *data = handle->handle_data;
	bool took = false;
	if (!data || data->fd < 0) {
		return false;
	}
#ifndef PHP_WIN32
	siginfo_t info;
	while (php_poll_signal_source_take(data->fd, &data->set, &info) > 0) {
		php_io_poll_signal_handle_record(data, &info);
		took = true;
	}
#endif
	return took;
}

static php_poll_handle_ops php_io_poll_signal_handle_ops = {
	.get_fd   = php_io_poll_signal_handle_get_fd,
	.is_valid = php_io_poll_signal_handle_is_valid,
	.cleanup  = php_io_poll_signal_handle_cleanup,
	.event    = PHP_POLL_SIGNAL,
	.fired    = php_io_poll_signal_handle_fired,
	.added    = php_io_poll_signal_handle_added,
	.removed  = php_io_poll_signal_handle_removed,
};

static zend_object *php_io_poll_signal_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &php_io_poll_signal_handle_ops);
	intern->std.handlers = &php_io_poll_handle_object_handlers;
	return &intern->std;
}

static void php_io_poll_signal_handle_init(php_poll_handle_object *handle, const php_sigset_t *set)
{
	php_io_poll_signal_handle_data *data = ecalloc(1, sizeof(*data));
	data->set = *set;
	data->fd = -1;
#ifndef PHP_WIN32
	/* The source sees only blocked signals, so it is inert until a context
	 * watches the handle. No source (ENOSYS, or the platform could not open
	 * one): the handle is still identity for a provider, and Context::add()
	 * refuses it */
	data->fd = php_poll_signal_source_open(set);
#endif
	handle->handle_data = data;
}

PHPAPI void php_io_poll_signal_handle_create(zval *dest, const php_sigset_t *set)
{
	object_init_ex(dest, php_io_poll_signal_handle_class_entry);
	php_io_poll_signal_handle_init(PHP_POLL_HANDLE_OBJ_FROM_ZV(dest), set);
}

PHPAPI int php_io_poll_signal_handle_take(zend_object *handle_obj, const php_sigset_t *set, php_siginfo_t *info)
{
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle_obj);
	if (handle->ops != &php_io_poll_signal_handle_ops) {
		return 0;
	}
	php_io_poll_signal_handle_data *data = handle->handle_data;
	if (!data) {
		return 0;
	}
	for (uint32_t i = 0; i < data->n_infos; i++) {
		int signo = data->infos[i].si_signo;
		if (php_sigismember(set, signo) == 1) {
			if (info) {
				*info = data->infos[i];
			}
			memmove(&data->infos[i], &data->infos[i + 1], (data->n_infos - i - 1) * sizeof(*data->infos));
			data->n_infos--;
			return signo;
		}
	}
	return 0;
}

/* SIGKILL and SIGSTOP cannot be blocked, and blocking a signal raised by a
 * fault is undefined behaviour */
static bool php_io_poll_signal_unwatchable(zend_long signo)
{
	switch (signo) {
#ifdef SIGKILL
		case SIGKILL:
#endif
#ifdef SIGSTOP
		case SIGSTOP:
#endif
#ifdef SIGBUS
		case SIGBUS:
#endif
		case SIGSEGV:
		case SIGFPE:
		case SIGILL:
			return true;
		default:
			return false;
	}
}

/* max_execution_time runs on this signal; blocking it disables the timeout */
static bool php_io_poll_signal_is_engine_timeout(zend_long signo)
{
#if defined(ZEND_MAX_EXECUTION_TIMERS)
	return signo == SIGRTMIN;
#elif defined(PHP_WIN32)
	return false;
#elif defined(__CYGWIN__) || defined(__PASE__) || (defined(__aarch64__) && defined(__APPLE__))
	return signo == SIGALRM;
#else
	return signo == SIGPROF;
#endif
}

PHP_METHOD(Io_Poll_SignalHandle, __construct)
{
	HashTable *signals;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(signals)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}
#ifdef ZTS
	/* The mask is per thread: in a multi-threaded SAPI another thread would
	 * take the signal with its default action and the handle would never
	 * see it. The CLI runs one PHP thread. */
	if (strcmp(sapi_module.name, "cli") != 0) {
		zend_throw_exception(php_io_poll_exception_class_entry,
			"Io\\Poll\\SignalHandle is only available in the CLI in thread-safe builds", 0);
		RETURN_THROWS();
	}
#endif
	if (zend_hash_num_elements(signals) == 0) {
		zend_argument_must_not_be_empty_error(1);
		RETURN_THROWS();
	}

	php_sigset_t set;
	php_sigemptyset(&set);
	ZEND_HASH_FOREACH_VAL(signals, zval *entry) {
		ZVAL_DEREF(entry);
		if (Z_TYPE_P(entry) != IS_LONG) {
			zend_argument_type_error(1, "signals must be of type int, %s given", zend_zval_value_name(entry));
			RETURN_THROWS();
		}
		zend_long signo = Z_LVAL_P(entry);
		if (signo < 1 || signo >= PHP_NSIG) {
			zend_argument_value_error(1, "signals must be between 1 and %d", PHP_NSIG - 1);
			RETURN_THROWS();
		}
		if (php_io_poll_signal_unwatchable(signo)) {
			zend_argument_value_error(1, "must not contain signal " ZEND_LONG_FMT ", which cannot be blocked", signo);
			RETURN_THROWS();
		}
		if (php_io_poll_signal_is_engine_timeout(signo)) {
			zend_argument_value_error(1, "must not contain signal " ZEND_LONG_FMT ", which the execution timeout uses", signo);
			RETURN_THROWS();
		}
		if (php_sigaddset(&set, (int) signo) != 0) {
			zend_argument_value_error(1, "must not contain signal " ZEND_LONG_FMT ", which is reserved", signo);
			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();

	php_io_poll_signal_handle_init(intern, &set);
}

PHP_METHOD(Io_Poll_SignalHandle, getSignals)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_signal_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	array_init(return_value);
	for (int signo = 1; signo < PHP_NSIG; signo++) {
		if (php_sigismember(&data->set, signo) == 1) {
			add_next_index_long(return_value, signo);
		}
	}
}

PHP_METHOD(Io_Poll_SignalHandle, getDelivered)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_signal_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	array_init(return_value);
	for (uint32_t i = 0; i < data->n_infos; i++) {
		add_next_index_long(return_value, data->infos[i].si_signo);
	}
	data->n_infos = 0;
}

PHP_METHOD(Io_Poll_NotifyHandle, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}

	php_io_poll_notify_handle_data *data = ecalloc(1, sizeof(*data));
	data->read_fd = data->write_fd = -1;
	data->owned = true;
	if (php_io_poll_notify_handle_open(data) != SUCCESS) {
		efree(data);
		zend_throw_exception_ex(php_io_poll_failed_context_init_class_entry, PHP_POLL_ERR_SYSTEM,
				"Failed to create the notification descriptor: %s", strerror(errno));
		RETURN_THROWS();
	}
	intern->handle_data = data;
}

PHPAPI void php_io_poll_notify_handle_create_external(zval *dest, php_socket_t fd,
		void (*clear)(void *arg), void *arg, zend_object *owner)
{
	object_init_ex(dest, php_io_poll_notify_handle_class_entry);
	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(dest);
	php_io_poll_notify_handle_data *data = ecalloc(1, sizeof(*data));
	data->read_fd = (php_io_poll_notify_fd) fd;
	data->write_fd = -1;
	data->owned = false;
	data->clear = clear;
	data->clear_arg = arg;
	data->owner = owner;
	if (owner) {
		GC_ADDREF(owner);
	}
	intern->handle_data = data;
}

/* Handles hold no zvals; an external NotifyHandle holds its owner */
static HashTable *php_io_poll_handle_get_gc(zend_object *obj, zval **table, int *n)
{
	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (intern->ops == &php_io_poll_notify_handle_ops && intern->handle_data) {
		php_io_poll_notify_handle_data *data = intern->handle_data;
		if (data->owner) {
			zend_get_gc_buffer_add_obj(gc_buffer, data->owner);
		}
	}
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

PHP_METHOD(Io_Poll_NotifyHandle, notify)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	php_io_poll_notify_handle_data *data = intern->handle_data;
	if (!data->owned) {
		zend_throw_error(NULL, "This Io\\Poll\\NotifyHandle is raised by its owner and cannot be notified");
		RETURN_THROWS();
	}
	php_poll_notify(&intern->std);
}

PHP_METHOD(Io_Poll_NotifyHandle, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_notify_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	if (!data->owned) {
		data->clear(data->clear_arg);
		return;
	}
#ifdef __linux__
	uint64_t count;
	ssize_t n = read(data->read_fd, &count, sizeof(count));
	(void) n;
#elif defined(PHP_WIN32)
	char buf[64];
	while (recv((php_socket_t) data->read_fd, buf, sizeof(buf), 0) > 0) {
	}
#else
	char buf[64];
	while (read(data->read_fd, buf, sizeof(buf)) > 0) {
	}
#endif
}

/* Handle interface internal only */
static void php_stream_poll_handle_implement_interface(zend_class_entry *interface, zend_class_entry *implementor)
{
	if (implementor->type == ZEND_USER_CLASS) {
		zend_error_noreturn(E_ERROR, "Io\\Poll\\Handle cannot be implemented by user classes");
	}
}

/* Object Creation Functions */

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
	intern->closed = false;
	intern->context = NULL;
	intern->fd = SOCK_ERR;
	intern->stream = NULL;
	intern->timer = NULL;
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
	intern->timer_watchers = NULL;
	intern->removed = NULL;
	intern->on_watcher_removed_fcc = empty_fcall_info_cache;

	return &intern->std;
}

/* Watcher registration helpers */

static zend_always_inline zend_ulong php_io_poll_compute_ptr_key(void *ptr)
{
	zend_ulong key = (zend_ulong) (uintptr_t) ptr;
	return (key >> 3) | (key << ((sizeof(key) * 8) - 3));
}

static zend_always_inline void php_io_poll_watcher_deactivate(php_io_poll_watcher_object *watcher)
{
	watcher->active = false;
	watcher->context = NULL;
	watcher->fd = SOCK_ERR;
}

static void php_io_poll_stream_watch(php_stream *stream, php_io_poll_watcher_object *watcher)
{
	if (!stream->poll_watchers) {
		stream->poll_watchers = pemalloc(sizeof(HashTable), stream->is_persistent);
		zend_hash_init(stream->poll_watchers, 4, NULL, NULL, stream->is_persistent);
	}

	zval zv;
	ZVAL_PTR(&zv, watcher);
	zend_hash_index_add_new(stream->poll_watchers, php_io_poll_compute_ptr_key(watcher), &zv);
	watcher->stream = stream;
}

static void php_io_poll_stream_unwatch(php_io_poll_watcher_object *watcher)
{
	php_stream *stream = watcher->stream;

	if (!stream) {
		return;
	}
	watcher->stream = NULL;

	zend_hash_index_del(stream->poll_watchers, php_io_poll_compute_ptr_key(watcher));
	if (zend_hash_num_elements(stream->poll_watchers) == 0) {
		zend_hash_destroy(stream->poll_watchers);
		pefree(stream->poll_watchers, stream->is_persistent);
		stream->poll_watchers = NULL;
	}
}

static void php_io_poll_handle_unwatch(
		php_io_poll_context_object *context, php_io_poll_watcher_object *watcher)
{
	php_poll_handle_object *handle = watcher->handle;
	if (handle && handle->watching
			&& zend_hash_index_del(handle->watching, php_io_poll_compute_ptr_key(context)) == SUCCESS
			&& handle->ops->removed) {
		handle->ops->removed(handle);
	}
}

/* The single removal path: drops the backend registration, both reverse
 * lookups and the context's reference. notify queues a removal the caller
 * did not initiate for onWatcherRemoved(); no user code runs here. */
static void php_io_poll_context_retire_watcher(
		php_io_poll_context_object *context, php_io_poll_watcher_object *watcher, bool notify)
{
	php_socket_t fd = watcher->fd;
	bool is_timer = watcher->timer != NULL;

	if (is_timer) {
		php_poll_timer_remove(context->ctx, watcher->timer);
		watcher->timer = NULL;
	} else {
		php_poll_remove(context->ctx, (int) fd);
	}
	php_io_poll_stream_unwatch(watcher);
	php_io_poll_handle_unwatch(context, watcher);
	php_io_poll_watcher_deactivate(watcher);

	/* Keep the watcher alive across the registry removal and the callback */
	GC_ADDREF(&watcher->std);
	if (is_timer) {
		zend_hash_index_del(context->timer_watchers, php_io_poll_compute_ptr_key(watcher));
	} else {
		zend_hash_index_del(context->watchers, (zend_ulong) fd);
	}

	if (notify && ZEND_FCC_INITIALIZED(context->on_watcher_removed_fcc)) {
		if (!context->removed) {
			context->removed = emalloc(sizeof(HashTable));
			zend_hash_init(context->removed, 4, NULL, ZVAL_PTR_DTOR, 0);
		}
		zval watcher_zv;
		ZVAL_OBJ_COPY(&watcher_zv, &watcher->std);
		zend_hash_next_index_insert_new(context->removed, &watcher_zv);
	}

	OBJ_RELEASE(&watcher->std);
}

/* Runs the queued onWatcherRemoved() calls, stopping at the first exception
 * with the rest still queued */
static zend_result php_io_poll_context_deliver_removed(php_io_poll_context_object *context)
{
	while (context->removed && zend_hash_num_elements(context->removed)) {
		zend_ulong idx = 0;
		zval *first = NULL, watcher_zv;
		ZEND_HASH_FOREACH_NUM_KEY_VAL(context->removed, idx, first) {
			break;
		} ZEND_HASH_FOREACH_END();
		ZVAL_COPY(&watcher_zv, first);
		zend_hash_index_del(context->removed, idx);
		if (zend_hash_num_elements(context->removed) == 0) {
			zend_hash_clean(context->removed);
		}

		if (ZEND_FCC_INITIALIZED(context->on_watcher_removed_fcc)) {
			zend_call_known_fcc(&context->on_watcher_removed_fcc, NULL, 1, &watcher_zv, NULL);
		}
		zval_ptr_dtor(&watcher_zv);
		if (EG(exception)) {
			return FAILURE;
		}
	}
	return SUCCESS;
}

/* Retires every watcher of the list; they are kept alive until the loop is
 * done so that releasing one runs no user code in the middle of it */
static void php_io_poll_retire_watchers(php_io_poll_watcher_object **list, uint32_t n)
{
	for (uint32_t i = 0; i < n; i++) {
		php_io_poll_watcher_object *watcher = list[i];
		watcher->closed = true;
		if (watcher->active && watcher->context) {
			php_io_poll_context_retire_watcher(watcher->context, watcher, true);
		}
	}
	for (uint32_t i = 0; i < n; i++) {
		OBJ_RELEASE(&list[i]->std);
	}
	efree(list);
}

/* Called from php_stream_free() while the fd is still open */
PHPAPI void php_io_poll_stream_notify_close(php_stream *stream)
{
	HashTable *watchers = stream->poll_watchers;
	stream->poll_watchers = NULL;

	uint32_t n = 0;
	php_io_poll_watcher_object **list = safe_emalloc(zend_hash_num_elements(watchers), sizeof(*list), 0);
	ZEND_HASH_FOREACH_VAL(watchers, zval *zv) {
		php_io_poll_watcher_object *watcher = Z_PTR_P(zv);
		watcher->stream = NULL;
		GC_ADDREF(&watcher->std);
		list[n++] = watcher;
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(watchers);
	pefree(watchers, stream->is_persistent);

	php_io_poll_retire_watchers(list, n);
}

/* Object Destruction Functions */

static void php_io_poll_watcher_free_object(zend_object *obj)
{
	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(obj);

	php_io_poll_stream_unwatch(intern);
	zval_ptr_dtor(&intern->data);

	/* Freed by the cycle collector before its context */
	if (intern->active && intern->context) {
		php_io_poll_handle_unwatch(intern->context, intern);
	}

	if (intern->handle) {
		OBJ_RELEASE(&intern->handle->std);
		/* A context freed later in the same collection must not follow it */
		intern->handle = NULL;
	}

	zend_object_std_dtor(&intern->std);
}

static void php_io_poll_context_free_object(zend_object *obj)
{
	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(obj);

	if (intern->watchers) {
		ZEND_HASH_FOREACH_VAL(intern->watchers, zval *zv) {
			php_io_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(zv));
			php_io_poll_stream_unwatch(watcher);
			php_io_poll_handle_unwatch(intern, watcher);
			php_io_poll_watcher_deactivate(watcher);
		} ZEND_HASH_FOREACH_END();
	}

	if (intern->timer_watchers) {
		ZEND_HASH_FOREACH_VAL(intern->timer_watchers, zval *zv) {
			php_io_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(zv));
			php_poll_timer_remove(intern->ctx, watcher->timer);
			watcher->timer = NULL;
			php_io_poll_handle_unwatch(intern, watcher);
			php_io_poll_watcher_deactivate(watcher);
		} ZEND_HASH_FOREACH_END();
	}

	if (intern->ctx) {
		php_poll_destroy(intern->ctx);
	}

	if (intern->watchers) {
		zend_hash_destroy(intern->watchers);
		efree(intern->watchers);
	}
	if (intern->timer_watchers) {
		zend_hash_destroy(intern->timer_watchers);
		efree(intern->timer_watchers);
	}

	if (intern->removed) {
		zend_hash_destroy(intern->removed);
		efree(intern->removed);
	}

	if (ZEND_FCC_INITIALIZED(intern->on_watcher_removed_fcc)) {
		zend_fcc_dtor(&intern->on_watcher_removed_fcc);
	}

	zend_object_std_dtor(&intern->std);
}

static HashTable *php_io_poll_watcher_get_gc(zend_object *obj, zval **table, int *n)
{
	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	zend_get_gc_buffer_add_zval(gc_buffer, &intern->data);
	if (intern->handle) {
		zend_get_gc_buffer_add_obj(gc_buffer, &intern->handle->std);
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

static HashTable *php_io_poll_context_get_gc(zend_object *obj, zval **table, int *n)
{
	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZOBJ(obj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	if (intern->watchers) {
		ZEND_HASH_FOREACH_VAL(intern->watchers, zval *zv) {
			zend_get_gc_buffer_add_zval(gc_buffer, zv);
		} ZEND_HASH_FOREACH_END();
	}
	if (intern->timer_watchers) {
		ZEND_HASH_FOREACH_VAL(intern->timer_watchers, zval *zv) {
			zend_get_gc_buffer_add_zval(gc_buffer, zv);
		} ZEND_HASH_FOREACH_END();
	}
	if (intern->removed) {
		ZEND_HASH_FOREACH_VAL(intern->removed, zval *zv) {
			zend_get_gc_buffer_add_zval(gc_buffer, zv);
		} ZEND_HASH_FOREACH_END();
	}

	if (ZEND_FCC_INITIALIZED(intern->on_watcher_removed_fcc)) {
		if (intern->on_watcher_removed_fcc.object) {
			zend_get_gc_buffer_add_obj(gc_buffer, intern->on_watcher_removed_fcc.object);
		}
		if (intern->on_watcher_removed_fcc.closure) {
			zend_get_gc_buffer_add_obj(gc_buffer, intern->on_watcher_removed_fcc.closure);
		}
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

/* Utility functions */

static zend_result php_io_poll_watcher_modify_events(
		php_io_poll_watcher_object *watcher, uint32_t events)
{
	if (!watcher->active || !watcher->context) {
		zend_throw_exception(
				php_io_poll_inactive_watcher_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	php_poll_ctx *poll_ctx = watcher->context->ctx;

	if (watcher->timer) {
		php_io_poll_timer_handle_data *td = watcher->handle->handle_data;
		if ((events & ~(PHP_POLL_TIMER | PHP_POLL_ONESHOT)) || !(events & PHP_POLL_TIMER)) {
			zend_argument_value_error(1, "must be Event::Timer for a TimerHandle");
			return FAILURE;
		}
		/* Re-arm from now, with the handle's timeout */
		php_poll_timer_modify(poll_ctx, watcher->timer, zend_hrtime() + td->timeout,
				td->periodic ? td->timeout : 0, watcher);
		watcher->watched_events = events;
		return SUCCESS;
	}

	uint32_t backend_events;
	if (php_io_poll_handle_backend_events(watcher->handle, events, 1, &backend_events) == FAILURE) {
		return FAILURE;
	}

	/* Re-add if the backend dropped a fired one-shot registration */
	int fd = (int) watcher->fd;
	if (php_poll_modify(poll_ctx, fd, backend_events, watcher) != SUCCESS
			&& (php_poll_get_error(poll_ctx) != PHP_POLL_ERR_NOTFOUND
					|| php_poll_add(poll_ctx, fd, backend_events, watcher) != SUCCESS)) {
		php_poll_error err = php_poll_get_error(poll_ctx);
		php_io_poll_throw_failed_operation(php_io_poll_failed_watcher_mod_class_entry,
				"Failed to modify watcher in polling system", err);
		return FAILURE;
	}

	/* Update watcher state */
	watcher->watched_events = events;

	return SUCCESS;
}

static zend_result php_io_poll_watcher_modify_data(php_io_poll_watcher_object *watcher, zval *data)
{
	if (!watcher->active) {
		zend_throw_exception(
				php_io_poll_inactive_watcher_class_entry, "Cannot modify inactive watcher", 0);
		return FAILURE;
	}

	/* Update user data */
	zval_ptr_dtor(&watcher->data);
	ZVAL_COPY(&watcher->data, data);

	return SUCCESS;
}

/* PHP Method Implementations */

PHP_METHOD(Io_Poll_Backend, getAvailableBackends)
{
	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	/* Check each backend type for availability */
	php_poll_backend_type backends[] = {PHP_POLL_BACKEND_POLL, PHP_POLL_BACKEND_EPOLL,
			PHP_POLL_BACKEND_KQUEUE, PHP_POLL_BACKEND_EVENTPORT, PHP_POLL_BACKEND_WSAPOLL};

	for (size_t i = 0; i < sizeof(backends) / sizeof(backends[0]); i++) {
		if (php_poll_is_backend_available(backends[i])) {
			const char *name = php_io_poll_backend_type_to_name(backends[i]);
			zval enum_case;
			ZVAL_OBJ_COPY(&enum_case, zend_enum_get_case_cstr(php_io_poll_backend_class_entry, name));
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

PHP_METHOD(Io_Poll_Backend, supportsPriority)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_backend_type type = php_io_poll_backend_enum_to_type(Z_OBJ_P(ZEND_THIS));
	RETURN_BOOL(php_poll_backend_supports_priority(type));
}

PHP_METHOD(Io_Poll_Backend, supportsProcessHandles)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_backend_type type = php_io_poll_backend_enum_to_type(Z_OBJ_P(ZEND_THIS));
	RETURN_BOOL(php_poll_backend_supports_process_handles(type));
}

PHP_METHOD(Io_Poll_Backend, supportsSignalHandles)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_backend_type type = php_io_poll_backend_enum_to_type(Z_OBJ_P(ZEND_THIS));
	RETURN_BOOL(php_poll_backend_supports_signal_handles(type));
}

PHP_METHOD(StreamPollHandle, __construct)
{
	php_stream *stream;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		PHP_Z_PARAM_STREAM(stream)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);

	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}

	/* Set up stream-specific data */
	php_stream_poll_handle_data *data = emalloc(sizeof(php_stream_poll_handle_data));
	data->res = stream->res;
	intern->handle_data = data;

	/* Add reference to stream */
	GC_ADDREF(data->res);
}

PHP_METHOD(StreamPollHandle, getStream)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	php_stream_poll_handle_data *data = intern->handle_data;

	if (!data || !data->res) {
		RETURN_NULL();
	}

	GC_ADDREF(data->res);
	ZVAL_RES(return_value, data->res);
}

PHP_METHOD(StreamPollHandle, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL(intern->ops->is_valid(intern));
}

PHP_METHOD(Io_Poll_Watcher, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Watcher, use Context::add");
}

PHP_METHOD(Io_Poll_Watcher, getHandle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
	if (!intern->handle) {
		RETURN_NULL();
	}

	RETURN_OBJ_COPY(&intern->handle->std);
}

PHP_METHOD(Io_Poll_Watcher, getWatchedEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
	php_io_poll_events_to_event_enums(intern->watched_events, return_value);
}

PHP_METHOD(Io_Poll_Watcher, getTriggeredEvents)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
	php_io_poll_events_to_event_enums(intern->triggered_events, return_value);
}

PHP_METHOD(Io_Poll_Watcher, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
	ZVAL_COPY(return_value, &intern->data);
}

PHP_METHOD(Io_Poll_Watcher, hasTriggered)
{
	zval *event_enum;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(event_enum, php_io_poll_event_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	uint32_t event_bit = php_io_poll_event_enum_to_bit(Z_OBJ_P(event_enum));

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL((intern->triggered_events & event_bit) != 0);
}

PHP_METHOD(Io_Poll_Watcher, isActive)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);
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
		zend_argument_type_error(1, "must be array of Event enums");
		RETURN_THROWS();
	}

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);

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
		zend_argument_type_error(1, "must be array of Event enums");
		RETURN_THROWS();
	}

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);

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

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);

	if (php_io_poll_watcher_modify_data(intern, data) != SUCCESS) {
		RETURN_THROWS();
	}
}

PHP_METHOD(Io_Poll_Watcher, remove)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_watcher_object *intern = PHP_POLL_WATCHER_OBJ_FROM_ZV(ZEND_THIS);

	if (!intern->active || !intern->context) {
		/* Closing the stream already removed it, so this is just the expected cleanup */
		if (intern->closed) {
			return;
		}
		zend_throw_exception(
				php_io_poll_inactive_watcher_class_entry, "Cannot remove inactive watcher", 0);
		RETURN_THROWS();
	}

	php_io_poll_context_retire_watcher(intern->context, intern, false);
}

PHP_METHOD(Io_Poll_Context, __construct)
{
	zval *backend_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS(backend_obj, php_io_poll_backend_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(ZEND_THIS);

	if (intern->ctx) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}

	php_poll_backend_type backend_type = PHP_POLL_BACKEND_AUTO;
	if (backend_obj != NULL) {
		backend_type = php_io_poll_backend_enum_to_type(Z_OBJ_P(backend_obj));
	}

	intern->ctx = php_poll_create(backend_type, 0);

	if (!intern->ctx) {
		zend_throw_exception_ex(
				php_io_poll_failed_backend_unavailable_class_entry, 0, "Backend %s not available",
				php_io_poll_backend_type_to_name(backend_type));
		RETURN_THROWS();
	}

	if (php_poll_init(intern->ctx) != SUCCESS) {
		php_poll_error err = php_poll_get_error(intern->ctx);
		php_poll_destroy(intern->ctx);
		intern->ctx = NULL;
		php_io_poll_throw_failed_operation(php_io_poll_failed_context_init_class_entry,
				"Failed to initialize polling context", err);
		RETURN_THROWS();
	}

	intern->watchers = emalloc(sizeof(HashTable));
	zend_hash_init(intern->watchers, 8, NULL, ZVAL_PTR_DTOR, 0);
	intern->timer_watchers = emalloc(sizeof(HashTable));
	zend_hash_init(intern->timer_watchers, 4, NULL, ZVAL_PTR_DTOR, 0);
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

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(ZEND_THIS);
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZV(handle_obj);

	events = php_io_poll_event_enums_to_events(event_enums);
	if (!events) {
		zend_argument_type_error(2, "must be array of Event enums");
		RETURN_THROWS();
	}

	if (handle->ops == &php_io_poll_timer_handle_ops) {
		php_io_poll_timer_handle_data *td = handle->handle_data;
		if ((events & ~(PHP_POLL_TIMER | PHP_POLL_ONESHOT)) || !(events & PHP_POLL_TIMER)) {
			zend_argument_value_error(2, "must be Event::Timer for a TimerHandle");
			RETURN_THROWS();
		}
		if (!td) {
			zend_throw_exception(
					php_io_poll_invalid_handle_class_entry, "Invalid handle for polling", 0);
			RETURN_THROWS();
		}

		object_init_ex(return_value, php_io_poll_watcher_class_entry);
		php_io_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZV(return_value);
		watcher->handle = handle;
		watcher->watched_events = events;
		GC_ADDREF(&handle->std);
		if (data) {
			ZVAL_COPY(&watcher->data, data);
		}

		watcher->timer = php_poll_timer_add(intern->ctx, zend_hrtime() + td->timeout,
				td->periodic ? td->timeout : 0, watcher);

		zval watcher_zv;
		ZVAL_OBJ(&watcher_zv, &watcher->std);
		GC_ADDREF(&watcher->std);
		zend_hash_index_add_new(intern->timer_watchers, php_io_poll_compute_ptr_key(watcher), &watcher_zv);

		watcher->active = true;
		watcher->context = intern;
		return;
	}

	uint32_t backend_events;
	if (php_io_poll_handle_backend_events(handle, events, 2, &backend_events) == FAILURE) {
		RETURN_THROWS();
	}

	/* Get file descriptor */
	php_socket_t fd = php_poll_handle_get_fd(handle);
	if (fd == SOCK_ERR && handle->ops->event && handle->handle_data) {
		/* A process or signal handle without a source on this platform */
		php_io_poll_throw_failed_operation(php_io_poll_failed_handle_add_class_entry,
				"This backend has no source for the handle", PHP_POLL_ERR_NOSUPPORT);
		RETURN_THROWS();
	}
	if (fd == SOCK_ERR) {
		zend_throw_exception(
				php_io_poll_invalid_handle_class_entry, "Invalid handle for polling", 0);
		RETURN_THROWS();
	}

	zval *existing_zv = zend_hash_index_find(intern->watchers, (zend_ulong) fd);
	if (existing_zv) {
		php_io_poll_watcher_object *existing = PHP_POLL_WATCHER_OBJ_FROM_ZOBJ(Z_OBJ_P(existing_zv));
		if (php_poll_handle_get_fd(existing->handle) == fd) {
			zend_throw_exception(
					php_io_poll_handle_already_watched_class_entry, "Handle already added", 0);
			RETURN_THROWS();
		}
		php_io_poll_context_retire_watcher(intern, existing, true);
	}

	/* Create watcher object */
	object_init_ex(return_value, php_io_poll_watcher_class_entry);
	php_io_poll_watcher_object *watcher = PHP_POLL_WATCHER_OBJ_FROM_ZV(return_value);

	watcher->handle = handle;
	watcher->watched_events = events;
	watcher->triggered_events = 0;

	GC_ADDREF(&handle->std);

	if (data) {
		ZVAL_COPY(&watcher->data, data);
	} else {
		ZVAL_NULL(&watcher->data);
	}

	/* Add to poll context */
	if (php_poll_add(intern->ctx, (int) fd, backend_events, watcher) != SUCCESS) {
		php_poll_error err = php_poll_get_error(intern->ctx);
		if (err == PHP_POLL_ERR_EXISTS) {
			zend_throw_exception(
					php_io_poll_handle_already_watched_class_entry, "Handle already added", 0);
		} else {
			php_io_poll_throw_failed_operation(
					php_io_poll_failed_handle_add_class_entry, "Failed to add handle", err);
		}
		RETURN_THROWS();
	}

	/* Store in our watchers map */
	zval watcher_zv;
	ZVAL_OBJ_COPY(&watcher_zv, &watcher->std);
	zend_hash_index_add_new(intern->watchers, (zend_ulong) fd, &watcher_zv);

	watcher->active = true;
	watcher->context = intern;
	watcher->fd = fd;

	/* Reverse lookup so that invalidating the handle finds every context */
	if (!handle->watching) {
		handle->watching = emalloc(sizeof(HashTable));
		zend_hash_init(handle->watching, 4, NULL, NULL, 0);
	}
	zval watcher_ptr_zv;
	ZVAL_PTR(&watcher_ptr_zv, watcher);
	zend_hash_index_update(handle->watching, php_io_poll_compute_ptr_key(intern), &watcher_ptr_zv);
	if (handle->ops->added) {
		handle->ops->added(handle);
	}

	if (handle->ops == &php_stream_poll_handle_ops) {
		php_stream *stream = php_stream_poll_handle_get_stream(handle);
		if (stream) {
			php_io_poll_stream_watch(stream, watcher);
		}
	}
}

PHP_METHOD(Io_Poll_Context, wait)
{
	php_date_time_duration *timeout = NULL;
	zend_long max_events = 0;
	bool max_events_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_DATE_TIME_DURATION_OR_NULL(timeout)
		Z_PARAM_LONG_OR_NULL(max_events, max_events_is_null)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(ZEND_THIS);

	/* Build timespec from php_date_time_duration, or NULL for indefinite */
	struct timespec timeout_ts;
	if (timeout) {
		if (timeout->duration.negative) {
			zend_argument_value_error(1, "must not be negative");
			RETURN_THROWS();
		}

		timeout_ts.tv_sec = timeout->duration.seconds;
		timeout_ts.tv_nsec = timeout->duration.nanoseconds;
	}

	if (max_events_is_null) {
		max_events = php_poll_get_suitable_max_events(intern->ctx);
		if (max_events <= 0) {
			max_events = 64;
		}
		/* Timers are reported in the same array */
		max_events += php_poll_timer_count(intern->ctx);
	} else if (UNEXPECTED(max_events <= 0)) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	} else if (ZEND_LONG_INT_OVFL(max_events)) {
		zend_argument_value_error(2, "must be less than or equal to %d", INT_MAX);
		RETURN_THROWS();
	}

	if (php_io_poll_context_deliver_removed(intern) == FAILURE) {
		RETURN_THROWS();
	}

	/* A handler pending before the wait runs before it, as one that interrupts it does */
	if (php_io_interrupt_pending()) {
		RETURN_EMPTY_ARRAY();
	}

	php_poll_event *events = safe_emalloc((size_t) max_events, sizeof(*events), 0);
	int num_events = php_poll_wait(intern->ctx, events, (int) max_events, timeout ? &timeout_ts : NULL);

	if (num_events < 0) {
		php_poll_error err = php_poll_get_error(intern->ctx);
		efree(events);
		if (err == PHP_POLL_ERR_INTERRUPTED) {
			/* A signal handler runs before the caller waits again */
			RETURN_EMPTY_ARRAY();
		}
		php_io_poll_throw_failed_operation(
				php_io_poll_failed_wait_class_entry, "Poll wait failed", err);
		RETURN_THROWS();
	}

	array_init(return_value);

	for (int i = 0; i < num_events; i++) {
		php_io_poll_watcher_object *watcher = (php_io_poll_watcher_object *) events[i].data;
		if (watcher) {
			uint32_t triggered = php_io_poll_handle_fired(watcher->handle, events[i].revents);
			if (!watcher->timer && php_io_poll_handle_exhausted(watcher->handle)) {
				/* Reported once; the watcher stays until it is removed */
				php_poll_remove(intern->ctx, (int) watcher->fd);
			}
			if (!triggered) {
				continue;
			}
			watcher->triggered_events = triggered;

			add_next_index_object(return_value, zend_object_copy(&watcher->std));
		}
	}

	efree(events);
}

PHP_METHOD(Io_Poll_Context, getBackend)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(ZEND_THIS);
	php_poll_backend_type backend_type = php_poll_get_backend_type(intern->ctx);
	const char *backend_name = php_io_poll_backend_type_to_name(backend_type);

	RETURN_OBJ_COPY(zend_enum_get_case_cstr(php_io_poll_backend_class_entry, backend_name));
}

PHP_METHOD(Io_Poll_Context, onWatcherRemoved)
{
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fcc = empty_fcall_info_cache;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	php_io_poll_context_object *intern = PHP_POLL_CONTEXT_OBJ_FROM_ZV(ZEND_THIS);

	if (ZEND_FCC_INITIALIZED(intern->on_watcher_removed_fcc)) {
		zend_fcc_dtor(&intern->on_watcher_removed_fcc);
	}

	if (ZEND_FCI_INITIALIZED(fci)) {
		intern->on_watcher_removed_fcc = fcc;
		zend_fcc_addref(&intern->on_watcher_removed_fcc);
	}
}

PHPAPI void php_poll_handle_invalidate(zend_object *handle_obj)
{
	php_io_poll_handle_remove_from_all_contexts(handle_obj);
}

/* Invalidation: the handle's resource is going away, so every watcher on it
 * is retired from its context while the fd is still open. */
PHPAPI void php_io_poll_handle_remove_from_all_contexts(zend_object *handle_obj)
{
	php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle_obj);
	if (!handle->watching) {
		return;
	}

	HashTable *watching = handle->watching;
	handle->watching = NULL;

	uint32_t n = 0;
	php_io_poll_watcher_object **list = safe_emalloc(zend_hash_num_elements(watching), sizeof(*list), 0);
	ZEND_HASH_FOREACH_VAL(watching, zval *zv) {
		php_io_poll_watcher_object *watcher = (php_io_poll_watcher_object *) Z_PTR_P(zv);
		GC_ADDREF(&watcher->std);
		list[n++] = watcher;
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(watching);
	efree(watching);

	php_io_poll_retire_watchers(list, n);
	if (handle->ops->removed) {
		for (uint32_t i = 0; i < n; i++) {
			handle->ops->removed(handle);
		}
	}
}

/* Initialize the stream poll classes - add to PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(poll)
{
	/* Register backend enum */
	php_io_poll_backend_class_entry = register_class_Io_Poll_Backend();

	/* Register event enum */
	php_io_poll_event_class_entry = register_class_Io_Poll_Event();

	/* Register Handle interface */
	php_io_poll_handle_class_entry = register_class_Io_Poll_Handle();
	php_io_poll_handle_class_entry->interface_gets_implemented = php_stream_poll_handle_implement_interface;

	/* Register StreamPollHandle class */
	php_stream_poll_handle_class_entry
			= register_class_StreamPollHandle(php_io_poll_handle_class_entry);
	php_stream_poll_handle_class_entry->create_object = php_stream_poll_handle_create_object;

	memcpy(&php_io_poll_handle_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_io_poll_handle_object_handlers.offset = offsetof(php_poll_handle_object, std);
	php_io_poll_handle_object_handlers.free_obj = php_poll_handle_object_free;
	php_io_poll_handle_object_handlers.get_gc = php_io_poll_handle_get_gc;
	php_io_poll_handle_object_handlers.clone_obj = NULL;
	php_stream_poll_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	/* Register WeakHandle interface */
	php_io_poll_weak_handle_class_entry = register_class_Io_Poll_WeakHandle(php_io_poll_handle_class_entry);
	php_io_poll_weak_handle_class_entry->interface_gets_implemented = php_stream_poll_handle_implement_interface;

	/* Register StreamPollWeakHandle class */
	php_stream_poll_weak_handle_class_entry
			= register_class_StreamPollWeakHandle(php_io_poll_weak_handle_class_entry);
	php_stream_poll_weak_handle_class_entry->create_object = php_stream_poll_weak_handle_create_object;
	php_stream_poll_weak_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	/* Register the generic handles */
	php_io_poll_timer_handle_class_entry = register_class_Io_Poll_TimerHandle(php_io_poll_handle_class_entry);
	php_io_poll_timer_handle_class_entry->create_object = php_io_poll_timer_handle_create_object;
	php_io_poll_timer_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	php_io_poll_notify_handle_class_entry = register_class_Io_Poll_NotifyHandle(php_io_poll_handle_class_entry);
	php_io_poll_notify_handle_class_entry->create_object = php_io_poll_notify_handle_create_object;
	php_io_poll_notify_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	php_io_poll_process_handle_class_entry = register_class_Io_Poll_ProcessHandle(php_io_poll_handle_class_entry);
	php_io_poll_process_handle_class_entry->create_object = php_io_poll_process_handle_create_object;
	php_io_poll_process_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	php_io_poll_signal_handle_class_entry = register_class_Io_Poll_SignalHandle(php_io_poll_handle_class_entry);
	php_io_poll_signal_handle_class_entry->create_object = php_io_poll_signal_handle_create_object;
	php_io_poll_signal_handle_class_entry->default_object_handlers = &php_io_poll_handle_object_handlers;

	/* Register Watcher class */
	php_io_poll_watcher_class_entry = register_class_Io_Poll_Watcher();
	php_io_poll_watcher_class_entry->create_object = php_io_poll_watcher_create_object;

	memcpy(&php_io_poll_watcher_object_handlers, &std_object_handlers,
			sizeof(zend_object_handlers));
	php_io_poll_watcher_object_handlers.offset = offsetof(php_io_poll_watcher_object, std);
	php_io_poll_watcher_object_handlers.free_obj = php_io_poll_watcher_free_object;
	php_io_poll_watcher_object_handlers.get_gc = php_io_poll_watcher_get_gc;
	php_io_poll_watcher_object_handlers.clone_obj = NULL;
	php_io_poll_watcher_class_entry->default_object_handlers = &php_io_poll_watcher_object_handlers;

	/* Register Context class */
	php_io_poll_context_class_entry = register_class_Io_Poll_Context();
	php_io_poll_context_class_entry->create_object = php_io_poll_context_create_object;

	memcpy(&php_io_poll_context_object_handlers, &std_object_handlers,
			sizeof(zend_object_handlers));
	php_io_poll_context_object_handlers.offset = offsetof(php_io_poll_context_object, std);
	php_io_poll_context_object_handlers.free_obj = php_io_poll_context_free_object;
	php_io_poll_context_object_handlers.get_gc = php_io_poll_context_get_gc;
	php_io_poll_context_object_handlers.clone_obj = NULL;
	php_io_poll_context_class_entry->default_object_handlers = &php_io_poll_context_object_handlers;

	/* Register exception hierarchy */
	php_io_exception_class_entry = register_class_Io_IoException(zend_ce_exception);

	php_io_poll_exception_class_entry
			= register_class_Io_Poll_PollException(php_io_exception_class_entry);

	php_io_poll_failed_operation_class_entry = register_class_Io_Poll_FailedPollOperationException(
			php_io_poll_exception_class_entry);

	php_io_poll_failed_context_init_class_entry
			= register_class_Io_Poll_FailedContextInitializationException(
					php_io_poll_failed_operation_class_entry);

	php_io_poll_failed_handle_add_class_entry = register_class_Io_Poll_FailedHandleAddException(
			php_io_poll_failed_operation_class_entry);

	php_io_poll_failed_watcher_mod_class_entry
			= register_class_Io_Poll_FailedWatcherModificationException(
					php_io_poll_failed_operation_class_entry);

	php_io_poll_failed_wait_class_entry = register_class_Io_Poll_FailedPollWaitException(
			php_io_poll_failed_operation_class_entry);

	php_io_poll_failed_backend_unavailable_class_entry = register_class_Io_Poll_BackendUnavailableException(
		php_io_poll_exception_class_entry);

	php_io_poll_inactive_watcher_class_entry = register_class_Io_Poll_InactiveWatcherException(
			php_io_poll_exception_class_entry);

	php_io_poll_handle_already_watched_class_entry
			= register_class_Io_Poll_HandleAlreadyWatchedException(
					php_io_poll_exception_class_entry);

	php_io_poll_invalid_handle_class_entry
			= register_class_Io_Poll_InvalidHandleException(php_io_poll_exception_class_entry);

	/* Initialize polling backends */
	php_poll_register_backends();

	return SUCCESS;
}
