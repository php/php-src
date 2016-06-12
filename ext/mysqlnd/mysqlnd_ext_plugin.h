/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_EXT_PLUGIN_H
#define MYSQLND_EXT_PLUGIN_H

PHPAPI void ** _mysqlnd_plugin_get_plugin_connection_data(const MYSQLND * conn, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_connection_data(c, p_id) _mysqlnd_plugin_get_plugin_connection_data((c), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_connection_data_data(const MYSQLND_CONN_DATA * conn, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_connection_data_data(c, p_id) _mysqlnd_plugin_get_plugin_connection_data_data((c), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_result_data(const MYSQLND_RES * result, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_result_data(r, p_id) _mysqlnd_plugin_get_plugin_result_data((r), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_result_unbuffered_data(const MYSQLND_RES_UNBUFFERED * result, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_result_unbuffered_data(r, p_id) _mysqlnd_plugin_get_plugin_result_unbuffered_data((r), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_result_buffered_data_zval(const MYSQLND_RES_BUFFERED_ZVAL * result, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_result_buffered_data_zval(r, p_id) _mysqlnd_plugin_get_plugin_result_buffered_data_zval((r), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_result_buffered_data_c(const MYSQLND_RES_BUFFERED_C * result, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_result_buffered_data_c(r, p_id) _mysqlnd_plugin_get_plugin_result_buffered_data_c((r), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_stmt_data(const MYSQLND_STMT * stmt, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_stmt_data(s, p_id) _mysqlnd_plugin_get_plugin_stmt_data((s), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_protocol_data(const MYSQLND_PROTOCOL * protocol, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_protocol_data(p, p_id) _mysqlnd_plugin_get_plugin_protocol_data((p), (p_id))

PHPAPI void ** _mysqlnd_plugin_get_plugin_net_data(const MYSQLND_NET * net, unsigned int plugin_id);
#define mysqlnd_plugin_get_plugin_net_data(n, p_id) _mysqlnd_plugin_get_plugin_net_data((n), (p_id))


PHPAPI struct st_mysqlnd_conn_methods * mysqlnd_conn_get_methods();
PHPAPI void mysqlnd_conn_set_methods(struct st_mysqlnd_conn_methods * methods);

PHPAPI struct st_mysqlnd_conn_data_methods * mysqlnd_conn_data_get_methods();
PHPAPI void mysqlnd_conn_data_set_methods(struct st_mysqlnd_conn_data_methods * methods);

PHPAPI struct st_mysqlnd_res_methods * mysqlnd_result_get_methods();
PHPAPI void mysqlnd_result_set_methods(struct st_mysqlnd_res_methods * methods);

PHPAPI struct st_mysqlnd_result_unbuffered_methods * mysqlnd_result_unbuffered_get_methods();
PHPAPI void mysqlnd_result_unbuffered_set_methods(struct st_mysqlnd_result_unbuffered_methods * methods);

PHPAPI struct st_mysqlnd_result_buffered_methods * mysqlnd_result_buffered_get_methods();
PHPAPI void mysqlnd_result_buffered_set_methods(struct st_mysqlnd_result_buffered_methods * methods);

PHPAPI struct st_mysqlnd_stmt_methods * mysqlnd_stmt_get_methods();
PHPAPI void mysqlnd_stmt_set_methods(struct st_mysqlnd_stmt_methods * methods);

PHPAPI struct st_mysqlnd_protocol_methods * mysqlnd_protocol_get_methods();
PHPAPI void mysqlnd_protocol_set_methods(struct st_mysqlnd_protocol_methods * methods);

PHPAPI struct st_mysqlnd_net_methods * mysqlnd_net_get_methods();
PHPAPI void mysqlnd_net_set_methods(struct st_mysqlnd_net_methods * methods);


#endif	/* MYSQLND_EXT_PLUGIN_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
