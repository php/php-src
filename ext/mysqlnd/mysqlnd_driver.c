/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2013 The PHP Group                                |
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
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd.c 317989 2011-10-10 20:49:28Z andrey $ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_charset.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_reverse_api.h"
#include "mysqlnd_ext_plugin.h"

static zend_bool mysqlnd_library_initted = FALSE;

static struct st_mysqlnd_plugin_core mysqlnd_plugin_core =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"mysqlnd",
		MYSQLND_VERSION_ID,
		MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@mysql.com>,  Ulf Wendel <uwendel@mysql.com>, Georg Richter <georg@mysql.com>",
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
PHPAPI void mysqlnd_library_end(TSRMLS_D)
{
	if (mysqlnd_library_initted == TRUE) {
		mysqlnd_plugin_subsystem_end(TSRMLS_C);
		mysqlnd_stats_end(mysqlnd_global_stats);
		mysqlnd_global_stats = NULL;
		mysqlnd_library_initted = FALSE;
		mysqlnd_reverse_api_end(TSRMLS_C);
	}
}
/* }}} */


/* {{{ mysqlnd_library_init */
PHPAPI void mysqlnd_library_init(TSRMLS_D)
{
	if (mysqlnd_library_initted == FALSE) {
		mysqlnd_library_initted = TRUE;
		mysqlnd_conn_set_methods(&MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_conn));
		mysqlnd_conn_data_set_methods(&MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_conn_data));
		_mysqlnd_init_ps_subsystem();
		/* Should be calloc, as mnd_calloc will reference LOCK_access*/
		mysqlnd_stats_init(&mysqlnd_global_stats, STAT_LAST);
		mysqlnd_plugin_subsystem_init(TSRMLS_C);
		{
			mysqlnd_plugin_core.plugin_header.plugin_stats.values = mysqlnd_global_stats;
			mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_plugin_core TSRMLS_CC);
		}
#if defined(MYSQLND_DBG_ENABLED) && MYSQLND_DBG_ENABLED == 1
		mysqlnd_example_plugin_register(TSRMLS_C);
#endif
		mysqlnd_debug_trace_plugin_register(TSRMLS_C);
		mysqlnd_register_builtin_authentication_plugins(TSRMLS_C);

		mysqlnd_reverse_api_init(TSRMLS_C);
	}
}
/* }}} */



/* {{{ mysqlnd_error_list_pdtor */
static void
mysqlnd_error_list_pdtor(void * pDest)
{
	MYSQLND_ERROR_LIST_ELEMENT * element = (MYSQLND_ERROR_LIST_ELEMENT *) pDest;
#ifdef ZTS
	TSRMLS_FETCH();
#endif
	DBG_ENTER("mysqlnd_error_list_pdtor");
	if (element->error) {
		mnd_pefree(element->error, TRUE);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_connection */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_object_factory, get_connection)(zend_bool persistent TSRMLS_DC)
{
	size_t alloc_size_ret = sizeof(MYSQLND) + mysqlnd_plugin_count() * sizeof(void *);
	size_t alloc_size_ret_data = sizeof(MYSQLND_CONN_DATA) + mysqlnd_plugin_count() * sizeof(void *);
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

	data->error_info = &(data->error_info_impl);
	data->options = &(data->options_impl);
	data->upsert_status = &(data->upsert_status_impl);

	data->persistent = persistent;
	data->m = mysqlnd_conn_data_get_methods();
	CONN_SET_STATE(data, CONN_ALLOCED);
	data->m->get_reference(data TSRMLS_CC);

	if (PASS != data->m->init(data TSRMLS_CC)) {
		new_object->m->dtor(new_object TSRMLS_CC);
		DBG_RETURN(NULL);
	}

	data->error_info->error_list = mnd_pecalloc(1, sizeof(zend_llist), persistent);
	if (!data->error_info->error_list) {
		new_object->m->dtor(new_object TSRMLS_CC);
		DBG_RETURN(NULL);
	} else {
		zend_llist_init(data->error_info->error_list, sizeof(MYSQLND_ERROR_LIST_ELEMENT), (llist_dtor_func_t)mysqlnd_error_list_pdtor, persistent);
	}

	DBG_RETURN(new_object);
}
/* }}} */


/* {{{ mysqlnd_object_factory::clone_connection_object */
static MYSQLND *
MYSQLND_METHOD(mysqlnd_object_factory, clone_connection_object)(MYSQLND * to_be_cloned TSRMLS_DC)
{
	size_t alloc_size_ret = sizeof(MYSQLND) + mysqlnd_plugin_count() * sizeof(void *);
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

	new_object->data = to_be_cloned->data->m->get_reference(to_be_cloned->data TSRMLS_CC);
	if (!new_object->data) {
		new_object->m->dtor(new_object TSRMLS_CC);
		new_object = NULL;
	}
	DBG_RETURN(new_object);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_prepared_statement */
static MYSQLND_STMT *
MYSQLND_METHOD(mysqlnd_object_factory, get_prepared_statement)(MYSQLND_CONN_DATA * const conn TSRMLS_DC)
{
	size_t alloc_size = sizeof(MYSQLND_STMT) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_STMT * ret = mnd_pecalloc(1, alloc_size, conn->persistent);
	MYSQLND_STMT_DATA * stmt = NULL;

	DBG_ENTER("mysqlnd_object_factory::get_prepared_statement");
	do {
		if (!ret) {
			break;
		}
		ret->m = mysqlnd_stmt_get_methods();
		ret->persistent = conn->persistent;

		stmt = ret->data = mnd_pecalloc(1, sizeof(MYSQLND_STMT_DATA), conn->persistent);
		DBG_INF_FMT("stmt=%p", stmt);
		if (!stmt) {
			break;
		}
		stmt->persistent = conn->persistent;
		stmt->error_info = &(stmt->error_info_impl);
		stmt->upsert_status = &(stmt->upsert_status_impl);
		stmt->state = MYSQLND_STMT_INITTED;
		stmt->execute_cmd_buffer.length = 4096;
		stmt->execute_cmd_buffer.buffer = mnd_pemalloc(stmt->execute_cmd_buffer.length, stmt->persistent);
		if (!stmt->execute_cmd_buffer.buffer) {
			break;
		}

		stmt->prefetch_rows = MYSQLND_DEFAULT_PREFETCH_ROWS;
		/*
		  Mark that we reference the connection, thus it won't be
		  be destructed till there is open statements. The last statement
		  or normal query result will close it then.
		*/
		stmt->conn = conn->m->get_reference(conn TSRMLS_CC);
		stmt->error_info->error_list = mnd_pecalloc(1, sizeof(zend_llist), ret->persistent);
		if (!stmt->error_info->error_list) {
			break;
		}

		zend_llist_init(stmt->error_info->error_list, sizeof(MYSQLND_ERROR_LIST_ELEMENT), (llist_dtor_func_t) mysqlnd_error_list_pdtor, conn->persistent);

		DBG_RETURN(ret);
	} while (0);

	SET_OOM_ERROR(*conn->error_info);
	if (ret) {
		ret->m->dtor(ret, TRUE TSRMLS_CC);
		ret = NULL;
	}
	DBG_RETURN(NULL);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_io_channel */
PHPAPI MYSQLND_NET *
MYSQLND_METHOD(mysqlnd_object_factory, get_io_channel)(zend_bool persistent, MYSQLND_STATS * stats, MYSQLND_ERROR_INFO * error_info TSRMLS_DC)
{
	size_t net_alloc_size = sizeof(MYSQLND_NET) + mysqlnd_plugin_count() * sizeof(void *);
	size_t net_data_alloc_size = sizeof(MYSQLND_NET_DATA) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_NET * net = mnd_pecalloc(1, net_alloc_size, persistent);
	MYSQLND_NET_DATA * net_data = mnd_pecalloc(1, net_data_alloc_size, persistent);

	DBG_ENTER("mysqlnd_object_factory::get_io_channel");
	DBG_INF_FMT("persistent=%u", persistent);
	if (net && net_data) {
		net->data = net_data;
		net->persistent = net->data->persistent = persistent;
		net->data->m = *mysqlnd_net_get_methods();

		if (PASS != net->data->m.init(net, stats, error_info TSRMLS_CC)) {
			net->data->m.dtor(net, stats, error_info TSRMLS_CC);
			net = NULL;
		}
	} else {
		if (net_data) {
			mnd_pefree(net_data, persistent);
			net_data = NULL;
		}
		if (net) {
			mnd_pefree(net, persistent);
			net = NULL;
		}
	}
	DBG_RETURN(net);
}
/* }}} */


/* {{{ mysqlnd_object_factory::get_protocol_decoder */
static MYSQLND_PROTOCOL *
MYSQLND_METHOD(mysqlnd_object_factory, get_protocol_decoder)(zend_bool persistent TSRMLS_DC)
{
	size_t alloc_size = sizeof(MYSQLND_PROTOCOL) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_PROTOCOL *ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_object_factory::get_protocol_decoder");
	DBG_INF_FMT("persistent=%u", persistent);
	if (ret) {
		ret->persistent = persistent;
		ret->m = mysqlnd_mysqlnd_protocol_methods;
	}

	DBG_RETURN(ret);
}
/* }}} */


PHPAPI MYSQLND_CLASS_METHODS_START(mysqlnd_object_factory)
	MYSQLND_METHOD(mysqlnd_object_factory, get_connection),
	MYSQLND_METHOD(mysqlnd_object_factory, clone_connection_object),
	MYSQLND_METHOD(mysqlnd_object_factory, get_prepared_statement),
	MYSQLND_METHOD(mysqlnd_object_factory, get_io_channel),
	MYSQLND_METHOD(mysqlnd_object_factory, get_protocol_decoder)
MYSQLND_CLASS_METHODS_END;

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
