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
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_compile.h"
#include "zend_execute_locks.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_foreach.h"

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET)
{
	zval **obj, *retval;

	if (EX(opline)->extended_value) {
		obj = spl_get_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		if (spl_is_instance_of(obj, spl_ce_iterator TSRMLS_CC)) {
			spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
			MAKE_STD_ZVAL(retval);
			spl_begin_method_call_this(obj, "new_iterator", retval TSRMLS_CC);
			EX_T(EX(opline)->result.u.var).var.ptr = retval;
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = &EX_T(EX(opline)->result.u.var).var.ptr;	
			EX(opline)->op2.u.EA.type = 0; /* missuse as index */

			PZVAL_LOCK(retval);

			NEXT_OPCODE();
		} else if (spl_is_instance_of(obj, spl_ce_forward TSRMLS_CC)) {
			spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

			EX_T(EX(opline)->result.u.var).var.ptr = *obj;
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = &EX_T(EX(opline)->result.u.var).var.ptr;	
			EX(opline)->op2.u.EA.type = 0; /* missuse as index */

			(*obj)->refcount++;
			PZVAL_LOCK(*obj);

			NEXT_OPCODE();
		}
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_RESET);
}
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_FETCH) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_FETCH)
{
	zval **obj = spl_get_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
	zval more, tmp, *value, *key, *result;

	if (spl_is_instance_of(obj, spl_ce_forward TSRMLS_CC)) {
		zend_uint index = EX(opline)->op2.u.EA.type++;
		
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		PZVAL_LOCK(*obj);

		if (index) {
			spl_begin_method_call_this(obj, "next", &more TSRMLS_CC);
		} else if (spl_is_instance_of(obj, spl_ce_sequence TSRMLS_CC)) {
			spl_begin_method_call_this(obj, "rewind", &more TSRMLS_CC);
		}

		spl_begin_method_call_this(obj, "has_more", &more TSRMLS_CC);
		if (zend_is_true(&more)) {
			result = &EX_T(EX(opline)->result.u.var).tmp_var;
			array_init(result);
			ALLOC_ZVAL(value);

			spl_begin_method_call_this(obj, "current", value TSRMLS_CC);

			zend_hash_index_update(result->value.ht, 0, &value, sizeof(zval *), NULL);
		
			if (spl_is_instance_of(obj, spl_ce_assoc TSRMLS_CC)) {
				ALLOC_ZVAL(key);
				spl_begin_method_call_this(obj, "key", key TSRMLS_CC);
			} else {
				/* If someone makes a reference to this value then there is
				 * a real problem. And the only way to avoid it is to alloc
				 * dealloc this temporary zval then.
				 */
				tmp.value.lval = index;
				tmp.type = IS_LONG;
				tmp.refcount = 0;
				tmp.is_ref = 0;
				key = &tmp;
			}
			zend_hash_index_update(result->value.ht, 1, &key, sizeof(zval *), NULL);

			NEXT_OPCODE();
		}
		else
		EX(opline) = op_array->opcodes+EX(opline)->op2.u.opline_num;
		return 0;
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_FETCH);
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
