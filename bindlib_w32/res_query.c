/*
 * ++Copyright++ 1988, 1993
 * -
 * Copyright (c) 1988, 1993
 *    The Regents of the University of California.  All rights reserved.
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
 * 	This product includes software developed by the University of
 * 	California, Berkeley and its contributors.
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
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)res_query.c	8.1 (Berkeley) 6/4/93";
static char rcsid[] = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#ifndef WINNT
#include <sys/param.h>
#include <netinet/in.h>
#endif
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <stdio.h>
#include <netdb.h>
#include <resolv.h>
#include <ctype.h>
#include <errno.h>
#if defined(BSD) && (BSD >= 199306)
# include <stdlib.h>
# include <string.h>
#else
# include "conf/portability.h"
#endif

#if defined(USE_OPTIONS_H)
# include <conf/options.h>
#endif

#if PACKETSZ > 1024
#define MAXPACKET	PACKETSZ
#else
#define MAXPACKET	1024
#endif

const char *hostalias __P((const char *));
#ifndef WINNT
int h_errno;
#endif

/*
 * Formulate a normal query, send, and await answer.
 * Returned answer is placed in supplied buffer "answer".
 * Perform preliminary check of answer, returning success only
 * if no error is indicated and the answer count is nonzero.
 * Return the size of the response on success, -1 on error.
 * Error number is left in h_errno.
 *
 * Caller must parse answer and determine whether it answers the question.
 */
int
res_query(name, class, type, answer, anslen)
	const char *name;	/* domain name */
	int class, type;	/* class and type of query */
	u_char *answer;		/* buffer to put answer */
	int anslen;		/* size of answer buffer */
{
	u_char buf[MAXPACKET];
	register HEADER *hp = (HEADER *) answer;
	int n;

	hp->rcode = NOERROR;	/* default */

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (-1);
	}
#ifdef DEBUG
	if (_res.options & RES_DEBUG)
		printf(";; res_query(%s, %d, %d)\n", name, class, type);
#endif

	n = res_mkquery(QUERY, name, class, type, NULL, 0, NULL,
			buf, sizeof(buf));
	if (n <= 0) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf(";; res_query: mkquery failed\n");
#endif
#ifndef WINNT
		h_errno = NO_RECOVERY;
#else
		WSASetLastError(NO_RECOVERY);
#endif
		return (n);
	}
	n = res_send(buf, n, answer, anslen);
	if (n < 0) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf(";; res_query: send error\n");
#endif
#ifndef WINNT
		h_errno = TRY_AGAIN;
#else
		WSASetLastError(TRY_AGAIN);
#endif
		return (n);
	}

	if (hp->rcode != NOERROR || ntohs( (u_short) hp->ancount) == 0) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf(";; rcode = %d, ancount=%d\n", hp->rcode,
			    ntohs( (u_short) hp->ancount));
#endif
		switch (hp->rcode) {
		case NXDOMAIN:
#ifndef WINNT
			h_errno = HOST_NOT_FOUND;
#else
			WSASetLastError(HOST_NOT_FOUND);
#endif
			break;
		case SERVFAIL:
#ifndef WINNT
			h_errno = TRY_AGAIN;
#else
			WSASetLastError(TRY_AGAIN);
#endif
			break;
		case NOERROR:
#ifndef WINNT
			h_errno = NO_DATA;
#else
			WSASetLastError(NO_DATA);
#endif
			break;
		case FORMERR:
		case NOTIMP:
		case REFUSED:
		default:
#ifndef WINNT
			h_errno = NO_RECOVERY;
#else
			WSASetLastError(NO_RECOVERY);
#endif
			break;
		}
		return (-1);
	}
	return (n);
}

/*
 * Formulate a normal query, send, and retrieve answer in supplied buffer.
 * Return the size of the response on success, -1 on error.
 * If enabled, implement search rules until answer or unrecoverable failure
 * is detected.  Error code, if any, is left in h_errno.
 */
int
res_search(name, class, type, answer, anslen)
	const char *name;	/* domain name */
	int class, type;	/* class and type of query */
	u_char *answer;		/* buffer to put answer */
	int anslen;		/* size of answer */
{
	register const char *cp, * const *domain;
	HEADER *hp = (HEADER *) answer;
	u_int dots;
	int trailing_dot, ret, saved_herrno;
	int got_nodata = 0, got_servfail = 0, tried_as_is = 0;

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (-1);
	}
#ifndef WINNT
	errno = 0;
	h_errno = HOST_NOT_FOUND;	/* default, if we never query */
#else
	WSASetLastError(HOST_NOT_FOUND);
	SetLastError(0);
#endif
	dots = 0;
	for (cp = name; *cp; cp++)
		dots += (*cp == '.');
	trailing_dot = 0;
	if (cp > name && *--cp == '.')
		trailing_dot++;

	/*
	 * if there aren't any dots, it could be a user-level alias
	 */
	if (!dots && (cp = __hostalias(name)) != NULL)
		return (res_query(cp, class, type, answer, anslen));

	/*
	 * If there are dots in the name already, let's just give it a try
	 * 'as is'.  The threshold can be set with the "ndots" option.
	 */
	saved_herrno = -1;
	if (dots >= _res.ndots) {
		ret = res_querydomain(name, NULL, class, type, answer, anslen);
		if (ret > 0)
			return (ret);
#ifndef WINNT
		saved_herrno = h_errno;
#else
                saved_herrno = WSAGetLastError();
#endif
		tried_as_is++;
	}

	/*
	 * We do at least one level of search if
	 *	- there is no dot and RES_DEFNAME is set, or
	 *	- there is at least one dot, there is no trailing dot,
	 *	  and RES_DNSRCH is set.
	 */
	if ((!dots && (_res.options & RES_DEFNAMES)) ||
	    (dots && !trailing_dot && (_res.options & RES_DNSRCH))) {
		int done = 0;

		for (domain = (const char * const *)_res.dnsrch;
		     *domain && !done;
		     domain++) {

			ret = res_querydomain(name, *domain, class, type,
					      answer, anslen);
			if (ret > 0)
				return (ret);

			/*
			 * If no server present, give up.
			 * If name isn't found in this domain,
			 * keep trying higher domains in the search list
			 * (if that's enabled).
			 * On a NO_DATA error, keep trying, otherwise
			 * a wildcard entry of another type could keep us
			 * from finding this entry higher in the domain.
			 * If we get some other error (negative answer or
			 * server failure), then stop searching up,
			 * but try the input name below in case it's
			 * fully-qualified.
			 */
			if (errno == ECONNREFUSED) {
#ifndef WINNT
				h_errno = TRY_AGAIN;
#else
				WSASetLastError(TRY_AGAIN);
#endif
				return (-1);
			}

			switch (h_errno) {
			case NO_DATA:
				got_nodata++;
				/* FALLTHROUGH */
			case HOST_NOT_FOUND:
				/* keep trying */
				break;
			case TRY_AGAIN:
				if (hp->rcode == SERVFAIL) {
					/* try next search element, if any */
					got_servfail++;
					break;
				}
				/* FALLTHROUGH */
			default:
				/* anything else implies that we're done */
				done++;
			}

			/* if we got here for some reason other than DNSRCH,
			 * we only wanted one iteration of the loop, so stop.
			 */
			if (!(_res.options & RES_DNSRCH))
				done++;
		}
	}

	/* if we have not already tried the name "as is", do that now.
	 * note that we do this regardless of how many dots were in the
	 * name or whether it ends with a dot.
	 */
	if (!tried_as_is) {
		ret = res_querydomain(name, NULL, class, type, answer, anslen);
		if (ret > 0)
			return (ret);
	}

	/* if we got here, we didn't satisfy the search.
	 * if we did an initial full query, return that query's h_errno
	 * (note that we wouldn't be here if that query had succeeded).
	 * else if we ever got a nodata, send that back as the reason.
	 * else send back meaningless h_errno, that being the one from
	 * the last DNSRCH we did.
	 */
#ifndef WINNT
	if (saved_herrno != -1)
		h_errno = saved_herrno;
	else if (got_nodata)
		h_errno = NO_DATA;
	else if (got_servfail)
		h_errno = TRY_AGAIN;
#else
	if (saved_herrno != -1)
		WSASetLastError(saved_herrno);
	else if (got_nodata)
		WSASetLastError(NO_DATA);
	else if (got_servfail)
		WSASetLastError(TRY_AGAIN);
#endif
	return (-1);
}

/*
 * Perform a call on res_query on the concatenation of name and domain,
 * removing a trailing dot from name if domain is NULL.
 */
int
res_querydomain(name, domain, class, type, answer, anslen)
	const char *name, *domain;
	int class, type;	/* class and type of query */
	u_char *answer;		/* buffer to put answer */
	int anslen;		/* size of answer */
{
	char nbuf[MAXDNAME];
	const char *longname = nbuf;
	int n, d;

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (-1);
	}
#ifdef DEBUG
	if (_res.options & RES_DEBUG)
		printf(";; res_querydomain(%s, %s, %d, %d)\n",
		       name, domain?domain:"<Nil>", class, type);
#endif
	if (domain == NULL) {
		/*
		 * Check for trailing '.';
		 * copy without '.' if present.
		 */
		n = strlen(name);
		if (n >= MAXDNAME) {
#ifndef WINNT
		h_errno = NO_RECOVERY;
#else
		WSASetLastError(NO_RECOVERY);
#endif
			return (-1);
		}
		n--;
		if (n >= 0 && name[n] == '.') {
			strncpy(nbuf, name, n);
			nbuf[n] = '\0';
		} else
			longname = name;
	} else {
		n = strlen(name);
		d = strlen(domain);
		if (n + d + 1 >= MAXDNAME) {
#ifndef WINNT
		h_errno = NO_RECOVERY;
#else
		WSASetLastError(NO_RECOVERY);
#endif
			return (-1);
		}
		sprintf(nbuf, "%s.%s", name, domain);
	}
	return (res_query(longname, class, type, answer, anslen));
}

const char *
hostalias(name)
	register const char *name;
{
	register char *cp1, *cp2;
	FILE *fp;
	char *file;
	char buf[BUFSIZ];
	static char abuf[MAXDNAME];

	if (_res.options & RES_NOALIASES)
		return (NULL);
	file = getenv("HOSTALIASES");
	if (file == NULL || (fp = fopen(file, "r")) == NULL)
		return (NULL);
	setbuf(fp, NULL);
	buf[sizeof(buf) - 1] = '\0';
	while (fgets(buf, sizeof(buf), fp)) {
		for (cp1 = buf; *cp1 && !isspace(*cp1); ++cp1)
			;
		if (!*cp1)
			break;
		*cp1 = '\0';
		if (!strcasecmp(buf, name)) {
			while (isspace(*++cp1))
				;
			if (!*cp1)
				break;
			for (cp2 = cp1 + 1; *cp2 && !isspace(*cp2); ++cp2)
				;
			abuf[sizeof(abuf) - 1] = *cp2 = '\0';
			strncpy(abuf, cp1, sizeof(abuf) - 1);
			fclose(fp);
			return (abuf);
		}
	}
	fclose(fp);
	return (NULL);
}
