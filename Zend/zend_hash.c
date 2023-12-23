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

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"

#if defined(__aarch64__) || defined(_M_ARM64)
# include <arm_neon.h>
#endif

/* Prefer to use AVX2 instructions for better latency and throughput */
#if defined(__AVX2__)
# include <immintrin.h>
#elif defined( __SSE2__)
# include <mmintrin.h>
# include <emmintrin.h>
#endif

#if ZEND_DEBUG
# define HT_ASSERT(ht, expr) \
	ZEND_ASSERT((expr) || (HT_FLAGS(ht) & HASH_FLAG_ALLOW_COW_VIOLATION))
#else
# define HT_ASSERT(ht, expr)
#endif

#define HT_ASSERT_RC1(ht) HT_ASSERT(ht, GC_REFCOUNT(ht) == 1)

#define HT_POISONED_PTR ((HashTable *) (intptr_t) -1)

#if ZEND_DEBUG

#define HT_OK					0x00
#define HT_IS_DESTROYING		0x01
#define HT_DESTROYED			0x02
#define HT_CLEANING				0x03

static void _zend_is_inconsistent(const HashTable *ht, const char *file, int line)
{
	if ((HT_FLAGS(ht) & HASH_FLAG_CONSISTENCY) == HT_OK) {
		return;
	}
	switch (HT_FLAGS(ht) & HASH_FLAG_CONSISTENCY) {
		case HT_IS_DESTROYING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being destroyed", file, line, ht);
			break;
		case HT_DESTROYED:
			zend_output_debug_string(1, "%s(%d) : ht=%p is already destroyed", file, line, ht);
			break;
		case HT_CLEANING:
			zend_output_debug_string(1, "%s(%d) : ht=%p is being cleaned", file, line, ht);
			break;
		default:
			zend_output_debug_string(1, "%s(%d) : ht=%p is inconsistent", file, line, ht);
			break;
	}
	ZEND_UNREACHABLE();
}
#define IS_CONSISTENT(a) _zend_is_inconsistent(a, __FILE__, __LINE__);
#define SET_INCONSISTENT(n) do { \
		HT_FLAGS(ht) = (HT_FLAGS(ht) & ~HASH_FLAG_CONSISTENCY) | (n); \
	} while (0)
#else
#define IS_CONSISTENT(a)
#define SET_INCONSISTENT(n)
#endif

#define ZEND_HASH_IF_FULL_DO_RESIZE(ht)				\
	if ((ht)->nNumUsed >= (ht)->nTableSize) {		\
		zend_hash_do_resize(ht);					\
	}

ZEND_API void *zend_hash_str_find_ptr_lc(const HashTable *ht, const char *str, size_t len) {
	void *result;
	char *lc_str;

	/* Stack allocate small strings to improve performance */
	ALLOCA_FLAG(use_heap)

	lc_str = zend_str_tolower_copy(do_alloca(len + 1, use_heap), str, len);
	result = zend_hash_str_find_ptr(ht, lc_str, len);
	free_alloca(lc_str, use_heap);

	return result;
}

ZEND_API void *zend_hash_find_ptr_lc(const HashTable *ht, zend_string *key) {
	void *result;
	zend_string *lc_key = zend_string_tolower(key);
	result = zend_hash_find_ptr(ht, lc_key);
	zend_string_release(lc_key);
	return result;
}

static void ZEND_FASTCALL zend_hash_do_resize(HashTable *ht);

static zend_always_inline uint32_t zend_hash_check_size(uint32_t nSize)
{
#ifdef ZEND_WIN32
	unsigned long index;
#endif

	/* Use big enough power of 2 */
	/* size should be between HT_MIN_SIZE and HT_MAX_SIZE */
	if (nSize <= HT_MIN_SIZE) {
		return HT_MIN_SIZE;
	} else if (UNEXPECTED(nSize > HT_MAX_SIZE)) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%u * %zu + %zu)", nSize, sizeof(Bucket), sizeof(Bucket));
	}

#ifdef ZEND_WIN32
	if (BitScanReverse(&index, nSize - 1)) {
		return 0x2u << ((31 - index) ^ 0x1f);
	} else {
		/* nSize is ensured to be in the valid range, fall back to it
		   rather than using an undefined bis scan result. */
		return nSize;
	}
#elif (defined(__GNUC__) || __has_builtin(__builtin_clz))  && defined(PHP_HAVE_BUILTIN_CLZ)
	return 0x2u << (__builtin_clz(nSize - 1) ^ 0x1f);
#else
	nSize -= 1;
	nSize |= (nSize >> 1);
	nSize |= (nSize >> 2);
	nSize |= (nSize >> 4);
	nSize |= (nSize >> 8);
	nSize |= (nSize >> 16);
	return nSize + 1;
#endif
}

static zend_always_inline void zend_hash_real_init_packed_ex(HashTable *ht)
{
	void *data;
	uint8_t prev_flags = HT_FLAGS(ht) & HASH_FLAG_ALLOW_COW_VIOLATION;

	if (UNEXPECTED(GC_FLAGS(ht) & IS_ARRAY_PERSISTENT)) {
		data = pemalloc(HT_PACKED_SIZE_EX(ht->nTableSize, HT_MIN_MASK), 1);
	} else if (EXPECTED(ht->nTableSize == HT_MIN_SIZE)) {
		/* Use specialized API with constant allocation amount for a particularly common case. */
		data = emalloc(HT_PACKED_SIZE_EX(HT_MIN_SIZE, HT_MIN_MASK));
	} else {
		data = emalloc(HT_PACKED_SIZE_EX(ht->nTableSize, HT_MIN_MASK));
	}
	HT_SET_DATA_ADDR(ht, data);
	/* Don't overwrite iterator count. */
	ht->u.v.flags = prev_flags | HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
	HT_HASH_RESET_PACKED(ht);
}

static zend_always_inline void zend_hash_real_init_mixed_ex(HashTable *ht)
{
	void *data;
	uint32_t nSize = ht->nTableSize;
	uint8_t prev_flags = HT_FLAGS(ht) & HASH_FLAG_ALLOW_COW_VIOLATION;

	ZEND_ASSERT(HT_SIZE_TO_MASK(nSize));

	if (UNEXPECTED(GC_FLAGS(ht) & IS_ARRAY_PERSISTENT)) {
		data = pemalloc(HT_SIZE_EX(nSize, HT_SIZE_TO_MASK(nSize)), 1);
	} else if (EXPECTED(nSize == HT_MIN_SIZE)) {
		data = emalloc(HT_SIZE_EX(HT_MIN_SIZE, HT_SIZE_TO_MASK(HT_MIN_SIZE)));
		ht->nTableMask = HT_SIZE_TO_MASK(HT_MIN_SIZE);
		HT_SET_DATA_ADDR(ht, data);
		/* Don't overwrite iterator count. */
		ht->u.v.flags = prev_flags | HASH_FLAG_STATIC_KEYS;
#if defined(__AVX2__)
		do {
			__m256i ymm0 = _mm256_setzero_si256();
			ymm0 = _mm256_cmpeq_epi64(ymm0, ymm0);
			_mm256_storeu_si256((__m256i*)&HT_HASH_EX(data,  0), ymm0);
			_mm256_storeu_si256((__m256i*)&HT_HASH_EX(data,  8), ymm0);
		} while(0);
#elif defined (__SSE2__)
		do {
			__m128i xmm0 = _mm_setzero_si128();
			xmm0 = _mm_cmpeq_epi8(xmm0, xmm0);
			_mm_storeu_si128((__m128i*)&HT_HASH_EX(data,  0), xmm0);
			_mm_storeu_si128((__m128i*)&HT_HASH_EX(data,  4), xmm0);
			_mm_storeu_si128((__m128i*)&HT_HASH_EX(data,  8), xmm0);
			_mm_storeu_si128((__m128i*)&HT_HASH_EX(data, 12), xmm0);
		} while (0);
#elif defined(__aarch64__) || defined(_M_ARM64)
		do {
			int32x4_t t = vdupq_n_s32(-1);
			vst1q_s32((int32_t*)&HT_HASH_EX(data,  0), t);
			vst1q_s32((int32_t*)&HT_HASH_EX(data,  4), t);
			vst1q_s32((int32_t*)&HT_HASH_EX(data,  8), t);
			vst1q_s32((int32_t*)&HT_HASH_EX(data, 12), t);
		} while (0);
#else
		HT_HASH_EX(data,  0) = -1;
		HT_HASH_EX(data,  1) = -1;
		HT_HASH_EX(data,  2) = -1;
		HT_HASH_EX(data,  3) = -1;
		HT_HASH_EX(data,  4) = -1;
		HT_HASH_EX(data,  5) = -1;
		HT_HASH_EX(data,  6) = -1;
		HT_HASH_EX(data,  7) = -1;
		HT_HASH_EX(data,  8) = -1;
		HT_HASH_EX(data,  9) = -1;
		HT_HASH_EX(data, 10) = -1;
		HT_HASH_EX(data, 11) = -1;
		HT_HASH_EX(data, 12) = -1;
		HT_HASH_EX(data, 13) = -1;
		HT_HASH_EX(data, 14) = -1;
		HT_HASH_EX(data, 15) = -1;
#endif
		return;
	} else {
		data = emalloc(HT_SIZE_EX(nSize, HT_SIZE_TO_MASK(nSize)));
	}
	ht->nTableMask = HT_SIZE_TO_MASK(nSize);
	HT_SET_DATA_ADDR(ht, data);
	HT_FLAGS(ht) = prev_flags | HASH_FLAG_STATIC_KEYS;
	HT_HASH_RESET(ht);
}

static zend_always_inline void zend_hash_real_init_ex(HashTable *ht, bool packed)
{
	HT_ASSERT_RC1(ht);
	ZEND_ASSERT(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED);
	if (packed) {
		zend_hash_real_init_packed_ex(ht);
	} else {
		zend_hash_real_init_mixed_ex(ht);
	}
}

static const uint32_t uninitialized_bucket[-HT_MIN_MASK] =
	{HT_INVALID_IDX, HT_INVALID_IDX};

ZEND_API const HashTable zend_empty_array = {
	.gc.refcount = 2,
	.gc.u.type_info = IS_ARRAY | (GC_IMMUTABLE << GC_FLAGS_SHIFT),
	.u.flags = HASH_FLAG_UNINITIALIZED,
	.nTableMask = HT_MIN_MASK,
	{.arData = (Bucket*)&uninitialized_bucket[2]},
	.nNumUsed = 0,
	.nNumOfElements = 0,
	.nTableSize = HT_MIN_SIZE,
	.nInternalPointer = 0,
	.nNextFreeElement = ZEND_LONG_MIN,
	.pDestructor = ZVAL_PTR_DTOR
};

static zend_always_inline void _zend_hash_init_int(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, bool persistent)
{
	GC_SET_REFCOUNT(ht, 1);
	GC_TYPE_INFO(ht) = GC_ARRAY | (persistent ? ((GC_PERSISTENT|GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT) : 0);
	HT_FLAGS(ht) = HASH_FLAG_UNINITIALIZED;
	ht->nTableMask = HT_MIN_MASK;
	HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nInternalPointer = 0;
	ht->nNextFreeElement = ZEND_LONG_MIN;
	ht->pDestructor = pDestructor;
	ht->nTableSize = zend_hash_check_size(nSize);
}

ZEND_API void ZEND_FASTCALL _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, bool persistent)
{
	_zend_hash_init_int(ht, nSize, pDestructor, persistent);
}

ZEND_API HashTable* ZEND_FASTCALL _zend_new_array_0(void)
{
	HashTable *ht = emalloc(sizeof(HashTable));
	_zend_hash_init_int(ht, HT_MIN_SIZE, ZVAL_PTR_DTOR, 0);
	return ht;
}

ZEND_API HashTable* ZEND_FASTCALL _zend_new_array(uint32_t nSize)
{
	HashTable *ht = emalloc(sizeof(HashTable));
	_zend_hash_init_int(ht, nSize, ZVAL_PTR_DTOR, 0);
	return ht;
}

ZEND_API HashTable* ZEND_FASTCALL zend_new_pair(zval *val1, zval *val2)
{
	zval *zv;
	HashTable *ht = emalloc(sizeof(HashTable));
	_zend_hash_init_int(ht, HT_MIN_SIZE, ZVAL_PTR_DTOR, 0);
	ht->nNumUsed = ht->nNumOfElements = ht->nNextFreeElement = 2;
	zend_hash_real_init_packed_ex(ht);

	zv = ht->arPacked;
	ZVAL_COPY_VALUE(zv, val1);
	zv++;
	ZVAL_COPY_VALUE(zv, val2);
	return ht;
}

ZEND_API void ZEND_FASTCALL zend_hash_packed_grow(HashTable *ht)
{
	HT_ASSERT_RC1(ht);
	if (ht->nTableSize >= HT_MAX_SIZE) {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%u * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket), sizeof(Bucket));
	}
	uint32_t newTableSize = ht->nTableSize * 2;
	HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_PACKED_SIZE_EX(newTableSize, HT_MIN_MASK), HT_PACKED_USED_SIZE(ht), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT));
	ht->nTableSize = newTableSize;
}

ZEND_API void ZEND_FASTCALL zend_hash_real_init(HashTable *ht, bool packed)
{
	IS_CONSISTENT(ht);

	HT_ASSERT_RC1(ht);
	zend_hash_real_init_ex(ht, packed);
}

ZEND_API void ZEND_FASTCALL zend_hash_real_init_packed(HashTable *ht)
{
	IS_CONSISTENT(ht);

	HT_ASSERT_RC1(ht);
	zend_hash_real_init_packed_ex(ht);
}

ZEND_API void ZEND_FASTCALL zend_hash_real_init_mixed(HashTable *ht)
{
	IS_CONSISTENT(ht);

	HT_ASSERT_RC1(ht);
	zend_hash_real_init_mixed_ex(ht);
}

ZEND_API void ZEND_FASTCALL zend_hash_packed_to_hash(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	zval *src = ht->arPacked;
	Bucket *dst;
	uint32_t i;
	uint32_t nSize = ht->nTableSize;

	ZEND_ASSERT(HT_SIZE_TO_MASK(nSize));

	HT_ASSERT_RC1(ht);
	// Alloc before assign to avoid inconsistencies on OOM
	new_data = pemalloc(HT_SIZE_EX(nSize, HT_SIZE_TO_MASK(nSize)), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	HT_FLAGS(ht) &= ~HASH_FLAG_PACKED;
	ht->nTableMask = HT_SIZE_TO_MASK(ht->nTableSize);
	HT_SET_DATA_ADDR(ht, new_data);
	dst = ht->arData;
	for (i = 0; i < ht->nNumUsed; i++) {
		ZVAL_COPY_VALUE(&dst->val, src);
		dst->h = i;
		dst->key = NULL;
		dst++;
		src++;
	}
	pefree(old_data, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	zend_hash_rehash(ht);
}

ZEND_API void ZEND_FASTCALL zend_hash_to_packed(HashTable *ht)
{
	void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
	Bucket *src = ht->arData;
	zval *dst;
	uint32_t i;

	HT_ASSERT_RC1(ht);
	new_data = pemalloc(HT_PACKED_SIZE_EX(ht->nTableSize, HT_MIN_MASK), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	HT_FLAGS(ht) |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
	ht->nTableMask = HT_MIN_MASK;
	HT_SET_DATA_ADDR(ht, new_data);
	HT_HASH_RESET_PACKED(ht);
	dst = ht->arPacked;
	for (i = 0; i < ht->nNumUsed; i++) {
		ZVAL_COPY_VALUE(dst, &src->val);
		dst++;
		src++;
	}
	pefree(old_data, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
}

ZEND_API void ZEND_FASTCALL zend_hash_extend(HashTable *ht, uint32_t nSize, bool packed)
{
	HT_ASSERT_RC1(ht);

	if (nSize == 0) return;

	ZEND_ASSERT(HT_SIZE_TO_MASK(nSize));

	if (UNEXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
		if (nSize > ht->nTableSize) {
			ht->nTableSize = zend_hash_check_size(nSize);
		}
		zend_hash_real_init(ht, packed);
	} else {
		if (packed) {
			ZEND_ASSERT(HT_IS_PACKED(ht));
			if (nSize > ht->nTableSize) {
				uint32_t newTableSize = zend_hash_check_size(nSize);
				HT_SET_DATA_ADDR(ht, perealloc2(HT_GET_DATA_ADDR(ht), HT_PACKED_SIZE_EX(newTableSize, HT_MIN_MASK), HT_PACKED_USED_SIZE(ht), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT));
				ht->nTableSize = newTableSize;
			}
		} else {
			ZEND_ASSERT(!HT_IS_PACKED(ht));
			if (nSize > ht->nTableSize) {
				void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
				Bucket *old_buckets = ht->arData;
				nSize = zend_hash_check_size(nSize);
				new_data = pemalloc(HT_SIZE_EX(nSize, HT_SIZE_TO_MASK(nSize)), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
				ht->nTableSize = nSize;
				ht->nTableMask = HT_SIZE_TO_MASK(ht->nTableSize);
				HT_SET_DATA_ADDR(ht, new_data);
				memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
				pefree(old_data, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
				zend_hash_rehash(ht);
			}
		}
	}
}

ZEND_API void ZEND_FASTCALL zend_hash_discard(HashTable *ht, uint32_t nNumUsed)
{
	Bucket *p, *end, *arData;
	uint32_t nIndex;

	ZEND_ASSERT(!HT_IS_PACKED(ht));
	arData = ht->arData;
	p = arData + ht->nNumUsed;
	end = arData + nNumUsed;
	ht->nNumUsed = nNumUsed;
	while (p != end) {
		p--;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		ht->nNumOfElements--;
		/* Collision pointers always directed from higher to lower buckets */
#if 0
		if (!(Z_NEXT(p->val) == HT_INVALID_IDX || HT_HASH_TO_BUCKET_EX(arData, Z_NEXT(p->val)) < p)) {
			abort();
		}
#endif
		nIndex = p->h | ht->nTableMask;
		HT_HASH_EX(arData, nIndex) = Z_NEXT(p->val);
	}
}

static uint32_t zend_array_recalc_elements(HashTable *ht)
{
	zval *val;
	uint32_t num = ht->nNumOfElements;

	ZEND_HASH_MAP_FOREACH_VAL(ht, val) {
		if (Z_TYPE_P(val) == IS_INDIRECT) {
			if (UNEXPECTED(Z_TYPE_P(Z_INDIRECT_P(val)) == IS_UNDEF)) {
				num--;
			}
		}
	} ZEND_HASH_FOREACH_END();
	return num;
}
/* }}} */

ZEND_API uint32_t zend_array_count(HashTable *ht)
{
	uint32_t num;
	if (UNEXPECTED(HT_FLAGS(ht) & HASH_FLAG_HAS_EMPTY_IND)) {
		num = zend_array_recalc_elements(ht);
		if (UNEXPECTED(ht->nNumOfElements == num)) {
			HT_FLAGS(ht) &= ~HASH_FLAG_HAS_EMPTY_IND;
		}
	} else if (UNEXPECTED(ht == &EG(symbol_table))) {
		num = zend_array_recalc_elements(ht);
	} else {
		num = zend_hash_num_elements(ht);
	}
	return num;
}
/* }}} */

static zend_always_inline HashPosition _zend_hash_get_valid_pos(const HashTable *ht, HashPosition pos)
{
	if (HT_IS_PACKED(ht)) {
		while (pos < ht->nNumUsed && Z_ISUNDEF(ht->arPacked[pos])) {
			pos++;
		}
	} else {
		while (pos < ht->nNumUsed && Z_ISUNDEF(ht->arData[pos].val)) {
			pos++;
		}
	}
	return pos;
}

static zend_always_inline HashPosition _zend_hash_get_current_pos(const HashTable *ht)
{
	return _zend_hash_get_valid_pos(ht, ht->nInternalPointer);
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_get_current_pos(const HashTable *ht)
{
	return _zend_hash_get_current_pos(ht);
}

static void zend_hash_remove_iterator_copies(uint32_t idx) {
	HashTableIterator *iterators = EG(ht_iterators);

	HashTableIterator *iter = iterators + idx;
	uint32_t next_idx = iter->next_copy;
	while (next_idx != idx) {
		uint32_t cur_idx = next_idx;
		HashTableIterator *cur_iter = iterators + cur_idx;
		next_idx = cur_iter->next_copy;
		cur_iter->next_copy = cur_idx; // avoid recursion in zend_hash_iterator_del
		zend_hash_iterator_del(cur_idx);
	}
	iter->next_copy = idx;
}

ZEND_API uint32_t ZEND_FASTCALL zend_hash_iterator_add(HashTable *ht, HashPosition pos)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_count);
	uint32_t idx;

	if (EXPECTED(!HT_ITERATORS_OVERFLOW(ht))) {
		HT_INC_ITERATORS_COUNT(ht);
	}
	while (iter != end) {
		if (iter->ht == NULL) {
			iter->ht = ht;
			iter->pos = pos;
			idx = iter - EG(ht_iterators);
			iter->next_copy = idx;
			if (idx + 1 > EG(ht_iterators_used)) {
				EG(ht_iterators_used) = idx + 1;
			}
			return idx;
		}
		iter++;
	}
	if (EG(ht_iterators) == EG(ht_iterators_slots)) {
		EG(ht_iterators) = emalloc(sizeof(HashTableIterator) * (EG(ht_iterators_count) + 8));
		memcpy(EG(ht_iterators), EG(ht_iterators_slots), sizeof(HashTableIterator) * EG(ht_iterators_count));
	} else {
		EG(ht_iterators) = erealloc(EG(ht_iterators), sizeof(HashTableIterator) * (EG(ht_iterators_count) + 8));
	}
	iter = EG(ht_iterators) + EG(ht_iterators_count);
	EG(ht_iterators_count) += 8;
	iter->ht = ht;
	iter->pos = pos;
	memset(iter + 1, 0, sizeof(HashTableIterator) * 7);
	idx = iter - EG(ht_iterators);
	iter->next_copy = idx;
	EG(ht_iterators_used) = idx + 1;
	return idx;
}

// To avoid losing track of the HashTable when separating arrays, we track all copies at once.
static zend_always_inline bool zend_hash_iterator_find_copy_pos(uint32_t idx, HashTable *ht) {
	HashTableIterator *iter = EG(ht_iterators) + idx;

	uint32_t next_idx = iter->next_copy;
	if (EXPECTED(next_idx != idx)) {
		HashTableIterator *copy_iter;
		while (next_idx != idx) {
			copy_iter = EG(ht_iterators) + next_idx;
			if (copy_iter->ht == ht) {
				// We have found the hashtable we are actually iterating over
				// Now clean any intermittent copies and replace the original index by the found one
				if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
					&& EXPECTED(!HT_ITERATORS_OVERFLOW(iter->ht))) {
					HT_DEC_ITERATORS_COUNT(iter->ht);
				}
				if (EXPECTED(!HT_ITERATORS_OVERFLOW(ht))) {
					HT_INC_ITERATORS_COUNT(ht);
				}
				iter->ht = copy_iter->ht;
				iter->pos = copy_iter->pos;
				zend_hash_remove_iterator_copies(idx);
				return true;
			}
			next_idx = copy_iter->next_copy;
		}
		zend_hash_remove_iterator_copies(idx);
	}

	return false;
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos(uint32_t idx, HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);
	if (UNEXPECTED(iter->ht != ht) && !zend_hash_iterator_find_copy_pos(idx, ht)) {
		if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
				&& EXPECTED(!HT_ITERATORS_OVERFLOW(iter->ht))) {
			HT_DEC_ITERATORS_COUNT(iter->ht);
		}
		if (EXPECTED(!HT_ITERATORS_OVERFLOW(ht))) {
			HT_INC_ITERATORS_COUNT(ht);
		}
		iter->ht = ht;
		iter->pos = _zend_hash_get_current_pos(ht);
	}
	return iter->pos;
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterator_pos_ex(uint32_t idx, zval *array)
{
	HashTable *ht = Z_ARRVAL_P(array);
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);
	if (UNEXPECTED(iter->ht != ht) && !zend_hash_iterator_find_copy_pos(idx, ht)) {
		if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
				&& EXPECTED(!HT_ITERATORS_OVERFLOW(ht))) {
			HT_DEC_ITERATORS_COUNT(iter->ht);
		}
		SEPARATE_ARRAY(array);
		ht = Z_ARRVAL_P(array);
		if (EXPECTED(!HT_ITERATORS_OVERFLOW(ht))) {
			HT_INC_ITERATORS_COUNT(ht);
		}
		iter->ht = ht;
		iter->pos = _zend_hash_get_current_pos(ht);
	}
	return iter->pos;
}

ZEND_API void ZEND_FASTCALL zend_hash_iterator_del(uint32_t idx)
{
	HashTableIterator *iter = EG(ht_iterators) + idx;

	ZEND_ASSERT(idx != (uint32_t)-1);

	if (EXPECTED(iter->ht) && EXPECTED(iter->ht != HT_POISONED_PTR)
			&& EXPECTED(!HT_ITERATORS_OVERFLOW(iter->ht))) {
		ZEND_ASSERT(HT_ITERATORS_COUNT(iter->ht) != 0);
		HT_DEC_ITERATORS_COUNT(iter->ht);
	}
	iter->ht = NULL;

	if (UNEXPECTED(iter->next_copy != idx)) {
		zend_hash_remove_iterator_copies(idx);
	}

	if (idx == EG(ht_iterators_used) - 1) {
		while (idx > 0 && EG(ht_iterators)[idx - 1].ht == NULL) {
			idx--;
		}
		EG(ht_iterators_used) = idx;
	}
}

static zend_never_inline void ZEND_FASTCALL _zend_hash_iterators_remove(HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht) {
			iter->ht = HT_POISONED_PTR;
		}
		iter++;
	}
}

static zend_always_inline void zend_hash_iterators_remove(HashTable *ht)
{
	if (UNEXPECTED(HT_HAS_ITERATORS(ht))) {
		_zend_hash_iterators_remove(ht);
	}
}

ZEND_API HashPosition ZEND_FASTCALL zend_hash_iterators_lower_pos(HashTable *ht, HashPosition start)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);
	HashPosition res = ht->nNumUsed;

	while (iter != end) {
		if (iter->ht == ht) {
			if (iter->pos >= start && iter->pos < res) {
				res = iter->pos;
			}
		}
		iter++;
	}
	return res;
}

ZEND_API void ZEND_FASTCALL _zend_hash_iterators_update(HashTable *ht, HashPosition from, HashPosition to)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht && iter->pos == from) {
			iter->pos = to;
		}
		iter++;
	}
}

ZEND_API void ZEND_FASTCALL zend_hash_iterators_advance(HashTable *ht, HashPosition step)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht) {
			iter->pos += step;
		}
		iter++;
	}
}

/* Hash must be known and precomputed before */
static zend_always_inline Bucket *zend_hash_find_bucket(const HashTable *ht, const zend_string *key)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	ZEND_ASSERT(ZSTR_H(key) != 0 && "Hash must be known");

	arData = ht->arData;
	nIndex = ZSTR_H(key) | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);

	if (UNEXPECTED(idx == HT_INVALID_IDX)) {
		return NULL;
	}
	p = HT_HASH_TO_BUCKET_EX(arData, idx);
	if (EXPECTED(p->key == key)) { /* check for the same interned string */
		return p;
	}

	while (1) {
		if (p->h == ZSTR_H(key) &&
		    EXPECTED(p->key) &&
		    zend_string_equal_content(p->key, key)) {
			return p;
		}
		idx = Z_NEXT(p->val);
		if (idx == HT_INVALID_IDX) {
			return NULL;
		}
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if (p->key == key) { /* check for the same interned string */
			return p;
		}
	}
}

static zend_always_inline Bucket *zend_hash_str_find_bucket(const HashTable *ht, const char *str, size_t len, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	arData = ht->arData;
	nIndex = h | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);
	while (idx != HT_INVALID_IDX) {
		ZEND_ASSERT(idx < HT_IDX_TO_HASH(ht->nTableSize));
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if ((p->h == h)
			 && p->key
			 && zend_string_equals_cstr(p->key, str, len)) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline Bucket *zend_hash_index_find_bucket(const HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	arData = ht->arData;
	nIndex = h | ht->nTableMask;
	idx = HT_HASH_EX(arData, nIndex);
	while (idx != HT_INVALID_IDX) {
		ZEND_ASSERT(idx < HT_IDX_TO_HASH(ht->nTableSize));
		p = HT_HASH_TO_BUCKET_EX(arData, idx);
		if (p->h == h && !p->key) {
			return p;
		}
		idx = Z_NEXT(p->val);
	}
	return NULL;
}

static zend_always_inline zval *_zend_hash_add_or_update_i(HashTable *ht, zend_string *key, zval *pData, uint32_t flag)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p, *arData;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);
	zend_string_hash_val(key);

	if (UNEXPECTED(HT_FLAGS(ht) & (HASH_FLAG_UNINITIALIZED|HASH_FLAG_PACKED))) {
		if (EXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
			zend_hash_real_init_mixed(ht);
			goto add_to_hash;
		} else {
			zend_hash_packed_to_hash(ht);
		}
	} else if ((flag & HASH_ADD_NEW) == 0 || ZEND_DEBUG) {
		p = zend_hash_find_bucket(ht, key);

		if (p) {
			zval *data;

			ZEND_ASSERT((flag & HASH_ADD_NEW) == 0);
			if (flag & HASH_LOOKUP) {
				return &p->val;
			} else if (flag & HASH_ADD) {
				if (!(flag & HASH_UPDATE_INDIRECT)) {
					return NULL;
				}
				ZEND_ASSERT(&p->val != pData);
				data = &p->val;
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) != IS_UNDEF) {
						return NULL;
					}
				} else {
					return NULL;
				}
			} else {
				ZEND_ASSERT(&p->val != pData);
				data = &p->val;
				if ((flag & HASH_UPDATE_INDIRECT) && Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
				}
			}
			if (ht->pDestructor) {
				ht->pDestructor(data);
			}
			ZVAL_COPY_VALUE(data, pData);
			return data;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	if (!ZSTR_IS_INTERNED(key)) {
		zend_string_addref(key);
		HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
	}
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	arData = ht->arData;
	p = arData + idx;
	p->key = key;
	p->h = h = ZSTR_H(key);
	nIndex = h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH_EX(arData, nIndex);
	HT_HASH_EX(arData, nIndex) = HT_IDX_TO_HASH(idx);
	if (flag & HASH_LOOKUP) {
		ZVAL_NULL(&p->val);
	} else {
		ZVAL_COPY_VALUE(&p->val, pData);
	}

	return &p->val;
}

static zend_always_inline zval *_zend_hash_str_add_or_update_i(HashTable *ht, const char *str, size_t len, zend_ulong h, zval *pData, uint32_t flag)
{
	zend_string *key;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (UNEXPECTED(HT_FLAGS(ht) & (HASH_FLAG_UNINITIALIZED|HASH_FLAG_PACKED))) {
		if (EXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
			zend_hash_real_init_mixed(ht);
			goto add_to_hash;
		} else {
			zend_hash_packed_to_hash(ht);
		}
	} else if ((flag & HASH_ADD_NEW) == 0) {
		p = zend_hash_str_find_bucket(ht, str, len, h);

		if (p) {
			zval *data;

			if (flag & HASH_LOOKUP) {
				return &p->val;
			} else if (flag & HASH_ADD) {
				if (!(flag & HASH_UPDATE_INDIRECT)) {
					return NULL;
				}
				ZEND_ASSERT(&p->val != pData);
				data = &p->val;
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) != IS_UNDEF) {
						return NULL;
					}
				} else {
					return NULL;
				}
			} else {
				ZEND_ASSERT(&p->val != pData);
				data = &p->val;
				if ((flag & HASH_UPDATE_INDIRECT) && Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
				}
			}
			if (ht->pDestructor) {
				ht->pDestructor(data);
			}
			ZVAL_COPY_VALUE(data, pData);
			return data;
		}
	}

	ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */

add_to_hash:
	idx = ht->nNumUsed++;
	ht->nNumOfElements++;
	p = ht->arData + idx;
	p->key = key = zend_string_init(str, len, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
#if ZEND_RC_DEBUG
	if (GC_FLAGS(ht) & GC_PERSISTENT_LOCAL) {
		GC_MAKE_PERSISTENT_LOCAL(key);
	}
#endif
	p->h = ZSTR_H(key) = h;
	HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
	if (flag & HASH_LOOKUP) {
		ZVAL_NULL(&p->val);
	} else {
		ZVAL_COPY_VALUE(&p->val, pData);
	}
	nIndex = h | ht->nTableMask;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);

	return &p->val;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_add_or_update(HashTable *ht, zend_string *key, zval *pData, uint32_t flag)
{
	if (flag == HASH_ADD) {
		return zend_hash_add(ht, key, pData);
	} else if (flag == HASH_ADD_NEW) {
		return zend_hash_add_new(ht, key, pData);
	} else if (flag == HASH_UPDATE) {
		return zend_hash_update(ht, key, pData);
	} else {
		ZEND_ASSERT(flag == (HASH_UPDATE|HASH_UPDATE_INDIRECT));
		return zend_hash_update_ind(ht, key, pData);
	}
}

ZEND_API zval* ZEND_FASTCALL zend_hash_add(HashTable *ht, zend_string *key, zval *pData)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_update(HashTable *ht, zend_string *key, zval *pData)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_update_ind(HashTable *ht, zend_string *key, zval *pData)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_add_new(HashTable *ht, zend_string *key, zval *pData)
{
	return _zend_hash_add_or_update_i(ht, key, pData, HASH_ADD_NEW);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_lookup(HashTable *ht, zend_string *key)
{
	return _zend_hash_add_or_update_i(ht, key, NULL, HASH_LOOKUP);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_or_update(HashTable *ht, const char *str, size_t len, zval *pData, uint32_t flag)
{
	if (flag == HASH_ADD) {
		return zend_hash_str_add(ht, str, len, pData);
	} else if (flag == HASH_ADD_NEW) {
		return zend_hash_str_add_new(ht, str, len, pData);
	} else if (flag == HASH_UPDATE) {
		return zend_hash_str_update(ht, str, len, pData);
	} else {
		ZEND_ASSERT(flag == (HASH_UPDATE|HASH_UPDATE_INDIRECT));
		return zend_hash_str_update_ind(ht, str, len, pData);
	}
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_update(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong h = zend_hash_func(str, len);

	return _zend_hash_str_add_or_update_i(ht, str, len, h, pData, HASH_UPDATE);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_update_ind(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong h = zend_hash_func(str, len);

	return _zend_hash_str_add_or_update_i(ht, str, len, h, pData, HASH_UPDATE | HASH_UPDATE_INDIRECT);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong h = zend_hash_func(str, len);

	return _zend_hash_str_add_or_update_i(ht, str, len, h, pData, HASH_ADD);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_new(HashTable *ht, const char *str, size_t len, zval *pData)
{
	zend_ulong h = zend_hash_func(str, len);

	return _zend_hash_str_add_or_update_i(ht, str, len, h, pData, HASH_ADD_NEW);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_empty_element(HashTable *ht, zend_ulong h)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_index_add(ht, h, &dummy);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_add_empty_element(HashTable *ht, zend_string *key)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_add(ht, key, &dummy);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_add_empty_element(HashTable *ht, const char *str, size_t len)
{
	zval dummy;

	ZVAL_NULL(&dummy);
	return zend_hash_str_add(ht, str, len, &dummy);
}

static zend_always_inline zval *_zend_hash_index_add_or_update_i(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	zval *zv;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if ((flag & HASH_ADD_NEXT) && h == ZEND_LONG_MIN) {
		h = 0;
	}

	if (HT_IS_PACKED(ht)) {
		if ((flag & (HASH_ADD_NEW|HASH_ADD_NEXT)) != (HASH_ADD_NEW|HASH_ADD_NEXT)
		 && h < ht->nNumUsed) {
			zv = ht->arPacked + h;
			if (Z_TYPE_P(zv) != IS_UNDEF) {
				if (flag & HASH_LOOKUP) {
					return zv;
				}
replace:
				if (flag & HASH_ADD) {
					return NULL;
				}
				if (ht->pDestructor) {
					ht->pDestructor(zv);
				}
				ZVAL_COPY_VALUE(zv, pData);
				return zv;
			} else { /* we have to keep the order :( */
				goto convert_to_hash;
			}
		} else if (EXPECTED(h < ht->nTableSize)) {
add_to_packed:
			zv = ht->arPacked + h;
			/* incremental initialization of empty Buckets */
			if ((flag & (HASH_ADD_NEW|HASH_ADD_NEXT)) != (HASH_ADD_NEW|HASH_ADD_NEXT)) {
				if (h > ht->nNumUsed) {
					zval *q = ht->arPacked + ht->nNumUsed;
					while (q != zv) {
						ZVAL_UNDEF(q);
						q++;
					}
				}
			}
			ht->nNextFreeElement = ht->nNumUsed = h + 1;
			ht->nNumOfElements++;
			if (flag & HASH_LOOKUP) {
				ZVAL_NULL(zv);
			} else {
				ZVAL_COPY_VALUE(zv, pData);
			}

			return zv;
		} else if ((h >> 1) < ht->nTableSize &&
		           (ht->nTableSize >> 1) < ht->nNumOfElements) {
			zend_hash_packed_grow(ht);
			goto add_to_packed;
		} else {
			if (ht->nNumUsed >= ht->nTableSize) {
				ht->nTableSize += ht->nTableSize;
			}
convert_to_hash:
			zend_hash_packed_to_hash(ht);
		}
	} else if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		if (h < ht->nTableSize) {
			zend_hash_real_init_packed_ex(ht);
			goto add_to_packed;
		}
		zend_hash_real_init_mixed(ht);
	} else {
		if ((flag & HASH_ADD_NEW) == 0 || ZEND_DEBUG) {
			p = zend_hash_index_find_bucket(ht, h);
			if (p) {
				if (flag & HASH_LOOKUP) {
					return &p->val;
				}
				ZEND_ASSERT((flag & HASH_ADD_NEW) == 0);
				zv = &p->val;
				goto replace;
			}
		}
		ZEND_HASH_IF_FULL_DO_RESIZE(ht);		/* If the Hash table is full, resize it */
	}

	idx = ht->nNumUsed++;
	nIndex = h | ht->nTableMask;
	p = ht->arData + idx;
	Z_NEXT(p->val) = HT_HASH(ht, nIndex);
	HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
	if ((zend_long)h >= ht->nNextFreeElement) {
		ht->nNextFreeElement = (zend_long)h < ZEND_LONG_MAX ? h + 1 : ZEND_LONG_MAX;
	}
	ht->nNumOfElements++;
	p->h = h;
	p->key = NULL;
	if (flag & HASH_LOOKUP) {
		ZVAL_NULL(&p->val);
	} else {
		ZVAL_COPY_VALUE(&p->val, pData);
	}

	return &p->val;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_or_update(HashTable *ht, zend_ulong h, zval *pData, uint32_t flag)
{
	if (flag == HASH_ADD) {
		return zend_hash_index_add(ht, h, pData);
	} else if (flag == (HASH_ADD|HASH_ADD_NEW)) {
		return zend_hash_index_add_new(ht, h, pData);
	} else if (flag == (HASH_ADD|HASH_ADD_NEXT)) {
		ZEND_ASSERT(h == ht->nNextFreeElement);
		return zend_hash_next_index_insert(ht, pData);
	} else if (flag == (HASH_ADD|HASH_ADD_NEW|HASH_ADD_NEXT)) {
		ZEND_ASSERT(h == ht->nNextFreeElement);
		return zend_hash_next_index_insert_new(ht, pData);
	} else {
		ZEND_ASSERT(flag == HASH_UPDATE);
		return zend_hash_index_update(ht, h, pData);
	}
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add(HashTable *ht, zend_ulong h, zval *pData)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_add_new(HashTable *ht, zend_ulong h, zval *pData)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_ADD | HASH_ADD_NEW);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_update(HashTable *ht, zend_ulong h, zval *pData)
{
	return _zend_hash_index_add_or_update_i(ht, h, pData, HASH_UPDATE);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_next_index_insert(HashTable *ht, zval *pData)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEXT);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_next_index_insert_new(HashTable *ht, zval *pData)
{
	return _zend_hash_index_add_or_update_i(ht, ht->nNextFreeElement, pData, HASH_ADD | HASH_ADD_NEW | HASH_ADD_NEXT);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_lookup(HashTable *ht, zend_ulong h)
{
	return _zend_hash_index_add_or_update_i(ht, h, NULL, HASH_LOOKUP);
}

ZEND_API zval* ZEND_FASTCALL zend_hash_set_bucket_key(HashTable *ht, Bucket *b, zend_string *key)
{
	uint32_t nIndex;
	uint32_t idx, i;
	Bucket *p, *arData;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);
	ZEND_ASSERT(!HT_IS_PACKED(ht));

	(void)zend_string_hash_val(key);
	p = zend_hash_find_bucket(ht, key);
	if (UNEXPECTED(p)) {
		return (p == b) ? &p->val : NULL;
	}

	if (!ZSTR_IS_INTERNED(key)) {
		zend_string_addref(key);
		HT_FLAGS(ht) &= ~HASH_FLAG_STATIC_KEYS;
	}

	arData = ht->arData;

	/* del from hash */
	idx = HT_IDX_TO_HASH(b - arData);
	nIndex = b->h | ht->nTableMask;
	i = HT_HASH_EX(arData, nIndex);
	if (i == idx) {
		HT_HASH_EX(arData, nIndex) = Z_NEXT(b->val);
	} else {
		p = HT_HASH_TO_BUCKET_EX(arData, i);
		while (Z_NEXT(p->val) != idx) {
			i = Z_NEXT(p->val);
			p = HT_HASH_TO_BUCKET_EX(arData, i);
		}
		Z_NEXT(p->val) = Z_NEXT(b->val);
	}
	zend_string_release(b->key);

	/* add to hash */
	idx = b - arData;
	b->key = key;
	b->h = ZSTR_H(key);
	nIndex = b->h | ht->nTableMask;
	idx = HT_IDX_TO_HASH(idx);
	i = HT_HASH_EX(arData, nIndex);
	if (i == HT_INVALID_IDX || i < idx) {
		Z_NEXT(b->val) = i;
		HT_HASH_EX(arData, nIndex) = idx;
	} else {
		p = HT_HASH_TO_BUCKET_EX(arData, i);
		while (Z_NEXT(p->val) != HT_INVALID_IDX && Z_NEXT(p->val) > idx) {
			i = Z_NEXT(p->val);
			p = HT_HASH_TO_BUCKET_EX(arData, i);
		}
		Z_NEXT(b->val) = Z_NEXT(p->val);
		Z_NEXT(p->val) = idx;
	}
	return &b->val;
}

static void ZEND_FASTCALL zend_hash_do_resize(HashTable *ht)
{

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	ZEND_ASSERT(!HT_IS_PACKED(ht));
	if (ht->nNumUsed > ht->nNumOfElements + (ht->nNumOfElements >> 5)) { /* additional term is there to amortize the cost of compaction */
		zend_hash_rehash(ht);
	} else if (ht->nTableSize < HT_MAX_SIZE) {	/* Let's double the table size */
		void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
		uint32_t nSize = ht->nTableSize + ht->nTableSize;
		Bucket *old_buckets = ht->arData;

		ZEND_ASSERT(HT_SIZE_TO_MASK(nSize));

		new_data = pemalloc(HT_SIZE_EX(nSize, HT_SIZE_TO_MASK(nSize)), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		ht->nTableSize = nSize;
		ht->nTableMask = HT_SIZE_TO_MASK(ht->nTableSize);
		HT_SET_DATA_ADDR(ht, new_data);
		memcpy(ht->arData, old_buckets, sizeof(Bucket) * ht->nNumUsed);
		pefree(old_data, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
		zend_hash_rehash(ht);
	} else {
		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%u * %zu + %zu)", ht->nTableSize * 2, sizeof(Bucket) + sizeof(uint32_t), sizeof(Bucket));
	}
}

ZEND_API void ZEND_FASTCALL zend_hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint32_t nIndex, i;

	IS_CONSISTENT(ht);

	if (UNEXPECTED(ht->nNumOfElements == 0)) {
		if (!(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
			ht->nNumUsed = 0;
			HT_HASH_RESET(ht);
		}
		return;
	}

	HT_HASH_RESET(ht);
	i = 0;
	p = ht->arData;
	if (HT_IS_WITHOUT_HOLES(ht)) {
		do {
			nIndex = p->h | ht->nTableMask;
			Z_NEXT(p->val) = HT_HASH(ht, nIndex);
			HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(i);
			p++;
		} while (++i < ht->nNumUsed);
	} else {
		uint32_t old_num_used = ht->nNumUsed;
		do {
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) {
				uint32_t j = i;
				Bucket *q = p;

				if (EXPECTED(!HT_HAS_ITERATORS(ht))) {
					while (++i < ht->nNumUsed) {
						p++;
						if (EXPECTED(Z_TYPE_INFO(p->val) != IS_UNDEF)) {
							ZVAL_COPY_VALUE(&q->val, &p->val);
							q->h = p->h;
							nIndex = q->h | ht->nTableMask;
							q->key = p->key;
							Z_NEXT(q->val) = HT_HASH(ht, nIndex);
							HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(j);
							if (UNEXPECTED(ht->nInternalPointer == i)) {
								ht->nInternalPointer = j;
							}
							q++;
							j++;
						}
					}
				} else {
					uint32_t iter_pos = zend_hash_iterators_lower_pos(ht, i + 1);

					while (++i < ht->nNumUsed) {
						p++;
						if (EXPECTED(Z_TYPE_INFO(p->val) != IS_UNDEF)) {
							ZVAL_COPY_VALUE(&q->val, &p->val);
							q->h = p->h;
							nIndex = q->h | ht->nTableMask;
							q->key = p->key;
							Z_NEXT(q->val) = HT_HASH(ht, nIndex);
							HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(j);
							if (UNEXPECTED(ht->nInternalPointer == i)) {
								ht->nInternalPointer = j;
							}
							if (UNEXPECTED(i >= iter_pos)) {
								do {
									zend_hash_iterators_update(ht, iter_pos, j);
									iter_pos = zend_hash_iterators_lower_pos(ht, iter_pos + 1);
								} while (iter_pos < i);
							}
							q++;
							j++;
						}
					}
				}
				ht->nNumUsed = j;
				break;
			}
			nIndex = p->h | ht->nTableMask;
			Z_NEXT(p->val) = HT_HASH(ht, nIndex);
			HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(i);
			p++;
		} while (++i < ht->nNumUsed);

		/* Migrate pointer to one past the end of the array to the new one past the end, so that
		 * newly inserted elements are picked up correctly. */
		if (UNEXPECTED(HT_HAS_ITERATORS(ht))) {
			_zend_hash_iterators_update(ht, old_num_used, ht->nNumUsed);
		}
	}
}

static zend_always_inline void _zend_hash_packed_del_val(HashTable *ht, uint32_t idx, zval *zv)
{
	idx = HT_HASH_TO_IDX(idx);
	ht->nNumOfElements--;
	if (ht->nInternalPointer == idx || UNEXPECTED(HT_HAS_ITERATORS(ht))) {
		uint32_t new_idx;

		new_idx = idx;
		while (1) {
			new_idx++;
			if (new_idx >= ht->nNumUsed) {
				break;
			} else if (Z_TYPE(ht->arPacked[new_idx]) != IS_UNDEF) {
				break;
			}
		}
		if (ht->nInternalPointer == idx) {
			ht->nInternalPointer = new_idx;
		}
		zend_hash_iterators_update(ht, idx, new_idx);
	}
	if (ht->nNumUsed - 1 == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(ht->arPacked[ht->nNumUsed-1]) == IS_UNDEF)));
		ht->nInternalPointer = MIN(ht->nInternalPointer, ht->nNumUsed);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, zv);
		ZVAL_UNDEF(zv);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(zv);
	}
}

static zend_always_inline void _zend_hash_del_el_ex(HashTable *ht, uint32_t idx, Bucket *p, Bucket *prev)
{
	if (prev) {
		Z_NEXT(prev->val) = Z_NEXT(p->val);
	} else {
		HT_HASH(ht, p->h | ht->nTableMask) = Z_NEXT(p->val);
	}
	idx = HT_HASH_TO_IDX(idx);
	ht->nNumOfElements--;
	if (ht->nInternalPointer == idx || UNEXPECTED(HT_HAS_ITERATORS(ht))) {
		uint32_t new_idx;

		new_idx = idx;
		while (1) {
			new_idx++;
			if (new_idx >= ht->nNumUsed) {
				break;
			} else if (Z_TYPE(ht->arData[new_idx].val) != IS_UNDEF) {
				break;
			}
		}
		if (ht->nInternalPointer == idx) {
			ht->nInternalPointer = new_idx;
		}
		zend_hash_iterators_update(ht, idx, new_idx);
	}
	if (ht->nNumUsed - 1 == idx) {
		do {
			ht->nNumUsed--;
		} while (ht->nNumUsed > 0 && (UNEXPECTED(Z_TYPE(ht->arData[ht->nNumUsed-1].val) == IS_UNDEF)));
		ht->nInternalPointer = MIN(ht->nInternalPointer, ht->nNumUsed);
	}
	if (ht->pDestructor) {
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, &p->val);
		ZVAL_UNDEF(&p->val);
		ht->pDestructor(&tmp);
	} else {
		ZVAL_UNDEF(&p->val);
	}
}

static zend_always_inline void _zend_hash_del_el(HashTable *ht, uint32_t idx, Bucket *p)
{
	Bucket *prev = NULL;
	uint32_t nIndex;
	uint32_t i;

	nIndex = p->h | ht->nTableMask;
	i = HT_HASH(ht, nIndex);

	if (i != idx) {
		prev = HT_HASH_TO_BUCKET(ht, i);
		while (Z_NEXT(prev->val) != idx) {
			i = Z_NEXT(prev->val);
			prev = HT_HASH_TO_BUCKET(ht, i);
		}
	}

	if (p->key) {
		zend_string_release(p->key);
		p->key = NULL;
	}
	_zend_hash_del_el_ex(ht, idx, p, prev);
}

ZEND_API void ZEND_FASTCALL zend_hash_packed_del_val(HashTable *ht, zval *zv)
{
	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);
	ZEND_ASSERT(HT_IS_PACKED(ht));
	_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(zv - ht->arPacked), zv);
}


ZEND_API void ZEND_FASTCALL zend_hash_del_bucket(HashTable *ht, Bucket *p)
{
	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);
	ZEND_ASSERT(!HT_IS_PACKED(ht));
	_zend_hash_del_el(ht, HT_IDX_TO_HASH(p - ht->arData), p);
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_del(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	h = zend_string_hash_val(key);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     zend_string_equal_content(p->key, key))) {
			zend_string_release(p->key);
			p->key = NULL;
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_del_ind(HashTable *ht, zend_string *key)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	h = zend_string_hash_val(key);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->key == key) ||
			(p->h == h &&
		     p->key &&
		     zend_string_equal_content(p->key, key))) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						zval tmp;
						ZVAL_COPY_VALUE(&tmp, data);
						ZVAL_UNDEF(data);
						ht->pDestructor(&tmp);
					} else {
						ZVAL_UNDEF(data);
					}
					HT_FLAGS(ht) |= HASH_FLAG_HAS_EMPTY_IND;
				}
			} else {
				zend_string_release(p->key);
				p->key = NULL;
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_str_del_ind(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	h = zend_inline_hash_func(str, len);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h)
			 && p->key
			 && zend_string_equals_cstr(p->key, str, len)) {
			if (Z_TYPE(p->val) == IS_INDIRECT) {
				zval *data = Z_INDIRECT(p->val);

				if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
					return FAILURE;
				} else {
					if (ht->pDestructor) {
						ht->pDestructor(data);
					}
					ZVAL_UNDEF(data);
					HT_FLAGS(ht) |= HASH_FLAG_HAS_EMPTY_IND;
				}
			} else {
				zend_string_release(p->key);
				p->key = NULL;
				_zend_hash_del_el_ex(ht, idx, p, prev);
			}
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_str_del(HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	h = zend_inline_hash_func(str, len);
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h)
			 && p->key
			 && zend_string_equals_cstr(p->key, str, len)) {
			zend_string_release(p->key);
			p->key = NULL;
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_index_del(HashTable *ht, zend_ulong h)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;
	Bucket *prev = NULL;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			zval *zv = ht->arPacked + h;
			if (Z_TYPE_P(zv) != IS_UNDEF) {
				_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(h), zv);
				return SUCCESS;
			}
		}
		return FAILURE;
	}
	nIndex = h | ht->nTableMask;

	idx = HT_HASH(ht, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(ht, idx);
		if ((p->h == h) && (p->key == NULL)) {
			_zend_hash_del_el_ex(ht, idx, p, prev);
			return SUCCESS;
		}
		prev = p;
		idx = Z_NEXT(p->val);
	}
	return FAILURE;
}

ZEND_API void ZEND_FASTCALL zend_hash_destroy(HashTable *ht)
{
	IS_CONSISTENT(ht);
	HT_ASSERT(ht, GC_REFCOUNT(ht) <= 1);

	if (ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			if (ht->pDestructor) {
				zval *zv = ht->arPacked;
				zval *end = zv + ht->nNumUsed;

				SET_INCONSISTENT(HT_IS_DESTROYING);
				if (HT_IS_WITHOUT_HOLES(ht)) {
					do {
						ht->pDestructor(zv);
					} while (++zv != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE_P(zv) != IS_UNDEF)) {
							ht->pDestructor(zv);
						}
					} while (++zv != end);
				}
				SET_INCONSISTENT(HT_DESTROYED);
			}
			zend_hash_iterators_remove(ht);
		} else {
			Bucket *p = ht->arData;
			Bucket *end = p + ht->nNumUsed;

			if (ht->pDestructor) {
				SET_INCONSISTENT(HT_IS_DESTROYING);

				if (HT_HAS_STATIC_KEYS_ONLY(ht)) {
					if (HT_IS_WITHOUT_HOLES(ht)) {
						do {
							ht->pDestructor(&p->val);
						} while (++p != end);
					} else {
						do {
							if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
								ht->pDestructor(&p->val);
							}
						} while (++p != end);
					}
				} else if (HT_IS_WITHOUT_HOLES(ht)) {
					do {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
							if (EXPECTED(p->key)) {
								zend_string_release(p->key);
							}
						}
					} while (++p != end);
				}

				SET_INCONSISTENT(HT_DESTROYED);
			} else {
				if (!HT_HAS_STATIC_KEYS_ONLY(ht)) {
					do {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				}
			}
			zend_hash_iterators_remove(ht);
		}
	} else if (EXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
		return;
	}
	pefree(HT_GET_DATA_ADDR(ht), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
}

ZEND_API void ZEND_FASTCALL zend_array_destroy(HashTable *ht)
{
	IS_CONSISTENT(ht);
	HT_ASSERT(ht, GC_REFCOUNT(ht) <= 1);

	/* break possible cycles */
	GC_REMOVE_FROM_BUFFER(ht);
	GC_TYPE_INFO(ht) = GC_NULL /*???| (GC_WHITE << 16)*/;

	if (ht->nNumUsed) {
		/* In some rare cases destructors of regular arrays may be changed */
		if (UNEXPECTED(ht->pDestructor != ZVAL_PTR_DTOR)) {
			zend_hash_destroy(ht);
			goto free_ht;
		}

		SET_INCONSISTENT(HT_IS_DESTROYING);

		if (HT_IS_PACKED(ht)) {
			zval *zv = ht->arPacked;
			zval *end = zv + ht->nNumUsed;

			do {
				i_zval_ptr_dtor(zv);
			} while (++zv != end);
		} else {
			Bucket *p = ht->arData;
			Bucket *end = p + ht->nNumUsed;

			if (HT_HAS_STATIC_KEYS_ONLY(ht)) {
				do {
					i_zval_ptr_dtor(&p->val);
				} while (++p != end);
			} else if (HT_IS_WITHOUT_HOLES(ht)) {
				do {
					i_zval_ptr_dtor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release_ex(p->key, 0);
					}
				} while (++p != end);
			} else {
				do {
					if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
						i_zval_ptr_dtor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release_ex(p->key, 0);
						}
					}
				} while (++p != end);
			}
		}
	} else if (EXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
		goto free_ht;
	}
	SET_INCONSISTENT(HT_DESTROYED);
	efree(HT_GET_DATA_ADDR(ht));
free_ht:
	zend_hash_iterators_remove(ht);
	FREE_HASHTABLE(ht);
}

ZEND_API void ZEND_FASTCALL zend_hash_clean(HashTable *ht)
{
	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			zval *zv = ht->arPacked;
			zval *end = zv + ht->nNumUsed;

			if (ht->pDestructor) {
				if (HT_HAS_STATIC_KEYS_ONLY(ht)) {
					if (HT_IS_WITHOUT_HOLES(ht)) {
						do {
							ht->pDestructor(zv);
						} while (++zv != end);
					} else {
						do {
							if (EXPECTED(Z_TYPE_P(zv) != IS_UNDEF)) {
								ht->pDestructor(zv);
							}
						} while (++zv != end);
					}
				}
			}
		} else {
			Bucket *p = ht->arData;
			Bucket *end = p + ht->nNumUsed;

			if (ht->pDestructor) {
				if (HT_HAS_STATIC_KEYS_ONLY(ht)) {
					if (HT_IS_WITHOUT_HOLES(ht)) {
						do {
							ht->pDestructor(&p->val);
						} while (++p != end);
					} else {
						do {
							if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
								ht->pDestructor(&p->val);
							}
						} while (++p != end);
					}
				} else if (HT_IS_WITHOUT_HOLES(ht)) {
					do {
						ht->pDestructor(&p->val);
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				} else {
					do {
						if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
							ht->pDestructor(&p->val);
							if (EXPECTED(p->key)) {
								zend_string_release(p->key);
							}
						}
					} while (++p != end);
				}
			} else {
				if (!HT_HAS_STATIC_KEYS_ONLY(ht)) {
					do {
						if (EXPECTED(p->key)) {
							zend_string_release(p->key);
						}
					} while (++p != end);
				}
			}
			HT_HASH_RESET(ht);
		}
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = ZEND_LONG_MIN;
	ht->nInternalPointer = 0;
}

ZEND_API void ZEND_FASTCALL zend_symtable_clean(HashTable *ht)
{
	Bucket *p, *end;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (ht->nNumUsed) {
		ZEND_ASSERT(!HT_IS_PACKED(ht));
		p = ht->arData;
		end = p + ht->nNumUsed;
		if (HT_HAS_STATIC_KEYS_ONLY(ht)) {
			do {
				i_zval_ptr_dtor(&p->val);
			} while (++p != end);
		} else if (HT_IS_WITHOUT_HOLES(ht)) {
			do {
				i_zval_ptr_dtor(&p->val);
				if (EXPECTED(p->key)) {
					zend_string_release(p->key);
				}
			} while (++p != end);
		} else {
			do {
				if (EXPECTED(Z_TYPE(p->val) != IS_UNDEF)) {
					i_zval_ptr_dtor(&p->val);
					if (EXPECTED(p->key)) {
						zend_string_release(p->key);
					}
				}
			} while (++p != end);
		}
		HT_HASH_RESET(ht);
	}
	ht->nNumUsed = 0;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = ZEND_LONG_MIN;
	ht->nInternalPointer = 0;
}

ZEND_API void ZEND_FASTCALL zend_hash_graceful_destroy(HashTable *ht)
{
	uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (HT_IS_PACKED(ht)) {
		zval *zv = ht->arPacked;

		for (idx = 0; idx < ht->nNumUsed; idx++, zv++) {
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;
			_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
		}
	} else {
		Bucket *p = ht->arData;

		for (idx = 0; idx < ht->nNumUsed; idx++, p++) {
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
	}
	if (!(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
		pefree(HT_GET_DATA_ADDR(ht), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

ZEND_API void ZEND_FASTCALL zend_hash_graceful_reverse_destroy(HashTable *ht)
{
	uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	idx = ht->nNumUsed;
	if (HT_IS_PACKED(ht)) {
		zval *zv = ht->arPacked + ht->nNumUsed;

		while (idx > 0) {
			idx--;
			zv--;
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;
			_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
		}
	} else {
		Bucket *p = ht->arData + ht->nNumUsed;

		while (idx > 0) {
			idx--;
			p--;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
		}
	}

	if (!(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED)) {
		pefree(HT_GET_DATA_ADDR(ht), GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
	}

	SET_INCONSISTENT(HT_DESTROYED);
}

/* This is used to recurse elements and selectively delete certain entries
 * from a hashtable. apply_func() receives the data and decides if the entry
 * should be deleted or recursion should be stopped. The following three
 * return codes are possible:
 * ZEND_HASH_APPLY_KEEP   - continue
 * ZEND_HASH_APPLY_STOP   - stop iteration
 * ZEND_HASH_APPLY_REMOVE - delete the element, combinable with the former
 */

ZEND_API void ZEND_FASTCALL zend_hash_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);
	if (HT_IS_PACKED(ht)) {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			zval *zv = ht->arPacked + idx;

			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;
			result = apply_func(zv);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	} else {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			Bucket *p = ht->arData + idx;

			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			result = apply_func(&p->val);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	}
}


ZEND_API void ZEND_FASTCALL zend_hash_apply_with_argument(HashTable *ht, apply_func_arg_t apply_func, void *argument)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);
	if (HT_IS_PACKED(ht)) {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			zval *zv = ht->arPacked + idx;
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;
			result = apply_func(zv, argument);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	} else {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			Bucket *p = ht->arData + idx;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			result = apply_func(&p->val, argument);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	}
}


ZEND_API void zend_hash_apply_with_arguments(HashTable *ht, apply_func_args_t apply_func, int num_args, ...)
{
	uint32_t idx;
	va_list args;
	zend_hash_key hash_key;
	int result;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			zval *zv = ht->arPacked + idx;

			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;
			va_start(args, num_args);
			hash_key.h = idx;
			hash_key.key = NULL;

			result = apply_func(zv, num_args, args, &hash_key);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				va_end(args);
				break;
			}
			va_end(args);
		}
	} else {
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			Bucket *p = ht->arData + idx;

			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			va_start(args, num_args);
			hash_key.h = p->h;
			hash_key.key = p->key;

			result = apply_func(&p->val, num_args, args, &hash_key);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				va_end(args);
				break;
			}
			va_end(args);
		}
	}
}


ZEND_API void ZEND_FASTCALL zend_hash_reverse_apply(HashTable *ht, apply_func_t apply_func)
{
	uint32_t idx;
	int result;

	IS_CONSISTENT(ht);

	idx = ht->nNumUsed;
	if (HT_IS_PACKED(ht)) {
		zval *zv;

		while (idx > 0) {
			idx--;
			zv = ht->arPacked + idx;
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;

			result = apply_func(zv);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_packed_del_val(ht, HT_IDX_TO_HASH(idx), zv);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	} else {
		Bucket *p;

		while (idx > 0) {
			idx--;
			p = ht->arData + idx;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

			result = apply_func(&p->val);

			if (result & ZEND_HASH_APPLY_REMOVE) {
				HT_ASSERT_RC1(ht);
				_zend_hash_del_el(ht, HT_IDX_TO_HASH(idx), p);
			}
			if (result & ZEND_HASH_APPLY_STOP) {
				break;
			}
		}
	}
}


ZEND_API void ZEND_FASTCALL zend_hash_copy(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor)
{
	uint32_t idx;
	zval *new_entry, *data;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT_RC1(target);

	if (HT_IS_PACKED(source)) {
		for (idx = 0; idx < source->nNumUsed; idx++) {
			zval *zv = source->arPacked + idx;
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;

			new_entry = zend_hash_index_update(target, idx, zv);
			if (pCopyConstructor) {
				pCopyConstructor(new_entry);
			}
		}
		return;
	}

	for (idx = 0; idx < source->nNumUsed; idx++) {
		Bucket *p = source->arData + idx;

		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

		/* INDIRECT element may point to UNDEF-ined slots */
		data = &p->val;
		if (Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_P(data) == IS_UNDEF)) {
				continue;
			}
		}
		if (p->key) {
			new_entry = zend_hash_update(target, p->key, data);
		} else {
			new_entry = zend_hash_index_update(target, p->h, data);
		}
		if (pCopyConstructor) {
			pCopyConstructor(new_entry);
		}
	}
}


static zend_always_inline bool zend_array_dup_value(HashTable *source, HashTable *target, zval *data, zval *dest, bool packed, bool with_holes)
{
	if (with_holes) {
		if (!packed && Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
		}
		if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
			return 0;
		}
	} else if (!packed) {
		/* INDIRECT element may point to UNDEF-ined slots */
		if (Z_TYPE_INFO_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
			if (UNEXPECTED(Z_TYPE_INFO_P(data) == IS_UNDEF)) {
				return 0;
			}
		}
	}

	do {
		if (Z_OPT_REFCOUNTED_P(data)) {
			if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1 &&
			    (Z_TYPE_P(Z_REFVAL_P(data)) != IS_ARRAY ||
			      Z_ARRVAL_P(Z_REFVAL_P(data)) != source)) {
				data = Z_REFVAL_P(data);
				if (!Z_OPT_REFCOUNTED_P(data)) {
					break;
				}
			}
			Z_ADDREF_P(data);
		}
	} while (0);
	ZVAL_COPY_VALUE(dest, data);

	return 1;
}

static zend_always_inline bool zend_array_dup_element(HashTable *source, HashTable *target, uint32_t idx, Bucket *p, Bucket *q, bool packed, bool static_keys, bool with_holes)
{
	if (!zend_array_dup_value(source, target, &p->val, &q->val, packed, with_holes)) {
		return 0;
	}

	if (!packed) {
		uint32_t nIndex;

		q->h = p->h;
		q->key = p->key;
		if (!static_keys && q->key) {
			zend_string_addref(q->key);
		}

		nIndex = q->h | target->nTableMask;
		Z_NEXT(q->val) = HT_HASH(target, nIndex);
		HT_HASH(target, nIndex) = HT_IDX_TO_HASH(idx);
	}
	return 1;
}

// We need to duplicate iterators to be able to search through all copy-on-write copies to find the actually iterated HashTable and position back
static void zend_array_dup_ht_iterators(HashTable *source, HashTable *target) {
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == source) {
			uint32_t copy_idx = zend_hash_iterator_add(target, iter->pos);
			HashTableIterator *copy_iter = EG(ht_iterators) + copy_idx;
			copy_iter->next_copy = iter->next_copy;
			iter->next_copy = copy_idx;
		}
		iter++;
	}
}

static zend_always_inline void zend_array_dup_packed_elements(HashTable *source, HashTable *target, bool with_holes)
{
	zval *p = source->arPacked;
	zval *q = target->arPacked;
	zval *end = p + source->nNumUsed;

	do {
		if (!zend_array_dup_value(source, target, p, q, 1, with_holes)) {
			if (with_holes) {
				ZVAL_UNDEF(q);
			}
		}
		p++; q++;
	} while (p != end);

	if (UNEXPECTED(HT_HAS_ITERATORS(source))) {
		zend_array_dup_ht_iterators(source, target);
	}
}

static zend_always_inline uint32_t zend_array_dup_elements(HashTable *source, HashTable *target, bool static_keys, bool with_holes)
{
	uint32_t idx = 0;
	Bucket *p = source->arData;
	Bucket *q = target->arData;
	Bucket *end = p + source->nNumUsed;

	if (UNEXPECTED(HT_HAS_ITERATORS(source))) {
		zend_array_dup_ht_iterators(source, target);
	}

	do {
		if (!zend_array_dup_element(source, target, idx, p, q, 0, static_keys, with_holes)) {
			uint32_t target_idx = idx;

			idx++; p++;
			if (EXPECTED(!HT_HAS_ITERATORS(target))) {
				while (p != end) {
					if (zend_array_dup_element(source, target, target_idx, p, q, 0, static_keys, with_holes)) {
						if (source->nInternalPointer == idx) {
							target->nInternalPointer = target_idx;
						}
						target_idx++; q++;
					}
					idx++; p++;
				}
			} else {
				target->nNumUsed = source->nNumUsed;
				uint32_t iter_pos = zend_hash_iterators_lower_pos(target, idx);

				while (p != end) {
					if (zend_array_dup_element(source, target, target_idx, p, q, 0, static_keys, with_holes)) {
						if (source->nInternalPointer == idx) {
							target->nInternalPointer = target_idx;
						}
						if (UNEXPECTED(idx >= iter_pos)) {
							do {
								zend_hash_iterators_update(target, iter_pos, target_idx);
								iter_pos = zend_hash_iterators_lower_pos(target, iter_pos + 1);
							} while (iter_pos < idx);
						}
						target_idx++; q++;
					}
					idx++; p++;
				}
			}
			return target_idx;
		}
		idx++; p++; q++;
	} while (p != end);
	return idx;
}

ZEND_API HashTable* ZEND_FASTCALL zend_array_dup(HashTable *source)
{
	uint32_t idx;
	HashTable *target;

	IS_CONSISTENT(source);

	ALLOC_HASHTABLE(target);
	GC_SET_REFCOUNT(target, 1);
	GC_TYPE_INFO(target) = GC_ARRAY;

	target->pDestructor = ZVAL_PTR_DTOR;

	if (source->nNumOfElements == 0) {
		HT_FLAGS(target) = HASH_FLAG_UNINITIALIZED;
		target->nTableMask = HT_MIN_MASK;
		target->nNumUsed = 0;
		target->nNumOfElements = 0;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nInternalPointer = 0;
		target->nTableSize = HT_MIN_SIZE;
		HT_SET_DATA_ADDR(target, &uninitialized_bucket);
	} else if (GC_FLAGS(source) & IS_ARRAY_IMMUTABLE) {
		HT_FLAGS(target) = HT_FLAGS(source) & HASH_FLAG_MASK;
		target->nTableMask = source->nTableMask;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nTableSize = source->nTableSize;
		if (HT_IS_PACKED(source)) {
			HT_SET_DATA_ADDR(target, emalloc(HT_PACKED_SIZE(target)));
			target->nInternalPointer = source->nInternalPointer;
			memcpy(HT_GET_DATA_ADDR(target), HT_GET_DATA_ADDR(source), HT_PACKED_USED_SIZE(source));
		} else {
			HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
			target->nInternalPointer = source->nInternalPointer;
			memcpy(HT_GET_DATA_ADDR(target), HT_GET_DATA_ADDR(source), HT_USED_SIZE(source));
		}
	} else if (HT_IS_PACKED(source)) {
		HT_FLAGS(target) = HT_FLAGS(source) & HASH_FLAG_MASK;
		target->nTableMask = HT_MIN_MASK;
		target->nNumUsed = source->nNumUsed;
		target->nNumOfElements = source->nNumOfElements;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nTableSize = source->nTableSize;
		HT_SET_DATA_ADDR(target, emalloc(HT_PACKED_SIZE_EX(target->nTableSize, HT_MIN_MASK)));
		target->nInternalPointer =
			(source->nInternalPointer < source->nNumUsed) ?
				source->nInternalPointer : 0;

		HT_HASH_RESET_PACKED(target);

		if (HT_IS_WITHOUT_HOLES(target)) {
			zend_array_dup_packed_elements(source, target, 0);
		} else {
			zend_array_dup_packed_elements(source, target, 1);
		}
	} else {
		HT_FLAGS(target) = HT_FLAGS(source) & HASH_FLAG_MASK;
		target->nTableMask = source->nTableMask;
		target->nNextFreeElement = source->nNextFreeElement;
		target->nInternalPointer =
			(source->nInternalPointer < source->nNumUsed) ?
				source->nInternalPointer : 0;

		target->nTableSize = source->nTableSize;
		HT_SET_DATA_ADDR(target, emalloc(HT_SIZE(target)));
		HT_HASH_RESET(target);

		if (HT_HAS_STATIC_KEYS_ONLY(target)) {
			if (HT_IS_WITHOUT_HOLES(source)) {
				idx = zend_array_dup_elements(source, target, 1, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 1, 1);
			}
		} else {
			if (HT_IS_WITHOUT_HOLES(source)) {
				idx = zend_array_dup_elements(source, target, 0, 0);
			} else {
				idx = zend_array_dup_elements(source, target, 0, 1);
			}
		}
		target->nNumUsed = idx;
		target->nNumOfElements = idx;
	}
	return target;
}

ZEND_API HashTable* zend_array_to_list(HashTable *source)
{
	HashTable *result = _zend_new_array(zend_hash_num_elements(source));
	zend_hash_real_init_packed(result);

	ZEND_HASH_FILL_PACKED(result) {
		zval *entry;

		ZEND_HASH_FOREACH_VAL(source, entry) {
			if (UNEXPECTED(Z_ISREF_P(entry) && Z_REFCOUNT_P(entry) == 1)) {
				entry = Z_REFVAL_P(entry);
			}
			Z_TRY_ADDREF_P(entry);
			ZEND_HASH_FILL_ADD(entry);
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FILL_END();

	return result;
}


ZEND_API void ZEND_FASTCALL zend_hash_merge(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, bool overwrite)
{
	uint32_t idx;
	Bucket *p;
	zval *t, *s;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT_RC1(target);

	if (overwrite) {
		if (HT_IS_PACKED(source)) {
			for (idx = 0; idx < source->nNumUsed; idx++) {
				s = source->arPacked + idx;
				if (UNEXPECTED(Z_TYPE_P(s) == IS_UNDEF)) {
					continue;
				}
				t = zend_hash_index_update(target, idx, s);
				if (pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
			return;
		}

		for (idx = 0; idx < source->nNumUsed; idx++) {
			p = source->arData + idx;
			s = &p->val;
			if (UNEXPECTED(Z_TYPE_P(s) == IS_INDIRECT)) {
				s = Z_INDIRECT_P(s);
			}
			if (UNEXPECTED(Z_TYPE_P(s) == IS_UNDEF)) {
				continue;
			}
			if (p->key) {
				t = _zend_hash_add_or_update_i(target, p->key, s, HASH_UPDATE | HASH_UPDATE_INDIRECT);
				if (pCopyConstructor) {
					pCopyConstructor(t);
				}
			} else {
				t = zend_hash_index_update(target, p->h, s);
				if (pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
		}
	} else {
		if (HT_IS_PACKED(source)) {
			for (idx = 0; idx < source->nNumUsed; idx++) {
				s = source->arPacked + idx;
				if (UNEXPECTED(Z_TYPE_P(s) == IS_UNDEF)) {
					continue;
				}
				t = zend_hash_index_add(target, idx, s);
				if (t && pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
			return;
		}

		for (idx = 0; idx < source->nNumUsed; idx++) {
			p = source->arData + idx;
			s = &p->val;
			if (UNEXPECTED(Z_TYPE_P(s) == IS_INDIRECT)) {
				s = Z_INDIRECT_P(s);
			}
			if (UNEXPECTED(Z_TYPE_P(s) == IS_UNDEF)) {
				continue;
			}
			if (p->key) {
				t = _zend_hash_add_or_update_i(target, p->key, s, HASH_ADD | HASH_UPDATE_INDIRECT);
				if (t && pCopyConstructor) {
					pCopyConstructor(t);
				}
			} else {
				t = zend_hash_index_add(target, p->h, s);
				if (t && pCopyConstructor) {
					pCopyConstructor(t);
				}
			}
		}
	}
}


static bool ZEND_FASTCALL zend_hash_replace_checker_wrapper(HashTable *target, zval *source_data, zend_ulong h, zend_string *key, void *pParam, merge_checker_func_t merge_checker_func)
{
	zend_hash_key hash_key;

	hash_key.h = h;
	hash_key.key = key;
	return merge_checker_func(target, source_data, &hash_key, pParam);
}


ZEND_API void ZEND_FASTCALL zend_hash_merge_ex(HashTable *target, HashTable *source, copy_ctor_func_t pCopyConstructor, merge_checker_func_t pMergeSource, void *pParam)
{
	uint32_t idx;
	Bucket *p;
	zval *t;

	IS_CONSISTENT(source);
	IS_CONSISTENT(target);
	HT_ASSERT_RC1(target);

	ZEND_ASSERT(!HT_IS_PACKED(source));
	for (idx = 0; idx < source->nNumUsed; idx++) {
		p = source->arData + idx;
		if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
		if (zend_hash_replace_checker_wrapper(target, &p->val, p->h, p->key, pParam, pMergeSource)) {
			t = zend_hash_update(target, p->key, &p->val);
			if (pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
	}
}


/* Returns the hash table data if found and NULL if not. */
ZEND_API zval* ZEND_FASTCALL zend_hash_find(const HashTable *ht, zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	(void)zend_string_hash_val(key);
	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_find_known_hash(const HashTable *ht, const zend_string *key)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	p = zend_hash_find_bucket(ht, key);
	return p ? &p->val : NULL;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_str_find(const HashTable *ht, const char *str, size_t len)
{
	zend_ulong h;
	Bucket *p;

	IS_CONSISTENT(ht);

	h = zend_inline_hash_func(str, len);
	p = zend_hash_str_find_bucket(ht, str, len, h);
	return p ? &p->val : NULL;
}

ZEND_API zval* ZEND_FASTCALL zend_hash_index_find(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);

	if (HT_IS_PACKED(ht)) {
		if (h < ht->nNumUsed) {
			zval *zv = ht->arPacked + h;

			if (Z_TYPE_P(zv) != IS_UNDEF) {
				return zv;
			}
		}
		return NULL;
	}

	p = zend_hash_index_find_bucket(ht, h);
	return p ? &p->val : NULL;
}

ZEND_API zval* ZEND_FASTCALL _zend_hash_index_find(const HashTable *ht, zend_ulong h)
{
	Bucket *p;

	IS_CONSISTENT(ht);
	ZEND_ASSERT(!HT_IS_PACKED(ht));

	p = zend_hash_index_find_bucket(ht, h);
	return p ? &p->val : NULL;
}

ZEND_API void ZEND_FASTCALL zend_hash_internal_pointer_reset_ex(HashTable *ht, HashPosition *pos)
{
	IS_CONSISTENT(ht);
	HT_ASSERT(ht, &ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);
	*pos = _zend_hash_get_valid_pos(ht, 0);
}


/* This function will be extremely optimized by remembering
 * the end of the list
 */
ZEND_API void ZEND_FASTCALL zend_hash_internal_pointer_end_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT(ht, &ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	idx = ht->nNumUsed;
	if (HT_IS_PACKED(ht)) {
		while (idx > 0) {
			idx--;
			if (Z_TYPE(ht->arPacked[idx]) != IS_UNDEF) {
				*pos = idx;
				return;
			}
		}
	} else {
		while (idx > 0) {
			idx--;
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
				*pos = idx;
				return;
			}
		}
	}
	*pos = ht->nNumUsed;
}


ZEND_API zend_result ZEND_FASTCALL zend_hash_move_forward_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;

	IS_CONSISTENT(ht);
	HT_ASSERT(ht, &ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	idx = _zend_hash_get_valid_pos(ht, *pos);
	if (idx < ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			while (1) {
				idx++;
				if (idx >= ht->nNumUsed) {
					*pos = ht->nNumUsed;
					return SUCCESS;
				}
				if (Z_TYPE(ht->arPacked[idx]) != IS_UNDEF) {
					*pos = idx;
					return SUCCESS;
				}
			}
		} else {
			while (1) {
				idx++;
				if (idx >= ht->nNumUsed) {
					*pos = ht->nNumUsed;
					return SUCCESS;
				}
				if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
					*pos = idx;
					return SUCCESS;
				}
			}
		}
	} else {
		return FAILURE;
	}
}

ZEND_API zend_result ZEND_FASTCALL zend_hash_move_backwards_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx = *pos;

	IS_CONSISTENT(ht);
	HT_ASSERT(ht, &ht->nInternalPointer != pos || GC_REFCOUNT(ht) == 1);

	if (idx < ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			while (idx > 0) {
				idx--;
				if (Z_TYPE(ht->arPacked[idx]) != IS_UNDEF) {
					*pos = idx;
					return SUCCESS;
				}
			}
		} else {
			while (idx > 0) {
				idx--;
				if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) {
					*pos = idx;
					return SUCCESS;
				}
			}
		}
		*pos = ht->nNumUsed;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


/* This function should be made binary safe  */
ZEND_API int ZEND_FASTCALL zend_hash_get_current_key_ex(const HashTable *ht, zend_string **str_index, zend_ulong *num_index, const HashPosition *pos)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	idx = _zend_hash_get_valid_pos(ht, *pos);
	if (idx < ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			*num_index = idx;
			return HASH_KEY_IS_LONG;
		}
		p = ht->arData + idx;
		if (p->key) {
			*str_index = p->key;
			return HASH_KEY_IS_STRING;
		} else {
			*num_index = p->h;
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}

ZEND_API void ZEND_FASTCALL zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, const HashPosition *pos)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	idx = _zend_hash_get_valid_pos(ht, *pos);
	if (idx >= ht->nNumUsed) {
		ZVAL_NULL(key);
	} else {
		if (HT_IS_PACKED(ht)) {
			ZVAL_LONG(key, idx);
			return;
		}
		p = ht->arData + idx;
		if (p->key) {
			ZVAL_STR_COPY(key, p->key);
		} else {
			ZVAL_LONG(key, p->h);
		}
	}
}

ZEND_API int ZEND_FASTCALL zend_hash_get_current_key_type_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	idx = _zend_hash_get_valid_pos(ht, *pos);
	if (idx < ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			return HASH_KEY_IS_LONG;
		}
		p = ht->arData + idx;
		if (p->key) {
			return HASH_KEY_IS_STRING;
		} else {
			return HASH_KEY_IS_LONG;
		}
	}
	return HASH_KEY_NON_EXISTENT;
}


ZEND_API zval* ZEND_FASTCALL zend_hash_get_current_data_ex(HashTable *ht, HashPosition *pos)
{
	uint32_t idx;
	Bucket *p;

	IS_CONSISTENT(ht);
	idx = _zend_hash_get_valid_pos(ht, *pos);
	if (idx < ht->nNumUsed) {
		if (HT_IS_PACKED(ht)) {
			return &ht->arPacked[idx];
		}
		p = ht->arData + idx;
		return &p->val;
	} else {
		return NULL;
	}
}

ZEND_API void zend_hash_bucket_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;
	zend_string *key;

	val = p->val;
	h = p->h;
	key = p->key;

	p->val = q->val;
	p->h = q->h;
	p->key = q->key;

	q->val = val;
	q->h = h;
	q->key = key;
}

ZEND_API void zend_hash_bucket_renum_swap(Bucket *p, Bucket *q)
{
	zval val;

	val = p->val;
	p->val = q->val;
	q->val = val;
}

ZEND_API void zend_hash_bucket_packed_swap(Bucket *p, Bucket *q)
{
	zval val;
	zend_ulong h;

	val = p->val;
	h = p->h;

	p->val = q->val;
	p->h = q->h;

	q->val = val;
	q->h = h;
}

ZEND_API void ZEND_FASTCALL zend_hash_sort_ex(HashTable *ht, sort_func_t sort, bucket_compare_func_t compar, bool renumber)
{
	Bucket *p;
	uint32_t i, j;

	IS_CONSISTENT(ht);
	HT_ASSERT_RC1(ht);

	if (!(ht->nNumOfElements>1) && !(renumber && ht->nNumOfElements>0)) {
		/* Doesn't require sorting */
		return;
	}

	if (HT_IS_PACKED(ht)) {
		zend_hash_packed_to_hash(ht); // TODO: ???
	}

	if (HT_IS_WITHOUT_HOLES(ht)) {
		/* Store original order of elements in extra space to allow stable sorting. */
		for (i = 0; i < ht->nNumUsed; i++) {
			Z_EXTRA(ht->arData[i].val) = i;
		}
	} else {
		/* Remove holes and store original order. */
		for (j = 0, i = 0; j < ht->nNumUsed; j++) {
			p = ht->arData + j;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;
			if (i != j) {
				ht->arData[i] = *p;
			}
			Z_EXTRA(ht->arData[i].val) = i;
			i++;
		}
		ht->nNumUsed = i;
	}

	if (!HT_IS_PACKED(ht)) {
		/* We broke the hash collisions chains overriding Z_NEXT() by Z_EXTRA().
		 * Reset the hash headers table as well to avoid possible inconsistent
		 * access on recursive data structures.
	     *
	     * See Zend/tests/bug63882_2.phpt
		 */
		HT_HASH_RESET(ht);
	}

	sort((void *)ht->arData, ht->nNumUsed, sizeof(Bucket), (compare_func_t) compar,
			(swap_func_t)(renumber? zend_hash_bucket_renum_swap :
				(HT_IS_PACKED(ht) ? zend_hash_bucket_packed_swap : zend_hash_bucket_swap)));

	ht->nInternalPointer = 0;

	if (renumber) {
		for (j = 0; j < i; j++) {
			p = ht->arData + j;
			p->h = j;
			if (p->key) {
				zend_string_release(p->key);
				p->key = NULL;
			}
		}

		ht->nNextFreeElement = i;
	}
	if (HT_IS_PACKED(ht)) {
		if (!renumber) {
			zend_hash_packed_to_hash(ht);
		}
	} else {
		if (renumber) {
			void *new_data, *old_data = HT_GET_DATA_ADDR(ht);
			Bucket *old_buckets = ht->arData;
			zval *zv;

			new_data = pemalloc(HT_PACKED_SIZE_EX(ht->nTableSize, HT_MIN_MASK), (GC_FLAGS(ht) & IS_ARRAY_PERSISTENT));
			HT_FLAGS(ht) |= HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS;
			ht->nTableMask = HT_MIN_MASK;
			HT_SET_DATA_ADDR(ht, new_data);
			p = old_buckets;
			zv = ht->arPacked;
			for (i = 0; i < ht->nTableSize; i++) {
				ZVAL_COPY_VALUE(zv, &p->val);
				zv++;
				p++;
			}
			pefree(old_data, GC_FLAGS(ht) & IS_ARRAY_PERSISTENT);
			HT_HASH_RESET_PACKED(ht);
		} else {
			zend_hash_rehash(ht);
		}
	}
}

static zend_always_inline int zend_hash_compare_impl(HashTable *ht1, HashTable *ht2, compare_func_t compar, bool ordered) {
	uint32_t idx1, idx2;
	zend_string *key1, *key2;
	zend_ulong h1, h2;
	zval *pData1, *pData2;;
	int result;

	if (ht1->nNumOfElements != ht2->nNumOfElements) {
		return ht1->nNumOfElements > ht2->nNumOfElements ? 1 : -1;
	}

	for (idx1 = 0, idx2 = 0; idx1 < ht1->nNumUsed; idx1++) {
		if (HT_IS_PACKED(ht1)) {
			pData1 = ht1->arPacked + idx1;
			h1 = idx1;
			key1 = NULL;
		} else {
			Bucket *p = ht1->arData + idx1;
			pData1 = &p->val;
			h1 = p->h;
			key1 = p->key;
		}

		if (Z_TYPE_P(pData1) == IS_UNDEF) continue;
		if (ordered) {
			if (HT_IS_PACKED(ht2)) {
				while (1) {
					ZEND_ASSERT(idx2 != ht2->nNumUsed);
					pData2 = ht2->arPacked + idx2;
					h2 = idx2;
					key2 = NULL;
					if (Z_TYPE_P(pData2) != IS_UNDEF) break;
					idx2++;
				}
			} else {
				while (1) {
					Bucket *p;
					ZEND_ASSERT(idx2 != ht2->nNumUsed);
					p = ht2->arData + idx2;
					pData2 = &p->val;
					h2 = p->h;
					key2 = p->key;
					if (Z_TYPE_P(pData2) != IS_UNDEF) break;
					idx2++;
				}
			}
			if (key1 == NULL && key2 == NULL) { /* numeric indices */
				if (h1 != h2) {
					return h1 > h2 ? 1 : -1;
				}
			} else if (key1 != NULL && key2 != NULL) { /* string indices */
				if (ZSTR_LEN(key1) != ZSTR_LEN(key2)) {
					return ZSTR_LEN(key1) > ZSTR_LEN(key2) ? 1 : -1;
				}

				result = memcmp(ZSTR_VAL(key1), ZSTR_VAL(key2), ZSTR_LEN(key1));
				if (result != 0) {
					return result;
				}
			} else {
				/* Mixed key types: A string key is considered as larger */
				return key1 != NULL ? 1 : -1;
			}
			idx2++;
		} else {
			if (key1 == NULL) { /* numeric index */
				pData2 = zend_hash_index_find(ht2, h1);
				if (pData2 == NULL) {
					return 1;
				}
			} else { /* string index */
				pData2 = zend_hash_find(ht2, key1);
				if (pData2 == NULL) {
					return 1;
				}
			}
		}

		if (Z_TYPE_P(pData1) == IS_INDIRECT) {
			pData1 = Z_INDIRECT_P(pData1);
		}
		if (Z_TYPE_P(pData2) == IS_INDIRECT) {
			pData2 = Z_INDIRECT_P(pData2);
		}

		if (Z_TYPE_P(pData1) == IS_UNDEF) {
			if (Z_TYPE_P(pData2) != IS_UNDEF) {
				return -1;
			}
		} else if (Z_TYPE_P(pData2) == IS_UNDEF) {
			return 1;
		} else {
			result = compar(pData1, pData2);
			if (result != 0) {
				return result;
			}
		}
	}

	return 0;
}

ZEND_API int zend_hash_compare(HashTable *ht1, HashTable *ht2, compare_func_t compar, bool ordered)
{
	int result;
	IS_CONSISTENT(ht1);
	IS_CONSISTENT(ht2);

	if (ht1 == ht2) {
		return 0;
	}

	/* It's enough to protect only one of the arrays.
	 * The second one may be referenced from the first and this may cause
	 * false recursion detection.
	 */
	if (UNEXPECTED(GC_IS_RECURSIVE(ht1))) {
		zend_error_noreturn(E_ERROR, "Nesting level too deep - recursive dependency?");
	}

	GC_TRY_PROTECT_RECURSION(ht1);
	result = zend_hash_compare_impl(ht1, ht2, compar, ordered);
	GC_TRY_UNPROTECT_RECURSION(ht1);

	return result;
}


ZEND_API zval* ZEND_FASTCALL zend_hash_minmax(const HashTable *ht, compare_func_t compar, uint32_t flag)
{
	uint32_t idx;
	zval *res;

	IS_CONSISTENT(ht);

	if (ht->nNumOfElements == 0 ) {
		return NULL;
	}

	if (HT_IS_PACKED(ht)) {
		zval *zv;

		idx = 0;
		while (1) {
			if (idx == ht->nNumUsed) {
				return NULL;
			}
			if (Z_TYPE(ht->arPacked[idx]) != IS_UNDEF) break;
			idx++;
		}
		res = ht->arPacked + idx;
		for (; idx < ht->nNumUsed; idx++) {
			zv = ht->arPacked + idx;
			if (UNEXPECTED(Z_TYPE_P(zv) == IS_UNDEF)) continue;

			if (flag) {
				if (compar(res, zv) < 0) { /* max */
					res = zv;
				}
			} else {
				if (compar(res, zv) > 0) { /* min */
					res = zv;
				}
			}
		}
	} else {
		Bucket *p;

		idx = 0;
		while (1) {
			if (idx == ht->nNumUsed) {
				return NULL;
			}
			if (Z_TYPE(ht->arData[idx].val) != IS_UNDEF) break;
			idx++;
		}
		res = &ht->arData[idx].val;
		for (; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (UNEXPECTED(Z_TYPE(p->val) == IS_UNDEF)) continue;

			if (flag) {
				if (compar(res, &p->val) < 0) { /* max */
					res = &p->val;
				}
			} else {
				if (compar(res, &p->val) > 0) { /* min */
					res = &p->val;
				}
			}
		}
	}
	return res;
}

ZEND_API bool ZEND_FASTCALL _zend_handle_numeric_str_ex(const char *key, size_t length, zend_ulong *idx)
{
	const char *tmp = key;

	const char *end = key + length;

	if (*tmp == '-') {
		tmp++;
	}

	if ((*tmp == '0' && length > 1) /* numbers with leading zeros */
	 || (end - tmp > MAX_LENGTH_OF_LONG - 1) /* number too long */
	 || (SIZEOF_ZEND_LONG == 4 &&
	     end - tmp == MAX_LENGTH_OF_LONG - 1 &&
	     *tmp > '2')) { /* overflow */
		return 0;
	}
	*idx = (*tmp - '0');
	while (1) {
		++tmp;
		if (tmp == end) {
			if (*key == '-') {
				if (*idx-1 > ZEND_LONG_MAX) { /* overflow */
					return 0;
				}
				*idx = 0 - *idx;
			} else if (*idx > ZEND_LONG_MAX) { /* overflow */
				return 0;
			}
			return 1;
		}
		if (*tmp <= '9' && *tmp >= '0') {
			*idx = (*idx * 10) + (*tmp - '0');
		} else {
			return 0;
		}
	}
}

/* Takes a "symtable" hashtable (contains integer and non-numeric string keys)
 * and converts it to a "proptable" (contains only string keys).
 * If the symtable didn't need duplicating, its refcount is incremented.
 */
ZEND_API HashTable* ZEND_FASTCALL zend_symtable_to_proptable(HashTable *ht)
{
	zend_ulong num_key;
	zend_string *str_key;
	zval *zv;

	if (UNEXPECTED(HT_IS_PACKED(ht))) {
		goto convert;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY(ht, str_key) {
		if (!str_key) {
			goto convert;
		}
	} ZEND_HASH_FOREACH_END();

	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
		GC_ADDREF(ht);
	}

	return ht;

convert:
	{
		HashTable *new_ht = zend_new_array(zend_hash_num_elements(ht));

		ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, zv) {
			if (!str_key) {
				str_key = zend_long_to_str(num_key);
				zend_string_delref(str_key);
			}
			do {
				if (Z_OPT_REFCOUNTED_P(zv)) {
					if (Z_ISREF_P(zv) && Z_REFCOUNT_P(zv) == 1) {
						zv = Z_REFVAL_P(zv);
						if (!Z_OPT_REFCOUNTED_P(zv)) {
							break;
						}
					}
					Z_ADDREF_P(zv);
				}
			} while (0);
			zend_hash_update(new_ht, str_key, zv);
		} ZEND_HASH_FOREACH_END();

		return new_ht;
	}
}

/* Takes a "proptable" hashtable (contains only string keys) and converts it to
 * a "symtable" (contains integer and non-numeric string keys).
 * If the proptable didn't need duplicating, its refcount is incremented.
 */
ZEND_API HashTable* ZEND_FASTCALL zend_proptable_to_symtable(HashTable *ht, bool always_duplicate)
{
	zend_ulong num_key;
	zend_string *str_key;
	zval *zv;

	if (!HT_IS_PACKED(ht)) {
		ZEND_HASH_MAP_FOREACH_STR_KEY(ht, str_key) {
			/* The `str_key &&` here might seem redundant: property tables should
			 * only have string keys. Unfortunately, this isn't true, at the very
			 * least because of ArrayObject, which stores a symtable where the
			 * property table should be.
			 */
			if (str_key && ZEND_HANDLE_NUMERIC(str_key, num_key)) {
				goto convert;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (always_duplicate) {
		return zend_array_dup(ht);
	}

	if (EXPECTED(!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE))) {
		GC_ADDREF(ht);
	}

	return ht;

convert:
	{
		HashTable *new_ht = zend_new_array(zend_hash_num_elements(ht));

		ZEND_HASH_MAP_FOREACH_KEY_VAL_IND(ht, num_key, str_key, zv) {
			do {
				if (Z_OPT_REFCOUNTED_P(zv)) {
					if (Z_ISREF_P(zv) && Z_REFCOUNT_P(zv) == 1) {
						zv = Z_REFVAL_P(zv);
						if (!Z_OPT_REFCOUNTED_P(zv)) {
							break;
						}
					}
					Z_ADDREF_P(zv);
				}
			} while (0);
			/* Again, thank ArrayObject for `!str_key ||`. */
			if (!str_key || ZEND_HANDLE_NUMERIC(str_key, num_key)) {
				zend_hash_index_update(new_ht, num_key, zv);
			} else {
				zend_hash_update(new_ht, str_key, zv);
			}
		} ZEND_HASH_FOREACH_END();

		return new_ht;
	}
}
