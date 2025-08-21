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
	int events_capacity;
} kqueue_backend_data_t;

static zend_result kqueue_backend_init(php_poll_ctx *ctx)
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

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;
	data->events = pecalloc(initial_capacity, sizeof(struct kevent), ctx->persistent);
	if (!data->events) {
		close(data->kqueue_fd);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->events_capacity = initial_capacity;

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
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result kqueue_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct kevent changes[2]; /* Max 2 changes: read + write */
	int change_count = 0;

	uint16_t flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		flags |= EV_CLEAR;
	}

	if (events & PHP_POLL_READ) {
		EV_SET(&changes[change_count], fd, EVFILT_READ, flags, 0, 0, data);
		change_count++;
	}

	if (events & PHP_POLL_WRITE) {
		EV_SET(&changes[change_count], fd, EVFILT_WRITE, flags, 0, 0, data);
		change_count++;
	}

	if (change_count > 0) {
		int result = kevent(backend_data->kqueue_fd, changes, change_count, NULL, 0, NULL);
		if (result == -1) {
			switch (errno) {
				case EEXIST:
					php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
					break;
				case ENOMEM:
					php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
					break;
				case EBADF:
				case EINVAL:
					php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
					break;
				default:
					php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
					break;
			}
			return FAILURE;
		}
	}

	return SUCCESS;
}

static zend_result kqueue_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct kevent deletes[2];
	struct kevent adds[2];
	int delete_count = 0;
	int add_count = 0;

	uint16_t add_flags = EV_ADD | EV_ENABLE;
	if (events & PHP_POLL_ONESHOT) {
		add_flags |= EV_ONESHOT;
	}
	if (events & PHP_POLL_ET) {
		add_flags |= EV_CLEAR;
	}

	/* Delete existing filters that are not in the new events */
	if (!(events & PHP_POLL_READ)) {
		EV_SET(&deletes[delete_count], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		delete_count++;
	}
	if (!(events & PHP_POLL_WRITE)) {
		EV_SET(&deletes[delete_count], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		delete_count++;
	}

	/* Prepare add operations for requested events */
	if (events & PHP_POLL_READ) {
		EV_SET(&adds[add_count], fd, EVFILT_READ, add_flags, 0, 0, data);
		add_count++;
	}
	if (events & PHP_POLL_WRITE) {
		EV_SET(&adds[add_count], fd, EVFILT_WRITE, add_flags, 0, 0, data);
		add_count++;
	}

	/* Delete existing filters (ignore ENOENT errors) */
	if (delete_count > 0) {
		int result = kevent(backend_data->kqueue_fd, deletes, delete_count, NULL, 0, NULL);
		if (result == -1 && errno != ENOENT) {
			php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
			return FAILURE;
		}
	}

	/* Add new filters */
	if (add_count > 0) {
		int result = kevent(backend_data->kqueue_fd, adds, add_count, NULL, 0, NULL);
		if (result == -1) {
			switch (errno) {
				case ENOENT:
					php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
					break;
				case ENOMEM:
					php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
					break;
				case EBADF:
				case EINVAL:
					php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
					break;
				default:
					php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
					break;
			}
			return FAILURE;
		}
	}

	return SUCCESS;
}

static zend_result kqueue_backend_remove(php_poll_ctx *ctx, int fd)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	struct kevent changes[2];
	EV_SET(&changes[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	EV_SET(&changes[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	int result = kevent(backend_data->kqueue_fd, changes, 2, NULL, 0, NULL);
	if (result == -1 && errno != ENOENT) {
		switch (errno) {
			case EBADF:
			case EINVAL:
				php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
				break;
			default:
				php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
				break;
		}
		return FAILURE;
	}

	return SUCCESS;
}

static int kqueue_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	kqueue_backend_data_t *backend_data = (kqueue_backend_data_t *) ctx->backend_data;

	/* Ensure we have enough space for the requested events */
	if (max_events > backend_data->events_capacity) {
		struct kevent *new_events = perealloc(
				backend_data->events, max_events * sizeof(struct kevent), ctx->persistent);
		if (!new_events) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->events = new_events;
		backend_data->events_capacity = max_events;
	}

	struct timespec ts = { 0 }, *tsp = NULL;
	if (timeout >= 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tsp = &ts;
	}

	int nfds = kevent(backend_data->kqueue_fd, NULL, 0, backend_data->events, max_events, tsp);

	if (nfds > 0) {
		for (int i = 0; i < nfds; i++) {
			events[i].fd = (int) backend_data->events[i].ident;
			events[i].events = 0; /* Not used in results */
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
