/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include <errno.h>
#include "ext/standard/flock_compat.h"

#ifndef HAVE_FLOCK
PHPAPI int flock(int fd, int operation)
{
	return php_flock(fd, operation);
}
#endif /* !defined(HAVE_FLOCK) */

PHPAPI int php_flock(int fd, int operation)
#ifdef HAVE_STRUCT_FLOCK /* {{{ */
{
	struct flock flck;
	int ret;

	flck.l_start = flck.l_len = 0;
	flck.l_whence = SEEK_SET;

	if (operation & LOCK_SH)
		flck.l_type = F_RDLCK;
	else if (operation & LOCK_EX)
		flck.l_type = F_WRLCK;
	else if (operation & LOCK_UN)
		flck.l_type = F_UNLCK;
	else {
		errno = EINVAL;
		return -1;
	}

	ret = fcntl(fd, operation & LOCK_NB ? F_SETLK : F_SETLKW, &flck);

	if ((operation & LOCK_NB) && ret == -1 &&
			(errno == EACCES || errno == EAGAIN))
		errno = EWOULDBLOCK;

	if (ret != -1) ret = 0;

	return ret;
}
/* }}} */
#elif defined(PHP_WIN32) /* {{{ */
/*
 * Program:   Unix compatibility routines
 *
 * Author:  Mark Crispin
 *      Networks and Distributed Computing
 *      Computing & Communications
 *      University of Washington
 *      Administration Building, AG-44
 *      Seattle, WA  98195
 *      Internet: MRC@CAC.Washington.EDU
 *
 * Date:    14 September 1996
 * Last Edited: 14 August 1997
 *
 * Copyright 1997 by the University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * above copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Washington not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  This software is made available
 * "as is", and
 * THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
 * NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*              DEDICATION

 *  This file is dedicated to my dog, Unix, also known as Yun-chan and
 * Unix J. Terwilliker Jehosophat Aloysius Monstrosity Animal Beast.  Unix
 * passed away at the age of 11 1/2 on September 14, 1996, 12:18 PM PDT, after
 * a two-month bout with cirrhosis of the liver.
 *
 *  He was a dear friend, and I miss him terribly.
 *
 *  Lift a leg, Yunie.  Luv ya forever!!!!
 */
{
	HANDLE hdl = (HANDLE) _get_osfhandle(fd);
	DWORD low = 0xFFFFFFFF, high = 0xFFFFFFFF;
	OVERLAPPED offset =
	{0, 0, 0, 0, NULL};
	DWORD err;

	if (INVALID_HANDLE_VALUE == hdl) {
		_set_errno(EBADF);
		return -1;              /* error in file descriptor */
	}
	/* bug for bug compatible with Unix */
	UnlockFileEx(hdl, 0, low, high, &offset);
	switch (operation & ~LOCK_NB) {    /* translate to LockFileEx() op */
		case LOCK_EX:           /* exclusive */
			if (LockFileEx(hdl, LOCKFILE_EXCLUSIVE_LOCK +
						((operation & LOCK_NB) ? LOCKFILE_FAIL_IMMEDIATELY : 0),
						   0, low, high, &offset))
				return 0;
			break;
		case LOCK_SH:           /* shared */
			if (LockFileEx(hdl, ((operation & LOCK_NB) ? LOCKFILE_FAIL_IMMEDIATELY : 0),
						   0, low, high, &offset))
				return 0;
			break;
		case LOCK_UN:           /* unlock */
			return 0;           /* always succeeds */
		default:                /* default */
			break;
	}

	err = GetLastError();
	if (ERROR_LOCK_VIOLATION == err || ERROR_SHARING_VIOLATION == err) {
		_set_errno(EWOULDBLOCK);
	} else {
		_set_errno(EINVAL);             /* bad call */
	}

	return -1;
}
/* }}} */
#else
#warning no proper flock support for your site
{
	errno = 0;
	return 0;
}
#endif
