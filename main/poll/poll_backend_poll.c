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
	int fd;
	uint32_t events;
	void *data;
	bool active;
	uint32_t last_revents; /* For edge-trigger simulation */
} poll_fd_entry;

typedef struct {
	struct pollfd *fds;
	int fds_capacity;
	int fds_used;

	/* FD tracking for user data and edge-trigger simulation */
	poll_fd_entry *fd_entries;
	int fd_entries_capacity;
	int fd_count;
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

/* Find FD entry */
static poll_fd_entry *poll_find_fd_entry(poll_backend_data_t *data, int fd)
{
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (data->fd_entries[i].active && data->fd_entries[i].fd == fd) {
			return &data->fd_entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
static poll_fd_entry *poll_get_fd_entry(poll_backend_data_t *data, int fd, bool persistent)
{
	poll_fd_entry *entry = poll_find_fd_entry(data, fd);
	if (entry) {
		return entry;
	}

	/* Find empty slot */
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (!data->fd_entries[i].active) {
			data->fd_entries[i].fd = fd;
			data->fd_entries[i].active = true;
			data->fd_entries[i].last_revents = 0;
			data->fd_count++;
			return &data->fd_entries[i];
		}
	}

	/* Need to grow the array */
	int new_capacity = data->fd_entries_capacity ? data->fd_entries_capacity * 2 : 64;
	poll_fd_entry *new_entries
			= perealloc(data->fd_entries, new_capacity * sizeof(poll_fd_entry), persistent);
	if (!new_entries) {
		return NULL;
	}

	/* Initialize new entries */
	memset(new_entries + data->fd_entries_capacity, 0,
			(new_capacity - data->fd_entries_capacity) * sizeof(poll_fd_entry));

	data->fd_entries = new_entries;

	/* Use first new slot */
	poll_fd_entry *new_entry = &data->fd_entries[data->fd_entries_capacity];
	new_entry->fd = fd;
	new_entry->active = true;
	new_entry->last_revents = 0;
	data->fd_count++;

	data->fd_entries_capacity = new_capacity;
	return new_entry;
}

/* Remove FD entry */
static void poll_remove_fd_entry(poll_backend_data_t *data, int fd)
{
	poll_fd_entry *entry = poll_find_fd_entry(data, fd);
	if (entry) {
		entry->active = false;
		data->fd_count--;
	}
}

/* Find pollfd slot */
static struct pollfd *poll_find_pollfd_slot(poll_backend_data_t *data, int fd)
{
	for (int i = 0; i < data->fds_capacity; i++) {
		if (data->fds[i].fd == fd) {
			return &data->fds[i];
		}
	}
	return NULL;
}

/* Get empty pollfd slot */
static struct pollfd *poll_get_empty_pollfd_slot(poll_backend_data_t *data)
{
	for (int i = 0; i < data->fds_capacity; i++) {
		if (data->fds[i].fd == -1) {
			return &data->fds[i];
		}
	}
	return NULL;
}

static zend_result poll_backend_init(php_poll_ctx *ctx)
{
	poll_backend_data_t *data = pecalloc(1, sizeof(poll_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;

	data->fds = pecalloc(initial_capacity, sizeof(struct pollfd), ctx->persistent);
	if (!data->fds) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->fds_capacity = initial_capacity;
	data->fds_used = 0;

	/* Initialize FD tracking array */
	data->fd_entries = pecalloc(initial_capacity, sizeof(poll_fd_entry), ctx->persistent);
	if (!data->fd_entries) {
		pefree(data->fds, ctx->persistent);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->fd_entries_capacity = initial_capacity;
	data->fd_count = 0;

	/* Initialize all fds to -1 */
	for (int i = 0; i < initial_capacity; i++) {
		data->fds[i].fd = -1;
	}

	ctx->backend_data = data;
	return SUCCESS;
}

static void poll_backend_cleanup(php_poll_ctx *ctx)
{
	poll_backend_data_t *data = (poll_backend_data_t *) ctx->backend_data;
	if (data) {
		pefree(data->fds, ctx->persistent);
		pefree(data->fd_entries, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result poll_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	/* Check if FD already exists */
	if (poll_find_fd_entry(backend_data, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	/* Get FD entry for tracking */
	poll_fd_entry *entry = poll_get_fd_entry(backend_data, fd, ctx->persistent);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;

	/* Find empty pollfd slot */
	struct pollfd *pfd = poll_get_empty_pollfd_slot(backend_data);
	if (!pfd) {
		/* Need to grow the pollfd array */
		int new_capacity = backend_data->fds_capacity * 2;
		struct pollfd *new_fds = perealloc(
				backend_data->fds, new_capacity * sizeof(struct pollfd), ctx->persistent);
		if (!new_fds) {
			poll_remove_fd_entry(backend_data, fd);
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return FAILURE;
		}

		/* Initialize new slots */
		for (int i = backend_data->fds_capacity; i < new_capacity; i++) {
			new_fds[i].fd = -1;
		}

		backend_data->fds = new_fds;
		pfd = &backend_data->fds[backend_data->fds_capacity];
		backend_data->fds_capacity = new_capacity;
	}

	/* Set up pollfd */
	pfd->fd = fd;
	pfd->events = poll_events_to_native(events & ~(PHP_POLL_ET | PHP_POLL_ONESHOT));
	pfd->revents = 0;
	backend_data->fds_used++;

	return SUCCESS;
}

static zend_result poll_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	/* Find existing entry */
	poll_fd_entry *entry = poll_find_fd_entry(backend_data, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Update entry */
	entry->events = events;
	entry->data = user_data;
	entry->last_revents = 0; /* Reset on modify */

	/* Find pollfd and update */
	struct pollfd *pfd = poll_find_pollfd_slot(backend_data, fd);
	if (pfd) {
		pfd->events = poll_events_to_native(events & ~(PHP_POLL_ET | PHP_POLL_ONESHOT));
	}

	return SUCCESS;
}

static zend_result poll_backend_remove(php_poll_ctx *ctx, int fd)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	/* Find existing entry */
	poll_fd_entry *entry = poll_find_fd_entry(backend_data, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Find and clear pollfd */
	struct pollfd *pfd = poll_find_pollfd_slot(backend_data, fd);
	if (pfd) {
		pfd->fd = -1;
		pfd->events = 0;
		pfd->revents = 0;
		backend_data->fds_used--;
	}

	/* Remove from tracking */
	poll_remove_fd_entry(backend_data, fd);

	return SUCCESS;
}

/* Edge-trigger simulation */
static int poll_simulate_edge_trigger(
		poll_backend_data_t *backend_data, php_poll_event *events, int nfds)
{
	int filtered_count = 0;

	for (int i = 0; i < nfds; i++) {
		poll_fd_entry *entry = poll_find_fd_entry(backend_data, events[i].fd);
		if (!entry) {
			continue;
		}

		uint32_t new_events = events[i].revents;
		uint32_t reported_events = 0;

		if (entry->events & PHP_POLL_ET) {
			/* Edge-triggered: report edges only */
			if ((new_events & PHP_POLL_READ) && !(entry->last_revents & PHP_POLL_READ)) {
				reported_events |= PHP_POLL_READ;
			}
			if ((new_events & PHP_POLL_WRITE) && !(entry->last_revents & PHP_POLL_WRITE)) {
				reported_events |= PHP_POLL_WRITE;
			}
			/* Always report error and hangup events */
			reported_events |= (new_events & (PHP_POLL_ERROR | PHP_POLL_HUP | PHP_POLL_RDHUP));
		} else {
			/* Level-triggered: report all active events */
			reported_events = new_events;
		}

		entry->last_revents = new_events;

		/* Only include this event if we have something to report */
		if (reported_events != 0) {
			if (filtered_count != i) {
				events[filtered_count] = events[i];
			}
			events[filtered_count].revents = reported_events;
			filtered_count++;

			/* Handle oneshot: remove after first event */
			if (entry->events & PHP_POLL_ONESHOT) {
				/* Mark pollfd as disabled */
				struct pollfd *pfd = poll_find_pollfd_slot(backend_data, events[i].fd);
				if (pfd) {
					pfd->fd = -1;
					pfd->events = 0;
					pfd->revents = 0;
					backend_data->fds_used--;
				}
				poll_remove_fd_entry(backend_data, events[i].fd);
			}
		}
	}

	return filtered_count;
}

static int poll_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	poll_backend_data_t *backend_data = (poll_backend_data_t *) ctx->backend_data;

	if (backend_data->fds_used == 0) {
		/* No FDs to monitor, but respect timeout */
		if (timeout > 0) {
			struct timespec ts;
			ts.tv_sec = timeout / 1000;
			ts.tv_nsec = (timeout % 1000) * 1000000;
			nanosleep(&ts, NULL);
		}
		return 0;
	}

	int nfds = poll(backend_data->fds, backend_data->fds_capacity, timeout);

	if (nfds > 0) {
		int event_count = 0;
		for (int i = 0; i < backend_data->fds_capacity && event_count < max_events; i++) {
			if (backend_data->fds[i].fd != -1 && backend_data->fds[i].revents != 0) {
				poll_fd_entry *entry = poll_find_fd_entry(backend_data, backend_data->fds[i].fd);
				if (entry) {
					events[event_count].fd = backend_data->fds[i].fd;
					events[event_count].events = entry->events;
					events[event_count].revents
							= poll_events_from_native(backend_data->fds[i].revents);
					events[event_count].data = entry->data;
					event_count++;
				}

				backend_data->fds[i].revents = 0; /* Clear for next poll */
			}
		}

		/* Apply edge-trigger simulation */
		nfds = poll_simulate_edge_trigger(backend_data, events, event_count);
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
	.supports_et = false, /* poll() doesn't support ET natively, but we simulate it */
};

#endif /* HAVE_POLL */
