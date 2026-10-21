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
#include "Zend/zend_closures.h"
#include "Zend/zend_operators.h"

#define UCACHE_SGRAPH_COPY_RETRIES				2U
#define UCACHE_SGRAPH_PACKED_COMPACTION_PERCENT	75U
#define UCACHE_SGRAPH_STATE_SCHEMA_MAX_VALS		64U
#define UCACHE_DECLARED_PROP_IDX_NONE			0U

typedef bool (*ucache_sgraph_state_producer_t)(const zval *val, zval *state, const void *producer_arg);

typedef enum {
	UCACHE_OBJ_ROUTE_UNSTORABLE = 0,
	UCACHE_OBJ_ROUTE_SAFE_DIRECT,
	UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE,
	UCACHE_OBJ_ROUTE_SERIALIZE_PROPS,
	UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE,
	UCACHE_OBJ_ROUTE_SLEEP,
	UCACHE_OBJ_ROUTE_WAKEUP,
	UCACHE_OBJ_ROUTE_SERDES,
	UCACHE_OBJ_ROUTE_PLAIN
} ucache_obj_route;

typedef struct {
	ucache_obj_route route;
	const php_ucache_safe_direct_handlers *sd_handlers;
} ucache_obj_route_info;

typedef struct {
	size_t size;
	HashTable seen_objs;
	HashTable seen_refs;
	HashTable str_dedup;
	HashTable arr_shape_dedup;
	HashTable state_schema_dedup;
	HashTable direct_arr_dedup;
	HashTable direct_verdicts;
	HashTable enum_dedup;
	uint16_t pin_word_count;
	bool verbatim_arrs_allowed;
	bool reserve_failed;
	bool has_custom_obj_handlers;
	HashTable *verbatim_verdicts;
	HashTable *state_memo;
} ucache_sgraph_calc_ctx;

typedef struct {
	uint8_t *buf;
	size_t size;
	size_t pos;
	uint32_t *fixup_offsets;
	uint32_t fixup_count;
	uint32_t fixup_capacity;
	HashTable seen_objs;
	HashTable seen_refs;
	HashTable str_dedup;
	HashTable arr_shape_dedup;
	HashTable state_schema_dedup;
	HashTable direct_arr_dedup;
	HashTable direct_verdicts;
	HashTable enum_dedup;
	HashTable identity_pins;
	uint16_t pin_word_count;
	bool has_shared_identity;
	bool has_obj;
	bool prefers_proto;
	bool has_userland_restore_obj;
	bool has_verbatim_arr;
	bool packed_vals_allowed;
	bool verbatim_arrs_allowed;
	const HashTable *verbatim_verdicts;
	HashTable *state_memo;
} ucache_sgraph_copy_ctx;

typedef struct {
	zend_class_entry *ce;
	uint32_t offset;
	uint32_t count;
	uint32_t sleep_slots_offset;
	zend_string *keys[1];
} ucache_sgraph_shape;

static uint32_t ucache_sgraph_declared_prop_idx_plus_one(
		zend_class_entry *ce,
		zend_string *name,
		uint32_t pos);
static void ucache_sgraph_shape_dtor(zval *zv);
static bool ucache_sgraph_calc_val(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val);
static bool ucache_sgraph_copy_val(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst);
static bool ucache_sgraph_copy_verbatim_val(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zval *dst);

static zend_always_inline uint16_t ucache_sgraph_active_pin_word_count(void)
{
	return (uint16_t) (ucache_ctx_graph_pin_slot_count(ucache_active_ctx()) / 32U);
}

static zend_always_inline void ucache_sgraph_calc_init(ucache_sgraph_calc_ctx *ctx)
{
	ctx->size = 0;
	ctx->pin_word_count = ucache_sgraph_active_pin_word_count();
	ctx->reserve_failed = false;
	ctx->has_custom_obj_handlers = false;
	ctx->verbatim_arrs_allowed = false;
	ctx->verbatim_verdicts = NULL;
	ctx->state_memo = NULL;

	zend_hash_init(&ctx->seen_objs, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_refs, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->str_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->arr_shape_dedup, 8, NULL, ucache_sgraph_shape_dtor, 0);
	zend_hash_init(&ctx->state_schema_dedup, 8, NULL, ucache_sgraph_shape_dtor, 0);
	zend_hash_init(&ctx->direct_arr_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->enum_dedup, 8, NULL, NULL, 0);
}

static zend_always_inline void ucache_sgraph_calc_destroy(ucache_sgraph_calc_ctx *ctx)
{
	zend_hash_destroy(&ctx->enum_dedup);
	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->direct_arr_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->arr_shape_dedup);
	zend_hash_destroy(&ctx->str_dedup);
	zend_hash_destroy(&ctx->seen_refs);
	zend_hash_destroy(&ctx->seen_objs);
}

static zend_always_inline void ucache_sgraph_calc_verbatim_init(ucache_sgraph_calc_ctx *ctx)
{
	ctx->size = 0;
	ctx->pin_word_count = ucache_sgraph_active_pin_word_count();
	ctx->reserve_failed = false;

	zend_hash_init(&ctx->str_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_arr_dedup, 8, NULL, NULL, 0);
}

static zend_always_inline void ucache_sgraph_calc_verbatim_destroy(ucache_sgraph_calc_ctx *ctx)
{
	zend_hash_destroy(&ctx->direct_arr_dedup);
	zend_hash_destroy(&ctx->str_dedup);
}

static zend_always_inline bool ucache_sgraph_calc_reserve(
		ucache_sgraph_calc_ctx *ctx,
		size_t amount)
{
	size_t aligned_amount;

	aligned_amount = UCACHE_ALIGNED_SIZE(amount);
	if (ctx->size > SIZE_MAX - aligned_amount) {
		ctx->reserve_failed = true;

		return false;
	}

	ctx->size += aligned_amount;

	if (UNEXPECTED(ctx->size > UINT32_MAX)) {
		ctx->reserve_failed = true;

		return false;
	}

	return true;
}

static zend_always_inline bool ucache_sgraph_calc_reserve_str(
		ucache_sgraph_calc_ctx *ctx,
		const zend_string *str)
{
	if (zend_hash_add_empty_element(&ctx->str_dedup, (zend_string *) str) == NULL) {
		return true;
	}

	return ucache_sgraph_calc_reserve(ctx, _ZSTR_STRUCT_SIZE(ZSTR_LEN(str)));
}

static zend_always_inline bool ucache_sgraph_arr_is_fresh_empty(const HashTable *arr)
{
	return arr->nNumOfElements == 0 && arr->nNextFreeElement == ZEND_LONG_MIN;
}

static zend_always_inline uint32_t ucache_sgraph_verbatim_compact_table_size(uint32_t count)
{
	uint32_t table_size = HT_MIN_SIZE;

	while (table_size < count) {
		table_size <<= 1;
	}

	return table_size;
}

static zend_always_inline size_t ucache_sgraph_verbatim_compact_data_size(uint32_t count)
{
	return HT_HASH_SIZE(HT_SIZE_TO_MASK(ucache_sgraph_verbatim_compact_table_size(count))) +
		(size_t) count * sizeof(Bucket)
	;
}

static zend_always_inline bool ucache_sgraph_verbatim_arr_needs_compaction(const HashTable *arr)
{
	if (HT_IS_PACKED(arr)) {
		return !HT_IS_WITHOUT_HOLES(arr) &&
			ucache_sgraph_verbatim_compact_data_size(arr->nNumOfElements) * 100U <=
				HT_PACKED_USED_SIZE(arr) * UCACHE_SGRAPH_PACKED_COMPACTION_PERCENT
		;
	}

	return arr->nNumUsed != arr->nNumOfElements ||
		(arr->nTableSize > HT_MIN_SIZE && arr->nTableSize / 2 >= arr->nNumOfElements)
	;
}

static zend_always_inline size_t ucache_sgraph_verbatim_data_size(const HashTable *arr)
{
	if (ucache_sgraph_verbatim_arr_needs_compaction(arr)) {
		return ucache_sgraph_verbatim_compact_data_size(arr->nNumOfElements);
	}

	return HT_IS_PACKED(arr) ? HT_PACKED_USED_SIZE(arr) : HT_USED_SIZE(arr);
}

static zend_always_inline bool ucache_sgraph_next_free_is_wide(zend_long next_free)
{
	return next_free < 0 || (zend_ulong) next_free > (zend_ulong) UINT32_MAX;
}

static zend_always_inline bool ucache_sgraph_calc_reserve_dynamic_arr_as_mixed(
		ucache_sgraph_calc_ctx *ctx,
		const HashTable *arr)
{
	return ucache_sgraph_calc_reserve(ctx, sizeof(ucache_sgraph_arr)) &&
		ucache_sgraph_calc_reserve(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(ucache_sgraph_arr_elem)
		) &&
		(
			!ucache_sgraph_next_free_is_wide(arr->nNextFreeElement) ||
			ucache_sgraph_calc_reserve(ctx, sizeof(int64_t))
		)
	;
}

static zend_always_inline uint32_t ucache_sgraph_shape_next_free_encode(zend_long next_free)
{
	return next_free == ZEND_LONG_MIN ? UCACHE_SGRAPH_SHAPE_NEXT_FREE_UNSET : (uint32_t) next_free;
}

static zend_always_inline uint16_t ucache_sgraph_sleep_slot_hint(
		zend_class_entry *ce,
		zend_string *prop_name,
		uint32_t pos)
{
	uint32_t slot_plus_one = ucache_sgraph_declared_prop_idx_plus_one(ce, prop_name, pos);

	return slot_plus_one <= UINT16_MAX ? (uint16_t) slot_plus_one : 0;
}

static zend_always_inline bool ucache_sgraph_arr_has_shape(const HashTable *arr)
{
	zend_string *key;

	if (arr->nNumOfElements == 0 ||
		HT_IS_PACKED(arr) ||
		arr->nNumOfElements > UCACHE_SGRAPH_ARR_SHAPE_MAX_KEYS ||
		(arr->nNextFreeElement < 0 && arr->nNextFreeElement != ZEND_LONG_MIN) ||
		arr->nNextFreeElement >= UCACHE_SGRAPH_SHAPE_NEXT_FREE_UNSET ||
		!HT_IS_WITHOUT_HOLES(arr)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (key == NULL) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static zend_always_inline bool ucache_sgraph_state_memo_entry_is_schema_verdict(const zval *entry)
{
	return Z_TYPE_P(entry) == IS_TRUE || Z_TYPE_P(entry) == IS_FALSE;
}

static zend_always_inline void ucache_sgraph_state_memo_retain_key(zend_object *obj)
{
	GC_ADDREF(obj);
}

static zend_always_inline bool ucache_sgraph_can_use_verbatim_arrs(void)
{
#ifdef ZEND_WIN32
	return false;
#else
	return !ucache_ctx_is_boundary(ucache_active_ctx());
#endif
}

static zend_always_inline bool ucache_sgraph_safe_direct_prop_is_internal(
		const php_ucache_safe_direct_handlers *handlers,
		const zend_object *obj,
		const zend_string *prop_name)
{
	return handlers->is_internal_prop != NULL && handlers->is_internal_prop(obj, prop_name);
}

static zend_always_inline bool ucache_sgraph_props_unbuilt(zend_object *obj)
{
	return obj->properties == NULL && !zend_object_is_lazy(obj);
}

static zend_always_inline bool ucache_sgraph_plain_props_unbuilt(zend_object *obj)
{
	return obj->handlers->get_properties_for == NULL &&
		obj->handlers->get_properties == zend_std_get_properties &&
		ucache_sgraph_props_unbuilt(obj)
	;
}

static zend_always_inline zval *ucache_sgraph_declared_prop(
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers,
		uint32_t slot,
		zend_string **prop_name)
{
	zend_property_info *prop_info = obj->ce->properties_info_table[slot];
	zval *prop;

	if (prop_info == NULL) {
		return NULL;
	}

	prop = OBJ_PROP(obj, prop_info->offset);
	if (handlers != NULL && ucache_sgraph_safe_direct_prop_is_internal(handlers, obj, prop_info->name)) {
		return NULL;
	}

	*prop_name = prop_info->name;

	return prop;
}

static zend_always_inline uint32_t ucache_sgraph_declared_prop_count(
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers)
{
	zend_string *prop_name;
	uint32_t slot, count = 0;

	for (slot = 0; slot < (uint32_t) obj->ce->default_properties_count; slot++) {
		if (ucache_sgraph_declared_prop(obj, handlers, slot, &prop_name) != NULL) {
			count++;
		}
	}

	return count;
}

static zend_always_inline void ucache_sgraph_release_traversed_arr(HashTable *arr)
{
	if (!(GC_FLAGS(arr) & GC_IMMUTABLE)) {
		GC_DTOR(arr);
	}
}

static zend_always_inline void ucache_sgraph_hold_props_across_hooks(zend_object *obj, HashTable *props)
{
	GC_ADDREF(obj);
	GC_TRY_ADDREF(props);
}

static zend_always_inline void ucache_sgraph_release_held_props(zend_object *obj, HashTable *props)
{
	zend_release_properties(props);

	OBJ_RELEASE(obj);
}

static zend_always_inline const php_ucache_safe_direct_handlers *ucache_sgraph_usable_safe_direct_handlers(
		zend_class_entry *ce)
{
	const php_ucache_safe_direct_handlers *handlers;
	zend_class_entry *base_ce = NULL;

	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		return NULL;
	}

	handlers = ucache_safe_direct_find_handlers(ce, &base_ce);
	if (handlers == NULL || ucache_class_overrides_safe_direct_magic_serialize(ce, handlers, base_ce)) {
		return NULL;
	}

	return handlers;
}

static zend_always_inline bool ucache_class_has_sleep(zend_class_entry *ce)
{
	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL;
}

static zend_always_inline bool ucache_sgraph_can_use_sleep_obj(zend_class_entry *ce)
{
	if ((ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		ucache_class_has_serialize_handlers(ce) ||
		!ucache_class_has_sleep(ce)
	) {
		return false;
	}

	return ucache_sgraph_can_restore_props(ce, true);
}

static zend_always_inline bool ucache_sgraph_can_use_wakeup_obj(zend_class_entry *ce)
{
	return ce->__serialize == NULL &&
		!ucache_class_has_sleep(ce) &&
		ucache_sgraph_wakeup_rebuilds_state(ce)
	;
}

static zend_always_inline void ucache_sgraph_copy_record_fixup(
		ucache_sgraph_copy_ctx *ctx,
		const void *slot)
{
	uint32_t capacity;

	ZEND_ASSERT(ucache_sgraph_ptr_in_range(slot, ctx->buf, ctx->size));
	ZEND_ASSERT(((uintptr_t) slot & (sizeof(uintptr_t) - 1)) == 0);

	if (ctx->fixup_count == ctx->fixup_capacity) {
		capacity = ctx->fixup_capacity == 0 ? 8 : ctx->fixup_capacity * 2;

		ctx->fixup_offsets = safe_erealloc(ctx->fixup_offsets, capacity, sizeof(*ctx->fixup_offsets), 0);
		ctx->fixup_capacity = capacity;
	}

	ctx->fixup_offsets[ctx->fixup_count++] = (uint32_t) ((const uint8_t *) slot - ctx->buf);
}

static zend_always_inline bool ucache_sgraph_long_is_inline(zend_long val)
{
	return (zend_long) (int32_t) val == val;
}

static zend_always_inline bool ucache_sgraph_ref_is_held_once(const zend_reference *ref)
{
	return GC_REFCOUNT(ref) == 1;
}

static inline bool ucache_class_magic_route_active(zend_class_entry *ce)
{
	const php_ucache_safe_direct_handlers *handlers;
	zend_class_entry *base_ce = NULL;

	if (ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) {
		return false;
	}

	handlers = ucache_safe_direct_find_handlers(ce, &base_ce);

	if (ucache_class_overrides_safe_direct_magic_serialize(ce, handlers, base_ce)) {
		return true;
	}

	return handlers == NULL;
}

static inline bool ucache_class_uses_magic_serialize(zend_class_entry *ce)
{
	return ce->__serialize != NULL && ce->__unserialize != NULL &&
		ucache_class_magic_route_active(ce)
	;
}

static inline bool ucache_class_uses_serialize_props(zend_class_entry *ce)
{
	return ce->__serialize != NULL && ce->__unserialize == NULL &&
		ucache_class_magic_route_active(ce)
	;
}

static inline bool ucache_class_uses_magic_unserialize(zend_class_entry *ce)
{
	if (ce->__serialize != NULL || ce->__unserialize == NULL) {
		return false;
	}

	if (ucache_class_has_serialize_handlers(ce)) {
		return false;
	}

	return ucache_class_magic_route_active(ce);
}

static inline bool ucache_class_uses_serdes(zend_class_entry *ce)
{
	return (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) == 0 && ucache_class_has_serialize_handlers(ce);
}

static void ucache_sgraph_shape_dtor(zval *zv)
{
	ucache_sgraph_shape *shape = Z_PTR_P(zv);
	uint32_t i;

	for (i = 0; i < shape->count; i++) {
		zend_string_release(shape->keys[i]);
	}

	efree(shape);
}

static zend_ulong ucache_sgraph_arr_shape_hash(const HashTable *arr, const zend_class_entry *ce)
{
	const Bucket *bucket;
	zend_ulong hash = (zend_ulong) arr->nNumOfElements;

	ZEND_ASSERT(ucache_sgraph_arr_has_shape(arr));

	if (ce != NULL) {
		hash ^= zend_string_hash_val(ce->name);
	}

	ZEND_HASH_MAP_FOREACH_BUCKET((HashTable *) arr, bucket) {
		hash = (hash + bucket->h) * (zend_ulong) UINT64_C(0x9E3779B97F4A7C15);
	} ZEND_HASH_FOREACH_END();

	return hash;
}

static ucache_sgraph_shape *ucache_sgraph_shape_find(
		HashTable *dedup,
		zend_ulong hash,
		const zend_class_entry *ce,
		const HashTable *arr,
		bool *collision)
{
	const Bucket *bucket;
	ucache_sgraph_shape *shape;
	uint32_t i = 0;

	*collision = false;

	shape = zend_hash_index_find_ptr(dedup, hash);
	if (shape == NULL) {
		return NULL;
	}

	*collision = true;

	if (shape->ce != ce || shape->count != arr->nNumOfElements) {
		return NULL;
	}

	ZEND_HASH_MAP_FOREACH_BUCKET((HashTable *) arr, bucket) {
		if (!zend_string_equals(shape->keys[i], bucket->key)) {
			return NULL;
		}

		i++;
	} ZEND_HASH_FOREACH_END();

	*collision = false;

	return shape;
}

static ucache_sgraph_shape *ucache_sgraph_shape_add(
		HashTable *dedup,
		zend_ulong hash,
		zend_class_entry *ce,
		const HashTable *arr,
		uint32_t offset)
{
	const Bucket *bucket;
	ucache_sgraph_shape *shape;
	uint32_t i = 0;

	shape = safe_emalloc(
		arr->nNumOfElements,
		sizeof(zend_string *),
		offsetof(ucache_sgraph_shape, keys)
	);
	shape->ce = ce;
	shape->offset = offset;
	shape->count = arr->nNumOfElements;
	shape->sleep_slots_offset = 0;

	ZEND_HASH_MAP_FOREACH_BUCKET((HashTable *) arr, bucket) {
		shape->keys[i++] = zend_string_copy(bucket->key);
	} ZEND_HASH_FOREACH_END();

	return zend_hash_index_add_new_ptr(dedup, hash, shape);
}

static bool ucache_sgraph_calc_arr_shape(
		ucache_sgraph_calc_ctx *ctx,
		const HashTable *arr)
{
	zend_string *key;
	zend_ulong hash;
	bool collision;

	hash = ucache_sgraph_arr_shape_hash(arr, NULL);
	if (ucache_sgraph_shape_find(&ctx->arr_shape_dedup, hash, NULL, arr, &collision) != NULL) {
		return true;
	}

	if (!ucache_sgraph_calc_reserve(
			ctx,
			sizeof(ucache_sgraph_arr_shape)
		) ||
		!ucache_sgraph_calc_reserve(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(ucache_sgraph_arr_shape_elem)
		)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (!ucache_sgraph_calc_reserve_str(ctx, key)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	if (!collision) {
		ucache_sgraph_shape_add(&ctx->arr_shape_dedup, hash, NULL, arr, 0);
	}

	return true;
}

static bool ucache_sgraph_calc_state_schema(
		ucache_sgraph_calc_ctx *ctx,
		zend_class_entry *ce,
		const HashTable *arr)
{
	zend_ulong hash;
	bool collision;

	hash = ucache_sgraph_arr_shape_hash(arr, ce);
	if (ucache_sgraph_shape_find(&ctx->state_schema_dedup, hash, ce, arr, &collision) != NULL) {
		return true;
	}

	if (!ucache_sgraph_calc_reserve(
			ctx,
			sizeof(ucache_sgraph_state_schema)
		) ||
		!ucache_sgraph_calc_reserve_str(ctx, ce->name) ||
		!ucache_sgraph_calc_arr_shape(ctx, arr)
	) {
		return false;
	}

	if (!collision) {
		ucache_sgraph_shape_add(&ctx->state_schema_dedup, hash, ce, arr, 0);
	}

	return true;
}

static bool ucache_sgraph_state_val_fits_schema(
		zval *val,
		const HashTable *state_arr,
		uint32_t *vals_left)
{
	zval *elem;

	if (*vals_left == 0 || ucache_stack_overflowed()) {
		return false;
	}

	--*vals_left;

	ZVAL_DEREF(val);

	switch (Z_TYPE_P(val)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			return true;
		case IS_ARRAY:
			if (Z_ARRVAL_P(val) == state_arr) {
				return false;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), elem) {
				if (!ucache_sgraph_state_val_fits_schema(elem, state_arr, vals_left)) {
					return false;
				}
			} ZEND_HASH_FOREACH_END();

			return true;
		default:
			return false;
	}
}

static bool ucache_sgraph_state_arr_fits_schema(const HashTable *arr)
{
	zval *elem;
	uint32_t vals_left = UCACHE_SGRAPH_STATE_SCHEMA_MAX_VALS;

	if (!ucache_sgraph_arr_has_shape(arr)) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
		if (!ucache_sgraph_state_val_fits_schema(elem, arr, &vals_left)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool ucache_sgraph_state_arr_fits_schema_memo(
		HashTable *state_memo,
		const HashTable *arr)
{
	zval *verdict, verdict_zv;

	ZEND_ASSERT(state_memo != NULL);

	verdict = zend_hash_index_find(state_memo, (zend_ulong) (uintptr_t) arr);
	if (verdict != NULL && ucache_sgraph_state_memo_entry_is_schema_verdict(verdict)) {
		return Z_TYPE_P(verdict) == IS_TRUE;
	}

	ZVAL_BOOL(&verdict_zv, ucache_sgraph_state_arr_fits_schema(arr));

	zend_hash_index_add(state_memo, (zend_ulong) (uintptr_t) arr, &verdict_zv);

	return Z_TYPE(verdict_zv) == IS_TRUE;
}

static bool ucache_sgraph_calc_shaped_state_obj(
		ucache_sgraph_calc_ctx *ctx,
		zend_class_entry *ce,
		const HashTable *state_arr)
{
	zval *elem;

	if (!ucache_sgraph_calc_reserve(
			ctx,
			sizeof(ucache_sgraph_shaped_state_obj)
		) ||
		!ucache_sgraph_calc_reserve(
			ctx,
			(size_t) state_arr->nNumOfElements * sizeof(ucache_sgraph_val)
		) ||
		!ucache_sgraph_calc_state_schema(ctx, ce, state_arr)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL((HashTable *) state_arr, elem) {
		if (!ucache_sgraph_calc_val(ctx, elem)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static ucache_obj_route ucache_sgraph_classify_obj_route_impl(zend_class_entry *ce)
{
	if (ucache_class_uses_magic_serialize(ce)) {
		return UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE;
	}

	if (ucache_class_uses_serialize_props(ce)) {
		return ucache_sgraph_can_restore_props(ce, true)
			? UCACHE_OBJ_ROUTE_SERIALIZE_PROPS
			: UCACHE_OBJ_ROUTE_UNSTORABLE
		;
	}

	if (ucache_class_uses_magic_unserialize(ce)) {
		return UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE;
	}

	if (ucache_sgraph_can_use_sleep_obj(ce)) {
		return UCACHE_OBJ_ROUTE_SLEEP;
	}

	if (ucache_sgraph_can_use_wakeup_obj(ce)) {
		return UCACHE_OBJ_ROUTE_WAKEUP;
	}

	if (ucache_class_uses_serdes(ce)) {
		return UCACHE_OBJ_ROUTE_SERDES;
	}

	if (!ucache_sgraph_can_restore_direct(ce)) {
		return UCACHE_OBJ_ROUTE_UNSTORABLE;
	}

	return UCACHE_OBJ_ROUTE_PLAIN;
}

static zend_never_inline ucache_obj_route_info ucache_sgraph_memoize_obj_route(zend_class_entry *ce)
{
	ucache_obj_route_info info;
	zval route_zv;

	if (UC_G(obj_route_memo) == NULL) {
		UC_G(obj_route_memo) = emalloc(sizeof(HashTable));

		zend_hash_init(UC_G(obj_route_memo), 8, NULL, NULL, 0);
	}

	info.sd_handlers = ucache_sgraph_usable_safe_direct_handlers(ce);
	if (info.sd_handlers != NULL) {
		info.route = UCACHE_OBJ_ROUTE_SAFE_DIRECT;

		ZVAL_PTR(&route_zv, (void *) info.sd_handlers);
	} else {
		info.route = ucache_sgraph_classify_obj_route_impl(ce);

		ZVAL_LONG(&route_zv, (zend_long) info.route);
	}

	zend_hash_index_add(UC_G(obj_route_memo), (zend_ulong) (uintptr_t) ce, &route_zv);

	return info;
}

static ucache_obj_route_info ucache_sgraph_classify_obj_route(zend_class_entry *ce)
{
	ucache_obj_route_info info;
	zval *cached;

	if (UC_G(obj_route_memo) == NULL) {
		return ucache_sgraph_memoize_obj_route(ce);
	}

	cached = zend_hash_index_find(UC_G(obj_route_memo), (zend_ulong) (uintptr_t) ce);
	if (cached == NULL) {
		return ucache_sgraph_memoize_obj_route(ce);
	}

	if (Z_TYPE_P(cached) == IS_PTR) {
		info.route = UCACHE_OBJ_ROUTE_SAFE_DIRECT;
		info.sd_handlers = Z_PTR_P(cached);
	} else {
		info.route = (ucache_obj_route) Z_LVAL_P(cached);
		info.sd_handlers = NULL;
	}

	return info;
}

static bool ucache_sgraph_can_copy_verbatim_val(HashTable *direct_verdicts, const zval *val)
{
	const zval *packed_val;
	const HashTable *arr;
	const Bucket *bucket;
	zend_ulong arr_key;
	zval *cached, verdict;
	uint32_t i;
	bool result = true;

	if (ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(val)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			return true;
		case IS_ARRAY:
			arr = Z_ARRVAL_P(val);
			if (ucache_sgraph_arr_is_fresh_empty(arr)) {
				return true;
			}

			if (HT_FLAGS(arr) & HASH_FLAG_UNINITIALIZED) {
				return false;
			}

			arr_key = (zend_ulong) (uintptr_t) arr;

			if (GC_REFCOUNT(arr) > 1) {
				cached = zend_hash_index_find(direct_verdicts, arr_key);
				if (cached != NULL) {
					return Z_TYPE_P(cached) == IS_TRUE;
				}
			}

			if (HT_IS_PACKED(arr)) {
				for (i = 0; i < arr->nNumUsed; i++) {
					packed_val = &arr->arPacked[i];
					if (!ucache_sgraph_can_copy_verbatim_val(direct_verdicts, packed_val)) {
						result = false;

						break;
					}
				}
			} else {
				bucket = arr->arData;

				for (i = 0; i < arr->nNumUsed; i++) {
					if (Z_TYPE(bucket[i].val) != IS_UNDEF &&
						!ucache_sgraph_can_copy_verbatim_val(direct_verdicts, &bucket[i].val)
					) {
						result = false;

						break;
					}
				}
			}

			if (GC_REFCOUNT(arr) > 1 || !result) {
				ZVAL_BOOL(&verdict, result);

				zend_hash_index_add(direct_verdicts, arr_key, &verdict);
			}

			return result;
		default:
			return false;
	}
}

static bool ucache_sgraph_can_copy_verbatim_arr(HashTable *direct_verdicts, const zval *val)
{
	const zval *cached;

	if (GC_REFCOUNT(Z_ARRVAL_P(val)) == 1) {
		cached = zend_hash_index_find(direct_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(val));
		if (cached != NULL) {
			return Z_TYPE_P(cached) == IS_TRUE;
		}
	}

	return ucache_sgraph_can_copy_verbatim_val(direct_verdicts, val);
}

static bool ucache_sgraph_calc_verbatim_val(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val)
{
	const zval *packed_val;
	const HashTable *arr;
	const Bucket *bucket;
	zend_ulong arr_key;
	zval *cached, seen_marker;
	uint32_t i;
	size_t data_size;
	bool result = true;

	if (ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(val)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return ucache_sgraph_calc_reserve_str(ctx, Z_STR_P(val));
		case IS_ARRAY:
			arr = Z_ARRVAL_P(val);
			if (ucache_sgraph_arr_is_fresh_empty(arr)) {
				return true;
			}

			if (HT_FLAGS(arr) & HASH_FLAG_UNINITIALIZED) {
				return false;
			}

			arr_key = (zend_ulong) (uintptr_t) arr;
			if (GC_REFCOUNT(arr) > 1) {
				cached = zend_hash_index_find(&ctx->direct_arr_dedup, arr_key);
				if (cached != NULL) {
					return true;
				}
			}

			data_size = ucache_sgraph_verbatim_data_size(arr);
			if (!ucache_sgraph_calc_reserve(ctx, sizeof(zend_array)) ||
				!ucache_sgraph_calc_reserve(ctx, data_size)
			) {
				return false;
			}

			if (HT_IS_PACKED(arr)) {
				for (i = 0; i < arr->nNumUsed; i++) {
					packed_val = &arr->arPacked[i];
					if (!ucache_sgraph_calc_verbatim_val(ctx, packed_val)) {
						result = false;

						break;
					}
				}
			} else {
				bucket = arr->arData;
				for (i = 0; i < arr->nNumUsed; i++) {
					if (bucket[i].key != NULL &&
						!ucache_sgraph_calc_reserve_str(ctx, bucket[i].key)
					) {
						result = false;

						break;
					}

					if (Z_TYPE(bucket[i].val) != IS_UNDEF &&
						!ucache_sgraph_calc_verbatim_val(ctx, &bucket[i].val)
					) {
						result = false;

						break;
					}
				}
			}

			if (result && GC_REFCOUNT(arr) > 1) {
				ZVAL_TRUE(&seen_marker);

				zend_hash_index_add(&ctx->direct_arr_dedup, arr_key, &seen_marker);
			}

			return result;
		default:
			return false;
	}
}

static bool ucache_sgraph_produce_safe_direct_state(
		const zval *val,
		zval *state,
		const void *producer_arg)
{
	const php_ucache_safe_direct_handlers *handlers = producer_arg;

	if (!handlers->state_serialize(state, val) || Z_TYPE_P(state) != IS_ARRAY) {
		if (!Z_ISUNDEF_P(state)) {
			zval_ptr_dtor(state);

			ZVAL_UNDEF(state);
		}

		if (!EG(exception)) {
			zend_type_error(
				"The state of the %s object cannot be stored in the user cache",
				ZSTR_VAL(Z_OBJCE_P(val)->name)
			);
		}

		return false;
	}

	return true;
}

static HashTable *ucache_sgraph_copy_with_req_keys(HashTable *props)
{
	HashTable *copy = zend_new_array(zend_hash_num_elements(props));
	zend_string *key, *req_key;
	zend_ulong idx;
	zval *val;

	ZEND_HASH_FOREACH_KEY_VAL(props, idx, key, val) {
		Z_TRY_ADDREF_P(val);

		if (key == NULL) {
			zend_hash_index_add_new(copy, idx, val);

			continue;
		}

		req_key = ZSTR_IS_INTERNED(key) || !(GC_FLAGS(key) & IS_STR_PERSISTENT)
			? zend_string_copy(key)
			: zend_string_init(ZSTR_VAL(key), ZSTR_LEN(key), false)
		;
		zend_hash_add_new(copy, req_key, val);

		zend_string_release(req_key);
	} ZEND_HASH_FOREACH_END();

	return copy;
}

static uint32_t ucache_sgraph_declared_prop_idx_plus_one(
		zend_class_entry *ce,
		zend_string *name,
		uint32_t pos)
{
	zend_property_info *prop_info;
	uint32_t prop_idx;

	if (ce->type != ZEND_USER_CLASS || ce->properties_info_table == NULL) {
		return UCACHE_DECLARED_PROP_IDX_NONE;
	}

	if (pos < (uint32_t) ce->default_properties_count) {
		prop_info = ce->properties_info_table[pos];
		if (prop_info != NULL &&
			zend_string_equals(prop_info->name, name) &&
			(prop_info->flags & (ZEND_ACC_STATIC|ZEND_ACC_VIRTUAL)) == 0 &&
			prop_info->offset != ZEND_VIRTUAL_PROPERTY_OFFSET &&
			OBJ_PROP_TO_NUM(prop_info->offset) == pos
		) {
			return pos + 1;
		}
	}

	prop_info = ucache_sgraph_declared_prop_info(ce, name);
	if (prop_info == NULL) {
		return UCACHE_DECLARED_PROP_IDX_NONE;
	}

	prop_idx = OBJ_PROP_TO_NUM(prop_info->offset);
	if (prop_idx >= (uint32_t) ce->default_properties_count) {
		return UCACHE_DECLARED_PROP_IDX_NONE;
	}

	return prop_idx + 1;
}

static bool ucache_sgraph_extract_serialize_snapshot(
		const zval *val,
		zval *state,
		const void *producer_arg)
{
	zval obj_zv;

	ZVAL_OBJ(&obj_zv, Z_OBJ_P(val));

	if (php_var_serialize_call_magic_serialize(state, &obj_zv) != SUCCESS) {
		ZVAL_UNDEF(state);

		return false;
	}

	return true;
}

static bool ucache_sgraph_extract_prop_snapshot(
		const zval *val,
		zval *state,
		const void *producer_arg)
{
	zend_ulong num_key;
	zend_string *key;
	zval *prop, elem;
	HashTable *props;
	bool result = true;

	props = zend_get_properties_for((zval *) val, ZEND_PROP_PURPOSE_SERIALIZE);
	if (props == NULL) {
		array_init(state);

		return true;
	}

	array_init_size(state, zend_hash_num_elements(props));

	ZEND_HASH_FOREACH_KEY_VAL(props, num_key, key, prop) {
		if (Z_TYPE_P(prop) == IS_INDIRECT) {
			prop = Z_INDIRECT_P(prop);
			if (Z_TYPE_P(prop) == IS_UNDEF) {
				continue;
			}
		}

		if (Z_ISREF_P(prop) && Z_REFCOUNT_P(prop) == 1) {
			prop = Z_REFVAL_P(prop);
		}

		ZVAL_COPY(&elem, prop);
		if (key != NULL) {
			result = zend_hash_add_new(Z_ARRVAL_P(state), key, &elem) != NULL;
		} else {
			result = zend_hash_index_add_new(Z_ARRVAL_P(state), num_key, &elem) != NULL;
		}

		if (!result) {
			zval_ptr_dtor(&elem);

			break;
		}
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);

	if (!result) {
		zval_ptr_dtor(state);

		ZVAL_UNDEF(state);
	}

	return result;
}

static bool ucache_sgraph_extract_sleep_snapshot(
		const zval *val,
		zval *state,
		const void *producer_arg)
{
	zend_object *obj = Z_OBJ_P(val);
	zval *sleep_zv, obj_zv;
	HashTable *names, *props;
	bool result;

	sleep_zv = zend_hash_find_known_hash(&obj->ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP));
	ZEND_ASSERT(sleep_zv != NULL);

	ZVAL_UNDEF(state);

	names = php_var_serialize_call_sleep(obj, Z_FUNC_P(sleep_zv));
	if (names == NULL) {
		if (!EG(exception)) {
			zend_type_error(
				"%s::__sleep() did not return an array of member names; the object cannot be stored in the user cache",
				ZSTR_VAL(obj->ce->name)
			);
		}

		return false;
	}

	ZVAL_OBJ(&obj_zv, obj);

	props = emalloc(sizeof(HashTable));
	result = php_var_serialize_get_sleep_props(props, &obj_zv, names) == SUCCESS;

	zend_array_release(names);

	if (result && obj->ce->type == ZEND_INTERNAL_CLASS) {
		ZVAL_ARR(state, ucache_sgraph_copy_with_req_keys(props));
	} else if (result) {
		ZVAL_ARR(state, props);
	}

	if (!result || obj->ce->type == ZEND_INTERNAL_CLASS) {
		zend_hash_destroy(props);

		efree(props);
	}

	return result;
}

static bool ucache_sgraph_extract_unserialize_route_snapshot(
		const zval *val,
		zval *state,
		const void *producer_arg)
{
	HashTable *symtable;
	bool produced = ucache_class_has_sleep(Z_OBJCE_P(val))
		? ucache_sgraph_extract_sleep_snapshot(val, state, NULL)
		: ucache_sgraph_extract_prop_snapshot(val, state, NULL)
	;

	if (!produced) {
		return false;
	}

	symtable = zend_proptable_to_symtable(Z_ARRVAL_P(state), false);

	zval_ptr_dtor(state);

	ZVAL_ARR(state, symtable);

	return true;
}

static bool ucache_sgraph_get_memoized_state(
		const zval *val,
		HashTable *state_memo,
		ucache_sgraph_state_producer_t produce_state,
		const void *producer_arg,
		zval **state_ptr)
{
	zend_ulong memo_key;
	zend_object *obj;
	zval *memo_state, owned_state;

	*state_ptr = NULL;

	if (state_memo == NULL) {
		return false;
	}

	obj = Z_OBJ_P(val);
	memo_key = (zend_ulong) (uintptr_t) obj;
	memo_state = zend_hash_index_find(state_memo, memo_key);
	if (memo_state != NULL) {
		if (Z_TYPE_P(memo_state) != IS_ARRAY) {
			return false;
		}

		*state_ptr = memo_state;

		return true;
	}

	ucache_sgraph_state_memo_retain_key(obj);

	ZVAL_UNDEF(&owned_state);

	if (!produce_state(val, &owned_state, producer_arg)) {
		OBJ_RELEASE(obj);

		zval_ptr_dtor(&owned_state);

		return false;
	}

	*state_ptr = zend_hash_index_add_new(state_memo, memo_key, &owned_state);

	return true;
}

static bool ucache_sgraph_get_safe_direct_state(
		const zval *val,
		const php_ucache_safe_direct_handlers *handlers,
		HashTable *state_memo,
		zval *borrowed_state,
		zval *owned_state)
{
	zval *state_ptr;

	ZVAL_UNDEF(owned_state);

	if (state_memo != NULL) {
		if (!ucache_sgraph_get_memoized_state(
				val,
				state_memo,
				ucache_sgraph_produce_safe_direct_state,
				handlers,
				&state_ptr
			)
		) {
			return false;
		}

		ZVAL_COPY_VALUE(borrowed_state, state_ptr);

		return true;
	}

	if (!ucache_sgraph_produce_safe_direct_state(val, owned_state, handlers)) {
		if (!Z_ISUNDEF_P(owned_state)) {
			zval_ptr_dtor(owned_state);

			ZVAL_UNDEF(owned_state);
		}

		return false;
	}

	ZVAL_COPY_VALUE(borrowed_state, owned_state);

	return true;
}

static bool ucache_sgraph_get_serdes_blob(
		const zval *val,
		HashTable *state_memo,
		zend_string **blob_ptr)
{
	zend_ulong memo_key;
	zend_string *blob;
	zend_object *obj = Z_OBJ_P(val);
	zval *memo_state, blob_zv;
	smart_str buf = {0};

	*blob_ptr = NULL;

	if (state_memo == NULL) {
		return false;
	}

	memo_key = (zend_ulong) (uintptr_t) obj;
	memo_state = zend_hash_index_find(state_memo, memo_key);
	if (memo_state != NULL) {
		if (Z_TYPE_P(memo_state) != IS_STRING) {
			return false;
		}

		*blob_ptr = Z_STR_P(memo_state);

		return true;
	}

	ucache_sgraph_state_memo_retain_key(obj);

	if (!ucache_serdes_encode((zval *) val, &buf)) {
		smart_str_free(&buf);

		OBJ_RELEASE(obj);

		return false;
	}

	blob = smart_str_extract(&buf);
	if (ZSTR_LEN(blob) > UINT32_MAX) {
		zend_string_release(blob);

		OBJ_RELEASE(obj);

		return false;
	}

	ZVAL_STR(&blob_zv, blob);
	zend_hash_index_add_new(state_memo, memo_key, &blob_zv);

	*blob_ptr = blob;

	return true;
}

static ucache_sgraph_state_producer_t ucache_sgraph_route_state_producer(
		ucache_obj_route route)
{
	switch (route) {
		case UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE:
		case UCACHE_OBJ_ROUTE_SERIALIZE_PROPS:
			return ucache_sgraph_extract_serialize_snapshot;
		case UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_sgraph_extract_unserialize_route_snapshot;
		case UCACHE_OBJ_ROUTE_SLEEP:
			return ucache_sgraph_extract_sleep_snapshot;
		case UCACHE_OBJ_ROUTE_WAKEUP:
			return ucache_sgraph_extract_prop_snapshot;
		default:
			return NULL;
	}
}

static bool ucache_sgraph_get_route_state(
		const zval *val,
		HashTable *state_memo,
		ucache_obj_route route,
		HashTable **state_arr)
{
	zval *state_ptr;

	if (!ucache_sgraph_get_memoized_state(
			val,
			state_memo,
			ucache_sgraph_route_state_producer(route),
			NULL,
			&state_ptr
		)
	) {
		return false;
	}

	*state_arr = Z_ARRVAL_P(state_ptr);

	return true;
}

static bool ucache_sgraph_calc_magic_state_obj(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val,
		zend_object *obj,
		ucache_obj_route route)
{
	HashTable *state_ht;
	zval state_zv;
	bool result;

	if (!ucache_seen_test_and_add(&ctx->seen_objs, obj)) {
		return true;
	}

	if (!ucache_sgraph_get_route_state(val, ctx->state_memo, route, &state_ht)) {
		return false;
	}

	if (ucache_sgraph_state_arr_fits_schema_memo(ctx->state_memo, state_ht)) {
		result = ucache_sgraph_calc_shaped_state_obj(
			ctx,
			obj->ce,
			state_ht
		);
	} else {
		ZVAL_ARR(&state_zv, state_ht);
		result = ucache_sgraph_calc_reserve(ctx,
			sizeof(ucache_sgraph_serialized_obj)) &&
			ucache_sgraph_calc_reserve_str(ctx, obj->ce->name) &&
			ucache_sgraph_calc_val(ctx, &state_zv)
		;
	}

	return result;
}

static bool ucache_sgraph_calc_sleep_state_obj(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val,
		zend_object *obj,
		ucache_obj_route route)
{
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *prop_val;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!ucache_seen_test_and_add(&ctx->seen_objs, obj)) {
		return true;
	}

	if (!ucache_sgraph_get_route_state(val, ctx->state_memo, route, &props)) {
		return false;
	}

	if (ucache_sgraph_state_arr_fits_schema_memo(ctx->state_memo, props)) {
		result = ucache_sgraph_calc_shaped_state_obj(
			ctx,
			obj->ce,
			props
		);
	} else {
		prop_count = zend_hash_num_elements(props);
		result = ucache_sgraph_calc_reserve(
				ctx,
				sizeof(ucache_sgraph_obj)
			) &&
			ucache_sgraph_calc_reserve_str(ctx, obj->ce->name) &&
			(prop_count == 0 ||
				ucache_sgraph_calc_reserve(
					ctx,
					(size_t) prop_count * sizeof(ucache_sgraph_prop)
				)
			)
		;

		if (result) {
			ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
				resolved_name = prop_name != NULL
					? zend_string_copy(prop_name)
					: zend_long_to_str((zend_long) num_key)
				;

				if (!ucache_sgraph_calc_reserve_str(ctx, resolved_name) ||
					!ucache_sgraph_calc_val(ctx, prop_val)
				) {
					zend_string_release(resolved_name);
					result = false;

					break;
				}

				zend_string_release(resolved_name);
			} ZEND_HASH_FOREACH_END();
		}
	}

	return result;
}

static bool ucache_sgraph_calc_declared_props(
		ucache_sgraph_calc_ctx *ctx,
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers)
{
	zend_string *prop_name;
	zval *prop_val;
	uint32_t slot, prop_count = ucache_sgraph_declared_prop_count(obj, handlers);
	bool result = true;

	if (prop_count != 0 &&
		!ucache_sgraph_calc_reserve(ctx, (size_t) prop_count * sizeof(ucache_sgraph_prop))
	) {
		return false;
	}

	GC_ADDREF(obj);

	for (slot = 0; slot < (uint32_t) obj->ce->default_properties_count; slot++) {
		prop_val = ucache_sgraph_declared_prop(obj, handlers, slot, &prop_name);
		if (prop_val == NULL) {
			continue;
		}

		if (!ucache_sgraph_calc_reserve_str(ctx, prop_name) ||
			!ucache_sgraph_calc_val(ctx, prop_val)
		) {
			result = false;

			break;
		}
	}

	OBJ_RELEASE(obj);

	return result;
}

static bool ucache_sgraph_calc_safe_direct_obj(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val,
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers)
{
	zend_string *prop_name;
	zval *prop_val, *src_val, borrowed_state, state;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!ucache_sgraph_get_safe_direct_state(
			val,
			handlers,
			ctx->state_memo,
			&borrowed_state,
			&state
		)
	) {
		return false;
	}

	if (!ucache_seen_test_and_add(&ctx->seen_objs, obj)) {
		result = true;

		goto done;
	}

	if (!ucache_sgraph_calc_reserve(ctx,
		sizeof(ucache_sgraph_safe_direct_obj)) ||
		!ucache_sgraph_calc_reserve_str(ctx, obj->ce->name) ||
		!ucache_sgraph_calc_val(ctx, &borrowed_state)
	) {
		result = false;

		goto done;
	}

	if (ucache_sgraph_props_unbuilt(obj)) {
		result = ucache_sgraph_calc_declared_props(ctx, obj, handlers);

		goto done;
	}

	props = zend_std_get_properties(obj);
	prop_count = 0;
	result = true;

	if (props == NULL) {
		goto done;
	}

	ZEND_HASH_FOREACH_STR_KEY(props, prop_name) {
		if (prop_name == NULL) {
			result = false;

			break;
		}

		if (!ucache_sgraph_safe_direct_prop_is_internal(handlers, obj, prop_name)) {
			prop_count++;
		}
	} ZEND_HASH_FOREACH_END();

	if (!result) {
		goto done;
	}

	if (prop_count != 0 &&
		!ucache_sgraph_calc_reserve(
			ctx,
			(size_t) prop_count * sizeof(ucache_sgraph_prop)
		)
	) {
		result = false;

		goto done;
	}

	ucache_sgraph_hold_props_across_hooks(obj, props);

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
		if (prop_name == NULL) {
			result = false;

			break;
		}

		src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
			? Z_INDIRECT_P(prop_val)
			: prop_val
		;

		if (ucache_sgraph_safe_direct_prop_is_internal(handlers, obj, prop_name)) {
			continue;
		}

		if (!ucache_sgraph_calc_reserve_str(ctx, prop_name) ||
			!ucache_sgraph_calc_val(ctx, src_val)
		) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	ucache_sgraph_release_held_props(obj, props);

done:
	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool ucache_sgraph_calc_plain_obj(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val,
		zend_object *obj)
{
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	bool result;

	if (!ucache_seen_test_and_add(&ctx->seen_objs, obj)) {
		return true;
	}

	if (!ucache_sgraph_calc_reserve(
			ctx,
			sizeof(ucache_sgraph_obj)
		) ||
		!ucache_sgraph_calc_reserve_str(ctx, obj->ce->name)
	) {
		return false;
	}

	if (obj->handlers != &std_object_handlers) {
		ctx->has_custom_obj_handlers = true;
	}

	if (ucache_sgraph_plain_props_unbuilt(obj)) {
		return ucache_sgraph_calc_declared_props(ctx, obj, NULL);
	}

	props = zend_get_properties_for((zval *) val, ZEND_PROP_PURPOSE_SERIALIZE);
	if (props == NULL) {
		return true;
	}

	result = true;
	if (props->nNumOfElements != 0 &&
		!ucache_sgraph_calc_reserve(
			ctx,
			(size_t) props->nNumOfElements * sizeof(ucache_sgraph_prop)
		)
	) {
		zend_release_properties(props);

		return false;
	}

	GC_ADDREF(obj);

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
		src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
			? Z_INDIRECT_P(prop_val)
			: prop_val
		;

		if (prop_name == NULL ||
			!ucache_sgraph_calc_reserve_str(ctx, prop_name) ||
			!ucache_sgraph_calc_val(ctx, src_val)
		) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);

	OBJ_RELEASE(obj);

	return result;
}

static bool ucache_sgraph_calc_obj(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val)
{
	ucache_obj_route_info route_info;
	zend_string *case_name, *serdes_blob;
	zend_class_entry *ce;
	zend_object *obj;

	obj = Z_OBJ_P(val);
	ce = obj->ce;
	if (ce == zend_ce_closure) {
		ucache_throw_unstorable_obj(ce);

		return false;
	}

	if (!zend_lazy_object_initialized(obj)) {
		if (!EG(exception)) {
			zend_type_error(UCACHE_MSG_LAZY_OBJ_UNSTORABLE);
		}

		return false;
	}

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		if (!ucache_seen_test_and_add(&ctx->enum_dedup, obj)) {
			return true;
		}

		case_name = Z_STR_P(zend_enum_fetch_case_name(obj));

		return ucache_sgraph_calc_reserve(
				ctx,
				sizeof(ucache_sgraph_enum)
			) &&
			ucache_sgraph_calc_reserve_str(ctx, ce->name) &&
			ucache_sgraph_calc_reserve_str(ctx, case_name)
		;
	}

	route_info = ucache_sgraph_classify_obj_route(ce);

	switch (route_info.route) {
		case UCACHE_OBJ_ROUTE_SAFE_DIRECT:
			return ucache_sgraph_calc_safe_direct_obj(ctx, val, obj, route_info.sd_handlers);
		case UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE:
			return ucache_sgraph_calc_magic_state_obj(
				ctx, val, obj, UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE
			);
		case UCACHE_OBJ_ROUTE_SERIALIZE_PROPS:
			return ucache_sgraph_calc_sleep_state_obj(
				ctx, val, obj, UCACHE_OBJ_ROUTE_SERIALIZE_PROPS
			);
		case UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_sgraph_calc_magic_state_obj(
				ctx, val, obj, UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE
			);
		case UCACHE_OBJ_ROUTE_SLEEP:
			return ucache_sgraph_calc_sleep_state_obj(
				ctx, val, obj, UCACHE_OBJ_ROUTE_SLEEP
			);
		case UCACHE_OBJ_ROUTE_WAKEUP:
			return ucache_sgraph_calc_sleep_state_obj(
				ctx, val, obj, UCACHE_OBJ_ROUTE_WAKEUP
			);
		case UCACHE_OBJ_ROUTE_SERDES:
			if (!ucache_seen_test_and_add(&ctx->seen_objs, obj)) {
				return true;
			}

			if (!ucache_sgraph_get_serdes_blob(val, ctx->state_memo, &serdes_blob)) {
				return false;
			}

			return ucache_sgraph_calc_reserve(
				ctx,
				sizeof(ucache_sgraph_serdes_obj) + ZSTR_LEN(serdes_blob)
			);
		case UCACHE_OBJ_ROUTE_PLAIN:
			return ucache_sgraph_calc_plain_obj(ctx, val, obj);
		case UCACHE_OBJ_ROUTE_UNSTORABLE:
			ucache_throw_unstorable_obj(obj->ce);

			return false;
	}

	return false;
}

static bool ucache_sgraph_calc_arr(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val)
{
	HashTable *arr;
	zend_string *key;
	zval *elem, *verdict, verdict_zv;
	bool result, verbatim, verdicts_usable;

	arr = Z_ARRVAL_P(val);

	if (arr->nNumOfElements == 0) {
		if (!ucache_sgraph_arr_is_fresh_empty(arr)) {
			return ucache_sgraph_calc_reserve(ctx, sizeof(ucache_sgraph_arr)) &&
					(
						!ucache_sgraph_next_free_is_wide(arr->nNextFreeElement) ||
						ucache_sgraph_calc_reserve(ctx, sizeof(int64_t))
					)
			;
		}

		return true;
	}

	if (ctx->verbatim_arrs_allowed) {
		if (GC_FLAGS(arr) & IS_ARRAY_IMMUTABLE) {
			return ucache_sgraph_calc_verbatim_val(ctx, val);
		}

		verdicts_usable = ctx->verbatim_verdicts != NULL &&
			(
				ctx->state_memo == NULL ||
				zend_hash_num_elements(ctx->state_memo) == 0
			)
		;
		verdict = verdicts_usable
			? zend_hash_index_find(ctx->verbatim_verdicts, (zend_ulong) (uintptr_t) arr)
			: NULL
		;

		if (verdict != NULL) {
			verbatim = Z_TYPE_P(verdict) == IS_TRUE;
		} else {
			verbatim = ucache_sgraph_can_copy_verbatim_arr(&ctx->direct_verdicts, val);

			if (verdicts_usable) {
				ZVAL_BOOL(&verdict_zv, verbatim);

				zend_hash_index_add(ctx->verbatim_verdicts, (zend_ulong) (uintptr_t) arr, &verdict_zv);
			}
		}

		if (verbatim) {
			return ucache_sgraph_calc_verbatim_val(ctx, val);
		}
	}

	result = true;

	GC_TRY_ADDREF(arr);

	if (ucache_sgraph_arr_has_shape(arr)) {
		if (!ucache_sgraph_calc_reserve(
				ctx,
				sizeof(ucache_sgraph_shaped_arr)
			) ||
			!ucache_sgraph_calc_reserve(
				ctx,
				(size_t) arr->nNumOfElements * sizeof(ucache_sgraph_val)
			) ||
			!ucache_sgraph_calc_arr_shape(ctx, arr)
		) {
			result = false;

			goto done;
		}

		ZEND_HASH_FOREACH_VAL(arr, elem) {
			if (!ucache_sgraph_calc_val(ctx, elem)) {
				result = false;

				break;
			}
		} ZEND_HASH_FOREACH_END();

		goto done;
	}

	if (!ucache_sgraph_calc_reserve_dynamic_arr_as_mixed(ctx, arr)) {
		result = false;

		goto done;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(arr, key, elem) {
		if (key != NULL && !ucache_sgraph_calc_reserve_str(ctx, key)) {
			result = false;

			break;
		}

		if (!ucache_sgraph_calc_val(ctx, elem)) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

done:
	ucache_sgraph_release_traversed_arr(arr);

	return result;
}

static bool ucache_sgraph_calc_ref(
		ucache_sgraph_calc_ctx *ctx,
		zend_reference *ref)
{
	zval *reached_again;
	bool held_once, result;

	held_once = ucache_sgraph_ref_is_held_once(ref);

	reached_again = zend_hash_index_lookup(&ctx->seen_refs, (zend_ulong) (uintptr_t) ref);
	if (Z_TYPE_P(reached_again) != IS_NULL) {
		ZVAL_TRUE(reached_again);

		return true;
	}

	ZVAL_FALSE(reached_again);

	result = ucache_sgraph_calc_reserve(ctx, sizeof(ucache_sgraph_ref)) &&
		ucache_sgraph_calc_val(ctx, &ref->val)
	;

	if (held_once) {
		reached_again = zend_hash_index_find(&ctx->seen_refs, (zend_ulong) (uintptr_t) ref);
		if (Z_TYPE_P(reached_again) == IS_FALSE) {
			zend_hash_index_del(&ctx->seen_refs, (zend_ulong) (uintptr_t) ref);
		}
	}

	return result;
}

static bool ucache_sgraph_calc_val(
		ucache_sgraph_calc_ctx *ctx,
		const zval *val)
{
	if (ucache_stack_overflowed()) {
		if (!EG(exception)) {
			zend_type_error(UCACHE_MSG_NESTED_TOO_DEEPLY);
		}

		return false;
	}

	switch (Z_TYPE_P(val)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return true;
		case IS_LONG:
			return ucache_sgraph_long_is_inline(Z_LVAL_P(val)) ||
				ucache_sgraph_calc_reserve(ctx, sizeof(int64_t))
			;
		case IS_DOUBLE:
			return ucache_sgraph_calc_reserve(ctx, sizeof(double));
		case IS_STRING:
			return ucache_sgraph_calc_reserve_str(ctx, Z_STR_P(val));
		case IS_RESOURCE:
			ucache_throw_unstorable_res();

			return false;
		case IS_ARRAY:
			return ucache_sgraph_calc_arr(ctx, val);
		case IS_OBJECT:
			return ucache_sgraph_calc_obj(ctx, val);
		case IS_REFERENCE:
			return ucache_sgraph_calc_ref(ctx, Z_REF_P(val));
		default:
			return false;
	}
}

static void ucache_sgraph_copy_init(
		ucache_sgraph_copy_ctx *ctx,
		uint8_t *buf,
		size_t size)
{
	ctx->buf = buf;
	ctx->size = size;
	ctx->pos = 0;
	ctx->fixup_offsets = NULL;
	ctx->fixup_count = 0;
	ctx->fixup_capacity = 0;

	zend_hash_init(&ctx->seen_objs, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_refs, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->str_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->arr_shape_dedup, 8, NULL, ucache_sgraph_shape_dtor, 0);
	zend_hash_init(&ctx->state_schema_dedup, 8, NULL, ucache_sgraph_shape_dtor, 0);
	zend_hash_init(&ctx->direct_arr_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->enum_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->identity_pins, 8, NULL, ZVAL_PTR_DTOR, 0);

	ctx->has_shared_identity = false;
	ctx->has_obj = false;
	ctx->prefers_proto = false;
	ctx->has_userland_restore_obj = false;
	ctx->has_verbatim_arr = false;
	ctx->verbatim_arrs_allowed = ucache_sgraph_can_use_verbatim_arrs();
	ctx->verbatim_verdicts = NULL;
	ctx->pin_word_count = ucache_sgraph_active_pin_word_count();
}

static void ucache_sgraph_copy_destroy(ucache_sgraph_copy_ctx *ctx)
{
	if (ctx->fixup_offsets != NULL) {
		efree(ctx->fixup_offsets);

		ctx->fixup_offsets = NULL;
	}

	zend_hash_destroy(&ctx->enum_dedup);
	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->direct_arr_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->arr_shape_dedup);
	zend_hash_destroy(&ctx->str_dedup);
	zend_hash_destroy(&ctx->seen_refs);
	zend_hash_destroy(&ctx->seen_objs);
	zend_hash_destroy(&ctx->identity_pins);
}

static bool ucache_sgraph_seen_record_obj_offsets(
		ucache_sgraph_copy_ctx *ctx,
		zend_object *obj,
		uint32_t obj_offset,
		uint32_t flags_offset)
{
	zval pin;
	uint32_t delta;

	delta = flags_offset - obj_offset;

	ZEND_ASSERT((obj_offset & 3) == 0);
	ZEND_ASSERT((delta == 4 || delta == 12) && "unexpected flags field offset");

	if (zend_hash_index_add_ptr(
			&ctx->seen_objs,
			(zend_ulong) (uintptr_t) obj,
			(void *) (uintptr_t) (obj_offset | (delta >> 2))
		) == NULL
	) {
		return false;
	}

	ZVAL_OBJ_COPY(&pin, obj);
	zend_hash_next_index_insert_new(&ctx->identity_pins, &pin);

	return true;
}

static bool ucache_sgraph_copy_emit_obj_ref_if_seen(
		ucache_sgraph_copy_ctx *ctx,
		const zend_object *obj,
		ucache_sgraph_val *dst)
{
	uint32_t packed, obj_offset, flags_offset;
	void *seen_offset;

	seen_offset = zend_hash_index_find_ptr(&ctx->seen_objs, (zend_ulong) (uintptr_t) obj);
	if (seen_offset == NULL) {
		return false;
	}

	packed = (uint32_t) (uintptr_t) seen_offset;
	obj_offset = packed & ~(uint32_t) 3;
	flags_offset = obj_offset + ((packed & 3) << 2);

	*(uint32_t *) (ctx->buf + flags_offset) |= UCACHE_SGRAPH_OBJ_FLAG_SHARED;
	ctx->has_shared_identity = true;

	dst->type = UCACHE_SGRAPH_VAL_OBJ_REF;
	dst->offset = obj_offset;

	return true;
}

static bool ucache_sgraph_copy_alloc(
		ucache_sgraph_copy_ctx *ctx,
		size_t amount,
		uint32_t *offset)
{
	size_t aligned_amount;

	aligned_amount = UCACHE_ALIGNED_SIZE(amount);
	if (ctx->pos > ctx->size || aligned_amount > ctx->size - ctx->pos) {
		return false;
	}

	*offset = (uint32_t) ctx->pos;

	if (aligned_amount > amount) {
		memset(ctx->buf + ctx->pos + amount, 0, aligned_amount - amount);
	}

	ctx->pos += aligned_amount;

	return true;
}

static bool ucache_sgraph_copy_str(
		ucache_sgraph_copy_ctx *ctx,
		const zend_string *str,
		uint32_t *offset)
{
	zend_string *new_str;
	zval *cached;
	uint32_t str_offset;
	size_t str_size;

	cached = zend_hash_lookup(&ctx->str_dedup, (zend_string *) str);
	if (Z_TYPE_P(cached) == IS_LONG) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		return true;
	}

	str_size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(str));
	if (!ucache_sgraph_copy_alloc(ctx, str_size, &str_offset)) {
		return false;
	}

	new_str = (zend_string *) (ctx->buf + str_offset);

	memcpy(new_str, str, str_size);

	GC_SET_REFCOUNT(new_str, 2);
	GC_TYPE_INFO(new_str) = GC_STRING |
		((IS_STR_INTERNED | (ZSTR_IS_VALID_UTF8(str) ? IS_STR_VALID_UTF8 : 0)) << GC_FLAGS_SHIFT)
	;

	*offset = str_offset;

	ZVAL_LONG(cached, (zend_long) str_offset);

	return true;
}

static bool ucache_sgraph_copy_arr_shape(
		ucache_sgraph_copy_ctx *ctx,
		const HashTable *arr,
		uint32_t *offset)
{
	ucache_sgraph_arr_shape *gshape;
	ucache_sgraph_arr_shape_elem *shape_elem;
	ucache_sgraph_shape *shape;
	zend_string *key;
	zend_ulong hash;
	uint32_t shape_offset, elems_offset, key_offset;
	bool collision;

	hash = ucache_sgraph_arr_shape_hash(arr, NULL);
	shape = ucache_sgraph_shape_find(&ctx->arr_shape_dedup, hash, NULL, arr, &collision);
	if (shape != NULL) {
		*offset = shape->offset;

		return true;
	}

	if (!ucache_sgraph_copy_alloc(ctx, sizeof(*gshape), &shape_offset) ||
		!ucache_sgraph_copy_alloc(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(*shape_elem),
			&elems_offset
		)
	) {
		return false;
	}

	gshape = (ucache_sgraph_arr_shape *) (ctx->buf + shape_offset);
	gshape->count = (uint32_t) arr->nNumOfElements;
	gshape->elems_offset = elems_offset;

	shape_elem = (ucache_sgraph_arr_shape_elem *) (ctx->buf + elems_offset);

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		ZEND_ASSERT(key != NULL);

		if (!ucache_sgraph_copy_str(ctx, key, &key_offset)) {
			return false;
		}

		shape_elem->key_offset = key_offset;

		++shape_elem;
	} ZEND_HASH_FOREACH_END();

	if (!collision) {
		ucache_sgraph_shape_add(&ctx->arr_shape_dedup, hash, NULL, arr, shape_offset);
	}

	*offset = shape_offset;

	return true;
}

static bool ucache_sgraph_copy_state_schema(
		ucache_sgraph_copy_ctx *ctx,
		zend_class_entry *ce,
		const HashTable *arr,
		uint32_t *offset,
		ucache_sgraph_shape **dedup_entry)
{
	ucache_sgraph_state_schema *schema;
	ucache_sgraph_shape *shape;
	zend_ulong hash;
	uint32_t schema_offset, class_name_offset, shape_offset;
	bool collision;

	hash = ucache_sgraph_arr_shape_hash(arr, ce);
	shape = ucache_sgraph_shape_find(&ctx->state_schema_dedup, hash, ce, arr, &collision);
	*dedup_entry = shape;
	if (shape != NULL) {
		*offset = shape->offset;

		return true;
	}

	if (!ucache_sgraph_copy_alloc(ctx, sizeof(*schema), &schema_offset) ||
		!ucache_sgraph_copy_str(ctx, ce->name, &class_name_offset) ||
		!ucache_sgraph_copy_arr_shape(ctx, arr, &shape_offset)
	) {
		return false;
	}

	schema = (ucache_sgraph_state_schema *) (ctx->buf + schema_offset);
	schema->class_name_offset = class_name_offset;
	schema->shape_offset = shape_offset;
	schema->count = (uint32_t) arr->nNumOfElements;

	if (!collision) {
		*dedup_entry = ucache_sgraph_shape_add(&ctx->state_schema_dedup, hash, ce, arr, schema_offset);
	}

	*offset = schema_offset;

	return true;
}

static bool ucache_sgraph_copy_shaped_state_vals(
		ucache_sgraph_copy_ctx *ctx,
		const HashTable *state_arr,
		uint32_t vals_offset,
		uint32_t count)
{
	ucache_sgraph_val *gvals;
	zval *elem;
	uint32_t i;

	gvals = (ucache_sgraph_val *) (ctx->buf + vals_offset);
	i = 0;

	ZEND_HASH_FOREACH_VAL((HashTable *) state_arr, elem) {
		if (i == count || !ucache_sgraph_copy_val(ctx, elem, &gvals[i])) {
			return false;
		}

		++i;
	} ZEND_HASH_FOREACH_END();

	return i == count;
}

static void ucache_sgraph_copy_sleep_slot_hints(
		ucache_sgraph_copy_ctx *ctx,
		ucache_sgraph_shape *schema_entry,
		zend_class_entry *ce,
		const HashTable *state_arr,
		uint32_t vals_offset)
{
	const ucache_sgraph_val *known_vals;
	ucache_sgraph_val *gvals;
	zend_string *key;
	uint32_t i = 0;

	ZEND_ASSERT(vals_offset != 0);

	gvals = (ucache_sgraph_val *) (ctx->buf + vals_offset);

	if (schema_entry != NULL && schema_entry->sleep_slots_offset != 0) {
		known_vals = (const ucache_sgraph_val *) (ctx->buf + schema_entry->sleep_slots_offset);
		for (i = 0; i < schema_entry->count; i++) {
			gvals[i].sleep_slot_plus_one = known_vals[i].sleep_slot_plus_one;
		}

		return;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY((HashTable *) state_arr, key) {
		gvals[i].sleep_slot_plus_one = ucache_sgraph_sleep_slot_hint(ce, key, i);
		i++;
	} ZEND_HASH_FOREACH_END();

	if (schema_entry != NULL) {
		schema_entry->sleep_slots_offset = vals_offset;
	}
}

static bool ucache_sgraph_copy_verbatim_compacted_bucket(
		ucache_sgraph_copy_ctx *ctx,
		zend_array *target,
		uint32_t idx,
		zend_ulong h,
		zend_string *key,
		const zval *val)
{
	Bucket *dst_bucket = &target->arData[idx];
	uint32_t key_offset, hash_slot;

	dst_bucket->h = h;
	dst_bucket->key = NULL;

	if (key != NULL) {
		if (!ucache_sgraph_copy_str(ctx, key, &key_offset)) {
			return false;
		}

		dst_bucket->key = (zend_string *) (void *) (ctx->buf + key_offset);

		ucache_sgraph_copy_record_fixup(ctx, &dst_bucket->key);
	}

	if (!ucache_sgraph_copy_verbatim_val(ctx, val, &dst_bucket->val)) {
		return false;
	}

	hash_slot = (uint32_t) h | target->nTableMask;
	Z_NEXT(dst_bucket->val) = HT_HASH(target, hash_slot);
	HT_HASH(target, hash_slot) = HT_IDX_TO_HASH(idx);

	return true;
}

static bool ucache_sgraph_copy_verbatim_compacted_buckets(
		ucache_sgraph_copy_ctx *ctx,
		const HashTable *src_arr,
		zend_array *target)
{
	const Bucket *src_bucket;
	uint32_t i, idx = 0;

	if (HT_IS_PACKED(src_arr)) {
		HT_FLAGS(target) &= ~HASH_FLAG_PACKED;

		for (i = 0; i < src_arr->nNumUsed; i++) {
			if (Z_TYPE(src_arr->arPacked[i]) == IS_UNDEF) {
				continue;
			}

			if (!ucache_sgraph_copy_verbatim_compacted_bucket(ctx, target, idx, i, NULL, &src_arr->arPacked[i])) {
				return false;
			}

			idx++;
		}

		return true;
	}

	for (i = 0; i < src_arr->nNumUsed; i++) {
		src_bucket = &src_arr->arData[i];
		if (Z_TYPE(src_bucket->val) == IS_UNDEF) {
			continue;
		}

		if (!ucache_sgraph_copy_verbatim_compacted_bucket(
				ctx,
				target,
				idx,
				src_bucket->h,
				src_bucket->key,
				&src_bucket->val
			)
		) {
			return false;
		}

		idx++;
	}

	return true;
}

static bool ucache_sgraph_copy_verbatim_val(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zval *dst)
{
	const zval *src_packed;
	const HashTable *src_arr;
	const Bucket *src_bucket;
	zend_ulong arr_key;
	zend_array *target;
	zval *dst_packed, *cached, cached_offset, pin;
	Bucket *dst_bucket;
	uint32_t i, str_offset, arr_offset, data_offset, key_offset;
	size_t data_size;
	bool result = true, compact;

	if (ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_TRUE:
		case IS_FALSE:
			Z_LVAL_P(dst) = 0;
			Z_TYPE_INFO_P(dst) = Z_TYPE_P(src);

			return true;
		case IS_LONG:
			ZVAL_LONG(dst, Z_LVAL_P(src));

			return true;
		case IS_DOUBLE:
			ZVAL_DOUBLE(dst, Z_DVAL_P(src));

			return true;
		case IS_STRING:
			if (!ucache_sgraph_copy_str(ctx, Z_STR_P(src), &str_offset)) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, (zend_string *) (void *) (ctx->buf + str_offset));

			if (ucache_sgraph_ptr_in_range(dst, ctx->buf, ctx->size)) {
				ucache_sgraph_copy_record_fixup(ctx, dst);
			}

			return true;
		case IS_ARRAY:
			src_arr = Z_ARRVAL_P(src);
			if (ucache_sgraph_arr_is_fresh_empty(src_arr)) {
				ZVAL_EMPTY_ARRAY(dst);

				return true;
			}

			if (HT_FLAGS(src_arr) & HASH_FLAG_UNINITIALIZED) {
				return false;
			}

			if (GC_REFCOUNT(src_arr) > 1) {
				cached = zend_hash_index_find(&ctx->direct_arr_dedup, (zend_ulong) (uintptr_t) src_arr);
				if (cached != NULL) {
					ZVAL_ARR(dst, (zend_array *) (void *) (ctx->buf + (uint32_t) Z_LVAL_P(cached)));
					Z_TYPE_FLAGS_P(dst) = 0;

					if (ucache_sgraph_ptr_in_range(dst, ctx->buf, ctx->size)) {
						ucache_sgraph_copy_record_fixup(ctx, dst);
					}

					return true;
				}
			}

			arr_key = (zend_ulong) (uintptr_t) src_arr;
			data_size = ucache_sgraph_verbatim_data_size(src_arr);
			if (!ucache_sgraph_copy_alloc(ctx, sizeof(zend_array), &arr_offset) ||
				!ucache_sgraph_copy_alloc(ctx, data_size, &data_offset)
			) {
				return false;
			}

			target = (zend_array *) (ctx->buf + arr_offset);
			compact = ucache_sgraph_verbatim_arr_needs_compaction(src_arr);

			memcpy(target, src_arr, sizeof(zend_array));

			GC_SET_REFCOUNT(target, 2);
			GC_TYPE_INFO(target) = GC_ARRAY | ((IS_ARRAY_IMMUTABLE | GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT);

			HT_FLAGS(target) |= HASH_FLAG_STATIC_KEYS;
			HT_SET_ITERATORS_COUNT(target, 0);

			target->pDestructor = NULL;
			target->nInternalPointer = 0;

			if (compact) {
				target->nTableSize = ucache_sgraph_verbatim_compact_table_size(src_arr->nNumOfElements);
				target->nTableMask = HT_SIZE_TO_MASK(target->nTableSize);
				target->nNumUsed = src_arr->nNumOfElements;

				HT_SET_DATA_ADDR(target, ctx->buf + data_offset);
				HT_HASH_RESET(target);
			} else {
				memcpy(ctx->buf + data_offset, HT_GET_DATA_ADDR(src_arr), data_size);

				HT_SET_DATA_ADDR(target, ctx->buf + data_offset);
			}

			ucache_sgraph_copy_record_fixup(ctx, &target->arData);

			if (compact) {
				result = ucache_sgraph_copy_verbatim_compacted_buckets(ctx, src_arr, target);
			} else if (HT_IS_PACKED(src_arr)) {
				dst_packed = target->arPacked;
				for (i = 0; i < src_arr->nNumUsed; i++) {
					src_packed = &src_arr->arPacked[i];

					memset(&dst_packed[i], 0, sizeof(zval));

					if (!ucache_sgraph_copy_verbatim_val(ctx, src_packed, &dst_packed[i])) {
						result = false;

						break;
					}
				}
			} else {
				src_bucket = src_arr->arData;
				dst_bucket = target->arData;

				for (i = 0; i < src_arr->nNumUsed; i++) {
					if (src_bucket[i].key != NULL) {
						if (!ucache_sgraph_copy_str(
								ctx,
								src_bucket[i].key,
								&key_offset
							)
						) {
							result = false;

							break;
						}

						dst_bucket[i].key = (zend_string *) (void *) (ctx->buf + key_offset);

						ucache_sgraph_copy_record_fixup(ctx, &dst_bucket[i].key);
					} else {
						dst_bucket[i].key = NULL;
					}

					if (!ucache_sgraph_copy_verbatim_val(ctx, &src_bucket[i].val, &dst_bucket[i].val)) {
						result = false;

						break;
					}
				}
			}

			if (!result) {
				return false;
			}

			if (GC_REFCOUNT(src_arr) > 1) {
				ZVAL_LONG(&cached_offset, (zend_long) arr_offset);

				zend_hash_index_add(&ctx->direct_arr_dedup, arr_key, &cached_offset);

				ZVAL_COPY(&pin, src);
				zend_hash_next_index_insert_new(&ctx->identity_pins, &pin);
			}

			ZVAL_ARR(dst, (zend_array *) (void *) (ctx->buf + arr_offset));
			Z_TYPE_FLAGS_P(dst) = 0;

			if (ucache_sgraph_ptr_in_range(dst, ctx->buf, ctx->size)) {
				ucache_sgraph_copy_record_fixup(ctx, dst);
			}

			return true;
		default:
			return false;
	}
}

static bool ucache_sgraph_copy_shaped_state_obj(
		ucache_sgraph_copy_ctx *ctx,
		zend_object *obj,
		const HashTable *state_arr,
		uint8_t node_type,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_shaped_state_obj *gsstate;
	ucache_sgraph_shape *schema_entry;
	uint32_t sstate_offset, state_schema_offset, state_vals_offset,
		count = state_arr->nNumOfElements
	;

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(ucache_sgraph_shaped_state_obj),
			&sstate_offset
		) ||
		!ucache_sgraph_copy_state_schema(
			ctx,
			obj->ce,
			state_arr,
			&state_schema_offset,
			&schema_entry
		) ||
		!ucache_sgraph_copy_alloc(
			ctx,
			(size_t) count * sizeof(ucache_sgraph_val),
			&state_vals_offset
		)
	) {
		return false;
	}

	if (!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			sstate_offset,
			sstate_offset + offsetof(ucache_sgraph_shaped_state_obj, flags)
		)
	) {
		return false;
	}

	gsstate = (ucache_sgraph_shaped_state_obj *)
		(ctx->buf + sstate_offset)
	;
	gsstate->state_schema_offset = state_schema_offset;
	gsstate->flags = 0;
	gsstate->state_vals_offset = state_vals_offset;
	gsstate->state_next_free =
		ucache_sgraph_shape_next_free_encode(state_arr->nNextFreeElement)
	;

	if (!ucache_sgraph_copy_shaped_state_vals(
			ctx,
			state_arr,
			state_vals_offset,
			count
		)
	) {
		return false;
	}

	if (node_type == UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ) {
		ucache_sgraph_copy_sleep_slot_hints(
			ctx,
			schema_entry,
			obj->ce,
			state_arr,
			state_vals_offset
		);
	}

	dst->type = node_type;
	dst->offset = sstate_offset;

	return true;
}

static bool ucache_sgraph_copy_magic_state_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zend_object *obj,
		ucache_obj_route route,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_serialized_obj *gser;
	HashTable *sd_state_ht;
	zval sd_state_zv;
	uint32_t sd_offset, sd_class_offset;
	bool result;

	ctx->has_userland_restore_obj = true;

	if (ucache_sgraph_copy_emit_obj_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	if (!ucache_sgraph_get_route_state(src, ctx->state_memo, route, &sd_state_ht)) {
		return false;
	}

	if (ucache_sgraph_state_arr_fits_schema_memo(ctx->state_memo, sd_state_ht)) {
		result = ucache_sgraph_copy_shaped_state_obj(
			ctx,
			obj,
			sd_state_ht,
			UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ,
			dst
		);

		goto done;
	}

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(ucache_sgraph_serialized_obj), &sd_offset
		) ||
		!ucache_sgraph_copy_str(ctx, obj->ce->name, &sd_class_offset)
	) {
		result = false;

		goto done;
	}

	if (!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(ucache_sgraph_serialized_obj, flags)
		)
	) {
		result = false;

		goto done;
	}

	gser = (ucache_sgraph_serialized_obj *) (ctx->buf + sd_offset);
	gser->class_name_offset = sd_class_offset;
	gser->flags = 0;

	ZVAL_ARR(&sd_state_zv, sd_state_ht);
	if (!ucache_sgraph_copy_val(ctx, &sd_state_zv, &gser->state)) {
		result = false;

		goto done;
	}

	dst->type = UCACHE_SGRAPH_VAL_SERIALIZED_OBJ;
	dst->offset = sd_offset;
	result = true;

done:
	return result;
}

static bool ucache_sgraph_copy_sleep_state_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zend_object *obj,
		ucache_obj_route route,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_obj *gobj;
	ucache_sgraph_prop *gprops;
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *prop_val;
	HashTable *props;
	uint32_t obj_offset, class_name_offset, props_offset,
		prop_idx, prop_count
	;
	uint16_t slot_hint;
	bool result;

	if (route != UCACHE_OBJ_ROUTE_SERIALIZE_PROPS ||
		zend_hash_find_known_hash(
			&obj->ce->function_table,
			ZSTR_KNOWN(ZEND_STR_WAKEUP)
		) != NULL
	) {
		ctx->has_userland_restore_obj = true;
	}

	if (ucache_sgraph_copy_emit_obj_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	if (!ucache_sgraph_get_route_state(src, ctx->state_memo, route, &props)) {
		return false;
	}

	if (ucache_sgraph_state_arr_fits_schema_memo(ctx->state_memo, props)) {
		result = ucache_sgraph_copy_shaped_state_obj(
			ctx,
			obj,
			props,
			UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ,
			dst
		);

		goto done;
	}

	prop_count = zend_hash_num_elements(props);
	props_offset = 0;

	if (!ucache_sgraph_copy_alloc(ctx, sizeof(*gobj), &obj_offset) ||
		!ucache_sgraph_copy_str(ctx, obj->ce->name, &class_name_offset) ||
		(prop_count != 0 &&
			!ucache_sgraph_copy_alloc(
				ctx,
				(size_t) prop_count * sizeof(ucache_sgraph_prop),
				&props_offset
			)
		)
	) {
		result = false;

		goto done;
	}

	if (!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(ucache_sgraph_obj, flags)
		)
	) {
		result = false;

		goto done;
	}

	gobj = (ucache_sgraph_obj *) (ctx->buf + obj_offset);
	gobj->class_name_offset = class_name_offset;
	gobj->prop_count = prop_count;
	gobj->props_offset = props_offset;
	gobj->flags = 0;

	if (prop_count == 0) {
		dst->type = UCACHE_SGRAPH_VAL_SLEEP_OBJ;
		dst->offset = obj_offset;
		result = true;

		goto done;
	}

	gprops = (ucache_sgraph_prop *) (ctx->buf + props_offset);
	prop_idx = 0;
	result = true;

	ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
		if (prop_idx == prop_count) {
			result = false;

			break;
		}

		resolved_name = prop_name != NULL
			? zend_string_copy(prop_name)
			: zend_long_to_str((zend_long) num_key)
		;

		gprops[prop_idx].name_offset = 0;
		gprops[prop_idx].val.type = UCACHE_SGRAPH_VAL_UNDEF;

		slot_hint = ucache_sgraph_sleep_slot_hint(obj->ce, resolved_name, prop_idx);

		if (!ucache_sgraph_copy_str(
				ctx,
				resolved_name,
				&gprops[prop_idx].name_offset
			) ||
			!ucache_sgraph_copy_val(
				ctx,
				prop_val,
				&gprops[prop_idx].val
			)
		) {
			zend_string_release(resolved_name);
			result = false;

			break;
		}

		gprops[prop_idx].val.sleep_slot_plus_one = slot_hint;

		zend_string_release(resolved_name);

		++prop_idx;
	} ZEND_HASH_FOREACH_END();

	if (result && prop_idx == prop_count) {
		dst->type = UCACHE_SGRAPH_VAL_SLEEP_OBJ;
		dst->offset = obj_offset;
	} else {
		result = false;
	}

done:
	return result;
}

static bool ucache_sgraph_copy_declared_props(
		ucache_sgraph_copy_ctx *ctx,
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers,
		uint32_t *props_offset,
		uint32_t *prop_count)
{
	ucache_sgraph_prop *gprops;
	zend_string *prop_name;
	zval *prop_val;
	uint32_t slot, prop_idx = 0;
	bool result = true;

	*prop_count = ucache_sgraph_declared_prop_count(obj, handlers);
	*props_offset = 0;

	if (*prop_count == 0) {
		return true;
	}

	if (!ucache_sgraph_copy_alloc(
			ctx,
			(size_t) *prop_count * sizeof(ucache_sgraph_prop),
			props_offset
		)
	) {
		return false;
	}

	gprops = (ucache_sgraph_prop *) (ctx->buf + *props_offset);

	GC_ADDREF(obj);

	for (slot = 0; slot < (uint32_t) obj->ce->default_properties_count; slot++) {
		prop_val = ucache_sgraph_declared_prop(obj, handlers, slot, &prop_name);
		if (prop_val == NULL) {
			continue;
		}

		if (prop_idx == *prop_count) {
			result = false;

			break;
		}

		memset(&gprops[prop_idx], 0, sizeof(gprops[prop_idx]));

		if (!ucache_sgraph_copy_str(ctx, prop_name, &gprops[prop_idx].name_offset) ||
			!ucache_sgraph_copy_val(ctx, prop_val, &gprops[prop_idx].val)
		) {
			result = false;

			break;
		}

		++prop_idx;
	}

	OBJ_RELEASE(obj);

	return result && prop_idx == *prop_count;
}

static bool ucache_sgraph_copy_safe_direct_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zend_object *obj,
		const php_ucache_safe_direct_handlers *handlers,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_safe_direct_obj *gsd;
	ucache_sgraph_prop *gprops;
	zend_string *prop_name;
	zval *prop_val, *src_val, sd_borrowed_state, sd_state;
	HashTable *props;
	uint32_t str_offset, prop_idx, prop_count,
		sd_offset, sd_class_offset,
		sd_props_offset
	;
	bool result;

	if (handlers->prefer_req_local_proto) {
		ctx->prefers_proto = true;
	}

	if (ucache_sgraph_copy_emit_obj_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	if (!ucache_sgraph_get_safe_direct_state(
			src,
			handlers,
			ctx->state_memo,
			&sd_borrowed_state,
			&sd_state
		)
	) {
		return false;
	}

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(ucache_sgraph_safe_direct_obj),
			&sd_offset
		) ||
		!ucache_sgraph_copy_str(
			ctx,
			obj->ce->name,
			&sd_class_offset
		)
	) {
		result = false;

		goto done;
	}

	if (!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(ucache_sgraph_safe_direct_obj, flags)
		)
	) {
		result = false;

		goto done;
	}

	gsd = (ucache_sgraph_safe_direct_obj *) (ctx->buf + sd_offset);
	gsd->class_name_offset = sd_class_offset;
	gsd->flags = 0;

	if (!ucache_sgraph_copy_val(ctx, &sd_borrowed_state, &gsd->state)) {
		result = false;

		goto done;
	}

	if (ucache_sgraph_props_unbuilt(obj)) {
		result = ucache_sgraph_copy_declared_props(ctx, obj, handlers, &sd_props_offset, &prop_count);
		if (result) {
			gsd->prop_count = prop_count;
			gsd->props_offset = sd_props_offset;
			dst->type = UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ;
			dst->offset = sd_offset;
		}

		goto done;
	}

	props = zend_std_get_properties(obj);
	prop_count = 0;
	result = true;
	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY(props, prop_name) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			if (!ucache_sgraph_safe_direct_prop_is_internal(handlers, obj, prop_name)) {
				prop_count++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (!result) {
		goto done;
	}

	gsd->prop_count = prop_count;
	gsd->props_offset = 0;

	if (prop_count != 0) {
		if (!ucache_sgraph_copy_alloc(
				ctx,
				(size_t) prop_count * sizeof(ucache_sgraph_prop),
				&sd_props_offset
			)
		) {
			result = false;

			goto done;
		}

		gsd->props_offset = sd_props_offset;
		gprops = (ucache_sgraph_prop *) (ctx->buf + sd_props_offset);
		prop_idx = 0;

		ucache_sgraph_hold_props_across_hooks(obj, props);

		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (ucache_sgraph_safe_direct_prop_is_internal(handlers, obj, prop_name)) {
				continue;
			}

			if (prop_idx == prop_count) {
				result = false;

				break;
			}

			memset(&gprops[prop_idx], 0, sizeof(gprops[prop_idx]));

			if (!ucache_sgraph_copy_str(ctx, prop_name, &str_offset)) {
				result = false;

				break;
			}

			gprops[prop_idx].name_offset = str_offset;

			if (!ucache_sgraph_copy_val(
					ctx,
					src_val,
					&gprops[prop_idx].val
				)
			) {
				result = false;

				break;
			}

			++prop_idx;
		} ZEND_HASH_FOREACH_END();

		ucache_sgraph_release_held_props(obj, props);

		if (!result || prop_idx != prop_count) {
			result = false;

			goto done;
		}
	}

	dst->type = UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ;
	dst->offset = sd_offset;

done:
	if (!Z_ISUNDEF(sd_state)) {
		zval_ptr_dtor(&sd_state);
	}

	return result;
}

static bool ucache_sgraph_copy_serdes_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zend_object *obj,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_serdes_obj *gserdes;
	zend_string *serdes_blob;
	uint32_t serdes_offset;

	ctx->has_userland_restore_obj = true;

	if (ucache_sgraph_copy_emit_obj_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	if (!ucache_sgraph_get_serdes_blob(src, ctx->state_memo, &serdes_blob)) {
		return false;
	}

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(ucache_sgraph_serdes_obj) + ZSTR_LEN(serdes_blob),
			&serdes_offset
		)
	) {
		return false;
	}

	if (!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			serdes_offset,
			serdes_offset + offsetof(ucache_sgraph_serdes_obj, flags)
		)
	) {
		return false;
	}

	gserdes = (ucache_sgraph_serdes_obj *) (ctx->buf + serdes_offset);
	gserdes->blob_len = (uint32_t) ZSTR_LEN(serdes_blob);
	gserdes->flags = 0;

	memcpy(gserdes + 1, ZSTR_VAL(serdes_blob), ZSTR_LEN(serdes_blob));

	dst->type = UCACHE_SGRAPH_VAL_SERDES_OBJ;
	dst->offset = serdes_offset;

	return true;
}

static bool ucache_sgraph_copy_plain_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		zend_object *obj,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_obj *gobj;
	ucache_sgraph_prop *gprops;
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	uint32_t obj_offset, class_name_offset, props_offset,
		prop_idx, prop_count
	;
	bool result;

	ctx->prefers_proto = true;

	if (ucache_sgraph_copy_emit_obj_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(*gobj),
			&obj_offset
		) ||
		!ucache_sgraph_seen_record_obj_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(ucache_sgraph_obj, flags)
		) ||
		!ucache_sgraph_copy_str(
			ctx,
			obj->ce->name,
			&class_name_offset
		)
	) {
		return false;
	}

	if (ucache_sgraph_plain_props_unbuilt(obj)) {
		gobj = (ucache_sgraph_obj *) (ctx->buf + obj_offset);
		gobj->class_name_offset = class_name_offset;
		gobj->prop_count = 0;
		gobj->props_offset = 0;
		gobj->flags = 0;

		if (!ucache_sgraph_copy_declared_props(ctx, obj, NULL, &props_offset, &prop_count)) {
			return false;
		}

		gobj->prop_count = prop_count;
		gobj->props_offset = props_offset;
		dst->type = UCACHE_SGRAPH_VAL_OBJ;
		dst->offset = obj_offset;

		return true;
	}

	props = zend_get_properties_for((zval *) src, ZEND_PROP_PURPOSE_SERIALIZE);
	prop_count = props != NULL ? props->nNumOfElements : 0;
	props_offset = 0;

	if (prop_count != 0 &&
		!ucache_sgraph_copy_alloc(
			ctx,
			((size_t) prop_count * sizeof(ucache_sgraph_prop)),
			&props_offset
		)
	) {
		if (props != NULL) {
			zend_release_properties(props);
		}

		return false;
	}

	gobj = (ucache_sgraph_obj *) (ctx->buf + obj_offset);
	gobj->class_name_offset = class_name_offset;
	gobj->prop_count = prop_count;
	gobj->props_offset = props_offset;
	gobj->flags = 0;

	if (prop_count == 0) {
		if (props != NULL) {
			zend_release_properties(props);
		}

		dst->type = UCACHE_SGRAPH_VAL_OBJ;
		dst->offset = obj_offset;

		return true;
	}

	gprops = (ucache_sgraph_prop *) (ctx->buf + props_offset);
	prop_idx = 0;
	result = true;

	GC_ADDREF(obj);

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
		if (prop_name == NULL || prop_idx == prop_count) {
			result = false;

			break;
		}

		gprops[prop_idx].name_offset = 0;
		gprops[prop_idx].val.type = UCACHE_SGRAPH_VAL_UNDEF;

		src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
			? Z_INDIRECT_P(prop_val)
			: prop_val
		;

		if (!ucache_sgraph_copy_str(ctx, prop_name, &gprops[prop_idx].name_offset) ||
			!ucache_sgraph_copy_val(
				ctx,
				src_val,
				&gprops[prop_idx].val
			)
		) {
			result = false;

			break;
		}

		++prop_idx;
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);
	OBJ_RELEASE(obj);

	if (!result || prop_idx != prop_count) {
		return false;
	}

	dst->type = UCACHE_SGRAPH_VAL_OBJ;
	dst->offset = obj_offset;

	return true;
}

static bool ucache_sgraph_copy_enum(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_enum *genum;
	zend_class_entry *ce;
	zend_string *case_name;
	uint32_t enum_offset, enum_class_offset, enum_case_offset;
	void *seen_offset;

	seen_offset = zend_hash_index_find_ptr(&ctx->enum_dedup, (zend_ulong) (uintptr_t) Z_OBJ_P(src));
	if (seen_offset != NULL) {
		dst->type = UCACHE_SGRAPH_VAL_ENUM;
		dst->offset = (uint32_t) (uintptr_t) seen_offset;

		return true;
	}

	ce = Z_OBJCE_P(src);
	case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(src)));

	if (!ucache_sgraph_copy_alloc(ctx,
		sizeof(ucache_sgraph_enum), &enum_offset) ||
		!ucache_sgraph_copy_str(ctx, ce->name, &enum_class_offset) ||
		!ucache_sgraph_copy_str(ctx, case_name, &enum_case_offset)
	) {
		return false;
	}

	genum = (ucache_sgraph_enum *) (ctx->buf + enum_offset);
	genum->class_name_offset = enum_class_offset;
	genum->case_name_offset = enum_case_offset;

	zend_hash_index_add_ptr(
		&ctx->enum_dedup,
		(zend_ulong) (uintptr_t) Z_OBJ_P(src),
		(void *) (uintptr_t) enum_offset
	);

	dst->type = UCACHE_SGRAPH_VAL_ENUM;
	dst->offset = enum_offset;

	return true;
}

static void ucache_sgraph_copy_leave_ref_held_once(
		ucache_sgraph_copy_ctx *ctx,
		zend_reference *ref,
		const ucache_sgraph_ref *gref)
{
	zval pin;

	if (!(gref->flags & UCACHE_SGRAPH_OBJ_FLAG_SHARED)) {
		zend_hash_index_del(&ctx->seen_refs, (zend_ulong) (uintptr_t) ref);

		if (GC_REFCOUNT(ref) > 1) {
			GC_DTOR(ref);

			return;
		}
	}

	ZVAL_REF(&pin, ref);
	zend_hash_next_index_insert_new(&ctx->identity_pins, &pin);
}

static bool ucache_sgraph_copy_ref(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_ref *gref;
	zend_reference *ref;
	zval pin;
	uint32_t shared_offset, ref_offset;
	bool held_once, result;
	void *seen_offset;

	ref = Z_REF_P(src);
	seen_offset = zend_hash_index_find_ptr(&ctx->seen_refs, (zend_ulong) (uintptr_t) ref);

	if (seen_offset != NULL) {
		shared_offset = (uint32_t) (uintptr_t) seen_offset;
		((ucache_sgraph_ref *) (ctx->buf + shared_offset))->flags |=
			UCACHE_SGRAPH_OBJ_FLAG_SHARED
		;

		dst->type = UCACHE_SGRAPH_VAL_REF_REF;
		dst->offset = shared_offset;

		ctx->has_shared_identity = true;

		return true;
	}

	held_once = ucache_sgraph_ref_is_held_once(ref);

	if (!ucache_sgraph_copy_alloc(
			ctx,
			sizeof(ucache_sgraph_ref),
			&ref_offset
		) ||
		zend_hash_index_add_ptr(
			&ctx->seen_refs,
			(zend_ulong) (uintptr_t) ref,
			(void *) (uintptr_t) ref_offset
		) == NULL
	) {
		return false;
	}

	GC_ADDREF(ref);

	if (!held_once) {
		ZVAL_REF(&pin, ref);
		zend_hash_next_index_insert_new(&ctx->identity_pins, &pin);
	}

	gref = (ucache_sgraph_ref *) (ctx->buf + ref_offset);
	gref->flags = 0;

	result = ucache_sgraph_copy_val(ctx, &ref->val, &gref->inner);

	if (held_once) {
		ucache_sgraph_copy_leave_ref_held_once(ctx, ref, gref);
	}

	if (!result) {
		return false;
	}

	dst->type = UCACHE_SGRAPH_VAL_REF;
	dst->offset = ref_offset;

	ctx->has_shared_identity = true;

	return true;
}

static bool ucache_sgraph_copy_arr(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst)
{
	ucache_sgraph_arr *garr;
	ucache_sgraph_arr_elem *gelems, *gelem;
	ucache_sgraph_shaped_arr *gsarr;
	ucache_sgraph_val *gvals;
	zend_ulong h;
	zend_string *key;
	zval *elem, *verdict, arr_val;
	HashTable *arr;
	uint32_t count, elem_idx,
		arr_offset, elems_offset, key_offset,
		shape_offset, vals_offset,
		wide_next_free_offset
	;
	int64_t wide_next_free;
	bool result, verbatim, packed;

	arr = Z_ARRVAL_P(src);
	result = true;

	if (arr->nNumOfElements == 0) {
		if (ucache_sgraph_arr_is_fresh_empty(arr)) {
			dst->type = UCACHE_SGRAPH_VAL_ARR;
			dst->offset = 0;

			return true;
		}

		if (!ucache_sgraph_copy_alloc(ctx, sizeof(*garr), &arr_offset)) {
			return false;
		}

		garr = (ucache_sgraph_arr *) (ctx->buf + arr_offset);
		garr->count = 0;
		garr->elems_offset = 0;
		garr->flags = 0;

		if (UNEXPECTED(ucache_sgraph_next_free_is_wide(arr->nNextFreeElement))) {
			wide_next_free = (int64_t) arr->nNextFreeElement;

			if (!ucache_sgraph_copy_alloc(ctx, sizeof(wide_next_free), &wide_next_free_offset)) {
				return false;
			}

			memcpy(ctx->buf + wide_next_free_offset, &wide_next_free, sizeof(wide_next_free));

			garr->next_free = wide_next_free_offset;
			garr->flags |= UCACHE_SGRAPH_ARR_FLAG_WIDE_NEXT_FREE;
		} else {
			garr->next_free = (uint32_t) arr->nNextFreeElement;
		}

		dst->type = UCACHE_SGRAPH_VAL_DYNAMIC_ARR;
		dst->offset = arr_offset;

		return true;
	}

	if (ctx->verbatim_arrs_allowed) {
		if (GC_FLAGS(arr) & IS_ARRAY_IMMUTABLE) {
			verbatim = true;
		} else {
			verdict = ctx->verbatim_verdicts != NULL
				? zend_hash_index_find(ctx->verbatim_verdicts, (zend_ulong) (uintptr_t) arr)
				: NULL
			;
			if (verdict != NULL) {
				verbatim = Z_TYPE_P(verdict) == IS_TRUE;
			} else {
				verbatim = ucache_sgraph_can_copy_verbatim_arr(&ctx->direct_verdicts, src);
			}
		}

		if (verbatim) {
			if (!ucache_sgraph_copy_verbatim_val(ctx, src, &arr_val)) {
				return false;
			}

			dst->type = UCACHE_SGRAPH_VAL_ARR;
			dst->offset = (uint32_t) ((uint8_t *) Z_ARRVAL(arr_val) - ctx->buf);

			ctx->has_verbatim_arr = true;

			return true;
		}
	}

	count = arr->nNumOfElements;
	elem_idx = 0;

	GC_TRY_ADDREF(arr);

	if (ucache_sgraph_arr_has_shape(arr)) {
		if (!ucache_sgraph_copy_alloc(ctx, sizeof(*gsarr), &arr_offset) ||
			!ucache_sgraph_copy_alloc(
				ctx,
				(size_t) count * sizeof(*gvals),
				&vals_offset
			) ||
			!ucache_sgraph_copy_arr_shape(ctx, arr, &shape_offset)
		) {
			result = false;

			goto done;
		}

		gsarr = (ucache_sgraph_shaped_arr *) (ctx->buf + arr_offset);
		gsarr->count = count;
		gsarr->next_free = ucache_sgraph_shape_next_free_encode(arr->nNextFreeElement);
		gsarr->shape_offset = shape_offset;

		ZEND_ASSERT(vals_offset == ucache_sgraph_shaped_arr_vals_offset(arr_offset));

		gvals = (ucache_sgraph_val *) (ctx->buf + vals_offset);

		ZEND_HASH_FOREACH_VAL(arr, elem) {
			if (elem_idx == count || !ucache_sgraph_copy_val(ctx, elem, &gvals[elem_idx])) {
				result = false;

				break;
			}

			++elem_idx;
		} ZEND_HASH_FOREACH_END();

		if (result && elem_idx == count) {
			dst->type = UCACHE_SGRAPH_VAL_SHAPED_ARR;
			dst->offset = arr_offset;
		} else {
			result = false;
		}

		goto done;
	}

	packed = ctx->packed_vals_allowed && HT_IS_PACKED(arr) && HT_IS_WITHOUT_HOLES(arr);

	if (!ucache_sgraph_copy_alloc(ctx, sizeof(*garr), &arr_offset) ||
		!ucache_sgraph_copy_alloc(
			ctx,
			(size_t) count * (packed ? sizeof(*gvals) : sizeof(*gelems)),
			&elems_offset
		)
	) {
		result = false;

		goto done;
	}

	garr = (ucache_sgraph_arr *) (ctx->buf + arr_offset);
	garr->count = count;
	garr->elems_offset = elems_offset;
	garr->flags = (HT_IS_PACKED(arr) && HT_IS_WITHOUT_HOLES(arr))
		? UCACHE_SGRAPH_ARR_FLAG_PACKED
		: 0
	;

	if (packed) {
		garr->flags |= UCACHE_SGRAPH_ARR_FLAG_PACKED_VALS;
	}

	if (UNEXPECTED(ucache_sgraph_next_free_is_wide(arr->nNextFreeElement))) {
		wide_next_free = (int64_t) arr->nNextFreeElement;

		if (!ucache_sgraph_copy_alloc(ctx, sizeof(wide_next_free), &wide_next_free_offset)) {
			result = false;

			goto done;
		}

		memcpy(ctx->buf + wide_next_free_offset, &wide_next_free, sizeof(wide_next_free));

		garr->next_free = wide_next_free_offset;
		garr->flags |= UCACHE_SGRAPH_ARR_FLAG_WIDE_NEXT_FREE;
	} else {
		garr->next_free = (uint32_t) arr->nNextFreeElement;
	}

	if (packed) {
		gvals = (ucache_sgraph_val *) (ctx->buf + elems_offset);

		ZEND_HASH_PACKED_FOREACH_VAL(arr, elem) {
			if (elem_idx == count || !ucache_sgraph_copy_val(ctx, elem, &gvals[elem_idx])) {
				result = false;

				break;
			}

			++elem_idx;
		} ZEND_HASH_FOREACH_END();
	} else {
		gelems = (ucache_sgraph_arr_elem *) (ctx->buf + elems_offset);

		ZEND_HASH_FOREACH_KEY_VAL(arr, h, key, elem) {
			if (elem_idx == count) {
				result = false;

				break;
			}

			gelem = &gelems[elem_idx];
			memset(gelem, 0, sizeof(*gelem));

			if (key != NULL) {
				if (!ucache_sgraph_copy_str(ctx, key, &key_offset)) {
					result = false;

					break;
				}

				gelem->key = key_offset;
			} else {
				gelem->key = (uint32_t) h;
				gelem->h_hi = (uint32_t) ((uint64_t) h >> 32);
			}

			if (!ucache_sgraph_copy_val(ctx, elem, &gelem->val)) {
				result = false;

				break;
			}

			if (key != NULL) {
				gelem->val.flags = UCACHE_SGRAPH_ELEM_STR_KEY;
			}

			++elem_idx;
		} ZEND_HASH_FOREACH_END();
	}

	if (result && elem_idx == count) {
		dst->type = UCACHE_SGRAPH_VAL_DYNAMIC_ARR;
		dst->offset = arr_offset;
	} else {
		result = false;
	}

done:
	ucache_sgraph_release_traversed_arr(arr);

	return result;
}

static bool ucache_sgraph_copy_obj(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst)
{
	ucache_obj_route_info route_info;
	zend_class_entry *ce;
	zend_object *obj;

	ctx->has_obj = true;
	ce = Z_OBJCE_P(src);

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		return ucache_sgraph_copy_enum(ctx, src, dst);
	}

	obj = Z_OBJ_P(src);

	if (!zend_lazy_object_initialized(obj)) {
		if (!EG(exception)) {
			zend_type_error(UCACHE_MSG_LAZY_OBJ_UNSTORABLE);
		}

		return false;
	}

	route_info = ucache_sgraph_classify_obj_route(obj->ce);

	switch (route_info.route) {
		case UCACHE_OBJ_ROUTE_SAFE_DIRECT:
			return ucache_sgraph_copy_safe_direct_obj(ctx, src, obj, route_info.sd_handlers, dst);
		case UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE:
			return ucache_sgraph_copy_magic_state_obj(
				ctx, src, obj, UCACHE_OBJ_ROUTE_MAGIC_SERIALIZE, dst
			);
		case UCACHE_OBJ_ROUTE_SERIALIZE_PROPS:
			return ucache_sgraph_copy_sleep_state_obj(
				ctx, src, obj, UCACHE_OBJ_ROUTE_SERIALIZE_PROPS, dst
			);
		case UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_sgraph_copy_magic_state_obj(
				ctx, src, obj, UCACHE_OBJ_ROUTE_MAGIC_UNSERIALIZE, dst
			);
		case UCACHE_OBJ_ROUTE_SLEEP:
			return ucache_sgraph_copy_sleep_state_obj(
				ctx, src, obj, UCACHE_OBJ_ROUTE_SLEEP, dst
			);
		case UCACHE_OBJ_ROUTE_WAKEUP:
			return ucache_sgraph_copy_sleep_state_obj(
				ctx, src, obj, UCACHE_OBJ_ROUTE_WAKEUP, dst
			);
		case UCACHE_OBJ_ROUTE_SERDES:
			return ucache_sgraph_copy_serdes_obj(ctx, src, obj, dst);
		case UCACHE_OBJ_ROUTE_PLAIN:
			return ucache_sgraph_copy_plain_obj(ctx, src, obj, dst);
		case UCACHE_OBJ_ROUTE_UNSTORABLE:
			ucache_throw_unstorable_obj(obj->ce);

			return false;
	}

	return false;
}

static bool ucache_sgraph_copy_wide_scalar(
		ucache_sgraph_copy_ctx *ctx,
		uint32_t type,
		const void *bytes,
		ucache_sgraph_val *dst)
{
	uint32_t offset;

	if (!ucache_sgraph_copy_alloc(ctx, sizeof(uint64_t), &offset)) {
		return false;
	}

	memcpy(ctx->buf + offset, bytes, sizeof(uint64_t));

	dst->type = type;
	dst->offset = offset;

	return true;
}

static bool ucache_sgraph_copy_val(
		ucache_sgraph_copy_ctx *ctx,
		const zval *src,
		ucache_sgraph_val *dst)
{
	uint32_t str_offset;
	int64_t wide;
	double dval;

	if (ucache_stack_overflowed()) {
		return false;
	}

	memset(dst, 0, sizeof(*dst));

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
			dst->type = UCACHE_SGRAPH_VAL_UNDEF;

			return true;
		case IS_NULL:
			dst->type = UCACHE_SGRAPH_VAL_NULL;

			return true;
		case IS_TRUE:
			dst->type = UCACHE_SGRAPH_VAL_TRUE;

			return true;
		case IS_FALSE:
			dst->type = UCACHE_SGRAPH_VAL_FALSE;

			return true;
		case IS_LONG:
			if (ucache_sgraph_long_is_inline(Z_LVAL_P(src))) {
				dst->type = UCACHE_SGRAPH_VAL_LONG;
				dst->long_val = (int32_t) Z_LVAL_P(src);

				return true;
			}

			wide = (int64_t) Z_LVAL_P(src);

			return ucache_sgraph_copy_wide_scalar(
				ctx, UCACHE_SGRAPH_VAL_LONG_WIDE, &wide, dst
			);
		case IS_DOUBLE:
			dval = Z_DVAL_P(src);

			return ucache_sgraph_copy_wide_scalar(
				ctx, UCACHE_SGRAPH_VAL_DOUBLE, &dval, dst
			);
		case IS_STRING:
			if (!ucache_sgraph_copy_str(ctx, Z_STR_P(src), &str_offset)) {
				return false;
			}

			dst->type = UCACHE_SGRAPH_VAL_STR;
			dst->offset = str_offset;

			return true;
		case IS_ARRAY:
			return ucache_sgraph_copy_arr(ctx, src, dst);
		case IS_RESOURCE:
			ucache_throw_unstorable_res();

			return false;
		case IS_OBJECT:
			return ucache_sgraph_copy_obj(ctx, src, dst);
		case IS_REFERENCE:
			return ucache_sgraph_copy_ref(ctx, src, dst);
		default:
			return false;
	}
}

static bool ucache_sgraph_copy_root(
		ucache_sgraph_copy_ctx *ctx,
		const zval *val,
		ucache_sgraph_val *root_val)
{
	uint32_t hdr_offset;

	if (!ucache_sgraph_copy_alloc(ctx, UCACHE_SGRAPH_HDR_SIZE(ctx->pin_word_count), &hdr_offset) ||
		hdr_offset != 0 ||
		(Z_TYPE_P(val) != IS_OBJECT && Z_TYPE_P(val) != IS_ARRAY && Z_TYPE_P(val) != IS_STRING) ||
		!ucache_sgraph_copy_val(ctx, val, root_val) ||
		(root_val->offset != 0 && root_val->offset != UCACHE_SGRAPH_ROOT_OFFSET(ctx->pin_word_count))
	) {
		return false;
	}

	switch (root_val->type) {
		case UCACHE_SGRAPH_VAL_OBJ:
		case UCACHE_SGRAPH_VAL_SAFE_DIRECT_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_OBJ:
		case UCACHE_SGRAPH_VAL_SERIALIZED_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_SERDES_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_OBJ:
		case UCACHE_SGRAPH_VAL_SLEEP_SHAPED_OBJ:
		case UCACHE_SGRAPH_VAL_ENUM:
			return Z_TYPE_P(val) == IS_OBJECT;
		case UCACHE_SGRAPH_VAL_ARR:
		case UCACHE_SGRAPH_VAL_DYNAMIC_ARR:
		case UCACHE_SGRAPH_VAL_SHAPED_ARR:
			return Z_TYPE_P(val) == IS_ARRAY;
		case UCACHE_SGRAPH_VAL_STR:
			return Z_TYPE_P(val) == IS_STRING;
		default:
			return false;
	}
}

static void ucache_sgraph_pin_owners_init(ucache_sgraph_hdr *hdr)
{
	uint32_t w;

	for (w = 0; w < MIN(hdr->pin_word_count, UCACHE_GRAPH_PIN_WORDS_MAX); w++) {
		atomic_init(&hdr->pin_owners[w], 0);
	}
}

ZEND_COLD void ucache_throw_unstorable_res(void)
{
	if (!EG(exception)) {
		zend_type_error(UCACHE_MSG_RESOURCE_UNSTORABLE);
	}
}

ZEND_COLD void ucache_throw_unstorable_obj(const zend_class_entry *ce)
{
	if (!EG(exception)) {
		zend_type_error(UCACHE_MSG_OBJ_UNSTORABLE, ZSTR_VAL(ce->name));
	}
}

void ucache_sgraph_obj_route_memo_release(void)
{
	if (UC_G(obj_route_memo) != NULL) {
		zend_hash_destroy(UC_G(obj_route_memo));

		efree(UC_G(obj_route_memo));

		UC_G(obj_route_memo) = NULL;
	}
}

void ucache_sgraph_calc_verbatim_root(
		const zval *val,
		HashTable *verbatim_verdicts,
		size_t *buf_len)
{
	ucache_sgraph_calc_ctx calc_ctx;
	zval verdict_zv;
	bool completed, decided;

	if (!ucache_sgraph_can_use_verbatim_arrs() ||
		Z_ARRVAL_P(val)->nNumOfElements == 0
	) {
		return;
	}

	ucache_sgraph_calc_verbatim_init(&calc_ctx);

	completed = ucache_sgraph_calc_reserve(
			&calc_ctx,
			UCACHE_SGRAPH_HDR_SIZE(calc_ctx.pin_word_count)
		) &&
		ucache_sgraph_calc_verbatim_val(&calc_ctx, val)
	;

	if (completed && calc_ctx.size <= UINT32_MAX - UCACHE_SGRAPH_ALIGNMENT_SLACK) {
		*buf_len = calc_ctx.size + UCACHE_SGRAPH_ALIGNMENT_SLACK;
	}

	decided = completed || !calc_ctx.reserve_failed;

	ucache_sgraph_calc_verbatim_destroy(&calc_ctx);

	if (decided && !(GC_FLAGS(Z_ARRVAL_P(val)) & IS_ARRAY_IMMUTABLE)) {
		ZVAL_BOOL(&verdict_zv, completed);
		zend_hash_index_add(
			verbatim_verdicts,
			(zend_ulong) (uintptr_t) Z_ARRVAL_P(val),
			&verdict_zv
		);
	}
}

bool ucache_calc_sgraph_size(
		const zval *val,
		HashTable *state_memo,
		HashTable *verbatim_verdicts,
		size_t *buf_len,
		bool *packed_vals_allowed)
{
	ucache_sgraph_calc_ctx calc_ctx;
	bool result;

	*buf_len = 0;
	*packed_vals_allowed = false;

	if (Z_TYPE_P(val) != IS_OBJECT &&
		Z_TYPE_P(val) != IS_ARRAY &&
		Z_TYPE_P(val) != IS_STRING
	) {
		return false;
	}

	ucache_sgraph_calc_init(&calc_ctx);

	calc_ctx.verbatim_arrs_allowed = ucache_sgraph_can_use_verbatim_arrs();
	calc_ctx.verbatim_verdicts = verbatim_verdicts;
	calc_ctx.state_memo = state_memo;
	result = ucache_sgraph_calc_reserve(
		&calc_ctx,
		UCACHE_SGRAPH_HDR_SIZE(calc_ctx.pin_word_count)
	);

	if (result) {
		result = ucache_sgraph_calc_val(&calc_ctx, val) && !EG(exception);
	}

	if (result) {
		if (calc_ctx.size > UINT32_MAX - UCACHE_SGRAPH_ALIGNMENT_SLACK) {
			result = false;
		} else {
			calc_ctx.size += UCACHE_SGRAPH_ALIGNMENT_SLACK;
		}
	}

	if (result) {
		*buf_len = calc_ctx.size;
		*packed_vals_allowed = !calc_ctx.has_custom_obj_handlers &&
			state_memo != NULL &&
			zend_hash_num_elements(state_memo) == 0
		;
	}

	ucache_sgraph_calc_destroy(&calc_ctx);

	return result;
}

bool ucache_build_sgraph_in_place(
		const zval *val,
		HashTable *state_memo,
		const HashTable *verbatim_verdicts,
		bool packed_vals_allowed,
		uint8_t *buf,
		size_t buf_len,
		size_t *glen,
		bool *has_verbatim_arr,
		uint32_t **fixup_offsets,
		uint32_t *fixup_count)
{
	ucache_sgraph_copy_ctx copy_ctx;
	ucache_sgraph_hdr *hdr;
	ucache_sgraph_val root_val;
	uint32_t memo_count, attempt;
	size_t padding;
	bool result, protects_gc, gc_was_protected = false;

	if (buf == NULL) {
		return false;
	}

	padding = ucache_sgraph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < UCACHE_SGRAPH_HDR_SIZE(0)) {
		return false;
	}

	if (padding != 0) {
		memset(buf, 0, padding);
	}

	buf += padding;
	buf_len -= padding;

	memo_count = state_memo != NULL ? zend_hash_num_elements(state_memo) : 0;
	protects_gc = state_memo == NULL;

	if (protects_gc) {
		gc_was_protected = gc_protect(true);
	}

	for (attempt = 0; ; attempt++) {
		ucache_sgraph_copy_init(&copy_ctx, buf, buf_len);

		copy_ctx.verbatim_verdicts = attempt == 0 ? verbatim_verdicts : NULL;
		copy_ctx.state_memo = state_memo;
		copy_ctx.packed_vals_allowed = attempt == 0 && packed_vals_allowed;

		result = ucache_sgraph_copy_root(&copy_ctx, val, &root_val);

		if (state_memo == NULL || EG(exception) || zend_hash_num_elements(state_memo) == memo_count) {
			break;
		}

		if (attempt == UCACHE_SGRAPH_COPY_RETRIES) {
			result = false;

			break;
		}

		memo_count = zend_hash_num_elements(state_memo);

		ucache_sgraph_copy_destroy(&copy_ctx);
	}

	if (!result || EG(exception)) {
		ucache_sgraph_copy_destroy(&copy_ctx);

		if (protects_gc) {
			gc_protect(gc_was_protected);
		}

		return false;
	}

	hdr = (ucache_sgraph_hdr *) buf;
	hdr->root_type = (uint8_t) root_val.type;
	hdr->pin_word_count = copy_ctx.pin_word_count;
	hdr->flags =
		(root_val.offset == 0
			? UCACHE_SGRAPH_FLAG_EMPTY_ROOT
			: 0
		) |
		(copy_ctx.has_shared_identity
			? UCACHE_SGRAPH_FLAG_HAS_SHARED_IDENTITY
			: 0
		) |
		(copy_ctx.has_obj
			? UCACHE_SGRAPH_FLAG_HAS_OBJ
			: 0
		) |
		((copy_ctx.prefers_proto && !copy_ctx.has_userland_restore_obj)
			? UCACHE_SGRAPH_FLAG_PREFERS_PROTO
			: 0
		)
	;

	atomic_init(&hdr->ref_state, 0);

	ucache_sgraph_pin_owners_init(hdr);

	if (glen != NULL) {
		*glen = copy_ctx.pos;
	}

	if (has_verbatim_arr != NULL) {
		*has_verbatim_arr = copy_ctx.has_verbatim_arr;
	}

	ZEND_ASSERT(copy_ctx.has_verbatim_arr || copy_ctx.fixup_count == 0);

	if (fixup_offsets != NULL) {
		*fixup_offsets = copy_ctx.fixup_offsets;
		*fixup_count = copy_ctx.fixup_count;

		copy_ctx.fixup_offsets = NULL;
		copy_ctx.fixup_count = 0;
		copy_ctx.fixup_capacity = 0;
	}

	ucache_sgraph_copy_destroy(&copy_ctx);

	if (protects_gc) {
		gc_protect(gc_was_protected);
	}

	return true;
}

bool ucache_sgraph_copy_fits_buf(
		const uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_glen)
{
	size_t dst_padding;

	if (src_buf == NULL ||
		dst_buf == NULL ||
		src_glen == 0 ||
		src_glen > buf_len
	) {
		return false;
	}

	dst_padding = ucache_sgraph_alignment_padding(dst_buf);

	return dst_padding <= buf_len &&
		(src_glen <= buf_len - dst_padding)
	;
}

bool ucache_sgraph_publish_copied_payload_locked(
		uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_glen,
		bool has_verbatim_arr,
		const uint32_t *fixup_offsets,
		uint32_t fixup_count)
{
	const uint8_t *src_base;
	ucache_sgraph_hdr *hdr;
	uint32_t i;
	uint8_t *dst_base;
	uintptr_t delta, ptr;
	size_t src_padding, dst_padding;

	if (!ucache_sgraph_copy_fits_buf(dst_buf, src_buf, buf_len, src_glen)) {
		return false;
	}

	src_padding = ucache_sgraph_alignment_padding(src_buf);
	if (src_padding > buf_len ||
		buf_len - src_padding < src_glen ||
		src_glen < sizeof(*hdr)
	) {
		return false;
	}

	src_base = src_buf + src_padding;
	dst_padding = ucache_sgraph_alignment_padding(dst_buf);
	if (dst_padding != 0) {
		memset(dst_buf, 0, dst_padding);
	}

	dst_base = dst_buf + dst_padding;

	memcpy(dst_base, src_base, src_glen);

	hdr = (ucache_sgraph_hdr *) dst_base;

	atomic_init(&hdr->ref_state, 0);

	ucache_sgraph_pin_owners_init(hdr);

	if (!has_verbatim_arr) {
		ZEND_ASSERT(fixup_count == 0);

#if ZEND_DEBUG
		ucache_sgraph_check_rebase_complete(dst_base, src_glen, src_base);
#endif

		return true;
	}

	ZEND_ASSERT(fixup_offsets != NULL && fixup_count > 0);

	delta = (uintptr_t) dst_base - (uintptr_t) src_base;
	for (i = 0; i < fixup_count; i++) {
		ZEND_ASSERT(fixup_offsets[i] <= src_glen - sizeof(uintptr_t));
		ZEND_ASSERT((((uintptr_t) dst_base + fixup_offsets[i]) & (sizeof(uintptr_t) - 1)) == 0);

		memcpy(&ptr, dst_base + fixup_offsets[i], sizeof(ptr));

		ptr += delta;

		memcpy(dst_base + fixup_offsets[i], &ptr, sizeof(ptr));
	}

#if ZEND_DEBUG
	ucache_sgraph_check_rebase_complete(dst_base, src_glen, src_base);
#endif

	return true;
}
