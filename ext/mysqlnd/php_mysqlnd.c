/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2017 The PHP Group                                |
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

#include "php.h"
#include "php_ini.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_reverse_api.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"

/* {{{ mysqlnd_functions[]
 *
 * Every user visible function must have an entry in mysqlnd_functions[].
 */
static zend_function_entry mysqlnd_functions[] = {
	PHP_FE_END
};
/* }}} */


/* {{{ mysqlnd_minfo_print_hash */
PHPAPI void
mysqlnd_minfo_print_hash(zval *values)
{
	zval *values_entry;
	zend_string	*string_key;

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(values), string_key, values_entry) {
		convert_to_string(values_entry);
		php_info_print_table_row(2, ZSTR_VAL(string_key), Z_STRVAL_P(values_entry));
	} ZEND_HASH_FOREACH_END();
}
/* }}} */


/* {{{ mysqlnd_minfo_dump_plugin_stats */
static int
mysqlnd_minfo_dump_plugin_stats(zval *el, void * argument)
{
	struct st_mysqlnd_plugin_header * plugin_header = (struct st_mysqlnd_plugin_header *)Z_PTR_P(el);
	if (plugin_header->plugin_stats.values) {
		char buf[64];
		zval values;
		snprintf(buf, sizeof(buf), "%s statistics", plugin_header->plugin_name);

		mysqlnd_fill_stats_hash(plugin_header->plugin_stats.values, plugin_header->plugin_stats.names, &values ZEND_FILE_LINE_CC);

		php_info_print_table_start();
		php_info_print_table_header(2, buf, "");
		mysqlnd_minfo_print_hash(&values);
		php_info_print_table_end();
		zval_dtor(&values);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */


/* {{{ mysqlnd_minfo_dump_loaded_plugins */
static int
mysqlnd_minfo_dump_loaded_plugins(zval *el, void * buf)
{
	smart_str * buffer = (smart_str *) buf;
	struct st_mysqlnd_plugin_header * plugin_header = (struct st_mysqlnd_plugin_header *)Z_PTR_P(el);
	if (plugin_header->plugin_name) {
		if (buffer->s) {
			smart_str_appendc(buffer, ',');
		}
		smart_str_appends(buffer, plugin_header->plugin_name);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */


/* {{{ mysqlnd_minfo_dump_api_plugins */
static void
mysqlnd_minfo_dump_api_plugins(smart_str * buffer)
{
	HashTable *ht = mysqlnd_reverse_api_get_api_list();
	MYSQLND_REVERSE_API *ext;

	ZEND_HASH_FOREACH_PTR(ht, ext) {
		if (buffer->s) {
			smart_str_appendc(buffer, ',');
		}
		smart_str_appends(buffer, ext->module->name);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysqlnd)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "mysqlnd", "enabled");
	php_info_print_table_row(2, "Version", mysqlnd_get_client_info());
	php_info_print_table_row(2, "Compression",
#ifdef MYSQLND_COMPRESSION_ENABLED
								"supported");
#else
								"not supported");
#endif
	php_info_print_table_row(2, "core SSL",
#ifdef MYSQLND_SSL_SUPPORTED
								"supported");
#else
								"not supported");
#endif
	php_info_print_table_row(2, "extended SSL",
#ifdef MYSQLND_HAVE_SSL
								"supported");
#else
								"not supported");
#endif
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MYSQLND_G(net_cmd_buffer_size));
	php_info_print_table_row(2, "Command buffer size", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MYSQLND_G(net_read_buffer_size));
	php_info_print_table_row(2, "Read buffer size", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, MYSQLND_G(net_read_timeout));
	php_info_print_table_row(2, "Read timeout", buf);
	php_info_print_table_row(2, "Collecting statistics", MYSQLND_G(collect_statistics)? "Yes":"No");
	php_info_print_table_row(2, "Collecting memory statistics", MYSQLND_G(collect_memory_statistics)? "Yes":"No");

	php_info_print_table_row(2, "Tracing", MYSQLND_G(debug)? MYSQLND_G(debug):"n/a");

	/* loaded plugins */
	{
		smart_str tmp_str = {0};
		mysqlnd_plugin_apply_with_argument(mysqlnd_minfo_dump_loaded_plugins, &tmp_str);
		smart_str_0(&tmp_str);
		php_info_print_table_row(2, "Loaded plugins", tmp_str.s? ZSTR_VAL(tmp_str.s) : "");
		smart_str_free(&tmp_str);

		mysqlnd_minfo_dump_api_plugins(&tmp_str);
		smart_str_0(&tmp_str);
		php_info_print_table_row(2, "API Extensions", tmp_str.s? ZSTR_VAL(tmp_str.s) : "");
		smart_str_free(&tmp_str);
	}

	php_info_print_table_end();


	/* Print client stats */
	mysqlnd_plugin_apply_with_argument(mysqlnd_minfo_dump_plugin_stats, NULL);
}
/* }}} */


PHPAPI ZEND_DECLARE_MODULE_GLOBALS(mysqlnd)


/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(mysqlnd)
{
#if defined(COMPILE_DL_MYSQLND) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	mysqlnd_globals->collect_statistics = TRUE;
	mysqlnd_globals->collect_memory_statistics = FALSE;
	mysqlnd_globals->debug = NULL;	/* The actual string */
	mysqlnd_globals->dbg = NULL;	/* The DBG object*/
	mysqlnd_globals->trace_alloc_settings = NULL;
	mysqlnd_globals->trace_alloc = NULL;
	mysqlnd_globals->net_cmd_buffer_size = MYSQLND_NET_CMD_BUFFER_MIN_SIZE;
	mysqlnd_globals->net_read_buffer_size = 32768;
	mysqlnd_globals->net_read_timeout = 31536000;
	mysqlnd_globals->log_mask = 0;
	mysqlnd_globals->mempool_default_size = 16000;
	mysqlnd_globals->debug_emalloc_fail_threshold = -1;
	mysqlnd_globals->debug_ecalloc_fail_threshold = -1;
	mysqlnd_globals->debug_erealloc_fail_threshold = -1;
	mysqlnd_globals->debug_malloc_fail_threshold = -1;
	mysqlnd_globals->debug_calloc_fail_threshold = -1;
	mysqlnd_globals->debug_realloc_fail_threshold = -1;
	mysqlnd_globals->sha256_server_public_key = NULL;
	mysqlnd_globals->fetch_data_copy = FALSE;
}
/* }}} */


/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateNetCmdBufferSize)
{
	zend_long long_value;

	ZEND_ATOL(long_value, ZSTR_VAL(new_value));
	if (long_value < MYSQLND_NET_CMD_BUFFER_MIN_SIZE) {
		return FAILURE;
	}
	MYSQLND_G(net_cmd_buffer_size) = long_value;

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_INI_BEGIN
*/
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("mysqlnd.collect_statistics",	"1", 	PHP_INI_ALL,	OnUpdateBool,	collect_statistics, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_BOOLEAN("mysqlnd.collect_memory_statistics","0",PHP_INI_SYSTEM, OnUpdateBool,	collect_memory_statistics, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.debug",					NULL, 	PHP_INI_SYSTEM, OnUpdateString,	debug, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.trace_alloc",			NULL, 	PHP_INI_SYSTEM, OnUpdateString,	trace_alloc_settings, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.net_cmd_buffer_size",	MYSQLND_NET_CMD_BUFFER_MIN_SIZE_STR,	PHP_INI_ALL,	OnUpdateNetCmdBufferSize,	net_cmd_buffer_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.net_read_buffer_size",	"32768",PHP_INI_ALL,	OnUpdateLong,	net_read_buffer_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.net_read_timeout",	"31536000",	PHP_INI_SYSTEM, OnUpdateLong,	net_read_timeout, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.log_mask",				"0", 	PHP_INI_ALL,	OnUpdateLong,	log_mask, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.mempool_default_size","16000",   PHP_INI_ALL,	OnUpdateLong,	mempool_default_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.sha256_server_public_key",NULL, 	PHP_INI_PERDIR, OnUpdateString,	sha256_server_public_key, zend_mysqlnd_globals, mysqlnd_globals)
	STD_PHP_INI_BOOLEAN("mysqlnd.fetch_data_copy",	"0", 		PHP_INI_ALL,	OnUpdateBool,	fetch_data_copy, zend_mysqlnd_globals, mysqlnd_globals)
#if PHP_DEBUG
	STD_PHP_INI_ENTRY("mysqlnd.debug_emalloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_emalloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.debug_ecalloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_ecalloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.debug_erealloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_erealloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)

	STD_PHP_INI_ENTRY("mysqlnd.debug_malloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_malloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.debug_calloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_calloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_PHP_INI_ENTRY("mysqlnd.debug_realloc_fail_threshold","-1",   PHP_INI_SYSTEM,	OnUpdateLong,	debug_realloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
#endif
PHP_INI_END()
/* }}} */


/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(mysqlnd)
{
	REGISTER_INI_ENTRIES();

	mysqlnd_library_init();
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(mysqlnd)
{
	mysqlnd_library_end();

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */


#if PHP_DEBUG
/* {{{ PHP_RINIT_FUNCTION
 */
static PHP_RINIT_FUNCTION(mysqlnd)
{
	if (MYSQLND_G(debug)) {
		struct st_mysqlnd_plugin_trace_log * trace_log_plugin = mysqlnd_plugin_find("debug_trace");
		MYSQLND_G(dbg) = NULL;
		if (trace_log_plugin) {
			MYSQLND_DEBUG * dbg = trace_log_plugin->methods.trace_instance_init(mysqlnd_debug_std_no_trace_funcs);
			MYSQLND_DEBUG * trace_alloc = trace_log_plugin->methods.trace_instance_init(NULL);
			if (!dbg || !trace_alloc) {
				return FAILURE;
			}
			dbg->m->set_mode(dbg, MYSQLND_G(debug));
			trace_alloc->m->set_mode(trace_alloc, MYSQLND_G(trace_alloc_settings));
			MYSQLND_G(dbg) = dbg;
			MYSQLND_G(trace_alloc) = trace_alloc;
		}
	}
	return SUCCESS;
}
/* }}} */
#endif


#if PHP_DEBUG
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
static PHP_RSHUTDOWN_FUNCTION(mysqlnd)
{
	MYSQLND_DEBUG * dbg = MYSQLND_G(dbg);
	MYSQLND_DEBUG * trace_alloc = MYSQLND_G(trace_alloc);
	DBG_ENTER("RSHUTDOWN");
	if (dbg) {
		dbg->m->close(dbg);
		dbg->m->free_handle(dbg);
		MYSQLND_G(dbg) = NULL;
	}
	if (trace_alloc) {
		trace_alloc->m->close(trace_alloc);
		trace_alloc->m->free_handle(trace_alloc);
		MYSQLND_G(trace_alloc) = NULL;
	}
	return SUCCESS;
}
/* }}} */
#endif


static const zend_module_dep mysqlnd_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};

/* {{{ mysqlnd_module_entry
 */
zend_module_entry mysqlnd_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	mysqlnd_deps,
	"mysqlnd",
	mysqlnd_functions,
	PHP_MINIT(mysqlnd),
	PHP_MSHUTDOWN(mysqlnd),
#if PHP_DEBUG
	PHP_RINIT(mysqlnd),
#else
	NULL,
#endif
#if PHP_DEBUG
	PHP_RSHUTDOWN(mysqlnd),
#else
	NULL,
#endif
	PHP_MINFO(mysqlnd),
	PHP_MYSQLND_VERSION,
	PHP_MODULE_GLOBALS(mysqlnd),
	PHP_GINIT(mysqlnd),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* {{{ COMPILE_DL_MYSQLND */
#ifdef COMPILE_DL_MYSQLND
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(mysqlnd)
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
