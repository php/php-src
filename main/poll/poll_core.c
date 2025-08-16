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

#include "php_poll_internal.h"

/* Backend registry */
static const php_poll_backend_ops *registered_backends[16];
static int num_registered_backends = 0;

/* Forward declarations for backend ops */

#ifdef HAVE_POLL
extern const php_poll_backend_ops php_poll_backend_poll_ops;
#endif
#ifdef HAVE_EPOLL
extern const php_poll_backend_ops php_poll_backend_epoll_ops;
#endif
#ifdef HAVE_KQUEUE
extern const php_poll_backend_ops php_poll_backend_kqueue_ops;
#endif
#ifdef HAVE_EVENT_PORTS
extern const php_poll_backend_ops php_poll_backend_eventport_ops;
#endif
#ifdef _WIN32
extern const php_poll_backend_ops php_poll_backend_iocp_ops;
#endif
extern const php_poll_backend_ops php_poll_backend_select_ops;

/* Register all available backends */
PHPAPI void php_poll_register_backends(void)
{
	num_registered_backends = 0;

#ifdef _WIN32
	/* IOCP is preferred on Windows for high performance */
	if (php_poll_backend_iocp_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_iocp_ops;
	}
#endif

#ifdef HAVE_EVENT_PORTS
	/* Event Ports are preferred on Solaris */
	if (php_poll_backend_eventport_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_eventport_ops;
	}
#endif

#ifdef HAVE_KQUEUE
	if (php_poll_backend_kqueue_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_kqueue_ops;
	}
#endif

#ifdef HAVE_EPOLL
	if (php_poll_backend_epoll_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_epoll_ops;
	}
#endif

#ifdef HAVE_POLL
	/* Poll is available on Unix-like systems */
	registered_backends[num_registered_backends++] = &php_poll_backend_poll_ops;
#endif

	/* select() as a fallback */
	if (php_poll_backend_select_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_select_ops;
	}
}

/* Get backend operations */
const php_poll_backend_ops *php_poll_get_backend_ops(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		/* Return the first (best) available backend */
		return num_registered_backends > 0 ? registered_backends[0] : NULL;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i] && registered_backends[i]->type == backend) {
			return registered_backends[i];
		}
	}

	return NULL;
}

/* Find FD entry */
php_poll_fd_entry *php_poll_find_fd_entry(php_poll_ctx *ctx, int fd)
{
	for (int i = 0; i < ctx->fd_entries_size; i++) {
		if (ctx->fd_entries[i].active && ctx->fd_entries[i].fd == fd) {
			return &ctx->fd_entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
static php_poll_fd_entry *php_poll_get_fd_entry(php_poll_ctx *ctx, int fd)
{
	php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, fd);
	if (entry) {
		return entry;
	}

	/* Find empty slot */
	for (int i = 0; i < ctx->fd_entries_size; i++) {
		if (!ctx->fd_entries[i].active) {
			ctx->fd_entries[i].fd = fd;
			ctx->fd_entries[i].active = true;
			ctx->fd_entries[i].last_revents = 0;
			ctx->fd_entries[i].et_armed = true;
			return &ctx->fd_entries[i];
		}
	}

	return NULL;
}

/* Edge-trigger simulation */
static int php_poll_simulate_et(php_poll_ctx *ctx, php_poll_event *events, int nfds)
{
	if (!ctx->simulate_et) {
		return nfds; /* No simulation needed */
	}

	int filtered_count = 0;

	for (int i = 0; i < nfds; i++) {
		php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, events[i].fd);
		if (!entry) {
			continue;
		}

		uint32_t new_events = events[i].revents;
		uint32_t edge_events = 0;

		/* Detect edges for each event type */
		if ((new_events & PHP_POLL_READ) && !(entry->last_revents & PHP_POLL_READ)) {
			edge_events |= PHP_POLL_READ;
		}
		if ((new_events & PHP_POLL_WRITE) && !(entry->last_revents & PHP_POLL_WRITE)) {
			edge_events |= PHP_POLL_WRITE;
		}

		/* Always report error and hangup events */
		edge_events |= (new_events & (PHP_POLL_ERROR | PHP_POLL_HUP | PHP_POLL_RDHUP));

		entry->last_revents = new_events;

		/* Only include this event if we detected an edge or it's an error */
		if (edge_events != 0) {
			if (filtered_count != i) {
				events[filtered_count] = events[i];
			}
			events[filtered_count].revents = edge_events;
			filtered_count++;
		}
	}

	return filtered_count;
}

/* Create new poll context */
PHPAPI php_poll_ctx *php_poll_create(
		int max_events, php_poll_backend_type preferred_backend, bool persistent)
{
	if (max_events <= 0) {
		return NULL;
	}

	php_poll_ctx *ctx = pecalloc(1, sizeof(php_poll_ctx), persistent);
	if (!ctx) {
		return NULL;
	}
	ctx->persistent = persistent;

	/* Get backend operations */
	ctx->backend_ops = php_poll_get_backend_ops(preferred_backend);
	if (!ctx->backend_ops) {
		pefree(ctx, persistent);
		return NULL;
	}

	ctx->max_events = max_events;
	ctx->backend_type = preferred_backend;
	ctx->simulate_et = !ctx->backend_ops->supports_et;

	/* Allocate FD entries for edge-trigger simulation */
	ctx->fd_entries = pecalloc(max_events, sizeof(php_poll_fd_entry), persistent);
	ctx->fd_entries_size = max_events;
	if (!ctx->fd_entries) {
		pefree(ctx, persistent);
		return NULL;
	}

	return ctx;
}

/* Initialize poll context */
PHPAPI zend_result php_poll_init(php_poll_ctx *ctx)
{
	if (UNEXPECTED(ctx->initialized)) {
		return SUCCESS;
	}

	/* Initialize backend */
	if (EXPECTED(ctx->backend_ops->init(ctx, ctx->max_events) == SUCCESS)) {
		ctx->initialized = true;
		return SUCCESS;
	}

	php_poll_set_system_error_if_not_set(ctx);
	return FAILURE;
}

/* Destroy poll context */
PHPAPI void php_poll_destroy(php_poll_ctx *ctx)
{
	if (!ctx) {
		return;
	}

	if (ctx->backend_ops && ctx->backend_ops->cleanup) {
		ctx->backend_ops->cleanup(ctx);
	}

	pefree(ctx->fd_entries, ctx->persistent);
	pefree(ctx, ctx->persistent);
}

/* Add file descriptor */
PHPAPI zend_result php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	if (UNEXPECTED(ctx->num_fds >= ctx->max_events)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Get FD entry for tracking */
	php_poll_fd_entry *entry = php_poll_get_fd_entry(ctx, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = data;

	/* If simulating edge triggering, convert ET events to level-triggered */
	uint32_t backend_events = events;
	if (ctx->simulate_et && (events & PHP_POLL_ET)) {
		backend_events &= ~PHP_POLL_ET; /* Remove ET flag for backend */
	}

	if (EXPECTED(ctx->backend_ops->add(ctx, fd, backend_events, data) == SUCCESS)) {
		ctx->num_fds++;
		return SUCCESS;
	}

	entry->active = false; /* Rollback */
	php_poll_set_system_error_if_not_set(ctx);

	return FAILURE;
}

/* Modify file descriptor */
PHPAPI zend_result php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, fd);
	if (UNEXPECTED(!entry)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	entry->events = events;
	entry->data = data;
	entry->et_armed = true; /* Re-arm edge triggering */

	uint32_t backend_events = events;
	if (ctx->simulate_et && (events & PHP_POLL_ET)) {
		backend_events &= ~PHP_POLL_ET;
	}

	if (EXPECTED(ctx->backend_ops->modify(ctx, fd, backend_events, data) == SUCCESS)) {
		return SUCCESS;
	}

	php_poll_set_system_error_if_not_set(ctx);
	return FAILURE;
}

/* Remove file descriptor */
PHPAPI zend_result php_poll_remove(php_poll_ctx *ctx, int fd)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || fd < 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, fd);
	if (UNEXPECTED(!entry)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	if (EXPECTED(ctx->backend_ops->remove(ctx, fd) == SUCCESS)) {
		entry->active = false;
		ctx->num_fds--;
	}

	php_poll_set_system_error_if_not_set(ctx);
	return FAILURE;
}

/* Wait for events */
PHPAPI int php_poll_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	if (UNEXPECTED(!ctx || !ctx->initialized || !events || max_events <= 0)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return -1;
	}

	int nfds = ctx->backend_ops->wait(ctx, events, max_events, timeout);

	if (nfds > 0 && ctx->simulate_et) {
		nfds = php_poll_simulate_et(ctx, events, nfds);
	}

	if (UNEXPECTED(nfds < 0)) {
		php_poll_set_system_error_if_not_set(ctx);
	}

	return nfds;
}

/* Get backend name */
PHPAPI const char *php_poll_backend_name(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops ? ctx->backend_ops->name : "unknown";
}

/* Get backend type */
PHPAPI php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx)
{
	return ctx ? ctx->backend_type : PHP_POLL_BACKEND_AUTO;
}

/* Check edge-triggering support */
PHPAPI bool php_poll_supports_et(php_poll_ctx *ctx)
{
	return ctx && (ctx->backend_ops->supports_et || ctx->simulate_et);
}

/* Error retrieval */
PHPAPI php_poll_error php_poll_get_error(php_poll_ctx *ctx)
{
	return ctx ? ctx->last_error : PHP_POLL_ERR_INVALID;
}
