/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  |          Johannes Schlüter <johannes@php.net>                        |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_EXT_PLUGIN_H
#define MYSQLND_EXT_PLUGIN_H

struct st_mysqlnd_plugin__plugin_area_getters
{
	void ** (*get_connection_area)(const MYSQLND * conn, const unsigned int plugin_id);
	void ** (*get_connection_data_area)(const MYSQLND_CONN_DATA * conn, const unsigned int plugin_id);
	void ** (*get_result_area)(const MYSQLND_RES * result, const unsigned int plugin_id);
	void ** (*get_unbuffered_area)(const MYSQLND_RES_UNBUFFERED * result, const unsigned int plugin_id);
	void ** (*get_result_buffered_area)(const MYSQLND_RES_BUFFERED_ZVAL * result, const unsigned int plugin_id);
	void ** (*get_result_buffered_aread_c)(const MYSQLND_RES_BUFFERED_C * result, const unsigned int plugin_id);
	void ** (*get_stmt_area)(const MYSQLND_STMT * stmt, const unsigned int plugin_id);
	void ** (*get_protocol_decoder_area)(const MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * factory, const unsigned int plugin_id);
	void ** (*get_pfc_area)(const MYSQLND_PFC * pfc, const unsigned int plugin_id);
	void ** (*get_vio_area)(const MYSQLND_VIO * vio, const unsigned int plugin_id);
};

PHPAPI extern struct st_mysqlnd_plugin__plugin_area_getters mysqlnd_plugin_area_getters;

#define mysqlnd_plugin_get_plugin_connection_data(c, p_id)				mysqlnd_plugin_area_getters.get_connection_area((c), (p_id))
#define mysqlnd_plugin_get_plugin_connection_data_data(c, p_id)			mysqlnd_plugin_area_getters.get_connection_data_area((c), (p_id))
#define mysqlnd_plugin_get_plugin_result_data(res, p_id)				mysqlnd_plugin_area_getters.get_result_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_unbuffered_data(res, p_id)		mysqlnd_plugin_area_getters.get_unbuffered_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_buffered_data_zval(res, p_id)	mysqlnd_plugin_area_getters.get_result_buffered_area((res), (p_id))
#define mysqlnd_plugin_get_plugin_result_buffered_data_c(res, p_id)		mysqlnd_plugin_area_getters.get_result_buffered_aread_c((res), (p_id))
#define mysqlnd_plugin_get_plugin_stmt_data(stmt, p_id)					mysqlnd_plugin_area_getters.get_stmt_area((stmt), (p_id))
#define mysqlnd_plugin_get_plugin_protocol_data(proto, p_id)			mysqlnd_plugin_area_getters.get_protocol_decoder_area((proto), (p_id))
#define mysqlnd_plugin_get_plugin_pfc_data(pfc, p_id)					mysqlnd_plugin_area_getters.get_pfc_area((pfc), (p_id))
#define mysqlnd_plugin_get_plugin_vio_data(vio, p_id)					mysqlnd_plugin_area_getters.get_pfc_area((vio), (p_id))


struct st_mysqlnd_plugin_methods_xetters
{
	struct st_mnd_object_factory_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *methods);
	} object_factory;

	struct st_mnd_connection_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn) *methods);
	} connection;

	struct st_mnd_connection_data_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data) *methods);
	} connection_data;

	struct st_mnd_result_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res) *methods);
	} result;

	struct st_mnd_unbuffered_result_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered) *methods);
	} unbuffered_result;

	struct st_mnd_buffered_result_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered)* (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered) *methods);
	} buffered_result;

	struct st_mnd_stmt_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt) * methods);
	} statement;

	struct st_mnd_protocol_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory)* (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory) *methods);
	} protocol;

	struct st_mnd_pfc_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec) * methods);
	} pfc;

	struct st_mnd_vio_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio) * methods);
	} vio;

	struct st_mnd_error_info_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info) * methods);
	} error_info;

	struct st_mnd_command_xetters
	{
		MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command) * (*get)();
		void (*set)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command) * methods);
	} command;
};

PHPAPI extern struct st_mysqlnd_plugin_methods_xetters mysqlnd_plugin_methods_xetters;


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

#define mysqlnd_pfc_get_methods()		mysqlnd_plugin_methods_xetters.pfc.get()
#define mysqlnd_pfc_set_methods(m)		mysqlnd_plugin_methods_xetters.pfc.set((m))

#define mysqlnd_vio_get_methods()		mysqlnd_plugin_methods_xetters.vio.get()
#define mysqlnd_vio_set_methods(m)		mysqlnd_plugin_methods_xetters.vio.set((m))

#define mysqlnd_command_get_methods()		mysqlnd_plugin_methods_xetters.command.get()
#define mysqlnd_command_set_methods(m)		mysqlnd_plugin_methods_xetters.command.set((m))

#define mysqlnd_error_info_get_methods()	mysqlnd_plugin_methods_xetters.error_info.get()
#define mysqlnd_error_info_set_methods(m)	mysqlnd_plugin_methods_xetters.error_info.set((m))

#endif	/* MYSQLND_EXT_PLUGIN_H */
