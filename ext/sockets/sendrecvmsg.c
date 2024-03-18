/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes    <cataphract@php.net>                       |
   +----------------------------------------------------------------------+
 */

#ifdef __sun
/* to enable 'new' ancillary data layout instead */
# define _XPG4_2
#endif
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

#define LONG_CHECK_VALID_INT(l, arg_pos) \
	do { \
		if ((l) < INT_MIN || (l) > INT_MAX) { \
			zend_argument_value_error((arg_pos), "must be between %d and %d", INT_MIN, INT_MAX); \
			RETURN_THROWS(); \
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
	zend_hash_str_update_mem(&ancillary_registry.ht, (char*)&key, sizeof(key), (void*)&entry, sizeof(entry))

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
#ifdef HAVE_STRUCT_UCRED
	PUT_ENTRY(sizeof(struct ucred), 0, 0, from_zval_write_ucred,
			to_zval_read_ucred, SOL_SOCKET, SCM_CREDENTIALS);
#else
	PUT_ENTRY(sizeof(struct cmsgcred), 0, 0, from_zval_write_ucred,
			to_zval_read_ucred, SOL_SOCKET, SCM_CREDS);
#endif
#endif

#if defined(LOCAL_CREDS_PERSISTENT)
	PUT_ENTRY(SOCKCRED2SIZE(1), 1, 0, from_zval_write_ucred,
			to_zval_read_ucred, SOL_SOCKET, SCM_CREDS2);
#elif defined(LOCAL_CREDS)
	PUT_ENTRY(SOCKCREDSIZE(1), 1, 0, from_zval_write_ucred,
			to_zval_read_ucred, SOL_SOCKET, SCM_CREDS);
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

	if ((entry = zend_hash_str_find_ptr(&ancillary_registry.ht, (char*)&key, sizeof(key))) != NULL) {
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
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa|l", &zsocket, socket_ce, &zmsg, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	LONG_CHECK_VALID_INT(flags, 3);

	php_sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(php_sock);

	msghdr = from_zval_run_conversions(zmsg, php_sock, from_zval_write_msghdr_send,
			sizeof(*msghdr), "msghdr", &allocations, &err);

	if (err.has_error) {
		err_msg_dispose(&err);
		RETURN_FALSE;
	}

	res = sendmsg(php_sock->bsd_socket, msghdr, (int)flags);

	if (res != -1) {
		RETVAL_LONG((zend_long)res);
	} else {
		PHP_SOCKET_ERROR(php_sock, "Error in sendmsg", errno);
		RETVAL_FALSE;
	}

	allocations_dispose(&allocations);
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
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oa|l", &zsocket, socket_ce, &zmsg, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	LONG_CHECK_VALID_INT(flags, 3);

	php_sock = Z_SOCKET_P(zsocket);
	ENSURE_SOCKET_VALID(php_sock);

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

		zval_ptr_dtor(zmsg);
		if (!err.has_error) {
			ZVAL_COPY_VALUE(zmsg, zres);
		} else {
			err_msg_dispose(&err);
			ZVAL_FALSE(zmsg);
			/* no need to destroy/free zres -- it's NULL in this circumstance */
			assert(zres == NULL);
		}
		RETVAL_LONG((zend_long)res);
	} else {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Error in recvmsg [%d]: %s",
				errno, sockets_strerror(errno));
		RETVAL_FALSE;
	}

	allocations_dispose(&allocations);
}

PHP_FUNCTION(socket_cmsg_space)
{
	zend_long				level,
						type,
						n = 0;
	ancillary_reg_entry	*entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l",
			&level, &type, &n) == FAILURE) {
		RETURN_THROWS();
	}

	LONG_CHECK_VALID_INT(level, 1);
	LONG_CHECK_VALID_INT(type, 2);
	LONG_CHECK_VALID_INT(n, 3);

	if (n < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	entry = get_ancillary_reg_entry(level, type);
	if (entry == NULL) {
		zend_value_error("Pair level " ZEND_LONG_FMT " and/or type " ZEND_LONG_FMT " is not supported",
			level, type);
		RETURN_THROWS();
	}

	if (entry->var_el_size > 0) {
		/* Leading underscore to avoid symbol collision on AIX. */
		size_t _rem_size = ZEND_LONG_MAX - entry->size;
		size_t n_max = _rem_size / entry->var_el_size;
		size_t size = entry->size + n * entry->var_el_size;
		size_t total_size = CMSG_SPACE(size);
		if (n > n_max /* zend_long overflow */
			|| total_size > ZEND_LONG_MAX
			|| total_size < size /* align overflow */) {
			zend_argument_value_error(3, "is too large");
			RETURN_THROWS();
		}
	}

	RETURN_LONG((zend_long)CMSG_SPACE(entry->size + n * entry->var_el_size));
}

#ifdef HAVE_IPV6
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
			php_error_docref(NULL, E_WARNING, "Windows does not "
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
		PHP_SOCKET_ERROR(php_sock, "Unable to set socket option", errno);
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
