/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
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

PHP_FUNCTION(stream_socket_client);
PHP_FUNCTION(stream_socket_server);
PHP_FUNCTION(stream_socket_accept);
PHP_FUNCTION(stream_socket_get_name);

PHP_FUNCTION(set_socket_blocking); /* deprecated */
PHP_FUNCTION(stream_set_blocking);
PHP_FUNCTION(stream_select);
PHP_FUNCTION(stream_set_timeout);
PHP_FUNCTION(stream_set_write_buffer);
PHP_FUNCTION(stream_get_wrappers);
PHP_FUNCTION(stream_get_line);
PHP_FUNCTION(stream_get_meta_data);
PHP_FUNCTION(stream_register_wrapper);
PHP_FUNCTION(stream_context_create);
PHP_FUNCTION(stream_context_set_params);
PHP_FUNCTION(stream_context_set_option);
PHP_FUNCTION(stream_context_get_options);
PHP_FUNCTION(stream_filter_prepend);
PHP_FUNCTION(stream_filter_append);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

