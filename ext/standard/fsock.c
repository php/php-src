/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/
/* $Id$ */

#include "php.h"
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
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

#ifndef THREAD_SAFE
extern int le_fp;
#endif

#define FREE_SOCK if(socketd >= 0) close(socketd); efree(sock); if (key) efree(key)

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
	char *readbuf;
	size_t readbuflen;
	size_t readpos;
	size_t writepos;
	struct php3i_sockbuf *next;
};

static struct php3i_sockbuf *phpsockbuf;

typedef struct php3i_sockbuf php3i_sockbuf;

static int php3_minit_fsock(INIT_FUNC_ARGS);
static int php3_mshutdown_fsock(SHUTDOWN_FUNC_ARGS);
static int php3_rshutdown_fsock(SHUTDOWN_FUNC_ARGS);

php3_module_entry fsock_module_entry = {
	"Socket functions", fsock_functions, php3_minit_fsock, php3_mshutdown_fsock, NULL, php3_rshutdown_fsock, NULL, STANDARD_MODULE_PROPERTIES
};

#ifndef THREAD_SAFE
static HashTable ht_keys;
static HashTable ht_socks;
#endif

/* {{{ lookup_hostname */

/*
 * Converts a host name to an IP address.
 */
int lookup_hostname(const char *addr, struct in_addr *in)
{
	struct hostent *host_info;

	if(!inet_aton(addr, in)) {
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

	if (_php3_hash_find(&ht_socks, (char *) &sock, sizeof(sock),
				(void **) &key) == SUCCESS) {
		return 1;
	}
	return 0;
}
/* }}} */
/* {{{ _php3_fsockopen() */

/* 
   This function takes an optional third argument which should be
   passed by reference.  The error code from the connect call is written
   to this variable.
*/
static void _php3_fsockopen(INTERNAL_FUNCTION_PARAMETERS, int persistent) {
	pval *args[4];
	int *sock=emalloc(sizeof(int));
	int *sockp;
	int id, arg_count=ARG_COUNT(ht);
	int socketd = -1;
	unsigned short portno;
	char *key = NULL;
	
	if (arg_count > 4 || arg_count < 2 || getParametersArray(ht,arg_count,args)==FAILURE) {
		FREE_SOCK;
		WRONG_PARAM_COUNT;
	}
	switch(arg_count) {
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

	if (persistent && _php3_hash_find(&ht_keys, key, strlen(key) + 1,
				(void *) &sockp) == SUCCESS) {
		efree(key);
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
  
		if (connect(socketd, (struct sockaddr *)&server, sizeof(server)) == SOCK_CONN_ERR) {
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

		if (connect(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr)) == SOCK_CONN_ERR) {
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
		_php3_hash_update(&ht_keys, key, strlen(key) + 1, 
				sock, sizeof(*sock), NULL);
		_php3_hash_update(&ht_socks, (char *) sock, sizeof(*sock),
				key, strlen(key) + 1, NULL);
	}
	if(key) efree(key);
	id = php3_list_insert(sock,wsa_fp);
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int fsockopen(string hostname, int port [, int errno [, string errstr]])
   Open Internet or Unix domain socket connection */
PHP_FUNCTION(fsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */
/* {{{ proto int pfsockopen(string hostname, int port [, int errno [, string errstr]])
   Open persistent Internet or Unix domain socket connection */
PHP_FUNCTION(pfsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* Known issues with the socket buffering code:
 * - does not work reliably with persistent sockets yet
 *   (buffered data is not persistent)
 * - php3_fopen_url_wrapper() is still doing single-byte lookahead/read
 */

static php3i_sockbuf *_php3_sock_findsock(int socket)
{
	/* FIXME: O(n) could be improved */

	php3i_sockbuf *buf = NULL, *tmp;
	
	for(tmp = phpsockbuf; tmp; tmp = tmp->next)
		if(tmp->socket == socket) {
			buf = tmp;
			break;
		}

	return buf;
}

int _php3_sock_eof(int socket)
{
	php3i_sockbuf *sockbuf;
	int ret = 0;

	sockbuf = _php3_sock_findsock(socket);
	if(sockbuf) {
		ret = (sockbuf->writepos - sockbuf->readpos) == 0 ? 1 : 0;
	}
	return ret;
}

/* {{{ _php3_sock_fgets() */
int _php3_sock_fgets(char *buf, int maxlen, int socket)
{
	struct php3i_sockbuf *sockbuf;
	int bytesread, toread, len, buflen, count = 0;
	char *nl;
	
	sockbuf = _php3_sock_findsock(socket);
	
	if (sockbuf) {
		toread = sockbuf->writepos - sockbuf->readpos;
		if (toread > maxlen) {
			toread = maxlen;
		}
		if ((nl = memchr(sockbuf->readbuf + sockbuf->readpos, '\n', toread)) != NULL) {
			toread = (nl - (sockbuf->readbuf + sockbuf->readpos)) + 1;
		}
		memcpy(buf, sockbuf->readbuf + sockbuf->readpos, toread);
		sockbuf->readpos += toread;
		count += toread;
		buf += toread;
		if (sockbuf->readpos >= sockbuf->writepos) {
			sockbuf->readpos = sockbuf->writepos = 0;
		}
		if (nl != NULL) {
			/* if a newline was found, skip the recv() loop */
			goto sock_fgets_exit;
		}
	}

	nl = NULL;
	buflen = 0;
	while (count < maxlen && nl == NULL) {
		toread = maxlen - count;
		bytesread = recv(socket, buf, toread, 0);
		if (bytesread <= 0) {
			break;
		}
		if ((nl = memchr(buf, '\n', bytesread)) != NULL) {
			len = (nl - buf) + 1;
			count += len;
			buf += len;
			if (len < bytesread) {
				buflen = bytesread - len;
				break;
			}
		} else {
			count += bytesread;
			buf += bytesread;
		}
	}

	if (buflen > 0) { /* there was data after the "\n" ... */
		if (sockbuf == NULL) {
			sockbuf = emalloc(sizeof(struct php3i_sockbuf));
			sockbuf->socket = socket;
			sockbuf->readbuf = emalloc(maxlen);
			sockbuf->readbuflen = maxlen;
			sockbuf->readpos = sockbuf->writepos = 0;
			sockbuf->next = phpsockbuf;
			phpsockbuf = sockbuf;
		} else {
			uint needlen = sockbuf->writepos + buflen;

			if (needlen > sockbuf->readbuflen) {
				sockbuf->readbuflen += maxlen;
				sockbuf->readbuf = erealloc(sockbuf->readbuf, sockbuf->readbuflen);
			}
		}
		memcpy(sockbuf->readbuf + sockbuf->writepos, buf, buflen);
		sockbuf->writepos += buflen;
	}

 sock_fgets_exit:
	*buf = '\0';
	return count;
}

/* }}} */
/* {{{ _php3_sock_fread() */

int _php3_sock_fread(char *buf, int maxlen, int socket)
{
	struct php3i_sockbuf *sockbuf = phpsockbuf;
	int bytesread, toread, count = 0;

	while (sockbuf) {
		if (sockbuf->socket == socket) {
			toread = sockbuf->writepos - sockbuf->readpos;
			if (toread > maxlen) {
				toread = maxlen;
			}
			memcpy(buf, sockbuf->readbuf + sockbuf->readpos, toread);
			sockbuf->readpos += toread;
			count += toread;
			buf += toread;
			break;
		}
		sockbuf = sockbuf->next;
	}

	while (count < maxlen) {
		toread = maxlen - count;
		bytesread = recv(socket, buf, toread, 0);
		if (bytesread <= 0) {
			break;
		}
		count += bytesread;
		buf += bytesread;
	}

	*buf = '\0';
	return count;
}

/* }}} */
/* {{{ module start/shutdown functions */

	/* {{{ _php3_sock_destroy */
#ifndef THREAD_SAFE
static void _php3_sock_destroy(void *data)
{
	int *sock = (int *) data;
	close(*sock);
}
#endif
/* }}} */
	/* {{{ php3_minit_fsock */

static int php3_minit_fsock(INIT_FUNC_ARGS)
{
#ifndef THREAD_SAFE
	_php3_hash_init(&ht_keys, 0, NULL, NULL, 1);
	_php3_hash_init(&ht_socks, 0, NULL, _php3_sock_destroy, 1);
#endif
	return SUCCESS;
}
/* }}} */
	/* {{{ php3_mshutdown_fsock */

static int php3_mshutdown_fsock(SHUTDOWN_FUNC_ARGS)
{
#ifndef THREAD_SAFE
	_php3_hash_destroy(&ht_socks);
	_php3_hash_destroy(&ht_keys);
#endif
	return SUCCESS;
}
/* }}} */
    /* {{{ php3_rshutdown_fsock() */

static int php3_rshutdown_fsock(SHUTDOWN_FUNC_ARGS)
{
	struct php3i_sockbuf *sockbuf = phpsockbuf, *this;

	while (sockbuf) {
		this = sockbuf;
		sockbuf = this->next;
		efree(this->readbuf);
		efree(this);
	}
	phpsockbuf = NULL;
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
