/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#define OPTIMIZED_ARRAY_CONSTRUCT

typedef struct {
	zend_function      *next;
	zend_function      *rewind;
	zend_function      *more;
	zend_function      *current;
	zend_function      *key;
} spl_foreach_funcs;

typedef struct {
	zval               *obj;
	zend_class_entry   *obj_ce;
	zend_uint          index;
	spl_is_a           is_a;
	spl_foreach_funcs  funcs;
	char               dummy; /* needed for '\0' but we can't set it due to compiler optimizations */
} spl_foreach_proxy;

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET)
{
	zval **obj, *retval;
	spl_foreach_proxy *proxy;
	zend_class_entry *instance_ce, *obj_ce;
	spl_is_a is_a;
	temp_variable *tmp;

	obj = spl_get_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

	if (!obj || (instance_ce = spl_get_class_entry(*obj TSRMLS_CC)) == NULL) {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_RESET);
	}

	is_a = spl_implements(instance_ce);

	if (is_a & SPL_IS_A_ITERATOR) {
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		obj_ce = instance_ce;
		spl_call_method_0(obj, obj_ce, NULL, "new_iterator", sizeof("new_iterator")-1, &retval);
		instance_ce = spl_get_class_entry(retval TSRMLS_CC);
		is_a = spl_implements(instance_ce);
		if (!(is_a & SPL_IS_A_FORWARD)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Objects created by %s::new_iterator() must implement spl_forward", obj_ce->name);
			ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_RESET);
		}
		PZVAL_LOCK(retval);
	} else if (is_a & SPL_IS_A_FORWARD) {
		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		retval = *obj;
		retval->refcount += 2; /* lock two times */
	} else {
		ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_RESET);
	}

	/* create the proxy */
	proxy = emalloc(sizeof(spl_foreach_proxy));
	proxy->obj = retval;
	proxy->obj_ce = instance_ce;
	proxy->index = 0;
	proxy->is_a = is_a;
	memset(&proxy->funcs, 0, sizeof(spl_foreach_funcs));
	((char*)proxy)[sizeof(spl_foreach_proxy)-1] = '\0';
	/* And pack it into a zval. Since it is nowhere accessible using a 
	 * zval of type STRING is the fastest approach of storing the proxy.
	 */
	ALLOC_ZVAL(retval);
	ZVAL_STRINGL(retval, (char*)proxy, sizeof(spl_foreach_proxy)-1, 0);
	retval->is_ref = 0;
	retval->refcount = 2; /* lock two times */
	/* return the created proxy container */
	tmp = &EX_T(EX(opline)->result.u.var);
	tmp->var.ptr = retval;
	tmp->var.ptr_ptr = &tmp->var.ptr;

	NEXT_OPCODE();
}
/* }}} */

/* {{{ OPTIMIZED_ARRAY_CONSTRUCT macros */
#ifdef OPTIMIZED_ARRAY_CONSTRUCT
#define CONNECT_TO_BUCKET_DLLIST(element, list_head)		\
	(element)->pNext = (list_head);							\
	(element)->pLast = NULL;

#define CONNECT_TO_GLOBAL_DLLIST(element, ht)				\
	(element)->pListLast = (ht)->pListTail;					\
	(ht)->pListTail = (element);							\
	(element)->pListNext = NULL;							\
	if ((element)->pListLast != NULL) {						\
		(element)->pListLast->pListNext = (element);		\
	}														\
	if (!(ht)->pListHead) {									\
		(ht)->pListHead = (element);						\
	}														\
	if ((ht)->pInternalPointer == NULL) {					\
		(ht)->pInternalPointer = (element);					\
	}
#endif
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_FETCH) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_FETCH)
{
	znode *op1 = &EX(opline)->op1;
 	zval **obj = spl_get_zval_ptr_ptr(op1, EX(Ts) TSRMLS_CC);
	zval *more, *value, *key, *result;
	spl_foreach_proxy *proxy;

	if (Z_TYPE_PP(obj) == IS_STRING) {
		int has_more;

		proxy = (spl_foreach_proxy*)Z_STRVAL_PP(obj);
		obj = &proxy->obj; /* will be optimized out */

		if (proxy->index++) {
			spl_call_method_0(obj, proxy->obj_ce, &proxy->funcs.next, "next", sizeof("next")-1, NULL);
		} else {
			if (proxy->is_a & SPL_IS_A_SEQUENCE) {
				spl_call_method_0(obj, proxy->obj_ce, &proxy->funcs.rewind, "rewind", sizeof("rewind")-1, NULL);
			}
			/* now this is an optimization trick:
			   ZEND_SWITCH_FREE receives the array copy or the spl object in op1 and has an unused op2
			   We have to check for op1 being an object that implements spl_forwar... Or we simply set 
			   op2 and know we can safely free the object as needed, which is waht we do. */
			op_array->opcodes[EX(opline)->op2.u.opline_num].op2 = *op1;
		}

		spl_call_method_0(obj, proxy->obj_ce, &proxy->funcs.more, "has_more", sizeof("has_more")-1, &more);
		has_more = i_zend_is_true(more);
		zval_dtor(more);
		FREE_ZVAL(more);
		if (has_more) {
			result = &EX_T(EX(opline)->result.u.var).tmp_var;

			spl_call_method_0(obj, proxy->obj_ce, &proxy->funcs.current, "current", sizeof("current")-1, &value);

			if (proxy->is_a & SPL_IS_A_ASSOC) {
				spl_call_method_0(obj, proxy->obj_ce, &proxy->funcs.key, "key", sizeof("key")-1, &key);
			} else {
				MAKE_STD_ZVAL(key);
				key->value.lval = proxy->index;
				key->type = IS_LONG;
			}
#ifndef OPTIMIZED_ARRAY_CONSTRUCT
			array_init(result);
			add_next_index_zval(result, value);
			add_next_index_zval(result, key);
#else
			{
				Bucket *p;
				HashTable *ht;

				ht = emalloc(sizeof(HashTable));
				result->value.ht = ht;
				ht->nTableSize = 1 << 1;
				ht->nTableMask = ht->nTableSize - 1;
#if ZEND_DEBUG
				ht->inconsistent = 0; /*HT_OK;*/
#endif
				
				ht->arBuckets = (Bucket **)emalloc(ht->nTableSize * sizeof(Bucket *));
				
				ht->pDestructor = ZVAL_PTR_DTOR;
				ht->pListHead = NULL;
				ht->pListTail = NULL;
				ht->nNumOfElements = 0;
				ht->nNextFreeElement = 0;
				ht->pInternalPointer = NULL;
				ht->persistent = 0;
				ht->nApplyCount = 0;
				ht->bApplyProtection = 1;
				result->type = IS_ARRAY;

				p = (Bucket*)emalloc(sizeof(Bucket)-1);
				p->pDataPtr = value;
				p->pData = &p->pDataPtr;
				p->nKeyLength = 0;
				p->h = 0;
				result->value.ht->arBuckets[0] = p;
				CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[0]);
				CONNECT_TO_GLOBAL_DLLIST(p, ht);

				p = (Bucket*)emalloc(sizeof(Bucket)-1);
				p->pDataPtr = key;
				p->pData = &p->pDataPtr;
				p->nKeyLength = 0;
				p->h = 1;
				result->value.ht->arBuckets[1] = p;
				CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[1]);
				CONNECT_TO_GLOBAL_DLLIST(p, ht);

				ht->nNumOfElements = 2;
			}
#endif
			NEXT_OPCODE();
		}
		EX(opline) = op_array->opcodes+EX(opline)->op2.u.opline_num;
		return 0;
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_FETCH);
}
/* }}} */

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_SWITCH_FREE) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_SWITCH_FREE)
{
	/* See not in ZEND_FE_FETCH on setting op2 */
	znode *op2 = &EX(opline)->op2;
	zval *tmp, **obj = spl_get_zval_ptr_ptr(op2, EX(Ts) TSRMLS_CC);
	spl_foreach_proxy *proxy;

	if (obj) {
		proxy = (spl_foreach_proxy*)Z_STRVAL_PP(obj);
		tmp = *obj;
		*obj = proxy->obj; /* restore */

		efree(tmp->value.str.val);
		FREE_ZVAL(tmp);

		spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		PZVAL_LOCK(*obj);
		
		SET_UNUSED(*op2);
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_SWITCH_FREE);
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
