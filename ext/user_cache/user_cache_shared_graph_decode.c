/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_shared_graph.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_fibers.h"
#include "Zend/zend_operators.h"

#define UCACHE_DECODE_CACHE_MAX_ENTRIES			4096U
#define UCACHE_DECODE_MAX_DEPTH					8192U
#define UCACHE_VERBATIM_VERDICT_ADDR_SHIFT		4
#define UCACHE_VERBATIM_VERDICT_ADDR_MIX_SHIFT	14
#ifdef ZEND_CHECK_STACK_LIMIT
# define UCACHE_DECODE_COUNTS_NESTING			0
#else
# define UCACHE_DECODE_COUNTS_NESTING			1
#endif
#define UCACHE_DECODE_CLASS_HAS_WAKEUP			((uintptr_t) 1)
#define UCACHE_DECODE_RESOLVE_KIND_SHIFT		1
#define UCACHE_DECODE_RESOLVE_KIND_CLASS		((uintptr_t) 1 << UCACHE_DECODE_RESOLVE_KIND_SHIFT)
#define UCACHE_DECODE_RESOLVE_KIND_ENUM_CASE	((uintptr_t) 2 << UCACHE_DECODE_RESOLVE_KIND_SHIFT)
#define UCACHE_DECODE_RESOLVE_KIND_MASK			((uintptr_t) 3 << UCACHE_DECODE_RESOLVE_KIND_SHIFT)
#define UCACHE_DECODE_RESOLVE_TAG_MASK			(UCACHE_DECODE_CLASS_HAS_WAKEUP | UCACHE_DECODE_RESOLVE_KIND_MASK)
#define UCACHE_VERBATIM_ARR_GC_TYPE_INFO \
	(GC_ARRAY | ((IS_ARRAY_IMMUTABLE | GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT))
#define UCACHE_VERBATIM_ARR_HT_FLAGS_ALLOWED \
	(HASH_FLAG_PACKED | HASH_FLAG_STATIC_KEYS | HASH_FLAG_ALLOW_COW_VIOLATION)

#if ZEND_DEBUG
# define UCACHE_HT_DISALLOW_COW_VIOLATION(ht) HT_FLAGS(ht) &= ~HASH_FLAG_ALLOW_COW_VIOLATION
# define UCACHE_ASSERT_RESOLVED_WAKEUP(obj_zv, has_wakeup) \
	ZEND_ASSERT((has_wakeup) == (zend_hash_find_known_hash(&Z_OBJCE_P(obj_zv)->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL))
#else
# define UCACHE_HT_DISALLOW_COW_VIOLATION(ht)
# define UCACHE_ASSERT_RESOLVED_WAKEUP(obj_zv, has_wakeup)
#endif

#define UCACHE_DEFINE_DECODE_MAP(name, ctype, dtor, release_on_fail) \
	static zend_always_inline void ucache_decode_##name##_map_teardown(void) \
	{ \
		HashTable *map = UC_G(decode_##name##_map); \
		if (map != NULL) { \
			UC_G(decode_##name##_map) = NULL; \
			zend_hash_destroy(map); \
			efree(map); \
		} \
	} \
	\
	static zend_always_inline bool ucache_decode_##name##_map_insert(uint32_t offset, ctype *entry) \
	{ \
		if (UC_G(decode_##name##_map) == NULL) { \
			UC_G(decode_##name##_map) = emalloc(sizeof(HashTable)); \
			zend_hash_init(UC_G(decode_##name##_map), 8, NULL, dtor, 0); \
		} \
		\
		GC_ADDREF(entry); \
		if (zend_hash_index_add_ptr(UC_G(decode_##name##_map), offset, entry) == NULL) { \
			release_on_fail; \
			\
			return false; \
		} \
		\
		return true; \
	} \
	\
	static zend_always_inline ctype *ucache_decode_##name##_map_find(uint32_t offset) \
	{ \
		if (UC_G(decode_##name##_map) == NULL) { \
			return NULL; \
		} \
		\
		return zend_hash_index_find_ptr(UC_G(decode_##name##_map), offset); \
	}

#if ZEND_DEBUG
typedef struct {
	const uint8_t *old_base;
	const uint8_t *new_base;
	size_t len;
	HashTable *seen;
} ucache_sgraph_rebase_ctx;
#endif

typedef struct {
	const uint8_t *buf;
	const uint8_t *snapshot_origin;
	size_t buf_len;
	HashTable seen;
	uint32_t depth;
} ucache_verbatim_check_ctx;

typedef struct {
	zval obj;
	zval state;
} ucache_restore_call;

struct _ucache_owned_decode_frame {
	ucache_owned_decode_frame *prev;
	uint64_t owner_pid;
	HashTable *strs;
	HashTable *arrs;
	HashTable *resolve;
	HashTable *shapes;
	HashTable *saved_identity_map;
	HashTable *saved_ref_map;
	ucache_sgraph_snapshot *snapshot;
};

struct _ucache_sgraph_snapshot {
	const uint8_t *origin;
	size_t len;
	uint8_t data[1];
};

struct _ucache_restore_queue {
	ucache_restore_queue *prev;
	ucache_restore_call *calls;
	HashTable *saved_identity_map;
	HashTable *saved_ref_map;
	uint32_t depth;
	uint32_t count;
	uint32_t capacity;
	uint32_t next;
};

static void ucache_owned_str_dtor(zval *val);
static void ucache_decode_shape_proto_dtor(zval *zv);
static bool ucache_restore_queue_finish(ucache_restore_queue *queue, bool result);
static void ucache_restore_queue_destroy(ucache_restore_queue *queue);
static bool ucache_verbatim_arr_check(ucache_verbatim_check_ctx *ctx, const zend_array *arr);
static zend_never_inline bool ucache_decode_verbatim_arr_validate(
		const uint8_t *buf,
		size_t buf_len,
		const uint8_t *snapshot_origin,
		const zend_array *arr,
		const void *shm_arr);
static zend_never_inline void ucache_decode_save_maps(void);
static ZEND_COLD zend_never_inline void ucache_decode_release_keeping_overflow(zval *dst);
static PHP_UCACHE_HOT bool ucache_sgraph_decode_val(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst);

static zend_always_inline void ucache_restore_queue_detach_next(
		ucache_restore_queue *queue,
		zval *obj,
		zval *state)
{
	ucache_restore_call *call = &queue->calls[queue->next];

	ZVAL_COPY_VALUE(obj, &call->obj);
	ZVAL_COPY_VALUE(state, &call->state);

	queue->next++;
}

static zend_always_inline bool ucache_restore_queue_finish_impl(
		ucache_restore_queue *queue,
		bool result,
		bool owned)
{
	ucache_restore_call *call;
	zend_object *obj;
	zend_function *wakeup;
	zval retval, obj_zv, state;

	while (queue->next < queue->count) {
		call = &queue->calls[queue->next];
		obj = Z_OBJ(call->obj);

		ZVAL_COPY_VALUE(&state, &call->state);

		if (result && !EG(exception) && (!owned || ucache_owned_decode_validate_proc_impl())) {
			if (Z_ISUNDEF(state)) {
				wakeup = zend_hash_find_ptr(&obj->ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));
				BG(serialize_lock)++;

				zend_call_known_instance_method(wakeup, obj, &retval, 0, NULL);
				BG(serialize_lock)--;

				result = !EG(exception) && (!owned || ucache_owned_decode_validate_proc_impl());

				zval_ptr_dtor(&retval);
			} else {
				zend_object_set_properties_reinitable(obj, true);
				BG(serialize_lock)++;

				zend_call_known_instance_method_with_1_params(obj->ce->__unserialize, obj, NULL, &state);
				BG(serialize_lock)--;

				zend_object_set_properties_reinitable(obj, false);

				result = !EG(exception) && (!owned || ucache_owned_decode_validate_proc_impl());
			}
		} else {
			result = false;
		}

		if (!result) {
			GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
		}

		ucache_restore_queue_detach_next(queue, &obj_zv, &state);

		zval_ptr_dtor(&obj_zv);
		zval_ptr_dtor(&state);
	}

	return result &&
		!EG(exception) &&
		(!owned || ucache_owned_decode_validate_proc_impl())
	;
}

static zend_always_inline bool ucache_decode_range_ok(
		size_t buf_len,
		uint32_t offset,
		size_t need)
{
	return offset <= buf_len && need <= buf_len - offset;
}

static zend_always_inline bool ucache_decode_arr_range_ok(
		size_t buf_len,
		uint32_t offset,
		uint32_t count,
		size_t elem_size)
{
	if (offset > buf_len) {
		return false;
	}

	return count <= (buf_len - offset) / elem_size;
}

static zend_always_inline bool ucache_decode_fail_zval(zval *dst)
{
	ucache_decode_release_keeping_overflow(dst);

	return false;
}

static zend_always_inline bool ucache_decode_str_hdr_ok(const zend_string *str)
{
	return GC_TYPE(str) == IS_STRING && (GC_FLAGS(str) & IS_STR_INTERNED) != 0;
}

static zend_always_inline zend_string *ucache_decode_str_at_raw(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t offset)
{
	zend_string *str;

	if (!ucache_decode_range_ok(buf_len, offset, _ZSTR_HEADER_SIZE)) {
		return NULL;
	}

	str = (zend_string *) (void *) (buf + offset);
	if (!ucache_decode_str_hdr_ok(str) ||
		ZSTR_LEN(str) > buf_len ||
		!ucache_decode_range_ok(
			buf_len,
			offset,
			_ZSTR_STRUCT_SIZE(ZSTR_LEN(str))
		)
	) {
		return NULL;
	}

	return str;
}

static zend_always_inline bool ucache_owned_decode_frame_reads_inherited_lease(
		const ucache_owned_decode_frame *frame)
{
	return frame->owner_pid != ucache_cached_pid() && frame->snapshot == NULL;
}

static zend_always_inline zend_string *ucache_owned_str_dup(const zend_string *src)
{
	zend_string *str = zend_string_init(ZSTR_VAL(src), ZSTR_LEN(src), 0);

	ZSTR_H(str) = ZSTR_H(src);

	if (ZSTR_IS_VALID_UTF8(src)) {
		GC_ADD_FLAGS(str, IS_STR_VALID_UTF8);
	}

	return str;
}

static zend_always_inline zend_string *ucache_owned_decode_str(zend_string *src)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);
	zend_string *str;
	zend_ulong key = (zend_ulong) (uintptr_t) src;

	ZEND_ASSERT(frame != NULL);
	if (frame->strs == NULL) {
		frame->strs = emalloc(sizeof(HashTable));

		zend_hash_init(frame->strs, 8, NULL, ucache_owned_str_dtor, 0);
	} else {
		str = zend_hash_index_find_ptr(frame->strs, key);
		if (str != NULL) {
			return str;
		}
	}

	str = ucache_owned_str_dup(src);

	zend_hash_index_add_new_ptr(frame->strs, key, str);

	return str;
}

static zend_always_inline zend_string *ucache_decode_str_at(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t offset)
{
	zend_string *str = ucache_decode_str_at_raw(buf, buf_len, offset);

	if (str != NULL && UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
		return ucache_owned_decode_str(str);
	}

	return str;
}

static zend_always_inline bool ucache_owned_verbatim_val_is_borrowed(const zval *val)
{
	return Z_TYPE_P(val) == IS_ARRAY || Z_TYPE_P(val) == IS_STRING;
}

static zend_always_inline size_t ucache_decode_node_hdr_size(uint8_t type)
{
	switch (type) {
		case UCACHE_SGRAPH_VAL_DYNAMIC_ARR:
			return sizeof(ucache_sgraph_arr);
		case UCACHE_SGRAPH_VAL_SHAPED_ARR:
			return sizeof(ucache_sgraph_shaped_arr);
		case UCACHE_SGRAPH_VAL_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_OBJ:
			return sizeof(ucache_sgraph_obj);
		case UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ:
			return sizeof(ucache_sgraph_safe_direct_obj);
		case UCACHE_SGRAPH_VAL_SERIALIZED_OBJ:
			return sizeof(ucache_sgraph_serialized_obj);
		case UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ:
			return sizeof(ucache_sgraph_shaped_state_obj);
		case UCACHE_SGRAPH_VAL_SERDES_OBJ:
			return sizeof(ucache_sgraph_serdes_obj);
		case UCACHE_SGRAPH_VAL_ENUM:
			return sizeof(ucache_sgraph_enum);
		case UCACHE_SGRAPH_VAL_REF:
			return sizeof(ucache_sgraph_ref);
		default:
			return 0;
	}
}

static zend_always_inline uintptr_t ucache_decode_resolved_of_kind(uintptr_t resolved, uintptr_t kind)
{
	ZEND_ASSERT(kind != 0 && (kind & ~UCACHE_DECODE_RESOLVE_KIND_MASK) == 0);

	return (resolved & UCACHE_DECODE_RESOLVE_KIND_MASK) == kind ? resolved : 0;
}

static zend_always_inline uintptr_t ucache_decode_resolve_cache_find(const void *addr, uintptr_t kind)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);
	uintptr_t resolved = 0;
	uint32_t i;

	if (UNEXPECTED(frame != NULL)) {
		if (frame->resolve != NULL) {
			resolved = (uintptr_t) zend_hash_index_find_ptr(frame->resolve, (zend_ulong) (uintptr_t) addr);
		}

		return ucache_decode_resolved_of_kind(resolved, kind);
	}

	for (i = 0; i < UCACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
		if (UC_G(decode_resolve_direct_keys)[i] == addr) {
			resolved = (uintptr_t) UC_G(decode_resolve_direct_vals)[i];

			return ucache_decode_resolved_of_kind(resolved, kind);
		}
	}

	if (UC_G(decode_resolve_cache) == NULL) {
		return 0;
	}

	resolved = (uintptr_t) zend_hash_index_find_ptr(
		UC_G(decode_resolve_cache),
		(zend_ulong) (uintptr_t) addr
	);

	return ucache_decode_resolved_of_kind(resolved, kind);
}

static zend_always_inline void ucache_decode_resolve_cache_store(const void *addr, uintptr_t resolved)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);
	void *val = (void *) resolved;
	uint32_t slot;

	ZEND_ASSERT((resolved & UCACHE_DECODE_RESOLVE_KIND_MASK) != 0);

	if (UNEXPECTED(frame != NULL)) {
		if (frame->resolve == NULL) {
			frame->resolve = emalloc(sizeof(HashTable));

			zend_hash_init(frame->resolve, 8, NULL, NULL, 0);
		}

		zend_hash_index_add_ptr(frame->resolve, (zend_ulong) (uintptr_t) addr, val);

		return;
	}

	slot = UC_G(decode_resolve_direct_next)++ % UCACHE_DECODE_DIRECT_CACHE_SLOTS;

	UC_G(decode_resolve_direct_keys)[slot] = addr;
	UC_G(decode_resolve_direct_vals)[slot] = val;

	if (UC_G(decode_resolve_cache) == NULL) {
		UC_G(decode_resolve_cache) = emalloc(sizeof(HashTable));

		zend_hash_init(UC_G(decode_resolve_cache), 8, NULL, NULL, 0);
	} else if (zend_hash_num_elements(UC_G(decode_resolve_cache)) >= UCACHE_DECODE_CACHE_MAX_ENTRIES) {
		zend_hash_clean(UC_G(decode_resolve_cache));
	}

	zend_hash_index_add_ptr(
		UC_G(decode_resolve_cache),
		(zend_ulong) (uintptr_t) addr,
		val
	);
}

static zend_always_inline uintptr_t ucache_decode_resolved_class(zend_class_entry *ce)
{
	uintptr_t resolved = (uintptr_t) ce | UCACHE_DECODE_RESOLVE_KIND_CLASS;

	ZEND_ASSERT(((uintptr_t) ce & UCACHE_DECODE_RESOLVE_TAG_MASK) == 0);

	if (zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL) {
		return resolved | UCACHE_DECODE_CLASS_HAS_WAKEUP;
	}

	return resolved;
}

static zend_always_inline zend_class_entry *ucache_decode_resolved_class_entry(uintptr_t resolved)
{
	ZEND_ASSERT(resolved == 0 || (resolved & UCACHE_DECODE_RESOLVE_KIND_MASK) == UCACHE_DECODE_RESOLVE_KIND_CLASS);

	return (zend_class_entry *) (resolved & ~UCACHE_DECODE_RESOLVE_TAG_MASK);
}

static zend_always_inline bool ucache_decode_resolved_class_has_wakeup(uintptr_t resolved)
{
	return (resolved & UCACHE_DECODE_CLASS_HAS_WAKEUP) != 0;
}

static zend_always_inline uintptr_t ucache_decode_resolved_enum_case(zend_object *case_obj)
{
	ZEND_ASSERT(((uintptr_t) case_obj & UCACHE_DECODE_RESOLVE_TAG_MASK) == 0);

	return (uintptr_t) case_obj | UCACHE_DECODE_RESOLVE_KIND_ENUM_CASE;
}

static zend_always_inline zend_object *ucache_decode_resolved_enum_case_obj(uintptr_t resolved)
{
	ZEND_ASSERT(resolved == 0 || (resolved & UCACHE_DECODE_RESOLVE_KIND_MASK) == UCACHE_DECODE_RESOLVE_KIND_ENUM_CASE);

	return (zend_object *) (resolved & ~UCACHE_DECODE_RESOLVE_TAG_MASK);
}

UCACHE_DEFINE_DECODE_MAP(
		identity,
		zend_object,
		ucache_obj_table_dtor,
		OBJ_RELEASE(entry))

UCACHE_DEFINE_DECODE_MAP(
		ref,
		zend_reference,
		ucache_ref_table_dtor,
		if (GC_DELREF(entry) == 0) efree_size(entry, sizeof(zend_reference)))

static zend_always_inline uint32_t ucache_decode_enter(void)
{
	uint32_t depth = ++UC_G(decode_depth);

	if (depth == 1) {
		UC_G(decode_nesting) = 0;
	}

	zend_fiber_switch_block();

	if (UNEXPECTED(UC_G(decode_identity_map) != NULL || UC_G(decode_ref_map) != NULL)) {
		ucache_decode_save_maps();
	}

	return depth;
}

static zend_always_inline bool ucache_decode_leave_impl(uint32_t depth, bool result)
{
	ucache_restore_queue *queue = UC_G(decode_restore_queue);

	if (queue != NULL && queue->depth == depth) {
		result = ucache_restore_queue_finish(queue, result);

		ZEND_ASSERT(UC_G(decode_restore_queue) == queue);
	} else {
		queue = NULL;
	}

	ucache_decode_identity_map_teardown();
	ucache_decode_ref_map_teardown();

	if (queue != NULL) {
		UC_G(decode_identity_map) = queue->saved_identity_map;
		UC_G(decode_ref_map) = queue->saved_ref_map;
		UC_G(decode_restore_queue) = queue->prev;

		ucache_restore_queue_destroy(queue);
	}

	UC_G(decode_depth) = depth - 1;

	zend_fiber_switch_unblock();

	return result;
}

static zend_always_inline bool ucache_decode_nesting_overflowed(uint32_t nesting)
{
	if (!UCACHE_DECODE_COUNTS_NESTING || EXPECTED(nesting < UCACHE_DECODE_MAX_DEPTH)) {
		return false;
	}

	UC_G(stack_overflowed) = true;

	return true;
}

static zend_always_inline bool ucache_decode_node_enter(void)
{
	if (!UCACHE_DECODE_COUNTS_NESTING) {
		return true;
	}

	if (ucache_decode_nesting_overflowed(UC_G(decode_nesting))) {
		return false;
	}

	UC_G(decode_nesting)++;

	return true;
}

static zend_always_inline bool ucache_decode_node_leave(bool result)
{
	if (UCACHE_DECODE_COUNTS_NESTING) {
		ZEND_ASSERT(UC_G(decode_nesting) != 0);

		UC_G(decode_nesting)--;
	}

	return result;
}

static zend_always_inline const void *ucache_decode_verbatim_arr_shm_addr(
		const uint8_t *buf,
		const uint8_t *snapshot_origin,
		const zend_array *arr)
{
	if (snapshot_origin == NULL) {
		return arr;
	}

	return snapshot_origin + ((const uint8_t *) arr - buf);
}

static zend_always_inline uintptr_t ucache_verdict_set_idx(const void *shm_addr)
{
	uintptr_t addr = (uintptr_t) shm_addr;

	return (addr >> UCACHE_VERBATIM_VERDICT_ADDR_SHIFT) ^ (addr >> UCACHE_VERBATIM_VERDICT_ADDR_MIX_SHIFT);
}

static zend_always_inline bool ucache_verdict_set_find(const ucache_verbatim_verdict *set, const void *shm_addr)
{
	const ucache_ctx *ctx = ucache_active_ctx();
	uint64_t gen = UC_G(decode_payload_gen);
	uint32_t way;

	for (way = 0; way < UCACHE_VERBATIM_VERDICT_WAYS; way++) {
		if (set[way].arr == shm_addr && set[way].gen == gen && set[way].ctx == ctx) {
			return true;
		}
	}

	return false;
}

static zend_always_inline void ucache_verdict_set_store(ucache_verbatim_verdict *set, uint8_t *next_way, const void *shm_addr)
{
	uint32_t way = *next_way % UCACHE_VERBATIM_VERDICT_WAYS;

	(*next_way)++;

	set[way].ctx = ucache_active_ctx();
	set[way].arr = shm_addr;
	set[way].gen = UC_G(decode_payload_gen);
}

static zend_always_inline ucache_verbatim_verdict *ucache_verbatim_verdict_set(const void *shm_arr)
{
	return &UC_G(verbatim_verdicts)[(ucache_verdict_set_idx(shm_arr) & (UCACHE_VERBATIM_VERDICT_SETS - 1)) * UCACHE_VERBATIM_VERDICT_WAYS];
}

static zend_always_inline ucache_verbatim_verdict *ucache_payload_verdict_set(const void *shm_payload)
{
	return &UC_G(payload_verdicts)[(ucache_verdict_set_idx(shm_payload) & (UCACHE_PAYLOAD_VERDICT_SETS - 1)) * UCACHE_VERBATIM_VERDICT_WAYS];
}

static zend_always_inline bool ucache_verbatim_verdict_find(const void *shm_arr)
{
	return ucache_verdict_set_find(ucache_verbatim_verdict_set(shm_arr), shm_arr);
}

static zend_always_inline void ucache_verbatim_verdict_store(const void *shm_arr)
{
	ucache_verdict_set_store(ucache_verbatim_verdict_set(shm_arr), &UC_G(verbatim_verdict_next_way), shm_arr);
}

static zend_always_inline bool ucache_decode_payload_begin(const void *shm_payload, uint64_t gen)
{
	UC_G(decode_payload_gen) = gen;
	UC_G(decode_payload_validated) = ucache_verdict_set_find(ucache_payload_verdict_set(shm_payload), shm_payload);

	return UC_G(decode_payload_validated);
}

static zend_always_inline void ucache_decode_payload_end(const void *shm_payload, bool validated, bool result)
{
	if (result && !validated) {
		ucache_verdict_set_store(ucache_payload_verdict_set(shm_payload), &UC_G(payload_verdict_next_way), shm_payload);
	}
}

static zend_always_inline const uint8_t *ucache_decode_snapshot_origin(void)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);

	return frame != NULL && frame->snapshot != NULL ? frame->snapshot->origin : NULL;
}

static zend_always_inline HashTable *ucache_decode_shape_proto_cache(void)
{
	if (UC_G(decode_shape_proto_cache) == NULL) {
		UC_G(decode_shape_proto_cache) = emalloc(sizeof(HashTable));
		zend_hash_init(
			UC_G(decode_shape_proto_cache),
			8,
			NULL,
			ucache_decode_shape_proto_dtor,
			0
		);
	}

	return UC_G(decode_shape_proto_cache);
}

static zend_always_inline bool ucache_sgraph_decode_arr_next_free(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_arr *garr,
		zend_long *next_free)
{
	int64_t wide;

	if (!(garr->flags & UCACHE_SGRAPH_ARR_FLAG_WIDE_NEXT_FREE)) {
#if SIZEOF_ZEND_LONG < 8
		if ((uint64_t) garr->next_free > (uint64_t) ZEND_LONG_MAX) {
			return false;
		}
#endif

		*next_free = (zend_long) garr->next_free;

		return true;
	}

	if (!ucache_decode_range_ok(buf_len, garr->next_free, sizeof(wide))) {
		return false;
	}

	memcpy(&wide, buf + garr->next_free, sizeof(wide));

	if (wide > (int64_t) ZEND_LONG_MAX || wide < (int64_t) ZEND_LONG_MIN) {
		return false;
	}

	*next_free = (zend_long) wide;

	return true;
}

static zend_always_inline zend_long ucache_sgraph_shape_next_free_decode(uint32_t next_free)
{
	return next_free == UCACHE_SGRAPH_SHAPE_NEXT_FREE_UNSET ? ZEND_LONG_MIN : (zend_long) next_free;
}

static zend_always_inline void ucache_decode_shape_proto_direct_cache_store(
		const ucache_sgraph_arr_shape *gshape,
		zend_array *proto)
{
	uint32_t slot;

	slot = UC_G(decode_shape_proto_direct_next)++ % UCACHE_DECODE_DIRECT_CACHE_SLOTS;

	UC_G(decode_shape_proto_direct_keys)[slot] = gshape;
	UC_G(decode_shape_proto_direct_vals)[slot] = proto;
}

static zend_always_inline zend_ulong ucache_sgraph_elem_idx(
		const ucache_sgraph_arr_elem *elem)
{
	return (zend_ulong) (((uint64_t) elem->h_hi << 32) | elem->key);
}

static zend_always_inline bool ucache_decode_arr_still_packed_fillable(const HashTable *arr, uint32_t remaining)
{
	return HT_IS_PACKED(arr) &&
		arr->nInternalPointer == 0 &&
		arr->nNumUsed == arr->nNumOfElements &&
		arr->nNextFreeElement == arr->nNumUsed &&
		remaining <= arr->nTableSize - arr->nNumUsed
	;
}

static zend_always_inline const void *ucache_verbatim_resolve_ptr(
		const ucache_verbatim_check_ctx *ctx,
		const void *ptr,
		size_t size,
		size_t alignment)
{
	uintptr_t addr = (uintptr_t) ptr, base;
	size_t offset;

	if ((addr & (alignment - 1)) != 0) {
		return NULL;
	}

	base = (uintptr_t) ctx->buf;
	if (addr >= base && addr - base <= ctx->buf_len) {
		offset = (size_t) (addr - base);

		return size <= ctx->buf_len - offset ? ptr : NULL;
	}

	if (ctx->snapshot_origin == NULL) {
		return NULL;
	}

	base = (uintptr_t) ctx->snapshot_origin;
	if (addr < base || addr - base > ctx->buf_len) {
		return NULL;
	}

	offset = (size_t) (addr - base);

	return size <= ctx->buf_len - offset ? ctx->buf + offset : NULL;
}

static zend_always_inline bool ucache_verbatim_str_check(
		const ucache_verbatim_check_ctx *ctx,
		const zend_string *str)
{
	const zend_string *hdr;

	hdr = ucache_verbatim_resolve_ptr(ctx, str, _ZSTR_HEADER_SIZE, alignof(zend_string));
	if (hdr == NULL || !ucache_decode_str_hdr_ok(hdr) || ZSTR_LEN(hdr) > ctx->buf_len) {
		return false;
	}

	return ucache_verbatim_resolve_ptr(ctx, str, _ZSTR_STRUCT_SIZE(ZSTR_LEN(hdr)), alignof(zend_string)) != NULL;
}

static zend_always_inline uint32_t ucache_verbatim_hash_slot(const zend_array *arr, zend_ulong h)
{
	uint32_t idx = (uint32_t) h | arr->nTableMask;

	return idx - arr->nTableMask;
}

static zend_always_inline bool ucache_verbatim_elem_check(
		ucache_verbatim_check_ctx *ctx,
		const zval *val)
{
	const zend_array *arr;

	switch (Z_TYPE_INFO_P(val)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_INTERNED_STRING_EX:
			return ucache_verbatim_str_check(ctx, Z_STR_P(val));
		case IS_ARRAY:
			if (Z_ARR_P(val) == &zend_empty_array) {
				return true;
			}

			arr = ucache_verbatim_resolve_ptr(ctx, Z_ARR_P(val), sizeof(zend_array), alignof(zend_array));

			return arr != NULL && ucache_verbatim_arr_check(ctx, arr);
		default:
			return false;
	}
}

static zend_always_inline bool ucache_decode_verbatim_arr_ok(
		const uint8_t *buf,
		size_t buf_len,
		const uint8_t *snapshot_origin,
		const zend_array *arr)
{
	const void *shm_arr;

	if (EXPECTED(UC_G(decode_payload_validated))) {
		return true;
	}

	shm_arr = ucache_decode_verbatim_arr_shm_addr(buf, snapshot_origin, arr);
	if (ucache_verbatim_verdict_find(shm_arr)) {
		return true;
	}

	return ucache_decode_verbatim_arr_validate(buf, buf_len, snapshot_origin, arr, shm_arr);
}

static zend_always_inline bool ucache_sgraph_decode_simple_val(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	zend_string *str;
	int64_t wide;
	double dval;

	switch (gval->type) {
		case UCACHE_SGRAPH_VAL_UNDEF:
			return false;
		case UCACHE_SGRAPH_VAL_NULL:
			ZVAL_NULL(dst);

			return true;
		case UCACHE_SGRAPH_VAL_TRUE:
			ZVAL_TRUE(dst);

			return true;
		case UCACHE_SGRAPH_VAL_FALSE:
			ZVAL_FALSE(dst);

			return true;
		case UCACHE_SGRAPH_VAL_LONG:
			ZVAL_LONG(dst, gval->long_val);

			return true;
		case UCACHE_SGRAPH_VAL_LONG_WIDE:
			if (!ucache_decode_range_ok(buf_len, gval->offset, sizeof(wide))) {
				return false;
			}

			memcpy(&wide, buf + gval->offset, sizeof(wide));
#if SIZEOF_ZEND_LONG < 8
			if (wide > (int64_t) ZEND_LONG_MAX || wide < (int64_t) ZEND_LONG_MIN) {
				return false;
			}
#endif

			ZVAL_LONG(dst, (zend_long) wide);

			return true;
		case UCACHE_SGRAPH_VAL_DOUBLE:
			if (!ucache_decode_range_ok(buf_len, gval->offset, sizeof(dval))) {
				return false;
			}

			memcpy(&dval, buf + gval->offset, sizeof(dval));

			ZVAL_DOUBLE(dst, dval);

			return true;
		case UCACHE_SGRAPH_VAL_STR:
			str = ucache_decode_str_at(buf, buf_len, gval->offset);
			if (str == NULL) {
				return false;
			}

			if (UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
				ZVAL_STR_COPY(dst, str);
			} else {
				ZVAL_INTERNED_STR(dst, str);
			}

			return true;
		default:
			return false;
	}
}

static inline zend_object *ucache_enum_case_find(zend_class_entry *ce, zend_string *name)
{
	zend_class_constant *c;

	c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), name);
	if (c == NULL || !(ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE)) {
		return NULL;
	}

	if (Z_TYPE(c->value) == IS_CONSTANT_AST &&
		zval_update_constant_ex(&c->value, c->ce) == FAILURE
	) {
		return NULL;
	}

	if (Z_TYPE(c->value) != IS_OBJECT) {
		return NULL;
	}

	return Z_OBJ(c->value);
}

static void ucache_owned_str_dtor(zval *val)
{
	zend_string_release((zend_string *) Z_PTR_P(val));
}

static ZEND_COLD zend_never_inline void ucache_decode_release_keeping_overflow(zval *dst)
{
	bool overflowed = UC_G(stack_overflowed);

	if (Z_TYPE_P(dst) == IS_OBJECT) {
		GC_ADD_FLAGS(Z_OBJ_P(dst), IS_OBJ_DESTRUCTOR_CALLED);
	}

	zval_ptr_dtor(dst);

	ZVAL_UNDEF(dst);

	UC_G(stack_overflowed) = overflowed;
}

static ucache_restore_queue *ucache_restore_queue_push(void)
{
	ucache_restore_queue *queue = ecalloc(1, sizeof(*queue));

	queue->prev = UC_G(decode_restore_queue);
	queue->depth = UC_G(decode_depth);

	UC_G(decode_restore_queue) = queue;

	return queue;
}

static bool ucache_defer_restore(zval *obj, zval *state)
{
	ucache_restore_queue *queue = UC_G(decode_restore_queue);
	ucache_restore_call *call;
	uint32_t capacity;

	if (queue == NULL || queue->depth != UC_G(decode_depth)) {
		if (UNEXPECTED(UC_G(decode_depth) == 0)) {
			return false;
		}

		queue = ucache_restore_queue_push();
	}

	if (queue->count == queue->capacity) {
		if (queue->capacity > UINT32_MAX / 2) {
			return false;
		}

		capacity = queue->capacity == 0 ? 8 : queue->capacity * 2;

		queue->calls = safe_erealloc(queue->calls, capacity, sizeof(*queue->calls), 0);
		queue->capacity = capacity;
	}

	call = &queue->calls[queue->count++];
	ZVAL_COPY(&call->obj, obj);

	UC_G(restore_hook_calls)++;

	if (state != NULL) {
		ZVAL_COPY(&call->state, state);
	} else {
		ZVAL_UNDEF(&call->state);
	}

	return true;
}

static zend_never_inline bool ucache_restore_queue_finish_owned(
		ucache_restore_queue *queue,
		bool result)
{
	return ucache_restore_queue_finish_impl(queue, result, true);
}

static bool ucache_restore_queue_finish(ucache_restore_queue *queue, bool result)
{
	if (UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
		return ucache_restore_queue_finish_owned(queue, result);
	}

	return ucache_restore_queue_finish_impl(queue, result, false);
}

static void ucache_restore_queue_destroy(ucache_restore_queue *queue)
{
	if (queue->calls != NULL) {
		efree(queue->calls);
	}

	efree(queue);
}

static bool ucache_sgraph_mangled_name_is_well_formed(const zend_string *name)
{
	size_t class_name_len;

	if (ZSTR_LEN(name) < 3 || ZSTR_VAL(name)[1] == '\0') {
		return false;
	}

	class_name_len = zend_strnlen(ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 2);

	return class_name_len < ZSTR_LEN(name) - 2 && ZSTR_VAL(name)[class_name_len + 1] == '\0';
}

static PHP_UCACHE_HOT bool ucache_sgraph_try_update_declared_prop(
		zend_object *obj,
		zend_string *prop_name,
		zend_property_info *prop_info,
		zval *prop_val,
		bool *failed)
{
	zval *slot, tmp, indirect;

	*failed = false;

	if (prop_info == NULL ||
		prop_info == ZEND_WRONG_PROPERTY_INFO ||
		!zend_string_equals(prop_info->name, prop_name) ||
		(prop_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) != 0 ||
		prop_info->offset == ZEND_VIRTUAL_PROPERTY_OFFSET
	) {
		return false;
	}

	slot = OBJ_PROP(obj, prop_info->offset);

	if (Z_ISREF_P(prop_val)) {
		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			if (!zend_verify_prop_assignable_by_ref(prop_info, prop_val, true)) {
				*failed = true;

				return false;
			}

			ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(prop_val), prop_info);
		}

		ZVAL_COPY(&tmp, prop_val);
	} else {
		if (UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY) &&
			!Z_ISUNDEF_P(slot) &&
			!(Z_PROP_FLAG_P(slot) & IS_PROP_REINITABLE)
		) {
			zend_readonly_property_modification_error(prop_info);

			*failed = true;

			return false;
		}

		ZVAL_COPY(&tmp, prop_val);

		if (ZEND_TYPE_IS_SET(prop_info->type) &&
			!zend_verify_property_type(prop_info, &tmp, true)
		) {
			zval_ptr_dtor(&tmp);

			*failed = true;

			return false;
		}
	}

	if (UNEXPECTED(Z_ISREF_P(slot)) && ZEND_TYPE_IS_SET(prop_info->type)) {
		ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(slot), prop_info);
	}

	zval_ptr_dtor(slot);

	ZVAL_COPY_VALUE(slot, &tmp);

	if (obj->properties != NULL) {
		ZVAL_INDIRECT(&indirect, slot);
		zend_hash_update(obj->properties, prop_name, &indirect);
	}

	return true;
}

static zend_property_info *ucache_sgraph_renamed_prop_info(zend_class_entry *ce, zend_string *prop_name)
{
	const char *class_name = NULL, *unmangled_name;
	size_t unmangled_name_len;

	if (zend_hash_num_elements(&ce->properties_info) == 0 ||
		zend_unmangle_property_name_ex(prop_name, &class_name, &unmangled_name, &unmangled_name_len) != SUCCESS
	) {
		return NULL;
	}

	if (class_name != NULL && strcmp(class_name, "*") != 0 && strcasecmp(class_name, ZSTR_VAL(ce->name)) != 0) {
		return NULL;
	}

	return zend_hash_str_find_ptr(&ce->properties_info, unmangled_name, unmangled_name_len);
}

static bool ucache_sgraph_insert_prop(zend_object *obj, zend_string *prop_name, zval *prop_val)
{
	Z_TRY_ADDREF_P(prop_val);

	zend_hash_update(zend_std_get_properties_ex(obj), prop_name, prop_val);

	return true;
}

static bool ucache_sgraph_add_dynamic_prop(zend_object *obj, zend_string *prop_name, zval *prop_val)
{
	if (UNEXPECTED(obj->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
		zend_throw_error(
			NULL,
			"Cannot create dynamic property %s::$%s",
			ZSTR_VAL(obj->ce->name),
			zend_get_unmangled_property_name(prop_name)
		);

		return false;
	}

	if (!(obj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
		zend_error(
			E_DEPRECATED,
			"Creation of dynamic property %s::$%s is deprecated",
			ZSTR_VAL(obj->ce->name),
			zend_get_unmangled_property_name(prop_name)
		);

		if (EG(exception)) {
			return false;
		}
	}

	return ucache_sgraph_insert_prop(obj, prop_name, prop_val);
}

static PHP_UCACHE_HOT uintptr_t ucache_decode_lookup_class(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t class_name_offset)
{
	const void *key;
	zend_string *class_name;
	zend_class_entry *ce;
	uintptr_t resolved;

	if (UNEXPECTED(class_name_offset >= buf_len)) {
		return 0;
	}

	key = buf + class_name_offset;
	resolved = ucache_decode_resolve_cache_find(key, UCACHE_DECODE_RESOLVE_KIND_CLASS);
	if (resolved != 0) {
		return resolved;
	}

	class_name = ucache_decode_str_at(buf, buf_len, class_name_offset);
	if (class_name == NULL) {
		return 0;
	}

	BG(serialize_lock)++;
	ce = zend_lookup_class(class_name);
	BG(serialize_lock)--;

	if (!ucache_owned_decode_validate_proc() || ce == NULL) {
		return 0;
	}

	resolved = ucache_decode_resolved_class(ce);
	ucache_decode_resolve_cache_store(key, resolved);

	return resolved;
}

static PHP_UCACHE_HOT uintptr_t ucache_decode_lookup_state_schema_class(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_state_schema *state_schema)
{
	uintptr_t resolved;

	resolved = ucache_decode_resolve_cache_find(state_schema, UCACHE_DECODE_RESOLVE_KIND_CLASS);
	if (resolved != 0) {
		return resolved;
	}

	resolved = ucache_decode_lookup_class(buf, buf_len, state_schema->class_name_offset);
	if (resolved != 0) {
		ucache_decode_resolve_cache_store(state_schema, resolved);
	}

	return resolved;
}

static void ucache_owned_acyclic_verbatim_arr_dtor(zval *zv)
{
	zend_array_release((zend_array *) Z_PTR_P(zv));
}

static void ucache_decode_shape_proto_dtor(zval *zv)
{
	zend_array_destroy((zend_array *) Z_PTR_P(zv));
}

static zend_array *ucache_decode_shape_proto_create(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_arr_shape *gshape)
{
	const ucache_sgraph_arr_shape_elem *shape_elems, *shape_elem;
	zend_string *prop_name;
	zend_array *proto;
	zval empty;
	uint32_t i, count;

	count = gshape->count;
	if (count == 0 ||
		count > UCACHE_SGRAPH_ARR_SHAPE_MAX_KEYS ||
		!ucache_decode_arr_range_ok(buf_len, gshape->elems_offset, count, sizeof(*shape_elems))
	) {
		return NULL;
	}

	proto = zend_new_array(count);
	if (HT_FLAGS(proto) & HASH_FLAG_UNINITIALIZED) {
		zend_hash_real_init_mixed(proto);
	}

	shape_elems =
		(const ucache_sgraph_arr_shape_elem *) (buf + gshape->elems_offset)
	;

	ZVAL_LONG(&empty, 0);

	for (i = 0; i < count; i++) {
		shape_elem = &shape_elems[i];
		prop_name = ucache_decode_str_at(buf, buf_len, shape_elem->key_offset);
		if (shape_elem->key_offset == 0 || prop_name == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}

		if (_zend_hash_append_ex(proto, prop_name, &empty, UC_G(owned_decode_frame) == NULL) == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}
	}

	proto->nNextFreeElement = 0;

	return proto;
}

static zend_array *ucache_decode_shape_proto_borrow(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_arr_shape *gshape)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);
	zend_ulong key;
	zend_array *proto;
	HashTable *cache;
	uint32_t i;

	if (UNEXPECTED(frame != NULL)) {
		key = (zend_ulong) (uintptr_t) gshape;
		if (frame->shapes != NULL) {
			proto = zend_hash_index_find_ptr(frame->shapes, key);
			if (proto != NULL) {
				return proto;
			}
		} else {
			frame->shapes = emalloc(sizeof(HashTable));

			zend_hash_init(frame->shapes, 8, NULL, ucache_decode_shape_proto_dtor, 0);
		}

		proto = ucache_decode_shape_proto_create(buf, buf_len, gshape);
		if (proto != NULL) {
			zend_hash_index_add_new_ptr(frame->shapes, key, proto);
		}

		return proto;
	}

	for (i = 0; i < UCACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
		if (UC_G(decode_shape_proto_direct_keys)[i] == gshape) {
			return UC_G(decode_shape_proto_direct_vals)[i];
		}
	}

	key = (zend_ulong) (uintptr_t) gshape;
	if (UC_G(decode_shape_proto_cache) != NULL) {
		proto = zend_hash_index_find_ptr(UC_G(decode_shape_proto_cache), key);
		if (proto != NULL) {
			ucache_decode_shape_proto_direct_cache_store(gshape, proto);

			return proto;
		}
	}

	proto = ucache_decode_shape_proto_create(buf, buf_len, gshape);
	if (proto == NULL) {
		return NULL;
	}

	cache = ucache_decode_shape_proto_cache();
	if (zend_hash_num_elements(cache) >= UCACHE_DECODE_CACHE_MAX_ENTRIES) {
		zend_hash_clean(cache);

		memset((void *) UC_G(decode_shape_proto_direct_keys), 0, sizeof(UC_G(decode_shape_proto_direct_keys)));
		memset(UC_G(decode_shape_proto_direct_vals), 0, sizeof(UC_G(decode_shape_proto_direct_vals)));
	}

	if (zend_hash_index_add_ptr(cache, key, proto) == NULL) {
		zend_array_destroy(proto);

		return NULL;
	}

	ucache_decode_shape_proto_direct_cache_store(gshape, proto);

	return proto;
}

static PHP_UCACHE_HOT bool ucache_decode_shape_proto_clone(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_arr_shape *gshape,
		uint32_t count,
		uint32_t next_free,
		zval *dst)
{
	zend_array *proto, *arr;

	if (gshape->count != count) {
		return false;
	}

	proto = ucache_decode_shape_proto_borrow(buf, buf_len, gshape);
	if (proto == NULL || proto->nNumUsed != count || proto->nNumOfElements != count) {
		return false;
	}

	if (UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
		arr = zend_array_dup(proto);
		arr->nInternalPointer = 0;
		arr->nNextFreeElement = ucache_sgraph_shape_next_free_decode(next_free);

		ZVAL_ARR(dst, arr);

		return true;
	}

	ALLOC_HASHTABLE(arr);

	GC_SET_REFCOUNT(arr, 1);
	GC_TYPE_INFO(arr) = GC_ARRAY;

	HT_FLAGS(arr) = HT_FLAGS(proto) & HASH_FLAG_MASK;

	arr->nTableMask = proto->nTableMask;
	arr->nNumUsed = proto->nNumUsed;
	arr->nNumOfElements = proto->nNumOfElements;
	arr->nTableSize = proto->nTableSize;
	arr->nInternalPointer = 0;
	arr->nNextFreeElement = ucache_sgraph_shape_next_free_decode(next_free);
	arr->pDestructor = ZVAL_PTR_DTOR;

	HT_SET_DATA_ADDR(arr, emalloc(HT_SIZE(arr)));

	ZEND_ASSERT(HT_HAS_STATIC_KEYS_ONLY(proto));

	memcpy(HT_GET_DATA_ADDR(arr), HT_GET_DATA_ADDR(proto), HT_USED_SIZE(proto));

	ZVAL_ARR(dst, arr);

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_shaped_arr(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t shape_offset,
		uint32_t vals_offset,
		uint32_t count,
		uint32_t next_free,
		zval *dst)
{
	const ucache_sgraph_arr_shape *gshape;
	const ucache_sgraph_val *gvals;
	zval val;
	Bucket *bucket;
	uint32_t i;

	if (!ucache_decode_range_ok(buf_len, shape_offset, sizeof(*gshape)) ||
		!ucache_decode_arr_range_ok(buf_len, vals_offset, count, sizeof(*gvals))
	) {
		return false;
	}

	gshape = (const ucache_sgraph_arr_shape *) (buf + shape_offset);
	if (!ucache_decode_shape_proto_clone(
			buf,
			buf_len,
			gshape,
			count,
			next_free,
			dst
		)
	) {
		return false;
	}

	HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	gvals = (const ucache_sgraph_val *) (buf + vals_offset);
	bucket = Z_ARRVAL_P(dst)->arData;

	for (i = 0; i < count; i++) {
		if (UNEXPECTED(gvals[i].type == UCACHE_SGRAPH_VAL_UNDEF)) {
			return ucache_decode_fail_zval(dst);
		}

		if (ucache_sgraph_decode_simple_val(buf, buf_len, &gvals[i], &bucket[i].val)) {
			continue;
		}

		ZVAL_UNDEF(&val);

		if (!ucache_sgraph_decode_val(buf, buf_len, &gvals[i], &val)) {
			return ucache_decode_fail_zval(dst);
		}

		ZVAL_COPY_VALUE(&bucket[i].val, &val);
	}

	UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static zend_never_inline bool ucache_sgraph_decode_packed_vals(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_arr *garr,
		zval *dst)
{
	const ucache_sgraph_val *gvals, *gval;
	zend_long next_free;
	zval val;
	uint32_t i, count;

	count = garr->count;
	if (!ucache_decode_arr_range_ok(
			buf_len,
			garr->elems_offset,
			count,
			sizeof(*gvals)
		)
	) {
		return false;
	}

	array_init_size(dst, count);

	if (count > 0) {
		zend_hash_real_init_packed(Z_ARRVAL_P(dst));

		HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	}

	gvals = (const ucache_sgraph_val *) (buf + garr->elems_offset);

	for (i = 0; i < count; i++) {
		gval = &gvals[i];

		if (gval->type > UCACHE_SGRAPH_VAL_UNDEF &&
			gval->type <= UCACHE_SGRAPH_VAL_STR &&
			ucache_decode_arr_still_packed_fillable(Z_ARRVAL_P(dst), count - i)
		) {
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(dst)) {
				while (i < count) {
					gval = &gvals[i];
					if (gval->type == UCACHE_SGRAPH_VAL_UNDEF ||
						gval->type > UCACHE_SGRAPH_VAL_STR
					) {
						break;
					}

					if (!ucache_sgraph_decode_simple_val(buf, buf_len, gval, &val)) {
						ZEND_HASH_FILL_FINISH();

						return ucache_decode_fail_zval(dst);
					}

					ZEND_HASH_FILL_ADD(&val);

					i++;
				}
			} ZEND_HASH_FILL_END();

			if (i == count) {
				break;
			}

			gval = &gvals[i];
		}

		if (!ucache_sgraph_decode_simple_val(buf, buf_len, gval, &val)) {
			ZVAL_UNDEF(&val);

			if (!ucache_sgraph_decode_val(buf, buf_len, gval, &val)) {
				return ucache_decode_fail_zval(dst);
			}
		}

		if (zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &val) == NULL) {
			zval_ptr_dtor(&val);

			return ucache_decode_fail_zval(dst);
		}
	}

	if (!ucache_sgraph_decode_arr_next_free(buf, buf_len, garr, &next_free)) {
		return ucache_decode_fail_zval(dst);
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = next_free;

	UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_dynamic_arr(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_arr *garr;
	const ucache_sgraph_arr_elem *gelems, *gelem;
	zend_long next_free;
	zend_string *key;
	zval val;
	uint32_t i, count;

	if (!ucache_decode_range_ok(buf_len, gval->offset, sizeof(*garr))) {
		return false;
	}

	garr = (const ucache_sgraph_arr *) (buf + gval->offset);
	if (garr->flags & UCACHE_SGRAPH_ARR_FLAG_PACKED_VALS) {
		if (!(garr->flags & UCACHE_SGRAPH_ARR_FLAG_PACKED)) {
			return false;
		}

		return ucache_sgraph_decode_packed_vals(buf, buf_len, garr, dst);
	}

	count = garr->count;
	if (!ucache_decode_arr_range_ok(
			buf_len,
			garr->elems_offset,
			count,
			sizeof(*gelems)
		)
	) {
		return false;
	}

	array_init_size(dst, count);

	if (count > 0) {
		if (garr->flags & UCACHE_SGRAPH_ARR_FLAG_PACKED) {
			zend_hash_real_init_packed(Z_ARRVAL_P(dst));
		} else {
			zend_hash_real_init_mixed(Z_ARRVAL_P(dst));
		}

		HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	}

	gelems = (const ucache_sgraph_arr_elem *) (buf + garr->elems_offset);

	if (garr->flags & UCACHE_SGRAPH_ARR_FLAG_PACKED) {
		for (i = 0; i < count; i++) {
			gelem = &gelems[i];

			if (gelem->val.type > UCACHE_SGRAPH_VAL_UNDEF &&
				gelem->val.type <= UCACHE_SGRAPH_VAL_STR &&
				ucache_decode_arr_still_packed_fillable(Z_ARRVAL_P(dst), count - i)
			) {
				ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(dst)) {
					while (i < count) {
						gelem = &gelems[i];

						if (gelem->val.type == UCACHE_SGRAPH_VAL_UNDEF ||
							gelem->val.type > UCACHE_SGRAPH_VAL_STR
						) {
							break;
						}

						if ((gelem->val.flags & UCACHE_SGRAPH_ELEM_STR_KEY) != 0 ||
							ucache_sgraph_elem_idx(gelem) != i ||
							!ucache_sgraph_decode_simple_val(
								buf,
								buf_len,
								&gelem->val,
								&val
							)
						) {
							ZEND_HASH_FILL_FINISH();

							return ucache_decode_fail_zval(dst);
						}

						ZEND_HASH_FILL_ADD(&val);

						i++;
					}
				} ZEND_HASH_FILL_END();

				if (i == count) {
					break;
				}

				gelem = &gelems[i];
			}

			if ((gelem->val.flags & UCACHE_SGRAPH_ELEM_STR_KEY) != 0 ||
				ucache_sgraph_elem_idx(gelem) != i
			) {
				return ucache_decode_fail_zval(dst);
			}

			if (!ucache_sgraph_decode_simple_val(buf, buf_len, &gelem->val, &val)) {
				ZVAL_UNDEF(&val);

				if (!ucache_sgraph_decode_val(buf, buf_len, &gelem->val, &val)) {
					return ucache_decode_fail_zval(dst);
				}
			}

			if (zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &val) == NULL) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}
		}

		if (!ucache_sgraph_decode_arr_next_free(buf, buf_len, garr, &next_free)) {
			return ucache_decode_fail_zval(dst);
		}

		Z_ARRVAL_P(dst)->nNextFreeElement = next_free;

		UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

		return true;
	}

	for (i = 0; i < count; i++) {
		gelem = &gelems[i];

		if (!ucache_sgraph_decode_simple_val(buf, buf_len, &gelem->val, &val)) {
			ZVAL_UNDEF(&val);

			if (!ucache_sgraph_decode_val(buf, buf_len, &gelem->val, &val)) {
				return ucache_decode_fail_zval(dst);
			}
		}

		if (gelem->val.flags & UCACHE_SGRAPH_ELEM_STR_KEY) {
			key = ucache_decode_str_at(buf, buf_len, gelem->key);
			if (key == NULL) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}

			_zend_hash_append(Z_ARRVAL_P(dst), key, &val);
		} else {
			zend_hash_index_add_new(Z_ARRVAL_P(dst), ucache_sgraph_elem_idx(gelem), &val);
		}
	}

	if (!ucache_sgraph_decode_arr_next_free(buf, buf_len, garr, &next_free)) {
		return ucache_decode_fail_zval(dst);
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = next_free;

	UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_init_obj(
		zend_class_entry *ce,
		uint32_t node_offset,
		bool shared,
		zval *dst)
{
	if ((ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 || object_init_ex(dst, ce) != SUCCESS) {
		return false;
	}

	if (!ucache_owned_decode_validate_proc()) {
		return ucache_decode_fail_zval(dst);
	}

	if (shared && !ucache_decode_identity_map_insert(node_offset, Z_OBJ_P(dst))) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_update_obj_prop(
		zval *obj_zv,
		zend_string *prop_name,
		zval *prop_val)
{
	const char *unmangled_class, *unmangled_prop;
	zend_object *obj;
	zend_property_info *prop_info;
	size_t unmangled_prop_len;
	bool failed;

	obj = Z_OBJ_P(obj_zv);

	if (ZSTR_LEN(prop_name) != 0 && ZSTR_VAL(prop_name)[0] == '\0' &&
		zend_hash_num_elements(&obj->ce->properties_info) != 0 &&
		zend_unmangle_property_name_ex(prop_name, &unmangled_class, &unmangled_prop, &unmangled_prop_len) != SUCCESS
	) {
		return false;
	}

	prop_info = ucache_sgraph_declared_prop_info(obj->ce, prop_name);
	if (ucache_sgraph_try_update_declared_prop(
			obj,
			prop_name,
			prop_info,
			prop_val,
			&failed
		)
	) {
		return true;
	}

	if (failed) {
		return false;
	}

	prop_info = ucache_sgraph_renamed_prop_info(obj->ce, prop_name);
	if (prop_info != NULL) {
		if (prop_info->flags & ZEND_ACC_VIRTUAL) {
			php_error_docref(
				NULL,
				E_WARNING,
				"Cannot unserialize value for virtual property %s::$%s",
				ZSTR_VAL(prop_info->ce->name),
				zend_get_unmangled_property_name(prop_name)
			);

			return false;
		}

		if (ucache_sgraph_try_update_declared_prop(
				obj,
				prop_info->name,
				prop_info,
				prop_val,
				&failed
			)
		) {
			return true;
		}

		if (failed) {
			return false;
		}

		return ucache_sgraph_insert_prop(obj, prop_info->name, prop_val);
	}

	return ucache_sgraph_add_dynamic_prop(obj, prop_name, prop_val);
}

static PHP_UCACHE_HOT bool ucache_sgraph_update_obj_prop_at(
		zval *obj_zv,
		zend_string *prop_name,
		uint32_t prop_idx,
		zval *prop_val)
{
	zend_object *obj;
	bool failed;

	obj = Z_OBJ_P(obj_zv);
	if (obj->ce->type == ZEND_USER_CLASS &&
		obj->ce->properties_info_table != NULL &&
		prop_idx < obj->ce->default_properties_count
	) {
		if (ucache_sgraph_try_update_declared_prop(
				obj,
				prop_name,
				obj->ce->properties_info_table[prop_idx],
				prop_val,
				&failed
			)
		) {
			return true;
		}

		if (failed) {
			return false;
		}
	}

	return ucache_sgraph_update_obj_prop(
		obj_zv,
		prop_name,
		prop_val
	);
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_apply_prop(
		zval *dst,
		zend_string *prop_name,
		uint32_t slot_idx_plus_one,
		zval *prop_val)
{
	bool result;

	if (slot_idx_plus_one != 0) {
		result = ucache_sgraph_update_obj_prop_at(
			dst,
			prop_name,
			slot_idx_plus_one - 1,
			prop_val
		);
	} else {
		result = ucache_sgraph_update_obj_prop(
			dst,
			prop_name,
			prop_val
		);
	}

	if (!result) {
		zval_ptr_dtor(prop_val);

		return ucache_decode_fail_zval(dst);
	}

	zval_ptr_dtor_nogc(prop_val);

	if (!ucache_owned_decode_validate_proc()) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_obj_props(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t props_offset,
		uint32_t prop_count,
		bool use_sleep_slots,
		zval *dst)
{
	const ucache_sgraph_prop *props, *prop;
	zend_string *prop_name;
	zval prop_val;
	uint32_t i;

	if (prop_count == 0) {
		return true;
	}

	if (!ucache_decode_arr_range_ok(buf_len, props_offset, prop_count, sizeof(*props))) {
		return ucache_decode_fail_zval(dst);
	}

	props = (const ucache_sgraph_prop *) (buf + props_offset);
	for (i = 0; i < prop_count; i++) {
		prop = &props[i];
		if (prop->val.type == UCACHE_SGRAPH_VAL_UNDEF) {
			continue;
		}

		prop_name = ucache_decode_str_at(buf, buf_len, prop->name_offset);
		if (prop_name == NULL) {
			return ucache_decode_fail_zval(dst);
		}

		if (!ucache_sgraph_decode_simple_val(buf, buf_len, &prop->val, &prop_val)) {
			ZVAL_UNDEF(&prop_val);

			if (!ucache_sgraph_decode_val(buf, buf_len, &prop->val, &prop_val)) {
				zval_ptr_dtor(&prop_val);

				return ucache_decode_fail_zval(dst);
			}
		}

		if (!ucache_sgraph_decode_apply_prop(
				dst,
				prop_name,
				use_sleep_slots ? prop->val.sleep_slot_plus_one : i + 1,
				&prop_val
			)
		) {
			return false;
		}
	}

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_call_wakeup(zval *dst, bool has_wakeup)
{
	UCACHE_ASSERT_RESOLVED_WAKEUP(dst, has_wakeup);

	if (has_wakeup && !ucache_defer_restore(dst, NULL)) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_call_unserialize(zval *dst, zval *state)
{
	bool result = ucache_defer_restore(dst, state);

	zval_ptr_dtor(state);

	if (!result) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static bool ucache_sgraph_decode_obj_state(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t props_offset,
		uint32_t prop_count,
		zval *state)
{
	const ucache_sgraph_prop *props, *prop;
	zend_string *prop_name;
	zval prop_val;
	uint32_t i;

	if (prop_count == 0) {
		array_init(state);

		return true;
	}

	if (!ucache_decode_arr_range_ok(buf_len, props_offset, prop_count, sizeof(*props))) {
		ZVAL_EMPTY_ARRAY(state);

		return false;
	}

	array_init_size(state, prop_count);

	props = (const ucache_sgraph_prop *) (buf + props_offset);
	for (i = 0; i < prop_count; i++) {
		prop = &props[i];
		if (prop->val.type == UCACHE_SGRAPH_VAL_UNDEF) {
			continue;
		}

		prop_name = ucache_decode_str_at(buf, buf_len, prop->name_offset);
		if (prop_name == NULL) {
			return false;
		}

		ZVAL_UNDEF(&prop_val);

		if (!ucache_sgraph_decode_val(buf, buf_len, &prop->val, &prop_val)) {
			zval_ptr_dtor(&prop_val);

			return false;
		}

		zend_symtable_update(Z_ARRVAL_P(state), prop_name, &prop_val);
	}

	return true;
}

static zend_never_inline bool ucache_sgraph_decode_obj_with_unserialize(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		const ucache_sgraph_obj *gobj,
		zend_class_entry *ce,
		zval *dst)
{
	zval state;

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gobj->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!ucache_sgraph_decode_obj_state(
			buf,
			buf_len,
			gobj->props_offset,
			gobj->prop_count,
			&state
		)
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	return ucache_sgraph_decode_call_unserialize(dst, &state);
}

static bool ucache_sgraph_state_has_numeric_str_key(const HashTable *state)
{
	zend_string *key;
	zend_ulong idx;

	ZEND_HASH_FOREACH_STR_KEY(state, key) {
		if (key != NULL && ZEND_HANDLE_NUMERIC(key, idx)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static void ucache_sgraph_state_to_symtable(zval *state)
{
	HashTable *symtable;
	zend_string *key;
	zend_ulong idx;
	zval *val;

	if (!ucache_sgraph_state_has_numeric_str_key(Z_ARRVAL_P(state))) {
		return;
	}

	symtable = zend_new_array(zend_hash_num_elements(Z_ARRVAL_P(state)));

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(state), idx, key, val) {
		Z_TRY_ADDREF_P(val);

		if (key != NULL) {
			zend_symtable_update(symtable, key, val);
		} else {
			zend_hash_index_update(symtable, idx, val);
		}
	} ZEND_HASH_FOREACH_END();

	zval_ptr_dtor(state);

	ZVAL_ARR(state, symtable);
}

static zend_never_inline bool ucache_sgraph_decode_sleep_shaped_obj_with_unserialize(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		const ucache_sgraph_shaped_state_obj *gsstate,
		const ucache_sgraph_state_schema *state_schema,
		zend_class_entry *ce,
		zval *dst)
{
	zval state;

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gsstate->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!ucache_sgraph_decode_shaped_arr(
			buf,
			buf_len,
			state_schema->shape_offset,
			gsstate->state_vals_offset,
			state_schema->count,
			gsstate->state_next_free,
			&state
		) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	ucache_sgraph_state_to_symtable(&state);

	return ucache_sgraph_decode_call_unserialize(dst, &state);
}

static zend_never_inline bool ucache_sgraph_decode_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		bool sleep_obj,
		zval *dst)
{
	const ucache_sgraph_obj *gobj;
	zend_class_entry *ce;
	uintptr_t resolved;

	gobj = (const ucache_sgraph_obj *) (buf + gval->offset);
	resolved = ucache_decode_lookup_class(buf, buf_len, gobj->class_name_offset);
	ce = ucache_decode_resolved_class_entry(resolved);
	if (ce == NULL) {
		return false;
	}

	if (UNEXPECTED(ce->__unserialize != NULL)) {
		return ucache_sgraph_decode_obj_with_unserialize(buf, buf_len, gval, gobj, ce, dst);
	}

	if (!ucache_sgraph_can_restore_props(ce, sleep_obj)) {
		return false;
	}

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gobj->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!ucache_sgraph_decode_obj_props(
			buf,
			buf_len,
			gobj->props_offset,
			gobj->prop_count,
			sleep_obj,
			dst
		)
	) {
		return false;
	}

	return ucache_sgraph_decode_call_wakeup(dst, ucache_decode_resolved_class_has_wakeup(resolved));
}

static zend_never_inline bool ucache_sgraph_decode_sleep_shaped_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_shaped_state_obj *gsstate;
	const ucache_sgraph_state_schema *state_schema;
	const ucache_sgraph_arr_shape *gshape;
	const ucache_sgraph_arr_shape_elem *shape_elems;
	const ucache_sgraph_val *gvals;
	zend_string *prop_name;
	zend_class_entry *ce;
	zval prop_val;
	uint32_t i;
	uintptr_t resolved;

	gsstate = (const ucache_sgraph_shaped_state_obj *) (buf + gval->offset);
	if (!ucache_decode_range_ok(buf_len, gsstate->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const ucache_sgraph_state_schema *) (buf + gsstate->state_schema_offset);
	resolved = ucache_decode_lookup_state_schema_class(buf, buf_len, state_schema);
	ce = ucache_decode_resolved_class_entry(resolved);
	if (ce == NULL) {
		return false;
	}

	if (UNEXPECTED(ce->__unserialize != NULL)) {
		return ucache_sgraph_decode_sleep_shaped_obj_with_unserialize(
			buf,
			buf_len,
			gval,
			gsstate,
			state_schema,
			ce,
			dst
		);
	}

	if (!ucache_sgraph_can_restore_props(ce, true)) {
		return false;
	}

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gsstate->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!ucache_decode_range_ok(buf_len, state_schema->shape_offset, sizeof(*gshape))) {
		return ucache_decode_fail_zval(dst);
	}

	gshape = (const ucache_sgraph_arr_shape *) (buf + state_schema->shape_offset);
	if (gshape->count != state_schema->count) {
		return ucache_decode_fail_zval(dst);
	}

	if (!ucache_decode_arr_range_ok(buf_len, gshape->elems_offset, state_schema->count, sizeof(*shape_elems)) ||
		!ucache_decode_arr_range_ok(buf_len, gsstate->state_vals_offset, state_schema->count, sizeof(*gvals))
	) {
		return ucache_decode_fail_zval(dst);
	}

	shape_elems = (const ucache_sgraph_arr_shape_elem *) (buf + gshape->elems_offset);
	gvals = (const ucache_sgraph_val *) (buf + gsstate->state_vals_offset);

	for (i = 0; i < state_schema->count; i++) {
		prop_name = ucache_decode_str_at(buf, buf_len, shape_elems[i].key_offset);
		if (prop_name == NULL) {
			return ucache_decode_fail_zval(dst);
		}

		if (!ucache_sgraph_decode_simple_val(
				buf,
				buf_len,
				&gvals[i],
				&prop_val
			)
		) {
			ZVAL_UNDEF(&prop_val);

			if (!ucache_sgraph_decode_val(
					buf,
					buf_len,
					&gvals[i],
					&prop_val
				)
			) {
				zval_ptr_dtor(&prop_val);

				return ucache_decode_fail_zval(dst);
			}
		}

		if (!ucache_sgraph_decode_apply_prop(
				dst,
				prop_name,
				gvals[i].sleep_slot_plus_one,
				&prop_val
			)
		) {
			return false;
		}
	}

	return ucache_sgraph_decode_call_wakeup(dst, ucache_decode_resolved_class_has_wakeup(resolved));
}

static zend_never_inline bool ucache_sgraph_decode_safe_direct_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_safe_direct_obj *gsd;
	const php_ucache_safe_direct_handlers *handlers;
	zend_class_entry *ce, *base_ce = NULL;
	zval sd_state;
	bool restored;

	gsd = (const ucache_sgraph_safe_direct_obj *) (buf + gval->offset);

	ce = ucache_decode_resolved_class_entry(ucache_decode_lookup_class(buf, buf_len, gsd->class_name_offset));
	if (ce == NULL) {
		return false;
	}

	handlers = ucache_safe_direct_find_handlers(ce, &base_ce);

	if (handlers == NULL ||
		ucache_class_overrides_safe_direct_magic_serialize(ce, handlers, base_ce) ||
		!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gsd->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&sd_state);

	if (!ucache_sgraph_decode_val(buf, buf_len, &gsd->state, &sd_state) ||
		Z_TYPE(sd_state) != IS_ARRAY
	) {
		zval_ptr_dtor(&sd_state);

		return ucache_decode_fail_zval(dst);
	}

	BG(serialize_lock)++;
	restored = handlers->state_unserialize(dst, &sd_state);
	BG(serialize_lock)--;

	zval_ptr_dtor(&sd_state);

	if (!restored || !ucache_owned_decode_validate_proc()) {
		return ucache_decode_fail_zval(dst);
	}

	return ucache_sgraph_decode_obj_props(
		buf,
		buf_len,
		gsd->props_offset,
		gsd->prop_count,
		false,
		dst
	);
}

static zend_never_inline bool ucache_sgraph_decode_serialized_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_serialized_obj *gser;
	zend_class_entry *ce;
	zval state;

	gser = (const ucache_sgraph_serialized_obj *) (buf + gval->offset);

	ce = ucache_decode_resolved_class_entry(ucache_decode_lookup_class(buf, buf_len, gser->class_name_offset));
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gser->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!ucache_sgraph_decode_val(buf, buf_len, &gser->state, &state) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	return ucache_sgraph_decode_call_unserialize(dst, &state);
}

static zend_never_inline bool ucache_sgraph_decode_serialized_shaped_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_shaped_state_obj *gsstate;
	const ucache_sgraph_state_schema *state_schema;
	zend_class_entry *ce;
	zval state;

	gsstate = (const ucache_sgraph_shaped_state_obj *) (buf + gval->offset);
	if (!ucache_decode_range_ok(buf_len, gsstate->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const ucache_sgraph_state_schema *) (buf + gsstate->state_schema_offset);
	ce = ucache_decode_resolved_class_entry(ucache_decode_lookup_state_schema_class(buf, buf_len, state_schema));
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!ucache_sgraph_decode_init_obj(
			ce,
			gval->offset,
			(gsstate->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!ucache_sgraph_decode_shaped_arr(
			buf,
			buf_len,
			state_schema->shape_offset,
			gsstate->state_vals_offset,
			state_schema->count,
			gsstate->state_next_free,
			&state
		) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	return ucache_sgraph_decode_call_unserialize(dst, &state);
}

static zend_never_inline bool ucache_sgraph_decode_serdes_obj(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_serdes_obj *gserdes;

	gserdes =
		(const ucache_sgraph_serdes_obj *) (buf + gval->offset)
	;

	if (!ucache_decode_range_ok(
			buf_len,
			gval->offset + (uint32_t) sizeof(*gserdes),
			gserdes->blob_len
		)
	) {
		return false;
	}

	UC_G(restore_hook_calls)++;

	if (!ucache_serdes_decode(
			(const uint8_t *) (gserdes + 1),
			gserdes->blob_len,
			dst
		) ||
		Z_TYPE_P(dst) != IS_OBJECT
	) {
		return ucache_decode_fail_zval(dst);
	}

	if ((gserdes->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) &&
		!ucache_decode_identity_map_insert(
			gval->offset,
			Z_OBJ_P(dst)
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static zend_never_inline bool ucache_sgraph_decode_enum(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_enum *genum;
	zend_string *case_name;
	zend_class_entry *ce;
	zend_object *case_obj;

	genum = (const ucache_sgraph_enum *) (buf + gval->offset);
	case_obj = ucache_decode_resolved_enum_case_obj(
		ucache_decode_resolve_cache_find(genum, UCACHE_DECODE_RESOLVE_KIND_ENUM_CASE)
	);
	if (case_obj == NULL) {
		ce = ucache_decode_resolved_class_entry(ucache_decode_lookup_class(buf, buf_len, genum->class_name_offset));
		if (ce == NULL || !(ce->ce_flags & ZEND_ACC_ENUM)) {
			return false;
		}

		case_name = ucache_decode_str_at(buf, buf_len, genum->case_name_offset);
		if (case_name == NULL) {
			return false;
		}

		case_obj = ucache_enum_case_find(ce, case_name);
		if (case_obj == NULL || !ucache_owned_decode_validate_proc()) {
			return false;
		}

		ucache_decode_resolve_cache_store(genum, ucache_decode_resolved_enum_case(case_obj));
	}

	ZVAL_OBJ(dst, case_obj);

	GC_ADDREF(case_obj);

	return true;
}

static zend_never_inline bool ucache_sgraph_decode_ref(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_ref *gref;
	zend_reference *ref;

	gref = (const ucache_sgraph_ref *) (buf + gval->offset);

	ZVAL_NEW_EMPTY_REF(dst);
	ref = Z_REF_P(dst);

	ZVAL_UNDEF(&ref->val);

	if ((gref->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED) &&
		!ucache_decode_ref_map_insert(
			gval->offset,
			ref
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	if (!ucache_sgraph_decode_val(
			buf,
			buf_len,
			&gref->inner,
			&ref->val
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static bool ucache_verbatim_hash_chains_check(
		const zend_array *arr,
		const uint32_t *hash,
		const Bucket *buckets)
{
	uint32_t slot_count = (uint32_t) -arr->nTableMask;
	uint32_t slot, idx, bucket_idx, visited = 0;

	for (slot = 0; slot < slot_count; slot++) {
		idx = hash[slot];

		while (idx != HT_INVALID_IDX) {
			bucket_idx = HT_HASH_TO_IDX(idx);
			if (bucket_idx >= arr->nNumUsed ||
				HT_IDX_TO_HASH(bucket_idx) != idx ||
				visited == arr->nNumOfElements ||
				Z_TYPE_INFO(buckets[bucket_idx].val) == IS_UNDEF ||
				ucache_verbatim_hash_slot(arr, buckets[bucket_idx].h) != slot
			) {
				return false;
			}

			visited++;
			idx = Z_NEXT(buckets[bucket_idx].val);
		}
	}

	return visited == arr->nNumOfElements;
}

static const uint8_t *ucache_verbatim_arr_data(
		const ucache_verbatim_check_ctx *ctx,
		const zend_array *arr)
{
	uintptr_t data_addr;
	size_t hash_size, used_size;
	bool is_packed;

	if (GC_TYPE_INFO(arr) != UCACHE_VERBATIM_ARR_GC_TYPE_INFO ||
		GC_REFCOUNT(arr) < 2 ||
		(HT_FLAGS(arr) & ~UCACHE_VERBATIM_ARR_HT_FLAGS_ALLOWED) != 0 ||
		!(HT_FLAGS(arr) & HASH_FLAG_STATIC_KEYS) ||
		arr->nTableSize < HT_MIN_SIZE ||
		arr->nTableSize > HT_MAX_SIZE ||
		(arr->nTableSize & (arr->nTableSize - 1)) != 0 ||
		arr->nNumUsed > arr->nTableSize ||
		arr->nNumOfElements > arr->nNumUsed
	) {
		return NULL;
	}

	is_packed = HT_IS_PACKED(arr);
	if (arr->nTableMask != (is_packed ? HT_MIN_MASK : HT_SIZE_TO_MASK(arr->nTableSize))) {
		return NULL;
	}

	hash_size = HT_HASH_SIZE(arr->nTableMask);
	used_size = (size_t) arr->nNumUsed * (is_packed ? sizeof(zval) : sizeof(Bucket));
	data_addr = (uintptr_t) arr->arData;
	if (data_addr < hash_size) {
		return NULL;
	}

	return ucache_verbatim_resolve_ptr(
		ctx,
		(const void *) (data_addr - hash_size),
		hash_size + used_size,
		alignof(Bucket)
	);
}

static bool ucache_verbatim_arr_check(ucache_verbatim_check_ctx *ctx, const zend_array *arr)
{
	const uint8_t *data;
	const uint32_t *hash;
	const Bucket *buckets;
	const zval *packed, *seen;
	size_t hash_size;
	uint32_t i, live = 0;

	if (ucache_stack_overflowed() || ucache_decode_nesting_overflowed(ctx->depth)) {
		return false;
	}

	if (!ucache_seen_test_and_add(&ctx->seen, arr)) {
		seen = zend_hash_index_find(&ctx->seen, (zend_ulong) (uintptr_t) arr);

		return seen != NULL && Z_TYPE_P(seen) == IS_TRUE;
	}

	data = ucache_verbatim_arr_data(ctx, arr);
	if (data == NULL) {
		return false;
	}

	hash = (const uint32_t *) data;
	hash_size = HT_HASH_SIZE(arr->nTableMask);
	ctx->depth++;

	if (HT_IS_PACKED(arr)) {
		if (hash[0] != HT_INVALID_IDX || hash[1] != HT_INVALID_IDX) {
			return false;
		}

		packed = (const zval *) (data + hash_size);
		for (i = 0; i < arr->nNumUsed; i++) {
			if (!ucache_verbatim_elem_check(ctx, &packed[i])) {
				return false;
			}

			if (Z_TYPE_INFO(packed[i]) != IS_UNDEF) {
				live++;
			}
		}
	} else {
		buckets = (const Bucket *) (data + hash_size);
		for (i = 0; i < arr->nNumUsed; i++) {
			if ((buckets[i].key != NULL && !ucache_verbatim_str_check(ctx, buckets[i].key)) ||
				!ucache_verbatim_elem_check(ctx, &buckets[i].val)
			) {
				return false;
			}

			if (Z_TYPE_INFO(buckets[i].val) != IS_UNDEF) {
				live++;
			}
		}

		if (!ucache_verbatim_hash_chains_check(arr, hash, buckets)) {
			return false;
		}
	}

	if (live != arr->nNumOfElements) {
		return false;
	}

	ctx->depth--;

	ZVAL_TRUE(zend_hash_index_lookup(&ctx->seen, (zend_ulong) (uintptr_t) arr));

	return true;
}

static zend_never_inline bool ucache_decode_verbatim_arr_validate(
		const uint8_t *buf,
		size_t buf_len,
		const uint8_t *snapshot_origin,
		const zend_array *arr,
		const void *shm_arr)
{
	ucache_verbatim_check_ctx ctx;
	bool result;

	if (UCACHE_DEBUG_FAULT("VERBATIM_ARR_INVALID")) {
		return false;
	}

	ctx.buf = buf;
	ctx.snapshot_origin = snapshot_origin;
	ctx.buf_len = buf_len;
	ctx.depth = UC_G(decode_nesting);

	zend_hash_init(&ctx.seen, 8, NULL, NULL, 0);

	result = ucache_verbatim_arr_check(&ctx, arr);

	zend_hash_destroy(&ctx.seen);

	if (result) {
		ucache_verbatim_verdict_store(shm_arr);
	}

	return result;
}

static void *ucache_owned_decode_relocate(const void *ptr, size_t size)
{
	const ucache_sgraph_snapshot *snapshot = UC_G(owned_decode_frame)->snapshot;
	uintptr_t addr = (uintptr_t) ptr, base;

	if (snapshot == NULL) {
		return (void *) ptr;
	}

	base = (uintptr_t) snapshot->origin;
	if (addr >= base && addr - base <= snapshot->len && size <= snapshot->len - (addr - base)) {
		return (void *) (snapshot->data + (addr - base));
	}

	base = (uintptr_t) snapshot->data;
	if (addr >= base && addr - base <= snapshot->len && size <= snapshot->len - (addr - base)) {
		return (void *) ptr;
	}

	return NULL;
}

static zend_string *ucache_owned_decode_relocate_str(const zend_string *str)
{
	str = ucache_owned_decode_relocate(str, _ZSTR_HEADER_SIZE);
	if (str == NULL ||
		ZSTR_LEN(str) > SIZE_MAX / 2 ||
		ucache_owned_decode_relocate(str, _ZSTR_STRUCT_SIZE(ZSTR_LEN(str))) == NULL
	) {
		return NULL;
	}

	return (zend_string *) str;
}

static zend_array *ucache_owned_decode_relocate_arr(zend_array *arr)
{
	void *data;

	if (UC_G(owned_decode_frame)->snapshot == NULL) {
		return arr;
	}

	arr = ucache_owned_decode_relocate(arr, sizeof(zend_array));
	if (arr == NULL) {
		return NULL;
	}

	data = ucache_owned_decode_relocate(
		HT_GET_DATA_ADDR(arr),
		HT_IS_PACKED(arr) ? HT_PACKED_USED_SIZE(arr) : HT_USED_SIZE(arr)
	);
	if (data == NULL) {
		return NULL;
	}

	HT_SET_DATA_ADDR(arr, data);

	return arr;
}

static void ucache_owned_verbatim_arr_drop_borrowed(zend_array *arr)
{
	zval *elem;
	Bucket *bucket;
	uint32_t i;

	if (HT_IS_PACKED(arr)) {
		for (i = 0; i < arr->nNumUsed; i++) {
			elem = &arr->arPacked[i];
			if (ucache_owned_verbatim_val_is_borrowed(elem)) {
				ZVAL_NULL(elem);
			}
		}
	} else {
		HT_FLAGS(arr) &= ~HASH_FLAG_STATIC_KEYS;

		for (i = 0; i < arr->nNumUsed; i++) {
			bucket = &arr->arData[i];
			bucket->key = NULL;
			if (ucache_owned_verbatim_val_is_borrowed(&bucket->val)) {
				ZVAL_NULL(&bucket->val);
			}
		}
	}
}

static bool ucache_owned_decode_verbatim(zval *dst, const zval *src)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);
	zend_array *arr, *src_arr;
	zend_string *str;
	zend_ulong key;
	zval owned;
	Bucket *bucket;
	uint32_t i;

	if (ucache_stack_overflowed()) {
		return false;
	}

	if (Z_TYPE_P(src) == IS_STRING) {
		str = ucache_owned_decode_relocate_str(Z_STR_P(src));
		if (str == NULL) {
			return false;
		}

		ZVAL_STR_COPY(dst, ucache_owned_decode_str(str));

		return true;
	}

	if (Z_TYPE_P(src) != IS_ARRAY) {
		ZVAL_COPY_VALUE(dst, src);

		return true;
	}

	if (Z_ARR_P(src) == &zend_empty_array) {
		ZVAL_EMPTY_ARRAY(dst);

		return true;
	}

	src_arr = ucache_owned_decode_relocate_arr(Z_ARR_P(src));
	if (src_arr == NULL) {
		return false;
	}

	key = (zend_ulong) (uintptr_t) src_arr;
	if (frame->arrs != NULL) {
		arr = zend_hash_index_find_ptr(frame->arrs, key);
		if (arr != NULL) {
			GC_ADDREF(arr);

			ZVAL_ARR(dst, arr);

			return true;
		}
	} else {
		frame->arrs = emalloc(sizeof(HashTable));

		zend_hash_init(frame->arrs, 8, NULL, ucache_owned_acyclic_verbatim_arr_dtor, 0);
	}

	arr = zend_array_dup(src_arr);

	ucache_owned_verbatim_arr_drop_borrowed(arr);

	GC_ADDREF(arr);

	zend_hash_index_add_new_ptr(frame->arrs, key, arr);

	ZVAL_ARR(dst, arr);

	if (HT_IS_PACKED(arr)) {
		for (i = 0; i < arr->nNumUsed; i++) {
			if (!ucache_owned_verbatim_val_is_borrowed(&src_arr->arPacked[i])) {
				continue;
			}

			if (!ucache_owned_decode_verbatim(&owned, &src_arr->arPacked[i])) {
				return ucache_decode_fail_zval(dst);
			}

			ZVAL_COPY_VALUE(&arr->arPacked[i], &owned);
		}
	} else {
		for (i = 0; i < arr->nNumUsed; i++) {
			bucket = &arr->arData[i];
			if (Z_TYPE(bucket->val) == IS_UNDEF) {
				continue;
			}

			if (src_arr->arData[i].key != NULL) {
				str = ucache_owned_decode_relocate_str(src_arr->arData[i].key);

				if (str == NULL) {
					return ucache_decode_fail_zval(dst);
				}

				bucket->key = zend_string_copy(ucache_owned_decode_str(str));
			}

			if (!ucache_owned_verbatim_val_is_borrowed(&src_arr->arData[i].val)) {
				continue;
			}

			if (!ucache_owned_decode_verbatim(&owned, &src_arr->arData[i].val)) {
				return ucache_decode_fail_zval(dst);
			}

			ZVAL_COPY_VALUE(&bucket->val, &owned);
		}
	}

	return true;
}

static zend_always_inline bool ucache_sgraph_decode_node(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	const ucache_sgraph_shaped_arr *gsarr;
	zend_reference *shared_ref;
	zend_object *shared_obj;
	zend_array *arr;
	zval src;
	size_t node_hdr_size;

	node_hdr_size = ucache_decode_node_hdr_size(gval->type);
	if (node_hdr_size != 0 &&
		!ucache_decode_range_ok(buf_len, gval->offset, node_hdr_size)
	) {
		return false;
	}

	switch (gval->type) {
		case UCACHE_SGRAPH_VAL_UNDEF:
			return false;
		case UCACHE_SGRAPH_VAL_NULL:
		case UCACHE_SGRAPH_VAL_TRUE:
		case UCACHE_SGRAPH_VAL_FALSE:
		case UCACHE_SGRAPH_VAL_LONG:
		case UCACHE_SGRAPH_VAL_LONG_WIDE:
		case UCACHE_SGRAPH_VAL_DOUBLE:
		case UCACHE_SGRAPH_VAL_STR:
			return ucache_sgraph_decode_simple_val(buf, buf_len, gval, dst);
		case UCACHE_SGRAPH_VAL_ARR:
			if (gval->offset == 0) {
				ZVAL_EMPTY_ARRAY(dst);
			} else {
				if (!ucache_decode_range_ok(
						buf_len,
						gval->offset,
						sizeof(zend_array)
					)
				) {
					return false;
				}

				arr = (zend_array *) (void *) (buf + gval->offset);
				if (!ucache_decode_verbatim_arr_ok(buf, buf_len, ucache_decode_snapshot_origin(), arr)) {
					return false;
				}

				ZVAL_ARR(dst, arr);
				Z_TYPE_FLAGS_P(dst) = 0;
				if (UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
					ZVAL_COPY_VALUE(&src, dst);
					ZVAL_UNDEF(dst);

					return ucache_owned_decode_verbatim(dst, &src);
				}
			}

			return true;
		case UCACHE_SGRAPH_VAL_DYNAMIC_ARR:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_dynamic_arr(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_SHAPED_ARR:
			gsarr = (const ucache_sgraph_shaped_arr *) (buf + gval->offset);

			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_shaped_arr(
					buf,
					buf_len,
					gsarr->shape_offset,
					ucache_sgraph_shaped_arr_vals_offset(gval->offset),
					gsarr->count,
					gsarr->next_free,
					dst
				))
			;
		case UCACHE_SGRAPH_VAL_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_obj(buf, buf_len, gval, false, dst))
			;
		case UCACHE_SGRAPH_VAL_SLEEP_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_obj(buf, buf_len, gval, true, dst))
			;
		case UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_sleep_shaped_obj(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_OBJ_REF: {
			shared_obj = ucache_decode_identity_map_find(gval->offset);
			if (shared_obj == NULL) {
				return false;
			}

			ZVAL_OBJ(dst, shared_obj);

			GC_ADDREF(shared_obj);

			return true;
		}
		case UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_safe_direct_obj(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_SERIALIZED_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_serialized_obj(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_serialized_shaped_obj(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_SERDES_OBJ:
			return ucache_sgraph_decode_serdes_obj(buf, buf_len, gval, dst);
		case UCACHE_SGRAPH_VAL_ENUM:
			return ucache_sgraph_decode_enum(buf, buf_len, gval, dst);
		case UCACHE_SGRAPH_VAL_REF:
			return ucache_decode_node_enter() &&
				ucache_decode_node_leave(ucache_sgraph_decode_ref(buf, buf_len, gval, dst))
			;
		case UCACHE_SGRAPH_VAL_REF_REF: {
			shared_ref = ucache_decode_ref_map_find(gval->offset);

			if (shared_ref == NULL) {
				return false;
			}

			ZVAL_REF(dst, shared_ref);

			GC_ADDREF(shared_ref);

			return true;
		}
		default:
			return false;
	}
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_val(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *gval,
		zval *dst)
{
	if (ucache_stack_overflowed()) {
		return false;
	}

	return ucache_sgraph_decode_node(buf, buf_len, gval, dst);
}

static bool ucache_sgraph_load_root_val(
		const ucache_sgraph_hdr *hdr,
		size_t buf_len,
		ucache_sgraph_val *root_val)
{
	uint32_t root_offset = (hdr->flags & UCACHE_SGRAPH_FLAG_EMPTY_ROOT)
		? 0
		: UCACHE_SGRAPH_ROOT_OFFSET(hdr->pin_word_count)
	;

	if (root_offset != 0 && root_offset >= buf_len) {
		return false;
	}

	memset(root_val, 0, sizeof(*root_val));

	root_val->type = hdr->root_type;
	root_val->offset = root_offset;

	switch (root_val->type) {
		case UCACHE_SGRAPH_VAL_ARR:
		case UCACHE_SGRAPH_VAL_DYNAMIC_ARR:
		case UCACHE_SGRAPH_VAL_SHAPED_ARR:
		case UCACHE_SGRAPH_VAL_ENUM:
		case UCACHE_SGRAPH_VAL_STR:
			return true;
		case UCACHE_SGRAPH_VAL_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_SERDES_OBJ:
			return root_offset != 0;
		default:
			return false;
	}
}

#if ZEND_DEBUG
static void *ucache_sgraph_rebase_ptr(
		void *ptr,
		const uint8_t *old_base,
		const uint8_t *new_base,
		size_t len)
{
	size_t offset;

	if (!ucache_sgraph_ptr_in_range(ptr, old_base, len)) {
		return ptr;
	}

	offset = (uintptr_t) ptr - (uintptr_t) old_base;

	return (void *) (new_base + offset);
}

static zend_array *ucache_sgraph_rebase_verbatim_zval(
		ucache_sgraph_rebase_ctx *ctx,
		zval *val)
{
	switch (Z_TYPE_P(val)) {
		case IS_STRING:
			Z_STR_P(val) = (zend_string *) ucache_sgraph_rebase_ptr(
				Z_STR_P(val),
				ctx->old_base,
				ctx->new_base,
				ctx->len
			);

			return NULL;
		case IS_ARRAY:
			Z_ARR_P(val) = (zend_array *) ucache_sgraph_rebase_ptr(
				Z_ARR_P(val),
				ctx->old_base,
				ctx->new_base,
				ctx->len
			);

			return Z_ARR_P(val);
		default:
			return NULL;
	}
}

static bool ucache_sgraph_rebase_verbatim_arr(
		ucache_sgraph_rebase_ctx *ctx,
		zend_array *arr)
{
	zend_array *elem_arr;
	zval *packed;
	Bucket *bucket;
	uint32_t i;
	void *data;

	if (ucache_stack_overflowed()) {
		return false;
	}

	if (!ucache_sgraph_ptr_in_range(
			arr,
			ctx->new_base,
			ctx->len
		)
	) {
		return true;
	}

	if (!ucache_seen_test_and_add(ctx->seen, arr)) {
		return true;
	}

	data = HT_GET_DATA_ADDR(arr);
	data = ucache_sgraph_rebase_ptr(data, ctx->old_base, ctx->new_base, ctx->len);

	HT_SET_DATA_ADDR(arr, data);

	if (!ucache_sgraph_ptr_in_range(data, ctx->new_base, ctx->len)) {
		return false;
	}

	if (HT_IS_PACKED(arr)) {
		packed = arr->arPacked;
		for (i = 0; i < arr->nNumUsed; i++) {
			elem_arr = ucache_sgraph_rebase_verbatim_zval(ctx, &packed[i]);
			if (elem_arr != NULL && !ucache_sgraph_rebase_verbatim_arr(ctx, elem_arr)) {
				return false;
			}
		}
	} else {
		bucket = arr->arData;
		for (i = 0; i < arr->nNumUsed; i++) {
			if (bucket[i].key != NULL) {
				bucket[i].key = (zend_string *) ucache_sgraph_rebase_ptr(
					bucket[i].key,
					ctx->old_base,
					ctx->new_base,
					ctx->len
				);

				if (!ucache_sgraph_ptr_in_range(bucket[i].key, ctx->new_base, ctx->len)) {
					return false;
				}
			}

			elem_arr = ucache_sgraph_rebase_verbatim_zval(ctx, &bucket[i].val);
			if (elem_arr != NULL && !ucache_sgraph_rebase_verbatim_arr(ctx, elem_arr)) {
				return false;
			}
		}
	}

	return true;
}

static bool ucache_sgraph_rebase_graph_val(
		ucache_sgraph_rebase_ctx *ctx,
		const ucache_sgraph_val *gval)
{
	const ucache_sgraph_arr *garr;
	const ucache_sgraph_arr_elem *gelems;
	const ucache_sgraph_shaped_arr *gsarr;
	const ucache_sgraph_val *gvals;
	const ucache_sgraph_obj *gobj;
	const ucache_sgraph_prop *props;
	const ucache_sgraph_safe_direct_obj *gsd;
	const ucache_sgraph_serialized_obj *gser;
	const ucache_sgraph_shaped_state_obj *gsstate;
	const ucache_sgraph_state_schema *state_schema;
	zend_array *arr;
	uint32_t i;

	if (ucache_stack_overflowed()) {
		return false;
	}

	switch (gval->type) {
		case UCACHE_SGRAPH_VAL_ARR:
			if (gval->offset == 0) {
				return true;
			}

			arr = (zend_array *) (void *) (ctx->new_base + gval->offset);

			return ucache_sgraph_rebase_verbatim_arr(ctx, arr);
		case UCACHE_SGRAPH_VAL_DYNAMIC_ARR:
			garr = (const ucache_sgraph_arr *) (ctx->new_base + gval->offset);
			if (garr->flags & UCACHE_SGRAPH_ARR_FLAG_PACKED_VALS) {
				gvals = (const ucache_sgraph_val *) (ctx->new_base + garr->elems_offset);
				for (i = 0; i < garr->count; i++) {
					if (!ucache_sgraph_rebase_graph_val(ctx, &gvals[i])) {
						return false;
					}
				}

				return true;
			}

			gelems = (const ucache_sgraph_arr_elem *) (ctx->new_base + garr->elems_offset);

			for (i = 0; i < garr->count; i++) {
				if (!ucache_sgraph_rebase_graph_val(ctx, &gelems[i].val)) {
					return false;
				}
			}

			return true;
		case UCACHE_SGRAPH_VAL_SHAPED_ARR:
			gsarr = (const ucache_sgraph_shaped_arr *) (ctx->new_base + gval->offset);
			gvals = (const ucache_sgraph_val *) (
				ctx->new_base + ucache_sgraph_shaped_arr_vals_offset(gval->offset)
			);

			for (i = 0; i < gsarr->count; i++) {
				if (!ucache_sgraph_rebase_graph_val(ctx, &gvals[i])) {
					return false;
				}
			}

			return true;
		case UCACHE_SGRAPH_VAL_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_OBJ:
			gobj = (const ucache_sgraph_obj *) (ctx->new_base + gval->offset);
			props = (const ucache_sgraph_prop *) (ctx->new_base + gobj->props_offset);

			for (i = 0; i < gobj->prop_count; i++) {
				if (!ucache_sgraph_rebase_graph_val(ctx, &props[i].val)) {
					return false;
				}
			}

			return true;
		case UCACHE_SGRAPH_VAL_REF:
			return ucache_sgraph_rebase_graph_val(
				ctx,
				&((const ucache_sgraph_ref *) (ctx->new_base + gval->offset))->inner
			);
		case UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ:
			gsstate =
				(const ucache_sgraph_shaped_state_obj *) (ctx->new_base + gval->offset)
			;
			state_schema =
				(const ucache_sgraph_state_schema *) (ctx->new_base + gsstate->state_schema_offset)
			;
			gvals = (const ucache_sgraph_val *) (ctx->new_base + gsstate->state_vals_offset);

			for (i = 0; i < state_schema->count; i++) {
				if (!ucache_sgraph_rebase_graph_val(ctx, &gvals[i])) {
					return false;
				}
			}

			return true;
		case UCACHE_SGRAPH_VAL_SERIALIZED_OBJ:
			gser =
				(const ucache_sgraph_serialized_obj *) (ctx->new_base + gval->offset)
			;

			return ucache_sgraph_rebase_graph_val(ctx, &gser->state);
		case UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ: {
			gsd =
				(const ucache_sgraph_safe_direct_obj *) (ctx->new_base + gval->offset)
			;

			if (!ucache_sgraph_rebase_graph_val(ctx, &gsd->state)) {
				return false;
			}

			props = (const ucache_sgraph_prop *) (ctx->new_base + gsd->props_offset);
			for (i = 0; i < gsd->prop_count; i++) {
				if (!ucache_sgraph_rebase_graph_val(ctx, &props[i].val)) {
					return false;
				}
			}

			return true;
		}
		default:
			return true;
	}
}

static bool ucache_sgraph_rebase_payload_ptrs(
		const uint8_t *buf,
		size_t glen,
		const uint8_t *old_base)
{
	const ucache_sgraph_hdr *hdr;
	ucache_sgraph_rebase_ctx ctx;
	ucache_sgraph_val root_val;
	HashTable seen_arrs;
	bool result;

	hdr = (const ucache_sgraph_hdr *) buf;

	if (!ucache_sgraph_load_root_val(hdr, glen, &root_val)) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	ctx.old_base = old_base;
	ctx.new_base = buf;
	ctx.len = glen;
	ctx.seen = &seen_arrs;

	result = ucache_sgraph_rebase_graph_val(&ctx, &root_val);

	zend_hash_destroy(&seen_arrs);

	return result;
}
#endif /* ZEND_DEBUG */

static void ucache_decode_resolve_cache_release(void)
{
	memset((void *) UC_G(decode_resolve_direct_keys), 0, sizeof(UC_G(decode_resolve_direct_keys)));
	memset(UC_G(decode_resolve_direct_vals), 0, sizeof(UC_G(decode_resolve_direct_vals)));

	UC_G(decode_resolve_direct_next) = 0;

	if (UC_G(decode_resolve_cache) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(decode_resolve_cache));

	efree(UC_G(decode_resolve_cache));

	UC_G(decode_resolve_cache) = NULL;
}

static void ucache_decode_shape_proto_cache_release(void)
{
	memset((void *) UC_G(decode_shape_proto_direct_keys), 0, sizeof(UC_G(decode_shape_proto_direct_keys)));
	memset(UC_G(decode_shape_proto_direct_vals), 0, sizeof(UC_G(decode_shape_proto_direct_vals)));

	UC_G(decode_shape_proto_direct_next) = 0;

	if (UC_G(decode_shape_proto_cache) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(decode_shape_proto_cache));

	efree(UC_G(decode_shape_proto_cache));

	UC_G(decode_shape_proto_cache) = NULL;
}

static void ucache_owned_decode_frame_destroy(ucache_owned_decode_frame *frame)
{
	HashTable *tables[] = { frame->arrs, frame->shapes, frame->resolve, frame->strs };
	uint32_t i;

	for (i = 0; i < sizeof(tables) / sizeof(tables[0]); i++) {
		if (tables[i] != NULL) {
			zend_hash_destroy(tables[i]);
			efree(tables[i]);
		}
	}

	if (frame->snapshot != NULL) {
		efree(frame->snapshot);
	}

	efree(frame);
}

static zend_never_inline void ucache_decode_save_maps(void)
{
	ucache_restore_queue *queue = ucache_restore_queue_push();

	queue->saved_identity_map = UC_G(decode_identity_map);
	queue->saved_ref_map = UC_G(decode_ref_map);

	UC_G(decode_identity_map) = NULL;
	UC_G(decode_ref_map) = NULL;
}

static ZEND_COLD zend_never_inline bool ucache_decode_leave_keeping_overflow(uint32_t depth)
{
	bool overflowed = UC_G(stack_overflowed);

	ucache_decode_leave_impl(depth, false);

	UC_G(stack_overflowed) = overflowed;

	return false;
}

static bool ucache_decode_leave(uint32_t depth, bool result)
{
	if (UNEXPECTED(!result)) {
		return ucache_decode_leave_keeping_overflow(depth);
	}

	return ucache_decode_leave_impl(depth, true);
}

static zend_never_inline bool ucache_sgraph_decode_owned(
		const uint8_t *buf,
		size_t buf_len,
		ucache_sgraph_snapshot *snapshot,
		zval *dst)
{
	const ucache_sgraph_hdr *hdr;
	const uint8_t *gbuf;
	ucache_sgraph_val root_val;
	ucache_owned_decode_frame *owned_frame;
	zend_string *str;
	uint32_t depth;
	bool result = false;

	if (!ucache_owned_decode_validate_proc_impl()) {
		goto done;
	}

	gbuf = ucache_sgraph_locate(
		buf,
		buf_len,
		&buf_len
	);
	if (gbuf == NULL) {
		goto done;
	}

	buf = gbuf;

	hdr = (const ucache_sgraph_hdr *) buf;
	if (!ucache_sgraph_load_root_val(hdr, buf_len, &root_val)) {
		goto done;
	}

	if (root_val.type == UCACHE_SGRAPH_VAL_STR) {
		str = ucache_decode_str_at_raw(buf, buf_len, root_val.offset);
		if (str != NULL) {
			ZVAL_NEW_STR(dst, ucache_owned_str_dup(str));
			result = true;
		}

		goto done;
	}

	owned_frame = ecalloc(1, sizeof(*owned_frame));
	owned_frame->prev = UC_G(owned_decode_frame);
	owned_frame->owner_pid = ucache_cached_pid();
	owned_frame->saved_identity_map = UC_G(decode_identity_map);
	owned_frame->saved_ref_map = UC_G(decode_ref_map);
	owned_frame->snapshot = snapshot;

	UC_G(owned_decode_frame) = owned_frame;
	UC_G(decode_identity_map) = NULL;
	UC_G(decode_ref_map) = NULL;

	depth = ucache_decode_enter();

	result = ucache_sgraph_decode_val(buf, buf_len, &root_val, dst);
	result = ucache_owned_decode_validate_proc_impl() && result;
	result = ucache_decode_leave(depth, result);
	result = ucache_owned_decode_validate_proc_impl() && result;

	UC_G(decode_identity_map) = owned_frame->saved_identity_map;
	UC_G(decode_ref_map) = owned_frame->saved_ref_map;
	UC_G(owned_decode_frame) = owned_frame->prev;

	ucache_owned_decode_frame_destroy(owned_frame);

	return result;

done:
	if (snapshot != NULL) {
		efree(snapshot);
	}

	return result;
}

static zend_never_inline bool ucache_sgraph_decode_outside_owned_frame(
		const uint8_t *buf,
		size_t buf_len,
		const ucache_sgraph_val *root_val,
		zval *dst)
{
	ucache_owned_decode_frame *owned_frame = UC_G(owned_decode_frame);
	uint32_t depth;
	bool result = false;

	UC_G(owned_decode_frame) = NULL;

	zend_try {
		depth = ucache_decode_enter();

		result = ucache_sgraph_decode_val(buf, buf_len, root_val, dst);
		result = ucache_decode_leave(depth, result);
	} zend_catch {
		UC_G(owned_decode_frame) = owned_frame;

		zend_bailout();
	} zend_end_try();

	UC_G(owned_decode_frame) = owned_frame;

	return result;
}

zend_property_info *ucache_sgraph_declared_prop_info(
		zend_class_entry *ce,
		zend_string *name)
{
	const char *class_name, *prop_name;
	zend_property_info *prop_info;
	zend_class_entry *scope;
	size_t prop_len, class_name_len;

	if (ZSTR_LEN(name) == 0) {
		return NULL;
	}

	if (ZSTR_VAL(name)[0] != '\0') {
		prop_info = zend_hash_find_ptr(&ce->properties_info, name);
	} else {
		if (!ucache_sgraph_mangled_name_is_well_formed(name) ||
			zend_unmangle_property_name_ex(name, &class_name, &prop_name, &prop_len) != SUCCESS ||
			class_name == NULL
		) {
			return NULL;
		}

		scope = ce;
		if (class_name[0] != '*' || class_name[1] != '\0') {
			class_name_len = (size_t) (prop_name - class_name - 1);
			while (scope != NULL &&
				zend_binary_strcasecmp(ZSTR_VAL(scope->name), ZSTR_LEN(scope->name), class_name, class_name_len) != 0
			) {
				scope = scope->parent;
			}

			if (scope == NULL) {
				return NULL;
			}
		}

		prop_info = zend_hash_str_find_ptr(&scope->properties_info, prop_name, prop_len);
	}

	if (prop_info == NULL ||
		(prop_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) != 0 ||
		prop_info->offset == ZEND_VIRTUAL_PROPERTY_OFFSET ||
		!zend_string_equals(prop_info->name, name)
	) {
		return NULL;
	}

	return prop_info;
}

#if ZEND_DEBUG
void ucache_sgraph_check_rebase_complete(
		const uint8_t *dst_base,
		size_t glen,
		const uint8_t *src_base)
{
	uint8_t *check_buf;
	bool result;

	check_buf = emalloc(glen);
	memcpy(check_buf, dst_base, glen);

	result = ucache_sgraph_rebase_payload_ptrs(
		dst_base,
		glen,
		src_base
	);

	ZEND_ASSERT(result);
	ZEND_ASSERT(memcmp(check_buf, dst_base, glen) == 0);

	efree(check_buf);
}
#endif /* ZEND_DEBUG */

bool ucache_owned_decode_validate_proc_impl(void)
{
	ucache_owned_decode_frame *frame = UC_G(owned_decode_frame);

	if (frame == NULL || !ucache_owned_decode_frame_reads_inherited_lease(frame)) {
		return true;
	}

	UC_G(exec_poisoned) = true;
	UC_G(in_req_shutdown) = true;
	UC_G(runtime_resolved) = false;

	if (!EG(exception)) {
		zend_throw_error(NULL, "Cannot continue UserCache restoration after fork; restart the PHP execution in the child");
	}

	return false;
}

void ucache_decode_payload_addr_caches_release(void)
{
	ucache_decode_resolve_cache_release();
	ucache_decode_shape_proto_cache_release();
}

void ucache_decode_maps_teardown(void)
{
	ucache_restore_queue *queue;
	ucache_owned_decode_frame *frame;

	while ((queue = UC_G(decode_restore_queue)) != NULL) {
		(void) ucache_restore_queue_finish(queue, false);

		UC_G(decode_restore_queue) = queue->prev;

		ucache_decode_identity_map_teardown();
		ucache_decode_ref_map_teardown();

		UC_G(decode_identity_map) = queue->saved_identity_map;
		UC_G(decode_ref_map) = queue->saved_ref_map;

		ucache_restore_queue_destroy(queue);
	}

	while ((frame = UC_G(owned_decode_frame)) != NULL) {
		ucache_decode_identity_map_teardown();
		ucache_decode_ref_map_teardown();

		UC_G(decode_identity_map) = frame->saved_identity_map;
		UC_G(decode_ref_map) = frame->saved_ref_map;
		UC_G(owned_decode_frame) = frame->prev;

		ucache_owned_decode_frame_destroy(frame);
	}

	ucache_decode_identity_map_teardown();
	ucache_decode_ref_map_teardown();

	UC_G(decode_depth) = 0;
	UC_G(decode_nesting) = 0;
}

static PHP_UCACHE_HOT bool ucache_sgraph_decode_impl(
		const uint8_t *buf,
		size_t buf_len,
		zval *dst)
{
	const ucache_sgraph_hdr *hdr;
	const uint8_t *gbuf;
	ucache_sgraph_val root_val;
	zend_string *str;
	zend_array *arr;
	uint32_t depth;
	bool result;

	if (UNEXPECTED(UC_G(persistent_exec))) {
		return ucache_sgraph_decode_owned(buf, buf_len, NULL, dst);
	}

	gbuf = ucache_sgraph_locate(buf, buf_len, &buf_len);
	if (gbuf == NULL) {
		return false;
	}

	buf = gbuf;
	hdr = (const ucache_sgraph_hdr *) buf;
	if (!ucache_sgraph_load_root_val(hdr, buf_len, &root_val)) {
		return false;
	}

	switch (root_val.type) {
		case UCACHE_SGRAPH_VAL_STR:
			if (ucache_stack_overflowed()) {
				return false;
			}

			str = ucache_decode_str_at_raw(buf, buf_len, root_val.offset);
			if (str == NULL) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, str);

			return true;
		case UCACHE_SGRAPH_VAL_ARR:
			if (ucache_stack_overflowed()) {
				return false;
			}

			if (root_val.offset == 0) {
				ZVAL_EMPTY_ARRAY(dst);

				return true;
			}

			if (!ucache_decode_range_ok(buf_len, root_val.offset, sizeof(zend_array))) {
				return false;
			}

			arr = (zend_array *) (void *) (buf + root_val.offset);
			if (!ucache_decode_verbatim_arr_ok(buf, buf_len, NULL, arr)) {
				ucache_decode_payload_addr_caches_release();

				return false;
			}

			ZVAL_ARR(dst, arr);

			Z_TYPE_FLAGS_P(dst) = 0;

			return true;
		default:
			break;
	}

	if (UNEXPECTED(UC_G(owned_decode_frame) != NULL)) {
		result = ucache_sgraph_decode_outside_owned_frame(buf, buf_len, &root_val, dst);
	} else {
		depth = ucache_decode_enter();

		result = ucache_sgraph_decode_val(buf, buf_len, &root_val, dst);
		result = ucache_decode_leave(depth, result);
	}

	if (!result) {
		ucache_decode_payload_addr_caches_release();
	}

	return result;
}

PHP_UCACHE_HOT bool ucache_sgraph_decode(
		const uint8_t *buf,
		size_t buf_len,
		uint64_t gen,
		zval *dst)
{
	uint64_t prev_gen = UC_G(decode_payload_gen);
	bool prev_validated = UC_G(decode_payload_validated), validated, result;

	validated = ucache_decode_payload_begin(buf, gen);
	result = ucache_sgraph_decode_impl(buf, buf_len, dst);
	ucache_decode_payload_end(buf, validated, result);

	UC_G(decode_payload_validated) = prev_validated;
	UC_G(decode_payload_gen) = prev_gen;

	return result;
}

ucache_sgraph_snapshot *ucache_sgraph_snapshot_create(const uint8_t *buf, size_t buf_len)
{
	const uint8_t *gbuf;
	ucache_sgraph_snapshot *snapshot;
	size_t glen;

	gbuf = ucache_sgraph_locate(buf, buf_len, &glen);
	if (gbuf == NULL) {
		return NULL;
	}

	snapshot = safe_emalloc(1, glen, offsetof(ucache_sgraph_snapshot, data));
	snapshot->origin = gbuf;
	snapshot->len = glen;

	memcpy(snapshot->data, gbuf, glen);

	return snapshot;
}

bool ucache_sgraph_decode_snapshot(ucache_sgraph_snapshot *snapshot, uint64_t gen, zval *dst)
{
	const uint8_t *origin = snapshot->origin;
	uint64_t prev_gen = UC_G(decode_payload_gen);
	bool prev_validated = UC_G(decode_payload_validated), validated, result;

	validated = ucache_decode_payload_begin(origin, gen);
	result = ucache_sgraph_decode_owned(snapshot->data, snapshot->len, snapshot, dst);
	ucache_decode_payload_end(origin, validated, result);

	UC_G(decode_payload_validated) = prev_validated;
	UC_G(decode_payload_gen) = prev_gen;

	return result;
}
