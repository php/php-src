/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if WIN32|WINNT
#if HAVE_BINDLIB
#ifndef WINNT
#define WINNT 1
#endif
/* located in www.php.net/extra/bindlib.zip */
#include "arpa/inet.h"
#include "netdb.h"
#include "arpa/nameser.h"
#include "resolv.h"
#endif
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef _OSD_POSIX
#undef STATUS
#undef T_UNSPEC
#endif
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#include "dns.h"

char *_php3_gethostbyaddr(char *ip);
char *_php3_gethostbyname(char *name);

/* {{{ proto string gethostbyaddr(string ip_address)
   Get the Internet host name corresponding to a given IP address */
PHP_FUNCTION(gethostbyaddr)
{
	pval *arg;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	return_value->value.str.val = _php3_gethostbyaddr(arg->value.str.val);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
}
/* }}} */


char *_php3_gethostbyaddr(char *ip)
{
	unsigned long addr;
	struct hostent *hp;

	if ((int) (addr = inet_addr(ip)) == -1) {
#if DEBUG
		php3_error(E_WARNING, "address not in a.b.c.d form");
#endif
		return estrdup(ip);
	}
	hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
	if (!hp) {
#if DEBUG
		php3_error(E_WARNING, "Unable to resolve %s\n", ip);
#endif
		return estrdup(ip);
	}
	return estrdup(hp->h_name);
}

/* {{{ proto string gethostbyname(string hostname)
   Get the IP address corresponding to a given Internet host name */
PHP_FUNCTION(gethostbyname)
{
	pval *arg;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	return_value->value.str.val = _php3_gethostbyname(arg->value.str.val);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto array gethostbynamel(string hostname)
   Return a list of IP addresses that a given hostname resolves to. */
PHP_FUNCTION(gethostbynamel)
{
	pval *arg;
	struct hostent *hp;
	struct in_addr in;
	int i;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	hp = gethostbyname(arg->value.str.val);
	if (hp == NULL || hp->h_addr_list == NULL) {
#if DEBUG
		php3_error(E_WARNING, "Unable to resolve %s\n", arg->value.str.val);
#endif
		return;
	}

	for (i = 0 ; hp->h_addr_list[i] != 0 ; i++) {
		memcpy(&in.s_addr, hp->h_addr_list[i], sizeof(in.s_addr));
		add_next_index_string(return_value, inet_ntoa(in), 1);
	}

	return;
}
/* }}} */

char *_php3_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr in;

	hp = gethostbyname(name);
	if (!hp || !hp->h_addr_list) {
#if DEBUG
		php3_error(E_WARNING, "Unable to resolve %s\n", name);
#endif
		return estrdup(name);
	}
	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));
	return estrdup(inet_ntoa(in));
}

#if !(WIN32|WINNT)||HAVE_BINDLIB

/* {{{ proto int checkdnsrr(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(checkdnsrr)
{
	pval *arg1,*arg2;
	int type,i;
#ifndef MAXPACKET
#define MAXPACKET  8192 /* max packet size used internally by BIND */
#endif
	u_char ans[MAXPACKET];
	
	switch (ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		type = T_MX;
		convert_to_string(arg1);
		break;
	case 2:
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string(arg1);
		convert_to_string(arg2);
		if ( !strcasecmp("A",arg2->value.str.val) ) type = T_A;
		else if ( !strcasecmp("NS",arg2->value.str.val) ) type = T_NS;
		else if ( !strcasecmp("MX",arg2->value.str.val) ) type = T_MX;
		else if ( !strcasecmp("PTR",arg2->value.str.val) ) type = T_PTR;
		else if ( !strcasecmp("ANY",arg2->value.str.val) ) type = T_ANY;
		else if ( !strcasecmp("SOA",arg2->value.str.val) ) type = T_SOA;
		else if ( !strcasecmp("CNAME",arg2->value.str.val) ) type = T_CNAME;
		else {
			php3_error(E_WARNING,"Type '%s' not supported",arg2->value.str.val);
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	i = res_search(arg1->value.str.val,C_IN,type,ans,sizeof(ans));
	if ( i < 0 ) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#ifndef HFIXEDSZ
#define HFIXEDSZ        12      /* fixed data in header <arpa/nameser.h> */
#endif /* HFIXEDSZ */

#ifndef QFIXEDSZ
#define QFIXEDSZ        4       /* fixed data in query <arpa/nameser.h> */
#endif /* QFIXEDSZ */

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  256
#endif /* MAXHOSTNAMELEN */

/* {{{ proto int getmxrr(string hostname, array mxhosts [, array weight])
   Get MX records corresponding to a given Internet host name */
PHP_FUNCTION(getmxrr)
{
	pval *host, *mx_list, *weight_list;
	int need_weight = 0;
	int count,qdc;
	u_short type,weight;
	u_char ans[MAXPACKET];
	char buf[MAXHOSTNAMELEN];
	HEADER *hp;
	u_char *cp,*end;
	int i;

	switch(ARG_COUNT(ht)) {
	case 2:
		if (getParameters(ht, 2, &host, &mx_list) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		if (!ParameterPassedByReference(ht, 2)) {
			php3_error(E_WARNING, "Array to be filled with values must be passed by reference.");
			RETURN_FALSE;
		}
        break;
    case 3:
		if (getParameters(ht, 3, &host, &mx_list, &weight_list) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		if (!ParameterPassedByReference(ht, 2) || !ParameterPassedByReference(ht, 3)) {
			php3_error(E_WARNING, "Array to be filled with values must be passed by reference.");
			RETURN_FALSE;
		}
        need_weight = 1;
		pval_destructor(weight_list); /* start with clean array */
		if ( array_init(weight_list) == FAILURE ) {
			RETURN_FALSE;
		}
        break;
    default:
		WRONG_PARAM_COUNT;
    }

    convert_to_string( host );
    pval_destructor(mx_list); /* start with clean array */
    if ( array_init(mx_list) == FAILURE ) {
        RETURN_FALSE;
    }

	/* Go! */
	i = res_search(host->value.str.val,C_IN,T_MX,(u_char *)&ans,sizeof(ans));
	if ( i < 0 ) {
		RETURN_FALSE;
	}
	if ( i > sizeof(ans) ) i = sizeof(ans);
	hp = (HEADER *)&ans;
	cp = (u_char *)&ans + HFIXEDSZ;
	end = (u_char *)&ans +i;
	for ( qdc = ntohs((unsigned short)hp->qdcount); qdc--; cp += i + QFIXEDSZ) {
		if ( (i = dn_skipname(cp,end)) < 0 ) {
			RETURN_FALSE;
		}
	}
	count = ntohs((unsigned short)hp->ancount);
	while ( --count >= 0 && cp < end ) {
		if ( (i = dn_skipname(cp,end)) < 0 ) {
			RETURN_FALSE;
		}
		cp += i;
		GETSHORT(type,cp);
		cp += INT16SZ + INT32SZ;
		GETSHORT(i,cp);
		if ( type != T_MX ) {
			cp += i;
			continue;
		}
		GETSHORT(weight,cp);
		if ( (i = dn_expand(ans,end,cp,buf,sizeof(buf)-1)) < 0 ) {
			RETURN_FALSE;
		}
		cp += i;
		add_next_index_string(mx_list, buf, 1);
		if ( need_weight ) {
			add_next_index_long(weight_list, weight);
		}
	}
	RETURN_TRUE;
}
/* }}} */

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
