/*
  +----------------------------------------------------------------------+
  | collections extension for PHP                                        |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#include "ext/standard/info.h"

#include "collections_deque.h"

#include "php_collections.h"

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(collections)
{
	PHP_MINIT(collections_deque)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(collections)
{
	(void) ((ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU));
	php_info_print_table_start();
	php_info_print_table_header(2, "collections support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ collections_module_entry */
static const zend_module_dep collections_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_REQUIRED("json")
	ZEND_MOD_END
};

zend_module_entry collections_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	collections_deps,
	"collections",				/* Extension name */
	NULL,						/* zend_function_entry */
	PHP_MINIT(collections),		/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	NULL,						/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(collections),		/* PHP_MINFO - Module info */
	PHP_COLLECTIONS_VERSION,	/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COLLECTIONS
ZEND_GET_MODULE(collections)
#endif
