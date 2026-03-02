/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes    <cataphract@php.net>                       |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SENDRECVMSG_H
#define PHP_SENDRECVMSG_H 1

#include <php.h>
#include "conversions.h"

/* for sockets.c */

#ifdef PHP_WIN32
#define IPV6_RECVPKTINFO	IPV6_PKTINFO
#define IPV6_RECVHOPLIMIT	IPV6_HOPLIMIT
#endif

void php_socket_sendrecvmsg_init(INIT_FUNC_ARGS);
void php_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS);

int php_do_setsockopt_ipv6_rfc3542(php_socket *php_sock, int level, int optname, zval *arg4);
int php_do_getsockopt_ipv6_rfc3542(php_socket *php_sock, int level, int optname, zval *result);

/* for conversions.c */
typedef struct {
	int	cmsg_level;	/* originating protocol */
	int	cmsg_type;	/* protocol-specific type */
} anc_reg_key;

typedef size_t (calculate_req_space)(const zval *value, ser_context *ctx);

typedef struct {
	socklen_t size; /* size of native structure */
	socklen_t var_el_size; /* size of repeatable component */
	calculate_req_space *calc_space;
	from_zval_write_field *from_array;
	to_zval_read_field *to_array;
} ancillary_reg_entry;

ancillary_reg_entry *get_ancillary_reg_entry(int cmsg_level, int msg_type);

#endif
