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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |          Jason Greene     <jason@php.net>                            |
   |          Gustavo Lopes    <cataphract@php.net>                       |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_network.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#ifdef PHP_WIN32
# include "windows_common.h"
# include <windows.h>
# include <Ws2tcpip.h>
# include "php_sockets.h"
# include <win32/sockets.h>
# include <win32/winutil.h>
#else
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
# define set_errno(a) (errno = a)
# include "php_sockets.h"
# if HAVE_IF_NAMETOINDEX
#  include <net/if.h>
# endif
# if defined(HAVE_LINUX_SOCK_DIAG_H)
#  include <linux/sock_diag.h>
# else
#  undef SO_MEMINFO
# endif
# if defined(HAVE_LINUX_FILTER_H)
#  include <linux/filter.h>
# else
#  undef SO_BPF_EXTENSIONS
# endif
#endif

#include <stddef.h>

#include "sockaddr_conv.h"
#include "multicast.h"
#include "sendrecvmsg.h"
#include "sockets_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(sockets)

#define SUN_LEN_NO_UB(su) (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
/* The SUN_LEN macro does pointer arithmetics on NULL which triggers errors in the Clang UBSAN build */
#ifdef __has_feature
# if __has_feature(undefined_behavior_sanitizer)
#  undef SUN_LEN
#  define SUN_LEN(su) SUN_LEN_NO_UB(su)
# endif
#endif
#ifndef SUN_LEN
# define SUN_LEN(su) SUN_LEN_NO_UB(su)
#endif

#ifndef PF_INET
#define PF_INET AF_INET
#endif

static PHP_GINIT_FUNCTION(sockets);
static PHP_GSHUTDOWN_FUNCTION(sockets);
static PHP_MINIT_FUNCTION(sockets);
static PHP_MSHUTDOWN_FUNCTION(sockets);
static PHP_MINFO_FUNCTION(sockets);
static PHP_RSHUTDOWN_FUNCTION(sockets);

/* Socket class */

zend_class_entry *socket_ce;
static zend_object_handlers socket_object_handlers;

static zend_object *socket_create_object(zend_class_entry *class_type) {
	php_socket *intern = zend_object_alloc(sizeof(php_socket), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->bsd_socket = -1; /* invalid socket */
	intern->type		 = PF_UNSPEC;
	intern->error		 = 0;
	intern->blocking	 = 1;
	ZVAL_UNDEF(&intern->zstream);

	return &intern->std;
}

static zend_function *socket_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct Socket, use socket_create() instead");
	return NULL;
}

static void socket_free_obj(zend_object *object)
{
	php_socket *socket = socket_from_obj(object);

	if (Z_ISUNDEF(socket->zstream)) {
		if (!IS_INVALID_SOCKET(socket)) {
			close(socket->bsd_socket);
		}
	} else {
		zval_ptr_dtor(&socket->zstream);
	}

	zend_object_std_dtor(&socket->std);
}

static HashTable *socket_get_gc(zend_object *object, zval **table, int *n)
{
	php_socket *socket = socket_from_obj(object);

	*table = &socket->zstream;
	*n = 1;

	return zend_std_get_properties(object);
}

/* AddressInfo class */

typedef struct {
	struct addrinfo addrinfo;
	zend_object std;
} php_addrinfo;

zend_class_entry *address_info_ce;
static zend_object_handlers address_info_object_handlers;

static inline php_addrinfo *address_info_from_obj(zend_object *obj) {
	return (php_addrinfo *)((char *)(obj) - XtOffsetOf(php_addrinfo, std));
}

#define Z_ADDRESS_INFO_P(zv) address_info_from_obj(Z_OBJ_P(zv))

static zend_object *address_info_create_object(zend_class_entry *class_type) {
	php_addrinfo *intern = zend_object_alloc(sizeof(php_addrinfo), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *address_info_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct AddressInfo, use socket_addrinfo_lookup() instead");
	return NULL;
}

static void address_info_free_obj(zend_object *object)
{
	php_addrinfo *address_info = address_info_from_obj(object);

	if (address_info->addrinfo.ai_canonname != NULL) {
		efree(address_info->addrinfo.ai_canonname);
	}
	efree(address_info->addrinfo.ai_addr);

	zend_object_std_dtor(&address_info->std);
}

/* Module registration */

zend_module_entry sockets_module_entry = {
	STANDARD_MODULE_HEADER,
	"sockets",
	ext_functions,
	PHP_MINIT(sockets),
	PHP_MSHUTDOWN(sockets),
	NULL,
	PHP_RSHUTDOWN(sockets),
	PHP_MINFO(sockets),
	PHP_SOCKETS_VERSION,
	PHP_MODULE_GLOBALS(sockets),
	PHP_GINIT(sockets),
	PHP_GSHUTDOWN(sockets),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_SOCKETS
#ifdef ZTS
	ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(sockets)
#endif

static bool php_open_listen_sock(php_socket *sock, int port, int backlog) /* {{{ */
{
	struct sockaddr_in  la;
	struct hostent		*hp;

#ifndef PHP_WIN32
	if ((hp = php_network_gethostbyname("0.0.0.0")) == NULL) {
#else
	if ((hp = php_network_gethostbyname("localhost")) == NULL) {
#endif
		return 0;
	}

	memcpy((char *) &la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons((unsigned short) port);

	sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);
	sock->blocking = 1;

	if (IS_INVALID_SOCKET(sock)) {
		PHP_SOCKET_ERROR(sock, "unable to create listening socket", errno);
		return 0;
	}

	sock->type = PF_INET;

	if (bind(sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to bind to given address", errno);
		close(sock->bsd_socket);
		return 0;
	}

	if (listen(sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to listen on socket", errno);
		close(sock->bsd_socket);
		return 0;
	}

	return 1;
}
/* }}} */

static bool php_accept_connect(php_socket *in_sock, php_socket *out_sock, struct sockaddr *la, socklen_t *la_len) /* {{{ */
{
	out_sock->bsd_socket = accept(in_sock->bsd_socket, la, la_len);

	if (IS_INVALID_SOCKET(out_sock)) {
		PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		return 0;
	}

	out_sock->error = 0;
	out_sock->blocking = 1;
	out_sock->type = la->sa_family;

	return 1;
}
/* }}} */

/* {{{ php_read -- wrapper around read() so that it only reads to a \r or \n. */
static int php_read(php_socket *sock, void *buf, size_t maxlen, int flags)
{
	int m = 0;
	size_t n = 0;
	int no_read = 0;
	int nonblock = 0;
	char *t = (char *) buf;

#ifndef PHP_WIN32
	m = fcntl(sock->bsd_socket, F_GETFL);
	if (m < 0) {
		return m;
	}
	nonblock = (m & O_NONBLOCK);
	m = 0;
#else
	nonblock = !sock->blocking;
#endif
	set_errno(0);

	*t = '\0';
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
			m = recv(sock->bsd_socket, (void *) t, 1, flags);
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

char *sockets_strerror(int error) /* {{{ */
{
	const char *buf;

#ifndef PHP_WIN32
	if (error < -10000) {
		error = -error - 10000;

#ifdef HAVE_HSTRERROR
		buf = hstrerror(error);
#else
		{
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			spprintf(&(SOCKETS_G(strerror_buf)), 0, "Host lookup error %d", error);
			buf = SOCKETS_G(strerror_buf);
		}
#endif
	} else {
		buf = strerror(error);
	}
#else
	{
		char *tmp = php_win32_error_to_msg(error);
		buf = NULL;

		if (tmp[0]) {
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			SOCKETS_G(strerror_buf) = estrdup(tmp);
			free(tmp);

			buf = SOCKETS_G(strerror_buf);
		}
	}
#endif

	return (buf ? (char *) buf : "");
}
/* }}} */

#ifdef PHP_WIN32
static void sockets_destroy_wsa_info(zval *data)
{/*{{{*/
	HANDLE h = (HANDLE)Z_PTR_P(data);
	CloseHandle(h);
}/*}}}*/
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(sockets)
{
#if defined(COMPILE_DL_SOCKETS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	sockets_globals->last_error = 0;
	sockets_globals->strerror_buf = NULL;
#ifdef PHP_WIN32
	sockets_globals->wsa_child_count = 0;
	zend_hash_init(&sockets_globals->wsa_info, 0, NULL, sockets_destroy_wsa_info, 1);
#endif
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(sockets)
{
#ifdef PHP_WIN32
	zend_hash_destroy(&sockets_globals->wsa_info);
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(sockets)
{
#if defined(COMPILE_DL_SOCKETS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	socket_ce = register_class_Socket();
	socket_ce->create_object = socket_create_object;
	socket_ce->default_object_handlers = &socket_object_handlers;

	memcpy(&socket_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	socket_object_handlers.offset = XtOffsetOf(php_socket, std);
	socket_object_handlers.free_obj = socket_free_obj;
	socket_object_handlers.get_constructor = socket_get_constructor;
	socket_object_handlers.clone_obj = NULL;
	socket_object_handlers.get_gc = socket_get_gc;
	socket_object_handlers.compare = zend_objects_not_comparable;

	address_info_ce = register_class_AddressInfo();
	address_info_ce->create_object = address_info_create_object;
	address_info_ce->default_object_handlers = &address_info_object_handlers;

	memcpy(&address_info_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	address_info_object_handlers.offset = XtOffsetOf(php_addrinfo, std);
	address_info_object_handlers.free_obj = address_info_free_obj;
	address_info_object_handlers.get_constructor = address_info_get_constructor;
	address_info_object_handlers.clone_obj = NULL;
	address_info_object_handlers.compare = zend_objects_not_comparable;

	register_sockets_symbols(module_number);

	php_socket_sendrecvmsg_init(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(sockets)
{
	php_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(sockets)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Sockets Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
static PHP_RSHUTDOWN_FUNCTION(sockets)
{
	if (SOCKETS_G(strerror_buf)) {
		efree(SOCKETS_G(strerror_buf));
		SOCKETS_G(strerror_buf) = NULL;
	}

	return SUCCESS;
}
/* }}} */

static int php_sock_array_to_fd_set(uint32_t arg_num, zval *sock_array, fd_set *fds, PHP_SOCKET *max_fd) /* {{{ */
{
	zval		*element;
	php_socket	*php_sock;
	int			num = 0;

	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(sock_array), element) {
		ZVAL_DEREF(element);

		if (Z_TYPE_P(element) != IS_OBJECT || Z_OBJCE_P(element) != socket_ce) {
			zend_argument_type_error(arg_num, "must only have elements of type Socket, %s given", zend_zval_value_name(element));
			return -1;
		}

		php_sock = Z_SOCKET_P(element);
		if (IS_INVALID_SOCKET(php_sock)) {
			zend_argument_type_error(arg_num, "contains a closed socket");
			return -1;
		}

		PHP_SAFE_FD_SET(php_sock->bsd_socket, fds);
		if (php_sock->bsd_socket > *max_fd) {
			*max_fd = php_sock->bsd_socket;
		}
		num++;
	} ZEND_HASH_FOREACH_END();

	return num ? 1 : 0;
}
/* }}} */

static void php_sock_array_from_fd_set(zval *sock_array, fd_set *fds) /* {{{ */
{
	zval		*element;
	zval		*dest_element;
	php_socket	*php_sock;
	zval		new_hash;
	zend_ulong  num_key;
	zend_string *key;

	ZEND_ASSERT(Z_TYPE_P(sock_array) == IS_ARRAY);

	array_init(&new_hash);
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(sock_array), num_key, key, element) {
		ZVAL_DEREF(element);

		php_sock = Z_SOCKET_P(element);
		ZEND_ASSERT(php_sock); /* element is supposed to be Socket object */
		ZEND_ASSERT(!IS_INVALID_SOCKET(php_sock));

		if (PHP_SAFE_FD_ISSET(php_sock->bsd_socket, fds)) {
			/* Add fd to new array */
			if (key) {
				dest_element = zend_hash_add(Z_ARRVAL(new_hash), key, element);
			} else {
				dest_element = zend_hash_index_update(Z_ARRVAL(new_hash), num_key, element);
			}
			if (dest_element) {
				Z_ADDREF_P(dest_element);
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* Destroy old array, add new one */
	zval_ptr_dtor(sock_array);

	ZVAL_COPY_VALUE(sock_array, &new_hash);
}
/* }}} */

/* {{{ Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(socket_select)
{
	zval			*r_array, *w_array, *e_array;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	PHP_SOCKET		max_fd = 0;
	int				retval, sets = 0;
	zend_long		sec, usec = 0;
	bool		sec_is_null = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a!a!a!l!|l", &r_array, &w_array, &e_array, &sec, &sec_is_null, &usec) == FAILURE) {
		RETURN_THROWS();
	}

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		sets += retval = php_sock_array_to_fd_set(1, r_array, &rfds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
	}
	if (w_array != NULL) {
		sets += retval = php_sock_array_to_fd_set(2, w_array, &wfds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
	}
	if (e_array != NULL) {
		sets += retval = php_sock_array_to_fd_set(3, e_array, &efds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
	}

	if (!sets) {
		zend_value_error("socket_select(): At least one array argument must be passed");
		RETURN_THROWS();
	}

	if (!PHP_SAFE_MAX_FD(max_fd, 0)) {
		RETURN_FALSE;
	}

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (!sec_is_null) {
		/* Solaris + BSD do not like microsecond values which are >= 1 sec */
		if (usec > 999999) {
			tv.tv_sec = sec + (usec / 1000000);
			tv.tv_usec = usec % 1000000;
		} else {
			tv.tv_sec = sec;
			tv.tv_usec = usec;
		}

		tv_p = &tv;
	}

	retval = select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to select [%d]: %s", errno, sockets_strerror(errno));
		RETURN_FALSE;
	}

	if (r_array != NULL) php_sock_array_from_fd_set(r_array, &rfds);
	if (w_array != NULL) php_sock_array_from_fd_set(w_array, &wfds);
	if (e_array != NULL) php_sock_array_from_fd_set(e_array, &efds);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Opens a socket on port to accept connections */
PHP_FUNCTION(socket_create_listen)
{
	php_socket	*php_sock;
	zend_long		port, backlog = 128;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &port, &backlog) == FAILURE) {
		RETURN_THROWS();
	}

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	if (!php_open_listen_sock(php_sock, port, backlog)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;
}
/* }}} */

/* {{{ Accepts a connection on the listening socket fd */
PHP_FUNCTION(socket_accept)
{
	zval			 *arg1;
	php_socket			 *php_sock, *new_sock;
	php_sockaddr_storage sa;
	socklen_t			 php_sa_len = sizeof(sa);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	object_init_ex(return_value, socket_ce);
	new_sock = Z_SOCKET_P(return_value);

	if (!php_accept_connect(php_sock, new_sock, (struct sockaddr*)&sa, &php_sa_len)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Sets nonblocking mode on a socket resource */
PHP_FUNCTION(socket_set_nonblock)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (!Z_ISUNDEF(php_sock->zstream)) {
		php_stream *stream;
		/* omit notice if resource doesn't exist anymore */
		stream = zend_fetch_resource2_ex(&php_sock->zstream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream != NULL) {
			if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, 0,
					NULL) != -1) {
				php_sock->blocking = 0;
				RETURN_TRUE;
			}
		}
	}

	if (php_set_sock_blocking(php_sock->bsd_socket, 0) == SUCCESS) {
		php_sock->blocking = 0;
		RETURN_TRUE;
	} else {
		PHP_SOCKET_ERROR(php_sock, "unable to set nonblocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Sets blocking mode on a socket resource */
PHP_FUNCTION(socket_set_block)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	/* if socket was created from a stream, give the stream a chance to take
	 * care of the operation itself, thereby allowing it to update its internal
	 * state */
	if (!Z_ISUNDEF(php_sock->zstream)) {
		php_stream *stream;
		stream = zend_fetch_resource2_ex(&php_sock->zstream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream != NULL) {
			if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, 1,
					NULL) != -1) {
				php_sock->blocking = 1;
				RETURN_TRUE;
			}
		}
	}

	if (php_set_sock_blocking(php_sock->bsd_socket, 1) == SUCCESS) {
		php_sock->blocking = 1;
		RETURN_TRUE;
	} else {
		PHP_SOCKET_ERROR(php_sock, "unable to set blocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(socket_listen)
{
	zval		*arg1;
	php_socket	*php_sock;
	zend_long		backlog = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &arg1, socket_ce, &backlog) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (listen(php_sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to listen on socket", errno);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Closes a file descriptor */
PHP_FUNCTION(socket_close)
{
	zval *arg1;
	php_socket *php_socket;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	php_socket = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_socket);

	if (!Z_ISUNDEF(php_socket->zstream)) {
		php_stream *stream = NULL;
		php_stream_from_zval_no_verify(stream, &php_socket->zstream);
		if (stream != NULL) {
			/* close & destroy stream, incl. removing it from the rsrc list;
			 * resource stored in php_sock->zstream will become invalid */
			php_stream_free(stream,
					PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_CLOSE |
					(stream->is_persistent?PHP_STREAM_FREE_CLOSE_PERSISTENT:0));
		}
	} else {
		if (!IS_INVALID_SOCKET(php_socket)) {
			close(php_socket->bsd_socket);
		}
	}

	ZVAL_UNDEF(&php_socket->zstream);
	php_socket->bsd_socket = -1;
}
/* }}} */

/* {{{ Writes the buffer to the socket resource, length is optional */
PHP_FUNCTION(socket_write)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			retval;
	size_t      str_len;
	zend_long	length = 0;
	bool   length_is_null = 1;
	char		*str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os|l!", &arg1, socket_ce, &str, &str_len, &length, &length_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (length < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (length_is_null) {
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

/* {{{ Reads a maximum of length bytes from socket */
PHP_FUNCTION(socket_read)
{
	zval		*arg1;
	php_socket	*php_sock;
	zend_string	*tmpbuf;
	int			retval;
	zend_long		length, type = PHP_BINARY_READ;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|l", &arg1, socket_ce, &length, &type) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	/* overflow check */
	if ((length + 1) < 2) {
		RETURN_FALSE;
	}

	tmpbuf = zend_string_alloc(length, 0);

	if (type == PHP_NORMAL_READ) {
		retval = php_read(php_sock, ZSTR_VAL(tmpbuf), length, 0);
	} else {
		retval = recv(php_sock->bsd_socket, ZSTR_VAL(tmpbuf), length, 0);
	}

	if (retval == -1) {
		/* if the socket is in non-blocking mode and there's no data to read,
		don't output any error, as this is a normal situation, and not an error */
		if (PHP_IS_TRANSIENT_ERROR(errno)) {
			php_sock->error = errno;
			SOCKETS_G(last_error) = errno;
		} else {
			PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		}

		zend_string_efree(tmpbuf);
		RETURN_FALSE;
	} else if (!retval) {
		zend_string_efree(tmpbuf);
		RETURN_EMPTY_STRING();
	}

	tmpbuf = zend_string_truncate(tmpbuf, retval, 0);
	ZSTR_LEN(tmpbuf) = retval;
	ZSTR_VAL(tmpbuf)[ZSTR_LEN(tmpbuf)] = '\0' ;

	RETURN_NEW_STR(tmpbuf);
}
/* }}} */

/* {{{ Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getsockname)
{
	zval					*arg1, *addr, *port = NULL;
	php_sockaddr_storage	sa_storage = {0};
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6		*sin6;
#endif
	char					addrbuf[INET6_ADDRSTRLEN];
	struct sockaddr_un		*s_un;
	const char				*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oz|z", &arg1, socket_ce, &addr, &port) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	sa = (struct sockaddr *) &sa_storage;

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#ifdef HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr,  addrbuf, sizeof(addrbuf));
			ZEND_TRY_ASSIGN_REF_STRING(addr, addrbuf);

			if (port != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(port, htons(sin6->sin6_port));
			}
			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			addr_string = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, sizeof(addrbuf));
			ZEND_TRY_ASSIGN_REF_STRING(addr, addr_string);

			if (port != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(port, htons(sin->sin_port));
			}
			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(addr, s_un->sun_path);
			RETURN_TRUE;
			break;

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getpeername)
{
	zval					*arg1, *arg2, *arg3 = NULL;
	php_sockaddr_storage	sa_storage = {0};
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6		*sin6;
#endif
	char					addrbuf[INET6_ADDRSTRLEN];
	struct sockaddr_un		*s_un;
	const char				*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oz|z", &arg1, socket_ce, &arg2, &arg3) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	sa = (struct sockaddr *) &sa_storage;

	if (getpeername(php_sock->bsd_socket, sa, &salen) < 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve peer name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#ifdef HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr, addrbuf, sizeof(addrbuf));

			ZEND_TRY_ASSIGN_REF_STRING(arg2, addrbuf);

			if (arg3 != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(arg3, htons(sin6->sin6_port));
			}

			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			addr_string = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, sizeof(addrbuf));
			ZEND_TRY_ASSIGN_REF_STRING(arg2, addr_string);

			if (arg3 != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(arg3, htons(sin->sin_port));
			}

			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(arg2, s_un->sun_path);
			RETURN_TRUE;
			break;

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket_create)
{
	zend_long	domain, type, protocol;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll", &domain, &type, &protocol) == FAILURE) {
		RETURN_THROWS();
	}

	if (domain != AF_UNIX
#ifdef HAVE_IPV6
		&& domain != AF_INET6
#endif
		&& domain != AF_INET) {
		zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET6, or AF_INET");
		RETURN_THROWS();
	}

	if (type > 10) {
		zend_argument_value_error(2, "must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET,"
			" SOCK_RAW, or SOCK_RDM");
		RETURN_THROWS();
	}

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	php_sock->bsd_socket = socket(domain, type, protocol);
	php_sock->type = domain;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;
}
/* }}} */

/* {{{ Opens a connection to addr:port on the socket specified by socket */
PHP_FUNCTION(socket_connect)
{
	zval				*resource_socket;
	php_socket			*php_sock;
	char				*addr;
	int					retval;
	size_t              addr_len;
	zend_long				port;
	bool				port_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os|l!", &resource_socket, socket_ce, &addr, &addr_len, &port, &port_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(resource_socket);
	ENSURE_SOCKET_VALID(php_sock);

	switch(php_sock->type) {
#ifdef HAVE_IPV6
		case AF_INET6: {
			struct sockaddr_in6 sin6 = {0};

			if (port_is_null) {
				zend_argument_value_error(3, "cannot be null when the socket type is AF_INET6");
				RETURN_THROWS();
			}

			memset(&sin6, 0, sizeof(struct sockaddr_in6));

			sin6.sin6_family = AF_INET6;
			sin6.sin6_port   = htons((unsigned short int)port);

			if (! php_set_inet6_addr(&sin6, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin6, sizeof(struct sockaddr_in6));
			break;
		}
#endif
		case AF_INET: {
			struct sockaddr_in sin = {0};

			if (port_is_null) {
				zend_argument_value_error(3, "cannot be null when the socket type is AF_INET");
				RETURN_THROWS();
			}

			sin.sin_family = AF_INET;
			sin.sin_port   = htons((unsigned short int)port);

			if (! php_set_inet_addr(&sin, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
			break;
		}

		case AF_UNIX: {
			struct sockaddr_un s_un = {0};

			if (addr_len >= sizeof(s_un.sun_path)) {
				zend_argument_value_error(2, "must be less than %d", sizeof(s_un.sun_path));
				RETURN_THROWS();
			}

			s_un.sun_family = AF_UNIX;
			memcpy(&s_un.sun_path, addr, addr_len);
			retval = connect(php_sock->bsd_socket, (struct sockaddr *) &s_un,
				(socklen_t)(XtOffsetOf(struct sockaddr_un, sun_path) + addr_len));
			break;
		}

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
		}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to connect", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns a string describing an error */
PHP_FUNCTION(socket_strerror)
{
	zend_long	arg1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &arg1) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_STRING(sockets_strerror(arg1));
}
/* }}} */

/* {{{ Binds an open socket to a listening port, port is only specified in AF_INET family. */
PHP_FUNCTION(socket_bind)
{
	zval					*arg1;
	php_sockaddr_storage	sa_storage = {0};
	struct sockaddr			*sock_type = (struct sockaddr*) &sa_storage;
	php_socket				*php_sock;
	char					*addr;
	size_t						addr_len;
	zend_long					port = 0;
	zend_long					retval = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os|l", &arg1, socket_ce, &addr, &addr_len, &port) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	switch(php_sock->type) {
		case AF_UNIX:
			{
				struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;

				sa->sun_family = AF_UNIX;

				if (addr_len >= sizeof(sa->sun_path)) {
					zend_argument_value_error(2, "must be less than %d", sizeof(sa->sun_path));
					RETURN_THROWS();
				}
				memcpy(&sa->sun_path, addr, addr_len);

				retval = bind(php_sock->bsd_socket, (struct sockaddr *) sa,
						offsetof(struct sockaddr_un, sun_path) + addr_len);
				break;
			}

		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) sock_type;

				sa->sin_family = AF_INET;
				sa->sin_port = htons((unsigned short) port);

				if (! php_set_inet_addr(sa, addr, php_sock)) {
					RETURN_FALSE;
				}

				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in));
				break;
			}
#ifdef HAVE_IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sa = (struct sockaddr_in6 *) sock_type;

				sa->sin6_family = AF_INET6;
				sa->sin6_port = htons((unsigned short) port);

				if (! php_set_inet6_addr(sa, addr, php_sock)) {
					RETURN_FALSE;
				}

				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in6));
				break;
			}
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to bind address", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Receives data from a connected socket */
PHP_FUNCTION(socket_recv)
{
	zval		*php_sock_res, *buf;
	zend_string	*recv_buf;
	php_socket	*php_sock;
	int			retval;
	zend_long		len, flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ozll", &php_sock_res, socket_ce, &buf, &len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(php_sock_res);
	ENSURE_SOCKET_VALID(php_sock);

	/* overflow check */
	if ((len + 1) < 2) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(len, 0);

	if ((retval = recv(php_sock->bsd_socket, ZSTR_VAL(recv_buf), len, flags)) < 1) {
		zend_string_efree(recv_buf);
		ZEND_TRY_ASSIGN_REF_NULL(buf);
	} else {
		ZSTR_LEN(recv_buf) = retval;
		ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(buf, recv_buf);
	}

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "Unable to read from socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Sends data to a connected socket */
PHP_FUNCTION(socket_send)
{
	zval		*arg1;
	php_socket	*php_sock;
	size_t			buf_len, retval;
	zend_long		len, flags;
	char		*buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Osll", &arg1, socket_ce, &buf, &buf_len, &len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	retval = send(php_sock->bsd_socket, buf, (buf_len < (size_t)len ? buf_len : (size_t)len), flags);

	if (retval == (size_t)-1) {
		PHP_SOCKET_ERROR(php_sock, "Unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Receives data from a socket, connected or not */
PHP_FUNCTION(socket_recvfrom)
{
	zval				*arg1, *arg2, *arg5, *arg6 = NULL;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
	char				addrbuf[INET6_ADDRSTRLEN];
	socklen_t			slen;
	int					retval;
	zend_long				arg3, arg4;
	const char			*address;
	zend_string			*recv_buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ozllz|z", &arg1, socket_ce, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	/* overflow check */
	/* Shouldthrow ? */
	if ((arg3 + 2) < 3) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(arg3 + 1, 0);

	switch (php_sock->type) {
		case AF_UNIX:
			slen = sizeof(s_un);
			memset(&s_un, 0, slen);
			s_un.sun_family = AF_UNIX;

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&s_un, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, s_un.sun_path);
			break;

		case AF_INET:
			slen = sizeof(sin);
			memset(&sin, 0, slen);
			sin.sin_family = AF_INET;

			if (arg6 == NULL) {
				zend_string_efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&sin, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			address = inet_ntop(AF_INET, &sin.sin_addr, addrbuf, sizeof(addrbuf));

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, address ? address : "0.0.0.0");
			ZEND_TRY_ASSIGN_REF_LONG(arg6, ntohs(sin.sin_port));
			break;
#ifdef HAVE_IPV6
		case AF_INET6:
			slen = sizeof(sin6);
			memset(&sin6, 0, slen);
			sin6.sin6_family = AF_INET6;

			if (arg6 == NULL) {
				zend_string_efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&sin6, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			memset(addrbuf, 0, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &sin6.sin6_addr,  addrbuf, sizeof(addrbuf));

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, addrbuf[0] ? addrbuf : "::");
			ZEND_TRY_ASSIGN_REF_LONG(arg6, ntohs(sin6.sin6_port));
			break;
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(socket_sendto)
{
	zval				*arg1;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
	int					retval;
	size_t              buf_len, addr_len;
	zend_long			len, flags, port;
	bool           port_is_null = 1;
	char				*buf, *addr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oslls|l!", &arg1, socket_ce, &buf, &buf_len, &len, &flags, &addr, &addr_len, &port, &port_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	switch (php_sock->type) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, sizeof(s_un.sun_path), "%s", addr);

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len,	flags, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			break;

		case AF_INET:
			if (port_is_null) {
				zend_argument_value_error(6, "cannot be null when the socket type is AF_INET");
				RETURN_THROWS();
			}

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons((unsigned short) port);

			if (! php_set_inet_addr(&sin, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin, sizeof(sin));
			break;
#ifdef HAVE_IPV6
		case AF_INET6:
			if (port_is_null) {
				zend_argument_value_error(6, "cannot be null when the socket type is AF_INET6");
				RETURN_THROWS();
			}

			memset(&sin6, 0, sizeof(sin6));
			sin6.sin6_family = AF_INET6;
			sin6.sin6_port = htons((unsigned short) port);

			if (! php_set_inet6_addr(&sin6, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin6, sizeof(sin6));
			break;
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "Unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Gets socket options for the socket */
PHP_FUNCTION(socket_get_option)
{
	zval			*arg1;
	struct linger	linger_val;
	struct timeval	tv;
#ifdef PHP_WIN32
	int				timeout = 0;
#endif
	socklen_t		optlen;
	php_socket		*php_sock;
	int				other_val;
	zend_long			level, optname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oll", &arg1, socket_ce, &level, &optname) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (level == IPPROTO_IP) {
		switch (optname) {
		case IP_MULTICAST_IF: {
			struct in_addr if_addr;
			unsigned int if_index;
			optlen = sizeof(if_addr);
			if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&if_addr, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
				RETURN_FALSE;
			}
			if (php_add4_to_if_index(&if_addr, php_sock, &if_index) == SUCCESS) {
				RETURN_LONG((zend_long) if_index);
			} else {
				RETURN_FALSE;
			}
		}
		}
	}
#ifdef HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int ret = php_do_getsockopt_ipv6_rfc3542(php_sock, level, optname, return_value);
		if (ret == SUCCESS) {
			return;
		} else if (ret == FAILURE) {
			RETURN_FALSE;
		} /* else continue */
	}
#endif

	if (level == IPPROTO_TCP) {
		switch (optname) {
#ifdef TCP_CONGESTION
		case TCP_CONGESTION: {
			char name[16];
			optlen = sizeof(name);
			if (getsockopt(php_sock->bsd_socket, level, optname, name, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
				RETURN_FALSE;
			} else {
				array_init(return_value);

				add_assoc_string(return_value, "name", name);
				return;
			}
		}
#endif
		}
	}

	if (level == SOL_SOCKET) {
		switch (optname) {
			case SO_LINGER:
				optlen = sizeof(linger_val);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&linger_val, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				array_init(return_value);
				add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
				add_assoc_long(return_value, "l_linger", linger_val.l_linger);
				return;

			case SO_RCVTIMEO:
			case SO_SNDTIMEO:
#ifndef PHP_WIN32
				optlen = sizeof(tv);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&tv, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}
#else
				optlen = sizeof(int);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&timeout, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				tv.tv_sec = timeout ? timeout / 1000 : 0;
				tv.tv_usec = timeout ? (timeout * 1000) % 1000000 : 0;
#endif

				array_init(return_value);

				add_assoc_long(return_value, "sec", tv.tv_sec);
				add_assoc_long(return_value, "usec", tv.tv_usec);
				return;
#ifdef SO_MEMINFO
			case SO_MEMINFO: {
				uint32_t minfo[SK_MEMINFO_VARS];
				optlen = sizeof(minfo);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)minfo, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				if (UNEXPECTED(optlen != sizeof(minfo))) {
					// unlikely since the kernel fills up the whole array if getsockopt succeeded
					// but just an extra precaution in case.
					php_error_docref(NULL, E_WARNING, "Unable to retrieve all socket meminfo data");
					RETURN_FALSE;
				}

				array_init(return_value);

				add_assoc_long(return_value, "rmem_alloc", minfo[SK_MEMINFO_RMEM_ALLOC]);
				add_assoc_long(return_value, "rcvbuf", minfo[SK_MEMINFO_RCVBUF]);
				add_assoc_long(return_value, "wmem_alloc", minfo[SK_MEMINFO_WMEM_ALLOC]);
				add_assoc_long(return_value, "sndbuf", minfo[SK_MEMINFO_SNDBUF]);
				add_assoc_long(return_value, "fwd_alloc", minfo[SK_MEMINFO_FWD_ALLOC]);
				add_assoc_long(return_value, "wmem_queued", minfo[SK_MEMINFO_WMEM_QUEUED]);
				add_assoc_long(return_value, "optmem", minfo[SK_MEMINFO_OPTMEM]);
				add_assoc_long(return_value, "backlog", minfo[SK_MEMINFO_BACKLOG]);
				add_assoc_long(return_value, "drops", minfo[SK_MEMINFO_DROPS]);
				return;
			}
#endif
#ifdef SO_ACCEPTFILTER
			case SO_ACCEPTFILTER: {

				struct accept_filter_arg af = {0};
				optlen = sizeof(af);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&af, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				array_init(return_value);

				add_assoc_string(return_value, "af_name", af.af_name);
				return;
			}
#endif

		}
	}

#ifdef SOL_FILTER
	if (level == SOL_FILTER) {
		switch (optname) {

			case FIL_LIST: {
				size_t i;
				struct fil_info fi[32] = {{0}};
				optlen = sizeof(fi);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)fi, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				array_init(return_value);

				for (i = 0; i < optlen / sizeof(struct fil_info); i++) {
					add_index_string(return_value, i, fi[i].fi_name);
				}

				return;
			}
		}
	}
#endif

	optlen = sizeof(other_val);

	if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&other_val, &optlen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
		RETURN_FALSE;
	}

	if (optlen == 1) {
		other_val = *((unsigned char *)&other_val);
	}

	RETURN_LONG(other_val);
}
/* }}} */

/* {{{ Sets socket options for the socket */
PHP_FUNCTION(socket_set_option)
{
	zval					*arg1, *arg4;
	struct linger			lv;
	php_socket				*php_sock;
	int						ov, optlen, retval;
#ifdef PHP_WIN32
	int						timeout;
#else
	struct					timeval tv;
#endif
	zend_long					level, optname;
	void 					*opt_ptr;
	HashTable		 		*opt_ht;
	zval 					*l_onoff, *l_linger;
	zval		 			*sec, *usec;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ollz", &arg1, socket_ce, &level, &optname, &arg4) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	set_errno(0);

#define HANDLE_SUBCALL(res) \
	do { \
		if (res == 1) { goto default_case; } \
		else if (res == SUCCESS) { RETURN_TRUE; } \
		else { RETURN_FALSE; } \
	} while (0)


	if (level == IPPROTO_IP) {
		int res = php_do_setsockopt_ip_mcast(php_sock, level, optname, arg4);
		HANDLE_SUBCALL(res);
	}

#ifdef HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int res = php_do_setsockopt_ipv6_mcast(php_sock, level, optname, arg4);
		if (res == 1) {
			res = php_do_setsockopt_ipv6_rfc3542(php_sock, level, optname, arg4);
		}
		HANDLE_SUBCALL(res);
	}
#endif

	if (level == IPPROTO_TCP) {
		switch (optname) {
#ifdef TCP_CONGESTION
		case TCP_CONGESTION: {
			if (Z_TYPE_P(arg4) == IS_STRING) {
				opt_ptr = Z_STRVAL_P(arg4);
				optlen = Z_STRLEN_P(arg4);
			} else {
				opt_ptr = "";
				optlen = 0;
			}
			if (setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to set socket option", errno);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}
#endif
		}
	}

	switch (optname) {
		case SO_LINGER: {
			const char l_onoff_key[] = "l_onoff";
			const char l_linger_key[] = "l_linger";

			convert_to_array(arg4);
			opt_ht = Z_ARRVAL_P(arg4);

			if ((l_onoff = zend_hash_str_find(opt_ht, l_onoff_key, sizeof(l_onoff_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", l_onoff_key);
				RETURN_THROWS();
			}
			if ((l_linger = zend_hash_str_find(opt_ht, l_linger_key, sizeof(l_linger_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", l_linger_key);
				RETURN_THROWS();
			}

			convert_to_long(l_onoff);
			convert_to_long(l_linger);

			lv.l_onoff = (unsigned short)Z_LVAL_P(l_onoff);
			lv.l_linger = (unsigned short)Z_LVAL_P(l_linger);

			optlen = sizeof(lv);
			opt_ptr = &lv;
			break;
		}

		case SO_RCVTIMEO:
		case SO_SNDTIMEO: {
			const char sec_key[] = "sec";
			const char usec_key[] = "usec";

			convert_to_array(arg4);
			opt_ht = Z_ARRVAL_P(arg4);

			if ((sec = zend_hash_str_find(opt_ht, sec_key, sizeof(sec_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", sec_key);
				RETURN_THROWS();
			}
			if ((usec = zend_hash_str_find(opt_ht, usec_key, sizeof(usec_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", usec_key);
				RETURN_THROWS();
			}

			convert_to_long(sec);
			convert_to_long(usec);
#ifndef PHP_WIN32
			tv.tv_sec = Z_LVAL_P(sec);
			tv.tv_usec = Z_LVAL_P(usec);
			optlen = sizeof(tv);
			opt_ptr = &tv;
#else
			timeout = Z_LVAL_P(sec) * 1000 + Z_LVAL_P(usec) / 1000;
			optlen = sizeof(int);
			opt_ptr = &timeout;
#endif
			break;
		}
#ifdef SO_BINDTODEVICE
		case SO_BINDTODEVICE: {
			if (Z_TYPE_P(arg4) == IS_STRING) {
				opt_ptr = Z_STRVAL_P(arg4);
				optlen = Z_STRLEN_P(arg4);
			} else {
				opt_ptr = "";
				optlen = 0;
			}
			break;
		}
#endif

#ifdef SO_ACCEPTFILTER
		case SO_ACCEPTFILTER: {
			if (Z_TYPE_P(arg4) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Invalid filter argument type");
				RETURN_FALSE;
			}
			struct accept_filter_arg af = {0};
			strlcpy(af.af_name, Z_STRVAL_P(arg4), sizeof(af.af_name));
			opt_ptr = &af;
			optlen = sizeof(af);
			break;
		}
#endif

#ifdef FIL_ATTACH
		case FIL_ATTACH:
		case FIL_DETACH: {
			if (level != SOL_FILTER) {
				php_error_docref(NULL, E_WARNING, "Invalid level");
				RETURN_FALSE;
			}
			if (Z_TYPE_P(arg4) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Invalid filter argument type");
				RETURN_FALSE;
			}
			opt_ptr = Z_STRVAL_P(arg4);
			optlen = Z_STRLEN_P(arg4);
			break;
		}
#endif

#ifdef SO_ATTACH_REUSEPORT_CBPF
		case SO_ATTACH_REUSEPORT_CBPF: {
			convert_to_long(arg4);

			if (!Z_LVAL_P(arg4)) {
				ov = 1;
				optlen = sizeof(ov);
				opt_ptr = &ov;
				optname = SO_DETACH_BPF;
			} else {
				uint32_t k = (uint32_t)Z_LVAL_P(arg4);
				static struct sock_filter cbpf[8] = {0};
				static struct sock_fprog bpfprog;

				switch (k) {
					case SKF_AD_CPU:
					case SKF_AD_QUEUE:
						cbpf[0].code = (BPF_LD|BPF_W|BPF_ABS);
						cbpf[0].k = (uint32_t)(SKF_AD_OFF + k);
						cbpf[1].code = (BPF_RET|BPF_A);
						bpfprog.len = 2;
					break;
					default:
						php_error_docref(NULL, E_WARNING, "Unsupported CBPF filter");
						RETURN_FALSE;
				}

				bpfprog.filter = cbpf;
				optlen = sizeof(bpfprog);
				opt_ptr = &bpfprog;
			}
			break;
		}
#endif

		default:
default_case:
			convert_to_long(arg4);
			ov = Z_LVAL_P(arg4);

			optlen = sizeof(ov);
			opt_ptr = &ov;
			break;
	}

	retval = setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen);
	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to set socket option", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_SOCKETPAIR
/* {{{ Creates a pair of indistinguishable sockets and stores them in fds. */
PHP_FUNCTION(socket_create_pair)
{
	zval		retval[2], *fds_array_zval;
	php_socket	*php_sock[2];
	PHP_SOCKET	fds_array[2];
	zend_long		domain, type, protocol;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lllz", &domain, &type, &protocol, &fds_array_zval) == FAILURE) {
		RETURN_THROWS();
	}

	if (domain != AF_INET
#ifdef HAVE_IPV6
		&& domain != AF_INET6
#endif
		&& domain != AF_UNIX) {
		zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET6, or AF_INET");
		RETURN_THROWS();
	}

	if (type > 10) {
		zend_argument_value_error(2, "must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET,"
			" SOCK_RAW, or SOCK_RDM");
		RETURN_THROWS();
	}

	object_init_ex(&retval[0], socket_ce);
	php_sock[0] = Z_SOCKET_P(&retval[0]);

	object_init_ex(&retval[1], socket_ce);
	php_sock[1] = Z_SOCKET_P(&retval[1]);

	if (socketpair(domain, type, protocol, fds_array) != 0) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket pair [%d]: %s", errno, sockets_strerror(errno));
		zval_ptr_dtor(&retval[0]);
		zval_ptr_dtor(&retval[1]);
		RETURN_FALSE;
	}

	fds_array_zval = zend_try_array_init(fds_array_zval);
	if (!fds_array_zval) {
		zval_ptr_dtor(&retval[0]);
		zval_ptr_dtor(&retval[1]);
		RETURN_THROWS();
	}

	php_sock[0]->bsd_socket = fds_array[0];
	php_sock[1]->bsd_socket = fds_array[1];
	php_sock[0]->type		= domain;
	php_sock[1]->type		= domain;
	php_sock[0]->error		= 0;
	php_sock[1]->error		= 0;
	php_sock[0]->blocking	= 1;
	php_sock[1]->blocking	= 1;

	add_index_zval(fds_array_zval, 0, &retval[0]);
	add_index_zval(fds_array_zval, 1, &retval[1]);

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_SHUTDOWN
/* {{{ Shuts down a socket for receiving, sending, or both. */
PHP_FUNCTION(socket_shutdown)
{
	zval		*arg1;
	zend_long		how_shutdown = 2;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &arg1, socket_ce, &how_shutdown) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (shutdown(php_sock->bsd_socket, how_shutdown) != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to shutdown socket", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_SOCKATMARK
PHP_FUNCTION(socket_atmark)
{
	zval		*arg1;
	php_socket	*php_sock;
	int r;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	r = sockatmark(php_sock->bsd_socket);
	if (r < 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to apply sockmark", errno);
		RETURN_FALSE;
	} else if (r == 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
#endif

/* {{{ Returns the last socket error (either the last used or the provided socket resource) */
PHP_FUNCTION(socket_last_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	if (arg1) {
		php_sock = Z_SOCKET_P(arg1);
		ENSURE_SOCKET_VALID(php_sock);

		RETVAL_LONG(php_sock->error);
	} else {
		RETVAL_LONG(SOCKETS_G(last_error));
	}
}
/* }}} */

/* {{{ Clears the error on the socket or the last error code. */
PHP_FUNCTION(socket_clear_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &arg1, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	if (arg1) {
		php_sock = Z_SOCKET_P(arg1);
		ENSURE_SOCKET_VALID(php_sock);

		php_sock->error = 0;
	} else {
		SOCKETS_G(last_error) = 0;
	}

	return;
}
/* }}} */

bool socket_import_file_descriptor(PHP_SOCKET socket, php_socket *retsock)
{
#ifdef SO_DOMAIN
	int						type;
	socklen_t				type_len = sizeof(type);
#endif
	php_sockaddr_storage	addr;
	socklen_t				addr_len = sizeof(addr);
#ifndef PHP_WIN32
	int					 t;
#endif

	retsock->bsd_socket = socket;

	/* determine family */
#ifdef SO_DOMAIN
	if (getsockopt(socket, SOL_SOCKET, SO_DOMAIN, &type, &type_len) == 0) {
		retsock->type = type;
	} else
#endif
	if (getsockname(socket, (struct sockaddr*)&addr, &addr_len) == 0) {
		retsock->type = addr.ss_family;
	} else {
		PHP_SOCKET_ERROR(retsock, "Unable to obtain socket family", errno);
		return 0;
	}

	/* determine blocking mode */
#ifndef PHP_WIN32
	t = fcntl(socket, F_GETFL);
	if (t == -1) {
		PHP_SOCKET_ERROR(retsock, "Unable to obtain blocking state", errno);
		return 0;
	} else {
		retsock->blocking = !(t & O_NONBLOCK);
	}
#endif

	return 1;
}

/* {{{ Imports a stream that encapsulates a socket into a socket extension resource. */
PHP_FUNCTION(socket_import_stream)
{
	zval				 *zstream;
	php_stream			 *stream;
	php_socket			 *retsock = NULL;
	PHP_SOCKET			 socket; /* fd */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zstream) == FAILURE) {
		RETURN_THROWS();
	}
	php_stream_from_zval(stream, zstream);

	if (php_stream_cast(stream, PHP_STREAM_AS_SOCKETD, (void**)&socket, 1)) {
		/* error supposedly already shown */
		RETURN_FALSE;
	}

	object_init_ex(return_value, socket_ce);
	retsock = Z_SOCKET_P(return_value);

	if (!socket_import_file_descriptor(socket, retsock)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

#ifdef PHP_WIN32
	/* on windows, check if the stream is a socket stream and read its
	 * private data; otherwise assume it's in non-blocking mode */
	if (php_stream_is(stream, PHP_STREAM_IS_SOCKET)) {
		retsock->blocking =
				((php_netstream_data_t *)stream->abstract)->is_blocked;
	} else {
		retsock->blocking = 1;
	}
#endif

	/* hold a zval reference to the stream (holding a php_stream* directly could
	 * also be done, but this makes socket_export_stream a bit simpler) */
	ZVAL_COPY(&retsock->zstream, zstream);

	php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
}
/* }}} */

/* {{{ Exports a socket extension resource into a stream that encapsulates a socket. */
PHP_FUNCTION(socket_export_stream)
{
	zval *zsocket;
	php_socket *socket;
	php_stream *stream = NULL;
	php_netstream_data_t *stream_data;
	const char *protocol = NULL;
	size_t protocollen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zsocket, socket_ce) == FAILURE) {
		RETURN_THROWS();
	}

	socket = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(socket);

	/* Either we already exported a stream or the socket came from an import,
	 * just return the existing stream */
	if (!Z_ISUNDEF(socket->zstream)) {
		RETURN_COPY(&socket->zstream);
	}

	/* Determine if socket is using a protocol with one of the default registered
	 * socket stream wrappers */
	if (socket->type == PF_INET
#ifdef HAVE_IPV6
		 || socket->type == PF_INET6
#endif
	) {
		int protoid;
		socklen_t protoidlen = sizeof(protoid);

		getsockopt(socket->bsd_socket, SOL_SOCKET, SO_TYPE, (char *) &protoid, &protoidlen);

		if (protoid == SOCK_STREAM) {
			/* SO_PROTOCOL is not (yet?) supported on OS X, so let's assume it's TCP there */
#ifdef SO_PROTOCOL
			protoidlen = sizeof(protoid);
			getsockopt(socket->bsd_socket, SOL_SOCKET, SO_PROTOCOL, (char *) &protoid, &protoidlen);
			if (protoid == IPPROTO_TCP)
#endif
			{
				protocol = "tcp://";
				protocollen = sizeof("tcp://") - 1;
			}
		} else if (protoid == SOCK_DGRAM) {
			protocol = "udp://";
			protocollen = sizeof("udp://") - 1;
		}
#ifdef PF_UNIX
	} else if (socket->type == PF_UNIX) {
		int type;
		socklen_t typelen = sizeof(type);

		getsockopt(socket->bsd_socket, SOL_SOCKET, SO_TYPE, (char *) &type, &typelen);

		if (type == SOCK_STREAM) {
			protocol = "unix://";
			protocollen = sizeof("unix://") - 1;
		} else if (type == SOCK_DGRAM) {
			protocol = "udg://";
			protocollen = sizeof("udg://") - 1;
		}
#endif
	}

	/* Try to get a stream with the registered sockops for the protocol in use
	 * We don't want streams to actually *do* anything though, so don't give it
	 * anything apart from the protocol */
	if (protocol != NULL) {
		stream = php_stream_xport_create(protocol, protocollen, 0, 0, NULL, NULL, NULL, NULL, NULL);
	}

	/* Fall back to creating a generic socket stream */
	if (stream == NULL) {
		stream = php_stream_sock_open_from_socket(socket->bsd_socket, 0);

		if (stream == NULL) {
			php_error_docref(NULL, E_WARNING, "Failed to create stream");
			RETURN_FALSE;
		}
	}

	stream_data = (php_netstream_data_t *) stream->abstract;
	stream_data->socket = socket->bsd_socket;
	stream_data->is_blocked = socket->blocking;
	stream_data->timeout.tv_sec = FG(default_socket_timeout);
	stream_data->timeout.tv_usec = 0;

	php_stream_to_zval(stream, &socket->zstream);

	RETURN_COPY(&socket->zstream);
}
/* }}} */

/* {{{ Gets array with contents of getaddrinfo about the given hostname. */
PHP_FUNCTION(socket_addrinfo_lookup)
{
	char *service = NULL;
	size_t service_len = 0;
	zend_string *hostname, *key;
	zval *hint, *zhints = NULL;

	struct addrinfo hints, *result, *rp;
	php_addrinfo *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|s!a", &hostname, &service, &service_len, &zhints) == FAILURE) {
		RETURN_THROWS();
	}

	memset(&hints, 0, sizeof(hints));

	if (zhints && !HT_IS_PACKED(Z_ARRVAL_P(zhints))) {
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zhints), key, hint) {
			if (key) {
				if (zend_string_equals_literal(key, "ai_flags")) {
					hints.ai_flags = zval_get_long(hint);
				} else if (zend_string_equals_literal(key, "ai_socktype")) {
					hints.ai_socktype = zval_get_long(hint);
				} else if (zend_string_equals_literal(key, "ai_protocol")) {
					hints.ai_protocol = zval_get_long(hint);
				} else if (zend_string_equals_literal(key, "ai_family")) {
					hints.ai_family = zval_get_long(hint);
				} else {
					zend_argument_value_error(3, "must only contain array keys \"ai_flags\", \"ai_socktype\", "
						"\"ai_protocol\", or \"ai_family\"");
					RETURN_THROWS();
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (getaddrinfo(ZSTR_VAL(hostname), service, &hints, &result) != 0) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_family != AF_UNSPEC) {
			zval zaddr;

			object_init_ex(&zaddr, address_info_ce);
			res = Z_ADDRESS_INFO_P(&zaddr);

			memcpy(&res->addrinfo, rp, sizeof(struct addrinfo));

			res->addrinfo.ai_addr = emalloc(rp->ai_addrlen);
			memcpy(res->addrinfo.ai_addr, rp->ai_addr, rp->ai_addrlen);

			if (rp->ai_canonname != NULL) {
				res->addrinfo.ai_canonname = estrdup(rp->ai_canonname);
			}

			add_next_index_zval(return_value, &zaddr);
		}
	}

	freeaddrinfo(result);
}
/* }}} */

/* {{{ Creates and binds to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_bind)
{
	zval			*arg1;
	int				retval;
	php_addrinfo	*ai;
	php_socket		*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, address_info_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ai = Z_ADDRESS_INFO_P(arg1);

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	php_sock->bsd_socket = socket(ai->addrinfo.ai_family, ai->addrinfo.ai_socktype, ai->addrinfo.ai_protocol);
	php_sock->type = ai->addrinfo.ai_family;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	switch(php_sock->type) {
		case AF_UNIX:
			{
				// AF_UNIX sockets via getaddrino are not implemented due to security problems
				close(php_sock->bsd_socket);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
			}

		case AF_INET:
#ifdef HAVE_IPV6
		case AF_INET6:
#endif
			{
				retval = bind(php_sock->bsd_socket, ai->addrinfo.ai_addr, ai->addrinfo.ai_addrlen);
				break;
			}
		default:
			close(php_sock->bsd_socket);
			zval_ptr_dtor(return_value);
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to bind address", errno);
		close(php_sock->bsd_socket);
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Creates and connects to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_connect)
{
	zval			*arg1;
	int				retval;
	php_addrinfo	*ai;
	php_socket		*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, address_info_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ai = Z_ADDRESS_INFO_P(arg1);

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	php_sock->bsd_socket = socket(ai->addrinfo.ai_family, ai->addrinfo.ai_socktype, ai->addrinfo.ai_protocol);
	php_sock->type = ai->addrinfo.ai_family;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	switch(php_sock->type) {
		case AF_UNIX:
			{
				// AF_UNIX sockets via getaddrino are not implemented due to security problems
				close(php_sock->bsd_socket);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
			}

		case AF_INET:
#ifdef HAVE_IPV6
		case AF_INET6:
#endif
			{
				retval = connect(php_sock->bsd_socket, ai->addrinfo.ai_addr, ai->addrinfo.ai_addrlen);
				break;
			}
		default:
			zend_argument_value_error(1, "socket type must be one of AF_UNIX, AF_INET, or AF_INET6");
			close(php_sock->bsd_socket);
			zval_ptr_dtor(return_value);
			RETURN_THROWS();
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "Unable to connect address", errno);
		close(php_sock->bsd_socket);
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Creates and connects to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_explain)
{
	zval			*arg1, sockaddr;
	php_addrinfo	*ai;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &arg1, address_info_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ai = Z_ADDRESS_INFO_P(arg1);

	array_init(return_value);

	add_assoc_long(return_value, "ai_flags", ai->addrinfo.ai_flags);
	add_assoc_long(return_value, "ai_family", ai->addrinfo.ai_family);
	add_assoc_long(return_value, "ai_socktype", ai->addrinfo.ai_socktype);
	add_assoc_long(return_value, "ai_protocol", ai->addrinfo.ai_protocol);
	if (ai->addrinfo.ai_canonname != NULL) {
		add_assoc_string(return_value, "ai_canonname", ai->addrinfo.ai_canonname);
	}

	array_init(&sockaddr);
	switch (ai->addrinfo.ai_family) {
		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) ai->addrinfo.ai_addr;
				char addr[INET_ADDRSTRLEN];

				add_assoc_long(&sockaddr, "sin_port", ntohs((unsigned short) sa->sin_port));
				inet_ntop(ai->addrinfo.ai_family, &sa->sin_addr, addr, sizeof(addr));
				add_assoc_string(&sockaddr, "sin_addr", addr);
				break;
			}
#ifdef HAVE_IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sa = (struct sockaddr_in6 *) ai->addrinfo.ai_addr;
				char addr[INET6_ADDRSTRLEN];

				add_assoc_long(&sockaddr, "sin6_port", ntohs((unsigned short) sa->sin6_port));
				inet_ntop(ai->addrinfo.ai_family, &sa->sin6_addr, addr, sizeof(addr));
				add_assoc_string(&sockaddr, "sin6_addr", addr);
				break;
			}
#endif
	}

	add_assoc_zval(return_value, "ai_addr", &sockaddr);
}
/* }}} */

#ifdef PHP_WIN32

/* {{{ Exports the network socket information suitable to be used in another process and returns the info id. */
PHP_FUNCTION(socket_wsaprotocol_info_export)
{
	WSAPROTOCOL_INFO wi;
	zval *zsocket;
	php_socket *socket;
	zend_long target_pid;
	zend_string *seg_name;
	HANDLE map;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &zsocket, socket_ce, &target_pid) == FAILURE) {
		RETURN_THROWS();
	}

	socket = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(socket);

	if (SOCKET_ERROR == WSADuplicateSocket(socket->bsd_socket, (DWORD)target_pid, &wi)) {
		DWORD err = WSAGetLastError();
		char *buf = php_win32_error_to_msg(err);

		if (!buf[0]) {
			php_error_docref(NULL, E_WARNING, "Unable to export WSA protocol info [0x%08lx]", err);
		} else {
			php_error_docref(NULL, E_WARNING, "Unable to export WSA protocol info [0x%08lx]: %s", err, buf);
		}

		php_win32_error_msg_free(buf);

		RETURN_FALSE;
	}

	seg_name = zend_strpprintf(0, "php_wsa_for_%u", SOCKETS_G(wsa_child_count)++);
	map = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(WSAPROTOCOL_INFO), ZSTR_VAL(seg_name));
	if (NULL != map) {
		LPVOID view = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, 0);
		if (view) {
			memcpy(view, &wi, sizeof(wi));
			UnmapViewOfFile(view);
			zend_hash_add_ptr(&(SOCKETS_G(wsa_info)), seg_name, map);
			RETURN_STR(seg_name);
		} else {
			DWORD err = GetLastError();
			php_error_docref(NULL, E_WARNING, "Unable to map file view [0x%08lx]", err);
		}
	} else {
		DWORD err = GetLastError();
		php_error_docref(NULL, E_WARNING, "Unable to create file mapping [0x%08lx]", err);
	}
	zend_string_release_ex(seg_name, 0);

	RETURN_FALSE;
}
/* }}} */

/* {{{ Imports the network socket information using the supplied id and creates a new socket on its base. */
PHP_FUNCTION(socket_wsaprotocol_info_import)
{
	char *id;
	size_t id_len;
	WSAPROTOCOL_INFO wi;
	PHP_SOCKET sock;
	php_socket	*php_sock;
	HANDLE map;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &id, &id_len) == FAILURE) {
		RETURN_THROWS();
	}

	map = OpenFileMapping(FILE_MAP_READ, FALSE, id);
	if (map) {
		LPVOID view = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
		if (view) {
			memcpy(&wi, view, sizeof(WSAPROTOCOL_INFO));
			UnmapViewOfFile(view);
		} else {
			DWORD err = GetLastError();
			php_error_docref(NULL, E_WARNING, "Unable to map file view [0x%08lx]", err);
			RETURN_FALSE;
		}
		CloseHandle(map);
	} else {
		DWORD err = GetLastError();
		php_error_docref(NULL, E_WARNING, "Unable to open file mapping [0x%08lx]", err);
		RETURN_FALSE;
	}

	sock = WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &wi, 0, 0);
	if (INVALID_SOCKET == sock) {
		DWORD err = WSAGetLastError();
		char *buf = php_win32_error_to_msg(err);

		if (!buf[0]) {
			php_error_docref(NULL, E_WARNING, "Unable to import WSA protocol info [0x%08lx]", err);
		} else {
			php_error_docref(NULL, E_WARNING, "Unable to import WSA protocol info [0x%08lx]: %s", err, buf);
		}

		php_win32_error_msg_free(buf);

		RETURN_FALSE;
	}

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	php_sock->bsd_socket = sock;
	php_sock->type = wi.iAddressFamily;
	php_sock->error = 0;
	php_sock->blocking = 1;
}
/* }}} */

/* {{{ Frees the exported info and corresponding resources using the supplied id. */
PHP_FUNCTION(socket_wsaprotocol_info_release)
{
	char *id;
	size_t id_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &id, &id_len) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(SUCCESS == zend_hash_str_del(&(SOCKETS_G(wsa_info)), id, id_len));
}
/* }}} */
#endif
