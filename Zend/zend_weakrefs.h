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

#include "zend_alloc.h"

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

/* Because php uses the raw numbers as a hash function, raw pointers will lead to hash collisions.
 * We have a guarantee that the lowest ZEND_MM_ALIGNED_OFFSET_LOG2 bits of a pointer are zero.
 *
 * E.g. On most 64-bit platforms, pointers are aligned to 8 bytes, so the least significant 3 bits are always 0 and can be discarded.
 *
 * NOTE: This function is only used for EG(weakrefs) and zend_weakmap->ht.
 * It is not used for the HashTable instances associated with ZEND_WEAKREF_TAG_HT tags (created in zend_weakref_register, which uses ZEND_WEAKREF_ENCODE instead).
 * The ZEND_WEAKREF_TAG_HT instances are used to disambiguate between multiple weak references to the same zend_object.
 */
static zend_always_inline zend_ulong zend_object_to_weakref_key(const zend_object *object)
{
	ZEND_ASSERT(((uintptr_t)object) % ZEND_MM_ALIGNMENT == 0);
	return ((uintptr_t) object) >> ZEND_MM_ALIGNMENT_LOG2;
}

static zend_always_inline zend_object *zend_weakref_key_to_object(zend_ulong key)
{
	return (zend_object *) (((uintptr_t) key) << ZEND_MM_ALIGNMENT_LOG2);
}

HashTable *zend_weakmap_get_gc(zend_object *object, zval **table, int *n);
HashTable *zend_weakmap_get_key_entry_gc(zend_object *object, zval **table, int *n);
HashTable *zend_weakmap_get_entry_gc(zend_object *object, zval **table, int *n);
HashTable *zend_weakmap_get_object_key_entry_gc(zend_object *object, zval **table, int *n);
HashTable *zend_weakmap_get_object_entry_gc(zend_object *object, zval **table, int *n);

END_EXTERN_C()

#endif

