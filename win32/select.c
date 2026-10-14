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

/* Which of the read/write/except sets a non-socket handle belongs to. */
#define PHP_SELECT_SET_READ   1
#define PHP_SELECT_SET_WRITE  2
#define PHP_SELECT_SET_EXCEPT 4

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
	int handle_dirs[MAXIMUM_WAIT_OBJECTS];
	int n_handles = 0, i;
	int num_read_pipes = 0;
	php_growable_fd_set sock_read = {0}, sock_write = {0}, sock_except = {0};
	php_growable_fd_set aread = {0}, awrite = {0}, aexcept = {0};
	bool has_sockets = false;
	struct timeval tvslice;
	int retcode = -1;

	/* As max_fd is unsigned, non socket might overflow. */
	if (max_fd > (php_socket_t)INT_MAX) {
		return -1;
	}

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

	/* Split each input set into sockets (driven by winsock select()) and
	 * non-socket handles (pipes/files, waited on with WaitForMultipleObjects()).
	 * A single linear pass over the three fd_arrays; max_fd is not an iteration
	 * bound, as a socket handle value can be very large. */
	{
		fd_set *in_sets[3];
		php_growable_fd_set *sock_sets[3];
		int set_dirs[3];

		in_sets[0] = rfds; sock_sets[0] = &sock_read;   set_dirs[0] = PHP_SELECT_SET_READ;
		in_sets[1] = wfds; sock_sets[1] = &sock_write;  set_dirs[1] = PHP_SELECT_SET_WRITE;
		in_sets[2] = efds; sock_sets[2] = &sock_except; set_dirs[2] = PHP_SELECT_SET_EXCEPT;

		for (int s = 0; s < 3; s++) {
			fd_set *set = in_sets[s];

			if (set == NULL) {
				continue;
			}
			for (u_int k = 0; k < set->fd_count; k++) {
				php_socket_t fd = set->fd_array[k];
				int _type;
				int _len = sizeof(_type);

				if (getsockopt((SOCKET)fd, SOL_SOCKET, SO_TYPE, (char*)&_type, &_len) == 0 || WSAGetLastError() != WSAENOTSOCK) {
					/* socket */
					php_growable_fd_set_add(sock_sets[s], fd);
					has_sockets = true;
				} else {
					HANDLE handle = (HANDLE)(uintptr_t)_get_osfhandle((int)fd);
					int j;

					if (handle == INVALID_HANDLE_VALUE) {
						continue;
					}
					/* Dedupe the handle across the read/write/except sets; at most
					 * MAXIMUM_WAIT_OBJECTS entries, so this scan stays short. */
					for (j = 0; j < n_handles; j++) {
						if (handle_slot_to_fd[j] == (int)fd) {
							handle_dirs[j] |= set_dirs[s];
							break;
						}
					}
					if (j < n_handles) {
						continue;
					}
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
					handle_slot_to_fd[n_handles] = (int)fd;
					handle_dirs[n_handles] = set_dirs[s];
					n_handles++;
				}
			}
		}
	}

	/* How many of the handles are pipes in the read set (for the data-available
	 * peek further down). */
	for (i = 0; i < n_handles; i++) {
		if ((handle_dirs[i] & PHP_SELECT_SET_READ) && GetFileType(handles[i]) == FILE_TYPE_PIPE) {
			num_read_pipes++;
		}
	}

	/* De-duplicate each socket set (a socket may be listed more than once) and
	 * leave it sorted - for select() and for the binary-search isset() back in
	 * stream_array_from_fd_set(). */
	php_growable_fd_set_sort_unique(&sock_read);
	php_growable_fd_set_sort_unique(&sock_write);
	php_growable_fd_set_sort_unique(&sock_except);

	if (n_handles == 0) {
		/* plain sockets only - let winsock handle the whole thing. The sets are
		 * growable, so this is no longer bounded by FD_SETSIZE. Pass NULL where
		 * the caller's set was NULL, so the call matches the pre-split one. */
		retcode = select(-1, rfds ? sock_read.set : NULL, wfds ? sock_write.set : NULL, efds ? sock_except.set : NULL, tv);
		if (retcode >= 0) {
			/* select() rewrote the arrays with the ready sockets, unordered;
			 * re-sort so the caller can binary-search, then copy back. */
			php_growable_fd_set_sort_unique(&sock_read);
			php_growable_fd_set_sort_unique(&sock_write);
			php_growable_fd_set_sort_unique(&sock_except);
			if (rfds) {
				memcpy(rfds, sock_read.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(sock_read.set->fd_count));
			}
			if (wfds) {
				memcpy(wfds, sock_write.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(sock_write.set->fd_count));
			}
			if (efds) {
				memcpy(efds, sock_except.set, PHP_GROWABLE_FD_SET_ALLOC_SIZE(sock_except.set->fd_count));
			}
		}
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

		if (has_sockets) {
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
						if (handle_dirs[i] & PHP_SELECT_SET_READ) {
							DWORD avail_read = 0;
							if (num_read_pipes < n_handles
								|| !PeekNamedPipe(handles[i], NULL, 0, NULL, &avail_read, NULL)
								|| avail_read > 0
							) {
								php_growable_fd_set_add(&aread, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
								retcode++;
							}
						}
						if (handle_dirs[i] & PHP_SELECT_SET_WRITE) {
							php_growable_fd_set_add(&awrite, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
							retcode++;
						}
						if (handle_dirs[i] & PHP_SELECT_SET_EXCEPT) {
							php_growable_fd_set_add(&aexcept, (SOCKET)(uintptr_t)handle_slot_to_fd[i]);
							retcode++;
						}
					}
				}
			}
		}
		if (retcode == 0 && num_read_pipes == n_handles && !has_sockets) {
			usleep(100);
		}
	} while (retcode == 0 && (ms_total == INFINITE || GetTickCount64() < limit));

	/* Copy the results back into the caller's sets, sorted so the caller can
	 * binary-search them. memcpy (not struct assignment) because the sets are
	 * dynamically sized; only fd_count + the used fd_array entries are written.
	 * This never overflows the caller's buffer: every fd in a* was present in
	 * the corresponding input set, so a*->fd_count <= the input fd_count, which
	 * the caller's buffer already held. */
	php_growable_fd_set_sort_unique(&aread);
	php_growable_fd_set_sort_unique(&awrite);
	php_growable_fd_set_sort_unique(&aexcept);
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
