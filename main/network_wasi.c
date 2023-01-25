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
 */

#include "php.h"

PHPAPI int php_network_parse_network_address_with_port(const char *addr, zend_long addrlen, struct sockaddr *sa, socklen_t *sl)
{
	return FAILURE;
}

PHPAPI int php_set_sock_blocking(int socketd, int block)
{
	return FAILURE;
}

PHPAPI char *php_socket_strerror(long err, char *buf, size_t bufsize)
{
	return NULL;
}

PHPAPI void _php_emit_fd_setsize_warning(int max_fd)
{
}
