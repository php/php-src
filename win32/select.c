/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Win32 select() will only work with sockets, so we roll our own implementation that will
 * get the OS file handle from regular fd's and sockets and then use WaitForMultipleObjects().
 * This implementation is not as feature-full as posix select, but it works for our purposes
 */
PHPAPI int php_select(int max_fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	HANDLE *handles;
	DWORD waitret;
	DWORD ms_total;
	int i, f, s, fd_count = 0, sock_count = 0;
	int retval;
	fd_set ard, awr, aex; /* active fd sets */

	for (i = 0; i < max_fd; i++) {
		if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
			if (_get_osfhandle(i) == 0xffffffff) {
				/* it is a socket */
				sock_count++;
			} else {
				fd_count++;
			}
		}
	}

	if (fd_count + sock_count == 0) {
		return 0;
	}

	handles = (HANDLE*)emalloc((fd_count + sock_count) * sizeof(HANDLE));

	/* populate the events and handles arrays */
	f = 0;
	s = 0;
	for (i = 0; i < max_fd; i++) {
		if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
			long h = _get_osfhandle(i);
			if (h == 0xFFFFFFFF) {
				HANDLE evt;
				long evt_flags = 0;

				if (FD_ISSET(i, rfds)) {
					evt_flags |= FD_READ|FD_CONNECT|FD_ACCEPT|FD_CLOSE;
				}
				if (FD_ISSET(i, wfds)) {
					evt_flags |= FD_WRITE;
				}
				if (FD_ISSET(i, efds)) {
					evt_flags |= FD_OOB;
				}

				evt = WSACreateEvent();
				WSAEventSelect(i, evt, evt_flags); 

				handles[fd_count + s] = evt;
				s++;
			} else {
				handles[f++] = (HANDLE)h;
			}
		}
	}

	/* calculate how long we need to wait in milliseconds */
	if (tv == NULL) {
		ms_total = INFINITE;
	} else {
		ms_total = tv->tv_sec * 1000;
		ms_total += tv->tv_usec / 1000;
	}

	waitret = MsgWaitForMultipleObjects(fd_count + sock_count, handles, FALSE, ms_total, QS_ALLEVENTS);

	if (waitret == WAIT_TIMEOUT) {
		retval = 0;
	} else if (waitret == 0xFFFFFFFF) {
		retval = -1;
	} else {

		FD_ZERO(&ard);
		FD_ZERO(&awr);
		FD_ZERO(&aex);

		f = 0;
		retval = 0;
		for (i = 0; i < max_fd; i++) {
			if (FD_ISSET(i, rfds) || FD_ISSET(i, wfds) || FD_ISSET(i, efds)) {
				if (f >= fd_count) {
					/* socket event */
					HANDLE evt = handles[f];

					if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
						/* check for various signal states */
						if (FD_ISSET(i, rfds)) {
							WSAEventSelect(i, evt, FD_READ|FD_CONNECT|FD_ACCEPT|FD_CLOSE);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &ard);
							}
						}
						if (FD_ISSET(i, wfds)) {
							WSAEventSelect(i, evt, FD_WRITE);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &awr);
							}
						}
						if (FD_ISSET(i, efds)) {
							WSAEventSelect(i, evt, FD_OOB);
							if (WAIT_OBJECT_0 == WaitForSingleObject(evt, 0)) {
								FD_SET(i, &aex);
							}
						}
						retval++;
					}

					WSACloseEvent(evt);

				} else {
					if (WAIT_OBJECT_0 == WaitForSingleObject(handles[f], 0)) {
						if (FD_ISSET(i, rfds)) {
							FD_SET(i, &ard);
						}
						if (FD_ISSET(i, wfds)) {
							FD_SET(i, &awr);
						}
						if (FD_ISSET(i, efds)) {
							FD_SET(i, &aex);
						}
						retval++;
					}

				}
				f++;
			}
		}

		if (rfds) {
			*rfds = ard;
		}
		if (wfds) {
			*wfds = awr;
		}
		if (efds) {
			*efds = aex;
		}
	}

	efree(handles);

	return retval;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
