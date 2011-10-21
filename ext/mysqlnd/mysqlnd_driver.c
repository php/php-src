/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2011 The PHP Group                                |
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
		_mysqlnd_init_ps_subsystem();
		/* Should be calloc, as mnd_calloc will reference LOCK_access*/
		mysqlnd_stats_init(&mysqlnd_global_stats, STAT_LAST);
		mysqlnd_plugin_subsystem_init(TSRMLS_C);
		{
			mysqlnd_plugin_core.plugin_header.plugin_stats.values = mysqlnd_global_stats;
			mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_plugin_core TSRMLS_CC);
		}
		mysqlnd_example_plugin_register(TSRMLS_C);
		mysqlnd_debug_trace_plugin_register(TSRMLS_C);
		mysqlnd_register_builtin_authentication_plugins(TSRMLS_C);

		mysqlnd_reverse_api_init(TSRMLS_C);
	}
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
