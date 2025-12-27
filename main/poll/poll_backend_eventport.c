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
	int port_fd;
	port_event_t *events;
	int events_capacity;
	php_poll_fd_table *fd_table;
} eventport_backend_data_t;

/* We use last_revents field to track if fd needs (re)association */
#define EVENTPORT_NEEDS_ASSOC 1
#define EVENTPORT_IS_ASSOCIATED 0

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

/* Add file descriptor to event port - just store in table */
static zend_result eventport_backend_add(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	if (php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_fd_table_get_new(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;
	entry->last_revents = EVENTPORT_NEEDS_ASSOC; /* Mark as needing association */

	return SUCCESS;
}

/* Modify file descriptor in event port - just update table */
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

	/* If currently associated, dissociate so we can re-associate with new events */
	if (entry->last_revents == EVENTPORT_IS_ASSOCIATED) {
		port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd);
	}

	entry->last_revents = EVENTPORT_NEEDS_ASSOC; /* Mark as needing re-association */

	return SUCCESS;
}

/* Remove file descriptor from event port */
static zend_result eventport_backend_remove(php_poll_ctx *ctx, int fd)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Only dissociate if it was actually associated */
	if (entry->last_revents == EVENTPORT_IS_ASSOCIATED) {
		if (port_dissociate(backend_data->port_fd, PORT_SOURCE_FD, fd) == -1) {
			/* Only fail if it's not ENOENT (might already be auto-dissociated) */
			if (!php_poll_is_not_found_error()) {
				php_poll_set_current_errno_error(ctx);
				return FAILURE;
			}
		}
	}

	php_poll_fd_table_remove(backend_data->fd_table, fd);
	return SUCCESS;
}

/* Callback context for associating fds */
typedef struct {
	eventport_backend_data_t *backend_data;
	php_poll_ctx *ctx;
	bool has_error;
} eventport_associate_ctx;

/* Callback to associate fds that need association */
static bool eventport_associate_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	eventport_associate_ctx *assoc_ctx = (eventport_associate_ctx *) user_data;

	/* Only associate if marked as needing association */
	if (entry->last_revents == EVENTPORT_NEEDS_ASSOC) {
		int native_events = eventport_events_to_native(entry->events);

		if (port_associate(assoc_ctx->backend_data->port_fd, PORT_SOURCE_FD, fd, native_events,
					entry->data)
				== -1) {
			/* Association failed - could set error here if needed */
			switch (errno) {
				case EBADFD:
					/* fd got closed - remove it */
					php_poll_fd_table_remove(assoc_ctx->backend_data->fd_table, fd);
					return true;
				case ENOMEM:
					php_poll_set_error(assoc_ctx->ctx, PHP_POLL_ERR_NOMEM);
					break;
				case EINVAL:
					php_poll_set_error(assoc_ctx->ctx, PHP_POLL_ERR_INVALID);
					break;
				default:
					php_poll_set_error(assoc_ctx->ctx, PHP_POLL_ERR_SYSTEM);
					break;
			}
			assoc_ctx->has_error = true;
			return false; /* Stop iteration */
		}

		/* Mark as associated */
		entry->last_revents = EVENTPORT_IS_ASSOCIATED;
	}

	return true; /* Continue iteration */
}

/* Wait for events using event port */
static int eventport_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	eventport_backend_data_t *backend_data = (eventport_backend_data_t *) ctx->backend_data;

	int fd_count = php_poll_fd_table_count(backend_data->fd_table);
	if (fd_count == 0) {
		/* No fds to monitor, but we still need to respect timeout */
		if (timeout > 0) {
			struct timespec ts;
			ts.tv_sec = timeout / 1000;
			ts.tv_nsec = (timeout % 1000) * 1000000;
			nanosleep(&ts, NULL);
		}
		return 0;
	}

	/* First: associate all fds that need association */
	eventport_associate_ctx assoc_ctx
			= { .backend_data = backend_data, .ctx = ctx, .has_error = false };

	php_poll_fd_table_foreach(backend_data->fd_table, eventport_associate_callback, &assoc_ctx);

	if (assoc_ctx.has_error) {
		return -1;
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
		if (php_poll_is_timeout_error()) {
			return 0;
		}
		php_poll_set_current_errno_error(ctx);
		return -1;
	}

	int nfds = (int) nget;

	/* Process events */
	for (int i = 0; i < nfds; i++) {
		port_event_t *port_event = &backend_data->events[i];

		/* Only handle PORT_SOURCE_FD events */
		if (port_event->portev_source == PORT_SOURCE_FD) {
			int fd = (int) port_event->portev_object;

			events[i].fd = fd;
			events[i].events = 0;
			events[i].revents = eventport_events_from_native(port_event->portev_events);
			events[i].data = port_event->portev_user;

			/* After event fires, the association is automatically removed by event ports */
			php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
			if (entry) {
				if (entry->events & PHP_POLL_ONESHOT) {
					/* Oneshot: remove from tracking completely */
					php_poll_fd_table_remove(backend_data->fd_table, fd);
				} else {
					/* Mark for re-association on next wait() call */
					entry->last_revents = EVENTPORT_NEEDS_ASSOC;
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

	int fd_count = php_poll_fd_table_count(backend_data->fd_table);

	if (fd_count == 0) {
		return 1;
	}

	/* Event ports can return exactly one event per association,
	 * so the suitable max_events is exactly the number of tracked fds */
	return fd_count;
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
