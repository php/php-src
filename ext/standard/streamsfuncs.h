/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Flags for stream_socket_client */
#define PHP_STREAM_CLIENT_PERSISTENT	1
#define PHP_STREAM_CLIENT_ASYNC_CONNECT	2
#define PHP_STREAM_CLIENT_CONNECT		4

PHP_FUNCTION(stream_socket_client);
PHP_FUNCTION(stream_socket_server);
PHP_FUNCTION(stream_socket_accept);
PHP_FUNCTION(stream_socket_get_name);
PHP_FUNCTION(stream_socket_recvfrom);
PHP_FUNCTION(stream_socket_sendto);

PHP_FUNCTION(stream_copy_to_stream);
PHP_FUNCTION(stream_get_contents);

PHP_FUNCTION(stream_set_blocking);
PHP_FUNCTION(stream_select);
PHP_FUNCTION(stream_set_timeout);
PHP_FUNCTION(stream_set_read_buffer);
PHP_FUNCTION(stream_set_write_buffer);
PHP_FUNCTION(stream_set_chunk_size);
PHP_FUNCTION(stream_get_transports);
PHP_FUNCTION(stream_get_wrappers);
PHP_FUNCTION(stream_get_line);
PHP_FUNCTION(stream_get_meta_data);
PHP_FUNCTION(stream_wrapper_register);
PHP_FUNCTION(stream_wrapper_unregister);
PHP_FUNCTION(stream_wrapper_restore);
PHP_FUNCTION(stream_context_create);
PHP_FUNCTION(stream_context_set_params);
PHP_FUNCTION(stream_context_get_params);
PHP_FUNCTION(stream_context_set_option);
PHP_FUNCTION(stream_context_get_options);
PHP_FUNCTION(stream_context_get_default);
PHP_FUNCTION(stream_context_set_default);
PHP_FUNCTION(stream_filter_prepend);
PHP_FUNCTION(stream_filter_append);
PHP_FUNCTION(stream_filter_remove);
PHP_FUNCTION(stream_socket_enable_crypto);
PHP_FUNCTION(stream_socket_shutdown);
PHP_FUNCTION(stream_resolve_include_path);
PHP_FUNCTION(stream_is_local);
PHP_FUNCTION(stream_supports_lock);

#if HAVE_SOCKETPAIR
PHP_FUNCTION(stream_socket_pair);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

