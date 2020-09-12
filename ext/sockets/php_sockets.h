/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#endif
