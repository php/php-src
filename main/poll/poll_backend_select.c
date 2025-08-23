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

#include "php_poll_internal.h"
#include "php_network.h"

typedef struct {
	fd_set read_fds, write_fds, error_fds;
	fd_set master_read_fds, master_write_fds, master_error_fds;
	php_poll_fd_table *fd_table;
	php_socket_t max_fd;
} select_backend_data_t;

/* Select-specific helper functions */
static bool find_max_fd_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	php_socket_t *max_fd = (php_socket_t *) user_data;
	php_socket_t sock = (php_socket_t) fd;

	if (sock > *max_fd) {
		*max_fd = sock;
	}
	return true;
}

static php_socket_t select_get_max_fd(php_poll_fd_table *table)
{
	php_socket_t max_fd = 0;
	php_poll_fd_table_foreach(table, find_max_fd_callback, &max_fd);
	return max_fd;
}

/* Update max_fd for select() */
static void select_update_max_fd(select_backend_data_t *data)
{
	data->max_fd = select_get_max_fd(data->fd_table);
}

static zend_result select_backend_init(php_poll_ctx *ctx)
{
	select_backend_data_t *data
			= php_poll_calloc(1, sizeof(select_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;

	/* Initialize FD tracking using helper - now uses HashTable internally */
	data->fd_table = php_poll_fd_table_init(initial_capacity, ctx->persistent);
	if (!data->fd_table) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

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
		php_poll_fd_table_cleanup(data->fd_table);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result select_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *user_data)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	php_socket_t sock = (php_socket_t) fd;

	if (events & PHP_POLL_ET) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

#ifdef FD_SETSIZE
	if (sock >= FD_SETSIZE) {
		php_poll_set_error(ctx, PHP_POLL_ERR_INVALID);
		return FAILURE;
	}
#endif

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

	if (events & PHP_POLL_ET) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOSUPPORT);
		return FAILURE;
	}

	php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	entry->events = events;
	entry->data = user_data;
	entry->last_revents = 0;

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

	if (!php_poll_fd_table_find(backend_data->fd_table, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	FD_CLR(sock, &backend_data->master_read_fds);
	FD_CLR(sock, &backend_data->master_write_fds);
	FD_CLR(sock, &backend_data->master_error_fds);

	php_poll_fd_table_remove(backend_data->fd_table, fd);

	if (sock == backend_data->max_fd) {
		select_update_max_fd(backend_data);
	}

	return SUCCESS;
}

static void select_handle_oneshot_removal(select_backend_data_t *backend_data, int fd)
{
	php_socket_t sock = (php_socket_t) fd;

	FD_CLR(sock, &backend_data->master_read_fds);
	FD_CLR(sock, &backend_data->master_write_fds);
	FD_CLR(sock, &backend_data->master_error_fds);

	php_poll_fd_table_remove(backend_data->fd_table, fd);

	if (sock == backend_data->max_fd) {
		select_update_max_fd(backend_data);
	}
}

/* Callback function for processing select() results */
typedef struct {
	select_backend_data_t *backend_data;
	php_poll_event *events;
	int max_events;
	int event_count;
} select_result_context;

static bool process_select_result_callback(int fd, php_poll_fd_entry *entry, void *user_data)
{
	select_result_context *ctx = (select_result_context *) user_data;

	if (ctx->event_count >= ctx->max_events) {
		return false; /* Stop iteration, events array is full */
	}

	php_socket_t sock = (php_socket_t) fd;
	uint32_t revents = 0;

	if (FD_ISSET(sock, &ctx->backend_data->read_fds)) {
		revents |= PHP_POLL_READ;
	}
	if (FD_ISSET(sock, &ctx->backend_data->write_fds)) {
		revents |= PHP_POLL_WRITE;
	}
	if (FD_ISSET(sock, &ctx->backend_data->error_fds)) {
		revents |= PHP_POLL_ERROR;
	}

	if (revents != 0) {
		ctx->events[ctx->event_count].fd = fd;
		ctx->events[ctx->event_count].events = entry->events;
		ctx->events[ctx->event_count].revents = revents;
		ctx->events[ctx->event_count].data = entry->data;
		ctx->event_count++;
	}

	return true; /* Continue iteration */
}

static int select_backend_wait(
		php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;

	if (php_poll_fd_table_is_empty(backend_data->fd_table)) {
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

	/* Process results using iteration over active FDs only */
	select_result_context result_ctx = {
		.backend_data = backend_data, .events = events, .max_events = max_events, .event_count = 0
	};

	php_poll_fd_table_foreach(backend_data->fd_table, process_select_result_callback, &result_ctx);
	int event_count = result_ctx.event_count;

	/* Handle oneshot removals */
	for (int i = 0; i < event_count; i++) {
		php_poll_fd_entry *entry = php_poll_fd_table_find(backend_data->fd_table, events[i].fd);
		if (entry && (entry->events & PHP_POLL_ONESHOT) && events[i].revents != 0) {
			select_handle_oneshot_removal(backend_data, events[i].fd);
		}
	}

	return event_count;
}

static bool select_backend_is_available(void)
{
	return true; /* select() is always available */
}

static int select_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;

	if (UNEXPECTED(!backend_data || !backend_data->fd_table)) {
		return -1;
	}

	int active_fds = php_poll_fd_table_count(backend_data->fd_table);

	if (active_fds == 0) {
		return 1;
	}

	return active_fds;
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
	.get_suitable_max_events = select_backend_get_suitable_max_events,
	.supports_et = false /* select() doesn't support ET natively, but we simulate it */
};
