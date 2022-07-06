/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 90c3c470833947859433a2668d328fddfff94ae9 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_select, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, write, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, except, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_create_listen, 0, 1, Socket, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, backlog, IS_LONG, 0, "128")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_accept, 0, 1, Socket, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_set_nonblock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
ZEND_END_ARG_INFO()

#define arginfo_socket_set_block arginfo_socket_set_nonblock

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_listen, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, backlog, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_write, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_read, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PHP_BINARY_READ")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_getsockname, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_INFO(1, address)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, port, "null")
ZEND_END_ARG_INFO()

#define arginfo_socket_getpeername arginfo_socket_getsockname

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_create, 0, 3, Socket, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_connect, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_strerror, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_bind, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recv, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_INFO(1, data)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_send, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recvfrom, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_INFO(1, data)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_INFO(1, address)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, port, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_sendto, 0, 5, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, address, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_get_option, 0, 3, MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_socket_getopt arginfo_socket_get_option

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_set_option, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_socket_setopt arginfo_socket_set_option

#if defined(HAVE_SOCKETPAIR)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_create_pair, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, protocol, IS_LONG, 0)
	ZEND_ARG_INFO(1, pair)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_SHUTDOWN)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_shutdown, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "2")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_last_error, 0, 0, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, socket, Socket, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_clear_error, 0, 0, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, socket, Socket, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_import_stream, 0, 1, Socket, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_export_stream, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_sendmsg, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_recvmsg, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(1, message, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_cmsg_space, 0, 2, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_addrinfo_lookup, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, service, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hints, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_addrinfo_connect, 0, 1, Socket, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, address, AddressInfo, 0)
ZEND_END_ARG_INFO()

#define arginfo_socket_addrinfo_bind arginfo_socket_addrinfo_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_addrinfo_explain, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, address, AddressInfo, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_socket_wsaprotocol_info_export, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, socket, Socket, 0)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_socket_wsaprotocol_info_import, 0, 1, Socket, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, info_id, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_socket_wsaprotocol_info_release, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, info_id, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(socket_select);
ZEND_FUNCTION(socket_create_listen);
ZEND_FUNCTION(socket_accept);
ZEND_FUNCTION(socket_set_nonblock);
ZEND_FUNCTION(socket_set_block);
ZEND_FUNCTION(socket_listen);
ZEND_FUNCTION(socket_close);
ZEND_FUNCTION(socket_write);
ZEND_FUNCTION(socket_read);
ZEND_FUNCTION(socket_getsockname);
ZEND_FUNCTION(socket_getpeername);
ZEND_FUNCTION(socket_create);
ZEND_FUNCTION(socket_connect);
ZEND_FUNCTION(socket_strerror);
ZEND_FUNCTION(socket_bind);
ZEND_FUNCTION(socket_recv);
ZEND_FUNCTION(socket_send);
ZEND_FUNCTION(socket_recvfrom);
ZEND_FUNCTION(socket_sendto);
ZEND_FUNCTION(socket_get_option);
ZEND_FUNCTION(socket_set_option);
#if defined(HAVE_SOCKETPAIR)
ZEND_FUNCTION(socket_create_pair);
#endif
#if defined(HAVE_SHUTDOWN)
ZEND_FUNCTION(socket_shutdown);
#endif
ZEND_FUNCTION(socket_last_error);
ZEND_FUNCTION(socket_clear_error);
ZEND_FUNCTION(socket_import_stream);
ZEND_FUNCTION(socket_export_stream);
ZEND_FUNCTION(socket_sendmsg);
ZEND_FUNCTION(socket_recvmsg);
ZEND_FUNCTION(socket_cmsg_space);
ZEND_FUNCTION(socket_addrinfo_lookup);
ZEND_FUNCTION(socket_addrinfo_connect);
ZEND_FUNCTION(socket_addrinfo_bind);
ZEND_FUNCTION(socket_addrinfo_explain);
#if defined(PHP_WIN32)
ZEND_FUNCTION(socket_wsaprotocol_info_export);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(socket_wsaprotocol_info_import);
#endif
#if defined(PHP_WIN32)
ZEND_FUNCTION(socket_wsaprotocol_info_release);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(socket_select, arginfo_socket_select)
	ZEND_FE(socket_create_listen, arginfo_socket_create_listen)
	ZEND_FE(socket_accept, arginfo_socket_accept)
	ZEND_FE(socket_set_nonblock, arginfo_socket_set_nonblock)
	ZEND_FE(socket_set_block, arginfo_socket_set_block)
	ZEND_FE(socket_listen, arginfo_socket_listen)
	ZEND_FE(socket_close, arginfo_socket_close)
	ZEND_FE(socket_write, arginfo_socket_write)
	ZEND_FE(socket_read, arginfo_socket_read)
	ZEND_FE(socket_getsockname, arginfo_socket_getsockname)
	ZEND_FE(socket_getpeername, arginfo_socket_getpeername)
	ZEND_FE(socket_create, arginfo_socket_create)
	ZEND_FE(socket_connect, arginfo_socket_connect)
	ZEND_FE(socket_strerror, arginfo_socket_strerror)
	ZEND_FE(socket_bind, arginfo_socket_bind)
	ZEND_FE(socket_recv, arginfo_socket_recv)
	ZEND_FE(socket_send, arginfo_socket_send)
	ZEND_FE(socket_recvfrom, arginfo_socket_recvfrom)
	ZEND_FE(socket_sendto, arginfo_socket_sendto)
	ZEND_FE(socket_get_option, arginfo_socket_get_option)
	ZEND_FALIAS(socket_getopt, socket_get_option, arginfo_socket_getopt)
	ZEND_FE(socket_set_option, arginfo_socket_set_option)
	ZEND_FALIAS(socket_setopt, socket_set_option, arginfo_socket_setopt)
#if defined(HAVE_SOCKETPAIR)
	ZEND_FE(socket_create_pair, arginfo_socket_create_pair)
#endif
#if defined(HAVE_SHUTDOWN)
	ZEND_FE(socket_shutdown, arginfo_socket_shutdown)
#endif
	ZEND_FE(socket_last_error, arginfo_socket_last_error)
	ZEND_FE(socket_clear_error, arginfo_socket_clear_error)
	ZEND_FE(socket_import_stream, arginfo_socket_import_stream)
	ZEND_FE(socket_export_stream, arginfo_socket_export_stream)
	ZEND_FE(socket_sendmsg, arginfo_socket_sendmsg)
	ZEND_FE(socket_recvmsg, arginfo_socket_recvmsg)
	ZEND_FE(socket_cmsg_space, arginfo_socket_cmsg_space)
	ZEND_FE(socket_addrinfo_lookup, arginfo_socket_addrinfo_lookup)
	ZEND_FE(socket_addrinfo_connect, arginfo_socket_addrinfo_connect)
	ZEND_FE(socket_addrinfo_bind, arginfo_socket_addrinfo_bind)
	ZEND_FE(socket_addrinfo_explain, arginfo_socket_addrinfo_explain)
#if defined(PHP_WIN32)
	ZEND_FE(socket_wsaprotocol_info_export, arginfo_socket_wsaprotocol_info_export)
#endif
#if defined(PHP_WIN32)
	ZEND_FE(socket_wsaprotocol_info_import, arginfo_socket_wsaprotocol_info_import)
#endif
#if defined(PHP_WIN32)
	ZEND_FE(socket_wsaprotocol_info_release, arginfo_socket_wsaprotocol_info_release)
#endif
	ZEND_FE_END
};


static const zend_function_entry class_Socket_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_AddressInfo_methods[] = {
	ZEND_FE_END
};
