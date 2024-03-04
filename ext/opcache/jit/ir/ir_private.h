/*
 * IR - Lightweight JIT Compilation Framework
 * (Common data structures and non public definitions)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_PRIVATE_H
#define IR_PRIVATE_H
#include <string.h>
#include <stdlib.h>

#ifdef IR_DEBUG
# include <assert.h>
# define IR_ASSERT(x) assert(x)
#else
# define IR_ASSERT(x)
#endif

#ifdef _WIN32
# include <intrin.h>
# ifdef _M_X64
#  pragma intrinsic(_BitScanForward64)
#  pragma intrinsic(_BitScanReverse64)
# endif
# pragma intrinsic(_BitScanForward)
# pragma intrinsic(_BitScanReverse)
#endif

#ifdef __has_builtin
# if __has_builtin(__builtin_expect)
#   define EXPECTED(condition)   __builtin_expect(!!(condition), 1)
#   define UNEXPECTED(condition) __builtin_expect(!!(condition), 0)
# endif
# if __has_attribute(__aligned__)
#  define IR_SET_ALIGNED(alignment, decl) decl __attribute__ ((__aligned__ (alignment)))
# endif
# if __has_attribute(__fallthrough__)
#  define IR_FALLTHROUGH __attribute__((__fallthrough__))
# endif
#elif defined(_WIN32)
# define IR_SET_ALIGNED(alignment, decl) __declspec(align(alignment)) decl
#else /* GCC prior to 10 or non-clang/msvc compilers */
#define __has_builtin(x) 0
#endif
#ifndef EXPECTED
# define EXPECTED(condition)   (condition)
# define UNEXPECTED(condition) (condition)
#endif
#ifndef IR_SET_ALIGNED
#  define IR_SET_ALIGNED(alignment, decl) decl
#endif
#ifndef IR_FALLTHROUGH
#  define IR_FALLTHROUGH ((void)0)
#endif

/*** Helper routines ***/

#define IR_ALIGNED_SIZE(size, alignment) \
	(((size) + ((alignment) - 1)) & ~((alignment) - 1))

#define IR_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define IR_MIN(a, b)          (((a) < (b)) ? (a) : (b))

#define IR_IS_POWER_OF_TWO(x) (!((x) & ((x) - 1)))

#define IR_LOG2(x) ir_ntzl(x)

IR_ALWAYS_INLINE uint8_t ir_rol8(uint8_t op1, uint8_t op2)
{
	return (op1 << op2) | (op1 >> (8 - op2));
}

IR_ALWAYS_INLINE uint16_t ir_rol16(uint16_t op1, uint16_t op2)
{
	return (op1 << op2) | (op1 >> (16 - op2));
}

IR_ALWAYS_INLINE uint32_t ir_rol32(uint32_t op1, uint32_t op2)
{
	return (op1 << op2) | (op1 >> (32 - op2));
}

IR_ALWAYS_INLINE uint64_t ir_rol64(uint64_t op1, uint64_t op2)
{
	return (op1 << op2) | (op1 >> (64 - op2));
}

IR_ALWAYS_INLINE uint8_t ir_ror8(uint8_t op1, uint8_t op2)
{
	return (op1 >> op2) | (op1 << (8 - op2));
}

IR_ALWAYS_INLINE uint16_t ir_ror16(uint16_t op1, uint16_t op2)
{
	return (op1 >> op2) | (op1 << (16 - op2));
}

IR_ALWAYS_INLINE uint32_t ir_ror32(uint32_t op1, uint32_t op2)
{
	return (op1 >> op2) | (op1 << (32 - op2));
}

IR_ALWAYS_INLINE uint64_t ir_ror64(uint64_t op1, uint64_t op2)
{
	return (op1 >> op2) | (op1 << (64 - op2));
}

/* Number of trailing zero bits (0x01 -> 0; 0x40 -> 6; 0x00 -> LEN) */
IR_ALWAYS_INLINE uint32_t ir_ntz(uint32_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctz))
	return __builtin_ctz(num);
#elif defined(_WIN32)
	uint32_t index;

	if (!_BitScanForward(&index, num)) {
		/* undefined behavior */
		return 32;
	}

	return index;
#else
	int n;

	if (num == 0) return 32;

	n = 1;
	if ((num & 0x0000ffff) == 0) {n += 16; num = num >> 16;}
	if ((num & 0x000000ff) == 0) {n +=  8; num = num >>  8;}
	if ((num & 0x0000000f) == 0) {n +=  4; num = num >>  4;}
	if ((num & 0x00000003) == 0) {n +=  2; num = num >>  2;}
	return n - (num & 1);
#endif
}

/* Number of trailing zero bits (0x01 -> 0; 0x40 -> 6; 0x00 -> LEN) */
IR_ALWAYS_INLINE uint32_t ir_ntzl(uint64_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_ctzl))
	return __builtin_ctzl(num);
#elif defined(_WIN64)
	unsigned long index;

	if (!_BitScanForward64(&index, num)) {
		/* undefined behavior */
		return 64;
	}

	return (uint32_t) index;
#else
	uint32_t n;

	if (num == 0) return 64;

	n = 1;
	if ((num & 0xffffffff) == 0) {n += 32; num = num >> 32;}
	if ((num & 0x0000ffff) == 0) {n += 16; num = num >> 16;}
	if ((num & 0x000000ff) == 0) {n +=  8; num = num >>  8;}
	if ((num & 0x0000000f) == 0) {n +=  4; num = num >>  4;}
	if ((num & 0x00000003) == 0) {n +=  2; num = num >>  2;}
	return n - (uint32_t)(num & 1);
#endif
}

/* Number of leading zero bits (Undefined for zero) */
IR_ALWAYS_INLINE int ir_nlz(uint32_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_clz))
	return __builtin_clz(num);
#elif defined(_WIN32)
	uint32_t index;

	if (!_BitScanReverse(&index, num)) {
		/* undefined behavior */
		return 32;
	}

	return (int) (32 - 1) - index;
#else
	uint32_t x;
	uint32_t n;

	n = 32;
	x = num >> 16; if (x != 0) {n -= 16; num = x;}
	x = num >> 8;  if (x != 0) {n -=  8; num = x;}
	x = num >> 4;  if (x != 0) {n -=  4; num = x;}
	x = num >> 2;  if (x != 0) {n -=  2; num = x;}
	x = num >> 1;  if (x != 0) return n - 2;
	return n - num;
#endif
}

IR_ALWAYS_INLINE int ir_nlzl(uint64_t num)
{
#if (defined(__GNUC__) || __has_builtin(__builtin_clzll))
	return __builtin_clzll(num);
#elif defined(_WIN64)
	unsigned long index;

	if (!_BitScanReverse64(&index, num)) {
		/* undefined behavior */
		return 64;
	}

	return (int) (64 - 1) - index;
#else
	uint64_t x;
	uint32_t n;

	n = 64;
	x = num >> 32; if (x != 0) {n -= 32; num = x;}
	x = num >> 16; if (x != 0) {n -= 16; num = x;}
	x = num >> 8;  if (x != 0) {n -=  8; num = x;}
	x = num >> 4;  if (x != 0) {n -=  4; num = x;}
	x = num >> 2;  if (x != 0) {n -=  2; num = x;}
	x = num >> 1;  if (x != 0) return n - 2;
	return n - (uint32_t)num;
#endif
}

/*** Helper data types ***/

/* Arena */
struct _ir_arena {
	char     *ptr;
	char     *end;
	ir_arena *prev;
};

IR_ALWAYS_INLINE ir_arena* ir_arena_create(size_t size)
{
	ir_arena *arena;

	IR_ASSERT(size >= IR_ALIGNED_SIZE(sizeof(ir_arena), 8));
	arena = (ir_arena*)ir_mem_malloc(size);
	arena->ptr = (char*) arena + IR_ALIGNED_SIZE(sizeof(ir_arena), 8);
	arena->end = (char*) arena + size;
	arena->prev = NULL;
	return arena;
}

IR_ALWAYS_INLINE void ir_arena_free(ir_arena *arena)
{
	do {
		ir_arena *prev = arena->prev;
		ir_mem_free(arena);
		arena = prev;
	} while (arena);
}

IR_ALWAYS_INLINE void* ir_arena_alloc(ir_arena **arena_ptr, size_t size)
{
	ir_arena *arena = *arena_ptr;
	char *ptr = arena->ptr;

	size = IR_ALIGNED_SIZE(size, 8);

	if (EXPECTED(size <= (size_t)(arena->end - ptr))) {
		arena->ptr = ptr + size;
	} else {
		size_t arena_size =
			UNEXPECTED((size + IR_ALIGNED_SIZE(sizeof(ir_arena), 8)) > (size_t)(arena->end - (char*) arena)) ?
				(size + IR_ALIGNED_SIZE(sizeof(ir_arena), 8)) :
				(size_t)(arena->end - (char*) arena);
		ir_arena *new_arena = (ir_arena*)ir_mem_malloc(arena_size);

		ptr = (char*) new_arena + IR_ALIGNED_SIZE(sizeof(ir_arena), 8);
		new_arena->ptr = (char*) new_arena + IR_ALIGNED_SIZE(sizeof(ir_arena), 8) + size;
		new_arena->end = (char*) new_arena + arena_size;
		new_arena->prev = arena;
		*arena_ptr = new_arena;
	}

	return (void*) ptr;
}

IR_ALWAYS_INLINE void* ir_arena_checkpoint(ir_arena *arena)
{
	return arena->ptr;
}

IR_ALWAYS_INLINE void ir_release(ir_arena **arena_ptr, void *checkpoint)
{
	ir_arena *arena = *arena_ptr;

	while (UNEXPECTED((char*)checkpoint > arena->end) ||
	       UNEXPECTED((char*)checkpoint <= (char*)arena)) {
		ir_arena *prev = arena->prev;
		ir_mem_free(arena);
		*arena_ptr = arena = prev;
	}
	IR_ASSERT((char*)checkpoint > (char*)arena && (char*)checkpoint <= arena->end);
	arena->ptr = (char*)checkpoint;
}

/* Bitsets */
#if defined(IR_TARGET_X86)
# define IR_BITSET_BITS 32
# define IR_BITSET_ONE  1U
# define ir_bitset_base_t uint32_t
# define ir_bitset_ntz  ir_ntz
#else
# define IR_BITSET_BITS   64
# ifdef _M_X64 /* MSVC*/
#  define IR_BITSET_ONE    1ui64
# else
#  define IR_BITSET_ONE    1UL
# endif
# define ir_bitset_base_t uint64_t
# define ir_bitset_ntz    ir_ntzl
#endif

typedef ir_bitset_base_t *ir_bitset;

IR_ALWAYS_INLINE uint32_t ir_bitset_len(uint32_t n)
{
	return (n + (IR_BITSET_BITS - 1)) / IR_BITSET_BITS;
}

IR_ALWAYS_INLINE ir_bitset ir_bitset_malloc(uint32_t n)
{
	return ir_mem_calloc(ir_bitset_len(n), IR_BITSET_BITS / 8);
}

IR_ALWAYS_INLINE void ir_bitset_incl(ir_bitset set, uint32_t n)
{
	set[n / IR_BITSET_BITS] |= IR_BITSET_ONE << (n % IR_BITSET_BITS);
}

IR_ALWAYS_INLINE void ir_bitset_excl(ir_bitset set, uint32_t n)
{
	set[n / IR_BITSET_BITS] &= ~(IR_BITSET_ONE << (n % IR_BITSET_BITS));
}

IR_ALWAYS_INLINE bool ir_bitset_in(const ir_bitset set, uint32_t n)
{
	return (set[(n / IR_BITSET_BITS)] & (IR_BITSET_ONE << (n % IR_BITSET_BITS))) != 0;
}

IR_ALWAYS_INLINE void ir_bitset_clear(ir_bitset set, uint32_t len)
{
	memset(set, 0, len * (IR_BITSET_BITS / 8));
}

IR_ALWAYS_INLINE void ir_bitset_fill(ir_bitset set, uint32_t len)
{
	memset(set, 0xff, len * (IR_BITSET_BITS / 8));
}

IR_ALWAYS_INLINE bool ir_bitset_empty(const ir_bitset set, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++) {
		if (set[i]) {
			return 0;
		}
	}
	return 1;
}

IR_ALWAYS_INLINE bool ir_bitset_equal(const ir_bitset set1, const ir_bitset set2, uint32_t len)
{
    return memcmp(set1, set2, len * (IR_BITSET_BITS / 8)) == 0;
}

IR_ALWAYS_INLINE void ir_bitset_copy(ir_bitset set1, const ir_bitset set2, uint32_t len)
{
    memcpy(set1, set2, len * (IR_BITSET_BITS / 8));
}

IR_ALWAYS_INLINE void ir_bitset_intersection(ir_bitset set1, const ir_bitset set2, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
		set1[i] &= set2[i];
	}
}

IR_ALWAYS_INLINE void ir_bitset_union(ir_bitset set1, const ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] |= set2[i];
	}
}

IR_ALWAYS_INLINE void ir_bitset_difference(ir_bitset set1, const ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		set1[i] = set1[i] & ~set2[i];
	}
}

IR_ALWAYS_INLINE bool ir_bitset_is_subset(const ir_bitset set1, const ir_bitset set2, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set1[i] & ~set2[i]) {
			return 0;
		}
	}
	return 1;
}

IR_ALWAYS_INLINE int ir_bitset_first(const ir_bitset set, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (set[i]) {
			return IR_BITSET_BITS * i + ir_bitset_ntz(set[i]);
		}
	}
	return -1; /* empty set */
}

IR_ALWAYS_INLINE int ir_bitset_last(const ir_bitset set, uint32_t len)
{
	uint32_t i = len;

	while (i > 0) {
		i--;
		if (set[i]) {
			uint32_t j = IR_BITSET_BITS * i - 1;
			ir_bitset_base_t x = set[i];
			do {
				x = x >> 1;
				j++;
			} while (x != 0);
			return j;
		}
	}
	return -1; /* empty set */
}

IR_ALWAYS_INLINE int ir_bitset_pop_first(ir_bitset set, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		ir_bitset_base_t x = set[i];

		if (x) {
			int bit = IR_BITSET_BITS * i + ir_bitset_ntz(x);
			set[i] = x & (x - 1);
			return bit;
		}
	}
	return -1; /* empty set */
}

#define IR_BITSET_FOREACH(set, len, bit) do { \
	ir_bitset _set = (set); \
	uint32_t _i, _len = (len); \
	for (_i = 0; _i < _len; _set++, _i++) { \
		ir_bitset_base_t _x = *_set; \
		while (_x) { \
			(bit) = IR_BITSET_BITS * _i + ir_bitset_ntz(_x); \
			_x &= _x - 1;

#define IR_BITSET_FOREACH_DIFFERENCE(set1, set2, len, bit) do { \
	ir_bitset _set1 = (set1); \
	ir_bitset _set2 = (set2); \
	uint32_t _i, _len = (len); \
	for (_i = 0; _i < _len; _i++) { \
		ir_bitset_base_t _x = _set1[_i] & ~_set2[_i]; \
		while (_x) { \
			(bit) = IR_BITSET_BITS * _i + ir_bitset_ntz(_x); \
			_x &= _x - 1;

#define IR_BITSET_FOREACH_END() \
		} \
	} \
} while (0)

/* Bit Queue */
typedef struct _ir_bitqueue {
	uint32_t  len;
	uint32_t  pos;
	ir_bitset set;
} ir_bitqueue;

IR_ALWAYS_INLINE void ir_bitqueue_init(ir_bitqueue *q, uint32_t n)
{
	q->len = ir_bitset_len(n);
	q->pos = q->len - 1;
	q->set = ir_bitset_malloc(n);
}

IR_ALWAYS_INLINE void ir_bitqueue_free(ir_bitqueue *q)
{
	ir_mem_free(q->set);
}

IR_ALWAYS_INLINE void ir_bitqueue_clear(ir_bitqueue *q)
{
	q->pos = q->len - 1;
	ir_bitset_clear(q->set, q->len);
}

IR_ALWAYS_INLINE int ir_bitqueue_pop(ir_bitqueue *q)
{
	uint32_t i = q->pos;
	ir_bitset_base_t x, *p = q->set + i;
	do {
		x = *p;
		if (x) {
			int bit = IR_BITSET_BITS * i + ir_bitset_ntz(x);
			*p = x & (x - 1);
			q->pos = i;
			return bit;
		}
		p++;
		i++;
	} while (i < q->len);
	q->pos = q->len - 1;
	return -1; /* empty set */
}

IR_ALWAYS_INLINE void ir_bitqueue_add(ir_bitqueue *q, uint32_t n)
{
	uint32_t i = n / IR_BITSET_BITS;
	q->set[i] |= IR_BITSET_ONE << (n % IR_BITSET_BITS);
	if (i < q->pos) {
		q->pos = i;
	}
}

IR_ALWAYS_INLINE void ir_bitqueue_del(ir_bitqueue *q, uint32_t n)
{
	ir_bitset_excl(q->set, n);
}

IR_ALWAYS_INLINE bool ir_bitqueue_in(const ir_bitqueue *q, uint32_t n)
{
	return ir_bitset_in(q->set, n);
}

/* Dynamic array of numeric references */
typedef struct _ir_array {
	ir_ref   *refs;
	uint32_t  size;
} ir_array;

void ir_array_grow(ir_array *a, uint32_t size);
void ir_array_insert(ir_array *a, uint32_t i, ir_ref val);
void ir_array_remove(ir_array *a, uint32_t i);

IR_ALWAYS_INLINE void ir_array_init(ir_array *a, uint32_t size)
{
	a->refs = ir_mem_malloc(size * sizeof(ir_ref));
	a->size = size;
}

IR_ALWAYS_INLINE void ir_array_free(ir_array *a)
{
	ir_mem_free(a->refs);
	a->refs = NULL;
	a->size = 0;
}

IR_ALWAYS_INLINE uint32_t ir_array_size(const ir_array *a)
{
	return a->size;
}

IR_ALWAYS_INLINE ir_ref ir_array_get(const ir_array *a, uint32_t i)
{
	return (i < a->size) ? a->refs[i] : IR_UNUSED;
}

IR_ALWAYS_INLINE ir_ref ir_array_at(const ir_array *a, uint32_t i)
{
	IR_ASSERT(i < a->size);
	return a->refs[i];
}

IR_ALWAYS_INLINE void ir_array_set(ir_array *a, uint32_t i, ir_ref val)
{
	if (i >= a->size) {
		ir_array_grow(a, i + 1);
	}
	a->refs[i] = val;
}

IR_ALWAYS_INLINE void ir_array_set_unchecked(ir_array *a, uint32_t i, ir_ref val)
{
	IR_ASSERT(i < a->size);
	a->refs[i] = val;
}

/* List/Stack of numeric references */
typedef struct _ir_list {
	ir_array a;
	uint32_t len;
} ir_list;

bool ir_list_contains(const ir_list *l, ir_ref val);
void ir_list_insert(ir_list *l, uint32_t i, ir_ref val);
void ir_list_remove(ir_list *l, uint32_t i);

IR_ALWAYS_INLINE void ir_list_init(ir_list *l, uint32_t size)
{
	ir_array_init(&l->a, size);
	l->len = 0;
}

IR_ALWAYS_INLINE void ir_list_free(ir_list *l)
{
	ir_array_free(&l->a);
	l->len = 0;
}

IR_ALWAYS_INLINE void ir_list_clear(ir_list *l)
{
	l->len = 0;
}

IR_ALWAYS_INLINE uint32_t ir_list_len(const ir_list *l)
{
	return l->len;
}

IR_ALWAYS_INLINE uint32_t ir_list_capasity(const ir_list *l)
{
	return ir_array_size(&l->a);
}

IR_ALWAYS_INLINE void ir_list_push(ir_list *l, ir_ref val)
{
	ir_array_set(&l->a, l->len++, val);
}

IR_ALWAYS_INLINE void ir_list_push_unchecked(ir_list *l, ir_ref val)
{
	ir_array_set_unchecked(&l->a, l->len++, val);
}

IR_ALWAYS_INLINE ir_ref ir_list_pop(ir_list *l)
{
	IR_ASSERT(l->len > 0);
	return ir_array_at(&l->a, --l->len);
}

IR_ALWAYS_INLINE ir_ref ir_list_peek(const ir_list *l)
{
	IR_ASSERT(l->len > 0);
	return ir_array_at(&l->a, l->len - 1);
}

IR_ALWAYS_INLINE ir_ref ir_list_at(const ir_list *l, uint32_t i)
{
	IR_ASSERT(i < l->len);
	return ir_array_at(&l->a, i);
}

IR_ALWAYS_INLINE void ir_list_set(ir_list *l, uint32_t i, ir_ref val)
{
	IR_ASSERT(i < l->len);
	ir_array_set_unchecked(&l->a, i, val);
}

/* Worklist (unique list) */
typedef struct _ir_worklist {
	ir_list l;
	ir_bitset visited;
} ir_worklist;

IR_ALWAYS_INLINE void ir_worklist_init(ir_worklist *w, uint32_t size)
{
	ir_list_init(&w->l, size);
	w->visited = ir_bitset_malloc(size);
}

IR_ALWAYS_INLINE void ir_worklist_free(ir_worklist *w)
{
	ir_list_free(&w->l);
	ir_mem_free(w->visited);
}

IR_ALWAYS_INLINE uint32_t ir_worklist_len(const ir_worklist *w)
{
	return ir_list_len(&w->l);
}

IR_ALWAYS_INLINE uint32_t ir_worklist_capasity(const ir_worklist *w)
{
	return ir_list_capasity(&w->l);
}

IR_ALWAYS_INLINE void ir_worklist_clear(ir_worklist *w)
{
	ir_list_clear(&w->l);
	ir_bitset_clear(w->visited, ir_bitset_len(ir_worklist_capasity(w)));
}

IR_ALWAYS_INLINE bool ir_worklist_push(ir_worklist *w, ir_ref val)
{
	IR_ASSERT(val >= 0 && (uint32_t)val < ir_worklist_capasity(w));
	if (ir_bitset_in(w->visited, val)) {
		return 0;
	}
	ir_bitset_incl(w->visited, val);
	IR_ASSERT(ir_list_len(&w->l) < ir_list_capasity(&w->l));
	ir_list_push_unchecked(&w->l, val);
	return 1;
}

IR_ALWAYS_INLINE ir_ref ir_worklist_pop(ir_worklist *w)
{
	return ir_list_pop(&w->l);
}

IR_ALWAYS_INLINE ir_ref ir_worklist_peek(const ir_worklist *w)
{
	return ir_list_peek(&w->l);
}

/* IR Hash Table */
#define IR_INVALID_IDX 0xffffffff
#define IR_INVALID_VAL 0x80000000

typedef struct _ir_hashtab_bucket {
	uint32_t    key;
	ir_ref      val;
	uint32_t    next;
} ir_hashtab_bucket;

typedef struct _ir_hashtab {
	void       *data;
	uint32_t    mask;
	uint32_t    size;
	uint32_t    count;
	uint32_t    pos;
} ir_hashtab;

void ir_hashtab_init(ir_hashtab *tab, uint32_t size);
void ir_hashtab_free(ir_hashtab *tab);
ir_ref ir_hashtab_find(const ir_hashtab *tab, uint32_t key);
bool ir_hashtab_add(ir_hashtab *tab, uint32_t key, ir_ref val);
void ir_hashtab_key_sort(ir_hashtab *tab);

/* IR Addr Table */
typedef struct _ir_addrtab_bucket {
	uint64_t    key;
	ir_ref      val;
	uint32_t    next;
} ir_addrtab_bucket;

void ir_addrtab_init(ir_hashtab *tab, uint32_t size);
void ir_addrtab_free(ir_hashtab *tab);
ir_ref ir_addrtab_find(const ir_hashtab *tab, uint64_t key);
void ir_addrtab_set(ir_hashtab *tab, uint64_t key, ir_ref val);

/*** IR OP info ***/
extern const uint8_t ir_type_flags[IR_LAST_TYPE];
extern const char *ir_type_name[IR_LAST_TYPE];
extern const char *ir_type_cname[IR_LAST_TYPE];
extern const uint8_t ir_type_size[IR_LAST_TYPE];
extern const uint32_t ir_op_flags[IR_LAST_OP];
extern const char *ir_op_name[IR_LAST_OP];

#define IR_IS_CONST_OP(op)       ((op) > IR_NOP && (op) <= IR_C_FLOAT)
#define IR_IS_FOLDABLE_OP(op)    ((op) <= IR_LAST_FOLDABLE_OP)
#define IR_IS_SYM_CONST(op)      ((op) == IR_STR || (op) == IR_SYM || (op) == IR_FUNC)

ir_ref ir_const_ex(ir_ctx *ctx, ir_val val, uint8_t type, uint32_t optx);

IR_ALWAYS_INLINE bool ir_const_is_true(const ir_insn *v)
{

	if (v->type == IR_BOOL) {
		return v->val.b;
	} else if (IR_IS_TYPE_INT(v->type)) {
		return v->val.i64 != 0;
	} else if (v->type == IR_DOUBLE) {
		return v->val.d != 0.0;
	} else {
		IR_ASSERT(v->type == IR_FLOAT);
		return v->val.f != 0.0;
	}
	return 0;
}

IR_ALWAYS_INLINE bool ir_ref_is_true(ir_ctx *ctx, ir_ref ref)
{
	if (ref == IR_TRUE) {
		return 1;
	} else if (ref == IR_FALSE) {
		return 0;
	} else {
		IR_ASSERT(IR_IS_CONST_REF(ref));
		return ir_const_is_true(&ctx->ir_base[ref]);
	}
}

/* IR OP flags */
#define IR_OP_FLAG_OPERANDS_SHIFT 3

#define IR_OP_FLAG_EDGES_MASK     0x03
#define IR_OP_FLAG_VAR_INPUTS     0x04
#define IR_OP_FLAG_OPERANDS_MASK  0x18
#define IR_OP_FLAG_MEM_MASK       ((1<<6)|(1<<7))

#define IR_OP_FLAG_DATA           (1<<8)
#define IR_OP_FLAG_CONTROL        (1<<9)
#define IR_OP_FLAG_MEM            (1<<10)
#define IR_OP_FLAG_COMMUTATIVE    (1<<11)
#define IR_OP_FLAG_BB_START       (1<<12)
#define IR_OP_FLAG_BB_END         (1<<13)
#define IR_OP_FLAG_TERMINATOR     (1<<14)
#define IR_OP_FLAG_PINNED         (1<<15)

#define IR_OP_FLAG_MEM_LOAD       ((0<<6)|(0<<7))
#define IR_OP_FLAG_MEM_STORE      ((0<<6)|(1<<7))
#define IR_OP_FLAG_MEM_CALL       ((1<<6)|(0<<7))
#define IR_OP_FLAG_MEM_ALLOC      ((1<<6)|(1<<7))
#define IR_OP_FLAG_MEM_MASK       ((1<<6)|(1<<7))

#define IR_OPND_UNUSED            0x0
#define IR_OPND_DATA              0x1
#define IR_OPND_CONTROL           0x2
#define IR_OPND_CONTROL_DEP       0x3
#define IR_OPND_CONTROL_REF       0x4
#define IR_OPND_STR               0x5
#define IR_OPND_NUM               0x6
#define IR_OPND_PROB              0x7
#define IR_OPND_PROTO             0x8

#define IR_OP_FLAGS(op_flags, op1_flags, op2_flags, op3_flags) \
	((op_flags) | ((op1_flags) << 20) | ((op2_flags) << 24) | ((op3_flags) << 28))

#define IR_INPUT_EDGES_COUNT(flags) (flags & IR_OP_FLAG_EDGES_MASK)
#define IR_OPERANDS_COUNT(flags)    ((flags & IR_OP_FLAG_OPERANDS_MASK) >> IR_OP_FLAG_OPERANDS_SHIFT)

#define IR_OP_HAS_VAR_INPUTS(flags) ((flags) & IR_OP_FLAG_VAR_INPUTS)

#define IR_OPND_KIND(flags, i) \
	(((flags) >> (16 + (4 * (((i) > 3) ? 3 : (i))))) & 0xf)

#define IR_IS_REF_OPND_KIND(kind) \
	((kind) >= IR_OPND_DATA && (kind) <= IR_OPND_CONTROL_REF)

IR_ALWAYS_INLINE ir_ref ir_operands_count(const ir_ctx *ctx, const ir_insn *insn)
{
	uint32_t flags = ir_op_flags[insn->op];
	uint32_t n = IR_OPERANDS_COUNT(flags);

	if (UNEXPECTED(IR_OP_HAS_VAR_INPUTS(flags))) {
		/* MERGE, PHI, CALL, etc */
		n = insn->inputs_count;
	}
	return n;
}

IR_ALWAYS_INLINE ir_ref ir_input_edges_count(const ir_ctx *ctx, const ir_insn *insn)
{
	uint32_t flags = ir_op_flags[insn->op];
	uint32_t n = IR_INPUT_EDGES_COUNT(flags);
	if (UNEXPECTED(IR_OP_HAS_VAR_INPUTS(flags))) {
		/* MERGE, PHI, CALL, etc */
		n = insn->inputs_count;
	}
	return n;
}

IR_ALWAYS_INLINE uint32_t ir_insn_inputs_to_len(uint32_t inputs_count)
{
	return 1 + (inputs_count >> 2);
}

IR_ALWAYS_INLINE uint32_t ir_insn_len(const ir_insn *insn)
{
	return ir_insn_inputs_to_len(insn->inputs_count);
}

/*** IR Context Private Flags (ir_ctx->flags2) ***/
#define IR_CFG_HAS_LOOPS       (1<<0)
#define IR_IRREDUCIBLE_CFG     (1<<1)
#define IR_HAS_ALLOCA          (1<<2)
#define IR_HAS_CALLS           (1<<3)
#define IR_OPT_IN_SCCP         (1<<4)
#define IR_LINEAR              (1<<5)
#define IR_HAS_VA_START        (1<<6)
#define IR_HAS_VA_COPY         (1<<7)
#define IR_HAS_VA_ARG_GP       (1<<8)
#define IR_HAS_VA_ARG_FP       (1<<9)
#define IR_HAS_FP_RET_SLOT     (1<<10)

/* Temporary: SCCP -> CFG */
#define IR_SCCP_DONE           (1<<25)

/* Temporary: Dominators -> Loops */
#define IR_NO_LOOPS            (1<<25)

/* Temporary: Live Ranges */
#define IR_LR_HAVE_DESSA_MOVES (1<<25)

/* Temporary: Register Allocator */
#define IR_RA_HAVE_SPLITS      (1<<25)
#define IR_RA_HAVE_SPILLS      (1<<26)

#define IR_RESERVED_FLAG_1     (1U<<31)

/*** IR Binding ***/
IR_ALWAYS_INLINE ir_ref ir_binding_find(const ir_ctx *ctx, ir_ref ref)
{
	ir_ref var = ir_hashtab_find(ctx->binding, ref);
	return (var != (ir_ref)IR_INVALID_VAL) ? var : 0;
}

/*** IR Use Lists ***/
struct _ir_use_list {
	ir_ref        refs; /* index in ir_ctx->use_edges[] array */
	ir_ref        count;
};

void ir_use_list_remove_all(ir_ctx *ctx, ir_ref from, ir_ref use);
void ir_use_list_remove_one(ir_ctx *ctx, ir_ref from, ir_ref use);
void ir_use_list_replace(ir_ctx *ctx, ir_ref ref, ir_ref use, ir_ref new_use);
bool ir_use_list_add(ir_ctx *ctx, ir_ref to, ir_ref new_use);

/*** IR Basic Blocks info ***/
#define IR_IS_BB_START(op) \
	((ir_op_flags[op] & IR_OP_FLAG_BB_START) != 0)

#define IR_IS_BB_MERGE(op) \
	((op) == IR_MERGE || (op) == IR_LOOP_BEGIN)

#define IR_IS_BB_END(op) \
	((ir_op_flags[op] & IR_OP_FLAG_BB_END) != 0)

#define IR_BB_UNREACHABLE      (1<<0)
#define IR_BB_START            (1<<1)
#define IR_BB_ENTRY            (1<<2)
#define IR_BB_LOOP_HEADER      (1<<3)
#define IR_BB_IRREDUCIBLE_LOOP (1<<4)
#define IR_BB_DESSA_MOVES      (1<<5) /* translation out of SSA requires MOVEs */
#define IR_BB_EMPTY            (1<<6)
#define IR_BB_PREV_EMPTY_ENTRY (1<<7)
#define IR_BB_OSR_ENTRY_LOADS  (1<<8) /* OSR Entry-point with register LOADs   */
#define IR_BB_LOOP_WITH_ENTRY  (1<<9) /* set together with LOOP_HEADER if there is an ENTRY in the loop */

/* The following flags are set by GCM */
#define IR_BB_HAS_PHI          (1<<10)
#define IR_BB_HAS_PI           (1<<11)
#define IR_BB_HAS_PARAM        (1<<12)
#define IR_BB_HAS_VAR          (1<<13)


struct _ir_block {
	uint32_t flags;
	ir_ref   start;              /* index of first instruction                 */
	ir_ref   end;                /* index of last instruction                  */
	uint32_t successors;         /* index in ir_ctx->cfg_edges[] array         */
	uint32_t successors_count;
	uint32_t predecessors;       /* index in ir_ctx->cfg_edges[] array         */
	uint32_t predecessors_count;
	union {
		uint32_t dom_parent;     /* immediate dominator block                  */
		uint32_t idom;           /* immediate dominator block                  */
	};
	union {
		uint32_t dom_depth;      /* depth from the root of the dominators tree */
		uint32_t postnum;        /* used temporary during tree constructon     */
	};
	uint32_t     dom_child;      /* first dominated blocks                     */
	uint32_t     dom_next_child; /* next dominated block (linked list)         */
	uint32_t     loop_header;
	uint32_t     loop_depth;
};

uint32_t ir_skip_empty_target_blocks(const ir_ctx *ctx, uint32_t b);
uint32_t ir_next_block(const ir_ctx *ctx, uint32_t b);
void ir_get_true_false_blocks(const ir_ctx *ctx, uint32_t b, uint32_t *true_block, uint32_t *false_block);

IR_ALWAYS_INLINE uint32_t ir_phi_input_number(const ir_ctx *ctx, const ir_block *bb, uint32_t from)
{
	uint32_t n, *p;

	for (n = 0, p = &ctx->cfg_edges[bb->predecessors]; n < bb->predecessors_count; p++, n++) {
		if (*p == from) {
			return n + 2; /* first input is a reference to MERGE */
		}
	}
	IR_ASSERT(0);
	return 0;
}

/*** Folding Engine (see ir.c and ir_fold.h) ***/
typedef enum _ir_fold_action {
	IR_FOLD_DO_RESTART,
	IR_FOLD_DO_CSE,
	IR_FOLD_DO_EMIT,
	IR_FOLD_DO_COPY,
	IR_FOLD_DO_CONST
} ir_fold_action;

ir_ref ir_folding(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3, ir_insn *op1_insn, ir_insn *op2_insn, ir_insn *op3_insn);

/*** IR Live Info ***/
typedef ir_ref                   ir_live_pos;
typedef struct _ir_use_pos       ir_use_pos;

#define IR_SUB_REFS_COUNT                4

#define IR_LOAD_SUB_REF                  0
#define IR_USE_SUB_REF                   1
#define IR_DEF_SUB_REF                   2
#define IR_SAVE_SUB_REF                  3

#define IR_LIVE_POS_TO_REF(pos)          ((pos) / IR_SUB_REFS_COUNT)
#define IR_LIVE_POS_TO_SUB_REF(pos)      ((pos) % IR_SUB_REFS_COUNT)

#define IR_LIVE_POS_FROM_REF(ref)        ((ref) * IR_SUB_REFS_COUNT)

#define IR_START_LIVE_POS_FROM_REF(ref)  ((ref) * IR_SUB_REFS_COUNT)
#define IR_LOAD_LIVE_POS_FROM_REF(ref)   ((ref) * IR_SUB_REFS_COUNT + IR_LOAD_SUB_REF)
#define IR_USE_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_USE_SUB_REF)
#define IR_DEF_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_DEF_SUB_REF)
#define IR_SAVE_LIVE_POS_FROM_REF(ref)   ((ref) * IR_SUB_REFS_COUNT + IR_SAVE_SUB_REF)
#define IR_END_LIVE_POS_FROM_REF(ref)    ((ref) * IR_SUB_REFS_COUNT + IR_SUB_REFS_COUNT)

/* ir_use_pos.flags bits */
#define IR_USE_MUST_BE_IN_REG            (1<<0)
#define IR_USE_SHOULD_BE_IN_REG          (1<<1)
#define IR_DEF_REUSES_OP1_REG            (1<<2)
#define IR_DEF_CONFLICTS_WITH_INPUT_REGS (1<<3)

#define IR_FUSED_USE                     (1<<6)
#define IR_PHI_USE                       (1<<7)

#define IR_OP1_MUST_BE_IN_REG            (1<<8)
#define IR_OP1_SHOULD_BE_IN_REG          (1<<9)
#define IR_OP2_MUST_BE_IN_REG            (1<<10)
#define IR_OP2_SHOULD_BE_IN_REG          (1<<11)
#define IR_OP3_MUST_BE_IN_REG            (1<<12)
#define IR_OP3_SHOULD_BE_IN_REG          (1<<13)

#define IR_USE_FLAGS(def_flags, op_num)  (((def_flags) >> (6 + (IR_MIN((op_num), 3) * 2))) & 3)

struct _ir_use_pos {
	uint16_t       op_num; /* 0 - means result */
	int8_t         hint;
	uint8_t        flags;
	ir_ref         hint_ref; /* negative references are used for FUSION anf PHI */
	ir_live_pos    pos;
	ir_use_pos    *next;
};

struct _ir_live_range {
	ir_live_pos    start; /* inclusive */
	ir_live_pos    end;   /* exclusive */
	ir_live_range *next;
};

/* ir_live_interval.flags bits (two low bits are reserved for temporary register number) */
#define IR_LIVE_INTERVAL_FIXED           (1<<0)
#define IR_LIVE_INTERVAL_TEMP            (1<<1)
#define IR_LIVE_INTERVAL_HAS_HINT_REGS   (1<<2)
#define IR_LIVE_INTERVAL_HAS_HINT_REFS   (1<<3)
#define IR_LIVE_INTERVAL_MEM_PARAM       (1<<4)
#define IR_LIVE_INTERVAL_MEM_LOAD        (1<<5)
#define IR_LIVE_INTERVAL_COALESCED       (1<<6)
#define IR_LIVE_INTERVAL_SPILL_SPECIAL   (1<<7) /* spill slot is pre-allocated in a special area (see ir_ctx.spill_reserved_base) */
#define IR_LIVE_INTERVAL_SPILLED         (1<<8)
#define IR_LIVE_INTERVAL_SPLIT_CHILD     (1<<9)

struct _ir_live_interval {
	uint8_t           type;
	int8_t            reg;
	uint16_t          flags;
	union {
		int32_t       vreg;
		int32_t       tmp_ref;
	};
	union {
		int32_t       stack_spill_pos;
		ir_ref        tmp_op_num;
	};
	ir_live_pos       end;       /* end of the last live range (cahce of ival.range.{next->}end) */
	ir_live_range     range;
	ir_live_range    *current_range;
	ir_use_pos       *use_pos;
	ir_live_interval *next;
	ir_live_interval *list_next; /* linked list of active, inactive or unhandled intervals */
};

typedef int (*emit_copy_t)(ir_ctx *ctx, uint8_t type, ir_ref from, ir_ref to);

int ir_gen_dessa_moves(ir_ctx *ctx, uint32_t b, emit_copy_t emit_copy);

#if defined(IR_REGSET_64BIT)

/*typedef enum _ir_reg ir_reg;*/
typedef int8_t ir_reg;

/*** Register Sets ***/
#if IR_REGSET_64BIT
typedef uint64_t ir_regset;
#else
typedef uint32_t ir_regset;
#endif

#define IR_REGSET_EMPTY 0

#define IR_REGSET_IS_EMPTY(regset) \
	(regset == IR_REGSET_EMPTY)

#define IR_REGSET_IS_SINGLETON(regset) \
	(regset && !(regset & (regset - 1)))

#if IR_REGSET_64BIT
# define IR_REGSET(reg) \
	(1ull << (reg))
#else
# define IR_REGSET(reg) \
	(1u << (reg))
#endif

#if IR_REGSET_64BIT
# define IR_REGSET_INTERVAL(reg1, reg2) \
	(((1ull << ((reg2) - (reg1) + 1)) - 1) << (reg1))
#else
# define IR_REGSET_INTERVAL(reg1, reg2) \
	(((1u << ((reg2) - (reg1) + 1)) - 1) << (reg1))
#endif

#define IR_REGSET_IN(regset, reg) \
	(((regset) & IR_REGSET(reg)) != 0)

#define IR_REGSET_INCL(regset, reg) \
	(regset) |= IR_REGSET(reg)

#define IR_REGSET_EXCL(regset, reg) \
	(regset) &= ~IR_REGSET(reg)

#define IR_REGSET_UNION(set1, set2) \
	((set1) | (set2))

#define IR_REGSET_INTERSECTION(set1, set2) \
	((set1) & (set2))

#define IR_REGSET_DIFFERENCE(set1, set2) \
	((set1) & ~(set2))

#if IR_REGSET_64BIT
# define IR_REGSET_FIRST(set) ((ir_reg)ir_ntzl(set))
# define ir_REGSET_LAST(set)  ((ir_reg)(ir_nlzl(set)(set)^63))
#else
# define IR_REGSET_FIRST(set) ((ir_reg)ir_ntz(set))
# define IR_REGSET_LAST(set)  ((ir_reg)(ir_nlz(set)^31))
#endif

IR_ALWAYS_INLINE ir_reg ir_regset_pop_first(ir_regset *set)
{
	ir_reg reg;

	IR_ASSERT(!IR_REGSET_IS_EMPTY(*set));
	reg = IR_REGSET_FIRST(*set);
	*set = (*set) & ((*set) - 1);
	return reg;
}

#define IR_REGSET_FOREACH(set, reg) \
	do { \
		ir_regset _tmp = (set); \
		while (!IR_REGSET_IS_EMPTY(_tmp)) { \
			reg = ir_regset_pop_first(&_tmp);

#define IR_REGSET_FOREACH_END() \
		} \
	} while (0)

#endif /* defined(IR_REGSET_64BIT) */

/*** IR Register Allocation ***/
/* Flags for ctx->regs[][] (low bits are used for register number itself) */
typedef struct _ir_reg_alloc_data {
	int32_t unused_slot_4;
	int32_t unused_slot_2;
	int32_t unused_slot_1;
	ir_live_interval **handled;
} ir_reg_alloc_data;

int32_t ir_allocate_spill_slot(ir_ctx *ctx, ir_type type, ir_reg_alloc_data *data);

IR_ALWAYS_INLINE void ir_set_alocated_reg(ir_ctx *ctx, ir_ref ref, int op_num, int8_t reg)
{
	int8_t *regs = ctx->regs[ref];

	if (op_num > 0) {
		/* regs[] is not limited by the declared boundary 4, the real boundary checked below */
		IR_ASSERT(op_num <= IR_MAX(3, ctx->ir_base[ref].inputs_count));
	}
	regs[op_num] = reg;
}

IR_ALWAYS_INLINE int8_t ir_get_alocated_reg(const ir_ctx *ctx, ir_ref ref, int op_num)
{
	int8_t *regs = ctx->regs[ref];

	/* regs[] is not limited by the declared boundary 4, the real boundary checked below */
	IR_ASSERT(op_num <= IR_MAX(3, ctx->ir_base[ref].inputs_count));
	return regs[op_num];
}

/*** IR Target Interface ***/

/* ctx->rules[] flags */
#define IR_FUSED     (1U<<31) /* Insn is fused into others (code is generated as part of the fusion root) */
#define IR_SKIPPED   (1U<<30) /* Insn is skipped (code is not generated) */
#define IR_SIMPLE    (1U<<29) /* Insn doesn't have any target constraints */
#define IR_FUSED_REG (1U<<28) /* Register assignemnt may be stored in ctx->fused_regs instead of ctx->regs */
#define IR_MAY_SWAP  (1U<<27) /* Allow swapping operands for better register allocation */
#define IR_MAY_REUSE (1U<<26) /* Result may reuse register of the source */

#define IR_RULE_MASK 0xff

extern const char *ir_rule_name[];

typedef struct _ir_target_constraints ir_target_constraints;

#define IR_TMP_REG(_num, _type, _start, _end) \
	(ir_tmp_reg){.num=(_num), .type=(_type), .start=(_start), .end=(_end)}
#define IR_SCRATCH_REG(_reg, _start, _end) \
	(ir_tmp_reg){.reg=(_reg), .type=IR_VOID, .start=(_start), .end=(_end)}

int ir_get_target_constraints(ir_ctx *ctx, ir_ref ref, ir_target_constraints *constraints);

void ir_fix_stack_frame(ir_ctx *ctx);

/* Utility */
ir_type ir_get_return_type(ir_ctx *ctx);
bool ir_is_fastcall(const ir_ctx *ctx, const ir_insn *insn);
bool ir_is_vararg(const ir_ctx *ctx, ir_insn *insn);

//#define IR_BITSET_LIVENESS

#endif /* IR_PRIVATE_H */
