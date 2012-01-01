/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "php_network.h"

#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
# include "win32/inet.h"
# include <winsock2.h>
# include <windows.h>
# include <Ws2tcpip.h>
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
#ifdef DARWIN
# define BIND_8_COMPAT 1
#endif
#include <arpa/nameser.h>
#endif
#if HAVE_RESOLV_H
#include <resolv.h>
#endif
#ifdef HAVE_DNS_H
#include <dns.h>
#endif
#endif

/* Borrowed from SYS/SOCKET.H */
#if defined(NETWARE) && defined(USE_WINSOCK)
#define AF_INET 2   /* internetwork: UDP, TCP, etc. */
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 255
#endif

/* For the local hostname obtained via gethostname which is different from the
   dns-related MAXHOSTNAMELEN constant above */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

#include "php_dns.h"

/* type compat */
#ifndef DNS_T_A
#define DNS_T_A		1
#endif
#ifndef DNS_T_NS
#define DNS_T_NS	2
#endif
#ifndef DNS_T_CNAME
#define DNS_T_CNAME	5
#endif
#ifndef DNS_T_SOA
#define DNS_T_SOA	6
#endif
#ifndef DNS_T_PTR
#define DNS_T_PTR	12
#endif
#ifndef DNS_T_HINFO
#define DNS_T_HINFO	13
#endif
#ifndef DNS_T_MINFO
#define DNS_T_MINFO	14
#endif
#ifndef DNS_T_MX
#define DNS_T_MX	15
#endif
#ifndef DNS_T_TXT
#define DNS_T_TXT	16
#endif
#ifndef DNS_T_AAAA
#define DNS_T_AAAA	28
#endif
#ifndef DNS_T_SRV
#define DNS_T_SRV	33
#endif
#ifndef DNS_T_NAPTR
#define DNS_T_NAPTR	35
#endif
#ifndef DNS_T_A6
#define DNS_T_A6	38
#endif

#ifndef DNS_T_ANY
#define DNS_T_ANY	255
#endif
/* }}} */

static char *php_gethostbyaddr(char *ip);
static char *php_gethostbyname(char *name);

#ifdef HAVE_GETHOSTNAME
/* {{{ proto string gethostname()
   Get the host name of the current machine */
PHP_FUNCTION(gethostname)
{
	char buf[HOST_NAME_MAX];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (gethostname(buf, sizeof(buf) - 1)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to fetch host [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_STRING(buf, 1);
}
/* }}} */
#endif

/* TODO: Reimplement the gethostby* functions using the new winxp+ API, in dns_win32.c, then
 we can have a dns.c, dns_unix.c and dns_win32.c instead of a messy dns.c full of #ifdef
*/

/* {{{ proto string gethostbyaddr(string ip_address)
   Get the Internet host name corresponding to a given IP address */
PHP_FUNCTION(gethostbyaddr)
{
	char *addr;
	int addr_len;
	char *hostname;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &addr, &addr_len) == FAILURE) {
		return;
	}

	hostname = php_gethostbyaddr(addr);

	if (hostname == NULL) {
#if HAVE_IPV6 && HAVE_INET_PTON
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Address is not a valid IPv4 or IPv6 address");
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Address is not in a.b.c.d form");
#endif
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(hostname, 0);
	}
}
/* }}} */

/* {{{ php_gethostbyaddr */
static char *php_gethostbyaddr(char *ip)
{
#if HAVE_IPV6 && HAVE_INET_PTON
	struct in6_addr addr6;
#endif
	struct in_addr addr;
	struct hostent *hp;

#if HAVE_IPV6 && HAVE_INET_PTON
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
	char *hostname;
	int hostname_len;
	char *addr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostname, &hostname_len) == FAILURE) {
		return;
	}

	addr = php_gethostbyname(hostname);

	RETVAL_STRING(addr, 0);
}
/* }}} */

/* {{{ proto array gethostbynamel(string hostname)
   Return a list of IP addresses that a given hostname resolves to. */
PHP_FUNCTION(gethostbynamel)
{
	char *hostname;
	int hostname_len;
	struct hostent *hp;
	struct in_addr in;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hostname, &hostname_len) == FAILURE) {
		return;
	}

	hp = gethostbyname(hostname);
	if (hp == NULL || hp->h_addr_list == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

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

	if (!hp || !*(hp->h_addr_list)) {
		return estrdup(name);
	}

	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));

	return estrdup(inet_ntoa(in));
}
/* }}} */

#if HAVE_FULL_DNS_FUNCS || defined(PHP_WIN32)
# define PHP_DNS_NUM_TYPES	12	/* Number of DNS Types Supported by PHP currently */

# define PHP_DNS_A      0x00000001
# define PHP_DNS_NS     0x00000002
# define PHP_DNS_CNAME  0x00000010
# define PHP_DNS_SOA    0x00000020
# define PHP_DNS_PTR    0x00000800
# define PHP_DNS_HINFO  0x00001000
# define PHP_DNS_MX     0x00004000
# define PHP_DNS_TXT    0x00008000
# define PHP_DNS_A6     0x01000000
# define PHP_DNS_SRV    0x02000000
# define PHP_DNS_NAPTR  0x04000000
# define PHP_DNS_AAAA   0x08000000
# define PHP_DNS_ANY    0x10000000
# define PHP_DNS_ALL    (PHP_DNS_A|PHP_DNS_NS|PHP_DNS_CNAME|PHP_DNS_SOA|PHP_DNS_PTR|PHP_DNS_HINFO|PHP_DNS_MX|PHP_DNS_TXT|PHP_DNS_A6|PHP_DNS_SRV|PHP_DNS_NAPTR|PHP_DNS_AAAA)
#endif /* HAVE_FULL_DNS_FUNCS || defined(PHP_WIN32) */

/* Note: These functions are defined in ext/standard/dns_win32.c for Windows! */
#if !defined(PHP_WIN32) && (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))
  
#ifndef HFIXEDSZ
#define HFIXEDSZ        12      /* fixed data in header <arpa/nameser.h> */
#endif /* HFIXEDSZ */

#ifndef QFIXEDSZ
#define QFIXEDSZ        4       /* fixed data in query <arpa/nameser.h> */
#endif /* QFIXEDSZ */

#undef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  1024

#ifndef MAXRESOURCERECORDS
#define MAXRESOURCERECORDS	64
#endif /* MAXRESOURCERECORDS */

typedef union {
	HEADER qb1;
	u_char qb2[65536];
} querybuf;

/* just a hack to free resources allocated by glibc in __res_nsend()
 * See also:
 *   res_thread_freeres() in glibc/resolv/res_init.c
 *   __libc_res_nsend()   in resolv/res_send.c
 * */

#if defined(__GLIBC__) && !defined(HAVE_DEPRECATED_DNS_FUNCS)
#define php_dns_free_res(__res__) _php_dns_free_res(__res__)
static void _php_dns_free_res(struct __res_state res) { /* {{{ */
	int ns;
	for (ns = 0; ns < MAXNS; ns++) {
		if (res._u._ext.nsaddrs[ns] != NULL) {
			free (res._u._ext.nsaddrs[ns]);
			res._u._ext.nsaddrs[ns] = NULL;
		}
	}
} /* }}} */
#else
#define php_dns_free_res(__res__)
#endif

/* {{{ proto bool dns_check_record(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(dns_check_record)
{
#ifndef MAXPACKET
#define MAXPACKET  8192 /* max packet size used internally by BIND */
#endif
	u_char ans[MAXPACKET];
	char *hostname, *rectype = NULL;
	int hostname_len, rectype_len = 0;
	int type = T_MX, i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &hostname, &hostname_len, &rectype, &rectype_len) == FAILURE) {
		return;
	}

	if (hostname_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Host cannot be empty");
		RETURN_FALSE;
	}

	if (rectype) {
		if (!strcasecmp("A",     rectype)) type = T_A;
		else if (!strcasecmp("NS",    rectype)) type = DNS_T_NS;
		else if (!strcasecmp("MX",    rectype)) type = DNS_T_MX;
		else if (!strcasecmp("PTR",   rectype)) type = DNS_T_PTR;
		else if (!strcasecmp("ANY",   rectype)) type = DNS_T_ANY;
		else if (!strcasecmp("SOA",   rectype)) type = DNS_T_SOA;
		else if (!strcasecmp("TXT",   rectype)) type = DNS_T_TXT;
		else if (!strcasecmp("CNAME", rectype)) type = DNS_T_CNAME;
		else if (!strcasecmp("AAAA",  rectype)) type = DNS_T_AAAA;
		else if (!strcasecmp("SRV",   rectype)) type = DNS_T_SRV;
		else if (!strcasecmp("NAPTR", rectype)) type = DNS_T_NAPTR;
		else if (!strcasecmp("A6",    rectype)) type = DNS_T_A6;
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%s' not supported", rectype);
			RETURN_FALSE;
		}
	}

#if defined(HAVE_DNS_SEARCH)
	handle = dns_open(NULL);
	if (handle == NULL) {
		RETURN_FALSE;
	}
#elif defined(HAVE_RES_NSEARCH)
    memset(&state, 0, sizeof(state));
    if (res_ninit(handle)) {
			RETURN_FALSE;
	}
#else
	res_init();
#endif

	RETVAL_TRUE;
	i = php_dns_search(handle, hostname, C_IN, type, ans, sizeof(ans));

	if (i < 0) {
		RETVAL_FALSE;
	}

	php_dns_free_handle(handle);
}
/* }}} */

#if HAVE_FULL_DNS_FUNCS

/* {{{ php_parserr */
static u_char *php_parserr(u_char *cp, querybuf *answer, int type_to_fetch, int store, int raw, zval **subarray)
{
	u_short type, class, dlen;
	u_long ttl;
	long n, i;
	u_short s;
	u_char *tp, *p;
	char name[MAXHOSTNAMELEN];
	int have_v6_break = 0, in_v6_break = 0;

	*subarray = NULL;

	n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, sizeof(name) - 2);
	if (n < 0) {
		return NULL;
	}
	cp += n;

	GETSHORT(type, cp);
	GETSHORT(class, cp);
	GETLONG(ttl, cp);
	GETSHORT(dlen, cp);
	if (type_to_fetch != T_ANY && type != type_to_fetch) {
		cp += dlen;
		return cp;
	}

	if (!store) {
		cp += dlen;
		return cp;
	}

	ALLOC_INIT_ZVAL(*subarray);
	array_init(*subarray);

	add_assoc_string(*subarray, "host", name, 1);
	add_assoc_string(*subarray, "class", "IN", 1);
	add_assoc_long(*subarray, "ttl", ttl);

	if (raw) {
		add_assoc_long(*subarray, "type", type);
		add_assoc_stringl(*subarray, "data", (char*) cp, (uint) dlen, 1);
		cp += dlen;
		return cp;
	}

	switch (type) {
		case DNS_T_A:
			add_assoc_string(*subarray, "type", "A", 1);
			snprintf(name, sizeof(name), "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
			add_assoc_string(*subarray, "ip", name, 1);
			cp += dlen;
			break;
		case DNS_T_MX:
			add_assoc_string(*subarray, "type", "MX", 1);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "pri", n);
			/* no break; */
		case DNS_T_CNAME:
			if (type == DNS_T_CNAME) {
				add_assoc_string(*subarray, "type", "CNAME", 1);
			}
			/* no break; */
		case DNS_T_NS:
			if (type == DNS_T_NS) {
				add_assoc_string(*subarray, "type", "NS", 1);
			}
			/* no break; */
		case DNS_T_PTR:
			if (type == DNS_T_PTR) {
				add_assoc_string(*subarray, "type", "PTR", 1);
			}
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "target", name, 1);
			break;
		case DNS_T_HINFO:
			/* See RFC 1010 for values */
			add_assoc_string(*subarray, "type", "HINFO", 1);
			n = *cp & 0xFF;
			cp++;
			add_assoc_stringl(*subarray, "cpu", (char*)cp, n, 1);
			cp += n;
			n = *cp & 0xFF;
			cp++;
			add_assoc_stringl(*subarray, "os", (char*)cp, n, 1);
			cp += n;
			break;
		case DNS_T_TXT:
			{
				int ll = 0;
				zval *entries = NULL;

				add_assoc_string(*subarray, "type", "TXT", 1);
				tp = emalloc(dlen + 1);
				
				MAKE_STD_ZVAL(entries);
				array_init(entries);
				
				while (ll < dlen) {
					n = cp[ll];
					memcpy(tp + ll , cp + ll + 1, n);
					add_next_index_stringl(entries, cp + ll + 1, n, 1);
					ll = ll + n + 1;
				}
				tp[dlen] = '\0';
				cp += dlen;

				add_assoc_stringl(*subarray, "txt", tp, dlen - 1, 0);
				add_assoc_zval(*subarray, "entries", entries);
			}
			break;
		case DNS_T_SOA:
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
		case DNS_T_AAAA:
			tp = (u_char*)name;
			for(i=0; i < 8; i++) {
				GETSHORT(s, cp);
				if (s != 0) {
					if (tp > (u_char *)name) {
						in_v6_break = 0;
						tp[0] = ':';
						tp++;
					}
					tp += sprintf((char*)tp,"%x",s);
				} else {
					if (!have_v6_break) {
						have_v6_break = 1;
						in_v6_break = 1;
						tp[0] = ':';
						tp++;
					} else if (!in_v6_break) {
						tp[0] = ':';
						tp++;
						tp[0] = '0';
						tp++;
					}
				}
			}
			if (have_v6_break && in_v6_break) {
				tp[0] = ':';
				tp++;
			}
			tp[0] = '\0';
			add_assoc_string(*subarray, "type", "AAAA", 1);
			add_assoc_string(*subarray, "ipv6", name, 1);
			break;
		case DNS_T_A6:
			p = cp;
			add_assoc_string(*subarray, "type", "A6", 1);
			n = ((int)cp[0]) & 0xFF;
			cp++;
			add_assoc_long(*subarray, "masklen", n);
			tp = (u_char*)name;
			if (n > 15) {
				have_v6_break = 1;
				in_v6_break = 1;
				tp[0] = ':';
				tp++;
			}
			if (n % 16 > 8) {
				/* Partial short */
				if (cp[0] != 0) {
					if (tp > (u_char *)name) {
						in_v6_break = 0;
						tp[0] = ':';
						tp++;
					}
					sprintf((char*)tp, "%x", cp[0] & 0xFF);
				} else {
					if (!have_v6_break) {
						have_v6_break = 1;
						in_v6_break = 1;
						tp[0] = ':';
						tp++;
					} else if (!in_v6_break) {
						tp[0] = ':';
						tp++;
						tp[0] = '0';
						tp++;
					}
				}
				cp++;
			}
			for (i = (n + 8) / 16; i < 8; i++) {
				GETSHORT(s, cp);
				if (s != 0) {
					if (tp > (u_char *)name) {
						in_v6_break = 0;
						tp[0] = ':';
						tp++;
					}
					tp += sprintf((char*)tp,"%x",s);
				} else {
					if (!have_v6_break) {
						have_v6_break = 1;
						in_v6_break = 1;
						tp[0] = ':';
						tp++;
					} else if (!in_v6_break) {
						tp[0] = ':';
						tp++;
						tp[0] = '0';
						tp++;
					}
				}
			}
			if (have_v6_break && in_v6_break) {
				tp[0] = ':';
				tp++;
			}
			tp[0] = '\0';
			add_assoc_string(*subarray, "ipv6", name, 1);
			if (cp < p + dlen) {
				n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) - 2);
				if (n < 0) {
					return NULL;
				}
				cp += n;
				add_assoc_string(*subarray, "chain", name, 1);
			}
			break;
		case DNS_T_SRV:
			add_assoc_string(*subarray, "type", "SRV", 1);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "pri", n);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "weight", n);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "port", n);
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "target", name, 1);
			break;
		case DNS_T_NAPTR:
			add_assoc_string(*subarray, "type", "NAPTR", 1);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "order", n);
			GETSHORT(n, cp);
			add_assoc_long(*subarray, "pref", n);
			n = (cp[0] & 0xFF);
			add_assoc_stringl(*subarray, "flags", (char*)++cp, n, 1);
			cp += n;
			n = (cp[0] & 0xFF);
			add_assoc_stringl(*subarray, "services", (char*)++cp, n, 1);
			cp += n;
			n = (cp[0] & 0xFF);
			add_assoc_stringl(*subarray, "regex", (char*)++cp, n, 1);
			cp += n;
			n = dn_expand(answer->qb2, answer->qb2+65536, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(*subarray, "replacement", name, 1);
			break;
		default:
			zval_ptr_dtor(subarray);
			*subarray = NULL;
			cp += dlen;
			break;
	}

	return cp;
}
/* }}} */

/* {{{ proto array|false dns_get_record(string hostname [, int type[, array authns, array addtl]])
   Get any Resource Record corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_record)
{
	char *hostname;
	int hostname_len;
	long type_param = PHP_DNS_ANY;
	zval *authns = NULL, *addtl = NULL;
	int type_to_fetch;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif
	HEADER *hp;
	querybuf answer;
	u_char *cp = NULL, *end = NULL;
	int n, qd, an, ns = 0, ar = 0;
	int type, first_query = 1, store_results = 1;
	zend_bool raw = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz!z!b",
			&hostname, &hostname_len, &type_param, &authns, &addtl, &raw) == FAILURE) {
		return;
	}

	if (authns) {
		zval_dtor(authns);
		array_init(authns);
	}
	if (addtl) {
		zval_dtor(addtl);
		array_init(addtl);
	}

	if (!raw) {
		if ((type_param & ~PHP_DNS_ALL) && (type_param != PHP_DNS_ANY)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%ld' not supported", type_param);
			RETURN_FALSE;
		}
	} else {
		if ((type_param < 1) || (type_param > 0xFFFF)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Numeric DNS record type must be between 1 and 65535, '%ld' given", type_param);
			RETURN_FALSE;
		}
	}

	/* Initialize the return array */
	array_init(return_value);

	/* - We emulate an or'ed type mask by querying type by type. (Steps 0 - NUMTYPES-1 )
	 *   If additional info is wanted we check again with DNS_T_ANY (step NUMTYPES / NUMTYPES+1 )
	 *   store_results is used to skip storing the results retrieved in step
	 *   NUMTYPES+1 when results were already fetched.
	 * - In case of PHP_DNS_ANY we use the directly fetch DNS_T_ANY. (step NUMTYPES+1 )
	 * - In case of raw mode, we query only the requestd type instead of looping type by type
	 *   before going with the additional info stuff.
	 */

	if (raw) {
		type = -1;
	} else if (type_param == PHP_DNS_ANY) {
		type = PHP_DNS_NUM_TYPES + 1;
	} else {
		type = 0;
	}

	for ( ;
		type < (addtl ? (PHP_DNS_NUM_TYPES + 2) : PHP_DNS_NUM_TYPES) || first_query;
		type++
	) {
		first_query = 0;
		switch (type) {
			case -1: /* raw */
				type_to_fetch = type_param;
				/* skip over the rest and go directly to additional records */
				type = PHP_DNS_NUM_TYPES - 1;
				break;
			case 0:
				type_to_fetch = type_param&PHP_DNS_A     ? DNS_T_A     : 0;
				break;
			case 1:
				type_to_fetch = type_param&PHP_DNS_NS    ? DNS_T_NS    : 0;
				break;
			case 2:
				type_to_fetch = type_param&PHP_DNS_CNAME ? DNS_T_CNAME : 0;
				break;
			case 3:
				type_to_fetch = type_param&PHP_DNS_SOA   ? DNS_T_SOA   : 0;
				break;
			case 4:
				type_to_fetch = type_param&PHP_DNS_PTR   ? DNS_T_PTR   : 0;
				break;
			case 5:
				type_to_fetch = type_param&PHP_DNS_HINFO ? DNS_T_HINFO : 0;
				break;
			case 6:
				type_to_fetch = type_param&PHP_DNS_MX    ? DNS_T_MX    : 0;
				break;
			case 7:
				type_to_fetch = type_param&PHP_DNS_TXT   ? DNS_T_TXT   : 0;
				break;
			case 8:
				type_to_fetch = type_param&PHP_DNS_AAAA	 ? DNS_T_AAAA  : 0;
				break;
			case 9:
				type_to_fetch = type_param&PHP_DNS_SRV   ? DNS_T_SRV   : 0;
				break;
			case 10:
				type_to_fetch = type_param&PHP_DNS_NAPTR ? DNS_T_NAPTR : 0;
				break;
			case 11:
				type_to_fetch = type_param&PHP_DNS_A6	 ? DNS_T_A6 : 0;
				break;
			case PHP_DNS_NUM_TYPES:
				store_results = 0;
				continue;
			default:
			case (PHP_DNS_NUM_TYPES + 1):
				type_to_fetch = DNS_T_ANY;
				break;
		}

		if (type_to_fetch) {
#if defined(HAVE_DNS_SEARCH)
			handle = dns_open(NULL);
			if (handle == NULL) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}
#elif defined(HAVE_RES_NSEARCH)
		    memset(&state, 0, sizeof(state));
		    if (res_ninit(handle)) {
		    	zval_dtor(return_value);
				RETURN_FALSE;
			}
#else
			res_init();
#endif

			n = php_dns_search(handle, hostname, C_IN, type_to_fetch, answer.qb2, sizeof answer);

			if (n < 0) {
				php_dns_free_handle(handle);
				continue;
			}

			cp = answer.qb2 + HFIXEDSZ;
			end = answer.qb2 + n;
			hp = (HEADER *)&answer;
			qd = ntohs(hp->qdcount);
			an = ntohs(hp->ancount);
			ns = ntohs(hp->nscount);
			ar = ntohs(hp->arcount);

			/* Skip QD entries, they're only used by dn_expand later on */
			while (qd-- > 0) {
				n = dn_skipname(cp, end);
				if (n < 0) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse DNS data received");
					zval_dtor(return_value);
					php_dns_free_handle(handle);
					RETURN_FALSE;
				}
				cp += n + QFIXEDSZ;
			}

			/* YAY! Our real answers! */
			while (an-- && cp && cp < end) {
				zval *retval;

				cp = php_parserr(cp, &answer, type_to_fetch, store_results, raw, &retval);
				if (retval != NULL && store_results) {
					add_next_index_zval(return_value, retval);
				}
			}

			if (authns || addtl) {
				/* List of Authoritative Name Servers
				 * Process when only requesting addtl so that we can skip through the section
				 */
				while (ns-- > 0 && cp && cp < end) {
					zval *retval = NULL;

					cp = php_parserr(cp, &answer, DNS_T_ANY, authns != NULL, raw, &retval);
					if (retval != NULL) {
						add_next_index_zval(authns, retval);
					}
				}
			}

			if (addtl) {
				/* Additional records associated with authoritative name servers */
				while (ar-- > 0 && cp && cp < end) {
					zval *retval = NULL;

					cp = php_parserr(cp, &answer, DNS_T_ANY, 1, raw, &retval);
					if (retval != NULL) {
						add_next_index_zval(addtl, retval);
					}
				}
			}
			php_dns_free_handle(handle);
		}
	}
}
/* }}} */

/* {{{ proto bool dns_get_mx(string hostname, array mxhosts [, array weight])
   Get MX records corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_mx)
{
	char *hostname;
	int hostname_len;
	zval *mx_list, *weight_list = NULL;
	int count, qdc;
	u_short type, weight;
	u_char ans[MAXPACKET];
	char buf[MAXHOSTNAMELEN];
	HEADER *hp;
	u_char *cp, *end;
	int i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &hostname, &hostname_len, &mx_list, &weight_list) == FAILURE) {
		return;
	}

	zval_dtor(mx_list);
	array_init(mx_list);

	if (weight_list) {
		zval_dtor(weight_list);
		array_init(weight_list);
	}

#if defined(HAVE_DNS_SEARCH)
	handle = dns_open(NULL);
	if (handle == NULL) {
		RETURN_FALSE;
	}
#elif defined(HAVE_RES_NSEARCH)
    memset(&state, 0, sizeof(state));
    if (res_ninit(handle)) {
			RETURN_FALSE;
	}
#else
	res_init();
#endif

	i = php_dns_search(handle, hostname, C_IN, DNS_T_MX, (u_char *)&ans, sizeof(ans));
	if (i < 0) {
		RETURN_FALSE;
	}
	if (i > (int)sizeof(ans)) {
		i = sizeof(ans);
	}
	hp = (HEADER *)&ans;
	cp = (u_char *)&ans + HFIXEDSZ;
	end = (u_char *)&ans +i;
	for (qdc = ntohs((unsigned short)hp->qdcount); qdc--; cp += i + QFIXEDSZ) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
	}
	count = ntohs((unsigned short)hp->ancount);
	while (--count >= 0 && cp < end) {
		if ((i = dn_skipname(cp, end)) < 0 ) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
		cp += i;
		GETSHORT(type, cp);
		cp += INT16SZ + INT32SZ;
		GETSHORT(i, cp);
		if (type != DNS_T_MX) {
			cp += i;
			continue;
		}
		GETSHORT(weight, cp);
		if ((i = dn_expand(ans, end, cp, buf, sizeof(buf)-1)) < 0) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
		cp += i;
		add_next_index_string(mx_list, buf, 1);
		if (weight_list) {
			add_next_index_long(weight_list, weight);
		}
	}
	php_dns_free_handle(handle);
	RETURN_TRUE;
}
/* }}} */
#endif /* HAVE_FULL_DNS_FUNCS */
#endif /* !defined(PHP_WIN32) && (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE))) */

#if HAVE_FULL_DNS_FUNCS || defined(PHP_WIN32)
PHP_MINIT_FUNCTION(dns) {
	REGISTER_LONG_CONSTANT("DNS_A",     PHP_DNS_A,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_NS",    PHP_DNS_NS,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_CNAME", PHP_DNS_CNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_SOA",   PHP_DNS_SOA,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_PTR",   PHP_DNS_PTR,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_HINFO", PHP_DNS_HINFO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_MX",    PHP_DNS_MX,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_TXT",   PHP_DNS_TXT,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_SRV",   PHP_DNS_SRV,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_NAPTR", PHP_DNS_NAPTR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_AAAA",  PHP_DNS_AAAA,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_A6",    PHP_DNS_A6,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_ANY",   PHP_DNS_ANY,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DNS_ALL",   PHP_DNS_ALL,   CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
#endif /* HAVE_FULL_DNS_FUNCS */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
