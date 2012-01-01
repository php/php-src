/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_network.h"

#ifdef PHP_WIN32

/* $Id$ */

/* Win32 select() will only work with sockets, so we roll our own implementation here.
 * - If you supply only sockets, this simply passes through to winsock select().
 * - If you supply file handles, there is no way to distinguish between
 *   ready for read/write or OOB, so any set in which the handle is found will
 *   be marked as ready.
 * - If you supply a mixture of handles and sockets, the system will interleave
 *   calls between select() and WaitForMultipleObjects(). The time slicing may
 *   cause this function call to take up to 100 ms longer than you specified.
 * - Calling this with NULL sets as a portable way to sleep with sub-second
 *   accuracy is not supported.
 * */
PHPAPI int php_select(int max_fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	DWORD ms_total, limit;
	HANDLE handles[MAXIMUM_WAIT_OBJECTS];
	int handle_slot_to_fd[MAXIMUM_WAIT_OBJECTS];
	int n_handles = 0, i;
	fd_set sock_read, sock_write, sock_except;
	fd_set aread, awrite, aexcept;
	int sock_max_fd = -1;
	struct timeval tvslice;
	int retcode;

#define SAFE_FD_ISSET(fd, set)	(set != NULL && FD_ISSET(fd, set))
	
	/* calculate how long we need to wait in milliseconds */
	if (tv == NULL) {
		ms_total = INFINITE;
	} else {
		ms_total = tv->tv_sec * 1000;
		ms_total += tv->tv_usec / 1000;
	}

	FD_ZERO(&sock_read);
	FD_ZERO(&sock_write);
	FD_ZERO(&sock_except);
	
	/* build an array of handles for non-sockets */
	for (i = 0; i < max_fd; i++) {
		if (SAFE_FD_ISSET(i, rfds) || SAFE_FD_ISSET(i, wfds) || SAFE_FD_ISSET(i, efds)) {
			handles[n_handles] = (HANDLE)(zend_uintptr_t)_get_osfhandle(i);
			if (handles[n_handles] == INVALID_HANDLE_VALUE) {
				/* socket */
				if (SAFE_FD_ISSET(i, rfds)) {
					FD_SET((uint)i, &sock_read);
				}
				if (SAFE_FD_ISSET(i, wfds)) {
					FD_SET((uint)i, &sock_write);
				}
				if (SAFE_FD_ISSET(i, efds)) {
					FD_SET((uint)i, &sock_except);
				}
				if (i > sock_max_fd) {
					sock_max_fd = i;
				}
			} else {
				handle_slot_to_fd[n_handles] = i;
				n_handles++;
			}
		}
	}

	if (n_handles == 0) {
		/* plain sockets only - let winsock handle the whole thing */
		return select(max_fd, rfds, wfds, efds, tv);
	}
	
	/* mixture of handles and sockets; lets multiplex between
	 * winsock and waiting on the handles */

	FD_ZERO(&aread);
	FD_ZERO(&awrite);
	FD_ZERO(&aexcept);
	
	limit = GetTickCount() + ms_total;
	do {
		retcode = 0;
	
		if (sock_max_fd >= 0) {
			/* overwrite the zero'd sets here; the select call
			 * will clear those that are not active */
			aread = sock_read;
			awrite = sock_write;
			aexcept = sock_except;

			tvslice.tv_sec = 0;
			tvslice.tv_usec = 100000;

			retcode = select(sock_max_fd+1, &aread, &awrite, &aexcept, &tvslice);
		}
		if (n_handles > 0) {
			/* check handles */
			DWORD wret;

			wret = MsgWaitForMultipleObjects(n_handles, handles, FALSE, retcode > 0 ? 0 : 100, QS_ALLEVENTS);

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
							FD_SET((uint)handle_slot_to_fd[i], &aread);
						}
						if (SAFE_FD_ISSET(handle_slot_to_fd[i], wfds)) {
							FD_SET((uint)handle_slot_to_fd[i], &awrite);
						}
						if (SAFE_FD_ISSET(handle_slot_to_fd[i], efds)) {
							FD_SET((uint)handle_slot_to_fd[i], &aexcept);
						}
						retcode++;
					}
				}
			}
		}
	} while (retcode == 0 && (ms_total == INFINITE || GetTickCount() < limit));

	if (rfds) {
		*rfds = aread;
	}
	if (wfds) {
		*wfds = awrite;
	}
	if (efds) {
		*efds = aexcept;
	}	
	
	return retcode;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
