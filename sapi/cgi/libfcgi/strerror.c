/*
 * The terms in the file "LICENSE.TERMS" do not apply to this file.
 * See terms below.
 *
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)strerror.c	5.6 (Berkeley) 5/4/91";*/
static char *rcsid = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include "fcgi_config.h"

#if ! defined (HAVE_STRERROR)
#include <string.h>

/*
 * Since perror() is not allowed to change the contents of strerror()'s
 * static buffer, both functions supply their own buffers to the
 * internal function __strerror().
 */

char *
__strerror(int num, char *buf)
{
#define	UPREFIX	"Unknown error: "
	extern char *sys_errlist[];
	extern int sys_nerr;
	register unsigned int errnum;
	register char *p, *t;
	char tmp[40];

	errnum = num;				/* convert to unsigned */
	if (errnum < sys_nerr)
		return(sys_errlist[errnum]);

	/* Do this by hand, so we don't include stdio(3). */
	t = tmp;
	do {
		*t++ = "0123456789"[errnum % 10];
	} while (errnum /= 10);

	strcpy (buf, UPREFIX);
	for (p = buf + sizeof(UPREFIX) -1;;) {
		*p++ = *--t;
		if (t <= tmp)
			break;
	}

	return buf;
}


char *
strerror(int num)
{
	static char buf[40];			/* 64-bit number + slop */
	return __strerror(num, buf);
}

#endif
