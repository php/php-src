/*
 * ++Copyright++ 1985, 1993
 * -
 * Copyright (c) 1985, 1993
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
static char sccsid[] = "@(#)res_comp.c	8.1 (Berkeley) 6/4/93";
static char rcsid[] = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#ifndef WINNT
#include <sys/param.h>
#include <netinet/in.h>
#endif
#include <arpa/nameser.h>

#include <ctype.h>
#include <errno.h>
#include <resolv.h>
#include <stdio.h>

#if defined(BSD) && (BSD >= 199103)
# include <unistd.h>
# include <string.h>
#else
# include "conf/portability.h"
#endif

static int	ns_name_ntop __P((const u_char *, char *, size_t));
static int	ns_name_pton __P((const char *, u_char *, size_t));
static int	ns_name_unpack __P((const u_char *, const u_char *,
				    const u_char *, u_char *, size_t));
static int	ns_name_pack __P((const u_char *, u_char *, int,
				  const u_char **, const u_char **));
static int	ns_name_uncompress __P((const u_char *, const u_char *,
					const u_char *, char *, size_t));
static int	ns_name_compress __P((const char *, u_char *, size_t,
				      const u_char **, const u_char **));
static int	ns_name_skip __P((const u_char **, const u_char *));

/*
 * Expand compressed domain name 'comp_dn' to full domain name.
 * 'msg' is a pointer to the begining of the message,
 * 'eomorig' points to the first location after the message,
 * 'exp_dn' is a pointer to a buffer of size 'length' for the result.
 * Return size of compressed name or -1 if there was an error.
 */
int
dn_expand(msg, eom, src, dst, dstsiz)
	const u_char *msg;
	const u_char *eom;
	const u_char *src;
	char *dst;
	int dstsiz;
{
	int n = ns_name_uncompress(msg, eom, src, dst, (size_t)dstsiz);

	if (n > 0 && dst[0] == '.')
		dst[0] = '\0';
	return (n);
}

/*
 * Pack domain name 'exp_dn' in presentation form into 'comp_dn'.
 * Return the size of the compressed name or -1.
 * 'length' is the size of the array pointed to by 'comp_dn'.
 */
int
dn_comp(src, dst, dstsiz, dnptrs, lastdnptr)
	const char *src;
	u_char *dst;
	int dstsiz;
	u_char **dnptrs;
	u_char **lastdnptr;
{
	return (ns_name_compress(src, dst, (size_t)dstsiz,
				 (const u_char **)dnptrs,
				 (const u_char **)lastdnptr));
}

/*
 * Skip over a compressed domain name. Return the size or -1.
 */
int
__dn_skipname(ptr, eom)
	const u_char *ptr;
	const u_char *eom;
{
	const u_char *saveptr = ptr;

	if (ns_name_skip(&ptr, eom) == -1)
		return (-1);
	return (ptr - saveptr);
}

/*
 * Verify that a domain name uses an acceptable character set.
 */

/*
 * Note the conspicuous absence of ctype macros in these definitions.  On
 * non-ASCII hosts, we can't depend on string literals or ctype macros to
 * tell us anything about network-format data.  The rest of the BIND system
 * is not careful about this, but for some reason, we're doing it right here.
 */
#define PERIOD 0x2e
#define	hyphenchar(c) ((c) == 0x2d)
#define bslashchar(c) ((c) == 0x5c)
#define periodchar(c) ((c) == PERIOD)
#define asterchar(c) ((c) == 0x2a)
#define alphachar(c) (((c) >= 0x41 && (c) <= 0x5a) \
		   || ((c) >= 0x61 && (c) <= 0x7a))
#define digitchar(c) ((c) >= 0x30 && (c) <= 0x39)

#define borderchar(c) (alphachar(c) || digitchar(c))
#define middlechar(c) (borderchar(c) || hyphenchar(c))
#define	domainchar(c) ((c) > 0x20 && (c) < 0x7f)

int
res_hnok(dn)
	const char *dn;
{
	int ppch = '\0', pch = PERIOD, ch = *dn++;

	while (ch != '\0') {
		int nch = *dn++;

		if (periodchar(ch)) {
			NULL;
		} else if (periodchar(pch)) {
			if (!borderchar(ch))
				return (0);
		} else if (periodchar(nch) || nch == '\0') {
			if (!borderchar(ch))
				return (0);
		} else {
			if (!middlechar(ch))
				return (0);
		}
		ppch = pch, pch = ch, ch = nch;
	}
	return (1);
}

/*
 * hostname-like (A, MX, WKS) owners can have "*" as their first label
 * but must otherwise be as a host name.
 */
int
res_ownok(dn)
	const char *dn;
{
	if (asterchar(dn[0])) {
		if (periodchar(dn[1]))
			return (res_hnok(dn+2));
		if (dn[1] == '\0')
			return (1);
	}
	return (res_hnok(dn));
}

/*
 * SOA RNAMEs and RP RNAMEs can have any printable character in their first
 * label, but the rest of the name has to look like a host name.
 */
int
res_mailok(dn)
	const char *dn;
{
	int ch, escaped = 0;

	/* "." is a valid missing representation */
	if (*dn == '\0')
		return(1);

	/* otherwise <label>.<hostname> */
	while ((ch = *dn++) != '\0') {
		if (!domainchar(ch))
			return (0);
		if (!escaped && periodchar(ch))
			break;
		if (escaped)
			escaped = 0;
		else if (bslashchar(ch))
			escaped = 1;
	}
	if (periodchar(ch))
		return (res_hnok(dn));
	return(0);
}

/*
 * This function is quite liberal, since RFC 1034's character sets are only
 * recommendations.
 */
int
res_dnok(dn)
	const char *dn;
{
	int ch;

	while ((ch = *dn++) != '\0')
		if (!domainchar(ch))
			return (0);
	return (1);
}

/*
 * Routines to insert/extract short/long's.
 */

u_int16_t
_getshort(msgp)
	register const u_char *msgp;
{
	register u_int16_t u;

	GETSHORT(u, msgp);
	return (u);
}

#ifdef NeXT
/*
 * nExt machines have some funky library conventions, which we must maintain.
 */
u_int16_t
res_getshort(msgp)
	register const u_char *msgp;
{
	return (_getshort(msgp));
}
#endif

u_int32_t
_getlong(msgp)
	register const u_char *msgp;
{
	register u_int32_t u;

	GETLONG(u, msgp);
	return (u);
}

void
#if defined(__STDC__) || defined(__cplusplus)
__putshort(register u_int16_t s, register u_char *msgp)	/* must match proto */
#else
__putshort(s, msgp)
	register u_int16_t s;
	register u_char *msgp;
#endif
{
	PUTSHORT(s, msgp);
}

void
__putlong(l, msgp)
	register u_int32_t l;
	register u_char *msgp;
{
	PUTLONG(l, msgp);
}

/* ++ From BIND 8.1.1. ++ */
/*
 * Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*"Id: ns_name.c,v 1.1 1997/12/13 02:41:13 vixie Exp vixie"*/

/*#include "port_before.h"*/

/*#include <sys/types.h>*/

/*#include <netinet/in.h>*/
/*#include <arpa/nameser.h>*/

/*#include <errno.h>*/
/*#include <resolv.h>*/
/*#include <string.h>*/

/*#include "port_after.h"*/

#define NS_CMPRSFLGS	0xc0	/* Flag bits indicating name compression. */
#define NS_MAXCDNAME	255	/* maximum compressed domain name */

/* Data. */

static char		digits[] = "0123456789";

/* Forward. */

static int		special(int);
static int		printable(int);
static int		dn_find(const u_char *, const u_char *,
				const u_char * const *,
				const u_char * const *);

/* Public. */

/*
 * ns_name_ntop(src, dst, dstsiz)
 *	Convert an encoded domain name to printable ascii as per RFC1035.
 * return:
 *	Number of bytes written to buffer, or -1 (with errno set)
 * notes:
 *	The root is returned as "."
 *	All other domains are returned in non absolute form
 */
static int
ns_name_ntop(src, dst, dstsiz)
	const u_char *src;
	char *dst;
	size_t dstsiz;
{
	const u_char *cp;
	char *dn, *eom;
	u_char c;
	u_int n;

	cp = src;
	dn = dst;
	eom = dst + dstsiz;

	while ((n = *cp++) != 0) {
		if ((n & NS_CMPRSFLGS) != 0) {
			/* Some kind of compression pointer. */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
     return (-1);

		}
		if (dn != dst) {
			if (dn >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
     return (-1);
			}
			*dn++ = '.';
		}
		if (dn + n >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		for ((void)NULL; n > 0; n--) {
			c = *cp++;
			if (special(c)) {
				if (dn + 1 >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				*dn++ = '\\';
				*dn++ = (char)c;
			} else if (!printable(c)) {
				if (dn + 3 >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				*dn++ = '\\';
				*dn++ = digits[c / 100];
				*dn++ = digits[(c % 100) / 10];
				*dn++ = digits[c % 10];
			} else {
				if (dn >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				*dn++ = (char)c;
			}
		}
	}
	if (dn == dst) {
		if (dn >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		*dn++ = '.';
	}
	if (dn >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	*dn++ = '\0';
	return (dn - dst);
}

/*
 * ns_name_pton(src, dst, dstsiz)
 *	Convert a ascii string into an encoded domain name as per RFC1035.
 * return:
 *	-1 if it fails
 *	1 if string was fully qualified
 *	0 is string was not fully qualified
 * notes:
 *	Enforces label and domain length limits.
 */

static int
ns_name_pton(src, dst, dstsiz)
	const char *src;
	u_char *dst;
	size_t dstsiz;
{
	u_char *label, *bp, *eom;
	int c, n, escaped;
	char *cp;

	escaped = 0;
	bp = dst;
	eom = dst + dstsiz;
	label = bp++;

	while ((c = *src++) != 0) {
		if (escaped) {
			if ((cp = strchr(digits, c)) != NULL) {
				n = (cp - digits) * 100;
				if ((c = *src++) == 0 ||
				    (cp = strchr(digits, c)) == NULL) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				n += (cp - digits) * 10;
				if ((c = *src++) == 0 ||
				    (cp = strchr(digits, c)) == NULL) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				n += (cp - digits);
				if (n > 255) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				c = n;
			}
			escaped = 0;
		} else if (c == '\\') {
			escaped = 1;
			continue;
		} else if (c == '.') {
			c = (bp - label - 1);
			if ((c & NS_CMPRSFLGS) != 0) {	/* Label too big. */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			if (label >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			*label = c;
			/* Fully qualified ? */
			if (*src == '\0') {
				if (c != 0) {
					if (bp >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
						return (-1);
					}
					*bp++ = '\0';
				}
				if ((bp - dst) > MAXCDNAME) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				return (1);
			}
			if (c == 0) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			label = bp++;
			continue;
		}
		if (bp >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		*bp++ = (u_char)c;
	}
	c = (bp - label - 1);
	if ((c & NS_CMPRSFLGS) != 0) {		/* Label too big. */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	if (label >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	*label = c;
	if (c != 0) {
		if (bp >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		*bp++ = 0;
	}
	if ((bp - dst) > MAXCDNAME) {	/* src too big */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	return (0);
}

/*
 * ns_name_unpack(msg, eom, src, dst, dstsiz)
 *	Unpack a domain name from a message, source may be compressed.
 * return:
 *	-1 if it fails, or consumed octets if it succeeds.
 */
static int
ns_name_unpack(msg, eom, src, dst, dstsiz)
	const u_char *msg;
	const u_char *eom;
	const u_char *src;
	u_char *dst;
	size_t dstsiz;
{
	const u_char *srcp, *dstlim;
	u_char *dstp;
	int n, len, checked;

	len = -1;
	checked = 0;
	dstp = dst;
	srcp = src;
	dstlim = dst + dstsiz;
	if (srcp < msg || srcp >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	/* Fetch next label in domain name. */
	while ((n = *srcp++) != 0) {
		/* Check for indirection. */
		switch (n & NS_CMPRSFLGS) {
		case 0:
			/* Limit checks. */
			if (dstp + n + 1 >= dstlim || srcp + n >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			checked += n + 1;
			*dstp++ = n;
			memcpy(dstp, srcp, n);
			dstp += n;
			srcp += n;
			break;

		case NS_CMPRSFLGS:
			if (srcp >= eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			if (len < 0)
				len = srcp - src + 1;
			srcp = msg + (((n & 0x3f) << 8) | (*srcp & 0xff));
			if (srcp < msg || srcp >= eom) {  /* Out of range. */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			checked += 2;
			/*
			 * Check for loops in the compressed name;
			 * if we've looked at the whole message,
			 * there must be a loop.
			 */
			if (checked >= eom - msg) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
			break;

		default:
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);			/* flag error */
		}
	}
	*dstp = '\0';
	if (len < 0)
		len = srcp - src;
	return (len);
}

/*
 * ns_name_pack(src, dst, dstsiz, dnptrs, lastdnptr)
 *	Pack domain name 'domain' into 'comp_dn'.
 * return:
 *	Size of the compressed name, or -1.
 * notes:
 *	'dnptrs' is an array of pointers to previous compressed names.
 *	dnptrs[0] is a pointer to the beginning of the message. The array
 *	ends with NULL.
 *	'lastdnptr' is a pointer to the end of the array pointed to
 *	by 'dnptrs'.
 * Side effects:
 *	The list of pointers in dnptrs is updated for labels inserted into
 *	the message as we compress the name.  If 'dnptr' is NULL, we don't
 *	try to compress names. If 'lastdnptr' is NULL, we don't update the
 *	list.
 */
static int
ns_name_pack(src, dst, dstsiz, dnptrs, lastdnptr)
	const u_char *src;
	u_char *dst;
	int dstsiz;
	const u_char **dnptrs;
	const u_char **lastdnptr;
{
	u_char *dstp;
	const u_char **cpp, **lpp, *eob, *msg;
	const u_char *srcp;
	int n, l;

	srcp = src;
	dstp = dst;
	eob = dstp + dstsiz;
	lpp = cpp = NULL;
	if (dnptrs != NULL) {
		if ((msg = *dnptrs++) != NULL) {
			for (cpp = dnptrs; *cpp != NULL; cpp++)
				(void)NULL;
			lpp = cpp;	/* end of list to search */
		}
	} else
		msg = NULL;

	/* make sure the domain we are about to add is legal */
	l = 0;
	do {
		n = *srcp;
		if ((n & NS_CMPRSFLGS) != 0) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		l += n + 1;
		if (l > MAXCDNAME) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		srcp += n + 1;
	} while (n != 0);

	srcp = src;
	do {
		/* Look to see if we can use pointers. */
		n = *srcp;
		if (n != 0 && msg != NULL) {
			l = dn_find(srcp, msg, (const u_char * const *)dnptrs,
				    (const u_char * const *)lpp);
			if (l >= 0) {
				if (dstp + 1 >= eob) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
					return (-1);
				}
				*dstp++ = (l >> 8) | NS_CMPRSFLGS;
				*dstp++ = l % 256;
				return (dstp - dst);
			}
			/* Not found, save it. */
			if (lastdnptr != NULL && cpp < lastdnptr - 1 &&
			    (dstp - msg) < 0x4000) {
				*cpp++ = dstp;
				*cpp = NULL;
			}
		}
		/* copy label to buffer */
		if (n & NS_CMPRSFLGS) {		/* Should not happen. */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		if (dstp + 1 + n >= eob) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		memcpy(dstp, srcp, n + 1);
		srcp += n + 1;
		dstp += n + 1;
	} while (n != 0);

	if (dstp > eob) {
		if (msg != NULL)
			*lpp = NULL;
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	} 
	return (dstp - dst);
}

/*
 * ns_name_uncompress(msg, eom, src, dst, dstsiz)
 *	Expand compressed domain name to presentation format.
 * return:
 *	Number of bytes read out of `src', or -1 (with errno set).
 * note:
 *	Root domain returns as "." not "".
 */
static int
ns_name_uncompress(msg, eom, src, dst, dstsiz)
	const u_char *msg;
	const u_char *eom;
	const u_char *src;
	char *dst;
	size_t dstsiz;
{
	u_char tmp[NS_MAXCDNAME];
	int n;
	
	if ((n = ns_name_unpack(msg, eom, src, tmp, sizeof tmp)) == -1)
		return (-1);
	if (ns_name_ntop(tmp, dst, dstsiz) == -1)
		return (-1);
	return (n);
}

/*
 * ns_name_compress(src, dst, dstsiz, dnptrs, lastdnptr)
 *	Compress a domain name into wire format, using compression pointers.
 * return:
 *	Number of bytes consumed in `dst' or -1 (with errno set).
 * notes:
 *	'dnptrs' is an array of pointers to previous compressed names.
 *	dnptrs[0] is a pointer to the beginning of the message.
 *	The list ends with NULL.  'lastdnptr' is a pointer to the end of the
 *	array pointed to by 'dnptrs'. Side effect is to update the list of
 *	pointers for labels inserted into the message as we compress the name.
 *	If 'dnptr' is NULL, we don't try to compress names. If 'lastdnptr'
 *	is NULL, we don't update the list.
 */
static int
ns_name_compress(src, dst, dstsiz, dnptrs, lastdnptr)
	const char *src;
	u_char *dst;
	size_t dstsiz;
	const u_char **dnptrs;
	const u_char **lastdnptr;
{
	u_char tmp[NS_MAXCDNAME];

	if (ns_name_pton(src, tmp, sizeof tmp) == -1)
		return (-1);
	return (ns_name_pack(tmp, dst, dstsiz, dnptrs, lastdnptr));
}

/*
 * ns_name_skip(ptrptr, eom)
 *	Advance *ptrptr to skip over the compressed name it points at.
 * return:
 *	0 on success, -1 (with errno set) on failure.
 */
static int
ns_name_skip(ptrptr, eom)
	const u_char **ptrptr;
	const u_char *eom;
{
	const u_char *cp;
	u_int n;

	cp = *ptrptr;
	while (cp < eom && (n = *cp++) != 0) {
		/* Check for indirection. */
		switch (n & NS_CMPRSFLGS) {
		case 0:			/* normal case, n == len */
			cp += n;
			continue;
		case NS_CMPRSFLGS:	/* indirection */
			cp++;
			break;
		default:		/* illegal type */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
			return (-1);
		}
		break;
	}
	if (cp > eom) {
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
		return (-1);
	}
	*ptrptr = cp;
	return (0);
}

/* Private. */

/*
 * special(ch)
 *	Thinking in noninternationalized USASCII (per the DNS spec),
 *	is this characted special ("in need of quoting") ?
 * return:
 *	boolean.
 */
static int
special(ch)
	int ch;
{
	switch (ch) {
	case 0x22: /* '"' */
	case 0x2E: /* '.' */
	case 0x3B: /* ';' */
	case 0x5C: /* '\\' */
	/* Special modifiers in zone files. */
	case 0x40: /* '@' */
	case 0x24: /* '$' */
		return (1);
	default:
		return (0);
	}
}

/*
 * printable(ch)
 *	Thinking in noninternationalized USASCII (per the DNS spec),
 *	is this character visible and not a space when printed ?
 * return:
 *	boolean.
 */
static int
printable(ch)
	int ch;
{
	return (ch > 0x20 && ch < 0x7f);
}

/*
 *	Thinking in noninternationalized USASCII (per the DNS spec),
 *	convert this character to lower case if it's upper case.
 */
static int
mklower(ch)
	int ch;
{
	if (ch >= 0x41 && ch <= 0x5A)
		return (ch + 0x20);
	return (ch);
}

/*
 * dn_find(domain, msg, dnptrs, lastdnptr)
 *	Search for the counted-label name in an array of compressed names.
 * return:
 *	offset from msg if found, or -1.
 * notes:
 *	dnptrs is the pointer to the first name on the list,
 *	not the pointer to the start of the message.
 */
static int
dn_find(domain, msg, dnptrs, lastdnptr)
	const u_char *domain;
	const u_char *msg;
	const u_char * const *dnptrs;
	const u_char * const *lastdnptr;
{
	const u_char *dn, *cp, *sp;
	const u_char * const *cpp;
	u_int n;

	for (cpp = dnptrs; cpp < lastdnptr; cpp++) {
		dn = domain;
		sp = cp = *cpp;
		while ((n = *cp++) != 0) {
			/*
			 * check for indirection
			 */
			switch (n & NS_CMPRSFLGS) {
			case 0:			/* normal case, n == len */
				if (n != *dn++)
					goto next;
				for ((void)NULL; n > 0; n--)
					if (mklower(*dn++) != mklower(*cp++))
						goto next;
				/* Is next root for both ? */
				if (*dn == '\0' && *cp == '\0')
					return (sp - msg);
				if (*dn)
					continue;
				goto next;

			case NS_CMPRSFLGS:	/* indirection */
				cp = msg + (((n & 0x3f) << 8) | *cp);
				break;

			default:	/* illegal type */
#ifndef WINNT
     errno = EMSGSIZE;
#else
     WSASetLastError(WSAEMSGSIZE);
     SetLastError(WSAEMSGSIZE);
#endif
				return (-1);
			}
		}
 next: ;
	}
#ifndef WINNT
    errno = ENOENT;
#else
     SetLastError(ENOENT);
#endif
	return (-1);
}

/* -- From BIND 8.1.1. -- */
