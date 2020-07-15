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
ZEND_TLS unsigned int nesting_depth = 1;

PHP_FUNCTION(observer_op_array_extension_handle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETVAL_LONG(zend_observer_fcall_op_array_extension);
}

static char *observer_get_filename(zend_execute_data *execute_data) {
	const zend_op *opline = execute_data->opline;
	uint32_t extended_value = opline->extended_value;
	ZEND_ASSERT(opline->opcode == ZEND_INCLUDE_OR_EVAL);
	ZEND_ASSERT(
		extended_value == ZEND_INCLUDE
		|| extended_value == ZEND_INCLUDE_ONCE
		|| extended_value == ZEND_REQUIRE
		|| extended_value == ZEND_REQUIRE_ONCE
	);

	zval *op = zend_get_zval_ptr(opline, opline->op1_type, &opline->op1, execute_data);
	return (op && Z_TYPE_P(op) == IS_STRING) ? Z_STRVAL_P(op) : NULL;
}

static char *include_pretty_name[] = {
	[ZEND_EVAL] = "eval",
	[ZEND_INCLUDE] = "include",
	[ZEND_INCLUDE_ONCE] = "include_once",
	[ZEND_REQUIRE] = "require",
	[ZEND_REQUIRE_ONCE] = "require_once",
};

static void observer_begin(zend_execute_data *execute_data)
{

	if (getenv("OBSERVER_DEBUG")) {
		if (execute_data->func && execute_data->func->common.function_name) {
			printf("%*s<%s>\n", 2 * nesting_depth, "", ZSTR_VAL(execute_data->func->common.function_name));
		} else if (execute_data->opline->opcode == ZEND_INCLUDE_OR_EVAL) {
			uint32_t extended_value = execute_data->opline->extended_value;
			switch (extended_value) {
				case ZEND_INCLUDE:
				case ZEND_INCLUDE_ONCE:
				case ZEND_REQUIRE:
				case ZEND_REQUIRE_ONCE: {
					char *filename = observer_get_filename(execute_data);
					printf("%*s<%s filename=\"%s\">\n", 2 * nesting_depth, "", include_pretty_name[extended_value], filename ?: "(unknown)");
				}
				break;

				case ZEND_EVAL:
					printf("%*s<eval>\n", 2 * nesting_depth, "");
			}
		}

		++nesting_depth;
	}
}

static void observer_end(zend_execute_data *execute_data, zval *retval) {
	if (getenv("OBSERVER_DEBUG")) {
    --nesting_depth;

		if (execute_data->func && execute_data->func->common.function_name) {
			printf("%*s</%s>\n", 2 * nesting_depth, "", ZSTR_VAL(execute_data->func->common.function_name));
		} else if (execute_data->opline->opcode == ZEND_INCLUDE_OR_EVAL) {
			uint32_t extended_value = execute_data->opline->extended_value;
			switch (extended_value) {
				case ZEND_INCLUDE:
				case ZEND_INCLUDE_ONCE:
				case ZEND_REQUIRE:
				case ZEND_REQUIRE_ONCE: {
					printf("%*s</%s>\n", 2 * nesting_depth, "", include_pretty_name[extended_value]);
				}
				break;

				case ZEND_EVAL:
					printf("%*s</eval>\n", 2 * nesting_depth, "");

				default:
					ZEND_ASSERT(0 );
			}
		} else {
			ZEND_ASSERT(0);
		}
	}
}

static zend_observer_fcall observer_fcall_init(zend_function *fbc)
{
	return (zend_observer_fcall){observer_begin, observer_end};
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
