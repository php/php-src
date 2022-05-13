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
#include "mysqlnd_priv.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"

/*--------------------------------------------------------------------*/
#if MYSQLND_DBG_ENABLED == 1
static enum_func_status mysqlnd_example_plugin_end(void * p);

static MYSQLND_STATS * mysqlnd_plugin_example_stats = NULL;

enum mysqlnd_plugin_example_collected_stats
{
	EXAMPLE_STAT1,
	EXAMPLE_STAT2,
	EXAMPLE_STAT_LAST
};

static const MYSQLND_STRING mysqlnd_plugin_example_stats_values_names[EXAMPLE_STAT_LAST] =
{
	{ MYSQLND_STR_W_LEN("stat1") },
	{ MYSQLND_STR_W_LEN("stat2") }
};

static struct st_mysqlnd_typeii_plugin_example mysqlnd_example_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"example",
		10001L,
		"1.00.01",
		"PHP License",
		"Andrey Hristov <andrey@php.net>",
		{
			NULL, /* will be filled later */
			mysqlnd_plugin_example_stats_values_names,
		},
		{
			mysqlnd_example_plugin_end
		}
	},
	NULL,	/* methods */
	0
};


/* {{{ mysqlnd_example_plugin_end */
static
enum_func_status mysqlnd_example_plugin_end(void * p)
{
	struct st_mysqlnd_typeii_plugin_example * plugin = (struct st_mysqlnd_typeii_plugin_example *) p;
	DBG_ENTER("mysqlnd_example_plugin_end");
	mysqlnd_stats_end(plugin->plugin_header.plugin_stats.values, 1);
	plugin->plugin_header.plugin_stats.values = NULL;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_example_plugin_register */
void
mysqlnd_example_plugin_register(void)
{
	mysqlnd_stats_init(&mysqlnd_plugin_example_stats, EXAMPLE_STAT_LAST, 1);
	mysqlnd_example_plugin.plugin_header.plugin_stats.values = mysqlnd_plugin_example_stats;
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_example_plugin);
}
/* }}} */
#endif /* MYSQLND_DBG_ENABLED == 1 */
/*--------------------------------------------------------------------*/

static HashTable mysqlnd_registered_plugins;

static unsigned int mysqlnd_plugins_counter = 0;


/* {{{ mysqlnd_plugin_subsystem_init */
void
mysqlnd_plugin_subsystem_init(void)
{
	zend_hash_init(&mysqlnd_registered_plugins, 4 /* initial hash size */, NULL /* hash_func */, NULL /* dtor */, TRUE /* pers */);
}
/* }}} */


/* {{{ mysqlnd_plugin_end_apply_func */
int
mysqlnd_plugin_end_apply_func(zval *el)
{
	struct st_mysqlnd_plugin_header * plugin_header = (struct st_mysqlnd_plugin_header *)Z_PTR_P(el);
	if (plugin_header->m.plugin_shutdown) {
		plugin_header->m.plugin_shutdown(plugin_header);
	}
	return ZEND_HASH_APPLY_REMOVE;
}
/* }}} */


/* {{{ mysqlnd_plugin_subsystem_end */
void
mysqlnd_plugin_subsystem_end(void)
{
	zend_hash_apply(&mysqlnd_registered_plugins, mysqlnd_plugin_end_apply_func);
	zend_hash_destroy(&mysqlnd_registered_plugins);
}
/* }}} */


/* {{{ mysqlnd_plugin_register */
PHPAPI unsigned int mysqlnd_plugin_register(void)
{
	return mysqlnd_plugin_register_ex(NULL);
}
/* }}} */


/* {{{ mysqlnd_plugin_register_ex */
PHPAPI unsigned int mysqlnd_plugin_register_ex(struct st_mysqlnd_plugin_header * plugin)
{
	if (plugin) {
		if (plugin->plugin_api_version == MYSQLND_PLUGIN_API_VERSION) {
			zend_hash_str_update_ptr(&mysqlnd_registered_plugins, plugin->plugin_name, strlen(plugin->plugin_name), plugin);
		} else {
			php_error_docref(NULL, E_WARNING, "Plugin API version mismatch while loading plugin %s. Expected %d, got %d",
							plugin->plugin_name, MYSQLND_PLUGIN_API_VERSION, plugin->plugin_api_version);
			return 0xCAFE;
		}
	}
	return mysqlnd_plugins_counter++;
}
/* }}} */


/* {{{ mysqlnd_plugin_find */
PHPAPI void * mysqlnd_plugin_find(const char * const name)
{
	void * plugin;
	if ((plugin = zend_hash_str_find_ptr(&mysqlnd_registered_plugins, name, strlen(name))) != NULL) {
		return plugin;
	}
	return NULL;
}
/* }}} */


/* {{{ mysqlnd_plugin_apply_with_argument */
PHPAPI void mysqlnd_plugin_apply_with_argument(apply_func_arg_t apply_func, void * argument)
{
	zval *val;
	int result;

	ZEND_HASH_MAP_FOREACH_VAL(&mysqlnd_registered_plugins, val) {
		result = apply_func(val, argument);
		if (result & ZEND_HASH_APPLY_REMOVE) {
			php_error_docref(NULL, E_WARNING, "mysqlnd_plugin_apply_with_argument must not remove table entries");
		}
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */


/* {{{ mysqlnd_plugin_count */
PHPAPI unsigned int mysqlnd_plugin_count(void)
{
	return mysqlnd_plugins_counter;
}
/* }}} */
