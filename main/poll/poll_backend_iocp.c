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

#ifdef PHP_WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

typedef struct iocp_operation {
	OVERLAPPED overlapped;
	int fd;
	uint32_t events;
	void *user_data;
	bool active;
	char buffer[1]; /* Minimal buffer for accept/recv operations */
} iocp_operation_t;

typedef struct {
	int fd;
	uint32_t events;
	void *data;
	bool active;
	bool associated; /* Whether socket is associated with IOCP */
} iocp_fd_entry;

typedef struct {
	HANDLE iocp_handle;
	iocp_operation_t *operations;
	int operations_capacity;
	int operation_count;

	/* FD tracking */
	iocp_fd_entry *fd_entries;
	int fd_entries_capacity;
	int fd_count;

	LPFN_ACCEPTEX AcceptEx;
	LPFN_CONNECTEX ConnectEx;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
} iocp_backend_data_t;

/* Find FD entry */
static iocp_fd_entry *iocp_find_fd_entry(iocp_backend_data_t *data, int fd)
{
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (data->fd_entries[i].active && data->fd_entries[i].fd == fd) {
			return &data->fd_entries[i];
		}
	}
	return NULL;
}

/* Get or create FD entry */
static iocp_fd_entry *iocp_get_fd_entry(iocp_backend_data_t *data, int fd, bool persistent)
{
	iocp_fd_entry *entry = iocp_find_fd_entry(data, fd);
	if (entry) {
		return entry;
	}

	/* Find empty slot */
	for (int i = 0; i < data->fd_entries_capacity; i++) {
		if (!data->fd_entries[i].active) {
			data->fd_entries[i].fd = fd;
			data->fd_entries[i].active = true;
			data->fd_entries[i].associated = false;
			data->fd_count++;
			return &data->fd_entries[i];
		}
	}

	/* Need to grow the array */
	int new_capacity = data->fd_entries_capacity ? data->fd_entries_capacity * 2 : 64;
	iocp_fd_entry *new_entries
			= php_poll_realloc(data->fd_entries, new_capacity * sizeof(iocp_fd_entry), persistent);
	if (!new_entries) {
		return NULL;
	}

	/* Initialize new entries */
	memset(new_entries + data->fd_entries_capacity, 0,
			(new_capacity - data->fd_entries_capacity) * sizeof(iocp_fd_entry));

	data->fd_entries = new_entries;

	/* Use first new slot */
	iocp_fd_entry *new_entry = &data->fd_entries[data->fd_entries_capacity];
	new_entry->fd = fd;
	new_entry->active = true;
	new_entry->associated = false;
	data->fd_count++;

	data->fd_entries_capacity = new_capacity;
	return new_entry;
}

/* Remove FD entry */
static void iocp_remove_fd_entry(iocp_backend_data_t *data, int fd)
{
	iocp_fd_entry *entry = iocp_find_fd_entry(data, fd);
	if (entry) {
		entry->active = false;
		entry->associated = false;
		data->fd_count--;
	}
}

static zend_result iocp_backend_init(php_poll_ctx *ctx)
{
	iocp_backend_data_t *data = php_poll_calloc(1, sizeof(iocp_backend_data_t), ctx->persistent);
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Create I/O Completion Port */
	data->iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (data->iocp_handle == NULL) {
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	/* Use hint for initial allocation if provided, otherwise start with reasonable default */
	int initial_capacity = ctx->max_events_hint > 0 ? ctx->max_events_hint : 64;

	data->operations = php_poll_calloc(initial_capacity, sizeof(iocp_operation_t), ctx->persistent);
	if (!data->operations) {
		CloseHandle(data->iocp_handle);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->operations_capacity = initial_capacity;
	data->operation_count = 0;

	/* Initialize FD tracking array */
	data->fd_entries = php_poll_calloc(initial_capacity, sizeof(iocp_fd_entry), ctx->persistent);
	if (!data->fd_entries) {
		CloseHandle(data->iocp_handle);
		pefree(data->operations, ctx->persistent);
		pefree(data, ctx->persistent);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}
	data->fd_entries_capacity = initial_capacity;
	data->fd_count = 0;

	/* Load Winsock extension functions */
	SOCKET dummy_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (dummy_socket != INVALID_SOCKET) {
		GUID acceptex_guid = WSAID_ACCEPTEX;
		GUID connectex_guid = WSAID_CONNECTEX;
		GUID getacceptexsockaddrs_guid = WSAID_GETACCEPTEXSOCKADDRS;
		DWORD bytes;

		WSAIoctl(dummy_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_guid,
				sizeof(acceptex_guid), &data->AcceptEx, sizeof(data->AcceptEx), &bytes, NULL, NULL);

		WSAIoctl(dummy_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectex_guid,
				sizeof(connectex_guid), &data->ConnectEx, sizeof(data->ConnectEx), &bytes, NULL,
				NULL);

		WSAIoctl(dummy_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &getacceptexsockaddrs_guid,
				sizeof(getacceptexsockaddrs_guid), &data->GetAcceptExSockaddrs,
				sizeof(data->GetAcceptExSockaddrs), &bytes, NULL, NULL);

		closesocket(dummy_socket);
	}

	ctx->backend_data = data;
	return SUCCESS;
}

static void iocp_backend_cleanup(php_poll_ctx *ctx)
{
	iocp_backend_data_t *data = (iocp_backend_data_t *) ctx->backend_data;
	if (data) {
		if (data->iocp_handle != NULL) {
			CloseHandle(data->iocp_handle);
		}
		pefree(data->operations, ctx->persistent);
		pefree(data->fd_entries, ctx->persistent);
		pefree(data, ctx->persistent);
		ctx->backend_data = NULL;
	}
}

static zend_result iocp_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	/* Check if FD already exists */
	if (iocp_find_fd_entry(backend_data, fd)) {
		php_poll_set_error(ctx, PHP_POLL_ERR_EXISTS);
		return FAILURE;
	}

	/* Get FD entry for tracking */
	iocp_fd_entry *entry = iocp_get_fd_entry(backend_data, fd, ctx->persistent);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	entry->events = events;
	entry->data = data;

	/* Associate socket with completion port if not already done */
	if (!entry->associated) {
		HANDLE result = CreateIoCompletionPort(
				(HANDLE) sock, backend_data->iocp_handle, (ULONG_PTR) sock, 0);
		if (result == NULL) {
			iocp_remove_fd_entry(backend_data, fd);
			php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
			return FAILURE;
		}
		entry->associated = true;
	}

	/* Note: IOCP operations are typically initiated on-demand rather than pre-posted.
	 * For a polling API, we would need to simulate readiness using techniques like:
	 * - Zero-byte reads to check readability
	 * - Connect() to localhost to check writability
	 * This is complex and IOCP is better suited for async I/O rather than polling.
	 * For now, we just track the association. */

	return SUCCESS;
}

static zend_result iocp_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;

	/* Find existing entry */
	iocp_fd_entry *entry = iocp_find_fd_entry(backend_data, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Update entry */
	entry->events = events;
	entry->data = data;

	return SUCCESS;
}

static zend_result iocp_backend_remove(php_poll_ctx *ctx, int fd)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	/* Find existing entry */
	iocp_fd_entry *entry = iocp_find_fd_entry(backend_data, fd);
	if (!entry) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOTFOUND);
		return FAILURE;
	}

	/* Cancel all I/O operations on this socket */
	CancelIo((HANDLE) sock);

	/* Remove from tracking */
	iocp_remove_fd_entry(backend_data, fd);

	return SUCCESS;
}

static int iocp_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;

	/* IOCP is fundamentally different from other polling mechanisms.
	 * It's completion-based rather than readiness-based.
	 * A proper implementation would need to:
	 * 1. Post overlapped I/O operations for all registered FDs
	 * 2. Wait for completions
	 * 3. Return the completed operations as events
	 *
	 * This is complex and doesn't map well to a traditional polling API.
	 * For now, we provide a minimal implementation that can detect some completions. */

	if (backend_data->fd_count == 0) {
		/* No FDs to monitor, but respect timeout */
		if (timeout > 0) {
			Sleep(timeout);
		}
		return 0;
	}

	DWORD bytes_transferred;
	ULONG_PTR completion_key;
	LPOVERLAPPED overlapped;

	BOOL result = GetQueuedCompletionStatus(backend_data->iocp_handle, &bytes_transferred,
			&completion_key, &overlapped, (timeout < 0) ? INFINITE : timeout);

	if (!result && overlapped == NULL) {
		/* Timeout or error */
		DWORD error = GetLastError();
		if (error == WAIT_TIMEOUT) {
			return 0;
		} else {
			php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
			return -1;
		}
	}

	if (overlapped != NULL) {
		/* We got a completion, but since we're not posting operations in add(),
		 * this would only happen if the application is doing its own overlapped I/O.
		 * Try to match it to one of our tracked FDs. */

		SOCKET completed_socket = (SOCKET) completion_key;
		iocp_fd_entry *entry = iocp_find_fd_entry(backend_data, (int) completed_socket);

		if (entry && max_events > 0) {
			events[0].fd = entry->fd;
			events[0].events = entry->events;
			events[0].data = entry->data;

			if (result) {
				/* Successful completion - determine event type */
				events[0].revents = entry->events & (PHP_POLL_READ | PHP_POLL_WRITE);
			} else {
				/* Error completion */
				events[0].revents = PHP_POLL_ERROR;
			}

			return 1;
		}
	}

	return 0;
}

static bool iocp_backend_is_available(void)
{
	/* IOCP is available on Windows NT and later (basically all modern Windows) */
	return true;
}

static int iocp_backend_get_suitable_max_events(php_poll_ctx *ctx)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;

	if (!backend_data) {
		return -1;
	}

	/* For IOCP, we track exactly how many FDs are registered */
	int active_fds = backend_data->fd_count;

	if (active_fds == 0) {
		return 1;
	}

	/* IOCP can potentially return multiple completions per socket,
	 * but typically it's one completion per operation.
	 * Since we're simulating polling behavior, use the FD count directly. */
	return active_fds;
}

const php_poll_backend_ops php_poll_backend_iocp_ops = {
	.type = PHP_POLL_BACKEND_IOCP,
	.name = "iocp",
	.init = iocp_backend_init,
	.cleanup = iocp_backend_cleanup,
	.add = iocp_backend_add,
	.modify = iocp_backend_modify,
	.remove = iocp_backend_remove,
	.wait = iocp_backend_wait,
	.is_available = iocp_backend_is_available,
	.get_suitable_max_events = iocp_backend_get_suitable_max_events,
	.supports_et = true /* IOCP provides completion-based model which is naturally edge-triggered */
};

#endif /* PHP_WIN32 */
