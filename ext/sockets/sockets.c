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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |          Jason Greene     <jason@php.net>                            |
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

#include "php_network.h"
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
# include "php_sockets.h"
# include "php_sockets_win.h"
# define IS_INVALID_SOCKET(a)	(a->bsd_socket == INVALID_SOCKET)
#endif

ZEND_DECLARE_MODULE_GLOBALS(sockets)

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

#define PHP_SOCKET_ERROR(socket,msg,errn)	socket->error = errn;	\
											SOCKETS_G(last_error) = errn; \
											php_error(E_WARNING, "%s() %s [%d]: %s", \
													  get_active_function_name(TSRMLS_C), msg, errn, php_strerror(errn TSRMLS_CC))

static int le_iov;
#define le_iov_name "Socket I/O vector"
static int le_socket;
#define le_socket_name "Socket"

static unsigned char first_through_third_args_force_ref[] =
{3, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};

static unsigned char second_and_third_args_force_ref[] =
{3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char second_arg_of_four_force_ref[] =
{4, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE};

static unsigned char fourth_arg_force_ref[] =
{4, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE};

static unsigned char second_fifth_and_sixth_args_force_ref[] =
{6, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char third_through_seventh_args_force_ref[] =
{7, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};

/* {{{ sockets_functions[]
 */
function_entry sockets_functions[] = {
	PHP_FE(socket_iovec_alloc,		NULL)
	PHP_FE(socket_iovec_free,		NULL)
	PHP_FE(socket_iovec_set,		NULL)
	PHP_FE(socket_iovec_fetch,		NULL)
	PHP_FE(socket_iovec_add,		NULL)
	PHP_FE(socket_iovec_delete,		NULL)
	PHP_FE(socket_select,			first_through_third_args_force_ref)
	PHP_FE(socket_create,			NULL)
	PHP_FE(socket_create_listen,	NULL)
	PHP_FE(socket_create_pair,		fourth_arg_force_ref)
	PHP_FE(socket_accept,			NULL)
	PHP_FE(socket_set_nonblock,		NULL)
	PHP_FE(socket_set_block,		NULL)
	PHP_FE(socket_listen,			NULL)
	PHP_FE(socket_close,			NULL)
	PHP_FE(socket_write,			NULL)
	PHP_FE(socket_read,				NULL)
	PHP_FE(socket_getsockname, 		second_and_third_args_force_ref)
	PHP_FE(socket_getpeername, 		second_and_third_args_force_ref)
	PHP_FE(socket_connect,			NULL)
	PHP_FE(socket_strerror,			NULL)
	PHP_FE(socket_bind,				NULL)
	PHP_FE(socket_recv,				second_arg_of_four_force_ref)
	PHP_FE(socket_send,				NULL)
	PHP_FE(socket_recvfrom,			second_fifth_and_sixth_args_force_ref)
	PHP_FE(socket_sendto,			NULL)
	PHP_FE(socket_recvmsg,			third_through_seventh_args_force_ref)
	PHP_FE(socket_sendmsg,			NULL)
	PHP_FE(socket_readv,			NULL)
	PHP_FE(socket_writev,			NULL)
	PHP_FE(socket_get_option,		NULL)
	PHP_FE(socket_set_option,		NULL)
	PHP_FE(socket_shutdown,			NULL)
	PHP_FE(socket_last_error,		NULL)
	PHP_FE(socket_clear_error,		NULL)
	
	/* for downwards compatability */
	PHP_FALIAS(socket_getopt, socket_get_option, NULL)
	PHP_FALIAS(socket_setopt, socket_set_option, NULL)
	
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry sockets_module_entry = {
	STANDARD_MODULE_HEADER,
	"sockets",
	sockets_functions,
	PHP_MINIT(sockets),
	NULL,
	PHP_RINIT(sockets),
	PHP_RSHUTDOWN(sockets),
	PHP_MINFO(sockets),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

/* inet_ntop should be used instead of inet_ntoa */
int inet_ntoa_lock = 0;

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

static char *php_strerror(int error TSRMLS_DC);

int open_listen_sock(php_socket **php_sock, int port, int backlog TSRMLS_DC)
{
	struct sockaddr_in  la;
	struct hostent		*hp;
	php_socket			*sock = (php_socket*)emalloc(sizeof(php_socket));

	*php_sock = sock;

#ifndef PHP_WIN32
	if ((hp = gethostbyname("0.0.0.0")) == NULL) {
#else
	if ((hp = gethostbyname("localhost")) == NULL) {
#endif
		efree(sock);
		return 0;
	}
	
	memcpy((char *) &la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons((unsigned short) port);

	sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (IS_INVALID_SOCKET(sock)) {
		PHP_SOCKET_ERROR(sock, "unable to create listening socket", errno);
		efree(sock);
		return 0;
	}

	sock->type = PF_INET;

	if (bind(sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) < 0) {
		PHP_SOCKET_ERROR(sock, "unable to bind to given adress", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	if (listen(sock->bsd_socket, backlog) < 0) {
		PHP_SOCKET_ERROR(sock, "unable to listen on socket", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	return 1;
}

int accept_connect(php_socket *in_sock, php_socket **new_sock, struct sockaddr *la TSRMLS_DC)
{
	socklen_t	salen;
	php_socket	*out_sock = (php_socket*)emalloc(sizeof(php_socket));
	
	*new_sock = out_sock;
	salen = sizeof(*la);

	out_sock->bsd_socket = accept(in_sock->bsd_socket, la, &salen);

	if (IS_INVALID_SOCKET(out_sock)) {
		PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		efree(out_sock);
		return 0;
	}
	
	return 1;
}

/* {{{ php_read -- wrapper around read() so that it only reads to a \r or \n. */
int php_read(int bsd_socket, void *buf, size_t maxlen, int flags)
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
			m = recv(bsd_socket, (void *) t, 1, flags);
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

static char *php_strerror(int error TSRMLS_DC) {
	const char *buf;

#ifndef PHP_WIN32
	if (error < -10000) {
		error = -error - 10000;

#ifdef HAVE_HSTRERROR
		buf = hstrerror(error);
#else
		{
			sprintf(SOCKETS_G(strerror_buf), "Host lookup error %d", error);
			buf = SOCKETS_G(strerror_buf);
		}
#endif
	} else {
		buf = strerror(error);
	}
#else 
	{
		LPTSTR tmp = NULL;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |	FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &tmp, 0, NULL);

		strlcpy(SOCKETS_G(strerror_buf), (char *) tmp, 10000);
		LocalFree(tmp);
		
		buf = SOCKETS_G(strerror_buf);
	}
#endif
	
	return (buf ? (char *) buf : "");
}

/* Sets addr by hostname, or by ip in string form (AF_INET)  */
int php_set_inet_addr(struct sockaddr_in *sin, char *string, php_socket *php_sock  TSRMLS_DC) {
	struct in_addr tmp;
	struct hostent *host_entry;

	if (inet_aton(string, &tmp)) {
		sin->sin_addr.s_addr = tmp.s_addr;
	} else {
		if (! (host_entry = gethostbyname(string))) {
			/* Note: < -10000 indicates a host lookup error */
#ifdef PHP_WIN32
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", WSAGetLastError());
#else
			PHP_SOCKET_ERROR(php_sock, "Host lookup failed", (-10000 - h_errno));
#endif
			return 0;
		}
		if (host_entry->h_addrtype != AF_INET) {
			php_error(E_WARNING, "%s() Host lookup failed: Non AF_INET domain returned on AF_INET socket", get_active_function_name(TSRMLS_C));
			return 0;
		}
		memcpy(&(sin->sin_addr.s_addr), host_entry->h_addr_list[0], host_entry->h_length);
	}

	return 1;
}


static void php_sockets_init_globals(zend_sockets_globals *sockets_globals TSRMLS_DC)
{
	sockets_globals->last_error = 0;
	sockets_globals->strerror_buf = NULL;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sockets)
{
	struct protoent *pe;

	ZEND_INIT_MODULE_GLOBALS(sockets, php_sockets_init_globals, NULL);

	le_socket	= zend_register_list_destructors_ex(destroy_socket,	NULL, le_socket_name, module_number);
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
	REGISTER_LONG_CONSTANT("SOMAXCONN",		SOMAXCONN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_NORMAL_READ", PHP_NORMAL_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_BINARY_READ", PHP_BINARY_READ, CONST_CS | CONST_PERSISTENT);

#ifndef WIN32
# include "unix_socket_constants.h"
#else
# include "win32_socket_constants.h"
#endif

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

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(sockets)
{
	if ((SOCKETS_G(strerror_buf) = emalloc(16384))) 
		return SUCCESS;
	
	return FAILURE;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(sockets)
{
	efree(SOCKETS_G(strerror_buf));
	
	return SUCCESS;
}
/* }}} */
		
int php_sock_array_to_fd_set(zval *sock_array, fd_set *fds, SOCKET *max_fd TSRMLS_DC) {
	zval		**element;
	php_socket	*php_sock;
	
	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(sock_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(sock_array), (void **) &element) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(sock_array))) {

		php_sock = (php_socket*) zend_fetch_resource(element TSRMLS_CC, -1, le_socket_name, NULL, 1, le_socket);
		if (!php_sock) continue; /* If element is not a resource, skip it */

		FD_SET(php_sock->bsd_socket, fds);
		if (php_sock->bsd_socket > *max_fd) {
			*max_fd = php_sock->bsd_socket;
		}
	}

	return 1;
}

int php_sock_array_from_fd_set(zval *sock_array, fd_set *fds TSRMLS_DC) {
	zval		**element;
	zval		**dest_element;
	php_socket	*php_sock;
	HashTable	*new_hash;
	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(sock_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(sock_array), (void **) &element) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(sock_array))) {

		php_sock = (php_socket*) zend_fetch_resource(element TSRMLS_CC, -1, le_socket_name, NULL, 1, le_socket);
		if (!php_sock) continue; /* If element is not a resource, skip it */

		if (FD_ISSET(php_sock->bsd_socket, fds)) {
			/* Add fd to new array */
			zend_hash_next_index_insert(new_hash, (void *)element, sizeof(zval *), (void **)&dest_element);
			if (dest_element) zval_add_ref(dest_element);
		}
	}

	/* Destroy old array, add new one */
	zend_hash_destroy(Z_ARRVAL_P(sock_array));
	efree(Z_ARRVAL_P(sock_array));

	zend_hash_internal_pointer_reset(new_hash);
	Z_ARRVAL_P(sock_array) = new_hash;

	return 1;
}


/* {{{ proto int socket_select(array &read_fds, array &write_fds, &array except_fds, int tv_sec[, int tv_usec])
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(socket_select)
{
	zval			*r_array, *w_array, *e_array, *sec;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	SOCKET			max_fd = 0;
	int				retval, sets = 0, usec = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a!a!a!z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE)
		return;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) sets += php_sock_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
	if (w_array != NULL) sets += php_sock_array_to_fd_set(w_array, &wfds, &max_fd TSRMLS_CC);
	if (e_array != NULL) sets += php_sock_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);

	if (!sets) {
		php_error(E_WARNING, "%s() no resource arrays were passed to select", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		convert_to_long_ex(&sec);
		tv.tv_sec = Z_LVAL_P(sec);
		tv.tv_usec = usec;
		tv_p = &tv;
	}

	retval = select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		SOCKETS_G(last_error) = errno;
		php_error(E_WARNING, "%s() %s [%d]: %s", get_active_function_name(TSRMLS_C), "unable to select", errno, php_strerror(errno TSRMLS_CC));
		RETURN_FALSE;
	}

	if (r_array != NULL) php_sock_array_from_fd_set(r_array, &rfds TSRMLS_CC);
	if (w_array != NULL) php_sock_array_from_fd_set(w_array, &wfds TSRMLS_CC);
	if (e_array != NULL) php_sock_array_from_fd_set(e_array, &efds TSRMLS_CC);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto resource socket_create_listen(int port[, int backlog])
   Opens a socket on port to accept connections */
PHP_FUNCTION(socket_create_listen)
{
	php_socket	*php_sock;
	int			port, backlog = 128;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &port, &backlog) == FAILURE)
		return;

	if (!open_listen_sock(&php_sock, port, backlog TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, php_sock, le_socket);
}
/* }}} */

/* {{{ proto resource socket_accept(resource socket)
   Accepts a connection on the listening socket fd */
PHP_FUNCTION(socket_accept)
{
	zval				*arg1;
	php_socket			*php_sock, *new_sock;
	struct sockaddr_in	sa;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	
	if (!accept_connect(php_sock, &new_sock, (struct sockaddr *) &sa TSRMLS_CC)) {
		php_error(E_WARNING, "%s() unable to accept socket connection [%d]: %s",
				  get_active_function_name(TSRMLS_C), errno, php_strerror(errno TSRMLS_CC));
		RETURN_FALSE;
	}
	
	ZEND_REGISTER_RESOURCE(return_value, new_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_set_nonblock(resource socket)
   Sets nonblocking mode on a socket resource */
PHP_FUNCTION(socket_set_nonblock)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			flags;
   
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE)
		return;
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	
	flags = fcntl(php_sock->bsd_socket, F_GETFL);

	/* Safely append non blocking to other flags unless the get fails.
	 * Note: This does not abort on failure becuse getfl will always fail
	 *       under the current win32 code. */
	if (flags > -1) flags |= O_NONBLOCK;
		else flags = O_NONBLOCK;
	
	if (fcntl(php_sock->bsd_socket, F_SETFL, flags) > -1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool socket_set_block(resource socket)
   Sets blocking mode on a socket resource */
PHP_FUNCTION(socket_set_block)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			flags;
   
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE)
		return;
	
	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	
	flags = fcntl(php_sock->bsd_socket, F_GETFL);

	/* Safely remove blocking from flags unless the get fails.
	 * Note: This does not abort on failure becuse getfl will always fail
	 *       under the current win32 code. */
	if (flags > -1) flags &= ~O_NONBLOCK;
		else flags = 0;
	
	if (fcntl(php_sock->bsd_socket, F_SETFL, flags) > -1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool socket_listen(resource socket[, int backlog])
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(socket_listen)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			backlog = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &arg1, &backlog) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	if (listen(php_sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to listen on socket", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void socket_close(resource socket)
   Closes a file descriptor */
PHP_FUNCTION(socket_close)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	zend_list_delete(Z_RESVAL_P(arg1));
}
/* }}} */

/* {{{ proto int socket_write(resource socket, string buf[, int length])
   Writes the buffer to the socket resource, length is optional */
PHP_FUNCTION(socket_write)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			retval, str_len, length;
	char		*str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &arg1, &str, &str_len, &length) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	if (ZEND_NUM_ARGS() < 4) {
		length = str_len;
	}

#ifndef PHP_WIN32
	retval = write(php_sock->bsd_socket, str, MIN(length, str_len));
#else
	retval = send(php_sock->bsd_socket, str, min(length, str_len), 0);
#endif

	if (retval < 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

typedef int (*read_func)(int, void *, size_t, int);

/* {{{ proto string socket_read(resource socket, int length [, int type])
   Reads a maximum of length bytes from socket */
PHP_FUNCTION(socket_read)
{
	zval		*arg1;
	php_socket	*php_sock;
	read_func	read_function = (read_func) recv;
	char		*tmpbuf;
	int			retval, length, type = PHP_BINARY_READ;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &arg1, &length, &type) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	if (type == PHP_NORMAL_READ) {
		read_function = (read_func) php_read;
	}

	tmpbuf = emalloc(length + 1);

	retval = (*read_function)(php_sock->bsd_socket, tmpbuf, length, 0);
	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		efree(tmpbuf);
		RETURN_FALSE;
	}
	
	tmpbuf = erealloc(tmpbuf, retval + 1);
	tmpbuf[ retval ] = '\0' ;

	RETURN_STRINGL(tmpbuf, retval, 0);
}
/* }}} */

/* {{{ proto bool socket_getsockname(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getsockname)
{
	zval					*arg1, *addr, *port = NULL;
	php_sockaddr_storage	sa_storage;
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &arg1, &addr, &port) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	sa = (struct sockaddr *) &sa_storage;

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket name", errno);
		RETURN_FALSE;
	}
	
	switch (sa->sa_family) {
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;
				
			zval_dtor(addr);
			ZVAL_STRING(addr, addr_string, 1);

			if (port != NULL) {
				zval_dtor(port);
				ZVAL_LONG(port, htons(sin->sin_port));
			}
			RETURN_TRUE;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			zval_dtor(addr);
			ZVAL_STRING(addr, s_un->sun_path, 1);
			RETURN_TRUE;

		default:
			php_error(E_WARNING, "%s() Unsupported address family %d",
					  get_active_function_name(TSRMLS_C), sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_getpeername(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getpeername)
{
	zval					*arg1, *arg2, *arg3 = NULL;
	php_sockaddr_storage	sa_storage;
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &arg1, &arg2, &arg3) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	sa = (struct sockaddr *) &sa_storage;

	if (getpeername(php_sock->bsd_socket, sa, &salen) < 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve peer name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;
			
			zval_dtor(arg2);
			ZVAL_STRING(arg2, addr_string, 1);

			if (arg3 != NULL) {
				zval_dtor(arg3);
				ZVAL_LONG(arg3, htons(sin->sin_port));
			}

			RETURN_TRUE;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			zval_dtor(arg2);
			ZVAL_STRING(arg2, s_un->sun_path, 1);
			RETURN_TRUE;

		default:
			php_error(E_WARNING, "%s() Unsupported address family %d",
					  get_active_function_name(TSRMLS_C), sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource socket_create(int domain, int type, int protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket_create)
{
	int			arg1, arg2, arg3;
	php_socket	*php_sock = (php_socket*)emalloc(sizeof(php_socket));

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &arg1, &arg2, &arg3) == FAILURE) {
		efree(php_sock);
		return;
    }

	if (arg1 != AF_UNIX && arg1 != AF_INET) {
		php_error(E_WARNING, "%s() invalid socket domain [%d] specified for argument 1, assuming AF_INET", get_active_function_name(TSRMLS_C), arg1);
		arg1 = AF_INET;
	}

	if (arg2 > 10) {
		php_error(E_WARNING, "%s() invalid socket type [%d] specified for argument 2, assuming SOCK_STREAM", get_active_function_name(TSRMLS_C), arg2);
		arg2 = SOCK_STREAM;
	}
	
	php_sock->bsd_socket = socket(arg1, arg2, arg3);
	php_sock->type = arg1;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error(E_WARNING, "%s() Unable to create socket [%d]: %s",
				  get_active_function_name(TSRMLS_C), errno, php_strerror(errno TSRMLS_CC));
		efree(php_sock);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, php_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_connect(resource socket, string addr [, int port])
   Opens a connection to addr:port on the socket specified by socket */
PHP_FUNCTION(socket_connect)
{
	zval				*arg1;
	php_socket			*php_sock;
	struct sockaddr_in	sin;
	struct sockaddr_un	s_un;
	char				*addr;
	int					retval, addr_len, port;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	switch(php_sock->type) {
		case AF_INET:
			if (ZEND_NUM_ARGS() != 3) {
				php_error(E_WARNING, "%s() Socket of type AF_INET requires 3 arguments",
						  get_active_function_name(TSRMLS_C));
				RETURN_FALSE;
			}

			sin.sin_family	= AF_INET;
			sin.sin_port	= htons((unsigned short int)port);
			
			if (! php_set_inet_addr(&sin, addr, php_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}

			retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
			break;

		case AF_UNIX:
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, 108, "%s", addr);
			retval = connect(php_sock->bsd_socket, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			break;

		default:
			php_error(E_WARNING, "%s() Unsupported socket type %d",
					  get_active_function_name(TSRMLS_C), php_sock->type);
			RETURN_FALSE;
		}	
	
	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to connect", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string socket_strerror(int errno)
   Returns a string describing an error */
PHP_FUNCTION(socket_strerror)
{
	int	arg1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &arg1) == FAILURE)
		return;

	RETURN_STRING(php_strerror(arg1 TSRMLS_CC), 1);
}
/* }}} */

/* {{{ proto bool socket_bind(resource socket, string addr [, int port])
   Binds an open socket to a listening port, port is only specified in AF_INET family. */
PHP_FUNCTION(socket_bind)
{
	zval					*arg1;
	php_sockaddr_storage	sa_storage;
	struct sockaddr			*sock_type = (struct sockaddr*) &sa_storage;
	php_socket				*php_sock;
	char					*addr;
	int						addr_len, port = 0;
	long					retval = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	
	switch(php_sock->type) {
		case AF_UNIX:
			{
				struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;
				memset(sa, 0, sizeof(sa_storage));
				sa->sun_family = AF_UNIX;
				snprintf(sa->sun_path, 108, "%s", addr);
				retval = bind(php_sock->bsd_socket, (struct sockaddr *) sa, SUN_LEN(sa));
				break;
			}
		
		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) sock_type;

				memset(sa, 0, sizeof(sa_storage)); /* Apparently, Mac OSX needs this */
				
				sa->sin_family = AF_INET;
				sa->sin_port = htons((unsigned short) port);
				
				if (! php_set_inet_addr(sa, addr, php_sock TSRMLS_CC)) {
					RETURN_FALSE;
				}
				
				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in));
				break;
			}
		
		default:
			php_error(E_WARNING, "%s() unsupported socket type '%d', must be AF_UNIX or AF_INET", get_active_function_name(TSRMLS_C), php_sock->type);
			RETURN_FALSE;
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to bind address", errno);
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
	zval			***args = (zval ***)NULL;
	php_iovec_t		*vector;
	struct iovec	*vector_array;
	int				i, j, num_vectors, argc = ZEND_NUM_ARGS();
	
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
	
	efree(args);

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
	zval			*iovec_id;
	php_iovec_t		*vector;
	unsigned int	iovec_position;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &iovec_id, &iovec_position) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &iovec_id, -1, le_iov_name, le_iov);

	if (iovec_position >= vector->count) {
		php_error(E_WARNING, "%s() can't access a vector position past the amount of vectors set in the array", get_active_function_name(TSRMLS_C));
		RETURN_EMPTY_STRING();
	}

	RETURN_STRINGL(vector->iov_array[iovec_position].iov_base, vector->iov_array[iovec_position].iov_len, 1);
}
/* }}} */

/* {{{ proto bool socket_iovec_set(resource iovec, int iovec_position, string new_val)
   Sets the data held in iovec_id[iovec_position] to new_val */
PHP_FUNCTION(socket_iovec_set)
{
	zval			*iovec_id;
	php_iovec_t		*vector;
	int				new_val_len;
	unsigned int	iovec_position;
	char			*new_val;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls", &iovec_id, &iovec_position, &new_val, &new_val_len) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &iovec_id, -1, le_iov_name, le_iov);

	if (iovec_position >= vector->count) {
		php_error(E_WARNING, "%s() can't access a vector position outside of the vector array bounds", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	if (vector->iov_array[iovec_position].iov_base) {
		efree(vector->iov_array[iovec_position].iov_base);
	}
	
	vector->iov_array[iovec_position].iov_base	= estrdup(new_val);
	vector->iov_array[iovec_position].iov_len	= strlen(new_val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_iovec_add(resource iovec, int iov_len)
   Adds a new vector to the scatter/gather array */
PHP_FUNCTION(socket_iovec_add)
{
	zval			*iovec_id;
	php_iovec_t		*vector;
	struct iovec	*vector_array;
	int				iov_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &iovec_id, &iov_len) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &iovec_id, -1, le_iov_name, le_iov);

	vector_array = (struct iovec*)emalloc(sizeof(struct iovec) * (vector->count + 2));
	memcpy(vector_array, vector->iov_array, sizeof(struct iovec) * vector->count);

	vector_array[vector->count].iov_base	= (char*)emalloc(iov_len);
	vector_array[vector->count].iov_len		= iov_len;
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
	zval			*iovec_id;
	php_iovec_t		*vector;
	struct iovec	*vector_array;
	unsigned int	i, iov_pos;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &iovec_id, &iov_pos) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &iovec_id, -1, le_iov_name, le_iov);

	if (iov_pos > vector->count) {
		php_error(E_WARNING, "%s() can't delete an IO vector that is out of array bounds", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	vector_array = emalloc(vector->count * sizeof(struct iovec));

	for (i = 0; i < vector->count; i++) {
		if (i < iov_pos) {
			memcpy(&(vector->iov_array[i]), &(vector_array[i]), sizeof(struct iovec));
		} else if (i > iov_pos) {
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
	zval		*iovec_id;
	php_iovec_t	*vector;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &iovec_id) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &iovec_id, -1, le_iov_name, le_iov);
	
	zend_list_delete(Z_RESVAL_P(iovec_id));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_readv(resource socket, resource iovec_id)
   Reads from an fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(socket_readv)
{
	zval		*arg1, *arg2;
	php_iovec_t	*vector;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &arg1, &arg2) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &arg2, -1, le_iov_name, le_iov);

	if (readv(php_sock->bsd_socket, vector->iov_array, vector->count) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_writev(resource socket, resource iovec_id)
   Writes to a file descriptor, fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(socket_writev)
{
	zval		*arg1, *arg2;
	php_iovec_t	*vector;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &arg1, &arg2) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, &arg2, -1, le_iov_name, le_iov);

	if (writev(php_sock->bsd_socket, vector->iov_array, vector->count) != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to write to socket", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int socket_recv(resource socket, string &buf, int len, int flags)
   Receives data from a connected socket */
PHP_FUNCTION(socket_recv)
{
	zval		*php_sock_res, *buf;
	char		*recv_buf;
	php_socket	*php_sock;
	int			retval, len, flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzll", &php_sock_res, &buf, &len, &flags) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &php_sock_res, -1, le_socket_name, le_socket);

	recv_buf = emalloc(len + 1);
	memset(recv_buf, 0, len + 1);

	if ((retval = recv(php_sock->bsd_socket, recv_buf, len, flags)) < 1) {
		efree(recv_buf);
		
		zval_dtor(buf);
		Z_TYPE_P(buf) = IS_NULL;
	} else {
		recv_buf[retval] = '\0';

		/* Rebuild buffer zval */
		zval_dtor(buf);
	
		Z_STRVAL_P(buf) = recv_buf;
		Z_STRLEN_P(buf) = retval;
		Z_TYPE_P(buf) = IS_STRING;
	}
	
	if (retval == -1) { 
		PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		RETURN_FALSE;
	}
	
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_send(resource socket, string buf, int len, int flags)
   Sends data to a connected socket */
PHP_FUNCTION(socket_send)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			buf_len, len, flags, retval;
	char		*buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &arg1, &buf, &buf_len, &len, &flags) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	retval = send(php_sock->bsd_socket, buf, (buf_len < len ? buf_len : len), flags);

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}
   
	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_recvfrom(resource socket, string &buf, int len, int flags, string &name [, int &port])
   Receives data from a socket, connected or not */
PHP_FUNCTION(socket_recvfrom)
{
	zval				*arg1, *arg2, *arg5, *arg6 = NULL;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
	socklen_t			slen;
	int					retval, arg3, arg4;
	char				*recv_buf, *address;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzllz|z", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	recv_buf = emalloc(arg3 + 2);
	memset(recv_buf, 0, arg3 + 2);
	
	switch (php_sock->type) {
		case AF_UNIX:
			slen = sizeof(s_un);
			s_un.sun_family = AF_UNIX;
			retval = recvfrom(php_sock->bsd_socket, recv_buf, arg3, arg4, (struct sockaddr *)&s_un, (socklen_t *)&slen);
			
			if (retval < 0) {
				efree(recv_buf);
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				RETURN_FALSE;
			}

			zval_dtor(arg2);
			zval_dtor(arg5);

			ZVAL_STRINGL(arg2, recv_buf, retval, 0);
			ZVAL_STRING(arg5, s_un.sun_path, 1);
			break;

		case AF_INET:
			slen = sizeof(sin);
			memset(&sin, 0, slen);
			sin.sin_family = AF_INET;
		
			if (arg6 == NULL) {
				WRONG_PARAM_COUNT;
			}
			
			retval = recvfrom(php_sock->bsd_socket, recv_buf, arg3, arg4, (struct sockaddr *)&sin, (socklen_t *)&slen);
			
			if (retval < 0) {
				efree(recv_buf);
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				RETURN_FALSE;
			}
			
			zval_dtor(arg2);
			zval_dtor(arg5);
			zval_dtor(arg6);

			address = inet_ntoa(sin.sin_addr);

			ZVAL_STRINGL(arg2, recv_buf, retval, 0); 
			ZVAL_STRING(arg5, address ? address : "0.0.0.0", 1);
			ZVAL_LONG(arg6, ntohs(sin.sin_port));
			break;

		default:
			php_error(E_WARNING, "%s() Unsupported socket type %d",
					  get_active_function_name(TSRMLS_C), php_sock->type);
			RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_sendto(resource socket, string buf, int len, int flags, string addr [, int port])
   Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(socket_sendto)
{
	zval				*arg1;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
	int					retval, buf_len, len, flags, addr_len, port = 0;
	char				*buf, *addr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslls|l", &arg1, &buf, &buf_len, &len, &flags, &addr, &addr_len, &port) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);


	switch (php_sock->type) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, 108, "%s", addr);

			retval = sendto(php_sock->bsd_socket, buf, (len > buf_len) ? buf_len : len,	flags, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			break;

		case AF_INET:
			if (ZEND_NUM_ARGS() != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons((unsigned short) port);
			
			if (! php_set_inet_addr(&sin, addr, php_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}
			
			retval = sendto(php_sock->bsd_socket, buf, (len > buf_len) ? buf_len : len, flags, (struct sockaddr *) &sin, sizeof(sin));
			break;

		default:
			php_error(E_WARNING, "%s() Unsupported socket type %d",
					  get_active_function_name(TSRMLS_C), php_sock->type);
			RETURN_FALSE;
	}

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool socket_recvmsg(resource socket, resource iovec, array &control, int &controllen, int &flags, string &addr [, int &port])
   Used to receive messages on a socket, whether connection-oriented or not */
PHP_FUNCTION(socket_recvmsg)
{
	zval					*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7 = NULL;
	php_iovec_t				*iov;
	struct msghdr			hdr;
	php_sockaddr_storage	sa_storage;
	php_socket				*php_sock;
	struct cmsghdr			*ctl_buf;
	struct sockaddr			*sa = (struct sockaddr *) &sa_storage;
	struct sockaddr_in		*sin = (struct sockaddr_in *) sa;
	struct sockaddr_un		*s_un = (struct sockaddr_un *) sa;
	socklen_t				salen = sizeof(sa_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrzzzz|z", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, &arg2, -1, le_iov_name, le_iov);

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to receive message", errno);
		RETURN_FALSE;
	}
	
	ctl_buf = (Z_LVAL_P(arg4) > sizeof(struct cmsghdr)) ? (struct cmsghdr*)emalloc(Z_LVAL_P(arg4)) : NULL;

	switch (sa->sa_family) {
		case AF_INET:
			
			if (arg7 == NULL) {
				efree(ctl_buf);
				WRONG_PARAM_COUNT;
			}
			
			memset(sa, 0, sizeof(sa_storage));
			hdr.msg_name	= (void *) sin;
			hdr.msg_namelen	= sizeof(sa_storage);
			hdr.msg_iov		= iov->iov_array;
			hdr.msg_iovlen	= iov->count;

			hdr.msg_control = ctl_buf ? (void *) ctl_buf : NULL;
			hdr.msg_controllen = ctl_buf ? Z_LVAL_P(arg4) : 0;
#ifndef MISSING_MSGHDR_MSGFLAGS
			hdr.msg_flags	= 0;
#endif

			if (recvmsg(php_sock->bsd_socket, &hdr, Z_LVAL_P(arg5)) < 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to receive message", errno);
				RETURN_FALSE;
			} else {
				struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;
				
				zval_dtor(arg3);
				zval_dtor(arg4);
				zval_dtor(arg5);
				zval_dtor(arg6);
				zval_dtor(arg7);
				
				ZVAL_LONG(arg4, hdr.msg_controllen);
#ifndef MISSING_MSGHDR_MSGFLAGS
				ZVAL_LONG(arg5, hdr.msg_flags);
#endif
				ZVAL_LONG(arg7, ntohs(sin->sin_port));
				
				if (array_init(arg3) == FAILURE) {
					php_error(E_WARNING, "%s() cannot intialize array", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
				}
				
				if (mhdr != NULL) {
					add_assoc_long(arg3,	"cmsg_level",	mhdr->cmsg_level);
					add_assoc_long(arg3,	"cmsg_type",	mhdr->cmsg_type);
					add_assoc_string(arg3,	"cmsg_data",	CMSG_DATA(mhdr), 1);
				}
				
				{
					char *tmp = inet_ntoa(sin->sin_addr);
					if (tmp == NULL) {
						ZVAL_STRING(arg6, "0.0.0.0", 1);
					} else {
						ZVAL_STRING(arg6, tmp, 1);
					}
				}
				
				RETURN_TRUE;
			}

	case AF_UNIX:
		memset(sa, 0, sizeof(sa_storage));
		hdr.msg_name	= (void *) s_un;
		hdr.msg_namelen	= sizeof(struct sockaddr_un);
		hdr.msg_iov		= iov->iov_array;
		hdr.msg_iovlen	= iov->count;
		
		if (ctl_buf) {
			hdr.msg_control = (void *) ctl_buf;
			hdr.msg_controllen = Z_LVAL_P(arg4);
		} else {
			hdr.msg_control = NULL;
			hdr.msg_controllen = 0;
		}
#ifndef MISSING_MSGHDR_MSGFLAGS		
		hdr.msg_flags = 0;
#endif 

		
		if (recvmsg(php_sock->bsd_socket, &hdr, Z_LVAL_P(arg5)) != 0) {
			PHP_SOCKET_ERROR(php_sock, "unable to receive message", errno);
			RETURN_FALSE;
		} else {
			struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;
			
			if (mhdr != NULL) {
				
				zval_dtor(arg3);
				zval_dtor(arg4);
				zval_dtor(arg5);
				zval_dtor(arg6);
				
				ZVAL_LONG(arg4, hdr.msg_controllen);
#ifndef MISSING_MSGHDR_MSGFLAGS
				ZVAL_LONG(arg5, hdr.msg_flags);
#endif
				
				if (array_init(arg3) == FAILURE) {
					php_error(E_WARNING, "%s() cannot initialize return value", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
				}
				
				add_assoc_long(arg3, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(arg3, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(arg3, "cmsg_data", CMSG_DATA(mhdr), 1);
			}
			
			
			ZVAL_STRING(arg6, s_un->sun_path, 1);
			RETURN_TRUE;
		}
		
	default:
		php_error(E_WARNING, "%s() Unsupported address family %d",
				  get_active_function_name(TSRMLS_C), sa->sa_family);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_sendmsg(resource socket, resource iovec, int flags, string addr [, int port])
   Sends a message to a socket, regardless of whether it is connection-oriented or not */
PHP_FUNCTION(socket_sendmsg)
{
	zval			*arg1, *arg2;
	php_iovec_t		*iov;
	php_socket		*php_sock;
	struct sockaddr	sa;
	char			*addr;
	socklen_t		salen;
	int				flags, addr_len, port;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrls|l", &arg1, &arg2, &flags, &addr, &addr_len, &port) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, &arg2, -1, le_iov_name, le_iov);

	salen = sizeof(sa);
	if (getsockname(php_sock->bsd_socket, &sa, &salen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to send messge", errno);
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
				hdr.msg_name = (void *) &sa;
				hdr.msg_namelen = sizeof(sa);
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;
				
				memset(sin, 0, sizeof(sa));
				
				sin->sin_family = AF_INET;
				sin->sin_port = htons((unsigned short)port);
				
				if (! php_set_inet_addr(sin, addr, php_sock TSRMLS_CC)) {
					RETURN_FALSE;
				}
				
				if (sendmsg(php_sock->bsd_socket, &hdr, flags) == -1) {
					PHP_SOCKET_ERROR(php_sock, "unable to send message", errno);
				}
				
				RETURN_TRUE;
			}
			
		case AF_UNIX:
			{
				struct msghdr hdr;
				struct sockaddr_un *s_un = (struct sockaddr_un *) &sa;
				
				set_errno(0);
				
				hdr.msg_name = (void *) s_un;
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;
				
				snprintf(s_un->sun_path, 108, "%s", addr);
				
				hdr.msg_namelen = SUN_LEN(s_un);
				
				if (sendmsg(php_sock->bsd_socket, &hdr, flags) == -1) {
					PHP_SOCKET_ERROR(php_sock, "unable to send message", errno);
					RETURN_FALSE;
				}
				
				RETURN_TRUE;
			}

		default:
			php_error(E_WARNING, "%s() Unsupported address family %d",
					  get_active_function_name(TSRMLS_C), sa.sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed socket_get_option(resource socket, int level, int optname)
   Gets socket options for the socket */
PHP_FUNCTION(socket_get_option)
{
	zval			*arg1;
	struct linger	linger_val;
	struct timeval		tv;
	socklen_t		optlen;
	php_socket		*php_sock;
	int				other_val, level, optname;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &arg1, &level, &optname) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);
	
	switch(optname) {
		case SO_LINGER: 
			optlen = sizeof(linger_val);

			if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&linger_val, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			if (array_init(return_value) == FAILURE) {
				RETURN_FALSE;
			}

			add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
			add_assoc_long(return_value, "l_linger", linger_val.l_linger);
	     
			break; 
		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
			optlen = sizeof(tv);

			if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&tv, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			if (array_init(return_value) == FAILURE) {
				RETURN_FALSE;
			}
			
			add_assoc_long(return_value, "sec", tv.tv_sec);
			add_assoc_long(return_value, "usec", tv.tv_usec);
			
			break;
		default:
			optlen = sizeof(other_val);
			
			if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&other_val, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			RETURN_LONG(other_val);
			break;
	}
}
/* }}} */

/* {{{ proto bool socket_set_option(resource socket, int level, int optname, int|array optval)
   Sets socket options for the socket */
PHP_FUNCTION(socket_set_option)
{
	zval			*arg1, *arg4;
	struct linger	lv;
	struct timeval tv;
	php_socket		*php_sock;
	int				ov, optlen, retval, level, optname;
	void 			*opt_ptr;
	
	HashTable 		*opt_ht;
	zval 			**l_onoff, **l_linger;
	zval 			**sec, **usec;
	
	/* key name constants */     
	char			*l_onoff_key = "l_onoff";
	char			*l_linger_key = "l_linger";
	char			*sec_key = "sec";
	char			*usec_key = "usec";

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllz", &arg1, &level, &optname, &arg4) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &arg1, -1, le_socket_name, le_socket);

	set_errno(0);

	switch (optname) {
		case SO_LINGER:
			convert_to_array_ex(&arg4);
			opt_ht = HASH_OF(arg4);

			if (zend_hash_find(opt_ht, l_onoff_key, strlen(l_onoff_key) + 1, (void **)&l_onoff) == FAILURE) {
				php_error(E_WARNING, "%s() no key \"%s\" passed in optval", get_active_function_name(TSRMLS_C), l_onoff_key);
				RETURN_FALSE;
			}
			if (zend_hash_find(opt_ht, l_linger_key, strlen(l_linger_key) + 1, (void **)&l_linger) == FAILURE) {
				php_error(E_WARNING, "%s() no key \"%s\" passed in optval", get_active_function_name(TSRMLS_C), l_linger_key);
				RETURN_FALSE;
			}

			convert_to_long_ex(l_onoff);
			convert_to_long_ex(l_linger);

			lv.l_onoff = (unsigned short)Z_LVAL_PP(l_onoff);
			lv.l_linger = (unsigned short)Z_LVAL_PP(l_linger);

			optlen = sizeof(lv);
			opt_ptr = &lv;
			break;
		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
			convert_to_array_ex(&arg4);
			opt_ht = HASH_OF(arg4);

			if (zend_hash_find(opt_ht, sec_key, strlen(sec_key) + 1, (void **)&sec) == FAILURE) {
				php_error(E_WARNING, "%s() no key \"%s\" passed in optval", get_active_function_name(TSRMLS_C), sec_key);
				RETURN_FALSE;
			}
			if (zend_hash_find(opt_ht, usec_key, strlen(usec_key) + 1, (void **)&usec) == FAILURE) {
				php_error(E_WARNING, "%s() no key \"%s\" passed in optval", get_active_function_name(TSRMLS_C), usec_key);
				RETURN_FALSE;
			}
			
			convert_to_long_ex(sec);
			convert_to_long_ex(usec);
			tv.tv_sec = Z_LVAL_PP(sec);
			tv.tv_usec = Z_LVAL_PP(usec);

			optlen = sizeof(tv);
			opt_ptr = &tv;
			break;
		default:
			convert_to_long_ex(&arg4);
			ov = Z_LVAL_P(arg4);
			
			optlen = sizeof(ov);
			opt_ptr = &ov;
			break;
	}

	retval = setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen);
	
	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to set socket option", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_create_pair(int domain, int type, int protocol, array &fd)
   Creates a pair of indistinguishable sockets and stores them in fds. */
PHP_FUNCTION(socket_create_pair)
{
	zval		*retval[2], *fds_array_zval;
	php_socket	*php_sock[2];
	SOCKET		fds_array[2];
	int			domain, type, protocol;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllz", &domain, &type, &protocol, &fds_array_zval) == FAILURE)
		return;

	php_sock[0] = (php_socket*)emalloc(sizeof(php_socket));
	php_sock[1] = (php_socket*)emalloc(sizeof(php_socket));

	if (domain != AF_INET && domain != AF_UNIX) {
		php_error(E_WARNING, "%s() invalid socket domain [%d] specified for argument 1, assuming AF_INET", get_active_function_name(TSRMLS_C), domain);
		domain = AF_INET;
	}
	
	if (type > 10) {
		php_error(E_WARNING, "%s() invalid socket type [%d] specified for argument 2, assuming SOCK_STREAM", get_active_function_name(TSRMLS_C), type);
		type = SOCK_STREAM;
	}
	
	if (socketpair(domain, type, protocol, fds_array) != 0) {
		SOCKETS_G(last_error) = errno;
		php_error(E_WARNING, "%s() unable to create socket pair [%d]: %s", get_active_function_name(TSRMLS_C), errno, php_strerror(errno TSRMLS_CC));
		efree(php_sock[0]);
		efree(php_sock[1]);
		RETURN_FALSE;
	}

	zval_dtor(fds_array_zval);
	if (array_init(fds_array_zval) == FAILURE) {
		php_error(E_WARNING, "%s() can't initialize array for 4th argument", get_active_function_name(TSRMLS_C));
		efree(php_sock[0]);
		efree(php_sock[1]);
		RETURN_FALSE;
	}

	MAKE_STD_ZVAL(retval[0]);
	MAKE_STD_ZVAL(retval[1]);

	php_sock[0]->bsd_socket = fds_array[0];
	php_sock[1]->bsd_socket = fds_array[1];
	php_sock[0]->type		= domain;
	php_sock[1]->type		= domain;

	ZEND_REGISTER_RESOURCE(retval[0], php_sock[0], le_socket);
	ZEND_REGISTER_RESOURCE(retval[1], php_sock[1], le_socket);

	add_index_zval(fds_array_zval, 0, retval[0]);
	add_index_zval(fds_array_zval, 1, retval[1]);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool socket_shutdown(resource socket[, int how])
   Shuts down a socket for receiving, sending, or both. */
PHP_FUNCTION(socket_shutdown)
{
	zval		*arg1;
	int			how_shutdown = 2;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &arg1, &how_shutdown) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(php_sock, php_socket*, &arg1, -1, le_socket_name, le_socket);
	
	if (shutdown(php_sock->bsd_socket, how_shutdown) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to shutdown socket", errno);
		RETURN_FALSE;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int socket_last_error([resource socket])
   Returns the last socket error (either the last used or the provided socket resource) */
PHP_FUNCTION(socket_last_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &arg1) == FAILURE)
		return;

	if (arg1) {
		ZEND_FETCH_RESOURCE(php_sock, php_socket*, &arg1, -1, le_socket_name, le_socket);
		RETVAL_LONG(php_sock->error);
	} else {
		RETVAL_LONG(SOCKETS_G(last_error));
	}
}
/* }}} */

/* {{{ proto void socket_clear_error([resource socket])
   Clears the error on the socket or the last error code. */
PHP_FUNCTION(socket_clear_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &arg1) == FAILURE)
		return;

	if (arg1) {
		ZEND_FETCH_RESOURCE(php_sock, php_socket*, &arg1, -1, le_socket_name, le_socket);
		php_sock->error = 0;
	} else {
		SOCKETS_G(last_error) = 0;
	}

	return;
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
