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

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

typedef struct iocp_operation {
	OVERLAPPED overlapped;
	int fd;
	uint32_t events;
	void *user_data;
	char buffer[1]; /* Minimal buffer for accept/recv operations */
} iocp_operation_t;

typedef struct {
	HANDLE iocp_handle;
	iocp_operation_t *operations;
	int max_operations;
	int operation_count;
	LPFN_ACCEPTEX AcceptEx;
	LPFN_CONNECTEX ConnectEx;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
} iocp_backend_data_t;

static zend_result iocp_backend_init(php_poll_ctx *ctx, int max_events)
{
	iocp_backend_data_t *data = calloc(1, sizeof(iocp_backend_data_t));
	if (!data) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Create I/O Completion Port */
	data->iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (data->iocp_handle == NULL) {
		free(data);
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	data->max_operations = max_events;
	data->operations = calloc(max_events, sizeof(iocp_operation_t));

	if (!data->operations) {
		CloseHandle(data->iocp_handle);
		free(data);
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

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

	data->operation_count = 0;
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
		free(data->operations);
		free(data);
		ctx->backend_data = NULL;
	}
}

static zend_result iocp_backend_add(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	if (backend_data->operation_count >= backend_data->max_operations) {
		php_poll_set_error(ctx, PHP_POLL_ERR_NOMEM);
		return FAILURE;
	}

	/* Associate socket with completion port */
	HANDLE result
			= CreateIoCompletionPort((HANDLE) sock, backend_data->iocp_handle, (ULONG_PTR) sock, 0);
	if (result == NULL) {
		php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
		return FAILURE;
	}

	/* Set up operation structure */
	iocp_operation_t *op = &backend_data->operations[backend_data->operation_count++];
	memset(op, 0, sizeof(iocp_operation_t));
	op->fd = fd;
	op->events = events;
	op->user_data = data;

	/* For read events, post a recv operation */
	if (events & PHP_POLL_READ) {
		WSABUF wsabuf = { 1, op->buffer };
		DWORD flags = 0;
		DWORD bytes_received;

		int result = WSARecv(sock, &wsabuf, 1, &bytes_received, &flags, &op->overlapped, NULL);

		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			backend_data->operation_count--;
			php_poll_set_error(ctx, PHP_POLL_ERR_SYSTEM);
			return FAILURE;
		}
	}

	return SUCCESS;
}

static zend_result iocp_backend_modify(php_poll_ctx *ctx, int fd, uint32_t events, void *data)
{
	/* For IOCP, we need to cancel existing operations and re-add */
	iocp_backend_remove(ctx, fd);
	return iocp_backend_add(ctx, fd, events, data);
}

static zend_result iocp_backend_remove(php_poll_ctx *ctx, int fd)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;
	SOCKET sock = (SOCKET) fd;

	/* Cancel all I/O operations on this socket */
	CancelIo((HANDLE) sock);

	/* Remove from our operation list */
	for (int i = 0; i < backend_data->operation_count; i++) {
		if (backend_data->operations[i].fd == fd) {
			/* Shift remaining operations */
			for (int j = i; j < backend_data->operation_count - 1; j++) {
				backend_data->operations[j] = backend_data->operations[j + 1];
			}
			backend_data->operation_count--;
			break;
		}
	}

	return SUCCESS;
}

static int iocp_backend_wait(php_poll_ctx *ctx, php_poll_event *events, int max_events, int timeout)
{
	iocp_backend_data_t *backend_data = (iocp_backend_data_t *) ctx->backend_data;

	DWORD bytes_transferred;
	ULONG_PTR completion_key;
	LPOVERLAPPED overlapped;

	BOOL result = GetQueuedCompletionStatus(backend_data->iocp_handle, &bytes_transferred,
			&completion_key, &overlapped, (timeout < 0) ? INFINITE : timeout);

	if (!result && overlapped == NULL) {
		/* Timeout or error */
		return (GetLastError() == WAIT_TIMEOUT) ? 0 : -1;
	}

	if (overlapped != NULL) {
		/* Find the operation that completed */
		iocp_operation_t *op = CONTAINING_RECORD(overlapped, iocp_operation_t, overlapped);

		events[0].fd = op->fd;
		events[0].events = op->events;
		events[0].data = op->user_data;

		if (result) {
			/* Successful completion */
			if (op->events & PHP_POLL_READ) {
				events[0].revents = PHP_POLL_READ;
			} else if (op->events & PHP_POLL_WRITE) {
				events[0].revents = PHP_POLL_WRITE;
			}
		} else {
			/* Error completion */
			events[0].revents = PHP_POLL_ERROR;
		}

		return 1;
	}

	return 0;
}

static bool iocp_backend_is_available(void)
{
	/* IOCP is available on Windows NT and later */
	OSVERSIONINFO osvi = { 0 };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (GetVersionEx(&osvi)) {
		return (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	}

	return false;
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
	.supports_et = true /* IOCP provides completion-based model which is edge-triggered */
};

#endif /* _WIN32 */
