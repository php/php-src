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
#include "php_network.h"

typedef struct {
	php_socket_t fd;
	uint32_t events;
	void *data;
	bool active;
	uint32_t last_revents; /* For edge-trigger simulation */
} select_fd_entry;

typedef struct {
	fd_set read_fds, write_fds, error_fds;
	fd_set master_read_fds, master_write_fds, master_error_fds;

	/* FD tracking for user data and edge-trigger simulation */
	select_fd_entry *fd_entries;
	int fd_entries_capacity;
	int fd_count;

	php_socket_t max_fd; /* Highest fd for select() */
} select_backend_data_t;

/* Find FD entry */
static select_fd_entry *select_find_fd_entry(select_backend_data_t *data, php_socket_t fd)
{
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (data->fd_entries[i].active && data->fd_entries[i].fd == fd) {
			return &data->fd_entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
static select_fd_entry *select_get_fd_entry(
		select_backend_data_t *data, php_socket_t fd, bool persistent)
{
	select_fd_entry *entry = select_find_fd_entry(data, fd);
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
	select_fd_entry *new_entries
			= perealloc(data->fd_entries, new_capacity * sizeof(select_fd_entry), persistent);
	if (!new_entries) {
		return NULL;
	}

	/* Initialize new entries */
	memset(new_entries + data->fd_entries_capacity, 0,
			(new_capacity - data->fd_entries_capacity) * sizeof(select_fd_entry));

	data->fd_entries = new_entries;

	/* Use first new slot */
	select_fd_entry *new_entry = &data->fd_entries[data->fd_entries_capacity];
	new_entry->fd = fd;
	new_entry->active = true;
	new_entry->last_revents = 0;
	data->fd_count++;

	data->fd_entries_capacity = new_capacity;
	return new_entry;
}

/* Remove FD entry */
static void select_remove_fd_entry(select_backend_data_t *data, php_socket_t fd)
{
	select_fd_entry *entry = select_find_fd_entry(data, fd);
	if (entry) {
		entry->active = false;
		data->fd_count--;
	}
}

/* Update max_fd for select() */
static void select_update_max_fd(select_backend_data_t *data)
{
	data->max_fd = 0;
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (data->fd_entries[i].active && data->fd_entries[i].fd > data->max_fd) {
			data->max_fd = data->fd_entries[i].fd;
		}
	}
}

static zend_result select_backend_init(php_poll_ctx *ctx)
{
	select_backend_data_t *data = pecalloc(1, sizeof(select_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;

	data->fd_entries = pecalloc(initial_capacity, sizeof(select_fd_entry), ctx->persistent);
	if (!data->fd_entries) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->fd_entries_capacity = initial_capacity;
	data->fd_count = 0;

	FD_ZERO(&data->master_read_fds);
	FD_ZERO(&data->master_write_fds);
	FD_ZERO(&data->master_error_fds);
	data->max_fd = 0;

	ctx->backend_data = data;
	return SUCCESS;
}

static void select_backend_cleanup(php_poll_ctx *ctx)
{
	select_backend_data_t *data = (select_backend_data_t *) ctx->backend_data;
	if (data) {
		pefree(data->fd_entries, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result select_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	php_socket_t sock = (php_socket_t) fd;

#ifdef FD_SETSIZE
	if (sock >= FD_SETSIZE) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}
#endif

	/* Check if FD already exists */
	if (select_find_fd_entry(backend_data, sock)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	/* Get FD entry for tracking */
	select_fd_entry *entry = select_get_fd_entry(backend_data, sock, ctx->persistent);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;

	/* Add to appropriate fd_sets */
	if (events & PHP_POLL_READ) {
		FD_SET(sock, &backend_data->master_read_fds);
	}
	if (events & PHP_POLL_WRITE) {
		FD_SET(sock, &backend_data->master_write_fds);
	}
	/* Always monitor for errors */
	FD_SET(sock, &backend_data->master_error_fds);

	/* Update max_fd */
	if (sock > backend_data->max_fd) {
		backend_data->max_fd = sock;
	}

	return SUCCESS;
}

static zend_result select_backend_modify(
		php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	php_socket_t sock = (php_socket_t) fd;

	/* Find existing entry */
	select_fd_entry *entry = select_find_fd_entry(backend_data, sock);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Update entry */
	entry->events = events;
	entry->data = user_data;
	entry->last_revents = 0; /* Reset on modify */

	/* Remove from all sets first */
	FD_CLR(sock, &backend_data->master_read_fds);
	FD_CLR(sock, &backend_data->master_write_fds);
	FD_CLR(sock, &backend_data->master_error_fds);

	/* Add back based on new events */
	if (events & PHP_POLL_READ) {
		FD_SET(sock, &backend_data->master_read_fds);
	}
	if (events & PHP_POLL_WRITE) {
		FD_SET(sock, &backend_data->master_write_fds);
	}
	FD_SET(sock, &backend_data->master_error_fds);

	return SUCCESS;
}

static zend_result select_backend_remove(php_poll_ctx *ctx, int fd)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	php_socket_t sock = (php_socket_t) fd;

	/* Find existing entry */
	select_fd_entry *entry = select_find_fd_entry(backend_data, sock);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Remove from fd_sets */
	FD_CLR(sock, &backend_data->master_read_fds);
	FD_CLR(sock, &backend_data->master_write_fds);
	FD_CLR(sock, &backend_data->master_error_fds);

	/* Remove from tracking */
	select_remove_fd_entry(backend_data, sock);

	/* Update max_fd if this was the highest */
	if (sock == backend_data->max_fd) {
		select_update_max_fd(backend_data);
	}

	return SUCCESS;
}

/* Edge-trigger simulation */
static int select_simulate_edge_trigger(
		select_backend_data_t *backend_data, php_poll_event *events, int nfds)
{
	int filtered_count = 0;

	for (int i = 0; i < nfds; i++) {
		select_fd_entry *entry = select_find_fd_entry(backend_data, (php_socket_t) events[i].fd);
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
				php_socket_t sock = (php_socket_t) events[i].fd;

				/* Remove from fd_sets */
				FD_CLR(sock, &backend_data->master_read_fds);
				FD_CLR(sock, &backend_data->master_write_fds);
				FD_CLR(sock, &backend_data->master_error_fds);

				/* Remove from tracking */
				select_remove_fd_entry(backend_data, sock);

				/* Update max_fd if needed */
				if (sock == backend_data->max_fd) {
					select_update_max_fd(backend_data);
				}
			}
		}
	}

	return filtered_count;
}

static int select_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;

	if (backend_data->fd_count == 0) {
		/* No sockets to wait for, but respect timeout */
		if (timeout > 0) {
#ifdef _WIN32
			Sleep(timeout);
#else
			struct timespec ts;
			ts.tv_sec = timeout / 1000;
			ts.tv_nsec = (timeout % 1000) * 1000000;
			nanosleep(&ts, NULL);
#endif
		}
		return 0;
	}

	/* Copy master sets */
	memcpy(&backend_data->read_fds, &backend_data->master_read_fds, sizeof(fd_set));
	memcpy(&backend_data->write_fds, &backend_data->master_write_fds, sizeof(fd_set));
	memcpy(&backend_data->error_fds, &backend_data->master_error_fds, sizeof(fd_set));

	/* Setup timeout */
	struct timeval tv = { 0 }, *ptv = NULL;
	if (timeout >= 0) {
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		ptv = &tv;
	}

	/* Call select() */
	int result = select((int) (backend_data->max_fd + 1), &backend_data->read_fds,
			&backend_data->write_fds, &backend_data->error_fds, ptv);

	if (result <= 0) {
		return (result == 0) ? 0 : -1;
	}

	/* Process results */
	int event_count = 0;
	for (int i = 0; i < backend_data->fd_entries_capacity && event_count < max_events; i++) {
		if (!backend_data->fd_entries[i].active) {
			continue;
		}

		php_socket_t sock = backend_data->fd_entries[i].fd;
		uint32_t revents = 0;

		if (FD_ISSET(sock, &backend_data->read_fds)) {
			revents |= PHP_POLL_READ;
		}
		if (FD_ISSET(sock, &backend_data->write_fds)) {
			revents |= PHP_POLL_WRITE;
		}
		if (FD_ISSET(sock, &backend_data->error_fds)) {
			revents |= PHP_POLL_ERROR;
		}

		if (revents != 0) {
			events[event_count].fd = (int) sock;
			events[event_count].events = backend_data->fd_entries[i].events;
			events[event_count].revents = revents;
			events[event_count].data = backend_data->fd_entries[i].data;
			event_count++;
		}
	}

	/* Apply edge-trigger simulation */
	int nfds = select_simulate_edge_trigger(backend_data, events, event_count);

	return nfds;
}

static bool select_backend_is_available(void)
{
	return true; /* select() is always available */
}

const php_poll_backend_ops php_poll_backend_select_ops = {
	.type = PHP_POLL_BACKEND_SELECT,
	.name = "select",
	.init = select_backend_init,
	.cleanup = select_backend_cleanup,
	.add = select_backend_add,
	.modify = select_backend_modify,
	.remove = select_backend_remove,
	.wait = select_backend_wait,
	.is_available = select_backend_is_available,
	.supports_et = false /* select() doesn't support ET natively, but we simulate it */
};
