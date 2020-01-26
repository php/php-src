/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_select, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, read_fds, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, write_fds, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, except_fds, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, tv_sec, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, tv_usec, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create_listen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, backlog, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_accept, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_set_nonblock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

#define arginfo_socket_set_block arginfo_socket_set_nonblock

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_listen, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, backlog, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_write, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, buf, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_read, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_getsockname, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, addr)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

#define arginfo_socket_getpeername arginfo_socket_getsockname

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, domain, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_connect, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, addr, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errno, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_socket_bind arginfo_socket_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recv, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_send, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, buf, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recvfrom, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_INFO(1, name)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_sendto, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, buf, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, addr, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_get_option, 0, 3, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, optname, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_set_option, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, optname, IS_LONG, 0)
	ZEND_ARG_INFO(0, optval)
ZEND_END_ARG_INFO()

#if defined(HAVE_SOCKETPAIR)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_create_pair, 0, 4, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, domain, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
	ZEND_ARG_INFO(1, fd)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SHUTDOWN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_shutdown, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, how, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_last_error, 0, 0, IS_LONG, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_clear_error, 0, 0, IS_VOID, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_import_stream, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_socket_export_stream arginfo_socket_accept

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_sendmsg, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, msghdr, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recvmsg, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(1, msghdr, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_cmsg_space, 0, 2, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, n, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_addrinfo_lookup, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, service, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hints, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_addrinfo_connect, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

#define arginfo_socket_addrinfo_bind arginfo_socket_addrinfo_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_addrinfo_explain, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_wsaprotocol_info_export, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_TYPE_INFO(0, target_pid, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_wsaprotocol_info_import, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, info_id, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_wsaprotocol_info_release, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, info_id, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif
