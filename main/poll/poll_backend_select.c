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

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct {
	fd_set read_fds, write_fds, error_fds;
	fd_set master_read_fds, master_write_fds, master_error_fds;
	SOCKET *socket_list;
	void **data_list;
	int socket_count;
	int max_sockets;
} select_backend_data_t;

static int select_backend_init(php_poll_ctx *ctx, int max_events)
{
	select_backend_data_t *data = calloc(1, sizeof(select_backend_data_t));
	if (!data) {
		return PHP_POLL_NOMEM;
	}

	data->max_sockets = max_events;
	data->socket_list = calloc(max_events, sizeof(SOCKET));
	data->data_list = calloc(max_events, sizeof(void *));

	if (!data->socket_list || !data->data_list) {
		free(data->socket_list);
		free(data->data_list);
		free(data);
		return PHP_POLL_NOMEM;
	}

	FD_ZERO(&data->master_read_fds);
	FD_ZERO(&data->master_write_fds);
	FD_ZERO(&data->master_error_fds);
	data->socket_count = 0;

	ctx->backend_data = data;
	return PHP_POLL_OK;
}

static void select_backend_cleanup(php_poll_ctx *ctx)
{
	select_backend_data_t *data = (select_backend_data_t *) ctx->backend_data;
	if (data) {
		free(data->socket_list);
		free(data->data_list);
		free(data);
		ctx->backend_data = NULL;
	}
}

static int select_find_socket_index(select_backend_data_t *data, SOCKET sock)
{
	for (int i = 0; i < data->socket_count; i++) {
		if (data->socket_list[i] == sock) {
			return i;
		}
	}
	return -1;
}

static int select_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	if (backend_data->socket_count >= backend_data->max_sockets) {
		return PHP_POLL_NOMEM;
	}

	/* Check if socket already exists */
	if (select_find_socket_index(backend_data, sock) >= 0) {
		return PHP_POLL_EXISTS;
	}

	/* Add socket to our tracking */
	int index = backend_data->socket_count++;
	backend_data->socket_list[index] = sock;
	backend_data->data_list[index] = data;

	/* Add to appropriate fd_sets */
	if (events & PHP_POLL_READ) {
		FD_SET(sock, &backend_data->master_read_fds);
	}
	if (events & PHP_POLL_WRITE) {
		FD_SET(sock, &backend_data->master_write_fds);
	}
	/* Always monitor for errors */
	FD_SET(sock, &backend_data->master_error_fds);

	return PHP_POLL_OK;
}

static int select_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	int index = select_find_socket_index(backend_data, sock);
	if (index < 0) {
		return PHP_POLL_NOTFOUND;
	}

	/* Update user data */
	backend_data->data_list[index] = data;

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

	return PHP_POLL_OK;
}

static int select_backend_remove(php_poll_ctx *ctx, int fd)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	int index = select_find_socket_index(backend_data, sock);
	if (index < 0) {
		return PHP_POLL_NOTFOUND;
	}

	/* Remove from fd_sets */
	FD_CLR(sock, &backend_data->master_read_fds);
	FD_CLR(sock, &backend_data->master_write_fds);
	FD_CLR(sock, &backend_data->master_error_fds);

	/* Remove from socket list by shifting elements */
	for (int i = index; i < backend_data->socket_count - 1; i++) {
		backend_data->socket_list[i] = backend_data->socket_list[i + 1];
		backend_data->data_list[i] = backend_data->data_list[i + 1];
	}
	backend_data->socket_count--;

	return PHP_POLL_OK;
}

static int select_backend_wait(
		php_poll_ctx *ctx, php_poll_event_t *events, int max_events, int timeout)
{
	select_backend_data_t *backend_data = (select_backend_data_t *) ctx->backend_data;

	if (backend_data->socket_count == 0) {
		/* No sockets to wait for */
		if (timeout > 0) {
			Sleep(timeout);
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
	int result = select(
			0, &backend_data->read_fds, &backend_data->write_fds, &backend_data->error_fds, ptv);

	if (result <= 0) {
		return (result == 0) ? 0 : PHP_POLL_ERROR;
	}

	/* Process results */
	int event_count = 0;
	for (int i = 0; i < backend_data->socket_count && event_count < max_events; i++) {
		SOCKET sock = backend_data->socket_list[i];
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
			events[event_count].events = 0; /* Not used in results */
			events[event_count].revents = revents;
			events[event_count].data = backend_data->data_list[i];
			event_count++;
		}
	}

	return event_count;
}

static bool select_backend_is_available(void)
{
	return true; /* select() is always available */
}

const php_poll_backend_ops php_poll_backend_select_ops = {
	.name = "select",
	.init = select_backend_init,
	.cleanup = select_backend_cleanup,
	.add = select_backend_add,
	.modify = select_backend_modify,
	.remove = select_backend_remove,
	.wait = select_backend_wait,
	.is_available = select_backend_is_available,
	.supports_et = false /* select() doesn't support edge triggering */
};

#endif _WIN32
