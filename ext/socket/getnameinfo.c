/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Issues to be discussed:
 * - Thread safe-ness must be checked
 * - Return values.  There seems to be no standard for return value (RFC2133)
 *   but INRIA implementation returns EAI_xxx defined for getaddrinfo().
 */

#include "ruby/config.h"
#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif
#include <stdio.h>
#include <sys/types.h>
#ifndef _WIN32
#if defined(__BEOS__) && !defined(__HAIKU__) && !defined(BONE)
# include <net/socket.h>
#else
# include <sys/socket.h>
#endif
#include <netinet/in.h>
#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif
#if defined(HAVE_ARPA_NAMESER_H)
#include <arpa/nameser.h>
#endif
#include <netdb.h>
#if defined(HAVE_RESOLV_H)
#include <resolv.h>
#endif
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf _snprintf
#endif

#include <string.h>
#include <stddef.h>

#ifdef SOCKS5
#include <socks.h>
#endif

#include "addrinfo.h"
#include "sockport.h"

#define SUCCESS 0
#define ANY 0
#define YES 1
#define NO  0

struct sockinet {
	u_char	si_len;
	u_char	si_family;
	u_short	si_port;
};

static struct afd {
	int a_af;
	int a_addrlen;
	int a_socklen;
	int a_off;
} afdl [] = {
#ifdef INET6
#define N_INET6 0
	{PF_INET6, sizeof(struct in6_addr),
	 sizeof(struct sockaddr_in6),
	 offsetof(struct sockaddr_in6, sin6_addr)},
#define N_INET  1
#else
#define N_INET  0
#endif
	{PF_INET, sizeof(struct in_addr),
	 sizeof(struct sockaddr_in),
	 offsetof(struct sockaddr_in, sin_addr)},
	{0, 0, 0, 0},
};

#define ENI_NOSOCKET 	0
#define ENI_NOSERVNAME	1
#define ENI_NOHOSTNAME	2
#define ENI_MEMORY	3
#define ENI_SYSTEM	4
#define ENI_FAMILY	5
#define ENI_SALEN	6

#ifndef HAVE_INET_NTOP
static const char *
inet_ntop(int af, const void *addr, char *numaddr, size_t numaddr_len)
{
#ifdef HAVE_INET_NTOA
	struct in_addr in;
	memcpy(&in.s_addr, addr, sizeof(in.s_addr));
	snprintf(numaddr, numaddr_len, "%s", inet_ntoa(in));
#else
	unsigned long x = ntohl(*(unsigned long*)addr);
	snprintf(numaddr, numaddr_len, "%d.%d.%d.%d",
		 (int) (x>>24) & 0xff, (int) (x>>16) & 0xff,
		 (int) (x>> 8) & 0xff, (int) (x>> 0) & 0xff);
#endif
	return numaddr;
}
#endif

int
getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags)
{
	struct afd *afd;
	struct hostent *hp;
	u_short port;
	int family, len, i;
	char *addr, *p;
	u_long v4a;
#ifdef INET6
	u_char pfx;
#endif
	int h_error;
	char numserv[512];
	char numaddr[512];

	if (sa == NULL)
		return ENI_NOSOCKET;

	len = SA_LEN(sa);
	if (len != salen) return ENI_SALEN;

	family = sa->sa_family;
	for (i = 0; afdl[i].a_af; i++)
		if (afdl[i].a_af == family) {
			afd = &afdl[i];
			goto found;
		}
	return ENI_FAMILY;

 found:
	if (len != afd->a_socklen) return ENI_SALEN;

	port = ((struct sockinet *)sa)->si_port; /* network byte order */
	addr = (char *)sa + afd->a_off;

	if (serv == NULL || servlen == 0) {
		/* what we should do? */
	} else if (flags & NI_NUMERICSERV) {
		snprintf(numserv, sizeof(numserv), "%d", ntohs(port));
		if (strlen(numserv) + 1 > servlen)
			return ENI_MEMORY;
		strcpy(serv, numserv);
	} else {
#if defined(HAVE_GETSERVBYPORT)
		struct servent *sp = getservbyport(port, (flags & NI_DGRAM) ? "udp" : "tcp");
		if (sp) {
			if (strlen(sp->s_name) + 1 > servlen)
				return ENI_MEMORY;
			strcpy(serv, sp->s_name);
		} else
			return ENI_NOSERVNAME;
#else
		return ENI_NOSERVNAME;
#endif
	}

	switch (sa->sa_family) {
	case AF_INET:
		v4a = ntohl(((struct sockaddr_in *)sa)->sin_addr.s_addr);
		if (IN_MULTICAST(v4a) || IN_EXPERIMENTAL(v4a))
			flags |= NI_NUMERICHOST;
		v4a >>= IN_CLASSA_NSHIFT;
		if (v4a == 0)
			flags |= NI_NUMERICHOST;
		break;
#ifdef INET6
	case AF_INET6:
#ifdef HAVE_ADDR8
		pfx = ((struct sockaddr_in6 *)sa)->sin6_addr.s6_addr8[0];
#else
		pfx = ((struct sockaddr_in6 *)sa)->sin6_addr.s6_addr[0];
#endif
		if (pfx == 0 || pfx == 0xfe || pfx == 0xff)
			flags |= NI_NUMERICHOST;
		break;
#endif
	}
	if (host == NULL || hostlen == 0) {
		/* what should we do? */
	} else if (flags & NI_NUMERICHOST) {
		if (inet_ntop(afd->a_af, addr, numaddr, sizeof(numaddr))
		    == NULL)
			return ENI_SYSTEM;
		if (strlen(numaddr) > hostlen)
			return ENI_MEMORY;
		strcpy(host, numaddr);
	} else {
#ifdef INET6
		hp = getipnodebyaddr(addr, afd->a_addrlen, afd->a_af, &h_error);
#else
		hp = gethostbyaddr(addr, afd->a_addrlen, afd->a_af);
		h_error = h_errno;
#endif

		if (hp) {
			if (flags & NI_NOFQDN) {
				p = strchr(hp->h_name, '.');
				if (p) *p = '\0';
			}
			if (strlen(hp->h_name) + 1 > hostlen) {
#ifdef INET6
				freehostent(hp);
#endif
				return ENI_MEMORY;
			}
			strcpy(host, hp->h_name);
#ifdef INET6
			freehostent(hp);
#endif
		} else {
			if (flags & NI_NAMEREQD)
				return ENI_NOHOSTNAME;
			if (inet_ntop(afd->a_af, addr, numaddr, sizeof(numaddr))
			    == NULL)
				return ENI_NOHOSTNAME;
			if (strlen(numaddr) > hostlen)
				return ENI_MEMORY;
			strcpy(host, numaddr);
		}
	}
	return SUCCESS;
}
