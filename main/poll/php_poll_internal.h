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

#include "php_poll.h"

/* Poll event structure */
struct php_poll_event {
	int fd; /* File descriptor */
	uint32_t events; /* Requested events */
	uint32_t revents; /* Returned events */
	void *data; /* User data pointer */
};

/* FD entry for tracking state */
struct php_poll_fd_entry {
	int fd;
	uint32_t events;
	uint32_t last_revents; /* For edge-trigger simulation */
	void *data;
	bool active;
	bool et_armed; /* Edge-trigger state */
};

/* Backend interface */
typedef struct php_poll_backend_ops {
	php_poll_backend_type type;
	const char *name;

	/* Initialize backend */
	zend_result (*init)(php_poll_ctx *ctx, int max_events);

	/* Cleanup backend */
	void (*cleanup)(php_poll_ctx *ctx);

	/* Add file descriptor */
	zend_result (*add)(php_poll_ctx *ctx, int fd, uint32_t events, void *data);

	/* Modify file descriptor */
	zend_result (*modify)(php_poll_ctx *ctx, int fd, uint32_t events, void *data);

	/* Remove file descriptor */
	zend_result (*remove)(php_poll_ctx *ctx, int fd);

	/* Wait for events */
	int (*wait)(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout);

	/* Check if backend is available */
	bool (*is_available)(void);

	/* Backend supports edge triggering natively */
	bool supports_et;
} php_poll_backend_ops;

/* Main poll context */
struct php_poll_ctx {
	const php_poll_backend_ops *backend_ops;
	php_poll_backend_type backend_type;

	int max_events;
	int num_fds;
	bool initialized;
	bool persistent;
	/* Whether to simulate edge triggering */
	bool simulate_et;

	/* FD tracking for edge-trigger simulation */
	php_poll_fd_entry *fd_entries;
	int fd_entries_size;

	/* Last error */
	php_poll_error last_error;

	/* Backend-specific data */
	void *backend_data;
};

php_poll_fd_entry *php_poll_find_fd_entry(php_poll_ctx *ctx, int fd);

static inline void php_poll_set_error(php_poll_ctx *ctx, php_poll_error error)
{
	ctx->last_error = error;
}

static inline void php_poll_set_system_error_if_not_set(php_poll_ctx *ctx)
{
	if (ctx->last_error == PHP_POLL_ERR_NONE) {
		ctx->last_error = PHP_POLL_ERR_SYSTEM;
	}
}
