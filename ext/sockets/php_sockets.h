/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/* $Id$ */

#if HAVE_SOCKETS

extern zend_module_entry sockets_module_entry;
#define phpext_sockets_ptr &sockets_module_entry

#ifdef PHP_WIN32
#include <winsock.h>
#else
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#endif

PHP_MINIT_FUNCTION(sockets);
PHP_MINFO_FUNCTION(sockets);
PHP_RSHUTDOWN_FUNCTION(sockets);

PHP_FUNCTION(socket_select);
PHP_FUNCTION(socket_create_listen);
#ifdef HAVE_SOCKETPAIR
PHP_FUNCTION(socket_create_pair);
#endif
PHP_FUNCTION(socket_accept);
PHP_FUNCTION(socket_set_nonblock);
PHP_FUNCTION(socket_set_block);
PHP_FUNCTION(socket_listen);
PHP_FUNCTION(socket_close);
PHP_FUNCTION(socket_write);
PHP_FUNCTION(socket_read);
PHP_FUNCTION(socket_getsockname);
PHP_FUNCTION(socket_getpeername);
PHP_FUNCTION(socket_create);
PHP_FUNCTION(socket_connect);
PHP_FUNCTION(socket_strerror);
PHP_FUNCTION(socket_bind);
PHP_FUNCTION(socket_recv);
PHP_FUNCTION(socket_send);
PHP_FUNCTION(socket_recvfrom);
PHP_FUNCTION(socket_sendto);
PHP_FUNCTION(socket_get_option);
PHP_FUNCTION(socket_set_option);
#ifdef HAVE_SHUTDOWN
PHP_FUNCTION(socket_shutdown);
#endif
PHP_FUNCTION(socket_last_error);
PHP_FUNCTION(socket_clear_error);

#ifndef PHP_WIN32
typedef int PHP_SOCKET;
# define PHP_SOCKETS_API PHPAPI
#else
# define PHP_SOCKETS_API __declspec(dllexport)
typedef SOCKET PHP_SOCKET;
#endif

typedef struct {
	PHP_SOCKET bsd_socket;
	int		type;
	int		error;
	int		blocking;
} php_socket;

#ifdef PHP_WIN32
struct	sockaddr_un {
	short	sun_family;
	char	sun_path[108];
};
#endif

PHP_SOCKETS_API int php_sockets_le_socket(void);

#define php_sockets_le_socket_name "Socket"

/* Prototypes */
#ifdef ilia_0 /* not needed, only causes a compiler warning */
static int php_open_listen_sock(php_socket **php_sock, int port, int backlog TSRMLS_DC);
static int php_accept_connect(php_socket *in_sock, php_socket **new_sock, struct sockaddr *la TSRMLS_DC);
static int php_read(php_socket *sock, void *buf, size_t maxlen, int flags);
static char *php_strerror(int error TSRMLS_DC);
#endif

ZEND_BEGIN_MODULE_GLOBALS(sockets)
	int last_error;
	char *strerror_buf;
ZEND_END_MODULE_GLOBALS(sockets)

#ifdef ZTS
#define SOCKETS_G(v) TSRMG(sockets_globals_id, zend_sockets_globals *, v)
#else
#define SOCKETS_G(v) (sockets_globals.v)
#endif

#else
#define phpext_sockets_ptr NULL
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

