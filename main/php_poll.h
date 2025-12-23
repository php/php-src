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

#include "php.h"
#include "php_network.h"

/* ----- Public generic API ----- */

/* clang-format off */

/* Event types */
#define PHP_POLL_READ    0x01
#define PHP_POLL_WRITE   0x02
#define PHP_POLL_ERROR   0x04
#define PHP_POLL_HUP     0x08
#define PHP_POLL_RDHUP   0x10
#define PHP_POLL_ONESHOT 0x20
#define PHP_POLL_ET      0x40 /* Edge-triggered */

/* Poll flags */
#define PHP_POLL_FLAG_PERSISTENT 0x01
#define PHP_POLL_FLAG_RAW_EVENTS 0x02

/* Poll backend types */
typedef enum {
	PHP_POLL_BACKEND_AUTO = -1,
	PHP_POLL_BACKEND_POLL = 0,
	PHP_POLL_BACKEND_EPOLL,
	PHP_POLL_BACKEND_KQUEUE,
	PHP_POLL_BACKEND_EVENTPORT,
	PHP_POLL_BACKEND_WSAPOLL
} php_poll_backend_type;

/* Error codes */
typedef enum {
	PHP_POLL_ERR_NONE,          /* No error */
	PHP_POLL_ERR_SYSTEM,        /* Generic system error */
	PHP_POLL_ERR_NOMEM,         /* Out of memory (ENOMEM) */
	PHP_POLL_ERR_INVALID,       /* Invalid argument (EINVAL, EBADF) */
	PHP_POLL_ERR_EXISTS,        /* Already exists (EEXIST) */
	PHP_POLL_ERR_NOTFOUND,      /* Not found (ENOENT) */
	PHP_POLL_ERR_TIMEOUT,       /* Operation timed out (ETIME, ETIMEDOUT) */
	PHP_POLL_ERR_INTERRUPTED,   /* Interrupted by signal (EINTR) */
	PHP_POLL_ERR_PERMISSION,    /* Permission denied (EACCES, EPERM) */
	PHP_POLL_ERR_TOOBIG,        /* Too many resources (EMFILE, ENFILE) */
	PHP_POLL_ERR_AGAIN,         /* Try again (EAGAIN, EWOULDBLOCK) */
	PHP_POLL_ERR_NOSUPPORT,     /* Not supported (ENOSYS, EOPNOTSUPP) */
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

PHPAPI php_poll_ctx *php_poll_create(php_poll_backend_type preferred_backend, uint32_t flags);
PHPAPI php_poll_ctx *php_poll_create_by_name(const char *preferred_backend, uint32_t flags);

PHPAPI zend_result php_poll_set_max_events_hint(php_poll_ctx *ctx, int max_events);
PHPAPI zend_result php_poll_init(php_poll_ctx *ctx);
PHPAPI void php_poll_destroy(php_poll_ctx *ctx);

PHPAPI zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
PHPAPI zend_result php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data);
PHPAPI zend_result php_poll_remove(php_poll_ctx *ctx, int fd);

PHPAPI int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout);

PHPAPI const char *php_poll_backend_name(php_poll_ctx *ctx);
PHPAPI php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx);
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx);
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
	 * @param handle The handle object
	 * @return File descriptor or SOCK_ERR if invalid/not applicable
	 */
	php_socket_t (*get_fd)(php_poll_handle_object *handle);

	/**
	 * Check if handle is still valid
	 * @param handle The handle object
	 * @return true if valid, false if invalid
	 */
	int (*is_valid)(php_poll_handle_object *handle);

	/**
	 * Cleanup handle-specific data
	 * @param handle The handle object
	 */
	void (*cleanup)(php_poll_handle_object *handle);
};

/* Base poll handle object structure */
struct php_poll_handle_object {
	php_poll_handle_ops *ops;
	void *handle_data;
	zend_object std;
};

#define PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(obj) \
	((php_poll_handle_object *) ((char *) (obj) - XtOffsetOf(php_poll_handle_object, std)))

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
