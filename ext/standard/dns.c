/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: The typical suspects                                        |
   |          Pollita <pollita@php.net>                                   |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes */
#include "php.h"

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
#if HAVE_LIBBIND
#ifndef WINNT
#define WINNT 1
#endif
/* located in www.php.net/extra/bindlib.zip */
#if HAVE_ARPA_INET_H 
#include "arpa/inet.h"
#endif
#include "netdb.h"
#if HAVE_ARPA_NAMESERV_H
#include "arpa/nameser.h"
#endif
#if HAVE_RESOLV_H
#include "resolv.h"
#endif
#endif /* HAVE_LIBBIND */
#include <winsock.h>
#else	/* This holds good for NetWare too, both for Winsock and Berkeley sockets */
#include <netinet/in.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <netdb.h>
#ifdef _OSD_POSIX
#undef STATUS
#undef T_UNSPEC
#endif
#if HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#if HAVE_RESOLV_H
#include <resolv.h>
#endif
#endif

/* Borrowed from SYS/SOCKET.H */
#if defined(NETWARE) && defined(USE_WINSOCK)
#define AF_INET 2   /* internetwork: UDP, TCP, etc. */
#endif

#include "dns.h"
/* }}} */

static char *php_gethostbyaddr(char *ip);
static char *php_gethostbyname(char *name);

/* {{{ proto string gethostbyaddr(string ip_address)
   Get the Internet host name corresponding to a given IP address */
PHP_FUNCTION(gethostbyaddr)
{
	zval **arg;
	char *addr;	
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(arg);
	
	addr = php_gethostbyaddr(Z_STRVAL_PP(arg));

	if(addr == NULL) {
#if HAVE_IPV6 && !defined(__MacOSX__)
/* MacOSX at this time has support for IPv6, but not inet_pton()
 * so disabling IPv6 until further notice.  MacOSX 10.1.2 (kalowsky) */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Address is not a valid IPv4 or IPv6 address");
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Address is not in a.b.c.d form");
#endif
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(addr, 0);
	}
}
/* }}} */

/* {{{ php_gethostbyaddr */
static char *php_gethostbyaddr(char *ip)
{
#if HAVE_IPV6 && !defined(__MacOSX__)
/* MacOSX at this time has support for IPv6, but not inet_pton()
 * so disabling IPv6 until further notice.  MacOSX 10.1.2 (kalowsky) */
	struct in6_addr addr6;
#endif
	struct in_addr addr;
	struct hostent *hp;

#if HAVE_IPV6 && !defined(__MacOSX__)
/* MacOSX at this time has support for IPv6, but not inet_pton()
 * so disabling IPv6 until further notice.  MacOSX 10.1.2 (kalowsky) */
	if (inet_pton(AF_INET6, ip, &addr6)) {
		hp = gethostbyaddr((char *) &addr6, sizeof(addr6), AF_INET6);
	} else if (inet_pton(AF_INET, ip, &addr)) {
		hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
	} else {
		return NULL;
	}
#else
	addr.s_addr = inet_addr(ip);

	if (addr.s_addr == -1) {
		return NULL;
	}

	hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
#endif

	if (!hp || hp->h_name == NULL || hp->h_name[0] == '\0') {
		return estrdup(ip);
	}

	return estrdup(hp->h_name);
}
/* }}} */

/* {{{ proto string gethostbyname(string hostname)
   Get the IP address corresponding to a given Internet host name */
PHP_FUNCTION(gethostbyname)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(arg);

	RETVAL_STRING(php_gethostbyname(Z_STRVAL_PP(arg)), 0);
}
/* }}} */

/* {{{ proto array gethostbynamel(string hostname)
   Return a list of IP addresses that a given hostname resolves to. */
PHP_FUNCTION(gethostbynamel)
{
	zval **arg;
	struct hostent *hp;
	struct in_addr in;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(arg);

	array_init(return_value);

	hp = gethostbyname(Z_STRVAL_PP(arg));
	if (hp == NULL || hp->h_addr_list == NULL) {
		RETURN_FALSE;
	}

	for (i = 0 ; hp->h_addr_list[i] != 0 ; i++) {
		in = *(struct in_addr *) hp->h_addr_list[i];
		add_next_index_string(return_value, inet_ntoa(in), 1);
	}
}
/* }}} */

/* {{{ php_gethostbyname */
static char *php_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr in;

	hp = gethostbyname(name);

	if (!hp || !hp->h_addr_list) {
		return estrdup(name);
	}

	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));

	return estrdup(inet_ntoa(in));
}
/* }}} */

#if HAVE_RES_SEARCH && !(defined(__BEOS__)||defined(PHP_WIN32) || defined(NETWARE))

/* {{{ proto int dns_check_recored(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(dns_check_record)
{
	zval **arg1, **arg2;
	int type, i;
#ifndef MAXPACKET
#define MAXPACKET  8192 /* max packet size used internally by BIND */
#endif
	u_char ans[MAXPACKET];
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			type = T_MX;
			convert_to_string_ex(arg1);
			break;

		case 2:
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg1);
			convert_to_string_ex(arg2);

			if (!strcasecmp("A", Z_STRVAL_PP(arg2))) type = T_A;
			else if (!strcasecmp("NS",    Z_STRVAL_PP(arg2))) type = T_NS;
			else if (!strcasecmp("MX",    Z_STRVAL_PP(arg2))) type = T_MX;
			else if (!strcasecmp("PTR",   Z_STRVAL_PP(arg2))) type = T_PTR;
			else if (!strcasecmp("ANY",   Z_STRVAL_PP(arg2))) type = T_ANY;
			else if (!strcasecmp("SOA",   Z_STRVAL_PP(arg2))) type = T_SOA;
			else if (!strcasecmp("CNAME", Z_STRVAL_PP(arg2))) type = T_CNAME;
			else if (!strcasecmp("AAAA",  Z_STRVAL_PP(arg2))) type = T_AAAA;
			else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%s' not supported", Z_STRVAL_PP(arg2));
				RETURN_FALSE;
			}
			break;

		default:
			WRONG_PARAM_COUNT;
	}

	i = res_search(Z_STRVAL_PP(arg1), C_IN, type, ans, sizeof(ans));

	if (i < 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#if HAVE_DNS_FUNCS

/* PHP_DNS_xx = 1<<(T_xx-1) */
#define PHP_DNS_A      0x00000001
#define PHP_DNS_NS     0x00000002
#define PHP_DNS_CNAME  0x00000020
#define PHP_DNS_SOA    0x00000040
#define PHP_DNS_PTR    0x00000800
#define PHP_DNS_HINFO  0x00001000
#define PHP_DNS_MX     0x00004000
#define PHP_DNS_TXT    0x00008000    
#define PHP_DNS_AAAA   0x08000000
#define PHP_DNS_ANY    0x10000000
#define PHP_DNS_ALL    (PHP_DNS_A|PHP_DNS_NS|PHP_DNS_CNAME|PHP_DNS_SOA|PHP_DNS_PTR|PHP_DNS_HINFO|PHP_DNS_MX|PHP_DNS_TXT|PHP_DNS_AAAA)

PHP_MINIT_FUNCTION(dns) {
	REGISTER_LONG_CONSTANT("DNS_A",     PHP_DNS_A,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_NS",    PHP_DNS_NS,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_CNAME", PHP_DNS_CNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_SOA",   PHP_DNS_SOA,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_PTR",   PHP_DNS_PTR,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_HINFO", PHP_DNS_HINFO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_MX",    PHP_DNS_MX,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_TXT",   PHP_DNS_TXT,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_AAAA",  PHP_DNS_AAAA,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_ANY",   PHP_DNS_ANY,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_ALL",   PHP_DNS_ALL,   CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

#ifndef HFIXEDSZ
#define HFIXEDSZ        12      /* fixed data in header <arpa/nameser.h> */
#endif /* HFIXEDSZ */

#ifndef QFIXEDSZ
#define QFIXEDSZ        4       /* fixed data in query <arpa/nameser.h> */
#endif /* QFIXEDSZ */

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  256
#endif /* MAXHOSTNAMELEN */

#ifndef MAXRESOURCERECORDS
#define MAXRESOURCERECORDS	64
#endif /* MAXRESOURCERECORDS */

typedef union {
	HEADER qb1;
	u_char qb2[65536];
} querybuf;

/* {{{ php_parserr */
static u_char *php_parserr(u_char *cp, querybuf *answer, int type_to_fetch, int store, zval **subarray) {
	u_short type, class, dlen;
	u_long ttl;
	long n, i;
	u_short s;
	u_char *tp;
	char name[MAXHOSTNAMELEN];

	n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof(name)) - 2);
	if (n < 0) {
		return NULL;
	}
	cp += n;
	
	GETSHORT(type, cp);
	GETSHORT(class, cp);
	GETLONG(ttl, cp);
	GETSHORT(dlen, cp);
	if (type_to_fetch != T_ANY && type != type_to_fetch) {
		*subarray = NULL;
		cp += dlen;
		return cp;
	}

	if (!store) {
		*subarray = NULL;
		cp += dlen;
		return cp;
	}

	MAKE_STD_ZVAL(*subarray);
	array_init(*subarray);

	add_assoc_string(*subarray, "host", name, 1);
	switch (type) {
		case T_A:
			add_assoc_string(*subarray, "type", "A", 1);
			sprintf(name, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
			add_assoc_string(*subarray, "ip", name, 1);
			cp += dlen;
			break;
		case T_MX:
			add_assoc_string(*subarray, "type", "MX", 1);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "pri", n);
			/* no break; */
		case T_CNAME:
			if (type == T_CNAME)
				add_assoc_string(*subarray, "type", "CNAME", 1);
			/* no break; */
		case T_NS:
			if (type == T_NS)
				add_assoc_string(*subarray, "type", "NS", 1);
			/* no break; */
		case T_PTR:
			if (type == T_PTR)
				add_assoc_string(*subarray, "type", "PTR", 1);
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "target", name, 1);
			break;
		case T_HINFO:
			/* See RFC 1010 for values */
			add_assoc_string(*subarray, "type", "HINFO", 1);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "cpu", n);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "os", n);
			break;
		case T_TXT:
			add_assoc_string(*subarray, "type", "TXT", 1);
			n = cp[0]; 
			for(i=1; i<=n; i++)
				name[i-1] = cp[i];
			name[i-1] = '\0';
			cp += dlen;
			add_assoc_string(*subarray, "txt", name, 1);
			break;
		case T_SOA:
			add_assoc_string(*subarray, "type", "SOA", 1);
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) -2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "mname", name, 1);
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) -2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "rname", name, 1);
			GETLONG(n, cp);
			add_assoc_long(*subarray, "serial", n);
			GETLONG(n, cp);
			add_assoc_long(*subarray, "refresh", n);
			GETLONG(n, cp);
			add_assoc_long(*subarray, "retry", n);
			GETLONG(n, cp);
			add_assoc_long(*subarray, "expire", n);
			GETLONG(n, cp);
			add_assoc_long(*subarray, "minimum-ttl", n);
			break;
		case T_AAAA:
			tp = name;
			for(i=0; i < 8; i++) {
				GETSHORT(s, cp);
				if (s > 0) {
					if (tp > (u_char *)name) {
						tp[0] = ':';
						tp++;
					}
					sprintf(tp,"%x",s);
					tp += strlen(tp);
				} else if (s == 0) {
					if ((tp > (u_char *)name) && (tp[-1] != ':')) {
						tp[0] = ':';
						tp++;
					}
				}
			}
			if ((tp > (u_char *)name) && (tp[-1] == ':'))
				tp[-1] = '\0';
			tp[0] = '\0';
			add_assoc_string(*subarray, "type", "AAAA", 1);
			add_assoc_string(*subarray, "ipv6", name, 1);
			break;
		default:
			cp += dlen;
	}

	add_assoc_string(*subarray, "class", "IN", 1);
	add_assoc_long(*subarray, "ttl", ttl);

	return cp;
}
/* }}} */

/* {{{ proto array|false dns_get_record(string hostname [, int type[, array authns, array addtl]])
   Get any Resource Record corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_record)
{
	zval *subarray[MAXRESOURCERECORDS];
	pval *addtl, *host, *authns, *fetch_type;
	int addtl_recs = 0;
	int type_to_fetch, type_param = PHP_DNS_ANY;
	int current_subarray = 0;
	struct __res_state res;
	HEADER *hp;
	querybuf buf, answer, *ans;
	u_char *cp = NULL, *end = NULL;
	long n, qd, an, ns = 0, ar = 0;
	int type, first_query = 1, store_results = 1;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters(ht, 1, &host) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if (zend_get_parameters(ht, 2, &host, &fetch_type) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			type_param = Z_LVAL_P(fetch_type);
			break;
		case 4:
			if (zend_get_parameters(ht, 4, &host, &fetch_type, &authns, &addtl) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			type_param = Z_LVAL_P(fetch_type);
			pval_destructor(authns);
			addtl_recs = 1;		/* We want the additional Records */
			array_init(authns);
			pval_destructor(addtl);
			array_init(addtl);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	
	convert_to_string(host);

	if (type_param&~PHP_DNS_ALL && type_param!=PHP_DNS_ANY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%d' not supported", type_param);
		RETURN_FALSE;
	}

	/* Initialize the return array */
	array_init(return_value);

	/* - We emulate an or'ed type mask by querying type by type. (Steps 0 - 8)
	 *   If additional info is wanted we check again with T_ANY (step 9/10)
	 *   store_results is used to skip storing the results retrieved in step
	 *   10 when results were already fetched.
	 * - In case of PHP_DNS_ANY we use the directly fetch T_ANY. (step 10)
	 */
	for(type = (type_param==PHP_DNS_ANY ? 10 : 0); type < (addtl_recs ? 11 : 9) || first_query; type++)
	{
		first_query = 0;
		switch(type) {
			case 0: 
				type_to_fetch = type_param&PHP_DNS_A     ? T_A     : 0;
				break;
			case 1: 
				type_to_fetch = type_param&PHP_DNS_NS    ? T_NS    : 0;
				break;
			case 2: 
				type_to_fetch = type_param&PHP_DNS_CNAME ? T_CNAME : 0;
				break;
			case 3: 
				type_to_fetch = type_param&PHP_DNS_SOA   ? T_SOA   : 0;
				break;
			case 4: 
				type_to_fetch = type_param&PHP_DNS_PTR   ? T_PTR   : 0;
				break;
			case 5: 
				type_to_fetch = type_param&PHP_DNS_HINFO ? T_HINFO : 0;
				break;
			case 6: 
				type_to_fetch = type_param&PHP_DNS_MX    ? T_MX    : 0;
				break;
			case 7: 
				type_to_fetch = type_param&PHP_DNS_TXT   ? T_TXT   : 0;
				break;
			case 8:
				type_to_fetch = type_param&PHP_DNS_AAAA	 ? T_AAAA  : 0;
				break;
			case 9:
				store_results = 0;
				continue;
			default:
			case 10:
				type_to_fetch = T_ANY;
				break;
		}
		if (type_to_fetch) {
			res_ninit(&res);
			res.retrans = 5;
			res.options &= ~RES_DEFNAMES;
		
			n = res_nmkquery(&res, QUERY, Z_STRVAL_P(host), C_IN, type_to_fetch, NULL, 0, NULL, buf.qb2, sizeof buf);
			if (n<0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "res_nmkquery() failed");
				zval_ptr_dtor(&return_value);
				RETURN_FALSE;
			}
			n = res_nsend(&res, buf.qb2, n, answer.qb2, sizeof answer);
			if (n<0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "res_nsend() failed");
				zval_ptr_dtor(&return_value);
				RETURN_FALSE;
			}
		
			cp = answer.qb2 + HFIXEDSZ;
			end = answer.qb2 + n;
			ans = &answer;
			hp = (HEADER *)ans;
			qd = ntohs(hp->qdcount);
			an = ntohs(hp->ancount);
			ns = ntohs(hp->nscount);
			ar = ntohs(hp->arcount);
		
			/* Skip QD entries, they're only used by dn_expand later on */
			while (qd-- > 0) {
				n = dn_skipname(cp, end);
				if (n < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse DNS data received");
					zval_ptr_dtor(&return_value);
					RETURN_FALSE;
				}
				cp += n + QFIXEDSZ;
			}
		
			/* YAY! Our real answers! */
			while (an-- && cp && cp < end) {
				cp = php_parserr(cp, &answer, type_to_fetch, store_results, &subarray[current_subarray]);
				if (subarray[current_subarray] != NULL && store_results)
					zend_hash_next_index_insert(HASH_OF(return_value), (void *)&subarray[current_subarray], sizeof(zval *), NULL);
				current_subarray++;
			}
			res_nclose(&res);
		}
	}

	if (addtl_recs) {
		/* List of Authoritative Name Servers */
		while (ns-- > 0 && cp && cp < end) {
			cp = php_parserr(cp, &answer, T_ANY, 1, &subarray[current_subarray]);
			if (subarray[current_subarray] != NULL)
				zend_hash_next_index_insert(HASH_OF(authns), (void *)&subarray[current_subarray], sizeof(zval *), NULL);
			current_subarray++;
		}
		/* Additional records associated with authoritative name servers */
		while (ar-- > 0 && cp && cp < end) {
			cp = php_parserr(cp, &answer, T_ANY, 1, &subarray[current_subarray]);
			if (subarray[current_subarray] != NULL)
				zend_hash_next_index_insert(HASH_OF(addtl), (void *)&subarray[current_subarray], sizeof(zval *), NULL);
			current_subarray++;
		}
	}
}
/* }}} */

/* {{{ proto bool dns_get_mx(string hostname, array mxhosts [, array weight])
   Get MX records corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_mx)
{
	pval *host, *mx_list, *weight_list;
	int need_weight = 0;
	int count, qdc;
	u_short type, weight;
	u_char ans[MAXPACKET];
	char buf[MAXHOSTNAMELEN];
	HEADER *hp;
	u_char *cp, *end;
	int i;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters(ht, 2, &host, &mx_list) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;

		case 3:
			if (zend_get_parameters(ht, 3, &host, &mx_list, &weight_list) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			need_weight = 1;
			pval_destructor(weight_list); /* start with clean array */
			array_init(weight_list);
			break;

		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string(host);
	pval_destructor(mx_list); /* start with clean array */
	array_init(mx_list);

	/* Go! */
	i = res_search(Z_STRVAL_P(host), C_IN, T_MX, (u_char *)&ans, sizeof(ans));
	if (i < 0) {
		RETURN_FALSE;
	}
	if (i > sizeof(ans)) {
		i = sizeof(ans);
	}
	hp = (HEADER *)&ans;
	cp = (u_char *)&ans + HFIXEDSZ;
	end = (u_char *)&ans +i;
	for (qdc = ntohs((unsigned short)hp->qdcount); qdc--; cp += i + QFIXEDSZ) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			RETURN_FALSE;
		}
	}
	count = ntohs((unsigned short)hp->ancount);
	while (--count >= 0 && cp < end) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			RETURN_FALSE;
		}
		cp += i;
		GETSHORT(type, cp);
		cp += INT16SZ + INT32SZ;
		GETSHORT(i, cp);
		if (type != T_MX) {
			cp += i;
			continue;
		}
		GETSHORT(weight, cp);
		if ((i = dn_expand(ans, end, cp, buf, sizeof(buf)-1)) < 0) {
			RETURN_FALSE;
		}
		cp += i;
		add_next_index_string(mx_list, buf, 1);
		if (need_weight) {
			add_next_index_long(weight_list, weight);
		}
	}
	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_DNS_FUNCS */
#endif /* HAVE_RES_SEARCH && !(defined(__BEOS__)||defined(PHP_WIN32) || defined(NETWARE)) */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
