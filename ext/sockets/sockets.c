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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |          Jason Greene     <jason@php.net>                            |
   |          Gustavo Lopes    <cataphract@php.net>                       |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

#include "zend_exceptions.h"
#include "zend_hrtime.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#include "php_network.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "ext/standard/io_poll.h"
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
# ifdef HAVE_IF_NAMETOINDEX
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
# if defined(HAVE_LINUX_IF_PACKET_H)
#  include <linux/if_packet.h>
# endif
# if defined(HAVE_LINUX_IF_ETHER_H)
#  include <linux/if_ether.h>
# endif
# if defined(HAVE_LINUX_UDP_H)
#  include <linux/udp.h>
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

#if defined(AF_PACKET)
#define PHP_ETH_PROTO_CHECK(protocol, family)								\
	do {												\
		/* We ll let EINVAL errno warning about miusage, too many protocols conflicts */	\
                if (protocol <= USHRT_MAX && family == AF_PACKET) {					\
                    protocol = htons(protocol);								\
                }											\
        } while (0)
#else
#define PHP_ETH_PROTO_CHECK(protocol, family)								\
	(void)protocol;											\
	(void)family
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
zend_class_entry *socket_poll_handle_ce;
zend_class_entry *socket_poll_weak_handle_ce;
static zend_object_handlers socket_poll_handle_object_handlers;
static zend_object_handlers socket_poll_weak_handle_object_handlers;

static zend_object *socket_create_object(zend_class_entry *class_type) {
	php_socket *intern = zend_object_alloc(sizeof(php_socket), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->bsd_socket = -1; /* invalid socket */
	intern->type		 = PF_UNSPEC;
	intern->error		 = 0;
	intern->blocking	 = 1;
	intern->nonblocking_fd = false;
	intern->in_use = false;
	ZVAL_UNDEF(&intern->zstream);
	intern->weak_handle = NULL;
	intern->strong_handles = NULL;

	return &intern->std;
}

/* SocketPollHandle keeps its Socket and is retired with the descriptor; SocketPollWeakHandle is
 * the Socket's one handle, created by its operations and registrations or by create(), kept by the
 * Socket until the descriptor closes and invalid from then on */

struct _php_socket_poll_handle_data {
	php_socket *sock; /* referenced */
	php_poll_handle_object *handle;
	bool linked; /* on the Socket's list, until the descriptor closes */
	php_socket_poll_handle_data *prev;
	php_socket_poll_handle_data *next;
};

typedef struct php_socket_poll_weak_handle_data {
	php_socket *sock; /* not referenced, NULL once the descriptor closed */
} php_socket_poll_weak_handle_data;

static php_socket_t socket_poll_handle_get_fd(php_poll_handle_object *handle)
{
	php_socket_poll_handle_data *data = handle->handle_data;
	if (!data || IS_INVALID_SOCKET(data->sock)) {
		return SOCK_ERR;
	}
	return (php_socket_t) data->sock->bsd_socket;
}

static int socket_poll_handle_is_valid(php_poll_handle_object *handle)
{
	return socket_poll_handle_get_fd(handle) != SOCK_ERR;
}

static void socket_poll_handle_unlink(php_socket_poll_handle_data *data)
{
	if (!data->linked) {
		return;
	}
	data->linked = false;
	if (data->prev) {
		data->prev->next = data->next;
	} else {
		data->sock->strong_handles = data->next;
	}
	if (data->next) {
		data->next->prev = data->prev;
	}
}

static void socket_poll_handle_cleanup(php_poll_handle_object *handle)
{
	php_socket_poll_handle_data *data = handle->handle_data;
	if (data) {
		socket_poll_handle_unlink(data);
		OBJ_RELEASE(&data->sock->std);
		efree(data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops socket_poll_handle_ops = {
	.get_fd = socket_poll_handle_get_fd,
	.is_valid = socket_poll_handle_is_valid,
	.cleanup = socket_poll_handle_cleanup,
};

static zend_object *socket_poll_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &socket_poll_handle_ops);
	intern->std.handlers = &socket_poll_handle_object_handlers;
	return &intern->std;
}

static HashTable *socket_poll_handle_get_gc(zend_object *object, zval **table, int *n)
{
	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(object);
	php_socket_poll_handle_data *data = intern->handle_data;
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	if (data) {
		zend_get_gc_buffer_add_obj(gc_buffer, &data->sock->std);
	}
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

static php_socket_t socket_poll_weak_handle_get_fd(php_poll_handle_object *handle)
{
	php_socket_poll_weak_handle_data *data = handle->handle_data;
	if (!data || !data->sock || IS_INVALID_SOCKET(data->sock)) {
		return SOCK_ERR;
	}
	return (php_socket_t) data->sock->bsd_socket;
}

static int socket_poll_weak_handle_is_valid(php_poll_handle_object *handle)
{
	return socket_poll_weak_handle_get_fd(handle) != SOCK_ERR;
}

static void socket_poll_weak_handle_cleanup(php_poll_handle_object *handle)
{
	php_socket_poll_weak_handle_data *data = handle->handle_data;
	if (data) {
		/* Freed by the object store before the Socket let go of it */
		if (data->sock) {
			data->sock->weak_handle = NULL;
		}
		efree(data);
		handle->handle_data = NULL;
	}
}

static php_poll_handle_ops socket_poll_weak_handle_ops = {
	.get_fd = socket_poll_weak_handle_get_fd,
	.is_valid = socket_poll_weak_handle_is_valid,
	.cleanup = socket_poll_weak_handle_cleanup,
};

static zend_object *socket_poll_weak_handle_create_object(zend_class_entry *ce)
{
	php_poll_handle_object *intern = php_poll_handle_object_create(
			sizeof(php_poll_handle_object), ce, &socket_poll_weak_handle_ops);
	intern->std.handlers = &socket_poll_weak_handle_object_handlers;
	return &intern->std;
}

/* The Socket's handle, created on the first call and kept by the Socket: borrowed */
static zend_object *php_socket_get_weak_handle(php_socket *sock)
{
	if (!sock->weak_handle) {
		zval handle_zv;
		object_init_ex(&handle_zv, socket_poll_weak_handle_ce);
		php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(&handle_zv);
		php_socket_poll_weak_handle_data *data = emalloc(sizeof(*data));
		data->sock = sock;
		intern->handle_data = data;
		/* Every Socket is a script's */
		intern->flags |= PHP_POLL_HANDLE_F_EXPOSED;
		sock->weak_handle = Z_OBJ(handle_zv);
	}
	return sock->weak_handle;
}

/* While the descriptor is still open: the handles stop reporting, the registrations end with the
 * provider's remove() per pair, then the Socket drops its handle */
static void php_socket_release_io(php_socket *sock)
{
	while (sock->strong_handles) {
		php_socket_poll_handle_data *data = sock->strong_handles;
		socket_poll_handle_unlink(data);
		/* Retiring the watchers may drop the last reference to the handle */
		php_poll_handle_invalidate(&data->handle->std);
	}
	zend_object *handle_obj = sock->weak_handle;
	if (handle_obj) {
		php_poll_handle_object *handle = PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(handle_obj);
		((php_socket_poll_weak_handle_data *) handle->handle_data)->sock = NULL;
		php_poll_handle_invalidate(handle_obj);
		php_io_unregister_all(&handle->registrations);
		/* A remove() that closed the Socket again let go of it already */
		if (sock->weak_handle == handle_obj) {
			sock->weak_handle = NULL;
			OBJ_RELEASE(handle_obj);
		}
	}
}

PHP_METHOD(SocketPollHandle, __construct)
{
	zval *zsocket;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	if (intern->handle_data) {
		zend_throw_error(NULL, "%pS object is already constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}
	php_socket *sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(sock);

	php_socket_poll_handle_data *data = emalloc(sizeof(*data));
	data->sock = sock;
	data->handle = intern;
	data->linked = true;
	data->prev = NULL;
	data->next = sock->strong_handles;
	if (data->next) {
		data->next->prev = data;
	}
	sock->strong_handles = data;
	GC_ADDREF(&sock->std);
	intern->handle_data = data;
}

PHP_METHOD(SocketPollHandle, getSocket)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_socket_poll_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	if (!data) {
		zend_throw_error(NULL, "%pS object is not constructed", Z_OBJCE_P(ZEND_THIS)->name);
		RETURN_THROWS();
	}
	RETURN_OBJ_COPY(&data->sock->std);
}

PHP_METHOD(SocketPollHandle, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL(intern->ops->is_valid(intern));
}

PHP_METHOD(SocketPollWeakHandle, __construct)
{
	zend_throw_error(NULL, "Direct instantiation of %pS is not allowed, use %pS::create instead",
			Z_OBJCE_P(ZEND_THIS)->name, Z_OBJCE_P(ZEND_THIS)->name);
}

PHP_METHOD(SocketPollWeakHandle, create)
{
	zval *zsocket;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_socket *sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(sock);
	RETURN_OBJ_COPY(php_socket_get_weak_handle(sock));
}

/* Not while the Socket is in an operation */
PHP_METHOD(SocketPollWeakHandle, getSocket)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_socket_poll_weak_handle_data *data = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS)->handle_data;
	if (!data || !data->sock || data->sock->in_use) {
		RETURN_NULL();
	}
	RETURN_OBJ_COPY(&data->sock->std);
}

PHP_METHOD(SocketPollWeakHandle, isValid)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_poll_handle_object *intern = PHP_POLL_HANDLE_OBJ_FROM_ZV(ZEND_THIS);
	RETURN_BOOL(intern->ops->is_valid(intern));
}

static zend_function *socket_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct Socket, use socket_create() instead");
	return NULL;
}

static void php_socket_release_io(php_socket *sock);

static void socket_free_obj(zend_object *object)
{
	php_socket *socket = socket_from_obj(object);

	php_socket_release_io(socket);
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

#define address_info_from_obj(obj) ZEND_CONTAINER_OF(obj, php_addrinfo, std)

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

/* The poll handle interfaces are ext/standard's, so it starts first */
static const zend_module_dep sockets_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};

zend_module_entry sockets_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	sockets_deps,
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

#ifdef PHP_WIN32
# define PHP_SOCKET_WAIT_TIMEOUT WSAETIMEDOUT
#else
# define PHP_SOCKET_WAIT_TIMEOUT EAGAIN
#endif

/* Waits for a Socket that emulates blocking mode, bounded by its SO_RCVTIMEO
 * or SO_SNDTIMEO like the blocking call. false with errno set on failure. */
static bool php_socket_wait(php_socket *sock, php_socket_waiter *w, int events)
{
	if (!w->started) {
		w->started = true;
		w->end = 0;
#ifdef PHP_WIN32
		DWORD ms = 0;
		int optlen = sizeof(ms);
		if (getsockopt(sock->bsd_socket, SOL_SOCKET, w->optname, (char *) &ms, &optlen) == 0 && ms > 0) {
			w->end = zend_hrtime() + (uint64_t) ms * 1000000;
		}
#else
		struct timeval tv;
		socklen_t optlen = sizeof(tv);
		if (getsockopt(sock->bsd_socket, SOL_SOCKET, w->optname, &tv, &optlen) == 0
				&& (tv.tv_sec > 0 || tv.tv_usec > 0)) {
			w->end = zend_hrtime() + (uint64_t) tv.tv_sec * ZEND_NANO_IN_SEC + (uint64_t) tv.tv_usec * 1000;
		}
#endif
	}

	for (;;) {
		int timeout = -1;
		if (w->end) {
			uint64_t now = zend_hrtime();
			uint64_t ms = now < w->end ? (w->end - now + 999999) / 1000000 : 0;
			timeout = ms > INT_MAX ? INT_MAX : (int) ms;
		}
		int n = php_pollfd_for_ms(sock->bsd_socket, events, timeout);
		if (n > 0) {
			return true;
		}
		if (n == 0) {
			set_errno(PHP_SOCKET_WAIT_TIMEOUT);
			return false;
		}
		/* a timed call fails on a signal, otherwise only when PHP has work for it */
		if (errno != EINTR || w->end || atomic_load(&EG(vm_interrupt))) {
			return false;
		}
	}
}

/* Under a provider a blocking point is an operation that may suspend: the descriptor is made
 * non-blocking once, so no syscall blocks and the Socket emulates its mode from then on, the Socket
 * is frozen for the call, and the op carries the stream the Socket shares its descriptor with or
 * the Socket's own handle. Without a provider the call proceeds as before. */
bool php_socket_op_begin(php_socket *sock, php_socket_op *o)
{
	o->stream = NULL;
	o->handle = NULL;
	o->active = php_io_hooks_active();
	if (!o->active) {
		return true;
	}
	if (sock->in_use) {
		zend_throw_error(NULL, "Concurrent access to a socket");
		set_errno(ECANCELED);
		return false;
	}
	if (sock->blocking && !sock->nonblocking_fd
			&& php_set_sock_blocking(sock->bsd_socket, 0) == SUCCESS) {
		sock->nonblocking_fd = true;
	}
	if (!Z_ISUNDEF(sock->zstream)) {
		o->stream = zend_fetch_resource2_ex(&sock->zstream, NULL, php_file_le_stream(),
				php_file_le_pstream());
	}
	if (!o->stream) {
		o->handle = php_socket_get_weak_handle(sock);
	}
	sock->in_use = true;
	return true;
}

void php_socket_op_end(php_socket *sock, php_socket_op *o)
{
	if (o->active) {
		sock->in_use = false;
	}
}

/* The op's deadline from the Socket's mode: the blocking call's SO_RCVTIMEO or SO_SNDTIMEO */
static php_deadline php_socket_op_deadline(php_socket *sock, int optname, int flags)
{
	php_deadline dl;
	if (!sock->blocking
#ifdef MSG_DONTWAIT
			|| (flags & MSG_DONTWAIT)
#endif
	) {
		php_deadline_init_nonblock(&dl);
		return dl;
	}
	struct timeval tv = { 0, 0 };
#ifdef PHP_WIN32
	DWORD ms = 0;
	int optlen = sizeof(ms);
	if (getsockopt(sock->bsd_socket, SOL_SOCKET, optname, (char *) &ms, &optlen) == 0 && ms > 0) {
		tv.tv_sec = ms / 1000;
		tv.tv_usec = (ms % 1000) * 1000;
	}
#else
	socklen_t optlen = sizeof(tv);
	if (getsockopt(sock->bsd_socket, SOL_SOCKET, optname, &tv, &optlen) != 0) {
		tv.tv_sec = tv.tv_usec = 0;
	}
#endif
	if (tv.tv_sec > 0 || tv.tv_usec > 0) {
		php_deadline_init(&dl, &tv);
	} else {
		php_deadline_init_infinite(&dl);
	}
	return dl;
}

/* An op that timed out fails like the blocking call */
static zend_always_inline void php_socket_op_errno(ssize_t n)
{
	if (n < 0 && errno == PHP_IO_SOCK_ETIMEDOUT) {
		set_errno(PHP_SOCKET_WAIT_TIMEOUT);
	}
}

bool php_socket_wait_retry(php_socket *sock, php_socket_waiter *w, int events, int flags)
{
	if (!PHP_SOCKET_EMULATES_BLOCKING(sock)) {
		return false;
	}
	if (!PHP_IS_TRANSIENT_ERROR(errno)) {
		return false;
	}
#ifdef MSG_DONTWAIT
	if (flags & MSG_DONTWAIT) {
		return false;
	}
#endif
	if (w->op.active) {
		/* A readiness op after the drain; the deadline is the call's, not the wait's */
		if (!w->started) {
			w->started = true;
			w->deadline = php_socket_op_deadline(sock, w->optname, flags);
		}
		int n = php_io_poll_ex(w->op.stream, w->op.handle, sock->bsd_socket,
				(events & POLLIN) ? PHP_POLL_READ : PHP_POLL_WRITE, &w->deadline, PHP_IO_OP_F_AFTER_DRAIN);
		if (n == 0) {
			set_errno(PHP_SOCKET_WAIT_TIMEOUT);
		}
		return n > 0;
	}
	return php_socket_wait(sock, w, events);
}

static ssize_t php_socket_recv_once(php_socket *sock, php_socket_op *o, char *buf, size_t len,
		int flags)
{
	if (!o->active) {
		return recv(sock->bsd_socket, buf, len, flags);
	}
	php_deadline dl = php_socket_op_deadline(sock, SO_RCVTIMEO, flags);
	ssize_t n = php_io_recv_ex(o->stream, o->handle, sock->bsd_socket, buf, len, flags, &dl);
	php_socket_op_errno(n);
	return n;
}

static ssize_t php_socket_send_once(php_socket *sock, php_socket_op *o, const char *buf, size_t len,
		int flags)
{
	if (!o->active) {
		return send(sock->bsd_socket, buf, len, flags);
	}
	php_deadline dl = php_socket_op_deadline(sock, SO_SNDTIMEO, flags);
	ssize_t n = php_io_send_ex(o->stream, o->handle, sock->bsd_socket, buf, len, flags, &dl);
	php_socket_op_errno(n);
	return n;
}

static ssize_t php_socket_recv(php_socket *sock, char *buf, size_t len, int flags)
{
	php_socket_waiter w = PHP_SOCKET_WAITER(SO_RCVTIMEO);
	size_t got = 0;
	ssize_t n;
#ifdef PHP_WIN32
	/* Winsock refuses MSG_WAITALL on a non-blocking socket (WSAEOPNOTSUPP);
	 * the loop below provides it for a Socket that emulates blocking mode */
	int call_flags = sock->nonblocking_fd ? flags & ~MSG_WAITALL : flags;
#else
	int call_flags = flags;
#endif

	if (!php_socket_op_begin(sock, &w.op)) {
		return -1;
	}
	for (;;) {
		n = php_socket_recv_once(sock, &w.op, buf + got, len - got, call_flags);
		if (n > 0) {
			got += n;
			if (got < len && (flags & MSG_WAITALL) && !(flags & MSG_PEEK) && PHP_SOCKET_EMULATES_BLOCKING(sock)) {
				continue;
			}
			break;
		}
		if (n == 0 || w.op.active || !php_socket_wait_retry(sock, &w, PHP_POLLREADABLE, flags)) {
			break;
		}
	}
	php_socket_op_end(sock, &w.op);
	return got > 0 ? (ssize_t) got : n;
}

/* Only for a Socket that emulates blocking mode: sends everything on a
 * stream socket like the blocking call */
static ssize_t php_socket_send_all(php_socket *sock, php_socket_op *o, const char *buf, size_t len,
		int flags)
{
	php_socket_waiter w = PHP_SOCKET_WAITER(SO_SNDTIMEO);
	size_t sent = 0;
	ssize_t n;

	w.op = *o;
	for (;;) {
		n = php_socket_send_once(sock, o, buf + sent, len - sent, flags);
		if (n > 0) {
			sent += n;
			if (sent == len) {
				break;
			}
#ifdef MSG_DONTWAIT
			if (flags & MSG_DONTWAIT) {
				break;
			}
#endif
			continue;
		}
		if (n == 0 || o->active || !php_socket_wait_retry(sock, &w, POLLOUT, flags)) {
			break;
		}
	}
	return sent > 0 ? (ssize_t) sent : n;
}

static int php_socket_connect(php_socket *sock, const struct sockaddr *addr, socklen_t addrlen)
{
	php_socket_op o;
	if (!php_socket_op_begin(sock, &o)) {
		return -1;
	}
	if (o.active) {
		php_deadline dl = php_socket_op_deadline(sock, SO_SNDTIMEO, 0);
		int ret = php_io_connect_ex(o.stream, o.handle, sock->bsd_socket, addr, addrlen, &dl);
		php_socket_op_end(sock, &o);
		php_socket_op_errno(ret);
		if (ret != 0 && errno == PHP_SOCKET_WAIT_TIMEOUT) {
			set_errno(EINPROGRESS);
		}
		return ret;
	}
	int ret = connect(sock->bsd_socket, addr, addrlen);
	if (ret == 0 || !PHP_SOCKET_EMULATES_BLOCKING(sock)) {
		return ret;
	}
#ifdef PHP_WIN32
	if (errno != WSAEWOULDBLOCK) {
		return ret;
	}
	/* a failed connect is reported in the except set */
	int events = POLLOUT | POLLPRI;
#else
	if (errno != EINPROGRESS) {
		return ret;
	}
	int events = POLLOUT;
#endif

	php_socket_waiter w = PHP_SOCKET_WAITER(SO_SNDTIMEO);
	if (!php_socket_wait(sock, &w, events)) {
#ifndef PHP_WIN32
		if (errno == EAGAIN) {
			errno = EINPROGRESS;
		}
#endif
		return -1;
	}

	int err = 0;
	socklen_t errlen = sizeof(err);
	if (getsockopt(sock->bsd_socket, SOL_SOCKET, SO_ERROR, (char *) &err, &errlen) != 0) {
		return -1;
	}
	if (err) {
		set_errno(err);
		return -1;
	}
	return 0;
}

static bool php_open_listen_sock(php_socket *sock, unsigned short port, int backlog) /* {{{ */
{
	struct sockaddr_in  la = {0};

	la.sin_addr.s_addr = htonl(INADDR_ANY);
	la.sin_family = AF_INET;
	la.sin_port = htons(port);

	sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);
	sock->blocking = 1;

	if (IS_INVALID_SOCKET(sock)) {
		PHP_SOCKET_ERROR(sock, "unable to create listening socket", errno);
		return false;
	}

	sock->type = PF_INET;

	if (bind(sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to bind to given address", errno);
		close(sock->bsd_socket);
		return false;
	}

	if (listen(sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to listen on socket", errno);
		close(sock->bsd_socket);
		return false;
	}

	return true;
}
/* }}} */

static bool php_accept_connect(php_socket *in_sock, php_socket *out_sock, struct sockaddr *la, socklen_t *la_len) /* {{{ */
{
	php_socket_op o;
	if (!php_socket_op_begin(in_sock, &o)) {
		return false;
	}
	if (o.active) {
		php_deadline dl = php_socket_op_deadline(in_sock, SO_RCVTIMEO, 0);
		out_sock->bsd_socket = php_io_accept_ex(o.stream, o.handle, in_sock->bsd_socket, la, la_len, &dl);
		php_socket_op_end(in_sock, &o);
		if (IS_INVALID_SOCKET(out_sock)) {
			if (EG(exception)) {
				return false;
			}
			php_socket_op_errno(-1);
			PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
			return false;
		}
#ifndef PHP_WIN32
		int mode = fcntl(out_sock->bsd_socket, F_GETFD);
		if (mode >= 0 && !(mode & FD_CLOEXEC)) {
			fcntl(out_sock->bsd_socket, F_SETFD, mode | FD_CLOEXEC);
		}
#endif
		if (!in_sock->blocking) {
			php_set_sock_blocking(out_sock->bsd_socket, 0);
		}
		out_sock->error = 0;
		out_sock->blocking = 1;
		out_sock->type = la->sa_family;
		return true;
	}
#if defined(HAVE_ACCEPT4)
	int flags = SOCK_CLOEXEC;
	if (!in_sock->blocking) {
		flags |= SOCK_NONBLOCK;
	}

	php_socket_waiter w = PHP_SOCKET_WAITER(SO_RCVTIMEO);
	socklen_t la_size = *la_len;
	do {
		*la_len = la_size;
		out_sock->bsd_socket = accept4(in_sock->bsd_socket, la, la_len, flags);
	} while (IS_INVALID_SOCKET(out_sock) && php_socket_wait_retry(in_sock, &w, PHP_POLLREADABLE, 0));

	if (IS_INVALID_SOCKET(out_sock)) {
		PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		return false;
	}
#else
	php_socket_waiter w = PHP_SOCKET_WAITER(SO_RCVTIMEO);
	socklen_t la_size = *la_len;
	do {
		*la_len = la_size;
		out_sock->bsd_socket = accept(in_sock->bsd_socket, la, la_len);
	} while (IS_INVALID_SOCKET(out_sock) && php_socket_wait_retry(in_sock, &w, PHP_POLLREADABLE, 0));

	if (IS_INVALID_SOCKET(out_sock)) {
		PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		return false;
	}

	/* the accepted socket may inherit the mode the stream gave the listening one */
	if (in_sock->nonblocking_fd) {
		php_set_sock_blocking(out_sock->bsd_socket, in_sock->blocking);
	}

#if !defined(PHP_WIN32)
	/**
	 * for fewer and fewer platforms not supporting accept4 syscall we use fcntl instead,
	 * win32, not having any concept of child process, has no need to address it.
	 */
	int mode;

	if ((mode = fcntl(out_sock->bsd_socket, F_GETFD)) < 0) {
		PHP_SOCKET_ERROR(out_sock, "unable to get fcntl mode on the socket", errno);
		return false;
	}

	int cloexec = (mode | FD_CLOEXEC);

	if (mode != cloexec) {
		if (fcntl(out_sock->bsd_socket, F_SETFD, cloexec) < 0) {
			PHP_SOCKET_ERROR(out_sock, "unable to set cloexec mode on the socket", errno);
			return false;
		}
	}
#endif
#endif

	out_sock->error = 0;
	out_sock->blocking = 1;
	out_sock->type = la->sa_family;

	return true;
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
	if (sock->nonblocking_fd) {
		nonblock = !sock->blocking;
	} else {
		m = fcntl(sock->bsd_socket, F_GETFL);
		if (m < 0) {
			return m;
		}
		nonblock = (m & O_NONBLOCK);
		m = 0;
	}
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
			m = php_socket_recv(sock, t, 1, flags);
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
		if (error == INT_MIN) {
			error = 2147473648;
		} else {
			error = -error - 10000;
		}

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
	socket_object_handlers.offset = offsetof(php_socket, std);
	socket_object_handlers.free_obj = socket_free_obj;
	socket_object_handlers.get_constructor = socket_get_constructor;
	socket_object_handlers.clone_obj = NULL;
	socket_object_handlers.get_gc = socket_get_gc;
	socket_object_handlers.compare = zend_objects_not_comparable;

	address_info_ce = register_class_AddressInfo();
	address_info_ce->create_object = address_info_create_object;
	address_info_ce->default_object_handlers = &address_info_object_handlers;

	memcpy(&address_info_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	address_info_object_handlers.offset = offsetof(php_addrinfo, std);
	address_info_object_handlers.free_obj = address_info_free_obj;
	address_info_object_handlers.get_constructor = address_info_get_constructor;
	address_info_object_handlers.clone_obj = NULL;
	address_info_object_handlers.compare = zend_objects_not_comparable;

	socket_poll_handle_ce = register_class_SocketPollHandle(php_io_poll_handle_class_entry);
	socket_poll_handle_ce->create_object = socket_poll_handle_create_object;
	memcpy(&socket_poll_handle_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	socket_poll_handle_object_handlers.offset = offsetof(php_poll_handle_object, std);
	socket_poll_handle_object_handlers.free_obj = php_poll_handle_object_free;
	socket_poll_handle_object_handlers.get_gc = socket_poll_handle_get_gc;
	socket_poll_handle_object_handlers.clone_obj = NULL;
	socket_poll_handle_ce->default_object_handlers = &socket_poll_handle_object_handlers;

	socket_poll_weak_handle_ce = register_class_SocketPollWeakHandle(
			php_io_poll_weak_handle_class_entry);
	socket_poll_weak_handle_ce->create_object = socket_poll_weak_handle_create_object;
	memcpy(&socket_poll_weak_handle_object_handlers, &std_object_handlers,
			sizeof(zend_object_handlers));
	socket_poll_weak_handle_object_handlers.offset = offsetof(php_poll_handle_object, std);
	socket_poll_weak_handle_object_handlers.free_obj = php_poll_handle_object_free;
	socket_poll_weak_handle_object_handlers.clone_obj = NULL;
	socket_poll_weak_handle_ce->default_object_handlers = &socket_poll_weak_handle_object_handlers;

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

	return num;
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

/* Under a provider socket_select() is what stream_select() is: a zero-timeout poll() over the
 * sets first, then one Any op of a Poll member per Socket and a Timer member for the timeout, with
 * every Socket of the sets frozen for the wait, and the stream a Socket shares its descriptor
 * with frozen and carried like the Socket's own operations carry it. */

typedef struct php_socket_select_member {
	php_socket *sock;
	php_socket_t fd;
	uint32_t events;
} php_socket_select_member;

typedef struct php_socket_select_frozen {
	php_socket **socks;
	php_stream **streams;
	uint32_t n_socks, cap_socks, n_streams, cap_streams;
} php_socket_select_frozen;

static bool php_socket_select_freeze(HashTable *sock_array, php_socket_select_frozen *fz)
{
	ZEND_HASH_FOREACH_VAL(sock_array, zval *element) {
		ZVAL_DEREF(element);
		php_socket *sock = Z_SOCKET_P(element);
		bool seen = false;
		for (uint32_t i = 0; i < fz->n_socks; i++) {
			if (fz->socks[i] == sock) {
				seen = true;
				break;
			}
		}
		if (seen) {
			continue;
		}
		if (sock->in_use) {
			zend_throw_error(NULL, "Concurrent access to a socket");
			return false;
		}
		php_stream *stream = NULL;
		if (!Z_ISUNDEF(sock->zstream)) {
			stream = zend_fetch_resource2_ex(&sock->zstream, NULL, php_file_le_stream(),
					php_file_le_pstream());
		}
		if (stream) {
			if (stream->flags & PHP_STREAM_FLAG_IN_USE) {
				zend_throw_error(NULL, "Concurrent access to a stream");
				return false;
			}
			if (fz->n_streams == fz->cap_streams) {
				fz->cap_streams = fz->cap_streams ? fz->cap_streams * 2 : 8;
				fz->streams = safe_erealloc(fz->streams, fz->cap_streams, sizeof(*fz->streams), 0);
			}
			stream->flags |= PHP_STREAM_FLAG_IN_USE;
			GC_ADDREF(stream->res);
			fz->streams[fz->n_streams++] = stream;
		}
		if (fz->n_socks == fz->cap_socks) {
			fz->cap_socks = fz->cap_socks ? fz->cap_socks * 2 : 8;
			fz->socks = safe_erealloc(fz->socks, fz->cap_socks, sizeof(*fz->socks), 0);
		}
		sock->in_use = true;
		fz->socks[fz->n_socks++] = sock;
	} ZEND_HASH_FOREACH_END();
	return true;
}

static void php_socket_select_unfreeze(php_socket_select_frozen *fz)
{
	for (uint32_t i = 0; i < fz->n_socks; i++) {
		fz->socks[i]->in_use = false;
	}
	for (uint32_t i = 0; i < fz->n_streams; i++) {
		zend_resource *res = fz->streams[i]->res;
		fz->streams[i]->flags &= ~PHP_STREAM_FLAG_IN_USE;
		zend_list_delete(res);
	}
	if (fz->socks) {
		efree(fz->socks);
	}
	if (fz->streams) {
		efree(fz->streams);
	}
}

static void php_socket_select_collect(HashTable *sock_array, uint32_t events,
		php_socket_select_member **members, uint32_t *n, uint32_t *cap)
{
	ZEND_HASH_FOREACH_VAL(sock_array, zval *element) {
		ZVAL_DEREF(element);
		php_socket *sock = Z_SOCKET_P(element);
		php_socket_t fd = (php_socket_t) sock->bsd_socket;
		bool merged = false;
		for (uint32_t i = 0; i < *n; i++) {
			if ((*members)[i].fd == fd) {
				(*members)[i].events |= events;
				merged = true;
				break;
			}
		}
		if (!merged) {
			if (*n == *cap) {
				*cap = *cap ? *cap * 2 : 8;
				*members = safe_erealloc(*members, *cap, sizeof(**members), 0);
			}
			(*members)[*n].sock = sock;
			(*members)[*n].fd = fd;
			(*members)[*n].events = events;
			(*n)++;
		}
	} ZEND_HASH_FOREACH_END();
}

/* Returns the number of ready descriptors, 0 on timeout, -1 with errno */
static int php_socket_select_any(zval *r_array, zval *w_array, zval *e_array, struct timeval *tv,
		fd_set *rfds, fd_set *wfds, fd_set *efds)
{
	php_socket_select_member *members = NULL;
	uint32_t n = 0, cap = 0;
	bool priority = php_poll_backend_supports_priority(PHP_POLL_BACKEND_AUTO);
	php_socket_select_frozen fz = { NULL, NULL, 0, 0, 0, 0 };

	if ((r_array && !php_socket_select_freeze(Z_ARRVAL_P(r_array), &fz))
			|| (w_array && !php_socket_select_freeze(Z_ARRVAL_P(w_array), &fz))
			|| (e_array && !php_socket_select_freeze(Z_ARRVAL_P(e_array), &fz))) {
		php_socket_select_unfreeze(&fz);
		set_errno(ECANCELED);
		return -1;
	}

	if (r_array) {
		php_socket_select_collect(Z_ARRVAL_P(r_array), PHP_POLL_READ, &members, &n, &cap);
	}
	if (w_array) {
		php_socket_select_collect(Z_ARRVAL_P(w_array), PHP_POLL_WRITE, &members, &n, &cap);
	}
	if (e_array) {
		php_socket_select_collect(Z_ARRVAL_P(e_array), priority ? PHP_POLL_PRI : 0, &members, &n, &cap);
	}

	bool checked = false;
	if (n > 0) {
		php_pollfd stack[16];
		php_pollfd *fds = n > sizeof(stack) / sizeof(stack[0]) ? safe_emalloc(n, sizeof(*fds), 0) : stack;
		for (uint32_t i = 0; i < n; i++) {
			fds[i].fd = members[i].fd;
			fds[i].events = ((members[i].events & PHP_POLL_READ) ? POLLIN : 0)
					| ((members[i].events & PHP_POLL_WRITE) ? POLLOUT : 0)
					| ((members[i].events & PHP_POLL_PRI) ? POLLPRI : 0);
			fds[i].revents = 0;
		}
		int ready = php_poll2(fds, n, 0);
		int found = 0;
		if (ready > 0) {
			for (uint32_t i = 0; i < n; i++) {
				short revents = fds[i].revents;
				if (revents & POLLNVAL) {
					found = 0;
					break;
				}
				if ((revents & (POLLIN | POLLHUP | POLLERR)) && (members[i].events & PHP_POLL_READ)) {
					PHP_SAFE_FD_SET(members[i].fd, rfds);
					found++;
				}
				if ((revents & (POLLOUT | POLLERR)) && (members[i].events & PHP_POLL_WRITE)) {
					PHP_SAFE_FD_SET(members[i].fd, wfds);
					found++;
				}
				if ((revents & POLLPRI) && (members[i].events & PHP_POLL_PRI)) {
					PHP_SAFE_FD_SET(members[i].fd, efds);
					found++;
				}
			}
		}
		if (fds != stack) {
			efree(fds);
		}
		if (found > 0 || (ready == 0 && tv && tv->tv_sec == 0 && tv->tv_usec == 0)) {
			efree(members);
			php_socket_select_unfreeze(&fz);
			return found;
		}
		checked = ready == 0;
	}

	php_io_op *ops = safe_emalloc(n + 1, sizeof(php_io_op), 0);
	php_io_op **op_ptrs = safe_emalloc(n + 1, sizeof(php_io_op *), 0);
	php_io_op_result *results = safe_emalloc(n + 1, sizeof(php_io_op_result), 0);
	uint32_t n_members = 0;

	for (uint32_t i = 0; i < n; i++) {
		if (members[i].events == 0) {
			continue;
		}
		php_socket *sock = members[i].sock;
		php_stream *stream = NULL;
		if (!Z_ISUNDEF(sock->zstream)) {
			stream = zend_fetch_resource2_ex(&sock->zstream, NULL, php_file_le_stream(),
					php_file_le_pstream());
		}
		php_io_op_poll(&ops[n_members], stream ? NULL : php_socket_get_weak_handle(sock), members[i].fd,
				members[i].events, php_io_deadline_infinite());
		ops[n_members].stream = stream;
		if (checked) {
			ops[n_members].flags |= PHP_IO_OP_F_CHECKED;
		}
		op_ptrs[n_members] = &ops[n_members];
		n_members++;
	}
	uint32_t timer_index = UINT32_MAX;
	if (tv) {
		php_io_op_timer(&ops[n_members], php_io_deadline_from_timeval(tv));
		op_ptrs[n_members] = &ops[n_members];
		timer_index = n_members;
		n_members++;
	}

	int ret = 0;
	if (n_members == 0) {
		set_errno(EINVAL);
		ret = -1;
	} else {
		php_io_op any;
		php_io_op_result any_result;
		php_io_op_any(&any, op_ptrs, n_members, results);
		if (php_io_run(&any, &any_result) == FAILURE) {
			set_errno(ECANCELED);
			ret = -1;
		} else if (any_result.status == PHP_IO_INTERRUPTED) {
			set_errno(EINTR);
			ret = -1;
		} else {
			for (uint32_t i = 0; i < any.u.any.n_results; i++) {
				uint32_t index = results[i].index;
				if (index == timer_index || (results[i].status != PHP_IO_DONE && results[i].status != PHP_IO_READY)) {
					continue;
				}
				php_socket_t fd = ops[index].fd;
				uint32_t revents = (uint32_t) results[i].res;
				if ((revents & (PHP_POLL_READ | PHP_POLL_HUP | PHP_POLL_ERROR)) && (ops[index].u.poll.events & PHP_POLL_READ)) {
					PHP_SAFE_FD_SET(fd, rfds);
					ret++;
				}
				if ((revents & (PHP_POLL_WRITE | PHP_POLL_ERROR)) && (ops[index].u.poll.events & PHP_POLL_WRITE)) {
					PHP_SAFE_FD_SET(fd, wfds);
					ret++;
				}
				if ((revents & PHP_POLL_PRI) && (ops[index].u.poll.events & PHP_POLL_PRI)) {
					PHP_SAFE_FD_SET(fd, efds);
					ret++;
				}
			}
		}
	}

	efree(results);
	efree(op_ptrs);
	efree(ops);
	if (members) {
		efree(members);
	}
	php_socket_select_unfreeze(&fz);
	return ret;
}

/* {{{ Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(socket_select)
{
	zval			*r_array, *w_array, *e_array;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	PHP_SOCKET		max_fd = 0;
	int				retval, max_set_count = 0;
	zend_long		sec, usec = 0;
	bool		sec_is_null = 0;

	ZEND_PARSE_PARAMETERS_START(4, 5)
		Z_PARAM_ARRAY_EX2(r_array, 1, 1, 0)
		Z_PARAM_ARRAY_EX2(w_array, 1, 1, 0)
		Z_PARAM_ARRAY_EX2(e_array, 1, 1, 0)
		Z_PARAM_LONG_OR_NULL(sec, sec_is_null)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(usec)
	ZEND_PARSE_PARAMETERS_END();

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) {
		retval = php_sock_array_to_fd_set(1, r_array, &rfds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
		if (retval > max_set_count) {
			max_set_count = retval;
		}
	}
	if (w_array != NULL) {
		retval = php_sock_array_to_fd_set(2, w_array, &wfds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
		if (retval > max_set_count) {
			max_set_count = retval;
		}
	}
	if (e_array != NULL) {
		retval = php_sock_array_to_fd_set(3, e_array, &efds, &max_fd);
		if (retval == -1) {
			RETURN_THROWS();
		}
		if (retval > max_set_count) {
			max_set_count = retval;
		}
	}

	if (!max_set_count) {
		zend_value_error("socket_select(): At least one array argument must be passed");
		RETURN_THROWS();
	}

	if (!PHP_SAFE_MAX_FD(max_fd, max_set_count)) {
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

	if (php_io_hooks_active()) {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		retval = php_socket_select_any(r_array, w_array, e_array, tv_p, &rfds, &wfds, &efds);
		if (retval == -1 && EG(exception)) {
			RETURN_THROWS();
		}
	} else {
		retval = select(max_fd+1, &rfds, &wfds, &efds, tv_p);
	}

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
	zend_long	port, backlog = SOMAXCONN;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(port)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(backlog)
	ZEND_PARSE_PARAMETERS_END();

	if (port < 0 || port > USHRT_MAX) {
		zend_argument_value_error(1, "must be between 0 and %u", USHRT_MAX);
		RETURN_THROWS();
	}

	object_init_ex(return_value, socket_ce);
	php_sock = Z_SOCKET_P(return_value);

	if (!php_open_listen_sock(php_sock, (unsigned short)port, backlog)) {
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	object_init_ex(return_value, socket_ce);
	new_sock = Z_SOCKET_P(return_value);

	if (!php_accept_connect(php_sock, new_sock, (struct sockaddr*)&sa, &php_sa_len)) {
		zval_ptr_dtor(return_value);
		if (EG(exception)) {
			RETURN_THROWS();
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Sets nonblocking mode on a socket resource */
PHP_FUNCTION(socket_set_nonblock)
{
	zval		*arg1;
	php_socket	*php_sock;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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

	/* A descriptor switched for a provider keeps its mode: the change is logical */
	if (php_sock->nonblocking_fd && Z_ISUNDEF(php_sock->zstream)) {
		php_sock->blocking = 0;
		RETURN_TRUE;
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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

	if (php_sock->nonblocking_fd && Z_ISUNDEF(php_sock->zstream)) {
		php_sock->blocking = 1;
		RETURN_TRUE;
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
	zend_long	backlog = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(backlog)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

	php_socket *socket = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(socket);
	ENSURE_SOCKET_FREE(socket);

	php_socket_release_io(socket);
	if (!Z_ISUNDEF(socket->zstream)) {
		php_stream *stream = NULL;
		php_stream_from_zval_no_verify(stream, &socket->zstream);
		if (stream != NULL) {
			/* close & destroy stream, incl. removing it from the rsrc list;
			 * resource stored in php_sock->zstream will become invalid */
			php_stream_free(stream,
					PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_CLOSE |
					(stream->is_persistent?PHP_STREAM_FREE_CLOSE_PERSISTENT:0));
		}
	} else {
		if (!IS_INVALID_SOCKET(socket)) {
			close(socket->bsd_socket);
		}
	}

	ZVAL_UNDEF(&socket->zstream);
	socket->bsd_socket = -1;
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

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, length_is_null)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (length < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (length_is_null) {
		length = str_len;
	}

	php_socket_op o;
	if (!php_socket_op_begin(php_sock, &o)) {
		RETURN_THROWS();
	}
	if (PHP_SOCKET_EMULATES_BLOCKING(php_sock)) {
		retval = php_socket_send_all(php_sock, &o, str, MIN(length, str_len), 0);
	} else if (o.active) {
		retval = php_socket_send_once(php_sock, &o, str, MIN(length, str_len), 0);
	} else {
#ifndef PHP_WIN32
		retval = write(php_sock->bsd_socket, str, MIN(length, str_len));
#else
		retval = send(php_sock->bsd_socket, str, min(length, str_len), 0);
#endif
	}
	php_socket_op_end(php_sock, &o);

	if (retval < 0) {
		if (EG(exception)) {
			RETURN_THROWS();
		}
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

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_LONG(length)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(type)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	/* overflow check */
	if (length <= 0 || length == ZEND_LONG_MAX) {
		RETURN_FALSE;
	}

	tmpbuf = zend_string_alloc(length, 0);

	if (type == PHP_NORMAL_READ) {
		retval = php_read(php_sock, ZSTR_VAL(tmpbuf), length, 0);
	} else {
		retval = php_socket_recv(php_sock, ZSTR_VAL(tmpbuf), length, 0);
	}

	if (retval == -1) {
		if (EG(exception)) {
			zend_string_efree(tmpbuf);
			RETURN_THROWS();
		}
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
	zval					*arg1, *addr, *objint = NULL;
	php_sockaddr_storage	sa_storage = {0};
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6		*sin6;
#endif
#ifdef AF_PACKET
	struct sockaddr_ll              *sll;
#endif
	char					addrbuf[INET6_ADDRSTRLEN];
	struct sockaddr_un		*s_un;
	const char				*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_ZVAL(addr)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(objint)
	ZEND_PARSE_PARAMETERS_END();

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

			if (objint != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(objint, htons(sin6->sin6_port));
			}
			RETURN_TRUE;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			addr_string = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, sizeof(addrbuf));
			ZEND_TRY_ASSIGN_REF_STRING(addr, addr_string);

			if (objint != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(objint, htons(sin->sin_port));
			}
			RETURN_TRUE;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(addr, s_un->sun_path);
			RETURN_TRUE;
#ifdef AF_PACKET
		case AF_PACKET:
			sll = (struct sockaddr_ll *) sa;
			char ifrname[IFNAMSIZ];

			if (UNEXPECTED(!if_indextoname(sll->sll_ifindex, ifrname))) {
				zend_throw_error(NULL, "invalid interface index");
				RETURN_THROWS();
			}

			ZEND_TRY_ASSIGN_REF_STRING(addr, ifrname);
			if (objint != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(objint, sll->sll_ifindex);
			}
			RETURN_TRUE;
#endif

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_PACKET, AF_INET, or AF_INET6");
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

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_ZVAL(arg2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(arg3)
	ZEND_PARSE_PARAMETERS_END();

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
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			addr_string = inet_ntop(AF_INET, &sin->sin_addr, addrbuf, sizeof(addrbuf));
			ZEND_TRY_ASSIGN_REF_STRING(arg2, addr_string);

			if (arg3 != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(arg3, htons(sin->sin_port));
			}

			RETURN_TRUE;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(arg2, s_un->sun_path);
			RETURN_TRUE;

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket_create)
{
	zend_long	domain, type, checktype, protocol;
	php_socket	*php_sock;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(domain)
		Z_PARAM_LONG(type)
		Z_PARAM_LONG(protocol)
	ZEND_PARSE_PARAMETERS_END();

	if (domain != AF_UNIX
#ifdef HAVE_IPV6
		&& domain != AF_INET6
#endif
#ifdef AF_PACKET
		&& domain != AF_PACKET
#endif
		&& domain != AF_INET) {
		zend_argument_value_error(1, "must be one of AF_UNIX, AF_PACKET, AF_INET6, or AF_INET");
		RETURN_THROWS();
	}

	checktype = type;
#ifdef SOCK_NONBLOCK
	checktype &= ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
#endif

	if (checktype > 10) {
		zend_argument_value_error(2, "must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET,"
			" SOCK_RAW, or SOCK_RDM"
#ifdef SOCK_NONBLOCK
			" optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK"
#endif
		);
		RETURN_THROWS();
	}

	PHP_ETH_PROTO_CHECK(protocol, domain);

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
	zend_string                     *addr;
	int					retval;
	zend_long				port;
	bool				port_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(resource_socket, socket_ce)
		Z_PARAM_STR(addr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(port, port_is_null)
	ZEND_PARSE_PARAMETERS_END();

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

			sin6.sin6_family = AF_INET6;
			sin6.sin6_port   = htons((unsigned short int)port);

			if (! php_set_inet6_addr(&sin6, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = php_socket_connect(php_sock, (struct sockaddr *)&sin6, sizeof(struct sockaddr_in6));
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

			retval = php_socket_connect(php_sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
			break;
		}

		case AF_UNIX: {
			struct sockaddr_un s_un = {0};

			if (ZSTR_LEN(addr) >= sizeof(s_un.sun_path)) {
				zend_argument_value_error(2, "must be less than %d", sizeof(s_un.sun_path));
				RETURN_THROWS();
			}

			s_un.sun_family = AF_UNIX;
			memcpy(&s_un.sun_path, ZSTR_VAL(addr), ZSTR_LEN(addr));
			retval = php_socket_connect(php_sock, (struct sockaddr *) &s_un,
				(socklen_t)(offsetof(struct sockaddr_un, sun_path) + ZSTR_LEN(addr)));
			break;
		}

		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET, or AF_INET6");
			RETURN_THROWS();
		}

	if (retval != 0) {
		if (EG(exception)) {
			RETURN_THROWS();
		}
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(arg1)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_LONG_EXCEEDS_INT(arg1)) {
		zend_argument_value_error(1, "must be between %d and %d", INT_MIN, INT_MAX);
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
	zend_string                             *addr;
	zend_long					objint = 0;
	zend_long					retval = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_STR(addr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(objint)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (objint < 0 || objint > USHRT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %u", USHRT_MAX);
		RETURN_THROWS();
	}

	switch(php_sock->type) {
		case AF_UNIX:
			{
				struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;

				sa->sun_family = AF_UNIX;

				if (ZSTR_LEN(addr) >= sizeof(sa->sun_path)) {
					zend_argument_value_error(2, "must be less than %d", sizeof(sa->sun_path));
					RETURN_THROWS();
				}
				memcpy(&sa->sun_path, ZSTR_VAL(addr), ZSTR_LEN(addr));

				retval = bind(php_sock->bsd_socket, (struct sockaddr *) sa,
						offsetof(struct sockaddr_un, sun_path) + ZSTR_LEN(addr));
				break;
			}

		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) sock_type;

				sa->sin_family = AF_INET;
				sa->sin_port = htons((unsigned short) objint);

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
				sa->sin6_port = htons((unsigned short) objint);

				if (! php_set_inet6_addr(sa, addr, php_sock)) {
					RETURN_FALSE;
				}

				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in6));
				break;
			}
#endif
#ifdef AF_PACKET
		case AF_PACKET:
			{
				struct sockaddr_ll *sa = (struct sockaddr_ll *) sock_type;
				socklen_t sa_len = sizeof(sa);

				if (getsockname(php_sock->bsd_socket, sock_type, &sa_len) < 0) {
					zend_value_error("invalid AF_PACKET socket");
					RETURN_THROWS();
				}

				sa->sll_ifindex = if_nametoindex(ZSTR_VAL(addr));

				retval = bind(php_sock->bsd_socket, sock_type, sizeof(struct sockaddr_ll));
				break;
			}
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_PACKET, AF_INET, or AF_INET6");
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

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_OBJECT_OF_CLASS(php_sock_res, socket_ce)
		Z_PARAM_ZVAL(buf)
		Z_PARAM_LONG(len)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(php_sock_res);
	ENSURE_SOCKET_VALID(php_sock);

	/* overflow check */
	if (len <= 0 || len == ZEND_LONG_MAX) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(len, 0);

	if ((retval = php_socket_recv(php_sock, ZSTR_VAL(recv_buf), len, flags)) < 1) {
		zend_string_efree(recv_buf);
		ZEND_TRY_ASSIGN_REF_NULL(buf);
	} else {
		ZSTR_LEN(recv_buf) = retval;
		ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(buf, recv_buf);
	}

	if (retval == -1) {
		if (EG(exception)) {
			RETURN_THROWS();
		}
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

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_STRING(buf, buf_len)
		Z_PARAM_LONG(len)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	php_socket_op o;
	if (!php_socket_op_begin(php_sock, &o)) {
		RETURN_THROWS();
	}
	if (PHP_SOCKET_EMULATES_BLOCKING(php_sock)) {
		retval = php_socket_send_all(php_sock, &o, buf, (buf_len < (size_t)len ? buf_len : (size_t)len), flags);
	} else if (o.active) {
		retval = php_socket_send_once(php_sock, &o, buf, (buf_len < (size_t)len ? buf_len : (size_t)len), flags);
	} else {
		retval = send(php_sock->bsd_socket, buf, (buf_len < (size_t)len ? buf_len : (size_t)len), flags);
	}
	php_socket_op_end(php_sock, &o);

	if (retval == (size_t)-1) {
		if (EG(exception)) {
			RETURN_THROWS();
		}
		PHP_SOCKET_ERROR(php_sock, "Unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Receives data from a socket, connected or not */
PHP_FUNCTION(socket_recvfrom)
{
	zval				*zsocket, *zdata, *zaddr, *zport = NULL;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
#ifdef AF_PACKET
	struct sockaddr_ll	sll;
#endif
	char				addrbuf[INET6_ADDRSTRLEN];
	socklen_t			slen;
	int					retval;
	zend_long			length, flags;
	const char			*address;
	zend_string			*recv_buf;
	php_socket_waiter	w = PHP_SOCKET_WAITER(SO_RCVTIMEO);

	ZEND_PARSE_PARAMETERS_START(5, 6)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
		Z_PARAM_ZVAL(zdata)
		Z_PARAM_LONG(length)
		Z_PARAM_LONG(flags)
		Z_PARAM_ZVAL(zaddr)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(zport)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(php_sock);

#ifdef AF_PACKET
	/* On packet sockets, restrict flags to a finite safe subset. In
	 * particular MSG_TRUNC must be excluded: it makes recvfrom() report the
	 * untruncated frame length, which can exceed the buffer. */
	if (php_sock->type == AF_PACKET) {
		const zend_long allowed_flags = 0
#ifdef MSG_OOB
			| MSG_OOB
#endif
#ifdef MSG_PEEK
			| MSG_PEEK
#endif
#ifdef MSG_WAITALL
			| MSG_WAITALL
#endif
#ifdef MSG_DONTWAIT
			| MSG_DONTWAIT
#endif
#ifdef MSG_ERRQUEUE
			| MSG_ERRQUEUE
#endif
#ifdef MSG_CMSG_CLOEXEC
			| MSG_CMSG_CLOEXEC
#endif
			;

		if (flags & ~allowed_flags) {
			zend_argument_value_error(4, "must be a combination of MSG_OOB, MSG_PEEK, MSG_WAITALL, MSG_DONTWAIT, MSG_ERRQUEUE, and MSG_CMSG_CLOEXEC for AF_PACKET sockets");
			RETURN_THROWS();
		}
	}
#endif

	/* overflow check */
	/* Shouldthrow ? */

	if (length <= 0 || length > ZEND_LONG_MAX - 1) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(length + 1, 0);

	switch (php_sock->type) {
		case AF_UNIX:
			slen = sizeof(s_un);
			memset(&s_un, 0, slen);
			s_un.sun_family = AF_UNIX;

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), length, flags, (struct sockaddr *)&s_un, (socklen_t *)&slen);
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, PHP_POLLREADABLE, flags));
			php_socket_op_end(php_sock, &w.op);

			if (retval < 0) {
				if (EG(exception)) {
					RETURN_THROWS();
				}
				PHP_SOCKET_ERROR(php_sock, "Unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = MIN((size_t)retval, (size_t)length);
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			ZEND_TRY_ASSIGN_REF_NEW_STR(zdata, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(zaddr, s_un.sun_path);
			break;

		case AF_INET:
			slen = sizeof(sin);
			memset(&sin, 0, slen);
			sin.sin_family = AF_INET;

			if (zport == NULL) {
				zend_string_efree(recv_buf);
				zend_throw_exception(
					zend_ce_argument_count_error,
					"socket_recvfrom() expects exactly 6 arguments when argument #1 ($socket) is of type AF_INET or AF_INET6",
					0);
				RETURN_THROWS();
			}

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), length, flags, (struct sockaddr *)&sin, (socklen_t *)&slen);
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, PHP_POLLREADABLE, flags));
			php_socket_op_end(php_sock, &w.op);

			if (retval < 0) {
				if (EG(exception)) {
					RETURN_THROWS();
				}
				PHP_SOCKET_ERROR(php_sock, "Unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = MIN((size_t)retval, (size_t)length);
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			address = inet_ntop(AF_INET, &sin.sin_addr, addrbuf, sizeof(addrbuf));

			ZEND_TRY_ASSIGN_REF_NEW_STR(zdata, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(zaddr, address ? address : "0.0.0.0");
			ZEND_TRY_ASSIGN_REF_LONG(zport, ntohs(sin.sin_port));
			break;
#ifdef HAVE_IPV6
		case AF_INET6:
			slen = sizeof(sin6);
			memset(&sin6, 0, slen);
			sin6.sin6_family = AF_INET6;

			if (zport == NULL) {
				zend_string_efree(recv_buf);
				zend_throw_exception(
					zend_ce_argument_count_error,
					"socket_recvfrom() expects exactly 6 arguments when argument #1 ($socket) is of type AF_INET or AF_INET6",
					0);
				RETURN_THROWS();
			}

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), length, flags, (struct sockaddr *)&sin6, (socklen_t *)&slen);
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, PHP_POLLREADABLE, flags));
			php_socket_op_end(php_sock, &w.op);

			if (retval < 0) {
				if (EG(exception)) {
					RETURN_THROWS();
				}
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = MIN((size_t)retval, (size_t)length);
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			inet_ntop(AF_INET6, &sin6.sin6_addr,  addrbuf, sizeof(addrbuf));

			ZEND_TRY_ASSIGN_REF_NEW_STR(zdata, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(zaddr, addrbuf[0] ? addrbuf : "::");
			ZEND_TRY_ASSIGN_REF_LONG(zport, ntohs(sin6.sin6_port));
			break;
#endif
#ifdef AF_PACKET
		case AF_PACKET: {
			char ifrname[IFNAMSIZ];

			slen = sizeof(sll);
			memset(&sll, 0, slen);

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), length, flags, (struct sockaddr *)&sll, (socklen_t *)&slen);
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, PHP_POLLREADABLE, flags));
			php_socket_op_end(php_sock, &w.op);

			if (retval < 0) {
				if (EG(exception)) {
					RETURN_THROWS();
				}
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = MIN((size_t)retval, (size_t)length);
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			if (UNEXPECTED(!if_indextoname(sll.sll_ifindex, ifrname))) {
				PHP_SOCKET_ERROR(php_sock, "unable to get the interface name", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}

			ZEND_TRY_ASSIGN_REF_NEW_STR(zdata, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(zaddr, ifrname);

			if (zport) {
				ZEND_TRY_ASSIGN_REF_LONG(zport, sll.sll_ifindex);
			}
			break;
		}
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_PACKET, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(socket_sendto)
{
	zval				*zsocket;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#ifdef HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
#ifdef AF_PACKET
	struct sockaddr_ll	sll;
#endif
	int					retval;
	size_t              buf_len;
	zend_long			len, flags, port = 0;
	php_socket_waiter	w = PHP_SOCKET_WAITER(SO_SNDTIMEO);
	bool           port_is_null = 1;
	char				*buf;
	zend_string *addr;

	ZEND_PARSE_PARAMETERS_START(5, 6)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
		Z_PARAM_STRING(buf, buf_len)
		Z_PARAM_LONG(len)
		Z_PARAM_LONG(flags)
		Z_PARAM_STR(addr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(port, port_is_null)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(php_sock);

#ifdef AF_PACKET
	if (php_sock->type != AF_PACKET) {
#endif
		if (port < 0 || port > USHRT_MAX) {
			zend_argument_value_error(6, "must be between 0 and %u", USHRT_MAX);
			RETURN_THROWS();
		}
#ifdef AF_PACKET
	}
#endif

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	switch (php_sock->type) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));

			if (ZSTR_LEN(addr) >= sizeof(s_un.sun_path)) {
				zend_argument_value_error(5, "must be less than %d", sizeof(s_un.sun_path));
				RETURN_THROWS();
			}

			s_un.sun_family = AF_UNIX;
			memcpy(s_un.sun_path, ZSTR_VAL(addr), ZSTR_LEN(addr) + 1);

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len,	flags, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, POLLOUT, flags));
			php_socket_op_end(php_sock, &w.op);
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

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin, sizeof(sin));
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, POLLOUT, flags));
			php_socket_op_end(php_sock, &w.op);
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

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin6, sizeof(sin6));
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, POLLOUT, flags));
			php_socket_op_end(php_sock, &w.op);
			break;
#endif
#ifdef AF_PACKET
		case AF_PACKET:
			if (port_is_null) {
				zend_argument_value_error(6, "cannot be null when the socket type is AF_PACKET");
				RETURN_THROWS();
			}

			memset(&sll, 0, sizeof(sll));
			sll.sll_family = AF_PACKET;
			sll.sll_ifindex = (int)port;

			if (!php_socket_op_begin(php_sock, &w.op)) {
				RETURN_THROWS();
			}
			do {
				retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *)&sll, sizeof(sll));
			} while (retval == -1 && php_socket_wait_retry(php_sock, &w, POLLOUT, flags));
			php_socket_op_end(php_sock, &w.op);
			break;
#endif
		default:
			zend_argument_value_error(1, "must be one of AF_UNIX, AF_PACKET, AF_INET, or AF_INET6");
			RETURN_THROWS();
	}

	if (retval == -1) {
		if (EG(exception)) {
			RETURN_THROWS();
		}
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
	DWORD				timeout = 0;
#else
	struct timeval timeout;
#endif
	socklen_t		optlen;
	php_socket		*php_sock;
	int				other_val;
	zend_long			level, optname;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_LONG(level)
		Z_PARAM_LONG(optname)
	ZEND_PARSE_PARAMETERS_END();

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

#ifdef TCP_FUNCTION_BLK
		case TCP_FUNCTION_BLK:
#ifdef TCP_FUNCTION_ALIAS
		case TCP_FUNCTION_ALIAS:
#endif
		{

			struct tcp_function_set tsf = {0};
			optlen = sizeof(tsf);

			if (getsockopt(php_sock->bsd_socket, level, optname, &tsf, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			array_init_size(return_value, 2);

			add_assoc_string(return_value, "function_set_name", tsf.function_set_name);
			add_assoc_long(return_value, "pcbcnt", tsf.pcbcnt);
			return;
		}
#endif
		}
	}

	if (level == SOL_SOCKET) {
		switch (optname) {
#ifdef SO_LINGER_SEC
			case SO_LINGER_SEC:
#endif
			case SO_LINGER:
				optlen = sizeof(linger_val);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&linger_val, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				array_init_size(return_value, 2);
				add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
				add_assoc_long(return_value, "l_linger", linger_val.l_linger);
				return;

			case SO_RCVTIMEO:
			case SO_SNDTIMEO:
				optlen = sizeof(timeout);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&timeout, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "Unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

#ifndef PHP_WIN32
				tv.tv_sec = timeout.tv_sec;
				tv.tv_usec = timeout.tv_usec;
#else
				tv.tv_sec = timeout ? (long)(timeout / 1000) : 0;
				tv.tv_usec = timeout ? (long)((timeout % 1000) * 1000) : 0;
#endif

				array_init_size(return_value, 2);

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

				array_init_size(return_value, 9);

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

				array_init_size(return_value, 1);

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

				size_t arrlen = optlen / sizeof(struct fil_info);

				array_init_size(return_value, arrlen);
				zend_hash_real_init_packed(Z_ARRVAL_P(return_value));

				for (i = 0; i < arrlen; i++) {
					add_index_string(return_value, i, fi[i].fi_name);
				}

				return;
			}
		}
	}
#endif

	optlen = sizeof(other_val);
	other_val = 0;

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
	DWORD						timeout;
#else
	struct					timeval tv;
#endif
#ifdef SO_ATTACH_REUSEPORT_CBPF
	struct sock_filter cbpf[8] = {0};
	struct sock_fprog bpfprog;
#endif
	zend_long					level, optname;
	void 					*opt_ptr;
	HashTable		 		*opt_ht;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_LONG(level)
		Z_PARAM_LONG(optname)
		Z_PARAM_ZVAL(arg4)
	ZEND_PARSE_PARAMETERS_END();

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
			if (Z_TYPE_P(arg4) != IS_STRING) {
				zend_argument_type_error(4, "must be of type string when argument #3 ($option) is TCP_CONGESTION, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			opt_ptr = Z_STRVAL_P(arg4);
			optlen = Z_STRLEN_P(arg4);
			if (setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to set socket option", errno);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}
#endif

#ifdef TCP_FUNCTION_BLK
		case TCP_FUNCTION_BLK: {
			if (Z_TYPE_P(arg4) != IS_STRING) {
				zend_argument_type_error(4, "must be of type string when argument #3 ($option) is TCP_FUNCTION_BLK, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			if (zend_str_has_nul_byte(Z_STR_P(arg4))) {
				zend_argument_value_error(4, "must not contain null bytes when argument #3 ($option) is TCP_FUNCTION_BLK");
				RETURN_THROWS();
			}
			/* [...] the unique name of the TCP stack, and should be no longer than
			 * TCP_FUNCTION_NAME_LEN_MAX-1 characters in length.
			 * https://github.com/freebsd/freebsd-src/blob/da2c88dfcf4f425e6e0a58d6df3a7c8e88d8df92/share/man/man9/tcp_functions.9#L193C1-L194C50 */
			if (Z_STRLEN_P(arg4) >= TCP_FUNCTION_NAME_LEN_MAX) {
				zend_argument_value_error(4, "must be less than %zu bytes when argument #3 ($option) is TCP_FUNCTION_BLK", TCP_FUNCTION_NAME_LEN_MAX);
				RETURN_THROWS();
			}
			struct tcp_function_set tfs = {0};
			/* Copy the trailing nul byte */
			memcpy(tfs.function_set_name, Z_STRVAL_P(arg4), Z_STRLEN_P(arg4)+1);

			optlen = sizeof(tfs);
			opt_ptr = &tfs;
			if (setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "Unable to set socket option", errno);
				RETURN_FALSE;
			}

			RETURN_TRUE;
		}
#endif

#if defined(TCP_USER_TIMEOUT)
		case TCP_USER_TIMEOUT: {
			zend_long timeout = zval_get_long(arg4);

			// TCP_USER_TIMEOUT unsigned int
			if (timeout < 0 || timeout > UINT_MAX) {
				zend_argument_value_error(4, "must be between 0 and %u", UINT_MAX);
				RETURN_THROWS();
			}

			unsigned int val = (unsigned int)timeout;
			optlen = sizeof(val);
			opt_ptr = &val;
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
#ifdef SO_LINGER_SEC
		case SO_LINGER_SEC:
#endif
		case SO_LINGER: {
			const char l_onoff_key[] = "l_onoff";
			const char l_linger_key[] = "l_linger";
			zval *l_onoff, *l_linger;

			if (Z_TYPE_P(arg4) != IS_ARRAY) {
				if (UNEXPECTED(Z_TYPE_P(arg4) != IS_OBJECT)) {
					zend_argument_type_error(4, "must be of type array when argument #3 ($option) is SO_LINGER, %s given", zend_zval_value_name(arg4));
					RETURN_THROWS();
				} else {
					opt_ht = Z_OBJPROP_P(arg4);
				}
			} else {
				opt_ht = Z_ARRVAL_P(arg4);
			}

			if ((l_onoff = zend_hash_str_find(opt_ht, l_onoff_key, sizeof(l_onoff_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", l_onoff_key);
				RETURN_THROWS();
			}
			if ((l_linger = zend_hash_str_find(opt_ht, l_linger_key, sizeof(l_linger_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", l_linger_key);
				RETURN_THROWS();
			}

			zend_long val_lonoff = zval_get_long(l_onoff);
			zend_long val_linger = zval_get_long(l_linger);

			if (val_lonoff < 0 || val_lonoff > USHRT_MAX) {
				zend_argument_value_error(4, "\"%s\" must be between 0 and %u", l_onoff_key, USHRT_MAX);
				RETURN_THROWS();
			}

			if (val_linger < 0 || val_linger > USHRT_MAX) {
				zend_argument_value_error(4, "\"%s\" must be between 0 and %u", l_linger_key, USHRT_MAX);
				RETURN_THROWS();
			}

			lv.l_onoff = (unsigned short)val_lonoff;
			lv.l_linger = (unsigned short)val_linger;

			optlen = sizeof(lv);
			opt_ptr = &lv;
			break;
		}

		case SO_RCVTIMEO:
		case SO_SNDTIMEO: {
			const char sec_key[] = "sec";
			const char usec_key[] = "usec";
			zval *sec, *usec;

			if (Z_TYPE_P(arg4) != IS_ARRAY) {
				if (UNEXPECTED(Z_TYPE_P(arg4) != IS_OBJECT)) {
					zend_argument_type_error(4, "must be of type array when argument #3 ($option) is %s, %s given",
							optname == SO_RCVTIMEO ? "SO_RCVTIMEO" : "SO_SNDTIMEO",
							zend_zval_value_name(arg4));
					RETURN_THROWS();
				} else {
					opt_ht = Z_OBJPROP_P(arg4);
				}
			} else {
				opt_ht = Z_ARRVAL_P(arg4);
			}

			if ((sec = zend_hash_str_find(opt_ht, sec_key, sizeof(sec_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", sec_key);
				RETURN_THROWS();
			}
			if ((usec = zend_hash_str_find(opt_ht, usec_key, sizeof(usec_key) - 1)) == NULL) {
				zend_argument_value_error(4, "must have key \"%s\"", usec_key);
				RETURN_THROWS();
			}

			zend_long valsec = zval_get_long(sec);
			zend_long valusec = zval_get_long(usec);
#ifndef PHP_WIN32
			tv.tv_sec = valsec;
			tv.tv_usec = valusec;
			optlen = sizeof(tv);
			opt_ptr = &tv;
#else
			if (valsec < 0 || valsec > ULONG_MAX / 1000) {
				zend_argument_value_error(4, "\"%s\" must be between 0 and %u", sec_key, (ULONG_MAX / 1000));
				RETURN_THROWS();
			}

			timeout = valsec * 1000;


			/*
			 * We deliberately throw if (valusec / 1000) > ULONG_MAX, treating it as a programmer error.
			 * On Windows, ULONG_MAX = 2^32, unlike ZEND_LONG_MAX = 2^63.
			 */
			if (valusec < 0 || timeout > ULONG_MAX - (valusec / 1000)) {
				zend_argument_value_error(4, "\"%s\" must be between 0 and %u", usec_key, (DWORD)(ULONG_MAX - (valusec / 1000)));
				RETURN_THROWS();
			}

			timeout += valusec / 1000;
			optlen = sizeof(timeout);
			opt_ptr = &timeout;
#endif
			break;
		}
#ifdef SO_BINDTODEVICE
		case SO_BINDTODEVICE: {
			if (Z_TYPE_P(arg4) != IS_STRING) {
				zend_argument_type_error(4, "must be of type string when argument #3 ($option) is SO_BINDTODEVICE, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			opt_ptr = Z_STRVAL_P(arg4);
			optlen = Z_STRLEN_P(arg4);
			break;
		}
#endif

#ifdef SO_ACCEPTFILTER
		case SO_ACCEPTFILTER: {
			if (Z_TYPE_P(arg4) != IS_STRING) {
				zend_argument_type_error(4, "must be of type string when argument #3 ($option) is SO_ACCEPTFILTER, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			if (zend_str_has_nul_byte(Z_STR_P(arg4))) {
				zend_argument_value_error(4, "must not contain null bytes when argument #3 ($option) is SO_ACCEPTFILTER");
				RETURN_THROWS();
			}
			struct accept_filter_arg af = {0};
			if (Z_STRLEN_P(arg4) >= sizeof(af.af_name)) {
				zend_argument_value_error(4, "must be less than %zu bytes when argument #3 ($option) is SO_ACCEPTFILTER", sizeof(af.af_name));
				RETURN_THROWS();
			}
			/* Copy the trailing nul byte */
			memcpy(af.af_name, Z_STRVAL_P(arg4), Z_STRLEN_P(arg4)+1);
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
				zend_argument_type_error(4, "must be of type string when argument #3 ($option) is FIL_ATTACH or FIL_DETACH, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			opt_ptr = Z_STRVAL_P(arg4);
			optlen = Z_STRLEN_P(arg4);
			break;
		}
#endif

#ifdef SO_ATTACH_REUSEPORT_CBPF
		case SO_ATTACH_REUSEPORT_CBPF: {
			if (level != SOL_SOCKET) {
				php_error_docref(NULL, E_WARNING, "Invalid level");
				RETURN_FALSE;
			}
			if (Z_TYPE_P(arg4) != IS_LONG) {
				zend_argument_type_error(4, "must be of type int when argument #3 ($option) is SO_ATTACH_REUSEPORT_CBPF, %s given", zend_zval_value_name(arg4));
				RETURN_THROWS();
			}
			zend_long cbpf_val = zval_get_long(arg4);

			if (!cbpf_val) {
#ifdef SO_DETACH_REUSEPORT_BPF
				ov = 1;
				optlen = sizeof(ov);
				opt_ptr = &ov;
				optname = SO_DETACH_REUSEPORT_BPF;
#else
				php_error_docref(NULL, E_WARNING, "Detaching a reuseport CBPF filter is unsupported");
				RETURN_FALSE;
#endif
			} else {
				uint32_t k = (uint32_t)cbpf_val;

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

#if defined(UDP_SEGMENT)
		case UDP_SEGMENT: {
			ov = zval_get_long(arg4);

			// UDP segmentation offload maximum size or 0 to disable it
			if (ov < 0 || ov > USHRT_MAX) {
				zend_argument_value_error(4, "must be between 0 and %u", USHRT_MAX);
				RETURN_THROWS();
			}

			optlen = sizeof(ov);
			opt_ptr = &ov;
			break;
		}
#endif

		default:
default_case:
			ov = zval_get_long(arg4);

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
	zend_long		domain, type, checktype, protocol;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(domain)
		Z_PARAM_LONG(type)
		Z_PARAM_LONG(protocol)
		Z_PARAM_ZVAL(fds_array_zval)
	ZEND_PARSE_PARAMETERS_END();

	if (domain != AF_INET
#ifdef HAVE_IPV6
		&& domain != AF_INET6
#endif
		&& domain != AF_UNIX) {
		zend_argument_value_error(1, "must be one of AF_UNIX, AF_INET6, or AF_INET");
		RETURN_THROWS();
	}

	checktype = type;
#ifdef SOCK_NONBLOCK
	checktype &= ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
#endif

	if (checktype > 10) {
		zend_argument_value_error(2, "must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET,"
			" SOCK_RAW, or SOCK_RDM"
#ifdef SOCK_NONBLOCK
			" optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK"
#endif
		);
		RETURN_THROWS();
	}

	if (socketpair(domain, type, protocol, fds_array) != 0) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket pair [%d]: %s", errno, sockets_strerror(errno));
		RETURN_FALSE;
	}

	fds_array_zval = zend_try_array_init_size(fds_array_zval, 2);
	if (!fds_array_zval) {
		RETURN_THROWS();
	}

	object_init_ex(&retval[0], socket_ce);
	php_sock[0] = Z_SOCKET_P(&retval[0]);

	object_init_ex(&retval[1], socket_ce);
	php_sock[1] = Z_SOCKET_P(&retval[1]);

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

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(how_shutdown)
	ZEND_PARSE_PARAMETERS_END();

	php_sock = Z_SOCKET_P(arg1);
	ENSURE_SOCKET_VALID(php_sock);
	ENSURE_SOCKET_FREE(php_sock);

	if (how_shutdown < SHUT_RD || how_shutdown > SHUT_RDWR) {
		zend_argument_value_error(2, "must be one of SHUT_RD, SHUT_WR or SHUT_RDWR");
		RETURN_THROWS();
	}

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(arg1, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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
		return false;
	}

	/* determine blocking mode */
#ifndef PHP_WIN32
	t = fcntl(socket, F_GETFL);
	if (t == -1) {
		PHP_SOCKET_ERROR(retsock, "Unable to obtain blocking state", errno);
		return false;
	} else {
		retsock->blocking = !(t & O_NONBLOCK);
	}
#endif

	return true;
}

/* {{{ Imports a stream that encapsulates a socket into a socket extension resource. */
PHP_FUNCTION(socket_import_stream)
{
	zval				 *zstream;
	php_stream			 *stream;
	php_socket			 *retsock = NULL;
	PHP_SOCKET			 socket; /* fd */

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(zstream)
	ZEND_PARSE_PARAMETERS_END();
	php_stream_from_zval(stream, zstream);

	if (php_stream_cast(stream, PHP_STREAM_AS_SOCKETD, (void**)&socket, 1) == FAILURE) {
		/* error supposedly already shown */
		RETURN_FALSE;
	}

	object_init_ex(return_value, socket_ce);
	retsock = Z_SOCKET_P(return_value);

	if (!socket_import_file_descriptor(socket, retsock)) {
		retsock->bsd_socket = -1;
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	/* socket streams keep the descriptor non-blocking and report their own mode */
	zval meta;
	array_init(&meta);
	if (php_stream_set_option(stream, PHP_STREAM_OPTION_META_DATA_API, 0, &meta) == PHP_STREAM_OPTION_RETURN_OK) {
		zval *blocked = zend_hash_str_find(Z_ARRVAL(meta), "blocked", sizeof("blocked") - 1);
#ifdef PHP_WIN32
		retsock->blocking = blocked ? zend_is_true(blocked) : 1;
		retsock->nonblocking_fd = blocked != NULL;
#else
		if (blocked && !retsock->blocking) {
			retsock->blocking = zend_is_true(blocked);
			retsock->nonblocking_fd = true;
		}
#endif
	}
	zval_ptr_dtor(&meta);

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
	ZEND_PARSE_PARAMETERS_END();

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
	stream_data->restore_blocking = false;
	php_netstream_set_nonblocking(stream_data);
#ifdef PHP_WIN32
	stream_data->restore_blocking = socket->blocking;
#endif
	socket->nonblocking_fd = true;
	/* The stream is the registrant from now on */
	if (socket->weak_handle) {
		php_io_unregister_all(&PHP_POLL_HANDLE_OBJ_FROM_ZOBJ(socket->weak_handle)->registrations);
	}

	php_stream_to_zval(stream, &socket->zstream);

	RETURN_COPY(&socket->zstream);
}
/* }}} */

/* {{{ Gets array with contents of getaddrinfo about the given hostname. */
PHP_FUNCTION(socket_addrinfo_lookup)
{
	zend_string *service = NULL;
	zend_string *hostname, *key;
	zval *hint, *zhints = NULL, *error_code = NULL;
	int ret = 0;

	struct addrinfo hints, *result, *rp;
	php_addrinfo *res;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STR(hostname)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(service)
		Z_PARAM_ARRAY(zhints)
		Z_PARAM_ZVAL_OR_NULL(error_code)
	ZEND_PARSE_PARAMETERS_END();

	memset(&hints, 0, sizeof(hints));

#if defined(PHP_WIN32)
#  if !defined(AF_MAX)
#    define AF_MAX (AF_BTH + 1)
#  endif
#endif

	if (zhints) {
		if (UNEXPECTED(HT_IS_PACKED(Z_ARRVAL_P(zhints)))) {
			zend_argument_value_error(3, "must only contain array keys \"ai_flags\", \"ai_socktype\", "
					"\"ai_protocol\", or \"ai_family\"");
			RETURN_THROWS();
		}

		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zhints), key, hint) {
			if (key) {
				bool failed = false;
				if (zend_string_equals_literal(key, "ai_flags")) {
					zend_long val = zval_try_get_long(hint, &failed);
					if (failed) {
						zend_argument_type_error(3, "\"ai_flags\" key must be of type int, %s given", zend_zval_type_name(hint));
						RETURN_THROWS();
					}
					if (val < 0 || val > INT_MAX) {
						zend_argument_value_error(3, "\"ai_flags\" key must be between 0 and %d", INT_MAX);
						RETURN_THROWS();
					}
					hints.ai_flags = (int)val;
				} else if (zend_string_equals_literal(key, "ai_socktype")) {
					zend_long val = zval_try_get_long(hint, &failed);
					if (failed) {
						zend_argument_type_error(3, "\"ai_socktype\" key must be of type int, %s given", zend_zval_type_name(hint));
						RETURN_THROWS();
					}
					if (val < 0 || val > INT_MAX) {
						zend_argument_value_error(3, "\"ai_socktype\" key must be between 0 and %d", INT_MAX);
						RETURN_THROWS();
					}
					hints.ai_socktype = (int)val;
				} else if (zend_string_equals_literal(key, "ai_protocol")) {
					zend_long val = zval_try_get_long(hint, &failed);
					if (failed) {
						zend_argument_type_error(3, "\"ai_protocol\" key must be of type int, %s given", zend_zval_type_name(hint));
						RETURN_THROWS();
					}
					if (val < 0 || val > INT_MAX) {
						zend_argument_value_error(3, "\"ai_protocol\" key must be between 0 and %d", INT_MAX);
						RETURN_THROWS();
					}
					hints.ai_protocol = (int)val;
				} else if (zend_string_equals_literal(key, "ai_family")) {
					zend_long val = zval_try_get_long(hint, &failed);
					if (failed) {
						zend_argument_type_error(3, "\"ai_family\" key must be of type int, %s given", zend_zval_type_name(hint));
						RETURN_THROWS();
					}
					// Some platforms support also PF_LOCAL/AF_UNIX (e.g. FreeBSD) but the security concerns implied
					// make it not worth handling it (e.g. unwarranted write permissions on the socket).
					// Note existing socket_addrinfo* api already forbid such case.
					if (val != AF_UNSPEC) {
#ifdef HAVE_IPV6
						if (val != AF_INET && val != AF_INET6) {
							zend_argument_value_error(3, "\"ai_family\" key must be AF_INET or AF_INET6");
#else
						if (val != AF_INET) {
							zend_argument_value_error(3, "\"ai_family\" key must be AF_INET");
#endif
							RETURN_THROWS();
						}
					}
					hints.ai_family = (int)val;
				} else {
					zend_argument_value_error(3, "must only contain array keys \"ai_flags\", \"ai_socktype\", "
							"\"ai_protocol\", or \"ai_family\"");
					RETURN_THROWS();
					}
			} else {
				zend_argument_value_error(3, "must only contain array keys \"ai_flags\", \"ai_socktype\", "
						"\"ai_protocol\", or \"ai_family\"");
				RETURN_THROWS();
			}
		} ZEND_HASH_FOREACH_END();
	}

	if ((ret = getaddrinfo(ZSTR_VAL(hostname), service ? ZSTR_VAL(service) : NULL, &hints, &result)) != 0) {
		if (error_code) {
			ZEND_TRY_ASSIGN_REF_LONG(error_code, ret);
		}
		RETURN_FALSE;
	}

	array_init(return_value);
	zend_hash_real_init_packed(Z_ARRVAL_P(return_value));

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_family == AF_INET
#ifdef HAVE_IPV6
		 || rp->ai_family == AF_INET6
#endif
				) {
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
	php_addrinfo	*ai;
	php_socket		*php_sock;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, address_info_ce)
	ZEND_PARSE_PARAMETERS_END();

	ai = Z_ADDRESS_INFO_P(arg1);

	ZEND_ASSERT(ai->addrinfo.ai_family == AF_INET || ai->addrinfo.ai_family == AF_INET6);

	PHP_ETH_PROTO_CHECK(ai->addrinfo.ai_protocol, ai->addrinfo.ai_family);

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

	if (bind(php_sock->bsd_socket, ai->addrinfo.ai_addr, ai->addrinfo.ai_addrlen) != 0) {
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
	php_addrinfo	*ai;
	php_socket		*php_sock;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, address_info_ce)
	ZEND_PARSE_PARAMETERS_END();

	ai = Z_ADDRESS_INFO_P(arg1);

	ZEND_ASSERT(ai->addrinfo.ai_family == AF_INET || ai->addrinfo.ai_family == AF_INET6);

	PHP_ETH_PROTO_CHECK(ai->addrinfo.ai_protocol, ai->addrinfo.ai_family);

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

	if (connect(php_sock->bsd_socket, ai->addrinfo.ai_addr, ai->addrinfo.ai_addrlen) != 0) {
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(arg1, address_info_ce)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS(zsocket, socket_ce)
		Z_PARAM_LONG(target_pid)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(id, id_len)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(id, id_len)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(SUCCESS == zend_hash_str_del(&(SOCKETS_G(wsa_info)), id, id_len));
}
/* }}} */
#endif
