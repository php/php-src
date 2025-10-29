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

#ifdef HAVE_EVENT_PORTS

#include <port.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>

typedef struct {
	int port_fd;
	port_event_t *events;
	int events_capacity;
	int active_associations;
	php_poll_fd_table *fd_table;
} eventport_backend_data_t;

/* Convert our event flags to event port flags */
static int eventport_events_to_native(uint32_t events)
{
	int native = 0;
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
	if (events & PHP_POLL_RDHUP) {
		native |= POLLHUP; /* Map RDHUP to HUP */
	}
	return native;
}

/* Convert event port flags back to our event flags */
static uint32_t eventport_events_from_native(int native)
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
		events |= PHP_POLL_ERROR;
	}
	return events;
}

/* Initialize event port backend */
static zend_result eventport_backend_init(php_poll_ctx *ctx)
{
	eventport_backend_data_t *data
			= php_poll_calloc(1, sizeof(eventport_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Create event port */
	data->port_fd = port_create();
	if (data->port_fd == -1) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	data->active_associations = 0;

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;
	data->events = php_poll_calloc(initial_capacity, sizeof(port_event_t), ctx->persistent);
	if (!data->events) {
		close(data->port_fd);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->events_capacity = initial_capacity;

	/* Initialize FD tracking using helper */
	data->fd_table = php_poll_fd_table_init(initial_capacity, ctx->persistent);
	if (!data->fd_table) {
		close(data->port_fd);
		pefree(data->events, ctx->persistent);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	ctx->backend_data = data;
	return SUCCESS;
}

/* Cleanup event port backend */
static void eventport_backend_cleanup(php_poll_ctx *ctx)
{
	eventport_backend_data_t *data = (eventport_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->port_fd >= 0) {
			close(data->port_fd);
		}
		pefree(data->events, ctx->persistent);
		php_poll_fd_table_cleanup(data->fd_table);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

/* Add file descriptor to event port */
static zend_result eventport_backend_add(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

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

	int native_events = eventport_events_to_native(events);

	/* Associate file descriptor with event port */
	if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events, user_data) == -1) {
		php_poll_fd_table_remove(backend_data->fd_table, fd);
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

	backend_data->active_associations++;
	return SUCCESS;
}

/* Modify file descriptor in event port */
static zend_result eventport_backend_modify(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Update entry */
	entry->events = events;
	entry->data = user_data;

	/* For event ports, we need to dissociate and re-associate */
	/* Note: dissociate might fail if the fd was already fired and auto-dissociated */
	port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd);

	int native_events = eventport_events_to_native(events);
	if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events, user_data) == -1) {
		switch (errno) {
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

	return SUCCESS;
}

/* Remove file descriptor from event port */
static zend_result eventport_backend_remove(php_poll_ctx *ctx, int fd)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	/* Check if exists using helper */
	if (!php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	if (port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd) == -1) {
		/* Only fail if it's not ENOENT (might already be dissociated) */
		if (!php_poll_is_not_found_error()) {
			php_poll_set_current_errno_error(ctx);
			return FAILURE;
		}
	}

	php_poll_fd_table_remove(backend_data->fd_table, fd);
	backend_data->active_associations--;
	return SUCCESS;
}

/* Wait for events using event port */
static int eventport_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	if (backend_data->active_associations == 0) {
		/* No active associations, but we still need to respect timeout */
		if (timeout > 0) {
			struct timespec ts;
			ts.tv_sec = timeout / 1000;
			ts.tv_nsec = (timeout % 1000) * 1000000;
			nanosleep(&ts, NULL);
		}
		return 0;
	}

	/* Ensure we have enough space for the requested events */
	if (max_events > backend_data->events_capacity) {
		port_event_t *new_events = php_poll_realloc(
				backend_data->events, max_events * sizeof(port_event_t), ctx->persistent);
		if (!new_events) {
			php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
			return -1;
		}
		backend_data->events = new_events;
		backend_data->events_capacity = max_events;
	}

	/* Setup timeout structure */
	struct timespec ts = { 0 }, *tsp = NULL;
	if (timeout >= 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tsp = &ts;
	}

	/* Retrieve events from port */
	uint_t nget = 1; /* We want to get multiple events if available */
	int result = port_getn(backend_data->port_fd, backend_data->events, max_events, &nget, tsp);

	if (result == -1) {
		php_poll_set_current_errno_error(ctx);
		return -1;
	}

	int nfds = (int) nget;
	int check_count = 0;

	/* First pass: process events, identify unfired events, and re-associate */
	for (int i = 0; i < nfds; i++) {
		port_event_t *port_event = &backend_data->events[i];

		/* Only handle PORT_SOURCE_FD events */
		if (port_event->portev_source == PORT_SOURCE_FD) {
			int fd = (int) port_event->portev_object;
			uint32_t fired = eventport_events_from_native(port_event->portev_events);

			events[i].fd = fd;
			events[i].events = 0;
			events[i].revents = fired;
			events[i].data = port_event->portev_user;

			/* Get entry and handle re-association */
			php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
			if (entry) {
				/* Check if there are other events we're monitoring */
				uint32_t monitored = entry->events & (PHP_POLL_READ | PHP_POLL_WRITE);
				uint32_t unfired = monitored & ~fired;

				if (unfired) {
					/* Store unfired events for potential second-round check */
					events[i].events = unfired;
					check_count++;
				}

				if (entry->events & PHP_POLL_ONESHOT) {
					/* Oneshot: remove from tracking */
					php_poll_fd_table_remove(backend_data->fd_table, fd);
					backend_data->active_associations--;
				} else {
					/* Re-associate immediately with all originally registered events */
					int native_events = eventport_events_to_native(entry->events);
					if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events,
								entry->data)
							!= 0) {
						/* Re-association failed - remove from tracking */
						php_poll_fd_table_remove(backend_data->fd_table, fd);
						backend_data->active_associations--;
					}
				}
			}
		} else {
			/* Handle other event sources if needed (timers, user events, etc.) */
			events[i].fd = -1;
			events[i].events = 0;
			events[i].revents = 0;
			events[i].data = port_event->portev_user;
		}
	}

	/* Second pass: if we have unfired events, check them with poll() */
	if (check_count > 0) {
		struct pollfd *check_fds
				= php_poll_calloc(check_count, sizeof(struct pollfd), ctx->persistent);
		int *check_indices = php_poll_calloc(check_count, sizeof(int), ctx->persistent);

		if (check_fds && check_indices) {
			int check_idx = 0;
			for (int i = 0; i < nfds; i++) {
				if (events[i].events != 0 && events[i].fd >= 0) {
					check_fds[check_idx].fd = events[i].fd;
					check_fds[check_idx].events = eventport_events_to_native(events[i].events);
					check_fds[check_idx].revents = 0;
					check_indices[check_idx] = i;
					check_idx++;
					events[i].events = 0; /* Clear it as it was just temporary */
				}
			}

			/* Non-blocking poll to check if other events are ready */
			if (poll(check_fds, check_count, 0) > 0) {
				for (int j = 0; j < check_count; j++) {
					if (check_fds[j].revents != 0) {
						int evt_idx = check_indices[j];
						uint32_t additional = eventport_events_from_native(check_fds[j].revents);
						/* Add the additional ready events to revents */
						events[evt_idx].revents |= additional;
					}
				}
			}
		}

		if (check_fds) {
			pefree(check_fds, ctx->persistent);
		}
		if (check_indices) {
			pefree(check_indices, ctx->persistent);
		}
	}

	return nfds;
}

/* Check if event port backend is available */
static bool eventport_backend_is_available(void)
{
	int fd = port_create();
	if (fd >= 0) {
		close(fd);
		return true;
	}
	return false;
}

static int eventport_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	if (!backend_data || !backend_data->fd_table) {
		return -1;
	}

	/* For event ports, we track exactly how many FD associations are active */
	int active_associations = backend_data->active_associations;

	if (active_associations == 0) {
		return 1;
	}

	/* Event ports can return exactly one event per association,
	 * so the suitable max_events is exactly the number of active associations */
	return active_associations;
}

/* Event port backend operations structure */
const php_poll_backend_ops php_poll_backend_eventport_ops = {
	.type = PHP_POLL_BACKEND_EVENTPORT,
	.name = "eventport",
	.init = eventport_backend_init,
	.cleanup = eventport_backend_cleanup,
	.add = eventport_backend_add,
	.modify = eventport_backend_modify,
	.remove = eventport_backend_remove,
	.wait = eventport_backend_wait,
	.is_available = eventport_backend_is_available,
	.get_suitable_max_events = eventport_backend_get_suitable_max_events,
	.supports_et = false /* Event ports are level-triggered only */
};

#endif /* HAVE_EVENT_PORTS */
