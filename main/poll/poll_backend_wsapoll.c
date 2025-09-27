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

#ifdef PHP_WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct {
	php_poll_fd_table *fd_table;
	WSAPOLLFD *temp_fds;
	int temp_fds_capacity;
} wsapoll_backend_data_t;

static uint32_t wsapoll_events_to_native(uint32_t events)
{
	uint32_t native = 0;
	if (events & PHP_POLL_READ) {
		native |= POLLRDNORM | POLLRDBAND;
	}
	if (events & PHP_POLL_WRITE) {
		native |= POLLWRNORM;
	}
	if (events & PHP_POLL_ERROR) {
		native |= POLLERR;
	}
	if (events & PHP_POLL_HUP) {
		native |= POLLHUP;
	}
	return native;
}

static uint32_t wsapoll_events_from_native(uint32_t native)
{
	uint32_t events = 0;
	if (native & (POLLRDNORM | POLLRDBAND)) {
		events |= PHP_POLL_READ;
	}
	if (native & POLLWRNORM) {
		events |= PHP_POLL_WRITE;
	}
	if (native & POLLERR) {
		events |= PHP_POLL_ERROR;
	}
	if (native & POLLHUP) {
		events |= PHP_POLL_HUP;
	}
	if (native & POLLNVAL) {
		events |= PHP_POLL_ERROR; /* Map invalid socket to error */
	}
	return events;
}

static zend_result wsapoll_backend_init(php_poll_ctx *ctx)
{
	wsapoll_backend_data_t *data
			= php_poll_calloc(1, sizeof(wsapoll_backend_data_t), ctx->persistent);
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

	data->temp_fds = php_poll_calloc(initial_capacity, sizeof(WSAPOLLFD), ctx->persistent);
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

static void wsapoll_backend_cleanup(php_poll_ctx *ctx)
{
	wsapoll_backend_data_t *data = (wsapoll_backend_data_t *) ctx->backend_data;
	if (data) {
		php_poll_fd_table_cleanup(data->fd_table);
		pefree(data->temp_fds, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result wsapoll_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	wsapoll_backend_data_t *backend_data = (wsapoll_backend_data_t *) ctx->backend_data;

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

static zend_result wsapoll_backend_modify(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	wsapoll_backend_data_t *backend_data = (wsapoll_backend_data_t *) ctx->backend_data;

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

static zend_result wsapoll_backend_remove(php_poll_ctx *ctx, int fd)
{
	wsapoll_backend_data_t *backend_data = (wsapoll_backend_data_t *) ctx->backend_data;

	if (!php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	php_poll_fd_table_remove(backend_data->fd_table, fd);
	return SUCCESS;
}

/* Context for building WSAPOLLFD array */
typedef struct {
	WSAPOLLFD *fds;
	int index;
} wsapoll_build_context;

/* Callback to build WSAPOLLFD array from fd_table */
static bool wsapoll_build_fds_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	wsapoll_build_context *ctx = (wsapoll_build_context *) user_data;

	ctx->fds[ctx->index].fd = (SOCKET) fd;
	ctx->fds[ctx->index].events
			= (SHORT) wsapoll_events_to_native(entry->events & ~(PHP_POLL_ET | PHP_POLL_ONESHOT));
	ctx->fds[ctx->index].revents = 0;
	ctx->index++;

	return true;
}

static int wsapoll_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	wsapoll_backend_data_t *backend_data = (wsapoll_backend_data_t *) ctx->backend_data;

	int fd_count = php_poll_fd_table_count(backend_data->fd_table);
	if (fd_count == 0) {
		if (timeout > 0) {
			Sleep(timeout);
		}
		return 0;
	}

	/* Ensure temp_fds array is large enough */
	if (fd_count > backend_data->temp_fds_capacity) {
		WSAPOLLFD *new_fds = php_poll_realloc(
				backend_data->temp_fds, fd_count * sizeof(WSAPOLLFD), ctx->persistent);
		if (!new_fds) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->temp_fds = new_fds;
		backend_data->temp_fds_capacity = fd_count;
	}

	/* Build WSAPOLLFD array from fd_table */
	wsapoll_build_context build_ctx = { .fds = backend_data->temp_fds, .index = 0 };
	php_poll_fd_table_foreach(backend_data->fd_table, wsapoll_build_fds_callback, &build_ctx);

	/* Call WSAPoll */
	int nfds = WSAPoll(backend_data->temp_fds, fd_count, timeout);

	if (nfds == SOCKET_ERROR) {
		/* WSAPoll specific error handling */
		int wsa_error = WSAGetLastError();
		php_poll_error error_code;

		switch (wsa_error) {
			case WSAENOTSOCK:
				/* Special case: all sockets in array are invalid
				 * WSAPoll fails entirely, but we should clean up and return 0
				 * This differs from Unix poll() which would report POLLNVAL per socket */

				/* Remove all invalid sockets from fd_table */
				for (int i = 0; i < fd_count; i++) {
					int fd = (int) backend_data->temp_fds[i].fd;
					php_poll_fd_table_remove(backend_data->fd_table, fd);
				}
				return 0;
			case WSAENOBUFS:
				error_code = PHP_POLL_ERR_NOMEM;
				break;
			case WSAEINVAL:
			case WSAEFAULT:
				error_code = PHP_POLL_ERR_INVALID;
				break;
			default:
				error_code = PHP_POLL_ERR_SYSTEM;
				break;
		}

		php_poll_set_error(ctx, error_code);
		return -1;
	}

	if (nfds == 0) {
		return 0; /* Timeout */
	}

	int event_count = 0;
	for (int i = 0; i < fd_count && event_count < max_events; i++) {
		WSAPOLLFD *pfd = &backend_data->temp_fds[i];

		if (pfd->revents != 0) {
			int fd = (int) pfd->fd;
			php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
			if (entry) {
				/* WSAPoll-specific handling of POLLNVAL */
				if (pfd->revents & POLLNVAL) {
					php_poll_fd_table_remove(backend_data->fd_table, fd);
					continue; /* Do not report this event */
				}

				/* Convert WSAPoll events to PHP poll events */
				uint32_t converted_events = wsapoll_events_from_native(pfd->revents);

				events[event_count].fd = fd;
				events[event_count].events = entry->events;
				events[event_count].revents = converted_events;
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

static bool wsapoll_backend_is_available(void)
{
	/* Always available on the currently supported Windows versions. */
	return true;
}

static int wsapoll_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	wsapoll_backend_data_t *backend_data = (wsapoll_backend_data_t *) ctx->backend_data;

	if (UNEXPECTED(!backend_data || !backend_data->fd_table)) {
		return -1;
	}

	int active_fds = php_poll_fd_table_count(backend_data->fd_table);
	return active_fds == 0 ? 1 : active_fds;
}

const php_poll_backend_ops php_poll_backend_wsapoll_ops = {
	.type = PHP_POLL_BACKEND_WSAPOLL,
	.name = "wsapoll",
	.init = wsapoll_backend_init,
	.cleanup = wsapoll_backend_cleanup,
	.add = wsapoll_backend_add,
	.modify = wsapoll_backend_modify,
	.remove = wsapoll_backend_remove,
	.wait = wsapoll_backend_wait,
	.is_available = wsapoll_backend_is_available,
	.get_suitable_max_events = wsapoll_backend_get_suitable_max_events,
	.supports_et = false,
};

#endif /* PHP_WIN32 */
