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
   | Authors: Gustavo Lopes    <cataphract@php.net>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_SOCKETS

#include "php_network.h"
#ifdef PHP_WIN32
# include "win32/inet.h"
# include <winsock2.h>
# include <windows.h>
# include <Ws2tcpip.h>
# include <Ws2ipdef.h>
# include "php_sockets.h"
# include "win32/sockets.h"
# define NTDDI_XP NTDDI_WINXP /* bug in SDK */
# include <IPHlpApi.h>
# undef NTDDI_XP
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "php_sockets.h"
#include "multicast.h"
#include "main/php_network.h"


enum source_op {
	JOIN_SOURCE,
	LEAVE_SOURCE,
	BLOCK_SOURCE,
	UNBLOCK_SOURCE
};

static int _php_mcast_join_leave(php_socket *sock, int level, struct sockaddr *group, socklen_t group_len, unsigned int if_index, int join TSRMLS_DC);
#ifdef HAS_MCAST_EXT
static int _php_mcast_source_op(php_socket *sock, int level, struct sockaddr *group, socklen_t group_len, struct sockaddr *source, socklen_t source_len, unsigned int if_index, enum source_op sop TSRMLS_DC);
#endif

#ifdef RFC3678_API
static int _php_source_op_to_rfc3678_op(enum source_op sop);
#elif HAS_MCAST_EXT
static const char *_php_source_op_to_string(enum source_op sop);
static int _php_source_op_to_ipv4_op(enum source_op sop);
#endif

int php_mcast_join(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_join_leave(sock, level, group, group_len, if_index, 1 TSRMLS_CC);
}

int php_mcast_leave(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_join_leave(sock, level, group, group_len, if_index, 0 TSRMLS_CC);
}

#ifdef HAS_MCAST_EXT
int php_mcast_join_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_source_op(sock, level, group, group_len, source, source_len, if_index, JOIN_SOURCE TSRMLS_CC);
}

int php_mcast_leave_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_source_op(sock, level, group, group_len, source, source_len, if_index, LEAVE_SOURCE TSRMLS_CC);
}

int php_mcast_block_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_source_op(sock, level, group, group_len, source, source_len, if_index, BLOCK_SOURCE TSRMLS_CC);
}

int php_mcast_unblock_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC)
{
	return _php_mcast_source_op(sock, level, group, group_len, source, source_len, if_index, UNBLOCK_SOURCE TSRMLS_CC);
}
#endif /* HAS_MCAST_EXT */


static int _php_mcast_join_leave(
	php_socket *sock,
	int level,
	struct sockaddr *group, /* struct sockaddr_in/sockaddr_in6 */
	socklen_t group_len,
	unsigned int if_index,
	int join TSRMLS_DC)
{
#ifdef RFC3678_API
	struct group_req greq = {0};
	
	memcpy(&greq.gr_group, group, group_len);
	assert(greq.gr_group.ss_family != 0); /* the caller has set this */
	greq.gr_interface = if_index;

	return setsockopt(sock->bsd_socket, level,
			join ? MCAST_JOIN_GROUP : MCAST_LEAVE_GROUP, (char*)&greq,
			sizeof(greq));	
#else
	if (sock->type == AF_INET) {
		struct ip_mreq mreq = {0};
		struct in_addr addr;
		
		assert(group_len == sizeof(struct sockaddr_in));
		
		if (if_index != 0) {
			if (php_if_index_to_addr4(if_index, sock, &addr TSRMLS_CC) ==
					FAILURE)
				return -2; /* failure, but notice already emitted */
			mreq.imr_interface = addr;
		} else {
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		}
		mreq.imr_multiaddr = ((struct sockaddr_in*)group)->sin_addr;
		return setsockopt(sock->bsd_socket, level,
				join ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, (char*)&mreq,
				sizeof(mreq));
	}
#if HAVE_IPV6
	else if (sock->type == AF_INET6) {
		struct ipv6_mreq mreq = {0};
		
		assert(group_len == sizeof(struct sockaddr_in6));

		mreq.ipv6mr_multiaddr = ((struct sockaddr_in6*)group)->sin6_addr;
		mreq.ipv6mr_interface = if_index;
		
		return setsockopt(sock->bsd_socket, level,
				join ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, (char*)&mreq,
				sizeof(mreq));
	}
#endif
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Option %s is inapplicable to this socket type",
			join ? "MCAST_JOIN_GROUP" : "MCAST_LEAVE_GROUP");
		return -2;
	}
#endif
}

#ifdef HAS_MCAST_EXT
static int _php_mcast_source_op(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index,
	enum source_op sop TSRMLS_DC)
{
#ifdef RFC3678_API
	struct group_source_req gsreq = {0};
	
	memcpy(&gsreq.gsr_group, group, group_len);
	assert(gsreq.gsr_group.ss_family != 0);
	memcpy(&gsreq.gsr_source, source, source_len);
	assert(gsreq.gsr_source.ss_family != 0);
	gsreq.gsr_interface = if_index;
	
	return setsockopt(sock->bsd_socket, level,
			_php_source_op_to_rfc3678_op(sop), (char*)&gsreq, sizeof(gsreq));
#else
	if (sock->type == AF_INET) {
		struct ip_mreq_source mreqs = {0};
		struct in_addr addr;
		
		mreqs.imr_multiaddr = ((struct sockaddr_in*)group)->sin_addr;
		mreqs.imr_sourceaddr =  ((struct sockaddr_in*)source)->sin_addr;
		
		assert(group_len == sizeof(struct sockaddr_in));
		assert(source_len == sizeof(struct sockaddr_in));
		
		if (if_index != 0) {
			if (php_if_index_to_addr4(if_index, sock, &addr TSRMLS_CC) ==
					FAILURE)
				return -2; /* failure, but notice already emitted */
			mreqs.imr_interface = addr;
		} else {
			mreqs.imr_interface.s_addr = htonl(INADDR_ANY);
		}
		
		return setsockopt(sock->bsd_socket, level,
				_php_source_op_to_ipv4_op(sop), (char*)&mreqs, sizeof(mreqs));
	}
#if HAVE_IPV6
	else if (sock->type == AF_INET6) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"This platform does not support %s for IPv6 sockets",
			_php_source_op_to_string(sop));
		return -2;
	}
#endif
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Option %s is inapplicable to this socket type",
			_php_source_op_to_string(sop));
		return -2;
	}
#endif
}

#if RFC3678_API
static int _php_source_op_to_rfc3678_op(enum source_op sop)
{
	switch (sop) {
	case JOIN_SOURCE:
		return MCAST_JOIN_SOURCE_GROUP;
	case LEAVE_SOURCE:
		return MCAST_LEAVE_SOURCE_GROUP;
	case BLOCK_SOURCE:
		return MCAST_BLOCK_SOURCE;
	case UNBLOCK_SOURCE:
		return MCAST_UNBLOCK_SOURCE;
	}
	
	assert(0);
	return 0;
}
#else
static const char *_php_source_op_to_string(enum source_op sop)
{
	switch (sop) {
	case JOIN_SOURCE:
		return "MCAST_JOIN_SOURCE_GROUP";
	case LEAVE_SOURCE:
		return "MCAST_LEAVE_SOURCE_GROUP";
	case BLOCK_SOURCE:
		return "MCAST_BLOCK_SOURCE";
	case UNBLOCK_SOURCE:
		return "MCAST_UNBLOCK_SOURCE";
	}
	
	assert(0);
	return "";
}

static int _php_source_op_to_ipv4_op(enum source_op sop)
{
	switch (sop) {
	case JOIN_SOURCE:
		return IP_ADD_SOURCE_MEMBERSHIP;
	case LEAVE_SOURCE:
		return IP_DROP_SOURCE_MEMBERSHIP;
	case BLOCK_SOURCE:
		return IP_BLOCK_SOURCE;
	case UNBLOCK_SOURCE:
		return IP_UNBLOCK_SOURCE;
	}
	
	assert(0);
	return 0;
}
#endif

#endif /* HAS_MCAST_EXT */

#if PHP_WIN32
int php_if_index_to_addr4(unsigned if_index, php_socket *php_sock, struct in_addr *out_addr TSRMLS_DC)
{
	MIB_IPADDRTABLE *addr_table;
    ULONG size;
    DWORD retval;
	DWORD i;

	(void) php_sock; /* not necessary */

	if (if_index == 0) {
		out_addr->s_addr = INADDR_ANY;
		return SUCCESS;
	}

	size = 4 * (sizeof *addr_table);
	addr_table = emalloc(size);
retry:
	retval = GetIpAddrTable(addr_table, &size, 0);
	if (retval == ERROR_INSUFFICIENT_BUFFER) {
		efree(addr_table);
		addr_table = emalloc(size);
		goto retry;
	}
	if (retval != NO_ERROR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"GetIpAddrTable failed with error %lu", retval);
		return FAILURE;
	}
	for (i = 0; i < addr_table->dwNumEntries; i++) {
		MIB_IPADDRROW r = addr_table->table[i];
		if (r.dwIndex == if_index) {
			out_addr->s_addr = r.dwAddr;
			return SUCCESS;
		}
	}
	php_error_docref(NULL TSRMLS_CC, E_WARNING,
		"No interface with index %u was found", if_index);
	return FAILURE;
}

int php_add4_to_if_index(struct in_addr *addr, php_socket *php_sock, unsigned *if_index TSRMLS_DC)
{
	MIB_IPADDRTABLE *addr_table;
    ULONG size;
    DWORD retval;
	DWORD i;

	(void) php_sock; /* not necessary */

	if (addr->s_addr == INADDR_ANY) {
		*if_index = 0;
		return SUCCESS;
	}

	size = 4 * (sizeof *addr_table);
	addr_table = emalloc(size);
retry:
	retval = GetIpAddrTable(addr_table, &size, 0);
	if (retval == ERROR_INSUFFICIENT_BUFFER) {
		efree(addr_table);
		addr_table = emalloc(size);
		goto retry;
	}
	if (retval != NO_ERROR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"GetIpAddrTable failed with error %lu", retval);
		return FAILURE;
	}
	for (i = 0; i < addr_table->dwNumEntries; i++) {
		MIB_IPADDRROW r = addr_table->table[i];
		if (r.dwAddr == addr->s_addr) {
			*if_index = r.dwIndex;
			return SUCCESS;
		}
	}

	{
		char addr_str[17] = {0};
		inet_ntop(AF_INET, addr, addr_str, sizeof(addr_str));
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"The interface with IP address %s was not found", addr_str);
	}
	return FAILURE;
}

#else

int php_if_index_to_addr4(unsigned if_index, php_socket *php_sock, struct in_addr *out_addr TSRMLS_DC)
{
	struct ifreq if_req;
	
	if (if_index == 0) {
		out_addr->s_addr = INADDR_ANY;
		return SUCCESS;
	}
	
#if !defined(ifr_ifindex) && defined(ifr_index)
#define ifr_ifindex ifr_index
#endif
	
#if defined(SIOCGIFNAME)
	if_req.ifr_ifindex = if_index;
	if (ioctl(php_sock->bsd_socket, SIOCGIFNAME, &if_req) == -1) {
#elif defined(HAVE_IF_INDEXTONAME)
	if (if_indextoname(if_index, if_req.ifr_name) == NULL) {
#else
#error Neither SIOCGIFNAME nor if_indextoname are available
#endif
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Failed obtaining address for interface %u: error %d", if_index, errno);
		return FAILURE;
	}
	
	if (ioctl(php_sock->bsd_socket, SIOCGIFADDR, &if_req) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Failed obtaining address for interface %u: error %d", if_index, errno);
		return FAILURE;
	}
	
	memcpy(out_addr, &((struct sockaddr_in *) &if_req.ifr_addr)->sin_addr,
		sizeof *out_addr);
	return SUCCESS;
}

int php_add4_to_if_index(struct in_addr *addr, php_socket *php_sock, unsigned *if_index TSRMLS_DC)
{
	struct ifconf	if_conf = {0};
	char			*buf = NULL,
					*p;
	int				size = 0,
					lastsize = 0;
	size_t			entry_len;
	
	if (addr->s_addr == INADDR_ANY) {
		*if_index = 0;
		return SUCCESS;
	}
	
	for(;;) {
		size += 5 * sizeof(struct ifreq);
		buf = ecalloc(size, 1);
		if_conf.ifc_len = size;
		if_conf.ifc_buf = buf;
		
		if (ioctl(php_sock->bsd_socket, SIOCGIFCONF, (char*)&if_conf) == -1 &&
				(errno != EINVAL || lastsize != 0)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Failed obtaining interfaces list: error %d", errno);
			goto err;
		}
		
		if (if_conf.ifc_len == lastsize)
			/* not increasing anymore */
			break;
		else {
			lastsize = if_conf.ifc_len;
			efree(buf);
			buf = NULL;
		}
	}
	
	for (p = if_conf.ifc_buf;
		 p < if_conf.ifc_buf + if_conf.ifc_len;
		 p += entry_len) {
		struct ifreq *cur_req;
		
		/* let's hope the pointer is aligned */
		cur_req = (struct ifreq*) p;
		
#ifdef HAVE_SOCKADDR_SA_LEN
		entry_len = cur_req->ifr_addr.sa_len + sizeof(cur_req->ifr_name);
#else
		/* if there's no sa_len, assume the ifr_addr field is a sockaddr */
		entry_len = sizeof(struct sockaddr) + sizeof(cur_req->ifr_name);
#endif
		entry_len = MAX(entry_len, sizeof(*cur_req));
		
		if ((((struct sockaddr*)&cur_req->ifr_addr)->sa_family == AF_INET) &&
				(((struct sockaddr_in*)&cur_req->ifr_addr)->sin_addr.s_addr ==
					addr->s_addr)) {
#if defined(SIOCGIFINDEX)
			if (ioctl(php_sock->bsd_socket, SIOCGIFINDEX, (char*)cur_req)
					== -1) {
#elif defined(HAVE_IF_NAMETOINDEX)
			unsigned index_tmp;
			if ((index_tmp = if_nametoindex(cur_req->ifr_name)) == 0) {
#else
#error Neither SIOCGIFINDEX nor if_nametoindex are available
#endif
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Error converting interface name to index: error %d",
					errno);
				goto err;
			} else {
#if defined(SIOCGIFINDEX)
				*if_index = cur_req->ifr_ifindex;
#else
				*if_index = index_tmp;
#endif
				efree(buf);
				return SUCCESS;
			}
		}
	}

	{
		char addr_str[17] = {0};
		inet_ntop(AF_INET, addr, addr_str, sizeof(addr_str));
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"The interface with IP address %s was not found", addr_str);
	}
	
err:
	if (buf != NULL)
		efree(buf);
	return FAILURE;
}
#endif

#endif /* HAVE_SOCKETS */
