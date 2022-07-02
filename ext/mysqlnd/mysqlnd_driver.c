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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_vio.h"
#include "mysqlnd_protocol_frame_codec.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_connection.h"
#include "mysqlnd_ps.h"
#include "mysqlnd_plugin.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_reverse_api.h"
#include "mysqlnd_ext_plugin.h"

static bool mysqlnd_library_initted = FALSE;

static struct st_mysqlnd_plugin_core mysqlnd_plugin_core =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"mysqlnd",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@php.net>,  Ulf Wendel <uw@php.net>, Georg Richter <georg@php.net>",
		{
			NULL, /* will be filled later */
			mysqlnd_stats_values_names,
		},
		{
			NULL /* plugin shutdown */
		}
	}
};


/* {{{ mysqlnd_library_end */
PHPAPI void mysqlnd_library_end(void)
{
	if (mysqlnd_library_initted == TRUE) {
		mysqlnd_plugin_subsystem_end();
		mysqlnd_stats_end(mysqlnd_global_stats, 1);
		mysqlnd_global_stats = NULL;
		mysqlnd_library_initted = FALSE;
		mysqlnd_reverse_api_end();
	}
}
/* }}} */


/* {{{ mysqlnd_library_init */
PHPAPI void mysqlnd_library_init(void)
{
	if (mysqlnd_library_initted == FALSE) {
		mysqlnd_library_initted = TRUE;
		mysqlnd_conn_set_methods(&MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_conn));
		mysqlnd_conn_data_set_methods(&MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_conn_data));
		_mysqlnd_init_ps_subsystem();
		/* Should be calloc, as mnd_calloc will reference LOCK_access*/
		mysqlnd_stats_init(&mysqlnd_global_stats, STAT_LAST, 1);
		mysqlnd_plugin_subsystem_init();
		{
			mysqlnd_plugin_core.plugin_header.plugin_stats.values = mysqlnd_global_stats;
			mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_plugin_core);
		}
#if MYSQLND_DBG_ENABLED == 1
		mysqlnd_example_plugin_register();
#endif
		mysqlnd_debug_trace_plugin_register();
		mysqlnd_register_builtin_authentication_plugins();

		mysqlnd_reverse_api_init();
	}
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_connection */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_object_factory, get_connection)(MYSQLND_CLASS_METHODS_TYPE(mysqlnd_object_factory) *factory, const bool persistent)
{
	const size_t alloc_size_ret = sizeof(MYSQLND) + mysqlnd_plugin_count() * sizeof(void *);
	const size_t alloc_size_ret_data = sizeof(MYSQLND_CONN_DATA) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND * new_object;
	MYSQLND_CONN_DATA * data;

	DBG_ENTER("mysqlnd_driver::get_connection");
	DBG_INF_FMT("persistent=%u", persistent);
	new_object = mnd_pecalloc(1, alloc_size_ret, persistent);
	if (!new_object) {
		DBG_RETURN(NULL);
	}
	new_object->data = mnd_pecalloc(1, alloc_size_ret_data, persistent);
	if (!new_object->data) {
		mnd_pefree(new_object, persistent);
		DBG_RETURN(NULL);
	}
	new_object->persistent = persistent;
	new_object->m = mysqlnd_conn_get_methods();
	data = new_object->data;

	mysqlnd_error_info_init(&data->error_info_impl, persistent);
	data->error_info = &data->error_info_impl;

	data->options = &(data->options_impl);

	mysqlnd_upsert_status_init(&data->upsert_status_impl);
	data->upsert_status = &(data->upsert_status_impl);
	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(data->upsert_status);

	data->persistent = persistent;
	data->m = mysqlnd_conn_data_get_methods();
	data->object_factory = *factory;

	mysqlnd_connection_state_init(&data->state);

	data->m->get_reference(data);

	mysqlnd_stats_init(&data->stats, STAT_LAST, persistent);

	data->protocol_frame_codec = mysqlnd_pfc_init(persistent, factory, data->stats, data->error_info);
	data->vio = mysqlnd_vio_init(persistent, factory, data->stats, data->error_info);
	data->payload_decoder_factory = mysqlnd_protocol_payload_decoder_factory_init(data, persistent);
	data->command = mysqlnd_command_get_methods();

	if (!data->protocol_frame_codec || !data->vio || !data->payload_decoder_factory || !data->command) {
		new_object->m->dtor(new_object);
		DBG_RETURN(NULL);
	}

	DBG_RETURN(new_object);
}
/* }}} */


/* {{{ mysqlnd_object_factory::clone_connection_object */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_object_factory, clone_connection_object)(MYSQLND * to_be_cloned)
{
	const size_t alloc_size_ret = sizeof(MYSQLND) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND * new_object;

	DBG_ENTER("mysqlnd_driver::clone_connection_object");
	DBG_INF_FMT("persistent=%u", to_be_cloned->persistent);
	if (!to_be_cloned || !to_be_cloned->data) {
		DBG_RETURN(NULL);
	}
	new_object = mnd_pecalloc(1, alloc_size_ret, to_be_cloned->persistent);
	if (!new_object) {
		DBG_RETURN(NULL);
	}
	new_object->persistent = to_be_cloned->persistent;
	new_object->m = to_be_cloned->m;

	new_object->data = to_be_cloned->data->m->get_reference(to_be_cloned->data);
	if (!new_object->data) {
		new_object->m->dtor(new_object);
		new_object = NULL;
	}
	DBG_RETURN(new_object);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_prepared_statement */
static MYSQLND_STMT *
MYSQLND_METHOD(mysqlnd_object_factory, get_prepared_statement)(MYSQLND_CONN_DATA * const conn)
{
	const size_t alloc_size = sizeof(MYSQLND_STMT) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_STMT * ret = mnd_ecalloc(1, alloc_size);
	MYSQLND_STMT_DATA * stmt = NULL;

	DBG_ENTER("mysqlnd_object_factory::get_prepared_statement");
	ret->m = mysqlnd_stmt_get_methods();

	stmt = ret->data = mnd_ecalloc(1, sizeof(MYSQLND_STMT_DATA));
	DBG_INF_FMT("stmt=%p", stmt);

	mysqlnd_error_info_init(&stmt->error_info_impl, 0);
	stmt->error_info = &stmt->error_info_impl;

	mysqlnd_upsert_status_init(&stmt->upsert_status_impl);
	stmt->upsert_status = &(stmt->upsert_status_impl);
	stmt->state = MYSQLND_STMT_INITTED;
	stmt->execute_cmd_buffer.length = 4096;
	stmt->execute_cmd_buffer.buffer = mnd_emalloc(stmt->execute_cmd_buffer.length);
	stmt->prefetch_rows = MYSQLND_DEFAULT_PREFETCH_ROWS;

	/*
	  Mark that we reference the connection, thus it won't be
	  be destructed till there is open statements. The last statement
	  or normal query result will close it then.
	*/
	stmt->conn = conn->m->get_reference(conn);

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_pfc */
static MYSQLND_PFC *
MYSQLND_METHOD(mysqlnd_object_factory, get_pfc)(const bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	const size_t pfc_alloc_size = ZEND_MM_ALIGNED_SIZE(sizeof(MYSQLND_PFC) + mysqlnd_plugin_count() * sizeof(void *));
	const size_t pfc_data_alloc_size = sizeof(MYSQLND_PFC_DATA) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_PFC * pfc = mnd_pecalloc(1, pfc_alloc_size + pfc_data_alloc_size, persistent);

	DBG_ENTER("mysqlnd_object_factory::get_pfc");
	DBG_INF_FMT("persistent=%u", persistent);
	if (pfc) {
		pfc->data = (MYSQLND_PFC_DATA*)((char*)pfc + pfc_alloc_size);
		pfc->persistent = pfc->data->persistent = persistent;
		pfc->data->m = *mysqlnd_pfc_get_methods();

		pfc->data->m.init(pfc, stats, error_info);
	}
	DBG_RETURN(pfc);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_vio */
static MYSQLND_VIO *
MYSQLND_METHOD(mysqlnd_object_factory, get_vio)(const bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info)
{
	const size_t vio_alloc_size = ZEND_MM_ALIGNED_SIZE(sizeof(MYSQLND_VIO) + mysqlnd_plugin_count() * sizeof(void *));
	const size_t vio_data_alloc_size = sizeof(MYSQLND_VIO_DATA) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_VIO * vio = mnd_pecalloc(1, vio_alloc_size + vio_data_alloc_size, persistent);

	DBG_ENTER("mysqlnd_object_factory::get_vio");
	DBG_INF_FMT("persistent=%u", persistent);
	if (vio) {
		vio->data = (MYSQLND_VIO_DATA*)((char*)vio + vio_alloc_size);
		vio->persistent = vio->data->persistent = persistent;
		vio->data->m = *mysqlnd_vio_get_methods();

		vio->data->m.init(vio, stats, error_info);
	}
	DBG_RETURN(vio);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_protocol_payload_decoder_factory */
static MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY *
MYSQLND_METHOD(mysqlnd_object_factory, get_protocol_payload_decoder_factory)(MYSQLND_CONN_DATA * conn, const bool persistent)
{
	const size_t alloc_size = sizeof(MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY *ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_object_factory::get_protocol_payload_decoder_factory");
	DBG_INF_FMT("persistent=%u", persistent);
	if (ret) {
		ret->persistent = persistent;
		ret->conn = conn;
		ret->m = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol_payload_decoder_factory);
	}

	DBG_RETURN(ret);
}
/* }}} */


PHPAPI MYSQLND_CLASS_METHODS_START(mysqlnd_object_factory)
	MYSQLND_METHOD(mysqlnd_object_factory, get_connection),
	MYSQLND_METHOD(mysqlnd_object_factory, clone_connection_object),
	MYSQLND_METHOD(mysqlnd_object_factory, get_prepared_statement),
	MYSQLND_METHOD(mysqlnd_object_factory, get_pfc),
	MYSQLND_METHOD(mysqlnd_object_factory, get_vio),
	MYSQLND_METHOD(mysqlnd_object_factory, get_protocol_payload_decoder_factory)
MYSQLND_CLASS_METHODS_END;
