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

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_commands.h"
#include "mysqlnd_ext_plugin.h"

static struct st_mysqlnd_conn_methods * mysqlnd_conn_methods;
static struct st_mysqlnd_conn_data_methods * mysqlnd_conn_data_methods;
static struct st_mysqlnd_stmt_methods * mysqlnd_stmt_methods;

/* {{{ mysqlnd_plugin__get_plugin_connection_data */
static void **
mysqlnd_plugin__get_plugin_connection_data(const MYSQLND * conn, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_connection_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!conn || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)conn + sizeof(MYSQLND) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_plugin__get_plugin_connection_data_data */
static void **
mysqlnd_plugin__get_plugin_connection_data_data(const MYSQLND_CONN_DATA * conn, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_connection_data_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!conn || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)conn + sizeof(MYSQLND_CONN_DATA) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_plugin__get_plugin_result_data */
static void **
mysqlnd_plugin__get_plugin_result_data(const MYSQLND_RES * result, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin__get_plugin_result_unbuffered_data */
static void **
mysqlnd_plugin__get_plugin_result_unbuffered_data(const MYSQLND_RES_UNBUFFERED * result, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_UNBUFFERED) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin__get_plugin_result_buffered_data */
static void **
mysqlnd_plugin__get_plugin_result_buffered_data_zval(const MYSQLND_RES_BUFFERED_ZVAL * result, const unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin__get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_BUFFERED_ZVAL) + plugin_id * sizeof(void *)));
}
/* }}} */

/* {{{ mysqlnd_plugin__get_plugin_result_buffered_data */
static void **
mysqlnd_plugin__get_plugin_result_buffered_data_c(const MYSQLND_RES_BUFFERED_C * result, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_BUFFERED_C) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_plugin__get_plugin_protocol_data */
static void **
mysqlnd_plugin__get_plugin_protocol_data(const MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * factory, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_protocol_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!factory || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)factory + sizeof(MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_plugin__get_plugin_stmt_data */
static void **
mysqlnd_plugin__get_plugin_stmt_data(const MYSQLND_STMT * stmt, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_stmt_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!stmt || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)stmt + sizeof(MYSQLND_STMT) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_plugin__get_plugin_pfc_data */
static void **
mysqlnd_plugin__get_plugin_pfc_data(const MYSQLND_PFC * pfc, const unsigned int plugin_id)
{
	DBG_ENTER("mysqlnd_plugin__get_plugin_pfc_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!pfc || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)pfc + sizeof(MYSQLND_PFC) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin__get_plugin_vio_data */
static void **
mysqlnd_plugin__get_plugin_vio_data(const MYSQLND_VIO * vio, const unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin__get_plugin_vio_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!vio || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)vio + sizeof(MYSQLND_VIO) + plugin_id * sizeof(void *)));
}
/* }}} */

struct st_mysqlnd_plugin__plugin_area_getters mysqlnd_plugin_area_getters =
{
	mysqlnd_plugin__get_plugin_connection_data,
	mysqlnd_plugin__get_plugin_connection_data_data,
	mysqlnd_plugin__get_plugin_result_data,
	mysqlnd_plugin__get_plugin_result_unbuffered_data,
	mysqlnd_plugin__get_plugin_result_buffered_data_zval,
	mysqlnd_plugin__get_plugin_result_buffered_data_c,
	mysqlnd_plugin__get_plugin_stmt_data,
	mysqlnd_plugin__get_plugin_protocol_data,
	mysqlnd_plugin__get_plugin_pfc_data,
	mysqlnd_plugin__get_plugin_vio_data,
};



/* {{{ _mysqlnd_object_factory_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *
_mysqlnd_object_factory_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory);
}
/* }}} */

/* {{{ mysqlnd_conn_set_methods */
static void
_mysqlnd_object_factory_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_conn_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn) *
_mysqlnd_conn_get_methods()
{
	return mysqlnd_conn_methods;
}
/* }}} */

/* {{{ _mysqlnd_conn_set_methods */
static void
_mysqlnd_conn_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn) *methods)
{
	mysqlnd_conn_methods = methods;
}
/* }}} */


/* {{{ _mysqlnd_conn_data_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data) *
_mysqlnd_conn_data_get_methods()
{
	return mysqlnd_conn_data_methods;
}
/* }}} */

/* {{{ _mysqlnd_conn_data_set_methods */
static void
_mysqlnd_conn_data_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_conn_data) * methods)
{
	mysqlnd_conn_data_methods = methods;
}
/* }}} */


/* {{{ _mysqlnd_result_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res) *
_mysqlnd_result_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_res);
}
/* }}} */


/* {{{ _mysqlnd_result_set_methods */
static void
_mysqlnd_result_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_res) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_res) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_result_unbuffered_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered) *
_mysqlnd_result_unbuffered_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_unbuffered);
}
/* }}} */


/* {{{ _mysqlnd_result_unbuffered_set_methods */
static void
_mysqlnd_result_unbuffered_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_unbuffered) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_unbuffered) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_result_buffered_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered) *
_mysqlnd_result_buffered_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_buffered);
}
/* }}} */


/* {{{ _mysqlnd_result_buffered_set_methods */
static void
_mysqlnd_result_buffered_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_result_buffered) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_buffered) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_stmt_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt) *
_mysqlnd_stmt_get_methods()
{
	return mysqlnd_stmt_methods;
}
/* }}} */


/* {{{ _mysqlnd_stmt_set_methods */
static void
_mysqlnd_stmt_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_stmt) *methods)
{
	mysqlnd_stmt_methods = methods;
}
/* }}} */


/* {{{ _mysqlnd_protocol_payload_decoder_factory_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory) *
_mysqlnd_protocol_payload_decoder_factory_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol_payload_decoder_factory);
}
/* }}} */


/* {{{ _mysqlnd_protocol_payload_decoder_factory_set_methods */
static void
_mysqlnd_protocol_payload_decoder_factory_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_payload_decoder_factory) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol_payload_decoder_factory) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_pfc_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec) *
_mysqlnd_pfc_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol_packet_frame_codec);
}
/* }}} */


/* {{{ _mysqlnd_pfc_set_methods */
static void
_mysqlnd_pfc_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_protocol_packet_frame_codec) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol_packet_frame_codec) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_vio_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio) *
_mysqlnd_vio_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_vio);
}
/* }}} */


/* {{{ _mysqlnd_vio_set_methods */
static void
_mysqlnd_vio_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_vio) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_vio) = *methods;
}
/* }}} */


/* {{{ mysqlnd_command_factory_get */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command) *
_mysqlnd_command_factory_get()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_command);
}
/* }}} */


/* {{{ mysqlnd_command_factory_set */
static void
_mysqlnd_command_factory_set(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_command) * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_command) = *methods;
}
/* }}} */


/* {{{ _mysqlnd_error_info_get_methods */
static MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info) *
_mysqlnd_error_info_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_error_info);
}
/* }}} */


/* {{{ _mysqlnd_error_info_set_methods */
static void
_mysqlnd_error_info_set_methods(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_error_info) *methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_error_info) = *methods;
}
/* }}} */


struct st_mysqlnd_plugin_methods_xetters mysqlnd_plugin_methods_xetters =
{
	{
		_mysqlnd_object_factory_get_methods,
		_mysqlnd_object_factory_set_methods
	},
	{
		_mysqlnd_conn_get_methods,
		_mysqlnd_conn_set_methods,
	},
	{
		_mysqlnd_conn_data_get_methods,
		_mysqlnd_conn_data_set_methods,
	},
	{
		_mysqlnd_result_get_methods,
		_mysqlnd_result_set_methods,
	},
	{
		_mysqlnd_result_unbuffered_get_methods,
		_mysqlnd_result_unbuffered_set_methods,
	},
	{
		_mysqlnd_result_buffered_get_methods,
		_mysqlnd_result_buffered_set_methods,
	},
	{
		_mysqlnd_stmt_get_methods,
		_mysqlnd_stmt_set_methods,
	},
	{
		_mysqlnd_protocol_payload_decoder_factory_get_methods,
		_mysqlnd_protocol_payload_decoder_factory_set_methods,
	},
	{
		_mysqlnd_pfc_get_methods,
		_mysqlnd_pfc_set_methods,
	},
	{
		_mysqlnd_vio_get_methods,
		_mysqlnd_vio_set_methods,
	},
	{
		_mysqlnd_error_info_get_methods,
		_mysqlnd_error_info_set_methods,
	},
	{
		_mysqlnd_command_factory_get,
		_mysqlnd_command_factory_set,
	},
};
