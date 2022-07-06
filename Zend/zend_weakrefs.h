/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: krakjoe@php.net                                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_WEAKREFS_H
#define ZEND_WEAKREFS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_weakref;

void zend_register_weakref_ce(void);

void zend_weakrefs_init(void);
void zend_weakrefs_shutdown(void);

ZEND_API void zend_weakrefs_notify(zend_object *object);

ZEND_API zval *zend_weakrefs_hash_add(HashTable *ht, zend_object *key, zval *pData);
ZEND_API zend_result zend_weakrefs_hash_del(HashTable *ht, zend_object *key);
static zend_always_inline void *zend_weakrefs_hash_add_ptr(HashTable *ht, zend_object *key, void *ptr) {
	zval tmp, *zv;
	ZVAL_PTR(&tmp, ptr);
	if ((zv = zend_weakrefs_hash_add(ht, key, &tmp))) {
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

END_EXTERN_C()

#endif

