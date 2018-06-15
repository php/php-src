/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_AUTH_H
#define MYSQLND_AUTH_H
enum_func_status
mysqlnd_auth_handshake(MYSQLND_CONN_DATA * conn,
						const char * const user,
						const char * const passwd,
						const size_t passwd_len,
						const char * const db,
						const size_t db_len,
						const MYSQLND_SESSION_OPTIONS * const session_options,
						zend_ulong mysql_flags,
						unsigned int server_charset_no,
						zend_bool use_full_blown_auth_packet,
						const char * const auth_protocol,
						struct st_mysqlnd_authentication_plugin * auth_plugin,
						const zend_uchar * const orig_auth_plugin_data,
						const size_t orig_auth_plugin_data_len,
						const zend_uchar * const auth_plugin_data,
						const size_t auth_plugin_data_len,
						char ** switch_to_auth_protocol,
						size_t * switch_to_auth_protocol_len,
						zend_uchar ** switch_to_auth_protocol_data,
						size_t * switch_to_auth_protocol_data_len
						);

enum_func_status
mysqlnd_auth_change_user(MYSQLND_CONN_DATA * const conn,
								const char * const user,
								const size_t user_len,
								const char * const passwd,
								const size_t passwd_len,
								const char * const db,
								const size_t db_len,
								const zend_bool silent,
								zend_bool use_full_blown_auth_packet,
								const char * const auth_protocol,
								zend_uchar * auth_plugin_data,
								size_t auth_plugin_data_len,
								char ** switch_to_auth_protocol,
								size_t * switch_to_auth_protocol_len,
								zend_uchar ** switch_to_auth_protocol_data,
								size_t * switch_to_auth_protocol_data_len
								);


enum_func_status
mysqlnd_connect_run_authentication(
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const char * const db,
			size_t db_len,
			size_t passwd_len,
			MYSQLND_STRING authentication_plugin_data,
			const char * const authentication_protocol,
			const unsigned int charset_no,
			size_t server_capabilities,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			zend_ulong mysql_flags
			);

enum_func_status
mysqlnd_run_authentication(
			MYSQLND_CONN_DATA * conn,
			const char * const user,
			const char * const passwd,
			const size_t passwd_len,
			const char * const db,
			const size_t db_len,
			const MYSQLND_STRING auth_plugin_data,
			const char * const auth_protocol,
			unsigned int charset_no,
			const MYSQLND_SESSION_OPTIONS * const session_options,
			zend_ulong mysql_flags,
			zend_bool silent,
			zend_bool is_change_user
			);

PHPAPI void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const pass, const size_t pass_len);

#endif /* MYSQLND_AUTH_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
