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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Wez Furlong                                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.24 1999-06-18 [ssb] */

#ifndef FSOCK_H
#define FSOCK_H

#ifdef NETWARE
#ifdef NEW_LIBC
#include "sys/timeval.h"
#else
#include "netware/time_nw.h"    /* For 'timeval' */
#endif
#endif

#include "file.h"

#define PHP_FSOCK_CHUNK_SIZE 8192

#include "php_network.h"

#if HAVE_PHP_STREAM
extern php_stream_ops php_stream_socket_ops;
#endif

/* stream->abstract points to an instance of this */
struct php_sockbuf {
	int socket;
	unsigned char *readbuf;
	size_t readbuflen;
	size_t readpos;
	size_t writepos;
	struct php_sockbuf *next;
	struct php_sockbuf *prev;
	char eof;
	char persistent;
	char is_blocked;
	size_t chunk_size;
	struct timeval timeout;
	char timeout_event;
#if HAVE_PHP_STREAM
	php_stream * stream;
#endif
};

typedef struct php_sockbuf php_sockbuf;

PHP_FUNCTION(fsockopen);
PHP_FUNCTION(pfsockopen);

PHPAPI int php_lookup_hostname(const char *addr, struct in_addr *in);
PHPAPI char *php_sock_fgets(char *buf, size_t maxlen, int socket);
PHPAPI size_t php_sock_fread(char *buf, size_t maxlen, int socket);
PHPAPI int php_sock_feof(int socket);
PHPAPI int php_sock_fgetc(int socket);
PHPAPI int php_is_persistent_sock(int);
PHPAPI int php_sockset_blocking(int socket, int mode);
PHPAPI void php_sockset_timeout(int socket, struct timeval *timeout);
PHPAPI int php_sockdestroy(int socket);
PHPAPI int php_sock_close(int socket);
PHPAPI size_t php_sock_set_def_chunk_size(size_t size);
PHPAPI void php_msock_destroy(int *data);
PHPAPI void php_cleanup_sockbuf(int persistent TSRMLS_DC);

PHPAPI struct php_sockbuf *php_get_socket(int socket);

PHP_RSHUTDOWN_FUNCTION(fsock);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim: sw=4 ts=4
 */
#endif /* FSOCK_H */
