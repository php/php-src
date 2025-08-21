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

#ifndef PHP_POLL_INTERNAL_H
#define PHP_POLL_INTERNAL_H

#include "php_poll.h"

/* Allocation macros */
#define php_poll_calloc(nmemb, size, persistent) \
	((persistent) ? calloc((nmemb), (size)) : ecalloc((nmemb), (size)))
#define php_poll_malloc(size, persistent) ((persistent) ? malloc((size)) : emalloc((size)))
#define php_poll_realloc(ptr, size, persistent) \
	((persistent) ? realloc((ptr), (size)) : erealloc((ptr), (size)))

/* Backend interface */
typedef struct php_poll_backend_ops {
	php_poll_backend_type type;
	const char *name;

	/* Initialize backend */
	zend_result (*init)(php_poll_ctx *ctx);

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

	bool initialized;
	bool persistent;

	/* Optional capacity hint for backends */
	int max_events_hint;

	/* Last error */
	php_poll_error last_error;

	/* Backend-specific data */
	void *backend_data;
};

/* Generic FD entry structure */
typedef struct php_poll_fd_entry {
	int fd;
	uint32_t events;
	void *data;
	bool active;
	uint32_t last_revents; /* For edge-trigger simulation */
} php_poll_fd_entry;

/* FD tracking table */
typedef struct php_poll_fd_table {
	php_poll_fd_entry *entries;
	int capacity;
	int count;
	bool persistent;
} php_poll_fd_table;

/* Poll FD helpers */
php_poll_fd_table *php_poll_fd_table_init(int initial_capacity, bool persistent);
void php_poll_fd_table_cleanup(php_poll_fd_table *table);
php_poll_fd_entry *php_poll_fd_table_find(php_poll_fd_table *table, int fd);
php_poll_fd_entry *php_poll_fd_table_get(php_poll_fd_table *table, int fd);
void php_poll_fd_table_remove(php_poll_fd_table *table, int fd);
int php_poll_simulate_edge_trigger(php_poll_fd_table *table, php_poll_event *events, int nfds);

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

#endif /* PHP_POLL_INTERNAL_H */
