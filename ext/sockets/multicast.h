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

#if defined(MCAST_JOIN_GROUP) && \
	(!defined(PHP_WIN32) || (_WIN32_WINNT >= 0x600 && SOCKETS_ENABLE_VISTA_API)) && \
	!defined(__APPLE__)
#define RFC3678_API 1
/* has block/unblock and source membership, in this case for both IPv4 and IPv6 */
#define HAS_MCAST_EXT 1
#elif defined(IP_ADD_SOURCE_MEMBERSHIP) && !defined(__APPLE__)
/* has block/unblock and source membership, but only for IPv4 */
#define HAS_MCAST_EXT 1
#endif

int php_if_index_to_addr4(
        unsigned if_index,
        php_socket *php_sock,
        struct in_addr *out_addr TSRMLS_DC);

int php_add4_to_if_index(
        struct in_addr *addr,
        php_socket *php_sock,
        unsigned *if_index TSRMLS_DC);

int php_mcast_join(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC);

int php_mcast_leave(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC);

#ifdef HAS_MCAST_EXT
int php_mcast_join_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int php_mcast_leave_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int php_mcast_block_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int php_mcast_unblock_source(
	php_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);
#endif
