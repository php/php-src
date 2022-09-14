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
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SOCKETS_H
#define PHP_SOCKETS_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#if HAVE_SOCKETS

#include <php.h>
#ifdef PHP_WIN32
# include "windows_common.h"
#else
# define IS_INVALID_SOCKET(a) (a->bsd_socket < 0)
#endif

#define PHP_SOCKETS_VERSION PHP_VERSION

extern zend_module_entry sockets_module_entry;
#define phpext_sockets_ptr &sockets_module_entry

#ifdef PHP_WIN32
#include <Winsock2.h>
#else
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#endif

#ifndef PHP_WIN32
typedef int PHP_SOCKET;
#else
typedef SOCKET PHP_SOCKET;
#endif

#ifdef PHP_WIN32
#	ifdef PHP_SOCKETS_EXPORTS
#		define PHP_SOCKETS_API __declspec(dllexport)
#	else
#		define PHP_SOCKETS_API __declspec(dllimport)
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SOCKETS_API __attribute__ ((visibility("default")))
#else
#	define PHP_SOCKETS_API
#endif

/* Socket class */

typedef struct {
	PHP_SOCKET	bsd_socket;
	int			type;
	int			error;
	int			blocking;
	zval		zstream;
	zend_object std;
} php_socket;

extern PHP_SOCKETS_API zend_class_entry *socket_ce;

static inline php_socket *socket_from_obj(zend_object *obj) {
	return (php_socket *)((char *)(obj) - XtOffsetOf(php_socket, std));
}

#define Z_SOCKET_P(zv) socket_from_obj(Z_OBJ_P(zv))

#define ENSURE_SOCKET_VALID(php_sock) do { \
	if (IS_INVALID_SOCKET(php_sock)) { \
		zend_argument_error(NULL, 1, "has already been closed"); \
		RETURN_THROWS(); \
	} \
} while (0)

#ifdef PHP_WIN32
struct	sockaddr_un {
	short	sun_family;
	char	sun_path[108];
};
#endif

#define PHP_SOCKET_ERROR(socket, msg, errn) \
		do { \
			int _err = (errn); /* save value to avoid repeated calls to WSAGetLastError() on Windows */ \
			(socket)->error = _err; \
			SOCKETS_G(last_error) = _err; \
			if (_err != EAGAIN && _err != EWOULDBLOCK && _err != EINPROGRESS) { \
				php_error_docref(NULL, E_WARNING, "%s [%d]: %s", msg, _err, sockets_strerror(_err)); \
			} \
		} while (0)

ZEND_BEGIN_MODULE_GLOBALS(sockets)
	int last_error;
	char *strerror_buf;
#ifdef PHP_WIN32
	uint32_t wsa_child_count;
	HashTable wsa_info;
#endif
ZEND_END_MODULE_GLOBALS(sockets)

PHP_SOCKETS_API ZEND_EXTERN_MODULE_GLOBALS(sockets)
#define SOCKETS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(sockets, v)

enum sockopt_return {
	SOCKOPT_ERROR,
	SOCKOPT_CONTINUE,
	SOCKOPT_SUCCESS
};

PHP_SOCKETS_API char *sockets_strerror(int error);
PHP_SOCKETS_API int socket_import_file_descriptor(PHP_SOCKET socket, php_socket *retsock);

#else
#define phpext_sockets_ptr NULL
#endif

#if defined(_AIX) && !defined(HAVE_SA_SS_FAMILY)
# define ss_family __ss_family
#endif

#ifndef MSG_EOF
#ifdef MSG_FIN
#define MSG_EOF MSG_FIN
#endif
#endif

#ifndef MSG_WAITALL
#ifdef LINUX
#define MSG_WAITALL 0x00000100
#else
#define MSG_WAITALL 0x00000000
#endif
#endif

#define PHP_NORMAL_READ 0x0001
#define PHP_BINARY_READ 0x0002

#ifdef WIN32
#define PHP_SOCKET_EINTR WSAEINTR
#elif defined(EINTR)
#define PHP_SOCKET_EINTR EINTR
#endif

#ifdef WIN32
#define PHP_SOCKET_EBADF WSAEBADF
#elif defined(EBADF)
#define PHP_SOCKET_EBADF EBADF
#endif

#ifdef WIN32
#define PHP_SOCKET_EACCES WSAEACCES
#elif defined(EACCES)
#define PHP_SOCKET_EACCES EACCES
#endif

#ifdef WIN32
#define PHP_SOCKET_EFAULT WSAEFAULT
#elif defined(EFAULT)
#define PHP_SOCKET_EFAULT EFAULT
#endif

#ifdef WIN32
#define PHP_SOCKET_EINVAL WSAEINVAL
#elif defined(EINVAL)
#define PHP_SOCKET_EINVAL EINVAL
#endif

#ifdef ENFILE
#define PHP_SOCKET_ENFILE ENFILE
#endif

#ifdef WIN32
#define PHP_SOCKET_EMFILE WSAEMFILE
#elif defined(EMFILE)
#define PHP_SOCKET_EMFILE EMFILE
#endif

#ifdef WIN32
#define PHP_SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#elif defined(EWOULDBLOCK)
#define PHP_SOCKET_EWOULDBLOCK EWOULDBLOCK
#endif

#ifdef WIN32
#define PHP_SOCKET_EINPROGRESS WSAEINPROGRESS
#elif defined(EINPROGRESS)
#define PHP_SOCKET_EINPROGRESS EINPROGRESS
#endif

#ifdef WIN32
#define PHP_SOCKET_EALREADY WSAEALREADY
#elif defined(EALREADY)
#define PHP_SOCKET_EALREADY EALREADY
#endif

#ifdef WIN32
#define PHP_SOCKET_ENOTSOCK WSAENOTSOCK
#elif defined(ENOTSOCK)
#define PHP_SOCKET_ENOTSOCK ENOTSOCK
#endif

#ifdef WIN32
#define PHP_SOCKET_EDESTADDRREQ WSAEDESTADDRREQ
#elif defined(EDESTADDRREQ)
#define PHP_SOCKET_EDESTADDRREQ EDESTADDRREQ
#endif

#ifdef WIN32
#define PHP_SOCKET_EMSGSIZE WSAEMSGSIZE
#elif defined(EMSGSIZE)
#define PHP_SOCKET_EMSGSIZE EMSGSIZE
#endif

#ifdef WIN32
#define PHP_SOCKET_EPROTOTYPE WSAEPROTOTYPE
#elif defined(EPROTOTYPE)
#define PHP_SOCKET_EPROTOTYPE EPROTOTYPE
#endif

#ifdef WIN32
#define PHP_SOCKET_ENOPROTOOPT WSAENOPROTOOPT
#elif defined(ENOPROTOOPT)
#define PHP_SOCKET_ENOPROTOOPT ENOPROTOOPT
#endif

#ifdef WIN32
#define PHP_SOCKET_EPROTONOSUPPORT WSAEPROTONOSUPPORT
#elif defined(EPROTONOSUPPORT)
#define PHP_SOCKET_EPROTONOSUPPORT EPROTONOSUPPORT
#endif

#ifdef WIN32
#define PHP_SOCKET_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#elif defined(ESOCKTNOSUPPORT)
#define PHP_SOCKET_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
#endif

#ifdef WIN32
#define PHP_SOCKET_EOPNOTSUPP WSAEOPNOTSUPP
#elif defined(EOPNOTSUPP)
#define PHP_SOCKET_EOPNOTSUPP EOPNOTSUPP
#endif

#ifdef WIN32
#define PHP_SOCKET_EPFNOSUPPORT WSAEPFNOSUPPORT
#elif defined(EPFNOSUPPORT)
#define PHP_SOCKET_EPFNOSUPPORT EPFNOSUPPORT
#endif

#ifdef WIN32
#define PHP_SOCKET_EAFNOSUPPORT WSAEAFNOSUPPORT
#elif defined(EAFNOSUPPORT)
#define PHP_SOCKET_EAFNOSUPPORT EAFNOSUPPORT
#endif

#ifdef WIN32
#define PHP_SOCKET_EADDRINUSE WSAEADDRINUSE
#elif defined(EADDRINUSE)
#define PHP_SOCKET_EADDRINUSE EADDRINUSE
#endif

#ifdef WIN32
#define PHP_SOCKET_EADDRNOTAVAIL WSAEADDRNOTAVAIL
#elif defined(EADDRNOTAVAIL)
#define PHP_SOCKET_EADDRNOTAVAIL EADDRNOTAVAIL
#endif

#ifdef WIN32
#define PHP_SOCKET_ENETDOWN WSAENETDOWN
#elif defined(ENETDOWN)
#define PHP_SOCKET_ENETDOWN ENETDOWN
#endif

#ifdef WIN32
#define PHP_SOCKET_ENETUNREACH WSAENETUNREACH
#elif defined(ENETUNREACH)
#define PHP_SOCKET_ENETUNREACH ENETUNREACH
#endif

#ifdef WIN32
#define PHP_SOCKET_ENETRESET WSAENETRESET
#elif defined(ENETRESET)
#define PHP_SOCKET_ENETRESET ENETRESET
#endif

#ifdef WIN32
#define PHP_SOCKET_ECONNABORTED WSAECONNABORTED
#elif defined(ECONNABORTED)
#define PHP_SOCKET_ECONNABORTED ECONNABORTED
#endif

#ifdef WIN32
#define PHP_SOCKET_ECONNRESET WSAECONNRESET
#elif defined(ECONNRESET)
#define PHP_SOCKET_ECONNRESET ECONNRESET
#endif

#ifdef WIN32
#define PHP_SOCKET_ENOBUFS WSAENOBUFS
#elif defined(ENOBUFS)
#define PHP_SOCKET_ENOBUFS ENOBUFS
#endif

#ifdef WIN32
#define PHP_SOCKET_EISCONN WSAEISCONN
#elif defined(EISCONN)
#define PHP_SOCKET_EISCONN EISCONN
#endif

#ifdef WIN32
#define PHP_SOCKET_ENOTCONN WSAENOTCONN
#elif defined(ENOTCONN)
#define PHP_SOCKET_ENOTCONN ENOTCONN
#endif

#ifdef WIN32
#define PHP_SOCKET_ESHUTDOWN WSAESHUTDOWN
#elif defined(ESHUTDOWN)
#define PHP_SOCKET_ESHUTDOWN ESHUTDOWN
#endif

#ifdef WIN32
#define PHP_SOCKET_ETOOMANYREFS WSAETOOMANYREFS
#elif defined(ETOOMANYREFS)
#define PHP_SOCKET_ETOOMANYREFS ETOOMANYREFS
#endif

#ifdef WIN32
#define PHP_SOCKET_ETIMEDOUT WSAETIMEDOUT
#elif defined(ETIMEDOUT)
#define PHP_SOCKET_ETIMEDOUT ETIMEDOUT
#endif

#ifdef WIN32
#define PHP_SOCKET_ECONNREFUSED WSAECONNREFUSED
#elif defined(ECONNREFUSED)
#define PHP_SOCKET_ECONNREFUSED ECONNREFUSED
#endif

#ifdef WIN32
#define PHP_SOCKET_ELOOP WSAELOOP
#elif defined(ELOOP)
#define PHP_SOCKET_ELOOP ELOOP
#endif

#ifdef WIN32
#define PHP_SOCKET_ENAMETOOLONG WSAENAMETOOLONG
#elif defined(ENAMETOOLONG)
#define PHP_SOCKET_ENAMETOOLONG ENAMETOOLONG
#endif

#ifdef WIN32
#define PHP_SOCKET_EHOSTDOWN WSAEHOSTDOWN
#elif defined(EHOSTDOWN)
#define PHP_SOCKET_EHOSTDOWN EHOSTDOWN
#endif

#ifdef WIN32
#define PHP_SOCKET_EHOSTUNREACH WSAEHOSTUNREACH
#elif defined(EHOSTUNREACH)
#define PHP_SOCKET_EHOSTUNREACH EHOSTUNREACH
#endif

#ifdef WIN32
#define PHP_SOCKET_ENOTEMPTY WSAENOTEMPTY
#elif defined(ENOTEMPTY)
#define PHP_SOCKET_ENOTEMPTY ENOTEMPTY
#endif

#ifdef WIN32
#define PHP_SOCKET_EUSERS WSAEUSERS
#elif defined(EUSERS)
#define PHP_SOCKET_EUSERS EUSERS
#endif

#ifdef WIN32
#define PHP_SOCKET_EDQUOT WSAEDQUOT
#elif defined(EDQUOT)
#define PHP_SOCKET_EDQUOT EDQUOT
#endif

#ifdef WIN32
#define PHP_SOCKET_EREMOTE WSAEREMOTE
#elif defined(EREMOTE)
#define PHP_SOCKET_EREMOTE EREMOTE
#endif

#endif
