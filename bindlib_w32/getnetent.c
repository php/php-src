/*
 * Copyright (c) 1983 Regents of the University of California.
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

/* Portions Copyright (c) 1993 Carlos Leandro and Rui Salgueiro
 *	Dep. Matematica Universidade de Coimbra, Portugal, Europe
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * from getnetent.c	1.1 (Coimbra) 93/06/02
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getnetent.c	8.1 (Berkeley) 6/4/93";
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
#include <resolv.h>
#include <netdb.h>
#include <string.h>
#ifdef WINNT
#include "conf/portability.h"
extern char *pathnetworks;
extern int res_paths_initialized;
extern init_res_paths();
#endif

#ifndef _PATH_NETWORKS 
#define _PATH_NETWORKS  "/etc/networks"
#endif



#define	MAXALIASES	35

static FILE *netf;
static char line[BUFSIZ+1];
static struct netent net;
static char *net_aliases[MAXALIASES];
int _net_stayopen;

void _setnetent __P((int));
void _endnetent __P((void));

void
setnetent(stayopen)
	int stayopen;
{

	sethostent(stayopen);
	_setnetent(stayopen);
}

void
endnetent()
{

	endhostent();
	_endnetent();
}

void
_setnetent(f)
	int f;
{

#ifndef WINNT
	if (netf == NULL)
		netf = fopen(_PATH_NETWORKS, "r" );
#else
       if (!res_paths_initialized)
         init_res_paths();

	if (netf == NULL)
		netf = fopen(pathnetworks, "r" );
#endif
	else
		rewind(netf);
	_net_stayopen |= f;
}

void
_endnetent()
{

	if (netf) {
		fclose(netf);
		netf = NULL;
	}
	_net_stayopen = 0;
}

struct netent *
getnetent()
{
	char *p;
	register char *cp, **q;

#ifndef WINNT
	if (netf == NULL && (netf = fopen(_PATH_NETWORKS, "r" )) == NULL)
#else
       if (!res_paths_initialized)
         init_res_paths();

	if (netf == NULL && (netf = fopen(pathnetworks, "r" )) == NULL)
#endif

		return (NULL);
again:
	p = fgets(line, sizeof line, netf);
	if (p == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = strpbrk(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	net.n_name = p;
	cp = strpbrk(p, " \t");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	while (*cp == ' ' || *cp == '\t')
		cp++;
	p = strpbrk(cp, " \t");
	if (p != NULL)
		*p++ = '\0';
	net.n_net = inet_network(cp);
	net.n_addrtype = AF_INET;
	q = net.n_aliases = net_aliases;
	if (p != NULL) {
		cp = p;
		while (cp && *cp) {
			if (*cp == ' ' || *cp == '\t') {
				cp++;
				continue;
			}
			if (q < &net_aliases[MAXALIASES - 1])
				*q++ = cp;
			cp = strpbrk(cp, " \t");
			if (cp != NULL)
				*cp++ = '\0';
		}
	}
	*q = NULL;
	return (&net);
}
