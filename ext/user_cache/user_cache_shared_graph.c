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

#include "user_cache_internal.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_operators.h"
#include "ext/random/php_random.h"

#define PHP_UCACHE_DECODE_HOT	PHP_UCACHE_HOT

#define PHP_UCACHE_GRAPH_PIN_OWNER_RECLAIMING	(-1)
#define PHP_UCACHE_CONTENT_HASH_STRING_KEY_TAG	(1ULL << 63)

/* Request-local maps preserve identity for repeated graph nodes. */
#define PHP_UCACHE_DEFINE_DECODE_MAP(name, ctype, dtor, release_on_fail) \
	static zend_always_inline void ucache_decode_##name##_map_teardown(void) \
	{ \
		if (UC_G(decode_##name##_map) != NULL) { \
			zend_hash_destroy(UC_G(decode_##name##_map)); \
			efree(UC_G(decode_##name##_map)); \
			UC_G(decode_##name##_map) = NULL; \
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

#define PHP_UCACHE_GRAPH_PIN_PROBE_INTERVAL_SEC	8U
#define PHP_UCACHE_GRAPH_COLUMN_ALIGNMENT		8U

typedef bool (*php_ucache_shared_graph_state_producer_t)(const zval *value, zval *state);

/* Per-string COPY state; string_dedup maps each string to its index. */
typedef struct {
	uint32_t payload_offset;
	uint32_t intern_offset;
	uint32_t intern_candidate; /* Index plus one, 0 = none. */
} php_ucache_graph_string_record_t;

typedef enum {
	PHP_UCACHE_GRAPH_KEY_PLAIN = 0,
	PHP_UCACHE_GRAPH_KEY_INTERNED,
	PHP_UCACHE_GRAPH_KEY_CANDIDATE
} php_ucache_graph_key_kind_t;

typedef struct {
	php_ucache_graph_key_kind_t kind;
	uint32_t segment_offset;
	uint32_t payload_offset;
	uint32_t candidate;
} php_ucache_graph_key_ref_t;

/* Located columns of one block; a column absent from the block is NULL. */
typedef struct {
	uint8_t *types;
	php_ucache_shared_graph_payload_t *payloads;
	uint32_t *key_offsets;
	zend_ulong *hashes;
	uint32_t *name_offsets;
	uint32_t *sleep_indices;
} php_ucache_graph_columns_t;

/* CALC and COPY must use the same route precedence. */
typedef enum {
	PHP_UCACHE_OBJECT_ROUTE_UNSTORABLE = 0,
	PHP_UCACHE_OBJECT_ROUTE_SAFE_DIRECT,
	PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE,
	PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS,
	PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE,
	PHP_UCACHE_OBJECT_ROUTE_SLEEP,
	PHP_UCACHE_OBJECT_ROUTE_WAKEUP,
	PHP_UCACHE_OBJECT_ROUTE_SERDES,
	PHP_UCACHE_OBJECT_ROUTE_PLAIN
} php_ucache_object_route_t;

typedef struct {
	size_t size;
	HashTable seen_arrays;
	HashTable seen_objects;
	HashTable seen_references;
	HashTable string_dedup;
	HashTable array_shape_dedup;
	HashTable state_schema_dedup;
	HashTable direct_array_dedup;
	/* Content hash to the first verbatim array with that content. */
	HashTable verbatim_content_dedup;
	HashTable direct_verdicts;
	HashTable enum_dedup;
	bool verbatim_arrays_allowed;
	/* Pure-data roots run no state hook, so equal-content verbatim arrays
	 * can share one copy; mixed roots keep address identity only. */
	bool dedup_verbatim_content;
	/* Distinguishes a sizing failure from an ineligible node. */
	bool reserve_failed;
	/* Array address to verbatim eligibility, shared by CALC and COPY. */
	HashTable *shared_verdicts;
	HashTable *verbatim_content_hashes;
	HashTable *verbatim_canonicals;
	HashTable *state_memo;
	/* Distinct key-like strings within the per-payload intern cap. */
	uint32_t intern_key_count;
} php_ucache_shared_graph_calc_ctx_t;

typedef struct {
	uint8_t *buffer;
	size_t size;
	size_t position;
	/* Absolute pointer slots to patch if the payload moves. */
	uint32_t *fixup_offsets;
	uint32_t fixup_count;
	uint32_t fixup_capacity;
	HashTable seen_arrays;
	/* Packs the aligned object offset and flags-field displacement. */
	HashTable seen_objects;
	HashTable seen_references;
	HashTable string_dedup;
	HashTable array_shape_dedup;
	HashTable state_schema_dedup;
	HashTable direct_array_dedup;
	/* Content hash to the first verbatim array with that content. */
	HashTable verbatim_content_dedup;
	HashTable own_content_hashes;
	HashTable own_canonicals;
	HashTable direct_verdicts;
	/* Enum case address to emitted node offset. */
	HashTable enum_dedup;
	bool dedup_verbatim_content;
	bool has_shared_identity;
	bool has_object;
	bool prefers_prototype;
	bool has_userland_restore_object;
	bool has_verbatim_array;
	bool verbatim_arrays_allowed;
	/* CALC verdicts remain valid only if no snapshot hook ran. */
	const HashTable *shared_verdicts;
	HashTable *verbatim_content_hashes;
	HashTable *verbatim_canonicals;
	HashTable *state_memo;
	php_ucache_graph_string_record_t *records;
	uint32_t record_count;
	uint32_t record_capacity;
	php_ucache_graph_intern_plan_t *intern;
	uint32_t *intern_list;
} php_ucache_shared_graph_copy_ctx_t;

typedef struct {
	const uint8_t *old_base;
	const uint8_t *new_base;
	size_t len;
	ptrdiff_t delta;
	HashTable *seen;
} php_ucache_shared_graph_rebase_ctx_t;

static void ucache_decode_array_dtor(zval *zv);
static void ucache_decode_shape_prototype_dtor(zval *zv);
static bool ucache_shared_graph_calc_value(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value);
static bool ucache_shared_graph_copy_value(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst);
static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_value(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst);
#if ZEND_DEBUG
static bool ucache_shared_graph_rebase_verbatim_array(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		zend_array *arr);
static bool ucache_shared_graph_rebase_graph_value(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		const php_ucache_shared_graph_value_t *value);
#endif

static zend_always_inline bool ucache_decode_range_ok(
		size_t buf_len,
		uint32_t offset,
		size_t need)
{
	return offset <= buf_len && need <= buf_len - offset;
}

/* Check without overflowing size_t. */
static zend_always_inline bool ucache_decode_array_range_ok(
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

/* Column blocks keep one field per element side by side, every column
 * padded to PHP_UCACHE_GRAPH_COLUMN_ALIGNMENT; CALC, COPY, decode and the
 * debug rebase all derive the layout from these helpers. */
static zend_always_inline uint64_t ucache_graph_column_bytes(uint32_t count, size_t elem_size)
{
	uint64_t mask = PHP_UCACHE_GRAPH_COLUMN_ALIGNMENT - 1;

	return ((uint64_t) count * elem_size + mask) & ~mask;
}

static zend_always_inline uint64_t ucache_graph_value_columns_size(uint32_t count)
{
	return ucache_graph_column_bytes(count, sizeof(uint8_t))
		+ ucache_graph_column_bytes(count, sizeof(php_ucache_shared_graph_payload_t))
	;
}

static zend_always_inline uint64_t ucache_graph_property_columns_size(uint32_t count, bool sleep_indices)
{
	return ucache_graph_column_bytes(count, sizeof(uint32_t)) * (sleep_indices ? 2 : 1)
		+ ucache_graph_value_columns_size(count)
	;
}

static zend_always_inline uint64_t ucache_graph_array_columns_size(uint32_t count, uint32_t flags)
{
	uint64_t size = ucache_graph_value_columns_size(count);

	if (flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_STRING_KEYS) {
		size += ucache_graph_column_bytes(count, sizeof(uint32_t));
	}

	if (flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_INT_KEYS) {
		size += ucache_graph_column_bytes(count, sizeof(zend_ulong));
	}

	return size;
}

static zend_always_inline bool ucache_graph_columns_fit(size_t buf_len, uint32_t offset, uint64_t size)
{
	return size <= (uint64_t) SIZE_MAX && ucache_decode_range_ok(buf_len, offset, (size_t) size);
}

static zend_always_inline void ucache_graph_value_columns_locate(
		uint8_t *base,
		uint32_t count,
		php_ucache_graph_columns_t *cols)
{
	memset(cols, 0, sizeof(*cols));

	cols->types = base;
	cols->payloads = (php_ucache_shared_graph_payload_t *) (void *)
		(base + ucache_graph_column_bytes(count, sizeof(uint8_t)))
	;
}

static zend_always_inline void ucache_graph_property_columns_locate(
		uint8_t *base,
		uint32_t count,
		bool sleep_indices,
		php_ucache_graph_columns_t *cols)
{
	uint8_t *cursor = base;

	memset(cols, 0, sizeof(*cols));

	cols->name_offsets = (uint32_t *) (void *) cursor;
	cursor += ucache_graph_column_bytes(count, sizeof(uint32_t));

	if (sleep_indices) {
		cols->sleep_indices = (uint32_t *) (void *) cursor;
		cursor += ucache_graph_column_bytes(count, sizeof(uint32_t));
	}

	cols->types = cursor;
	cols->payloads = (php_ucache_shared_graph_payload_t *) (void *)
		(cursor + ucache_graph_column_bytes(count, sizeof(uint8_t)))
	;
}

static zend_always_inline void ucache_graph_array_columns_locate(
		uint8_t *base,
		uint32_t count,
		uint32_t flags,
		php_ucache_graph_columns_t *cols)
{
	uint8_t *cursor;

	ucache_graph_value_columns_locate(base, count, cols);

	cursor = base + ucache_graph_value_columns_size(count);

	if (flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_STRING_KEYS) {
		cols->key_offsets = (uint32_t *) (void *) cursor;
		cursor += ucache_graph_column_bytes(count, sizeof(uint32_t));
	}

	if (flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_INT_KEYS) {
		cols->hashes = (zend_ulong *) (void *) cursor;
	}
}

static zend_always_inline void ucache_graph_columns_load_value(
		const php_ucache_graph_columns_t *cols,
		uint32_t i,
		php_ucache_shared_graph_value_t *value)
{
	value->type = cols->types[i];
	value->payload = cols->payloads[i];
}

static zend_always_inline void ucache_graph_columns_store_value(
		php_ucache_graph_columns_t *cols,
		uint32_t i,
		const php_ucache_shared_graph_value_t *value)
{
	cols->types[i] = value->type;
	cols->payloads[i] = value->payload;
}

static zend_always_inline uint32_t ucache_graph_array_key_flags(const HashTable *arr)
{
	zend_string *key;
	uint32_t flags = 0;

	if (HT_IS_PACKED(arr) && HT_IS_WITHOUT_HOLES(arr)) {
		return PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED;
	}

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		flags |= key != NULL
			? PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_STRING_KEYS
			: PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_INT_KEYS
		;

		if (flags == (PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_STRING_KEYS | PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_INT_KEYS)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();

	return flags;
}

static zend_always_inline bool ucache_decode_fail_zval(zval *dst)
{
	zval_ptr_dtor(dst);

	ZVAL_UNDEF(dst);

	return false;
}

static zend_always_inline zend_string *ucache_decode_segment_string_at(uint32_t unit_offset)
{
	const uint8_t *base = UC_G(decode_segment_base);
	size_t limit = UC_G(decode_segment_len), byte_offset = php_ucache_shm_bytes(unit_offset);
	zend_string *str;

	if (base == NULL || byte_offset > limit || limit - byte_offset < _ZSTR_HEADER_SIZE) {
		return NULL;
	}

	str = (zend_string *) (void *) (base + byte_offset);
	if (ZSTR_LEN(str) > limit || limit - byte_offset < _ZSTR_STRUCT_SIZE(ZSTR_LEN(str))) {
		return NULL;
	}

	return str;
}

static zend_always_inline zend_string *ucache_decode_string_at(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t offset)
{
	zend_string *str;

	if (offset & PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG) {
		return ucache_decode_segment_string_at(offset & ~PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG);
	}

	if (!ucache_decode_range_ok(buf_len, offset, _ZSTR_HEADER_SIZE)) {
		return NULL;
	}

	str = (zend_string *) (void *) (buf + offset);
	if (ZSTR_LEN(str) > buf_len ||
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

static zend_always_inline size_t ucache_decode_node_header_size(uint8_t type)
{
	switch (type) {
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			return sizeof(php_ucache_shared_graph_array_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			return sizeof(php_ucache_shared_graph_shaped_array_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			return sizeof(php_ucache_shared_graph_object_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			return sizeof(php_ucache_shared_graph_safe_direct_object_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			return sizeof(php_ucache_shared_graph_shaped_state_object_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return sizeof(php_ucache_shared_graph_serdes_object_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM:
			return sizeof(php_ucache_shared_graph_enum_t);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return sizeof(php_ucache_shared_graph_reference_t);
		default:
			return 0;
	}
}

/* Address-keyed entries are valid while the decoded payload remains pinned. */
static zend_always_inline void *ucache_decode_resolve_cache_find(const void *addr)
{
	uint32_t i;

	for (i = 0; i < PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
		if (UC_G(decode_resolve_direct_keys)[i] == addr) {
			return UC_G(decode_resolve_direct_values)[i];
		}
	}

	if (UC_G(decode_resolve_cache) == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(
		UC_G(decode_resolve_cache),
		(zend_ulong) (uintptr_t) addr
	);
}

static zend_always_inline void ucache_decode_resolve_cache_store(const void *addr, void *value)
{
	uint32_t slot;

	slot = UC_G(decode_resolve_direct_next)++ % PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS;

	UC_G(decode_resolve_direct_keys)[slot] = addr;
	UC_G(decode_resolve_direct_values)[slot] = value;

	if (UC_G(decode_resolve_cache) == NULL) {
		UC_G(decode_resolve_cache) = emalloc(sizeof(HashTable));
		zend_hash_init(UC_G(decode_resolve_cache), 8, NULL, NULL, 0);
	}

	zend_hash_index_add_ptr(
		UC_G(decode_resolve_cache),
		(zend_ulong) (uintptr_t) addr,
		value
	);
}

PHP_UCACHE_DEFINE_DECODE_MAP(
	identity,
	zend_object,
	php_ucache_object_table_dtor,
	OBJ_RELEASE(entry)
)

PHP_UCACHE_DEFINE_DECODE_MAP(
	reference,
	zend_reference,
	php_ucache_reference_table_dtor,
	if (GC_DELREF(entry) == 0) efree_size(entry, sizeof(zend_reference))
)

PHP_UCACHE_DEFINE_DECODE_MAP(
	array,
	zend_array,
	ucache_decode_array_dtor,
	if (GC_DELREF(entry) == 0) zend_array_destroy(entry)
)

static zend_always_inline HashTable *ucache_decode_shape_prototype_cache(void)
{
	if (UC_G(decode_shape_prototype_cache) == NULL) {
		UC_G(decode_shape_prototype_cache) = emalloc(sizeof(HashTable));
		zend_hash_init(
			UC_G(decode_shape_prototype_cache),
			8,
			NULL,
			ucache_decode_shape_prototype_dtor,
			0
		);
	}

	return UC_G(decode_shape_prototype_cache);
}

static zend_always_inline size_t ucache_shared_graph_alignment_padding(const void *buf)
{
	uintptr_t raw_addr, aligned_addr;

	raw_addr = (uintptr_t) buf;
	aligned_addr = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_addr);

	return (size_t) (aligned_addr - raw_addr);
}

static zend_always_inline bool ucache_shared_graph_header_is_valid(
		const php_ucache_shared_graph_header_t *header)
{
	return header->magic == PHP_UCACHE_SHARED_GRAPH_MAGIC &&
		header->version == PHP_UCACHE_SHARED_GRAPH_VERSION
	;
}

static zend_always_inline const uint8_t *ucache_shared_graph_locate(
		const uint8_t *buf,
		size_t buf_len,
		size_t *graph_len)
{
	const php_ucache_shared_graph_header_t *header;
	size_t padding;

	padding = ucache_shared_graph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < sizeof(php_ucache_shared_graph_header_t)) {
		return NULL;
	}

	buf += padding;
	buf_len -= padding;
	header = (const php_ucache_shared_graph_header_t *) buf;
	if (!ucache_shared_graph_header_is_valid(header)) {
		return NULL;
	}

	if (graph_len != NULL) {
		*graph_len = buf_len;
	}

	return buf;
}

static zend_always_inline void ucache_shared_graph_calc_init(php_ucache_shared_graph_calc_ctx_t *ctx)
{
	ctx->size = 0;
	ctx->reserve_failed = false;
	ctx->shared_verdicts = NULL;
	ctx->verbatim_content_hashes = NULL;
	ctx->verbatim_canonicals = NULL;
	ctx->dedup_verbatim_content = false;
	ctx->intern_key_count = 0;

	zend_hash_init(&ctx->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_references, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->string_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->array_shape_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->state_schema_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_array_dedup, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->verbatim_content_dedup, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->enum_dedup, 8, NULL, NULL, 0);
}

static zend_always_inline void ucache_shared_graph_calc_destroy(php_ucache_shared_graph_calc_ctx_t *ctx)
{
	zend_hash_destroy(&ctx->enum_dedup);
	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->verbatim_content_dedup);
	zend_hash_destroy(&ctx->direct_array_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->array_shape_dedup);
	zend_hash_destroy(&ctx->string_dedup);
	zend_hash_destroy(&ctx->seen_references);
	zend_hash_destroy(&ctx->seen_objects);
	zend_hash_destroy(&ctx->seen_arrays);
}

static zend_always_inline bool ucache_shared_graph_calc_reserve(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		size_t amount)
{
	size_t aligned_amount;

	aligned_amount = PHP_UCACHE_ALIGNED_SIZE(amount);
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

/* string_dedup holds IS_TRUE once the string counts as an intern key. */
static zend_always_inline bool ucache_shared_graph_calc_reserve_string_ex(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zend_string *string,
		bool key)
{
	zval *entry = zend_hash_lookup(&ctx->string_dedup, (zend_string *) string);

	if (Z_TYPE_P(entry) == IS_NULL) {
		if (!ucache_shared_graph_calc_reserve(ctx, _ZSTR_STRUCT_SIZE(ZSTR_LEN(string)))) {
			return false;
		}

		ZVAL_FALSE(entry);
	}

	if (key &&
		Z_TYPE_P(entry) == IS_FALSE &&
		ctx->intern_key_count < PHP_UCACHE_INTERN_MAX_KEYS_PER_PAYLOAD &&
		php_ucache_intern_eligible(string)
	) {
		ZVAL_TRUE(entry);
		ctx->intern_key_count++;
	}

	return true;
}

static zend_always_inline bool ucache_shared_graph_calc_reserve_string(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zend_string *string)
{
	return ucache_shared_graph_calc_reserve_string_ex(ctx, string, false);
}

static zend_always_inline bool ucache_shared_graph_calc_reserve_key_string(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zend_string *string)
{
	return ucache_shared_graph_calc_reserve_string_ex(ctx, string, true);
}

static zend_always_inline size_t ucache_shared_graph_intern_list_bytes(uint32_t key_count)
{
	return key_count == 0 ? 0 : PHP_UCACHE_ALIGNED_SIZE((size_t) key_count * sizeof(uint32_t));
}

static zend_always_inline void ucache_shared_graph_key_append_u32(smart_str *key, uint32_t value)
{
	smart_str_appendl(key, (const char *) &value, sizeof(value));
}

/* nNextFreeElement is a zend_long; the 4-byte node field covers [0, UINT32_MAX]
 * and everything else goes to an out-of-line int64 (WIDE_NEXT_FREE). */
static zend_always_inline bool ucache_shared_graph_next_free_is_wide(zend_long next_free)
{
	return next_free < 0 || (zend_ulong) next_free > (zend_ulong) UINT32_MAX;
}

static zend_always_inline bool ucache_shared_graph_decode_array_next_free(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_array_t *graph_array,
		zend_long *next_free)
{
	int64_t wide;

	if (!(graph_array->flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_WIDE_NEXT_FREE)) {
#if SIZEOF_ZEND_LONG < 8
		if ((uint64_t) graph_array->next_free > (uint64_t) ZEND_LONG_MAX) {
			return false;
		}
#endif

		*next_free = (zend_long) graph_array->next_free;

		return true;
	}

	if (!ucache_decode_range_ok(buf_len, graph_array->next_free, sizeof(wide))) {
		return false;
	}

	memcpy(&wide, buf + graph_array->next_free, sizeof(wide));

	if (wide > (int64_t) ZEND_LONG_MAX || wide < (int64_t) ZEND_LONG_MIN) {
		return false;
	}

	*next_free = (zend_long) wide;

	return true;
}

static zend_always_inline bool ucache_shared_graph_array_has_shape(const HashTable *arr)
{
	zend_string *key;

	if (arr->nNumOfElements == 0 ||
		HT_IS_PACKED(arr) ||
		arr->nNumOfElements > PHP_UCACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS ||
		arr->nNextFreeElement < 0 ||
		arr->nNextFreeElement > UINT32_MAX ||
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

static zend_always_inline bool ucache_shared_graph_can_restore_direct(zend_class_entry *ce)
{
	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		return false;
	}

	if (ce->type != ZEND_USER_CLASS && ce->create_object != NULL) {
		return false;
	}

	return true;
}

static zend_always_inline bool ucache_shared_graph_can_use_verbatim_arrays(void)
{
#ifdef ZEND_WIN32
	return false;
#else
	return !php_ucache_active_context()->boundary_shared;
#endif
}

static zend_always_inline bool ucache_shared_graph_is_unmangled_property_name(zend_string *prop_name)
{
	return ZSTR_LEN(prop_name) != 0 && ZSTR_VAL(prop_name)[0] != '\0';
}

static zend_always_inline bool ucache_shared_graph_safe_direct_property_shadows_state(
		zend_string *prop_name,
		const HashTable *state_ht)
{
	return state_ht != NULL &&
		ucache_shared_graph_is_unmangled_property_name(prop_name) &&
		zend_hash_exists(state_ht, prop_name)
	;
}

static zend_always_inline bool ucache_shared_graph_can_use_safe_direct(zend_class_entry *ce)
{
	const php_ucache_safe_direct_handlers_t *handlers;
	zend_class_entry *base_ce = NULL;

	handlers = php_ucache_safe_direct_find_handlers(ce, &base_ce);

	return handlers != NULL &&
		!php_ucache_class_overrides_safe_direct_magic_serialize_ex(ce, handlers, base_ce)
	;
}

static zend_always_inline bool ucache_shared_graph_can_use_sleep_object(zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		(ce->serialize != NULL && ce->unserialize != NULL)
	) {
		return false;
	}

	return php_ucache_class_has_sleep(ce);
}

static zend_always_inline bool ucache_shared_graph_can_use_wakeup_object(zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		ce->__serialize != NULL ||
		ce->__unserialize != NULL ||
		(ce->serialize != NULL && ce->unserialize != NULL) ||
		php_ucache_class_has_sleep(ce)
	) {
		return false;
	}

	return php_ucache_class_has_wakeup(ce);
}

static zend_always_inline bool ucache_shared_graph_pointer_in_range(
		const void *ptr,
		const uint8_t *base,
		size_t len)
{
	uintptr_t addr, start;

	if (ptr == NULL || base == NULL || len == 0) {
		return false;
	}

	addr = (uintptr_t) ptr;
	start = (uintptr_t) base;

	return addr >= start && addr - start < len;
}

static zend_always_inline bool ucache_shared_graph_pointer_in_segment_data(const void *ptr)
{
	php_ucache_header_t *header = php_ucache_header_ptr();

	return header != NULL &&
		ucache_shared_graph_pointer_in_range(
			ptr,
			(const uint8_t *) header + php_ucache_shm_bytes(header->data_offset),
			php_ucache_shm_bytes(header->data_size)
		)
	;
}

#if ZEND_DEBUG
static zend_always_inline void *ucache_shared_graph_rebase_pointer(
		void *ptr,
		const uint8_t *old_base,
		size_t len,
		ptrdiff_t delta)
{
	if (!ucache_shared_graph_pointer_in_range(ptr, old_base, len)) {
		return ptr;
	}

	return (void *) ((char *) ptr - delta);
}
#endif /* ZEND_DEBUG */

static zend_always_inline void ucache_shared_graph_copy_record_fixup(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const void *slot)
{
	ZEND_ASSERT(ucache_shared_graph_pointer_in_range(slot, ctx->buffer, ctx->size));
	ZEND_ASSERT(((uintptr_t) slot & (sizeof(uintptr_t) - 1)) == 0);

	if (ctx->fixup_count == ctx->fixup_capacity) {
		ctx->fixup_capacity = ctx->fixup_capacity == 0 ? 8 : ctx->fixup_capacity * 2;
		ctx->fixup_offsets = erealloc(ctx->fixup_offsets, sizeof(*ctx->fixup_offsets) * ctx->fixup_capacity);
	}

	ctx->fixup_offsets[ctx->fixup_count++] = (uint32_t) ((const uint8_t *) slot - ctx->buffer);
}

static zend_always_inline uint64_t ucache_shared_graph_content_hash_mix(uint64_t hash, uint64_t value)
{
	uint64_t seed = hash ^ value;

	return php_random_splitmix64(&seed);
}

static zend_always_inline const zval *ucache_shared_graph_verbatim_next_element(
		const HashTable *arr,
		uint32_t *pos,
		zend_ulong *h,
		zend_string **key)
{
	const zval *val;
	const Bucket *bucket;

	while (*pos < arr->nNumUsed) {
		if (HT_IS_PACKED(arr)) {
			val = &arr->arPacked[*pos];
			*h = *pos;
			*key = NULL;
		} else {
			bucket = &arr->arData[*pos];
			val = &bucket->val;
			*h = bucket->h;
			*key = bucket->key;
		}

		(*pos)++;

		if (Z_TYPE_P(val) != IS_UNDEF) {
			return val;
		}
	}

	return NULL;
}

static zend_always_inline uint64_t ucache_shared_graph_content_hash_begin(const HashTable *arr)
{
	uint64_t shape = ((uint64_t) arr->nNumOfElements << 1) | (HT_IS_PACKED(arr) ? 1 : 0);

	return ucache_shared_graph_content_hash_mix(shape, (uint64_t) arr->nNextFreeElement);
}

static zend_always_inline uint64_t ucache_shared_graph_content_hash_element(
		uint64_t hash,
		zend_ulong h,
		zend_string *key,
		const zval *val,
		zend_ulong nested_hash)
{
	uint64_t value_hash;

	switch (Z_TYPE_P(val)) {
		case IS_LONG:
			value_hash = (uint64_t) Z_LVAL_P(val);
			break;
		case IS_DOUBLE:
			memcpy(&value_hash, &Z_DVAL_P(val), sizeof(value_hash));
			break;
		case IS_STRING:
			value_hash = zend_string_hash_val(Z_STR_P(val));
			break;
		case IS_ARRAY:
			value_hash = nested_hash;
			break;
		default:
			value_hash = 0;
			break;
	}

	value_hash ^= (uint64_t) Z_TYPE_P(val) << 56;
	value_hash = (value_hash << 32) | (value_hash >> 32);

	return ucache_shared_graph_content_hash_mix(
		hash,
		(key != NULL ? zend_string_hash_val(key) ^ PHP_UCACHE_CONTENT_HASH_STRING_KEY_TAG : h) ^ value_hash
	);
}

static zend_always_inline bool ucache_shared_graph_copy_verbatim_array_ref(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		uint32_t array_offset,
		zval *dst)
{
	ZVAL_ARR(dst, (zend_array *) (void *) (ctx->buffer + array_offset));
	Z_TYPE_FLAGS_P(dst) = 0;

	if (ucache_shared_graph_pointer_in_range(dst, ctx->buffer, ctx->size)) {
		ucache_shared_graph_copy_record_fixup(ctx, dst);
	}

	return true;
}

static zend_always_inline void ucache_decode_shape_prototype_direct_cache_store(
		const php_ucache_shared_graph_array_shape_t *graph_shape,
		zend_array *proto)
{
	uint32_t slot;

	slot = UC_G(decode_shape_prototype_direct_next)++ % PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS;

	UC_G(decode_shape_prototype_direct_keys)[slot] = graph_shape;
	UC_G(decode_shape_prototype_direct_values)[slot] = proto;
}

static zend_always_inline bool ucache_shared_graph_decode_simple_value(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	zend_string *str;

	switch (value->type) {
		case PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF:
			ZVAL_UNDEF(dst);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_NULL:
			ZVAL_NULL(dst);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_TRUE:
			ZVAL_TRUE(dst);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_FALSE:
			ZVAL_FALSE(dst);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_LONG:
			ZVAL_LONG(dst, value->payload.long_value);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DOUBLE:
			ZVAL_DOUBLE(dst, value->payload.double_value);

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_STRING:
			str = ucache_decode_string_at(buf, buf_len, (uint32_t) value->payload.offset);
			if (str == NULL) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, str);

			return true;
		default:
			return false;
	}
}

static zend_always_inline bool ucache_shared_graph_decode_value_inline(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	if (ucache_shared_graph_decode_simple_value(buf, buf_len, value, dst)) {
		return true;
	}

	ZVAL_UNDEF(dst);

	return ucache_shared_graph_decode_value(buf, buf_len, value, dst);
}

static zend_string *ucache_shared_graph_array_shape_key(const HashTable *arr)
{
	zend_string *key;
	smart_str shape_key = {0};
	uint32_t count, key_len;

	ZEND_ASSERT(ucache_shared_graph_array_has_shape(arr));

	count = (uint32_t) arr->nNumOfElements;
	smart_str_appendl(&shape_key, "ucshape", sizeof("ucshape") - 1);

	ucache_shared_graph_key_append_u32(&shape_key, count);

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (ZSTR_LEN(key) > UINT32_MAX) {
			smart_str_free(&shape_key);

			return NULL;
		}

		key_len = (uint32_t) ZSTR_LEN(key);

		ucache_shared_graph_key_append_u32(&shape_key, key_len);

		smart_str_appendl(&shape_key, ZSTR_VAL(key), ZSTR_LEN(key));
	} ZEND_HASH_FOREACH_END();

	return smart_str_extract(&shape_key);
}

static bool ucache_shared_graph_calc_array_shape(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const HashTable *arr,
		zend_string *shape_key)
{
	zend_string *key;
	bool result = true, owns_shape_key = false;

	if (shape_key == NULL) {
		shape_key = ucache_shared_graph_array_shape_key(arr);
		if (shape_key == NULL) {
			return false;
		}

		owns_shape_key = true;
	}

	if (zend_hash_exists(&ctx->array_shape_dedup, shape_key)) {
		goto bailout;
	}

	if (!ucache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_ucache_shared_graph_array_shape_t)
		) ||
		!ucache_shared_graph_calc_reserve(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(php_ucache_shared_graph_array_shape_element_t)
		)
	) {
		result = false;

		goto bailout;
	}

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (!ucache_shared_graph_calc_reserve_key_string(ctx, key)) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (result) {
		result = zend_hash_add_empty_element(&ctx->array_shape_dedup, shape_key) != NULL;
	}

bailout:
	if (owns_shape_key) {
		zend_string_release(shape_key);
	}

	return result;
}

static zend_string *ucache_shared_graph_state_schema_key(
		zend_class_entry *ce,
		zend_string *shape_key)
{
	smart_str schema_key = {0};
	uint32_t class_name_len, shape_key_len;

	if (ZSTR_LEN(ce->name) > UINT32_MAX || ZSTR_LEN(shape_key) > UINT32_MAX) {
		return NULL;
	}

	class_name_len = (uint32_t) ZSTR_LEN(ce->name);
	shape_key_len = (uint32_t) ZSTR_LEN(shape_key);

	smart_str_appendl(&schema_key, "ucstate", sizeof("ucstate") - 1);
	ucache_shared_graph_key_append_u32(&schema_key, class_name_len);

	smart_str_appendl(&schema_key, ZSTR_VAL(ce->name), ZSTR_LEN(ce->name));
	ucache_shared_graph_key_append_u32(&schema_key, shape_key_len);

	smart_str_appendl(&schema_key, ZSTR_VAL(shape_key), ZSTR_LEN(shape_key));

	return smart_str_extract(&schema_key);
}

static bool ucache_shared_graph_calc_state_schema(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		zend_class_entry *ce,
		const HashTable *arr)
{
	zend_string *schema_key, *shape_key;
	bool result = true;

	shape_key = ucache_shared_graph_array_shape_key(arr);
	if (shape_key == NULL) {
		return false;
	}

	schema_key = ucache_shared_graph_state_schema_key(ce, shape_key);
	if (schema_key == NULL) {
		zend_string_release(shape_key);

		return false;
	}

	if (zend_hash_exists(&ctx->state_schema_dedup, schema_key)) {
		goto bailout;
	}

	result = ucache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_ucache_shared_graph_state_schema_t)
		) &&
		ucache_shared_graph_calc_reserve_key_string(ctx, ce->name) &&
		ucache_shared_graph_calc_array_shape(ctx, arr, shape_key)
	;

	if (result) {
		result = zend_hash_add_empty_element(&ctx->state_schema_dedup, schema_key) != NULL;
	}

bailout:
	zend_string_release(shape_key);
	zend_string_release(schema_key);

	return result;
}

static bool ucache_shared_graph_state_value_fits_schema(
		zval *value,
		const HashTable *state_array,
		HashTable *seen_arrs)
{
	zend_ulong arr_key;
	zval *elem;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	ZVAL_DEREF(value);

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			return true;
		case IS_ARRAY:
			if (Z_ARRVAL_P(value) == state_array) {
				return false;
			}

			arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);

			if (!php_ucache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(value))) {
				return true;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), elem) {
				if (!ucache_shared_graph_state_value_fits_schema(
						elem,
						state_array,
						seen_arrs
					)
				) {
					zend_hash_index_del(seen_arrs, arr_key);

					return false;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(seen_arrs, arr_key);

			return true;
		default:
			return false;
	}
}

static bool ucache_shared_graph_state_array_fits_schema(const HashTable *arr)
{
	zval *elem;
	HashTable seen_arrs;
	bool result = true;

	if (!ucache_shared_graph_array_has_shape(arr)) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
		if (!ucache_shared_graph_state_value_fits_schema(
				elem,
				arr,
				&seen_arrs
			)
		) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&seen_arrs);

	return result;
}

/* Memoize the schema-fit verdict keyed by the (pointer-stable, memoized)
 * state array so COPY reuses CALC's answer instead of re-walking the whole
 * state. Lives in state_memo: object-keyed entries there are states/blobs
 * (IS_ARRAY/IS_STRING), array-keyed verdicts are IS_TRUE/IS_FALSE. */
static bool ucache_shared_graph_state_array_fits_schema_memo(
		HashTable *state_memo,
		const HashTable *arr)
{
	zval *verdict, verdict_zv;

	if (state_memo == NULL) {
		return ucache_shared_graph_state_array_fits_schema(arr);
	}

	verdict = zend_hash_index_find(state_memo, (zend_ulong) (uintptr_t) arr);
	if (verdict != NULL &&
		(Z_TYPE_P(verdict) == IS_TRUE || Z_TYPE_P(verdict) == IS_FALSE)
	) {
		return Z_TYPE_P(verdict) == IS_TRUE;
	}

	ZVAL_BOOL(&verdict_zv, ucache_shared_graph_state_array_fits_schema(arr));
	zend_hash_index_add(state_memo, (zend_ulong) (uintptr_t) arr, &verdict_zv);

	return Z_TYPE(verdict_zv) == IS_TRUE;
}

static bool ucache_shared_graph_calc_shaped_state_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		zend_class_entry *ce,
		const HashTable *state_array)
{
	zval *elem;

	if (!ucache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_ucache_shared_graph_shaped_state_object_t)
		) ||
		!ucache_shared_graph_calc_reserve(
			ctx,
			ucache_graph_value_columns_size(state_array->nNumOfElements)
		) ||
		!ucache_shared_graph_calc_state_schema(ctx, ce, state_array)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL((HashTable *) state_array, elem) {
		if (!ucache_shared_graph_calc_value(ctx, elem)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static php_ucache_object_route_t ucache_shared_graph_classify_object_route_impl(zend_class_entry *ce)
{
	if (ucache_shared_graph_can_use_safe_direct(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_SAFE_DIRECT;
	}

	if (php_ucache_class_uses_magic_serialize(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE;
	}

	if (php_ucache_class_uses_serialize_props(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS;
	}

	if (php_ucache_class_uses_magic_unserialize(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE;
	}

	if (ucache_shared_graph_can_use_sleep_object(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_SLEEP;
	}

	if (ucache_shared_graph_can_use_wakeup_object(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_WAKEUP;
	}

	if (php_ucache_class_uses_serdes(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_SERDES;
	}

	if (!ucache_shared_graph_can_restore_direct(ce)) {
		return PHP_UCACHE_OBJECT_ROUTE_UNSTORABLE;
	}

	return PHP_UCACHE_OBJECT_ROUTE_PLAIN;
}

static php_ucache_object_route_t ucache_shared_graph_classify_object_route(zend_class_entry *ce)
{
	php_ucache_object_route_t route;
	zval *cached, route_zv;

	if (UC_G(object_route_memo) != NULL) {
		cached = zend_hash_index_find(UC_G(object_route_memo), (zend_ulong) (uintptr_t) ce);
		if (cached != NULL) {
			return (php_ucache_object_route_t) Z_LVAL_P(cached);
		}
	} else {
		UC_G(object_route_memo) = emalloc(sizeof(HashTable));

		zend_hash_init(UC_G(object_route_memo), 8, NULL, NULL, 0);
	}

	route = ucache_shared_graph_classify_object_route_impl(ce);

	ZVAL_LONG(&route_zv, (zend_long) route);
	zend_hash_index_add(UC_G(object_route_memo), (zend_ulong) (uintptr_t) ce, &route_zv);

	return route;
}

static void ucache_shared_graph_object_route_memo_release(void)
{
	if (UC_G(object_route_memo) != NULL) {
		zend_hash_destroy(UC_G(object_route_memo));

		efree(UC_G(object_route_memo));

		UC_G(object_route_memo) = NULL;
	}
}

static bool ucache_shared_graph_can_copy_verbatim_value(HashTable *direct_verdicts, const zval *value)
{
	const zval *packed_value;
	const HashTable *arr;
	const Bucket *bucket;
	zend_ulong arr_key;
	zval *cached, verdict;
	uint32_t i;
	bool result = true;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			return true;
		case IS_ARRAY:
			arr = Z_ARRVAL_P(value);
			if (arr->nNumOfElements == 0) {
				return true;
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
					packed_value = &arr->arPacked[i];
					if (!ucache_shared_graph_can_copy_verbatim_value(
							direct_verdicts,
							packed_value
						)
					) {
						result = false;

						break;
					}
				}
			} else {
				bucket = arr->arData;

				for (i = 0; i < arr->nNumUsed; i++) {
					if (Z_TYPE(bucket[i].val) != IS_UNDEF &&
						!ucache_shared_graph_can_copy_verbatim_value(
							direct_verdicts,
							&bucket[i].val
						)
					) {
						result = false;

						break;
					}
				}
			}

			if (GC_REFCOUNT(arr) > 1) {
				ZVAL_BOOL(&verdict, result);
				zend_hash_index_add(direct_verdicts, arr_key, &verdict);
			}

			return result;
		default:
			return false;
	}
}

static bool ucache_shared_graph_verbatim_content_hash(
		HashTable *memo,
		const HashTable *arr,
		zend_ulong *content_hash)
{
	const zval *val;
	zend_string *key = NULL;
	zend_ulong h = 0, arr_key = (zend_ulong) (uintptr_t) arr, nested_hash;
	zval *cached, hash_zv;
	uint64_t hash, hash2 = 0;
	uint32_t pos = 0, n = 0;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	cached = zend_hash_index_find(memo, arr_key);
	if (cached != NULL) {
		*content_hash = (zend_ulong) Z_LVAL_P(cached);

		return true;
	}

	hash = ucache_shared_graph_content_hash_begin(arr);

	while ((val = ucache_shared_graph_verbatim_next_element(arr, &pos, &h, &key)) != NULL) {
		nested_hash = 0;

		if (Z_TYPE_P(val) == IS_ARRAY &&
			Z_ARRVAL_P(val)->nNumOfElements != 0 &&
			!ucache_shared_graph_verbatim_content_hash(memo, Z_ARRVAL_P(val), &nested_hash)
		) {
			return false;
		}

		if (n++ & 1) {
			hash2 = ucache_shared_graph_content_hash_element(hash2, h, key, val, nested_hash);
		} else {
			hash = ucache_shared_graph_content_hash_element(hash, h, key, val, nested_hash);
		}
	}

	*content_hash = (zend_ulong) ucache_shared_graph_content_hash_mix(hash, hash2);

	ZVAL_LONG(&hash_zv, (zend_long) *content_hash);
	zend_hash_index_add(memo, arr_key, &hash_zv);

	return true;
}

/* Nested arrays that both already resolved to a canonical compare by that
 * address, so a duplicate subtree costs one level, not its whole depth. */
static bool ucache_shared_graph_verbatim_arrays_identical(
		const HashTable *canonicals,
		const HashTable *a,
		const HashTable *b)
{
	const zval *val_a, *val_b;
	const HashTable *canonical_a, *canonical_b;
	zend_string *key_a = NULL, *key_b = NULL;
	zend_ulong h_a = 0, h_b = 0;
	uint32_t pos_a = 0, pos_b = 0;

	if (a == b) {
		return true;
	}

	if (php_ucache_stack_overflowed() ||
		a->nNumOfElements != b->nNumOfElements ||
		a->nNextFreeElement != b->nNextFreeElement ||
		HT_IS_PACKED(a) != HT_IS_PACKED(b)
	) {
		return false;
	}

	for (;;) {
		val_a = ucache_shared_graph_verbatim_next_element(a, &pos_a, &h_a, &key_a);
		val_b = ucache_shared_graph_verbatim_next_element(b, &pos_b, &h_b, &key_b);

		if (val_a == NULL || val_b == NULL) {
			return val_a == val_b;
		}

		if ((key_a == NULL) != (key_b == NULL) ||
			(key_a != NULL ? !zend_string_equal_content(key_a, key_b) : h_a != h_b) ||
			Z_TYPE_P(val_a) != Z_TYPE_P(val_b)
		) {
			return false;
		}

		switch (Z_TYPE_P(val_a)) {
			case IS_LONG:
				if (Z_LVAL_P(val_a) != Z_LVAL_P(val_b)) {
					return false;
				}

				break;
			case IS_DOUBLE:
				if (memcmp(&Z_DVAL_P(val_a), &Z_DVAL_P(val_b), sizeof(double)) != 0) {
					return false;
				}

				break;
			case IS_STRING:
				if (!zend_string_equal_content(Z_STR_P(val_a), Z_STR_P(val_b))) {
					return false;
				}

				break;
			case IS_ARRAY:
				canonical_a = zend_hash_index_find_ptr(canonicals, (zend_ulong) (uintptr_t) Z_ARRVAL_P(val_a));
				canonical_b = zend_hash_index_find_ptr(canonicals, (zend_ulong) (uintptr_t) Z_ARRVAL_P(val_b));

				if (canonical_a != NULL && canonical_b != NULL) {
					if (canonical_a != canonical_b) {
						return false;
					}
				} else if (!ucache_shared_graph_verbatim_arrays_identical(canonicals, Z_ARRVAL_P(val_a), Z_ARRVAL_P(val_b))) {
					return false;
				}

				break;
			default:
				break;
		}
	}
}

/* Records which array's copy this one shares: the candidate when its content
 * matches, otherwise itself. */
static const HashTable *ucache_shared_graph_verbatim_record_canonical(
		HashTable *canonicals,
		const HashTable *candidate,
		const HashTable *arr)
{
	const HashTable *canonical = candidate != NULL &&
		(candidate == arr || ucache_shared_graph_verbatim_arrays_identical(canonicals, candidate, arr))
		? candidate
		: arr
	;

	zend_hash_index_add_ptr(canonicals, (zend_ulong) (uintptr_t) arr, (void *) canonical);

	return canonical;
}

static const HashTable *ucache_shared_graph_verbatim_canonical(
		HashTable *content_dedup,
		HashTable *canonicals,
		zend_ulong content_hash,
		const HashTable *arr)
{
	return ucache_shared_graph_verbatim_record_canonical(
		canonicals,
		zend_hash_index_find_ptr(content_dedup, content_hash),
		arr
	);
}

/* CALC visits an array after its children, so registering it here keeps the
 * same first-of-its-content order as COPY's post-order registration. */
static const HashTable *ucache_shared_graph_verbatim_canonical_or_register(
		HashTable *content_dedup,
		HashTable *canonicals,
		zend_ulong content_hash,
		const HashTable *arr)
{
	zval *slot = zend_hash_index_lookup(content_dedup, content_hash);

	if (Z_TYPE_P(slot) == IS_NULL) {
		ZVAL_PTR(slot, (void *) arr);
	}

	return ucache_shared_graph_verbatim_record_canonical(canonicals, Z_PTR_P(slot), arr);
}

/* Sizes a pure-data subtree and hashes it on the same walk; an array whose
 * content matches an earlier one costs nothing beyond its strings. */
static bool ucache_shared_graph_calc_verbatim_value(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value,
		zend_ulong *content_hash)
{
	const zval *elem;
	const HashTable *arr;
	zend_string *key = NULL;
	zend_ulong arr_key, h = 0, elem_hash;
	zval *cached, hash_zv;
	uint64_t hash, hash2 = 0;
	uint32_t pos = 0, n = 0;
	size_t data_size;
	bool hashing = content_hash != NULL && ctx->dedup_verbatim_content;

	if (content_hash != NULL) {
		*content_hash = 0;
	}

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return ucache_shared_graph_calc_reserve_string(ctx, Z_STR_P(value));
		case IS_ARRAY:
			arr = Z_ARRVAL_P(value);
			if (arr->nNumOfElements == 0) {
				return true;
			}

			arr_key = (zend_ulong) (uintptr_t) arr;
			if (GC_REFCOUNT(arr) > 1) {
				if (hashing) {
					cached = zend_hash_index_find(ctx->verbatim_content_hashes, arr_key);
					if (cached != NULL) {
						*content_hash = (zend_ulong) Z_LVAL_P(cached);

						return true;
					}
				} else if (!ctx->dedup_verbatim_content && zend_hash_index_exists(&ctx->direct_array_dedup, arr_key)) {
					return true;
				}
			}

			hash = hashing ? ucache_shared_graph_content_hash_begin(arr) : 0;

			while ((elem = ucache_shared_graph_verbatim_next_element(arr, &pos, &h, &key)) != NULL) {
				if (key != NULL && !ucache_shared_graph_calc_reserve_key_string(ctx, key)) {
					return false;
				}

				if (!ucache_shared_graph_calc_verbatim_value(ctx, elem, &elem_hash)) {
					return false;
				}

				if (hashing) {
					if (n++ & 1) {
						hash2 = ucache_shared_graph_content_hash_element(hash2, h, key, elem, elem_hash);
					} else {
						hash = ucache_shared_graph_content_hash_element(hash, h, key, elem, elem_hash);
					}
				}
			}

			if (hashing) {
				hash = ucache_shared_graph_content_hash_mix(hash, hash2);
			}

			/* The store root is visited last, so nothing can dedup against it:
			 * it only needs the canonical entry COPY looks up by address. */
			if (content_hash == NULL && ctx->dedup_verbatim_content) {
				zend_hash_index_add_ptr(ctx->verbatim_canonicals, arr_key, (void *) arr);
			}

			if (hashing) {
				*content_hash = (zend_ulong) hash;

				if (GC_REFCOUNT(arr) > 1) {
					ZVAL_LONG(&hash_zv, (zend_long) *content_hash);
					zend_hash_index_add(ctx->verbatim_content_hashes, arr_key, &hash_zv);
				}

				if (ucache_shared_graph_verbatim_canonical_or_register(
						&ctx->verbatim_content_dedup,
						ctx->verbatim_canonicals,
						*content_hash,
						arr
					) != arr
				) {
					return true;
				}
			}

			data_size = HT_IS_PACKED(arr) ? HT_PACKED_USED_SIZE(arr) : HT_USED_SIZE(arr);
			if (!ucache_shared_graph_calc_reserve(ctx, sizeof(zend_array)) ||
				!ucache_shared_graph_calc_reserve(ctx, data_size)
			) {
				return false;
			}

			if (!ctx->dedup_verbatim_content && GC_REFCOUNT(arr) > 1) {
				zend_hash_index_add_empty_element(&ctx->direct_array_dedup, arr_key);
			}

			return true;
		default:
			return false;
	}
}

static bool ucache_shared_graph_produce_safe_direct_state(const zval *value, zval *state)
{
	php_ucache_safe_direct_state_serialize_func_t serialize_func;

	serialize_func = php_ucache_safe_direct_state_serialize_func(Z_OBJCE_P(value));
	if (serialize_func == NULL ||
		!serialize_func(state, value) ||
		Z_TYPE_P(state) != IS_ARRAY
	) {
		if (!Z_ISUNDEF_P(state)) {
			zval_ptr_dtor(state);
			ZVAL_UNDEF(state);
		}

		/* Deterministic error policy: a handler that declines without its
		 * own exception must still surface a throw, never a silent false. */
		if (!EG(exception)) {
			zend_type_error(
				"the %s object's state cannot be stored in the user cache",
				ZSTR_VAL(Z_OBJCE_P(value)->name)
			);
		}

		return false;
	}

	return true;
}

static bool ucache_shared_graph_extract_serialize_snapshot(const zval *value, zval *state)
{
	return php_ucache_serdes_call_magic_serialize(Z_OBJ_P(value), state);
}

static bool ucache_shared_graph_extract_property_snapshot(const zval *value, zval *state)
{
	zend_ulong num_key;
	zend_string *key;
	zval *prop, elem;
	HashTable *props;
	bool result = true;

	props = zend_get_properties_for((zval *) value, ZEND_PROP_PURPOSE_SERIALIZE);
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

static bool ucache_shared_graph_extract_sleep_snapshot(const zval *value, zval *state)
{
	const char *msg = NULL;

	if (!php_ucache_serdes_get_sleep_state((zval *) value, state, &msg)) {
		if (msg != NULL) {
			zend_type_error("%s", msg);
		}

		return false;
	}

	return true;
}

static bool ucache_shared_graph_extract_unserialize_route_snapshot(const zval *value, zval *state)
{
	if (php_ucache_class_has_sleep(Z_OBJCE_P(value))) {
		return ucache_shared_graph_extract_sleep_snapshot(value, state);
	}

	return ucache_shared_graph_extract_property_snapshot(value, state);
}

static bool ucache_shared_graph_get_memoized_state(
		const zval *value,
		HashTable *state_memo,
		php_ucache_shared_graph_state_producer_t produce_state,
		zval **state_ptr)
{
	zend_ulong memo_key;
	zend_object *obj;
	zval *memo_state, produced;

	*state_ptr = NULL;

	if (state_memo == NULL) {
		return false;
	}

	obj = Z_OBJ_P(value);
	memo_key = (zend_ulong) (uintptr_t) obj;
	memo_state = zend_hash_index_find(state_memo, memo_key);
	if (memo_state != NULL) {
		if (Z_TYPE_P(memo_state) != IS_ARRAY) {
			return false;
		}

		*state_ptr = memo_state;

		return true;
	}

	/* Pin the object for the memo's lifetime: __sleep()/__serialize() may
	 * drop the last external reference while both graph passes keep using
	 * the raw pointer (native serialize() pins through its var_hash for
	 * the same reason). Released in php_ucache_destroy_prepared_value(). */
	GC_ADDREF(obj);

	ZVAL_UNDEF(&produced);

	if (!produce_state(value, &produced)) {
		OBJ_RELEASE(obj);

		if (!Z_ISUNDEF(produced)) {
			zval_ptr_dtor(&produced);
		}

		return false;
	}

	memo_state = zend_hash_index_add(state_memo, memo_key, &produced);
	if (memo_state == NULL) {
		OBJ_RELEASE(obj);

		zval_ptr_dtor(&produced);

		return false;
	}

	*state_ptr = memo_state;

	return true;
}

/* Safe-direct state may be produced under the write lock. */
static bool ucache_shared_graph_get_safe_direct_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	ZVAL_UNDEF(owned_state);

	if (state_memo != NULL) {
		return ucache_shared_graph_get_memoized_state(
			value,
			state_memo,
			ucache_shared_graph_produce_safe_direct_state,
			state_ptr
		);
	}

	*state_ptr = NULL;

	if (!ucache_shared_graph_produce_safe_direct_state(value, owned_state)) {
		if (!Z_ISUNDEF_P(owned_state)) {
			zval_ptr_dtor(owned_state);

			ZVAL_UNDEF(owned_state);
		}

		return false;
	}

	*state_ptr = owned_state;

	return true;
}

static bool ucache_shared_graph_get_serdes_blob(
		const zval *value,
		HashTable *state_memo,
		zend_string **blob_ptr)
{
	const char *msg = NULL;
	zend_ulong memo_key;
	zend_string *blob;
	zend_object *obj = Z_OBJ_P(value);
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

	/* Pin before the encoder runs user code: serialize() may drop the
	 * caller's last reference while the object is still being walked. On
	 * success the pin lives as the memo key, released in
	 * php_ucache_destroy_prepared_value(). */
	GC_ADDREF(obj);

	if (!php_ucache_serdes_encode((zval *) value, &buf, &msg)) {
		if (msg != NULL) {
			zend_type_error("%s", msg);
		}

		smart_str_free(&buf);

		OBJ_RELEASE(obj);

		return false;
	}

	blob = smart_str_extract(&buf);
	if (ZSTR_LEN(blob) == 0 || ZSTR_LEN(blob) > UINT32_MAX) {
		zend_string_release(blob);

		OBJ_RELEASE(obj);

		return false;
	}

	ZVAL_STR(&blob_zv, blob);
	memo_state = zend_hash_index_add(state_memo, memo_key, &blob_zv);
	if (memo_state == NULL) {
		zend_string_release(blob);

		OBJ_RELEASE(obj);

		return false;
	}

	*blob_ptr = Z_STR_P(memo_state);

	return true;
}

static php_ucache_shared_graph_state_producer_t ucache_shared_graph_route_state_producer(
		php_ucache_object_route_t route)
{
	switch (route) {
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
		case PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return ucache_shared_graph_extract_serialize_snapshot;
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_shared_graph_extract_unserialize_route_snapshot;
		case PHP_UCACHE_OBJECT_ROUTE_SLEEP:
			return ucache_shared_graph_extract_sleep_snapshot;
		case PHP_UCACHE_OBJECT_ROUTE_WAKEUP:
			return ucache_shared_graph_extract_property_snapshot;
		default:
			return NULL;
	}
}

static bool ucache_shared_graph_calc_magic_state_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value,
		zend_object *obj,
		php_ucache_object_route_t route)
{
	php_ucache_shared_graph_state_producer_t produce_state;
	HashTable *state_ht;
	zval *state_ptr, state_zv;
	bool result;

	if (!php_ucache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	produce_state = ucache_shared_graph_route_state_producer(route);
	if (!ucache_shared_graph_get_memoized_state(value, ctx->state_memo, produce_state, &state_ptr)) {
		return false;
	}

	/* state_ptr may dangle once the memo resizes (schema verdicts insert
	 * into the same table); keep the stable array pointer instead. */
	state_ht = Z_ARRVAL_P(state_ptr);

	if (ucache_shared_graph_state_array_fits_schema_memo(ctx->state_memo, state_ht)) {
		result = ucache_shared_graph_calc_shaped_state_object(
			ctx,
			obj->ce,
			state_ht
		);
	} else {
		ZVAL_ARR(&state_zv, state_ht);
		result = ucache_shared_graph_calc_reserve(ctx,
			sizeof(php_ucache_shared_graph_safe_direct_object_t)) &&
			ucache_shared_graph_calc_reserve_key_string(ctx, obj->ce->name) &&
			ucache_shared_graph_calc_value(ctx, &state_zv)
		;
	}

	return result;
}

static bool ucache_shared_graph_calc_sleep_state_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value,
		zend_object *obj,
		php_ucache_object_route_t route)
{
	php_ucache_shared_graph_state_producer_t produce_state;
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *state_ptr, *prop_val;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!php_ucache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	produce_state = ucache_shared_graph_route_state_producer(route);
	if (!ucache_shared_graph_get_memoized_state(value, ctx->state_memo, produce_state, &state_ptr)) {
		return false;
	}

	props = Z_ARRVAL_P(state_ptr);
	if (ucache_shared_graph_state_array_fits_schema_memo(ctx->state_memo, props)) {
		result = ucache_shared_graph_calc_shaped_state_object(
			ctx,
			obj->ce,
			props
		);
	} else {
		prop_count = zend_hash_num_elements(props);
		result = ucache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_ucache_shared_graph_object_t)
			) &&
			ucache_shared_graph_calc_reserve_key_string(ctx, obj->ce->name) &&
			(prop_count == 0 ||
				ucache_shared_graph_calc_reserve(
					ctx,
					ucache_graph_property_columns_size(prop_count, true)
				)
			)
		;

		if (result) {
			ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
				resolved_name = prop_name != NULL
					? zend_string_copy(prop_name)
					: zend_long_to_str((zend_long) num_key)
				;

				if (!ucache_shared_graph_calc_reserve_key_string(ctx, resolved_name) ||
					!ucache_shared_graph_calc_value(ctx, prop_val)
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

static bool ucache_shared_graph_calc_safe_direct_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value,
		zend_object *obj)
{
	HashTable *state_ht;
	zend_string *prop_name;
	zval *prop_val, *src_val, *state_ptr, state, state_zv;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!ucache_shared_graph_get_safe_direct_state(
			value,
			ctx->state_memo,
			&state_ptr,
			&state
		)
	) {
		return false;
	}

	state_ht = Z_ARRVAL_P(state_ptr);

	if (!php_ucache_seen_test_and_add(&ctx->seen_objects, obj)) {
		result = true;

		goto bailout;
	}

	ZVAL_ARR(&state_zv, state_ht);

	if (!ucache_shared_graph_calc_reserve(ctx,
		sizeof(php_ucache_shared_graph_safe_direct_object_t)) ||
		!ucache_shared_graph_calc_reserve_key_string(ctx, obj->ce->name) ||
		!ucache_shared_graph_calc_value(ctx, &state_zv)
	) {
		result = false;

		goto bailout;
	}

	/* Count the properties that will actually be emitted: state-shadowed
	 * ones are skipped entirely instead of occupying zeroed slots. */
	props = zend_std_get_properties(obj);
	prop_count = 0;
	result = true;
	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY(props, prop_name) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			if (!ucache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					state_ht
				)
			) {
				prop_count++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (!result) {
		goto bailout;
	}

	if (prop_count != 0 &&
		!ucache_shared_graph_calc_reserve(
			ctx,
			ucache_graph_property_columns_size(prop_count, false)
		)
	) {
		result = false;

		goto bailout;
	}

	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			/* calc_value() may run user code that mutates the table. */
			if (prop_name == NULL) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (ucache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					state_ht
				)
			) {
				continue;
			}

			if (!ucache_shared_graph_calc_reserve_key_string(ctx, prop_name) ||
				!ucache_shared_graph_calc_value(ctx, src_val)
			) {
				result = false;

				break;
			}
		} ZEND_HASH_FOREACH_END();
	}

bailout:
	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool ucache_shared_graph_calc_plain_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value,
		zend_object *obj)
{
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	uint32_t prop_count;
	bool result = true;

	if (!php_ucache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	if (!ucache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_ucache_shared_graph_object_t)
		) ||
		!ucache_shared_graph_calc_reserve_key_string(ctx, obj->ce->name)
	) {
		return false;
	}

	props = zend_get_properties_for((zval *) value, ZEND_PROP_PURPOSE_SERIALIZE);
	prop_count = props != NULL ? zend_hash_num_elements(props) : 0;
	if (prop_count != 0 &&
		!ucache_shared_graph_calc_reserve(
			ctx,
			ucache_graph_property_columns_size(prop_count, false)
		)
	) {
		result = false;

		goto bailout;
	}

	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (!ucache_shared_graph_calc_reserve_key_string(ctx, prop_name) ||
				!ucache_shared_graph_calc_value(ctx, src_val)
			) {
				result = false;

				break;
			}
		} ZEND_HASH_FOREACH_END();
	}

bailout:
	if (props != NULL) {
		zend_release_properties(props);
	}

	return result;
}

static bool ucache_shared_graph_calc_object(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value)
{
	zend_string *case_name, *serdes_blob;
	zend_class_entry *ce;
	zend_object *obj;

	obj = Z_OBJ_P(value);
	ce = obj->ce;
	if (ce == zend_ce_closure) {
		zend_type_error(PHP_UCACHE_MSG_CLOSURE_UNSTORABLE);

		return false;
	}

	/* Do not initialize lazy objects while walking serialized state. */
	if (zend_object_is_lazy(obj)) {
		zend_type_error(PHP_UCACHE_MSG_LAZY_OBJECT_UNSTORABLE);

		return false;
	}

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		/* Enum cases are singletons: repeated occurrences share one node. */
		if (!php_ucache_seen_test_and_add(&ctx->enum_dedup, obj)) {
			return true;
		}

		case_name = Z_STR_P(zend_enum_fetch_case_name(obj));

		return ucache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_ucache_shared_graph_enum_t)
			) &&
			ucache_shared_graph_calc_reserve_key_string(ctx, ce->name) &&
			ucache_shared_graph_calc_reserve_key_string(ctx, case_name)
		;
	}

	switch (ucache_shared_graph_classify_object_route(ce)) {
		case PHP_UCACHE_OBJECT_ROUTE_SAFE_DIRECT:
			return ucache_shared_graph_calc_safe_direct_object(ctx, value, obj);
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
			return ucache_shared_graph_calc_magic_state_object(
				ctx, value, obj, PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE
			);
		case PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return ucache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS
			);
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_shared_graph_calc_magic_state_object(
				ctx, value, obj, PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE
			);
		case PHP_UCACHE_OBJECT_ROUTE_SLEEP:
			return ucache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_UCACHE_OBJECT_ROUTE_SLEEP
			);
		case PHP_UCACHE_OBJECT_ROUTE_WAKEUP:
			return ucache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_UCACHE_OBJECT_ROUTE_WAKEUP
			);
		case PHP_UCACHE_OBJECT_ROUTE_SERDES:
			if (!php_ucache_seen_test_and_add(&ctx->seen_objects, obj)) {
				return true;
			}

			if (!ucache_shared_graph_get_serdes_blob(value, ctx->state_memo, &serdes_blob)) {
				return false;
			}

			return ucache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_ucache_shared_graph_serdes_object_t) + ZSTR_LEN(serdes_blob)
			);
		case PHP_UCACHE_OBJECT_ROUTE_PLAIN:
			return ucache_shared_graph_calc_plain_object(ctx, value, obj);
		case PHP_UCACHE_OBJECT_ROUTE_UNSTORABLE:
			return false;
	}

	return false;
}

static bool ucache_shared_graph_calc_array(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value)
{
	const HashTable *arr;
	zend_string *key;
	zval *elem, *verdict, verdict_zv;
	zend_ulong verbatim_hash;
	bool verbatim;

	arr = Z_ARRVAL_P(value);

	if (arr->nNumOfElements == 0) {
		if (arr->nNextFreeElement != 0 && arr->nNextFreeElement != ZEND_LONG_MIN) {
			return ucache_shared_graph_calc_reserve(ctx, sizeof(php_ucache_shared_graph_array_t)) &&
					(
						!ucache_shared_graph_next_free_is_wide(arr->nNextFreeElement) ||
						ucache_shared_graph_calc_reserve(ctx, sizeof(int64_t))
					)
			;
		}

		return true;
	}

	if (ctx->verbatim_arrays_allowed) {
		if (GC_FLAGS(arr) & IS_ARRAY_IMMUTABLE) {
			return ucache_shared_graph_calc_verbatim_value(ctx, value, &verbatim_hash);
		}

		verdict = NULL;

		if (ctx->shared_verdicts != NULL &&
			(
				ctx->state_memo == NULL ||
				zend_hash_num_elements(ctx->state_memo) == 0
			)
		) {
			verdict = zend_hash_index_find(ctx->shared_verdicts, (zend_ulong) (uintptr_t) arr);
		}

		if (verdict != NULL) {
			verbatim = Z_TYPE_P(verdict) == IS_TRUE;
		} else {
			verbatim = ucache_shared_graph_can_copy_verbatim_value(
				&ctx->direct_verdicts,
				value
			);

			if (ctx->shared_verdicts != NULL) {
				ZVAL_BOOL(&verdict_zv, verbatim);
				zend_hash_index_add(ctx->shared_verdicts, (zend_ulong) (uintptr_t) arr, &verdict_zv);
			}
		}

		if (verbatim) {
			return ucache_shared_graph_calc_verbatim_value(ctx, value, &verbatim_hash);
		}
	}

	if (!php_ucache_seen_test_and_add(&ctx->seen_arrays, arr)) {
		return true;
	}

	if (ucache_shared_graph_array_has_shape(arr)) {
		if (!ucache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_ucache_shared_graph_shaped_array_t)
			) ||
			!ucache_shared_graph_calc_reserve(
				ctx,
				ucache_graph_value_columns_size(arr->nNumOfElements)
			) ||
			!ucache_shared_graph_calc_array_shape(ctx, arr, NULL)
		) {
			return false;
		}

		ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
			if (!ucache_shared_graph_calc_value(ctx, elem)) {
				return false;
			}
		} ZEND_HASH_FOREACH_END();

		return true;
	}

	if (!ucache_shared_graph_calc_reserve(ctx, sizeof(php_ucache_shared_graph_array_t)) ||
		!ucache_shared_graph_calc_reserve(
			ctx,
			ucache_graph_array_columns_size(arr->nNumOfElements, ucache_graph_array_key_flags(arr))
		) ||
		(
			ucache_shared_graph_next_free_is_wide(arr->nNextFreeElement) &&
			!ucache_shared_graph_calc_reserve(ctx, sizeof(int64_t))
		)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL((HashTable *) arr, key, elem) {
		if (key != NULL && !ucache_shared_graph_calc_reserve_key_string(ctx, key)) {
			return false;
		}

		if (!ucache_shared_graph_calc_value(ctx, elem)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool ucache_shared_graph_calc_value(
		php_ucache_shared_graph_calc_ctx_t *ctx,
		const zval *value)
{
	zend_reference *ref;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
		case IS_LONG:
		case IS_DOUBLE:
			return true;
		case IS_STRING:
			return ucache_shared_graph_calc_reserve_string(ctx, Z_STR_P(value));
		case IS_RESOURCE:
			zend_type_error(PHP_UCACHE_MSG_RESOURCE_UNSTORABLE);

			return false;
		case IS_ARRAY:
			return ucache_shared_graph_calc_array(ctx, value);
		case IS_OBJECT:
			return ucache_shared_graph_calc_object(ctx, value);
		case IS_REFERENCE:
			ref = Z_REF_P(value);

			if (!php_ucache_seen_test_and_add(&ctx->seen_references, ref)) {
				return true;
			}

			if (!ucache_shared_graph_calc_reserve(ctx,
				sizeof(php_ucache_shared_graph_reference_t))
			) {
				return false;
			}

			return ucache_shared_graph_calc_value(ctx, &ref->val);
		default:
			return false;
	}
}

static void ucache_shared_graph_copy_init(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		uint8_t *buf,
		size_t size)
{
	ctx->buffer = buf;
	ctx->size = size;
	ctx->position = 0;
	ctx->fixup_offsets = NULL;
	ctx->fixup_count = 0;
	ctx->fixup_capacity = 0;

	zend_hash_init(&ctx->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_references, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->string_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->array_shape_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->state_schema_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_array_dedup, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->verbatim_content_dedup, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->own_content_hashes, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->own_canonicals, 32, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->enum_dedup, 8, NULL, NULL, 0);

	ctx->has_shared_identity = false;
	ctx->has_object = false;
	ctx->prefers_prototype = false;
	ctx->has_userland_restore_object = false;
	ctx->has_verbatim_array = false;
	ctx->verbatim_arrays_allowed = ucache_shared_graph_can_use_verbatim_arrays();
	ctx->shared_verdicts = NULL;
	ctx->verbatim_content_hashes = &ctx->own_content_hashes;
	ctx->verbatim_canonicals = &ctx->own_canonicals;
	ctx->dedup_verbatim_content = false;
	ctx->records = NULL;
	ctx->record_count = 0;
	ctx->record_capacity = 0;
	ctx->intern = NULL;
	ctx->intern_list = NULL;
}

static void ucache_shared_graph_copy_destroy(php_ucache_shared_graph_copy_ctx_t *ctx)
{
	if (ctx->fixup_offsets != NULL) {
		efree(ctx->fixup_offsets);

		ctx->fixup_offsets = NULL;
	}

	if (ctx->records != NULL) {
		efree(ctx->records);

		ctx->records = NULL;
	}

	zend_hash_destroy(&ctx->enum_dedup);
	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->own_canonicals);
	zend_hash_destroy(&ctx->own_content_hashes);
	zend_hash_destroy(&ctx->verbatim_content_dedup);
	zend_hash_destroy(&ctx->direct_array_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->array_shape_dedup);
	zend_hash_destroy(&ctx->string_dedup);
	zend_hash_destroy(&ctx->seen_references);
	zend_hash_destroy(&ctx->seen_objects);
	zend_hash_destroy(&ctx->seen_arrays);
}

static bool ucache_shared_graph_seen_record_object_offsets(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_object *obj,
		uint32_t obj_offset,
		uint32_t flags_offset)
{
	uint32_t delta;

	delta = flags_offset - obj_offset;

	ZEND_ASSERT((obj_offset & 3) == 0);
	ZEND_ASSERT((delta == 4 || delta == 12) && "unexpected flags field offset");

	return zend_hash_index_add_ptr(
		&ctx->seen_objects,
		(zend_ulong) (uintptr_t) obj,
		(void *) (uintptr_t) (obj_offset | (delta >> 2))
	) != NULL;
}

static bool ucache_shared_graph_copy_emit_object_ref_if_seen(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_object *obj,
		php_ucache_shared_graph_value_t *dst)
{
	uint32_t packed, obj_offset, flags_offset;
	void *seen_offset;

	seen_offset = zend_hash_index_find_ptr(&ctx->seen_objects, (zend_ulong) (uintptr_t) obj);
	if (seen_offset == NULL) {
		return false;
	}

	packed = (uint32_t) (uintptr_t) seen_offset;
	obj_offset = packed & ~(uint32_t) 3;
	flags_offset = obj_offset + ((packed & 3) << 2);

	*(uint32_t *) (ctx->buffer + flags_offset) |= PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED;
	ctx->has_shared_identity = true;

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT_REF;
	dst->payload.offset = obj_offset;

	return true;
}

static bool ucache_shared_graph_copy_alloc(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		size_t amount,
		uint32_t *offset)
{
	size_t aligned_amount;

	aligned_amount = PHP_UCACHE_ALIGNED_SIZE(amount);
	if (ctx->position > ctx->size || aligned_amount > ctx->size - ctx->position) {
		return false;
	}

	*offset = (uint32_t) ctx->position;

	if (aligned_amount > amount) {
		memset(ctx->buffer + ctx->position + amount, 0, aligned_amount - amount);
	}

	ctx->position += aligned_amount;

	return true;
}

/* The returned pointer is valid until the next new string is recorded. */
static zend_always_inline php_ucache_graph_string_record_t *ucache_shared_graph_copy_string_record(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_string *str)
{
	php_ucache_graph_string_record_t *record;
	zval *entry = zend_hash_lookup(&ctx->string_dedup, (zend_string *) str);

	if (Z_TYPE_P(entry) != IS_NULL) {
		return &ctx->records[Z_LVAL_P(entry)];
	}

	if (ctx->record_count == ctx->record_capacity) {
		ctx->record_capacity = ctx->record_capacity == 0 ? 16 : ctx->record_capacity * 2;
		ctx->records = erealloc(ctx->records, sizeof(*ctx->records) * ctx->record_capacity);
	}

	record = &ctx->records[ctx->record_count];
	memset(record, 0, sizeof(*record));

	ZVAL_LONG(entry, (zend_long) ctx->record_count++);

	return record;
}

static bool ucache_shared_graph_copy_string(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_string *str,
		uint32_t *offset)
{
	php_ucache_graph_string_record_t *record;
	zend_string *new_str;
	uint32_t str_offset;
	size_t str_size;

	record = ucache_shared_graph_copy_string_record(ctx, str);
	if (record->payload_offset != 0) {
		*offset = record->payload_offset;

		return true;
	}

	str_size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(str));
	if (!ucache_shared_graph_copy_alloc(ctx, str_size, &str_offset)) {
		return false;
	}

	new_str = (zend_string *) (ctx->buffer + str_offset);

	memcpy(new_str, str, str_size);

	/* Shared-segment strings are pinned as interned+permanent so the engine
	 * never refcounts or frees them. */
	GC_SET_REFCOUNT(new_str, 2);
	GC_TYPE_INFO(new_str) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	*offset = str_offset;
	record->payload_offset = str_offset;

	return true;
}

static uint32_t ucache_graph_intern_plan_push_candidate(
		php_ucache_graph_intern_plan_t *plan,
		uint32_t payload_offset)
{
	if (plan->candidate_count == plan->candidate_capacity) {
		plan->candidate_capacity = plan->candidate_capacity == 0 ? 8 : plan->candidate_capacity * 2;
		plan->candidates = erealloc(plan->candidates, sizeof(*plan->candidates) * plan->candidate_capacity);
	}

	plan->candidates[plan->candidate_count] = payload_offset;

	return plan->candidate_count++;
}

static void ucache_graph_intern_plan_push_site(
		php_ucache_graph_intern_plan_t *plan,
		uint32_t candidate,
		uint32_t site_offset,
		bool pointer)
{
	php_ucache_graph_intern_site_t *site;

	if (plan->site_count == plan->site_capacity) {
		plan->site_capacity = plan->site_capacity == 0 ? 16 : plan->site_capacity * 2;
		plan->sites = erealloc(plan->sites, sizeof(*plan->sites) * plan->site_capacity);
	}

	site = &plan->sites[plan->site_count++];
	site->candidate = candidate;
	site->site_offset = site_offset;
	site->pointer = pointer;
}

/* False only when the in-payload copy of a candidate could not be made. */
static bool ucache_shared_graph_intern_key(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_string *str,
		php_ucache_graph_key_ref_t *ref)
{
	php_ucache_graph_intern_plan_t *plan = ctx->intern;
	php_ucache_graph_string_record_t *record;
	uint64_t generation;
	uint32_t seg_offset;

	memset(ref, 0, sizeof(*ref));

	if (plan == NULL || !php_ucache_intern_eligible(str)) {
		return true;
	}

	record = ucache_shared_graph_copy_string_record(ctx, str);

	if (record->intern_offset != 0) {
		ref->kind = PHP_UCACHE_GRAPH_KEY_INTERNED;
		ref->segment_offset = record->intern_offset;

		return true;
	}

	if (record->intern_candidate != 0) {
		ref->kind = PHP_UCACHE_GRAPH_KEY_CANDIDATE;
		ref->payload_offset = record->payload_offset;
		ref->candidate = record->intern_candidate - 1;

		return true;
	}

	if (plan->list_count + plan->candidate_count >= plan->list_capacity) {
		return true;
	}

	seg_offset = php_ucache_intern_find((zend_string *) str, &generation);
	if (seg_offset != 0) {
		if (plan->generation == 0) {
			plan->generation = generation;
		} else if (plan->generation != generation) {
			plan->generation_conflict = true;
		}

		ctx->intern_list[plan->list_count++] = seg_offset;
		record->intern_offset = seg_offset;

		ref->kind = PHP_UCACHE_GRAPH_KEY_INTERNED;
		ref->segment_offset = seg_offset;

		return true;
	}

	/* str is already recorded, so record survives this copy. */
	if (!ucache_shared_graph_copy_string(ctx, str, &ref->payload_offset)) {
		return false;
	}

	ref->kind = PHP_UCACHE_GRAPH_KEY_CANDIDATE;
	ref->candidate = ucache_graph_intern_plan_push_candidate(plan, ref->payload_offset);
	record->intern_candidate = ref->candidate + 1;

	return true;
}

static bool ucache_shared_graph_copy_key_string(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_string *str,
		uint32_t *field)
{
	php_ucache_graph_key_ref_t ref;

	if (!ucache_shared_graph_intern_key(ctx, str, &ref)) {
		return false;
	}

	switch (ref.kind) {
		case PHP_UCACHE_GRAPH_KEY_INTERNED:
			*field = ref.segment_offset | PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG;

			return true;
		case PHP_UCACHE_GRAPH_KEY_CANDIDATE:
			*field = ref.payload_offset;

			ucache_graph_intern_plan_push_site(
				ctx->intern,
				ref.candidate,
				(uint32_t) ((const uint8_t *) field - ctx->buffer),
				false
			);

			return true;
		default:
			return ucache_shared_graph_copy_string(ctx, str, field);
	}
}

/* A verbatim bucket key: an absolute pointer, valid in every process of
 * the fork context that shares the segment mapping. */
static bool ucache_shared_graph_copy_key_string_ptr(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zend_string *str,
		zend_string **field)
{
	php_ucache_graph_key_ref_t ref;

	if (!ucache_shared_graph_intern_key(ctx, str, &ref)) {
		return false;
	}

	if (ref.kind == PHP_UCACHE_GRAPH_KEY_INTERNED) {
		*field = (zend_string *) php_ucache_ptr(ref.segment_offset);

		return true;
	}

	if (ref.kind == PHP_UCACHE_GRAPH_KEY_PLAIN &&
		!ucache_shared_graph_copy_string(ctx, str, &ref.payload_offset)
	) {
		return false;
	}

	*field = (zend_string *) (void *) (ctx->buffer + ref.payload_offset);

	ucache_shared_graph_copy_record_fixup(ctx, field);

	if (ref.kind == PHP_UCACHE_GRAPH_KEY_CANDIDATE) {
		ucache_graph_intern_plan_push_site(
			ctx->intern,
			ref.candidate,
			(uint32_t) ((const uint8_t *) field - ctx->buffer),
			true
		);
	}

	return true;
}

static bool ucache_shared_graph_copy_array_shape(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const HashTable *arr,
		uint32_t *offset,
		zend_string *shape_key)
{
	php_ucache_shared_graph_array_shape_t *graph_shape;
	php_ucache_shared_graph_array_shape_element_t *shape_elems, *shape_elem;
	zend_string *key;
	zval *cached, cached_offset;
	uint32_t shape_offset, elems_offset;
	bool result = true, owns_shape_key = false;

	if (shape_key == NULL) {
		shape_key = ucache_shared_graph_array_shape_key(arr);
		if (shape_key == NULL) {
			return false;
		}

		owns_shape_key = true;
	}

	cached = zend_hash_find(&ctx->array_shape_dedup, shape_key);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		goto bailout;
	}

	if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*graph_shape), &shape_offset) ||
		!ucache_shared_graph_copy_alloc(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(*shape_elems),
			&elems_offset
		)
	) {
		result = false;

		goto bailout;
	}

	graph_shape = (php_ucache_shared_graph_array_shape_t *) (ctx->buffer + shape_offset);
	graph_shape->count = (uint32_t) arr->nNumOfElements;
	graph_shape->elements_offset = elems_offset;

	shape_elems = (php_ucache_shared_graph_array_shape_element_t *) (ctx->buffer + elems_offset);
	shape_elem = shape_elems;

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		ZEND_ASSERT(key != NULL);

		if (!ucache_shared_graph_copy_key_string(ctx, key, &shape_elem->key_offset)) {
			result = false;

			break;
		}

		++shape_elem;
	} ZEND_HASH_FOREACH_END();

	if (result) {
		ZVAL_LONG(&cached_offset, (zend_long) shape_offset);
		result = zend_hash_add(&ctx->array_shape_dedup, shape_key, &cached_offset) != NULL;
	}

	if (result) {
		*offset = shape_offset;
	}

bailout:
	if (owns_shape_key) {
		zend_string_release(shape_key);
	}

	return result;
}

static bool ucache_shared_graph_copy_state_schema(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		zend_class_entry *ce,
		const HashTable *arr,
		uint32_t *offset)
{
	php_ucache_shared_graph_state_schema_t *schema;
	zend_string *schema_key, *shape_key;
	zval *cached, cached_offset;
	uint32_t schema_offset, shape_offset;
	bool result = true;

	shape_key = ucache_shared_graph_array_shape_key(arr);
	if (shape_key == NULL) {
		return false;
	}

	schema_key = ucache_shared_graph_state_schema_key(ce, shape_key);
	if (schema_key == NULL) {
		zend_string_release(shape_key);

		return false;
	}

	cached = zend_hash_find(&ctx->state_schema_dedup, schema_key);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		goto bailout;
	}

	if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*schema), &schema_offset)) {
		result = false;

		goto bailout;
	}

	schema = (php_ucache_shared_graph_state_schema_t *) (ctx->buffer + schema_offset);
	schema->class_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(ctx, ce->name, &schema->class_name_offset) ||
		!ucache_shared_graph_copy_array_shape(ctx, arr, &shape_offset, shape_key)
	) {
		result = false;

		goto bailout;
	}

	schema->shape_offset = shape_offset;
	schema->count = (uint32_t) arr->nNumOfElements;

	ZVAL_LONG(&cached_offset, (zend_long) schema_offset);
	if (zend_hash_add(&ctx->state_schema_dedup, schema_key, &cached_offset) == NULL) {
		result = false;

		goto bailout;
	}

	*offset = schema_offset;

bailout:
	zend_string_release(shape_key);
	zend_string_release(schema_key);

	return result;
}

static bool ucache_shared_graph_copy_value_columns(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const HashTable *arr,
		uint32_t values_offset)
{
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t value;
	zval *elem;
	uint32_t i = 0;

	memset(ctx->buffer + values_offset, 0, (size_t) ucache_graph_value_columns_size(arr->nNumOfElements));
	ucache_graph_value_columns_locate(ctx->buffer + values_offset, arr->nNumOfElements, &cols);

	ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
		if (i == arr->nNumOfElements ||
			!ucache_shared_graph_copy_value(ctx, elem, &value)
		) {
			return false;
		}

		ucache_graph_columns_store_value(&cols, i, &value);

		++i;
	} ZEND_HASH_FOREACH_END();

	return i == arr->nNumOfElements;
}

static bool ucache_shared_graph_copy_verbatim_value(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zval *dst)
{
	const zval *src_packed;
	const HashTable *src_arr, *shared_src;
	const Bucket *src_bucket;
	zend_ulong arr_key, content_hash = 0;
	zend_array *target;
	zval *dst_packed, *cached, cached_offset;
	Bucket *dst_bucket;
	uint32_t i, string_offset, array_offset, data_offset;
	size_t data_size;
	bool hashed_here = false;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_TRUE:
		case IS_FALSE:
			/* ZVAL_NULL() and friends write the type tag only, and PHP never
			 * initializes the value union of these types, so copying it would
			 * leak uninitialized heap bytes into the shared segment. */
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
			if (!ucache_shared_graph_copy_string(ctx, Z_STR_P(src), &string_offset)) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, (zend_string *) (void *) (ctx->buffer + string_offset));

			ucache_shared_graph_copy_record_fixup(ctx, dst);

			return true;
		case IS_ARRAY:
			src_arr = Z_ARRVAL_P(src);
			if (src_arr->nNumOfElements == 0) {
				ZVAL_EMPTY_ARRAY(dst);

				return true;
			}

			arr_key = (zend_ulong) (uintptr_t) src_arr;
			if (GC_REFCOUNT(src_arr) > 1) {
				cached = zend_hash_index_find(&ctx->direct_array_dedup, arr_key);
				if (cached != NULL) {
					return ucache_shared_graph_copy_verbatim_array_ref(ctx, (uint32_t) Z_LVAL_P(cached), dst);
				}
			}

			if (ctx->dedup_verbatim_content) {
				shared_src = zend_hash_index_find_ptr(ctx->verbatim_canonicals, arr_key);
				if (shared_src == NULL) {
					if (!ucache_shared_graph_verbatim_content_hash(ctx->verbatim_content_hashes, src_arr, &content_hash)) {
						return false;
					}

					shared_src = ucache_shared_graph_verbatim_canonical(
						&ctx->verbatim_content_dedup,
						ctx->verbatim_canonicals,
						content_hash,
						src_arr
					);

					hashed_here = true;
				}

				if (shared_src != src_arr) {
					cached = zend_hash_index_find(&ctx->direct_array_dedup, (zend_ulong) (uintptr_t) shared_src);
					if (cached == NULL) {
						return false;
					}

					return ucache_shared_graph_copy_verbatim_array_ref(ctx, (uint32_t) Z_LVAL_P(cached), dst);
				}
			}

			data_size = HT_IS_PACKED(src_arr) ? HT_PACKED_USED_SIZE(src_arr) : HT_USED_SIZE(src_arr);
			if (!ucache_shared_graph_copy_alloc(ctx, sizeof(zend_array), &array_offset) ||
				!ucache_shared_graph_copy_alloc(ctx, data_size, &data_offset)
			) {
				return false;
			}

			target = (zend_array *) (ctx->buffer + array_offset);

			memcpy(target, src_arr, sizeof(zend_array));
			memcpy(ctx->buffer + data_offset, HT_GET_DATA_ADDR(src_arr), data_size);

			/* Freeze the shared copy: immutable, uncollectable and
			 * destructor-free so readers can use it without refcounting. */
			GC_SET_REFCOUNT(target, 2);
			GC_TYPE_INFO(target) = GC_ARRAY | ((IS_ARRAY_IMMUTABLE | GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT);

			HT_FLAGS(target) |= HASH_FLAG_STATIC_KEYS;
			HT_SET_ITERATORS_COUNT(target, 0);

			target->pDestructor = NULL;
			target->nInternalPointer = 0;

			HT_SET_DATA_ADDR(target, ctx->buffer + data_offset);

			/* arData moves with the containing buffer. */
			ucache_shared_graph_copy_record_fixup(ctx, &target->arData);

			if (HT_IS_PACKED(src_arr)) {
				dst_packed = target->arPacked;
				for (i = 0; i < src_arr->nNumUsed; i++) {
					src_packed = &src_arr->arPacked[i];

					/* Packed slots leave zval.u2 (and, for holes, the value
					 * union) uninitialized, so the memcpy above copied heap
					 * garbage; zero the slot before writing its value. */
					memset(&dst_packed[i], 0, sizeof(zval));

					if (!ucache_shared_graph_copy_verbatim_value(ctx, src_packed, &dst_packed[i])) {
						return false;
					}
				}
			} else {
				src_bucket = src_arr->arData;
				dst_bucket = target->arData;
				for (i = 0; i < src_arr->nNumUsed; i++) {
					if (src_bucket[i].key != NULL) {
						if (!ucache_shared_graph_copy_key_string_ptr(
								ctx,
								src_bucket[i].key,
								&dst_bucket[i].key
							)
						) {
							return false;
						}
					} else {
						dst_bucket[i].key = NULL;
					}

					if (!ucache_shared_graph_copy_verbatim_value(ctx, &src_bucket[i].val, &dst_bucket[i].val)) {
						return false;
					}
				}
			}

			if (hashed_here) {
				zend_hash_index_add_ptr(&ctx->verbatim_content_dedup, content_hash, (void *) src_arr);
			}

			if (ctx->dedup_verbatim_content || GC_REFCOUNT(src_arr) > 1) {
				ZVAL_LONG(&cached_offset, (zend_long) array_offset);
				zend_hash_index_add(&ctx->direct_array_dedup, arr_key, &cached_offset);
			}

			return ucache_shared_graph_copy_verbatim_array_ref(ctx, array_offset, dst);
		default:
			return false;
	}
}

static bool ucache_shared_graph_copy_shaped_state_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		zend_object *obj,
		const HashTable *state_array,
		uint8_t node_type,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_shaped_state_object_t *graph_shaped_state;
	uint32_t shaped_state_offset, state_schema_offset, state_values_offset;

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_ucache_shared_graph_shaped_state_object_t),
			&shaped_state_offset
		) ||
		!ucache_shared_graph_copy_state_schema(
			ctx,
			obj->ce,
			state_array,
			&state_schema_offset
		) ||
		!ucache_shared_graph_copy_alloc(
			ctx,
			(size_t) ucache_graph_value_columns_size(state_array->nNumOfElements),
			&state_values_offset
		)
	) {
		return false;
	}

	if (!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			shaped_state_offset,
			shaped_state_offset + offsetof(php_ucache_shared_graph_shaped_state_object_t, flags)
		)
	) {
		return false;
	}

	graph_shaped_state = (php_ucache_shared_graph_shaped_state_object_t *)
		(ctx->buffer + shaped_state_offset)
	;
	graph_shaped_state->state_schema_offset = state_schema_offset;
	graph_shaped_state->flags = 0;
	graph_shaped_state->state_values_offset = state_values_offset;
	graph_shaped_state->state_next_free = (uint32_t) state_array->nNextFreeElement;

	if (!ucache_shared_graph_copy_value_columns(
			ctx,
			state_array,
			state_values_offset
		)
	) {
		return false;
	}

	dst->type = node_type;
	dst->payload.offset = shaped_state_offset;

	return true;
}

static bool ucache_shared_graph_copy_magic_state_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zend_object *obj,
		php_ucache_object_route_t route,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_safe_direct_object_t *graph_safe_direct;
	php_ucache_shared_graph_state_producer_t produce_state;
	HashTable *sd_state_ht;
	zval *sd_state_ptr, sd_state_zv;
	uint32_t sd_offset;

	ctx->has_userland_restore_object = true;

	produce_state = ucache_shared_graph_route_state_producer(route);
	if (!ucache_shared_graph_get_memoized_state(src, ctx->state_memo, produce_state, &sd_state_ptr)) {
		return false;
	}

	/* sd_state_ptr may dangle once the memo resizes (schema verdicts insert
	 * into the same table); keep the stable array pointer instead. */
	sd_state_ht = Z_ARRVAL_P(sd_state_ptr);

	if (ucache_shared_graph_state_array_fits_schema_memo(ctx->state_memo, sd_state_ht)) {
		return ucache_shared_graph_copy_shaped_state_object(
			ctx,
			obj,
			sd_state_ht,
			PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT,
			dst
		);
	}

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_ucache_shared_graph_safe_direct_object_t), &sd_offset
		)
	) {
		return false;
	}

	graph_safe_direct = (php_ucache_shared_graph_safe_direct_object_t *) (ctx->buffer + sd_offset);
	graph_safe_direct->class_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(ctx, obj->ce->name, &graph_safe_direct->class_name_offset) ||
		!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(php_ucache_shared_graph_safe_direct_object_t, flags)
		)
	) {
		return false;
	}

	graph_safe_direct->property_count = 0;
	graph_safe_direct->properties_offset = 0;
	graph_safe_direct->flags = 0;

	ZVAL_ARR(&sd_state_zv, sd_state_ht);
	if (!ucache_shared_graph_copy_value(ctx, &sd_state_zv, &graph_safe_direct->state)) {
		return false;
	}

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT;
	dst->payload.offset = sd_offset;

	return true;
}

static bool ucache_shared_graph_copy_sleep_state_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zend_object *obj,
		php_ucache_object_route_t route,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_object_t *graph_obj;
	php_ucache_shared_graph_state_producer_t produce_state;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t prop_value;
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *state_ptr, *prop_val;
	HashTable *props;
	uint32_t obj_offset, properties_offset,
		prop_idx, prop_count
	;
	bool result;

	/* Values requiring restore hooks cannot use request-local cloning. */
	if (route != PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS ||
		php_ucache_class_has_wakeup(obj->ce)
	) {
		ctx->has_userland_restore_object = true;
	}

	produce_state = ucache_shared_graph_route_state_producer(route);
	if (!ucache_shared_graph_get_memoized_state(src, ctx->state_memo, produce_state, &state_ptr)) {
		return false;
	}

	props = Z_ARRVAL_P(state_ptr);
	if (ucache_shared_graph_state_array_fits_schema_memo(ctx->state_memo, props)) {
		return ucache_shared_graph_copy_shaped_state_object(
			ctx,
			obj,
			props,
			PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT,
			dst
		);
	}

	prop_count = zend_hash_num_elements(props);
	properties_offset = 0;

	if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*graph_obj), &obj_offset)) {
		return false;
	}

	graph_obj = (php_ucache_shared_graph_object_t *) (ctx->buffer + obj_offset);
	graph_obj->class_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(ctx, obj->ce->name, &graph_obj->class_name_offset) ||
		(prop_count != 0 &&
			!ucache_shared_graph_copy_alloc(
				ctx,
				(size_t) ucache_graph_property_columns_size(prop_count, true),
				&properties_offset
			)
		) ||
		!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(php_ucache_shared_graph_object_t, flags)
		)
	) {
		return false;
	}

	graph_obj->property_count = prop_count;
	graph_obj->properties_offset = properties_offset;
	graph_obj->flags = 0;

	if (prop_count == 0) {
		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT;
		dst->payload.offset = obj_offset;

		return true;
	}

	memset(ctx->buffer + properties_offset, 0, (size_t) ucache_graph_property_columns_size(prop_count, true));
	ucache_graph_property_columns_locate(ctx->buffer + properties_offset, prop_count, true, &cols);
	prop_idx = 0;
	result = true;

	/* Native serialization restores integer keys as property names. */
	ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
		if (prop_idx == prop_count) {
			result = false;

			break;
		}

		resolved_name = prop_name != NULL
			? zend_string_copy(prop_name)
			: zend_long_to_str((zend_long) num_key)
		;

		cols.sleep_indices[prop_idx] =
			php_ucache_serdes_declared_property_index_plus_one(obj->ce, resolved_name)
		;

		if (!ucache_shared_graph_copy_key_string(
				ctx,
				resolved_name,
				&cols.name_offsets[prop_idx]
			) ||
			!ucache_shared_graph_copy_value(
				ctx,
				prop_val,
				&prop_value
			)
		) {
			zend_string_release(resolved_name);
			result = false;

			break;
		}

		ucache_graph_columns_store_value(&cols, prop_idx, &prop_value);

		zend_string_release(resolved_name);
		++prop_idx;
	} ZEND_HASH_FOREACH_END();

	if (result) {
		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT;
		dst->payload.offset = obj_offset;
	}

	return result;
}

static bool ucache_shared_graph_copy_safe_direct_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zend_object *obj,
		php_ucache_shared_graph_value_t *dst)
{
	HashTable *sd_state_ht;
	php_ucache_shared_graph_safe_direct_object_t *graph_safe_direct;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t prop_value;
	zend_string *prop_name;
	zval *prop_val, *src_val, *sd_state_ptr, sd_state, sd_state_zv;
	HashTable *props;
	uint32_t prop_idx, prop_count,
		sd_offset,
		sd_props_offset
	;
	bool result;

	if (php_ucache_safe_direct_prefers_request_local_prototype(obj->ce)) {
		ctx->prefers_prototype = true;
	}

	if (!ucache_shared_graph_get_safe_direct_state(
			src,
			ctx->state_memo,
			&sd_state_ptr,
			&sd_state
		)
	) {
		return false;
	}

	/* sd_state_ptr may dangle once recursion resizes the memo; keep the array. */
	sd_state_ht = Z_ARRVAL_P(sd_state_ptr);

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_ucache_shared_graph_safe_direct_object_t),
			&sd_offset
		)
	) {
		result = false;

		goto bailout;
	}

	graph_safe_direct = (php_ucache_shared_graph_safe_direct_object_t *) (ctx->buffer + sd_offset);
	graph_safe_direct->class_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(
			ctx,
			obj->ce->name,
			&graph_safe_direct->class_name_offset
		) ||
		!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(php_ucache_shared_graph_safe_direct_object_t, flags)
		)
	) {
		result = false;

		goto bailout;
	}

	graph_safe_direct->flags = 0;

	ZVAL_ARR(&sd_state_zv, sd_state_ht);
	if (!ucache_shared_graph_copy_value(ctx, &sd_state_zv, &graph_safe_direct->state)) {
		result = false;

		goto bailout;
	}

	/* Mirror CALC: count only the emitted (non-shadowed) properties so
	 * shadowed ones cost no slot. */
	props = zend_std_get_properties(obj);
	prop_count = 0;
	result = true;
	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY(props, prop_name) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			if (!ucache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					sd_state_ht
				)
			) {
				prop_count++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (!result) {
		goto bailout;
	}

	graph_safe_direct->property_count = prop_count;
	graph_safe_direct->properties_offset = 0;

	if (prop_count != 0) {
		if (!ucache_shared_graph_copy_alloc(
				ctx,
				(size_t) ucache_graph_property_columns_size(prop_count, false),
				&sd_props_offset
			)
		) {
			result = false;

			goto bailout;
		}

		graph_safe_direct->properties_offset = sd_props_offset;
		memset(ctx->buffer + sd_props_offset, 0, (size_t) ucache_graph_property_columns_size(prop_count, false));
		ucache_graph_property_columns_locate(ctx->buffer + sd_props_offset, prop_count, false, &cols);
		prop_idx = 0;

		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			if (prop_name == NULL) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (ucache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					sd_state_ht
				)
			) {
				continue;
			}

			if (prop_idx == prop_count ||
				!ucache_shared_graph_copy_key_string(ctx, prop_name, &cols.name_offsets[prop_idx]) ||
				!ucache_shared_graph_copy_value(ctx, src_val, &prop_value)
			) {
				result = false;

				break;
			}

			ucache_graph_columns_store_value(&cols, prop_idx, &prop_value);

			++prop_idx;
		} ZEND_HASH_FOREACH_END();

		if (!result) {
			goto bailout;
		}
	}

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT;
	dst->payload.offset = sd_offset;

bailout:
	zval_ptr_dtor(&sd_state);

	return result;
}

static bool ucache_shared_graph_copy_serdes_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zend_object *obj,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_serdes_object_t *graph_serdes;
	zend_string *serdes_blob;
	uint32_t serdes_offset;

	ctx->has_userland_restore_object = true;

	if (!ucache_shared_graph_get_serdes_blob(src, ctx->state_memo, &serdes_blob)) {
		return false;
	}

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_ucache_shared_graph_serdes_object_t) + ZSTR_LEN(serdes_blob),
			&serdes_offset
		)
	) {
		return false;
	}

	if (!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			serdes_offset,
			serdes_offset + offsetof(php_ucache_shared_graph_serdes_object_t, flags)
		)
	) {
		return false;
	}

	graph_serdes = (php_ucache_shared_graph_serdes_object_t *) (ctx->buffer + serdes_offset);
	graph_serdes->blob_len = (uint32_t) ZSTR_LEN(serdes_blob);
	graph_serdes->flags = 0;

	memcpy(graph_serdes + 1, ZSTR_VAL(serdes_blob), ZSTR_LEN(serdes_blob));

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT;
	dst->payload.offset = serdes_offset;

	return true;
}

static bool ucache_shared_graph_copy_plain_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		zend_object *obj,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_object_t *graph_obj;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t prop_value;
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	uint32_t obj_offset, properties_offset,
		prop_idx, prop_count
	;
	bool result = true;

	ctx->prefers_prototype = true;

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(*graph_obj),
			&obj_offset
		) ||
		!ucache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(php_ucache_shared_graph_object_t, flags)
		)
	) {
		return false;
	}

	graph_obj = (php_ucache_shared_graph_object_t *) (ctx->buffer + obj_offset);
	graph_obj->class_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(
			ctx,
			obj->ce->name,
			&graph_obj->class_name_offset
		)
	) {
		return false;
	}

	props = zend_get_properties_for((zval *) src, ZEND_PROP_PURPOSE_SERIALIZE);
	prop_count = props != NULL ? zend_hash_num_elements(props) : 0;
	properties_offset = 0;

	if (prop_count != 0 &&
		!ucache_shared_graph_copy_alloc(
			ctx,
			(size_t) ucache_graph_property_columns_size(prop_count, false),
			&properties_offset
		)
	) {
		result = false;

		goto bailout;
	}

	graph_obj->property_count = prop_count;
	graph_obj->properties_offset = properties_offset;
	graph_obj->flags = 0;

	if (prop_count != 0) {
		memset(ctx->buffer + properties_offset, 0, (size_t) ucache_graph_property_columns_size(prop_count, false));
		ucache_graph_property_columns_locate(ctx->buffer + properties_offset, prop_count, false, &cols);
		prop_idx = 0;

		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			if (prop_name == NULL || prop_idx == prop_count) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (!ucache_shared_graph_copy_key_string(ctx, prop_name, &cols.name_offsets[prop_idx]) ||
				!ucache_shared_graph_copy_value(ctx, src_val, &prop_value)
			) {
				result = false;

				break;
			}

			ucache_graph_columns_store_value(&cols, prop_idx, &prop_value);

			++prop_idx;
		} ZEND_HASH_FOREACH_END();
	}

	if (result) {
		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT;
		dst->payload.offset = obj_offset;
	}

bailout:
	zend_release_properties(props);

	return result;
}

static bool ucache_shared_graph_copy_enum(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_enum_t *graph_enum;
	zend_class_entry *ce;
	zend_string *case_name;
	uint32_t enum_offset;
	void *seen_offset;

	/* Singleton case objects dedup by identity; the decoder is stateless,
	 * so aliased slots pointing at one node decode identically. */
	seen_offset = zend_hash_index_find_ptr(&ctx->enum_dedup, (zend_ulong) (uintptr_t) Z_OBJ_P(src));
	if (seen_offset != NULL) {
		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM;
		dst->payload.offset = (uint32_t) (uintptr_t) seen_offset;

		return true;
	}

	ce = Z_OBJCE_P(src);
	case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(src)));

	if (!ucache_shared_graph_copy_alloc(ctx,
		sizeof(php_ucache_shared_graph_enum_t), &enum_offset)
	) {
		return false;
	}

	graph_enum = (php_ucache_shared_graph_enum_t *) (ctx->buffer + enum_offset);
	graph_enum->class_name_offset = 0;
	graph_enum->case_name_offset = 0;

	if (!ucache_shared_graph_copy_key_string(ctx, ce->name, &graph_enum->class_name_offset) ||
		!ucache_shared_graph_copy_key_string(ctx, case_name, &graph_enum->case_name_offset)
	) {
		return false;
	}

	zend_hash_index_add_ptr(
		&ctx->enum_dedup,
		(zend_ulong) (uintptr_t) Z_OBJ_P(src),
		(void *) (uintptr_t) enum_offset
	);

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM;
	dst->payload.offset = enum_offset;

	return true;
}

static bool ucache_shared_graph_copy_reference(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_reference_t *graph_reference;
	zend_reference *ref;
	uint32_t shared_offset, reference_offset;
	void *seen_offset;

	ref = Z_REF_P(src);
	seen_offset = zend_hash_index_find_ptr(&ctx->seen_references, (zend_ulong) (uintptr_t) ref);

	if (seen_offset != NULL) {
		shared_offset = (uint32_t) (uintptr_t) seen_offset;
		((php_ucache_shared_graph_reference_t *) (ctx->buffer + shared_offset))->flags |=
			PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
		;

		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE_REF;
		dst->payload.offset = shared_offset;
		ctx->has_shared_identity = true;

		return true;
	}

	if (!ucache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_ucache_shared_graph_reference_t),
			&reference_offset
		) ||
		zend_hash_index_add_ptr(
			&ctx->seen_references,
			(zend_ulong) (uintptr_t) ref,
			(void *) (uintptr_t) reference_offset
		) == NULL
	) {
		return false;
	}

	graph_reference = (php_ucache_shared_graph_reference_t *) (ctx->buffer + reference_offset);
	graph_reference->flags = 0;
	graph_reference->reserved = 0;

	if (!ucache_shared_graph_copy_value(
			ctx,
			&ref->val,
			&graph_reference->inner
		)
	) {
		return false;
	}

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE;
	dst->payload.offset = reference_offset;

	ctx->has_shared_identity = true;

	return true;
}

static bool ucache_shared_graph_copy_array_next_free(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		php_ucache_shared_graph_array_t *graph_array,
		zend_long next_free)
{
	uint32_t wide_next_free_offset;
	int64_t wide_next_free;

	if (UNEXPECTED(ucache_shared_graph_next_free_is_wide(next_free))) {
		wide_next_free = (int64_t) next_free;

		if (!ucache_shared_graph_copy_alloc(ctx, sizeof(wide_next_free), &wide_next_free_offset)) {
			return false;
		}

		memcpy(ctx->buffer + wide_next_free_offset, &wide_next_free, sizeof(wide_next_free));

		graph_array->next_free = wide_next_free_offset;
		graph_array->flags |= PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_WIDE_NEXT_FREE;
	} else {
		graph_array->next_free = (uint32_t) next_free;
	}

	return true;
}

static bool ucache_shared_graph_copy_array(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst)
{
	php_ucache_shared_graph_array_t *graph_array;
	php_ucache_shared_graph_shaped_array_t *graph_shaped_array;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t elem_value;
	zend_ulong arr_key, h;
	zend_string *key;
	zval *elem, *verdict, array_value;
	uint32_t elem_idx, elem_count, key_flags,
		array_offset, elems_offset,
		shape_offset, values_offset,
		shared_offset
	;
	bool verbatim;
	void *seen_offset;

	if (Z_ARRVAL_P(src)->nNumOfElements == 0) {
		/* Only a pristine empty array (next free index 0 or the ZEND_LONG_MIN
		 * sentinel) collapses to the offset-0 empty-array sentinel; an
		 * emptied array with any other next free index still needs a node
		 * to round-trip nNextFreeElement. */
		if (Z_ARRVAL_P(src)->nNextFreeElement != 0 && Z_ARRVAL_P(src)->nNextFreeElement != ZEND_LONG_MIN) {
			if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*graph_array), &array_offset)) {
				return false;
			}

			graph_array = (php_ucache_shared_graph_array_t *) (ctx->buffer + array_offset);
			graph_array->count = 0;
			graph_array->elements_offset = 0;
			graph_array->flags = 0;

			if (!ucache_shared_graph_copy_array_next_free(
					ctx,
					graph_array,
					Z_ARRVAL_P(src)->nNextFreeElement
				)
			) {
				return false;
			}

			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
			dst->payload.offset = array_offset;

			return true;
		}

		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY;
		dst->payload.offset = 0;

		return true;
	}

	if (ctx->verbatim_arrays_allowed) {
		if (GC_FLAGS(Z_ARRVAL_P(src)) & IS_ARRAY_IMMUTABLE) {
			verbatim = true;
		} else {
			/* No state hook ran, so address-keyed verdicts are still valid. */
			verdict = ctx->shared_verdicts != NULL
				? zend_hash_index_find(ctx->shared_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(src))
				: NULL
			;
			if (verdict != NULL) {
				verbatim = Z_TYPE_P(verdict) == IS_TRUE;
			} else {
				verbatim = ucache_shared_graph_can_copy_verbatim_value(
					&ctx->direct_verdicts,
					src
				);
			}
		}

		if (verbatim) {
			if (!ucache_shared_graph_copy_verbatim_value(ctx, src, &array_value)) {
				return false;
			}

			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY;
			dst->payload.offset = (uint32_t) ((uint8_t *) Z_ARRVAL(array_value) - ctx->buffer);
			ctx->has_verbatim_array = true;

			return true;
		}
	}

	arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);

	seen_offset = zend_hash_index_find_ptr(&ctx->seen_arrays, arr_key);
	if (seen_offset != NULL) {
		shared_offset = (uint32_t) (uintptr_t) seen_offset;
		ZEND_ASSERT((uintptr_t) seen_offset < ctx->size);
		/* flags lives at the same offset in shaped and plain array nodes,
		 * so the SHARED flag can be set through the plain-array cast
		 * regardless of which kind was seen. */
		((php_ucache_shared_graph_array_t *) (ctx->buffer + shared_offset))->flags |=
			PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
		;

		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY_REF;
		dst->payload.offset = shared_offset;

		ctx->has_shared_identity = true;

		return true;
	}

	if (ucache_shared_graph_array_has_shape(Z_ARRVAL_P(src))) {
		if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*graph_shaped_array), &array_offset) ||
			!ucache_shared_graph_copy_alloc(
				ctx,
				(size_t) ucache_graph_value_columns_size(Z_ARRVAL_P(src)->nNumOfElements),
				&values_offset
			) ||
			!ucache_shared_graph_copy_array_shape(ctx, Z_ARRVAL_P(src), &shape_offset, NULL)
		) {
			return false;
		}

		graph_shaped_array = (php_ucache_shared_graph_shaped_array_t *) (ctx->buffer + array_offset);
		graph_shaped_array->count = Z_ARRVAL_P(src)->nNumOfElements;
		graph_shaped_array->next_free = (uint32_t) Z_ARRVAL_P(src)->nNextFreeElement;
		graph_shaped_array->shape_offset = shape_offset;
		graph_shaped_array->flags = 0;
		graph_shaped_array->values_offset = values_offset;

		if (zend_hash_index_add_ptr(&ctx->seen_arrays, arr_key, (void *) (uintptr_t) array_offset) == NULL) {
			return false;
		}

		if (!ucache_shared_graph_copy_value_columns(ctx, Z_ARRVAL_P(src), values_offset)) {
			return false;
		}

		dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY;
		dst->payload.offset = array_offset;

		return true;
	}

	elem_count = Z_ARRVAL_P(src)->nNumOfElements;
	key_flags = ucache_graph_array_key_flags(Z_ARRVAL_P(src));

	if (!ucache_shared_graph_copy_alloc(ctx, sizeof(*graph_array), &array_offset) ||
		!ucache_shared_graph_copy_alloc(
			ctx,
			(size_t) ucache_graph_array_columns_size(elem_count, key_flags),
			&elems_offset
		)
	) {
		return false;
	}

	graph_array = (php_ucache_shared_graph_array_t *) (ctx->buffer + array_offset);
	graph_array->count = elem_count;
	graph_array->elements_offset = elems_offset;
	graph_array->flags = key_flags;

	if (!ucache_shared_graph_copy_array_next_free(ctx, graph_array, Z_ARRVAL_P(src)->nNextFreeElement)) {
		return false;
	}

	if (zend_hash_index_add_ptr(&ctx->seen_arrays, arr_key, (void *) (uintptr_t) array_offset) == NULL) {
		return false;
	}

	memset(ctx->buffer + elems_offset, 0, (size_t) ucache_graph_array_columns_size(elem_count, key_flags));
	ucache_graph_array_columns_locate(ctx->buffer + elems_offset, elem_count, key_flags, &cols);
	elem_idx = 0;

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(src), h, key, elem) {
		if (elem_idx == elem_count) {
			return false;
		}

		if (key != NULL) {
			if (cols.key_offsets == NULL ||
				!ucache_shared_graph_copy_key_string(ctx, key, &cols.key_offsets[elem_idx])
			) {
				return false;
			}
		} else if (cols.hashes != NULL) {
			cols.hashes[elem_idx] = h;
		} else if (h != elem_idx) {
			/* A packed block encodes the index implicitly. */
			return false;
		}

		if (!ucache_shared_graph_copy_value(ctx, elem, &elem_value)) {
			return false;
		}

		ucache_graph_columns_store_value(&cols, elem_idx, &elem_value);

		++elem_idx;
	} ZEND_HASH_FOREACH_END();

	dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
	dst->payload.offset = array_offset;

	return true;
}

static bool ucache_shared_graph_copy_object(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst)
{
	zend_class_entry *ce;
	zend_object *obj;

	ctx->has_object = true;
	ce = Z_OBJCE_P(src);

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		return ucache_shared_graph_copy_enum(ctx, src, dst);
	}

	obj = Z_OBJ_P(src);

	if (ucache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst)) {
		return true;
	}

	switch (ucache_shared_graph_classify_object_route(obj->ce)) {
		case PHP_UCACHE_OBJECT_ROUTE_SAFE_DIRECT:
			return ucache_shared_graph_copy_safe_direct_object(ctx, src, obj, dst);
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
			return ucache_shared_graph_copy_magic_state_object(
				ctx, src, obj, PHP_UCACHE_OBJECT_ROUTE_MAGIC_SERIALIZE, dst
			);
		case PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return ucache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_UCACHE_OBJECT_ROUTE_SERIALIZE_PROPS, dst
			);
		case PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return ucache_shared_graph_copy_magic_state_object(
				ctx, src, obj, PHP_UCACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE, dst
			);
		case PHP_UCACHE_OBJECT_ROUTE_SLEEP:
			return ucache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_UCACHE_OBJECT_ROUTE_SLEEP, dst
			);
		case PHP_UCACHE_OBJECT_ROUTE_WAKEUP:
			return ucache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_UCACHE_OBJECT_ROUTE_WAKEUP, dst
			);
		case PHP_UCACHE_OBJECT_ROUTE_SERDES:
			return ucache_shared_graph_copy_serdes_object(ctx, src, obj, dst);
		case PHP_UCACHE_OBJECT_ROUTE_PLAIN:
			return ucache_shared_graph_copy_plain_object(ctx, src, obj, dst);
		case PHP_UCACHE_OBJECT_ROUTE_UNSTORABLE:
			return false;
	}

	return false;
}

static bool ucache_shared_graph_copy_value(
		php_ucache_shared_graph_copy_ctx_t *ctx,
		const zval *src,
		php_ucache_shared_graph_value_t *dst)
{
	uint32_t string_offset;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	memset(dst, 0, sizeof(*dst));

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF;

			return true;
		case IS_NULL:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_NULL;

			return true;
		case IS_TRUE:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_TRUE;

			return true;
		case IS_FALSE:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_FALSE;

			return true;
		case IS_LONG:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_LONG;
			dst->payload.long_value = Z_LVAL_P(src);

			return true;
		case IS_DOUBLE:
			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_DOUBLE;
			dst->payload.double_value = Z_DVAL_P(src);

			return true;
		case IS_STRING:
			if (!ucache_shared_graph_copy_string(ctx, Z_STR_P(src), &string_offset)) {
				return false;
			}

			dst->type = PHP_UCACHE_SHARED_GRAPH_VALUE_STRING;
			dst->payload.offset = string_offset;

			return true;
		case IS_ARRAY:
			return ucache_shared_graph_copy_array(ctx, src, dst);
		case IS_OBJECT:
			return ucache_shared_graph_copy_object(ctx, src, dst);
		case IS_REFERENCE:
			return ucache_shared_graph_copy_reference(ctx, src, dst);
		default:
			return false;
	}
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_try_update_declared_property(
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

		zval_ptr_dtor(slot);

		ZVAL_COPY(slot, prop_val);

		if (obj->properties != NULL) {
			ZVAL_INDIRECT(&indirect, slot);
			zend_hash_update(obj->properties, prop_name, &indirect);
		}

		return true;
	}

	if ((prop_info->flags & (ZEND_ACC_READONLY|ZEND_ACC_PPP_SET_MASK)) != 0 ||
		(prop_info->flags & ZEND_ACC_PPP_MASK) != ZEND_ACC_PUBLIC
	) {
		return false;
	}

	ZVAL_COPY_DEREF(&tmp, prop_val);

	if (ZEND_TYPE_IS_SET(prop_info->type) &&
		!zend_verify_property_type(prop_info, &tmp, true)
	) {
		zval_ptr_dtor(&tmp);

		*failed = true;

		return false;
	}

	zval_ptr_dtor(slot);

	ZVAL_COPY_VALUE(slot, &tmp);

	if (obj->properties != NULL) {
		ZVAL_INDIRECT(&indirect, slot);
		zend_hash_update(obj->properties, prop_name, &indirect);
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT zend_class_entry *ucache_decode_lookup_class(zend_string *class_name)
{
	zend_class_entry *ce;

	ce = ucache_decode_resolve_cache_find(class_name);
	if (ce != NULL) {
		return ce;
	}

	ce = zend_lookup_class(class_name);
	if (ce != NULL) {
		ucache_decode_resolve_cache_store(class_name, ce);
	}

	return ce;
}

static PHP_UCACHE_DECODE_HOT zend_class_entry *ucache_decode_lookup_state_schema_class(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_state_schema_t *state_schema)
{
	zend_string *class_name;
	zend_class_entry *ce;

	ce = ucache_decode_resolve_cache_find(state_schema);
	if (ce != NULL) {
		return ce;
	}

	class_name = ucache_decode_string_at(buf, buf_len, state_schema->class_name_offset);
	if (class_name == NULL) {
		return NULL;
	}

	ce = ucache_decode_lookup_class(class_name);
	if (ce != NULL) {
		ucache_decode_resolve_cache_store(state_schema, ce);
	}

	return ce;
}

static void ucache_decode_array_dtor(zval *zv)
{
	zval tmp;

	ZVAL_ARR(&tmp, (zend_array *) Z_PTR_P(zv));

	zval_ptr_dtor(&tmp);
}

static void ucache_decode_shape_prototype_dtor(zval *zv)
{
	zend_array_destroy((zend_array *) Z_PTR_P(zv));
}

static zend_array *ucache_decode_shape_prototype_create(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_array_shape_t *graph_shape)
{
	const php_ucache_shared_graph_array_shape_element_t *shape_elems, *shape_elem;
	zend_string *prop_name;
	zend_array *proto;
	zval empty;
	uint32_t i;

	if (graph_shape->count == 0 ||
		graph_shape->count > PHP_UCACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS ||
		!ucache_decode_array_range_ok(
			buf_len,
			graph_shape->elements_offset,
			graph_shape->count,
			sizeof(*shape_elems)
		)
	) {
		return NULL;
	}

	proto = zend_new_array(graph_shape->count);
	zend_hash_real_init_mixed(proto);

	shape_elems =
		(const php_ucache_shared_graph_array_shape_element_t *) (buf + graph_shape->elements_offset)
	;

	ZVAL_LONG(&empty, 0);

	for (i = 0; i < graph_shape->count; i++) {
		shape_elem = &shape_elems[i];
		prop_name = ucache_decode_string_at(buf, buf_len, shape_elem->key_offset);
		if (shape_elem->key_offset == 0 || prop_name == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}

		if (_zend_hash_append_ex(proto, prop_name, &empty, true) == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}
	}

	return proto;
}

static zend_array *ucache_decode_shape_prototype_get(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_array_shape_t *graph_shape)
{
	zend_ulong key;
	zend_array *proto;
	HashTable *cache;
	uint32_t i;

	for (i = 0; i < PHP_UCACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
		if (UC_G(decode_shape_prototype_direct_keys)[i] == graph_shape) {
			return UC_G(decode_shape_prototype_direct_values)[i];
		}
	}

	key = (zend_ulong) (uintptr_t) graph_shape;
	if (UC_G(decode_shape_prototype_cache) != NULL) {
		proto = zend_hash_index_find_ptr(UC_G(decode_shape_prototype_cache), key);
		if (proto != NULL) {
			ucache_decode_shape_prototype_direct_cache_store(graph_shape, proto);

			return proto;
		}
	}

	proto = ucache_decode_shape_prototype_create(buf, buf_len, graph_shape);
	if (proto == NULL) {
		return NULL;
	}

	cache = ucache_decode_shape_prototype_cache();
	if (zend_hash_index_add_ptr(cache, key, proto) == NULL) {
		zend_array_destroy(proto);

		return NULL;
	}

	ucache_decode_shape_prototype_direct_cache_store(graph_shape, proto);

	return proto;
}

static PHP_UCACHE_DECODE_HOT bool ucache_decode_shape_prototype_clone(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_array_shape_t *graph_shape,
		uint32_t count,
		uint32_t next_free,
		zval *dst)
{
	zend_array *proto, *arr;

	if (graph_shape->count != count) {
		return false;
	}

	proto = ucache_decode_shape_prototype_get(buf, buf_len, graph_shape);
	if (proto == NULL || proto->nNumUsed != count || proto->nNumOfElements != count) {
		return false;
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
	arr->nNextFreeElement = (zend_long) next_free;
	arr->pDestructor = ZVAL_PTR_DTOR;

	HT_SET_DATA_ADDR(arr, emalloc(HT_SIZE(arr)));

	memcpy(HT_GET_DATA_ADDR(arr), HT_GET_DATA_ADDR(proto), HT_USED_SIZE(proto));

	ZVAL_ARR(dst, arr);

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_shaped_array(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t shape_offset,
		uint32_t values_offset,
		uint32_t count,
		uint32_t next_free,
		uint32_t node_offset,
		bool shared,
		zval *dst)
{
	const php_ucache_shared_graph_array_shape_t *graph_shape;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t value;
	Bucket *bucket;
	uint32_t i;

	if (!ucache_decode_range_ok(buf_len, shape_offset, sizeof(*graph_shape)) ||
		!ucache_graph_columns_fit(buf_len, values_offset, ucache_graph_value_columns_size(count))
	) {
		return false;
	}

	graph_shape = (const php_ucache_shared_graph_array_shape_t *) (buf + shape_offset);
	if (!ucache_decode_shape_prototype_clone(
			buf,
			buf_len,
			graph_shape,
			count,
			next_free,
			dst
		)
	) {
		return false;
	}

	if (shared) {
		if (!ucache_decode_array_map_insert(node_offset, Z_ARRVAL_P(dst))) {
			return ucache_decode_fail_zval(dst);
		}
	}

	HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	ucache_graph_value_columns_locate((uint8_t *) buf + values_offset, count, &cols);
	bucket = Z_ARRVAL_P(dst)->arData;
	for (i = 0; i < count; i++) {
		ucache_graph_columns_load_value(&cols, i, &value);

		/* UNDEF is only a property skip marker; in an array position it
		 * would corrupt nNumOfElements accounting. */
		if (UNEXPECTED(value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF)) {
			return ucache_decode_fail_zval(dst);
		}

		if (!ucache_shared_graph_decode_value_inline(buf, buf_len, &value, &bucket[i].val)) {
			return ucache_decode_fail_zval(dst);
		}
	}

	PHP_UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_dynamic_array(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_array_t *graph_array;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t elem_value;
	zend_long next_free;
	zend_string *key;
	zval val;
	uint32_t i;

	if (!ucache_decode_range_ok(buf_len, (uint32_t) value->payload.offset, sizeof(*graph_array))) {
		return false;
	}

	graph_array = (const php_ucache_shared_graph_array_t *) (buf + (uint32_t) value->payload.offset);
	if (!ucache_graph_columns_fit(
			buf_len,
			graph_array->elements_offset,
			ucache_graph_array_columns_size(graph_array->count, graph_array->flags)
		)
	) {
		return false;
	}

	array_init_size(dst, graph_array->count);

	if (graph_array->count > 0) {
		if (graph_array->flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED) {
			zend_hash_real_init_packed(Z_ARRVAL_P(dst));
		} else {
			zend_hash_real_init_mixed(Z_ARRVAL_P(dst));
		}

		HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	}

	if (graph_array->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) {
		if (!ucache_decode_array_map_insert((uint32_t) value->payload.offset, Z_ARRVAL_P(dst))) {
			return ucache_decode_fail_zval(dst);
		}
	}

	ucache_graph_array_columns_locate(
		(uint8_t *) buf + graph_array->elements_offset,
		graph_array->count,
		graph_array->flags,
		&cols
	);

	if (graph_array->flags & PHP_UCACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED) {
		/* A packed block carries no key columns: the index is implicit. */
		if (cols.key_offsets != NULL || cols.hashes != NULL) {
			return ucache_decode_fail_zval(dst);
		}

		for (i = 0; i < graph_array->count; i++) {
			ucache_graph_columns_load_value(&cols, i, &elem_value);

			if (UNEXPECTED(elem_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF)) {
				return ucache_decode_fail_zval(dst);
			}

			if (!ucache_shared_graph_decode_value_inline(buf, buf_len, &elem_value, &val)) {
				return ucache_decode_fail_zval(dst);
			}

			if (zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &val) == NULL) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}
		}

		if (!ucache_shared_graph_decode_array_next_free(buf, buf_len, graph_array, &next_free)) {
			return ucache_decode_fail_zval(dst);
		}

		Z_ARRVAL_P(dst)->nNextFreeElement = next_free;

		PHP_UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

		return true;
	}

	for (i = 0; i < graph_array->count; i++) {
		ucache_graph_columns_load_value(&cols, i, &elem_value);

		if (UNEXPECTED(elem_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF)) {
			return ucache_decode_fail_zval(dst);
		}

		if (!ucache_shared_graph_decode_value_inline(buf, buf_len, &elem_value, &val)) {
			return ucache_decode_fail_zval(dst);
		}

		if (cols.key_offsets != NULL && cols.key_offsets[i] != 0) {
			key = ucache_decode_string_at(buf, buf_len, cols.key_offsets[i]);
			if (key == NULL) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}

			/* Reject a corrupt payload with duplicate keys rather than trusting
			 * the encoder's uniqueness; add_new would assert or double-insert. */
			if (zend_hash_add(Z_ARRVAL_P(dst), key, &val) == NULL) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}
		} else {
			if (cols.hashes == NULL ||
				zend_hash_index_add(Z_ARRVAL_P(dst), cols.hashes[i], &val) == NULL
			) {
				zval_ptr_dtor(&val);

				return ucache_decode_fail_zval(dst);
			}
		}
	}

	if (!ucache_shared_graph_decode_array_next_free(buf, buf_len, graph_array, &next_free)) {
		return ucache_decode_fail_zval(dst);
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = next_free;
	PHP_UCACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_init_object(
		zend_class_entry *ce,
		uint32_t node_offset,
		bool shared,
		zval *dst)
{
	if (object_init_ex(dst, ce) != SUCCESS) {
		return false;
	}

	if (shared && !ucache_decode_identity_map_insert(node_offset, Z_OBJ_P(dst))) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_apply_property(
		zval *dst,
		zend_string *prop_name,
		uint32_t slot_idx_plus_one,
		zval *prop_val)
{
	if (slot_idx_plus_one != 0) {
		return php_ucache_shared_graph_update_object_property_at(
			dst,
			prop_name,
			slot_idx_plus_one - 1,
			prop_val
		);
	}

	return php_ucache_shared_graph_update_object_property(dst, prop_name, prop_val);
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_object_properties(
		const uint8_t *buf,
		size_t buf_len,
		uint32_t properties_offset,
		uint32_t property_count,
		bool use_sleep_slots,
		zval *dst)
{
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t value;
	zend_string *prop_name;
	zval prop_val;
	uint32_t i;
	bool applied;

	if (property_count == 0) {
		return true;
	}

	if (!ucache_graph_columns_fit(
			buf_len,
			properties_offset,
			ucache_graph_property_columns_size(property_count, use_sleep_slots)
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	ucache_graph_property_columns_locate(
		(uint8_t *) buf + properties_offset,
		property_count,
		use_sleep_slots,
		&cols
	);
	for (i = 0; i < property_count; i++) {
		ucache_graph_columns_load_value(&cols, i, &value);

		if (value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF) {
			continue;
		}

		prop_name = ucache_decode_string_at(buf, buf_len, cols.name_offsets[i]);
		if (prop_name == NULL) {
			return ucache_decode_fail_zval(dst);
		}

		if (!ucache_shared_graph_decode_value_inline(buf, buf_len, &value, &prop_val)) {
			return ucache_decode_fail_zval(dst);
		}

		applied = ucache_shared_graph_decode_apply_property(
			dst,
			prop_name,
			use_sleep_slots ? cols.sleep_indices[i] : i + 1,
			&prop_val
		);

		zval_ptr_dtor(&prop_val);

		if (!applied) {
			return ucache_decode_fail_zval(dst);
		}
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_call_wakeup(zend_class_entry *ce, zval *dst)
{
	zval retval, *wakeup_zv;

	wakeup_zv = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));
	if (wakeup_zv != NULL) {
		zend_call_known_instance_method(Z_FUNC_P(wakeup_zv), Z_OBJ_P(dst), &retval, 0, NULL);

		zval_ptr_dtor(&retval);

		if (EG(exception)) {
			return ucache_decode_fail_zval(dst);
		}
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		bool sleep,
		zval *dst)
{
	const php_ucache_shared_graph_object_t *graph_obj;
	zend_string *class_name;
	zend_class_entry *ce;

	graph_obj = (const php_ucache_shared_graph_object_t *) (buf + (uint32_t) value->payload.offset);
	class_name = ucache_decode_string_at(buf, buf_len, graph_obj->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = ucache_decode_lookup_class(class_name);
	if (ce == NULL) {
		return false;
	}

	if (!ucache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_obj->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!ucache_shared_graph_decode_object_properties(
			buf,
			buf_len,
			graph_obj->properties_offset,
			graph_obj->property_count,
			sleep,
			dst
		)
	) {
		return false;
	}

	return !sleep || ucache_shared_graph_decode_call_wakeup(ce, dst);
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_sleep_shaped_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_shaped_state_object_t *graph_shaped_state;
	const php_ucache_shared_graph_state_schema_t *state_schema;
	const php_ucache_shared_graph_array_shape_t *graph_shape;
	const php_ucache_shared_graph_array_shape_element_t *shape_elems;
	php_ucache_graph_columns_t cols;
	php_ucache_shared_graph_value_t state_value;
	zend_string *prop_name;
	zend_class_entry *ce;
	zval prop_val;
	uint32_t i, prop_idx_plus_one;
	bool applied;

	graph_shaped_state = (const php_ucache_shared_graph_shaped_state_object_t *) (buf + (uint32_t) value->payload.offset);
	if (!ucache_decode_range_ok(buf_len, graph_shaped_state->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const php_ucache_shared_graph_state_schema_t *) (buf + graph_shaped_state->state_schema_offset);
	ce = ucache_decode_lookup_state_schema_class(buf, buf_len, state_schema);

	if (ce == NULL) {
		return false;
	}

	if (!ucache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_shaped_state->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!ucache_decode_range_ok(buf_len, state_schema->shape_offset, sizeof(*graph_shape))) {
		return ucache_decode_fail_zval(dst);
	}

	graph_shape = (const php_ucache_shared_graph_array_shape_t *) (buf + state_schema->shape_offset);
	if (graph_shape->count != state_schema->count) {
		return ucache_decode_fail_zval(dst);
	}

	if (!ucache_decode_array_range_ok(buf_len, graph_shape->elements_offset, state_schema->count, sizeof(*shape_elems)) ||
		!ucache_graph_columns_fit(
			buf_len,
			graph_shaped_state->state_values_offset,
			ucache_graph_value_columns_size(state_schema->count)
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	shape_elems = (const php_ucache_shared_graph_array_shape_element_t *) (buf + graph_shape->elements_offset);
	ucache_graph_value_columns_locate(
		(uint8_t *) buf + graph_shaped_state->state_values_offset,
		state_schema->count,
		&cols
	);
	for (i = 0; i < state_schema->count; i++) {
		prop_name = ucache_decode_string_at(buf, buf_len, shape_elems[i].key_offset);
		if (prop_name == NULL) {
			return ucache_decode_fail_zval(dst);
		}

		ucache_graph_columns_load_value(&cols, i, &state_value);

		if (!ucache_shared_graph_decode_value_inline(buf, buf_len, &state_value, &prop_val)) {
			return ucache_decode_fail_zval(dst);
		}

		prop_idx_plus_one = php_ucache_serdes_declared_property_index_plus_one(ce, prop_name);
		applied = ucache_shared_graph_decode_apply_property(dst, prop_name, prop_idx_plus_one, &prop_val);

		zval_ptr_dtor(&prop_val);

		if (!applied) {
			return ucache_decode_fail_zval(dst);
		}
	}

	return ucache_shared_graph_decode_call_wakeup(ce, dst);
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_safe_direct_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_safe_direct_object_t *graph_safe_direct;
	php_ucache_safe_direct_state_unserialize_func_t unserialize_func;
	zend_string *class_name;
	zend_class_entry *ce;
	zval sd_state;

	graph_safe_direct = (const php_ucache_shared_graph_safe_direct_object_t *) (buf + (uint32_t) value->payload.offset);

	class_name = ucache_decode_string_at(buf, buf_len, graph_safe_direct->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = ucache_decode_lookup_class(class_name);
	if (ce == NULL) {
		return false;
	}

	unserialize_func = php_ucache_safe_direct_state_unserialize_func(ce);

	if (unserialize_func == NULL ||
		!ucache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_safe_direct->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&sd_state);
	if (!ucache_shared_graph_decode_value(buf, buf_len, &graph_safe_direct->state, &sd_state) ||
		Z_TYPE(sd_state) != IS_ARRAY ||
		!unserialize_func(dst, &sd_state)
	) {
		zval_ptr_dtor(&sd_state);

		return ucache_decode_fail_zval(dst);
	}

	zval_ptr_dtor(&sd_state);

	return ucache_shared_graph_decode_object_properties(
		buf,
		buf_len,
		graph_safe_direct->properties_offset,
		graph_safe_direct->property_count,
		false,
		dst
	);
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_serialized_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_safe_direct_object_t *graph_serialized;
	zend_string *class_name;
	zend_class_entry *ce;
	zval state;

	graph_serialized = (const php_ucache_shared_graph_safe_direct_object_t *) (buf + (uint32_t) value->payload.offset);

	class_name = ucache_decode_string_at(buf, buf_len, graph_serialized->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = ucache_decode_lookup_class(class_name);
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!ucache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_serialized->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!ucache_shared_graph_decode_value(buf, buf_len, &graph_serialized->state, &state) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	zend_call_known_instance_method_with_1_params(ce->__unserialize, Z_OBJ_P(dst), NULL, &state);

	zval_ptr_dtor(&state);

	if (EG(exception)) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_serialized_shaped_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_shaped_state_object_t *graph_shaped_state;
	const php_ucache_shared_graph_state_schema_t *state_schema;
	zend_class_entry *ce;
	zval state;

	graph_shaped_state = (const php_ucache_shared_graph_shaped_state_object_t *) (buf + (uint32_t) value->payload.offset);
	if (!ucache_decode_range_ok(buf_len, graph_shaped_state->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const php_ucache_shared_graph_state_schema_t *) (buf + graph_shaped_state->state_schema_offset);
	ce = ucache_decode_lookup_state_schema_class(buf, buf_len, state_schema);
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!ucache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_shaped_state->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!ucache_shared_graph_decode_shaped_array(
			buf,
			buf_len,
			state_schema->shape_offset,
			graph_shaped_state->state_values_offset,
			state_schema->count,
			graph_shaped_state->state_next_free,
			0,
			false,
			&state
		) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);

		return ucache_decode_fail_zval(dst);
	}

	zend_call_known_instance_method_with_1_params(ce->__unserialize, Z_OBJ_P(dst), NULL, &state);

	zval_ptr_dtor(&state);

	if (EG(exception)) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_serdes_object(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_serdes_object_t *graph_serdes;

	graph_serdes =
		(const php_ucache_shared_graph_serdes_object_t *) (buf + (uint32_t) value->payload.offset)
	;

	if (!ucache_decode_range_ok(
			buf_len,
			(uint32_t) value->payload.offset + (uint32_t) sizeof(*graph_serdes),
			graph_serdes->blob_len
		)
	) {
		return false;
	}

	if (!php_ucache_serdes_decode(
			(const uint8_t *) (graph_serdes + 1),
			graph_serdes->blob_len,
			dst
		) ||
		Z_TYPE_P(dst) != IS_OBJECT
	) {
		return ucache_decode_fail_zval(dst);
	}

	if ((graph_serdes->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
		!ucache_decode_identity_map_insert(
			(uint32_t) value->payload.offset,
			Z_OBJ_P(dst)
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_enum(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_enum_t *graph_enum;
	zend_string *class_name, *case_name;
	zend_class_entry *ce;
	zend_object *case_obj;

	graph_enum = (const php_ucache_shared_graph_enum_t *) (buf + (uint32_t) value->payload.offset);
	case_obj = ucache_decode_resolve_cache_find(graph_enum);
	if (case_obj == NULL) {
		class_name = ucache_decode_string_at(buf, buf_len, graph_enum->class_name_offset);
		case_name = ucache_decode_string_at(buf, buf_len, graph_enum->case_name_offset);
		if (class_name == NULL || case_name == NULL) {
			return false;
		}

		ce = ucache_decode_lookup_class(class_name);

		if (ce == NULL || !(ce->ce_flags & ZEND_ACC_ENUM)) {
			return false;
		}

		case_obj = php_ucache_enum_case_find(ce, case_name);
		if (case_obj == NULL) {
			return false;
		}

		ucache_decode_resolve_cache_store(graph_enum, case_obj);
	}

	ZVAL_OBJ(dst, case_obj);
	GC_ADDREF(case_obj);

	return true;
}

static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_reference(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_reference_t *graph_ref;
	zend_reference *ref;

	graph_ref = (const php_ucache_shared_graph_reference_t *) (buf + (uint32_t) value->payload.offset);

	ZVAL_NEW_EMPTY_REF(dst);
	ref = Z_REF_P(dst);

	ZVAL_UNDEF(&ref->val);

	if ((graph_ref->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
		!ucache_decode_reference_map_insert(
			(uint32_t) value->payload.offset,
			ref
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	if (!ucache_shared_graph_decode_value(
			buf,
			buf_len,
			&graph_ref->inner,
			&ref->val
		)
	) {
		return ucache_decode_fail_zval(dst);
	}

	return true;
}

/* On failure *dst owns nothing: UNDEF, or NULL when object_init_ex() failed. */
static PHP_UCACHE_DECODE_HOT bool ucache_shared_graph_decode_value(
		const uint8_t *buf,
		size_t buf_len,
		const php_ucache_shared_graph_value_t *value,
		zval *dst)
{
	const php_ucache_shared_graph_shaped_array_t *graph_shaped_array;
	zend_reference *shared_reference;
	zend_array *shared_array;
	zend_object *shared_obj;
	size_t node_header_size;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	node_header_size = ucache_decode_node_header_size(value->type);
	if (node_header_size != 0 &&
		!ucache_decode_range_ok(buf_len, (uint32_t) value->payload.offset, node_header_size)
	) {
		return false;
	}

	switch (value->type) {
		case PHP_UCACHE_SHARED_GRAPH_VALUE_UNDEF:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_NULL:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_TRUE:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_FALSE:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_LONG:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DOUBLE:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_STRING:
			return ucache_shared_graph_decode_simple_value(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				ZVAL_EMPTY_ARRAY(dst);
			} else {
				if (!ucache_decode_range_ok(
						buf_len,
						(uint32_t) value->payload.offset,
						sizeof(zend_array)
					)
				) {
					return false;
				}

				ZVAL_ARR(dst, (zend_array *) (void *) (buf + (uint32_t) value->payload.offset));
				Z_TYPE_FLAGS_P(dst) = 0;
			}

			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			return ucache_shared_graph_decode_dynamic_array(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			graph_shaped_array = (const php_ucache_shared_graph_shaped_array_t *) (buf + (uint32_t) value->payload.offset);

			return ucache_shared_graph_decode_shaped_array(
				buf,
				buf_len,
				graph_shaped_array->shape_offset,
				graph_shaped_array->values_offset,
				graph_shaped_array->count,
				graph_shaped_array->next_free,
				(uint32_t) value->payload.offset,
				(graph_shaped_array->flags & PHP_UCACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
				dst
			);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY_REF: {
			shared_array = ucache_decode_array_map_find((uint32_t) value->payload.offset);
			if (shared_array == NULL) {
				return false;
			}

			ZVAL_ARR(dst, shared_array);

			GC_ADDREF(shared_array);

			return true;
		}
		case PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT:
			return ucache_shared_graph_decode_object(buf, buf_len, value, false, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			return ucache_shared_graph_decode_object(buf, buf_len, value, true, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
			return ucache_shared_graph_decode_sleep_shaped_object(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT_REF: {
			shared_obj = ucache_decode_identity_map_find((uint32_t) value->payload.offset);
			if (shared_obj == NULL) {
				return false;
			}

			ZVAL_OBJ(dst, shared_obj);
			GC_ADDREF(shared_obj);

			return true;
		}
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
			return ucache_shared_graph_decode_safe_direct_object(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			return ucache_shared_graph_decode_serialized_object(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			return ucache_shared_graph_decode_serialized_shaped_object(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return ucache_shared_graph_decode_serdes_object(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM:
			return ucache_shared_graph_decode_enum(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return ucache_shared_graph_decode_reference(buf, buf_len, value, dst);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE_REF: {
			shared_reference = ucache_decode_reference_map_find((uint32_t) value->payload.offset);

			if (shared_reference == NULL) {
				return false;
			}

			ZVAL_REF(dst, shared_reference);

			GC_ADDREF(shared_reference);

			return true;
		}
		default:
			return false;
	}
}

static PHP_UCACHE_DECODE_HOT php_ucache_shared_graph_header_t *ucache_shared_graph_payload_header(uint32_t payload_offset)
{
	const uint8_t *graph_buf;
	size_t buf_len;

	if (payload_offset == 0) {
		return NULL;
	}

	buf_len = php_ucache_block_payload_capacity(payload_offset);
	if (buf_len == 0) {
		return NULL;
	}

	graph_buf = ucache_shared_graph_locate(
		php_ucache_ptr(payload_offset),
		buf_len,
		NULL
	);
	if (graph_buf == NULL) {
		return NULL;
	}

	return (php_ucache_shared_graph_header_t *) graph_buf;
}

static void ucache_shared_graph_pin_owners_init(php_ucache_shared_graph_header_t *header)
{
	uint32_t w;

	for (w = 0; w < PHP_UCACHE_GRAPH_PIN_WORDS; w++) {
		ZEND_ATOMIC_INT_INIT(&header->pin_owners[w], 0);
	}
}

static uint32_t ucache_graph_pin_popcount(uint32_t v)
{
#if defined(__GNUC__) || __has_builtin(__builtin_popcount)
	return (uint32_t) __builtin_popcount(v);
#else
	v = v - ((v >> 1) & 0x55555555U);
	v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);

	return (((v + (v >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
#endif
}

static void ucache_graph_pin_count_add(php_ucache_graph_pin_slot_t *slot, int delta)
{
	int expected = zend_atomic_int_load_ex(&slot->pin_count), desired;

	for (;;) {
		desired = expected + delta;
		if (desired < 0) {
			desired = 0;
		}

		if (zend_atomic_int_compare_exchange_ex(&slot->pin_count, &expected, desired)) {
			return;
		}
	}
}

static void ucache_graph_pin_bit_set(php_ucache_shared_graph_header_t *header, uint32_t slot_idx)
{
	zend_atomic_int *word = &header->pin_owners[slot_idx / 32U];
	int mask = (int) (1U << (slot_idx % 32U)),
		expected = zend_atomic_int_load_ex(word)
	;

	while ((expected & mask) == 0) {
		if (zend_atomic_int_compare_exchange_ex(word, &expected, expected | mask)) {
			return;
		}
	}
}

static bool ucache_graph_pin_bit_clear(php_ucache_shared_graph_header_t *header, uint32_t slot_idx)
{
	zend_atomic_int *word = &header->pin_owners[slot_idx / 32U];
	int mask = (int) (1U << (slot_idx % 32U)),
		expected = zend_atomic_int_load_ex(word)
	;

	while ((expected & mask) != 0) {
		if (zend_atomic_int_compare_exchange_ex(word, &expected, expected & ~mask)) {
			return true;
		}
	}

	return false;
}

/* Find (optionally claiming) this process's pin slot for the given segment.
 * Runs lock-free: claims race with other readers, so the pid is taken by
 * CAS. Claims are cached per header and validated against the slot's pid, so
 * a fork child never reuses its parent's slot. Returns -1 when the table is
 * saturated; the caller then pins without a record, which restores the
 * pre-record (unreclaimable on crash) behavior for that reference only. */
static int32_t ucache_graph_pin_slot_find(php_ucache_header_t *header, bool claim)
{
	php_ucache_graph_pin_claim_t *claims = UC_G(graph_pin_claims);
	php_ucache_graph_pin_slot_t *slot;
	uint64_t my_pid = php_ucache_cached_pid();
	uint32_t i = 0;
	int32_t found = -1;
	int my_pid32 = (int) (uint32_t) my_pid, expected;

	while (i < UC_G(graph_pin_claim_count)) {
		slot = &claims[i].header->graph_pin_slots[claims[i].slot_index];

		if (zend_atomic_int_load_ex(&slot->owner_pid) != my_pid32) {
			/* Stale after a fork (the slot still belongs to the parent) or
			 * after the slot was reclaimed; drop the cache entry. */
			claims[i] = claims[--UC_G(graph_pin_claim_count)];

			continue;
		}

		if (claims[i].header == header) {
			return (int32_t) claims[i].slot_index;
		}

		i++;
	}

	if (!claim || UC_G(graph_pin_claim_count) == PHP_UCACHE_GRAPH_PIN_CLAIM_MAX) {
		return -1;
	}

	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		slot = &header->graph_pin_slots[i];
		expected = zend_atomic_int_load_ex(&slot->owner_pid);

		if (expected == PHP_UCACHE_GRAPH_PIN_OWNER_RECLAIMING) {
			continue;
		}

		if (expected != 0) {
			/* Only steal slots whose owner exited cleanly (no pins left);
			 * slots with pins outstanding belong to the dead-owner sweep.
			 * A slot carrying our own pid can be a sibling thread's claim,
			 * so it is never taken over either. */
			if (expected == my_pid32 ||
				zend_atomic_int_load_ex(&slot->pin_count) != 0 ||
				!php_ucache_graph_pin_owner_is_dead(
					(uint64_t) (uint32_t) expected,
					php_ucache_atomic_load_64(&slot->owner_start_time)
				)
			) {
				continue;
			}

			if (!zend_atomic_int_compare_exchange_ex(
					&slot->owner_pid,
					&expected,
					PHP_UCACHE_GRAPH_PIN_OWNER_RECLAIMING
				)
			) {
				continue;
			}

			/* Clear start time before pid so scanners cannot combine owners. */
			php_ucache_atomic_store_64(&slot->owner_start_time, 0);
			zend_atomic_int_store_ex(&slot->owner_pid, 0);
		}

		expected = 0;
		if (zend_atomic_int_compare_exchange_ex(&slot->owner_pid, &expected, my_pid32)) {
			php_ucache_atomic_store_64(&slot->owner_start_time, php_ucache_self_start_time_token());
			found = (int32_t) i;

			break;
		}
	}

	if (found >= 0) {
		claims[UC_G(graph_pin_claim_count)].header = header;
		claims[UC_G(graph_pin_claim_count)].slot_index = (uint32_t) found;

		UC_G(graph_pin_claim_count)++;
	}

	return found;
}

/* Clear the dead owners' bits and drop ref_state accordingly. Runs with the
 * write lock held and payload readers quiesced. */
static uint32_t ucache_graph_pin_strip_payload_locked(
		php_ucache_shared_graph_header_t *graph_header,
		const uint32_t *dead_pin_mask)
{
	uint32_t w, drop, cleared = 0;
	int expected, desired, refcount;

	for (w = 0; w < PHP_UCACHE_GRAPH_PIN_WORDS; w++) {
		if (dead_pin_mask[w] == 0) {
			continue;
		}

		expected = zend_atomic_int_load_ex(&graph_header->pin_owners[w]);

		for (;;) {
			desired = expected & ~(int) dead_pin_mask[w];
			if (desired == expected) {
				break;
			}

			if (zend_atomic_int_compare_exchange_ex(&graph_header->pin_owners[w], &expected, desired)) {
				cleared += ucache_graph_pin_popcount((uint32_t) expected & dead_pin_mask[w]);

				break;
			}
		}
	}

	if (cleared == 0) {
		return 0;
	}

	expected = zend_atomic_int_load_ex(&graph_header->ref_state);

	for (;;) {
		refcount = expected & PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK;
		/* Fewer references than stripped bits means the state is already
		 * corrupt; clamp rather than underflow into the RETIRED bit. */
		drop = (uint32_t) refcount < cleared ? (uint32_t) refcount : cleared;
		desired = (expected & PHP_UCACHE_SHARED_GRAPH_RETIRED) | (refcount - (int) drop);

		if (zend_atomic_int_compare_exchange_ex(&graph_header->ref_state, &expected, desired)) {
			break;
		}
	}

	php_ucache_header_ptr()->graph_dead_pins_stripped += cleared;

	return cleared;
}

/* Debug-only cross-check for recorded relocation fixups. */
#if ZEND_DEBUG
static bool ucache_shared_graph_rebase_verbatim_zval(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		zval *value)
{
	zend_array *arr;

	switch (Z_TYPE_P(value)) {
		case IS_STRING:
			Z_STR_P(value) = (zend_string *) ucache_shared_graph_rebase_pointer(
				Z_STR_P(value),
				ctx->old_base,
				ctx->len,
				ctx->delta
			);

			return true;
		case IS_ARRAY:
			arr = (zend_array *) ucache_shared_graph_rebase_pointer(
				Z_ARR_P(value),
				ctx->old_base,
				ctx->len,
				ctx->delta
			);

			Z_ARR_P(value) = arr;

			return ucache_shared_graph_rebase_verbatim_array(ctx, arr);
		default:
			return true;
	}
}

static bool ucache_shared_graph_rebase_verbatim_array(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		zend_array *arr)
{
	zval *packed;
	Bucket *bucket;
	uint32_t i;
	void *data;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	if (!ucache_shared_graph_pointer_in_range(
			arr,
			ctx->new_base,
			ctx->len
		)
	) {
		return true;
	}

	if (!php_ucache_seen_test_and_add(ctx->seen, arr)) {
		return true;
	}

	data = HT_GET_DATA_ADDR(arr);
	data = ucache_shared_graph_rebase_pointer(data, ctx->old_base, ctx->len, ctx->delta);

	HT_SET_DATA_ADDR(arr, data);

	if (!ucache_shared_graph_pointer_in_range(data, ctx->new_base, ctx->len)) {
		return false;
	}

	if (HT_IS_PACKED(arr)) {
		packed = arr->arPacked;
		for (i = 0; i < arr->nNumUsed; i++) {
			if (!ucache_shared_graph_rebase_verbatim_zval(ctx, &packed[i])) {
				return false;
			}
		}
	} else {
		bucket = arr->arData;
		for (i = 0; i < arr->nNumUsed; i++) {
			if (bucket[i].key != NULL) {
				bucket[i].key = (zend_string *) ucache_shared_graph_rebase_pointer(
					bucket[i].key,
					ctx->old_base,
					ctx->len,
					ctx->delta
				);

				if (!ucache_shared_graph_pointer_in_range(bucket[i].key, ctx->new_base, ctx->len) &&
					!ucache_shared_graph_pointer_in_segment_data(bucket[i].key)
				) {
					return false;
				}
			}

			if (!ucache_shared_graph_rebase_verbatim_zval(ctx, &bucket[i].val)) {
				return false;
			}
		}
	}

	return true;
}

static bool ucache_shared_graph_rebase_value_columns(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		const php_ucache_graph_columns_t *cols,
		uint32_t count)
{
	php_ucache_shared_graph_value_t value;
	uint32_t i;

	for (i = 0; i < count; i++) {
		ucache_graph_columns_load_value(cols, i, &value);

		if (!ucache_shared_graph_rebase_graph_value(ctx, &value)) {
			return false;
		}
	}

	return true;
}

static bool ucache_shared_graph_rebase_graph_value(
		php_ucache_shared_graph_rebase_ctx_t *ctx,
		const php_ucache_shared_graph_value_t *value)
{
	const php_ucache_shared_graph_array_t *graph_array;
	const php_ucache_shared_graph_shaped_array_t *graph_shaped_array;
	const php_ucache_shared_graph_object_t *graph_obj;
	const php_ucache_shared_graph_safe_direct_object_t *graph_safe_direct;
	const php_ucache_shared_graph_shaped_state_object_t *graph_shaped_state;
	const php_ucache_shared_graph_state_schema_t *state_schema;
	php_ucache_graph_columns_t cols;
	zend_array *arr;

	if (php_ucache_stack_overflowed()) {
		return false;
	}

	switch (value->type) {
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				return true;
			}

			arr = (zend_array *) (void *) (ctx->new_base + (uint32_t) value->payload.offset);

			return ucache_shared_graph_rebase_verbatim_array(ctx, arr);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const php_ucache_shared_graph_array_t *) (ctx->new_base + (uint32_t) value->payload.offset);
			ucache_graph_array_columns_locate(
				(uint8_t *) ctx->new_base + graph_array->elements_offset,
				graph_array->count,
				graph_array->flags,
				&cols
			);

			return ucache_shared_graph_rebase_value_columns(ctx, &cols, graph_array->count);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			graph_shaped_array = (const php_ucache_shared_graph_shaped_array_t *) (ctx->new_base + (uint32_t) value->payload.offset);
			ucache_graph_value_columns_locate(
				(uint8_t *) ctx->new_base + graph_shaped_array->values_offset,
				graph_shaped_array->count,
				&cols
			);

			return ucache_shared_graph_rebase_value_columns(ctx, &cols, graph_shaped_array->count);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			graph_obj = (const php_ucache_shared_graph_object_t *) (ctx->new_base + (uint32_t) value->payload.offset);
			if (graph_obj->property_count == 0) {
				return true;
			}

			ucache_graph_property_columns_locate(
				(uint8_t *) ctx->new_base + graph_obj->properties_offset,
				graph_obj->property_count,
				value->type == PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT,
				&cols
			);

			return ucache_shared_graph_rebase_value_columns(ctx, &cols, graph_obj->property_count);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return ucache_shared_graph_rebase_graph_value(
				ctx,
				&((const php_ucache_shared_graph_reference_t *) (ctx->new_base + (uint32_t) value->payload.offset))->inner
			);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			graph_shaped_state =
				(const php_ucache_shared_graph_shaped_state_object_t *) (ctx->new_base + (uint32_t) value->payload.offset)
			;
			state_schema =
				(const php_ucache_shared_graph_state_schema_t *) (ctx->new_base + graph_shaped_state->state_schema_offset)
			;
			ucache_graph_value_columns_locate(
				(uint8_t *) ctx->new_base + graph_shaped_state->state_values_offset,
				state_schema->count,
				&cols
			);

			return ucache_shared_graph_rebase_value_columns(ctx, &cols, state_schema->count);
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT: {
			graph_safe_direct =
				(const php_ucache_shared_graph_safe_direct_object_t *) (ctx->new_base + (uint32_t) value->payload.offset)
			;

			if (!ucache_shared_graph_rebase_graph_value(ctx, &graph_safe_direct->state)) {
				return false;
			}

			if (graph_safe_direct->property_count == 0) {
				return true;
			}

			ucache_graph_property_columns_locate(
				(uint8_t *) ctx->new_base + graph_safe_direct->properties_offset,
				graph_safe_direct->property_count,
				false,
				&cols
			);

			return ucache_shared_graph_rebase_value_columns(ctx, &cols, graph_safe_direct->property_count);
		}
		default:
			return true;
	}
}
#endif /* ZEND_DEBUG */

static void ucache_shared_graph_force_retire_locked(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header = ucache_shared_graph_payload_header(payload_offset);
	int state, expected;

	if (header == NULL) {
		return;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);

		if ((state & PHP_UCACHE_SHARED_GRAPH_RETIRED) != 0) {
			return;
		}

		expected = state;
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state | PHP_UCACHE_SHARED_GRAPH_RETIRED)) {
			return;
		}
	}
}

static int ucache_shared_graph_orphan_offset_compare(const void *lhs_ptr, const void *rhs_ptr)
{
	uint32_t lhs = *(const uint32_t *) lhs_ptr, rhs = *(const uint32_t *) rhs_ptr;

	return ZEND_THREEWAY_COMPARE(lhs, rhs);
}

static uint32_t ucache_shared_graph_snapshot_live_offsets_locked(
		php_ucache_header_t *header,
		uint32_t **offsets_out)
{
	const php_ucache_entry_lock_record_t *lock_record;
	php_ucache_entry_t *entries, *entry;
	uint32_t *offsets, i, count;

	entries = php_ucache_entries_ptr(header);
	offsets = emalloc(
		((size_t) header->capacity * 2 + header->entry_lock_capacity) * sizeof(*offsets)
	);
	count = 0;

	for (i = php_ucache_occupancy_next_used(header, 0);
		i != UINT32_MAX;
		i = php_ucache_occupancy_next_used(header, i + 1)
	) {
		entry = &entries[i];

		if (entry->key_offset != 0) {
			offsets[count++] = entry->key_offset;
		}

		if (entry->value_offset != 0) {
			offsets[count++] = entry->value_offset;
		}
	}

	/* Entry-lock key blocks hold user-controlled bytes outside the entry
	 * table; without them in the snapshot a crafted key that parses as a
	 * retired graph header could be freed out from under its record. */
	for (i = 0; i < header->entry_lock_capacity; i++) {
		lock_record = &php_ucache_entry_lock_records_ptr(header)[i];
		if (lock_record->state == PHP_UCACHE_ENTRY_LOCK_USED &&
			lock_record->key_offset != 0
		) {
			offsets[count++] = lock_record->key_offset;
		}
	}

	qsort(offsets, count, sizeof(*offsets), ucache_shared_graph_orphan_offset_compare);

	*offsets_out = offsets;

	return count;
}

static bool ucache_orphaned_graph_block_is_referenced_locked(
		const uint32_t *live_offsets,
		uint32_t live_offset_count,
		uint32_t block_offset,
		uint32_t block_size)
{
	uint32_t payload_start, lo, hi, mid;

	payload_start = block_offset + PHP_UCACHE_BLOCK_HEADER_UNITS;
	lo = 0;
	hi = live_offset_count;

	while (lo < hi) {
		mid = lo + (hi - lo) / 2;

		if (live_offsets[mid] < payload_start) {
			lo = mid + 1;
		} else {
			hi = mid;
		}
	}

	return lo < live_offset_count && live_offsets[lo] < block_offset + block_size;
}

/* Marks the interned strings a payload references. Offsets are only ever
 * trusted after they resolve to a live slot, so a block that merely looks
 * like a graph header can at worst over-mark. */
static void ucache_shared_graph_intern_mark_payload_locked(
		php_ucache_header_t *header,
		const php_ucache_shared_graph_header_t *graph_header,
		size_t graph_len,
		zend_ulong *marks)
{
	const php_ucache_intern_slot_t *slots = php_ucache_intern_slots_ptr(header);
	const zend_string *str;
	const uint32_t *list;
	uint32_t i, str_offset, mask, slot_idx, probe;

	if (graph_header->intern_count == 0 ||
		!ucache_decode_array_range_ok(
			graph_len,
			graph_header->intern_list_offset,
			graph_header->intern_count,
			sizeof(uint32_t)
		)
	) {
		return;
	}

	list = (const uint32_t *) (const void *) ((const uint8_t *) graph_header + graph_header->intern_list_offset);
	mask = header->intern_capacity - 1;

	for (i = 0; i < graph_header->intern_count; i++) {
		str_offset = list[i];

		if (str_offset == 0 ||
			!php_ucache_payload_in_bounds(header, str_offset, _ZSTR_HEADER_SIZE)
		) {
			continue;
		}

		str = (const zend_string *) php_ucache_ptr(str_offset);

		for (slot_idx = (uint32_t) (ZSTR_H(str) & mask), probe = 0;
			probe < header->intern_capacity && slots[slot_idx].str_offset != 0;
			probe++, slot_idx = (slot_idx + 1) & mask
		) {
			if (slots[slot_idx].str_offset == str_offset) {
				marks[slot_idx / PHP_UCACHE_OCCUPANCY_WORD_BITS] |=
					(zend_ulong) 1 << (slot_idx % PHP_UCACHE_OCCUPANCY_WORD_BITS)
				;

				break;
			}
		}
	}
}

static bool ucache_shared_graph_reclaim_orphaned_by_scan_locked(
		php_ucache_header_t *header,
		const uint32_t *dead_pin_mask)
{
	php_ucache_block_t *block;
	php_ucache_shared_graph_header_t *graph_header;
	uint32_t *live_offsets, used_end, offset, block_size, payload_offset, live_offset_count;
	bool reclaimed = false, restart;

	live_offset_count = ucache_shared_graph_snapshot_live_offsets_locked(header, &live_offsets);

	do {
		restart = false;
		used_end = header->data_offset + header->next_free;
		offset = header->data_offset;

		while (offset < used_end) {
			block = php_ucache_block_ptr(offset);
			block_size = block->size;

			if (block_size < php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(sizeof(php_ucache_block_t) + 1)) ||
				block_size > used_end - offset
			) {
				goto finish;
			}

			if (!php_ucache_block_is_free(block) &&
				!ucache_orphaned_graph_block_is_referenced_locked(
					live_offsets, live_offset_count, offset, block_size
				)
			) {
				payload_offset = offset + PHP_UCACHE_BLOCK_HEADER_UNITS;
				graph_header = ucache_shared_graph_payload_header(payload_offset);

				if (graph_header != NULL) {
					if (dead_pin_mask != NULL) {
						(void) ucache_graph_pin_strip_payload_locked(graph_header, dead_pin_mask);
					}

					if (zend_atomic_int_load_ex(&graph_header->ref_state) == PHP_UCACHE_SHARED_GRAPH_RETIRED) {
						php_ucache_free_locked(payload_offset);
						reclaimed = true;
						restart = true;

						break;
					}
				}
			}

			offset += block_size;
		}
	} while (restart);

finish:
	efree(live_offsets);

	return reclaimed;
}

static bool ucache_shared_graph_load_root_value(
		const php_ucache_shared_graph_header_t *header,
		size_t buf_len,
		php_ucache_shared_graph_value_t *root_value)
{
	if (header->root_offset != 0 && header->root_offset >= buf_len) {
		return false;
	}

	memset(root_value, 0, sizeof(*root_value));

	root_value->type = (uint8_t) header->root_type;
	root_value->payload.offset = header->root_offset;

	switch (header->root_type) {
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_STRING:
			return true;
		case PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
		case PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return header->root_offset != 0;
		default:
			return false;
	}
}

#if ZEND_DEBUG
static bool ucache_shared_graph_rebase_payload_pointers(
		const uint8_t *buf,
		size_t graph_len,
		const uint8_t *old_base,
		ptrdiff_t delta)
{
	const php_ucache_shared_graph_header_t *header;
	php_ucache_shared_graph_rebase_ctx_t ctx;
	php_ucache_shared_graph_value_t root_value;
	HashTable seen_arrs;
	bool result;

	header = (const php_ucache_shared_graph_header_t *) buf;

	/* This path has not called shared_graph_locate(). */
	if (!ucache_shared_graph_header_is_valid(header) ||
		!ucache_shared_graph_load_root_value(header, graph_len, &root_value)
	) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	ctx.old_base = old_base;
	ctx.new_base = buf;
	ctx.len = graph_len;
	ctx.delta = delta;
	ctx.seen = &seen_arrs;

	result = ucache_shared_graph_rebase_graph_value(&ctx, &root_value);

	zend_hash_destroy(&seen_arrs);

	return result;
}

static void ucache_shared_graph_check_rebase_complete(
		const uint8_t *dst_base,
		size_t graph_len,
		const uint8_t *src_base)
{
	uint8_t *check_buf;
	bool result;

	check_buf = emalloc(graph_len);
	memcpy(check_buf, dst_base, graph_len);

	result = ucache_shared_graph_rebase_payload_pointers(
		dst_base,
		graph_len,
		src_base,
		(ptrdiff_t) (src_base - dst_base)
	);
	ZEND_ASSERT(result);
	ZEND_ASSERT(memcmp(check_buf, dst_base, graph_len) == 0);

	efree(check_buf);
}
#else /* !ZEND_DEBUG */
# define ucache_shared_graph_check_rebase_complete(dst_base, graph_len, src_base)
#endif /* ZEND_DEBUG */

static void ucache_destroy_shared_graph_ref_index(void)
{
	if (UC_G(shared_graph_ref_index) != NULL) {
		zend_hash_destroy(UC_G(shared_graph_ref_index));
		efree(UC_G(shared_graph_ref_index));

		UC_G(shared_graph_ref_index) = NULL;
	}
}

/* The buffer is request memory: leaving it registered would carry a
 * dangling pointer (and a non-zero capacity) into the next request. */
static void ucache_shared_graph_reset_request_refs(void)
{
	if (UC_G(shared_graph_refs) != NULL) {
		efree(UC_G(shared_graph_refs));

		UC_G(shared_graph_refs) = NULL;
	}

	UC_G(shared_graph_ref_count) = 0;
	UC_G(shared_graph_ref_capacity) = 0;

	ucache_destroy_shared_graph_ref_index();
}

static void ucache_shared_graph_refs_check_fork(void)
{
	uint64_t pid = php_ucache_cached_pid();

	if (UC_G(shared_graph_ref_owner_pid) == pid) {
		return;
	}

	if (UC_G(shared_graph_ref_owner_pid) != 0) {
		ucache_shared_graph_reset_request_refs();
	}

	UC_G(shared_graph_ref_owner_pid) = pid;
}

static void ucache_grow_shared_graph_refs(void)
{
	UC_G(shared_graph_ref_capacity) = UC_G(shared_graph_ref_capacity) == 0
		? 8
		: UC_G(shared_graph_ref_capacity) * 2
	;
	UC_G(shared_graph_refs) = erealloc(
		UC_G(shared_graph_refs),
		sizeof(*UC_G(shared_graph_refs)) * UC_G(shared_graph_ref_capacity)
	);
}

static void ucache_ensure_shared_graph_ref_index(void)
{
	if (UC_G(shared_graph_ref_index) == NULL) {
		UC_G(shared_graph_ref_index) = emalloc(sizeof(HashTable));
		zend_hash_init(UC_G(shared_graph_ref_index), 8, NULL, NULL, 0);
	}
}

/* The context disambiguates pools. Mixed because same-sized payloads sit a
 * power-of-two stride apart and zend_hash buckets by the low bits. */
static zend_ulong ucache_shared_graph_ref_index_key(
		const php_ucache_ctx_t *ctx,
		uint32_t payload_offset)
{
	return (zend_ulong) ucache_shared_graph_content_hash_mix((uint64_t) (uintptr_t) ctx, (uint64_t) payload_offset);
}

/* Returns true only when this drops the last reference of an already-retired
 * payload, signaling the caller to free it. */
static bool ucache_shared_graph_release_ref_locked(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header = ucache_shared_graph_payload_header(payload_offset);
	php_ucache_header_t *cache_header = php_ucache_header_ptr();
	int32_t pin_slot = -1;
	bool released = false, bit_cleared = false;
	int state, refcount, expected, desired = 0;

	if (header == NULL) {
		return false;
	}

	/* Clear the owner bit before the refcount: a crash between the two steps
	 * must leave a leaked reference, never a bit the dead-owner sweep would
	 * turn into a second decrement. */
	if (cache_header != NULL) {
		pin_slot = ucache_graph_pin_slot_find(cache_header, false);
		if (pin_slot >= 0) {
			bit_cleared = ucache_graph_pin_bit_clear(header, (uint32_t) pin_slot);
		}
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			break;
		}

		desired = (state & PHP_UCACHE_SHARED_GRAPH_RETIRED) | (refcount - 1);
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, desired)) {
			released = true;

			break;
		}
	}

	if (bit_cleared) {
		ucache_graph_pin_count_add(&cache_header->graph_pin_slots[pin_slot], -1);
	}

	return released &&
		(desired & PHP_UCACHE_SHARED_GRAPH_RETIRED) != 0 &&
		(desired & PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK) == 0
	;
}

static bool ucache_shared_graph_root_array_is_verbatim(
		const HashTable *shared_verdicts,
		HashTable *direct_verdicts,
		const zval *value)
{
	const zval *verdict;

	if (Z_TYPE_P(value) != IS_ARRAY ||
		Z_ARRVAL_P(value)->nNumOfElements == 0 ||
		!ucache_shared_graph_can_use_verbatim_arrays()
	) {
		return false;
	}

	if (GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) {
		return true;
	}

	verdict = shared_verdicts != NULL
		? zend_hash_index_find(shared_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value))
		: NULL
	;
	if (verdict != NULL) {
		return Z_TYPE_P(verdict) == IS_TRUE;
	}

	return ucache_shared_graph_can_copy_verbatim_value(direct_verdicts, value);
}

PHP_UCACHE_DECODE_HOT bool php_ucache_shared_graph_update_object_property(
		zval *obj_zv,
		zend_string *prop_name,
		zval *prop_val)
{
	const char *class_name, *unmangled_name;
	zend_string *cname;
	zend_class_entry *scope;
	zend_object *obj;
	zend_property_info *prop_info;
	HashTable *props;
	size_t unmangled_name_len;
	bool failed;

	obj = Z_OBJ_P(obj_zv);
	if (ucache_shared_graph_is_unmangled_property_name(prop_name)) {
		prop_info = zend_get_property_info(obj->ce, prop_name, true);
		if (ucache_shared_graph_try_update_declared_property(
				obj,
				prop_name,
				prop_info,
				prop_val,
				&failed)
		) {
			return true;
		}

		if (failed) {
			return false;
		}

		if (Z_ISREF_P(prop_val)) {
			props = zend_std_get_properties(obj);

			Z_TRY_ADDREF_P(prop_val);

			zend_hash_update(props, prop_name, prop_val);

			return true;
		}

		scope = obj->ce;
		if (prop_info != NULL &&
			prop_info != ZEND_WRONG_PROPERTY_INFO &&
			prop_info->ce != NULL
		) {
			scope = prop_info->ce;
		}

		zend_update_property(scope, obj, ZSTR_VAL(prop_name), ZSTR_LEN(prop_name), prop_val);

		return !EG(exception);
	}

	if (zend_unmangle_property_name_ex(prop_name, &class_name, &unmangled_name, &unmangled_name_len) == SUCCESS) {
		if (class_name == NULL) {
			zend_update_property(obj->ce, obj, unmangled_name, unmangled_name_len, prop_val);

			return !EG(exception);
		}

		if (class_name[0] != '*') {
			cname = zend_string_init(class_name, strlen(class_name), 0);
			scope = zend_lookup_class(cname);

			if (scope == NULL) {
				zend_string_release_ex(cname, 0);

				return false;
			}

			zend_update_property(scope, obj, unmangled_name, unmangled_name_len, prop_val);
			zend_string_release_ex(cname, 0);
		} else {
			scope = obj->ce;
			prop_info = zend_hash_str_find_ptr(&obj->ce->properties_info, unmangled_name, unmangled_name_len);
			if (prop_info != NULL && prop_info->ce != NULL) {
				scope = prop_info->ce;
			}

			zend_update_property(scope, obj, unmangled_name, unmangled_name_len, prop_val);
		}
	}

	return !EG(exception);
}

PHP_UCACHE_DECODE_HOT bool php_ucache_shared_graph_update_object_property_at(
		zval *obj_zv,
		zend_string *prop_name,
		uint32_t prop_idx,
		zval *prop_val)
{
	zend_object *obj;
	zend_property_info *prop_info;
	bool failed;

	obj = Z_OBJ_P(obj_zv);
	if (ucache_shared_graph_is_unmangled_property_name(prop_name) &&
		obj->ce->type == ZEND_USER_CLASS &&
		obj->ce->properties_info_table != NULL &&
		prop_idx < obj->ce->default_properties_count
	) {
		prop_info = obj->ce->properties_info_table[prop_idx];

		if (ucache_shared_graph_try_update_declared_property(
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
	}

	return php_ucache_shared_graph_update_object_property(
		obj_zv,
		prop_name,
		prop_val
	);
}

void php_ucache_decode_resolve_cache_release(void)
{
	ucache_shared_graph_object_route_memo_release();

	memset((void *) UC_G(decode_resolve_direct_keys), 0, sizeof(UC_G(decode_resolve_direct_keys)));
	memset(UC_G(decode_resolve_direct_values), 0, sizeof(UC_G(decode_resolve_direct_values)));

	UC_G(decode_resolve_direct_next) = 0;

	if (UC_G(decode_resolve_cache) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(decode_resolve_cache));

	efree(UC_G(decode_resolve_cache));

	UC_G(decode_resolve_cache) = NULL;
}

void php_ucache_decode_shape_prototype_cache_release(void)
{
	memset((void *) UC_G(decode_shape_prototype_direct_keys), 0, sizeof(UC_G(decode_shape_prototype_direct_keys)));
	memset(UC_G(decode_shape_prototype_direct_values), 0, sizeof(UC_G(decode_shape_prototype_direct_values)));

	UC_G(decode_shape_prototype_direct_next) = 0;

	if (UC_G(decode_shape_prototype_cache) == NULL) {
		return;
	}

	zend_hash_destroy(UC_G(decode_shape_prototype_cache));
	efree(UC_G(decode_shape_prototype_cache));

	UC_G(decode_shape_prototype_cache) = NULL;
}

/* A bailout may bypass normal decode-map cleanup. */
void php_ucache_decode_maps_teardown(void)
{
	ucache_decode_identity_map_teardown();
	ucache_decode_reference_map_teardown();
	ucache_decode_array_map_teardown();
}

bool php_ucache_shared_graph_prefers_prototype(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header =
		ucache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & PHP_UCACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE) != 0;
}

bool php_ucache_shared_graph_payload_has_aliases(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header =
		ucache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY) != 0;
}

bool php_ucache_shared_graph_decode_is_lock_safe(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header =
		ucache_shared_graph_payload_header(payload_offset)
	;

	return header != NULL &&
		(header->flags & PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_OBJECT) == 0
	;
}

/* Prove verbatim eligibility and compute the root size in one walk. */
php_ucache_verbatim_root_result_t php_ucache_shared_graph_calc_verbatim_root(
		const zval *value,
		php_ucache_verbatim_memo_t *verbatim_memo,
		size_t *buf_len,
		uint32_t *intern_key_count)
{
	php_ucache_shared_graph_calc_ctx_t calc_ctx;
	php_ucache_verbatim_root_result_t result;
	zval verdict_zv;
	bool immutable, completed;

	*intern_key_count = 0;

	if (!ucache_shared_graph_can_use_verbatim_arrays() ||
		Z_ARRVAL_P(value)->nNumOfElements == 0
	) {
		return PHP_UCACHE_VERBATIM_ROOT_UNDECIDED;
	}

	immutable = (GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) != 0;

	ucache_shared_graph_calc_init(&calc_ctx);

	calc_ctx.verbatim_arrays_allowed = true;
	calc_ctx.verbatim_content_hashes = &verbatim_memo->content_hashes;
	calc_ctx.verbatim_canonicals = &verbatim_memo->canonicals;
	calc_ctx.dedup_verbatim_content = true;
	calc_ctx.state_memo = NULL;

	completed = ucache_shared_graph_calc_reserve(
			&calc_ctx,
			sizeof(php_ucache_shared_graph_header_t)
		) &&
		ucache_shared_graph_calc_verbatim_value(&calc_ctx, value, NULL) &&
		ucache_shared_graph_calc_reserve(
			&calc_ctx,
			ucache_shared_graph_intern_list_bytes(calc_ctx.intern_key_count)
		)
	;

	if (completed && calc_ctx.size <= UINT32_MAX - (ZEND_MM_ALIGNMENT - 1)) {
		*buf_len = calc_ctx.size + (ZEND_MM_ALIGNMENT - 1);
		*intern_key_count = calc_ctx.intern_key_count;
		result = PHP_UCACHE_VERBATIM_ROOT_SIZED;
	} else if (immutable || completed) {
		/* Immutable arrays cannot fail verbatim eligibility. */
		result = PHP_UCACHE_VERBATIM_ROOT_ELIGIBLE_UNSIZED;
	} else if (calc_ctx.reserve_failed) {
		result = PHP_UCACHE_VERBATIM_ROOT_UNDECIDED;
	} else {
		result = PHP_UCACHE_VERBATIM_ROOT_INELIGIBLE;
	}

	ucache_shared_graph_calc_destroy(&calc_ctx);

	if (!immutable && result != PHP_UCACHE_VERBATIM_ROOT_UNDECIDED) {
		ZVAL_BOOL(&verdict_zv, result != PHP_UCACHE_VERBATIM_ROOT_INELIGIBLE);
		zend_hash_index_add(&verbatim_memo->verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value), &verdict_zv);
	}

	return result;
}

bool php_ucache_shared_graph_can_copy_verbatim_root(const zval *value, php_ucache_verbatim_memo_t *verbatim_memo)
{
	HashTable direct_verdicts;
	zval verdict_zv;
	bool eligible;

	if (!ucache_shared_graph_can_use_verbatim_arrays()) {
		return false;
	}

	if (GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) {
		return true;
	}

	zend_hash_init(&direct_verdicts, 8, NULL, NULL, 0);

	eligible = ucache_shared_graph_can_copy_verbatim_value(&direct_verdicts, value);

	zend_hash_destroy(&direct_verdicts);

	ZVAL_BOOL(&verdict_zv, eligible);
	zend_hash_index_add(&verbatim_memo->verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value), &verdict_zv);

	return eligible;
}

bool php_ucache_calculate_shared_graph_size(
		const zval *value,
		HashTable *state_memo,
		php_ucache_verbatim_memo_t *verbatim_memo,
		size_t *buf_len,
		uint32_t *intern_key_count)
{
	php_ucache_shared_graph_calc_ctx_t calc_ctx;
	zend_class_entry *root_ce;
	bool result;

	*buf_len = 0;
	*intern_key_count = 0;

	if (Z_TYPE_P(value) == IS_OBJECT) {
		root_ce = Z_OBJCE_P(value);
		if (!(root_ce->ce_flags & ZEND_ACC_ENUM) &&
			ucache_shared_graph_classify_object_route(root_ce) == PHP_UCACHE_OBJECT_ROUTE_UNSTORABLE
		) {
			return false;
		}
	} else if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_STRING) {
		return false;
	}

	ucache_shared_graph_calc_init(&calc_ctx);

	ZEND_ASSERT(verbatim_memo != NULL);

	calc_ctx.verbatim_arrays_allowed = ucache_shared_graph_can_use_verbatim_arrays();
	calc_ctx.shared_verdicts = &verbatim_memo->verdicts;
	calc_ctx.verbatim_content_hashes = &verbatim_memo->content_hashes;
	calc_ctx.verbatim_canonicals = &verbatim_memo->canonicals;
	calc_ctx.dedup_verbatim_content = ucache_shared_graph_root_array_is_verbatim(
		calc_ctx.shared_verdicts,
		&calc_ctx.direct_verdicts,
		value
	);
	calc_ctx.state_memo = state_memo;
	result = ucache_shared_graph_calc_reserve(
		&calc_ctx,
		sizeof(php_ucache_shared_graph_header_t)
	);

	if (result) {
		result = ucache_shared_graph_calc_value(&calc_ctx, value) &&
			ucache_shared_graph_calc_reserve(
				&calc_ctx,
				ucache_shared_graph_intern_list_bytes(calc_ctx.intern_key_count)
			)
		;
	}

	/* Payload-relative string offsets keep bit 31 for interned strings. */
	if (result) {
		if (calc_ctx.size > (size_t) PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG - ZEND_MM_ALIGNMENT) {
			result = false;
		} else {
			calc_ctx.size += ZEND_MM_ALIGNMENT - 1;
		}
	}

	if (result) {
		*buf_len = calc_ctx.size;
		*intern_key_count = calc_ctx.intern_key_count;
	}

	ucache_shared_graph_calc_destroy(&calc_ctx);

	return result;
}

bool php_ucache_build_shared_graph_in_place(
		const zval *value,
		HashTable *state_memo,
		php_ucache_verbatim_memo_t *verbatim_memo,
		uint8_t *buf,
		size_t buf_len,
		size_t *graph_len,
		bool *has_verbatim_array,
		uint32_t **fixup_offsets,
		uint32_t *fixup_count,
		php_ucache_graph_intern_plan_t *intern_plan)
{
	php_ucache_shared_graph_copy_ctx_t copy_ctx;
	php_ucache_shared_graph_header_t *header;
	php_ucache_shared_graph_value_t root_value;
	uint32_t header_offset, root_offset, root_type, intern_list_offset = 0;
	size_t padding, intern_list_bytes;
	bool result;

	padding = ucache_shared_graph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < sizeof(php_ucache_shared_graph_header_t)) {
		return false;
	}

	if (padding != 0) {
		memset(buf, 0, padding);
	}

	buf += padding;
	buf_len -= padding;

	ucache_shared_graph_copy_init(&copy_ctx, buf, buf_len);

	if (verbatim_memo != NULL) {
		copy_ctx.shared_verdicts = &verbatim_memo->verdicts;
		copy_ctx.verbatim_content_hashes = &verbatim_memo->content_hashes;
		copy_ctx.verbatim_canonicals = &verbatim_memo->canonicals;
	}

	copy_ctx.dedup_verbatim_content = ucache_shared_graph_root_array_is_verbatim(
		copy_ctx.shared_verdicts,
		&copy_ctx.direct_verdicts,
		value
	);
	copy_ctx.state_memo = state_memo;
	root_offset = 0;
	root_type = 0;

	result = ucache_shared_graph_copy_alloc(&copy_ctx, sizeof(*header), &header_offset) && header_offset == 0;

	/* The intern list follows the header so its slot capacity is fixed
	 * before any key is copied. */
	if (result && intern_plan != NULL && intern_plan->list_capacity != 0) {
		intern_list_bytes = ucache_shared_graph_intern_list_bytes(intern_plan->list_capacity);
		result = ucache_shared_graph_copy_alloc(&copy_ctx, intern_list_bytes, &intern_list_offset);

		if (result) {
			memset(buf + intern_list_offset, 0, intern_list_bytes);

			intern_plan->list_offset = intern_list_offset;
			intern_plan->enabled = true;
			copy_ctx.intern = intern_plan;
			copy_ctx.intern_list = (uint32_t *) (void *) (buf + intern_list_offset);
		}
	}

	if (result) {
		if (Z_TYPE_P(value) == IS_OBJECT) {
			result = ucache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				(
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_ENUM
				)
			;
		} else if (Z_TYPE_P(value) == IS_ARRAY) {
			result = ucache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				(
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_ARRAY ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY ||
					root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY
				)
			;
		} else if (Z_TYPE_P(value) == IS_STRING) {
			result = ucache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				root_value.type == PHP_UCACHE_SHARED_GRAPH_VALUE_STRING
			;
		} else {
			result = false;
		}

		if (result) {
			root_type = root_value.type;
			root_offset = (uint32_t) root_value.payload.offset;
		}
	}

	if (!result) {
		ucache_shared_graph_copy_destroy(&copy_ctx);

		return false;
	}

	header = (php_ucache_shared_graph_header_t *) buf;
	header->magic = PHP_UCACHE_SHARED_GRAPH_MAGIC;
	header->version = PHP_UCACHE_SHARED_GRAPH_VERSION;
	header->root_offset = root_offset;
	header->root_type = root_type;
	header->flags =
		(copy_ctx.has_shared_identity ? PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY : 0) |
		(copy_ctx.has_object ? PHP_UCACHE_SHARED_GRAPH_FLAG_HAS_OBJECT : 0) |
		((copy_ctx.prefers_prototype && !copy_ctx.has_userland_restore_object) ? PHP_UCACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE : 0)
	;
	header->intern_list_offset = intern_list_offset;
	header->intern_count = copy_ctx.intern != NULL ? copy_ctx.intern->list_count : 0;

	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);
	ucache_shared_graph_pin_owners_init(header);

	if (graph_len != NULL) {
		*graph_len = copy_ctx.position;
	}

	if (has_verbatim_array != NULL) {
		*has_verbatim_array = copy_ctx.has_verbatim_array;
	}

	ZEND_ASSERT(copy_ctx.has_verbatim_array || copy_ctx.fixup_count == 0);

	if (fixup_offsets != NULL) {
		*fixup_offsets = copy_ctx.fixup_offsets;
		*fixup_count = copy_ctx.fixup_count;

		copy_ctx.fixup_offsets = NULL;
		copy_ctx.fixup_count = 0;
		copy_ctx.fixup_capacity = 0;
	}

	ucache_shared_graph_copy_destroy(&copy_ctx);

	return true;
}

bool php_ucache_shared_graph_copy_fits_buffer(
		const uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_graph_len)
{
	size_t dst_padding;

	if (src_buf == NULL || dst_buf == NULL || src_graph_len == 0) {
		return false;
	}

	dst_padding = ucache_shared_graph_alignment_padding(dst_buf);

	return dst_padding <= buf_len && src_graph_len <= buf_len - dst_padding;
}

PHP_UCACHE_DECODE_HOT bool php_ucache_shared_graph_decode(
		const uint8_t *buf,
		size_t buf_len,
		zval *dst)
{
	const php_ucache_shared_graph_header_t *header;
	const php_ucache_header_t *cache_header;
	const uint8_t *graph_buf, *saved_segment_base;
	php_ucache_shared_graph_value_t root_value;
	HashTable *saved_identity_map, *saved_reference_map, *saved_array_map;
	size_t saved_segment_len;
	bool result;

	graph_buf = ucache_shared_graph_locate(
		buf,
		buf_len,
		&buf_len
	);
	if (graph_buf == NULL) {
		return false;
	}

	buf = graph_buf;

	/* Interned strings resolve against the segment of this payload; a
	 * re-entrant fetch may decode from another segment, so save and
	 * restore like the decode maps below. */
	saved_segment_base = UC_G(decode_segment_base);
	saved_segment_len = UC_G(decode_segment_len);
	cache_header = php_ucache_header_ptr();
	if (cache_header != NULL) {
		UC_G(decode_segment_base) = (const uint8_t *) cache_header;
		UC_G(decode_segment_len) = php_ucache_shm_bytes(cache_header->data_offset)
			+ php_ucache_shm_bytes(cache_header->data_size)
		;
	} else {
		UC_G(decode_segment_base) = NULL;
		UC_G(decode_segment_len) = 0;
	}

	header = (const php_ucache_shared_graph_header_t *) buf;
	if (!ucache_shared_graph_load_root_value(header, buf_len, &root_value)) {
		return false;
	}

	/* Restore hooks may re-enter fetch. */
	saved_identity_map = UC_G(decode_identity_map);
	UC_G(decode_identity_map) = NULL;

	saved_reference_map = UC_G(decode_reference_map);
	UC_G(decode_reference_map) = NULL;

	saved_array_map = UC_G(decode_array_map);
	UC_G(decode_array_map) = NULL;

	result = ucache_shared_graph_decode_value(buf, buf_len, &root_value, dst);

	php_ucache_decode_maps_teardown();

	UC_G(decode_identity_map) = saved_identity_map;
	UC_G(decode_reference_map) = saved_reference_map;
	UC_G(decode_array_map) = saved_array_map;
	UC_G(decode_segment_base) = saved_segment_base;
	UC_G(decode_segment_len) = saved_segment_len;

	/* Drop address-keyed caches after releasing a failed payload. */
	if (!result) {
		ucache_decode_fail_zval(dst);

		php_ucache_decode_resolve_cache_release();
		php_ucache_decode_shape_prototype_cache_release();
	}

	return result;
}

/* Cheap advisory probe for eviction victim scans: a payload with live
 * references would be retired instead of freed, so evicting its entry
 * reclaims no space (and the references mean it is in active use anyway). */
bool php_ucache_shared_graph_payload_has_refs_locked(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header =
		ucache_shared_graph_payload_header(payload_offset)
	;

	return header != NULL && zend_atomic_int_load_ex(&header->ref_state) != 0;
}

bool php_ucache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header =
		ucache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return false;
	}

	if (!php_ucache_quiesce_graph_payloads_locked()) {
		return false;
	}

	return zend_atomic_int_load_ex(&header->ref_state) == 0;
}

static void ucache_shared_graph_publish_interns_locked(
		uint8_t *dst_base,
		php_ucache_shared_graph_header_t *header,
		php_ucache_graph_intern_plan_t *plan)
{
	php_ucache_header_t *cache_header = php_ucache_header_ptr();
	php_ucache_graph_intern_site_t *site;
	uint32_t *list, i, seg_offset;
	bool swept = false;

	list = (uint32_t *) (void *) (dst_base + plan->list_offset);

	for (i = 0; i < plan->candidate_count; i++) {
		seg_offset = php_ucache_intern_add_locked((zend_string *) (void *) (dst_base + plan->candidates[i]));

		/* The first insert to hit the load limit sweeps once; a table that
		 * stays full afterwards refuses inserts until the next sweep. */
		if (seg_offset == 0 && !swept && cache_header != NULL && cache_header->intern_saturated == 1) {
			swept = true;

			(void) php_ucache_shared_graph_intern_sweep_locked();

			if (cache_header->intern_saturated == 0) {
				seg_offset = php_ucache_intern_add_locked((zend_string *) (void *) (dst_base + plan->candidates[i]));
			} else {
				cache_header->intern_saturated = 2;
			}
		}

		plan->candidates[i] = seg_offset;

		/* Keep the header count current: a sweep triggered by a later
		 * candidate must already see this reference. */
		if (seg_offset != 0 && plan->list_count < plan->list_capacity) {
			list[plan->list_count++] = seg_offset;
			header->intern_count = plan->list_count;
		}
	}

	for (i = 0; i < plan->site_count; i++) {
		site = &plan->sites[i];
		seg_offset = plan->candidates[site->candidate];

		if (seg_offset == 0) {
			continue;
		}

		if (site->pointer) {
			*(zend_string **) (void *) (dst_base + site->site_offset) =
				(zend_string *) php_ucache_ptr(seg_offset)
			;
		} else {
			*(uint32_t *) (void *) (dst_base + site->site_offset) =
				seg_offset | PHP_UCACHE_GRAPH_SEGMENT_STRING_FLAG
			;
		}
	}

	header->intern_count = plan->list_count;
}

php_ucache_publish_result_t php_ucache_shared_graph_publish_copied_payload_locked(
		uint8_t *dst_buf,
		const uint8_t *src_buf,
		size_t buf_len,
		size_t src_graph_len,
		bool has_verbatim_array,
		const uint32_t *fixup_offsets,
		uint32_t fixup_count,
		php_ucache_graph_intern_plan_t *intern_plan)
{
	const uint8_t *src_base;
	php_ucache_shared_graph_header_t *header;
	php_ucache_header_t *cache_header;
	uint8_t *dst_base;
	uintptr_t delta;
	size_t src_padding, dst_padding;
	uint32_t i;

	if (!php_ucache_shared_graph_copy_fits_buffer(dst_buf, src_buf, buf_len, src_graph_len)) {
		return PHP_UCACHE_PUBLISH_FAILED;
	}

	src_padding = ucache_shared_graph_alignment_padding(src_buf);
	if (src_padding > buf_len ||
		buf_len - src_padding < src_graph_len ||
		src_graph_len < sizeof(*header)
	) {
		return PHP_UCACHE_PUBLISH_FAILED;
	}

	/* Resolved interns are only valid for the generation they were looked
	 * up in; a sweep since then means the payload must be rebuilt. */
	if (intern_plan != NULL && intern_plan->enabled && intern_plan->list_count != 0) {
		cache_header = php_ucache_header_ptr();

		if (cache_header == NULL ||
			intern_plan->generation_conflict ||
			cache_header->intern_generation != intern_plan->generation
		) {
			return PHP_UCACHE_PUBLISH_STALE_INTERNS;
		}
	}

	src_base = src_buf + src_padding;
	dst_padding = ucache_shared_graph_alignment_padding(dst_buf);
	if (dst_padding != 0) {
		memset(dst_buf, 0, dst_padding);
	}

	dst_base = dst_buf + dst_padding;

	if (src_base != dst_base) {
		memcpy(dst_base, src_base, src_graph_len);
	}

	header = (php_ucache_shared_graph_header_t *) dst_base;
	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);

	ucache_shared_graph_pin_owners_init(header);

	if (src_base == dst_base) {
		return PHP_UCACHE_PUBLISH_DONE;
	}

	/* Only non-empty verbatim arrays contain absolute pointers. */
	if (has_verbatim_array) {
		ZEND_ASSERT(fixup_offsets != NULL && fixup_count > 0);

		delta = (uintptr_t) dst_base - (uintptr_t) src_base;
		for (i = 0; i < fixup_count; i++) {
			ZEND_ASSERT(fixup_offsets[i] <= src_graph_len - sizeof(uintptr_t));
			ZEND_ASSERT((((uintptr_t) dst_base + fixup_offsets[i]) & (sizeof(uintptr_t) - 1)) == 0);

			*(uintptr_t *) (void *) (dst_base + fixup_offsets[i]) += delta;
		}
	} else {
		ZEND_ASSERT(fixup_count == 0);
	}

	ucache_shared_graph_check_rebase_complete(dst_base, src_graph_len, src_base);

	if (intern_plan != NULL && intern_plan->enabled) {
		ucache_shared_graph_publish_interns_locked(dst_base, header, intern_plan);
	}

	return PHP_UCACHE_PUBLISH_DONE;
}

void php_ucache_graph_intern_plan_destroy(php_ucache_graph_intern_plan_t *plan)
{
	if (plan->candidates != NULL) {
		efree(plan->candidates);
	}

	if (plan->sites != NULL) {
		efree(plan->sites);
	}

	memset(plan, 0, sizeof(*plan));
}

void php_ucache_shared_graph_orphan_payload_locked(uint32_t payload_offset)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	uint32_t i;

	if (header == NULL || payload_offset == 0) {
		return;
	}

	ucache_shared_graph_force_retire_locked(payload_offset);

	for (i = 0; i < PHP_UCACHE_ORPHANED_GRAPH_SLOTS; i++) {
		if (header->orphaned_graphs[i] == payload_offset) {
			return;
		}

		if (header->orphaned_graphs[i] == 0) {
			header->orphaned_graphs[i] = payload_offset;

			return;
		}
	}

	header->orphaned_graphs_saturated = 1;
}

void php_ucache_shared_graph_reclaim_orphaned_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_shared_graph_header_t *graph_header;
	uint32_t i, payload_offset;
	bool checked_quiescent = false;

	if (header == NULL) {
		return;
	}

	for (i = 0; i < PHP_UCACHE_ORPHANED_GRAPH_SLOTS; i++) {
		payload_offset = header->orphaned_graphs[i];
		if (payload_offset == 0) {
			continue;
		}

		if (!checked_quiescent) {
			if (!php_ucache_quiesce_graph_payloads_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		graph_header = ucache_shared_graph_payload_header(payload_offset);
		header->orphaned_graphs[i] = 0;

		if (graph_header == NULL) {
			continue;
		}

		if (zend_atomic_int_load_ex(&graph_header->ref_state) !=
			PHP_UCACHE_SHARED_GRAPH_RETIRED
		) {
			continue;
		}

		php_ucache_free_locked(payload_offset);
	}

	if (header->orphaned_graphs_saturated != 0) {
		if (!checked_quiescent && !php_ucache_quiesce_graph_payloads_locked()) {
			return;
		}

		ucache_shared_graph_reclaim_orphaned_by_scan_locked(header, NULL);

		header->orphaned_graphs_saturated = 0;
	}
}

/* Strip shared-graph pins left behind by abnormally terminated owners so
 * their retired payloads can reach refcount zero and be reclaimed. Requires
 * the write lock. force runs the liveness probes unconditionally (reset,
 * clear, allocation pressure); otherwise they are rate-limited per process.
 * Returns true when payload blocks were freed. */
bool php_ucache_shared_graph_strip_dead_pins_locked(bool force)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_graph_pin_slot_t *slot;
	php_ucache_entry_t *entries, *entry;
	php_ucache_shared_graph_header_t *graph_header;
	uint64_t now, owner_pid;
	uint32_t i, dead_mask[PHP_UCACHE_GRAPH_PIN_WORDS];
	int value;
	bool any_pinned = false, any_dead = false;

	if (header == NULL || !php_ucache_header_is_initialized_locked()) {
		return false;
	}

	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		slot = &header->graph_pin_slots[i];

		if (zend_atomic_int_load_ex(&slot->owner_pid) != 0 &&
			zend_atomic_int_load_ex(&slot->pin_count) != 0
		) {
			any_pinned = true;

			break;
		}
	}

	if (!any_pinned) {
		return false;
	}

	if (!force) {
		now = (uint64_t) time(NULL);

		if (UC_G(graph_pin_probe_last_at) != 0 &&
			now - UC_G(graph_pin_probe_last_at) < PHP_UCACHE_GRAPH_PIN_PROBE_INTERVAL_SEC
		) {
			return false;
		}

		UC_G(graph_pin_probe_last_at) = now;
	}

	memset(dead_mask, 0, sizeof(dead_mask));

	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		slot = &header->graph_pin_slots[i];

		value = zend_atomic_int_load_ex(&slot->owner_pid);
		if (value == 0 ||
			value == PHP_UCACHE_GRAPH_PIN_OWNER_RECLAIMING ||
			zend_atomic_int_load_ex(&slot->pin_count) == 0
		) {
			continue;
		}

		owner_pid = (uint64_t) (uint32_t) value;
		if (owner_pid == php_ucache_cached_pid()) {
			continue;
		}

		if (php_ucache_graph_pin_owner_is_dead(owner_pid, php_ucache_atomic_load_64(&slot->owner_start_time))) {
			dead_mask[i / 32U] |= 1U << (i % 32U);
			any_dead = true;
		}
	}

	if (!any_dead) {
		return false;
	}

	if (!php_ucache_quiesce_graph_payloads_locked()) {
		return false;
	}

	/* Attached payloads first: an owner can die while the entry is still
	 * stored, and those blocks are invisible to the orphan scan below. */
	entries = php_ucache_entries_ptr(header);
	for (i = php_ucache_occupancy_next_used(header, 0);
		i != UINT32_MAX;
		i = php_ucache_occupancy_next_used(header, i + 1)
	) {
		entry = &entries[i];

		if (entry->value_type != PHP_UCACHE_VALUE_SHARED_GRAPH ||
			entry->value_offset == 0
		) {
			continue;
		}

		graph_header = ucache_shared_graph_payload_header(entry->value_offset);
		if (graph_header != NULL) {
			(void) ucache_graph_pin_strip_payload_locked(graph_header, dead_mask);
		}
	}

	/* Detached (retired or orphaned) payloads, freeing whatever reaches
	 * refcount zero. */
	if (!ucache_shared_graph_reclaim_orphaned_by_scan_locked(header, dead_mask)) {
		any_dead = false;
	}

	/* Release the dead slots, clearing the pid last so a concurrent claimer
	 * can only take a fully cleaned slot. */
	for (i = 0; i < PHP_UCACHE_GRAPH_PIN_SLOTS; i++) {
		if ((dead_mask[i / 32U] & (1U << (i % 32U))) == 0) {
			continue;
		}

		slot = &header->graph_pin_slots[i];

		zend_atomic_int_store_ex(&slot->pin_count, 0);
		php_ucache_atomic_store_64(&slot->owner_start_time, 0);
		zend_atomic_int_store_ex(&slot->owner_pid, 0);

		header->graph_dead_pin_owners_reclaimed++;
	}

	return any_dead;
}

/* Mark-sweep over the intern table: every allocated block that carries a
 * graph header marks the strings its list references; the rest are freed
 * and the table is rebuilt in a new generation. */
bool php_ucache_shared_graph_intern_sweep_locked(void)
{
	php_ucache_header_t *header = php_ucache_header_ptr();
	php_ucache_intern_slot_t *slots;
	php_ucache_block_t *block;
	php_ucache_shared_graph_header_t *graph_header;
	const uint8_t *graph_buf;
	zend_ulong *marks;
	uint32_t *survivors, i, used_end, offset, block_size, payload_offset, survivor_count = 0;
	size_t graph_len;
	bool freed = false;

	if (header == NULL ||
		!php_ucache_header_is_initialized_locked() ||
		header->intern_count == 0
	) {
		return false;
	}

	/* Plain allocations: a bailout here would longjmp with the write lock
	 * held. Skipping the sweep is always safe. */
	marks = calloc(PHP_UCACHE_OCCUPANCY_WORDS(header->intern_capacity), sizeof(*marks));
	if (marks == NULL) {
		return false;
	}

	survivors = malloc((size_t) header->intern_count * sizeof(*survivors));
	if (survivors == NULL) {
		free(marks);

		return false;
	}

	used_end = header->data_offset + header->next_free;
	offset = header->data_offset;

	while (offset < used_end) {
		block = php_ucache_block_ptr(offset);
		block_size = block->size;

		if (block_size < php_ucache_shm_units(PHP_UCACHE_ALIGNED_SIZE(sizeof(php_ucache_block_t) + 1)) ||
			block_size > used_end - offset
		) {
			break;
		}

		if (!php_ucache_block_is_free(block)) {
			payload_offset = offset + PHP_UCACHE_BLOCK_HEADER_UNITS;
			graph_buf = ucache_shared_graph_locate(
				php_ucache_ptr(payload_offset),
				php_ucache_block_payload_capacity(payload_offset),
				&graph_len
			);

			if (graph_buf != NULL) {
				graph_header = (php_ucache_shared_graph_header_t *) graph_buf;

				ucache_shared_graph_intern_mark_payload_locked(header, graph_header, graph_len, marks);
			}
		}

		offset += block_size;
	}

	slots = php_ucache_intern_slots_ptr(header);
	for (i = 0; i < header->intern_capacity; i++) {
		if (slots[i].str_offset == 0) {
			continue;
		}

		if ((marks[i / PHP_UCACHE_OCCUPANCY_WORD_BITS] & ((zend_ulong) 1 << (i % PHP_UCACHE_OCCUPANCY_WORD_BITS))) != 0) {
			if (survivor_count < header->intern_count) {
				survivors[survivor_count++] = slots[i].str_offset;
			}
		} else {
			php_ucache_free_locked(slots[i].str_offset);
			freed = true;
		}
	}

	php_ucache_intern_table_reset_locked();

	for (i = 0; i < survivor_count; i++) {
		php_ucache_intern_table_insert_locked(survivors[i]);
	}

	header->intern_sweep_count++;

	free(survivors);
	free(marks);

	return freed;
}

bool php_ucache_shared_graph_acquire_ref(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header = ucache_shared_graph_payload_header(payload_offset);
	php_ucache_header_t *cache_header = php_ucache_header_ptr();
	int32_t pin_slot = -1;
	int state, refcount, expected;

	if (header == NULL) {
		return false;
	}

	/* Record ownership before the reference is visible: every crash window
	 * below then errs toward an inflated pin_count or an unattributed (never
	 * a double-stripped) reference. */
	if (cache_header != NULL) {
		pin_slot = ucache_graph_pin_slot_find(cache_header, true);
		if (pin_slot >= 0) {
			ucache_graph_pin_count_add(&cache_header->graph_pin_slots[pin_slot], 1);
		}
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if ((state & PHP_UCACHE_SHARED_GRAPH_RETIRED) != 0 ||
			refcount == PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK
		) {
			if (pin_slot >= 0) {
				ucache_graph_pin_count_add(&cache_header->graph_pin_slots[pin_slot], -1);
			}

			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state + 1)) {
			break;
		}
	}

	if (pin_slot >= 0) {
		ucache_graph_pin_bit_set(header, (uint32_t) pin_slot);
	}

	return true;
}

/* Owners that terminate abnormally while holding references leave retired
 * payloads pinned; php_ucache_shared_graph_strip_dead_pins_locked() strips
 * such pins via the per-owner records so the payloads become reclaimable. */
bool php_ucache_shared_graph_retire_payload_locked(uint32_t payload_offset)
{
	php_ucache_shared_graph_header_t *header = ucache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return true;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_UCACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return true;
		}

		if ((state & PHP_UCACHE_SHARED_GRAPH_RETIRED) != 0) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(
				&header->ref_state,
				&expected,
				(state | PHP_UCACHE_SHARED_GRAPH_RETIRED)
			)
		) {
			return false;
		}
	}
}

bool php_ucache_has_request_shared_graph_ref(uint32_t payload_offset)
{
	php_ucache_ctx_t *ctx;
	zval *index_zv;
	uint32_t i;

	ucache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == 0) {
		return false;
	}

	ctx = php_ucache_active_context();

	/* A non-zero ref_count implies the index exists: both are reset
	 * together by ucache_shared_graph_reset_request_refs. */
	ZEND_ASSERT(UC_G(shared_graph_ref_index) != NULL);

	index_zv = zend_hash_index_find(
		UC_G(shared_graph_ref_index),
		ucache_shared_graph_ref_index_key(ctx, payload_offset)
	);
	if (index_zv == NULL) {
		return false;
	}

	i = (uint32_t) Z_LVAL_P(index_zv);
	if (i < UC_G(shared_graph_ref_count) &&
		UC_G(shared_graph_refs)[i].ctx == ctx &&
		UC_G(shared_graph_refs)[i].payload_offset == payload_offset
	) {
		return true;
	}

	for (i = 0; i < UC_G(shared_graph_ref_count); i++) {
		if (UC_G(shared_graph_refs)[i].ctx == ctx &&
			UC_G(shared_graph_refs)[i].payload_offset == payload_offset
		) {
			return true;
		}
	}

	return false;
}

void php_ucache_shared_graph_ref_reserve(void)
{
	ucache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == UC_G(shared_graph_ref_capacity)) {
		ucache_grow_shared_graph_refs();
	}

	ucache_ensure_shared_graph_ref_index();

	zend_hash_extend(
		UC_G(shared_graph_ref_index),
		zend_hash_num_elements(UC_G(shared_graph_ref_index)) + 1,
		0
	);
}

void php_ucache_register_shared_graph_ref(uint32_t payload_offset)
{
	php_ucache_ctx_t *ctx;
	zval index_zv;

	ucache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == UC_G(shared_graph_ref_capacity)) {
		ucache_grow_shared_graph_refs();
	}

	ctx = php_ucache_active_context();

	UC_G(shared_graph_refs)[UC_G(shared_graph_ref_count)].ctx = ctx;
	UC_G(shared_graph_refs)[UC_G(shared_graph_ref_count)].payload_offset = payload_offset;

	ucache_ensure_shared_graph_ref_index();

	ZVAL_LONG(&index_zv, (zend_long) UC_G(shared_graph_ref_count));

	zend_hash_index_add(
		UC_G(shared_graph_ref_index),
		ucache_shared_graph_ref_index_key(ctx, payload_offset),
		&index_zv
	);

	UC_G(shared_graph_ref_count)++;
}

bool php_ucache_release_request_shared_graph_refs(void)
{
	php_ucache_shared_graph_ref_t *ref;
	php_ucache_ctx_t *ctx, *prev_ctx;
	uint32_t i, inner;
	bool released = false, write_section_entered;

	ucache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == 0) {
		ucache_shared_graph_reset_request_refs();

		return false;
	}

	for (i = 0; i < UC_G(shared_graph_ref_count); i++) {
		ctx = UC_G(shared_graph_refs)[i].ctx;

		if (ctx == NULL) {
			continue;
		}

		prev_ctx = php_ucache_activate_context(ctx);

		if (php_ucache_wlock_for_ref_release(&write_section_entered)) {
			if (!write_section_entered) {
				for (inner = i; inner < UC_G(shared_graph_ref_count); inner++) {
					ref = &UC_G(shared_graph_refs)[inner];

					if (ref->ctx != ctx) {
						continue;
					}

					if (ref->payload_offset != 0) {
						released = true;
						(void) ucache_shared_graph_release_ref_locked(ref->payload_offset);
					}

					ref->ctx = NULL;
				}
			} else if (php_ucache_header_is_initialized_locked()) {
				for (inner = i; inner < UC_G(shared_graph_ref_count); inner++) {
					ref = &UC_G(shared_graph_refs)[inner];

					if (ref->ctx != ctx) {
						continue;
					}

					if (ref->payload_offset == 0) {
						ref->ctx = NULL;

						continue;
					}

					released = true;
					if (ucache_shared_graph_release_ref_locked(ref->payload_offset)) {
						if (php_ucache_quiesce_graph_payloads_locked()) {
							php_ucache_free_locked(ref->payload_offset);
						} else {
							php_ucache_shared_graph_orphan_payload_locked(ref->payload_offset);
						}
					}

					ref->ctx = NULL;
				}

				php_ucache_shared_graph_reclaim_orphaned_locked();
				(void) php_ucache_shared_graph_strip_dead_pins_locked(false);
			}

			php_ucache_unlock();
		}

		php_ucache_restore_context(prev_ctx);
	}

	ucache_shared_graph_reset_request_refs();

	return released;
}
