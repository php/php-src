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

typedef struct {
	php_poll_fd_table *fd_table;
	php_pollfd *temp_fds;
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
	if (native & POLLNVAL) {
		events |= PHP_POLL_ERROR; /* Map invalid FD to error */
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

	data->temp_fds = php_poll_calloc(initial_capacity, sizeof(php_pollfd), ctx->persistent);
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

	if (events & PHP_POLL_ET) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

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

	if (events & PHP_POLL_ET) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;

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

/* Context for building php_pollfd array */
typedef struct {
	php_pollfd *fds;
	int index;
} poll_build_context;

/* Callback to build php_pollfd array from fd_table */
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

static void php_poll_msleep(int timeout_ms)
{
	if (timeout_ms <= 0) {
		return;
	}

#ifdef PHP_WIN32
	Sleep(timeout_ms);
#else
	struct timespec ts;
	ts.tv_sec = timeout_ms / 1000;
	ts.tv_nsec = (timeout_ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

static int poll_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	int fd_count = php_poll_fd_table_count(backend_data->fd_table);
	if (fd_count == 0) {
		if (timeout > 0) {
			php_poll_msleep(timeout);
		}
		return 0;
	}

	/* Ensure temp_fds array is large enough */
	if (fd_count > backend_data->temp_fds_capacity) {
		php_pollfd *new_fds = php_poll_realloc(
				backend_data->temp_fds, fd_count * sizeof(php_pollfd), ctx->persistent);
		if (!new_fds) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->temp_fds = new_fds;
		backend_data->temp_fds_capacity = fd_count;
	}

	/* Build php_pollfd array from fd_table */
	poll_build_context build_ctx = { .fds = backend_data->temp_fds, .index = 0 };
	php_poll_fd_table_foreach(backend_data->fd_table, poll_build_fds_callback, &build_ctx);

	/* Call poll() or its emulation (Windows) */
	int nfds = php_poll2(backend_data->temp_fds, fd_count, timeout);

	if (nfds <= 0) {
		return nfds; /* Return 0 for timeout, -1 for error */
	}

	/* Process results - iterate through php_pollfd array directly */
	int event_count = 0;
	for (int i = 0; i < fd_count && event_count < max_events; i++) {
		php_pollfd *pfd = &backend_data->temp_fds[i];

		if (pfd->revents != 0) {
			php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, pfd->fd);
			if (entry) {
				/* Handle POLLNVAL by automatically removing the invalid FD */
				if (pfd->revents & POLLNVAL) {
					php_poll_fd_table_remove(backend_data->fd_table, pfd->fd);
					continue; /* Don't report this event */
				}

				events[event_count].fd = pfd->fd;
				events[event_count].events = entry->events;
				events[event_count].revents = poll_events_from_native(pfd->revents);
				events[event_count].data = entry->data;
				event_count++;
			}
		}
	}

	/* Handle oneshot removals */
	for (int i = 0; i < event_count; i++) {
		php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, events[i].fd);
		if (entry && (entry->events & PHP_POLL_ONESHOT) && events[i].revents != 0) {
			php_poll_fd_table_remove(backend_data->fd_table, events[i].fd);
		}
	}

	return event_count;
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
