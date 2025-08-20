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

#ifdef HAVE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
typedef struct {
	int kqueue_fd;
	struct kevent *events;
	struct kevent *change_list;
	int change_count;
	int change_capacity;
	int events_capacity;
} kqueue_backend_data_t;

static zend_result kqueue_backend_init(php_poll_ctx *ctx, int max_events)
{
	kqueue_backend_data_t *data = pecalloc(1, sizeof(kqueue_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	data->kqueue_fd = kqueue();
	if (data->kqueue_fd == -1) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	int initial_events = (max_events > 0) ? max_events : 64;
	int initial_changes = max_events * 2;

	data->events = pecalloc(initial_events, sizeof(struct kevent), ctx->persistent);
	data->change_list = pecalloc(initial_changes, sizeof(struct kevent), ctx->persistent);
	data->events_capacity = initial_events;
	data->change_capacity = initial_changes;
	data->change_count = 0;

	if (!data->events || !data->change_list) {
		close(data->kqueue_fd);
		pefree(data->events, ctx->persistent);
		pefree(data->change_list, ctx->persistent);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	ctx->backend_data = data;
	return SUCCESS;
}

static void kqueue_backend_cleanup(php_poll_ctx *ctx)
{
	kqueue_backend_data_t *data = (kqueue_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->kqueue_fd >= 0) {
			close(data->kqueue_fd);
		}
		pefree(data->events, ctx->persistent);
		pefree(data->change_list, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

/* Helper function to find existing change for same (ident, filter) */
static int find_existing_change(kqueue_backend_data_t *data, int fd, int16_t filter)
{
	for (int i = 0; i < data->change_count; i++) {
		if (data->change_list[i].ident == (uintptr_t) fd && data->change_list[i].filter == filter) {
			return i;
		}
	}
	return -1;
}

/* Helper function to grow change list capacity */
static zend_result grow_change_list(php_poll_ctx *ctx, kqueue_backend_data_t *data)
{
	int new_capacity = data->change_capacity * 2;
	struct kevent *new_list
			= perealloc(data->change_list, new_capacity * sizeof(struct kevent), ctx->persistent);
	if (!new_list) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	data->change_list = new_list;
	data->change_capacity = new_capacity;
	return SUCCESS;
}

static zend_result kqueue_add_change(php_poll_ctx *ctx, kqueue_backend_data_t *data, int fd,
		int16_t filter, uint16_t flags, void *udata)
{
	int existing_idx = find_existing_change(data, fd, filter);

	if (existing_idx >= 0) {
		/* Update existing change */
		struct kevent *existing = &data->change_list[existing_idx];
		EV_SET(existing, fd, filter, flags, 0, 0, udata);
		return SUCCESS;
	}

	if (data->change_count >= data->change_capacity) {
		zend_result result = grow_change_list(ctx, data);
		if (result != SUCCESS) {
			return result;
		}
	}

	/* Set new change */
	struct kevent *kev = &data->change_list[data->change_count++];
	EV_SET(kev, fd, filter, flags, 0, 0, udata);
	return SUCCESS;
}

static zend_result kqueue_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	uint16_t flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		flags |= EV_CLEAR;
	}

	zend_result result = SUCCESS;

	if (events & PHP_POLL_READ) {
		result = kqueue_add_change(ctx, backend_data, fd, EVFILT_READ, flags, data);
		if (result != SUCCESS) {
			return result;
		}
	}

	if (events & PHP_POLL_WRITE) {
		result = kqueue_add_change(ctx, backend_data, fd, EVFILT_WRITE, flags, data);
		if (result != SUCCESS) {
			return result;
		}
	}

	return SUCCESS;
}

static zend_result kqueue_backend_remove(php_poll_ctx *ctx, int fd)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	/* Delete both read and write filters */
	zend_result result = kqueue_add_change(ctx, backend_data, fd, EVFILT_READ, EV_DELETE, NULL);
	if (result != SUCCESS) {
		return result;
	}
	result = kqueue_add_change(ctx, backend_data, fd, EVFILT_WRITE, EV_DELETE, NULL);
	if (result != SUCCESS) {
		return result;
	}

	return SUCCESS;
}

static zend_result kqueue_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	/* For epoll-consistent behavior: completely replace the event mask */
	zend_result result = kqueue_add_change(ctx, backend_data, fd, EVFILT_READ, EV_DELETE, NULL);
	if (result != SUCCESS) {
		return result;
	}

	result = kqueue_add_change(ctx, backend_data, fd, EVFILT_WRITE, EV_DELETE, NULL);
	if (result != SUCCESS) {
		return result;
	}

	/* Add back the requested events (coalescing will handle DELETE -> ADD optimization) */
	return kqueue_backend_add(ctx, fd, events, data);
}

static int kqueue_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	if (max_events > backend_data->events_capacity) {
		int new_capacity = max_events;
		struct kevent *new_events = perealloc(
				backend_data->events, new_capacity * sizeof(struct kevent), ctx->persistent);
		if (!new_events) {
			max_events = backend_data->events_capacity;
		} else {
			backend_data->events = new_events;
			backend_data->events_capacity = new_capacity;
		}
	}

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
	.type = PHP_POLL_BACKEND_KQUEUE,
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
