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

#ifdef HAVE_POLL

#include <poll.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	php_poll_fd_table *fd_table;
	/* Temporary arrays allocated during poll_backend_wait() */
	struct pollfd *temp_fds;
	int temp_fds_capacity;
} poll_backend_data_t;

static uint32_t poll_events_to_native(uint32_t events)
{
	uint32_t native = 0;
	if (events & PHP_POLL_READ) {
		native |= POLLIN;
	}
	if (events & PHP_POLL_WRITE) {
		native |= POLLOUT;
	}
	if (events & PHP_POLL_ERROR) {
		native |= POLLERR;
	}
	if (events & PHP_POLL_HUP) {
		native |= POLLHUP;
	}
	return native;
}

static uint32_t poll_events_from_native(uint32_t native)
{
	uint32_t events = 0;
	if (native & POLLIN) {
		events |= PHP_POLL_READ;
	}
	if (native & POLLOUT) {
		events |= PHP_POLL_WRITE;
	}
	if (native & POLLERR) {
		events |= PHP_POLL_ERROR;
	}
	if (native & POLLHUP) {
		events |= PHP_POLL_HUP;
	}
	return events;
}

static zend_result poll_backend_init(php_poll_ctx *ctx)
{
	poll_backend_data_t *data = php_poll_calloc(1, sizeof(poll_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;

	data->fd_table = php_poll_fd_table_init(initial_capacity, ctx->persistent);
	if (!data->fd_table) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Pre-allocate temporary pollfd array */
	data->temp_fds = php_poll_calloc(initial_capacity, sizeof(struct pollfd), ctx->persistent);
	if (!data->temp_fds) {
		php_poll_fd_table_cleanup(data->fd_table);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->temp_fds_capacity = initial_capacity;

	ctx->backend_data = data;
	return SUCCESS;
}

static void poll_backend_cleanup(php_poll_ctx *ctx)
{
	poll_backend_data_t *data = (poll_backend_data_t *) ctx->backend_data;
	if (data) {
		php_poll_fd_table_cleanup(data->fd_table);
		pefree(data->temp_fds, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result poll_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	if (php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_fd_table_get(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;

	return SUCCESS;
}

static zend_result poll_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;
	entry->last_revents = 0;

	return SUCCESS;
}

static zend_result poll_backend_remove(php_poll_ctx *ctx, int fd)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	if (!php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	php_poll_fd_table_remove(backend_data->fd_table, fd);
	return SUCCESS;
}

/* Context for building pollfd array */
typedef struct {
	struct pollfd *fds;
	int index;
} poll_build_context;

/* Callback to build pollfd array from fd_table */
static bool poll_build_fds_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	poll_build_context *ctx = (poll_build_context *) user_data;

	ctx->fds[ctx->index].fd = fd;
	ctx->fds[ctx->index].events
			= poll_events_to_native(entry->events & ~(PHP_POLL_ET | PHP_POLL_ONESHOT));
	ctx->fds[ctx->index].revents = 0;
	ctx->index++;

	return true;
}

/* Context for processing poll results */
typedef struct {
	poll_backend_data_t *backend_data;
	struct pollfd *pollfds;
	php_poll_event *events;
	int max_events;
	int event_count;
} poll_result_context;

/* Callback to process poll results */
static bool poll_process_results_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	poll_result_context *ctx = (poll_result_context *) user_data;

	if (ctx->event_count >= ctx->max_events) {
		return false; /* Stop if events array is full */
	}

	/* Find the corresponding pollfd entry */
	for (int i = 0; i < php_poll_fd_table_count(ctx->backend_data->fd_table); i++) {
		if (ctx->pollfds[i].fd == fd && ctx->pollfds[i].revents != 0) {
			ctx->events[ctx->event_count].fd = fd;
			ctx->events[ctx->event_count].events = entry->events;
			ctx->events[ctx->event_count].revents
					= poll_events_from_native(ctx->pollfds[i].revents);
			ctx->events[ctx->event_count].data = entry->data;
			ctx->event_count++;
			break;
		}
	}

	return true;
}

static int poll_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	int fd_count = php_poll_fd_table_count(backend_data->fd_table);
	if (fd_count == 0) {
		if (timeout > 0) {
			struct timespec ts;
			ts.tv_sec = timeout / 1000;
			ts.tv_nsec = (timeout % 1000) * 1000000;
			nanosleep(&ts, NULL);
		}
		return 0;
	}

	/* Ensure temp_fds array is large enough */
	if (fd_count > backend_data->temp_fds_capacity) {
		struct pollfd *new_fds = php_poll_realloc(
				backend_data->temp_fds, fd_count * sizeof(struct pollfd), ctx->persistent);
		if (!new_fds) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->temp_fds = new_fds;
		backend_data->temp_fds_capacity = fd_count;
	}

	/* Build pollfd array from fd_table */
	poll_build_context build_ctx = { .fds = backend_data->temp_fds, .index = 0 };
	php_poll_fd_table_foreach(backend_data->fd_table, poll_build_fds_callback, &build_ctx);

	/* Call poll() */
	int nfds = poll(backend_data->temp_fds, fd_count, timeout);

	if (nfds > 0) {
		/* Process results */
		poll_result_context result_ctx = { .backend_data = backend_data,
			.pollfds = backend_data->temp_fds,
			.events = events,
			.max_events = max_events,
			.event_count = 0 };

		php_poll_fd_table_foreach(
				backend_data->fd_table, poll_process_results_callback, &result_ctx);
		int event_count = result_ctx.event_count;

		/* Apply edge-trigger simulation */
		nfds = php_poll_simulate_edge_trigger(backend_data->fd_table, events, event_count);

		/* Handle oneshot removals */
		for (int i = 0; i < nfds; i++) {
			php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, events[i].fd);
			if (entry && (entry->events & PHP_POLL_ONESHOT) && events[i].revents != 0) {
				php_poll_fd_table_remove(backend_data->fd_table, events[i].fd);
			}
		}
	}

	return nfds;
}

static bool poll_backend_is_available(void)
{
	return true;
}

static int poll_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	if (UNEXPECTED(!backend_data || !backend_data->fd_table)) {
		return -1;
	}

	int active_fds = php_poll_fd_table_count(backend_data->fd_table);
	return active_fds == 0 ? 1 : active_fds;
}

const php_poll_backend_ops php_poll_backend_poll_ops = {
	.type = PHP_POLL_BACKEND_POLL,
	.name = "poll",
	.init = poll_backend_init,
	.cleanup = poll_backend_cleanup,
	.add = poll_backend_add,
	.modify = poll_backend_modify,
	.remove = poll_backend_remove,
	.wait = poll_backend_wait,
	.is_available = poll_backend_is_available,
	.get_suitable_max_events = poll_backend_get_suitable_max_events,
	.supports_et = false,
};

#endif /* HAVE_POLL */
