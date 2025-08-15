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

#ifdef HAVE_KQUEUE

typedef struct {
	int kqueue_fd;
	struct kevent *events;
	struct kevent *change_list;
	int change_count;
	int change_capacity;
} kqueue_backend_data_t;

static int kqueue_backend_init(php_poll_ctx *ctx, int max_events)
{
	kqueue_backend_data_t *data = calloc(1, sizeof(kqueue_backend_data_t));
	if (!data) {
		return PHP_POLL_NOMEM;
	}

	data->kqueue_fd = kqueue();
	if (data->kqueue_fd == -1) {
		free(data);
		return PHP_POLL_ERROR;
	}

	data->events = calloc(max_events, sizeof(struct kevent));
	data->change_list = calloc(max_events * 2, sizeof(struct kevent)); /* Read + Write */
	data->change_capacity = max_events * 2;
	data->change_count = 0;

	if (!data->events || !data->change_list) {
		close(data->kqueue_fd);
		free(data->events);
		free(data->change_list);
		free(data);
		return PHP_POLL_NOMEM;
	}

	ctx->backend_data = data;
	return PHP_POLL_OK;
}

static void kqueue_backend_cleanup(php_poll_ctx *ctx)
{
	kqueue_backend_data_t *data = (kqueue_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->kqueue_fd >= 0) {
			close(data->kqueue_fd);
		}
		free(data->events);
		free(data->change_list);
		free(data);
		ctx->backend_data = NULL;
	}
}

static int kqueue_add_change(
		kqueue_backend_data_t *data, int fd, int16_t filter, uint16_t flags, void *udata)
{
	if (data->change_count >= data->change_capacity) {
		return PHP_POLL_NOMEM;
	}

	struct kevent *kev = &data->change_list[data->change_count++];
	EV_SET(kev, fd, filter, flags, 0, 0, udata);
	return PHP_POLL_OK;
}

static int kqueue_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	uint16_t flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		flags |= EV_CLEAR; /* kqueue edge-triggering */
	}

	int result = PHP_POLL_OK;

	if (events & PHP_POLL_READ) {
		result = kqueue_add_change(backend_data, fd, EVFILT_READ, flags, data);
		if (result != PHP_POLL_OK) {
			return result;
		}
	}

	if (events & PHP_POLL_WRITE) {
		result = kqueue_add_change(backend_data, fd, EVFILT_WRITE, flags, data);
		if (result != PHP_POLL_OK) {
			return result;
		}
	}

	return PHP_POLL_OK;
}

static int kqueue_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	/* For kqueue, we delete and re-add */
	kqueue_backend_remove(ctx, fd);
	return kqueue_backend_add(ctx, fd, events, data);
}

static int kqueue_backend_remove(php_poll_ctx *ctx, int fd)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	/* Add delete operations for both read and write filters */
	kqueue_add_change(backend_data, fd, EVFILT_READ, EV_DELETE, NULL);
	kqueue_add_change(backend_data, fd, EVFILT_WRITE, EV_DELETE, NULL);

	return PHP_POLL_OK;
}

static int kqueue_backend_wait(
		php_poll_ctx *ctx, php_poll_event_t *events, int max_events, int timeout)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct timespec ts = { 0 }, *tsp = NULL;
	if (timeout >= 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tsp = &ts;
	}

	/* Apply pending changes and wait for events */
	int nfds = kevent(backend_data->kqueue_fd, backend_data->change_list,
			backend_data->change_count, backend_data->events, max_events, tsp);

	backend_data->change_count = 0; /* Reset change list */

	if (nfds > 0) {
		for (int i = 0; i < nfds; i++) {
			events[i].fd = (int) backend_data->events[i].ident;
			events[i].events = 0;
			events[i].data = backend_data->events[i].udata;
			events[i].revents = 0;

			if (backend_data->events[i].filter == EVFILT_READ) {
				events[i].revents |= PHP_POLL_READ;
			} else if (backend_data->events[i].filter == EVFILT_WRITE) {
				events[i].revents |= PHP_POLL_WRITE;
			}

			if (backend_data->events[i].flags & EV_EOF) {
				events[i].revents |= PHP_POLL_HUP;
			}
			if (backend_data->events[i].flags & EV_ERROR) {
				events[i].revents |= PHP_POLL_ERROR;
			}
		}
	}

	return nfds;
}

static bool kqueue_backend_is_available(void)
{
	int fd = kqueue();
	if (fd >= 0) {
		close(fd);
		return true;
	}
	return false;
}

const php_poll_backend_ops php_poll_backend_kqueue_ops = {
	.name = "kqueue",
	.init = kqueue_backend_init,
	.cleanup = kqueue_backend_cleanup,
	.add = kqueue_backend_add,
	.modify = kqueue_backend_modify,
	.remove = kqueue_backend_remove,
	.wait = kqueue_backend_wait,
	.is_available = kqueue_backend_is_available,
	.supports_et = true /* kqueue supports EV_CLEAR for edge triggering */
};

#endif /* HAVE_KQUEUE */
