/*
 * ++Copyright++ 1985, 1989, 1993
 * -
 * Copyright (c) 1985, 1989, 1993
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
static char sccsid[] = "@(#)res_send.c	8.1 (Berkeley) 6/4/93";
static char rcsid[] = "$Id$";
#endif /* LIBC_SCCS and not lint */

	/* change this to "0"
	 * if you talk to a lot
	 * of multi-homed SunOS
	 * ("broken") name servers.
	 */
#define	CHECK_SRVR_ADDR	1	/* XXX - should be in options.h */

/*
 * Send query to name server and wait for reply.
 */

#include <sys/types.h>
#ifndef WINNT
#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#endif
#include <arpa/nameser.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <resolv.h>
#if defined(BSD) && (BSD >= 199306)
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
#else
# include "conf/portability.h"
#endif

#if defined(USE_OPTIONS_H)
# include <conf/options.h>
#endif

#ifndef WINNT
static int s = -1;	/* socket used for communications */
#else
static SOCKET s = INVALID_SOCKET;
typedef char * caddr_t;
#endif
static int connected = 0;	/* is the socket connected */
static int vc = 0;	/* is the socket a virtual ciruit? */

#ifndef FD_SET
/* XXX - should be in portability.h */
#define	NFDBITS		32
#define	FD_SETSIZE	32
#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

/* XXX - this should be done in portability.h */
#if (defined(BSD) && (BSD >= 199103)) || defined(linux)
# define CAN_RECONNECT 1
#else
# define CAN_RECONNECT 0
#endif

#ifndef DEBUG
#   define Dprint(cond, args) /*empty*/
#   define DprintQ(cond, args, query, size) /*empty*/
#   define Aerror(file, string, error, address) /*empty*/
#   define Perror(file, string, error) /*empty*/
#else
#   define Dprint(cond, args) if (cond) {fprintf args;} else {}
#   define DprintQ(cond, args, query, size) if (cond) {\
			fprintf args;\
			__fp_nquery(query, size, stdout);\
		} else {}
    static void
    Aerror(file, string, error, address)
	FILE *file;
	char *string;
	int error;
	struct sockaddr_in address;
    {
	int save = errno;

	if (_res.options & RES_DEBUG) {
		fprintf(file, "res_send: %s ([%s].%u): %s\n",
			string,
			inet_ntoa(address.sin_addr),
			ntohs(address.sin_port),
			strerror(error));
	}
#ifndef WINNT
	errno = save;
#else
	WSASetLastError(save);
	SetLastError(save);
#endif
    }
    static void
    Perror(file, string, error)
	FILE *file;
	char *string;
	int error;
    {
	int save = errno;

	if (_res.options & RES_DEBUG) {
		fprintf(file, "res_send: %s: %s\n",
			string, strerror(error));
	}
#ifndef WINNT
	errno = save;
#else
 	SetLastError(save);
#endif
    }
#endif

static res_send_qhook Qhook = NULL;
static res_send_rhook Rhook = NULL;

void
res_send_setqhook(hook)
	res_send_qhook hook;
{

	Qhook = hook;
}

void
res_send_setrhook(hook)
	res_send_rhook hook;
{

	Rhook = hook;
}

/* int
 * res_isourserver(ina)
 *	looks up "ina" in _res.ns_addr_list[]
 * returns:
 *	0  : not found
 *	>0 : found
 * author:
 *	paul vixie, 29may94
 */
int
res_isourserver(inp)
	const struct sockaddr_in *inp;
{
	struct sockaddr_in ina;
	register int ns, ret;

	ina = *inp;
	ret = 0;
	for (ns = 0;  ns < _res.nscount;  ns++) {
		register const struct sockaddr_in *srv = &_res.nsaddr_list[ns];

		if (srv->sin_family == ina.sin_family &&
		    srv->sin_port == ina.sin_port &&
		    (srv->sin_addr.s_addr == INADDR_ANY ||
		     srv->sin_addr.s_addr == ina.sin_addr.s_addr)) {
			ret++;
			break;
		}
	}
	return (ret);
}

/* int
 * res_nameinquery(name, type, class, buf, eom)
 *	look for (name,type,class) in the query section of packet (buf,eom)
 * requires:
 *	buf + HFIXESDZ <= eom
 * returns:
 *	-1 : format error
 *	0  : not found
 *	>0 : found
 * author:
 *	paul vixie, 29may94
 */
int
res_nameinquery(name, type, class, buf, eom)
	const char *name;
	register int type, class;
	const u_char *buf, *eom;
{
	register const u_char *cp = buf + HFIXEDSZ;
	int qdcount = ntohs( (u_short) ((HEADER*)buf)->qdcount);

	while (qdcount-- > 0) {
		char tname[MAXDNAME+1];
		register int n, ttype, tclass;

		n = dn_expand(buf, eom, cp, tname, sizeof tname);
		if (n < 0)
			return (-1);
		cp += n;
		if (cp + 2 * INT16SZ > eom)
			return (-1);
		ttype = _getshort(cp); cp += INT16SZ;
		tclass = _getshort(cp); cp += INT16SZ;
		if (ttype == type &&
		    tclass == class &&
		    strcasecmp(tname, name) == 0)
			return (1);
	}
	return (0);
}

/* int
 * res_queriesmatch(buf1, eom1, buf2, eom2)
 *	is there a 1:1 mapping of (name,type,class)
 *	in (buf1,eom1) and (buf2,eom2)?
 * returns:
 *	-1 : format error
 *	0  : not a 1:1 mapping
 *	>0 : is a 1:1 mapping
 * author:
 *	paul vixie, 29may94
 */
int
res_queriesmatch(buf1, eom1, buf2, eom2)
	const u_char *buf1, *eom1;
	const u_char *buf2, *eom2;
{
	register const u_char *cp = buf1 + HFIXEDSZ;
	int qdcount = ntohs( (u_short) ((HEADER*)buf1)->qdcount);

	if (buf1 + HFIXEDSZ > eom1 || buf2 + HFIXEDSZ > eom2)
		return (-1);

	if (qdcount != ntohs( (u_short) ((HEADER*)buf2)->qdcount))
		return (0);
	while (qdcount-- > 0) {
		char tname[MAXDNAME+1];
		register int n, ttype, tclass;

		n = dn_expand(buf1, eom1, cp, tname, sizeof tname);
		if (n < 0)
			return (-1);
		cp += n;
		if (cp + 2 * INT16SZ > eom1)
			return (-1);
		ttype = _getshort(cp);	cp += INT16SZ;
		tclass = _getshort(cp); cp += INT16SZ;
		if (!res_nameinquery(tname, ttype, tclass, buf2, eom2))
			return (0);
	}
	return (1);
}

int
res_send(buf, buflen, ans, anssiz)
	const u_char *buf;
	int buflen;
	u_char *ans;
	int anssiz;
{
	HEADER *hp = (HEADER *) buf;
	HEADER *anhp = (HEADER *) ans;
	int gotsomewhere, connreset, terrno, try, v_circuit, resplen, ns;
	register int n;
	u_int badns;	/* XXX NSMAX can't exceed #/bits in this var */
#ifdef WINNT
	int optionValue = SO_SYNCHRONOUS_NONALERT;
	DWORD BytesRead;
#endif

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
		/* errno should have been set by res_init() in this case. */
		return (-1);
	}
	if (anssiz < HFIXEDSZ) {
#ifndef WINNT
        errno = EINVAL;
#else
	SetLastError(EINVAL);
#endif
		return (-1);
	}
	DprintQ((_res.options & RES_DEBUG) || (_res.pfcode & RES_PRF_QUERY),
		(stdout, ";; res_send()\n"), buf, buflen);
	v_circuit = (_res.options & RES_USEVC) || buflen > PACKETSZ;
	gotsomewhere = 0;
	connreset = 0;
	terrno = ETIMEDOUT;
	badns = 0;

	/*
	 * Send request, RETRY times, or until successful
	 */
	for (try = 0; try < _res.retry; try++) {
	    for (ns = 0; ns < _res.nscount; ns++) {
		struct sockaddr_in *nsap = &_res.nsaddr_list[ns];
    same_ns:
		if (badns & (1 << ns)) {
			res_close();
			goto next_ns;
		}

		if (Qhook) {
			int done = 0, loops = 0;

			do {
				res_sendhookact act;

				act = (*Qhook)(&nsap, &buf, &buflen,
					       ans, anssiz, &resplen);
				switch (act) {
				case res_goahead:
					done = 1;
					break;
				case res_nextns:
					res_close();
					goto next_ns;
				case res_done:
					return (resplen);
				case res_modified:
					/* give the hook another try */
					if (++loops < 42) /*doug adams*/
						break;
					/*FALLTHROUGH*/
				case res_error:
					/*FALLTHROUGH*/
				default:
					return (-1);
				}
			} while (!done);
		}

		Dprint(_res.options & RES_DEBUG,
		       (stdout, ";; Querying server (# %d) address = %s\n",
			ns + 1, inet_ntoa(nsap->sin_addr)));

		if (v_circuit) {
			int truncated;
			struct iovec iov[2];
			u_short len;
			u_char *cp;
#ifdef WINNT
			/* set up things so that subsequent socket()
			   calls will open blocking sockets */
			if (setsockopt(INVALID_SOCKET, SOL_SOCKET,
				  	SO_OPENTYPE, (char *)&optionValue,
					sizeof(optionValue)) != NO_ERROR) {
						Perror(stderr, "cannot open nonoverlapped socket", errno);
						return (-1);
			}
#endif
			/*
			 * Use virtual circuit;
			 * at most one attempt per server.
			 */
			try = _res.retry;
			truncated = 0;
			if ((s < 0) || (!vc)) {
				if (s >= 0)
					res_close();

				s = socket(PF_INET, SOCK_STREAM, 0);
#ifndef WINNT
				if (s < 0) {
#else
				if (s == INVALID_SOCKET) {
#endif
					terrno = errno;
					Perror(stderr, "socket(vc)", errno);
					return (-1);
				}
#ifndef WINNT
				errno = 0;
#else
				SetLastError(0);
#endif
				if (connect(s, (struct sockaddr *)nsap,
					    sizeof(struct sockaddr)) < 0) {
					terrno = errno;
					Aerror(stderr, "connect/vc",
					       errno, *nsap);
					badns |= (1 << ns);
					res_close();
					goto next_ns;
				}
				vc = 1;
			}
			/*
			 * Send length & message
			 */
			putshort((u_short)buflen, (u_char*)&len);
			iov[0].iov_base = (caddr_t)&len;
			iov[0].iov_len = INT16SZ;
			iov[1].iov_base = (caddr_t)buf;
			iov[1].iov_len = buflen;
			if (writev(s, iov, 2) != (INT16SZ + buflen)) {
				terrno = errno;
				Perror(stderr, "write failed", errno);
				badns |= (1 << ns);
				res_close();
				goto next_ns;
			}
			/*
			 * Receive length & response
			 */
read_len:
			cp = ans;
			len = INT16SZ;
#ifndef WINNT
			while ((n = read(s, (char *)cp, (int)len)) > 0) {
#else
			while (ReadFile((HANDLE)s, (char *)cp, (DWORD)len,
				             &BytesRead, NULL) &&
				   (n = (int)BytesRead) > 0) {
#endif
				cp += n;
				if ((len -= n) <= 0)
					break;
			}
			if (n <= 0) {
				terrno = errno;
				Perror(stderr, "read failed", errno);
				res_close();
				/*
				 * A long running process might get its TCP
				 * connection reset if the remote server was
				 * restarted.  Requery the server instead of
				 * trying a new one.  When there is only one
				 * server, this means that a query might work
				 * instead of failing.  We only allow one reset
				 * per query to prevent looping.
				 */
				if (terrno == ECONNRESET && !connreset) {
					connreset = 1;
					res_close();
					goto same_ns;
				}
				res_close();
				goto next_ns;
			}
			resplen = _getshort(ans);
			if (resplen > anssiz) {
				Dprint(_res.options & RES_DEBUG,
				       (stdout, ";; response truncated\n")
				       );
				truncated = 1;
				len = anssiz;
			} else
				len = resplen;
			if (len < HFIXEDSZ) {
				/*
				 * Undersized message.
				 */
				Dprint(_res.options & RES_DEBUG,
				       (stdout, ";; undersized: %d\n", len));
#ifndef WINNT
				terrno = EMSGSIZE;
#else
				terrno = WSAEMSGSIZE;
#endif
				badns |= (1 << ns);
				res_close();
				goto next_ns;
			}
			cp = ans;
			while (len != 0 &&
#ifndef WINNT
			       (n = read(s, (char *)cp, (int)len)) > 0) {
#else
					ReadFile((HANDLE)s, (char *)cp, (DWORD)len,
				             &BytesRead, NULL) &&
				    (n = (int)BytesRead) > 0) {
#endif
				cp += n;
				len -= n;
			}
			if (n <= 0) {
				terrno = errno;
				Perror(stderr, "read(vc)", errno);
				res_close();
				goto next_ns;
			}
			if (truncated) {
				/*
				 * Flush rest of answer
				 * so connection stays in synch.
				 */
				anhp->tc = 1;
				len = resplen - anssiz;
				while (len != 0) {
					char junk[PACKETSZ];

					n = (len > sizeof(junk)
					     ? sizeof(junk)
					     : len);
#ifndef WINNT
					if ((n = read(s, junk, n)) > 0)
#else
					if (ReadFile((HANDLE)s, (char *)junk, (DWORD)n,
				             &BytesRead, NULL) &&
				        (n = (int)BytesRead) > 0)
#endif
						len -= n;
					else
						break;
				}
			}
			/*
			 * The calling applicating has bailed out of
			 * a previous call and failed to arrange to have
			 * the circuit closed or the server has got
			 * itself confused. Anyway drop the packet and
			 * wait for the correct one.
			 */
			if (hp->id != anhp->id) {
				DprintQ((_res.options & RES_DEBUG) ||
					(_res.pfcode & RES_PRF_REPLY),
					(stdout, ";; old answer (unexpected):\n"),
					ans, (resplen>anssiz)?anssiz:resplen);
				goto read_len;
			}
		} else {
			/*
			 * Use datagrams.
			 */
			struct timeval timeout;
			fd_set dsmask;
			struct sockaddr_in from;
			int fromlen;

#ifndef WINNT
  			if ((s < 0) || vc) {
#else
 			if ((s == INVALID_SOCKET) || vc) {
#endif
				if (vc)
					res_close();
				s = socket(PF_INET, SOCK_DGRAM, 0);
#ifndef WINNT
				if (s < 0) {
#else
 				if (s == INVALID_SOCKET) {
#endif
#if !CAN_RECONNECT
 bad_dg_sock:
#endif
					terrno = errno;
					Perror(stderr, "socket(dg)", errno);
					return (-1);
				}
				connected = 0;
			}
			/*
			 * On a 4.3BSD+ machine (client and server,
			 * actually), sending to a nameserver datagram
			 * port with no nameserver will cause an
			 * ICMP port unreachable message to be returned.
			 * If our datagram socket is "connected" to the
			 * server, we get an ECONNREFUSED error on the next
			 * socket operation, and select returns if the
			 * error message is received.  We can thus detect
			 * the absence of a nameserver without timing out.
			 * If we have sent queries to at least two servers,
			 * however, we don't want to remain connected,
			 * as we wish to receive answers from the first
			 * server to respond.
			 */
#ifndef WINNT
			if (_res.nscount == 1 || (try == 0 && ns == 0)) {
				/*
				 * Connect only if we are sure we won't
				 * receive a response from another server.
				 */
				if (!connected) {
					if (connect(s, (struct sockaddr *)nsap,
						    sizeof(struct sockaddr)
						    ) < 0) {
						Aerror(stderr,
						       "connect(dg)",
						       errno, *nsap);
						badns |= (1 << ns);
						res_close();
						goto next_ns;
					}
					connected = 1;
				}
				if (send(s, (char*)buf, buflen, 0) != buflen) {
					Perror(stderr, "send", errno);
					badns |= (1 << ns);
					res_close();
					goto next_ns;
				}
			} else {
#endif /* WINNT */
				/*
				 * Disconnect if we want to listen
				 * for responses from more than one server.
				 */
				if (connected) {
#if CAN_RECONNECT
					struct sockaddr_in no_addr;

					no_addr.sin_family = AF_INET;
					no_addr.sin_addr.s_addr = INADDR_ANY;
					no_addr.sin_port = 0;
					(void) connect(s,
						       (struct sockaddr *)
						        &no_addr,
						       sizeof(no_addr));
#else
					int s1 = socket(PF_INET, SOCK_DGRAM,0);
#ifndef WINNT
					if (s1 < 0)
#else
					if (s1 == INVALID_SOCKET)
#endif
						goto bad_dg_sock;
					(void) dup2(s1, s);
					(void) close(s1);
					Dprint(_res.options & RES_DEBUG,
					       (stdout, ";; new DG socket\n"))
#endif
					connected = 0;
#ifndef WINNT
					errno = 0;
#else
					SetLastError(0);
#endif
				}
				if (sendto(s, (char*)buf, buflen, 0,
					   (struct sockaddr *)nsap,
					   sizeof(struct sockaddr))
				    != buflen) {
					Aerror(stderr, "sendto", errno, *nsap);
					badns |= (1 << ns);
					res_close();
					goto next_ns;
				}
#ifndef WINNT
			}
#endif

			/*
			 * Wait for reply
			 */
			timeout.tv_sec = (_res.retrans << try);
			if (try > 0)
				timeout.tv_sec /= _res.nscount;
			if ((long) timeout.tv_sec <= 0)
				timeout.tv_sec = 1;
			timeout.tv_usec = 0;
    wait:
			if (s < 0 || s >= FD_SETSIZE) {
				Perror(stderr, "s out-of-bounds", EMFILE);
				res_close();
				goto next_ns;
			}
			FD_ZERO(&dsmask);
			FD_SET(s, &dsmask);
			n = select(s+1, &dsmask, (fd_set *)NULL,
				   (fd_set *)NULL, &timeout);
			if (n < 0) {
#ifndef WINNT
				if (errno == EINTR)
#else
				if (errno == WSAEINTR)
#endif
					goto wait;
				Perror(stderr, "select", errno);
				res_close();
				goto next_ns;
			}
			if (n == 0) {
				/*
				 * timeout
				 */
				Dprint(_res.options & RES_DEBUG,
				       (stdout, ";; timeout\n"));
				gotsomewhere = 1;
				res_close();
				goto next_ns;
			}
#ifndef WINNT
			errno = 0;
#else
 			WSASetLastError(0);
 			SetLastError(0);
#endif
			fromlen = sizeof(struct sockaddr_in);
			resplen = recvfrom(s, (char*)ans, anssiz, 0,
					   (struct sockaddr *)&from, &fromlen);
			if (resplen <= 0) {
#ifndef WINNT
				Perror(stderr, "recvfrom", errno);
#else
 				Perror(stderr, "recvfrom", WSAGetLastError());
#endif
				res_close();
				goto next_ns;
			}
			gotsomewhere = 1;
			if (resplen < HFIXEDSZ) {
				/*
				 * Undersized message.
				 */
				Dprint(_res.options & RES_DEBUG,
				       (stdout, ";; undersized: %d\n",
					resplen));
#ifndef WINNT
				terrno = EMSGSIZE;
#else
				terrno = WSAEMSGSIZE;
#endif
				badns |= (1 << ns);
				res_close();
				goto next_ns;
			}
			if (hp->id != anhp->id) {
				/*
				 * response from old query, ignore it.
				 * XXX - potential security hazard could
				 *	 be detected here.
				 */
				DprintQ((_res.options & RES_DEBUG) ||
					(_res.pfcode & RES_PRF_REPLY),
					(stdout, ";; old answer:\n"),
					ans, (resplen>anssiz)?anssiz:resplen);
				goto wait;
			}
#if CHECK_SRVR_ADDR
			if (!(_res.options & RES_INSECURE1) &&
			    !res_isourserver(&from)) {
				/*
				 * response from wrong server? ignore it.
				 * XXX - potential security hazard could
				 *	 be detected here.
				 */
				DprintQ((_res.options & RES_DEBUG) ||
					(_res.pfcode & RES_PRF_REPLY),
					(stdout, ";; not our server:\n"),
					ans, (resplen>anssiz)?anssiz:resplen);
				goto wait;
			}
#endif
			if (!(_res.options & RES_INSECURE2) &&
			    !res_queriesmatch(buf, buf + buflen,
					      ans, ans + anssiz)) {
				/*
				 * response contains wrong query? ignore it.
				 * XXX - potential security hazard could
				 *	 be detected here.
				 */
				DprintQ((_res.options & RES_DEBUG) ||
					(_res.pfcode & RES_PRF_REPLY),
					(stdout, ";; wrong query name:\n"),
					ans, (resplen>anssiz)?anssiz:resplen);
				goto wait;
			}
			if (anhp->rcode == SERVFAIL ||
			    anhp->rcode == NOTIMP ||
			    anhp->rcode == REFUSED) {
				DprintQ(_res.options & RES_DEBUG,
					(stdout, "server rejected query:\n"),
					ans, (resplen>anssiz)?anssiz:resplen);
				badns |= (1 << ns);
				res_close();
				/* don't retry if called from dig */
				if (!_res.pfcode)
					goto next_ns;
			}
			if (!(_res.options & RES_IGNTC) && anhp->tc) {
				/*
				 * get rest of answer;
				 * use TCP with same server.
				 */
				Dprint(_res.options & RES_DEBUG,
				       (stdout, ";; truncated answer\n"));
				v_circuit = 1;
				res_close();
				goto same_ns;
			}
		} /*if vc/dg*/
		Dprint((_res.options & RES_DEBUG) ||
		       ((_res.pfcode & RES_PRF_REPLY) &&
			(_res.pfcode & RES_PRF_HEAD1)),
		       (stdout, ";; got answer:\n"));
		DprintQ((_res.options & RES_DEBUG) ||
			(_res.pfcode & RES_PRF_REPLY),
			(stdout, ""),
			ans, (resplen>anssiz)?anssiz:resplen);
		/*
		 * If using virtual circuits, we assume that the first server
		 * is preferred over the rest (i.e. it is on the local
		 * machine) and only keep that one open.
		 * If we have temporarily opened a virtual circuit,
		 * or if we haven't been asked to keep a socket open,
		 * close the socket.
		 */
		if ((v_circuit && (!(_res.options & RES_USEVC) || ns != 0)) ||
		    !(_res.options & RES_STAYOPEN)) {
			res_close();
		}
		if (Rhook) {
			int done = 0, loops = 0;

			do {
				res_sendhookact act;

				act = (*Rhook)(nsap, buf, buflen,
					       ans, anssiz, &resplen);
				switch (act) {
				case res_goahead:
				case res_done:
					done = 1;
					break;
				case res_nextns:
					res_close();
					goto next_ns;
				case res_modified:
					/* give the hook another try */
					if (++loops < 42) /*doug adams*/
						break;
					/*FALLTHROUGH*/
				case res_error:
					/*FALLTHROUGH*/
				default:
					return (-1);
				}
			} while (!done);

		}
		return (resplen);
    next_ns: ;
	   } /*foreach ns*/
	} /*foreach retry*/
	res_close();
	if (!v_circuit)
		if (!gotsomewhere) {
#ifndef WINNT
			errno = ECONNREFUSED;	/* no nameservers found */
#else
 			WSASetLastError(ECONNREFUSED);
  			SetLastError(ECONNREFUSED);
#endif
		} else {
#ifndef WINNT
			errno = ETIMEDOUT;	/* no answer obtained */
#else
 			WSASetLastError(ETIMEDOUT);
 			SetLastError(ETIMEDOUT);
#endif
		} else {
#ifndef WINNT
		errno = terrno;
#else
 		WSASetLastError(terrno);
 		SetLastError(terrno);
#endif
	return (-1);
 }
}

/*
 * This routine is for closing the socket if a virtual circuit is used and
 * the program wants to close it.  This provides support for endhostent()
 * which expects to close the socket.
 *
 * This routine is not expected to be user visible.
 */
void
res_close()
{
	if (s >= 0) {
#ifndef WINNT
		(void) close(s);
#else
    (void) closesocket(s);
#endif

#ifndef WINNT
		s = -1;
#else
		s = INVALID_SOCKET;
#endif
		connected = 0;
		vc = 0;
	}
}

#ifdef ultrix
/* ultrix 4.0 had some icky packaging in its libc.a.  alias for it here.
 * there is more gunk of this kind over in res_debug.c.
 */

void
_res_close()
{
	res_close();
}

#undef res_send
int
res_send(buf, buflen, ans, anssiz)
	const u_char *buf;
	int buflen;
	u_char *ans;
	int anssiz;
{
	return (__res_send(buf, buflen, ans, anssiz));
}
#endif /* Ultrix 4.0 hackery */
