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

typedef struct {
	int fd;
	uint32_t events;
	void *data;
	bool active;
} eventport_fd_entry;

typedef struct {
	int port_fd;
	port_event_t *events;
	int events_capacity;
	int active_associations;

	/* FD tracking for re-association */
	eventport_fd_entry *fd_entries;
	int fd_entries_capacity;
	int fd_count;
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

/* Find FD entry */
static eventport_fd_entry *eventport_find_fd_entry(eventport_backend_data_t *data, int fd)
{
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (data->fd_entries[i].active && data->fd_entries[i].fd == fd) {
			return &data->fd_entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
static eventport_fd_entry *eventport_get_fd_entry(
		eventport_backend_data_t *data, int fd, bool persistent)
{
	eventport_fd_entry *entry = eventport_find_fd_entry(data, fd);
	if (entry) {
		return entry;
	}

	/* Find empty slot */
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (!data->fd_entries[i].active) {
			data->fd_entries[i].fd = fd;
			data->fd_entries[i].active = true;
			data->fd_count++;
			return &data->fd_entries[i];
		}
	}

	int new_capacity = data->fd_entries_capacity ? data->fd_entries_capacity * 2 : 64;
	eventport_fd_entry *new_entries
			= perealloc(data->fd_entries, new_capacity * sizeof(eventport_fd_entry), persistent);
	if (!new_entries) {
		return NULL;
	}

	memset(new_entries + data->fd_entries_capacity, 0,
			(new_capacity - data->fd_entries_capacity) * sizeof(eventport_fd_entry));

	data->fd_entries = new_entries;

	/* Use first new slot */
	eventport_fd_entry *new_entry = &data->fd_entries[data->fd_entries_capacity];
	new_entry->fd = fd;
	new_entry->active = true;
	data->fd_count++;

	data->fd_entries_capacity = new_capacity;
	return new_entry;
}

/* Remove FD entry */
static void eventport_remove_fd_entry(eventport_backend_data_t *data, int fd)
{
	eventport_fd_entry *entry = eventport_find_fd_entry(data, fd);
	if (entry) {
		entry->active = false;
		data->fd_count--;
	}
}

/* Initialize event port backend */
static zend_result eventport_backend_init(php_poll_ctx *ctx)
{
	eventport_backend_data_t *data = pecalloc(1, sizeof(eventport_backend_data_t), ctx->persistent);
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
	data->fd_count = 0;

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;
	data->events = pecalloc(initial_capacity, sizeof(port_event_t), ctx->persistent);
	if (!data->events) {
		close(data->port_fd);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->events_capacity = initial_capacity;

	/* Initialize FD tracking array */
	data->fd_entries = pecalloc(initial_capacity, sizeof(eventport_fd_entry), ctx->persistent);
	if (!data->fd_entries) {
		close(data->port_fd);
		pefree(data->events, ctx->persistent);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->fd_entries_capacity = initial_capacity;

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
		pefree(data->fd_entries, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

/* Add file descriptor to event port */
static zend_result eventport_backend_add(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	/* Check if FD already exists */
	if (eventport_find_fd_entry(backend_data, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	/* Get FD entry for tracking */
	eventport_fd_entry *entry = eventport_get_fd_entry(backend_data, fd, ctx->persistent);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;

	int native_events = eventport_events_to_native(events);

	/* Associate file descriptor with event port */
	if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events, user_data) == -1) {
		eventport_remove_fd_entry(backend_data, fd);
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

	/* Find existing entry */
	eventport_fd_entry *entry = eventport_find_fd_entry(backend_data, fd);
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

	/* Find existing entry */
	eventport_fd_entry *entry = eventport_find_fd_entry(backend_data, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	if (port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd) == -1) {
		/* Only fail if it's not ENOENT (might already be dissociated) */
		if (errno != ENOENT) {
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
	}

	eventport_remove_fd_entry(backend_data, fd);
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
		port_event_t *new_events = perealloc(
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
		if (errno == ETIME) {
			/* Timeout - this is normal */
			return 0;
		} else if (errno == EINTR) {
			/* Interrupted by signal */
			return 0;
		} else {
			/* Real error */
			return -1;
		}
	}

	int nfds = (int) nget;

	/* Process the raw events first */
	for (int i = 0; i < nfds; i++) {
		port_event_t *port_event = &backend_data->events[i];

		/* Only handle PORT_SOURCE_FD events */
		if (port_event->portev_source == PORT_SOURCE_FD) {
			int fd = (int) port_event->portev_object;
			events[i].fd = fd;
			events[i].events = 0; /* Not used in results */
			events[i].revents = eventport_events_from_native(port_event->portev_events);
			events[i].data = port_event->portev_user;

			/* Event ports are naturally level-triggered but auto-dissociate after firing.
			 * Re-associate unless it's oneshot */
			eventport_fd_entry *entry = eventport_find_fd_entry(backend_data, fd);
			if (entry && !(entry->events & PHP_POLL_ONESHOT)) {
				/* For level-triggered: re-associate with all events
				 * For edge-triggered: re-associate with events that didn't fire */
				uint32_t reassoc_events = entry->events;
				if (entry->events & PHP_POLL_ET) {
					/* Edge-triggered: don't re-associate with events that just fired */
					reassoc_events &= ~events[i].revents;
					reassoc_events &= ~PHP_POLL_ET; /* Remove ET flag for port_associate */
				}

				if (reassoc_events != 0) {
					int native_events = eventport_events_to_native(reassoc_events);
					if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events,
								entry->data)
							!= 0) {
						/* Re-association failed  might be due to fd being closed */
						eventport_remove_fd_entry(backend_data, fd);
						backend_data->active_associations--;
					}
				} else {
					/* No events to re-associate with */
					eventport_remove_fd_entry(backend_data, fd);
					backend_data->active_associations--;
				}
			} else {
				/* Oneshot or entry not found - remove from tracking */
				if (entry) {
					eventport_remove_fd_entry(backend_data, fd);
				}
				backend_data->active_associations--;
			}
		} else {
			/* Handle other event sources if needed (timers, user events, etc.) */
			events[i].fd = -1;
			events[i].events = 0;
			events[i].revents = 0;
			events[i].data = port_event->portev_user;
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
	.supports_et = true /* Supports both level and edge triggering */
};

#endif /* HAVE_EVENT_PORTS */
