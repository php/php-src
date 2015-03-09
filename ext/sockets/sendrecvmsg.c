/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes    <cataphract@php.net>                       |
   +----------------------------------------------------------------------+
 */

#include <php.h>
#include "php_sockets.h"
#include "sendrecvmsg.h"
#include "conversions.h"
#include <limits.h>
#include <Zend/zend_llist.h>
#ifdef ZTS
#include <TSRM/TSRM.h>
#endif

#define MAX_USER_BUFF_SIZE ((size_t)(100*1024*1024))
#define DEFAULT_BUFF_SIZE 8192
#define MAX_ARRAY_KEY_SIZE 128

#ifdef PHP_WIN32
#include "windows_common.h"
#include <Mswsock.h>
#define IPV6_RECVPKTINFO	IPV6_PKTINFO
#define IPV6_RECVHOPLIMIT	IPV6_HOPLIMIT
#define msghdr _WSAMSG

static GUID WSARecvMsg_GUID = WSAID_WSARECVMSG;
static __declspec(thread) LPFN_WSARECVMSG WSARecvMsg = NULL;
inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	DWORD	recvd = 0,
			bytesReturned;

	if (WSARecvMsg == NULL)	{
		int res = WSAIoctl((SOCKET) sockfd, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&WSARecvMsg_GUID, sizeof(WSARecvMsg_GUID),
			&WSARecvMsg, sizeof(WSARecvMsg),
			&bytesReturned, NULL, NULL);
		if (res != 0) {
			return -1;
		}
	}

	msg->dwFlags = (DWORD)flags;
	return WSARecvMsg((SOCKET)sockfd, msg, &recvd, NULL, NULL) == 0
		? (ssize_t)recvd
		: -1;
}
inline ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
	DWORD sent = 0;
	return WSASendMsg((SOCKET)sockfd, (struct msghdr*)msg, (DWORD)flags, &sent, NULL, NULL) == 0
		? (ssize_t)sent
		: -1;
}
#endif

#define LONG_CHECK_VALID_INT(l) \
	do { \
		if ((l) < INT_MIN && (l) > INT_MAX) { \
			php_error_docref0(NULL, E_WARNING, "The value %pd does not fit inside " \
					"the boundaries of a native integer", (l)); \
			return; \
		} \
	} while (0)

static struct {
	int			initialized;
	HashTable	ht;
} ancillary_registry;


static void ancillary_registery_free_elem(zval *el) {
	pefree(Z_PTR_P(el), 1);
}

#ifdef ZTS
static MUTEX_T ancillary_mutex;
#endif
static void init_ancillary_registry(void)
{
	ancillary_reg_entry entry;
	anc_reg_key key;
	ancillary_registry.initialized = 1;

	zend_hash_init(&ancillary_registry.ht, 32, NULL, ancillary_registery_free_elem, 1);

#define PUT_ENTRY(sizev, var_size, calc, from, to, level, type) \
	entry.size			= sizev; \
	entry.var_el_size	= var_size; \
	entry.calc_space	= calc; \
	entry.from_array	= from; \
	entry.to_array		= to; \
	key.cmsg_level		= level; \
	key.cmsg_type		= type; \
	zend_hash_str_update_mem(&ancillary_registry.ht, (char*)&key, sizeof(key) - 1, (void*)&entry, sizeof(entry))

#if defined(IPV6_PKTINFO) && HAVE_IPV6
	PUT_ENTRY(sizeof(struct in6_pktinfo), 0, 0, from_zval_write_in6_pktinfo,
			to_zval_read_in6_pktinfo, IPPROTO_IPV6, IPV6_PKTINFO);
#endif

#if defined(IPV6_HOPLIMIT) && HAVE_IPV6
	PUT_ENTRY(sizeof(int), 0, 0, from_zval_write_int,
			to_zval_read_int, IPPROTO_IPV6, IPV6_HOPLIMIT);
#endif

#if defined(IPV6_TCLASS) && HAVE_IPV6
	PUT_ENTRY(sizeof(int), 0, 0, from_zval_write_int,
			to_zval_read_int, IPPROTO_IPV6, IPV6_TCLASS);
#endif

#ifdef SO_PASSCRED
	PUT_ENTRY(sizeof(struct ucred), 0, 0, from_zval_write_ucred,
			to_zval_read_ucred, SOL_SOCKET, SCM_CREDENTIALS);
#endif

#ifdef SCM_RIGHTS
	PUT_ENTRY(0, sizeof(int), calculate_scm_rights_space, from_zval_write_fd_array,
			to_zval_read_fd_array, SOL_SOCKET, SCM_RIGHTS);
#endif

}
static void destroy_ancillary_registry(void)
{
	if (ancillary_registry.initialized) {
		zend_hash_destroy(&ancillary_registry.ht);
		ancillary_registry.initialized = 0;
	}
}
ancillary_reg_entry *get_ancillary_reg_entry(int cmsg_level, int msg_type)
{
	anc_reg_key			key = { cmsg_level, msg_type };
	ancillary_reg_entry	*entry;

#ifdef ZTS
	tsrm_mutex_lock(ancillary_mutex);
#endif
	if (!ancillary_registry.initialized) {
		init_ancillary_registry();
	}
#ifdef ZTS
	tsrm_mutex_unlock(ancillary_mutex);
#endif

	if ((entry = zend_hash_str_find_ptr(&ancillary_registry.ht, (char*)&key, sizeof(key) - 1)) != NULL) {
		return entry;
	} else {
		return NULL;
	}
}

PHP_FUNCTION(socket_sendmsg)
{
	zval			*zsocket,
					*zmsg;
	zend_long			flags = 0;
	php_socket		*php_sock;
	struct msghdr	*msghdr;
	zend_llist		*allocations;
	struct err_s	err = {0};
	ssize_t			res;

	/* zmsg should be passed by ref */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra|l", &zsocket, &zmsg, &flags) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(flags);

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(zsocket),
					php_sockets_le_socket_name, php_sockets_le_socket())) == NULL) {
		RETURN_FALSE;
	}

	msghdr = from_zval_run_conversions(zmsg, php_sock, from_zval_write_msghdr_send,
			sizeof(*msghdr), "msghdr", &allocations, &err);

	if (err.has_error) {
		err_msg_dispose(&err);
		RETURN_FALSE;
	}

	res = sendmsg(php_sock->bsd_socket, msghdr, (int)flags);

	if (res != -1) {
		zend_llist_destroy(allocations);
		efree(allocations);

		RETURN_LONG((zend_long)res);
	} else {
		PHP_SOCKET_ERROR(php_sock, "error in sendmsg", errno);
		RETURN_FALSE;
	}
}

PHP_FUNCTION(socket_recvmsg)
{
	zval			*zsocket,
					*zmsg;
	zend_long			flags = 0;
	php_socket		*php_sock;
	ssize_t			res;
	struct msghdr	*msghdr;
	zend_llist		*allocations;
	struct err_s	err = {0};

	//ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra/|l",
			&zsocket, &zmsg, &flags) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(flags);

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(zsocket),
					php_sockets_le_socket_name, php_sockets_le_socket())) == NULL) {
		RETURN_FALSE;
	}

	msghdr = from_zval_run_conversions(zmsg, php_sock, from_zval_write_msghdr_recv,
			sizeof(*msghdr), "msghdr", &allocations, &err);

	if (err.has_error) {
		err_msg_dispose(&err);
		RETURN_FALSE;
	}

	res = recvmsg(php_sock->bsd_socket, msghdr, (int)flags);

	if (res != -1) {
		zval *zres, tmp;
		struct key_value kv[] = {
				{KEY_RECVMSG_RET, sizeof(KEY_RECVMSG_RET), &res},
				{0}
		};


		zres = to_zval_run_conversions((char *)msghdr, to_zval_read_msghdr,
				"msghdr", kv, &err, &tmp);

		/* we don;t need msghdr anymore; free it */
		msghdr = NULL;
		allocations_dispose(&allocations);

		zval_dtor(zmsg);
		if (!err.has_error) {
			ZVAL_COPY_VALUE(zmsg, zres);
		} else {
			err_msg_dispose(&err);
			ZVAL_FALSE(zmsg);
			/* no need to destroy/free zres -- it's NULL in this circumstance */
			assert(zres == NULL);
		}
	} else {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "error in recvmsg [%d]: %s",
				errno, sockets_strerror(errno));
		RETURN_FALSE;
	}

	RETURN_LONG((zend_long)res);
}

PHP_FUNCTION(socket_cmsg_space)
{
	zend_long				level,
						type,
						n = 0;
	ancillary_reg_entry	*entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l",
			&level, &type, &n) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(level);
	LONG_CHECK_VALID_INT(type);
	LONG_CHECK_VALID_INT(n);

	if (n < 0) {
		php_error_docref0(NULL, E_WARNING, "The third argument "
				"cannot be negative");
		return;
	}

	entry = get_ancillary_reg_entry(level, type);
	if (entry == NULL) {
		php_error_docref0(NULL, E_WARNING, "The pair level %pd/type %pd is "
				"not supported by PHP", level, type);
		return;
	}

	if (entry->var_el_size > 0 && n > (ZEND_LONG_MAX - (zend_long)entry->size -
			(zend_long)CMSG_SPACE(0) - 15L) / entry->var_el_size) {
		/* the -15 is to account for any padding CMSG_SPACE may add after the data */
		php_error_docref0(NULL, E_WARNING, "The value for the "
				"third argument (%pd) is too large", n);
		return;
	}

	RETURN_LONG((zend_long)CMSG_SPACE(entry->size + n * entry->var_el_size));
}

#if HAVE_IPV6
int php_do_setsockopt_ipv6_rfc3542(php_socket *php_sock, int level, int optname, zval *arg4)
{
	struct err_s	err = {0};
	zend_llist		*allocations = NULL;
	void			*opt_ptr;
	socklen_t		optlen;
	int				retval;

	assert(level == IPPROTO_IPV6);

	switch (optname) {
#ifdef IPV6_PKTINFO
	case IPV6_PKTINFO:
#ifdef PHP_WIN32
		if (Z_TYPE_P(arg4) == IS_ARRAY) {
			php_error_docref0(NULL, E_WARNING, "Windows does not "
					"support sticky IPV6_PKTINFO");
			return FAILURE;
		} else {
			/* windows has no IPV6_RECVPKTINFO, and uses IPV6_PKTINFO
			 * for the same effect. We define IPV6_RECVPKTINFO to be
			 * IPV6_PKTINFO, so assume the assume user used IPV6_RECVPKTINFO */
			return 1;
		}
#endif
		opt_ptr = from_zval_run_conversions(arg4, php_sock, from_zval_write_in6_pktinfo,
				sizeof(struct in6_pktinfo),	"in6_pktinfo", &allocations, &err);
		if (err.has_error) {
			err_msg_dispose(&err);
			return FAILURE;
		}

		optlen = sizeof(struct in6_pktinfo);
		goto dosockopt;
#endif
	}

	/* we also support IPV6_TCLASS, but that can be handled by the default
	 * integer optval handling in the caller */
	return 1;

dosockopt:
	retval = setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen);
	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to set socket option", errno);
	}
	allocations_dispose(&allocations);

	return retval != 0 ? FAILURE : SUCCESS;
}

int php_do_getsockopt_ipv6_rfc3542(php_socket *php_sock, int level, int optname, zval *result)
{
	struct err_s		err = {0};
	void				*buffer;
	socklen_t			size;
	int					res;
	to_zval_read_field	*reader;

	assert(level == IPPROTO_IPV6);

	switch (optname) {
#ifdef IPV6_PKTINFO
	case IPV6_PKTINFO:
		size = sizeof(struct in6_pktinfo);
		reader = &to_zval_read_in6_pktinfo;
		break;
#endif
	default:
		return 1;
	}

	buffer = ecalloc(1, size);
	res = getsockopt(php_sock->bsd_socket, level, optname, buffer, &size);
	if (res != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to get socket option", errno);
	} else {
		zval tmp;
		zval *zv = to_zval_run_conversions(buffer, reader, "in6_pktinfo",
				empty_key_value_list, &err, &tmp);
		if (err.has_error) {
			err_msg_dispose(&err);
			res = -1;
		} else {
			ZVAL_COPY_VALUE(result, zv);
		}
	}
	efree(buffer);

	return res == 0 ? SUCCESS : FAILURE;
}
#endif /* HAVE_IPV6 */

void php_socket_sendrecvmsg_init(INIT_FUNC_ARGS)
{
	/* IPv6 ancillary data */
#if defined(IPV6_RECVPKTINFO) && HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_RECVPKTINFO",		IPV6_RECVPKTINFO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_PKTINFO",          IPV6_PKTINFO,       CONST_CS | CONST_PERSISTENT);
#endif
#if defined(IPV6_RECVHOPLIMIT) && HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_RECVHOPLIMIT",		IPV6_RECVHOPLIMIT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_HOPLIMIT",         IPV6_HOPLIMIT,      CONST_CS | CONST_PERSISTENT);
#endif
	/* would require some effort:
	REGISTER_LONG_CONSTANT("IPV6_RECVRTHDR",		IPV6_RECVRTHDR,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_RECVHOPOPTS",		IPV6_RECVHOPOPTS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_RECVDSTOPTS",		IPV6_RECVDSTOPTS,	CONST_CS | CONST_PERSISTENT);
	*/
#if defined(IPV6_RECVTCLASS) && HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_RECVTCLASS",		IPV6_RECVTCLASS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_TCLASS",			IPV6_TCLASS,		CONST_CS | CONST_PERSISTENT);
#endif

	/*
	REGISTER_LONG_CONSTANT("IPV6_RTHDR",			IPV6_RTHDR,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_HOPOPTS",			IPV6_HOPOPTS,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_DSTOPTS",			IPV6_DSTOPTS,		CONST_CS | CONST_PERSISTENT);
	*/

#ifdef SCM_RIGHTS
	REGISTER_LONG_CONSTANT("SCM_RIGHTS",			SCM_RIGHTS,			CONST_CS | CONST_PERSISTENT);
#endif
#ifdef SO_PASSCRED
	REGISTER_LONG_CONSTANT("SCM_CREDENTIALS",		SCM_CREDENTIALS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_PASSCRED",			SO_PASSCRED,		CONST_CS | CONST_PERSISTENT);
#endif

#ifdef ZTS
	ancillary_mutex = tsrm_mutex_alloc();
#endif
}

void php_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS)
{
#ifdef ZTS
	tsrm_mutex_free(ancillary_mutex);
#endif

	destroy_ancillary_registry();
}
