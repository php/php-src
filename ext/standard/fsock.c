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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.121 1999-06-18 [ssb] */
/* Synced with php 3.0 revision 1.133 1999-07-21 [sas] */

#include "php.h"
#include "php_globals.h"
#include <stdlib.h>
#include <stddef.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef PHP_WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifdef PHP_WIN32
#undef AF_UNIX
#endif
#if defined(AF_UNIX)
#include <sys/un.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <string.h>
#include <errno.h>

#include "base64.h"
#include "file.h"
#include "url.h"
#include "fsock.h"

#ifdef ZTS
static int fsock_globals_id;
#else
static php_fsock_globals fsock_globals;
extern int le_fp;
#endif

#define CLOSE_SOCK(free_sock)	\
	if(socketd >= 0) {			\
		close(socketd);			\
	}							\
	if (free_sock) {			\
		efree(sock);			\
	}							\
	if (key) {					\
		efree(key);				\
	}

#define SEARCHCR() \
	p = memchr(READPTR(sock), '\n', MIN(TOREAD(sock), maxlen));

#ifdef PHP_WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include "build-defs.h"
#endif

/* {{{ lookup_hostname */

/*
 * Converts a host name to an IP address.
 */
int lookup_hostname(const char *addr, struct in_addr *in)
{
	struct hostent *host_info;

	if (!inet_aton(addr, in)) {
		/* XXX NOT THREAD SAFE */
		host_info = gethostbyname(addr);
		if (host_info == 0) {
			/* Error: unknown host */
			return -1;
		}
		*in = *((struct in_addr *) host_info->h_addr);
	}
	return 0;
}
/* }}} */
/* {{{ php_is_persistent_sock */

int php_is_persistent_sock(int sock)
{
	char *key;
	FLS_FETCH();

	if (zend_hash_find(&FG(ht_fsock_socks), (char *) &sock, sizeof(sock),
				(void **) &key) == SUCCESS) {
		return 1;
	}
	return 0;
}
/* }}} */
/* {{{ connect_nonb */
PHPAPI int connect_nonb(int sockfd,
						struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
/* probably won't work on Win32, someone else might try it (read: fix it ;) */
#if !defined(PHP_WIN32) && (defined(O_NONBLOCK) || defined(O_NDELAY))

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

	int flags;
	int n;
	int error = 0;
	socklen_t len;
	int ret = 0;
	fd_set rset;
	fd_set wset;

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	if ((n = connect(sockfd, addr, addrlen)) < 0) {
		if (errno != EINPROGRESS) {
			return -1;
		}
	}

	if (n == 0) {
		goto ok;
	}

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	wset = rset;

	if ((n = select(sockfd + 1, &rset, &wset, NULL, timeout)) == 0) {
		error = ETIMEDOUT;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		/*
		  BSD-derived systems set errno correctly
		  Solaris returns -1 from getsockopt in case of error
		*/
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			ret = -1;
		}
	} else {
		/* whoops: sockfd has disappeared */
		ret = -1;
	}

 ok:
	fcntl(sockfd, F_SETFL, flags);

	if(error) {
		errno = error;
		ret = -1;
	}
	return ret;
#else /* !defined(PHP_WIN32) && ... */
      return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */
/* {{{ php_fsockopen() */

/* 
   This function takes an optional third argument which should be
   passed by reference.  The error code from the connect call is written
   to this variable.
*/
static void php_fsockopen(INTERNAL_FUNCTION_PARAMETERS, int persistent) {
	pval **args[5];
	int *sock=emalloc(sizeof(int));
	int *sockp;
	int arg_count=ZEND_NUM_ARGS();
	int socketd = -1;
	unsigned char udp = 0;
	struct timeval timeout = { 60, 0 };
	unsigned short portno;
	unsigned long conv;
	char *key = NULL;
	FLS_FETCH();
	
	if (arg_count > 5 || arg_count < 2 || zend_get_parameters_array_ex(arg_count,args)==FAILURE) {
		CLOSE_SOCK(1);
		WRONG_PARAM_COUNT;
	}
	switch(arg_count) {
		case 5:
			convert_to_double_ex(args[4]);
			conv = (unsigned long) ((*args[4])->value.dval * 1000000.0);
			timeout.tv_sec = conv / 1000000;
			timeout.tv_usec = conv % 1000000;
			/* fall-through */
		case 4:
			zval_dtor(*args[3]);
			ZVAL_STRING(*args[3],"",1);
			/* fall-through */
		case 3:
			zval_dtor(*args[2]);
			ZVAL_LONG(*args[2],0);
			break;
	}
	convert_to_string_ex(args[0]);
	convert_to_long_ex(args[1]);
	portno = (unsigned short) (*args[1])->value.lval;

	key = emalloc((*args[0])->value.str.len + 10);
	sprintf(key, "%s:%d", (*args[0])->value.str.val, portno);

	if (persistent && zend_hash_find(&FG(ht_fsock_keys), key, strlen(key) + 1,
				(void *) &sockp) == SUCCESS) {
		CLOSE_SOCK(0);
		*sock = *sockp;
		ZEND_REGISTER_RESOURCE(return_value,sock,php_file_le_socket());
		return;
	}
	
	if (portno) {
		struct sockaddr_in server;

		memset(&server, 0, sizeof(server));
		if((*args[0])->value.str.val[0] == 'u' &&
			(*args[0])->value.str.val[1] == 'd' &&
			(*args[0])->value.str.val[2] == 'p' &&
			(*args[0])->value.str.val[3] == ':' &&
			(*args[0])->value.str.val[4] == '/' &&
			(*args[0])->value.str.val[5] == '/') {
			udp = 1;
		}
		
		socketd = socket(AF_INET,udp ? SOCK_DGRAM : SOCK_STREAM,0);

		if (socketd == SOCK_ERR) {
			CLOSE_SOCK(1);
			RETURN_FALSE;
		}
	  
		server.sin_family = AF_INET;
		
		if(lookup_hostname(udp ? &(*args[0])->value.str.val[6] : (*args[0])->value.str.val,&server.sin_addr)) {
			CLOSE_SOCK(1);
			RETURN_FALSE;
		}
  
		server.sin_port = htons(portno);
  
		if (connect_nonb(socketd, (struct sockaddr *)&server, sizeof(server), &timeout) == SOCK_CONN_ERR) {
			CLOSE_SOCK(1);

			if (arg_count>2) {
				zval_dtor(*args[2]);
				ZVAL_LONG(*args[2],errno);
			}
			if (arg_count>3) {
				zval_dtor(*args[3]);
				ZVAL_STRING(*args[3],strerror(errno),1);
			}
			RETURN_FALSE;
		}
#if defined(AF_UNIX)
	} else {
		/* Unix domain socket.  s->strval is socket name. */
		struct  sockaddr_un unix_addr;
		socketd = socket(AF_UNIX,SOCK_STREAM,0);
		if (socketd == SOCK_ERR) {
			CLOSE_SOCK(1);
			RETURN_FALSE;
		}
	  
		memset(&unix_addr, (char)0, sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strlcpy(unix_addr.sun_path, (*args[0])->value.str.val, sizeof(unix_addr.sun_path));

		if (connect_nonb(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr), &timeout) == SOCK_CONN_ERR) {
			CLOSE_SOCK(1);
			if(arg_count>2) (*args[2])->value.lval = errno;
			if(arg_count>3) {
				(*args[3])->value.str.val = estrdup(strerror(errno));
				(*args[3])->value.str.len = strlen((*args[3])->value.str.val);
			}
			RETURN_FALSE;
		}
#endif /* AF_UNIX */
	}

#if 0
	if ((fp = fdopen (socketd, "r+")) == NULL){
		RETURN_LONG(-6);  /* FIXME */
	}

#ifdef HAVE_SETVBUF  
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0){
		RETURN_LONG(-7);  /* FIXME */
	}
#endif
#endif

	*sock=socketd;
	if (persistent) {
		zend_hash_update(&FG(ht_fsock_keys), key, strlen(key) + 1, 
				sock, sizeof(*sock), NULL);
		zend_hash_update(&FG(ht_fsock_socks), (char *) sock, sizeof(*sock),
				key, strlen(key) + 1, NULL);
	}
	if(key) efree(key);

	ZEND_REGISTER_RESOURCE(return_value,sock,php_file_le_socket());
}
/* }}} */

/* {{{ proto int fsockopen(string hostname, int port [, int errno [, string errstr [, double timeout]]])
   Open Internet or Unix domain socket connection */
PHP_FUNCTION(fsockopen) 
{
	php_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
/* {{{ proto int pfsockopen(string hostname, int port [, int errno [, string errstr [, double timeout]]])
   Open persistent Internet or Unix domain socket connection */
PHP_FUNCTION(pfsockopen) 
{
	php_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

#define CHUNK_SIZE 8192
#define SOCK_DESTROY(sock) \
		if(sock->readbuf) pefree(sock->readbuf, sock->persistent); \
		if(sock->prev) sock->prev->next = sock->next; \
		if(sock->next) sock->next->prev = sock->prev; \
		if(sock == FG(phpsockbuf)) \
			FG(phpsockbuf) = sock->next; \
		pefree(sock, sock->persistent)

static void php_cleanup_sockbuf(int persistent FLS_DC)
{
	php_sockbuf *now, *next;

	for(now = FG(phpsockbuf); now; now = next) {
		next = now->next;
		if(now->persistent == persistent) {
			SOCK_DESTROY(now);
		}
	}
}

#define TOREAD(sock) ((sock)->writepos - (sock)->readpos)
#define READPTR(sock) ((sock)->readbuf + (sock)->readpos)
#define WRITEPTR(sock) ((sock)->readbuf + (sock)->writepos)
#define SOCK_FIND(sock,socket) \
      php_sockbuf *sock; \
      FLS_FETCH(); \
      sock = php_sockfind(socket FLS_CC); \
      if(!sock) sock = php_sockcreate(socket FLS_CC)

static php_sockbuf *php_sockfind(int socket FLS_DC)
{
	php_sockbuf *buf = NULL, *tmp;

	for(tmp = FG(phpsockbuf); tmp; tmp = tmp->next) 
		if(tmp->socket == socket) {
			buf = tmp;
			break;
		}

	return buf;
}

static php_sockbuf *php_sockcreate(int socket FLS_DC)
{
	php_sockbuf *sock;
	int persistent = php_is_persistent_sock(socket);

	sock = pecalloc(sizeof(*sock), 1, persistent);
	sock->socket = socket;
	if((sock->next = FG(phpsockbuf))) 
		FG(phpsockbuf)->prev = sock;
	sock->persistent = persistent;
	sock->is_blocked = 1;
	sock->chunk_size = FG(def_chunk_size);
	sock->timeout.tv_sec = -1;
	FG(phpsockbuf) = sock;

	return sock;
}

PHPAPI php_sockbuf *php_get_socket(int socket)
{
	SOCK_FIND(sock, socket);
	return sock;
}

size_t php_sock_set_def_chunk_size(size_t size)
{
	size_t old;
	FLS_FETCH();

	old = FG(def_chunk_size);

	if(size <= CHUNK_SIZE || size > 0) 
		FG(def_chunk_size) = size;

	return old;
}

int php_sockdestroy(int socket)
{
	int ret = 0;
	php_sockbuf *sock;
	FLS_FETCH();

	sock = php_sockfind(socket FLS_CC);
	if(sock) {
		ret = 1;
		SOCK_DESTROY(sock);
	}

	return ret;
}

#if !defined(PHP_WIN32)
#undef closesocket
#define closesocket close
#endif

#ifndef HAVE_SHUTDOWN
#undef shutdown
#define shutdown
#endif

#define SOCK_CLOSE(s) shutdown(s, 0); closesocket(s)

int php_sock_close(int socket)
{
	int ret = 0;
	php_sockbuf *sock;
	FLS_FETCH();

	sock = php_sockfind(socket FLS_CC);
	if(sock) {
		if(!sock->persistent) {
			SOCK_CLOSE(sock->socket);
			SOCK_DESTROY(sock);
		}
	} else {
		SOCK_CLOSE(socket);
	}

	return ret;
}

#define MAX_CHUNKS_PER_READ 10

static void php_sockwait_for_data(php_sockbuf *sock)
{
	fd_set fdr, tfdr;
	int retval;
	struct timeval timeout, *ptimeout;

	FD_ZERO(&fdr);
	FD_SET(sock->socket, &fdr);
	sock->timeout_event = 0;

	if (sock->timeout.tv_sec == -1) 
		ptimeout = NULL;
	else
		ptimeout = &timeout;

	while(1) {
		tfdr = fdr;
		timeout = sock->timeout;

		retval = select(sock->socket + 1, &tfdr, NULL, NULL, ptimeout);
		
		if (retval == 0)
			sock->timeout_event = 1;

		if (retval >= 0)
			break;
	}
}

static size_t php_sockread_internal(php_sockbuf *sock)
{
	char buf[CHUNK_SIZE];
	int nr_bytes;
	size_t nr_read = 0;
	
	/* For blocking sockets, we wait until there is some
	   data to read (real data or EOF)
	   
	   Otherwise, recv() may time out and return 0 and
	   therefore sock->eof would be set errornously.
	 */

	
	if(sock->is_blocked) {
		php_sockwait_for_data(sock);
		if (sock->timeout_event)
			return 0;
	}

	/* read at a maximum sock->chunk_size */
	nr_bytes = recv(sock->socket, buf, sock->chunk_size, 0);
	if(nr_bytes > 0) {
		if(sock->writepos + nr_bytes > sock->readbuflen) {
			sock->readbuflen += sock->chunk_size;
			sock->readbuf = perealloc(sock->readbuf, sock->readbuflen,
					sock->persistent);
		}
		memcpy(WRITEPTR(sock), buf, nr_bytes);
		sock->writepos += nr_bytes;
		nr_read = nr_bytes;
	} else if(nr_bytes == 0 || (nr_bytes < 0 && errno != EWOULDBLOCK)) {
		sock->eof = 1;
	}

	return nr_read;
}

static void php_sockread_total(php_sockbuf *sock, size_t maxread)
{
	while(!sock->eof && TOREAD(sock) < maxread && !sock->timeout_event) {
		php_sockread_internal(sock);
	}
}

static size_t php_sockread(php_sockbuf *sock)
{
	size_t nr_bytes;
	size_t nr_read = 0;
	int i;
	
	for(i = 0; !sock->eof && i < MAX_CHUNKS_PER_READ; i++) {
		nr_bytes = php_sockread_internal(sock);
		if(nr_bytes == 0) break;
		nr_read += nr_bytes;
	}

	return nr_read;
}

int php_sockset_blocking(int socket, int mode)
{
	int old;
	SOCK_FIND(sock, socket);

	old = sock->is_blocked;
	
	sock->is_blocked = mode;
	
	return old;
}

void php_sockset_timeout(int socket, struct timeval *timeout)
{
	SOCK_FIND(sock, socket);

	sock->timeout = *timeout;
}

#define SOCK_FIND_AND_READ_MAX(max) \
	SOCK_FIND(sock, socket); \
	if(sock->is_blocked) php_sockread_total(sock, max); else php_sockread(sock)

/* {{{ php_sock_fgets() */
/*
 * FIXME: fgets depends on '\n' as line delimiter
 */
char *php_sock_fgets(char *buf, size_t maxlen, int socket)
{
	char *p = NULL;
	char *ret = NULL;
	size_t amount = 0;
	SOCK_FIND(sock, socket);

	if (maxlen==0) {
		buf[0] = 0;
		return buf;
	}

	SEARCHCR();

	if(!p) {
		if(sock->is_blocked) {
			while(!p && !sock->eof && !sock->timeout_event && TOREAD(sock) < maxlen) {
				php_sockread_internal(sock);
				SEARCHCR();
			}
		} else {
			php_sockread(sock);
			SEARCHCR();
		}
	}

	
	if(p) {
		amount = (ptrdiff_t) p - (ptrdiff_t) READPTR(sock) + 1;
	} else {
		amount = TOREAD(sock);
	}
	
	amount = MIN(amount, maxlen);

	if(amount > 0) {
		memcpy(buf, READPTR(sock), amount);
		sock->readpos += amount;
	}
	buf[amount] = '\0';
	
	/* signal error only, if we don't return data from this call and 
	   if there is no data to read and if the eof flag is set */
	if(amount || TOREAD(sock) || !sock->eof)
		ret = buf;

	return ret;
}

/* }}} */

/*
 * FIXME: fgetc returns EOF, if no data is available on a nonblocking socket.
 * I don't have any documentation on the semantics of fgetc in this case.
 *
 * ss@2ns.de 19990528
 */

int php_sock_fgetc(int socket)
{
	int ret = EOF;
	SOCK_FIND_AND_READ_MAX(1);

	if(TOREAD(sock) > 0) {
		ret = *READPTR(sock);
		sock->readpos++;
	}

	return ret;
}

int php_sock_feof(int socket)
{
	int ret = 0;
	SOCK_FIND(sock, socket);

	if(!sock->is_blocked)
		php_sockread(sock);
	
	if(!TOREAD(sock) && sock->eof) 
		ret = 1;

	return ret;
}

/* {{{ php_sock_fread() */

size_t php_sock_fread(char *ptr, size_t size, int socket)
{
	size_t ret = 0;
	SOCK_FIND_AND_READ_MAX(size);

	if(size < 0) 
		return ret;

	ret = MIN(TOREAD(sock), size);
	if(ret) {
		memcpy(ptr, READPTR(sock), ret);
		sock->readpos += ret;
	}

	return ret;
}

/* }}} */
/* {{{ module start/shutdown functions */

	/* {{{ php_msock_destroy */
void php_msock_destroy(int *data)
{
	close(*data);
}
/* }}} */

static void fsock_globals_ctor(FLS_D)
{
	zend_hash_init(&FG(ht_fsock_keys), 0, NULL, NULL, 1);
	zend_hash_init(&FG(ht_fsock_socks), 0, NULL, (void (*)(void *))php_msock_destroy, 1);
	FG(def_chunk_size) = CHUNK_SIZE;
	FG(phpsockbuf) = NULL;
}

static void fsock_globals_dtor(FLS_D)
{
	zend_hash_destroy(&FG(ht_fsock_socks));
	zend_hash_destroy(&FG(ht_fsock_keys));
	php_cleanup_sockbuf(1 FLS_CC);
}

PHP_MINIT_FUNCTION(fsock)
{
#ifdef ZTS
	fsock_globals_id = ts_allocate_id(sizeof(php_fsock_globals), (ts_allocate_ctor) fsock_globals_ctor, (ts_allocate_dtor) fsock_globals_dtor);
#else
	fsock_globals_ctor(FLS_C);
#endif
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fsock)
{
#ifndef ZTS
	fsock_globals_dtor(FLS_C);
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(fsock)
{
	FLS_FETCH();

	php_cleanup_sockbuf(0 FLS_CC);
	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
