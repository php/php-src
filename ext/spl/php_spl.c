/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_foreach.h"
#include "spl_array.h"

#ifdef COMPILE_DL_SPL
ZEND_GET_MODULE(spl)
#endif

ZEND_DECLARE_MODULE_GLOBALS(spl)

/* {{{ spl_functions
 */
function_entry spl_functions[] = {
	PHP_FE(spl_classes,             NULL)
	PHP_FE(class_name,              NULL)
	PHP_FE(class_parents,           NULL)
	PHP_FE(class_implements,        NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ spl_module_entry
 */
zend_module_entry spl_module_entry = {
	STANDARD_MODULE_HEADER,
	"spl",
	spl_functions,
	PHP_MINIT(spl),
	PHP_MSHUTDOWN(spl),
	PHP_RINIT(spl),
	PHP_RSHUTDOWN(spl),
	PHP_MINFO(spl),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

zend_namespace   *spl_ns_spl;
zend_class_entry *spl_ce_iterator;
zend_class_entry *spl_ce_forward;
zend_class_entry *spl_ce_assoc;
zend_class_entry *spl_ce_sequence;
zend_class_entry *spl_ce_forward_assoc;
zend_class_entry *spl_ce_sequence_assoc;
zend_class_entry *spl_ce_array_read;
zend_class_entry *spl_ce_array_access;
zend_class_entry *spl_ce_array_access_ex;
zend_class_entry *spl_ce_array_writer;
#ifdef SPL_ARRAY_WRITE
zend_class_entry *spl_ce_array_writer_default;
#endif /* SPL_ARRAY_WRITE */

/* {{{ spl_functions_none
 */
function_entry spl_functions_none[] = {
	{NULL, NULL, NULL}
};
/* }}} */

static unsigned char first_of_two_force_ref[] = { 2, BYREF_FORCE, BYREF_NONE };

/* {{{ spl_array_writer_funcs
 */
function_entry spl_array_writer_funcs[] = {
	SPL_CLASS_FE(array_writer_default,   __construct,   first_of_two_force_ref)
	SPL_CLASS_FE(array_writer_default,   set,           NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ spl_init_globals
 */
static void spl_init_globals(zend_spl_globals *spl_globals)
{
#ifdef SPL_FOREACH
	ZEND_EXECUTE_HOOK(ZEND_FE_RESET);
	ZEND_EXECUTE_HOOK(ZEND_FE_FETCH);
#endif

#if defined(SPL_ARRAY_READ) | defined(SPl_ARRAY_WRITE)
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_R);
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_W);
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_RW);
#endif

#ifdef SPL_ARRAY_WRITE
	ZEND_EXECUTE_HOOK(ZEND_ASSIGN);
#endif /* SPL_ARRAY_WRITE */
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl)
 */
PHP_MINIT_FUNCTION(spl)
{
	ZEND_INIT_MODULE_GLOBALS(spl, spl_init_globals, NULL);

	REGISTER_SPL_NAMESPACE(spl);

	REGISTER_SPL_INTERFACE(spl, iterator);
	REGISTER_SPL_INTF_FUNC(spl, iterator, new_iterator);

	REGISTER_SPL_INTERFACE(spl, forward);
	REGISTER_SPL_INTF_FUNC(spl, forward, current);
	REGISTER_SPL_INTF_FUNC(spl, forward, next);
	REGISTER_SPL_INTF_FUNC(spl, forward, more);

	REGISTER_SPL_INTERFACE(spl, sequence);
	REGISTER_SPL_INTF_FUNC(spl, sequence, rewind);
	REGISTER_SPL_IMPLEMENT(spl, sequence, forward);

	REGISTER_SPL_INTERFACE(spl, assoc);
	REGISTER_SPL_INTF_FUNC(spl, assoc, key);

	REGISTER_SPL_INTERFACE(spl, forward_assoc);
	REGISTER_SPL_IMPLEMENT(spl, forward_assoc, assoc);
	REGISTER_SPL_IMPLEMENT(spl, forward_assoc, forward);

	REGISTER_SPL_INTERFACE(spl, sequence_assoc);
	REGISTER_SPL_IMPLEMENT(spl, sequence_assoc, forward_assoc);
	REGISTER_SPL_IMPLEMENT(spl, sequence_assoc, sequence);

	REGISTER_SPL_INTERFACE(spl, array_read);
	REGISTER_SPL_INTF_FUNC(spl, array_read, get);
	REGISTER_SPL_INTF_FUNC(spl, array_read, exists);

	REGISTER_SPL_INTERFACE(spl, array_access);
	REGISTER_SPL_IMPLEMENT(spl, array_access, array_read);
	REGISTER_SPL_INTF_FUNC(spl, array_access, set);

	REGISTER_SPL_INTERFACE(spl, array_access_ex);
	REGISTER_SPL_IMPLEMENT(spl, array_access_ex, array_access);
	REGISTER_SPL_INTF_FUNC(spl, array_access_ex, new_writer);

	REGISTER_SPL_INTERFACE(spl, array_writer);
	REGISTER_SPL_INTF_FUNC(spl, array_writer, set); 

#ifdef SPL_ARRAY_WRITE
	REGISTER_SPL_STD_CLASS(spl, array_writer_default, spl_array_writer_default_create);
	REGISTER_SPL_FUNCTIONS(spl, array_writer_default, spl_array_writer_funcs);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(spl)
 */
PHP_RINIT_FUNCTION(spl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(spl)
 */
PHP_RSHUTDOWN_FUNCTION(spl)
{                      
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(spl)
 */
PHP_MSHUTDOWN_FUNCTION(spl)
{                      
	SPL_DEBUG(fprintf(stderr, "%s\n", "Shutting down SPL");)

#ifdef SPL_FOREACH
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FE_RESET);
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FE_FETCH);
#endif

#if defined(SPL_ARRAY_READ) | defined(SPL_ARRAY_WRITE)
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_R);
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_W);
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_RW); 
#endif

#ifdef SPL_ARRAY_WRITE
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_ASSIGN);
#endif /* SPL_ARRAY_WRITE */
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO(spl)
 */
PHP_MINFO_FUNCTION(spl)
{
#ifdef SPL_FOREACH
	char *foreach = "beta";
#else /* SPL_ARRAY_WRITE */
	char *foreach = "beta, not hooked";
#endif
#ifdef SPL_ARRAY_READ
	char *array_read = "beta";
#else /* SPL_ARRAY_WRITE */
	char *array_read = "beta, not hooked";
#endif
#ifdef SPL_ARRAY_WRITE
	char *array_write = "beta";
#else /* SPL_ARRAY_WRITE */
	char *array_write = "beta, not hooked";
#endif /* SPL_ARRAY_WRITE */

	php_info_print_table_start();
	php_info_print_table_header(2, "SPL support",        "enabled");
	php_info_print_table_row(2,    "iterator",           foreach);
	php_info_print_table_row(2,    "forward",            foreach);
	php_info_print_table_row(2,    "sequence",           foreach);
	php_info_print_table_row(2,    "assoc",              foreach);
	php_info_print_table_row(2,    "forward_assoc",      foreach);
	php_info_print_table_row(2,    "sequence_assoc",     foreach);
	php_info_print_table_row(2,    "array_read",         array_read);
	php_info_print_table_row(2,    "array_access",       array_write);
	php_info_print_table_row(2,    "array_access_ex",    array_write);
	php_info_print_table_row(2,    "array_writer",       array_write);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto string class_name(object)
       Retrieve  */
PHP_FUNCTION(class_name)
{
	zval *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRING(spl_make_fully_qualyfied_name(Z_OBJCE_P(obj) TSRMLS_CC), 0);
}
/* }}} */

/* {{{ class_parents
 */
PHP_FUNCTION(class_parents)
{
	zval *obj;
	zend_class_entry *parent_class;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	parent_class = Z_OBJCE_P(obj)->parent;
	while (parent_class) {
		spl_add_class_name(return_value, parent_class TSRMLS_CC);
		parent_class = parent_class->parent;
	}
}
/* }}} */

/* {{{ class_implements
 */
PHP_FUNCTION(class_implements)
{
	zval *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	spl_add_interfaces(return_value, Z_OBJCE_P(obj) TSRMLS_CC);
}
/* }}} */

/* {{{ spl_classes */
PHP_FUNCTION(spl_classes)
{
	array_init(return_value);
	zend_hash_apply_with_argument(&spl_ns_spl->class_table, (apply_func_arg_t)spl_add_classes, return_value TSRMLS_CC);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
