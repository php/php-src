/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_SOCKETS

#define _XOPEN_SOURCE_EXTENDED
#define _XPG4_2
#define __EXTENSIONS__


#include "ext/standard/info.h"
#include "php_ini.h"

#ifndef PHP_WIN32
# include "php_sockets.h"
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/uio.h>
# define IS_INVALID_SOCKET(a)	(a->bsd_socket < 0)
# define set_errno(a) (errno = a)
# define set_h_errno(a) (h_errno = a)
#else /* windows */
# include <winsock.h>
# include "php_sockets.h"
# include "php_sockets_win.h"
# define IS_INVALID_SOCKET(a)	(a->bsd_socket == INVALID_SOCKET)
#endif

#ifdef ZTS
int sockets_globals_id;
#else
php_sockets_globals sockets_globals;
#endif


#ifndef MSG_WAITALL
#ifdef LINUX
#define MSG_WAITALL 0x00000100
#else
#define MSG_WAITALL 0x00000000
#endif
#endif

#ifndef SUN_LEN
#define SUN_LEN(su) (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

#ifndef PF_INET
#define PF_INET AF_INET
#endif


#define PHP_NORMAL_READ 0x0001
#define PHP_BINARY_READ 0x0002


static int le_iov;
#define le_iov_name "Socket I/O vector"
static int le_destroy; 
#define le_destroy_name "Socket file descriptor set"
static int le_socket;
#define le_socket_name "Socket"

static unsigned char second_and_third_args_force_ref[] =
{3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char second_fifth_and_sixth_args_force_ref[] =
{6, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char third_through_seventh_args_force_ref[] =
{7, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};

/* {{{ sockets_functions[]
 */
function_entry sockets_functions[] = {
	PHP_FE(socket_fd_alloc, 		NULL)
	PHP_FE(socket_fd_free, 			NULL)
	PHP_FE(socket_fd_set, 			NULL)
	PHP_FE(socket_fd_isset, 		NULL)
	PHP_FE(socket_fd_clear, 		NULL)
	PHP_FE(socket_fd_zero,	 		NULL)
	PHP_FE(socket_iovec_alloc,		NULL)
	PHP_FE(socket_iovec_free,		NULL)
	PHP_FE(socket_iovec_set,		NULL)
	PHP_FE(socket_iovec_fetch,		NULL)
	PHP_FE(socket_iovec_add,		NULL)
	PHP_FE(socket_iovec_delete,		NULL)
	PHP_FE(socket_select, 			NULL)
	PHP_FE(socket_create, 			NULL)
	PHP_FE(socket_create_listen, 	NULL)
	PHP_FE(socket_create_pair,		NULL)
	PHP_FE(socket_accept, 			NULL)
	PHP_FE(socket_set_nonblock,		NULL)
	PHP_FE(socket_listen, 			NULL)
	PHP_FE(socket_close,			NULL)
	PHP_FE(socket_write, 			NULL)
	PHP_FE(socket_read, 			NULL)
	PHP_FE(socket_getsockname, 		second_and_third_args_force_ref)
	PHP_FE(socket_getpeername, 		second_and_third_args_force_ref)
	PHP_FE(socket_connect, 			NULL)
	PHP_FE(socket_strerror, 		NULL)
	PHP_FE(socket_bind,				NULL)
	PHP_FE(socket_recv,				NULL)
	PHP_FE(socket_send,				NULL)
	PHP_FE(socket_recvfrom,			second_fifth_and_sixth_args_force_ref)
	PHP_FE(socket_sendto,			NULL)
	PHP_FE(socket_recvmsg,			third_through_seventh_args_force_ref)
	PHP_FE(socket_sendmsg,			NULL)
	PHP_FE(socket_readv,			NULL)
	PHP_FE(socket_writev,			NULL)
	PHP_FE(socket_getopt,			NULL)
	PHP_FE(socket_setopt,			NULL)
	PHP_FE(socket_shutdown,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry sockets_module_entry = {
	"sockets",
	sockets_functions,
	PHP_MINIT(sockets),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sockets),
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

/* inet_ntop should be used instead of inet_ntoa */
int inet_ntoa_lock = 0;

static void destroy_fd_sets(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_fd_set *php_fd = (php_fd_set*)rsrc->ptr;

	efree(php_fd);
}

static void destroy_iovec(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	unsigned int i;
	php_iovec_t *iov = (php_iovec_t *) rsrc->ptr;

	if (iov->count && iov->iov_array) {
		for (i = 0; i < iov->count; i++) {
			efree(iov->iov_array[i].iov_base);
		}

		efree(iov->iov_array);
		efree(iov);
	}
}

static void destroy_socket(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_socket *php_sock = (php_socket *) rsrc->ptr;

	close(php_sock->bsd_socket);
	efree(php_sock);
}

static php_socket *open_listen_sock(int port, int backlog)
{
	struct sockaddr_in  la;
	struct hostent     *hp;
	php_socket         *php_sock;
	
	php_sock = emalloc(sizeof(php_socket));

#ifndef PHP_WIN32
	if ((hp = gethostbyname("0.0.0.0")) == NULL) {
#else
	if ((hp = gethostbyname("localhost")) == NULL) {
#endif
		efree(php_sock);
		return NULL;
	}
	
	memcpy((char *) &la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons((unsigned short) port);

	php_sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);
	php_sock->type = PF_INET;
	if (IS_INVALID_SOCKET(php_sock)) {
		php_error(E_WARNING, "Couldn't allocate a new socket from open_listen_sock()");
		efree(php_sock);
		return NULL;
	}

	if (bind(php_sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) < 0) {
		php_error(E_WARNING, "Couldn't bind socket to given address from open_listen_sock()");
		close(php_sock->bsd_socket);
		efree(php_sock);
		return NULL;
	}

	if (listen(php_sock->bsd_socket, backlog) < 0) {
		php_error(E_WARNING, "Couldn't listen on socket %d from open_listen_sock()", php_sock->bsd_socket);
		close(php_sock->bsd_socket);
		efree(php_sock);
		return NULL;
	}

	return php_sock;
}

static php_socket *accept_connect(php_socket *php_sock, struct sockaddr *la)
{
	int m;
	php_socket *retval;
	
	retval = emalloc(sizeof(php_socket));

	m = sizeof(*la);

	retval->bsd_socket = accept(php_sock->bsd_socket, la, &m);
	if (IS_INVALID_SOCKET(retval)) {
		php_error(E_WARNING, "Couldn't accept incoming connection in accept_connect()");
		efree(retval);
		return NULL;
	}
	
	return retval;
}

/* {{{ php_read -- wrapper around read() so that it only reads to a \r or \n. */
int php_read(int bsd_socket, void *buf, int maxlen)
{
	int m = 0, n = 0;
	int no_read = 0;
	int nonblock = 0;
	char *t = (char *) buf;

	m = fcntl(bsd_socket, F_GETFL);
	if (m < 0) {
		return m;
	}

	nonblock = (m & O_NONBLOCK);
	m = 0;

	set_errno(0);

	while (*t != '\n' && *t != '\r' && n < maxlen) {
		if (m > 0) {
			t++;
			n++;
		} else if (m == 0) {
			no_read++;
			if (nonblock && no_read >= 2) {
				return n;
				/* The first pass, m always is 0, so no_read becomes 1
				 * in the first pass. no_read becomes 2 in the second pass,
				 * and if this is nonblocking, we should return.. */
			}
			 
			if (no_read > 200) {
				set_errno(ECONNRESET);
				return -1;
			}
		}
		 
		if (n < maxlen) {
			m = read(bsd_socket, (void *) t, 1);
		}
		 
		if (errno != 0 && errno != ESPIPE && errno != EAGAIN) {
			return -1;
		}
		
		set_errno(0);
	}
	 
	if (n < maxlen) {
		n++;
		/* The only reasons it makes it to here is
		 * if '\n' or '\r' are encountered. So, increase
		 * the return by 1 to make up for the lack of the
		 * '\n' or '\r' in the count (since read() takes
		 * place at the end of the loop..) */
	}
	 
	return n;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sockets)
{
	struct protoent *pe;

	le_socket	= zend_register_list_destructors_ex(destroy_socket,	NULL, le_socket_name, module_number);
	le_destroy	= zend_register_list_destructors_ex(destroy_fd_sets, NULL, le_destroy_name, module_number);
	le_iov		= zend_register_list_destructors_ex(destroy_iovec,	NULL, le_iov_name, module_number);

	REGISTER_LONG_CONSTANT("AF_UNIX",		AF_UNIX,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AF_INET",		AF_INET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_STREAM",	SOCK_STREAM,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_DGRAM",	SOCK_DGRAM,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RAW",		SOCK_RAW,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_SEQPACKET",SOCK_SEQPACKET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RDM",		SOCK_RDM,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_OOB",		MSG_OOB,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_WAITALL",	MSG_WAITALL,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_PEEK",		MSG_PEEK,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_DONTROUTE", MSG_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DEBUG",		SO_DEBUG,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_REUSEADDR",	SO_REUSEADDR,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_KEEPALIVE",	SO_KEEPALIVE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DONTROUTE",	SO_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LINGER",		SO_LINGER,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_BROADCAST",	SO_BROADCAST,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_OOBINLINE",	SO_OOBINLINE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDBUF",		SO_SNDBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVBUF",		SO_RCVBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDLOWAT",	SO_SNDLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVLOWAT",	SO_RCVLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDTIMEO",	SO_SNDTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVTIMEO",	SO_RCVTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_TYPE",		SO_TYPE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_ERROR",		SO_ERROR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOL_SOCKET",	SOL_SOCKET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_NORMAL_READ", PHP_NORMAL_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_BINARY_READ", PHP_BINARY_READ, CONST_CS | CONST_PERSISTENT);

	if ((pe = getprotobyname("tcp"))) {
		REGISTER_LONG_CONSTANT("SOL_TCP", pe->p_proto, CONST_CS | CONST_PERSISTENT);
	}

	if ((pe = getprotobyname("udp"))) {
		REGISTER_LONG_CONSTANT("SOL_UDP", pe->p_proto, CONST_CS | CONST_PERSISTENT);
	}
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sockets)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Sockets Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource socket_fd_alloc(void)
   Allocates a new file descriptor set */
PHP_FUNCTION(socket_fd_alloc)
{
	php_fd_set *php_fd;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	php_fd = (php_fd_set*)emalloc(sizeof(php_fd_set));

	FD_ZERO(&(php_fd->set));
	
	ZEND_REGISTER_RESOURCE(return_value, php_fd, le_destroy);
}
/* }}} */

/* {{{ proto bool socket_fd_dealloc(resource set)
   Deallocates a file descriptor set */
PHP_FUNCTION(socket_fd_free)
{
	zval **arg1;
	php_fd_set *php_fd;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_fd, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);
	
	zend_list_delete(Z_RESVAL_PP(arg1));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_fd_set(resource set, mixed socket)
   Adds (a) file descriptor(s) to a set */
PHP_FUNCTION(socket_fd_set)
{
	zval **arg1, **arg2, **tmp;
	php_fd_set *php_fd;
	php_socket *php_sock;
	SOCKET max_fd = 0;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_fd, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);

	if (Z_TYPE_PP(arg2) == IS_ARRAY) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(arg2));
		while (zend_hash_get_current_data(Z_ARRVAL_PP(arg2), (void**)&tmp) == SUCCESS) {
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, tmp, -1, le_socket_name, le_socket);
			FD_SET(php_sock->bsd_socket, &(php_fd->set));
			max_fd = (php_sock->bsd_socket > max_fd) ? php_sock->bsd_socket : max_fd;
			zend_hash_move_forward(Z_ARRVAL_PP(arg2));
		}
	} else if (Z_TYPE_PP(arg2) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg2, -1, le_socket_name, le_socket);
		FD_SET(php_sock->bsd_socket, &(php_fd->set));
		max_fd = php_sock->bsd_socket;
	} else {
		php_error(E_ERROR, "expecting variable of type array or resource in %s()", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	php_fd->max_fd = max_fd;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_fd_clear(resource set, mixed socket)
   Clears (a) file descriptor(s) from a set */
PHP_FUNCTION(socket_fd_clear)
{
	zval **arg1, **arg2, **tmp;
	php_fd_set *php_fd;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_fd, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);

	if (Z_TYPE_PP(arg2) == IS_ARRAY) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(arg2));
		while (zend_hash_get_current_data(Z_ARRVAL_PP(arg2), (void**)&tmp) == SUCCESS) {
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, tmp, -1, le_socket_name, le_socket);
			FD_CLR(php_sock->bsd_socket, &(php_fd->set));
			zend_hash_move_forward(Z_ARRVAL_PP(arg2));
		}
	} else if (Z_TYPE_PP(arg2) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg2, -1, le_socket_name, le_socket);
		FD_CLR(php_sock->bsd_socket, &(php_fd->set));
	} else {
		php_error(E_ERROR, "expecting variable of type array or resource in %s()", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	php_fd->max_fd = 0;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_fd_isset(resource set, resource socket)
   Checks to see if a file descriptor is set within the file descrirptor set */
PHP_FUNCTION(socket_fd_isset)
{
	zval **arg1, **arg2;
	php_fd_set *php_fd;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_fd, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg2, -1, le_socket_name, le_socket);

	if (FD_ISSET(php_sock->bsd_socket, &(php_fd->set))) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool socket_fd_zero(resource set)
   Clears a file descriptor set */
PHP_FUNCTION(socket_fd_zero)
{
	zval **arg1;
	php_fd_set *php_fd;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_fd, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);

	FD_ZERO(&(php_fd->set));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int socket_select(resource read_fd, resource write_fd, resource except_fd, int tv_sec, int tv_usec)
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(socket_select)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	struct timeval tv;
	php_fd_set *rfds = NULL, *wfds = NULL, *xfds = NULL;
	SOCKET max_fd;
	int sets = 0;

	if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	if (Z_TYPE_PP(arg1) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(rfds, php_fd_set*, arg1, -1, le_destroy_name, le_destroy);
		max_fd = rfds->max_fd;
		sets++;
	}

	if (Z_TYPE_PP(arg2) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(wfds, php_fd_set*, arg2, -1, le_destroy_name, le_destroy);
		max_fd = (max_fd > wfds->max_fd) ? max_fd : wfds->max_fd;
		sets++;
	}

	if (Z_TYPE_PP(arg3) == IS_RESOURCE) {
		ZEND_FETCH_RESOURCE(xfds, php_fd_set*, arg3, -1, le_destroy_name, le_destroy);
		max_fd = (max_fd > xfds->max_fd) ? max_fd : xfds->max_fd;
		sets++;
	}

	if (!sets) {
		php_error(E_ERROR, "expecting atleast one %s in %s()", le_destroy_name, get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	tv.tv_sec  = Z_LVAL_PP(arg4);
	tv.tv_usec = Z_LVAL_PP(arg5);

	RETURN_LONG(select(max_fd+1, rfds ? &(rfds->set) : NULL,
				wfds ? &(wfds->set) : NULL,
				xfds ? &(xfds->set) : NULL,
				&tv));
}
/* }}} */

/* {{{ proto resource socket_create_listen(int port[, int backlog])
   Opens a socket on port to accept connections */
PHP_FUNCTION(socket_create_listen)
{
	zval       **arg1;
	zval       **arg2;
	php_socket  *php_sock;
	int          backlog = 128;
	int          argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 || 
	    zend_get_parameters_ex(argc, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg1);

	if (argc > 1) {
		convert_to_long_ex(arg2);
		backlog = Z_LVAL_PP(arg2);
	}

	php_sock = open_listen_sock(Z_LVAL_PP(arg1), backlog);
	if (php_sock == NULL) {
		php_error(E_WARNING, "unable to create listening socket [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, php_sock, le_socket);
}
/* }}} */

/* {{{ proto resource socket_accept(resource socket)
   Accepts a connection on the listening socket fd */
PHP_FUNCTION(socket_accept)
{
	zval                **arg1;
	php_socket           *php_sock;
	php_socket           *new_sock;
	struct sockaddr_in    sa;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
	
	new_sock = accept_connect(php_sock, (struct sockaddr *) &sa);
	if (new_sock == NULL) {
		php_error(E_WARNING, "unable to accept connection [%d]: %s", errno, strerror(errno));
		RETURN_FALSE
	}
	
	ZEND_REGISTER_RESOURCE(return_value, new_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_set_nonblock(resource socket)
   Sets nonblocking mode for file descriptor fd */
PHP_FUNCTION(socket_set_nonblock)
{
	zval       **arg1;
	php_socket  *php_sock;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);

	if (fcntl(php_sock->bsd_socket, F_SETFL, O_NONBLOCK) == 0) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool socket_listen(resource socket[, int backlog])
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(socket_listen)
{
	zval       **arg1;
	zval       **arg2;
	php_socket  *php_sock;
	int          backlog = 0;
	int          argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 || 
	    zend_get_parameters_ex(argc, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
	if (argc > 1) {
		convert_to_long_ex(arg2);
		backlog = Z_LVAL_PP(arg2);
	}

	if (listen(php_sock->bsd_socket, backlog) != 0) {
		php_error(E_WARNING, "unable to listen [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void socket_close(resource socket)
   Closes a file descriptor */
PHP_FUNCTION(socket_close)
{
	zval       **arg1;
	php_socket  *php_sock;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */

/* {{{ proto int socket_write(resource socket, string buf[, int length])
   Writes the buffer to the file descriptor fd, length is optional */
PHP_FUNCTION(socket_write)
{
	zval        **arg1;
	zval        **arg2;
	zval        **arg3;
	php_socket   *php_sock;
	int           retval;
	int           length;
	int           argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || 
	    zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_string_ex(arg2);

	if (argc > 2) {
		convert_to_long_ex(arg3);
		length = Z_LVAL_PP(arg3);
	}
	else {
		length = Z_STRLEN_PP(arg2);
	}

#ifndef PHP_WIN32
	retval = write(php_sock->bsd_socket, Z_STRVAL_PP(arg2), length);
#else
	retval = send(php_sock->bsd_socket, Z_STRVAL_PP(arg2), length, 0);
#endif

	if (retval <= 0) {
		php_error(E_WARNING, "Unable to write to socket %d [%d]: %s", php_sock->bsd_socket, errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

typedef int (*read_func)(int, void *, int);

/* {{{ proto string socket_read(resource socket, int length [, int type])
   Reads length bytes from socket */
PHP_FUNCTION(socket_read)
{
	zval        **arg1;
	zval        **arg2;
	zval        **arg3;
	php_socket   *php_sock;
	read_func     read_function = (read_func) read;
	char         *tmpbuf;
	int           retval;
	int           argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || 
	    zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
	convert_to_long_ex(arg2);

	if (argc > 2) {
		convert_to_long_ex(arg3);
		if (Z_LVAL_PP(arg3) == PHP_NORMAL_READ)
			read_function = (read_func) php_read;
	}

	tmpbuf = emalloc(Z_LVAL_PP(arg2) + 1);

#ifndef PHP_WIN32
	retval = (*read_function)(php_sock->bsd_socket, tmpbuf, Z_LVAL_PP(arg2));
#else
	retval = recv(php_sock->bsd_socket, tmpbuf, Z_LVAL_PP(arg2), 0);
#endif

	if (retval <= 0) {
		if (retval != 0) { 
			/* Not EOF */
			php_error(E_WARNING, "Couldn't read %d bytes from socket %d [%d]: %s",
		          	Z_LVAL_PP(arg2), php_sock->bsd_socket, errno, strerror(errno));
		}
		efree(tmpbuf);
		RETURN_FALSE;
	}

	tmpbuf[retval] = 0;
	RETURN_STRINGL(tmpbuf, retval, 0);
}
/* }}} */

/* {{{ proto bool socket_getsockname(resource socket, string &addr[, int &port])
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the local side of a socket */
PHP_FUNCTION(socket_getsockname)
{
	zval **arg1, **addr, **port;
	php_sockaddr_storage sa_storage;
	php_socket *php_sock;
	struct sockaddr *sa;
	struct sockaddr_in *sin;
	struct sockaddr_un *s_un;
	int salen = sizeof(php_sockaddr_storage);
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &arg1, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_long_ex(port);
	convert_to_string_ex(addr);

	sa = (struct sockaddr *) &sa_storage;

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		php_error(E_WARNING, "unable to retrieve socket name, %i", errno);
		RETURN_FALSE;
	} else {
		char *addr_string;

		switch (sa->sa_family) {
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;
				
			if (Z_STRLEN_PP(addr) > 0) {
				efree(Z_STRVAL_PP(addr));
			}

			ZVAL_STRING(*addr, addr_string, 1);
			Z_LVAL_PP(port)   = htons(sin->sin_port);
			RETURN_TRUE;
		case AF_UNIX:
			if (Z_STRLEN_PP(addr) > 0) {
				efree(Z_STRVAL_PP(addr));
			}
			s_un = (struct sockaddr_un *) sa;
			ZVAL_STRING(*addr, s_un->sun_path, 1);
			RETURN_TRUE;
		default:
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto bool socket_getpeername(resource socket, string &addr[, int &port])
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the remote side of a socket */
PHP_FUNCTION(socket_getpeername)
{
	zval **arg1, **arg2, **arg3;
	php_sockaddr_storage sa_storage;
	php_socket *php_sock;
	struct sockaddr *sa;
	struct sockaddr_in *sin;
	struct sockaddr_un *s_un;
	char *addr_string;
	int salen = sizeof(php_sockaddr_storage);
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);

	sa = (struct sockaddr *) &sa_storage;

	if (getpeername(php_sock->bsd_socket, sa, &salen) < 0) {
		php_error(E_WARNING, "Unable to retrieve peername [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
	case AF_INET:
		sin = (struct sockaddr_in *) sa;
		while (inet_ntoa_lock == 1);
		inet_ntoa_lock = 1;
		addr_string = inet_ntoa(sin->sin_addr);
		inet_ntoa_lock = 0;
		
		if (Z_STRLEN_PP(arg2) > 0) {
			efree(Z_STRVAL_PP(arg2));
		}

		ZVAL_STRING(*arg2, addr_string, 1);

		if (argc > 2) {
			Z_LVAL_PP(arg3) = htons(sin->sin_port);
		}

		RETURN_TRUE;
	case AF_UNIX:
		if (Z_STRLEN_PP(arg2) > 0) {
			efree(Z_STRVAL_PP(arg2));
		}
		s_un = (struct sockaddr_un *) sa;
		ZVAL_STRING(*arg2, s_un->sun_path, 1);
		RETURN_TRUE;
	default:
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed socket_create(int domain, int type, int protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket_create)
{
	zval **arg1, **arg2, **arg3;
	php_socket *php_sock = (php_socket*)emalloc(sizeof(php_socket));

	if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);

	if (Z_LVAL_PP(arg1) != AF_UNIX && Z_LVAL_PP(arg1) != AF_INET) {
		php_error(E_WARNING, "invalid socket domain [%d] specified, assuming AF_INET", Z_LVAL_PP(arg1));
		Z_LVAL_PP(arg1) = AF_INET;
	}

	if (Z_LVAL_PP(arg2) > 10) {
		php_error(E_WARNING, "invalid socket type [%d] specified, assuming SOCK_STREAM", Z_LVAL_PP(arg2));
		Z_LVAL_PP(arg2) = SOCK_STREAM;
	}
	
	php_sock->bsd_socket = socket(Z_LVAL_PP(arg1), Z_LVAL_PP(arg2), Z_LVAL_PP(arg3));
	php_sock->type = Z_LVAL_PP(arg1);
	if (IS_INVALID_SOCKET(php_sock)) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, php_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_connect(resource socket, string addr [, int port])
   Opens a connection to addr:port on the socket specified by socket */
PHP_FUNCTION(socket_connect)
{
	zval **arg1, **arg2, **arg3;
	php_socket *php_sock;
	struct sockaddr_in sin;
	struct sockaddr_un s_un;
	int retval;
	struct in_addr addr_buf;
	struct hostent *host_struct;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
	convert_to_string_ex(arg2);
	
	if (argc == 3) {
		convert_to_long_ex(arg3);
	}


	switch(php_sock->type) {
	case AF_INET:
		if (argc != 3) {
			WRONG_PARAM_COUNT;
		}

		sin.sin_family = AF_INET;
		sin.sin_port = htons((unsigned short int)Z_LVAL_PP(arg3));

		if (inet_aton(Z_STRVAL_PP(arg2), &addr_buf)) {
			sin.sin_addr.s_addr = addr_buf.s_addr;
		} else {
			char *q = (char *) &(sin.sin_addr.s_addr);
			host_struct = gethostbyname(Z_STRVAL_PP(arg2));
			if (host_struct->h_addrtype != AF_INET) {
				RETURN_FALSE;
			}
			
			q[0] = host_struct->h_addr_list[0][0];
			q[1] = host_struct->h_addr_list[0][1];
			q[2] = host_struct->h_addr_list[0][2];
			q[3] = host_struct->h_addr_list[0][3];
		}
	
		retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
		break;
	case AF_UNIX:
		s_un.sun_family = AF_UNIX;
		snprintf(s_un.sun_path, 108, "%s", Z_STRVAL_PP(arg2));
		retval = connect(php_sock->bsd_socket, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
		break;
	default:
		RETURN_FALSE;
	}	
	
	if (retval != 0) {
		php_error(E_WARNING, "unable to connect [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string socket_strerror(int errno)
   Returns a string describing an error */
PHP_FUNCTION(socket_strerror)
{
	zval **arg1;
	const char *buf;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

#ifndef PHP_WIN32
	if (Z_LVAL_PP(arg1) < -10000) {
		Z_LVAL_PP(arg1) += 10000;
#ifdef HAVE_HSTRERROR
               buf = hstrerror(-(Z_LVAL_PP(arg1)));
#else
               {
               static char buf[100];
               sprintf(buf, "Host lookup error %d", -(Z_LVAL_PP(arg1)));
               }
#endif 
	} else {
		buf = strerror(-(Z_LVAL_PP(arg1)));
	}
#else
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				Z_LVAL_PP(arg1),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&buf,
				0,
				NULL
				);
#endif
	RETURN_STRING(buf ? (char *) buf : "", 1);
}
/* }}} */

/* {{{ proto bool socket_bind(resource socket, string addr [, int port])
   Binds an open socket to a listening port, port is only specified in AF_INET family. */
PHP_FUNCTION(socket_bind)
{
	zval **arg1, **arg2, **arg3;
	long retval = 0;
	php_sockaddr_storage sa_storage;
	struct sockaddr *sock_type = (struct sockaddr*) &sa_storage;
	php_socket *php_sock;
	int argc = ZEND_NUM_ARGS();


	if (argc < 2 ||  argc > 3 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 3:
			convert_to_long_ex(arg3);
		case 2:
			convert_to_string_ex(arg2);
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
			break;
	}
	

	if (php_sock->type == AF_UNIX) {
		struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;
		memset(sa, 0, sizeof(sa_storage));
		sa->sun_family = AF_UNIX;
		snprintf(sa->sun_path, 108, "%s", Z_STRVAL_PP(arg2));
		retval = bind(php_sock->bsd_socket, (struct sockaddr *) sa, SUN_LEN(sa));
	} else if (php_sock->type == AF_INET) {
		struct sockaddr_in sa;
		struct hostent *hp;

		if (argc != 3) {
			WRONG_PARAM_COUNT;
		}
		
		if ((hp = gethostbyname(Z_STRVAL_PP(arg2))) == NULL) {
			php_error(E_WARNING, "unable to lookup [%s], %i", Z_STRVAL_PP(arg2), h_errno);
			RETURN_FALSE;
		}
		
		memcpy((char *)&sa.sin_addr, hp->h_addr, hp->h_length);
		sa.sin_family	= hp->h_addrtype;
		sa.sin_port		= htons((unsigned short)Z_LVAL_PP(arg3));
		retval = bind(php_sock->bsd_socket, (struct sockaddr *)&sa, sizeof(sa));

	} else {
		php_error(E_WARNING,"the specified port is not supported");
		RETURN_FALSE;
	}
	
	if (retval != 0) {
		php_error(E_WARNING, "unable to bind address, [%i] %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource socket_iovec_alloc(int num_vectors [, int ...])
   Builds a 'struct iovec' for use with sendmsg, recvmsg, writev, and readv */
/* First parameter is number of vectors, each additional parameter is the
   length of the vector to create.
 */
PHP_FUNCTION(socket_iovec_alloc)
{
	zval ***args = (zval ***)NULL;
	php_iovec_t *vector;
	struct iovec *vector_array;
	int i, j, num_vectors, argc = ZEND_NUM_ARGS();
	
	args = emalloc(argc*sizeof(zval**));

	if (argc < 1 || zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(args[0]);
	num_vectors = Z_LVAL_PP(args[0]);
	
	vector_array = emalloc(sizeof(struct iovec)*(num_vectors+1));

	for (i = 0, j = 1; i < num_vectors; i++, j++) {
		convert_to_long_ex(args[j]);
		
		vector_array[i].iov_base	= (char*)emalloc(Z_LVAL_PP(args[j]));
		vector_array[i].iov_len		= Z_LVAL_PP(args[j]);
	}

	vector = emalloc(sizeof(php_iovec_t));
	vector->iov_array = vector_array;
	vector->count = num_vectors;

	ZEND_REGISTER_RESOURCE(return_value, vector, le_iov);
}
/* }}} */

/* {{{ proto string socket_iovec_fetch(resource iovec, int iovec_position)
   Returns the data held in the iovec specified by iovec_id[iovec_position] */
PHP_FUNCTION(socket_iovec_fetch)
{
	zval **iovec_id, **iovec_position;
	php_iovec_t *vector;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iovec_position) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (Z_LVAL_PP(iovec_position) > vector->count) {
		php_error(E_WARNING, "Can't access a vector position past the amount of vectors set in the array");
		RETURN_NULL();
	}

	RETURN_STRINGL(vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base,
		       vector->iov_array[Z_LVAL_PP(iovec_position)].iov_len, 1);
}
/* }}} */

/* {{{ proto bool socket_iovec_set(resource iovec, int iovec_position, string new_val)
   Sets the data held in iovec_id[iovec_position] to new_val */
PHP_FUNCTION(socket_iovec_set)
{
	zval **iovec_id, **iovec_position, **new_val;
	php_iovec_t *vector;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &iovec_id, &iovec_position, &new_val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (Z_LVAL_PP(iovec_position) > vector->count) {
		php_error(E_WARNING, "Can't access a vector position outside of the vector array bounds");
		RETURN_FALSE;
	}
	
	if (vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base) {
		efree(vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base);
	}
	
	vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base = estrdup(Z_STRVAL_PP(new_val));
	vector->iov_array[Z_LVAL_PP(iovec_position)].iov_len = strlen(Z_STRVAL_PP(new_val));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_iovec_add(resource iovec, int iov_len)
   Adds a new vector to the scatter/gather array */
PHP_FUNCTION(socket_iovec_add)
{
	zval **iovec_id, **iov_len;
	php_iovec_t *vector;
	struct iovec *vector_array;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iov_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	vector_array = (struct iovec*)emalloc(sizeof(struct iovec) * (vector->count + 2));
	memcpy(vector_array, vector->iov_array, sizeof(struct iovec) * vector->count);

	vector_array[vector->count].iov_base = (char*)emalloc(Z_LVAL_PP(iov_len));
	vector_array[vector->count].iov_len = Z_LVAL_PP(iov_len);
	efree(vector->iov_array);
	vector->iov_array = vector_array;
	vector->count++;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool socket_iovec_delete(resource iovec, int iov_pos)
   Deletes a vector from an array of vectors */
PHP_FUNCTION(socket_iovec_delete)
{
	zval **iovec_id, **iov_pos;
	php_iovec_t *vector;
	struct iovec *vector_array;
	int i;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iov_pos) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (Z_LVAL_PP(iov_pos) > vector->count) {
		php_error(E_WARNING, "Can't delete an IO vector that is out of array bounds");
		RETURN_FALSE;
	}
	vector_array = emalloc(vector->count * sizeof(struct iovec));

	for (i = 0; i < vector->count; i++) {
		if (i < Z_LVAL_PP(iov_pos)) {
			memcpy(&(vector->iov_array[i]), &(vector_array[i]), sizeof(struct iovec));
		} else if (i > Z_LVAL_PP(iov_pos)) {
			memcpy(&(vector->iov_array[i]), &(vector_array[i - 1]), sizeof(struct iovec));
		}
	}

	efree(vector->iov_array);
	vector->iov_array = vector_array;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool socket_iovec_free(resource iovec)
   Frees the iovec specified by iovec_id */
PHP_FUNCTION(socket_iovec_free)
{
	zval **arg1;
	php_iovec_t *vector;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, arg1, -1, "IO vector table", le_iov);

	zend_list_delete(Z_RESVAL_PP(arg1));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_readv(resource socket, resource iovec_id)
   Reads from an fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(socket_readv)
{
	zval **arg1, **arg2;
	php_iovec_t *vector;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, arg2, -1, "IO vector table", le_iov);

	if (readv(php_sock->bsd_socket, vector->iov_array, vector->count) != 0) {
		php_error(E_WARNING, "unable to read, %i", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_writev(resource socket, resource iovec_id)
   Writes to a file descriptor, fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(socket_writev)
{
	zval **arg1, **arg2;
	php_iovec_t *vector;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, arg2, -1, "IO vector table", le_iov);

	if (writev(php_sock->bsd_socket, vector->iov_array, vector->count) != 0) {
		php_error(E_WARNING, "unable to write, %i", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed socket_recv(resource socket, int len, int flags)
   Receives data from a connected socket */
PHP_FUNCTION(socket_recv)
{
	zval **arg1, **arg2, **arg3;
	char *recv_buf;
	php_socket *php_sock;
	int retval;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);	
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);

	recv_buf = emalloc(Z_LVAL_PP(arg2) + 2);
	memset(recv_buf, 0, Z_LVAL_PP(arg2) + 2);

	retval = recv(php_sock->bsd_socket, recv_buf, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3));

	if (retval == 0) {
		efree(recv_buf);
		RETURN_FALSE;
	}

	recv_buf[retval+1] = '\0';
	RETURN_STRING(recv_buf, 0);
}
/* }}} */

/* {{{ proto int socket_send(resource socket, string buf, int len, int flags)
   Sends data to a connected socket */
PHP_FUNCTION(socket_send)
{
	zval **arg1, **arg2, **arg3, **arg4;
	php_socket *php_sock;
	int retval;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_string_ex(arg2);

	retval = send(php_sock->bsd_socket, Z_STRVAL_PP(arg2), (Z_STRLEN_PP(arg2) < Z_LVAL_PP(arg3) ? Z_STRLEN_PP(arg2) : Z_LVAL_PP(arg3)), Z_LVAL_PP(arg4));
	
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_recvfrom(resource socket, string &buf, int len, int flags, string &name [, int &port])
   Receives data from a socket, connected or not */
PHP_FUNCTION(socket_recvfrom)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	php_socket *php_sock;
	struct sockaddr_un s_un;
	struct sockaddr_in sin;
	socklen_t slen;
	int retval, argc = ZEND_NUM_ARGS();
	char *recv_buf, *address;


	if (argc < 5 || argc > 6 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 6:
			convert_to_long_ex(arg6);
		case 5:
			convert_to_string_ex(arg5);
			convert_to_long_ex(arg4);
			convert_to_long_ex(arg3);
			convert_to_string_ex(arg2);
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
			break;
	}

	recv_buf = emalloc(Z_LVAL_PP(arg3) + 2);
	memset(recv_buf, 0, Z_LVAL_PP(arg3) + 2);
	
	switch (php_sock->type) {
	case AF_UNIX:
		slen = sizeof(s_un);
		s_un.sun_family = AF_UNIX;
		retval = recvfrom(php_sock->bsd_socket, recv_buf, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4),
			(struct sockaddr *)&s_un, (socklen_t *)&slen);
			
		if (retval < 0) {
			efree(recv_buf);
			php_error(E_WARNING, "unable to recvfrom, %i", errno);
			RETURN_FALSE;
		}

		if (Z_STRVAL_PP(arg2) != NULL) {
			efree(Z_STRVAL_PP(arg2));
		}
		ZVAL_STRING(*arg2, recv_buf, 0);
				
		if (Z_STRLEN_PP(arg5) > 0) {
			efree(Z_STRVAL_PP(arg5));
		}
		ZVAL_STRING(*arg5, s_un.sun_path, 1);
		break;
	case AF_INET:
		slen = sizeof(sin);
		sin.sin_family = AF_INET;
		
		if (argc != 6) {
			WRONG_PARAM_COUNT;
		}
				
		retval = recvfrom(php_sock->bsd_socket, recv_buf, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4),
			(struct sockaddr *)&sin, (socklen_t *)&slen);
			
		if (retval < 0) {
			efree(recv_buf);
			php_error(E_WARNING, "unable to recvfrom, %i", errno);
			RETURN_FALSE;
		}
				
		if (Z_STRLEN_PP(arg2) > 0) {
			efree(Z_STRVAL_PP(arg2));
		}

		ZVAL_STRING(*arg2, recv_buf, 0); 
				
		if (Z_STRLEN_PP(arg5) > 0) {
			efree(Z_STRVAL_PP(arg5));
		}
				
		address = inet_ntoa(sin.sin_addr);
		ZVAL_STRING(*arg5, address ? address : "0.0.0.0", 1);
			
		ZVAL_LONG(*arg6, ntohs(sin.sin_port));
	default:
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_sendto(resource socket, string buf, int len, int flags, string addr [, int port])
   Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(socket_sendto)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	php_socket *php_sock;
	struct sockaddr_un s_un;
	struct sockaddr_in sin;
	struct in_addr addr_buf;
	int retval, argc = ZEND_NUM_ARGS(), which;

	if (argc < 5 || argc > 6 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 6:
			convert_to_long_ex(arg6);
		case 5:
			convert_to_string_ex(arg5);
			convert_to_long_ex(arg4);
			convert_to_long_ex(arg3);
			convert_to_string_ex(arg2);
			ZEND_FETCH_RESOURCE(php_sock, php_socket *, arg1, -1, le_socket_name, le_socket);
			break;
	}


	switch ( php_sock->bsd_socket ) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, 108, "%s", Z_STRVAL_PP(arg5));

			which = (Z_STRLEN_PP(arg2) > Z_LVAL_PP(arg3)) ? 1 : 0;
			retval = sendto(php_sock->bsd_socket, Z_STRVAL_PP(arg2), (which ? Z_LVAL_PP(arg3) : Z_STRLEN_PP(arg2)),
			Z_LVAL_PP(arg4), (struct sockaddr *) &s_un, SUN_LEN(&s_un));

		case AF_INET:
			if (argc != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
	
			if (inet_aton(Z_STRVAL_PP(arg5), &addr_buf) == 0) {
				sin.sin_addr.s_addr = addr_buf.s_addr;
			} else {
				struct hostent *he;
				he = gethostbyname(Z_STRVAL_PP(arg4));

				if (he == NULL) {
					php_error(E_WARNING, "unable to sendto, %i", h_errno);
					RETURN_FALSE;
				}

				sin.sin_addr.s_addr = *(int *) (he->h_addr_list[0]);
			}

			sin.sin_port = htons((unsigned short)Z_LVAL_PP(arg6));
			which = (Z_STRLEN_PP(arg2) > Z_LVAL_PP(arg3)) ? 1 : 0;
			retval = sendto(php_sock->bsd_socket, Z_STRVAL_PP(arg2), (which ? Z_LVAL_PP(arg3) : Z_STRLEN_PP(arg2)),
				Z_LVAL_PP(arg4), (struct sockaddr *) &sin, sizeof(sin));

		default:
			RETURN_LONG(0);
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool socket_recvmsg(resource socket, resource iovec, array &control, int &controllen, int &flags, string &addr [, int &port])
   Used to receive messages on a socket, whether connection-oriented or not */
PHP_FUNCTION(socket_recvmsg)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	zval *control_array = NULL;
	php_iovec_t *iov;
	struct msghdr hdr;
	php_sockaddr_storage sa_storage;
	php_socket *php_sock;
	struct sockaddr *sa = (struct sockaddr *) &sa_storage;
	struct sockaddr_in *sin = (struct sockaddr_in *) sa;
	struct sockaddr_un *s_un = (struct sockaddr_un *) sa;
	struct cmsghdr *ctl_buf;
	socklen_t salen = sizeof(sa_storage);
	int argc = ZEND_NUM_ARGS();

	if (argc < 6 || argc > 7 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 7:
			convert_to_long_ex(arg7);
		case 6:
			convert_to_string_ex(arg6);
			convert_to_long_ex(arg5);
			convert_to_long_ex(arg4);
			convert_to_array_ex(arg3);
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);			
			ZEND_FETCH_RESOURCE(iov, php_iovec_t *, arg2, -1, "IO vector table", le_iov);
			break;
	}

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		php_error(E_WARNING, "unable to recvms, %i", errno);
		RETURN_FALSE;
	}
	
	ctl_buf = (Z_LVAL_PP(arg4) > sizeof(struct cmsghdr)) ? (struct cmsghdr*)emalloc(Z_LVAL_PP(arg4)) : NULL;
	MAKE_STD_ZVAL(control_array);

	switch (sa->sa_family) {
		case AF_INET:
			
			if (ZEND_NUM_ARGS() != 7) {
				efree(ctl_buf);
				WRONG_PARAM_COUNT;
			}
			
			memset(sa, 0, sizeof(sa_storage));
			hdr.msg_name = sin;
			hdr.msg_namelen = sizeof(sa_storage);
			hdr.msg_iov = iov->iov_array;
			hdr.msg_iovlen = iov->count;

			hdr.msg_control = ctl_buf ? ctl_buf : NULL;
			hdr.msg_controllen = ctl_buf ? Z_LVAL_PP(arg4) : 0;
			hdr.msg_flags = 0;
	
			if (recvmsg(php_sock->bsd_socket, &hdr, Z_LVAL_PP(arg5)) != 0) {
				php_error(E_WARNING, "unable to recvmsg, %i", errno);
				RETURN_FALSE;
			} else {
				struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;
				
				/* copy values as appropriate... */
				if (array_init(control_array) == FAILURE) {
					php_error(E_WARNING, "Cannot intialize array");
					RETURN_FALSE;
				}
				
				add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
				*arg3 = control_array;
				zval_copy_ctor(*arg3);
					
				Z_LVAL_PP(arg4) = hdr.msg_controllen;
				Z_LVAL_PP(arg5) = hdr.msg_flags;

				if (Z_STRLEN_PP(arg6) > 0) {
					efree(Z_STRVAL_PP(arg6));
				}
				
				{
					char *tmp = inet_ntoa(sin->sin_addr);
					if (tmp == NULL) {
						Z_STRVAL_PP(arg6) = estrdup("0.0.0.0");
					} else {
						Z_STRVAL_PP(arg6) = estrdup(tmp);
					}
				}

				Z_STRLEN_PP(arg6) = strlen(Z_STRVAL_PP(arg6));
				Z_LVAL_PP(arg7)   = ntohs(sin->sin_port);
				RETURN_TRUE;
			}

	case AF_UNIX:
		memset(sa, 0, sizeof(sa_storage));
		hdr.msg_name = s_un;
		hdr.msg_namelen = sizeof(struct sockaddr_un);
		hdr.msg_iov = iov->iov_array;
		hdr.msg_iovlen = iov->count;
		
		if (ctl_buf) {
			hdr.msg_control = ctl_buf;
			hdr.msg_controllen = Z_LVAL_PP(arg4);
		} else {
			hdr.msg_control = NULL;
			hdr.msg_controllen = 0;
		}

		hdr.msg_flags = 0;
	
		if (recvmsg(php_sock->bsd_socket, &hdr, Z_LVAL_PP(arg5)) != 0) {
			php_error(E_WARNING, "unable to recvmsg, %i", errno);
			RETURN_FALSE;
		} else {
			struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;
			
			if (mhdr != NULL) {
				/* copy values as appropriate... */
				if (array_init(control_array) == FAILURE) {
					php_error(E_WARNING, "Cannot initialize return value from recvmsg()");
					RETURN_FALSE;
				}
				
				add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
				*arg3 = control_array;
				Z_LVAL_PP(arg4) = hdr.msg_controllen;
			}

			Z_LVAL_PP(arg5)      = hdr.msg_flags;
			
			if (Z_STRVAL_PP(arg6) != NULL) {
				efree(Z_STRVAL_PP(arg6));
			}

			Z_STRVAL_PP(arg6) = estrdup(s_un->sun_path);
			RETURN_TRUE;
		}
	default:
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_sendmsg(resource socket, resource iovec, int flags, string addr [, int port])
   Sends a message to a socket, regardless of whether it is connection-oriented or not */
PHP_FUNCTION(socket_sendmsg)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	php_iovec_t *iov;
	php_socket *php_sock;
	int argc = ZEND_NUM_ARGS();
	struct sockaddr sa;
	int salen;

	if (argc < 4 || argc > 5 || zend_get_parameters_ex(argc, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (argc) {
		case 5:
			convert_to_long_ex(arg5);
		case 4:
			convert_to_string_ex(arg4);
			convert_to_long_ex(arg3);
			ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
			ZEND_FETCH_RESOURCE(iov, php_iovec_t *, arg2, -1, "IO vector table", le_iov);
			break;
	}

	salen = sizeof(sa);
	if (getsockname(php_sock->bsd_socket, &sa, &salen) != 0) {
		php_error(E_WARNING, "unable to sendmsg, %i", errno);
		RETURN_FALSE;
	}

	switch(sa.sa_family) {
		case AF_INET:
			{
				struct msghdr hdr;
				struct sockaddr_in *sin = (struct sockaddr_in *) &sa;

				set_h_errno(0);
				set_errno(0);

				memset(&hdr, 0, sizeof(hdr));
				hdr.msg_name = &sa;
				hdr.msg_namelen = sizeof(sa);
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;
				
				if (inet_aton(Z_STRVAL_PP(arg4), &sin->sin_addr) != 0) {
					struct hostent *he = gethostbyname(Z_STRVAL_PP(arg4));
					if (!he) {
						php_error(E_WARNING, "unable to sendmsg, %i", h_errno);
						RETURN_FALSE;
					}
					sin->sin_addr.s_addr = *(int *)(he->h_addr_list[0]);
				}

				sin->sin_port = htons((unsigned short)Z_LVAL_PP(arg5));
				
				if (sendmsg(php_sock->bsd_socket, &hdr, Z_LVAL_PP(arg3)) != 0) {
					php_error(E_WARNING, "unable to sendmsg, %i", errno);
				}

				RETURN_TRUE;
			}

		case AF_UNIX:
			{
				struct msghdr hdr;
				struct sockaddr_un *s_un = (struct sockaddr_un *) &sa;

				set_errno(0);

				hdr.msg_name = s_un;
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;

				snprintf(s_un->sun_path, 108, "%s", Z_STRVAL_PP(arg4));

				hdr.msg_namelen = SUN_LEN(s_un);

				if (sendmsg(php_sock->bsd_socket, &hdr, Z_LVAL_PP(arg3)) != 0) {
					php_error(E_WARNING, "unable to sendmsg, %i", errno);
					RETURN_FALSE;
				}
				
				RETURN_TRUE;
			}

		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed socket_getopt(resource socket, int level, int optname)
   Gets socket options for the socket */
PHP_FUNCTION(socket_getopt)
{
	zval **arg1, **arg2, **arg3;
	struct linger linger_val;
	int other_val;
	socklen_t optlen;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	
	if (Z_LVAL_PP(arg2) == SO_LINGER) {
		optlen = sizeof(struct linger);

		if (getsockopt(php_sock->bsd_socket, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), (char*)&linger_val, &optlen) != 0) {
			php_error(E_WARNING, "unable to retrieve socket option, %i", errno);
			RETURN_FALSE;
		}

		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
		add_assoc_long(return_value, "l_linger", linger_val.l_linger);

	} else {
		optlen = sizeof(other_val);
		
		if (getsockopt(php_sock->bsd_socket, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), (char*)&other_val, &optlen) != 0) {
			php_error(E_WARNING, "unable to retrieve socket option, %i", errno);
			RETURN_FALSE;
		}

		RETURN_LONG(other_val);
	}
}
/* }}} */

/* {{{ proto bool socket_setopt(resource socket, int level, int optname, int|array optval)
   Sets socket options for the socket */
PHP_FUNCTION(socket_setopt)
{
	zval **arg1, **arg2, **arg3, **arg4;
	struct linger lv;
	int ov, optlen, retval;
	php_socket *php_sock;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);

	set_errno(0);

	if (Z_LVAL_PP(arg3) == SO_LINGER) {
		HashTable *ht;
		zval **l_onoff;
		zval **l_linger;

		convert_to_array_ex(arg4);
		ht = HASH_OF(*arg4);

		if (zend_hash_find(ht, "l_onoff", strlen("l_onoff") + 1, (void **)&l_onoff) == FAILURE) {
			php_error(E_WARNING, "No key \"l_onoff\" passed in optval");
			RETURN_FALSE;
		}
		if (zend_hash_find(ht, "l_linger", strlen("l_linger") + 1, (void **)&l_linger) == FAILURE) {
			php_error(E_WARNING, "No key \"l_linger\" passed in optval");
			RETURN_FALSE;
		}

		convert_to_long_ex(l_onoff);
		convert_to_long_ex(l_linger);

		lv.l_onoff	= (unsigned short)Z_LVAL_PP(l_onoff);
		lv.l_linger = (unsigned short)Z_LVAL_PP(l_linger);

		optlen = sizeof(lv);
		
		retval = setsockopt(php_sock->bsd_socket, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), (char*)&lv, optlen);

	} else {
		convert_to_long_ex(arg4);

		optlen = sizeof(ov);
		ov = Z_LVAL_PP(arg3);

		retval = setsockopt(php_sock->bsd_socket, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), (char*)&ov, optlen);
	}

	if (retval != 0) {
		php_error(E_WARNING, "unable to set socket option, %i", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_create_pair(int domain, int type, int protocol, array &fd)
   Creates a pair of indistinguishable sockets and stores them in fds. */
PHP_FUNCTION(socket_create_pair)
{
	zval **arg1, **arg2, **arg3, **arg4;
	zval *retval[2];
	php_socket *php_sock[2];
	SOCKET fds_array[2];
	
	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(arg1);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_array_ex(arg4);

	php_sock[0]= (php_socket*)emalloc(sizeof(php_socket));
	php_sock[1]= (php_socket*)emalloc(sizeof(php_socket));

	if (Z_LVAL_PP(arg1) != AF_INET && Z_LVAL_PP(arg2) != AF_UNIX) {
		php_error(E_WARNING, "invalid socket domain specified, assuming AF_INET");
		Z_LVAL_PP(arg1) = AF_INET;
	}
	
	if (Z_LVAL_PP(arg2) > 10) {
		php_error(E_WARNING, "Invalid socket type specified, assuming SOCK_STREAM");
		Z_LVAL_PP(arg2) = SOCK_STREAM;
	}
	
	if (array_init(*arg4) == FAILURE) {
		php_error(E_WARNING, "Can't initialize fds array");
		RETURN_FALSE;
	}

	if (socketpair(Z_LVAL_PP(arg1), Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), fds_array) != 0) {
		php_error(E_WARNING, "unable to create socket pair, %i", errno);
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(retval[0]);
	MAKE_STD_ZVAL(retval[1]);

	php_sock[0]->bsd_socket = fds_array[0];
	php_sock[1]->bsd_socket = fds_array[1];
	php_sock[0]->type = Z_LVAL_PP(arg1);
	php_sock[1]->type = Z_LVAL_PP(arg1);	

	ZEND_REGISTER_RESOURCE(retval[0], php_sock[0], le_socket);
	ZEND_REGISTER_RESOURCE(retval[1], php_sock[1], le_socket);

	add_index_zval(*arg4, 0, retval[0]);
	add_index_zval(*arg4, 1, retval[1]);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_shutdown(resource socket[, int how])
   Shuts down a socket for receiving, sending, or both. */
PHP_FUNCTION(socket_shutdown)
{
	zval **arg1, **arg2;
	int how_shutdown = 0, argc = ZEND_NUM_ARGS();
	php_socket *php_sock;
	
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, arg1, -1, "Socket", le_socket);
	
	if (argc > 1) {
		convert_to_long_ex(arg2);
		how_shutdown = Z_LVAL_PP(arg2);
	}
	
	if (shutdown(php_sock->bsd_socket, how_shutdown) != 0) {
		php_error(E_WARNING, "unable to shutdown socket, %i", errno);
		RETURN_FALSE;
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
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
