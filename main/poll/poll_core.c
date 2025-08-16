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

/* Backend registry */
static const php_poll_backend_ops *registered_backends[16];
static int num_registered_backends = 0;

/* Forward declarations for backend ops */
extern const php_poll_backend_ops php_poll_backend_poll_ops;
#ifdef HAVE_EPOLL
extern const php_poll_backend_ops php_poll_backend_epoll_ops;
#endif
#ifdef HAVE_KQUEUE
extern const php_poll_backend_ops php_poll_backend_kqueue_ops;
#endif
#ifdef HAVE_PORT_H
extern const php_poll_backend_ops php_poll_backend_eventport_ops;
#endif
#ifdef _WIN32
extern const php_poll_backend_ops php_poll_backend_iocp_ops;
extern const php_poll_backend_ops php_poll_backend_select_ops;
#endif

/* Register all available backends */
void php_poll_register_backends(void)
{
	num_registered_backends = 0;

#ifdef _WIN32
	/* IOCP is preferred on Windows for high performance */
	if (php_poll_backend_iocp_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_iocp_ops;
	}
#endif

#ifdef HAVE_PORT_H
	/* Event Ports are preferred on Solaris */
	if (php_poll_backend_eventport_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_eventport_ops;
	}
#endif

#ifdef HAVE_SYS_EPOLL_H
	if (php_poll_backend_epoll_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_epoll_ops;
	}
#endif

#ifdef HAVE_SYS_EVENT_H
	if (php_poll_backend_kqueue_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_kqueue_ops;
	}
#endif

#ifdef _WIN32
	/* Windows select() as fallback before poll() */
	if (php_poll_backend_select_ops.is_available()) {
		registered_backends[num_registered_backends++] = &php_poll_backend_select_ops;
	}
#endif

#ifndef _WIN32
	/* Poll is available on Unix-like systems */
	registered_backends[num_registered_backends++] = &php_poll_backend_poll_ops;
#endif
}

/* Get backend operations */
const php_poll_backend_ops *php_poll_get_backend_ops(php_poll_backend_type backend)
{
	if (backend == PHP_POLL_BACKEND_AUTO) {
		/* Return the first (best) available backend */
		return num_registered_backends > 0 ? registered_backends[0] : NULL;
	}

	for (int i = 0; i < num_registered_backends; i++) {
		if (registered_backends[i]
				&& ((backend == PHP_POLL_BACKEND_EPOLL
							&& strcmp(registered_backends[i]->name, "epoll") == 0)
						|| (backend == PHP_POLL_BACKEND_KQUEUE
								&& strcmp(registered_backends[i]->name, "kqueue") == 0)
						|| (backend == PHP_POLL_BACKEND_EVENTPORT
								&& strcmp(registered_backends[i]->name, "eventport") == 0)
						|| (backend == PHP_POLL_BACKEND_IOCP
								&& strcmp(registered_backends[i]->name, "iocp") == 0)
						|| (backend == PHP_POLL_BACKEND_SELECT
								&& strcmp(registered_backends[i]->name, "select") == 0)
						|| (backend == PHP_POLL_BACKEND_POLL
								&& strcmp(registered_backends[i]->name, "poll") == 0))) {
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
	if (entry)
		return entry;

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
static int php_poll_simulate_et(php_poll_ctx *ctx, php_poll_event_t *events, int nfds)
{
	if (!ctx->simulate_et) {
		return nfds; /* No simulation needed */
	}

	int filtered_count = 0;

	for (int i = 0; i < nfds; i++) {
		php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, events[i].fd);
		if (!entry)
			continue;

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
php_poll_ctx *php_poll_create(int max_events, php_poll_backend_type preferred_backend)
{
	if (max_events <= 0)
		return NULL;

	php_poll_ctx *ctx = calloc(1, sizeof(php_poll_ctx));
	if (!ctx)
		return NULL;

	/* Get backend operations */
	ctx->backend_ops = php_poll_get_backend_ops(preferred_backend);
	if (!ctx->backend_ops) {
		free(ctx);
		return NULL;
	}

	ctx->max_events = max_events;
	ctx->backend_type = preferred_backend;

	/* Allocate FD entries for edge-trigger simulation */
	ctx->fd_entries = calloc(max_events, sizeof(php_poll_fd_entry));
	ctx->fd_entries_size = max_events;
	if (!ctx->fd_entries) {
		free(ctx);
		return NULL;
	}

	/* Initialize backend */
	if (ctx->backend_ops->init(ctx, max_events) != PHP_POLL_ERR_NONE) {
		free(ctx->fd_entries);
		free(ctx);
		return NULL;
	}

	ctx->initialized = true;
	ctx->simulate_et = !ctx->backend_ops->supports_et;

	return ctx;
}

/* Destroy poll context */
void php_poll_destroy(php_poll_ctx *ctx)
{
	if (!ctx)
		return;

	if (ctx->backend_ops && ctx->backend_ops->cleanup) {
		ctx->backend_ops->cleanup(ctx);
	}

	free(ctx->fd_entries);
	free(ctx);
}

/* Add file descriptor */
int php_poll_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (!ctx || !ctx->initialized || fd < 0) {
		return PHP_POLL_ERR_INVALID;
	}

	if (ctx->num_fds >= ctx->max_events) {
		return PHP_POLL_ERR_NOMEM;
	}

	/* Get FD entry for tracking */
	php_poll_fd_entry *entry = php_poll_get_fd_entry(ctx, fd);
	if (!entry) {
		return PHP_POLL_ERR_NOMEM;
	}

	entry->events = events;
	entry->data = data;

	/* If simulating edge triggering, convert ET events to level-triggered */
	uint32_t backend_events = events;
	if (ctx->simulate_et && (events & PHP_POLL_ET)) {
		backend_events &= ~PHP_POLL_ET; /* Remove ET flag for backend */
	}

	int result = ctx->backend_ops->add(ctx, fd, backend_events, data);
	if (result == PHP_POLL_ERR_NONE) {
		ctx->num_fds++;
	} else {
		entry->active = false; /* Rollback */
	}

	return result;
}

/* Modify file descriptor */
int php_poll_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	if (!ctx || !ctx->initialized || fd < 0) {
		return PHP_POLL_ERR_INVALID;
	}

	php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, fd);
	if (!entry) {
		return PHP_POLL_ERR_NOTFOUND;
	}

	entry->events = events;
	entry->data = data;
	entry->et_armed = true; /* Re-arm edge triggering */

	uint32_t backend_events = events;
	if (ctx->simulate_et && (events & PHP_POLL_ET)) {
		backend_events &= ~PHP_POLL_ET;
	}

	return ctx->backend_ops->modify(ctx, fd, backend_events, data);
}

/* Remove file descriptor */
int php_poll_remove(php_poll_ctx *ctx, int fd)
{
	if (!ctx || !ctx->initialized || fd < 0) {
		return PHP_POLL_ERR_INVALID;
	}

	php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, fd);
	if (!entry) {
		return PHP_POLL_ERR_NOTFOUND;
	}

	int result = ctx->backend_ops->remove(ctx, fd);
	if (result == PHP_POLL_ERR_NONE) {
		entry->active = false;
		ctx->num_fds--;
	}

	return result;
}

/* Wait for events */
int php_poll_wait(php_poll_ctx *ctx, php_poll_event_t *events, int max_events, int timeout)
{
	if (!ctx || !ctx->initialized || !events || max_events <= 0) {
		return PHP_POLL_ERR_INVALID;
	}

	int nfds = ctx->backend_ops->wait(ctx, events, max_events, timeout);

	if (nfds > 0 && ctx->simulate_et) {
		nfds = php_poll_simulate_et(ctx, events, nfds);
	}

	return nfds;
}

/* Get backend name */
const char *php_poll_backend_name(php_poll_ctx *ctx)
{
	return ctx && ctx->backend_ops ? ctx->backend_ops->name : "unknown";
}

/* Get backend type */
php_poll_backend_type php_poll_get_backend_type(php_poll_ctx *ctx)
{
	return ctx ? ctx->backend_type : PHP_POLL_BACKEND_AUTO;
}

/* Check edge-triggering support */
bool php_poll_supports_et(php_poll_ctx *ctx)
{
	return ctx && (ctx->backend_ops->supports_et || ctx->simulate_et);
}
