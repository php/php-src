/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
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
