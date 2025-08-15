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

#ifdef HAVE_PORT_CREATE

#include <port.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
	int port_fd;
	port_event_t *events;
	int max_events;
	int active_associations;
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
	/* Event ports provide edge-triggered semantics by default */
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
static int eventport_backend_init(php_poll_ctx_t *ctx, int max_events)
{
	eventport_backend_data_t *data = calloc(1, sizeof(eventport_backend_data_t));
	if (!data) {
		return PHP_POLL_NOMEM;
	}

	/* Create event port */
	data->port_fd = port_create();
	if (data->port_fd == -1) {
		free(data);
		return PHP_POLL_ERROR;
	}

	data->max_events = max_events;
	data->active_associations = 0;

	/* Allocate event array for port_getn() */
	data->events = calloc(max_events, sizeof(port_event_t));
	if (!data->events) {
		close(data->port_fd);
		free(data);
		return PHP_POLL_NOMEM;
	}

	ctx->backend_data = data;
	return PHP_POLL_OK;
}

/* Cleanup event port backend */
static void eventport_backend_cleanup(php_poll_ctx_t *ctx)
{
	eventport_backend_data_t *data = (eventport_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->port_fd >= 0) {
			close(data->port_fd);
		}
		free(data->events);
		free(data);
		ctx->backend_data = NULL;
	}
}

/* Add file descriptor to event port */
static int eventport_backend_add(php_poll_ctx_t *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	int native_events = eventport_events_to_native(events);

	/* Associate file descriptor with event port */
	if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events, user_data) == -1) {
		switch (errno) {
			case EEXIST:
				return PHP_POLL_EXISTS;
			case ENOMEM:
				return PHP_POLL_NOMEM;
			case EBADF:
			case EINVAL:
				return PHP_POLL_INVALID;
			default:
				return PHP_POLL_ERROR;
		}
	}

	backend_data->active_associations++;
	return PHP_POLL_OK;
}

/* Modify file descriptor in event port */
static int eventport_backend_modify(php_poll_ctx_t *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	/* For event ports, we need to dissociate and re-associate */
	/* Note: dissociate might fail if the fd was already fired and auto-dissociated */
	port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd);

	int native_events = eventport_events_to_native(events);
	if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, fd, native_events, user_data) == -1) {
		switch (errno) {
			case ENOMEM:
				return PHP_POLL_NOMEM;
			case EBADF:
			case EINVAL:
				return PHP_POLL_INVALID;
			default:
				return PHP_POLL_ERROR;
		}
	}

	return PHP_POLL_OK;
}

/* Remove file descriptor from event port */
static int eventport_backend_remove(php_poll_ctx_t *ctx, int fd)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	if (port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd) == -1) {
		switch (errno) {
			case ENOENT:
				return PHP_POLL_NOTFOUND;
			case EBADF:
			case EINVAL:
				return PHP_POLL_INVALID;
			default:
				return PHP_POLL_ERROR;
		}
	}

	backend_data->active_associations--;
	return PHP_POLL_OK;
}

/* Wait for events using event port */
static int eventport_backend_wait(
		php_poll_ctx_t *ctx, php_poll_event_t *events, int max_events, int timeout)
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

	/* Setup timeout structure */
	struct timespec ts = { 0 }, *tsp = NULL;
	if (timeout >= 0) {
		ts.tv_sec = timeout / 1000;
		ts.tv_nsec = (timeout % 1000) * 1000000;
		tsp = &ts;
	}

	/* Retrieve events from port */
	uint_t nget = 1; /* We want to get multiple events if available */
	int result = port_getn(backend_data->port_fd, backend_data->events,
			MIN(max_events, backend_data->max_events), &nget, tsp);

	if (result == -1) {
		if (errno == ETIME) {
			/* Timeout - this is normal */
			return 0;
		} else if (errno == EINTR) {
			/* Interrupted by signal */
			return 0;
		} else {
			/* Real error */
			return PHP_POLL_ERROR;
		}
	}

	int nfds = (int) nget;

	/* Process the events */
	for (int i = 0; i < nfds; i++) {
		port_event_t *port_event = &backend_data->events[i];

		/* Only handle PORT_SOURCE_FD events */
		if (port_event->portev_source == PORT_SOURCE_FD) {
			events[i].fd = (int) port_event->portev_object;
			events[i].events = 0; /* Not used in results */
			events[i].revents = eventport_events_from_native(port_event->portev_events);
			events[i].data = port_event->portev_user;

			/* Event ports automatically dissociate after firing, so we need to
			   re-associate if this is not a oneshot event and we want level-triggered behavior */
			php_poll_fd_entry_t *entry = php_poll_find_fd_entry(ctx, events[i].fd);
			if (entry && !(entry->events & PHP_POLL_ONESHOT)) {
				/* Re-associate for continued monitoring */
				int native_events = eventport_events_to_native(entry->events);
				if (port_associate(backend_data->port_fd, PORT_SOURCE_FD, events[i].fd,
							native_events, entry->data)
						== 0) {
					/* Re-association successful */
				} else {
					/* Re-association failed - might be due to fd being closed */
					backend_data->active_associations--;
				}
			} else {
				/* Oneshot event or entry not found - reduce association count */
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
const php_poll_backend_ops_t php_poll_backend_eventport_ops = {
	.name = "eventport",
	.init = eventport_backend_init,
	.cleanup = eventport_backend_cleanup,
	.add = eventport_backend_add,
	.modify = eventport_backend_modify,
	.remove = eventport_backend_remove,
	.wait = eventport_backend_wait,
	.is_available = eventport_backend_is_available,
	.supports_et = true /* Event ports provide edge-triggered semantics by default */
};

#endif /* HAVE_PORT_CREATE */
