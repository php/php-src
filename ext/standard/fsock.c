/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php3 revision 1.121 1999-06-18 [ssb] */
/* Synced with php3 revision 1.133 1999-07-21 [sas] */

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
#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#undef AF_UNIX
#endif
#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include <string.h>
#include <errno.h>

#include "base64.h"
#include "file.h"
#include "post.h"
#include "url.h"
#include "fsock.h"

#ifndef ZTS
extern int le_fp;
#endif

#define FREE_SOCK if(socketd >= 0) close(socketd); efree(sock); if (key) efree(key)

#define SEARCHCR \
	p = memchr(READPTR(sock), '\n', MIN(TOREAD(sock), maxlen - 1));

#if WIN32|WINNT
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include "build-defs.h"
#endif

static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };

function_entry fsock_functions[] = {
      PHP_FE(fsockopen, third_and_fourth_args_force_ref)
      PHP_FE(pfsockopen, third_and_fourth_args_force_ref)
      {NULL, NULL, NULL}
};

struct php3i_sockbuf {
	int socket;
	unsigned char *readbuf;
	size_t readbuflen;
	size_t readpos;
	size_t writepos;
	struct php3i_sockbuf *next;
	struct php3i_sockbuf *prev;
	char eof;
	char persistent;
	char is_blocked;
	size_t chunk_size;
};

static struct php3i_sockbuf *phpsockbuf;

typedef struct php3i_sockbuf php3i_sockbuf;

php3_module_entry fsock_module_entry = {
	"Socket functions",
	fsock_functions,
	php3_minit_fsock,
	php3_mshutdown_fsock,
	NULL,
	php3_rshutdown_fsock,
	NULL,
	STANDARD_MODULE_PROPERTIES
};
 

/* {{{ lookup_hostname */

/*
 * Converts a host name to an IP address.
 */
int lookup_hostname(const char *addr, struct in_addr *in)
{
	struct hostent *host_info;

	if (!inet_aton(addr, in)) {
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
/* {{{ _php3_is_persistent_sock */

int _php3_is_persistent_sock(int sock)
{
	char *key;
	PLS_FETCH();

	if (_php3_hash_find(&PG(ht_fsock_socks), (char *) &sock, sizeof(sock),
				(void **) &key) == SUCCESS) {
		return 1;
	}
	return 0;
}
/* }}} */
/* {{{ connect_nonb */
PHPAPI int connect_nonb(int sockfd,
						struct sockaddr *addr,
						int addrlen,
						struct timeval *timeout)
{
/* probably won't work on Win32, someone else might try it (read: fix it ;) */
#if !defined(WIN32) && (defined(O_NONBLOCK) || defined(O_NDELAY))

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

	int flags;
	int n;
	int error = 0;
	int len;
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
#else /* !defined(WIN32) && ... */
      return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */
/* {{{ _php3_fsockopen() */

/* 
   This function takes an optional third argument which should be
   passed by reference.  The error code from the connect call is written
   to this variable.
*/
static void _php3_fsockopen(INTERNAL_FUNCTION_PARAMETERS, int persistent) {
	pval *args[5];
	int *sock=emalloc(sizeof(int));
	int *sockp;
	int id, arg_count=ARG_COUNT(ht);
	int socketd = -1;
	struct timeval timeout = { 60, 0 };
	unsigned short portno;
	unsigned long conv;
	char *key = NULL;
	PLS_FETCH();
	
	if (arg_count > 5 || arg_count < 2 || getParametersArray(ht,arg_count,args)==FAILURE) {
		FREE_SOCK;
		WRONG_PARAM_COUNT;
	}
	switch(arg_count) {
		case 5:
			convert_to_double(args[4]);
			conv = args[4]->value.dval * 1000000.0;
			timeout.tv_sec = conv / 1000000;
			timeout.tv_usec = conv % 1000000;
			/* fall-through */
		case 4:
			if(!ParameterPassedByReference(ht,4)) {
				php3_error(E_WARNING,"error string argument to fsockopen not passed by reference");
			}
			pval_copy_constructor(args[3]);
			args[3]->value.str.val = empty_string;
			args[3]->value.str.len = 0;
			args[3]->type = IS_STRING;
			/* fall-through */
		case 3:
			if(!ParameterPassedByReference(ht,3)) {
				php3_error(E_WARNING,"error argument to fsockopen not passed by reference");
			}
			args[2]->type = IS_LONG;
			args[2]->value.lval = 0;
			break;
	}
	convert_to_string(args[0]);
	convert_to_long(args[1]);
	portno = (unsigned short) args[1]->value.lval;

	key = emalloc(args[0]->value.str.len + 10);
	sprintf(key, "%s:%d", args[0]->value.str.val, portno);

	if (persistent && _php3_hash_find(&PG(ht_fsock_keys), key, strlen(key) + 1,
				(void *) &sockp) == SUCCESS) {
		FREE_SOCK;
		*sock = *sockp;
		RETURN_LONG(php3_list_insert(sock, wsa_fp));
	}
	
	if (portno) {
		struct sockaddr_in server;

		memset(&server, 0, sizeof(server));
		socketd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		server.sin_family = AF_INET;
		
		if (lookup_hostname(args[0]->value.str.val, &server.sin_addr)) {
			FREE_SOCK;
			RETURN_FALSE;
		}
  
		server.sin_port = htons(portno);
  
		if (connect_nonb(socketd, (struct sockaddr *)&server, sizeof(server), &timeout) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
			}
			RETURN_FALSE;
		}
#if defined(AF_UNIX)
	} else {
		/* Unix domain socket.  s->strval is socket name. */
		struct  sockaddr_un unix_addr;
		socketd = socket(AF_UNIX,SOCK_STREAM,0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		memset(&unix_addr,(char)0,sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strcpy(unix_addr.sun_path, args[0]->value.str.val);

		if (connect_nonb(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr), &timeout) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
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
		_php3_hash_update(&PG(ht_fsock_keys), key, strlen(key) + 1, 
				sock, sizeof(*sock), NULL);
		_php3_hash_update(&PG(ht_fsock_socks), (char *) sock, sizeof(*sock),
				key, strlen(key) + 1, NULL);
	}
	if(key) efree(key);
	id = php3_list_insert(sock,wsa_fp);
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int fsockopen(string hostname, int port [, int errno [, string errstr [, double timeout]]])
   Open Internet or Unix domain socket connection */
PHP_FUNCTION(fsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
/* {{{ proto int pfsockopen(string hostname, int port [, int errno [, string errstr [, double timeout]]])
   Open persistent Internet or Unix domain socket connection */
PHP_FUNCTION(pfsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

#define CHUNK_SIZE 8192
#define SOCK_DESTROY(sock) \
		if(sock->readbuf) pefree(sock->readbuf, sock->persistent); \
		if(sock->prev) sock->prev->next = sock->next; \
		if(sock->next) sock->next->prev = sock->prev; \
		if(sock == phpsockbuf) \
			phpsockbuf = sock->next; \
		pefree(sock, sock->persistent)

static size_t def_chunk_size = CHUNK_SIZE;

static void php_cleanup_sockbuf(int persistent)
{
	php3i_sockbuf *now, *next;

	for(now = phpsockbuf; now; now = next) {
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
      php3i_sockbuf *sock; \
      sock = _php3_sock_find(socket); \
      if(!sock) sock = _php3_sock_create(socket)

static php3i_sockbuf *_php3_sock_find(int socket)
{
	php3i_sockbuf *buf = NULL, *tmp;

	for(tmp = phpsockbuf; tmp; tmp = tmp->next) 
		if(tmp->socket == socket) {
			buf = tmp;
			break;
		}

	return buf;
}

static php3i_sockbuf *_php3_sock_create(int socket)
{
	php3i_sockbuf *sock;
	int persistent = _php3_is_persistent_sock(socket);

	sock = pecalloc(sizeof(*sock), 1, persistent);
	sock->socket = socket;
	if((sock->next = phpsockbuf)) 
		phpsockbuf->prev = sock;
	sock->persistent = persistent;
	sock->is_blocked = 1;
	sock->chunk_size = def_chunk_size;
	phpsockbuf = sock;

	return sock;
}

size_t _php3_sock_set_def_chunk_size(size_t size)
{
	size_t old;

	old = def_chunk_size;

	if(size <= CHUNK_SIZE || size > 0) 
		def_chunk_size = size;

	return old;
}

int _php3_sock_destroy(int socket)
{
	int ret = 0;
	php3i_sockbuf *sock;

	sock = _php3_sock_find(socket);
	if(sock) {
		ret = 1;
		SOCK_DESTROY(sock);
	}

	return ret;
}

#if !defined(WIN32) && !defined(WINNT)
#undef closesocket
#define closesocket close
#endif

#ifndef HAVE_SHUTDOWN
#undef shutdown
#define shutdown
#endif

#define SOCK_CLOSE(s) shutdown(s, 0); closesocket(s)

int _php3_sock_close(int socket)
{
	int ret = 0;
	php3i_sockbuf *sock;

	sock = _php3_sock_find(socket);
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

static void _php3_sock_wait_for_data(php3i_sockbuf *sock)
{
	fd_set fdr, tfdr;

	FD_ZERO(&fdr);
	FD_SET(sock->socket, &fdr);

	while(1) {
		tfdr = fdr;
		if(select(sock->socket + 1, &tfdr, NULL, NULL, NULL) == 1) 
			break;
	}
}

static size_t _php3_sock_read_internal(php3i_sockbuf *sock)
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
		_php3_sock_wait_for_data(sock);
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

static void _php3_sock_read_total(php3i_sockbuf *sock, size_t maxread)
{
	while(!sock->eof && TOREAD(sock) < maxread) {
		_php3_sock_read_internal(sock);
	}
}

static size_t _php3_sock_read(php3i_sockbuf *sock)
{
	size_t nr_bytes;
	size_t nr_read = 0;
	int i;
	
	for(i = 0; !sock->eof && i < MAX_CHUNKS_PER_READ; i++) {
		nr_bytes = _php3_sock_read_internal(sock);
		if(nr_bytes == 0) break;
		nr_read += nr_bytes;
	}

	return nr_read;
}

int _php3_sock_set_blocking(int socket, int mode)
{
	int old;
	SOCK_FIND(sock, socket);

	old = sock->is_blocked;
	
	sock->is_blocked = mode;
	
	return old;
}

#define SOCK_FIND_AND_READ_MAX(max) \
	SOCK_FIND(sock, socket); \
	if(sock->is_blocked) _php3_sock_read_total(sock, max); else _php3_sock_read(sock)

/* {{{ _php3_sock_fgets() */
/*
 * FIXME: fgets depends on '\n' as line delimiter
 */
char *_php3_sock_fgets(char *buf, size_t maxlen, int socket)
{
	char *p = NULL;
	char *ret = NULL;
	size_t amount = 0;
	SOCK_FIND(sock, socket);

	SEARCHCR;

	if(!p) {
		if(sock->is_blocked) {
			while(!p && !sock->eof && TOREAD(sock) < maxlen - 1) {
				_php3_sock_read_internal(sock);
				SEARCHCR;
			}
		} else {
			_php3_sock_read(sock);
			SEARCHCR;
		}
	}

	
	if(p) {
		amount = (ptrdiff_t) p - (ptrdiff_t) READPTR(sock) + 1;
	} else {
		amount = TOREAD(sock);
	}
	
	amount = MIN(amount, maxlen - 1);

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

int _php3_sock_fgetc(int socket)
{
	int ret = EOF;
	SOCK_FIND_AND_READ_MAX(1);

	if(TOREAD(sock) > 0) {
		ret = *READPTR(sock);
		sock->readpos++;
	}

	return ret;
}

int _php3_sock_feof(int socket)
{
	int ret = 0;
	SOCK_FIND(sock, socket);

	if(!sock->is_blocked)
		_php3_sock_read(sock);
	
	if(!TOREAD(sock) && sock->eof) 
		ret = 1;

	return ret;
}

/* {{{ _php3_sock_fread() */

size_t _php3_sock_fread(char *ptr, size_t size, int socket)
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

	/* {{{ _php3_sock_destroy */
#ifndef ZTS
static int _php3_msock_destroy(int *data)
{
	close(*data);
	return 1;
}
#endif
/* }}} */
	/* {{{ php3_minit_fsock */

int php3_minit_fsock(INIT_FUNC_ARGS)
{
#ifndef ZTS
	_php3_hash_init(&PG(ht_fsock_keys), 0, NULL, NULL, 1);
	_php3_hash_init(&PG(ht_fsock_socks), 0, NULL, (int (*)(void *))_php3_msock_destroy, 1);
#endif
	return SUCCESS;
}
/* }}} */
	/* {{{ php3_mshutdown_fsock */

int php3_mshutdown_fsock(SHUTDOWN_FUNC_ARGS)
{
#ifndef ZTS
	_php3_hash_destroy(&PG(ht_fsock_socks));
	_php3_hash_destroy(&PG(ht_fsock_keys));
#endif
	php_cleanup_sockbuf(1);
	return SUCCESS;
}
/* }}} */
    /* {{{ php3_rshutdown_fsock() */

int php3_rshutdown_fsock(SHUTDOWN_FUNC_ARGS)
{
	php_cleanup_sockbuf(0);
	return SUCCESS;
}

/* }}} */

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
