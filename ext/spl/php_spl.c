/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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
	"0.2",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

zend_class_entry *spl_ce_iterator;
zend_class_entry *spl_ce_forward;
zend_class_entry *spl_ce_assoc;
zend_class_entry *spl_ce_sequence;
zend_class_entry *spl_ce_forward_assoc;
zend_class_entry *spl_ce_sequence_assoc;
zend_class_entry *spl_ce_array_read;
zend_class_entry *spl_ce_array_access;

/* {{{ spl_functions_none
 */
function_entry spl_functions_none[] = {
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
	ZEND_EXECUTE_HOOK(ZEND_SWITCH_FREE);
#endif

#if defined(SPL_ARRAY_READ) | defined(SPL_ARRAY_WRITE)
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_R);
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_W);
	ZEND_EXECUTE_HOOK(ZEND_FETCH_DIM_RW);
#endif

#ifdef SPL_ARRAY_WRITE
	ZEND_EXECUTE_HOOK(ZEND_ASSIGN_DIM);
	ZEND_EXECUTE_HOOK(ZEND_UNSET_DIM_OBJ);
#endif /* SPL_ARRAY_WRITE */
}
/* }}} */

PHP_FUNCTION(spl_abstract) {}

#define SPL_ABSTRACT_FE(class, name, arg_info) \
	{ #name, ZEND_FN(spl_abstract), arg_info, sizeof(arg_info)/sizeof(struct _zend_arg_info)-1, ZEND_ACC_ABSTRACT|ZEND_ACC_PUBLIC },

static
ZEND_BEGIN_ARG_INFO(arginfo_one_param, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_two_params, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

function_entry spl_funcs_iterator[] = {
	SPL_ABSTRACT_FE(iterator, newIterator, NULL)
	{NULL, NULL, NULL}
};

function_entry spl_funcs_forward[] = {
	SPL_ABSTRACT_FE(forward, current,  NULL)
	SPL_ABSTRACT_FE(forward, next,     NULL)
	SPL_ABSTRACT_FE(forward, hasMore,  NULL)
	{NULL, NULL, NULL}
};

function_entry spl_funcs_sequence[] = {
	SPL_ABSTRACT_FE(sequence, rewind,  NULL)
	{NULL, NULL, NULL}
};

function_entry spl_funcs_assoc[] = {
	SPL_ABSTRACT_FE(assoc, key,  NULL)
	{NULL, NULL, NULL}
};

function_entry *spl_funcs_forward_assoc  = NULL;
function_entry *spl_funcs_sequence_assoc = NULL;

function_entry spl_funcs_array_read[] = {
	SPL_ABSTRACT_FE(array_read, get,     arginfo_one_param)
	SPL_ABSTRACT_FE(array_read, exists,  arginfo_one_param)
	{NULL, NULL, NULL}
};

function_entry spl_funcs_array_access[] = {
	SPL_ABSTRACT_FE(array_access, set,  arginfo_two_params)
	SPL_ABSTRACT_FE(array_access, del,  arginfo_one_param)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(spl)
 */
PHP_MINIT_FUNCTION(spl)
{
	ZEND_INIT_MODULE_GLOBALS(spl, spl_init_globals, NULL);

	REGISTER_SPL_INTERFACE(iterator);

	REGISTER_SPL_INTERFACE(forward);

	REGISTER_SPL_INTERFACE(sequence);
	REGISTER_SPL_IMPLEMENT(sequence, forward);

	REGISTER_SPL_INTERFACE(assoc);

	REGISTER_SPL_INTERFACE(forward_assoc);
	REGISTER_SPL_IMPLEMENT(forward_assoc, assoc);
	REGISTER_SPL_IMPLEMENT(forward_assoc, forward);

	REGISTER_SPL_INTERFACE(sequence_assoc);
	REGISTER_SPL_IMPLEMENT(sequence_assoc, forward_assoc);
	REGISTER_SPL_IMPLEMENT(sequence_assoc, sequence);

	REGISTER_SPL_INTERFACE(array_read);

	REGISTER_SPL_INTERFACE(array_access);
	REGISTER_SPL_IMPLEMENT(array_access, array_read);

	PHP_MINIT(spl_array)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(spl_directory)(INIT_FUNC_ARGS_PASSTHRU);

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
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_SWITCH_FREE);
#endif

#if defined(SPL_ARRAY_READ) | defined(SPL_ARRAY_WRITE)
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_R);
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_W);
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_FETCH_DIM_RW); 
#endif

#ifdef SPL_ARRAY_WRITE
	ZEND_EXECUTE_HOOK_RESTORE(ZEND_ASSIGN_DIM);
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
	php_info_print_table_end();
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

#define SPL_ADD_CLASS(class_name) \
	spl_add_classes(&spl_ce_ ## class_name, return_value TSRMLS_CC)

/* {{{ spl_classes */
PHP_FUNCTION(spl_classes)
{
	array_init(return_value);

	SPL_ADD_CLASS(iterator);
	SPL_ADD_CLASS(forward);
	SPL_ADD_CLASS(sequence);
	SPL_ADD_CLASS(assoc);
	SPL_ADD_CLASS(forward_assoc);
	SPL_ADD_CLASS(sequence_assoc);
	SPL_ADD_CLASS(array_read);
	SPL_ADD_CLASS(array_access);
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
