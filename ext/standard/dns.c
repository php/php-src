/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: The typical suspects                                        |
   |          Pollita <pollita@php.net>                                   |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
#include "php.h"
#include "php_network.h"

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef PHP_WIN32
# include "win32/inet.h"
# include <winsock2.h>
# include <windows.h>
# include <Ws2tcpip.h>
#else
#include <netinet/in.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <netdb.h>
#ifdef _OSD_POSIX
#undef STATUS
#undef T_UNSPEC
#endif
#ifdef HAVE_ARPA_NAMESER_H
#ifdef DARWIN
# define BIND_8_COMPAT 1
#endif
#include <arpa/nameser.h>
#endif
#ifdef HAVE_RESOLV_H
#include <resolv.h>
#if defined(__HAIKU__)
extern void __res_ndestroy(res_state statp);
#define res_ndestroy __res_ndestroy
#endif
#endif
#ifdef HAVE_DNS_H
#include <dns.h>
#endif
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
#ifndef DNS_T_CAA
#define DNS_T_CAA	257
#endif

#ifndef DNS_T_ANY
#define DNS_T_ANY	255
#endif
/* }}} */

static zend_string *php_gethostbyaddr(char *ip);
static zend_string *php_gethostbyname(char *name);

#ifdef HAVE_GETHOSTNAME
/* {{{ Get the host name of the current machine */
PHP_FUNCTION(gethostname)
{
	char buf[HOST_NAME_MAX + 1];

	ZEND_PARSE_PARAMETERS_NONE();

	if (gethostname(buf, sizeof(buf))) {
		php_error_docref(NULL, E_WARNING, "Unable to fetch host [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_STRING(buf);
}
/* }}} */
#endif

/* TODO: Reimplement the gethostby* functions using the new winxp+ API, in dns_win32.c, then
 we can have a dns.c, dns_unix.c and dns_win32.c instead of a messy dns.c full of #ifdef
*/

/* {{{ Get the Internet host name corresponding to a given IP address */
PHP_FUNCTION(gethostbyaddr)
{
	char *addr;
	size_t addr_len;
	zend_string *hostname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(addr, addr_len)
	ZEND_PARSE_PARAMETERS_END();

	hostname = php_gethostbyaddr(addr);

	if (hostname == NULL) {
#if defined(HAVE_IPV6) && defined(HAVE_INET_PTON)
		php_error_docref(NULL, E_WARNING, "Address is not a valid IPv4 or IPv6 address");
#else
		php_error_docref(NULL, E_WARNING, "Address is not in a.b.c.d form");
#endif
		RETVAL_FALSE;
	} else {
		RETVAL_STR(hostname);
	}
}
/* }}} */

/* {{{ php_gethostbyaddr */
static zend_string *php_gethostbyaddr(char *ip)
{
#if defined(HAVE_IPV6) && defined(HAVE_INET_PTON)
	struct sockaddr_in sa4;
	struct sockaddr_in6 sa6;
	char out[NI_MAXHOST];
	memset(&sa4, 0, sizeof(struct sockaddr_in));
	memset(&sa6, 0, sizeof(struct sockaddr_in6));

	if (inet_pton(AF_INET6, ip, &sa6.sin6_addr)) {
		sa6.sin6_family = AF_INET6;

		if (getnameinfo((struct sockaddr *)&sa6, sizeof(sa6), out, sizeof(out), NULL, 0, NI_NAMEREQD) != 0) {
			return zend_string_init(ip, strlen(ip), 0);
		}
		return zend_string_init(out, strlen(out), 0);
	} else if (inet_pton(AF_INET, ip, &sa4.sin_addr)) {
		sa4.sin_family = AF_INET;

		if (getnameinfo((struct sockaddr *)&sa4, sizeof(sa4), out, sizeof(out), NULL, 0, NI_NAMEREQD) != 0) {
			return zend_string_init(ip, strlen(ip), 0);
		}
		return zend_string_init(out, strlen(out), 0);
	}
	return NULL; /* not a valid IP */
#else
	struct in_addr addr;
	struct hostent *hp;

	addr.s_addr = inet_addr(ip);

	if (addr.s_addr == -1) {
		return NULL;
	}

	hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);

	if (!hp || hp->h_name == NULL || hp->h_name[0] == '\0') {
		return zend_string_init(ip, strlen(ip), 0);
	}

	return zend_string_init(hp->h_name, strlen(hp->h_name), 0);
#endif
}
/* }}} */

/* {{{ Get the IP address corresponding to a given Internet host name */
PHP_FUNCTION(gethostbyname)
{
	char *hostname;
	size_t hostname_len;
	zend_string *ipaddr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(hostname, hostname_len)
	ZEND_PARSE_PARAMETERS_END();

	if (hostname_len > MAXFQDNLEN) {
		/* name too long, protect from CVE-2015-0235 */
		php_error_docref(NULL, E_WARNING, "Host name cannot be longer than %d characters", MAXFQDNLEN);
		RETURN_STRINGL(hostname, hostname_len);
	}

	if (!(ipaddr = php_gethostbyname(hostname))) {
		php_error_docref(NULL, E_WARNING, "Host name to ip failed %s", hostname);
		RETURN_STRINGL(hostname, hostname_len);
	} else {
		RETURN_STR(ipaddr);
	}
}
/* }}} */

/* {{{ Return a list of IP addresses that a given hostname resolves to. */
PHP_FUNCTION(gethostbynamel)
{
	char *hostname;
	size_t hostname_len;
	struct hostent *hp;
	struct in_addr in;
	int i;
	char addr4[INET_ADDRSTRLEN];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(hostname, hostname_len)
	ZEND_PARSE_PARAMETERS_END();

	if (hostname_len > MAXFQDNLEN) {
		/* name too long, protect from CVE-2015-0235 */
		php_error_docref(NULL, E_WARNING, "Host name cannot be longer than %d characters", MAXFQDNLEN);
		RETURN_FALSE;
	}

	hp = php_network_gethostbyname(hostname);
	if (!hp) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (i = 0;; i++) {
		/* On macos h_addr_list entries may be misaligned. */
		const char *ipaddr;
		struct in_addr *h_addr_entry; /* Don't call this h_addr, it's a macro! */
		memcpy(&h_addr_entry, &hp->h_addr_list[i], sizeof(struct in_addr *));
		if (!h_addr_entry) {
			return;
		}

		in = *h_addr_entry;
		if (!(ipaddr = inet_ntop(AF_INET, &in, addr4, INET_ADDRSTRLEN))) {
			/* unlikely regarding (too) long hostname and protocols but checking still */
			php_error_docref(NULL, E_WARNING, "Host name to ip failed %s", hostname);
			continue;
		} else {
			add_next_index_string(return_value, ipaddr);
		}
	}
}
/* }}} */

/* {{{ php_gethostbyname */
static zend_string *php_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr *h_addr_0; /* Don't call this h_addr, it's a macro! */
	struct in_addr in;
	char addr4[INET_ADDRSTRLEN];
	const char *address;

	hp = php_network_gethostbyname(name);
	if (!hp) {
		return zend_string_init(name, strlen(name), 0);
	}

	/* On macos h_addr_list entries may be misaligned. */
	memcpy(&h_addr_0, &hp->h_addr_list[0], sizeof(struct in_addr *));
	if (!h_addr_0) {
		return zend_string_init(name, strlen(name), 0);
	}

	memcpy(&in.s_addr, h_addr_0, sizeof(in.s_addr));

	if (!(address = inet_ntop(AF_INET, &in, addr4, INET_ADDRSTRLEN))) {
		return NULL;
	}

	return zend_string_init(address, strlen(address), 0);
}
/* }}} */

/* Note: These functions are defined in ext/standard/dns_win32.c for Windows! */
#if !defined(PHP_WIN32) && defined(HAVE_DNS_SEARCH_FUNC)

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
	uint8_t qb2[65536];
} querybuf;

/* just a hack to free resources allocated by glibc in __res_nsend()
 * See also:
 *   res_thread_freeres() in glibc/resolv/res_init.c
 *   __libc_res_nsend()   in resolv/res_send.c
 * */

#if defined(__GLIBC__) && !defined(HAVE_DEPRECATED_DNS_FUNCS)
#define php_dns_free_res(__res__) _php_dns_free_res(__res__)
static void _php_dns_free_res(struct __res_state *res) { /* {{{ */
	int ns;
	for (ns = 0; ns < MAXNS; ns++) {
		if (res->_u._ext.nsaddrs[ns] != NULL) {
			free (res->_u._ext.nsaddrs[ns]);
			res->_u._ext.nsaddrs[ns] = NULL;
		}
	}
} /* }}} */
#else
#define php_dns_free_res(__res__)
#endif

/* {{{ Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(dns_check_record)
{
	HEADER *hp;
	querybuf answer = {0};
	char *hostname;
	size_t hostname_len;
	zend_string *rectype = NULL;
	int type = DNS_T_MX, i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(hostname, hostname_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(rectype)
	ZEND_PARSE_PARAMETERS_END();

	if (hostname_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (rectype) {
		if (zend_string_equals_literal_ci(rectype, "A")) type = DNS_T_A;
		else if (zend_string_equals_literal_ci(rectype, "NS")) type = DNS_T_NS;
		else if (zend_string_equals_literal_ci(rectype, "MX")) type = DNS_T_MX;
		else if (zend_string_equals_literal_ci(rectype, "PTR")) type = DNS_T_PTR;
		else if (zend_string_equals_literal_ci(rectype, "ANY")) type = DNS_T_ANY;
		else if (zend_string_equals_literal_ci(rectype, "SOA")) type = DNS_T_SOA;
		else if (zend_string_equals_literal_ci(rectype, "CAA")) type = DNS_T_CAA;
		else if (zend_string_equals_literal_ci(rectype, "TXT")) type = DNS_T_TXT;
		else if (zend_string_equals_literal_ci(rectype, "CNAME")) type = DNS_T_CNAME;
		else if (zend_string_equals_literal_ci(rectype, "AAAA")) type = DNS_T_AAAA;
		else if (zend_string_equals_literal_ci(rectype, "SRV")) type = DNS_T_SRV;
		else if (zend_string_equals_literal_ci(rectype, "NAPTR")) type = DNS_T_NAPTR;
		else if (zend_string_equals_literal_ci(rectype, "A6")) type = DNS_T_A6;
		else {
			zend_argument_value_error(2, "must be a valid DNS record type");
			RETURN_THROWS();
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

	i = php_dns_search(handle, hostname, C_IN, type, answer.qb2, sizeof answer);
	php_dns_free_handle(handle);

	if (i < 0) {
		RETURN_FALSE;
	}
	hp = (HEADER *)&answer;
	RETURN_BOOL(ntohs(hp->ancount) != 0);
}
/* }}} */

#ifdef HAVE_FULL_DNS_FUNCS

#define CHECKCP(n) do { \
	if (cp + n > end) { \
		return NULL; \
	} \
} while (0)

/* {{{ php_parserr */
static uint8_t *php_parserr(uint8_t *cp, uint8_t *end, querybuf *answer, int type_to_fetch, int store, bool raw, zval *subarray)
{
	u_short type, class, dlen;
	u_long ttl;
	long n, i;
	u_short s;
	uint8_t *tp, *p;
	char name[MAXHOSTNAMELEN] = {0};
	int have_v6_break = 0, in_v6_break = 0;

	ZVAL_UNDEF(subarray);

	n = dn_expand(answer->qb2, end, cp, name, sizeof(name) - 2);
	if (n < 0) {
		return NULL;
	}
	cp += n;

	CHECKCP(10);
	GETSHORT(type, cp);
	GETSHORT(class, cp);
	GETLONG(ttl, cp);
	GETSHORT(dlen, cp);
	CHECKCP(dlen);
	if (dlen == 0) {
		/* No data in the response - nothing to do */
		return NULL;
	}
	if (type_to_fetch != DNS_T_ANY && type != type_to_fetch) {
		cp += dlen;
		return cp;
	}

	if (!store) {
		cp += dlen;
		return cp;
	}

	array_init(subarray);

	add_assoc_string(subarray, "host", name);
	add_assoc_string(subarray, "class", "IN");
	add_assoc_long(subarray, "ttl", ttl);
	(void) class;

	if (raw) {
		add_assoc_long(subarray, "type", type);
		add_assoc_stringl(subarray, "data", (char*) cp, (uint32_t) dlen);
		cp += dlen;
		return cp;
	}

	switch (type) {
		case DNS_T_A:
			CHECKCP(4);
			add_assoc_string(subarray, "type", "A");
			snprintf(name, sizeof(name), "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
			add_assoc_string(subarray, "ip", name);
			cp += dlen;
			break;
		case DNS_T_MX:
			CHECKCP(2);
			add_assoc_string(subarray, "type", "MX");
			GETSHORT(n, cp);
			add_assoc_long(subarray, "pri", n);
			ZEND_FALLTHROUGH;
		case DNS_T_CNAME:
			if (type == DNS_T_CNAME) {
				add_assoc_string(subarray, "type", "CNAME");
			}
			ZEND_FALLTHROUGH;
		case DNS_T_NS:
			if (type == DNS_T_NS) {
				add_assoc_string(subarray, "type", "NS");
			}
			ZEND_FALLTHROUGH;
		case DNS_T_PTR:
			if (type == DNS_T_PTR) {
				add_assoc_string(subarray, "type", "PTR");
			}
			n = dn_expand(answer->qb2, end, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(subarray, "target", name);
			break;
		case DNS_T_HINFO:
			/* See RFC 1010 for values */
			add_assoc_string(subarray, "type", "HINFO");
			CHECKCP(1);
			n = *cp & 0xFF;
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "cpu", (char*)cp, n);
			cp += n;
			CHECKCP(1);
			n = *cp & 0xFF;
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "os", (char*)cp, n);
			cp += n;
			break;
		case DNS_T_CAA:
			/* See RFC 6844 for values https://tools.ietf.org/html/rfc6844 */
			add_assoc_string(subarray, "type", "CAA");
			// 1 flag byte
			CHECKCP(1);
			n = *cp & 0xFF;
			add_assoc_long(subarray, "flags", n);
			cp++;
			// Tag length (1 byte)
			CHECKCP(1);
			n = *cp & 0xFF;
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "tag", (char*)cp, n);
			cp += n;
			if ( (size_t) dlen < ((size_t)n) + 2 ) {
				return NULL;
			}
			n = dlen - n - 2;
			CHECKCP(n);
			add_assoc_stringl(subarray, "value", (char*)cp, n);
			cp += n;
			break;
		case DNS_T_TXT:
			{
				int l1 = 0, l2 = 0;
				zval entries;
				zend_string *tp;

				add_assoc_string(subarray, "type", "TXT");
				tp = zend_string_alloc(dlen, 0);

				array_init(&entries);

				while (l1 < dlen) {
					n = cp[l1];
					if ((l1 + n) >= dlen) {
						// Invalid chunk length, truncate
						n = dlen - (l1 + 1);
					}
					if (n) {
						memcpy(ZSTR_VAL(tp) + l2 , cp + l1 + 1, n);
						add_next_index_stringl(&entries, (char *) cp + l1 + 1, n);
					}
					l1 = l1 + n + 1;
					l2 = l2 + n;
				}
				ZSTR_VAL(tp)[l2] = '\0';
				ZSTR_LEN(tp) = l2;
				cp += dlen;

				add_assoc_str(subarray, "txt", tp);
				add_assoc_zval(subarray, "entries", &entries);
			}
			break;
		case DNS_T_SOA:
			add_assoc_string(subarray, "type", "SOA");
			n = dn_expand(answer->qb2, end, cp, name, (sizeof name) -2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(subarray, "mname", name);
			n = dn_expand(answer->qb2, end, cp, name, (sizeof name) -2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(subarray, "rname", name);
			CHECKCP(5*4);
			GETLONG(n, cp);
			add_assoc_long(subarray, "serial", n);
			GETLONG(n, cp);
			add_assoc_long(subarray, "refresh", n);
			GETLONG(n, cp);
			add_assoc_long(subarray, "retry", n);
			GETLONG(n, cp);
			add_assoc_long(subarray, "expire", n);
			GETLONG(n, cp);
			add_assoc_long(subarray, "minimum-ttl", n);
			break;
		case DNS_T_AAAA:
			tp = (uint8_t*)name;
			CHECKCP(8*2);
			for(i=0; i < 8; i++) {
				GETSHORT(s, cp);
				if (s != 0) {
					if (tp > (uint8_t *)name) {
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
			add_assoc_string(subarray, "type", "AAAA");
			add_assoc_string(subarray, "ipv6", name);
			break;
		case DNS_T_A6:
			p = cp;
			add_assoc_string(subarray, "type", "A6");
			CHECKCP(1);
			n = ((int)cp[0]) & 0xFF;
			cp++;
			add_assoc_long(subarray, "masklen", n);
			tp = (uint8_t*)name;
			if (n > 15) {
				have_v6_break = 1;
				in_v6_break = 1;
				tp[0] = ':';
				tp++;
			}
			if (n % 16 > 8) {
				/* Partial short */
				if (cp[0] != 0) {
					if (tp > (uint8_t *)name) {
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
				CHECKCP(2);
				GETSHORT(s, cp);
				if (s != 0) {
					if (tp > (uint8_t *)name) {
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
			add_assoc_string(subarray, "ipv6", name);
			if (cp < p + dlen) {
				n = dn_expand(answer->qb2, end, cp, name, (sizeof name) - 2);
				if (n < 0) {
					return NULL;
				}
				cp += n;
				add_assoc_string(subarray, "chain", name);
			}
			break;
		case DNS_T_SRV:
			CHECKCP(3*2);
			add_assoc_string(subarray, "type", "SRV");
			GETSHORT(n, cp);
			add_assoc_long(subarray, "pri", n);
			GETSHORT(n, cp);
			add_assoc_long(subarray, "weight", n);
			GETSHORT(n, cp);
			add_assoc_long(subarray, "port", n);
			n = dn_expand(answer->qb2, end, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(subarray, "target", name);
			break;
		case DNS_T_NAPTR:
			CHECKCP(2*2);
			add_assoc_string(subarray, "type", "NAPTR");
			GETSHORT(n, cp);
			add_assoc_long(subarray, "order", n);
			GETSHORT(n, cp);
			add_assoc_long(subarray, "pref", n);

			CHECKCP(1);
			n = (cp[0] & 0xFF);
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "flags", (char*)cp, n);
			cp += n;

			CHECKCP(1);
			n = (cp[0] & 0xFF);
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "services", (char*)cp, n);
			cp += n;

			CHECKCP(1);
			n = (cp[0] & 0xFF);
			cp++;
			CHECKCP(n);
			add_assoc_stringl(subarray, "regex", (char*)cp, n);
			cp += n;

			n = dn_expand(answer->qb2, end, cp, name, (sizeof name) - 2);
			if (n < 0) {
				return NULL;
			}
			cp += n;
			add_assoc_string(subarray, "replacement", name);
			break;
		default:
			zval_ptr_dtor(subarray);
			ZVAL_UNDEF(subarray);
			cp += dlen;
			break;
	}

	return cp;
}
/* }}} */

/* {{{ Get any Resource Record corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_record)
{
	char *hostname;
	size_t hostname_len;
	zend_long type_param = PHP_DNS_ANY;
	zval *authns = NULL, *addtl = NULL;
	int type_to_fetch;
	int dns_errno;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif
	HEADER *hp;
	querybuf answer = {0};
	uint8_t *cp = NULL, *end = NULL;
	int n, qd, an, ns = 0, ar = 0;
	int type, first_query = 1, store_results = 1;
	bool raw = 0;

	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_STRING(hostname, hostname_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(type_param)
		Z_PARAM_ZVAL(authns)
		Z_PARAM_ZVAL(addtl)
		Z_PARAM_BOOL(raw)
	ZEND_PARSE_PARAMETERS_END();

	if (authns) {
		authns = zend_try_array_init(authns);
		if (!authns) {
			RETURN_THROWS();
		}
	}
	if (addtl) {
		addtl = zend_try_array_init(addtl);
		if (!addtl) {
			RETURN_THROWS();
		}
	}

	if (!raw) {
		if ((type_param & ~PHP_DNS_ALL) && (type_param != PHP_DNS_ANY)) {
			zend_argument_value_error(2, "must be a DNS_* constant");
			RETURN_THROWS();
		}
	} else {
		if ((type_param < 1) || (type_param > 0xFFFF)) {
			zend_argument_value_error(2, "must be between 1 and 65535 when argument #5 ($raw) is true");
			RETURN_THROWS();
		}
	}

	/* Initialize the return array */
	array_init(return_value);

	/* - We emulate an or'ed type mask by querying type by type. (Steps 0 - NUMTYPES-1 )
	 *   If additional info is wanted we check again with DNS_T_ANY (step NUMTYPES / NUMTYPES+1 )
	 *   store_results is used to skip storing the results retrieved in step
	 *   NUMTYPES+1 when results were already fetched.
	 * - In case of PHP_DNS_ANY we use the directly fetch DNS_T_ANY. (step NUMTYPES+1 )
	 * - In case of raw mode, we query only the requested type instead of looping type by type
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
			case 12:
				type_to_fetch = type_param&PHP_DNS_CAA ? DNS_T_CAA : 0;
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
				zend_array_destroy(Z_ARR_P(return_value));
				RETURN_FALSE;
			}
#elif defined(HAVE_RES_NSEARCH)
		    memset(&state, 0, sizeof(state));
		    if (res_ninit(handle)) {
		    	zend_array_destroy(Z_ARR_P(return_value));
				RETURN_FALSE;
			}
#else
			res_init();
#endif

			n = php_dns_search(handle, hostname, C_IN, type_to_fetch, answer.qb2, sizeof answer);

			if (n < 0) {
				dns_errno = php_dns_errno(handle);
				php_dns_free_handle(handle);
				switch (dns_errno) {
					case NO_DATA:
					case HOST_NOT_FOUND:
						continue;

					case NO_RECOVERY:
						php_error_docref(NULL, E_WARNING, "An unexpected server failure occurred.");
						break;

					case TRY_AGAIN:
						php_error_docref(NULL, E_WARNING, "A temporary server error occurred.");
						break;

					default:
						php_error_docref(NULL, E_WARNING, "DNS Query failed");
				}
				zend_array_destroy(Z_ARR_P(return_value));
				RETURN_FALSE;
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
					php_error_docref(NULL, E_WARNING, "Unable to parse DNS data received");
					zend_array_destroy(Z_ARR_P(return_value));
					php_dns_free_handle(handle);
					RETURN_FALSE;
				}
				cp += n + QFIXEDSZ;
			}

			/* YAY! Our real answers! */
			while (an-- && cp && cp < end) {
				zval retval;

				cp = php_parserr(cp, end, &answer, type_to_fetch, store_results, raw, &retval);
				if (Z_TYPE(retval) != IS_UNDEF && store_results) {
					add_next_index_zval(return_value, &retval);
				}
			}

			if (authns || addtl) {
				/* List of Authoritative Name Servers
				 * Process when only requesting addtl so that we can skip through the section
				 */
				while (ns-- > 0 && cp && cp < end) {
					zval retval;

					cp = php_parserr(cp, end, &answer, DNS_T_ANY, authns != NULL, raw, &retval);
					if (Z_TYPE(retval) != IS_UNDEF) {
						add_next_index_zval(authns, &retval);
					}
				}
			}

			if (addtl) {
				/* Additional records associated with authoritative name servers */
				while (ar-- > 0 && cp && cp < end) {
					zval retval;

					cp = php_parserr(cp, end, &answer, DNS_T_ANY, 1, raw, &retval);
					if (Z_TYPE(retval) != IS_UNDEF) {
						add_next_index_zval(addtl, &retval);
					}
				}
			}
			php_dns_free_handle(handle);
		}
	}
}
/* }}} */

/* {{{ Get MX records corresponding to a given Internet host name */
PHP_FUNCTION(dns_get_mx)
{
	char *hostname;
	size_t hostname_len;
	zval *mx_list, *weight_list = NULL;
	int count, qdc;
	u_short type, weight;
	querybuf answer = {0};
	char buf[MAXHOSTNAMELEN] = {0};
	HEADER *hp;
	uint8_t *cp, *end;
	int i;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(hostname, hostname_len)
		Z_PARAM_ZVAL(mx_list)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(weight_list)
	ZEND_PARSE_PARAMETERS_END();

	mx_list = zend_try_array_init(mx_list);
	if (!mx_list) {
		RETURN_THROWS();
	}

	if (weight_list) {
		weight_list = zend_try_array_init(weight_list);
		if (!weight_list) {
			RETURN_THROWS();
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

	i = php_dns_search(handle, hostname, C_IN, DNS_T_MX, answer.qb2, sizeof(answer));
	if (i < 0) {
		php_dns_free_handle(handle);
		RETURN_FALSE;
	}
	hp = (HEADER *)&answer;
	cp = answer.qb2 + HFIXEDSZ;
	end = answer.qb2 + i;
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
		if ((i = dn_expand(answer.qb2, end, cp, buf, sizeof(buf)-1)) < 0) {
			php_dns_free_handle(handle);
			RETURN_FALSE;
		}
		cp += i;
		add_next_index_string(mx_list, buf);
		if (weight_list) {
			add_next_index_long(weight_list, weight);
		}
	}
	php_dns_free_handle(handle);
	RETURN_BOOL(zend_hash_num_elements(Z_ARRVAL_P(mx_list)) != 0);
}
/* }}} */
#endif /* HAVE_FULL_DNS_FUNCS */
#endif /* !defined(PHP_WIN32) && HAVE_DNS_SEARCH_FUNC */
