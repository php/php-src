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
   | Authors:                                                             |
   | Chris Vandomelen <chrisv@b0rked.dhs.org>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_ini.h"
#include "php_sockets.h"

#include "../standard/info.h"

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

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
 */
#if HAVE_SOCKETS

#ifdef ZTS
int sockets_globals_id;

#else
php_sockets_globals sockets_globals;

#endif

#define ZVAL(arg, type) ((*(arg))->value.type)

/* Perform convert_to_long_ex on a list of items */

void v_convert_to_long_ex(int items,...)
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
/* *INDENT-OFF* */
static unsigned char second_and_third_args_force_ref[] =
{3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char second_fifth_and_sixth_args_force_ref[] =
{6, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

static unsigned char fourth_arg_force_ref[] =
{4, BYREF_NONE, BYREF_NONE, BYREF_NONE, BYREF_FORCE};

static unsigned char third_through_seventh_args_force_ref[] =
{7, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE, BYREF_FORCE};

/* Every user visible function must have an entry in sockets_functions[].
 */
function_entry sockets_functions[] =
{
	PHP_FE(confirm_sockets_compiled,	NULL)	/* For testing, remove later. */
	PHP_FE(fd_alloc, 			NULL)	/* OK */
	PHP_FE(fd_dealloc, 			NULL)	/* OK */
	PHP_FE(fd_set, 				NULL)	/* OK */
	PHP_FE(fd_isset, 			NULL)	/* OK */
	PHP_FE(fd_clear, 			NULL)	/* OK */
	PHP_FE(fd_zero, 			NULL)	/* OK */
	PHP_FE(select, 				NULL)	/* OK */
	PHP_FE(open_listen_sok, 		NULL)	/* OK */
	PHP_FE(accept_connect, 			NULL)	/* OK */
	PHP_FE(set_nonblock, 			NULL)	/* OK */
	PHP_FE(listen, 				NULL)	/* OK */
	PHP_FE(close, 				NULL)	/* OK */
	PHP_FE(write, 				NULL)	/* OK */
	PHP_FE(read, 				second_arg_force_ref)	/* OK */
#if 0
/* If and when asynchronous context switching is avaliable, this will work. Otherwise, forget it. */
	PHP_FE(signal, 				NULL)
#endif
#if 0
/* These are defined elsewhere.. would these be more appropriate? */
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
	PHP_FE(delete_iovec,			NULL)
	PHP_FE(set_iovec,			NULL)
	PHP_FE(recvmsg,				third_through_seventh_args_force_ref)
	PHP_FE(sendmsg,				NULL)
	PHP_FE(readv,				NULL)
	PHP_FE(writev,				NULL)
	PHP_FE(getsockopt,			fourth_arg_force_ref)
	PHP_FE(setsockopt,			NULL)
	{NULL, NULL, NULL}	/* Must be the last line in sockets_functions[] */
};

/* *INDENT-ON* */




zend_module_entry sockets_module_entry =
{
	"sockets",
	sockets_functions,
	PHP_MINIT(sockets),
	PHP_MSHUTDOWN(sockets),
	NULL,
	NULL,
	PHP_MINFO(sockets),
	STANDARD_MODULE_PROPERTIES
};

int le_destroy;
int le_iov;

#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

/* Remove comments and fill if you need to have entries in php.ini
   PHP_INI_BEGIN()
   PHP_INI_END()
 */

typedef struct php_iovec {
	struct iovec *iov_array;
	unsigned int count;
} php_iovec_t;

static void destroy_fd_sets(fd_set * set)
{
	efree(set);
}

static void destroy_iovec(php_iovec_t * iov)
{
	int i;

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
/* Remove comments if you have entries in php.ini
   REGISTER_INI_ENTRIES();
 */
	le_destroy = register_list_destructors(destroy_fd_sets, NULL);
	le_iov = register_list_destructors(destroy_iovec, NULL);

	REGISTER_LONG_CONSTANT("AF_UNIX", 		AF_UNIX, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AF_INET", 		AF_INET, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_STREAM", 		SOCK_STREAM, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_DGRAM", 		SOCK_DGRAM, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RAW", 		SOCK_RAW, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_SEQPACKET", 	SOCK_SEQPACKET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RDM", 		SOCK_RDM, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_OOB", 		MSG_OOB, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_WAITALL", 		MSG_WAITALL, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_PEEK", 		MSG_PEEK, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_DONTROUTE",		MSG_DONTROUTE,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DEBUG", 		SO_DEBUG, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_REUSEADDR", 		SO_REUSEADDR, 	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_KEEPALIVE",		SO_KEEPALIVE,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DONTROUTE",		SO_DONTROUTE,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LINGER",		SO_LINGER,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_BROADCAST",		SO_BROADCAST,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_OOBINLINE",		SO_OOBINLINE,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDBUF",		SO_SNDBUF,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVBUF",		SO_RCVBUF,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDLOWAT",		SO_SNDLOWAT,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVLOWAT",		SO_RCVLOWAT,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDTIMEO",		SO_SNDTIMEO,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVTIMEO",		SO_RCVTIMEO,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_TYPE",		SO_TYPE,	CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_ERROR",		SO_ERROR,	CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SOL_SOCKET",		SOL_SOCKET,	CONST_PERSISTENT);
	{
		struct protoent *pe;
		pe = getprotobyname("tcp");
		if (pe)
			REGISTER_LONG_CONSTANT("SOL_TCP",	pe->p_proto,	CONST_PERSISTENT);
		pe = getprotobyname("udp");
		if (pe)
			REGISTER_LONG_CONSTANT("SOL_UDP",	pe->p_proto,	CONST_PERSISTENT);
	}
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(sockets)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(sockets)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "sockets support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	   DISPLAY_INI_ENTRIES();
	 */
}

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_sockets_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_sockets_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/sockets/config.m4, module %s is compiled in PHP", Z_STRVAL_PP(arg));
	RETVAL_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
 */

/* {{{ proto resource fd_alloc(void)
   Allocate a file descriptor set */
PHP_FUNCTION(fd_alloc)
{
	fd_set *set;
	int ret;

	set = emalloc(sizeof(fd_set));
	if (!set) {
		zend_error(E_ERROR, "Can't allocate memory for fd_set");
		RETVAL_FALSE;
	}
	ret = ZEND_REGISTER_RESOURCE(return_value, set, le_destroy);
}
/* }}} */

/* {{{ proto void fd_dealloc(void)
   De-allocate a file descriptor set */
/*   ** BUG: This is currently a no-op! */
PHP_FUNCTION(fd_dealloc)
{
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool fd_set(long fd, resource set)
   Add a file descriptor to a set */
PHP_FUNCTION(fd_set)
{
	zval **set, **fd;
	fd_set *the_set;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, set, fd);

	if ((*set)->type != IS_RESOURCE) {
		(*set)->type = IS_RESOURCE;
	}
	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", le_destroy);

	FD_SET((*fd)->value.lval, the_set);

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool fd_clear(long fd, resource set)
   Clear a file descriptor from a set */
PHP_FUNCTION(fd_clear)
{
	zval **set, **fd;
	fd_set *the_set;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, set, fd);

	(*set)->type = IS_RESOURCE;

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", le_destroy);

	FD_CLR((*fd)->value.lval, the_set);

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool fd_isset(long fd, resource set)
   Check to see if a file descriptor is set within the file descrirptor set */
PHP_FUNCTION(fd_isset)
{
	zval **set, **fd;
	fd_set *the_set;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, set, fd);

	(*set)->type = IS_RESOURCE;

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", le_destroy);

	if (FD_ISSET((*fd)->value.lval, the_set)) {
		RETVAL_TRUE;
	}
	RETVAL_FALSE;
}
/* }}} */

/* {{{ proto void fd_zero(resource set)
   Clear a file descriptor set */
PHP_FUNCTION(fd_zero)
{
	zval **set;
	fd_set *the_set;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(2, &set) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(set);

	(*set)->type = IS_RESOURCE;

	ZEND_FETCH_RESOURCE(the_set, fd_set *, set, -1, "File descriptor set", le_destroy);

	FD_ZERO(the_set);

	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto void select(long max_fd, resource readfds, resource writefds, resource exceptfds, long tv_sec, long tv_usec)
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec. */
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
	zval **max_fd, **readfds, **writefds, **exceptfds, **tv_sec, 
             **tv_usec;
	struct timeval tv;
	fd_set *rfds, *wfds, *xfds;
	int ret = 0;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &max_fd, &readfds, &writefds, &exceptfds,
		 			   &tv_sec, &tv_usec) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(6, max_fd, readfds, writefds, exceptfds, tv_sec, tv_usec);

	tv.tv_sec = ZVAL(tv_sec, lval);
	tv.tv_usec = ZVAL(tv_usec, lval);

	(*readfds)->type = IS_RESOURCE;
	(*writefds)->type = IS_RESOURCE;
	(*exceptfds)->type = IS_RESOURCE;

	if (ZVAL(readfds, lval) == 0) {
		rfds = NULL;
	} else {
		ZEND_FETCH_RESOURCE(rfds, fd_set *, readfds, -1, "File descriptor set", le_destroy);
	}

	if (ZVAL(writefds, lval) == 0) {
		wfds = NULL;
	} else {
		ZEND_FETCH_RESOURCE(wfds, fd_set *, writefds, -1, "File descriptor set", le_destroy);
	}

	if (ZVAL(exceptfds, lval) == 0) {
		xfds = NULL;
	} else {
		ZEND_FETCH_RESOURCE(xfds, fd_set *, exceptfds, -1, "File descriptor set", le_destroy);
	}

	ret = select(ZVAL(max_fd, lval), rfds, wfds, xfds, &tv);

	if (ret < 0) {
		RETVAL_LONG(-errno);
	} else {
		RETVAL_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long open_listen_sok(long port)
   Opens a socket on port to accept connections */

int open_listen_sok(int port)
{
	int fd;
	struct sockaddr_in la;
	struct hostent *hp;

	if ((hp = gethostbyname("0.0.0.0")) == NULL) {
		return -1;
	}
	bcopy(hp->h_addr, (char *) &la.sin_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons(port);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	if ((bind(fd, (struct sockaddr *) &la, sizeof(la)) < 0)) {
		return -1;
	}
	listen(fd, 128);
	return fd;
}

PHP_FUNCTION(open_listen_sok)
{
	zval **port;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(port);

	ret = open_listen_sok(ZVAL(port, lval));

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int accept_connect(long fd)
   accepts a connection on the listening socket fd */
int accept_connect(int fd, struct sockaddr *la)
{
	int foo, m;

	m = sizeof(*la);
	if ((foo = accept(fd, la, &m)) < 0) {
		return -1;
	}
	return foo;
}

PHP_FUNCTION(accept_connect)
{
	zval **fd;
	int ret;
	struct sockaddr_in sa;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ret = accept_connect(ZVAL(fd, lval), (struct sockaddr *) &sa);

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto bool set_nonblock(long fd)
   sets nonblocking mode for file descriptor fd */
PHP_FUNCTION(set_nonblock)
{
	zval **fd;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(fd);

	ret = fcntl(ZVAL(fd, lval), F_SETFL, O_NONBLOCK);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto bool listen(long fd, long backlog)
   sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(listen)
{
	zval **fd, **backlog;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &backlog) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, backlog);

	ret = listen(ZVAL(fd, lval), ZVAL(backlog, lval));

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto bool close(long fd)
   Close a file descriptor */
PHP_FUNCTION(close)
{
	zval **arg;
	int ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg);
	ret = close(ZVAL(arg, lval));

	if (ret == -1) {
		zend_error(E_WARNING, "Invalid file descriptor");
		RETVAL_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto long write(long fd, string buf, long length)
   Writes length bytes of buf to the file descriptor fd */
PHP_FUNCTION(write)
{
	zval **fd, **buf, **length;
	int ret;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &fd, &buf, &length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, length);
	convert_to_string_ex(buf);

	if (ZVAL(buf, str.len) < ZVAL(length, lval)) {
		ret = write(ZVAL(fd, lval), (void *) ZVAL(buf, str.val), ZVAL(buf, str.len));
	} else {
		ret = write(ZVAL(fd, lval), (void *) ZVAL(buf, str.val), ZVAL(length, lval));
	}

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long read(long fd, string &buf, long length)
   Reads length bytes from fd into buf */
PHP_FUNCTION(read)
{
	zval **fd, **buf, **length;
	char *tmp;
	int ret;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &fd, &buf, &length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, length);
	convert_to_string_ex(buf);

	tmp = emalloc(ZVAL(length, lval));
	ret = read(ZVAL(fd, lval), tmp, ZVAL(length, lval));

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		ZVAL(buf, str.val) = tmp;
		ZVAL(buf, str.len) = ret;

		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long getsockname(long fd, string &addr, long &port)
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the local side of a socket. */

/* A lock to prevent inet_ntoa() from causing problems in threading */
volatile int inet_ntoa_lock = 0;

PHP_FUNCTION(getsockname)
{
	zval **fd, **addr, **port;
	char *tmp;
	struct sockaddr_in sa;
	int salen = sizeof(struct sockaddr_in);
	int ret;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &fd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, port);
	convert_to_string_ex(addr);

	ret = getsockname(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		char *addr_string;

		while (inet_ntoa_lock == 1);
		inet_ntoa_lock = 1;
		addr_string = inet_ntoa(sa.sin_addr);
		tmp = emalloc(strlen(addr_string) + 1);
		bzero(tmp, strlen(addr_string) + 1);
		strncpy(tmp, addr_string, strlen(addr_string));
		inet_ntoa_lock = 0;

		ZVAL(addr, str.val) = tmp;
		ZVAL(addr, str.len) = strlen(tmp);
		ZVAL(port, lval) = htons(sa.sin_port);

		RETURN_LONG(ret);
	}
}
/* }}} */

#if 0
/* {{{ proto long gethostbyname(string name, string &addr)
   Given a hostname, sets addr to be a human-readable version of the host's address */

/* Another lock to prevent multiple threads from grabbing gethostbyname() */
volatile int gethostbyname_lock = 0;

PHP_FUNCTION(gethostbyname)
{
	zval **name, **addr;
	int ret;
	char *tmp, *addr_string;
	struct hostent *host_struct;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &name, &addr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(addr);

	while (gethostbyname_lock == 1);
	gethostbyname_lock = 1;

	host_struct = gethostbyname(ZVAL(name, str.val));
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

	addr_string = inet_ntoa((struct in_addr) host_struct->h_addr);
	tmp = emalloc(strlen(addr_string) + 1);
	strncpy(tmp, addr_string, strlen(addr_string));

	inet_ntoa_lock = 0;
	gethostbyname_lock = 0;

	ZVAL(addr, str.val) = tmp;
	ZVAL(addr, str.len) = strlen(tmp);

	RETURN_LONG(0);
}

/* }}} */

#endif

/* {{{ proto long getpeername(long fd, string &addr, long &port)
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port into addr and port describing the remote side of a socket. */

PHP_FUNCTION(getpeername)
{
	zval **fd, **addr, **port;
	char *tmp;
	struct sockaddr_in sa;
	int salen = sizeof(struct sockaddr_in);
	int ret;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &fd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, fd, port);
	convert_to_string_ex(addr);

	ret = getpeername(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		char *addr_string;

		while (inet_ntoa_lock == 1);
		inet_ntoa_lock = 1;
		addr_string = inet_ntoa(sa.sin_addr);
		tmp = emalloc(strlen(addr_string) + 1);
		bzero(tmp, strlen(addr_string) + 1);
		strncpy(tmp, addr_string, strlen(addr_string));
		inet_ntoa_lock = 0;

		ZVAL(addr, str.val) = tmp;
		ZVAL(addr, str.len) = strlen(tmp);
		ZVAL(port, lval) = htons(sa.sin_port);

		RETURN_LONG(ret);
	}
}
/* }}} */

#if 0
/* {{{ proto long gethostbyaddr(string addr, string &name)
   Given a human-readable address, sets name to be the host's name */

/* Another lock to prevent multiple threads from grabbing gethostbyname() */
volatile int gethostbyaddr_lock = 0;

PHP_FUNCTION(gethostbyaddr)
{
	zval **name, **addr;
	int ret;
	char *tmp, *addr_string;
	struct hostent *host_struct;
	struct in_addr addr_buf;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &addr, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(name);
	convert_to_string_ex(addr);

	ret = inet_aton(ZVAL(addr, str.val), &addr_buf);
	if (ret < 0) {
		RETURN_LONG(-EINVAL);
	}
	while (gethostbyaddr_lock == 1);
	gethostbyaddr_lock = 1;

	host_struct = gethostbyname((char *) &addr_buf, sizeof(addr_buf), AF_INET);
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

	ZVAL(addr, str.val) = tmp;
	ZVAL(addr, str.len) = strlen(tmp);

	RETURN_LONG(0);
}
/* }}} */

#endif

/* {{{ proto long socket(long domain, long type, long protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket)
{
	zval **domain, **type, **protocol;
	int ret;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &domain, &type, &protocol) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, domain, type, protocol);

	if (ZVAL(domain, lval) != AF_INET && ZVAL(domain, lval) != AF_UNIX) {
		zend_error(E_WARNING, "invalid socket domain specified - assuming AF_INET");
		ZVAL(domain, lval) = AF_INET;
	}
	if (ZVAL(type, lval) > 10) {
		zend_error(E_WARNING, "invalid socket type specified - assuming SOCK_STREAM");
		ZVAL(type, lval) = SOCK_STREAM;
	}
	ret = socket(ZVAL(domain, lval), ZVAL(type, lval), ZVAL(protocol, lval));
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long connect(long sockfd, string addr, long port)
   Opens a connection to addr:port on the socket specified by sockfd */
PHP_FUNCTION(connect)
{
	zval **sockfd, **addr, **port;
	struct sockaddr_in sa;
	int salen = sizeof(sa);
	int ret;
	struct in_addr addr_buf;
	struct hostent *host_struct;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &sockfd, &addr, &port) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(2, sockfd, port);
	convert_to_string_ex(addr);

	bzero(&sa, sizeof(sa));
	sa.sin_port = ZVAL(port, lval);

	if (inet_aton(ZVAL(addr, str.val), &addr_buf) == 0) {
		sa.sin_addr.s_addr = addr_buf.s_addr;
	} else {
		host_struct = gethostbyname(ZVAL(addr, str.val));
		if (host_struct->h_addrtype != AF_INET) {
			RETURN_LONG(-EINVAL);
		}
		sa.sin_addr.s_addr = (int) *(host_struct->h_addr_list[0]);
	}

	ret = connect(ZVAL(sockfd, lval), (struct sockaddr *) &sa, salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto string strerror(long errno)
   Returns a string describing an error */
PHP_FUNCTION(strerror)
{
	zval **error;
	const char *buf;
	char *obuf;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &error) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (ZVAL(error, lval) < -10000) {
		ZVAL(error, lval) += 10000;
		buf = hstrerror(-(ZVAL(error, lval)));
	} else {
		buf = strerror(-(ZVAL(error, lval)));
	}

	obuf = estrndup(buf, strlen(buf));
	ZVAL(&return_value, str.val) = obuf;
	ZVAL(&return_value, str.len) = strlen(buf);
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto long bind(long sockfd, long addr [, long port])
   Binds an open socket to a listening port. */
/* Port is only specified if sockfd is in the AF_INET family. */
PHP_FUNCTION(bind)
{
	zval **arg0, **arg1, **arg2;
	long ret;
	struct sockaddr sock_type;
	socklen_t length = sizeof(sock_type);

	if (ZEND_NUM_ARGS() < 2) {
		WRONG_PARAM_COUNT;
	}
	switch (ZEND_NUM_ARGS()) {
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

	ret = getsockname(ZVAL(arg0, lval), &sock_type, &length);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	if (sock_type.sa_family == AF_UNIX) {
		struct sockaddr_un sa;

		if (ZEND_NUM_ARGS() < 2) {
			WRONG_PARAM_COUNT;
		}
		snprintf(sa.sun_path, 108, "%s", ZVAL(arg1, str.val));
		ret = bind(ZVAL(arg0, lval), &sa, SUN_LEN(&sa));
	} else if (sock_type.sa_family == AF_INET) {
		struct sockaddr_in sa;
		struct in_addr addr_buf;

		if (ZEND_NUM_ARGS() != 3) {
			WRONG_PARAM_COUNT;
		}
		sa.sin_port = htons(ZVAL(arg2, lval));
		if (inet_aton(ZVAL(arg1, str.val), &addr_buf) < 0) {
			struct hostent *host_struct = gethostbyname(ZVAL(arg1, str.val));

			if (host_struct == NULL) {
				RETURN_LONG(-(h_errno) - 10000);
			}
			sa.sin_addr.s_addr = (int) *(host_struct->h_addr_list[0]);
		} else {
			sa.sin_addr.s_addr = addr_buf.s_addr;
		}
		ret = bind(ZVAL(arg0, lval), &sa, sizeof(sa));
	} else {
		RETURN_LONG(-EPROTONOSUPPORT);
	}

	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto resource build_iovec(int num_vectors, ...)
   Build a 'struct iovec' for use with sendmsg, recvmsg, writev, and readv. */
/* First parameter is number of vectors, each additional parameter is the
   length of the vector to create.
 */

PHP_FUNCTION(build_iovec)
{
	zval **num_vectors, **vector_len;
	php_iovec_t *vector;
	struct iovec *vector_array;
	void **p;
	int arg_count;
	int i;

	ELS_FETCH();

	if (ZEND_NUM_ARGS() < 1) {
		WRONG_PARAM_COUNT;
	}
	p = EG(argument_stack).top_element - 2;
	arg_count = (ulong) * p;

	num_vectors = (zval **) p - (arg_count--);

	convert_to_long_ex(num_vectors);

	vector_array = emalloc(sizeof(struct iovec) * (ZVAL(num_vectors, lval) + 1));

	for (i = 0; i < ZVAL(num_vectors, lval); i++) {
		if (arg_count < 1) {
			WRONG_PARAM_COUNT;
		}
		vector_len = (zval **) p - (arg_count--);

		convert_to_long_ex(vector_len);

		vector_array[i].iov_base = emalloc(ZVAL(vector_len, lval));
		vector_array[i].iov_len = ZVAL(vector_len, lval);
	}

	vector = emalloc(sizeof(php_iovec_t));
	vector->iov_array = vector_array;
	vector->count = ZVAL(num_vectors, lval);

	ZEND_REGISTER_RESOURCE(return_value, vector, le_iov);
}
/* }}} */

/* {{{ proto string fetch_iovec(resource iovec_id, long iovec_position)
   Returns the data held in the iovec specified by iovec_id[iovec_position]. */
PHP_FUNCTION(fetch_iovec)
{
	zval **iovec_id, **iovec_position;
	php_iovec_t *vector;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iovec_position) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (ZVAL(iovec_position, lval) > vector->count) {
		zend_error(E_WARNING, "Can't access a vector position past the amount of vectors set in the array");
		RETURN_NULL();
	}
	RETURN_STRINGL(vector->iov_array[ZVAL(iovec_position, lval)].iov_base,
		       vector->iov_array[ZVAL(iovec_position, lval)].iov_len,
		       1);
}
/* }}} */

/* {{{ proto bool set_iovec(resource iovec_id, long iovec_position, string new_val)
   Sets the data held in iovec_id[iovec_position] to new_val. */
PHP_FUNCTION(set_iovec)
{
	zval **iovec_id, **iovec_position, **new_val;
	php_iovec_t *vector;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &iovec_id, &iovec_position, &new_val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (ZVAL(iovec_position, lval) > vector->count) {
		zend_error(E_WARNING, "Can't access a vector position outside of the vector array bounds");
		RETURN_FALSE;
	}
	if (vector->iov_array[ZVAL(iovec_position, lval)].iov_base) {
		efree(vector->iov_array[ZVAL(iovec_position, lval)].iov_base);
	}
	vector->iov_array[ZVAL(iovec_position, lval)].iov_base = estrdup(ZVAL(new_val, str.val));
	vector->iov_array[ZVAL(iovec_position, lval)].iov_len = strlen(ZVAL(new_val, str.val));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool add_iovec(resource iovec_id, long iov_len)
   Adds a new vector to the scatter/gather array */
PHP_FUNCTION(add_iovec)
{
	zval **iovec_id, **iov_len;
	php_iovec_t *vector;
	struct iovec *vector_array;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iov_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	vector_array = emalloc(sizeof(struct iovec) * (vector->count + 2));
	bcopy(vector->iov_array, vector_array, sizeof(struct iovec) * vector->count);

	vector_array[vector->count].iov_base = emalloc(ZVAL(iov_len, lval));
	vector_array[vector->count].iov_len = ZVAL(iov_len, lval);
	efree(vector->iov_array);
	vector->iov_array = vector_array;
	vector->count++;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool delete_iovec(resource iovec_id, long iov_pos)
   Deletes a vector from an array of vectors */
PHP_FUNCTION(delete_iovec)
{
	zval **iovec_id, **iov_pos;
	php_iovec_t *vector;
	struct iovec *vector_array;
	int i;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &iovec_id, &iov_pos) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	if (ZVAL(iov_pos, lval) > vector->count) {
		zend_error(E_WARNING, "Can't delete an IO vector that is out of array bounds");
		RETURN_FALSE;
	}
	vector_array = emalloc(vector->count * sizeof(struct iovec));

	for (i = 0; i < vector->count; i++) {
		if (i < ZVAL(iov_pos, lval)) {
			bcopy(&(vector_array[i]), &(vector->iov_array[i]), sizeof(struct iovec));
		} else if (i > ZVAL(iov_pos, lval)) {
			bcopy(&(vector_array[i - 1]), &(vector->iov_array[i]), sizeof(struct iovec));
		}
	}

	efree(vector->iov_array);
	vector->iov_array = vector_array;

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool free_iovec(resource iovec_id)
   Frees the iovec specified by iovec_id. */
PHP_FUNCTION(free_iovec)
{
	zval **iovec_id;
	php_iovec_t *vector;
	int pos;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	for (pos = 0; pos < vector->count; pos++) {
		efree(vector->iov_array[pos].iov_base);
	}

	efree(vector->iov_array);

	vector->iov_array = NULL;
	vector->count = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto long readv(long fd, resource iovec_id)
   Reads from an fd, using the scatter-gather array defined by iovec_id. */
PHP_FUNCTION(readv)
{
	zval **fd, **iovec_id;
	php_iovec_t *vector;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	ret = readv(ZVAL(fd, lval), vector->iov_array, vector->count);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long writev(long fd, resource iovec_id)
   Writes to a fd, using the scatter-gather array defined by iovec_id. */
PHP_FUNCTION(writev)
{
	zval **fd, **iovec_id;
	php_iovec_t *vector;
	int ret;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fd, &iovec_id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	(*iovec_id)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(vector, php_iovec_t *, iovec_id, -1, "IO vector table", le_iov);

	ret = writev(ZVAL(fd, lval), vector->iov_array, vector->count);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long recv(long fd, string buf, long len, long flags)
   Receive data from a connected socket. */
/* May be used with SOCK_DGRAM sockets. */
PHP_FUNCTION(recv)
{
	zval **fd, **buf, **len, **flags;
	int ret;
	char *recv_buf;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fd, &buf, &len, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, fd, len, flags);
	convert_to_string_ex(buf);

	recv_buf = emalloc(ZVAL(len, lval) + 2);
	bzero(recv_buf, ZVAL(len, lval) + 2);

	ret = recv(ZVAL(fd, lval), recv_buf, ZVAL(len, lval), ZVAL(flags, lval));
	if (ret < 0) {
		efree(recv_buf);
		RETURN_LONG(-errno);
	} else {
		if (ZVAL(buf, str.val) != NULL) {
			efree(ZVAL(buf, str.val));
		}
		ZVAL(buf, str.val) = estrndup(recv_buf, strlen(recv_buf));
		ZVAL(buf, str.len) = strlen(recv_buf);

		efree(recv_buf);

		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long send(long fd, string buf, long len, long flags)
   Send data to a connected socket. */
/* May be used with SOCK_DGRAM sockets. */
PHP_FUNCTION(send)
{
	zval **fd, **buf, **len, **flags;
	int ret;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fd, &buf, &len, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	v_convert_to_long_ex(3, fd, len, flags);
	convert_to_string_ex(buf);

	ret = send(ZVAL(fd, lval), ZVAL(buf, str.val),
		   (ZVAL(buf, str.len) < ZVAL(len, lval) ? ZVAL(buf, str.len) : ZVAL(len, lval)),
		   ZVAL(flags, lval));
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long recvfrom(long fd, string &buf, long len, long flags, string &name [, long &port])
   Receive data from a socket, connected or not. */
PHP_FUNCTION(recvfrom)
{
	zval **fd, **buf, **len, **flags, **name, **port;
	int ret;
	struct sockaddr sa;
	socklen_t salen;

	if (ZEND_NUM_ARGS() < 5) {
		WRONG_PARAM_COUNT;
	}
	switch (ZEND_NUM_ARGS()) {
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

	ret = getsockname(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	switch (sa.sa_family) {
	case AF_UNIX:
		{
			struct sockaddr_un sun;
			char *recv_buf = emalloc(ZVAL(len, lval) + 2);
			socklen_t sun_length = sizeof(sun);

			if (ZEND_NUM_ARGS() != 5) {
				WRONG_PARAM_COUNT;
			}
			bzero(recv_buf, ZVAL(len, lval) + 2);

			ret = recvfrom(ZVAL(fd, lval), recv_buf, ZVAL(len, lval), ZVAL(flags, lval),
				       (struct sockaddr *) &sun, (socklen_t *) & sun_length);

			if (ret < 0) {
				efree(recv_buf);
				RETURN_LONG(-errno);
			}
			if (ZVAL(buf, str.val) != NULL) {
				efree(ZVAL(buf, str.val));
			}
			ZVAL(buf, str.val) = estrndup(recv_buf, strlen(recv_buf));
			ZVAL(buf, str.len) = strlen(recv_buf);

			if (ZVAL(name, str.val) != NULL) {
				efree(ZVAL(name, str.val));
			}
			ZVAL(name, str.val) = estrdup(sun.sun_path);
			ZVAL(name, str.len) = strlen(sun.sun_path);

			efree(recv_buf);

			RETURN_LONG(ret);
		}
		break;
	case AF_INET:
		{
			struct sockaddr_in sin;
			char *recv_buf = emalloc(ZVAL(len, lval) + 2);
			socklen_t sin_length = sizeof(sin);
			char *address;

			if (ZEND_NUM_ARGS() != 6) {
				WRONG_PARAM_COUNT;
			}
			bzero(recv_buf, ZVAL(len, lval) + 2);

			ret = recvfrom(ZVAL(fd, lval), recv_buf, ZVAL(len, lval), ZVAL(flags, lval),
				       (struct sockaddr *) &sin, (socklen_t *) & sin_length);

			if (ret < 0) {
				efree(recv_buf);
				RETURN_LONG(-errno);
			}
			if (ZVAL(buf, str.val) != NULL) {
				efree(ZVAL(buf, str.val));
			}
			if (ZVAL(name, str.val) != NULL) {
				efree(ZVAL(name, str.val));
			}
			ZVAL(buf, str.val) = estrdup(recv_buf);
			ZVAL(buf, str.len) = strlen(recv_buf);

			address = inet_ntoa(sin.sin_addr);
			if (address == NULL) {
				ZVAL(name, str.val) = estrdup("0.0.0.0");
				ZVAL(name, str.len) = strlen(ZVAL(name, str.val));
			} else {
				ZVAL(name, str.val) = estrdup(address);
				ZVAL(name, str.len) = strlen(address);
			}

			ZVAL(port, lval) = ntohs(sin.sin_port);

			efree(recv_buf);
			RETURN_LONG(ret);
		}
		break;
	default:
		RETURN_LONG(-EPROTONOSUPPORT);

	}
}
/* }}} */

/* {{{ proto long sendto(long fd, string buf, long len, long flags, string addr [, long port])
   Sends a message to a socket, whether it is connected or not. */
PHP_FUNCTION(sendto)
{
	zval **fd, **buf, **len, **flags, **addr, **port;
	struct sockaddr sa;
	socklen_t salen = sizeof(sa);
	int ret;

	switch (ZEND_NUM_ARGS()) {
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
	ret = getsockname(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	switch (sa.sa_family) {
	case AF_UNIX:
		{
			struct sockaddr_un sun;

			if (ZEND_NUM_ARGS() != 5) {
				WRONG_PARAM_COUNT;
			}
			bzero(&sun, sizeof(sun));
			sun.sun_family = AF_UNIX;
			snprintf(sun.sun_path, 108, "%s", ZVAL(port, str.val));
			ret = sendto(ZVAL(fd, lval), ZVAL(buf, str.val),
				     (ZVAL(buf, str.len) > ZVAL(len, lval) ? ZVAL(len, lval) : ZVAL(buf, str.len)),
				     ZVAL(flags, lval), &sun, SUN_LEN(&sun));
			if (ret < 0) {
				RETURN_LONG(-errno);
			} else {
				RETURN_LONG(ret);
			}
		}
		break;
	case AF_INET:
		{
			struct sockaddr_in sin;
			struct in_addr addr_buf;

			if (ZEND_NUM_ARGS() != 6) {
				WRONG_PARAM_COUNT;
			}
			bzero(&sin, sizeof(sin));
			sin.sin_family = AF_INET;

			if (inet_aton(ZVAL(addr, str.val), &addr_buf) == 0) {
				sin.sin_addr.s_addr = addr_buf.s_addr;
			} else {
				struct hostent *he;

				he = gethostbyname(ZVAL(addr, str.val));
				if (he == NULL) {
					RETURN_LONG(-(h_errno) - 10000);
				}
				sin.sin_addr.s_addr = *(int *) (he->h_addr_list[0]);
			}
			sin.sin_port = htons(ZVAL(port, lval));
			ret = sendto(ZVAL(fd, lval), ZVAL(buf, str.val),
				     (ZVAL(buf, str.len) > ZVAL(len, lval) ? ZVAL(len, lval) : ZVAL(buf, str.len)),
				     ZVAL(flags, lval), &sin, sizeof(sin));
			if (ret < 0) {
				RETURN_LONG(-errno);
			} else {
				RETURN_LONG(ret);
			}
		}
		break;
	default:
		RETURN_LONG(-EPROTONOSUPPORT);
		break;
	}
}
/* }}} */

/* {{{ proto long recvmsg(long fd, resource iovec, array &control, long &controllen, long &flags, string &addr [, long &port])
   Used to receive messages on a socket, whether connection-oriented or not. */
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

	switch (ZEND_NUM_ARGS()) {
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
	v_convert_to_long_ex(4, fd, iovec, controllen, flags);
	convert_to_string_ex(control);
	convert_to_string_ex(addr);
	if (ZEND_NUM_ARGS() == 7) {
		convert_to_long_ex(port);
	}
	(*iovec)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, iovec, -1, "IO vector table", le_iov);

	ret = getsockname(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}
	if (ZVAL(controllen, lval) > sizeof(struct cmsghdr)) {
		ctl_buf = emalloc(ZVAL(controllen, lval));
	} else {
		ctl_buf = NULL;
	}

	switch (sa.sa_family) {
	case AF_INET:
		{
			if (ZEND_NUM_ARGS() != 7) {
				efree(ctl_buf);
				WRONG_PARAM_COUNT;
			}
			bzero(&sa, sizeof(sa));
			hdr.msg_name = sin;
			hdr.msg_namelen = sizeof(sa);
			hdr.msg_iov = iov->iov_array;
			hdr.msg_iovlen = iov->count;

			if (ctl_buf) {
				hdr.msg_control = ctl_buf;
				hdr.msg_controllen = ZVAL(controllen, lval);
			} else {
				hdr.msg_control = NULL;
				hdr.msg_controllen = 0;
			}

			hdr.msg_flags = 0;

			ret = recvmsg(ZVAL(fd, lval), &hdr, ZVAL(flags, lval));
			if (ret < 0) {
				RETURN_LONG(-errno);
			} else {
				struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;

				/* copy values as appropriate... */
				array_init(control_array);
				add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
				*control = control_array;
				ZVAL(controllen, lval) = hdr.msg_controllen;
				ZVAL(flags, lval) = hdr.msg_flags;
				if (ZVAL(addr, str.val) != NULL) {
					efree(ZVAL(addr, str.val));
				} {
					char *tmp = inet_ntoa(sin->sin_addr);

					if (tmp == NULL) {
						ZVAL(addr, str.val) = estrdup("0.0.0.0");
					} else {
						ZVAL(addr, str.val) = estrdup(tmp);
					}
				}
				ZVAL(addr, str.len) = strlen(ZVAL(addr, str.val));
				ZVAL(port, lval) = ntohs(sin->sin_port);
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
			bzero(&sa, sizeof(sa));
			hdr.msg_name = sun;
			hdr.msg_namelen = sizeof(sa);
			hdr.msg_iov = iov->iov_array;
			hdr.msg_iovlen = iov->count;

			if (ctl_buf) {
				hdr.msg_control = ctl_buf;
				hdr.msg_controllen = ZVAL(controllen, lval);
			} else {
				hdr.msg_control = NULL;
				hdr.msg_controllen = 0;
			}

			hdr.msg_flags = 0;

			ret = recvmsg(ZVAL(fd, lval), &hdr, ZVAL(flags, lval));
			if (ret < 0) {
				RETURN_LONG(-errno);
			} else {
				struct cmsghdr *mhdr = (struct cmsghdr *) hdr.msg_control;

				/* copy values as appropriate... */
				array_init(control_array);
				add_assoc_long(control_array, "cmsg_level", mhdr->cmsg_level);
				add_assoc_long(control_array, "cmsg_type", mhdr->cmsg_type);
				add_assoc_string(control_array, "cmsg_data", CMSG_DATA(mhdr), 1);
				*control = control_array;
				ZVAL(controllen, lval) = hdr.msg_controllen;
				ZVAL(flags, lval) = hdr.msg_flags;
				if (ZVAL(addr, str.val) != NULL) {
					efree(ZVAL(addr, str.val));
				}
				ZVAL(addr, str.val) = estrdup(sun->sun_path);
				RETURN_LONG(ret);
			}
		}
		break;
	default:
		RETURN_LONG(-EPROTONOSUPPORT);
	}
}
/* }}} */

/* {{{ proto long sendmsg(long fd, resource iovec, long flags, string addr [, long port])
   Sends a message to a socket, regardless of whether it is connection-oriented or not */
PHP_FUNCTION(sendmsg)
{
	zval **fd, **iovec, **flags, **addr, **port;
	php_iovec_t *iov;
	int ret;
	struct sockaddr sa;
	int salen;

	if (ZEND_NUM_ARGS() == 4) {
		ret = zend_get_parameters_ex(4, &fd, &iovec, &flags, &addr);
	} else if (ZEND_NUM_ARGS() == 5) {
		ret = zend_get_parameters_ex(5, &fd, &iovec, &flags, &addr, &port);
	}

	if (ret == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	salen = sizeof(sa);
	ret = getsockname(ZVAL(fd, lval), &sa, &salen);
	if (ret < 0) {
		RETURN_LONG(-errno);
	}

	(*iovec)->type = IS_RESOURCE;
	ZEND_FETCH_RESOURCE(iov, php_iovec_t *, iovec, -1, "IO vector table", le_iov);

	switch(sa.sa_family)
	{
		case AF_INET:
			{
				struct msghdr hdr;
				struct sockaddr_in *sin = (struct sockaddr_in *) &sa;
				h_errno = 0;
				errno = 0;
				bzero(&hdr, sizeof(hdr));
				hdr.msg_name = &sa;
				hdr.msg_namelen = sizeof(sa);
				hdr.msg_iov = iov->iov_array;
				hdr.msg_iovlen = iov->count;
				
				if (inet_aton(ZVAL(addr, str.val), &sin->sin_addr) != 0) {
					struct hostent *he = gethostbyname(ZVAL(addr, str.val));
					if (!he) {
						RETURN_LONG(h_errno > 0 ? -(h_errno) - 10000 : -errno)
					}
					sin->sin_addr.s_addr = *(int *)(he->h_addr_list[0]);
				}

				sin->sin_port = htons(ZVAL(port, lval));
				ret = sendmsg(ZVAL(fd, lval), &hdr, ZVAL(flags, lval));
				if (ret < 0) {
					RETURN_LONG(-errno);
				} else {
					RETURN_LONG(ret);
				}
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

				snprintf(sun->sun_path, 108, "%s", ZVAL(addr, str.val));

				hdr.msg_namelen = SUN_LEN(sun);

				ret = sendmsg(ZVAL(fd, lval), &hdr, ZVAL(flags, lval));
				if (ret < 0) {
					RETURN_LONG(-errno);
				} else {
					RETURN_LONG(ret);
				}
			}
			break;
		default:
			RETURN_LONG(-EPROTONOSUPPORT);
	}
}
/* }}} */

/* {{{ proto long getsockopt(long fd, long level, long optname, array/long &optval)
   Gets socket options for the socket. */
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

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fd, &level, &optname, &optval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, fd, level, optname);
	/* optname is set on the way out .. */

	if (ZVAL(level, lval) == SO_LINGER) {
		zval *optval_array = NULL;
		optlen = sizeof(struct linger);
		ret = getsockopt(ZVAL(fd, lval), ZVAL(level, lval), ZVAL(optname, lval), &linger_val, &optlen);

		if (ret < 0) {
			RETURN_LONG(-errno);
		}

		array_init(optval_array);
		add_assoc_long(optval_array, "l_onoff", linger_val.l_onoff);
		add_assoc_long(optval_array, "l_linger", linger_val.l_linger);

		*optval = optval_array;
		RETURN_LONG(ret);
	} else {
		optlen = sizeof(other_val);
		ret = getsockopt(ZVAL(fd, lval), ZVAL(level, lval), ZVAL(optname, lval), &other_val, &optlen);
		if (ret < 0) {
			RETURN_LONG(-errno);
		}

		ZVAL_LONG(*optval, other_val);
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto long setsockopt(long fd, long level, long optname, long/array optval)
   Sets socket options for the socket. */
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

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &fd, &level, &optname, &optval) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	v_convert_to_long_ex(3, fd, level, optname);

	if (ZVAL(optname, lval) == SO_LINGER) {
		HashTable *ht;
		zval **l_onoff;
		zval **l_linger;

		convert_to_array_ex(optval);
		ht = HASH_OF(*optval);

		if (zend_hash_find(ht, "l_onoff", strlen("l_onoff") + 1, (void **) &l_onoff) == FAILURE) {
			zend_error(E_ERROR, "No key \"l_onoff\" passed in optval");
			return;
		}
		if (zend_hash_find(ht, "l_linger", strlen("l_linger") + 1, (void **) &l_linger) == FAILURE) {
			zend_error(E_ERROR, "No key \"l_linger\" passed in optval");
			return;
		}

		convert_to_long_ex(l_onoff);
		convert_to_long_ex(l_linger);

		lv.l_onoff = ZVAL(l_onoff, lval);
		lv.l_linger = ZVAL(l_linger, lval);

		optlen = sizeof(lv);
		ret = setsockopt(ZVAL(fd, lval), ZVAL(level, lval), ZVAL(optname, lval), &lv, optlen);
	} else {
		convert_to_long_ex(optval);

		optlen = sizeof(ov);
		ov = ZVAL(optval, lval);

		ret = setsockopt(ZVAL(fd, lval), ZVAL(level, lval), ZVAL(optname, lval), &ov, optlen);
	}

	printf("returning: ret = %i, errno = %i\n", ret, errno);
	if (ret < 0) {
		RETURN_LONG(-errno);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

#endif				/* HAVE_SOCKETS */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
