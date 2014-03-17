/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_HASH_H
#define ZEND_HASH_H

#include <sys/types.h>
#include "zend.h"

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_LONG 2
#define HASH_KEY_NON_EXISTENT 3
#define HASH_KEY_NON_EXISTANT HASH_KEY_NON_EXISTENT /* Keeping old define (with typo) for backward compatibility */

#define HASH_UPDATE 		(1<<0)
#define HASH_ADD			(1<<1)
#define HASH_NEXT_INSERT	(1<<2)

#define HASH_UPDATE_KEY_IF_NONE    0
#define HASH_UPDATE_KEY_IF_BEFORE  1
#define HASH_UPDATE_KEY_IF_AFTER   2
#define HASH_UPDATE_KEY_ANYWAY     3

#define INVALID_IDX ((uint)-1)

#define HASH_FLAG_PERSISTENT       (1<<0)
#define HASH_FLAG_APPLY_PROTECTION (1<<1)
#define HASH_FLAG_PACKED           (1<<2)

#define HASH_MASK_CONSISTENCY      0x60

typedef struct _zend_hash_key {
	ulong        h;
	zend_string *key;
} zend_hash_key;

typedef zend_bool (*merge_checker_func_t)(HashTable *target_ht, zval *source_data, zend_hash_key *hash_key, void *pParam);

typedef uint HashPosition;

BEGIN_EXTERN_C()

/* startup/shutdown */
ZEND_API int _zend_hash_init(HashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC);
ZEND_API int _zend_hash_init_ex(HashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC);
ZEND_API void zend_hash_destroy(HashTable *ht);
ZEND_API void zend_hash_clean(HashTable *ht);
#define zend_hash_init(ht, nSize, pHashFunction, pDestructor, persistent)						_zend_hash_init((ht), (nSize), (pDestructor), (persistent) ZEND_FILE_LINE_CC)
#define zend_hash_init_ex(ht, nSize, pHashFunction, pDestructor, persistent, bApplyProtection)		_zend_hash_init_ex((ht), (nSize), (pDestructor), (persistent), (bApplyProtection) ZEND_FILE_LINE_CC)

ZEND_API void zend_hash_real_init(HashTable *ht, int packed);
ZEND_API void zend_hash_packed_to_hash(HashTable *ht);
ZEND_API void zend_hash_to_packed(HashTable *ht);

/* additions/updates/changes */
ZEND_API zval *_zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, int flag ZEND_FILE_LINE_DC);
#define zend_hash_update(ht, key, pData) \
		_zend_hash_add_or_update(ht, key, pData, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_hash_add(ht, key, pData) \
		_zend_hash_add_or_update(ht, key, pData, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API zval *_zend_hash_str_add_or_update(HashTable *ht, const char *key, int len, zval *pData, int flag ZEND_FILE_LINE_DC);
#define zend_hash_str_update(ht, key, len, pData) \
		_zend_hash_str_add_or_update(ht, key, len, pData, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_hash_str_add(ht, key, len, pData) \
		_zend_hash_str_add_or_update(ht, key, len, pData, HASH_ADD ZEND_FILE_LINE_CC)

ZEND_API zval *_zend_hash_index_update_or_next_insert(HashTable *ht, ulong h, zval *pData, int flag ZEND_FILE_LINE_DC);
#define zend_hash_index_update(ht, h, pData) \
		_zend_hash_index_update_or_next_insert(ht, h, pData, HASH_UPDATE ZEND_FILE_LINE_CC)
#define zend_hash_next_index_insert(ht, pData) \
		_zend_hash_index_update_or_next_insert(ht, 0, pData, HASH_NEXT_INSERT ZEND_FILE_LINE_CC)

ZEND_API zval *zend_hash_add_empty_element(HashTable *ht, zend_string *key);
ZEND_API zval *zend_hash_str_add_empty_element(HashTable *ht, const char *key, int len);

#define ZEND_HASH_APPLY_KEEP				0
#define ZEND_HASH_APPLY_REMOVE				1<<0
#define ZEND_HASH_APPLY_STOP				1<<1

typedef int (*apply_func_t)(zval *pDest TSRMLS_DC);
typedef int (*apply_func_arg_t)(zval *pDest, void *argument TSRMLS_DC);
typedef int (*apply_func_args_t)(zval *pDest TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key);

ZEND_API void zend_hash_graceful_destroy(HashTable *ht);
ZEND_API void zend_hash_graceful_reverse_destroy(HashTable *ht);
ZEND_API void zend_hash_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC);
ZEND_API void zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void * TSRMLS_DC);
ZEND_API void zend_hash_apply_with_arguments(HashTable *ht TSRMLS_DC, apply_func_args_t apply_func, int, ...);

/* This function should be used with special care (in other words,
 * it should usually not be used).  When used with the ZEND_HASH_APPLY_STOP
 * return value, it assumes things about the order of the elements in the hash.
 * Also, it does not provide the same kind of reentrancy protection that
 * the standard apply functions do.
 */
ZEND_API void zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func TSRMLS_DC);


/* Deletes */
ZEND_API int zend_hash_del(HashTable *ht, zend_string *key);
ZEND_API int zend_hash_str_del(HashTable *ht, const char *key, int len);
ZEND_API int zend_hash_index_del(HashTable *ht, ulong h);

/* Data retreival */
ZEND_API zval *zend_hash_find(const HashTable *ht, zend_string *key);
ZEND_API zval *zend_hash_str_find(const HashTable *ht, const char *key, int len);
ZEND_API zval *zend_hash_index_find(const HashTable *ht, ulong h);

/* Misc */
ZEND_API int zend_hash_exists(const HashTable *ht, zend_string *key);
ZEND_API int zend_hash_str_exists(const HashTable *ht, const char *str, int len);
ZEND_API int zend_hash_index_exists(const HashTable *ht, ulong h);
ZEND_API ulong zend_hash_next_free_element(const HashTable *ht);

/* traversing */
#define zend_hash_has_more_elements_ex(ht, pos) \
	(zend_hash_get_current_key_type_ex(ht, pos) == HASH_KEY_NON_EXISTENT ? FAILURE : SUCCESS)
ZEND_API int zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos);
ZEND_API int zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos);
ZEND_API int zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, ulong *num_index, zend_bool duplicate, HashPosition *pos);
ZEND_API void zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos);
ZEND_API int zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos);
ZEND_API zval *zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos);
ZEND_API void zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos);
ZEND_API void zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos);
ZEND_API int zend_hash_update_current_key_ex(HashTable *ht, int key_type, zend_string *str_index, ulong num_index, int mode, HashPosition *pos);

typedef struct _HashPointer {
	HashPosition pos;
	HashTable *ht;
	ulong h;
} HashPointer;

ZEND_API int zend_hash_get_pointer(const HashTable *ht, HashPointer *ptr);
ZEND_API int zend_hash_set_pointer(HashTable *ht, const HashPointer *ptr);

#define zend_hash_has_more_elements(ht) \
	zend_hash_has_more_elements_ex(ht, NULL)
#define zend_hash_move_forward(ht) \
	zend_hash_move_forward_ex(ht, NULL)
#define zend_hash_move_backwards(ht) \
	zend_hash_move_backwards_ex(ht, NULL)
#define zend_hash_get_current_key(ht, str_index, num_index, duplicate) \
	zend_hash_get_current_key_ex(ht, str_index, num_index, duplicate, NULL)
#define zend_hash_get_current_key_zval(ht, key) \
	zend_hash_get_current_key_zval_ex(ht, key, NULL)
#define zend_hash_get_current_key_type(ht) \
	zend_hash_get_current_key_type_ex(ht, NULL)
#define zend_hash_get_current_data(ht) \
	zend_hash_get_current_data_ex(ht, NULL)
#define zend_hash_internal_pointer_reset(ht) \
	zend_hash_internal_pointer_reset_ex(ht, NULL)
#define zend_hash_internal_pointer_end(ht) \
	zend_hash_internal_pointer_end_ex(ht, NULL)
#define zend_hash_update_current_key(ht, key_type, str_index, str_length, num_index) \
	zend_hash_update_current_key_ex(ht, key_type, str_index, str_length, num_index, HASH_UPDATE_KEY_ANYWAY, NULL)

/* Copying, merging and sorting */
ZEND_API void zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor);
ZEND_API void _zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, int overwrite ZEND_FILE_LINE_DC);
ZEND_API void zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam);
ZEND_API int zend_hash_sort(HashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber TSRMLS_DC);
ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, zend_bool ordered TSRMLS_DC);
ZEND_API zval *zend_hash_minmax(const HashTable *ht, compare_func_t compar, int flag TSRMLS_DC);

#define zend_hash_merge(target, source, pCopyConstructor, overwrite)					\
	_zend_hash_merge(target, source, pCopyConstructor, overwrite ZEND_FILE_LINE_CC)

ZEND_API int zend_hash_num_elements(const HashTable *ht);

ZEND_API int zend_hash_rehash(HashTable *ht);

#if ZEND_DEBUG
/* debug */
void zend_hash_display_pListTail(const HashTable *ht);
void zend_hash_display(const HashTable *ht);
#endif

END_EXTERN_C()

#define ZEND_INIT_SYMTABLE(ht)								\
	ZEND_INIT_SYMTABLE_EX(ht, 2, 0)

#define ZEND_INIT_SYMTABLE_EX(ht, n, persistent)			\
	zend_hash_init(ht, n, NULL, ZVAL_PTR_DTOR, persistent)

#define ZEND_HANDLE_NUMERIC_EX(key, length, idx, func) do {					\
	register const char *tmp = key;											\
																			\
	if (*tmp == '-') {														\
		tmp++;																\
	}																		\
	if (*tmp >= '0' && *tmp <= '9') { /* possibly a numeric index */		\
		const char *end = key + length - 1;									\
																			\
		if ((*end != '\0') /* not a null terminated string */				\
		 || (*tmp == '0' && length > 2) /* numbers with leading zeros */	\
		 || (end - tmp > MAX_LENGTH_OF_LONG - 1) /* number too long */		\
		 || (SIZEOF_LONG == 4 &&											\
		     end - tmp == MAX_LENGTH_OF_LONG - 1 &&							\
		     *tmp > '2')) { /* overflow */									\
			break;															\
		}																	\
		idx = (*tmp - '0');													\
		while (++tmp != end && *tmp >= '0' && *tmp <= '9') {				\
			idx = (idx * 10) + (*tmp - '0');								\
		}																	\
		if (tmp == end) {													\
			if (*key == '-') {												\
				if (idx-1 > LONG_MAX) { /* overflow */						\
					break;													\
				}															\
				idx = 0 - idx;               									\
			} else if (idx > LONG_MAX) { /* overflow */						\
				break;														\
			}																\
			func;															\
		}																	\
	}																		\
} while (0)

#define ZEND_HANDLE_NUMERIC(key, length, func) do {							\
	ulong idx;																\
																			\
	ZEND_HANDLE_NUMERIC_EX(key, length, idx, return func);					\
} while (0)

static inline zval *zend_symtable_update(HashTable *ht, zend_string *key, zval *pData)
{
	ZEND_HANDLE_NUMERIC(key->val, key->len+1, zend_hash_index_update(ht, idx, pData));
	return zend_hash_update(ht, key, pData);
}


static inline int zend_symtable_del(HashTable *ht, zend_string *key)
{
	ZEND_HANDLE_NUMERIC(key->val, key->len+1, zend_hash_index_del(ht, idx));
	return zend_hash_del(ht, key);
}


static inline zval *zend_symtable_find(const HashTable *ht, zend_string *key)
{
	ZEND_HANDLE_NUMERIC(key->val, key->len+1, zend_hash_index_find(ht, idx));
	return zend_hash_find(ht, key);
}


static inline int zend_symtable_exists(HashTable *ht, zend_string *key)
{
	ZEND_HANDLE_NUMERIC(key->val, key->len+1, zend_hash_index_exists(ht, idx));
	return zend_hash_exists(ht, key);
}

static inline zval *zend_symtable_str_update(HashTable *ht, const char *str, int len, zval *pData)
{
	ZEND_HANDLE_NUMERIC(str, len+1, zend_hash_index_update(ht, idx, pData));
	return zend_hash_str_update(ht, str, len, pData);
}


static inline int zend_symtable_str_del(HashTable *ht, const char *str, int len)
{
	ZEND_HANDLE_NUMERIC(str, len+1, zend_hash_index_del(ht, idx));
	return zend_hash_str_del(ht, str, len);
}


static inline zval *zend_symtable_str_find(HashTable *ht, const char *str, int len)
{
	ZEND_HANDLE_NUMERIC(str, len+1, zend_hash_index_find(ht, idx));
	return zend_hash_str_find(ht, str, len);
}


static inline int zend_symtable_str_exists(HashTable *ht, const char *str, int len)
{
	ZEND_HANDLE_NUMERIC(str, len+1, zend_hash_index_exists(ht, idx));
	return zend_hash_str_exists(ht, str, len);
}

static inline int zend_symtable_update_current_key_ex(HashTable *ht, zend_string *key, int mode, HashPosition *pos)
{
ZEND_HANDLE_NUMERIC(key->val, key->len+1, zend_hash_update_current_key_ex(ht, HASH_KEY_IS_LONG, NULL, idx, mode, pos));
	return zend_hash_update_current_key_ex(ht, HASH_KEY_IS_STRING, key, 0, mode, pos);
}
#define zend_symtable_update_current_key(ht, key, mode) \
	zend_symtable_update_current_key_ex(ht, key, mode, NULL)

static inline void *zend_hash_add_ptr(HashTable *ht, zend_string *key, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_add(ht, key, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_str_add_ptr(HashTable *ht, const char *str, int len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_str_add(ht, str, len, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_update_ptr(HashTable *ht, zend_string *key, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_update(ht, key, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_str_update_ptr(HashTable *ht, const char *str, int len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_str_update(ht, str, len, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_add_mem(HashTable *ht, zend_string *key, void *pData, size_t size)
{
	void *p, *r;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	if ((r = zend_hash_add_ptr(ht, key, p))) {
		return r;
	}
	pefree(p, ht->flags & HASH_FLAG_PERSISTENT);
	return NULL;
}

static inline void *zend_hash_str_add_mem(HashTable *ht, const char *str, int len, void *pData, size_t size)
{
	void *p, *r;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	if ((r = zend_hash_str_add_ptr(ht, str, len, p))) {
		return r;
	}
	pefree(p, ht->flags & HASH_FLAG_PERSISTENT);
	return NULL;
}

static inline void *zend_hash_update_mem(HashTable *ht, zend_string *key, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_update_ptr(ht, key, p);
}

static inline void *zend_hash_str_update_mem(HashTable *ht, const char *str, int len, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_str_update_ptr(ht, str, len, p);
}

static inline void *zend_hash_index_update_ptr(HashTable *ht, ulong h, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_index_update(ht, h, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_next_index_insert_ptr(HashTable *ht, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_next_index_insert(ht, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_index_update_mem(HashTable *ht, ulong h, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_index_update_ptr(ht, h, p);
}

static inline void *zend_hash_next_index_insert_mem(HashTable *ht, void *pData, size_t size)
{
	void *p, *r;

	p = pemalloc(size, ht->flags & HASH_FLAG_PERSISTENT);
	memcpy(p, pData, size);
	if ((r = zend_hash_next_index_insert_ptr(ht, p))) {
		return r;
	}
	pefree(p, ht->flags & HASH_FLAG_PERSISTENT);
	return NULL;
}

static inline void *zend_hash_find_ptr(const HashTable *ht, zend_string *key)
{
	zval *zv;

	zv = zend_hash_find(ht, key);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_str_find_ptr(const HashTable *ht, const char *str, int len)
{
	zval *zv;

	zv = zend_hash_str_find(ht, str, len);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_index_find_ptr(const HashTable *ht, ulong h)
{
	zval *zv;

	zv = zend_hash_index_find(ht, h);
	return zv ? Z_PTR_P(zv) : NULL;
}

static inline void *zend_hash_get_current_data_ptr_ex(HashTable *ht, HashPosition *pos)
{
	zval *zv;

	zv = zend_hash_get_current_data_ex(ht, pos);
	return zv ? Z_PTR_P(zv) : NULL;
}

#define zend_hash_get_current_data_ptr(ht) \
	zend_hash_get_current_data_ptr_ex(ht, NULL)

#endif							/* ZEND_HASH_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
