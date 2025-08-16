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

#ifdef HAVE_EPOLL

#include <sys/epoll.h>

typedef struct {
	int epoll_fd;
	struct epoll_event *events;
} epoll_backend_data_t;

static uint32_t epoll_events_to_native(uint32_t events)
{
	uint32_t native = 0;
	if (events & PHP_POLL_READ) {
		native |= EPOLLIN;
	}
	if (events & PHP_POLL_WRITE) {
		native |= EPOLLOUT;
	}
	if (events & PHP_POLL_ERROR) {
		native |= EPOLLERR;
	}
	if (events & PHP_POLL_HUP) {
		native |= EPOLLHUP;
	}
	if (events & PHP_POLL_RDHUP) {
		native |= EPOLLRDHUP;
	}
	if (events & PHP_POLL_ONESHOT) {
		native |= EPOLLONESHOT;
	}
	if (events & PHP_POLL_ET) {
		native |= EPOLLET;
	}
	return native;
}

static uint32_t epoll_events_from_native(uint32_t native)
{
	uint32_t events = 0;
	if (native & EPOLLIN) {
		events |= PHP_POLL_READ;
	}
	if (native & EPOLLOUT) {
		events |= PHP_POLL_WRITE;
	}
	if (native & EPOLLERR) {
		events |= PHP_POLL_ERROR;
	}
	if (native & EPOLLHUP) {
		events |= PHP_POLL_HUP;
	}
	if (native & EPOLLRDHUP) {
		events |= PHP_POLL_RDHUP;
	}
	return events;
}

static int epoll_backend_init(php_poll_ctx *ctx, int max_events)
{
	epoll_backend_data_t *data = calloc(1, sizeof(epoll_backend_data_t));
	if (!data) {
		return PHP_POLL_ERR_NOMEM;
	}

	data->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (data->epoll_fd == -1) {
		free(data);
		return PHP_POLL_ERROR;
	}

	data->events = calloc(max_events, sizeof(struct epoll_event));
	if (!data->events) {
		close(data->epoll_fd);
		free(data);
		return PHP_POLL_ERR_NOMEM;
	}

	ctx->backend_data = data;
	return PHP_POLL_ERR_NONE;
}

static void epoll_backend_cleanup(php_poll_ctx *ctx)
{
	epoll_backend_data_t *data = (epoll_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->epoll_fd >= 0) {
			close(data->epoll_fd);
		}
		free(data->events);
		free(data);
		ctx->backend_data = NULL;
	}
}

static int epoll_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	epoll_backend_data_t *backend_data = (epoll_backend_data_t *) ctx->backend_data;

	struct epoll_event ev = { 0 };
	ev.events = epoll_events_to_native(events);
	ev.data.ptr = data;

	if (epoll_ctl(backend_data->epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		return (errno == EEXIST) ? PHP_POLL_ERR_EXISTS : PHP_POLL_ERROR;
	}

	return PHP_POLL_ERR_NONE;
}

static int epoll_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	epoll_backend_data_t *backend_data = (epoll_backend_data_t *) ctx->backend_data;

	struct epoll_event ev = { 0 };
	ev.events = epoll_events_to_native(events);
	ev.data.ptr = data;

	if (epoll_ctl(backend_data->epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		return (errno == ENOENT) ? PHP_POLL_ERR_NOTFOUND : PHP_POLL_ERROR;
	}

	return PHP_POLL_ERR_NONE;
}

static int epoll_backend_remove(php_poll_ctx *ctx, int fd)
{
	epoll_backend_data_t *backend_data = (epoll_backend_data_t *) ctx->backend_data;

	if (epoll_ctl(backend_data->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		return (errno == ENOENT) ? PHP_POLL_ERR_NOTFOUND : PHP_POLL_ERROR;
	}

	return PHP_POLL_ERR_NONE;
}

static int epoll_backend_wait(
		php_poll_ctx *ctx, php_poll_event_t *events, int max_events, int timeout)
{
	epoll_backend_data_t *backend_data = (epoll_backend_data_t *) ctx->backend_data;

	int nfds = epoll_wait(backend_data->epoll_fd, backend_data->events, max_events, timeout);

	if (nfds > 0) {
		for (int i = 0; i < nfds; i++) {
			events[i].fd = backend_data->events[i].data.fd;
			events[i].events = 0; /* Not used in results */
			events[i].revents = epoll_events_from_native(backend_data->events[i].events);
			events[i].data = backend_data->events[i].data.ptr;
		}
	}

	return nfds;
}

static bool epoll_backend_is_available(void)
{
	int fd = epoll_create1(EPOLL_CLOEXEC);
	if (fd >= 0) {
		close(fd);
		return true;
	}
	return false;
}

const php_poll_backend_ops php_poll_backend_epoll_ops = { .name = "epoll",
	.init = epoll_backend_init,
	.cleanup = epoll_backend_cleanup,
	.add = epoll_backend_add,
	.modify = epoll_backend_modify,
	.remove = epoll_backend_remove,
	.wait = epoll_backend_wait,
	.is_available = epoll_backend_is_available,
	.supports_et = true };

#endif /* HAVE_EPOLL */
