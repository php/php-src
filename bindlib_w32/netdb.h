/*
 * ++Copyright++ 1980, 1983, 1988, 1993
 * -
 * Copyright (c) 1980, 1983, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
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

/*
 *      @(#)netdb.h	8.1 (Berkeley) 6/2/93
 *	$Id$
 */

#ifndef _NETDB_H_
#define _NETDB_H_

#ifndef WINNT
#include <sys/param.h>
#endif /* WINNT */
#if (!defined(BSD)) || (BSD < 199306)
#include <sys/bitypes.h>
#endif
#include <sys/cdefs.h>

#ifndef WINNT
#define	_PATH_HEQUIV	"/etc/hosts.equiv"
#define	_PATH_HOSTS	"/etc/hosts"
#define	_PATH_NETWORKS	"/etc/networks"
#define	_PATH_PROTOCOLS	"/etc/protocols"
#define	_PATH_SERVICES	"/etc/services"

extern int h_errno;
#else /* WINNT */

#ifndef WIN95
#define	_PATH_HOSTS	"%windir%\\SYSTEM32\\DRIVERS\\ETC\\HOSTS"
#define	_PATH_NETWORKS	"%windir%\\SYSTEM32\\DRIVERS\\ETC\\NETWORKS"
#define	_PATH_PROTOCOL	"%windir%\\SYSTEM32\\DRIVERS\\ETC\\PROTOCOL"
#define	_PATH_SERVICES	"%windir%\\SYSTEM32\\DRIVERS\\ETC\\SERVICES"
#else
#define	_PATH_HOSTS	"%windir%\\HOSTS"
#define	_PATH_NETWORKS	"%windir%\\NETWORKS"
#define	_PATH_PROTOCOL	"%windir%\\PROTOCOL"
#define	_PATH_SERVICES	"%windir%\\SERVICES"
#endif /* WIN95 */
#endif /* WINNT */

#ifndef WINNT
/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses from name server */
#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in an unsigned long -- probably a poor one.
 */
struct	netent {
	char		*n_name;	/* official name of net */
	char		**n_aliases;	/* alias list */
	int		n_addrtype;	/* net address type */
	unsigned long	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};
#endif /* WINNT */

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define	NETDB_INTERNAL	-1	/* see errno */
#define	NETDB_SUCCESS	0	/* no problem */
#ifndef WINNT
#define	HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	TRY_AGAIN	2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA		4 /* Valid name, no data record of requested type */
#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */
#endif /* WINNT */

__BEGIN_DECLS
void		endhostent __P((void));
void		endnetent __P((void));
void		endprotoent __P((void));
void		endservent __P((void));
#ifndef WINNT
struct hostent	*gethostbyaddr __P((const char *, int, int));
struct hostent	*gethostbyname __P((const char *));
struct hostent	*gethostbyname2 __P((const char *, int));
struct hostent	*gethostent __P((void));
struct netent	*getnetbyaddr __P((unsigned long, int)); /* u_long? */
struct netent	*getnetbyname __P((const char *));
#else
struct hostent	*gethostbyaddr_nt __P((const char *, int, int));
struct hostent	*gethostbyname_nt __P((const char *));
struct hostent	*gethostbyname2 __P((const char *, int));
struct hostent	*gethostent __P((void));
struct netent	*getnetbyaddr __P((unsigned long, int)); /* u_long? */
struct netent	*getnetbyname __P((const char *));
#endif

struct netent	*getnetent __P((void));
#ifndef WINNT
struct protoent	*getprotobyname __P((const char *));
struct protoent	*getprotobynumber __P((int));
#endif
struct protoent	*getprotoent __P((void));
#ifndef WINNT
struct servent	*getservbyname __P((const char *, const char *));
struct servent	*getservbyport __P((int, const char *));
#endif
struct servent	*getservent __P((void));
void		herror __P((const char *));
const char	*hstrerror __P((int));
void		sethostent __P((int));
/* void		sethostfile __P((const char *)); */
void		setnetent __P((int));
void		setprotoent __P((int));
void		setservent __P((int));
__END_DECLS

/* This is nec'y to make this include file properly replace the sun version. */
#ifdef sun
#ifdef __GNU_LIBRARY__
#include <rpc/netdb.h>
#else
struct rpcent {
	char	*r_name;	/* name of server for this rpc program */
	char	**r_aliases;	/* alias list */
	int	r_number;	/* rpc program number */
};
struct rpcent	*getrpcbyname(), *getrpcbynumber(), *getrpcent();
#endif /* __GNU_LIBRARY__ */
#endif /* sun */

/*
 * The Motorola kernel will only support 64 characters for hostname
 * also defined in /usr/ucbinclude/netdb.h
 */
#ifdef __m88k__ 
#define MAXHOSTNAMELEN 64
#endif
#ifdef WINNT 
#define MAXHOSTNAMELEN 64
#endif

#endif /* !_NETDB_H_ */
