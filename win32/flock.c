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
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
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

#include "php.h"
#include <windows.h>
#include <io.h>
#include <errno.h>
#include "flock.h"

PHPAPI int flock(int fd, int op)
{
	HANDLE hdl = (HANDLE) _get_osfhandle(fd);
	DWORD low = 1, high = 0;
	OVERLAPPED offset =
	{0, 0, 0, 0, NULL};
	if (hdl < 0)
		return -1;				/* error in file descriptor */
	/* bug for bug compatible with Unix */
	UnlockFileEx(hdl, 0, low, high, &offset);
	switch (op & ~LOCK_NB) {	/* translate to LockFileEx() op */
		case LOCK_EX:			/* exclusive */
			if (LockFileEx(hdl, LOCKFILE_EXCLUSIVE_LOCK +
						((op & LOCK_NB) ? LOCKFILE_FAIL_IMMEDIATELY : 0),
						   0, low, high, &offset))
				return 0;
			break;
		case LOCK_SH:			/* shared */
			if (LockFileEx(hdl, ((op & LOCK_NB) ? LOCKFILE_FAIL_IMMEDIATELY : 0),
						   0, low, high, &offset))
				return 0;
			break;
		case LOCK_UN:			/* unlock */
			return 0;			/* always succeeds */
		default:				/* default */
			break;
	}
	errno = EINVAL;				/* bad call */
	return -1;
}
