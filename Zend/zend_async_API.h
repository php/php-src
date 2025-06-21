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
#ifndef ZEND_ASYNC_API_H
#define ZEND_ASYNC_API_H

#include "zend_fibers.h"
#include "zend_globals.h"

#define ZEND_ASYNC_API "TrueAsync API v1.0.0-dev"
#define ZEND_ASYNC_API_VERSION_MAJOR 1
#define ZEND_ASYNC_API_VERSION_MINOR 0
#define ZEND_ASYNC_API_VERSION_PATCH 0

#define ZEND_ASYNC_API_VERSION_NUMBER \
	((ZEND_ASYNC_API_VERSION_MAJOR << 16) | \
	(ZEND_ASYNC_API_VERSION_MINOR << 8)  | \
	(ZEND_ASYNC_API_VERSION_PATCH))

#ifndef PHP_WIN32
#include <netdb.h>
#endif

/* Reactor Poll API */
typedef enum {
	ASYNC_READABLE = 1,
	ASYNC_WRITABLE = 2,
	ASYNC_DISCONNECT = 4,
	ASYNC_PRIORITIZED = 8
} async_poll_event;

/* Signal Constants */
#define ZEND_ASYNC_SIGHUP    1
#define ZEND_ASYNC_SIGINT    2
#define ZEND_ASYNC_SIGQUIT   3
#define ZEND_ASYNC_SIGILL    4
#define ZEND_ASYNC_SIGABRT_COMPAT 6
#define ZEND_ASYNC_SIGFPE    8
#define ZEND_ASYNC_SIGKILL   9
#define ZEND_ASYNC_SIGSEGV   11
#define ZEND_ASYNC_SIGTERM   15
#define ZEND_ASYNC_SIGBREAK  21
#define ZEND_ASYNC_SIGABRT   22
#define ZEND_ASYNC_SIGWINCH  28

//
// Definitions compatibles with proc_open()
//
#ifdef PHP_WIN32
typedef HANDLE zend_file_descriptor_t;
#define ZEND_FD_NULL NULL
typedef DWORD zend_process_id_t;
typedef HANDLE zend_process_t;
typedef SOCKET zend_socket_t;
#else
typedef int zend_file_descriptor_t;
typedef pid_t zend_process_id_t;
typedef pid_t zend_process_t;
typedef int zend_socket_t;
#define ZEND_FD_NULL 0
#endif

/**
 * php_exec
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 * If type==4, output will be saved to a memory buffer (shell_exec)
 */
typedef enum {
	ZEND_ASYNC_EXEC_MODE_EXEC,
	ZEND_ASYNC_EXEC_MODE_SYSTEM,
	ZEND_ASYNC_EXEC_MODE_EXEC_ARRAY,
	ZEND_ASYNC_EXEC_MODE_PASSTHRU,
	ZEND_ASYNC_EXEC_MODE_SHELL_EXEC
} zend_async_exec_mode;

typedef enum
{
	ZEND_ASYNC_CLASS_NO = 0,
	ZEND_ASYNC_CLASS_AWAITABLE = 1,
	ZEND_ASYNC_CLASS_COROUTINE = 2,
	ZEND_ASYNC_CLASS_SCOPE = 3,
	ZEND_ASYNC_CLASS_CONTEXT = 4,
	ZEND_ASYNC_CLASS_SCOPE_PROVIDER = 5,
	ZEND_ASYNC_CLASS_SPAWN_STRATEGY = 6,
	ZEND_ASYNC_CLASS_TIMEOUT = 7,

	ZEND_ASYNC_CLASS_CHANNEL = 10,
	ZEND_ASYNC_CLASS_FUTURE = 11,

	ZEND_ASYNC_EXCEPTION_DEFAULT = 30,
	ZEND_ASYNC_EXCEPTION_CANCELLATION = 31,
	ZEND_ASYNC_EXCEPTION_TIMEOUT = 32,
	ZEND_ASYNC_EXCEPTION_INPUT_OUTPUT = 33,
	ZEND_ASYNC_EXCEPTION_POLL = 34,
	ZEND_ASYNC_EXCEPTION_DNS = 35,
} zend_async_class;

/**
 * zend_coroutine_t is a Basic data structure that represents a coroutine in the Zend Engine.
 */
typedef struct _zend_coroutine_s zend_coroutine_t;
typedef struct _zend_async_context_s zend_async_context_t;
typedef struct _zend_async_waker_s zend_async_waker_t;
typedef struct _zend_async_microtask_s zend_async_microtask_t;
typedef struct _zend_async_scope_s zend_async_scope_t;
typedef struct _zend_fcall_s zend_fcall_t;
typedef void (*zend_coroutine_entry_t)(void);

/* Coroutine Switch Handlers */
typedef struct _zend_coroutine_switch_handler_s zend_coroutine_switch_handler_t;
typedef struct _zend_coroutine_switch_handlers_vector_s zend_coroutine_switch_handlers_vector_t;

typedef bool (*zend_coroutine_switch_handler_fn)(
	zend_coroutine_t *coroutine,
	bool is_enter,     /* true = entering coroutine, false = leaving */
	bool is_finishing  /* true = coroutine is finishing */
	/* returns: true = keep handler, false = remove handler after execution */
);

typedef struct _zend_async_event_s zend_async_event_t;
typedef struct _zend_async_event_callback_s zend_async_event_callback_t;
typedef struct _zend_async_waker_trigger_s zend_async_waker_trigger_t;
typedef struct _zend_coroutine_event_callback_s zend_coroutine_event_callback_t;
typedef void (*zend_async_event_callback_fn)
(
	zend_async_event_t *event,
	zend_async_event_callback_t *callback,
	void * result,
	zend_object *exception
);
typedef void (*zend_async_event_callback_dispose_fn)(zend_async_event_callback_t *callback, zend_async_event_t * event);
typedef void (*zend_async_event_add_callback_t)(zend_async_event_t *event, zend_async_event_callback_t *callback);
typedef void (*zend_async_event_del_callback_t)(zend_async_event_t *event, zend_async_event_callback_t *callback);
typedef void (*zend_async_event_callbacks_notify_t)(zend_async_event_t *event, void *result, zend_object *exception);
typedef void (*zend_async_event_start_t) (zend_async_event_t *event);
typedef void (*zend_async_event_stop_t) (zend_async_event_t *event);

/**
 * The replay method can be called in several modes:
 * If the callback parameter is not NULL, it will be invoked synchronously and immediately.
 * If callback is NULL, then the `result` and `exception` parameters will be filled in.
 *
 * The method will return true if the result was applied.
 */
typedef bool (*zend_async_event_replay_t) (
	zend_async_event_t *event, zend_async_event_callback_t *callback, zval *result, zend_object **exception
);
typedef void (*zend_async_event_dispose_t) (zend_async_event_t *event);
typedef zend_string* (*zend_async_event_info_t) (zend_async_event_t *event);

typedef struct _zend_async_poll_event_s zend_async_poll_event_t;
typedef struct _zend_async_timer_event_s zend_async_timer_event_t;
typedef struct _zend_async_signal_event_s zend_async_signal_event_t;
typedef struct _zend_async_filesystem_event_s zend_async_filesystem_event_t;

typedef struct _zend_async_process_event_s zend_async_process_event_t;
typedef struct _zend_async_thread_event_s zend_async_thread_event_t;

typedef struct _zend_async_dns_nameinfo_s zend_async_dns_nameinfo_t;
typedef struct _zend_async_dns_addrinfo_s zend_async_dns_addrinfo_t;

typedef struct _zend_async_exec_event_s zend_async_exec_event_t;

typedef struct _zend_async_listen_event_s zend_async_listen_event_t;

typedef struct _zend_async_task_s zend_async_task_t;

/* Internal context typedefs removed - using direct functions */

typedef zend_coroutine_t * (*zend_async_new_coroutine_t)(zend_async_scope_t *scope);
typedef zend_async_scope_t * (*zend_async_new_scope_t)(zend_async_scope_t * parent_scope);
typedef zend_coroutine_t * (*zend_async_spawn_t)(zend_async_scope_t *scope, zend_object *scope_provider);
typedef void (*zend_async_suspend_t)(bool from_main);
typedef void (*zend_async_enqueue_coroutine_t)(zend_coroutine_t *coroutine);
typedef void (*zend_async_resume_t)(zend_coroutine_t *coroutine, zend_object * error, const bool transfer_error);
typedef void (*zend_async_cancel_t)(zend_coroutine_t *coroutine, zend_object * error, bool transfer_error, const bool is_safely);
typedef void (*zend_async_shutdown_t)(void);
typedef zend_array* (*zend_async_get_coroutines_t)(void);
typedef void (*zend_async_add_microtask_t)(zend_async_microtask_t *microtask);
typedef zend_array* (*zend_async_get_awaiting_info_t)(zend_coroutine_t * coroutine);
typedef zend_class_entry* (*zend_async_get_class_ce_t)(zend_async_class type);

typedef void (*zend_async_reactor_startup_t)(void);
typedef void (*zend_async_reactor_shutdown_t)(void);
typedef bool (*zend_async_reactor_execute_t)(bool no_wait);
typedef bool (*zend_async_reactor_loop_alive_t)(void);

typedef zend_async_poll_event_t* (*zend_async_new_socket_event_t)(
	zend_socket_t socket, async_poll_event events, size_t extra_size
);
typedef zend_async_poll_event_t* (*zend_async_new_poll_event_t)(
	zend_file_descriptor_t fh, zend_socket_t socket, async_poll_event events, size_t extra_size
);
typedef zend_async_timer_event_t* (*zend_async_new_timer_event_t)(
	const zend_ulong timeout, const zend_ulong nanoseconds, const bool is_periodic, size_t extra_size
);
typedef zend_async_signal_event_t* (*zend_async_new_signal_event_t)(int signum, size_t extra_size);
typedef zend_async_process_event_t* (*zend_async_new_process_event_t)(zend_process_t process_handle, size_t extra_size);
typedef void (*zend_async_thread_entry_t)(void *arg, size_t extra_size);
typedef zend_async_thread_event_t* (*zend_async_new_thread_event_t)(
	zend_async_thread_entry_t entry, void *arg, size_t extra_size
);
typedef zend_async_filesystem_event_t* (*zend_async_new_filesystem_event_t)(
	zend_string * path, const unsigned int flags, size_t extra_size
);

typedef zend_async_dns_nameinfo_t* (*zend_async_getnameinfo_t)(const struct sockaddr* addr, int flags, size_t extra_size);
typedef zend_async_dns_addrinfo_t* (*zend_async_getaddrinfo_t)(
	const char *node, const char *service, const struct addrinfo *hints, size_t extra_size
);
typedef void (*zend_async_freeaddrinfo_t)(struct addrinfo *ai);

typedef zend_async_exec_event_t* (*zend_async_new_exec_event_t) (
	zend_async_exec_mode exec_mode,
	const char *cmd,
	zval *return_buffer,
	zval *return_value,
	zval *std_error,
	const char *cwd,
	const char *env,
	size_t extra_size
);

typedef zend_async_listen_event_t* (*zend_async_socket_listen_t)(
	const char *host, int port, int backlog, size_t extra_size
);

typedef int (*zend_async_listen_get_local_address_t)(
	zend_async_listen_event_t *listen_event,
	char *host, size_t host_len,
	int *port
);

typedef int (* zend_async_exec_t) (
	zend_async_exec_mode exec_mode,
	const char *cmd,
	zval *return_buffer,
	zval *return_value,
	zval *std_error,
	const char *cwd,
	const char *env,
	const zend_ulong timeout
);

typedef void (*zend_async_queue_task_t)(zend_async_task_t *task);

typedef void (*zend_async_microtask_handler_t)(zend_async_microtask_t *microtask);

struct _zend_fcall_s {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
};

///////////////////////////////////////////////////////////////////
/// Coroutine Switch Handlers Structures
///////////////////////////////////////////////////////////////////

struct _zend_coroutine_switch_handler_s {
	zend_coroutine_switch_handler_fn handler;  /* Handler function pointer */
};

struct _zend_coroutine_switch_handlers_vector_s {
	uint32_t length;                           /* Number of handlers */
	uint32_t capacity;                         /* Allocated capacity */
	zend_coroutine_switch_handler_t *data;     /* Array of handlers */
	bool in_execution;                         /* Protection flag during execution */
};

struct _zend_async_microtask_s {
	zend_async_microtask_handler_t handler;
	zend_async_microtask_handler_t dtor;
	bool is_cancelled;
	uint32_t ref_count;
};

///////////////////////////////////////////////////////////////////
/// Event Structures
///////////////////////////////////////////////////////////////////

struct _zend_async_event_callback_s {
	uint32_t ref_count;
	zend_async_event_callback_fn callback;
	zend_async_event_callback_dispose_fn dispose;
};

struct _zend_coroutine_event_callback_s {
	zend_async_event_callback_t base;
	zend_coroutine_t *coroutine;
};

struct _zend_async_waker_trigger_s {
	zend_async_event_t *event;
	zend_async_event_callback_t *callback;
};

/* Dynamic array of async event callbacks with single iterator protection */
typedef struct _zend_async_callbacks_vector_s {
	uint32_t                      length;          /* current number of callbacks          */
	uint32_t                      capacity;        /* allocated slots in the array         */
	zend_async_event_callback_t  **data;           /* dynamically allocated callback array */
	
	/* Single iterator tracking - NULL means no active iteration */
	uint32_t                     *current_iterator; /* pointer to active iterator index     */
} zend_async_callbacks_vector_t;

/**
 * Basic structure for representing events.
 * An event can be either an internal C object or a Zend object implementing the Awaitable interface.
 * In that case, the ZEND_ASYNC_EVENT_F_ZEND_OBJ flag is set to TRUE,
 * and the zend_object_offset field points to the offset of the zend_object structure.
 *
 * To manage the reference counter, use the macros:
 * ZEND_ASYNC_EVENT_ADD_REF, ZEND_ASYNC_EVENT_DEL_REF, ZEND_ASYNC_EVENT_RELEASE.
 *
 */
struct _zend_async_event_s {
	/* If event is closed, it cannot be started or stopped. */
	uint32_t flags;
	/* Offset to the beginning of additional data associated with the event (used for extensions) */
	uint32_t extra_offset;
	union
	{
		/* The refcount of the event. */
		uint32_t ref_count;
		/* The offset of Zend object structure. */
		uint32_t zend_object_offset;
	};
	/* The Event loop reference count. */
	uint32_t loop_ref_count;
	/* Events callbacks */
	zend_async_callbacks_vector_t callbacks;
	/* Methods */
	zend_async_event_add_callback_t add_callback;
	zend_async_event_del_callback_t del_callback;
	zend_async_event_start_t start;
	zend_async_event_stop_t stop;
	/*
	 * Replay method. Nullable.
	 * This method is implemented only by those events that can provide a result again, even after they have completed.
	 * For example, this method is relevant for coroutines and futures, which can provide the result again and again.
	 */
	zend_async_event_replay_t replay;
	zend_async_event_dispose_t dispose;
	/* Event info: can be NULL */
	zend_async_event_info_t info;
	/*
	 * Handler that is invoked before all event listeners are notified.
	 * May be NULL.
	 */
	zend_async_event_callbacks_notify_t notify_handler;
};

/**
 * Event reference. A special data structure that allows representing an object with the Awaitable interface,
 * but which does not store the event directly—instead, it holds only a reference to it.
 * This is necessary for events that are destroyed asynchronously and therefore cannot be used as Zend objects.
 *
 * For example, events like Timer, Poll, and Signal cannot be Zend objects
 * because their destruction cycle does not align.
 *
 * * flags should always be equal to ZEND_ASYNC_EVENT_REFERENCE_PREFIX.
 * * zend_object_offset is the offset of the Zend object structure.
 * * event is a pointer to the zend_async_event_t structure.
 */
#define ZEND_ASYNC_EVENT_REF_PROLOG         \
	uint32_t flags;                         \
	uint32_t zend_object_offset;

#define ZEND_ASYNC_EVENT_REF_FIELDS         \
	uint32_t flags;                         \
	uint32_t zend_object_offset;            \
	zend_async_event_t *event;

typedef struct {
	ZEND_ASYNC_EVENT_REF_FIELDS
} zend_async_event_ref_t;

#define ZEND_ASYNC_EVENT_F_CLOSED        (1u << 0)  /* event was closed */
#define ZEND_ASYNC_EVENT_F_RESULT_USED   (1u << 1)  /* result will be used in exception handler */
#define ZEND_ASYNC_EVENT_F_EXC_CAUGHT    (1u << 2)  /* error was caught in exception handler */
/* Indicates that the event produces a ZVAL pointer during the callback. */
#define ZEND_ASYNC_EVENT_F_ZVAL_RESULT   (1u << 3)
#define ZEND_ASYNC_EVENT_F_ZEND_OBJ 	 (1u << 4)  /* event is a zend object */
#define ZEND_ASYNC_EVENT_F_NO_FREE_MEMORY (1u << 5) /* event will not free memory in dispose handler */
#define ZEND_ASYNC_EVENT_F_EXCEPTION_HANDLED (1u << 6) /* exception has been caught and processed */

#define ZEND_ASYNC_EVENT_F_REFERENCE 	  (1u << 7)  /* event is a reference structure */

// Flag indicating that the event has a zend_object reference by extra_offset.
#define ZEND_ASYNC_EVENT_F_OBJ_REF 		  (1u << 8)  /* has zend_object ref */

#define ZEND_ASYNC_EVENT_REFERENCE_PREFIX ((uint32_t)0x80) /* prefix for reference structures */

// Create a reference to an event with the given offset and event pointer.
#define ZEND_ASYNC_EVENT_REF_SET(ptr, offset, ev) do {  \
	(ptr)->flags = ZEND_ASYNC_EVENT_REFERENCE_PREFIX;   \
	(ptr)->zend_object_offset = (offset);               \
	(ptr)->event = (ev);                                \
	} while (0)

#define ZEND_ASYNC_EVENT_IS_CLOSED(ev)         (((ev)->flags & ZEND_ASYNC_EVENT_F_CLOSED) != 0)
#define ZEND_ASYNC_EVENT_WILL_RESULT_USED(ev)  (((ev)->flags & ZEND_ASYNC_EVENT_F_RESULT_USED) != 0)
#define ZEND_ASYNC_EVENT_WILL_EXC_CAUGHT(ev)   (((ev)->flags & ZEND_ASYNC_EVENT_F_EXC_CAUGHT) != 0)
#define ZEND_ASYNC_EVENT_WILL_ZVAL_RESULT(ev)  (((ev)->flags & ZEND_ASYNC_EVENT_F_ZVAL_RESULT) != 0)
#define ZEND_ASYNC_EVENT_IS_ZEND_OBJ(ev)      (((ev)->flags & ZEND_ASYNC_EVENT_F_ZEND_OBJ) != 0)
#define ZEND_ASYNC_EVENT_IS_NO_FREE_MEMORY(ev) (((ev)->flags & ZEND_ASYNC_EVENT_F_NO_FREE_MEMORY) != 0)
#define ZEND_ASYNC_EVENT_IS_EXCEPTION_HANDLED(ev) (((ev)->flags & ZEND_ASYNC_EVENT_F_EXCEPTION_HANDLED) != 0)

#define ZEND_ASYNC_EVENT_SET_CLOSED(ev)        ((ev)->flags |=  ZEND_ASYNC_EVENT_F_CLOSED)
#define ZEND_ASYNC_EVENT_CLR_CLOSED(ev)        ((ev)->flags &= ~ZEND_ASYNC_EVENT_F_CLOSED)

#define ZEND_ASYNC_EVENT_SET_RESULT_USED(ev)   ((ev)->flags |=  ZEND_ASYNC_EVENT_F_RESULT_USED)
#define ZEND_ASYNC_EVENT_CLR_RESULT_USED(ev)   ((ev)->flags &= ~ZEND_ASYNC_EVENT_F_RESULT_USED)

#define ZEND_ASYNC_EVENT_SET_EXC_CAUGHT(ev)    ((ev)->flags |=  ZEND_ASYNC_EVENT_F_EXC_CAUGHT)
#define ZEND_ASYNC_EVENT_CLR_EXC_CAUGHT(ev)    ((ev)->flags &= ~ZEND_ASYNC_EVENT_F_EXC_CAUGHT)

#define ZEND_ASYNC_EVENT_SET_ZVAL_RESULT(ev)   ((ev)->flags |=  ZEND_ASYNC_EVENT_F_ZVAL_RESULT)
#define ZEND_ASYNC_EVENT_CLR_ZVAL_RESULT(ev)   ((ev)->flags &= ~ZEND_ASYNC_EVENT_F_ZVAL_RESULT)

#define ZEND_ASYNC_EVENT_SET_ZEND_OBJ(ev)      ((ev)->flags |=  ZEND_ASYNC_EVENT_F_ZEND_OBJ)
#define ZEND_ASYNC_EVENT_SET_ZEND_OBJ_OFFSET(ev, offset) ((ev)->zend_object_offset = (unsigned int) (offset))

#define ZEND_ASYNC_EVENT_SET_NO_FREE_MEMORY(ev) ((ev)->flags |=  ZEND_ASYNC_EVENT_F_NO_FREE_MEMORY)

#define ZEND_ASYNC_EVENT_SET_EXCEPTION_HANDLED(ev) ((ev)->flags |=  ZEND_ASYNC_EVENT_F_EXCEPTION_HANDLED)
#define ZEND_ASYNC_EVENT_CLR_EXCEPTION_HANDLED(ev) ((ev)->flags &= ~ZEND_ASYNC_EVENT_F_EXCEPTION_HANDLED)

#define ZEND_ASYNC_EVENT_WITH_OBJECT_REF(ev) ((ev)->flags |=  ZEND_ASYNC_EVENT_F_OBJ_REF)

// Convert awaitable Zend object to zend_async_event_t pointer
#define ZEND_ASYNC_EVENT_IS_REFERENCE(ptr) (*((const uint32_t *)(ptr)) == ZEND_ASYNC_EVENT_REFERENCE_PREFIX)
#define ZEND_ASYNC_OBJECT_TO_EVENT(obj)													 \
	(																					 \
		ZEND_ASYNC_EVENT_IS_REFERENCE((void *)((char *)(obj) - (obj)->handlers->offset)) \
		? ((zend_async_event_ref_t *)((char *)(obj) - (obj)->handlers->offset))->event	 \
		: (zend_async_event_t *)((char *)(obj) - (obj)->handlers->offset)				 \
	)

// Convert zend_async_event_t to zend_object pointer
#define ZEND_ASYNC_EVENT_TO_OBJECT(ev)							\
	(((ev)->flags & ZEND_ASYNC_EVENT_F_OBJ_REF)					\
	? *(zend_object **)((char *)(ev) + (ev)->extra_offset)		\
	: (zend_object *)((char *)(ev) + (ev)->zend_object_offset) )

// Get refcount of the event object
#define ZEND_ASYNC_EVENT_REF(ev) (ZEND_ASYNC_EVENT_IS_ZEND_OBJ(ev) ? \
		GC_REFCOUNT(ZEND_ASYNC_EVENT_TO_OBJECT(ev)) : \
		(ev)->ref_count)

// Proper increment of the event object's reference count.
#define ZEND_ASYNC_EVENT_ADD_REF(ev) (ZEND_ASYNC_EVENT_IS_ZEND_OBJ(ev) ? \
		GC_ADDREF(ZEND_ASYNC_EVENT_TO_OBJECT(ev)) : \
		++(ev)->ref_count)

// Proper decrement of the event object's reference count.
#define ZEND_ASYNC_EVENT_DEL_REF(ev) (ZEND_ASYNC_EVENT_IS_ZEND_OBJ(ev) ? \
		GC_DELREF(ZEND_ASYNC_EVENT_TO_OBJECT(ev)) : \
		--(ev)->ref_count)

/* Properly release the event object */
#define ZEND_ASYNC_EVENT_RELEASE(ev) do { \
	if (ZEND_ASYNC_EVENT_IS_ZEND_OBJ(ev)) { \
		if(GC_REFCOUNT(ZEND_ASYNC_EVENT_TO_OBJECT(ev)) == 1) { \
			OBJ_RELEASE(ZEND_ASYNC_EVENT_TO_OBJECT(ev)); \
		} else { \
			GC_DELREF(ZEND_ASYNC_EVENT_TO_OBJECT(ev)); \
		} \
	} else { \
		if ((ev)->ref_count == 1) { \
			(ev)->ref_count = 0; \
			(ev)->dispose(ev); \
		} else { \
			(ev)->ref_count--; \
		} \
	} \
} while (0)

#define ZEND_ASYNC_EVENT_REPLAY(ev, callback) (ev->replay != NULL ? ev->replay(ev, callback, NULL, NULL) : false)
#define ZEND_ASYNC_EVENT_EXTRACT_RESULT(ev, result) (ev->replay != NULL ? ev->replay(ev, NULL, result, NULL) : false)
#define ZEND_ASYNC_EVENT_EXTRACT_RESULT_OR_ERROR(ev, result, exception) (ev->replay != NULL ? ev->replay(ev, NULL, result, exception) : false)

/* Public callback vector functions - implementations in zend_async_API.c */
ZEND_API void zend_async_callbacks_notify(zend_async_event_t *event, void *result, zend_object *exception, bool from_handler);
ZEND_API void zend_async_callbacks_remove(zend_async_event_t *event, zend_async_event_callback_t *callback);
ZEND_API void zend_async_callbacks_free(zend_async_event_t *event);
ZEND_API void zend_async_callbacks_notify_and_close(zend_async_event_t *event, void *result, zend_object *exception);

#define ZEND_ASYNC_CALLBACKS_NOTIFY(event, result, exception) \
	zend_async_callbacks_notify((event), (result), (exception), false)

#define ZEND_ASYNC_CALLBACKS_NOTIFY_AND_CLOSE(event, result, exception) \
	zend_async_callbacks_notify_and_close((event), (result), (exception))

#define ZEND_ASYNC_CALLBACKS_NOTIFY_FROM_HANDLER(event, result, exception) \
	zend_async_callbacks_notify((event), (result), (exception), true)


/* Append a callback; grows the buffer when needed */
static zend_always_inline void
zend_async_callbacks_push(zend_async_event_t *event, zend_async_event_callback_t *callback)
{
	if (event->callbacks.data == NULL) {
		event->callbacks.data = safe_emalloc(4, sizeof(zend_async_event_callback_t *), 0);
		event->callbacks.capacity = 4;
	}

	zend_async_callbacks_vector_t *vector = &event->callbacks;

	if (vector->length == vector->capacity) {
		vector->capacity = vector->capacity ? vector->capacity * 2 : 4;
		vector->data = safe_erealloc(vector->data,
									 vector->capacity,
									 sizeof(zend_async_event_callback_t *),
									 0);
	}

	vector->data[vector->length++] = callback;
}

struct _zend_async_poll_event_s {
	zend_async_event_t base;
	bool is_socket;
	union {
		zend_file_descriptor_t file;
		zend_socket_t socket;
	};
	async_poll_event events;
	async_poll_event triggered_events;
};

struct _zend_async_timer_event_s {
	zend_async_event_t base;
	/* The timeout in milliseconds. */
	unsigned int timeout;
	/* The timer is periodic. */
	bool is_periodic;
};

struct _zend_async_signal_event_s {
	zend_async_event_t base;
	int signal;
};

struct _zend_async_process_event_s {
	zend_async_event_t base;
	zend_process_t process;
	zend_long exit_code;
};

struct _zend_async_thread_event_s {
	zend_async_event_t base;
};

struct _zend_async_filesystem_event_s {
	zend_async_event_t base;
	zend_string *path;
	unsigned int flags;
	unsigned int triggered_events;
	zend_string *triggered_filename;
};

struct _zend_async_dns_nameinfo_s {
	zend_async_event_t base;
	/* These structure fields store the RESULT of the operation.
	 * It will be automatically freed when the structure is destroyed. */
	zend_string *hostname;
	zend_string *service;
};

struct _zend_async_dns_addrinfo_s {
	zend_async_event_t base;
	const char *node;
	const char *service;
	/* The DNS resolution result must be explicitly and mandatorily freed using the ZEND_ASYNC_FREEADDRINFO method! */
	struct addrinfo *result;
};

struct _zend_async_exec_event_s {
	zend_async_event_t base;
	zend_async_exec_mode exec_mode;
	bool terminated;
	char * cmd;
	zval * return_value;
	zval * result_buffer;
	size_t output_len;
	char * output_buffer;
	zend_long exit_code;
	int term_signal;
	zval * std_error;
};

struct _zend_async_listen_event_s {
	zend_async_event_t base;
	const char *host;
	int port;
	int backlog;
	zend_socket_t socket_fd;
	zend_async_listen_get_local_address_t get_local_address;
};

struct _zend_async_task_s {
	zend_async_event_t base;
};

///////////////////////////////////////////////////////////////////
/// Scope Structures
///////////////////////////////////////////////////////////////////

typedef void (*zend_async_before_coroutine_enqueue_t)(zend_coroutine_t *coroutine, zend_async_scope_t *scope, zval *result);
typedef void (*zend_async_after_coroutine_enqueue_t)(zend_coroutine_t *coroutine, zend_async_scope_t *scope);
typedef void (*zend_async_scope_dispose_t)(zend_async_scope_t *scope);

/* Dynamic array of async event callbacks */
typedef struct _zend_async_scopes_vector_s {
	uint32_t                      length;   /* current number of items              */
	uint32_t                      capacity; /* allocated slots in the array         */
	zend_async_scope_t			  **data;    /* dynamically allocated array			*/
} zend_async_scopes_vector_t;

/**
 * The internal Scope structure and the Zend object Scope are different data structures.
 * This separation is intentional to manage their lifetimes independently.
 * The internal Scope structure can outlive the Zend object.
 * When the Zend object triggers the dtor_obj method,
 * it initiates the disposal process of the Scope.
 *
 * However, the internal Scope structure remains in memory until the last coroutine has completed.
 *
 */
struct _zend_async_scope_s {
	/* The scope ZEND_ASYNC_SCOPE_F flags */
	uint32_t flags;
	/* The link to the zend_object structure */
	zend_object * scope_object;

	zend_async_scopes_vector_t scopes;
	zend_async_scope_t *parent_scope;
	/* Scope context object */
	zend_async_context_t *context;

	zend_async_before_coroutine_enqueue_t before_coroutine_enqueue;
	zend_async_after_coroutine_enqueue_t after_coroutine_enqueue;
	zend_async_scope_dispose_t dispose;
};

#define ZEND_ASYNC_SCOPE_F_CLOSED				  (1u << 0)  /* scope was closed */
#define ZEND_ASYNC_SCOPE_F_NO_FREE_MEMORY	      (1u << 1)  /* scope will not free memory in dispose handler */
#define ZEND_ASYNC_SCOPE_F_DISPOSE_SAFELY 		  (1u << 2)  /* scope will be disposed safely */

#define ZEND_ASYNC_SCOPE_IS_CLOSED(scope)         (((scope)->flags & ZEND_ASYNC_SCOPE_F_CLOSED) != 0)
#define ZEND_ASYNC_SCOPE_IS_NO_FREE_MEMORY(scope) (((scope)->flags & ZEND_ASYNC_SCOPE_F_NO_FREE_MEMORY) != 0)
#define ZEND_ASYNC_SCOPE_IS_DISPOSE_SAFELY(scope) (((scope)->flags & ZEND_ASYNC_SCOPE_F_DISPOSE_SAFELY) != 0)

#define ZEND_ASYNC_SCOPE_SET_CLOSED(scope)        ((scope)->flags |=  ZEND_ASYNC_SCOPE_F_CLOSED)
#define ZEND_ASYNC_SCOPE_CLR_CLOSED(scope)        ((scope)->flags &= ~ZEND_ASYNC_SCOPE_F_CLOSED)

#define ZEND_ASYNC_SCOPE_SET_NO_FREE_MEMORY(scope) ((scope)->flags |=  ZEND_ASYNC_SCOPE_F_NO_FREE_MEMORY)
#define ZEND_ASYNC_SCOPE_CLR_NO_FREE_MEMORY(scope) ((scope)->flags &= ~ZEND_ASYNC_SCOPE_F_NO_FREE_MEMORY)

#define ZEND_ASYNC_SCOPE_SET_DISPOSE(scope)        ((scope)->flags |=  ZEND_ASYNC_SCOPE_F_DISPOSE_SAFELY)
#define ZEND_ASYNC_SCOPE_CLR_DISPOSE(scope)        ((scope)->flags &= ~ZEND_ASYNC_SCOPE_F_DISPOSE_SAFELY)

static zend_always_inline void
zend_async_scope_add_child(zend_async_scope_t *parent_scope, zend_async_scope_t *child_scope)
{
	zend_async_scopes_vector_t *vector = &parent_scope->scopes;

	child_scope->parent_scope = parent_scope;

	if (vector->data == NULL) {
		vector->data = safe_emalloc(4, sizeof(zend_async_scope_t *), 0);
		vector->capacity = 4;
	}

	if (vector->length == vector->capacity) {
		vector->capacity *= 2;
		vector->data = safe_erealloc(vector->data, vector->capacity, sizeof(zend_async_scope_t *), 0);
	}

	vector->data[vector->length++] = child_scope;
}

static zend_always_inline void
zend_async_scope_remove_child(zend_async_scope_t *parent_scope, zend_async_scope_t *child_scope)
{
	zend_async_scopes_vector_t *vector = &parent_scope->scopes;
	for (uint32_t i = 0; i < vector->length; ++i) {
		if (vector->data[i] == child_scope) {
			vector->data[i] = vector->data[--vector->length];
			child_scope->parent_scope = NULL;

			// Try to dispose the parent scope if it is empty
			if (parent_scope->scopes.length == 0) {
				parent_scope->dispose(parent_scope);
			}

			return;
		}
	}
}

static zend_always_inline void
zend_async_scope_free_children(zend_async_scope_t *parent_scope)
{
	zend_async_scopes_vector_t *vector = &parent_scope->scopes;

	if (vector->data != NULL) {
		efree(vector->data);
	}

	vector->data = NULL;
	vector->length = 0;
	vector->capacity = 0;
}

///////////////////////////////////////////////////////////////////
/// Waker Structures
///////////////////////////////////////////////////////////////////

typedef void (*zend_async_waker_dtor)(zend_coroutine_t *coroutine);

typedef enum {
	ZEND_ASYNC_WAKER_NO_STATUS,
	ZEND_ASYNC_WAKER_WAITING,
	ZEND_ASYNC_WAKER_QUEUED,
	ZEND_ASYNC_WAKER_IGNORED,
	ZEND_ASYNC_WAKER_RESULT
} ZEND_ASYNC_WAKER_STATUS;

/**
 *  Condition that is TRUE if the coroutine is in the queue
 */
#define ZEND_ASYNC_WAKER_IN_QUEUE(waker) (waker != NULL && ((waker)->status == ZEND_ASYNC_WAKER_QUEUED || waker->status == ZEND_ASYNC_WAKER_IGNORED))

struct _zend_async_waker_s {
	/* The waker status. */
	ZEND_ASYNC_WAKER_STATUS status;
	/* The array of zend_async_trigger_callback_t. */
	HashTable events;
	/* A list of events objects (zend_async_event_t) that occurred during the last iteration of the event loop. */
	HashTable *triggered_events;
	/* Result of the waker. */
	zval result;
	/* Error object. */
	zend_object *error;
	/* Filename of the waker object creation point. */
	zend_string *filename;
	/* Line number of the waker object creation point. */
	uint32_t lineno;
	/* The waker destructor. */
	zend_async_waker_dtor dtor;
};

#define ZEND_ASYNC_WAKER_WAITING(waker) ((waker)->status < ZEND_ASYNC_WAKER_RESULT)

/**
 * Coroutine destructor. Called when the coroutine needs to clean up all its data.
 */
typedef void (*zend_async_coroutine_dispose)(zend_coroutine_t *coroutine);

struct _zend_coroutine_s {
	zend_async_event_t event;
	/*
	 * Callback and info / cache to be used when coroutine is started.
	 * If NULL, the coroutine is not a userland coroutine and internal_entry is used.
	 */
	zend_fcall_t *fcall;

	/*
	 * The internal entry point of the coroutine.
	 * If NULL, the coroutine is a userland coroutine and fcall is used.
	 */
	zend_coroutine_entry_t internal_entry;

	/* The custom data for the coroutine. Can be NULL */
	void *extended_data;

	/* Coroutine waker */
	zend_async_waker_t *waker;
	/* Coroutine scope */
	zend_async_scope_t *scope;

	/* Storage for return value. */
	zval result;

	/* Exception object, if any, nullable */
	zend_object *exception;

	/* Coroutine context object */
	zend_async_context_t *context;

	/* Internal context (for C extensions with numeric keys) */
	HashTable *internal_context;

	/* Spawned file and line number */
	zend_string *filename;
	uint32_t lineno;

	/* Extended dispose handler */
	zend_async_coroutine_dispose extended_dispose;

	/* Switch handlers for context switching */
	zend_coroutine_switch_handlers_vector_t *switch_handlers;
};

/**
 * The macro evaluates to TRUE if the coroutine is in a waiting state —
 * either waiting for events or waiting in the execution queue.
 */
#define ZEND_COROUTINE_SUSPENDED(coroutine) ((coroutine)->waker != NULL && ZEND_ASYNC_WAKER_WAITING((coroutine)->waker))

/* Coroutine flags */
#define ZEND_COROUTINE_F_STARTED (1u << 10) /* coroutine is started */
#define ZEND_COROUTINE_F_CANCELLED (1u << 11) /* coroutine is cancelled */
#define ZEND_COROUTINE_F_ZOMBIE (1u << 12) /* coroutine is a zombie */
#define ZEND_COROUTINE_F_PROTECTED (1u << 13) /* coroutine is protected */
#define ZEND_COROUTINE_F_MAIN (1u << 14) /* coroutine is a main coroutine */

#define ZEND_COROUTINE_IS_ZOMBIE(coroutine) (((coroutine)->event.flags & ZEND_COROUTINE_F_ZOMBIE) != 0)
#define ZEND_COROUTINE_SET_ZOMBIE(coroutine) ((coroutine)->event.flags |= ZEND_COROUTINE_F_ZOMBIE)
#define ZEND_COROUTINE_IS_STARTED(coroutine) (((coroutine)->event.flags & ZEND_COROUTINE_F_STARTED) != 0)
#define ZEND_COROUTINE_IS_CANCELLED(coroutine) (((coroutine)->event.flags & ZEND_COROUTINE_F_CANCELLED) != 0)
#define ZEND_COROUTINE_IS_FINISHED(coroutine) (((coroutine)->event.flags & ZEND_ASYNC_EVENT_F_CLOSED) != 0)
#define ZEND_COROUTINE_IS_PROTECTED(coroutine) (((coroutine)->event.flags & ZEND_COROUTINE_F_PROTECTED) != 0)
#define ZEND_COROUTINE_IS_EXCEPTION_HANDLED(coroutine) ZEND_ASYNC_EVENT_IS_EXCEPTION_HANDLED(&(coroutine)->event)
#define ZEND_COROUTINE_IS_MAIN(coroutine) (((coroutine)->event.flags & ZEND_COROUTINE_F_MAIN) != 0)
#define ZEND_COROUTINE_SET_STARTED(coroutine) ((coroutine)->event.flags |= ZEND_COROUTINE_F_STARTED)
#define ZEND_COROUTINE_SET_CANCELLED(coroutine) ((coroutine)->event.flags |= ZEND_COROUTINE_F_CANCELLED)
#define ZEND_COROUTINE_SET_FINISHED(coroutine) ((coroutine)->event.flags |= ZEND_ASYNC_EVENT_F_CLOSED)
#define ZEND_COROUTINE_SET_PROTECTED(coroutine) ((coroutine)->event.flags |= ZEND_COROUTINE_F_PROTECTED)
#define ZEND_COROUTINE_SET_MAIN(coroutine) ((coroutine)->event.flags |= ZEND_COROUTINE_F_MAIN)
#define ZEND_COROUTINE_CLR_PROTECTED(coroutine) ((coroutine)->event.flags &= ~ZEND_COROUTINE_F_PROTECTED)
#define ZEND_COROUTINE_SET_EXCEPTION_HANDLED(coroutine) ZEND_ASYNC_EVENT_SET_EXCEPTION_HANDLED(&(coroutine)->event)
#define ZEND_COROUTINE_CLR_EXCEPTION_HANDLED(coroutine) ZEND_ASYNC_EVENT_CLR_EXCEPTION_HANDLED(&(coroutine)->event)

static zend_always_inline zend_string *zend_coroutine_callable_name(const zend_coroutine_t *coroutine)
{
	if (ZEND_COROUTINE_IS_MAIN(coroutine)) {
		return zend_string_init("main", sizeof("main") - 1, 0);
	}

	if (coroutine->fcall) {
		return zend_get_callable_name_ex(&coroutine->fcall->fci.function_name, NULL);
	}

	return zend_string_init("internal function", sizeof("internal function") - 1, 0);
}

///////////////////////////////////////////////////////////////
/// Async Context Structures
///////////////////////////////////////////////////////////////

typedef zend_async_context_t * (*zend_async_new_context_t)(void);
typedef bool (*zend_async_context_find_t)(zend_async_context_t *context, zval *key, zval *result, bool include_parent);
typedef void (*zend_async_context_set_t)(zend_async_context_t *context, zval *key, zval *value);
typedef bool (*zend_async_context_unset_t)(zend_async_context_t *context, zval *key);
typedef void (*zend_async_context_dispose_t)(zend_async_context_t *context);

struct _zend_async_context_s {
	/* flags for the context: reserved */
	uint32_t flags;
	/* offset of the context zend object */
	uint32_t offset;
	zend_async_context_find_t find;
	zend_async_context_set_t set;
	zend_async_context_unset_t unset;
	zend_async_context_dispose_t dispose;
};

///////////////////////////////////////////////////////////////
/// Global Macros
///////////////////////////////////////////////////////////////
/*
 * Async module state
 */
typedef enum {
	// The module is inactive.
	ZEND_ASYNC_OFF,
	// The module is ready for use but has not been activated yet.
	ZEND_ASYNC_READY,
	// The module is active and can be used.
	ZEND_ASYNC_ACTIVE
} zend_async_state_t;

typedef struct {
	zend_async_state_t state;
	/* The flag is TRUE if the Scheduler was able to gain control. */
	zend_atomic_bool heartbeat;
	/* Equal TRUE if the scheduler executed now */
	bool in_scheduler_context;
	/* Equal TRUE if the reactor is in the process of shutting down */
	bool graceful_shutdown;
	/* Number of active coroutines */
	unsigned int active_coroutine_count;
	/* Number of active event handles */
	unsigned int active_event_count;
	/* The current coroutine context. */
	zend_coroutine_t *coroutine;
	/* The main async scope. */
	zend_async_scope_t *main_scope;
	/* Scheduler coroutine */
	zend_coroutine_t *scheduler;
	/* Exit exception object */
	zend_object *exit_exception;
} zend_async_globals_t;

BEGIN_EXTERN_C()
#ifdef ZTS
ZEND_API extern int zend_async_globals_id;
ZEND_API extern size_t zend_async_globals_offset;
#define ZEND_ASYNC_G(v) ZEND_TSRMG_FAST(zend_async_globals_offset, zend_async_globals_t *, v)
#else
#define ZEND_ASYNC_G(v) (zend_async_globals_api.v)
ZEND_API extern zend_async_globals_t zend_async_globals_api;
#endif
END_EXTERN_C()

#define ZEND_ASYNC_ON (ZEND_ASYNC_G(state) > ZEND_ASYNC_OFF)
#define ZEND_ASYNC_IS_ACTIVE (ZEND_ASYNC_G(state) == ZEND_ASYNC_ACTIVE)
#define ZEND_ASYNC_IS_OFF (ZEND_ASYNC_G(state) == ZEND_ASYNC_OFF)
#define ZEND_ASYNC_IS_READY (ZEND_ASYNC_G(state) == ZEND_ASYNC_READY)
#define ZEND_ASYNC_ACTIVATE ZEND_ASYNC_G(state) = ZEND_ASYNC_ACTIVE
#define ZEND_ASYNC_INITIALIZE ZEND_ASYNC_G(state) = ZEND_ASYNC_READY
#define ZEND_ASYNC_DEACTIVATE ZEND_ASYNC_G(state) = ZEND_ASYNC_OFF
#define ZEND_ASYNC_SCHEDULER_ALIVE (zend_atomic_bool_load(&ZEND_ASYNC_G(heartbeat)) == true)
#define ZEND_ASYNC_SCHEDULER_HEARTBEAT zend_atomic_bool_store(&ZEND_ASYNC_G(heartbeat), true)
#define ZEND_ASYNC_SCHEDULER_WAIT zend_atomic_bool_store(&ZEND_ASYNC_G(heartbeat), false)
#define ZEND_ASYNC_SCHEDULER_CONTEXT ZEND_ASYNC_G(in_scheduler_context)
#define ZEND_ASYNC_IS_SCHEDULER_CONTEXT (ZEND_ASYNC_G(in_scheduler_context) == true)
#define ZEND_ASYNC_ACTIVE_COROUTINE_COUNT ZEND_ASYNC_G(active_coroutine_count)
#define ZEND_ASYNC_ACTIVE_EVENT_COUNT ZEND_ASYNC_G(active_event_count)
#define ZEND_ASYNC_GRACEFUL_SHUTDOWN ZEND_ASYNC_G(graceful_shutdown)
#define ZEND_ASYNC_EXIT_EXCEPTION ZEND_ASYNC_G(exit_exception)
#define ZEND_ASYNC_CURRENT_COROUTINE ZEND_ASYNC_G(coroutine)
#define ZEND_ASYNC_CURRENT_SCOPE (ZEND_ASYNC_G(coroutine) ? ZEND_ASYNC_G(coroutine)->scope : NULL)
#define ZEND_ASYNC_MAIN_SCOPE ZEND_ASYNC_G(main_scope)
#define ZEND_ASYNC_SCHEDULER ZEND_ASYNC_G(scheduler)

#define ZEND_ASYNC_INCREASE_EVENT_COUNT  if (ZEND_ASYNC_G(active_event_count) < UINT_MAX) { \
		ZEND_ASYNC_G(active_event_count)++; \
	} else { \
		ZEND_ASSERT("The event count is already max."); \
	}

#define ZEND_ASYNC_DECREASE_EVENT_COUNT  if (ZEND_ASYNC_G(active_event_count) > 0) { \
		ZEND_ASYNC_G(active_event_count)--; \
	} else { \
		ZEND_ASSERT("The event count is already zero."); \
	}

#define ZEND_ASYNC_INCREASE_COROUTINE_COUNT  if (ZEND_ASYNC_G(active_coroutine_count) < UINT_MAX) { \
		ZEND_ASYNC_G(active_coroutine_count)++; \
	} else { \
		ZEND_ASSERT("The coroutine count is already max."); \
	}

#define ZEND_ASYNC_DECREASE_COROUTINE_COUNT  if (ZEND_ASYNC_G(active_coroutine_count) > 0) { \
		ZEND_ASYNC_G(active_coroutine_count)--; \
	} else { \
		ZEND_ASSERT("The coroutine count is already zero."); \
	}


BEGIN_EXTERN_C()

ZEND_API bool zend_async_is_enabled(void);
ZEND_API bool zend_scheduler_is_enabled(void);

void zend_async_init(void);
void zend_async_shutdown(void);
void zend_async_globals_ctor(void);
void zend_async_globals_dtor(void);

ZEND_API const char * zend_async_get_api_version(void);
ZEND_API int zend_async_get_api_version_number(void);

ZEND_API ZEND_COLD zend_object * zend_async_new_exception(zend_async_class type, const char *format, ...);
ZEND_API ZEND_COLD zend_object * zend_async_throw(zend_async_class type, const char *format, ...);
ZEND_API ZEND_COLD zend_object * zend_async_throw_cancellation(const char *format, ...);
ZEND_API ZEND_COLD zend_object * zend_async_throw_timeout(const char *format, const zend_long timeout);

/* Scheduler API */

ZEND_API extern zend_async_spawn_t zend_async_spawn_fn;
ZEND_API extern zend_async_new_coroutine_t zend_async_new_coroutine_fn;
ZEND_API extern zend_async_new_scope_t zend_async_new_scope_fn;
ZEND_API extern zend_async_suspend_t zend_async_suspend_fn;
ZEND_API extern zend_async_enqueue_coroutine_t zend_async_enqueue_coroutine_fn;
ZEND_API extern zend_async_resume_t zend_async_resume_fn;
ZEND_API extern zend_async_cancel_t zend_async_cancel_fn;
ZEND_API extern zend_async_shutdown_t zend_async_shutdown_fn;
ZEND_API extern zend_async_get_coroutines_t zend_async_get_coroutines_fn;
ZEND_API extern zend_async_add_microtask_t zend_async_add_microtask_fn;
ZEND_API extern zend_async_get_awaiting_info_t zend_async_get_awaiting_info_fn;
ZEND_API extern zend_async_get_class_ce_t zend_async_get_class_ce_fn;

/* Context API */
ZEND_API extern zend_async_new_context_t zend_async_new_context_fn;

/* Internal Context API - Direct Functions */
ZEND_API uint32_t zend_async_internal_context_key_alloc(const char *key_name);
ZEND_API const char* zend_async_internal_context_key_name(uint32_t key);
ZEND_API zval * zend_async_internal_context_find(zend_coroutine_t *coroutine, uint32_t key);
ZEND_API void zend_async_internal_context_set(zend_coroutine_t *coroutine, uint32_t key, zval *value);
ZEND_API bool zend_async_internal_context_unset(zend_coroutine_t *coroutine, uint32_t key);

/* Internal Context initialization and cleanup */
ZEND_API void zend_async_init_internal_context_api(void);
ZEND_API void zend_async_coroutine_internal_context_dispose(zend_coroutine_t *coroutine);
ZEND_API void zend_async_internal_context_api_shutdown(void);
ZEND_API void zend_async_coroutine_internal_context_init(zend_coroutine_t *coroutine);

/* Reactor API */

ZEND_API bool zend_async_reactor_is_enabled(void);
ZEND_API extern zend_async_reactor_startup_t zend_async_reactor_startup_fn;
ZEND_API extern zend_async_reactor_shutdown_t zend_async_reactor_shutdown_fn;
ZEND_API extern zend_async_reactor_execute_t zend_async_reactor_execute_fn;
ZEND_API extern zend_async_reactor_loop_alive_t zend_async_reactor_loop_alive_fn;
ZEND_API extern zend_async_new_socket_event_t zend_async_new_socket_event_fn;
ZEND_API extern zend_async_new_poll_event_t zend_async_new_poll_event_fn;
ZEND_API extern zend_async_new_timer_event_t zend_async_new_timer_event_fn;
ZEND_API extern zend_async_new_signal_event_t zend_async_new_signal_event_fn;
ZEND_API extern zend_async_new_process_event_t zend_async_new_process_event_fn;
ZEND_API extern zend_async_new_thread_event_t zend_async_new_thread_event_fn;
ZEND_API extern zend_async_new_filesystem_event_t zend_async_new_filesystem_event_fn;

/* Socket Listening API */

ZEND_API extern zend_async_socket_listen_t zend_async_socket_listen_fn;

/* DNS API */

ZEND_API extern zend_async_getnameinfo_t zend_async_getnameinfo_fn;
ZEND_API extern zend_async_getaddrinfo_t zend_async_getaddrinfo_fn;
ZEND_API extern zend_async_freeaddrinfo_t zend_async_freeaddrinfo_fn;

/* Exec API */
ZEND_API extern zend_async_new_exec_event_t zend_async_new_exec_event_fn;
ZEND_API extern zend_async_exec_t zend_async_exec_fn;

/* Thread pool API */
ZEND_API bool zend_async_thread_pool_is_enabled(void);
ZEND_API extern zend_async_queue_task_t zend_async_queue_task_fn;

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
);

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
);

ZEND_API void zend_async_thread_pool_register(
	zend_string *module, bool allow_override, zend_async_queue_task_t queue_task_fn
);

ZEND_API bool zend_async_socket_listening_register(
	char *module,
	bool allow_override,
	zend_async_socket_listen_t socket_listen_fn
);

ZEND_API zend_string* zend_coroutine_gen_info(zend_coroutine_t *coroutine, char *zend_coroutine_name);

ZEND_API zend_async_event_callback_t * zend_async_event_callback_new(zend_async_event_callback_fn callback, size_t size);

#define ZEND_ASYNC_EVENT_CALLBACK(callback) zend_async_event_callback_new(callback, 0)
#define ZEND_ASYNC_EVENT_CALLBACK_EX(callback, size) zend_async_event_callback_new(callback, size)

ZEND_API zend_coroutine_event_callback_t * zend_async_coroutine_callback_new(
	zend_coroutine_t * coroutine, zend_async_event_callback_fn callback, size_t size
);

/* Waker API */
ZEND_API zend_async_waker_t *zend_async_waker_new(zend_coroutine_t *coroutine);
ZEND_API zend_async_waker_t * zend_async_waker_new_with_timeout(
	zend_coroutine_t * coroutine, const zend_ulong timeout, zend_async_event_t *cancellation
);
ZEND_API bool zend_async_waker_apply_error(
	zend_async_waker_t *waker, zend_object *error, bool transfer_error, bool override, bool for_cancellation
);
ZEND_API void zend_async_waker_destroy(zend_coroutine_t *coroutine);
ZEND_API void zend_async_waker_add_triggered_event(zend_coroutine_t *coroutine, zend_async_event_t *event);

#define ZEND_ASYNC_WAKER_APPLY_ERROR(waker, error, transfer) zend_async_waker_apply_error((waker), (error), (transfer), true, false)
#define ZEND_ASYNC_WAKER_APPEND_ERROR(waker, error, transfer) zend_async_waker_apply_error((waker), (error), (transfer), false, false)
#define ZEND_ASYNC_WAKER_APPLY_CANCELLATION(waker, error, transfer) zend_async_waker_apply_error((waker), (error), (transfer), true, true)

ZEND_API void zend_async_resume_when(
		zend_coroutine_t			*coroutine,
		zend_async_event_t			*event,
		const bool					trans_event,
		zend_async_event_callback_fn callback,
		zend_coroutine_event_callback_t *event_callback
	);

ZEND_API void zend_async_waker_callback_resolve(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
);

ZEND_API void zend_async_waker_callback_cancel(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
);

ZEND_API void zend_async_waker_callback_timeout(
	zend_async_event_t *event, zend_async_event_callback_t *callback, void * result, zend_object *exception
);

/* Coroutine Switch Handlers API */
ZEND_API uint32_t zend_coroutine_add_switch_handler(
	zend_coroutine_t *coroutine,
	zend_coroutine_switch_handler_fn handler
);

ZEND_API bool zend_coroutine_remove_switch_handler(
	zend_coroutine_t *coroutine,
	uint32_t handler_index
);

ZEND_API void zend_coroutine_call_switch_handlers(
	zend_coroutine_t *coroutine,
	bool is_enter,
	bool is_finishing
);

ZEND_API void zend_coroutine_switch_handlers_init(zend_coroutine_t *coroutine);
ZEND_API void zend_coroutine_switch_handlers_destroy(zend_coroutine_t *coroutine);

/* Global Main Coroutine Switch Handlers API */
ZEND_API void zend_async_add_main_coroutine_start_handler(
	zend_coroutine_switch_handler_fn handler
);

ZEND_API void zend_async_call_main_coroutine_start_handlers(zend_coroutine_t *main_coroutine);

END_EXTERN_C()

#define ZEND_ASYNC_IS_ENABLED() zend_async_is_enabled()
#define ZEND_ASYNC_SPAWN() zend_async_spawn_fn(NULL, NULL)
#define ZEND_ASYNC_SPAWN_WITH(scope) zend_async_spawn_fn(scope, NULL)
#define ZEND_ASYNC_SPAWN_WITH_PROVIDER(scope_provider) zend_async_spawn_fn(NULL, scope_provider)
#define ZEND_ASYNC_NEW_COROUTINE(scope) zend_async_new_coroutine_fn(scope)
#define ZEND_ASYNC_NEW_SCOPE(parent) zend_async_new_scope_fn(parent)
#define ZEND_ASYNC_SUSPEND() zend_async_suspend_fn(false)
#define ZEND_ASYNC_RUN_SCHEDULER_AFTER_MAIN() zend_async_suspend_fn(true)
#define ZEND_ASYNC_ENQUEUE_COROUTINE(coroutine) zend_async_enqueue_coroutine_fn(coroutine)
#define ZEND_ASYNC_RESUME(coroutine) zend_async_resume_fn(coroutine, NULL, false)
#define ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, error, transfer_error) zend_async_resume_fn(coroutine, error, transfer_error)
#define ZEND_ASYNC_CANCEL(coroutine, error, transfer_error) zend_async_cancel_fn(coroutine, error, transfer_error, false)
#define ZEND_ASYNC_CANCEL_EX(coroutine, error, transfer_error, is_safely) zend_async_cancel_fn(coroutine, error, transfer_error, is_safely)
#define ZEND_ASYNC_SHUTDOWN() zend_async_shutdown_fn()
#define ZEND_ASYNC_GET_COROUTINES() zend_async_get_coroutines_fn()
#define ZEND_ASYNC_ADD_MICROTASK(microtask) zend_async_add_microtask_fn(microtask)
#define ZEND_ASYNC_GET_AWAITING_INFO(coroutine) zend_async_get_awaiting_info_fn(coroutine)
#define ZEND_ASYNC_GET_CE(type) zend_async_get_class_ce_fn(type)
#define ZEND_ASYNC_GET_EXCEPTION_CE(type) zend_async_get_class_ce_fn(type)

#define ZEND_ASYNC_REACTOR_IS_ENABLED() zend_async_reactor_is_enabled()
#define ZEND_ASYNC_REACTOR_STARTUP() zend_async_reactor_startup_fn()
#define ZEND_ASYNC_REACTOR_SHUTDOWN() zend_async_reactor_shutdown_fn()

#define ZEND_ASYNC_REACTOR_EXECUTE(no_wait) zend_async_reactor_execute_fn(no_wait)
#define ZEND_ASYNC_REACTOR_LOOP_ALIVE() zend_async_reactor_loop_alive_fn()

#define ZEND_ASYNC_NEW_SOCKET_EVENT(socket, events) zend_async_new_socket_event_fn(socket, events, 0)
#define ZEND_ASYNC_NEW_SOCKET_EVENT_EX(socket, events, extra_size) zend_async_new_socket_event_fn(socket, events, extra_size)
#define ZEND_ASYNC_NEW_POLL_EVENT(fh, socket, events) zend_async_new_poll_event_fn(fh, socket, events, 0)
#define ZEND_ASYNC_NEW_POLL_EVENT_EX(fh, socket, events, extra_size) zend_async_new_poll_event_fn(fh, socket, events, extra_size)
#define ZEND_ASYNC_NEW_TIMER_EVENT(timeout, is_periodic) zend_async_new_timer_event_fn(timeout, 0, is_periodic, 0)
#define ZEND_ASYNC_NEW_TIMER_EVENT_EX(timeout, is_periodic, extra_size) zend_async_new_timer_event_fn(timeout, 0, is_periodic, extra_size)
#define ZEND_ASYNC_NEW_TIMER_EVENT_NS(timeout, nanoseconds, is_periodic) zend_async_new_timer_event_fn(timeout, nanoseconds, is_periodic, 0)
#define ZEND_ASYNC_NEW_TIMER_EVENT_NS_EX(timeout, nanoseconds, is_periodic, extra_size) zend_async_new_timer_event_fn(timeout, nanoseconds, is_periodic, extra_size)
#define ZEND_ASYNC_NEW_SIGNAL_EVENT(signum) zend_async_new_signal_event_fn(signum, 0)
#define ZEND_ASYNC_NEW_SIGNAL_EVENT_EX(signum, extra_size) zend_async_new_signal_event_fn(signum, extra_size)
#define ZEND_ASYNC_NEW_PROCESS_EVENT(process_handle) zend_async_new_process_event_fn(process_handle, 0)
#define ZEND_ASYNC_NEW_PROCESS_EVENT_EX(process_handle, extra_size) zend_async_new_process_event_fn(process_handle, extra_size)
#define ZEND_ASYNC_NEW_THREAD_EVENT(entry, arg) zend_async_new_thread_event_fn(entry, arg, 0)
#define ZEND_ASYNC_NEW_THREAD_EVENT_EX(entry, arg) zend_async_new_thread_event_fn(entry, arg, 0)
#define ZEND_ASYNC_NEW_FILESYSTEM_EVENT(path, flags) zend_async_new_filesystem_event_fn(path, flags, 0)
#define ZEND_ASYNC_NEW_FILESYSTEM_EVENT_EX(path, flags, extra_size) zend_async_new_filesystem_event_fn(path, flags, extra_size)

#define ZEND_ASYNC_GETNAMEINFO(addr, flags) zend_async_getnameinfo_fn(addr, flags, 0)
#define ZEND_ASYNC_GETNAMEINFO_EX(addr, flags, extra_size) zend_async_getnameinfo_fn(addr, flags, extra_size)
#define ZEND_ASYNC_GETADDRINFO(node, service, hints) zend_async_getaddrinfo_fn(node, service, hints, 0)
#define ZEND_ASYNC_GETADDRINFO_EX(node, service, hints, extra_size) zend_async_getaddrinfo_fn(node, service, hints, extra_size)
#define ZEND_ASYNC_FREEADDRINFO(ai) zend_async_freeaddrinfo_fn(ai)

#define ZEND_ASYNC_NEW_EXEC_EVENT(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env) \
	zend_async_new_exec_event_fn(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env, 0)
#define ZEND_ASYNC_NEW_EXEC_EVENT_EX(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env, extra_size) \
	zend_async_new_exec_event_fn(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env, extra_size)
#define ZEND_ASYNC_EXEC(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env, timeout) \
	zend_async_exec_fn(exec_mode, cmd, return_buffer, return_value, std_error, cwd, env, timeout)

#define ZEND_ASYNC_QUEUE_TASK(task) zend_async_queue_task_fn(task)

/* Socket Listening API Macros */
#define ZEND_ASYNC_SOCKET_LISTEN(host, port, backlog) \
	zend_async_socket_listen_fn(host, port, backlog, 0)
#define ZEND_ASYNC_SOCKET_LISTEN_EX(host, port, backlog, extra_size) \
	zend_async_socket_listen_fn(host, port, backlog, extra_size)

/* Context API Macros */
#define ZEND_ASYNC_NEW_CONTEXT(parent) zend_async_new_context_fn(parent)
#define ZEND_ASYNC_CURRENT_CONTEXT (ZEND_ASYNC_G(coroutine) != NULL ? ZEND_ASYNC_G(coroutine)->scope->context : NULL)
#define ZEND_ASYNC_GET_COROUTINE_CONTEXT() \
	((ZEND_ASYNC_G(coroutine)) ? \
		(ZEND_ASYNC_G(coroutine)->context ? \
			ZEND_ASYNC_G(coroutine)->context : \
			(ZEND_ASYNC_G(coroutine)->context = ZEND_ASYNC_NEW_CONTEXT(NULL))) \
		: NULL)

/* Internal Context API Macros */
#define ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC(key_name) zend_async_internal_context_key_alloc(key_name)
#define ZEND_ASYNC_INTERNAL_CONTEXT_KEY_NAME(key) zend_async_internal_context_key_name(key)
#define ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coro, key) zend_async_internal_context_find(coro, key)
#define ZEND_ASYNC_INTERNAL_CONTEXT_SET(coro, key, value) zend_async_internal_context_set(coro, key, value)
#define ZEND_ASYNC_INTERNAL_CONTEXT_UNSET(coro, key) zend_async_internal_context_unset(coro, key)

/* Coroutine Switch Handlers API Macros */
#define ZEND_COROUTINE_ADD_SWITCH_HANDLER(coroutine, handler) zend_coroutine_add_switch_handler(coroutine, handler)

#define ZEND_COROUTINE_ENTER(coroutine) zend_coroutine_call_switch_handlers(coroutine, true, false);
#define ZEND_COROUTINE_LEAVE(coroutine) zend_coroutine_call_switch_handlers(coroutine, false, false)
#define ZEND_COROUTINE_FINISH(coroutine) zend_coroutine_call_switch_handlers(coroutine, false, true)

/* Global Main Coroutine Switch Handlers API Macros */
#define ZEND_ASYNC_ADD_MAIN_COROUTINE_START_HANDLER(handler) zend_async_add_main_coroutine_start_handler(handler)

#endif //ZEND_ASYNC_API_H