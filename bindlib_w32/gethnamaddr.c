/*
 * ++Copyright++ 1985, 1988, 1993
 * -
 * Copyright (c) 1985, 1988, 1993
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
static char sccsid[] = "@(#)gethostnamadr.c	8.1 (Berkeley) 6/4/93";
static char rcsid[] = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#ifndef WINNT
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <stdio.h>
#include <netdb.h>
#include <resolv.h>
#include <ctype.h>	 
#include <errno.h>
#ifndef WINNT
#include <syslog.h>
#endif

#ifndef LOG_AUTH
#define LOG_AUTH 0
#endif

#define MULTI_PTRS_ARE_ALIASES 1	/* XXX - experimental */

#if defined(BSD) && (BSD >= 199103) && defined(AF_INET6)
#include <stdlib.h>
#include <string.h>
#else
#include "conf/portability.h"
#endif

#if defined(USE_OPTIONS_H)
#include <conf/options.h>
#endif

#ifdef SPRINTF_CHAR
#define SPRINTF(x) strlen(sprintf/**/x)
#else
#define SPRINTF(x) ((size_t)sprintf x)
#endif

#ifdef WINNT
extern char *pathhosts;
extern int res_paths_initialized;
extern init_res_paths();
#endif

#define	MAXALIASES	35
#define	MAXADDRS	35

static const char AskedForGot[] =
			  "gethostby*.getanswer: asked for \"%s\", got \"%s\"";

static char *h_addr_ptrs[MAXADDRS + 1];

static struct hostent host;
static char *host_aliases[MAXALIASES];
static char hostbuf[8*1024];
static u_char host_addr[16];	/* IPv4 or IPv6 */
static FILE *hostf = NULL;
static int stayopen = 0;

int inet_pton();
static void map_v4v6_address __P((const char *src, char *dst));
static void map_v4v6_hostent __P((struct hostent *hp, char **bp, int *len));

#ifdef RESOLVSORT
static void addrsort __P((char **, int));
#endif

#if PACKETSZ > 1024
#define	MAXPACKET	PACKETSZ
#else
#define	MAXPACKET	1024
#endif

typedef union {
    HEADER hdr;
    u_char buf[MAXPACKET];
} querybuf;

typedef union {
    int32_t al;
    char ac;
} align;

#ifndef WINNT
extern int h_errno;
#endif

#ifdef DEBUG
static void
dprintf(msg, num)
	char *msg;
	int num;
{
	if (_res.options & RES_DEBUG) {
		int save = errno;

		printf(msg, num);
#ifndef WINNT
		errno = save;
#else
		WSASetLastError(save);
		SetLastError(save);
#endif
	}
}
#else
#define dprintf(msg, num) /*nada*/
#endif

#ifndef WINNT
#define BOUNDED_INCR(x) \
	do { \
		cp += x; \
		if (cp > eom) { \
			h_errno = NO_RECOVERY; \
			return (NULL); \
		} \
	} while (0)

#else

#define BOUNDED_INCR(x) \
	do { \
		cp += x; \
		if (cp > eom) { \
 		WSASetLastError(NO_RECOVERY); \
			return (NULL); \
		} \
	} while (0)

#endif

#ifndef WINNT	
#define BOUNDS_CHECK(ptr, count) \
	do { \
		if ((ptr) + (count) > eom) { \
			h_errno = NO_RECOVERY; \
			return (NULL); \
		} \
	} while (0)
#else
#define BOUNDS_CHECK(ptr, count) \
	do { \
		if ((ptr) + (count) > eom) { \
 		WSASetLastError(NO_RECOVERY); \
			return (NULL); \
		} \
	} while (0)
#endif
	
static struct hostent *
getanswer(answer, anslen, qname, qtype)
	const querybuf *answer;
	int anslen;
	const char *qname;
	int qtype;
{
	register const HEADER *hp;
	register const u_char *cp;
	register int n;
	const u_char *eom, *erdata;
	char *bp, **ap, **hap;
	int type, class, buflen, ancount, qdcount;
	int haveanswer, had_error;
	int toobig = 0;
	char tbuf[MAXDNAME];
	const char *tname;
	int (*name_ok) __P((const char *));

	tname = qname;
	host.h_name = NULL;
	eom = answer->buf + anslen;
	switch (qtype) {
	case T_A:
	case T_AAAA:
		name_ok = res_hnok;
		break;
	case T_PTR:
		name_ok = res_dnok;
		break;
	default:
		return (NULL);	/* XXX should be abort(); */
	}
	/*
	 * find first satisfactory answer
	 */
	hp = &answer->hdr;
	ancount = ntohs((u_short)hp->ancount);
	qdcount = ntohs((u_short)hp->qdcount);
	bp = hostbuf;
	buflen = sizeof hostbuf;
	cp = answer->buf;
	BOUNDED_INCR(HFIXEDSZ);
	if (qdcount != 1) {
#ifndef WINNT
  		h_errno = NO_RECOVERY;
#else
 		WSASetLastError(NO_RECOVERY);
#endif
		return (NULL);
	}
	n = dn_expand(answer->buf, eom, cp, bp, buflen);
	if ((n < 0) || !(*name_ok)(bp)) {
#ifndef WINNT
  		h_errno = NO_RECOVERY;
#else
 		WSASetLastError(NO_RECOVERY);
#endif
		return (NULL);
	}
	BOUNDED_INCR(n + QFIXEDSZ);
	if (qtype == T_A || qtype == T_AAAA) {
		/* res_send() has already verified that the query name is the
		 * same as the one we sent; this just gets the expanded name
		 * (i.e., with the succeeding search-domain tacked on).
		 */
		n = strlen(bp) + 1;		/* for the \0 */
		if (n >= MAXHOSTNAMELEN) {
#ifndef WINNT
		h_errno = NO_RECOVERY;
#else
		WSASetLastError(NO_RECOVERY);
#endif
			return (NULL);
		}
		host.h_name = bp;
		bp += n;
		buflen -= n;
		/* The qname can be abbreviated, but h_name is now absolute. */
		qname = host.h_name;
	}
	ap = host_aliases;
	*ap = NULL;
	host.h_aliases = host_aliases;
	hap = h_addr_ptrs;
	*hap = NULL;
	host.h_addr_list = h_addr_ptrs;
	haveanswer = 0;
	had_error = 0;
	while (ancount-- > 0 && cp < eom && !had_error) {
		n = dn_expand(answer->buf, eom, cp, bp, buflen);
		if ((n < 0) || !(*name_ok)(bp)) {
			had_error++;
			continue;
		}
		cp += n;			/* name */
		BOUNDS_CHECK(cp, 3 * INT16SZ + INT32SZ);
		type = _getshort(cp);
 		cp += INT16SZ;			/* type */
		class = _getshort(cp);
 		cp += INT16SZ + INT32SZ;	/* class, TTL */
		n = _getshort(cp);
		cp += INT16SZ;			/* len */
		BOUNDS_CHECK(cp, n);
		erdata = cp + n;
		if (class != C_IN) {
			/* XXX - debug? syslog? */
			cp += n;
			continue;		/* XXX - had_error++ ? */
		}
		if ((qtype == T_A || qtype == T_AAAA) && type == T_CNAME) {
			if (ap >= &host_aliases[MAXALIASES-1])
				continue;
			n = dn_expand(answer->buf, eom, cp, tbuf, sizeof tbuf);
			if ((n < 0) || !(*name_ok)(tbuf)) {
				had_error++;
				continue;
			}
			cp += n;
			if (cp != erdata) {
#ifndef WINNT
   		        h_errno = NO_RECOVERY;
#else
 		        WSASetLastError(NO_RECOVERY);
#endif
				return (NULL);
			}
			/* Store alias. */
			*ap++ = bp;
			n = strlen(bp) + 1;	/* for the \0 */
			if (n >= MAXHOSTNAMELEN) {
				had_error++;
				continue;
			}
			bp += n;
			buflen -= n;
			/* Get canonical name. */
			n = strlen(tbuf) + 1;	/* for the \0 */
			if (n > buflen || n >= MAXHOSTNAMELEN) {
				had_error++;
				continue;
			}
			strcpy(bp, tbuf);
			host.h_name = bp;
			bp += n;
			buflen -= n;
			continue;
		}
		if (qtype == T_PTR && type == T_CNAME) {
			n = dn_expand(answer->buf, eom, cp, tbuf, sizeof tbuf);
			if (n < 0 || !res_dnok(tbuf)) {
				had_error++;
				continue;
			}
			cp += n;
			if (cp != erdata) {
#ifndef WINNT
        		h_errno = NO_RECOVERY;
#else
        		WSASetLastError(NO_RECOVERY);
#endif
				return (NULL);
			}
			/* Get canonical name. */
			n = strlen(tbuf) + 1;	/* for the \0 */
			if (n > buflen || n >= MAXHOSTNAMELEN) {
				had_error++;
				continue;
			}
			strcpy(bp, tbuf);
			tname = bp;
			bp += n;
			buflen -= n;
			continue;
		}
		if (type != qtype) {
			syslog(LOG_NOTICE|LOG_AUTH,
	       "gethostby*.getanswer: asked for \"%s %s %s\", got type \"%s\"",
			       qname, p_class(C_IN), p_type(qtype),
			       p_type(type));
			cp += n;
			continue;		/* XXX - had_error++ ? */
		}
		switch (type) {
		case T_PTR:
			if (strcasecmp(tname, bp) != 0) {
				syslog(LOG_NOTICE|LOG_AUTH,
				       (char *) AskedForGot, qname, bp);
				cp += n;
				continue;	/* XXX - had_error++ ? */
			}
			n = dn_expand(answer->buf, eom, cp, bp, buflen);
			if ((n < 0) || !res_hnok(bp)) {
				had_error++;
				break;
			}
#if MULTI_PTRS_ARE_ALIASES
			cp += n;
			if (cp != erdata) {
#ifndef WINNT
        		h_errno = NO_RECOVERY;
#else
        		WSASetLastError(NO_RECOVERY);
#endif
				return (NULL);
			}
			if (!haveanswer)
				host.h_name = bp;
			else if (ap < &host_aliases[MAXALIASES-1])
				*ap++ = bp;
			else
				n = -1;
			if (n != -1) {
				n = strlen(bp) + 1;	/* for the \0 */
				if (n >= MAXHOSTNAMELEN) {
					had_error++;
					break;
				}
				bp += n;
				buflen -= n;
			}
			break;
#else
			host.h_name = bp;
			if (_res.options & RES_USE_INET6) {
				n = strlen(bp) + 1;	/* for the \0 */
				if (n >= MAXHOSTNAMELEN) {
					had_error++;
					break;
				}
				bp += n;
				buflen -= n;
				map_v4v6_hostent(&host, &bp, &buflen);
			}
#ifndef WINNT
   		       h_errno = NETDB_SUCCESS;
#else
 		       WSASetLastError(NETDB_SUCCESS);
#endif
			return (&host);
#endif
		case T_A:
		case T_AAAA:
			if (strcasecmp(host.h_name, bp) != 0) {
				syslog(LOG_NOTICE|LOG_AUTH,
				       (char *)AskedForGot, host.h_name, bp);
				cp += n;
				continue;	/* XXX - had_error++ ? */
			}
			if (n != host.h_length) {
				cp += n;
				continue;
			}
			if (!haveanswer) {
				register int nn;

				host.h_name = bp;
				nn = strlen(bp) + 1;	/* for the \0 */
				bp += nn;
				buflen -= nn;
			}

			bp += sizeof(align) - ((u_long)bp % sizeof(align));

			if (bp + n >= &hostbuf[sizeof hostbuf]) {
				dprintf("size (%d) too big\n", n);
				had_error++;
				continue;
			}
			if (hap >= &h_addr_ptrs[MAXADDRS-1]) {
				if (!toobig++)
					dprintf("Too many addresses (%d)\n",
						MAXADDRS);
				cp += n;
				continue;
			}
			bcopy(cp, *hap++ = bp, n);
			bp += n;
			buflen -= n;
			cp += n;
			if (cp != erdata) {
#ifndef WINNT
				h_errno = NO_RECOVERY;
#else
                		WSASetLastError(NO_RECOVERY);
#endif
				return (NULL);
			}
			break;
		default:
			abort();
		}
		if (!had_error)
			haveanswer++;
	}
	if (haveanswer) {
		*ap = NULL;
		*hap = NULL;
#if defined(RESOLVSORT)
		/*
		 * Note: we sort even if host can take only one address
		 * in its return structures - should give it the "best"
		 * address in that case, not some random one
		 */
		if (_res.nsort && haveanswer > 1 && qtype == T_A)
			addrsort(h_addr_ptrs, haveanswer);
#endif /*RESOLVSORT*/
		if (!host.h_name) {
			n = strlen(qname) + 1;	/* for the \0 */
			if (n > buflen || n >= MAXHOSTNAMELEN)
				goto no_recovery;
			strcpy(bp, qname);
			host.h_name = bp;
			bp += n;
			buflen -= n;
		}
		if (_res.options & RES_USE_INET6)
			map_v4v6_hostent(&host, &bp, &buflen);
#ifndef WINNT
    		       h_errno = NETDB_SUCCESS;
#else
 		       WSASetLastError(NETDB_SUCCESS);
#endif
		return (&host);
	}
 no_recovery:
#ifndef WINNT
		h_errno = NO_RECOVERY;
#else
		WSASetLastError(NO_RECOVERY);
#endif
	return (NULL);
}


struct hostent *
gethostbyname2(name, af)
	const char *name;
	int af;
{
	querybuf buf;
	register const char *cp;
	char *bp;
	int n, size, type, len;
	extern struct hostent *_gethtbyname2();

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (NULL);
	}

	switch (af) {
	case AF_INET:
		size = INADDRSZ;
		type = T_A;
		break;
	case AF_INET6:
		size = IN6ADDRSZ;
		type = T_AAAA;
		break;
	default:
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
		errno = EAFNOSUPPORT;
#else
		WSASetLastError(NETDB_INTERNAL);
		SetLastError(WSAEAFNOSUPPORT);
#endif
		return (NULL);
	}

	host.h_addrtype = af;
	host.h_length = size;

	/*
	 * if there aren't any dots, it could be a user-level alias.
	 * this is also done in res_query() since we are not the only
	 * function that looks up host names.
	 */
	if (!strchr(name, '.') && (cp = __hostalias(name)))
		name = cp;

	/*
	 * disallow names consisting only of digits/dots, unless
	 * they end in a dot.
	 */
	if (isdigit(name[0]))
		for (cp = name;; ++cp) {
			if (!*cp) {
				if (*--cp == '.')
					break;
				/*
				 * All-numeric, no dot at the end.
				 * Fake up a hostent as if we'd actually
				 * done a lookup.
				 */
				if (inet_pton(af, name, host_addr) <= 0) {
#ifndef WINNT
					h_errno = HOST_NOT_FOUND;
#else
                                        WSASetLastError(HOST_NOT_FOUND);
#endif
					return (NULL);
				}
				strncpy(hostbuf, name, MAXDNAME);
				hostbuf[MAXDNAME] = '\0';
				bp = hostbuf + MAXDNAME;
				len = sizeof hostbuf - MAXDNAME;
				host.h_name = hostbuf;
				host.h_aliases = host_aliases;
				host_aliases[0] = NULL;
				h_addr_ptrs[0] = (char *)host_addr;
				h_addr_ptrs[1] = NULL;
				host.h_addr_list = h_addr_ptrs;
				if (_res.options & RES_USE_INET6)
					map_v4v6_hostent(&host, &bp, &len);
#ifndef WINNT
   		                 h_errno = NETDB_SUCCESS;
#else
 	 	                 WSASetLastError(NETDB_SUCCESS);
#endif
				return (&host);
			}
			if (!isdigit(*cp) && *cp != '.') 
				break;
		}
	if ((isxdigit(name[0]) && strchr(name, ':') != NULL) ||
	    name[0] == ':')
		for (cp = name;; ++cp) {
			if (!*cp) {
				if (*--cp == '.')
					break;
				/*
				 * All-IPv6-legal, no dot at the end.
				 * Fake up a hostent as if we'd actually
				 * done a lookup.
				 */
				if (inet_pton(af, name, host_addr) <= 0) {
#ifndef WINNT
					h_errno = HOST_NOT_FOUND;
#else
                                        WSASetLastError(HOST_NOT_FOUND);
#endif
					return (NULL);
				}
				strncpy(hostbuf, name, MAXDNAME);
				hostbuf[MAXDNAME] = '\0';
				bp = hostbuf + MAXDNAME;
				len = sizeof hostbuf - MAXDNAME;
				host.h_name = hostbuf;
				host.h_aliases = host_aliases;
				host_aliases[0] = NULL;
				h_addr_ptrs[0] = (char *)host_addr;
				h_addr_ptrs[1] = NULL;
				host.h_addr_list = h_addr_ptrs;
#ifndef WINNT
   		                h_errno = NETDB_SUCCESS;
#else
 		                WSASetLastError(NETDB_SUCCESS);
#endif
				return (&host);
			}
			if (!isxdigit(*cp) && *cp != ':' && *cp != '.') 
				break;
		}

	if ((n = res_search(name, C_IN, type, buf.buf, sizeof(buf))) < 0) {
		dprintf("res_search failed (%d)\n", n);
		if (errno == ECONNREFUSED)
			return (_gethtbyname2(name, af));
		return (NULL);
	}
	return (getanswer(&buf, n, name, type));
}

struct hostent *
#ifndef WINNT
gethostbyname(name)
#else
gethostbyname_nt(name)
#endif
	const char *name;
{
	struct hostent *hp;

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (NULL);
	}
	if (_res.options & RES_USE_INET6) {
		hp = gethostbyname2(name, AF_INET6);
		if (hp)
			return (hp);
	}
	return (gethostbyname2(name, AF_INET));
}

struct hostent *
#ifndef WINNT
gethostbyaddr(addr, len, af)
#else
gethostbyaddr_nt(addr, len, af)
#endif
	const char *addr;	/* XXX should have been def'd as u_char! */
	int len, af;
{
	const u_char *uaddr = (const u_char *)addr;
	static const u_char mapped[] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0xff,0xff };
	static const u_char tunnelled[] = { 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 };
	int n, size;
	querybuf buf;
	register struct hostent *hp;
	char qbuf[MAXDNAME+1], *qp;
#ifdef SUNSECURITY
	register struct hostent *rhp;
	char **haddr;
	u_long old_options;
	char hname2[MAXDNAME+1];
#endif /*SUNSECURITY*/
extern struct hostent *_gethtbyaddr();
	
	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (0);
	}
	if (af == AF_INET6 && len == IN6ADDRSZ &&
	    (!bcmp(uaddr, mapped, sizeof mapped) ||
	     !bcmp(uaddr, tunnelled, sizeof tunnelled))) {
		/* Unmap. */
		addr += sizeof mapped;
		uaddr += sizeof mapped;
		af = AF_INET;
		len = INADDRSZ;
	}
	switch (af) {
	case AF_INET:
		size = INADDRSZ;
		break;
	case AF_INET6:
		size = IN6ADDRSZ;
		break;
	default:
#ifndef WINNT
		errno = EAFNOSUPPORT;
		h_errno = NETDB_INTERNAL;
#else
 		SetLastError(WSAEAFNOSUPPORT);
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (0);
	}
	if (size != len) {
#ifndef WINNT
		errno = EINVAL;
		h_errno = NETDB_INTERNAL;
#else
 		SetLastError(WSAEINVAL);
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (0);
	}
	switch (af) {
	case AF_INET:
		(void) sprintf(qbuf, "%u.%u.%u.%u.in-addr.arpa",
			       (uaddr[3] & 0xff),
			       (uaddr[2] & 0xff),
			       (uaddr[1] & 0xff),
			       (uaddr[0] & 0xff));
		break;
	case AF_INET6:
		qp = qbuf;
		for (n = IN6ADDRSZ - 1; n >= 0; n--) {
			qp += SPRINTF((qp, "%x.%x.",
				       uaddr[n] & 0xf,
				       (uaddr[n] >> 4) & 0xf));
		}
		strcpy(qp, "ip6.int");
		break;
	default:
		abort();
	}
	n = res_query(qbuf, C_IN, T_PTR, (u_char *)buf.buf, sizeof buf.buf);
	if (n < 0) {
		dprintf("res_query failed (%d)\n", n);
		if (errno == ECONNREFUSED)
			return (_gethtbyaddr(addr, len, af));
		return (0);
	}
	if (!(hp = getanswer(&buf, n, qbuf, T_PTR)))
		return (0);	/* h_errno was set by getanswer() */
#ifdef SUNSECURITY
	if (af == AF_INET) {
	    /*
	     * turn off search as the name should be absolute,
	     * 'localhost' should be matched by defnames
	     */
	    strncpy(hname2, hp->h_name, MAXDNAME);
	    hname2[MAXDNAME] = '\0';
	    old_options = _res.options;
	    _res.options &= ~RES_DNSRCH;
	    _res.options |= RES_DEFNAMES;
	    if (!(rhp = gethostbyname_nt(hname2))) {
		syslog(LOG_NOTICE|LOG_AUTH,
		       "gethostbyaddr: No A record for %s (verifying [%s])",
		       hname2, inet_ntoa(*((struct in_addr *)addr)));
		_res.options = old_options;
#ifndef WINNT
		    h_errno = HOST_NOT_FOUND;
#else
                    WSASetLastError(HOST_NOT_FOUND);
#endif
		return (NULL);
	    }
	    _res.options = old_options;
	    for (haddr = rhp->h_addr_list; *haddr; haddr++)
		if (!memcmp(*haddr, addr, INADDRSZ))
			break;
	    if (!*haddr) {
		syslog(LOG_NOTICE|LOG_AUTH,
		       "gethostbyaddr: A record of %s != PTR record [%s]",
		       hname2, inet_ntoa(*((struct in_addr *)addr)));
#ifndef WINNT
		    h_errno = HOST_NOT_FOUND;
#else
                    WSASetLastError(HOST_NOT_FOUND);
#endif
		return (NULL);
	    }
	}
#endif /*SUNSECURITY*/
	hp->h_addrtype = af;
	hp->h_length = len;
	bcopy(addr, host_addr, len);
	h_addr_ptrs[0] = (char *)host_addr;
	h_addr_ptrs[1] = NULL;
	if (af == AF_INET && (_res.options & RES_USE_INET6)) {
		map_v4v6_address((char*)host_addr, (char*)host_addr);
		hp->h_addrtype = AF_INET6;
		hp->h_length = IN6ADDRSZ;
	}
#ifndef WINNT
   		       h_errno = NETDB_SUCCESS;
#else
 		       WSASetLastError(NETDB_SUCCESS);
#endif
	return (hp);
}

void
_sethtent(f)
	int f;
{
    
#ifndef WINNT
	if (!hostf)
		hostf = fopen(_PATH_HOSTS, "r" );
#else
        if (!res_paths_initialized)
          init_res_paths();

	if (!hostf)
		hostf = fopen(pathhosts, "r" );
#endif

	else
		rewind(hostf);
	stayopen = f;
}

void
_endhtent()
{
	if (hostf && !stayopen) {
		(void) fclose(hostf);
		hostf = NULL;
	}
}

struct hostent *
_gethtent()
{
	char *p;
	register char *cp, **q;
	int af, len;

#ifndef WINNT
 	if (!hostf && !(hostf = fopen(_PATH_HOSTS, "r" ))) {
#else
        if (!res_paths_initialized)
           init_res_paths();
 
 	if (!hostf && !(hostf = fopen(pathhosts, "r" ))) {
#endif
 
#ifndef WINNT
		h_errno = NETDB_INTERNAL;
#else
		WSASetLastError(NETDB_INTERNAL);
#endif
		return (NULL);
	}
 again:
	if (!(p = fgets(hostbuf, sizeof hostbuf, hostf))) {
#ifndef WINNT
		    h_errno = HOST_NOT_FOUND;
#else
                    WSASetLastError(HOST_NOT_FOUND);
#endif
		return (NULL);
	}
	if (*p == '#')
		goto again;
	if (!(cp = strpbrk(p, "#\n")))
		goto again;
	*cp = '\0';
	if (!(cp = strpbrk(p, " \t")))
		goto again;
	*cp++ = '\0';
	if (inet_pton(AF_INET6, p, host_addr) > 0) {
		af = AF_INET6;
		len = IN6ADDRSZ;
	} else if (inet_pton(AF_INET, p, host_addr) > 0) {
		if (_res.options & RES_USE_INET6) {
			map_v4v6_address((char*)host_addr, (char*)host_addr);
			af = AF_INET6;
			len = IN6ADDRSZ;
		} else {
			af = AF_INET;
			len = INADDRSZ;
		}
	} else {
		goto again;
	}
	h_addr_ptrs[0] = (char *)host_addr;
	h_addr_ptrs[1] = NULL;
	host.h_addr_list = h_addr_ptrs;
	host.h_length = len;
	host.h_addrtype = af;
	while (*cp == ' ' || *cp == '\t')
		cp++;
	host.h_name = cp;
	q = host.h_aliases = host_aliases;
	if (cp = strpbrk(cp, " \t"))
		*cp++ = '\0';
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &host_aliases[MAXALIASES - 1])
			*q++ = cp;
		if (cp = strpbrk(cp, " \t"))
			*cp++ = '\0';
	}
	*q = NULL;
#ifndef WINNT
   		       h_errno = NETDB_SUCCESS;
#else
		       WSASetLastError(NETDB_SUCCESS);
#endif
	return (&host);
}

struct hostent *
_gethtbyname(name)
	const char *name;
{
	extern struct hostent *_gethtbyname2();
	struct hostent *hp;

	if (_res.options & RES_USE_INET6) {
		hp = _gethtbyname2(name, AF_INET6);
		if (hp)
			return (hp);
	}
	return (_gethtbyname2(name, AF_INET));
}

struct hostent *
_gethtbyname2(name, af)
	const char *name;
	int af;
{
	register struct hostent *p;
	register char **cp;
	
	_sethtent(0);
	while (p = _gethtent()) {
		if (p->h_addrtype != af)
			continue;
		if (strcasecmp(p->h_name, name) == 0)
			break;
		for (cp = p->h_aliases; *cp != 0; cp++)
			if (strcasecmp(*cp, name) == 0)
				goto found;
	}
 found:
	_endhtent();
	return (p);
}

struct hostent *
_gethtbyaddr(addr, len, af)
	const char *addr;
	int len, af;
{
	register struct hostent *p;

	_sethtent(0);
	while (p = _gethtent())
		if (p->h_addrtype == af && !bcmp(p->h_addr, addr, len))
			break;
	_endhtent();
	return (p);
}

static void
map_v4v6_address(src, dst)
	const char *src;
	char *dst;
{
	u_char *p = (u_char *)dst;
	char tmp[INADDRSZ];
	int i;

	/* Stash a temporary copy so our caller can update in place. */
	bcopy(src, tmp, INADDRSZ);
	/* Mark this ipv6 addr as a mapped ipv4. */
	for (i = 0; i < 10; i++)
		*p++ = 0x00;
	*p++ = 0xff;
	*p++ = 0xff;
	/* Retrieve the saved copy and we're done. */
	bcopy(tmp, (void*)p, INADDRSZ);
}

static void
map_v4v6_hostent(hp, bpp, lenp)
	struct hostent *hp;
	char **bpp;
	int *lenp;
{
	char **ap;

	if (hp->h_addrtype != AF_INET || hp->h_length != INADDRSZ)
		return;
	hp->h_addrtype = AF_INET6;
	hp->h_length = IN6ADDRSZ;
	for (ap = hp->h_addr_list; *ap; ap++) {
		int i = sizeof(align) - ((u_long)*bpp % sizeof(align));

		if (*lenp < (i + IN6ADDRSZ)) {
			/* Out of memory.  Truncate address list here.  XXX */
			*ap = NULL;
			return;
		}
		*bpp += i;
		*lenp -= i;
		map_v4v6_address(*ap, *bpp);
		*ap = *bpp;
		*bpp += IN6ADDRSZ;
		*lenp -= IN6ADDRSZ;
	}
}

#ifdef RESOLVSORT
static void
addrsort(ap, num)
	char **ap;
	int num;
{
	int i, j;
	char **p;
	short aval[MAXADDRS];
	int needsort = 0;

	p = ap;
	for (i = 0; i < num; i++, p++) {
	    for (j = 0 ; (unsigned)j < _res.nsort; j++)
		if (_res.sort_list[j].addr.s_addr == 
		    (((struct in_addr *)(*p))->s_addr & _res.sort_list[j].mask))
			break;
	    aval[i] = j;
	    if (needsort == 0 && i > 0 && j < aval[i-1])
		needsort = i;
	}
	if (!needsort)
	    return;

	while (needsort < num) {
	    for (j = needsort - 1; j >= 0; j--) {
		if (aval[j] > aval[j+1]) {
		    char *hp;

		    i = aval[j];
		    aval[j] = aval[j+1];
		    aval[j+1] = i;

		    hp = ap[j];
		    ap[j] = ap[j+1];
		    ap[j+1] = hp;

		} else
		    break;
	    }
	    needsort++;
	}
}
#endif

#if defined(BSD43_BSD43_NFS) || defined(sun)
/* some libc's out there are bound internally to these names (UMIPS) */
void
ht_sethostent(stayopen)
	int stayopen;
{
	_sethtent(stayopen);
}

void
ht_endhostent()
{
	_endhtent();
}

struct hostent *
ht_gethostbyname(name)
	char *name;
{
	return (_gethtbyname(name));
}

struct hostent *
ht_gethostbyaddr(addr, len, af)
	const char *addr;
	int len, af;
{
	return (_gethtbyaddr(addr, len, af));
}

struct hostent *
gethostent()
{
	return (_gethtent());
}

void
dns_service()
{
	return;
}

#undef dn_skipname
dn_skipname(comp_dn, eom)
	const u_char *comp_dn, *eom;
{
	return (__dn_skipname(comp_dn, eom));
}
#endif /*old-style libc with yp junk in it*/
