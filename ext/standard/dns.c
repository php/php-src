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
   | Author:                                                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes
 */
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
#endif
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

/* {{{ php_gethostbyaddr
 */
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

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

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

/* {{{ php_gethostbyname
 */
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

/* {{{ proto int checkdnsrr(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
PHP_FUNCTION(checkdnsrr)
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
		if ( !strcasecmp("A", Z_STRVAL_PP(arg2)) ) type = T_A;
		else if ( !strcasecmp("NS", Z_STRVAL_PP(arg2)) ) type = T_NS;
		else if ( !strcasecmp("MX", Z_STRVAL_PP(arg2)) ) type = T_MX;
		else if ( !strcasecmp("PTR", Z_STRVAL_PP(arg2)) ) type = T_PTR;
		else if ( !strcasecmp("ANY", Z_STRVAL_PP(arg2)) ) type = T_ANY;
		else if ( !strcasecmp("SOA", Z_STRVAL_PP(arg2)) ) type = T_SOA;
		else if ( !strcasecmp("CNAME", Z_STRVAL_PP(arg2)) ) type = T_CNAME;
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%s' not supported", Z_STRVAL_PP(arg2));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	i = res_search(Z_STRVAL_PP(arg1), C_IN, type, ans, sizeof(ans));
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
	i = res_search(Z_STRVAL_P(host), C_IN, T_MX, (u_char *)&ans, sizeof(ans));
	if ( i < 0 ) {
		RETURN_FALSE;
	}
	if ( i > sizeof(ans) ) i = sizeof(ans);
	hp = (HEADER *)&ans;
	cp = (u_char *)&ans + HFIXEDSZ;
	end = (u_char *)&ans +i;
	for ( qdc = ntohs((unsigned short)hp->qdcount); qdc--; cp += i + QFIXEDSZ) {
		if ( (i = dn_skipname(cp, end)) < 0 ) {
			RETURN_FALSE;
		}
	}
	count = ntohs((unsigned short)hp->ancount);
	while ( --count >= 0 && cp < end ) {
		if ( (i = dn_skipname(cp, end)) < 0 ) {
			RETURN_FALSE;
		}
		cp += i;
		GETSHORT(type, cp);
		cp += INT16SZ + INT32SZ;
		GETSHORT(i, cp);
		if ( type != T_MX ) {
			cp += i;
			continue;
		}
		GETSHORT(weight, cp);
		if ( (i = dn_expand(ans, end, cp, buf, sizeof(buf)-1)) < 0 ) {
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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
