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

#define OPTIMIZED_ARRAY_CONSTRUCT

#define ezalloc(size) \
	memset(emalloc(size), 0, size)

typedef struct {
	zend_uint        index;
	zend_class_entry *obj_ce;
	zend_uint        is_ce_assoc;
	zend_function    *f_next;
	zend_function    *f_rewind;
	zend_function    *f_more;
	zend_function    *f_current;
	zend_function    *f_key;
} spl_foreach_proxy;

/* {{{ ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET) */
ZEND_EXECUTE_HOOK_FUNCTION(ZEND_FE_RESET)
{
	zval **obj, *retval;

	if (EX(opline)->extended_value) {
		obj = spl_get_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
		if (spl_implements(obj, spl_ce_iterator TSRMLS_CC)) {
			spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);
			spl_begin_method_call_ex(obj, NULL, NULL, "new_iterator", sizeof("new_iterator")-1, &retval TSRMLS_CC);
			EX_T(EX(opline)->result.u.var).var.ptr = retval;
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = &EX_T(EX(opline)->result.u.var).var.ptr;	
			/* EX(opline)->result.u.EA.type = 0; */

			PZVAL_LOCK(retval);

			NEXT_OPCODE();
		} else if (spl_implements(obj, spl_ce_forward TSRMLS_CC)) {
			spl_unlock_zval_ptr_ptr(&EX(opline)->op1, EX(Ts) TSRMLS_CC);

			EX_T(EX(opline)->result.u.var).var.ptr = *obj;
			EX_T(EX(opline)->result.u.var).var.ptr_ptr = &EX_T(EX(opline)->result.u.var).var.ptr;	
			/* EX(opline)->result.u.EA.type = 0; */

			(*obj)->refcount++;
			PZVAL_LOCK(*obj);

			NEXT_OPCODE();
		}
	}
	ZEND_EXECUTE_HOOK_ORIGINAL(ZEND_FE_RESET);
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
	zval more, tmp, *value, *key, *result;
	spl_foreach_proxy *proxy = (spl_foreach_proxy*)op1->u.EA.type;

	if (proxy || spl_implements(obj, spl_ce_forward TSRMLS_CC)) {
		if (!proxy) {
			(spl_foreach_proxy*)op1->u.EA.type = proxy = ezalloc(sizeof(spl_foreach_proxy));
			proxy->obj_ce = spl_get_class_entry(*obj TSRMLS_CC);
		}
		
		spl_unlock_zval_ptr_ptr(op1, EX(Ts) TSRMLS_CC);
		PZVAL_LOCK(*obj);

		if (proxy->index++) {
			spl_begin_method_call_this(obj, proxy->obj_ce, &proxy->f_next, "next", sizeof("next")-1, &tmp TSRMLS_CC);
		} else {
			proxy->is_ce_assoc = spl_implements(obj, spl_ce_assoc TSRMLS_CC);
			if (spl_implements(obj, spl_ce_sequence TSRMLS_CC)) {
				spl_begin_method_call_this(obj, proxy->obj_ce, &proxy->f_rewind, "rewind", sizeof("rewind")-1, &tmp TSRMLS_CC);
			}
		}

		spl_begin_method_call_this(obj, proxy->obj_ce, &proxy->f_more, "has_more", sizeof("has_more")-1, &more TSRMLS_CC);
		if (zend_is_true(&more)) {
			result = &EX_T(EX(opline)->result.u.var).tmp_var;

			spl_begin_method_call_ex(obj, proxy->obj_ce, &proxy->f_current, "current", sizeof("current")-1, &value TSRMLS_CC);

			if (proxy->is_ce_assoc) {
				spl_begin_method_call_ex(obj, proxy->obj_ce, &proxy->f_key, "key", sizeof("key")-1, &key TSRMLS_CC);
			} else {
				MAKE_STD_ZVAL(key);
				key->value.lval = proxy->index;
				key->type = IS_LONG;
			}
#ifndef OPTIMIZED_ARRAY_CONSTRUCT
			array_init(result);
			zend_hash_index_update(result->value.ht, 0, &value, sizeof(zval *), NULL);
			zend_hash_index_update(result->value.ht, 1, &key, sizeof(zval *), NULL);
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
		efree(proxy);
		op1->u.EA.type = 0;
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
