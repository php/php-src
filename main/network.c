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
   | Author: Stig Venaas <venaas@uninett.no>                              |
   | Streams work by Wez Furlong <wez@thebrainroom.com>                   |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/*#define DEBUG_MAIN_NETWORK 1*/
#define MAX_CHUNKS_PER_READ 10
#define SOCKET_DEFAULT_TIMEOUT 60

#include "php.h"

#include <stddef.h>

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef _FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif

#ifndef PHP_WIN32
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#ifndef HAVE_INET_ATON
int inet_aton(const char *, struct in_addr *);
#endif

#include "php_network.h"

#if defined(PHP_WIN32) || defined(__riscos__)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "ext/standard/file.h"

#ifdef PHP_WIN32
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
#endif

#ifdef HAVE_GETADDRINFO
#ifdef HAVE_GAI_STRERROR
#  define PHP_GAI_STRERROR(x) (gai_strerror(x))
#else
#  define PHP_GAI_STRERROR(x) (php_gai_strerror(x))
/* {{{ php_gai_strerror
 */
static char *php_gai_strerror(int code)
{
        static struct {
                int code;
                const char *msg;
        } values[] = {
#  ifdef EAI_ADDRFAMILY
                {EAI_ADDRFAMILY, "Address family for hostname not supported"},
#  endif
                {EAI_AGAIN, "Temporary failure in name resolution"},
                {EAI_BADFLAGS, "Bad value for ai_flags"},
                {EAI_FAIL, "Non-recoverable failure in name resolution"},
                {EAI_FAMILY, "ai_family not supported"},
                {EAI_MEMORY, "Memory allocation failure"},
#  ifdef EAI_NODATA
                {EAI_NODATA, "No address associated with hostname"},
#  endif    
                {EAI_NONAME, "Name or service not known"},
                {EAI_SERVICE, "Servname not supported for ai_socktype"},
                {EAI_SOCKTYPE, "ai_socktype not supported"},
                {EAI_SYSTEM, "System error"},
                {0, NULL}
        };
        int i;

        for (i = 0; values[i].msg != NULL; i++) {
                if (values[i].code == code) {
                        return (char *)values[i].msg;
                }
        }
        
        return "Unknown error";
}
/* }}} */
#endif
#endif

/* {{{ php_network_freeaddresses
 */
static void php_network_freeaddresses(struct sockaddr **sal)
{
	struct sockaddr **sap;

	if (sal == NULL)
		return;
	for (sap = sal; *sap != NULL; sap++)
		efree(*sap);
	efree(sal);
}
/* }}} */

/* {{{ php_network_getaddresses
 * Returns number of addresses, 0 for none/error
 */
static int php_network_getaddresses(const char *host, struct sockaddr ***sal)
{
	struct sockaddr **sap;
	int n;

	if (host == NULL) {
		return 0;
	}

	{
#ifdef HAVE_GETADDRINFO
		struct addrinfo hints, *res, *sai;

		memset(&hints, '\0', sizeof(hints));
#  ifdef HAVE_IPV6
		hints.ai_family = AF_UNSPEC;
#  else
		hints.ai_family = AF_INET;
#  endif
		if ((n = getaddrinfo(host, NULL, &hints, &res))) {
			php_error(E_WARNING, "php_network_getaddresses: getaddrinfo failed: %s", PHP_GAI_STRERROR(n));
			return 0;
		} else if (res == NULL) {
			php_error(E_WARNING, "php_network_getaddresses: getaddrinfo failed (null result pointer)");
			return 0;
		}

		sai = res;
		for (n = 1; (sai = sai->ai_next) != NULL; n++);
		*sal = emalloc((n + 1) * sizeof(*sal));
		sai = res;
		sap = *sal;
		do {
			switch (sai->ai_family) {
#  ifdef HAVE_IPV6
			case AF_INET6:
				*sap = emalloc(sizeof(struct sockaddr_in6));
				*(struct sockaddr_in6 *)*sap =
					*((struct sockaddr_in6 *)sai->ai_addr);
				sap++;
				break;
#  endif
			case AF_INET:
				*sap = emalloc(sizeof(struct sockaddr_in));
				*(struct sockaddr_in *)*sap =
					*((struct sockaddr_in *)sai->ai_addr);
				sap++;
				break;
			}
		} while ((sai = sai->ai_next) != NULL);
		freeaddrinfo(res);
#else
		struct hostent *host_info;
		struct in_addr in;

		if (!inet_aton(host, &in)) {
			/* XXX NOT THREAD SAFE */
			host_info = gethostbyname(host);
			if (host_info == NULL) {
				php_error(E_WARNING, "php_network_getaddresses: gethostbyname failed");
				return 0;
			}
			in = *((struct in_addr *) host_info->h_addr);
		}

		*sal = emalloc(2 * sizeof(*sal));
		sap = *sal;
		*sap = emalloc(sizeof(struct sockaddr_in));
		(*sap)->sa_family = AF_INET;
		((struct sockaddr_in *)*sap)->sin_addr = in;
		sap++;
		n = 1;
#endif
	}
	*sap = NULL;
	return n;
}
/* }}} */

/* {{{ php_connect_nonb */
PHPAPI int php_connect_nonb(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
	/* probably won't work on Win32, someone else might try it (read: fix it ;) */

#if (!defined(__BEOS__) && !defined(PHP_WIN32)) && (defined(O_NONBLOCK) || defined(O_NDELAY))

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

	if (timeout == NULL)	{
		/* blocking mode */
		return connect(sockfd, addr, addrlen);
	}
	
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
#ifdef PHP_WIN32
	return php_connect_nonb_win32((SOCKET) sockfd, addr, addrlen, timeout);
#endif
	return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */

#ifdef PHP_WIN32
/* {{{ php_connect_nonb_win32 */
PHPAPI int php_connect_nonb_win32(SOCKET sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
	int error = 0, error_len, ret;
	u_long non_block = TRUE, block = FALSE;

	fd_set rset, wset;

	if (timeout == NULL)	{
		/* blocking mode */
		return connect(sockfd, addr, addrlen);
	}
	
	/* Set the socket to be non-blocking */
	ioctlsocket(sockfd, FIONBIO, &non_block);

	if (connect(sockfd, addr, addrlen) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			return SOCKET_ERROR;
		}
	}

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	FD_ZERO(&wset);
	FD_SET(sockfd, &wset);

	if ((ret = select(sockfd + 1, &rset, &wset, NULL, timeout)) == 0) {
		WSASetLastError(WSAETIMEDOUT);
		return SOCKET_ERROR;
	}

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		error_len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *) &error, &error_len) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
	} else {
		/* whoops: sockfd has disappeared */
		return SOCKET_ERROR;
	}

	/* Set the socket back to blocking */
	ioctlsocket(sockfd, FIONBIO, &block);

	if (error) { 
		WSASetLastError(error);
		return SOCKET_ERROR;
	}

	return 0;
}
/* }}} */
#endif

/* {{{ php_hostconnect
 * Creates a socket of type socktype and connects to the given host and
 * port, returns the created socket on success, else returns -1.
 * timeout gives timeout in seconds, 0 means blocking mode.
 */
int php_hostconnect(const char *host, unsigned short port, int socktype, struct timeval *timeout)
{	
	int n, repeatto, s;
	struct sockaddr **sal, **psal;
	struct timeval individual_timeout;
	int set_timeout = 0;
#ifdef PHP_WIN32
	int err;
#endif
	
	n = php_network_getaddresses(host, &sal);

	if (n == 0)
		return -1;
	
	if (timeout != NULL) {
		/* is this a good idea? 5s? */
		repeatto = timeout->tv_sec / n > 5;
		if (repeatto) {
			individual_timeout.tv_sec = timeout->tv_sec / n;
		} else {
			individual_timeout.tv_sec = timeout->tv_sec;
		}

		individual_timeout.tv_usec = timeout->tv_usec;
	} else {
		individual_timeout.tv_sec = 0;
		individual_timeout.tv_usec = 0;
	}
	
	/* Boolean indicating whether to pass a timeout */
	set_timeout = individual_timeout.tv_sec + individual_timeout.tv_usec;
	
	psal = sal;
	while (*sal != NULL) {
		s = socket((*sal)->sa_family, socktype, 0);
		if (s != SOCK_ERR) {
			switch ((*sal)->sa_family) {
#if defined( HAVE_GETADDRINFO ) && defined( HAVE_IPV6 )
				case AF_INET6:
					{
						struct sockaddr_in6 *sa =
							(struct sockaddr_in6 *)*sal;

						sa->sin6_family = (*sal)->sa_family;
						sa->sin6_port = htons(port);
						if (php_connect_nonb(s, (struct sockaddr *) sa,
									sizeof(*sa), (set_timeout) ? &individual_timeout : NULL) != SOCK_CONN_ERR)
							goto ok;
					} 
					break;
#endif
				case AF_INET:
					{
						struct sockaddr_in *sa =
							(struct sockaddr_in *)*sal;

						sa->sin_family = (*sal)->sa_family;
						sa->sin_port = htons(port);
						if (php_connect_nonb(s, (struct sockaddr *) sa,
									sizeof(*sa), (set_timeout) ? &individual_timeout : NULL) != SOCK_CONN_ERR)
							goto ok;

					} 
					break;
			}
#ifdef PHP_WIN32
			/* Preserve the last error */
			err = WSAGetLastError();
#endif
			close (s);
		}
		sal++;
	}
	php_network_freeaddresses(psal);
	php_error(E_WARNING, "php_hostconnect: connect failed");

#ifdef PHP_WIN32
	/* Restore the last error */
	WSASetLastError(err);
#endif 

	return -1;

 ok:
	php_network_freeaddresses(psal);
	return s;
}
/* }}} */

/* {{{ php_any_addr
 * Fills the any (wildcard) address into php_sockaddr_storage
 */
void php_any_addr(int family, php_sockaddr_storage *addr, unsigned short port)
{
	memset(addr, 0, sizeof(php_sockaddr_storage));
	switch (family) {
#ifdef HAVE_IPV6
	case AF_INET6: {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) addr;
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = htons(port);
		sin6->sin6_addr = in6addr_any;
		break;
	}
#endif
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *) addr;
		sin->sin_family = AF_INET;
		sin->sin_port = htons(port);
		sin->sin_addr.s_addr = INADDR_ANY;
		break;
	}
	}
}
/* }}} */

/* {{{ php_sockaddr_size
 * Returns the size of struct sockaddr_xx for the family
 */
int php_sockaddr_size(php_sockaddr_storage *addr)
{
	switch (((struct sockaddr *)addr)->sa_family) {
	case AF_INET:
		return sizeof(struct sockaddr_in);
#ifdef HAVE_IPV6
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
#endif
#ifdef AF_UNIX
	case AF_UNIX:
		return sizeof(struct sockaddr_un);
#endif
	default:
		return 0;
	}
}
/* }}} */

PHPAPI php_stream *_php_stream_sock_open_from_socket(int socket, int persistent STREAMS_DC TSRMLS_DC)
{
	php_stream *stream;
	php_netstream_data_t *sock;

	sock = pemalloc(sizeof(php_netstream_data_t), persistent);
	memset(sock, 0, sizeof(php_netstream_data_t));

	sock->is_blocked = 1;
	sock->chunk_size = FG(def_chunk_size);
	sock->timeout.tv_sec = SOCKET_DEFAULT_TIMEOUT;
	sock->socket = socket;

	stream = php_stream_alloc_rel(&php_stream_socket_ops, sock, persistent, "r+");

	if (stream == NULL)	
		pefree(sock, persistent);

	return stream;
}

PHPAPI php_stream *_php_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, int persistent STREAMS_DC TSRMLS_DC)
{
	int socket;

	socket = php_hostconnect(host, port, socktype, timeout);

	if (socket == -1)
		return NULL;

	return php_stream_sock_open_from_socket_rel(socket, persistent);
}

PHPAPI php_stream *_php_stream_sock_open_unix(const char *path, int pathlen, int persistent,
		struct timeval *timeout STREAMS_DC TSRMLS_DC)
{
#if defined(AF_UNIX)
	int socketd;
	struct  sockaddr_un unix_addr;

	socketd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (socketd == SOCK_ERR)
		return NULL;

	memset(&unix_addr, 0, sizeof(unix_addr));
	unix_addr.sun_family = AF_UNIX;

	/* we need to be binary safe on systems that support an abstract
	 * namespace */
	if (pathlen >= sizeof(unix_addr.sun_path)) {
		/* On linux, when the path begins with a NUL byte we are
		 * referring to an abstract namespace.  In theory we should
		 * allow an extra byte below, since we don't need the NULL.
		 * BUT, to get into this branch of code, the name is too long,
		 * so we don't care. */
		pathlen = sizeof(unix_addr.sun_path) - 1;
	}
	
	memcpy(unix_addr.sun_path, path, pathlen);

	if (php_connect_nonb(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr), timeout) == SOCK_CONN_ERR) 
		return NULL;

	return php_stream_sock_open_from_socket_rel(socketd, persistent);
#else
	return NULL;
#endif
}

#if HAVE_OPENSSL_EXT
PHPAPI int php_stream_sock_ssl_activate_with_method(php_stream *stream, int activate, SSL_METHOD *method TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	SSL_CTX *ctx = NULL;

	if (!php_stream_is(stream, PHP_STREAM_IS_SOCKET)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_stream_sock_ssl_activate_with_method: stream is not a network stream");
		return FAILURE;
	}
	
	if (activate == sock->ssl_active)
		return SUCCESS;	/* already in desired mode */
	
	if (activate && sock->ssl_handle == NULL)	{
		ctx = SSL_CTX_new(method);
		if (ctx == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_stream_sock_ssl_activate_with_method: failed to create an SSL context");
			return FAILURE;
		}

		sock->ssl_handle = SSL_new(ctx);
		if (sock->ssl_handle == NULL)	{
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_stream_sock_ssl_activate_with_method: failed to create an SSL handle");
			SSL_CTX_free(ctx);
			return FAILURE;
		}
		
		SSL_set_fd(sock->ssl_handle, sock->socket);
	}

	if (activate)	{
		if (SSL_connect(sock->ssl_handle) <= 0)	{
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_stream_sock_ssl_activate_with_method: SSL handshake/connection failed");
			SSL_shutdown(sock->ssl_handle);
			return FAILURE;
		}
		sock->ssl_active = activate;
	}
	else	{
		SSL_shutdown(sock->ssl_handle);
		sock->ssl_active = 0;
	}
	return SUCCESS;
}
#endif

PHPAPI void php_stream_sock_set_timeout(php_stream *stream, struct timeval *timeout TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	if (!php_stream_is(stream, PHP_STREAM_IS_SOCKET))
		return;
	
	sock->timeout = *timeout;
	sock->timeout_event = 0;
}

PHPAPI int php_set_sock_blocking(int socketd, int block TSRMLS_DC)
{
      int ret = SUCCESS;
      int flags;
      int myflag = 0;

#ifdef PHP_WIN32
      /* with ioctlsocket, a non-zero sets nonblocking, a zero sets blocking */
	  flags = !block;
	  if (ioctlsocket(socketd, FIONBIO, &flags)==SOCKET_ERROR){
		  php_error(E_WARNING, "%s", WSAGetLastError());
		  ret = FALSE;
      }
#else
      flags = fcntl(socketd, F_GETFL);
#ifdef O_NONBLOCK
      myflag = O_NONBLOCK; /* POSIX version */
#elif defined(O_NDELAY)
      myflag = O_NDELAY;   /* old non-POSIX version */
#endif
      if (!block) {
              flags |= myflag;
      } else {
              flags &= ~myflag;
      }
      fcntl(socketd, F_SETFL, flags);
#endif
      return ret;
}

PHPAPI size_t php_stream_sock_set_chunk_size(php_stream *stream, size_t size TSRMLS_DC)
{
	size_t oldsize;
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	if (!php_stream_is(stream, PHP_STREAM_IS_SOCKET))
		return 0;
	
	oldsize = sock->chunk_size;
	sock->chunk_size = size;

	return oldsize;
}

#define TOREAD(sock) ((sock)->writepos - (sock)->readpos)
#define READPTR(sock) ((sock)->readbuf + (sock)->readpos)
#define WRITEPTR(sock) ((sock)->readbuf + (sock)->writepos)

static size_t php_sockop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	size_t didwrite;
	
#if HAVE_OPENSSL_EXT
	if (sock->ssl_active)
		didwrite = SSL_write(sock->ssl_handle, buf, count);
	else
#endif
	didwrite = send(sock->socket, buf, count, 0);
	
	php_stream_notify_progress_increment(stream->context, didwrite, 0);

	return didwrite;
}

#if ZEND_DEBUG && DEBUG_MAIN_NETWORK
static inline void dump_sock_state(char *msg, php_netstream_data_t *sock TSRMLS_DC)
{
	printf("%s: blocked=%d timeout_event=%d eof=%d inbuf=%d timeout=%d\n", msg, sock->is_blocked, sock->timeout_event, sock->eof, TOREAD(sock), sock->timeout);
}
# define DUMP_SOCK_STATE(msg, sock)	dump_sock_state(msg, sock TSRMLS_CC)
#else
# define DUMP_SOCK_STATE(msg, sock)	/* nothing */
#endif

static void php_sock_stream_wait_for_data(php_stream *stream, php_netstream_data_t *sock TSRMLS_DC)
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

DUMP_SOCK_STATE("wait_for_data", sock);

		retval = select(sock->socket + 1, &tfdr, NULL, NULL, ptimeout);

		if (retval == 0)
			sock->timeout_event = 1;

		if (retval >= 0)
			break;
	}
DUMP_SOCK_STATE("wait_for_data: done", sock);
}


static size_t php_sock_stream_read_internal(php_stream *stream, php_netstream_data_t *sock TSRMLS_DC)
{
	char buf[PHP_SOCK_CHUNK_SIZE];
	int nr_bytes;
	size_t nr_read = 0;
	
	/* For blocking sockets, we wait until there is some
	   data to read (real data or EOF)

	   Otherwise, recv() may time out and return 0 and
	   therefore sock->eof would be set errornously.
	 */

DUMP_SOCK_STATE("read_internal entry", sock);

	if(sock->is_blocked) {
		php_sock_stream_wait_for_data(stream, sock TSRMLS_CC);
		if (sock->timeout_event)
			return 0;
	}

DUMP_SOCK_STATE("read_internal about to recv/SSL_read", sock);

	/* read at a maximum sock->chunk_size */
#if HAVE_OPENSSL_EXT
	if (sock->ssl_active)
		nr_bytes = SSL_read(sock->ssl_handle, buf, sock->chunk_size);
	else
#endif
	nr_bytes = recv(sock->socket, buf, sock->chunk_size, 0);
DUMP_SOCK_STATE("read_internal after recv/SSL_read", sock);

#if DEBUG_MAIN_NETWORK
printf("read_internal read %d/%d bytes\n", nr_bytes, sock->chunk_size);
#endif

	if(nr_bytes > 0) {

		php_stream_notify_progress_increment(stream->context, nr_bytes, 0);
		
		/* try to avoid an ever-expanding buffer */
		if (sock->readpos > 0) {
			memmove(sock->readbuf, READPTR(sock), sock->readbuflen - sock->readpos);
			sock->writepos -= sock->readpos;
			sock->readpos = 0;
		}
		
		if(sock->writepos + nr_bytes > sock->readbuflen) {
			sock->readbuflen += sock->chunk_size;
			sock->readbuf = perealloc(sock->readbuf, sock->readbuflen,
					php_stream_is_persistent(stream));
		}
		memcpy(WRITEPTR(sock), buf, nr_bytes);
		sock->writepos += nr_bytes;
		nr_read = nr_bytes;
	} else if(nr_bytes == 0 || (nr_bytes < 0 && streams_socket_errno != EWOULDBLOCK)) {
		sock->eof = 1;
	}

	return nr_read;

}

static size_t php_sock_stream_read(php_stream *stream, php_netstream_data_t *sock TSRMLS_DC)
{
	size_t nr_bytes;
	size_t nr_read = 0;
	int i;

	for(i = 0; !sock->eof && i < MAX_CHUNKS_PER_READ; i++) {
DUMP_SOCK_STATE("read about to read_internal", sock);
		nr_bytes = php_sock_stream_read_internal(stream, sock TSRMLS_CC);
		if(nr_bytes == 0)
			break;
		
		
		nr_read += nr_bytes;
	}

	return nr_read;
}

static size_t php_sockop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	size_t ret = 0;

	if (buf == NULL && count == 0) {
		/* check for EOF condition */
		int save_blocked;
		
DUMP_SOCK_STATE("check for EOF", sock);
		
		if (sock->eof)
			return EOF;
		
		if (TOREAD(sock))
			return 0;

		/* no data in the buffer - lets examine the socket */
#if HAVE_SYS_POLL_H && HAVE_POLL
		{
			struct pollfd topoll;
			
			topoll.fd = sock->socket;
			topoll.events = POLLIN;
			topoll.revents = 0;
			
			if (poll(&topoll, 1, 0) == 1) {
				return topoll.revents & POLLHUP ? EOF : 0;
			}
		}
#endif
		
		/* in the absence of other methods of checking if the
		 * socket is still active, try to read a chunk of data,
		 * but lets not block. */
		sock->timeout_event = 0;
		save_blocked = php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, 0, NULL);
		php_sock_stream_read_internal(stream, sock TSRMLS_CC);
		php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, save_blocked, NULL);
		
		if (sock->eof)
			return EOF;
		
		return 0;
	}
	
	if (sock->is_blocked) {
		sock->timeout_event = 0;
		while(!sock->eof && TOREAD(sock) < count && !sock->timeout_event)
			if (php_sock_stream_read_internal(stream, sock TSRMLS_CC) == 0)
				break;
	} else {
		php_sock_stream_read(stream, sock TSRMLS_CC);
	}

	if(count < 0)
		return ret;

	ret = MIN(TOREAD(sock), count);
	if (ret) {
		memcpy(buf, READPTR(sock), ret);
		sock->readpos += ret;
	}
#if DEBUG_MAIN_NETWORK
	DUMP_SOCK_STATE("sockop_read", sock);
	printf("sockop_read returning with %d bytes read\n", ret);
#endif
	return ret;
}

static int php_sockop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	if (close_handle) {
#if HAVE_OPENSSL_EXT
		if (sock->ssl_active) {
			SSL_shutdown(sock->ssl_handle);
			sock->ssl_active = 0;
		}
		if (sock->ssl_handle) {
			SSL_free(sock->ssl_handle);
			sock->ssl_handle = NULL;
		}
#endif

		shutdown(sock->socket, 0);
		closesocket(sock->socket);

	}
	if (sock->readbuf)
		pefree(sock->readbuf, php_stream_is_persistent(stream));

	pefree(sock, php_stream_is_persistent(stream));
	
	return 0;
}

static int php_sockop_flush(php_stream *stream TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fsync(sock->socket);
}

static int php_sockop_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	return fstat(sock->socket, &ssb->sb);
}

static int php_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	int oldmode;
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	switch(option) {
		case PHP_STREAM_OPTION_BLOCKING:
	
			oldmode = sock->is_blocked;
	
			/* no need to change anything */
			if (value == oldmode)
				return oldmode;
	
			if (SUCCESS == php_set_sock_blocking(sock->socket, value TSRMLS_CC)) {
				sock->is_blocked = value;
				return oldmode;
			}

			return -1;

		default:
			return -1;
	}
}

static int php_sockop_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
#if HAVE_OPENSSL_EXT
			if (sock->ssl_active)
				return FAILURE;
#endif
			if (ret)	{
				/* DANGER!: data buffered in stream->readbuf will be forgotten! */
				if (TOREAD(sock) > 0)
					zend_error(E_WARNING, "%s(): %d bytes of buffered data lost during conversion to FILE*!", get_active_function_name(TSRMLS_C), TOREAD(sock));
				*ret = fdopen(sock->socket, stream->mode);
				if (*ret)
					return SUCCESS;
				return FAILURE;
			}
			return SUCCESS;
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
#if HAVE_OPENSSL_EXT
			if (sock->ssl_active)
				return FAILURE;
#endif
			if (ret)
				*ret = (void*)sock->socket;
			return SUCCESS;
		default:
			return FAILURE;
	}
}

#define SEARCHCR() do {												\
	if (TOREAD(sock)) {												\
		for (p = READPTR(sock), pe = p + MIN(TOREAD(sock), maxlen);	\
				*p != '\n'; ) 										\
			if (++p >= pe) { 										\
				p = NULL; 											\
				break; 												\
			}														\
	} else															\
		p = NULL;													\
} while (0)


static char *php_sockop_gets(php_stream *stream, char *buf, size_t maxlen TSRMLS_DC)
{
	php_netstream_data_t *sock = (php_netstream_data_t*)stream->abstract;
	char *p = NULL, *pe;
	char *ret = NULL;
	size_t amount = 0;

	if (maxlen==0) {
		buf[0] = 0;
		return buf;
	}

	SEARCHCR();

	if(!p) {
		if(sock->is_blocked) {
			sock->timeout_event = 0;
			
			while(!p && !sock->eof && !sock->timeout_event && TOREAD(sock) < maxlen) {
				php_sock_stream_read_internal(stream, sock TSRMLS_CC);
				SEARCHCR();
			}
		} else {
			php_sock_stream_read(stream, sock TSRMLS_CC);
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
	if(amount || TOREAD(sock) || !sock->eof) {
		ret = buf;
	}

	return ret;
}

php_stream_ops php_stream_socket_ops = {
	php_sockop_write, php_sockop_read,
	php_sockop_close, php_sockop_flush,
	"socket",
	NULL, php_sockop_gets,
	php_sockop_cast,
	php_sockop_stat,
	php_sockop_set_option
};




/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
