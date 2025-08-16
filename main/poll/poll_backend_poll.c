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
	struct pollfd *fds;
	int allocated;
	int used;
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

static zend_result poll_backend_init(php_poll_ctx *ctx, int max_events)
{
	poll_backend_data_t *data = calloc(1, sizeof(poll_backend_data_t));
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	data->fds = calloc(max_events, sizeof(struct pollfd));
	if (!data->fds) {
		free(data);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	data->allocated = max_events;
	data->used = 0;

	/* Initialize all fds to -1 */
	for (int i = 0; i < max_events; i++) {
		data->fds[i].fd = -1;
	}

	ctx->backend_data = data;
	return SUCCESS;
}

static void poll_backend_cleanup(php_poll_ctx *ctx)
{
	poll_backend_data_t *data = (poll_backend_data_t *) ctx->backend_data;
	if (data) {
		free(data->fds);
		free(data);
		ctx->backend_data = NULL;
	}
}

static zend_result poll_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	/* Find empty slot */
	for (int i = 0; i < backend_data->allocated; i++) {
		if (backend_data->fds[i].fd == -1) {
			backend_data->fds[i].fd = fd;
			backend_data->fds[i].events = poll_events_to_native(events);
			backend_data->fds[i].revents = 0;
			backend_data->used++;
			return SUCCESS;
		}
	}

	php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
	return FAILURE;
}

static zend_result poll_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	for (int i = 0; i < backend_data->allocated; i++) {
		if (backend_data->fds[i].fd == fd) {
			backend_data->fds[i].events = poll_events_to_native(events);
			return SUCCESS;
		}
	}

	php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
	return FAILURE;
}

static zend_result poll_backend_remove(php_poll_ctx *ctx, int fd)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	for (int i = 0; i < backend_data->allocated; i++) {
		if (backend_data->fds[i].fd == fd) {
			backend_data->fds[i].fd = -1;
			backend_data->fds[i].events = 0;
			backend_data->fds[i].revents = 0;
			backend_data->used--;
			return SUCCESS;
		}
	}

	php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
	return FAILURE;
}

static int poll_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	int nfds = poll(backend_data->fds, backend_data->allocated, timeout);

	if (nfds > 0) {
		int event_count = 0;
		for (int i = 0; i < backend_data->allocated && event_count < max_events; i++) {
			if (backend_data->fds[i].fd != -1 && backend_data->fds[i].revents != 0) {
				events[event_count].fd = backend_data->fds[i].fd;
				events[event_count].events = backend_data->fds[i].events;
				events[event_count].revents = poll_events_from_native(backend_data->fds[i].revents);
				events[event_count].data = NULL; /* poll doesn't support user data directly */

				/* Find the user data from our FD tracking */
				php_poll_fd_entry *entry = php_poll_find_fd_entry(ctx, events[event_count].fd);
				if (entry) {
					events[event_count].data = entry->data;
				}

				backend_data->fds[i].revents = 0; /* Clear for next poll */
				event_count++;
			}
		}
		nfds = event_count;
	}

	return nfds;
}

static bool poll_backend_is_available(void)
{
	return true; /* poll() is always available */
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
	.supports_et = false,
};

#endif /* HAVE_POLL */
