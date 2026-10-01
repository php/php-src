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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_network.h"
#include "win32/time.h"

/* Win32 select() will only work with sockets, so we roll our own implementation here.
 * - If you supply only sockets, this simply passes through to winsock select().
 * - If you supply file handles, there is no way to distinguish between
 *   ready for read/write or OOB, so any set in which the handle is found will
 *   be marked as ready.
 * - If you supply only pipe handles in rfds, and no handles in wfds or efds,
 *   the pipes will only be marked as ready if there is data available.
 * - If you supply a mixture of handles and sockets, the system will interleave
 *   calls between select() and WaitForMultipleObjects(). The time slicing may
 *   cause this function call to take up to 100 ms longer than you specified.
 * - Calling this with NULL sets as a portable way to sleep with sub-second
 *   accuracy is not supported.
 * */
PHPAPI int php_select(php_socket_t max_fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	ULONGLONG ms_total, limit;
	HANDLE handles[MAXIMUM_WAIT_OBJECTS];
	int handle_slot_to_fd[MAXIMUM_WAIT_OBJECTS];
	int n_handles = 0, i;
	int num_read_pipes = 0;
	php_growable_fd_set sock_read = {0}, sock_write = {0}, sock_except = {0};
	php_growable_fd_set aread = {0}, awrite = {0}, aexcept = {0};
	int sock_max_fd = -1;
	struct timeval tvslice;
	int retcode = -1;

	/* As max_fd is unsigned, non socket might overflow. */
	if (max_fd > (php_socket_t)INT_MAX) {
		return -1;
	}

#define SAFE_FD_ISSET(fd, set)	(set != NULL && FD_ISSET(fd, set))

	/* calculate how long we need to wait in milliseconds */
	if (tv == NULL) {
		ms_total = INFINITE;
	} else {
		ms_total = tv->tv_sec * 1000;
		ms_total += tv->tv_usec / 1000;
	}

	php_growable_fd_set_init(&sock_read, FD_SETSIZE);
	php_growable_fd_set_init(&sock_write, FD_SETSIZE);
	php_growable_fd_set_init(&sock_except, FD_SETSIZE);

	/* build an array of handles for non-sockets */
	for (i = 0; (uint32_t)i < max_fd; i++) {
		if (SAFE_FD_ISSET(i, rfds) || SAFE_FD_ISSET(i, wfds) || SAFE_FD_ISSET(i, efds)) {
			int _type;
			int _len = sizeof(_type);

			if (getsockopt((SOCKET)i, SOL_SOCKET, SO_TYPE, (char*)&_type, &_len) == 0 || WSAGetLastError() != WSAENOTSOCK) {
				/* socket */
				if (SAFE_FD_ISSET(i, rfds)) {
					php_growable_fd_set_add(&sock_read, (SOCKET)(uintptr_t)i);
				}
				if (SAFE_FD_ISSET(i, wfds)) {
					php_growable_fd_set_add(&sock_write, (SOCKET)(uintptr_t)i);
				}
				if (SAFE_FD_ISSET(i, efds)) {
					php_growable_fd_set_add(&sock_except, (SOCKET)(uintptr_t)i);
				}
				if (i > sock_max_fd) {
					sock_max_fd = i;
				}
			} else {
				HANDLE handle = (HANDLE)(uintptr_t)_get_osfhandle(i);
				if (handle != INVALID_HANDLE_VALUE) {
					if (n_handles >= MAXIMUM_WAIT_OBJECTS) {
						/* WaitForMultipleObjects() cannot wait on more than
						 * MAXIMUM_WAIT_OBJECTS (64) handles at once. Fail
						 * gracefully (the caller turns -1 into a warning and
						 * false) instead of overflowing the fixed-size
						 * handles[]/handle_slot_to_fd[] stack arrays. */
						errno = EINVAL;
						retcode = -1;
						goto cleanup;
					}
					handles[n_handles] = handle;
					if (SAFE_FD_ISSET(i, rfds) && GetFileType(handle) == FILE_TYPE_PIPE) {
						num_read_pipes++;
					}
					handle_slot_to_fd[n_handles] = i;
					n_handles++;
				}
			}
		}
	}

	if (n_handles == 0) {
		/* plain sockets only - let winsock handle the whole thing. rfds/wfds/efds
		 * are growable sets, so this is no longer bounded by FD_SETSIZE. */
		retcode = select(-1, rfds, wfds, efds, tv);
		goto cleanup;
	}

	/* mixture of handles and sockets; lets multiplex between
	 * winsock and waiting on the handles */

	php_growable_fd_set_init(&aread, sock_read.set->fd_count);
	php_growable_fd_set_init(&awrite, sock_write.set->fd_count);
	php_growable_fd_set_init(&aexcept, sock_except.set->fd_count);

	limit = GetTickCount64() + ms_total;
	do {
		retcode = 0;

		if (sock_max_fd >= 0) {
			/* refresh the working copies; the select call will clear the fds
			 * that are not active. memcpy (via _copy) instead of struct
			 * assignment because the sets are dynamically sized. */
			php_growable_fd_set_copy(&aread, &sock_read);
			php_growable_fd_set_copy(&awrite, &sock_write);
			php_growable_fd_set_copy(&aexcept, &sock_except);

			tvslice.tv_sec = 0;
			tvslice.tv_usec = 100000;

			retcode = select(-1, aread.set, awrite.set, aexcept.set, &tvslice);
		} else {
			php_growable_fd_set_zero(&aread);
			php_growable_fd_set_zero(&awrite);
			php_growable_fd_set_zero(&aexcept);
		}
		if (n_handles > 0) {
			/* check handles */
			DWORD wret;

			wret = WaitForMultipleObjects(n_handles, handles, FALSE, retcode > 0 ? 0 : 100);

			if (wret == WAIT_TIMEOUT) {
				/* set retcode to 0; this is the default.
				 * select() may have set it to something else,
				 * in which case we leave it alone, so this branch
				 * does nothing */
				;
			} else if (wret == WAIT_FAILED) {
				if (retcode == 0) {
					retcode = -1;
				}
			} else {
				if (retcode < 0) {
					retcode = 0;
				}
				for (i = 0; i < n_handles; i++) {
					if (WAIT_OBJECT_0 == WaitForSingleObject(handles[i], 0)) {
						if (SAFE_FD_ISSET(handle_slot_to_fd[i], rfds)) {
							DWORD avail_read = 0;
							if (num_read_pipes < n_handles
								|| !PeekNamedPipe(handles[i], NULL, 0, NULL, &avail_read, NULL)
								|| avail_read > 0
							) {
								php_growable_fd_set_add(&aread, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
								retcode++;
							}
						}
						if (SAFE_FD_ISSET(handle_slot_to_fd[i], wfds)) {
							php_growable_fd_set_add(&awrite, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
							retcode++;
						}
						if (SAFE_FD_ISSET(handle_slot_to_fd[i], efds)) {
							php_growable_fd_set_add(&aexcept, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
							retcode++;
						}
					}
				}
			}
		}
		if (retcode == 0 && num_read_pipes == n_handles && sock_max_fd < 0) {
			usleep(100);
		}
	} while (retcode == 0 && (ms_total == INFINITE || GetTickCount64() < limit));

	/* Copy the results back into the caller's sets. memcpy (not struct
	 * assignment) because the sets are dynamically sized; only fd_count + the
	 * used fd_array entries are written. This never overflows the caller's
	 * buffer: every fd in a* was present in the corresponding input set, so
	 * a*->fd_count <= the input fd_count, which the caller's buffer already
	 * held. */
	if (rfds) {
		memcpy(rfds, aread.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(aread.set->fd_count));
	}
	if (wfds) {
		memcpy(wfds, awrite.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(awrite.set->fd_count));
	}
	if (efds) {
		memcpy(efds, aexcept.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(aexcept.set->fd_count));
	}

cleanup:
	php_growable_fd_set_destroy(&sock_read);
	php_growable_fd_set_destroy(&sock_write);
	php_growable_fd_set_destroy(&sock_except);
	php_growable_fd_set_destroy(&aread);
	php_growable_fd_set_destroy(&awrite);
	php_growable_fd_set_destroy(&aexcept);

	return retcode;
}
