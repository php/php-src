/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2014 The PHP Group                                |
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

/* $Id: mysqlnd.c 306407 2010-12-16 12:56:19Z andrey $ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"

/*--------------------------------------------------------------------*/
#if defined(MYSQLND_DBG_ENABLED) && MYSQLND_DBG_ENABLED == 1
static enum_func_status mysqlnd_example_plugin_end(void * p TSRMLS_DC);

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
};


/* {{{ mysqlnd_example_plugin_end */
static 
enum_func_status mysqlnd_example_plugin_end(void * p TSRMLS_DC)
{
	struct st_mysqlnd_typeii_plugin_example * plugin = (struct st_mysqlnd_typeii_plugin_example *) p;
	DBG_ENTER("mysqlnd_example_plugin_end");
	mysqlnd_stats_end(plugin->plugin_header.plugin_stats.values);
	plugin->plugin_header.plugin_stats.values = NULL;
	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ mysqlnd_example_plugin_register */
void
mysqlnd_example_plugin_register(TSRMLS_D)
{
	mysqlnd_stats_init(&mysqlnd_plugin_example_stats, EXAMPLE_STAT_LAST);
	mysqlnd_example_plugin.plugin_header.plugin_stats.values = mysqlnd_plugin_example_stats;
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_example_plugin TSRMLS_CC);
}
/* }}} */
#endif /* defined(MYSQLND_DBG_ENABLED) && MYSQLND_DBG_ENABLED == 1 */
/*--------------------------------------------------------------------*/

static HashTable mysqlnd_registered_plugins;
		
static unsigned int mysqlnd_plugins_counter = 0;


/* {{{ mysqlnd_plugin_subsystem_init */
void
mysqlnd_plugin_subsystem_init(TSRMLS_D)
{
	zend_hash_init(&mysqlnd_registered_plugins, 4 /* initial hash size */, NULL /* hash_func */, NULL /* dtor */, TRUE /* pers */);
}
/* }}} */


/* {{{ mysqlnd_plugin_end_apply_func */
int
mysqlnd_plugin_end_apply_func(void *pDest TSRMLS_DC)
{
	struct st_mysqlnd_plugin_header * plugin_header = *(struct st_mysqlnd_plugin_header **) pDest;
	if (plugin_header->m.plugin_shutdown) {
		plugin_header->m.plugin_shutdown(plugin_header TSRMLS_CC);
	}
	return ZEND_HASH_APPLY_REMOVE;
}
/* }}} */


/* {{{ mysqlnd_plugin_subsystem_end */
void
mysqlnd_plugin_subsystem_end(TSRMLS_D)
{
	zend_hash_apply(&mysqlnd_registered_plugins, mysqlnd_plugin_end_apply_func TSRMLS_CC);
	zend_hash_destroy(&mysqlnd_registered_plugins);
}
/* }}} */


/* {{{ mysqlnd_plugin_register */
PHPAPI unsigned int mysqlnd_plugin_register()
{
	TSRMLS_FETCH();
	return mysqlnd_plugin_register_ex(NULL TSRMLS_CC);
}
/* }}} */


/* {{{ mysqlnd_plugin_register_ex */
PHPAPI unsigned int mysqlnd_plugin_register_ex(struct st_mysqlnd_plugin_header * plugin TSRMLS_DC)
{
	if (plugin) {
		if (plugin->plugin_api_version == MYSQLND_PLUGIN_API_VERSION) {
			zend_hash_update(&mysqlnd_registered_plugins, plugin->plugin_name, strlen(plugin->plugin_name) + 1, &plugin, sizeof(void *), NULL);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Plugin API version mismatch while loading plugin %s. Expected %d, got %d",
							plugin->plugin_name, MYSQLND_PLUGIN_API_VERSION, plugin->plugin_api_version);
			return 0xCAFE;
		}
	}
	return mysqlnd_plugins_counter++;
}
/* }}} */


/* {{{ mysqlnd_plugin_find */
PHPAPI void * _mysqlnd_plugin_find(const char * const name TSRMLS_DC)
{
	void * plugin;
	if (SUCCESS == zend_hash_find(&mysqlnd_registered_plugins, name, strlen(name) + 1, (void **) &plugin)) {
		return (void *)*(char **) plugin;
	}
	return NULL;

}
/* }}} */


/* {{{ _mysqlnd_plugin_apply_with_argument */
PHPAPI void _mysqlnd_plugin_apply_with_argument(apply_func_arg_t apply_func, void * argument TSRMLS_DC)
{
	/* Note: We want to be thread-safe (read-only), so we can use neither
	 * zend_hash_apply_with_argument nor zend_hash_internal_pointer_reset and
	 * friends
	 */
	Bucket *p;

	p = mysqlnd_registered_plugins.pListHead;
	while (p != NULL) {
		int result = apply_func(p->pData, argument TSRMLS_CC);

		if (result & ZEND_HASH_APPLY_REMOVE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mysqlnd_plugin_apply_with_argument must not remove table entries");
		}
		p = p->pListNext;
		if (result & ZEND_HASH_APPLY_STOP) {
			break;
		}
	}
}
/* }}} */


/* {{{ mysqlnd_plugin_count */
PHPAPI unsigned int mysqlnd_plugin_count()
{
	return mysqlnd_plugins_counter;
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
