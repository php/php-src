/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_HASH_H
#define ZEND_HASH_H

#include "zend_types.h"
#include "zend_gc.h"
#include "zend_string.h"
#include "zend_sort.h"

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_LONG 2
#define HASH_KEY_NON_EXISTENT 3

#define HASH_UPDATE 			(1<<0)
#define HASH_ADD				(1<<1)
#define HASH_UPDATE_INDIRECT	(1<<2)
#define HASH_ADD_NEW			(1<<3)
#define HASH_ADD_NEXT			(1<<4)
#define HASH_LOOKUP				(1<<5)

#define HASH_FLAG_CONSISTENCY      ((1<<0) | (1<<1))
#define HASH_FLAG_PACKED           (1<<2)
#define HASH_FLAG_UNINITIALIZED    (1<<3)
#define HASH_FLAG_STATIC_KEYS      (1<<4) /* long and interned strings */
#define HASH_FLAG_HAS_EMPTY_IND    (1<<5)
#define HASH_FLAG_ALLOW_COW_VIOLATION (1<<6)

/* Only the low byte are real flags */
#define HASH_FLAG_MASK 0xff

#define HT_FLAGS(ht) (ht)->u.flags

#define HT_INVALIDATE(ht) do { \
		HT_FLAGS(ht) = HASH_FLAG_UNINITIALIZED; \
	} while (0)

#define HT_IS_INITIALIZED(ht) \
	((HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) == 0)

#define HT_IS_PACKED(ht) \
	((HT_FLAGS(ht) & HASH_FLAG_PACKED) != 0)

#define HT_IS_WITHOUT_HOLES(ht) \
	((ht)->nNumUsed == (ht)->nNumOfElements)

#define HT_HAS_STATIC_KEYS_ONLY(ht) \
	((HT_FLAGS(ht) & (HASH_FLAG_PACKED|HASH_FLAG_STATIC_KEYS)) != 0)

#if ZEND_DEBUG
# define HT_ALLOW_COW_VIOLATION(ht) HT_FLAGS(ht) |= HASH_FLAG_ALLOW_COW_VIOLATION
#else
# define HT_ALLOW_COW_VIOLATION(ht)
#endif

#define HT_ITERATORS_COUNT(ht) (ht)->u.v.nIteratorsCount
#define HT_ITERATORS_OVERFLOW(ht) (HT_ITERATORS_COUNT(ht) == 0xff)
#define HT_HAS_ITERATORS(ht) (HT_ITERATORS_COUNT(ht) != 0)

#define HT_SET_ITERATORS_COUNT(ht, iters) \
	do { HT_ITERATORS_COUNT(ht) = (iters); } while (0)
#define HT_INC_ITERATORS_COUNT(ht) \
	HT_SET_ITERATORS_COUNT(ht, HT_ITERATORS_COUNT(ht) + 1)
#define HT_DEC_ITERATORS_COUNT(ht) \
	HT_SET_ITERATORS_COUNT(ht, HT_ITERATORS_COUNT(ht) - 1)

extern ZEND_API const HashTable zend_empty_array;

#define ZVAL_EMPTY_ARRAY(z) do {						\
		zval *__z = (z);								\
		Z_ARR_P(__z) = (zend_array*)&zend_empty_array;	\
		Z_TYPE_INFO_P(__z) = IS_ARRAY; \
	} while (0)


typedef struct _zend_hash_key {
	zend_ulong h;
	zend_string *key;
} zend_hash_key;

typedef bool (*merge_checker_func_t)(HashTable *target_ht, zval *source_data, zend_hash_key *hash_key, void *pParam);

BEGIN_EXTERN_C()

/* startup/shutdown */
ZEND_API void ZEND_FASTCALL _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, bool persistent);
ZEND_API void ZEND_FASTCALL zend_hash_destroy(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_clean(HashTable *ht);

#define zend_hash_init(ht, nSize, pHashFunction, pDestructor, persistent) \
	_zend_hash_init((ht), (nSize), (pDestructor), (persistent))

ZEND_API void ZEND_FASTCALL zend_hash_real_init(HashTable *ht, bool packed);
ZEND_API void ZEND_FASTCALL zend_hash_real_init_packed(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_real_init_mixed(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_packed_to_hash(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_to_packed(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_extend(HashTable *ht, uint32_t nSize, bool packed);
ZEND_API void ZEND_FASTCALL zend_hash_discard(HashTable *ht, uint32_t nNumUsed);
ZEND_API void ZEND_FASTCALL zend_hash_packed_grow(HashTable *ht);

/* additions/updates/changes */
ZEND_API zval* ZEND_FASTCALL zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag);
ZEND_API zval* ZEND_FASTCALL zend_hash_update(HashTable *ht, zend_string *key,zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_update_ind(HashTable *ht, zend_string *key,zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_add(HashTable *ht, zend_string *key,zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_add_new(HashTable *ht, zend_string *key,zval *pData);

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_or_update(HashTable *ht, const char *key, size_t len, zval *pData, uint32_t flag);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_update(HashTable *ht, const char *key, size_t len, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_update_ind(HashTable *ht, const char *key, size_t len, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_add(HashTable *ht, const char *key, size_t len, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_new(HashTable *ht, const char *key, size_t len, zval *pData);

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag);
ZEND_API zval* ZEND_FASTCALL zend_hash_index_add(HashTable *ht, zend_ulong h, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_new(HashTable *ht, zend_ulong h, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_next_index_insert(HashTable *ht, zval *pData);
ZEND_API zval* ZEND_FASTCALL zend_hash_next_index_insert_new(HashTable *ht, zval *pData);

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h);
ZEND_API zval* ZEND_FASTCALL zend_hash_add_empty_element(HashTable *ht, zend_string *key);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_empty_element(HashTable *ht, const char *key, size_t len);

ZEND_API zval* ZEND_FASTCALL zend_hash_set_bucket_key(HashTable *ht, Bucket *p, zend_string *key);

#define ZEND_HASH_APPLY_KEEP				0
#define ZEND_HASH_APPLY_REMOVE				1<<0
#define ZEND_HASH_APPLY_STOP				1<<1

typedef int (*apply_func_t)(zval *pDest);
typedef int (*apply_func_arg_t)(zval *pDest, void *argument);
typedef int (*apply_func_args_t)(zval *pDest, int num_args, va_list args, zend_hash_key *hash_key);

ZEND_API void ZEND_FASTCALL zend_hash_graceful_destroy(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_graceful_reverse_destroy(HashTable *ht);
ZEND_API void ZEND_FASTCALL zend_hash_apply(HashTable *ht, apply_func_t apply_func);
ZEND_API void ZEND_FASTCALL zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *);
ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int, ...);

/* This function should be used with special care (in other words,
 * it should usually not be used).  When used with the ZEND_HASH_APPLY_STOP
 * return value, it assumes things about the order of the elements in the hash.
 * Also, it does not provide the same kind of reentrancy protection that
 * the standard apply functions do.
 */
ZEND_API void ZEND_FASTCALL zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func);


/* Deletes */
ZEND_API zend_result ZEND_FASTCALL zend_hash_del(HashTable *ht, zend_string *key);
ZEND_API zend_result ZEND_FASTCALL zend_hash_del_ind(HashTable *ht, zend_string *key);
ZEND_API zend_result ZEND_FASTCALL zend_hash_str_del(HashTable *ht, const char *key, size_t len);
ZEND_API zend_result ZEND_FASTCALL zend_hash_str_del_ind(HashTable *ht, const char *key, size_t len);
ZEND_API zend_result ZEND_FASTCALL zend_hash_index_del(HashTable *ht, zend_ulong h);
ZEND_API void ZEND_FASTCALL zend_hash_del_bucket(HashTable *ht, Bucket *p);
ZEND_API void ZEND_FASTCALL zend_hash_packed_del_val(HashTable *ht, zval *zv);

/* Data retrieval */
ZEND_API zval* ZEND_FASTCALL zend_hash_find(const HashTable *ht, zend_string *key);
ZEND_API zval* ZEND_FASTCALL zend_hash_str_find(const HashTable *ht, const char *key, size_t len);
ZEND_API zval* ZEND_FASTCALL zend_hash_index_find(const HashTable *ht, zend_ulong h);
ZEND_API zval* ZEND_FASTCALL _zend_hash_index_find(const HashTable *ht, zend_ulong h);

/* The same as zend_hash_find(), but hash value of the key must be already calculated. */
ZEND_API zval* ZEND_FASTCALL zend_hash_find_known_hash(const HashTable *ht, const zend_string *key);

static zend_always_inline zval *zend_hash_find_ex(const HashTable *ht, zend_string *key, bool known_hash)
{
	if (known_hash) {
		return zend_hash_find_known_hash(ht, key);
	} else {
		return zend_hash_find(ht, key);
	}
}

#define ZEND_HASH_INDEX_FIND(_ht, _h, _ret, _not_found) do { \
		if (EXPECTED(HT_IS_PACKED(_ht))) { \
			if (EXPECTED((zend_ulong)(_h) < (zend_ulong)(_ht)->nNumUsed)) { \
				_ret = &_ht->arPacked[_h]; \
				if (UNEXPECTED(Z_TYPE_P(_ret) == IS_UNDEF)) { \
					goto _not_found; \
				} \
			} else { \
				goto _not_found; \
			} \
		} else { \
			_ret = _zend_hash_index_find(_ht, _h); \
			if (UNEXPECTED(_ret == NULL)) { \
				goto _not_found; \
			} \
		} \
	} while (0)


/* Find or add NULL, if doesn't exist */
ZEND_API zval* ZEND_FASTCALL zend_hash_lookup(HashTable *ht, zend_string *key);
ZEND_API zval* ZEND_FASTCALL zend_hash_index_lookup(HashTable *ht, zend_ulong h);

#define ZEND_HASH_INDEX_LOOKUP(_ht, _h, _ret) do { \
		if (EXPECTED(HT_IS_PACKED(_ht))) { \
			if (EXPECTED((zend_ulong)(_h) < (zend_ulong)(_ht)->nNumUsed)) { \
				_ret = &_ht->arPacked[_h]; \
				if (EXPECTED(Z_TYPE_P(_ret) != IS_UNDEF)) { \
					break; \
				} \
			} \
		} \
		_ret = zend_hash_index_lookup(_ht, _h); \
	} while (0)

/* Misc */
static zend_always_inline bool zend_hash_exists(const HashTable *ht, zend_string *key)
{
	return zend_hash_find(ht, key) != NULL;
}

static zend_always_inline bool zend_hash_str_exists(const HashTable *ht, const char *str, size_t len)
{
	return zend_hash_str_find(ht, str, len) != NULL;
}

static zend_always_inline bool zend_hash_index_exists(const HashTable *ht, zend_ulong h)
{
	return zend_hash_index_find(ht, h) != NULL;
}

/* traversing */
ZEND_API HashPosition ZEND_FASTCALL zend_hash_get_current_pos(const HashTable *ht);

ZEND_API zend_result   ZEND_FASTCALL zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos);
ZEND_API zend_result   ZEND_FASTCALL zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos);
ZEND_API int   ZEND_FASTCALL zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, zend_ulong *num_index, const HashPosition *pos);
ZEND_API void  ZEND_FASTCALL zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, const HashPosition *pos);
ZEND_API int   ZEND_FASTCALL zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos);
ZEND_API zval* ZEND_FASTCALL zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos);
ZEND_API void  ZEND_FASTCALL zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos);
ZEND_API void  ZEND_FASTCALL zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos);

static zend_always_inline zend_result zend_hash_has_more_elements_ex(HashTable *ht, HashPosition *pos) {
	return (zend_hash_get_current_key_type_ex(ht, pos) == HASH_KEY_NON_EXISTENT ? FAILURE : SUCCESS);
}
static zend_always_inline zend_result zend_hash_has_more_elements(HashTable *ht) {
	return zend_hash_has_more_elements_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline zend_result zend_hash_move_forward(HashTable *ht) {
	return zend_hash_move_forward_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline zend_result zend_hash_move_backwards(HashTable *ht) {
	return zend_hash_move_backwards_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline int zend_hash_get_current_key(const HashTable *ht, zend_string **str_index, zend_ulong *num_index) {
	return zend_hash_get_current_key_ex(ht, str_index, num_index, &ht->nInternalPointer);
}
static zend_always_inline void zend_hash_get_current_key_zval(const HashTable *ht, zval *key) {
	zend_hash_get_current_key_zval_ex(ht, key, &ht->nInternalPointer);
}
static zend_always_inline int zend_hash_get_current_key_type(HashTable *ht) {
	return zend_hash_get_current_key_type_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline zval* zend_hash_get_current_data(HashTable *ht) {
	return zend_hash_get_current_data_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline void zend_hash_internal_pointer_reset(HashTable *ht) {
	zend_hash_internal_pointer_reset_ex(ht, &ht->nInternalPointer);
}
static zend_always_inline void zend_hash_internal_pointer_end(HashTable *ht) {
	zend_hash_internal_pointer_end_ex(ht, &ht->nInternalPointer);
}

/* Copying, merging and sorting */
ZEND_API void  ZEND_FASTCALL zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor);
ZEND_API void  ZEND_FASTCALL zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, bool overwrite);
ZEND_API void  ZEND_FASTCALL zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam);
ZEND_API void  zend_hash_bucket_swap(Bucket *p, Bucket *q);
ZEND_API void  zend_hash_bucket_renum_swap(Bucket *p, Bucket *q);
ZEND_API void  zend_hash_bucket_packed_swap(Bucket *p, Bucket *q);

typedef int (*bucket_compare_func_t)(Bucket *a, Bucket *b);
ZEND_API int   zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, bool ordered);
ZEND_API void  ZEND_FASTCALL zend_hash_sort_ex(HashTable *ht, sort_func_t sort_func, bucket_compare_func_t compare_func, bool renumber);
ZEND_API zval* ZEND_FASTCALL zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag);

static zend_always_inline void ZEND_FASTCALL zend_hash_sort(HashTable *ht, bucket_compare_func_t compare_func, bool renumber) {
	zend_hash_sort_ex(ht, zend_sort, compare_func, renumber);
}

static zend_always_inline uint32_t zend_hash_num_elements(const HashTable *ht) {
	return ht->nNumOfElements;
}

static zend_always_inline zend_long zend_hash_next_free_element(const HashTable *ht) {
	return ht->nNextFreeElement;
}

ZEND_API void ZEND_FASTCALL zend_hash_rehash(HashTable *ht);

#if !ZEND_DEBUG && defined(HAVE_BUILTIN_CONSTANT_P)
# define zend_new_array(size) \
	(__builtin_constant_p(size) ? \
		((((uint32_t)(size)) <= HT_MIN_SIZE) ? \
			_zend_new_array_0() \
		: \
			_zend_new_array((size)) \
		) \
	: \
		_zend_new_array((size)) \
	)
#else
# define zend_new_array(size) \
	_zend_new_array(size)
#endif

ZEND_API HashTable* ZEND_FASTCALL _zend_new_array_0(void);
ZEND_API HashTable* ZEND_FASTCALL _zend_new_array(uint32_t size);
ZEND_API HashTable* ZEND_FASTCALL zend_new_pair(zval *val1, zval *val2);
ZEND_API uint32_t zend_array_count(HashTable *ht);
ZEND_API HashTable* ZEND_FASTCALL zend_array_dup(HashTable *source);
ZEND_API void ZEND_FASTCALL zend_array_destroy(HashTable *ht);
ZEND_API HashTable* zend_array_to_list(HashTable *source);
ZEND_API void ZEND_FASTCALL zend_symtable_clean(HashTable *ht);
ZEND_API HashTable* ZEND_FASTCALL zend_symtable_to_proptable(HashTable *ht);
ZEND_API HashTable* ZEND_FASTCALL zend_proptable_to_symtable(HashTable *ht, bool always_duplicate);

ZEND_API bool ZEND_FASTCALL _zend_handle_numeric_str_ex(const char *key, size_t length, zend_ulong *idx);

ZEND_API uint32_t     ZEND_FASTCALL zend_hash_iterator_add(HashTable *ht, HashPosition pos);
ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos(uint32_t idx, HashTable *ht);
ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos_ex(uint32_t idx, zval *array);
ZEND_API void         ZEND_FASTCALL zend_hash_iterator_del(uint32_t idx);
ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterators_lower_pos(HashTable *ht, HashPosition start);
ZEND_API void         ZEND_FASTCALL _zend_hash_iterators_update(HashTable *ht, HashPosition from, HashPosition to);
ZEND_API void         ZEND_FASTCALL zend_hash_iterators_advance(HashTable *ht, HashPosition step);

static zend_always_inline void zend_hash_iterators_update(HashTable *ht, HashPosition from, HashPosition to)
{
	if (UNEXPECTED(HT_HAS_ITERATORS(ht))) {
		_zend_hash_iterators_update(ht, from, to);
	}
}

/* For regular arrays (non-persistent, storing zvals). */
static zend_always_inline void zend_array_release(zend_array *array)
{
	if (!(GC_FLAGS(array) & IS_ARRAY_IMMUTABLE)) {
		if (GC_DELREF(array) == 0) {
			zend_array_destroy(array);
		}
	}
}

/* For general hashes (possibly persistent, storing any kind of value). */
static zend_always_inline void zend_hash_release(zend_array *array)
{
	if (!(GC_FLAGS(array) & IS_ARRAY_IMMUTABLE)) {
		if (GC_DELREF(array) == 0) {
			zend_hash_destroy(array);
			pefree(array, GC_FLAGS(array) & IS_ARRAY_PERSISTENT);
		}
	}
}

END_EXTERN_C()

#define ZEND_INIT_SYMTABLE(ht)								\
	ZEND_INIT_SYMTABLE_EX(ht, 8, 0)

#define ZEND_INIT_SYMTABLE_EX(ht, n, persistent)			\
	zend_hash_init(ht, n, NULL, ZVAL_PTR_DTOR, persistent)

static zend_always_inline bool _zend_handle_numeric_str(const char *key, size_t length, zend_ulong *idx)
{
	const char *tmp = key;

	if (EXPECTED(*tmp > '9')) {
		return 0;
	} else if (*tmp < '0') {
		if (*tmp != '-') {
			return 0;
		}
		tmp++;
		if (*tmp > '9' || *tmp < '0') {
			return 0;
		}
	}
	return _zend_handle_numeric_str_ex(key, length, idx);
}

#define ZEND_HANDLE_NUMERIC_STR(key, length, idx) \
	_zend_handle_numeric_str(key, length, &idx)

#define ZEND_HANDLE_NUMERIC(key, idx) \
	ZEND_HANDLE_NUMERIC_STR(ZSTR_VAL(key), ZSTR_LEN(key), idx)


static zend_always_inline zval *zend_hash_find_ind(const HashTable *ht, zend_string *key)
{
	zval *zv;

	zv = zend_hash_find(ht, key);
	return (zv && Z_TYPE_P(zv) == IS_INDIRECT) ?
		((Z_TYPE_P(Z_INDIRECT_P(zv)) != IS_UNDEF) ? Z_INDIRECT_P(zv) : NULL) : zv;
}


static zend_always_inline zval *zend_hash_find_ex_ind(const HashTable *ht, zend_string *key, bool known_hash)
{
	zval *zv;

	zv = zend_hash_find_ex(ht, key, known_hash);
	return (zv && Z_TYPE_P(zv) == IS_INDIRECT) ?
		((Z_TYPE_P(Z_INDIRECT_P(zv)) != IS_UNDEF) ? Z_INDIRECT_P(zv) : NULL) : zv;
}


static zend_always_inline bool zend_hash_exists_ind(const HashTable *ht, zend_string *key)
{
	zval *zv;

	zv = zend_hash_find(ht, key);
	return zv && (Z_TYPE_P(zv) != IS_INDIRECT ||
			Z_TYPE_P(Z_INDIRECT_P(zv)) != IS_UNDEF);
}


static zend_always_inline zval *zend_hash_str_find_ind(const HashTable *ht, const char *str, size_t len)
{
	zval *zv;

	zv = zend_hash_str_find(ht, str, len);
	return (zv && Z_TYPE_P(zv) == IS_INDIRECT) ?
		((Z_TYPE_P(Z_INDIRECT_P(zv)) != IS_UNDEF) ? Z_INDIRECT_P(zv) : NULL) : zv;
}


static zend_always_inline bool zend_hash_str_exists_ind(const HashTable *ht, const char *str, size_t len)
{
	zval *zv;

	zv = zend_hash_str_find(ht, str, len);
	return zv && (Z_TYPE_P(zv) != IS_INDIRECT ||
			Z_TYPE_P(Z_INDIRECT_P(zv)) != IS_UNDEF);
}

static zend_always_inline zval *zend_symtable_add_new(HashTable *ht, zend_string *key, zval *pData)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_add_new(ht, idx, pData);
	} else {
		return zend_hash_add_new(ht, key, pData);
	}
}

static zend_always_inline zval *zend_symtable_update(HashTable *ht, zend_string *key, zval *pData)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_update(ht, idx, pData);
	} else {
		return zend_hash_update(ht, key, pData);
	}
}


static zend_always_inline zval *zend_symtable_update_ind(HashTable *ht, zend_string *key, zval *pData)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_update(ht, idx, pData);
	} else {
		return zend_hash_update_ind(ht, key, pData);
	}
}


static zend_always_inline zend_result zend_symtable_del(HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_del(ht, idx);
	} else {
		return zend_hash_del(ht, key);
	}
}


static zend_always_inline zend_result zend_symtable_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_del(ht, idx);
	} else {
		return zend_hash_del_ind(ht, key);
	}
}


static zend_always_inline zval *zend_symtable_find(const HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_find(ht, idx);
	} else {
		return zend_hash_find(ht, key);
	}
}


static zend_always_inline zval *zend_symtable_find_ind(const HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_find(ht, idx);
	} else {
		return zend_hash_find_ind(ht, key);
	}
}


static zend_always_inline bool zend_symtable_exists(HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_exists(ht, idx);
	} else {
		return zend_hash_exists(ht, key);
	}
}


static zend_always_inline bool zend_symtable_exists_ind(HashTable *ht, zend_string *key)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC(key, idx)) {
		return zend_hash_index_exists(ht, idx);
	} else {
		return zend_hash_exists_ind(ht, key);
	}
}


static zend_always_inline zval *zend_symtable_str_update(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_update(ht, idx, pData);
	} else {
		return zend_hash_str_update(ht, str, len, pData);
	}
}


static zend_always_inline zval *zend_symtable_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_update(ht, idx, pData);
	} else {
		return zend_hash_str_update_ind(ht, str, len, pData);
	}
}


static zend_always_inline zend_result zend_symtable_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_del(ht, idx);
	} else {
		return zend_hash_str_del(ht, str, len);
	}
}


static zend_always_inline zend_result zend_symtable_str_del_ind(HashTable *ht, const char *str, size_t len)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_del(ht, idx);
	} else {
		return zend_hash_str_del_ind(ht, str, len);
	}
}


static zend_always_inline zval *zend_symtable_str_find(HashTable *ht, const char *str, size_t len)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_find(ht, idx);
	} else {
		return zend_hash_str_find(ht, str, len);
	}
}


static zend_always_inline bool zend_symtable_str_exists(HashTable *ht, const char *str, size_t len)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_exists(ht, idx);
	} else {
		return zend_hash_str_exists(ht, str, len);
	}
}

static zend_always_inline void *zend_hash_add_ptr(HashTable *ht, zend_string *key, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_add(ht, key, &tmp);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_add_new_ptr(HashTable *ht, zend_string *key, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_add_new(ht, key, &tmp);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_str_add_ptr(HashTable *ht, const char *str, size_t len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_str_add(ht, str, len, &tmp);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_str_add_new_ptr(HashTable *ht, const char *str, size_t len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_str_add_new(ht, str, len, &tmp);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_update_ptr(HashTable *ht, zend_string *key, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_update(ht, key, &tmp);
	ZEND_ASSUME(Z_PTR_P(zv));
	return Z_PTR_P(zv);
}

static zend_always_inline void *zend_hash_str_update_ptr(HashTable *ht, const char *str, size_t len, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_str_update(ht, str, len, &tmp);
	ZEND_ASSUME(Z_PTR_P(zv));
	return Z_PTR_P(zv);
}

static zend_always_inline void *zend_hash_add_mem(HashTable *ht, zend_string *key, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_add(ht, key, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_add_new_mem(HashTable *ht, zend_string *key, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_add_new(ht, key, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_str_add_mem(HashTable *ht, const char *str, size_t len, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_str_add(ht, str, len, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_str_add_new_mem(HashTable *ht, const char *str, size_t len, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_str_add_new(ht, str, len, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_update_mem(HashTable *ht, zend_string *key, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_update_ptr(ht, key, p);
}

static zend_always_inline void *zend_hash_str_update_mem(HashTable *ht, const char *str, size_t len, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_str_update_ptr(ht, str, len, p);
}

static zend_always_inline void *zend_hash_index_add_ptr(HashTable *ht, zend_ulong h, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_index_add(ht, h, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static zend_always_inline void *zend_hash_index_add_new_ptr(HashTable *ht, zend_ulong h, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_index_add_new(ht, h, &tmp);
	return zv ? Z_PTR_P(zv) : NULL;
}

static zend_always_inline void *zend_hash_index_update_ptr(HashTable *ht, zend_ulong h, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_index_update(ht, h, &tmp);
	ZEND_ASSUME(Z_PTR_P(zv));
	return Z_PTR_P(zv);
}

static zend_always_inline void *zend_hash_index_add_mem(HashTable *ht, zend_ulong h, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_index_add(ht, h, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_next_index_insert_ptr(HashTable *ht, void *pData)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, pData);
	zv = zend_hash_next_index_insert(ht, &tmp);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_index_update_mem(HashTable *ht, zend_ulong h, void *pData, size_t size)
{
	void *p;

	p = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	memcpy(p, pData, size);
	return zend_hash_index_update_ptr(ht, h, p);
}

static zend_always_inline void *zend_hash_next_index_insert_mem(HashTable *ht, void *pData, size_t size)
{
	zval tmp, *zv;

	ZVAL_PTR(&tmp, NULL);
	if ((zv = zend_hash_next_index_insert(ht, &tmp))) {
		Z_PTR_P(zv) = pemalloc(size, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		memcpy(Z_PTR_P(zv), pData, size);
		return Z_PTR_P(zv);
	}
	return NULL;
}

static zend_always_inline void *zend_hash_find_ptr(const HashTable *ht, zend_string *key)
{
	zval *zv;

	zv = zend_hash_find(ht, key);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_find_ex_ptr(const HashTable *ht, zend_string *key, bool known_hash)
{
	zval *zv;

	zv = zend_hash_find_ex(ht, key, known_hash);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline void *zend_hash_str_find_ptr(const HashTable *ht, const char *str, size_t len)
{
	zval *zv;

	zv = zend_hash_str_find(ht, str, len);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

/* Will lowercase the str; use only if you don't need the lowercased string for
 * anything else. If you have a lowered string, use zend_hash_str_find_ptr. */
ZEND_API void *zend_hash_str_find_ptr_lc(const HashTable *ht, const char *str, size_t len);

/* Will lowercase the str; use only if you don't need the lowercased string for
 * anything else. If you have a lowered string, use zend_hash_find_ptr. */
ZEND_API void *zend_hash_find_ptr_lc(const HashTable *ht, zend_string *key);

static zend_always_inline void *zend_hash_index_find_ptr(const HashTable *ht, zend_ulong h)
{
	zval *zv;

	zv = zend_hash_index_find(ht, h);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

static zend_always_inline zval *zend_hash_index_find_deref(HashTable *ht, zend_ulong h)
{
	zval *zv = zend_hash_index_find(ht, h);
	if (zv) {
		ZVAL_DEREF(zv);
	}
	return zv;
}

static zend_always_inline zval *zend_hash_find_deref(HashTable *ht, zend_string *str)
{
	zval *zv = zend_hash_find(ht, str);
	if (zv) {
		ZVAL_DEREF(zv);
	}
	return zv;
}

static zend_always_inline zval *zend_hash_str_find_deref(HashTable *ht, const char *str, size_t len)
{
	zval *zv = zend_hash_str_find(ht, str, len);
	if (zv) {
		ZVAL_DEREF(zv);
	}
	return zv;
}

static zend_always_inline void *zend_symtable_str_find_ptr(HashTable *ht, const char *str, size_t len)
{
	zend_ulong idx;

	if (ZEND_HANDLE_NUMERIC_STR(str, len, idx)) {
		return zend_hash_index_find_ptr(ht, idx);
	} else {
		return zend_hash_str_find_ptr(ht, str, len);
	}
}

static zend_always_inline void *zend_hash_get_current_data_ptr_ex(HashTable *ht, HashPosition *pos)
{
	zval *zv;

	zv = zend_hash_get_current_data_ex(ht, pos);
	if (zv) {
		ZEND_ASSUME(Z_PTR_P(zv));
		return Z_PTR_P(zv);
	} else {
		return NULL;
	}
}

#define zend_hash_get_current_data_ptr(ht) \
	zend_hash_get_current_data_ptr_ex(ht, &(ht)->nInternalPointer)

/* Common hash/packed array iterators */
#if 0
# define ZEND_HASH_ELEMENT_SIZE(__ht) \
	(HT_IS_PACKED(__ht) ? sizeof(zval) : sizeof(Bucket))
#else /* optimized version */
# define ZEND_HASH_ELEMENT_SIZE(__ht) \
	(sizeof(zval) + (~HT_FLAGS(__ht) & HASH_FLAG_PACKED) * ((sizeof(Bucket)-sizeof(zval))/HASH_FLAG_PACKED))
#endif

#define ZEND_HASH_ELEMENT_EX(__ht, _idx, _size) \
	((zval*)(((char*)(__ht)->arPacked) + ((_idx) * (_size))))

#define ZEND_HASH_ELEMENT(__ht, _idx) \
	ZEND_HASH_ELEMENT_EX(__ht, _idx, ZEND_HASH_ELEMENT_SIZE(__ht))

#define ZEND_HASH_NEXT_ELEMENT(_el, _size) \
	((zval*)(((char*)(_el)) + (_size)))

#define ZEND_HASH_PREV_ELEMENT(_el, _size) \
	((zval*)(((char*)(_el)) - (_size)))

#define _ZEND_HASH_FOREACH_VAL(_ht) do { \
		const HashTable *__ht = (_ht); \
		uint32_t _count = __ht->nNumUsed; \
		size_t _size = ZEND_HASH_ELEMENT_SIZE(__ht); \
		zval *_z = __ht->arPacked; \
		for (; _count > 0; _z = ZEND_HASH_NEXT_ELEMENT(_z, _size), _count--) { \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define _ZEND_HASH_REVERSE_FOREACH_VAL(_ht) do { \
		const HashTable *__ht = (_ht); \
		uint32_t _idx = __ht->nNumUsed; \
		size_t _size = ZEND_HASH_ELEMENT_SIZE(__ht); \
		zval *_z = ZEND_HASH_ELEMENT_EX(__ht, _idx, _size); \
		for (;_idx > 0; _idx--) { \
			_z = ZEND_HASH_PREV_ELEMENT(_z, _size); \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_FOREACH_FROM(_ht, indirect, _from) do { \
		const HashTable *__ht = (_ht); \
		zend_ulong __h; \
		zend_string *__key = NULL; \
		uint32_t _idx = (_from); \
		size_t _size = ZEND_HASH_ELEMENT_SIZE(__ht); \
		zval *__z = ZEND_HASH_ELEMENT_EX(__ht, _idx, _size); \
		uint32_t _count = __ht->nNumUsed - _idx; \
		for (;_count > 0; _count--) { \
			zval *_z = __z; \
			if (HT_IS_PACKED(__ht)) { \
				__z++; \
				__h = _idx; \
				_idx++; \
			} else { \
				Bucket *_p = (Bucket*)__z; \
				__z = &(_p + 1)->val; \
				__h = _p->h; \
				__key = _p->key; \
				if (indirect && Z_TYPE_P(_z) == IS_INDIRECT) { \
					_z = Z_INDIRECT_P(_z); \
				} \
			} \
			(void) __h; (void) __key; (void) _idx; \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_FOREACH(_ht, indirect) ZEND_HASH_FOREACH_FROM(_ht, indirect, 0)

#define ZEND_HASH_REVERSE_FOREACH(_ht, indirect) do { \
		const HashTable *__ht = (_ht); \
		uint32_t _idx = __ht->nNumUsed; \
		zval *_z; \
		zend_ulong __h; \
		zend_string *__key = NULL; \
		size_t _size = ZEND_HASH_ELEMENT_SIZE(__ht); \
		zval *__z = ZEND_HASH_ELEMENT_EX(__ht, _idx, _size); \
		for (;_idx > 0; _idx--) { \
			if (HT_IS_PACKED(__ht)) { \
				__z--; \
				_z = __z; \
				__h = _idx - 1; \
			} else { \
				Bucket *_p = (Bucket*)__z; \
				_p--; \
				__z = &_p->val; \
				_z = __z; \
				__h = _p->h; \
				__key = _p->key; \
				if (indirect && Z_TYPE_P(_z) == IS_INDIRECT) { \
					_z = Z_INDIRECT_P(_z); \
				} \
			} \
			(void) __h; (void) __key; (void) __z; \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_FOREACH_END() \
		} \
	} while (0)

#define ZEND_HASH_FOREACH_END_DEL() \
	ZEND_HASH_MAP_FOREACH_END_DEL()

#define ZEND_HASH_FOREACH_BUCKET(ht, _bucket) \
	ZEND_HASH_MAP_FOREACH_BUCKET(ht, _bucket)

#define ZEND_HASH_FOREACH_BUCKET_FROM(ht, _bucket, _from) \
	ZEND_HASH_MAP_FOREACH_BUCKET_FROM(ht, _bucket, _from)

#define ZEND_HASH_REVERSE_FOREACH_BUCKET(ht, _bucket) \
	ZEND_HASH_MAP_REVERSE_FOREACH_BUCKET(ht, _bucket)

#define ZEND_HASH_FOREACH_VAL(ht, _val) \
	_ZEND_HASH_FOREACH_VAL(ht); \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_VAL(ht, _val) \
	_ZEND_HASH_REVERSE_FOREACH_VAL(ht); \
	_val = _z;

#define ZEND_HASH_FOREACH_VAL_IND(ht, _val) \
	ZEND_HASH_FOREACH(ht, 1); \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_VAL_IND(ht, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 1); \
	_val = _z;

#define ZEND_HASH_FOREACH_PTR(ht, _ptr) \
	_ZEND_HASH_FOREACH_VAL(ht); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_FOREACH_PTR_FROM(ht, _ptr, _from) \
	ZEND_HASH_FOREACH_FROM(ht, 0, _from); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_REVERSE_FOREACH_PTR(ht, _ptr) \
	_ZEND_HASH_REVERSE_FOREACH_VAL(ht); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_FOREACH_NUM_KEY(ht, _h) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h;

#define ZEND_HASH_REVERSE_FOREACH_NUM_KEY(ht, _h) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h;

#define ZEND_HASH_FOREACH_STR_KEY(ht, _key) \
	ZEND_HASH_FOREACH(ht, 0); \
	_key = __key;

#define ZEND_HASH_REVERSE_FOREACH_STR_KEY(ht, _key) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_key = __key;

#define ZEND_HASH_FOREACH_KEY(ht, _h, _key) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key;

#define ZEND_HASH_REVERSE_FOREACH_KEY(ht, _h, _key) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key;

#define ZEND_HASH_FOREACH_NUM_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h; \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_NUM_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h; \
	_val = _z;

#define ZEND_HASH_FOREACH_STR_KEY_VAL(ht, _key, _val) \
	ZEND_HASH_FOREACH(ht, 0); \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_FOREACH_STR_KEY_VAL_FROM(ht, _key, _val, _from) \
	ZEND_HASH_FOREACH_FROM(ht, 0, _from); \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL(ht, _key, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_FOREACH_KEY_VAL(ht, _h, _key, _val) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_KEY_VAL(ht, _h, _key, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_FOREACH_STR_KEY_VAL_IND(ht, _key, _val) \
	ZEND_HASH_FOREACH(ht, 1); \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL_IND(ht, _key, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 1); \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_FOREACH_KEY_VAL_IND(ht, _h, _key, _val) \
	ZEND_HASH_FOREACH(ht, 1); \
	_h = __h; \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_REVERSE_FOREACH_KEY_VAL_IND(ht, _h, _key, _val) \
	ZEND_HASH_REVERSE_FOREACH(ht, 1); \
	_h = __h; \
	_key = __key; \
	_val = _z;

#define ZEND_HASH_FOREACH_NUM_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_REVERSE_FOREACH_NUM_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_FOREACH_STR_KEY_PTR(ht, _key, _ptr) \
	ZEND_HASH_FOREACH(ht, 0); \
	_key = __key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_REVERSE_FOREACH_STR_KEY_PTR(ht, _key, _ptr) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_key = __key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
	ZEND_HASH_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_REVERSE_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
	ZEND_HASH_REVERSE_FOREACH(ht, 0); \
	_h = __h; \
	_key = __key; \
	_ptr = Z_PTR_P(_z);

/* Hash array iterators */
#define ZEND_HASH_MAP_FOREACH_FROM(_ht, indirect, _from) do { \
		const HashTable *__ht = (_ht); \
		Bucket *_p = __ht->arData + (_from); \
		const Bucket *_end = __ht->arData + __ht->nNumUsed; \
		ZEND_ASSERT(!HT_IS_PACKED(__ht)); \
		for (; _p != _end; _p++) { \
			zval *_z = &_p->val; \
			if (indirect && Z_TYPE_P(_z) == IS_INDIRECT) { \
				_z = Z_INDIRECT_P(_z); \
			} \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_MAP_FOREACH(_ht, indirect) ZEND_HASH_MAP_FOREACH_FROM(_ht, indirect, 0)

#define ZEND_HASH_MAP_REVERSE_FOREACH(_ht, indirect) do { \
		/* const */ HashTable *__ht = (_ht); \
		uint32_t _idx = __ht->nNumUsed; \
		Bucket *_p = __ht->arData + _idx; \
		zval *_z; \
		ZEND_ASSERT(!HT_IS_PACKED(__ht)); \
		for (_idx = __ht->nNumUsed; _idx > 0; _idx--) { \
			_p--; \
			_z = &_p->val; \
			if (indirect && Z_TYPE_P(_z) == IS_INDIRECT) { \
				_z = Z_INDIRECT_P(_z); \
			} \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_MAP_FOREACH_END_DEL() \
			ZEND_ASSERT(!HT_IS_PACKED(__ht)); \
			__ht->nNumOfElements--; \
			do { \
				uint32_t j = HT_IDX_TO_HASH(_idx - 1); \
				uint32_t nIndex = _p->h | __ht->nTableMask; \
				uint32_t i = HT_HASH(__ht, nIndex); \
				if (UNEXPECTED(j != i)) { \
					Bucket *prev = HT_HASH_TO_BUCKET(__ht, i); \
					while (Z_NEXT(prev->val) != j) { \
						i = Z_NEXT(prev->val); \
						prev = HT_HASH_TO_BUCKET(__ht, i); \
					} \
					Z_NEXT(prev->val) = Z_NEXT(_p->val); \
				} else { \
					HT_HASH(__ht, nIndex) = Z_NEXT(_p->val); \
				} \
			} while (0); \
		} \
		__ht->nNumUsed = _idx; \
	} while (0)

#define ZEND_HASH_MAP_FOREACH_BUCKET(ht, _bucket) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_bucket = _p;

#define ZEND_HASH_MAP_FOREACH_BUCKET_FROM(ht, _bucket, _from) \
	ZEND_HASH_MAP_FOREACH_FROM(ht, 0, _from); \
	_bucket = _p;

#define ZEND_HASH_MAP_REVERSE_FOREACH_BUCKET(ht, _bucket) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_bucket = _p;

#define ZEND_HASH_MAP_FOREACH_VAL(ht, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_VAL(ht, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_VAL_IND(ht, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 1); \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_VAL_IND(ht, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 1); \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_PTR(ht, _ptr) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_FOREACH_PTR_FROM(ht, _ptr, _from) \
	ZEND_HASH_MAP_FOREACH_FROM(ht, 0, _from); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_REVERSE_FOREACH_PTR(ht, _ptr) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_FOREACH_NUM_KEY(ht, _h) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h;

#define ZEND_HASH_MAP_REVERSE_FOREACH_NUM_KEY(ht, _h) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h;

#define ZEND_HASH_MAP_FOREACH_STR_KEY(ht, _key) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_key = _p->key;

#define ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY(ht, _key) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_key = _p->key;

#define ZEND_HASH_MAP_FOREACH_KEY(ht, _h, _key) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key;

#define ZEND_HASH_MAP_REVERSE_FOREACH_KEY(ht, _h, _key) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key;

#define ZEND_HASH_MAP_FOREACH_NUM_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h; \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_NUM_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(ht, _key, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_STR_KEY_VAL_FROM(ht, _key, _val, _from) \
	ZEND_HASH_MAP_FOREACH_FROM(ht, 0, _from); \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL(ht, _key, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_KEY_VAL(ht, _h, _key, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_KEY_VAL(ht, _h, _key, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_STR_KEY_VAL_IND(ht, _key, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 1); \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL_IND(ht, _key, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 1); \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_KEY_VAL_IND(ht, _h, _key, _val) \
	ZEND_HASH_MAP_FOREACH(ht, 1); \
	_h = _p->h; \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_REVERSE_FOREACH_KEY_VAL_IND(ht, _h, _key, _val) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 1); \
	_h = _p->h; \
	_key = _p->key; \
	_val = _z;

#define ZEND_HASH_MAP_FOREACH_NUM_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_REVERSE_FOREACH_NUM_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(ht, _key, _ptr) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_key = _p->key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_PTR(ht, _key, _ptr) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_key = _p->key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
	ZEND_HASH_MAP_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_MAP_REVERSE_FOREACH_KEY_PTR(ht, _h, _key, _ptr) \
	ZEND_HASH_MAP_REVERSE_FOREACH(ht, 0); \
	_h = _p->h; \
	_key = _p->key; \
	_ptr = Z_PTR_P(_z);

/* Packed array iterators */
#define ZEND_HASH_PACKED_FOREACH_FROM(_ht, _from) do { \
		const HashTable *__ht = (_ht); \
		zend_ulong _idx = (_from); \
		zval *_z = __ht->arPacked + (_from); \
		zval *_end = __ht->arPacked + __ht->nNumUsed; \
		ZEND_ASSERT(HT_IS_PACKED(__ht)); \
		for (;_z != _end; _z++, _idx++) { \
			(void) _idx; \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_PACKED_FOREACH(_ht) ZEND_HASH_PACKED_FOREACH_FROM(_ht, 0)

#define ZEND_HASH_PACKED_REVERSE_FOREACH(_ht) do { \
		const HashTable *__ht = (_ht); \
		zend_ulong _idx = __ht->nNumUsed; \
		zval *_z = __ht->arPacked + _idx; \
		ZEND_ASSERT(HT_IS_PACKED(__ht)); \
		while (_idx > 0) { \
			_z--; \
			_idx--; \
			(void) _idx; \
			if (UNEXPECTED(Z_TYPE_P(_z) == IS_UNDEF)) continue;

#define ZEND_HASH_PACKED_FOREACH_VAL(ht, _val) \
	ZEND_HASH_PACKED_FOREACH(ht); \
	_val = _z;

#define ZEND_HASH_PACKED_REVERSE_FOREACH_VAL(ht, _val) \
	ZEND_HASH_PACKED_REVERSE_FOREACH(ht); \
	_val = _z;

#define ZEND_HASH_PACKED_FOREACH_PTR(ht, _ptr) \
	ZEND_HASH_PACKED_FOREACH(ht); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_PACKED_REVERSE_FOREACH_PTR(ht, _ptr) \
	ZEND_HASH_PACKED_REVERSE_FOREACH(ht); \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_PACKED_FOREACH_KEY(ht, _h) \
	ZEND_HASH_PACKED_FOREACH(ht); \
	_h = _idx;

#define ZEND_HASH_PACKED_REVERSE_FOREACH_KEY(ht, _h) \
	ZEND_HASH_PACKED_REVERSE_FOREACH(ht); \
	_h = _idx;

#define ZEND_HASH_PACKED_FOREACH_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_PACKED_FOREACH(ht); \
	_h = _idx; \
	_val = _z;

#define ZEND_HASH_PACKED_REVERSE_FOREACH_KEY_VAL(ht, _h, _val) \
	ZEND_HASH_PACKED_REVERSE_FOREACH(ht); \
	_h = _idx; \
	_val = _z;

#define ZEND_HASH_PACKED_FOREACH_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_PACKED_FOREACH(ht); \
	_h = _idx; \
	_ptr = Z_PTR_P(_z);

#define ZEND_HASH_PACKED_REVERSE_FOREACH_KEY_PTR(ht, _h, _ptr) \
	ZEND_HASH_PACKED_REVERSE_FOREACH(ht); \
	_h = _idx; \
	_ptr = Z_PTR_P(_z);

/* The following macros are useful to insert a sequence of new elements
 * of packed array. They may be used instead of series of
 * zend_hash_next_index_insert_new()
 * (HashTable must have enough free buckets).
 */
#define ZEND_HASH_FILL_PACKED(ht) do { \
		HashTable *__fill_ht = (ht); \
		zval *__fill_val = __fill_ht->arPacked + __fill_ht->nNumUsed; \
		uint32_t __fill_idx = __fill_ht->nNumUsed; \
		ZEND_ASSERT(HT_IS_PACKED(__fill_ht));

#define ZEND_HASH_FILL_GROW() do { \
		if (UNEXPECTED(__fill_idx >= __fill_ht->nTableSize)) { \
			__fill_ht->nNumOfElements += __fill_idx - __fill_ht->nNumUsed; \
			__fill_ht->nNumUsed = __fill_idx; \
			__fill_ht->nNextFreeElement = __fill_idx; \
			zend_hash_packed_grow(__fill_ht); \
			__fill_val = __fill_ht->arPacked + __fill_idx; \
		} \
	} while (0);

#define ZEND_HASH_FILL_SET(_val) \
		ZVAL_COPY_VALUE(__fill_val, _val)

#define ZEND_HASH_FILL_SET_NULL() \
		ZVAL_NULL(__fill_val)

#define ZEND_HASH_FILL_SET_LONG(_val) \
		ZVAL_LONG(__fill_val, _val)

#define ZEND_HASH_FILL_SET_DOUBLE(_val) \
		ZVAL_DOUBLE(__fill_val, _val)

#define ZEND_HASH_FILL_SET_STR(_val) \
		ZVAL_STR(__fill_val, _val)

#define ZEND_HASH_FILL_SET_STR_COPY(_val) \
		ZVAL_STR_COPY(__fill_val, _val)

#define ZEND_HASH_FILL_SET_INTERNED_STR(_val) \
		ZVAL_INTERNED_STR(__fill_val, _val)

#define ZEND_HASH_FILL_NEXT() do {\
		__fill_val++; \
		__fill_idx++; \
	} while (0)

#define ZEND_HASH_FILL_ADD(_val) do { \
		ZEND_HASH_FILL_SET(_val); \
		ZEND_HASH_FILL_NEXT(); \
	} while (0)

#define ZEND_HASH_FILL_FINISH() do { \
		__fill_ht->nNumOfElements += __fill_idx - __fill_ht->nNumUsed; \
		__fill_ht->nNumUsed = __fill_idx; \
		__fill_ht->nNextFreeElement = __fill_idx; \
		__fill_ht->nInternalPointer = 0; \
	} while (0)

#define ZEND_HASH_FILL_END() \
		ZEND_HASH_FILL_FINISH(); \
	} while (0)

/* Check if an array is a list */
static zend_always_inline bool zend_array_is_list(zend_array *array)
{
	zend_ulong expected_idx = 0;
	zend_ulong num_idx;
	zend_string* str_idx;
	/* Empty arrays are lists */
	if (zend_hash_num_elements(array) == 0) {
		return 1;
	}

	/* Packed arrays are lists */
	if (HT_IS_PACKED(array)) {
		if (HT_IS_WITHOUT_HOLES(array)) {
			return 1;
		}
		/* Check if the list could theoretically be repacked */
		ZEND_HASH_PACKED_FOREACH_KEY(array, num_idx) {
			if (num_idx != expected_idx++) {
				return 0;
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		/* Check if the list could theoretically be repacked */
		ZEND_HASH_MAP_FOREACH_KEY(array, num_idx, str_idx) {
			if (str_idx != NULL || num_idx != expected_idx++) {
				return 0;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return 1;
}


static zend_always_inline zval *_zend_hash_append_ex(HashTable *ht, zend_string *key, zval *zv, bool interned)
{
	uint32_t idx = ht->nNumUsed++;
	uint32_t nIndex;
	Bucket *p = ht->arData + idx;

	ZVAL_COPY_VALUE(&p->val, zv);
	if (!interned && !ZSTR_IS_INTERNED(key)) {
		HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
		zend_string_addref(key);
		zend_string_hash_val(key);
	}
	p->key = key;
	p->h = ZSTR_H(key);
	nIndex = (uint32_t)p->h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	ht->nNumOfElements++;
	return &p->val;
}

static zend_always_inline zval *_zend_hash_append(HashTable *ht, zend_string *key, zval *zv)
{
	return _zend_hash_append_ex(ht, key, zv, 0);
}

static zend_always_inline zval *_zend_hash_append_ptr_ex(HashTable *ht, zend_string *key, void *ptr, bool interned)
{
	uint32_t idx = ht->nNumUsed++;
	uint32_t nIndex;
	Bucket *p = ht->arData + idx;

	ZVAL_PTR(&p->val, ptr);
	if (!interned && !ZSTR_IS_INTERNED(key)) {
		HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
		zend_string_addref(key);
		zend_string_hash_val(key);
	}
	p->key = key;
	p->h = ZSTR_H(key);
	nIndex = (uint32_t)p->h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	ht->nNumOfElements++;
	return &p->val;
}

static zend_always_inline zval *_zend_hash_append_ptr(HashTable *ht, zend_string *key, void *ptr)
{
	return _zend_hash_append_ptr_ex(ht, key, ptr, 0);
}

static zend_always_inline void _zend_hash_append_ind(HashTable *ht, zend_string *key, zval *ptr)
{
	uint32_t idx = ht->nNumUsed++;
	uint32_t nIndex;
	Bucket *p = ht->arData + idx;

	ZVAL_INDIRECT(&p->val, ptr);
	if (!ZSTR_IS_INTERNED(key)) {
		HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
		zend_string_addref(key);
		zend_string_hash_val(key);
	}
	p->key = key;
	p->h = ZSTR_H(key);
	nIndex = (uint32_t)p->h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	ht->nNumOfElements++;
}

#endif							/* ZEND_HASH_H */
