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
#include "php_network.h"

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

	/* Get suitable max_events for this backend */
	int (*get_suitable_max_events)(php_poll_ctx *ctx);

	/* Backend supports edge triggering natively */
	bool supports_et;
} php_poll_backend_ops;

/* Main poll context */
struct php_poll_ctx {
	const php_poll_backend_ops *backend_ops;
	php_poll_backend_type backend_type;
	php_poll_error last_error;

	/* Optional capacity hint for backends */
	int max_events_hint;

	/* Flags */
	uint32_t initialized : 1;
	uint32_t persistent : 1;
	uint32_t raw_events : 1;

	/* Backend-specific data */
	void *backend_data;
};

/* Generic FD entry structure */
typedef struct php_poll_fd_entry {
	int fd;
	uint32_t events;
	void *data;
	bool active;
	uint32_t last_revents;
} php_poll_fd_entry;

/* FD tracking table */
typedef struct php_poll_fd_table {
	HashTable entries_ht;
	bool persistent;
} php_poll_fd_table;

/* Iterator callback function type */
typedef bool (*php_poll_fd_iterator_func_t)(int fd, php_poll_fd_entry *entry, void *user_data);

/* Poll FD helpers - clean API with accessor functions */
php_poll_fd_table *php_poll_fd_table_init(int initial_capacity, bool persistent);
void php_poll_fd_table_cleanup(php_poll_fd_table *table);
php_poll_fd_entry *php_poll_fd_table_find(php_poll_fd_table *table, int fd);
php_poll_fd_entry *php_poll_fd_table_get(php_poll_fd_table *table, int fd);
void php_poll_fd_table_remove(php_poll_fd_table *table, int fd);

/* Accessor functions for table properties */
static inline int php_poll_fd_table_count(php_poll_fd_table *table)
{
	return zend_hash_num_elements(&table->entries_ht);
}

static inline bool php_poll_fd_table_is_empty(php_poll_fd_table *table)
{
	return zend_hash_num_elements(&table->entries_ht) == 0;
}

/* New helper functions for improved backend integration */
void php_poll_fd_table_foreach(
		php_poll_fd_table *table, php_poll_fd_iterator_func_t callback, void *user_data);
php_socket_t php_poll_fd_table_get_max_fd(php_poll_fd_table *table);
int php_poll_fd_table_collect_events(
		php_poll_fd_table *table, php_poll_event *events, int max_events);

/* Error helper functions */
php_poll_error php_poll_errno_to_error(int err);

static inline void php_poll_set_errno_error(php_poll_ctx *ctx, int err)
{
	ctx->last_error = php_poll_errno_to_error(err);
}

static inline void php_poll_set_current_errno_error(php_poll_ctx *ctx)
{
	php_poll_set_errno_error(ctx, errno);
}

static inline bool php_poll_is_not_found_error(void)
{
	return errno == ENOENT;
}

static inline bool php_poll_is_timeout_error(void)
{
#if defined(ETIME) && defined(ETIMEDOUT)
	return errno == ETIME || errno == ETIMEDOUT;
#elif defined(ETIME)
	return errno == ETIME;
#elif defined(ETIMEDOUT)
	return errno = ETIMEDOUT;
#else
	return false;
#endif
}


static inline void php_poll_set_error(php_poll_ctx *ctx, php_poll_error error)
{
	ctx->last_error = error;
}

#endif /* PHP_POLL_INTERNAL_H */
