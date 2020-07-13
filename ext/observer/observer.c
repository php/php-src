/* observer extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_observer.h"
#include "observer_arginfo.h"

#include "zend_observer.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

int zend_observer_fcall_op_array_extension;

PHP_FUNCTION(observer_op_array_extension_handle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETVAL_LONG(zend_observer_fcall_op_array_extension);
}

static void observer_begin(zend_execute_data *execute_data)
{
	if (getenv("OBSERVER_DEBUG") && execute_data->func->common.function_name) {
		php_printf("BEGIN (%s)\n", ZSTR_VAL(execute_data->func->common.function_name));
	}
}

static zend_observer_fcall observer_fcall_init(zend_function *fbc)
{
	return (zend_observer_fcall){observer_begin, NULL};
}

PHP_MINIT_FUNCTION(observer)
{
	zend_observer_fcall_register(observer_fcall_init);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(observer) { return SUCCESS; }

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(observer)
{
#if defined(ZTS) && defined(COMPILE_DL_OBSERVER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(observer)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "observer support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ observer_module_entry
 */
zend_module_entry observer_module_entry = {
	STANDARD_MODULE_HEADER,
	"observer",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(observer),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(observer),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(observer),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(observer),			/* PHP_MINFO - Module info */
	PHP_OBSERVER_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OBSERVER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(observer)
#endif
