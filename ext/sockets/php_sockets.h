/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SOCKETS_H
#define PHP_SOCKETS_H

/* $Id$ */

#if HAVE_SOCKETS

extern zend_module_entry sockets_module_entry;
#define phpext_sockets_ptr &sockets_module_entry

#ifdef PHP_WIN32
#define PHP_SOCKETS_API __declspec(dllexport)
#else
#define PHP_SOCKETS_API
#endif

PHP_MINIT_FUNCTION(sockets);
PHP_MINFO_FUNCTION(sockets);

PHP_FUNCTION(fd_alloc);
PHP_FUNCTION(fd_dealloc);
PHP_FUNCTION(fd_set);
PHP_FUNCTION(fd_isset);
PHP_FUNCTION(fd_clear);
PHP_FUNCTION(fd_zero);
PHP_FUNCTION(select);
PHP_FUNCTION(open_listen_sock);
PHP_FUNCTION(accept_connect);
PHP_FUNCTION(set_nonblock);
PHP_FUNCTION(listen);
PHP_FUNCTION(close);
PHP_FUNCTION(write);
PHP_FUNCTION(read);
PHP_FUNCTION(getsockname);
PHP_FUNCTION(getpeername);
PHP_FUNCTION(socket);
PHP_FUNCTION(connect);
PHP_FUNCTION(strerror);
PHP_FUNCTION(bind);
PHP_FUNCTION(recv);
PHP_FUNCTION(send);
PHP_FUNCTION(recvfrom);
PHP_FUNCTION(sendto);
PHP_FUNCTION(build_iovec);
PHP_FUNCTION(fetch_iovec);
PHP_FUNCTION(free_iovec);
PHP_FUNCTION(add_iovec);
PHP_FUNCTION(delete_iovec);
PHP_FUNCTION(set_iovec);
PHP_FUNCTION(recvmsg);
PHP_FUNCTION(sendmsg);
PHP_FUNCTION(readv);
PHP_FUNCTION(writev);
PHP_FUNCTION(getsockopt);
PHP_FUNCTION(setsockopt);
PHP_FUNCTION(socketpair);
PHP_FUNCTION(shutdown);

typedef struct php_iovec {
	struct iovec *iov_array;
	unsigned int count;
} php_iovec_t;

typedef struct {
	int le_destroy;
	int le_iov;
	int use_system_read;
} php_sockets_globals;


#ifdef ZTS
#define SOCKETSG(v) (sockets_globals->v)
#define SOCKETSLS_FETCH() php_sockets_globals *sockets_globals = ts_resource(sockets_globals_id)
#else
#define SOCKETSG(v) (sockets_globals.v)
#define SOCKETSLS_FETCH()
#endif

#else

#define phpext_sockets_ptr NULL

#endif

#endif	/* PHP_SOCKETS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
