/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sas@schell.de>                             |
   +----------------------------------------------------------------------+

   $Id$
 */


#include <php.h>
#include <errno.h>
#include "php_compat.h"

#if HAVE_STRUCT_FLOCK
#include <unistd.h>
#include <fcntl.h>
#endif

#if WIN32|WINNT
#include <windows.h>
#include <io.h>
#endif

#if !HAVE_FLOCK
PHPAPI int flock(int fd, int operation)
#if HAVE_STRUCT_FLOCK
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

	if (operation & LOCK_NB && ret == -1 && 
			(errno == EACCES || errno == EAGAIN))
		errno = EWOULDBLOCK;

	if (ret != -1) ret = 0;

	return ret;
}
#elif WIN32|WINNT
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
    DWORD low = 1, high = 0;
    OVERLAPPED offset =
    {0, 0, 0, 0, NULL};
    if (hdl < 0)
        return -1;              /* error in file descriptor */
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
	/* Under Win32 MT library, errno is not a variable but a function call,
	 * which cannot be assigned to.
	 */
#if !(WIN32|WINNT)
    errno = EINVAL;             /* bad call */
#endif
    return -1;
}
#else
#warning no proper flock support for your site
{
	errno = 0;
	return 0;
}
#endif
#endif /* !HAVE_FLOCK */

#if !(HAVE_INET_ATON)

/* 
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */

PHPAPI int
inet_aton(const char *cp, struct in_addr *ap)
{
    int dots = 0;
    register unsigned long acc = 0, addr = 0;

    do {
        register char cc = *cp;

        switch (cc) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            acc = acc * 10 + (cc - '0');
            break;

        case '.':
            if (++dots > 3) {
                return 0;
            }
            /* Fall through */

        case '\0':
            if (acc > 255) {
                return 0;
            }
            addr = addr << 8 | acc;
            acc = 0;
            break;

        default:
            return 0;
        }
    } while (*cp++) ;

    /* Normalize the address */
    if (dots < 3) {
        addr <<= 8 * (3 - dots) ;
    }

    /* Store it if requested */
    if (ap) {
        ap->s_addr = htonl(addr);
    }

    return 1;    
}

#endif /* !HAVE_INET_ATON */
