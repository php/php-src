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

#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

/* Remove comments and fill if you need to have entries in php.ini
   PHP_INI_BEGIN()
   PHP_INI_END()
 */

static void destroy_fd_sets(fd_set * set)
{
	efree(set);
}

PHP_MINIT_FUNCTION(sockets)
{
/* Remove comments if you have entries in php.ini
   REGISTER_INI_ENTRIES();
 */
	le_destroy = register_list_destructors(destroy_fd_sets, NULL);
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
	zval *new_resource;
	int ret;

	set = emalloc(sizeof(fd_set));
	if (!set) {
		zend_error(E_ERROR, "Can't allocate memory for fd_set");
		RETVAL_FALSE;
	}
	ret = ZEND_REGISTER_RESOURCE(new_resource, set, le_destroy);
	RETURN_RESOURCE(ret);
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
	int ret;

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
   Check to see if a file descriptor is set within the file descriptor set */
PHP_FUNCTION(fd_isset)
{
	zval **set, **fd;
	fd_set *the_set;
	int ret;

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
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
/*
   See select(2) man page for details.

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
   Accepts a connection on the listening socket fd */
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
   Sets nonblocking mode for file descriptor fd */
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
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
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
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port int addr and port describing the local side of a socket */

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
   Given an fd, stores a string representing sa.sin_addr and the value of sa.sin_port int addr and port describing the remote side of a socket */

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

/* {{{ proto long socket(string domain, string type, long protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket)
{
	zval **domain, **type, **protocol;
	int ret;
	int my_type, my_domain;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &domain, &type, &protocol) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain);
	convert_to_string_ex(type);
	convert_to_long_ex(protocol);

	if (!strcasecmp(ZVAL(domain, str.val), "af_inet")) {
		my_domain = AF_INET;
	} else if (!strcasecmp(ZVAL(domain, str.val), "af_unix")) {
		my_domain = AF_UNIX;
	} else {
		zend_error(E_WARNING, "invalid communications domain specified - assuming AF_INET");
		my_domain = AF_INET;
	}

	if (!strcasecmp(ZVAL(type, str.val), "sock_stream")) {
		my_type = SOCK_STREAM;
	} else if (!strcasecmp(ZVAL(type, str.val), "sock_dgram")) {
		my_type = SOCK_DGRAM;
	} else if (!strcasecmp(ZVAL(type, str.val), "sock_raw")) {
		my_type = SOCK_RAW;
	} else if (!strcasecmp(ZVAL(type, str.val), "sock_seqpacket")) {
		my_type = SOCK_SEQPACKET;
	} else if (!strcasecmp(ZVAL(type, str.val), "sock_rdm")) {
		my_type = SOCK_RDM;
	} else {
		zend_error(E_WARNING, "invalid socket type specified - assuming SOCK_STREAM");
		my_type = SOCK_STREAM;
	}

	ret = socket(my_domain, my_type, ZVAL(protocol, lval));
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

/* {{{ proto long bind(long sockfd, string domain [, string ...])
   Binds an open socket to a listening port */
/*	-- domain = "af_unix", 3rd arg is path to socket
	-- domain = "af_inet", 3rd arg is address to bind to, 4th arg is port */
PHP_FUNCTION(bind)
{
	zval **arg0, **arg1, **arg2, **arg3;
	long ret;
	void **p;
	int arg_count;
	va_list ptr;
	ELS_FETCH();

	if (ZEND_NUM_ARGS() < 2) {
		WRONG_PARAM_COUNT;
	}

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	arg0 = (zval **) p-(arg_count --);
	arg1 = (zval **) p-(arg_count--);

	convert_to_long_ex(arg0);
	convert_to_string_ex(arg1);

	if (!strcasecmp(ZVAL(arg1, str.val), "af_unix")) {
		struct sockaddr_un sa;
		if (ZEND_NUM_ARGS() != 3) {
			WRONG_PARAM_COUNT;
		}
		arg2 = (zval **) p-(arg_count--);
		snprintf(sa.sun_path, 108, "%s", ZVAL(arg2, str.val));
		ret = bind(ZVAL(arg0, lval), &sa, sizeof(sa));
	} else if (!strcasecmp(ZVAL(arg1, str.val), "af_inet")) {
		struct sockaddr_in sa;
		struct in_addr addr_buf;
		if (ZEND_NUM_ARGS() != 4) {
			WRONG_PARAM_COUNT;
		}
		arg2 = (zval **) p-(arg_count--);
		arg3 = (zval **) p-(arg_count--);
		sa.sin_port = htons(ZVAL(arg3, lval));
		if (inet_aton(ZVAL(arg2, str.val), &addr_buf) < 0) {
			struct hostent *host_struct = gethostbyname(ZVAL(arg2, str.val));
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

#endif				/* HAVE_SOCKETS */




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
