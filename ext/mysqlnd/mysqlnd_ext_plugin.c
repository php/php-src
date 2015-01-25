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
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd.c 318221 2011-10-19 15:04:12Z andrey $ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_result.h"
#include "mysqlnd_debug.h"

static struct st_mysqlnd_conn_methods * mysqlnd_conn_methods;
static struct st_mysqlnd_conn_data_methods * mysqlnd_conn_data_methods;
static struct st_mysqlnd_stmt_methods * mysqlnd_stmt_methods;

/* {{{ _mysqlnd_plugin_get_plugin_connection_data */
PHPAPI void **
_mysqlnd_plugin_get_plugin_connection_data(const MYSQLND * conn, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_connection_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!conn || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)conn + sizeof(MYSQLND) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_connection_data_data */
PHPAPI void **
_mysqlnd_plugin_get_plugin_connection_data_data(const MYSQLND_CONN_DATA * conn, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_connection_data_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!conn || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)conn + sizeof(MYSQLND_CONN_DATA) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_result_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_result_data(const MYSQLND_RES * result, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_result_unbuffered_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_result_unbuffered_data(const MYSQLND_RES_UNBUFFERED * result, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_UNBUFFERED) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_result_buffered_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_result_buffered_data_zval(const MYSQLND_RES_BUFFERED_ZVAL * result, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_BUFFERED_ZVAL) + plugin_id * sizeof(void *)));
}
/* }}} */

/* {{{ _mysqlnd_plugin_get_plugin_result_buffered_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_result_buffered_data_c(const MYSQLND_RES_BUFFERED_C * result, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_result_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!result || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)result + sizeof(MYSQLND_RES_BUFFERED_C) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_protocol_data */
PHPAPI void **
_mysqlnd_plugin_get_plugin_protocol_data(const MYSQLND_PROTOCOL * protocol, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_protocol_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!protocol || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)protocol + sizeof(MYSQLND_PROTOCOL) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_stmt_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_stmt_data(const MYSQLND_STMT * stmt, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_stmt_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!stmt || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)stmt + sizeof(MYSQLND_STMT) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_net_data */
PHPAPI void ** _mysqlnd_plugin_get_plugin_net_data(const MYSQLND_NET * net, unsigned int plugin_id)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_net_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!net || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)net + sizeof(MYSQLND_NET) + plugin_id * sizeof(void *)));
}
/* }}} */



/* {{{ mysqlnd_conn_get_methods */
PHPAPI struct st_mysqlnd_conn_methods *
mysqlnd_conn_get_methods()
{
	return mysqlnd_conn_methods;
}
/* }}} */

/* {{{ mysqlnd_conn_set_methods */
PHPAPI void mysqlnd_conn_set_methods(struct st_mysqlnd_conn_methods *methods)
{
	mysqlnd_conn_methods = methods;
}
/* }}} */


/* {{{ mysqlnd_conn_get_methods */
PHPAPI struct st_mysqlnd_conn_data_methods *
mysqlnd_conn_data_get_methods()
{
	return mysqlnd_conn_data_methods;
}
/* }}} */

/* {{{ mysqlnd_conn_set_methods */
PHPAPI void mysqlnd_conn_data_set_methods(struct st_mysqlnd_conn_data_methods * methods)
{
	mysqlnd_conn_data_methods = methods;
}
/* }}} */


/* {{{ mysqlnd_result_get_methods */
PHPAPI struct st_mysqlnd_res_methods *
mysqlnd_result_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_res);
}
/* }}} */


/* {{{ mysqlnd_result_set_methods */
PHPAPI void
mysqlnd_result_set_methods(struct st_mysqlnd_res_methods * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_res) = *methods;
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered_get_methods */
PHPAPI struct st_mysqlnd_result_unbuffered_methods *
mysqlnd_result_unbuffered_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_unbuffered);
}
/* }}} */


/* {{{ mysqlnd_result_unbuffered_set_methods */
PHPAPI void
mysqlnd_result_unbuffered_set_methods(struct st_mysqlnd_result_unbuffered_methods * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_unbuffered) = *methods;
}
/* }}} */


/* {{{ mysqlnd_result_buffered_get_methods */
PHPAPI struct st_mysqlnd_result_buffered_methods *
mysqlnd_result_buffered_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_buffered);
}
/* }}} */


/* {{{ mysqlnd_result_buffered_set_methods */
PHPAPI void
mysqlnd_result_buffered_set_methods(struct st_mysqlnd_result_buffered_methods * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_result_buffered) = *methods;
}
/* }}} */


/* {{{ mysqlnd_stmt_get_methods */
PHPAPI struct st_mysqlnd_stmt_methods *
mysqlnd_stmt_get_methods()
{
	return mysqlnd_stmt_methods;
}
/* }}} */


/* {{{ mysqlnd_stmt_set_methods */
PHPAPI void
mysqlnd_stmt_set_methods(struct st_mysqlnd_stmt_methods *methods)
{
	mysqlnd_stmt_methods = methods;
}
/* }}} */


/* {{{ mysqlnd_protocol_get_methods */
PHPAPI struct st_mysqlnd_protocol_methods *
mysqlnd_protocol_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol);
}
/* }}} */


/* {{{ mysqlnd_protocol_set_methods */
PHPAPI void
mysqlnd_protocol_set_methods(struct st_mysqlnd_protocol_methods * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_protocol) = *methods;
}
/* }}} */


/* {{{ mysqlnd_net_get_methods */
PHPAPI struct st_mysqlnd_net_methods *
mysqlnd_net_get_methods()
{
	return &MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_net);
}
/* }}} */


/* {{{ mysqlnd_net_set_methods */
PHPAPI void
mysqlnd_net_set_methods(struct st_mysqlnd_net_methods * methods)
{
	MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_net) = *methods;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
