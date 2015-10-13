/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/
/* $Id: mysqlnd.h 318221 2011-10-19 15:04:12Z andrey $ */

#ifndef MYSQLND_EXT_PLUGIN_H
#define MYSQLND_EXT_PLUGIN_H

struct st_mysqlnd_plugin__plugin_area_getters
{
	void ** (*get_connection_area)(const MYSQLND * conn, unsigned int plugin_id);
	void ** (*get_connection_data_area)(const MYSQLND_CONN_DATA * conn, unsigned int plugin_id);
	void ** (*get_result_area)(const MYSQLND_RES * result, unsigned int plugin_id);
	void ** (*get_unbuffered_area)(const MYSQLND_RES_UNBUFFERED * result, unsigned int plugin_id);
	void ** (*get_result_buffered_area)(const MYSQLND_RES_BUFFERED_ZVAL * result, unsigned int plugin_id);
	void ** (*get_result_buffered_aread_c)(const MYSQLND_RES_BUFFERED_C * result, unsigned int plugin_id);
	void ** (*get_stmt_area)(const MYSQLND_STMT * stmt, unsigned int plugin_id);
	void ** (*get_protocol_decoder_area)(const MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * factory, unsigned int plugin_id);
	void ** (*get_net_area)(const MYSQLND_NET * net, unsigned int plugin_id);
};

extern struct st_mysqlnd_plugin__plugin_area_getters mysqlnd_plugin_area_getters;

#define mysqlnd_plugin_get_plugin_connection_data(c, p_id)				mysqlnd_plugin_area_getters.get_connection_area((c), (p_id))
#define mysqlnd_plugin_get_plugin_connection_data_data(c, p_id)			mysqlnd_plugin_area_getters.get_connection_data_area((c), (p_id))
#define mysqlnd_plugin_get_plugin_result_data(res, p_id)				mysqlnd_plugin_area_getters.get_result_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_unbuffered_data(res, p_id)		mysqlnd_plugin_area_getters.get_unbuffered_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_buffered_data_zval(res, p_id)	mysqlnd_plugin_area_getters.get_result_buffered_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_buffered_data_c(res, p_id)		mysqlnd_plugin_area_getters.get_result_buffered_aread_c((res), (p_id))
#define mysqlnd_plugin_get_plugin_stmt_data(stmt, p_id)					mysqlnd_plugin_area_getters.get_stmt_area((stmt), (p_id))
#define mysqlnd_plugin_get_plugin_protocol_data(proto, p_id)			mysqlnd_plugin_area_getters.get_protocol_decoder_area((proto), (p_id))
#define mysqlnd_plugin_get_plugin_net_data(net, p_id)					mysqlnd_plugin_area_getters.get_net_area((net), (p_id))


struct st_mysqlnd_plugin_methods_xetters
{
	struct st_mnd_object_factory_xetters
	{
		struct st_mysqlnd_object_factory_methods * (*get)();
		void (*set)(struct st_mysqlnd_object_factory_methods * methods);
	} object_factory;

	struct st_mnd_connection_xetters
	{
		struct st_mysqlnd_conn_methods * (*get)();
		void (*set)(struct st_mysqlnd_conn_methods * methods);
	} connection;

	struct st_mnd_connection_data_xetters
	{
		struct st_mysqlnd_conn_data_methods * (*get)();
		void (*set)(struct st_mysqlnd_conn_data_methods * methods);
	} connection_data;

	struct st_mnd_result_etters
	{
		struct st_mysqlnd_res_methods * (*get)();
		void (*set)(struct st_mysqlnd_res_methods * methods);
	} result;

	struct st_mnd_unbuffered_result_xetters
	{
		struct st_mysqlnd_result_unbuffered_methods * (*get)();
		void (*set)(struct st_mysqlnd_result_unbuffered_methods * methods);
	} unbuffered_result;

	struct st_mnd_buffered_result_xetters
	{
		struct st_mysqlnd_result_buffered_methods * (*get)();
		void (*set)(struct st_mysqlnd_result_buffered_methods * methods);
	} buffered_result;

	struct st_mnd_stmt_xetters
	{
		struct st_mysqlnd_stmt_methods * (*get)();
		void (*set)(struct st_mysqlnd_stmt_methods * methods);
	} statement;

	struct st_mnd_protocol_xetters
	{
		struct st_mysqlnd_protocol_payload_decoder_factory_methods * (*get)();
		void (*set)(struct st_mysqlnd_protocol_payload_decoder_factory_methods * methods);
	} protocol;

	struct st_mnd_io_xetters
	{
		struct st_mysqlnd_net_methods * (*get)();
		void (*set)(struct st_mysqlnd_net_methods * methods);
	} io;
};

extern struct st_mysqlnd_plugin_methods_xetters mysqlnd_plugin_methods_xetters;


#define mysqlnd_object_factory_get_methods()	mysqlnd_plugin_methods_xetters.object_factory.get()
#define mysqlnd_object_factory_set_methods(m)	mysqlnd_plugin_methods_xetters.object_factory.set((m))

#define mysqlnd_conn_get_methods()			mysqlnd_plugin_methods_xetters.connection.get()
#define mysqlnd_conn_set_methods(m)			mysqlnd_plugin_methods_xetters.connection.set((m))

#define mysqlnd_conn_data_get_methods()		mysqlnd_plugin_methods_xetters.connection_data.get()
#define mysqlnd_conn_data_set_methods(m)	mysqlnd_plugin_methods_xetters.connection_data.set((m))

#define mysqlnd_result_get_methods()		mysqlnd_plugin_methods_xetters.result.get()
#define mysqlnd_result_set_methods(m)		mysqlnd_plugin_methods_xetters.result.set((m))

#define mysqlnd_result_unbuffered_get_methods()		mysqlnd_plugin_methods_xetters.unbuffered_result.get()
#define mysqlnd_result_unbuffered_set_methods(m)	mysqlnd_plugin_methods_xetters.unbuffered_result.set((m))

#define mysqlnd_result_buffered_get_methods()		mysqlnd_plugin_methods_xetters.buffered_result.get()
#define mysqlnd_result_buffered_set_methods(m)		mysqlnd_plugin_methods_xetters.buffered_result.set((m))

#define mysqlnd_stmt_get_methods()		mysqlnd_plugin_methods_xetters.statement.get()
#define mysqlnd_stmt_set_methods(m)		mysqlnd_plugin_methods_xetters.statement.set((m))

#define mysqlnd_protocol_get_methods()	mysqlnd_plugin_methods_xetters.protocol.get()
#define mysqlnd_protocol_set_methods(m)	mysqlnd_plugin_methods_xetters.protocol.set((m))

#define mysqlnd_net_get_methods()		mysqlnd_plugin_methods_xetters.io.get()
#define mysqlnd_net_set_methods(m)		mysqlnd_plugin_methods_xetters.io.set((m))

#endif	/* MYSQLND_EXT_PLUGIN_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
