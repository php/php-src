/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef SPL_ENGINE_H
#define SPL_ENGINE_H

#include "php.h"
#include "php_spl.h"
#include "zend_interfaces.h"

PHPAPI void spl_instantiate(zend_class_entry *pce, zval *object);

PHPAPI zend_long spl_offset_convert_to_long(zval *offset);

/* {{{ spl_instantiate_arg_ex1 */
static inline int spl_instantiate_arg_ex1(zend_class_entry *pce, zval *retval, zval *arg1)
{
	zend_function *func = pce->constructor;
	spl_instantiate(pce, retval);

	zend_call_method(retval, pce, &func, ZSTR_VAL(func->common.function_name), ZSTR_LEN(func->common.function_name), NULL, 1, arg1, NULL);
	return 0;
}
/* }}} */

/* {{{ spl_instantiate_arg_ex2 */
static inline int spl_instantiate_arg_ex2(zend_class_entry *pce, zval *retval, zval *arg1, zval *arg2)
{
	zend_function *func = pce->constructor;
	spl_instantiate(pce, retval);

	zend_call_method(retval, pce, &func, ZSTR_VAL(func->common.function_name), ZSTR_LEN(func->common.function_name), NULL, 2, arg1, arg2);
	return 0;
}
/* }}} */

/* {{{ spl_instantiate_arg_n */
static inline void spl_instantiate_arg_n(zend_class_entry *pce, zval *retval, int argc, zval *argv)
{
	zend_function *func = pce->constructor;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval dummy;

	spl_instantiate(pce, retval);

	fci.size = sizeof(zend_fcall_info);
	fci.function_table = &pce->function_table;
	ZVAL_STR(&fci.function_name, func->common.function_name);
	fci.object = Z_OBJ_P(retval);
	fci.symbol_table = NULL;
	fci.retval = &dummy;
	fci.param_count = argc;
	fci.params = argv;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = func;
	fcc.calling_scope = EG(scope);
	fcc.called_scope = pce;
	fcc.object = Z_OBJ_P(retval);

	zend_call_function(&fci, &fcc);
}
/* }}} */

#endif /* SPL_ENGINE_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
