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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_POLL_H
#define PHP_POLL_H

#include "Zend/zend_hrtime.h"

#include "php.h"
#include "php_network.h"

#include <time.h>
#include <errno.h>
#include <signal.h>
#ifndef PHP_WIN32
# include <sys/types.h>
#endif

/* ----- Public generic API ----- */

/* clang-format off */

/* Event types. Keep in sync with io_poll.stub.php! */
#define PHP_POLL_READ    0x01
#define PHP_POLL_WRITE   0x02
#define PHP_POLL_ERROR   0x04
#define PHP_POLL_HUP     0x08
#define PHP_POLL_RDHUP   0x10
#define PHP_POLL_ONESHOT 0x20
#define PHP_POLL_ET      0x40 /* Edge-triggered */
#define PHP_POLL_PRI     0x80 /* Priority data, backend dependent */
#define PHP_POLL_TIMER   0x100
#define PHP_POLL_NOTIFY  0x200
#define PHP_POLL_SIGNAL  0x400
#define PHP_POLL_PROCESS 0x800 /* The child exited */

/* Poll flags */
#define PHP_POLL_FLAG_PERSISTENT 0x01
#define PHP_POLL_FLAG_RAW_EVENTS 0x02

/* Poll backend types. Keep in sync with io_poll.stub.php! */
typedef enum php_poll_backend_type {
	PHP_POLL_BACKEND_AUTO = -1,
	PHP_POLL_BACKEND_POLL = 0,
	PHP_POLL_BACKEND_EPOLL,
	PHP_POLL_BACKEND_KQUEUE,
	PHP_POLL_BACKEND_EVENTPORT,
	PHP_POLL_BACKEND_WSAPOLL
} php_poll_backend_type;

/* Error code constants for exception codes */
#define PHP_POLL_ERROR_CODE_NONE        0
#define PHP_POLL_ERROR_CODE_SYSTEM      1
#define PHP_POLL_ERROR_CODE_NOMEM       2
#define PHP_POLL_ERROR_CODE_INVALID     3
#define PHP_POLL_ERROR_CODE_EXISTS      4
#define PHP_POLL_ERROR_CODE_NOTFOUND    5
#define PHP_POLL_ERROR_CODE_TIMEOUT     6
#define PHP_POLL_ERROR_CODE_INTERRUPTED 7
#define PHP_POLL_ERROR_CODE_PERMISSION  8
#define PHP_POLL_ERROR_CODE_TOOBIG      9
#define PHP_POLL_ERROR_CODE_AGAIN       10
#define PHP_POLL_ERROR_CODE_NOSUPPORT   11

/* Error codes */
typedef enum php_poll_error {
	PHP_POLL_ERR_NONE        = PHP_POLL_ERROR_CODE_NONE,        /* No error */
	PHP_POLL_ERR_SYSTEM      = PHP_POLL_ERROR_CODE_SYSTEM,      /* Generic system error */
	PHP_POLL_ERR_NOMEM       = PHP_POLL_ERROR_CODE_NOMEM,       /* Out of memory (ENOMEM) */
	PHP_POLL_ERR_INVALID     = PHP_POLL_ERROR_CODE_INVALID,     /* Invalid argument (EINVAL, EBADF) */
	PHP_POLL_ERR_EXISTS      = PHP_POLL_ERROR_CODE_EXISTS,      /* Already exists (EEXIST) */
	PHP_POLL_ERR_NOTFOUND    = PHP_POLL_ERROR_CODE_NOTFOUND,    /* Not found (ENOENT) */
	PHP_POLL_ERR_TIMEOUT     = PHP_POLL_ERROR_CODE_TIMEOUT,     /* Operation timed out (ETIME, ETIMEDOUT) */
	PHP_POLL_ERR_INTERRUPTED = PHP_POLL_ERROR_CODE_INTERRUPTED, /* Interrupted by signal (EINTR) */
	PHP_POLL_ERR_PERMISSION  = PHP_POLL_ERROR_CODE_PERMISSION,  /* Permission denied (EACCES, EPERM) */
	PHP_POLL_ERR_TOOBIG      = PHP_POLL_ERROR_CODE_TOOBIG,      /* Too many resources (EMFILE, ENFILE) */
	PHP_POLL_ERR_AGAIN       = PHP_POLL_ERROR_CODE_AGAIN,       /* Try again (EAGAIN, EWOULDBLOCK) */
	PHP_POLL_ERR_NOSUPPORT   = PHP_POLL_ERROR_CODE_NOSUPPORT,   /* Not supported (ENOSYS, EOPNOTSUPP) */
} php_poll_error;

/* clang-format on */

/* Poll event structure */
struct php_poll_event {
	int fd; /* File descriptor */
	uint32_t events; /* Requested events */
	uint32_t revents; /* Returned events */
	void *data; /* User data pointer */
};

/* Forward declarations */
typedef struct php_poll_ctx php_poll_ctx;
typedef struct php_poll_backend_ops php_poll_backend_ops;
typedef struct php_poll_event php_poll_event;

PHPAPI bool php_poll_is_backend_available(php_poll_backend_type backend);
PHPAPI bool php_poll_backend_supports_edge_triggering(php_poll_backend_type backend);
PHPAPI bool php_poll_backend_supports_priority(php_poll_backend_type backend);
/* Whether ProcessHandle and SignalHandle have a native source */
PHPAPI bool php_poll_backend_supports_process_handles(php_poll_backend_type backend);
PHPAPI bool php_poll_backend_supports_signal_handles(php_poll_backend_type backend);

PHPAPI php_poll_ctx *php_poll_create(php_poll_backend_type preferred_backend, uint32_t flags);
PHPAPI php_poll_ctx *php_poll_create_by_name(const char *preferred_backend, uint32_t flags);

PHPAPI zend_result php_poll_set_max_events_hint(php_poll_ctx *ctx, int max_events);
PHPAPI zend_result php_poll_init(php_poll_ctx *ctx);
PHPAPI void php_poll_destroy(php_poll_ctx *ctx);

PHPAPI zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
PHPAPI zend_result php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
PHPAPI zend_result php_poll_remove(php_poll_ctx *ctx, int fd);

PHPAPI int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events,
		const struct timespec *timeout);

/* Timers are reported by php_poll_wait() as events with fd -1 and PHP_POLL_TIMER, before the
 * descriptor events. Deadlines are zend_hrtime() values. A one-shot timer (period 0) stays
 * registered but disarmed after it fired; a periodic one re-arms from its previous deadline. */
typedef struct php_poll_timer php_poll_timer;

PHPAPI php_poll_timer *php_poll_timer_add(php_poll_ctx *ctx, zend_hrtime_t deadline,
		zend_hrtime_t period, void *data);
PHPAPI zend_result php_poll_timer_modify(php_poll_ctx *ctx, php_poll_timer *timer,
		zend_hrtime_t deadline, zend_hrtime_t period, void *data);
PHPAPI void php_poll_timer_remove(php_poll_ctx *ctx, php_poll_timer *timer);
/* Armed timers */
PHPAPI uint32_t php_poll_timer_count(php_poll_ctx *ctx);

/* On Windows a set is a bit per CRT signal number, like ior_sigset_t */
#ifdef PHP_WIN32
typedef struct php_sigset_t { uint32_t bits; } php_sigset_t;
typedef struct php_siginfo_t { int si_signo; int si_code; } php_siginfo_t;
# define PHP_NSIG NSIG
static zend_always_inline int php_sigemptyset(php_sigset_t *set)
{
	set->bits = 0;
	return 0;
}
static zend_always_inline int php_sigaddset(php_sigset_t *set, int signo)
{
	if (signo < 1 || signo >= 32) {
		_set_errno(EINVAL);
		return -1;
	}
	set->bits |= 1u << signo;
	return 0;
}
static zend_always_inline int php_sigdelset(php_sigset_t *set, int signo)
{
	if (signo < 1 || signo >= 32) {
		_set_errno(EINVAL);
		return -1;
	}
	set->bits &= ~(1u << signo);
	return 0;
}
static zend_always_inline int php_sigismember(const php_sigset_t *set, int signo)
{
	if (signo < 1 || signo >= 32) {
		_set_errno(EINVAL);
		return -1;
	}
	return (set->bits >> signo) & 1;
}
#else
typedef sigset_t php_sigset_t;
typedef siginfo_t php_siginfo_t;
# define PHP_NSIG NSIG
# define php_sigemptyset sigemptyset
# define php_sigaddset sigaddset
# define php_sigdelset sigdelset
# define php_sigismember sigismember
#endif

#ifndef PHP_WIN32
/* A descriptor readable when the child exited or a signal of the set is pending: a pidfd or a
 * signalfd, or a private kqueue. -1 with ENOSYS where there is none, ESRCH for a process that is
 * not a waitable child. It neither reaps the child nor consumes the signal. */
PHPAPI bool php_poll_has_process_source(void);
PHPAPI bool php_poll_has_signal_source(void);
PHPAPI int php_poll_process_source_open(pid_t pid);
PHPAPI int php_poll_signal_source_open(const sigset_t *set);
/* Never waits, 0 when nothing is pending; fd -1 takes from the pending set */
PHPAPI int php_poll_signal_source_take(int fd, const sigset_t *set, siginfo_t *info);
/* Never waits, 0 when nothing is pending */
PHPAPI int php_poll_signal_take_pending(const sigset_t *set, siginfo_t *info);
#endif

PHPAPI const char *php_poll_backend_name(php_poll_ctx *ctx);
PHPAPI php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx);
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx);
PHPAPI bool php_poll_supports_priority(php_poll_ctx *ctx);
PHPAPI php_poll_error php_poll_get_error(php_poll_ctx *ctx);

/* Get suitable max_events for backend */
PHPAPI int php_poll_get_suitable_max_events(php_poll_ctx *ctx);

/* Backend registration */
PHPAPI void php_poll_register_backends(void);

/* Error string for the error */
PHPAPI const char *php_poll_error_string(php_poll_error error);

/* ----- Public extension API ----- */

typedef struct php_poll_handle_ops php_poll_handle_ops;
typedef struct php_poll_handle_object php_poll_handle_object;

/* Handle operations structure - extensions can provide their own */
struct php_poll_handle_ops {
	/**
	 * Get file descriptor for this handle
	 * @return File descriptor or SOCK_ERR if invalid/not applicable
	 */
	php_socket_t (*get_fd)(php_poll_handle_object *handle);

	/**
	 * Check if handle is still valid
	 * @return true if valid, false if invalid
	 */
	int (*is_valid)(php_poll_handle_object *handle);

	/**
	 * Cleanup handle-specific data
	 */
	void (*cleanup)(php_poll_handle_object *handle);

	/**
	 * The event a NOTIFY, SIGNAL or PROCESS handle reports, watched as READ on its descriptor.
	 * Zero for descriptor handles.
	 */
	uint32_t event;

	/**
	 * Consumes the source of such a handle once it is readable. May be NULL.
	 * @return whether the event is reported
	 */
	bool (*fired)(php_poll_handle_object *handle);
};

/* A script may reach the resource through the handle: set by the userland factory, or by the core
 * for a stream a script holds; sticky */
#define PHP_POLL_HANDLE_F_EXPOSED 0x01

/* Base poll handle object structure */
struct php_poll_handle_object {
	php_poll_handle_ops *ops;
	void *handle_data;
	HashTable *watching; /* context key -> watcher, not refcounted */
	uint32_t flags; /* PHP_POLL_HANDLE_F_* */
	struct _php_io_registration *registrations; /* IO hooks registrations on this handle */
	zend_object std;
};

#define PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(obj) ZEND_CONTAINER_OF(obj, php_poll_handle_object, std)

#define PHP_POLL_HANDLE_OBJ_FROM_ZV(zv) PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(Z_OBJ_P(zv))

/* Default operations */
extern php_poll_handle_ops php_poll_handle_default_ops;

/* Utility functions for extensions */
PHPAPI php_poll_handle_object *php_poll_handle_object_create(
		size_t obj_size, zend_class_entry *ce, php_poll_handle_ops *ops);
PHPAPI void php_poll_handle_object_free(zend_object *obj);

/* Get file descriptor from any poll handle */
PHPAPI php_socket_t php_poll_handle_get_fd(php_poll_handle_object *handle);

#endif /* PHP_POLL_H */
