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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"


#if HAVE_SOCKETS

/* This hopefully will fix some compile errors on other platforms --
 * the usage of msg_control/msg_controllen are X/Open Extended attributes,
 * or so it seems, by reading HP/UX 10.20 manual pages. */

#define _XOPEN_SOURCE_EXTENDED

#include "ext/standard/info.h"
#include "php_sockets.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/uio.h>

#ifdef ZTS
int sockets_globals_id;
#else
php_sockets_globals sockets_globals;
#endif

/* It seems some platforms don't have this... */

#ifndef MSG_WAITALL
#ifdef LINUX
/* Well, it's defined on linux to this, and on a few
 * distributions, it doesn't get included... */
#define MSG_WAITALL 0x00000100
#else
#define MSG_WAITALL 0x00000000
#endif
#endif


/* Perform convert_to_long_ex on a list of items */
static void v_convert_to_long_ex(int items,...)
{
	va_list ap;
	zval **arg;
	int i;

	va_start(ap, items);

	for (i = 0; i < items; i++) {
		arg = va_arg(ap, zval **);
		convert_to_long_ex(arg);
	}
	va_end(ap);

}


static unsigned char second_and_third_args_force_ref[] =
{3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char second_fifth_and_sixth_args_force_ref[] =
{6, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char fourth_arg_force_ref[] =
{4, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE};

static unsigned char third_through_seventh_args_force_ref[] =
{7, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};

function_entry sockets_functions[] = {
	PHP_FE(fd_alloc, 			NULL)	/* OK */
	PHP_FE(fd_dealloc, 			NULL)	/* OK */
	PHP_FE(fd_set, 				NULL)	/* OK */
	PHP_FE(fd_isset, 			NULL)	/* OK */
	PHP_FE(fd_clear, 			NULL)	/* OK */
	PHP_FE(fd_zero, 			NULL)	/* OK */
	PHP_FE(select, 				NULL)	/* OK */
	PHP_FE(open_listen_sock, 		NULL)	/* OK */
	PHP_FE(accept_connect, 			NULL)	/* OK */
	PHP_FE(set_nonblock, 			NULL)	/* OK */
	PHP_FE(listen, 				NULL)	/* OK */
	PHP_FE(close, 				NULL)	/* OK */
	PHP_FE(write, 				NULL)	/* OK */
	PHP_FE(read, 				second_arg_force_ref)	/* OK */
#if 0
/* 
 * If and when asynchronous context switching is avaliable, 
 * this will work. Otherwise, forget it. 
 */
	PHP_FE(signal, 				NULL)
#endif
/* 
 * These are defined elsewhere.. 
 * would these be more appropriate?
 */
#if 0
	PHP_FE(gethostbyname, 			second_arg_force_ref)	/* OK */
	PHP_FE(gethostbyaddr, 			second_arg_force_ref)	/* OK */
#endif
	PHP_FE(getsockname, 			second_and_third_args_force_ref)	/* OK */
	PHP_FE(getpeername, 			second_and_third_args_force_ref)	/* OK */
	PHP_FE(socket, 				NULL)	/* OK */
	PHP_FE(connect, 			NULL)	/* OK */
	PHP_FE(strerror, 			NULL)	/* OK */
	PHP_FE(bind,				NULL)
	PHP_FE(recv,				second_arg_force_ref)
	PHP_FE(send,				NULL)
	PHP_FE(recvfrom,			second_fifth_and_sixth_args_force_ref)
	PHP_FE(sendto,				NULL)
	PHP_FE(build_iovec,			NULL)
	PHP_FE(fetch_iovec,			NULL)
	PHP_FE(free_iovec,			NULL)
	PHP_FE(add_iovec,			NULL)
	PHP_FE(delete_iovec,		NULL)
	PHP_FE(set_iovec,			NULL)
	PHP_FE(recvmsg,				third_through_seventh_args_force_ref)
	PHP_FE(sendmsg,				NULL)
	PHP_FE(readv,				NULL)
	PHP_FE(writev,				NULL)
	PHP_FE(getsockopt,			fourth_arg_force_ref)
	PHP_FE(setsockopt,			NULL)
	PHP_FE(socketpair,			NULL)
	PHP_FE(shutdown,            NULL)
	{NULL, NULL, NULL}
};

zend_module_entry sockets_module_entry = {
	"sockets",
	sockets_functions,
	PHP_MINIT(sockets),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sockets),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

static void destroy_fd_sets(zend_rsrc_list_entry *rsrc)
{
	fd_set *set = (fd_set *) rsrc->ptr;
	efree(set);
}

static void destroy_iovec(zend_rsrc_list_entry *rsrc)
{
	int i;
	php_iovec_t *iov = (php_iovec_t *) rsrc->ptr;

	if (iov->count && iov->iov_array) {
		for (i = 0; i < iov->count; i++) {
			efree(iov->iov_array[i].iov_base);
		}

		efree(iov->iov_array);
		efree(iov);
	}
}

PHP_MINIT_FUNCTION(sockets)
{
	SOCKETSLS_FETCH();
	SOCKETSG(le_destroy) = register_list_destructors(destroy_fd_sets, NULL, "sockets file descriptor set");
	SOCKETSG(le_iov)     = register_list_destructors(destroy_iovec,   NULL, "sockets i/o vector");

	REGISTER_LONG_CONSTANT("AF_UNIX", AF_UNIX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AF_INET", AF_INET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_STREAM", SOCK_STREAM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_DGRAM", SOCK_DGRAM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RAW", SOCK_RAW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_SEQPACKET", SOCK_SEQPACKET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RDM", SOCK_RDM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_OOB", MSG_OOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_WAITALL", MSG_WAITALL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_PEEK", MSG_PEEK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_DONTROUTE", MSG_DONTROUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DEBUG", SO_DEBUG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_REUSEADDR", SO_REUSEADDR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_KEEPALIVE", SO_KEEPALIVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DONTROUTE", SO_DONTROUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LINGER", SO_LINGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_BROADCAST", SO_BROADCAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_OOBINLINE", SO_OOBINLINE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDBUF", SO_SNDBUF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVBUF", SO_RCVBUF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDLOWAT", SO_SNDLOWAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVLOWAT", SO_RCVLOWAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDTIMEO", SO_SNDTIMEO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVTIMEO", SO_RCVTIMEO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_TYPE", SO_TYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_ERROR", SO_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOL_SOCKET", SOL_SOCKET, CONST_CS | CONST_PERSISTENT);
	
	{
		struct protoent *pe;
		pe = getprotobyname("tcp");
		if (pe) {
			REGISTER_LONG_CONSTANT("SOL_TCP", pe->p_proto, CONST_CS | CONST_PERSISTENT);
		}
		
		pe = getprotobyname("udp");
		if (pe) {
			REGISTER_LONG_CONSTANT("SOL_UDP", pe->p_proto, CONST_CS | CONST_PERSISTENT);
		}
	}
	
	return SUCCESS;
}

PHP_MINFO_FUNCTION(sockets)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "sockets support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto resource fd_alloc(void)
   Allocates a file descriptor set */
PHP_FUNCTION(fd_alloc)
{
	fd_set *set;
	SOCKETSLS_FETCH();

	set = (fd_set *)emalloc(sizeof(fd_set));
	if (!set) {
		php_error(E_WARNING, "Can't allocate memory for fd_set");
		RETURN_FALSE;
	}
	
	ZEND_REGISTER_RESOURCE(return_value, set, SOCKETSG(le_destroy));
}
/* }}} */

/* {{{ proto void fd_dealloc(void)
   De-allocates a file descriptor set */
/*   ** BUG: This is currently a no-op! */
PHP_FUNCTION(fd_dealloc)
{
	RETURN_NULL();
}
/* }}} */

/* {{{ proto bool fd_set(int fd, resource set)
   Adds a file descriptor to a set */
PHP_FUNCTION(fd_set)
{
	zval **set, **fd;
	fd_set *the_set;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", SOCKETSG(le_destroy));

	if (Z_LVAL_PP(fd) < 0) {
		php_error(E_WARNING, "Can't set negative fd falues in a set");
		RETURN_FALSE;
	}

	FD_SET(Z_LVAL_PP(fd), the_set);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fd_clear(int fd, resource set)
   Clears a file descriptor from a set */
PHP_FUNCTION(fd_clear)
{
	zval **set, **fd;
	fd_set *the_set;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", SOCKETSG(le_destroy));

	if (Z_LVAL_PP(fd) < 0) {
		php_error(E_WARNING, "Can't clear negative fd values in a set");
		RETURN_FALSE;
	}

	FD_CLR(Z_LVAL_PP(fd), the_set);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fd_isset(int fd, resource set)
   Checks to see if a file descriptor is set within the file descrirptor set */
PHP_FUNCTION(fd_isset)
{
	zval **set, **fd;
	fd_set *the_set;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", SOCKETSG(le_destroy));

	if (Z_LVAL_PP(fd) < 0) {
		php_error(E_WARNING, "Can't check for negative fd values in a set");
		RETURN_FALSE;
	}

	if (FD_ISSET(Z_LVAL_PP(fd), the_set)) {
		RETURN_TRUE;
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void fd_zero(resource set)
   Clears a file descriptor set */
PHP_FUNCTION(fd_zero)
{
	zval **set;
	fd_set *the_set;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", SOCKETSG(le_destroy));

	FD_ZERO(the_set);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int select(int max_fd, resource readfds, resource writefds, resource exceptfds, int tv_sec, int tv_usec)
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
/* See select(2) man page for details.

   From man page:
   select waits for a number of file descriptors to change status.

   Three independent sets of descriptors are watched.  Those in 
   readfds will be watched to see if characters become avaliable for 
   reading, those in writefds will be watched to see if it is ok to 
   immediately write on them, and those in exceptfds will be watched 
   for exceptions.  On exit, the sets are modified in place to 
   indicate which descriptors actually changed status.

   -1 is passed for any sets for which NULL would be passed to the 
   system call.
 */

PHP_FUNCTION(select)
{
	zval **max_fd, **readfds, **writefds, 
	     **exceptfds, **tv_sec, **tv_usec;

	struct timeval tv;
	fd_set *rfds, *wfds, *xfds;
	int ret = 0;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 || 
	    zend_get_parameters_ex(6, &max_fd, &readfds, &writefds, &exceptfds, &tv_sec, &tv_usec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, max_fd, tv_sec, tv_usec);

	tv.tv_sec  = Z_LVAL_PP(tv_sec);
	tv.tv_usec = Z_LVAL_PP(tv_usec);

	if (Z_LVAL_PP(readfds) != 0) {
		ZEND_FETCH_RESOURCE(rfds, fd_set *, readfds, -1, "File descriptor set", SOCKETSG(le_destroy));
	} else {
		rfds = NULL;
	}
	
	if (Z_LVAL_PP(writefds) != 0) {
		ZEND_FETCH_RESOURCE(wfds, fd_set *, writefds, -1, "File descriptor set", SOCKETSG(le_destroy));
	} else {
		wfds = NULL;
	}
	
	if (Z_LVAL_PP(exceptfds) != 0) {
		ZEND_FETCH_RESOURCE(xfds, fd_set *, exceptfds, -1, "File descriptor set", SOCKETSG(le_destroy));
	} else {
		xfds = NULL;
	}

	ret = select(Z_LVAL_PP(max_fd), rfds, wfds, xfds, &tv);

	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

static int open_listen_sock(int port)
{
	int fd;
	struct sockaddr_in la;
	struct hostent *hp;

	if ((hp = gethostbyname("0.0.0.0")) == NULL) {
		return -1;
	}
	
	memcpy((char *)&la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons(port);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	if ((bind(fd, (struct sockaddr *)&la, sizeof(la)) < 0)) {
		return -1;
	}
	listen(fd, 128);
	return fd;
}

/* {{{ proto int open_listen_sock(int port)
   Opens a socket on port to accept connections */
PHP_FUNCTION(open_listen_sock)
{
	zval **port;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);

	ret = open_listen_sock(Z_LVAL_PP(port));

	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

static int accept_connect(int fd, struct sockaddr *la)
{
	int ret, m;

	m = sizeof(*la);
	if ((ret = accept(fd, la, &m)) < 0) {
		return -1;
	}
	return ret;
}

/* {{{ proto int accept_connect(int fd)
   Accepts a connection on the listening socket fd */
PHP_FUNCTION(accept_connect)
{
	zval **fd;
	int ret;
	struct sockaddr_in sa;

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &fd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ret = accept_connect(Z_LVAL_PP(fd), (struct sockaddr *)&sa);
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto bool set_nonblock(int fd)
   Sets nonblocking mode for file descriptor fd */
PHP_FUNCTION(set_nonblock)
{
	zval **fd;
	int ret;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &fd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ret = fcntl(Z_LVAL_PP(fd), F_SETFL, O_NONBLOCK);
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto bool listen(int fd, int backlog)
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(listen)
{
	zval **fd, **backlog;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &backlog) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, backlog);

	ret = listen(Z_LVAL_PP(fd), Z_LVAL_PP(backlog));
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto bool close(int fd)
   Closes a file descriptor */
PHP_FUNCTION(close)
{
	zval **fd;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &fd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);
	ret = close(Z_LVAL_PP(fd));
	
	if (ret == -1) {
		php_error(E_WARNING, "Invalid file descriptor");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int write(int fd, string buf, int length)
   Writes length bytes of buf to the file descriptor fd */
PHP_FUNCTION(write)
{
	zval **fd, **buf, **length;
	int ret;

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &fd, &buf, &length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	v_convert_to_long_ex(2, fd, length);
	convert_to_string_ex(buf);

	if (Z_STRLEN_PP(buf) < Z_LVAL_PP(length)) {
		ret = write(Z_LVAL_PP(fd), (void *)Z_STRVAL_PP(buf), Z_STRLEN_PP(buf));
	} else {
		ret = write(Z_LVAL_PP(fd), (void *)Z_STRVAL_PP(buf), Z_LVAL_PP(length));
	}
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int read(int fd, string &buf, int length [, int binary])
   Reads length bytes from fd into buf */

/* php_read -- wrapper around read() so that it only reads to a \r, \n, or \0. */

int php_read(int fd, void *buf, int maxlen, int binary)
{
     char *t;
     int m = 0, n = 0;
     int no_read = 0;
     int nonblock = 0;
     int keep_going = 1;
     errno = 0;

     m = fcntl(fd, F_GETFL);
     if (m == -1)
          return -1;

     if (m & O_NONBLOCK)
          nonblock = 1;

     {
	char buf[255];
	snprintf(buf, 255, "nonblock = %i\n", nonblock);
	PUTS(buf);
     }

     t = (char *) buf;
     while (keep_going) { /* (*t != '\n' && *t != '\r' && *t != '\0' && n < maxlen) { */
          m = read(fd, (void *) t, 1);
          if (m > 0) {
               t++;
               n++;
          } else if (m == 0) {
               no_read++;
               if (no_read > 200) {
                    errno = ECONNRESET;
                    return -1;
               }
          }
          if (errno != 0 && errno != ESPIPE && errno != EAGAIN) {
               return -1;
          }
          errno = 0;
          if (!binary) {
		if (*t == '\n' || *t == '\r' || *t == '\0') {
			keep_going = 0;
		}
	  }
	  if (n == maxlen)
		keep_going = 0;
	  if (m == 0 && nonblock)
		keep_going = 0;
     }
     return n;
}

PHP_FUNCTION(read)
{
	zval **fd, **buf, **length, **binary;
	char *tmpbuf;
	int ret;

	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4 ||
	    zend_get_parameters_ex(ZEND_NUM_ARGS(), &fd, &buf, &length, &binary) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(ZEND_NUM_ARGS() - 1, fd, length, binary);
	convert_to_string_ex(buf);

	tmpbuf = emalloc(Z_LVAL_PP(length)*sizeof(char));
	if (tmpbuf == NULL) {
		php_error(E_WARNING, "Couldn't allocate memory from %s()", get_active_function_name());
		RETURN_FALSE;
	}
	
	ret = php_read(Z_LVAL_PP(fd), tmpbuf, Z_LVAL_PP(length), ZEND_NUM_ARGS() == 4 ? Z_LVAL_PP(binary) : 0);
	
	if (ret >= 0) {
		Z_STRVAL_PP(buf) = estrndup(tmpbuf,ret);
		Z_STRLEN_PP(buf) = ret;
		
		efree(tmpbuf);

		RETURN_LONG(ret);
	} else {
		efree(tmpbuf);
		RETURN_LONG(-errno);
	}
}
/* }}} */

/* {{{ proto int getsockname(int fd, string &addr, int &port)
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the local side of a socket */

/* A lock to prevent inet_ntoa() from causing problems in threading */
volatile int inet_ntoa_lock = 0;

PHP_FUNCTION(getsockname)
{
	zval **fd, **addr, **port;
	char *tmp;
	struct sockaddr_in sa;
	int salen = sizeof(struct sockaddr_in);
	int ret;

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &fd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, port);
	convert_to_string_ex(addr);

	ret = getsockname(Z_LVAL_PP(fd), (struct sockaddr *) &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		char *addr_string;

		while (inet_ntoa_lock == 1);
		inet_ntoa_lock = 1;
		addr_string = inet_ntoa(sa.sin_addr);
		tmp = emalloc(strlen(addr_string) + 1);
		memset(tmp, 0, strlen(addr_string) + 1);
		strncpy(tmp, addr_string, strlen(addr_string));
		inet_ntoa_lock = 0;
		
		Z_STRVAL_PP(addr) = tmp;
		Z_STRLEN_PP(addr) = strlen(tmp);
		Z_LVAL_PP(port)   = htons(sa.sin_port);

		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int gethostbyname(string name, string &addr)
   Given a hostname, sets addr to be a human-readable version of the host's address */

#if 0
/* Another lock to prevent multiple threads from grabbing gethostbyname() */
volatile int gethostbyname_lock = 0;

PHP_FUNCTION(gethostbyname)
{
	zval **name, **addr;
	int ret;
	char *tmp, *addr_string;
	struct hostent *host_struct;
	struct in_addr a;

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &name, &addr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(addr);

	while (gethostbyname_lock == 1);
	gethostbyname_lock = 1;

	host_struct = gethostbyname(Z_STRVAL_PP(name));
	if (!host_struct) {
		gethostbyname_lock = 0;
		RETURN_LONG(-(h_errno) - 10000);	/* return a value that is out of range for errno */
	}
	if (host_struct->h_addrtype != AF_INET) {
		gethostbyname_lock = 0;
		RETURN_LONG(-EINVAL);
	}
	while (inet_ntoa_lock == 1);
	inet_ntoa_lock = 1;

	a.s_addr = (int) (*(host_struct->h_addr_list[0]));
	addr_string = inet_ntoa(a);
	tmp = emalloc(strlen(addr_string) + 1);
	strncpy(tmp, addr_string, strlen(addr_string));

	inet_ntoa_lock = 0;
	gethostbyname_lock = 0;

	Z_STRVAL_PP(addr) = tmp;
	Z_STRLEN_PP(addr) = strlen(tmp);
	
	RETURN_LONG(0);
}

/* }}} */

#endif

/* {{{ proto int getpeername(int fd, string &addr, int &port)
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the remote side of a socket */

PHP_FUNCTION(getpeername)
{
	zval **fd, **addr, **port;
	char *tmp;
	struct sockaddr_in sa;
	int salen = sizeof(struct sockaddr_in);
	int ret;

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &fd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, port);
	convert_to_string_ex(addr);

	ret = getpeername(Z_LVAL_PP(fd), (struct sockaddr *) &sa, &salen);
	
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		char *addr_string;

		while (inet_ntoa_lock == 1);
		inet_ntoa_lock = 1;
		addr_string = inet_ntoa(sa.sin_addr);
		tmp = emalloc(strlen(addr_string) + 1);
		memset(tmp, 0, strlen(addr_string) + 1);
		strncpy(tmp, addr_string, strlen(addr_string));
		inet_ntoa_lock = 0;
		
		Z_STRVAL_PP(addr) = tmp;
		Z_STRLEN_PP(addr) = strlen(tmp);
		Z_LVAL_PP(addr) = htons(sa.sin_port);

		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int gethostbyaddr(string addr, string &name)
   Given a human-readable address, sets name to be the host's name */

#if 0
/* Another lock to prevent multiple threads from grabbing gethostbyname() */
volatile int gethostbyaddr_lock = 0;

PHP_FUNCTION(gethostbyaddr)
{
	zval **name, **addr;
	int ret;
	char *tmp, *addr_string;
	struct hostent *host_struct;
	struct in_addr addr_buf;

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &addr, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(addr);
	convert_to_string_ex(name);

	ret = inet_aton(Z_STRVAL_PP(addr), &addr_buf);
	
	if (ret < 0) {
		RETURN_LONG(-EINVAL);
	}
	
	while (gethostbyaddr_lock == 1);
	gethostbyaddr_lock = 1;

	host_struct = gethostbyaddr((char *)&addr_buf, sizeof(addr_buf), AF_INET);

	if (!host_struct) {
		gethostbyaddr_lock = 0;
		RETURN_LONG(-(h_errno) - 10000);
	}
	if (host_struct->h_addrtype != AF_INET) {
		gethostbyaddr_lock = 0;
		RETURN_LONG(-EINVAL);
	}
	
	addr_string = host_struct->h_name;
	tmp = emalloc(strlen(addr_string) + 1);
	strncpy(tmp, addr_string, strlen(addr_string));

	gethostbyaddr_lock = 0;

	Z_STRVAL_PP(addr) = tmp;
	Z_STRLEN_PP(addr) = strlen(tmp);

	RETURN_LONG(0);
}
/* }}} */

#endif

/* {{{ proto int socket(int domain, int type, int protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket)
{
	zval **domain, **type, **protocol;
	int ret;

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &domain, &type, &protocol) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, domain, type, protocol);

	if (Z_LVAL_PP(domain) != AF_INET && Z_LVAL_PP(domain) != AF_UNIX) {
		php_error(E_WARNING, "invalid socket domain specified - assuming AF_INET");
		Z_LVAL_PP(domain) = AF_INET;
	}
	
	if (Z_LVAL_PP(type) > 10) {
		php_error(E_WARNING, "invalid socket type specified - assuming SOCK_STREAM");
		Z_LVAL_PP(type) = SOCK_STREAM;
	}
	
	ret = socket(Z_LVAL_PP(domain), Z_LVAL_PP(type), Z_LVAL_PP(protocol));
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int connect(int sockfd, string addr [, int port])
   Opens a connection to addr:port on the socket specified by sockfd */
PHP_FUNCTION(connect)
{
	zval **sockfd, **addr, **port;
	struct sockaddr sa;
	struct sockaddr_in *sin;
	struct sockaddr_un *sun;
	int ret, salen = sizeof(sa);
	struct in_addr addr_buf;
	struct hostent *host_struct;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 ||
	    zend_get_parameters_ex(argc, &sockfd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sockfd)
	convert_to_string_ex(addr);
	
	if (argc > 2) {
		convert_to_long_ex(port);
	}

	memset(&sa, 0, sizeof(sa));

	ret = getsockname(Z_LVAL_PP(sockfd), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	switch(sa.sa_family) {
		case AF_INET: {
			sin = (struct sockaddr_in *)&sa;

			if (argc != 3) {
				WRONG_PARAM_COUNT;
			}

			sin->sin_port = htons((unsigned short int)Z_LVAL_PP(port));
			if (inet_aton(Z_STRVAL_PP(addr), &addr_buf) == 0) {
				sin->sin_addr.s_addr = addr_buf.s_addr;
			} else {
				char *q = (char *) &(sin->sin_addr.s_addr);
				host_struct = gethostbyname(Z_STRVAL_PP(addr));
				if (host_struct->h_addrtype != AF_INET) {
					RETURN_LONG(-EINVAL);
				}
				q[0] = host_struct->h_addr_list[0][0];
				q[1] = host_struct->h_addr_list[0][1];
				q[2] = host_struct->h_addr_list[0][2];
				q[3] = host_struct->h_addr_list[0][3];
			}
	
			ret = connect(Z_LVAL_PP(sockfd), (struct sockaddr *) sin, salen);
			break;
		}
	case AF_UNIX: {
			sun = (struct sockaddr_un *)&sa;
			snprintf(sun->sun_path, 108, "%s", Z_STRVAL_PP(addr));
			ret = connect(Z_LVAL_PP(sockfd), (struct sockaddr *) sun, SUN_LEN(sun));
			break;
	}
	default:
		ret = -1;
		errno = EINVAL;
	}	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto string strerror(int errno)
   Returns a string describing an error */
PHP_FUNCTION(strerror)
{
	zval **error;
	const char *buf;
	char *obuf;

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &error) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (Z_LVAL_PP(error) < -10000) {
		Z_LVAL_PP(error) += 10000;
		buf = hstrerror(-(Z_LVAL_PP(error)));
	} else {
		buf = strerror(-(Z_LVAL_PP(error)));
	}
	
	RETURN_STRING((char *)buf, 1);
}
/* }}} */

/* {{{ proto int bind(int sockfd, string addr [, int port])
   Binds an open socket to a listening port */
/* Port is only specified if sockfd is in the AF_INET family. */
PHP_FUNCTION(bind)
{
	zval **arg0, **arg1, **arg2;
	long ret;
	struct sockaddr sock_type;
	socklen_t length = sizeof(sock_type);
	
	switch (ZEND_NUM_ARGS())
	{
		case 2:
			if (zend_get_parameters_ex(2, &arg0, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &arg0, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg0);

	ret = getsockname(Z_LVAL_PP(arg0), &sock_type, &length);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	if (sock_type.sa_family == AF_UNIX) {
		struct sockaddr_un sa;
		snprintf(sa.sun_path, 108, "%s", Z_STRVAL_PP(arg1));
		ret = bind(Z_LVAL_PP(arg0), (struct sockaddr *) &sa, SUN_LEN(&sa));
	} else if (sock_type.sa_family == AF_INET) {
		struct sockaddr_in sa;
		struct in_addr addr_buf;

		if (ZEND_NUM_ARGS() != 3) {
			WRONG_PARAM_COUNT;
		}
		
		sa.sin_port = htons(Z_LVAL_PP(arg2));

		if (inet_aton(Z_STRVAL_PP(arg1), &addr_buf) < 0) {
			struct hostent *host_struct;

			if ((host_struct = gethostbyname(Z_STRVAL_PP(arg1))) == NULL) {
				RETURN_LONG(-(h_errno) - 10000);
			}
			
			sa.sin_addr.s_addr = (int) *(host_struct->h_addr_list[0]);
		} else {
			sa.sin_addr.s_addr = addr_buf.s_addr;
		}

		ret = bind(Z_LVAL_PP(arg0), (struct sockaddr *) &sa, sizeof(sa));
	} else {
		RETURN_LONG(-EPROTONOSUPPORT);
	}
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto resource build_iovec(int num_vectors [, int ...])
   Builds a 'struct iovec' for use with sendmsg, recvmsg, writev, and readv */
/* First parameter is number of vectors, each additional parameter is the
   length of the vector to create.
 */

PHP_FUNCTION(build_iovec)
{
	zval ***args = (zval ***)NULL;
	php_iovec_t *vector;
	struct iovec *vector_array;
	int i, j, num_vectors, argcount = ZEND_NUM_ARGS();
	SOCKETSLS_FETCH();
	
	args = emalloc(argcount * sizeof(zval *));

	if (argcount < 1 ||
	    zend_get_parameters_array_ex(argcount, args) == FAILURE) {
	    	if (args)
	    		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(args[0]);
	num_vectors = Z_LVAL_PP(args[0]);
	
	vector_array = emalloc(sizeof(struct iovec) * (num_vectors + 1));

	for (i = 0, j = 1; i < num_vectors; i++, j++) {
		convert_to_long_ex(args[j]);
		
		vector_array[i].iov_base = emalloc(Z_LVAL_PP(args[j]));
		vector_array[i].iov_len = Z_LVAL_PP(args[j]);
	}

	vector = emalloc(sizeof(php_iovec_t));
	vector->iov_array = vector_array;
	vector->count = num_vectors;

	ZEND_REGISTER_RESOURCE(return_value, vector, SOCKETSG(le_iov));
}
/* }}} */

/* {{{ proto string fetch_iovec(resource iovec_id, int iovec_position)
   Returns the data held in the iovec specified by iovec_id[iovec_position] */
PHP_FUNCTION(fetch_iovec)
{
	zval **iovec_id, **iovec_position;
	php_iovec_t *vector;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &iovec_id, &iovec_position) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	if (Z_LVAL_PP(iovec_position) > vector->count) {
		php_error(E_WARNING, "Can't access a vector position past the amount of vectors set in the array");
		RETURN_NULL();
	}
	RETURN_STRINGL(vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base,
		       vector->iov_array[Z_LVAL_PP(iovec_position)].iov_len, 1);
}
/* }}} */

/* {{{ proto bool set_iovec(resource iovec_id, int iovec_position, string new_val)
   Sets the data held in iovec_id[iovec_position] to new_val */
PHP_FUNCTION(set_iovec)
{
	zval **iovec_id, **iovec_position, **new_val;
	php_iovec_t *vector;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 3 || 
	    zend_get_parameters_ex(3, &iovec_id, &iovec_position, &new_val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	if (Z_LVAL_PP(iovec_position) > vector->count) {
		php_error(E_WARNING, "Can't access a vector position outside of the vector array bounds");
		RETURN_FALSE;
	}
	
	if (vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base) {
		efree(vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base);
	}
	
	vector->iov_array[Z_LVAL_PP(iovec_position)].iov_base = estrdup(Z_STRVAL_PP(new_val));
	vector->iov_array[Z_LVAL_PP(iovec_position)].iov_len = strlen(Z_STRVAL_PP(new_val));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool add_iovec(resource iovec_id, int iov_len)
   Adds a new vector to the scatter/gather array */
PHP_FUNCTION(add_iovec)
{
	zval **iovec_id, **iov_len;
	php_iovec_t *vector;
	struct iovec *vector_array;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &iovec_id, &iov_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	vector_array = emalloc(sizeof(struct iovec) * (vector->count + 2));
	memcpy(vector_array, vector->iov_array, sizeof(struct iovec) * vector->count);

	vector_array[vector->count].iov_base = emalloc(Z_LVAL_PP(iov_len));
	vector_array[vector->count].iov_len = Z_LVAL_PP(iov_len);
	efree(vector->iov_array);
	vector->iov_array = vector_array;
	vector->count++;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool delete_iovec(resource iovec_id, int iov_pos)
   Deletes a vector from an array of vectors */
PHP_FUNCTION(delete_iovec)
{
	zval **iovec_id, **iov_pos;
	php_iovec_t *vector;
	struct iovec *vector_array;
	int i;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &iovec_id, &iov_pos) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	if (Z_LVAL_PP(iov_pos) > vector->count) {
		php_error(E_WARNING, "Can't delete an IO vector that is out of array bounds");
		RETURN_FALSE;
	}
	vector_array = emalloc(vector->count * sizeof(struct iovec));

	for (i = 0; i < vector->count; i++) {
		if (i < Z_LVAL_PP(iov_pos)) {
			memcpy(&(vector->iov_array[i]), &(vector_array[i]), sizeof(struct iovec));
		} else if (i > Z_LVAL_PP(iov_pos)) {
			memcpy(&(vector->iov_array[i]), &(vector_array[i - 1]), sizeof(struct iovec));
		}
	}

	efree(vector->iov_array);
	vector->iov_array = vector_array;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool free_iovec(resource iovec_id)
   Frees the iovec specified by iovec_id */
PHP_FUNCTION(free_iovec)
{
	zval **iovec_id;
	php_iovec_t *vector;
	int pos;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	zend_list_delete(Z_LVAL_PP(iovec_id));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int readv(int fd, resource iovec_id)
   Reads from an fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(readv)
{
	zval **fd, **iovec_id;
	php_iovec_t *vector;
	int ret;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &fd, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	ret = readv(Z_LVAL_PP(fd), vector->iov_array, vector->count);
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int writev(int fd, resource iovec_id)
   Writes to a file descriptor, fd, using the scatter-gather array defined by iovec_id */
PHP_FUNCTION(writev)
{
	zval **fd, **iovec_id;
	php_iovec_t *vector;
	int ret;
	SOCKETSLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &fd, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", SOCKETSG(le_iov));

	ret = writev(Z_LVAL_PP(fd), vector->iov_array, vector->count);
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int recv(int fd, string buf, int len, int flags)
   Receives data from a connected socket */
/* May be used with SOCK_DGRAM sockets. */
PHP_FUNCTION(recv)
{
	zval **fd, **buf, **len, **flags;
	int ret;
	char *recv_buf;

	if (ZEND_NUM_ARGS() != 4 || 
	    zend_get_parameters_ex(4, &fd, &buf, &len, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, fd, len, flags);
	convert_to_string_ex(buf);

	recv_buf = emalloc(Z_LVAL_PP(len) + 2);
	memset(recv_buf, 0, Z_LVAL_PP(len) + 2);

	ret = recv(Z_LVAL_PP(fd), recv_buf, Z_LVAL_PP(len), Z_LVAL_PP(flags));
	
	if (ret < 0) {
		efree(recv_buf);
		RETURN_LONG(-errno);
	} else {
		if (Z_STRLEN_PP(buf) > 0) {
			efree(Z_STRVAL_PP(buf));
		}
		Z_STRVAL_PP(buf) = estrndup(recv_buf, strlen(recv_buf));
		Z_STRLEN_PP(buf) = strlen(recv_buf);

		efree(recv_buf);

		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int send(int fd, string buf, int len, int flags)
   Sends data to a connected socket */
/* May be used with SOCK_DGRAM sockets. */
PHP_FUNCTION(send)
{
	zval **fd, **buf, **len, **flags;
	int ret;

	if (ZEND_NUM_ARGS() != 4 || 
	    zend_get_parameters_ex(4, &fd, &buf, &len, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, fd, len, flags);
	convert_to_string_ex(buf);

	ret = send(Z_LVAL_PP(fd), Z_STRVAL_PP(buf),
		   (Z_STRLEN_PP(buf) < Z_LVAL_PP(len) ? Z_STRLEN_PP(buf) : Z_LVAL_PP(len)),
		   Z_LVAL_PP(flags));
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int recvfrom(int fd, string &buf, int len, int flags, string &name [, int &port])
   Receives data from a socket, connected or not */
PHP_FUNCTION(recvfrom)
{
	zval **fd, **buf, **len, **flags, **name, **port;
	int ret;
	struct sockaddr sa;
	socklen_t salen;

	switch (ZEND_NUM_ARGS()) 
	{
		case 5:
			if (zend_get_parameters_ex(5, &fd, &buf, &len, &flags, &name) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 6:
			if (zend_get_parameters_ex(6, &fd, &buf, &len, &flags, &name, &port) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	salen = sizeof(sa);

	ret = getsockname(Z_LVAL_PP(fd), &sa, &salen);
	
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	switch (sa.sa_family)
	{
		case AF_UNIX:
			{
				struct sockaddr_un sun;
				char *recv_buf = emalloc(Z_LVAL_PP(len) + 2);
				socklen_t sun_length = sizeof(sun);
	
				if (ZEND_NUM_ARGS() != 5) {
					WRONG_PARAM_COUNT;
				}
				memset(recv_buf, 0, Z_LVAL_PP(len) + 2);

				ret = recvfrom(Z_LVAL_PP(fd), recv_buf, Z_LVAL_PP(len), Z_LVAL_PP(flags),
				               (struct sockaddr *)&sun, (socklen_t *) & sun_length);

				if (ret < 0) {
					efree(recv_buf);
					RETURN_LONG(-errno);
				}
				
				if (Z_STRVAL_PP(buf) != NULL) {
					efree(Z_STRVAL_PP(buf));
				}
				
				Z_STRVAL_PP(buf) = estrndup(recv_buf, strlen(recv_buf));
				Z_STRLEN_PP(buf) = strlen(recv_buf);

				if (Z_STRVAL_PP(name) != NULL) {
					efree(Z_STRVAL_PP(name));
				}
				
				Z_STRVAL_PP(name) = estrdup(sun.sun_path);
				Z_STRLEN_PP(name) = strlen(sun.sun_path);

				efree(recv_buf);

				RETURN_LONG(ret);
			}
			break;
		case AF_INET:
			{
				struct sockaddr_in sin;
				char *recv_buf = emalloc(Z_LVAL_PP(len) + 2);
				socklen_t sin_length = sizeof(sin);
				char *address;

				if (ZEND_NUM_ARGS() != 6) {
					WRONG_PARAM_COUNT;
				}
				memset(recv_buf, 0, Z_LVAL_PP(len) + 2);

				ret = recvfrom(Z_LVAL_PP(fd), recv_buf, Z_LVAL_PP(len), Z_LVAL_PP(flags),
				               (struct sockaddr *)&sin, (socklen_t *) & sin_length);
	
				if (ret < 0) {
					efree(recv_buf);
					RETURN_LONG(-errno);
				}
				
				if (Z_STRVAL_PP(buf) != NULL) {
					efree(Z_STRVAL_PP(buf));
				}
				
				if (Z_STRVAL_PP(name) != NULL) {
					efree(Z_STRVAL_PP(name));
				}
				
				Z_STRVAL_PP(buf) = estrdup(recv_buf);
				Z_STRLEN_PP(buf) = strlen(recv_buf);

				address = inet_ntoa(sin.sin_addr);
				if (address == NULL) {
					Z_STRVAL_PP(name) = estrdup("0.0.0.0");
					Z_STRLEN_PP(name) = strlen(Z_STRVAL_PP(name));
				} else {
					Z_STRVAL_PP(name) = estrdup(address);
					Z_STRLEN_PP(name) = strlen(address);
				}
				
				Z_LVAL_PP(port) = ntohs(sin.sin_port);;
	
				efree(recv_buf);
				RETURN_LONG(ret);
			}
			break;
		default:
			RETURN_LONG(-EPROTONOSUPPORT);
	}
}
/* }}} */

/* {{{ proto int sendto(int fd, string buf, int len, int flags, string addr [, int port])
   Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(sendto)
{
	zval **fd, **buf, **len, **flags, **addr, **port;
	struct sockaddr sa;
	socklen_t salen = sizeof(sa);
	int ret;

	switch (ZEND_NUM_ARGS())
	{
		case 5:
			ret = zend_get_parameters_ex(5, &fd, &buf, &len, &flags, &addr);
			break;
		case 6:
			ret = zend_get_parameters_ex(6, &fd, &buf, &len, &flags, &addr, &port);
			break;
		default:
			ret = FAILURE;
	}

	if (ret == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, fd, len, flags);
	convert_to_string_ex(buf);

	if (ZEND_NUM_ARGS() == 6) {
		convert_to_long_ex(port);
	}

	ret = getsockname(Z_LVAL_PP(fd), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	switch (sa.sa_family)
	{
		case AF_UNIX:
			{
				struct sockaddr_un sun;
	
				if (ZEND_NUM_ARGS() != 5) {
					WRONG_PARAM_COUNT;
				}
				memset(&sun, 0, sizeof(sun));
				sun.sun_family = AF_UNIX;
				snprintf(sun.sun_path, 108, "%s", Z_STRVAL_PP(port));
				ret = sendto(Z_LVAL_PP(fd), Z_STRVAL_PP(buf),
				             (Z_STRLEN_PP(buf) > Z_LVAL_PP(len) ? Z_LVAL_PP(len) : Z_STRLEN_PP(buf)),
				             Z_LVAL_PP(flags), (struct sockaddr *) &sun, SUN_LEN(&sun));

				RETURN_LONG(((ret < 0) ? -errno : ret));
			}
			break;
		case AF_INET:
			{
				struct sockaddr_in sin;
				struct in_addr addr_buf;

				if (ZEND_NUM_ARGS() != 6) {
					WRONG_PARAM_COUNT;
				}
				memset(&sin, 0, sizeof(sin));
				sin.sin_family = AF_INET;
	
				if (inet_aton(Z_STRVAL_PP(addr), &addr_buf) == 0) {
					sin.sin_addr.s_addr = addr_buf.s_addr;
				} else {
					struct hostent *he;

					he = gethostbyname(Z_STRVAL_PP(addr));
					if (he == NULL) {
						RETURN_LONG(-(h_errno) - 10000);
					}
					sin.sin_addr.s_addr = *(int *) (he->h_addr_list[0]);
				}
				sin.sin_port = htons(Z_LVAL_PP(port));
				ret = sendto(Z_LVAL_PP(fd), Z_STRVAL_PP(buf),
				             (Z_STRLEN_PP(buf) > Z_LVAL_PP(len) ? Z_LVAL_PP(len) : Z_STRLEN_PP(buf)),
				             Z_LVAL_PP(flags), (struct sockaddr *) &sin, sizeof(sin));
				
				RETURN_LONG(((ret < 0) ? -errno : ret));
			}
			break;
		default:
			RETURN_LONG(-EPROTONOSUPPORT);
			break;
	}
}
/* }}} */

/* {{{ proto int recvmsg(int fd, resource iovec, array &control, int &controllen, int &flags, string &addr [, int &port])
   Used to receive messages on a socket, whether connection-oriented or not */
PHP_FUNCTION(recvmsg)
{
	zval **fd, **iovec, **control, **controllen, **flags, **addr, **port;
	zval *control_array = NULL;
	php_iovec_t *iov;
	struct msghdr hdr;
	struct sockaddr sa;
	struct sockaddr_in *sin = (struct sockaddr_in *) &sa;
	struct sockaddr_un *sun = (struct sockaddr_un *) &sa;
	struct cmsghdr *ctl_buf;
	socklen_t salen = sizeof(sa);
	int ret;
	SOCKETSLS_FETCH();

	switch (ZEND_NUM_ARGS())
	{
		case 6:
			ret = zend_get_parameters_ex(6, &fd, &iovec, &control, &controllen, &flags, &addr);
			break;
		case 7:
			ret = zend_get_parameters_ex(7, &fd, &iovec, &control, &controllen, &flags, &addr, &port);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (ret == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, fd, controllen, flags);
	convert_to_string_ex(control);
	convert_to_string_ex(addr);
	if (ZEND_NUM_ARGS() == 7) {
		convert_to_long_ex(port);
	}

	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, iovec, -1, "IO vector table", SOCKETSG(le_iov));

	ret = getsockname(Z_LVAL_PP(fd), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	
	if (Z_LVAL_PP(controllen) > sizeof(struct cmsghdr)) {
		ctl_buf = emalloc(Z_LVAL_PP(controllen));
	} else {
		ctl_buf = NULL;
	}

	switch (sa.sa_family)
	{
		case AF_INET:
			{
				if (ZEND_NUM_ARGS() != 7) {
					efree(ctl_buf);
					WRONG_PARAM_COUNT;
				}
				memset(&sa, 0, sizeof(sa));
				hdr.msg_name = sin;
				hdr.msg_namelen = sizeof(sa);
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;

				if (ctl_buf) {
					hdr.msg_control = ctl_buf;
					hdr.msg_controllen = Z_LVAL_PP(controllen);
				} else {
					hdr.msg_control = NULL;
					hdr.msg_controllen = 0;
				}

				hdr.msg_flags = 0;
	
				ret = recvmsg(Z_LVAL_PP(fd), &hdr, Z_LVAL_PP(flags));
				if (ret < 0) {
					RETURN_LONG(-errno);
				} else {
					struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;

					/* copy values as appropriate... */
					if (array_init(control_array) == FAILURE) {
						php_error(E_WARNING, "Cannot intialize array");
						RETURN_FALSE;
					}
					add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
					add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
					add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
					*control = control_array;
					zval_copy_ctor(*control);
					
					Z_LVAL_PP(controllen) = hdr.msg_controllen;
					Z_LVAL_PP(flags)      = hdr.msg_flags;
					if (Z_STRVAL_PP(addr) != NULL) {
						efree(Z_STRVAL_PP(addr));
					} 
					{
						char *tmp = inet_ntoa(sin->sin_addr);
	
						if (tmp == NULL) {
							Z_STRVAL_PP(addr) = estrdup("0.0.0.0");
						} else {
							Z_STRVAL_PP(addr) = estrdup(tmp);
						}
					}
					Z_STRLEN_PP(addr) = strlen(Z_STRVAL_PP(addr));
					Z_LVAL_PP(port)   = ntohs(sin->sin_port);
					RETURN_LONG(ret);
				}
			}
			break;
	case AF_UNIX:
		{
			if (ZEND_NUM_ARGS() != 6) {
				efree(ctl_buf);
				WRONG_PARAM_COUNT;
			}
			memset(&sa, 0, sizeof(sa));
			hdr.msg_name = sun;
			hdr.msg_namelen = sizeof(sa);
			hdr.msg_iov = iov->iov_array;
			hdr.msg_iovlen = iov->count;

			if (ctl_buf) {
				hdr.msg_control = ctl_buf;
				hdr.msg_controllen = Z_LVAL_PP(controllen);
			} else {
				hdr.msg_control = NULL;
				hdr.msg_controllen = 0;
			}

			hdr.msg_flags = 0;

			ret = recvmsg(Z_LVAL_PP(fd), &hdr, Z_LVAL_PP(flags));
			if (ret < 0) {
				RETURN_LONG(-errno);
			} else {
				struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;

				/* copy values as appropriate... */
				if (array_init(control_array) == FAILURE) {
					php_error(E_WARNING, "Cannot initialize return value from recvmsg()");
					RETURN_FALSE;
				}
				add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
				*control = control_array;
				Z_LVAL_PP(controllen) = hdr.msg_controllen;
				Z_LVAL_PP(flags)      = hdr.msg_flags;
				if (Z_STRVAL_PP(addr) != NULL) {
					efree(Z_STRVAL_PP(addr));
				}
				Z_STRVAL_PP(addr) = estrdup(sun->sun_path);
				RETURN_LONG(ret);
			}
		}
		break;
	default:
		RETURN_LONG(-EPROTONOSUPPORT);
	}
}
/* }}} */

/* {{{ proto int sendmsg(int fd, resource iovec, int flags, string addr [, int port])
   Sends a message to a socket, regardless of whether it is connection-oriented or not */
PHP_FUNCTION(sendmsg)
{
	zval **fd, **iovec, **flags, **addr, **port;
	php_iovec_t *iov;
	int ret, argc = ZEND_NUM_ARGS();
	struct sockaddr sa;
	int salen;
	SOCKETSLS_FETCH();

	if (argc < 4 || argc > 5 ||
	    zend_get_parameters_ex(argc, &fd, &iovec, &flags, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	salen = sizeof(sa);
	ret = getsockname(Z_LVAL_PP(fd), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, iovec, -1, "IO vector table", SOCKETSG(le_iov));

	switch(sa.sa_family)
	{
		case AF_INET:
			{
				struct msghdr hdr;
				struct sockaddr_in *sin = (struct sockaddr_in *) &sa;
				h_errno = 0;
				errno = 0;
				memset(&hdr, 0, sizeof(hdr));
				hdr.msg_name = &sa;
				hdr.msg_namelen = sizeof(sa);
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;
				
				if (inet_aton(Z_STRVAL_PP(addr), &sin->sin_addr) != 0) {
					struct hostent *he = gethostbyname(Z_STRVAL_PP(addr));
					if (!he) {
						RETURN_LONG(h_errno > 0 ? -(h_errno) - 10000 : -errno)
					}
					sin->sin_addr.s_addr = *(int *)(he->h_addr_list[0]);
				}

				sin->sin_port = htons(Z_LVAL_PP(port));
				ret = sendmsg(Z_LVAL_PP(fd), &hdr, Z_LVAL_PP(flags));
				
				RETURN_LONG(((ret < 0) ? -errno : ret));
			}
			break;
		case AF_UNIX:
			{
				struct msghdr hdr;
				struct sockaddr_un *sun = (struct sockaddr_un *) &sa;
				errno = 0;
				hdr.msg_name = sun;
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;

				snprintf(sun->sun_path, 108, "%s", Z_STRVAL_PP(addr));

				hdr.msg_namelen = SUN_LEN(sun);

				ret = sendmsg(Z_LVAL_PP(fd), &hdr, Z_LVAL_PP(flags));
				
				RETURN_LONG(((ret < 0) ? -errno : ret));
			}
			break;
		default:
			RETURN_LONG(-EPROTONOSUPPORT);
	}
}
/* }}} */

/* {{{ proto int getsockopt(int fd, int level, int optname, array|int &optval)
   Gets socket options for the socket */
/* If optname is SO_LINGER, optval is returned as an array with members 
   "l_onoff" and "l_linger", otherwise it is an integer.
*/
PHP_FUNCTION(getsockopt)
{
	zval **fd, **level, **optname, **optval;
	struct linger linger_val;
	int other_val;
	int optlen;
	int ret;

	fd = level = optname = optval = NULL;

	if (ZEND_NUM_ARGS() != 4 || 
	    zend_get_parameters_ex(4, &fd, &level, &optname, &optval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, fd, level, optname);
	/* optname is set on the way out .. */

	if (Z_LVAL_PP(level) == SO_LINGER) {
		zval *optval_array = NULL;
		optlen = sizeof(struct linger);
		ret = getsockopt(Z_LVAL_PP(fd), Z_LVAL_PP(level), Z_LVAL_PP(optname), &linger_val, &optlen);

		if (ret < 0) {
			RETURN_LONG(-errno);
		}

		if (array_init(optval_array) == FAILURE) {
			php_error(E_WARNING, "Cannot initialize array from getsockopt()");
			RETURN_FALSE;
		}
		add_assoc_long(optval_array, "l_onoff", linger_val.l_onoff);
		add_assoc_long(optval_array, "l_linger", linger_val.l_linger);

		*optval = optval_array;
		RETURN_LONG(ret);
	} else {
		optlen = sizeof(other_val);
		ret = getsockopt(Z_LVAL_PP(fd), Z_LVAL_PP(level), Z_LVAL_PP(optname), &other_val, &optlen);
		if (ret < 0) {
			RETURN_LONG(-errno);
		}

		Z_LVAL_PP(optval) = other_val;
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int setsockopt(int fd, int level, int optname, int|array optval)
   Sets socket options for the socket */
/* If optname is SO_LINGER, optval is expected to be an array
   with members "l_onoff" and "l_linger", otherwise it should be an integer. */
PHP_FUNCTION(setsockopt)
{
	zval **fd, **level, **optname, **optval;
	int ret;
	struct linger lv;
	int ov;
	int optlen;

	errno = 0;

	if (ZEND_NUM_ARGS() != 4 || 
	    zend_get_parameters_ex(4, &fd, &level, &optname, &optval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, fd, level, optname);

	if (Z_LVAL_PP(optname) == SO_LINGER) {
		HashTable *ht;
		zval **l_onoff;
		zval **l_linger;

		convert_to_array_ex(optval);
		ht = HASH_OF(*optval);

		if (zend_hash_find(ht, "l_onoff", strlen("l_onoff") + 1, (void **)&l_onoff) == FAILURE) {
			php_error(E_WARNING, "No key \"l_onoff\" passed in optval");
			RETURN_FALSE;
		}
		if (zend_hash_find(ht, "l_linger", strlen("l_linger") + 1, (void **) &l_linger) == FAILURE) {
			php_error(E_WARNING, "No key \"l_linger\" passed in optval");
			RETURN_FALSE;
		}

		convert_to_long_ex(l_onoff);
		convert_to_long_ex(l_linger);

		lv.l_onoff = Z_LVAL_PP(l_onoff);
		lv.l_linger = Z_LVAL_PP(l_linger);

		optlen = sizeof(lv);
		ret = setsockopt(Z_LVAL_PP(fd), Z_LVAL_PP(level), Z_LVAL_PP(optname), &lv, optlen);
	} else {
		convert_to_long_ex(optval);

		optlen = sizeof(ov);
		ov = Z_LVAL_PP(optval);

		ret = setsockopt(Z_LVAL_PP(fd), Z_LVAL_PP(level), Z_LVAL_PP(optname), &ov, optlen);
	}

	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

/* {{{ proto int socketpair(int domain, int type, int protocol, array &fds)
   Creates a pair of indistinguishable sockets and stores them in fds. */
PHP_FUNCTION(socketpair)
{
	zval **domain, **type, **protocol, **fds, **fd;
	int ret, fds_ar[2];
	HashTable *fd_ar;
	
	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &domain, &type, &protocol, &fds) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, domain, type, protocol);

	if (Z_LVAL_PP(domain) != AF_INET && Z_LVAL_PP(domain) != AF_UNIX) {
		php_error(E_WARNING, "invalid socket domain specified -- assuming AF_INET");
		Z_LVAL_PP(domain) = AF_INET;
	}
	
	if (Z_LVAL_PP(type) > 10) {
		php_error(E_WARNING, "Invalid socket type specified - assuming SOCK_STREAM");
		Z_LVAL_PP(type) = SOCK_STREAM;
	}
	
	/* Initialize the array */
	if (array_init(*fds) == FAILURE) {
		php_error(E_WARNING, "Can't initialize fds array");
		RETURN_FALSE;
	}
	
	/* Get the sockets */
	ret = socketpair(Z_LVAL_PP(domain), Z_LVAL_PP(type), Z_LVAL_PP(protocol), fds_ar);
	
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	/* Put them into the array */
	add_index_long(*fds, 0, fds_ar[0]);
	add_index_long(*fds, 1, fds_ar[1]);

	/* ... and return */
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int shutdown(int fd, int how)
   Shuts down a socket for receiving, sending, or both. */

PHP_FUNCTION(shutdown)
{
	zval **fd, **how;
	int how_shutdown = 0, argc = ZEND_NUM_ARGS(), ret;
	
	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &fd, &how) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);
	
	if (argc > 1) {
		convert_to_long_ex(how);
		how_shutdown = Z_LVAL_PP(how);
	}
	
	ret = shutdown(Z_LVAL_PP(fd), how_shutdown);
	
	RETURN_LONG(((ret < 0) ? -errno : ret));
}
/* }}} */

#endif				/* HAVE_SOCKETS */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
